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
extern void* _region_malloc(struct _RegionHandle *, unsigned);
extern void* _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void  _free_region(struct _RegionHandle *);

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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(
struct _tuple0*);extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*x,
int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*name;int*identity;void*kind;
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrInfo{
void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Tqual
tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*
name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct
Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
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
15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*
tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_uchar_t;
extern void*Cyc_Absyn_ushort_t;extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_ulonglong_t;
extern void*Cyc_Absyn_schar_t;extern void*Cyc_Absyn_sshort_t;extern void*Cyc_Absyn_sint_t;
extern void*Cyc_Absyn_slonglong_t;extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b);void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_at_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);void*Cyc_Absyn_pointer_expand(
void*);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct
Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_Std_zstrcmp(struct _tagged_arr,
struct _tagged_arr);struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_vrprintf(struct _RegionHandle*r1,
struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{int
tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct
Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);
struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _tagged_arr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[
11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern
unsigned char Cyc_Dict_Absent[11];struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};
struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct
Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;
};struct Cyc_Tcenv_Frame_struct{int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);int Cyc_Tcutil_kind_leq(
void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,
struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
struct _tuple4{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple5{
struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_r_make_inst_var(
struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);
int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);struct _tuple6{int f1;void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_okay_szofarg(
void*t);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr,struct Cyc_Position_Segment*);struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tagged_arr msg_part);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tagged_arr msg,
struct _tagged_arr ap){({void*_tmp0[0]={};Cyc_Tcutil_terr(loc,(struct _tagged_arr)
Cyc_Std_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_arr(_tmp0,sizeof(void*),0));});
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp1=_cycalloc(sizeof(*_tmp1));
_tmp1->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1;}));}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp2;_push_handler(& _tmp2);{int _tmp4=0;if(setjmp(_tmp2.handler))
_tmp4=1;if(!_tmp4){{void*_tmp5=Cyc_Tcenv_lookup_ordinary(te,e->loc,q);void*_tmp6;
struct Cyc_Absyn_Enumdecl*_tmp7;struct Cyc_Absyn_Enumfield*_tmp8;void*_tmp9;struct
Cyc_Absyn_Enumfield*_tmpA;struct Cyc_Absyn_Tuniondecl*_tmpB;struct Cyc_Absyn_Tunionfield*
_tmpC;_LL1: if(*((int*)_tmp5)!= 0)goto _LL3;_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp5)->f1;_LL2:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmpD=
_cycalloc(sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Absyn_Var_e_struct _tmpE;_tmpE.tag=
1;_tmpE.f1=q;_tmpE.f2=(void*)_tmp6;_tmpE;});_tmpD;})));goto _LL0;_LL3: if(*((int*)
_tmp5)!= 3)goto _LL5;_tmp7=((struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((
struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f2;_LL4:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Enum_e_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct
Cyc_Absyn_Enum_e_struct _tmp10;_tmp10.tag=31;_tmp10.f1=q;_tmp10.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp7;_tmp10.f3=(struct Cyc_Absyn_Enumfield*)_tmp8;_tmp10;});_tmpF;})));goto _LL0;
_LL5: if(*((int*)_tmp5)!= 4)goto _LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5)->f1;_tmpA=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp11=_cycalloc(sizeof(*
_tmp11));_tmp11[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp12;_tmp12.tag=32;
_tmp12.f1=q;_tmp12.f2=(void*)_tmp9;_tmp12.f3=(struct Cyc_Absyn_Enumfield*)_tmpA;
_tmp12;});_tmp11;})));goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((
struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f2;_LL8:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*
_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=({struct Cyc_Absyn_Tunion_e_struct
_tmp14;_tmp14.tag=30;_tmp14.f1=0;_tmp14.f2=_tmpB;_tmp14.f3=_tmpC;_tmp14;});
_tmp13;})));goto _LL0;_LL9: if(*((int*)_tmp5)!= 1)goto _LL0;_LLA:({struct Cyc_Std_String_pa_struct
_tmp16;_tmp16.tag=0;_tmp16.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(q);{
void*_tmp15[1]={& _tmp16};Cyc_Tcutil_terr(e->loc,_tag_arr("bad occurrence of type name %s",
sizeof(unsigned char),31),_tag_arr(_tmp15,sizeof(void*),1));}});(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp17=_cycalloc(sizeof(*_tmp17));
_tmp17[0]=({struct Cyc_Absyn_Var_e_struct _tmp18;_tmp18.tag=1;_tmp18.f1=q;_tmp18.f2=(
void*)((void*)0);_tmp18;});_tmp17;})));goto _LL0;_LL0:;};_pop_handler();}else{
void*_tmp3=(void*)_exn_thrown;void*_tmp1A=_tmp3;_LLC: if(_tmp1A != Cyc_Dict_Absent)
goto _LLE;_LLD:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp1B=
_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Absyn_Var_e_struct _tmp1C;_tmp1C.tag=
1;_tmp1C.f1=q;_tmp1C.f2=(void*)((void*)0);_tmp1C;});_tmp1B;})));goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp1A);_LLB:;}}}struct _tuple7{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple7*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){return({struct _tuple7*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->f1=0;_tmp1D->f2=
e;_tmp1D;});}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1E=(
void*)e1->r;struct _tuple1*_tmp1F;_LL11: if(*((int*)_tmp1E)!= 2)goto _LL13;_tmp1F=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp1E)->f1;_LL12: {struct _handler_cons
_tmp20;_push_handler(& _tmp20);{int _tmp22=0;if(setjmp(_tmp20.handler))_tmp22=1;
if(!_tmp22){{void*_tmp23=Cyc_Tcenv_lookup_ordinary(te,e1->loc,_tmp1F);void*
_tmp24;struct Cyc_Absyn_Tuniondecl*_tmp25;struct Cyc_Absyn_Tunionfield*_tmp26;
struct Cyc_Absyn_Aggrdecl*_tmp27;_LL16: if(*((int*)_tmp23)!= 0)goto _LL18;_tmp24=(
void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp23)->f1;_LL17:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[
0]=({struct Cyc_Absyn_FnCall_e_struct _tmp29;_tmp29.tag=9;_tmp29.f1=e1;_tmp29.f2=
es;_tmp29.f3=0;_tmp29;});_tmp28;})));_npop_handler(0);return;_LL18: if(*((int*)
_tmp23)!= 2)goto _LL1A;_tmp25=((struct Cyc_Tcenv_TunionRes_struct*)_tmp23)->f1;
_tmp26=((struct Cyc_Tcenv_TunionRes_struct*)_tmp23)->f2;_LL19: if(_tmp26->typs == 0)({
struct Cyc_Std_String_pa_struct _tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp26->name);{void*_tmp2A[1]={& _tmp2B};Cyc_Tcutil_terr(
e->loc,_tag_arr("%s is a constant, not a function",sizeof(unsigned char),33),
_tag_arr(_tmp2A,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*
_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C[0]=({struct Cyc_Absyn_Tunion_e_struct
_tmp2D;_tmp2D.tag=30;_tmp2D.f1=es;_tmp2D.f2=_tmp25;_tmp2D.f3=_tmp26;_tmp2D;});
_tmp2C;})));_npop_handler(0);return;_LL1A: if(*((int*)_tmp23)!= 1)goto _LL1C;
_tmp27=((struct Cyc_Tcenv_AggrRes_struct*)_tmp23)->f1;_LL1B: {struct Cyc_List_List*
_tmp2E=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Struct_e_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[
0]=({struct Cyc_Absyn_Struct_e_struct _tmp30;_tmp30.tag=28;_tmp30.f1=_tmp27->name;
_tmp30.f2=0;_tmp30.f3=_tmp2E;_tmp30.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp27;_tmp30;});
_tmp2F;})));_npop_handler(0);return;}_LL1C: if(*((int*)_tmp23)!= 4)goto _LL1E;
_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp23)!= 3)goto _LL15;_LL1F:({struct Cyc_Std_String_pa_struct
_tmp32;_tmp32.tag=0;_tmp32.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1F);{
void*_tmp31[1]={& _tmp32};Cyc_Tcutil_terr(e->loc,_tag_arr("%s is an enum constructor, not a function",
sizeof(unsigned char),42),_tag_arr(_tmp31,sizeof(void*),1));}});_npop_handler(0);
return;_LL15:;};_pop_handler();}else{void*_tmp21=(void*)_exn_thrown;void*_tmp34=
_tmp21;_LL21: if(_tmp34 != Cyc_Dict_Absent)goto _LL23;_LL22:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[
0]=({struct Cyc_Absyn_FnCall_e_struct _tmp36;_tmp36.tag=9;_tmp36.f1=e1;_tmp36.f2=
es;_tmp36.f3=0;_tmp36;});_tmp35;})));return;_LL23:;_LL24:(void)_throw(_tmp34);
_LL20:;}}}_LL13:;_LL14:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp38;_tmp38.tag=9;_tmp38.f1=e1;_tmp38.f2=es;_tmp38.f3=0;_tmp38;});_tmp37;})));
return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3A;_tmp3A.tag=26;_tmp3A.f1=des;_tmp3A;});_tmp39;})));return;}{void*t=*topt;
void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp3C;void*_tmp3D;
void*_tmp3E;struct Cyc_Absyn_Tqual _tmp3F;_LL26: if(_tmp3B <= (void*)3?1:*((int*)
_tmp3B)!= 10)goto _LL28;_tmp3C=((struct Cyc_Absyn_AggrType_struct*)_tmp3B)->f1;
_tmp3D=(void*)_tmp3C.aggr_info;_LL27:{void*_tmp40=_tmp3D;struct Cyc_Absyn_Aggrdecl**
_tmp41;struct Cyc_Absyn_Aggrdecl*_tmp42;_LL2F: if(*((int*)_tmp40)!= 0)goto _LL31;
_LL30:({void*_tmp43[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("struct type not properly set",sizeof(unsigned char),29),_tag_arr(
_tmp43,sizeof(void*),0));});_LL31: if(*((int*)_tmp40)!= 1)goto _LL2E;_tmp41=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp40)->f1;_tmp42=*_tmp41;_LL32:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp44=_cycalloc(sizeof(*_tmp44));
_tmp44[0]=({struct Cyc_Absyn_Struct_e_struct _tmp45;_tmp45.tag=28;_tmp45.f1=_tmp42->name;
_tmp45.f2=0;_tmp45.f3=des;_tmp45.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp42;_tmp45;});
_tmp44;})));_LL2E:;}goto _LL25;_LL28: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 7)
goto _LL2A;_tmp3E=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3B)->f1;_tmp3F=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3B)->f2;_LL29:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Array_e_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({
struct Cyc_Absyn_Array_e_struct _tmp47;_tmp47.tag=26;_tmp47.f1=des;_tmp47;});
_tmp46;})));goto _LL25;_LL2A: if(_tmp3B <= (void*)3?1:*((int*)_tmp3B)!= 11)goto
_LL2C;_LL2B:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*
_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48[0]=({struct Cyc_Absyn_AnonStruct_e_struct
_tmp49;_tmp49.tag=29;_tmp49.f1=(void*)t;_tmp49.f2=des;_tmp49;});_tmp48;})));goto
_LL25;_LL2C:;_LL2D:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_Array_e_struct
_tmp4B;_tmp4B.tag=26;_tmp4B.f1=des;_tmp4B;});_tmp4A;})));goto _LL25;_LL25:;}}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct
Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_tagged_arr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(te,(
void**)& Cyc_Absyn_sint_t,e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_Std_String_pa_struct
_tmp4E;_tmp4E.tag=0;_tmp4E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _tagged_arr)msg_part;{void*_tmp4C[2]={&
_tmp4D,& _tmp4E};Cyc_Tcutil_terr(e->loc,_tag_arr("test of %s has type %s instead of integral or * type",
sizeof(unsigned char),53),_tag_arr(_tmp4C,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*c,struct
Cyc_Absyn_Exp*e){void*t;{void*_tmp4F=c;void*_tmp50;void*_tmp51;void*_tmp52;void*
_tmp53;int _tmp54;struct _tagged_arr _tmp55;_LL34: if(_tmp4F <= (void*)1?1:*((int*)
_tmp4F)!= 0)goto _LL36;_tmp50=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp4F)->f1;
_LL35: t=_tmp50 == (void*)1?Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;goto _LL33;_LL36:
if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 1)goto _LL38;_tmp51=(void*)((struct Cyc_Absyn_Short_c_struct*)
_tmp4F)->f1;_LL37: t=_tmp51 == (void*)1?Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;goto
_LL33;_LL38: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 3)goto _LL3A;_tmp52=(void*)((
struct Cyc_Absyn_LongLong_c_struct*)_tmp4F)->f1;_LL39: t=_tmp52 == (void*)1?Cyc_Absyn_ulonglong_t:
Cyc_Absyn_slonglong_t;goto _LL33;_LL3A: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 4)
goto _LL3C;_LL3B: t=Cyc_Absyn_float_typ;goto _LL33;_LL3C: if(_tmp4F <= (void*)1?1:*((
int*)_tmp4F)!= 2)goto _LL3E;_tmp53=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp4F)->f1;
_tmp54=((struct Cyc_Absyn_Int_c_struct*)_tmp4F)->f2;_LL3D: if(topt == 0)t=_tmp53 == (
void*)1?Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;else{void*_tmp56=Cyc_Tcutil_compress(*
topt);void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;_LL43: if(
_tmp56 <= (void*)3?1:*((int*)_tmp56)!= 5)goto _LL45;_tmp57=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp56)->f1;_tmp58=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56)->f2;if((int)
_tmp58 != 0)goto _LL45;_LL44: t=_tmp57 == (void*)1?Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp5C=_cycalloc(
sizeof(*_tmp5C));_tmp5C[0]=({struct Cyc_Absyn_Const_e_struct _tmp5D;_tmp5D.tag=0;
_tmp5D.f1=(void*)((void*)({struct Cyc_Absyn_Char_c_struct*_tmp5E=_cycalloc(
sizeof(*_tmp5E));_tmp5E[0]=({struct Cyc_Absyn_Char_c_struct _tmp5F;_tmp5F.tag=0;
_tmp5F.f1=(void*)_tmp57;_tmp5F.f2=(unsigned char)_tmp54;_tmp5F;});_tmp5E;}));
_tmp5D;});_tmp5C;})));goto _LL42;_LL45: if(_tmp56 <= (void*)3?1:*((int*)_tmp56)!= 5)
goto _LL47;_tmp59=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56)->f1;_tmp5A=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp56)->f2;if((int)_tmp5A != 1)goto _LL47;
_LL46: t=_tmp59 == (void*)1?Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp60=_cycalloc(sizeof(*_tmp60));
_tmp60[0]=({struct Cyc_Absyn_Const_e_struct _tmp61;_tmp61.tag=0;_tmp61.f1=(void*)((
void*)({struct Cyc_Absyn_Short_c_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[
0]=({struct Cyc_Absyn_Short_c_struct _tmp63;_tmp63.tag=1;_tmp63.f1=(void*)_tmp59;
_tmp63.f2=(short)_tmp54;_tmp63;});_tmp62;}));_tmp61;});_tmp60;})));goto _LL42;
_LL47: if(_tmp56 <= (void*)3?1:*((int*)_tmp56)!= 5)goto _LL49;_tmp5B=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp56)->f1;_LL48: t=_tmp5B == (void*)1?Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t;goto _LL42;_LL49: if(_tmp56 <= (void*)3?1:*((int*)_tmp56)!= 4)goto
_LL4B;if(!(_tmp54 == 0))goto _LL4B;_LL4A:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_Const_e_struct
_tmp65;_tmp65.tag=0;_tmp65.f1=(void*)((void*)0);_tmp65;});_tmp64;})));{struct Cyc_List_List*
_tmp66=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67[0]=({struct Cyc_Absyn_PointerType_struct
_tmp68;_tmp68.tag=4;_tmp68.f1=({struct Cyc_Absyn_PtrInfo _tmp69;_tmp69.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->v=_tmp66;_tmp6B;}));_tmp69.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->v=_tmp66;_tmp6A;}));_tmp69.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp69.tq=Cyc_Absyn_empty_tqual();
_tmp69.bounds=Cyc_Absyn_empty_conref();_tmp69;});_tmp68;});_tmp67;});goto _LL42;}
_LL4B:;_LL4C: t=_tmp53 == (void*)1?Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;goto _LL42;
_LL42:;}goto _LL33;_LL3E: if(_tmp4F <= (void*)1?1:*((int*)_tmp4F)!= 5)goto _LL40;
_tmp55=((struct Cyc_Absyn_String_c_struct*)_tmp4F)->f1;_LL3F: {int len=(int)
_get_arr_size(_tmp55,sizeof(unsigned char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_Int_c_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=({
struct Cyc_Absyn_Int_c_struct _tmp79;_tmp79.tag=2;_tmp79.f1=(void*)((void*)1);
_tmp79.f2=len;_tmp79;});_tmp78;}),loc);elen->topt=({struct Cyc_Core_Opt*_tmp6C=
_cycalloc(sizeof(*_tmp6C));_tmp6C->v=(void*)Cyc_Absyn_uint_t;_tmp6C;});t=Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,(void*)2,Cyc_Absyn_const_tqual(),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp6E;_tmp6E.tag=0;_tmp6E.f1=elen;_tmp6E;});_tmp6D;}));if(topt != 0){void*_tmp6F=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Tqual _tmp70;_LL4E: if(_tmp6F <= (void*)
3?1:*((int*)_tmp6F)!= 7)goto _LL50;_tmp70=((struct Cyc_Absyn_ArrayType_struct*)
_tmp6F)->f2;_LL4F: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp71=
_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_ArrayType_struct _tmp72;
_tmp72.tag=7;_tmp72.f1=(void*)Cyc_Absyn_uchar_t;_tmp72.f2=_tmp70;_tmp72.f3=(
struct Cyc_Absyn_Exp*)elen;_tmp72;});_tmp71;});_LL50: if(_tmp6F <= (void*)3?1:*((
int*)_tmp6F)!= 4)goto _LL52;_LL51: if(!Cyc_Tcutil_unify(*topt,t)?Cyc_Tcutil_silent_castable(
te,loc,t,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp73=_cycalloc(sizeof(*_tmp73));
_tmp73->v=(void*)t;_tmp73;});Cyc_Tcutil_unchecked_cast(te,e,*topt);t=*topt;}
else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp74;})),Cyc_Absyn_const_tqual(),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp76;_tmp76.tag=0;_tmp76.f1=elen;_tmp76;});
_tmp75;}));if(!Cyc_Tcutil_unify(*topt,t)?Cyc_Tcutil_silent_castable(te,loc,t,*
topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->v=(
void*)t;_tmp77;});Cyc_Tcutil_unchecked_cast(te,e,*topt);t=*topt;}}goto _LL4D;
_LL52:;_LL53: goto _LL4D;_LL4D:;}return t;}_LL40: if((int)_tmp4F != 0)goto _LL33;_LL41: {
struct Cyc_List_List*_tmp7A=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({struct Cyc_Absyn_PointerType_struct
_tmp7C;_tmp7C.tag=4;_tmp7C.f1=({struct Cyc_Absyn_PtrInfo _tmp7D;_tmp7D.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->v=_tmp7A;_tmp7F;}));_tmp7D.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->v=_tmp7A;_tmp7E;}));_tmp7D.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp7D.tq=Cyc_Absyn_empty_tqual();
_tmp7D.bounds=Cyc_Absyn_empty_conref();_tmp7D;});_tmp7C;});_tmp7B;});goto _LL33;}
_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp80=b;struct Cyc_Absyn_Vardecl*_tmp81;struct
Cyc_Absyn_Fndecl*_tmp82;struct Cyc_Absyn_Vardecl*_tmp83;struct Cyc_Absyn_Vardecl*
_tmp84;struct Cyc_Absyn_Vardecl*_tmp85;_LL55: if((int)_tmp80 != 0)goto _LL57;_LL56:
return({struct Cyc_Std_String_pa_struct _tmp87;_tmp87.tag=0;_tmp87.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(q);{void*_tmp86[1]={& _tmp87};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("undeclared identifier: %s",sizeof(unsigned char),26),_tag_arr(
_tmp86,sizeof(void*),1));}});_LL57: if(_tmp80 <= (void*)1?1:*((int*)_tmp80)!= 0)
goto _LL59;_tmp81=((struct Cyc_Absyn_Global_b_struct*)_tmp80)->f1;_LL58:*q=*_tmp81->name;
return(void*)_tmp81->type;_LL59: if(_tmp80 <= (void*)1?1:*((int*)_tmp80)!= 1)goto
_LL5B;_tmp82=((struct Cyc_Absyn_Funname_b_struct*)_tmp80)->f1;_LL5A:*q=*_tmp82->name;
return Cyc_Tcutil_fndecl2typ(_tmp82);_LL5B: if(_tmp80 <= (void*)1?1:*((int*)_tmp80)
!= 4)goto _LL5D;_tmp83=((struct Cyc_Absyn_Pat_b_struct*)_tmp80)->f1;_LL5C: _tmp84=
_tmp83;goto _LL5E;_LL5D: if(_tmp80 <= (void*)1?1:*((int*)_tmp80)!= 3)goto _LL5F;
_tmp84=((struct Cyc_Absyn_Local_b_struct*)_tmp80)->f1;_LL5E: _tmp85=_tmp84;goto
_LL60;_LL5F: if(_tmp80 <= (void*)1?1:*((int*)_tmp80)!= 2)goto _LL54;_tmp85=((struct
Cyc_Absyn_Param_b_struct*)_tmp80)->f1;_LL60:(*q).f1=(void*)0;return(void*)_tmp85->type;
_LL54:;}static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
fmt,struct Cyc_Core_Opt*opt_args,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr,struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{
void*_tmp88=(void*)fmt->r;void*_tmp89;struct _tagged_arr _tmp8A;struct Cyc_Absyn_Exp*
_tmp8B;struct Cyc_Absyn_Exp _tmp8C;void*_tmp8D;void*_tmp8E;struct _tagged_arr _tmp8F;
_LL62: if(*((int*)_tmp88)!= 0)goto _LL64;_tmp89=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp88)->f1;if(_tmp89 <= (void*)1?1:*((int*)_tmp89)!= 5)goto _LL64;_tmp8A=((struct
Cyc_Absyn_String_c_struct*)_tmp89)->f1;_LL63: _tmp8F=_tmp8A;goto _LL65;_LL64: if(*((
int*)_tmp88)!= 13)goto _LL66;_tmp8B=((struct Cyc_Absyn_Cast_e_struct*)_tmp88)->f2;
_tmp8C=*_tmp8B;_tmp8D=(void*)_tmp8C.r;if(*((int*)_tmp8D)!= 0)goto _LL66;_tmp8E=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp8D)->f1;if(_tmp8E <= (void*)1?1:*((
int*)_tmp8E)!= 5)goto _LL66;_tmp8F=((struct Cyc_Absyn_String_c_struct*)_tmp8E)->f1;
_LL65: desc_types=type_getter(te,(struct _tagged_arr)_tmp8F,fmt->loc);goto _LL61;
_LL66:;_LL67: return;_LL61:;}if(opt_args != 0){struct Cyc_List_List*_tmp90=(struct
Cyc_List_List*)opt_args->v;for(0;desc_types != 0?_tmp90 != 0: 0;(desc_types=
desc_types->tl,_tmp90=_tmp90->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_tmp90->hd;if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct
Cyc_Std_String_pa_struct _tmp93;_tmp93.tag=0;_tmp93.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp92;_tmp92.tag=0;_tmp92.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmp91[2]={& _tmp92,& _tmp93};Cyc_Tcutil_terr(e->loc,_tag_arr("descriptor has type \n%s\n but argument has type \n%s",
sizeof(unsigned char),51),_tag_arr(_tmp91,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
if(desc_types != 0)({void*_tmp94[0]={};Cyc_Tcutil_terr(fmt->loc,_tag_arr("too few arguments",
sizeof(unsigned char),18),_tag_arr(_tmp94,sizeof(void*),0));});if(_tmp90 != 0)({
void*_tmp95[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp90->hd)->loc,
_tag_arr("too many arguments",sizeof(unsigned char),19),_tag_arr(_tmp95,sizeof(
void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp96=p;_LL69: if((
int)_tmp96 != 0)goto _LL6B;_LL6A: goto _LL6C;_LL6B: if((int)_tmp96 != 2)goto _LL6D;
_LL6C: if(!Cyc_Tcutil_is_numeric(e))({struct Cyc_Std_String_pa_struct _tmp98;_tmp98.tag=
0;_tmp98.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp97[1]={&
_tmp98};Cyc_Tcutil_terr(loc,_tag_arr("expecting arithmetic type but found %s",
sizeof(unsigned char),39),_tag_arr(_tmp97,sizeof(void*),1));}});return(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL6D: if((int)_tmp96 != 11)goto _LL6F;
_LL6E: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({
struct Cyc_Std_String_pa_struct _tmp9A;_tmp9A.tag=0;_tmp9A.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t);{void*_tmp99[1]={& _tmp9A};Cyc_Tcutil_terr(loc,_tag_arr("expecting integral or * type but found %s",
sizeof(unsigned char),42),_tag_arr(_tmp99,sizeof(void*),1));}});return Cyc_Absyn_sint_t;
_LL6F: if((int)_tmp96 != 12)goto _LL71;_LL70: if(!Cyc_Tcutil_is_integral(e))({struct
Cyc_Std_String_pa_struct _tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmp9B[1]={& _tmp9C};Cyc_Tcutil_terr(loc,_tag_arr("expecting integral type but found %s",
sizeof(unsigned char),37),_tag_arr(_tmp9B,sizeof(void*),1));}});return(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL71: if((int)_tmp96 != 19)goto _LL73;
_LL72:{void*_tmp9D=t;_LL76: if(_tmp9D <= (void*)3?1:*((int*)_tmp9D)!= 7)goto _LL78;
_LL77: goto _LL79;_LL78: if(_tmp9D <= (void*)3?1:*((int*)_tmp9D)!= 4)goto _LL7A;_LL79:
goto _LL75;_LL7A:;_LL7B:({struct Cyc_Std_String_pa_struct _tmp9F;_tmp9F.tag=0;
_tmp9F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp9E[1]={& _tmp9F};
Cyc_Tcutil_terr(loc,_tag_arr("size requires pointer or array type, not %s",
sizeof(unsigned char),44),_tag_arr(_tmp9E,sizeof(void*),1));}});_LL75:;}return
Cyc_Absyn_uint_t;_LL73:;_LL74:({void*_tmpA0[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Non-unary primop",sizeof(
unsigned char),17),_tag_arr(_tmpA0,sizeof(void*),0));});_LL68:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({struct Cyc_Std_String_pa_struct
_tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{void*_tmpA1[1]={& _tmpA2};Cyc_Tcutil_terr(
e1->loc,_tag_arr("type %s cannot be used here",sizeof(unsigned char),28),
_tag_arr(_tmpA1,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->v=Cyc_Tcenv_lookup_type_vars(te);
_tmpA3;}));}if(!checker(e2)){({struct Cyc_Std_String_pa_struct _tmpA5;_tmpA5.tag=0;
_tmpA5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{void*_tmpA4[1]={& _tmpA5};Cyc_Tcutil_terr(e2->loc,
_tag_arr("type %s cannot be used here",sizeof(unsigned char),28),_tag_arr(_tmpA4,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpA6=
_cycalloc(sizeof(*_tmpA6));_tmpA6->v=Cyc_Tcenv_lookup_type_vars(te);_tmpA6;}));}{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpA7=t1;struct Cyc_Absyn_PtrInfo
_tmpA8;void*_tmpA9;void*_tmpAA;struct Cyc_Absyn_Conref*_tmpAB;struct Cyc_Absyn_Tqual
_tmpAC;struct Cyc_Absyn_Conref*_tmpAD;_LL7D: if(_tmpA7 <= (void*)3?1:*((int*)_tmpA7)
!= 4)goto _LL7F;_tmpA8=((struct Cyc_Absyn_PointerType_struct*)_tmpA7)->f1;_tmpA9=(
void*)_tmpA8.elt_typ;_tmpAA=(void*)_tmpA8.rgn_typ;_tmpAB=_tmpA8.nullable;_tmpAC=
_tmpA8.tq;_tmpAD=_tmpA8.bounds;_LL7E: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
_tmpA9),(void*)1))({void*_tmpAE[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(unsigned char),50),_tag_arr(_tmpAE,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(
te,e2))({struct Cyc_Std_String_pa_struct _tmpB0;_tmpB0.tag=0;_tmpB0.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmpAF[1]={& _tmpB0};Cyc_Tcutil_terr(
e2->loc,_tag_arr("expecting int but found %s",sizeof(unsigned char),27),_tag_arr(
_tmpAF,sizeof(void*),1));}});_tmpAD=Cyc_Absyn_compress_conref(_tmpAD);{void*
_tmpB1=(void*)_tmpAD->v;void*_tmpB2;void*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;_LL82:
if(_tmpB1 <= (void*)1?1:*((int*)_tmpB1)!= 0)goto _LL84;_tmpB2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpB1)->f1;if((int)_tmpB2 != 0)goto _LL84;_LL83: return t1;_LL84: if(_tmpB1 <= (void*)
1?1:*((int*)_tmpB1)!= 0)goto _LL86;_tmpB3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpB1)->f1;if(_tmpB3 <= (void*)1?1:*((int*)_tmpB3)!= 0)goto _LL86;_tmpB4=((struct
Cyc_Absyn_Upper_b_struct*)_tmpB3)->f1;_LL85: {struct Cyc_Absyn_PointerType_struct*
_tmpB5=({struct Cyc_Absyn_PointerType_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));
_tmpB6[0]=({struct Cyc_Absyn_PointerType_struct _tmpB7;_tmpB7.tag=4;_tmpB7.f1=({
struct Cyc_Absyn_PtrInfo _tmpB8;_tmpB8.elt_typ=(void*)_tmpA9;_tmpB8.rgn_typ=(void*)
_tmpAA;_tmpB8.nullable=((struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(
1);_tmpB8.tq=_tmpAC;_tmpB8.bounds=Cyc_Absyn_new_conref((void*)0);_tmpB8;});
_tmpB7;});_tmpB6;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpB5);return(void*)
_tmpB5;}_LL86:;_LL87:(void*)(_tmpAD->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmpBA;_tmpBA.tag=0;_tmpBA.f1=(void*)((void*)0);_tmpBA;});_tmpB9;})));return t1;
_LL81:;}_LL7F:;_LL80: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LL7C:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)
0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_Std_String_pa_struct _tmpBD;_tmpBD.tag=
0;_tmpBD.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{struct Cyc_Std_String_pa_struct _tmpBC;_tmpBC.tag=0;
_tmpBC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);{void*_tmpBB[2]={& _tmpBC,& _tmpBD};Cyc_Tcutil_terr(e1->loc,
_tag_arr("pointer arithmetic on values of different types (%s != %s)",sizeof(
unsigned char),59),_tag_arr(_tmpBB,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_t;}else{if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),(
void*)1))({void*_tmpBE[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(unsigned char),50),_tag_arr(_tmpBE,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_sint_typ(
te,e2)){({struct Cyc_Std_String_pa_struct _tmpC1;_tmpC1.tag=0;_tmpC1.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmpC0;
_tmpC0.tag=0;_tmpC0.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmpBF[2]={& _tmpC0,& _tmpC1};Cyc_Tcutil_terr(e2->loc,_tag_arr("expecting either %s or int but found %s",
sizeof(unsigned char),40),_tag_arr(_tmpBF,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t1;}}else{return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num?e2_is_num: 0)return
Cyc_Absyn_sint_t;else{if(Cyc_Tcutil_unify(t1,t2)?Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({struct Cyc_Core_Opt*_tmpC2=_cycalloc(sizeof(*
_tmpC2));_tmpC2->v=Cyc_Tcenv_lookup_type_vars(te);_tmpC2;}))): 0)return Cyc_Absyn_sint_t;
else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,
e2,t1);return Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2);return Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e1)?1: Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_t;else{{
struct _tuple0 _tmpC4=({struct _tuple0 _tmpC3;_tmpC3.f1=Cyc_Tcutil_compress(t1);
_tmpC3.f2=Cyc_Tcutil_compress(t2);_tmpC3;});void*_tmpC5;struct Cyc_Absyn_PtrInfo
_tmpC6;void*_tmpC7;void*_tmpC8;struct Cyc_Absyn_PtrInfo _tmpC9;void*_tmpCA;_LL89:
_tmpC5=_tmpC4.f1;if(_tmpC5 <= (void*)3?1:*((int*)_tmpC5)!= 4)goto _LL8B;_tmpC6=((
struct Cyc_Absyn_PointerType_struct*)_tmpC5)->f1;_tmpC7=(void*)_tmpC6.elt_typ;
_tmpC8=_tmpC4.f2;if(_tmpC8 <= (void*)3?1:*((int*)_tmpC8)!= 4)goto _LL8B;_tmpC9=((
struct Cyc_Absyn_PointerType_struct*)_tmpC8)->f1;_tmpCA=(void*)_tmpC9.elt_typ;
_LL8A: if(Cyc_Tcutil_unify(_tmpC7,_tmpCA))return Cyc_Absyn_sint_t;goto _LL88;_LL8B:;
_LL8C: goto _LL88;_LL88:;}({struct Cyc_Std_String_pa_struct _tmpCD;_tmpCD.tag=0;
_tmpCD.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct
_tmpCC;_tmpCC.tag=0;_tmpCC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{
void*_tmpCB[2]={& _tmpCC,& _tmpCD};Cyc_Tcutil_terr(loc,_tag_arr("comparison not allowed between %s and %s",
sizeof(unsigned char),41),_tag_arr(_tmpCB,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpCE=_cycalloc(sizeof(*_tmpCE));
_tmpCE->v=Cyc_Tcenv_lookup_type_vars(te);_tmpCE;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpCF=p;_LL8E: if((int)_tmpCF != 0)
goto _LL90;_LL8F: return Cyc_Tcexp_tcPlus(te,e1,e2);_LL90: if((int)_tmpCF != 2)goto
_LL92;_LL91: return Cyc_Tcexp_tcMinus(te,e1,e2);_LL92: if((int)_tmpCF != 1)goto _LL94;
_LL93: goto _LL95;_LL94: if((int)_tmpCF != 3)goto _LL96;_LL95: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL96: if((int)_tmpCF != 4)goto _LL98;_LL97: goto
_LL99;_LL98: if((int)_tmpCF != 13)goto _LL9A;_LL99: goto _LL9B;_LL9A: if((int)_tmpCF != 
14)goto _LL9C;_LL9B: goto _LL9D;_LL9C: if((int)_tmpCF != 15)goto _LL9E;_LL9D: goto _LL9F;
_LL9E: if((int)_tmpCF != 16)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if((int)_tmpCF != 17)
goto _LLA2;_LLA1: goto _LLA3;_LLA2: if((int)_tmpCF != 18)goto _LLA4;_LLA3: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_integral);_LLA4: if((int)_tmpCF != 5)goto _LLA6;_LLA5: goto
_LLA7;_LLA6: if((int)_tmpCF != 6)goto _LLA8;_LLA7: goto _LLA9;_LLA8: if((int)_tmpCF != 
7)goto _LLAA;_LLA9: goto _LLAB;_LLAA: if((int)_tmpCF != 8)goto _LLAC;_LLAB: goto _LLAD;
_LLAC: if((int)_tmpCF != 9)goto _LLAE;_LLAD: goto _LLAF;_LLAE: if((int)_tmpCF != 10)
goto _LLB0;_LLAF: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLB0:;_LLB1:({void*
_tmpD0[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("bad binary primop",sizeof(unsigned char),18),_tag_arr(_tmpD0,sizeof(
void*),0));});_LL8D:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_List_List*es){if(p == (
void*)2?((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1: 0)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLB2: return({void*_tmpD1[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("primitive operator has 0 arguments",
sizeof(unsigned char),35),_tag_arr(_tmpD1,sizeof(void*),0));});case 1: _LLB3: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLB4: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLB5: return({void*_tmpD2[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("primitive operator has > 2 arguments",
sizeof(unsigned char),37),_tag_arr(_tmpD2,sizeof(void*),0));});}return t;}}struct
_tuple8{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmpD3=(void*)e->r;void*
_tmpD4;struct Cyc_Absyn_Vardecl*_tmpD5;void*_tmpD6;struct Cyc_Absyn_Vardecl*_tmpD7;
void*_tmpD8;struct Cyc_Absyn_Vardecl*_tmpD9;void*_tmpDA;struct Cyc_Absyn_Vardecl*
_tmpDB;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*
_tmpDE;struct _tagged_arr*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct _tagged_arr*
_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*
_tmpE4;_LLB8: if(*((int*)_tmpD3)!= 1)goto _LLBA;_tmpD4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpD3)->f2;if(_tmpD4 <= (void*)1?1:*((int*)_tmpD4)!= 2)goto _LLBA;_tmpD5=((struct
Cyc_Absyn_Param_b_struct*)_tmpD4)->f1;_LLB9: _tmpD7=_tmpD5;goto _LLBB;_LLBA: if(*((
int*)_tmpD3)!= 1)goto _LLBC;_tmpD6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpD3)->f2;
if(_tmpD6 <= (void*)1?1:*((int*)_tmpD6)!= 3)goto _LLBC;_tmpD7=((struct Cyc_Absyn_Local_b_struct*)
_tmpD6)->f1;_LLBB: _tmpD9=_tmpD7;goto _LLBD;_LLBC: if(*((int*)_tmpD3)!= 1)goto _LLBE;
_tmpD8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpD3)->f2;if(_tmpD8 <= (void*)1?1:*((
int*)_tmpD8)!= 4)goto _LLBE;_tmpD9=((struct Cyc_Absyn_Pat_b_struct*)_tmpD8)->f1;
_LLBD: _tmpDB=_tmpD9;goto _LLBF;_LLBE: if(*((int*)_tmpD3)!= 1)goto _LLC0;_tmpDA=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmpD3)->f2;if(_tmpDA <= (void*)1?1:*((int*)
_tmpDA)!= 0)goto _LLC0;_tmpDB=((struct Cyc_Absyn_Global_b_struct*)_tmpDA)->f1;
_LLBF: if(!(_tmpDB->tq).q_const)return;goto _LLB7;_LLC0: if(*((int*)_tmpD3)!= 23)
goto _LLC2;_tmpDC=((struct Cyc_Absyn_Subscript_e_struct*)_tmpD3)->f1;_tmpDD=((
struct Cyc_Absyn_Subscript_e_struct*)_tmpD3)->f2;_LLC1:{void*_tmpE5=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpDC->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpE6;struct Cyc_Absyn_Tqual _tmpE7;struct Cyc_Absyn_Tqual _tmpE8;struct Cyc_List_List*
_tmpE9;_LLCF: if(_tmpE5 <= (void*)3?1:*((int*)_tmpE5)!= 4)goto _LLD1;_tmpE6=((
struct Cyc_Absyn_PointerType_struct*)_tmpE5)->f1;_tmpE7=_tmpE6.tq;_LLD0: _tmpE8=
_tmpE7;goto _LLD2;_LLD1: if(_tmpE5 <= (void*)3?1:*((int*)_tmpE5)!= 7)goto _LLD3;
_tmpE8=((struct Cyc_Absyn_ArrayType_struct*)_tmpE5)->f2;_LLD2: if(!_tmpE8.q_const)
return;goto _LLCE;_LLD3: if(_tmpE5 <= (void*)3?1:*((int*)_tmpE5)!= 9)goto _LLD5;
_tmpE9=((struct Cyc_Absyn_TupleType_struct*)_tmpE5)->f1;_LLD4: {unsigned int i=Cyc_Evexp_eval_const_uint_exp(
_tmpDD);{struct _handler_cons _tmpEA;_push_handler(& _tmpEA);{int _tmpEC=0;if(setjmp(
_tmpEA.handler))_tmpEC=1;if(!_tmpEC){{struct _tuple8 _tmpEE;struct Cyc_Absyn_Tqual
_tmpEF;struct _tuple8*_tmpED=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmpE9,(int)i);_tmpEE=*_tmpED;_tmpEF=_tmpEE.f1;if(!_tmpEF.q_const){_npop_handler(
0);return;}};_pop_handler();}else{void*_tmpEB=(void*)_exn_thrown;void*_tmpF1=
_tmpEB;_LLD8: if(_tmpF1 != Cyc_List_Nth)goto _LLDA;_LLD9: return;_LLDA:;_LLDB:(void)
_throw(_tmpF1);_LLD7:;}}}goto _LLCE;}_LLD5:;_LLD6: goto _LLCE;_LLCE:;}goto _LLB7;
_LLC2: if(*((int*)_tmpD3)!= 21)goto _LLC4;_tmpDE=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpD3)->f1;_tmpDF=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpD3)->f2;_LLC3:{
void*_tmpF2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpDE->topt))->v);
struct Cyc_Absyn_AggrInfo _tmpF3;void*_tmpF4;struct Cyc_Absyn_Aggrdecl**_tmpF5;
struct Cyc_List_List*_tmpF6;_LLDD: if(_tmpF2 <= (void*)3?1:*((int*)_tmpF2)!= 10)
goto _LLDF;_tmpF3=((struct Cyc_Absyn_AggrType_struct*)_tmpF2)->f1;_tmpF4=(void*)
_tmpF3.aggr_info;if(*((int*)_tmpF4)!= 1)goto _LLDF;_tmpF5=((struct Cyc_Absyn_KnownAggr_struct*)
_tmpF4)->f1;_LLDE: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmpF5 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmpF5,_tmpDF);if(sf == 0?1: !(sf->tq).q_const)
return;goto _LLDC;}_LLDF: if(_tmpF2 <= (void*)3?1:*((int*)_tmpF2)!= 11)goto _LLE1;
_tmpF6=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpF2)->f2;_LLE0: {struct Cyc_Absyn_Aggrfield*
sf=Cyc_Absyn_lookup_field(_tmpF6,_tmpDF);if(sf == 0?1: !(sf->tq).q_const)return;
goto _LLDC;}_LLE1:;_LLE2: goto _LLDC;_LLDC:;}goto _LLB7;_LLC4: if(*((int*)_tmpD3)!= 
22)goto _LLC6;_tmpE0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpD3)->f1;_tmpE1=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmpD3)->f2;_LLC5:{void*_tmpF7=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE0->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF8;void*_tmpF9;_LLE4: if(_tmpF7 <= (void*)3?1:*((int*)_tmpF7)!= 4)goto _LLE6;
_tmpF8=((struct Cyc_Absyn_PointerType_struct*)_tmpF7)->f1;_tmpF9=(void*)_tmpF8.elt_typ;
_LLE5:{void*_tmpFA=Cyc_Tcutil_compress(_tmpF9);struct Cyc_Absyn_AggrInfo _tmpFB;
void*_tmpFC;struct Cyc_Absyn_Aggrdecl**_tmpFD;struct Cyc_List_List*_tmpFE;_LLE9:
if(_tmpFA <= (void*)3?1:*((int*)_tmpFA)!= 10)goto _LLEB;_tmpFB=((struct Cyc_Absyn_AggrType_struct*)
_tmpFA)->f1;_tmpFC=(void*)_tmpFB.aggr_info;if(*((int*)_tmpFC)!= 1)goto _LLEB;
_tmpFD=((struct Cyc_Absyn_KnownAggr_struct*)_tmpFC)->f1;_LLEA: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmpFD == 0?0: Cyc_Absyn_lookup_decl_field(*_tmpFD,
_tmpE1);if(sf == 0?1: !(sf->tq).q_const)return;goto _LLE8;}_LLEB: if(_tmpFA <= (void*)
3?1:*((int*)_tmpFA)!= 11)goto _LLED;_tmpFE=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpFA)->f2;_LLEC: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmpFE,
_tmpE1);if(sf == 0?1: !(sf->tq).q_const)return;goto _LLE8;}_LLED:;_LLEE: goto _LLE8;
_LLE8:;}goto _LLE3;_LLE6:;_LLE7: goto _LLE3;_LLE3:;}goto _LLB7;_LLC6: if(*((int*)
_tmpD3)!= 20)goto _LLC8;_tmpE2=((struct Cyc_Absyn_Deref_e_struct*)_tmpD3)->f1;
_LLC7:{void*_tmpFF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpE2->topt))->v);struct Cyc_Absyn_PtrInfo _tmp100;struct Cyc_Absyn_Tqual _tmp101;
struct Cyc_Absyn_Tqual _tmp102;_LLF0: if(_tmpFF <= (void*)3?1:*((int*)_tmpFF)!= 4)
goto _LLF2;_tmp100=((struct Cyc_Absyn_PointerType_struct*)_tmpFF)->f1;_tmp101=
_tmp100.tq;_LLF1: _tmp102=_tmp101;goto _LLF3;_LLF2: if(_tmpFF <= (void*)3?1:*((int*)
_tmpFF)!= 7)goto _LLF4;_tmp102=((struct Cyc_Absyn_ArrayType_struct*)_tmpFF)->f2;
_LLF3: if(!_tmp102.q_const)return;goto _LLEF;_LLF4:;_LLF5: goto _LLEF;_LLEF:;}goto
_LLB7;_LLC8: if(*((int*)_tmpD3)!= 11)goto _LLCA;_tmpE3=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpD3)->f1;_LLC9: _tmpE4=_tmpE3;goto _LLCB;_LLCA: if(*((int*)_tmpD3)!= 12)goto
_LLCC;_tmpE4=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpD3)->f1;_LLCB: Cyc_Tcexp_check_writable(
te,_tmpE4);return;_LLCC:;_LLCD: goto _LLB7;_LLB7:;}({struct Cyc_Std_String_pa_struct
_tmp104;_tmp104.tag=0;_tmp104.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e);{
void*_tmp103[1]={& _tmp104};Cyc_Tcutil_terr(e->loc,_tag_arr("attempt to write a const location: %s",
sizeof(unsigned char),38),_tag_arr(_tmp103,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({
void*_tmp105[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("increment/decrement of non-lvalue",
sizeof(unsigned char),34),_tag_arr(_tmp105,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt)){if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(telt),(void*)1))({void*_tmp106[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("can't perform arithmetic on abstract pointer type",sizeof(
unsigned char),50),_tag_arr(_tmp106,sizeof(void*),0));});}else{({struct Cyc_Std_String_pa_struct
_tmp108;_tmp108.tag=0;_tmp108.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp107[1]={& _tmp108};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting arithmetic or ? type but found %s",
sizeof(unsigned char),44),_tag_arr(_tmp107,sizeof(void*),1));}});}}return t;}}
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){Cyc_Tcexp_tcTest(te,e1,_tag_arr("conditional expression",sizeof(
unsigned char),23));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{
void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*
_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp110;}));struct Cyc_List_List _tmp109=({struct Cyc_List_List _tmp10F;_tmp10F.hd=
e3;_tmp10F.tl=0;_tmp10F;});struct Cyc_List_List _tmp10A=({struct Cyc_List_List
_tmp10E;_tmp10E.hd=e2;_tmp10E.tl=(struct Cyc_List_List*)& _tmp109;_tmp10E;});if(!
Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp10A)){({struct Cyc_Std_String_pa_struct
_tmp10D;_tmp10D.tag=0;_tmp10D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e3->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp10C;_tmp10C.tag=0;_tmp10C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{void*_tmp10B[2]={& _tmp10C,&
_tmp10D};Cyc_Tcutil_terr(loc,_tag_arr("conditional clause types do not match: %s != %s",
sizeof(unsigned char),48),_tag_arr(_tmp10B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t;}}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExpNoPromote(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp111=Cyc_Tcutil_compress(t1);_LLF7: if(_tmp111 <= (void*)3?1:*((int*)
_tmp111)!= 7)goto _LLF9;_LLF8:({void*_tmp112[0]={};Cyc_Tcutil_terr(loc,_tag_arr("cannot assign to an array",
sizeof(unsigned char),26),_tag_arr(_tmp112,sizeof(void*),0));});goto _LLF6;_LLF9:;
_LLFA: goto _LLF6;_LLF6:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),(void*)1))({
void*_tmp113[0]={};Cyc_Tcutil_terr(loc,_tag_arr("type is abstract (can't determine size).",
sizeof(unsigned char),41),_tag_arr(_tmp113,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(
e1))return({void*_tmp114[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("assignment to non-lvalue",
sizeof(unsigned char),25),_tag_arr(_tmp114,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e1);if(po == 0){if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp115=({struct
Cyc_Std_String_pa_struct _tmp118;_tmp118.tag=0;_tmp118.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t2);{struct Cyc_Std_String_pa_struct _tmp117;_tmp117.tag=0;_tmp117.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp116[2]={& _tmp117,& _tmp118};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type mismatch: %s != %s",sizeof(unsigned char),24),_tag_arr(
_tmp116,sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp115;}}else{void*_tmp119=(void*)po->v;void*_tmp11A=Cyc_Tcexp_tcBinPrimop(
te,loc,0,_tmp119,e1,e2);if(!(Cyc_Tcutil_unify(_tmp11A,t1)?1:(Cyc_Tcutil_coerceable(
_tmp11A)?Cyc_Tcutil_coerceable(t1): 0))){void*_tmp11B=({struct Cyc_Std_String_pa_struct
_tmp11E;_tmp11E.tag=0;_tmp11E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp11D;_tmp11D.tag=0;_tmp11D.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp11C[2]={& _tmp11D,& _tmp11E};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof(unsigned char),82),_tag_arr(_tmp11C,sizeof(void*),2));}}});Cyc_Tcutil_unify(
_tmp11A,t1);Cyc_Tcutil_explain_failure();return _tmp11B;}return _tmp11A;}return t1;}}
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(
e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,
struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct dbl={6,0};static
void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;{void*_tmp11F=Cyc_Tcutil_compress(t1);void*_tmp120;void*
_tmp121;_LLFC: if((int)_tmp11F != 1)goto _LLFE;_LLFD: Cyc_Tcutil_unchecked_cast(te,e,
dbl_typ);t1=dbl_typ;goto _LLFB;_LLFE: if(_tmp11F <= (void*)3?1:*((int*)_tmp11F)!= 5)
goto _LL100;_tmp120=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp11F)->f2;if((int)
_tmp120 != 0)goto _LL100;_LLFF: goto _LL101;_LL100: if(_tmp11F <= (void*)3?1:*((int*)
_tmp11F)!= 5)goto _LL102;_tmp121=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp11F)->f2;
if((int)_tmp121 != 1)goto _LL102;_LL101: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_t);
t1=Cyc_Absyn_sint_t;goto _LLFB;_LL102:;_LL103: goto _LLFB;_LLFB:;}for(fields=fs;
fields != 0;fields=fields->tl){struct _tuple1*_tmp123;struct Cyc_List_List*_tmp124;
struct Cyc_Position_Segment*_tmp125;void*_tmp126;struct Cyc_Absyn_Tunionfield
_tmp122=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp123=_tmp122.name;_tmp124=
_tmp122.typs;_tmp125=_tmp122.loc;_tmp126=(void*)_tmp122.sc;if(_tmp124 == 0?1:
_tmp124->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple8*)_tmp124->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp128;struct Cyc_List_List*_tmp129;struct Cyc_Position_Segment*
_tmp12A;void*_tmp12B;struct Cyc_Absyn_Tunionfield _tmp127=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp128=_tmp127.name;_tmp129=_tmp127.typs;_tmp12A=_tmp127.loc;
_tmp12B=(void*)_tmp127.sc;if(_tmp129 == 0?1: _tmp129->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple8*)_tmp129->hd)).f2);if(Cyc_Tcutil_coerce_arg(te,e,t2))
return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}({
struct Cyc_Std_String_pa_struct _tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{struct Cyc_Std_String_pa_struct _tmp12D;
_tmp12D.tag=0;_tmp12D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(tu);{void*
_tmp12C[2]={& _tmp12D,& _tmp12E};Cyc_Tcutil_terr(e->loc,_tag_arr("can't find a field in %s to inject a value of type %s",
sizeof(unsigned char),54),_tag_arr(_tmp12C,sizeof(void*),2));}}});return 0;}
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info){struct Cyc_List_List*_tmp130=args;te=Cyc_Tcenv_new_block(loc,te);
Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp131=t;struct Cyc_Absyn_PtrInfo _tmp132;void*
_tmp133;void*_tmp134;struct Cyc_Absyn_Conref*_tmp135;struct Cyc_Absyn_Tqual _tmp136;
struct Cyc_Absyn_Conref*_tmp137;_LL105: if(_tmp131 <= (void*)3?1:*((int*)_tmp131)!= 
4)goto _LL107;_tmp132=((struct Cyc_Absyn_PointerType_struct*)_tmp131)->f1;_tmp133=(
void*)_tmp132.elt_typ;_tmp134=(void*)_tmp132.rgn_typ;_tmp135=_tmp132.nullable;
_tmp136=_tmp132.tq;_tmp137=_tmp132.bounds;_LL106: Cyc_Tcenv_check_rgn_accessible(
te,loc,_tmp134);Cyc_Tcutil_check_nonzero_bound(loc,_tmp137);{void*_tmp138=Cyc_Tcutil_compress(
_tmp133);struct Cyc_Absyn_FnInfo _tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_Core_Opt*
_tmp13B;void*_tmp13C;struct Cyc_List_List*_tmp13D;int _tmp13E;struct Cyc_Absyn_VarargInfo*
_tmp13F;struct Cyc_List_List*_tmp140;struct Cyc_List_List*_tmp141;_LL10A: if(
_tmp138 <= (void*)3?1:*((int*)_tmp138)!= 8)goto _LL10C;_tmp139=((struct Cyc_Absyn_FnType_struct*)
_tmp138)->f1;_tmp13A=_tmp139.tvars;_tmp13B=_tmp139.effect;_tmp13C=(void*)_tmp139.ret_typ;
_tmp13D=_tmp139.args;_tmp13E=_tmp139.c_varargs;_tmp13F=_tmp139.cyc_varargs;
_tmp140=_tmp139.rgn_po;_tmp141=_tmp139.attributes;_LL10B: if(topt != 0)Cyc_Tcutil_unify(
_tmp13C,*topt);while(_tmp130 != 0?_tmp13D != 0: 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp130->hd;void*t2=(*((struct _tuple2*)_tmp13D->hd)).f3;Cyc_Tcexp_tcExp(
te,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,t2)){({struct Cyc_Std_String_pa_struct
_tmp144;_tmp144.tag=0;_tmp144.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp143;_tmp143.tag=0;_tmp143.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{
void*_tmp142[2]={& _tmp143,& _tmp144};Cyc_Tcutil_terr(e1->loc,_tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof(unsigned char),54),_tag_arr(_tmp142,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
_tmp130=_tmp130->tl;_tmp13D=_tmp13D->tl;}if(_tmp13D != 0)({void*_tmp145[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("too few arguments for function",sizeof(unsigned char),31),_tag_arr(
_tmp145,sizeof(void*),0));});else{if((_tmp130 != 0?1: _tmp13E)?1: _tmp13F != 0){if(
_tmp13E)for(0;_tmp130 != 0;_tmp130=_tmp130->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp130->hd);}else{if(_tmp13F == 0)({void*_tmp146[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("too many arguments for function",sizeof(unsigned char),32),_tag_arr(
_tmp146,sizeof(void*),0));});else{void*_tmp148;int _tmp149;struct Cyc_Absyn_VarargInfo
_tmp147=*_tmp13F;_tmp148=(void*)_tmp147.type;_tmp149=_tmp147.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp14A=({struct Cyc_Absyn_VarargCallInfo*_tmp15E=_cycalloc(sizeof(*_tmp15E));
_tmp15E->num_varargs=0;_tmp15E->injectors=0;_tmp15E->vai=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp13F);_tmp15E;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp14A;if(!_tmp149)for(0;_tmp130 != 0;_tmp130=_tmp130->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp130->hd;_tmp14A->num_varargs ++;Cyc_Tcexp_tcExp(te,(
void**)& _tmp148,e1);if(!Cyc_Tcutil_coerce_arg(te,e1,_tmp148)){({struct Cyc_Std_String_pa_struct
_tmp14D;_tmp14D.tag=0;_tmp14D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp14C;_tmp14C.tag=0;_tmp14C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp148);{void*_tmp14B[2]={& _tmp14C,& _tmp14D};Cyc_Tcutil_terr(loc,_tag_arr("vararg requires type %s but argument has type %s",
sizeof(unsigned char),49),_tag_arr(_tmp14B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
else{void*_tmp14E=Cyc_Tcutil_compress(_tmp148);struct Cyc_Absyn_TunionInfo _tmp14F;
void*_tmp150;struct Cyc_Absyn_Tuniondecl**_tmp151;struct Cyc_Absyn_Tuniondecl*
_tmp152;struct Cyc_List_List*_tmp153;void*_tmp154;_LL10F: if(_tmp14E <= (void*)3?1:*((
int*)_tmp14E)!= 2)goto _LL111;_tmp14F=((struct Cyc_Absyn_TunionType_struct*)
_tmp14E)->f1;_tmp150=(void*)_tmp14F.tunion_info;if(*((int*)_tmp150)!= 1)goto
_LL111;_tmp151=((struct Cyc_Absyn_KnownTunion_struct*)_tmp150)->f1;_tmp152=*
_tmp151;_tmp153=_tmp14F.targs;_tmp154=(void*)_tmp14F.rgn;_LL110: {struct Cyc_Absyn_Tuniondecl*
_tmp155=*Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp152->name);struct Cyc_List_List*
fields=0;if(_tmp155->fields == 0)({struct Cyc_Std_String_pa_struct _tmp157;_tmp157.tag=
0;_tmp157.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp148);{void*_tmp156[1]={&
_tmp157};Cyc_Tcutil_terr(loc,_tag_arr("can't inject into %s",sizeof(
unsigned char),21),_tag_arr(_tmp156,sizeof(void*),1));}});else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp155->fields))->v;}if(!Cyc_Tcutil_unify(
_tmp154,Cyc_Tcenv_curr_rgn(te)))({void*_tmp158[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bad region for injected varargs",sizeof(unsigned char),32),_tag_arr(
_tmp158,sizeof(void*),0));});{struct _RegionHandle _tmp159=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp159;_push_region(rgn);{struct Cyc_List_List*_tmp15A=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp155->tvs,_tmp153);for(0;
_tmp130 != 0;_tmp130=_tmp130->tl){_tmp14A->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp130->hd;Cyc_Tcexp_tcExp(te,0,e1);{struct Cyc_Absyn_Tunionfield*
_tmp15B=Cyc_Tcexp_tcInjection(te,e1,_tmp148,rgn,_tmp15A,fields);if(_tmp15B != 0)
_tmp14A->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp14A->injectors,({struct Cyc_List_List*_tmp15C=_cycalloc(
sizeof(*_tmp15C));_tmp15C->hd=(struct Cyc_Absyn_Tunionfield*)_check_null(_tmp15B);
_tmp15C->tl=0;_tmp15C;}));}}}};_pop_region(rgn);}goto _LL10E;}_LL111:;_LL112:({
void*_tmp15D[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad inject vararg type",sizeof(
unsigned char),23),_tag_arr(_tmp15D,sizeof(void*),0));});goto _LL10E;_LL10E:;}}}}}}{
struct Cyc_List_List*a=_tmp141;for(0;a != 0;a=a->tl){void*_tmp15F=(void*)a->hd;
void*_tmp160;int _tmp161;int _tmp162;_LL114: if(_tmp15F <= (void*)16?1:*((int*)
_tmp15F)!= 3)goto _LL116;_tmp160=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp15F)->f1;_tmp161=((struct Cyc_Absyn_Format_att_struct*)_tmp15F)->f2;_tmp162=((
struct Cyc_Absyn_Format_att_struct*)_tmp15F)->f3;_LL115:{struct _handler_cons
_tmp163;_push_handler(& _tmp163);{int _tmp165=0;if(setjmp(_tmp163.handler))_tmp165=
1;if(!_tmp165){{struct Cyc_Absyn_Exp*_tmp166=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(args,_tmp161 - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp162 == 
0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp167=_cycalloc(sizeof(*
_tmp167));_tmp167->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(
args,_tmp162 - 1);_tmp167;});}{void*_tmp168=_tmp160;_LL119: if((int)_tmp168 != 0)
goto _LL11B;_LL11A: Cyc_Tcexp_check_format_args(te,_tmp166,fmt_args,Cyc_Formatstr_get_format_typs);
goto _LL118;_LL11B: if((int)_tmp168 != 1)goto _LL118;_LL11C: Cyc_Tcexp_check_format_args(
te,_tmp166,fmt_args,Cyc_Formatstr_get_scanf_typs);goto _LL118;_LL118:;}};
_pop_handler();}else{void*_tmp164=(void*)_exn_thrown;void*_tmp16A=_tmp164;_LL11E:
if(_tmp16A != Cyc_List_Nth)goto _LL120;_LL11F:({void*_tmp16B[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bad format arguments",sizeof(unsigned char),21),_tag_arr(_tmp16B,
sizeof(void*),0));});goto _LL11D;_LL120:;_LL121:(void)_throw(_tmp16A);_LL11D:;}}}
goto _LL113;_LL116:;_LL117: goto _LL113;_LL113:;}}Cyc_Tcenv_check_effect_accessible(
te,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp13B))->v);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp140);return _tmp13C;_LL10C:;_LL10D: return({void*_tmp16C[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expected pointer to function",sizeof(unsigned char),29),
_tag_arr(_tmp16C,sizeof(void*),0));});_LL109:;}_LL107:;_LL108: return({void*
_tmp16D[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expected pointer to function",
sizeof(unsigned char),29),_tag_arr(_tmp16D,sizeof(void*),0));});_LL104:;}}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,
e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({struct Cyc_Std_String_pa_struct
_tmp16F;_tmp16F.tag=0;_tmp16F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp16E[1]={& _tmp16F};Cyc_Tcutil_terr(
loc,_tag_arr("expected xtunion exn but found %s",sizeof(unsigned char),34),
_tag_arr(_tmp16E,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp170;}));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp171=t1;struct Cyc_Absyn_PtrInfo _tmp172;void*_tmp173;void*_tmp174;struct
Cyc_Absyn_Conref*_tmp175;struct Cyc_Absyn_Tqual _tmp176;struct Cyc_Absyn_Conref*
_tmp177;_LL123: if(_tmp171 <= (void*)3?1:*((int*)_tmp171)!= 4)goto _LL125;_tmp172=((
struct Cyc_Absyn_PointerType_struct*)_tmp171)->f1;_tmp173=(void*)_tmp172.elt_typ;
_tmp174=(void*)_tmp172.rgn_typ;_tmp175=_tmp172.nullable;_tmp176=_tmp172.tq;
_tmp177=_tmp172.bounds;_LL124:{void*_tmp178=Cyc_Tcutil_compress(_tmp173);struct
Cyc_Absyn_FnInfo _tmp179;struct Cyc_List_List*_tmp17A;struct Cyc_Core_Opt*_tmp17B;
void*_tmp17C;struct Cyc_List_List*_tmp17D;int _tmp17E;struct Cyc_Absyn_VarargInfo*
_tmp17F;struct Cyc_List_List*_tmp180;struct Cyc_List_List*_tmp181;_LL128: if(
_tmp178 <= (void*)3?1:*((int*)_tmp178)!= 8)goto _LL12A;_tmp179=((struct Cyc_Absyn_FnType_struct*)
_tmp178)->f1;_tmp17A=_tmp179.tvars;_tmp17B=_tmp179.effect;_tmp17C=(void*)_tmp179.ret_typ;
_tmp17D=_tmp179.args;_tmp17E=_tmp179.c_varargs;_tmp17F=_tmp179.cyc_varargs;
_tmp180=_tmp179.rgn_po;_tmp181=_tmp179.attributes;_LL129: {struct Cyc_List_List*
instantiation=0;if(Cyc_List_length(ts)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp17A))return({void*_tmp182[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type instantiation mismatch",
sizeof(unsigned char),28),_tag_arr(_tmp182,sizeof(void*),0));});for(0;ts != 0;(ts=
ts->tl,_tmp17A=_tmp17A->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp17A))->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp183=_cycalloc(
sizeof(*_tmp183));_tmp183->hd=({struct _tuple5*_tmp184=_cycalloc(sizeof(*_tmp184));
_tmp184->f1=(struct Cyc_Absyn_Tvar*)_tmp17A->hd;_tmp184->f2=(void*)ts->hd;_tmp184;});
_tmp183->tl=instantiation;_tmp183;});}{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp188=_cycalloc(sizeof(*
_tmp188));_tmp188[0]=({struct Cyc_Absyn_FnType_struct _tmp189;_tmp189.tag=8;
_tmp189.f1=({struct Cyc_Absyn_FnInfo _tmp18A;_tmp18A.tvars=0;_tmp18A.effect=
_tmp17B;_tmp18A.ret_typ=(void*)_tmp17C;_tmp18A.args=_tmp17D;_tmp18A.c_varargs=
_tmp17E;_tmp18A.cyc_varargs=_tmp17F;_tmp18A.rgn_po=_tmp180;_tmp18A.attributes=
_tmp181;_tmp18A;});_tmp189;});_tmp188;}));void*new_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Absyn_PointerType_struct
_tmp186;_tmp186.tag=4;_tmp186.f1=({struct Cyc_Absyn_PtrInfo _tmp187;_tmp187.elt_typ=(
void*)new_fn_typ;_tmp187.rgn_typ=(void*)_tmp174;_tmp187.nullable=_tmp175;_tmp187.tq=
_tmp176;_tmp187.bounds=_tmp177;_tmp187;});_tmp186;});_tmp185;});return new_typ;}}
_LL12A:;_LL12B: goto _LL127;_LL127:;}goto _LL122;_LL125:;_LL126: goto _LL122;_LL122:;}
return({struct Cyc_Std_String_pa_struct _tmp18C;_tmp18C.tag=0;_tmp18C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp18B[1]={& _tmp18C};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting polymorphic type but found %s",sizeof(unsigned char),
40),_tag_arr(_tmp18B,sizeof(void*),1));}});}}static void*Cyc_Tcexp_tcCast(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct Cyc_Absyn_Exp*
e){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);Cyc_Tcexp_tcExp(
te,0,e);{void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if((!Cyc_Tcutil_silent_castable(
te,loc,t2,t)?!Cyc_Tcutil_castable(te,loc,t2,t): 0)?!Cyc_Tcutil_zero_to_null(te,t,
e): 0){Cyc_Tcutil_unify(t2,t);{void*_tmp18D=({struct Cyc_Std_String_pa_struct
_tmp190;_tmp190.tag=0;_tmp190.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp18F;_tmp18F.tag=0;_tmp18F.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp18E[2]={& _tmp18F,& _tmp190};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("cannot cast %s to %s",sizeof(unsigned char),21),_tag_arr(
_tmp18E,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return _tmp18D;}}
return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp191=0;
struct Cyc_Absyn_Tqual _tmp192=Cyc_Absyn_empty_tqual();if(topt != 0){void*_tmp193=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp194;void*_tmp195;struct Cyc_Absyn_Tqual
_tmp196;_LL12D: if(_tmp193 <= (void*)3?1:*((int*)_tmp193)!= 4)goto _LL12F;_tmp194=((
struct Cyc_Absyn_PointerType_struct*)_tmp193)->f1;_tmp195=(void*)_tmp194.elt_typ;
_tmp196=_tmp194.tq;_LL12E: _tmp191=({void**_tmp197=_cycalloc(sizeof(*_tmp197));
_tmp197[0]=_tmp195;_tmp197;});_tmp192=_tmp196;goto _LL12C;_LL12F:;_LL130: goto
_LL12C;_LL12C:;}Cyc_Tcexp_tcExpNoInst(te,_tmp191,e);{void*_tmp198=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19A;_LL132: if(*((int*)
_tmp198)!= 28)goto _LL134;_LL133: goto _LL135;_LL134: if(*((int*)_tmp198)!= 24)goto
_LL136;_LL135:({void*_tmp19B[0]={};Cyc_Tcutil_warn(loc,_tag_arr("& used to allocate",
sizeof(unsigned char),19),_tag_arr(_tmp19B,sizeof(void*),0));});{void*(*_tmp19C)(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq)=Cyc_Absyn_at_typ;if(topt != 0){void*
_tmp19D=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp19E;struct Cyc_Absyn_Conref*
_tmp19F;_LL13B: if(_tmp19D <= (void*)3?1:*((int*)_tmp19D)!= 4)goto _LL13D;_tmp19E=((
struct Cyc_Absyn_PointerType_struct*)_tmp19D)->f1;_tmp19F=_tmp19E.nullable;_LL13C:
if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp19F))_tmp19C=Cyc_Absyn_star_typ;
goto _LL13A;_LL13D:;_LL13E: goto _LL13A;_LL13A:;}return _tmp19C((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(void*)2,_tmp192);}_LL136: if(*((int*)_tmp198)!= 23)goto
_LL138;_tmp199=((struct Cyc_Absyn_Subscript_e_struct*)_tmp198)->f1;_tmp19A=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp198)->f2;_LL137:{void*_tmp1A0=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp199->topt))->v);_LL140: if(_tmp1A0 <= (
void*)3?1:*((int*)_tmp1A0)!= 9)goto _LL142;_LL141: goto _LL13F;_LL142:;_LL143:(void*)(
e0->r=(void*)((void*)(Cyc_Absyn_add_exp(_tmp199,_tmp19A,0))->r));return Cyc_Tcexp_tcPlus(
te,_tmp199,_tmp19A);_LL13F:;}goto _LL131;_LL138:;_LL139: goto _LL131;_LL131:;}{int
_tmp1A2;void*_tmp1A3;struct _tuple6 _tmp1A1=Cyc_Tcutil_addressof_props(te,e);
_tmp1A2=_tmp1A1.f1;_tmp1A3=_tmp1A1.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();
if(_tmp1A2)tq.q_const=1;{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp1A3,tq);return t;}}}}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({
struct Cyc_Std_String_pa_struct _tmp1A5;_tmp1A5.tag=0;_tmp1A5.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1A4[1]={& _tmp1A5};Cyc_Tcutil_terr(
loc,_tag_arr("sizeof applied to type %s, which has unknown size here",sizeof(
unsigned char),55),_tag_arr(_tmp1A4,sizeof(void*),1));}});if(topt != 0){void*
_tmp1A6=Cyc_Tcutil_compress(*topt);_LL145: if(_tmp1A6 <= (void*)3?1:*((int*)
_tmp1A6)!= 14)goto _LL147;_LL146: return(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp1A8;_tmp1A8.tag=14;_tmp1A8.f1=(void*)t;_tmp1A8;});_tmp1A7;});_LL147:;_LL148:
goto _LL144;_LL144:;}return Cyc_Absyn_uint_t;}int Cyc_Tcexp_structfield_has_name(
struct _tagged_arr*n,struct Cyc_Absyn_Aggrfield*sf){return Cyc_Std_strcmp(*n,*sf->name)
== 0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);{void*_tmp1A9=n;struct _tagged_arr*_tmp1AA;unsigned int _tmp1AB;
_LL14A: if(*((int*)_tmp1A9)!= 0)goto _LL14C;_tmp1AA=((struct Cyc_Absyn_StructField_struct*)
_tmp1A9)->f1;_LL14B: {int bad_type=1;{void*_tmp1AC=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp1AD;void*_tmp1AE;struct Cyc_Absyn_Aggrdecl**_tmp1AF;struct
Cyc_List_List*_tmp1B0;_LL14F: if(_tmp1AC <= (void*)3?1:*((int*)_tmp1AC)!= 10)goto
_LL151;_tmp1AD=((struct Cyc_Absyn_AggrType_struct*)_tmp1AC)->f1;_tmp1AE=(void*)
_tmp1AD.aggr_info;if(*((int*)_tmp1AE)!= 1)goto _LL151;_tmp1AF=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp1AE)->f1;_LL150: if((*_tmp1AF)->fields == 0)goto _LL14E;if(!((int(*)(int(*pred)(
struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),struct _tagged_arr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp1AA,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null((*_tmp1AF)->fields))->v))({struct Cyc_Std_String_pa_struct
_tmp1B2;_tmp1B2.tag=0;_tmp1B2.f1=(struct _tagged_arr)*_tmp1AA;{void*_tmp1B1[1]={&
_tmp1B2};Cyc_Tcutil_terr(loc,_tag_arr("no field of struct/union has name %s",
sizeof(unsigned char),37),_tag_arr(_tmp1B1,sizeof(void*),1));}});bad_type=0;goto
_LL14E;_LL151: if(_tmp1AC <= (void*)3?1:*((int*)_tmp1AC)!= 11)goto _LL153;_tmp1B0=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1AC)->f2;_LL152: if(!((int(*)(int(*pred)(
struct _tagged_arr*,struct Cyc_Absyn_Aggrfield*),struct _tagged_arr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp1AA,_tmp1B0))({struct
Cyc_Std_String_pa_struct _tmp1B4;_tmp1B4.tag=0;_tmp1B4.f1=(struct _tagged_arr)*
_tmp1AA;{void*_tmp1B3[1]={& _tmp1B4};Cyc_Tcutil_terr(loc,_tag_arr("no field of struct/union has name %s",
sizeof(unsigned char),37),_tag_arr(_tmp1B3,sizeof(void*),1));}});bad_type=0;goto
_LL14E;_LL153:;_LL154: goto _LL14E;_LL14E:;}if(bad_type)({struct Cyc_Std_String_pa_struct
_tmp1B6;_tmp1B6.tag=0;_tmp1B6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp1B5[1]={& _tmp1B6};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known struct/union type",
sizeof(unsigned char),36),_tag_arr(_tmp1B5,sizeof(void*),1));}});goto _LL149;}
_LL14C: if(*((int*)_tmp1A9)!= 1)goto _LL149;_tmp1AB=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp1A9)->f1;_LL14D: {int bad_type=1;{void*_tmp1B7=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp1B8;void*_tmp1B9;struct Cyc_Absyn_Aggrdecl**_tmp1BA;struct
Cyc_List_List*_tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_TunionFieldInfo
_tmp1BD;void*_tmp1BE;struct Cyc_Absyn_Tunionfield*_tmp1BF;_LL156: if(_tmp1B7 <= (
void*)3?1:*((int*)_tmp1B7)!= 10)goto _LL158;_tmp1B8=((struct Cyc_Absyn_AggrType_struct*)
_tmp1B7)->f1;_tmp1B9=(void*)_tmp1B8.aggr_info;if(*((int*)_tmp1B9)!= 1)goto _LL158;
_tmp1BA=((struct Cyc_Absyn_KnownAggr_struct*)_tmp1B9)->f1;_LL157: if((*_tmp1BA)->fields
== 0)goto _LL155;_tmp1BB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*
_tmp1BA)->fields))->v;goto _LL159;_LL158: if(_tmp1B7 <= (void*)3?1:*((int*)_tmp1B7)
!= 11)goto _LL15A;_tmp1BB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1B7)->f2;
_LL159: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1BB)<= _tmp1AB)({
struct Cyc_Std_Int_pa_struct _tmp1C2;_tmp1C2.tag=1;_tmp1C2.f1=(int)_tmp1AB;{struct
Cyc_Std_Int_pa_struct _tmp1C1;_tmp1C1.tag=1;_tmp1C1.f1=(int)((unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1BB));{void*_tmp1C0[2]={& _tmp1C1,&
_tmp1C2};Cyc_Tcutil_terr(loc,_tag_arr("struct/union has too few components: %d <= %d",
sizeof(unsigned char),46),_tag_arr(_tmp1C0,sizeof(void*),2));}}});bad_type=0;
goto _LL155;_LL15A: if(_tmp1B7 <= (void*)3?1:*((int*)_tmp1B7)!= 9)goto _LL15C;
_tmp1BC=((struct Cyc_Absyn_TupleType_struct*)_tmp1B7)->f1;_LL15B: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1BC)<= _tmp1AB)({struct Cyc_Std_Int_pa_struct
_tmp1C5;_tmp1C5.tag=1;_tmp1C5.f1=(int)_tmp1AB;{struct Cyc_Std_Int_pa_struct
_tmp1C4;_tmp1C4.tag=1;_tmp1C4.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp1BC));{void*_tmp1C3[2]={& _tmp1C4,& _tmp1C5};Cyc_Tcutil_terr(
loc,_tag_arr("tuple has too few components: %d <= %d",sizeof(unsigned char),39),
_tag_arr(_tmp1C3,sizeof(void*),2));}}});bad_type=0;goto _LL155;_LL15C: if(_tmp1B7
<= (void*)3?1:*((int*)_tmp1B7)!= 3)goto _LL15E;_tmp1BD=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1B7)->f1;_tmp1BE=(void*)_tmp1BD.field_info;if(*((int*)_tmp1BE)!= 1)goto
_LL15E;_tmp1BF=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp1BE)->f2;_LL15D:
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1BF->typs)< _tmp1AB)({
struct Cyc_Std_Int_pa_struct _tmp1C8;_tmp1C8.tag=1;_tmp1C8.f1=(int)_tmp1AB;{struct
Cyc_Std_Int_pa_struct _tmp1C7;_tmp1C7.tag=1;_tmp1C7.f1=(int)((unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1BF->typs));{void*_tmp1C6[2]={&
_tmp1C7,& _tmp1C8};Cyc_Tcutil_terr(loc,_tag_arr("tunionfield has too few components: %d < %d",
sizeof(unsigned char),44),_tag_arr(_tmp1C6,sizeof(void*),2));}}});bad_type=0;
goto _LL155;_LL15E:;_LL15F: goto _LL155;_LL155:;}if(bad_type)({struct Cyc_Std_String_pa_struct
_tmp1CA;_tmp1CA.tag=0;_tmp1CA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp1C9[1]={& _tmp1CA};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known type",
sizeof(unsigned char),23),_tag_arr(_tmp1C9,sizeof(void*),1));}});goto _LL149;}
_LL149:;}return Cyc_Absyn_uint_t;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1CB=t;struct Cyc_Absyn_PtrInfo _tmp1CC;void*_tmp1CD;void*_tmp1CE;struct
Cyc_Absyn_Conref*_tmp1CF;_LL161: if(_tmp1CB <= (void*)3?1:*((int*)_tmp1CB)!= 4)
goto _LL163;_tmp1CC=((struct Cyc_Absyn_PointerType_struct*)_tmp1CB)->f1;_tmp1CD=(
void*)_tmp1CC.elt_typ;_tmp1CE=(void*)_tmp1CC.rgn_typ;_tmp1CF=_tmp1CC.bounds;
_LL162: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp1CE);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp1CF);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp1CD),(void*)1))({
void*_tmp1D0[0]={};Cyc_Tcutil_terr(loc,_tag_arr("can't dereference abstract pointer type",
sizeof(unsigned char),40),_tag_arr(_tmp1D0,sizeof(void*),0));});return _tmp1CD;
_LL163:;_LL164: return({struct Cyc_Std_String_pa_struct _tmp1D2;_tmp1D2.tag=0;
_tmp1D2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1D1[1]={&
_tmp1D2};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting * or @ type but found %s",
sizeof(unsigned char),35),_tag_arr(_tmp1D1,sizeof(void*),1));}});_LL160:;}}
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,e);{void*_tmp1D3=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp1D4;void*_tmp1D5;struct Cyc_Absyn_Aggrdecl**_tmp1D6;struct Cyc_Absyn_Aggrdecl*
_tmp1D7;struct Cyc_List_List*_tmp1D8;struct Cyc_List_List*_tmp1D9;_LL166: if(
_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 10)goto _LL168;_tmp1D4=((struct Cyc_Absyn_AggrType_struct*)
_tmp1D3)->f1;_tmp1D5=(void*)_tmp1D4.aggr_info;if(*((int*)_tmp1D5)!= 1)goto _LL168;
_tmp1D6=((struct Cyc_Absyn_KnownAggr_struct*)_tmp1D5)->f1;_tmp1D7=*_tmp1D6;
_tmp1D8=_tmp1D4.targs;_LL167: {struct Cyc_Absyn_Aggrfield*_tmp1DA=Cyc_Absyn_lookup_decl_field(
_tmp1D7,f);if(_tmp1DA == 0)return({struct Cyc_Std_String_pa_struct _tmp1DD;_tmp1DD.tag=
0;_tmp1DD.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct _tmp1DC;
_tmp1DC.tag=0;_tmp1DC.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1D7->name);{
void*_tmp1DB[2]={& _tmp1DC,& _tmp1DD};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type %s has no %s field",
sizeof(unsigned char),24),_tag_arr(_tmp1DB,sizeof(void*),2));}}});if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D7->exist_vars))->v != 0)return({void*
_tmp1DE[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(unsigned char),64),_tag_arr(_tmp1DE,sizeof(void*),0));});{void*t2;{struct
_RegionHandle _tmp1DF=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1DF;
_push_region(rgn);{struct Cyc_List_List*_tmp1E0=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1D7->tvs,_tmp1D8);t2=Cyc_Tcutil_rsubstitute(rgn,
_tmp1E0,(void*)_tmp1DA->type);};_pop_region(rgn);}return t2;}}_LL168: if(_tmp1D3 <= (
void*)3?1:*((int*)_tmp1D3)!= 11)goto _LL16A;_tmp1D9=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1D3)->f2;_LL169: {struct Cyc_Absyn_Aggrfield*_tmp1E1=Cyc_Absyn_lookup_field(
_tmp1D9,f);if(_tmp1E1 == 0)return({struct Cyc_Std_String_pa_struct _tmp1E3;_tmp1E3.tag=
0;_tmp1E3.f1=(struct _tagged_arr)*f;{void*_tmp1E2[1]={& _tmp1E3};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type has no %s field",sizeof(unsigned char),21),_tag_arr(
_tmp1E2,sizeof(void*),1));}});return(void*)_tmp1E1->type;}_LL16A: if(_tmp1D3 <= (
void*)3?1:*((int*)_tmp1D3)!= 7)goto _LL16C;if(!(Cyc_Std_zstrcmp(*f,_tag_arr("size",
sizeof(unsigned char),5))== 0))goto _LL16C;_LL16B: goto _LL16D;_LL16C: if(_tmp1D3 <= (
void*)3?1:*((int*)_tmp1D3)!= 4)goto _LL16E;if(!(Cyc_Std_zstrcmp(*f,_tag_arr("size",
sizeof(unsigned char),5))== 0))goto _LL16E;_LL16D:(void*)(outer_e->r=(void*)((
void*)({struct Cyc_Absyn_Primop_e_struct*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));
_tmp1E4[0]=({struct Cyc_Absyn_Primop_e_struct _tmp1E5;_tmp1E5.tag=3;_tmp1E5.f1=(
void*)((void*)19);_tmp1E5.f2=({struct Cyc_List_List*_tmp1E6=_cycalloc(sizeof(*
_tmp1E6));_tmp1E6->hd=e;_tmp1E6->tl=0;_tmp1E6;});_tmp1E5;});_tmp1E4;})));return
Cyc_Absyn_uint_t;_LL16E:;_LL16F: if(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(
unsigned char),5))== 0)return({struct Cyc_Std_String_pa_struct _tmp1E8;_tmp1E8.tag=
0;_tmp1E8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmp1E7[1]={& _tmp1E8};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("expecting struct, union, or array, found %s",sizeof(unsigned char),44),
_tag_arr(_tmp1E7,sizeof(void*),1));}});else{return({struct Cyc_Std_String_pa_struct
_tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1E9[1]={& _tmp1EA};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct or union, found %s",sizeof(unsigned char),36),
_tag_arr(_tmp1E9,sizeof(void*),1));}});}_LL165:;}}static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _tagged_arr*f){Cyc_Tcexp_tcExp(te,0,e);{void*_tmp1EB=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1EC;void*_tmp1ED;void*_tmp1EE;struct Cyc_Absyn_Conref*_tmp1EF;_LL171: if(
_tmp1EB <= (void*)3?1:*((int*)_tmp1EB)!= 4)goto _LL173;_tmp1EC=((struct Cyc_Absyn_PointerType_struct*)
_tmp1EB)->f1;_tmp1ED=(void*)_tmp1EC.elt_typ;_tmp1EE=(void*)_tmp1EC.rgn_typ;
_tmp1EF=_tmp1EC.bounds;_LL172: Cyc_Tcutil_check_nonzero_bound(loc,_tmp1EF);{void*
_tmp1F0=Cyc_Tcutil_compress(_tmp1ED);struct Cyc_Absyn_AggrInfo _tmp1F1;void*
_tmp1F2;struct Cyc_Absyn_Aggrdecl**_tmp1F3;struct Cyc_Absyn_Aggrdecl*_tmp1F4;
struct Cyc_List_List*_tmp1F5;struct Cyc_List_List*_tmp1F6;_LL176: if(_tmp1F0 <= (
void*)3?1:*((int*)_tmp1F0)!= 10)goto _LL178;_tmp1F1=((struct Cyc_Absyn_AggrType_struct*)
_tmp1F0)->f1;_tmp1F2=(void*)_tmp1F1.aggr_info;if(*((int*)_tmp1F2)!= 1)goto _LL178;
_tmp1F3=((struct Cyc_Absyn_KnownAggr_struct*)_tmp1F2)->f1;_tmp1F4=*_tmp1F3;
_tmp1F5=_tmp1F1.targs;_LL177: {struct Cyc_Absyn_Aggrfield*_tmp1F7=Cyc_Absyn_lookup_decl_field(
_tmp1F4,f);if(_tmp1F7 == 0)return({struct Cyc_Std_String_pa_struct _tmp1FA;_tmp1FA.tag=
0;_tmp1FA.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct _tmp1F9;
_tmp1F9.tag=0;_tmp1F9.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1F4->name);{
void*_tmp1F8[2]={& _tmp1F9,& _tmp1FA};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type %s has no %s field",
sizeof(unsigned char),24),_tag_arr(_tmp1F8,sizeof(void*),2));}}});if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F4->exist_vars))->v != 0)return({void*
_tmp1FB[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("must use pattern-matching to access fields of existential types",
sizeof(unsigned char),64),_tag_arr(_tmp1FB,sizeof(void*),0));});{void*t3;{struct
_RegionHandle _tmp1FC=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1FC;
_push_region(rgn);{struct Cyc_List_List*_tmp1FD=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1F4->tvs,_tmp1F5);t3=Cyc_Tcutil_rsubstitute(rgn,
_tmp1FD,(void*)_tmp1F7->type);};_pop_region(rgn);}return t3;}}_LL178: if(_tmp1F0 <= (
void*)3?1:*((int*)_tmp1F0)!= 11)goto _LL17A;_tmp1F6=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F0)->f2;_LL179: {struct Cyc_Absyn_Aggrfield*_tmp1FE=Cyc_Absyn_lookup_field(
_tmp1F6,f);if(_tmp1FE == 0)return({struct Cyc_Std_String_pa_struct _tmp200;_tmp200.tag=
0;_tmp200.f1=(struct _tagged_arr)*f;{void*_tmp1FF[1]={& _tmp200};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type has no %s field",sizeof(unsigned char),21),_tag_arr(
_tmp1FF,sizeof(void*),1));}});return(void*)_tmp1FE->type;}_LL17A:;_LL17B: goto
_LL175;_LL175:;}goto _LL170;_LL173:;_LL174: goto _LL170;_LL170:;}return({struct Cyc_Std_String_pa_struct
_tmp202;_tmp202.tag=0;_tmp202.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp201[1]={& _tmp202};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct pointer, found %s",sizeof(unsigned char),35),
_tag_arr(_tmp201,sizeof(void*),1));}});}static void*Cyc_Tcexp_ithTupleType(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,struct
Cyc_Absyn_Exp*index){unsigned int i=Cyc_Evexp_eval_const_uint_exp(index);struct
_handler_cons _tmp203;_push_handler(& _tmp203);{int _tmp205=0;if(setjmp(_tmp203.handler))
_tmp205=1;if(!_tmp205){{void*_tmp206=(*((struct _tuple8*(*)(struct Cyc_List_List*x,
int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0);return _tmp206;};_pop_handler();}
else{void*_tmp204=(void*)_exn_thrown;void*_tmp208=_tmp204;_LL17D: if(_tmp208 != 
Cyc_List_Nth)goto _LL17F;_LL17E: return({struct Cyc_Std_Int_pa_struct _tmp20B;
_tmp20B.tag=1;_tmp20B.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts));{struct Cyc_Std_Int_pa_struct _tmp20A;_tmp20A.tag=1;_tmp20A.f1=(int)i;{void*
_tmp209[2]={& _tmp20A,& _tmp20B};Cyc_Tcexp_expr_err(te,loc,_tag_arr("index is %d but tuple has only %d fields",
sizeof(unsigned char),41),_tag_arr(_tmp209,sizeof(void*),2));}}});_LL17F:;_LL180:(
void)_throw(_tmp208);_LL17C:;}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(
te,e2))return({struct Cyc_Std_String_pa_struct _tmp20D;_tmp20D.tag=0;_tmp20D.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp20C[1]={& _tmp20D};Cyc_Tcexp_expr_err(
te,e2->loc,_tag_arr("expecting int subscript, found %s",sizeof(unsigned char),34),
_tag_arr(_tmp20C,sizeof(void*),1));}});{void*_tmp20E=t1;struct Cyc_Absyn_PtrInfo
_tmp20F;void*_tmp210;void*_tmp211;struct Cyc_Absyn_Tqual _tmp212;struct Cyc_Absyn_Conref*
_tmp213;struct Cyc_List_List*_tmp214;_LL182: if(_tmp20E <= (void*)3?1:*((int*)
_tmp20E)!= 4)goto _LL184;_tmp20F=((struct Cyc_Absyn_PointerType_struct*)_tmp20E)->f1;
_tmp210=(void*)_tmp20F.elt_typ;_tmp211=(void*)_tmp20F.rgn_typ;_tmp212=_tmp20F.tq;
_tmp213=_tmp20F.bounds;_LL183: if(Cyc_Tcutil_is_const_exp(te,e2))Cyc_Tcutil_check_bound(
loc,Cyc_Evexp_eval_const_uint_exp(e2),_tmp213);else{if(Cyc_Tcutil_is_bound_one(
_tmp213))({void*_tmp215[0]={};Cyc_Tcutil_warn(e1->loc,_tag_arr("subscript applied to pointer to one object",
sizeof(unsigned char),43),_tag_arr(_tmp215,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(
loc,_tmp213);}Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp211);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp210),(void*)1))({void*_tmp216[0]={};Cyc_Tcutil_terr(e1->loc,
_tag_arr("can't subscript an abstract pointer",sizeof(unsigned char),36),
_tag_arr(_tmp216,sizeof(void*),0));});return _tmp210;_LL184: if(_tmp20E <= (void*)3?
1:*((int*)_tmp20E)!= 9)goto _LL186;_tmp214=((struct Cyc_Absyn_TupleType_struct*)
_tmp20E)->f1;_LL185: return Cyc_Tcexp_ithTupleType(te,loc,_tmp214,e2);_LL186:;
_LL187: return({struct Cyc_Std_String_pa_struct _tmp218;_tmp218.tag=0;_tmp218.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp217[1]={& _tmp218};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("subscript applied to %s",sizeof(unsigned char),24),_tag_arr(
_tmp217,sizeof(void*),1));}});_LL181:;}}}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*es){int done=0;
struct Cyc_List_List*fields=0;if(topt != 0){void*_tmp219=Cyc_Tcutil_compress(*topt);
struct Cyc_List_List*_tmp21A;_LL189: if(_tmp219 <= (void*)3?1:*((int*)_tmp219)!= 9)
goto _LL18B;_tmp21A=((struct Cyc_Absyn_TupleType_struct*)_tmp219)->f1;_LL18A: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp21A)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL188;for(0;es != 0;(es=es->tl,_tmp21A=_tmp21A->tl)){
void*_tmp21B=(*((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp21A))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp21B,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->hd=({
struct _tuple8*_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D->f1=(*((struct _tuple8*)
_tmp21A->hd)).f1;_tmp21D->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp21D;});_tmp21C->tl=fields;_tmp21C;});}done=
1;goto _LL188;_LL18B:;_LL18C: goto _LL188;_LL188:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp21E=_cycalloc(sizeof(*_tmp21E));_tmp21E->hd=({struct _tuple8*_tmp21F=
_cycalloc(sizeof(*_tmp21F));_tmp21F->f1=Cyc_Absyn_empty_tqual();_tmp21F->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp21F;});_tmp21E->tl=fields;_tmp21E;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Absyn_TupleType_struct
_tmp221;_tmp221.tag=9;_tmp221.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp221;});_tmp220;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp222[0]={};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("tcCompoundLit",sizeof(unsigned char),14),_tag_arr(_tmp222,sizeof(void*),
0));});}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,struct Cyc_List_List*des){struct Cyc_List_List*es=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple7*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp233=_cycalloc(
sizeof(*_tmp233));_tmp233->v=Cyc_Tcenv_lookup_type_vars(te);_tmp233;}));struct
Cyc_Absyn_Const_e_struct*_tmp223=({struct Cyc_Absyn_Const_e_struct*_tmp22F=
_cycalloc(sizeof(*_tmp22F));_tmp22F[0]=({struct Cyc_Absyn_Const_e_struct _tmp230;
_tmp230.tag=0;_tmp230.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp231=
_cycalloc(sizeof(*_tmp231));_tmp231[0]=({struct Cyc_Absyn_Int_c_struct _tmp232;
_tmp232.tag=2;_tmp232.f1=(void*)((void*)1);_tmp232.f2=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es);_tmp232;});_tmp231;}));_tmp230;});_tmp22F;});struct Cyc_Absyn_Exp*
sz_exp=Cyc_Absyn_new_exp((void*)_tmp223,loc);sz_exp->topt=({struct Cyc_Core_Opt*
_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->v=(void*)Cyc_Absyn_uint_t;_tmp224;});{
void*res_t2=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp22D=_cycalloc(sizeof(*
_tmp22D));_tmp22D[0]=({struct Cyc_Absyn_ArrayType_struct _tmp22E;_tmp22E.tag=7;
_tmp22E.f1=(void*)res;_tmp22E.f2=Cyc_Absyn_empty_tqual();_tmp22E.f3=(struct Cyc_Absyn_Exp*)
sz_exp;_tmp22E;});_tmp22D;});{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(
te,res,es))({struct Cyc_Std_String_pa_struct _tmp226;_tmp226.tag=0;_tmp226.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(res);{void*_tmp225[1]={& _tmp226};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,_tag_arr("elements of array do not all have the same type (%s)",
sizeof(unsigned char),53),_tag_arr(_tmp225,sizeof(void*),1));}});{int offset=0;
for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple7*)
des->hd)).f1;if(ds != 0){void*_tmp227=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp228;
_LL18E: if(*((int*)_tmp227)!= 1)goto _LL190;_LL18F:({void*_tmp229[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("only array index designators are supported",sizeof(unsigned char),
43),_tag_arr(_tmp229,sizeof(void*),0));});goto _LL18D;_LL190: if(*((int*)_tmp227)
!= 0)goto _LL18D;_tmp228=((struct Cyc_Absyn_ArrayElement_struct*)_tmp227)->f1;
_LL191: Cyc_Tcexp_tcExpInitializer(te,0,_tmp228);{unsigned int i=Cyc_Evexp_eval_const_uint_exp(
_tmp228);if(i != offset)({struct Cyc_Std_Int_pa_struct _tmp22C;_tmp22C.tag=1;
_tmp22C.f1=(int)i;{struct Cyc_Std_Int_pa_struct _tmp22B;_tmp22B.tag=1;_tmp22B.f1=(
int)((unsigned int)offset);{void*_tmp22A[2]={& _tmp22B,& _tmp22C};Cyc_Tcutil_terr(
_tmp228->loc,_tag_arr("expecting index designator %d but found %d",sizeof(
unsigned char),43),_tag_arr(_tmp22A,sizeof(void*),2));}}});goto _LL18D;}_LL18D:;}}}
return res_t2;}}static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*
bound,struct Cyc_Absyn_Exp*body){Cyc_Tcexp_tcExp(te,0,bound);if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_Std_String_pa_struct _tmp235;_tmp235.tag=0;_tmp235.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);{void*_tmp234[1]={& _tmp235};Cyc_Tcutil_terr(bound->loc,
_tag_arr("expecting unsigned int, found %s",sizeof(unsigned char),33),_tag_arr(
_tmp234,sizeof(void*),1));}});if(!(vd->tq).q_const)({void*_tmp236[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("comprehension index variable is not declared const!",
sizeof(unsigned char),52),_tag_arr(_tmp236,sizeof(void*),0));});if(te->le != 0){
te=Cyc_Tcenv_new_block(loc,te);te=Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp237=0;struct Cyc_Absyn_Tqual*_tmp238=0;if(topt != 0){void*_tmp239=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp23A;void*_tmp23B;void**
_tmp23C;struct Cyc_Absyn_Tqual _tmp23D;struct Cyc_Absyn_Tqual*_tmp23E;struct Cyc_Absyn_Exp*
_tmp23F;_LL193: if(_tmp239 <= (void*)3?1:*((int*)_tmp239)!= 4)goto _LL195;_tmp23A=((
struct Cyc_Absyn_PointerType_struct*)_tmp239)->f1;_LL194: pinfo=_tmp23A;_tmp237=(
void**)((void**)((void*)& pinfo.elt_typ));_tmp238=(struct Cyc_Absyn_Tqual*)& pinfo.tq;
goto _LL192;_LL195: if(_tmp239 <= (void*)3?1:*((int*)_tmp239)!= 7)goto _LL197;
_tmp23B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp239)->f1;_tmp23C=(void**)&((
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp239)->f1);_tmp23D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp239)->f2;_tmp23E=(struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_ArrayType_struct*)
_tmp239)->f2;_tmp23F=((struct Cyc_Absyn_ArrayType_struct*)_tmp239)->f3;_LL196:
_tmp237=(void**)_tmp23C;_tmp238=(struct Cyc_Absyn_Tqual*)_tmp23E;goto _LL192;
_LL197:;_LL198: goto _LL192;_LL192:;}{void*t=Cyc_Tcexp_tcExp(te,_tmp237,body);if(
te->le == 0){if(!Cyc_Tcutil_is_const_exp(te,bound))({void*_tmp240[0]={};Cyc_Tcutil_terr(
bound->loc,_tag_arr("bound is not constant",sizeof(unsigned char),22),_tag_arr(
_tmp240,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(te,body))({void*_tmp241[
0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("body is not constant",sizeof(
unsigned char),21),_tag_arr(_tmp241,sizeof(void*),0));});}return(void*)({struct
Cyc_Absyn_ArrayType_struct*_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp243;_tmp243.tag=7;_tmp243.f1=(void*)t;
_tmp243.f2=_tmp238 == 0?Cyc_Absyn_empty_tqual():*_tmp238;_tmp243.f3=(struct Cyc_Absyn_Exp*)
bound;_tmp243;});_tmp242;});}}}struct _tuple9{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,
struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp244;_push_handler(& _tmp244);{int _tmp246=0;if(setjmp(
_tmp244.handler))_tmp246=1;if(!_tmp246){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp245=(void*)_exn_thrown;void*_tmp248=_tmp245;_LL19A:
if(_tmp248 != Cyc_Dict_Absent)goto _LL19C;_LL19B:({struct Cyc_Std_String_pa_struct
_tmp24A;_tmp24A.tag=0;_tmp24A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{
void*_tmp249[1]={& _tmp24A};Cyc_Tcutil_terr(loc,_tag_arr("unbound struct name %s",
sizeof(unsigned char),23),_tag_arr(_tmp249,sizeof(void*),1));}});return topt != 0?*
topt:(void*)0;_LL19C:;_LL19D:(void)_throw(_tmp248);_LL199:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({void*_tmp24B[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("expecting struct but found union",sizeof(unsigned char),33),
_tag_arr(_tmp24B,sizeof(void*),0));});if(ad->fields == 0){({void*_tmp24C[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("can't build abstract struct",sizeof(unsigned char),
28),_tag_arr(_tmp24C,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp24D;}));}{struct _RegionHandle _tmp24E=_new_region("rgn");struct _RegionHandle*
rgn=& _tmp24E;_push_region(rgn);{struct _tuple4 _tmp24F=({struct _tuple4 _tmp26A;
_tmp26A.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp26A.f2=rgn;_tmp26A;});struct Cyc_List_List*
_tmp250=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp24F,ad->tvs);struct Cyc_List_List*_tmp251=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp24F,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ad->exist_vars))->v);struct Cyc_List_List*_tmp252=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp250);struct Cyc_List_List*_tmp253=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp251);struct Cyc_List_List*_tmp254=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp250,_tmp251);struct Cyc_Absyn_AggrType_struct*_tmp255=({struct Cyc_Absyn_AggrType_struct*
_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264[0]=({struct Cyc_Absyn_AggrType_struct
_tmp265;_tmp265.tag=10;_tmp265.f1=({struct Cyc_Absyn_AggrInfo _tmp266;_tmp266.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp267=_cycalloc(sizeof(*
_tmp267));_tmp267[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp268;_tmp268.tag=1;
_tmp268.f1=({struct Cyc_Absyn_Aggrdecl**_tmp269=_cycalloc(sizeof(*_tmp269));
_tmp269[0]=ad;_tmp269;});_tmp268;});_tmp267;}));_tmp266.targs=_tmp252;_tmp266;});
_tmp265;});_tmp264;});struct Cyc_List_List*_tmp256=*ts;struct Cyc_List_List*
_tmp257=_tmp253;while(_tmp256 != 0?_tmp257 != 0: 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp256->hd);Cyc_Tcutil_unify((
void*)_tmp256->hd,(void*)_tmp257->hd);_tmp256=_tmp256->tl;_tmp257=_tmp257->tl;}
if(_tmp256 != 0)({void*_tmp258[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too many explicit witness types",
sizeof(unsigned char),32),_tag_arr(_tmp258,sizeof(void*),0));});*ts=_tmp253;if(
topt != 0)Cyc_Tcutil_unify((void*)_tmp255,*topt);{struct Cyc_List_List*fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ad->fields))->v);
for(0;fields != 0;fields=fields->tl){struct _tuple9 _tmp25A;struct Cyc_Absyn_Aggrfield*
_tmp25B;struct Cyc_Absyn_Exp*_tmp25C;struct _tuple9*_tmp259=(struct _tuple9*)fields->hd;
_tmp25A=*_tmp259;_tmp25B=_tmp25A.f1;_tmp25C=_tmp25A.f2;{void*_tmp25D=Cyc_Tcutil_rsubstitute(
rgn,_tmp254,(void*)_tmp25B->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp25D,
_tmp25C);if(!Cyc_Tcutil_coerce_arg(te,_tmp25C,_tmp25D)){({struct Cyc_Std_String_pa_struct
_tmp262;_tmp262.tag=0;_tmp262.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp25C->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp261;_tmp261.tag=0;_tmp261.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp25D);{struct Cyc_Std_String_pa_struct _tmp260;_tmp260.tag=0;_tmp260.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(*tn);{struct Cyc_Std_String_pa_struct _tmp25F;
_tmp25F.tag=0;_tmp25F.f1=(struct _tagged_arr)*_tmp25B->name;{void*_tmp25E[4]={&
_tmp25F,& _tmp260,& _tmp261,& _tmp262};Cyc_Tcutil_terr(_tmp25C->loc,_tag_arr("field %s of struct %s expects type %s != %s",
sizeof(unsigned char),44),_tag_arr(_tmp25E,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
void*_tmp263=(void*)_tmp255;_npop_handler(0);return _tmp263;}}};_pop_region(rgn);}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){{struct _RegionHandle _tmp26B=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp26B;_push_region(rgn);{void*_tmp26C=Cyc_Tcutil_compress(
ts);void*_tmp26D;struct Cyc_List_List*_tmp26E;_LL19F: if(_tmp26C <= (void*)3?1:*((
int*)_tmp26C)!= 11)goto _LL1A1;_tmp26D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp26C)->f1;_tmp26E=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp26C)->f2;_LL1A0:
if(_tmp26D == (void*)1)({void*_tmp26F[0]={};Cyc_Tcutil_terr(loc,_tag_arr("expecting struct but found union",
sizeof(unsigned char),33),_tag_arr(_tmp26F,sizeof(void*),0));});{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,_tmp26E);for(0;fields != 0;fields=fields->tl){struct _tuple9 _tmp271;
struct Cyc_Absyn_Aggrfield*_tmp272;struct Cyc_Absyn_Exp*_tmp273;struct _tuple9*
_tmp270=(struct _tuple9*)fields->hd;_tmp271=*_tmp270;_tmp272=_tmp271.f1;_tmp273=
_tmp271.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)((void**)((void*)& _tmp272->type)),
_tmp273);if(!Cyc_Tcutil_coerce_arg(te,_tmp273,(void*)_tmp272->type)){({struct Cyc_Std_String_pa_struct
_tmp277;_tmp277.tag=0;_tmp277.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp273->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp276;_tmp276.tag=0;_tmp276.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
_tmp272->type);{struct Cyc_Std_String_pa_struct _tmp275;_tmp275.tag=0;_tmp275.f1=(
struct _tagged_arr)*_tmp272->name;{void*_tmp274[3]={& _tmp275,& _tmp276,& _tmp277};
Cyc_Tcutil_terr(_tmp273->loc,_tag_arr("field %s of struct expects type %s != %s",
sizeof(unsigned char),41),_tag_arr(_tmp274,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
goto _LL19E;}_LL1A1:;_LL1A2:({void*_tmp278[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcAnonStruct: wrong type",
sizeof(unsigned char),25),_tag_arr(_tmp278,sizeof(void*),0));});_LL19E:;};
_pop_region(rgn);}return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Absyn_Tuniondecl*tud,struct Cyc_Absyn_Tunionfield*tuf){struct
_RegionHandle _tmp279=_new_region("rgn");struct _RegionHandle*rgn=& _tmp279;
_push_region(rgn);{struct _tuple4 _tmp27A=({struct _tuple4 _tmp299;_tmp299.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp299.f2=rgn;_tmp299;});struct Cyc_List_List*_tmp27B=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp27A,tud->tvs);struct Cyc_List_List*_tmp27C=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp27B);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp294=_cycalloc(sizeof(*_tmp294));_tmp294[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp295;_tmp295.tag=3;_tmp295.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp296;
_tmp296.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp298;_tmp298.tag=1;_tmp298.f1=tud;_tmp298.f2=tuf;_tmp298;});_tmp297;}));
_tmp296.targs=_tmp27C;_tmp296;});_tmp295;});_tmp294;});if(topt != 0){void*_tmp27D=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo _tmp27E;struct Cyc_List_List*
_tmp27F;void*_tmp280;_LL1A4: if(_tmp27D <= (void*)3?1:*((int*)_tmp27D)!= 3)goto
_LL1A6;_LL1A5: Cyc_Tcutil_unify(*topt,res);goto _LL1A3;_LL1A6: if(_tmp27D <= (void*)
3?1:*((int*)_tmp27D)!= 2)goto _LL1A8;_tmp27E=((struct Cyc_Absyn_TunionType_struct*)
_tmp27D)->f1;_tmp27F=_tmp27E.targs;_tmp280=(void*)_tmp27E.rgn;_LL1A7:{struct Cyc_List_List*
a=_tmp27C;for(0;a != 0?_tmp27F != 0: 0;(a=a->tl,_tmp27F=_tmp27F->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp27F->hd);}}if(tuf->typs == 0?es == 0: 0){e->topt=({struct Cyc_Core_Opt*
_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281->v=(void*)res;_tmp281;});res=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282[0]=({
struct Cyc_Absyn_TunionType_struct _tmp283;_tmp283.tag=2;_tmp283.f1=({struct Cyc_Absyn_TunionInfo
_tmp284;_tmp284.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp286;_tmp286.tag=1;_tmp286.f1=({struct Cyc_Absyn_Tuniondecl**_tmp287=_cycalloc(
sizeof(*_tmp287));_tmp287[0]=tud;_tmp287;});_tmp286;});_tmp285;}));_tmp284.targs=
_tmp27C;_tmp284.rgn=(void*)_tmp280;_tmp284;});_tmp283;});_tmp282;});Cyc_Tcutil_unchecked_cast(
te,e,res);{void*_tmp288=res;_npop_handler(0);return _tmp288;}}goto _LL1A3;_LL1A8:;
_LL1A9: goto _LL1A3;_LL1A3:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0?ts != 0:
0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*
t=Cyc_Tcutil_rsubstitute(rgn,_tmp27B,(*((struct _tuple8*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct
_tmp28C;_tmp28C.tag=0;_tmp28C.f1=(struct _tagged_arr)(e->topt == 0?_tag_arr("?",
sizeof(unsigned char),2): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{struct Cyc_Std_String_pa_struct _tmp28B;_tmp28B.tag=0;
_tmp28B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp28A;_tmp28A.tag=0;_tmp28A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp289[3]={& _tmp28A,& _tmp28B,& _tmp28C};Cyc_Tcutil_terr(e->loc,_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof(unsigned char),80),_tag_arr(_tmp289,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp28F=({struct Cyc_Std_String_pa_struct _tmp28E;_tmp28E.tag=0;
_tmp28E.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp28D[1]={&
_tmp28E};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(unsigned char),45),_tag_arr(_tmp28D,sizeof(void*),1));}});_npop_handler(0);
return _tmp28F;}if(ts != 0){void*_tmp292=({struct Cyc_Std_String_pa_struct _tmp291;
_tmp291.tag=0;_tmp291.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp290[1]={& _tmp291};Cyc_Tcexp_expr_err(te,loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(unsigned char),44),_tag_arr(_tmp290,sizeof(void*),1));}});_npop_handler(0);
return _tmp292;}{void*_tmp293=res;_npop_handler(0);return _tmp293;}}};_pop_region(
rgn);}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp29A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp29B;struct Cyc_Absyn_Conref*_tmp29C;void*_tmp29D;
struct Cyc_List_List*_tmp29E;struct Cyc_Absyn_AggrInfo _tmp29F;void*_tmp2A0;struct
Cyc_List_List*_tmp2A1;struct Cyc_List_List*_tmp2A2;_LL1AB: if((int)_tmp29A != 0)
goto _LL1AD;_LL1AC: return 1;_LL1AD: if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 0)
goto _LL1AF;_LL1AE: goto _LL1B0;_LL1AF: if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 1)
goto _LL1B1;_LL1B0: goto _LL1B2;_LL1B1: if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 2)
goto _LL1B3;_LL1B2: goto _LL1B4;_LL1B3: if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 3)
goto _LL1B5;_LL1B4: return 0;_LL1B5: if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 4)
goto _LL1B7;_tmp29B=((struct Cyc_Absyn_PointerType_struct*)_tmp29A)->f1;_tmp29C=
_tmp29B.nullable;_LL1B6: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmp29C);_LL1B7: if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 5)goto _LL1B9;_LL1B8:
goto _LL1BA;_LL1B9: if((int)_tmp29A != 1)goto _LL1BB;_LL1BA: goto _LL1BC;_LL1BB: if(
_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 6)goto _LL1BD;_LL1BC: return 1;_LL1BD: if(
_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 7)goto _LL1BF;_tmp29D=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp29A)->f1;_LL1BE: return Cyc_Tcexp_zeroable_type(_tmp29D);_LL1BF: if(_tmp29A <= (
void*)3?1:*((int*)_tmp29A)!= 8)goto _LL1C1;_LL1C0: return 0;_LL1C1: if(_tmp29A <= (
void*)3?1:*((int*)_tmp29A)!= 9)goto _LL1C3;_tmp29E=((struct Cyc_Absyn_TupleType_struct*)
_tmp29A)->f1;_LL1C2: for(0;(unsigned int)_tmp29E;_tmp29E=_tmp29E->tl){if(!Cyc_Tcexp_zeroable_type((*((
struct _tuple8*)_tmp29E->hd)).f2))return 0;}return 1;_LL1C3: if(_tmp29A <= (void*)3?1:*((
int*)_tmp29A)!= 10)goto _LL1C5;_tmp29F=((struct Cyc_Absyn_AggrType_struct*)_tmp29A)->f1;
_tmp2A0=(void*)_tmp29F.aggr_info;_tmp2A1=_tmp29F.targs;_LL1C4: {struct Cyc_Absyn_Aggrdecl*
_tmp2A3=Cyc_Absyn_get_known_aggrdecl(_tmp2A0);if(_tmp2A3->exist_vars == 0?1:(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A3->exist_vars))->v
!= 0)return 0;{struct _RegionHandle _tmp2A4=_new_region("r");struct _RegionHandle*r=&
_tmp2A4;_push_region(r);{struct Cyc_List_List*_tmp2A5=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp2A3->tvs,_tmp2A1);{struct Cyc_List_List*fs=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2A3->fields))->v;for(0;fs != 0;fs=fs->tl){if(
!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp2A5,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmp2A6=0;_npop_handler(0);return _tmp2A6;}}}{int _tmp2A7=1;
_npop_handler(0);return _tmp2A7;}};_pop_region(r);}}_LL1C5: if(_tmp29A <= (void*)3?
1:*((int*)_tmp29A)!= 12)goto _LL1C7;_LL1C6: return 1;_LL1C7: if(_tmp29A <= (void*)3?1:*((
int*)_tmp29A)!= 14)goto _LL1C9;_LL1C8: return 1;_LL1C9: if(_tmp29A <= (void*)3?1:*((
int*)_tmp29A)!= 11)goto _LL1CB;_tmp2A2=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp29A)->f2;_LL1CA: for(0;_tmp2A2 != 0;_tmp2A2=_tmp2A2->tl){if(!Cyc_Tcexp_zeroable_type((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp2A2->hd)->type))return 0;}return 1;_LL1CB:
if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 13)goto _LL1CD;_LL1CC: return 1;_LL1CD:
if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 15)goto _LL1CF;_LL1CE: return 0;_LL1CF:
if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 16)goto _LL1D1;_LL1D0: goto _LL1D2;
_LL1D1: if((int)_tmp29A != 2)goto _LL1D3;_LL1D2: goto _LL1D4;_LL1D3: if(_tmp29A <= (
void*)3?1:*((int*)_tmp29A)!= 17)goto _LL1D5;_LL1D4: goto _LL1D6;_LL1D5: if(_tmp29A <= (
void*)3?1:*((int*)_tmp29A)!= 18)goto _LL1D7;_LL1D6: goto _LL1D8;_LL1D7: if(_tmp29A <= (
void*)3?1:*((int*)_tmp29A)!= 19)goto _LL1AA;_LL1D8:({struct Cyc_Std_String_pa_struct
_tmp2A9;_tmp2A9.tag=0;_tmp2A9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp2A8[1]={& _tmp2A9};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bad type `%s' in zeroable type",sizeof(unsigned char),
31),_tag_arr(_tmp2A8,sizeof(void*),1));}});_LL1AA:;}static void Cyc_Tcexp_check_malloc_type(
int allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;if(topt != 0){void*_tmp2AA=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2AB;void*_tmp2AC;_LL1DA:
if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 4)goto _LL1DC;_tmp2AB=((struct Cyc_Absyn_PointerType_struct*)
_tmp2AA)->f1;_tmp2AC=(void*)_tmp2AB.elt_typ;_LL1DB: Cyc_Tcutil_unify(_tmp2AC,t);
if(Cyc_Tcutil_bits_only(t)?1:(allow_zero?Cyc_Tcexp_zeroable_type(t): 0))return;
goto _LL1D9;_LL1DC:;_LL1DD: goto _LL1D9;_LL1D9:;}({struct Cyc_Std_String_pa_struct
_tmp2AF;_tmp2AF.tag=0;_tmp2AF.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp2AE;_tmp2AE.tag=0;_tmp2AE.f1=(struct
_tagged_arr)(allow_zero?_tag_arr("calloc",sizeof(unsigned char),7): _tag_arr("malloc",
sizeof(unsigned char),7));{void*_tmp2AD[2]={& _tmp2AE,& _tmp2AF};Cyc_Tcutil_terr(
loc,_tag_arr("%s cannot be used with type %s\n\t(type needs initialization)",
sizeof(unsigned char),60),_tag_arr(_tmp2AD,sizeof(void*),2));}}});}static void*
Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*ropt,void***t,struct Cyc_Absyn_Exp**e,int is_calloc,int*
is_fat){void*rgn=(void*)2;if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2B5;_tmp2B5.tag=15;_tmp2B5.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2B6;}));_tmp2B5;});_tmp2B4;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(ropt));void*_tmp2B0=Cyc_Tcutil_compress(handle_type);void*_tmp2B1;
_LL1DF: if(_tmp2B0 <= (void*)3?1:*((int*)_tmp2B0)!= 15)goto _LL1E1;_tmp2B1=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp2B0)->f1;_LL1E0: rgn=_tmp2B1;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL1DE;_LL1E1:;_LL1E2:({struct Cyc_Std_String_pa_struct _tmp2B3;
_tmp2B3.tag=0;_tmp2B3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type);{
void*_tmp2B2[1]={& _tmp2B3};Cyc_Tcutil_terr(ropt->loc,_tag_arr("expecting region_t type but found %s",
sizeof(unsigned char),37),_tag_arr(_tmp2B2,sizeof(void*),1));}});goto _LL1DE;
_LL1DE:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_t,*e);{void*elt_type;struct
Cyc_Absyn_Exp*num_elts;int one_elt;if(is_calloc){if(*t == 0)({void*_tmp2B7[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("calloc with empty type",
sizeof(unsigned char),23),_tag_arr(_tmp2B7,sizeof(void*),0));});elt_type=*((void**)
_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;
one_elt=0;}else{void*_tmp2B8=(void*)(*e)->r;void*_tmp2B9;void*_tmp2BA;struct Cyc_List_List*
_tmp2BB;struct Cyc_List_List _tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_List_List*
_tmp2BE;struct Cyc_List_List _tmp2BF;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_List_List*
_tmp2C1;_LL1E4: if(*((int*)_tmp2B8)!= 16)goto _LL1E6;_tmp2B9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp2B8)->f1;_LL1E5: elt_type=_tmp2B9;*t=(void**)({void**_tmp2C2=_cycalloc(
sizeof(*_tmp2C2));_tmp2C2[0]=elt_type;_tmp2C2;});num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_t,num_elts);one_elt=1;goto _LL1E3;
_LL1E6: if(*((int*)_tmp2B8)!= 3)goto _LL1E8;_tmp2BA=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2B8)->f1;if((int)_tmp2BA != 1)goto _LL1E8;_tmp2BB=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2B8)->f2;if(_tmp2BB == 0)goto _LL1E8;_tmp2BC=*_tmp2BB;_tmp2BD=(struct Cyc_Absyn_Exp*)
_tmp2BC.hd;_tmp2BE=_tmp2BC.tl;if(_tmp2BE == 0)goto _LL1E8;_tmp2BF=*_tmp2BE;_tmp2C0=(
struct Cyc_Absyn_Exp*)_tmp2BF.hd;_tmp2C1=_tmp2BF.tl;if(_tmp2C1 != 0)goto _LL1E8;
_LL1E7:{struct _tuple0 _tmp2C4=({struct _tuple0 _tmp2C3;_tmp2C3.f1=(void*)_tmp2BD->r;
_tmp2C3.f2=(void*)_tmp2C0->r;_tmp2C3;});void*_tmp2C5;void*_tmp2C6;void*_tmp2C7;
void*_tmp2C8;_LL1EB: _tmp2C5=_tmp2C4.f1;if(*((int*)_tmp2C5)!= 16)goto _LL1ED;
_tmp2C6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2C5)->f1;_LL1EC: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp2C6);elt_type=_tmp2C6;*t=(void**)({void**_tmp2C9=_cycalloc(
sizeof(*_tmp2C9));_tmp2C9[0]=elt_type;_tmp2C9;});num_elts=_tmp2C0;one_elt=0;goto
_LL1EA;_LL1ED: _tmp2C7=_tmp2C4.f2;if(*((int*)_tmp2C7)!= 16)goto _LL1EF;_tmp2C8=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2C7)->f1;_LL1EE: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp2C8);elt_type=_tmp2C8;*t=(void**)({void**_tmp2CA=_cycalloc(
sizeof(*_tmp2CA));_tmp2CA[0]=elt_type;_tmp2CA;});num_elts=_tmp2BD;one_elt=0;goto
_LL1EA;_LL1EF:;_LL1F0: goto No_sizeof;_LL1EA:;}goto _LL1E3;_LL1E8:;_LL1E9: No_sizeof:
elt_type=Cyc_Absyn_uchar_t;*t=(void**)({void**_tmp2CB=_cycalloc(sizeof(*_tmp2CB));
_tmp2CB[0]=elt_type;_tmp2CB;});num_elts=*e;one_elt=0;goto _LL1E3;_LL1E3:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp2CC=elt_type;struct Cyc_Absyn_AggrInfo _tmp2CD;
void*_tmp2CE;struct Cyc_Absyn_Aggrdecl**_tmp2CF;struct Cyc_Absyn_Aggrdecl*_tmp2D0;
_LL1F2: if(_tmp2CC <= (void*)3?1:*((int*)_tmp2CC)!= 10)goto _LL1F4;_tmp2CD=((struct
Cyc_Absyn_AggrType_struct*)_tmp2CC)->f1;_tmp2CE=(void*)_tmp2CD.aggr_info;if(*((
int*)_tmp2CE)!= 1)goto _LL1F4;_tmp2CF=((struct Cyc_Absyn_KnownAggr_struct*)_tmp2CE)->f1;
_tmp2D0=*_tmp2CF;_LL1F3: if(_tmp2D0->exist_vars != 0?(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2D0->exist_vars))->v != 0: 0)({void*_tmp2D1[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("malloc with existential types not yet implemented",
sizeof(unsigned char),50),_tag_arr(_tmp2D1,sizeof(void*),0));});goto _LL1F1;
_LL1F4:;_LL1F5: goto _LL1F1;_LL1F1:;}{void*(*_tmp2D2)(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq)=Cyc_Absyn_at_typ;if(topt != 0){void*_tmp2D3=Cyc_Tcutil_compress(*topt);struct
Cyc_Absyn_PtrInfo _tmp2D4;struct Cyc_Absyn_Conref*_tmp2D5;_LL1F7: if(_tmp2D3 <= (
void*)3?1:*((int*)_tmp2D3)!= 4)goto _LL1F9;_tmp2D4=((struct Cyc_Absyn_PointerType_struct*)
_tmp2D3)->f1;_tmp2D5=_tmp2D4.nullable;_LL1F8: if(((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp2D5))_tmp2D2=Cyc_Absyn_star_typ;goto _LL1F6;_LL1F9:;
_LL1FA: goto _LL1F6;_LL1F6:;}if(!one_elt)_tmp2D2=Cyc_Absyn_tagged_typ;return
_tmp2D2(elt_type,rgn,Cyc_Absyn_empty_tqual());}}}static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(te,s),s,1);Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp2D6=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp2D7;struct Cyc_Absyn_Stmt*_tmp2D8;struct Cyc_Absyn_Stmt*_tmp2D9;struct Cyc_Absyn_Decl*
_tmp2DA;struct Cyc_Absyn_Stmt*_tmp2DB;_LL1FC: if(_tmp2D6 <= (void*)1?1:*((int*)
_tmp2D6)!= 0)goto _LL1FE;_tmp2D7=((struct Cyc_Absyn_Exp_s_struct*)_tmp2D6)->f1;
_LL1FD: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2D7->topt))->v;_LL1FE:
if(_tmp2D6 <= (void*)1?1:*((int*)_tmp2D6)!= 1)goto _LL200;_tmp2D8=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2D6)->f1;_tmp2D9=((struct Cyc_Absyn_Seq_s_struct*)_tmp2D6)->f2;_LL1FF: s=
_tmp2D9;continue;_LL200: if(_tmp2D6 <= (void*)1?1:*((int*)_tmp2D6)!= 12)goto _LL202;
_tmp2DA=((struct Cyc_Absyn_Decl_s_struct*)_tmp2D6)->f1;_tmp2DB=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2D6)->f2;_LL201: s=_tmp2DB;continue;_LL202:;_LL203: return({void*_tmp2DC[0]={};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("statement expression must end with expression",
sizeof(unsigned char),46),_tag_arr(_tmp2DC,sizeof(void*),0));});_LL1FB:;}}static
void*Cyc_Tcexp_tcCodegen(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Fndecl*fd){return({void*_tmp2DD[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("tcCodegen",sizeof(unsigned char),10),_tag_arr(_tmp2DD,sizeof(
void*),0));});}static void*Cyc_Tcexp_tcFill(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){return({void*_tmp2DE[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("tcFill",sizeof(unsigned char),7),_tag_arr(_tmp2DE,sizeof(void*),
0));});}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2E4;_tmp2E4.tag=15;_tmp2E4.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2E5=_cycalloc(sizeof(*_tmp2E5));_tmp2E5->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2E5;}));_tmp2E4;});_tmp2E3;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(rgn_handle));void*_tmp2DF=Cyc_Tcutil_compress(handle_type);void*
_tmp2E0;_LL205: if(_tmp2DF <= (void*)3?1:*((int*)_tmp2DF)!= 15)goto _LL207;_tmp2E0=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2DF)->f1;_LL206: rgn=_tmp2E0;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL204;_LL207:;_LL208:({struct Cyc_Std_String_pa_struct
_tmp2E2;_tmp2E2.tag=0;_tmp2E2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
handle_type);{void*_tmp2E1[1]={& _tmp2E2};Cyc_Tcutil_terr(rgn_handle->loc,
_tag_arr("expecting region_t type but found %s",sizeof(unsigned char),37),
_tag_arr(_tmp2E1,sizeof(void*),1));}});goto _LL204;_LL204:;}{void*_tmp2E6=(void*)
e1->r;struct Cyc_Absyn_Vardecl*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E8;struct Cyc_Absyn_Exp*
_tmp2E9;struct Cyc_Core_Opt*_tmp2EA;struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*
_tmp2EC;void*_tmp2ED;struct _tagged_arr _tmp2EE;_LL20A: if(*((int*)_tmp2E6)!= 27)
goto _LL20C;_tmp2E7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2E6)->f1;
_tmp2E8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2E6)->f2;_tmp2E9=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2E6)->f3;_LL20B: {void*_tmp2EF=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp2F0=Cyc_Tcutil_compress(_tmp2EF);void*_tmp2F1;struct Cyc_Absyn_Tqual
_tmp2F2;_LL215: if(_tmp2F0 <= (void*)3?1:*((int*)_tmp2F0)!= 7)goto _LL217;_tmp2F1=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp2F0)->f1;_tmp2F2=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2F0)->f2;_LL216: {void*b=Cyc_Tcutil_is_const_exp(te,_tmp2E8)?(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp2F7=_cycalloc(sizeof(*_tmp2F7));_tmp2F7[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp2F8;_tmp2F8.tag=0;_tmp2F8.f1=_tmp2E8;_tmp2F8;});
_tmp2F7;}):(void*)0;void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4[0]=({struct Cyc_Absyn_PointerType_struct
_tmp2F5;_tmp2F5.tag=4;_tmp2F5.f1=({struct Cyc_Absyn_PtrInfo _tmp2F6;_tmp2F6.elt_typ=(
void*)_tmp2F1;_tmp2F6.rgn_typ=(void*)rgn;_tmp2F6.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp2F6.tq=_tmp2F2;_tmp2F6.bounds=Cyc_Absyn_new_conref(
b);_tmp2F6;});_tmp2F5;});_tmp2F4;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,
res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*
_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3->v=(void*)res_typ;_tmp2F3;});Cyc_Tcutil_unchecked_cast(
te,e,*topt);res_typ=*topt;}}return res_typ;}_LL217:;_LL218:({void*_tmp2F9[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcNew: comprehension returned non-array type",
sizeof(unsigned char),45),_tag_arr(_tmp2F9,sizeof(void*),0));});_LL214:;}_LL20C:
if(*((int*)_tmp2E6)!= 34)goto _LL20E;_tmp2EA=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp2E6)->f1;_tmp2EB=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp2E6)->f2;
_LL20D:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp2FA=
_cycalloc(sizeof(*_tmp2FA));_tmp2FA[0]=({struct Cyc_Absyn_Array_e_struct _tmp2FB;
_tmp2FB.tag=26;_tmp2FB.f1=_tmp2EB;_tmp2FB;});_tmp2FA;})));_tmp2EC=_tmp2EB;goto
_LL20F;_LL20E: if(*((int*)_tmp2E6)!= 26)goto _LL210;_tmp2EC=((struct Cyc_Absyn_Array_e_struct*)
_tmp2E6)->f1;_LL20F: {void**elt_typ_opt=0;if(topt != 0){void*_tmp2FC=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp2FD;void*_tmp2FE;void**_tmp2FF;struct Cyc_Absyn_Tqual
_tmp300;_LL21A: if(_tmp2FC <= (void*)3?1:*((int*)_tmp2FC)!= 4)goto _LL21C;_tmp2FD=((
struct Cyc_Absyn_PointerType_struct*)_tmp2FC)->f1;_tmp2FE=(void*)_tmp2FD.elt_typ;
_tmp2FF=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp2FC)->f1).elt_typ;
_tmp300=_tmp2FD.tq;_LL21B: elt_typ_opt=(void**)_tmp2FF;goto _LL219;_LL21C:;_LL21D:
goto _LL219;_LL219:;}{void*_tmp301=Cyc_Tcexp_tcExpNoPromote(te,elt_typ_opt,e1);
void*res_typ;{void*_tmp302=Cyc_Tcutil_compress(_tmp301);void*_tmp303;struct Cyc_Absyn_Tqual
_tmp304;struct Cyc_Absyn_Exp*_tmp305;_LL21F: if(_tmp302 <= (void*)3?1:*((int*)
_tmp302)!= 7)goto _LL221;_tmp303=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp302)->f1;_tmp304=((struct Cyc_Absyn_ArrayType_struct*)_tmp302)->f2;_tmp305=((
struct Cyc_Absyn_ArrayType_struct*)_tmp302)->f3;_LL220: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp306=_cycalloc(sizeof(*_tmp306));_tmp306[0]=({struct Cyc_Absyn_PointerType_struct
_tmp307;_tmp307.tag=4;_tmp307.f1=({struct Cyc_Absyn_PtrInfo _tmp308;_tmp308.elt_typ=(
void*)_tmp303;_tmp308.rgn_typ=(void*)rgn;_tmp308.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp308.tq=_tmp304;_tmp308.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp309=_cycalloc(sizeof(*_tmp309));
_tmp309[0]=({struct Cyc_Absyn_Upper_b_struct _tmp30A;_tmp30A.tag=0;_tmp30A.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp305);_tmp30A;});_tmp309;}));_tmp308;});
_tmp307;});_tmp306;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp30B=_cycalloc(sizeof(*
_tmp30B));_tmp30B->v=(void*)res_typ;_tmp30B;});Cyc_Tcutil_unchecked_cast(te,e,*
topt);res_typ=*topt;}}goto _LL21E;_LL221:;_LL222:({void*_tmp30C[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof(unsigned char),50),_tag_arr(_tmp30C,sizeof(void*),0));});_LL21E:;}return
res_typ;}}_LL210: if(*((int*)_tmp2E6)!= 0)goto _LL212;_tmp2ED=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2E6)->f1;if(_tmp2ED <= (void*)1?1:*((int*)_tmp2ED)!= 5)goto _LL212;_tmp2EE=((
struct Cyc_Absyn_String_c_struct*)_tmp2ED)->f1;_LL211: {void*_tmp30D=Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,rgn,Cyc_Absyn_const_tqual(),(void*)0);void*_tmp30E=Cyc_Tcexp_tcExp(
te,(void**)& _tmp30D,e1);return Cyc_Absyn_atb_typ(_tmp30E,rgn,Cyc_Absyn_empty_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp30F=_cycalloc(sizeof(*_tmp30F));
_tmp30F[0]=({struct Cyc_Absyn_Upper_b_struct _tmp310;_tmp310.tag=0;_tmp310.f1=Cyc_Absyn_uint_exp(
1,0);_tmp310;});_tmp30F;}));}_LL212:;_LL213: {void**topt2=0;if(topt != 0){void*
_tmp311=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp312;void*_tmp313;
void**_tmp314;struct Cyc_Absyn_Conref*_tmp315;_LL224: if(_tmp311 <= (void*)3?1:*((
int*)_tmp311)!= 4)goto _LL226;_tmp312=((struct Cyc_Absyn_PointerType_struct*)
_tmp311)->f1;_tmp313=(void*)_tmp312.elt_typ;_tmp314=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp311)->f1).elt_typ;_tmp315=_tmp312.nullable;_LL225: topt2=(void**)_tmp314;goto
_LL223;_LL226: if(_tmp311 <= (void*)3?1:*((int*)_tmp311)!= 2)goto _LL228;_LL227:
topt2=({void**_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316[0]=*topt;_tmp316;});
goto _LL223;_LL228:;_LL229: goto _LL223;_LL223:;}{void*telt=Cyc_Tcexp_tcExp(te,
topt2,e1);void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp318=
_cycalloc(sizeof(*_tmp318));_tmp318[0]=({struct Cyc_Absyn_PointerType_struct
_tmp319;_tmp319.tag=4;_tmp319.f1=({struct Cyc_Absyn_PtrInfo _tmp31A;_tmp31A.elt_typ=(
void*)telt;_tmp31A.rgn_typ=(void*)rgn;_tmp31A.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp31A.tq=Cyc_Absyn_empty_tqual();_tmp31A.bounds=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp31B=_cycalloc(sizeof(*_tmp31B));
_tmp31B[0]=({struct Cyc_Absyn_Upper_b_struct _tmp31C;_tmp31C.tag=0;_tmp31C.f1=Cyc_Absyn_uint_exp(
1,0);_tmp31C;});_tmp31B;}));_tmp31A;});_tmp319;});_tmp318;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ)?Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt):
0){e->topt=({struct Cyc_Core_Opt*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->v=(
void*)res_typ;_tmp317;});Cyc_Tcutil_unchecked_cast(te,e,*topt);res_typ=*topt;}}
return res_typ;}}_LL209:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp31D=t;void*_tmp31E;struct Cyc_Absyn_Tqual _tmp31F;struct Cyc_Absyn_Exp*
_tmp320;_LL22B: if(_tmp31D <= (void*)3?1:*((int*)_tmp31D)!= 7)goto _LL22D;_tmp31E=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp31D)->f1;_tmp31F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp31D)->f2;_tmp320=((struct Cyc_Absyn_ArrayType_struct*)_tmp31D)->f3;_LL22C: {
void*_tmp322;struct _tuple6 _tmp321=Cyc_Tcutil_addressof_props(te,e);_tmp322=
_tmp321.f2;{void*_tmp323=_tmp320 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp325;_tmp325.tag=0;_tmp325.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp320);
_tmp325;});_tmp324;});t=Cyc_Absyn_atb_typ(_tmp31E,_tmp322,_tmp31F,_tmp323);(void*)(((
struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);return t;}}_LL22D:;_LL22E:
return t;_LL22A:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);void*
_tmp326=(void*)e->r;void*_tmp327;_LL230: if(*((int*)_tmp326)!= 26)goto _LL232;
_LL231: goto _LL233;_LL232: if(*((int*)_tmp326)!= 27)goto _LL234;_LL233: goto _LL235;
_LL234: if(*((int*)_tmp326)!= 0)goto _LL236;_tmp327=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp326)->f1;if(_tmp327 <= (void*)1?1:*((int*)_tmp327)!= 5)goto _LL236;_LL235:
return t;_LL236:;_LL237: t=Cyc_Tcutil_compress(t);{void*_tmp328=t;void*_tmp329;
struct Cyc_Absyn_Tqual _tmp32A;struct Cyc_Absyn_Exp*_tmp32B;_LL239: if(_tmp328 <= (
void*)3?1:*((int*)_tmp328)!= 7)goto _LL23B;_tmp329=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp328)->f1;_tmp32A=((struct Cyc_Absyn_ArrayType_struct*)_tmp328)->f2;_tmp32B=((
struct Cyc_Absyn_ArrayType_struct*)_tmp328)->f3;_LL23A: {void*_tmp32D;struct
_tuple6 _tmp32C=Cyc_Tcutil_addressof_props(te,e);_tmp32D=_tmp32C.f2;{void*b=
_tmp32B == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp32E=_cycalloc(
sizeof(*_tmp32E));_tmp32E[0]=({struct Cyc_Absyn_Upper_b_struct _tmp32F;_tmp32F.tag=
0;_tmp32F.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp32B);_tmp32F;});_tmp32E;});t=
Cyc_Absyn_atb_typ(_tmp329,_tmp32D,_tmp32A,b);Cyc_Tcutil_unchecked_cast(te,e,t);
return t;}}_LL23B:;_LL23C: return t;_LL238:;}_LL22F:;}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp330=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp331;_LL23E: if(*((int*)_tmp330)!= 11)goto _LL240;_tmp331=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp330)->f1;_LL23F: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp331);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp331->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp331->topt))->v));
e->topt=_tmp331->topt;goto _LL23D;_LL240:;_LL241: Cyc_Tcexp_tcExpNoInst(te,topt,e);(
void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp332=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp333;void*_tmp334;void*_tmp335;struct Cyc_Absyn_Conref*_tmp336;struct Cyc_Absyn_Tqual
_tmp337;struct Cyc_Absyn_Conref*_tmp338;_LL243: if(_tmp332 <= (void*)3?1:*((int*)
_tmp332)!= 4)goto _LL245;_tmp333=((struct Cyc_Absyn_PointerType_struct*)_tmp332)->f1;
_tmp334=(void*)_tmp333.elt_typ;_tmp335=(void*)_tmp333.rgn_typ;_tmp336=_tmp333.nullable;
_tmp337=_tmp333.tq;_tmp338=_tmp333.bounds;_LL244:{void*_tmp339=Cyc_Tcutil_compress(
_tmp334);struct Cyc_Absyn_FnInfo _tmp33A;struct Cyc_List_List*_tmp33B;struct Cyc_Core_Opt*
_tmp33C;void*_tmp33D;struct Cyc_List_List*_tmp33E;int _tmp33F;struct Cyc_Absyn_VarargInfo*
_tmp340;struct Cyc_List_List*_tmp341;struct Cyc_List_List*_tmp342;_LL248: if(
_tmp339 <= (void*)3?1:*((int*)_tmp339)!= 8)goto _LL24A;_tmp33A=((struct Cyc_Absyn_FnType_struct*)
_tmp339)->f1;_tmp33B=_tmp33A.tvars;_tmp33C=_tmp33A.effect;_tmp33D=(void*)_tmp33A.ret_typ;
_tmp33E=_tmp33A.args;_tmp33F=_tmp33A.c_varargs;_tmp340=_tmp33A.cyc_varargs;
_tmp341=_tmp33A.rgn_po;_tmp342=_tmp33A.attributes;_LL249: if(_tmp33B != 0){struct
_RegionHandle _tmp343=_new_region("rgn");struct _RegionHandle*rgn=& _tmp343;
_push_region(rgn);{struct _tuple4 _tmp344=({struct _tuple4 _tmp34E;_tmp34E.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp34E.f2=rgn;_tmp34E;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp344,_tmp33B);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B[0]=({struct Cyc_Absyn_FnType_struct
_tmp34C;_tmp34C.tag=8;_tmp34C.f1=({struct Cyc_Absyn_FnInfo _tmp34D;_tmp34D.tvars=0;
_tmp34D.effect=_tmp33C;_tmp34D.ret_typ=(void*)_tmp33D;_tmp34D.args=_tmp33E;
_tmp34D.c_varargs=_tmp33F;_tmp34D.cyc_varargs=_tmp340;_tmp34D.rgn_po=_tmp341;
_tmp34D.attributes=_tmp342;_tmp34D;});_tmp34C;});_tmp34B;}));void*new_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348[0]=({
struct Cyc_Absyn_PointerType_struct _tmp349;_tmp349.tag=4;_tmp349.f1=({struct Cyc_Absyn_PtrInfo
_tmp34A;_tmp34A.elt_typ=(void*)ftyp;_tmp34A.rgn_typ=(void*)_tmp335;_tmp34A.nullable=
_tmp336;_tmp34A.tq=_tmp337;_tmp34A.bounds=_tmp338;_tmp34A;});_tmp349;});_tmp348;});
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Instantiate_e_struct*_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345[
0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp346;_tmp346.tag=12;_tmp346.f1=inner;
_tmp346.f2=ts;_tmp346;});_tmp345;})));e->topt=({struct Cyc_Core_Opt*_tmp347=
_cycalloc(sizeof(*_tmp347));_tmp347->v=(void*)new_typ;_tmp347;});};_pop_region(
rgn);}goto _LL247;_LL24A:;_LL24B: goto _LL247;_LL247:;}goto _LL242;_LL245:;_LL246:
goto _LL242;_LL242:;}goto _LL23D;_LL23D:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*
t;{void*_tmp34F=(void*)e->r;struct Cyc_Absyn_Exp*_tmp350;struct _tuple1*_tmp351;
struct Cyc_Absyn_Exp*_tmp352;struct Cyc_List_List*_tmp353;struct Cyc_Core_Opt*
_tmp354;struct Cyc_List_List*_tmp355;void*_tmp356;struct _tuple1*_tmp357;void*
_tmp358;void*_tmp359;struct Cyc_List_List*_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;
void*_tmp35C;struct Cyc_Absyn_Exp*_tmp35D;struct Cyc_Core_Opt*_tmp35E;struct Cyc_Absyn_Exp*
_tmp35F;struct Cyc_Absyn_Exp*_tmp360;struct Cyc_Absyn_Exp*_tmp361;struct Cyc_Absyn_Exp*
_tmp362;struct Cyc_Absyn_Exp*_tmp363;struct Cyc_Absyn_Exp*_tmp364;struct Cyc_Absyn_Exp*
_tmp365;struct Cyc_List_List*_tmp366;struct Cyc_Absyn_VarargCallInfo*_tmp367;
struct Cyc_Absyn_VarargCallInfo**_tmp368;struct Cyc_Absyn_Exp*_tmp369;struct Cyc_Absyn_Exp*
_tmp36A;struct Cyc_List_List*_tmp36B;void*_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;
struct Cyc_Absyn_Exp*_tmp36E;struct Cyc_Absyn_Exp*_tmp36F;struct Cyc_Absyn_Exp*
_tmp370;struct Cyc_Absyn_Exp*_tmp371;void*_tmp372;void*_tmp373;void*_tmp374;
struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*_tmp376;struct _tagged_arr*
_tmp377;struct Cyc_Absyn_Exp*_tmp378;struct _tagged_arr*_tmp379;struct Cyc_Absyn_Exp*
_tmp37A;struct Cyc_Absyn_Exp*_tmp37B;struct Cyc_List_List*_tmp37C;struct _tuple2*
_tmp37D;struct Cyc_List_List*_tmp37E;struct Cyc_List_List*_tmp37F;struct Cyc_Absyn_Stmt*
_tmp380;struct Cyc_Absyn_Fndecl*_tmp381;struct Cyc_Absyn_Exp*_tmp382;struct Cyc_Absyn_Vardecl*
_tmp383;struct Cyc_Absyn_Exp*_tmp384;struct Cyc_Absyn_Exp*_tmp385;struct _tuple1*
_tmp386;struct _tuple1**_tmp387;struct Cyc_List_List*_tmp388;struct Cyc_List_List**
_tmp389;struct Cyc_List_List*_tmp38A;struct Cyc_Absyn_Aggrdecl*_tmp38B;struct Cyc_Absyn_Aggrdecl**
_tmp38C;void*_tmp38D;struct Cyc_List_List*_tmp38E;struct Cyc_List_List*_tmp38F;
struct Cyc_Absyn_Tuniondecl*_tmp390;struct Cyc_Absyn_Tunionfield*_tmp391;struct
_tuple1*_tmp392;struct _tuple1**_tmp393;struct Cyc_Absyn_Enumdecl*_tmp394;struct
Cyc_Absyn_Enumfield*_tmp395;struct _tuple1*_tmp396;struct _tuple1**_tmp397;void*
_tmp398;struct Cyc_Absyn_Enumfield*_tmp399;struct Cyc_Absyn_MallocInfo _tmp39A;int
_tmp39B;struct Cyc_Absyn_Exp*_tmp39C;void**_tmp39D;void***_tmp39E;struct Cyc_Absyn_Exp*
_tmp39F;struct Cyc_Absyn_Exp**_tmp3A0;int _tmp3A1;int*_tmp3A2;_LL24D: if(*((int*)
_tmp34F)!= 11)goto _LL24F;_tmp350=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp34F)->f1;_LL24E: Cyc_Tcexp_tcExpNoInst(te,0,_tmp350);return;_LL24F: if(*((int*)
_tmp34F)!= 2)goto _LL251;_tmp351=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp34F)->f1;
_LL250: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp351);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL251: if(*((int*)_tmp34F)!= 8)goto _LL253;_tmp352=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp34F)->f1;_tmp353=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp34F)->f2;_LL252:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp352,_tmp353);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL253: if(*((int*)_tmp34F)!= 34)goto _LL255;_tmp354=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp34F)->f1;_tmp355=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp34F)->f2;
_LL254: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp355);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL255: if(*((int*)_tmp34F)!= 0)goto _LL257;_tmp356=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp34F)->f1;_LL256: t=Cyc_Tcexp_tcConst(te,loc,
topt,_tmp356,e);goto _LL24C;_LL257: if(*((int*)_tmp34F)!= 1)goto _LL259;_tmp357=((
struct Cyc_Absyn_Var_e_struct*)_tmp34F)->f1;_tmp358=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp34F)->f2;_LL258: t=Cyc_Tcexp_tcVar(te,loc,_tmp357,_tmp358);goto _LL24C;_LL259:
if(*((int*)_tmp34F)!= 3)goto _LL25B;_tmp359=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp34F)->f1;_tmp35A=((struct Cyc_Absyn_Primop_e_struct*)_tmp34F)->f2;_LL25A: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp359,_tmp35A);goto _LL24C;_LL25B: if(*((int*)
_tmp34F)!= 5)goto _LL25D;_tmp35B=((struct Cyc_Absyn_Increment_e_struct*)_tmp34F)->f1;
_tmp35C=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp34F)->f2;_LL25C: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp35B,_tmp35C);goto _LL24C;_LL25D: if(*((int*)_tmp34F)!= 4)goto
_LL25F;_tmp35D=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp34F)->f1;_tmp35E=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp34F)->f2;_tmp35F=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp34F)->f3;_LL25E: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp35D,_tmp35E,_tmp35F);
goto _LL24C;_LL25F: if(*((int*)_tmp34F)!= 6)goto _LL261;_tmp360=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp34F)->f1;_tmp361=((struct Cyc_Absyn_Conditional_e_struct*)_tmp34F)->f2;
_tmp362=((struct Cyc_Absyn_Conditional_e_struct*)_tmp34F)->f3;_LL260: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp360,_tmp361,_tmp362);goto _LL24C;_LL261: if(*((int*)_tmp34F)!= 7)
goto _LL263;_tmp363=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp34F)->f1;_tmp364=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp34F)->f2;_LL262: t=Cyc_Tcexp_tcSeqExp(te,loc,
topt,_tmp363,_tmp364);goto _LL24C;_LL263: if(*((int*)_tmp34F)!= 9)goto _LL265;
_tmp365=((struct Cyc_Absyn_FnCall_e_struct*)_tmp34F)->f1;_tmp366=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp34F)->f2;_tmp367=((struct Cyc_Absyn_FnCall_e_struct*)_tmp34F)->f3;_tmp368=(
struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)_tmp34F)->f3;
_LL264: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp365,_tmp366,_tmp368);goto _LL24C;
_LL265: if(*((int*)_tmp34F)!= 10)goto _LL267;_tmp369=((struct Cyc_Absyn_Throw_e_struct*)
_tmp34F)->f1;_LL266: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp369);goto _LL24C;_LL267:
if(*((int*)_tmp34F)!= 12)goto _LL269;_tmp36A=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp34F)->f1;_tmp36B=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp34F)->f2;_LL268:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp36A,_tmp36B);goto _LL24C;_LL269: if(*((
int*)_tmp34F)!= 13)goto _LL26B;_tmp36C=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp34F)->f1;_tmp36D=((struct Cyc_Absyn_Cast_e_struct*)_tmp34F)->f2;_LL26A: t=Cyc_Tcexp_tcCast(
te,loc,topt,_tmp36C,_tmp36D);goto _LL24C;_LL26B: if(*((int*)_tmp34F)!= 14)goto
_LL26D;_tmp36E=((struct Cyc_Absyn_Address_e_struct*)_tmp34F)->f1;_LL26C: t=Cyc_Tcexp_tcAddress(
te,loc,e,topt,_tmp36E);goto _LL24C;_LL26D: if(*((int*)_tmp34F)!= 15)goto _LL26F;
_tmp36F=((struct Cyc_Absyn_New_e_struct*)_tmp34F)->f1;_tmp370=((struct Cyc_Absyn_New_e_struct*)
_tmp34F)->f2;_LL26E: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp36F,e,_tmp370);goto _LL24C;
_LL26F: if(*((int*)_tmp34F)!= 17)goto _LL271;_tmp371=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp34F)->f1;_LL270: {void*_tmp3A3=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp371);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp3A3);goto _LL24C;}_LL271: if(*((int*)_tmp34F)!= 16)goto _LL273;
_tmp372=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp34F)->f1;_LL272: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp372);goto _LL24C;_LL273: if(*((int*)_tmp34F)!= 18)goto _LL275;
_tmp373=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp34F)->f1;_tmp374=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp34F)->f2;_LL274: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp373,_tmp374);goto _LL24C;_LL275: if(*((int*)_tmp34F)!= 19)goto _LL277;
_LL276:({void*_tmp3A4[0]={};Cyc_Tcutil_terr(loc,_tag_arr("gen() not in top-level initializer",
sizeof(unsigned char),35),_tag_arr(_tmp3A4,sizeof(void*),0));});return;_LL277:
if(*((int*)_tmp34F)!= 20)goto _LL279;_tmp375=((struct Cyc_Absyn_Deref_e_struct*)
_tmp34F)->f1;_LL278: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp375);goto _LL24C;_LL279:
if(*((int*)_tmp34F)!= 21)goto _LL27B;_tmp376=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp34F)->f1;_tmp377=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp34F)->f2;_LL27A:
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp376,_tmp377);goto _LL24C;_LL27B: if(*((
int*)_tmp34F)!= 22)goto _LL27D;_tmp378=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp34F)->f1;_tmp379=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp34F)->f2;_LL27C: t=
Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp378,_tmp379);goto _LL24C;_LL27D: if(*((int*)
_tmp34F)!= 23)goto _LL27F;_tmp37A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp34F)->f1;
_tmp37B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp34F)->f2;_LL27E: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp37A,_tmp37B);goto _LL24C;_LL27F: if(*((int*)_tmp34F)!= 24)goto
_LL281;_tmp37C=((struct Cyc_Absyn_Tuple_e_struct*)_tmp34F)->f1;_LL280: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp37C);goto _LL24C;_LL281: if(*((int*)_tmp34F)!= 25)goto _LL283;
_tmp37D=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp34F)->f1;_tmp37E=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp34F)->f2;_LL282: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp37D,_tmp37E);goto
_LL24C;_LL283: if(*((int*)_tmp34F)!= 26)goto _LL285;_tmp37F=((struct Cyc_Absyn_Array_e_struct*)
_tmp34F)->f1;_LL284: {void**elt_topt=0;if(topt != 0){void*_tmp3A5=Cyc_Tcutil_compress(*
topt);void*_tmp3A6;void**_tmp3A7;_LL29A: if(_tmp3A5 <= (void*)3?1:*((int*)_tmp3A5)
!= 7)goto _LL29C;_tmp3A6=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3A5)->f1;
_tmp3A7=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3A5)->f1);
_LL29B: elt_topt=(void**)_tmp3A7;goto _LL299;_LL29C:;_LL29D: goto _LL299;_LL299:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,_tmp37F);goto _LL24C;}_LL285: if(*((int*)_tmp34F)
!= 35)goto _LL287;_tmp380=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp34F)->f1;_LL286:
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp380);goto _LL24C;_LL287: if(*((int*)_tmp34F)
!= 36)goto _LL289;_tmp381=((struct Cyc_Absyn_Codegen_e_struct*)_tmp34F)->f1;_LL288:
t=Cyc_Tcexp_tcCodegen(te,loc,topt,_tmp381);goto _LL24C;_LL289: if(*((int*)_tmp34F)
!= 37)goto _LL28B;_tmp382=((struct Cyc_Absyn_Fill_e_struct*)_tmp34F)->f1;_LL28A: t=
Cyc_Tcexp_tcFill(te,loc,topt,_tmp382);goto _LL24C;_LL28B: if(*((int*)_tmp34F)!= 27)
goto _LL28D;_tmp383=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp34F)->f1;
_tmp384=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp34F)->f2;_tmp385=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp34F)->f3;_LL28C: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp383,_tmp384,_tmp385);goto _LL24C;_LL28D: if(*((int*)_tmp34F)!= 28)
goto _LL28F;_tmp386=((struct Cyc_Absyn_Struct_e_struct*)_tmp34F)->f1;_tmp387=(
struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp34F)->f1;_tmp388=((
struct Cyc_Absyn_Struct_e_struct*)_tmp34F)->f2;_tmp389=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Struct_e_struct*)_tmp34F)->f2;_tmp38A=((struct Cyc_Absyn_Struct_e_struct*)
_tmp34F)->f3;_tmp38B=((struct Cyc_Absyn_Struct_e_struct*)_tmp34F)->f4;_tmp38C=(
struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp34F)->f4;
_LL28E: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp387,_tmp389,_tmp38A,_tmp38C);goto
_LL24C;_LL28F: if(*((int*)_tmp34F)!= 29)goto _LL291;_tmp38D=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp34F)->f1;_tmp38E=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp34F)->f2;_LL290:
t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp38D,_tmp38E);goto _LL24C;_LL291: if(*((int*)
_tmp34F)!= 30)goto _LL293;_tmp38F=((struct Cyc_Absyn_Tunion_e_struct*)_tmp34F)->f1;
_tmp390=((struct Cyc_Absyn_Tunion_e_struct*)_tmp34F)->f2;_tmp391=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp34F)->f3;_LL292: t=Cyc_Tcexp_tcTunion(te,loc,topt,e,_tmp38F,_tmp390,_tmp391);
goto _LL24C;_LL293: if(*((int*)_tmp34F)!= 31)goto _LL295;_tmp392=((struct Cyc_Absyn_Enum_e_struct*)
_tmp34F)->f1;_tmp393=(struct _tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp34F)->f1;
_tmp394=((struct Cyc_Absyn_Enum_e_struct*)_tmp34F)->f2;_tmp395=((struct Cyc_Absyn_Enum_e_struct*)
_tmp34F)->f3;_LL294:*_tmp393=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp395))->name;
t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));
_tmp3A8[0]=({struct Cyc_Absyn_EnumType_struct _tmp3A9;_tmp3A9.tag=12;_tmp3A9.f1=((
struct Cyc_Absyn_Enumdecl*)_check_null(_tmp394))->name;_tmp3A9.f2=_tmp394;_tmp3A9;});
_tmp3A8;});goto _LL24C;_LL295: if(*((int*)_tmp34F)!= 32)goto _LL297;_tmp396=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp34F)->f1;_tmp397=(struct _tuple1**)&((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp34F)->f1;_tmp398=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp34F)->f2;_tmp399=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp34F)->f3;_LL296:*
_tmp397=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp399))->name;t=_tmp398;goto
_LL24C;_LL297: if(*((int*)_tmp34F)!= 33)goto _LL24C;_tmp39A=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp34F)->f1;_tmp39B=_tmp39A.is_calloc;_tmp39C=_tmp39A.rgn;_tmp39D=_tmp39A.elt_type;
_tmp39E=(void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp34F)->f1).elt_type;
_tmp39F=_tmp39A.num_elts;_tmp3A0=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp34F)->f1).num_elts;_tmp3A1=_tmp39A.fat_result;_tmp3A2=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp34F)->f1).fat_result;_LL298: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp39C,_tmp39E,
_tmp3A0,_tmp39B,_tmp3A2);goto _LL24C;_LL24C:;}e->topt=({struct Cyc_Core_Opt*
_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA->v=(void*)t;_tmp3AA;});}
