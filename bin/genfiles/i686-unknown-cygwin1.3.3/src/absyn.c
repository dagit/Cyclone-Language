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
f3;struct Cyc_List_List*f4;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;
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
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
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
struct _tagged_arr f2;struct _tagged_arr f3;};struct _tuple8{struct _tagged_arr f1;void*
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
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_394;static struct Cyc_Typerep_Int_struct
Cyc__genrep_5={0,0,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep;extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;static unsigned char _tmp156[7]="Static";
static struct _tuple7 Cyc__gentuple_135={0,{_tmp156,_tmp156,_tmp156 + 7}};static
unsigned char _tmp157[9]="Abstract";static struct _tuple7 Cyc__gentuple_136={1,{
_tmp157,_tmp157,_tmp157 + 9}};static unsigned char _tmp158[7]="Public";static struct
_tuple7 Cyc__gentuple_137={2,{_tmp158,_tmp158,_tmp158 + 7}};static unsigned char
_tmp159[7]="Extern";static struct _tuple7 Cyc__gentuple_138={3,{_tmp159,_tmp159,
_tmp159 + 7}};static unsigned char _tmp15A[8]="ExternC";static struct _tuple7 Cyc__gentuple_139={
4,{_tmp15A,_tmp15A,_tmp15A + 8}};static struct _tuple7*Cyc__genarr_140[5]={& Cyc__gentuple_135,&
Cyc__gentuple_136,& Cyc__gentuple_137,& Cyc__gentuple_138,& Cyc__gentuple_139};
static struct _tuple5*Cyc__genarr_141[0]={};static unsigned char _tmp15C[6]="Scope";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep={5,{_tmp15C,_tmp15C,_tmp15C
+ 6},{(void*)((struct _tuple7**)Cyc__genarr_140),(void*)((struct _tuple7**)Cyc__genarr_140),(
void*)((struct _tuple7**)Cyc__genarr_140 + 5)},{(void*)((struct _tuple5**)Cyc__genarr_141),(
void*)((struct _tuple5**)Cyc__genarr_141),(void*)((struct _tuple5**)Cyc__genarr_141
+ 0)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_11;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_17;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_18;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12;extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_13;static struct Cyc_Typerep_Int_struct Cyc__genrep_14={0,0,8};static
struct Cyc_Typerep_FatPtr_struct Cyc__genrep_13={2,(void*)((void*)& Cyc__genrep_14)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12={1,1,(void*)((void*)& Cyc__genrep_13)};
static unsigned char _tmp160[5]="List";static struct _tagged_arr Cyc__genname_22={
_tmp160,_tmp160,_tmp160 + 5};static unsigned char _tmp161[3]="hd";static struct
_tuple5 Cyc__gentuple_19={offsetof(struct Cyc_List_List,hd),{_tmp161,_tmp161,
_tmp161 + 3},(void*)& Cyc__genrep_12};static unsigned char _tmp162[3]="tl";static
struct _tuple5 Cyc__gentuple_20={offsetof(struct Cyc_List_List,tl),{_tmp162,_tmp162,
_tmp162 + 3},(void*)& Cyc__genrep_18};static struct _tuple5*Cyc__genarr_21[2]={& Cyc__gentuple_19,&
Cyc__gentuple_20};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_22,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_21),(void*)((struct _tuple5**)Cyc__genarr_21),(void*)((
struct _tuple5**)Cyc__genarr_21 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_18={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};struct _tuple11{
unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple6 Cyc__gentuple_23={
offsetof(struct _tuple11,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_24={
offsetof(struct _tuple11,f2),(void*)& Cyc__genrep_18};static struct _tuple6*Cyc__genarr_25[
2]={& Cyc__gentuple_23,& Cyc__gentuple_24};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_17={
4,sizeof(struct _tuple11),{(void*)((struct _tuple6**)Cyc__genarr_25),(void*)((
struct _tuple6**)Cyc__genarr_25),(void*)((struct _tuple6**)Cyc__genarr_25 + 2)}};
static unsigned char _tmp166[6]="Loc_n";static struct _tuple7 Cyc__gentuple_15={0,{
_tmp166,_tmp166,_tmp166 + 6}};static struct _tuple7*Cyc__genarr_16[1]={& Cyc__gentuple_15};
static unsigned char _tmp167[6]="Rel_n";static struct _tuple5 Cyc__gentuple_26={0,{
_tmp167,_tmp167,_tmp167 + 6},(void*)& Cyc__genrep_17};static unsigned char _tmp168[6]="Abs_n";
static struct _tuple5 Cyc__gentuple_27={1,{_tmp168,_tmp168,_tmp168 + 6},(void*)& Cyc__genrep_17};
static struct _tuple5*Cyc__genarr_28[2]={& Cyc__gentuple_26,& Cyc__gentuple_27};
static unsigned char _tmp16A[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={
5,{_tmp16A,_tmp16A,_tmp16A + 8},{(void*)((struct _tuple7**)Cyc__genarr_16),(void*)((
struct _tuple7**)Cyc__genarr_16),(void*)((struct _tuple7**)Cyc__genarr_16 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_28),(void*)((struct _tuple5**)Cyc__genarr_28),(
void*)((struct _tuple5**)Cyc__genarr_28 + 2)}};static struct _tuple6 Cyc__gentuple_29={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_30={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_12};static
struct _tuple6*Cyc__genarr_31[2]={& Cyc__gentuple_29,& Cyc__gentuple_30};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_11={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_31),(void*)((struct _tuple6**)Cyc__genarr_31),(void*)((
struct _tuple6**)Cyc__genarr_31 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10={
1,1,(void*)((void*)& Cyc__genrep_11)};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_132;
struct _tuple12{unsigned char f1;};static struct _tuple6 Cyc__gentuple_133={offsetof(
struct _tuple12,f1),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*Cyc__genarr_134[
1]={& Cyc__gentuple_133};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_132={4,
sizeof(struct _tuple12),{(void*)((struct _tuple6**)Cyc__genarr_134),(void*)((
struct _tuple6**)Cyc__genarr_134),(void*)((struct _tuple6**)Cyc__genarr_134 + 1)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1069;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1070;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static unsigned char _tmp16E[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_185={0,{_tmp16E,_tmp16E,_tmp16E + 8}};static
unsigned char _tmp16F[8]="MemKind";static struct _tuple7 Cyc__gentuple_186={1,{
_tmp16F,_tmp16F,_tmp16F + 8}};static unsigned char _tmp170[8]="BoxKind";static
struct _tuple7 Cyc__gentuple_187={2,{_tmp170,_tmp170,_tmp170 + 8}};static
unsigned char _tmp171[8]="RgnKind";static struct _tuple7 Cyc__gentuple_188={3,{
_tmp171,_tmp171,_tmp171 + 8}};static unsigned char _tmp172[8]="EffKind";static
struct _tuple7 Cyc__gentuple_189={4,{_tmp172,_tmp172,_tmp172 + 8}};static struct
_tuple7*Cyc__genarr_190[5]={& Cyc__gentuple_185,& Cyc__gentuple_186,& Cyc__gentuple_187,&
Cyc__gentuple_188,& Cyc__gentuple_189};static struct _tuple5*Cyc__genarr_191[0]={};
static unsigned char _tmp174[5]="Kind";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep={
5,{_tmp174,_tmp174,_tmp174 + 5},{(void*)((struct _tuple7**)Cyc__genarr_190),(void*)((
struct _tuple7**)Cyc__genarr_190),(void*)((struct _tuple7**)Cyc__genarr_190 + 5)},{(
void*)((struct _tuple5**)Cyc__genarr_191),(void*)((struct _tuple5**)Cyc__genarr_191),(
void*)((struct _tuple5**)Cyc__genarr_191 + 0)}};static unsigned char _tmp175[4]="Opt";
static struct _tagged_arr Cyc__genname_1073={_tmp175,_tmp175,_tmp175 + 4};static
unsigned char _tmp176[2]="v";static struct _tuple5 Cyc__gentuple_1071={offsetof(
struct Cyc_Core_Opt,v),{_tmp176,_tmp176,_tmp176 + 2},(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple5*Cyc__genarr_1072[1]={& Cyc__gentuple_1071};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kind_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_1073,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1072),(void*)((
struct _tuple5**)Cyc__genarr_1072),(void*)((struct _tuple5**)Cyc__genarr_1072 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1070={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep;static unsigned char _tmp179[4]="Opt";static
struct _tagged_arr Cyc__genname_64={_tmp179,_tmp179,_tmp179 + 4};static
unsigned char _tmp17A[2]="v";static struct _tuple5 Cyc__gentuple_62={offsetof(struct
Cyc_Core_Opt,v),{_tmp17A,_tmp17A,_tmp17A + 2},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple5*Cyc__genarr_63[1]={& Cyc__gentuple_62};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_64,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_63),(void*)((
struct _tuple5**)Cyc__genarr_63),(void*)((struct _tuple5**)Cyc__genarr_63 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};
static struct Cyc_Typerep_Int_struct Cyc__genrep_102={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_357;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_236;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_182;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_212;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_212={1,1,(void*)((void*)& Cyc__genrep_102)};extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_204;
static struct _tuple6 Cyc__gentuple_205={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_206={offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple6*Cyc__genarr_207[2]={& Cyc__gentuple_205,& Cyc__gentuple_206};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_204={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_207),(void*)((struct _tuple6**)Cyc__genarr_207),(
void*)((struct _tuple6**)Cyc__genarr_207 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_200;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_192;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;static
unsigned char _tmp180[4]="Opt";static struct _tagged_arr Cyc__genname_195={_tmp180,
_tmp180,_tmp180 + 4};static unsigned char _tmp181[2]="v";static struct _tuple5 Cyc__gentuple_193={
offsetof(struct Cyc_Core_Opt,v),{_tmp181,_tmp181,_tmp181 + 2},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_194[1]={& Cyc__gentuple_193};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_195,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_194),(void*)((
struct _tuple5**)Cyc__genarr_194),(void*)((struct _tuple5**)Cyc__genarr_194 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_192={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple13{unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_201={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_202={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_192};static struct _tuple6*Cyc__genarr_203[
2]={& Cyc__gentuple_201,& Cyc__gentuple_202};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_200={4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_203),(
void*)((struct _tuple6**)Cyc__genarr_203),(void*)((struct _tuple6**)Cyc__genarr_203
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_184;struct _tuple14{
unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};static struct _tuple6 Cyc__gentuple_196={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_197={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_192};static struct _tuple6 Cyc__gentuple_198={
offsetof(struct _tuple14,f3),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*
Cyc__genarr_199[3]={& Cyc__gentuple_196,& Cyc__gentuple_197,& Cyc__gentuple_198};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_184={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_199),(void*)((struct _tuple6**)Cyc__genarr_199),(
void*)((struct _tuple6**)Cyc__genarr_199 + 3)}};static struct _tuple7*Cyc__genarr_183[
0]={};static unsigned char _tmp186[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_208={
0,{_tmp186,_tmp186,_tmp186 + 6},(void*)& Cyc__genrep_204};static unsigned char
_tmp187[11]="Unknown_kb";static struct _tuple5 Cyc__gentuple_209={1,{_tmp187,
_tmp187,_tmp187 + 11},(void*)& Cyc__genrep_200};static unsigned char _tmp188[8]="Less_kb";
static struct _tuple5 Cyc__gentuple_210={2,{_tmp188,_tmp188,_tmp188 + 8},(void*)& Cyc__genrep_184};
static struct _tuple5*Cyc__genarr_211[3]={& Cyc__gentuple_208,& Cyc__gentuple_209,&
Cyc__gentuple_210};static unsigned char _tmp18A[10]="KindBound";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kindbound_t_rep={5,{_tmp18A,_tmp18A,_tmp18A + 10},{(void*)((struct
_tuple7**)Cyc__genarr_183),(void*)((struct _tuple7**)Cyc__genarr_183),(void*)((
struct _tuple7**)Cyc__genarr_183 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_211),(
void*)((struct _tuple5**)Cyc__genarr_211),(void*)((struct _tuple5**)Cyc__genarr_211
+ 3)}};static unsigned char _tmp18B[5]="Tvar";static struct _tagged_arr Cyc__genname_217={
_tmp18B,_tmp18B,_tmp18B + 5};static unsigned char _tmp18C[5]="name";static struct
_tuple5 Cyc__gentuple_213={offsetof(struct Cyc_Absyn_Tvar,name),{_tmp18C,_tmp18C,
_tmp18C + 5},(void*)& Cyc__genrep_12};static unsigned char _tmp18D[9]="identity";
static struct _tuple5 Cyc__gentuple_214={offsetof(struct Cyc_Absyn_Tvar,identity),{
_tmp18D,_tmp18D,_tmp18D + 9},(void*)& Cyc__genrep_212};static unsigned char _tmp18E[
5]="kind";static struct _tuple5 Cyc__gentuple_215={offsetof(struct Cyc_Absyn_Tvar,kind),{
_tmp18E,_tmp18E,_tmp18E + 5},(void*)& Cyc_Absyn_kindbound_t_rep};static struct
_tuple5*Cyc__genarr_216[3]={& Cyc__gentuple_213,& Cyc__gentuple_214,& Cyc__gentuple_215};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={3,(struct _tagged_arr*)&
Cyc__genname_217,sizeof(struct Cyc_Absyn_Tvar),{(void*)((struct _tuple5**)Cyc__genarr_216),(
void*)((struct _tuple5**)Cyc__genarr_216),(void*)((struct _tuple5**)Cyc__genarr_216
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_182={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tvar_rep)};static unsigned char _tmp191[5]="List";static struct
_tagged_arr Cyc__genname_240={_tmp191,_tmp191,_tmp191 + 5};static unsigned char
_tmp192[3]="hd";static struct _tuple5 Cyc__gentuple_237={offsetof(struct Cyc_List_List,hd),{
_tmp192,_tmp192,_tmp192 + 3},(void*)& Cyc__genrep_182};static unsigned char _tmp193[
3]="tl";static struct _tuple5 Cyc__gentuple_238={offsetof(struct Cyc_List_List,tl),{
_tmp193,_tmp193,_tmp193 + 3},(void*)& Cyc__genrep_236};static struct _tuple5*Cyc__genarr_239[
2]={& Cyc__gentuple_237,& Cyc__gentuple_238};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_240,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_239),(void*)((struct _tuple5**)Cyc__genarr_239),(void*)((
struct _tuple5**)Cyc__genarr_239 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_236={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static
unsigned char _tmp196[4]="Opt";static struct _tagged_arr Cyc__genname_360={_tmp196,
_tmp196,_tmp196 + 4};static unsigned char _tmp197[2]="v";static struct _tuple5 Cyc__gentuple_358={
offsetof(struct Cyc_Core_Opt,v),{_tmp197,_tmp197,_tmp197 + 2},(void*)& Cyc__genrep_236};
static struct _tuple5*Cyc__genarr_359[1]={& Cyc__gentuple_358};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_360,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_359),(
void*)((struct _tuple5**)Cyc__genarr_359),(void*)((struct _tuple5**)Cyc__genarr_359
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_357={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};struct _tuple15{
unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;struct Cyc_Core_Opt*
f5;};static struct _tuple6 Cyc__gentuple_1074={offsetof(struct _tuple15,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1075={offsetof(struct _tuple15,f2),(
void*)& Cyc__genrep_1070};static struct _tuple6 Cyc__gentuple_1076={offsetof(struct
_tuple15,f3),(void*)& Cyc__genrep_61};static struct _tuple6 Cyc__gentuple_1077={
offsetof(struct _tuple15,f4),(void*)((void*)& Cyc__genrep_102)};static struct
_tuple6 Cyc__gentuple_1078={offsetof(struct _tuple15,f5),(void*)& Cyc__genrep_357};
static struct _tuple6*Cyc__genarr_1079[5]={& Cyc__gentuple_1074,& Cyc__gentuple_1075,&
Cyc__gentuple_1076,& Cyc__gentuple_1077,& Cyc__gentuple_1078};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1069={4,sizeof(struct _tuple15),{(void*)((struct _tuple6**)Cyc__genarr_1079),(
void*)((struct _tuple6**)Cyc__genarr_1079),(void*)((struct _tuple6**)Cyc__genarr_1079
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1065;struct _tuple16{
unsigned int f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1066={
offsetof(struct _tuple16,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1067={
offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_182};static struct _tuple6*Cyc__genarr_1068[
2]={& Cyc__gentuple_1066,& Cyc__gentuple_1067};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1065={4,sizeof(struct _tuple16),{(void*)((struct _tuple6**)Cyc__genarr_1068),(
void*)((struct _tuple6**)Cyc__genarr_1068),(void*)((struct _tuple6**)Cyc__genarr_1068
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1039;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1046;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static unsigned char _tmp19C[18]="UnknownTunionInfo";
static struct _tagged_arr Cyc__genname_1050={_tmp19C,_tmp19C,_tmp19C + 18};static
unsigned char _tmp19D[5]="name";static struct _tuple5 Cyc__gentuple_1047={offsetof(
struct Cyc_Absyn_UnknownTunionInfo,name),{_tmp19D,_tmp19D,_tmp19D + 5},(void*)& Cyc__genrep_10};
static unsigned char _tmp19E[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_1048={
offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),{_tmp19E,_tmp19E,_tmp19E + 
11},(void*)((void*)& Cyc__genrep_102)};static struct _tuple5*Cyc__genarr_1049[2]={&
Cyc__gentuple_1047,& Cyc__gentuple_1048};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionInfo_rep={
3,(struct _tagged_arr*)& Cyc__genname_1050,sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1049),(void*)((struct _tuple5**)Cyc__genarr_1049),(
void*)((struct _tuple5**)Cyc__genarr_1049 + 2)}};struct _tuple17{unsigned int f1;
struct Cyc_Absyn_UnknownTunionInfo f2;};static struct _tuple6 Cyc__gentuple_1051={
offsetof(struct _tuple17,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1052={
offsetof(struct _tuple17,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple6*Cyc__genarr_1053[2]={& Cyc__gentuple_1051,& Cyc__gentuple_1052};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1046={4,sizeof(struct _tuple17),{(
void*)((struct _tuple6**)Cyc__genarr_1053),(void*)((struct _tuple6**)Cyc__genarr_1053),(
void*)((struct _tuple6**)Cyc__genarr_1053 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1041;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1042;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_289;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_290;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_291;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_271;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_272;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_273;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_274;static struct
_tuple6 Cyc__gentuple_275={offsetof(struct _tuple3,f1),(void*)& Cyc__genrep_132};
static struct _tuple6 Cyc__gentuple_276={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_277[2]={& Cyc__gentuple_275,&
Cyc__gentuple_276};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_274={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_277),(void*)((struct
_tuple6**)Cyc__genarr_277),(void*)((struct _tuple6**)Cyc__genarr_277 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_273={1,1,(void*)((void*)& Cyc__genrep_274)};
static unsigned char _tmp1A3[5]="List";static struct _tagged_arr Cyc__genname_281={
_tmp1A3,_tmp1A3,_tmp1A3 + 5};static unsigned char _tmp1A4[3]="hd";static struct
_tuple5 Cyc__gentuple_278={offsetof(struct Cyc_List_List,hd),{_tmp1A4,_tmp1A4,
_tmp1A4 + 3},(void*)& Cyc__genrep_273};static unsigned char _tmp1A5[3]="tl";static
struct _tuple5 Cyc__gentuple_279={offsetof(struct Cyc_List_List,tl),{_tmp1A5,
_tmp1A5,_tmp1A5 + 3},(void*)& Cyc__genrep_272};static struct _tuple5*Cyc__genarr_280[
2]={& Cyc__gentuple_278,& Cyc__gentuple_279};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_281,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_280),(void*)((struct _tuple5**)Cyc__genarr_280),(void*)((
struct _tuple5**)Cyc__genarr_280 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_272={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_2={1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};static
unsigned char _tmp1A9[12]="Tunionfield";static struct _tagged_arr Cyc__genname_288={
_tmp1A9,_tmp1A9,_tmp1A9 + 12};static unsigned char _tmp1AA[5]="name";static struct
_tuple5 Cyc__gentuple_282={offsetof(struct Cyc_Absyn_Tunionfield,name),{_tmp1AA,
_tmp1AA,_tmp1AA + 5},(void*)& Cyc__genrep_10};static unsigned char _tmp1AB[4]="tvs";
static struct _tuple5 Cyc__gentuple_283={offsetof(struct Cyc_Absyn_Tunionfield,tvs),{
_tmp1AB,_tmp1AB,_tmp1AB + 4},(void*)& Cyc__genrep_236};static unsigned char _tmp1AC[
5]="typs";static struct _tuple5 Cyc__gentuple_284={offsetof(struct Cyc_Absyn_Tunionfield,typs),{
_tmp1AC,_tmp1AC,_tmp1AC + 5},(void*)& Cyc__genrep_272};static unsigned char _tmp1AD[
4]="loc";static struct _tuple5 Cyc__gentuple_285={offsetof(struct Cyc_Absyn_Tunionfield,loc),{
_tmp1AD,_tmp1AD,_tmp1AD + 4},(void*)& Cyc__genrep_2};static unsigned char _tmp1AE[3]="sc";
static struct _tuple5 Cyc__gentuple_286={offsetof(struct Cyc_Absyn_Tunionfield,sc),{
_tmp1AE,_tmp1AE,_tmp1AE + 3},(void*)& Cyc_Absyn_scope_t_rep};static struct _tuple5*
Cyc__genarr_287[5]={& Cyc__gentuple_282,& Cyc__gentuple_283,& Cyc__gentuple_284,&
Cyc__gentuple_285,& Cyc__gentuple_286};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_288,sizeof(struct Cyc_Absyn_Tunionfield),{(
void*)((struct _tuple5**)Cyc__genarr_287),(void*)((struct _tuple5**)Cyc__genarr_287),(
void*)((struct _tuple5**)Cyc__genarr_287 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_271={1,1,(void*)((void*)& Cyc_struct_Absyn_Tunionfield_rep)};static
unsigned char _tmp1B1[5]="List";static struct _tagged_arr Cyc__genname_295={_tmp1B1,
_tmp1B1,_tmp1B1 + 5};static unsigned char _tmp1B2[3]="hd";static struct _tuple5 Cyc__gentuple_292={
offsetof(struct Cyc_List_List,hd),{_tmp1B2,_tmp1B2,_tmp1B2 + 3},(void*)& Cyc__genrep_271};
static unsigned char _tmp1B3[3]="tl";static struct _tuple5 Cyc__gentuple_293={
offsetof(struct Cyc_List_List,tl),{_tmp1B3,_tmp1B3,_tmp1B3 + 3},(void*)& Cyc__genrep_291};
static struct _tuple5*Cyc__genarr_294[2]={& Cyc__gentuple_292,& Cyc__gentuple_293};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_295,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_294),(void*)((struct _tuple5**)Cyc__genarr_294),(void*)((
struct _tuple5**)Cyc__genarr_294 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_291={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};static
unsigned char _tmp1B6[4]="Opt";static struct _tagged_arr Cyc__genname_298={_tmp1B6,
_tmp1B6,_tmp1B6 + 4};static unsigned char _tmp1B7[2]="v";static struct _tuple5 Cyc__gentuple_296={
offsetof(struct Cyc_Core_Opt,v),{_tmp1B7,_tmp1B7,_tmp1B7 + 2},(void*)& Cyc__genrep_291};
static struct _tuple5*Cyc__genarr_297[1]={& Cyc__gentuple_296};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={3,(struct
_tagged_arr*)& Cyc__genname_298,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_297),(void*)((struct _tuple5**)Cyc__genarr_297),(void*)((
struct _tuple5**)Cyc__genarr_297 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_290={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static unsigned char _tmp1BA[11]="Tuniondecl";static struct _tagged_arr Cyc__genname_305={
_tmp1BA,_tmp1BA,_tmp1BA + 11};static unsigned char _tmp1BB[3]="sc";static struct
_tuple5 Cyc__gentuple_299={offsetof(struct Cyc_Absyn_Tuniondecl,sc),{_tmp1BB,
_tmp1BB,_tmp1BB + 3},(void*)& Cyc_Absyn_scope_t_rep};static unsigned char _tmp1BC[5]="name";
static struct _tuple5 Cyc__gentuple_300={offsetof(struct Cyc_Absyn_Tuniondecl,name),{
_tmp1BC,_tmp1BC,_tmp1BC + 5},(void*)& Cyc__genrep_10};static unsigned char _tmp1BD[4]="tvs";
static struct _tuple5 Cyc__gentuple_301={offsetof(struct Cyc_Absyn_Tuniondecl,tvs),{
_tmp1BD,_tmp1BD,_tmp1BD + 4},(void*)& Cyc__genrep_236};static unsigned char _tmp1BE[
7]="fields";static struct _tuple5 Cyc__gentuple_302={offsetof(struct Cyc_Absyn_Tuniondecl,fields),{
_tmp1BE,_tmp1BE,_tmp1BE + 7},(void*)& Cyc__genrep_290};static unsigned char _tmp1BF[
11]="is_xtunion";static struct _tuple5 Cyc__gentuple_303={offsetof(struct Cyc_Absyn_Tuniondecl,is_xtunion),{
_tmp1BF,_tmp1BF,_tmp1BF + 11},(void*)((void*)& Cyc__genrep_102)};static struct
_tuple5*Cyc__genarr_304[5]={& Cyc__gentuple_299,& Cyc__gentuple_300,& Cyc__gentuple_301,&
Cyc__gentuple_302,& Cyc__gentuple_303};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_305,sizeof(struct Cyc_Absyn_Tuniondecl),{(
void*)((struct _tuple5**)Cyc__genarr_304),(void*)((struct _tuple5**)Cyc__genarr_304),(
void*)((struct _tuple5**)Cyc__genarr_304 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_289={1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1042={1,1,(void*)((void*)& Cyc__genrep_289)};
struct _tuple18{unsigned int f1;struct Cyc_Absyn_Tuniondecl**f2;};static struct
_tuple6 Cyc__gentuple_1043={offsetof(struct _tuple18,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1044={offsetof(struct _tuple18,f2),(void*)& Cyc__genrep_1042};
static struct _tuple6*Cyc__genarr_1045[2]={& Cyc__gentuple_1043,& Cyc__gentuple_1044};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1041={4,sizeof(struct _tuple18),{(
void*)((struct _tuple6**)Cyc__genarr_1045),(void*)((struct _tuple6**)Cyc__genarr_1045),(
void*)((struct _tuple6**)Cyc__genarr_1045 + 2)}};static struct _tuple7*Cyc__genarr_1040[
0]={};static unsigned char _tmp1C4[14]="UnknownTunion";static struct _tuple5 Cyc__gentuple_1054={
0,{_tmp1C4,_tmp1C4,_tmp1C4 + 14},(void*)& Cyc__genrep_1046};static unsigned char
_tmp1C5[12]="KnownTunion";static struct _tuple5 Cyc__gentuple_1055={1,{_tmp1C5,
_tmp1C5,_tmp1C5 + 12},(void*)& Cyc__genrep_1041};static struct _tuple5*Cyc__genarr_1056[
2]={& Cyc__gentuple_1054,& Cyc__gentuple_1055};static unsigned char _tmp1C7[12]="TunionInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep={5,{_tmp1C7,
_tmp1C7,_tmp1C7 + 12},{(void*)((struct _tuple7**)Cyc__genarr_1040),(void*)((struct
_tuple7**)Cyc__genarr_1040),(void*)((struct _tuple7**)Cyc__genarr_1040 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_1056),(void*)((struct _tuple5**)Cyc__genarr_1056),(
void*)((struct _tuple5**)Cyc__genarr_1056 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_52;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep;
static unsigned char _tmp1C8[5]="List";static struct _tagged_arr Cyc__genname_56={
_tmp1C8,_tmp1C8,_tmp1C8 + 5};static unsigned char _tmp1C9[3]="hd";static struct
_tuple5 Cyc__gentuple_53={offsetof(struct Cyc_List_List,hd),{_tmp1C9,_tmp1C9,
_tmp1C9 + 3},(void*)((void*)& Cyc_Absyn_type_t_rep)};static unsigned char _tmp1CA[3]="tl";
static struct _tuple5 Cyc__gentuple_54={offsetof(struct Cyc_List_List,tl),{_tmp1CA,
_tmp1CA,_tmp1CA + 3},(void*)& Cyc__genrep_52};static struct _tuple5*Cyc__genarr_55[2]={&
Cyc__gentuple_53,& Cyc__gentuple_54};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_56,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_55),(void*)((struct _tuple5**)Cyc__genarr_55),(void*)((
struct _tuple5**)Cyc__genarr_55 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_52={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};static
unsigned char _tmp1CD[11]="TunionInfo";static struct _tagged_arr Cyc__genname_1061={
_tmp1CD,_tmp1CD,_tmp1CD + 11};static unsigned char _tmp1CE[12]="tunion_info";static
struct _tuple5 Cyc__gentuple_1057={offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),{
_tmp1CE,_tmp1CE,_tmp1CE + 12},(void*)& Cyc_tunion_Absyn_TunionInfoU_rep};static
unsigned char _tmp1CF[6]="targs";static struct _tuple5 Cyc__gentuple_1058={offsetof(
struct Cyc_Absyn_TunionInfo,targs),{_tmp1CF,_tmp1CF,_tmp1CF + 6},(void*)& Cyc__genrep_52};
static unsigned char _tmp1D0[4]="rgn";static struct _tuple5 Cyc__gentuple_1059={
offsetof(struct Cyc_Absyn_TunionInfo,rgn),{_tmp1D0,_tmp1D0,_tmp1D0 + 4},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static struct _tuple5*Cyc__genarr_1060[3]={& Cyc__gentuple_1057,&
Cyc__gentuple_1058,& Cyc__gentuple_1059};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_1061,sizeof(struct Cyc_Absyn_TunionInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1060),(void*)((struct _tuple5**)Cyc__genarr_1060),(
void*)((struct _tuple5**)Cyc__genarr_1060 + 3)}};struct _tuple19{unsigned int f1;
struct Cyc_Absyn_TunionInfo f2;};static struct _tuple6 Cyc__gentuple_1062={offsetof(
struct _tuple19,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1063={
offsetof(struct _tuple19,f2),(void*)& Cyc_Absyn_tunion_info_t_rep};static struct
_tuple6*Cyc__genarr_1064[2]={& Cyc__gentuple_1062,& Cyc__gentuple_1063};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1039={4,sizeof(struct _tuple19),{(void*)((
struct _tuple6**)Cyc__genarr_1064),(void*)((struct _tuple6**)Cyc__genarr_1064),(
void*)((struct _tuple6**)Cyc__genarr_1064 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1013;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1020;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static unsigned char _tmp1D3[23]="UnknownTunionFieldInfo";
static struct _tagged_arr Cyc__genname_1025={_tmp1D3,_tmp1D3,_tmp1D3 + 23};static
unsigned char _tmp1D4[12]="tunion_name";static struct _tuple5 Cyc__gentuple_1021={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),{_tmp1D4,_tmp1D4,
_tmp1D4 + 12},(void*)& Cyc__genrep_10};static unsigned char _tmp1D5[11]="field_name";
static struct _tuple5 Cyc__gentuple_1022={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),{
_tmp1D5,_tmp1D5,_tmp1D5 + 11},(void*)& Cyc__genrep_10};static unsigned char _tmp1D6[
11]="is_xtunion";static struct _tuple5 Cyc__gentuple_1023={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),{
_tmp1D6,_tmp1D6,_tmp1D6 + 11},(void*)((void*)& Cyc__genrep_102)};static struct
_tuple5*Cyc__genarr_1024[3]={& Cyc__gentuple_1021,& Cyc__gentuple_1022,& Cyc__gentuple_1023};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={3,(
struct _tagged_arr*)& Cyc__genname_1025,sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1024),(void*)((struct _tuple5**)Cyc__genarr_1024),(
void*)((struct _tuple5**)Cyc__genarr_1024 + 3)}};struct _tuple20{unsigned int f1;
struct Cyc_Absyn_UnknownTunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1026={
offsetof(struct _tuple20,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1027={
offsetof(struct _tuple20,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1028[2]={& Cyc__gentuple_1026,& Cyc__gentuple_1027};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1020={4,sizeof(struct _tuple20),{(
void*)((struct _tuple6**)Cyc__genarr_1028),(void*)((struct _tuple6**)Cyc__genarr_1028),(
void*)((struct _tuple6**)Cyc__genarr_1028 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1015;struct _tuple21{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;};static struct _tuple6 Cyc__gentuple_1016={
offsetof(struct _tuple21,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1017={
offsetof(struct _tuple21,f2),(void*)((void*)& Cyc__genrep_289)};static struct
_tuple6 Cyc__gentuple_1018={offsetof(struct _tuple21,f3),(void*)& Cyc__genrep_271};
static struct _tuple6*Cyc__genarr_1019[3]={& Cyc__gentuple_1016,& Cyc__gentuple_1017,&
Cyc__gentuple_1018};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1015={4,
sizeof(struct _tuple21),{(void*)((struct _tuple6**)Cyc__genarr_1019),(void*)((
struct _tuple6**)Cyc__genarr_1019),(void*)((struct _tuple6**)Cyc__genarr_1019 + 3)}};
static struct _tuple7*Cyc__genarr_1014[0]={};static unsigned char _tmp1DA[19]="UnknownTunionfield";
static struct _tuple5 Cyc__gentuple_1029={0,{_tmp1DA,_tmp1DA,_tmp1DA + 19},(void*)&
Cyc__genrep_1020};static unsigned char _tmp1DB[17]="KnownTunionfield";static struct
_tuple5 Cyc__gentuple_1030={1,{_tmp1DB,_tmp1DB,_tmp1DB + 17},(void*)& Cyc__genrep_1015};
static struct _tuple5*Cyc__genarr_1031[2]={& Cyc__gentuple_1029,& Cyc__gentuple_1030};
static unsigned char _tmp1DD[17]="TunionFieldInfoU";struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_TunionFieldInfoU_rep={5,{_tmp1DD,_tmp1DD,_tmp1DD + 17},{(void*)((
struct _tuple7**)Cyc__genarr_1014),(void*)((struct _tuple7**)Cyc__genarr_1014),(
void*)((struct _tuple7**)Cyc__genarr_1014 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_1031),(
void*)((struct _tuple5**)Cyc__genarr_1031),(void*)((struct _tuple5**)Cyc__genarr_1031
+ 2)}};static unsigned char _tmp1DE[16]="TunionFieldInfo";static struct _tagged_arr
Cyc__genname_1035={_tmp1DE,_tmp1DE,_tmp1DE + 16};static unsigned char _tmp1DF[11]="field_info";
static struct _tuple5 Cyc__gentuple_1032={offsetof(struct Cyc_Absyn_TunionFieldInfo,field_info),{
_tmp1DF,_tmp1DF,_tmp1DF + 11},(void*)& Cyc_tunion_Absyn_TunionFieldInfoU_rep};
static unsigned char _tmp1E0[6]="targs";static struct _tuple5 Cyc__gentuple_1033={
offsetof(struct Cyc_Absyn_TunionFieldInfo,targs),{_tmp1E0,_tmp1E0,_tmp1E0 + 6},(
void*)& Cyc__genrep_52};static struct _tuple5*Cyc__genarr_1034[2]={& Cyc__gentuple_1032,&
Cyc__gentuple_1033};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_1035,sizeof(struct Cyc_Absyn_TunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1034),(void*)((struct _tuple5**)Cyc__genarr_1034),(
void*)((struct _tuple5**)Cyc__genarr_1034 + 2)}};struct _tuple22{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1036={
offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1037={
offsetof(struct _tuple22,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1038[2]={& Cyc__gentuple_1036,& Cyc__gentuple_1037};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1013={4,sizeof(struct _tuple22),{(
void*)((struct _tuple6**)Cyc__genarr_1038),(void*)((struct _tuple6**)Cyc__genarr_1038),(
void*)((struct _tuple6**)Cyc__genarr_1038 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_977;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_999;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_985;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_bounds_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_76;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_77;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_836;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_851;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep;static unsigned char _tmp1E3[7]="Signed";
static struct _tuple7 Cyc__gentuple_411={0,{_tmp1E3,_tmp1E3,_tmp1E3 + 7}};static
unsigned char _tmp1E4[9]="Unsigned";static struct _tuple7 Cyc__gentuple_412={1,{
_tmp1E4,_tmp1E4,_tmp1E4 + 9}};static struct _tuple7*Cyc__genarr_413[2]={& Cyc__gentuple_411,&
Cyc__gentuple_412};static struct _tuple5*Cyc__genarr_414[0]={};static unsigned char
_tmp1E6[5]="Sign";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep={5,{
_tmp1E6,_tmp1E6,_tmp1E6 + 5},{(void*)((struct _tuple7**)Cyc__genarr_413),(void*)((
struct _tuple7**)Cyc__genarr_413),(void*)((struct _tuple7**)Cyc__genarr_413 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_414),(void*)((struct _tuple5**)Cyc__genarr_414),(
void*)((struct _tuple5**)Cyc__genarr_414 + 0)}};struct _tuple23{unsigned int f1;void*
f2;unsigned char f3;};static struct _tuple6 Cyc__gentuple_852={offsetof(struct
_tuple23,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_853={
offsetof(struct _tuple23,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_854={
offsetof(struct _tuple23,f3),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_855[3]={& Cyc__gentuple_852,& Cyc__gentuple_853,& Cyc__gentuple_854};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_851={4,sizeof(struct _tuple23),{(
void*)((struct _tuple6**)Cyc__genarr_855),(void*)((struct _tuple6**)Cyc__genarr_855),(
void*)((struct _tuple6**)Cyc__genarr_855 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_845;static struct Cyc_Typerep_Int_struct Cyc__genrep_846={0,1,16};
struct _tuple24{unsigned int f1;void*f2;short f3;};static struct _tuple6 Cyc__gentuple_847={
offsetof(struct _tuple24,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_848={
offsetof(struct _tuple24,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_849={
offsetof(struct _tuple24,f3),(void*)& Cyc__genrep_846};static struct _tuple6*Cyc__genarr_850[
3]={& Cyc__gentuple_847,& Cyc__gentuple_848,& Cyc__gentuple_849};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_845={4,sizeof(struct _tuple24),{(void*)((struct _tuple6**)Cyc__genarr_850),(
void*)((struct _tuple6**)Cyc__genarr_850),(void*)((struct _tuple6**)Cyc__genarr_850
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_410;struct _tuple25{
unsigned int f1;void*f2;int f3;};static struct _tuple6 Cyc__gentuple_415={offsetof(
struct _tuple25,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_416={
offsetof(struct _tuple25,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_417={
offsetof(struct _tuple25,f3),(void*)((void*)& Cyc__genrep_102)};static struct
_tuple6*Cyc__genarr_418[3]={& Cyc__gentuple_415,& Cyc__gentuple_416,& Cyc__gentuple_417};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_410={4,sizeof(struct _tuple25),{(
void*)((struct _tuple6**)Cyc__genarr_418),(void*)((struct _tuple6**)Cyc__genarr_418),(
void*)((struct _tuple6**)Cyc__genarr_418 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_839;static struct Cyc_Typerep_Int_struct Cyc__genrep_840={0,1,64};
struct _tuple26{unsigned int f1;void*f2;long long f3;};static struct _tuple6 Cyc__gentuple_841={
offsetof(struct _tuple26,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_842={
offsetof(struct _tuple26,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_843={
offsetof(struct _tuple26,f3),(void*)& Cyc__genrep_840};static struct _tuple6*Cyc__genarr_844[
3]={& Cyc__gentuple_841,& Cyc__gentuple_842,& Cyc__gentuple_843};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_839={4,sizeof(struct _tuple26),{(void*)((struct _tuple6**)Cyc__genarr_844),(
void*)((struct _tuple6**)Cyc__genarr_844),(void*)((struct _tuple6**)Cyc__genarr_844
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_112;static struct _tuple6
Cyc__gentuple_113={offsetof(struct _tuple7,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_114={offsetof(struct _tuple7,f2),(void*)((void*)& Cyc__genrep_13)};
static struct _tuple6*Cyc__genarr_115[2]={& Cyc__gentuple_113,& Cyc__gentuple_114};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_112={4,sizeof(struct _tuple7),{(
void*)((struct _tuple6**)Cyc__genarr_115),(void*)((struct _tuple6**)Cyc__genarr_115),(
void*)((struct _tuple6**)Cyc__genarr_115 + 2)}};static unsigned char _tmp1EE[7]="Null_c";
static struct _tuple7 Cyc__gentuple_837={0,{_tmp1EE,_tmp1EE,_tmp1EE + 7}};static
struct _tuple7*Cyc__genarr_838[1]={& Cyc__gentuple_837};static unsigned char _tmp1EF[
7]="Char_c";static struct _tuple5 Cyc__gentuple_856={0,{_tmp1EF,_tmp1EF,_tmp1EF + 7},(
void*)& Cyc__genrep_851};static unsigned char _tmp1F0[8]="Short_c";static struct
_tuple5 Cyc__gentuple_857={1,{_tmp1F0,_tmp1F0,_tmp1F0 + 8},(void*)& Cyc__genrep_845};
static unsigned char _tmp1F1[6]="Int_c";static struct _tuple5 Cyc__gentuple_858={2,{
_tmp1F1,_tmp1F1,_tmp1F1 + 6},(void*)& Cyc__genrep_410};static unsigned char _tmp1F2[
11]="LongLong_c";static struct _tuple5 Cyc__gentuple_859={3,{_tmp1F2,_tmp1F2,
_tmp1F2 + 11},(void*)& Cyc__genrep_839};static unsigned char _tmp1F3[8]="Float_c";
static struct _tuple5 Cyc__gentuple_860={4,{_tmp1F3,_tmp1F3,_tmp1F3 + 8},(void*)& Cyc__genrep_112};
static unsigned char _tmp1F4[9]="String_c";static struct _tuple5 Cyc__gentuple_861={5,{
_tmp1F4,_tmp1F4,_tmp1F4 + 9},(void*)& Cyc__genrep_112};static struct _tuple5*Cyc__genarr_862[
6]={& Cyc__gentuple_856,& Cyc__gentuple_857,& Cyc__gentuple_858,& Cyc__gentuple_859,&
Cyc__gentuple_860,& Cyc__gentuple_861};static unsigned char _tmp1F6[5]="Cnst";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep={5,{_tmp1F6,_tmp1F6,_tmp1F6 + 
5},{(void*)((struct _tuple7**)Cyc__genarr_838),(void*)((struct _tuple7**)Cyc__genarr_838),(
void*)((struct _tuple7**)Cyc__genarr_838 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_862),(
void*)((struct _tuple5**)Cyc__genarr_862),(void*)((struct _tuple5**)Cyc__genarr_862
+ 6)}};static struct _tuple6 Cyc__gentuple_863={offsetof(struct _tuple6,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_864={offsetof(struct _tuple6,f2),(
void*)& Cyc_Absyn_cnst_t_rep};static struct _tuple6*Cyc__genarr_865[2]={& Cyc__gentuple_863,&
Cyc__gentuple_864};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_836={4,
sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_865),(void*)((struct
_tuple6**)Cyc__genarr_865),(void*)((struct _tuple6**)Cyc__genarr_865 + 2)}};extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_823;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_81;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_82;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Fndecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_590;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_591;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_592;struct _tuple27{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*
f3;};static struct _tuple6 Cyc__gentuple_593={offsetof(struct _tuple27,f1),(void*)&
Cyc__genrep_12};static struct _tuple6 Cyc__gentuple_594={offsetof(struct _tuple27,f2),(
void*)& Cyc__genrep_132};static struct _tuple6 Cyc__gentuple_595={offsetof(struct
_tuple27,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_596[
3]={& Cyc__gentuple_593,& Cyc__gentuple_594,& Cyc__gentuple_595};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_592={4,sizeof(struct _tuple27),{(void*)((struct _tuple6**)Cyc__genarr_596),(
void*)((struct _tuple6**)Cyc__genarr_596),(void*)((struct _tuple6**)Cyc__genarr_596
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_591={1,1,(void*)((void*)&
Cyc__genrep_592)};static unsigned char _tmp1FA[5]="List";static struct _tagged_arr
Cyc__genname_600={_tmp1FA,_tmp1FA,_tmp1FA + 5};static unsigned char _tmp1FB[3]="hd";
static struct _tuple5 Cyc__gentuple_597={offsetof(struct Cyc_List_List,hd),{_tmp1FB,
_tmp1FB,_tmp1FB + 3},(void*)& Cyc__genrep_591};static unsigned char _tmp1FC[3]="tl";
static struct _tuple5 Cyc__gentuple_598={offsetof(struct Cyc_List_List,tl),{_tmp1FC,
_tmp1FC,_tmp1FC + 3},(void*)& Cyc__genrep_590};static struct _tuple5*Cyc__genarr_599[
2]={& Cyc__gentuple_597,& Cyc__gentuple_598};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_600,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_599),(void*)((struct _tuple5**)Cyc__genarr_599),(void*)((
struct _tuple5**)Cyc__genarr_599 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_590={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_579;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_580;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
unsigned char _tmp1FF[4]="Opt";static struct _tagged_arr Cyc__genname_583={_tmp1FF,
_tmp1FF,_tmp1FF + 4};static unsigned char _tmp200[2]="v";static struct _tuple5 Cyc__gentuple_581={
offsetof(struct Cyc_Core_Opt,v),{_tmp200,_tmp200,_tmp200 + 2},(void*)& Cyc__genrep_12};
static struct _tuple5*Cyc__genarr_582[1]={& Cyc__gentuple_581};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_var_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_583,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_582),(void*)((
struct _tuple5**)Cyc__genarr_582),(void*)((struct _tuple5**)Cyc__genarr_582 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_580={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static unsigned char _tmp203[11]="VarargInfo";static struct _tagged_arr Cyc__genname_589={
_tmp203,_tmp203,_tmp203 + 11};static unsigned char _tmp204[5]="name";static struct
_tuple5 Cyc__gentuple_584={offsetof(struct Cyc_Absyn_VarargInfo,name),{_tmp204,
_tmp204,_tmp204 + 5},(void*)& Cyc__genrep_580};static unsigned char _tmp205[3]="tq";
static struct _tuple5 Cyc__gentuple_585={offsetof(struct Cyc_Absyn_VarargInfo,tq),{
_tmp205,_tmp205,_tmp205 + 3},(void*)& Cyc__genrep_132};static unsigned char _tmp206[
5]="type";static struct _tuple5 Cyc__gentuple_586={offsetof(struct Cyc_Absyn_VarargInfo,type),{
_tmp206,_tmp206,_tmp206 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static
unsigned char _tmp207[7]="inject";static struct _tuple5 Cyc__gentuple_587={offsetof(
struct Cyc_Absyn_VarargInfo,inject),{_tmp207,_tmp207,_tmp207 + 7},(void*)((void*)&
Cyc__genrep_102)};static struct _tuple5*Cyc__genarr_588[4]={& Cyc__gentuple_584,&
Cyc__gentuple_585,& Cyc__gentuple_586,& Cyc__gentuple_587};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_589,sizeof(
struct Cyc_Absyn_VarargInfo),{(void*)((struct _tuple5**)Cyc__genarr_588),(void*)((
struct _tuple5**)Cyc__genarr_588),(void*)((struct _tuple5**)Cyc__genarr_588 + 4)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_579={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_569;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_570;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_571;static struct
_tuple6 Cyc__gentuple_572={offsetof(struct _tuple9,f1),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6 Cyc__gentuple_573={offsetof(struct _tuple9,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_574[2]={& Cyc__gentuple_572,&
Cyc__gentuple_573};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_571={4,
sizeof(struct _tuple9),{(void*)((struct _tuple6**)Cyc__genarr_574),(void*)((struct
_tuple6**)Cyc__genarr_574),(void*)((struct _tuple6**)Cyc__genarr_574 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_570={1,1,(void*)((void*)& Cyc__genrep_571)};
static unsigned char _tmp20C[5]="List";static struct _tagged_arr Cyc__genname_578={
_tmp20C,_tmp20C,_tmp20C + 5};static unsigned char _tmp20D[3]="hd";static struct
_tuple5 Cyc__gentuple_575={offsetof(struct Cyc_List_List,hd),{_tmp20D,_tmp20D,
_tmp20D + 3},(void*)& Cyc__genrep_570};static unsigned char _tmp20E[3]="tl";static
struct _tuple5 Cyc__gentuple_576={offsetof(struct Cyc_List_List,tl),{_tmp20E,
_tmp20E,_tmp20E + 3},(void*)& Cyc__genrep_569};static struct _tuple5*Cyc__genarr_577[
2]={& Cyc__gentuple_575,& Cyc__gentuple_576};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_578,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_577),(void*)((struct _tuple5**)Cyc__genarr_577),(void*)((
struct _tuple5**)Cyc__genarr_577 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_569={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_536;struct _tuple28{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_537={
offsetof(struct _tuple28,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_538={
offsetof(struct _tuple28,f2),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_539={
offsetof(struct _tuple28,f3),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_540[
3]={& Cyc__gentuple_537,& Cyc__gentuple_538,& Cyc__gentuple_539};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_536={4,sizeof(struct _tuple28),{(void*)((struct _tuple6**)Cyc__genarr_540),(
void*)((struct _tuple6**)Cyc__genarr_540),(void*)((struct _tuple6**)Cyc__genarr_540
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_532;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_73;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_73={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple29{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_533={offsetof(struct _tuple29,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_534={offsetof(struct _tuple29,f2),(
void*)& Cyc__genrep_73};static struct _tuple6*Cyc__genarr_535[2]={& Cyc__gentuple_533,&
Cyc__gentuple_534};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_532={4,
sizeof(struct _tuple29),{(void*)((struct _tuple6**)Cyc__genarr_535),(void*)((
struct _tuple6**)Cyc__genarr_535),(void*)((struct _tuple6**)Cyc__genarr_535 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_526;struct _tuple30{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*f3;struct Cyc_Absyn_Stmt*f4;};
static struct _tuple6 Cyc__gentuple_527={offsetof(struct _tuple30,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_528={offsetof(struct _tuple30,f2),(void*)& Cyc__genrep_77};
static struct _tuple6 Cyc__gentuple_529={offsetof(struct _tuple30,f3),(void*)& Cyc__genrep_159};
static struct _tuple6 Cyc__gentuple_530={offsetof(struct _tuple30,f4),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_531[4]={& Cyc__gentuple_527,& Cyc__gentuple_528,&
Cyc__gentuple_529,& Cyc__gentuple_530};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_526={
4,sizeof(struct _tuple30),{(void*)((struct _tuple6**)Cyc__genarr_531),(void*)((
struct _tuple6**)Cyc__genarr_531),(void*)((struct _tuple6**)Cyc__genarr_531 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_521;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_168;static struct _tuple6 Cyc__gentuple_169={offsetof(struct _tuple2,f1),(
void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_170={offsetof(struct
_tuple2,f2),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_171[2]={&
Cyc__gentuple_169,& Cyc__gentuple_170};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_168={
4,sizeof(struct _tuple2),{(void*)((struct _tuple6**)Cyc__genarr_171),(void*)((
struct _tuple6**)Cyc__genarr_171),(void*)((struct _tuple6**)Cyc__genarr_171 + 2)}};
struct _tuple31{unsigned int f1;struct _tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static
struct _tuple6 Cyc__gentuple_522={offsetof(struct _tuple31,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_523={offsetof(struct _tuple31,f2),(void*)& Cyc__genrep_168};
static struct _tuple6 Cyc__gentuple_524={offsetof(struct _tuple31,f3),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_525[3]={& Cyc__gentuple_522,& Cyc__gentuple_523,&
Cyc__gentuple_524};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_521={4,
sizeof(struct _tuple31),{(void*)((struct _tuple6**)Cyc__genarr_525),(void*)((
struct _tuple6**)Cyc__genarr_525),(void*)((struct _tuple6**)Cyc__genarr_525 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_517;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_512;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_512={1,1,(void*)((
void*)& Cyc_struct_Absyn_Stmt_rep)};struct _tuple32{unsigned int f1;struct Cyc_Absyn_Stmt*
f2;};static struct _tuple6 Cyc__gentuple_518={offsetof(struct _tuple32,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_519={offsetof(struct _tuple32,f2),(
void*)& Cyc__genrep_512};static struct _tuple6*Cyc__genarr_520[2]={& Cyc__gentuple_518,&
Cyc__gentuple_519};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_517={4,
sizeof(struct _tuple32),{(void*)((struct _tuple6**)Cyc__genarr_520),(void*)((
struct _tuple6**)Cyc__genarr_520),(void*)((struct _tuple6**)Cyc__genarr_520 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_511;struct _tuple33{unsigned int
f1;struct _tagged_arr*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_513={
offsetof(struct _tuple33,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_514={
offsetof(struct _tuple33,f2),(void*)& Cyc__genrep_12};static struct _tuple6 Cyc__gentuple_515={
offsetof(struct _tuple33,f3),(void*)& Cyc__genrep_512};static struct _tuple6*Cyc__genarr_516[
3]={& Cyc__gentuple_513,& Cyc__gentuple_514,& Cyc__gentuple_515};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_511={4,sizeof(struct _tuple33),{(void*)((struct _tuple6**)Cyc__genarr_516),(
void*)((struct _tuple6**)Cyc__genarr_516),(void*)((struct _tuple6**)Cyc__genarr_516
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_504;struct _tuple34{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_505={offsetof(struct _tuple34,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_506={offsetof(struct _tuple34,f2),(
void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_507={offsetof(struct
_tuple34,f3),(void*)& Cyc__genrep_168};static struct _tuple6 Cyc__gentuple_508={
offsetof(struct _tuple34,f4),(void*)& Cyc__genrep_168};static struct _tuple6 Cyc__gentuple_509={
offsetof(struct _tuple34,f5),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_510[
5]={& Cyc__gentuple_505,& Cyc__gentuple_506,& Cyc__gentuple_507,& Cyc__gentuple_508,&
Cyc__gentuple_509};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_504={4,
sizeof(struct _tuple34),{(void*)((struct _tuple6**)Cyc__genarr_510),(void*)((
struct _tuple6**)Cyc__genarr_510),(void*)((struct _tuple6**)Cyc__genarr_510 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_499;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_224;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_225;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Switch_clause_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_226;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_406;
struct _tuple35{unsigned int f1;unsigned char f2;};static struct _tuple6 Cyc__gentuple_407={
offsetof(struct _tuple35,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_408={
offsetof(struct _tuple35,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_409[2]={& Cyc__gentuple_407,& Cyc__gentuple_408};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_406={4,sizeof(struct _tuple35),{(void*)((struct _tuple6**)Cyc__genarr_409),(
void*)((struct _tuple6**)Cyc__genarr_409),(void*)((struct _tuple6**)Cyc__genarr_409
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_402;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_231;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;
static unsigned char _tmp21C[5]="List";static struct _tagged_arr Cyc__genname_235={
_tmp21C,_tmp21C,_tmp21C + 5};static unsigned char _tmp21D[3]="hd";static struct
_tuple5 Cyc__gentuple_232={offsetof(struct Cyc_List_List,hd),{_tmp21D,_tmp21D,
_tmp21D + 3},(void*)& Cyc__genrep_226};static unsigned char _tmp21E[3]="tl";static
struct _tuple5 Cyc__gentuple_233={offsetof(struct Cyc_List_List,tl),{_tmp21E,
_tmp21E,_tmp21E + 3},(void*)& Cyc__genrep_231};static struct _tuple5*Cyc__genarr_234[
2]={& Cyc__gentuple_232,& Cyc__gentuple_233};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_235,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_234),(void*)((struct _tuple5**)Cyc__genarr_234),(void*)((
struct _tuple5**)Cyc__genarr_234 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_231={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};static struct
_tuple6 Cyc__gentuple_403={offsetof(struct _tuple11,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_404={offsetof(struct _tuple11,f2),(void*)& Cyc__genrep_231};
static struct _tuple6*Cyc__genarr_405[2]={& Cyc__gentuple_403,& Cyc__gentuple_404};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_402={4,sizeof(struct _tuple11),{(
void*)((struct _tuple6**)Cyc__genarr_405),(void*)((struct _tuple6**)Cyc__genarr_405),(
void*)((struct _tuple6**)Cyc__genarr_405 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_398;struct _tuple36{unsigned int f1;struct Cyc_Absyn_Pat*f2;};static
struct _tuple6 Cyc__gentuple_399={offsetof(struct _tuple36,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_400={offsetof(struct _tuple36,f2),(void*)& Cyc__genrep_226};
static struct _tuple6*Cyc__genarr_401[2]={& Cyc__gentuple_399,& Cyc__gentuple_400};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_398={4,sizeof(struct _tuple36),{(
void*)((struct _tuple6**)Cyc__genarr_401),(void*)((struct _tuple6**)Cyc__genarr_401),(
void*)((struct _tuple6**)Cyc__genarr_401 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_312;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_377;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep;
static unsigned char _tmp223[8]="StructA";static struct _tuple7 Cyc__gentuple_361={0,{
_tmp223,_tmp223,_tmp223 + 8}};static unsigned char _tmp224[7]="UnionA";static struct
_tuple7 Cyc__gentuple_362={1,{_tmp224,_tmp224,_tmp224 + 7}};static struct _tuple7*
Cyc__genarr_363[2]={& Cyc__gentuple_361,& Cyc__gentuple_362};static struct _tuple5*
Cyc__genarr_364[0]={};static unsigned char _tmp226[9]="AggrKind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_aggr_kind_t_rep={5,{_tmp226,_tmp226,_tmp226 + 9},{(void*)((struct
_tuple7**)Cyc__genarr_363),(void*)((struct _tuple7**)Cyc__genarr_363),(void*)((
struct _tuple7**)Cyc__genarr_363 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_364),(
void*)((struct _tuple5**)Cyc__genarr_364),(void*)((struct _tuple5**)Cyc__genarr_364
+ 0)}};struct _tuple37{unsigned int f1;void*f2;struct _tuple0*f3;};static struct
_tuple6 Cyc__gentuple_378={offsetof(struct _tuple37,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_379={offsetof(struct _tuple37,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_380={offsetof(struct _tuple37,f3),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_381[3]={& Cyc__gentuple_378,& Cyc__gentuple_379,&
Cyc__gentuple_380};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_377={4,
sizeof(struct _tuple37),{(void*)((struct _tuple6**)Cyc__genarr_381),(void*)((
struct _tuple6**)Cyc__genarr_381),(void*)((struct _tuple6**)Cyc__genarr_381 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_337;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_338;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_339;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_340;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_aggrfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_341;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_342;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_83;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_attribute_t46H2_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_116;
struct _tuple38{unsigned int f1;int f2;};static struct _tuple6 Cyc__gentuple_117={
offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_118={
offsetof(struct _tuple38,f2),(void*)((void*)& Cyc__genrep_102)};static struct
_tuple6*Cyc__genarr_119[2]={& Cyc__gentuple_117,& Cyc__gentuple_118};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_116={4,sizeof(struct _tuple38),{(void*)((
struct _tuple6**)Cyc__genarr_119),(void*)((struct _tuple6**)Cyc__genarr_119),(void*)((
struct _tuple6**)Cyc__genarr_119 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_101;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep;static
unsigned char _tmp229[10]="Printf_ft";static struct _tuple7 Cyc__gentuple_103={0,{
_tmp229,_tmp229,_tmp229 + 10}};static unsigned char _tmp22A[9]="Scanf_ft";static
struct _tuple7 Cyc__gentuple_104={1,{_tmp22A,_tmp22A,_tmp22A + 9}};static struct
_tuple7*Cyc__genarr_105[2]={& Cyc__gentuple_103,& Cyc__gentuple_104};static struct
_tuple5*Cyc__genarr_106[0]={};static unsigned char _tmp22C[12]="Format_Type";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep={5,{_tmp22C,
_tmp22C,_tmp22C + 12},{(void*)((struct _tuple7**)Cyc__genarr_105),(void*)((struct
_tuple7**)Cyc__genarr_105),(void*)((struct _tuple7**)Cyc__genarr_105 + 2)},{(void*)((
struct _tuple5**)Cyc__genarr_106),(void*)((struct _tuple5**)Cyc__genarr_106),(void*)((
struct _tuple5**)Cyc__genarr_106 + 0)}};struct _tuple39{unsigned int f1;void*f2;int
f3;int f4;};static struct _tuple6 Cyc__gentuple_107={offsetof(struct _tuple39,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_108={offsetof(struct
_tuple39,f2),(void*)& Cyc_tunion_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_109={
offsetof(struct _tuple39,f3),(void*)((void*)& Cyc__genrep_102)};static struct
_tuple6 Cyc__gentuple_110={offsetof(struct _tuple39,f4),(void*)((void*)& Cyc__genrep_102)};
static struct _tuple6*Cyc__genarr_111[4]={& Cyc__gentuple_107,& Cyc__gentuple_108,&
Cyc__gentuple_109,& Cyc__gentuple_110};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_101={
4,sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_111),(void*)((
struct _tuple6**)Cyc__genarr_111),(void*)((struct _tuple6**)Cyc__genarr_111 + 4)}};
static unsigned char _tmp22E[12]="Stdcall_att";static struct _tuple7 Cyc__gentuple_84={
0,{_tmp22E,_tmp22E,_tmp22E + 12}};static unsigned char _tmp22F[10]="Cdecl_att";
static struct _tuple7 Cyc__gentuple_85={1,{_tmp22F,_tmp22F,_tmp22F + 10}};static
unsigned char _tmp230[13]="Fastcall_att";static struct _tuple7 Cyc__gentuple_86={2,{
_tmp230,_tmp230,_tmp230 + 13}};static unsigned char _tmp231[13]="Noreturn_att";
static struct _tuple7 Cyc__gentuple_87={3,{_tmp231,_tmp231,_tmp231 + 13}};static
unsigned char _tmp232[10]="Const_att";static struct _tuple7 Cyc__gentuple_88={4,{
_tmp232,_tmp232,_tmp232 + 10}};static unsigned char _tmp233[11]="Packed_att";static
struct _tuple7 Cyc__gentuple_89={5,{_tmp233,_tmp233,_tmp233 + 11}};static
unsigned char _tmp234[13]="Nocommon_att";static struct _tuple7 Cyc__gentuple_90={6,{
_tmp234,_tmp234,_tmp234 + 13}};static unsigned char _tmp235[11]="Shared_att";static
struct _tuple7 Cyc__gentuple_91={7,{_tmp235,_tmp235,_tmp235 + 11}};static
unsigned char _tmp236[11]="Unused_att";static struct _tuple7 Cyc__gentuple_92={8,{
_tmp236,_tmp236,_tmp236 + 11}};static unsigned char _tmp237[9]="Weak_att";static
struct _tuple7 Cyc__gentuple_93={9,{_tmp237,_tmp237,_tmp237 + 9}};static
unsigned char _tmp238[14]="Dllimport_att";static struct _tuple7 Cyc__gentuple_94={10,{
_tmp238,_tmp238,_tmp238 + 14}};static unsigned char _tmp239[14]="Dllexport_att";
static struct _tuple7 Cyc__gentuple_95={11,{_tmp239,_tmp239,_tmp239 + 14}};static
unsigned char _tmp23A[27]="No_instrument_function_att";static struct _tuple7 Cyc__gentuple_96={
12,{_tmp23A,_tmp23A,_tmp23A + 27}};static unsigned char _tmp23B[16]="Constructor_att";
static struct _tuple7 Cyc__gentuple_97={13,{_tmp23B,_tmp23B,_tmp23B + 16}};static
unsigned char _tmp23C[15]="Destructor_att";static struct _tuple7 Cyc__gentuple_98={
14,{_tmp23C,_tmp23C,_tmp23C + 15}};static unsigned char _tmp23D[26]="No_check_memory_usage_att";
static struct _tuple7 Cyc__gentuple_99={15,{_tmp23D,_tmp23D,_tmp23D + 26}};static
struct _tuple7*Cyc__genarr_100[16]={& Cyc__gentuple_84,& Cyc__gentuple_85,& Cyc__gentuple_86,&
Cyc__gentuple_87,& Cyc__gentuple_88,& Cyc__gentuple_89,& Cyc__gentuple_90,& Cyc__gentuple_91,&
Cyc__gentuple_92,& Cyc__gentuple_93,& Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,&
Cyc__gentuple_97,& Cyc__gentuple_98,& Cyc__gentuple_99};static unsigned char _tmp23E[
12]="Regparm_att";static struct _tuple5 Cyc__gentuple_120={0,{_tmp23E,_tmp23E,
_tmp23E + 12},(void*)& Cyc__genrep_116};static unsigned char _tmp23F[12]="Aligned_att";
static struct _tuple5 Cyc__gentuple_121={1,{_tmp23F,_tmp23F,_tmp23F + 12},(void*)&
Cyc__genrep_116};static unsigned char _tmp240[12]="Section_att";static struct
_tuple5 Cyc__gentuple_122={2,{_tmp240,_tmp240,_tmp240 + 12},(void*)& Cyc__genrep_112};
static unsigned char _tmp241[11]="Format_att";static struct _tuple5 Cyc__gentuple_123={
3,{_tmp241,_tmp241,_tmp241 + 11},(void*)& Cyc__genrep_101};static struct _tuple5*Cyc__genarr_124[
4]={& Cyc__gentuple_120,& Cyc__gentuple_121,& Cyc__gentuple_122,& Cyc__gentuple_123};
static unsigned char _tmp243[10]="Attribute";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={
5,{_tmp243,_tmp243,_tmp243 + 10},{(void*)((struct _tuple7**)Cyc__genarr_100),(void*)((
struct _tuple7**)Cyc__genarr_100),(void*)((struct _tuple7**)Cyc__genarr_100 + 16)},{(
void*)((struct _tuple5**)Cyc__genarr_124),(void*)((struct _tuple5**)Cyc__genarr_124),(
void*)((struct _tuple5**)Cyc__genarr_124 + 4)}};static unsigned char _tmp244[5]="List";
static struct _tagged_arr Cyc__genname_128={_tmp244,_tmp244,_tmp244 + 5};static
unsigned char _tmp245[3]="hd";static struct _tuple5 Cyc__gentuple_125={offsetof(
struct Cyc_List_List,hd),{_tmp245,_tmp245,_tmp245 + 3},(void*)& Cyc_Absyn_attribute_t_rep};
static unsigned char _tmp246[3]="tl";static struct _tuple5 Cyc__gentuple_126={
offsetof(struct Cyc_List_List,tl),{_tmp246,_tmp246,_tmp246 + 3},(void*)& Cyc__genrep_83};
static struct _tuple5*Cyc__genarr_127[2]={& Cyc__gentuple_125,& Cyc__gentuple_126};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_128,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_127),(void*)((struct _tuple5**)Cyc__genarr_127),(void*)((
struct _tuple5**)Cyc__genarr_127 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_83={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};static
unsigned char _tmp249[10]="Aggrfield";static struct _tagged_arr Cyc__genname_349={
_tmp249,_tmp249,_tmp249 + 10};static unsigned char _tmp24A[5]="name";static struct
_tuple5 Cyc__gentuple_343={offsetof(struct Cyc_Absyn_Aggrfield,name),{_tmp24A,
_tmp24A,_tmp24A + 5},(void*)& Cyc__genrep_12};static unsigned char _tmp24B[3]="tq";
static struct _tuple5 Cyc__gentuple_344={offsetof(struct Cyc_Absyn_Aggrfield,tq),{
_tmp24B,_tmp24B,_tmp24B + 3},(void*)& Cyc__genrep_132};static unsigned char _tmp24C[
5]="type";static struct _tuple5 Cyc__gentuple_345={offsetof(struct Cyc_Absyn_Aggrfield,type),{
_tmp24C,_tmp24C,_tmp24C + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static
unsigned char _tmp24D[6]="width";static struct _tuple5 Cyc__gentuple_346={offsetof(
struct Cyc_Absyn_Aggrfield,width),{_tmp24D,_tmp24D,_tmp24D + 6},(void*)& Cyc__genrep_73};
static unsigned char _tmp24E[11]="attributes";static struct _tuple5 Cyc__gentuple_347={
offsetof(struct Cyc_Absyn_Aggrfield,attributes),{_tmp24E,_tmp24E,_tmp24E + 11},(
void*)& Cyc__genrep_83};static struct _tuple5*Cyc__genarr_348[5]={& Cyc__gentuple_343,&
Cyc__gentuple_344,& Cyc__gentuple_345,& Cyc__gentuple_346,& Cyc__gentuple_347};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={3,(struct
_tagged_arr*)& Cyc__genname_349,sizeof(struct Cyc_Absyn_Aggrfield),{(void*)((
struct _tuple5**)Cyc__genarr_348),(void*)((struct _tuple5**)Cyc__genarr_348),(void*)((
struct _tuple5**)Cyc__genarr_348 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_342={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrfield_rep)};static unsigned char _tmp251[
5]="List";static struct _tagged_arr Cyc__genname_353={_tmp251,_tmp251,_tmp251 + 5};
static unsigned char _tmp252[3]="hd";static struct _tuple5 Cyc__gentuple_350={
offsetof(struct Cyc_List_List,hd),{_tmp252,_tmp252,_tmp252 + 3},(void*)& Cyc__genrep_342};
static unsigned char _tmp253[3]="tl";static struct _tuple5 Cyc__gentuple_351={
offsetof(struct Cyc_List_List,tl),{_tmp253,_tmp253,_tmp253 + 3},(void*)& Cyc__genrep_341};
static struct _tuple5*Cyc__genarr_352[2]={& Cyc__gentuple_350,& Cyc__gentuple_351};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_353,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_352),(void*)((struct _tuple5**)Cyc__genarr_352),(void*)((
struct _tuple5**)Cyc__genarr_352 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_341={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};static
unsigned char _tmp256[4]="Opt";static struct _tagged_arr Cyc__genname_356={_tmp256,
_tmp256,_tmp256 + 4};static unsigned char _tmp257[2]="v";static struct _tuple5 Cyc__gentuple_354={
offsetof(struct Cyc_Core_Opt,v),{_tmp257,_tmp257,_tmp257 + 2},(void*)& Cyc__genrep_341};
static struct _tuple5*Cyc__genarr_355[1]={& Cyc__gentuple_354};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_aggrfield_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_356,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_355),(
void*)((struct _tuple5**)Cyc__genarr_355),(void*)((struct _tuple5**)Cyc__genarr_355
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_340={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_aggrfield_t46H22_rep)};static unsigned char
_tmp25A[9]="Aggrdecl";static struct _tagged_arr Cyc__genname_373={_tmp25A,_tmp25A,
_tmp25A + 9};static unsigned char _tmp25B[5]="kind";static struct _tuple5 Cyc__gentuple_365={
offsetof(struct Cyc_Absyn_Aggrdecl,kind),{_tmp25B,_tmp25B,_tmp25B + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};
static unsigned char _tmp25C[3]="sc";static struct _tuple5 Cyc__gentuple_366={
offsetof(struct Cyc_Absyn_Aggrdecl,sc),{_tmp25C,_tmp25C,_tmp25C + 3},(void*)& Cyc_Absyn_scope_t_rep};
static unsigned char _tmp25D[5]="name";static struct _tuple5 Cyc__gentuple_367={
offsetof(struct Cyc_Absyn_Aggrdecl,name),{_tmp25D,_tmp25D,_tmp25D + 5},(void*)& Cyc__genrep_10};
static unsigned char _tmp25E[4]="tvs";static struct _tuple5 Cyc__gentuple_368={
offsetof(struct Cyc_Absyn_Aggrdecl,tvs),{_tmp25E,_tmp25E,_tmp25E + 4},(void*)& Cyc__genrep_236};
static unsigned char _tmp25F[11]="exist_vars";static struct _tuple5 Cyc__gentuple_369={
offsetof(struct Cyc_Absyn_Aggrdecl,exist_vars),{_tmp25F,_tmp25F,_tmp25F + 11},(
void*)& Cyc__genrep_357};static unsigned char _tmp260[7]="fields";static struct
_tuple5 Cyc__gentuple_370={offsetof(struct Cyc_Absyn_Aggrdecl,fields),{_tmp260,
_tmp260,_tmp260 + 7},(void*)& Cyc__genrep_340};static unsigned char _tmp261[11]="attributes";
static struct _tuple5 Cyc__gentuple_371={offsetof(struct Cyc_Absyn_Aggrdecl,attributes),{
_tmp261,_tmp261,_tmp261 + 11},(void*)& Cyc__genrep_83};static struct _tuple5*Cyc__genarr_372[
7]={& Cyc__gentuple_365,& Cyc__gentuple_366,& Cyc__gentuple_367,& Cyc__gentuple_368,&
Cyc__gentuple_369,& Cyc__gentuple_370,& Cyc__gentuple_371};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Aggrdecl_rep={3,(struct _tagged_arr*)& Cyc__genname_373,sizeof(
struct Cyc_Absyn_Aggrdecl),{(void*)((struct _tuple5**)Cyc__genarr_372),(void*)((
struct _tuple5**)Cyc__genarr_372),(void*)((struct _tuple5**)Cyc__genarr_372 + 7)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_339={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_338={1,1,(void*)((void*)& Cyc__genrep_339)};
struct _tuple40{unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6
Cyc__gentuple_374={offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_375={offsetof(struct _tuple40,f2),(void*)& Cyc__genrep_338};
static struct _tuple6*Cyc__genarr_376[2]={& Cyc__gentuple_374,& Cyc__gentuple_375};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_337={4,sizeof(struct _tuple40),{(
void*)((struct _tuple6**)Cyc__genarr_376),(void*)((struct _tuple6**)Cyc__genarr_376),(
void*)((struct _tuple6**)Cyc__genarr_376 + 2)}};static struct _tuple7*Cyc__genarr_336[
0]={};static unsigned char _tmp266[12]="UnknownAggr";static struct _tuple5 Cyc__gentuple_382={
0,{_tmp266,_tmp266,_tmp266 + 12},(void*)& Cyc__genrep_377};static unsigned char
_tmp267[10]="KnownAggr";static struct _tuple5 Cyc__gentuple_383={1,{_tmp267,_tmp267,
_tmp267 + 10},(void*)& Cyc__genrep_337};static struct _tuple5*Cyc__genarr_384[2]={&
Cyc__gentuple_382,& Cyc__gentuple_383};static unsigned char _tmp269[10]="AggrInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep={5,{_tmp269,
_tmp269,_tmp269 + 10},{(void*)((struct _tuple7**)Cyc__genarr_336),(void*)((struct
_tuple7**)Cyc__genarr_336),(void*)((struct _tuple7**)Cyc__genarr_336 + 0)},{(void*)((
struct _tuple5**)Cyc__genarr_384),(void*)((struct _tuple5**)Cyc__genarr_384),(void*)((
struct _tuple5**)Cyc__genarr_384 + 2)}};static unsigned char _tmp26A[9]="AggrInfo";
static struct _tagged_arr Cyc__genname_388={_tmp26A,_tmp26A,_tmp26A + 9};static
unsigned char _tmp26B[10]="aggr_info";static struct _tuple5 Cyc__gentuple_385={
offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),{_tmp26B,_tmp26B,_tmp26B + 10},(void*)&
Cyc_tunion_Absyn_AggrInfoU_rep};static unsigned char _tmp26C[6]="targs";static
struct _tuple5 Cyc__gentuple_386={offsetof(struct Cyc_Absyn_AggrInfo,targs),{
_tmp26C,_tmp26C,_tmp26C + 6},(void*)& Cyc__genrep_52};static struct _tuple5*Cyc__genarr_387[
2]={& Cyc__gentuple_385,& Cyc__gentuple_386};struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_388,sizeof(struct Cyc_Absyn_AggrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_387),(void*)((struct _tuple5**)Cyc__genarr_387),(void*)((
struct _tuple5**)Cyc__genarr_387 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_313;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_314;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_315;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_316;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_318;struct _tuple41{unsigned int f1;struct _tagged_arr*f2;};static
struct _tuple6 Cyc__gentuple_319={offsetof(struct _tuple41,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_320={offsetof(struct _tuple41,f2),(void*)& Cyc__genrep_12};
static struct _tuple6*Cyc__genarr_321[2]={& Cyc__gentuple_319,& Cyc__gentuple_320};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_318={4,sizeof(struct _tuple41),{(
void*)((struct _tuple6**)Cyc__genarr_321),(void*)((struct _tuple6**)Cyc__genarr_321),(
void*)((struct _tuple6**)Cyc__genarr_321 + 2)}};static struct _tuple7*Cyc__genarr_317[
0]={};static unsigned char _tmp26F[13]="ArrayElement";static struct _tuple5 Cyc__gentuple_322={
0,{_tmp26F,_tmp26F,_tmp26F + 13},(void*)& Cyc__genrep_76};static unsigned char
_tmp270[10]="FieldName";static struct _tuple5 Cyc__gentuple_323={1,{_tmp270,_tmp270,
_tmp270 + 10},(void*)& Cyc__genrep_318};static struct _tuple5*Cyc__genarr_324[2]={&
Cyc__gentuple_322,& Cyc__gentuple_323};static unsigned char _tmp272[11]="Designator";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={5,{_tmp272,_tmp272,
_tmp272 + 11},{(void*)((struct _tuple7**)Cyc__genarr_317),(void*)((struct _tuple7**)
Cyc__genarr_317),(void*)((struct _tuple7**)Cyc__genarr_317 + 0)},{(void*)((struct
_tuple5**)Cyc__genarr_324),(void*)((struct _tuple5**)Cyc__genarr_324),(void*)((
struct _tuple5**)Cyc__genarr_324 + 2)}};static unsigned char _tmp273[5]="List";
static struct _tagged_arr Cyc__genname_328={_tmp273,_tmp273,_tmp273 + 5};static
unsigned char _tmp274[3]="hd";static struct _tuple5 Cyc__gentuple_325={offsetof(
struct Cyc_List_List,hd),{_tmp274,_tmp274,_tmp274 + 3},(void*)& Cyc_Absyn_designator_t_rep};
static unsigned char _tmp275[3]="tl";static struct _tuple5 Cyc__gentuple_326={
offsetof(struct Cyc_List_List,tl),{_tmp275,_tmp275,_tmp275 + 3},(void*)& Cyc__genrep_316};
static struct _tuple5*Cyc__genarr_327[2]={& Cyc__gentuple_325,& Cyc__gentuple_326};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_328,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_327),(void*)((struct _tuple5**)Cyc__genarr_327),(void*)((
struct _tuple5**)Cyc__genarr_327 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_316={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};struct
_tuple42{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_329={
offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_316};static struct _tuple6 Cyc__gentuple_330={
offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_226};static struct _tuple6*Cyc__genarr_331[
2]={& Cyc__gentuple_329,& Cyc__gentuple_330};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_315={4,sizeof(struct _tuple42),{(void*)((struct _tuple6**)Cyc__genarr_331),(
void*)((struct _tuple6**)Cyc__genarr_331),(void*)((struct _tuple6**)Cyc__genarr_331
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_314={1,1,(void*)((void*)&
Cyc__genrep_315)};static unsigned char _tmp27A[5]="List";static struct _tagged_arr
Cyc__genname_335={_tmp27A,_tmp27A,_tmp27A + 5};static unsigned char _tmp27B[3]="hd";
static struct _tuple5 Cyc__gentuple_332={offsetof(struct Cyc_List_List,hd),{_tmp27B,
_tmp27B,_tmp27B + 3},(void*)& Cyc__genrep_314};static unsigned char _tmp27C[3]="tl";
static struct _tuple5 Cyc__gentuple_333={offsetof(struct Cyc_List_List,tl),{_tmp27C,
_tmp27C,_tmp27C + 3},(void*)& Cyc__genrep_313};static struct _tuple5*Cyc__genarr_334[
2]={& Cyc__gentuple_332,& Cyc__gentuple_333};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_335,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_334),(void*)((struct _tuple5**)Cyc__genarr_334),(void*)((
struct _tuple5**)Cyc__genarr_334 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_313={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple43{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;};static struct _tuple6 Cyc__gentuple_389={offsetof(struct
_tuple43,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_390={
offsetof(struct _tuple43,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_391={offsetof(struct _tuple43,f3),(void*)& Cyc__genrep_236};
static struct _tuple6 Cyc__gentuple_392={offsetof(struct _tuple43,f4),(void*)& Cyc__genrep_313};
static struct _tuple6*Cyc__genarr_393[4]={& Cyc__gentuple_389,& Cyc__gentuple_390,&
Cyc__gentuple_391,& Cyc__gentuple_392};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_312={
4,sizeof(struct _tuple43),{(void*)((struct _tuple6**)Cyc__genarr_393),(void*)((
struct _tuple6**)Cyc__genarr_393),(void*)((struct _tuple6**)Cyc__genarr_393 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_270;struct _tuple44{unsigned int
f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;struct Cyc_List_List*
f4;struct Cyc_List_List*f5;};static struct _tuple6 Cyc__gentuple_306={offsetof(
struct _tuple44,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_307={
offsetof(struct _tuple44,f2),(void*)((void*)& Cyc__genrep_289)};static struct
_tuple6 Cyc__gentuple_308={offsetof(struct _tuple44,f3),(void*)& Cyc__genrep_271};
static struct _tuple6 Cyc__gentuple_309={offsetof(struct _tuple44,f4),(void*)& Cyc__genrep_236};
static struct _tuple6 Cyc__gentuple_310={offsetof(struct _tuple44,f5),(void*)& Cyc__genrep_231};
static struct _tuple6*Cyc__genarr_311[5]={& Cyc__gentuple_306,& Cyc__gentuple_307,&
Cyc__gentuple_308,& Cyc__gentuple_309,& Cyc__gentuple_310};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_270={4,sizeof(struct _tuple44),{(void*)((struct _tuple6**)Cyc__genarr_311),(
void*)((struct _tuple6**)Cyc__genarr_311),(void*)((struct _tuple6**)Cyc__genarr_311
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_255;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_256;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_257;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_71;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_72;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumfield_rep;static unsigned char _tmp281[10]="Enumfield";static
struct _tagged_arr Cyc__genname_915={_tmp281,_tmp281,_tmp281 + 10};static
unsigned char _tmp282[5]="name";static struct _tuple5 Cyc__gentuple_911={offsetof(
struct Cyc_Absyn_Enumfield,name),{_tmp282,_tmp282,_tmp282 + 5},(void*)& Cyc__genrep_10};
static unsigned char _tmp283[4]="tag";static struct _tuple5 Cyc__gentuple_912={
offsetof(struct Cyc_Absyn_Enumfield,tag),{_tmp283,_tmp283,_tmp283 + 4},(void*)& Cyc__genrep_73};
static unsigned char _tmp284[4]="loc";static struct _tuple5 Cyc__gentuple_913={
offsetof(struct Cyc_Absyn_Enumfield,loc),{_tmp284,_tmp284,_tmp284 + 4},(void*)& Cyc__genrep_2};
static struct _tuple5*Cyc__genarr_914[3]={& Cyc__gentuple_911,& Cyc__gentuple_912,&
Cyc__gentuple_913};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_915,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple5**)Cyc__genarr_914),(void*)((struct _tuple5**)Cyc__genarr_914),(void*)((
struct _tuple5**)Cyc__genarr_914 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_72={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};static unsigned char _tmp287[
5]="List";static struct _tagged_arr Cyc__genname_919={_tmp287,_tmp287,_tmp287 + 5};
static unsigned char _tmp288[3]="hd";static struct _tuple5 Cyc__gentuple_916={
offsetof(struct Cyc_List_List,hd),{_tmp288,_tmp288,_tmp288 + 3},(void*)& Cyc__genrep_72};
static unsigned char _tmp289[3]="tl";static struct _tuple5 Cyc__gentuple_917={
offsetof(struct Cyc_List_List,tl),{_tmp289,_tmp289,_tmp289 + 3},(void*)& Cyc__genrep_71};
static struct _tuple5*Cyc__genarr_918[2]={& Cyc__gentuple_916,& Cyc__gentuple_917};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_919,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_918),(void*)((struct _tuple5**)Cyc__genarr_918),(void*)((
struct _tuple5**)Cyc__genarr_918 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_71={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static
unsigned char _tmp28C[4]="Opt";static struct _tagged_arr Cyc__genname_260={_tmp28C,
_tmp28C,_tmp28C + 4};static unsigned char _tmp28D[2]="v";static struct _tuple5 Cyc__gentuple_258={
offsetof(struct Cyc_Core_Opt,v),{_tmp28D,_tmp28D,_tmp28D + 2},(void*)& Cyc__genrep_71};
static struct _tuple5*Cyc__genarr_259[1]={& Cyc__gentuple_258};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_260,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_259),(
void*)((struct _tuple5**)Cyc__genarr_259),(void*)((struct _tuple5**)Cyc__genarr_259
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_257={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};static unsigned char
_tmp290[9]="Enumdecl";static struct _tagged_arr Cyc__genname_265={_tmp290,_tmp290,
_tmp290 + 9};static unsigned char _tmp291[3]="sc";static struct _tuple5 Cyc__gentuple_261={
offsetof(struct Cyc_Absyn_Enumdecl,sc),{_tmp291,_tmp291,_tmp291 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static unsigned char _tmp292[5]="name";static struct _tuple5 Cyc__gentuple_262={
offsetof(struct Cyc_Absyn_Enumdecl,name),{_tmp292,_tmp292,_tmp292 + 5},(void*)& Cyc__genrep_10};
static unsigned char _tmp293[7]="fields";static struct _tuple5 Cyc__gentuple_263={
offsetof(struct Cyc_Absyn_Enumdecl,fields),{_tmp293,_tmp293,_tmp293 + 7},(void*)&
Cyc__genrep_257};static struct _tuple5*Cyc__genarr_264[3]={& Cyc__gentuple_261,& Cyc__gentuple_262,&
Cyc__gentuple_263};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_265,sizeof(struct Cyc_Absyn_Enumdecl),{(void*)((
struct _tuple5**)Cyc__genarr_264),(void*)((struct _tuple5**)Cyc__genarr_264),(void*)((
struct _tuple5**)Cyc__genarr_264 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_256={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};struct _tuple45{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};static struct
_tuple6 Cyc__gentuple_266={offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_267={offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_256};
static struct _tuple6 Cyc__gentuple_268={offsetof(struct _tuple45,f3),(void*)& Cyc__genrep_72};
static struct _tuple6*Cyc__genarr_269[3]={& Cyc__gentuple_266,& Cyc__gentuple_267,&
Cyc__gentuple_268};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_255={4,
sizeof(struct _tuple45),{(void*)((struct _tuple6**)Cyc__genarr_269),(void*)((
struct _tuple6**)Cyc__genarr_269),(void*)((struct _tuple6**)Cyc__genarr_269 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_250;struct _tuple46{unsigned int
f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_251={
offsetof(struct _tuple46,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_252={
offsetof(struct _tuple46,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_253={offsetof(struct _tuple46,f3),(void*)& Cyc__genrep_72};
static struct _tuple6*Cyc__genarr_254[3]={& Cyc__gentuple_251,& Cyc__gentuple_252,&
Cyc__gentuple_253};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_250={4,
sizeof(struct _tuple46),{(void*)((struct _tuple6**)Cyc__genarr_254),(void*)((
struct _tuple6**)Cyc__genarr_254),(void*)((struct _tuple6**)Cyc__genarr_254 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_246;struct _tuple47{unsigned int
f1;struct _tuple0*f2;};static struct _tuple6 Cyc__gentuple_247={offsetof(struct
_tuple47,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_248={
offsetof(struct _tuple47,f2),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_249[
2]={& Cyc__gentuple_247,& Cyc__gentuple_248};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_246={4,sizeof(struct _tuple47),{(void*)((struct _tuple6**)Cyc__genarr_249),(
void*)((struct _tuple6**)Cyc__genarr_249),(void*)((struct _tuple6**)Cyc__genarr_249
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_230;struct _tuple48{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
static struct _tuple6 Cyc__gentuple_241={offsetof(struct _tuple48,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_242={offsetof(struct _tuple48,f2),(void*)& Cyc__genrep_10};
static struct _tuple6 Cyc__gentuple_243={offsetof(struct _tuple48,f3),(void*)& Cyc__genrep_236};
static struct _tuple6 Cyc__gentuple_244={offsetof(struct _tuple48,f4),(void*)& Cyc__genrep_231};
static struct _tuple6*Cyc__genarr_245[4]={& Cyc__gentuple_241,& Cyc__gentuple_242,&
Cyc__gentuple_243,& Cyc__gentuple_244};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_230={
4,sizeof(struct _tuple48),{(void*)((struct _tuple6**)Cyc__genarr_245),(void*)((
struct _tuple6**)Cyc__genarr_245),(void*)((struct _tuple6**)Cyc__genarr_245 + 4)}};
static unsigned char _tmp29A[7]="Wild_p";static struct _tuple7 Cyc__gentuple_227={0,{
_tmp29A,_tmp29A,_tmp29A + 7}};static unsigned char _tmp29B[7]="Null_p";static struct
_tuple7 Cyc__gentuple_228={1,{_tmp29B,_tmp29B,_tmp29B + 7}};static struct _tuple7*
Cyc__genarr_229[2]={& Cyc__gentuple_227,& Cyc__gentuple_228};static unsigned char
_tmp29C[6]="Var_p";static struct _tuple5 Cyc__gentuple_419={0,{_tmp29C,_tmp29C,
_tmp29C + 6},(void*)& Cyc__genrep_394};static unsigned char _tmp29D[6]="Int_p";
static struct _tuple5 Cyc__gentuple_420={1,{_tmp29D,_tmp29D,_tmp29D + 6},(void*)& Cyc__genrep_410};
static unsigned char _tmp29E[7]="Char_p";static struct _tuple5 Cyc__gentuple_421={2,{
_tmp29E,_tmp29E,_tmp29E + 7},(void*)& Cyc__genrep_406};static unsigned char _tmp29F[
8]="Float_p";static struct _tuple5 Cyc__gentuple_422={3,{_tmp29F,_tmp29F,_tmp29F + 8},(
void*)& Cyc__genrep_112};static unsigned char _tmp2A0[8]="Tuple_p";static struct
_tuple5 Cyc__gentuple_423={4,{_tmp2A0,_tmp2A0,_tmp2A0 + 8},(void*)& Cyc__genrep_402};
static unsigned char _tmp2A1[10]="Pointer_p";static struct _tuple5 Cyc__gentuple_424={
5,{_tmp2A1,_tmp2A1,_tmp2A1 + 10},(void*)& Cyc__genrep_398};static unsigned char
_tmp2A2[12]="Reference_p";static struct _tuple5 Cyc__gentuple_425={6,{_tmp2A2,
_tmp2A2,_tmp2A2 + 12},(void*)& Cyc__genrep_394};static unsigned char _tmp2A3[7]="Aggr_p";
static struct _tuple5 Cyc__gentuple_426={7,{_tmp2A3,_tmp2A3,_tmp2A3 + 7},(void*)& Cyc__genrep_312};
static unsigned char _tmp2A4[9]="Tunion_p";static struct _tuple5 Cyc__gentuple_427={8,{
_tmp2A4,_tmp2A4,_tmp2A4 + 9},(void*)& Cyc__genrep_270};static unsigned char _tmp2A5[
7]="Enum_p";static struct _tuple5 Cyc__gentuple_428={9,{_tmp2A5,_tmp2A5,_tmp2A5 + 7},(
void*)& Cyc__genrep_255};static unsigned char _tmp2A6[11]="AnonEnum_p";static struct
_tuple5 Cyc__gentuple_429={10,{_tmp2A6,_tmp2A6,_tmp2A6 + 11},(void*)& Cyc__genrep_250};
static unsigned char _tmp2A7[12]="UnknownId_p";static struct _tuple5 Cyc__gentuple_430={
11,{_tmp2A7,_tmp2A7,_tmp2A7 + 12},(void*)& Cyc__genrep_246};static unsigned char
_tmp2A8[14]="UnknownCall_p";static struct _tuple5 Cyc__gentuple_431={12,{_tmp2A8,
_tmp2A8,_tmp2A8 + 14},(void*)& Cyc__genrep_230};static struct _tuple5*Cyc__genarr_432[
13]={& Cyc__gentuple_419,& Cyc__gentuple_420,& Cyc__gentuple_421,& Cyc__gentuple_422,&
Cyc__gentuple_423,& Cyc__gentuple_424,& Cyc__gentuple_425,& Cyc__gentuple_426,& Cyc__gentuple_427,&
Cyc__gentuple_428,& Cyc__gentuple_429,& Cyc__gentuple_430,& Cyc__gentuple_431};
static unsigned char _tmp2AA[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_pat_t_rep={
5,{_tmp2AA,_tmp2AA,_tmp2AA + 8},{(void*)((struct _tuple7**)Cyc__genarr_229),(void*)((
struct _tuple7**)Cyc__genarr_229),(void*)((struct _tuple7**)Cyc__genarr_229 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_432),(void*)((struct _tuple5**)Cyc__genarr_432),(
void*)((struct _tuple5**)Cyc__genarr_432 + 13)}};static unsigned char _tmp2AB[4]="Pat";
static struct _tagged_arr Cyc__genname_437={_tmp2AB,_tmp2AB,_tmp2AB + 4};static
unsigned char _tmp2AC[2]="r";static struct _tuple5 Cyc__gentuple_433={offsetof(
struct Cyc_Absyn_Pat,r),{_tmp2AC,_tmp2AC,_tmp2AC + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};
static unsigned char _tmp2AD[5]="topt";static struct _tuple5 Cyc__gentuple_434={
offsetof(struct Cyc_Absyn_Pat,topt),{_tmp2AD,_tmp2AD,_tmp2AD + 5},(void*)& Cyc__genrep_61};
static unsigned char _tmp2AE[4]="loc";static struct _tuple5 Cyc__gentuple_435={
offsetof(struct Cyc_Absyn_Pat,loc),{_tmp2AE,_tmp2AE,_tmp2AE + 4},(void*)& Cyc__genrep_2};
static struct _tuple5*Cyc__genarr_436[3]={& Cyc__gentuple_433,& Cyc__gentuple_434,&
Cyc__gentuple_435};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={3,(
struct _tagged_arr*)& Cyc__genname_437,sizeof(struct Cyc_Absyn_Pat),{(void*)((
struct _tuple5**)Cyc__genarr_436),(void*)((struct _tuple5**)Cyc__genarr_436),(void*)((
struct _tuple5**)Cyc__genarr_436 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_226={
1,1,(void*)((void*)& Cyc_struct_Absyn_Pat_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_129;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;static unsigned char _tmp2B1[5]="List";
static struct _tagged_arr Cyc__genname_155={_tmp2B1,_tmp2B1,_tmp2B1 + 5};static
unsigned char _tmp2B2[3]="hd";static struct _tuple5 Cyc__gentuple_152={offsetof(
struct Cyc_List_List,hd),{_tmp2B2,_tmp2B2,_tmp2B2 + 3},(void*)& Cyc__genrep_131};
static unsigned char _tmp2B3[3]="tl";static struct _tuple5 Cyc__gentuple_153={
offsetof(struct Cyc_List_List,tl),{_tmp2B3,_tmp2B3,_tmp2B3 + 3},(void*)& Cyc__genrep_130};
static struct _tuple5*Cyc__genarr_154[2]={& Cyc__gentuple_152,& Cyc__gentuple_153};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_155,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_154),(void*)((struct _tuple5**)Cyc__genarr_154),(void*)((
struct _tuple5**)Cyc__genarr_154 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static
unsigned char _tmp2B6[4]="Opt";static struct _tagged_arr Cyc__genname_158={_tmp2B6,
_tmp2B6,_tmp2B6 + 4};static unsigned char _tmp2B7[2]="v";static struct _tuple5 Cyc__gentuple_156={
offsetof(struct Cyc_Core_Opt,v),{_tmp2B7,_tmp2B7,_tmp2B7 + 2},(void*)& Cyc__genrep_130};
static struct _tuple5*Cyc__genarr_157[1]={& Cyc__gentuple_156};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_158,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_157),(
void*)((struct _tuple5**)Cyc__genarr_157),(void*)((struct _tuple5**)Cyc__genarr_157
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_129={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};static unsigned char
_tmp2BA[14]="Switch_clause";static struct _tagged_arr Cyc__genname_444={_tmp2BA,
_tmp2BA,_tmp2BA + 14};static unsigned char _tmp2BB[8]="pattern";static struct _tuple5
Cyc__gentuple_438={offsetof(struct Cyc_Absyn_Switch_clause,pattern),{_tmp2BB,
_tmp2BB,_tmp2BB + 8},(void*)& Cyc__genrep_226};static unsigned char _tmp2BC[9]="pat_vars";
static struct _tuple5 Cyc__gentuple_439={offsetof(struct Cyc_Absyn_Switch_clause,pat_vars),{
_tmp2BC,_tmp2BC,_tmp2BC + 9},(void*)& Cyc__genrep_129};static unsigned char _tmp2BD[
13]="where_clause";static struct _tuple5 Cyc__gentuple_440={offsetof(struct Cyc_Absyn_Switch_clause,where_clause),{
_tmp2BD,_tmp2BD,_tmp2BD + 13},(void*)& Cyc__genrep_73};static unsigned char _tmp2BE[
5]="body";static struct _tuple5 Cyc__gentuple_441={offsetof(struct Cyc_Absyn_Switch_clause,body),{
_tmp2BE,_tmp2BE,_tmp2BE + 5},(void*)& Cyc__genrep_159};static unsigned char _tmp2BF[
4]="loc";static struct _tuple5 Cyc__gentuple_442={offsetof(struct Cyc_Absyn_Switch_clause,loc),{
_tmp2BF,_tmp2BF,_tmp2BF + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_443[
5]={& Cyc__gentuple_438,& Cyc__gentuple_439,& Cyc__gentuple_440,& Cyc__gentuple_441,&
Cyc__gentuple_442};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _tagged_arr*)& Cyc__genname_444,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_443),(void*)((struct _tuple5**)Cyc__genarr_443),(
void*)((struct _tuple5**)Cyc__genarr_443 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_225={1,1,(void*)((void*)& Cyc_struct_Absyn_Switch_clause_rep)};static
unsigned char _tmp2C2[5]="List";static struct _tagged_arr Cyc__genname_448={_tmp2C2,
_tmp2C2,_tmp2C2 + 5};static unsigned char _tmp2C3[3]="hd";static struct _tuple5 Cyc__gentuple_445={
offsetof(struct Cyc_List_List,hd),{_tmp2C3,_tmp2C3,_tmp2C3 + 3},(void*)((void*)&
Cyc__genrep_225)};static unsigned char _tmp2C4[3]="tl";static struct _tuple5 Cyc__gentuple_446={
offsetof(struct Cyc_List_List,tl),{_tmp2C4,_tmp2C4,_tmp2C4 + 3},(void*)& Cyc__genrep_224};
static struct _tuple5*Cyc__genarr_447[2]={& Cyc__gentuple_445,& Cyc__gentuple_446};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_448,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_447),(void*)((struct _tuple5**)Cyc__genarr_447),(void*)((
struct _tuple5**)Cyc__genarr_447 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_224={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};struct
_tuple49{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_500={offsetof(struct _tuple49,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_501={offsetof(struct _tuple49,f2),(void*)& Cyc__genrep_77};
static struct _tuple6 Cyc__gentuple_502={offsetof(struct _tuple49,f3),(void*)& Cyc__genrep_224};
static struct _tuple6*Cyc__genarr_503[3]={& Cyc__gentuple_500,& Cyc__gentuple_501,&
Cyc__gentuple_502};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_499={4,
sizeof(struct _tuple49),{(void*)((struct _tuple6**)Cyc__genarr_503),(void*)((
struct _tuple6**)Cyc__genarr_503),(void*)((struct _tuple6**)Cyc__genarr_503 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_483;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_484;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_485;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep;static unsigned char _tmp2C8[15]="SwitchC_clause";
static struct _tagged_arr Cyc__genname_490={_tmp2C8,_tmp2C8,_tmp2C8 + 15};static
unsigned char _tmp2C9[9]="cnst_exp";static struct _tuple5 Cyc__gentuple_486={
offsetof(struct Cyc_Absyn_SwitchC_clause,cnst_exp),{_tmp2C9,_tmp2C9,_tmp2C9 + 9},(
void*)& Cyc__genrep_73};static unsigned char _tmp2CA[5]="body";static struct _tuple5
Cyc__gentuple_487={offsetof(struct Cyc_Absyn_SwitchC_clause,body),{_tmp2CA,
_tmp2CA,_tmp2CA + 5},(void*)& Cyc__genrep_159};static unsigned char _tmp2CB[4]="loc";
static struct _tuple5 Cyc__gentuple_488={offsetof(struct Cyc_Absyn_SwitchC_clause,loc),{
_tmp2CB,_tmp2CB,_tmp2CB + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_489[
3]={& Cyc__gentuple_486,& Cyc__gentuple_487,& Cyc__gentuple_488};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep={3,(struct _tagged_arr*)& Cyc__genname_490,
sizeof(struct Cyc_Absyn_SwitchC_clause),{(void*)((struct _tuple5**)Cyc__genarr_489),(
void*)((struct _tuple5**)Cyc__genarr_489),(void*)((struct _tuple5**)Cyc__genarr_489
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_485={1,1,(void*)((void*)&
Cyc_struct_Absyn_SwitchC_clause_rep)};static unsigned char _tmp2CE[5]="List";
static struct _tagged_arr Cyc__genname_494={_tmp2CE,_tmp2CE,_tmp2CE + 5};static
unsigned char _tmp2CF[3]="hd";static struct _tuple5 Cyc__gentuple_491={offsetof(
struct Cyc_List_List,hd),{_tmp2CF,_tmp2CF,_tmp2CF + 3},(void*)& Cyc__genrep_485};
static unsigned char _tmp2D0[3]="tl";static struct _tuple5 Cyc__gentuple_492={
offsetof(struct Cyc_List_List,tl),{_tmp2D0,_tmp2D0,_tmp2D0 + 3},(void*)& Cyc__genrep_484};
static struct _tuple5*Cyc__genarr_493[2]={& Cyc__gentuple_491,& Cyc__gentuple_492};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_494,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_493),(void*)((struct _tuple5**)Cyc__genarr_493),(void*)((
struct _tuple5**)Cyc__genarr_493 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_484={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep)};static
struct _tuple6 Cyc__gentuple_495={offsetof(struct _tuple49,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_496={offsetof(struct _tuple49,f2),(void*)& Cyc__genrep_77};
static struct _tuple6 Cyc__gentuple_497={offsetof(struct _tuple49,f3),(void*)& Cyc__genrep_484};
static struct _tuple6*Cyc__genarr_498[3]={& Cyc__gentuple_495,& Cyc__gentuple_496,&
Cyc__gentuple_497};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_483={4,
sizeof(struct _tuple49),{(void*)((struct _tuple6**)Cyc__genarr_498),(void*)((
struct _tuple6**)Cyc__genarr_498),(void*)((struct _tuple6**)Cyc__genarr_498 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_472;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_474;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep;
static unsigned char _tmp2D4[5]="List";static struct _tagged_arr Cyc__genname_478={
_tmp2D4,_tmp2D4,_tmp2D4 + 5};static unsigned char _tmp2D5[3]="hd";static struct
_tuple5 Cyc__gentuple_475={offsetof(struct Cyc_List_List,hd),{_tmp2D5,_tmp2D5,
_tmp2D5 + 3},(void*)& Cyc__genrep_77};static unsigned char _tmp2D6[3]="tl";static
struct _tuple5 Cyc__gentuple_476={offsetof(struct Cyc_List_List,tl),{_tmp2D6,
_tmp2D6,_tmp2D6 + 3},(void*)& Cyc__genrep_474};static struct _tuple5*Cyc__genarr_477[
2]={& Cyc__gentuple_475,& Cyc__gentuple_476};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_478,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_477),(void*)((struct _tuple5**)Cyc__genarr_477),(void*)((
struct _tuple5**)Cyc__genarr_477 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_474={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_473;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_473={1,1,(void*)((
void*)& Cyc__genrep_225)};struct _tuple50{unsigned int f1;struct Cyc_List_List*f2;
struct Cyc_Absyn_Switch_clause**f3;};static struct _tuple6 Cyc__gentuple_479={
offsetof(struct _tuple50,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_480={
offsetof(struct _tuple50,f2),(void*)& Cyc__genrep_474};static struct _tuple6 Cyc__gentuple_481={
offsetof(struct _tuple50,f3),(void*)& Cyc__genrep_473};static struct _tuple6*Cyc__genarr_482[
3]={& Cyc__gentuple_479,& Cyc__gentuple_480,& Cyc__gentuple_481};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_472={4,sizeof(struct _tuple50),{(void*)((struct _tuple6**)Cyc__genarr_482),(
void*)((struct _tuple6**)Cyc__genarr_482),(void*)((struct _tuple6**)Cyc__genarr_482
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_467;struct _tuple51{
unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};static struct
_tuple6 Cyc__gentuple_468={offsetof(struct _tuple51,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_469={offsetof(struct _tuple51,f2),(void*)& Cyc__genrep_1};
static struct _tuple6 Cyc__gentuple_470={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_471[3]={& Cyc__gentuple_468,& Cyc__gentuple_469,&
Cyc__gentuple_470};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_467={4,
sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_471),(void*)((
struct _tuple6**)Cyc__genarr_471),(void*)((struct _tuple6**)Cyc__genarr_471 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_463;static struct _tuple6 Cyc__gentuple_464={
offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_465={
offsetof(struct _tuple32,f2),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_466[
2]={& Cyc__gentuple_464,& Cyc__gentuple_465};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_463={4,sizeof(struct _tuple32),{(void*)((struct _tuple6**)Cyc__genarr_466),(
void*)((struct _tuple6**)Cyc__genarr_466),(void*)((struct _tuple6**)Cyc__genarr_466
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_458;static struct _tuple6
Cyc__gentuple_459={offsetof(struct _tuple33,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_460={offsetof(struct _tuple33,f2),(void*)& Cyc__genrep_12};
static struct _tuple6 Cyc__gentuple_461={offsetof(struct _tuple33,f3),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_462[3]={& Cyc__gentuple_459,& Cyc__gentuple_460,&
Cyc__gentuple_461};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_458={4,
sizeof(struct _tuple33),{(void*)((struct _tuple6**)Cyc__genarr_462),(void*)((
struct _tuple6**)Cyc__genarr_462),(void*)((struct _tuple6**)Cyc__genarr_462 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_453;struct _tuple52{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static struct _tuple6 Cyc__gentuple_454={
offsetof(struct _tuple52,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_455={
offsetof(struct _tuple52,f2),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_456={
offsetof(struct _tuple52,f3),(void*)& Cyc__genrep_168};static struct _tuple6*Cyc__genarr_457[
3]={& Cyc__gentuple_454,& Cyc__gentuple_455,& Cyc__gentuple_456};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_453={4,sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_457),(
void*)((struct _tuple6**)Cyc__genarr_457),(void*)((struct _tuple6**)Cyc__genarr_457
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_223;struct _tuple53{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_449={offsetof(struct _tuple53,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_450={offsetof(struct _tuple53,f2),(void*)& Cyc__genrep_159};
static struct _tuple6 Cyc__gentuple_451={offsetof(struct _tuple53,f3),(void*)& Cyc__genrep_224};
static struct _tuple6*Cyc__genarr_452[3]={& Cyc__gentuple_449,& Cyc__gentuple_450,&
Cyc__gentuple_451};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_223={4,
sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_452),(void*)((
struct _tuple6**)Cyc__genarr_452),(void*)((struct _tuple6**)Cyc__genarr_452 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_181;struct _tuple54{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;struct Cyc_Absyn_Stmt*f4;};
static struct _tuple6 Cyc__gentuple_218={offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_219={offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_182};
static struct _tuple6 Cyc__gentuple_220={offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_131};
static struct _tuple6 Cyc__gentuple_221={offsetof(struct _tuple54,f4),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_222[4]={& Cyc__gentuple_218,& Cyc__gentuple_219,&
Cyc__gentuple_220,& Cyc__gentuple_221};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_181={
4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_222),(void*)((
struct _tuple6**)Cyc__genarr_222),(void*)((struct _tuple6**)Cyc__genarr_222 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_167;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_forarray_info_t_rep;static unsigned char _tmp2E1[13]="ForArrayInfo";
static struct _tagged_arr Cyc__genname_177={_tmp2E1,_tmp2E1,_tmp2E1 + 13};static
unsigned char _tmp2E2[6]="defns";static struct _tuple5 Cyc__gentuple_172={offsetof(
struct Cyc_Absyn_ForArrayInfo,defns),{_tmp2E2,_tmp2E2,_tmp2E2 + 6},(void*)& Cyc__genrep_130};
static unsigned char _tmp2E3[10]="condition";static struct _tuple5 Cyc__gentuple_173={
offsetof(struct Cyc_Absyn_ForArrayInfo,condition),{_tmp2E3,_tmp2E3,_tmp2E3 + 10},(
void*)& Cyc__genrep_168};static unsigned char _tmp2E4[6]="delta";static struct
_tuple5 Cyc__gentuple_174={offsetof(struct Cyc_Absyn_ForArrayInfo,delta),{_tmp2E4,
_tmp2E4,_tmp2E4 + 6},(void*)& Cyc__genrep_168};static unsigned char _tmp2E5[5]="body";
static struct _tuple5 Cyc__gentuple_175={offsetof(struct Cyc_Absyn_ForArrayInfo,body),{
_tmp2E5,_tmp2E5,_tmp2E5 + 5},(void*)& Cyc__genrep_159};static struct _tuple5*Cyc__genarr_176[
4]={& Cyc__gentuple_172,& Cyc__gentuple_173,& Cyc__gentuple_174,& Cyc__gentuple_175};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_forarray_info_t_rep={3,(struct
_tagged_arr*)& Cyc__genname_177,sizeof(struct Cyc_Absyn_ForArrayInfo),{(void*)((
struct _tuple5**)Cyc__genarr_176),(void*)((struct _tuple5**)Cyc__genarr_176),(void*)((
struct _tuple5**)Cyc__genarr_176 + 4)}};struct _tuple55{unsigned int f1;struct Cyc_Absyn_ForArrayInfo
f2;};static struct _tuple6 Cyc__gentuple_178={offsetof(struct _tuple55,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_179={offsetof(struct _tuple55,f2),(
void*)& Cyc_Absyn_forarray_info_t_rep};static struct _tuple6*Cyc__genarr_180[2]={&
Cyc__gentuple_178,& Cyc__gentuple_179};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_167={
4,sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_180),(void*)((
struct _tuple6**)Cyc__genarr_180),(void*)((struct _tuple6**)Cyc__genarr_180 + 2)}};
static unsigned char _tmp2E8[7]="Skip_s";static struct _tuple7 Cyc__gentuple_165={0,{
_tmp2E8,_tmp2E8,_tmp2E8 + 7}};static struct _tuple7*Cyc__genarr_166[1]={& Cyc__gentuple_165};
static unsigned char _tmp2E9[6]="Exp_s";static struct _tuple5 Cyc__gentuple_541={0,{
_tmp2E9,_tmp2E9,_tmp2E9 + 6},(void*)& Cyc__genrep_76};static unsigned char _tmp2EA[6]="Seq_s";
static struct _tuple5 Cyc__gentuple_542={1,{_tmp2EA,_tmp2EA,_tmp2EA + 6},(void*)& Cyc__genrep_536};
static unsigned char _tmp2EB[9]="Return_s";static struct _tuple5 Cyc__gentuple_543={2,{
_tmp2EB,_tmp2EB,_tmp2EB + 9},(void*)& Cyc__genrep_532};static unsigned char _tmp2EC[
13]="IfThenElse_s";static struct _tuple5 Cyc__gentuple_544={3,{_tmp2EC,_tmp2EC,
_tmp2EC + 13},(void*)& Cyc__genrep_526};static unsigned char _tmp2ED[8]="While_s";
static struct _tuple5 Cyc__gentuple_545={4,{_tmp2ED,_tmp2ED,_tmp2ED + 8},(void*)& Cyc__genrep_521};
static unsigned char _tmp2EE[8]="Break_s";static struct _tuple5 Cyc__gentuple_546={5,{
_tmp2EE,_tmp2EE,_tmp2EE + 8},(void*)& Cyc__genrep_517};static unsigned char _tmp2EF[
11]="Continue_s";static struct _tuple5 Cyc__gentuple_547={6,{_tmp2EF,_tmp2EF,
_tmp2EF + 11},(void*)& Cyc__genrep_517};static unsigned char _tmp2F0[7]="Goto_s";
static struct _tuple5 Cyc__gentuple_548={7,{_tmp2F0,_tmp2F0,_tmp2F0 + 7},(void*)& Cyc__genrep_511};
static unsigned char _tmp2F1[6]="For_s";static struct _tuple5 Cyc__gentuple_549={8,{
_tmp2F1,_tmp2F1,_tmp2F1 + 6},(void*)& Cyc__genrep_504};static unsigned char _tmp2F2[
9]="Switch_s";static struct _tuple5 Cyc__gentuple_550={9,{_tmp2F2,_tmp2F2,_tmp2F2 + 
9},(void*)& Cyc__genrep_499};static unsigned char _tmp2F3[10]="SwitchC_s";static
struct _tuple5 Cyc__gentuple_551={10,{_tmp2F3,_tmp2F3,_tmp2F3 + 10},(void*)& Cyc__genrep_483};
static unsigned char _tmp2F4[11]="Fallthru_s";static struct _tuple5 Cyc__gentuple_552={
11,{_tmp2F4,_tmp2F4,_tmp2F4 + 11},(void*)& Cyc__genrep_472};static unsigned char
_tmp2F5[7]="Decl_s";static struct _tuple5 Cyc__gentuple_553={12,{_tmp2F5,_tmp2F5,
_tmp2F5 + 7},(void*)& Cyc__genrep_467};static unsigned char _tmp2F6[6]="Cut_s";
static struct _tuple5 Cyc__gentuple_554={13,{_tmp2F6,_tmp2F6,_tmp2F6 + 6},(void*)&
Cyc__genrep_463};static unsigned char _tmp2F7[9]="Splice_s";static struct _tuple5 Cyc__gentuple_555={
14,{_tmp2F7,_tmp2F7,_tmp2F7 + 9},(void*)& Cyc__genrep_463};static unsigned char
_tmp2F8[8]="Label_s";static struct _tuple5 Cyc__gentuple_556={15,{_tmp2F8,_tmp2F8,
_tmp2F8 + 8},(void*)& Cyc__genrep_458};static unsigned char _tmp2F9[5]="Do_s";static
struct _tuple5 Cyc__gentuple_557={16,{_tmp2F9,_tmp2F9,_tmp2F9 + 5},(void*)& Cyc__genrep_453};
static unsigned char _tmp2FA[11]="TryCatch_s";static struct _tuple5 Cyc__gentuple_558={
17,{_tmp2FA,_tmp2FA,_tmp2FA + 11},(void*)& Cyc__genrep_223};static unsigned char
_tmp2FB[9]="Region_s";static struct _tuple5 Cyc__gentuple_559={18,{_tmp2FB,_tmp2FB,
_tmp2FB + 9},(void*)& Cyc__genrep_181};static unsigned char _tmp2FC[11]="ForArray_s";
static struct _tuple5 Cyc__gentuple_560={19,{_tmp2FC,_tmp2FC,_tmp2FC + 11},(void*)&
Cyc__genrep_167};static struct _tuple5*Cyc__genarr_561[20]={& Cyc__gentuple_541,&
Cyc__gentuple_542,& Cyc__gentuple_543,& Cyc__gentuple_544,& Cyc__gentuple_545,& Cyc__gentuple_546,&
Cyc__gentuple_547,& Cyc__gentuple_548,& Cyc__gentuple_549,& Cyc__gentuple_550,& Cyc__gentuple_551,&
Cyc__gentuple_552,& Cyc__gentuple_553,& Cyc__gentuple_554,& Cyc__gentuple_555,& Cyc__gentuple_556,&
Cyc__gentuple_557,& Cyc__gentuple_558,& Cyc__gentuple_559,& Cyc__gentuple_560};
static unsigned char _tmp2FE[9]="Raw_stmt";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep={
5,{_tmp2FE,_tmp2FE,_tmp2FE + 9},{(void*)((struct _tuple7**)Cyc__genarr_166),(void*)((
struct _tuple7**)Cyc__genarr_166),(void*)((struct _tuple7**)Cyc__genarr_166 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_561),(void*)((struct _tuple5**)Cyc__genarr_561),(
void*)((struct _tuple5**)Cyc__genarr_561 + 20)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_160;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep;
static unsigned char _tmp2FF[5]="List";static struct _tagged_arr Cyc__genname_164={
_tmp2FF,_tmp2FF,_tmp2FF + 5};static unsigned char _tmp300[3]="hd";static struct
_tuple5 Cyc__gentuple_161={offsetof(struct Cyc_List_List,hd),{_tmp300,_tmp300,
_tmp300 + 3},(void*)& Cyc__genrep_159};static unsigned char _tmp301[3]="tl";static
struct _tuple5 Cyc__gentuple_162={offsetof(struct Cyc_List_List,tl),{_tmp301,
_tmp301,_tmp301 + 3},(void*)& Cyc__genrep_160};static struct _tuple5*Cyc__genarr_163[
2]={& Cyc__gentuple_161,& Cyc__gentuple_162};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_164,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_163),(void*)((struct _tuple5**)Cyc__genarr_163),(void*)((
struct _tuple5**)Cyc__genarr_163 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_160={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple8*Cyc__genarr_74[0]={};static
unsigned char _tmp305[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
6,{_tmp305,_tmp305,_tmp305 + 11},{(void*)((struct _tuple8**)Cyc__genarr_74),(void*)((
struct _tuple8**)Cyc__genarr_74),(void*)((struct _tuple8**)Cyc__genarr_74 + 0)}};
static unsigned char _tmp306[5]="Stmt";static struct _tagged_arr Cyc__genname_568={
_tmp306,_tmp306,_tmp306 + 5};static unsigned char _tmp307[2]="r";static struct
_tuple5 Cyc__gentuple_562={offsetof(struct Cyc_Absyn_Stmt,r),{_tmp307,_tmp307,
_tmp307 + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};static unsigned char _tmp308[4]="loc";
static struct _tuple5 Cyc__gentuple_563={offsetof(struct Cyc_Absyn_Stmt,loc),{
_tmp308,_tmp308,_tmp308 + 4},(void*)& Cyc__genrep_2};static unsigned char _tmp309[16]="non_local_preds";
static struct _tuple5 Cyc__gentuple_564={offsetof(struct Cyc_Absyn_Stmt,non_local_preds),{
_tmp309,_tmp309,_tmp309 + 16},(void*)& Cyc__genrep_160};static unsigned char _tmp30A[
10]="try_depth";static struct _tuple5 Cyc__gentuple_565={offsetof(struct Cyc_Absyn_Stmt,try_depth),{
_tmp30A,_tmp30A,_tmp30A + 10},(void*)((void*)& Cyc__genrep_102)};static
unsigned char _tmp30B[6]="annot";static struct _tuple5 Cyc__gentuple_566={offsetof(
struct Cyc_Absyn_Stmt,annot),{_tmp30B,_tmp30B,_tmp30B + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};
static struct _tuple5*Cyc__genarr_567[5]={& Cyc__gentuple_562,& Cyc__gentuple_563,&
Cyc__gentuple_564,& Cyc__gentuple_565,& Cyc__gentuple_566};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep={3,(struct _tagged_arr*)& Cyc__genname_568,sizeof(struct
Cyc_Absyn_Stmt),{(void*)((struct _tuple5**)Cyc__genarr_567),(void*)((struct
_tuple5**)Cyc__genarr_567),(void*)((struct _tuple5**)Cyc__genarr_567 + 5)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159={1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};
static unsigned char _tmp30E[7]="Fndecl";static struct _tagged_arr Cyc__genname_616={
_tmp30E,_tmp30E,_tmp30E + 7};static unsigned char _tmp30F[3]="sc";static struct
_tuple5 Cyc__gentuple_601={offsetof(struct Cyc_Absyn_Fndecl,sc),{_tmp30F,_tmp30F,
_tmp30F + 3},(void*)& Cyc_Absyn_scope_t_rep};static unsigned char _tmp310[10]="is_inline";
static struct _tuple5 Cyc__gentuple_602={offsetof(struct Cyc_Absyn_Fndecl,is_inline),{
_tmp310,_tmp310,_tmp310 + 10},(void*)((void*)& Cyc__genrep_102)};static
unsigned char _tmp311[5]="name";static struct _tuple5 Cyc__gentuple_603={offsetof(
struct Cyc_Absyn_Fndecl,name),{_tmp311,_tmp311,_tmp311 + 5},(void*)& Cyc__genrep_10};
static unsigned char _tmp312[4]="tvs";static struct _tuple5 Cyc__gentuple_604={
offsetof(struct Cyc_Absyn_Fndecl,tvs),{_tmp312,_tmp312,_tmp312 + 4},(void*)& Cyc__genrep_236};
static unsigned char _tmp313[7]="effect";static struct _tuple5 Cyc__gentuple_605={
offsetof(struct Cyc_Absyn_Fndecl,effect),{_tmp313,_tmp313,_tmp313 + 7},(void*)& Cyc__genrep_61};
static unsigned char _tmp314[9]="ret_type";static struct _tuple5 Cyc__gentuple_606={
offsetof(struct Cyc_Absyn_Fndecl,ret_type),{_tmp314,_tmp314,_tmp314 + 9},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static unsigned char _tmp315[5]="args";static struct
_tuple5 Cyc__gentuple_607={offsetof(struct Cyc_Absyn_Fndecl,args),{_tmp315,_tmp315,
_tmp315 + 5},(void*)& Cyc__genrep_590};static unsigned char _tmp316[10]="c_varargs";
static struct _tuple5 Cyc__gentuple_608={offsetof(struct Cyc_Absyn_Fndecl,c_varargs),{
_tmp316,_tmp316,_tmp316 + 10},(void*)((void*)& Cyc__genrep_102)};static
unsigned char _tmp317[12]="cyc_varargs";static struct _tuple5 Cyc__gentuple_609={
offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),{_tmp317,_tmp317,_tmp317 + 12},(void*)&
Cyc__genrep_579};static unsigned char _tmp318[7]="rgn_po";static struct _tuple5 Cyc__gentuple_610={
offsetof(struct Cyc_Absyn_Fndecl,rgn_po),{_tmp318,_tmp318,_tmp318 + 7},(void*)& Cyc__genrep_569};
static unsigned char _tmp319[5]="body";static struct _tuple5 Cyc__gentuple_611={
offsetof(struct Cyc_Absyn_Fndecl,body),{_tmp319,_tmp319,_tmp319 + 5},(void*)& Cyc__genrep_159};
static unsigned char _tmp31A[11]="cached_typ";static struct _tuple5 Cyc__gentuple_612={
offsetof(struct Cyc_Absyn_Fndecl,cached_typ),{_tmp31A,_tmp31A,_tmp31A + 11},(void*)&
Cyc__genrep_61};static unsigned char _tmp31B[15]="param_vardecls";static struct
_tuple5 Cyc__gentuple_613={offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),{
_tmp31B,_tmp31B,_tmp31B + 15},(void*)& Cyc__genrep_129};static unsigned char _tmp31C[
11]="attributes";static struct _tuple5 Cyc__gentuple_614={offsetof(struct Cyc_Absyn_Fndecl,attributes),{
_tmp31C,_tmp31C,_tmp31C + 11},(void*)& Cyc__genrep_83};static struct _tuple5*Cyc__genarr_615[
14]={& Cyc__gentuple_601,& Cyc__gentuple_602,& Cyc__gentuple_603,& Cyc__gentuple_604,&
Cyc__gentuple_605,& Cyc__gentuple_606,& Cyc__gentuple_607,& Cyc__gentuple_608,& Cyc__gentuple_609,&
Cyc__gentuple_610,& Cyc__gentuple_611,& Cyc__gentuple_612,& Cyc__gentuple_613,& Cyc__gentuple_614};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep={3,(struct _tagged_arr*)&
Cyc__genname_616,sizeof(struct Cyc_Absyn_Fndecl),{(void*)((struct _tuple5**)Cyc__genarr_615),(
void*)((struct _tuple5**)Cyc__genarr_615),(void*)((struct _tuple5**)Cyc__genarr_615
+ 14)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_82={1,1,(void*)((void*)&
Cyc_struct_Absyn_Fndecl_rep)};struct _tuple56{unsigned int f1;struct Cyc_Absyn_Fndecl*
f2;};static struct _tuple6 Cyc__gentuple_617={offsetof(struct _tuple56,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_618={offsetof(struct _tuple56,f2),(
void*)& Cyc__genrep_82};static struct _tuple6*Cyc__genarr_619[2]={& Cyc__gentuple_617,&
Cyc__gentuple_618};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_81={4,sizeof(
struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_619),(void*)((struct
_tuple6**)Cyc__genarr_619),(void*)((struct _tuple6**)Cyc__genarr_619 + 2)}};static
unsigned char _tmp320[13]="Unresolved_b";static struct _tuple7 Cyc__gentuple_824={0,{
_tmp320,_tmp320,_tmp320 + 13}};static struct _tuple7*Cyc__genarr_825[1]={& Cyc__gentuple_824};
static unsigned char _tmp321[9]="Global_b";static struct _tuple5 Cyc__gentuple_826={0,{
_tmp321,_tmp321,_tmp321 + 9},(void*)& Cyc__genrep_394};static unsigned char _tmp322[
10]="Funname_b";static struct _tuple5 Cyc__gentuple_827={1,{_tmp322,_tmp322,_tmp322
+ 10},(void*)& Cyc__genrep_81};static unsigned char _tmp323[8]="Param_b";static
struct _tuple5 Cyc__gentuple_828={2,{_tmp323,_tmp323,_tmp323 + 8},(void*)& Cyc__genrep_394};
static unsigned char _tmp324[8]="Local_b";static struct _tuple5 Cyc__gentuple_829={3,{
_tmp324,_tmp324,_tmp324 + 8},(void*)& Cyc__genrep_394};static unsigned char _tmp325[
6]="Pat_b";static struct _tuple5 Cyc__gentuple_830={4,{_tmp325,_tmp325,_tmp325 + 6},(
void*)& Cyc__genrep_394};static struct _tuple5*Cyc__genarr_831[5]={& Cyc__gentuple_826,&
Cyc__gentuple_827,& Cyc__gentuple_828,& Cyc__gentuple_829,& Cyc__gentuple_830};
static unsigned char _tmp327[8]="Binding";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep={
5,{_tmp327,_tmp327,_tmp327 + 8},{(void*)((struct _tuple7**)Cyc__genarr_825),(void*)((
struct _tuple7**)Cyc__genarr_825),(void*)((struct _tuple7**)Cyc__genarr_825 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_831),(void*)((struct _tuple5**)Cyc__genarr_831),(
void*)((struct _tuple5**)Cyc__genarr_831 + 5)}};struct _tuple57{unsigned int f1;
struct _tuple0*f2;void*f3;};static struct _tuple6 Cyc__gentuple_832={offsetof(struct
_tuple57,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_833={
offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_834={
offsetof(struct _tuple57,f3),(void*)& Cyc_Absyn_binding_t_rep};static struct _tuple6*
Cyc__genarr_835[3]={& Cyc__gentuple_832,& Cyc__gentuple_833,& Cyc__gentuple_834};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_823={4,sizeof(struct _tuple57),{(
void*)((struct _tuple6**)Cyc__genarr_835),(void*)((struct _tuple6**)Cyc__genarr_835),(
void*)((struct _tuple6**)Cyc__genarr_835 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_818;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep;
static unsigned char _tmp329[5]="Plus";static struct _tuple7 Cyc__gentuple_788={0,{
_tmp329,_tmp329,_tmp329 + 5}};static unsigned char _tmp32A[6]="Times";static struct
_tuple7 Cyc__gentuple_789={1,{_tmp32A,_tmp32A,_tmp32A + 6}};static unsigned char
_tmp32B[6]="Minus";static struct _tuple7 Cyc__gentuple_790={2,{_tmp32B,_tmp32B,
_tmp32B + 6}};static unsigned char _tmp32C[4]="Div";static struct _tuple7 Cyc__gentuple_791={
3,{_tmp32C,_tmp32C,_tmp32C + 4}};static unsigned char _tmp32D[4]="Mod";static struct
_tuple7 Cyc__gentuple_792={4,{_tmp32D,_tmp32D,_tmp32D + 4}};static unsigned char
_tmp32E[3]="Eq";static struct _tuple7 Cyc__gentuple_793={5,{_tmp32E,_tmp32E,_tmp32E
+ 3}};static unsigned char _tmp32F[4]="Neq";static struct _tuple7 Cyc__gentuple_794={
6,{_tmp32F,_tmp32F,_tmp32F + 4}};static unsigned char _tmp330[3]="Gt";static struct
_tuple7 Cyc__gentuple_795={7,{_tmp330,_tmp330,_tmp330 + 3}};static unsigned char
_tmp331[3]="Lt";static struct _tuple7 Cyc__gentuple_796={8,{_tmp331,_tmp331,_tmp331
+ 3}};static unsigned char _tmp332[4]="Gte";static struct _tuple7 Cyc__gentuple_797={
9,{_tmp332,_tmp332,_tmp332 + 4}};static unsigned char _tmp333[4]="Lte";static struct
_tuple7 Cyc__gentuple_798={10,{_tmp333,_tmp333,_tmp333 + 4}};static unsigned char
_tmp334[4]="Not";static struct _tuple7 Cyc__gentuple_799={11,{_tmp334,_tmp334,
_tmp334 + 4}};static unsigned char _tmp335[7]="Bitnot";static struct _tuple7 Cyc__gentuple_800={
12,{_tmp335,_tmp335,_tmp335 + 7}};static unsigned char _tmp336[7]="Bitand";static
struct _tuple7 Cyc__gentuple_801={13,{_tmp336,_tmp336,_tmp336 + 7}};static
unsigned char _tmp337[6]="Bitor";static struct _tuple7 Cyc__gentuple_802={14,{
_tmp337,_tmp337,_tmp337 + 6}};static unsigned char _tmp338[7]="Bitxor";static struct
_tuple7 Cyc__gentuple_803={15,{_tmp338,_tmp338,_tmp338 + 7}};static unsigned char
_tmp339[10]="Bitlshift";static struct _tuple7 Cyc__gentuple_804={16,{_tmp339,
_tmp339,_tmp339 + 10}};static unsigned char _tmp33A[11]="Bitlrshift";static struct
_tuple7 Cyc__gentuple_805={17,{_tmp33A,_tmp33A,_tmp33A + 11}};static unsigned char
_tmp33B[11]="Bitarshift";static struct _tuple7 Cyc__gentuple_806={18,{_tmp33B,
_tmp33B,_tmp33B + 11}};static unsigned char _tmp33C[5]="Size";static struct _tuple7
Cyc__gentuple_807={19,{_tmp33C,_tmp33C,_tmp33C + 5}};static struct _tuple7*Cyc__genarr_808[
20]={& Cyc__gentuple_788,& Cyc__gentuple_789,& Cyc__gentuple_790,& Cyc__gentuple_791,&
Cyc__gentuple_792,& Cyc__gentuple_793,& Cyc__gentuple_794,& Cyc__gentuple_795,& Cyc__gentuple_796,&
Cyc__gentuple_797,& Cyc__gentuple_798,& Cyc__gentuple_799,& Cyc__gentuple_800,& Cyc__gentuple_801,&
Cyc__gentuple_802,& Cyc__gentuple_803,& Cyc__gentuple_804,& Cyc__gentuple_805,& Cyc__gentuple_806,&
Cyc__gentuple_807};static struct _tuple5*Cyc__genarr_809[0]={};static unsigned char
_tmp33E[7]="Primop";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={5,{
_tmp33E,_tmp33E,_tmp33E + 7},{(void*)((struct _tuple7**)Cyc__genarr_808),(void*)((
struct _tuple7**)Cyc__genarr_808),(void*)((struct _tuple7**)Cyc__genarr_808 + 20)},{(
void*)((struct _tuple5**)Cyc__genarr_809),(void*)((struct _tuple5**)Cyc__genarr_809),(
void*)((struct _tuple5**)Cyc__genarr_809 + 0)}};struct _tuple58{unsigned int f1;void*
f2;struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_819={offsetof(
struct _tuple58,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_820={
offsetof(struct _tuple58,f2),(void*)& Cyc_Absyn_primop_t_rep};static struct _tuple6
Cyc__gentuple_821={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_474};static
struct _tuple6*Cyc__genarr_822[3]={& Cyc__gentuple_819,& Cyc__gentuple_820,& Cyc__gentuple_821};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_818={4,sizeof(struct _tuple58),{(
void*)((struct _tuple6**)Cyc__genarr_822),(void*)((struct _tuple6**)Cyc__genarr_822),(
void*)((struct _tuple6**)Cyc__genarr_822 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_786;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_787;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;static
unsigned char _tmp340[4]="Opt";static struct _tagged_arr Cyc__genname_812={_tmp340,
_tmp340,_tmp340 + 4};static unsigned char _tmp341[2]="v";static struct _tuple5 Cyc__gentuple_810={
offsetof(struct Cyc_Core_Opt,v),{_tmp341,_tmp341,_tmp341 + 2},(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple5*Cyc__genarr_811[1]={& Cyc__gentuple_810};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_primop_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_812,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_811),(void*)((
struct _tuple5**)Cyc__genarr_811),(void*)((struct _tuple5**)Cyc__genarr_811 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_787={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple59{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_813={offsetof(struct
_tuple59,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_814={
offsetof(struct _tuple59,f2),(void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_815={
offsetof(struct _tuple59,f3),(void*)& Cyc__genrep_787};static struct _tuple6 Cyc__gentuple_816={
offsetof(struct _tuple59,f4),(void*)& Cyc__genrep_77};static struct _tuple6*Cyc__genarr_817[
4]={& Cyc__gentuple_813,& Cyc__gentuple_814,& Cyc__gentuple_815,& Cyc__gentuple_816};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_786={4,sizeof(struct _tuple59),{(
void*)((struct _tuple6**)Cyc__genarr_817),(void*)((struct _tuple6**)Cyc__genarr_817),(
void*)((struct _tuple6**)Cyc__genarr_817 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_775;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep;
static unsigned char _tmp345[7]="PreInc";static struct _tuple7 Cyc__gentuple_776={0,{
_tmp345,_tmp345,_tmp345 + 7}};static unsigned char _tmp346[8]="PostInc";static
struct _tuple7 Cyc__gentuple_777={1,{_tmp346,_tmp346,_tmp346 + 8}};static
unsigned char _tmp347[7]="PreDec";static struct _tuple7 Cyc__gentuple_778={2,{
_tmp347,_tmp347,_tmp347 + 7}};static unsigned char _tmp348[8]="PostDec";static
struct _tuple7 Cyc__gentuple_779={3,{_tmp348,_tmp348,_tmp348 + 8}};static struct
_tuple7*Cyc__genarr_780[4]={& Cyc__gentuple_776,& Cyc__gentuple_777,& Cyc__gentuple_778,&
Cyc__gentuple_779};static struct _tuple5*Cyc__genarr_781[0]={};static unsigned char
_tmp34A[12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={
5,{_tmp34A,_tmp34A,_tmp34A + 12},{(void*)((struct _tuple7**)Cyc__genarr_780),(void*)((
struct _tuple7**)Cyc__genarr_780),(void*)((struct _tuple7**)Cyc__genarr_780 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_781),(void*)((struct _tuple5**)Cyc__genarr_781),(
void*)((struct _tuple5**)Cyc__genarr_781 + 0)}};struct _tuple60{unsigned int f1;
struct Cyc_Absyn_Exp*f2;void*f3;};static struct _tuple6 Cyc__gentuple_782={offsetof(
struct _tuple60,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_783={
offsetof(struct _tuple60,f2),(void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_784={
offsetof(struct _tuple60,f3),(void*)& Cyc_Absyn_incrementor_t_rep};static struct
_tuple6*Cyc__genarr_785[3]={& Cyc__gentuple_782,& Cyc__gentuple_783,& Cyc__gentuple_784};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_775={4,sizeof(struct _tuple60),{(
void*)((struct _tuple6**)Cyc__genarr_785),(void*)((struct _tuple6**)Cyc__genarr_785),(
void*)((struct _tuple6**)Cyc__genarr_785 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_769;struct _tuple61{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_770={offsetof(
struct _tuple61,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_771={
offsetof(struct _tuple61,f2),(void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_772={
offsetof(struct _tuple61,f3),(void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_773={
offsetof(struct _tuple61,f4),(void*)& Cyc__genrep_77};static struct _tuple6*Cyc__genarr_774[
4]={& Cyc__gentuple_770,& Cyc__gentuple_771,& Cyc__gentuple_772,& Cyc__gentuple_773};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_769={4,sizeof(struct _tuple61),{(
void*)((struct _tuple6**)Cyc__genarr_774),(void*)((struct _tuple6**)Cyc__genarr_774),(
void*)((struct _tuple6**)Cyc__genarr_774 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_708;struct _tuple62{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;};static struct _tuple6 Cyc__gentuple_709={offsetof(struct _tuple62,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_710={offsetof(struct _tuple62,f2),(
void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_711={offsetof(struct
_tuple62,f3),(void*)& Cyc__genrep_77};static struct _tuple6*Cyc__genarr_712[3]={&
Cyc__gentuple_709,& Cyc__gentuple_710,& Cyc__gentuple_711};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_708={4,sizeof(struct _tuple62),{(void*)((struct _tuple6**)Cyc__genarr_712),(
void*)((struct _tuple6**)Cyc__genarr_712),(void*)((struct _tuple6**)Cyc__genarr_712
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_764;static struct _tuple6
Cyc__gentuple_765={offsetof(struct _tuple49,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_766={offsetof(struct _tuple49,f2),(void*)& Cyc__genrep_77};
static struct _tuple6 Cyc__gentuple_767={offsetof(struct _tuple49,f3),(void*)& Cyc__genrep_474};
static struct _tuple6*Cyc__genarr_768[3]={& Cyc__gentuple_765,& Cyc__gentuple_766,&
Cyc__gentuple_767};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_764={4,
sizeof(struct _tuple49),{(void*)((struct _tuple6**)Cyc__genarr_768),(void*)((
struct _tuple6**)Cyc__genarr_768),(void*)((struct _tuple6**)Cyc__genarr_768 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_751;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_752;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_753;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_753={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};static
unsigned char _tmp350[15]="VarargCallInfo";static struct _tagged_arr Cyc__genname_758={
_tmp350,_tmp350,_tmp350 + 15};static unsigned char _tmp351[12]="num_varargs";static
struct _tuple5 Cyc__gentuple_754={offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),{
_tmp351,_tmp351,_tmp351 + 12},(void*)((void*)& Cyc__genrep_102)};static
unsigned char _tmp352[10]="injectors";static struct _tuple5 Cyc__gentuple_755={
offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),{_tmp352,_tmp352,_tmp352 + 10},(
void*)& Cyc__genrep_291};static unsigned char _tmp353[4]="vai";static struct _tuple5
Cyc__gentuple_756={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),{_tmp353,_tmp353,
_tmp353 + 4},(void*)& Cyc__genrep_753};static struct _tuple5*Cyc__genarr_757[3]={&
Cyc__gentuple_754,& Cyc__gentuple_755,& Cyc__gentuple_756};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_call_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_758,
sizeof(struct Cyc_Absyn_VarargCallInfo),{(void*)((struct _tuple5**)Cyc__genarr_757),(
void*)((struct _tuple5**)Cyc__genarr_757),(void*)((struct _tuple5**)Cyc__genarr_757
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_752={1,1,(void*)((void*)&
Cyc_Absyn_vararg_call_info_t_rep)};struct _tuple63{unsigned int f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_VarargCallInfo*f4;};static struct
_tuple6 Cyc__gentuple_759={offsetof(struct _tuple63,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_760={offsetof(struct _tuple63,f2),(void*)& Cyc__genrep_77};
static struct _tuple6 Cyc__gentuple_761={offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_474};
static struct _tuple6 Cyc__gentuple_762={offsetof(struct _tuple63,f4),(void*)& Cyc__genrep_752};
static struct _tuple6*Cyc__genarr_763[4]={& Cyc__gentuple_759,& Cyc__gentuple_760,&
Cyc__gentuple_761,& Cyc__gentuple_762};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_751={
4,sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_763),(void*)((
struct _tuple6**)Cyc__genarr_763),(void*)((struct _tuple6**)Cyc__genarr_763 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_746;static struct _tuple6 Cyc__gentuple_747={
offsetof(struct _tuple49,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_748={
offsetof(struct _tuple49,f2),(void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_749={
offsetof(struct _tuple49,f3),(void*)& Cyc__genrep_52};static struct _tuple6*Cyc__genarr_750[
3]={& Cyc__gentuple_747,& Cyc__gentuple_748,& Cyc__gentuple_749};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_746={4,sizeof(struct _tuple49),{(void*)((struct _tuple6**)Cyc__genarr_750),(
void*)((struct _tuple6**)Cyc__genarr_750),(void*)((struct _tuple6**)Cyc__genarr_750
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_741;struct _tuple64{
unsigned int f1;void*f2;struct Cyc_Absyn_Exp*f3;};static struct _tuple6 Cyc__gentuple_742={
offsetof(struct _tuple64,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_743={
offsetof(struct _tuple64,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_744={offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_77};
static struct _tuple6*Cyc__genarr_745[3]={& Cyc__gentuple_742,& Cyc__gentuple_743,&
Cyc__gentuple_744};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_741={4,
sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_745),(void*)((
struct _tuple6**)Cyc__genarr_745),(void*)((struct _tuple6**)Cyc__genarr_745 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_736;static struct _tuple6 Cyc__gentuple_737={
offsetof(struct _tuple62,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_738={
offsetof(struct _tuple62,f2),(void*)& Cyc__genrep_73};static struct _tuple6 Cyc__gentuple_739={
offsetof(struct _tuple62,f3),(void*)& Cyc__genrep_77};static struct _tuple6*Cyc__genarr_740[
3]={& Cyc__gentuple_737,& Cyc__gentuple_738,& Cyc__gentuple_739};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_736={4,sizeof(struct _tuple62),{(void*)((struct _tuple6**)Cyc__genarr_740),(
void*)((struct _tuple6**)Cyc__genarr_740),(void*)((struct _tuple6**)Cyc__genarr_740
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_47;static struct _tuple6 Cyc__gentuple_48={
offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_49={
offsetof(struct _tuple6,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_50[2]={& Cyc__gentuple_48,& Cyc__gentuple_49};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_47={4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_50),(
void*)((struct _tuple6**)Cyc__genarr_50),(void*)((struct _tuple6**)Cyc__genarr_50 + 
2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_723;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_725;
struct _tuple65{unsigned int f1;unsigned int f2;};static struct _tuple6 Cyc__gentuple_726={
offsetof(struct _tuple65,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_727={
offsetof(struct _tuple65,f2),(void*)& Cyc__genrep_5};static struct _tuple6*Cyc__genarr_728[
2]={& Cyc__gentuple_726,& Cyc__gentuple_727};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_725={4,sizeof(struct _tuple65),{(void*)((struct _tuple6**)Cyc__genarr_728),(
void*)((struct _tuple6**)Cyc__genarr_728),(void*)((struct _tuple6**)Cyc__genarr_728
+ 2)}};static struct _tuple7*Cyc__genarr_724[0]={};static unsigned char _tmp35C[12]="StructField";
static struct _tuple5 Cyc__gentuple_729={0,{_tmp35C,_tmp35C,_tmp35C + 12},(void*)&
Cyc__genrep_318};static unsigned char _tmp35D[11]="TupleIndex";static struct _tuple5
Cyc__gentuple_730={1,{_tmp35D,_tmp35D,_tmp35D + 11},(void*)& Cyc__genrep_725};
static struct _tuple5*Cyc__genarr_731[2]={& Cyc__gentuple_729,& Cyc__gentuple_730};
static unsigned char _tmp35F[14]="OffsetofField";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep={5,{_tmp35F,_tmp35F,_tmp35F + 14},{(void*)((struct
_tuple7**)Cyc__genarr_724),(void*)((struct _tuple7**)Cyc__genarr_724),(void*)((
struct _tuple7**)Cyc__genarr_724 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_731),(
void*)((struct _tuple5**)Cyc__genarr_731),(void*)((struct _tuple5**)Cyc__genarr_731
+ 2)}};struct _tuple66{unsigned int f1;void*f2;void*f3;};static struct _tuple6 Cyc__gentuple_732={
offsetof(struct _tuple66,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_733={
offsetof(struct _tuple66,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_734={offsetof(struct _tuple66,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};
static struct _tuple6*Cyc__genarr_735[3]={& Cyc__gentuple_732,& Cyc__gentuple_733,&
Cyc__gentuple_734};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_723={4,
sizeof(struct _tuple66),{(void*)((struct _tuple6**)Cyc__genarr_735),(void*)((
struct _tuple6**)Cyc__genarr_735),(void*)((struct _tuple6**)Cyc__genarr_735 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_718;struct _tuple67{unsigned int
f1;struct Cyc_List_List*f2;void*f3;};static struct _tuple6 Cyc__gentuple_719={
offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_720={
offsetof(struct _tuple67,f2),(void*)& Cyc__genrep_236};static struct _tuple6 Cyc__gentuple_721={
offsetof(struct _tuple67,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_722[3]={& Cyc__gentuple_719,& Cyc__gentuple_720,& Cyc__gentuple_721};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_718={4,sizeof(struct _tuple67),{(
void*)((struct _tuple6**)Cyc__genarr_722),(void*)((struct _tuple6**)Cyc__genarr_722),(
void*)((struct _tuple6**)Cyc__genarr_722 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_713;struct _tuple68{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct
_tagged_arr*f3;};static struct _tuple6 Cyc__gentuple_714={offsetof(struct _tuple68,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_715={offsetof(struct
_tuple68,f2),(void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_716={
offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_12};static struct _tuple6*Cyc__genarr_717[
3]={& Cyc__gentuple_714,& Cyc__gentuple_715,& Cyc__gentuple_716};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_713={4,sizeof(struct _tuple68),{(void*)((struct _tuple6**)Cyc__genarr_717),(
void*)((struct _tuple6**)Cyc__genarr_717),(void*)((struct _tuple6**)Cyc__genarr_717
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_704;static struct _tuple6
Cyc__gentuple_705={offsetof(struct _tuple11,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_706={offsetof(struct _tuple11,f2),(void*)& Cyc__genrep_474};
static struct _tuple6*Cyc__genarr_707[2]={& Cyc__gentuple_705,& Cyc__gentuple_706};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_704={4,sizeof(struct _tuple11),{(
void*)((struct _tuple6**)Cyc__genarr_707),(void*)((struct _tuple6**)Cyc__genarr_707),(
void*)((struct _tuple6**)Cyc__genarr_707 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_693;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_694;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_695;static struct _tuple6 Cyc__gentuple_696={
offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_580};static struct _tuple6 Cyc__gentuple_697={
offsetof(struct _tuple1,f2),(void*)& Cyc__genrep_132};static struct _tuple6 Cyc__gentuple_698={
offsetof(struct _tuple1,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_699[3]={& Cyc__gentuple_696,& Cyc__gentuple_697,& Cyc__gentuple_698};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_695={4,sizeof(struct _tuple1),{(
void*)((struct _tuple6**)Cyc__genarr_699),(void*)((struct _tuple6**)Cyc__genarr_699),(
void*)((struct _tuple6**)Cyc__genarr_699 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_694={1,1,(void*)((void*)& Cyc__genrep_695)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_621;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_622;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_623;static struct _tuple6 Cyc__gentuple_624={offsetof(struct _tuple10,f1),(
void*)& Cyc__genrep_316};static struct _tuple6 Cyc__gentuple_625={offsetof(struct
_tuple10,f2),(void*)& Cyc__genrep_77};static struct _tuple6*Cyc__genarr_626[2]={&
Cyc__gentuple_624,& Cyc__gentuple_625};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_623={
4,sizeof(struct _tuple10),{(void*)((struct _tuple6**)Cyc__genarr_626),(void*)((
struct _tuple6**)Cyc__genarr_626),(void*)((struct _tuple6**)Cyc__genarr_626 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_622={1,1,(void*)((void*)& Cyc__genrep_623)};
static unsigned char _tmp368[5]="List";static struct _tagged_arr Cyc__genname_630={
_tmp368,_tmp368,_tmp368 + 5};static unsigned char _tmp369[3]="hd";static struct
_tuple5 Cyc__gentuple_627={offsetof(struct Cyc_List_List,hd),{_tmp369,_tmp369,
_tmp369 + 3},(void*)& Cyc__genrep_622};static unsigned char _tmp36A[3]="tl";static
struct _tuple5 Cyc__gentuple_628={offsetof(struct Cyc_List_List,tl),{_tmp36A,
_tmp36A,_tmp36A + 3},(void*)& Cyc__genrep_621};static struct _tuple5*Cyc__genarr_629[
2]={& Cyc__gentuple_627,& Cyc__gentuple_628};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_630,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_629),(void*)((struct _tuple5**)Cyc__genarr_629),(void*)((
struct _tuple5**)Cyc__genarr_629 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_621={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple69{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_700={offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_701={offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_694};
static struct _tuple6 Cyc__gentuple_702={offsetof(struct _tuple69,f3),(void*)& Cyc__genrep_621};
static struct _tuple6*Cyc__genarr_703[3]={& Cyc__gentuple_700,& Cyc__gentuple_701,&
Cyc__gentuple_702};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_693={4,
sizeof(struct _tuple69),{(void*)((struct _tuple6**)Cyc__genarr_703),(void*)((
struct _tuple6**)Cyc__genarr_703),(void*)((struct _tuple6**)Cyc__genarr_703 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_689;static struct _tuple6 Cyc__gentuple_690={
offsetof(struct _tuple11,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_691={
offsetof(struct _tuple11,f2),(void*)& Cyc__genrep_621};static struct _tuple6*Cyc__genarr_692[
2]={& Cyc__gentuple_690,& Cyc__gentuple_691};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_689={4,sizeof(struct _tuple11),{(void*)((struct _tuple6**)Cyc__genarr_692),(
void*)((struct _tuple6**)Cyc__genarr_692),(void*)((struct _tuple6**)Cyc__genarr_692
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_683;struct _tuple70{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_684={offsetof(struct _tuple70,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_685={offsetof(struct _tuple70,f2),(
void*)& Cyc__genrep_131};static struct _tuple6 Cyc__gentuple_686={offsetof(struct
_tuple70,f3),(void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_687={
offsetof(struct _tuple70,f4),(void*)& Cyc__genrep_77};static struct _tuple6*Cyc__genarr_688[
4]={& Cyc__gentuple_684,& Cyc__gentuple_685,& Cyc__gentuple_686,& Cyc__gentuple_687};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_683={4,sizeof(struct _tuple70),{(
void*)((struct _tuple6**)Cyc__genarr_688),(void*)((struct _tuple6**)Cyc__genarr_688),(
void*)((struct _tuple6**)Cyc__genarr_688 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_676;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_677;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_677={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};
struct _tuple71{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};static struct _tuple6 Cyc__gentuple_678={offsetof(struct _tuple71,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_679={offsetof(struct _tuple71,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_680={offsetof(struct
_tuple71,f3),(void*)& Cyc__genrep_621};static struct _tuple6 Cyc__gentuple_681={
offsetof(struct _tuple71,f4),(void*)& Cyc__genrep_677};static struct _tuple6*Cyc__genarr_682[
4]={& Cyc__gentuple_678,& Cyc__gentuple_679,& Cyc__gentuple_680,& Cyc__gentuple_681};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_676={4,sizeof(struct _tuple71),{(
void*)((struct _tuple6**)Cyc__genarr_682),(void*)((struct _tuple6**)Cyc__genarr_682),(
void*)((struct _tuple6**)Cyc__genarr_682 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_671;static struct _tuple6 Cyc__gentuple_672={offsetof(struct _tuple58,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_673={offsetof(struct
_tuple58,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_674={
offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_621};static struct _tuple6*Cyc__genarr_675[
3]={& Cyc__gentuple_672,& Cyc__gentuple_673,& Cyc__gentuple_674};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_671={4,sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_675),(
void*)((struct _tuple6**)Cyc__genarr_675),(void*)((struct _tuple6**)Cyc__genarr_675
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_665;struct _tuple72{
unsigned int f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Tuniondecl*f3;struct Cyc_Absyn_Tunionfield*
f4;};static struct _tuple6 Cyc__gentuple_666={offsetof(struct _tuple72,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_667={offsetof(struct _tuple72,f2),(
void*)& Cyc__genrep_474};static struct _tuple6 Cyc__gentuple_668={offsetof(struct
_tuple72,f3),(void*)((void*)& Cyc__genrep_289)};static struct _tuple6 Cyc__gentuple_669={
offsetof(struct _tuple72,f4),(void*)& Cyc__genrep_271};static struct _tuple6*Cyc__genarr_670[
4]={& Cyc__gentuple_666,& Cyc__gentuple_667,& Cyc__gentuple_668,& Cyc__gentuple_669};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_665={4,sizeof(struct _tuple72),{(
void*)((struct _tuple6**)Cyc__genarr_670),(void*)((struct _tuple6**)Cyc__genarr_670),(
void*)((struct _tuple6**)Cyc__genarr_670 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_658;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_659;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_659={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_652;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_652={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple73{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_660={offsetof(struct _tuple73,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_661={offsetof(struct _tuple73,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_662={offsetof(struct
_tuple73,f3),(void*)& Cyc__genrep_659};static struct _tuple6 Cyc__gentuple_663={
offsetof(struct _tuple73,f4),(void*)& Cyc__genrep_652};static struct _tuple6*Cyc__genarr_664[
4]={& Cyc__gentuple_660,& Cyc__gentuple_661,& Cyc__gentuple_662,& Cyc__gentuple_663};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_658={4,sizeof(struct _tuple73),{(
void*)((struct _tuple6**)Cyc__genarr_664),(void*)((struct _tuple6**)Cyc__genarr_664),(
void*)((struct _tuple6**)Cyc__genarr_664 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_651;struct _tuple74{unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_653={offsetof(struct _tuple74,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_654={offsetof(struct _tuple74,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_655={offsetof(struct
_tuple74,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_656={
offsetof(struct _tuple74,f4),(void*)& Cyc__genrep_652};static struct _tuple6*Cyc__genarr_657[
4]={& Cyc__gentuple_653,& Cyc__gentuple_654,& Cyc__gentuple_655,& Cyc__gentuple_656};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_651={4,sizeof(struct _tuple74),{(
void*)((struct _tuple6**)Cyc__genarr_657),(void*)((struct _tuple6**)Cyc__genarr_657),(
void*)((struct _tuple6**)Cyc__genarr_657 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_639;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_640;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_640={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};static unsigned char
_tmp379[11]="MallocInfo";static struct _tagged_arr Cyc__genname_647={_tmp379,
_tmp379,_tmp379 + 11};static unsigned char _tmp37A[10]="is_calloc";static struct
_tuple5 Cyc__gentuple_641={offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),{
_tmp37A,_tmp37A,_tmp37A + 10},(void*)((void*)& Cyc__genrep_102)};static
unsigned char _tmp37B[4]="rgn";static struct _tuple5 Cyc__gentuple_642={offsetof(
struct Cyc_Absyn_MallocInfo,rgn),{_tmp37B,_tmp37B,_tmp37B + 4},(void*)& Cyc__genrep_73};
static unsigned char _tmp37C[9]="elt_type";static struct _tuple5 Cyc__gentuple_643={
offsetof(struct Cyc_Absyn_MallocInfo,elt_type),{_tmp37C,_tmp37C,_tmp37C + 9},(void*)&
Cyc__genrep_640};static unsigned char _tmp37D[9]="num_elts";static struct _tuple5 Cyc__gentuple_644={
offsetof(struct Cyc_Absyn_MallocInfo,num_elts),{_tmp37D,_tmp37D,_tmp37D + 9},(void*)&
Cyc__genrep_77};static unsigned char _tmp37E[11]="fat_result";static struct _tuple5
Cyc__gentuple_645={offsetof(struct Cyc_Absyn_MallocInfo,fat_result),{_tmp37E,
_tmp37E,_tmp37E + 11},(void*)((void*)& Cyc__genrep_102)};static struct _tuple5*Cyc__genarr_646[
5]={& Cyc__gentuple_641,& Cyc__gentuple_642,& Cyc__gentuple_643,& Cyc__gentuple_644,&
Cyc__gentuple_645};struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep={3,(
struct _tagged_arr*)& Cyc__genname_647,sizeof(struct Cyc_Absyn_MallocInfo),{(void*)((
struct _tuple5**)Cyc__genarr_646),(void*)((struct _tuple5**)Cyc__genarr_646),(void*)((
struct _tuple5**)Cyc__genarr_646 + 5)}};struct _tuple75{unsigned int f1;struct Cyc_Absyn_MallocInfo
f2;};static struct _tuple6 Cyc__gentuple_648={offsetof(struct _tuple75,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_649={offsetof(struct _tuple75,f2),(
void*)& Cyc_Absyn_malloc_info_t_rep};static struct _tuple6*Cyc__genarr_650[2]={& Cyc__gentuple_648,&
Cyc__gentuple_649};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_639={4,
sizeof(struct _tuple75),{(void*)((struct _tuple6**)Cyc__genarr_650),(void*)((
struct _tuple6**)Cyc__genarr_650),(void*)((struct _tuple6**)Cyc__genarr_650 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_620;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_631;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;
static unsigned char _tmp381[4]="Opt";static struct _tagged_arr Cyc__genname_634={
_tmp381,_tmp381,_tmp381 + 4};static unsigned char _tmp382[2]="v";static struct
_tuple5 Cyc__gentuple_632={offsetof(struct Cyc_Core_Opt,v),{_tmp382,_tmp382,
_tmp382 + 2},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_633[1]={& Cyc__gentuple_632};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={3,(
struct _tagged_arr*)& Cyc__genname_634,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_633),(void*)((struct _tuple5**)Cyc__genarr_633),(void*)((
struct _tuple5**)Cyc__genarr_633 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_631={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};struct
_tuple76{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_635={offsetof(struct _tuple76,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_636={offsetof(struct _tuple76,f2),(void*)& Cyc__genrep_631};
static struct _tuple6 Cyc__gentuple_637={offsetof(struct _tuple76,f3),(void*)& Cyc__genrep_621};
static struct _tuple6*Cyc__genarr_638[3]={& Cyc__gentuple_635,& Cyc__gentuple_636,&
Cyc__gentuple_637};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_620={4,
sizeof(struct _tuple76),{(void*)((struct _tuple6**)Cyc__genarr_638),(void*)((
struct _tuple6**)Cyc__genarr_638),(void*)((struct _tuple6**)Cyc__genarr_638 + 3)}};
static struct _tuple7*Cyc__genarr_75[0]={};static unsigned char _tmp386[8]="Const_e";
static struct _tuple5 Cyc__gentuple_866={0,{_tmp386,_tmp386,_tmp386 + 8},(void*)& Cyc__genrep_836};
static unsigned char _tmp387[6]="Var_e";static struct _tuple5 Cyc__gentuple_867={1,{
_tmp387,_tmp387,_tmp387 + 6},(void*)& Cyc__genrep_823};static unsigned char _tmp388[
12]="UnknownId_e";static struct _tuple5 Cyc__gentuple_868={2,{_tmp388,_tmp388,
_tmp388 + 12},(void*)& Cyc__genrep_246};static unsigned char _tmp389[9]="Primop_e";
static struct _tuple5 Cyc__gentuple_869={3,{_tmp389,_tmp389,_tmp389 + 9},(void*)& Cyc__genrep_818};
static unsigned char _tmp38A[11]="AssignOp_e";static struct _tuple5 Cyc__gentuple_870={
4,{_tmp38A,_tmp38A,_tmp38A + 11},(void*)& Cyc__genrep_786};static unsigned char
_tmp38B[12]="Increment_e";static struct _tuple5 Cyc__gentuple_871={5,{_tmp38B,
_tmp38B,_tmp38B + 12},(void*)& Cyc__genrep_775};static unsigned char _tmp38C[14]="Conditional_e";
static struct _tuple5 Cyc__gentuple_872={6,{_tmp38C,_tmp38C,_tmp38C + 14},(void*)&
Cyc__genrep_769};static unsigned char _tmp38D[9]="SeqExp_e";static struct _tuple5 Cyc__gentuple_873={
7,{_tmp38D,_tmp38D,_tmp38D + 9},(void*)& Cyc__genrep_708};static unsigned char
_tmp38E[14]="UnknownCall_e";static struct _tuple5 Cyc__gentuple_874={8,{_tmp38E,
_tmp38E,_tmp38E + 14},(void*)& Cyc__genrep_764};static unsigned char _tmp38F[9]="FnCall_e";
static struct _tuple5 Cyc__gentuple_875={9,{_tmp38F,_tmp38F,_tmp38F + 9},(void*)& Cyc__genrep_751};
static unsigned char _tmp390[8]="Throw_e";static struct _tuple5 Cyc__gentuple_876={10,{
_tmp390,_tmp390,_tmp390 + 8},(void*)& Cyc__genrep_76};static unsigned char _tmp391[
16]="NoInstantiate_e";static struct _tuple5 Cyc__gentuple_877={11,{_tmp391,_tmp391,
_tmp391 + 16},(void*)& Cyc__genrep_76};static unsigned char _tmp392[14]="Instantiate_e";
static struct _tuple5 Cyc__gentuple_878={12,{_tmp392,_tmp392,_tmp392 + 14},(void*)&
Cyc__genrep_746};static unsigned char _tmp393[7]="Cast_e";static struct _tuple5 Cyc__gentuple_879={
13,{_tmp393,_tmp393,_tmp393 + 7},(void*)& Cyc__genrep_741};static unsigned char
_tmp394[10]="Address_e";static struct _tuple5 Cyc__gentuple_880={14,{_tmp394,
_tmp394,_tmp394 + 10},(void*)& Cyc__genrep_76};static unsigned char _tmp395[6]="New_e";
static struct _tuple5 Cyc__gentuple_881={15,{_tmp395,_tmp395,_tmp395 + 6},(void*)&
Cyc__genrep_736};static unsigned char _tmp396[12]="Sizeoftyp_e";static struct
_tuple5 Cyc__gentuple_882={16,{_tmp396,_tmp396,_tmp396 + 12},(void*)& Cyc__genrep_47};
static unsigned char _tmp397[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_883={
17,{_tmp397,_tmp397,_tmp397 + 12},(void*)& Cyc__genrep_76};static unsigned char
_tmp398[11]="Offsetof_e";static struct _tuple5 Cyc__gentuple_884={18,{_tmp398,
_tmp398,_tmp398 + 11},(void*)& Cyc__genrep_723};static unsigned char _tmp399[9]="Gentyp_e";
static struct _tuple5 Cyc__gentuple_885={19,{_tmp399,_tmp399,_tmp399 + 9},(void*)&
Cyc__genrep_718};static unsigned char _tmp39A[8]="Deref_e";static struct _tuple5 Cyc__gentuple_886={
20,{_tmp39A,_tmp39A,_tmp39A + 8},(void*)& Cyc__genrep_76};static unsigned char
_tmp39B[13]="AggrMember_e";static struct _tuple5 Cyc__gentuple_887={21,{_tmp39B,
_tmp39B,_tmp39B + 13},(void*)& Cyc__genrep_713};static unsigned char _tmp39C[12]="AggrArrow_e";
static struct _tuple5 Cyc__gentuple_888={22,{_tmp39C,_tmp39C,_tmp39C + 12},(void*)&
Cyc__genrep_713};static unsigned char _tmp39D[12]="Subscript_e";static struct
_tuple5 Cyc__gentuple_889={23,{_tmp39D,_tmp39D,_tmp39D + 12},(void*)& Cyc__genrep_708};
static unsigned char _tmp39E[8]="Tuple_e";static struct _tuple5 Cyc__gentuple_890={24,{
_tmp39E,_tmp39E,_tmp39E + 8},(void*)& Cyc__genrep_704};static unsigned char _tmp39F[
14]="CompoundLit_e";static struct _tuple5 Cyc__gentuple_891={25,{_tmp39F,_tmp39F,
_tmp39F + 14},(void*)& Cyc__genrep_693};static unsigned char _tmp3A0[8]="Array_e";
static struct _tuple5 Cyc__gentuple_892={26,{_tmp3A0,_tmp3A0,_tmp3A0 + 8},(void*)&
Cyc__genrep_689};static unsigned char _tmp3A1[16]="Comprehension_e";static struct
_tuple5 Cyc__gentuple_893={27,{_tmp3A1,_tmp3A1,_tmp3A1 + 16},(void*)& Cyc__genrep_683};
static unsigned char _tmp3A2[9]="Struct_e";static struct _tuple5 Cyc__gentuple_894={
28,{_tmp3A2,_tmp3A2,_tmp3A2 + 9},(void*)& Cyc__genrep_676};static unsigned char
_tmp3A3[13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_895={29,{_tmp3A3,
_tmp3A3,_tmp3A3 + 13},(void*)& Cyc__genrep_671};static unsigned char _tmp3A4[9]="Tunion_e";
static struct _tuple5 Cyc__gentuple_896={30,{_tmp3A4,_tmp3A4,_tmp3A4 + 9},(void*)&
Cyc__genrep_665};static unsigned char _tmp3A5[7]="Enum_e";static struct _tuple5 Cyc__gentuple_897={
31,{_tmp3A5,_tmp3A5,_tmp3A5 + 7},(void*)& Cyc__genrep_658};static unsigned char
_tmp3A6[11]="AnonEnum_e";static struct _tuple5 Cyc__gentuple_898={32,{_tmp3A6,
_tmp3A6,_tmp3A6 + 11},(void*)& Cyc__genrep_651};static unsigned char _tmp3A7[9]="Malloc_e";
static struct _tuple5 Cyc__gentuple_899={33,{_tmp3A7,_tmp3A7,_tmp3A7 + 9},(void*)&
Cyc__genrep_639};static unsigned char _tmp3A8[16]="UnresolvedMem_e";static struct
_tuple5 Cyc__gentuple_900={34,{_tmp3A8,_tmp3A8,_tmp3A8 + 16},(void*)& Cyc__genrep_620};
static unsigned char _tmp3A9[10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_901={
35,{_tmp3A9,_tmp3A9,_tmp3A9 + 10},(void*)& Cyc__genrep_463};static unsigned char
_tmp3AA[10]="Codegen_e";static struct _tuple5 Cyc__gentuple_902={36,{_tmp3AA,
_tmp3AA,_tmp3AA + 10},(void*)& Cyc__genrep_81};static unsigned char _tmp3AB[7]="Fill_e";
static struct _tuple5 Cyc__gentuple_903={37,{_tmp3AB,_tmp3AB,_tmp3AB + 7},(void*)&
Cyc__genrep_76};static struct _tuple5*Cyc__genarr_904[38]={& Cyc__gentuple_866,& Cyc__gentuple_867,&
Cyc__gentuple_868,& Cyc__gentuple_869,& Cyc__gentuple_870,& Cyc__gentuple_871,& Cyc__gentuple_872,&
Cyc__gentuple_873,& Cyc__gentuple_874,& Cyc__gentuple_875,& Cyc__gentuple_876,& Cyc__gentuple_877,&
Cyc__gentuple_878,& Cyc__gentuple_879,& Cyc__gentuple_880,& Cyc__gentuple_881,& Cyc__gentuple_882,&
Cyc__gentuple_883,& Cyc__gentuple_884,& Cyc__gentuple_885,& Cyc__gentuple_886,& Cyc__gentuple_887,&
Cyc__gentuple_888,& Cyc__gentuple_889,& Cyc__gentuple_890,& Cyc__gentuple_891,& Cyc__gentuple_892,&
Cyc__gentuple_893,& Cyc__gentuple_894,& Cyc__gentuple_895,& Cyc__gentuple_896,& Cyc__gentuple_897,&
Cyc__gentuple_898,& Cyc__gentuple_899,& Cyc__gentuple_900,& Cyc__gentuple_901,& Cyc__gentuple_902,&
Cyc__gentuple_903};static unsigned char _tmp3AD[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_exp_t_rep={5,{_tmp3AD,_tmp3AD,_tmp3AD + 8},{(void*)((struct _tuple7**)
Cyc__genarr_75),(void*)((struct _tuple7**)Cyc__genarr_75),(void*)((struct _tuple7**)
Cyc__genarr_75 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_904),(void*)((struct
_tuple5**)Cyc__genarr_904),(void*)((struct _tuple5**)Cyc__genarr_904 + 38)}};
static unsigned char _tmp3AE[4]="Exp";static struct _tagged_arr Cyc__genname_910={
_tmp3AE,_tmp3AE,_tmp3AE + 4};static unsigned char _tmp3AF[5]="topt";static struct
_tuple5 Cyc__gentuple_905={offsetof(struct Cyc_Absyn_Exp,topt),{_tmp3AF,_tmp3AF,
_tmp3AF + 5},(void*)& Cyc__genrep_61};static unsigned char _tmp3B0[2]="r";static
struct _tuple5 Cyc__gentuple_906={offsetof(struct Cyc_Absyn_Exp,r),{_tmp3B0,_tmp3B0,
_tmp3B0 + 2},(void*)& Cyc_Absyn_raw_exp_t_rep};static unsigned char _tmp3B1[4]="loc";
static struct _tuple5 Cyc__gentuple_907={offsetof(struct Cyc_Absyn_Exp,loc),{_tmp3B1,
_tmp3B1,_tmp3B1 + 4},(void*)& Cyc__genrep_2};static unsigned char _tmp3B2[6]="annot";
static struct _tuple5 Cyc__gentuple_908={offsetof(struct Cyc_Absyn_Exp,annot),{
_tmp3B2,_tmp3B2,_tmp3B2 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static struct
_tuple5*Cyc__genarr_909[4]={& Cyc__gentuple_905,& Cyc__gentuple_906,& Cyc__gentuple_907,&
Cyc__gentuple_908};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(
struct _tagged_arr*)& Cyc__genname_910,sizeof(struct Cyc_Absyn_Exp),{(void*)((
struct _tuple5**)Cyc__genarr_909),(void*)((struct _tuple5**)Cyc__genarr_909),(void*)((
struct _tuple5**)Cyc__genarr_909 + 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_77={
1,1,(void*)((void*)& Cyc_struct_Absyn_Exp_rep)};static struct _tuple6 Cyc__gentuple_78={
offsetof(struct _tuple29,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_79={
offsetof(struct _tuple29,f2),(void*)& Cyc__genrep_77};static struct _tuple6*Cyc__genarr_80[
2]={& Cyc__gentuple_78,& Cyc__gentuple_79};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_76={
4,sizeof(struct _tuple29),{(void*)((struct _tuple6**)Cyc__genarr_80),(void*)((
struct _tuple6**)Cyc__genarr_80),(void*)((struct _tuple6**)Cyc__genarr_80 + 2)}};
static unsigned char _tmp3B6[10]="Unknown_b";static struct _tuple7 Cyc__gentuple_986={
0,{_tmp3B6,_tmp3B6,_tmp3B6 + 10}};static struct _tuple7*Cyc__genarr_987[1]={& Cyc__gentuple_986};
static unsigned char _tmp3B7[8]="Upper_b";static struct _tuple5 Cyc__gentuple_988={0,{
_tmp3B7,_tmp3B7,_tmp3B7 + 8},(void*)& Cyc__genrep_76};static struct _tuple5*Cyc__genarr_989[
1]={& Cyc__gentuple_988};static unsigned char _tmp3B9[7]="Bounds";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_bounds_t_rep={5,{_tmp3B9,_tmp3B9,_tmp3B9 + 7},{(void*)((struct _tuple7**)
Cyc__genarr_987),(void*)((struct _tuple7**)Cyc__genarr_987),(void*)((struct
_tuple7**)Cyc__genarr_987 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_989),(void*)((
struct _tuple5**)Cyc__genarr_989),(void*)((struct _tuple5**)Cyc__genarr_989 + 1)}};
static struct _tuple6 Cyc__gentuple_990={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_991={offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_bounds_t_rep};
static struct _tuple6*Cyc__genarr_992[2]={& Cyc__gentuple_990,& Cyc__gentuple_991};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_985={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_992),(void*)((struct _tuple6**)Cyc__genarr_992),(
void*)((struct _tuple6**)Cyc__genarr_992 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_981;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_978;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;
static unsigned char _tmp3BB[7]="Conref";static struct _tagged_arr Cyc__genname_998={
_tmp3BB,_tmp3BB,_tmp3BB + 7};static unsigned char _tmp3BC[2]="v";static struct
_tuple5 Cyc__gentuple_996={offsetof(struct Cyc_Absyn_Conref,v),{_tmp3BC,_tmp3BC,
_tmp3BC + 2},(void*)& Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep};static
struct _tuple5*Cyc__genarr_997[1]={& Cyc__gentuple_996};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_998,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_997),(void*)((
struct _tuple5**)Cyc__genarr_997),(void*)((struct _tuple5**)Cyc__genarr_997 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_978={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};
struct _tuple77{unsigned int f1;struct Cyc_Absyn_Conref*f2;};static struct _tuple6 Cyc__gentuple_982={
offsetof(struct _tuple77,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_983={
offsetof(struct _tuple77,f2),(void*)& Cyc__genrep_978};static struct _tuple6*Cyc__genarr_984[
2]={& Cyc__gentuple_982,& Cyc__gentuple_983};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_981={4,sizeof(struct _tuple77),{(void*)((struct _tuple6**)Cyc__genarr_984),(
void*)((struct _tuple6**)Cyc__genarr_984),(void*)((struct _tuple6**)Cyc__genarr_984
+ 2)}};static unsigned char _tmp3C0[10]="No_constr";static struct _tuple7 Cyc__gentuple_979={
0,{_tmp3C0,_tmp3C0,_tmp3C0 + 10}};static struct _tuple7*Cyc__genarr_980[1]={& Cyc__gentuple_979};
static unsigned char _tmp3C1[10]="Eq_constr";static struct _tuple5 Cyc__gentuple_993={
0,{_tmp3C1,_tmp3C1,_tmp3C1 + 10},(void*)& Cyc__genrep_985};static unsigned char
_tmp3C2[15]="Forward_constr";static struct _tuple5 Cyc__gentuple_994={1,{_tmp3C2,
_tmp3C2,_tmp3C2 + 15},(void*)& Cyc__genrep_981};static struct _tuple5*Cyc__genarr_995[
2]={& Cyc__gentuple_993,& Cyc__gentuple_994};static unsigned char _tmp3C4[11]="Constraint";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep={
5,{_tmp3C4,_tmp3C4,_tmp3C4 + 11},{(void*)((struct _tuple7**)Cyc__genarr_980),(void*)((
struct _tuple7**)Cyc__genarr_980),(void*)((struct _tuple7**)Cyc__genarr_980 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_995),(void*)((struct _tuple5**)Cyc__genarr_995),(
void*)((struct _tuple5**)Cyc__genarr_995 + 2)}};static unsigned char _tmp3C5[7]="Conref";
static struct _tagged_arr Cyc__genname_1002={_tmp3C5,_tmp3C5,_tmp3C5 + 7};static
unsigned char _tmp3C6[2]="v";static struct _tuple5 Cyc__gentuple_1000={offsetof(
struct Cyc_Absyn_Conref,v),{_tmp3C6,_tmp3C6,_tmp3C6 + 2},(void*)& Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep};
static struct _tuple5*Cyc__genarr_1001[1]={& Cyc__gentuple_1000};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep={3,(struct _tagged_arr*)& Cyc__genname_1002,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_1001),(void*)((
struct _tuple5**)Cyc__genarr_1001),(void*)((struct _tuple5**)Cyc__genarr_1001 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_999={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0bool2_rep)};
static unsigned char _tmp3C9[8]="PtrInfo";static struct _tagged_arr Cyc__genname_1009={
_tmp3C9,_tmp3C9,_tmp3C9 + 8};static unsigned char _tmp3CA[8]="elt_typ";static struct
_tuple5 Cyc__gentuple_1003={offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),{_tmp3CA,
_tmp3CA,_tmp3CA + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static unsigned char
_tmp3CB[8]="rgn_typ";static struct _tuple5 Cyc__gentuple_1004={offsetof(struct Cyc_Absyn_PtrInfo,rgn_typ),{
_tmp3CB,_tmp3CB,_tmp3CB + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static
unsigned char _tmp3CC[9]="nullable";static struct _tuple5 Cyc__gentuple_1005={
offsetof(struct Cyc_Absyn_PtrInfo,nullable),{_tmp3CC,_tmp3CC,_tmp3CC + 9},(void*)&
Cyc__genrep_999};static unsigned char _tmp3CD[3]="tq";static struct _tuple5 Cyc__gentuple_1006={
offsetof(struct Cyc_Absyn_PtrInfo,tq),{_tmp3CD,_tmp3CD,_tmp3CD + 3},(void*)& Cyc__genrep_132};
static unsigned char _tmp3CE[7]="bounds";static struct _tuple5 Cyc__gentuple_1007={
offsetof(struct Cyc_Absyn_PtrInfo,bounds),{_tmp3CE,_tmp3CE,_tmp3CE + 7},(void*)&
Cyc__genrep_978};static struct _tuple5*Cyc__genarr_1008[5]={& Cyc__gentuple_1003,&
Cyc__gentuple_1004,& Cyc__gentuple_1005,& Cyc__gentuple_1006,& Cyc__gentuple_1007};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_1009,sizeof(struct Cyc_Absyn_PtrInfo),{(void*)((struct _tuple5**)Cyc__genarr_1008),(
void*)((struct _tuple5**)Cyc__genarr_1008),(void*)((struct _tuple5**)Cyc__genarr_1008
+ 5)}};struct _tuple78{unsigned int f1;struct Cyc_Absyn_PtrInfo f2;};static struct
_tuple6 Cyc__gentuple_1010={offsetof(struct _tuple78,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1011={offsetof(struct _tuple78,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};
static struct _tuple6*Cyc__genarr_1012[2]={& Cyc__gentuple_1010,& Cyc__gentuple_1011};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_977={4,sizeof(struct _tuple78),{(
void*)((struct _tuple6**)Cyc__genarr_1012),(void*)((struct _tuple6**)Cyc__genarr_1012),(
void*)((struct _tuple6**)Cyc__genarr_1012 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_966;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep;
static unsigned char _tmp3D1[3]="B1";static struct _tuple7 Cyc__gentuple_967={0,{
_tmp3D1,_tmp3D1,_tmp3D1 + 3}};static unsigned char _tmp3D2[3]="B2";static struct
_tuple7 Cyc__gentuple_968={1,{_tmp3D2,_tmp3D2,_tmp3D2 + 3}};static unsigned char
_tmp3D3[3]="B4";static struct _tuple7 Cyc__gentuple_969={2,{_tmp3D3,_tmp3D3,_tmp3D3
+ 3}};static unsigned char _tmp3D4[3]="B8";static struct _tuple7 Cyc__gentuple_970={3,{
_tmp3D4,_tmp3D4,_tmp3D4 + 3}};static struct _tuple7*Cyc__genarr_971[4]={& Cyc__gentuple_967,&
Cyc__gentuple_968,& Cyc__gentuple_969,& Cyc__gentuple_970};static struct _tuple5*Cyc__genarr_972[
0]={};static unsigned char _tmp3D6[8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={
5,{_tmp3D6,_tmp3D6,_tmp3D6 + 8},{(void*)((struct _tuple7**)Cyc__genarr_971),(void*)((
struct _tuple7**)Cyc__genarr_971),(void*)((struct _tuple7**)Cyc__genarr_971 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_972),(void*)((struct _tuple5**)Cyc__genarr_972),(
void*)((struct _tuple5**)Cyc__genarr_972 + 0)}};static struct _tuple6 Cyc__gentuple_973={
offsetof(struct _tuple66,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_974={
offsetof(struct _tuple66,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_975={
offsetof(struct _tuple66,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple6*
Cyc__genarr_976[3]={& Cyc__gentuple_973,& Cyc__gentuple_974,& Cyc__gentuple_975};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_966={4,sizeof(struct _tuple66),{(
void*)((struct _tuple6**)Cyc__genarr_976),(void*)((struct _tuple6**)Cyc__genarr_976),(
void*)((struct _tuple6**)Cyc__genarr_976 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_960;struct _tuple79{unsigned int f1;void*f2;struct Cyc_Absyn_Tqual f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_961={offsetof(struct
_tuple79,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_962={
offsetof(struct _tuple79,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_963={offsetof(struct _tuple79,f3),(void*)& Cyc__genrep_132};
static struct _tuple6 Cyc__gentuple_964={offsetof(struct _tuple79,f4),(void*)& Cyc__genrep_73};
static struct _tuple6*Cyc__genarr_965[4]={& Cyc__gentuple_961,& Cyc__gentuple_962,&
Cyc__gentuple_963,& Cyc__gentuple_964};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_960={
4,sizeof(struct _tuple79),{(void*)((struct _tuple6**)Cyc__genarr_965),(void*)((
struct _tuple6**)Cyc__genarr_965),(void*)((struct _tuple6**)Cyc__genarr_965 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_941;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_fn_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_942;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static unsigned char _tmp3D9[5]="List";static struct _tagged_arr Cyc__genname_946={
_tmp3D9,_tmp3D9,_tmp3D9 + 5};static unsigned char _tmp3DA[3]="hd";static struct
_tuple5 Cyc__gentuple_943={offsetof(struct Cyc_List_List,hd),{_tmp3DA,_tmp3DA,
_tmp3DA + 3},(void*)& Cyc__genrep_694};static unsigned char _tmp3DB[3]="tl";static
struct _tuple5 Cyc__gentuple_944={offsetof(struct Cyc_List_List,tl),{_tmp3DB,
_tmp3DB,_tmp3DB + 3},(void*)& Cyc__genrep_942};static struct _tuple5*Cyc__genarr_945[
2]={& Cyc__gentuple_943,& Cyc__gentuple_944};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_946,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_945),(void*)((struct _tuple5**)Cyc__genarr_945),(void*)((
struct _tuple5**)Cyc__genarr_945 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_942={
1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static unsigned char _tmp3DE[7]="FnInfo";static struct _tagged_arr Cyc__genname_956={
_tmp3DE,_tmp3DE,_tmp3DE + 7};static unsigned char _tmp3DF[6]="tvars";static struct
_tuple5 Cyc__gentuple_947={offsetof(struct Cyc_Absyn_FnInfo,tvars),{_tmp3DF,
_tmp3DF,_tmp3DF + 6},(void*)& Cyc__genrep_236};static unsigned char _tmp3E0[7]="effect";
static struct _tuple5 Cyc__gentuple_948={offsetof(struct Cyc_Absyn_FnInfo,effect),{
_tmp3E0,_tmp3E0,_tmp3E0 + 7},(void*)& Cyc__genrep_61};static unsigned char _tmp3E1[8]="ret_typ";
static struct _tuple5 Cyc__gentuple_949={offsetof(struct Cyc_Absyn_FnInfo,ret_typ),{
_tmp3E1,_tmp3E1,_tmp3E1 + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static
unsigned char _tmp3E2[5]="args";static struct _tuple5 Cyc__gentuple_950={offsetof(
struct Cyc_Absyn_FnInfo,args),{_tmp3E2,_tmp3E2,_tmp3E2 + 5},(void*)& Cyc__genrep_942};
static unsigned char _tmp3E3[10]="c_varargs";static struct _tuple5 Cyc__gentuple_951={
offsetof(struct Cyc_Absyn_FnInfo,c_varargs),{_tmp3E3,_tmp3E3,_tmp3E3 + 10},(void*)((
void*)& Cyc__genrep_102)};static unsigned char _tmp3E4[12]="cyc_varargs";static
struct _tuple5 Cyc__gentuple_952={offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),{
_tmp3E4,_tmp3E4,_tmp3E4 + 12},(void*)& Cyc__genrep_579};static unsigned char _tmp3E5[
7]="rgn_po";static struct _tuple5 Cyc__gentuple_953={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),{
_tmp3E5,_tmp3E5,_tmp3E5 + 7},(void*)& Cyc__genrep_569};static unsigned char _tmp3E6[
11]="attributes";static struct _tuple5 Cyc__gentuple_954={offsetof(struct Cyc_Absyn_FnInfo,attributes),{
_tmp3E6,_tmp3E6,_tmp3E6 + 11},(void*)& Cyc__genrep_83};static struct _tuple5*Cyc__genarr_955[
8]={& Cyc__gentuple_947,& Cyc__gentuple_948,& Cyc__gentuple_949,& Cyc__gentuple_950,&
Cyc__gentuple_951,& Cyc__gentuple_952,& Cyc__gentuple_953,& Cyc__gentuple_954};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_956,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((struct _tuple5**)Cyc__genarr_955),(
void*)((struct _tuple5**)Cyc__genarr_955),(void*)((struct _tuple5**)Cyc__genarr_955
+ 8)}};struct _tuple80{unsigned int f1;struct Cyc_Absyn_FnInfo f2;};static struct
_tuple6 Cyc__gentuple_957={offsetof(struct _tuple80,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_958={offsetof(struct _tuple80,f2),(void*)& Cyc_Absyn_fn_info_t_rep};
static struct _tuple6*Cyc__genarr_959[2]={& Cyc__gentuple_957,& Cyc__gentuple_958};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_941={4,sizeof(struct _tuple80),{(
void*)((struct _tuple6**)Cyc__genarr_959),(void*)((struct _tuple6**)Cyc__genarr_959),(
void*)((struct _tuple6**)Cyc__genarr_959 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_937;static struct _tuple6 Cyc__gentuple_938={offsetof(struct _tuple11,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_939={offsetof(struct
_tuple11,f2),(void*)& Cyc__genrep_272};static struct _tuple6*Cyc__genarr_940[2]={&
Cyc__gentuple_938,& Cyc__gentuple_939};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_937={
4,sizeof(struct _tuple11),{(void*)((struct _tuple6**)Cyc__genarr_940),(void*)((
struct _tuple6**)Cyc__genarr_940),(void*)((struct _tuple6**)Cyc__genarr_940 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_933;struct _tuple81{unsigned int
f1;struct Cyc_Absyn_AggrInfo f2;};static struct _tuple6 Cyc__gentuple_934={offsetof(
struct _tuple81,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_935={
offsetof(struct _tuple81,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6*Cyc__genarr_936[2]={& Cyc__gentuple_934,& Cyc__gentuple_935};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_933={4,sizeof(struct _tuple81),{(void*)((
struct _tuple6**)Cyc__genarr_936),(void*)((struct _tuple6**)Cyc__genarr_936),(void*)((
struct _tuple6**)Cyc__genarr_936 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_928;
static struct _tuple6 Cyc__gentuple_929={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_930={offsetof(struct _tuple58,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_931={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_341};
static struct _tuple6*Cyc__genarr_932[3]={& Cyc__gentuple_929,& Cyc__gentuple_930,&
Cyc__gentuple_931};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_928={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_932),(void*)((
struct _tuple6**)Cyc__genarr_932),(void*)((struct _tuple6**)Cyc__genarr_932 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_923;struct _tuple82{unsigned int
f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;};static struct _tuple6 Cyc__gentuple_924={
offsetof(struct _tuple82,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_925={
offsetof(struct _tuple82,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_926={
offsetof(struct _tuple82,f3),(void*)& Cyc__genrep_659};static struct _tuple6*Cyc__genarr_927[
3]={& Cyc__gentuple_924,& Cyc__gentuple_925,& Cyc__gentuple_926};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_923={4,sizeof(struct _tuple82),{(void*)((struct _tuple6**)Cyc__genarr_927),(
void*)((struct _tuple6**)Cyc__genarr_927),(void*)((struct _tuple6**)Cyc__genarr_927
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_70;static struct _tuple6 Cyc__gentuple_920={
offsetof(struct _tuple11,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_921={
offsetof(struct _tuple11,f2),(void*)& Cyc__genrep_71};static struct _tuple6*Cyc__genarr_922[
2]={& Cyc__gentuple_920,& Cyc__gentuple_921};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_70={4,sizeof(struct _tuple11),{(void*)((struct _tuple6**)Cyc__genarr_922),(
void*)((struct _tuple6**)Cyc__genarr_922),(void*)((struct _tuple6**)Cyc__genarr_922
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_60;struct _tuple83{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_Core_Opt*f4;};
static struct _tuple6 Cyc__gentuple_65={offsetof(struct _tuple83,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_66={offsetof(struct _tuple83,f2),(void*)& Cyc__genrep_10};
static struct _tuple6 Cyc__gentuple_67={offsetof(struct _tuple83,f3),(void*)& Cyc__genrep_52};
static struct _tuple6 Cyc__gentuple_68={offsetof(struct _tuple83,f4),(void*)& Cyc__genrep_61};
static struct _tuple6*Cyc__genarr_69[4]={& Cyc__gentuple_65,& Cyc__gentuple_66,& Cyc__gentuple_67,&
Cyc__gentuple_68};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_60={4,sizeof(
struct _tuple83),{(void*)((struct _tuple6**)Cyc__genarr_69),(void*)((struct _tuple6**)
Cyc__genarr_69),(void*)((struct _tuple6**)Cyc__genarr_69 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_51;static struct _tuple6 Cyc__gentuple_57={offsetof(struct _tuple11,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_58={offsetof(struct
_tuple11,f2),(void*)& Cyc__genrep_52};static struct _tuple6*Cyc__genarr_59[2]={& Cyc__gentuple_57,&
Cyc__gentuple_58};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_51={4,sizeof(
struct _tuple11),{(void*)((struct _tuple6**)Cyc__genarr_59),(void*)((struct _tuple6**)
Cyc__genarr_59),(void*)((struct _tuple6**)Cyc__genarr_59 + 2)}};static
unsigned char _tmp3F0[9]="VoidType";static struct _tuple7 Cyc__gentuple_43={0,{
_tmp3F0,_tmp3F0,_tmp3F0 + 9}};static unsigned char _tmp3F1[10]="FloatType";static
struct _tuple7 Cyc__gentuple_44={1,{_tmp3F1,_tmp3F1,_tmp3F1 + 10}};static
unsigned char _tmp3F2[8]="HeapRgn";static struct _tuple7 Cyc__gentuple_45={2,{
_tmp3F2,_tmp3F2,_tmp3F2 + 8}};static struct _tuple7*Cyc__genarr_46[3]={& Cyc__gentuple_43,&
Cyc__gentuple_44,& Cyc__gentuple_45};static unsigned char _tmp3F3[5]="Evar";static
struct _tuple5 Cyc__gentuple_1080={0,{_tmp3F3,_tmp3F3,_tmp3F3 + 5},(void*)& Cyc__genrep_1069};
static unsigned char _tmp3F4[8]="VarType";static struct _tuple5 Cyc__gentuple_1081={1,{
_tmp3F4,_tmp3F4,_tmp3F4 + 8},(void*)& Cyc__genrep_1065};static unsigned char _tmp3F5[
11]="TunionType";static struct _tuple5 Cyc__gentuple_1082={2,{_tmp3F5,_tmp3F5,
_tmp3F5 + 11},(void*)& Cyc__genrep_1039};static unsigned char _tmp3F6[16]="TunionFieldType";
static struct _tuple5 Cyc__gentuple_1083={3,{_tmp3F6,_tmp3F6,_tmp3F6 + 16},(void*)&
Cyc__genrep_1013};static unsigned char _tmp3F7[12]="PointerType";static struct
_tuple5 Cyc__gentuple_1084={4,{_tmp3F7,_tmp3F7,_tmp3F7 + 12},(void*)& Cyc__genrep_977};
static unsigned char _tmp3F8[8]="IntType";static struct _tuple5 Cyc__gentuple_1085={5,{
_tmp3F8,_tmp3F8,_tmp3F8 + 8},(void*)& Cyc__genrep_966};static unsigned char _tmp3F9[
11]="DoubleType";static struct _tuple5 Cyc__gentuple_1086={6,{_tmp3F9,_tmp3F9,
_tmp3F9 + 11},(void*)& Cyc__genrep_116};static unsigned char _tmp3FA[10]="ArrayType";
static struct _tuple5 Cyc__gentuple_1087={7,{_tmp3FA,_tmp3FA,_tmp3FA + 10},(void*)&
Cyc__genrep_960};static unsigned char _tmp3FB[7]="FnType";static struct _tuple5 Cyc__gentuple_1088={
8,{_tmp3FB,_tmp3FB,_tmp3FB + 7},(void*)& Cyc__genrep_941};static unsigned char
_tmp3FC[10]="TupleType";static struct _tuple5 Cyc__gentuple_1089={9,{_tmp3FC,
_tmp3FC,_tmp3FC + 10},(void*)& Cyc__genrep_937};static unsigned char _tmp3FD[9]="AggrType";
static struct _tuple5 Cyc__gentuple_1090={10,{_tmp3FD,_tmp3FD,_tmp3FD + 9},(void*)&
Cyc__genrep_933};static unsigned char _tmp3FE[13]="AnonAggrType";static struct
_tuple5 Cyc__gentuple_1091={11,{_tmp3FE,_tmp3FE,_tmp3FE + 13},(void*)& Cyc__genrep_928};
static unsigned char _tmp3FF[9]="EnumType";static struct _tuple5 Cyc__gentuple_1092={
12,{_tmp3FF,_tmp3FF,_tmp3FF + 9},(void*)& Cyc__genrep_923};static unsigned char
_tmp400[13]="AnonEnumType";static struct _tuple5 Cyc__gentuple_1093={13,{_tmp400,
_tmp400,_tmp400 + 13},(void*)& Cyc__genrep_70};static unsigned char _tmp401[11]="SizeofType";
static struct _tuple5 Cyc__gentuple_1094={14,{_tmp401,_tmp401,_tmp401 + 11},(void*)&
Cyc__genrep_47};static unsigned char _tmp402[14]="RgnHandleType";static struct
_tuple5 Cyc__gentuple_1095={15,{_tmp402,_tmp402,_tmp402 + 14},(void*)& Cyc__genrep_47};
static unsigned char _tmp403[12]="TypedefType";static struct _tuple5 Cyc__gentuple_1096={
16,{_tmp403,_tmp403,_tmp403 + 12},(void*)& Cyc__genrep_60};static unsigned char
_tmp404[10]="AccessEff";static struct _tuple5 Cyc__gentuple_1097={17,{_tmp404,
_tmp404,_tmp404 + 10},(void*)& Cyc__genrep_47};static unsigned char _tmp405[8]="JoinEff";
static struct _tuple5 Cyc__gentuple_1098={18,{_tmp405,_tmp405,_tmp405 + 8},(void*)&
Cyc__genrep_51};static unsigned char _tmp406[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1099={
19,{_tmp406,_tmp406,_tmp406 + 8},(void*)& Cyc__genrep_47};static struct _tuple5*Cyc__genarr_1100[
20]={& Cyc__gentuple_1080,& Cyc__gentuple_1081,& Cyc__gentuple_1082,& Cyc__gentuple_1083,&
Cyc__gentuple_1084,& Cyc__gentuple_1085,& Cyc__gentuple_1086,& Cyc__gentuple_1087,&
Cyc__gentuple_1088,& Cyc__gentuple_1089,& Cyc__gentuple_1090,& Cyc__gentuple_1091,&
Cyc__gentuple_1092,& Cyc__gentuple_1093,& Cyc__gentuple_1094,& Cyc__gentuple_1095,&
Cyc__gentuple_1096,& Cyc__gentuple_1097,& Cyc__gentuple_1098,& Cyc__gentuple_1099};
static unsigned char _tmp408[5]="Type";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep={
5,{_tmp408,_tmp408,_tmp408 + 5},{(void*)((struct _tuple7**)Cyc__genarr_46),(void*)((
struct _tuple7**)Cyc__genarr_46),(void*)((struct _tuple7**)Cyc__genarr_46 + 3)},{(
void*)((struct _tuple5**)Cyc__genarr_1100),(void*)((struct _tuple5**)Cyc__genarr_1100),(
void*)((struct _tuple5**)Cyc__genarr_1100 + 20)}};static unsigned char _tmp409[8]="Vardecl";
static struct _tagged_arr Cyc__genname_151={_tmp409,_tmp409,_tmp409 + 8};static
unsigned char _tmp40A[3]="sc";static struct _tuple5 Cyc__gentuple_142={offsetof(
struct Cyc_Absyn_Vardecl,sc),{_tmp40A,_tmp40A,_tmp40A + 3},(void*)& Cyc_Absyn_scope_t_rep};
static unsigned char _tmp40B[5]="name";static struct _tuple5 Cyc__gentuple_143={
offsetof(struct Cyc_Absyn_Vardecl,name),{_tmp40B,_tmp40B,_tmp40B + 5},(void*)& Cyc__genrep_10};
static unsigned char _tmp40C[3]="tq";static struct _tuple5 Cyc__gentuple_144={
offsetof(struct Cyc_Absyn_Vardecl,tq),{_tmp40C,_tmp40C,_tmp40C + 3},(void*)& Cyc__genrep_132};
static unsigned char _tmp40D[5]="type";static struct _tuple5 Cyc__gentuple_145={
offsetof(struct Cyc_Absyn_Vardecl,type),{_tmp40D,_tmp40D,_tmp40D + 5},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static unsigned char _tmp40E[12]="initializer";
static struct _tuple5 Cyc__gentuple_146={offsetof(struct Cyc_Absyn_Vardecl,initializer),{
_tmp40E,_tmp40E,_tmp40E + 12},(void*)& Cyc__genrep_73};static unsigned char _tmp40F[
4]="rgn";static struct _tuple5 Cyc__gentuple_147={offsetof(struct Cyc_Absyn_Vardecl,rgn),{
_tmp40F,_tmp40F,_tmp40F + 4},(void*)& Cyc__genrep_61};static unsigned char _tmp410[
11]="attributes";static struct _tuple5 Cyc__gentuple_148={offsetof(struct Cyc_Absyn_Vardecl,attributes),{
_tmp410,_tmp410,_tmp410 + 11},(void*)& Cyc__genrep_83};static unsigned char _tmp411[
8]="escapes";static struct _tuple5 Cyc__gentuple_149={offsetof(struct Cyc_Absyn_Vardecl,escapes),{
_tmp411,_tmp411,_tmp411 + 8},(void*)((void*)& Cyc__genrep_102)};static struct
_tuple5*Cyc__genarr_150[8]={& Cyc__gentuple_142,& Cyc__gentuple_143,& Cyc__gentuple_144,&
Cyc__gentuple_145,& Cyc__gentuple_146,& Cyc__gentuple_147,& Cyc__gentuple_148,& Cyc__gentuple_149};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_151,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((struct
_tuple5**)Cyc__genarr_150),(void*)((struct _tuple5**)Cyc__genarr_150),(void*)((
struct _tuple5**)Cyc__genarr_150 + 8)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_131={
1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};struct _tuple84{unsigned int f1;
struct Cyc_Absyn_Vardecl*f2;};static struct _tuple6 Cyc__gentuple_395={offsetof(
struct _tuple84,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_396={
offsetof(struct _tuple84,f2),(void*)& Cyc__genrep_131};static struct _tuple6*Cyc__genarr_397[
2]={& Cyc__gentuple_395,& Cyc__gentuple_396};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_394={4,sizeof(struct _tuple84),{(void*)((struct _tuple6**)Cyc__genarr_397),(
void*)((struct _tuple6**)Cyc__genarr_397),(void*)((struct _tuple6**)Cyc__genarr_397
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1125;struct _tuple85{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Core_Opt*
f4;struct Cyc_Absyn_Exp*f5;int f6;};static struct _tuple6 Cyc__gentuple_1126={
offsetof(struct _tuple85,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1127={
offsetof(struct _tuple85,f2),(void*)& Cyc__genrep_226};static struct _tuple6 Cyc__gentuple_1128={
offsetof(struct _tuple85,f3),(void*)& Cyc__genrep_129};static struct _tuple6 Cyc__gentuple_1129={
offsetof(struct _tuple85,f4),(void*)& Cyc__genrep_61};static struct _tuple6 Cyc__gentuple_1130={
offsetof(struct _tuple85,f5),(void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_1131={
offsetof(struct _tuple85,f6),(void*)((void*)& Cyc__genrep_102)};static struct
_tuple6*Cyc__genarr_1132[6]={& Cyc__gentuple_1126,& Cyc__gentuple_1127,& Cyc__gentuple_1128,&
Cyc__gentuple_1129,& Cyc__gentuple_1130,& Cyc__gentuple_1131};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1125={4,sizeof(struct _tuple85),{(void*)((struct _tuple6**)Cyc__genarr_1132),(
void*)((struct _tuple6**)Cyc__genarr_1132),(void*)((struct _tuple6**)Cyc__genarr_1132
+ 6)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1121;static struct _tuple6
Cyc__gentuple_1122={offsetof(struct _tuple11,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_1123={offsetof(struct _tuple11,f2),(void*)& Cyc__genrep_130};
static struct _tuple6*Cyc__genarr_1124[2]={& Cyc__gentuple_1122,& Cyc__gentuple_1123};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1121={4,sizeof(struct _tuple11),{(
void*)((struct _tuple6**)Cyc__genarr_1124),(void*)((struct _tuple6**)Cyc__genarr_1124),(
void*)((struct _tuple6**)Cyc__genarr_1124 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1117;struct _tuple86{unsigned int f1;struct Cyc_Absyn_Aggrdecl*f2;};
static struct _tuple6 Cyc__gentuple_1118={offsetof(struct _tuple86,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1119={offsetof(struct _tuple86,f2),(void*)((void*)&
Cyc__genrep_339)};static struct _tuple6*Cyc__genarr_1120[2]={& Cyc__gentuple_1118,&
Cyc__gentuple_1119};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1117={4,
sizeof(struct _tuple86),{(void*)((struct _tuple6**)Cyc__genarr_1120),(void*)((
struct _tuple6**)Cyc__genarr_1120),(void*)((struct _tuple6**)Cyc__genarr_1120 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1113;struct _tuple87{unsigned int
f1;struct Cyc_Absyn_Tuniondecl*f2;};static struct _tuple6 Cyc__gentuple_1114={
offsetof(struct _tuple87,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1115={
offsetof(struct _tuple87,f2),(void*)((void*)& Cyc__genrep_289)};static struct
_tuple6*Cyc__genarr_1116[2]={& Cyc__gentuple_1114,& Cyc__gentuple_1115};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1113={4,sizeof(struct _tuple87),{(void*)((
struct _tuple6**)Cyc__genarr_1116),(void*)((struct _tuple6**)Cyc__genarr_1116),(
void*)((struct _tuple6**)Cyc__genarr_1116 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1109;struct _tuple88{unsigned int f1;struct Cyc_Absyn_Enumdecl*f2;};
static struct _tuple6 Cyc__gentuple_1110={offsetof(struct _tuple88,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1111={offsetof(struct _tuple88,f2),(void*)& Cyc__genrep_256};
static struct _tuple6*Cyc__genarr_1112[2]={& Cyc__gentuple_1110,& Cyc__gentuple_1111};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1109={4,sizeof(struct _tuple88),{(
void*)((struct _tuple6**)Cyc__genarr_1112),(void*)((struct _tuple6**)Cyc__genarr_1112),(
void*)((struct _tuple6**)Cyc__genarr_1112 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_41;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_42;extern struct
Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;static unsigned char
_tmp41A[12]="Typedefdecl";static struct _tagged_arr Cyc__genname_1105={_tmp41A,
_tmp41A,_tmp41A + 12};static unsigned char _tmp41B[5]="name";static struct _tuple5 Cyc__gentuple_1101={
offsetof(struct Cyc_Absyn_Typedefdecl,name),{_tmp41B,_tmp41B,_tmp41B + 5},(void*)&
Cyc__genrep_10};static unsigned char _tmp41C[4]="tvs";static struct _tuple5 Cyc__gentuple_1102={
offsetof(struct Cyc_Absyn_Typedefdecl,tvs),{_tmp41C,_tmp41C,_tmp41C + 4},(void*)&
Cyc__genrep_236};static unsigned char _tmp41D[5]="defn";static struct _tuple5 Cyc__gentuple_1103={
offsetof(struct Cyc_Absyn_Typedefdecl,defn),{_tmp41D,_tmp41D,_tmp41D + 5},(void*)&
Cyc_Absyn_type_t_rep};static struct _tuple5*Cyc__genarr_1104[3]={& Cyc__gentuple_1101,&
Cyc__gentuple_1102,& Cyc__gentuple_1103};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_1105,sizeof(struct Cyc_Absyn_Typedefdecl),{(
void*)((struct _tuple5**)Cyc__genarr_1104),(void*)((struct _tuple5**)Cyc__genarr_1104),(
void*)((struct _tuple5**)Cyc__genarr_1104 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_42={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct
_tuple89{unsigned int f1;struct Cyc_Absyn_Typedefdecl*f2;};static struct _tuple6 Cyc__gentuple_1106={
offsetof(struct _tuple89,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1107={
offsetof(struct _tuple89,f2),(void*)& Cyc__genrep_42};static struct _tuple6*Cyc__genarr_1108[
2]={& Cyc__gentuple_1106,& Cyc__gentuple_1107};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_41={4,sizeof(struct _tuple89),{(void*)((struct _tuple6**)Cyc__genarr_1108),(
void*)((struct _tuple6**)Cyc__genarr_1108),(void*)((struct _tuple6**)Cyc__genarr_1108
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_36;struct _tuple90{
unsigned int f1;struct _tagged_arr*f2;struct Cyc_List_List*f3;};static struct _tuple6
Cyc__gentuple_37={offsetof(struct _tuple90,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_38={offsetof(struct _tuple90,f2),(void*)& Cyc__genrep_12};
static struct _tuple6 Cyc__gentuple_39={offsetof(struct _tuple90,f3),(void*)& Cyc__genrep_0};
static struct _tuple6*Cyc__genarr_40[3]={& Cyc__gentuple_37,& Cyc__gentuple_38,& Cyc__gentuple_39};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_36={4,sizeof(struct _tuple90),{(
void*)((struct _tuple6**)Cyc__genarr_40),(void*)((struct _tuple6**)Cyc__genarr_40),(
void*)((struct _tuple6**)Cyc__genarr_40 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9;struct _tuple91{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*
f3;};static struct _tuple6 Cyc__gentuple_32={offsetof(struct _tuple91,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_33={offsetof(struct _tuple91,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_34={offsetof(struct
_tuple91,f3),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_35[3]={& Cyc__gentuple_32,&
Cyc__gentuple_33,& Cyc__gentuple_34};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_9={
4,sizeof(struct _tuple91),{(void*)((struct _tuple6**)Cyc__genarr_35),(void*)((
struct _tuple6**)Cyc__genarr_35),(void*)((struct _tuple6**)Cyc__genarr_35 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_4;static struct _tuple6 Cyc__gentuple_6={
offsetof(struct _tuple11,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_7={
offsetof(struct _tuple11,f2),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_8[
2]={& Cyc__gentuple_6,& Cyc__gentuple_7};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_4={
4,sizeof(struct _tuple11),{(void*)((struct _tuple6**)Cyc__genarr_8),(void*)((
struct _tuple6**)Cyc__genarr_8),(void*)((struct _tuple6**)Cyc__genarr_8 + 2)}};
static struct _tuple7*Cyc__genarr_3[0]={};static unsigned char _tmp424[6]="Var_d";
static struct _tuple5 Cyc__gentuple_1133={0,{_tmp424,_tmp424,_tmp424 + 6},(void*)&
Cyc__genrep_394};static unsigned char _tmp425[5]="Fn_d";static struct _tuple5 Cyc__gentuple_1134={
1,{_tmp425,_tmp425,_tmp425 + 5},(void*)& Cyc__genrep_81};static unsigned char
_tmp426[6]="Let_d";static struct _tuple5 Cyc__gentuple_1135={2,{_tmp426,_tmp426,
_tmp426 + 6},(void*)& Cyc__genrep_1125};static unsigned char _tmp427[7]="Letv_d";
static struct _tuple5 Cyc__gentuple_1136={3,{_tmp427,_tmp427,_tmp427 + 7},(void*)&
Cyc__genrep_1121};static unsigned char _tmp428[7]="Aggr_d";static struct _tuple5 Cyc__gentuple_1137={
4,{_tmp428,_tmp428,_tmp428 + 7},(void*)& Cyc__genrep_1117};static unsigned char
_tmp429[9]="Tunion_d";static struct _tuple5 Cyc__gentuple_1138={5,{_tmp429,_tmp429,
_tmp429 + 9},(void*)& Cyc__genrep_1113};static unsigned char _tmp42A[7]="Enum_d";
static struct _tuple5 Cyc__gentuple_1139={6,{_tmp42A,_tmp42A,_tmp42A + 7},(void*)&
Cyc__genrep_1109};static unsigned char _tmp42B[10]="Typedef_d";static struct _tuple5
Cyc__gentuple_1140={7,{_tmp42B,_tmp42B,_tmp42B + 10},(void*)& Cyc__genrep_41};
static unsigned char _tmp42C[12]="Namespace_d";static struct _tuple5 Cyc__gentuple_1141={
8,{_tmp42C,_tmp42C,_tmp42C + 12},(void*)& Cyc__genrep_36};static unsigned char
_tmp42D[8]="Using_d";static struct _tuple5 Cyc__gentuple_1142={9,{_tmp42D,_tmp42D,
_tmp42D + 8},(void*)& Cyc__genrep_9};static unsigned char _tmp42E[10]="ExternC_d";
static struct _tuple5 Cyc__gentuple_1143={10,{_tmp42E,_tmp42E,_tmp42E + 10},(void*)&
Cyc__genrep_4};static struct _tuple5*Cyc__genarr_1144[11]={& Cyc__gentuple_1133,&
Cyc__gentuple_1134,& Cyc__gentuple_1135,& Cyc__gentuple_1136,& Cyc__gentuple_1137,&
Cyc__gentuple_1138,& Cyc__gentuple_1139,& Cyc__gentuple_1140,& Cyc__gentuple_1141,&
Cyc__gentuple_1142,& Cyc__gentuple_1143};static unsigned char _tmp430[9]="Raw_decl";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={5,{_tmp430,_tmp430,
_tmp430 + 9},{(void*)((struct _tuple7**)Cyc__genarr_3),(void*)((struct _tuple7**)
Cyc__genarr_3),(void*)((struct _tuple7**)Cyc__genarr_3 + 0)},{(void*)((struct
_tuple5**)Cyc__genarr_1144),(void*)((struct _tuple5**)Cyc__genarr_1144),(void*)((
struct _tuple5**)Cyc__genarr_1144 + 11)}};static unsigned char _tmp431[5]="Decl";
static struct _tagged_arr Cyc__genname_1148={_tmp431,_tmp431,_tmp431 + 5};static
unsigned char _tmp432[2]="r";static struct _tuple5 Cyc__gentuple_1145={offsetof(
struct Cyc_Absyn_Decl,r),{_tmp432,_tmp432,_tmp432 + 2},(void*)& Cyc_Absyn_raw_decl_t_rep};
static unsigned char _tmp433[4]="loc";static struct _tuple5 Cyc__gentuple_1146={
offsetof(struct Cyc_Absyn_Decl,loc),{_tmp433,_tmp433,_tmp433 + 4},(void*)& Cyc__genrep_2};
static struct _tuple5*Cyc__genarr_1147[2]={& Cyc__gentuple_1145,& Cyc__gentuple_1146};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={3,(struct _tagged_arr*)&
Cyc__genname_1148,sizeof(struct Cyc_Absyn_Decl),{(void*)((struct _tuple5**)Cyc__genarr_1147),(
void*)((struct _tuple5**)Cyc__genarr_1147),(void*)((struct _tuple5**)Cyc__genarr_1147
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)&
Cyc_struct_Absyn_Decl_rep)};static unsigned char _tmp436[5]="List";static struct
_tagged_arr Cyc__genname_1152={_tmp436,_tmp436,_tmp436 + 5};static unsigned char
_tmp437[3]="hd";static struct _tuple5 Cyc__gentuple_1149={offsetof(struct Cyc_List_List,hd),{
_tmp437,_tmp437,_tmp437 + 3},(void*)& Cyc__genrep_1};static unsigned char _tmp438[3]="tl";
static struct _tuple5 Cyc__gentuple_1150={offsetof(struct Cyc_List_List,tl),{_tmp438,
_tmp438,_tmp438 + 3},(void*)& Cyc__genrep_0};static struct _tuple5*Cyc__genarr_1151[
2]={& Cyc__gentuple_1149,& Cyc__gentuple_1150};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_1152,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_1151),(void*)((struct _tuple5**)Cyc__genarr_1151),(
void*)((struct _tuple5**)Cyc__genarr_1151 + 2)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};
void*Cyc_decls_rep=(void*)& Cyc__genrep_0;
