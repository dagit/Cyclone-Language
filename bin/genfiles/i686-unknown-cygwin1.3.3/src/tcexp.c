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
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern int Cyc_List_length(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern
unsigned char Cyc_List_List_mismatch[18];extern struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_rappend(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern unsigned char
Cyc_List_Nth[8];extern void*Cyc_List_nth(struct Cyc_List_List*x,int n);extern struct
Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);extern int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;
struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[9];
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
f3;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;
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
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
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
15];extern struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual();extern struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
extern struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
extern void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_uchar_t;
extern void*Cyc_Absyn_ushort_t;extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_ulonglong_t;
extern void*Cyc_Absyn_schar_t;extern void*Cyc_Absyn_sshort_t;extern void*Cyc_Absyn_sint_t;
extern void*Cyc_Absyn_slonglong_t;extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;
extern void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b);
extern void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);extern void*Cyc_Absyn_pointer_expand(void*);extern
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(
struct Cyc_List_List*,struct _tagged_arr*);extern struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);extern struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info);extern int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);
extern int Cyc_Std_zstrcmp(struct _tagged_arr,struct _tagged_arr);struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern
struct _tagged_arr Cyc_Std_vrprintf(struct _RegionHandle*r1,struct _tagged_arr fmt,
struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);extern
struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);extern struct
_tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Set_Set;extern
unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];struct Cyc_Tcenv_VarRes_struct{int tag;
void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};
struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*
f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*
aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct
Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*
availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};extern void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);extern struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);extern struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);extern struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);extern struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct
Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*);extern void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
extern void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);extern void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*eff);extern void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(struct
_tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern int Cyc_Tcutil_kind_leq(void*k1,
void*k2);extern void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);extern void*Cyc_Tcutil_typ_kind(
void*t);extern void*Cyc_Tcutil_compress(void*t);extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);extern int Cyc_Tcutil_coerceable(void*);
extern int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);extern int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);extern int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);extern int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt
Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*
e);extern void*Cyc_Tcutil_max_arithmetic_type(void*,void*);extern void Cyc_Tcutil_explain_failure();
extern int Cyc_Tcutil_unify(void*,void*);extern void*Cyc_Tcutil_substitute(struct
Cyc_List_List*,void*);extern void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);extern void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
struct _tuple4{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple5{
struct Cyc_Absyn_Tvar*f1;void*f2;};extern struct _tuple5*Cyc_Tcutil_r_make_inst_var(
struct _tuple4*,struct Cyc_Absyn_Tvar*);extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);extern void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
extern void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
extern void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,
struct Cyc_Absyn_Conref*);extern int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*
b);extern struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields);extern int Cyc_Tcutil_is_tagged_pointer_typ(void*);extern int
Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);struct _tuple6{
int f1;void*f2;};extern struct _tuple6 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);extern int Cyc_Tcutil_bits_only(void*t);extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);extern int Cyc_Evexp_okay_szofarg(void*t);extern void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);extern struct Cyc_List_List*
Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);
extern struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr,struct Cyc_Position_Segment*);extern void*Cyc_Tcexp_tcExp(struct
Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);extern void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tagged_arr msg_part);struct
Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct
Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;void*rop;};extern
unsigned char Cyc_CfFlowInfo_IsZero[11];struct Cyc_CfFlowInfo_IsZero_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_CfFlowInfo_NotZero[
12];struct Cyc_CfFlowInfo_NotZero_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
unsigned char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int
tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};extern void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tagged_arr msg,
struct _tagged_arr ap){({void*_tmp0[0]={};Cyc_Tcutil_terr(loc,(struct _tagged_arr)
Cyc_Std_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_arr(_tmp0,sizeof(void*),0));});
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp1=_cycalloc(sizeof(*_tmp1));
_tmp1->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1;}));}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp2;_push_handler(& _tmp2);{int _tmp4=0;if(setjmp(_tmp2.handler)){
_tmp4=1;}if(! _tmp4){{void*_tmp5=Cyc_Tcenv_lookup_ordinary(te,e->loc,q);void*
_tmp6;struct Cyc_Absyn_Enumfield*_tmp7;struct Cyc_Absyn_Enumdecl*_tmp8;struct Cyc_Absyn_Enumfield*
_tmp9;void*_tmpA;struct Cyc_Absyn_Tunionfield*_tmpB;struct Cyc_Absyn_Tuniondecl*
_tmpC;_LL1: if(*((int*)_tmp5)== 0){_LLB: _tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp5)->f1;goto _LL2;}else{goto _LL3;}_LL3: if(*((int*)_tmp5)== 3){_LLD: _tmp8=((
struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;goto _LLC;_LLC: _tmp7=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f2;goto _LL4;}else{goto _LL5;}_LL5: if(*((int*)_tmp5)== 4){_LLF: _tmpA=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;goto _LLE;_LLE: _tmp9=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5)->f2;goto _LL6;}else{goto _LL7;}_LL7: if(*((int*)_tmp5)== 2){_LL11: _tmpC=((
struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f1;goto _LL10;_LL10: _tmpB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f2;goto _LL8;}else{goto _LL9;}_LL9: if(*((int*)_tmp5)== 1){goto _LLA;}else{
goto _LL0;}_LL2:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmpD=
_cycalloc(sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Absyn_Var_e_struct _tmpE;_tmpE.tag=
1;_tmpE.f1=q;_tmpE.f2=(void*)_tmp6;_tmpE;});_tmpD;})));goto _LL0;_LL4:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Enum_e_struct*_tmpF=_cycalloc(sizeof(*_tmpF));
_tmpF[0]=({struct Cyc_Absyn_Enum_e_struct _tmp10;_tmp10.tag=31;_tmp10.f1=q;_tmp10.f2=(
struct Cyc_Absyn_Enumdecl*)_tmp8;_tmp10.f3=(struct Cyc_Absyn_Enumfield*)_tmp7;
_tmp10;});_tmpF;})));goto _LL0;_LL6:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_e_struct*
_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[0]=({struct Cyc_Absyn_AnonEnum_e_struct
_tmp12;_tmp12.tag=32;_tmp12.f1=q;_tmp12.f2=(void*)_tmpA;_tmp12.f3=(struct Cyc_Absyn_Enumfield*)
_tmp9;_tmp12;});_tmp11;})));goto _LL0;_LL8:(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_Tunion_e_struct*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=({struct
Cyc_Absyn_Tunion_e_struct _tmp14;_tmp14.tag=30;_tmp14.f1=0;_tmp14.f2=_tmpC;_tmp14.f3=
_tmpB;_tmp14;});_tmp13;})));goto _LL0;_LLA:({struct Cyc_Std_String_pa_struct _tmp16;
_tmp16.tag=0;_tmp16.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(q);{void*
_tmp15[1]={& _tmp16};Cyc_Tcutil_terr(e->loc,_tag_arr("bad occurrence of type name %s",
sizeof(unsigned char),31),_tag_arr(_tmp15,sizeof(void*),1));}});(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp17=_cycalloc(sizeof(*_tmp17));
_tmp17[0]=({struct Cyc_Absyn_Var_e_struct _tmp18;_tmp18.tag=1;_tmp18.f1=q;_tmp18.f2=(
void*)((void*)0);_tmp18;});_tmp17;})));goto _LL0;_LL0:;};_pop_handler();}else{
void*_tmp3=(void*)_exn_thrown;void*_tmp1A=_tmp3;_LL13: if(_tmp1A == Cyc_Dict_Absent){
goto _LL14;}else{goto _LL15;}_LL15: goto _LL16;_LL14:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Var_e_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({
struct Cyc_Absyn_Var_e_struct _tmp1C;_tmp1C.tag=1;_tmp1C.f1=q;_tmp1C.f2=(void*)((
void*)0);_tmp1C;});_tmp1B;})));goto _LL12;_LL16:(void)_throw(_tmp1A);_LL12:;}}}
struct _tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple7*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){return({struct _tuple7*
_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->f1=0;_tmp1D->f2=e;_tmp1D;});}static
void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1E=(void*)e1->r;struct
_tuple1*_tmp1F;_LL18: if(*((int*)_tmp1E)== 2){_LL1C: _tmp1F=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1E)->f1;goto _LL19;}else{goto _LL1A;}_LL1A: goto _LL1B;_LL19: {struct
_handler_cons _tmp20;_push_handler(& _tmp20);{int _tmp22=0;if(setjmp(_tmp20.handler)){
_tmp22=1;}if(! _tmp22){{void*_tmp23=Cyc_Tcenv_lookup_ordinary(te,e1->loc,_tmp1F);
void*_tmp24;struct Cyc_Absyn_Tunionfield*_tmp25;struct Cyc_Absyn_Tuniondecl*_tmp26;
struct Cyc_Absyn_Aggrdecl*_tmp27;_LL1E: if(*((int*)_tmp23)== 0){_LL28: _tmp24=(void*)((
struct Cyc_Tcenv_VarRes_struct*)_tmp23)->f1;goto _LL1F;}else{goto _LL20;}_LL20: if(*((
int*)_tmp23)== 2){_LL2A: _tmp26=((struct Cyc_Tcenv_TunionRes_struct*)_tmp23)->f1;
goto _LL29;_LL29: _tmp25=((struct Cyc_Tcenv_TunionRes_struct*)_tmp23)->f2;goto _LL21;}
else{goto _LL22;}_LL22: if(*((int*)_tmp23)== 1){_LL2B: _tmp27=((struct Cyc_Tcenv_AggrRes_struct*)
_tmp23)->f1;goto _LL23;}else{goto _LL24;}_LL24: if(*((int*)_tmp23)== 4){goto _LL25;}
else{goto _LL26;}_LL26: if(*((int*)_tmp23)== 3){goto _LL27;}else{goto _LL1D;}_LL1F:(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp28=_cycalloc(
sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp29;_tmp29.tag=9;
_tmp29.f1=e1;_tmp29.f2=es;_tmp29.f3=0;_tmp29;});_tmp28;})));_npop_handler(0);
return;_LL21: if(_tmp25->typs == 0){({struct Cyc_Std_String_pa_struct _tmp2B;_tmp2B.tag=
0;_tmp2B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp25->name);{void*
_tmp2A[1]={& _tmp2B};Cyc_Tcutil_terr(e->loc,_tag_arr("%s is a constant, not a function",
sizeof(unsigned char),33),_tag_arr(_tmp2A,sizeof(void*),1));}});}(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));
_tmp2C[0]=({struct Cyc_Absyn_Tunion_e_struct _tmp2D;_tmp2D.tag=30;_tmp2D.f1=es;
_tmp2D.f2=_tmp26;_tmp2D.f3=_tmp25;_tmp2D;});_tmp2C;})));_npop_handler(0);return;
_npop_handler(0);return;_LL23: {struct Cyc_List_List*_tmp2E=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcexp_make_struct_arg,es);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp30;_tmp30.tag=28;_tmp30.f1=_tmp27->name;_tmp30.f2=_tmp2E;_tmp30.f3=(struct
Cyc_Absyn_Aggrdecl*)_tmp27;_tmp30;});_tmp2F;})));_npop_handler(0);return;}_LL25:
goto _LL27;_LL27:({struct Cyc_Std_String_pa_struct _tmp32;_tmp32.tag=0;_tmp32.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1F);{void*_tmp31[1]={& _tmp32};Cyc_Tcutil_terr(
e->loc,_tag_arr("%s is an enum constructor, not a function",sizeof(unsigned char),
42),_tag_arr(_tmp31,sizeof(void*),1));}});_npop_handler(0);return;_LL1D:;};
_pop_handler();}else{void*_tmp21=(void*)_exn_thrown;void*_tmp34=_tmp21;_LL2D: if(
_tmp34 == Cyc_Dict_Absent){goto _LL2E;}else{goto _LL2F;}_LL2F: goto _LL30;_LL2E:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp35=_cycalloc(sizeof(*
_tmp35));_tmp35[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp36;_tmp36.tag=9;_tmp36.f1=
e1;_tmp36.f2=es;_tmp36.f3=0;_tmp36;});_tmp35;})));return;_LL30:(void)_throw(
_tmp34);_LL2C:;}}}_LL1B:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp38;_tmp38.tag=9;_tmp38.f1=e1;_tmp38.f2=es;_tmp38.f3=0;_tmp38;});_tmp37;})));
return;_LL17:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3A;_tmp3A.tag=26;_tmp3A.f1=des;_tmp3A;});_tmp39;})));return;}{void*t=*topt;
void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp3C;void*_tmp3D;
struct Cyc_Absyn_Tqual _tmp3E;void*_tmp3F;_LL32: if((unsigned int)_tmp3B > 3?*((int*)
_tmp3B)== 10: 0){_LL3A: _tmp3C=((struct Cyc_Absyn_AggrType_struct*)_tmp3B)->f1;
_LL3B: _tmp3D=(void*)_tmp3C.aggr_info;goto _LL33;}else{goto _LL34;}_LL34: if((
unsigned int)_tmp3B > 3?*((int*)_tmp3B)== 7: 0){_LL3D: _tmp3F=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp3B)->f1;goto _LL3C;_LL3C: _tmp3E=((struct Cyc_Absyn_ArrayType_struct*)_tmp3B)->f2;
goto _LL35;}else{goto _LL36;}_LL36: if((unsigned int)_tmp3B > 3?*((int*)_tmp3B)== 11:
0){goto _LL37;}else{goto _LL38;}_LL38: goto _LL39;_LL33:{void*_tmp40=_tmp3D;struct
Cyc_Absyn_Aggrdecl**_tmp41;struct Cyc_Absyn_Aggrdecl*_tmp42;_LL3F: if(*((int*)
_tmp40)== 0){goto _LL40;}else{goto _LL41;}_LL41: if(*((int*)_tmp40)== 1){_LL43:
_tmp41=((struct Cyc_Absyn_KnownAggr_struct*)_tmp40)->f1;_tmp42=*_tmp41;goto _LL42;}
else{goto _LL3E;}_LL40:({void*_tmp43[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("struct type not properly set",sizeof(
unsigned char),29),_tag_arr(_tmp43,sizeof(void*),0));});_LL42:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[
0]=({struct Cyc_Absyn_Struct_e_struct _tmp45;_tmp45.tag=28;_tmp45.f1=_tmp42->name;
_tmp45.f2=des;_tmp45.f3=(struct Cyc_Absyn_Aggrdecl*)_tmp42;_tmp45;});_tmp44;})));
_LL3E:;}goto _LL31;_LL35:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Absyn_Array_e_struct
_tmp47;_tmp47.tag=26;_tmp47.f1=des;_tmp47;});_tmp46;})));goto _LL31;_LL37:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*_tmp48=_cycalloc(
sizeof(*_tmp48));_tmp48[0]=({struct Cyc_Absyn_AnonStruct_e_struct _tmp49;_tmp49.tag=
29;_tmp49.f1=(void*)t;_tmp49.f2=des;_tmp49;});_tmp48;})));goto _LL31;_LL39:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp4A=_cycalloc(sizeof(*
_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_Array_e_struct _tmp4B;_tmp4B.tag=26;_tmp4B.f1=
des;_tmp4B;});_tmp4A;})));goto _LL31;_LL31:;}}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _tagged_arr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_t,e);if(! Cyc_Tcutil_coerce_to_bool(
te,e)){({struct Cyc_Std_String_pa_struct _tmp4E;_tmp4E.tag=0;_tmp4E.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
struct Cyc_Std_String_pa_struct _tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _tagged_arr)
msg_part;{void*_tmp4C[2]={& _tmp4D,& _tmp4E};Cyc_Tcutil_terr(e->loc,_tag_arr("test of %s has type %s instead of integral or * type",
sizeof(unsigned char),53),_tag_arr(_tmp4C,sizeof(void*),2));}}});}}static void*
Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,void*c,struct Cyc_Absyn_Exp*e){void*t;{void*_tmp4F=c;void*_tmp50;void*_tmp51;
void*_tmp52;int _tmp53;void*_tmp54;struct _tagged_arr _tmp55;_LL45: if((unsigned int)
_tmp4F > 1?*((int*)_tmp4F)== 0: 0){_LL53: _tmp50=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp4F)->f1;goto _LL46;}else{goto _LL47;}_LL47: if((unsigned int)_tmp4F > 1?*((int*)
_tmp4F)== 1: 0){_LL54: _tmp51=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp4F)->f1;
goto _LL48;}else{goto _LL49;}_LL49: if((unsigned int)_tmp4F > 1?*((int*)_tmp4F)== 3:
0){_LL55: _tmp52=(void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp4F)->f1;goto
_LL4A;}else{goto _LL4B;}_LL4B: if((unsigned int)_tmp4F > 1?*((int*)_tmp4F)== 4: 0){
goto _LL4C;}else{goto _LL4D;}_LL4D: if((unsigned int)_tmp4F > 1?*((int*)_tmp4F)== 2:
0){_LL57: _tmp54=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp4F)->f1;goto _LL56;
_LL56: _tmp53=((struct Cyc_Absyn_Int_c_struct*)_tmp4F)->f2;goto _LL4E;}else{goto
_LL4F;}_LL4F: if((unsigned int)_tmp4F > 1?*((int*)_tmp4F)== 5: 0){_LL58: _tmp55=((
struct Cyc_Absyn_String_c_struct*)_tmp4F)->f1;goto _LL50;}else{goto _LL51;}_LL51:
if((int)_tmp4F == 0){goto _LL52;}else{goto _LL44;}_LL46: t=_tmp50 == (void*)1? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t;goto _LL44;_LL48: t=_tmp51 == (void*)1? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL44;_LL4A: t=_tmp52 == (void*)1? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t;
goto _LL44;_LL4C: t=Cyc_Absyn_float_typ;goto _LL44;_LL4E: if(topt == 0){t=_tmp54 == (
void*)1? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}else{void*_tmp56=Cyc_Tcutil_compress(*
topt);void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;_LL5A: if((
unsigned int)_tmp56 > 3?*((int*)_tmp56)== 5: 0){_LL65: _tmp58=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp56)->f1;goto _LL64;_LL64: _tmp57=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp56)->f2;if((int)_tmp57 == 0){goto _LL5B;}else{goto _LL5C;}}else{goto _LL5C;}
_LL5C: if((unsigned int)_tmp56 > 3?*((int*)_tmp56)== 5: 0){_LL67: _tmp5A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp56)->f1;goto _LL66;_LL66: _tmp59=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp56)->f2;if((int)_tmp59 == 1){goto _LL5D;}else{
goto _LL5E;}}else{goto _LL5E;}_LL5E: if((unsigned int)_tmp56 > 3?*((int*)_tmp56)== 5:
0){_LL68: _tmp5B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56)->f1;goto _LL5F;}
else{goto _LL60;}_LL60: if((unsigned int)_tmp56 > 3?*((int*)_tmp56)== 4: 0){goto
_LL69;}else{goto _LL62;}_LL69: if(_tmp53 == 0){goto _LL61;}else{goto _LL62;}_LL62:
goto _LL63;_LL5B: t=_tmp58 == (void*)1? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp5C=_cycalloc(sizeof(*
_tmp5C));_tmp5C[0]=({struct Cyc_Absyn_Const_e_struct _tmp5D;_tmp5D.tag=0;_tmp5D.f1=(
void*)((void*)({struct Cyc_Absyn_Char_c_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));
_tmp5E[0]=({struct Cyc_Absyn_Char_c_struct _tmp5F;_tmp5F.tag=0;_tmp5F.f1=(void*)
_tmp58;_tmp5F.f2=(unsigned char)_tmp53;_tmp5F;});_tmp5E;}));_tmp5D;});_tmp5C;})));
goto _LL59;_LL5D: t=_tmp5A == (void*)1? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp60=_cycalloc(sizeof(*
_tmp60));_tmp60[0]=({struct Cyc_Absyn_Const_e_struct _tmp61;_tmp61.tag=0;_tmp61.f1=(
void*)((void*)({struct Cyc_Absyn_Short_c_struct*_tmp62=_cycalloc(sizeof(*_tmp62));
_tmp62[0]=({struct Cyc_Absyn_Short_c_struct _tmp63;_tmp63.tag=1;_tmp63.f1=(void*)
_tmp5A;_tmp63.f2=(short)_tmp53;_tmp63;});_tmp62;}));_tmp61;});_tmp60;})));goto
_LL59;_LL5F: t=_tmp5B == (void*)1? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;goto _LL59;
_LL61:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp64=
_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_Const_e_struct _tmp65;
_tmp65.tag=0;_tmp65.f1=(void*)((void*)0);_tmp65;});_tmp64;})));{struct Cyc_List_List*
_tmp66=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67[0]=({struct Cyc_Absyn_PointerType_struct
_tmp68;_tmp68.tag=4;_tmp68.f1=({struct Cyc_Absyn_PtrInfo _tmp69;_tmp69.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->v=_tmp66;_tmp6B;}));_tmp69.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->v=_tmp66;_tmp6A;}));_tmp69.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp69.tq=Cyc_Absyn_empty_tqual();
_tmp69.bounds=Cyc_Absyn_empty_conref();_tmp69;});_tmp68;});_tmp67;});goto _LL59;}
_LL63: t=_tmp54 == (void*)1? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;goto _LL59;_LL59:;}
goto _LL44;_LL50: {int len=(int)_get_arr_size(_tmp55,sizeof(unsigned char));struct
Cyc_Absyn_Const_e_struct*_tmp6C=({struct Cyc_Absyn_Const_e_struct*_tmp81=
_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_Const_e_struct _tmp82;
_tmp82.tag=0;_tmp82.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp83=
_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_Int_c_struct _tmp84;_tmp84.tag=
2;_tmp84.f1=(void*)((void*)1);_tmp84.f2=len;_tmp84;});_tmp83;}));_tmp82;});
_tmp81;});struct Cyc_Absyn_Exp*elen=Cyc_Absyn_new_exp((void*)_tmp6C,loc);elen->topt=({
struct Cyc_Core_Opt*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->v=(void*)Cyc_Absyn_uint_t;
_tmp6D;});t=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,(void*)2,Cyc_Absyn_const_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[
0]=({struct Cyc_Absyn_Upper_b_struct _tmp6F;_tmp6F.tag=0;_tmp6F.f1=elen;_tmp6F;});
_tmp6E;}));if(topt != 0){void*_tmp70=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Exp*
_tmp71;struct Cyc_Absyn_Tqual _tmp72;void*_tmp73;struct Cyc_Absyn_PtrInfo _tmp74;
struct Cyc_Absyn_Conref*_tmp75;struct Cyc_Absyn_Tqual _tmp76;struct Cyc_Absyn_Conref*
_tmp77;void*_tmp78;void*_tmp79;_LL6B: if((unsigned int)_tmp70 > 3?*((int*)_tmp70)
== 7: 0){_LL73: _tmp73=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp70)->f1;goto
_LL72;_LL72: _tmp72=((struct Cyc_Absyn_ArrayType_struct*)_tmp70)->f2;goto _LL71;
_LL71: _tmp71=((struct Cyc_Absyn_ArrayType_struct*)_tmp70)->f3;goto _LL6C;}else{
goto _LL6D;}_LL6D: if((unsigned int)_tmp70 > 3?*((int*)_tmp70)== 4: 0){_LL74: _tmp74=((
struct Cyc_Absyn_PointerType_struct*)_tmp70)->f1;_LL79: _tmp79=(void*)_tmp74.elt_typ;
goto _LL78;_LL78: _tmp78=(void*)_tmp74.rgn_typ;goto _LL77;_LL77: _tmp77=_tmp74.nullable;
goto _LL76;_LL76: _tmp76=_tmp74.tq;goto _LL75;_LL75: _tmp75=_tmp74.bounds;goto _LL6E;}
else{goto _LL6F;}_LL6F: goto _LL70;_LL6C: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp7B;_tmp7B.tag=7;_tmp7B.f1=(void*)Cyc_Absyn_uchar_t;_tmp7B.f2=_tmp72;_tmp7B.f3=(
struct Cyc_Absyn_Exp*)elen;_tmp7B;});_tmp7A;});_LL6E: if(! Cyc_Tcutil_unify(*topt,t)?
Cyc_Tcutil_silent_castable(te,loc,t,*topt): 0){e->topt=({struct Cyc_Core_Opt*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->v=(void*)t;_tmp7C;});Cyc_Tcutil_unchecked_cast(
te,e,*topt);t=*topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp7D=_cycalloc(sizeof(*
_tmp7D));_tmp7D->v=Cyc_Tcenv_lookup_type_vars(te);_tmp7D;})),Cyc_Absyn_const_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[
0]=({struct Cyc_Absyn_Upper_b_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=elen;_tmp7F;});
_tmp7E;}));if(! Cyc_Tcutil_unify(*topt,t)? Cyc_Tcutil_silent_castable(te,loc,t,*
topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->v=(
void*)t;_tmp80;});Cyc_Tcutil_unchecked_cast(te,e,*topt);t=*topt;}}goto _LL6A;
_LL70: goto _LL6A;_LL6A:;}return t;}_LL52: {struct Cyc_List_List*_tmp85=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp86=_cycalloc(sizeof(*
_tmp86));_tmp86[0]=({struct Cyc_Absyn_PointerType_struct _tmp87;_tmp87.tag=4;
_tmp87.f1=({struct Cyc_Absyn_PtrInfo _tmp88;_tmp88.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp8A=_cycalloc(sizeof(*
_tmp8A));_tmp8A->v=_tmp85;_tmp8A;}));_tmp88.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp89=_cycalloc(sizeof(*
_tmp89));_tmp89->v=_tmp85;_tmp89;}));_tmp88.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmp88.tq=Cyc_Absyn_empty_tqual();_tmp88.bounds=
Cyc_Absyn_empty_conref();_tmp88;});_tmp87;});_tmp86;});goto _LL44;}_LL44:;}return
t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp8B=b;struct Cyc_Absyn_Vardecl*_tmp8C;struct
Cyc_Absyn_Fndecl*_tmp8D;struct Cyc_Absyn_Vardecl*_tmp8E;struct Cyc_Absyn_Vardecl*
_tmp8F;struct Cyc_Absyn_Vardecl*_tmp90;_LL7B: if((int)_tmp8B == 0){goto _LL7C;}else{
goto _LL7D;}_LL7D: if((unsigned int)_tmp8B > 1?*((int*)_tmp8B)== 0: 0){_LL87: _tmp8C=((
struct Cyc_Absyn_Global_b_struct*)_tmp8B)->f1;goto _LL7E;}else{goto _LL7F;}_LL7F:
if((unsigned int)_tmp8B > 1?*((int*)_tmp8B)== 1: 0){_LL88: _tmp8D=((struct Cyc_Absyn_Funname_b_struct*)
_tmp8B)->f1;goto _LL80;}else{goto _LL81;}_LL81: if((unsigned int)_tmp8B > 1?*((int*)
_tmp8B)== 4: 0){_LL89: _tmp8E=((struct Cyc_Absyn_Pat_b_struct*)_tmp8B)->f1;goto
_LL82;}else{goto _LL83;}_LL83: if((unsigned int)_tmp8B > 1?*((int*)_tmp8B)== 3: 0){
_LL8A: _tmp8F=((struct Cyc_Absyn_Local_b_struct*)_tmp8B)->f1;goto _LL84;}else{goto
_LL85;}_LL85: if((unsigned int)_tmp8B > 1?*((int*)_tmp8B)== 2: 0){_LL8B: _tmp90=((
struct Cyc_Absyn_Param_b_struct*)_tmp8B)->f1;goto _LL86;}else{goto _LL7A;}_LL7C:
return({struct Cyc_Std_String_pa_struct _tmp92;_tmp92.tag=0;_tmp92.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(q);{void*_tmp91[1]={& _tmp92};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("undeclared identifier: %s",sizeof(unsigned char),26),_tag_arr(
_tmp91,sizeof(void*),1));}});_LL7E: q[0]=(_tmp8C->name)[
_check_known_subscript_notnull(1,0)];return(void*)_tmp8C->type;_LL80: q[0]=(
_tmp8D->name)[_check_known_subscript_notnull(1,0)];return Cyc_Tcutil_fndecl2typ(
_tmp8D);_LL82: _tmp8F=_tmp8E;goto _LL84;_LL84: _tmp90=_tmp8F;goto _LL86;_LL86:(*q).f1=(
void*)0;return(void*)_tmp90->type;_LL7A:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _tagged_arr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmp93=(void*)
fmt->r;void*_tmp94;struct _tagged_arr _tmp95;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp
_tmp97;void*_tmp98;void*_tmp99;struct _tagged_arr _tmp9A;_LL8D: if(*((int*)_tmp93)
== 0){_LL93: _tmp94=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp93)->f1;if((
unsigned int)_tmp94 > 1?*((int*)_tmp94)== 5: 0){_LL94: _tmp95=((struct Cyc_Absyn_String_c_struct*)
_tmp94)->f1;goto _LL8E;}else{goto _LL8F;}}else{goto _LL8F;}_LL8F: if(*((int*)_tmp93)
== 13){_LL95: _tmp96=((struct Cyc_Absyn_Cast_e_struct*)_tmp93)->f2;_tmp97=*_tmp96;
_LL96: _tmp98=(void*)_tmp97.r;if(*((int*)_tmp98)== 0){_LL97: _tmp99=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp98)->f1;if((unsigned int)_tmp99 > 1?*((int*)
_tmp99)== 5: 0){_LL98: _tmp9A=((struct Cyc_Absyn_String_c_struct*)_tmp99)->f1;goto
_LL90;}else{goto _LL91;}}else{goto _LL91;}}else{goto _LL91;}_LL91: goto _LL92;_LL8E:
_tmp9A=_tmp95;goto _LL90;_LL90: desc_types=type_getter(te,(struct _tagged_arr)
_tmp9A,fmt->loc);goto _LL8C;_LL92: return;_LL8C:;}if(opt_args != 0){struct Cyc_List_List*
_tmp9B=(struct Cyc_List_List*)opt_args->v;for(0;desc_types != 0? _tmp9B != 0: 0;(
desc_types=desc_types->tl,_tmp9B=_tmp9B->tl)){void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp9B->hd;if(! Cyc_Tcutil_coerce_arg(
te,e,t)){({struct Cyc_Std_String_pa_struct _tmp9E;_tmp9E.tag=0;_tmp9E.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
struct Cyc_Std_String_pa_struct _tmp9D;_tmp9D.tag=0;_tmp9D.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t);{void*_tmp9C[2]={& _tmp9D,& _tmp9E};Cyc_Tcutil_terr(e->loc,
_tag_arr("descriptor has type \n%s\n but argument has type \n%s",sizeof(
unsigned char),51),_tag_arr(_tmp9C,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
if(desc_types != 0){({void*_tmp9F[0]={};Cyc_Tcutil_terr(fmt->loc,_tag_arr("too few arguments",
sizeof(unsigned char),18),_tag_arr(_tmp9F,sizeof(void*),0));});}if(_tmp9B != 0){({
void*_tmpA0[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp9B->hd)->loc,
_tag_arr("too many arguments",sizeof(unsigned char),19),_tag_arr(_tmpA0,sizeof(
void*),0));});}}}static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmpA1=p;_LL9A: if((
int)_tmpA1 == 0){goto _LL9B;}else{goto _LL9C;}_LL9C: if((int)_tmpA1 == 2){goto _LL9D;}
else{goto _LL9E;}_LL9E: if((int)_tmpA1 == 11){goto _LL9F;}else{goto _LLA0;}_LLA0: if((
int)_tmpA1 == 12){goto _LLA1;}else{goto _LLA2;}_LLA2: if((int)_tmpA1 == 19){goto _LLA3;}
else{goto _LLA4;}_LLA4: goto _LLA5;_LL9B: goto _LL9D;_LL9D: if(! Cyc_Tcutil_is_numeric(
e)){({struct Cyc_Std_String_pa_struct _tmpA3;_tmpA3.tag=0;_tmpA3.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpA2[1]={& _tmpA3};Cyc_Tcutil_terr(
loc,_tag_arr("expecting arithmetic type but found %s",sizeof(unsigned char),39),
_tag_arr(_tmpA2,sizeof(void*),1));}});}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL9F: Cyc_Tcutil_check_contains_assign(e);if(! Cyc_Tcutil_coerce_to_bool(
te,e)){({struct Cyc_Std_String_pa_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpA4[1]={& _tmpA5};Cyc_Tcutil_terr(
loc,_tag_arr("expecting integral or * type but found %s",sizeof(unsigned char),
42),_tag_arr(_tmpA4,sizeof(void*),1));}});}return Cyc_Absyn_sint_t;_LLA1: if(! Cyc_Tcutil_is_integral(
e)){({struct Cyc_Std_String_pa_struct _tmpA7;_tmpA7.tag=0;_tmpA7.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpA6[1]={& _tmpA7};Cyc_Tcutil_terr(
loc,_tag_arr("expecting integral type but found %s",sizeof(unsigned char),37),
_tag_arr(_tmpA6,sizeof(void*),1));}});}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LLA3:{void*_tmpA8=t;struct Cyc_Absyn_PtrInfo _tmpA9;
_LLA7: if((unsigned int)_tmpA8 > 3?*((int*)_tmpA8)== 7: 0){goto _LLA8;}else{goto
_LLA9;}_LLA9: if((unsigned int)_tmpA8 > 3?*((int*)_tmpA8)== 4: 0){_LLAD: _tmpA9=((
struct Cyc_Absyn_PointerType_struct*)_tmpA8)->f1;goto _LLAA;}else{goto _LLAB;}_LLAB:
goto _LLAC;_LLA8: goto _LLAA;_LLAA: goto _LLA6;_LLAC:({struct Cyc_Std_String_pa_struct
_tmpAB;_tmpAB.tag=0;_tmpAB.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpAA[1]={& _tmpAB};Cyc_Tcutil_terr(loc,_tag_arr("size requires pointer or array type, not %s",
sizeof(unsigned char),44),_tag_arr(_tmpAA,sizeof(void*),1));}});goto _LLA6;_LLA6:;}
return Cyc_Absyn_uint_t;_LLA5:({void*_tmpAC[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Non-unary primop",sizeof(
unsigned char),17),_tag_arr(_tmpAC,sizeof(void*),0));});_LL99:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(! checker(e1)){({struct Cyc_Std_String_pa_struct
_tmpAE;_tmpAE.tag=0;_tmpAE.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{void*_tmpAD[1]={& _tmpAE};Cyc_Tcutil_terr(
e1->loc,_tag_arr("type %s cannot be used here",sizeof(unsigned char),28),
_tag_arr(_tmpAD,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpAF;}));}if(! checker(e2)){({struct Cyc_Std_String_pa_struct _tmpB1;_tmpB1.tag=0;
_tmpB1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{void*_tmpB0[1]={& _tmpB1};Cyc_Tcutil_terr(e2->loc,
_tag_arr("type %s cannot be used here",sizeof(unsigned char),28),_tag_arr(_tmpB0,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpB2=
_cycalloc(sizeof(*_tmpB2));_tmpB2->v=Cyc_Tcenv_lookup_type_vars(te);_tmpB2;}));}{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpB3=t1;struct Cyc_Absyn_PtrInfo
_tmpB4;struct Cyc_Absyn_Conref*_tmpB5;struct Cyc_Absyn_Tqual _tmpB6;struct Cyc_Absyn_Conref*
_tmpB7;void*_tmpB8;void*_tmpB9;_LLAF: if((unsigned int)_tmpB3 > 3?*((int*)_tmpB3)
== 4: 0){_LLB3: _tmpB4=((struct Cyc_Absyn_PointerType_struct*)_tmpB3)->f1;_LLB8:
_tmpB9=(void*)_tmpB4.elt_typ;goto _LLB7;_LLB7: _tmpB8=(void*)_tmpB4.rgn_typ;goto
_LLB6;_LLB6: _tmpB7=_tmpB4.nullable;goto _LLB5;_LLB5: _tmpB6=_tmpB4.tq;goto _LLB4;
_LLB4: _tmpB5=_tmpB4.bounds;goto _LLB0;}else{goto _LLB1;}_LLB1: goto _LLB2;_LLB0: if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpB9),(void*)1)){({void*_tmpBA[0]={};
Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(unsigned char),50),_tag_arr(_tmpBA,sizeof(void*),0));});}if(! Cyc_Tcutil_coerce_sint_typ(
te,e2)){({struct Cyc_Std_String_pa_struct _tmpBC;_tmpBC.tag=0;_tmpBC.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmpBB[1]={& _tmpBC};Cyc_Tcutil_terr(
e2->loc,_tag_arr("expecting int but found %s",sizeof(unsigned char),27),_tag_arr(
_tmpBB,sizeof(void*),1));}});}_tmpB5=Cyc_Absyn_compress_conref(_tmpB5);{void*
_tmpBD=(void*)_tmpB5->v;void*_tmpBE;void*_tmpBF;struct Cyc_Absyn_Exp*_tmpC0;_LLBA:
if((unsigned int)_tmpBD > 1?*((int*)_tmpBD)== 0: 0){_LLC0: _tmpBE=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmpBD)->f1;if((int)_tmpBE == 0){goto _LLBB;}else{goto
_LLBC;}}else{goto _LLBC;}_LLBC: if((unsigned int)_tmpBD > 1?*((int*)_tmpBD)== 0: 0){
_LLC1: _tmpBF=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpBD)->f1;if((
unsigned int)_tmpBF > 1?*((int*)_tmpBF)== 0: 0){_LLC2: _tmpC0=((struct Cyc_Absyn_Upper_b_struct*)
_tmpBF)->f1;goto _LLBD;}else{goto _LLBE;}}else{goto _LLBE;}_LLBE: goto _LLBF;_LLBB:
return t1;_LLBD: {struct Cyc_Absyn_PointerType_struct*_tmpC1=({struct Cyc_Absyn_PointerType_struct*
_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_PointerType_struct
_tmpC3;_tmpC3.tag=4;_tmpC3.f1=({struct Cyc_Absyn_PtrInfo _tmpC4;_tmpC4.elt_typ=(
void*)_tmpB9;_tmpC4.rgn_typ=(void*)_tmpB8;_tmpC4.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmpC4.tq=_tmpB6;_tmpC4.bounds=Cyc_Absyn_new_conref((
void*)0);_tmpC4;});_tmpC3;});_tmpC2;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)
_tmpC1);return(void*)_tmpC1;}_LLBF:(void*)(_tmpB5->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmpC6;_tmpC6.tag=0;_tmpC6.f1=(void*)((void*)0);_tmpC6;});_tmpC5;})));return t1;
_LLB9:;}_LLB2: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLAE:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)
0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(! Cyc_Tcutil_unify(t1,t2)){({struct Cyc_Std_String_pa_struct _tmpC9;_tmpC9.tag=
0;_tmpC9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{struct Cyc_Std_String_pa_struct _tmpC8;_tmpC8.tag=0;
_tmpC8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);{void*_tmpC7[2]={& _tmpC8,& _tmpC9};Cyc_Tcutil_terr(e1->loc,
_tag_arr("pointer arithmetic on values of different types (%s != %s)",sizeof(
unsigned char),59),_tag_arr(_tmpC7,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_t;}else{if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),(
void*)1)){({void*_tmpCA[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(unsigned char),50),_tag_arr(_tmpCA,sizeof(void*),0));});}if(! Cyc_Tcutil_coerce_sint_typ(
te,e2)){({struct Cyc_Std_String_pa_struct _tmpCD;_tmpCD.tag=0;_tmpCD.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmpCC;
_tmpCC.tag=0;_tmpCC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmpCB[2]={& _tmpCC,& _tmpCD};Cyc_Tcutil_terr(e2->loc,_tag_arr("expecting either %s or int but found %s",
sizeof(unsigned char),40),_tag_arr(_tmpCB,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t1;}}else{return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num? e2_is_num: 0){return
Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_unify(t1,t2)? Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({struct Cyc_Core_Opt*_tmpCE=_cycalloc(sizeof(*
_tmpCE));_tmpCE->v=Cyc_Tcenv_lookup_type_vars(te);_tmpCE;}))): 0){return Cyc_Absyn_sint_t;}
else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,
e2,t1);return Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2);return Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e1)? 1: Cyc_Tcutil_zero_to_null(te,t1,e2)){return Cyc_Absyn_sint_t;}else{{
struct _tuple0 _tmpD0=({struct _tuple0 _tmpCF;_tmpCF.f1=Cyc_Tcutil_compress(t1);
_tmpCF.f2=Cyc_Tcutil_compress(t2);_tmpCF;});void*_tmpD1;struct Cyc_Absyn_PtrInfo
_tmpD2;void*_tmpD3;void*_tmpD4;struct Cyc_Absyn_PtrInfo _tmpD5;void*_tmpD6;_LLC4:
_LLCB: _tmpD4=_tmpD0.f1;if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 4: 0){_LLCC:
_tmpD5=((struct Cyc_Absyn_PointerType_struct*)_tmpD4)->f1;_LLCD: _tmpD6=(void*)
_tmpD5.elt_typ;goto _LLC8;}else{goto _LLC6;}_LLC8: _tmpD1=_tmpD0.f2;if((
unsigned int)_tmpD1 > 3?*((int*)_tmpD1)== 4: 0){_LLC9: _tmpD2=((struct Cyc_Absyn_PointerType_struct*)
_tmpD1)->f1;_LLCA: _tmpD3=(void*)_tmpD2.elt_typ;goto _LLC5;}else{goto _LLC6;}_LLC6:
goto _LLC7;_LLC5: if(Cyc_Tcutil_unify(_tmpD6,_tmpD3)){return Cyc_Absyn_sint_t;}goto
_LLC3;_LLC7: goto _LLC3;_LLC3:;}({struct Cyc_Std_String_pa_struct _tmpD9;_tmpD9.tag=
0;_tmpD9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct
_tmpD8;_tmpD8.tag=0;_tmpD8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{
void*_tmpD7[2]={& _tmpD8,& _tmpD9};Cyc_Tcutil_terr(loc,_tag_arr("comparison not allowed between %s and %s",
sizeof(unsigned char),41),_tag_arr(_tmpD7,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpDA=_cycalloc(sizeof(*_tmpDA));
_tmpDA->v=Cyc_Tcenv_lookup_type_vars(te);_tmpDA;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpDB=p;_LLCF: if((int)_tmpDB == 0){
goto _LLD0;}else{goto _LLD1;}_LLD1: if((int)_tmpDB == 2){goto _LLD2;}else{goto _LLD3;}
_LLD3: if((int)_tmpDB == 1){goto _LLD4;}else{goto _LLD5;}_LLD5: if((int)_tmpDB == 3){
goto _LLD6;}else{goto _LLD7;}_LLD7: if((int)_tmpDB == 4){goto _LLD8;}else{goto _LLD9;}
_LLD9: if((int)_tmpDB == 13){goto _LLDA;}else{goto _LLDB;}_LLDB: if((int)_tmpDB == 14){
goto _LLDC;}else{goto _LLDD;}_LLDD: if((int)_tmpDB == 15){goto _LLDE;}else{goto _LLDF;}
_LLDF: if((int)_tmpDB == 16){goto _LLE0;}else{goto _LLE1;}_LLE1: if((int)_tmpDB == 17){
goto _LLE2;}else{goto _LLE3;}_LLE3: if((int)_tmpDB == 18){goto _LLE4;}else{goto _LLE5;}
_LLE5: if((int)_tmpDB == 5){goto _LLE6;}else{goto _LLE7;}_LLE7: if((int)_tmpDB == 6){
goto _LLE8;}else{goto _LLE9;}_LLE9: if((int)_tmpDB == 7){goto _LLEA;}else{goto _LLEB;}
_LLEB: if((int)_tmpDB == 8){goto _LLEC;}else{goto _LLED;}_LLED: if((int)_tmpDB == 9){
goto _LLEE;}else{goto _LLEF;}_LLEF: if((int)_tmpDB == 10){goto _LLF0;}else{goto _LLF1;}
_LLF1: goto _LLF2;_LLD0: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLD2: return Cyc_Tcexp_tcMinus(
te,e1,e2);_LLD4: goto _LLD6;_LLD6: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LLD8: goto _LLDA;_LLDA: goto _LLDC;_LLDC: goto _LLDE;_LLDE: goto _LLE0;_LLE0: goto _LLE2;
_LLE2: goto _LLE4;_LLE4: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);
_LLE6: goto _LLE8;_LLE8: goto _LLEA;_LLEA: goto _LLEC;_LLEC: goto _LLEE;_LLEE: goto _LLF0;
_LLF0: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLF2:({void*_tmpDC[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad binary primop",
sizeof(unsigned char),18),_tag_arr(_tmpDC,sizeof(void*),0));});_LLCE:;}static
void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*p,struct Cyc_List_List*es){if(p == (void*)2?((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es)== 1: 0){return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd);}Cyc_Tcexp_tcExpList(te,es);{void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLF3: return({
void*_tmpDD[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("primitive operator has 0 arguments",
sizeof(unsigned char),35),_tag_arr(_tmpDD,sizeof(void*),0));});case 1: _LLF4: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLF5: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLF6: return({void*_tmpDE[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("primitive operator has > 2 arguments",
sizeof(unsigned char),37),_tag_arr(_tmpDE,sizeof(void*),0));});}return t;}}struct
_tuple8{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmpDF=(void*)e->r;void*
_tmpE0;struct Cyc_Absyn_Vardecl*_tmpE1;void*_tmpE2;struct Cyc_Absyn_Vardecl*_tmpE3;
void*_tmpE4;struct Cyc_Absyn_Vardecl*_tmpE5;void*_tmpE6;struct Cyc_Absyn_Vardecl*
_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;struct _tagged_arr*
_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct _tagged_arr*_tmpEC;struct Cyc_Absyn_Exp*
_tmpED;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*
_tmpF0;_LLF9: if(*((int*)_tmpDF)== 1){_LL10F: _tmpE0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpDF)->f2;if((unsigned int)_tmpE0 > 1?*((int*)_tmpE0)== 2: 0){_LL110: _tmpE1=((
struct Cyc_Absyn_Param_b_struct*)_tmpE0)->f1;goto _LLFA;}else{goto _LLFB;}}else{
goto _LLFB;}_LLFB: if(*((int*)_tmpDF)== 1){_LL111: _tmpE2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpDF)->f2;if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 3: 0){_LL112: _tmpE3=((
struct Cyc_Absyn_Local_b_struct*)_tmpE2)->f1;goto _LLFC;}else{goto _LLFD;}}else{
goto _LLFD;}_LLFD: if(*((int*)_tmpDF)== 1){_LL113: _tmpE4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpDF)->f2;if((unsigned int)_tmpE4 > 1?*((int*)_tmpE4)== 4: 0){_LL114: _tmpE5=((
struct Cyc_Absyn_Pat_b_struct*)_tmpE4)->f1;goto _LLFE;}else{goto _LLFF;}}else{goto
_LLFF;}_LLFF: if(*((int*)_tmpDF)== 1){_LL115: _tmpE6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpDF)->f2;if((unsigned int)_tmpE6 > 1?*((int*)_tmpE6)== 0: 0){_LL116: _tmpE7=((
struct Cyc_Absyn_Global_b_struct*)_tmpE6)->f1;goto _LL100;}else{goto _LL101;}}else{
goto _LL101;}_LL101: if(*((int*)_tmpDF)== 23){_LL118: _tmpE9=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpDF)->f1;goto _LL117;_LL117: _tmpE8=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpDF)->f2;goto _LL102;}else{goto _LL103;}_LL103: if(*((int*)_tmpDF)== 21){_LL11A:
_tmpEB=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpDF)->f1;goto _LL119;_LL119:
_tmpEA=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpDF)->f2;goto _LL104;}else{goto
_LL105;}_LL105: if(*((int*)_tmpDF)== 22){_LL11C: _tmpED=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpDF)->f1;goto _LL11B;_LL11B: _tmpEC=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpDF)->f2;goto _LL106;}else{goto _LL107;}_LL107: if(*((int*)_tmpDF)== 20){_LL11D:
_tmpEE=((struct Cyc_Absyn_Deref_e_struct*)_tmpDF)->f1;goto _LL108;}else{goto _LL109;}
_LL109: if(*((int*)_tmpDF)== 11){_LL11E: _tmpEF=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpDF)->f1;goto _LL10A;}else{goto _LL10B;}_LL10B: if(*((int*)_tmpDF)== 12){_LL11F:
_tmpF0=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpDF)->f1;goto _LL10C;}else{goto
_LL10D;}_LL10D: goto _LL10E;_LLFA: _tmpE3=_tmpE1;goto _LLFC;_LLFC: _tmpE5=_tmpE3;goto
_LLFE;_LLFE: _tmpE7=_tmpE5;goto _LL100;_LL100: if(!(_tmpE7->tq).q_const){return;}
goto _LLF8;_LL102:{void*_tmpF1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpE9->topt))->v);struct Cyc_Absyn_PtrInfo _tmpF2;struct Cyc_Absyn_Tqual
_tmpF3;struct Cyc_Absyn_Tqual _tmpF4;struct Cyc_List_List*_tmpF5;_LL121: if((
unsigned int)_tmpF1 > 3?*((int*)_tmpF1)== 4: 0){_LL129: _tmpF2=((struct Cyc_Absyn_PointerType_struct*)
_tmpF1)->f1;_LL12A: _tmpF3=_tmpF2.tq;goto _LL122;}else{goto _LL123;}_LL123: if((
unsigned int)_tmpF1 > 3?*((int*)_tmpF1)== 7: 0){_LL12B: _tmpF4=((struct Cyc_Absyn_ArrayType_struct*)
_tmpF1)->f2;goto _LL124;}else{goto _LL125;}_LL125: if((unsigned int)_tmpF1 > 3?*((
int*)_tmpF1)== 9: 0){_LL12C: _tmpF5=((struct Cyc_Absyn_TupleType_struct*)_tmpF1)->f1;
goto _LL126;}else{goto _LL127;}_LL127: goto _LL128;_LL122: _tmpF4=_tmpF3;goto _LL124;
_LL124: if(! _tmpF4.q_const){return;}goto _LL120;_LL126: {unsigned int i=Cyc_Evexp_eval_const_uint_exp(
_tmpE8);{struct _handler_cons _tmpF6;_push_handler(& _tmpF6);{int _tmpF8=0;if(setjmp(
_tmpF6.handler)){_tmpF8=1;}if(! _tmpF8){{struct _tuple8 _tmpFA;struct Cyc_Absyn_Tqual
_tmpFB;struct _tuple8*_tmpF9=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmpF5,(int)i);_tmpFA=*_tmpF9;_LL12E: _tmpFB=_tmpFA.f1;goto _LL12D;_LL12D: if(!
_tmpFB.q_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmpF7=(
void*)_exn_thrown;void*_tmpFD=_tmpF7;_LL130: if(_tmpFD == Cyc_List_Nth){goto _LL131;}
else{goto _LL132;}_LL132: goto _LL133;_LL131: return;_LL133:(void)_throw(_tmpFD);
_LL12F:;}}}goto _LL120;}_LL128: goto _LL120;_LL120:;}goto _LLF8;_LL104:{void*_tmpFE=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpEB->topt))->v);
struct Cyc_Absyn_AggrInfo _tmpFF;void*_tmp100;struct Cyc_Absyn_Aggrdecl**_tmp101;
struct Cyc_List_List*_tmp102;_LL135: if((unsigned int)_tmpFE > 3?*((int*)_tmpFE)== 
10: 0){_LL13B: _tmpFF=((struct Cyc_Absyn_AggrType_struct*)_tmpFE)->f1;_LL13C:
_tmp100=(void*)_tmpFF.aggr_info;if(*((int*)_tmp100)== 1){_LL13D: _tmp101=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp100)->f1;goto _LL136;}else{goto _LL137;}}else{goto
_LL137;}_LL137: if((unsigned int)_tmpFE > 3?*((int*)_tmpFE)== 11: 0){_LL13E: _tmp102=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpFE)->f2;goto _LL138;}else{goto _LL139;}
_LL139: goto _LL13A;_LL136: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp101 == 0? 0: Cyc_Absyn_lookup_decl_field(*_tmp101,_tmpEA);if(sf == 0? 1: !(sf->tq).q_const){
return;}goto _LL134;}_LL138: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp102,_tmpEA);if(sf == 0? 1: !(sf->tq).q_const){return;}goto _LL134;}_LL13A: goto
_LL134;_LL134:;}goto _LLF8;_LL106:{void*_tmp103=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpED->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp104;void*_tmp105;_LL140: if((unsigned int)_tmp103 > 3?*((int*)_tmp103)== 4: 0){
_LL144: _tmp104=((struct Cyc_Absyn_PointerType_struct*)_tmp103)->f1;_LL145: _tmp105=(
void*)_tmp104.elt_typ;goto _LL141;}else{goto _LL142;}_LL142: goto _LL143;_LL141:{
void*_tmp106=Cyc_Tcutil_compress(_tmp105);struct Cyc_Absyn_AggrInfo _tmp107;void*
_tmp108;struct Cyc_Absyn_Aggrdecl**_tmp109;struct Cyc_List_List*_tmp10A;_LL147: if((
unsigned int)_tmp106 > 3?*((int*)_tmp106)== 10: 0){_LL14D: _tmp107=((struct Cyc_Absyn_AggrType_struct*)
_tmp106)->f1;_LL14E: _tmp108=(void*)_tmp107.aggr_info;if(*((int*)_tmp108)== 1){
_LL14F: _tmp109=((struct Cyc_Absyn_KnownAggr_struct*)_tmp108)->f1;goto _LL148;}
else{goto _LL149;}}else{goto _LL149;}_LL149: if((unsigned int)_tmp106 > 3?*((int*)
_tmp106)== 11: 0){_LL150: _tmp10A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp106)->f2;
goto _LL14A;}else{goto _LL14B;}_LL14B: goto _LL14C;_LL148: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp109 == 0? 0: Cyc_Absyn_lookup_decl_field(*
_tmp109,_tmpEC);if(sf == 0? 1: !(sf->tq).q_const){return;}goto _LL146;}_LL14A: {
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp10A,_tmpEC);if(sf == 0? 1:
!(sf->tq).q_const){return;}goto _LL146;}_LL14C: goto _LL146;_LL146:;}goto _LL13F;
_LL143: goto _LL13F;_LL13F:;}goto _LLF8;_LL108:{void*_tmp10B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEE->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp10C;struct Cyc_Absyn_Tqual _tmp10D;struct Cyc_Absyn_Tqual _tmp10E;_LL152: if((
unsigned int)_tmp10B > 3?*((int*)_tmp10B)== 4: 0){_LL158: _tmp10C=((struct Cyc_Absyn_PointerType_struct*)
_tmp10B)->f1;_LL159: _tmp10D=_tmp10C.tq;goto _LL153;}else{goto _LL154;}_LL154: if((
unsigned int)_tmp10B > 3?*((int*)_tmp10B)== 7: 0){_LL15A: _tmp10E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp10B)->f2;goto _LL155;}else{goto _LL156;}_LL156: goto _LL157;_LL153: _tmp10E=
_tmp10D;goto _LL155;_LL155: if(! _tmp10E.q_const){return;}goto _LL151;_LL157: goto
_LL151;_LL151:;}goto _LLF8;_LL10A: _tmpF0=_tmpEF;goto _LL10C;_LL10C: Cyc_Tcexp_check_writable(
te,_tmpF0);return;_LL10E: goto _LLF8;_LLF8:;}({struct Cyc_Std_String_pa_struct
_tmp110;_tmp110.tag=0;_tmp110.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e);{
void*_tmp10F[1]={& _tmp110};Cyc_Tcutil_terr(e->loc,_tag_arr("attempt to write a const location: %s",
sizeof(unsigned char),38),_tag_arr(_tmp10F,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(! Cyc_Absyn_is_lvalue(e)){return({
void*_tmp111[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("increment/decrement of non-lvalue",
sizeof(unsigned char),34),_tag_arr(_tmp111,sizeof(void*),0));});}Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(! Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt)){if(! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(telt),(void*)1)){({void*_tmp112[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("can't perform arithmetic on abstract pointer type",sizeof(
unsigned char),50),_tag_arr(_tmp112,sizeof(void*),0));});}}else{({struct Cyc_Std_String_pa_struct
_tmp114;_tmp114.tag=0;_tmp114.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp113[1]={& _tmp114};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting arithmetic or ? type but found %s",
sizeof(unsigned char),44),_tag_arr(_tmp113,sizeof(void*),1));}});}}return t;}}
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){Cyc_Tcexp_tcTest(te,e1,_tag_arr("conditional expression",sizeof(
unsigned char),23));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{
void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp11C;}));struct Cyc_List_List*_tmp115=({struct Cyc_List_List*_tmp11B=_cycalloc(
sizeof(*_tmp11B));_tmp11B->hd=e3;_tmp11B->tl=0;_tmp11B;});struct Cyc_List_List*
_tmp116=({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->hd=e2;
_tmp11A->tl=_tmp115;_tmp11A;});if(! Cyc_Tcutil_coerce_list(te,t,_tmp116)){({
struct Cyc_Std_String_pa_struct _tmp119;_tmp119.tag=0;_tmp119.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v);{
struct Cyc_Std_String_pa_struct _tmp118;_tmp118.tag=0;_tmp118.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{
void*_tmp117[2]={& _tmp118,& _tmp119};Cyc_Tcutil_terr(loc,_tag_arr("conditional clause types do not match: %s != %s",
sizeof(unsigned char),48),_tag_arr(_tmp117,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t;}}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExpNoPromote(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp11D=Cyc_Tcutil_compress(t1);_LL15C: if((unsigned int)_tmp11D > 3?*((int*)
_tmp11D)== 7: 0){goto _LL15D;}else{goto _LL15E;}_LL15E: goto _LL15F;_LL15D:({void*
_tmp11E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("cannot assign to an array",sizeof(
unsigned char),26),_tag_arr(_tmp11E,sizeof(void*),0));});goto _LL15B;_LL15F: goto
_LL15B;_LL15B:;}if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),(void*)1)){({
void*_tmp11F[0]={};Cyc_Tcutil_terr(loc,_tag_arr("type is abstract (can't determine size).",
sizeof(unsigned char),41),_tag_arr(_tmp11F,sizeof(void*),0));});}if(! Cyc_Absyn_is_lvalue(
e1)){return({void*_tmp120[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("assignment to non-lvalue",
sizeof(unsigned char),25),_tag_arr(_tmp120,sizeof(void*),0));});}Cyc_Tcexp_check_writable(
te,e1);if(po == 0){if(! Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp121=({struct
Cyc_Std_String_pa_struct _tmp124;_tmp124.tag=0;_tmp124.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t2);{struct Cyc_Std_String_pa_struct _tmp123;_tmp123.tag=0;_tmp123.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp122[2]={& _tmp123,& _tmp124};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type mismatch: %s != %s",sizeof(unsigned char),24),_tag_arr(
_tmp122,sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp121;}}else{void*_tmp125=(void*)po->v;void*_tmp126=Cyc_Tcexp_tcBinPrimop(
te,loc,0,_tmp125,e1,e2);if(!(Cyc_Tcutil_unify(_tmp126,t1)? 1:(Cyc_Tcutil_coerceable(
_tmp126)? Cyc_Tcutil_coerceable(t1): 0))){void*_tmp127=({struct Cyc_Std_String_pa_struct
_tmp12A;_tmp12A.tag=0;_tmp12A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp129;_tmp129.tag=0;_tmp129.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp128[2]={& _tmp129,& _tmp12A};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof(unsigned char),82),_tag_arr(_tmp128,sizeof(void*),2));}}});Cyc_Tcutil_unify(
_tmp126,t1);Cyc_Tcutil_explain_failure();return _tmp127;}return _tmp126;}return t1;}}
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(
e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,
struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct dbl={6,0};static
void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;{void*_tmp12B=Cyc_Tcutil_compress(t1);void*_tmp12C;void*
_tmp12D;_LL161: if((int)_tmp12B == 1){goto _LL162;}else{goto _LL163;}_LL163: if((
unsigned int)_tmp12B > 3?*((int*)_tmp12B)== 5: 0){_LL169: _tmp12C=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp12B)->f2;if((int)_tmp12C == 0){goto _LL164;}else{
goto _LL165;}}else{goto _LL165;}_LL165: if((unsigned int)_tmp12B > 3?*((int*)_tmp12B)
== 5: 0){_LL16A: _tmp12D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp12B)->f2;if((
int)_tmp12D == 1){goto _LL166;}else{goto _LL167;}}else{goto _LL167;}_LL167: goto
_LL168;_LL162: Cyc_Tcutil_unchecked_cast(te,e,dbl_typ);t1=dbl_typ;goto _LL160;
_LL164: goto _LL166;_LL166: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_t);t1=Cyc_Absyn_sint_t;
goto _LL160;_LL168: goto _LL160;_LL160:;}for(fields=fs;fields != 0;fields=fields->tl){
void*_tmp12F;struct Cyc_Position_Segment*_tmp130;struct Cyc_List_List*_tmp131;
struct _tuple1*_tmp132;struct Cyc_Absyn_Tunionfield _tmp12E=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_LL16F: _tmp132=_tmp12E.name;goto _LL16E;_LL16E: _tmp131=_tmp12E.typs;
goto _LL16D;_LL16D: _tmp130=_tmp12E.loc;goto _LL16C;_LL16C: _tmp12F=(void*)_tmp12E.sc;
goto _LL16B;_LL16B: if(_tmp131 == 0? 1: _tmp131->tl != 0){continue;}{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple8*)_tmp131->hd)).f2);if(Cyc_Tcutil_unify(t1,t2)){return(
struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}}for(
fields=fs;fields != 0;fields=fields->tl){void*_tmp134;struct Cyc_Position_Segment*
_tmp135;struct Cyc_List_List*_tmp136;struct _tuple1*_tmp137;struct Cyc_Absyn_Tunionfield
_tmp133=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_LL174: _tmp137=_tmp133.name;
goto _LL173;_LL173: _tmp136=_tmp133.typs;goto _LL172;_LL172: _tmp135=_tmp133.loc;
goto _LL171;_LL171: _tmp134=(void*)_tmp133.sc;goto _LL170;_LL170: if(_tmp136 == 0? 1:
_tmp136->tl != 0){continue;}{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple8*)_tmp136->hd)).f2);if(Cyc_Tcutil_coerce_arg(te,e,t2)){return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}}({struct Cyc_Std_String_pa_struct
_tmp13A;_tmp13A.tag=0;_tmp13A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{
struct Cyc_Std_String_pa_struct _tmp139;_tmp139.tag=0;_tmp139.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(tu);{void*_tmp138[2]={& _tmp139,& _tmp13A};Cyc_Tcutil_terr(
e->loc,_tag_arr("can't find a field in %s to inject a value of type %s",sizeof(
unsigned char),54),_tag_arr(_tmp138,sizeof(void*),2));}}});return 0;}static void*
Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info){struct Cyc_List_List*_tmp13C=args;te=Cyc_Tcenv_new_block(loc,te);
Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp13D=t;struct Cyc_Absyn_PtrInfo _tmp13E;struct
Cyc_Absyn_Conref*_tmp13F;struct Cyc_Absyn_Tqual _tmp140;struct Cyc_Absyn_Conref*
_tmp141;void*_tmp142;void*_tmp143;_LL176: if((unsigned int)_tmp13D > 3?*((int*)
_tmp13D)== 4: 0){_LL17A: _tmp13E=((struct Cyc_Absyn_PointerType_struct*)_tmp13D)->f1;
_LL17F: _tmp143=(void*)_tmp13E.elt_typ;goto _LL17E;_LL17E: _tmp142=(void*)_tmp13E.rgn_typ;
goto _LL17D;_LL17D: _tmp141=_tmp13E.nullable;goto _LL17C;_LL17C: _tmp140=_tmp13E.tq;
goto _LL17B;_LL17B: _tmp13F=_tmp13E.bounds;goto _LL177;}else{goto _LL178;}_LL178:
goto _LL179;_LL177: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp142);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp13F);{void*_tmp144=Cyc_Tcutil_compress(_tmp143);struct Cyc_Absyn_FnInfo
_tmp145;struct Cyc_List_List*_tmp146;struct Cyc_List_List*_tmp147;struct Cyc_Absyn_VarargInfo*
_tmp148;int _tmp149;struct Cyc_List_List*_tmp14A;void*_tmp14B;struct Cyc_Core_Opt*
_tmp14C;struct Cyc_List_List*_tmp14D;_LL181: if((unsigned int)_tmp144 > 3?*((int*)
_tmp144)== 8: 0){_LL185: _tmp145=((struct Cyc_Absyn_FnType_struct*)_tmp144)->f1;
_LL18D: _tmp14D=_tmp145.tvars;goto _LL18C;_LL18C: _tmp14C=_tmp145.effect;goto _LL18B;
_LL18B: _tmp14B=(void*)_tmp145.ret_typ;goto _LL18A;_LL18A: _tmp14A=_tmp145.args;
goto _LL189;_LL189: _tmp149=_tmp145.c_varargs;goto _LL188;_LL188: _tmp148=_tmp145.cyc_varargs;
goto _LL187;_LL187: _tmp147=_tmp145.rgn_po;goto _LL186;_LL186: _tmp146=_tmp145.attributes;
goto _LL182;}else{goto _LL183;}_LL183: goto _LL184;_LL182: if(topt != 0){Cyc_Tcutil_unify(
_tmp14B,*topt);}while(_tmp13C != 0? _tmp14A != 0: 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp13C->hd;void*t2=(*((struct _tuple2*)_tmp14A->hd)).f3;Cyc_Tcexp_tcExp(
te,(void**)& t2,e1);if(! Cyc_Tcutil_coerce_arg(te,e1,t2)){({struct Cyc_Std_String_pa_struct
_tmp150;_tmp150.tag=0;_tmp150.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp14F;_tmp14F.tag=0;_tmp14F.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{
void*_tmp14E[2]={& _tmp14F,& _tmp150};Cyc_Tcutil_terr(e1->loc,_tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof(unsigned char),54),_tag_arr(_tmp14E,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
_tmp13C=_tmp13C->tl;_tmp14A=_tmp14A->tl;}if(_tmp14A != 0){({void*_tmp151[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments for function",sizeof(
unsigned char),31),_tag_arr(_tmp151,sizeof(void*),0));});}else{if((_tmp13C != 0? 1:
_tmp149)? 1: _tmp148 != 0){if(_tmp149){for(0;_tmp13C != 0;_tmp13C=_tmp13C->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmp13C->hd);}}else{if(_tmp148 == 0){({void*_tmp152[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too many arguments for function",sizeof(
unsigned char),32),_tag_arr(_tmp152,sizeof(void*),0));});}else{int _tmp154;void*
_tmp155;struct Cyc_Absyn_VarargInfo _tmp153=*_tmp148;_LL190: _tmp155=(void*)_tmp153.type;
goto _LL18F;_LL18F: _tmp154=_tmp153.inject;goto _LL18E;_LL18E: {struct Cyc_Absyn_VarargCallInfo*
_tmp156=({struct Cyc_Absyn_VarargCallInfo*_tmp16A=_cycalloc(sizeof(*_tmp16A));
_tmp16A->num_varargs=0;_tmp16A->injectors=0;_tmp16A->vai=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp148);_tmp16A;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp156;if(! _tmp154){for(0;_tmp13C != 0;_tmp13C=_tmp13C->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp13C->hd;_tmp156->num_varargs ++;Cyc_Tcexp_tcExp(te,(
void**)& _tmp155,e1);if(! Cyc_Tcutil_coerce_arg(te,e1,_tmp155)){({struct Cyc_Std_String_pa_struct
_tmp159;_tmp159.tag=0;_tmp159.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp158;_tmp158.tag=0;_tmp158.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp155);{void*_tmp157[2]={& _tmp158,& _tmp159};Cyc_Tcutil_terr(loc,_tag_arr("vararg requires type %s but argument has type %s",
sizeof(unsigned char),49),_tag_arr(_tmp157,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}}
else{void*_tmp15A=Cyc_Tcutil_compress(_tmp155);struct Cyc_Absyn_TunionInfo _tmp15B;
void*_tmp15C;struct Cyc_List_List*_tmp15D;void*_tmp15E;struct Cyc_Absyn_Tuniondecl**
_tmp15F;struct Cyc_Absyn_Tuniondecl*_tmp160;_LL192: if((unsigned int)_tmp15A > 3?*((
int*)_tmp15A)== 2: 0){_LL196: _tmp15B=((struct Cyc_Absyn_TunionType_struct*)_tmp15A)->f1;
_LL199: _tmp15E=(void*)_tmp15B.tunion_info;if(*((int*)_tmp15E)== 1){_LL19A:
_tmp15F=((struct Cyc_Absyn_KnownTunion_struct*)_tmp15E)->f1;_tmp160=*_tmp15F;goto
_LL198;}else{goto _LL194;}_LL198: _tmp15D=_tmp15B.targs;goto _LL197;_LL197: _tmp15C=(
void*)_tmp15B.rgn;goto _LL193;}else{goto _LL194;}_LL194: goto _LL195;_LL193: {struct
Cyc_Absyn_Tuniondecl*_tmp161=*Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp160->name);
struct Cyc_List_List*fields=0;if(_tmp161->fields == 0){({struct Cyc_Std_String_pa_struct
_tmp163;_tmp163.tag=0;_tmp163.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp155);{void*_tmp162[1]={& _tmp163};Cyc_Tcutil_terr(loc,_tag_arr("can't inject into %s",
sizeof(unsigned char),21),_tag_arr(_tmp162,sizeof(void*),1));}});}else{fields=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp161->fields))->v;}if(
! Cyc_Tcutil_unify(_tmp15C,Cyc_Tcenv_curr_rgn(te))){({void*_tmp164[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bad region for injected varargs",sizeof(unsigned char),32),
_tag_arr(_tmp164,sizeof(void*),0));});}{struct _RegionHandle _tmp165=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp165;_push_region(rgn);{struct Cyc_List_List*_tmp166=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp161->tvs,_tmp15D);for(0;
_tmp13C != 0;_tmp13C=_tmp13C->tl){_tmp156->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp13C->hd;Cyc_Tcexp_tcExp(te,0,e1);{struct Cyc_Absyn_Tunionfield*
_tmp167=Cyc_Tcexp_tcInjection(te,e1,_tmp155,rgn,_tmp166,fields);if(_tmp167 != 0){
_tmp156->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp156->injectors,({struct Cyc_List_List*_tmp168=_cycalloc(
sizeof(*_tmp168));_tmp168->hd=(struct Cyc_Absyn_Tunionfield*)_check_null(_tmp167);
_tmp168->tl=0;_tmp168;}));}}}}};_pop_region(rgn);}goto _LL191;}_LL195:({void*
_tmp169[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad inject vararg type",sizeof(
unsigned char),23),_tag_arr(_tmp169,sizeof(void*),0));});goto _LL191;_LL191:;}}}}}}{
struct Cyc_List_List*a=_tmp146;for(0;a != 0;a=a->tl){void*_tmp16B=(void*)a->hd;int
_tmp16C;int _tmp16D;void*_tmp16E;_LL19C: if((unsigned int)_tmp16B > 16?*((int*)
_tmp16B)== 3: 0){_LL1A2: _tmp16E=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp16B)->f1;goto _LL1A1;_LL1A1: _tmp16D=((struct Cyc_Absyn_Format_att_struct*)
_tmp16B)->f2;goto _LL1A0;_LL1A0: _tmp16C=((struct Cyc_Absyn_Format_att_struct*)
_tmp16B)->f3;goto _LL19D;}else{goto _LL19E;}_LL19E: goto _LL19F;_LL19D:{struct
_handler_cons _tmp16F;_push_handler(& _tmp16F);{int _tmp171=0;if(setjmp(_tmp16F.handler)){
_tmp171=1;}if(! _tmp171){{struct Cyc_Absyn_Exp*_tmp172=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp16D - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp16C == 0){fmt_args=0;}else{fmt_args=({struct Cyc_Core_Opt*_tmp173=
_cycalloc(sizeof(*_tmp173));_tmp173->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,int i))Cyc_List_nth_tail)(args,_tmp16C - 1);_tmp173;});}{void*_tmp174=_tmp16E;
_LL1A4: if((int)_tmp174 == 0){goto _LL1A5;}else{goto _LL1A6;}_LL1A6: if((int)_tmp174
== 1){goto _LL1A7;}else{goto _LL1A3;}_LL1A5: Cyc_Tcexp_check_format_args(te,_tmp172,
fmt_args,Cyc_Formatstr_get_format_typs);goto _LL1A3;_LL1A7: Cyc_Tcexp_check_format_args(
te,_tmp172,fmt_args,Cyc_Formatstr_get_scanf_typs);goto _LL1A3;_LL1A3:;}};
_pop_handler();}else{void*_tmp170=(void*)_exn_thrown;void*_tmp176=_tmp170;_LL1A9:
if(_tmp176 == Cyc_List_Nth){goto _LL1AA;}else{goto _LL1AB;}_LL1AB: goto _LL1AC;_LL1AA:({
void*_tmp177[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format arguments",sizeof(
unsigned char),21),_tag_arr(_tmp177,sizeof(void*),0));});goto _LL1A8;_LL1AC:(void)
_throw(_tmp176);_LL1A8:;}}}goto _LL19B;_LL19F: goto _LL19B;_LL19B:;}}Cyc_Tcenv_check_effect_accessible(
te,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp14C))->v);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp147);return _tmp14B;_LL184: return({void*_tmp178[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expected pointer to function",sizeof(unsigned char),29),
_tag_arr(_tmp178,sizeof(void*),0));});_LL180:;}_LL179: return({void*_tmp179[0]={};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("expected pointer to function",sizeof(
unsigned char),29),_tag_arr(_tmp179,sizeof(void*),0));});_LL175:;}}static void*
Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,e);if(
! Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ)){({struct Cyc_Std_String_pa_struct
_tmp17B;_tmp17B.tag=0;_tmp17B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp17A[1]={& _tmp17B};Cyc_Tcutil_terr(
loc,_tag_arr("expected xtunion exn but found %s",sizeof(unsigned char),34),
_tag_arr(_tmp17A,sizeof(void*),1));}});}return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp17C;}));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp17D=t1;struct Cyc_Absyn_PtrInfo _tmp17E;struct Cyc_Absyn_Conref*_tmp17F;
struct Cyc_Absyn_Tqual _tmp180;struct Cyc_Absyn_Conref*_tmp181;void*_tmp182;void*
_tmp183;_LL1AE: if((unsigned int)_tmp17D > 3?*((int*)_tmp17D)== 4: 0){_LL1B2:
_tmp17E=((struct Cyc_Absyn_PointerType_struct*)_tmp17D)->f1;_LL1B7: _tmp183=(void*)
_tmp17E.elt_typ;goto _LL1B6;_LL1B6: _tmp182=(void*)_tmp17E.rgn_typ;goto _LL1B5;
_LL1B5: _tmp181=_tmp17E.nullable;goto _LL1B4;_LL1B4: _tmp180=_tmp17E.tq;goto _LL1B3;
_LL1B3: _tmp17F=_tmp17E.bounds;goto _LL1AF;}else{goto _LL1B0;}_LL1B0: goto _LL1B1;
_LL1AF:{void*_tmp184=Cyc_Tcutil_compress(_tmp183);struct Cyc_Absyn_FnInfo _tmp185;
struct Cyc_List_List*_tmp186;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_VarargInfo*
_tmp188;int _tmp189;struct Cyc_List_List*_tmp18A;void*_tmp18B;struct Cyc_Core_Opt*
_tmp18C;struct Cyc_List_List*_tmp18D;_LL1B9: if((unsigned int)_tmp184 > 3?*((int*)
_tmp184)== 8: 0){_LL1BD: _tmp185=((struct Cyc_Absyn_FnType_struct*)_tmp184)->f1;
_LL1C5: _tmp18D=_tmp185.tvars;goto _LL1C4;_LL1C4: _tmp18C=_tmp185.effect;goto _LL1C3;
_LL1C3: _tmp18B=(void*)_tmp185.ret_typ;goto _LL1C2;_LL1C2: _tmp18A=_tmp185.args;
goto _LL1C1;_LL1C1: _tmp189=_tmp185.c_varargs;goto _LL1C0;_LL1C0: _tmp188=_tmp185.cyc_varargs;
goto _LL1BF;_LL1BF: _tmp187=_tmp185.rgn_po;goto _LL1BE;_LL1BE: _tmp186=_tmp185.attributes;
goto _LL1BA;}else{goto _LL1BB;}_LL1BB: goto _LL1BC;_LL1BA: {struct Cyc_List_List*
instantiation=0;if(Cyc_List_length(ts)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp18D)){return({void*_tmp18E[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type instantiation mismatch",
sizeof(unsigned char),28),_tag_arr(_tmp18E,sizeof(void*),0));});}for(0;ts != 0;(
ts=ts->tl,_tmp18D=_tmp18D->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp18D))->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp18F=_cycalloc(
sizeof(*_tmp18F));_tmp18F->hd=({struct _tuple5*_tmp190=_cycalloc(sizeof(*_tmp190));
_tmp190->f1=(struct Cyc_Absyn_Tvar*)_tmp18D->hd;_tmp190->f2=(void*)ts->hd;_tmp190;});
_tmp18F->tl=instantiation;_tmp18F;});}{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp194=_cycalloc(sizeof(*
_tmp194));_tmp194[0]=({struct Cyc_Absyn_FnType_struct _tmp195;_tmp195.tag=8;
_tmp195.f1=({struct Cyc_Absyn_FnInfo _tmp196;_tmp196.tvars=0;_tmp196.effect=
_tmp18C;_tmp196.ret_typ=(void*)_tmp18B;_tmp196.args=_tmp18A;_tmp196.c_varargs=
_tmp189;_tmp196.cyc_varargs=_tmp188;_tmp196.rgn_po=_tmp187;_tmp196.attributes=
_tmp186;_tmp196;});_tmp195;});_tmp194;}));void*new_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191[0]=({struct Cyc_Absyn_PointerType_struct
_tmp192;_tmp192.tag=4;_tmp192.f1=({struct Cyc_Absyn_PtrInfo _tmp193;_tmp193.elt_typ=(
void*)new_fn_typ;_tmp193.rgn_typ=(void*)_tmp182;_tmp193.nullable=_tmp181;_tmp193.tq=
_tmp180;_tmp193.bounds=_tmp17F;_tmp193;});_tmp192;});_tmp191;});return new_typ;}}
_LL1BC: goto _LL1B8;_LL1B8:;}goto _LL1AD;_LL1B1: goto _LL1AD;_LL1AD:;}return({struct
Cyc_Std_String_pa_struct _tmp198;_tmp198.tag=0;_tmp198.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{void*_tmp197[1]={& _tmp198};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting polymorphic type but found %s",
sizeof(unsigned char),40),_tag_arr(_tmp197,sizeof(void*),1));}});}}static void*
Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,void*t,struct Cyc_Absyn_Exp*e){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,0,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if((! Cyc_Tcutil_silent_castable(te,loc,t2,t)? ! Cyc_Tcutil_castable(
te,loc,t2,t): 0)? ! Cyc_Tcutil_zero_to_null(te,t,e): 0){Cyc_Tcutil_unify(t2,t);{void*
_tmp199=({struct Cyc_Std_String_pa_struct _tmp19C;_tmp19C.tag=0;_tmp19C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp19B;
_tmp19B.tag=0;_tmp19B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*
_tmp19A[2]={& _tmp19B,& _tmp19C};Cyc_Tcexp_expr_err(te,loc,_tag_arr("cannot cast %s to %s",
sizeof(unsigned char),21),_tag_arr(_tmp19A,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return _tmp199;}}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp19D=0;struct Cyc_Absyn_Tqual _tmp19E=Cyc_Absyn_empty_tqual();if(topt
!= 0){void*_tmp19F=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp1A0;
struct Cyc_Absyn_Tqual _tmp1A1;void*_tmp1A2;_LL1C7: if((unsigned int)_tmp19F > 3?*((
int*)_tmp19F)== 4: 0){_LL1CB: _tmp1A0=((struct Cyc_Absyn_PointerType_struct*)
_tmp19F)->f1;_LL1CD: _tmp1A2=(void*)_tmp1A0.elt_typ;goto _LL1CC;_LL1CC: _tmp1A1=
_tmp1A0.tq;goto _LL1C8;}else{goto _LL1C9;}_LL1C9: goto _LL1CA;_LL1C8: _tmp19D=({void**
_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3[0]=_tmp1A2;_tmp1A3;});_tmp19E=
_tmp1A1;goto _LL1C6;_LL1CA: goto _LL1C6;_LL1C6:;}Cyc_Tcexp_tcExpNoInst(te,_tmp19D,e);{
void*_tmp1A4=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A6;
_LL1CF: if(*((int*)_tmp1A4)== 28){goto _LL1D0;}else{goto _LL1D1;}_LL1D1: if(*((int*)
_tmp1A4)== 24){goto _LL1D2;}else{goto _LL1D3;}_LL1D3: if(*((int*)_tmp1A4)== 23){
_LL1D8: _tmp1A6=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1A4)->f1;goto _LL1D7;
_LL1D7: _tmp1A5=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1A4)->f2;goto _LL1D4;}
else{goto _LL1D5;}_LL1D5: goto _LL1D6;_LL1D0: goto _LL1D2;_LL1D2:({void*_tmp1A7[0]={};
Cyc_Tcutil_warn(loc,_tag_arr("& used to allocate",sizeof(unsigned char),19),
_tag_arr(_tmp1A7,sizeof(void*),0));});{void*(*_tmp1A8)(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq)=Cyc_Absyn_at_typ;if(topt != 0){void*_tmp1A9=Cyc_Tcutil_compress(*topt);struct
Cyc_Absyn_PtrInfo _tmp1AA;struct Cyc_Absyn_Conref*_tmp1AB;_LL1DA: if((unsigned int)
_tmp1A9 > 3?*((int*)_tmp1A9)== 4: 0){_LL1DE: _tmp1AA=((struct Cyc_Absyn_PointerType_struct*)
_tmp1A9)->f1;_LL1DF: _tmp1AB=_tmp1AA.nullable;goto _LL1DB;}else{goto _LL1DC;}_LL1DC:
goto _LL1DD;_LL1DB: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp1AB)){_tmp1A8=Cyc_Absyn_star_typ;}goto _LL1D9;_LL1DD: goto _LL1D9;_LL1D9:;}
return _tmp1A8((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)2,
_tmp19E);}_LL1D4:{void*_tmp1AC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1A6->topt))->v);_LL1E1: if((unsigned int)_tmp1AC > 3?*((int*)
_tmp1AC)== 9: 0){goto _LL1E2;}else{goto _LL1E3;}_LL1E3: goto _LL1E4;_LL1E2: goto _LL1E0;
_LL1E4:(void*)(e0->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*_tmp1AD=
_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=({struct Cyc_Absyn_Primop_e_struct _tmp1AE;
_tmp1AE.tag=3;_tmp1AE.f1=(void*)((void*)0);_tmp1AE.f2=({struct Cyc_List_List*
_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->hd=_tmp1A6;_tmp1AF->tl=({struct Cyc_List_List*
_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->hd=_tmp1A5;_tmp1B0->tl=0;_tmp1B0;});
_tmp1AF;});_tmp1AE;});_tmp1AD;})));return Cyc_Tcexp_tcPlus(te,_tmp1A6,_tmp1A5);
_LL1E0:;}goto _LL1CE;_LL1D6: goto _LL1CE;_LL1CE:;}{void*_tmp1B2;int _tmp1B3;struct
_tuple6 _tmp1B1=Cyc_Tcutil_addressof_props(te,e);_LL1E7: _tmp1B3=_tmp1B1.f1;goto
_LL1E6;_LL1E6: _tmp1B2=_tmp1B1.f2;goto _LL1E5;_LL1E5: {struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();
if(_tmp1B3){tq.q_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp1B2,tq);return t;}}}}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(! Cyc_Evexp_okay_szofarg(t)){({
struct Cyc_Std_String_pa_struct _tmp1B5;_tmp1B5.tag=0;_tmp1B5.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1B4[1]={& _tmp1B5};Cyc_Tcutil_terr(
loc,_tag_arr("sizeof applied to type %s, which has unknown size here",sizeof(
unsigned char),55),_tag_arr(_tmp1B4,sizeof(void*),1));}});}if(topt != 0){void*
_tmp1B6=Cyc_Tcutil_compress(*topt);_LL1E9: if((unsigned int)_tmp1B6 > 3?*((int*)
_tmp1B6)== 14: 0){goto _LL1EA;}else{goto _LL1EB;}_LL1EB: goto _LL1EC;_LL1EA: return(
void*)({struct Cyc_Absyn_SizeofType_struct*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));
_tmp1B7[0]=({struct Cyc_Absyn_SizeofType_struct _tmp1B8;_tmp1B8.tag=14;_tmp1B8.f1=(
void*)t;_tmp1B8;});_tmp1B7;});_LL1EC: goto _LL1E8;_LL1E8:;}return Cyc_Absyn_uint_t;}
int Cyc_Tcexp_structfield_has_name(struct _tagged_arr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_Std_strcmp(*n,*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp1B9=n;struct _tagged_arr*_tmp1BA;unsigned int _tmp1BB;_LL1EE: if(*((int*)
_tmp1B9)== 0){_LL1F2: _tmp1BA=((struct Cyc_Absyn_StructField_struct*)_tmp1B9)->f1;
goto _LL1EF;}else{goto _LL1F0;}_LL1F0: if(*((int*)_tmp1B9)== 1){_LL1F3: _tmp1BB=((
struct Cyc_Absyn_TupleIndex_struct*)_tmp1B9)->f1;goto _LL1F1;}else{goto _LL1ED;}
_LL1EF: {int bad_type=1;{void*_tmp1BC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1BD;void*_tmp1BE;struct Cyc_Absyn_Aggrdecl**_tmp1BF;struct Cyc_List_List*
_tmp1C0;_LL1F5: if((unsigned int)_tmp1BC > 3?*((int*)_tmp1BC)== 10: 0){_LL1FB:
_tmp1BD=((struct Cyc_Absyn_AggrType_struct*)_tmp1BC)->f1;_LL1FC: _tmp1BE=(void*)
_tmp1BD.aggr_info;if(*((int*)_tmp1BE)== 1){_LL1FD: _tmp1BF=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1BE)->f1;goto _LL1F6;}else{goto _LL1F7;}}else{goto _LL1F7;}_LL1F7: if((
unsigned int)_tmp1BC > 3?*((int*)_tmp1BC)== 11: 0){_LL1FE: _tmp1C0=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1BC)->f2;goto _LL1F8;}else{goto _LL1F9;}_LL1F9: goto _LL1FA;_LL1F6: if((*_tmp1BF)->fields
== 0){goto _LL1F4;}if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1BA,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*_tmp1BF)->fields))->v)){({
struct Cyc_Std_String_pa_struct _tmp1C2;_tmp1C2.tag=0;_tmp1C2.f1=(struct
_tagged_arr)*_tmp1BA;{void*_tmp1C1[1]={& _tmp1C2};Cyc_Tcutil_terr(loc,_tag_arr("no field of struct/union has name %s",
sizeof(unsigned char),37),_tag_arr(_tmp1C1,sizeof(void*),1));}});}bad_type=0;
goto _LL1F4;_LL1F8: if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1BA,_tmp1C0)){({struct Cyc_Std_String_pa_struct _tmp1C4;_tmp1C4.tag=0;_tmp1C4.f1=(
struct _tagged_arr)*_tmp1BA;{void*_tmp1C3[1]={& _tmp1C4};Cyc_Tcutil_terr(loc,
_tag_arr("no field of struct/union has name %s",sizeof(unsigned char),37),
_tag_arr(_tmp1C3,sizeof(void*),1));}});}bad_type=0;goto _LL1F4;_LL1FA: goto _LL1F4;
_LL1F4:;}if(bad_type){({struct Cyc_Std_String_pa_struct _tmp1C6;_tmp1C6.tag=0;
_tmp1C6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1C5[1]={&
_tmp1C6};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known struct/union type",
sizeof(unsigned char),36),_tag_arr(_tmp1C5,sizeof(void*),1));}});}goto _LL1ED;}
_LL1F1: {int bad_type=1;{void*_tmp1C7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1C8;void*_tmp1C9;struct Cyc_Absyn_Aggrdecl**_tmp1CA;struct Cyc_List_List*
_tmp1CB;struct Cyc_List_List*_tmp1CC;struct Cyc_Absyn_TunionFieldInfo _tmp1CD;void*
_tmp1CE;struct Cyc_Absyn_Tunionfield*_tmp1CF;_LL200: if((unsigned int)_tmp1C7 > 3?*((
int*)_tmp1C7)== 10: 0){_LL20A: _tmp1C8=((struct Cyc_Absyn_AggrType_struct*)_tmp1C7)->f1;
_LL20B: _tmp1C9=(void*)_tmp1C8.aggr_info;if(*((int*)_tmp1C9)== 1){_LL20C: _tmp1CA=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp1C9)->f1;goto _LL201;}else{goto _LL202;}}
else{goto _LL202;}_LL202: if((unsigned int)_tmp1C7 > 3?*((int*)_tmp1C7)== 11: 0){
_LL20D: _tmp1CB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C7)->f2;goto _LL203;}
else{goto _LL204;}_LL204: if((unsigned int)_tmp1C7 > 3?*((int*)_tmp1C7)== 9: 0){
_LL20E: _tmp1CC=((struct Cyc_Absyn_TupleType_struct*)_tmp1C7)->f1;goto _LL205;}
else{goto _LL206;}_LL206: if((unsigned int)_tmp1C7 > 3?*((int*)_tmp1C7)== 3: 0){
_LL20F: _tmp1CD=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1C7)->f1;_LL210:
_tmp1CE=(void*)_tmp1CD.field_info;if(*((int*)_tmp1CE)== 1){_LL211: _tmp1CF=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp1CE)->f2;goto _LL207;}else{goto
_LL208;}}else{goto _LL208;}_LL208: goto _LL209;_LL201: if((*_tmp1CA)->fields == 0){
goto _LL1FF;}_tmp1CB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*
_tmp1CA)->fields))->v;goto _LL203;_LL203: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp1CB)<= _tmp1BB){({struct Cyc_Std_Int_pa_struct _tmp1D2;_tmp1D2.tag=1;_tmp1D2.f1=(
int)_tmp1BB;{struct Cyc_Std_Int_pa_struct _tmp1D1;_tmp1D1.tag=1;_tmp1D1.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CB));{void*
_tmp1D0[2]={& _tmp1D1,& _tmp1D2};Cyc_Tcutil_terr(loc,_tag_arr("struct/union has too few components: %d <= %d",
sizeof(unsigned char),46),_tag_arr(_tmp1D0,sizeof(void*),2));}}});}bad_type=0;
goto _LL1FF;_LL205: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CC)<= 
_tmp1BB){({struct Cyc_Std_Int_pa_struct _tmp1D5;_tmp1D5.tag=1;_tmp1D5.f1=(int)
_tmp1BB;{struct Cyc_Std_Int_pa_struct _tmp1D4;_tmp1D4.tag=1;_tmp1D4.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CC));{void*
_tmp1D3[2]={& _tmp1D4,& _tmp1D5};Cyc_Tcutil_terr(loc,_tag_arr("tuple has too few components: %d <= %d",
sizeof(unsigned char),39),_tag_arr(_tmp1D3,sizeof(void*),2));}}});}bad_type=0;
goto _LL1FF;_LL207: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CF->typs)
< _tmp1BB){({struct Cyc_Std_Int_pa_struct _tmp1D8;_tmp1D8.tag=1;_tmp1D8.f1=(int)
_tmp1BB;{struct Cyc_Std_Int_pa_struct _tmp1D7;_tmp1D7.tag=1;_tmp1D7.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CF->typs));{
void*_tmp1D6[2]={& _tmp1D7,& _tmp1D8};Cyc_Tcutil_terr(loc,_tag_arr("tunionfield has too few components: %d < %d",
sizeof(unsigned char),44),_tag_arr(_tmp1D6,sizeof(void*),2));}}});}bad_type=0;
goto _LL1FF;_LL209: goto _LL1FF;_LL1FF:;}if(bad_type){({struct Cyc_Std_String_pa_struct
_tmp1DA;_tmp1DA.tag=0;_tmp1DA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp1D9[1]={& _tmp1DA};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known type",
sizeof(unsigned char),23),_tag_arr(_tmp1D9,sizeof(void*),1));}});}goto _LL1ED;}
_LL1ED:;}return Cyc_Absyn_uint_t;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1DB=t;struct Cyc_Absyn_PtrInfo _tmp1DC;struct Cyc_Absyn_Conref*_tmp1DD;
void*_tmp1DE;void*_tmp1DF;_LL213: if((unsigned int)_tmp1DB > 3?*((int*)_tmp1DB)== 
4: 0){_LL217: _tmp1DC=((struct Cyc_Absyn_PointerType_struct*)_tmp1DB)->f1;_LL21A:
_tmp1DF=(void*)_tmp1DC.elt_typ;goto _LL219;_LL219: _tmp1DE=(void*)_tmp1DC.rgn_typ;
goto _LL218;_LL218: _tmp1DD=_tmp1DC.bounds;goto _LL214;}else{goto _LL215;}_LL215:
goto _LL216;_LL214: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp1DE);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp1DD);if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp1DF),(void*)1)){({
void*_tmp1E0[0]={};Cyc_Tcutil_terr(loc,_tag_arr("can't dereference abstract pointer type",
sizeof(unsigned char),40),_tag_arr(_tmp1E0,sizeof(void*),0));});}return _tmp1DF;
_LL216: return({struct Cyc_Std_String_pa_struct _tmp1E2;_tmp1E2.tag=0;_tmp1E2.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1E1[1]={& _tmp1E2};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting * or @ type but found %s",sizeof(unsigned char),35),
_tag_arr(_tmp1E1,sizeof(void*),1));}});_LL212:;}}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,
e);{void*_tmp1E3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp1E4;struct Cyc_List_List*_tmp1E5;void*_tmp1E6;struct
Cyc_Absyn_Aggrdecl**_tmp1E7;struct Cyc_Absyn_Aggrdecl*_tmp1E8;struct Cyc_List_List*
_tmp1E9;_LL21C: if((unsigned int)_tmp1E3 > 3?*((int*)_tmp1E3)== 10: 0){_LL226:
_tmp1E4=((struct Cyc_Absyn_AggrType_struct*)_tmp1E3)->f1;_LL228: _tmp1E6=(void*)
_tmp1E4.aggr_info;if(*((int*)_tmp1E6)== 1){_LL229: _tmp1E7=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1E6)->f1;_tmp1E8=*_tmp1E7;goto _LL227;}else{goto _LL21E;}_LL227: _tmp1E5=
_tmp1E4.targs;goto _LL21D;}else{goto _LL21E;}_LL21E: if((unsigned int)_tmp1E3 > 3?*((
int*)_tmp1E3)== 11: 0){_LL22A: _tmp1E9=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1E3)->f2;goto _LL21F;}else{goto _LL220;}_LL220: if((unsigned int)_tmp1E3 > 3?*((
int*)_tmp1E3)== 7: 0){goto _LL22B;}else{goto _LL222;}_LL22B: if(Cyc_Std_zstrcmp(*f,
_tag_arr("size",sizeof(unsigned char),5))== 0){goto _LL221;}else{goto _LL222;}
_LL222: if((unsigned int)_tmp1E3 > 3?*((int*)_tmp1E3)== 4: 0){goto _LL22C;}else{goto
_LL224;}_LL22C: if(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(unsigned char),5))== 
0){goto _LL223;}else{goto _LL224;}_LL224: goto _LL225;_LL21D: {struct Cyc_Absyn_Aggrfield*
_tmp1EA=Cyc_Absyn_lookup_decl_field(_tmp1E8,f);if(_tmp1EA == 0){return({struct Cyc_Std_String_pa_struct
_tmp1ED;_tmp1ED.tag=0;_tmp1ED.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct
_tmp1EC;_tmp1EC.tag=0;_tmp1EC.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp1E8->name);{void*_tmp1EB[2]={& _tmp1EC,& _tmp1ED};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("type %s has no %s field",sizeof(unsigned char),24),_tag_arr(_tmp1EB,
sizeof(void*),2));}}});}if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1E8->exist_vars))->v != 0){return({void*_tmp1EE[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(unsigned char),64),_tag_arr(_tmp1EE,sizeof(void*),0));});}{void*t2;{
struct _RegionHandle _tmp1EF=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1EF;
_push_region(rgn);{struct Cyc_List_List*_tmp1F0=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1E8->tvs,_tmp1E5);t2=Cyc_Tcutil_rsubstitute(rgn,
_tmp1F0,(void*)_tmp1EA->type);};_pop_region(rgn);}return t2;}}_LL21F: {struct Cyc_Absyn_Aggrfield*
_tmp1F1=Cyc_Absyn_lookup_field(_tmp1E9,f);if(_tmp1F1 == 0){return({struct Cyc_Std_String_pa_struct
_tmp1F3;_tmp1F3.tag=0;_tmp1F3.f1=(struct _tagged_arr)*f;{void*_tmp1F2[1]={&
_tmp1F3};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type has no %s field",sizeof(
unsigned char),21),_tag_arr(_tmp1F2,sizeof(void*),1));}});}return(void*)_tmp1F1->type;}
_LL221: goto _LL223;_LL223:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp1F5;_tmp1F5.tag=3;_tmp1F5.f1=(void*)((void*)19);_tmp1F5.f2=({struct Cyc_List_List*
_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6->hd=e;_tmp1F6->tl=0;_tmp1F6;});
_tmp1F5;});_tmp1F4;})));return Cyc_Absyn_uint_t;_LL225: if(Cyc_Std_zstrcmp(*f,
_tag_arr("size",sizeof(unsigned char),5))== 0){return({struct Cyc_Std_String_pa_struct
_tmp1F8;_tmp1F8.tag=0;_tmp1F8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F7[1]={& _tmp1F8};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct, union, or array, found %s",sizeof(
unsigned char),44),_tag_arr(_tmp1F7,sizeof(void*),1));}});}else{return({struct
Cyc_Std_String_pa_struct _tmp1FA;_tmp1FA.tag=0;_tmp1FA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F9[1]={& _tmp1FA};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting struct or union, found %s",sizeof(
unsigned char),36),_tag_arr(_tmp1F9,sizeof(void*),1));}});}_LL21B:;}}static void*
Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct _tagged_arr*f){Cyc_Tcexp_tcExp(te,0,e);{
void*_tmp1FB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1FC;struct Cyc_Absyn_Conref*_tmp1FD;void*_tmp1FE;void*
_tmp1FF;_LL22E: if((unsigned int)_tmp1FB > 3?*((int*)_tmp1FB)== 4: 0){_LL232:
_tmp1FC=((struct Cyc_Absyn_PointerType_struct*)_tmp1FB)->f1;_LL235: _tmp1FF=(void*)
_tmp1FC.elt_typ;goto _LL234;_LL234: _tmp1FE=(void*)_tmp1FC.rgn_typ;goto _LL233;
_LL233: _tmp1FD=_tmp1FC.bounds;goto _LL22F;}else{goto _LL230;}_LL230: goto _LL231;
_LL22F: Cyc_Tcutil_check_nonzero_bound(loc,_tmp1FD);{void*_tmp200=Cyc_Tcutil_compress(
_tmp1FF);struct Cyc_Absyn_AggrInfo _tmp201;struct Cyc_List_List*_tmp202;void*
_tmp203;struct Cyc_Absyn_Aggrdecl**_tmp204;struct Cyc_Absyn_Aggrdecl*_tmp205;
struct Cyc_List_List*_tmp206;_LL237: if((unsigned int)_tmp200 > 3?*((int*)_tmp200)
== 10: 0){_LL23D: _tmp201=((struct Cyc_Absyn_AggrType_struct*)_tmp200)->f1;_LL23F:
_tmp203=(void*)_tmp201.aggr_info;if(*((int*)_tmp203)== 1){_LL240: _tmp204=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp203)->f1;_tmp205=*_tmp204;goto _LL23E;}
else{goto _LL239;}_LL23E: _tmp202=_tmp201.targs;goto _LL238;}else{goto _LL239;}
_LL239: if((unsigned int)_tmp200 > 3?*((int*)_tmp200)== 11: 0){_LL241: _tmp206=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp200)->f2;goto _LL23A;}else{goto _LL23B;}
_LL23B: goto _LL23C;_LL238: {struct Cyc_Absyn_Aggrfield*_tmp207=Cyc_Absyn_lookup_decl_field(
_tmp205,f);if(_tmp207 == 0){return({struct Cyc_Std_String_pa_struct _tmp20A;_tmp20A.tag=
0;_tmp20A.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct _tmp209;
_tmp209.tag=0;_tmp209.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp205->name);{
void*_tmp208[2]={& _tmp209,& _tmp20A};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type %s has no %s field",
sizeof(unsigned char),24),_tag_arr(_tmp208,sizeof(void*),2));}}});}if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp205->exist_vars))->v != 0){return({void*
_tmp20B[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(unsigned char),64),_tag_arr(_tmp20B,sizeof(void*),0));});}{void*t3;{
struct _RegionHandle _tmp20C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp20C;
_push_region(rgn);{struct Cyc_List_List*_tmp20D=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp205->tvs,_tmp202);t3=Cyc_Tcutil_rsubstitute(rgn,
_tmp20D,(void*)_tmp207->type);};_pop_region(rgn);}return t3;}}_LL23A: {struct Cyc_Absyn_Aggrfield*
_tmp20E=Cyc_Absyn_lookup_field(_tmp206,f);if(_tmp20E == 0){return({struct Cyc_Std_String_pa_struct
_tmp210;_tmp210.tag=0;_tmp210.f1=(struct _tagged_arr)*f;{void*_tmp20F[1]={&
_tmp210};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type has no %s field",sizeof(
unsigned char),21),_tag_arr(_tmp20F,sizeof(void*),1));}});}return(void*)_tmp20E->type;}
_LL23C: goto _LL236;_LL236:;}goto _LL22D;_LL231: goto _LL22D;_LL22D:;}return({struct
Cyc_Std_String_pa_struct _tmp212;_tmp212.tag=0;_tmp212.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp211[1]={& _tmp212};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting struct pointer, found %s",sizeof(
unsigned char),35),_tag_arr(_tmp211,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int i=Cyc_Evexp_eval_const_uint_exp(index);
struct _handler_cons _tmp213;_push_handler(& _tmp213);{int _tmp215=0;if(setjmp(
_tmp213.handler)){_tmp215=1;}if(! _tmp215){{void*_tmp216=(*((struct _tuple8*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0);return
_tmp216;};_pop_handler();}else{void*_tmp214=(void*)_exn_thrown;void*_tmp218=
_tmp214;_LL243: if(_tmp218 == Cyc_List_Nth){goto _LL244;}else{goto _LL245;}_LL245:
goto _LL246;_LL244: return({struct Cyc_Std_Int_pa_struct _tmp21B;_tmp21B.tag=1;
_tmp21B.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts));{struct Cyc_Std_Int_pa_struct _tmp21A;_tmp21A.tag=1;_tmp21A.f1=(int)i;{void*
_tmp219[2]={& _tmp21A,& _tmp21B};Cyc_Tcexp_expr_err(te,loc,_tag_arr("index is %d but tuple has only %d fields",
sizeof(unsigned char),41),_tag_arr(_tmp219,sizeof(void*),2));}}});_LL246:(void)
_throw(_tmp218);_LL242:;}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(! Cyc_Tcutil_coerce_sint_typ(
te,e2)){return({struct Cyc_Std_String_pa_struct _tmp21D;_tmp21D.tag=0;_tmp21D.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp21C[1]={& _tmp21D};Cyc_Tcexp_expr_err(
te,e2->loc,_tag_arr("expecting int subscript, found %s",sizeof(unsigned char),34),
_tag_arr(_tmp21C,sizeof(void*),1));}});}{void*_tmp21E=t1;struct Cyc_Absyn_PtrInfo
_tmp21F;struct Cyc_Absyn_Conref*_tmp220;struct Cyc_Absyn_Tqual _tmp221;void*_tmp222;
void*_tmp223;struct Cyc_List_List*_tmp224;_LL248: if((unsigned int)_tmp21E > 3?*((
int*)_tmp21E)== 4: 0){_LL24E: _tmp21F=((struct Cyc_Absyn_PointerType_struct*)
_tmp21E)->f1;_LL252: _tmp223=(void*)_tmp21F.elt_typ;goto _LL251;_LL251: _tmp222=(
void*)_tmp21F.rgn_typ;goto _LL250;_LL250: _tmp221=_tmp21F.tq;goto _LL24F;_LL24F:
_tmp220=_tmp21F.bounds;goto _LL249;}else{goto _LL24A;}_LL24A: if((unsigned int)
_tmp21E > 3?*((int*)_tmp21E)== 9: 0){_LL253: _tmp224=((struct Cyc_Absyn_TupleType_struct*)
_tmp21E)->f1;goto _LL24B;}else{goto _LL24C;}_LL24C: goto _LL24D;_LL249: if(Cyc_Tcutil_is_const_exp(
te,e2)){Cyc_Tcutil_check_bound(loc,Cyc_Evexp_eval_const_uint_exp(e2),_tmp220);}
else{if(Cyc_Tcutil_is_bound_one(_tmp220)){({void*_tmp225[0]={};Cyc_Tcutil_warn(
e1->loc,_tag_arr("subscript applied to pointer to one object",sizeof(
unsigned char),43),_tag_arr(_tmp225,sizeof(void*),0));});}Cyc_Tcutil_check_nonzero_bound(
loc,_tmp220);}Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp222);if(! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp223),(void*)1)){({void*_tmp226[0]={};Cyc_Tcutil_terr(e1->loc,
_tag_arr("can't subscript an abstract pointer",sizeof(unsigned char),36),
_tag_arr(_tmp226,sizeof(void*),0));});}return _tmp223;_LL24B: return Cyc_Tcexp_ithTupleType(
te,loc,_tmp224,e2);_LL24D: return({struct Cyc_Std_String_pa_struct _tmp228;_tmp228.tag=
0;_tmp228.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp227[1]={&
_tmp228};Cyc_Tcexp_expr_err(te,loc,_tag_arr("subscript applied to %s",sizeof(
unsigned char),24),_tag_arr(_tmp227,sizeof(void*),1));}});_LL247:;}}}static void*
Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;if(topt != 0){
void*_tmp229=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp22A;_LL255: if((
unsigned int)_tmp229 > 3?*((int*)_tmp229)== 9: 0){_LL259: _tmp22A=((struct Cyc_Absyn_TupleType_struct*)
_tmp229)->f1;goto _LL256;}else{goto _LL257;}_LL257: goto _LL258;_LL256: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp22A)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es)){goto _LL254;}for(0;es != 0;(es=es->tl,_tmp22A=_tmp22A->tl)){
void*_tmp22B=(*((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp22A))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp22B,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C->hd=({
struct _tuple8*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->f1=(*((struct _tuple8*)
_tmp22A->hd)).f1;_tmp22D->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp22D;});_tmp22C->tl=fields;_tmp22C;});}done=
1;goto _LL254;_LL258: goto _LL254;_LL254:;}if(! done){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(
te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*_tmp22E=
_cycalloc(sizeof(*_tmp22E));_tmp22E->hd=({struct _tuple8*_tmp22F=_cycalloc(
sizeof(*_tmp22F));_tmp22F->f1=Cyc_Absyn_empty_tqual();_tmp22F->f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp22F;});_tmp22E->tl=fields;_tmp22E;});}}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230[0]=({struct Cyc_Absyn_TupleType_struct
_tmp231;_tmp231.tag=9;_tmp231.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp231;});_tmp230;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp232[0]={};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("tcCompoundLit",sizeof(unsigned char),14),_tag_arr(_tmp232,sizeof(void*),
0));});}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,struct Cyc_List_List*des){struct Cyc_List_List*es=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple7*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp243=_cycalloc(
sizeof(*_tmp243));_tmp243->v=Cyc_Tcenv_lookup_type_vars(te);_tmp243;}));struct
Cyc_Absyn_Const_e_struct*_tmp233=({struct Cyc_Absyn_Const_e_struct*_tmp23F=
_cycalloc(sizeof(*_tmp23F));_tmp23F[0]=({struct Cyc_Absyn_Const_e_struct _tmp240;
_tmp240.tag=0;_tmp240.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp241=
_cycalloc(sizeof(*_tmp241));_tmp241[0]=({struct Cyc_Absyn_Int_c_struct _tmp242;
_tmp242.tag=2;_tmp242.f1=(void*)((void*)1);_tmp242.f2=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es);_tmp242;});_tmp241;}));_tmp240;});_tmp23F;});struct Cyc_Absyn_Exp*
sz_exp=Cyc_Absyn_new_exp((void*)_tmp233,loc);sz_exp->topt=({struct Cyc_Core_Opt*
_tmp234=_cycalloc(sizeof(*_tmp234));_tmp234->v=(void*)Cyc_Absyn_uint_t;_tmp234;});{
void*res_t2=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp23D=_cycalloc(sizeof(*
_tmp23D));_tmp23D[0]=({struct Cyc_Absyn_ArrayType_struct _tmp23E;_tmp23E.tag=7;
_tmp23E.f1=(void*)res;_tmp23E.f2=Cyc_Absyn_empty_tqual();_tmp23E.f3=(struct Cyc_Absyn_Exp*)
sz_exp;_tmp23E;});_tmp23D;});{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(! Cyc_Tcutil_coerce_list(
te,res,es)){({struct Cyc_Std_String_pa_struct _tmp236;_tmp236.tag=0;_tmp236.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(res);{void*_tmp235[1]={& _tmp236};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,_tag_arr("elements of array do not all have the same type (%s)",
sizeof(unsigned char),53),_tag_arr(_tmp235,sizeof(void*),1));}});}{int offset=0;
for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple7*)
des->hd)).f1;if(ds != 0){void*_tmp237=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp238;
_LL25B: if(*((int*)_tmp237)== 1){goto _LL25C;}else{goto _LL25D;}_LL25D: if(*((int*)
_tmp237)== 0){_LL25F: _tmp238=((struct Cyc_Absyn_ArrayElement_struct*)_tmp237)->f1;
goto _LL25E;}else{goto _LL25A;}_LL25C:({void*_tmp239[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("only array index designators are supported",sizeof(unsigned char),43),
_tag_arr(_tmp239,sizeof(void*),0));});goto _LL25A;_LL25E: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp238);{unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmp238);if(i != 
offset){({struct Cyc_Std_Int_pa_struct _tmp23C;_tmp23C.tag=1;_tmp23C.f1=(int)i;{
struct Cyc_Std_Int_pa_struct _tmp23B;_tmp23B.tag=1;_tmp23B.f1=(int)((unsigned int)
offset);{void*_tmp23A[2]={& _tmp23B,& _tmp23C};Cyc_Tcutil_terr(_tmp238->loc,
_tag_arr("expecting index designator %d but found %d",sizeof(unsigned char),43),
_tag_arr(_tmp23A,sizeof(void*),2));}}});}goto _LL25A;}_LL25A:;}}}return res_t2;}}
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body){Cyc_Tcexp_tcExp(te,0,bound);if(! Cyc_Tcutil_coerce_uint_typ(te,bound)){({
struct Cyc_Std_String_pa_struct _tmp245;_tmp245.tag=0;_tmp245.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
bound->topt))->v);{void*_tmp244[1]={& _tmp245};Cyc_Tcutil_terr(bound->loc,
_tag_arr("expecting unsigned int, found %s",sizeof(unsigned char),33),_tag_arr(
_tmp244,sizeof(void*),1));}});}if(!(vd->tq).q_const){({void*_tmp246[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("comprehension index variable is not declared const!",
sizeof(unsigned char),52),_tag_arr(_tmp246,sizeof(void*),0));});}if(te->le != 0){
te=Cyc_Tcenv_new_block(loc,te);te=Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp247=0;struct Cyc_Absyn_Tqual*_tmp248=0;if(topt != 0){void*_tmp249=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp24A;struct Cyc_Absyn_Exp*
_tmp24B;struct Cyc_Absyn_Tqual _tmp24C;struct Cyc_Absyn_Tqual*_tmp24D;void*_tmp24E;
void**_tmp24F;_LL261: if((unsigned int)_tmp249 > 3?*((int*)_tmp249)== 4: 0){_LL267:
_tmp24A=((struct Cyc_Absyn_PointerType_struct*)_tmp249)->f1;goto _LL262;}else{goto
_LL263;}_LL263: if((unsigned int)_tmp249 > 3?*((int*)_tmp249)== 7: 0){_LL26A:
_tmp24E=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp249)->f1;_tmp24F=(void**)&((
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp249)->f1);goto _LL269;_LL269:
_tmp24C=((struct Cyc_Absyn_ArrayType_struct*)_tmp249)->f2;_tmp24D=(struct Cyc_Absyn_Tqual*)&((
struct Cyc_Absyn_ArrayType_struct*)_tmp249)->f2;goto _LL268;_LL268: _tmp24B=((
struct Cyc_Absyn_ArrayType_struct*)_tmp249)->f3;goto _LL264;}else{goto _LL265;}
_LL265: goto _LL266;_LL262: pinfo=_tmp24A;_tmp247=(void**)((void**)((void*)& pinfo.elt_typ));
_tmp248=(struct Cyc_Absyn_Tqual*)& pinfo.tq;goto _LL260;_LL264: _tmp247=(void**)
_tmp24F;_tmp248=(struct Cyc_Absyn_Tqual*)_tmp24D;goto _LL260;_LL266: goto _LL260;
_LL260:;}{void*t=Cyc_Tcexp_tcExp(te,_tmp247,body);if(te->le == 0){if(! Cyc_Tcutil_is_const_exp(
te,bound)){({void*_tmp250[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("bound is not constant",
sizeof(unsigned char),22),_tag_arr(_tmp250,sizeof(void*),0));});}if(! Cyc_Tcutil_is_const_exp(
te,body)){({void*_tmp251[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("body is not constant",
sizeof(unsigned char),21),_tag_arr(_tmp251,sizeof(void*),0));});}}return(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp253;_tmp253.tag=7;_tmp253.f1=(void*)t;
_tmp253.f2=_tmp248 == 0? Cyc_Absyn_empty_tqual():*_tmp248;_tmp253.f3=(struct Cyc_Absyn_Exp*)
bound;_tmp253;});_tmp252;});}}}struct _tuple9{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List*args,
struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0){ad=(
struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);}else{{struct _handler_cons _tmp254;
_push_handler(& _tmp254);{int _tmp256=0;if(setjmp(_tmp254.handler)){_tmp256=1;}if(
! _tmp256){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;_pop_handler();}else{void*
_tmp255=(void*)_exn_thrown;void*_tmp258=_tmp255;_LL26C: if(_tmp258 == Cyc_Dict_Absent){
goto _LL26D;}else{goto _LL26E;}_LL26E: goto _LL26F;_LL26D:({struct Cyc_Std_String_pa_struct
_tmp25A;_tmp25A.tag=0;_tmp25A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{
void*_tmp259[1]={& _tmp25A};Cyc_Tcutil_terr(loc,_tag_arr("unbound struct name %s",
sizeof(unsigned char),23),_tag_arr(_tmp259,sizeof(void*),1));}});return topt != 0?*
topt:(void*)0;_LL26F:(void)_throw(_tmp258);_LL26B:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if((void*)ad->kind == (void*)1){({void*_tmp25B[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("expecting struct but found union",sizeof(unsigned char),33),
_tag_arr(_tmp25B,sizeof(void*),0));});}if(ad->fields == 0){({void*_tmp25C[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("can't build abstract struct",sizeof(unsigned char),
28),_tag_arr(_tmp25C,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp25D;}));}{struct _RegionHandle _tmp25E=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp25E;_push_region(rgn);{struct _tuple4 _tmp25F=({struct _tuple4 _tmp278;
_tmp278.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp278.f2=rgn;_tmp278;});struct Cyc_List_List*
_tmp260=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp25F,ad->tvs);struct Cyc_List_List*_tmp261=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp25F,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ad->exist_vars))->v);struct Cyc_List_List*_tmp262=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp260);struct Cyc_List_List*_tmp263=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp261);struct Cyc_List_List*_tmp264=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp260,_tmp261);struct Cyc_List_List*_tmp265=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp264);struct Cyc_Absyn_AggrType_struct*_tmp266=({struct Cyc_Absyn_AggrType_struct*
_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272[0]=({struct Cyc_Absyn_AggrType_struct
_tmp273;_tmp273.tag=10;_tmp273.f1=({struct Cyc_Absyn_AggrInfo _tmp274;_tmp274.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp275=_cycalloc(sizeof(*
_tmp275));_tmp275[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp276;_tmp276.tag=1;
_tmp276.f1=({struct Cyc_Absyn_Aggrdecl**_tmp277=_cycalloc(sizeof(*_tmp277));
_tmp277[0]=ad;_tmp277;});_tmp276;});_tmp275;}));_tmp274.targs=_tmp262;_tmp274;});
_tmp273;});_tmp272;});if(topt != 0){Cyc_Tcutil_unify((void*)_tmp266,*topt);}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ad->fields))->v);for(0;fields != 0;fields=fields->tl){
struct _tuple9 _tmp268;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Aggrfield*
_tmp26A;struct _tuple9*_tmp267=(struct _tuple9*)fields->hd;_tmp268=*_tmp267;_LL272:
_tmp26A=_tmp268.f1;goto _LL271;_LL271: _tmp269=_tmp268.f2;goto _LL270;_LL270: {void*
_tmp26B=Cyc_Tcutil_rsubstitute(rgn,_tmp264,(void*)_tmp26A->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp26B,_tmp269);if(! Cyc_Tcutil_coerce_arg(te,_tmp269,_tmp26B)){({
struct Cyc_Std_String_pa_struct _tmp270;_tmp270.tag=0;_tmp270.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp269->topt))->v);{struct Cyc_Std_String_pa_struct _tmp26F;_tmp26F.tag=0;_tmp26F.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp26B);{struct Cyc_Std_String_pa_struct
_tmp26E;_tmp26E.tag=0;_tmp26E.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{
struct Cyc_Std_String_pa_struct _tmp26D;_tmp26D.tag=0;_tmp26D.f1=(struct
_tagged_arr)*_tmp26A->name;{void*_tmp26C[4]={& _tmp26D,& _tmp26E,& _tmp26F,& _tmp270};
Cyc_Tcutil_terr(_tmp269->loc,_tag_arr("field %s of struct %s expects type %s != %s",
sizeof(unsigned char),44),_tag_arr(_tmp26C,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
void*_tmp271=(void*)_tmp266;_npop_handler(0);return _tmp271;}}};_pop_region(rgn);}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){{struct _RegionHandle _tmp279=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp279;_push_region(rgn);{void*_tmp27A=Cyc_Tcutil_compress(
ts);struct Cyc_List_List*_tmp27B;void*_tmp27C;_LL274: if((unsigned int)_tmp27A > 3?*((
int*)_tmp27A)== 11: 0){_LL279: _tmp27C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp27A)->f1;goto _LL278;_LL278: _tmp27B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp27A)->f2;goto _LL275;}else{goto _LL276;}_LL276: goto _LL277;_LL275: if(_tmp27C == (
void*)1){({void*_tmp27D[0]={};Cyc_Tcutil_terr(loc,_tag_arr("expecting struct but found union",
sizeof(unsigned char),33),_tag_arr(_tmp27D,sizeof(void*),0));});}{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,_tmp27B);for(0;fields != 0;fields=fields->tl){struct _tuple9 _tmp27F;
struct Cyc_Absyn_Exp*_tmp280;struct Cyc_Absyn_Aggrfield*_tmp281;struct _tuple9*
_tmp27E=(struct _tuple9*)fields->hd;_tmp27F=*_tmp27E;_LL27C: _tmp281=_tmp27F.f1;
goto _LL27B;_LL27B: _tmp280=_tmp27F.f2;goto _LL27A;_LL27A: Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp281->type)),_tmp280);if(! Cyc_Tcutil_coerce_arg(
te,_tmp280,(void*)_tmp281->type)){({struct Cyc_Std_String_pa_struct _tmp285;
_tmp285.tag=0;_tmp285.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp280->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp284;_tmp284.tag=0;_tmp284.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
_tmp281->type);{struct Cyc_Std_String_pa_struct _tmp283;_tmp283.tag=0;_tmp283.f1=(
struct _tagged_arr)*_tmp281->name;{void*_tmp282[3]={& _tmp283,& _tmp284,& _tmp285};
Cyc_Tcutil_terr(_tmp280->loc,_tag_arr("field %s of struct expects type %s != %s",
sizeof(unsigned char),41),_tag_arr(_tmp282,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
goto _LL273;}_LL277:({void*_tmp286[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcAnonStruct: wrong type",sizeof(
unsigned char),25),_tag_arr(_tmp286,sizeof(void*),0));});_LL273:;};_pop_region(
rgn);}return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp287=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp287;_push_region(rgn);{struct _tuple4 _tmp288=({
struct _tuple4 _tmp2A7;_tmp2A7.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp2A7.f2=rgn;
_tmp2A7;});struct Cyc_List_List*_tmp289=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp288,tud->tvs);struct Cyc_List_List*_tmp28A=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp289);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp2A3;_tmp2A3.tag=3;_tmp2A3.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp2A4;
_tmp2A4.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp2A5=_cycalloc(sizeof(*_tmp2A5));_tmp2A5[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp2A6;_tmp2A6.tag=1;_tmp2A6.f1=tud;_tmp2A6.f2=tuf;_tmp2A6;});_tmp2A5;}));
_tmp2A4.targs=_tmp28A;_tmp2A4;});_tmp2A3;});_tmp2A2;});if(topt != 0){void*_tmp28B=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp28C;void*_tmp28D;struct
Cyc_List_List*_tmp28E;_LL27E: if((unsigned int)_tmp28B > 3?*((int*)_tmp28B)== 3: 0){
goto _LL27F;}else{goto _LL280;}_LL280: if((unsigned int)_tmp28B > 3?*((int*)_tmp28B)
== 2: 0){_LL284: _tmp28C=((struct Cyc_Absyn_TunionType_struct*)_tmp28B)->f1;_LL286:
_tmp28E=_tmp28C.targs;goto _LL285;_LL285: _tmp28D=(void*)_tmp28C.rgn;goto _LL281;}
else{goto _LL282;}_LL282: goto _LL283;_LL27F: Cyc_Tcutil_unify(*topt,res);goto _LL27D;
_LL281:{struct Cyc_List_List*a=_tmp28A;for(0;a != 0? _tmp28E != 0: 0;(a=a->tl,_tmp28E=
_tmp28E->tl)){Cyc_Tcutil_unify((void*)a->hd,(void*)_tmp28E->hd);}}if(tuf->typs == 
0? es == 0: 0){e->topt=({struct Cyc_Core_Opt*_tmp28F=_cycalloc(sizeof(*_tmp28F));
_tmp28F->v=(void*)res;_tmp28F;});res=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp290=_cycalloc(sizeof(*_tmp290));_tmp290[0]=({struct Cyc_Absyn_TunionType_struct
_tmp291;_tmp291.tag=2;_tmp291.f1=({struct Cyc_Absyn_TunionInfo _tmp292;_tmp292.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp293=_cycalloc(sizeof(*
_tmp293));_tmp293[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp294;_tmp294.tag=1;
_tmp294.f1=({struct Cyc_Absyn_Tuniondecl**_tmp295=_cycalloc(sizeof(*_tmp295));
_tmp295[0]=tud;_tmp295;});_tmp294;});_tmp293;}));_tmp292.targs=_tmp28A;_tmp292.rgn=(
void*)_tmp28D;_tmp292;});_tmp291;});_tmp290;});Cyc_Tcutil_unchecked_cast(te,e,
res);{void*_tmp296=res;_npop_handler(0);return _tmp296;}}goto _LL27D;_LL283: goto
_LL27D;_LL27D:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0? ts != 0: 0;(es=es->tl,
ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp289,(*((struct _tuple8*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)&
t,e);if(! Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct _tmp29A;
_tmp29A.tag=0;_tmp29A.f1=(struct _tagged_arr)(e->topt == 0? _tag_arr("?",sizeof(
unsigned char),2): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{struct Cyc_Std_String_pa_struct _tmp299;_tmp299.tag=0;
_tmp299.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp298;_tmp298.tag=0;_tmp298.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp297[3]={& _tmp298,& _tmp299,& _tmp29A};Cyc_Tcutil_terr(e->loc,_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof(unsigned char),80),_tag_arr(_tmp297,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp29D=({struct Cyc_Std_String_pa_struct _tmp29C;_tmp29C.tag=0;
_tmp29C.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp29B[1]={&
_tmp29C};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(unsigned char),45),_tag_arr(_tmp29B,sizeof(void*),1));}});_npop_handler(0);
return _tmp29D;}if(ts != 0){void*_tmp2A0=({struct Cyc_Std_String_pa_struct _tmp29F;
_tmp29F.tag=0;_tmp29F.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp29E[1]={& _tmp29F};Cyc_Tcexp_expr_err(te,loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(unsigned char),44),_tag_arr(_tmp29E,sizeof(void*),1));}});_npop_handler(0);
return _tmp2A0;}{void*_tmp2A1=res;_npop_handler(0);return _tmp2A1;}}};_pop_region(
rgn);}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp2A8=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp2A9;struct Cyc_Absyn_Conref*_tmp2AA;void*_tmp2AB;
struct Cyc_List_List*_tmp2AC;struct Cyc_Absyn_AggrInfo _tmp2AD;struct Cyc_List_List*
_tmp2AE;void*_tmp2AF;struct Cyc_List_List*_tmp2B0;_LL288: if((int)_tmp2A8 == 0){
goto _LL289;}else{goto _LL28A;}_LL28A: if((unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)
== 0: 0){goto _LL28B;}else{goto _LL28C;}_LL28C: if((unsigned int)_tmp2A8 > 3?*((int*)
_tmp2A8)== 1: 0){goto _LL28D;}else{goto _LL28E;}_LL28E: if((unsigned int)_tmp2A8 > 3?*((
int*)_tmp2A8)== 2: 0){goto _LL28F;}else{goto _LL290;}_LL290: if((unsigned int)
_tmp2A8 > 3?*((int*)_tmp2A8)== 3: 0){goto _LL291;}else{goto _LL292;}_LL292: if((
unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 4: 0){_LL2B6: _tmp2A9=((struct Cyc_Absyn_PointerType_struct*)
_tmp2A8)->f1;_LL2B7: _tmp2AA=_tmp2A9.nullable;goto _LL293;}else{goto _LL294;}_LL294:
if((unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 5: 0){goto _LL295;}else{goto _LL296;}
_LL296: if((int)_tmp2A8 == 1){goto _LL297;}else{goto _LL298;}_LL298: if((unsigned int)
_tmp2A8 > 3?*((int*)_tmp2A8)== 6: 0){goto _LL299;}else{goto _LL29A;}_LL29A: if((
unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 7: 0){_LL2B8: _tmp2AB=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp2A8)->f1;goto _LL29B;}else{goto _LL29C;}_LL29C: if((
unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 8: 0){goto _LL29D;}else{goto _LL29E;}
_LL29E: if((unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 9: 0){_LL2B9: _tmp2AC=((
struct Cyc_Absyn_TupleType_struct*)_tmp2A8)->f1;goto _LL29F;}else{goto _LL2A0;}
_LL2A0: if((unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 10: 0){_LL2BA: _tmp2AD=((
struct Cyc_Absyn_AggrType_struct*)_tmp2A8)->f1;_LL2BC: _tmp2AF=(void*)_tmp2AD.aggr_info;
goto _LL2BB;_LL2BB: _tmp2AE=_tmp2AD.targs;goto _LL2A1;}else{goto _LL2A2;}_LL2A2: if((
unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 12: 0){goto _LL2A3;}else{goto _LL2A4;}
_LL2A4: if((unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 14: 0){goto _LL2A5;}else{
goto _LL2A6;}_LL2A6: if((unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 11: 0){_LL2BD:
_tmp2B0=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2A8)->f2;goto _LL2A7;}else{
goto _LL2A8;}_LL2A8: if((unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 13: 0){goto
_LL2A9;}else{goto _LL2AA;}_LL2AA: if((unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 
15: 0){goto _LL2AB;}else{goto _LL2AC;}_LL2AC: if((unsigned int)_tmp2A8 > 3?*((int*)
_tmp2A8)== 16: 0){goto _LL2AD;}else{goto _LL2AE;}_LL2AE: if((int)_tmp2A8 == 2){goto
_LL2AF;}else{goto _LL2B0;}_LL2B0: if((unsigned int)_tmp2A8 > 3?*((int*)_tmp2A8)== 
17: 0){goto _LL2B1;}else{goto _LL2B2;}_LL2B2: if((unsigned int)_tmp2A8 > 3?*((int*)
_tmp2A8)== 18: 0){goto _LL2B3;}else{goto _LL2B4;}_LL2B4: if((unsigned int)_tmp2A8 > 3?*((
int*)_tmp2A8)== 19: 0){goto _LL2B5;}else{goto _LL287;}_LL289: return 1;_LL28B: goto
_LL28D;_LL28D: goto _LL28F;_LL28F: goto _LL291;_LL291: return 0;_LL293: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp2AA);_LL295: goto _LL297;
_LL297: goto _LL299;_LL299: return 1;_LL29B: return Cyc_Tcexp_zeroable_type(_tmp2AB);
_LL29D: return 0;_LL29F: for(0;(unsigned int)_tmp2AC;_tmp2AC=_tmp2AC->tl){if(! Cyc_Tcexp_zeroable_type((*((
struct _tuple8*)_tmp2AC->hd)).f2)){return 0;}}return 1;_LL2A1: {struct Cyc_Absyn_Aggrdecl*
_tmp2B1=Cyc_Absyn_get_known_aggrdecl(_tmp2AF);if(_tmp2B1->exist_vars == 0? 1:(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B1->exist_vars))->v
!= 0){return 0;}{struct _RegionHandle _tmp2B2=_new_region("r");struct _RegionHandle*
r=& _tmp2B2;_push_region(r);{struct Cyc_List_List*_tmp2B3=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp2B1->tvs,_tmp2AE);{struct Cyc_List_List*fs=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2B1->fields))->v;for(0;fs != 0;fs=fs->tl){if(
! Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp2B3,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmp2B4=0;_npop_handler(0);return _tmp2B4;}}}{int _tmp2B5=1;
_npop_handler(0);return _tmp2B5;}};_pop_region(r);}}_LL2A3: return 1;_LL2A5: return 1;
_LL2A7: for(0;_tmp2B0 != 0;_tmp2B0=_tmp2B0->tl){if(! Cyc_Tcexp_zeroable_type((void*)((
struct Cyc_Absyn_Aggrfield*)_tmp2B0->hd)->type)){return 0;}}return 1;_LL2A9: return 1;
_LL2AB: return 0;_LL2AD: goto _LL2AF;_LL2AF: goto _LL2B1;_LL2B1: goto _LL2B3;_LL2B3: goto
_LL2B5;_LL2B5:({struct Cyc_Std_String_pa_struct _tmp2B7;_tmp2B7.tag=0;_tmp2B7.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp2B6[1]={& _tmp2B7};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad type `%s' in zeroable type",
sizeof(unsigned char),31),_tag_arr(_tmp2B6,sizeof(void*),1));}});_LL287:;}static
void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*loc,
void**topt,void*t){if(Cyc_Tcutil_bits_only(t)? 1:(allow_zero? Cyc_Tcexp_zeroable_type(
t): 0)){return;}if(topt != 0){void*_tmp2B8=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp2B9;void*_tmp2BA;_LL2BF: if((unsigned int)_tmp2B8 > 3?*((int*)_tmp2B8)== 4: 0){
_LL2C3: _tmp2B9=((struct Cyc_Absyn_PointerType_struct*)_tmp2B8)->f1;_LL2C4: _tmp2BA=(
void*)_tmp2B9.elt_typ;goto _LL2C0;}else{goto _LL2C1;}_LL2C1: goto _LL2C2;_LL2C0: Cyc_Tcutil_unify(
_tmp2BA,t);if(Cyc_Tcutil_bits_only(t)? 1:(allow_zero? Cyc_Tcexp_zeroable_type(t): 0)){
return;}goto _LL2BE;_LL2C2: goto _LL2BE;_LL2BE:;}({struct Cyc_Std_String_pa_struct
_tmp2BD;_tmp2BD.tag=0;_tmp2BD.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp2BC;_tmp2BC.tag=0;_tmp2BC.f1=(struct
_tagged_arr)(allow_zero? _tag_arr("calloc",sizeof(unsigned char),7): _tag_arr("malloc",
sizeof(unsigned char),7));{void*_tmp2BB[2]={& _tmp2BC,& _tmp2BD};Cyc_Tcutil_terr(
loc,_tag_arr("%s cannot be used with type %s\n\t(type needs initialization)",
sizeof(unsigned char),60),_tag_arr(_tmp2BB,sizeof(void*),2));}}});}static void*
Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*ropt,void***t,struct Cyc_Absyn_Exp**e,int is_calloc,int*
is_fat){void*rgn=(void*)2;if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2C2=_cycalloc(sizeof(*_tmp2C2));_tmp2C2[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2C3;_tmp2C3.tag=15;_tmp2C3.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2C4=_cycalloc(sizeof(*_tmp2C4));_tmp2C4->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2C4;}));_tmp2C3;});_tmp2C2;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(ropt));void*_tmp2BE=Cyc_Tcutil_compress(handle_type);void*_tmp2BF;
_LL2C6: if((unsigned int)_tmp2BE > 3?*((int*)_tmp2BE)== 15: 0){_LL2CA: _tmp2BF=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp2BE)->f1;goto _LL2C7;}else{goto _LL2C8;}
_LL2C8: goto _LL2C9;_LL2C7: rgn=_tmp2BF;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL2C5;_LL2C9:({struct Cyc_Std_String_pa_struct _tmp2C1;_tmp2C1.tag=0;_tmp2C1.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type);{void*_tmp2C0[1]={& _tmp2C1};
Cyc_Tcutil_terr(ropt->loc,_tag_arr("expecting region_t type but found %s",
sizeof(unsigned char),37),_tag_arr(_tmp2C0,sizeof(void*),1));}});goto _LL2C5;
_LL2C5:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_t,*e);{void*elt_type;struct
Cyc_Absyn_Exp*num_elts;int one_elt;if(is_calloc){if(*t == 0){({void*_tmp2C5[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("calloc with empty type",
sizeof(unsigned char),23),_tag_arr(_tmp2C5,sizeof(void*),0));});}elt_type=*((
void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp2C6=(void*)(*e)->r;void*_tmp2C7;struct Cyc_List_List*
_tmp2C8;struct Cyc_List_List _tmp2C9;struct Cyc_List_List*_tmp2CA;struct Cyc_List_List
_tmp2CB;struct Cyc_List_List*_tmp2CC;struct Cyc_Absyn_Exp*_tmp2CD;struct Cyc_Absyn_Exp*
_tmp2CE;void*_tmp2CF;_LL2CC: if(*((int*)_tmp2C6)== 16){_LL2D2: _tmp2C7=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2C6)->f1;goto _LL2CD;}else{goto _LL2CE;}
_LL2CE: if(*((int*)_tmp2C6)== 3){_LL2D8: _tmp2CF=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2C6)->f1;if((int)_tmp2CF == 1){goto _LL2D3;}else{goto _LL2D0;}_LL2D3: _tmp2C8=((
struct Cyc_Absyn_Primop_e_struct*)_tmp2C6)->f2;if(_tmp2C8 == 0){goto _LL2D0;}else{
_tmp2C9=*_tmp2C8;_LL2D7: _tmp2CE=(struct Cyc_Absyn_Exp*)_tmp2C9.hd;goto _LL2D4;
_LL2D4: _tmp2CA=_tmp2C9.tl;if(_tmp2CA == 0){goto _LL2D0;}else{_tmp2CB=*_tmp2CA;
_LL2D6: _tmp2CD=(struct Cyc_Absyn_Exp*)_tmp2CB.hd;goto _LL2D5;_LL2D5: _tmp2CC=
_tmp2CB.tl;if(_tmp2CC == 0){goto _LL2CF;}else{goto _LL2D0;}}}}else{goto _LL2D0;}
_LL2D0: goto _LL2D1;_LL2CD: elt_type=_tmp2C7;*t=(void**)({void**_tmp2D0=_cycalloc(
sizeof(*_tmp2D0));_tmp2D0[0]=elt_type;_tmp2D0;});num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_t,num_elts);one_elt=1;goto _LL2CB;
_LL2CF:{struct _tuple0 _tmp2D2=({struct _tuple0 _tmp2D1;_tmp2D1.f1=(void*)_tmp2CE->r;
_tmp2D1.f2=(void*)_tmp2CD->r;_tmp2D1;});void*_tmp2D3;void*_tmp2D4;void*_tmp2D5;
void*_tmp2D6;_LL2DA: _LL2E0: _tmp2D3=_tmp2D2.f1;if(*((int*)_tmp2D3)== 16){_LL2E1:
_tmp2D4=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2D3)->f1;goto _LL2DB;}
else{goto _LL2DC;}_LL2DC: _LL2E2: _tmp2D5=_tmp2D2.f2;if(*((int*)_tmp2D5)== 16){
_LL2E3: _tmp2D6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2D5)->f1;goto
_LL2DD;}else{goto _LL2DE;}_LL2DE: goto _LL2DF;_LL2DB: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp2D4);elt_type=_tmp2D4;*t=(void**)({void**_tmp2D7=_cycalloc(sizeof(*
_tmp2D7));_tmp2D7[0]=elt_type;_tmp2D7;});num_elts=_tmp2CD;one_elt=0;goto _LL2D9;
_LL2DD: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp2D6);elt_type=_tmp2D6;*t=(void**)({
void**_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8[0]=elt_type;_tmp2D8;});
num_elts=_tmp2CE;one_elt=0;goto _LL2D9;_LL2DF: goto No_sizeof;_LL2D9:;}goto _LL2CB;
_LL2D1: No_sizeof: elt_type=Cyc_Absyn_uchar_t;*t=(void**)({void**_tmp2D9=_cycalloc(
sizeof(*_tmp2D9));_tmp2D9[0]=elt_type;_tmp2D9;});num_elts=*e;one_elt=0;goto
_LL2CB;_LL2CB:;}*e=num_elts;*is_fat=! one_elt;{void*_tmp2DA=elt_type;struct Cyc_Absyn_AggrInfo
_tmp2DB;void*_tmp2DC;struct Cyc_Absyn_Aggrdecl**_tmp2DD;struct Cyc_Absyn_Aggrdecl*
_tmp2DE;_LL2E5: if((unsigned int)_tmp2DA > 3?*((int*)_tmp2DA)== 10: 0){_LL2E9:
_tmp2DB=((struct Cyc_Absyn_AggrType_struct*)_tmp2DA)->f1;_LL2EA: _tmp2DC=(void*)
_tmp2DB.aggr_info;if(*((int*)_tmp2DC)== 1){_LL2EB: _tmp2DD=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp2DC)->f1;_tmp2DE=*_tmp2DD;goto _LL2E6;}else{goto _LL2E7;}}else{goto _LL2E7;}
_LL2E7: goto _LL2E8;_LL2E6: if(_tmp2DE->exist_vars != 0?(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2DE->exist_vars))->v != 0: 0){({void*_tmp2DF[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("malloc with existential types not yet implemented",
sizeof(unsigned char),50),_tag_arr(_tmp2DF,sizeof(void*),0));});}goto _LL2E4;
_LL2E8: goto _LL2E4;_LL2E4:;}{void*(*_tmp2E0)(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq)=Cyc_Absyn_at_typ;if(topt != 0){void*_tmp2E1=Cyc_Tcutil_compress(*topt);struct
Cyc_Absyn_PtrInfo _tmp2E2;struct Cyc_Absyn_Conref*_tmp2E3;_LL2ED: if((unsigned int)
_tmp2E1 > 3?*((int*)_tmp2E1)== 4: 0){_LL2F1: _tmp2E2=((struct Cyc_Absyn_PointerType_struct*)
_tmp2E1)->f1;_LL2F2: _tmp2E3=_tmp2E2.nullable;goto _LL2EE;}else{goto _LL2EF;}_LL2EF:
goto _LL2F0;_LL2EE: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp2E3)){_tmp2E0=Cyc_Absyn_star_typ;}goto _LL2EC;_LL2F0: goto _LL2EC;_LL2EC:;}if(!
one_elt){_tmp2E0=Cyc_Absyn_tagged_typ;}return _tmp2E0(elt_type,rgn,Cyc_Absyn_empty_tqual());}}}
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
te,s),s,1);Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));while(1){
void*_tmp2E4=(void*)s->r;struct Cyc_Absyn_Exp*_tmp2E5;struct Cyc_Absyn_Stmt*
_tmp2E6;struct Cyc_Absyn_Stmt*_tmp2E7;struct Cyc_Absyn_Stmt*_tmp2E8;struct Cyc_Absyn_Decl*
_tmp2E9;_LL2F4: if((unsigned int)_tmp2E4 > 1?*((int*)_tmp2E4)== 0: 0){_LL2FC:
_tmp2E5=((struct Cyc_Absyn_Exp_s_struct*)_tmp2E4)->f1;goto _LL2F5;}else{goto _LL2F6;}
_LL2F6: if((unsigned int)_tmp2E4 > 1?*((int*)_tmp2E4)== 1: 0){_LL2FE: _tmp2E7=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2E4)->f1;goto _LL2FD;_LL2FD: _tmp2E6=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2E4)->f2;goto _LL2F7;}else{goto _LL2F8;}_LL2F8: if((unsigned int)_tmp2E4 > 1?*((
int*)_tmp2E4)== 12: 0){_LL300: _tmp2E9=((struct Cyc_Absyn_Decl_s_struct*)_tmp2E4)->f1;
goto _LL2FF;_LL2FF: _tmp2E8=((struct Cyc_Absyn_Decl_s_struct*)_tmp2E4)->f2;goto
_LL2F9;}else{goto _LL2FA;}_LL2FA: goto _LL2FB;_LL2F5: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2E5->topt))->v;_LL2F7: s=_tmp2E6;continue;_LL2F9: s=_tmp2E8;
continue;_LL2FB: return({void*_tmp2EA[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("statement expression must end with expression",
sizeof(unsigned char),46),_tag_arr(_tmp2EA,sizeof(void*),0));});_LL2F3:;}}static
void*Cyc_Tcexp_tcCodegen(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Fndecl*fd){return({void*_tmp2EB[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("tcCodegen",sizeof(unsigned char),10),_tag_arr(_tmp2EB,sizeof(
void*),0));});}static void*Cyc_Tcexp_tcFill(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){return({void*_tmp2EC[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("tcFill",sizeof(unsigned char),7),_tag_arr(_tmp2EC,sizeof(void*),
0));});}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2F2;_tmp2F2.tag=15;_tmp2F2.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2F3;}));_tmp2F2;});_tmp2F1;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(rgn_handle));void*_tmp2ED=Cyc_Tcutil_compress(handle_type);void*
_tmp2EE;_LL302: if((unsigned int)_tmp2ED > 3?*((int*)_tmp2ED)== 15: 0){_LL306:
_tmp2EE=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2ED)->f1;goto _LL303;}
else{goto _LL304;}_LL304: goto _LL305;_LL303: rgn=_tmp2EE;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL301;_LL305:({struct Cyc_Std_String_pa_struct _tmp2F0;_tmp2F0.tag=
0;_tmp2F0.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type);{void*
_tmp2EF[1]={& _tmp2F0};Cyc_Tcutil_terr(rgn_handle->loc,_tag_arr("expecting region_t type but found %s",
sizeof(unsigned char),37),_tag_arr(_tmp2EF,sizeof(void*),1));}});goto _LL301;
_LL301:;}{void*_tmp2F4=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp2F5;struct Cyc_Absyn_Exp*
_tmp2F6;struct Cyc_Absyn_Vardecl*_tmp2F7;struct Cyc_List_List*_tmp2F8;struct Cyc_Core_Opt*
_tmp2F9;struct Cyc_List_List*_tmp2FA;void*_tmp2FB;struct _tagged_arr _tmp2FC;_LL308:
if(*((int*)_tmp2F4)== 27){_LL314: _tmp2F7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2F4)->f1;goto _LL313;_LL313: _tmp2F6=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2F4)->f2;goto _LL312;_LL312: _tmp2F5=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2F4)->f3;goto _LL309;}else{goto _LL30A;}_LL30A: if(*((int*)_tmp2F4)== 34){
_LL316: _tmp2F9=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp2F4)->f1;goto _LL315;
_LL315: _tmp2F8=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp2F4)->f2;goto _LL30B;}
else{goto _LL30C;}_LL30C: if(*((int*)_tmp2F4)== 26){_LL317: _tmp2FA=((struct Cyc_Absyn_Array_e_struct*)
_tmp2F4)->f1;goto _LL30D;}else{goto _LL30E;}_LL30E: if(*((int*)_tmp2F4)== 0){_LL318:
_tmp2FB=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp2F4)->f1;if((unsigned int)
_tmp2FB > 1?*((int*)_tmp2FB)== 5: 0){_LL319: _tmp2FC=((struct Cyc_Absyn_String_c_struct*)
_tmp2FB)->f1;goto _LL30F;}else{goto _LL310;}}else{goto _LL310;}_LL310: goto _LL311;
_LL309: {void*_tmp2FD=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*_tmp2FE=Cyc_Tcutil_compress(
_tmp2FD);struct Cyc_Absyn_Tqual _tmp2FF;void*_tmp300;_LL31B: if((unsigned int)
_tmp2FE > 3?*((int*)_tmp2FE)== 7: 0){_LL320: _tmp300=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp2FE)->f1;goto _LL31F;_LL31F: _tmp2FF=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2FE)->f2;goto _LL31C;}else{goto _LL31D;}_LL31D: goto _LL31E;_LL31C: {void*b=Cyc_Tcutil_is_const_exp(
te,_tmp2F6)?(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp305=_cycalloc(sizeof(*
_tmp305));_tmp305[0]=({struct Cyc_Absyn_Upper_b_struct _tmp306;_tmp306.tag=0;
_tmp306.f1=_tmp2F6;_tmp306;});_tmp305;}):(void*)0;void*res_typ=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp302=_cycalloc(sizeof(*_tmp302));_tmp302[0]=({
struct Cyc_Absyn_PointerType_struct _tmp303;_tmp303.tag=4;_tmp303.f1=({struct Cyc_Absyn_PtrInfo
_tmp304;_tmp304.elt_typ=(void*)_tmp300;_tmp304.rgn_typ=(void*)rgn;_tmp304.nullable=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp304.tq=_tmp2FF;
_tmp304.bounds=Cyc_Absyn_new_conref(b);_tmp304;});_tmp303;});_tmp302;});if(topt
!= 0){if(! Cyc_Tcutil_unify(*topt,res_typ)? Cyc_Tcutil_silent_castable(te,loc,
res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp301=_cycalloc(sizeof(*_tmp301));
_tmp301->v=(void*)res_typ;_tmp301;});Cyc_Tcutil_unchecked_cast(te,e,*topt);
res_typ=*topt;}}return res_typ;}_LL31E:({void*_tmp307[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcNew: comprehension returned non-array type",
sizeof(unsigned char),45),_tag_arr(_tmp307,sizeof(void*),0));});_LL31A:;}_LL30B:(
void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp308=_cycalloc(
sizeof(*_tmp308));_tmp308[0]=({struct Cyc_Absyn_Array_e_struct _tmp309;_tmp309.tag=
26;_tmp309.f1=_tmp2F8;_tmp309;});_tmp308;})));_tmp2FA=_tmp2F8;goto _LL30D;_LL30D: {
void**elt_typ_opt=0;if(topt != 0){void*_tmp30A=Cyc_Tcutil_compress(*topt);struct
Cyc_Absyn_PtrInfo _tmp30B;struct Cyc_Absyn_Tqual _tmp30C;void*_tmp30D;void**_tmp30E;
_LL322: if((unsigned int)_tmp30A > 3?*((int*)_tmp30A)== 4: 0){_LL326: _tmp30B=((
struct Cyc_Absyn_PointerType_struct*)_tmp30A)->f1;_LL328: _tmp30D=(void*)_tmp30B.elt_typ;
_tmp30E=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp30A)->f1).elt_typ;
goto _LL327;_LL327: _tmp30C=_tmp30B.tq;goto _LL323;}else{goto _LL324;}_LL324: goto
_LL325;_LL323: elt_typ_opt=(void**)_tmp30E;goto _LL321;_LL325: goto _LL321;_LL321:;}{
void*_tmp30F=Cyc_Tcexp_tcExpNoPromote(te,elt_typ_opt,e1);void*res_typ;{void*
_tmp310=Cyc_Tcutil_compress(_tmp30F);struct Cyc_Absyn_Exp*_tmp311;struct Cyc_Absyn_Tqual
_tmp312;void*_tmp313;_LL32A: if((unsigned int)_tmp310 > 3?*((int*)_tmp310)== 7: 0){
_LL330: _tmp313=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp310)->f1;goto
_LL32F;_LL32F: _tmp312=((struct Cyc_Absyn_ArrayType_struct*)_tmp310)->f2;goto
_LL32E;_LL32E: _tmp311=((struct Cyc_Absyn_ArrayType_struct*)_tmp310)->f3;goto
_LL32B;}else{goto _LL32C;}_LL32C: goto _LL32D;_LL32B: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314[0]=({struct Cyc_Absyn_PointerType_struct
_tmp315;_tmp315.tag=4;_tmp315.f1=({struct Cyc_Absyn_PtrInfo _tmp316;_tmp316.elt_typ=(
void*)_tmp313;_tmp316.rgn_typ=(void*)rgn;_tmp316.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp316.tq=_tmp312;_tmp316.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp317=_cycalloc(sizeof(*_tmp317));
_tmp317[0]=({struct Cyc_Absyn_Upper_b_struct _tmp318;_tmp318.tag=0;_tmp318.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp311);_tmp318;});_tmp317;}));_tmp316;});
_tmp315;});_tmp314;});if(topt != 0){if(! Cyc_Tcutil_unify(*topt,res_typ)? Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp319=_cycalloc(sizeof(*
_tmp319));_tmp319->v=(void*)res_typ;_tmp319;});Cyc_Tcutil_unchecked_cast(te,e,*
topt);res_typ=*topt;}}goto _LL329;_LL32D:({void*_tmp31A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof(unsigned char),50),_tag_arr(_tmp31A,sizeof(void*),0));});_LL329:;}return
res_typ;}}_LL30F: {void*_tmp31B=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,rgn,Cyc_Absyn_const_tqual(),(
void*)0);void*_tmp31C=Cyc_Tcexp_tcExp(te,(void**)& _tmp31B,e1);return Cyc_Absyn_atb_typ(
_tmp31C,rgn,Cyc_Absyn_empty_tqual(),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp31E;_tmp31E.tag=0;_tmp31E.f1=Cyc_Absyn_uint_exp(1,0);_tmp31E;});_tmp31D;}));}
_LL311: {void**topt2=0;if(topt != 0){void*_tmp31F=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp320;struct Cyc_Absyn_Conref*_tmp321;void*_tmp322;void**
_tmp323;_LL332: if((unsigned int)_tmp31F > 3?*((int*)_tmp31F)== 4: 0){_LL338:
_tmp320=((struct Cyc_Absyn_PointerType_struct*)_tmp31F)->f1;_LL33A: _tmp322=(void*)
_tmp320.elt_typ;_tmp323=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp31F)->f1).elt_typ;
goto _LL339;_LL339: _tmp321=_tmp320.nullable;goto _LL333;}else{goto _LL334;}_LL334:
if((unsigned int)_tmp31F > 3?*((int*)_tmp31F)== 2: 0){goto _LL335;}else{goto _LL336;}
_LL336: goto _LL337;_LL333: topt2=(void**)_tmp323;goto _LL331;_LL335: topt2=({void**
_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324[0]=*topt;_tmp324;});goto _LL331;
_LL337: goto _LL331;_LL331:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp326=_cycalloc(sizeof(*_tmp326));
_tmp326[0]=({struct Cyc_Absyn_PointerType_struct _tmp327;_tmp327.tag=4;_tmp327.f1=({
struct Cyc_Absyn_PtrInfo _tmp328;_tmp328.elt_typ=(void*)telt;_tmp328.rgn_typ=(void*)
rgn;_tmp328.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp328.tq=Cyc_Absyn_empty_tqual();_tmp328.bounds=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp32A;_tmp32A.tag=0;_tmp32A.f1=Cyc_Absyn_uint_exp(
1,0);_tmp32A;});_tmp329;}));_tmp328;});_tmp327;});_tmp326;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ)? Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt):
0){e->topt=({struct Cyc_Core_Opt*_tmp325=_cycalloc(sizeof(*_tmp325));_tmp325->v=(
void*)res_typ;_tmp325;});Cyc_Tcutil_unchecked_cast(te,e,*topt);res_typ=*topt;}}
return res_typ;}}_LL307:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp32B=t;struct Cyc_Absyn_Exp*_tmp32C;struct Cyc_Absyn_Tqual
_tmp32D;void*_tmp32E;_LL33C: if((unsigned int)_tmp32B > 3?*((int*)_tmp32B)== 7: 0){
_LL342: _tmp32E=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp32B)->f1;goto
_LL341;_LL341: _tmp32D=((struct Cyc_Absyn_ArrayType_struct*)_tmp32B)->f2;goto
_LL340;_LL340: _tmp32C=((struct Cyc_Absyn_ArrayType_struct*)_tmp32B)->f3;goto
_LL33D;}else{goto _LL33E;}_LL33E: goto _LL33F;_LL33D: {void*_tmp330;struct _tuple6
_tmp32F=Cyc_Tcutil_addressof_props(te,e);_LL344: _tmp330=_tmp32F.f2;goto _LL343;
_LL343: {void*_tmp331=_tmp32C == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp333;_tmp333.tag=0;_tmp333.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp32C);
_tmp333;});_tmp332;});t=Cyc_Absyn_atb_typ(_tmp32E,_tmp330,_tmp32D,_tmp331);(void*)(((
struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);return t;}}_LL33F: return t;
_LL33B:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,
struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);void*_tmp334=(
void*)e->r;void*_tmp335;_LL346: if(*((int*)_tmp334)== 26){goto _LL347;}else{goto
_LL348;}_LL348: if(*((int*)_tmp334)== 27){goto _LL349;}else{goto _LL34A;}_LL34A: if(*((
int*)_tmp334)== 0){_LL34E: _tmp335=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp334)->f1;if((unsigned int)_tmp335 > 1?*((int*)_tmp335)== 5: 0){goto _LL34B;}
else{goto _LL34C;}}else{goto _LL34C;}_LL34C: goto _LL34D;_LL347: goto _LL349;_LL349:
goto _LL34B;_LL34B: return t;_LL34D: t=Cyc_Tcutil_compress(t);{void*_tmp336=t;struct
Cyc_Absyn_Exp*_tmp337;struct Cyc_Absyn_Tqual _tmp338;void*_tmp339;_LL350: if((
unsigned int)_tmp336 > 3?*((int*)_tmp336)== 7: 0){_LL356: _tmp339=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp336)->f1;goto _LL355;_LL355: _tmp338=((struct Cyc_Absyn_ArrayType_struct*)
_tmp336)->f2;goto _LL354;_LL354: _tmp337=((struct Cyc_Absyn_ArrayType_struct*)
_tmp336)->f3;goto _LL351;}else{goto _LL352;}_LL352: goto _LL353;_LL351: {void*
_tmp33B;struct _tuple6 _tmp33A=Cyc_Tcutil_addressof_props(te,e);_LL358: _tmp33B=
_tmp33A.f2;goto _LL357;_LL357: {void*b=_tmp337 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp33D;_tmp33D.tag=0;_tmp33D.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp337);
_tmp33D;});_tmp33C;});t=Cyc_Absyn_atb_typ(_tmp339,_tmp33B,_tmp338,b);Cyc_Tcutil_unchecked_cast(
te,e,t);return t;}}_LL353: return t;_LL34F:;}_LL345:;}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp33E=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp33F;_LL35A: if(*((int*)_tmp33E)== 11){_LL35E: _tmp33F=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp33E)->f1;goto _LL35B;}else{goto _LL35C;}
_LL35C: goto _LL35D;_LL35B: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp33F);(void*)(((struct
Cyc_Core_Opt*)_check_null(_tmp33F->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp33F->topt))->v));e->topt=_tmp33F->topt;
goto _LL359;_LL35D: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{void*_tmp340=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp341;struct Cyc_Absyn_Conref*
_tmp342;struct Cyc_Absyn_Tqual _tmp343;struct Cyc_Absyn_Conref*_tmp344;void*_tmp345;
void*_tmp346;_LL360: if((unsigned int)_tmp340 > 3?*((int*)_tmp340)== 4: 0){_LL364:
_tmp341=((struct Cyc_Absyn_PointerType_struct*)_tmp340)->f1;_LL369: _tmp346=(void*)
_tmp341.elt_typ;goto _LL368;_LL368: _tmp345=(void*)_tmp341.rgn_typ;goto _LL367;
_LL367: _tmp344=_tmp341.nullable;goto _LL366;_LL366: _tmp343=_tmp341.tq;goto _LL365;
_LL365: _tmp342=_tmp341.bounds;goto _LL361;}else{goto _LL362;}_LL362: goto _LL363;
_LL361:{void*_tmp347=Cyc_Tcutil_compress(_tmp346);struct Cyc_Absyn_FnInfo _tmp348;
struct Cyc_List_List*_tmp349;struct Cyc_List_List*_tmp34A;struct Cyc_Absyn_VarargInfo*
_tmp34B;int _tmp34C;struct Cyc_List_List*_tmp34D;void*_tmp34E;struct Cyc_Core_Opt*
_tmp34F;struct Cyc_List_List*_tmp350;_LL36B: if((unsigned int)_tmp347 > 3?*((int*)
_tmp347)== 8: 0){_LL36F: _tmp348=((struct Cyc_Absyn_FnType_struct*)_tmp347)->f1;
_LL377: _tmp350=_tmp348.tvars;goto _LL376;_LL376: _tmp34F=_tmp348.effect;goto _LL375;
_LL375: _tmp34E=(void*)_tmp348.ret_typ;goto _LL374;_LL374: _tmp34D=_tmp348.args;
goto _LL373;_LL373: _tmp34C=_tmp348.c_varargs;goto _LL372;_LL372: _tmp34B=_tmp348.cyc_varargs;
goto _LL371;_LL371: _tmp34A=_tmp348.rgn_po;goto _LL370;_LL370: _tmp349=_tmp348.attributes;
goto _LL36C;}else{goto _LL36D;}_LL36D: goto _LL36E;_LL36C: if(_tmp350 != 0){struct
_RegionHandle _tmp351=_new_region("rgn");struct _RegionHandle*rgn=& _tmp351;
_push_region(rgn);{struct _tuple4 _tmp352=({struct _tuple4 _tmp35C;_tmp35C.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp35C.f2=rgn;_tmp35C;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp352,_tmp350);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp359=_cycalloc(sizeof(*_tmp359));_tmp359[0]=({struct Cyc_Absyn_FnType_struct
_tmp35A;_tmp35A.tag=8;_tmp35A.f1=({struct Cyc_Absyn_FnInfo _tmp35B;_tmp35B.tvars=0;
_tmp35B.effect=_tmp34F;_tmp35B.ret_typ=(void*)_tmp34E;_tmp35B.args=_tmp34D;
_tmp35B.c_varargs=_tmp34C;_tmp35B.cyc_varargs=_tmp34B;_tmp35B.rgn_po=_tmp34A;
_tmp35B.attributes=_tmp349;_tmp35B;});_tmp35A;});_tmp359;}));void*new_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356[0]=({
struct Cyc_Absyn_PointerType_struct _tmp357;_tmp357.tag=4;_tmp357.f1=({struct Cyc_Absyn_PtrInfo
_tmp358;_tmp358.elt_typ=(void*)ftyp;_tmp358.rgn_typ=(void*)_tmp345;_tmp358.nullable=
_tmp344;_tmp358.tq=_tmp343;_tmp358.bounds=_tmp342;_tmp358;});_tmp357;});_tmp356;});
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Instantiate_e_struct*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353[
0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp354;_tmp354.tag=12;_tmp354.f1=inner;
_tmp354.f2=ts;_tmp354;});_tmp353;})));e->topt=({struct Cyc_Core_Opt*_tmp355=
_cycalloc(sizeof(*_tmp355));_tmp355->v=(void*)new_typ;_tmp355;});};_pop_region(
rgn);}goto _LL36A;_LL36E: goto _LL36A;_LL36A:;}goto _LL35F;_LL363: goto _LL35F;_LL35F:;}
goto _LL359;_LL359:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp35D=(void*)e->r;struct
Cyc_Absyn_Exp*_tmp35E;struct _tuple1*_tmp35F;struct Cyc_List_List*_tmp360;struct
Cyc_Absyn_Exp*_tmp361;struct Cyc_List_List*_tmp362;struct Cyc_Core_Opt*_tmp363;
void*_tmp364;void*_tmp365;struct _tuple1*_tmp366;struct Cyc_List_List*_tmp367;void*
_tmp368;void*_tmp369;struct Cyc_Absyn_Exp*_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;
struct Cyc_Core_Opt*_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;struct Cyc_Absyn_Exp*
_tmp36E;struct Cyc_Absyn_Exp*_tmp36F;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Exp*
_tmp371;struct Cyc_Absyn_Exp*_tmp372;struct Cyc_Absyn_VarargCallInfo*_tmp373;
struct Cyc_Absyn_VarargCallInfo**_tmp374;struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Exp*
_tmp376;struct Cyc_Absyn_Exp*_tmp377;struct Cyc_List_List*_tmp378;struct Cyc_Absyn_Exp*
_tmp379;struct Cyc_Absyn_Exp*_tmp37A;void*_tmp37B;struct Cyc_Absyn_Exp*_tmp37C;
struct Cyc_Absyn_Exp*_tmp37D;struct Cyc_Absyn_Exp*_tmp37E;struct Cyc_Absyn_Exp*
_tmp37F;void*_tmp380;void*_tmp381;void*_tmp382;struct Cyc_Absyn_Exp*_tmp383;
struct _tagged_arr*_tmp384;struct Cyc_Absyn_Exp*_tmp385;struct _tagged_arr*_tmp386;
struct Cyc_Absyn_Exp*_tmp387;struct Cyc_Absyn_Exp*_tmp388;struct Cyc_Absyn_Exp*
_tmp389;struct Cyc_List_List*_tmp38A;struct Cyc_List_List*_tmp38B;struct _tuple2*
_tmp38C;struct Cyc_List_List*_tmp38D;struct Cyc_Absyn_Stmt*_tmp38E;struct Cyc_Absyn_Fndecl*
_tmp38F;struct Cyc_Absyn_Exp*_tmp390;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_Absyn_Exp*
_tmp392;struct Cyc_Absyn_Vardecl*_tmp393;struct Cyc_Absyn_Aggrdecl*_tmp394;struct
Cyc_Absyn_Aggrdecl**_tmp395;struct Cyc_List_List*_tmp396;struct _tuple1*_tmp397;
struct _tuple1**_tmp398;struct Cyc_List_List*_tmp399;void*_tmp39A;struct Cyc_Absyn_Tunionfield*
_tmp39B;struct Cyc_Absyn_Tuniondecl*_tmp39C;struct Cyc_List_List*_tmp39D;struct Cyc_Absyn_Enumfield*
_tmp39E;struct Cyc_Absyn_Enumdecl*_tmp39F;struct _tuple1*_tmp3A0;struct _tuple1**
_tmp3A1;struct Cyc_Absyn_Enumfield*_tmp3A2;void*_tmp3A3;struct _tuple1*_tmp3A4;
struct _tuple1**_tmp3A5;struct Cyc_Absyn_MallocInfo _tmp3A6;int _tmp3A7;int*_tmp3A8;
struct Cyc_Absyn_Exp*_tmp3A9;struct Cyc_Absyn_Exp**_tmp3AA;void**_tmp3AB;void***
_tmp3AC;struct Cyc_Absyn_Exp*_tmp3AD;int _tmp3AE;_LL379: if(*((int*)_tmp35D)== 11){
_LL3C5: _tmp35E=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp35D)->f1;goto _LL37A;}
else{goto _LL37B;}_LL37B: if(*((int*)_tmp35D)== 2){_LL3C6: _tmp35F=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp35D)->f1;goto _LL37C;}else{goto _LL37D;}_LL37D: if(*((int*)_tmp35D)== 8){_LL3C8:
_tmp361=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp35D)->f1;goto _LL3C7;_LL3C7:
_tmp360=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp35D)->f2;goto _LL37E;}else{
goto _LL37F;}_LL37F: if(*((int*)_tmp35D)== 34){_LL3CA: _tmp363=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp35D)->f1;goto _LL3C9;_LL3C9: _tmp362=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp35D)->f2;goto _LL380;}else{goto _LL381;}_LL381: if(*((int*)_tmp35D)== 0){_LL3CB:
_tmp364=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp35D)->f1;goto _LL382;}else{
goto _LL383;}_LL383: if(*((int*)_tmp35D)== 1){_LL3CD: _tmp366=((struct Cyc_Absyn_Var_e_struct*)
_tmp35D)->f1;goto _LL3CC;_LL3CC: _tmp365=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp35D)->f2;goto _LL384;}else{goto _LL385;}_LL385: if(*((int*)_tmp35D)== 3){_LL3CF:
_tmp368=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp35D)->f1;goto _LL3CE;_LL3CE:
_tmp367=((struct Cyc_Absyn_Primop_e_struct*)_tmp35D)->f2;goto _LL386;}else{goto
_LL387;}_LL387: if(*((int*)_tmp35D)== 5){_LL3D1: _tmp36A=((struct Cyc_Absyn_Increment_e_struct*)
_tmp35D)->f1;goto _LL3D0;_LL3D0: _tmp369=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp35D)->f2;goto _LL388;}else{goto _LL389;}_LL389: if(*((int*)_tmp35D)== 4){_LL3D4:
_tmp36D=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp35D)->f1;goto _LL3D3;_LL3D3:
_tmp36C=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp35D)->f2;goto _LL3D2;_LL3D2:
_tmp36B=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp35D)->f3;goto _LL38A;}else{goto
_LL38B;}_LL38B: if(*((int*)_tmp35D)== 6){_LL3D7: _tmp370=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp35D)->f1;goto _LL3D6;_LL3D6: _tmp36F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp35D)->f2;goto _LL3D5;_LL3D5: _tmp36E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp35D)->f3;goto _LL38C;}else{goto _LL38D;}_LL38D: if(*((int*)_tmp35D)== 7){_LL3D9:
_tmp372=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp35D)->f1;goto _LL3D8;_LL3D8:
_tmp371=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp35D)->f2;goto _LL38E;}else{goto
_LL38F;}_LL38F: if(*((int*)_tmp35D)== 9){_LL3DC: _tmp376=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp35D)->f1;goto _LL3DB;_LL3DB: _tmp375=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp35D)->f2;goto _LL3DA;_LL3DA: _tmp373=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp35D)->f3;_tmp374=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp35D)->f3;goto _LL390;}else{goto _LL391;}_LL391: if(*((int*)_tmp35D)== 10){
_LL3DD: _tmp377=((struct Cyc_Absyn_Throw_e_struct*)_tmp35D)->f1;goto _LL392;}else{
goto _LL393;}_LL393: if(*((int*)_tmp35D)== 12){_LL3DF: _tmp379=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp35D)->f1;goto _LL3DE;_LL3DE: _tmp378=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp35D)->f2;goto _LL394;}else{goto _LL395;}_LL395: if(*((int*)_tmp35D)== 13){
_LL3E1: _tmp37B=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp35D)->f1;goto _LL3E0;
_LL3E0: _tmp37A=((struct Cyc_Absyn_Cast_e_struct*)_tmp35D)->f2;goto _LL396;}else{
goto _LL397;}_LL397: if(*((int*)_tmp35D)== 14){_LL3E2: _tmp37C=((struct Cyc_Absyn_Address_e_struct*)
_tmp35D)->f1;goto _LL398;}else{goto _LL399;}_LL399: if(*((int*)_tmp35D)== 15){
_LL3E4: _tmp37E=((struct Cyc_Absyn_New_e_struct*)_tmp35D)->f1;goto _LL3E3;_LL3E3:
_tmp37D=((struct Cyc_Absyn_New_e_struct*)_tmp35D)->f2;goto _LL39A;}else{goto _LL39B;}
_LL39B: if(*((int*)_tmp35D)== 17){_LL3E5: _tmp37F=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp35D)->f1;goto _LL39C;}else{goto _LL39D;}_LL39D: if(*((int*)_tmp35D)== 16){
_LL3E6: _tmp380=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp35D)->f1;goto
_LL39E;}else{goto _LL39F;}_LL39F: if(*((int*)_tmp35D)== 18){_LL3E8: _tmp382=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp35D)->f1;goto _LL3E7;_LL3E7: _tmp381=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp35D)->f2;goto _LL3A0;}else{goto _LL3A1;}
_LL3A1: if(*((int*)_tmp35D)== 19){goto _LL3A2;}else{goto _LL3A3;}_LL3A3: if(*((int*)
_tmp35D)== 20){_LL3E9: _tmp383=((struct Cyc_Absyn_Deref_e_struct*)_tmp35D)->f1;
goto _LL3A4;}else{goto _LL3A5;}_LL3A5: if(*((int*)_tmp35D)== 21){_LL3EB: _tmp385=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp35D)->f1;goto _LL3EA;_LL3EA: _tmp384=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp35D)->f2;goto _LL3A6;}else{goto _LL3A7;}
_LL3A7: if(*((int*)_tmp35D)== 22){_LL3ED: _tmp387=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp35D)->f1;goto _LL3EC;_LL3EC: _tmp386=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp35D)->f2;goto _LL3A8;}else{goto _LL3A9;}_LL3A9: if(*((int*)_tmp35D)== 23){
_LL3EF: _tmp389=((struct Cyc_Absyn_Subscript_e_struct*)_tmp35D)->f1;goto _LL3EE;
_LL3EE: _tmp388=((struct Cyc_Absyn_Subscript_e_struct*)_tmp35D)->f2;goto _LL3AA;}
else{goto _LL3AB;}_LL3AB: if(*((int*)_tmp35D)== 24){_LL3F0: _tmp38A=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp35D)->f1;goto _LL3AC;}else{goto _LL3AD;}_LL3AD: if(*((int*)_tmp35D)== 25){
_LL3F2: _tmp38C=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp35D)->f1;goto _LL3F1;
_LL3F1: _tmp38B=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp35D)->f2;goto _LL3AE;}
else{goto _LL3AF;}_LL3AF: if(*((int*)_tmp35D)== 26){_LL3F3: _tmp38D=((struct Cyc_Absyn_Array_e_struct*)
_tmp35D)->f1;goto _LL3B0;}else{goto _LL3B1;}_LL3B1: if(*((int*)_tmp35D)== 35){
_LL3F4: _tmp38E=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp35D)->f1;goto _LL3B2;}
else{goto _LL3B3;}_LL3B3: if(*((int*)_tmp35D)== 36){_LL3F5: _tmp38F=((struct Cyc_Absyn_Codegen_e_struct*)
_tmp35D)->f1;goto _LL3B4;}else{goto _LL3B5;}_LL3B5: if(*((int*)_tmp35D)== 37){
_LL3F6: _tmp390=((struct Cyc_Absyn_Fill_e_struct*)_tmp35D)->f1;goto _LL3B6;}else{
goto _LL3B7;}_LL3B7: if(*((int*)_tmp35D)== 27){_LL3F9: _tmp393=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp35D)->f1;goto _LL3F8;_LL3F8: _tmp392=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp35D)->f2;goto _LL3F7;_LL3F7: _tmp391=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp35D)->f3;goto _LL3B8;}else{goto _LL3B9;}_LL3B9: if(*((int*)_tmp35D)== 28){
_LL3FC: _tmp397=((struct Cyc_Absyn_Struct_e_struct*)_tmp35D)->f1;_tmp398=(struct
_tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp35D)->f1;goto _LL3FB;_LL3FB:
_tmp396=((struct Cyc_Absyn_Struct_e_struct*)_tmp35D)->f2;goto _LL3FA;_LL3FA:
_tmp394=((struct Cyc_Absyn_Struct_e_struct*)_tmp35D)->f3;_tmp395=(struct Cyc_Absyn_Aggrdecl**)&((
struct Cyc_Absyn_Struct_e_struct*)_tmp35D)->f3;goto _LL3BA;}else{goto _LL3BB;}
_LL3BB: if(*((int*)_tmp35D)== 29){_LL3FE: _tmp39A=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp35D)->f1;goto _LL3FD;_LL3FD: _tmp399=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp35D)->f2;goto _LL3BC;}else{goto _LL3BD;}_LL3BD: if(*((int*)_tmp35D)== 30){
_LL401: _tmp39D=((struct Cyc_Absyn_Tunion_e_struct*)_tmp35D)->f1;goto _LL400;_LL400:
_tmp39C=((struct Cyc_Absyn_Tunion_e_struct*)_tmp35D)->f2;goto _LL3FF;_LL3FF:
_tmp39B=((struct Cyc_Absyn_Tunion_e_struct*)_tmp35D)->f3;goto _LL3BE;}else{goto
_LL3BF;}_LL3BF: if(*((int*)_tmp35D)== 31){_LL404: _tmp3A0=((struct Cyc_Absyn_Enum_e_struct*)
_tmp35D)->f1;_tmp3A1=(struct _tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp35D)->f1;
goto _LL403;_LL403: _tmp39F=((struct Cyc_Absyn_Enum_e_struct*)_tmp35D)->f2;goto
_LL402;_LL402: _tmp39E=((struct Cyc_Absyn_Enum_e_struct*)_tmp35D)->f3;goto _LL3C0;}
else{goto _LL3C1;}_LL3C1: if(*((int*)_tmp35D)== 32){_LL407: _tmp3A4=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp35D)->f1;_tmp3A5=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp35D)->f1;goto _LL406;_LL406: _tmp3A3=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp35D)->f2;goto _LL405;_LL405: _tmp3A2=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp35D)->f3;goto _LL3C2;}else{goto _LL3C3;}_LL3C3: if(*((int*)_tmp35D)== 33){
_LL408: _tmp3A6=((struct Cyc_Absyn_Malloc_e_struct*)_tmp35D)->f1;_LL40D: _tmp3AE=
_tmp3A6.is_calloc;goto _LL40C;_LL40C: _tmp3AD=_tmp3A6.rgn;goto _LL40B;_LL40B:
_tmp3AB=_tmp3A6.elt_type;_tmp3AC=(void***)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp35D)->f1).elt_type;goto _LL40A;_LL40A: _tmp3A9=_tmp3A6.num_elts;_tmp3AA=(
struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp35D)->f1).num_elts;
goto _LL409;_LL409: _tmp3A7=_tmp3A6.fat_result;_tmp3A8=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp35D)->f1).fat_result;goto _LL3C4;}else{goto _LL378;}_LL37A: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp35E);return;_LL37C: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp35F);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL37E: Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp361,_tmp360);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL380: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp362);
Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL382: t=Cyc_Tcexp_tcConst(te,loc,topt,
_tmp364,e);goto _LL378;_LL384: t=Cyc_Tcexp_tcVar(te,loc,_tmp366,_tmp365);goto
_LL378;_LL386: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp368,_tmp367);goto _LL378;
_LL388: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp36A,_tmp369);goto _LL378;_LL38A: t=
Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp36D,_tmp36C,_tmp36B);goto _LL378;_LL38C: t=
Cyc_Tcexp_tcConditional(te,loc,topt,_tmp370,_tmp36F,_tmp36E);goto _LL378;_LL38E: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp372,_tmp371);goto _LL378;_LL390: t=Cyc_Tcexp_tcFnCall(
te,loc,topt,_tmp376,_tmp375,_tmp374);goto _LL378;_LL392: t=Cyc_Tcexp_tcThrow(te,
loc,topt,_tmp377);goto _LL378;_LL394: t=Cyc_Tcexp_tcInstantiate(te,loc,topt,
_tmp379,_tmp378);goto _LL378;_LL396: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp37B,
_tmp37A);goto _LL378;_LL398: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp37C);goto
_LL378;_LL39A: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp37E,e,_tmp37D);goto _LL378;_LL39C: {
void*_tmp3AF=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp37F);t=Cyc_Tcexp_tcSizeof(te,loc,
topt,_tmp3AF);goto _LL378;}_LL39E: t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp380);goto
_LL378;_LL3A0: t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp382,_tmp381);goto _LL378;
_LL3A2:({void*_tmp3B0[0]={};Cyc_Tcutil_terr(loc,_tag_arr("gen() not in top-level initializer",
sizeof(unsigned char),35),_tag_arr(_tmp3B0,sizeof(void*),0));});return;_LL3A4: t=
Cyc_Tcexp_tcDeref(te,loc,topt,_tmp383);goto _LL378;_LL3A6: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp385,_tmp384);goto _LL378;_LL3A8: t=Cyc_Tcexp_tcAggrArrow(te,loc,
topt,_tmp387,_tmp386);goto _LL378;_LL3AA: t=Cyc_Tcexp_tcSubscript(te,loc,topt,
_tmp389,_tmp388);goto _LL378;_LL3AC: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp38A);goto
_LL378;_LL3AE: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp38C,_tmp38B);goto _LL378;
_LL3B0: {void**elt_topt=0;if(topt != 0){void*_tmp3B1=Cyc_Tcutil_compress(*topt);
void*_tmp3B2;void**_tmp3B3;_LL40F: if((unsigned int)_tmp3B1 > 3?*((int*)_tmp3B1)== 
7: 0){_LL413: _tmp3B2=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3B1)->f1;
_tmp3B3=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3B1)->f1);goto
_LL410;}else{goto _LL411;}_LL411: goto _LL412;_LL410: elt_topt=(void**)_tmp3B3;goto
_LL40E;_LL412: goto _LL40E;_LL40E:;}t=Cyc_Tcexp_tcArray(te,loc,elt_topt,_tmp38D);
goto _LL378;}_LL3B2: t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp38E);goto _LL378;_LL3B4:
t=Cyc_Tcexp_tcCodegen(te,loc,topt,_tmp38F);goto _LL378;_LL3B6: t=Cyc_Tcexp_tcFill(
te,loc,topt,_tmp390);goto _LL378;_LL3B8: t=Cyc_Tcexp_tcComprehension(te,loc,topt,
_tmp393,_tmp392,_tmp391);goto _LL378;_LL3BA: t=Cyc_Tcexp_tcStruct(te,loc,topt,
_tmp398,_tmp396,_tmp395);goto _LL378;_LL3BC: t=Cyc_Tcexp_tcAnonStruct(te,loc,
_tmp39A,_tmp399);goto _LL378;_LL3BE: t=Cyc_Tcexp_tcTunion(te,loc,topt,e,_tmp39D,
_tmp39C,_tmp39B);goto _LL378;_LL3C0:*_tmp3A1=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp39E))->name;t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp3B4=
_cycalloc(sizeof(*_tmp3B4));_tmp3B4[0]=({struct Cyc_Absyn_EnumType_struct _tmp3B5;
_tmp3B5.tag=12;_tmp3B5.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp39F))->name;
_tmp3B5.f2=_tmp39F;_tmp3B5;});_tmp3B4;});goto _LL378;_LL3C2:*_tmp3A5=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp3A2))->name;t=_tmp3A3;goto _LL378;_LL3C4: t=Cyc_Tcexp_tcMalloc(te,
loc,topt,_tmp3AD,_tmp3AC,_tmp3AA,_tmp3AE,_tmp3A8);goto _LL378;_LL378:;}e->topt=({
struct Cyc_Core_Opt*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->v=(void*)t;
_tmp3B6;});}
