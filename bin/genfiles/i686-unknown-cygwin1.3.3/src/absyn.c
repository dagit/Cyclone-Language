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
tl;};extern struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);extern struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
unsigned char Cyc_List_Nth[8];extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
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
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};unsigned char Cyc_Absyn_EmptyAnnot[
15]="\000\000\000\000EmptyAnnot";extern int Cyc_Absyn_qvar_cmp(struct _tuple0*,
struct _tuple0*);extern int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
extern int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);extern
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;extern void*Cyc_Absyn_rel_ns_null;
extern int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);extern struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual();extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref();extern struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x);extern void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern void*
Cyc_Absyn_compress_kb(void*);extern void*Cyc_Absyn_force_kb(void*kb);extern void*
Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*);extern void*Cyc_Absyn_uchar_t;extern void*Cyc_Absyn_ushort_t;
extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_ulong_t;extern void*Cyc_Absyn_ulonglong_t;
extern void*Cyc_Absyn_schar_t;extern void*Cyc_Absyn_sshort_t;extern void*Cyc_Absyn_sint_t;
extern void*Cyc_Absyn_slong_t;extern void*Cyc_Absyn_slonglong_t;extern void*Cyc_Absyn_float_typ;
extern void*Cyc_Absyn_double_typ(int);extern void*Cyc_Absyn_empty_effect;extern
struct _tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_exn_typ;extern struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar;extern void*Cyc_Absyn_string_typ(
void*rgn);extern void*Cyc_Absyn_const_string_typ(void*rgn);extern void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
extern void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b);
extern void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b);
extern void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_cstar_typ(
void*t,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_tagged_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_void_star_typ();extern struct Cyc_Core_Opt*
Cyc_Absyn_void_star_typ_opt();extern void*Cyc_Absyn_strct(struct _tagged_arr*name);
extern void*Cyc_Absyn_strctq(struct _tuple0*name);extern void*Cyc_Absyn_unionq_typ(
struct _tuple0*name);extern void*Cyc_Absyn_union_typ(struct _tagged_arr*name);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*,struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(unsigned char c,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr f,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _tagged_arr s,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*,struct Cyc_List_List*es,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*,void*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,
void*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_structmember_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,
struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*loc);extern
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(
struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,struct
Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);extern
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);extern
struct Cyc_Absyn_Stmt*Cyc_Absyn_splice_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _tagged_arr*v,struct
Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct
Cyc_List_List*scs,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _tagged_arr*lab,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(struct Cyc_List_List*,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);
extern struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct
Cyc_Core_Opt*t_opt,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);extern
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,
struct Cyc_Absyn_Exp*init);extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);extern struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct Cyc_Core_Opt*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,struct
Cyc_List_List*atts,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(void*s,struct Cyc_Core_Opt*n,struct Cyc_List_List*ts,struct
Cyc_Core_Opt*fs,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);extern
struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*loc);extern
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*);extern void*Cyc_Absyn_pointer_expand(
void*);extern int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _tagged_arr*);extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_struct_field(struct Cyc_Absyn_Structdecl*,struct _tagged_arr*);
extern struct Cyc_Absyn_Structfield*Cyc_Absyn_lookup_union_field(struct Cyc_Absyn_Uniondecl*,
struct _tagged_arr*);struct _tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};extern struct
_tuple3*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);extern struct
_tagged_arr Cyc_Absyn_attribute2string(void*);extern int Cyc_Absyn_fntype_att(void*
a);extern struct _tagged_arr*Cyc_Absyn_fieldname(int);extern void Cyc_Absyn_print_decls(
struct Cyc_List_List*);struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;}
;struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{
int tag;void*f1;};struct _tuple4{unsigned int f1;struct _tagged_arr f2;void*f3;};
struct Cyc_Typerep_Struct_struct{int tag;struct _tagged_arr*f1;unsigned int f2;
struct _tagged_arr f3;};struct _tuple5{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{
int tag;unsigned int f1;struct _tagged_arr f2;};struct _tuple6{unsigned int f1;struct
_tagged_arr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;
struct _tagged_arr f2;struct _tagged_arr f3;};struct _tuple7{struct _tagged_arr f1;void*
f2;};struct Cyc_Typerep_XTUnion_struct{int tag;struct _tagged_arr f1;struct
_tagged_arr f2;};struct Cyc_Typerep_Union_struct{int tag;struct _tagged_arr*f1;
struct _tagged_arr f2;};unsigned int Cyc_Typerep_size_type(void*rep);extern void*Cyc_decls_rep;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;extern int
Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int
Cyc_Std_printf(struct _tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr
Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[
11];struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_StructRes_struct{
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
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern void Cyc_Marshal_print_type(void*rep,void*val);static int Cyc_Absyn_zstrlist_cmp(
struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){return((int(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))
Cyc_List_list_cmp)(Cyc_Std_zstrptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct
Cyc_List_List*vs1,struct Cyc_List_List*vs2){return Cyc_Absyn_zstrlist_cmp(vs1,vs2);}
struct _tuple8{void*f1;void*f2;};int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct
_tuple0*q2){void*_tmp0=(*q1).f1;void*_tmp1=(*q2).f1;{struct _tuple8 _tmp3=({struct
_tuple8 _tmp2;_tmp2.f1=_tmp0;_tmp2.f2=_tmp1;_tmp2;});void*_tmp4;void*_tmp5;void*
_tmp6;struct Cyc_List_List*_tmp7;void*_tmp8;struct Cyc_List_List*_tmp9;void*_tmpA;
struct Cyc_List_List*_tmpB;void*_tmpC;struct Cyc_List_List*_tmpD;void*_tmpE;void*
_tmpF;void*_tmp10;void*_tmp11;_LL1: _LL10: _tmp5=_tmp3.f1;if((int)_tmp5 == 0){goto
_LLF;}else{goto _LL3;}_LLF: _tmp4=_tmp3.f2;if((int)_tmp4 == 0){goto _LL2;}else{goto
_LL3;}_LL3: _LL13: _tmp8=_tmp3.f1;if((unsigned int)_tmp8 > 1?*((int*)_tmp8)== 0: 0){
_LL14: _tmp9=((struct Cyc_Absyn_Rel_n_struct*)_tmp8)->f1;goto _LL11;}else{goto _LL5;}
_LL11: _tmp6=_tmp3.f2;if((unsigned int)_tmp6 > 1?*((int*)_tmp6)== 0: 0){_LL12: _tmp7=((
struct Cyc_Absyn_Rel_n_struct*)_tmp6)->f1;goto _LL4;}else{goto _LL5;}_LL5: _LL17:
_tmpC=_tmp3.f1;if((unsigned int)_tmpC > 1?*((int*)_tmpC)== 1: 0){_LL18: _tmpD=((
struct Cyc_Absyn_Abs_n_struct*)_tmpC)->f1;goto _LL15;}else{goto _LL7;}_LL15: _tmpA=
_tmp3.f2;if((unsigned int)_tmpA > 1?*((int*)_tmpA)== 1: 0){_LL16: _tmpB=((struct Cyc_Absyn_Abs_n_struct*)
_tmpA)->f1;goto _LL6;}else{goto _LL7;}_LL7: _LL19: _tmpE=_tmp3.f1;if((int)_tmpE == 0){
goto _LL8;}else{goto _LL9;}_LL9: _LL1A: _tmpF=_tmp3.f2;if((int)_tmpF == 0){goto _LLA;}
else{goto _LLB;}_LLB: _LL1B: _tmp10=_tmp3.f1;if((unsigned int)_tmp10 > 1?*((int*)
_tmp10)== 0: 0){goto _LLC;}else{goto _LLD;}_LLD: _LL1C: _tmp11=_tmp3.f2;if((
unsigned int)_tmp11 > 1?*((int*)_tmp11)== 0: 0){goto _LLE;}else{goto _LL0;}_LL2: goto
_LL0;_LL4: _tmpD=_tmp9;_tmpB=_tmp7;goto _LL6;_LL6: {int i=Cyc_Absyn_zstrlist_cmp(
_tmpD,_tmpB);if(i != 0){return i;}goto _LL0;}_LL8: return - 1;_LLA: return 1;_LLC: return
- 1;_LLE: return 1;_LL0:;}return Cyc_Std_zstrptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_Std_zstrptrcmp(tv1->name,
tv2->name);if(i != 0){return i;}if(tv1->identity == tv2->identity){return 0;}if(tv1->identity
!= 0? tv2->identity != 0: 0){return*((int*)_check_null(tv1->identity))- *((int*)
_check_null(tv2->identity));}else{if(tv1->identity == 0){return - 1;}else{return 1;}}}
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value={0,0};void*Cyc_Absyn_rel_ns_null=(
void*)& Cyc_Absyn_rel_ns_null_value;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*
qv){void*_tmp13=(*qv).f1;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp15;
_LL1E: if((unsigned int)_tmp13 > 1?*((int*)_tmp13)== 0: 0){_LL26: _tmp14=((struct Cyc_Absyn_Rel_n_struct*)
_tmp13)->f1;if(_tmp14 == 0){goto _LL1F;}else{goto _LL20;}}else{goto _LL20;}_LL20: if((
unsigned int)_tmp13 > 1?*((int*)_tmp13)== 1: 0){_LL27: _tmp15=((struct Cyc_Absyn_Abs_n_struct*)
_tmp13)->f1;if(_tmp15 == 0){goto _LL21;}else{goto _LL22;}}else{goto _LL22;}_LL22: if((
int)_tmp13 == 0){goto _LL23;}else{goto _LL24;}_LL24: goto _LL25;_LL1F: goto _LL21;_LL21:
goto _LL23;_LL23: return 0;_LL25: return 1;_LL1D:;}static int Cyc_Absyn_new_type_counter=
0;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){return(
void*)({struct Cyc_Absyn_Evar_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({
struct Cyc_Absyn_Evar_struct _tmp17;_tmp17.tag=0;_tmp17.f1=k;_tmp17.f2=0;_tmp17.f3=
Cyc_Absyn_new_type_counter ++;_tmp17.f4=env;_tmp17;});_tmp16;});}static struct Cyc_Core_Opt
Cyc_Absyn_mk={(void*)((void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({
struct Cyc_Absyn_Tqual _tmp18;_tmp18.q_const=x.q_const? 1: y.q_const;_tmp18.q_volatile=
x.q_volatile? 1: y.q_volatile;_tmp18.q_restrict=x.q_restrict? 1: y.q_restrict;_tmp18;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(){return({struct Cyc_Absyn_Tqual _tmp19;
_tmp19.q_const=0;_tmp19.q_volatile=0;_tmp19.q_restrict=0;_tmp19;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(){return({struct Cyc_Absyn_Tqual _tmp1A;_tmp1A.q_const=1;
_tmp1A.q_volatile=0;_tmp1A.q_restrict=0;_tmp1A;});}struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x){return({struct Cyc_Absyn_Conref*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->v=(
void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));
_tmp1C[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(void*)
x;_tmp1D;});_tmp1C;}));_tmp1B;});}struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref(){
return({struct Cyc_Absyn_Conref*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->v=(void*)((
void*)0);_tmp1E;});}static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_constraint={
0,(void*)1};static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_constraint={0,(
void*)0};struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={(void*)((void*)& Cyc_Absyn_true_constraint)};
struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(void*)((void*)& Cyc_Absyn_false_constraint)};
struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x){void*_tmp21=(void*)x->v;struct Cyc_Absyn_Conref*_tmp22;
struct Cyc_Absyn_Conref**_tmp23;_LL29: if((int)_tmp21 == 0){goto _LL2A;}else{goto
_LL2B;}_LL2B: if((unsigned int)_tmp21 > 1?*((int*)_tmp21)== 0: 0){goto _LL2C;}else{
goto _LL2D;}_LL2D: if((unsigned int)_tmp21 > 1?*((int*)_tmp21)== 1: 0){_LL2F: _tmp22=((
struct Cyc_Absyn_Forward_constr_struct*)_tmp21)->f1;_tmp23=(struct Cyc_Absyn_Conref**)&((
struct Cyc_Absyn_Forward_constr_struct*)_tmp21)->f1;goto _LL2E;}else{goto _LL28;}
_LL2A: return x;_LL2C: return x;_LL2E: {struct Cyc_Absyn_Conref*_tmp24=Cyc_Absyn_compress_conref(*
_tmp23);*_tmp23=_tmp24;return _tmp24;}_LL28:;}void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x){void*_tmp25=(void*)(Cyc_Absyn_compress_conref(x))->v;void*_tmp26;_LL31: if((
unsigned int)_tmp25 > 1?*((int*)_tmp25)== 0: 0){_LL35: _tmp26=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp25)->f1;goto _LL32;}else{goto _LL33;}_LL33: goto _LL34;_LL32: return _tmp26;_LL34:
return({void*_tmp27[0]={};Cyc_Tcutil_impos(_tag_arr("conref_val",sizeof(
unsigned char),11),_tag_arr(_tmp27,sizeof(void*),0));});_LL30:;}void*Cyc_Absyn_conref_def(
void*y,struct Cyc_Absyn_Conref*x){void*_tmp28=(void*)(Cyc_Absyn_compress_conref(x))->v;
void*_tmp29;_LL37: if((unsigned int)_tmp28 > 1?*((int*)_tmp28)== 0: 0){_LL3B: _tmp29=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp28)->f1;goto _LL38;}else{goto _LL39;}
_LL39: goto _LL3A;_LL38: return _tmp29;_LL3A: return y;_LL36:;}void*Cyc_Absyn_compress_kb(
void*k){void*_tmp2A=k;struct Cyc_Core_Opt*_tmp2B;struct Cyc_Core_Opt*_tmp2C;struct
Cyc_Core_Opt*_tmp2D;struct Cyc_Core_Opt _tmp2E;void*_tmp2F;void**_tmp30;struct Cyc_Core_Opt*
_tmp31;struct Cyc_Core_Opt _tmp32;void*_tmp33;void**_tmp34;_LL3D: if(*((int*)_tmp2A)
== 0){goto _LL3E;}else{goto _LL3F;}_LL3F: if(*((int*)_tmp2A)== 1){_LL47: _tmp2B=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp2A)->f1;if(_tmp2B == 0){goto _LL40;}else{
goto _LL41;}}else{goto _LL41;}_LL41: if(*((int*)_tmp2A)== 2){_LL48: _tmp2C=((struct
Cyc_Absyn_Less_kb_struct*)_tmp2A)->f1;if(_tmp2C == 0){goto _LL42;}else{goto _LL43;}}
else{goto _LL43;}_LL43: if(*((int*)_tmp2A)== 1){_LL49: _tmp2D=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp2A)->f1;if(_tmp2D == 0){goto _LL45;}else{_tmp2E=*_tmp2D;_LL4A: _tmp2F=(void*)
_tmp2E.v;_tmp30=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)_tmp2A)->f1).v;
goto _LL44;}}else{goto _LL45;}_LL45: if(*((int*)_tmp2A)== 2){_LL4B: _tmp31=((struct
Cyc_Absyn_Less_kb_struct*)_tmp2A)->f1;if(_tmp31 == 0){goto _LL3C;}else{_tmp32=*
_tmp31;_LL4C: _tmp33=(void*)_tmp32.v;_tmp34=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp2A)->f1).v;goto _LL46;}}else{goto _LL3C;}_LL3E: return k;_LL40: return k;_LL42:
return k;_LL44: _tmp34=_tmp30;goto _LL46;_LL46:*_tmp34=Cyc_Absyn_compress_kb(*
_tmp34);return*_tmp34;_LL3C:;}void*Cyc_Absyn_force_kb(void*kb){void*_tmp35=Cyc_Absyn_compress_kb(
kb);void*_tmp36;struct Cyc_Core_Opt*_tmp37;struct Cyc_Core_Opt**_tmp38;void*_tmp39;
struct Cyc_Core_Opt*_tmp3A;struct Cyc_Core_Opt**_tmp3B;_LL4E: if(*((int*)_tmp35)== 
0){_LL54: _tmp36=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp35)->f1;goto _LL4F;}
else{goto _LL50;}_LL50: if(*((int*)_tmp35)== 1){_LL55: _tmp37=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp35)->f1;_tmp38=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp35)->f1;goto _LL51;}else{goto _LL52;}_LL52: if(*((int*)_tmp35)== 2){_LL57:
_tmp3A=((struct Cyc_Absyn_Less_kb_struct*)_tmp35)->f1;_tmp3B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp35)->f1;goto _LL56;_LL56: _tmp39=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp35)->f2;goto _LL53;}else{goto _LL4D;}_LL4F:
return _tmp36;_LL51: _tmp3B=_tmp38;_tmp39=(void*)2;goto _LL53;_LL53:*_tmp3B=({
struct Cyc_Core_Opt*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->v=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(void*)_tmp39;_tmp3E;});
_tmp3D;}));_tmp3C;});return _tmp39;_LL4D:;}static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uchar_tt={5,(void*)((void*)1),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={5,(void*)((void*)1),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uint_tt={5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulong_tt={5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulonglong_tt={5,(void*)((void*)1),(void*)((void*)3)};void*Cyc_Absyn_uchar_t=(
void*)& Cyc_Absyn_uchar_tt;void*Cyc_Absyn_ushort_t=(void*)& Cyc_Absyn_ushort_tt;
void*Cyc_Absyn_uint_t=(void*)& Cyc_Absyn_uint_tt;void*Cyc_Absyn_ulong_t=(void*)&
Cyc_Absyn_ulong_tt;void*Cyc_Absyn_ulonglong_t=(void*)& Cyc_Absyn_ulonglong_tt;
static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={5,(void*)((void*)0),(
void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sshort_tt={5,(
void*)((void*)0),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={
5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slonglong_tt={
5,(void*)((void*)0),(void*)((void*)3)};void*Cyc_Absyn_schar_t=(void*)& Cyc_Absyn_schar_tt;
void*Cyc_Absyn_sshort_t=(void*)& Cyc_Absyn_sshort_tt;void*Cyc_Absyn_sint_t=(void*)&
Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_t=(void*)& Cyc_Absyn_slong_tt;void*Cyc_Absyn_slonglong_t=(
void*)& Cyc_Absyn_slonglong_tt;void*Cyc_Absyn_float_typ=(void*)1;void*Cyc_Absyn_double_typ(
int b){return(void*)({struct Cyc_Absyn_DoubleType_struct*_tmp49=_cycalloc_atomic(
sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_DoubleType_struct _tmp4A;_tmp4A.tag=
6;_tmp4A.f1=b;_tmp4A;});_tmp49;});}static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_abs_null={
1,0};static unsigned char _tmp4C[4]="exn";static struct _tagged_arr Cyc_Absyn_exn_str={
_tmp4C,_tmp4C,_tmp4C + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={(void*)& Cyc_Absyn_abs_null,&
Cyc_Absyn_exn_str};struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static
unsigned char _tmp4D[15]="Null_Exception";static struct _tagged_arr Cyc_Absyn_Null_Exception_str={
_tmp4D,_tmp4D,_tmp4D + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={(void*)&
Cyc_Absyn_abs_null,& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr,0,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static unsigned char
_tmp4E[13]="Array_bounds";static struct _tagged_arr Cyc_Absyn_Array_bounds_str={
_tmp4E,_tmp4E,_tmp4E + 13};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(void*)&
Cyc_Absyn_abs_null,& Cyc_Absyn_Array_bounds_str};struct _tuple0*Cyc_Absyn_Array_bounds_name=&
Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={&
Cyc_Absyn_Array_bounds_pr,0,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;static unsigned char
_tmp4F[16]="Match_Exception";static struct _tagged_arr Cyc_Absyn_Match_Exception_str={
_tmp4F,_tmp4F,_tmp4F + 16};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={(void*)&
Cyc_Absyn_abs_null,& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=&
Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={&
Cyc_Absyn_Match_Exception_pr,0,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v;static
unsigned char _tmp50[10]="Bad_alloc";static struct _tagged_arr Cyc_Absyn_Bad_alloc_str={
_tmp50,_tmp50,_tmp50 + 10};static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={(void*)& Cyc_Absyn_abs_null,&
Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;
static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,
0,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Bad_alloc_tuf=&
Cyc_Absyn_Bad_alloc_tuf_v;static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,
0};static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={(void*)((void*)3),& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1};struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Absyn_KnownTunion_struct Cyc_Absyn_exn_tinfou={1,& Cyc_Absyn_exn_tud};
static struct Cyc_Absyn_TunionType_struct Cyc_Absyn_exn_typ_tt={2,{(void*)((void*)&
Cyc_Absyn_exn_tinfou),0,(void*)((void*)2)}};void*Cyc_Absyn_exn_typ=(void*)& Cyc_Absyn_exn_typ_tt;
static unsigned char _tmp53[9]="PrintArg";static struct _tagged_arr Cyc_Absyn_printarg_str={
_tmp53,_tmp53,_tmp53 + 9};static unsigned char _tmp54[9]="ScanfArg";static struct
_tagged_arr Cyc_Absyn_scanfarg_str={_tmp54,_tmp54,_tmp54 + 9};static unsigned char
_tmp55[4]="Cyc";static struct _tagged_arr Cyc_Absyn_cyc_str={_tmp55,_tmp55,_tmp55 + 
4};static unsigned char _tmp56[4]="Std";static struct _tagged_arr Cyc_Absyn_std_str={
_tmp56,_tmp56,_tmp56 + 4};static struct Cyc_List_List Cyc_Absyn_std_list={(void*)&
Cyc_Absyn_std_str,0};static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_std_namespace={
1,(struct Cyc_List_List*)& Cyc_Absyn_std_list};static struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_tunion_scanf_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar=&
Cyc_Absyn_tunion_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar=&
Cyc_Absyn_tunion_scanf_arg_qvar_p;static struct Cyc_Core_Opt*Cyc_Absyn_string_t_opt=
0;void*Cyc_Absyn_string_typ(void*rgn){if(rgn != (void*)2){return Cyc_Absyn_starb_typ(
Cyc_Absyn_uchar_t,rgn,Cyc_Absyn_empty_tqual(),(void*)0);}else{if(Cyc_Absyn_string_t_opt
== 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_uchar_t,(void*)2,Cyc_Absyn_empty_tqual(),(
void*)0);Cyc_Absyn_string_t_opt=({struct Cyc_Core_Opt*_tmp58=_cycalloc(sizeof(*
_tmp58));_tmp58->v=(void*)t;_tmp58;});}return(void*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Absyn_string_t_opt))->v;}}static struct Cyc_Core_Opt*Cyc_Absyn_const_string_t_opt=
0;void*Cyc_Absyn_const_string_typ(void*rgn){if(rgn != (void*)2){return Cyc_Absyn_starb_typ(
Cyc_Absyn_uchar_t,rgn,Cyc_Absyn_const_tqual(),(void*)0);}else{if(Cyc_Absyn_const_string_t_opt
== 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_uchar_t,(void*)2,Cyc_Absyn_const_tqual(),(
void*)0);Cyc_Absyn_const_string_t_opt=({struct Cyc_Core_Opt*_tmp59=_cycalloc(
sizeof(*_tmp59));_tmp59->v=(void*)t;_tmp59;});}return(void*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Absyn_const_string_t_opt))->v;}}static struct Cyc_Absyn_Int_c_struct
Cyc_Absyn_one_intc={2,(void*)((void*)1),1};static struct Cyc_Absyn_Const_e_struct
Cyc_Absyn_one_b_raw={0,(void*)((void*)& Cyc_Absyn_one_intc)};struct Cyc_Absyn_Exp
Cyc_Absyn_exp_unsigned_one_v={0,(void*)((void*)& Cyc_Absyn_one_b_raw),0,(void*)((
void*)Cyc_Absyn_EmptyAnnot)};struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={0,& Cyc_Absyn_exp_unsigned_one_v};
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;void*Cyc_Absyn_starb_typ(void*
t,void*r,struct Cyc_Absyn_Tqual tq,void*b){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Absyn_PointerType_struct
_tmp5E;_tmp5E.tag=4;_tmp5E.f1=({struct Cyc_Absyn_PtrInfo _tmp5F;_tmp5F.elt_typ=(
void*)t;_tmp5F.rgn_typ=(void*)r;_tmp5F.nullable=Cyc_Absyn_true_conref;_tmp5F.tq=
tq;_tmp5F.bounds=Cyc_Absyn_new_conref(b);_tmp5F;});_tmp5E;});_tmp5D;});}void*Cyc_Absyn_atb_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Absyn_PointerType_struct
_tmp61;_tmp61.tag=4;_tmp61.f1=({struct Cyc_Absyn_PtrInfo _tmp62;_tmp62.elt_typ=(
void*)t;_tmp62.rgn_typ=(void*)r;_tmp62.nullable=Cyc_Absyn_false_conref;_tmp62.tq=
tq;_tmp62.bounds=Cyc_Absyn_new_conref(b);_tmp62;});_tmp61;});_tmp60;});}void*Cyc_Absyn_star_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq){return Cyc_Absyn_starb_typ(t,r,tq,Cyc_Absyn_bounds_one);}
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){return Cyc_Absyn_starb_typ(
t,(void*)2,tq,Cyc_Absyn_bounds_one);}void*Cyc_Absyn_at_typ(void*t,void*r,struct
Cyc_Absyn_Tqual tq){return Cyc_Absyn_atb_typ(t,r,tq,Cyc_Absyn_bounds_one);}void*
Cyc_Absyn_tagged_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({
struct Cyc_Absyn_PointerType_struct _tmp64;_tmp64.tag=4;_tmp64.f1=({struct Cyc_Absyn_PtrInfo
_tmp65;_tmp65.elt_typ=(void*)t;_tmp65.rgn_typ=(void*)r;_tmp65.nullable=Cyc_Absyn_true_conref;
_tmp65.tq=tq;_tmp65.bounds=Cyc_Absyn_new_conref((void*)0);_tmp65;});_tmp64;});
_tmp63;});}static struct Cyc_Core_Opt*Cyc_Absyn_file_t_opt=0;static unsigned char
_tmp66[8]="__sFILE";static struct _tagged_arr Cyc_Absyn_sf_str={_tmp66,_tmp66,
_tmp66 + 8};static struct _tagged_arr*Cyc_Absyn_sf=& Cyc_Absyn_sf_str;static
unsigned char _tmp67[4]="Std";static struct _tagged_arr Cyc_Absyn_st_str={_tmp67,
_tmp67,_tmp67 + 4};static struct _tagged_arr*Cyc_Absyn_st=& Cyc_Absyn_st_str;void*
Cyc_Absyn_file_typ(){if(Cyc_Absyn_file_t_opt == 0){struct _tuple0*file_t_name=({
struct _tuple0*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_Absyn_Abs_n_struct _tmp70;
_tmp70.tag=1;_tmp70.f1=({struct _tagged_arr*_tmp71[1];_tmp71[0]=Cyc_Absyn_st;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp71,
sizeof(struct _tagged_arr*),1));});_tmp70;});_tmp6F;});_tmp6E->f2=Cyc_Absyn_sf;
_tmp6E;});struct Cyc_Absyn_Structdecl*sd=({struct Cyc_Absyn_Structdecl*_tmp6C=
_cycalloc(sizeof(*_tmp6C));_tmp6C->sc=(void*)((void*)1);_tmp6C->name=({struct Cyc_Core_Opt*
_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->v=file_t_name;_tmp6D;});_tmp6C->tvs=0;
_tmp6C->fields=0;_tmp6C->attributes=0;_tmp6C;});void*file_struct_typ=(void*)({
struct Cyc_Absyn_StructType_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=({
struct Cyc_Absyn_StructType_struct _tmp6A;_tmp6A.tag=10;_tmp6A.f1=(struct _tuple0*)
file_t_name;_tmp6A.f2=0;_tmp6A.f3=({struct Cyc_Absyn_Structdecl**_tmp6B=_cycalloc(
sizeof(struct Cyc_Absyn_Structdecl*)* 1);_tmp6B[0]=sd;_tmp6B;});_tmp6A;});_tmp69;});
Cyc_Absyn_file_t_opt=({struct Cyc_Core_Opt*_tmp68=_cycalloc(sizeof(*_tmp68));
_tmp68->v=(void*)Cyc_Absyn_at_typ(file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual());
_tmp68;});}return(void*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_file_t_opt))->v;}
static struct Cyc_Core_Opt*Cyc_Absyn_void_star_t_opt=0;void*Cyc_Absyn_void_star_typ(){
if(Cyc_Absyn_void_star_t_opt == 0){Cyc_Absyn_void_star_t_opt=({struct Cyc_Core_Opt*
_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->v=(void*)Cyc_Absyn_star_typ((void*)0,(
void*)2,Cyc_Absyn_empty_tqual());_tmp72;});}return(void*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Absyn_void_star_t_opt))->v;}struct Cyc_Core_Opt*Cyc_Absyn_void_star_typ_opt(){
if(Cyc_Absyn_void_star_t_opt == 0){Cyc_Absyn_void_star_t_opt=({struct Cyc_Core_Opt*
_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->v=(void*)Cyc_Absyn_star_typ((void*)0,(
void*)2,Cyc_Absyn_empty_tqual());_tmp73;});}return Cyc_Absyn_void_star_t_opt;}
static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={20,0};void*Cyc_Absyn_empty_effect=(
void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_strct(struct _tagged_arr*name){return(
void*)({struct Cyc_Absyn_StructType_struct*_tmp75=_cycalloc(sizeof(*_tmp75));
_tmp75[0]=({struct Cyc_Absyn_StructType_struct _tmp76;_tmp76.tag=10;_tmp76.f1=({
struct _tuple0*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->f1=Cyc_Absyn_rel_ns_null;
_tmp77->f2=name;_tmp77;});_tmp76.f2=0;_tmp76.f3=0;_tmp76;});_tmp75;});}void*Cyc_Absyn_union_typ(
struct _tagged_arr*name){return(void*)({struct Cyc_Absyn_UnionType_struct*_tmp78=
_cycalloc(sizeof(*_tmp78));_tmp78[0]=({struct Cyc_Absyn_UnionType_struct _tmp79;
_tmp79.tag=11;_tmp79.f1=({struct _tuple0*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=
Cyc_Absyn_rel_ns_null;_tmp7A->f2=name;_tmp7A;});_tmp79.f2=0;_tmp79.f3=0;_tmp79;});
_tmp78;});}void*Cyc_Absyn_strctq(struct _tuple0*name){return(void*)({struct Cyc_Absyn_StructType_struct*
_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({struct Cyc_Absyn_StructType_struct
_tmp7C;_tmp7C.tag=10;_tmp7C.f1=(struct _tuple0*)name;_tmp7C.f2=0;_tmp7C.f3=0;
_tmp7C;});_tmp7B;});}void*Cyc_Absyn_unionq_typ(struct _tuple0*name){return(void*)({
struct Cyc_Absyn_UnionType_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({
struct Cyc_Absyn_UnionType_struct _tmp7E;_tmp7E.tag=11;_tmp7E.f1=(struct _tuple0*)
name;_tmp7E.f2=0;_tmp7E.f3=0;_tmp7E;});_tmp7D;});}struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Exp*_tmp7F=
_cycalloc(sizeof(*_tmp7F));_tmp7F->topt=0;_tmp7F->r=(void*)r;_tmp7F->loc=loc;
_tmp7F->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);_tmp7F;});}struct Cyc_Absyn_Exp*
Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_New_e_struct*_tmp80=
_cycalloc(sizeof(*_tmp80));_tmp80[0]=({struct Cyc_Absyn_New_e_struct _tmp81;_tmp81.tag=
15;_tmp81.f1=rgn_handle;_tmp81.f2=e;_tmp81;});_tmp80;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){return({struct Cyc_Absyn_Exp*_tmp82=
_cycalloc(sizeof(*_tmp82));_tmp82->topt=e->topt;_tmp82->r=(void*)((void*)e->r);
_tmp82->loc=e->loc;_tmp82->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);_tmp82;});}
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*c,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*_tmp83=_cycalloc(
sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_Const_e_struct _tmp84;_tmp84.tag=0;
_tmp84.f1=(void*)c;_tmp84;});_tmp83;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({struct Cyc_Absyn_Const_e_struct
_tmp86;_tmp86.tag=0;_tmp86.f1=(void*)((void*)0);_tmp86;});_tmp85;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*s,int i,struct Cyc_Position_Segment*seg){
return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*_tmp87=_cycalloc(
sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_Int_c_struct _tmp88;_tmp88.tag=2;
_tmp88.f1=(void*)s;_tmp88.f2=i;_tmp88;});_tmp87;}),seg);}struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((void*)0,i,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)1,(int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_signed_int_exp(b? 1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(
1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){
return Cyc_Absyn_bool_exp(0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
unsigned char c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((void*)({
struct Cyc_Absyn_Char_c_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({
struct Cyc_Absyn_Char_c_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(void*)((void*)1);
_tmp8A.f2=c;_tmp8A;});_tmp89;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _tagged_arr f,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Float_c_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[
0]=({struct Cyc_Absyn_Float_c_struct _tmp8C;_tmp8C.tag=4;_tmp8C.f1=f;_tmp8C;});
_tmp8B;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _tagged_arr s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_String_c_struct
_tmp8E;_tmp8E.tag=5;_tmp8E.f1=s;_tmp8E;});_tmp8D;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Var_e_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({
struct Cyc_Absyn_Var_e_struct _tmp90;_tmp90.tag=1;_tmp90.f1=q;_tmp90.f2=(void*)((
void*)0);_tmp90;});_tmp8F;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct
_tuple0*q,void*b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Var_e_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({
struct Cyc_Absyn_Var_e_struct _tmp92;_tmp92.tag=1;_tmp92.f1=q;_tmp92.f2=(void*)b;
_tmp92;});_tmp91;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct
_tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp94;_tmp94.tag=2;_tmp94.f1=q;_tmp94;});
_tmp93;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp96;_tmp96.tag=3;_tmp96.f1=(void*)p;_tmp96.f2=es;_tmp96;});_tmp95;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmp97=_cycalloc(sizeof(*
_tmp97));_tmp97->hd=e;_tmp97->tl=0;_tmp97;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmp98=_cycalloc(sizeof(*
_tmp98));_tmp98->hd=e1;_tmp98->tl=({struct Cyc_List_List*_tmp99=_cycalloc(sizeof(*
_tmp99));_tmp99->hd=e2;_tmp99->tl=0;_tmp99;});_tmp98;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_prim2_exp((void*)0,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)1,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)3,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)5,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)6,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)7,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)8,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)9,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)10,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmp9A=
_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmp9B;
_tmp9B.tag=4;_tmp9B.f1=e1;_tmp9B.f2=popt;_tmp9B.f3=e2;_tmp9B;});_tmp9A;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Absyn_Increment_e_struct
_tmp9D;_tmp9D.tag=5;_tmp9D.f1=e;_tmp9D.f2=(void*)i;_tmp9D;});_tmp9C;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmp9F;_tmp9F.tag=6;_tmp9F.f1=e1;_tmp9F.f2=e2;_tmp9F.f3=e3;_tmp9F;});_tmp9E;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,e2,Cyc_Absyn_false_exp(
loc),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,Cyc_Absyn_true_exp(
loc),e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[
0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpA1;_tmpA1.tag=7;_tmpA1.f1=e1;_tmpA1.f2=
e2;_tmpA1;});_tmpA0;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*_tmpA2=
_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpA3;_tmpA3.tag=8;_tmpA3.f1=e;_tmpA3.f2=es;_tmpA3;});_tmpA2;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmpA4=
_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_FnCall_e_struct _tmpA5;
_tmpA5.tag=9;_tmpA5.f1=e;_tmpA5.f2=es;_tmpA5.f3=0;_tmpA5;});_tmpA4;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpA7;_tmpA7.tag=11;_tmpA7.f1=e;_tmpA7;});_tmpA6;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpA9;_tmpA9.tag=12;_tmpA9.f1=e;_tmpA9.f2=ts;_tmpA9;});_tmpA8;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*_tmpAA=
_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_Cast_e_struct _tmpAB;
_tmpAB.tag=13;_tmpAB.f1=(void*)t;_tmpAB.f2=e;_tmpAB;});_tmpAA;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_struct*_tmpAC=_cycalloc(
sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_Absyn_Throw_e_struct _tmpAD;_tmpAD.tag=10;
_tmpAD.f1=e;_tmpAD;});_tmpAC;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Address_e_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));
_tmpAE[0]=({struct Cyc_Absyn_Address_e_struct _tmpAF;_tmpAF.tag=14;_tmpAF.f1=e;
_tmpAF;});_tmpAE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpB1;_tmpB1.tag=16;_tmpB1.f1=(void*)t;_tmpB1;});_tmpB0;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*_tmpB2=
_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_Sizeofexp_e_struct _tmpB3;
_tmpB3.tag=17;_tmpB3.f1=e;_tmpB3;});_tmpB2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmpB5;_tmpB5.tag=18;_tmpB5.f1=(void*)t;_tmpB5.f2=(
void*)of;_tmpB5;});_tmpB4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpB7;_tmpB7.tag=19;_tmpB7.f1=tvs;_tmpB7.f2=(
void*)t;_tmpB7;});_tmpB6;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpB9;_tmpB9.tag=20;_tmpB9.f1=e;_tmpB9;});_tmpB8;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_structmember_exp(struct Cyc_Absyn_Exp*e,
struct _tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StructMember_e_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));
_tmpBA[0]=({struct Cyc_Absyn_StructMember_e_struct _tmpBB;_tmpBB.tag=21;_tmpBB.f1=
e;_tmpBB.f2=n;_tmpBB;});_tmpBA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*e,struct _tagged_arr*n,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_StructArrow_e_struct*_tmpBC=_cycalloc(
sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_StructArrow_e_struct _tmpBD;_tmpBD.tag=
22;_tmpBD.f1=e;_tmpBD.f2=n;_tmpBD;});_tmpBC;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_arrow_exp(
struct Cyc_Absyn_Exp*e,struct _tagged_arr*n,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_deref_exp(Cyc_Absyn_structmember_exp(e,n,loc),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpBE=
_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpBF;
_tmpBF.tag=23;_tmpBF.f1=e1;_tmpBF.f2=e2;_tmpBF;});_tmpBE;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*_tmpC0=_cycalloc(
sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_Tuple_e_struct _tmpC1;_tmpC1.tag=24;
_tmpC1.f1=es;_tmpC1;});_tmpC0;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));
_tmpC2[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpC3;_tmpC3.tag=35;_tmpC3.f1=s;
_tmpC3;});_tmpC2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct _tuple9{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpC4=_cycalloc(sizeof(*
_tmpC4));_tmpC4->hd=({struct _tuple9*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->f1=
0;_tmpC5->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpC5;});_tmpC4->tl=dles;_tmpC4;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpC6=_cycalloc(
sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_Array_e_struct _tmpC7;_tmpC7.tag=26;
_tmpC7.f1=dles;_tmpC7;});_tmpC6;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpC8=
_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpC9;_tmpC9.tag=34;_tmpC9.f1=n;_tmpC9.f2=dles;_tmpC9;});_tmpC8;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->r=(void*)s;_tmpCA->loc=
loc;_tmpCA->non_local_preds=0;_tmpCA->try_depth=0;_tmpCA->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpCA;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpCB=_cycalloc(
sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Absyn_Exp_s_struct _tmpCC;_tmpCC.tag=0;
_tmpCC.f1=e;_tmpCC;});_tmpCB;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0){return Cyc_Absyn_skip_stmt(
loc);}else{if(ss->tl == 0){return(struct Cyc_Absyn_Stmt*)ss->hd;}else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpCD=
_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_Return_s_struct _tmpCE;
_tmpCE.tag=2;_tmpCE.f1=e;_tmpCE;});_tmpCD;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpD0;_tmpD0.tag=3;_tmpD0.f1=e;_tmpD0.f2=s1;_tmpD0.f3=s2;_tmpD0;});_tmpCF;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({struct Cyc_Absyn_While_s_struct
_tmpD2;_tmpD2.tag=4;_tmpD2.f1=({struct _tuple2 _tmpD3;_tmpD3.f1=e;_tmpD3.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpD3;});_tmpD2.f2=s;_tmpD2;});_tmpD1;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_Break_s_struct
_tmpD5;_tmpD5.tag=5;_tmpD5.f1=0;_tmpD5;});_tmpD4;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));
_tmpD6[0]=({struct Cyc_Absyn_Continue_s_struct _tmpD7;_tmpD7.tag=6;_tmpD7.f1=0;
_tmpD7;});_tmpD6;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_For_s_struct _tmpD9;
_tmpD9.tag=8;_tmpD9.f1=e1;_tmpD9.f2=({struct _tuple2 _tmpDA;_tmpDA.f1=e2;_tmpDA.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpDA;});_tmpD9.f3=({struct _tuple2 _tmpDB;_tmpDB.f1=
e3;_tmpDB.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmpDB;});_tmpD9.f4=s;_tmpD9;});_tmpD8;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*
_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_Switch_s_struct
_tmpDD;_tmpDD.tag=9;_tmpDD.f1=e;_tmpDD.f2=scs;_tmpDD;});_tmpDC;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){void*_tmpDE=(void*)s1->r;_LL59: if((int)_tmpDE
== 0){goto _LL5A;}else{goto _LL5B;}_LL5B: goto _LL5C;_LL5A: return s2;_LL5C: return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Seq_s_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({
struct Cyc_Absyn_Seq_s_struct _tmpE0;_tmpE0.tag=1;_tmpE0.f1=s1;_tmpE0.f2=s2;_tmpE0;});
_tmpDF;}),loc);_LL58:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*
el,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Fallthru_s_struct*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1[0]=({struct Cyc_Absyn_Fallthru_s_struct
_tmpE2;_tmpE2.tag=11;_tmpE2.f1=el;_tmpE2.f2=0;_tmpE2;});_tmpE1;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpE4;_tmpE4.tag=12;_tmpE4.f1=d;_tmpE4.f2=s;_tmpE4;});_tmpE3;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=({
struct Cyc_Absyn_Var_d_struct _tmpE8;_tmpE8.tag=0;_tmpE8.f1=Cyc_Absyn_new_vardecl(
x,t,init);_tmpE8;});_tmpE7;}),loc);return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpE6;_tmpE6.tag=12;_tmpE6.f1=d;_tmpE6.f2=s;_tmpE6;});_tmpE5;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Cut_s_struct*_tmpE9=_cycalloc(
sizeof(*_tmpE9));_tmpE9[0]=({struct Cyc_Absyn_Cut_s_struct _tmpEA;_tmpEA.tag=13;
_tmpEA.f1=s;_tmpEA;});_tmpE9;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[
0]=({struct Cyc_Absyn_Splice_s_struct _tmpEC;_tmpEC.tag=14;_tmpEC.f1=s;_tmpEC;});
_tmpEB;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _tagged_arr*v,
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[
0]=({struct Cyc_Absyn_Label_s_struct _tmpEE;_tmpEE.tag=15;_tmpEE.f1=v;_tmpEE.f2=s;
_tmpEE;});_tmpED;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Do_s_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({
struct Cyc_Absyn_Do_s_struct _tmpF0;_tmpF0.tag=16;_tmpF0.f1=s;_tmpF0.f2=({struct
_tuple2 _tmpF1;_tmpF1.f1=e;_tmpF1.f2=Cyc_Absyn_skip_stmt(e->loc);_tmpF1;});_tmpF0;});
_tmpEF;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*
s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_TryCatch_s_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));
_tmpF2[0]=({struct Cyc_Absyn_TryCatch_s_struct _tmpF3;_tmpF3.tag=17;_tmpF3.f1=s;
_tmpF3.f2=scs;_tmpF3;});_tmpF2;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _tagged_arr*lab,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Goto_s_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({
struct Cyc_Absyn_Goto_s_struct _tmpF5;_tmpF5.tag=7;_tmpF5.f1=lab;_tmpF5.f2=0;
_tmpF5;});_tmpF4;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(
struct Cyc_List_List*d,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_ForArray_s_struct*
_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_ForArray_s_struct
_tmpF7;_tmpF7.tag=19;_tmpF7.f1=({struct Cyc_Absyn_ForArrayInfo _tmpF8;_tmpF8.defns=
d;_tmpF8.condition=({struct _tuple2 _tmpFA;_tmpFA.f1=e1;_tmpFA.f2=Cyc_Absyn_skip_stmt(
e1->loc);_tmpFA;});_tmpF8.delta=({struct _tuple2 _tmpF9;_tmpF9.f1=e2;_tmpF9.f2=Cyc_Absyn_skip_stmt(
e2->loc);_tmpF9;});_tmpF8.body=s;_tmpF8;});_tmpF7;});_tmpF6;}),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){return({struct Cyc_Absyn_Pat*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->r=(void*)p;_tmpFB->topt=0;_tmpFB->loc=
s;_tmpFB;});}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Decl*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->r=(
void*)r;_tmpFC->loc=loc;_tmpFC;});}struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(
struct Cyc_Absyn_Pat*p,struct Cyc_Core_Opt*t_opt,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*_tmpFD=
_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_Let_d_struct _tmpFE;_tmpFE.tag=
2;_tmpFE.f1=p;_tmpFE.f2=0;_tmpFE.f3=t_opt;_tmpFE.f4=e;_tmpFE.f5=0;_tmpFE;});
_tmpFD;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*
_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_Letv_d_struct
_tmp100;_tmp100.tag=3;_tmp100.f1=vds;_tmp100;});_tmpFF;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->sc=(void*)((
void*)2);_tmp101->name=x;_tmp101->tq=Cyc_Absyn_empty_tqual();_tmp101->type=(void*)
t;_tmp101->initializer=init;_tmp101->rgn=0;_tmp101->attributes=0;_tmp101->escapes=
0;_tmp101;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp102=
_cycalloc(sizeof(*_tmp102));_tmp102->sc=(void*)((void*)0);_tmp102->name=x;
_tmp102->tq=Cyc_Absyn_empty_tqual();_tmp102->type=(void*)t;_tmp102->initializer=
init;_tmp102->rgn=0;_tmp102->attributes=0;_tmp102->escapes=0;_tmp102;});}struct
Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*s,struct Cyc_Core_Opt*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Struct_d_struct*_tmp103=
_cycalloc(sizeof(*_tmp103));_tmp103[0]=({struct Cyc_Absyn_Struct_d_struct _tmp104;
_tmp104.tag=4;_tmp104.f1=({struct Cyc_Absyn_Structdecl*_tmp105=_cycalloc(sizeof(*
_tmp105));_tmp105->sc=(void*)s;_tmp105->name=n;_tmp105->tvs=ts;_tmp105->fields=
fs;_tmp105->attributes=atts;_tmp105;});_tmp104;});_tmp103;}),loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(void*s,struct Cyc_Core_Opt*n,struct Cyc_List_List*ts,struct
Cyc_Core_Opt*fs,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Union_d_struct*_tmp106=_cycalloc(
sizeof(*_tmp106));_tmp106[0]=({struct Cyc_Absyn_Union_d_struct _tmp107;_tmp107.tag=
5;_tmp107.f1=({struct Cyc_Absyn_Uniondecl*_tmp108=_cycalloc(sizeof(*_tmp108));
_tmp108->sc=(void*)s;_tmp108->name=n;_tmp108->tvs=ts;_tmp108->fields=fs;_tmp108->attributes=
atts;_tmp108;});_tmp107;});_tmp106;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Tunion_d_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({
struct Cyc_Absyn_Tunion_d_struct _tmp10A;_tmp10A.tag=6;_tmp10A.f1=({struct Cyc_Absyn_Tuniondecl*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->sc=(void*)s;_tmp10B->name=n;_tmp10B->tvs=
ts;_tmp10B->fields=fs;_tmp10B->is_xtunion=is_xtunion;_tmp10B;});_tmp10A;});
_tmp109;}),loc);}static struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){
return({struct _tuple1*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->f1=(*a).f1;
_tmp10C->f2=(*a).f2;_tmp10C->f3=Cyc_Absyn_pointer_expand((*a).f3);_tmp10C;});}
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){return(void*)({
struct Cyc_Absyn_FnType_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({
struct Cyc_Absyn_FnType_struct _tmp10E;_tmp10E.tag=8;_tmp10E.f1=({struct Cyc_Absyn_FnInfo
_tmp10F;_tmp10F.tvars=tvs;_tmp10F.ret_typ=(void*)Cyc_Absyn_pointer_expand(
ret_typ);_tmp10F.effect=eff_typ;_tmp10F.args=((struct Cyc_List_List*(*)(struct
_tuple1*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,
args);_tmp10F.c_varargs=c_varargs;_tmp10F.cyc_varargs=cyc_varargs;_tmp10F.rgn_po=
rgn_po;_tmp10F.attributes=atts;_tmp10F;});_tmp10E;});_tmp10D;});}void*Cyc_Absyn_pointer_expand(
void*t){void*_tmp110=Cyc_Tcutil_compress(t);_LL5E: if((unsigned int)_tmp110 > 3?*((
int*)_tmp110)== 8: 0){goto _LL5F;}else{goto _LL60;}_LL60: goto _LL61;_LL5F: return Cyc_Absyn_at_typ(
t,(void*)2,Cyc_Absyn_empty_tqual());_LL61: return t;_LL5D:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp111=(void*)e->r;void*_tmp112;void*_tmp113;struct
Cyc_Absyn_Vardecl*_tmp114;void*_tmp115;struct Cyc_Absyn_Vardecl*_tmp116;struct Cyc_Absyn_Exp*
_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct _tagged_arr*_tmp119;struct Cyc_Absyn_Exp*
_tmp11A;struct _tagged_arr*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*
_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;_LL63: if(*((int*)
_tmp111)== 1){_LL79: _tmp112=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp111)->f2;
if((unsigned int)_tmp112 > 1?*((int*)_tmp112)== 1: 0){goto _LL64;}else{goto _LL65;}}
else{goto _LL65;}_LL65: if(*((int*)_tmp111)== 1){_LL7A: _tmp113=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp111)->f2;if((unsigned int)_tmp113 > 1?*((int*)_tmp113)== 0: 0){_LL7B: _tmp114=((
struct Cyc_Absyn_Global_b_struct*)_tmp113)->f1;goto _LL66;}else{goto _LL67;}}else{
goto _LL67;}_LL67: if(*((int*)_tmp111)== 1){_LL7C: _tmp115=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp111)->f2;if((unsigned int)_tmp115 > 1?*((int*)_tmp115)== 3: 0){_LL7D: _tmp116=((
struct Cyc_Absyn_Local_b_struct*)_tmp115)->f1;goto _LL68;}else{goto _LL69;}}else{
goto _LL69;}_LL69: if(*((int*)_tmp111)== 1){goto _LL6A;}else{goto _LL6B;}_LL6B: if(*((
int*)_tmp111)== 23){_LL7F: _tmp118=((struct Cyc_Absyn_Subscript_e_struct*)_tmp111)->f1;
goto _LL7E;_LL7E: _tmp117=((struct Cyc_Absyn_Subscript_e_struct*)_tmp111)->f2;goto
_LL6C;}else{goto _LL6D;}_LL6D: if(*((int*)_tmp111)== 21){_LL81: _tmp11A=((struct Cyc_Absyn_StructMember_e_struct*)
_tmp111)->f1;goto _LL80;_LL80: _tmp119=((struct Cyc_Absyn_StructMember_e_struct*)
_tmp111)->f2;goto _LL6E;}else{goto _LL6F;}_LL6F: if(*((int*)_tmp111)== 22){_LL83:
_tmp11C=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp111)->f1;goto _LL82;_LL82:
_tmp11B=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp111)->f2;goto _LL70;}else{
goto _LL71;}_LL71: if(*((int*)_tmp111)== 20){_LL84: _tmp11D=((struct Cyc_Absyn_Deref_e_struct*)
_tmp111)->f1;goto _LL72;}else{goto _LL73;}_LL73: if(*((int*)_tmp111)== 12){_LL85:
_tmp11E=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp111)->f1;goto _LL74;}else{
goto _LL75;}_LL75: if(*((int*)_tmp111)== 11){_LL86: _tmp11F=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp111)->f1;goto _LL76;}else{goto _LL77;}_LL77: goto _LL78;_LL64: return 0;_LL66:
_tmp116=_tmp114;goto _LL68;_LL68: {void*_tmp120=Cyc_Tcutil_compress((void*)
_tmp116->type);_LL88: if((unsigned int)_tmp120 > 3?*((int*)_tmp120)== 7: 0){goto
_LL89;}else{goto _LL8A;}_LL8A: goto _LL8B;_LL89: return 0;_LL8B: return 1;_LL87:;}_LL6A:
return 1;_LL6C: return 1;_LL6E: return Cyc_Absyn_is_lvalue(_tmp11A);_LL70: return 1;
_LL72: return 1;_LL74: return Cyc_Absyn_is_lvalue(_tmp11E);_LL76: return Cyc_Absyn_is_lvalue(
_tmp11F);_LL78: return 0;_LL62:;}struct Cyc_Absyn_Structfield*Cyc_Absyn_lookup_field(
struct Cyc_List_List*fields,struct _tagged_arr*v){{struct Cyc_List_List*fs=fields;
for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Structfield*_tmp121=(struct Cyc_Absyn_Structfield*)
fs->hd;if(Cyc_Std_zstrptrcmp(_tmp121->name,v)== 0){return(struct Cyc_Absyn_Structfield*)
_tmp121;}}}return 0;}struct Cyc_Absyn_Structfield*Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*sd,struct _tagged_arr*v){return sd->fields == 0? 0: Cyc_Absyn_lookup_field((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sd->fields))->v,v);}
struct Cyc_Absyn_Structfield*Cyc_Absyn_lookup_union_field(struct Cyc_Absyn_Uniondecl*
ud,struct _tagged_arr*v){return ud->fields == 0? 0: Cyc_Absyn_lookup_field((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ud->fields))->v,v);}struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts == 0){return 0;}ts=ts->tl;}if(
ts == 0){return 0;}return(struct _tuple3*)((struct _tuple3*)ts->hd);}struct
_tagged_arr Cyc_Absyn_attribute2string(void*a){void*_tmp122=a;int _tmp123;int
_tmp124;struct _tagged_arr _tmp125;int _tmp126;int _tmp127;void*_tmp128;int _tmp129;
int _tmp12A;void*_tmp12B;_LL8D: if((unsigned int)_tmp122 > 16?*((int*)_tmp122)== 0:
0){_LLB7: _tmp123=((struct Cyc_Absyn_Regparm_att_struct*)_tmp122)->f1;goto _LL8E;}
else{goto _LL8F;}_LL8F: if((int)_tmp122 == 0){goto _LL90;}else{goto _LL91;}_LL91: if((
int)_tmp122 == 1){goto _LL92;}else{goto _LL93;}_LL93: if((int)_tmp122 == 2){goto _LL94;}
else{goto _LL95;}_LL95: if((int)_tmp122 == 3){goto _LL96;}else{goto _LL97;}_LL97: if((
int)_tmp122 == 4){goto _LL98;}else{goto _LL99;}_LL99: if((unsigned int)_tmp122 > 16?*((
int*)_tmp122)== 1: 0){_LLB8: _tmp124=((struct Cyc_Absyn_Aligned_att_struct*)_tmp122)->f1;
goto _LL9A;}else{goto _LL9B;}_LL9B: if((int)_tmp122 == 5){goto _LL9C;}else{goto _LL9D;}
_LL9D: if((unsigned int)_tmp122 > 16?*((int*)_tmp122)== 2: 0){_LLB9: _tmp125=((
struct Cyc_Absyn_Section_att_struct*)_tmp122)->f1;goto _LL9E;}else{goto _LL9F;}
_LL9F: if((int)_tmp122 == 6){goto _LLA0;}else{goto _LLA1;}_LLA1: if((int)_tmp122 == 7){
goto _LLA2;}else{goto _LLA3;}_LLA3: if((int)_tmp122 == 8){goto _LLA4;}else{goto _LLA5;}
_LLA5: if((int)_tmp122 == 9){goto _LLA6;}else{goto _LLA7;}_LLA7: if((int)_tmp122 == 10){
goto _LLA8;}else{goto _LLA9;}_LLA9: if((int)_tmp122 == 11){goto _LLAA;}else{goto _LLAB;}
_LLAB: if((int)_tmp122 == 12){goto _LLAC;}else{goto _LLAD;}_LLAD: if((int)_tmp122 == 
13){goto _LLAE;}else{goto _LLAF;}_LLAF: if((int)_tmp122 == 14){goto _LLB0;}else{goto
_LLB1;}_LLB1: if((int)_tmp122 == 15){goto _LLB2;}else{goto _LLB3;}_LLB3: if((
unsigned int)_tmp122 > 16?*((int*)_tmp122)== 3: 0){_LLBC: _tmp128=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp122)->f1;if((int)_tmp128 == 0){goto _LLBB;}else{
goto _LLB5;}_LLBB: _tmp127=((struct Cyc_Absyn_Format_att_struct*)_tmp122)->f2;goto
_LLBA;_LLBA: _tmp126=((struct Cyc_Absyn_Format_att_struct*)_tmp122)->f3;goto _LLB4;}
else{goto _LLB5;}_LLB5: if((unsigned int)_tmp122 > 16?*((int*)_tmp122)== 3: 0){_LLBF:
_tmp12B=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp122)->f1;if((int)_tmp12B
== 1){goto _LLBE;}else{goto _LL8C;}_LLBE: _tmp12A=((struct Cyc_Absyn_Format_att_struct*)
_tmp122)->f2;goto _LLBD;_LLBD: _tmp129=((struct Cyc_Absyn_Format_att_struct*)
_tmp122)->f3;goto _LLB6;}else{goto _LL8C;}_LL8E: return(struct _tagged_arr)({struct
Cyc_Std_Int_pa_struct _tmp12D;_tmp12D.tag=1;_tmp12D.f1=(int)((unsigned int)
_tmp123);{void*_tmp12C[1]={& _tmp12D};Cyc_Std_aprintf(_tag_arr("regparm(%d)",
sizeof(unsigned char),12),_tag_arr(_tmp12C,sizeof(void*),1));}});_LL90: return
_tag_arr("stdcall",sizeof(unsigned char),8);_LL92: return _tag_arr("cdecl",sizeof(
unsigned char),6);_LL94: return _tag_arr("fastcall",sizeof(unsigned char),9);_LL96:
return _tag_arr("noreturn",sizeof(unsigned char),9);_LL98: return _tag_arr("const",
sizeof(unsigned char),6);_LL9A: if(_tmp124 == - 1){return _tag_arr("aligned",sizeof(
unsigned char),8);}else{return(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp12F;_tmp12F.tag=1;_tmp12F.f1=(int)((unsigned int)_tmp124);{void*_tmp12E[1]={&
_tmp12F};Cyc_Std_aprintf(_tag_arr("aligned(%d)",sizeof(unsigned char),12),
_tag_arr(_tmp12E,sizeof(void*),1));}});}_LL9C: return _tag_arr("packed",sizeof(
unsigned char),7);_LL9E: return(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp131;_tmp131.tag=0;_tmp131.f1=(struct _tagged_arr)_tmp125;{void*_tmp130[1]={&
_tmp131};Cyc_Std_aprintf(_tag_arr("section(\"%s\")",sizeof(unsigned char),14),
_tag_arr(_tmp130,sizeof(void*),1));}});_LLA0: return _tag_arr("nocommon",sizeof(
unsigned char),9);_LLA2: return _tag_arr("shared",sizeof(unsigned char),7);_LLA4:
return _tag_arr("unused",sizeof(unsigned char),7);_LLA6: return _tag_arr("weak",
sizeof(unsigned char),5);_LLA8: return _tag_arr("dllimport",sizeof(unsigned char),
10);_LLAA: return _tag_arr("dllexport",sizeof(unsigned char),10);_LLAC: return
_tag_arr("no_instrument_function",sizeof(unsigned char),23);_LLAE: return _tag_arr("constructor",
sizeof(unsigned char),12);_LLB0: return _tag_arr("destructor",sizeof(unsigned char),
11);_LLB2: return _tag_arr("no_check_memory_usage",sizeof(unsigned char),22);_LLB4:
return(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp134;_tmp134.tag=1;
_tmp134.f1=(unsigned int)_tmp126;{struct Cyc_Std_Int_pa_struct _tmp133;_tmp133.tag=
1;_tmp133.f1=(unsigned int)_tmp127;{void*_tmp132[2]={& _tmp133,& _tmp134};Cyc_Std_aprintf(
_tag_arr("format(printf,%u,%u)",sizeof(unsigned char),21),_tag_arr(_tmp132,
sizeof(void*),2));}}});_LLB6: return(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp137;_tmp137.tag=1;_tmp137.f1=(unsigned int)_tmp129;{struct Cyc_Std_Int_pa_struct
_tmp136;_tmp136.tag=1;_tmp136.f1=(unsigned int)_tmp12A;{void*_tmp135[2]={&
_tmp136,& _tmp137};Cyc_Std_aprintf(_tag_arr("format(scanf,%u,%u)",sizeof(
unsigned char),20),_tag_arr(_tmp135,sizeof(void*),2));}}});_LL8C:;}int Cyc_Absyn_fntype_att(
void*a){void*_tmp138=a;_LLC1: if((unsigned int)_tmp138 > 16?*((int*)_tmp138)== 0: 0){
goto _LLC2;}else{goto _LLC3;}_LLC3: if((int)_tmp138 == 2){goto _LLC4;}else{goto _LLC5;}
_LLC5: if((int)_tmp138 == 0){goto _LLC6;}else{goto _LLC7;}_LLC7: if((int)_tmp138 == 1){
goto _LLC8;}else{goto _LLC9;}_LLC9: if((int)_tmp138 == 3){goto _LLCA;}else{goto _LLCB;}
_LLCB: if((unsigned int)_tmp138 > 16?*((int*)_tmp138)== 3: 0){goto _LLCC;}else{goto
_LLCD;}_LLCD: if((int)_tmp138 == 4){goto _LLCE;}else{goto _LLCF;}_LLCF: goto _LLD0;
_LLC2: goto _LLC4;_LLC4: goto _LLC6;_LLC6: goto _LLC8;_LLC8: goto _LLCA;_LLCA: goto _LLCC;
_LLCC: goto _LLCE;_LLCE: return 1;_LLD0: return 0;_LLC0:;}static unsigned char _tmp139[3]="f0";
static struct _tagged_arr Cyc_Absyn_f0={_tmp139,_tmp139,_tmp139 + 3};static struct
_tagged_arr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};static struct _tagged_arr
Cyc_Absyn_field_names={(void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v),(
void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v),(void*)((struct _tagged_arr**)
Cyc_Absyn_field_names_v + 1)};struct _tagged_arr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_arr_size(Cyc_Absyn_field_names,sizeof(struct _tagged_arr*));
if(i >= fsz){Cyc_Absyn_field_names=({unsigned int _tmp13A=(unsigned int)(i + 1);
struct _tagged_arr**_tmp13B=(struct _tagged_arr**)_cycalloc(_check_times(sizeof(
struct _tagged_arr*),_tmp13A));struct _tagged_arr _tmp140=_tag_arr(_tmp13B,sizeof(
struct _tagged_arr*),(unsigned int)(i + 1));{unsigned int _tmp13C=_tmp13A;
unsigned int j;for(j=0;j < _tmp13C;j ++){_tmp13B[j]=j < fsz?*((struct _tagged_arr**)
_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct _tagged_arr*),(int)j)):({
struct _tagged_arr*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D[0]=(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp13F;_tmp13F.tag=1;_tmp13F.f1=(int)j;{
void*_tmp13E[1]={& _tmp13F};Cyc_Std_aprintf(_tag_arr("f%d",sizeof(unsigned char),
4),_tag_arr(_tmp13E,sizeof(void*),1));}});_tmp13D;});}};_tmp140;});}return*((
struct _tagged_arr**)_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*),i));}void Cyc_Absyn_print_decls(struct Cyc_List_List*decls){((void(*)(
void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(Cyc_decls_rep,& decls);({
void*_tmp141[0]={};Cyc_Std_printf(_tag_arr("\n",sizeof(unsigned char),2),
_tag_arr(_tmp141,sizeof(void*),0));});}extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_381;static struct Cyc_Typerep_Int_struct
Cyc__genrep_5={0,0,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep;extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;static unsigned char _tmp143[7]="Static";
static struct _tuple6 Cyc__gentuple_135={0,{_tmp143,_tmp143,_tmp143 + 7}};static
unsigned char _tmp144[9]="Abstract";static struct _tuple6 Cyc__gentuple_136={1,{
_tmp144,_tmp144,_tmp144 + 9}};static unsigned char _tmp145[7]="Public";static struct
_tuple6 Cyc__gentuple_137={2,{_tmp145,_tmp145,_tmp145 + 7}};static unsigned char
_tmp146[7]="Extern";static struct _tuple6 Cyc__gentuple_138={3,{_tmp146,_tmp146,
_tmp146 + 7}};static unsigned char _tmp147[8]="ExternC";static struct _tuple6 Cyc__gentuple_139={
4,{_tmp147,_tmp147,_tmp147 + 8}};static struct _tuple6*Cyc__genarr_140[5]={& Cyc__gentuple_135,&
Cyc__gentuple_136,& Cyc__gentuple_137,& Cyc__gentuple_138,& Cyc__gentuple_139};
static struct _tuple4*Cyc__genarr_141[0]={};static unsigned char _tmp149[6]="Scope";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep={5,{_tmp149,_tmp149,_tmp149
+ 6},{(void*)((struct _tuple6**)Cyc__genarr_140),(void*)((struct _tuple6**)Cyc__genarr_140),(
void*)((struct _tuple6**)Cyc__genarr_140 + 5)},{(void*)((struct _tuple4**)Cyc__genarr_141),(
void*)((struct _tuple4**)Cyc__genarr_141),(void*)((struct _tuple4**)Cyc__genarr_141
+ 0)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_11;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_17;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_18;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12;extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_13;static struct Cyc_Typerep_Int_struct Cyc__genrep_14={0,0,8};static
struct Cyc_Typerep_FatPtr_struct Cyc__genrep_13={2,(void*)((void*)& Cyc__genrep_14)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12={1,1,(void*)((void*)& Cyc__genrep_13)};
static unsigned char _tmp14D[5]="List";static struct _tagged_arr Cyc__genname_22={
_tmp14D,_tmp14D,_tmp14D + 5};static unsigned char _tmp14E[3]="hd";static struct
_tuple4 Cyc__gentuple_19={offsetof(struct Cyc_List_List,hd),{_tmp14E,_tmp14E,
_tmp14E + 3},(void*)& Cyc__genrep_12};static unsigned char _tmp14F[3]="tl";static
struct _tuple4 Cyc__gentuple_20={offsetof(struct Cyc_List_List,tl),{_tmp14F,_tmp14F,
_tmp14F + 3},(void*)& Cyc__genrep_18};static struct _tuple4*Cyc__genarr_21[2]={& Cyc__gentuple_19,&
Cyc__gentuple_20};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_22,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_21),(void*)((struct _tuple4**)Cyc__genarr_21),(void*)((
struct _tuple4**)Cyc__genarr_21 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_18={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};struct _tuple10{
unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple5 Cyc__gentuple_23={
offsetof(struct _tuple10,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_24={
offsetof(struct _tuple10,f2),(void*)& Cyc__genrep_18};static struct _tuple5*Cyc__genarr_25[
2]={& Cyc__gentuple_23,& Cyc__gentuple_24};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_17={
4,sizeof(struct _tuple10),{(void*)((struct _tuple5**)Cyc__genarr_25),(void*)((
struct _tuple5**)Cyc__genarr_25),(void*)((struct _tuple5**)Cyc__genarr_25 + 2)}};
static unsigned char _tmp153[6]="Loc_n";static struct _tuple6 Cyc__gentuple_15={0,{
_tmp153,_tmp153,_tmp153 + 6}};static struct _tuple6*Cyc__genarr_16[1]={& Cyc__gentuple_15};
static unsigned char _tmp154[6]="Rel_n";static struct _tuple4 Cyc__gentuple_26={0,{
_tmp154,_tmp154,_tmp154 + 6},(void*)& Cyc__genrep_17};static unsigned char _tmp155[6]="Abs_n";
static struct _tuple4 Cyc__gentuple_27={1,{_tmp155,_tmp155,_tmp155 + 6},(void*)& Cyc__genrep_17};
static struct _tuple4*Cyc__genarr_28[2]={& Cyc__gentuple_26,& Cyc__gentuple_27};
static unsigned char _tmp157[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={
5,{_tmp157,_tmp157,_tmp157 + 8},{(void*)((struct _tuple6**)Cyc__genarr_16),(void*)((
struct _tuple6**)Cyc__genarr_16),(void*)((struct _tuple6**)Cyc__genarr_16 + 1)},{(
void*)((struct _tuple4**)Cyc__genarr_28),(void*)((struct _tuple4**)Cyc__genarr_28),(
void*)((struct _tuple4**)Cyc__genarr_28 + 2)}};static struct _tuple5 Cyc__gentuple_29={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple5
Cyc__gentuple_30={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_12};static
struct _tuple5*Cyc__genarr_31[2]={& Cyc__gentuple_29,& Cyc__gentuple_30};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_11={4,sizeof(struct _tuple0),{(void*)((
struct _tuple5**)Cyc__genarr_31),(void*)((struct _tuple5**)Cyc__genarr_31),(void*)((
struct _tuple5**)Cyc__genarr_31 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10={
1,1,(void*)((void*)& Cyc__genrep_11)};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_132;
struct _tuple11{unsigned char f1;};static struct _tuple5 Cyc__gentuple_133={offsetof(
struct _tuple11,f1),(void*)((void*)& Cyc__genrep_14)};static struct _tuple5*Cyc__genarr_134[
1]={& Cyc__gentuple_133};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_132={4,
sizeof(struct _tuple11),{(void*)((struct _tuple5**)Cyc__genarr_134),(void*)((
struct _tuple5**)Cyc__genarr_134),(void*)((struct _tuple5**)Cyc__genarr_134 + 1)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1074;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1079;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static unsigned char _tmp15B[8]="AnyKind";
static struct _tuple6 Cyc__gentuple_185={0,{_tmp15B,_tmp15B,_tmp15B + 8}};static
unsigned char _tmp15C[8]="MemKind";static struct _tuple6 Cyc__gentuple_186={1,{
_tmp15C,_tmp15C,_tmp15C + 8}};static unsigned char _tmp15D[8]="BoxKind";static
struct _tuple6 Cyc__gentuple_187={2,{_tmp15D,_tmp15D,_tmp15D + 8}};static
unsigned char _tmp15E[8]="RgnKind";static struct _tuple6 Cyc__gentuple_188={3,{
_tmp15E,_tmp15E,_tmp15E + 8}};static unsigned char _tmp15F[8]="EffKind";static
struct _tuple6 Cyc__gentuple_189={4,{_tmp15F,_tmp15F,_tmp15F + 8}};static struct
_tuple6*Cyc__genarr_190[5]={& Cyc__gentuple_185,& Cyc__gentuple_186,& Cyc__gentuple_187,&
Cyc__gentuple_188,& Cyc__gentuple_189};static struct _tuple4*Cyc__genarr_191[0]={};
static unsigned char _tmp161[5]="Kind";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep={
5,{_tmp161,_tmp161,_tmp161 + 5},{(void*)((struct _tuple6**)Cyc__genarr_190),(void*)((
struct _tuple6**)Cyc__genarr_190),(void*)((struct _tuple6**)Cyc__genarr_190 + 5)},{(
void*)((struct _tuple4**)Cyc__genarr_191),(void*)((struct _tuple4**)Cyc__genarr_191),(
void*)((struct _tuple4**)Cyc__genarr_191 + 0)}};static unsigned char _tmp162[4]="Opt";
static struct _tagged_arr Cyc__genname_1082={_tmp162,_tmp162,_tmp162 + 4};static
unsigned char _tmp163[2]="v";static struct _tuple4 Cyc__gentuple_1080={offsetof(
struct Cyc_Core_Opt,v),{_tmp163,_tmp163,_tmp163 + 2},(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple4*Cyc__genarr_1081[1]={& Cyc__gentuple_1080};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kind_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_1082,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple4**)Cyc__genarr_1081),(void*)((
struct _tuple4**)Cyc__genarr_1081),(void*)((struct _tuple4**)Cyc__genarr_1081 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1079={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep;static unsigned char _tmp166[4]="Opt";static
struct _tagged_arr Cyc__genname_64={_tmp166,_tmp166,_tmp166 + 4};static
unsigned char _tmp167[2]="v";static struct _tuple4 Cyc__gentuple_62={offsetof(struct
Cyc_Core_Opt,v),{_tmp167,_tmp167,_tmp167 + 2},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple4*Cyc__genarr_63[1]={& Cyc__gentuple_62};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_64,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple4**)Cyc__genarr_63),(void*)((
struct _tuple4**)Cyc__genarr_63),(void*)((struct _tuple4**)Cyc__genarr_63 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};
static struct Cyc_Typerep_Int_struct Cyc__genrep_102={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1075;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_254;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_182;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_212;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_212={1,1,(void*)((void*)& Cyc__genrep_102)};extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_204;
static struct _tuple5 Cyc__gentuple_205={offsetof(struct _tuple5,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_206={offsetof(struct _tuple5,f2),(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple5*Cyc__genarr_207[2]={& Cyc__gentuple_205,& Cyc__gentuple_206};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_204={4,sizeof(struct _tuple5),{(
void*)((struct _tuple5**)Cyc__genarr_207),(void*)((struct _tuple5**)Cyc__genarr_207),(
void*)((struct _tuple5**)Cyc__genarr_207 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_200;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_192;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;static
unsigned char _tmp16D[4]="Opt";static struct _tagged_arr Cyc__genname_195={_tmp16D,
_tmp16D,_tmp16D + 4};static unsigned char _tmp16E[2]="v";static struct _tuple4 Cyc__gentuple_193={
offsetof(struct Cyc_Core_Opt,v),{_tmp16E,_tmp16E,_tmp16E + 2},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple4*Cyc__genarr_194[1]={& Cyc__gentuple_193};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_195,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple4**)Cyc__genarr_194),(void*)((
struct _tuple4**)Cyc__genarr_194),(void*)((struct _tuple4**)Cyc__genarr_194 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_192={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple12{unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple5 Cyc__gentuple_201={
offsetof(struct _tuple12,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_202={
offsetof(struct _tuple12,f2),(void*)& Cyc__genrep_192};static struct _tuple5*Cyc__genarr_203[
2]={& Cyc__gentuple_201,& Cyc__gentuple_202};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_200={4,sizeof(struct _tuple12),{(void*)((struct _tuple5**)Cyc__genarr_203),(
void*)((struct _tuple5**)Cyc__genarr_203),(void*)((struct _tuple5**)Cyc__genarr_203
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_184;struct _tuple13{
unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};static struct _tuple5 Cyc__gentuple_196={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_197={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_192};static struct _tuple5 Cyc__gentuple_198={
offsetof(struct _tuple13,f3),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple5*
Cyc__genarr_199[3]={& Cyc__gentuple_196,& Cyc__gentuple_197,& Cyc__gentuple_198};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_184={4,sizeof(struct _tuple13),{(
void*)((struct _tuple5**)Cyc__genarr_199),(void*)((struct _tuple5**)Cyc__genarr_199),(
void*)((struct _tuple5**)Cyc__genarr_199 + 3)}};static struct _tuple6*Cyc__genarr_183[
0]={};static unsigned char _tmp173[6]="Eq_kb";static struct _tuple4 Cyc__gentuple_208={
0,{_tmp173,_tmp173,_tmp173 + 6},(void*)& Cyc__genrep_204};static unsigned char
_tmp174[11]="Unknown_kb";static struct _tuple4 Cyc__gentuple_209={1,{_tmp174,
_tmp174,_tmp174 + 11},(void*)& Cyc__genrep_200};static unsigned char _tmp175[8]="Less_kb";
static struct _tuple4 Cyc__gentuple_210={2,{_tmp175,_tmp175,_tmp175 + 8},(void*)& Cyc__genrep_184};
static struct _tuple4*Cyc__genarr_211[3]={& Cyc__gentuple_208,& Cyc__gentuple_209,&
Cyc__gentuple_210};static unsigned char _tmp177[10]="KindBound";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kindbound_t_rep={5,{_tmp177,_tmp177,_tmp177 + 10},{(void*)((struct
_tuple6**)Cyc__genarr_183),(void*)((struct _tuple6**)Cyc__genarr_183),(void*)((
struct _tuple6**)Cyc__genarr_183 + 0)},{(void*)((struct _tuple4**)Cyc__genarr_211),(
void*)((struct _tuple4**)Cyc__genarr_211),(void*)((struct _tuple4**)Cyc__genarr_211
+ 3)}};static unsigned char _tmp178[5]="Tvar";static struct _tagged_arr Cyc__genname_217={
_tmp178,_tmp178,_tmp178 + 5};static unsigned char _tmp179[5]="name";static struct
_tuple4 Cyc__gentuple_213={offsetof(struct Cyc_Absyn_Tvar,name),{_tmp179,_tmp179,
_tmp179 + 5},(void*)& Cyc__genrep_12};static unsigned char _tmp17A[9]="identity";
static struct _tuple4 Cyc__gentuple_214={offsetof(struct Cyc_Absyn_Tvar,identity),{
_tmp17A,_tmp17A,_tmp17A + 9},(void*)& Cyc__genrep_212};static unsigned char _tmp17B[
5]="kind";static struct _tuple4 Cyc__gentuple_215={offsetof(struct Cyc_Absyn_Tvar,kind),{
_tmp17B,_tmp17B,_tmp17B + 5},(void*)& Cyc_Absyn_kindbound_t_rep};static struct
_tuple4*Cyc__genarr_216[3]={& Cyc__gentuple_213,& Cyc__gentuple_214,& Cyc__gentuple_215};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={3,(struct _tagged_arr*)&
Cyc__genname_217,sizeof(struct Cyc_Absyn_Tvar),{(void*)((struct _tuple4**)Cyc__genarr_216),(
void*)((struct _tuple4**)Cyc__genarr_216),(void*)((struct _tuple4**)Cyc__genarr_216
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_182={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tvar_rep)};static unsigned char _tmp17E[5]="List";static struct
_tagged_arr Cyc__genname_258={_tmp17E,_tmp17E,_tmp17E + 5};static unsigned char
_tmp17F[3]="hd";static struct _tuple4 Cyc__gentuple_255={offsetof(struct Cyc_List_List,hd),{
_tmp17F,_tmp17F,_tmp17F + 3},(void*)& Cyc__genrep_182};static unsigned char _tmp180[
3]="tl";static struct _tuple4 Cyc__gentuple_256={offsetof(struct Cyc_List_List,tl),{
_tmp180,_tmp180,_tmp180 + 3},(void*)& Cyc__genrep_254};static struct _tuple4*Cyc__genarr_257[
2]={& Cyc__gentuple_255,& Cyc__gentuple_256};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_258,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_257),(void*)((struct _tuple4**)Cyc__genarr_257),(void*)((
struct _tuple4**)Cyc__genarr_257 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_254={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static
unsigned char _tmp183[4]="Opt";static struct _tagged_arr Cyc__genname_1078={_tmp183,
_tmp183,_tmp183 + 4};static unsigned char _tmp184[2]="v";static struct _tuple4 Cyc__gentuple_1076={
offsetof(struct Cyc_Core_Opt,v),{_tmp184,_tmp184,_tmp184 + 2},(void*)& Cyc__genrep_254};
static struct _tuple4*Cyc__genarr_1077[1]={& Cyc__gentuple_1076};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_1078,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple4**)Cyc__genarr_1077),(
void*)((struct _tuple4**)Cyc__genarr_1077),(void*)((struct _tuple4**)Cyc__genarr_1077
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1075={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};struct _tuple14{
unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;struct Cyc_Core_Opt*
f5;};static struct _tuple5 Cyc__gentuple_1083={offsetof(struct _tuple14,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_1084={offsetof(struct _tuple14,f2),(
void*)& Cyc__genrep_1079};static struct _tuple5 Cyc__gentuple_1085={offsetof(struct
_tuple14,f3),(void*)& Cyc__genrep_61};static struct _tuple5 Cyc__gentuple_1086={
offsetof(struct _tuple14,f4),(void*)((void*)& Cyc__genrep_102)};static struct
_tuple5 Cyc__gentuple_1087={offsetof(struct _tuple14,f5),(void*)& Cyc__genrep_1075};
static struct _tuple5*Cyc__genarr_1088[5]={& Cyc__gentuple_1083,& Cyc__gentuple_1084,&
Cyc__gentuple_1085,& Cyc__gentuple_1086,& Cyc__gentuple_1087};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1074={4,sizeof(struct _tuple14),{(void*)((struct _tuple5**)Cyc__genarr_1088),(
void*)((struct _tuple5**)Cyc__genarr_1088),(void*)((struct _tuple5**)Cyc__genarr_1088
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1070;struct _tuple15{
unsigned int f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple5 Cyc__gentuple_1071={
offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_1072={
offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_182};static struct _tuple5*Cyc__genarr_1073[
2]={& Cyc__gentuple_1071,& Cyc__gentuple_1072};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1070={4,sizeof(struct _tuple15),{(void*)((struct _tuple5**)Cyc__genarr_1073),(
void*)((struct _tuple5**)Cyc__genarr_1073),(void*)((struct _tuple5**)Cyc__genarr_1073
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1044;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1051;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static unsigned char _tmp189[18]="UnknownTunionInfo";
static struct _tagged_arr Cyc__genname_1055={_tmp189,_tmp189,_tmp189 + 18};static
unsigned char _tmp18A[5]="name";static struct _tuple4 Cyc__gentuple_1052={offsetof(
struct Cyc_Absyn_UnknownTunionInfo,name),{_tmp18A,_tmp18A,_tmp18A + 5},(void*)& Cyc__genrep_10};
static unsigned char _tmp18B[11]="is_xtunion";static struct _tuple4 Cyc__gentuple_1053={
offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),{_tmp18B,_tmp18B,_tmp18B + 
11},(void*)((void*)& Cyc__genrep_102)};static struct _tuple4*Cyc__genarr_1054[2]={&
Cyc__gentuple_1052,& Cyc__gentuple_1053};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionInfo_rep={
3,(struct _tagged_arr*)& Cyc__genname_1055,sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(
void*)((struct _tuple4**)Cyc__genarr_1054),(void*)((struct _tuple4**)Cyc__genarr_1054),(
void*)((struct _tuple4**)Cyc__genarr_1054 + 2)}};struct _tuple16{unsigned int f1;
struct Cyc_Absyn_UnknownTunionInfo f2;};static struct _tuple5 Cyc__gentuple_1056={
offsetof(struct _tuple16,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_1057={
offsetof(struct _tuple16,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple5*Cyc__genarr_1058[2]={& Cyc__gentuple_1056,& Cyc__gentuple_1057};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1051={4,sizeof(struct _tuple16),{(
void*)((struct _tuple5**)Cyc__genarr_1058),(void*)((struct _tuple5**)Cyc__genarr_1058),(
void*)((struct _tuple5**)Cyc__genarr_1058 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1046;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1047;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_318;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_319;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_320;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_300;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_301;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_302;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_303;static struct
_tuple5 Cyc__gentuple_304={offsetof(struct _tuple3,f1),(void*)& Cyc__genrep_132};
static struct _tuple5 Cyc__gentuple_305={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple5*Cyc__genarr_306[2]={& Cyc__gentuple_304,&
Cyc__gentuple_305};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_303={4,
sizeof(struct _tuple3),{(void*)((struct _tuple5**)Cyc__genarr_306),(void*)((struct
_tuple5**)Cyc__genarr_306),(void*)((struct _tuple5**)Cyc__genarr_306 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_302={1,1,(void*)((void*)& Cyc__genrep_303)};
static unsigned char _tmp190[5]="List";static struct _tagged_arr Cyc__genname_310={
_tmp190,_tmp190,_tmp190 + 5};static unsigned char _tmp191[3]="hd";static struct
_tuple4 Cyc__gentuple_307={offsetof(struct Cyc_List_List,hd),{_tmp191,_tmp191,
_tmp191 + 3},(void*)& Cyc__genrep_302};static unsigned char _tmp192[3]="tl";static
struct _tuple4 Cyc__gentuple_308={offsetof(struct Cyc_List_List,tl),{_tmp192,
_tmp192,_tmp192 + 3},(void*)& Cyc__genrep_301};static struct _tuple4*Cyc__genarr_309[
2]={& Cyc__gentuple_307,& Cyc__gentuple_308};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_310,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_309),(void*)((struct _tuple4**)Cyc__genarr_309),(void*)((
struct _tuple4**)Cyc__genarr_309 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_301={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_2={1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};static
unsigned char _tmp196[12]="Tunionfield";static struct _tagged_arr Cyc__genname_317={
_tmp196,_tmp196,_tmp196 + 12};static unsigned char _tmp197[5]="name";static struct
_tuple4 Cyc__gentuple_311={offsetof(struct Cyc_Absyn_Tunionfield,name),{_tmp197,
_tmp197,_tmp197 + 5},(void*)& Cyc__genrep_10};static unsigned char _tmp198[4]="tvs";
static struct _tuple4 Cyc__gentuple_312={offsetof(struct Cyc_Absyn_Tunionfield,tvs),{
_tmp198,_tmp198,_tmp198 + 4},(void*)& Cyc__genrep_254};static unsigned char _tmp199[
5]="typs";static struct _tuple4 Cyc__gentuple_313={offsetof(struct Cyc_Absyn_Tunionfield,typs),{
_tmp199,_tmp199,_tmp199 + 5},(void*)& Cyc__genrep_301};static unsigned char _tmp19A[
4]="loc";static struct _tuple4 Cyc__gentuple_314={offsetof(struct Cyc_Absyn_Tunionfield,loc),{
_tmp19A,_tmp19A,_tmp19A + 4},(void*)& Cyc__genrep_2};static unsigned char _tmp19B[3]="sc";
static struct _tuple4 Cyc__gentuple_315={offsetof(struct Cyc_Absyn_Tunionfield,sc),{
_tmp19B,_tmp19B,_tmp19B + 3},(void*)& Cyc_Absyn_scope_t_rep};static struct _tuple4*
Cyc__genarr_316[5]={& Cyc__gentuple_311,& Cyc__gentuple_312,& Cyc__gentuple_313,&
Cyc__gentuple_314,& Cyc__gentuple_315};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_317,sizeof(struct Cyc_Absyn_Tunionfield),{(
void*)((struct _tuple4**)Cyc__genarr_316),(void*)((struct _tuple4**)Cyc__genarr_316),(
void*)((struct _tuple4**)Cyc__genarr_316 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_300={1,1,(void*)((void*)& Cyc_struct_Absyn_Tunionfield_rep)};static
unsigned char _tmp19E[5]="List";static struct _tagged_arr Cyc__genname_324={_tmp19E,
_tmp19E,_tmp19E + 5};static unsigned char _tmp19F[3]="hd";static struct _tuple4 Cyc__gentuple_321={
offsetof(struct Cyc_List_List,hd),{_tmp19F,_tmp19F,_tmp19F + 3},(void*)& Cyc__genrep_300};
static unsigned char _tmp1A0[3]="tl";static struct _tuple4 Cyc__gentuple_322={
offsetof(struct Cyc_List_List,tl),{_tmp1A0,_tmp1A0,_tmp1A0 + 3},(void*)& Cyc__genrep_320};
static struct _tuple4*Cyc__genarr_323[2]={& Cyc__gentuple_321,& Cyc__gentuple_322};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_324,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_323),(void*)((struct _tuple4**)Cyc__genarr_323),(void*)((
struct _tuple4**)Cyc__genarr_323 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_320={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};static
unsigned char _tmp1A3[4]="Opt";static struct _tagged_arr Cyc__genname_327={_tmp1A3,
_tmp1A3,_tmp1A3 + 4};static unsigned char _tmp1A4[2]="v";static struct _tuple4 Cyc__gentuple_325={
offsetof(struct Cyc_Core_Opt,v),{_tmp1A4,_tmp1A4,_tmp1A4 + 2},(void*)& Cyc__genrep_320};
static struct _tuple4*Cyc__genarr_326[1]={& Cyc__gentuple_325};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={3,(struct
_tagged_arr*)& Cyc__genname_327,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple4**)Cyc__genarr_326),(void*)((struct _tuple4**)Cyc__genarr_326),(void*)((
struct _tuple4**)Cyc__genarr_326 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_319={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static unsigned char _tmp1A7[11]="Tuniondecl";static struct _tagged_arr Cyc__genname_334={
_tmp1A7,_tmp1A7,_tmp1A7 + 11};static unsigned char _tmp1A8[3]="sc";static struct
_tuple4 Cyc__gentuple_328={offsetof(struct Cyc_Absyn_Tuniondecl,sc),{_tmp1A8,
_tmp1A8,_tmp1A8 + 3},(void*)& Cyc_Absyn_scope_t_rep};static unsigned char _tmp1A9[5]="name";
static struct _tuple4 Cyc__gentuple_329={offsetof(struct Cyc_Absyn_Tuniondecl,name),{
_tmp1A9,_tmp1A9,_tmp1A9 + 5},(void*)& Cyc__genrep_10};static unsigned char _tmp1AA[4]="tvs";
static struct _tuple4 Cyc__gentuple_330={offsetof(struct Cyc_Absyn_Tuniondecl,tvs),{
_tmp1AA,_tmp1AA,_tmp1AA + 4},(void*)& Cyc__genrep_254};static unsigned char _tmp1AB[
7]="fields";static struct _tuple4 Cyc__gentuple_331={offsetof(struct Cyc_Absyn_Tuniondecl,fields),{
_tmp1AB,_tmp1AB,_tmp1AB + 7},(void*)& Cyc__genrep_319};static unsigned char _tmp1AC[
11]="is_xtunion";static struct _tuple4 Cyc__gentuple_332={offsetof(struct Cyc_Absyn_Tuniondecl,is_xtunion),{
_tmp1AC,_tmp1AC,_tmp1AC + 11},(void*)((void*)& Cyc__genrep_102)};static struct
_tuple4*Cyc__genarr_333[5]={& Cyc__gentuple_328,& Cyc__gentuple_329,& Cyc__gentuple_330,&
Cyc__gentuple_331,& Cyc__gentuple_332};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_334,sizeof(struct Cyc_Absyn_Tuniondecl),{(
void*)((struct _tuple4**)Cyc__genarr_333),(void*)((struct _tuple4**)Cyc__genarr_333),(
void*)((struct _tuple4**)Cyc__genarr_333 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_318={1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1047={1,1,(void*)((void*)& Cyc__genrep_318)};
struct _tuple17{unsigned int f1;struct Cyc_Absyn_Tuniondecl**f2;};static struct
_tuple5 Cyc__gentuple_1048={offsetof(struct _tuple17,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_1049={offsetof(struct _tuple17,f2),(void*)& Cyc__genrep_1047};
static struct _tuple5*Cyc__genarr_1050[2]={& Cyc__gentuple_1048,& Cyc__gentuple_1049};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1046={4,sizeof(struct _tuple17),{(
void*)((struct _tuple5**)Cyc__genarr_1050),(void*)((struct _tuple5**)Cyc__genarr_1050),(
void*)((struct _tuple5**)Cyc__genarr_1050 + 2)}};static struct _tuple6*Cyc__genarr_1045[
0]={};static unsigned char _tmp1B1[14]="UnknownTunion";static struct _tuple4 Cyc__gentuple_1059={
0,{_tmp1B1,_tmp1B1,_tmp1B1 + 14},(void*)& Cyc__genrep_1051};static unsigned char
_tmp1B2[12]="KnownTunion";static struct _tuple4 Cyc__gentuple_1060={1,{_tmp1B2,
_tmp1B2,_tmp1B2 + 12},(void*)& Cyc__genrep_1046};static struct _tuple4*Cyc__genarr_1061[
2]={& Cyc__gentuple_1059,& Cyc__gentuple_1060};static unsigned char _tmp1B4[12]="TunionInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep={5,{_tmp1B4,
_tmp1B4,_tmp1B4 + 12},{(void*)((struct _tuple6**)Cyc__genarr_1045),(void*)((struct
_tuple6**)Cyc__genarr_1045),(void*)((struct _tuple6**)Cyc__genarr_1045 + 0)},{(
void*)((struct _tuple4**)Cyc__genarr_1061),(void*)((struct _tuple4**)Cyc__genarr_1061),(
void*)((struct _tuple4**)Cyc__genarr_1061 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_52;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep;
static unsigned char _tmp1B5[5]="List";static struct _tagged_arr Cyc__genname_56={
_tmp1B5,_tmp1B5,_tmp1B5 + 5};static unsigned char _tmp1B6[3]="hd";static struct
_tuple4 Cyc__gentuple_53={offsetof(struct Cyc_List_List,hd),{_tmp1B6,_tmp1B6,
_tmp1B6 + 3},(void*)((void*)& Cyc_Absyn_type_t_rep)};static unsigned char _tmp1B7[3]="tl";
static struct _tuple4 Cyc__gentuple_54={offsetof(struct Cyc_List_List,tl),{_tmp1B7,
_tmp1B7,_tmp1B7 + 3},(void*)& Cyc__genrep_52};static struct _tuple4*Cyc__genarr_55[2]={&
Cyc__gentuple_53,& Cyc__gentuple_54};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_56,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_55),(void*)((struct _tuple4**)Cyc__genarr_55),(void*)((
struct _tuple4**)Cyc__genarr_55 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_52={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};static
unsigned char _tmp1BA[11]="TunionInfo";static struct _tagged_arr Cyc__genname_1066={
_tmp1BA,_tmp1BA,_tmp1BA + 11};static unsigned char _tmp1BB[12]="tunion_info";static
struct _tuple4 Cyc__gentuple_1062={offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),{
_tmp1BB,_tmp1BB,_tmp1BB + 12},(void*)& Cyc_tunion_Absyn_TunionInfoU_rep};static
unsigned char _tmp1BC[6]="targs";static struct _tuple4 Cyc__gentuple_1063={offsetof(
struct Cyc_Absyn_TunionInfo,targs),{_tmp1BC,_tmp1BC,_tmp1BC + 6},(void*)& Cyc__genrep_52};
static unsigned char _tmp1BD[4]="rgn";static struct _tuple4 Cyc__gentuple_1064={
offsetof(struct Cyc_Absyn_TunionInfo,rgn),{_tmp1BD,_tmp1BD,_tmp1BD + 4},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static struct _tuple4*Cyc__genarr_1065[3]={& Cyc__gentuple_1062,&
Cyc__gentuple_1063,& Cyc__gentuple_1064};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_1066,sizeof(struct Cyc_Absyn_TunionInfo),{(
void*)((struct _tuple4**)Cyc__genarr_1065),(void*)((struct _tuple4**)Cyc__genarr_1065),(
void*)((struct _tuple4**)Cyc__genarr_1065 + 3)}};struct _tuple18{unsigned int f1;
struct Cyc_Absyn_TunionInfo f2;};static struct _tuple5 Cyc__gentuple_1067={offsetof(
struct _tuple18,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_1068={
offsetof(struct _tuple18,f2),(void*)& Cyc_Absyn_tunion_info_t_rep};static struct
_tuple5*Cyc__genarr_1069[2]={& Cyc__gentuple_1067,& Cyc__gentuple_1068};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1044={4,sizeof(struct _tuple18),{(void*)((
struct _tuple5**)Cyc__genarr_1069),(void*)((struct _tuple5**)Cyc__genarr_1069),(
void*)((struct _tuple5**)Cyc__genarr_1069 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1018;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1025;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static unsigned char _tmp1C0[23]="UnknownTunionFieldInfo";
static struct _tagged_arr Cyc__genname_1030={_tmp1C0,_tmp1C0,_tmp1C0 + 23};static
unsigned char _tmp1C1[12]="tunion_name";static struct _tuple4 Cyc__gentuple_1026={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),{_tmp1C1,_tmp1C1,
_tmp1C1 + 12},(void*)& Cyc__genrep_10};static unsigned char _tmp1C2[11]="field_name";
static struct _tuple4 Cyc__gentuple_1027={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),{
_tmp1C2,_tmp1C2,_tmp1C2 + 11},(void*)& Cyc__genrep_10};static unsigned char _tmp1C3[
11]="is_xtunion";static struct _tuple4 Cyc__gentuple_1028={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),{
_tmp1C3,_tmp1C3,_tmp1C3 + 11},(void*)((void*)& Cyc__genrep_102)};static struct
_tuple4*Cyc__genarr_1029[3]={& Cyc__gentuple_1026,& Cyc__gentuple_1027,& Cyc__gentuple_1028};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={3,(
struct _tagged_arr*)& Cyc__genname_1030,sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(
void*)((struct _tuple4**)Cyc__genarr_1029),(void*)((struct _tuple4**)Cyc__genarr_1029),(
void*)((struct _tuple4**)Cyc__genarr_1029 + 3)}};struct _tuple19{unsigned int f1;
struct Cyc_Absyn_UnknownTunionFieldInfo f2;};static struct _tuple5 Cyc__gentuple_1031={
offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_1032={
offsetof(struct _tuple19,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple5*Cyc__genarr_1033[2]={& Cyc__gentuple_1031,& Cyc__gentuple_1032};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1025={4,sizeof(struct _tuple19),{(
void*)((struct _tuple5**)Cyc__genarr_1033),(void*)((struct _tuple5**)Cyc__genarr_1033),(
void*)((struct _tuple5**)Cyc__genarr_1033 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1020;struct _tuple20{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;};static struct _tuple5 Cyc__gentuple_1021={
offsetof(struct _tuple20,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_1022={
offsetof(struct _tuple20,f2),(void*)((void*)& Cyc__genrep_318)};static struct
_tuple5 Cyc__gentuple_1023={offsetof(struct _tuple20,f3),(void*)& Cyc__genrep_300};
static struct _tuple5*Cyc__genarr_1024[3]={& Cyc__gentuple_1021,& Cyc__gentuple_1022,&
Cyc__gentuple_1023};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1020={4,
sizeof(struct _tuple20),{(void*)((struct _tuple5**)Cyc__genarr_1024),(void*)((
struct _tuple5**)Cyc__genarr_1024),(void*)((struct _tuple5**)Cyc__genarr_1024 + 3)}};
static struct _tuple6*Cyc__genarr_1019[0]={};static unsigned char _tmp1C7[19]="UnknownTunionfield";
static struct _tuple4 Cyc__gentuple_1034={0,{_tmp1C7,_tmp1C7,_tmp1C7 + 19},(void*)&
Cyc__genrep_1025};static unsigned char _tmp1C8[17]="KnownTunionfield";static struct
_tuple4 Cyc__gentuple_1035={1,{_tmp1C8,_tmp1C8,_tmp1C8 + 17},(void*)& Cyc__genrep_1020};
static struct _tuple4*Cyc__genarr_1036[2]={& Cyc__gentuple_1034,& Cyc__gentuple_1035};
static unsigned char _tmp1CA[17]="TunionFieldInfoU";struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_TunionFieldInfoU_rep={5,{_tmp1CA,_tmp1CA,_tmp1CA + 17},{(void*)((
struct _tuple6**)Cyc__genarr_1019),(void*)((struct _tuple6**)Cyc__genarr_1019),(
void*)((struct _tuple6**)Cyc__genarr_1019 + 0)},{(void*)((struct _tuple4**)Cyc__genarr_1036),(
void*)((struct _tuple4**)Cyc__genarr_1036),(void*)((struct _tuple4**)Cyc__genarr_1036
+ 2)}};static unsigned char _tmp1CB[16]="TunionFieldInfo";static struct _tagged_arr
Cyc__genname_1040={_tmp1CB,_tmp1CB,_tmp1CB + 16};static unsigned char _tmp1CC[11]="field_info";
static struct _tuple4 Cyc__gentuple_1037={offsetof(struct Cyc_Absyn_TunionFieldInfo,field_info),{
_tmp1CC,_tmp1CC,_tmp1CC + 11},(void*)& Cyc_tunion_Absyn_TunionFieldInfoU_rep};
static unsigned char _tmp1CD[6]="targs";static struct _tuple4 Cyc__gentuple_1038={
offsetof(struct Cyc_Absyn_TunionFieldInfo,targs),{_tmp1CD,_tmp1CD,_tmp1CD + 6},(
void*)& Cyc__genrep_52};static struct _tuple4*Cyc__genarr_1039[2]={& Cyc__gentuple_1037,&
Cyc__gentuple_1038};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_1040,sizeof(struct Cyc_Absyn_TunionFieldInfo),{(
void*)((struct _tuple4**)Cyc__genarr_1039),(void*)((struct _tuple4**)Cyc__genarr_1039),(
void*)((struct _tuple4**)Cyc__genarr_1039 + 2)}};struct _tuple21{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple5 Cyc__gentuple_1041={
offsetof(struct _tuple21,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_1042={
offsetof(struct _tuple21,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple5*Cyc__genarr_1043[2]={& Cyc__gentuple_1041,& Cyc__gentuple_1042};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1018={4,sizeof(struct _tuple21),{(
void*)((struct _tuple5**)Cyc__genarr_1043),(void*)((struct _tuple5**)Cyc__genarr_1043),(
void*)((struct _tuple5**)Cyc__genarr_1043 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_982;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1004;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_990;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_bounds_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_76;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_77;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_823;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_838;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep;static unsigned char _tmp1D0[7]="Signed";
static struct _tuple6 Cyc__gentuple_398={0,{_tmp1D0,_tmp1D0,_tmp1D0 + 7}};static
unsigned char _tmp1D1[9]="Unsigned";static struct _tuple6 Cyc__gentuple_399={1,{
_tmp1D1,_tmp1D1,_tmp1D1 + 9}};static struct _tuple6*Cyc__genarr_400[2]={& Cyc__gentuple_398,&
Cyc__gentuple_399};static struct _tuple4*Cyc__genarr_401[0]={};static unsigned char
_tmp1D3[5]="Sign";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep={5,{
_tmp1D3,_tmp1D3,_tmp1D3 + 5},{(void*)((struct _tuple6**)Cyc__genarr_400),(void*)((
struct _tuple6**)Cyc__genarr_400),(void*)((struct _tuple6**)Cyc__genarr_400 + 2)},{(
void*)((struct _tuple4**)Cyc__genarr_401),(void*)((struct _tuple4**)Cyc__genarr_401),(
void*)((struct _tuple4**)Cyc__genarr_401 + 0)}};struct _tuple22{unsigned int f1;void*
f2;unsigned char f3;};static struct _tuple5 Cyc__gentuple_839={offsetof(struct
_tuple22,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_840={
offsetof(struct _tuple22,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple5 Cyc__gentuple_841={
offsetof(struct _tuple22,f3),(void*)((void*)& Cyc__genrep_14)};static struct _tuple5*
Cyc__genarr_842[3]={& Cyc__gentuple_839,& Cyc__gentuple_840,& Cyc__gentuple_841};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_838={4,sizeof(struct _tuple22),{(
void*)((struct _tuple5**)Cyc__genarr_842),(void*)((struct _tuple5**)Cyc__genarr_842),(
void*)((struct _tuple5**)Cyc__genarr_842 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_832;static struct Cyc_Typerep_Int_struct Cyc__genrep_833={0,1,16};
struct _tuple23{unsigned int f1;void*f2;short f3;};static struct _tuple5 Cyc__gentuple_834={
offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_835={
offsetof(struct _tuple23,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple5 Cyc__gentuple_836={
offsetof(struct _tuple23,f3),(void*)& Cyc__genrep_833};static struct _tuple5*Cyc__genarr_837[
3]={& Cyc__gentuple_834,& Cyc__gentuple_835,& Cyc__gentuple_836};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_832={4,sizeof(struct _tuple23),{(void*)((struct _tuple5**)Cyc__genarr_837),(
void*)((struct _tuple5**)Cyc__genarr_837),(void*)((struct _tuple5**)Cyc__genarr_837
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_397;struct _tuple24{
unsigned int f1;void*f2;int f3;};static struct _tuple5 Cyc__gentuple_402={offsetof(
struct _tuple24,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_403={
offsetof(struct _tuple24,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple5 Cyc__gentuple_404={
offsetof(struct _tuple24,f3),(void*)((void*)& Cyc__genrep_102)};static struct
_tuple5*Cyc__genarr_405[3]={& Cyc__gentuple_402,& Cyc__gentuple_403,& Cyc__gentuple_404};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_397={4,sizeof(struct _tuple24),{(
void*)((struct _tuple5**)Cyc__genarr_405),(void*)((struct _tuple5**)Cyc__genarr_405),(
void*)((struct _tuple5**)Cyc__genarr_405 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_826;static struct Cyc_Typerep_Int_struct Cyc__genrep_827={0,1,64};
struct _tuple25{unsigned int f1;void*f2;long long f3;};static struct _tuple5 Cyc__gentuple_828={
offsetof(struct _tuple25,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_829={
offsetof(struct _tuple25,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple5 Cyc__gentuple_830={
offsetof(struct _tuple25,f3),(void*)& Cyc__genrep_827};static struct _tuple5*Cyc__genarr_831[
3]={& Cyc__gentuple_828,& Cyc__gentuple_829,& Cyc__gentuple_830};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_826={4,sizeof(struct _tuple25),{(void*)((struct _tuple5**)Cyc__genarr_831),(
void*)((struct _tuple5**)Cyc__genarr_831),(void*)((struct _tuple5**)Cyc__genarr_831
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_112;static struct _tuple5
Cyc__gentuple_113={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};static
struct _tuple5 Cyc__gentuple_114={offsetof(struct _tuple6,f2),(void*)((void*)& Cyc__genrep_13)};
static struct _tuple5*Cyc__genarr_115[2]={& Cyc__gentuple_113,& Cyc__gentuple_114};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_112={4,sizeof(struct _tuple6),{(
void*)((struct _tuple5**)Cyc__genarr_115),(void*)((struct _tuple5**)Cyc__genarr_115),(
void*)((struct _tuple5**)Cyc__genarr_115 + 2)}};static unsigned char _tmp1DB[7]="Null_c";
static struct _tuple6 Cyc__gentuple_824={0,{_tmp1DB,_tmp1DB,_tmp1DB + 7}};static
struct _tuple6*Cyc__genarr_825[1]={& Cyc__gentuple_824};static unsigned char _tmp1DC[
7]="Char_c";static struct _tuple4 Cyc__gentuple_843={0,{_tmp1DC,_tmp1DC,_tmp1DC + 7},(
void*)& Cyc__genrep_838};static unsigned char _tmp1DD[8]="Short_c";static struct
_tuple4 Cyc__gentuple_844={1,{_tmp1DD,_tmp1DD,_tmp1DD + 8},(void*)& Cyc__genrep_832};
static unsigned char _tmp1DE[6]="Int_c";static struct _tuple4 Cyc__gentuple_845={2,{
_tmp1DE,_tmp1DE,_tmp1DE + 6},(void*)& Cyc__genrep_397};static unsigned char _tmp1DF[
11]="LongLong_c";static struct _tuple4 Cyc__gentuple_846={3,{_tmp1DF,_tmp1DF,
_tmp1DF + 11},(void*)& Cyc__genrep_826};static unsigned char _tmp1E0[8]="Float_c";
static struct _tuple4 Cyc__gentuple_847={4,{_tmp1E0,_tmp1E0,_tmp1E0 + 8},(void*)& Cyc__genrep_112};
static unsigned char _tmp1E1[9]="String_c";static struct _tuple4 Cyc__gentuple_848={5,{
_tmp1E1,_tmp1E1,_tmp1E1 + 9},(void*)& Cyc__genrep_112};static struct _tuple4*Cyc__genarr_849[
6]={& Cyc__gentuple_843,& Cyc__gentuple_844,& Cyc__gentuple_845,& Cyc__gentuple_846,&
Cyc__gentuple_847,& Cyc__gentuple_848};static unsigned char _tmp1E3[5]="Cnst";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep={5,{_tmp1E3,_tmp1E3,_tmp1E3 + 
5},{(void*)((struct _tuple6**)Cyc__genarr_825),(void*)((struct _tuple6**)Cyc__genarr_825),(
void*)((struct _tuple6**)Cyc__genarr_825 + 1)},{(void*)((struct _tuple4**)Cyc__genarr_849),(
void*)((struct _tuple4**)Cyc__genarr_849),(void*)((struct _tuple4**)Cyc__genarr_849
+ 6)}};static struct _tuple5 Cyc__gentuple_850={offsetof(struct _tuple5,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_851={offsetof(struct _tuple5,f2),(
void*)& Cyc_Absyn_cnst_t_rep};static struct _tuple5*Cyc__genarr_852[2]={& Cyc__gentuple_850,&
Cyc__gentuple_851};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_823={4,
sizeof(struct _tuple5),{(void*)((struct _tuple5**)Cyc__genarr_852),(void*)((struct
_tuple5**)Cyc__genarr_852),(void*)((struct _tuple5**)Cyc__genarr_852 + 2)}};extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_810;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_81;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_82;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Fndecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_578;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_579;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_580;struct _tuple26{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*
f3;};static struct _tuple5 Cyc__gentuple_581={offsetof(struct _tuple26,f1),(void*)&
Cyc__genrep_12};static struct _tuple5 Cyc__gentuple_582={offsetof(struct _tuple26,f2),(
void*)& Cyc__genrep_132};static struct _tuple5 Cyc__gentuple_583={offsetof(struct
_tuple26,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple5*Cyc__genarr_584[
3]={& Cyc__gentuple_581,& Cyc__gentuple_582,& Cyc__gentuple_583};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_580={4,sizeof(struct _tuple26),{(void*)((struct _tuple5**)Cyc__genarr_584),(
void*)((struct _tuple5**)Cyc__genarr_584),(void*)((struct _tuple5**)Cyc__genarr_584
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_579={1,1,(void*)((void*)&
Cyc__genrep_580)};static unsigned char _tmp1E7[5]="List";static struct _tagged_arr
Cyc__genname_588={_tmp1E7,_tmp1E7,_tmp1E7 + 5};static unsigned char _tmp1E8[3]="hd";
static struct _tuple4 Cyc__gentuple_585={offsetof(struct Cyc_List_List,hd),{_tmp1E8,
_tmp1E8,_tmp1E8 + 3},(void*)& Cyc__genrep_579};static unsigned char _tmp1E9[3]="tl";
static struct _tuple4 Cyc__gentuple_586={offsetof(struct Cyc_List_List,tl),{_tmp1E9,
_tmp1E9,_tmp1E9 + 3},(void*)& Cyc__genrep_578};static struct _tuple4*Cyc__genarr_587[
2]={& Cyc__gentuple_585,& Cyc__gentuple_586};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_588,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_587),(void*)((struct _tuple4**)Cyc__genarr_587),(void*)((
struct _tuple4**)Cyc__genarr_587 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_578={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_567;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_568;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
unsigned char _tmp1EC[4]="Opt";static struct _tagged_arr Cyc__genname_571={_tmp1EC,
_tmp1EC,_tmp1EC + 4};static unsigned char _tmp1ED[2]="v";static struct _tuple4 Cyc__gentuple_569={
offsetof(struct Cyc_Core_Opt,v),{_tmp1ED,_tmp1ED,_tmp1ED + 2},(void*)& Cyc__genrep_12};
static struct _tuple4*Cyc__genarr_570[1]={& Cyc__gentuple_569};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_var_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_571,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple4**)Cyc__genarr_570),(void*)((
struct _tuple4**)Cyc__genarr_570),(void*)((struct _tuple4**)Cyc__genarr_570 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_568={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static unsigned char _tmp1F0[11]="VarargInfo";static struct _tagged_arr Cyc__genname_577={
_tmp1F0,_tmp1F0,_tmp1F0 + 11};static unsigned char _tmp1F1[5]="name";static struct
_tuple4 Cyc__gentuple_572={offsetof(struct Cyc_Absyn_VarargInfo,name),{_tmp1F1,
_tmp1F1,_tmp1F1 + 5},(void*)& Cyc__genrep_568};static unsigned char _tmp1F2[3]="tq";
static struct _tuple4 Cyc__gentuple_573={offsetof(struct Cyc_Absyn_VarargInfo,tq),{
_tmp1F2,_tmp1F2,_tmp1F2 + 3},(void*)& Cyc__genrep_132};static unsigned char _tmp1F3[
5]="type";static struct _tuple4 Cyc__gentuple_574={offsetof(struct Cyc_Absyn_VarargInfo,type),{
_tmp1F3,_tmp1F3,_tmp1F3 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static
unsigned char _tmp1F4[7]="inject";static struct _tuple4 Cyc__gentuple_575={offsetof(
struct Cyc_Absyn_VarargInfo,inject),{_tmp1F4,_tmp1F4,_tmp1F4 + 7},(void*)((void*)&
Cyc__genrep_102)};static struct _tuple4*Cyc__genarr_576[4]={& Cyc__gentuple_572,&
Cyc__gentuple_573,& Cyc__gentuple_574,& Cyc__gentuple_575};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_577,sizeof(
struct Cyc_Absyn_VarargInfo),{(void*)((struct _tuple4**)Cyc__genarr_576),(void*)((
struct _tuple4**)Cyc__genarr_576),(void*)((struct _tuple4**)Cyc__genarr_576 + 4)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_567={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_557;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_558;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_559;static struct
_tuple5 Cyc__gentuple_560={offsetof(struct _tuple8,f1),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple5 Cyc__gentuple_561={offsetof(struct _tuple8,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple5*Cyc__genarr_562[2]={& Cyc__gentuple_560,&
Cyc__gentuple_561};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_559={4,
sizeof(struct _tuple8),{(void*)((struct _tuple5**)Cyc__genarr_562),(void*)((struct
_tuple5**)Cyc__genarr_562),(void*)((struct _tuple5**)Cyc__genarr_562 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_558={1,1,(void*)((void*)& Cyc__genrep_559)};
static unsigned char _tmp1F9[5]="List";static struct _tagged_arr Cyc__genname_566={
_tmp1F9,_tmp1F9,_tmp1F9 + 5};static unsigned char _tmp1FA[3]="hd";static struct
_tuple4 Cyc__gentuple_563={offsetof(struct Cyc_List_List,hd),{_tmp1FA,_tmp1FA,
_tmp1FA + 3},(void*)& Cyc__genrep_558};static unsigned char _tmp1FB[3]="tl";static
struct _tuple4 Cyc__gentuple_564={offsetof(struct Cyc_List_List,tl),{_tmp1FB,
_tmp1FB,_tmp1FB + 3},(void*)& Cyc__genrep_557};static struct _tuple4*Cyc__genarr_565[
2]={& Cyc__gentuple_563,& Cyc__gentuple_564};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_566,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_565),(void*)((struct _tuple4**)Cyc__genarr_565),(void*)((
struct _tuple4**)Cyc__genarr_565 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_557={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_524;struct _tuple27{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple5 Cyc__gentuple_525={
offsetof(struct _tuple27,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_526={
offsetof(struct _tuple27,f2),(void*)& Cyc__genrep_159};static struct _tuple5 Cyc__gentuple_527={
offsetof(struct _tuple27,f3),(void*)& Cyc__genrep_159};static struct _tuple5*Cyc__genarr_528[
3]={& Cyc__gentuple_525,& Cyc__gentuple_526,& Cyc__gentuple_527};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_524={4,sizeof(struct _tuple27),{(void*)((struct _tuple5**)Cyc__genarr_528),(
void*)((struct _tuple5**)Cyc__genarr_528),(void*)((struct _tuple5**)Cyc__genarr_528
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_520;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_73;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_73={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple28{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple5 Cyc__gentuple_521={offsetof(struct _tuple28,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_522={offsetof(struct _tuple28,f2),(
void*)& Cyc__genrep_73};static struct _tuple5*Cyc__genarr_523[2]={& Cyc__gentuple_521,&
Cyc__gentuple_522};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_520={4,
sizeof(struct _tuple28),{(void*)((struct _tuple5**)Cyc__genarr_523),(void*)((
struct _tuple5**)Cyc__genarr_523),(void*)((struct _tuple5**)Cyc__genarr_523 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_514;struct _tuple29{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*f3;struct Cyc_Absyn_Stmt*f4;};
static struct _tuple5 Cyc__gentuple_515={offsetof(struct _tuple29,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_516={offsetof(struct _tuple29,f2),(void*)& Cyc__genrep_77};
static struct _tuple5 Cyc__gentuple_517={offsetof(struct _tuple29,f3),(void*)& Cyc__genrep_159};
static struct _tuple5 Cyc__gentuple_518={offsetof(struct _tuple29,f4),(void*)& Cyc__genrep_159};
static struct _tuple5*Cyc__genarr_519[4]={& Cyc__gentuple_515,& Cyc__gentuple_516,&
Cyc__gentuple_517,& Cyc__gentuple_518};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_514={
4,sizeof(struct _tuple29),{(void*)((struct _tuple5**)Cyc__genarr_519),(void*)((
struct _tuple5**)Cyc__genarr_519),(void*)((struct _tuple5**)Cyc__genarr_519 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_509;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_168;static struct _tuple5 Cyc__gentuple_169={offsetof(struct _tuple2,f1),(
void*)& Cyc__genrep_77};static struct _tuple5 Cyc__gentuple_170={offsetof(struct
_tuple2,f2),(void*)& Cyc__genrep_159};static struct _tuple5*Cyc__genarr_171[2]={&
Cyc__gentuple_169,& Cyc__gentuple_170};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_168={
4,sizeof(struct _tuple2),{(void*)((struct _tuple5**)Cyc__genarr_171),(void*)((
struct _tuple5**)Cyc__genarr_171),(void*)((struct _tuple5**)Cyc__genarr_171 + 2)}};
struct _tuple30{unsigned int f1;struct _tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static
struct _tuple5 Cyc__gentuple_510={offsetof(struct _tuple30,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_511={offsetof(struct _tuple30,f2),(void*)& Cyc__genrep_168};
static struct _tuple5 Cyc__gentuple_512={offsetof(struct _tuple30,f3),(void*)& Cyc__genrep_159};
static struct _tuple5*Cyc__genarr_513[3]={& Cyc__gentuple_510,& Cyc__gentuple_511,&
Cyc__gentuple_512};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_509={4,
sizeof(struct _tuple30),{(void*)((struct _tuple5**)Cyc__genarr_513),(void*)((
struct _tuple5**)Cyc__genarr_513),(void*)((struct _tuple5**)Cyc__genarr_513 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_505;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_500;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_500={1,1,(void*)((
void*)& Cyc_struct_Absyn_Stmt_rep)};struct _tuple31{unsigned int f1;struct Cyc_Absyn_Stmt*
f2;};static struct _tuple5 Cyc__gentuple_506={offsetof(struct _tuple31,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_507={offsetof(struct _tuple31,f2),(
void*)& Cyc__genrep_500};static struct _tuple5*Cyc__genarr_508[2]={& Cyc__gentuple_506,&
Cyc__gentuple_507};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_505={4,
sizeof(struct _tuple31),{(void*)((struct _tuple5**)Cyc__genarr_508),(void*)((
struct _tuple5**)Cyc__genarr_508),(void*)((struct _tuple5**)Cyc__genarr_508 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_499;struct _tuple32{unsigned int
f1;struct _tagged_arr*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple5 Cyc__gentuple_501={
offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_502={
offsetof(struct _tuple32,f2),(void*)& Cyc__genrep_12};static struct _tuple5 Cyc__gentuple_503={
offsetof(struct _tuple32,f3),(void*)& Cyc__genrep_500};static struct _tuple5*Cyc__genarr_504[
3]={& Cyc__gentuple_501,& Cyc__gentuple_502,& Cyc__gentuple_503};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_499={4,sizeof(struct _tuple32),{(void*)((struct _tuple5**)Cyc__genarr_504),(
void*)((struct _tuple5**)Cyc__genarr_504),(void*)((struct _tuple5**)Cyc__genarr_504
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_492;struct _tuple33{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple5 Cyc__gentuple_493={offsetof(struct _tuple33,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_494={offsetof(struct _tuple33,f2),(
void*)& Cyc__genrep_77};static struct _tuple5 Cyc__gentuple_495={offsetof(struct
_tuple33,f3),(void*)& Cyc__genrep_168};static struct _tuple5 Cyc__gentuple_496={
offsetof(struct _tuple33,f4),(void*)& Cyc__genrep_168};static struct _tuple5 Cyc__gentuple_497={
offsetof(struct _tuple33,f5),(void*)& Cyc__genrep_159};static struct _tuple5*Cyc__genarr_498[
5]={& Cyc__gentuple_493,& Cyc__gentuple_494,& Cyc__gentuple_495,& Cyc__gentuple_496,&
Cyc__gentuple_497};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_492={4,
sizeof(struct _tuple33),{(void*)((struct _tuple5**)Cyc__genarr_498),(void*)((
struct _tuple5**)Cyc__genarr_498),(void*)((struct _tuple5**)Cyc__genarr_498 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_487;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_224;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_225;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Switch_clause_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_226;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_393;
struct _tuple34{unsigned int f1;unsigned char f2;};static struct _tuple5 Cyc__gentuple_394={
offsetof(struct _tuple34,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_395={
offsetof(struct _tuple34,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple5*
Cyc__genarr_396[2]={& Cyc__gentuple_394,& Cyc__gentuple_395};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_393={4,sizeof(struct _tuple34),{(void*)((struct _tuple5**)Cyc__genarr_396),(
void*)((struct _tuple5**)Cyc__genarr_396),(void*)((struct _tuple5**)Cyc__genarr_396
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_389;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_265;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;
static unsigned char _tmp209[5]="List";static struct _tagged_arr Cyc__genname_269={
_tmp209,_tmp209,_tmp209 + 5};static unsigned char _tmp20A[3]="hd";static struct
_tuple4 Cyc__gentuple_266={offsetof(struct Cyc_List_List,hd),{_tmp20A,_tmp20A,
_tmp20A + 3},(void*)& Cyc__genrep_226};static unsigned char _tmp20B[3]="tl";static
struct _tuple4 Cyc__gentuple_267={offsetof(struct Cyc_List_List,tl),{_tmp20B,
_tmp20B,_tmp20B + 3},(void*)& Cyc__genrep_265};static struct _tuple4*Cyc__genarr_268[
2]={& Cyc__gentuple_266,& Cyc__gentuple_267};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_269,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_268),(void*)((struct _tuple4**)Cyc__genarr_268),(void*)((
struct _tuple4**)Cyc__genarr_268 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_265={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};static struct
_tuple5 Cyc__gentuple_390={offsetof(struct _tuple10,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_391={offsetof(struct _tuple10,f2),(void*)& Cyc__genrep_265};
static struct _tuple5*Cyc__genarr_392[2]={& Cyc__gentuple_390,& Cyc__gentuple_391};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_389={4,sizeof(struct _tuple10),{(
void*)((struct _tuple5**)Cyc__genarr_392),(void*)((struct _tuple5**)Cyc__genarr_392),(
void*)((struct _tuple5**)Cyc__genarr_392 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_385;struct _tuple35{unsigned int f1;struct Cyc_Absyn_Pat*f2;};static
struct _tuple5 Cyc__gentuple_386={offsetof(struct _tuple35,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_387={offsetof(struct _tuple35,f2),(void*)& Cyc__genrep_226};
static struct _tuple5*Cyc__genarr_388[2]={& Cyc__gentuple_386,& Cyc__gentuple_387};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_385={4,sizeof(struct _tuple35),{(
void*)((struct _tuple5**)Cyc__genarr_388),(void*)((struct _tuple5**)Cyc__genarr_388),(
void*)((struct _tuple5**)Cyc__genarr_388 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_341;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_346;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Structdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_364;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;
static unsigned char _tmp210[4]="Opt";static struct _tagged_arr Cyc__genname_367={
_tmp210,_tmp210,_tmp210 + 4};static unsigned char _tmp211[2]="v";static struct
_tuple4 Cyc__gentuple_365={offsetof(struct Cyc_Core_Opt,v),{_tmp211,_tmp211,
_tmp211 + 2},(void*)& Cyc__genrep_10};static struct _tuple4*Cyc__genarr_366[1]={& Cyc__gentuple_365};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={3,(
struct _tagged_arr*)& Cyc__genname_367,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple4**)Cyc__genarr_366),(void*)((struct _tuple4**)Cyc__genarr_366),(void*)((
struct _tuple4**)Cyc__genarr_366 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_364={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_347;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_structfield_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_348;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_structfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_349;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Structfield_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_83;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_116;struct _tuple36{unsigned int f1;int f2;};static struct _tuple5 Cyc__gentuple_117={
offsetof(struct _tuple36,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_118={
offsetof(struct _tuple36,f2),(void*)((void*)& Cyc__genrep_102)};static struct
_tuple5*Cyc__genarr_119[2]={& Cyc__gentuple_117,& Cyc__gentuple_118};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_116={4,sizeof(struct _tuple36),{(void*)((
struct _tuple5**)Cyc__genarr_119),(void*)((struct _tuple5**)Cyc__genarr_119),(void*)((
struct _tuple5**)Cyc__genarr_119 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_101;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_format_type_t_rep;static
unsigned char _tmp215[10]="Printf_ft";static struct _tuple6 Cyc__gentuple_103={0,{
_tmp215,_tmp215,_tmp215 + 10}};static unsigned char _tmp216[9]="Scanf_ft";static
struct _tuple6 Cyc__gentuple_104={1,{_tmp216,_tmp216,_tmp216 + 9}};static struct
_tuple6*Cyc__genarr_105[2]={& Cyc__gentuple_103,& Cyc__gentuple_104};static struct
_tuple4*Cyc__genarr_106[0]={};static unsigned char _tmp218[12]="Format_Type";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_format_type_t_rep={5,{_tmp218,_tmp218,
_tmp218 + 12},{(void*)((struct _tuple6**)Cyc__genarr_105),(void*)((struct _tuple6**)
Cyc__genarr_105),(void*)((struct _tuple6**)Cyc__genarr_105 + 2)},{(void*)((struct
_tuple4**)Cyc__genarr_106),(void*)((struct _tuple4**)Cyc__genarr_106),(void*)((
struct _tuple4**)Cyc__genarr_106 + 0)}};struct _tuple37{unsigned int f1;void*f2;int
f3;int f4;};static struct _tuple5 Cyc__gentuple_107={offsetof(struct _tuple37,f1),(
void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_108={offsetof(struct
_tuple37,f2),(void*)& Cyc_Absyn_format_type_t_rep};static struct _tuple5 Cyc__gentuple_109={
offsetof(struct _tuple37,f3),(void*)((void*)& Cyc__genrep_102)};static struct
_tuple5 Cyc__gentuple_110={offsetof(struct _tuple37,f4),(void*)((void*)& Cyc__genrep_102)};
static struct _tuple5*Cyc__genarr_111[4]={& Cyc__gentuple_107,& Cyc__gentuple_108,&
Cyc__gentuple_109,& Cyc__gentuple_110};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_101={
4,sizeof(struct _tuple37),{(void*)((struct _tuple5**)Cyc__genarr_111),(void*)((
struct _tuple5**)Cyc__genarr_111),(void*)((struct _tuple5**)Cyc__genarr_111 + 4)}};
static unsigned char _tmp21A[12]="Stdcall_att";static struct _tuple6 Cyc__gentuple_84={
0,{_tmp21A,_tmp21A,_tmp21A + 12}};static unsigned char _tmp21B[10]="Cdecl_att";
static struct _tuple6 Cyc__gentuple_85={1,{_tmp21B,_tmp21B,_tmp21B + 10}};static
unsigned char _tmp21C[13]="Fastcall_att";static struct _tuple6 Cyc__gentuple_86={2,{
_tmp21C,_tmp21C,_tmp21C + 13}};static unsigned char _tmp21D[13]="Noreturn_att";
static struct _tuple6 Cyc__gentuple_87={3,{_tmp21D,_tmp21D,_tmp21D + 13}};static
unsigned char _tmp21E[10]="Const_att";static struct _tuple6 Cyc__gentuple_88={4,{
_tmp21E,_tmp21E,_tmp21E + 10}};static unsigned char _tmp21F[11]="Packed_att";static
struct _tuple6 Cyc__gentuple_89={5,{_tmp21F,_tmp21F,_tmp21F + 11}};static
unsigned char _tmp220[13]="Nocommon_att";static struct _tuple6 Cyc__gentuple_90={6,{
_tmp220,_tmp220,_tmp220 + 13}};static unsigned char _tmp221[11]="Shared_att";static
struct _tuple6 Cyc__gentuple_91={7,{_tmp221,_tmp221,_tmp221 + 11}};static
unsigned char _tmp222[11]="Unused_att";static struct _tuple6 Cyc__gentuple_92={8,{
_tmp222,_tmp222,_tmp222 + 11}};static unsigned char _tmp223[9]="Weak_att";static
struct _tuple6 Cyc__gentuple_93={9,{_tmp223,_tmp223,_tmp223 + 9}};static
unsigned char _tmp224[14]="Dllimport_att";static struct _tuple6 Cyc__gentuple_94={10,{
_tmp224,_tmp224,_tmp224 + 14}};static unsigned char _tmp225[14]="Dllexport_att";
static struct _tuple6 Cyc__gentuple_95={11,{_tmp225,_tmp225,_tmp225 + 14}};static
unsigned char _tmp226[27]="No_instrument_function_att";static struct _tuple6 Cyc__gentuple_96={
12,{_tmp226,_tmp226,_tmp226 + 27}};static unsigned char _tmp227[16]="Constructor_att";
static struct _tuple6 Cyc__gentuple_97={13,{_tmp227,_tmp227,_tmp227 + 16}};static
unsigned char _tmp228[15]="Destructor_att";static struct _tuple6 Cyc__gentuple_98={
14,{_tmp228,_tmp228,_tmp228 + 15}};static unsigned char _tmp229[26]="No_check_memory_usage_att";
static struct _tuple6 Cyc__gentuple_99={15,{_tmp229,_tmp229,_tmp229 + 26}};static
struct _tuple6*Cyc__genarr_100[16]={& Cyc__gentuple_84,& Cyc__gentuple_85,& Cyc__gentuple_86,&
Cyc__gentuple_87,& Cyc__gentuple_88,& Cyc__gentuple_89,& Cyc__gentuple_90,& Cyc__gentuple_91,&
Cyc__gentuple_92,& Cyc__gentuple_93,& Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,&
Cyc__gentuple_97,& Cyc__gentuple_98,& Cyc__gentuple_99};static unsigned char _tmp22A[
12]="Regparm_att";static struct _tuple4 Cyc__gentuple_120={0,{_tmp22A,_tmp22A,
_tmp22A + 12},(void*)& Cyc__genrep_116};static unsigned char _tmp22B[12]="Aligned_att";
static struct _tuple4 Cyc__gentuple_121={1,{_tmp22B,_tmp22B,_tmp22B + 12},(void*)&
Cyc__genrep_116};static unsigned char _tmp22C[12]="Section_att";static struct
_tuple4 Cyc__gentuple_122={2,{_tmp22C,_tmp22C,_tmp22C + 12},(void*)& Cyc__genrep_112};
static unsigned char _tmp22D[11]="Format_att";static struct _tuple4 Cyc__gentuple_123={
3,{_tmp22D,_tmp22D,_tmp22D + 11},(void*)& Cyc__genrep_101};static struct _tuple4*Cyc__genarr_124[
4]={& Cyc__gentuple_120,& Cyc__gentuple_121,& Cyc__gentuple_122,& Cyc__gentuple_123};
static unsigned char _tmp22F[10]="Attribute";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={
5,{_tmp22F,_tmp22F,_tmp22F + 10},{(void*)((struct _tuple6**)Cyc__genarr_100),(void*)((
struct _tuple6**)Cyc__genarr_100),(void*)((struct _tuple6**)Cyc__genarr_100 + 16)},{(
void*)((struct _tuple4**)Cyc__genarr_124),(void*)((struct _tuple4**)Cyc__genarr_124),(
void*)((struct _tuple4**)Cyc__genarr_124 + 4)}};static unsigned char _tmp230[5]="List";
static struct _tagged_arr Cyc__genname_128={_tmp230,_tmp230,_tmp230 + 5};static
unsigned char _tmp231[3]="hd";static struct _tuple4 Cyc__gentuple_125={offsetof(
struct Cyc_List_List,hd),{_tmp231,_tmp231,_tmp231 + 3},(void*)& Cyc_Absyn_attribute_t_rep};
static unsigned char _tmp232[3]="tl";static struct _tuple4 Cyc__gentuple_126={
offsetof(struct Cyc_List_List,tl),{_tmp232,_tmp232,_tmp232 + 3},(void*)& Cyc__genrep_83};
static struct _tuple4*Cyc__genarr_127[2]={& Cyc__gentuple_125,& Cyc__gentuple_126};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_128,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_127),(void*)((struct _tuple4**)Cyc__genarr_127),(void*)((
struct _tuple4**)Cyc__genarr_127 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_83={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};static
unsigned char _tmp235[12]="Structfield";static struct _tagged_arr Cyc__genname_356={
_tmp235,_tmp235,_tmp235 + 12};static unsigned char _tmp236[5]="name";static struct
_tuple4 Cyc__gentuple_350={offsetof(struct Cyc_Absyn_Structfield,name),{_tmp236,
_tmp236,_tmp236 + 5},(void*)& Cyc__genrep_12};static unsigned char _tmp237[3]="tq";
static struct _tuple4 Cyc__gentuple_351={offsetof(struct Cyc_Absyn_Structfield,tq),{
_tmp237,_tmp237,_tmp237 + 3},(void*)& Cyc__genrep_132};static unsigned char _tmp238[
5]="type";static struct _tuple4 Cyc__gentuple_352={offsetof(struct Cyc_Absyn_Structfield,type),{
_tmp238,_tmp238,_tmp238 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static
unsigned char _tmp239[6]="width";static struct _tuple4 Cyc__gentuple_353={offsetof(
struct Cyc_Absyn_Structfield,width),{_tmp239,_tmp239,_tmp239 + 6},(void*)& Cyc__genrep_73};
static unsigned char _tmp23A[11]="attributes";static struct _tuple4 Cyc__gentuple_354={
offsetof(struct Cyc_Absyn_Structfield,attributes),{_tmp23A,_tmp23A,_tmp23A + 11},(
void*)& Cyc__genrep_83};static struct _tuple4*Cyc__genarr_355[5]={& Cyc__gentuple_350,&
Cyc__gentuple_351,& Cyc__gentuple_352,& Cyc__gentuple_353,& Cyc__gentuple_354};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Structfield_rep={3,(struct
_tagged_arr*)& Cyc__genname_356,sizeof(struct Cyc_Absyn_Structfield),{(void*)((
struct _tuple4**)Cyc__genarr_355),(void*)((struct _tuple4**)Cyc__genarr_355),(void*)((
struct _tuple4**)Cyc__genarr_355 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_349={
1,1,(void*)((void*)& Cyc_struct_Absyn_Structfield_rep)};static unsigned char
_tmp23D[5]="List";static struct _tagged_arr Cyc__genname_360={_tmp23D,_tmp23D,
_tmp23D + 5};static unsigned char _tmp23E[3]="hd";static struct _tuple4 Cyc__gentuple_357={
offsetof(struct Cyc_List_List,hd),{_tmp23E,_tmp23E,_tmp23E + 3},(void*)& Cyc__genrep_349};
static unsigned char _tmp23F[3]="tl";static struct _tuple4 Cyc__gentuple_358={
offsetof(struct Cyc_List_List,tl),{_tmp23F,_tmp23F,_tmp23F + 3},(void*)& Cyc__genrep_348};
static struct _tuple4*Cyc__genarr_359[2]={& Cyc__gentuple_357,& Cyc__gentuple_358};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_structfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_360,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_359),(void*)((struct _tuple4**)Cyc__genarr_359),(void*)((
struct _tuple4**)Cyc__genarr_359 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_348={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_structfield_t46H2_rep)};static
unsigned char _tmp242[4]="Opt";static struct _tagged_arr Cyc__genname_363={_tmp242,
_tmp242,_tmp242 + 4};static unsigned char _tmp243[2]="v";static struct _tuple4 Cyc__gentuple_361={
offsetof(struct Cyc_Core_Opt,v),{_tmp243,_tmp243,_tmp243 + 2},(void*)& Cyc__genrep_348};
static struct _tuple4*Cyc__genarr_362[1]={& Cyc__gentuple_361};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_structfield_t46H22_rep={3,(struct
_tagged_arr*)& Cyc__genname_363,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple4**)Cyc__genarr_362),(void*)((struct _tuple4**)Cyc__genarr_362),(void*)((
struct _tuple4**)Cyc__genarr_362 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_347={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_structfield_t46H22_rep)};
static unsigned char _tmp246[11]="Structdecl";static struct _tagged_arr Cyc__genname_374={
_tmp246,_tmp246,_tmp246 + 11};static unsigned char _tmp247[3]="sc";static struct
_tuple4 Cyc__gentuple_368={offsetof(struct Cyc_Absyn_Structdecl,sc),{_tmp247,
_tmp247,_tmp247 + 3},(void*)& Cyc_Absyn_scope_t_rep};static unsigned char _tmp248[5]="name";
static struct _tuple4 Cyc__gentuple_369={offsetof(struct Cyc_Absyn_Structdecl,name),{
_tmp248,_tmp248,_tmp248 + 5},(void*)& Cyc__genrep_364};static unsigned char _tmp249[
4]="tvs";static struct _tuple4 Cyc__gentuple_370={offsetof(struct Cyc_Absyn_Structdecl,tvs),{
_tmp249,_tmp249,_tmp249 + 4},(void*)& Cyc__genrep_254};static unsigned char _tmp24A[
7]="fields";static struct _tuple4 Cyc__gentuple_371={offsetof(struct Cyc_Absyn_Structdecl,fields),{
_tmp24A,_tmp24A,_tmp24A + 7},(void*)& Cyc__genrep_347};static unsigned char _tmp24B[
11]="attributes";static struct _tuple4 Cyc__gentuple_372={offsetof(struct Cyc_Absyn_Structdecl,attributes),{
_tmp24B,_tmp24B,_tmp24B + 11},(void*)& Cyc__genrep_83};static struct _tuple4*Cyc__genarr_373[
5]={& Cyc__gentuple_368,& Cyc__gentuple_369,& Cyc__gentuple_370,& Cyc__gentuple_371,&
Cyc__gentuple_372};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Structdecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_374,sizeof(struct Cyc_Absyn_Structdecl),{(
void*)((struct _tuple4**)Cyc__genarr_373),(void*)((struct _tuple4**)Cyc__genarr_373),(
void*)((struct _tuple4**)Cyc__genarr_373 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_346={1,1,(void*)((void*)& Cyc_struct_Absyn_Structdecl_rep)};extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_342;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_type_t46H22_rep;static unsigned char _tmp24E[
4]="Opt";static struct _tagged_arr Cyc__genname_345={_tmp24E,_tmp24E,_tmp24E + 4};
static unsigned char _tmp24F[2]="v";static struct _tuple4 Cyc__gentuple_343={
offsetof(struct Cyc_Core_Opt,v),{_tmp24F,_tmp24F,_tmp24F + 2},(void*)& Cyc__genrep_52};
static struct _tuple4*Cyc__genarr_344[1]={& Cyc__gentuple_343};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_type_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_345,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple4**)Cyc__genarr_344),(
void*)((struct _tuple4**)Cyc__genarr_344),(void*)((struct _tuple4**)Cyc__genarr_344
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_342={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_type_t46H22_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_231;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_232;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_233;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_234;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_236;struct _tuple38{unsigned int f1;struct _tagged_arr*f2;};static
struct _tuple5 Cyc__gentuple_237={offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_238={offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_12};
static struct _tuple5*Cyc__genarr_239[2]={& Cyc__gentuple_237,& Cyc__gentuple_238};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_236={4,sizeof(struct _tuple38),{(
void*)((struct _tuple5**)Cyc__genarr_239),(void*)((struct _tuple5**)Cyc__genarr_239),(
void*)((struct _tuple5**)Cyc__genarr_239 + 2)}};static struct _tuple6*Cyc__genarr_235[
0]={};static unsigned char _tmp253[13]="ArrayElement";static struct _tuple4 Cyc__gentuple_240={
0,{_tmp253,_tmp253,_tmp253 + 13},(void*)& Cyc__genrep_76};static unsigned char
_tmp254[10]="FieldName";static struct _tuple4 Cyc__gentuple_241={1,{_tmp254,_tmp254,
_tmp254 + 10},(void*)& Cyc__genrep_236};static struct _tuple4*Cyc__genarr_242[2]={&
Cyc__gentuple_240,& Cyc__gentuple_241};static unsigned char _tmp256[11]="Designator";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={5,{_tmp256,_tmp256,
_tmp256 + 11},{(void*)((struct _tuple6**)Cyc__genarr_235),(void*)((struct _tuple6**)
Cyc__genarr_235),(void*)((struct _tuple6**)Cyc__genarr_235 + 0)},{(void*)((struct
_tuple4**)Cyc__genarr_242),(void*)((struct _tuple4**)Cyc__genarr_242),(void*)((
struct _tuple4**)Cyc__genarr_242 + 2)}};static unsigned char _tmp257[5]="List";
static struct _tagged_arr Cyc__genname_246={_tmp257,_tmp257,_tmp257 + 5};static
unsigned char _tmp258[3]="hd";static struct _tuple4 Cyc__gentuple_243={offsetof(
struct Cyc_List_List,hd),{_tmp258,_tmp258,_tmp258 + 3},(void*)& Cyc_Absyn_designator_t_rep};
static unsigned char _tmp259[3]="tl";static struct _tuple4 Cyc__gentuple_244={
offsetof(struct Cyc_List_List,tl),{_tmp259,_tmp259,_tmp259 + 3},(void*)& Cyc__genrep_234};
static struct _tuple4*Cyc__genarr_245[2]={& Cyc__gentuple_243,& Cyc__gentuple_244};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_246,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_245),(void*)((struct _tuple4**)Cyc__genarr_245),(void*)((
struct _tuple4**)Cyc__genarr_245 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_234={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};struct
_tuple39{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple5 Cyc__gentuple_247={
offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_234};static struct _tuple5 Cyc__gentuple_248={
offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_226};static struct _tuple5*Cyc__genarr_249[
2]={& Cyc__gentuple_247,& Cyc__gentuple_248};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_233={4,sizeof(struct _tuple39),{(void*)((struct _tuple5**)Cyc__genarr_249),(
void*)((struct _tuple5**)Cyc__genarr_249),(void*)((struct _tuple5**)Cyc__genarr_249
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_232={1,1,(void*)((void*)&
Cyc__genrep_233)};static unsigned char _tmp25E[5]="List";static struct _tagged_arr
Cyc__genname_253={_tmp25E,_tmp25E,_tmp25E + 5};static unsigned char _tmp25F[3]="hd";
static struct _tuple4 Cyc__gentuple_250={offsetof(struct Cyc_List_List,hd),{_tmp25F,
_tmp25F,_tmp25F + 3},(void*)& Cyc__genrep_232};static unsigned char _tmp260[3]="tl";
static struct _tuple4 Cyc__gentuple_251={offsetof(struct Cyc_List_List,tl),{_tmp260,
_tmp260,_tmp260 + 3},(void*)& Cyc__genrep_231};static struct _tuple4*Cyc__genarr_252[
2]={& Cyc__gentuple_250,& Cyc__gentuple_251};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_253,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_252),(void*)((struct _tuple4**)Cyc__genarr_252),(void*)((
struct _tuple4**)Cyc__genarr_252 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_231={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple40{unsigned int f1;struct Cyc_Absyn_Structdecl*f2;struct Cyc_Core_Opt*
f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};static struct _tuple5 Cyc__gentuple_375={
offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_376={
offsetof(struct _tuple40,f2),(void*)((void*)& Cyc__genrep_346)};static struct
_tuple5 Cyc__gentuple_377={offsetof(struct _tuple40,f3),(void*)& Cyc__genrep_342};
static struct _tuple5 Cyc__gentuple_378={offsetof(struct _tuple40,f4),(void*)& Cyc__genrep_254};
static struct _tuple5 Cyc__gentuple_379={offsetof(struct _tuple40,f5),(void*)& Cyc__genrep_231};
static struct _tuple5*Cyc__genarr_380[5]={& Cyc__gentuple_375,& Cyc__gentuple_376,&
Cyc__gentuple_377,& Cyc__gentuple_378,& Cyc__gentuple_379};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_341={4,sizeof(struct _tuple40),{(void*)((struct _tuple5**)Cyc__genarr_380),(
void*)((struct _tuple5**)Cyc__genarr_380),(void*)((struct _tuple5**)Cyc__genarr_380
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_299;struct _tuple41{
unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;
struct Cyc_List_List*f4;struct Cyc_List_List*f5;};static struct _tuple5 Cyc__gentuple_335={
offsetof(struct _tuple41,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_336={
offsetof(struct _tuple41,f2),(void*)((void*)& Cyc__genrep_318)};static struct
_tuple5 Cyc__gentuple_337={offsetof(struct _tuple41,f3),(void*)& Cyc__genrep_300};
static struct _tuple5 Cyc__gentuple_338={offsetof(struct _tuple41,f4),(void*)& Cyc__genrep_254};
static struct _tuple5 Cyc__gentuple_339={offsetof(struct _tuple41,f5),(void*)& Cyc__genrep_265};
static struct _tuple5*Cyc__genarr_340[5]={& Cyc__gentuple_335,& Cyc__gentuple_336,&
Cyc__gentuple_337,& Cyc__gentuple_338,& Cyc__gentuple_339};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_299={4,sizeof(struct _tuple41),{(void*)((struct _tuple5**)Cyc__genarr_340),(
void*)((struct _tuple5**)Cyc__genarr_340),(void*)((struct _tuple5**)Cyc__genarr_340
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_284;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_285;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_286;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_71;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_72;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumfield_rep;static unsigned char _tmp265[10]="Enumfield";static
struct _tagged_arr Cyc__genname_902={_tmp265,_tmp265,_tmp265 + 10};static
unsigned char _tmp266[5]="name";static struct _tuple4 Cyc__gentuple_898={offsetof(
struct Cyc_Absyn_Enumfield,name),{_tmp266,_tmp266,_tmp266 + 5},(void*)& Cyc__genrep_10};
static unsigned char _tmp267[4]="tag";static struct _tuple4 Cyc__gentuple_899={
offsetof(struct Cyc_Absyn_Enumfield,tag),{_tmp267,_tmp267,_tmp267 + 4},(void*)& Cyc__genrep_73};
static unsigned char _tmp268[4]="loc";static struct _tuple4 Cyc__gentuple_900={
offsetof(struct Cyc_Absyn_Enumfield,loc),{_tmp268,_tmp268,_tmp268 + 4},(void*)& Cyc__genrep_2};
static struct _tuple4*Cyc__genarr_901[3]={& Cyc__gentuple_898,& Cyc__gentuple_899,&
Cyc__gentuple_900};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_902,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple4**)Cyc__genarr_901),(void*)((struct _tuple4**)Cyc__genarr_901),(void*)((
struct _tuple4**)Cyc__genarr_901 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_72={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};static unsigned char _tmp26B[
5]="List";static struct _tagged_arr Cyc__genname_906={_tmp26B,_tmp26B,_tmp26B + 5};
static unsigned char _tmp26C[3]="hd";static struct _tuple4 Cyc__gentuple_903={
offsetof(struct Cyc_List_List,hd),{_tmp26C,_tmp26C,_tmp26C + 3},(void*)& Cyc__genrep_72};
static unsigned char _tmp26D[3]="tl";static struct _tuple4 Cyc__gentuple_904={
offsetof(struct Cyc_List_List,tl),{_tmp26D,_tmp26D,_tmp26D + 3},(void*)& Cyc__genrep_71};
static struct _tuple4*Cyc__genarr_905[2]={& Cyc__gentuple_903,& Cyc__gentuple_904};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_906,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_905),(void*)((struct _tuple4**)Cyc__genarr_905),(void*)((
struct _tuple4**)Cyc__genarr_905 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_71={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static
unsigned char _tmp270[4]="Opt";static struct _tagged_arr Cyc__genname_289={_tmp270,
_tmp270,_tmp270 + 4};static unsigned char _tmp271[2]="v";static struct _tuple4 Cyc__gentuple_287={
offsetof(struct Cyc_Core_Opt,v),{_tmp271,_tmp271,_tmp271 + 2},(void*)& Cyc__genrep_71};
static struct _tuple4*Cyc__genarr_288[1]={& Cyc__gentuple_287};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_289,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple4**)Cyc__genarr_288),(
void*)((struct _tuple4**)Cyc__genarr_288),(void*)((struct _tuple4**)Cyc__genarr_288
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_286={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};static unsigned char
_tmp274[9]="Enumdecl";static struct _tagged_arr Cyc__genname_294={_tmp274,_tmp274,
_tmp274 + 9};static unsigned char _tmp275[3]="sc";static struct _tuple4 Cyc__gentuple_290={
offsetof(struct Cyc_Absyn_Enumdecl,sc),{_tmp275,_tmp275,_tmp275 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static unsigned char _tmp276[5]="name";static struct _tuple4 Cyc__gentuple_291={
offsetof(struct Cyc_Absyn_Enumdecl,name),{_tmp276,_tmp276,_tmp276 + 5},(void*)& Cyc__genrep_10};
static unsigned char _tmp277[7]="fields";static struct _tuple4 Cyc__gentuple_292={
offsetof(struct Cyc_Absyn_Enumdecl,fields),{_tmp277,_tmp277,_tmp277 + 7},(void*)&
Cyc__genrep_286};static struct _tuple4*Cyc__genarr_293[3]={& Cyc__gentuple_290,& Cyc__gentuple_291,&
Cyc__gentuple_292};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_294,sizeof(struct Cyc_Absyn_Enumdecl),{(void*)((
struct _tuple4**)Cyc__genarr_293),(void*)((struct _tuple4**)Cyc__genarr_293),(void*)((
struct _tuple4**)Cyc__genarr_293 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_285={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};struct _tuple42{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};static struct
_tuple5 Cyc__gentuple_295={offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_296={offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_285};
static struct _tuple5 Cyc__gentuple_297={offsetof(struct _tuple42,f3),(void*)& Cyc__genrep_72};
static struct _tuple5*Cyc__genarr_298[3]={& Cyc__gentuple_295,& Cyc__gentuple_296,&
Cyc__gentuple_297};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_284={4,
sizeof(struct _tuple42),{(void*)((struct _tuple5**)Cyc__genarr_298),(void*)((
struct _tuple5**)Cyc__genarr_298),(void*)((struct _tuple5**)Cyc__genarr_298 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_279;struct _tuple43{unsigned int
f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple5 Cyc__gentuple_280={
offsetof(struct _tuple43,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_281={
offsetof(struct _tuple43,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5 Cyc__gentuple_282={offsetof(struct _tuple43,f3),(void*)& Cyc__genrep_72};
static struct _tuple5*Cyc__genarr_283[3]={& Cyc__gentuple_280,& Cyc__gentuple_281,&
Cyc__gentuple_282};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_279={4,
sizeof(struct _tuple43),{(void*)((struct _tuple5**)Cyc__genarr_283),(void*)((
struct _tuple5**)Cyc__genarr_283),(void*)((struct _tuple5**)Cyc__genarr_283 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_275;struct _tuple44{unsigned int
f1;struct _tuple0*f2;};static struct _tuple5 Cyc__gentuple_276={offsetof(struct
_tuple44,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_277={
offsetof(struct _tuple44,f2),(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_278[
2]={& Cyc__gentuple_276,& Cyc__gentuple_277};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_275={4,sizeof(struct _tuple44),{(void*)((struct _tuple5**)Cyc__genarr_278),(
void*)((struct _tuple5**)Cyc__genarr_278),(void*)((struct _tuple5**)Cyc__genarr_278
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_264;struct _tuple45{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
static struct _tuple5 Cyc__gentuple_270={offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_271={offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_10};
static struct _tuple5 Cyc__gentuple_272={offsetof(struct _tuple45,f3),(void*)& Cyc__genrep_254};
static struct _tuple5 Cyc__gentuple_273={offsetof(struct _tuple45,f4),(void*)& Cyc__genrep_265};
static struct _tuple5*Cyc__genarr_274[4]={& Cyc__gentuple_270,& Cyc__gentuple_271,&
Cyc__gentuple_272,& Cyc__gentuple_273};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_264={
4,sizeof(struct _tuple45),{(void*)((struct _tuple5**)Cyc__genarr_274),(void*)((
struct _tuple5**)Cyc__genarr_274),(void*)((struct _tuple5**)Cyc__genarr_274 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_230;static struct _tuple5 Cyc__gentuple_259={
offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_260={
offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_10};static struct _tuple5 Cyc__gentuple_261={
offsetof(struct _tuple45,f3),(void*)& Cyc__genrep_254};static struct _tuple5 Cyc__gentuple_262={
offsetof(struct _tuple45,f4),(void*)& Cyc__genrep_231};static struct _tuple5*Cyc__genarr_263[
4]={& Cyc__gentuple_259,& Cyc__gentuple_260,& Cyc__gentuple_261,& Cyc__gentuple_262};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_230={4,sizeof(struct _tuple45),{(
void*)((struct _tuple5**)Cyc__genarr_263),(void*)((struct _tuple5**)Cyc__genarr_263),(
void*)((struct _tuple5**)Cyc__genarr_263 + 4)}};static unsigned char _tmp27F[7]="Wild_p";
static struct _tuple6 Cyc__gentuple_227={0,{_tmp27F,_tmp27F,_tmp27F + 7}};static
unsigned char _tmp280[7]="Null_p";static struct _tuple6 Cyc__gentuple_228={1,{
_tmp280,_tmp280,_tmp280 + 7}};static struct _tuple6*Cyc__genarr_229[2]={& Cyc__gentuple_227,&
Cyc__gentuple_228};static unsigned char _tmp281[6]="Var_p";static struct _tuple4 Cyc__gentuple_406={
0,{_tmp281,_tmp281,_tmp281 + 6},(void*)& Cyc__genrep_381};static unsigned char
_tmp282[6]="Int_p";static struct _tuple4 Cyc__gentuple_407={1,{_tmp282,_tmp282,
_tmp282 + 6},(void*)& Cyc__genrep_397};static unsigned char _tmp283[7]="Char_p";
static struct _tuple4 Cyc__gentuple_408={2,{_tmp283,_tmp283,_tmp283 + 7},(void*)& Cyc__genrep_393};
static unsigned char _tmp284[8]="Float_p";static struct _tuple4 Cyc__gentuple_409={3,{
_tmp284,_tmp284,_tmp284 + 8},(void*)& Cyc__genrep_112};static unsigned char _tmp285[
8]="Tuple_p";static struct _tuple4 Cyc__gentuple_410={4,{_tmp285,_tmp285,_tmp285 + 8},(
void*)& Cyc__genrep_389};static unsigned char _tmp286[10]="Pointer_p";static struct
_tuple4 Cyc__gentuple_411={5,{_tmp286,_tmp286,_tmp286 + 10},(void*)& Cyc__genrep_385};
static unsigned char _tmp287[12]="Reference_p";static struct _tuple4 Cyc__gentuple_412={
6,{_tmp287,_tmp287,_tmp287 + 12},(void*)& Cyc__genrep_381};static unsigned char
_tmp288[9]="Struct_p";static struct _tuple4 Cyc__gentuple_413={7,{_tmp288,_tmp288,
_tmp288 + 9},(void*)& Cyc__genrep_341};static unsigned char _tmp289[9]="Tunion_p";
static struct _tuple4 Cyc__gentuple_414={8,{_tmp289,_tmp289,_tmp289 + 9},(void*)& Cyc__genrep_299};
static unsigned char _tmp28A[7]="Enum_p";static struct _tuple4 Cyc__gentuple_415={9,{
_tmp28A,_tmp28A,_tmp28A + 7},(void*)& Cyc__genrep_284};static unsigned char _tmp28B[
11]="AnonEnum_p";static struct _tuple4 Cyc__gentuple_416={10,{_tmp28B,_tmp28B,
_tmp28B + 11},(void*)& Cyc__genrep_279};static unsigned char _tmp28C[12]="UnknownId_p";
static struct _tuple4 Cyc__gentuple_417={11,{_tmp28C,_tmp28C,_tmp28C + 12},(void*)&
Cyc__genrep_275};static unsigned char _tmp28D[14]="UnknownCall_p";static struct
_tuple4 Cyc__gentuple_418={12,{_tmp28D,_tmp28D,_tmp28D + 14},(void*)& Cyc__genrep_264};
static unsigned char _tmp28E[16]="UnknownFields_p";static struct _tuple4 Cyc__gentuple_419={
13,{_tmp28E,_tmp28E,_tmp28E + 16},(void*)& Cyc__genrep_230};static struct _tuple4*
Cyc__genarr_420[14]={& Cyc__gentuple_406,& Cyc__gentuple_407,& Cyc__gentuple_408,&
Cyc__gentuple_409,& Cyc__gentuple_410,& Cyc__gentuple_411,& Cyc__gentuple_412,& Cyc__gentuple_413,&
Cyc__gentuple_414,& Cyc__gentuple_415,& Cyc__gentuple_416,& Cyc__gentuple_417,& Cyc__gentuple_418,&
Cyc__gentuple_419};static unsigned char _tmp290[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,{_tmp290,_tmp290,_tmp290 + 8},{(void*)((struct _tuple6**)
Cyc__genarr_229),(void*)((struct _tuple6**)Cyc__genarr_229),(void*)((struct
_tuple6**)Cyc__genarr_229 + 2)},{(void*)((struct _tuple4**)Cyc__genarr_420),(void*)((
struct _tuple4**)Cyc__genarr_420),(void*)((struct _tuple4**)Cyc__genarr_420 + 14)}};
static unsigned char _tmp291[4]="Pat";static struct _tagged_arr Cyc__genname_425={
_tmp291,_tmp291,_tmp291 + 4};static unsigned char _tmp292[2]="r";static struct
_tuple4 Cyc__gentuple_421={offsetof(struct Cyc_Absyn_Pat,r),{_tmp292,_tmp292,
_tmp292 + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};static unsigned char _tmp293[5]="topt";
static struct _tuple4 Cyc__gentuple_422={offsetof(struct Cyc_Absyn_Pat,topt),{
_tmp293,_tmp293,_tmp293 + 5},(void*)& Cyc__genrep_61};static unsigned char _tmp294[4]="loc";
static struct _tuple4 Cyc__gentuple_423={offsetof(struct Cyc_Absyn_Pat,loc),{_tmp294,
_tmp294,_tmp294 + 4},(void*)& Cyc__genrep_2};static struct _tuple4*Cyc__genarr_424[3]={&
Cyc__gentuple_421,& Cyc__gentuple_422,& Cyc__gentuple_423};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Pat_rep={3,(struct _tagged_arr*)& Cyc__genname_425,sizeof(struct
Cyc_Absyn_Pat),{(void*)((struct _tuple4**)Cyc__genarr_424),(void*)((struct _tuple4**)
Cyc__genarr_424),(void*)((struct _tuple4**)Cyc__genarr_424 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_226={1,1,(void*)((void*)& Cyc_struct_Absyn_Pat_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_129;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;static unsigned char _tmp297[5]="List";
static struct _tagged_arr Cyc__genname_155={_tmp297,_tmp297,_tmp297 + 5};static
unsigned char _tmp298[3]="hd";static struct _tuple4 Cyc__gentuple_152={offsetof(
struct Cyc_List_List,hd),{_tmp298,_tmp298,_tmp298 + 3},(void*)& Cyc__genrep_131};
static unsigned char _tmp299[3]="tl";static struct _tuple4 Cyc__gentuple_153={
offsetof(struct Cyc_List_List,tl),{_tmp299,_tmp299,_tmp299 + 3},(void*)& Cyc__genrep_130};
static struct _tuple4*Cyc__genarr_154[2]={& Cyc__gentuple_152,& Cyc__gentuple_153};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_155,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_154),(void*)((struct _tuple4**)Cyc__genarr_154),(void*)((
struct _tuple4**)Cyc__genarr_154 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static
unsigned char _tmp29C[4]="Opt";static struct _tagged_arr Cyc__genname_158={_tmp29C,
_tmp29C,_tmp29C + 4};static unsigned char _tmp29D[2]="v";static struct _tuple4 Cyc__gentuple_156={
offsetof(struct Cyc_Core_Opt,v),{_tmp29D,_tmp29D,_tmp29D + 2},(void*)& Cyc__genrep_130};
static struct _tuple4*Cyc__genarr_157[1]={& Cyc__gentuple_156};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_158,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple4**)Cyc__genarr_157),(
void*)((struct _tuple4**)Cyc__genarr_157),(void*)((struct _tuple4**)Cyc__genarr_157
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_129={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};static unsigned char
_tmp2A0[14]="Switch_clause";static struct _tagged_arr Cyc__genname_432={_tmp2A0,
_tmp2A0,_tmp2A0 + 14};static unsigned char _tmp2A1[8]="pattern";static struct _tuple4
Cyc__gentuple_426={offsetof(struct Cyc_Absyn_Switch_clause,pattern),{_tmp2A1,
_tmp2A1,_tmp2A1 + 8},(void*)& Cyc__genrep_226};static unsigned char _tmp2A2[9]="pat_vars";
static struct _tuple4 Cyc__gentuple_427={offsetof(struct Cyc_Absyn_Switch_clause,pat_vars),{
_tmp2A2,_tmp2A2,_tmp2A2 + 9},(void*)& Cyc__genrep_129};static unsigned char _tmp2A3[
13]="where_clause";static struct _tuple4 Cyc__gentuple_428={offsetof(struct Cyc_Absyn_Switch_clause,where_clause),{
_tmp2A3,_tmp2A3,_tmp2A3 + 13},(void*)& Cyc__genrep_73};static unsigned char _tmp2A4[
5]="body";static struct _tuple4 Cyc__gentuple_429={offsetof(struct Cyc_Absyn_Switch_clause,body),{
_tmp2A4,_tmp2A4,_tmp2A4 + 5},(void*)& Cyc__genrep_159};static unsigned char _tmp2A5[
4]="loc";static struct _tuple4 Cyc__gentuple_430={offsetof(struct Cyc_Absyn_Switch_clause,loc),{
_tmp2A5,_tmp2A5,_tmp2A5 + 4},(void*)& Cyc__genrep_2};static struct _tuple4*Cyc__genarr_431[
5]={& Cyc__gentuple_426,& Cyc__gentuple_427,& Cyc__gentuple_428,& Cyc__gentuple_429,&
Cyc__gentuple_430};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _tagged_arr*)& Cyc__genname_432,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple4**)Cyc__genarr_431),(void*)((struct _tuple4**)Cyc__genarr_431),(
void*)((struct _tuple4**)Cyc__genarr_431 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_225={1,1,(void*)((void*)& Cyc_struct_Absyn_Switch_clause_rep)};static
unsigned char _tmp2A8[5]="List";static struct _tagged_arr Cyc__genname_436={_tmp2A8,
_tmp2A8,_tmp2A8 + 5};static unsigned char _tmp2A9[3]="hd";static struct _tuple4 Cyc__gentuple_433={
offsetof(struct Cyc_List_List,hd),{_tmp2A9,_tmp2A9,_tmp2A9 + 3},(void*)((void*)&
Cyc__genrep_225)};static unsigned char _tmp2AA[3]="tl";static struct _tuple4 Cyc__gentuple_434={
offsetof(struct Cyc_List_List,tl),{_tmp2AA,_tmp2AA,_tmp2AA + 3},(void*)& Cyc__genrep_224};
static struct _tuple4*Cyc__genarr_435[2]={& Cyc__gentuple_433,& Cyc__gentuple_434};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_436,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_435),(void*)((struct _tuple4**)Cyc__genarr_435),(void*)((
struct _tuple4**)Cyc__genarr_435 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_224={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};struct
_tuple46{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};static
struct _tuple5 Cyc__gentuple_488={offsetof(struct _tuple46,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_489={offsetof(struct _tuple46,f2),(void*)& Cyc__genrep_77};
static struct _tuple5 Cyc__gentuple_490={offsetof(struct _tuple46,f3),(void*)& Cyc__genrep_224};
static struct _tuple5*Cyc__genarr_491[3]={& Cyc__gentuple_488,& Cyc__gentuple_489,&
Cyc__gentuple_490};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_487={4,
sizeof(struct _tuple46),{(void*)((struct _tuple5**)Cyc__genarr_491),(void*)((
struct _tuple5**)Cyc__genarr_491),(void*)((struct _tuple5**)Cyc__genarr_491 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_471;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_472;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_473;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep;static unsigned char _tmp2AE[15]="SwitchC_clause";
static struct _tagged_arr Cyc__genname_478={_tmp2AE,_tmp2AE,_tmp2AE + 15};static
unsigned char _tmp2AF[9]="cnst_exp";static struct _tuple4 Cyc__gentuple_474={
offsetof(struct Cyc_Absyn_SwitchC_clause,cnst_exp),{_tmp2AF,_tmp2AF,_tmp2AF + 9},(
void*)& Cyc__genrep_73};static unsigned char _tmp2B0[5]="body";static struct _tuple4
Cyc__gentuple_475={offsetof(struct Cyc_Absyn_SwitchC_clause,body),{_tmp2B0,
_tmp2B0,_tmp2B0 + 5},(void*)& Cyc__genrep_159};static unsigned char _tmp2B1[4]="loc";
static struct _tuple4 Cyc__gentuple_476={offsetof(struct Cyc_Absyn_SwitchC_clause,loc),{
_tmp2B1,_tmp2B1,_tmp2B1 + 4},(void*)& Cyc__genrep_2};static struct _tuple4*Cyc__genarr_477[
3]={& Cyc__gentuple_474,& Cyc__gentuple_475,& Cyc__gentuple_476};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep={3,(struct _tagged_arr*)& Cyc__genname_478,
sizeof(struct Cyc_Absyn_SwitchC_clause),{(void*)((struct _tuple4**)Cyc__genarr_477),(
void*)((struct _tuple4**)Cyc__genarr_477),(void*)((struct _tuple4**)Cyc__genarr_477
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_473={1,1,(void*)((void*)&
Cyc_struct_Absyn_SwitchC_clause_rep)};static unsigned char _tmp2B4[5]="List";
static struct _tagged_arr Cyc__genname_482={_tmp2B4,_tmp2B4,_tmp2B4 + 5};static
unsigned char _tmp2B5[3]="hd";static struct _tuple4 Cyc__gentuple_479={offsetof(
struct Cyc_List_List,hd),{_tmp2B5,_tmp2B5,_tmp2B5 + 3},(void*)& Cyc__genrep_473};
static unsigned char _tmp2B6[3]="tl";static struct _tuple4 Cyc__gentuple_480={
offsetof(struct Cyc_List_List,tl),{_tmp2B6,_tmp2B6,_tmp2B6 + 3},(void*)& Cyc__genrep_472};
static struct _tuple4*Cyc__genarr_481[2]={& Cyc__gentuple_479,& Cyc__gentuple_480};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_482,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_481),(void*)((struct _tuple4**)Cyc__genarr_481),(void*)((
struct _tuple4**)Cyc__genarr_481 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_472={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep)};static
struct _tuple5 Cyc__gentuple_483={offsetof(struct _tuple46,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_484={offsetof(struct _tuple46,f2),(void*)& Cyc__genrep_77};
static struct _tuple5 Cyc__gentuple_485={offsetof(struct _tuple46,f3),(void*)& Cyc__genrep_472};
static struct _tuple5*Cyc__genarr_486[3]={& Cyc__gentuple_483,& Cyc__gentuple_484,&
Cyc__gentuple_485};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_471={4,
sizeof(struct _tuple46),{(void*)((struct _tuple5**)Cyc__genarr_486),(void*)((
struct _tuple5**)Cyc__genarr_486),(void*)((struct _tuple5**)Cyc__genarr_486 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_460;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_462;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep;
static unsigned char _tmp2BA[5]="List";static struct _tagged_arr Cyc__genname_466={
_tmp2BA,_tmp2BA,_tmp2BA + 5};static unsigned char _tmp2BB[3]="hd";static struct
_tuple4 Cyc__gentuple_463={offsetof(struct Cyc_List_List,hd),{_tmp2BB,_tmp2BB,
_tmp2BB + 3},(void*)& Cyc__genrep_77};static unsigned char _tmp2BC[3]="tl";static
struct _tuple4 Cyc__gentuple_464={offsetof(struct Cyc_List_List,tl),{_tmp2BC,
_tmp2BC,_tmp2BC + 3},(void*)& Cyc__genrep_462};static struct _tuple4*Cyc__genarr_465[
2]={& Cyc__gentuple_463,& Cyc__gentuple_464};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_466,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_465),(void*)((struct _tuple4**)Cyc__genarr_465),(void*)((
struct _tuple4**)Cyc__genarr_465 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_462={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_461;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_461={1,1,(void*)((
void*)& Cyc__genrep_225)};struct _tuple47{unsigned int f1;struct Cyc_List_List*f2;
struct Cyc_Absyn_Switch_clause**f3;};static struct _tuple5 Cyc__gentuple_467={
offsetof(struct _tuple47,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_468={
offsetof(struct _tuple47,f2),(void*)& Cyc__genrep_462};static struct _tuple5 Cyc__gentuple_469={
offsetof(struct _tuple47,f3),(void*)& Cyc__genrep_461};static struct _tuple5*Cyc__genarr_470[
3]={& Cyc__gentuple_467,& Cyc__gentuple_468,& Cyc__gentuple_469};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_460={4,sizeof(struct _tuple47),{(void*)((struct _tuple5**)Cyc__genarr_470),(
void*)((struct _tuple5**)Cyc__genarr_470),(void*)((struct _tuple5**)Cyc__genarr_470
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_455;struct _tuple48{
unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};static struct
_tuple5 Cyc__gentuple_456={offsetof(struct _tuple48,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_457={offsetof(struct _tuple48,f2),(void*)& Cyc__genrep_1};
static struct _tuple5 Cyc__gentuple_458={offsetof(struct _tuple48,f3),(void*)& Cyc__genrep_159};
static struct _tuple5*Cyc__genarr_459[3]={& Cyc__gentuple_456,& Cyc__gentuple_457,&
Cyc__gentuple_458};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_455={4,
sizeof(struct _tuple48),{(void*)((struct _tuple5**)Cyc__genarr_459),(void*)((
struct _tuple5**)Cyc__genarr_459),(void*)((struct _tuple5**)Cyc__genarr_459 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_451;static struct _tuple5 Cyc__gentuple_452={
offsetof(struct _tuple31,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_453={
offsetof(struct _tuple31,f2),(void*)& Cyc__genrep_159};static struct _tuple5*Cyc__genarr_454[
2]={& Cyc__gentuple_452,& Cyc__gentuple_453};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_451={4,sizeof(struct _tuple31),{(void*)((struct _tuple5**)Cyc__genarr_454),(
void*)((struct _tuple5**)Cyc__genarr_454),(void*)((struct _tuple5**)Cyc__genarr_454
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_446;static struct _tuple5
Cyc__gentuple_447={offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_5};static
struct _tuple5 Cyc__gentuple_448={offsetof(struct _tuple32,f2),(void*)& Cyc__genrep_12};
static struct _tuple5 Cyc__gentuple_449={offsetof(struct _tuple32,f3),(void*)& Cyc__genrep_159};
static struct _tuple5*Cyc__genarr_450[3]={& Cyc__gentuple_447,& Cyc__gentuple_448,&
Cyc__gentuple_449};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_446={4,
sizeof(struct _tuple32),{(void*)((struct _tuple5**)Cyc__genarr_450),(void*)((
struct _tuple5**)Cyc__genarr_450),(void*)((struct _tuple5**)Cyc__genarr_450 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_441;struct _tuple49{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static struct _tuple5 Cyc__gentuple_442={
offsetof(struct _tuple49,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_443={
offsetof(struct _tuple49,f2),(void*)& Cyc__genrep_159};static struct _tuple5 Cyc__gentuple_444={
offsetof(struct _tuple49,f3),(void*)& Cyc__genrep_168};static struct _tuple5*Cyc__genarr_445[
3]={& Cyc__gentuple_442,& Cyc__gentuple_443,& Cyc__gentuple_444};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_441={4,sizeof(struct _tuple49),{(void*)((struct _tuple5**)Cyc__genarr_445),(
void*)((struct _tuple5**)Cyc__genarr_445),(void*)((struct _tuple5**)Cyc__genarr_445
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_223;struct _tuple50{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};static struct
_tuple5 Cyc__gentuple_437={offsetof(struct _tuple50,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_438={offsetof(struct _tuple50,f2),(void*)& Cyc__genrep_159};
static struct _tuple5 Cyc__gentuple_439={offsetof(struct _tuple50,f3),(void*)& Cyc__genrep_224};
static struct _tuple5*Cyc__genarr_440[3]={& Cyc__gentuple_437,& Cyc__gentuple_438,&
Cyc__gentuple_439};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_223={4,
sizeof(struct _tuple50),{(void*)((struct _tuple5**)Cyc__genarr_440),(void*)((
struct _tuple5**)Cyc__genarr_440),(void*)((struct _tuple5**)Cyc__genarr_440 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_181;struct _tuple51{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;struct Cyc_Absyn_Stmt*f4;};
static struct _tuple5 Cyc__gentuple_218={offsetof(struct _tuple51,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_219={offsetof(struct _tuple51,f2),(void*)& Cyc__genrep_182};
static struct _tuple5 Cyc__gentuple_220={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_131};
static struct _tuple5 Cyc__gentuple_221={offsetof(struct _tuple51,f4),(void*)& Cyc__genrep_159};
static struct _tuple5*Cyc__genarr_222[4]={& Cyc__gentuple_218,& Cyc__gentuple_219,&
Cyc__gentuple_220,& Cyc__gentuple_221};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_181={
4,sizeof(struct _tuple51),{(void*)((struct _tuple5**)Cyc__genarr_222),(void*)((
struct _tuple5**)Cyc__genarr_222),(void*)((struct _tuple5**)Cyc__genarr_222 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_167;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_forarray_info_t_rep;static unsigned char _tmp2C7[13]="ForArrayInfo";
static struct _tagged_arr Cyc__genname_177={_tmp2C7,_tmp2C7,_tmp2C7 + 13};static
unsigned char _tmp2C8[6]="defns";static struct _tuple4 Cyc__gentuple_172={offsetof(
struct Cyc_Absyn_ForArrayInfo,defns),{_tmp2C8,_tmp2C8,_tmp2C8 + 6},(void*)& Cyc__genrep_130};
static unsigned char _tmp2C9[10]="condition";static struct _tuple4 Cyc__gentuple_173={
offsetof(struct Cyc_Absyn_ForArrayInfo,condition),{_tmp2C9,_tmp2C9,_tmp2C9 + 10},(
void*)& Cyc__genrep_168};static unsigned char _tmp2CA[6]="delta";static struct
_tuple4 Cyc__gentuple_174={offsetof(struct Cyc_Absyn_ForArrayInfo,delta),{_tmp2CA,
_tmp2CA,_tmp2CA + 6},(void*)& Cyc__genrep_168};static unsigned char _tmp2CB[5]="body";
static struct _tuple4 Cyc__gentuple_175={offsetof(struct Cyc_Absyn_ForArrayInfo,body),{
_tmp2CB,_tmp2CB,_tmp2CB + 5},(void*)& Cyc__genrep_159};static struct _tuple4*Cyc__genarr_176[
4]={& Cyc__gentuple_172,& Cyc__gentuple_173,& Cyc__gentuple_174,& Cyc__gentuple_175};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_forarray_info_t_rep={3,(struct
_tagged_arr*)& Cyc__genname_177,sizeof(struct Cyc_Absyn_ForArrayInfo),{(void*)((
struct _tuple4**)Cyc__genarr_176),(void*)((struct _tuple4**)Cyc__genarr_176),(void*)((
struct _tuple4**)Cyc__genarr_176 + 4)}};struct _tuple52{unsigned int f1;struct Cyc_Absyn_ForArrayInfo
f2;};static struct _tuple5 Cyc__gentuple_178={offsetof(struct _tuple52,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_179={offsetof(struct _tuple52,f2),(
void*)& Cyc_Absyn_forarray_info_t_rep};static struct _tuple5*Cyc__genarr_180[2]={&
Cyc__gentuple_178,& Cyc__gentuple_179};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_167={
4,sizeof(struct _tuple52),{(void*)((struct _tuple5**)Cyc__genarr_180),(void*)((
struct _tuple5**)Cyc__genarr_180),(void*)((struct _tuple5**)Cyc__genarr_180 + 2)}};
static unsigned char _tmp2CE[7]="Skip_s";static struct _tuple6 Cyc__gentuple_165={0,{
_tmp2CE,_tmp2CE,_tmp2CE + 7}};static struct _tuple6*Cyc__genarr_166[1]={& Cyc__gentuple_165};
static unsigned char _tmp2CF[6]="Exp_s";static struct _tuple4 Cyc__gentuple_529={0,{
_tmp2CF,_tmp2CF,_tmp2CF + 6},(void*)& Cyc__genrep_76};static unsigned char _tmp2D0[6]="Seq_s";
static struct _tuple4 Cyc__gentuple_530={1,{_tmp2D0,_tmp2D0,_tmp2D0 + 6},(void*)& Cyc__genrep_524};
static unsigned char _tmp2D1[9]="Return_s";static struct _tuple4 Cyc__gentuple_531={2,{
_tmp2D1,_tmp2D1,_tmp2D1 + 9},(void*)& Cyc__genrep_520};static unsigned char _tmp2D2[
13]="IfThenElse_s";static struct _tuple4 Cyc__gentuple_532={3,{_tmp2D2,_tmp2D2,
_tmp2D2 + 13},(void*)& Cyc__genrep_514};static unsigned char _tmp2D3[8]="While_s";
static struct _tuple4 Cyc__gentuple_533={4,{_tmp2D3,_tmp2D3,_tmp2D3 + 8},(void*)& Cyc__genrep_509};
static unsigned char _tmp2D4[8]="Break_s";static struct _tuple4 Cyc__gentuple_534={5,{
_tmp2D4,_tmp2D4,_tmp2D4 + 8},(void*)& Cyc__genrep_505};static unsigned char _tmp2D5[
11]="Continue_s";static struct _tuple4 Cyc__gentuple_535={6,{_tmp2D5,_tmp2D5,
_tmp2D5 + 11},(void*)& Cyc__genrep_505};static unsigned char _tmp2D6[7]="Goto_s";
static struct _tuple4 Cyc__gentuple_536={7,{_tmp2D6,_tmp2D6,_tmp2D6 + 7},(void*)& Cyc__genrep_499};
static unsigned char _tmp2D7[6]="For_s";static struct _tuple4 Cyc__gentuple_537={8,{
_tmp2D7,_tmp2D7,_tmp2D7 + 6},(void*)& Cyc__genrep_492};static unsigned char _tmp2D8[
9]="Switch_s";static struct _tuple4 Cyc__gentuple_538={9,{_tmp2D8,_tmp2D8,_tmp2D8 + 
9},(void*)& Cyc__genrep_487};static unsigned char _tmp2D9[10]="SwitchC_s";static
struct _tuple4 Cyc__gentuple_539={10,{_tmp2D9,_tmp2D9,_tmp2D9 + 10},(void*)& Cyc__genrep_471};
static unsigned char _tmp2DA[11]="Fallthru_s";static struct _tuple4 Cyc__gentuple_540={
11,{_tmp2DA,_tmp2DA,_tmp2DA + 11},(void*)& Cyc__genrep_460};static unsigned char
_tmp2DB[7]="Decl_s";static struct _tuple4 Cyc__gentuple_541={12,{_tmp2DB,_tmp2DB,
_tmp2DB + 7},(void*)& Cyc__genrep_455};static unsigned char _tmp2DC[6]="Cut_s";
static struct _tuple4 Cyc__gentuple_542={13,{_tmp2DC,_tmp2DC,_tmp2DC + 6},(void*)&
Cyc__genrep_451};static unsigned char _tmp2DD[9]="Splice_s";static struct _tuple4 Cyc__gentuple_543={
14,{_tmp2DD,_tmp2DD,_tmp2DD + 9},(void*)& Cyc__genrep_451};static unsigned char
_tmp2DE[8]="Label_s";static struct _tuple4 Cyc__gentuple_544={15,{_tmp2DE,_tmp2DE,
_tmp2DE + 8},(void*)& Cyc__genrep_446};static unsigned char _tmp2DF[5]="Do_s";static
struct _tuple4 Cyc__gentuple_545={16,{_tmp2DF,_tmp2DF,_tmp2DF + 5},(void*)& Cyc__genrep_441};
static unsigned char _tmp2E0[11]="TryCatch_s";static struct _tuple4 Cyc__gentuple_546={
17,{_tmp2E0,_tmp2E0,_tmp2E0 + 11},(void*)& Cyc__genrep_223};static unsigned char
_tmp2E1[9]="Region_s";static struct _tuple4 Cyc__gentuple_547={18,{_tmp2E1,_tmp2E1,
_tmp2E1 + 9},(void*)& Cyc__genrep_181};static unsigned char _tmp2E2[11]="ForArray_s";
static struct _tuple4 Cyc__gentuple_548={19,{_tmp2E2,_tmp2E2,_tmp2E2 + 11},(void*)&
Cyc__genrep_167};static struct _tuple4*Cyc__genarr_549[20]={& Cyc__gentuple_529,&
Cyc__gentuple_530,& Cyc__gentuple_531,& Cyc__gentuple_532,& Cyc__gentuple_533,& Cyc__gentuple_534,&
Cyc__gentuple_535,& Cyc__gentuple_536,& Cyc__gentuple_537,& Cyc__gentuple_538,& Cyc__gentuple_539,&
Cyc__gentuple_540,& Cyc__gentuple_541,& Cyc__gentuple_542,& Cyc__gentuple_543,& Cyc__gentuple_544,&
Cyc__gentuple_545,& Cyc__gentuple_546,& Cyc__gentuple_547,& Cyc__gentuple_548};
static unsigned char _tmp2E4[9]="Raw_stmt";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep={
5,{_tmp2E4,_tmp2E4,_tmp2E4 + 9},{(void*)((struct _tuple6**)Cyc__genarr_166),(void*)((
struct _tuple6**)Cyc__genarr_166),(void*)((struct _tuple6**)Cyc__genarr_166 + 1)},{(
void*)((struct _tuple4**)Cyc__genarr_549),(void*)((struct _tuple4**)Cyc__genarr_549),(
void*)((struct _tuple4**)Cyc__genarr_549 + 20)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_160;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep;
static unsigned char _tmp2E5[5]="List";static struct _tagged_arr Cyc__genname_164={
_tmp2E5,_tmp2E5,_tmp2E5 + 5};static unsigned char _tmp2E6[3]="hd";static struct
_tuple4 Cyc__gentuple_161={offsetof(struct Cyc_List_List,hd),{_tmp2E6,_tmp2E6,
_tmp2E6 + 3},(void*)& Cyc__genrep_159};static unsigned char _tmp2E7[3]="tl";static
struct _tuple4 Cyc__gentuple_162={offsetof(struct Cyc_List_List,tl),{_tmp2E7,
_tmp2E7,_tmp2E7 + 3},(void*)& Cyc__genrep_160};static struct _tuple4*Cyc__genarr_163[
2]={& Cyc__gentuple_161,& Cyc__gentuple_162};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_164,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_163),(void*)((struct _tuple4**)Cyc__genarr_163),(void*)((
struct _tuple4**)Cyc__genarr_163 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_160={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple7*Cyc__genarr_74[0]={};static
unsigned char _tmp2EB[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
6,{_tmp2EB,_tmp2EB,_tmp2EB + 11},{(void*)((struct _tuple7**)Cyc__genarr_74),(void*)((
struct _tuple7**)Cyc__genarr_74),(void*)((struct _tuple7**)Cyc__genarr_74 + 0)}};
static unsigned char _tmp2EC[5]="Stmt";static struct _tagged_arr Cyc__genname_556={
_tmp2EC,_tmp2EC,_tmp2EC + 5};static unsigned char _tmp2ED[2]="r";static struct
_tuple4 Cyc__gentuple_550={offsetof(struct Cyc_Absyn_Stmt,r),{_tmp2ED,_tmp2ED,
_tmp2ED + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};static unsigned char _tmp2EE[4]="loc";
static struct _tuple4 Cyc__gentuple_551={offsetof(struct Cyc_Absyn_Stmt,loc),{
_tmp2EE,_tmp2EE,_tmp2EE + 4},(void*)& Cyc__genrep_2};static unsigned char _tmp2EF[16]="non_local_preds";
static struct _tuple4 Cyc__gentuple_552={offsetof(struct Cyc_Absyn_Stmt,non_local_preds),{
_tmp2EF,_tmp2EF,_tmp2EF + 16},(void*)& Cyc__genrep_160};static unsigned char _tmp2F0[
10]="try_depth";static struct _tuple4 Cyc__gentuple_553={offsetof(struct Cyc_Absyn_Stmt,try_depth),{
_tmp2F0,_tmp2F0,_tmp2F0 + 10},(void*)((void*)& Cyc__genrep_102)};static
unsigned char _tmp2F1[6]="annot";static struct _tuple4 Cyc__gentuple_554={offsetof(
struct Cyc_Absyn_Stmt,annot),{_tmp2F1,_tmp2F1,_tmp2F1 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};
static struct _tuple4*Cyc__genarr_555[5]={& Cyc__gentuple_550,& Cyc__gentuple_551,&
Cyc__gentuple_552,& Cyc__gentuple_553,& Cyc__gentuple_554};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep={3,(struct _tagged_arr*)& Cyc__genname_556,sizeof(struct
Cyc_Absyn_Stmt),{(void*)((struct _tuple4**)Cyc__genarr_555),(void*)((struct
_tuple4**)Cyc__genarr_555),(void*)((struct _tuple4**)Cyc__genarr_555 + 5)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159={1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};
static unsigned char _tmp2F4[7]="Fndecl";static struct _tagged_arr Cyc__genname_604={
_tmp2F4,_tmp2F4,_tmp2F4 + 7};static unsigned char _tmp2F5[3]="sc";static struct
_tuple4 Cyc__gentuple_589={offsetof(struct Cyc_Absyn_Fndecl,sc),{_tmp2F5,_tmp2F5,
_tmp2F5 + 3},(void*)& Cyc_Absyn_scope_t_rep};static unsigned char _tmp2F6[10]="is_inline";
static struct _tuple4 Cyc__gentuple_590={offsetof(struct Cyc_Absyn_Fndecl,is_inline),{
_tmp2F6,_tmp2F6,_tmp2F6 + 10},(void*)((void*)& Cyc__genrep_102)};static
unsigned char _tmp2F7[5]="name";static struct _tuple4 Cyc__gentuple_591={offsetof(
struct Cyc_Absyn_Fndecl,name),{_tmp2F7,_tmp2F7,_tmp2F7 + 5},(void*)& Cyc__genrep_10};
static unsigned char _tmp2F8[4]="tvs";static struct _tuple4 Cyc__gentuple_592={
offsetof(struct Cyc_Absyn_Fndecl,tvs),{_tmp2F8,_tmp2F8,_tmp2F8 + 4},(void*)& Cyc__genrep_254};
static unsigned char _tmp2F9[7]="effect";static struct _tuple4 Cyc__gentuple_593={
offsetof(struct Cyc_Absyn_Fndecl,effect),{_tmp2F9,_tmp2F9,_tmp2F9 + 7},(void*)& Cyc__genrep_61};
static unsigned char _tmp2FA[9]="ret_type";static struct _tuple4 Cyc__gentuple_594={
offsetof(struct Cyc_Absyn_Fndecl,ret_type),{_tmp2FA,_tmp2FA,_tmp2FA + 9},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static unsigned char _tmp2FB[5]="args";static struct
_tuple4 Cyc__gentuple_595={offsetof(struct Cyc_Absyn_Fndecl,args),{_tmp2FB,_tmp2FB,
_tmp2FB + 5},(void*)& Cyc__genrep_578};static unsigned char _tmp2FC[10]="c_varargs";
static struct _tuple4 Cyc__gentuple_596={offsetof(struct Cyc_Absyn_Fndecl,c_varargs),{
_tmp2FC,_tmp2FC,_tmp2FC + 10},(void*)((void*)& Cyc__genrep_102)};static
unsigned char _tmp2FD[12]="cyc_varargs";static struct _tuple4 Cyc__gentuple_597={
offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),{_tmp2FD,_tmp2FD,_tmp2FD + 12},(void*)&
Cyc__genrep_567};static unsigned char _tmp2FE[7]="rgn_po";static struct _tuple4 Cyc__gentuple_598={
offsetof(struct Cyc_Absyn_Fndecl,rgn_po),{_tmp2FE,_tmp2FE,_tmp2FE + 7},(void*)& Cyc__genrep_557};
static unsigned char _tmp2FF[5]="body";static struct _tuple4 Cyc__gentuple_599={
offsetof(struct Cyc_Absyn_Fndecl,body),{_tmp2FF,_tmp2FF,_tmp2FF + 5},(void*)& Cyc__genrep_159};
static unsigned char _tmp300[11]="cached_typ";static struct _tuple4 Cyc__gentuple_600={
offsetof(struct Cyc_Absyn_Fndecl,cached_typ),{_tmp300,_tmp300,_tmp300 + 11},(void*)&
Cyc__genrep_61};static unsigned char _tmp301[15]="param_vardecls";static struct
_tuple4 Cyc__gentuple_601={offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),{
_tmp301,_tmp301,_tmp301 + 15},(void*)& Cyc__genrep_129};static unsigned char _tmp302[
11]="attributes";static struct _tuple4 Cyc__gentuple_602={offsetof(struct Cyc_Absyn_Fndecl,attributes),{
_tmp302,_tmp302,_tmp302 + 11},(void*)& Cyc__genrep_83};static struct _tuple4*Cyc__genarr_603[
14]={& Cyc__gentuple_589,& Cyc__gentuple_590,& Cyc__gentuple_591,& Cyc__gentuple_592,&
Cyc__gentuple_593,& Cyc__gentuple_594,& Cyc__gentuple_595,& Cyc__gentuple_596,& Cyc__gentuple_597,&
Cyc__gentuple_598,& Cyc__gentuple_599,& Cyc__gentuple_600,& Cyc__gentuple_601,& Cyc__gentuple_602};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep={3,(struct _tagged_arr*)&
Cyc__genname_604,sizeof(struct Cyc_Absyn_Fndecl),{(void*)((struct _tuple4**)Cyc__genarr_603),(
void*)((struct _tuple4**)Cyc__genarr_603),(void*)((struct _tuple4**)Cyc__genarr_603
+ 14)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_82={1,1,(void*)((void*)&
Cyc_struct_Absyn_Fndecl_rep)};struct _tuple53{unsigned int f1;struct Cyc_Absyn_Fndecl*
f2;};static struct _tuple5 Cyc__gentuple_605={offsetof(struct _tuple53,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_606={offsetof(struct _tuple53,f2),(
void*)& Cyc__genrep_82};static struct _tuple5*Cyc__genarr_607[2]={& Cyc__gentuple_605,&
Cyc__gentuple_606};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_81={4,sizeof(
struct _tuple53),{(void*)((struct _tuple5**)Cyc__genarr_607),(void*)((struct
_tuple5**)Cyc__genarr_607),(void*)((struct _tuple5**)Cyc__genarr_607 + 2)}};static
unsigned char _tmp306[13]="Unresolved_b";static struct _tuple6 Cyc__gentuple_811={0,{
_tmp306,_tmp306,_tmp306 + 13}};static struct _tuple6*Cyc__genarr_812[1]={& Cyc__gentuple_811};
static unsigned char _tmp307[9]="Global_b";static struct _tuple4 Cyc__gentuple_813={0,{
_tmp307,_tmp307,_tmp307 + 9},(void*)& Cyc__genrep_381};static unsigned char _tmp308[
10]="Funname_b";static struct _tuple4 Cyc__gentuple_814={1,{_tmp308,_tmp308,_tmp308
+ 10},(void*)& Cyc__genrep_81};static unsigned char _tmp309[8]="Param_b";static
struct _tuple4 Cyc__gentuple_815={2,{_tmp309,_tmp309,_tmp309 + 8},(void*)& Cyc__genrep_381};
static unsigned char _tmp30A[8]="Local_b";static struct _tuple4 Cyc__gentuple_816={3,{
_tmp30A,_tmp30A,_tmp30A + 8},(void*)& Cyc__genrep_381};static unsigned char _tmp30B[
6]="Pat_b";static struct _tuple4 Cyc__gentuple_817={4,{_tmp30B,_tmp30B,_tmp30B + 6},(
void*)& Cyc__genrep_381};static struct _tuple4*Cyc__genarr_818[5]={& Cyc__gentuple_813,&
Cyc__gentuple_814,& Cyc__gentuple_815,& Cyc__gentuple_816,& Cyc__gentuple_817};
static unsigned char _tmp30D[8]="Binding";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep={
5,{_tmp30D,_tmp30D,_tmp30D + 8},{(void*)((struct _tuple6**)Cyc__genarr_812),(void*)((
struct _tuple6**)Cyc__genarr_812),(void*)((struct _tuple6**)Cyc__genarr_812 + 1)},{(
void*)((struct _tuple4**)Cyc__genarr_818),(void*)((struct _tuple4**)Cyc__genarr_818),(
void*)((struct _tuple4**)Cyc__genarr_818 + 5)}};struct _tuple54{unsigned int f1;
struct _tuple0*f2;void*f3;};static struct _tuple5 Cyc__gentuple_819={offsetof(struct
_tuple54,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_820={
offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_10};static struct _tuple5 Cyc__gentuple_821={
offsetof(struct _tuple54,f3),(void*)& Cyc_Absyn_binding_t_rep};static struct _tuple5*
Cyc__genarr_822[3]={& Cyc__gentuple_819,& Cyc__gentuple_820,& Cyc__gentuple_821};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_810={4,sizeof(struct _tuple54),{(
void*)((struct _tuple5**)Cyc__genarr_822),(void*)((struct _tuple5**)Cyc__genarr_822),(
void*)((struct _tuple5**)Cyc__genarr_822 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_805;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep;
static unsigned char _tmp30F[5]="Plus";static struct _tuple6 Cyc__gentuple_775={0,{
_tmp30F,_tmp30F,_tmp30F + 5}};static unsigned char _tmp310[6]="Times";static struct
_tuple6 Cyc__gentuple_776={1,{_tmp310,_tmp310,_tmp310 + 6}};static unsigned char
_tmp311[6]="Minus";static struct _tuple6 Cyc__gentuple_777={2,{_tmp311,_tmp311,
_tmp311 + 6}};static unsigned char _tmp312[4]="Div";static struct _tuple6 Cyc__gentuple_778={
3,{_tmp312,_tmp312,_tmp312 + 4}};static unsigned char _tmp313[4]="Mod";static struct
_tuple6 Cyc__gentuple_779={4,{_tmp313,_tmp313,_tmp313 + 4}};static unsigned char
_tmp314[3]="Eq";static struct _tuple6 Cyc__gentuple_780={5,{_tmp314,_tmp314,_tmp314
+ 3}};static unsigned char _tmp315[4]="Neq";static struct _tuple6 Cyc__gentuple_781={
6,{_tmp315,_tmp315,_tmp315 + 4}};static unsigned char _tmp316[3]="Gt";static struct
_tuple6 Cyc__gentuple_782={7,{_tmp316,_tmp316,_tmp316 + 3}};static unsigned char
_tmp317[3]="Lt";static struct _tuple6 Cyc__gentuple_783={8,{_tmp317,_tmp317,_tmp317
+ 3}};static unsigned char _tmp318[4]="Gte";static struct _tuple6 Cyc__gentuple_784={
9,{_tmp318,_tmp318,_tmp318 + 4}};static unsigned char _tmp319[4]="Lte";static struct
_tuple6 Cyc__gentuple_785={10,{_tmp319,_tmp319,_tmp319 + 4}};static unsigned char
_tmp31A[4]="Not";static struct _tuple6 Cyc__gentuple_786={11,{_tmp31A,_tmp31A,
_tmp31A + 4}};static unsigned char _tmp31B[7]="Bitnot";static struct _tuple6 Cyc__gentuple_787={
12,{_tmp31B,_tmp31B,_tmp31B + 7}};static unsigned char _tmp31C[7]="Bitand";static
struct _tuple6 Cyc__gentuple_788={13,{_tmp31C,_tmp31C,_tmp31C + 7}};static
unsigned char _tmp31D[6]="Bitor";static struct _tuple6 Cyc__gentuple_789={14,{
_tmp31D,_tmp31D,_tmp31D + 6}};static unsigned char _tmp31E[7]="Bitxor";static struct
_tuple6 Cyc__gentuple_790={15,{_tmp31E,_tmp31E,_tmp31E + 7}};static unsigned char
_tmp31F[10]="Bitlshift";static struct _tuple6 Cyc__gentuple_791={16,{_tmp31F,
_tmp31F,_tmp31F + 10}};static unsigned char _tmp320[11]="Bitlrshift";static struct
_tuple6 Cyc__gentuple_792={17,{_tmp320,_tmp320,_tmp320 + 11}};static unsigned char
_tmp321[11]="Bitarshift";static struct _tuple6 Cyc__gentuple_793={18,{_tmp321,
_tmp321,_tmp321 + 11}};static unsigned char _tmp322[5]="Size";static struct _tuple6
Cyc__gentuple_794={19,{_tmp322,_tmp322,_tmp322 + 5}};static struct _tuple6*Cyc__genarr_795[
20]={& Cyc__gentuple_775,& Cyc__gentuple_776,& Cyc__gentuple_777,& Cyc__gentuple_778,&
Cyc__gentuple_779,& Cyc__gentuple_780,& Cyc__gentuple_781,& Cyc__gentuple_782,& Cyc__gentuple_783,&
Cyc__gentuple_784,& Cyc__gentuple_785,& Cyc__gentuple_786,& Cyc__gentuple_787,& Cyc__gentuple_788,&
Cyc__gentuple_789,& Cyc__gentuple_790,& Cyc__gentuple_791,& Cyc__gentuple_792,& Cyc__gentuple_793,&
Cyc__gentuple_794};static struct _tuple4*Cyc__genarr_796[0]={};static unsigned char
_tmp324[7]="Primop";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={5,{
_tmp324,_tmp324,_tmp324 + 7},{(void*)((struct _tuple6**)Cyc__genarr_795),(void*)((
struct _tuple6**)Cyc__genarr_795),(void*)((struct _tuple6**)Cyc__genarr_795 + 20)},{(
void*)((struct _tuple4**)Cyc__genarr_796),(void*)((struct _tuple4**)Cyc__genarr_796),(
void*)((struct _tuple4**)Cyc__genarr_796 + 0)}};struct _tuple55{unsigned int f1;void*
f2;struct Cyc_List_List*f3;};static struct _tuple5 Cyc__gentuple_806={offsetof(
struct _tuple55,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_807={
offsetof(struct _tuple55,f2),(void*)& Cyc_Absyn_primop_t_rep};static struct _tuple5
Cyc__gentuple_808={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_462};static
struct _tuple5*Cyc__genarr_809[3]={& Cyc__gentuple_806,& Cyc__gentuple_807,& Cyc__gentuple_808};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_805={4,sizeof(struct _tuple55),{(
void*)((struct _tuple5**)Cyc__genarr_809),(void*)((struct _tuple5**)Cyc__genarr_809),(
void*)((struct _tuple5**)Cyc__genarr_809 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_773;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_774;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;static
unsigned char _tmp326[4]="Opt";static struct _tagged_arr Cyc__genname_799={_tmp326,
_tmp326,_tmp326 + 4};static unsigned char _tmp327[2]="v";static struct _tuple4 Cyc__gentuple_797={
offsetof(struct Cyc_Core_Opt,v),{_tmp327,_tmp327,_tmp327 + 2},(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple4*Cyc__genarr_798[1]={& Cyc__gentuple_797};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_primop_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_799,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple4**)Cyc__genarr_798),(void*)((
struct _tuple4**)Cyc__genarr_798),(void*)((struct _tuple4**)Cyc__genarr_798 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_774={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple56{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple5 Cyc__gentuple_800={offsetof(struct
_tuple56,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_801={
offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_77};static struct _tuple5 Cyc__gentuple_802={
offsetof(struct _tuple56,f3),(void*)& Cyc__genrep_774};static struct _tuple5 Cyc__gentuple_803={
offsetof(struct _tuple56,f4),(void*)& Cyc__genrep_77};static struct _tuple5*Cyc__genarr_804[
4]={& Cyc__gentuple_800,& Cyc__gentuple_801,& Cyc__gentuple_802,& Cyc__gentuple_803};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_773={4,sizeof(struct _tuple56),{(
void*)((struct _tuple5**)Cyc__genarr_804),(void*)((struct _tuple5**)Cyc__genarr_804),(
void*)((struct _tuple5**)Cyc__genarr_804 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_762;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep;
static unsigned char _tmp32B[7]="PreInc";static struct _tuple6 Cyc__gentuple_763={0,{
_tmp32B,_tmp32B,_tmp32B + 7}};static unsigned char _tmp32C[8]="PostInc";static
struct _tuple6 Cyc__gentuple_764={1,{_tmp32C,_tmp32C,_tmp32C + 8}};static
unsigned char _tmp32D[7]="PreDec";static struct _tuple6 Cyc__gentuple_765={2,{
_tmp32D,_tmp32D,_tmp32D + 7}};static unsigned char _tmp32E[8]="PostDec";static
struct _tuple6 Cyc__gentuple_766={3,{_tmp32E,_tmp32E,_tmp32E + 8}};static struct
_tuple6*Cyc__genarr_767[4]={& Cyc__gentuple_763,& Cyc__gentuple_764,& Cyc__gentuple_765,&
Cyc__gentuple_766};static struct _tuple4*Cyc__genarr_768[0]={};static unsigned char
_tmp330[12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={
5,{_tmp330,_tmp330,_tmp330 + 12},{(void*)((struct _tuple6**)Cyc__genarr_767),(void*)((
struct _tuple6**)Cyc__genarr_767),(void*)((struct _tuple6**)Cyc__genarr_767 + 4)},{(
void*)((struct _tuple4**)Cyc__genarr_768),(void*)((struct _tuple4**)Cyc__genarr_768),(
void*)((struct _tuple4**)Cyc__genarr_768 + 0)}};struct _tuple57{unsigned int f1;
struct Cyc_Absyn_Exp*f2;void*f3;};static struct _tuple5 Cyc__gentuple_769={offsetof(
struct _tuple57,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_770={
offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_77};static struct _tuple5 Cyc__gentuple_771={
offsetof(struct _tuple57,f3),(void*)& Cyc_Absyn_incrementor_t_rep};static struct
_tuple5*Cyc__genarr_772[3]={& Cyc__gentuple_769,& Cyc__gentuple_770,& Cyc__gentuple_771};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_762={4,sizeof(struct _tuple57),{(
void*)((struct _tuple5**)Cyc__genarr_772),(void*)((struct _tuple5**)Cyc__genarr_772),(
void*)((struct _tuple5**)Cyc__genarr_772 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_756;struct _tuple58{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;struct Cyc_Absyn_Exp*f4;};static struct _tuple5 Cyc__gentuple_757={offsetof(
struct _tuple58,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_758={
offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_77};static struct _tuple5 Cyc__gentuple_759={
offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_77};static struct _tuple5 Cyc__gentuple_760={
offsetof(struct _tuple58,f4),(void*)& Cyc__genrep_77};static struct _tuple5*Cyc__genarr_761[
4]={& Cyc__gentuple_757,& Cyc__gentuple_758,& Cyc__gentuple_759,& Cyc__gentuple_760};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_756={4,sizeof(struct _tuple58),{(
void*)((struct _tuple5**)Cyc__genarr_761),(void*)((struct _tuple5**)Cyc__genarr_761),(
void*)((struct _tuple5**)Cyc__genarr_761 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_695;struct _tuple59{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;};static struct _tuple5 Cyc__gentuple_696={offsetof(struct _tuple59,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_697={offsetof(struct _tuple59,f2),(
void*)& Cyc__genrep_77};static struct _tuple5 Cyc__gentuple_698={offsetof(struct
_tuple59,f3),(void*)& Cyc__genrep_77};static struct _tuple5*Cyc__genarr_699[3]={&
Cyc__gentuple_696,& Cyc__gentuple_697,& Cyc__gentuple_698};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_695={4,sizeof(struct _tuple59),{(void*)((struct _tuple5**)Cyc__genarr_699),(
void*)((struct _tuple5**)Cyc__genarr_699),(void*)((struct _tuple5**)Cyc__genarr_699
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_751;static struct _tuple5
Cyc__gentuple_752={offsetof(struct _tuple46,f1),(void*)& Cyc__genrep_5};static
struct _tuple5 Cyc__gentuple_753={offsetof(struct _tuple46,f2),(void*)& Cyc__genrep_77};
static struct _tuple5 Cyc__gentuple_754={offsetof(struct _tuple46,f3),(void*)& Cyc__genrep_462};
static struct _tuple5*Cyc__genarr_755[3]={& Cyc__gentuple_752,& Cyc__gentuple_753,&
Cyc__gentuple_754};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_751={4,
sizeof(struct _tuple46),{(void*)((struct _tuple5**)Cyc__genarr_755),(void*)((
struct _tuple5**)Cyc__genarr_755),(void*)((struct _tuple5**)Cyc__genarr_755 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_738;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_739;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_740;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_740={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};static
unsigned char _tmp336[15]="VarargCallInfo";static struct _tagged_arr Cyc__genname_745={
_tmp336,_tmp336,_tmp336 + 15};static unsigned char _tmp337[12]="num_varargs";static
struct _tuple4 Cyc__gentuple_741={offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),{
_tmp337,_tmp337,_tmp337 + 12},(void*)((void*)& Cyc__genrep_102)};static
unsigned char _tmp338[10]="injectors";static struct _tuple4 Cyc__gentuple_742={
offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),{_tmp338,_tmp338,_tmp338 + 10},(
void*)& Cyc__genrep_320};static unsigned char _tmp339[4]="vai";static struct _tuple4
Cyc__gentuple_743={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),{_tmp339,_tmp339,
_tmp339 + 4},(void*)& Cyc__genrep_740};static struct _tuple4*Cyc__genarr_744[3]={&
Cyc__gentuple_741,& Cyc__gentuple_742,& Cyc__gentuple_743};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_call_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_745,
sizeof(struct Cyc_Absyn_VarargCallInfo),{(void*)((struct _tuple4**)Cyc__genarr_744),(
void*)((struct _tuple4**)Cyc__genarr_744),(void*)((struct _tuple4**)Cyc__genarr_744
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_739={1,1,(void*)((void*)&
Cyc_Absyn_vararg_call_info_t_rep)};struct _tuple60{unsigned int f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_VarargCallInfo*f4;};static struct
_tuple5 Cyc__gentuple_746={offsetof(struct _tuple60,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_747={offsetof(struct _tuple60,f2),(void*)& Cyc__genrep_77};
static struct _tuple5 Cyc__gentuple_748={offsetof(struct _tuple60,f3),(void*)& Cyc__genrep_462};
static struct _tuple5 Cyc__gentuple_749={offsetof(struct _tuple60,f4),(void*)& Cyc__genrep_739};
static struct _tuple5*Cyc__genarr_750[4]={& Cyc__gentuple_746,& Cyc__gentuple_747,&
Cyc__gentuple_748,& Cyc__gentuple_749};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_738={
4,sizeof(struct _tuple60),{(void*)((struct _tuple5**)Cyc__genarr_750),(void*)((
struct _tuple5**)Cyc__genarr_750),(void*)((struct _tuple5**)Cyc__genarr_750 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_733;static struct _tuple5 Cyc__gentuple_734={
offsetof(struct _tuple46,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_735={
offsetof(struct _tuple46,f2),(void*)& Cyc__genrep_77};static struct _tuple5 Cyc__gentuple_736={
offsetof(struct _tuple46,f3),(void*)& Cyc__genrep_52};static struct _tuple5*Cyc__genarr_737[
3]={& Cyc__gentuple_734,& Cyc__gentuple_735,& Cyc__gentuple_736};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_733={4,sizeof(struct _tuple46),{(void*)((struct _tuple5**)Cyc__genarr_737),(
void*)((struct _tuple5**)Cyc__genarr_737),(void*)((struct _tuple5**)Cyc__genarr_737
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_728;struct _tuple61{
unsigned int f1;void*f2;struct Cyc_Absyn_Exp*f3;};static struct _tuple5 Cyc__gentuple_729={
offsetof(struct _tuple61,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_730={
offsetof(struct _tuple61,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5 Cyc__gentuple_731={offsetof(struct _tuple61,f3),(void*)& Cyc__genrep_77};
static struct _tuple5*Cyc__genarr_732[3]={& Cyc__gentuple_729,& Cyc__gentuple_730,&
Cyc__gentuple_731};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_728={4,
sizeof(struct _tuple61),{(void*)((struct _tuple5**)Cyc__genarr_732),(void*)((
struct _tuple5**)Cyc__genarr_732),(void*)((struct _tuple5**)Cyc__genarr_732 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_723;static struct _tuple5 Cyc__gentuple_724={
offsetof(struct _tuple59,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_725={
offsetof(struct _tuple59,f2),(void*)& Cyc__genrep_73};static struct _tuple5 Cyc__gentuple_726={
offsetof(struct _tuple59,f3),(void*)& Cyc__genrep_77};static struct _tuple5*Cyc__genarr_727[
3]={& Cyc__gentuple_724,& Cyc__gentuple_725,& Cyc__gentuple_726};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_723={4,sizeof(struct _tuple59),{(void*)((struct _tuple5**)Cyc__genarr_727),(
void*)((struct _tuple5**)Cyc__genarr_727),(void*)((struct _tuple5**)Cyc__genarr_727
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_47;static struct _tuple5 Cyc__gentuple_48={
offsetof(struct _tuple5,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_49={
offsetof(struct _tuple5,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5*Cyc__genarr_50[2]={& Cyc__gentuple_48,& Cyc__gentuple_49};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_47={4,sizeof(struct _tuple5),{(void*)((struct _tuple5**)Cyc__genarr_50),(
void*)((struct _tuple5**)Cyc__genarr_50),(void*)((struct _tuple5**)Cyc__genarr_50 + 
2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_710;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_712;
struct _tuple62{unsigned int f1;unsigned int f2;};static struct _tuple5 Cyc__gentuple_713={
offsetof(struct _tuple62,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_714={
offsetof(struct _tuple62,f2),(void*)& Cyc__genrep_5};static struct _tuple5*Cyc__genarr_715[
2]={& Cyc__gentuple_713,& Cyc__gentuple_714};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_712={4,sizeof(struct _tuple62),{(void*)((struct _tuple5**)Cyc__genarr_715),(
void*)((struct _tuple5**)Cyc__genarr_715),(void*)((struct _tuple5**)Cyc__genarr_715
+ 2)}};static struct _tuple6*Cyc__genarr_711[0]={};static unsigned char _tmp342[12]="StructField";
static struct _tuple4 Cyc__gentuple_716={0,{_tmp342,_tmp342,_tmp342 + 12},(void*)&
Cyc__genrep_236};static unsigned char _tmp343[11]="TupleIndex";static struct _tuple4
Cyc__gentuple_717={1,{_tmp343,_tmp343,_tmp343 + 11},(void*)& Cyc__genrep_712};
static struct _tuple4*Cyc__genarr_718[2]={& Cyc__gentuple_716,& Cyc__gentuple_717};
static unsigned char _tmp345[14]="OffsetofField";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep={5,{_tmp345,_tmp345,_tmp345 + 14},{(void*)((struct
_tuple6**)Cyc__genarr_711),(void*)((struct _tuple6**)Cyc__genarr_711),(void*)((
struct _tuple6**)Cyc__genarr_711 + 0)},{(void*)((struct _tuple4**)Cyc__genarr_718),(
void*)((struct _tuple4**)Cyc__genarr_718),(void*)((struct _tuple4**)Cyc__genarr_718
+ 2)}};struct _tuple63{unsigned int f1;void*f2;void*f3;};static struct _tuple5 Cyc__gentuple_719={
offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_720={
offsetof(struct _tuple63,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5 Cyc__gentuple_721={offsetof(struct _tuple63,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};
static struct _tuple5*Cyc__genarr_722[3]={& Cyc__gentuple_719,& Cyc__gentuple_720,&
Cyc__gentuple_721};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_710={4,
sizeof(struct _tuple63),{(void*)((struct _tuple5**)Cyc__genarr_722),(void*)((
struct _tuple5**)Cyc__genarr_722),(void*)((struct _tuple5**)Cyc__genarr_722 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_705;struct _tuple64{unsigned int
f1;struct Cyc_List_List*f2;void*f3;};static struct _tuple5 Cyc__gentuple_706={
offsetof(struct _tuple64,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_707={
offsetof(struct _tuple64,f2),(void*)& Cyc__genrep_254};static struct _tuple5 Cyc__gentuple_708={
offsetof(struct _tuple64,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5*Cyc__genarr_709[3]={& Cyc__gentuple_706,& Cyc__gentuple_707,& Cyc__gentuple_708};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_705={4,sizeof(struct _tuple64),{(
void*)((struct _tuple5**)Cyc__genarr_709),(void*)((struct _tuple5**)Cyc__genarr_709),(
void*)((struct _tuple5**)Cyc__genarr_709 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_700;struct _tuple65{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct
_tagged_arr*f3;};static struct _tuple5 Cyc__gentuple_701={offsetof(struct _tuple65,f1),(
void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_702={offsetof(struct
_tuple65,f2),(void*)& Cyc__genrep_77};static struct _tuple5 Cyc__gentuple_703={
offsetof(struct _tuple65,f3),(void*)& Cyc__genrep_12};static struct _tuple5*Cyc__genarr_704[
3]={& Cyc__gentuple_701,& Cyc__gentuple_702,& Cyc__gentuple_703};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_700={4,sizeof(struct _tuple65),{(void*)((struct _tuple5**)Cyc__genarr_704),(
void*)((struct _tuple5**)Cyc__genarr_704),(void*)((struct _tuple5**)Cyc__genarr_704
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_691;static struct _tuple5
Cyc__gentuple_692={offsetof(struct _tuple10,f1),(void*)& Cyc__genrep_5};static
struct _tuple5 Cyc__gentuple_693={offsetof(struct _tuple10,f2),(void*)& Cyc__genrep_462};
static struct _tuple5*Cyc__genarr_694[2]={& Cyc__gentuple_692,& Cyc__gentuple_693};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_691={4,sizeof(struct _tuple10),{(
void*)((struct _tuple5**)Cyc__genarr_694),(void*)((struct _tuple5**)Cyc__genarr_694),(
void*)((struct _tuple5**)Cyc__genarr_694 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_680;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_681;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_682;static struct _tuple5 Cyc__gentuple_683={
offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_568};static struct _tuple5 Cyc__gentuple_684={
offsetof(struct _tuple1,f2),(void*)& Cyc__genrep_132};static struct _tuple5 Cyc__gentuple_685={
offsetof(struct _tuple1,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5*Cyc__genarr_686[3]={& Cyc__gentuple_683,& Cyc__gentuple_684,& Cyc__gentuple_685};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_682={4,sizeof(struct _tuple1),{(
void*)((struct _tuple5**)Cyc__genarr_686),(void*)((struct _tuple5**)Cyc__genarr_686),(
void*)((struct _tuple5**)Cyc__genarr_686 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_681={1,1,(void*)((void*)& Cyc__genrep_682)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_609;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_610;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_611;static struct _tuple5 Cyc__gentuple_612={offsetof(struct _tuple9,f1),(
void*)& Cyc__genrep_234};static struct _tuple5 Cyc__gentuple_613={offsetof(struct
_tuple9,f2),(void*)& Cyc__genrep_77};static struct _tuple5*Cyc__genarr_614[2]={& Cyc__gentuple_612,&
Cyc__gentuple_613};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_611={4,
sizeof(struct _tuple9),{(void*)((struct _tuple5**)Cyc__genarr_614),(void*)((struct
_tuple5**)Cyc__genarr_614),(void*)((struct _tuple5**)Cyc__genarr_614 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_610={1,1,(void*)((void*)& Cyc__genrep_611)};
static unsigned char _tmp34E[5]="List";static struct _tagged_arr Cyc__genname_618={
_tmp34E,_tmp34E,_tmp34E + 5};static unsigned char _tmp34F[3]="hd";static struct
_tuple4 Cyc__gentuple_615={offsetof(struct Cyc_List_List,hd),{_tmp34F,_tmp34F,
_tmp34F + 3},(void*)& Cyc__genrep_610};static unsigned char _tmp350[3]="tl";static
struct _tuple4 Cyc__gentuple_616={offsetof(struct Cyc_List_List,tl),{_tmp350,
_tmp350,_tmp350 + 3},(void*)& Cyc__genrep_609};static struct _tuple4*Cyc__genarr_617[
2]={& Cyc__gentuple_615,& Cyc__gentuple_616};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_618,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_617),(void*)((struct _tuple4**)Cyc__genarr_617),(void*)((
struct _tuple4**)Cyc__genarr_617 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_609={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple66{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple5 Cyc__gentuple_687={offsetof(struct _tuple66,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_688={offsetof(struct _tuple66,f2),(void*)& Cyc__genrep_681};
static struct _tuple5 Cyc__gentuple_689={offsetof(struct _tuple66,f3),(void*)& Cyc__genrep_609};
static struct _tuple5*Cyc__genarr_690[3]={& Cyc__gentuple_687,& Cyc__gentuple_688,&
Cyc__gentuple_689};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_680={4,
sizeof(struct _tuple66),{(void*)((struct _tuple5**)Cyc__genarr_690),(void*)((
struct _tuple5**)Cyc__genarr_690),(void*)((struct _tuple5**)Cyc__genarr_690 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_676;static struct _tuple5 Cyc__gentuple_677={
offsetof(struct _tuple10,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_678={
offsetof(struct _tuple10,f2),(void*)& Cyc__genrep_609};static struct _tuple5*Cyc__genarr_679[
2]={& Cyc__gentuple_677,& Cyc__gentuple_678};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_676={4,sizeof(struct _tuple10),{(void*)((struct _tuple5**)Cyc__genarr_679),(
void*)((struct _tuple5**)Cyc__genarr_679),(void*)((struct _tuple5**)Cyc__genarr_679
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_670;struct _tuple67{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple5 Cyc__gentuple_671={offsetof(struct _tuple67,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_672={offsetof(struct _tuple67,f2),(
void*)& Cyc__genrep_131};static struct _tuple5 Cyc__gentuple_673={offsetof(struct
_tuple67,f3),(void*)& Cyc__genrep_77};static struct _tuple5 Cyc__gentuple_674={
offsetof(struct _tuple67,f4),(void*)& Cyc__genrep_77};static struct _tuple5*Cyc__genarr_675[
4]={& Cyc__gentuple_671,& Cyc__gentuple_672,& Cyc__gentuple_673,& Cyc__gentuple_674};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_670={4,sizeof(struct _tuple67),{(
void*)((struct _tuple5**)Cyc__genarr_675),(void*)((struct _tuple5**)Cyc__genarr_675),(
void*)((struct _tuple5**)Cyc__genarr_675 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_662;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_663;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_663={1,1,(void*)((void*)& Cyc_struct_Absyn_Structdecl_rep)};
struct _tuple68{unsigned int f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;struct Cyc_List_List*
f4;struct Cyc_Absyn_Structdecl*f5;};static struct _tuple5 Cyc__gentuple_664={
offsetof(struct _tuple68,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_665={
offsetof(struct _tuple68,f2),(void*)& Cyc__genrep_10};static struct _tuple5 Cyc__gentuple_666={
offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_342};static struct _tuple5 Cyc__gentuple_667={
offsetof(struct _tuple68,f4),(void*)& Cyc__genrep_609};static struct _tuple5 Cyc__gentuple_668={
offsetof(struct _tuple68,f5),(void*)& Cyc__genrep_663};static struct _tuple5*Cyc__genarr_669[
5]={& Cyc__gentuple_664,& Cyc__gentuple_665,& Cyc__gentuple_666,& Cyc__gentuple_667,&
Cyc__gentuple_668};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_662={4,
sizeof(struct _tuple68),{(void*)((struct _tuple5**)Cyc__genarr_669),(void*)((
struct _tuple5**)Cyc__genarr_669),(void*)((struct _tuple5**)Cyc__genarr_669 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_657;static struct _tuple5 Cyc__gentuple_658={
offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_659={
offsetof(struct _tuple55,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5 Cyc__gentuple_660={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_609};
static struct _tuple5*Cyc__genarr_661[3]={& Cyc__gentuple_658,& Cyc__gentuple_659,&
Cyc__gentuple_660};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_657={4,
sizeof(struct _tuple55),{(void*)((struct _tuple5**)Cyc__genarr_661),(void*)((
struct _tuple5**)Cyc__genarr_661),(void*)((struct _tuple5**)Cyc__genarr_661 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_649;struct _tuple69{unsigned int
f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;struct Cyc_List_List*f4;struct Cyc_Absyn_Tuniondecl*
f5;struct Cyc_Absyn_Tunionfield*f6;};static struct _tuple5 Cyc__gentuple_650={
offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_651={
offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_342};static struct _tuple5 Cyc__gentuple_652={
offsetof(struct _tuple69,f3),(void*)& Cyc__genrep_342};static struct _tuple5 Cyc__gentuple_653={
offsetof(struct _tuple69,f4),(void*)& Cyc__genrep_462};static struct _tuple5 Cyc__gentuple_654={
offsetof(struct _tuple69,f5),(void*)((void*)& Cyc__genrep_318)};static struct
_tuple5 Cyc__gentuple_655={offsetof(struct _tuple69,f6),(void*)& Cyc__genrep_300};
static struct _tuple5*Cyc__genarr_656[6]={& Cyc__gentuple_650,& Cyc__gentuple_651,&
Cyc__gentuple_652,& Cyc__gentuple_653,& Cyc__gentuple_654,& Cyc__gentuple_655};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_649={4,sizeof(struct _tuple69),{(
void*)((struct _tuple5**)Cyc__genarr_656),(void*)((struct _tuple5**)Cyc__genarr_656),(
void*)((struct _tuple5**)Cyc__genarr_656 + 6)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_642;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_643;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_643={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_636;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_636={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple70{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple5 Cyc__gentuple_644={offsetof(struct _tuple70,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_645={offsetof(struct _tuple70,f2),(
void*)& Cyc__genrep_10};static struct _tuple5 Cyc__gentuple_646={offsetof(struct
_tuple70,f3),(void*)& Cyc__genrep_643};static struct _tuple5 Cyc__gentuple_647={
offsetof(struct _tuple70,f4),(void*)& Cyc__genrep_636};static struct _tuple5*Cyc__genarr_648[
4]={& Cyc__gentuple_644,& Cyc__gentuple_645,& Cyc__gentuple_646,& Cyc__gentuple_647};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_642={4,sizeof(struct _tuple70),{(
void*)((struct _tuple5**)Cyc__genarr_648),(void*)((struct _tuple5**)Cyc__genarr_648),(
void*)((struct _tuple5**)Cyc__genarr_648 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_635;struct _tuple71{unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple5 Cyc__gentuple_637={offsetof(struct _tuple71,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_638={offsetof(struct _tuple71,f2),(
void*)& Cyc__genrep_10};static struct _tuple5 Cyc__gentuple_639={offsetof(struct
_tuple71,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple5 Cyc__gentuple_640={
offsetof(struct _tuple71,f4),(void*)& Cyc__genrep_636};static struct _tuple5*Cyc__genarr_641[
4]={& Cyc__gentuple_637,& Cyc__gentuple_638,& Cyc__gentuple_639,& Cyc__gentuple_640};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_635={4,sizeof(struct _tuple71),{(
void*)((struct _tuple5**)Cyc__genarr_641),(void*)((struct _tuple5**)Cyc__genarr_641),(
void*)((struct _tuple5**)Cyc__genarr_641 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_623;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_624;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_624={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};static unsigned char
_tmp35F[11]="MallocInfo";static struct _tagged_arr Cyc__genname_631={_tmp35F,
_tmp35F,_tmp35F + 11};static unsigned char _tmp360[10]="is_calloc";static struct
_tuple4 Cyc__gentuple_625={offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),{
_tmp360,_tmp360,_tmp360 + 10},(void*)((void*)& Cyc__genrep_102)};static
unsigned char _tmp361[4]="rgn";static struct _tuple4 Cyc__gentuple_626={offsetof(
struct Cyc_Absyn_MallocInfo,rgn),{_tmp361,_tmp361,_tmp361 + 4},(void*)& Cyc__genrep_73};
static unsigned char _tmp362[9]="elt_type";static struct _tuple4 Cyc__gentuple_627={
offsetof(struct Cyc_Absyn_MallocInfo,elt_type),{_tmp362,_tmp362,_tmp362 + 9},(void*)&
Cyc__genrep_624};static unsigned char _tmp363[9]="num_elts";static struct _tuple4 Cyc__gentuple_628={
offsetof(struct Cyc_Absyn_MallocInfo,num_elts),{_tmp363,_tmp363,_tmp363 + 9},(void*)&
Cyc__genrep_77};static unsigned char _tmp364[11]="fat_result";static struct _tuple4
Cyc__gentuple_629={offsetof(struct Cyc_Absyn_MallocInfo,fat_result),{_tmp364,
_tmp364,_tmp364 + 11},(void*)((void*)& Cyc__genrep_102)};static struct _tuple4*Cyc__genarr_630[
5]={& Cyc__gentuple_625,& Cyc__gentuple_626,& Cyc__gentuple_627,& Cyc__gentuple_628,&
Cyc__gentuple_629};struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep={3,(
struct _tagged_arr*)& Cyc__genname_631,sizeof(struct Cyc_Absyn_MallocInfo),{(void*)((
struct _tuple4**)Cyc__genarr_630),(void*)((struct _tuple4**)Cyc__genarr_630),(void*)((
struct _tuple4**)Cyc__genarr_630 + 5)}};struct _tuple72{unsigned int f1;struct Cyc_Absyn_MallocInfo
f2;};static struct _tuple5 Cyc__gentuple_632={offsetof(struct _tuple72,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_633={offsetof(struct _tuple72,f2),(
void*)& Cyc_Absyn_malloc_info_t_rep};static struct _tuple5*Cyc__genarr_634[2]={& Cyc__gentuple_632,&
Cyc__gentuple_633};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_623={4,
sizeof(struct _tuple72),{(void*)((struct _tuple5**)Cyc__genarr_634),(void*)((
struct _tuple5**)Cyc__genarr_634),(void*)((struct _tuple5**)Cyc__genarr_634 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_608;struct _tuple73{unsigned int
f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};static struct _tuple5 Cyc__gentuple_619={
offsetof(struct _tuple73,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_620={
offsetof(struct _tuple73,f2),(void*)& Cyc__genrep_364};static struct _tuple5 Cyc__gentuple_621={
offsetof(struct _tuple73,f3),(void*)& Cyc__genrep_609};static struct _tuple5*Cyc__genarr_622[
3]={& Cyc__gentuple_619,& Cyc__gentuple_620,& Cyc__gentuple_621};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_608={4,sizeof(struct _tuple73),{(void*)((struct _tuple5**)Cyc__genarr_622),(
void*)((struct _tuple5**)Cyc__genarr_622),(void*)((struct _tuple5**)Cyc__genarr_622
+ 3)}};static struct _tuple6*Cyc__genarr_75[0]={};static unsigned char _tmp368[8]="Const_e";
static struct _tuple4 Cyc__gentuple_853={0,{_tmp368,_tmp368,_tmp368 + 8},(void*)& Cyc__genrep_823};
static unsigned char _tmp369[6]="Var_e";static struct _tuple4 Cyc__gentuple_854={1,{
_tmp369,_tmp369,_tmp369 + 6},(void*)& Cyc__genrep_810};static unsigned char _tmp36A[
12]="UnknownId_e";static struct _tuple4 Cyc__gentuple_855={2,{_tmp36A,_tmp36A,
_tmp36A + 12},(void*)& Cyc__genrep_275};static unsigned char _tmp36B[9]="Primop_e";
static struct _tuple4 Cyc__gentuple_856={3,{_tmp36B,_tmp36B,_tmp36B + 9},(void*)& Cyc__genrep_805};
static unsigned char _tmp36C[11]="AssignOp_e";static struct _tuple4 Cyc__gentuple_857={
4,{_tmp36C,_tmp36C,_tmp36C + 11},(void*)& Cyc__genrep_773};static unsigned char
_tmp36D[12]="Increment_e";static struct _tuple4 Cyc__gentuple_858={5,{_tmp36D,
_tmp36D,_tmp36D + 12},(void*)& Cyc__genrep_762};static unsigned char _tmp36E[14]="Conditional_e";
static struct _tuple4 Cyc__gentuple_859={6,{_tmp36E,_tmp36E,_tmp36E + 14},(void*)&
Cyc__genrep_756};static unsigned char _tmp36F[9]="SeqExp_e";static struct _tuple4 Cyc__gentuple_860={
7,{_tmp36F,_tmp36F,_tmp36F + 9},(void*)& Cyc__genrep_695};static unsigned char
_tmp370[14]="UnknownCall_e";static struct _tuple4 Cyc__gentuple_861={8,{_tmp370,
_tmp370,_tmp370 + 14},(void*)& Cyc__genrep_751};static unsigned char _tmp371[9]="FnCall_e";
static struct _tuple4 Cyc__gentuple_862={9,{_tmp371,_tmp371,_tmp371 + 9},(void*)& Cyc__genrep_738};
static unsigned char _tmp372[8]="Throw_e";static struct _tuple4 Cyc__gentuple_863={10,{
_tmp372,_tmp372,_tmp372 + 8},(void*)& Cyc__genrep_76};static unsigned char _tmp373[
16]="NoInstantiate_e";static struct _tuple4 Cyc__gentuple_864={11,{_tmp373,_tmp373,
_tmp373 + 16},(void*)& Cyc__genrep_76};static unsigned char _tmp374[14]="Instantiate_e";
static struct _tuple4 Cyc__gentuple_865={12,{_tmp374,_tmp374,_tmp374 + 14},(void*)&
Cyc__genrep_733};static unsigned char _tmp375[7]="Cast_e";static struct _tuple4 Cyc__gentuple_866={
13,{_tmp375,_tmp375,_tmp375 + 7},(void*)& Cyc__genrep_728};static unsigned char
_tmp376[10]="Address_e";static struct _tuple4 Cyc__gentuple_867={14,{_tmp376,
_tmp376,_tmp376 + 10},(void*)& Cyc__genrep_76};static unsigned char _tmp377[6]="New_e";
static struct _tuple4 Cyc__gentuple_868={15,{_tmp377,_tmp377,_tmp377 + 6},(void*)&
Cyc__genrep_723};static unsigned char _tmp378[12]="Sizeoftyp_e";static struct
_tuple4 Cyc__gentuple_869={16,{_tmp378,_tmp378,_tmp378 + 12},(void*)& Cyc__genrep_47};
static unsigned char _tmp379[12]="Sizeofexp_e";static struct _tuple4 Cyc__gentuple_870={
17,{_tmp379,_tmp379,_tmp379 + 12},(void*)& Cyc__genrep_76};static unsigned char
_tmp37A[11]="Offsetof_e";static struct _tuple4 Cyc__gentuple_871={18,{_tmp37A,
_tmp37A,_tmp37A + 11},(void*)& Cyc__genrep_710};static unsigned char _tmp37B[9]="Gentyp_e";
static struct _tuple4 Cyc__gentuple_872={19,{_tmp37B,_tmp37B,_tmp37B + 9},(void*)&
Cyc__genrep_705};static unsigned char _tmp37C[8]="Deref_e";static struct _tuple4 Cyc__gentuple_873={
20,{_tmp37C,_tmp37C,_tmp37C + 8},(void*)& Cyc__genrep_76};static unsigned char
_tmp37D[15]="StructMember_e";static struct _tuple4 Cyc__gentuple_874={21,{_tmp37D,
_tmp37D,_tmp37D + 15},(void*)& Cyc__genrep_700};static unsigned char _tmp37E[14]="StructArrow_e";
static struct _tuple4 Cyc__gentuple_875={22,{_tmp37E,_tmp37E,_tmp37E + 14},(void*)&
Cyc__genrep_700};static unsigned char _tmp37F[12]="Subscript_e";static struct
_tuple4 Cyc__gentuple_876={23,{_tmp37F,_tmp37F,_tmp37F + 12},(void*)& Cyc__genrep_695};
static unsigned char _tmp380[8]="Tuple_e";static struct _tuple4 Cyc__gentuple_877={24,{
_tmp380,_tmp380,_tmp380 + 8},(void*)& Cyc__genrep_691};static unsigned char _tmp381[
14]="CompoundLit_e";static struct _tuple4 Cyc__gentuple_878={25,{_tmp381,_tmp381,
_tmp381 + 14},(void*)& Cyc__genrep_680};static unsigned char _tmp382[8]="Array_e";
static struct _tuple4 Cyc__gentuple_879={26,{_tmp382,_tmp382,_tmp382 + 8},(void*)&
Cyc__genrep_676};static unsigned char _tmp383[16]="Comprehension_e";static struct
_tuple4 Cyc__gentuple_880={27,{_tmp383,_tmp383,_tmp383 + 16},(void*)& Cyc__genrep_670};
static unsigned char _tmp384[9]="Struct_e";static struct _tuple4 Cyc__gentuple_881={
28,{_tmp384,_tmp384,_tmp384 + 9},(void*)& Cyc__genrep_662};static unsigned char
_tmp385[13]="AnonStruct_e";static struct _tuple4 Cyc__gentuple_882={29,{_tmp385,
_tmp385,_tmp385 + 13},(void*)& Cyc__genrep_657};static unsigned char _tmp386[9]="Tunion_e";
static struct _tuple4 Cyc__gentuple_883={30,{_tmp386,_tmp386,_tmp386 + 9},(void*)&
Cyc__genrep_649};static unsigned char _tmp387[7]="Enum_e";static struct _tuple4 Cyc__gentuple_884={
31,{_tmp387,_tmp387,_tmp387 + 7},(void*)& Cyc__genrep_642};static unsigned char
_tmp388[11]="AnonEnum_e";static struct _tuple4 Cyc__gentuple_885={32,{_tmp388,
_tmp388,_tmp388 + 11},(void*)& Cyc__genrep_635};static unsigned char _tmp389[9]="Malloc_e";
static struct _tuple4 Cyc__gentuple_886={33,{_tmp389,_tmp389,_tmp389 + 9},(void*)&
Cyc__genrep_623};static unsigned char _tmp38A[16]="UnresolvedMem_e";static struct
_tuple4 Cyc__gentuple_887={34,{_tmp38A,_tmp38A,_tmp38A + 16},(void*)& Cyc__genrep_608};
static unsigned char _tmp38B[10]="StmtExp_e";static struct _tuple4 Cyc__gentuple_888={
35,{_tmp38B,_tmp38B,_tmp38B + 10},(void*)& Cyc__genrep_451};static unsigned char
_tmp38C[10]="Codegen_e";static struct _tuple4 Cyc__gentuple_889={36,{_tmp38C,
_tmp38C,_tmp38C + 10},(void*)& Cyc__genrep_81};static unsigned char _tmp38D[7]="Fill_e";
static struct _tuple4 Cyc__gentuple_890={37,{_tmp38D,_tmp38D,_tmp38D + 7},(void*)&
Cyc__genrep_76};static struct _tuple4*Cyc__genarr_891[38]={& Cyc__gentuple_853,& Cyc__gentuple_854,&
Cyc__gentuple_855,& Cyc__gentuple_856,& Cyc__gentuple_857,& Cyc__gentuple_858,& Cyc__gentuple_859,&
Cyc__gentuple_860,& Cyc__gentuple_861,& Cyc__gentuple_862,& Cyc__gentuple_863,& Cyc__gentuple_864,&
Cyc__gentuple_865,& Cyc__gentuple_866,& Cyc__gentuple_867,& Cyc__gentuple_868,& Cyc__gentuple_869,&
Cyc__gentuple_870,& Cyc__gentuple_871,& Cyc__gentuple_872,& Cyc__gentuple_873,& Cyc__gentuple_874,&
Cyc__gentuple_875,& Cyc__gentuple_876,& Cyc__gentuple_877,& Cyc__gentuple_878,& Cyc__gentuple_879,&
Cyc__gentuple_880,& Cyc__gentuple_881,& Cyc__gentuple_882,& Cyc__gentuple_883,& Cyc__gentuple_884,&
Cyc__gentuple_885,& Cyc__gentuple_886,& Cyc__gentuple_887,& Cyc__gentuple_888,& Cyc__gentuple_889,&
Cyc__gentuple_890};static unsigned char _tmp38F[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_exp_t_rep={5,{_tmp38F,_tmp38F,_tmp38F + 8},{(void*)((struct _tuple6**)
Cyc__genarr_75),(void*)((struct _tuple6**)Cyc__genarr_75),(void*)((struct _tuple6**)
Cyc__genarr_75 + 0)},{(void*)((struct _tuple4**)Cyc__genarr_891),(void*)((struct
_tuple4**)Cyc__genarr_891),(void*)((struct _tuple4**)Cyc__genarr_891 + 38)}};
static unsigned char _tmp390[4]="Exp";static struct _tagged_arr Cyc__genname_897={
_tmp390,_tmp390,_tmp390 + 4};static unsigned char _tmp391[5]="topt";static struct
_tuple4 Cyc__gentuple_892={offsetof(struct Cyc_Absyn_Exp,topt),{_tmp391,_tmp391,
_tmp391 + 5},(void*)& Cyc__genrep_61};static unsigned char _tmp392[2]="r";static
struct _tuple4 Cyc__gentuple_893={offsetof(struct Cyc_Absyn_Exp,r),{_tmp392,_tmp392,
_tmp392 + 2},(void*)& Cyc_Absyn_raw_exp_t_rep};static unsigned char _tmp393[4]="loc";
static struct _tuple4 Cyc__gentuple_894={offsetof(struct Cyc_Absyn_Exp,loc),{_tmp393,
_tmp393,_tmp393 + 4},(void*)& Cyc__genrep_2};static unsigned char _tmp394[6]="annot";
static struct _tuple4 Cyc__gentuple_895={offsetof(struct Cyc_Absyn_Exp,annot),{
_tmp394,_tmp394,_tmp394 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static struct
_tuple4*Cyc__genarr_896[4]={& Cyc__gentuple_892,& Cyc__gentuple_893,& Cyc__gentuple_894,&
Cyc__gentuple_895};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(
struct _tagged_arr*)& Cyc__genname_897,sizeof(struct Cyc_Absyn_Exp),{(void*)((
struct _tuple4**)Cyc__genarr_896),(void*)((struct _tuple4**)Cyc__genarr_896),(void*)((
struct _tuple4**)Cyc__genarr_896 + 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_77={
1,1,(void*)((void*)& Cyc_struct_Absyn_Exp_rep)};static struct _tuple5 Cyc__gentuple_78={
offsetof(struct _tuple28,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_79={
offsetof(struct _tuple28,f2),(void*)& Cyc__genrep_77};static struct _tuple5*Cyc__genarr_80[
2]={& Cyc__gentuple_78,& Cyc__gentuple_79};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_76={
4,sizeof(struct _tuple28),{(void*)((struct _tuple5**)Cyc__genarr_80),(void*)((
struct _tuple5**)Cyc__genarr_80),(void*)((struct _tuple5**)Cyc__genarr_80 + 2)}};
static unsigned char _tmp398[10]="Unknown_b";static struct _tuple6 Cyc__gentuple_991={
0,{_tmp398,_tmp398,_tmp398 + 10}};static struct _tuple6*Cyc__genarr_992[1]={& Cyc__gentuple_991};
static unsigned char _tmp399[8]="Upper_b";static struct _tuple4 Cyc__gentuple_993={0,{
_tmp399,_tmp399,_tmp399 + 8},(void*)& Cyc__genrep_76};static struct _tuple4*Cyc__genarr_994[
1]={& Cyc__gentuple_993};static unsigned char _tmp39B[7]="Bounds";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_bounds_t_rep={5,{_tmp39B,_tmp39B,_tmp39B + 7},{(void*)((struct _tuple6**)
Cyc__genarr_992),(void*)((struct _tuple6**)Cyc__genarr_992),(void*)((struct
_tuple6**)Cyc__genarr_992 + 1)},{(void*)((struct _tuple4**)Cyc__genarr_994),(void*)((
struct _tuple4**)Cyc__genarr_994),(void*)((struct _tuple4**)Cyc__genarr_994 + 1)}};
static struct _tuple5 Cyc__gentuple_995={offsetof(struct _tuple5,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_996={offsetof(struct _tuple5,f2),(void*)& Cyc_Absyn_bounds_t_rep};
static struct _tuple5*Cyc__genarr_997[2]={& Cyc__gentuple_995,& Cyc__gentuple_996};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_990={4,sizeof(struct _tuple5),{(
void*)((struct _tuple5**)Cyc__genarr_997),(void*)((struct _tuple5**)Cyc__genarr_997),(
void*)((struct _tuple5**)Cyc__genarr_997 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_986;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_983;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;
static unsigned char _tmp39D[7]="Conref";static struct _tagged_arr Cyc__genname_1003={
_tmp39D,_tmp39D,_tmp39D + 7};static unsigned char _tmp39E[2]="v";static struct
_tuple4 Cyc__gentuple_1001={offsetof(struct Cyc_Absyn_Conref,v),{_tmp39E,_tmp39E,
_tmp39E + 2},(void*)& Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep};static
struct _tuple4*Cyc__genarr_1002[1]={& Cyc__gentuple_1001};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_1003,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple4**)Cyc__genarr_1002),(void*)((
struct _tuple4**)Cyc__genarr_1002),(void*)((struct _tuple4**)Cyc__genarr_1002 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_983={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};
struct _tuple74{unsigned int f1;struct Cyc_Absyn_Conref*f2;};static struct _tuple5 Cyc__gentuple_987={
offsetof(struct _tuple74,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_988={
offsetof(struct _tuple74,f2),(void*)& Cyc__genrep_983};static struct _tuple5*Cyc__genarr_989[
2]={& Cyc__gentuple_987,& Cyc__gentuple_988};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_986={4,sizeof(struct _tuple74),{(void*)((struct _tuple5**)Cyc__genarr_989),(
void*)((struct _tuple5**)Cyc__genarr_989),(void*)((struct _tuple5**)Cyc__genarr_989
+ 2)}};static unsigned char _tmp3A2[10]="No_constr";static struct _tuple6 Cyc__gentuple_984={
0,{_tmp3A2,_tmp3A2,_tmp3A2 + 10}};static struct _tuple6*Cyc__genarr_985[1]={& Cyc__gentuple_984};
static unsigned char _tmp3A3[10]="Eq_constr";static struct _tuple4 Cyc__gentuple_998={
0,{_tmp3A3,_tmp3A3,_tmp3A3 + 10},(void*)& Cyc__genrep_990};static unsigned char
_tmp3A4[15]="Forward_constr";static struct _tuple4 Cyc__gentuple_999={1,{_tmp3A4,
_tmp3A4,_tmp3A4 + 15},(void*)& Cyc__genrep_986};static struct _tuple4*Cyc__genarr_1000[
2]={& Cyc__gentuple_998,& Cyc__gentuple_999};static unsigned char _tmp3A6[11]="Constraint";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep={
5,{_tmp3A6,_tmp3A6,_tmp3A6 + 11},{(void*)((struct _tuple6**)Cyc__genarr_985),(void*)((
struct _tuple6**)Cyc__genarr_985),(void*)((struct _tuple6**)Cyc__genarr_985 + 1)},{(
void*)((struct _tuple4**)Cyc__genarr_1000),(void*)((struct _tuple4**)Cyc__genarr_1000),(
void*)((struct _tuple4**)Cyc__genarr_1000 + 2)}};static unsigned char _tmp3A7[7]="Conref";
static struct _tagged_arr Cyc__genname_1007={_tmp3A7,_tmp3A7,_tmp3A7 + 7};static
unsigned char _tmp3A8[2]="v";static struct _tuple4 Cyc__gentuple_1005={offsetof(
struct Cyc_Absyn_Conref,v),{_tmp3A8,_tmp3A8,_tmp3A8 + 2},(void*)& Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep};
static struct _tuple4*Cyc__genarr_1006[1]={& Cyc__gentuple_1005};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep={3,(struct _tagged_arr*)& Cyc__genname_1007,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple4**)Cyc__genarr_1006),(void*)((
struct _tuple4**)Cyc__genarr_1006),(void*)((struct _tuple4**)Cyc__genarr_1006 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1004={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0bool2_rep)};
static unsigned char _tmp3AB[8]="PtrInfo";static struct _tagged_arr Cyc__genname_1014={
_tmp3AB,_tmp3AB,_tmp3AB + 8};static unsigned char _tmp3AC[8]="elt_typ";static struct
_tuple4 Cyc__gentuple_1008={offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),{_tmp3AC,
_tmp3AC,_tmp3AC + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static unsigned char
_tmp3AD[8]="rgn_typ";static struct _tuple4 Cyc__gentuple_1009={offsetof(struct Cyc_Absyn_PtrInfo,rgn_typ),{
_tmp3AD,_tmp3AD,_tmp3AD + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static
unsigned char _tmp3AE[9]="nullable";static struct _tuple4 Cyc__gentuple_1010={
offsetof(struct Cyc_Absyn_PtrInfo,nullable),{_tmp3AE,_tmp3AE,_tmp3AE + 9},(void*)&
Cyc__genrep_1004};static unsigned char _tmp3AF[3]="tq";static struct _tuple4 Cyc__gentuple_1011={
offsetof(struct Cyc_Absyn_PtrInfo,tq),{_tmp3AF,_tmp3AF,_tmp3AF + 3},(void*)& Cyc__genrep_132};
static unsigned char _tmp3B0[7]="bounds";static struct _tuple4 Cyc__gentuple_1012={
offsetof(struct Cyc_Absyn_PtrInfo,bounds),{_tmp3B0,_tmp3B0,_tmp3B0 + 7},(void*)&
Cyc__genrep_983};static struct _tuple4*Cyc__genarr_1013[5]={& Cyc__gentuple_1008,&
Cyc__gentuple_1009,& Cyc__gentuple_1010,& Cyc__gentuple_1011,& Cyc__gentuple_1012};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_1014,sizeof(struct Cyc_Absyn_PtrInfo),{(void*)((struct _tuple4**)Cyc__genarr_1013),(
void*)((struct _tuple4**)Cyc__genarr_1013),(void*)((struct _tuple4**)Cyc__genarr_1013
+ 5)}};struct _tuple75{unsigned int f1;struct Cyc_Absyn_PtrInfo f2;};static struct
_tuple5 Cyc__gentuple_1015={offsetof(struct _tuple75,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_1016={offsetof(struct _tuple75,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};
static struct _tuple5*Cyc__genarr_1017[2]={& Cyc__gentuple_1015,& Cyc__gentuple_1016};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_982={4,sizeof(struct _tuple75),{(
void*)((struct _tuple5**)Cyc__genarr_1017),(void*)((struct _tuple5**)Cyc__genarr_1017),(
void*)((struct _tuple5**)Cyc__genarr_1017 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_971;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep;
static unsigned char _tmp3B3[3]="B1";static struct _tuple6 Cyc__gentuple_972={0,{
_tmp3B3,_tmp3B3,_tmp3B3 + 3}};static unsigned char _tmp3B4[3]="B2";static struct
_tuple6 Cyc__gentuple_973={1,{_tmp3B4,_tmp3B4,_tmp3B4 + 3}};static unsigned char
_tmp3B5[3]="B4";static struct _tuple6 Cyc__gentuple_974={2,{_tmp3B5,_tmp3B5,_tmp3B5
+ 3}};static unsigned char _tmp3B6[3]="B8";static struct _tuple6 Cyc__gentuple_975={3,{
_tmp3B6,_tmp3B6,_tmp3B6 + 3}};static struct _tuple6*Cyc__genarr_976[4]={& Cyc__gentuple_972,&
Cyc__gentuple_973,& Cyc__gentuple_974,& Cyc__gentuple_975};static struct _tuple4*Cyc__genarr_977[
0]={};static unsigned char _tmp3B8[8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={
5,{_tmp3B8,_tmp3B8,_tmp3B8 + 8},{(void*)((struct _tuple6**)Cyc__genarr_976),(void*)((
struct _tuple6**)Cyc__genarr_976),(void*)((struct _tuple6**)Cyc__genarr_976 + 4)},{(
void*)((struct _tuple4**)Cyc__genarr_977),(void*)((struct _tuple4**)Cyc__genarr_977),(
void*)((struct _tuple4**)Cyc__genarr_977 + 0)}};static struct _tuple5 Cyc__gentuple_978={
offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_979={
offsetof(struct _tuple63,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple5 Cyc__gentuple_980={
offsetof(struct _tuple63,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple5*
Cyc__genarr_981[3]={& Cyc__gentuple_978,& Cyc__gentuple_979,& Cyc__gentuple_980};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_971={4,sizeof(struct _tuple63),{(
void*)((struct _tuple5**)Cyc__genarr_981),(void*)((struct _tuple5**)Cyc__genarr_981),(
void*)((struct _tuple5**)Cyc__genarr_981 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_965;struct _tuple76{unsigned int f1;void*f2;struct Cyc_Absyn_Tqual f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple5 Cyc__gentuple_966={offsetof(struct
_tuple76,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_967={
offsetof(struct _tuple76,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5 Cyc__gentuple_968={offsetof(struct _tuple76,f3),(void*)& Cyc__genrep_132};
static struct _tuple5 Cyc__gentuple_969={offsetof(struct _tuple76,f4),(void*)& Cyc__genrep_73};
static struct _tuple5*Cyc__genarr_970[4]={& Cyc__gentuple_966,& Cyc__gentuple_967,&
Cyc__gentuple_968,& Cyc__gentuple_969};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_965={
4,sizeof(struct _tuple76),{(void*)((struct _tuple5**)Cyc__genarr_970),(void*)((
struct _tuple5**)Cyc__genarr_970),(void*)((struct _tuple5**)Cyc__genarr_970 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_946;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_fn_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_947;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static unsigned char _tmp3BB[5]="List";static struct _tagged_arr Cyc__genname_951={
_tmp3BB,_tmp3BB,_tmp3BB + 5};static unsigned char _tmp3BC[3]="hd";static struct
_tuple4 Cyc__gentuple_948={offsetof(struct Cyc_List_List,hd),{_tmp3BC,_tmp3BC,
_tmp3BC + 3},(void*)& Cyc__genrep_681};static unsigned char _tmp3BD[3]="tl";static
struct _tuple4 Cyc__gentuple_949={offsetof(struct Cyc_List_List,tl),{_tmp3BD,
_tmp3BD,_tmp3BD + 3},(void*)& Cyc__genrep_947};static struct _tuple4*Cyc__genarr_950[
2]={& Cyc__gentuple_948,& Cyc__gentuple_949};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_951,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_950),(void*)((struct _tuple4**)Cyc__genarr_950),(void*)((
struct _tuple4**)Cyc__genarr_950 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_947={
1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static unsigned char _tmp3C0[7]="FnInfo";static struct _tagged_arr Cyc__genname_961={
_tmp3C0,_tmp3C0,_tmp3C0 + 7};static unsigned char _tmp3C1[6]="tvars";static struct
_tuple4 Cyc__gentuple_952={offsetof(struct Cyc_Absyn_FnInfo,tvars),{_tmp3C1,
_tmp3C1,_tmp3C1 + 6},(void*)& Cyc__genrep_254};static unsigned char _tmp3C2[7]="effect";
static struct _tuple4 Cyc__gentuple_953={offsetof(struct Cyc_Absyn_FnInfo,effect),{
_tmp3C2,_tmp3C2,_tmp3C2 + 7},(void*)& Cyc__genrep_61};static unsigned char _tmp3C3[8]="ret_typ";
static struct _tuple4 Cyc__gentuple_954={offsetof(struct Cyc_Absyn_FnInfo,ret_typ),{
_tmp3C3,_tmp3C3,_tmp3C3 + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static
unsigned char _tmp3C4[5]="args";static struct _tuple4 Cyc__gentuple_955={offsetof(
struct Cyc_Absyn_FnInfo,args),{_tmp3C4,_tmp3C4,_tmp3C4 + 5},(void*)& Cyc__genrep_947};
static unsigned char _tmp3C5[10]="c_varargs";static struct _tuple4 Cyc__gentuple_956={
offsetof(struct Cyc_Absyn_FnInfo,c_varargs),{_tmp3C5,_tmp3C5,_tmp3C5 + 10},(void*)((
void*)& Cyc__genrep_102)};static unsigned char _tmp3C6[12]="cyc_varargs";static
struct _tuple4 Cyc__gentuple_957={offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),{
_tmp3C6,_tmp3C6,_tmp3C6 + 12},(void*)& Cyc__genrep_567};static unsigned char _tmp3C7[
7]="rgn_po";static struct _tuple4 Cyc__gentuple_958={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),{
_tmp3C7,_tmp3C7,_tmp3C7 + 7},(void*)& Cyc__genrep_557};static unsigned char _tmp3C8[
11]="attributes";static struct _tuple4 Cyc__gentuple_959={offsetof(struct Cyc_Absyn_FnInfo,attributes),{
_tmp3C8,_tmp3C8,_tmp3C8 + 11},(void*)& Cyc__genrep_83};static struct _tuple4*Cyc__genarr_960[
8]={& Cyc__gentuple_952,& Cyc__gentuple_953,& Cyc__gentuple_954,& Cyc__gentuple_955,&
Cyc__gentuple_956,& Cyc__gentuple_957,& Cyc__gentuple_958,& Cyc__gentuple_959};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_961,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((struct _tuple4**)Cyc__genarr_960),(
void*)((struct _tuple4**)Cyc__genarr_960),(void*)((struct _tuple4**)Cyc__genarr_960
+ 8)}};struct _tuple77{unsigned int f1;struct Cyc_Absyn_FnInfo f2;};static struct
_tuple5 Cyc__gentuple_962={offsetof(struct _tuple77,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_963={offsetof(struct _tuple77,f2),(void*)& Cyc_Absyn_fn_info_t_rep};
static struct _tuple5*Cyc__genarr_964[2]={& Cyc__gentuple_962,& Cyc__gentuple_963};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_946={4,sizeof(struct _tuple77),{(
void*)((struct _tuple5**)Cyc__genarr_964),(void*)((struct _tuple5**)Cyc__genarr_964),(
void*)((struct _tuple5**)Cyc__genarr_964 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_942;static struct _tuple5 Cyc__gentuple_943={offsetof(struct _tuple10,f1),(
void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_944={offsetof(struct
_tuple10,f2),(void*)& Cyc__genrep_301};static struct _tuple5*Cyc__genarr_945[2]={&
Cyc__gentuple_943,& Cyc__gentuple_944};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_942={
4,sizeof(struct _tuple10),{(void*)((struct _tuple5**)Cyc__genarr_945),(void*)((
struct _tuple5**)Cyc__genarr_945),(void*)((struct _tuple5**)Cyc__genarr_945 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_935;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_929;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_929={1,1,(void*)((
void*)& Cyc__genrep_11)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_936;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_936={1,1,(void*)((void*)& Cyc__genrep_346)};
struct _tuple78{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl**
f4;};static struct _tuple5 Cyc__gentuple_937={offsetof(struct _tuple78,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_938={offsetof(struct _tuple78,f2),(
void*)& Cyc__genrep_929};static struct _tuple5 Cyc__gentuple_939={offsetof(struct
_tuple78,f3),(void*)& Cyc__genrep_52};static struct _tuple5 Cyc__gentuple_940={
offsetof(struct _tuple78,f4),(void*)& Cyc__genrep_936};static struct _tuple5*Cyc__genarr_941[
4]={& Cyc__gentuple_937,& Cyc__gentuple_938,& Cyc__gentuple_939,& Cyc__gentuple_940};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_935={4,sizeof(struct _tuple78),{(
void*)((struct _tuple5**)Cyc__genarr_941),(void*)((struct _tuple5**)Cyc__genarr_941),(
void*)((struct _tuple5**)Cyc__genarr_941 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_919;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_920;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_921;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Uniondecl_rep;static unsigned char _tmp3CF[10]="Uniondecl";static
struct _tagged_arr Cyc__genname_928={_tmp3CF,_tmp3CF,_tmp3CF + 10};static
unsigned char _tmp3D0[3]="sc";static struct _tuple4 Cyc__gentuple_922={offsetof(
struct Cyc_Absyn_Uniondecl,sc),{_tmp3D0,_tmp3D0,_tmp3D0 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static unsigned char _tmp3D1[5]="name";static struct _tuple4 Cyc__gentuple_923={
offsetof(struct Cyc_Absyn_Uniondecl,name),{_tmp3D1,_tmp3D1,_tmp3D1 + 5},(void*)&
Cyc__genrep_364};static unsigned char _tmp3D2[4]="tvs";static struct _tuple4 Cyc__gentuple_924={
offsetof(struct Cyc_Absyn_Uniondecl,tvs),{_tmp3D2,_tmp3D2,_tmp3D2 + 4},(void*)& Cyc__genrep_254};
static unsigned char _tmp3D3[7]="fields";static struct _tuple4 Cyc__gentuple_925={
offsetof(struct Cyc_Absyn_Uniondecl,fields),{_tmp3D3,_tmp3D3,_tmp3D3 + 7},(void*)&
Cyc__genrep_347};static unsigned char _tmp3D4[11]="attributes";static struct _tuple4
Cyc__gentuple_926={offsetof(struct Cyc_Absyn_Uniondecl,attributes),{_tmp3D4,
_tmp3D4,_tmp3D4 + 11},(void*)& Cyc__genrep_83};static struct _tuple4*Cyc__genarr_927[
5]={& Cyc__gentuple_922,& Cyc__gentuple_923,& Cyc__gentuple_924,& Cyc__gentuple_925,&
Cyc__gentuple_926};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Uniondecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_928,sizeof(struct Cyc_Absyn_Uniondecl),{(void*)((
struct _tuple4**)Cyc__genarr_927),(void*)((struct _tuple4**)Cyc__genarr_927),(void*)((
struct _tuple4**)Cyc__genarr_927 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_921={
1,1,(void*)((void*)& Cyc_struct_Absyn_Uniondecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_920={1,1,(void*)((void*)& Cyc__genrep_921)};struct _tuple79{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Uniondecl**
f4;};static struct _tuple5 Cyc__gentuple_930={offsetof(struct _tuple79,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_931={offsetof(struct _tuple79,f2),(
void*)& Cyc__genrep_929};static struct _tuple5 Cyc__gentuple_932={offsetof(struct
_tuple79,f3),(void*)& Cyc__genrep_52};static struct _tuple5 Cyc__gentuple_933={
offsetof(struct _tuple79,f4),(void*)& Cyc__genrep_920};static struct _tuple5*Cyc__genarr_934[
4]={& Cyc__gentuple_930,& Cyc__gentuple_931,& Cyc__gentuple_932,& Cyc__gentuple_933};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_919={4,sizeof(struct _tuple79),{(
void*)((struct _tuple5**)Cyc__genarr_934),(void*)((struct _tuple5**)Cyc__genarr_934),(
void*)((struct _tuple5**)Cyc__genarr_934 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_914;struct _tuple80{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*
f3;};static struct _tuple5 Cyc__gentuple_915={offsetof(struct _tuple80,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_916={offsetof(struct _tuple80,f2),(
void*)& Cyc__genrep_10};static struct _tuple5 Cyc__gentuple_917={offsetof(struct
_tuple80,f3),(void*)& Cyc__genrep_643};static struct _tuple5*Cyc__genarr_918[3]={&
Cyc__gentuple_915,& Cyc__gentuple_916,& Cyc__gentuple_917};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_914={4,sizeof(struct _tuple80),{(void*)((struct _tuple5**)Cyc__genarr_918),(
void*)((struct _tuple5**)Cyc__genarr_918),(void*)((struct _tuple5**)Cyc__genarr_918
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_910;static struct _tuple5
Cyc__gentuple_911={offsetof(struct _tuple10,f1),(void*)& Cyc__genrep_5};static
struct _tuple5 Cyc__gentuple_912={offsetof(struct _tuple10,f2),(void*)& Cyc__genrep_348};
static struct _tuple5*Cyc__genarr_913[2]={& Cyc__gentuple_911,& Cyc__gentuple_912};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_910={4,sizeof(struct _tuple10),{(
void*)((struct _tuple5**)Cyc__genarr_913),(void*)((struct _tuple5**)Cyc__genarr_913),(
void*)((struct _tuple5**)Cyc__genarr_913 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_70;static struct _tuple5 Cyc__gentuple_907={offsetof(struct _tuple10,f1),(
void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_908={offsetof(struct
_tuple10,f2),(void*)& Cyc__genrep_71};static struct _tuple5*Cyc__genarr_909[2]={&
Cyc__gentuple_907,& Cyc__gentuple_908};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_70={
4,sizeof(struct _tuple10),{(void*)((struct _tuple5**)Cyc__genarr_909),(void*)((
struct _tuple5**)Cyc__genarr_909),(void*)((struct _tuple5**)Cyc__genarr_909 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_60;struct _tuple81{unsigned int f1;
struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_Core_Opt*f4;};static struct
_tuple5 Cyc__gentuple_65={offsetof(struct _tuple81,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_66={offsetof(struct _tuple81,f2),(void*)& Cyc__genrep_10};
static struct _tuple5 Cyc__gentuple_67={offsetof(struct _tuple81,f3),(void*)& Cyc__genrep_52};
static struct _tuple5 Cyc__gentuple_68={offsetof(struct _tuple81,f4),(void*)& Cyc__genrep_61};
static struct _tuple5*Cyc__genarr_69[4]={& Cyc__gentuple_65,& Cyc__gentuple_66,& Cyc__gentuple_67,&
Cyc__gentuple_68};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_60={4,sizeof(
struct _tuple81),{(void*)((struct _tuple5**)Cyc__genarr_69),(void*)((struct _tuple5**)
Cyc__genarr_69),(void*)((struct _tuple5**)Cyc__genarr_69 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_51;static struct _tuple5 Cyc__gentuple_57={offsetof(struct _tuple10,f1),(
void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_58={offsetof(struct
_tuple10,f2),(void*)& Cyc__genrep_52};static struct _tuple5*Cyc__genarr_59[2]={& Cyc__gentuple_57,&
Cyc__gentuple_58};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_51={4,sizeof(
struct _tuple10),{(void*)((struct _tuple5**)Cyc__genarr_59),(void*)((struct _tuple5**)
Cyc__genarr_59),(void*)((struct _tuple5**)Cyc__genarr_59 + 2)}};static
unsigned char _tmp3DE[9]="VoidType";static struct _tuple6 Cyc__gentuple_43={0,{
_tmp3DE,_tmp3DE,_tmp3DE + 9}};static unsigned char _tmp3DF[10]="FloatType";static
struct _tuple6 Cyc__gentuple_44={1,{_tmp3DF,_tmp3DF,_tmp3DF + 10}};static
unsigned char _tmp3E0[8]="HeapRgn";static struct _tuple6 Cyc__gentuple_45={2,{
_tmp3E0,_tmp3E0,_tmp3E0 + 8}};static struct _tuple6*Cyc__genarr_46[3]={& Cyc__gentuple_43,&
Cyc__gentuple_44,& Cyc__gentuple_45};static unsigned char _tmp3E1[5]="Evar";static
struct _tuple4 Cyc__gentuple_1089={0,{_tmp3E1,_tmp3E1,_tmp3E1 + 5},(void*)& Cyc__genrep_1074};
static unsigned char _tmp3E2[8]="VarType";static struct _tuple4 Cyc__gentuple_1090={1,{
_tmp3E2,_tmp3E2,_tmp3E2 + 8},(void*)& Cyc__genrep_1070};static unsigned char _tmp3E3[
11]="TunionType";static struct _tuple4 Cyc__gentuple_1091={2,{_tmp3E3,_tmp3E3,
_tmp3E3 + 11},(void*)& Cyc__genrep_1044};static unsigned char _tmp3E4[16]="TunionFieldType";
static struct _tuple4 Cyc__gentuple_1092={3,{_tmp3E4,_tmp3E4,_tmp3E4 + 16},(void*)&
Cyc__genrep_1018};static unsigned char _tmp3E5[12]="PointerType";static struct
_tuple4 Cyc__gentuple_1093={4,{_tmp3E5,_tmp3E5,_tmp3E5 + 12},(void*)& Cyc__genrep_982};
static unsigned char _tmp3E6[8]="IntType";static struct _tuple4 Cyc__gentuple_1094={5,{
_tmp3E6,_tmp3E6,_tmp3E6 + 8},(void*)& Cyc__genrep_971};static unsigned char _tmp3E7[
11]="DoubleType";static struct _tuple4 Cyc__gentuple_1095={6,{_tmp3E7,_tmp3E7,
_tmp3E7 + 11},(void*)& Cyc__genrep_116};static unsigned char _tmp3E8[10]="ArrayType";
static struct _tuple4 Cyc__gentuple_1096={7,{_tmp3E8,_tmp3E8,_tmp3E8 + 10},(void*)&
Cyc__genrep_965};static unsigned char _tmp3E9[7]="FnType";static struct _tuple4 Cyc__gentuple_1097={
8,{_tmp3E9,_tmp3E9,_tmp3E9 + 7},(void*)& Cyc__genrep_946};static unsigned char
_tmp3EA[10]="TupleType";static struct _tuple4 Cyc__gentuple_1098={9,{_tmp3EA,
_tmp3EA,_tmp3EA + 10},(void*)& Cyc__genrep_942};static unsigned char _tmp3EB[11]="StructType";
static struct _tuple4 Cyc__gentuple_1099={10,{_tmp3EB,_tmp3EB,_tmp3EB + 11},(void*)&
Cyc__genrep_935};static unsigned char _tmp3EC[10]="UnionType";static struct _tuple4
Cyc__gentuple_1100={11,{_tmp3EC,_tmp3EC,_tmp3EC + 10},(void*)& Cyc__genrep_919};
static unsigned char _tmp3ED[9]="EnumType";static struct _tuple4 Cyc__gentuple_1101={
12,{_tmp3ED,_tmp3ED,_tmp3ED + 9},(void*)& Cyc__genrep_914};static unsigned char
_tmp3EE[11]="SizeofType";static struct _tuple4 Cyc__gentuple_1102={13,{_tmp3EE,
_tmp3EE,_tmp3EE + 11},(void*)& Cyc__genrep_47};static unsigned char _tmp3EF[15]="AnonStructType";
static struct _tuple4 Cyc__gentuple_1103={14,{_tmp3EF,_tmp3EF,_tmp3EF + 15},(void*)&
Cyc__genrep_910};static unsigned char _tmp3F0[14]="AnonUnionType";static struct
_tuple4 Cyc__gentuple_1104={15,{_tmp3F0,_tmp3F0,_tmp3F0 + 14},(void*)& Cyc__genrep_910};
static unsigned char _tmp3F1[13]="AnonEnumType";static struct _tuple4 Cyc__gentuple_1105={
16,{_tmp3F1,_tmp3F1,_tmp3F1 + 13},(void*)& Cyc__genrep_70};static unsigned char
_tmp3F2[14]="RgnHandleType";static struct _tuple4 Cyc__gentuple_1106={17,{_tmp3F2,
_tmp3F2,_tmp3F2 + 14},(void*)& Cyc__genrep_47};static unsigned char _tmp3F3[12]="TypedefType";
static struct _tuple4 Cyc__gentuple_1107={18,{_tmp3F3,_tmp3F3,_tmp3F3 + 12},(void*)&
Cyc__genrep_60};static unsigned char _tmp3F4[10]="AccessEff";static struct _tuple4
Cyc__gentuple_1108={19,{_tmp3F4,_tmp3F4,_tmp3F4 + 10},(void*)& Cyc__genrep_47};
static unsigned char _tmp3F5[8]="JoinEff";static struct _tuple4 Cyc__gentuple_1109={
20,{_tmp3F5,_tmp3F5,_tmp3F5 + 8},(void*)& Cyc__genrep_51};static unsigned char
_tmp3F6[8]="RgnsEff";static struct _tuple4 Cyc__gentuple_1110={21,{_tmp3F6,_tmp3F6,
_tmp3F6 + 8},(void*)& Cyc__genrep_47};static struct _tuple4*Cyc__genarr_1111[22]={&
Cyc__gentuple_1089,& Cyc__gentuple_1090,& Cyc__gentuple_1091,& Cyc__gentuple_1092,&
Cyc__gentuple_1093,& Cyc__gentuple_1094,& Cyc__gentuple_1095,& Cyc__gentuple_1096,&
Cyc__gentuple_1097,& Cyc__gentuple_1098,& Cyc__gentuple_1099,& Cyc__gentuple_1100,&
Cyc__gentuple_1101,& Cyc__gentuple_1102,& Cyc__gentuple_1103,& Cyc__gentuple_1104,&
Cyc__gentuple_1105,& Cyc__gentuple_1106,& Cyc__gentuple_1107,& Cyc__gentuple_1108,&
Cyc__gentuple_1109,& Cyc__gentuple_1110};static unsigned char _tmp3F8[5]="Type";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep={5,{_tmp3F8,_tmp3F8,_tmp3F8 + 
5},{(void*)((struct _tuple6**)Cyc__genarr_46),(void*)((struct _tuple6**)Cyc__genarr_46),(
void*)((struct _tuple6**)Cyc__genarr_46 + 3)},{(void*)((struct _tuple4**)Cyc__genarr_1111),(
void*)((struct _tuple4**)Cyc__genarr_1111),(void*)((struct _tuple4**)Cyc__genarr_1111
+ 22)}};static unsigned char _tmp3F9[8]="Vardecl";static struct _tagged_arr Cyc__genname_151={
_tmp3F9,_tmp3F9,_tmp3F9 + 8};static unsigned char _tmp3FA[3]="sc";static struct
_tuple4 Cyc__gentuple_142={offsetof(struct Cyc_Absyn_Vardecl,sc),{_tmp3FA,_tmp3FA,
_tmp3FA + 3},(void*)& Cyc_Absyn_scope_t_rep};static unsigned char _tmp3FB[5]="name";
static struct _tuple4 Cyc__gentuple_143={offsetof(struct Cyc_Absyn_Vardecl,name),{
_tmp3FB,_tmp3FB,_tmp3FB + 5},(void*)& Cyc__genrep_10};static unsigned char _tmp3FC[3]="tq";
static struct _tuple4 Cyc__gentuple_144={offsetof(struct Cyc_Absyn_Vardecl,tq),{
_tmp3FC,_tmp3FC,_tmp3FC + 3},(void*)& Cyc__genrep_132};static unsigned char _tmp3FD[
5]="type";static struct _tuple4 Cyc__gentuple_145={offsetof(struct Cyc_Absyn_Vardecl,type),{
_tmp3FD,_tmp3FD,_tmp3FD + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static
unsigned char _tmp3FE[12]="initializer";static struct _tuple4 Cyc__gentuple_146={
offsetof(struct Cyc_Absyn_Vardecl,initializer),{_tmp3FE,_tmp3FE,_tmp3FE + 12},(
void*)& Cyc__genrep_73};static unsigned char _tmp3FF[4]="rgn";static struct _tuple4
Cyc__gentuple_147={offsetof(struct Cyc_Absyn_Vardecl,rgn),{_tmp3FF,_tmp3FF,
_tmp3FF + 4},(void*)& Cyc__genrep_61};static unsigned char _tmp400[11]="attributes";
static struct _tuple4 Cyc__gentuple_148={offsetof(struct Cyc_Absyn_Vardecl,attributes),{
_tmp400,_tmp400,_tmp400 + 11},(void*)& Cyc__genrep_83};static unsigned char _tmp401[
8]="escapes";static struct _tuple4 Cyc__gentuple_149={offsetof(struct Cyc_Absyn_Vardecl,escapes),{
_tmp401,_tmp401,_tmp401 + 8},(void*)((void*)& Cyc__genrep_102)};static struct
_tuple4*Cyc__genarr_150[8]={& Cyc__gentuple_142,& Cyc__gentuple_143,& Cyc__gentuple_144,&
Cyc__gentuple_145,& Cyc__gentuple_146,& Cyc__gentuple_147,& Cyc__gentuple_148,& Cyc__gentuple_149};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_151,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((struct
_tuple4**)Cyc__genarr_150),(void*)((struct _tuple4**)Cyc__genarr_150),(void*)((
struct _tuple4**)Cyc__genarr_150 + 8)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131={
1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};struct _tuple82{unsigned int f1;
struct Cyc_Absyn_Vardecl*f2;};static struct _tuple5 Cyc__gentuple_382={offsetof(
struct _tuple82,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_383={
offsetof(struct _tuple82,f2),(void*)& Cyc__genrep_131};static struct _tuple5*Cyc__genarr_384[
2]={& Cyc__gentuple_382,& Cyc__gentuple_383};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_381={4,sizeof(struct _tuple82),{(void*)((struct _tuple5**)Cyc__genarr_384),(
void*)((struct _tuple5**)Cyc__genarr_384),(void*)((struct _tuple5**)Cyc__genarr_384
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1140;struct _tuple83{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Core_Opt*
f4;struct Cyc_Absyn_Exp*f5;int f6;};static struct _tuple5 Cyc__gentuple_1141={
offsetof(struct _tuple83,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_1142={
offsetof(struct _tuple83,f2),(void*)& Cyc__genrep_226};static struct _tuple5 Cyc__gentuple_1143={
offsetof(struct _tuple83,f3),(void*)& Cyc__genrep_129};static struct _tuple5 Cyc__gentuple_1144={
offsetof(struct _tuple83,f4),(void*)& Cyc__genrep_61};static struct _tuple5 Cyc__gentuple_1145={
offsetof(struct _tuple83,f5),(void*)& Cyc__genrep_77};static struct _tuple5 Cyc__gentuple_1146={
offsetof(struct _tuple83,f6),(void*)((void*)& Cyc__genrep_102)};static struct
_tuple5*Cyc__genarr_1147[6]={& Cyc__gentuple_1141,& Cyc__gentuple_1142,& Cyc__gentuple_1143,&
Cyc__gentuple_1144,& Cyc__gentuple_1145,& Cyc__gentuple_1146};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1140={4,sizeof(struct _tuple83),{(void*)((struct _tuple5**)Cyc__genarr_1147),(
void*)((struct _tuple5**)Cyc__genarr_1147),(void*)((struct _tuple5**)Cyc__genarr_1147
+ 6)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1136;static struct _tuple5
Cyc__gentuple_1137={offsetof(struct _tuple10,f1),(void*)& Cyc__genrep_5};static
struct _tuple5 Cyc__gentuple_1138={offsetof(struct _tuple10,f2),(void*)& Cyc__genrep_130};
static struct _tuple5*Cyc__genarr_1139[2]={& Cyc__gentuple_1137,& Cyc__gentuple_1138};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1136={4,sizeof(struct _tuple10),{(
void*)((struct _tuple5**)Cyc__genarr_1139),(void*)((struct _tuple5**)Cyc__genarr_1139),(
void*)((struct _tuple5**)Cyc__genarr_1139 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1132;struct _tuple84{unsigned int f1;struct Cyc_Absyn_Structdecl*f2;};
static struct _tuple5 Cyc__gentuple_1133={offsetof(struct _tuple84,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_1134={offsetof(struct _tuple84,f2),(void*)((void*)&
Cyc__genrep_346)};static struct _tuple5*Cyc__genarr_1135[2]={& Cyc__gentuple_1133,&
Cyc__gentuple_1134};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1132={4,
sizeof(struct _tuple84),{(void*)((struct _tuple5**)Cyc__genarr_1135),(void*)((
struct _tuple5**)Cyc__genarr_1135),(void*)((struct _tuple5**)Cyc__genarr_1135 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1128;struct _tuple85{unsigned int
f1;struct Cyc_Absyn_Uniondecl*f2;};static struct _tuple5 Cyc__gentuple_1129={
offsetof(struct _tuple85,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_1130={
offsetof(struct _tuple85,f2),(void*)((void*)& Cyc__genrep_921)};static struct
_tuple5*Cyc__genarr_1131[2]={& Cyc__gentuple_1129,& Cyc__gentuple_1130};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1128={4,sizeof(struct _tuple85),{(void*)((
struct _tuple5**)Cyc__genarr_1131),(void*)((struct _tuple5**)Cyc__genarr_1131),(
void*)((struct _tuple5**)Cyc__genarr_1131 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1124;struct _tuple86{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;};
static struct _tuple5 Cyc__gentuple_1125={offsetof(struct _tuple86,f1),(void*)& Cyc__genrep_5};
static struct _tuple5 Cyc__gentuple_1126={offsetof(struct _tuple86,f2),(void*)((void*)&
Cyc__genrep_318)};static struct _tuple5*Cyc__genarr_1127[2]={& Cyc__gentuple_1125,&
Cyc__gentuple_1126};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1124={4,
sizeof(struct _tuple86),{(void*)((struct _tuple5**)Cyc__genarr_1127),(void*)((
struct _tuple5**)Cyc__genarr_1127),(void*)((struct _tuple5**)Cyc__genarr_1127 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1120;struct _tuple87{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple5 Cyc__gentuple_1121={
offsetof(struct _tuple87,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_1122={
offsetof(struct _tuple87,f2),(void*)& Cyc__genrep_285};static struct _tuple5*Cyc__genarr_1123[
2]={& Cyc__gentuple_1121,& Cyc__gentuple_1122};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1120={4,sizeof(struct _tuple87),{(void*)((struct _tuple5**)Cyc__genarr_1123),(
void*)((struct _tuple5**)Cyc__genarr_1123),(void*)((struct _tuple5**)Cyc__genarr_1123
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_41;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_42;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;
static unsigned char _tmp40B[12]="Typedefdecl";static struct _tagged_arr Cyc__genname_1116={
_tmp40B,_tmp40B,_tmp40B + 12};static unsigned char _tmp40C[5]="name";static struct
_tuple4 Cyc__gentuple_1112={offsetof(struct Cyc_Absyn_Typedefdecl,name),{_tmp40C,
_tmp40C,_tmp40C + 5},(void*)& Cyc__genrep_10};static unsigned char _tmp40D[4]="tvs";
static struct _tuple4 Cyc__gentuple_1113={offsetof(struct Cyc_Absyn_Typedefdecl,tvs),{
_tmp40D,_tmp40D,_tmp40D + 4},(void*)& Cyc__genrep_254};static unsigned char _tmp40E[
5]="defn";static struct _tuple4 Cyc__gentuple_1114={offsetof(struct Cyc_Absyn_Typedefdecl,defn),{
_tmp40E,_tmp40E,_tmp40E + 5},(void*)& Cyc_Absyn_type_t_rep};static struct _tuple4*
Cyc__genarr_1115[3]={& Cyc__gentuple_1112,& Cyc__gentuple_1113,& Cyc__gentuple_1114};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_1116,sizeof(struct Cyc_Absyn_Typedefdecl),{(void*)((
struct _tuple4**)Cyc__genarr_1115),(void*)((struct _tuple4**)Cyc__genarr_1115),(
void*)((struct _tuple4**)Cyc__genarr_1115 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_42={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct
_tuple88{unsigned int f1;struct Cyc_Absyn_Typedefdecl*f2;};static struct _tuple5 Cyc__gentuple_1117={
offsetof(struct _tuple88,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_1118={
offsetof(struct _tuple88,f2),(void*)& Cyc__genrep_42};static struct _tuple5*Cyc__genarr_1119[
2]={& Cyc__gentuple_1117,& Cyc__gentuple_1118};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_41={4,sizeof(struct _tuple88),{(void*)((struct _tuple5**)Cyc__genarr_1119),(
void*)((struct _tuple5**)Cyc__genarr_1119),(void*)((struct _tuple5**)Cyc__genarr_1119
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_36;struct _tuple89{
unsigned int f1;struct _tagged_arr*f2;struct Cyc_List_List*f3;};static struct _tuple5
Cyc__gentuple_37={offsetof(struct _tuple89,f1),(void*)& Cyc__genrep_5};static
struct _tuple5 Cyc__gentuple_38={offsetof(struct _tuple89,f2),(void*)& Cyc__genrep_12};
static struct _tuple5 Cyc__gentuple_39={offsetof(struct _tuple89,f3),(void*)& Cyc__genrep_0};
static struct _tuple5*Cyc__genarr_40[3]={& Cyc__gentuple_37,& Cyc__gentuple_38,& Cyc__gentuple_39};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_36={4,sizeof(struct _tuple89),{(
void*)((struct _tuple5**)Cyc__genarr_40),(void*)((struct _tuple5**)Cyc__genarr_40),(
void*)((struct _tuple5**)Cyc__genarr_40 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9;struct _tuple90{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*
f3;};static struct _tuple5 Cyc__gentuple_32={offsetof(struct _tuple90,f1),(void*)&
Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_33={offsetof(struct _tuple90,f2),(
void*)& Cyc__genrep_10};static struct _tuple5 Cyc__gentuple_34={offsetof(struct
_tuple90,f3),(void*)& Cyc__genrep_0};static struct _tuple5*Cyc__genarr_35[3]={& Cyc__gentuple_32,&
Cyc__gentuple_33,& Cyc__gentuple_34};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_9={
4,sizeof(struct _tuple90),{(void*)((struct _tuple5**)Cyc__genarr_35),(void*)((
struct _tuple5**)Cyc__genarr_35),(void*)((struct _tuple5**)Cyc__genarr_35 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_4;static struct _tuple5 Cyc__gentuple_6={
offsetof(struct _tuple10,f1),(void*)& Cyc__genrep_5};static struct _tuple5 Cyc__gentuple_7={
offsetof(struct _tuple10,f2),(void*)& Cyc__genrep_0};static struct _tuple5*Cyc__genarr_8[
2]={& Cyc__gentuple_6,& Cyc__gentuple_7};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_4={
4,sizeof(struct _tuple10),{(void*)((struct _tuple5**)Cyc__genarr_8),(void*)((
struct _tuple5**)Cyc__genarr_8),(void*)((struct _tuple5**)Cyc__genarr_8 + 2)}};
static struct _tuple6*Cyc__genarr_3[0]={};static unsigned char _tmp415[6]="Var_d";
static struct _tuple4 Cyc__gentuple_1148={0,{_tmp415,_tmp415,_tmp415 + 6},(void*)&
Cyc__genrep_381};static unsigned char _tmp416[5]="Fn_d";static struct _tuple4 Cyc__gentuple_1149={
1,{_tmp416,_tmp416,_tmp416 + 5},(void*)& Cyc__genrep_81};static unsigned char
_tmp417[6]="Let_d";static struct _tuple4 Cyc__gentuple_1150={2,{_tmp417,_tmp417,
_tmp417 + 6},(void*)& Cyc__genrep_1140};static unsigned char _tmp418[7]="Letv_d";
static struct _tuple4 Cyc__gentuple_1151={3,{_tmp418,_tmp418,_tmp418 + 7},(void*)&
Cyc__genrep_1136};static unsigned char _tmp419[9]="Struct_d";static struct _tuple4
Cyc__gentuple_1152={4,{_tmp419,_tmp419,_tmp419 + 9},(void*)& Cyc__genrep_1132};
static unsigned char _tmp41A[8]="Union_d";static struct _tuple4 Cyc__gentuple_1153={5,{
_tmp41A,_tmp41A,_tmp41A + 8},(void*)& Cyc__genrep_1128};static unsigned char _tmp41B[
9]="Tunion_d";static struct _tuple4 Cyc__gentuple_1154={6,{_tmp41B,_tmp41B,_tmp41B + 
9},(void*)& Cyc__genrep_1124};static unsigned char _tmp41C[7]="Enum_d";static struct
_tuple4 Cyc__gentuple_1155={7,{_tmp41C,_tmp41C,_tmp41C + 7},(void*)& Cyc__genrep_1120};
static unsigned char _tmp41D[10]="Typedef_d";static struct _tuple4 Cyc__gentuple_1156={
8,{_tmp41D,_tmp41D,_tmp41D + 10},(void*)& Cyc__genrep_41};static unsigned char
_tmp41E[12]="Namespace_d";static struct _tuple4 Cyc__gentuple_1157={9,{_tmp41E,
_tmp41E,_tmp41E + 12},(void*)& Cyc__genrep_36};static unsigned char _tmp41F[8]="Using_d";
static struct _tuple4 Cyc__gentuple_1158={10,{_tmp41F,_tmp41F,_tmp41F + 8},(void*)&
Cyc__genrep_9};static unsigned char _tmp420[10]="ExternC_d";static struct _tuple4 Cyc__gentuple_1159={
11,{_tmp420,_tmp420,_tmp420 + 10},(void*)& Cyc__genrep_4};static struct _tuple4*Cyc__genarr_1160[
12]={& Cyc__gentuple_1148,& Cyc__gentuple_1149,& Cyc__gentuple_1150,& Cyc__gentuple_1151,&
Cyc__gentuple_1152,& Cyc__gentuple_1153,& Cyc__gentuple_1154,& Cyc__gentuple_1155,&
Cyc__gentuple_1156,& Cyc__gentuple_1157,& Cyc__gentuple_1158,& Cyc__gentuple_1159};
static unsigned char _tmp422[9]="Raw_decl";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={
5,{_tmp422,_tmp422,_tmp422 + 9},{(void*)((struct _tuple6**)Cyc__genarr_3),(void*)((
struct _tuple6**)Cyc__genarr_3),(void*)((struct _tuple6**)Cyc__genarr_3 + 0)},{(
void*)((struct _tuple4**)Cyc__genarr_1160),(void*)((struct _tuple4**)Cyc__genarr_1160),(
void*)((struct _tuple4**)Cyc__genarr_1160 + 12)}};static unsigned char _tmp423[5]="Decl";
static struct _tagged_arr Cyc__genname_1164={_tmp423,_tmp423,_tmp423 + 5};static
unsigned char _tmp424[2]="r";static struct _tuple4 Cyc__gentuple_1161={offsetof(
struct Cyc_Absyn_Decl,r),{_tmp424,_tmp424,_tmp424 + 2},(void*)& Cyc_Absyn_raw_decl_t_rep};
static unsigned char _tmp425[4]="loc";static struct _tuple4 Cyc__gentuple_1162={
offsetof(struct Cyc_Absyn_Decl,loc),{_tmp425,_tmp425,_tmp425 + 4},(void*)& Cyc__genrep_2};
static struct _tuple4*Cyc__genarr_1163[2]={& Cyc__gentuple_1161,& Cyc__gentuple_1162};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={3,(struct _tagged_arr*)&
Cyc__genname_1164,sizeof(struct Cyc_Absyn_Decl),{(void*)((struct _tuple4**)Cyc__genarr_1163),(
void*)((struct _tuple4**)Cyc__genarr_1163),(void*)((struct _tuple4**)Cyc__genarr_1163
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)&
Cyc_struct_Absyn_Decl_rep)};static unsigned char _tmp428[5]="List";static struct
_tagged_arr Cyc__genname_1168={_tmp428,_tmp428,_tmp428 + 5};static unsigned char
_tmp429[3]="hd";static struct _tuple4 Cyc__gentuple_1165={offsetof(struct Cyc_List_List,hd),{
_tmp429,_tmp429,_tmp429 + 3},(void*)& Cyc__genrep_1};static unsigned char _tmp42A[3]="tl";
static struct _tuple4 Cyc__gentuple_1166={offsetof(struct Cyc_List_List,tl),{_tmp42A,
_tmp42A,_tmp42A + 3},(void*)& Cyc__genrep_0};static struct _tuple4*Cyc__genarr_1167[
2]={& Cyc__gentuple_1165,& Cyc__gentuple_1166};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_1168,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple4**)Cyc__genarr_1167),(void*)((struct _tuple4**)Cyc__genarr_1167),(
void*)((struct _tuple4**)Cyc__genarr_1167 + 2)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};
void*Cyc_decls_rep=(void*)& Cyc__genrep_0;
