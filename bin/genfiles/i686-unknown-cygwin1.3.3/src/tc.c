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
tl;};extern int Cyc_List_length(struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];extern int Cyc_List_mem(int(*compare)(void*,
void*),struct Cyc_List_List*l,void*x);extern struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);extern int Cyc_Std_strptrcmp(struct
_tagged_arr*s1,struct _tagged_arr*s2);extern int Cyc_Std_zstrptrcmp(struct
_tagged_arr*,struct _tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
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
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern void*Cyc_Absyn_compress_kb(void*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);extern struct _tagged_arr Cyc_Absyn_attribute2string(
void*);extern int Cyc_Absyn_fntype_att(void*a);struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct
Cyc_List_List*curr_namespace;};extern void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;extern struct
_tagged_arr Cyc_Absynpp_typ2string(void*);extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(
void*,void*));extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*
elt);extern int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern unsigned char
Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];
extern unsigned char Cyc_Dict_Absent[11];extern int Cyc_Dict_member(struct Cyc_Dict_Dict*
d,void*k);extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,
void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);extern void Cyc_Dict_iter(void(*
f)(void*,void*),struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_map(
void*(*f)(void*),struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_filter_c(
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;
struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct
Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;
};struct Cyc_Tcenv_Frame_struct{int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};extern struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv();
extern struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct Cyc_Position_Segment*,struct
Cyc_Absyn_Fndecl*);extern struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tagged_arr*,struct Cyc_List_List*);
extern struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple0*);extern int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*);extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);extern int Cyc_Tcutil_is_function_type(void*t);extern void Cyc_Tcutil_explain_failure();
extern void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);extern void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn);extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,void*);extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);extern void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);extern int Cyc_Tcutil_bits_only(void*t);extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);extern int Cyc_Tcutil_supports_default(
void*);extern void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);extern void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,int new_block);extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);extern void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);extern struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
extern void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct
Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);extern void Cyc_Tc_tcTuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Tuniondecl*);extern void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);extern
unsigned char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*field;};extern struct
Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct
Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);
extern struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*
d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg);extern struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct
Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*loc,
struct _tagged_arr*msg);extern void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*f,int*res,struct _tagged_arr*v,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Tcgenrep_RepInfo;
extern struct Cyc_Dict_Dict*Cyc_Tcgenrep_empty_typerep_dict();struct _tuple3{struct
Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};extern struct
_tuple3 Cyc_Tcgenrep_tcGenrep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,void*type,struct Cyc_Dict_Dict*dict);static
unsigned char _tmp0[1]="";static struct _tagged_arr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,
_tmp0 + 1};static struct _tagged_arr*Cyc_Tc_tc_msg=(struct _tagged_arr*)& Cyc_Tc_tc_msg_c;
static struct Cyc_List_List*Cyc_Tc_transfer_fn_type_atts(void*t,struct Cyc_List_List*
atts){void*_tmp1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp2;struct Cyc_List_List*
_tmp3;struct Cyc_List_List**_tmp4;_LL1: if((unsigned int)_tmp1 > 3?*((int*)_tmp1)== 
8: 0){_LL5: _tmp2=((struct Cyc_Absyn_FnType_struct*)_tmp1)->f1;_LL6: _tmp3=_tmp2.attributes;
_tmp4=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp1)->f1).attributes;
goto _LL2;}else{goto _LL3;}_LL3: goto _LL4;_LL2: {struct Cyc_List_List*_tmp5=0;for(0;
atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){*_tmp4=({
struct Cyc_List_List*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->hd=(void*)((void*)
atts->hd);_tmp6->tl=*_tmp4;_tmp6;});}else{_tmp5=({struct Cyc_List_List*_tmp7=
_cycalloc(sizeof(*_tmp7));_tmp7->hd=(void*)((void*)atts->hd);_tmp7->tl=_tmp5;
_tmp7;});}}return _tmp5;}_LL4:({void*_tmp8[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("transfer_fn_type_atts",sizeof(
unsigned char),22),_tag_arr(_tmp8,sizeof(void*),0));});_LL0:;}struct _tuple4{void*
f1;int f2;};static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init){
struct _tagged_arr*v=(*vd->name).f2;void*t=(void*)vd->type;void*sc=(void*)vd->sc;
struct Cyc_List_List*atts=vd->attributes;vd->escapes=1;{void*_tmp9=(*vd->name).f1;
struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmpB;_LL8: if((unsigned int)_tmp9 > 
1?*((int*)_tmp9)== 0: 0){_LLE: _tmpA=((struct Cyc_Absyn_Rel_n_struct*)_tmp9)->f1;
if(_tmpA == 0){goto _LL9;}else{goto _LLA;}}else{goto _LLA;}_LLA: if((unsigned int)
_tmp9 > 1?*((int*)_tmp9)== 1: 0){_LLF: _tmpB=((struct Cyc_Absyn_Abs_n_struct*)_tmp9)->f1;
if(_tmpB == 0){goto _LLB;}else{goto _LLC;}}else{goto _LLC;}_LLC: goto _LLD;_LL9: goto
_LLB;_LLB: goto _LL7;_LLD:({struct Cyc_Std_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{void*_tmpC[1]={& _tmpD};Cyc_Tcutil_terr(
loc,_tag_arr("qualified variable declarations are not implemented (%s)",sizeof(
unsigned char),57),_tag_arr(_tmpC,sizeof(void*),1));}});return;_LL7:;}(*vd->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE[0]=({
struct Cyc_Absyn_Abs_n_struct _tmpF;_tmpF.tag=1;_tmpF.f1=te->ns;_tmpF;});_tmpE;});{
void*_tmp10=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Tqual
_tmp12;void*_tmp13;_LL11: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 7: 0){_LL18:
_tmp13=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f1;goto _LL17;_LL17:
_tmp12=((struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f2;goto _LL16;_LL16: _tmp11=((
struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f3;if(_tmp11 == 0){goto _LL15;}else{
goto _LL13;}}else{goto _LL13;}_LL15: if(vd->initializer != 0){goto _LL12;}else{goto
_LL13;}_LL13: goto _LL14;_LL12:{void*_tmp14=(void*)((struct Cyc_Absyn_Exp*)
_check_null(vd->initializer))->r;void*_tmp15;struct _tagged_arr _tmp16;struct Cyc_Absyn_Exp*
_tmp17;struct Cyc_List_List*_tmp18;struct Cyc_List_List*_tmp19;_LL1A: if(*((int*)
_tmp14)== 0){_LL24: _tmp15=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp14)->f1;
if((unsigned int)_tmp15 > 1?*((int*)_tmp15)== 5: 0){_LL25: _tmp16=((struct Cyc_Absyn_String_c_struct*)
_tmp15)->f1;goto _LL1B;}else{goto _LL1C;}}else{goto _LL1C;}_LL1C: if(*((int*)_tmp14)
== 27){_LL26: _tmp17=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp14)->f2;goto
_LL1D;}else{goto _LL1E;}_LL1E: if(*((int*)_tmp14)== 34){_LL27: _tmp18=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp14)->f2;goto _LL1F;}else{goto _LL20;}_LL20: if(*((int*)_tmp14)== 26){_LL28:
_tmp19=((struct Cyc_Absyn_Array_e_struct*)_tmp14)->f1;goto _LL21;}else{goto _LL22;}
_LL22: goto _LL23;_LL1B: t=(void*)(vd->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp1B;_tmp1B.tag=7;_tmp1B.f1=(void*)_tmp13;_tmp1B.f2=_tmp12;_tmp1B.f3=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_arr_size(_tmp16,sizeof(unsigned char)),0);
_tmp1B;});_tmp1A;})));goto _LL19;_LL1D: t=(void*)(vd->type=(void*)((void*)({struct
Cyc_Absyn_ArrayType_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp1D;_tmp1D.tag=7;_tmp1D.f1=(void*)_tmp13;_tmp1D.f2=
_tmp12;_tmp1D.f3=(struct Cyc_Absyn_Exp*)_tmp17;_tmp1D;});_tmp1C;})));goto _LL19;
_LL1F: _tmp19=_tmp18;goto _LL21;_LL21: t=(void*)(vd->type=(void*)((void*)({struct
Cyc_Absyn_ArrayType_struct*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp1F;_tmp1F.tag=7;_tmp1F.f1=(void*)_tmp13;_tmp1F.f2=
_tmp12;_tmp1F.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp19),0);_tmp1F;});_tmp1E;})));goto
_LL19;_LL23: goto _LL19;_LL19:;}goto _LL10;_LL14: goto _LL10;_LL10:;}Cyc_Tcutil_check_valid_toplevel_type(
loc,te,t);{void*_tmp20=Cyc_Tcutil_compress(t);_LL2A: if((unsigned int)_tmp20 > 3?*((
int*)_tmp20)== 7: 0){goto _LL2B;}else{goto _LL2C;}_LL2C: goto _LL2D;_LL2B: vd->escapes=
0;goto _LL29;_LL2D: goto _LL29;_LL29:;}if(Cyc_Tcutil_is_function_type(t)){atts=Cyc_Tc_transfer_fn_type_atts(
t,atts);}if(sc == (void*)3? 1: sc == (void*)4){if(vd->initializer != 0){({void*_tmp21[
0]={};Cyc_Tcutil_terr(loc,_tag_arr("extern declaration should not have initializer",
sizeof(unsigned char),47),_tag_arr(_tmp21,sizeof(void*),0));});}}else{if(! Cyc_Tcutil_is_function_type(
t)){for(0;atts != 0;atts=atts->tl){void*_tmp22=(void*)atts->hd;_LL2F: if((
unsigned int)_tmp22 > 16?*((int*)_tmp22)== 1: 0){goto _LL30;}else{goto _LL31;}_LL31:
if((unsigned int)_tmp22 > 16?*((int*)_tmp22)== 2: 0){goto _LL32;}else{goto _LL33;}
_LL33: if((int)_tmp22 == 6){goto _LL34;}else{goto _LL35;}_LL35: if((int)_tmp22 == 7){
goto _LL36;}else{goto _LL37;}_LL37: if((int)_tmp22 == 8){goto _LL38;}else{goto _LL39;}
_LL39: if((int)_tmp22 == 9){goto _LL3A;}else{goto _LL3B;}_LL3B: if((int)_tmp22 == 10){
goto _LL3C;}else{goto _LL3D;}_LL3D: if((int)_tmp22 == 11){goto _LL3E;}else{goto _LL3F;}
_LL3F: goto _LL40;_LL30: goto _LL32;_LL32: goto _LL34;_LL34: goto _LL36;_LL36: goto _LL38;
_LL38: goto _LL3A;_LL3A: goto _LL3C;_LL3C: goto _LL3E;_LL3E: continue;_LL40:({struct Cyc_Std_String_pa_struct
_tmp25;_tmp25.tag=0;_tmp25.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{
struct Cyc_Std_String_pa_struct _tmp24;_tmp24.tag=0;_tmp24.f1=(struct _tagged_arr)
Cyc_Absyn_attribute2string((void*)atts->hd);{void*_tmp23[2]={& _tmp24,& _tmp25};
Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s for variable %s",sizeof(
unsigned char),33),_tag_arr(_tmp23,sizeof(void*),2));}}});goto _LL2E;_LL2E:;}if(
vd->initializer == 0){if(check_var_init? ! Cyc_Tcutil_supports_default(t): 0){({
struct Cyc_Std_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp27;_tmp27.tag=0;
_tmp27.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{void*_tmp26[2]={&
_tmp27,& _tmp28};Cyc_Tcutil_terr(loc,_tag_arr("initializer required for variable %s of type %s",
sizeof(unsigned char),48),_tag_arr(_tmp26,sizeof(void*),2));}}});}}else{struct
Cyc_Absyn_Exp*_tmp29=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);void*
_tmp2A=Cyc_Tcexp_tcExpInitializer(te,(void**)& t,_tmp29);if(! Cyc_Tcutil_coerce_assign(
te,_tmp29,t)){({struct Cyc_Std_String_pa_struct _tmp2E;_tmp2E.tag=0;_tmp2E.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp2A);{struct Cyc_Std_String_pa_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(vd->name);{void*_tmp2B[3]={& _tmp2C,& _tmp2D,& _tmp2E};Cyc_Tcutil_terr(
loc,_tag_arr("%s is declared with type \n%s\n but initialized with type \n%s",
sizeof(unsigned char),60),_tag_arr(_tmp2B,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}
if(! Cyc_Tcutil_is_const_exp(te,_tmp29)){({void*_tmp2F[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("initializer is not a constant expression",sizeof(unsigned char),41),
_tag_arr(_tmp2F,sizeof(void*),0));});}}}else{for(0;atts != 0;atts=atts->tl){void*
_tmp30=(void*)atts->hd;_LL42: if((unsigned int)_tmp30 > 16?*((int*)_tmp30)== 0: 0){
goto _LL43;}else{goto _LL44;}_LL44: if((int)_tmp30 == 0){goto _LL45;}else{goto _LL46;}
_LL46: if((int)_tmp30 == 1){goto _LL47;}else{goto _LL48;}_LL48: if((int)_tmp30 == 2){
goto _LL49;}else{goto _LL4A;}_LL4A: if((int)_tmp30 == 3){goto _LL4B;}else{goto _LL4C;}
_LL4C: if((unsigned int)_tmp30 > 16?*((int*)_tmp30)== 3: 0){goto _LL4D;}else{goto
_LL4E;}_LL4E: if((int)_tmp30 == 4){goto _LL4F;}else{goto _LL50;}_LL50: if((
unsigned int)_tmp30 > 16?*((int*)_tmp30)== 1: 0){goto _LL51;}else{goto _LL52;}_LL52:
if((int)_tmp30 == 5){goto _LL53;}else{goto _LL54;}_LL54: goto _LL55;_LL43: goto _LL45;
_LL45: goto _LL47;_LL47: goto _LL49;_LL49: goto _LL4B;_LL4B: goto _LL4D;_LL4D: goto _LL4F;
_LL4F:({void*_tmp31[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("tcVardecl: fn type attributes in function var decl",sizeof(
unsigned char),51),_tag_arr(_tmp31,sizeof(void*),0));});_LL51: goto _LL53;_LL53:({
struct Cyc_Std_String_pa_struct _tmp33;_tmp33.tag=0;_tmp33.f1=(struct _tagged_arr)
Cyc_Absyn_attribute2string((void*)atts->hd);{void*_tmp32[1]={& _tmp33};Cyc_Tcutil_terr(
loc,_tag_arr("bad attribute %s in function declaration",sizeof(unsigned char),41),
_tag_arr(_tmp32,sizeof(void*),1));}});goto _LL41;_LL55: continue;_LL41:;}}}{struct
_handler_cons _tmp34;_push_handler(& _tmp34);{int _tmp36=0;if(setjmp(_tmp34.handler)){
_tmp36=1;}if(! _tmp36){{struct _tuple4*_tmp37=((struct _tuple4*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,v);void*_tmp38=(*_tmp37).f1;
void*_tmp39;_LL57: if(*((int*)_tmp38)== 0){_LL61: _tmp39=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp38)->f1;goto _LL58;}else{goto _LL59;}_LL59: if(*((int*)_tmp38)== 1){goto _LL5A;}
else{goto _LL5B;}_LL5B: if(*((int*)_tmp38)== 2){goto _LL5C;}else{goto _LL5D;}_LL5D:
if(*((int*)_tmp38)== 4){goto _LL5E;}else{goto _LL5F;}_LL5F: if(*((int*)_tmp38)== 3){
goto _LL60;}else{goto _LL56;}_LL58: {struct Cyc_Absyn_Global_b_struct*_tmp3A=({
struct Cyc_Absyn_Global_b_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=({
struct Cyc_Absyn_Global_b_struct _tmp40;_tmp40.tag=0;_tmp40.f1=vd;_tmp40;});_tmp3F;});
void*_tmp3B=Cyc_Tcdecl_merge_binding(_tmp39,(void*)_tmp3A,loc,Cyc_Tc_tc_msg);if(
_tmp3B == (void*)0){goto _LL56;}if(_tmp3B == _tmp39?(*_tmp37).f2: 0){goto _LL56;}ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*
v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple4*_tmp3C=_cycalloc(sizeof(*
_tmp3C));_tmp3C->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp3D=_cycalloc(
sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Tcenv_VarRes_struct _tmp3E;_tmp3E.tag=0;
_tmp3E.f1=(void*)_tmp3B;_tmp3E;});_tmp3D;});_tmp3C->f2=1;_tmp3C;}));goto _LL56;}
_LL5A:({void*_tmp41[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous struct declaration",
sizeof(unsigned char),57),_tag_arr(_tmp41,sizeof(void*),0));});goto _LL56;_LL5C:({
void*_tmp42[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous [x]tunion constructor",
sizeof(unsigned char),60),_tag_arr(_tmp42,sizeof(void*),0));});goto _LL56;_LL5E:
goto _LL60;_LL60:({void*_tmp43[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous enum tag",
sizeof(unsigned char),47),_tag_arr(_tmp43,sizeof(void*),0));});goto _LL56;_LL56:;};
_pop_handler();}else{void*_tmp35=(void*)_exn_thrown;void*_tmp45=_tmp35;_LL63: if(
_tmp45 == Cyc_Dict_Absent){goto _LL64;}else{goto _LL65;}_LL65: goto _LL66;_LL64: ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*
v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple4*_tmp46=_cycalloc(sizeof(*
_tmp46));_tmp46->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp47=_cycalloc(
sizeof(*_tmp47));_tmp47[0]=({struct Cyc_Tcenv_VarRes_struct _tmp48;_tmp48.tag=0;
_tmp48.f1=(void*)((void*)({struct Cyc_Absyn_Global_b_struct*_tmp49=_cycalloc(
sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_Global_b_struct _tmp4A;_tmp4A.tag=0;
_tmp4A.f1=vd;_tmp4A;});_tmp49;}));_tmp48;});_tmp47;});_tmp46->f2=0;_tmp46;}));
goto _LL62;_LL66:(void)_throw(_tmp45);_LL62:;}}}}static void Cyc_Tc_tcFndecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
Cyc_Absyn_Fndecl*fd){struct _tagged_arr*v=(*fd->name).f2;if((void*)fd->sc == (void*)
4){({void*_tmp4B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("extern \"C\" functions cannot be implemented in Cyclone",
sizeof(unsigned char),54),_tag_arr(_tmp4B,sizeof(void*),0));});}{void*_tmp4C=(*
fd->name).f1;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*_tmp4E;_LL68: if((
unsigned int)_tmp4C > 1?*((int*)_tmp4C)== 0: 0){_LL6E: _tmp4D=((struct Cyc_Absyn_Rel_n_struct*)
_tmp4C)->f1;if(_tmp4D == 0){goto _LL69;}else{goto _LL6A;}}else{goto _LL6A;}_LL6A: if((
unsigned int)_tmp4C > 1?*((int*)_tmp4C)== 1: 0){_LL6F: _tmp4E=((struct Cyc_Absyn_Abs_n_struct*)
_tmp4C)->f1;goto _LL6B;}else{goto _LL6C;}_LL6C: goto _LL6D;_LL69: goto _LL67;_LL6B:({
void*_tmp4F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("tc: Abs_n in tcFndecl",sizeof(unsigned char),22),_tag_arr(_tmp4F,
sizeof(void*),0));});_LL6D:({struct Cyc_Std_String_pa_struct _tmp51;_tmp51.tag=0;
_tmp51.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(fd->name);{void*_tmp50[1]={&
_tmp51};Cyc_Tcutil_terr(loc,_tag_arr("qualified function declarations are not implemented (%s)",
sizeof(unsigned char),57),_tag_arr(_tmp50,sizeof(void*),1));}});return;_LL67:;}(*
fd->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp52=_cycalloc(sizeof(*
_tmp52));_tmp52[0]=({struct Cyc_Absyn_Abs_n_struct _tmp53;_tmp53.tag=1;_tmp53.f1=
te->ns;_tmp53;});_tmp52;});Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{void*t=
Cyc_Tcutil_fndecl2typ(fd);fd->attributes=Cyc_Tc_transfer_fn_type_atts(t,fd->attributes);{
struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){void*
_tmp54=(void*)atts->hd;_LL71: if((int)_tmp54 == 5){goto _LL72;}else{goto _LL73;}
_LL73: if((unsigned int)_tmp54 > 16?*((int*)_tmp54)== 1: 0){goto _LL74;}else{goto
_LL75;}_LL75: goto _LL76;_LL72: goto _LL74;_LL74:({struct Cyc_Std_String_pa_struct
_tmp56;_tmp56.tag=0;_tmp56.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp55[1]={& _tmp56};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s for function",
sizeof(unsigned char),30),_tag_arr(_tmp55,sizeof(void*),1));}});goto _LL70;_LL76:
goto _LL70;_LL70:;}}{struct _handler_cons _tmp57;_push_handler(& _tmp57);{int _tmp59=
0;if(setjmp(_tmp57.handler)){_tmp59=1;}if(! _tmp59){{struct _tuple4*_tmp5A=((
struct _tuple4*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,
v);void*_tmp5B=(*_tmp5A).f1;void*_tmp5C;_LL78: if(*((int*)_tmp5B)== 0){_LL82:
_tmp5C=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5B)->f1;goto _LL79;}else{goto
_LL7A;}_LL7A: if(*((int*)_tmp5B)== 1){goto _LL7B;}else{goto _LL7C;}_LL7C: if(*((int*)
_tmp5B)== 2){goto _LL7D;}else{goto _LL7E;}_LL7E: if(*((int*)_tmp5B)== 4){goto _LL7F;}
else{goto _LL80;}_LL80: if(*((int*)_tmp5B)== 3){goto _LL81;}else{goto _LL77;}_LL79: {
struct Cyc_Absyn_Funname_b_struct*_tmp5D=({struct Cyc_Absyn_Funname_b_struct*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Absyn_Funname_b_struct
_tmp63;_tmp63.tag=1;_tmp63.f1=fd;_tmp63;});_tmp62;});void*_tmp5E=Cyc_Tcdecl_merge_binding(
_tmp5C,(void*)_tmp5D,loc,Cyc_Tc_tc_msg);if(_tmp5E == (void*)0){goto _LL77;}if(
_tmp5E == _tmp5C?(*_tmp5A).f2: 0){goto _LL77;}ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,
v,({struct _tuple4*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->f1=(void*)({struct
Cyc_Tcenv_VarRes_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp61;_tmp61.tag=0;_tmp61.f1=(void*)_tmp5E;_tmp61;});_tmp60;});_tmp5F->f2=1;
_tmp5F;}));goto _LL77;}_LL7B:({void*_tmp64[0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous type declaration",
sizeof(unsigned char),55),_tag_arr(_tmp64,sizeof(void*),0));});goto _LL77;_LL7D:({
void*_tmp65[0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous [x]tunion constructor",
sizeof(unsigned char),60),_tag_arr(_tmp65,sizeof(void*),0));});goto _LL77;_LL7F:
goto _LL81;_LL81:({void*_tmp66[0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous enum tag",
sizeof(unsigned char),47),_tag_arr(_tmp66,sizeof(void*),0));});goto _LL77;_LL77:;};
_pop_handler();}else{void*_tmp58=(void*)_exn_thrown;void*_tmp68=_tmp58;_LL84: if(
_tmp68 == Cyc_Dict_Absent){goto _LL85;}else{goto _LL86;}_LL86: goto _LL87;_LL85: ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*
v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple4*_tmp69=_cycalloc(sizeof(*
_tmp69));_tmp69->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp6A=_cycalloc(
sizeof(*_tmp6A));_tmp6A[0]=({struct Cyc_Tcenv_VarRes_struct _tmp6B;_tmp6B.tag=0;
_tmp6B.f1=(void*)((void*)({struct Cyc_Absyn_Funname_b_struct*_tmp6C=_cycalloc(
sizeof(*_tmp6C));_tmp6C[0]=({struct Cyc_Absyn_Funname_b_struct _tmp6D;_tmp6D.tag=1;
_tmp6D.f1=fd;_tmp6D;});_tmp6C;}));_tmp6B;});_tmp6A;});_tmp69->f2=0;_tmp69;}));
goto _LL83;_LL87:(void)_throw(_tmp68);_LL83:;}}}te->le=({struct Cyc_Core_Opt*
_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->v=(void*)((void*)({struct Cyc_Tcenv_Outermost_struct*
_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_Tcenv_Outermost_struct
_tmp70;_tmp70.tag=0;_tmp70.f1=(void*)Cyc_Tcenv_new_fenv(loc,fd);_tmp70;});_tmp6F;}));
_tmp6E;});Cyc_Tcstmt_tcStmt(te,fd->body,0);if(! Cyc_Tcenv_all_labels_resolved(te)){({
void*_tmp71[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function has goto statements to undefined labels",
sizeof(unsigned char),49),_tag_arr(_tmp71,sizeof(void*),0));});}te->le=0;}}
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*td){struct _tagged_arr*
v=(*td->name).f2;{void*_tmp72=(*td->name).f1;struct Cyc_List_List*_tmp73;struct
Cyc_List_List*_tmp74;_LL89: if((unsigned int)_tmp72 > 1?*((int*)_tmp72)== 0: 0){
_LL8F: _tmp73=((struct Cyc_Absyn_Rel_n_struct*)_tmp72)->f1;if(_tmp73 == 0){goto
_LL8A;}else{goto _LL8B;}}else{goto _LL8B;}_LL8B: if((unsigned int)_tmp72 > 1?*((int*)
_tmp72)== 1: 0){_LL90: _tmp74=((struct Cyc_Absyn_Abs_n_struct*)_tmp72)->f1;if(
_tmp74 == 0){goto _LL8C;}else{goto _LL8D;}}else{goto _LL8D;}_LL8D: goto _LL8E;_LL8A:
goto _LL8C;_LL8C: goto _LL88;_LL8E:({struct Cyc_Std_String_pa_struct _tmp76;_tmp76.tag=
0;_tmp76.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(td->name);{void*_tmp75[1]={&
_tmp76};Cyc_Tcutil_terr(loc,_tag_arr("qualified typedef declarations are not implemented (%s)",
sizeof(unsigned char),56),_tag_arr(_tmp75,sizeof(void*),1));}});return;_LL88:;}
if(((int(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_member)(ge->typedefs,
v)){({struct Cyc_Std_String_pa_struct _tmp78;_tmp78.tag=0;_tmp78.f1=(struct
_tagged_arr)*v;{void*_tmp77[1]={& _tmp78};Cyc_Tcutil_terr(loc,_tag_arr("redeclaration of typedef %s",
sizeof(unsigned char),28),_tag_arr(_tmp77,sizeof(void*),1));}});return;}(*td->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=({
struct Cyc_Absyn_Abs_n_struct _tmp7A;_tmp7A.tag=1;_tmp7A.f1=te->ns;_tmp7A;});
_tmp79;});Cyc_Tcutil_check_unique_tvars(loc,td->tvs);Cyc_Tcutil_add_tvar_identities(
td->tvs);Cyc_Tcutil_check_type(loc,te,td->tvs,(void*)0,0,(void*)td->defn);ge->typedefs=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Typedefdecl*
v))Cyc_Dict_insert)(ge->typedefs,v,td);}static void Cyc_Tc_tcAggrFields(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_List_List*
fields,struct Cyc_List_List*tvs){struct _RegionHandle _tmp7B=_new_region("uprev_rgn");
struct _RegionHandle*uprev_rgn=& _tmp7B;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;struct Cyc_List_List*_tmp7C=fields;for(0;_tmp7C != 0;_tmp7C=_tmp7C->tl){
struct Cyc_Absyn_Aggrfield _tmp7E;struct Cyc_List_List*_tmp7F;struct Cyc_Absyn_Exp*
_tmp80;void*_tmp81;struct Cyc_Absyn_Tqual _tmp82;struct _tagged_arr*_tmp83;struct
Cyc_Absyn_Aggrfield*_tmp7D=(struct Cyc_Absyn_Aggrfield*)_tmp7C->hd;_tmp7E=*_tmp7D;
_LL96: _tmp83=_tmp7E.name;goto _LL95;_LL95: _tmp82=_tmp7E.tq;goto _LL94;_LL94: _tmp81=(
void*)_tmp7E.type;goto _LL93;_LL93: _tmp80=_tmp7E.width;goto _LL92;_LL92: _tmp7F=
_tmp7E.attributes;goto _LL91;_LL91: if(((int(*)(int(*compare)(struct _tagged_arr*,
struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,
prev_fields,_tmp83)){({struct Cyc_Std_String_pa_struct _tmp85;_tmp85.tag=0;_tmp85.f1=(
struct _tagged_arr)*_tmp83;{void*_tmp84[1]={& _tmp85};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s",
sizeof(unsigned char),19),_tag_arr(_tmp84,sizeof(void*),1));}});}if(Cyc_Std_strcmp(*
_tmp83,_tag_arr("",sizeof(unsigned char),1))!= 0){prev_fields=({struct Cyc_List_List*
_tmp86=_region_malloc(uprev_rgn,sizeof(*_tmp86));_tmp86->hd=_tmp83;_tmp86->tl=
prev_fields;_tmp86;});}Cyc_Tcutil_check_type(loc,te,tvs,(void*)1,0,_tmp81);Cyc_Tcutil_check_bitfield(
loc,te,_tmp81,_tmp80,_tmp83);}};_pop_region(uprev_rgn);}struct _tuple5{struct Cyc_Core_Opt*
f1;struct Cyc_Core_Opt*f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tagged_arr*_tmp87=(*ad->name).f2;{struct Cyc_List_List*atts=ad->attributes;
for(0;atts != 0;atts=atts->tl){void*_tmp88=(void*)atts->hd;_LL98: if((int)_tmp88 == 
5){goto _LL99;}else{goto _LL9A;}_LL9A: if((unsigned int)_tmp88 > 16?*((int*)_tmp88)
== 1: 0){goto _LL9B;}else{goto _LL9C;}_LL9C: goto _LL9D;_LL99: goto _LL9B;_LL9B:
continue;_LL9D:({struct Cyc_Std_String_pa_struct _tmp8B;_tmp8B.tag=0;_tmp8B.f1=(
struct _tagged_arr)*_tmp87;{struct Cyc_Std_String_pa_struct _tmp8A;_tmp8A.tag=0;
_tmp8A.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd);{void*
_tmp89[2]={& _tmp8A,& _tmp8B};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s in  %s definition",
sizeof(unsigned char),35),_tag_arr(_tmp89,sizeof(void*),2));}}});goto _LL97;_LL97:;}}{
struct Cyc_List_List*_tmp8C=ad->tvs;{struct Cyc_List_List*tvs2=_tmp8C;for(0;tvs2 != 
0;tvs2=tvs2->tl){void*_tmp8D=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp8E;struct Cyc_Core_Opt**_tmp8F;void*
_tmp90;struct Cyc_Core_Opt*_tmp91;struct Cyc_Core_Opt**_tmp92;void*_tmp93;struct
Cyc_Core_Opt*_tmp94;struct Cyc_Core_Opt**_tmp95;void*_tmp96;_LL9F: if(*((int*)
_tmp8D)== 1){_LLA9: _tmp8E=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8D)->f1;
_tmp8F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8D)->f1;
goto _LLA0;}else{goto _LLA1;}_LLA1: if(*((int*)_tmp8D)== 2){_LLAB: _tmp91=((struct
Cyc_Absyn_Less_kb_struct*)_tmp8D)->f1;_tmp92=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D)->f1;goto _LLAA;_LLAA: _tmp90=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D)->f2;if((int)_tmp90 == 1){goto _LLA2;}else{goto _LLA3;}}else{goto _LLA3;}
_LLA3: if(*((int*)_tmp8D)== 2){_LLAD: _tmp94=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D)->f1;_tmp95=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D)->f1;goto _LLAC;_LLAC: _tmp93=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D)->f2;if((int)_tmp93 == 0){goto _LLA4;}else{goto _LLA5;}}else{goto _LLA5;}
_LLA5: if(*((int*)_tmp8D)== 0){_LLAE: _tmp96=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp8D)->f1;if((int)_tmp96 == 1){goto _LLA6;}else{goto _LLA7;}}else{goto _LLA7;}
_LLA7: goto _LLA8;_LLA0: _tmp92=_tmp8F;goto _LLA2;_LLA2: _tmp95=_tmp92;goto _LLA4;
_LLA4:*_tmp95=({struct Cyc_Core_Opt*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp98=_cycalloc(sizeof(*_tmp98));
_tmp98[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp99;_tmp99.tag=0;_tmp99.f1=(void*)((
void*)2);_tmp99;});_tmp98;}));_tmp97;});continue;_LLA6:({struct Cyc_Std_String_pa_struct
_tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name;{
struct Cyc_Std_String_pa_struct _tmp9B;_tmp9B.tag=0;_tmp9B.f1=(struct _tagged_arr)*
_tmp87;{void*_tmp9A[2]={& _tmp9B,& _tmp9C};Cyc_Tcutil_terr(loc,_tag_arr("type %s attempts to abstract type variable %s of kind M",
sizeof(unsigned char),56),_tag_arr(_tmp9A,sizeof(void*),2));}}});continue;_LLA8:
continue;_LL9E:;}}{void*_tmp9D=(*ad->name).f1;struct Cyc_List_List*_tmp9E;struct
Cyc_List_List*_tmp9F;_LLB0: if((unsigned int)_tmp9D > 1?*((int*)_tmp9D)== 0: 0){
_LLB6: _tmp9E=((struct Cyc_Absyn_Rel_n_struct*)_tmp9D)->f1;if(_tmp9E == 0){goto
_LLB1;}else{goto _LLB2;}}else{goto _LLB2;}_LLB2: if((unsigned int)_tmp9D > 1?*((int*)
_tmp9D)== 1: 0){_LLB7: _tmp9F=((struct Cyc_Absyn_Abs_n_struct*)_tmp9D)->f1;if(
_tmp9F == 0){goto _LLB3;}else{goto _LLB4;}}else{goto _LLB4;}_LLB4: goto _LLB5;_LLB1:
goto _LLB3;_LLB3: goto _LLAF;_LLB5:({struct Cyc_Std_String_pa_struct _tmpA1;_tmpA1.tag=
0;_tmpA1.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(ad->name);{void*_tmpA0[1]={&
_tmpA1};Cyc_Tcutil_terr(loc,_tag_arr("qualified struct declarations are not implemented (%s)",
sizeof(unsigned char),55),_tag_arr(_tmpA0,sizeof(void*),1));}});return;_LLAF:;}(*
ad->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmpA2=_cycalloc(sizeof(*
_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_Abs_n_struct _tmpA3;_tmpA3.tag=1;_tmpA3.f1=
te->ns;_tmpA3;});_tmpA2;});Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(
ad->tvs);{struct _tuple5 _tmpA5=({struct _tuple5 _tmpA4;_tmpA4.f1=ad->fields;_tmpA4.f2=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(
ge->aggrdecls,_tmp87);_tmpA4;});struct Cyc_Core_Opt*_tmpA6;struct Cyc_Core_Opt*
_tmpA7;struct Cyc_Core_Opt*_tmpA8;struct Cyc_Core_Opt*_tmpA9;struct Cyc_Core_Opt
_tmpAA;struct Cyc_List_List*_tmpAB;struct Cyc_Core_Opt*_tmpAC;struct Cyc_Core_Opt
_tmpAD;struct Cyc_Absyn_Aggrdecl**_tmpAE;struct Cyc_Core_Opt*_tmpAF;struct Cyc_Core_Opt
_tmpB0;struct Cyc_List_List*_tmpB1;struct Cyc_Core_Opt*_tmpB2;struct Cyc_Core_Opt
_tmpB3;struct Cyc_Absyn_Aggrdecl**_tmpB4;struct Cyc_Core_Opt*_tmpB5;_LLB9: _LLC2:
_tmpA7=_tmpA5.f1;if(_tmpA7 == 0){goto _LLC1;}else{goto _LLBB;}_LLC1: _tmpA6=_tmpA5.f2;
if(_tmpA6 == 0){goto _LLBA;}else{goto _LLBB;}_LLBB: _LLC4: _tmpA9=_tmpA5.f1;if(_tmpA9
== 0){goto _LLBD;}else{_tmpAA=*_tmpA9;_LLC5: _tmpAB=(struct Cyc_List_List*)_tmpAA.v;
goto _LLC3;}_LLC3: _tmpA8=_tmpA5.f2;if(_tmpA8 == 0){goto _LLBC;}else{goto _LLBD;}
_LLBD: _LLC8: _tmpAF=_tmpA5.f1;if(_tmpAF == 0){goto _LLBF;}else{_tmpB0=*_tmpAF;_LLC9:
_tmpB1=(struct Cyc_List_List*)_tmpB0.v;goto _LLC6;}_LLC6: _tmpAC=_tmpA5.f2;if(
_tmpAC == 0){goto _LLBF;}else{_tmpAD=*_tmpAC;_LLC7: _tmpAE=(struct Cyc_Absyn_Aggrdecl**)
_tmpAD.v;goto _LLBE;}_LLBF: _LLCC: _tmpB5=_tmpA5.f1;if(_tmpB5 == 0){goto _LLCA;}else{
goto _LLB8;}_LLCA: _tmpB2=_tmpA5.f2;if(_tmpB2 == 0){goto _LLB8;}else{_tmpB3=*_tmpB2;
_LLCB: _tmpB4=(struct Cyc_Absyn_Aggrdecl**)_tmpB3.v;goto _LLC0;}_LLBA: ge->aggrdecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl**
v))Cyc_Dict_insert)(ge->aggrdecls,_tmp87,({struct Cyc_Absyn_Aggrdecl**_tmpB6=
_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=ad;_tmpB6;}));goto _LLB8;_LLBC: {struct Cyc_Absyn_Aggrdecl**
_tmpB7=({struct Cyc_Absyn_Aggrdecl**_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({
struct Cyc_Absyn_Aggrdecl*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->kind=(void*)((
void*)ad->kind);_tmpBD->sc=(void*)((void*)3);_tmpBD->name=ad->name;_tmpBD->tvs=
_tmp8C;_tmpBD->exist_vars=0;_tmpBD->fields=0;_tmpBD->attributes=ad->attributes;
_tmpBD;});_tmpBC;});ge->aggrdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,
_tmp87,_tmpB7);Cyc_Tcutil_check_unique_tvars(loc,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(ad->exist_vars))->v);Cyc_Tcutil_add_tvar_identities((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ad->exist_vars))->v);Cyc_Tc_tcAggrFields(
te,ge,loc,_tmpAB,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp8C,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ad->exist_vars))->v));if((void*)ad->kind == (void*)1){struct Cyc_List_List*
f=_tmpAB;for(0;f != 0;f=f->tl){if(! Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
f->hd)->type)){({struct Cyc_Std_String_pa_struct _tmpBB;_tmpBB.tag=0;_tmpBB.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Absyn_Aggrfield*)f->hd)->type);{
struct Cyc_Std_String_pa_struct _tmpBA;_tmpBA.tag=0;_tmpBA.f1=(struct _tagged_arr)*
_tmp87;{struct Cyc_Std_String_pa_struct _tmpB9;_tmpB9.tag=0;_tmpB9.f1=(struct
_tagged_arr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name;{void*_tmpB8[3]={& _tmpB9,&
_tmpBA,& _tmpBB};Cyc_Tcutil_terr(loc,_tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof(unsigned char),58),_tag_arr(_tmpB8,sizeof(void*),3));}}}});}}}*_tmpB7=ad;
goto _LLB8;}_LLBE: if((void*)ad->kind != (void*)(*_tmpAE)->kind){({void*_tmpBE[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("cannot reuse struct names for unions and vice-versa",
sizeof(unsigned char),52),_tag_arr(_tmpBE,sizeof(void*),0));});}{struct Cyc_Absyn_Aggrdecl*
_tmpBF=*_tmpAE;*_tmpAE=({struct Cyc_Absyn_Aggrdecl*_tmpC0=_cycalloc(sizeof(*
_tmpC0));_tmpC0->kind=(void*)((void*)ad->kind);_tmpC0->sc=(void*)((void*)3);
_tmpC0->name=ad->name;_tmpC0->tvs=_tmp8C;_tmpC0->exist_vars=0;_tmpC0->fields=0;
_tmpC0->attributes=ad->attributes;_tmpC0;});Cyc_Tcutil_check_unique_tvars(loc,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ad->exist_vars))->v);Cyc_Tcutil_add_tvar_identities((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ad->exist_vars))->v);Cyc_Tc_tcAggrFields(
te,ge,loc,_tmpB1,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp8C,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ad->exist_vars))->v));*_tmpAE=_tmpBF;_tmpB4=_tmpAE;goto _LLC0;}_LLC0: {
struct Cyc_Absyn_Aggrdecl*_tmpC1=Cyc_Tcdecl_merge_aggrdecl(*_tmpB4,ad,loc,Cyc_Tc_tc_msg);
if(_tmpC1 == 0){return;}else{*_tmpB4=(struct Cyc_Absyn_Aggrdecl*)_check_null(
_tmpC1);ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(_tmpC1);goto _LLB8;}}_LLB8:;}ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*
v))Cyc_Dict_insert)(ge->ordinaries,_tmp87,({struct _tuple4*_tmpC2=_cycalloc(
sizeof(*_tmpC2));_tmpC2->f1=(void*)({struct Cyc_Tcenv_AggrRes_struct*_tmpC3=
_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Tcenv_AggrRes_struct _tmpC4;
_tmpC4.tag=1;_tmpC4.f1=ad;_tmpC4;});_tmpC3;});_tmpC2->f2=1;_tmpC2;}));}}struct
_tuple6{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_Tc_tcTunionFields(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct _tagged_arr obj,int is_xtunion,struct _tuple0*name,struct Cyc_List_List*fields,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tuniondecl*tudres){{struct Cyc_List_List*
_tmpC5=fields;for(0;_tmpC5 != 0;_tmpC5=_tmpC5->tl){struct Cyc_Absyn_Tunionfield*
_tmpC6=(struct Cyc_Absyn_Tunionfield*)_tmpC5->hd;{struct Cyc_List_List*typs=_tmpC6->typs;
for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmpC6->loc,te,tvs,(void*)1,
0,(*((struct _tuple6*)typs->hd)).f2);}}{void*_tmpC7=(*_tmpC6->name).f1;struct Cyc_List_List*
_tmpC8;_LLCE: if((unsigned int)_tmpC7 > 1?*((int*)_tmpC7)== 0: 0){_LLD6: _tmpC8=((
struct Cyc_Absyn_Rel_n_struct*)_tmpC7)->f1;if(_tmpC8 == 0){goto _LLCF;}else{goto
_LLD0;}}else{goto _LLD0;}_LLD0: if((unsigned int)_tmpC7 > 1?*((int*)_tmpC7)== 0: 0){
goto _LLD1;}else{goto _LLD2;}_LLD2: if((unsigned int)_tmpC7 > 1?*((int*)_tmpC7)== 1:
0){goto _LLD3;}else{goto _LLD4;}_LLD4: if((int)_tmpC7 == 0){goto _LLD5;}else{goto
_LLCD;}_LLCF: if(is_xtunion){(*_tmpC6->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_Abs_n_struct _tmpCA;
_tmpCA.tag=1;_tmpCA.f1=te->ns;_tmpCA;});_tmpC9;});}else{(*_tmpC6->name).f1=(*
name).f1;}goto _LLCD;_LLD1:({struct Cyc_Std_String_pa_struct _tmpCC;_tmpCC.tag=0;
_tmpCC.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpC6->name);{void*_tmpCB[
1]={& _tmpCC};Cyc_Tcutil_terr(_tmpC6->loc,_tag_arr("qualified tunionfield declarations are not allowed (%s)",
sizeof(unsigned char),56),_tag_arr(_tmpCB,sizeof(void*),1));}});goto _LLCD;_LLD3:
goto _LLCD;_LLD5:({void*_tmpCD[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcTunionFields: Loc_n",sizeof(
unsigned char),22),_tag_arr(_tmpCD,sizeof(void*),0));});_LLCD:;}}}{struct Cyc_List_List*
fields2;if(is_xtunion){int _tmpCE=1;struct Cyc_List_List*_tmpCF=Cyc_Tcdecl_sort_xtunion_fields(
fields,& _tmpCE,(*name).f2,loc,Cyc_Tc_tc_msg);if(_tmpCE){fields2=_tmpCF;}else{
fields2=0;}}else{struct _RegionHandle _tmpD0=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmpD0;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Tunionfield*
_tmpD1=(struct Cyc_Absyn_Tunionfield*)fs->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmpD1->name).f2)){({struct Cyc_Std_String_pa_struct
_tmpD4;_tmpD4.tag=0;_tmpD4.f1=(struct _tagged_arr)obj;{struct Cyc_Std_String_pa_struct
_tmpD3;_tmpD3.tag=0;_tmpD3.f1=(struct _tagged_arr)*(*_tmpD1->name).f2;{void*
_tmpD2[2]={& _tmpD3,& _tmpD4};Cyc_Tcutil_terr(_tmpD1->loc,_tag_arr("duplicate field name %s in %s",
sizeof(unsigned char),30),_tag_arr(_tmpD2,sizeof(void*),2));}}});}else{
prev_fields=({struct Cyc_List_List*_tmpD5=_region_malloc(uprev_rgn,sizeof(*_tmpD5));
_tmpD5->hd=(*_tmpD1->name).f2;_tmpD5->tl=prev_fields;_tmpD5;});}if((void*)_tmpD1->sc
!= (void*)2){({struct Cyc_Std_String_pa_struct _tmpD7;_tmpD7.tag=0;_tmpD7.f1=(
struct _tagged_arr)*(*_tmpD1->name).f2;{void*_tmpD6[1]={& _tmpD7};Cyc_Tcutil_warn(
loc,_tag_arr("ignoring scope of field %s",sizeof(unsigned char),27),_tag_arr(
_tmpD6,sizeof(void*),1));}});(void*)(_tmpD1->sc=(void*)((void*)2));}}}fields2=
fields;};_pop_region(uprev_rgn);}{struct Cyc_List_List*_tmpD8=fields;for(0;_tmpD8
!= 0;_tmpD8=_tmpD8->tl){struct Cyc_Absyn_Tunionfield*_tmpD9=(struct Cyc_Absyn_Tunionfield*)
_tmpD8->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmpD9->name).f2,({
struct _tuple4*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*
_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Tcenv_TunionRes_struct
_tmpDC;_tmpDC.tag=2;_tmpDC.f1=tudres;_tmpDC.f2=_tmpD9;_tmpDC;});_tmpDB;});_tmpDA->f2=
1;_tmpDA;}));}}return fields2;}}void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl*
tud){struct _tagged_arr*v=(*tud->name).f2;struct _tagged_arr obj=tud->is_xtunion?
_tag_arr("xtunion",sizeof(unsigned char),8): _tag_arr("tunion",sizeof(
unsigned char),7);struct Cyc_List_List*tvs=tud->tvs;{struct Cyc_List_List*tvs2=tvs;
for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmpDD=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmpDE;struct Cyc_Core_Opt**
_tmpDF;void*_tmpE0;struct Cyc_Core_Opt*_tmpE1;struct Cyc_Core_Opt**_tmpE2;void*
_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_Core_Opt**_tmpE5;void*_tmpE6;_LLD8:
if(*((int*)_tmpDD)== 1){_LLE2: _tmpDE=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpDD)->f1;
_tmpDF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmpDD)->f1;
goto _LLD9;}else{goto _LLDA;}_LLDA: if(*((int*)_tmpDD)== 2){_LLE4: _tmpE1=((struct
Cyc_Absyn_Less_kb_struct*)_tmpDD)->f1;_tmpE2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpDD)->f1;goto _LLE3;_LLE3: _tmpE0=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpDD)->f2;if((int)_tmpE0 == 1){goto _LLDB;}else{goto _LLDC;}}else{goto _LLDC;}
_LLDC: if(*((int*)_tmpDD)== 2){_LLE6: _tmpE4=((struct Cyc_Absyn_Less_kb_struct*)
_tmpDD)->f1;_tmpE5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpDD)->f1;goto _LLE5;_LLE5: _tmpE3=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpDD)->f2;if((int)_tmpE3 == 0){goto _LLDD;}else{goto _LLDE;}}else{goto _LLDE;}
_LLDE: if(*((int*)_tmpDD)== 0){_LLE7: _tmpE6=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpDD)->f1;if((int)_tmpE6 == 1){goto _LLDF;}else{goto _LLE0;}}else{goto _LLE0;}
_LLE0: goto _LLE1;_LLD9: _tmpE2=_tmpDF;goto _LLDB;_LLDB: _tmpE5=_tmpE2;goto _LLDD;
_LLDD:*_tmpE5=({struct Cyc_Core_Opt*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpE8=_cycalloc(sizeof(*_tmpE8));
_tmpE8[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpE9;_tmpE9.tag=0;_tmpE9.f1=(void*)((
void*)2);_tmpE9;});_tmpE8;}));_tmpE7;});goto _LLD7;_LLDF:({struct Cyc_Std_String_pa_struct
_tmpED;_tmpED.tag=0;_tmpED.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name;{
struct Cyc_Std_String_pa_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=(struct _tagged_arr)*
v;{struct Cyc_Std_String_pa_struct _tmpEB;_tmpEB.tag=0;_tmpEB.f1=(struct
_tagged_arr)obj;{void*_tmpEA[3]={& _tmpEB,& _tmpEC,& _tmpED};Cyc_Tcutil_terr(loc,
_tag_arr("%s %s attempts to abstract type variable %s of kind M",sizeof(
unsigned char),54),_tag_arr(_tmpEA,sizeof(void*),3));}}}});goto _LLD7;_LLE1: goto
_LLD7;_LLD7:;}}Cyc_Tcutil_check_unique_tvars(loc,tvs);Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_Core_Opt*tud_opt;if(tud->is_xtunion){{struct _handler_cons _tmpEE;
_push_handler(& _tmpEE);{int _tmpF0=0;if(setjmp(_tmpEE.handler)){_tmpF0=1;}if(!
_tmpF0){tud_opt=Cyc_Tcenv_lookup_xtuniondecl(te,loc,tud->name);;_pop_handler();}
else{void*_tmpEF=(void*)_exn_thrown;void*_tmpF2=_tmpEF;_LLE9: if(_tmpF2 == Cyc_Dict_Absent){
goto _LLEA;}else{goto _LLEB;}_LLEB: goto _LLEC;_LLEA:({struct Cyc_Std_String_pa_struct
_tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{
void*_tmpF3[1]={& _tmpF4};Cyc_Tcutil_terr(loc,_tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof(unsigned char),60),_tag_arr(_tmpF3,sizeof(void*),1));}});return;_LLEC:(
void)_throw(_tmpF2);_LLE8:;}}}if(tud_opt != 0){tud->name=(*((struct Cyc_Absyn_Tuniondecl**)
tud_opt->v))->name;}else{(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[0]=({struct Cyc_Absyn_Abs_n_struct _tmpF6;
_tmpF6.tag=1;_tmpF6.f1=te->ns;_tmpF6;});_tmpF5;});}}else{{void*_tmpF7=(*tud->name).f1;
struct Cyc_List_List*_tmpF8;_LLEE: if((unsigned int)_tmpF7 > 1?*((int*)_tmpF7)== 0:
0){_LLF4: _tmpF8=((struct Cyc_Absyn_Rel_n_struct*)_tmpF7)->f1;if(_tmpF8 == 0){goto
_LLEF;}else{goto _LLF0;}}else{goto _LLF0;}_LLF0: if((unsigned int)_tmpF7 > 1?*((int*)
_tmpF7)== 1: 0){goto _LLF1;}else{goto _LLF2;}_LLF2: goto _LLF3;_LLEF:(*tud->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({
struct Cyc_Absyn_Abs_n_struct _tmpFA;_tmpFA.tag=1;_tmpFA.f1=te->ns;_tmpFA;});
_tmpF9;});goto _LLED;_LLF1: goto _LLF3;_LLF3:({struct Cyc_Std_String_pa_struct _tmpFC;
_tmpFC.tag=0;_tmpFC.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{
void*_tmpFB[1]={& _tmpFC};Cyc_Tcutil_terr(loc,_tag_arr("qualified tunion declarations are not implemented (%s)",
sizeof(unsigned char),55),_tag_arr(_tmpFB,sizeof(void*),1));}});return;_LLED:;}
tud_opt=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(
ge->tuniondecls,v);}{struct _tuple5 _tmpFE=({struct _tuple5 _tmpFD;_tmpFD.f1=tud->fields;
_tmpFD.f2=tud_opt;_tmpFD;});struct Cyc_Core_Opt*_tmpFF;struct Cyc_Core_Opt*_tmp100;
struct Cyc_Core_Opt*_tmp101;struct Cyc_Core_Opt*_tmp102;struct Cyc_Core_Opt _tmp103;
struct Cyc_List_List*_tmp104;struct Cyc_List_List**_tmp105;struct Cyc_Core_Opt*
_tmp106;struct Cyc_Core_Opt _tmp107;struct Cyc_Absyn_Tuniondecl**_tmp108;struct Cyc_Core_Opt*
_tmp109;struct Cyc_Core_Opt _tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_List_List**
_tmp10C;struct Cyc_Core_Opt*_tmp10D;struct Cyc_Core_Opt _tmp10E;struct Cyc_Absyn_Tuniondecl**
_tmp10F;struct Cyc_Core_Opt*_tmp110;_LLF6: _LLFF: _tmp100=_tmpFE.f1;if(_tmp100 == 0){
goto _LLFE;}else{goto _LLF8;}_LLFE: _tmpFF=_tmpFE.f2;if(_tmpFF == 0){goto _LLF7;}
else{goto _LLF8;}_LLF8: _LL101: _tmp102=_tmpFE.f1;if(_tmp102 == 0){goto _LLFA;}else{
_tmp103=*_tmp102;_LL102: _tmp104=(struct Cyc_List_List*)_tmp103.v;_tmp105=(struct
Cyc_List_List**)&(*_tmpFE.f1).v;goto _LL100;}_LL100: _tmp101=_tmpFE.f2;if(_tmp101
== 0){goto _LLF9;}else{goto _LLFA;}_LLFA: _LL105: _tmp109=_tmpFE.f1;if(_tmp109 == 0){
goto _LLFC;}else{_tmp10A=*_tmp109;_LL106: _tmp10B=(struct Cyc_List_List*)_tmp10A.v;
_tmp10C=(struct Cyc_List_List**)&(*_tmpFE.f1).v;goto _LL103;}_LL103: _tmp106=_tmpFE.f2;
if(_tmp106 == 0){goto _LLFC;}else{_tmp107=*_tmp106;_LL104: _tmp108=(struct Cyc_Absyn_Tuniondecl**)
_tmp107.v;goto _LLFB;}_LLFC: _LL109: _tmp110=_tmpFE.f1;if(_tmp110 == 0){goto _LL107;}
else{goto _LLF5;}_LL107: _tmp10D=_tmpFE.f2;if(_tmp10D == 0){goto _LLF5;}else{_tmp10E=*
_tmp10D;_LL108: _tmp10F=(struct Cyc_Absyn_Tuniondecl**)_tmp10E.v;goto _LLFD;}_LLF7:
ge->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(ge->tuniondecls,v,({
struct Cyc_Absyn_Tuniondecl**_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=tud;
_tmp111;}));goto _LLF5;_LLF9: {struct Cyc_Absyn_Tuniondecl**_tmp112=({struct Cyc_Absyn_Tuniondecl**
_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Absyn_Tuniondecl*
_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->sc=(void*)((void*)3);_tmp114->name=
tud->name;_tmp114->tvs=tvs;_tmp114->fields=0;_tmp114->is_xtunion=tud->is_xtunion;
_tmp114;});_tmp113;});ge->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(ge->tuniondecls,
v,_tmp112);*_tmp105=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,tud->name,*
_tmp105,tvs,tud);*_tmp112=tud;goto _LLF5;}_LLFB: {struct Cyc_Absyn_Tuniondecl*
_tmp115=*_tmp108;*_tmp108=({struct Cyc_Absyn_Tuniondecl*_tmp116=_cycalloc(sizeof(*
_tmp116));_tmp116->sc=(void*)((void*)3);_tmp116->name=tud->name;_tmp116->tvs=tvs;
_tmp116->fields=0;_tmp116->is_xtunion=tud->is_xtunion;_tmp116;});*_tmp10C=Cyc_Tc_tcTunionFields(
te,ge,loc,obj,tud->is_xtunion,tud->name,*_tmp10C,tvs,tud);*_tmp108=_tmp115;
_tmp10F=_tmp108;goto _LLFD;}_LLFD: {struct Cyc_Absyn_Tuniondecl*_tmp117=Cyc_Tcdecl_merge_tuniondecl(*
_tmp10F,tud,loc,Cyc_Tc_tc_msg);if(_tmp117 == 0){return;}else{*_tmp10F=(struct Cyc_Absyn_Tuniondecl*)
_check_null(_tmp117);goto _LLF5;}}_LLF5:;}}}void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl*
ed){struct _tagged_arr*v=(*ed->name).f2;{void*_tmp118=(*ed->name).f1;struct Cyc_List_List*
_tmp119;struct Cyc_List_List*_tmp11A;_LL10B: if((unsigned int)_tmp118 > 1?*((int*)
_tmp118)== 0: 0){_LL111: _tmp119=((struct Cyc_Absyn_Rel_n_struct*)_tmp118)->f1;if(
_tmp119 == 0){goto _LL10C;}else{goto _LL10D;}}else{goto _LL10D;}_LL10D: if((
unsigned int)_tmp118 > 1?*((int*)_tmp118)== 1: 0){_LL112: _tmp11A=((struct Cyc_Absyn_Abs_n_struct*)
_tmp118)->f1;if(_tmp11A == 0){goto _LL10E;}else{goto _LL10F;}}else{goto _LL10F;}
_LL10F: goto _LL110;_LL10C: goto _LL10E;_LL10E: goto _LL10A;_LL110:({struct Cyc_Std_String_pa_struct
_tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(ed->name);{
void*_tmp11B[1]={& _tmp11C};Cyc_Tcutil_terr(loc,_tag_arr("qualified enum declarations are not implemented (%s)",
sizeof(unsigned char),53),_tag_arr(_tmp11B,sizeof(void*),1));}});return;_LL10A:;}(*
ed->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp11D=_cycalloc(sizeof(*
_tmp11D));_tmp11D[0]=({struct Cyc_Absyn_Abs_n_struct _tmp11E;_tmp11E.tag=1;_tmp11E.f1=
te->ns;_tmp11E;});_tmp11D;});if(ed->fields != 0){struct _RegionHandle _tmp11F=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp11F;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=0;struct Cyc_List_List*
fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;
fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*_tmp120=(struct Cyc_Absyn_Enumfield*)
fs->hd;if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,
prev_fields,(*_tmp120->name).f2)){({struct Cyc_Std_String_pa_struct _tmp122;
_tmp122.tag=0;_tmp122.f1=(struct _tagged_arr)*(*_tmp120->name).f2;{void*_tmp121[1]={&
_tmp122};Cyc_Tcutil_terr(_tmp120->loc,_tag_arr("duplicate field name %s",sizeof(
unsigned char),24),_tag_arr(_tmp121,sizeof(void*),1));}});}else{prev_fields=({
struct Cyc_List_List*_tmp123=_region_malloc(uprev_rgn,sizeof(*_tmp123));_tmp123->hd=(*
_tmp120->name).f2;_tmp123->tl=prev_fields;_tmp123;});}if(_tmp120->tag == 0){
_tmp120->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp120->loc);}
else{if(! Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp120->tag))){({
struct Cyc_Std_String_pa_struct _tmp126;_tmp126.tag=0;_tmp126.f1=(struct
_tagged_arr)*(*_tmp120->name).f2;{struct Cyc_Std_String_pa_struct _tmp125;_tmp125.tag=
0;_tmp125.f1=(struct _tagged_arr)*v;{void*_tmp124[2]={& _tmp125,& _tmp126};Cyc_Tcutil_terr(
loc,_tag_arr("enum %s, field %s: expression is not constant",sizeof(
unsigned char),46),_tag_arr(_tmp124,sizeof(void*),2));}}});}}{unsigned int t1=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp120->tag));tag_count=t1 + 1;(*_tmp120->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp128;_tmp128.tag=1;_tmp128.f1=te->ns;_tmp128;});
_tmp127;});}}};_pop_region(uprev_rgn);}{struct _handler_cons _tmp129;_push_handler(&
_tmp129);{int _tmp12B=0;if(setjmp(_tmp129.handler)){_tmp12B=1;}if(! _tmp12B){{
struct Cyc_Absyn_Enumdecl**_tmp12C=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*
_tmp12D=Cyc_Tcdecl_merge_enumdecl(*_tmp12C,ed,loc,Cyc_Tc_tc_msg);if(_tmp12D == 0){
_npop_handler(0);return;}*_tmp12C=(struct Cyc_Absyn_Enumdecl*)_check_null(_tmp12D);};
_pop_handler();}else{void*_tmp12A=(void*)_exn_thrown;void*_tmp12F=_tmp12A;_LL114:
if(_tmp12F == Cyc_Dict_Absent){goto _LL115;}else{goto _LL116;}_LL116: goto _LL117;
_LL115: {struct Cyc_Absyn_Enumdecl**_tmp130=({struct Cyc_Absyn_Enumdecl**_tmp131=
_cycalloc(sizeof(*_tmp131));_tmp131[0]=ed;_tmp131;});ge->enumdecls=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(
ge->enumdecls,v,_tmp130);goto _LL113;}_LL117:(void)_throw(_tmp12F);_LL113:;}}}if(
ed->fields != 0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp132=(struct Cyc_Absyn_Enumfield*)fs->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp132->name).f2,({struct _tuple4*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->f1=(
void*)({struct Cyc_Tcenv_EnumRes_struct*_tmp134=_cycalloc(sizeof(*_tmp134));
_tmp134[0]=({struct Cyc_Tcenv_EnumRes_struct _tmp135;_tmp135.tag=3;_tmp135.f1=ed;
_tmp135.f2=_tmp132;_tmp135;});_tmp134;});_tmp133->f2=1;_tmp133;}));}}}static int
Cyc_Tc_okay_externC(struct Cyc_Position_Segment*loc,void*sc){void*_tmp136=sc;
_LL119: if((int)_tmp136 == 0){goto _LL11A;}else{goto _LL11B;}_LL11B: if((int)_tmp136
== 1){goto _LL11C;}else{goto _LL11D;}_LL11D: if((int)_tmp136 == 2){goto _LL11E;}else{
goto _LL11F;}_LL11F: if((int)_tmp136 == 3){goto _LL120;}else{goto _LL121;}_LL121: if((
int)_tmp136 == 4){goto _LL122;}else{goto _LL118;}_LL11A:({void*_tmp137[0]={};Cyc_Tcutil_warn(
loc,_tag_arr("static declaration nested within extern \"C\"",sizeof(
unsigned char),44),_tag_arr(_tmp137,sizeof(void*),0));});return 0;_LL11C:({void*
_tmp138[0]={};Cyc_Tcutil_warn(loc,_tag_arr("abstract declaration nested within extern \"C\"",
sizeof(unsigned char),46),_tag_arr(_tmp138,sizeof(void*),0));});return 0;_LL11E:
goto _LL120;_LL120: return 1;_LL122:({void*_tmp139[0]={};Cyc_Tcutil_warn(loc,
_tag_arr("nested extern \"C\" declaration",sizeof(unsigned char),30),_tag_arr(
_tmp139,sizeof(void*),0));});return 1;_LL118:;}static void Cyc_Tc_tc_decls(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_List_List*last=0;
struct Cyc_Dict_Dict*dict=Cyc_Tcgenrep_empty_typerep_dict();struct Cyc_List_List*
_tmp13A=ds0;for(0;_tmp13A != 0;(last=_tmp13A,_tmp13A=_tmp13A->tl)){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)_tmp13A->hd;struct Cyc_Position_Segment*loc=d->loc;void*
_tmp13B=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp13C;struct Cyc_Absyn_Fndecl*
_tmp13D;struct Cyc_Absyn_Typedefdecl*_tmp13E;struct Cyc_Absyn_Aggrdecl*_tmp13F;
struct Cyc_Absyn_Tuniondecl*_tmp140;struct Cyc_Absyn_Enumdecl*_tmp141;struct Cyc_List_List*
_tmp142;struct _tagged_arr*_tmp143;struct Cyc_List_List*_tmp144;struct _tuple0*
_tmp145;struct _tuple0 _tmp146;struct _tagged_arr*_tmp147;void*_tmp148;struct Cyc_List_List*
_tmp149;_LL124: if(*((int*)_tmp13B)== 2){goto _LL125;}else{goto _LL126;}_LL126: if(*((
int*)_tmp13B)== 3){goto _LL127;}else{goto _LL128;}_LL128: if(*((int*)_tmp13B)== 0){
_LL13A: _tmp13C=((struct Cyc_Absyn_Var_d_struct*)_tmp13B)->f1;goto _LL129;}else{
goto _LL12A;}_LL12A: if(*((int*)_tmp13B)== 1){_LL13B: _tmp13D=((struct Cyc_Absyn_Fn_d_struct*)
_tmp13B)->f1;goto _LL12B;}else{goto _LL12C;}_LL12C: if(*((int*)_tmp13B)== 7){_LL13C:
_tmp13E=((struct Cyc_Absyn_Typedef_d_struct*)_tmp13B)->f1;goto _LL12D;}else{goto
_LL12E;}_LL12E: if(*((int*)_tmp13B)== 4){_LL13D: _tmp13F=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp13B)->f1;goto _LL12F;}else{goto _LL130;}_LL130: if(*((int*)_tmp13B)== 5){_LL13E:
_tmp140=((struct Cyc_Absyn_Tunion_d_struct*)_tmp13B)->f1;goto _LL131;}else{goto
_LL132;}_LL132: if(*((int*)_tmp13B)== 6){_LL13F: _tmp141=((struct Cyc_Absyn_Enum_d_struct*)
_tmp13B)->f1;goto _LL133;}else{goto _LL134;}_LL134: if(*((int*)_tmp13B)== 8){_LL141:
_tmp143=((struct Cyc_Absyn_Namespace_d_struct*)_tmp13B)->f1;goto _LL140;_LL140:
_tmp142=((struct Cyc_Absyn_Namespace_d_struct*)_tmp13B)->f2;goto _LL135;}else{goto
_LL136;}_LL136: if(*((int*)_tmp13B)== 9){_LL143: _tmp145=((struct Cyc_Absyn_Using_d_struct*)
_tmp13B)->f1;_tmp146=*_tmp145;_LL145: _tmp148=_tmp146.f1;goto _LL144;_LL144:
_tmp147=_tmp146.f2;goto _LL142;_LL142: _tmp144=((struct Cyc_Absyn_Using_d_struct*)
_tmp13B)->f2;goto _LL137;}else{goto _LL138;}_LL138: if(*((int*)_tmp13B)== 10){
_LL146: _tmp149=((struct Cyc_Absyn_ExternC_d_struct*)_tmp13B)->f1;goto _LL139;}
else{goto _LL123;}_LL125: goto _LL127;_LL127:({void*_tmp14A[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("top level let-declarations are not implemented",sizeof(
unsigned char),47),_tag_arr(_tmp14A,sizeof(void*),0));});goto _LL123;_LL129: if(
in_externC? Cyc_Tc_okay_externC(d->loc,(void*)_tmp13C->sc): 0){(void*)(_tmp13C->sc=(
void*)((void*)4));}if(_tmp13C->initializer != 0){void*_tmp14B=(void*)((struct Cyc_Absyn_Exp*)
_check_null(_tmp13C->initializer))->r;void*_tmp14C;_LL148: if(*((int*)_tmp14B)== 
19){_LL14C: _tmp14C=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp14B)->f2;goto
_LL149;}else{goto _LL14A;}_LL14A: goto _LL14B;_LL149: {struct Cyc_Absyn_Exp*_tmp14E;
struct Cyc_List_List*_tmp14F;struct Cyc_Dict_Dict*_tmp150;struct _tuple3 _tmp14D=Cyc_Tcgenrep_tcGenrep(
te,ge,loc,_tmp14C,dict);_LL150: _tmp150=_tmp14D.f1;goto _LL14F;_LL14F: _tmp14F=
_tmp14D.f2;goto _LL14E;_LL14E: _tmp14E=_tmp14D.f3;goto _LL14D;_LL14D: dict=_tmp150;
Cyc_Tc_tc_decls(te,_tmp14F,in_externC,check_var_init);_tmp13C->initializer=(
struct Cyc_Absyn_Exp*)_tmp14E;Cyc_Tc_tcVardecl(te,ge,loc,_tmp13C,check_var_init);
if(_tmp14F != 0){if(last != 0){((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp14F,_tmp13A);((struct Cyc_List_List*)
_check_null(last))->tl=_tmp14F;}else{struct Cyc_List_List tmp=({struct Cyc_List_List
_tmp151;_tmp151.hd=(struct Cyc_Absyn_Decl*)_tmp13A->hd;_tmp151.tl=_tmp13A->tl;
_tmp151;});(struct Cyc_Absyn_Decl*)(_tmp13A->hd=(void*)((struct Cyc_Absyn_Decl*)
_tmp14F->hd));_tmp13A->tl=_tmp14F->tl;(struct Cyc_Absyn_Decl*)(_tmp14F->hd=(void*)((
struct Cyc_Absyn_Decl*)tmp.hd));_tmp14F->tl=tmp.tl;((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp13A,
_tmp14F);}}continue;}_LL14B: goto _LL147;_LL147:;}Cyc_Tc_tcVardecl(te,ge,loc,
_tmp13C,check_var_init);goto _LL123;_LL12B: if(in_externC? Cyc_Tc_okay_externC(d->loc,(
void*)_tmp13D->sc): 0){(void*)(_tmp13D->sc=(void*)((void*)4));}Cyc_Tc_tcFndecl(te,
ge,loc,_tmp13D);goto _LL123;_LL12D: Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp13E);goto
_LL123;_LL12F: if(in_externC? Cyc_Tc_okay_externC(d->loc,(void*)_tmp13F->sc): 0){(
void*)(_tmp13F->sc=(void*)((void*)4));}Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp13F);goto
_LL123;_LL131: if(in_externC? Cyc_Tc_okay_externC(d->loc,(void*)_tmp140->sc): 0){(
void*)(_tmp140->sc=(void*)((void*)4));}Cyc_Tc_tcTuniondecl(te,ge,loc,_tmp140);
goto _LL123;_LL133: if(in_externC? Cyc_Tc_okay_externC(d->loc,(void*)_tmp141->sc): 0){(
void*)(_tmp141->sc=(void*)((void*)4));}Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp141);goto
_LL123;_LL135: {struct Cyc_List_List*_tmp152=te->ns;struct Cyc_List_List*_tmp153=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp152,({struct Cyc_List_List*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->hd=
_tmp143;_tmp154->tl=0;_tmp154;}));if(!((int(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_member)(ge->namespaces,_tmp143)){ge->namespaces=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(
ge->namespaces,_tmp143);te->ae=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp153,
Cyc_Tcenv_empty_genv());}te->ns=_tmp153;Cyc_Tc_tc_decls(te,_tmp142,in_externC,
check_var_init);te->ns=_tmp152;goto _LL123;}_LL137: {struct _tagged_arr*first;
struct Cyc_List_List*rest;{void*_tmp155=_tmp148;struct Cyc_List_List*_tmp156;
struct Cyc_List_List*_tmp157;struct Cyc_List_List*_tmp158;struct Cyc_List_List
_tmp159;struct Cyc_List_List*_tmp15A;struct _tagged_arr*_tmp15B;struct Cyc_List_List*
_tmp15C;struct Cyc_List_List _tmp15D;struct Cyc_List_List*_tmp15E;struct _tagged_arr*
_tmp15F;_LL152: if((int)_tmp155 == 0){goto _LL153;}else{goto _LL154;}_LL154: if((
unsigned int)_tmp155 > 1?*((int*)_tmp155)== 0: 0){_LL15C: _tmp156=((struct Cyc_Absyn_Rel_n_struct*)
_tmp155)->f1;if(_tmp156 == 0){goto _LL155;}else{goto _LL156;}}else{goto _LL156;}
_LL156: if((unsigned int)_tmp155 > 1?*((int*)_tmp155)== 1: 0){_LL15D: _tmp157=((
struct Cyc_Absyn_Abs_n_struct*)_tmp155)->f1;if(_tmp157 == 0){goto _LL157;}else{goto
_LL158;}}else{goto _LL158;}_LL158: if((unsigned int)_tmp155 > 1?*((int*)_tmp155)== 
0: 0){_LL15E: _tmp158=((struct Cyc_Absyn_Rel_n_struct*)_tmp155)->f1;if(_tmp158 == 0){
goto _LL15A;}else{_tmp159=*_tmp158;_LL160: _tmp15B=(struct _tagged_arr*)_tmp159.hd;
goto _LL15F;_LL15F: _tmp15A=_tmp159.tl;goto _LL159;}}else{goto _LL15A;}_LL15A: if((
unsigned int)_tmp155 > 1?*((int*)_tmp155)== 1: 0){_LL161: _tmp15C=((struct Cyc_Absyn_Abs_n_struct*)
_tmp155)->f1;if(_tmp15C == 0){goto _LL151;}else{_tmp15D=*_tmp15C;_LL163: _tmp15F=(
struct _tagged_arr*)_tmp15D.hd;goto _LL162;_LL162: _tmp15E=_tmp15D.tl;goto _LL15B;}}
else{goto _LL151;}_LL153: goto _LL155;_LL155: goto _LL157;_LL157: first=_tmp147;rest=0;
goto _LL151;_LL159: _tmp15F=_tmp15B;_tmp15E=_tmp15A;goto _LL15B;_LL15B: first=
_tmp15F;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp15E,({struct Cyc_List_List*_tmp160=_cycalloc(sizeof(*
_tmp160));_tmp160->hd=_tmp147;_tmp160->tl=0;_tmp160;}));goto _LL151;_LL151:;}{
struct Cyc_List_List*_tmp161=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);ge->availables=({
struct Cyc_List_List*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->hd=_tmp161;
_tmp162->tl=ge->availables;_tmp162;});Cyc_Tc_tc_decls(te,_tmp144,in_externC,
check_var_init);ge->availables=((struct Cyc_List_List*)_check_null(ge->availables))->tl;
goto _LL123;}}_LL139: Cyc_Tc_tc_decls(te,_tmp149,1,check_var_init);goto _LL123;
_LL123:;}}void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*
ds){Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);Cyc_Tc_tc_decls(te,ds,0,
check_var_init);}static int Cyc_Tc_vardecl_needed(struct Cyc_Dict_Dict*env,struct
Cyc_Absyn_Decl*d);static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Dict_Dict*
env,struct Cyc_List_List*ds){return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Dict_Dict*,
struct Cyc_Absyn_Decl*),struct Cyc_Dict_Dict*env,struct Cyc_List_List*x))Cyc_List_filter_c)(
Cyc_Tc_vardecl_needed,env,ds);}struct _tuple7{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*
f2;};static int Cyc_Tc_vardecl_needed(struct Cyc_Dict_Dict*env,struct Cyc_Absyn_Decl*
d){void*_tmp163=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp164;struct Cyc_List_List*
_tmp165;struct Cyc_List_List**_tmp166;struct Cyc_List_List*_tmp167;struct Cyc_List_List**
_tmp168;struct Cyc_List_List*_tmp169;struct Cyc_List_List**_tmp16A;_LL165: if(*((
int*)_tmp163)== 0){_LL16F: _tmp164=((struct Cyc_Absyn_Var_d_struct*)_tmp163)->f1;
goto _LL166;}else{goto _LL167;}_LL167: if(*((int*)_tmp163)== 10){_LL170: _tmp165=((
struct Cyc_Absyn_ExternC_d_struct*)_tmp163)->f1;_tmp166=(struct Cyc_List_List**)&((
struct Cyc_Absyn_ExternC_d_struct*)_tmp163)->f1;goto _LL168;}else{goto _LL169;}
_LL169: if(*((int*)_tmp163)== 9){_LL171: _tmp167=((struct Cyc_Absyn_Using_d_struct*)
_tmp163)->f2;_tmp168=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_struct*)
_tmp163)->f2;goto _LL16A;}else{goto _LL16B;}_LL16B: if(*((int*)_tmp163)== 8){_LL172:
_tmp169=((struct Cyc_Absyn_Namespace_d_struct*)_tmp163)->f2;_tmp16A=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Namespace_d_struct*)_tmp163)->f2;goto _LL16C;}else{goto _LL16D;}
_LL16D: goto _LL16E;_LL166: if((void*)_tmp164->sc != (void*)3?(void*)_tmp164->sc != (
void*)4: 0){return 1;}{struct _tuple0 _tmp16C;struct _tagged_arr*_tmp16D;void*_tmp16E;
struct _tuple0*_tmp16B=_tmp164->name;_tmp16C=*_tmp16B;_LL175: _tmp16E=_tmp16C.f1;
goto _LL174;_LL174: _tmp16D=_tmp16C.f2;goto _LL173;_LL173: {struct Cyc_List_List*ns;{
void*_tmp16F=_tmp16E;struct Cyc_List_List*_tmp170;struct Cyc_List_List*_tmp171;
_LL177: if((int)_tmp16F == 0){goto _LL178;}else{goto _LL179;}_LL179: if((unsigned int)
_tmp16F > 1?*((int*)_tmp16F)== 0: 0){_LL17D: _tmp170=((struct Cyc_Absyn_Rel_n_struct*)
_tmp16F)->f1;goto _LL17A;}else{goto _LL17B;}_LL17B: if((unsigned int)_tmp16F > 1?*((
int*)_tmp16F)== 1: 0){_LL17E: _tmp171=((struct Cyc_Absyn_Abs_n_struct*)_tmp16F)->f1;
goto _LL17C;}else{goto _LL176;}_LL178: ns=0;goto _LL176;_LL17A: ns=_tmp170;goto _LL176;
_LL17C: ns=_tmp171;goto _LL176;_LL176:;}{struct _tuple7*_tmp172=((struct _tuple7*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env,ns);struct Cyc_Tcenv_Genv*
_tmp173=(*_tmp172).f1;int _tmp174=(*((struct _tuple4*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp173->ordinaries,_tmp16D)).f2;if(!
_tmp174){(*_tmp172).f2=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_insert)((*_tmp172).f2,_tmp16D);}return _tmp174;}}}_LL168:
_tmp168=_tmp166;goto _LL16A;_LL16A: _tmp16A=_tmp168;goto _LL16C;_LL16C:*_tmp16A=Cyc_Tc_treeshake_f(
env,*_tmp16A);return 1;_LL16E: return 1;_LL164:;}static int Cyc_Tc_treeshake_remove_f(
struct Cyc_Set_Set*set,struct _tagged_arr*x,void*y){return !((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(set,x);}static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List*ns,struct _tuple7*nsenv){struct _tuple7 _tmp176;struct Cyc_Set_Set*
_tmp177;struct Cyc_Tcenv_Genv*_tmp178;struct _tuple7*_tmp175=nsenv;_tmp176=*
_tmp175;_LL181: _tmp178=_tmp176.f1;goto _LL180;_LL180: _tmp177=_tmp176.f2;goto
_LL17F;_LL17F: _tmp178->ordinaries=((struct Cyc_Dict_Dict*(*)(int(*f)(struct Cyc_Set_Set*,
struct _tagged_arr*,struct _tuple4*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict*d))
Cyc_Dict_filter_c)((int(*)(struct Cyc_Set_Set*set,struct _tagged_arr*x,struct
_tuple4*y))Cyc_Tc_treeshake_remove_f,_tmp177,_tmp178->ordinaries);}static struct
_tuple7*Cyc_Tc_treeshake_make_env_f(struct Cyc_Tcenv_Genv*ge){return({struct
_tuple7*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->f1=ge;_tmp179->f2=((struct
Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);_tmp179;});}struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*ds){struct Cyc_Dict_Dict*_tmp17A=((struct Cyc_Dict_Dict*(*)(
struct _tuple7*(*f)(struct Cyc_Tcenv_Genv*),struct Cyc_Dict_Dict*d))Cyc_Dict_map)(
Cyc_Tc_treeshake_make_env_f,te->ae);struct Cyc_List_List*_tmp17B=Cyc_Tc_treeshake_f(
_tmp17A,ds);((void(*)(void(*f)(struct Cyc_List_List*,struct _tuple7*),struct Cyc_Dict_Dict*
d))Cyc_Dict_iter)(Cyc_Tc_treeshake_remove,_tmp17A);return _tmp17B;}
