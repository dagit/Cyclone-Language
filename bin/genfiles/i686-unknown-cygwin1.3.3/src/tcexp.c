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
struct Cyc_List_List*_tmp132;struct _tuple1*_tmp133;struct Cyc_Absyn_Tunionfield
_tmp12E=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_LL170: _tmp133=_tmp12E.name;
goto _LL16F;_LL16F: _tmp132=_tmp12E.tvs;goto _LL16E;_LL16E: _tmp131=_tmp12E.typs;
goto _LL16D;_LL16D: _tmp130=_tmp12E.loc;goto _LL16C;_LL16C: _tmp12F=(void*)_tmp12E.sc;
goto _LL16B;_LL16B: if((_tmp131 == 0? 1: _tmp131->tl != 0)? 1: _tmp132 != 0){continue;}{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple8*)_tmp131->hd)).f2);if(
Cyc_Tcutil_unify(t1,t2)){return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}}for(fields=fs;fields != 0;fields=fields->tl){void*_tmp135;struct
Cyc_Position_Segment*_tmp136;struct Cyc_List_List*_tmp137;struct Cyc_List_List*
_tmp138;struct _tuple1*_tmp139;struct Cyc_Absyn_Tunionfield _tmp134=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_LL176: _tmp139=_tmp134.name;goto _LL175;_LL175: _tmp138=_tmp134.tvs;
goto _LL174;_LL174: _tmp137=_tmp134.typs;goto _LL173;_LL173: _tmp136=_tmp134.loc;
goto _LL172;_LL172: _tmp135=(void*)_tmp134.sc;goto _LL171;_LL171: if((_tmp137 == 0? 1:
_tmp137->tl != 0)? 1: _tmp138 != 0){continue;}{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((
struct _tuple8*)_tmp137->hd)).f2);if(Cyc_Tcutil_coerce_arg(te,e,t2)){return(
struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}}({
struct Cyc_Std_String_pa_struct _tmp13C;_tmp13C.tag=0;_tmp13C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{struct Cyc_Std_String_pa_struct _tmp13B;
_tmp13B.tag=0;_tmp13B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(tu);{void*
_tmp13A[2]={& _tmp13B,& _tmp13C};Cyc_Tcutil_terr(e->loc,_tag_arr("can't find a field in %s to inject a value of type %s",
sizeof(unsigned char),54),_tag_arr(_tmp13A,sizeof(void*),2));}}});return 0;}
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info){struct Cyc_List_List*_tmp13E=args;te=Cyc_Tcenv_new_block(loc,te);
Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp13F=t;struct Cyc_Absyn_PtrInfo _tmp140;struct
Cyc_Absyn_Conref*_tmp141;struct Cyc_Absyn_Tqual _tmp142;struct Cyc_Absyn_Conref*
_tmp143;void*_tmp144;void*_tmp145;_LL178: if((unsigned int)_tmp13F > 3?*((int*)
_tmp13F)== 4: 0){_LL17C: _tmp140=((struct Cyc_Absyn_PointerType_struct*)_tmp13F)->f1;
_LL181: _tmp145=(void*)_tmp140.elt_typ;goto _LL180;_LL180: _tmp144=(void*)_tmp140.rgn_typ;
goto _LL17F;_LL17F: _tmp143=_tmp140.nullable;goto _LL17E;_LL17E: _tmp142=_tmp140.tq;
goto _LL17D;_LL17D: _tmp141=_tmp140.bounds;goto _LL179;}else{goto _LL17A;}_LL17A:
goto _LL17B;_LL179: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp144);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp141);{void*_tmp146=Cyc_Tcutil_compress(_tmp145);struct Cyc_Absyn_FnInfo
_tmp147;struct Cyc_List_List*_tmp148;struct Cyc_List_List*_tmp149;struct Cyc_Absyn_VarargInfo*
_tmp14A;int _tmp14B;struct Cyc_List_List*_tmp14C;void*_tmp14D;struct Cyc_Core_Opt*
_tmp14E;struct Cyc_List_List*_tmp14F;_LL183: if((unsigned int)_tmp146 > 3?*((int*)
_tmp146)== 8: 0){_LL187: _tmp147=((struct Cyc_Absyn_FnType_struct*)_tmp146)->f1;
_LL18F: _tmp14F=_tmp147.tvars;goto _LL18E;_LL18E: _tmp14E=_tmp147.effect;goto _LL18D;
_LL18D: _tmp14D=(void*)_tmp147.ret_typ;goto _LL18C;_LL18C: _tmp14C=_tmp147.args;
goto _LL18B;_LL18B: _tmp14B=_tmp147.c_varargs;goto _LL18A;_LL18A: _tmp14A=_tmp147.cyc_varargs;
goto _LL189;_LL189: _tmp149=_tmp147.rgn_po;goto _LL188;_LL188: _tmp148=_tmp147.attributes;
goto _LL184;}else{goto _LL185;}_LL185: goto _LL186;_LL184: if(topt != 0){Cyc_Tcutil_unify(
_tmp14D,*topt);}while(_tmp13E != 0? _tmp14C != 0: 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp13E->hd;void*t2=(*((struct _tuple2*)_tmp14C->hd)).f3;Cyc_Tcexp_tcExp(
te,(void**)& t2,e1);if(! Cyc_Tcutil_coerce_arg(te,e1,t2)){({struct Cyc_Std_String_pa_struct
_tmp152;_tmp152.tag=0;_tmp152.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp151;_tmp151.tag=0;_tmp151.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{
void*_tmp150[2]={& _tmp151,& _tmp152};Cyc_Tcutil_terr(e1->loc,_tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof(unsigned char),54),_tag_arr(_tmp150,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
_tmp13E=_tmp13E->tl;_tmp14C=_tmp14C->tl;}if(_tmp14C != 0){({void*_tmp153[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments for function",sizeof(
unsigned char),31),_tag_arr(_tmp153,sizeof(void*),0));});}else{if((_tmp13E != 0? 1:
_tmp14B)? 1: _tmp14A != 0){if(_tmp14B){for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmp13E->hd);}}else{if(_tmp14A == 0){({void*_tmp154[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too many arguments for function",sizeof(
unsigned char),32),_tag_arr(_tmp154,sizeof(void*),0));});}else{int _tmp156;void*
_tmp157;struct Cyc_Absyn_VarargInfo _tmp155=*_tmp14A;_LL192: _tmp157=(void*)_tmp155.type;
goto _LL191;_LL191: _tmp156=_tmp155.inject;goto _LL190;_LL190: {struct Cyc_Absyn_VarargCallInfo*
_tmp158=({struct Cyc_Absyn_VarargCallInfo*_tmp16C=_cycalloc(sizeof(*_tmp16C));
_tmp16C->num_varargs=0;_tmp16C->injectors=0;_tmp16C->vai=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp14A);_tmp16C;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp158;if(! _tmp156){for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp13E->hd;_tmp158->num_varargs ++;Cyc_Tcexp_tcExp(te,(
void**)& _tmp157,e1);if(! Cyc_Tcutil_coerce_arg(te,e1,_tmp157)){({struct Cyc_Std_String_pa_struct
_tmp15B;_tmp15B.tag=0;_tmp15B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp15A;_tmp15A.tag=0;_tmp15A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp157);{void*_tmp159[2]={& _tmp15A,& _tmp15B};Cyc_Tcutil_terr(loc,_tag_arr("vararg requires type %s but argument has type %s",
sizeof(unsigned char),49),_tag_arr(_tmp159,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}}
else{void*_tmp15C=Cyc_Tcutil_compress(_tmp157);struct Cyc_Absyn_TunionInfo _tmp15D;
void*_tmp15E;struct Cyc_List_List*_tmp15F;void*_tmp160;struct Cyc_Absyn_Tuniondecl**
_tmp161;struct Cyc_Absyn_Tuniondecl*_tmp162;_LL194: if((unsigned int)_tmp15C > 3?*((
int*)_tmp15C)== 2: 0){_LL198: _tmp15D=((struct Cyc_Absyn_TunionType_struct*)_tmp15C)->f1;
_LL19B: _tmp160=(void*)_tmp15D.tunion_info;if(*((int*)_tmp160)== 1){_LL19C:
_tmp161=((struct Cyc_Absyn_KnownTunion_struct*)_tmp160)->f1;_tmp162=*_tmp161;goto
_LL19A;}else{goto _LL196;}_LL19A: _tmp15F=_tmp15D.targs;goto _LL199;_LL199: _tmp15E=(
void*)_tmp15D.rgn;goto _LL195;}else{goto _LL196;}_LL196: goto _LL197;_LL195: {struct
Cyc_Absyn_Tuniondecl*_tmp163=*Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp162->name);
struct Cyc_List_List*fields=0;if(_tmp163->fields == 0){({struct Cyc_Std_String_pa_struct
_tmp165;_tmp165.tag=0;_tmp165.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp157);{void*_tmp164[1]={& _tmp165};Cyc_Tcutil_terr(loc,_tag_arr("can't inject into %s",
sizeof(unsigned char),21),_tag_arr(_tmp164,sizeof(void*),1));}});}else{fields=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp163->fields))->v;}if(
! Cyc_Tcutil_unify(_tmp15E,Cyc_Tcenv_curr_rgn(te))){({void*_tmp166[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bad region for injected varargs",sizeof(unsigned char),32),
_tag_arr(_tmp166,sizeof(void*),0));});}{struct _RegionHandle _tmp167=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp167;_push_region(rgn);{struct Cyc_List_List*_tmp168=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp163->tvs,_tmp15F);for(0;
_tmp13E != 0;_tmp13E=_tmp13E->tl){_tmp158->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp13E->hd;Cyc_Tcexp_tcExp(te,0,e1);{struct Cyc_Absyn_Tunionfield*
_tmp169=Cyc_Tcexp_tcInjection(te,e1,_tmp157,rgn,_tmp168,fields);if(_tmp169 != 0){
_tmp158->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp158->injectors,({struct Cyc_List_List*_tmp16A=_cycalloc(
sizeof(*_tmp16A));_tmp16A->hd=(struct Cyc_Absyn_Tunionfield*)_check_null(_tmp169);
_tmp16A->tl=0;_tmp16A;}));}}}}};_pop_region(rgn);}goto _LL193;}_LL197:({void*
_tmp16B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad inject vararg type",sizeof(
unsigned char),23),_tag_arr(_tmp16B,sizeof(void*),0));});goto _LL193;_LL193:;}}}}}}{
struct Cyc_List_List*a=_tmp148;for(0;a != 0;a=a->tl){void*_tmp16D=(void*)a->hd;int
_tmp16E;int _tmp16F;void*_tmp170;_LL19E: if((unsigned int)_tmp16D > 16?*((int*)
_tmp16D)== 3: 0){_LL1A4: _tmp170=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp16D)->f1;goto _LL1A3;_LL1A3: _tmp16F=((struct Cyc_Absyn_Format_att_struct*)
_tmp16D)->f2;goto _LL1A2;_LL1A2: _tmp16E=((struct Cyc_Absyn_Format_att_struct*)
_tmp16D)->f3;goto _LL19F;}else{goto _LL1A0;}_LL1A0: goto _LL1A1;_LL19F:{struct
_handler_cons _tmp171;_push_handler(& _tmp171);{int _tmp173=0;if(setjmp(_tmp171.handler)){
_tmp173=1;}if(! _tmp173){{struct Cyc_Absyn_Exp*_tmp174=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp16F - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp16E == 0){fmt_args=0;}else{fmt_args=({struct Cyc_Core_Opt*_tmp175=
_cycalloc(sizeof(*_tmp175));_tmp175->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,int i))Cyc_List_nth_tail)(args,_tmp16E - 1);_tmp175;});}{void*_tmp176=_tmp170;
_LL1A6: if((int)_tmp176 == 0){goto _LL1A7;}else{goto _LL1A8;}_LL1A8: if((int)_tmp176
== 1){goto _LL1A9;}else{goto _LL1A5;}_LL1A7: Cyc_Tcexp_check_format_args(te,_tmp174,
fmt_args,Cyc_Formatstr_get_format_typs);goto _LL1A5;_LL1A9: Cyc_Tcexp_check_format_args(
te,_tmp174,fmt_args,Cyc_Formatstr_get_scanf_typs);goto _LL1A5;_LL1A5:;}};
_pop_handler();}else{void*_tmp172=(void*)_exn_thrown;void*_tmp178=_tmp172;_LL1AB:
if(_tmp178 == Cyc_List_Nth){goto _LL1AC;}else{goto _LL1AD;}_LL1AD: goto _LL1AE;_LL1AC:({
void*_tmp179[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format arguments",sizeof(
unsigned char),21),_tag_arr(_tmp179,sizeof(void*),0));});goto _LL1AA;_LL1AE:(void)
_throw(_tmp178);_LL1AA:;}}}goto _LL19D;_LL1A1: goto _LL19D;_LL19D:;}}Cyc_Tcenv_check_effect_accessible(
te,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp14E))->v);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp149);return _tmp14D;_LL186: return({void*_tmp17A[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expected pointer to function",sizeof(unsigned char),29),
_tag_arr(_tmp17A,sizeof(void*),0));});_LL182:;}_LL17B: return({void*_tmp17B[0]={};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("expected pointer to function",sizeof(
unsigned char),29),_tag_arr(_tmp17B,sizeof(void*),0));});_LL177:;}}static void*
Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,e);if(
! Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ)){({struct Cyc_Std_String_pa_struct
_tmp17D;_tmp17D.tag=0;_tmp17D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp17C[1]={& _tmp17D};Cyc_Tcutil_terr(
loc,_tag_arr("expected xtunion exn but found %s",sizeof(unsigned char),34),
_tag_arr(_tmp17C,sizeof(void*),1));}});}return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp17E;}));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp17F=t1;struct Cyc_Absyn_PtrInfo _tmp180;struct Cyc_Absyn_Conref*_tmp181;
struct Cyc_Absyn_Tqual _tmp182;struct Cyc_Absyn_Conref*_tmp183;void*_tmp184;void*
_tmp185;_LL1B0: if((unsigned int)_tmp17F > 3?*((int*)_tmp17F)== 4: 0){_LL1B4:
_tmp180=((struct Cyc_Absyn_PointerType_struct*)_tmp17F)->f1;_LL1B9: _tmp185=(void*)
_tmp180.elt_typ;goto _LL1B8;_LL1B8: _tmp184=(void*)_tmp180.rgn_typ;goto _LL1B7;
_LL1B7: _tmp183=_tmp180.nullable;goto _LL1B6;_LL1B6: _tmp182=_tmp180.tq;goto _LL1B5;
_LL1B5: _tmp181=_tmp180.bounds;goto _LL1B1;}else{goto _LL1B2;}_LL1B2: goto _LL1B3;
_LL1B1:{void*_tmp186=Cyc_Tcutil_compress(_tmp185);struct Cyc_Absyn_FnInfo _tmp187;
struct Cyc_List_List*_tmp188;struct Cyc_List_List*_tmp189;struct Cyc_Absyn_VarargInfo*
_tmp18A;int _tmp18B;struct Cyc_List_List*_tmp18C;void*_tmp18D;struct Cyc_Core_Opt*
_tmp18E;struct Cyc_List_List*_tmp18F;_LL1BB: if((unsigned int)_tmp186 > 3?*((int*)
_tmp186)== 8: 0){_LL1BF: _tmp187=((struct Cyc_Absyn_FnType_struct*)_tmp186)->f1;
_LL1C7: _tmp18F=_tmp187.tvars;goto _LL1C6;_LL1C6: _tmp18E=_tmp187.effect;goto _LL1C5;
_LL1C5: _tmp18D=(void*)_tmp187.ret_typ;goto _LL1C4;_LL1C4: _tmp18C=_tmp187.args;
goto _LL1C3;_LL1C3: _tmp18B=_tmp187.c_varargs;goto _LL1C2;_LL1C2: _tmp18A=_tmp187.cyc_varargs;
goto _LL1C1;_LL1C1: _tmp189=_tmp187.rgn_po;goto _LL1C0;_LL1C0: _tmp188=_tmp187.attributes;
goto _LL1BC;}else{goto _LL1BD;}_LL1BD: goto _LL1BE;_LL1BC: {struct Cyc_List_List*
instantiation=0;if(Cyc_List_length(ts)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp18F)){return({void*_tmp190[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type instantiation mismatch",
sizeof(unsigned char),28),_tag_arr(_tmp190,sizeof(void*),0));});}for(0;ts != 0;(
ts=ts->tl,_tmp18F=_tmp18F->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp18F))->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp191=_cycalloc(
sizeof(*_tmp191));_tmp191->hd=({struct _tuple5*_tmp192=_cycalloc(sizeof(*_tmp192));
_tmp192->f1=(struct Cyc_Absyn_Tvar*)_tmp18F->hd;_tmp192->f2=(void*)ts->hd;_tmp192;});
_tmp191->tl=instantiation;_tmp191;});}{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp196=_cycalloc(sizeof(*
_tmp196));_tmp196[0]=({struct Cyc_Absyn_FnType_struct _tmp197;_tmp197.tag=8;
_tmp197.f1=({struct Cyc_Absyn_FnInfo _tmp198;_tmp198.tvars=0;_tmp198.effect=
_tmp18E;_tmp198.ret_typ=(void*)_tmp18D;_tmp198.args=_tmp18C;_tmp198.c_varargs=
_tmp18B;_tmp198.cyc_varargs=_tmp18A;_tmp198.rgn_po=_tmp189;_tmp198.attributes=
_tmp188;_tmp198;});_tmp197;});_tmp196;}));void*new_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193[0]=({struct Cyc_Absyn_PointerType_struct
_tmp194;_tmp194.tag=4;_tmp194.f1=({struct Cyc_Absyn_PtrInfo _tmp195;_tmp195.elt_typ=(
void*)new_fn_typ;_tmp195.rgn_typ=(void*)_tmp184;_tmp195.nullable=_tmp183;_tmp195.tq=
_tmp182;_tmp195.bounds=_tmp181;_tmp195;});_tmp194;});_tmp193;});return new_typ;}}
_LL1BE: goto _LL1BA;_LL1BA:;}goto _LL1AF;_LL1B3: goto _LL1AF;_LL1AF:;}return({struct
Cyc_Std_String_pa_struct _tmp19A;_tmp19A.tag=0;_tmp19A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{void*_tmp199[1]={& _tmp19A};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting polymorphic type but found %s",
sizeof(unsigned char),40),_tag_arr(_tmp199,sizeof(void*),1));}});}}static void*
Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,void*t,struct Cyc_Absyn_Exp*e){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,0,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if((! Cyc_Tcutil_silent_castable(te,loc,t2,t)? ! Cyc_Tcutil_castable(
te,loc,t2,t): 0)? ! Cyc_Tcutil_zero_to_null(te,t,e): 0){Cyc_Tcutil_unify(t2,t);{void*
_tmp19B=({struct Cyc_Std_String_pa_struct _tmp19E;_tmp19E.tag=0;_tmp19E.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp19D;
_tmp19D.tag=0;_tmp19D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*
_tmp19C[2]={& _tmp19D,& _tmp19E};Cyc_Tcexp_expr_err(te,loc,_tag_arr("cannot cast %s to %s",
sizeof(unsigned char),21),_tag_arr(_tmp19C,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return _tmp19B;}}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp19F=0;struct Cyc_Absyn_Tqual _tmp1A0=Cyc_Absyn_empty_tqual();if(topt
!= 0){void*_tmp1A1=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp1A2;
struct Cyc_Absyn_Tqual _tmp1A3;void*_tmp1A4;_LL1C9: if((unsigned int)_tmp1A1 > 3?*((
int*)_tmp1A1)== 4: 0){_LL1CD: _tmp1A2=((struct Cyc_Absyn_PointerType_struct*)
_tmp1A1)->f1;_LL1CF: _tmp1A4=(void*)_tmp1A2.elt_typ;goto _LL1CE;_LL1CE: _tmp1A3=
_tmp1A2.tq;goto _LL1CA;}else{goto _LL1CB;}_LL1CB: goto _LL1CC;_LL1CA: _tmp19F=({void**
_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5[0]=_tmp1A4;_tmp1A5;});_tmp1A0=
_tmp1A3;goto _LL1C8;_LL1CC: goto _LL1C8;_LL1C8:;}Cyc_Tcexp_tcExpNoInst(te,_tmp19F,e);{
void*_tmp1A6=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A8;
_LL1D1: if(*((int*)_tmp1A6)== 28){goto _LL1D2;}else{goto _LL1D3;}_LL1D3: if(*((int*)
_tmp1A6)== 24){goto _LL1D4;}else{goto _LL1D5;}_LL1D5: if(*((int*)_tmp1A6)== 23){
_LL1DA: _tmp1A8=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1A6)->f1;goto _LL1D9;
_LL1D9: _tmp1A7=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1A6)->f2;goto _LL1D6;}
else{goto _LL1D7;}_LL1D7: goto _LL1D8;_LL1D2: goto _LL1D4;_LL1D4:({void*_tmp1A9[0]={};
Cyc_Tcutil_warn(loc,_tag_arr("& used to allocate",sizeof(unsigned char),19),
_tag_arr(_tmp1A9,sizeof(void*),0));});{void*(*_tmp1AA)(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq)=Cyc_Absyn_at_typ;if(topt != 0){void*_tmp1AB=Cyc_Tcutil_compress(*topt);struct
Cyc_Absyn_PtrInfo _tmp1AC;struct Cyc_Absyn_Conref*_tmp1AD;_LL1DC: if((unsigned int)
_tmp1AB > 3?*((int*)_tmp1AB)== 4: 0){_LL1E0: _tmp1AC=((struct Cyc_Absyn_PointerType_struct*)
_tmp1AB)->f1;_LL1E1: _tmp1AD=_tmp1AC.nullable;goto _LL1DD;}else{goto _LL1DE;}_LL1DE:
goto _LL1DF;_LL1DD: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp1AD)){_tmp1AA=Cyc_Absyn_star_typ;}goto _LL1DB;_LL1DF: goto _LL1DB;_LL1DB:;}
return _tmp1AA((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)2,
_tmp1A0);}_LL1D6:{void*_tmp1AE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1A8->topt))->v);_LL1E3: if((unsigned int)_tmp1AE > 3?*((int*)
_tmp1AE)== 9: 0){goto _LL1E4;}else{goto _LL1E5;}_LL1E5: goto _LL1E6;_LL1E4: goto _LL1E2;
_LL1E6:(void*)(e0->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*_tmp1AF=
_cycalloc(sizeof(*_tmp1AF));_tmp1AF[0]=({struct Cyc_Absyn_Primop_e_struct _tmp1B0;
_tmp1B0.tag=3;_tmp1B0.f1=(void*)((void*)0);_tmp1B0.f2=({struct Cyc_List_List*
_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->hd=_tmp1A8;_tmp1B1->tl=({struct Cyc_List_List*
_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2->hd=_tmp1A7;_tmp1B2->tl=0;_tmp1B2;});
_tmp1B1;});_tmp1B0;});_tmp1AF;})));return Cyc_Tcexp_tcPlus(te,_tmp1A8,_tmp1A7);
_LL1E2:;}goto _LL1D0;_LL1D8: goto _LL1D0;_LL1D0:;}{void*_tmp1B4;int _tmp1B5;struct
_tuple6 _tmp1B3=Cyc_Tcutil_addressof_props(te,e);_LL1E9: _tmp1B5=_tmp1B3.f1;goto
_LL1E8;_LL1E8: _tmp1B4=_tmp1B3.f2;goto _LL1E7;_LL1E7: {struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();
if(_tmp1B5){tq.q_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp1B4,tq);return t;}}}}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(! Cyc_Evexp_okay_szofarg(t)){({
struct Cyc_Std_String_pa_struct _tmp1B7;_tmp1B7.tag=0;_tmp1B7.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1B6[1]={& _tmp1B7};Cyc_Tcutil_terr(
loc,_tag_arr("sizeof applied to type %s, which has unknown size here",sizeof(
unsigned char),55),_tag_arr(_tmp1B6,sizeof(void*),1));}});}if(topt != 0){void*
_tmp1B8=Cyc_Tcutil_compress(*topt);_LL1EB: if((unsigned int)_tmp1B8 > 3?*((int*)
_tmp1B8)== 14: 0){goto _LL1EC;}else{goto _LL1ED;}_LL1ED: goto _LL1EE;_LL1EC: return(
void*)({struct Cyc_Absyn_SizeofType_struct*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));
_tmp1B9[0]=({struct Cyc_Absyn_SizeofType_struct _tmp1BA;_tmp1BA.tag=14;_tmp1BA.f1=(
void*)t;_tmp1BA;});_tmp1B9;});_LL1EE: goto _LL1EA;_LL1EA:;}return Cyc_Absyn_uint_t;}
int Cyc_Tcexp_structfield_has_name(struct _tagged_arr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_Std_strcmp(*n,*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp1BB=n;struct _tagged_arr*_tmp1BC;unsigned int _tmp1BD;_LL1F0: if(*((int*)
_tmp1BB)== 0){_LL1F4: _tmp1BC=((struct Cyc_Absyn_StructField_struct*)_tmp1BB)->f1;
goto _LL1F1;}else{goto _LL1F2;}_LL1F2: if(*((int*)_tmp1BB)== 1){_LL1F5: _tmp1BD=((
struct Cyc_Absyn_TupleIndex_struct*)_tmp1BB)->f1;goto _LL1F3;}else{goto _LL1EF;}
_LL1F1: {int bad_type=1;{void*_tmp1BE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1BF;void*_tmp1C0;struct Cyc_Absyn_Aggrdecl**_tmp1C1;struct Cyc_List_List*
_tmp1C2;_LL1F7: if((unsigned int)_tmp1BE > 3?*((int*)_tmp1BE)== 10: 0){_LL1FD:
_tmp1BF=((struct Cyc_Absyn_AggrType_struct*)_tmp1BE)->f1;_LL1FE: _tmp1C0=(void*)
_tmp1BF.aggr_info;if(*((int*)_tmp1C0)== 1){_LL1FF: _tmp1C1=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1C0)->f1;goto _LL1F8;}else{goto _LL1F9;}}else{goto _LL1F9;}_LL1F9: if((
unsigned int)_tmp1BE > 3?*((int*)_tmp1BE)== 11: 0){_LL200: _tmp1C2=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1BE)->f2;goto _LL1FA;}else{goto _LL1FB;}_LL1FB: goto _LL1FC;_LL1F8: if((*_tmp1C1)->fields
== 0){goto _LL1F6;}if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1BC,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*_tmp1C1)->fields))->v)){({
struct Cyc_Std_String_pa_struct _tmp1C4;_tmp1C4.tag=0;_tmp1C4.f1=(struct
_tagged_arr)*_tmp1BC;{void*_tmp1C3[1]={& _tmp1C4};Cyc_Tcutil_terr(loc,_tag_arr("no field of struct/union has name %s",
sizeof(unsigned char),37),_tag_arr(_tmp1C3,sizeof(void*),1));}});}bad_type=0;
goto _LL1F6;_LL1FA: if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1BC,_tmp1C2)){({struct Cyc_Std_String_pa_struct _tmp1C6;_tmp1C6.tag=0;_tmp1C6.f1=(
struct _tagged_arr)*_tmp1BC;{void*_tmp1C5[1]={& _tmp1C6};Cyc_Tcutil_terr(loc,
_tag_arr("no field of struct/union has name %s",sizeof(unsigned char),37),
_tag_arr(_tmp1C5,sizeof(void*),1));}});}bad_type=0;goto _LL1F6;_LL1FC: goto _LL1F6;
_LL1F6:;}if(bad_type){({struct Cyc_Std_String_pa_struct _tmp1C8;_tmp1C8.tag=0;
_tmp1C8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1C7[1]={&
_tmp1C8};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known struct/union type",
sizeof(unsigned char),36),_tag_arr(_tmp1C7,sizeof(void*),1));}});}goto _LL1EF;}
_LL1F3: {int bad_type=1;{void*_tmp1C9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1CA;void*_tmp1CB;struct Cyc_Absyn_Aggrdecl**_tmp1CC;struct Cyc_List_List*
_tmp1CD;struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_TunionFieldInfo _tmp1CF;void*
_tmp1D0;struct Cyc_Absyn_Tunionfield*_tmp1D1;_LL202: if((unsigned int)_tmp1C9 > 3?*((
int*)_tmp1C9)== 10: 0){_LL20C: _tmp1CA=((struct Cyc_Absyn_AggrType_struct*)_tmp1C9)->f1;
_LL20D: _tmp1CB=(void*)_tmp1CA.aggr_info;if(*((int*)_tmp1CB)== 1){_LL20E: _tmp1CC=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp1CB)->f1;goto _LL203;}else{goto _LL204;}}
else{goto _LL204;}_LL204: if((unsigned int)_tmp1C9 > 3?*((int*)_tmp1C9)== 11: 0){
_LL20F: _tmp1CD=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C9)->f2;goto _LL205;}
else{goto _LL206;}_LL206: if((unsigned int)_tmp1C9 > 3?*((int*)_tmp1C9)== 9: 0){
_LL210: _tmp1CE=((struct Cyc_Absyn_TupleType_struct*)_tmp1C9)->f1;goto _LL207;}
else{goto _LL208;}_LL208: if((unsigned int)_tmp1C9 > 3?*((int*)_tmp1C9)== 3: 0){
_LL211: _tmp1CF=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1C9)->f1;_LL212:
_tmp1D0=(void*)_tmp1CF.field_info;if(*((int*)_tmp1D0)== 1){_LL213: _tmp1D1=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp1D0)->f2;goto _LL209;}else{goto
_LL20A;}}else{goto _LL20A;}_LL20A: goto _LL20B;_LL203: if((*_tmp1CC)->fields == 0){
goto _LL201;}_tmp1CD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*
_tmp1CC)->fields))->v;goto _LL205;_LL205: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp1CD)<= _tmp1BD){({struct Cyc_Std_Int_pa_struct _tmp1D4;_tmp1D4.tag=1;_tmp1D4.f1=(
int)_tmp1BD;{struct Cyc_Std_Int_pa_struct _tmp1D3;_tmp1D3.tag=1;_tmp1D3.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CD));{void*
_tmp1D2[2]={& _tmp1D3,& _tmp1D4};Cyc_Tcutil_terr(loc,_tag_arr("struct/union has too few components: %d <= %d",
sizeof(unsigned char),46),_tag_arr(_tmp1D2,sizeof(void*),2));}}});}bad_type=0;
goto _LL201;_LL207: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CE)<= 
_tmp1BD){({struct Cyc_Std_Int_pa_struct _tmp1D7;_tmp1D7.tag=1;_tmp1D7.f1=(int)
_tmp1BD;{struct Cyc_Std_Int_pa_struct _tmp1D6;_tmp1D6.tag=1;_tmp1D6.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1CE));{void*
_tmp1D5[2]={& _tmp1D6,& _tmp1D7};Cyc_Tcutil_terr(loc,_tag_arr("tuple has too few components: %d <= %d",
sizeof(unsigned char),39),_tag_arr(_tmp1D5,sizeof(void*),2));}}});}bad_type=0;
goto _LL201;_LL209: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1D1->typs)
< _tmp1BD){({struct Cyc_Std_Int_pa_struct _tmp1DA;_tmp1DA.tag=1;_tmp1DA.f1=(int)
_tmp1BD;{struct Cyc_Std_Int_pa_struct _tmp1D9;_tmp1D9.tag=1;_tmp1D9.f1=(int)((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1D1->typs));{
void*_tmp1D8[2]={& _tmp1D9,& _tmp1DA};Cyc_Tcutil_terr(loc,_tag_arr("tunionfield has too few components: %d < %d",
sizeof(unsigned char),44),_tag_arr(_tmp1D8,sizeof(void*),2));}}});}bad_type=0;
goto _LL201;_LL20B: goto _LL201;_LL201:;}if(bad_type){({struct Cyc_Std_String_pa_struct
_tmp1DC;_tmp1DC.tag=0;_tmp1DC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp1DB[1]={& _tmp1DC};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known type",
sizeof(unsigned char),23),_tag_arr(_tmp1DB,sizeof(void*),1));}});}goto _LL1EF;}
_LL1EF:;}return Cyc_Absyn_uint_t;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1DD=t;struct Cyc_Absyn_PtrInfo _tmp1DE;struct Cyc_Absyn_Conref*_tmp1DF;
void*_tmp1E0;void*_tmp1E1;_LL215: if((unsigned int)_tmp1DD > 3?*((int*)_tmp1DD)== 
4: 0){_LL219: _tmp1DE=((struct Cyc_Absyn_PointerType_struct*)_tmp1DD)->f1;_LL21C:
_tmp1E1=(void*)_tmp1DE.elt_typ;goto _LL21B;_LL21B: _tmp1E0=(void*)_tmp1DE.rgn_typ;
goto _LL21A;_LL21A: _tmp1DF=_tmp1DE.bounds;goto _LL216;}else{goto _LL217;}_LL217:
goto _LL218;_LL216: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp1E0);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp1DF);if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp1E1),(void*)1)){({
void*_tmp1E2[0]={};Cyc_Tcutil_terr(loc,_tag_arr("can't dereference abstract pointer type",
sizeof(unsigned char),40),_tag_arr(_tmp1E2,sizeof(void*),0));});}return _tmp1E1;
_LL218: return({struct Cyc_Std_String_pa_struct _tmp1E4;_tmp1E4.tag=0;_tmp1E4.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1E3[1]={& _tmp1E4};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting * or @ type but found %s",sizeof(unsigned char),35),
_tag_arr(_tmp1E3,sizeof(void*),1));}});_LL214:;}}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,
e);{void*_tmp1E5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp1E6;struct Cyc_List_List*_tmp1E7;void*_tmp1E8;struct
Cyc_Absyn_Aggrdecl**_tmp1E9;struct Cyc_Absyn_Aggrdecl*_tmp1EA;struct Cyc_List_List*
_tmp1EB;_LL21E: if((unsigned int)_tmp1E5 > 3?*((int*)_tmp1E5)== 10: 0){_LL228:
_tmp1E6=((struct Cyc_Absyn_AggrType_struct*)_tmp1E5)->f1;_LL22A: _tmp1E8=(void*)
_tmp1E6.aggr_info;if(*((int*)_tmp1E8)== 1){_LL22B: _tmp1E9=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1E8)->f1;_tmp1EA=*_tmp1E9;goto _LL229;}else{goto _LL220;}_LL229: _tmp1E7=
_tmp1E6.targs;goto _LL21F;}else{goto _LL220;}_LL220: if((unsigned int)_tmp1E5 > 3?*((
int*)_tmp1E5)== 11: 0){_LL22C: _tmp1EB=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1E5)->f2;goto _LL221;}else{goto _LL222;}_LL222: if((unsigned int)_tmp1E5 > 3?*((
int*)_tmp1E5)== 7: 0){goto _LL22D;}else{goto _LL224;}_LL22D: if(Cyc_Std_zstrcmp(*f,
_tag_arr("size",sizeof(unsigned char),5))== 0){goto _LL223;}else{goto _LL224;}
_LL224: if((unsigned int)_tmp1E5 > 3?*((int*)_tmp1E5)== 4: 0){goto _LL22E;}else{goto
_LL226;}_LL22E: if(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(unsigned char),5))== 
0){goto _LL225;}else{goto _LL226;}_LL226: goto _LL227;_LL21F: {struct Cyc_Absyn_Aggrfield*
_tmp1EC=Cyc_Absyn_lookup_decl_field(_tmp1EA,f);if(_tmp1EC == 0){return({struct Cyc_Std_String_pa_struct
_tmp1EF;_tmp1EF.tag=0;_tmp1EF.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct
_tmp1EE;_tmp1EE.tag=0;_tmp1EE.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp1EA->name);{void*_tmp1ED[2]={& _tmp1EE,& _tmp1EF};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("type %s has no %s field",sizeof(unsigned char),24),_tag_arr(_tmp1ED,
sizeof(void*),2));}}});}if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1EA->exist_vars))->v != 0){return({void*_tmp1F0[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(unsigned char),64),_tag_arr(_tmp1F0,sizeof(void*),0));});}{void*t2;{
struct _RegionHandle _tmp1F1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1F1;
_push_region(rgn);{struct Cyc_List_List*_tmp1F2=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1EA->tvs,_tmp1E7);t2=Cyc_Tcutil_rsubstitute(rgn,
_tmp1F2,(void*)_tmp1EC->type);};_pop_region(rgn);}return t2;}}_LL221: {struct Cyc_Absyn_Aggrfield*
_tmp1F3=Cyc_Absyn_lookup_field(_tmp1EB,f);if(_tmp1F3 == 0){return({struct Cyc_Std_String_pa_struct
_tmp1F5;_tmp1F5.tag=0;_tmp1F5.f1=(struct _tagged_arr)*f;{void*_tmp1F4[1]={&
_tmp1F5};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type has no %s field",sizeof(
unsigned char),21),_tag_arr(_tmp1F4,sizeof(void*),1));}});}return(void*)_tmp1F3->type;}
_LL223: goto _LL225;_LL225:(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp1F7;_tmp1F7.tag=3;_tmp1F7.f1=(void*)((void*)19);_tmp1F7.f2=({struct Cyc_List_List*
_tmp1F8=_cycalloc(sizeof(*_tmp1F8));_tmp1F8->hd=e;_tmp1F8->tl=0;_tmp1F8;});
_tmp1F7;});_tmp1F6;})));return Cyc_Absyn_uint_t;_LL227: if(Cyc_Std_zstrcmp(*f,
_tag_arr("size",sizeof(unsigned char),5))== 0){return({struct Cyc_Std_String_pa_struct
_tmp1FA;_tmp1FA.tag=0;_tmp1FA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1F9[1]={& _tmp1FA};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct, union, or array, found %s",sizeof(
unsigned char),44),_tag_arr(_tmp1F9,sizeof(void*),1));}});}else{return({struct
Cyc_Std_String_pa_struct _tmp1FC;_tmp1FC.tag=0;_tmp1FC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1FB[1]={& _tmp1FC};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting struct or union, found %s",sizeof(
unsigned char),36),_tag_arr(_tmp1FB,sizeof(void*),1));}});}_LL21D:;}}static void*
Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct _tagged_arr*f){Cyc_Tcexp_tcExp(te,0,e);{
void*_tmp1FD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1FE;struct Cyc_Absyn_Conref*_tmp1FF;void*_tmp200;void*
_tmp201;_LL230: if((unsigned int)_tmp1FD > 3?*((int*)_tmp1FD)== 4: 0){_LL234:
_tmp1FE=((struct Cyc_Absyn_PointerType_struct*)_tmp1FD)->f1;_LL237: _tmp201=(void*)
_tmp1FE.elt_typ;goto _LL236;_LL236: _tmp200=(void*)_tmp1FE.rgn_typ;goto _LL235;
_LL235: _tmp1FF=_tmp1FE.bounds;goto _LL231;}else{goto _LL232;}_LL232: goto _LL233;
_LL231: Cyc_Tcutil_check_nonzero_bound(loc,_tmp1FF);{void*_tmp202=Cyc_Tcutil_compress(
_tmp201);struct Cyc_Absyn_AggrInfo _tmp203;struct Cyc_List_List*_tmp204;void*
_tmp205;struct Cyc_Absyn_Aggrdecl**_tmp206;struct Cyc_Absyn_Aggrdecl*_tmp207;
struct Cyc_List_List*_tmp208;_LL239: if((unsigned int)_tmp202 > 3?*((int*)_tmp202)
== 10: 0){_LL23F: _tmp203=((struct Cyc_Absyn_AggrType_struct*)_tmp202)->f1;_LL241:
_tmp205=(void*)_tmp203.aggr_info;if(*((int*)_tmp205)== 1){_LL242: _tmp206=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp205)->f1;_tmp207=*_tmp206;goto _LL240;}
else{goto _LL23B;}_LL240: _tmp204=_tmp203.targs;goto _LL23A;}else{goto _LL23B;}
_LL23B: if((unsigned int)_tmp202 > 3?*((int*)_tmp202)== 11: 0){_LL243: _tmp208=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp202)->f2;goto _LL23C;}else{goto _LL23D;}
_LL23D: goto _LL23E;_LL23A: {struct Cyc_Absyn_Aggrfield*_tmp209=Cyc_Absyn_lookup_decl_field(
_tmp207,f);if(_tmp209 == 0){return({struct Cyc_Std_String_pa_struct _tmp20C;_tmp20C.tag=
0;_tmp20C.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct _tmp20B;
_tmp20B.tag=0;_tmp20B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp207->name);{
void*_tmp20A[2]={& _tmp20B,& _tmp20C};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type %s has no %s field",
sizeof(unsigned char),24),_tag_arr(_tmp20A,sizeof(void*),2));}}});}if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp207->exist_vars))->v != 0){return({void*
_tmp20D[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(unsigned char),64),_tag_arr(_tmp20D,sizeof(void*),0));});}{void*t3;{
struct _RegionHandle _tmp20E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp20E;
_push_region(rgn);{struct Cyc_List_List*_tmp20F=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp207->tvs,_tmp204);t3=Cyc_Tcutil_rsubstitute(rgn,
_tmp20F,(void*)_tmp209->type);};_pop_region(rgn);}return t3;}}_LL23C: {struct Cyc_Absyn_Aggrfield*
_tmp210=Cyc_Absyn_lookup_field(_tmp208,f);if(_tmp210 == 0){return({struct Cyc_Std_String_pa_struct
_tmp212;_tmp212.tag=0;_tmp212.f1=(struct _tagged_arr)*f;{void*_tmp211[1]={&
_tmp212};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type has no %s field",sizeof(
unsigned char),21),_tag_arr(_tmp211,sizeof(void*),1));}});}return(void*)_tmp210->type;}
_LL23E: goto _LL238;_LL238:;}goto _LL22F;_LL233: goto _LL22F;_LL22F:;}return({struct
Cyc_Std_String_pa_struct _tmp214;_tmp214.tag=0;_tmp214.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp213[1]={& _tmp214};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting struct pointer, found %s",sizeof(
unsigned char),35),_tag_arr(_tmp213,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int i=Cyc_Evexp_eval_const_uint_exp(index);
struct _handler_cons _tmp215;_push_handler(& _tmp215);{int _tmp217=0;if(setjmp(
_tmp215.handler)){_tmp217=1;}if(! _tmp217){{void*_tmp218=(*((struct _tuple8*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0);return
_tmp218;};_pop_handler();}else{void*_tmp216=(void*)_exn_thrown;void*_tmp21A=
_tmp216;_LL245: if(_tmp21A == Cyc_List_Nth){goto _LL246;}else{goto _LL247;}_LL247:
goto _LL248;_LL246: return({struct Cyc_Std_Int_pa_struct _tmp21D;_tmp21D.tag=1;
_tmp21D.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts));{struct Cyc_Std_Int_pa_struct _tmp21C;_tmp21C.tag=1;_tmp21C.f1=(int)i;{void*
_tmp21B[2]={& _tmp21C,& _tmp21D};Cyc_Tcexp_expr_err(te,loc,_tag_arr("index is %d but tuple has only %d fields",
sizeof(unsigned char),41),_tag_arr(_tmp21B,sizeof(void*),2));}}});_LL248:(void)
_throw(_tmp21A);_LL244:;}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(! Cyc_Tcutil_coerce_sint_typ(
te,e2)){return({struct Cyc_Std_String_pa_struct _tmp21F;_tmp21F.tag=0;_tmp21F.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp21E[1]={& _tmp21F};Cyc_Tcexp_expr_err(
te,e2->loc,_tag_arr("expecting int subscript, found %s",sizeof(unsigned char),34),
_tag_arr(_tmp21E,sizeof(void*),1));}});}{void*_tmp220=t1;struct Cyc_Absyn_PtrInfo
_tmp221;struct Cyc_Absyn_Conref*_tmp222;struct Cyc_Absyn_Tqual _tmp223;void*_tmp224;
void*_tmp225;struct Cyc_List_List*_tmp226;_LL24A: if((unsigned int)_tmp220 > 3?*((
int*)_tmp220)== 4: 0){_LL250: _tmp221=((struct Cyc_Absyn_PointerType_struct*)
_tmp220)->f1;_LL254: _tmp225=(void*)_tmp221.elt_typ;goto _LL253;_LL253: _tmp224=(
void*)_tmp221.rgn_typ;goto _LL252;_LL252: _tmp223=_tmp221.tq;goto _LL251;_LL251:
_tmp222=_tmp221.bounds;goto _LL24B;}else{goto _LL24C;}_LL24C: if((unsigned int)
_tmp220 > 3?*((int*)_tmp220)== 9: 0){_LL255: _tmp226=((struct Cyc_Absyn_TupleType_struct*)
_tmp220)->f1;goto _LL24D;}else{goto _LL24E;}_LL24E: goto _LL24F;_LL24B: if(Cyc_Tcutil_is_const_exp(
te,e2)){Cyc_Tcutil_check_bound(loc,Cyc_Evexp_eval_const_uint_exp(e2),_tmp222);}
else{if(Cyc_Tcutil_is_bound_one(_tmp222)){({void*_tmp227[0]={};Cyc_Tcutil_warn(
e1->loc,_tag_arr("subscript applied to pointer to one object",sizeof(
unsigned char),43),_tag_arr(_tmp227,sizeof(void*),0));});}Cyc_Tcutil_check_nonzero_bound(
loc,_tmp222);}Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp224);if(! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp225),(void*)1)){({void*_tmp228[0]={};Cyc_Tcutil_terr(e1->loc,
_tag_arr("can't subscript an abstract pointer",sizeof(unsigned char),36),
_tag_arr(_tmp228,sizeof(void*),0));});}return _tmp225;_LL24D: return Cyc_Tcexp_ithTupleType(
te,loc,_tmp226,e2);_LL24F: return({struct Cyc_Std_String_pa_struct _tmp22A;_tmp22A.tag=
0;_tmp22A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp229[1]={&
_tmp22A};Cyc_Tcexp_expr_err(te,loc,_tag_arr("subscript applied to %s",sizeof(
unsigned char),24),_tag_arr(_tmp229,sizeof(void*),1));}});_LL249:;}}}static void*
Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;if(topt != 0){
void*_tmp22B=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp22C;_LL257: if((
unsigned int)_tmp22B > 3?*((int*)_tmp22B)== 9: 0){_LL25B: _tmp22C=((struct Cyc_Absyn_TupleType_struct*)
_tmp22B)->f1;goto _LL258;}else{goto _LL259;}_LL259: goto _LL25A;_LL258: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp22C)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es)){goto _LL256;}for(0;es != 0;(es=es->tl,_tmp22C=_tmp22C->tl)){
void*_tmp22D=(*((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp22C))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp22D,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->hd=({
struct _tuple8*_tmp22F=_cycalloc(sizeof(*_tmp22F));_tmp22F->f1=(*((struct _tuple8*)
_tmp22C->hd)).f1;_tmp22F->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp22F;});_tmp22E->tl=fields;_tmp22E;});}done=
1;goto _LL256;_LL25A: goto _LL256;_LL256:;}if(! done){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(
te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*_tmp230=
_cycalloc(sizeof(*_tmp230));_tmp230->hd=({struct _tuple8*_tmp231=_cycalloc(
sizeof(*_tmp231));_tmp231->f1=Cyc_Absyn_empty_tqual();_tmp231->f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp231;});_tmp230->tl=fields;_tmp230;});}}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232[0]=({struct Cyc_Absyn_TupleType_struct
_tmp233;_tmp233.tag=9;_tmp233.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp233;});_tmp232;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp234[0]={};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("tcCompoundLit",sizeof(unsigned char),14),_tag_arr(_tmp234,sizeof(void*),
0));});}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,struct Cyc_List_List*des){struct Cyc_List_List*es=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple7*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp245=_cycalloc(
sizeof(*_tmp245));_tmp245->v=Cyc_Tcenv_lookup_type_vars(te);_tmp245;}));struct
Cyc_Absyn_Const_e_struct*_tmp235=({struct Cyc_Absyn_Const_e_struct*_tmp241=
_cycalloc(sizeof(*_tmp241));_tmp241[0]=({struct Cyc_Absyn_Const_e_struct _tmp242;
_tmp242.tag=0;_tmp242.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp243=
_cycalloc(sizeof(*_tmp243));_tmp243[0]=({struct Cyc_Absyn_Int_c_struct _tmp244;
_tmp244.tag=2;_tmp244.f1=(void*)((void*)1);_tmp244.f2=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es);_tmp244;});_tmp243;}));_tmp242;});_tmp241;});struct Cyc_Absyn_Exp*
sz_exp=Cyc_Absyn_new_exp((void*)_tmp235,loc);sz_exp->topt=({struct Cyc_Core_Opt*
_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236->v=(void*)Cyc_Absyn_uint_t;_tmp236;});{
void*res_t2=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp23F=_cycalloc(sizeof(*
_tmp23F));_tmp23F[0]=({struct Cyc_Absyn_ArrayType_struct _tmp240;_tmp240.tag=7;
_tmp240.f1=(void*)res;_tmp240.f2=Cyc_Absyn_empty_tqual();_tmp240.f3=(struct Cyc_Absyn_Exp*)
sz_exp;_tmp240;});_tmp23F;});{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(! Cyc_Tcutil_coerce_list(
te,res,es)){({struct Cyc_Std_String_pa_struct _tmp238;_tmp238.tag=0;_tmp238.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(res);{void*_tmp237[1]={& _tmp238};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,_tag_arr("elements of array do not all have the same type (%s)",
sizeof(unsigned char),53),_tag_arr(_tmp237,sizeof(void*),1));}});}{int offset=0;
for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple7*)
des->hd)).f1;if(ds != 0){void*_tmp239=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp23A;
_LL25D: if(*((int*)_tmp239)== 1){goto _LL25E;}else{goto _LL25F;}_LL25F: if(*((int*)
_tmp239)== 0){_LL261: _tmp23A=((struct Cyc_Absyn_ArrayElement_struct*)_tmp239)->f1;
goto _LL260;}else{goto _LL25C;}_LL25E:({void*_tmp23B[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("only array index designators are supported",sizeof(unsigned char),43),
_tag_arr(_tmp23B,sizeof(void*),0));});goto _LL25C;_LL260: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp23A);{unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmp23A);if(i != 
offset){({struct Cyc_Std_Int_pa_struct _tmp23E;_tmp23E.tag=1;_tmp23E.f1=(int)i;{
struct Cyc_Std_Int_pa_struct _tmp23D;_tmp23D.tag=1;_tmp23D.f1=(int)((unsigned int)
offset);{void*_tmp23C[2]={& _tmp23D,& _tmp23E};Cyc_Tcutil_terr(_tmp23A->loc,
_tag_arr("expecting index designator %d but found %d",sizeof(unsigned char),43),
_tag_arr(_tmp23C,sizeof(void*),2));}}});}goto _LL25C;}_LL25C:;}}}return res_t2;}}
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body){Cyc_Tcexp_tcExp(te,0,bound);if(! Cyc_Tcutil_coerce_uint_typ(te,bound)){({
struct Cyc_Std_String_pa_struct _tmp247;_tmp247.tag=0;_tmp247.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
bound->topt))->v);{void*_tmp246[1]={& _tmp247};Cyc_Tcutil_terr(bound->loc,
_tag_arr("expecting unsigned int, found %s",sizeof(unsigned char),33),_tag_arr(
_tmp246,sizeof(void*),1));}});}if(!(vd->tq).q_const){({void*_tmp248[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("comprehension index variable is not declared const!",
sizeof(unsigned char),52),_tag_arr(_tmp248,sizeof(void*),0));});}if(te->le != 0){
te=Cyc_Tcenv_new_block(loc,te);te=Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp249=0;struct Cyc_Absyn_Tqual*_tmp24A=0;if(topt != 0){void*_tmp24B=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp24C;struct Cyc_Absyn_Exp*
_tmp24D;struct Cyc_Absyn_Tqual _tmp24E;struct Cyc_Absyn_Tqual*_tmp24F;void*_tmp250;
void**_tmp251;_LL263: if((unsigned int)_tmp24B > 3?*((int*)_tmp24B)== 4: 0){_LL269:
_tmp24C=((struct Cyc_Absyn_PointerType_struct*)_tmp24B)->f1;goto _LL264;}else{goto
_LL265;}_LL265: if((unsigned int)_tmp24B > 3?*((int*)_tmp24B)== 7: 0){_LL26C:
_tmp250=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp24B)->f1;_tmp251=(void**)&((
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp24B)->f1);goto _LL26B;_LL26B:
_tmp24E=((struct Cyc_Absyn_ArrayType_struct*)_tmp24B)->f2;_tmp24F=(struct Cyc_Absyn_Tqual*)&((
struct Cyc_Absyn_ArrayType_struct*)_tmp24B)->f2;goto _LL26A;_LL26A: _tmp24D=((
struct Cyc_Absyn_ArrayType_struct*)_tmp24B)->f3;goto _LL266;}else{goto _LL267;}
_LL267: goto _LL268;_LL264: pinfo=_tmp24C;_tmp249=(void**)((void**)((void*)& pinfo.elt_typ));
_tmp24A=(struct Cyc_Absyn_Tqual*)& pinfo.tq;goto _LL262;_LL266: _tmp249=(void**)
_tmp251;_tmp24A=(struct Cyc_Absyn_Tqual*)_tmp24F;goto _LL262;_LL268: goto _LL262;
_LL262:;}{void*t=Cyc_Tcexp_tcExp(te,_tmp249,body);if(te->le == 0){if(! Cyc_Tcutil_is_const_exp(
te,bound)){({void*_tmp252[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("bound is not constant",
sizeof(unsigned char),22),_tag_arr(_tmp252,sizeof(void*),0));});}if(! Cyc_Tcutil_is_const_exp(
te,body)){({void*_tmp253[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("body is not constant",
sizeof(unsigned char),21),_tag_arr(_tmp253,sizeof(void*),0));});}}return(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp255;_tmp255.tag=7;_tmp255.f1=(void*)t;
_tmp255.f2=_tmp24A == 0? Cyc_Absyn_empty_tqual():*_tmp24A;_tmp255.f3=(struct Cyc_Absyn_Exp*)
bound;_tmp255;});_tmp254;});}}}struct _tuple9{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List*args,
struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0){ad=(
struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);}else{{struct _handler_cons _tmp256;
_push_handler(& _tmp256);{int _tmp258=0;if(setjmp(_tmp256.handler)){_tmp258=1;}if(
! _tmp258){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;_pop_handler();}else{void*
_tmp257=(void*)_exn_thrown;void*_tmp25A=_tmp257;_LL26E: if(_tmp25A == Cyc_Dict_Absent){
goto _LL26F;}else{goto _LL270;}_LL270: goto _LL271;_LL26F:({struct Cyc_Std_String_pa_struct
_tmp25C;_tmp25C.tag=0;_tmp25C.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{
void*_tmp25B[1]={& _tmp25C};Cyc_Tcutil_terr(loc,_tag_arr("unbound struct name %s",
sizeof(unsigned char),23),_tag_arr(_tmp25B,sizeof(void*),1));}});return topt != 0?*
topt:(void*)0;_LL271:(void)_throw(_tmp25A);_LL26D:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if((void*)ad->kind == (void*)1){({void*_tmp25D[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("expecting struct but found union",sizeof(unsigned char),33),
_tag_arr(_tmp25D,sizeof(void*),0));});}if(ad->fields == 0){({void*_tmp25E[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("can't build abstract struct",sizeof(unsigned char),
28),_tag_arr(_tmp25E,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp25F;}));}{struct _RegionHandle _tmp260=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp260;_push_region(rgn);{struct _tuple4 _tmp261=({struct _tuple4 _tmp27A;
_tmp27A.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp27A.f2=rgn;_tmp27A;});struct Cyc_List_List*
_tmp262=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp261,ad->tvs);struct Cyc_List_List*_tmp263=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp261,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ad->exist_vars))->v);struct Cyc_List_List*_tmp264=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp262);struct Cyc_List_List*_tmp265=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp263);struct Cyc_List_List*_tmp266=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp262,_tmp263);struct Cyc_List_List*_tmp267=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp266);struct Cyc_Absyn_AggrType_struct*_tmp268=({struct Cyc_Absyn_AggrType_struct*
_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274[0]=({struct Cyc_Absyn_AggrType_struct
_tmp275;_tmp275.tag=10;_tmp275.f1=({struct Cyc_Absyn_AggrInfo _tmp276;_tmp276.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp277=_cycalloc(sizeof(*
_tmp277));_tmp277[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp278;_tmp278.tag=1;
_tmp278.f1=({struct Cyc_Absyn_Aggrdecl**_tmp279=_cycalloc(sizeof(*_tmp279));
_tmp279[0]=ad;_tmp279;});_tmp278;});_tmp277;}));_tmp276.targs=_tmp264;_tmp276;});
_tmp275;});_tmp274;});if(topt != 0){Cyc_Tcutil_unify((void*)_tmp268,*topt);}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ad->fields))->v);for(0;fields != 0;fields=fields->tl){
struct _tuple9 _tmp26A;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Aggrfield*
_tmp26C;struct _tuple9*_tmp269=(struct _tuple9*)fields->hd;_tmp26A=*_tmp269;_LL274:
_tmp26C=_tmp26A.f1;goto _LL273;_LL273: _tmp26B=_tmp26A.f2;goto _LL272;_LL272: {void*
_tmp26D=Cyc_Tcutil_rsubstitute(rgn,_tmp266,(void*)_tmp26C->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp26D,_tmp26B);if(! Cyc_Tcutil_coerce_arg(te,_tmp26B,_tmp26D)){({
struct Cyc_Std_String_pa_struct _tmp272;_tmp272.tag=0;_tmp272.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp26B->topt))->v);{struct Cyc_Std_String_pa_struct _tmp271;_tmp271.tag=0;_tmp271.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp26D);{struct Cyc_Std_String_pa_struct
_tmp270;_tmp270.tag=0;_tmp270.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{
struct Cyc_Std_String_pa_struct _tmp26F;_tmp26F.tag=0;_tmp26F.f1=(struct
_tagged_arr)*_tmp26C->name;{void*_tmp26E[4]={& _tmp26F,& _tmp270,& _tmp271,& _tmp272};
Cyc_Tcutil_terr(_tmp26B->loc,_tag_arr("field %s of struct %s expects type %s != %s",
sizeof(unsigned char),44),_tag_arr(_tmp26E,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
void*_tmp273=(void*)_tmp268;_npop_handler(0);return _tmp273;}}};_pop_region(rgn);}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){{struct _RegionHandle _tmp27B=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp27B;_push_region(rgn);{void*_tmp27C=Cyc_Tcutil_compress(
ts);struct Cyc_List_List*_tmp27D;void*_tmp27E;_LL276: if((unsigned int)_tmp27C > 3?*((
int*)_tmp27C)== 11: 0){_LL27B: _tmp27E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp27C)->f1;goto _LL27A;_LL27A: _tmp27D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp27C)->f2;goto _LL277;}else{goto _LL278;}_LL278: goto _LL279;_LL277: if(_tmp27E == (
void*)1){({void*_tmp27F[0]={};Cyc_Tcutil_terr(loc,_tag_arr("expecting struct but found union",
sizeof(unsigned char),33),_tag_arr(_tmp27F,sizeof(void*),0));});}{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,_tmp27D);for(0;fields != 0;fields=fields->tl){struct _tuple9 _tmp281;
struct Cyc_Absyn_Exp*_tmp282;struct Cyc_Absyn_Aggrfield*_tmp283;struct _tuple9*
_tmp280=(struct _tuple9*)fields->hd;_tmp281=*_tmp280;_LL27E: _tmp283=_tmp281.f1;
goto _LL27D;_LL27D: _tmp282=_tmp281.f2;goto _LL27C;_LL27C: Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp283->type)),_tmp282);if(! Cyc_Tcutil_coerce_arg(
te,_tmp282,(void*)_tmp283->type)){({struct Cyc_Std_String_pa_struct _tmp287;
_tmp287.tag=0;_tmp287.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp282->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp286;_tmp286.tag=0;_tmp286.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
_tmp283->type);{struct Cyc_Std_String_pa_struct _tmp285;_tmp285.tag=0;_tmp285.f1=(
struct _tagged_arr)*_tmp283->name;{void*_tmp284[3]={& _tmp285,& _tmp286,& _tmp287};
Cyc_Tcutil_terr(_tmp282->loc,_tag_arr("field %s of struct expects type %s != %s",
sizeof(unsigned char),41),_tag_arr(_tmp284,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
goto _LL275;}_LL279:({void*_tmp288[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcAnonStruct: wrong type",sizeof(
unsigned char),25),_tag_arr(_tmp288,sizeof(void*),0));});_LL275:;};_pop_region(
rgn);}return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp289=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp289;_push_region(rgn);{struct _tuple4 _tmp28A=({
struct _tuple4 _tmp2AC;_tmp2AC.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp2AC.f2=rgn;
_tmp2AC;});struct Cyc_List_List*_tmp28B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp28A,tud->tvs);struct Cyc_List_List*_tmp28C=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp28A,tuf->tvs);struct Cyc_List_List*_tmp28D=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp28B);struct Cyc_List_List*_tmp28E=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp28C);struct Cyc_List_List*_tmp28F=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp28B,_tmp28C);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp2A8;_tmp2A8.tag=3;_tmp2A8.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp2A9;
_tmp2A9.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp2AB;_tmp2AB.tag=1;_tmp2AB.f1=tud;_tmp2AB.f2=tuf;_tmp2AB;});_tmp2AA;}));
_tmp2A9.targs=_tmp28D;_tmp2A9;});_tmp2A8;});_tmp2A7;});if(topt != 0){void*_tmp290=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp291;void*_tmp292;struct
Cyc_List_List*_tmp293;_LL280: if((unsigned int)_tmp290 > 3?*((int*)_tmp290)== 3: 0){
goto _LL281;}else{goto _LL282;}_LL282: if((unsigned int)_tmp290 > 3?*((int*)_tmp290)
== 2: 0){_LL286: _tmp291=((struct Cyc_Absyn_TunionType_struct*)_tmp290)->f1;_LL288:
_tmp293=_tmp291.targs;goto _LL287;_LL287: _tmp292=(void*)_tmp291.rgn;goto _LL283;}
else{goto _LL284;}_LL284: goto _LL285;_LL281: Cyc_Tcutil_unify(*topt,res);goto _LL27F;
_LL283:{struct Cyc_List_List*a=_tmp28D;for(0;a != 0? _tmp293 != 0: 0;(a=a->tl,_tmp293=
_tmp293->tl)){Cyc_Tcutil_unify((void*)a->hd,(void*)_tmp293->hd);}}if(tuf->typs == 
0? es == 0: 0){e->topt=({struct Cyc_Core_Opt*_tmp294=_cycalloc(sizeof(*_tmp294));
_tmp294->v=(void*)res;_tmp294;});res=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp295=_cycalloc(sizeof(*_tmp295));_tmp295[0]=({struct Cyc_Absyn_TunionType_struct
_tmp296;_tmp296.tag=2;_tmp296.f1=({struct Cyc_Absyn_TunionInfo _tmp297;_tmp297.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp298=_cycalloc(sizeof(*
_tmp298));_tmp298[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp299;_tmp299.tag=1;
_tmp299.f1=({struct Cyc_Absyn_Tuniondecl**_tmp29A=_cycalloc(sizeof(*_tmp29A));
_tmp29A[0]=tud;_tmp29A;});_tmp299;});_tmp298;}));_tmp297.targs=_tmp28D;_tmp297.rgn=(
void*)_tmp292;_tmp297;});_tmp296;});_tmp295;});Cyc_Tcutil_unchecked_cast(te,e,
res);{void*_tmp29B=res;_npop_handler(0);return _tmp29B;}}goto _LL27F;_LL285: goto
_LL27F;_LL27F:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0? ts != 0: 0;(es=es->tl,
ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp28F,(*((struct _tuple8*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)&
t,e);if(! Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct _tmp29F;
_tmp29F.tag=0;_tmp29F.f1=(struct _tagged_arr)(e->topt == 0? _tag_arr("?",sizeof(
unsigned char),2): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{struct Cyc_Std_String_pa_struct _tmp29E;_tmp29E.tag=0;
_tmp29E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp29D;_tmp29D.tag=0;_tmp29D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp29C[3]={& _tmp29D,& _tmp29E,& _tmp29F};Cyc_Tcutil_terr(e->loc,_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof(unsigned char),80),_tag_arr(_tmp29C,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp2A2=({struct Cyc_Std_String_pa_struct _tmp2A1;_tmp2A1.tag=0;
_tmp2A1.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp2A0[1]={&
_tmp2A1};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(unsigned char),45),_tag_arr(_tmp2A0,sizeof(void*),1));}});_npop_handler(0);
return _tmp2A2;}if(ts != 0){void*_tmp2A5=({struct Cyc_Std_String_pa_struct _tmp2A4;
_tmp2A4.tag=0;_tmp2A4.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp2A3[1]={& _tmp2A4};Cyc_Tcexp_expr_err(te,loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(unsigned char),44),_tag_arr(_tmp2A3,sizeof(void*),1));}});_npop_handler(0);
return _tmp2A5;}{void*_tmp2A6=res;_npop_handler(0);return _tmp2A6;}}};_pop_region(
rgn);}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp2AD=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp2AE;struct Cyc_Absyn_Conref*_tmp2AF;void*_tmp2B0;
struct Cyc_List_List*_tmp2B1;struct Cyc_Absyn_AggrInfo _tmp2B2;struct Cyc_List_List*
_tmp2B3;void*_tmp2B4;struct Cyc_List_List*_tmp2B5;_LL28A: if((int)_tmp2AD == 0){
goto _LL28B;}else{goto _LL28C;}_LL28C: if((unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)
== 0: 0){goto _LL28D;}else{goto _LL28E;}_LL28E: if((unsigned int)_tmp2AD > 3?*((int*)
_tmp2AD)== 1: 0){goto _LL28F;}else{goto _LL290;}_LL290: if((unsigned int)_tmp2AD > 3?*((
int*)_tmp2AD)== 2: 0){goto _LL291;}else{goto _LL292;}_LL292: if((unsigned int)
_tmp2AD > 3?*((int*)_tmp2AD)== 3: 0){goto _LL293;}else{goto _LL294;}_LL294: if((
unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 4: 0){_LL2B8: _tmp2AE=((struct Cyc_Absyn_PointerType_struct*)
_tmp2AD)->f1;_LL2B9: _tmp2AF=_tmp2AE.nullable;goto _LL295;}else{goto _LL296;}_LL296:
if((unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 5: 0){goto _LL297;}else{goto _LL298;}
_LL298: if((int)_tmp2AD == 1){goto _LL299;}else{goto _LL29A;}_LL29A: if((unsigned int)
_tmp2AD > 3?*((int*)_tmp2AD)== 6: 0){goto _LL29B;}else{goto _LL29C;}_LL29C: if((
unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 7: 0){_LL2BA: _tmp2B0=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp2AD)->f1;goto _LL29D;}else{goto _LL29E;}_LL29E: if((
unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 8: 0){goto _LL29F;}else{goto _LL2A0;}
_LL2A0: if((unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 9: 0){_LL2BB: _tmp2B1=((
struct Cyc_Absyn_TupleType_struct*)_tmp2AD)->f1;goto _LL2A1;}else{goto _LL2A2;}
_LL2A2: if((unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 10: 0){_LL2BC: _tmp2B2=((
struct Cyc_Absyn_AggrType_struct*)_tmp2AD)->f1;_LL2BE: _tmp2B4=(void*)_tmp2B2.aggr_info;
goto _LL2BD;_LL2BD: _tmp2B3=_tmp2B2.targs;goto _LL2A3;}else{goto _LL2A4;}_LL2A4: if((
unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 12: 0){goto _LL2A5;}else{goto _LL2A6;}
_LL2A6: if((unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 14: 0){goto _LL2A7;}else{
goto _LL2A8;}_LL2A8: if((unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 11: 0){_LL2BF:
_tmp2B5=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2AD)->f2;goto _LL2A9;}else{
goto _LL2AA;}_LL2AA: if((unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 13: 0){goto
_LL2AB;}else{goto _LL2AC;}_LL2AC: if((unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 
15: 0){goto _LL2AD;}else{goto _LL2AE;}_LL2AE: if((unsigned int)_tmp2AD > 3?*((int*)
_tmp2AD)== 16: 0){goto _LL2AF;}else{goto _LL2B0;}_LL2B0: if((int)_tmp2AD == 2){goto
_LL2B1;}else{goto _LL2B2;}_LL2B2: if((unsigned int)_tmp2AD > 3?*((int*)_tmp2AD)== 
17: 0){goto _LL2B3;}else{goto _LL2B4;}_LL2B4: if((unsigned int)_tmp2AD > 3?*((int*)
_tmp2AD)== 18: 0){goto _LL2B5;}else{goto _LL2B6;}_LL2B6: if((unsigned int)_tmp2AD > 3?*((
int*)_tmp2AD)== 19: 0){goto _LL2B7;}else{goto _LL289;}_LL28B: return 1;_LL28D: goto
_LL28F;_LL28F: goto _LL291;_LL291: goto _LL293;_LL293: return 0;_LL295: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp2AF);_LL297: goto _LL299;
_LL299: goto _LL29B;_LL29B: return 1;_LL29D: return Cyc_Tcexp_zeroable_type(_tmp2B0);
_LL29F: return 0;_LL2A1: for(0;(unsigned int)_tmp2B1;_tmp2B1=_tmp2B1->tl){if(! Cyc_Tcexp_zeroable_type((*((
struct _tuple8*)_tmp2B1->hd)).f2)){return 0;}}return 1;_LL2A3: {struct Cyc_Absyn_Aggrdecl*
_tmp2B6=Cyc_Absyn_get_known_aggrdecl(_tmp2B4);if(_tmp2B6->exist_vars == 0? 1:(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B6->exist_vars))->v
!= 0){return 0;}{struct _RegionHandle _tmp2B7=_new_region("r");struct _RegionHandle*
r=& _tmp2B7;_push_region(r);{struct Cyc_List_List*_tmp2B8=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp2B6->tvs,_tmp2B3);{struct Cyc_List_List*fs=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2B6->fields))->v;for(0;fs != 0;fs=fs->tl){if(
! Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp2B8,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmp2B9=0;_npop_handler(0);return _tmp2B9;}}}{int _tmp2BA=1;
_npop_handler(0);return _tmp2BA;}};_pop_region(r);}}_LL2A5: return 1;_LL2A7: return 1;
_LL2A9: for(0;_tmp2B5 != 0;_tmp2B5=_tmp2B5->tl){if(! Cyc_Tcexp_zeroable_type((void*)((
struct Cyc_Absyn_Aggrfield*)_tmp2B5->hd)->type)){return 0;}}return 1;_LL2AB: return 1;
_LL2AD: return 0;_LL2AF: goto _LL2B1;_LL2B1: goto _LL2B3;_LL2B3: goto _LL2B5;_LL2B5: goto
_LL2B7;_LL2B7:({struct Cyc_Std_String_pa_struct _tmp2BC;_tmp2BC.tag=0;_tmp2BC.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp2BB[1]={& _tmp2BC};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad type `%s' in zeroable type",
sizeof(unsigned char),31),_tag_arr(_tmp2BB,sizeof(void*),1));}});_LL289:;}static
void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*loc,
void**topt,void*t){if(Cyc_Tcutil_bits_only(t)? 1:(allow_zero? Cyc_Tcexp_zeroable_type(
t): 0)){return;}if(topt != 0){void*_tmp2BD=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp2BE;void*_tmp2BF;_LL2C1: if((unsigned int)_tmp2BD > 3?*((int*)_tmp2BD)== 4: 0){
_LL2C5: _tmp2BE=((struct Cyc_Absyn_PointerType_struct*)_tmp2BD)->f1;_LL2C6: _tmp2BF=(
void*)_tmp2BE.elt_typ;goto _LL2C2;}else{goto _LL2C3;}_LL2C3: goto _LL2C4;_LL2C2: Cyc_Tcutil_unify(
_tmp2BF,t);if(Cyc_Tcutil_bits_only(t)? 1:(allow_zero? Cyc_Tcexp_zeroable_type(t): 0)){
return;}goto _LL2C0;_LL2C4: goto _LL2C0;_LL2C0:;}({struct Cyc_Std_String_pa_struct
_tmp2C2;_tmp2C2.tag=0;_tmp2C2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp2C1;_tmp2C1.tag=0;_tmp2C1.f1=(struct
_tagged_arr)(allow_zero? _tag_arr("calloc",sizeof(unsigned char),7): _tag_arr("malloc",
sizeof(unsigned char),7));{void*_tmp2C0[2]={& _tmp2C1,& _tmp2C2};Cyc_Tcutil_terr(
loc,_tag_arr("%s cannot be used with type %s\n\t(type needs initialization)",
sizeof(unsigned char),60),_tag_arr(_tmp2C0,sizeof(void*),2));}}});}static void*
Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*ropt,void***t,struct Cyc_Absyn_Exp**e,int is_calloc,int*
is_fat){void*rgn=(void*)2;if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2C7=_cycalloc(sizeof(*_tmp2C7));_tmp2C7[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2C8;_tmp2C8.tag=15;_tmp2C8.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));_tmp2C9->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2C9;}));_tmp2C8;});_tmp2C7;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(ropt));void*_tmp2C3=Cyc_Tcutil_compress(handle_type);void*_tmp2C4;
_LL2C8: if((unsigned int)_tmp2C3 > 3?*((int*)_tmp2C3)== 15: 0){_LL2CC: _tmp2C4=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp2C3)->f1;goto _LL2C9;}else{goto _LL2CA;}
_LL2CA: goto _LL2CB;_LL2C9: rgn=_tmp2C4;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL2C7;_LL2CB:({struct Cyc_Std_String_pa_struct _tmp2C6;_tmp2C6.tag=0;_tmp2C6.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type);{void*_tmp2C5[1]={& _tmp2C6};
Cyc_Tcutil_terr(ropt->loc,_tag_arr("expecting region_t type but found %s",
sizeof(unsigned char),37),_tag_arr(_tmp2C5,sizeof(void*),1));}});goto _LL2C7;
_LL2C7:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_t,*e);{void*elt_type;struct
Cyc_Absyn_Exp*num_elts;int one_elt;if(is_calloc){if(*t == 0){({void*_tmp2CA[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("calloc with empty type",
sizeof(unsigned char),23),_tag_arr(_tmp2CA,sizeof(void*),0));});}elt_type=*((
void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp2CB=(void*)(*e)->r;void*_tmp2CC;struct Cyc_List_List*
_tmp2CD;struct Cyc_List_List _tmp2CE;struct Cyc_List_List*_tmp2CF;struct Cyc_List_List
_tmp2D0;struct Cyc_List_List*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D2;struct Cyc_Absyn_Exp*
_tmp2D3;void*_tmp2D4;_LL2CE: if(*((int*)_tmp2CB)== 16){_LL2D4: _tmp2CC=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2CB)->f1;goto _LL2CF;}else{goto _LL2D0;}
_LL2D0: if(*((int*)_tmp2CB)== 3){_LL2DA: _tmp2D4=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2CB)->f1;if((int)_tmp2D4 == 1){goto _LL2D5;}else{goto _LL2D2;}_LL2D5: _tmp2CD=((
struct Cyc_Absyn_Primop_e_struct*)_tmp2CB)->f2;if(_tmp2CD == 0){goto _LL2D2;}else{
_tmp2CE=*_tmp2CD;_LL2D9: _tmp2D3=(struct Cyc_Absyn_Exp*)_tmp2CE.hd;goto _LL2D6;
_LL2D6: _tmp2CF=_tmp2CE.tl;if(_tmp2CF == 0){goto _LL2D2;}else{_tmp2D0=*_tmp2CF;
_LL2D8: _tmp2D2=(struct Cyc_Absyn_Exp*)_tmp2D0.hd;goto _LL2D7;_LL2D7: _tmp2D1=
_tmp2D0.tl;if(_tmp2D1 == 0){goto _LL2D1;}else{goto _LL2D2;}}}}else{goto _LL2D2;}
_LL2D2: goto _LL2D3;_LL2CF: elt_type=_tmp2CC;*t=(void**)({void**_tmp2D5=_cycalloc(
sizeof(*_tmp2D5));_tmp2D5[0]=elt_type;_tmp2D5;});num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_t,num_elts);one_elt=1;goto _LL2CD;
_LL2D1:{struct _tuple0 _tmp2D7=({struct _tuple0 _tmp2D6;_tmp2D6.f1=(void*)_tmp2D3->r;
_tmp2D6.f2=(void*)_tmp2D2->r;_tmp2D6;});void*_tmp2D8;void*_tmp2D9;void*_tmp2DA;
void*_tmp2DB;_LL2DC: _LL2E2: _tmp2D8=_tmp2D7.f1;if(*((int*)_tmp2D8)== 16){_LL2E3:
_tmp2D9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2D8)->f1;goto _LL2DD;}
else{goto _LL2DE;}_LL2DE: _LL2E4: _tmp2DA=_tmp2D7.f2;if(*((int*)_tmp2DA)== 16){
_LL2E5: _tmp2DB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2DA)->f1;goto
_LL2DF;}else{goto _LL2E0;}_LL2E0: goto _LL2E1;_LL2DD: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp2D9);elt_type=_tmp2D9;*t=(void**)({void**_tmp2DC=_cycalloc(sizeof(*
_tmp2DC));_tmp2DC[0]=elt_type;_tmp2DC;});num_elts=_tmp2D2;one_elt=0;goto _LL2DB;
_LL2DF: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp2DB);elt_type=_tmp2DB;*t=(void**)({
void**_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD[0]=elt_type;_tmp2DD;});
num_elts=_tmp2D3;one_elt=0;goto _LL2DB;_LL2E1: goto No_sizeof;_LL2DB:;}goto _LL2CD;
_LL2D3: No_sizeof: elt_type=Cyc_Absyn_uchar_t;*t=(void**)({void**_tmp2DE=_cycalloc(
sizeof(*_tmp2DE));_tmp2DE[0]=elt_type;_tmp2DE;});num_elts=*e;one_elt=0;goto
_LL2CD;_LL2CD:;}*e=num_elts;*is_fat=! one_elt;{void*_tmp2DF=elt_type;struct Cyc_Absyn_TunionFieldInfo
_tmp2E0;void*_tmp2E1;struct Cyc_Absyn_Tunionfield*_tmp2E2;struct Cyc_Absyn_AggrInfo
_tmp2E3;void*_tmp2E4;struct Cyc_Absyn_Aggrdecl**_tmp2E5;struct Cyc_Absyn_Aggrdecl*
_tmp2E6;_LL2E7: if((unsigned int)_tmp2DF > 3?*((int*)_tmp2DF)== 3: 0){_LL2ED:
_tmp2E0=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2DF)->f1;_LL2EE: _tmp2E1=(
void*)_tmp2E0.field_info;if(*((int*)_tmp2E1)== 1){_LL2EF: _tmp2E2=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp2E1)->f2;goto _LL2E8;}else{goto _LL2E9;}}else{goto _LL2E9;}_LL2E9: if((
unsigned int)_tmp2DF > 3?*((int*)_tmp2DF)== 10: 0){_LL2F0: _tmp2E3=((struct Cyc_Absyn_AggrType_struct*)
_tmp2DF)->f1;_LL2F1: _tmp2E4=(void*)_tmp2E3.aggr_info;if(*((int*)_tmp2E4)== 1){
_LL2F2: _tmp2E5=((struct Cyc_Absyn_KnownAggr_struct*)_tmp2E4)->f1;_tmp2E6=*_tmp2E5;
goto _LL2EA;}else{goto _LL2EB;}}else{goto _LL2EB;}_LL2EB: goto _LL2EC;_LL2E8: if(
_tmp2E2->tvs != 0){({void*_tmp2E7[0]={};Cyc_Tcutil_terr(loc,_tag_arr("malloc with existential types not yet implemented",
sizeof(unsigned char),50),_tag_arr(_tmp2E7,sizeof(void*),0));});}goto _LL2E6;
_LL2EA: if(_tmp2E6->exist_vars != 0?(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2E6->exist_vars))->v != 0: 0){({void*_tmp2E8[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("malloc with existential types not yet implemented",sizeof(
unsigned char),50),_tag_arr(_tmp2E8,sizeof(void*),0));});}goto _LL2E6;_LL2EC: goto
_LL2E6;_LL2E6:;}{void*(*_tmp2E9)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq)=Cyc_Absyn_at_typ;
if(topt != 0){void*_tmp2EA=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp2EB;struct Cyc_Absyn_Conref*_tmp2EC;_LL2F4: if((unsigned int)_tmp2EA > 3?*((int*)
_tmp2EA)== 4: 0){_LL2F8: _tmp2EB=((struct Cyc_Absyn_PointerType_struct*)_tmp2EA)->f1;
_LL2F9: _tmp2EC=_tmp2EB.nullable;goto _LL2F5;}else{goto _LL2F6;}_LL2F6: goto _LL2F7;
_LL2F5: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp2EC)){
_tmp2E9=Cyc_Absyn_star_typ;}goto _LL2F3;_LL2F7: goto _LL2F3;_LL2F3:;}if(! one_elt){
_tmp2E9=Cyc_Absyn_tagged_typ;}return _tmp2E9(elt_type,rgn,Cyc_Absyn_empty_tqual());}}}
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
te,s),s,1);Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));while(1){
void*_tmp2ED=(void*)s->r;struct Cyc_Absyn_Exp*_tmp2EE;struct Cyc_Absyn_Stmt*
_tmp2EF;struct Cyc_Absyn_Stmt*_tmp2F0;struct Cyc_Absyn_Stmt*_tmp2F1;struct Cyc_Absyn_Decl*
_tmp2F2;_LL2FB: if((unsigned int)_tmp2ED > 1?*((int*)_tmp2ED)== 0: 0){_LL303:
_tmp2EE=((struct Cyc_Absyn_Exp_s_struct*)_tmp2ED)->f1;goto _LL2FC;}else{goto _LL2FD;}
_LL2FD: if((unsigned int)_tmp2ED > 1?*((int*)_tmp2ED)== 1: 0){_LL305: _tmp2F0=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2ED)->f1;goto _LL304;_LL304: _tmp2EF=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2ED)->f2;goto _LL2FE;}else{goto _LL2FF;}_LL2FF: if((unsigned int)_tmp2ED > 1?*((
int*)_tmp2ED)== 12: 0){_LL307: _tmp2F2=((struct Cyc_Absyn_Decl_s_struct*)_tmp2ED)->f1;
goto _LL306;_LL306: _tmp2F1=((struct Cyc_Absyn_Decl_s_struct*)_tmp2ED)->f2;goto
_LL300;}else{goto _LL301;}_LL301: goto _LL302;_LL2FC: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2EE->topt))->v;_LL2FE: s=_tmp2EF;continue;_LL300: s=_tmp2F1;
continue;_LL302: return({void*_tmp2F3[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("statement expression must end with expression",
sizeof(unsigned char),46),_tag_arr(_tmp2F3,sizeof(void*),0));});_LL2FA:;}}static
void*Cyc_Tcexp_tcCodegen(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Fndecl*fd){return({void*_tmp2F4[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("tcCodegen",sizeof(unsigned char),10),_tag_arr(_tmp2F4,sizeof(
void*),0));});}static void*Cyc_Tcexp_tcFill(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){return({void*_tmp2F5[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("tcFill",sizeof(unsigned char),7),_tag_arr(_tmp2F5,sizeof(void*),
0));});}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2FA=_cycalloc(sizeof(*_tmp2FA));_tmp2FA[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2FB;_tmp2FB.tag=15;_tmp2FB.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2FC;}));_tmp2FB;});_tmp2FA;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(rgn_handle));void*_tmp2F6=Cyc_Tcutil_compress(handle_type);void*
_tmp2F7;_LL309: if((unsigned int)_tmp2F6 > 3?*((int*)_tmp2F6)== 15: 0){_LL30D:
_tmp2F7=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2F6)->f1;goto _LL30A;}
else{goto _LL30B;}_LL30B: goto _LL30C;_LL30A: rgn=_tmp2F7;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL308;_LL30C:({struct Cyc_Std_String_pa_struct _tmp2F9;_tmp2F9.tag=
0;_tmp2F9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type);{void*
_tmp2F8[1]={& _tmp2F9};Cyc_Tcutil_terr(rgn_handle->loc,_tag_arr("expecting region_t type but found %s",
sizeof(unsigned char),37),_tag_arr(_tmp2F8,sizeof(void*),1));}});goto _LL308;
_LL308:;}{void*_tmp2FD=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp2FE;struct Cyc_Absyn_Exp*
_tmp2FF;struct Cyc_Absyn_Vardecl*_tmp300;struct Cyc_List_List*_tmp301;struct Cyc_Core_Opt*
_tmp302;struct Cyc_List_List*_tmp303;void*_tmp304;struct _tagged_arr _tmp305;_LL30F:
if(*((int*)_tmp2FD)== 27){_LL31B: _tmp300=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2FD)->f1;goto _LL31A;_LL31A: _tmp2FF=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2FD)->f2;goto _LL319;_LL319: _tmp2FE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2FD)->f3;goto _LL310;}else{goto _LL311;}_LL311: if(*((int*)_tmp2FD)== 34){
_LL31D: _tmp302=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp2FD)->f1;goto _LL31C;
_LL31C: _tmp301=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp2FD)->f2;goto _LL312;}
else{goto _LL313;}_LL313: if(*((int*)_tmp2FD)== 26){_LL31E: _tmp303=((struct Cyc_Absyn_Array_e_struct*)
_tmp2FD)->f1;goto _LL314;}else{goto _LL315;}_LL315: if(*((int*)_tmp2FD)== 0){_LL31F:
_tmp304=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp2FD)->f1;if((unsigned int)
_tmp304 > 1?*((int*)_tmp304)== 5: 0){_LL320: _tmp305=((struct Cyc_Absyn_String_c_struct*)
_tmp304)->f1;goto _LL316;}else{goto _LL317;}}else{goto _LL317;}_LL317: goto _LL318;
_LL310: {void*_tmp306=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*_tmp307=Cyc_Tcutil_compress(
_tmp306);struct Cyc_Absyn_Tqual _tmp308;void*_tmp309;_LL322: if((unsigned int)
_tmp307 > 3?*((int*)_tmp307)== 7: 0){_LL327: _tmp309=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp307)->f1;goto _LL326;_LL326: _tmp308=((struct Cyc_Absyn_ArrayType_struct*)
_tmp307)->f2;goto _LL323;}else{goto _LL324;}_LL324: goto _LL325;_LL323: {void*b=Cyc_Tcutil_is_const_exp(
te,_tmp2FF)?(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp30E=_cycalloc(sizeof(*
_tmp30E));_tmp30E[0]=({struct Cyc_Absyn_Upper_b_struct _tmp30F;_tmp30F.tag=0;
_tmp30F.f1=_tmp2FF;_tmp30F;});_tmp30E;}):(void*)0;void*res_typ=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B[0]=({
struct Cyc_Absyn_PointerType_struct _tmp30C;_tmp30C.tag=4;_tmp30C.f1=({struct Cyc_Absyn_PtrInfo
_tmp30D;_tmp30D.elt_typ=(void*)_tmp309;_tmp30D.rgn_typ=(void*)rgn;_tmp30D.nullable=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp30D.tq=_tmp308;
_tmp30D.bounds=Cyc_Absyn_new_conref(b);_tmp30D;});_tmp30C;});_tmp30B;});if(topt
!= 0){if(! Cyc_Tcutil_unify(*topt,res_typ)? Cyc_Tcutil_silent_castable(te,loc,
res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp30A=_cycalloc(sizeof(*_tmp30A));
_tmp30A->v=(void*)res_typ;_tmp30A;});Cyc_Tcutil_unchecked_cast(te,e,*topt);
res_typ=*topt;}}return res_typ;}_LL325:({void*_tmp310[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcNew: comprehension returned non-array type",
sizeof(unsigned char),45),_tag_arr(_tmp310,sizeof(void*),0));});_LL321:;}_LL312:(
void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp311=_cycalloc(
sizeof(*_tmp311));_tmp311[0]=({struct Cyc_Absyn_Array_e_struct _tmp312;_tmp312.tag=
26;_tmp312.f1=_tmp301;_tmp312;});_tmp311;})));_tmp303=_tmp301;goto _LL314;_LL314: {
void**elt_typ_opt=0;if(topt != 0){void*_tmp313=Cyc_Tcutil_compress(*topt);struct
Cyc_Absyn_PtrInfo _tmp314;struct Cyc_Absyn_Tqual _tmp315;void*_tmp316;void**_tmp317;
_LL329: if((unsigned int)_tmp313 > 3?*((int*)_tmp313)== 4: 0){_LL32D: _tmp314=((
struct Cyc_Absyn_PointerType_struct*)_tmp313)->f1;_LL32F: _tmp316=(void*)_tmp314.elt_typ;
_tmp317=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp313)->f1).elt_typ;
goto _LL32E;_LL32E: _tmp315=_tmp314.tq;goto _LL32A;}else{goto _LL32B;}_LL32B: goto
_LL32C;_LL32A: elt_typ_opt=(void**)_tmp317;goto _LL328;_LL32C: goto _LL328;_LL328:;}{
void*_tmp318=Cyc_Tcexp_tcExpNoPromote(te,elt_typ_opt,e1);void*res_typ;{void*
_tmp319=Cyc_Tcutil_compress(_tmp318);struct Cyc_Absyn_Exp*_tmp31A;struct Cyc_Absyn_Tqual
_tmp31B;void*_tmp31C;_LL331: if((unsigned int)_tmp319 > 3?*((int*)_tmp319)== 7: 0){
_LL337: _tmp31C=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp319)->f1;goto
_LL336;_LL336: _tmp31B=((struct Cyc_Absyn_ArrayType_struct*)_tmp319)->f2;goto
_LL335;_LL335: _tmp31A=((struct Cyc_Absyn_ArrayType_struct*)_tmp319)->f3;goto
_LL332;}else{goto _LL333;}_LL333: goto _LL334;_LL332: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D[0]=({struct Cyc_Absyn_PointerType_struct
_tmp31E;_tmp31E.tag=4;_tmp31E.f1=({struct Cyc_Absyn_PtrInfo _tmp31F;_tmp31F.elt_typ=(
void*)_tmp31C;_tmp31F.rgn_typ=(void*)rgn;_tmp31F.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp31F.tq=_tmp31B;_tmp31F.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp320=_cycalloc(sizeof(*_tmp320));
_tmp320[0]=({struct Cyc_Absyn_Upper_b_struct _tmp321;_tmp321.tag=0;_tmp321.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp31A);_tmp321;});_tmp320;}));_tmp31F;});
_tmp31E;});_tmp31D;});if(topt != 0){if(! Cyc_Tcutil_unify(*topt,res_typ)? Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp322=_cycalloc(sizeof(*
_tmp322));_tmp322->v=(void*)res_typ;_tmp322;});Cyc_Tcutil_unchecked_cast(te,e,*
topt);res_typ=*topt;}}goto _LL330;_LL334:({void*_tmp323[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof(unsigned char),50),_tag_arr(_tmp323,sizeof(void*),0));});_LL330:;}return
res_typ;}}_LL316: {void*_tmp324=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,rgn,Cyc_Absyn_const_tqual(),(
void*)0);void*_tmp325=Cyc_Tcexp_tcExp(te,(void**)& _tmp324,e1);return Cyc_Absyn_atb_typ(
_tmp325,rgn,Cyc_Absyn_empty_tqual(),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp327;_tmp327.tag=0;_tmp327.f1=Cyc_Absyn_uint_exp(1,0);_tmp327;});_tmp326;}));}
_LL318: {void**topt2=0;if(topt != 0){void*_tmp328=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp329;struct Cyc_Absyn_Conref*_tmp32A;void*_tmp32B;void**
_tmp32C;_LL339: if((unsigned int)_tmp328 > 3?*((int*)_tmp328)== 4: 0){_LL33F:
_tmp329=((struct Cyc_Absyn_PointerType_struct*)_tmp328)->f1;_LL341: _tmp32B=(void*)
_tmp329.elt_typ;_tmp32C=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp328)->f1).elt_typ;
goto _LL340;_LL340: _tmp32A=_tmp329.nullable;goto _LL33A;}else{goto _LL33B;}_LL33B:
if((unsigned int)_tmp328 > 3?*((int*)_tmp328)== 2: 0){goto _LL33C;}else{goto _LL33D;}
_LL33D: goto _LL33E;_LL33A: topt2=(void**)_tmp32C;goto _LL338;_LL33C: topt2=({void**
_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D[0]=*topt;_tmp32D;});goto _LL338;
_LL33E: goto _LL338;_LL338:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp32F=_cycalloc(sizeof(*_tmp32F));
_tmp32F[0]=({struct Cyc_Absyn_PointerType_struct _tmp330;_tmp330.tag=4;_tmp330.f1=({
struct Cyc_Absyn_PtrInfo _tmp331;_tmp331.elt_typ=(void*)telt;_tmp331.rgn_typ=(void*)
rgn;_tmp331.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp331.tq=Cyc_Absyn_empty_tqual();_tmp331.bounds=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp333;_tmp333.tag=0;_tmp333.f1=Cyc_Absyn_uint_exp(
1,0);_tmp333;});_tmp332;}));_tmp331;});_tmp330;});_tmp32F;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ)? Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt):
0){e->topt=({struct Cyc_Core_Opt*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->v=(
void*)res_typ;_tmp32E;});Cyc_Tcutil_unchecked_cast(te,e,*topt);res_typ=*topt;}}
return res_typ;}}_LL30E:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp334=t;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_Absyn_Tqual
_tmp336;void*_tmp337;_LL343: if((unsigned int)_tmp334 > 3?*((int*)_tmp334)== 7: 0){
_LL349: _tmp337=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp334)->f1;goto
_LL348;_LL348: _tmp336=((struct Cyc_Absyn_ArrayType_struct*)_tmp334)->f2;goto
_LL347;_LL347: _tmp335=((struct Cyc_Absyn_ArrayType_struct*)_tmp334)->f3;goto
_LL344;}else{goto _LL345;}_LL345: goto _LL346;_LL344: {void*_tmp339;struct _tuple6
_tmp338=Cyc_Tcutil_addressof_props(te,e);_LL34B: _tmp339=_tmp338.f2;goto _LL34A;
_LL34A: {void*_tmp33A=_tmp335 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp33C;_tmp33C.tag=0;_tmp33C.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp335);
_tmp33C;});_tmp33B;});t=Cyc_Absyn_atb_typ(_tmp337,_tmp339,_tmp336,_tmp33A);(void*)(((
struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);return t;}}_LL346: return t;
_LL342:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,
struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);void*_tmp33D=(
void*)e->r;void*_tmp33E;_LL34D: if(*((int*)_tmp33D)== 26){goto _LL34E;}else{goto
_LL34F;}_LL34F: if(*((int*)_tmp33D)== 27){goto _LL350;}else{goto _LL351;}_LL351: if(*((
int*)_tmp33D)== 0){_LL355: _tmp33E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp33D)->f1;if((unsigned int)_tmp33E > 1?*((int*)_tmp33E)== 5: 0){goto _LL352;}
else{goto _LL353;}}else{goto _LL353;}_LL353: goto _LL354;_LL34E: goto _LL350;_LL350:
goto _LL352;_LL352: return t;_LL354: t=Cyc_Tcutil_compress(t);{void*_tmp33F=t;struct
Cyc_Absyn_Exp*_tmp340;struct Cyc_Absyn_Tqual _tmp341;void*_tmp342;_LL357: if((
unsigned int)_tmp33F > 3?*((int*)_tmp33F)== 7: 0){_LL35D: _tmp342=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp33F)->f1;goto _LL35C;_LL35C: _tmp341=((struct Cyc_Absyn_ArrayType_struct*)
_tmp33F)->f2;goto _LL35B;_LL35B: _tmp340=((struct Cyc_Absyn_ArrayType_struct*)
_tmp33F)->f3;goto _LL358;}else{goto _LL359;}_LL359: goto _LL35A;_LL358: {void*
_tmp344;struct _tuple6 _tmp343=Cyc_Tcutil_addressof_props(te,e);_LL35F: _tmp344=
_tmp343.f2;goto _LL35E;_LL35E: {void*b=_tmp340 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp346;_tmp346.tag=0;_tmp346.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp340);
_tmp346;});_tmp345;});t=Cyc_Absyn_atb_typ(_tmp342,_tmp344,_tmp341,b);Cyc_Tcutil_unchecked_cast(
te,e,t);return t;}}_LL35A: return t;_LL356:;}_LL34C:;}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp347=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp348;_LL361: if(*((int*)_tmp347)== 11){_LL365: _tmp348=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp347)->f1;goto _LL362;}else{goto _LL363;}
_LL363: goto _LL364;_LL362: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp348);(void*)(((struct
Cyc_Core_Opt*)_check_null(_tmp348->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp348->topt))->v));e->topt=_tmp348->topt;
goto _LL360;_LL364: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{void*_tmp349=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp34A;struct Cyc_Absyn_Conref*
_tmp34B;struct Cyc_Absyn_Tqual _tmp34C;struct Cyc_Absyn_Conref*_tmp34D;void*_tmp34E;
void*_tmp34F;_LL367: if((unsigned int)_tmp349 > 3?*((int*)_tmp349)== 4: 0){_LL36B:
_tmp34A=((struct Cyc_Absyn_PointerType_struct*)_tmp349)->f1;_LL370: _tmp34F=(void*)
_tmp34A.elt_typ;goto _LL36F;_LL36F: _tmp34E=(void*)_tmp34A.rgn_typ;goto _LL36E;
_LL36E: _tmp34D=_tmp34A.nullable;goto _LL36D;_LL36D: _tmp34C=_tmp34A.tq;goto _LL36C;
_LL36C: _tmp34B=_tmp34A.bounds;goto _LL368;}else{goto _LL369;}_LL369: goto _LL36A;
_LL368:{void*_tmp350=Cyc_Tcutil_compress(_tmp34F);struct Cyc_Absyn_FnInfo _tmp351;
struct Cyc_List_List*_tmp352;struct Cyc_List_List*_tmp353;struct Cyc_Absyn_VarargInfo*
_tmp354;int _tmp355;struct Cyc_List_List*_tmp356;void*_tmp357;struct Cyc_Core_Opt*
_tmp358;struct Cyc_List_List*_tmp359;_LL372: if((unsigned int)_tmp350 > 3?*((int*)
_tmp350)== 8: 0){_LL376: _tmp351=((struct Cyc_Absyn_FnType_struct*)_tmp350)->f1;
_LL37E: _tmp359=_tmp351.tvars;goto _LL37D;_LL37D: _tmp358=_tmp351.effect;goto _LL37C;
_LL37C: _tmp357=(void*)_tmp351.ret_typ;goto _LL37B;_LL37B: _tmp356=_tmp351.args;
goto _LL37A;_LL37A: _tmp355=_tmp351.c_varargs;goto _LL379;_LL379: _tmp354=_tmp351.cyc_varargs;
goto _LL378;_LL378: _tmp353=_tmp351.rgn_po;goto _LL377;_LL377: _tmp352=_tmp351.attributes;
goto _LL373;}else{goto _LL374;}_LL374: goto _LL375;_LL373: if(_tmp359 != 0){struct
_RegionHandle _tmp35A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp35A;
_push_region(rgn);{struct _tuple4 _tmp35B=({struct _tuple4 _tmp365;_tmp365.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp365.f2=rgn;_tmp365;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp35B,_tmp359);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362[0]=({struct Cyc_Absyn_FnType_struct
_tmp363;_tmp363.tag=8;_tmp363.f1=({struct Cyc_Absyn_FnInfo _tmp364;_tmp364.tvars=0;
_tmp364.effect=_tmp358;_tmp364.ret_typ=(void*)_tmp357;_tmp364.args=_tmp356;
_tmp364.c_varargs=_tmp355;_tmp364.cyc_varargs=_tmp354;_tmp364.rgn_po=_tmp353;
_tmp364.attributes=_tmp352;_tmp364;});_tmp363;});_tmp362;}));void*new_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F[0]=({
struct Cyc_Absyn_PointerType_struct _tmp360;_tmp360.tag=4;_tmp360.f1=({struct Cyc_Absyn_PtrInfo
_tmp361;_tmp361.elt_typ=(void*)ftyp;_tmp361.rgn_typ=(void*)_tmp34E;_tmp361.nullable=
_tmp34D;_tmp361.tq=_tmp34C;_tmp361.bounds=_tmp34B;_tmp361;});_tmp360;});_tmp35F;});
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Instantiate_e_struct*_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C[
0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp35D;_tmp35D.tag=12;_tmp35D.f1=inner;
_tmp35D.f2=ts;_tmp35D;});_tmp35C;})));e->topt=({struct Cyc_Core_Opt*_tmp35E=
_cycalloc(sizeof(*_tmp35E));_tmp35E->v=(void*)new_typ;_tmp35E;});};_pop_region(
rgn);}goto _LL371;_LL375: goto _LL371;_LL371:;}goto _LL366;_LL36A: goto _LL366;_LL366:;}
goto _LL360;_LL360:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp366=(void*)e->r;struct
Cyc_Absyn_Exp*_tmp367;struct _tuple1*_tmp368;struct Cyc_List_List*_tmp369;struct
Cyc_Absyn_Exp*_tmp36A;struct Cyc_List_List*_tmp36B;struct Cyc_Core_Opt*_tmp36C;
void*_tmp36D;void*_tmp36E;struct _tuple1*_tmp36F;struct Cyc_List_List*_tmp370;void*
_tmp371;void*_tmp372;struct Cyc_Absyn_Exp*_tmp373;struct Cyc_Absyn_Exp*_tmp374;
struct Cyc_Core_Opt*_tmp375;struct Cyc_Absyn_Exp*_tmp376;struct Cyc_Absyn_Exp*
_tmp377;struct Cyc_Absyn_Exp*_tmp378;struct Cyc_Absyn_Exp*_tmp379;struct Cyc_Absyn_Exp*
_tmp37A;struct Cyc_Absyn_Exp*_tmp37B;struct Cyc_Absyn_VarargCallInfo*_tmp37C;
struct Cyc_Absyn_VarargCallInfo**_tmp37D;struct Cyc_List_List*_tmp37E;struct Cyc_Absyn_Exp*
_tmp37F;struct Cyc_Absyn_Exp*_tmp380;struct Cyc_List_List*_tmp381;struct Cyc_Absyn_Exp*
_tmp382;struct Cyc_Absyn_Exp*_tmp383;void*_tmp384;struct Cyc_Absyn_Exp*_tmp385;
struct Cyc_Absyn_Exp*_tmp386;struct Cyc_Absyn_Exp*_tmp387;struct Cyc_Absyn_Exp*
_tmp388;void*_tmp389;void*_tmp38A;void*_tmp38B;struct Cyc_Absyn_Exp*_tmp38C;
struct _tagged_arr*_tmp38D;struct Cyc_Absyn_Exp*_tmp38E;struct _tagged_arr*_tmp38F;
struct Cyc_Absyn_Exp*_tmp390;struct Cyc_Absyn_Exp*_tmp391;struct Cyc_Absyn_Exp*
_tmp392;struct Cyc_List_List*_tmp393;struct Cyc_List_List*_tmp394;struct _tuple2*
_tmp395;struct Cyc_List_List*_tmp396;struct Cyc_Absyn_Stmt*_tmp397;struct Cyc_Absyn_Fndecl*
_tmp398;struct Cyc_Absyn_Exp*_tmp399;struct Cyc_Absyn_Exp*_tmp39A;struct Cyc_Absyn_Exp*
_tmp39B;struct Cyc_Absyn_Vardecl*_tmp39C;struct Cyc_Absyn_Aggrdecl*_tmp39D;struct
Cyc_Absyn_Aggrdecl**_tmp39E;struct Cyc_List_List*_tmp39F;struct _tuple1*_tmp3A0;
struct _tuple1**_tmp3A1;struct Cyc_List_List*_tmp3A2;void*_tmp3A3;struct Cyc_Absyn_Tunionfield*
_tmp3A4;struct Cyc_Absyn_Tuniondecl*_tmp3A5;struct Cyc_List_List*_tmp3A6;struct Cyc_Absyn_Enumfield*
_tmp3A7;struct Cyc_Absyn_Enumdecl*_tmp3A8;struct _tuple1*_tmp3A9;struct _tuple1**
_tmp3AA;struct Cyc_Absyn_Enumfield*_tmp3AB;void*_tmp3AC;struct _tuple1*_tmp3AD;
struct _tuple1**_tmp3AE;struct Cyc_Absyn_MallocInfo _tmp3AF;int _tmp3B0;int*_tmp3B1;
struct Cyc_Absyn_Exp*_tmp3B2;struct Cyc_Absyn_Exp**_tmp3B3;void**_tmp3B4;void***
_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B6;int _tmp3B7;_LL380: if(*((int*)_tmp366)== 11){
_LL3CC: _tmp367=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp366)->f1;goto _LL381;}
else{goto _LL382;}_LL382: if(*((int*)_tmp366)== 2){_LL3CD: _tmp368=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp366)->f1;goto _LL383;}else{goto _LL384;}_LL384: if(*((int*)_tmp366)== 8){_LL3CF:
_tmp36A=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp366)->f1;goto _LL3CE;_LL3CE:
_tmp369=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp366)->f2;goto _LL385;}else{
goto _LL386;}_LL386: if(*((int*)_tmp366)== 34){_LL3D1: _tmp36C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp366)->f1;goto _LL3D0;_LL3D0: _tmp36B=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp366)->f2;goto _LL387;}else{goto _LL388;}_LL388: if(*((int*)_tmp366)== 0){_LL3D2:
_tmp36D=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp366)->f1;goto _LL389;}else{
goto _LL38A;}_LL38A: if(*((int*)_tmp366)== 1){_LL3D4: _tmp36F=((struct Cyc_Absyn_Var_e_struct*)
_tmp366)->f1;goto _LL3D3;_LL3D3: _tmp36E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp366)->f2;goto _LL38B;}else{goto _LL38C;}_LL38C: if(*((int*)_tmp366)== 3){_LL3D6:
_tmp371=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp366)->f1;goto _LL3D5;_LL3D5:
_tmp370=((struct Cyc_Absyn_Primop_e_struct*)_tmp366)->f2;goto _LL38D;}else{goto
_LL38E;}_LL38E: if(*((int*)_tmp366)== 5){_LL3D8: _tmp373=((struct Cyc_Absyn_Increment_e_struct*)
_tmp366)->f1;goto _LL3D7;_LL3D7: _tmp372=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp366)->f2;goto _LL38F;}else{goto _LL390;}_LL390: if(*((int*)_tmp366)== 4){_LL3DB:
_tmp376=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp366)->f1;goto _LL3DA;_LL3DA:
_tmp375=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp366)->f2;goto _LL3D9;_LL3D9:
_tmp374=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp366)->f3;goto _LL391;}else{goto
_LL392;}_LL392: if(*((int*)_tmp366)== 6){_LL3DE: _tmp379=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp366)->f1;goto _LL3DD;_LL3DD: _tmp378=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp366)->f2;goto _LL3DC;_LL3DC: _tmp377=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp366)->f3;goto _LL393;}else{goto _LL394;}_LL394: if(*((int*)_tmp366)== 7){_LL3E0:
_tmp37B=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp366)->f1;goto _LL3DF;_LL3DF:
_tmp37A=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp366)->f2;goto _LL395;}else{goto
_LL396;}_LL396: if(*((int*)_tmp366)== 9){_LL3E3: _tmp37F=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp366)->f1;goto _LL3E2;_LL3E2: _tmp37E=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp366)->f2;goto _LL3E1;_LL3E1: _tmp37C=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp366)->f3;_tmp37D=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp366)->f3;goto _LL397;}else{goto _LL398;}_LL398: if(*((int*)_tmp366)== 10){
_LL3E4: _tmp380=((struct Cyc_Absyn_Throw_e_struct*)_tmp366)->f1;goto _LL399;}else{
goto _LL39A;}_LL39A: if(*((int*)_tmp366)== 12){_LL3E6: _tmp382=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp366)->f1;goto _LL3E5;_LL3E5: _tmp381=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp366)->f2;goto _LL39B;}else{goto _LL39C;}_LL39C: if(*((int*)_tmp366)== 13){
_LL3E8: _tmp384=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp366)->f1;goto _LL3E7;
_LL3E7: _tmp383=((struct Cyc_Absyn_Cast_e_struct*)_tmp366)->f2;goto _LL39D;}else{
goto _LL39E;}_LL39E: if(*((int*)_tmp366)== 14){_LL3E9: _tmp385=((struct Cyc_Absyn_Address_e_struct*)
_tmp366)->f1;goto _LL39F;}else{goto _LL3A0;}_LL3A0: if(*((int*)_tmp366)== 15){
_LL3EB: _tmp387=((struct Cyc_Absyn_New_e_struct*)_tmp366)->f1;goto _LL3EA;_LL3EA:
_tmp386=((struct Cyc_Absyn_New_e_struct*)_tmp366)->f2;goto _LL3A1;}else{goto _LL3A2;}
_LL3A2: if(*((int*)_tmp366)== 17){_LL3EC: _tmp388=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp366)->f1;goto _LL3A3;}else{goto _LL3A4;}_LL3A4: if(*((int*)_tmp366)== 16){
_LL3ED: _tmp389=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp366)->f1;goto
_LL3A5;}else{goto _LL3A6;}_LL3A6: if(*((int*)_tmp366)== 18){_LL3EF: _tmp38B=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp366)->f1;goto _LL3EE;_LL3EE: _tmp38A=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp366)->f2;goto _LL3A7;}else{goto _LL3A8;}
_LL3A8: if(*((int*)_tmp366)== 19){goto _LL3A9;}else{goto _LL3AA;}_LL3AA: if(*((int*)
_tmp366)== 20){_LL3F0: _tmp38C=((struct Cyc_Absyn_Deref_e_struct*)_tmp366)->f1;
goto _LL3AB;}else{goto _LL3AC;}_LL3AC: if(*((int*)_tmp366)== 21){_LL3F2: _tmp38E=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp366)->f1;goto _LL3F1;_LL3F1: _tmp38D=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp366)->f2;goto _LL3AD;}else{goto _LL3AE;}
_LL3AE: if(*((int*)_tmp366)== 22){_LL3F4: _tmp390=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp366)->f1;goto _LL3F3;_LL3F3: _tmp38F=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp366)->f2;goto _LL3AF;}else{goto _LL3B0;}_LL3B0: if(*((int*)_tmp366)== 23){
_LL3F6: _tmp392=((struct Cyc_Absyn_Subscript_e_struct*)_tmp366)->f1;goto _LL3F5;
_LL3F5: _tmp391=((struct Cyc_Absyn_Subscript_e_struct*)_tmp366)->f2;goto _LL3B1;}
else{goto _LL3B2;}_LL3B2: if(*((int*)_tmp366)== 24){_LL3F7: _tmp393=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp366)->f1;goto _LL3B3;}else{goto _LL3B4;}_LL3B4: if(*((int*)_tmp366)== 25){
_LL3F9: _tmp395=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp366)->f1;goto _LL3F8;
_LL3F8: _tmp394=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp366)->f2;goto _LL3B5;}
else{goto _LL3B6;}_LL3B6: if(*((int*)_tmp366)== 26){_LL3FA: _tmp396=((struct Cyc_Absyn_Array_e_struct*)
_tmp366)->f1;goto _LL3B7;}else{goto _LL3B8;}_LL3B8: if(*((int*)_tmp366)== 35){
_LL3FB: _tmp397=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp366)->f1;goto _LL3B9;}
else{goto _LL3BA;}_LL3BA: if(*((int*)_tmp366)== 36){_LL3FC: _tmp398=((struct Cyc_Absyn_Codegen_e_struct*)
_tmp366)->f1;goto _LL3BB;}else{goto _LL3BC;}_LL3BC: if(*((int*)_tmp366)== 37){
_LL3FD: _tmp399=((struct Cyc_Absyn_Fill_e_struct*)_tmp366)->f1;goto _LL3BD;}else{
goto _LL3BE;}_LL3BE: if(*((int*)_tmp366)== 27){_LL400: _tmp39C=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp366)->f1;goto _LL3FF;_LL3FF: _tmp39B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp366)->f2;goto _LL3FE;_LL3FE: _tmp39A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp366)->f3;goto _LL3BF;}else{goto _LL3C0;}_LL3C0: if(*((int*)_tmp366)== 28){
_LL403: _tmp3A0=((struct Cyc_Absyn_Struct_e_struct*)_tmp366)->f1;_tmp3A1=(struct
_tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp366)->f1;goto _LL402;_LL402:
_tmp39F=((struct Cyc_Absyn_Struct_e_struct*)_tmp366)->f2;goto _LL401;_LL401:
_tmp39D=((struct Cyc_Absyn_Struct_e_struct*)_tmp366)->f3;_tmp39E=(struct Cyc_Absyn_Aggrdecl**)&((
struct Cyc_Absyn_Struct_e_struct*)_tmp366)->f3;goto _LL3C1;}else{goto _LL3C2;}
_LL3C2: if(*((int*)_tmp366)== 29){_LL405: _tmp3A3=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp366)->f1;goto _LL404;_LL404: _tmp3A2=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp366)->f2;goto _LL3C3;}else{goto _LL3C4;}_LL3C4: if(*((int*)_tmp366)== 30){
_LL408: _tmp3A6=((struct Cyc_Absyn_Tunion_e_struct*)_tmp366)->f1;goto _LL407;_LL407:
_tmp3A5=((struct Cyc_Absyn_Tunion_e_struct*)_tmp366)->f2;goto _LL406;_LL406:
_tmp3A4=((struct Cyc_Absyn_Tunion_e_struct*)_tmp366)->f3;goto _LL3C5;}else{goto
_LL3C6;}_LL3C6: if(*((int*)_tmp366)== 31){_LL40B: _tmp3A9=((struct Cyc_Absyn_Enum_e_struct*)
_tmp366)->f1;_tmp3AA=(struct _tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp366)->f1;
goto _LL40A;_LL40A: _tmp3A8=((struct Cyc_Absyn_Enum_e_struct*)_tmp366)->f2;goto
_LL409;_LL409: _tmp3A7=((struct Cyc_Absyn_Enum_e_struct*)_tmp366)->f3;goto _LL3C7;}
else{goto _LL3C8;}_LL3C8: if(*((int*)_tmp366)== 32){_LL40E: _tmp3AD=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp366)->f1;_tmp3AE=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp366)->f1;goto _LL40D;_LL40D: _tmp3AC=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp366)->f2;goto _LL40C;_LL40C: _tmp3AB=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp366)->f3;goto _LL3C9;}else{goto _LL3CA;}_LL3CA: if(*((int*)_tmp366)== 33){
_LL40F: _tmp3AF=((struct Cyc_Absyn_Malloc_e_struct*)_tmp366)->f1;_LL414: _tmp3B7=
_tmp3AF.is_calloc;goto _LL413;_LL413: _tmp3B6=_tmp3AF.rgn;goto _LL412;_LL412:
_tmp3B4=_tmp3AF.elt_type;_tmp3B5=(void***)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp366)->f1).elt_type;goto _LL411;_LL411: _tmp3B2=_tmp3AF.num_elts;_tmp3B3=(
struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp366)->f1).num_elts;
goto _LL410;_LL410: _tmp3B0=_tmp3AF.fat_result;_tmp3B1=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp366)->f1).fat_result;goto _LL3CB;}else{goto _LL37F;}_LL381: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp367);return;_LL383: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp368);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL385: Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp36A,_tmp369);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL387: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp36B);
Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL389: t=Cyc_Tcexp_tcConst(te,loc,topt,
_tmp36D,e);goto _LL37F;_LL38B: t=Cyc_Tcexp_tcVar(te,loc,_tmp36F,_tmp36E);goto
_LL37F;_LL38D: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp371,_tmp370);goto _LL37F;
_LL38F: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp373,_tmp372);goto _LL37F;_LL391: t=
Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp376,_tmp375,_tmp374);goto _LL37F;_LL393: t=
Cyc_Tcexp_tcConditional(te,loc,topt,_tmp379,_tmp378,_tmp377);goto _LL37F;_LL395: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp37B,_tmp37A);goto _LL37F;_LL397: t=Cyc_Tcexp_tcFnCall(
te,loc,topt,_tmp37F,_tmp37E,_tmp37D);goto _LL37F;_LL399: t=Cyc_Tcexp_tcThrow(te,
loc,topt,_tmp380);goto _LL37F;_LL39B: t=Cyc_Tcexp_tcInstantiate(te,loc,topt,
_tmp382,_tmp381);goto _LL37F;_LL39D: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp384,
_tmp383);goto _LL37F;_LL39F: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp385);goto
_LL37F;_LL3A1: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp387,e,_tmp386);goto _LL37F;_LL3A3: {
void*_tmp3B8=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp388);t=Cyc_Tcexp_tcSizeof(te,loc,
topt,_tmp3B8);goto _LL37F;}_LL3A5: t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp389);goto
_LL37F;_LL3A7: t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp38B,_tmp38A);goto _LL37F;
_LL3A9:({void*_tmp3B9[0]={};Cyc_Tcutil_terr(loc,_tag_arr("gen() not in top-level initializer",
sizeof(unsigned char),35),_tag_arr(_tmp3B9,sizeof(void*),0));});return;_LL3AB: t=
Cyc_Tcexp_tcDeref(te,loc,topt,_tmp38C);goto _LL37F;_LL3AD: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp38E,_tmp38D);goto _LL37F;_LL3AF: t=Cyc_Tcexp_tcAggrArrow(te,loc,
topt,_tmp390,_tmp38F);goto _LL37F;_LL3B1: t=Cyc_Tcexp_tcSubscript(te,loc,topt,
_tmp392,_tmp391);goto _LL37F;_LL3B3: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp393);goto
_LL37F;_LL3B5: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp395,_tmp394);goto _LL37F;
_LL3B7: {void**elt_topt=0;if(topt != 0){void*_tmp3BA=Cyc_Tcutil_compress(*topt);
void*_tmp3BB;void**_tmp3BC;_LL416: if((unsigned int)_tmp3BA > 3?*((int*)_tmp3BA)== 
7: 0){_LL41A: _tmp3BB=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3BA)->f1;
_tmp3BC=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3BA)->f1);goto
_LL417;}else{goto _LL418;}_LL418: goto _LL419;_LL417: elt_topt=(void**)_tmp3BC;goto
_LL415;_LL419: goto _LL415;_LL415:;}t=Cyc_Tcexp_tcArray(te,loc,elt_topt,_tmp396);
goto _LL37F;}_LL3B9: t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp397);goto _LL37F;_LL3BB:
t=Cyc_Tcexp_tcCodegen(te,loc,topt,_tmp398);goto _LL37F;_LL3BD: t=Cyc_Tcexp_tcFill(
te,loc,topt,_tmp399);goto _LL37F;_LL3BF: t=Cyc_Tcexp_tcComprehension(te,loc,topt,
_tmp39C,_tmp39B,_tmp39A);goto _LL37F;_LL3C1: t=Cyc_Tcexp_tcStruct(te,loc,topt,
_tmp3A1,_tmp39F,_tmp39E);goto _LL37F;_LL3C3: t=Cyc_Tcexp_tcAnonStruct(te,loc,
_tmp3A3,_tmp3A2);goto _LL37F;_LL3C5: t=Cyc_Tcexp_tcTunion(te,loc,topt,e,_tmp3A6,
_tmp3A5,_tmp3A4);goto _LL37F;_LL3C7:*_tmp3AA=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp3A7))->name;t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp3BD=
_cycalloc(sizeof(*_tmp3BD));_tmp3BD[0]=({struct Cyc_Absyn_EnumType_struct _tmp3BE;
_tmp3BE.tag=12;_tmp3BE.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp3A8))->name;
_tmp3BE.f2=_tmp3A8;_tmp3BE;});_tmp3BD;});goto _LL37F;_LL3C9:*_tmp3AE=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp3AB))->name;t=_tmp3AC;goto _LL37F;_LL3CB: t=Cyc_Tcexp_tcMalloc(te,
loc,topt,_tmp3B6,_tmp3B5,_tmp3B3,_tmp3B7,_tmp3B1);goto _LL37F;_LL37F:;}e->topt=({
struct Cyc_Core_Opt*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF->v=(void*)t;
_tmp3BF;});}
