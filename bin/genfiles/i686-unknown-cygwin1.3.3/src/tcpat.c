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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};struct
_tuple0{void*f1;void*f2;};extern void*Cyc_Core_snd(struct _tuple0*);extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern int Cyc_List_length(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct
Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern struct
Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];extern struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);extern struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*
env);struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern
unsigned char Cyc_Dict_Absent[11];struct Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_empty(
int(*cmp)(void*,void*));extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*
s,void*elt);extern int Cyc_Set_cardinality(struct Cyc_Set_Set*s);extern int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern unsigned char Cyc_Set_Absent[11];extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);extern int Cyc_Std_zstrcmp(struct
_tagged_arr,struct _tagged_arr);extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,
struct _tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
_tuple1*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*
tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
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
_tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};
struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Core_Opt*f3;};struct Cyc_Absyn_AccessEff_struct{
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
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Primop_e_struct{
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
int tag;struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Aggrdecl*f3;};
struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Codegen_e_struct{int tag;struct
Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Fill_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct _tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;struct _tuple3 condition;
struct _tuple3 delta;struct Cyc_Absyn_Stmt*body;};struct Cyc_Absyn_Exp_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple3
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct
Cyc_Absyn_Goto_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple3 f2;
struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_SwitchC_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Cut_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Splice_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct _tuple3 f2;};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*
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
struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple1*f1;
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
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref(void*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
extern void*Cyc_Absyn_compress_kb(void*);extern void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_uchar_t;extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_sint_t;
extern void*Cyc_Absyn_float_typ;extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Cstdio___abstractFILE;
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
Cyc_List_List*curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*);extern struct _tagged_arr Cyc_Absynpp_kind2string(void*);extern struct
_tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Tcenv_VarRes_struct{
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
ae;struct Cyc_Core_Opt*le;};extern void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);extern struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);extern struct
Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_List_List*);extern void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);extern unsigned char Cyc_Tcutil_TypeErr[12];
extern void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap);extern void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct
_tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern int Cyc_Tcutil_coerceable(void*);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern int Cyc_Tcutil_unify(void*,void*);extern void*Cyc_Tcutil_rsubstitute(struct
_RegionHandle*,struct Cyc_List_List*,void*);struct _tuple4{struct Cyc_List_List*f1;
struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};extern
struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr err_msg);extern struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};extern struct _tuple6 Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p,void**topt,void**region_opt);extern void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);extern void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);extern int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);extern void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct _tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmp0=(void*)p->r;struct _tuple1*_tmp1;struct Cyc_List_List*_tmp2;struct
Cyc_List_List*_tmp3;struct _tuple1*_tmp4;struct Cyc_List_List*_tmp5;struct Cyc_List_List*
_tmp6;struct Cyc_Absyn_AggrInfo _tmp7;struct Cyc_List_List*_tmp8;void*_tmp9;struct
_tuple1*_tmpA;void*_tmpB;_LL1: if((unsigned int)_tmp0 > 2?*((int*)_tmp0)== 11: 0){
_LL9: _tmp1=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;goto _LL2;}else{goto
_LL3;}_LL3: if((unsigned int)_tmp0 > 2?*((int*)_tmp0)== 12: 0){_LLC: _tmp4=((struct
Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f1;goto _LLB;_LLB: _tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f2;goto _LLA;_LLA: _tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;
goto _LL4;}else{goto _LL5;}_LL5: if((unsigned int)_tmp0 > 2?*((int*)_tmp0)== 7: 0){
_LLF: _tmp7=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f1;_LL11: _tmp9=(void*)_tmp7.aggr_info;
if(*((int*)_tmp9)== 0){_LL13: _tmpB=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp9)->f1;goto _LL12;_LL12: _tmpA=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp9)->f2;
goto _LL10;}else{goto _LL7;}_LL10: _tmp8=_tmp7.targs;goto _LLE;_LLE: _tmp6=((struct
Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;goto _LLD;_LLD: _tmp5=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f3;goto _LL6;}else{goto _LL7;}_LL7: goto _LL8;_LL2:{struct _handler_cons _tmpC;
_push_handler(& _tmpC);{int _tmpE=0;if(setjmp(_tmpC.handler)){_tmpE=1;}if(! _tmpE){{
void*_tmpF=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp1);struct Cyc_Absyn_Tunionfield*
_tmp10;struct Cyc_Absyn_Tuniondecl*_tmp11;struct Cyc_Absyn_Enumfield*_tmp12;struct
Cyc_Absyn_Enumdecl*_tmp13;struct Cyc_Absyn_Enumfield*_tmp14;void*_tmp15;_LL15: if(*((
int*)_tmpF)== 1){goto _LL16;}else{goto _LL17;}_LL17: if(*((int*)_tmpF)== 2){_LL20:
_tmp11=((struct Cyc_Tcenv_TunionRes_struct*)_tmpF)->f1;goto _LL1F;_LL1F: _tmp10=((
struct Cyc_Tcenv_TunionRes_struct*)_tmpF)->f2;goto _LL18;}else{goto _LL19;}_LL19:
if(*((int*)_tmpF)== 3){_LL22: _tmp13=((struct Cyc_Tcenv_EnumRes_struct*)_tmpF)->f1;
goto _LL21;_LL21: _tmp12=((struct Cyc_Tcenv_EnumRes_struct*)_tmpF)->f2;goto _LL1A;}
else{goto _LL1B;}_LL1B: if(*((int*)_tmpF)== 4){_LL24: _tmp15=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmpF)->f1;goto _LL23;_LL23: _tmp14=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmpF)->f2;
goto _LL1C;}else{goto _LL1D;}_LL1D: if(*((int*)_tmpF)== 0){goto _LL1E;}else{goto
_LL14;}_LL16:({void*_tmp16[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("struct tag used without arguments in pattern",
sizeof(unsigned char),45),_tag_arr(_tmp16,sizeof(void*),0));});_npop_handler(0);
return;_LL18:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp18;_tmp18.tag=8;_tmp18.f1=_tmp11;_tmp18.f2=_tmp10;_tmp18.f3=0;_tmp18.f4=0;
_tmp18;});_tmp17;})));_npop_handler(0);return;_LL1A:(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Enum_p_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19[0]=({
struct Cyc_Absyn_Enum_p_struct _tmp1A;_tmp1A.tag=9;_tmp1A.f1=_tmp13;_tmp1A.f2=
_tmp12;_tmp1A;});_tmp19;})));_npop_handler(0);return;_LL1C:(void*)(p->r=(void*)((
void*)({struct Cyc_Absyn_AnonEnum_p_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));
_tmp1B[0]=({struct Cyc_Absyn_AnonEnum_p_struct _tmp1C;_tmp1C.tag=10;_tmp1C.f1=(
void*)_tmp15;_tmp1C.f2=_tmp14;_tmp1C;});_tmp1B;})));_npop_handler(0);return;
_LL1E: goto _LL14;_LL14:;};_pop_handler();}else{void*_tmpD=(void*)_exn_thrown;void*
_tmp1E=_tmpD;_LL26: if(_tmp1E == Cyc_Dict_Absent){goto _LL27;}else{goto _LL28;}_LL28:
goto _LL29;_LL27: goto _LL25;_LL29:(void)_throw(_tmp1E);_LL25:;}}}{void*_tmp1F=(*
_tmp1).f1;struct Cyc_List_List*_tmp20;_LL2B: if((int)_tmp1F == 0){goto _LL2C;}else{
goto _LL2D;}_LL2D: if((unsigned int)_tmp1F > 1?*((int*)_tmp1F)== 0: 0){_LL31: _tmp20=((
struct Cyc_Absyn_Rel_n_struct*)_tmp1F)->f1;if(_tmp20 == 0){goto _LL2E;}else{goto
_LL2F;}}else{goto _LL2F;}_LL2F: goto _LL30;_LL2C: goto _LL2E;_LL2E:(*_tmp1).f1=(void*)
0;(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp21=_cycalloc(
sizeof(*_tmp21));_tmp21[0]=({struct Cyc_Absyn_Var_p_struct _tmp22;_tmp22.tag=0;
_tmp22.f1=Cyc_Absyn_new_vardecl(_tmp1,(void*)0,0);_tmp22;});_tmp21;})));return;
_LL30:({void*_tmp23[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("qualified variable in pattern",
sizeof(unsigned char),30),_tag_arr(_tmp23,sizeof(void*),0));});return;_LL2A:;}
_LL4:{struct _handler_cons _tmp24;_push_handler(& _tmp24);{int _tmp26=0;if(setjmp(
_tmp24.handler)){_tmp26=1;}if(! _tmp26){{void*_tmp27=Cyc_Tcenv_lookup_ordinary(te,
p->loc,_tmp4);struct Cyc_Absyn_Aggrdecl*_tmp28;struct Cyc_Absyn_Tunionfield*_tmp29;
struct Cyc_Absyn_Tuniondecl*_tmp2A;_LL33: if(*((int*)_tmp27)== 1){_LL3D: _tmp28=((
struct Cyc_Tcenv_AggrRes_struct*)_tmp27)->f1;goto _LL34;}else{goto _LL35;}_LL35: if(*((
int*)_tmp27)== 2){_LL3F: _tmp2A=((struct Cyc_Tcenv_TunionRes_struct*)_tmp27)->f1;
goto _LL3E;_LL3E: _tmp29=((struct Cyc_Tcenv_TunionRes_struct*)_tmp27)->f2;goto _LL36;}
else{goto _LL37;}_LL37: if(*((int*)_tmp27)== 3){goto _LL38;}else{goto _LL39;}_LL39:
if(*((int*)_tmp27)== 4){goto _LL3A;}else{goto _LL3B;}_LL3B: if(*((int*)_tmp27)== 0){
goto _LL3C;}else{goto _LL32;}_LL34: if((void*)_tmp28->kind == (void*)1){({void*
_tmp2B[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("cannot pattern-match a union",
sizeof(unsigned char),29),_tag_arr(_tmp2B,sizeof(void*),0));});_npop_handler(0);
return;}{struct Cyc_List_List*_tmp2C=0;for(0;_tmp2 != 0;_tmp2=_tmp2->tl){_tmp2C=({
struct Cyc_List_List*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->hd=({struct _tuple7*
_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->f1=0;_tmp2E->f2=(struct Cyc_Absyn_Pat*)
_tmp2->hd;_tmp2E;});_tmp2D->tl=_tmp2C;_tmp2D;});}(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Aggr_p_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=({
struct Cyc_Absyn_Aggr_p_struct _tmp30;_tmp30.tag=7;_tmp30.f1=({struct Cyc_Absyn_AggrInfo
_tmp31;_tmp31.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp32=
_cycalloc(sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp33;
_tmp33.tag=1;_tmp33.f1=({struct Cyc_Absyn_Aggrdecl**_tmp34=_cycalloc(sizeof(*
_tmp34));_tmp34[0]=_tmp28;_tmp34;});_tmp33;});_tmp32;}));_tmp31.targs=0;_tmp31;});
_tmp30.f2=0;_tmp30.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp2C);_tmp30;});_tmp2F;})));_npop_handler(0);return;}_LL36:(void*)(p->r=(void*)((
void*)({struct Cyc_Absyn_Tunion_p_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[
0]=({struct Cyc_Absyn_Tunion_p_struct _tmp36;_tmp36.tag=8;_tmp36.f1=_tmp2A;_tmp36.f2=
_tmp29;_tmp36.f3=_tmp3;_tmp36.f4=_tmp2;_tmp36;});_tmp35;})));_npop_handler(0);
return;_LL38: goto _LL3A;_LL3A:({void*_tmp37[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("enum tag used with arguments in pattern",
sizeof(unsigned char),40),_tag_arr(_tmp37,sizeof(void*),0));});_npop_handler(0);
return;_LL3C: goto _LL32;_LL32:;};_pop_handler();}else{void*_tmp25=(void*)
_exn_thrown;void*_tmp39=_tmp25;_LL41: if(_tmp39 == Cyc_Dict_Absent){goto _LL42;}
else{goto _LL43;}_LL43: goto _LL44;_LL42: goto _LL40;_LL44:(void)_throw(_tmp39);_LL40:;}}}({
struct Cyc_Std_String_pa_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp4);{void*_tmp3A[1]={& _tmp3B};Cyc_Tcutil_terr(p->loc,
_tag_arr("%s is not a constructor in pattern",sizeof(unsigned char),35),_tag_arr(
_tmp3A,sizeof(void*),1));}});return;_LL6:{struct _handler_cons _tmp3C;
_push_handler(& _tmp3C);{int _tmp3E=0;if(setjmp(_tmp3C.handler)){_tmp3E=1;}if(!
_tmp3E){{struct Cyc_Absyn_Aggrdecl**_tmp3F=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,
_tmpA);struct Cyc_Absyn_Aggrdecl*_tmp40=*_tmp3F;if((void*)_tmp40->kind == (void*)1){({
void*_tmp41[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("cannot pattern-match a union",
sizeof(unsigned char),29),_tag_arr(_tmp41,sizeof(void*),0));});_npop_handler(0);
return;}if(_tmp40->fields == 0? 1: _tmp40->exist_vars == 0){({void*_tmp42[0]={};Cyc_Tcutil_terr(
p->loc,_tag_arr("can't destructure an abstract struct",sizeof(unsigned char),37),
_tag_arr(_tmp42,sizeof(void*),0));});_npop_handler(0);return;}{int more_exists=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp40->exist_vars))->v)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp6);if(more_exists != 0){({void*_tmp43[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("wrong number of existentially bound type variables",
sizeof(unsigned char),51),_tag_arr(_tmp43,sizeof(void*),0));});}(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp44=_cycalloc(sizeof(*_tmp44));
_tmp44[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp45;_tmp45.tag=7;_tmp45.f1=({struct
Cyc_Absyn_AggrInfo _tmp46;_tmp46.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmp48;_tmp48.tag=1;_tmp48.f1=_tmp3F;_tmp48;});_tmp47;}));_tmp46.targs=_tmp8;
_tmp46;});_tmp45.f2=_tmp6;_tmp45.f3=_tmp5;_tmp45;});_tmp44;})));}};_pop_handler();}
else{void*_tmp3D=(void*)_exn_thrown;void*_tmp4A=_tmp3D;_LL46: if(_tmp4A == Cyc_Dict_Absent){
goto _LL47;}else{goto _LL48;}_LL48: goto _LL49;_LL47:({void*_tmp4B[0]={};Cyc_Tcutil_terr(
p->loc,_tag_arr("Non-struct name has designator patterns",sizeof(unsigned char),
40),_tag_arr(_tmp4B,sizeof(void*),0));});goto _LL45;_LL49:(void)_throw(_tmp4A);
_LL45:;}}}return;_LL8: return;_LL0:;}static struct _tagged_arr*Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(
struct Cyc_List_List*s,void**topt){if(topt != 0){return*topt;}return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp4C=_cycalloc(sizeof(*
_tmp4C));_tmp4C->v=s;_tmp4C;}));}static void*Cyc_Tcpat_num_type(void**topt,void*
numt){if(topt != 0? Cyc_Tcutil_coerceable(*topt): 0){return*topt;}{void*_tmp4D=Cyc_Tcutil_compress(
numt);_LL4B: if((unsigned int)_tmp4D > 3?*((int*)_tmp4D)== 12: 0){goto _LL4C;}else{
goto _LL4D;}_LL4D: goto _LL4E;_LL4C: if(topt != 0){return*topt;}goto _LL4A;_LL4E: goto
_LL4A;_LL4A:;}return numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,
struct Cyc_List_List**v_result_ptr,void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual();*
v_result_ptr=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp4E=_cycalloc(
sizeof(*_tmp4E));_tmp4E->hd=vd;_tmp4E->tl=*v_result_ptr;_tmp4E;});}struct _tuple8{
struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple9{struct Cyc_Absyn_Aggrfield*f1;
struct Cyc_Absyn_Pat*f2;};struct _tuple6 Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt,void**region_opt){Cyc_Tcpat_resolve_pat(te,p);{
void*t;struct Cyc_List_List*tv_result=0;struct Cyc_List_List*v_result=0;{void*
_tmp4F=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp50;struct Cyc_Absyn_Vardecl*_tmp51;
void*_tmp52;void*_tmp53;struct Cyc_Absyn_Enumdecl*_tmp54;void*_tmp55;struct Cyc_Absyn_Pat*
_tmp56;struct Cyc_List_List*_tmp57;struct Cyc_List_List*_tmp58;struct Cyc_List_List*
_tmp59;struct Cyc_Absyn_AggrInfo _tmp5A;struct Cyc_List_List*_tmp5B;struct Cyc_List_List**
_tmp5C;void*_tmp5D;struct Cyc_Absyn_Aggrdecl**_tmp5E;struct Cyc_Absyn_Aggrdecl*
_tmp5F;struct Cyc_List_List*_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_Absyn_Tunionfield*
_tmp62;struct Cyc_Absyn_Tuniondecl*_tmp63;struct Cyc_Absyn_AggrInfo _tmp64;void*
_tmp65;_LL50: if((int)_tmp4F == 0){goto _LL51;}else{goto _LL52;}_LL52: if((
unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 0: 0){_LL72: _tmp50=((struct Cyc_Absyn_Var_p_struct*)
_tmp4F)->f1;goto _LL53;}else{goto _LL54;}_LL54: if((unsigned int)_tmp4F > 2?*((int*)
_tmp4F)== 6: 0){_LL73: _tmp51=((struct Cyc_Absyn_Reference_p_struct*)_tmp4F)->f1;
goto _LL55;}else{goto _LL56;}_LL56: if((unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 1:
0){_LL74: _tmp52=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp4F)->f1;if((int)
_tmp52 == 1){goto _LL57;}else{goto _LL58;}}else{goto _LL58;}_LL58: if((unsigned int)
_tmp4F > 2?*((int*)_tmp4F)== 1: 0){_LL75: _tmp53=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp4F)->f1;if((int)_tmp53 == 0){goto _LL59;}else{goto _LL5A;}}else{goto _LL5A;}
_LL5A: if((unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 2: 0){goto _LL5B;}else{goto
_LL5C;}_LL5C: if((unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 3: 0){goto _LL5D;}else{
goto _LL5E;}_LL5E: if((unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 9: 0){_LL76: _tmp54=((
struct Cyc_Absyn_Enum_p_struct*)_tmp4F)->f1;goto _LL5F;}else{goto _LL60;}_LL60: if((
unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 10: 0){_LL77: _tmp55=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp4F)->f1;goto _LL61;}else{goto _LL62;}_LL62: if((int)_tmp4F == 1){goto _LL63;}
else{goto _LL64;}_LL64: if((unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 5: 0){_LL78:
_tmp56=((struct Cyc_Absyn_Pointer_p_struct*)_tmp4F)->f1;goto _LL65;}else{goto _LL66;}
_LL66: if((unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 4: 0){_LL79: _tmp57=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp4F)->f1;goto _LL67;}else{goto _LL68;}_LL68: if((unsigned int)_tmp4F > 2?*((int*)
_tmp4F)== 7: 0){_LL7C: _tmp5A=((struct Cyc_Absyn_Aggr_p_struct*)_tmp4F)->f1;_LL7E:
_tmp5D=(void*)_tmp5A.aggr_info;if(*((int*)_tmp5D)== 1){_LL7F: _tmp5E=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp5D)->f1;_tmp5F=*_tmp5E;goto _LL7D;}else{goto _LL6A;}_LL7D: _tmp5B=_tmp5A.targs;
_tmp5C=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)_tmp4F)->f1).targs;
goto _LL7B;_LL7B: _tmp59=((struct Cyc_Absyn_Aggr_p_struct*)_tmp4F)->f2;goto _LL7A;
_LL7A: _tmp58=((struct Cyc_Absyn_Aggr_p_struct*)_tmp4F)->f3;goto _LL69;}else{goto
_LL6A;}_LL6A: if((unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 8: 0){_LL83: _tmp63=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp4F)->f1;goto _LL82;_LL82: _tmp62=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4F)->f2;goto _LL81;_LL81: _tmp61=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4F)->f3;
goto _LL80;_LL80: _tmp60=((struct Cyc_Absyn_Tunion_p_struct*)_tmp4F)->f4;goto _LL6B;}
else{goto _LL6C;}_LL6C: if((unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 7: 0){_LL84:
_tmp64=((struct Cyc_Absyn_Aggr_p_struct*)_tmp4F)->f1;_LL85: _tmp65=(void*)_tmp64.aggr_info;
if(*((int*)_tmp65)== 0){goto _LL6D;}else{goto _LL6E;}}else{goto _LL6E;}_LL6E: if((
unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 11: 0){goto _LL6F;}else{goto _LL70;}_LL70:
if((unsigned int)_tmp4F > 2?*((int*)_tmp4F)== 12: 0){goto _LL71;}else{goto _LL4F;}
_LL51: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);goto _LL4F;_LL53:
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);Cyc_Tcpat_set_vd(
_tmp50,& v_result,t);goto _LL4F;_LL55: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);if(region_opt == 0){({void*_tmp66[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("* pattern would point into an unknown/unallowed region",
sizeof(unsigned char),55),_tag_arr(_tmp66,sizeof(void*),0));});goto _LL4F;}Cyc_Tcpat_set_vd(
_tmp51,& v_result,(void*)({struct Cyc_Absyn_PointerType_struct*_tmp67=_cycalloc(
sizeof(*_tmp67));_tmp67[0]=({struct Cyc_Absyn_PointerType_struct _tmp68;_tmp68.tag=
4;_tmp68.f1=({struct Cyc_Absyn_PtrInfo _tmp69;_tmp69.elt_typ=(void*)t;_tmp69.rgn_typ=(
void*)*region_opt;_tmp69.nullable=((struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(
0);_tmp69.tq=Cyc_Absyn_empty_tqual();_tmp69.bounds=Cyc_Absyn_empty_conref();
_tmp69;});_tmp68;});_tmp67;}));goto _LL4F;_LL57: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_t);
goto _LL4F;_LL59: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_t);goto _LL4F;_LL5B: t=
Cyc_Tcpat_num_type(topt,Cyc_Absyn_uchar_t);goto _LL4F;_LL5D: t=Cyc_Tcpat_num_type(
topt,Cyc_Absyn_float_typ);goto _LL4F;_LL5F: t=Cyc_Tcpat_num_type(topt,(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=({
struct Cyc_Absyn_EnumType_struct _tmp6B;_tmp6B.tag=12;_tmp6B.f1=_tmp54->name;
_tmp6B.f2=(struct Cyc_Absyn_Enumdecl*)_tmp54;_tmp6B;});_tmp6A;}));goto _LL4F;_LL61:
t=Cyc_Tcpat_num_type(topt,_tmp55);goto _LL4F;_LL63: if(topt != 0){void*_tmp6C=Cyc_Tcutil_compress(*
topt);_LL87: if((unsigned int)_tmp6C > 3?*((int*)_tmp6C)== 4: 0){goto _LL88;}else{
goto _LL89;}_LL89: goto _LL8A;_LL88: t=*topt;goto tcpat_end;_LL8A: goto _LL86;_LL86:;}{
struct Cyc_List_List*_tmp6D=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=({struct Cyc_Absyn_PointerType_struct
_tmp6F;_tmp6F.tag=4;_tmp6F.f1=({struct Cyc_Absyn_PtrInfo _tmp70;_tmp70.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->v=_tmp6D;_tmp72;}));_tmp70.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->v=_tmp6D;_tmp71;}));_tmp70.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp70.tq=Cyc_Absyn_empty_tqual();
_tmp70.bounds=Cyc_Absyn_empty_conref();_tmp70;});_tmp6F;});_tmp6E;});goto _LL4F;}
_LL65: {void*inner_typ=(void*)0;void**_tmp73=0;if(topt != 0){void*_tmp74=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp75;void*_tmp76;_LL8C: if((unsigned int)_tmp74 > 3?*((
int*)_tmp74)== 4: 0){_LL90: _tmp75=((struct Cyc_Absyn_PointerType_struct*)_tmp74)->f1;
_LL91: _tmp76=(void*)_tmp75.elt_typ;goto _LL8D;}else{goto _LL8E;}_LL8E: goto _LL8F;
_LL8D: inner_typ=_tmp76;_tmp73=(void**)& inner_typ;goto _LL8B;_LL8F: goto _LL8B;_LL8B:;}{
void*ptr_rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp8C;}));struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp79;struct _tuple6
_tmp77=Cyc_Tcpat_tcPatRec(te,_tmp56,_tmp73,(void**)& ptr_rgn);_LL94: _tmp79=_tmp77.f1;
goto _LL93;_LL93: _tmp78=_tmp77.f2;goto _LL92;_LL92: tv_result=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tv_result,_tmp79);
v_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(v_result,_tmp78);if(_tmp73 == 0){void*_tmp7A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp56->topt))->v);struct Cyc_Absyn_TunionFieldInfo
_tmp7B;struct Cyc_List_List*_tmp7C;void*_tmp7D;struct Cyc_Absyn_Tunionfield*_tmp7E;
struct Cyc_Absyn_Tuniondecl*_tmp7F;_LL96: if((unsigned int)_tmp7A > 3?*((int*)
_tmp7A)== 3: 0){_LL9A: _tmp7B=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp7A)->f1;
_LL9C: _tmp7D=(void*)_tmp7B.field_info;if(*((int*)_tmp7D)== 1){_LL9E: _tmp7F=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7D)->f1;goto _LL9D;_LL9D: _tmp7E=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7D)->f2;goto _LL9B;}else{goto _LL98;}
_LL9B: _tmp7C=_tmp7B.targs;goto _LL97;}else{goto _LL98;}_LL98: goto _LL99;_LL97: t=(
void*)({struct Cyc_Absyn_TunionType_struct*_tmp80=_cycalloc(sizeof(*_tmp80));
_tmp80[0]=({struct Cyc_Absyn_TunionType_struct _tmp81;_tmp81.tag=2;_tmp81.f1=({
struct Cyc_Absyn_TunionInfo _tmp82;_tmp82.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp84;_tmp84.tag=1;_tmp84.f1=({struct Cyc_Absyn_Tuniondecl**_tmp85=_cycalloc(
sizeof(*_tmp85));_tmp85[0]=_tmp7F;_tmp85;});_tmp84;});_tmp83;}));_tmp82.targs=
_tmp7C;_tmp82.rgn=(void*)ptr_rgn;_tmp82;});_tmp81;});_tmp80;});goto _LL95;_LL99: t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp86=_cycalloc(sizeof(*_tmp86));
_tmp86[0]=({struct Cyc_Absyn_PointerType_struct _tmp87;_tmp87.tag=4;_tmp87.f1=({
struct Cyc_Absyn_PtrInfo _tmp88;_tmp88.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp56->topt))->v);_tmp88.rgn_typ=(void*)ptr_rgn;_tmp88.nullable=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp88.tq=Cyc_Absyn_empty_tqual();
_tmp88.bounds=Cyc_Absyn_empty_conref();_tmp88;});_tmp87;});_tmp86;});goto _LL95;
_LL95:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp89=_cycalloc(
sizeof(*_tmp89));_tmp89[0]=({struct Cyc_Absyn_PointerType_struct _tmp8A;_tmp8A.tag=
4;_tmp8A.f1=({struct Cyc_Absyn_PtrInfo _tmp8B;_tmp8B.elt_typ=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp56->topt))->v);_tmp8B.rgn_typ=(void*)ptr_rgn;
_tmp8B.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp8B.tq=
Cyc_Absyn_empty_tqual();_tmp8B.bounds=Cyc_Absyn_empty_conref();_tmp8B;});_tmp8A;});
_tmp89;});}goto _LL4F;}}_LL67: {struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*
topt_ts=0;if(topt != 0){void*_tmp8D=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*
_tmp8E;_LLA0: if((unsigned int)_tmp8D > 3?*((int*)_tmp8D)== 9: 0){_LLA4: _tmp8E=((
struct Cyc_Absyn_TupleType_struct*)_tmp8D)->f1;goto _LLA1;}else{goto _LLA2;}_LLA2:
goto _LLA3;_LLA1: topt_ts=_tmp8E;goto _LL9F;_LLA3: goto _LL9F;_LL9F:;}for(0;_tmp57 != 
0;_tmp57=_tmp57->tl){void**_tmp8F=0;if(topt_ts != 0){_tmp8F=(void**)&(*((struct
_tuple8*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}{struct Cyc_List_List*_tmp91;
struct Cyc_List_List*_tmp92;struct _tuple6 _tmp90=Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)
_tmp57->hd,_tmp8F,region_opt);_LLA7: _tmp92=_tmp90.f1;goto _LLA6;_LLA6: _tmp91=
_tmp90.f2;goto _LLA5;_LLA5: tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(tv_result,_tmp92);v_result=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(v_result,_tmp91);
pat_ts=({struct Cyc_List_List*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->hd=({
struct _tuple8*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->f1=Cyc_Absyn_empty_tqual();
_tmp94->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)
_tmp57->hd)->topt))->v;_tmp94;});_tmp93->tl=pat_ts;_tmp93;});}}t=(void*)({struct
Cyc_Absyn_TupleType_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct
Cyc_Absyn_TupleType_struct _tmp96;_tmp96.tag=9;_tmp96.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmp96;});_tmp95;});goto _LL4F;}
_LL69: if(_tmp5F->fields == 0? 1: _tmp5F->exist_vars == 0){({void*_tmp97[0]={};Cyc_Tcutil_terr(
p->loc,_tag_arr("can't destructure an abstract struct",sizeof(unsigned char),37),
_tag_arr(_tmp97,sizeof(void*),0));});t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp98;}));goto _LL4F;}{struct _RegionHandle _tmp99=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp99;_push_region(rgn);{struct Cyc_List_List*_tmp9A=0;{
struct Cyc_List_List*t=_tmp59;for(0;t != 0;t=t->tl){struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)t->hd;{void*_tmp9B=Cyc_Absyn_compress_kb((void*)tv->kind);
void*_tmp9C;struct Cyc_Core_Opt*_tmp9D;struct Cyc_Core_Opt**_tmp9E;void*_tmp9F;
struct Cyc_Core_Opt*_tmpA0;struct Cyc_Core_Opt**_tmpA1;void*_tmpA2;struct Cyc_Core_Opt*
_tmpA3;struct Cyc_Core_Opt**_tmpA4;void*_tmpA5;void*_tmpA6;_LLA9: if(*((int*)
_tmp9B)== 0){_LLB5: _tmp9C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp9B)->f1;if((
int)_tmp9C == 2){goto _LLAA;}else{goto _LLAB;}}else{goto _LLAB;}_LLAB: if(*((int*)
_tmp9B)== 1){_LLB6: _tmp9D=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp9B)->f1;
_tmp9E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp9B)->f1;
goto _LLAC;}else{goto _LLAD;}_LLAD: if(*((int*)_tmp9B)== 2){_LLB8: _tmpA0=((struct
Cyc_Absyn_Less_kb_struct*)_tmp9B)->f1;_tmpA1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp9B)->f1;goto _LLB7;_LLB7: _tmp9F=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp9B)->f2;if((int)_tmp9F == 1){goto _LLAE;}else{goto _LLAF;}}else{goto _LLAF;}
_LLAF: if(*((int*)_tmp9B)== 2){_LLBA: _tmpA3=((struct Cyc_Absyn_Less_kb_struct*)
_tmp9B)->f1;_tmpA4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp9B)->f1;goto _LLB9;_LLB9: _tmpA2=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp9B)->f2;if((int)_tmpA2 == 0){goto _LLB0;}else{goto _LLB1;}}else{goto _LLB1;}
_LLB1: if(*((int*)_tmp9B)== 0){_LLBB: _tmpA5=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp9B)->f1;goto _LLB2;}else{goto _LLB3;}_LLB3: if(*((int*)_tmp9B)== 2){_LLBC:
_tmpA6=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp9B)->f2;goto _LLB4;}else{goto
_LLA8;}_LLAA: goto _LLA8;_LLAC: _tmpA1=_tmp9E;goto _LLAE;_LLAE: _tmpA4=_tmpA1;goto
_LLB0;_LLB0:*_tmpA4=({struct Cyc_Core_Opt*_tmpA7=_cycalloc(sizeof(*_tmpA7));
_tmpA7->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpA8=_cycalloc(sizeof(*
_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpA9;_tmpA9.tag=0;_tmpA9.f1=(
void*)((void*)2);_tmpA9;});_tmpA8;}));_tmpA7;});goto _LLA8;_LLB2: _tmpA6=_tmpA5;
goto _LLB4;_LLB4:({struct Cyc_Std_String_pa_struct _tmpAD;_tmpAD.tag=0;_tmpAD.f1=(
struct _tagged_arr)Cyc_Absynpp_kind2string(_tmpA6);{struct Cyc_Std_String_pa_struct
_tmpAC;_tmpAC.tag=0;_tmpAC.f1=(struct _tagged_arr)*tv->name;{struct Cyc_Std_String_pa_struct
_tmpAB;_tmpAB.tag=0;_tmpAB.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp5F->name);{
void*_tmpAA[3]={& _tmpAB,& _tmpAC,& _tmpAD};Cyc_Tcutil_terr(p->loc,_tag_arr("struct %s abstracts type variable %s of kind %s != B",
sizeof(unsigned char),53),_tag_arr(_tmpAA,sizeof(void*),3));}}}});goto _LLA8;
_LLA8:;}_tmp9A=({struct Cyc_List_List*_tmpAE=_region_malloc(rgn,sizeof(*_tmpAE));
_tmpAE->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmpAF=_cycalloc(
sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_VarType_struct _tmpB0;_tmpB0.tag=1;
_tmpB0.f1=tv;_tmpB0;});_tmpAF;}));_tmpAE->tl=_tmp9A;_tmpAE;});}}_tmp9A=Cyc_List_imp_rev(
_tmp9A);tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(tv_result,_tmp59);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
p->loc,te,_tmp59);struct Cyc_List_List*_tmpB1=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple4 _tmpB2=({struct _tuple4 _tmpCA;_tmpCA.f1=_tmpB1;_tmpCA.f2=rgn;_tmpCA;});
struct Cyc_List_List*_tmpB3=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpB2,_tmp5F->tvs);struct
Cyc_List_List*_tmpB4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,
rgn,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5F->exist_vars))->v,
_tmp9A);struct Cyc_List_List*_tmpB5=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpB3);struct Cyc_List_List*_tmpB6=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpB4);struct Cyc_List_List*_tmpB7=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpB3,_tmpB4);*
_tmp5C=_tmpB5;t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpB8=_cycalloc(
sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_AggrType_struct _tmpB9;_tmpB9.tag=10;
_tmpB9.f1=({struct Cyc_Absyn_AggrInfo _tmpBA;_tmpBA.aggr_info=(void*)((void*)({
struct Cyc_Absyn_KnownAggr_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({
struct Cyc_Absyn_KnownAggr_struct _tmpBC;_tmpBC.tag=1;_tmpBC.f1=({struct Cyc_Absyn_Aggrdecl**
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=_tmp5F;_tmpBD;});_tmpBC;});_tmpBB;}));
_tmpBA.targs=*_tmp5C;_tmpBA;});_tmpB9;});_tmpB8;});{struct Cyc_List_List*fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,p->loc,_tmp58,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5F->fields))->v);
for(0;fields != 0;fields=fields->tl){struct _tuple9 _tmpBF;struct Cyc_Absyn_Pat*
_tmpC0;struct Cyc_Absyn_Aggrfield*_tmpC1;struct _tuple9*_tmpBE=(struct _tuple9*)
fields->hd;_tmpBF=*_tmpBE;_LLBF: _tmpC1=_tmpBF.f1;goto _LLBE;_LLBE: _tmpC0=_tmpBF.f2;
goto _LLBD;_LLBD: {void*_tmpC2=Cyc_Tcutil_rsubstitute(rgn,_tmpB7,(void*)_tmpC1->type);
struct Cyc_List_List*_tmpC4;struct Cyc_List_List*_tmpC5;struct _tuple6 _tmpC3=Cyc_Tcpat_tcPatRec(
te2,_tmpC0,(void**)& _tmpC2,region_opt);_LLC2: _tmpC5=_tmpC3.f1;goto _LLC1;_LLC1:
_tmpC4=_tmpC3.f2;goto _LLC0;_LLC0: tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(tv_result,_tmpC5);v_result=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(v_result,_tmpC4);
if(! Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmpC0->topt))->v,
_tmpC2)){({struct Cyc_Std_String_pa_struct _tmpC9;_tmpC9.tag=0;_tmpC9.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpC0->topt))->v);{struct Cyc_Std_String_pa_struct _tmpC8;_tmpC8.tag=0;_tmpC8.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpC2);{struct Cyc_Std_String_pa_struct
_tmpC7;_tmpC7.tag=0;_tmpC7.f1=(struct _tagged_arr)*_tmpC1->name;{void*_tmpC6[3]={&
_tmpC7,& _tmpC8,& _tmpC9};Cyc_Tcutil_terr(p->loc,_tag_arr("field %s of struct pattern expects type %s != %s",
sizeof(unsigned char),49),_tag_arr(_tmpC6,sizeof(void*),3));}}}});}}}}}};
_pop_region(rgn);}goto _LL4F;_LL6B: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp61)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp62->tvs)){({void*
_tmpCB[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("wrong number of existential type variables in pattern",
sizeof(unsigned char),54),_tag_arr(_tmpCB,sizeof(void*),0));});}if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp61)!= 0){region_opt=0;}{struct
_RegionHandle _tmpCC=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCC;
_push_region(rgn);{struct Cyc_List_List*_tmpCD=0;{struct Cyc_List_List*t=_tmp61;
for(0;t != 0;t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;{void*
_tmpCE=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpCF;struct Cyc_Core_Opt*
_tmpD0;struct Cyc_Core_Opt**_tmpD1;void*_tmpD2;struct Cyc_Core_Opt*_tmpD3;struct
Cyc_Core_Opt**_tmpD4;void*_tmpD5;struct Cyc_Core_Opt*_tmpD6;struct Cyc_Core_Opt**
_tmpD7;void*_tmpD8;void*_tmpD9;_LLC4: if(*((int*)_tmpCE)== 0){_LLD0: _tmpCF=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmpCE)->f1;if((int)_tmpCF == 2){goto _LLC5;}else{
goto _LLC6;}}else{goto _LLC6;}_LLC6: if(*((int*)_tmpCE)== 1){_LLD1: _tmpD0=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpCE)->f1;_tmpD1=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpCE)->f1;goto _LLC7;}else{goto _LLC8;}_LLC8: if(*((
int*)_tmpCE)== 2){_LLD3: _tmpD3=((struct Cyc_Absyn_Less_kb_struct*)_tmpCE)->f1;
_tmpD4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpCE)->f1;goto
_LLD2;_LLD2: _tmpD2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpCE)->f2;if((int)
_tmpD2 == 1){goto _LLC9;}else{goto _LLCA;}}else{goto _LLCA;}_LLCA: if(*((int*)_tmpCE)
== 2){_LLD5: _tmpD6=((struct Cyc_Absyn_Less_kb_struct*)_tmpCE)->f1;_tmpD7=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpCE)->f1;goto _LLD4;_LLD4:
_tmpD5=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpCE)->f2;if((int)_tmpD5 == 0){
goto _LLCB;}else{goto _LLCC;}}else{goto _LLCC;}_LLCC: if(*((int*)_tmpCE)== 0){_LLD6:
_tmpD8=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpCE)->f1;goto _LLCD;}else{goto
_LLCE;}_LLCE: if(*((int*)_tmpCE)== 2){_LLD7: _tmpD9=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpCE)->f2;goto _LLCF;}else{goto _LLC3;}_LLC5: goto _LLC3;_LLC7: _tmpD4=_tmpD1;goto
_LLC9;_LLC9: _tmpD7=_tmpD4;goto _LLCB;_LLCB:*_tmpD7=({struct Cyc_Core_Opt*_tmpDA=
_cycalloc(sizeof(*_tmpDA));_tmpDA->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpDC;
_tmpDC.tag=0;_tmpDC.f1=(void*)((void*)2);_tmpDC;});_tmpDB;}));_tmpDA;});goto
_LLC3;_LLCD: _tmpD9=_tmpD8;goto _LLCF;_LLCF:({struct Cyc_Std_String_pa_struct _tmpE0;
_tmpE0.tag=0;_tmpE0.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(_tmpD9);{
struct Cyc_Std_String_pa_struct _tmpDF;_tmpDF.tag=0;_tmpDF.f1=(struct _tagged_arr)*
tv->name;{struct Cyc_Std_String_pa_struct _tmpDE;_tmpDE.tag=0;_tmpDE.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp63->name);{void*_tmpDD[3]={& _tmpDE,&
_tmpDF,& _tmpE0};Cyc_Tcutil_terr(p->loc,_tag_arr("tunion member %s abstracts type variable %s of kind %s != B",
sizeof(unsigned char),60),_tag_arr(_tmpDD,sizeof(void*),3));}}}});goto _LLC3;
_LLC3:;}_tmpCD=({struct Cyc_List_List*_tmpE1=_region_malloc(rgn,sizeof(*_tmpE1));
_tmpE1->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmpE2=_cycalloc(
sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_VarType_struct _tmpE3;_tmpE3.tag=1;
_tmpE3.f1=tv;_tmpE3;});_tmpE2;}));_tmpE1->tl=_tmpCD;_tmpE1;});}}_tmpCD=Cyc_List_imp_rev(
_tmpCD);tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(tv_result,_tmp61);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
p->loc,te,_tmp61);struct Cyc_List_List*tqts=_tmp62->typs;struct Cyc_List_List*
_tmpE4=Cyc_Tcenv_lookup_type_vars(te);struct _tuple4 _tmpE5=({struct _tuple4 _tmp10D;
_tmp10D.f1=_tmpE4;_tmp10D.f2=rgn;_tmp10D;});struct Cyc_List_List*_tmpE6=((struct
Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct
Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,
Cyc_Tcutil_r_make_inst_var,& _tmpE5,_tmp63->tvs);struct Cyc_List_List*_tmpE7=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp62->tvs,_tmpCD);struct Cyc_List_List*
_tmpE8=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))
Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpE6);struct Cyc_List_List*
_tmpE9=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))
Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpE7);struct Cyc_List_List*
_tmpEA=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpE6,_tmpE7);if(tqts == 0){t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB[0]=({
struct Cyc_Absyn_TunionType_struct _tmpEC;_tmpEC.tag=2;_tmpEC.f1=({struct Cyc_Absyn_TunionInfo
_tmpED;_tmpED.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmpF0;_tmpF0.tag=1;_tmpF0.f1=({struct Cyc_Absyn_Tuniondecl**_tmpF1=_cycalloc(
sizeof(*_tmpF1));_tmpF1[0]=_tmp63;_tmpF1;});_tmpF0;});_tmpEF;}));_tmpED.targs=
_tmpE8;_tmpED.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->v=_tmpE4;_tmpEE;}));
_tmpED;});_tmpEC;});_tmpEB;});}else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpF3;_tmpF3.tag=3;_tmpF3.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpF4;_tmpF4.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmpF5=_cycalloc(sizeof(*
_tmpF5));_tmpF5[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmpF6;_tmpF6.tag=1;
_tmpF6.f1=_tmp63;_tmpF6.f2=_tmp62;_tmpF6;});_tmpF5;}));_tmpF4.targs=_tmpE8;
_tmpF4;});_tmpF3;});_tmpF2;});}if(topt != 0? tqts == 0: 0){void*_tmpF7=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_TunionInfo _tmpF8;struct Cyc_List_List*_tmpF9;_LLD9: if((
unsigned int)_tmpF7 > 3?*((int*)_tmpF7)== 3: 0){goto _LLDA;}else{goto _LLDB;}_LLDB:
if((unsigned int)_tmpF7 > 3?*((int*)_tmpF7)== 2: 0){_LLDF: _tmpF8=((struct Cyc_Absyn_TunionType_struct*)
_tmpF7)->f1;_LLE0: _tmpF9=_tmpF8.targs;goto _LLDC;}else{goto _LLDD;}_LLDD: goto _LLDE;
_LLDA: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmpFA=_cycalloc(sizeof(*
_tmpFA));_tmpFA[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmpFB;_tmpFB.tag=3;
_tmpFB.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpFC;_tmpFC.field_info=(void*)((
void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));
_tmpFD[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmpFE;_tmpFE.tag=1;_tmpFE.f1=
_tmp63;_tmpFE.f2=_tmp62;_tmpFE;});_tmpFD;}));_tmpFC.targs=_tmpE8;_tmpFC;});
_tmpFB;});_tmpFA;});goto _LLD8;_LLDC: {struct Cyc_List_List*_tmpFF=_tmpE8;for(0;
_tmpFF != 0? _tmpF9 != 0: 0;(_tmpFF=_tmpFF->tl,_tmpF9=_tmpF9->tl)){Cyc_Tcutil_unify((
void*)_tmpFF->hd,(void*)_tmpF9->hd);}goto _LLD8;}_LLDE: goto _LLD8;_LLD8:;}for(0;
_tmp60 != 0? tqts != 0: 0;(_tmp60=_tmp60->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmp100=(struct Cyc_Absyn_Pat*)_tmp60->hd;void*_tmp101=Cyc_Tcutil_rsubstitute(rgn,
_tmpEA,(*((struct _tuple8*)tqts->hd)).f2);struct Cyc_List_List*_tmp103;struct Cyc_List_List*
_tmp104;struct _tuple6 _tmp102=Cyc_Tcpat_tcPatRec(te2,_tmp100,(void**)& _tmp101,
region_opt);_LLE3: _tmp104=_tmp102.f1;goto _LLE2;_LLE2: _tmp103=_tmp102.f2;goto
_LLE1;_LLE1: tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(tv_result,_tmp104);v_result=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(v_result,_tmp103);if(!
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp100->topt))->v,
_tmp101)){({struct Cyc_Std_String_pa_struct _tmp108;_tmp108.tag=0;_tmp108.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp100->topt))->v);{struct Cyc_Std_String_pa_struct _tmp107;_tmp107.tag=
0;_tmp107.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp101);{struct Cyc_Std_String_pa_struct
_tmp106;_tmp106.tag=0;_tmp106.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp63->name);{void*_tmp105[3]={& _tmp106,& _tmp107,& _tmp108};Cyc_Tcutil_terr(
_tmp100->loc,_tag_arr("%s expects argument type %s, not %s",sizeof(unsigned char),
36),_tag_arr(_tmp105,sizeof(void*),3));}}}});}}if(_tmp60 != 0){({struct Cyc_Std_String_pa_struct
_tmp10A;_tmp10A.tag=0;_tmp10A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp63->name);{void*_tmp109[1]={& _tmp10A};Cyc_Tcutil_terr(p->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(unsigned char),45),_tag_arr(_tmp109,sizeof(void*),1));}});}if(tqts != 0){({
struct Cyc_Std_String_pa_struct _tmp10C;_tmp10C.tag=0;_tmp10C.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp63->name);{void*_tmp10B[1]={& _tmp10C};
Cyc_Tcutil_terr(p->loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(unsigned char),44),_tag_arr(_tmp10B,sizeof(void*),1));}});}}};_pop_region(
rgn);}goto _LL4F;_LL6D: goto _LL6F;_LL6F: goto _LL71;_LL71: t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp10E;}));goto _LL4F;_LL4F:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp10F=
_cycalloc(sizeof(*_tmp10F));_tmp10F->v=(void*)t;_tmp10F;});return({struct _tuple6
_tmp110;_tmp110.f1=tv_result;_tmp110.f2=v_result;_tmp110;});}}struct _tuple6 Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**region_opt){
struct _tuple6 _tmp111=Cyc_Tcpat_tcPatRec(te,p,topt,region_opt);{struct
_RegionHandle _tmp112=_new_region("r");struct _RegionHandle*r=& _tmp112;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp111.f2),p->loc,_tag_arr("pattern contains a repeated variable",
sizeof(unsigned char),37));;_pop_region(r);}return _tmp111;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp113=(void*)p->r;struct
Cyc_Absyn_Pat*_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_List_List*_tmp116;
struct Cyc_List_List*_tmp117;_LLE5: if((unsigned int)_tmp113 > 2?*((int*)_tmp113)== 
5: 0){_LLEF: _tmp114=((struct Cyc_Absyn_Pointer_p_struct*)_tmp113)->f1;goto _LLE6;}
else{goto _LLE7;}_LLE7: if((unsigned int)_tmp113 > 2?*((int*)_tmp113)== 8: 0){_LLF0:
_tmp115=((struct Cyc_Absyn_Tunion_p_struct*)_tmp113)->f4;goto _LLE8;}else{goto
_LLE9;}_LLE9: if((unsigned int)_tmp113 > 2?*((int*)_tmp113)== 7: 0){_LLF1: _tmp116=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp113)->f3;goto _LLEA;}else{goto _LLEB;}_LLEB: if((
unsigned int)_tmp113 > 2?*((int*)_tmp113)== 4: 0){_LLF2: _tmp117=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp113)->f1;goto _LLEC;}else{goto _LLED;}_LLED: goto _LLEE;_LLE6: Cyc_Tcpat_check_pat_regions(
te,_tmp114);{void*_tmp118=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp119;void*_tmp11A;struct Cyc_Absyn_TunionInfo _tmp11B;
void*_tmp11C;_LLF4: if((unsigned int)_tmp118 > 3?*((int*)_tmp118)== 4: 0){_LLFA:
_tmp119=((struct Cyc_Absyn_PointerType_struct*)_tmp118)->f1;_LLFB: _tmp11A=(void*)
_tmp119.rgn_typ;goto _LLF5;}else{goto _LLF6;}_LLF6: if((unsigned int)_tmp118 > 3?*((
int*)_tmp118)== 2: 0){_LLFC: _tmp11B=((struct Cyc_Absyn_TunionType_struct*)_tmp118)->f1;
_LLFD: _tmp11C=(void*)_tmp11B.rgn;goto _LLF7;}else{goto _LLF8;}_LLF8: goto _LLF9;
_LLF5: _tmp11C=_tmp11A;goto _LLF7;_LLF7: Cyc_Tcenv_check_rgn_accessible(te,p->loc,
_tmp11C);return;_LLF9:({void*_tmp11D[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_pat_regions: bad pointer type",
sizeof(unsigned char),36),_tag_arr(_tmp11D,sizeof(void*),0));});_LLF3:;}_LLE8:
for(0;_tmp115 != 0;_tmp115=_tmp115->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp115->hd);}{void*_tmp11E=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_TunionInfo _tmp11F;void*_tmp120;_LLFF: if((unsigned int)_tmp11E > 3?*((
int*)_tmp11E)== 2: 0){_LL105: _tmp11F=((struct Cyc_Absyn_TunionType_struct*)_tmp11E)->f1;
_LL106: _tmp120=(void*)_tmp11F.rgn;goto _LL100;}else{goto _LL101;}_LL101: if((
unsigned int)_tmp11E > 3?*((int*)_tmp11E)== 3: 0){goto _LL102;}else{goto _LL103;}
_LL103: goto _LL104;_LL100: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp120);
return;_LL102: return;_LL104:({void*_tmp121[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_pat_regions: bad tunion type",
sizeof(unsigned char),35),_tag_arr(_tmp121,sizeof(void*),0));});_LLFE:;}_LLEA:
for(0;_tmp116 != 0;_tmp116=_tmp116->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct
_tuple7*)_tmp116->hd)).f2);}return;_LLEC: for(0;_tmp117 != 0;_tmp117=_tmp117->tl){
Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmp117->hd);}return;_LLEE:
return;_LLE4:;}struct Cyc_Tcpat_Name_v_struct{int tag;struct _tagged_arr f1;};struct
Cyc_Tcpat_Int_v_struct{int tag;int f1;};struct Cyc_Tcpat_Con_s{void*name;int arity;
struct Cyc_Core_Opt*span;};struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*
c1,struct Cyc_Tcpat_Con_s*c2){void*_tmp122=(void*)c1->name;struct _tagged_arr
_tmp123;int _tmp124;_LL108: if(*((int*)_tmp122)== 0){_LL10C: _tmp123=((struct Cyc_Tcpat_Name_v_struct*)
_tmp122)->f1;goto _LL109;}else{goto _LL10A;}_LL10A: if(*((int*)_tmp122)== 1){_LL10D:
_tmp124=((struct Cyc_Tcpat_Int_v_struct*)_tmp122)->f1;goto _LL10B;}else{goto _LL107;}
_LL109: {void*_tmp125=(void*)c2->name;struct _tagged_arr _tmp126;_LL10F: if(*((int*)
_tmp125)== 0){_LL113: _tmp126=((struct Cyc_Tcpat_Name_v_struct*)_tmp125)->f1;goto
_LL110;}else{goto _LL111;}_LL111: if(*((int*)_tmp125)== 1){goto _LL112;}else{goto
_LL10E;}_LL110: return Cyc_Std_zstrcmp(_tmp123,_tmp126);_LL112: return - 1;_LL10E:;}
_LL10B: {void*_tmp127=(void*)c2->name;int _tmp128;_LL115: if(*((int*)_tmp127)== 0){
goto _LL116;}else{goto _LL117;}_LL117: if(*((int*)_tmp127)== 1){_LL119: _tmp128=((
struct Cyc_Tcpat_Int_v_struct*)_tmp127)->f1;goto _LL118;}else{goto _LL114;}_LL116:
return 1;_LL118: return _tmp124 - _tmp128;_LL114:;}_LL107:;}static struct Cyc_Set_Set*
Cyc_Tcpat_empty_con_set(){return((struct Cyc_Set_Set*(*)(int(*cmp)(struct Cyc_Tcpat_Con_s*,
struct Cyc_Tcpat_Con_s*)))Cyc_Set_empty)(Cyc_Tcpat_compare_con);}static struct Cyc_Core_Opt
Cyc_Tcpat_one_opt={(void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(void*)2};
static struct Cyc_Core_Opt*Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;
static unsigned char _tmp12A[5]="NULL";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={
0,{_tmp12A,_tmp12A,_tmp12A + 5}};static unsigned char _tmp12C[4]="ptr";static struct
Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={0,{_tmp12C,_tmp12C,_tmp12C + 4}};
static unsigned char _tmp12E[6]="tuple";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={
0,{_tmp12E,_tmp12E,_tmp12E + 6}};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)((void*)& Cyc_Tcpat_null_name_value),0,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={(void*)((void*)& Cyc_Tcpat_ptr_name_value),
1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)((void*)& Cyc_Tcpat_ptr_name_value),1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0;static struct
Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0;static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(
int i){return({struct Cyc_Tcpat_Con_s*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp130=_cycalloc_atomic(sizeof(*
_tmp130));_tmp130[0]=({struct Cyc_Tcpat_Int_v_struct _tmp131;_tmp131.tag=1;_tmp131.f1=
i;_tmp131;});_tmp130;}));_tmp12F->arity=0;_tmp12F->span=0;_tmp12F;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _tagged_arr f){return({struct Cyc_Tcpat_Con_s*
_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp134;_tmp134.tag=0;_tmp134.f1=f;_tmp134;});_tmp133;}));_tmp132->arity=0;
_tmp132->span=0;_tmp132;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(
unsigned char c){return({struct Cyc_Tcpat_Con_s*_tmp135=_cycalloc(sizeof(*_tmp135));
_tmp135->name=(void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp137=
_cycalloc_atomic(sizeof(*_tmp137));_tmp137[0]=({struct Cyc_Tcpat_Int_v_struct
_tmp138;_tmp138.tag=1;_tmp138.f1=(int)c;_tmp138;});_tmp137;}));_tmp135->arity=0;
_tmp135->span=({struct Cyc_Core_Opt*_tmp136=_cycalloc_atomic(sizeof(*_tmp136));
_tmp136->v=(void*)256;_tmp136;});_tmp135;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(
int i){return({struct Cyc_Tcpat_Con_s*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->name=(
void*)((void*)& Cyc_Tcpat_tuple_name_value);_tmp139->arity=i;_tmp139->span=Cyc_Tcpat_one_opt_ptr;
_tmp139;});}static void*Cyc_Tcpat_null_pat(){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=({struct Cyc_Tcpat_Con_struct
_tmp13B;_tmp13B.tag=0;_tmp13B.f1=Cyc_Tcpat_null_con;_tmp13B.f2=0;_tmp13B;});
_tmp13A;});}static void*Cyc_Tcpat_int_pat(int i){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C[0]=({struct Cyc_Tcpat_Con_struct
_tmp13D;_tmp13D.tag=0;_tmp13D.f1=Cyc_Tcpat_int_con(i);_tmp13D.f2=0;_tmp13D;});
_tmp13C;});}static void*Cyc_Tcpat_char_pat(unsigned char c){return(void*)({struct
Cyc_Tcpat_Con_struct*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E[0]=({struct Cyc_Tcpat_Con_struct
_tmp13F;_tmp13F.tag=0;_tmp13F.f1=Cyc_Tcpat_char_con(c);_tmp13F.f2=0;_tmp13F;});
_tmp13E;});}static void*Cyc_Tcpat_float_pat(struct _tagged_arr f){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140[0]=({
struct Cyc_Tcpat_Con_struct _tmp141;_tmp141.tag=0;_tmp141.f1=Cyc_Tcpat_float_con(f);
_tmp141.f2=0;_tmp141;});_tmp140;});}static void*Cyc_Tcpat_null_ptr_pat(void*p){
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp142=_cycalloc(sizeof(*_tmp142));
_tmp142[0]=({struct Cyc_Tcpat_Con_struct _tmp143;_tmp143.tag=0;_tmp143.f1=Cyc_Tcpat_null_ptr_con;
_tmp143.f2=({struct Cyc_List_List*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->hd=(
void*)p;_tmp144->tl=0;_tmp144;});_tmp143;});_tmp142;});}static void*Cyc_Tcpat_ptr_pat(
void*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp145=_cycalloc(sizeof(*
_tmp145));_tmp145[0]=({struct Cyc_Tcpat_Con_struct _tmp146;_tmp146.tag=0;_tmp146.f1=
Cyc_Tcpat_ptr_con;_tmp146.f2=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*
_tmp147));_tmp147->hd=(void*)p;_tmp147->tl=0;_tmp147;});_tmp146;});_tmp145;});}
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148[0]=({struct Cyc_Tcpat_Con_struct
_tmp149;_tmp149.tag=0;_tmp149.f1=Cyc_Tcpat_tuple_con(Cyc_List_length(ss));
_tmp149.f2=ss;_tmp149;});_tmp148;});}static void*Cyc_Tcpat_con_pat(struct
_tagged_arr con_name,struct Cyc_Core_Opt*span,struct Cyc_List_List*ps){struct Cyc_Tcpat_Con_s*
c=({struct Cyc_Tcpat_Con_s*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->name=(
void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));
_tmp14D[0]=({struct Cyc_Tcpat_Name_v_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=
con_name;_tmp14E;});_tmp14D;}));_tmp14C->arity=Cyc_List_length(ps);_tmp14C->span=
span;_tmp14C;});return(void*)({struct Cyc_Tcpat_Con_struct*_tmp14A=_cycalloc(
sizeof(*_tmp14A));_tmp14A[0]=({struct Cyc_Tcpat_Con_struct _tmp14B;_tmp14B.tag=0;
_tmp14B.f1=c;_tmp14B.f2=ps;_tmp14B;});_tmp14A;});}static void*Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat*p){void*s;{void*_tmp14F=(void*)p->r;int _tmp150;void*_tmp151;
unsigned char _tmp152;struct _tagged_arr _tmp153;struct Cyc_Absyn_Pat*_tmp154;struct
Cyc_List_List*_tmp155;struct Cyc_Absyn_Tunionfield*_tmp156;struct Cyc_Absyn_Tuniondecl*
_tmp157;struct Cyc_List_List*_tmp158;struct Cyc_List_List*_tmp159;struct Cyc_List_List*
_tmp15A;struct Cyc_Absyn_AggrInfo _tmp15B;void*_tmp15C;struct Cyc_Absyn_Aggrdecl**
_tmp15D;struct Cyc_Absyn_Aggrdecl*_tmp15E;struct Cyc_Absyn_Enumfield*_tmp15F;
struct Cyc_Absyn_Enumdecl*_tmp160;struct Cyc_Absyn_Enumfield*_tmp161;void*_tmp162;
_LL11B: if((int)_tmp14F == 0){goto _LL11C;}else{goto _LL11D;}_LL11D: if((unsigned int)
_tmp14F > 2?*((int*)_tmp14F)== 0: 0){goto _LL11E;}else{goto _LL11F;}_LL11F: if((int)
_tmp14F == 1){goto _LL120;}else{goto _LL121;}_LL121: if((unsigned int)_tmp14F > 2?*((
int*)_tmp14F)== 1: 0){_LL13E: _tmp151=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp14F)->f1;goto _LL13D;_LL13D: _tmp150=((struct Cyc_Absyn_Int_p_struct*)_tmp14F)->f2;
goto _LL122;}else{goto _LL123;}_LL123: if((unsigned int)_tmp14F > 2?*((int*)_tmp14F)
== 2: 0){_LL13F: _tmp152=((struct Cyc_Absyn_Char_p_struct*)_tmp14F)->f1;goto _LL124;}
else{goto _LL125;}_LL125: if((unsigned int)_tmp14F > 2?*((int*)_tmp14F)== 3: 0){
_LL140: _tmp153=((struct Cyc_Absyn_Float_p_struct*)_tmp14F)->f1;goto _LL126;}else{
goto _LL127;}_LL127: if((unsigned int)_tmp14F > 2?*((int*)_tmp14F)== 6: 0){goto
_LL128;}else{goto _LL129;}_LL129: if((unsigned int)_tmp14F > 2?*((int*)_tmp14F)== 5:
0){_LL141: _tmp154=((struct Cyc_Absyn_Pointer_p_struct*)_tmp14F)->f1;goto _LL12A;}
else{goto _LL12B;}_LL12B: if((unsigned int)_tmp14F > 2?*((int*)_tmp14F)== 8: 0){
_LL144: _tmp157=((struct Cyc_Absyn_Tunion_p_struct*)_tmp14F)->f1;goto _LL143;_LL143:
_tmp156=((struct Cyc_Absyn_Tunion_p_struct*)_tmp14F)->f2;goto _LL142;_LL142:
_tmp155=((struct Cyc_Absyn_Tunion_p_struct*)_tmp14F)->f4;if(_tmp155 == 0){goto
_LL12C;}else{goto _LL12D;}}else{goto _LL12D;}_LL12D: if((unsigned int)_tmp14F > 2?*((
int*)_tmp14F)== 8: 0){_LL145: _tmp158=((struct Cyc_Absyn_Tunion_p_struct*)_tmp14F)->f4;
goto _LL12E;}else{goto _LL12F;}_LL12F: if((unsigned int)_tmp14F > 2?*((int*)_tmp14F)
== 4: 0){_LL146: _tmp159=((struct Cyc_Absyn_Tuple_p_struct*)_tmp14F)->f1;goto _LL130;}
else{goto _LL131;}_LL131: if((unsigned int)_tmp14F > 2?*((int*)_tmp14F)== 7: 0){
_LL148: _tmp15B=((struct Cyc_Absyn_Aggr_p_struct*)_tmp14F)->f1;_LL149: _tmp15C=(
void*)_tmp15B.aggr_info;if(*((int*)_tmp15C)== 1){_LL14A: _tmp15D=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp15C)->f1;_tmp15E=*_tmp15D;goto _LL147;}else{goto _LL133;}_LL147: _tmp15A=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp14F)->f3;goto _LL132;}else{goto _LL133;}_LL133:
if((unsigned int)_tmp14F > 2?*((int*)_tmp14F)== 9: 0){_LL14C: _tmp160=((struct Cyc_Absyn_Enum_p_struct*)
_tmp14F)->f1;goto _LL14B;_LL14B: _tmp15F=((struct Cyc_Absyn_Enum_p_struct*)_tmp14F)->f2;
goto _LL134;}else{goto _LL135;}_LL135: if((unsigned int)_tmp14F > 2?*((int*)_tmp14F)
== 10: 0){_LL14E: _tmp162=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp14F)->f1;
goto _LL14D;_LL14D: _tmp161=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp14F)->f2;goto
_LL136;}else{goto _LL137;}_LL137: if((unsigned int)_tmp14F > 2?*((int*)_tmp14F)== 
11: 0){goto _LL138;}else{goto _LL139;}_LL139: if((unsigned int)_tmp14F > 2?*((int*)
_tmp14F)== 12: 0){goto _LL13A;}else{goto _LL13B;}_LL13B: if((unsigned int)_tmp14F > 2?*((
int*)_tmp14F)== 7: 0){goto _LL13C;}else{goto _LL11A;}_LL11C: s=(void*)0;goto _LL11A;
_LL11E: s=(void*)0;goto _LL11A;_LL120: s=Cyc_Tcpat_null_pat();goto _LL11A;_LL122: s=
Cyc_Tcpat_int_pat(_tmp150);goto _LL11A;_LL124: s=Cyc_Tcpat_char_pat(_tmp152);goto
_LL11A;_LL126: s=Cyc_Tcpat_float_pat(_tmp153);goto _LL11A;_LL128: s=(void*)0;goto
_LL11A;_LL12A:{void*_tmp163=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo _tmp164;struct Cyc_Absyn_Conref*
_tmp165;_LL150: if((unsigned int)_tmp163 > 3?*((int*)_tmp163)== 4: 0){_LL156:
_tmp164=((struct Cyc_Absyn_PointerType_struct*)_tmp163)->f1;_LL157: _tmp165=
_tmp164.nullable;goto _LL151;}else{goto _LL152;}_LL152: if((unsigned int)_tmp163 > 3?*((
int*)_tmp163)== 2: 0){goto _LL153;}else{goto _LL154;}_LL154: goto _LL155;_LL151: {int
is_nullable=0;int still_working=1;while(still_working){void*_tmp166=(void*)
_tmp165->v;struct Cyc_Absyn_Conref*_tmp167;int _tmp168;_LL159: if((unsigned int)
_tmp166 > 1?*((int*)_tmp166)== 1: 0){_LL15F: _tmp167=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp166)->f1;goto _LL15A;}else{goto _LL15B;}_LL15B: if((int)_tmp166 == 0){goto _LL15C;}
else{goto _LL15D;}_LL15D: if((unsigned int)_tmp166 > 1?*((int*)_tmp166)== 0: 0){
_LL160: _tmp168=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp166)->f1;goto _LL15E;}
else{goto _LL158;}_LL15A:(void*)(_tmp165->v=(void*)((void*)_tmp167->v));continue;
_LL15C:(void*)(_tmp165->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp16A;_tmp16A.tag=0;_tmp16A.f1=(void*)0;_tmp16A;});_tmp169;})));is_nullable=0;
still_working=0;goto _LL158;_LL15E: is_nullable=(int)_tmp168;still_working=0;goto
_LL158;_LL158:;}{void*ss=Cyc_Tcpat_compile_pat(_tmp154);if(is_nullable){s=Cyc_Tcpat_null_ptr_pat(
ss);}else{s=Cyc_Tcpat_ptr_pat(ss);}goto _LL14F;}}_LL153:{void*_tmp16B=(void*)
_tmp154->r;struct Cyc_List_List*_tmp16C;struct Cyc_Absyn_Tunionfield*_tmp16D;
struct Cyc_Absyn_Tuniondecl*_tmp16E;_LL162: if((unsigned int)_tmp16B > 2?*((int*)
_tmp16B)== 8: 0){_LL168: _tmp16E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp16B)->f1;
goto _LL167;_LL167: _tmp16D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp16B)->f2;goto
_LL166;_LL166: _tmp16C=((struct Cyc_Absyn_Tunion_p_struct*)_tmp16B)->f4;goto _LL163;}
else{goto _LL164;}_LL164: goto _LL165;_LL163: {struct Cyc_Core_Opt*span;if(_tmp16E->is_xtunion){
span=0;}else{span=({struct Cyc_Core_Opt*_tmp16F=_cycalloc_atomic(sizeof(*_tmp16F));
_tmp16F->v=(void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp16E->fields))->v);_tmp16F;});}s=Cyc_Tcpat_con_pat(*(*
_tmp16D->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp16C));goto _LL161;}
_LL165:({void*_tmp170[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("non-[x]tunion pattern has tunion type",sizeof(
unsigned char),38),_tag_arr(_tmp170,sizeof(void*),0));});_LL161:;}goto _LL14F;
_LL155:({void*_tmp171[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("pointer pattern does not have pointer type",sizeof(
unsigned char),43),_tag_arr(_tmp171,sizeof(void*),0));});_LL14F:;}goto _LL11A;
_LL12C: {struct Cyc_Core_Opt*span;{void*_tmp172=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v);_LL16A: if((unsigned int)_tmp172 > 3?*((
int*)_tmp172)== 2: 0){goto _LL16B;}else{goto _LL16C;}_LL16C: if((unsigned int)
_tmp172 > 3?*((int*)_tmp172)== 3: 0){goto _LL16D;}else{goto _LL16E;}_LL16E: goto
_LL16F;_LL16B: if(_tmp157->is_xtunion){span=0;}else{span=({struct Cyc_Core_Opt*
_tmp173=_cycalloc_atomic(sizeof(*_tmp173));_tmp173->v=(void*)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp157->fields))->v);_tmp173;});}goto _LL169;_LL16D: span=({struct Cyc_Core_Opt*
_tmp174=_cycalloc_atomic(sizeof(*_tmp174));_tmp174->v=(void*)1;_tmp174;});goto
_LL169;_LL16F: span=({void*_tmp175[0]={};((struct Cyc_Core_Opt*(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("void tunion pattern has bad type",
sizeof(unsigned char),33),_tag_arr(_tmp175,sizeof(void*),0));});goto _LL169;
_LL169:;}s=Cyc_Tcpat_con_pat(*(*_tmp156->name).f2,span,0);goto _LL11A;}_LL12E:
_tmp159=_tmp158;goto _LL130;_LL130: s=Cyc_Tcpat_tuple_pat(((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,
_tmp159));goto _LL11A;_LL132: {struct Cyc_List_List*ps=0;{struct Cyc_List_List*
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp15E->fields))->v;
for(0;fields != 0;fields=fields->tl){int found=Cyc_Std_strcmp(*((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name,_tag_arr("",sizeof(unsigned char),1))== 0;{struct Cyc_List_List*
dlps0=_tmp15A;for(0;! found? dlps0 != 0: 0;dlps0=dlps0->tl){struct _tuple7 _tmp177;
struct Cyc_Absyn_Pat*_tmp178;struct Cyc_List_List*_tmp179;struct _tuple7*_tmp176=(
struct _tuple7*)dlps0->hd;_tmp177=*_tmp176;_LL172: _tmp179=_tmp177.f1;goto _LL171;
_LL171: _tmp178=_tmp177.f2;goto _LL170;_LL170: {struct Cyc_List_List*_tmp17A=
_tmp179;struct Cyc_List_List _tmp17B;struct Cyc_List_List*_tmp17C;void*_tmp17D;
struct _tagged_arr*_tmp17E;_LL174: if(_tmp17A == 0){goto _LL176;}else{_tmp17B=*
_tmp17A;_LL179: _tmp17D=(void*)_tmp17B.hd;if(*((int*)_tmp17D)== 1){_LL17A: _tmp17E=((
struct Cyc_Absyn_FieldName_struct*)_tmp17D)->f1;goto _LL178;}else{goto _LL176;}
_LL178: _tmp17C=_tmp17B.tl;if(_tmp17C == 0){goto _LL175;}else{goto _LL176;}}_LL176:
goto _LL177;_LL175: if(Cyc_Std_zstrptrcmp(_tmp17E,((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name)== 0){ps=({struct Cyc_List_List*_tmp17F=_cycalloc(sizeof(*
_tmp17F));_tmp17F->hd=(void*)Cyc_Tcpat_compile_pat(_tmp178);_tmp17F->tl=ps;
_tmp17F;});found=1;}goto _LL173;_LL177:({void*_tmp180[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad designator(s)",
sizeof(unsigned char),18),_tag_arr(_tmp180,sizeof(void*),0));});_LL173:;}}}if(!
found){({void*_tmp181[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bad designator",sizeof(unsigned char),15),_tag_arr(
_tmp181,sizeof(void*),0));});}}}s=Cyc_Tcpat_tuple_pat(ps);goto _LL11A;}_LL134: {
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp160->fields))->v);s=Cyc_Tcpat_con_pat(*(*
_tmp15F->name).f2,({struct Cyc_Core_Opt*_tmp182=_cycalloc_atomic(sizeof(*_tmp182));
_tmp182->v=(void*)span;_tmp182;}),0);goto _LL11A;}_LL136: {struct Cyc_List_List*
fields;{void*_tmp183=Cyc_Tcutil_compress(_tmp162);struct Cyc_List_List*_tmp184;
_LL17C: if((unsigned int)_tmp183 > 3?*((int*)_tmp183)== 13: 0){_LL180: _tmp184=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp183)->f1;goto _LL17D;}else{goto _LL17E;}
_LL17E: goto _LL17F;_LL17D: fields=_tmp184;goto _LL17B;_LL17F:({void*_tmp185[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad type in AnonEnum_p",
sizeof(unsigned char),23),_tag_arr(_tmp185,sizeof(void*),0));});_LL17B:;}{int
span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*
_tmp161->name).f2,({struct Cyc_Core_Opt*_tmp186=_cycalloc_atomic(sizeof(*_tmp186));
_tmp186->v=(void*)span;_tmp186;}),0);goto _LL11A;}}_LL138:({void*_tmp187[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("compile_pat: unknownid",
sizeof(unsigned char),23),_tag_arr(_tmp187,sizeof(void*),0));});_LL13A:({void*
_tmp188[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("compile_pat: unknowncall",sizeof(unsigned char),25),_tag_arr(_tmp188,
sizeof(void*),0));});_LL13C:({void*_tmp189[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("compile_pat: bad aggr",sizeof(
unsigned char),22),_tag_arr(_tmp189,sizeof(void*),0));});_LL11A:;}return s;}
struct Cyc_Tcpat_Pos_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_Neg_struct{int tag;struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Success_struct{
int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct _tuple10{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static void*Cyc_Tcpat_add_neg(
void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp18A=td;struct Cyc_Set_Set*_tmp18B;
_LL182: if(*((int*)_tmp18A)== 1){_LL186: _tmp18B=((struct Cyc_Tcpat_Neg_struct*)
_tmp18A)->f1;goto _LL183;}else{goto _LL184;}_LL184: if(*((int*)_tmp18A)== 0){goto
_LL185;}else{goto _LL181;}_LL183: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp18B,c)){({void*_tmp18C[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("add_neg called when constructor already in set",
sizeof(unsigned char),47),_tag_arr(_tmp18C,sizeof(void*),0));});}if(c->span != 0?((
int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp18B)+ 1 >= (int)((struct Cyc_Core_Opt*)
_check_null(c->span))->v: 0){({void*_tmp18D[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("add_neg called when |cs U {c}| >= span(c)",
sizeof(unsigned char),42),_tag_arr(_tmp18D,sizeof(void*),0));});}return(void*)({
struct Cyc_Tcpat_Neg_struct*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E[0]=({
struct Cyc_Tcpat_Neg_struct _tmp18F;_tmp18F.tag=1;_tmp18F.f1=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp18B,c);
_tmp18F;});_tmp18E;});_LL185: return({void*_tmp190[0]={};Cyc_Tcutil_impos(
_tag_arr("add_neg called when td is Positive",sizeof(unsigned char),35),_tag_arr(
_tmp190,sizeof(void*),0));});_LL181:;}static void*Cyc_Tcpat_static_match(struct
Cyc_Tcpat_Con_s*c,void*td){void*_tmp191=td;struct Cyc_Tcpat_Con_s*_tmp192;struct
Cyc_Set_Set*_tmp193;_LL188: if(*((int*)_tmp191)== 0){_LL18C: _tmp192=((struct Cyc_Tcpat_Pos_struct*)
_tmp191)->f1;goto _LL189;}else{goto _LL18A;}_LL18A: if(*((int*)_tmp191)== 1){_LL18D:
_tmp193=((struct Cyc_Tcpat_Neg_struct*)_tmp191)->f1;goto _LL18B;}else{goto _LL187;}
_LL189: if(Cyc_Tcpat_compare_con(c,_tmp192)== 0){return(void*)0;}else{return(void*)
1;}_LL18B: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp193,c)){return(void*)1;}else{if(c->span != 0?(int)((struct Cyc_Core_Opt*)
_check_null(c->span))->v == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp193)+ 1: 0){return(void*)0;}else{return(void*)2;}}_LL187:;}struct _tuple11{
struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*_tmp194=ctxt;struct Cyc_List_List
_tmp195;struct Cyc_List_List*_tmp196;struct _tuple11*_tmp197;struct _tuple11 _tmp198;
struct Cyc_List_List*_tmp199;struct Cyc_Tcpat_Con_s*_tmp19A;_LL18F: if(_tmp194 == 0){
goto _LL190;}else{goto _LL191;}_LL191: if(_tmp194 == 0){goto _LL18E;}else{_tmp195=*
_tmp194;_LL194: _tmp197=(struct _tuple11*)_tmp195.hd;_tmp198=*_tmp197;_LL196:
_tmp19A=_tmp198.f1;goto _LL195;_LL195: _tmp199=_tmp198.f2;goto _LL193;_LL193:
_tmp196=_tmp195.tl;goto _LL192;}_LL190: return 0;_LL192: return({struct Cyc_List_List*
_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->hd=({struct _tuple11*_tmp19C=
_cycalloc(sizeof(*_tmp19C));_tmp19C->f1=_tmp19A;_tmp19C->f2=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->hd=(void*)dsc;
_tmp19D->tl=_tmp199;_tmp19D;});_tmp19C;});_tmp19B->tl=_tmp196;_tmp19B;});_LL18E:;}
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp19E=ctxt;struct Cyc_List_List _tmp19F;struct Cyc_List_List*
_tmp1A0;struct _tuple11*_tmp1A1;struct _tuple11 _tmp1A2;struct Cyc_List_List*_tmp1A3;
struct Cyc_Tcpat_Con_s*_tmp1A4;_LL198: if(_tmp19E == 0){goto _LL199;}else{goto _LL19A;}
_LL19A: if(_tmp19E == 0){goto _LL197;}else{_tmp19F=*_tmp19E;_LL19D: _tmp1A1=(struct
_tuple11*)_tmp19F.hd;_tmp1A2=*_tmp1A1;_LL19F: _tmp1A4=_tmp1A2.f1;goto _LL19E;
_LL19E: _tmp1A3=_tmp1A2.f2;goto _LL19C;_LL19C: _tmp1A0=_tmp19F.tl;goto _LL19B;}
_LL199: return({void*_tmp1A5[0]={};((struct Cyc_List_List*(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("norm_context: empty context",
sizeof(unsigned char),28),_tag_arr(_tmp1A5,sizeof(void*),0));});_LL19B: return Cyc_Tcpat_augment(
_tmp1A0,(void*)({struct Cyc_Tcpat_Pos_struct*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));
_tmp1A6[0]=({struct Cyc_Tcpat_Pos_struct _tmp1A7;_tmp1A7.tag=0;_tmp1A7.f1=_tmp1A4;
_tmp1A7.f2=Cyc_List_rev(_tmp1A3);_tmp1A7;});_tmp1A6;}));_LL197:;}static void*Cyc_Tcpat_build_desc(
struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){struct _tuple6 _tmp1A9=({
struct _tuple6 _tmp1A8;_tmp1A8.f1=ctxt;_tmp1A8.f2=work;_tmp1A8;});struct Cyc_List_List*
_tmp1AA;struct Cyc_List_List*_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List*
_tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_List_List _tmp1AF;struct Cyc_List_List*
_tmp1B0;struct _tuple10*_tmp1B1;struct _tuple10 _tmp1B2;struct Cyc_List_List*_tmp1B3;
struct Cyc_List_List*_tmp1B4;struct Cyc_List_List _tmp1B5;struct Cyc_List_List*
_tmp1B6;struct _tuple11*_tmp1B7;struct _tuple11 _tmp1B8;struct Cyc_List_List*_tmp1B9;
struct Cyc_Tcpat_Con_s*_tmp1BA;_LL1A1: _LL1AA: _tmp1AB=_tmp1A9.f1;if(_tmp1AB == 0){
goto _LL1A9;}else{goto _LL1A3;}_LL1A9: _tmp1AA=_tmp1A9.f2;if(_tmp1AA == 0){goto
_LL1A2;}else{goto _LL1A3;}_LL1A3: _LL1AB: _tmp1AC=_tmp1A9.f1;if(_tmp1AC == 0){goto
_LL1A4;}else{goto _LL1A5;}_LL1A5: _LL1AC: _tmp1AD=_tmp1A9.f2;if(_tmp1AD == 0){goto
_LL1A6;}else{goto _LL1A7;}_LL1A7: _LL1B1: _tmp1B4=_tmp1A9.f1;if(_tmp1B4 == 0){goto
_LL1A0;}else{_tmp1B5=*_tmp1B4;_LL1B3: _tmp1B7=(struct _tuple11*)_tmp1B5.hd;_tmp1B8=*
_tmp1B7;_LL1B5: _tmp1BA=_tmp1B8.f1;goto _LL1B4;_LL1B4: _tmp1B9=_tmp1B8.f2;goto
_LL1B2;_LL1B2: _tmp1B6=_tmp1B5.tl;goto _LL1AD;}_LL1AD: _tmp1AE=_tmp1A9.f2;if(
_tmp1AE == 0){goto _LL1A0;}else{_tmp1AF=*_tmp1AE;_LL1AF: _tmp1B1=(struct _tuple10*)
_tmp1AF.hd;_tmp1B2=*_tmp1B1;_LL1B0: _tmp1B3=_tmp1B2.f3;goto _LL1AE;_LL1AE: _tmp1B0=
_tmp1AF.tl;goto _LL1A8;}_LL1A2: return dsc;_LL1A4: goto _LL1A6;_LL1A6: return({void*
_tmp1BB[0]={};Cyc_Tcutil_impos(_tag_arr("build_desc: ctxt and work don't match",
sizeof(unsigned char),38),_tag_arr(_tmp1BB,sizeof(void*),0));});_LL1A8: {struct
Cyc_Tcpat_Pos_struct*_tmp1BC=({struct Cyc_Tcpat_Pos_struct*_tmp1BD=_cycalloc(
sizeof(*_tmp1BD));_tmp1BD[0]=({struct Cyc_Tcpat_Pos_struct _tmp1BE;_tmp1BE.tag=0;
_tmp1BE.f1=_tmp1BA;_tmp1BE.f2=Cyc_List_append(Cyc_List_rev(_tmp1B9),({struct Cyc_List_List*
_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF->hd=(void*)dsc;_tmp1BF->tl=_tmp1B3;
_tmp1BF;}));_tmp1BE;});_tmp1BD;});return Cyc_Tcpat_build_desc(_tmp1B6,(void*)
_tmp1BC,_tmp1B0);}_LL1A0:;}static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*
obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules);static void*Cyc_Tcpat_or_match(void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp1C0=allmrules;struct
Cyc_List_List _tmp1C1;struct Cyc_List_List*_tmp1C2;struct _tuple0*_tmp1C3;struct
_tuple0 _tmp1C4;void*_tmp1C5;void*_tmp1C6;_LL1B7: if(_tmp1C0 == 0){goto _LL1B8;}
else{goto _LL1B9;}_LL1B9: if(_tmp1C0 == 0){goto _LL1B6;}else{_tmp1C1=*_tmp1C0;_LL1BC:
_tmp1C3=(struct _tuple0*)_tmp1C1.hd;_tmp1C4=*_tmp1C3;_LL1BE: _tmp1C6=_tmp1C4.f1;
goto _LL1BD;_LL1BD: _tmp1C5=_tmp1C4.f2;goto _LL1BB;_LL1BB: _tmp1C2=_tmp1C1.tl;goto
_LL1BA;}_LL1B8: return(void*)0;_LL1BA: return Cyc_Tcpat_match(_tmp1C6,0,dsc,0,0,
_tmp1C5,_tmp1C2);_LL1B6:;}static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*
allmrules){return Cyc_Tcpat_or_match((void*)({struct Cyc_Tcpat_Neg_struct*_tmp1C7=
_cycalloc(sizeof(*_tmp1C7));_tmp1C7[0]=({struct Cyc_Tcpat_Neg_struct _tmp1C8;
_tmp1C8.tag=1;_tmp1C8.f1=Cyc_Tcpat_empty_con_set();_tmp1C8;});_tmp1C7;}),
allmrules);}static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp1C9=
work;struct Cyc_List_List _tmp1CA;struct Cyc_List_List*_tmp1CB;struct _tuple10*
_tmp1CC;struct _tuple10 _tmp1CD;struct Cyc_List_List*_tmp1CE;struct Cyc_List_List*
_tmp1CF;struct Cyc_List_List*_tmp1D0;struct Cyc_List_List _tmp1D1;struct Cyc_List_List*
_tmp1D2;struct _tuple10*_tmp1D3;struct _tuple10 _tmp1D4;struct Cyc_List_List*_tmp1D5;
struct Cyc_List_List*_tmp1D6;struct Cyc_List_List*_tmp1D7;_LL1C0: if(_tmp1C9 == 0){
goto _LL1C1;}else{goto _LL1C2;}_LL1C2: if(_tmp1C9 == 0){goto _LL1C4;}else{_tmp1CA=*
_tmp1C9;_LL1C7: _tmp1CC=(struct _tuple10*)_tmp1CA.hd;_tmp1CD=*_tmp1CC;_LL1CA:
_tmp1D0=_tmp1CD.f1;if(_tmp1D0 == 0){goto _LL1C9;}else{goto _LL1C4;}_LL1C9: _tmp1CF=
_tmp1CD.f2;if(_tmp1CF == 0){goto _LL1C8;}else{goto _LL1C4;}_LL1C8: _tmp1CE=_tmp1CD.f3;
if(_tmp1CE == 0){goto _LL1C6;}else{goto _LL1C4;}_LL1C6: _tmp1CB=_tmp1CA.tl;goto
_LL1C3;}_LL1C4: if(_tmp1C9 == 0){goto _LL1BF;}else{_tmp1D1=*_tmp1C9;_LL1CC: _tmp1D3=(
struct _tuple10*)_tmp1D1.hd;_tmp1D4=*_tmp1D3;_LL1CF: _tmp1D7=_tmp1D4.f1;goto _LL1CE;
_LL1CE: _tmp1D6=_tmp1D4.f2;goto _LL1CD;_LL1CD: _tmp1D5=_tmp1D4.f3;goto _LL1CB;_LL1CB:
_tmp1D2=_tmp1D1.tl;goto _LL1C5;}_LL1C1: return(void*)({struct Cyc_Tcpat_Success_struct*
_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8[0]=({struct Cyc_Tcpat_Success_struct
_tmp1D9;_tmp1D9.tag=0;_tmp1D9.f1=(void*)right_hand_side;_tmp1D9;});_tmp1D8;});
_LL1C3: return Cyc_Tcpat_and_match(Cyc_Tcpat_norm_context(ctx),_tmp1CB,
right_hand_side,rules);_LL1C5: if((_tmp1D7 == 0? 1: _tmp1D6 == 0)? 1: _tmp1D5 == 0){
return({void*_tmp1DA[0]={};Cyc_Tcutil_impos(_tag_arr("tcpat:and_match: malformed work frame",
sizeof(unsigned char),38),_tag_arr(_tmp1DA,sizeof(void*),0));});}{struct Cyc_List_List
_tmp1DC;struct Cyc_List_List*_tmp1DD;void*_tmp1DE;struct Cyc_List_List*_tmp1DB=(
struct Cyc_List_List*)_check_null(_tmp1D7);_tmp1DC=*_tmp1DB;_LL1D2: _tmp1DE=(void*)
_tmp1DC.hd;goto _LL1D1;_LL1D1: _tmp1DD=_tmp1DC.tl;goto _LL1D0;_LL1D0: {struct Cyc_List_List
_tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_List_List*
_tmp1DF=(struct Cyc_List_List*)_check_null(_tmp1D6);_tmp1E0=*_tmp1DF;_LL1D5:
_tmp1E2=(struct Cyc_List_List*)_tmp1E0.hd;goto _LL1D4;_LL1D4: _tmp1E1=_tmp1E0.tl;
goto _LL1D3;_LL1D3: {struct Cyc_List_List _tmp1E4;struct Cyc_List_List*_tmp1E5;void*
_tmp1E6;struct Cyc_List_List*_tmp1E3=(struct Cyc_List_List*)_check_null(_tmp1D5);
_tmp1E4=*_tmp1E3;_LL1D8: _tmp1E6=(void*)_tmp1E4.hd;goto _LL1D7;_LL1D7: _tmp1E5=
_tmp1E4.tl;goto _LL1D6;_LL1D6: {struct _tuple10*_tmp1E7=({struct _tuple10*_tmp1E9=
_cycalloc(sizeof(*_tmp1E9));_tmp1E9->f1=_tmp1DD;_tmp1E9->f2=_tmp1E1;_tmp1E9->f3=
_tmp1E5;_tmp1E9;});return Cyc_Tcpat_match(_tmp1DE,_tmp1E2,_tmp1E6,ctx,({struct Cyc_List_List*
_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->hd=_tmp1E7;_tmp1E8->tl=_tmp1D2;
_tmp1E8;}),right_hand_side,rules);}}}}_LL1BF:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp1EA=dsc;struct Cyc_Set_Set*_tmp1EB;
struct Cyc_List_List*_tmp1EC;_LL1DA: if(*((int*)_tmp1EA)== 1){_LL1DE: _tmp1EB=((
struct Cyc_Tcpat_Neg_struct*)_tmp1EA)->f1;goto _LL1DB;}else{goto _LL1DC;}_LL1DC: if(*((
int*)_tmp1EA)== 0){_LL1DF: _tmp1EC=((struct Cyc_Tcpat_Pos_struct*)_tmp1EA)->f2;
goto _LL1DD;}else{goto _LL1D9;}_LL1DB: {void*any=(void*)({struct Cyc_Tcpat_Neg_struct*
_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF[0]=({struct Cyc_Tcpat_Neg_struct
_tmp1F0;_tmp1F0.tag=1;_tmp1F0.f1=Cyc_Tcpat_empty_con_set();_tmp1F0;});_tmp1EF;});
struct Cyc_List_List*_tmp1ED=0;{int i=0;for(0;i < pcon->arity;++ i){_tmp1ED=({struct
Cyc_List_List*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->hd=(void*)any;_tmp1EE->tl=
_tmp1ED;_tmp1EE;});}}return _tmp1ED;}_LL1DD: return _tmp1EC;_LL1D9:;}static struct
Cyc_List_List*Cyc_Tcpat_getoarg(struct Cyc_List_List*obj,int i){return({struct Cyc_List_List*
_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1->hd=(void*)(i + 1);_tmp1F1->tl=obj;
_tmp1F1;});}static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*
pcon,struct Cyc_List_List*obj){return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*
f)(struct Cyc_List_List*,int),struct Cyc_List_List*env))Cyc_List_tabulate_c)(pcon->arity,
Cyc_Tcpat_getoarg,obj);}static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*
obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){void*_tmp1F2=p;struct Cyc_List_List*
_tmp1F3;struct Cyc_Tcpat_Con_s*_tmp1F4;_LL1E1: if((int)_tmp1F2 == 0){goto _LL1E2;}
else{goto _LL1E3;}_LL1E3: if((unsigned int)_tmp1F2 > 1?*((int*)_tmp1F2)== 0: 0){
_LL1E6: _tmp1F4=((struct Cyc_Tcpat_Con_struct*)_tmp1F2)->f1;goto _LL1E5;_LL1E5:
_tmp1F3=((struct Cyc_Tcpat_Con_struct*)_tmp1F2)->f2;goto _LL1E4;}else{goto _LL1E0;}
_LL1E2: return Cyc_Tcpat_and_match(Cyc_Tcpat_augment(ctx,dsc),work,right_hand_side,
rules);_LL1E4: {void*_tmp1F5=Cyc_Tcpat_static_match(_tmp1F4,dsc);_LL1E8: if((int)
_tmp1F5 == 0){goto _LL1E9;}else{goto _LL1EA;}_LL1EA: if((int)_tmp1F5 == 1){goto _LL1EB;}
else{goto _LL1EC;}_LL1EC: if((int)_tmp1F5 == 2){goto _LL1ED;}else{goto _LL1E7;}_LL1E9: {
struct Cyc_List_List*_tmp1F6=({struct Cyc_List_List*_tmp1FB=_cycalloc(sizeof(*
_tmp1FB));_tmp1FB->hd=({struct _tuple11*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));
_tmp1FC->f1=_tmp1F4;_tmp1FC->f2=0;_tmp1FC;});_tmp1FB->tl=ctx;_tmp1FB;});struct
_tuple10*_tmp1F7=({struct _tuple10*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->f1=
_tmp1F3;_tmp1FA->f2=Cyc_Tcpat_getoargs(_tmp1F4,obj);_tmp1FA->f3=Cyc_Tcpat_getdargs(
_tmp1F4,dsc);_tmp1FA;});struct Cyc_List_List*_tmp1F8=({struct Cyc_List_List*
_tmp1F9=_cycalloc(sizeof(*_tmp1F9));_tmp1F9->hd=_tmp1F7;_tmp1F9->tl=work;_tmp1F9;});
return Cyc_Tcpat_and_match(_tmp1F6,_tmp1F8,right_hand_side,rules);}_LL1EB: return
Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(ctx,dsc,work),rules);_LL1ED: {struct Cyc_List_List*
_tmp1FD=({struct Cyc_List_List*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->hd=({
struct _tuple11*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207->f1=_tmp1F4;_tmp207->f2=
0;_tmp207;});_tmp206->tl=ctx;_tmp206;});struct _tuple10*_tmp1FE=({struct _tuple10*
_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->f1=_tmp1F3;_tmp205->f2=Cyc_Tcpat_getoargs(
_tmp1F4,obj);_tmp205->f3=Cyc_Tcpat_getdargs(_tmp1F4,dsc);_tmp205;});struct Cyc_List_List*
_tmp1FF=({struct Cyc_List_List*_tmp204=_cycalloc(sizeof(*_tmp204));_tmp204->hd=
_tmp1FE;_tmp204->tl=work;_tmp204;});void*_tmp200=Cyc_Tcpat_and_match(_tmp1FD,
_tmp1FF,right_hand_side,rules);void*_tmp201=Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(
ctx,Cyc_Tcpat_add_neg(dsc,_tmp1F4),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*
_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp203;_tmp203.tag=1;_tmp203.f1=obj;_tmp203.f2=_tmp1F4;_tmp203.f3=(void*)
_tmp200;_tmp203.f4=(void*)_tmp201;_tmp203;});_tmp202;});}_LL1E7:;}_LL1E0:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*),void*
env1,void(*rhs_appears)(void*,void*),void*env2){void*_tmp208=d;void*_tmp209;void*
_tmp20A;void*_tmp20B;_LL1EF: if((int)_tmp208 == 0){goto _LL1F0;}else{goto _LL1F1;}
_LL1F1: if((unsigned int)_tmp208 > 1?*((int*)_tmp208)== 0: 0){_LL1F5: _tmp209=(void*)((
struct Cyc_Tcpat_Success_struct*)_tmp208)->f1;goto _LL1F2;}else{goto _LL1F3;}_LL1F3:
if((unsigned int)_tmp208 > 1?*((int*)_tmp208)== 1: 0){_LL1F7: _tmp20B=(void*)((
struct Cyc_Tcpat_IfEq_struct*)_tmp208)->f3;goto _LL1F6;_LL1F6: _tmp20A=(void*)((
struct Cyc_Tcpat_IfEq_struct*)_tmp208)->f4;goto _LL1F4;}else{goto _LL1EE;}_LL1F0:
not_exhaust(env1);goto _LL1EE;_LL1F2: rhs_appears(env2,_tmp209);goto _LL1EE;_LL1F4:
Cyc_Tcpat_check_exhaust_overlap(_tmp20B,not_exhaust,env1,rhs_appears,env2);Cyc_Tcpat_check_exhaust_overlap(
_tmp20A,not_exhaust,env1,rhs_appears,env2);goto _LL1EE;_LL1EE:;}struct _tuple12{
int f1;struct Cyc_Position_Segment*f2;};struct _tuple13{void*f1;struct _tuple12*f2;}
;static struct _tuple13*Cyc_Tcpat_get_match(int*r,struct Cyc_Absyn_Switch_clause*
swc){void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple12*rhs=({struct
_tuple12*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->f1=0;_tmp211->f2=(swc->pattern)->loc;
_tmp211;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*
_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->hd=(void*)sp0;_tmp20C->tl=({struct
Cyc_List_List*_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D->hd=(void*)Cyc_Tcpat_int_pat(*
r);_tmp20D->tl=0;_tmp20D;});_tmp20C;}));*r=*r + 1;}else{sp=Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List*_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E->hd=(void*)sp0;
_tmp20E->tl=({struct Cyc_List_List*_tmp20F=_cycalloc(sizeof(*_tmp20F));_tmp20F->hd=(
void*)((void*)0);_tmp20F->tl=0;_tmp20F;});_tmp20E;}));}return({struct _tuple13*
_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->f1=sp;_tmp210->f2=rhs;_tmp210;});}
static void Cyc_Tcpat_not_exhaust_err(struct Cyc_Position_Segment*loc){({void*
_tmp212[0]={};Cyc_Tcutil_terr(loc,_tag_arr("patterns may not be exhaustive",
sizeof(unsigned char),31),_tag_arr(_tmp212,sizeof(void*),0));});}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple12*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*swcs){int _tmp213=0;struct Cyc_List_List*
_tmp214=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(int*,struct Cyc_Absyn_Switch_clause*),
int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,& _tmp213,swcs);
void*_tmp215=((void*(*)(struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(
_tmp214);((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),struct
Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple12*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp215,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp214 != 0;
_tmp214=_tmp214->tl){struct _tuple13 _tmp217;struct _tuple12*_tmp218;struct _tuple12
_tmp219;struct Cyc_Position_Segment*_tmp21A;int _tmp21B;struct _tuple13*_tmp216=(
struct _tuple13*)_tmp214->hd;_tmp217=*_tmp216;_LL1F9: _tmp218=_tmp217.f2;_tmp219=*
_tmp218;_LL1FB: _tmp21B=_tmp219.f1;goto _LL1FA;_LL1FA: _tmp21A=_tmp219.f2;goto
_LL1F8;_LL1F8: if(! _tmp21B){({void*_tmp21C[0]={};Cyc_Tcutil_terr(_tmp21A,_tag_arr("redundant pattern",
sizeof(unsigned char),18),_tag_arr(_tmp21C,sizeof(void*),0));});}}}struct
_tuple14{struct Cyc_Position_Segment*f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(
struct _tuple14*pr){(*pr).f2=0;({void*_tmp21D[0]={};Cyc_Tcutil_warn((*pr).f1,
_tag_arr("pattern not exhaustive",sizeof(unsigned char),23),_tag_arr(_tmp21D,
sizeof(void*),0));});}static void Cyc_Tcpat_dummy_fn(int i,int j){return;}struct
_tuple15{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Pat*p){struct Cyc_List_List*_tmp21E=({struct Cyc_List_List*
_tmp222=_cycalloc(sizeof(*_tmp222));_tmp222->hd=({struct _tuple15*_tmp223=
_cycalloc(sizeof(*_tmp223));_tmp223->f1=Cyc_Tcpat_compile_pat(p);_tmp223->f2=0;
_tmp223;});_tmp222->tl=0;_tmp222;});void*_tmp21F=((void*(*)(struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp21E);struct _tuple14*_tmp220=({struct
_tuple14*_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221->f1=loc;_tmp221->f2=1;
_tmp221;});((void(*)(void*d,void(*not_exhaust)(struct _tuple14*),struct _tuple14*
env1,void(*rhs_appears)(int,int),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp21F,Cyc_Tcpat_not_exhaust_warn,_tmp220,Cyc_Tcpat_dummy_fn,0);return(*_tmp220).f2;}
static struct _tuple13*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple12*rhs=({struct _tuple12*
_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->f1=0;_tmp225->f2=(swc->pattern)->loc;
_tmp225;});return({struct _tuple13*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->f1=
sp0;_tmp224->f2=rhs;_tmp224;});}static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment*
loc){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,struct
Cyc_List_List*swcs){struct Cyc_List_List*_tmp226=((struct Cyc_List_List*(*)(struct
_tuple13*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp227=((void*(*)(struct Cyc_List_List*allmrules))
Cyc_Tcpat_match_compile)(_tmp226);((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),
struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple12*),int env2))
Cyc_Tcpat_check_exhaust_overlap)(_tmp227,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,
0);for(0;_tmp226 != 0;_tmp226=_tmp226->tl){struct _tuple13 _tmp229;struct _tuple12*
_tmp22A;struct _tuple12 _tmp22B;struct Cyc_Position_Segment*_tmp22C;int _tmp22D;
struct _tuple13*_tmp228=(struct _tuple13*)_tmp226->hd;_tmp229=*_tmp228;_LL1FD:
_tmp22A=_tmp229.f2;_tmp22B=*_tmp22A;_LL1FF: _tmp22D=_tmp22B.f1;goto _LL1FE;_LL1FE:
_tmp22C=_tmp22B.f2;goto _LL1FC;_LL1FC: if(! _tmp22D){({void*_tmp22E[0]={};Cyc_Tcutil_terr(
_tmp22C,_tag_arr("redundant pattern",sizeof(unsigned char),18),_tag_arr(_tmp22E,
sizeof(void*),0));});}}}
