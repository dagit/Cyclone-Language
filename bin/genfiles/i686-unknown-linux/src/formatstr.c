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
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
unsigned char Cyc_List_Nth[8];extern int isdigit(int);extern unsigned int Cyc_Std_strlen(
struct _tagged_arr s);extern struct _tagged_arr Cyc_Std_implode(struct Cyc_List_List*c);
struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];
struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{
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
15];extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();extern void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uchar_t;
extern void*Cyc_Absyn_ushort_t;extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_ulong_t;
extern void*Cyc_Absyn_schar_t;extern void*Cyc_Absyn_sshort_t;extern void*Cyc_Absyn_sint_t;
extern void*Cyc_Absyn_slong_t;extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_double_typ(
int);extern void*Cyc_Absyn_string_typ(void*rgn);extern void*Cyc_Absyn_const_string_typ(
void*rgn);extern void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);
struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_StructRes_struct{
int tag;struct Cyc_Absyn_Structdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*uniondecls;struct
Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};extern struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
extern unsigned char Cyc_Tcutil_TypeErr[12];extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern struct Cyc_List_List*Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);extern
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*,struct
_tagged_arr,struct Cyc_Position_Segment*);extern struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(
struct _tagged_arr s,int i);static struct Cyc_Core_Opt Cyc_Formatstr_rk={(void*)((void*)
3)};struct _tuple3{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;unsigned char f5;int f6;};struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(
struct _tagged_arr s,int i){unsigned int _tmp0=Cyc_Std_strlen(s);if(i < 0? 1: i >= _tmp0){
return 0;}{struct Cyc_List_List*_tmp1=0;unsigned char c=' ';for(0;i < _tmp0;i ++){c=((
const unsigned char*)s.curr)[i];switch(c){case '+': _LL0: goto _LL1;case '-': _LL1: goto
_LL2;case ' ': _LL2: goto _LL3;case '#': _LL3: goto _LL4;case '0': _LL4: _tmp1=({struct Cyc_List_List*
_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=(void*)((int)c);_tmp2->tl=_tmp1;_tmp2;});
continue;default: _LL5: break;}break;}if(i >= _tmp0){return 0;}_tmp1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1);{struct Cyc_List_List*_tmp3=0;c=((
const unsigned char*)s.curr)[i];if(c == '*'){_tmp3=({struct Cyc_List_List*_tmp4=
_cycalloc(sizeof(*_tmp4));_tmp4->hd=(void*)((int)c);_tmp4->tl=_tmp3;_tmp4;});i ++;}
else{for(0;i < _tmp0;i ++){c=((const unsigned char*)s.curr)[i];if(isdigit((int)c)){
_tmp3=({struct Cyc_List_List*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->hd=(void*)((
int)c);_tmp5->tl=_tmp3;_tmp5;});}else{break;}}}if(i >= _tmp0){return 0;}_tmp3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3);{struct
Cyc_List_List*_tmp6=0;c=((const unsigned char*)s.curr)[i];if(c == '.'){_tmp6=({
struct Cyc_List_List*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->hd=(void*)((int)c);
_tmp7->tl=_tmp6;_tmp7;});i ++;if(i >= _tmp0){return 0;}c=((const unsigned char*)s.curr)[
i];if(c == '*'){_tmp6=({struct Cyc_List_List*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8->hd=(
void*)((int)c);_tmp8->tl=_tmp6;_tmp8;});i ++;}else{for(0;i < _tmp0;i ++){c=((const
unsigned char*)s.curr)[i];if(isdigit((int)c)){_tmp6=({struct Cyc_List_List*_tmp9=
_cycalloc(sizeof(*_tmp9));_tmp9->hd=(void*)((int)c);_tmp9->tl=_tmp6;_tmp9;});}
else{break;}}}}if(i >= _tmp0){return 0;}_tmp6=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp6);{struct Cyc_List_List*_tmpA=0;c=((const unsigned char*)
s.curr)[i];switch(c){case 'h': _LL7: _tmpA=({struct Cyc_List_List*_tmpB=_cycalloc(
sizeof(*_tmpB));_tmpB->hd=(void*)((int)c);_tmpB->tl=_tmpA;_tmpB;});i ++;if(i >= 
_tmp0){return 0;}c=((const unsigned char*)s.curr)[i];if(c == 'h'){_tmpA=({struct Cyc_List_List*
_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC->hd=(void*)((int)c);_tmpC->tl=_tmpA;_tmpC;});
i ++;}break;case 'l': _LL8: _tmpA=({struct Cyc_List_List*_tmpD=_cycalloc(sizeof(*
_tmpD));_tmpD->hd=(void*)((int)c);_tmpD->tl=_tmpA;_tmpD;});i ++;if(i >= _tmp0){
return 0;}c=((const unsigned char*)s.curr)[i];if(c == 'l'){_tmpA=({struct Cyc_List_List*
_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=(void*)((int)c);_tmpE->tl=_tmpA;_tmpE;});
i ++;}break;case 'j': _LL9: goto _LLA;case 'z': _LLA: goto _LLB;case 't': _LLB: goto _LLC;
case 'L': _LLC: _tmpA=({struct Cyc_List_List*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->hd=(
void*)((int)c);_tmpF->tl=_tmpA;_tmpF;});i ++;break;default: _LLD: break;}if(i >= 
_tmp0){return 0;}_tmpA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpA);c=((const unsigned char*)s.curr)[i];switch(c){case 'd': _LLF: goto _LL10;case 'i':
_LL10: goto _LL11;case 'o': _LL11: goto _LL12;case 'u': _LL12: goto _LL13;case 'x': _LL13:
goto _LL14;case 'X': _LL14: goto _LL15;case 'f': _LL15: goto _LL16;case 'F': _LL16: goto
_LL17;case 'e': _LL17: goto _LL18;case 'E': _LL18: goto _LL19;case 'g': _LL19: goto _LL1A;
case 'G': _LL1A: goto _LL1B;case 'a': _LL1B: goto _LL1C;case 'A': _LL1C: goto _LL1D;case 'c':
_LL1D: goto _LL1E;case 's': _LL1E: goto _LL1F;case 'p': _LL1F: goto _LL20;case 'n': _LL20:
goto _LL21;case '%': _LL21: break;default: _LL22: return 0;}return({struct Cyc_Core_Opt*
_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->v=({struct _tuple3*_tmp11=_cycalloc(
sizeof(*_tmp11));_tmp11->f1=_tmp1;_tmp11->f2=_tmp3;_tmp11->f3=_tmp6;_tmp11->f4=
_tmpA;_tmp11->f5=c;_tmp11->f6=i + 1;_tmp11;});_tmp10;});}}}}}struct Cyc_List_List*
Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*te,struct _tagged_arr s,struct
Cyc_Position_Segment*loc){unsigned int _tmp12=Cyc_Std_strlen(s);struct Cyc_List_List*
_tmp13=0;int i;for(i=0;i < _tmp12;i ++){if(((const unsigned char*)s.curr)[i]!= '%'){
continue;}{struct Cyc_Core_Opt*_tmp14=Cyc_Formatstr_parse_conversionspecification(
s,i + 1);if(_tmp14 == 0){({void*_tmp15[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format string",
sizeof(unsigned char),18),_tag_arr(_tmp15,sizeof(void*),0));});return 0;}{struct
_tuple3 _tmp17;int _tmp18;unsigned char _tmp19;struct Cyc_List_List*_tmp1A;struct Cyc_List_List*
_tmp1B;struct Cyc_List_List*_tmp1C;struct Cyc_List_List*_tmp1D;struct _tuple3*
_tmp16=(struct _tuple3*)_tmp14->v;_tmp17=*_tmp16;_LL2A: _tmp1D=_tmp17.f1;goto _LL29;
_LL29: _tmp1C=_tmp17.f2;goto _LL28;_LL28: _tmp1B=_tmp17.f3;goto _LL27;_LL27: _tmp1A=
_tmp17.f4;goto _LL26;_LL26: _tmp19=_tmp17.f5;goto _LL25;_LL25: _tmp18=_tmp17.f6;goto
_LL24;_LL24: i=_tmp18 - 1;{struct Cyc_List_List*_tmp1E=_tmp1A;struct Cyc_List_List
_tmp1F;struct Cyc_List_List*_tmp20;int _tmp21;_LL2C: if(_tmp1E == 0){goto _LL2E;}
else{_tmp1F=*_tmp1E;_LL32: _tmp21=(int)_tmp1F.hd;goto _LL31;_LL31: _tmp20=_tmp1F.tl;
if(_tmp20 == 0){goto _LL30;}else{goto _LL2E;}}_LL30: if((_tmp21 == 'j'? 1: _tmp21 == 'z')?
1: _tmp21 == 't'){goto _LL2D;}else{goto _LL2E;}_LL2E: goto _LL2F;_LL2D:({struct Cyc_Std_Int_pa_struct
_tmp23;_tmp23.tag=1;_tmp23.f1=(int)((unsigned int)_tmp21);{void*_tmp22[1]={&
_tmp23};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%c' is not supported",
sizeof(unsigned char),38),_tag_arr(_tmp22,sizeof(void*),1));}});return 0;_LL2F:
goto _LL2B;_LL2B:;}{struct Cyc_List_List*_tmp24=_tmp1C;struct Cyc_List_List _tmp25;
struct Cyc_List_List*_tmp26;int _tmp27;_LL34: if(_tmp24 == 0){goto _LL36;}else{_tmp25=*
_tmp24;_LL3A: _tmp27=(int)_tmp25.hd;goto _LL39;_LL39: _tmp26=_tmp25.tl;if(_tmp26 == 
0){goto _LL38;}else{goto _LL36;}}_LL38: if(_tmp27 == '*'){goto _LL35;}else{goto _LL36;}
_LL36: goto _LL37;_LL35: _tmp13=({struct Cyc_List_List*_tmp28=_cycalloc(sizeof(*
_tmp28));_tmp28->hd=(void*)Cyc_Absyn_sint_t;_tmp28->tl=_tmp13;_tmp28;});goto
_LL33;_LL37: goto _LL33;_LL33:;}{struct Cyc_List_List*_tmp29=_tmp1B;struct Cyc_List_List
_tmp2A;struct Cyc_List_List*_tmp2B;struct Cyc_List_List _tmp2C;struct Cyc_List_List*
_tmp2D;int _tmp2E;int _tmp2F;_LL3C: if(_tmp29 == 0){goto _LL3E;}else{_tmp2A=*_tmp29;
_LL44: _tmp2F=(int)_tmp2A.hd;goto _LL41;_LL41: _tmp2B=_tmp2A.tl;if(_tmp2B == 0){goto
_LL3E;}else{_tmp2C=*_tmp2B;_LL43: _tmp2E=(int)_tmp2C.hd;goto _LL42;_LL42: _tmp2D=
_tmp2C.tl;if(_tmp2D == 0){goto _LL40;}else{goto _LL3E;}}}_LL40: if(_tmp2F == '.'?
_tmp2E == '*': 0){goto _LL3D;}else{goto _LL3E;}_LL3E: goto _LL3F;_LL3D: _tmp13=({struct
Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=(void*)Cyc_Absyn_sint_t;
_tmp30->tl=_tmp13;_tmp30;});goto _LL3B;_LL3F: goto _LL3B;_LL3B:;}{void*t;switch(
_tmp19){case 'd': _LL45: goto _LL46;case 'i': _LL46:{struct Cyc_List_List*f=_tmp1D;for(
0;f != 0;f=f->tl){if((int)f->hd == '#'){({struct Cyc_Std_Int_pa_struct _tmp32;_tmp32.tag=
1;_tmp32.f1=(int)((unsigned int)((int)_tmp19));{void*_tmp31[1]={& _tmp32};Cyc_Tcutil_terr(
loc,_tag_arr("flag '#' is not valid with %%%c",sizeof(unsigned char),32),
_tag_arr(_tmp31,sizeof(void*),1));}});return 0;}}}{struct Cyc_List_List*_tmp33=
_tmp1A;struct Cyc_List_List _tmp34;struct Cyc_List_List*_tmp35;int _tmp36;struct Cyc_List_List
_tmp37;struct Cyc_List_List*_tmp38;int _tmp39;struct Cyc_List_List _tmp3A;struct Cyc_List_List*
_tmp3B;struct Cyc_List_List _tmp3C;struct Cyc_List_List*_tmp3D;int _tmp3E;int _tmp3F;
_LL49: if(_tmp33 == 0){goto _LL4A;}else{goto _LL4B;}_LL4B: if(_tmp33 == 0){goto _LL4D;}
else{_tmp34=*_tmp33;_LL55: _tmp36=(int)_tmp34.hd;goto _LL54;_LL54: _tmp35=_tmp34.tl;
if(_tmp35 == 0){goto _LL53;}else{goto _LL4D;}}_LL53: if(_tmp36 == 'l'){goto _LL4C;}
else{goto _LL4D;}_LL4D: if(_tmp33 == 0){goto _LL4F;}else{_tmp37=*_tmp33;_LL58: _tmp39=(
int)_tmp37.hd;goto _LL57;_LL57: _tmp38=_tmp37.tl;if(_tmp38 == 0){goto _LL56;}else{
goto _LL4F;}}_LL56: if(_tmp39 == 'h'){goto _LL4E;}else{goto _LL4F;}_LL4F: if(_tmp33 == 
0){goto _LL51;}else{_tmp3A=*_tmp33;_LL5D: _tmp3F=(int)_tmp3A.hd;goto _LL5A;_LL5A:
_tmp3B=_tmp3A.tl;if(_tmp3B == 0){goto _LL51;}else{_tmp3C=*_tmp3B;_LL5C: _tmp3E=(int)
_tmp3C.hd;goto _LL5B;_LL5B: _tmp3D=_tmp3C.tl;if(_tmp3D == 0){goto _LL59;}else{goto
_LL51;}}}_LL59: if(_tmp3F == 'h'? _tmp3E == 'h': 0){goto _LL50;}else{goto _LL51;}_LL51:
goto _LL52;_LL4A: t=Cyc_Absyn_sint_t;goto _LL48;_LL4C: t=Cyc_Absyn_slong_t;goto _LL48;
_LL4E: t=Cyc_Absyn_sshort_t;goto _LL48;_LL50: t=Cyc_Absyn_schar_t;goto _LL48;_LL52:({
struct Cyc_Std_Int_pa_struct _tmp42;_tmp42.tag=1;_tmp42.f1=(int)((unsigned int)((
int)_tmp19));{struct Cyc_Std_String_pa_struct _tmp41;_tmp41.tag=0;_tmp41.f1=(
struct _tagged_arr)Cyc_Std_implode(_tmp1A);{void*_tmp40[2]={& _tmp41,& _tmp42};Cyc_Tcutil_terr(
loc,_tag_arr("length modifier '%s' is not allowed with %%%c",sizeof(
unsigned char),46),_tag_arr(_tmp40,sizeof(void*),2));}}});return 0;_LL48:;}_tmp13=({
struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->hd=(void*)t;_tmp43->tl=
_tmp13;_tmp43;});break;case 'u': _LL47:{struct Cyc_List_List*f=_tmp1D;for(0;f != 0;f=
f->tl){if((int)f->hd == '#'){({void*_tmp44[0]={};Cyc_Tcutil_terr(loc,_tag_arr("Flag '#' not valid with %%u",
sizeof(unsigned char),28),_tag_arr(_tmp44,sizeof(void*),0));});return 0;}}}goto
_LL5E;case 'o': _LL5E: goto _LL5F;case 'x': _LL5F: goto _LL60;case 'X': _LL60:{struct Cyc_List_List*
_tmp45=_tmp1A;struct Cyc_List_List _tmp46;struct Cyc_List_List*_tmp47;int _tmp48;
struct Cyc_List_List _tmp49;struct Cyc_List_List*_tmp4A;int _tmp4B;struct Cyc_List_List
_tmp4C;struct Cyc_List_List*_tmp4D;struct Cyc_List_List _tmp4E;struct Cyc_List_List*
_tmp4F;int _tmp50;int _tmp51;_LL63: if(_tmp45 == 0){goto _LL64;}else{goto _LL65;}_LL65:
if(_tmp45 == 0){goto _LL67;}else{_tmp46=*_tmp45;_LL6F: _tmp48=(int)_tmp46.hd;goto
_LL6E;_LL6E: _tmp47=_tmp46.tl;if(_tmp47 == 0){goto _LL6D;}else{goto _LL67;}}_LL6D:
if(_tmp48 == 'l'){goto _LL66;}else{goto _LL67;}_LL67: if(_tmp45 == 0){goto _LL69;}
else{_tmp49=*_tmp45;_LL72: _tmp4B=(int)_tmp49.hd;goto _LL71;_LL71: _tmp4A=_tmp49.tl;
if(_tmp4A == 0){goto _LL70;}else{goto _LL69;}}_LL70: if(_tmp4B == 'h'){goto _LL68;}
else{goto _LL69;}_LL69: if(_tmp45 == 0){goto _LL6B;}else{_tmp4C=*_tmp45;_LL77: _tmp51=(
int)_tmp4C.hd;goto _LL74;_LL74: _tmp4D=_tmp4C.tl;if(_tmp4D == 0){goto _LL6B;}else{
_tmp4E=*_tmp4D;_LL76: _tmp50=(int)_tmp4E.hd;goto _LL75;_LL75: _tmp4F=_tmp4E.tl;if(
_tmp4F == 0){goto _LL73;}else{goto _LL6B;}}}_LL73: if(_tmp51 == 'h'? _tmp50 == 'h': 0){
goto _LL6A;}else{goto _LL6B;}_LL6B: goto _LL6C;_LL64: t=Cyc_Absyn_uint_t;goto _LL62;
_LL66: t=Cyc_Absyn_ulong_t;goto _LL62;_LL68: t=Cyc_Absyn_ushort_t;goto _LL62;_LL6A: t=
Cyc_Absyn_uchar_t;goto _LL62;_LL6C:({struct Cyc_Std_Int_pa_struct _tmp54;_tmp54.tag=
1;_tmp54.f1=(int)((unsigned int)((int)_tmp19));{struct Cyc_Std_String_pa_struct
_tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp1A);{void*
_tmp52[2]={& _tmp53,& _tmp54};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof(unsigned char),46),_tag_arr(_tmp52,sizeof(void*),2));}}});return 0;_LL62:;}
_tmp13=({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->hd=(void*)
t;_tmp55->tl=_tmp13;_tmp55;});break;case 'f': _LL61: goto _LL78;case 'F': _LL78: goto
_LL79;case 'e': _LL79: goto _LL7A;case 'E': _LL7A: goto _LL7B;case 'g': _LL7B: goto _LL7C;
case 'G': _LL7C: goto _LL7D;case 'a': _LL7D: goto _LL7E;case 'A': _LL7E: if(_tmp1A != 0){({
struct Cyc_Std_Int_pa_struct _tmp57;_tmp57.tag=1;_tmp57.f1=(int)((unsigned int)((
int)_tmp19));{void*_tmp56[1]={& _tmp57};Cyc_Tcutil_terr(loc,_tag_arr("length modifiers are not allowed with %%%c",
sizeof(unsigned char),43),_tag_arr(_tmp56,sizeof(void*),1));}});return 0;}_tmp13=({
struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->hd=(void*)Cyc_Absyn_double_typ(
0);_tmp58->tl=_tmp13;_tmp58;});break;case 'c': _LL7F:{struct Cyc_List_List*f=_tmp1D;
for(0;f != 0;f=f->tl){if((int)f->hd == '#'? 1:(int)f->hd == '0'){({struct Cyc_Std_Int_pa_struct
_tmp5A;_tmp5A.tag=1;_tmp5A.f1=(int)((unsigned int)((int)f->hd));{void*_tmp59[1]={&
_tmp5A};Cyc_Tcutil_terr(loc,_tag_arr("flag '%c' not allowed with %%c",sizeof(
unsigned char),31),_tag_arr(_tmp59,sizeof(void*),1));}});return 0;}}}if(_tmp1A != 
0){({struct Cyc_Std_String_pa_struct _tmp5C;_tmp5C.tag=0;_tmp5C.f1=(struct
_tagged_arr)Cyc_Std_implode(_tmp1A);{void*_tmp5B[1]={& _tmp5C};Cyc_Tcutil_terr(
loc,_tag_arr("length modifier '%s' not allowed with %%c",sizeof(unsigned char),
42),_tag_arr(_tmp5B,sizeof(void*),1));}});return 0;}if(_tmp1B != 0){({struct Cyc_Std_String_pa_struct
_tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp1B);{void*
_tmp5D[1]={& _tmp5E};Cyc_Tcutil_terr(loc,_tag_arr("precision '%s' not allowed with %%c",
sizeof(unsigned char),36),_tag_arr(_tmp5D,sizeof(void*),1));}});return 0;}_tmp13=({
struct Cyc_List_List*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->hd=(void*)Cyc_Absyn_sint_t;
_tmp5F->tl=_tmp13;_tmp5F;});break;case 's': _LL80:{struct Cyc_List_List*f=_tmp1D;
for(0;f != 0;f=f->tl){if((int)f->hd != '-'){({void*_tmp60[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("a flag not allowed with %%s",sizeof(unsigned char),28),_tag_arr(
_tmp60,sizeof(void*),0));});return 0;}}}if(_tmp1A != 0){({void*_tmp61[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("length modifiers not allowed with %%s",sizeof(unsigned char),38),
_tag_arr(_tmp61,sizeof(void*),0));});return 0;}_tmp13=({struct Cyc_List_List*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=(void*)Cyc_Absyn_const_string_typ(
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,({struct Cyc_Core_Opt*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp63;})));_tmp62->tl=_tmp13;_tmp62;});break;case 'p': _LL81: _tmp13=({struct Cyc_List_List*
_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->hd=(void*)Cyc_Absyn_uint_t;_tmp64->tl=
_tmp13;_tmp64;});break;case 'n': _LL82:{struct Cyc_List_List*f=_tmp1D;for(0;f != 0;f=
f->tl){if((int)f->hd == '#'? 1:(int)f->hd == '0'){({struct Cyc_Std_Int_pa_struct
_tmp66;_tmp66.tag=1;_tmp66.f1=(int)((unsigned int)((int)f->hd));{void*_tmp65[1]={&
_tmp66};Cyc_Tcutil_terr(loc,_tag_arr("flag '%c' not allowed with %%n",sizeof(
unsigned char),31),_tag_arr(_tmp65,sizeof(void*),1));}});return 0;}}}if(_tmp1B != 
0){({struct Cyc_Std_String_pa_struct _tmp68;_tmp68.tag=0;_tmp68.f1=(struct
_tagged_arr)Cyc_Std_implode(_tmp1B);{void*_tmp67[1]={& _tmp68};Cyc_Tcutil_terr(
loc,_tag_arr("precision '%s' not allowed with %%n",sizeof(unsigned char),36),
_tag_arr(_tmp67,sizeof(void*),1));}});return 0;}{struct Cyc_Core_Opt*_tmp69=({
struct Cyc_Core_Opt*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp7B;});{struct Cyc_List_List*_tmp6A=_tmp1A;struct Cyc_List_List _tmp6B;
struct Cyc_List_List*_tmp6C;int _tmp6D;struct Cyc_List_List _tmp6E;struct Cyc_List_List*
_tmp6F;int _tmp70;struct Cyc_List_List _tmp71;struct Cyc_List_List*_tmp72;struct Cyc_List_List
_tmp73;struct Cyc_List_List*_tmp74;int _tmp75;int _tmp76;_LL85: if(_tmp6A == 0){goto
_LL86;}else{goto _LL87;}_LL87: if(_tmp6A == 0){goto _LL89;}else{_tmp6B=*_tmp6A;_LL91:
_tmp6D=(int)_tmp6B.hd;goto _LL90;_LL90: _tmp6C=_tmp6B.tl;if(_tmp6C == 0){goto _LL8F;}
else{goto _LL89;}}_LL8F: if(_tmp6D == 'l'){goto _LL88;}else{goto _LL89;}_LL89: if(
_tmp6A == 0){goto _LL8B;}else{_tmp6E=*_tmp6A;_LL94: _tmp70=(int)_tmp6E.hd;goto _LL93;
_LL93: _tmp6F=_tmp6E.tl;if(_tmp6F == 0){goto _LL92;}else{goto _LL8B;}}_LL92: if(
_tmp70 == 'h'){goto _LL8A;}else{goto _LL8B;}_LL8B: if(_tmp6A == 0){goto _LL8D;}else{
_tmp71=*_tmp6A;_LL99: _tmp76=(int)_tmp71.hd;goto _LL96;_LL96: _tmp72=_tmp71.tl;if(
_tmp72 == 0){goto _LL8D;}else{_tmp73=*_tmp72;_LL98: _tmp75=(int)_tmp73.hd;goto _LL97;
_LL97: _tmp74=_tmp73.tl;if(_tmp74 == 0){goto _LL95;}else{goto _LL8D;}}}_LL95: if(
_tmp76 == 'h'? _tmp75 == 'h': 0){goto _LL8C;}else{goto _LL8D;}_LL8D: goto _LL8E;_LL86: t=
Cyc_Absyn_sint_t;goto _LL84;_LL88: t=Cyc_Absyn_ulong_t;goto _LL84;_LL8A: t=Cyc_Absyn_sshort_t;
goto _LL84;_LL8C: t=Cyc_Absyn_schar_t;goto _LL84;_LL8E:({struct Cyc_Std_Int_pa_struct
_tmp79;_tmp79.tag=1;_tmp79.f1=(int)((unsigned int)((int)_tmp19));{struct Cyc_Std_String_pa_struct
_tmp78;_tmp78.tag=0;_tmp78.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp1A);{void*
_tmp77[2]={& _tmp78,& _tmp79};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof(unsigned char),46),_tag_arr(_tmp77,sizeof(void*),2));}}});return 0;_LL84:;}
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_tmp69),Cyc_Absyn_empty_tqual());_tmp13=({struct Cyc_List_List*_tmp7A=_cycalloc(
sizeof(*_tmp7A));_tmp7A->hd=(void*)t;_tmp7A->tl=_tmp13;_tmp7A;});break;}case '%':
_LL83: if(_tmp1D != 0){({struct Cyc_Std_String_pa_struct _tmp7D;_tmp7D.tag=0;_tmp7D.f1=(
struct _tagged_arr)Cyc_Std_implode(_tmp1D);{void*_tmp7C[1]={& _tmp7D};Cyc_Tcutil_terr(
loc,_tag_arr("flags '%s' not allowed with %%%%",sizeof(unsigned char),33),
_tag_arr(_tmp7C,sizeof(void*),1));}});return 0;}if(_tmp1C != 0){({struct Cyc_Std_String_pa_struct
_tmp7F;_tmp7F.tag=0;_tmp7F.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp1C);{void*
_tmp7E[1]={& _tmp7F};Cyc_Tcutil_terr(loc,_tag_arr("width '%s' not allowed with %%%%",
sizeof(unsigned char),33),_tag_arr(_tmp7E,sizeof(void*),1));}});return 0;}if(
_tmp1B != 0){({struct Cyc_Std_String_pa_struct _tmp81;_tmp81.tag=0;_tmp81.f1=(
struct _tagged_arr)Cyc_Std_implode(_tmp1B);{void*_tmp80[1]={& _tmp81};Cyc_Tcutil_terr(
loc,_tag_arr("precision '%s' not allowed with %%%%",sizeof(unsigned char),37),
_tag_arr(_tmp80,sizeof(void*),1));}});return 0;}if(_tmp1A != 0){({struct Cyc_Std_String_pa_struct
_tmp83;_tmp83.tag=0;_tmp83.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp1A);{void*
_tmp82[1]={& _tmp83};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' not allowed with %%%%",
sizeof(unsigned char),43),_tag_arr(_tmp82,sizeof(void*),1));}});return 0;}break;
default: _LL9A: return 0;}}}}}return Cyc_List_imp_rev(_tmp13);}struct _tuple4{int f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;unsigned char f4;int f5;};struct Cyc_Core_Opt*
Cyc_Formatstr_parse_inputformat(struct _tagged_arr s,int i){unsigned int _tmp84=Cyc_Std_strlen(
s);if(i < 0? 1: i >= _tmp84){return 0;}{int _tmp85=0;unsigned char _tmp86=((const
unsigned char*)s.curr)[i];if(_tmp86 == '*'){_tmp85=1;i ++;if(i >= _tmp84){return 0;}}{
struct Cyc_List_List*_tmp87=0;for(0;i < _tmp84;i ++){_tmp86=((const unsigned char*)s.curr)[
i];if(isdigit((int)_tmp86)){_tmp87=({struct Cyc_List_List*_tmp88=_cycalloc(
sizeof(*_tmp88));_tmp88->hd=(void*)((int)_tmp86);_tmp88->tl=_tmp87;_tmp88;});}
else{break;}}if(i >= _tmp84){return 0;}_tmp87=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp87);{struct Cyc_List_List*_tmp89=0;_tmp86=((const
unsigned char*)s.curr)[i];switch(_tmp86){case 'h': _LL9C: _tmp89=({struct Cyc_List_List*
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->hd=(void*)((int)_tmp86);_tmp8A->tl=
_tmp89;_tmp8A;});i ++;if(i >= _tmp84){return 0;}_tmp86=((const unsigned char*)s.curr)[
i];if(_tmp86 == 'h'){_tmp89=({struct Cyc_List_List*_tmp8B=_cycalloc(sizeof(*_tmp8B));
_tmp8B->hd=(void*)((int)_tmp86);_tmp8B->tl=_tmp89;_tmp8B;});i ++;}break;case 'l':
_LL9D: _tmp89=({struct Cyc_List_List*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->hd=(
void*)((int)_tmp86);_tmp8C->tl=_tmp89;_tmp8C;});i ++;if(i >= _tmp84){return 0;}
_tmp86=((const unsigned char*)s.curr)[i];if(_tmp86 == 'l'){_tmp89=({struct Cyc_List_List*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->hd=(void*)((int)_tmp86);_tmp8D->tl=
_tmp89;_tmp8D;});i ++;}break;case 'j': _LL9E: goto _LL9F;case 'z': _LL9F: goto _LLA0;case
't': _LLA0: goto _LLA1;case 'L': _LLA1: _tmp89=({struct Cyc_List_List*_tmp8E=_cycalloc(
sizeof(*_tmp8E));_tmp8E->hd=(void*)((int)_tmp86);_tmp8E->tl=_tmp89;_tmp8E;});i ++;
break;default: _LLA2: break;}if(i >= _tmp84){return 0;}_tmp89=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp89);_tmp86=((const unsigned char*)s.curr)[
i];switch(_tmp86){case 'd': _LLA4: goto _LLA5;case 'i': _LLA5: goto _LLA6;case 'o': _LLA6:
goto _LLA7;case 'u': _LLA7: goto _LLA8;case 'x': _LLA8: goto _LLA9;case 'X': _LLA9: goto
_LLAA;case 'f': _LLAA: goto _LLAB;case 'F': _LLAB: goto _LLAC;case 'e': _LLAC: goto _LLAD;
case 'E': _LLAD: goto _LLAE;case 'g': _LLAE: goto _LLAF;case 'G': _LLAF: goto _LLB0;case 'a':
_LLB0: goto _LLB1;case 'A': _LLB1: goto _LLB2;case 'c': _LLB2: goto _LLB3;case 's': _LLB3:
goto _LLB4;case 'p': _LLB4: goto _LLB5;case 'n': _LLB5: goto _LLB6;case '%': _LLB6: break;
default: _LLB7: return 0;}return({struct Cyc_Core_Opt*_tmp8F=_cycalloc(sizeof(*
_tmp8F));_tmp8F->v=({struct _tuple4*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->f1=
_tmp85;_tmp90->f2=_tmp87;_tmp90->f3=_tmp89;_tmp90->f4=_tmp86;_tmp90->f5=i + 1;
_tmp90;});_tmp8F;});}}}}struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct
Cyc_Tcenv_Tenv*te,struct _tagged_arr s,struct Cyc_Position_Segment*loc){
unsigned int _tmp91=Cyc_Std_strlen(s);struct Cyc_List_List*_tmp92=0;int i;for(i=0;i
< _tmp91;i ++){if(((const unsigned char*)s.curr)[i]!= '%'){continue;}{struct Cyc_Core_Opt*
_tmp93=Cyc_Formatstr_parse_inputformat(s,i + 1);if(_tmp93 == 0){({void*_tmp94[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("bad format string",sizeof(unsigned char),18),
_tag_arr(_tmp94,sizeof(void*),0));});return 0;}{struct _tuple4 _tmp96;int _tmp97;
unsigned char _tmp98;struct Cyc_List_List*_tmp99;struct Cyc_List_List*_tmp9A;int
_tmp9B;struct _tuple4*_tmp95=(struct _tuple4*)_tmp93->v;_tmp96=*_tmp95;_LLBE:
_tmp9B=_tmp96.f1;goto _LLBD;_LLBD: _tmp9A=_tmp96.f2;goto _LLBC;_LLBC: _tmp99=_tmp96.f3;
goto _LLBB;_LLBB: _tmp98=_tmp96.f4;goto _LLBA;_LLBA: _tmp97=_tmp96.f5;goto _LLB9;
_LLB9: i=_tmp97 - 1;{struct Cyc_List_List*_tmp9C=_tmp99;struct Cyc_List_List _tmp9D;
struct Cyc_List_List*_tmp9E;int _tmp9F;_LLC0: if(_tmp9C == 0){goto _LLC2;}else{_tmp9D=*
_tmp9C;_LLC6: _tmp9F=(int)_tmp9D.hd;goto _LLC5;_LLC5: _tmp9E=_tmp9D.tl;if(_tmp9E == 
0){goto _LLC4;}else{goto _LLC2;}}_LLC4: if((_tmp9F == 'j'? 1: _tmp9F == 'z')? 1: _tmp9F == 't'){
goto _LLC1;}else{goto _LLC2;}_LLC2: goto _LLC3;_LLC1:({struct Cyc_Std_Int_pa_struct
_tmpA1;_tmpA1.tag=1;_tmpA1.f1=(int)((unsigned int)_tmp9F);{void*_tmpA0[1]={&
_tmpA1};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%c' is not supported",
sizeof(unsigned char),38),_tag_arr(_tmpA0,sizeof(void*),1));}});return 0;_LLC3:
goto _LLBF;_LLBF:;}if(_tmp9B){continue;}{void*t;switch(_tmp98){case 'd': _LLC7: goto
_LLC8;case 'i': _LLC8: {struct Cyc_Core_Opt*_tmpA2=({struct Cyc_Core_Opt*_tmpB4=
_cycalloc(sizeof(*_tmpB4));_tmpB4->v=Cyc_Tcenv_lookup_type_vars(te);_tmpB4;});{
struct Cyc_List_List*_tmpA3=_tmp99;struct Cyc_List_List _tmpA4;struct Cyc_List_List*
_tmpA5;int _tmpA6;struct Cyc_List_List _tmpA7;struct Cyc_List_List*_tmpA8;int _tmpA9;
struct Cyc_List_List _tmpAA;struct Cyc_List_List*_tmpAB;struct Cyc_List_List _tmpAC;
struct Cyc_List_List*_tmpAD;int _tmpAE;int _tmpAF;_LLCB: if(_tmpA3 == 0){goto _LLCC;}
else{goto _LLCD;}_LLCD: if(_tmpA3 == 0){goto _LLCF;}else{_tmpA4=*_tmpA3;_LLD7: _tmpA6=(
int)_tmpA4.hd;goto _LLD6;_LLD6: _tmpA5=_tmpA4.tl;if(_tmpA5 == 0){goto _LLD5;}else{
goto _LLCF;}}_LLD5: if(_tmpA6 == 'l'){goto _LLCE;}else{goto _LLCF;}_LLCF: if(_tmpA3 == 
0){goto _LLD1;}else{_tmpA7=*_tmpA3;_LLDA: _tmpA9=(int)_tmpA7.hd;goto _LLD9;_LLD9:
_tmpA8=_tmpA7.tl;if(_tmpA8 == 0){goto _LLD8;}else{goto _LLD1;}}_LLD8: if(_tmpA9 == 'h'){
goto _LLD0;}else{goto _LLD1;}_LLD1: if(_tmpA3 == 0){goto _LLD3;}else{_tmpAA=*_tmpA3;
_LLDF: _tmpAF=(int)_tmpAA.hd;goto _LLDC;_LLDC: _tmpAB=_tmpAA.tl;if(_tmpAB == 0){goto
_LLD3;}else{_tmpAC=*_tmpAB;_LLDE: _tmpAE=(int)_tmpAC.hd;goto _LLDD;_LLDD: _tmpAD=
_tmpAC.tl;if(_tmpAD == 0){goto _LLDB;}else{goto _LLD3;}}}_LLDB: if(_tmpAF == 'h'?
_tmpAE == 'h': 0){goto _LLD2;}else{goto _LLD3;}_LLD3: goto _LLD4;_LLCC: t=Cyc_Absyn_sint_t;
goto _LLCA;_LLCE: t=Cyc_Absyn_slong_t;goto _LLCA;_LLD0: t=Cyc_Absyn_sshort_t;goto
_LLCA;_LLD2: t=Cyc_Absyn_schar_t;goto _LLCA;_LLD4:({struct Cyc_Std_Int_pa_struct
_tmpB2;_tmpB2.tag=1;_tmpB2.f1=(int)((unsigned int)((int)_tmp98));{struct Cyc_Std_String_pa_struct
_tmpB1;_tmpB1.tag=0;_tmpB1.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp99);{void*
_tmpB0[2]={& _tmpB1,& _tmpB2};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof(unsigned char),46),_tag_arr(_tmpB0,sizeof(void*),2));}}});return 0;_LLCA:;}
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_tmpA2),Cyc_Absyn_empty_tqual());_tmp92=({struct Cyc_List_List*_tmpB3=_cycalloc(
sizeof(*_tmpB3));_tmpB3->hd=(void*)t;_tmpB3->tl=_tmp92;_tmpB3;});break;}case 'u':
_LLC9: goto _LLE0;case 'o': _LLE0: goto _LLE1;case 'x': _LLE1: goto _LLE2;case 'X': _LLE2: {
struct Cyc_Core_Opt*_tmpB5=({struct Cyc_Core_Opt*_tmpC7=_cycalloc(sizeof(*_tmpC7));
_tmpC7->v=Cyc_Tcenv_lookup_type_vars(te);_tmpC7;});{struct Cyc_List_List*_tmpB6=
_tmp99;struct Cyc_List_List _tmpB7;struct Cyc_List_List*_tmpB8;int _tmpB9;struct Cyc_List_List
_tmpBA;struct Cyc_List_List*_tmpBB;int _tmpBC;struct Cyc_List_List _tmpBD;struct Cyc_List_List*
_tmpBE;struct Cyc_List_List _tmpBF;struct Cyc_List_List*_tmpC0;int _tmpC1;int _tmpC2;
_LLE5: if(_tmpB6 == 0){goto _LLE6;}else{goto _LLE7;}_LLE7: if(_tmpB6 == 0){goto _LLE9;}
else{_tmpB7=*_tmpB6;_LLF1: _tmpB9=(int)_tmpB7.hd;goto _LLF0;_LLF0: _tmpB8=_tmpB7.tl;
if(_tmpB8 == 0){goto _LLEF;}else{goto _LLE9;}}_LLEF: if(_tmpB9 == 'l'){goto _LLE8;}
else{goto _LLE9;}_LLE9: if(_tmpB6 == 0){goto _LLEB;}else{_tmpBA=*_tmpB6;_LLF4: _tmpBC=(
int)_tmpBA.hd;goto _LLF3;_LLF3: _tmpBB=_tmpBA.tl;if(_tmpBB == 0){goto _LLF2;}else{
goto _LLEB;}}_LLF2: if(_tmpBC == 'h'){goto _LLEA;}else{goto _LLEB;}_LLEB: if(_tmpB6 == 
0){goto _LLED;}else{_tmpBD=*_tmpB6;_LLF9: _tmpC2=(int)_tmpBD.hd;goto _LLF6;_LLF6:
_tmpBE=_tmpBD.tl;if(_tmpBE == 0){goto _LLED;}else{_tmpBF=*_tmpBE;_LLF8: _tmpC1=(int)
_tmpBF.hd;goto _LLF7;_LLF7: _tmpC0=_tmpBF.tl;if(_tmpC0 == 0){goto _LLF5;}else{goto
_LLED;}}}_LLF5: if(_tmpC2 == 'h'? _tmpC1 == 'h': 0){goto _LLEC;}else{goto _LLED;}_LLED:
goto _LLEE;_LLE6: t=Cyc_Absyn_uint_t;goto _LLE4;_LLE8: t=Cyc_Absyn_ulong_t;goto _LLE4;
_LLEA: t=Cyc_Absyn_ushort_t;goto _LLE4;_LLEC: t=Cyc_Absyn_uchar_t;goto _LLE4;_LLEE:({
struct Cyc_Std_Int_pa_struct _tmpC5;_tmpC5.tag=1;_tmpC5.f1=(int)((unsigned int)((
int)_tmp98));{struct Cyc_Std_String_pa_struct _tmpC4;_tmpC4.tag=0;_tmpC4.f1=(
struct _tagged_arr)Cyc_Std_implode(_tmp99);{void*_tmpC3[2]={& _tmpC4,& _tmpC5};Cyc_Tcutil_terr(
loc,_tag_arr("length modifier '%s' is not allowed with %%%c",sizeof(
unsigned char),46),_tag_arr(_tmpC3,sizeof(void*),2));}}});return 0;_LLE4:;}t=Cyc_Absyn_at_typ(
t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmpB5),Cyc_Absyn_empty_tqual());
_tmp92=({struct Cyc_List_List*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->hd=(void*)
t;_tmpC6->tl=_tmp92;_tmpC6;});break;}case 'f': _LLE3: goto _LLFA;case 'F': _LLFA: goto
_LLFB;case 'e': _LLFB: goto _LLFC;case 'E': _LLFC: goto _LLFD;case 'g': _LLFD: goto _LLFE;
case 'G': _LLFE: goto _LLFF;case 'a': _LLFF: goto _LL100;case 'A': _LL100: {struct Cyc_Core_Opt*
_tmpC8=({struct Cyc_Core_Opt*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpD1;});{struct Cyc_List_List*_tmpC9=_tmp99;struct Cyc_List_List _tmpCA;
struct Cyc_List_List*_tmpCB;int _tmpCC;_LL103: if(_tmpC9 == 0){goto _LL104;}else{goto
_LL105;}_LL105: if(_tmpC9 == 0){goto _LL107;}else{_tmpCA=*_tmpC9;_LL10B: _tmpCC=(int)
_tmpCA.hd;goto _LL10A;_LL10A: _tmpCB=_tmpCA.tl;if(_tmpCB == 0){goto _LL109;}else{
goto _LL107;}}_LL109: if(_tmpCC == 'l'){goto _LL106;}else{goto _LL107;}_LL107: goto
_LL108;_LL104: t=Cyc_Absyn_float_typ;goto _LL102;_LL106: t=Cyc_Absyn_double_typ(0);
goto _LL102;_LL108:({struct Cyc_Std_Int_pa_struct _tmpCF;_tmpCF.tag=1;_tmpCF.f1=(
int)((unsigned int)((int)_tmp98));{struct Cyc_Std_String_pa_struct _tmpCE;_tmpCE.tag=
0;_tmpCE.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp99);{void*_tmpCD[2]={& _tmpCE,&
_tmpCF};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof(unsigned char),46),_tag_arr(_tmpCD,sizeof(void*),2));}}});return 0;_LL102:;}
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_tmpC8),Cyc_Absyn_empty_tqual());_tmp92=({struct Cyc_List_List*_tmpD0=_cycalloc(
sizeof(*_tmpD0));_tmpD0->hd=(void*)t;_tmpD0->tl=_tmp92;_tmpD0;});break;}case 'c':
_LL101: goto _LL10C;case 's': _LL10C: {struct Cyc_Core_Opt*_tmpD2=({struct Cyc_Core_Opt*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpD4;});_tmp92=({struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->hd=(
void*)Cyc_Absyn_string_typ(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_tmpD2));_tmpD3->tl=_tmp92;_tmpD3;});break;}case '[': _LL10D: goto _LL10E;case 'p':
_LL10E:({struct Cyc_Std_Int_pa_struct _tmpD6;_tmpD6.tag=1;_tmpD6.f1=(int)((
unsigned int)((int)_tmp98));{void*_tmpD5[1]={& _tmpD6};Cyc_Tcutil_terr(loc,
_tag_arr("%%%c is not supported",sizeof(unsigned char),22),_tag_arr(_tmpD5,
sizeof(void*),1));}});return 0;case 'n': _LL10F: {struct Cyc_Core_Opt*_tmpD7=({
struct Cyc_Core_Opt*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpE9;});{struct Cyc_List_List*_tmpD8=_tmp99;struct Cyc_List_List _tmpD9;
struct Cyc_List_List*_tmpDA;int _tmpDB;struct Cyc_List_List _tmpDC;struct Cyc_List_List*
_tmpDD;int _tmpDE;struct Cyc_List_List _tmpDF;struct Cyc_List_List*_tmpE0;struct Cyc_List_List
_tmpE1;struct Cyc_List_List*_tmpE2;int _tmpE3;int _tmpE4;_LL112: if(_tmpD8 == 0){goto
_LL113;}else{goto _LL114;}_LL114: if(_tmpD8 == 0){goto _LL116;}else{_tmpD9=*_tmpD8;
_LL11E: _tmpDB=(int)_tmpD9.hd;goto _LL11D;_LL11D: _tmpDA=_tmpD9.tl;if(_tmpDA == 0){
goto _LL11C;}else{goto _LL116;}}_LL11C: if(_tmpDB == 'l'){goto _LL115;}else{goto
_LL116;}_LL116: if(_tmpD8 == 0){goto _LL118;}else{_tmpDC=*_tmpD8;_LL121: _tmpDE=(int)
_tmpDC.hd;goto _LL120;_LL120: _tmpDD=_tmpDC.tl;if(_tmpDD == 0){goto _LL11F;}else{
goto _LL118;}}_LL11F: if(_tmpDE == 'h'){goto _LL117;}else{goto _LL118;}_LL118: if(
_tmpD8 == 0){goto _LL11A;}else{_tmpDF=*_tmpD8;_LL126: _tmpE4=(int)_tmpDF.hd;goto
_LL123;_LL123: _tmpE0=_tmpDF.tl;if(_tmpE0 == 0){goto _LL11A;}else{_tmpE1=*_tmpE0;
_LL125: _tmpE3=(int)_tmpE1.hd;goto _LL124;_LL124: _tmpE2=_tmpE1.tl;if(_tmpE2 == 0){
goto _LL122;}else{goto _LL11A;}}}_LL122: if(_tmpE4 == 'h'? _tmpE3 == 'h': 0){goto _LL119;}
else{goto _LL11A;}_LL11A: goto _LL11B;_LL113: t=Cyc_Absyn_sint_t;goto _LL111;_LL115: t=
Cyc_Absyn_ulong_t;goto _LL111;_LL117: t=Cyc_Absyn_sshort_t;goto _LL111;_LL119: t=Cyc_Absyn_schar_t;
goto _LL111;_LL11B:({struct Cyc_Std_Int_pa_struct _tmpE7;_tmpE7.tag=1;_tmpE7.f1=(
int)((unsigned int)((int)_tmp98));{struct Cyc_Std_String_pa_struct _tmpE6;_tmpE6.tag=
0;_tmpE6.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp99);{void*_tmpE5[2]={& _tmpE6,&
_tmpE7};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof(unsigned char),46),_tag_arr(_tmpE5,sizeof(void*),2));}}});return 0;_LL111:;}
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_tmpD7),Cyc_Absyn_empty_tqual());_tmp92=({struct Cyc_List_List*_tmpE8=_cycalloc(
sizeof(*_tmpE8));_tmpE8->hd=(void*)t;_tmpE8->tl=_tmp92;_tmpE8;});break;}case '%':
_LL110: if(_tmp9B){({void*_tmpEA[0]={};Cyc_Tcutil_terr(loc,_tag_arr("Assignment suppression (*) is not allowed with %%%%",
sizeof(unsigned char),52),_tag_arr(_tmpEA,sizeof(void*),0));});return 0;}if(
_tmp9A != 0){({struct Cyc_Std_String_pa_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=(
struct _tagged_arr)Cyc_Std_implode(_tmp9A);{void*_tmpEB[1]={& _tmpEC};Cyc_Tcutil_terr(
loc,_tag_arr("width '%s' not allowed with %%%%",sizeof(unsigned char),33),
_tag_arr(_tmpEB,sizeof(void*),1));}});return 0;}if(_tmp99 != 0){({struct Cyc_Std_String_pa_struct
_tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp99);{void*
_tmpED[1]={& _tmpEE};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' not allowed with %%%%",
sizeof(unsigned char),43),_tag_arr(_tmpED,sizeof(void*),1));}});return 0;}break;
default: _LL127: return 0;}}}}}return Cyc_List_imp_rev(_tmp92);}
