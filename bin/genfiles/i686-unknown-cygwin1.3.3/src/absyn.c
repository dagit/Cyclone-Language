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
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
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
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);extern struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(
void*k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*exist_ts,
struct Cyc_Core_Opt*fs,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*s,struct _tuple0*n,struct
Cyc_List_List*ts,struct Cyc_Core_Opt*exist_ts,struct Cyc_Core_Opt*fs,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*exist_ts,struct
Cyc_Core_Opt*fs,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);extern
struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*loc);extern
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*);extern void*Cyc_Absyn_pointer_expand(
void*);extern int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _tagged_arr*);extern struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct
_tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};extern struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);extern struct _tagged_arr Cyc_Absyn_attribute2string(void*);
extern int Cyc_Absyn_fntype_att(void*a);extern struct _tagged_arr*Cyc_Absyn_fieldname(
int);struct _tuple4{void*f1;struct _tuple0*f2;};extern struct _tuple4 Cyc_Absyn_aggr_kinded_name(
void*);extern struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);
extern int Cyc_Absyn_is_union_type(void*);extern void Cyc_Absyn_print_decls(struct
Cyc_List_List*);struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};
struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{
int tag;void*f1;};struct _tuple5{unsigned int f1;struct _tagged_arr f2;void*f3;};
struct Cyc_Typerep_Struct_struct{int tag;struct _tagged_arr*f1;unsigned int f2;
struct _tagged_arr f3;};struct _tuple6{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{
int tag;unsigned int f1;struct _tagged_arr f2;};struct _tuple7{unsigned int f1;struct
_tagged_arr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;
struct _tagged_arr f2;struct _tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;unsigned int f3;struct _tagged_arr
f4;};struct _tuple8{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern void*Cyc_decls_rep;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);struct Cyc_Cstdio___abstractFILE;
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
11];struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
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
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*
Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(
void*t);extern void Cyc_Marshal_print_type(void*rep,void*val);static int Cyc_Absyn_zstrlist_cmp(
struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){return((int(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))
Cyc_List_list_cmp)(Cyc_Std_zstrptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct
Cyc_List_List*vs1,struct Cyc_List_List*vs2){return Cyc_Absyn_zstrlist_cmp(vs1,vs2);}
struct _tuple9{void*f1;void*f2;};int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct
_tuple0*q2){void*_tmp0=(*q1).f1;void*_tmp1=(*q2).f1;{struct _tuple9 _tmp3=({struct
_tuple9 _tmp2;_tmp2.f1=_tmp0;_tmp2.f2=_tmp1;_tmp2;});void*_tmp4;void*_tmp5;void*
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
_LL2A: goto _LL2C;_LL2C: return x;_LL2E: {struct Cyc_Absyn_Conref*_tmp24=Cyc_Absyn_compress_conref(*
_tmp23);*_tmp23=_tmp24;return _tmp24;}_LL28:;}void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x){void*_tmp25=(void*)(Cyc_Absyn_compress_conref(x))->v;void*_tmp26;_LL31: if((
unsigned int)_tmp25 > 1?*((int*)_tmp25)== 0: 0){_LL35: _tmp26=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp25)->f1;goto _LL32;}else{goto _LL33;}_LL33: goto _LL34;_LL32: return _tmp26;_LL34:({
void*_tmp27[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("conref_val",sizeof(unsigned char),11),_tag_arr(_tmp27,sizeof(void*),0));});
_LL30:;}void*Cyc_Absyn_conref_def(void*y,struct Cyc_Absyn_Conref*x){void*_tmp28=(
void*)(Cyc_Absyn_compress_conref(x))->v;void*_tmp29;_LL37: if((unsigned int)
_tmp28 > 1?*((int*)_tmp28)== 0: 0){_LL3B: _tmp29=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp28)->f1;goto _LL38;}else{goto _LL39;}_LL39: goto _LL3A;_LL38: return _tmp29;_LL3A:
return y;_LL36:;}void*Cyc_Absyn_compress_kb(void*k){void*_tmp2A=k;struct Cyc_Core_Opt*
_tmp2B;struct Cyc_Core_Opt*_tmp2C;struct Cyc_Core_Opt*_tmp2D;struct Cyc_Core_Opt
_tmp2E;void*_tmp2F;void**_tmp30;struct Cyc_Core_Opt*_tmp31;struct Cyc_Core_Opt
_tmp32;void*_tmp33;void**_tmp34;_LL3D: if(*((int*)_tmp2A)== 0){goto _LL3E;}else{
goto _LL3F;}_LL3F: if(*((int*)_tmp2A)== 1){_LL47: _tmp2B=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp2A)->f1;if(_tmp2B == 0){goto _LL40;}else{goto _LL41;}}else{goto _LL41;}_LL41: if(*((
int*)_tmp2A)== 2){_LL48: _tmp2C=((struct Cyc_Absyn_Less_kb_struct*)_tmp2A)->f1;if(
_tmp2C == 0){goto _LL42;}else{goto _LL43;}}else{goto _LL43;}_LL43: if(*((int*)_tmp2A)
== 1){_LL49: _tmp2D=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp2A)->f1;if(_tmp2D == 
0){goto _LL45;}else{_tmp2E=*_tmp2D;_LL4A: _tmp2F=(void*)_tmp2E.v;_tmp30=(void**)&(*((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp2A)->f1).v;goto _LL44;}}else{goto _LL45;}
_LL45: if(*((int*)_tmp2A)== 2){_LL4B: _tmp31=((struct Cyc_Absyn_Less_kb_struct*)
_tmp2A)->f1;if(_tmp31 == 0){goto _LL3C;}else{_tmp32=*_tmp31;_LL4C: _tmp33=(void*)
_tmp32.v;_tmp34=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)_tmp2A)->f1).v;goto
_LL46;}}else{goto _LL3C;}_LL3E: goto _LL40;_LL40: goto _LL42;_LL42: return k;_LL44:
_tmp34=_tmp30;goto _LL46;_LL46:*_tmp34=Cyc_Absyn_compress_kb(*_tmp34);return*
_tmp34;_LL3C:;}void*Cyc_Absyn_force_kb(void*kb){void*_tmp35=Cyc_Absyn_compress_kb(
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
Cyc_Absyn_Null_Exception_pr,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static unsigned char
_tmp4E[13]="Array_bounds";static struct _tagged_arr Cyc_Absyn_Array_bounds_str={
_tmp4E,_tmp4E,_tmp4E + 13};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(void*)&
Cyc_Absyn_abs_null,& Cyc_Absyn_Array_bounds_str};struct _tuple0*Cyc_Absyn_Array_bounds_name=&
Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={&
Cyc_Absyn_Array_bounds_pr,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static unsigned char _tmp4F[16]="Match_Exception";
static struct _tagged_arr Cyc_Absyn_Match_Exception_str={_tmp4F,_tmp4F,_tmp4F + 16};
static struct _tuple0 Cyc_Absyn_Match_Exception_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Match_Exception_str};
struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;
static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static unsigned char _tmp50[10]="Bad_alloc";static
struct _tagged_arr Cyc_Absyn_Bad_alloc_str={_tmp50,_tmp50,_tmp50 + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Bad_alloc_str};
struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Tunionfield
Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,(void*)((void*)3)};struct
Cyc_Absyn_Tunionfield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;static
struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,0};
static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
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
_tmp66 + 8};static unsigned char _tmp67[4]="Std";static struct _tagged_arr Cyc_Absyn_st_str={
_tmp67,_tmp67,_tmp67 + 4};static struct _tagged_arr*Cyc_Absyn_sf=& Cyc_Absyn_sf_str;
static struct _tagged_arr*Cyc_Absyn_st=& Cyc_Absyn_st_str;void*Cyc_Absyn_file_typ(){
if(Cyc_Absyn_file_t_opt == 0){struct _tuple0*file_t_name=({struct _tuple0*_tmp70=
_cycalloc(sizeof(*_tmp70));_tmp70->f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_Abs_n_struct _tmp72;
_tmp72.tag=1;_tmp72.f1=({struct _tagged_arr*_tmp73[1];_tmp73[0]=Cyc_Absyn_st;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp73,
sizeof(struct _tagged_arr*),1));});_tmp72;});_tmp71;});_tmp70->f2=Cyc_Absyn_sf;
_tmp70;});struct Cyc_Absyn_Aggrdecl*sd=({struct Cyc_Absyn_Aggrdecl*_tmp6F=
_cycalloc(sizeof(*_tmp6F));_tmp6F->kind=(void*)((void*)0);_tmp6F->sc=(void*)((
void*)1);_tmp6F->name=file_t_name;_tmp6F->tvs=0;_tmp6F->exist_vars=0;_tmp6F->fields=
0;_tmp6F->attributes=0;_tmp6F;});void*file_struct_typ=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=({struct Cyc_Absyn_AggrType_struct
_tmp6A;_tmp6A.tag=10;_tmp6A.f1=({struct Cyc_Absyn_AggrInfo _tmp6B;_tmp6B.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));
_tmp6C[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp6D;_tmp6D.tag=1;_tmp6D.f1=({
struct Cyc_Absyn_Aggrdecl**_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=sd;_tmp6E;});
_tmp6D;});_tmp6C;}));_tmp6B.targs=0;_tmp6B;});_tmp6A;});_tmp69;});Cyc_Absyn_file_t_opt=({
struct Cyc_Core_Opt*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->v=(void*)Cyc_Absyn_at_typ(
file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual());_tmp68;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_file_t_opt))->v;}static struct Cyc_Core_Opt*
Cyc_Absyn_void_star_t_opt=0;void*Cyc_Absyn_void_star_typ(){if(Cyc_Absyn_void_star_t_opt
== 0){Cyc_Absyn_void_star_t_opt=({struct Cyc_Core_Opt*_tmp74=_cycalloc(sizeof(*
_tmp74));_tmp74->v=(void*)Cyc_Absyn_star_typ((void*)0,(void*)2,Cyc_Absyn_empty_tqual());
_tmp74;});}return(void*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_void_star_t_opt))->v;}
struct Cyc_Core_Opt*Cyc_Absyn_void_star_typ_opt(){if(Cyc_Absyn_void_star_t_opt == 
0){Cyc_Absyn_void_star_t_opt=({struct Cyc_Core_Opt*_tmp75=_cycalloc(sizeof(*
_tmp75));_tmp75->v=(void*)Cyc_Absyn_star_typ((void*)0,(void*)2,Cyc_Absyn_empty_tqual());
_tmp75;});}return Cyc_Absyn_void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct
Cyc_Absyn_empty_eff={18,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;
void*Cyc_Absyn_aggr_typ(void*k,struct _tagged_arr*name){return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Absyn_AggrType_struct
_tmp78;_tmp78.tag=10;_tmp78.f1=({struct Cyc_Absyn_AggrInfo _tmp79;_tmp79.aggr_info=(
void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmp7A=_cycalloc(sizeof(*
_tmp7A));_tmp7A[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmp7B;_tmp7B.tag=0;
_tmp7B.f1=(void*)k;_tmp7B.f2=({struct _tuple0*_tmp7C=_cycalloc(sizeof(*_tmp7C));
_tmp7C->f1=Cyc_Absyn_rel_ns_null;_tmp7C->f2=name;_tmp7C;});_tmp7B;});_tmp7A;}));
_tmp79.targs=0;_tmp79;});_tmp78;});_tmp77;});}void*Cyc_Absyn_strct(struct
_tagged_arr*name){return Cyc_Absyn_aggr_typ((void*)0,name);}void*Cyc_Absyn_union_typ(
struct _tagged_arr*name){return Cyc_Absyn_aggr_typ((void*)1,name);}void*Cyc_Absyn_strctq(
struct _tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp7D=
_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_Absyn_AggrType_struct _tmp7E;
_tmp7E.tag=10;_tmp7E.f1=({struct Cyc_Absyn_AggrInfo _tmp7F;_tmp7F.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmp80=_cycalloc(sizeof(*_tmp80));
_tmp80[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmp81;_tmp81.tag=0;_tmp81.f1=(
void*)((void*)0);_tmp81.f2=name;_tmp81;});_tmp80;}));_tmp7F.targs=0;_tmp7F;});
_tmp7E;});_tmp7D;});}void*Cyc_Absyn_unionq_typ(struct _tuple0*name){return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82[0]=({
struct Cyc_Absyn_AggrType_struct _tmp83;_tmp83.tag=10;_tmp83.f1=({struct Cyc_Absyn_AggrInfo
_tmp84;_tmp84.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp86;_tmp86.tag=0;_tmp86.f1=(void*)((void*)1);_tmp86.f2=name;_tmp86;});_tmp85;}));
_tmp84.targs=0;_tmp84;});_tmp83;});_tmp82;});}struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Exp*_tmp87=
_cycalloc(sizeof(*_tmp87));_tmp87->topt=0;_tmp87->r=(void*)r;_tmp87->loc=loc;
_tmp87->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);_tmp87;});}struct Cyc_Absyn_Exp*
Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_New_e_struct*_tmp88=
_cycalloc(sizeof(*_tmp88));_tmp88[0]=({struct Cyc_Absyn_New_e_struct _tmp89;_tmp89.tag=
15;_tmp89.f1=rgn_handle;_tmp89.f2=e;_tmp89;});_tmp88;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){return({struct Cyc_Absyn_Exp*_tmp8A=
_cycalloc(sizeof(*_tmp8A));_tmp8A->topt=e->topt;_tmp8A->r=(void*)((void*)e->r);
_tmp8A->loc=e->loc;_tmp8A->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);_tmp8A;});}
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*c,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*_tmp8B=_cycalloc(
sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Absyn_Const_e_struct _tmp8C;_tmp8C.tag=0;
_tmp8C.f1=(void*)c;_tmp8C;});_tmp8B;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_Const_e_struct
_tmp8E;_tmp8E.tag=0;_tmp8E.f1=(void*)((void*)0);_tmp8E;});_tmp8D;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*s,int i,struct Cyc_Position_Segment*seg){
return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*_tmp8F=_cycalloc(
sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_Int_c_struct _tmp90;_tmp90.tag=2;
_tmp90.f1=(void*)s;_tmp90.f2=i;_tmp90;});_tmp8F;}),seg);}struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((void*)0,i,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)1,(int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_signed_int_exp(b? 1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(
1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){
return Cyc_Absyn_bool_exp(0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
unsigned char c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((void*)({
struct Cyc_Absyn_Char_c_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({
struct Cyc_Absyn_Char_c_struct _tmp92;_tmp92.tag=0;_tmp92.f1=(void*)((void*)1);
_tmp92.f2=c;_tmp92;});_tmp91;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _tagged_arr f,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Float_c_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[
0]=({struct Cyc_Absyn_Float_c_struct _tmp94;_tmp94.tag=4;_tmp94.f1=f;_tmp94;});
_tmp93;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _tagged_arr s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_String_c_struct*
_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Absyn_String_c_struct
_tmp96;_tmp96.tag=5;_tmp96.f1=s;_tmp96;});_tmp95;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Var_e_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({
struct Cyc_Absyn_Var_e_struct _tmp98;_tmp98.tag=1;_tmp98.f1=q;_tmp98.f2=(void*)((
void*)0);_tmp98;});_tmp97;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct
_tuple0*q,void*b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Var_e_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({
struct Cyc_Absyn_Var_e_struct _tmp9A;_tmp9A.tag=1;_tmp9A.f1=q;_tmp9A.f2=(void*)b;
_tmp9A;});_tmp99;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct
_tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp9C;_tmp9C.tag=2;_tmp9C.f1=q;_tmp9C;});
_tmp9B;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp9E;_tmp9E.tag=3;_tmp9E.f1=(void*)p;_tmp9E.f2=es;_tmp9E;});_tmp9D;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*
_tmp9F));_tmp9F->hd=e;_tmp9F->tl=0;_tmp9F;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpA0=_cycalloc(sizeof(*
_tmpA0));_tmpA0->hd=e1;_tmpA0->tl=({struct Cyc_List_List*_tmpA1=_cycalloc(sizeof(*
_tmpA1));_tmpA1->hd=e2;_tmpA1->tl=0;_tmpA1;});_tmpA0;}),loc);}struct Cyc_Absyn_Exp*
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
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmpA2=
_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmpA3;
_tmpA3.tag=4;_tmpA3.f1=e1;_tmpA3.f2=popt;_tmpA3.f3=e2;_tmpA3;});_tmpA2;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_Increment_e_struct
_tmpA5;_tmpA5.tag=5;_tmpA5.f1=e;_tmpA5.f2=(void*)i;_tmpA5;});_tmpA4;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmpA7;_tmpA7.tag=6;_tmpA7.f1=e1;_tmpA7.f2=e2;_tmpA7.f3=e3;_tmpA7;});_tmpA6;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,e2,Cyc_Absyn_false_exp(
loc),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,Cyc_Absyn_true_exp(
loc),e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[
0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpA9;_tmpA9.tag=7;_tmpA9.f1=e1;_tmpA9.f2=
e2;_tmpA9;});_tmpA8;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*_tmpAA=
_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpAB;_tmpAB.tag=8;_tmpAB.f1=e;_tmpAB.f2=es;_tmpAB;});_tmpAA;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmpAC=
_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_Absyn_FnCall_e_struct _tmpAD;
_tmpAD.tag=9;_tmpAD.f1=e;_tmpAD.f2=es;_tmpAD.f3=0;_tmpAD;});_tmpAC;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpAF;_tmpAF.tag=11;_tmpAF.f1=e;_tmpAF;});_tmpAE;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpB1;_tmpB1.tag=12;_tmpB1.f1=e;_tmpB1.f2=ts;_tmpB1;});_tmpB0;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*_tmpB2=
_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_Cast_e_struct _tmpB3;
_tmpB3.tag=13;_tmpB3.f1=(void*)t;_tmpB3.f2=e;_tmpB3;});_tmpB2;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_struct*_tmpB4=_cycalloc(
sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_Throw_e_struct _tmpB5;_tmpB5.tag=10;
_tmpB5.f1=e;_tmpB5;});_tmpB4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Address_e_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));
_tmpB6[0]=({struct Cyc_Absyn_Address_e_struct _tmpB7;_tmpB7.tag=14;_tmpB7.f1=e;
_tmpB7;});_tmpB6;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpB9;_tmpB9.tag=16;_tmpB9.f1=(void*)t;_tmpB9;});_tmpB8;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*_tmpBA=
_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Absyn_Sizeofexp_e_struct _tmpBB;
_tmpBB.tag=17;_tmpBB.f1=e;_tmpBB;});_tmpBA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmpBD;_tmpBD.tag=18;_tmpBD.f1=(void*)t;_tmpBD.f2=(
void*)of;_tmpBD;});_tmpBC;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpBF;_tmpBF.tag=19;_tmpBF.f1=tvs;_tmpBF.f2=(
void*)t;_tmpBF;});_tmpBE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpC1;_tmpC1.tag=20;_tmpC1.f1=e;_tmpC1;});_tmpC0;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpC3;_tmpC3.tag=21;_tmpC3.f1=e;_tmpC3.f2=n;
_tmpC3;});_tmpC2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));
_tmpC4[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpC5;_tmpC5.tag=22;_tmpC5.f1=e;
_tmpC5.f2=n;_tmpC5;});_tmpC4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpC6=
_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpC7;
_tmpC7.tag=23;_tmpC7.f1=e1;_tmpC7.f2=e2;_tmpC7;});_tmpC6;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*_tmpC8=_cycalloc(
sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_Tuple_e_struct _tmpC9;_tmpC9.tag=24;
_tmpC9.f1=es;_tmpC9;});_tmpC8;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));
_tmpCA[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpCB;_tmpCB.tag=35;_tmpCB.f1=s;
_tmpCB;});_tmpCA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct _tuple10{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*
_tmpCC));_tmpCC->hd=({struct _tuple10*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->f1=
0;_tmpCD->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpCD;});_tmpCC->tl=dles;_tmpCC;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpCE=_cycalloc(
sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_Array_e_struct _tmpCF;_tmpCF.tag=26;
_tmpCF.f1=dles;_tmpCF;});_tmpCE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD0=
_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD1;_tmpD1.tag=34;_tmpD1.f1=n;_tmpD1.f2=dles;_tmpD1;});_tmpD0;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->r=(void*)s;_tmpD2->loc=
loc;_tmpD2->non_local_preds=0;_tmpD2->try_depth=0;_tmpD2->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpD2;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpD3=_cycalloc(
sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_Exp_s_struct _tmpD4;_tmpD4.tag=0;
_tmpD4.f1=e;_tmpD4;});_tmpD3;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0){return Cyc_Absyn_skip_stmt(
loc);}else{if(ss->tl == 0){return(struct Cyc_Absyn_Stmt*)ss->hd;}else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpD5=
_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_Return_s_struct _tmpD6;
_tmpD6.tag=2;_tmpD6.f1=e;_tmpD6;});_tmpD5;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpD8;_tmpD8.tag=3;_tmpD8.f1=e;_tmpD8.f2=s1;_tmpD8.f3=s2;_tmpD8;});_tmpD7;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_While_s_struct
_tmpDA;_tmpDA.tag=4;_tmpDA.f1=({struct _tuple2 _tmpDB;_tmpDB.f1=e;_tmpDB.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpDB;});_tmpDA.f2=s;_tmpDA;});_tmpD9;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_Break_s_struct
_tmpDD;_tmpDD.tag=5;_tmpDD.f1=0;_tmpDD;});_tmpDC;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));
_tmpDE[0]=({struct Cyc_Absyn_Continue_s_struct _tmpDF;_tmpDF.tag=6;_tmpDF.f1=0;
_tmpDF;});_tmpDE;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_For_s_struct _tmpE1;
_tmpE1.tag=8;_tmpE1.f1=e1;_tmpE1.f2=({struct _tuple2 _tmpE2;_tmpE2.f1=e2;_tmpE2.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpE2;});_tmpE1.f3=({struct _tuple2 _tmpE3;_tmpE3.f1=
e3;_tmpE3.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmpE3;});_tmpE1.f4=s;_tmpE1;});_tmpE0;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*
_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_Switch_s_struct
_tmpE5;_tmpE5.tag=9;_tmpE5.f1=e;_tmpE5.f2=scs;_tmpE5;});_tmpE4;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){void*_tmpE6=(void*)s1->r;_LL59: if((int)_tmpE6
== 0){goto _LL5A;}else{goto _LL5B;}_LL5B: goto _LL5C;_LL5A: return s2;_LL5C: return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Seq_s_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=({
struct Cyc_Absyn_Seq_s_struct _tmpE8;_tmpE8.tag=1;_tmpE8.f1=s1;_tmpE8.f2=s2;_tmpE8;});
_tmpE7;}),loc);_LL58:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*
el,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Fallthru_s_struct*
_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9[0]=({struct Cyc_Absyn_Fallthru_s_struct
_tmpEA;_tmpEA.tag=11;_tmpEA.f1=el;_tmpEA.f2=0;_tmpEA;});_tmpE9;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpEC;_tmpEC.tag=12;_tmpEC.f1=d;_tmpEC.f2=s;_tmpEC;});_tmpEB;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({
struct Cyc_Absyn_Var_d_struct _tmpF0;_tmpF0.tag=0;_tmpF0.f1=Cyc_Absyn_new_vardecl(
x,t,init);_tmpF0;});_tmpEF;}),loc);return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpEE;_tmpEE.tag=12;_tmpEE.f1=d;_tmpEE.f2=s;_tmpEE;});_tmpED;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Cut_s_struct*_tmpF1=_cycalloc(
sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Absyn_Cut_s_struct _tmpF2;_tmpF2.tag=13;
_tmpF2.f1=s;_tmpF2;});_tmpF1;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[
0]=({struct Cyc_Absyn_Splice_s_struct _tmpF4;_tmpF4.tag=14;_tmpF4.f1=s;_tmpF4;});
_tmpF3;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _tagged_arr*v,
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[
0]=({struct Cyc_Absyn_Label_s_struct _tmpF6;_tmpF6.tag=15;_tmpF6.f1=v;_tmpF6.f2=s;
_tmpF6;});_tmpF5;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Do_s_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=({
struct Cyc_Absyn_Do_s_struct _tmpF8;_tmpF8.tag=16;_tmpF8.f1=s;_tmpF8.f2=({struct
_tuple2 _tmpF9;_tmpF9.f1=e;_tmpF9.f2=Cyc_Absyn_skip_stmt(e->loc);_tmpF9;});_tmpF8;});
_tmpF7;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*
s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_TryCatch_s_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));
_tmpFA[0]=({struct Cyc_Absyn_TryCatch_s_struct _tmpFB;_tmpFB.tag=17;_tmpFB.f1=s;
_tmpFB.f2=scs;_tmpFB;});_tmpFA;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _tagged_arr*lab,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Goto_s_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC[0]=({
struct Cyc_Absyn_Goto_s_struct _tmpFD;_tmpFD.tag=7;_tmpFD.f1=lab;_tmpFD.f2=0;
_tmpFD;});_tmpFC;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(
struct Cyc_List_List*d,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_ForArray_s_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_ForArray_s_struct
_tmpFF;_tmpFF.tag=19;_tmpFF.f1=({struct Cyc_Absyn_ForArrayInfo _tmp100;_tmp100.defns=
d;_tmp100.condition=({struct _tuple2 _tmp102;_tmp102.f1=e1;_tmp102.f2=Cyc_Absyn_skip_stmt(
e1->loc);_tmp102;});_tmp100.delta=({struct _tuple2 _tmp101;_tmp101.f1=e2;_tmp101.f2=
Cyc_Absyn_skip_stmt(e2->loc);_tmp101;});_tmp100.body=s;_tmp100;});_tmpFF;});
_tmpFE;}),loc);}struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*
s){return({struct Cyc_Absyn_Pat*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->r=(
void*)p;_tmp103->topt=0;_tmp103->loc=s;_tmp103;});}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp104=
_cycalloc(sizeof(*_tmp104));_tmp104->r=(void*)r;_tmp104->loc=loc;_tmp104;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Core_Opt*
t_opt,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Let_d_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[
0]=({struct Cyc_Absyn_Let_d_struct _tmp106;_tmp106.tag=2;_tmp106.f1=p;_tmp106.f2=0;
_tmp106.f3=t_opt;_tmp106.f4=e;_tmp106.f5=0;_tmp106;});_tmp105;}),loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp107=
_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Absyn_Letv_d_struct _tmp108;
_tmp108.tag=3;_tmp108.f1=vds;_tmp108;});_tmp107;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->sc=(void*)((
void*)2);_tmp109->name=x;_tmp109->tq=Cyc_Absyn_empty_tqual();_tmp109->type=(void*)
t;_tmp109->initializer=init;_tmp109->rgn=0;_tmp109->attributes=0;_tmp109->escapes=
0;_tmp109;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp10A=
_cycalloc(sizeof(*_tmp10A));_tmp10A->sc=(void*)((void*)0);_tmp10A->name=x;
_tmp10A->tq=Cyc_Absyn_empty_tqual();_tmp10A->type=(void*)t;_tmp10A->initializer=
init;_tmp10A->rgn=0;_tmp10A->attributes=0;_tmp10A->escapes=0;_tmp10A;});}struct
Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*exist_ts,struct Cyc_Core_Opt*fs,struct Cyc_List_List*atts,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp10C;_tmp10C.tag=4;_tmp10C.f1=({struct Cyc_Absyn_Aggrdecl*_tmp10D=_cycalloc(
sizeof(*_tmp10D));_tmp10D->kind=(void*)k;_tmp10D->sc=(void*)s;_tmp10D->name=n;
_tmp10D->tvs=ts;_tmp10D->exist_vars=exist_ts;_tmp10D->fields=fs;_tmp10D->attributes=
atts;_tmp10D;});_tmp10C;});_tmp10B;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*exist_ts,struct
Cyc_Core_Opt*fs,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_aggr_decl((void*)0,s,n,ts,exist_ts,fs,atts,loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
exist_ts,struct Cyc_Core_Opt*fs,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_aggr_decl((void*)1,s,n,ts,exist_ts,fs,atts,loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_xtunion,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Tunion_d_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({
struct Cyc_Absyn_Tunion_d_struct _tmp10F;_tmp10F.tag=5;_tmp10F.f1=({struct Cyc_Absyn_Tuniondecl*
_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->sc=(void*)s;_tmp110->name=n;_tmp110->tvs=
ts;_tmp110->fields=fs;_tmp110->is_xtunion=is_xtunion;_tmp110;});_tmp10F;});
_tmp10E;}),loc);}static struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){
return({struct _tuple1*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->f1=(*a).f1;
_tmp111->f2=(*a).f2;_tmp111->f3=Cyc_Absyn_pointer_expand((*a).f3);_tmp111;});}
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){return(void*)({
struct Cyc_Absyn_FnType_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=({
struct Cyc_Absyn_FnType_struct _tmp113;_tmp113.tag=8;_tmp113.f1=({struct Cyc_Absyn_FnInfo
_tmp114;_tmp114.tvars=tvs;_tmp114.ret_typ=(void*)Cyc_Absyn_pointer_expand(
ret_typ);_tmp114.effect=eff_typ;_tmp114.args=((struct Cyc_List_List*(*)(struct
_tuple1*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,
args);_tmp114.c_varargs=c_varargs;_tmp114.cyc_varargs=cyc_varargs;_tmp114.rgn_po=
rgn_po;_tmp114.attributes=atts;_tmp114;});_tmp113;});_tmp112;});}void*Cyc_Absyn_pointer_expand(
void*t){void*_tmp115=Cyc_Tcutil_compress(t);_LL5E: if((unsigned int)_tmp115 > 3?*((
int*)_tmp115)== 8: 0){goto _LL5F;}else{goto _LL60;}_LL60: goto _LL61;_LL5F: return Cyc_Absyn_at_typ(
t,(void*)2,Cyc_Absyn_empty_tqual());_LL61: return t;_LL5D:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp116=(void*)e->r;void*_tmp117;void*_tmp118;struct
Cyc_Absyn_Vardecl*_tmp119;void*_tmp11A;struct Cyc_Absyn_Vardecl*_tmp11B;struct Cyc_Absyn_Exp*
_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;_LL63: if(*((int*)
_tmp116)== 1){_LL79: _tmp117=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp116)->f2;
if((unsigned int)_tmp117 > 1?*((int*)_tmp117)== 1: 0){goto _LL64;}else{goto _LL65;}}
else{goto _LL65;}_LL65: if(*((int*)_tmp116)== 1){_LL7A: _tmp118=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp116)->f2;if((unsigned int)_tmp118 > 1?*((int*)_tmp118)== 0: 0){_LL7B: _tmp119=((
struct Cyc_Absyn_Global_b_struct*)_tmp118)->f1;goto _LL66;}else{goto _LL67;}}else{
goto _LL67;}_LL67: if(*((int*)_tmp116)== 1){_LL7C: _tmp11A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp116)->f2;if((unsigned int)_tmp11A > 1?*((int*)_tmp11A)== 3: 0){_LL7D: _tmp11B=((
struct Cyc_Absyn_Local_b_struct*)_tmp11A)->f1;goto _LL68;}else{goto _LL69;}}else{
goto _LL69;}_LL69: if(*((int*)_tmp116)== 1){goto _LL6A;}else{goto _LL6B;}_LL6B: if(*((
int*)_tmp116)== 22){goto _LL6C;}else{goto _LL6D;}_LL6D: if(*((int*)_tmp116)== 20){
goto _LL6E;}else{goto _LL6F;}_LL6F: if(*((int*)_tmp116)== 23){goto _LL70;}else{goto
_LL71;}_LL71: if(*((int*)_tmp116)== 21){_LL7E: _tmp11C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp116)->f1;goto _LL72;}else{goto _LL73;}_LL73: if(*((int*)_tmp116)== 12){_LL7F:
_tmp11D=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp116)->f1;goto _LL74;}else{
goto _LL75;}_LL75: if(*((int*)_tmp116)== 11){_LL80: _tmp11E=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp116)->f1;goto _LL76;}else{goto _LL77;}_LL77: goto _LL78;_LL64: return 0;_LL66:
_tmp11B=_tmp119;goto _LL68;_LL68: {void*_tmp11F=Cyc_Tcutil_compress((void*)
_tmp11B->type);_LL82: if((unsigned int)_tmp11F > 3?*((int*)_tmp11F)== 7: 0){goto
_LL83;}else{goto _LL84;}_LL84: goto _LL85;_LL83: return 0;_LL85: return 1;_LL81:;}_LL6A:
goto _LL6C;_LL6C: goto _LL6E;_LL6E: goto _LL70;_LL70: return 1;_LL72: return Cyc_Absyn_is_lvalue(
_tmp11C);_LL74: return Cyc_Absyn_is_lvalue(_tmp11D);_LL76: return Cyc_Absyn_is_lvalue(
_tmp11E);_LL78: return 0;_LL62:;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(
struct Cyc_List_List*fields,struct _tagged_arr*v){{struct Cyc_List_List*_tmp120=
fields;for(0;_tmp120 != 0;_tmp120=_tmp120->tl){if(Cyc_Std_zstrptrcmp(((struct Cyc_Absyn_Aggrfield*)
_tmp120->hd)->name,v)== 0){return(struct Cyc_Absyn_Aggrfield*)((struct Cyc_Absyn_Aggrfield*)
_tmp120->hd);}}}return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _tagged_arr*v){return ad->fields == 0? 0: Cyc_Absyn_lookup_field((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ad->fields))->v,v);}
struct _tuple3*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){for(0;i
!= 0;-- i){if(ts == 0){return 0;}ts=ts->tl;}if(ts == 0){return 0;}return(struct
_tuple3*)((struct _tuple3*)ts->hd);}struct _tagged_arr Cyc_Absyn_attribute2string(
void*a){void*_tmp121=a;int _tmp122;int _tmp123;struct _tagged_arr _tmp124;int _tmp125;
int _tmp126;void*_tmp127;int _tmp128;int _tmp129;void*_tmp12A;_LL87: if((
unsigned int)_tmp121 > 16?*((int*)_tmp121)== 0: 0){_LLB1: _tmp122=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp121)->f1;goto _LL88;}else{goto _LL89;}_LL89: if((int)_tmp121 == 0){goto _LL8A;}
else{goto _LL8B;}_LL8B: if((int)_tmp121 == 1){goto _LL8C;}else{goto _LL8D;}_LL8D: if((
int)_tmp121 == 2){goto _LL8E;}else{goto _LL8F;}_LL8F: if((int)_tmp121 == 3){goto _LL90;}
else{goto _LL91;}_LL91: if((int)_tmp121 == 4){goto _LL92;}else{goto _LL93;}_LL93: if((
unsigned int)_tmp121 > 16?*((int*)_tmp121)== 1: 0){_LLB2: _tmp123=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp121)->f1;goto _LL94;}else{goto _LL95;}_LL95: if((int)_tmp121 == 5){goto _LL96;}
else{goto _LL97;}_LL97: if((unsigned int)_tmp121 > 16?*((int*)_tmp121)== 2: 0){_LLB3:
_tmp124=((struct Cyc_Absyn_Section_att_struct*)_tmp121)->f1;goto _LL98;}else{goto
_LL99;}_LL99: if((int)_tmp121 == 6){goto _LL9A;}else{goto _LL9B;}_LL9B: if((int)
_tmp121 == 7){goto _LL9C;}else{goto _LL9D;}_LL9D: if((int)_tmp121 == 8){goto _LL9E;}
else{goto _LL9F;}_LL9F: if((int)_tmp121 == 9){goto _LLA0;}else{goto _LLA1;}_LLA1: if((
int)_tmp121 == 10){goto _LLA2;}else{goto _LLA3;}_LLA3: if((int)_tmp121 == 11){goto
_LLA4;}else{goto _LLA5;}_LLA5: if((int)_tmp121 == 12){goto _LLA6;}else{goto _LLA7;}
_LLA7: if((int)_tmp121 == 13){goto _LLA8;}else{goto _LLA9;}_LLA9: if((int)_tmp121 == 
14){goto _LLAA;}else{goto _LLAB;}_LLAB: if((int)_tmp121 == 15){goto _LLAC;}else{goto
_LLAD;}_LLAD: if((unsigned int)_tmp121 > 16?*((int*)_tmp121)== 3: 0){_LLB6: _tmp127=(
void*)((struct Cyc_Absyn_Format_att_struct*)_tmp121)->f1;if((int)_tmp127 == 0){
goto _LLB5;}else{goto _LLAF;}_LLB5: _tmp126=((struct Cyc_Absyn_Format_att_struct*)
_tmp121)->f2;goto _LLB4;_LLB4: _tmp125=((struct Cyc_Absyn_Format_att_struct*)
_tmp121)->f3;goto _LLAE;}else{goto _LLAF;}_LLAF: if((unsigned int)_tmp121 > 16?*((
int*)_tmp121)== 3: 0){_LLB9: _tmp12A=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp121)->f1;if((int)_tmp12A == 1){goto _LLB8;}else{goto _LL86;}_LLB8: _tmp129=((
struct Cyc_Absyn_Format_att_struct*)_tmp121)->f2;goto _LLB7;_LLB7: _tmp128=((struct
Cyc_Absyn_Format_att_struct*)_tmp121)->f3;goto _LLB0;}else{goto _LL86;}_LL88:
return(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp12C;_tmp12C.tag=1;
_tmp12C.f1=(int)((unsigned int)_tmp122);{void*_tmp12B[1]={& _tmp12C};Cyc_Std_aprintf(
_tag_arr("regparm(%d)",sizeof(unsigned char),12),_tag_arr(_tmp12B,sizeof(void*),
1));}});_LL8A: return _tag_arr("stdcall",sizeof(unsigned char),8);_LL8C: return
_tag_arr("cdecl",sizeof(unsigned char),6);_LL8E: return _tag_arr("fastcall",
sizeof(unsigned char),9);_LL90: return _tag_arr("noreturn",sizeof(unsigned char),9);
_LL92: return _tag_arr("const",sizeof(unsigned char),6);_LL94: if(_tmp123 == - 1){
return _tag_arr("aligned",sizeof(unsigned char),8);}else{return(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp12E;_tmp12E.tag=1;_tmp12E.f1=(int)((unsigned int)
_tmp123);{void*_tmp12D[1]={& _tmp12E};Cyc_Std_aprintf(_tag_arr("aligned(%d)",
sizeof(unsigned char),12),_tag_arr(_tmp12D,sizeof(void*),1));}});}_LL96: return
_tag_arr("packed",sizeof(unsigned char),7);_LL98: return(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp130;_tmp130.tag=0;_tmp130.f1=(struct
_tagged_arr)_tmp124;{void*_tmp12F[1]={& _tmp130};Cyc_Std_aprintf(_tag_arr("section(\"%s\")",
sizeof(unsigned char),14),_tag_arr(_tmp12F,sizeof(void*),1));}});_LL9A: return
_tag_arr("nocommon",sizeof(unsigned char),9);_LL9C: return _tag_arr("shared",
sizeof(unsigned char),7);_LL9E: return _tag_arr("unused",sizeof(unsigned char),7);
_LLA0: return _tag_arr("weak",sizeof(unsigned char),5);_LLA2: return _tag_arr("dllimport",
sizeof(unsigned char),10);_LLA4: return _tag_arr("dllexport",sizeof(unsigned char),
10);_LLA6: return _tag_arr("no_instrument_function",sizeof(unsigned char),23);
_LLA8: return _tag_arr("constructor",sizeof(unsigned char),12);_LLAA: return
_tag_arr("destructor",sizeof(unsigned char),11);_LLAC: return _tag_arr("no_check_memory_usage",
sizeof(unsigned char),22);_LLAE: return(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp133;_tmp133.tag=1;_tmp133.f1=(unsigned int)_tmp125;{struct Cyc_Std_Int_pa_struct
_tmp132;_tmp132.tag=1;_tmp132.f1=(unsigned int)_tmp126;{void*_tmp131[2]={&
_tmp132,& _tmp133};Cyc_Std_aprintf(_tag_arr("format(printf,%u,%u)",sizeof(
unsigned char),21),_tag_arr(_tmp131,sizeof(void*),2));}}});_LLB0: return(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp136;_tmp136.tag=1;_tmp136.f1=(
unsigned int)_tmp128;{struct Cyc_Std_Int_pa_struct _tmp135;_tmp135.tag=1;_tmp135.f1=(
unsigned int)_tmp129;{void*_tmp134[2]={& _tmp135,& _tmp136};Cyc_Std_aprintf(
_tag_arr("format(scanf,%u,%u)",sizeof(unsigned char),20),_tag_arr(_tmp134,
sizeof(void*),2));}}});_LL86:;}int Cyc_Absyn_fntype_att(void*a){void*_tmp137=a;
_LLBB: if((unsigned int)_tmp137 > 16?*((int*)_tmp137)== 0: 0){goto _LLBC;}else{goto
_LLBD;}_LLBD: if((int)_tmp137 == 2){goto _LLBE;}else{goto _LLBF;}_LLBF: if((int)
_tmp137 == 0){goto _LLC0;}else{goto _LLC1;}_LLC1: if((int)_tmp137 == 1){goto _LLC2;}
else{goto _LLC3;}_LLC3: if((int)_tmp137 == 3){goto _LLC4;}else{goto _LLC5;}_LLC5: if((
unsigned int)_tmp137 > 16?*((int*)_tmp137)== 3: 0){goto _LLC6;}else{goto _LLC7;}
_LLC7: if((int)_tmp137 == 4){goto _LLC8;}else{goto _LLC9;}_LLC9: goto _LLCA;_LLBC: goto
_LLBE;_LLBE: goto _LLC0;_LLC0: goto _LLC2;_LLC2: goto _LLC4;_LLC4: goto _LLC6;_LLC6: goto
_LLC8;_LLC8: return 1;_LLCA: return 0;_LLBA:;}static unsigned char _tmp138[3]="f0";
static struct _tagged_arr Cyc_Absyn_f0={_tmp138,_tmp138,_tmp138 + 3};static struct
_tagged_arr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};static struct _tagged_arr
Cyc_Absyn_field_names={(void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v),(
void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v),(void*)((struct _tagged_arr**)
Cyc_Absyn_field_names_v + 1)};struct _tagged_arr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_arr_size(Cyc_Absyn_field_names,sizeof(struct _tagged_arr*));
if(i >= fsz){Cyc_Absyn_field_names=({unsigned int _tmp139=(unsigned int)(i + 1);
struct _tagged_arr**_tmp13A=(struct _tagged_arr**)_cycalloc(_check_times(sizeof(
struct _tagged_arr*),_tmp139));struct _tagged_arr _tmp13F=_tag_arr(_tmp13A,sizeof(
struct _tagged_arr*),(unsigned int)(i + 1));{unsigned int _tmp13B=_tmp139;
unsigned int j;for(j=0;j < _tmp13B;j ++){_tmp13A[j]=j < fsz?*((struct _tagged_arr**)
_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct _tagged_arr*),(int)j)):({
struct _tagged_arr*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C[0]=(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp13E;_tmp13E.tag=1;_tmp13E.f1=(int)j;{
void*_tmp13D[1]={& _tmp13E};Cyc_Std_aprintf(_tag_arr("f%d",sizeof(unsigned char),
4),_tag_arr(_tmp13D,sizeof(void*),1));}});_tmp13C;});}};_tmp13F;});}return*((
struct _tagged_arr**)_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*),i));}struct _tuple4 Cyc_Absyn_aggr_kinded_name(void*info){void*
_tmp140=info;struct _tuple0*_tmp141;void*_tmp142;struct Cyc_Absyn_Aggrdecl**
_tmp143;struct Cyc_Absyn_Aggrdecl*_tmp144;struct Cyc_Absyn_Aggrdecl _tmp145;struct
_tuple0*_tmp146;void*_tmp147;_LLCC: if(*((int*)_tmp140)== 0){_LLD1: _tmp142=(void*)((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp140)->f1;goto _LLD0;_LLD0: _tmp141=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp140)->f2;goto _LLCD;}else{goto _LLCE;}
_LLCE: if(*((int*)_tmp140)== 1){_LLD2: _tmp143=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp140)->f1;_tmp144=*_tmp143;_tmp145=*_tmp144;_LLD4: _tmp147=(void*)_tmp145.kind;
goto _LLD3;_LLD3: _tmp146=_tmp145.name;goto _LLCF;}else{goto _LLCB;}_LLCD: return({
struct _tuple4 _tmp148;_tmp148.f1=_tmp142;_tmp148.f2=_tmp141;_tmp148;});_LLCF:
return({struct _tuple4 _tmp149;_tmp149.f1=_tmp147;_tmp149.f2=_tmp146;_tmp149;});
_LLCB:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info){void*
_tmp14A=info;struct _tuple0*_tmp14B;void*_tmp14C;struct Cyc_Absyn_Aggrdecl**
_tmp14D;struct Cyc_Absyn_Aggrdecl*_tmp14E;_LLD6: if(*((int*)_tmp14A)== 0){_LLDB:
_tmp14C=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp14A)->f1;goto _LLDA;
_LLDA: _tmp14B=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp14A)->f2;goto _LLD7;}
else{goto _LLD8;}_LLD8: if(*((int*)_tmp14A)== 1){_LLDC: _tmp14D=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp14A)->f1;_tmp14E=*_tmp14D;goto _LLD9;}else{goto _LLD5;}_LLD7:({void*_tmp14F[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unchecked aggrdecl",
sizeof(unsigned char),19),_tag_arr(_tmp14F,sizeof(void*),0));});_LLD9: return
_tmp14E;_LLD5:;}int Cyc_Absyn_is_union_type(void*t){void*_tmp150=Cyc_Tcutil_compress(
t);void*_tmp151;struct Cyc_Absyn_AggrInfo _tmp152;void*_tmp153;_LLDE: if((
unsigned int)_tmp150 > 3?*((int*)_tmp150)== 11: 0){_LLE4: _tmp151=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp150)->f1;if((int)_tmp151 == 1){goto _LLDF;}
else{goto _LLE0;}}else{goto _LLE0;}_LLE0: if((unsigned int)_tmp150 > 3?*((int*)
_tmp150)== 10: 0){_LLE5: _tmp152=((struct Cyc_Absyn_AggrType_struct*)_tmp150)->f1;
_LLE6: _tmp153=(void*)_tmp152.aggr_info;goto _LLE1;}else{goto _LLE2;}_LLE2: goto
_LLE3;_LLDF: return 1;_LLE1: return(Cyc_Absyn_aggr_kinded_name(_tmp153)).f1 == (void*)
1;_LLE3: return 0;_LLDD:;}void Cyc_Absyn_print_decls(struct Cyc_List_List*decls){((
void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(Cyc_decls_rep,&
decls);({void*_tmp154[0]={};Cyc_Std_printf(_tag_arr("\n",sizeof(unsigned char),2),
_tag_arr(_tmp154,sizeof(void*),0));});}extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;
static unsigned char _tmp155[6]="Var_d";static struct _tuple7 Cyc__gentuple_3={0,{
_tmp155,_tmp155,_tmp155 + 6}};static unsigned char _tmp156[5]="Fn_d";static struct
_tuple7 Cyc__gentuple_4={1,{_tmp156,_tmp156,_tmp156 + 5}};static unsigned char
_tmp157[6]="Let_d";static struct _tuple7 Cyc__gentuple_5={2,{_tmp157,_tmp157,
_tmp157 + 6}};static unsigned char _tmp158[7]="Letv_d";static struct _tuple7 Cyc__gentuple_6={
3,{_tmp158,_tmp158,_tmp158 + 7}};static unsigned char _tmp159[7]="Aggr_d";static
struct _tuple7 Cyc__gentuple_7={4,{_tmp159,_tmp159,_tmp159 + 7}};static
unsigned char _tmp15A[9]="Tunion_d";static struct _tuple7 Cyc__gentuple_8={5,{
_tmp15A,_tmp15A,_tmp15A + 9}};static unsigned char _tmp15B[7]="Enum_d";static struct
_tuple7 Cyc__gentuple_9={6,{_tmp15B,_tmp15B,_tmp15B + 7}};static unsigned char
_tmp15C[10]="Typedef_d";static struct _tuple7 Cyc__gentuple_10={7,{_tmp15C,_tmp15C,
_tmp15C + 10}};static unsigned char _tmp15D[12]="Namespace_d";static struct _tuple7
Cyc__gentuple_11={8,{_tmp15D,_tmp15D,_tmp15D + 12}};static unsigned char _tmp15E[8]="Using_d";
static struct _tuple7 Cyc__gentuple_12={9,{_tmp15E,_tmp15E,_tmp15E + 8}};static
unsigned char _tmp15F[10]="ExternC_d";static struct _tuple7 Cyc__gentuple_13={10,{
_tmp15F,_tmp15F,_tmp15F + 10}};static struct _tuple7*Cyc__genarr_14[11]={& Cyc__gentuple_3,&
Cyc__gentuple_4,& Cyc__gentuple_5,& Cyc__gentuple_6,& Cyc__gentuple_7,& Cyc__gentuple_8,&
Cyc__gentuple_9,& Cyc__gentuple_10,& Cyc__gentuple_11,& Cyc__gentuple_12,& Cyc__gentuple_13};
static struct _tuple5*Cyc__genarr_15[0]={};static unsigned char _tmp161[9]="Raw_decl";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={5,{_tmp161,_tmp161,
_tmp161 + 9},{(void*)((struct _tuple7**)Cyc__genarr_14),(void*)((struct _tuple7**)
Cyc__genarr_14),(void*)((struct _tuple7**)Cyc__genarr_14 + 11)},{(void*)((struct
_tuple5**)Cyc__genarr_15),(void*)((struct _tuple5**)Cyc__genarr_15),(void*)((
struct _tuple5**)Cyc__genarr_15 + 0)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};
static unsigned char _tmp163[5]="Decl";static struct _tagged_arr Cyc__genname_19={
_tmp163,_tmp163,_tmp163 + 5};static unsigned char _tmp164[2]="r";static struct
_tuple5 Cyc__gentuple_16={offsetof(struct Cyc_Absyn_Decl,r),{_tmp164,_tmp164,
_tmp164 + 2},(void*)& Cyc_Absyn_raw_decl_t_rep};static unsigned char _tmp165[4]="loc";
static struct _tuple5 Cyc__gentuple_17={offsetof(struct Cyc_Absyn_Decl,loc),{_tmp165,
_tmp165,_tmp165 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_18[2]={&
Cyc__gentuple_16,& Cyc__gentuple_17};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={
3,(struct _tagged_arr*)& Cyc__genname_19,sizeof(struct Cyc_Absyn_Decl),{(void*)((
struct _tuple5**)Cyc__genarr_18),(void*)((struct _tuple5**)Cyc__genarr_18),(void*)((
struct _tuple5**)Cyc__genarr_18 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={
1,1,(void*)((void*)& Cyc_struct_Absyn_Decl_rep)};static unsigned char _tmp168[5]="List";
static struct _tagged_arr Cyc__genname_23={_tmp168,_tmp168,_tmp168 + 5};static
unsigned char _tmp169[3]="hd";static struct _tuple5 Cyc__gentuple_20={offsetof(
struct Cyc_List_List,hd),{_tmp169,_tmp169,_tmp169 + 3},(void*)& Cyc__genrep_1};
static unsigned char _tmp16A[3]="tl";static struct _tuple5 Cyc__gentuple_21={
offsetof(struct Cyc_List_List,tl),{_tmp16A,_tmp16A,_tmp16A + 3},(void*)& Cyc__genrep_0};
static struct _tuple5*Cyc__genarr_22[2]={& Cyc__gentuple_20,& Cyc__gentuple_21};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_23,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_22),(void*)((struct _tuple5**)Cyc__genarr_22),(void*)((
struct _tuple5**)Cyc__genarr_22 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};void*Cyc_decls_rep=(
void*)& Cyc__genrep_0;
