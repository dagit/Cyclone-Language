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
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);extern
unsigned char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];int Cyc_List_mem(int(*
compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_Std_strcmp(struct
_tagged_arr s1,struct _tagged_arr s2);int Cyc_Std_strptrcmp(struct _tagged_arr*s1,
struct _tagged_arr*s2);int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);
struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
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
15];void*Cyc_Absyn_compress_kb(void*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Absyn_attribute2string(
void*);int Cyc_Absyn_fntype_att(void*a);struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,
int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,
void*elt);struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern unsigned char
Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];
extern unsigned char Cyc_Dict_Absent[11];int Cyc_Dict_is_empty(struct Cyc_Dict_Dict*
d);int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);
struct Cyc_Dict_Dict*Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d);struct _tuple3{void*f1;void*f2;};struct _tuple3*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple3*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_filter_c(
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Iter_Iter
Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d);struct Cyc_Tcenv_VarRes_struct{
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
ae;struct Cyc_Core_Opt*le;};struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv();struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_new_fenv(struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct
Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tagged_arr*,struct Cyc_List_List*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*);void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_is_function_type(
void*t);void Cyc_Tcutil_explain_failure();void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _tagged_arr*fn);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void
Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_supports_default(
void*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);void
Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Enumdecl*);extern unsigned char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*field;};struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);void*Cyc_Tcdecl_merge_binding(
void*d0,void*d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*f,int*res,struct _tagged_arr*v,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Tcgenrep_RepInfo;
struct Cyc_Dict_Dict*Cyc_Tcgenrep_empty_typerep_dict();struct _tuple4{struct Cyc_Dict_Dict*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple4 Cyc_Tcgenrep_tcGenrep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
void*type,struct Cyc_Dict_Dict*dict);static unsigned char _tmp0[1]="";static struct
_tagged_arr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};static struct _tagged_arr*Cyc_Tc_tc_msg=(
struct _tagged_arr*)& Cyc_Tc_tc_msg_c;static struct Cyc_List_List*Cyc_Tc_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmp1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmp2;struct Cyc_List_List*_tmp3;struct Cyc_List_List**_tmp4;_LL1: if(_tmp1 <= (void*)
3?1:*((int*)_tmp1)!= 8)goto _LL3;_tmp2=((struct Cyc_Absyn_FnType_struct*)_tmp1)->f1;
_tmp3=_tmp2.attributes;_tmp4=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp1)->f1).attributes;_LL2: {struct Cyc_List_List*_tmp5=0;for(0;atts != 0;atts=
atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))*_tmp4=({struct Cyc_List_List*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->hd=(void*)((void*)atts->hd);_tmp6->tl=*
_tmp4;_tmp6;});else{_tmp5=({struct Cyc_List_List*_tmp7=_cycalloc(sizeof(*_tmp7));
_tmp7->hd=(void*)((void*)atts->hd);_tmp7->tl=_tmp5;_tmp7;});}}return _tmp5;}_LL3:;
_LL4:({void*_tmp8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("transfer_fn_type_atts",sizeof(unsigned char),22),_tag_arr(_tmp8,
sizeof(void*),0));});_LL0:;}struct _tuple5{void*f1;int f2;};static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Vardecl*vd,int check_var_init){struct Cyc_Absyn_Vardecl _tmpA;void*
_tmpB;struct _tuple0*_tmpC;struct _tuple0 _tmpD;void*_tmpE;struct _tagged_arr*_tmpF;
void*_tmp10;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Vardecl*
_tmp9=vd;_tmpA=*_tmp9;_tmpB=(void*)_tmpA.sc;_tmpC=_tmpA.name;_tmpD=*_tmpC;_tmpE=
_tmpD.f1;_tmpF=_tmpD.f2;_tmp10=(void*)_tmpA.type;_tmp11=_tmpA.initializer;_tmp12=
_tmpA.attributes;{void*_tmp13=_tmpE;struct Cyc_List_List*_tmp14;struct Cyc_List_List*
_tmp15;_LL6: if(_tmp13 <= (void*)1?1:*((int*)_tmp13)!= 0)goto _LL8;_tmp14=((struct
Cyc_Absyn_Rel_n_struct*)_tmp13)->f1;if(_tmp14 != 0)goto _LL8;_LL7: goto _LL9;_LL8:
if(_tmp13 <= (void*)1?1:*((int*)_tmp13)!= 1)goto _LLA;_tmp15=((struct Cyc_Absyn_Abs_n_struct*)
_tmp13)->f1;if(_tmp15 != 0)goto _LLA;_LL9: goto _LL5;_LLA:;_LLB:({struct Cyc_Std_String_pa_struct
_tmp17;_tmp17.tag=0;_tmp17.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{
void*_tmp16[1]={& _tmp17};Cyc_Tcutil_terr(loc,_tag_arr("qualified variable declarations are not implemented (%s)",
sizeof(unsigned char),57),_tag_arr(_tmp16,sizeof(void*),1));}});return;_LL5:;}(*
vd->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp18=_cycalloc(sizeof(*
_tmp18));_tmp18[0]=({struct Cyc_Absyn_Abs_n_struct _tmp19;_tmp19.tag=1;_tmp19.f1=
te->ns;_tmp19;});_tmp18;});{void*_tmp1A=Cyc_Tcutil_compress(_tmp10);void*_tmp1B;
struct Cyc_Absyn_Tqual _tmp1C;struct Cyc_Absyn_Exp*_tmp1D;_LLD: if(_tmp1A <= (void*)3?
1:*((int*)_tmp1A)!= 7)goto _LLF;_tmp1B=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp1A)->f1;_tmp1C=((struct Cyc_Absyn_ArrayType_struct*)_tmp1A)->f2;_tmp1D=((
struct Cyc_Absyn_ArrayType_struct*)_tmp1A)->f3;if(_tmp1D != 0)goto _LLF;if(!(_tmp11
!= 0))goto _LLF;_LLE:{void*_tmp1E=(void*)_tmp11->r;void*_tmp1F;struct _tagged_arr
_tmp20;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_List_List*_tmp22;struct Cyc_List_List*
_tmp23;_LL12: if(*((int*)_tmp1E)!= 0)goto _LL14;_tmp1F=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1E)->f1;if(_tmp1F <= (void*)1?1:*((int*)_tmp1F)!= 5)goto _LL14;_tmp20=((struct
Cyc_Absyn_String_c_struct*)_tmp1F)->f1;_LL13: _tmp10=(void*)(vd->type=(void*)((
void*)({struct Cyc_Absyn_ArrayType_struct*_tmp24=_cycalloc(sizeof(*_tmp24));
_tmp24[0]=({struct Cyc_Absyn_ArrayType_struct _tmp25;_tmp25.tag=7;_tmp25.f1=(void*)
_tmp1B;_tmp25.f2=_tmp1C;_tmp25.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
_get_arr_size(_tmp20,sizeof(unsigned char)),0);_tmp25;});_tmp24;})));goto _LL11;
_LL14: if(*((int*)_tmp1E)!= 27)goto _LL16;_tmp21=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1E)->f2;_LL15: _tmp10=(void*)(vd->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp27;_tmp27.tag=7;_tmp27.f1=(void*)_tmp1B;_tmp27.f2=_tmp1C;_tmp27.f3=(struct
Cyc_Absyn_Exp*)_tmp21;_tmp27;});_tmp26;})));goto _LL11;_LL16: if(*((int*)_tmp1E)!= 
34)goto _LL18;_tmp22=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1E)->f2;_LL17:
_tmp23=_tmp22;goto _LL19;_LL18: if(*((int*)_tmp1E)!= 26)goto _LL1A;_tmp23=((struct
Cyc_Absyn_Array_e_struct*)_tmp1E)->f1;_LL19: _tmp10=(void*)(vd->type=(void*)((
void*)({struct Cyc_Absyn_ArrayType_struct*_tmp28=_cycalloc(sizeof(*_tmp28));
_tmp28[0]=({struct Cyc_Absyn_ArrayType_struct _tmp29;_tmp29.tag=7;_tmp29.f1=(void*)
_tmp1B;_tmp29.f2=_tmp1C;_tmp29.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp23),0);_tmp29;});
_tmp28;})));goto _LL11;_LL1A:;_LL1B: goto _LL11;_LL11:;}goto _LLC;_LLF:;_LL10: goto
_LLC;_LLC:;}Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp10);{void*_tmp2A=Cyc_Tcutil_compress(
_tmp10);_LL1D: if(_tmp2A <= (void*)3?1:*((int*)_tmp2A)!= 7)goto _LL1F;_LL1E: vd->escapes=
0;goto _LL1C;_LL1F:;_LL20: vd->escapes=1;goto _LL1C;_LL1C:;}if(Cyc_Tcutil_is_function_type(
_tmp10))_tmp12=Cyc_Tc_transfer_fn_type_atts(_tmp10,_tmp12);if(_tmpB == (void*)3?1:
_tmpB == (void*)4){if(_tmp11 != 0)({void*_tmp2B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("extern declaration should not have initializer",
sizeof(unsigned char),47),_tag_arr(_tmp2B,sizeof(void*),0));});}else{if(!Cyc_Tcutil_is_function_type(
_tmp10)){for(0;_tmp12 != 0;_tmp12=_tmp12->tl){void*_tmp2C=(void*)_tmp12->hd;_LL22:
if(_tmp2C <= (void*)16?1:*((int*)_tmp2C)!= 1)goto _LL24;_LL23: goto _LL25;_LL24: if(
_tmp2C <= (void*)16?1:*((int*)_tmp2C)!= 2)goto _LL26;_LL25: goto _LL27;_LL26: if((int)
_tmp2C != 6)goto _LL28;_LL27: goto _LL29;_LL28: if((int)_tmp2C != 7)goto _LL2A;_LL29:
goto _LL2B;_LL2A: if((int)_tmp2C != 8)goto _LL2C;_LL2B: goto _LL2D;_LL2C: if((int)
_tmp2C != 9)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if((int)_tmp2C != 10)goto _LL30;_LL2F:
goto _LL31;_LL30: if((int)_tmp2C != 11)goto _LL32;_LL31: continue;_LL32:;_LL33:({
struct Cyc_Std_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(vd->name);{struct Cyc_Std_String_pa_struct _tmp2E;_tmp2E.tag=
0;_tmp2E.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmp12->hd);{
void*_tmp2D[2]={& _tmp2E,& _tmp2F};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s for variable %s",
sizeof(unsigned char),33),_tag_arr(_tmp2D,sizeof(void*),2));}}});goto _LL21;_LL21:;}
if(_tmp11 == 0){if(check_var_init?!Cyc_Tcutil_supports_default(_tmp10): 0)({struct
Cyc_Std_String_pa_struct _tmp32;_tmp32.tag=0;_tmp32.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp10);{struct Cyc_Std_String_pa_struct _tmp31;_tmp31.tag=0;_tmp31.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{void*_tmp30[2]={& _tmp31,& _tmp32};
Cyc_Tcutil_terr(loc,_tag_arr("initializer required for variable %s of type %s",
sizeof(unsigned char),48),_tag_arr(_tmp30,sizeof(void*),2));}}});}else{struct Cyc_Absyn_Exp*
_tmp33=(struct Cyc_Absyn_Exp*)_check_null(_tmp11);void*_tmp34=Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp10,_tmp33);if(!Cyc_Tcutil_coerce_assign(te,_tmp33,_tmp10)){({
struct Cyc_Std_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(_tmp34);{struct Cyc_Std_String_pa_struct _tmp37;_tmp37.tag=
0;_tmp37.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp10);{struct Cyc_Std_String_pa_struct
_tmp36;_tmp36.tag=0;_tmp36.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{
void*_tmp35[3]={& _tmp36,& _tmp37,& _tmp38};Cyc_Tcutil_terr(loc,_tag_arr("%s declared with type \n%s\n but initialized with type \n%s",
sizeof(unsigned char),57),_tag_arr(_tmp35,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}
if(!Cyc_Tcutil_is_const_exp(te,_tmp33))({void*_tmp39[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("initializer is not a constant expression",sizeof(unsigned char),41),
_tag_arr(_tmp39,sizeof(void*),0));});}}else{for(0;_tmp12 != 0;_tmp12=_tmp12->tl){
void*_tmp3A=(void*)_tmp12->hd;_LL35: if(_tmp3A <= (void*)16?1:*((int*)_tmp3A)!= 0)
goto _LL37;_LL36: goto _LL38;_LL37: if((int)_tmp3A != 0)goto _LL39;_LL38: goto _LL3A;
_LL39: if((int)_tmp3A != 1)goto _LL3B;_LL3A: goto _LL3C;_LL3B: if((int)_tmp3A != 2)goto
_LL3D;_LL3C: goto _LL3E;_LL3D: if((int)_tmp3A != 3)goto _LL3F;_LL3E: goto _LL40;_LL3F:
if(_tmp3A <= (void*)16?1:*((int*)_tmp3A)!= 3)goto _LL41;_LL40: goto _LL42;_LL41: if((
int)_tmp3A != 4)goto _LL43;_LL42:({void*_tmp3B[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcVardecl: fn type atts in function var decl",
sizeof(unsigned char),45),_tag_arr(_tmp3B,sizeof(void*),0));});_LL43: if(_tmp3A <= (
void*)16?1:*((int*)_tmp3A)!= 1)goto _LL45;_LL44: goto _LL46;_LL45: if((int)_tmp3A != 
5)goto _LL47;_LL46:({struct Cyc_Std_String_pa_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)_tmp12->hd);{void*_tmp3C[1]={&
_tmp3D};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s in function declaration",
sizeof(unsigned char),41),_tag_arr(_tmp3C,sizeof(void*),1));}});goto _LL34;_LL47:;
_LL48: continue;_LL34:;}}}{struct _handler_cons _tmp3E;_push_handler(& _tmp3E);{int
_tmp40=0;if(setjmp(_tmp3E.handler))_tmp40=1;if(!_tmp40){{struct _tuple5*_tmp41=((
struct _tuple5*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,
_tmpF);void*_tmp42=(*_tmp41).f1;void*_tmp43;_LL4A: if(*((int*)_tmp42)!= 0)goto
_LL4C;_tmp43=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp42)->f1;_LL4B: {struct
Cyc_Absyn_Global_b_struct*_tmp44=({struct Cyc_Absyn_Global_b_struct*_tmp49=
_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_Global_b_struct _tmp4A;
_tmp4A.tag=0;_tmp4A.f1=vd;_tmp4A;});_tmp49;});void*_tmp45=Cyc_Tcdecl_merge_binding(
_tmp43,(void*)_tmp44,loc,Cyc_Tc_tc_msg);if(_tmp45 == (void*)0)goto _LL49;if(_tmp45
== _tmp43?(*_tmp41).f2: 0)goto _LL49;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,
_tmpF,({struct _tuple5*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->f1=(void*)({
struct Cyc_Tcenv_VarRes_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({
struct Cyc_Tcenv_VarRes_struct _tmp48;_tmp48.tag=0;_tmp48.f1=(void*)_tmp45;_tmp48;});
_tmp47;});_tmp46->f2=1;_tmp46;}));goto _LL49;}_LL4C: if(*((int*)_tmp42)!= 1)goto
_LL4E;_LL4D:({void*_tmp4B[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous struct declaration",
sizeof(unsigned char),57),_tag_arr(_tmp4B,sizeof(void*),0));});goto _LL49;_LL4E:
if(*((int*)_tmp42)!= 2)goto _LL50;_LL4F:({void*_tmp4C[0]={};Cyc_Tcutil_warn(loc,
_tag_arr("variable declaration shadows previous [x]tunion constructor",sizeof(
unsigned char),60),_tag_arr(_tmp4C,sizeof(void*),0));});goto _LL49;_LL50: if(*((
int*)_tmp42)!= 4)goto _LL52;_LL51: goto _LL53;_LL52: if(*((int*)_tmp42)!= 3)goto
_LL49;_LL53:({void*_tmp4D[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous enum tag",
sizeof(unsigned char),47),_tag_arr(_tmp4D,sizeof(void*),0));});goto _LL49;_LL49:;};
_pop_handler();}else{void*_tmp3F=(void*)_exn_thrown;void*_tmp4F=_tmp3F;_LL55: if(
_tmp4F != Cyc_Dict_Absent)goto _LL57;_LL56: ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,
_tmpF,({struct _tuple5*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->f1=(void*)({
struct Cyc_Tcenv_VarRes_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51[0]=({
struct Cyc_Tcenv_VarRes_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(void*)((void*)({
struct Cyc_Absyn_Global_b_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({
struct Cyc_Absyn_Global_b_struct _tmp54;_tmp54.tag=0;_tmp54.f1=vd;_tmp54;});_tmp53;}));
_tmp52;});_tmp51;});_tmp50->f2=0;_tmp50;}));goto _LL54;_LL57:;_LL58:(void)_throw(
_tmp4F);_LL54:;}}}}static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd){struct _tagged_arr*v=(*
fd->name).f2;if((void*)fd->sc == (void*)4)({void*_tmp55[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("extern \"C\" functions cannot be implemented in Cyclone",sizeof(
unsigned char),54),_tag_arr(_tmp55,sizeof(void*),0));});{void*_tmp56=(*fd->name).f1;
struct Cyc_List_List*_tmp57;struct Cyc_List_List*_tmp58;_LL5A: if(_tmp56 <= (void*)1?
1:*((int*)_tmp56)!= 0)goto _LL5C;_tmp57=((struct Cyc_Absyn_Rel_n_struct*)_tmp56)->f1;
if(_tmp57 != 0)goto _LL5C;_LL5B: goto _LL59;_LL5C: if(_tmp56 <= (void*)1?1:*((int*)
_tmp56)!= 1)goto _LL5E;_tmp58=((struct Cyc_Absyn_Abs_n_struct*)_tmp56)->f1;_LL5D:({
void*_tmp59[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("tc: Abs_n in tcFndecl",sizeof(unsigned char),22),_tag_arr(_tmp59,
sizeof(void*),0));});_LL5E:;_LL5F:({struct Cyc_Std_String_pa_struct _tmp5B;_tmp5B.tag=
0;_tmp5B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(fd->name);{void*_tmp5A[1]={&
_tmp5B};Cyc_Tcutil_terr(loc,_tag_arr("qualified function declarations are not implemented (%s)",
sizeof(unsigned char),57),_tag_arr(_tmp5A,sizeof(void*),1));}});return;_LL59:;}(*
fd->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp5C=_cycalloc(sizeof(*
_tmp5C));_tmp5C[0]=({struct Cyc_Absyn_Abs_n_struct _tmp5D;_tmp5D.tag=1;_tmp5D.f1=
te->ns;_tmp5D;});_tmp5C;});Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{void*t=
Cyc_Tcutil_fndecl2typ(fd);fd->attributes=Cyc_Tc_transfer_fn_type_atts(t,fd->attributes);{
struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){void*
_tmp5E=(void*)atts->hd;_LL61: if((int)_tmp5E != 5)goto _LL63;_LL62: goto _LL64;_LL63:
if(_tmp5E <= (void*)16?1:*((int*)_tmp5E)!= 1)goto _LL65;_LL64:({struct Cyc_Std_String_pa_struct
_tmp60;_tmp60.tag=0;_tmp60.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp5F[1]={& _tmp60};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s for function",
sizeof(unsigned char),30),_tag_arr(_tmp5F,sizeof(void*),1));}});goto _LL60;_LL65:;
_LL66: goto _LL60;_LL60:;}}{struct _handler_cons _tmp61;_push_handler(& _tmp61);{int
_tmp63=0;if(setjmp(_tmp61.handler))_tmp63=1;if(!_tmp63){{struct _tuple5*_tmp64=((
struct _tuple5*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,
v);void*_tmp65=(*_tmp64).f1;void*_tmp66;_LL68: if(*((int*)_tmp65)!= 0)goto _LL6A;
_tmp66=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp65)->f1;_LL69: {struct Cyc_Absyn_Funname_b_struct*
_tmp67=({struct Cyc_Absyn_Funname_b_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));
_tmp6C[0]=({struct Cyc_Absyn_Funname_b_struct _tmp6D;_tmp6D.tag=1;_tmp6D.f1=fd;
_tmp6D;});_tmp6C;});void*_tmp68=Cyc_Tcdecl_merge_binding(_tmp66,(void*)_tmp67,
loc,Cyc_Tc_tc_msg);if(_tmp68 == (void*)0)goto _LL67;if(_tmp68 == _tmp66?(*_tmp64).f2:
0)goto _LL67;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,v,({struct
_tuple5*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*
_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp6B;_tmp6B.tag=0;_tmp6B.f1=(void*)_tmp68;_tmp6B;});_tmp6A;});_tmp69->f2=1;
_tmp69;}));goto _LL67;}_LL6A: if(*((int*)_tmp65)!= 1)goto _LL6C;_LL6B:({void*_tmp6E[
0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous type declaration",
sizeof(unsigned char),55),_tag_arr(_tmp6E,sizeof(void*),0));});goto _LL67;_LL6C:
if(*((int*)_tmp65)!= 2)goto _LL6E;_LL6D:({void*_tmp6F[0]={};Cyc_Tcutil_warn(loc,
_tag_arr("function declaration shadows previous [x]tunion constructor",sizeof(
unsigned char),60),_tag_arr(_tmp6F,sizeof(void*),0));});goto _LL67;_LL6E: if(*((
int*)_tmp65)!= 4)goto _LL70;_LL6F: goto _LL71;_LL70: if(*((int*)_tmp65)!= 3)goto
_LL67;_LL71:({void*_tmp70[0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous enum tag",
sizeof(unsigned char),47),_tag_arr(_tmp70,sizeof(void*),0));});goto _LL67;_LL67:;};
_pop_handler();}else{void*_tmp62=(void*)_exn_thrown;void*_tmp72=_tmp62;_LL73: if(
_tmp72 != Cyc_Dict_Absent)goto _LL75;_LL74: ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,
v,({struct _tuple5*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->f1=(void*)({struct
Cyc_Tcenv_VarRes_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp75;_tmp75.tag=0;_tmp75.f1=(void*)((void*)({struct Cyc_Absyn_Funname_b_struct*
_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=({struct Cyc_Absyn_Funname_b_struct
_tmp77;_tmp77.tag=1;_tmp77.f1=fd;_tmp77;});_tmp76;}));_tmp75;});_tmp74;});_tmp73->f2=
0;_tmp73;}));goto _LL72;_LL75:;_LL76:(void)_throw(_tmp72);_LL72:;}}}te->le=({
struct Cyc_Core_Opt*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->v=(void*)((void*)({
struct Cyc_Tcenv_Outermost_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=({
struct Cyc_Tcenv_Outermost_struct _tmp7A;_tmp7A.tag=0;_tmp7A.f1=(void*)Cyc_Tcenv_new_fenv(
loc,fd);_tmp7A;});_tmp79;}));_tmp78;});Cyc_Tcstmt_tcStmt(te,fd->body,0);if(!Cyc_Tcenv_all_labels_resolved(
te))({void*_tmp7B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function has goto statements to undefined labels",
sizeof(unsigned char),49),_tag_arr(_tmp7B,sizeof(void*),0));});te->le=0;}}static
void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct
Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*td){struct _tagged_arr*v=(*
td->name).f2;{void*_tmp7C=(*td->name).f1;struct Cyc_List_List*_tmp7D;struct Cyc_List_List*
_tmp7E;_LL78: if(_tmp7C <= (void*)1?1:*((int*)_tmp7C)!= 0)goto _LL7A;_tmp7D=((
struct Cyc_Absyn_Rel_n_struct*)_tmp7C)->f1;if(_tmp7D != 0)goto _LL7A;_LL79: goto
_LL7B;_LL7A: if(_tmp7C <= (void*)1?1:*((int*)_tmp7C)!= 1)goto _LL7C;_tmp7E=((struct
Cyc_Absyn_Abs_n_struct*)_tmp7C)->f1;if(_tmp7E != 0)goto _LL7C;_LL7B: goto _LL77;
_LL7C:;_LL7D:({struct Cyc_Std_String_pa_struct _tmp80;_tmp80.tag=0;_tmp80.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(td->name);{void*_tmp7F[1]={& _tmp80};
Cyc_Tcutil_terr(loc,_tag_arr("qualified typedef declarations are not implemented (%s)",
sizeof(unsigned char),56),_tag_arr(_tmp7F,sizeof(void*),1));}});return;_LL77:;}
if(((int(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_member)(ge->typedefs,
v)){({struct Cyc_Std_String_pa_struct _tmp82;_tmp82.tag=0;_tmp82.f1=(struct
_tagged_arr)*v;{void*_tmp81[1]={& _tmp82};Cyc_Tcutil_terr(loc,_tag_arr("redeclaration of typedef %s",
sizeof(unsigned char),28),_tag_arr(_tmp81,sizeof(void*),1));}});return;}(*td->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({
struct Cyc_Absyn_Abs_n_struct _tmp84;_tmp84.tag=1;_tmp84.f1=te->ns;_tmp84;});
_tmp83;});Cyc_Tcutil_check_unique_tvars(loc,td->tvs);Cyc_Tcutil_add_tvar_identities(
td->tvs);Cyc_Tcutil_check_type(loc,te,td->tvs,(void*)0,0,(void*)td->defn);ge->typedefs=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Typedefdecl*
v))Cyc_Dict_insert)(ge->typedefs,v,td);}static void Cyc_Tc_tcAggrFields(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_List_List*
fields,struct Cyc_List_List*tvs){struct _RegionHandle _tmp85=_new_region("uprev_rgn");
struct _RegionHandle*uprev_rgn=& _tmp85;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;struct Cyc_List_List*_tmp86=fields;for(0;_tmp86 != 0;_tmp86=_tmp86->tl){
struct Cyc_Absyn_Aggrfield _tmp88;struct _tagged_arr*_tmp89;struct Cyc_Absyn_Tqual
_tmp8A;void*_tmp8B;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_List_List*_tmp8D;struct
Cyc_Absyn_Aggrfield*_tmp87=(struct Cyc_Absyn_Aggrfield*)_tmp86->hd;_tmp88=*_tmp87;
_tmp89=_tmp88.name;_tmp8A=_tmp88.tq;_tmp8B=(void*)_tmp88.type;_tmp8C=_tmp88.width;
_tmp8D=_tmp88.attributes;if(((int(*)(int(*compare)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,
prev_fields,_tmp89))({struct Cyc_Std_String_pa_struct _tmp8F;_tmp8F.tag=0;_tmp8F.f1=(
struct _tagged_arr)*_tmp89;{void*_tmp8E[1]={& _tmp8F};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s",
sizeof(unsigned char),19),_tag_arr(_tmp8E,sizeof(void*),1));}});if(Cyc_Std_strcmp(*
_tmp89,_tag_arr("",sizeof(unsigned char),1))!= 0)prev_fields=({struct Cyc_List_List*
_tmp90=_region_malloc(uprev_rgn,sizeof(*_tmp90));_tmp90->hd=_tmp89;_tmp90->tl=
prev_fields;_tmp90;});Cyc_Tcutil_check_type(loc,te,tvs,(void*)1,0,_tmp8B);Cyc_Tcutil_check_bitfield(
loc,te,_tmp8B,_tmp8C,_tmp89);}};_pop_region(uprev_rgn);}struct _tuple6{struct Cyc_Core_Opt*
f1;struct Cyc_Core_Opt*f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tagged_arr*_tmp91=(*ad->name).f2;{struct Cyc_List_List*atts=ad->attributes;
for(0;atts != 0;atts=atts->tl){void*_tmp92=(void*)atts->hd;_LL7F: if((int)_tmp92 != 
5)goto _LL81;_LL80: goto _LL82;_LL81: if(_tmp92 <= (void*)16?1:*((int*)_tmp92)!= 1)
goto _LL83;_LL82: continue;_LL83:;_LL84:({struct Cyc_Std_String_pa_struct _tmp95;
_tmp95.tag=0;_tmp95.f1=(struct _tagged_arr)*_tmp91;{struct Cyc_Std_String_pa_struct
_tmp94;_tmp94.tag=0;_tmp94.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp93[2]={& _tmp94,& _tmp95};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s in  %s definition",
sizeof(unsigned char),35),_tag_arr(_tmp93,sizeof(void*),2));}}});goto _LL7E;_LL7E:;}}{
struct Cyc_List_List*_tmp96=ad->tvs;{struct Cyc_List_List*tvs2=_tmp96;for(0;tvs2 != 
0;tvs2=tvs2->tl){void*_tmp97=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp98;struct Cyc_Core_Opt**_tmp99;struct Cyc_Core_Opt*
_tmp9A;struct Cyc_Core_Opt**_tmp9B;void*_tmp9C;struct Cyc_Core_Opt*_tmp9D;struct
Cyc_Core_Opt**_tmp9E;void*_tmp9F;void*_tmpA0;_LL86: if(*((int*)_tmp97)!= 1)goto
_LL88;_tmp98=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp97)->f1;_tmp99=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp97)->f1;_LL87: _tmp9B=_tmp99;goto _LL89;
_LL88: if(*((int*)_tmp97)!= 2)goto _LL8A;_tmp9A=((struct Cyc_Absyn_Less_kb_struct*)
_tmp97)->f1;_tmp9B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp97)->f1;_tmp9C=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp97)->f2;if((int)
_tmp9C != 1)goto _LL8A;_LL89: _tmp9E=_tmp9B;goto _LL8B;_LL8A: if(*((int*)_tmp97)!= 2)
goto _LL8C;_tmp9D=((struct Cyc_Absyn_Less_kb_struct*)_tmp97)->f1;_tmp9E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp97)->f1;_tmp9F=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp97)->f2;if((int)_tmp9F != 0)goto _LL8C;_LL8B:*_tmp9E=({struct Cyc_Core_Opt*
_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpA3;
_tmpA3.tag=0;_tmpA3.f1=(void*)((void*)2);_tmpA3;});_tmpA2;}));_tmpA1;});
continue;_LL8C: if(*((int*)_tmp97)!= 0)goto _LL8E;_tmpA0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp97)->f1;if((int)_tmpA0 != 1)goto _LL8E;_LL8D:({struct Cyc_Std_String_pa_struct
_tmpA6;_tmpA6.tag=0;_tmpA6.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name;{
struct Cyc_Std_String_pa_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct _tagged_arr)*
_tmp91;{void*_tmpA4[2]={& _tmpA5,& _tmpA6};Cyc_Tcutil_terr(loc,_tag_arr("type %s attempts to abstract type variable %s of kind M",
sizeof(unsigned char),56),_tag_arr(_tmpA4,sizeof(void*),2));}}});continue;_LL8E:;
_LL8F: continue;_LL85:;}}{void*_tmpA7=(*ad->name).f1;struct Cyc_List_List*_tmpA8;
struct Cyc_List_List*_tmpA9;_LL91: if(_tmpA7 <= (void*)1?1:*((int*)_tmpA7)!= 0)goto
_LL93;_tmpA8=((struct Cyc_Absyn_Rel_n_struct*)_tmpA7)->f1;if(_tmpA8 != 0)goto _LL93;
_LL92: goto _LL94;_LL93: if(_tmpA7 <= (void*)1?1:*((int*)_tmpA7)!= 1)goto _LL95;
_tmpA9=((struct Cyc_Absyn_Abs_n_struct*)_tmpA7)->f1;if(_tmpA9 != 0)goto _LL95;_LL94:
goto _LL90;_LL95:;_LL96:({struct Cyc_Std_String_pa_struct _tmpAB;_tmpAB.tag=0;
_tmpAB.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(ad->name);{void*_tmpAA[1]={&
_tmpAB};Cyc_Tcutil_terr(loc,_tag_arr("qualified struct declarations are not implemented (%s)",
sizeof(unsigned char),55),_tag_arr(_tmpAA,sizeof(void*),1));}});return;_LL90:;}(*
ad->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmpAC=_cycalloc(sizeof(*
_tmpAC));_tmpAC[0]=({struct Cyc_Absyn_Abs_n_struct _tmpAD;_tmpAD.tag=1;_tmpAD.f1=
te->ns;_tmpAD;});_tmpAC;});Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(
ad->tvs);{struct _tuple6 _tmpAF=({struct _tuple6 _tmpAE;_tmpAE.f1=ad->fields;_tmpAE.f2=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(
ge->aggrdecls,_tmp91);_tmpAE;});struct Cyc_Core_Opt*_tmpB0;struct Cyc_Core_Opt*
_tmpB1;struct Cyc_Core_Opt*_tmpB2;struct Cyc_Core_Opt _tmpB3;struct Cyc_List_List*
_tmpB4;struct Cyc_Core_Opt*_tmpB5;struct Cyc_Core_Opt*_tmpB6;struct Cyc_Core_Opt
_tmpB7;struct Cyc_List_List*_tmpB8;struct Cyc_Core_Opt*_tmpB9;struct Cyc_Core_Opt
_tmpBA;struct Cyc_Absyn_Aggrdecl**_tmpBB;struct Cyc_Core_Opt*_tmpBC;struct Cyc_Core_Opt*
_tmpBD;struct Cyc_Core_Opt _tmpBE;struct Cyc_Absyn_Aggrdecl**_tmpBF;_LL98: _tmpB0=
_tmpAF.f1;if(_tmpB0 != 0)goto _LL9A;_tmpB1=_tmpAF.f2;if(_tmpB1 != 0)goto _LL9A;_LL99:
ge->aggrdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp91,({struct Cyc_Absyn_Aggrdecl**
_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=ad;_tmpC0;}));goto _LL97;_LL9A: _tmpB2=
_tmpAF.f1;if(_tmpB2 == 0)goto _LL9C;_tmpB3=*_tmpB2;_tmpB4=(struct Cyc_List_List*)
_tmpB3.v;_tmpB5=_tmpAF.f2;if(_tmpB5 != 0)goto _LL9C;_LL9B: {struct Cyc_Absyn_Aggrdecl**
_tmpC1=({struct Cyc_Absyn_Aggrdecl**_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({
struct Cyc_Absyn_Aggrdecl*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->kind=(void*)((
void*)ad->kind);_tmpC7->sc=(void*)((void*)3);_tmpC7->name=ad->name;_tmpC7->tvs=
_tmp96;_tmpC7->exist_vars=0;_tmpC7->fields=0;_tmpC7->attributes=ad->attributes;
_tmpC7;});_tmpC6;});ge->aggrdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,
_tmp91,_tmpC1);Cyc_Tcutil_check_unique_tvars(loc,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(ad->exist_vars))->v);Cyc_Tcutil_add_tvar_identities((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ad->exist_vars))->v);Cyc_Tc_tcAggrFields(
te,ge,loc,_tmpB4,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp96,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ad->exist_vars))->v));if((void*)ad->kind == (void*)1){struct Cyc_List_List*
f=_tmpB4;for(0;f != 0;f=f->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
f->hd)->type))({struct Cyc_Std_String_pa_struct _tmpC5;_tmpC5.tag=0;_tmpC5.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Absyn_Aggrfield*)f->hd)->type);{
struct Cyc_Std_String_pa_struct _tmpC4;_tmpC4.tag=0;_tmpC4.f1=(struct _tagged_arr)*
_tmp91;{struct Cyc_Std_String_pa_struct _tmpC3;_tmpC3.tag=0;_tmpC3.f1=(struct
_tagged_arr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name;{void*_tmpC2[3]={& _tmpC3,&
_tmpC4,& _tmpC5};Cyc_Tcutil_terr(loc,_tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof(unsigned char),58),_tag_arr(_tmpC2,sizeof(void*),3));}}}});}}*_tmpC1=ad;
goto _LL97;}_LL9C: _tmpB6=_tmpAF.f1;if(_tmpB6 == 0)goto _LL9E;_tmpB7=*_tmpB6;_tmpB8=(
struct Cyc_List_List*)_tmpB7.v;_tmpB9=_tmpAF.f2;if(_tmpB9 == 0)goto _LL9E;_tmpBA=*
_tmpB9;_tmpBB=(struct Cyc_Absyn_Aggrdecl**)_tmpBA.v;_LL9D: if((void*)ad->kind != (
void*)(*_tmpBB)->kind)({void*_tmpC8[0]={};Cyc_Tcutil_terr(loc,_tag_arr("cannot reuse struct names for unions and vice-versa",
sizeof(unsigned char),52),_tag_arr(_tmpC8,sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*
_tmpC9=*_tmpBB;*_tmpBB=({struct Cyc_Absyn_Aggrdecl*_tmpCA=_cycalloc(sizeof(*
_tmpCA));_tmpCA->kind=(void*)((void*)ad->kind);_tmpCA->sc=(void*)((void*)3);
_tmpCA->name=ad->name;_tmpCA->tvs=_tmp96;_tmpCA->exist_vars=0;_tmpCA->fields=0;
_tmpCA->attributes=ad->attributes;_tmpCA;});Cyc_Tcutil_check_unique_tvars(loc,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ad->exist_vars))->v);Cyc_Tcutil_add_tvar_identities((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ad->exist_vars))->v);Cyc_Tc_tcAggrFields(
te,ge,loc,_tmpB8,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp96,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ad->exist_vars))->v));*_tmpBB=_tmpC9;_tmpBF=_tmpBB;goto _LL9F;}_LL9E:
_tmpBC=_tmpAF.f1;if(_tmpBC != 0)goto _LL97;_tmpBD=_tmpAF.f2;if(_tmpBD == 0)goto
_LL97;_tmpBE=*_tmpBD;_tmpBF=(struct Cyc_Absyn_Aggrdecl**)_tmpBE.v;_LL9F: {struct
Cyc_Absyn_Aggrdecl*_tmpCB=Cyc_Tcdecl_merge_aggrdecl(*_tmpBF,ad,loc,Cyc_Tc_tc_msg);
if(_tmpCB == 0)return;else{*_tmpBF=(struct Cyc_Absyn_Aggrdecl*)_check_null(_tmpCB);
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(_tmpCB);goto _LL97;}}_LL97:;}ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple5*
v))Cyc_Dict_insert)(ge->ordinaries,_tmp91,({struct _tuple5*_tmpCC=_cycalloc(
sizeof(*_tmpCC));_tmpCC->f1=(void*)({struct Cyc_Tcenv_AggrRes_struct*_tmpCD=
_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Tcenv_AggrRes_struct _tmpCE;
_tmpCE.tag=1;_tmpCE.f1=ad;_tmpCE;});_tmpCD;});_tmpCC->f2=1;_tmpCC;}));}}struct
_tuple7{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_Tc_tcTunionFields(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct _tagged_arr obj,int is_xtunion,struct _tuple0*name,struct Cyc_List_List*fields,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tuniondecl*tudres){{struct Cyc_List_List*
_tmpCF=fields;for(0;_tmpCF != 0;_tmpCF=_tmpCF->tl){struct Cyc_Absyn_Tunionfield*
_tmpD0=(struct Cyc_Absyn_Tunionfield*)_tmpCF->hd;{struct Cyc_List_List*typs=_tmpD0->typs;
for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmpD0->loc,te,tvs,(void*)1,
0,(*((struct _tuple7*)typs->hd)).f2);}}{void*_tmpD1=(*_tmpD0->name).f1;struct Cyc_List_List*
_tmpD2;_LLA1: if(_tmpD1 <= (void*)1?1:*((int*)_tmpD1)!= 0)goto _LLA3;_tmpD2=((
struct Cyc_Absyn_Rel_n_struct*)_tmpD1)->f1;if(_tmpD2 != 0)goto _LLA3;_LLA2: if(
is_xtunion)(*_tmpD0->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmpD3=
_cycalloc(sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_Abs_n_struct _tmpD4;_tmpD4.tag=
1;_tmpD4.f1=te->ns;_tmpD4;});_tmpD3;});else{(*_tmpD0->name).f1=(*name).f1;}goto
_LLA0;_LLA3: if(_tmpD1 <= (void*)1?1:*((int*)_tmpD1)!= 0)goto _LLA5;_LLA4:({struct
Cyc_Std_String_pa_struct _tmpD6;_tmpD6.tag=0;_tmpD6.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmpD0->name);{void*_tmpD5[1]={& _tmpD6};Cyc_Tcutil_terr(_tmpD0->loc,_tag_arr("qualified tunionfield declarations are not allowed (%s)",
sizeof(unsigned char),56),_tag_arr(_tmpD5,sizeof(void*),1));}});goto _LLA0;_LLA5:
if(_tmpD1 <= (void*)1?1:*((int*)_tmpD1)!= 1)goto _LLA7;_LLA6: goto _LLA0;_LLA7: if((
int)_tmpD1 != 0)goto _LLA0;_LLA8:({void*_tmpD7[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcTunionFields: Loc_n",sizeof(
unsigned char),22),_tag_arr(_tmpD7,sizeof(void*),0));});_LLA0:;}}}{struct Cyc_List_List*
fields2;if(is_xtunion){int _tmpD8=1;struct Cyc_List_List*_tmpD9=Cyc_Tcdecl_sort_xtunion_fields(
fields,& _tmpD8,(*name).f2,loc,Cyc_Tc_tc_msg);if(_tmpD8)fields2=_tmpD9;else{
fields2=0;}}else{struct _RegionHandle _tmpDA=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmpDA;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Tunionfield*
_tmpDB=(struct Cyc_Absyn_Tunionfield*)fs->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmpDB->name).f2))({struct Cyc_Std_String_pa_struct
_tmpDE;_tmpDE.tag=0;_tmpDE.f1=(struct _tagged_arr)obj;{struct Cyc_Std_String_pa_struct
_tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct _tagged_arr)*(*_tmpDB->name).f2;{void*
_tmpDC[2]={& _tmpDD,& _tmpDE};Cyc_Tcutil_terr(_tmpDB->loc,_tag_arr("duplicate field name %s in %s",
sizeof(unsigned char),30),_tag_arr(_tmpDC,sizeof(void*),2));}}});else{
prev_fields=({struct Cyc_List_List*_tmpDF=_region_malloc(uprev_rgn,sizeof(*_tmpDF));
_tmpDF->hd=(*_tmpDB->name).f2;_tmpDF->tl=prev_fields;_tmpDF;});}if((void*)_tmpDB->sc
!= (void*)2){({struct Cyc_Std_String_pa_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(
struct _tagged_arr)*(*_tmpDB->name).f2;{void*_tmpE0[1]={& _tmpE1};Cyc_Tcutil_warn(
loc,_tag_arr("ignoring scope of field %s",sizeof(unsigned char),27),_tag_arr(
_tmpE0,sizeof(void*),1));}});(void*)(_tmpDB->sc=(void*)((void*)2));}}}fields2=
fields;};_pop_region(uprev_rgn);}{struct Cyc_List_List*_tmpE2=fields;for(0;_tmpE2
!= 0;_tmpE2=_tmpE2->tl){struct Cyc_Absyn_Tunionfield*_tmpE3=(struct Cyc_Absyn_Tunionfield*)
_tmpE2->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmpE3->name).f2,({
struct _tuple5*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*
_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Tcenv_TunionRes_struct
_tmpE6;_tmpE6.tag=2;_tmpE6.f1=tudres;_tmpE6.f2=_tmpE3;_tmpE6;});_tmpE5;});_tmpE4->f2=
1;_tmpE4;}));}}return fields2;}}void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl*
tud){struct _tagged_arr*v=(*tud->name).f2;struct _tagged_arr obj=tud->is_xtunion?
_tag_arr("xtunion",sizeof(unsigned char),8): _tag_arr("tunion",sizeof(
unsigned char),7);struct Cyc_List_List*tvs=tud->tvs;{struct Cyc_List_List*tvs2=tvs;
for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmpE7=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmpE8;struct Cyc_Core_Opt**
_tmpE9;struct Cyc_Core_Opt*_tmpEA;struct Cyc_Core_Opt**_tmpEB;void*_tmpEC;struct
Cyc_Core_Opt*_tmpED;struct Cyc_Core_Opt**_tmpEE;void*_tmpEF;void*_tmpF0;_LLAA: if(*((
int*)_tmpE7)!= 1)goto _LLAC;_tmpE8=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpE7)->f1;
_tmpE9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmpE7)->f1;
_LLAB: _tmpEB=_tmpE9;goto _LLAD;_LLAC: if(*((int*)_tmpE7)!= 2)goto _LLAE;_tmpEA=((
struct Cyc_Absyn_Less_kb_struct*)_tmpE7)->f1;_tmpEB=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpE7)->f1;_tmpEC=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpE7)->f2;if((int)_tmpEC != 1)goto _LLAE;_LLAD: _tmpEE=_tmpEB;goto _LLAF;_LLAE: if(*((
int*)_tmpE7)!= 2)goto _LLB0;_tmpED=((struct Cyc_Absyn_Less_kb_struct*)_tmpE7)->f1;
_tmpEE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpE7)->f1;
_tmpEF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpE7)->f2;if((int)_tmpEF != 0)
goto _LLB0;_LLAF:*_tmpEE=({struct Cyc_Core_Opt*_tmpF1=_cycalloc(sizeof(*_tmpF1));
_tmpF1->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpF2=_cycalloc(sizeof(*
_tmpF2));_tmpF2[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=(
void*)((void*)2);_tmpF3;});_tmpF2;}));_tmpF1;});goto _LLA9;_LLB0: if(*((int*)
_tmpE7)!= 0)goto _LLB2;_tmpF0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpE7)->f1;
if((int)_tmpF0 != 1)goto _LLB2;_LLB1:({struct Cyc_Std_String_pa_struct _tmpF7;_tmpF7.tag=
0;_tmpF7.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name;{struct
Cyc_Std_String_pa_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _tagged_arr)*v;{
struct Cyc_Std_String_pa_struct _tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _tagged_arr)
obj;{void*_tmpF4[3]={& _tmpF5,& _tmpF6,& _tmpF7};Cyc_Tcutil_terr(loc,_tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof(unsigned char),54),_tag_arr(_tmpF4,sizeof(void*),3));}}}});goto _LLA9;
_LLB2:;_LLB3: goto _LLA9;_LLA9:;}}Cyc_Tcutil_check_unique_tvars(loc,tvs);Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_Core_Opt*tud_opt;if(tud->is_xtunion){{struct _handler_cons _tmpF8;
_push_handler(& _tmpF8);{int _tmpFA=0;if(setjmp(_tmpF8.handler))_tmpFA=1;if(!
_tmpFA){tud_opt=Cyc_Tcenv_lookup_xtuniondecl(te,loc,tud->name);;_pop_handler();}
else{void*_tmpF9=(void*)_exn_thrown;void*_tmpFC=_tmpF9;_LLB5: if(_tmpFC != Cyc_Dict_Absent)
goto _LLB7;_LLB6:({struct Cyc_Std_String_pa_struct _tmpFE;_tmpFE.tag=0;_tmpFE.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{void*_tmpFD[1]={& _tmpFE};
Cyc_Tcutil_terr(loc,_tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof(unsigned char),60),_tag_arr(_tmpFD,sizeof(void*),1));}});return;_LLB7:;
_LLB8:(void)_throw(_tmpFC);_LLB4:;}}}if(tud_opt != 0)tud->name=(*((struct Cyc_Absyn_Tuniondecl**)
tud_opt->v))->name;else{(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp100;_tmp100.tag=1;_tmp100.f1=te->ns;_tmp100;});_tmpFF;});}}else{{void*
_tmp101=(*tud->name).f1;struct Cyc_List_List*_tmp102;_LLBA: if(_tmp101 <= (void*)1?
1:*((int*)_tmp101)!= 0)goto _LLBC;_tmp102=((struct Cyc_Absyn_Rel_n_struct*)_tmp101)->f1;
if(_tmp102 != 0)goto _LLBC;_LLBB:(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp104;_tmp104.tag=1;_tmp104.f1=te->ns;_tmp104;});_tmp103;});goto _LLB9;_LLBC:
if(_tmp101 <= (void*)1?1:*((int*)_tmp101)!= 1)goto _LLBE;_LLBD: goto _LLBF;_LLBE:;
_LLBF:({struct Cyc_Std_String_pa_struct _tmp106;_tmp106.tag=0;_tmp106.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{void*_tmp105[1]={& _tmp106};Cyc_Tcutil_terr(
loc,_tag_arr("qualified tunion declarations are not implemented (%s)",sizeof(
unsigned char),55),_tag_arr(_tmp105,sizeof(void*),1));}});return;_LLB9:;}tud_opt=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(
ge->tuniondecls,v);}{struct _tuple6 _tmp108=({struct _tuple6 _tmp107;_tmp107.f1=tud->fields;
_tmp107.f2=tud_opt;_tmp107;});struct Cyc_Core_Opt*_tmp109;struct Cyc_Core_Opt*
_tmp10A;struct Cyc_Core_Opt*_tmp10B;struct Cyc_Core_Opt _tmp10C;struct Cyc_List_List*
_tmp10D;struct Cyc_List_List**_tmp10E;struct Cyc_Core_Opt*_tmp10F;struct Cyc_Core_Opt*
_tmp110;struct Cyc_Core_Opt _tmp111;struct Cyc_List_List*_tmp112;struct Cyc_List_List**
_tmp113;struct Cyc_Core_Opt*_tmp114;struct Cyc_Core_Opt _tmp115;struct Cyc_Absyn_Tuniondecl**
_tmp116;struct Cyc_Core_Opt*_tmp117;struct Cyc_Core_Opt*_tmp118;struct Cyc_Core_Opt
_tmp119;struct Cyc_Absyn_Tuniondecl**_tmp11A;_LLC1: _tmp109=_tmp108.f1;if(_tmp109
!= 0)goto _LLC3;_tmp10A=_tmp108.f2;if(_tmp10A != 0)goto _LLC3;_LLC2: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**
v))Cyc_Dict_insert)(ge->tuniondecls,v,({struct Cyc_Absyn_Tuniondecl**_tmp11B=
_cycalloc(sizeof(*_tmp11B));_tmp11B[0]=tud;_tmp11B;}));goto _LLC0;_LLC3: _tmp10B=
_tmp108.f1;if(_tmp10B == 0)goto _LLC5;_tmp10C=*_tmp10B;_tmp10D=(struct Cyc_List_List*)
_tmp10C.v;_tmp10E=(struct Cyc_List_List**)&(*_tmp108.f1).v;_tmp10F=_tmp108.f2;if(
_tmp10F != 0)goto _LLC5;_LLC4: {struct Cyc_Absyn_Tuniondecl**_tmp11C=({struct Cyc_Absyn_Tuniondecl**
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({struct Cyc_Absyn_Tuniondecl*
_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->sc=(void*)((void*)3);_tmp11E->name=
tud->name;_tmp11E->tvs=tvs;_tmp11E->fields=0;_tmp11E->is_xtunion=tud->is_xtunion;
_tmp11E;});_tmp11D;});ge->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(ge->tuniondecls,
v,_tmp11C);*_tmp10E=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,tud->name,*
_tmp10E,tvs,tud);*_tmp11C=tud;goto _LLC0;}_LLC5: _tmp110=_tmp108.f1;if(_tmp110 == 0)
goto _LLC7;_tmp111=*_tmp110;_tmp112=(struct Cyc_List_List*)_tmp111.v;_tmp113=(
struct Cyc_List_List**)&(*_tmp108.f1).v;_tmp114=_tmp108.f2;if(_tmp114 == 0)goto
_LLC7;_tmp115=*_tmp114;_tmp116=(struct Cyc_Absyn_Tuniondecl**)_tmp115.v;_LLC6: {
struct Cyc_Absyn_Tuniondecl*_tmp11F=*_tmp116;*_tmp116=({struct Cyc_Absyn_Tuniondecl*
_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->sc=(void*)((void*)3);_tmp120->name=
tud->name;_tmp120->tvs=tvs;_tmp120->fields=0;_tmp120->is_xtunion=tud->is_xtunion;
_tmp120;});*_tmp113=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,tud->name,*
_tmp113,tvs,tud);*_tmp116=_tmp11F;_tmp11A=_tmp116;goto _LLC8;}_LLC7: _tmp117=
_tmp108.f1;if(_tmp117 != 0)goto _LLC0;_tmp118=_tmp108.f2;if(_tmp118 == 0)goto _LLC0;
_tmp119=*_tmp118;_tmp11A=(struct Cyc_Absyn_Tuniondecl**)_tmp119.v;_LLC8: {struct
Cyc_Absyn_Tuniondecl*_tmp121=Cyc_Tcdecl_merge_tuniondecl(*_tmp11A,tud,loc,Cyc_Tc_tc_msg);
if(_tmp121 == 0)return;else{*_tmp11A=(struct Cyc_Absyn_Tuniondecl*)_check_null(
_tmp121);goto _LLC0;}}_LLC0:;}}}void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl*
ed){struct _tagged_arr*v=(*ed->name).f2;{void*_tmp122=(*ed->name).f1;struct Cyc_List_List*
_tmp123;struct Cyc_List_List*_tmp124;_LLCA: if(_tmp122 <= (void*)1?1:*((int*)
_tmp122)!= 0)goto _LLCC;_tmp123=((struct Cyc_Absyn_Rel_n_struct*)_tmp122)->f1;if(
_tmp123 != 0)goto _LLCC;_LLCB: goto _LLCD;_LLCC: if(_tmp122 <= (void*)1?1:*((int*)
_tmp122)!= 1)goto _LLCE;_tmp124=((struct Cyc_Absyn_Abs_n_struct*)_tmp122)->f1;if(
_tmp124 != 0)goto _LLCE;_LLCD: goto _LLC9;_LLCE:;_LLCF:({struct Cyc_Std_String_pa_struct
_tmp126;_tmp126.tag=0;_tmp126.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(ed->name);{
void*_tmp125[1]={& _tmp126};Cyc_Tcutil_terr(loc,_tag_arr("qualified enum declarations are not implemented (%s)",
sizeof(unsigned char),53),_tag_arr(_tmp125,sizeof(void*),1));}});return;_LLC9:;}(*
ed->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp127=_cycalloc(sizeof(*
_tmp127));_tmp127[0]=({struct Cyc_Absyn_Abs_n_struct _tmp128;_tmp128.tag=1;_tmp128.f1=
te->ns;_tmp128;});_tmp127;});if(ed->fields != 0){struct _RegionHandle _tmp129=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp129;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=0;struct Cyc_List_List*
fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;
fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*_tmp12A=(struct Cyc_Absyn_Enumfield*)
fs->hd;if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,
prev_fields,(*_tmp12A->name).f2))({struct Cyc_Std_String_pa_struct _tmp12C;_tmp12C.tag=
0;_tmp12C.f1=(struct _tagged_arr)*(*_tmp12A->name).f2;{void*_tmp12B[1]={& _tmp12C};
Cyc_Tcutil_terr(_tmp12A->loc,_tag_arr("duplicate field name %s",sizeof(
unsigned char),24),_tag_arr(_tmp12B,sizeof(void*),1));}});else{prev_fields=({
struct Cyc_List_List*_tmp12D=_region_malloc(uprev_rgn,sizeof(*_tmp12D));_tmp12D->hd=(*
_tmp12A->name).f2;_tmp12D->tl=prev_fields;_tmp12D;});}if(_tmp12A->tag == 0)
_tmp12A->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp12A->loc);
else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp12A->tag)))({
struct Cyc_Std_String_pa_struct _tmp130;_tmp130.tag=0;_tmp130.f1=(struct
_tagged_arr)*(*_tmp12A->name).f2;{struct Cyc_Std_String_pa_struct _tmp12F;_tmp12F.tag=
0;_tmp12F.f1=(struct _tagged_arr)*v;{void*_tmp12E[2]={& _tmp12F,& _tmp130};Cyc_Tcutil_terr(
loc,_tag_arr("enum %s, field %s: expression is not constant",sizeof(
unsigned char),46),_tag_arr(_tmp12E,sizeof(void*),2));}}});}{unsigned int t1=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp12A->tag));tag_count=t1 + 1;(*_tmp12A->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp132;_tmp132.tag=1;_tmp132.f1=te->ns;_tmp132;});
_tmp131;});}}};_pop_region(uprev_rgn);}{struct _handler_cons _tmp133;_push_handler(&
_tmp133);{int _tmp135=0;if(setjmp(_tmp133.handler))_tmp135=1;if(!_tmp135){{struct
Cyc_Absyn_Enumdecl**_tmp136=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*
_tmp137=Cyc_Tcdecl_merge_enumdecl(*_tmp136,ed,loc,Cyc_Tc_tc_msg);if(_tmp137 == 0){
_npop_handler(0);return;}*_tmp136=(struct Cyc_Absyn_Enumdecl*)_check_null(_tmp137);};
_pop_handler();}else{void*_tmp134=(void*)_exn_thrown;void*_tmp139=_tmp134;_LLD1:
if(_tmp139 != Cyc_Dict_Absent)goto _LLD3;_LLD2: {struct Cyc_Absyn_Enumdecl**_tmp13A=({
struct Cyc_Absyn_Enumdecl**_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B[0]=ed;
_tmp13B;});ge->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,
_tmp13A);goto _LLD0;}_LLD3:;_LLD4:(void)_throw(_tmp139);_LLD0:;}}}if(ed->fields != 
0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp13C=(struct Cyc_Absyn_Enumfield*)fs->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp13C->name).f2,({struct _tuple5*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->f1=(
void*)({struct Cyc_Tcenv_EnumRes_struct*_tmp13E=_cycalloc(sizeof(*_tmp13E));
_tmp13E[0]=({struct Cyc_Tcenv_EnumRes_struct _tmp13F;_tmp13F.tag=3;_tmp13F.f1=ed;
_tmp13F.f2=_tmp13C;_tmp13F;});_tmp13E;});_tmp13D->f2=1;_tmp13D;}));}}}static int
Cyc_Tc_okay_externC(struct Cyc_Position_Segment*loc,void*sc){void*_tmp140=sc;
_LLD6: if((int)_tmp140 != 0)goto _LLD8;_LLD7:({void*_tmp141[0]={};Cyc_Tcutil_warn(
loc,_tag_arr("static declaration nested within extern \"C\"",sizeof(
unsigned char),44),_tag_arr(_tmp141,sizeof(void*),0));});return 0;_LLD8: if((int)
_tmp140 != 1)goto _LLDA;_LLD9:({void*_tmp142[0]={};Cyc_Tcutil_warn(loc,_tag_arr("abstract declaration nested within extern \"C\"",
sizeof(unsigned char),46),_tag_arr(_tmp142,sizeof(void*),0));});return 0;_LLDA:
if((int)_tmp140 != 2)goto _LLDC;_LLDB: goto _LLDD;_LLDC: if((int)_tmp140 != 3)goto
_LLDE;_LLDD: return 1;_LLDE: if((int)_tmp140 != 4)goto _LLD5;_LLDF:({void*_tmp143[0]={};
Cyc_Tcutil_warn(loc,_tag_arr("nested extern \"C\" declaration",sizeof(
unsigned char),30),_tag_arr(_tmp143,sizeof(void*),0));});return 1;_LLD5:;}static
void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int
in_externC,int check_var_init){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;struct Cyc_Dict_Dict*dict=Cyc_Tcgenrep_empty_typerep_dict();
struct Cyc_List_List*_tmp144=ds0;for(0;_tmp144 != 0;(last=_tmp144,_tmp144=_tmp144->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp144->hd;struct Cyc_Position_Segment*
loc=d->loc;void*_tmp145=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp146;struct Cyc_Absyn_Fndecl*
_tmp147;struct Cyc_Absyn_Typedefdecl*_tmp148;struct Cyc_Absyn_Aggrdecl*_tmp149;
struct Cyc_Absyn_Tuniondecl*_tmp14A;struct Cyc_Absyn_Enumdecl*_tmp14B;struct
_tagged_arr*_tmp14C;struct Cyc_List_List*_tmp14D;struct _tuple0*_tmp14E;struct
_tuple0 _tmp14F;void*_tmp150;struct _tagged_arr*_tmp151;struct Cyc_List_List*
_tmp152;struct Cyc_List_List*_tmp153;_LLE1: if(*((int*)_tmp145)!= 2)goto _LLE3;
_LLE2: goto _LLE4;_LLE3: if(*((int*)_tmp145)!= 3)goto _LLE5;_LLE4:({void*_tmp154[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("top level let-declarations are not implemented",
sizeof(unsigned char),47),_tag_arr(_tmp154,sizeof(void*),0));});goto _LLE0;_LLE5:
if(*((int*)_tmp145)!= 0)goto _LLE7;_tmp146=((struct Cyc_Absyn_Var_d_struct*)
_tmp145)->f1;_LLE6: if(in_externC?Cyc_Tc_okay_externC(d->loc,(void*)_tmp146->sc):
0)(void*)(_tmp146->sc=(void*)((void*)4));if(_tmp146->initializer != 0){void*
_tmp155=(void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp146->initializer))->r;void*
_tmp156;_LLF8: if(*((int*)_tmp155)!= 19)goto _LLFA;_tmp156=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp155)->f2;_LLF9: {struct Cyc_Dict_Dict*_tmp158;struct Cyc_List_List*_tmp159;
struct Cyc_Absyn_Exp*_tmp15A;struct _tuple4 _tmp157=Cyc_Tcgenrep_tcGenrep(te,ge,loc,
_tmp156,dict);_tmp158=_tmp157.f1;_tmp159=_tmp157.f2;_tmp15A=_tmp157.f3;dict=
_tmp158;Cyc_Tc_tc_decls(te,_tmp159,in_externC,check_var_init);_tmp146->initializer=(
struct Cyc_Absyn_Exp*)_tmp15A;Cyc_Tc_tcVardecl(te,ge,loc,_tmp146,check_var_init);
if(_tmp159 != 0){if(last != 0){((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp159,_tmp144);((struct Cyc_List_List*)
_check_null(last))->tl=_tmp159;}else{struct Cyc_List_List tmp=({struct Cyc_List_List
_tmp15B;_tmp15B.hd=(struct Cyc_Absyn_Decl*)_tmp144->hd;_tmp15B.tl=_tmp144->tl;
_tmp15B;});(struct Cyc_Absyn_Decl*)(_tmp144->hd=(void*)((struct Cyc_Absyn_Decl*)
_tmp159->hd));_tmp144->tl=_tmp159->tl;(struct Cyc_Absyn_Decl*)(_tmp159->hd=(void*)((
struct Cyc_Absyn_Decl*)tmp.hd));_tmp159->tl=tmp.tl;((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp144,
_tmp159);}}continue;}_LLFA:;_LLFB: goto _LLF7;_LLF7:;}Cyc_Tc_tcVardecl(te,ge,loc,
_tmp146,check_var_init);goto _LLE0;_LLE7: if(*((int*)_tmp145)!= 1)goto _LLE9;
_tmp147=((struct Cyc_Absyn_Fn_d_struct*)_tmp145)->f1;_LLE8: if(in_externC?Cyc_Tc_okay_externC(
d->loc,(void*)_tmp147->sc): 0)(void*)(_tmp147->sc=(void*)((void*)4));Cyc_Tc_tcFndecl(
te,ge,loc,_tmp147);goto _LLE0;_LLE9: if(*((int*)_tmp145)!= 7)goto _LLEB;_tmp148=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp145)->f1;_LLEA: Cyc_Tc_tcTypedefdecl(te,ge,
loc,_tmp148);goto _LLE0;_LLEB: if(*((int*)_tmp145)!= 4)goto _LLED;_tmp149=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp145)->f1;_LLEC: if(in_externC?Cyc_Tc_okay_externC(d->loc,(
void*)_tmp149->sc): 0)(void*)(_tmp149->sc=(void*)((void*)4));Cyc_Tc_tcAggrdecl(te,
ge,loc,_tmp149);goto _LLE0;_LLED: if(*((int*)_tmp145)!= 5)goto _LLEF;_tmp14A=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp145)->f1;_LLEE: if(in_externC?Cyc_Tc_okay_externC(
d->loc,(void*)_tmp14A->sc): 0)(void*)(_tmp14A->sc=(void*)((void*)4));Cyc_Tc_tcTuniondecl(
te,ge,loc,_tmp14A);goto _LLE0;_LLEF: if(*((int*)_tmp145)!= 6)goto _LLF1;_tmp14B=((
struct Cyc_Absyn_Enum_d_struct*)_tmp145)->f1;_LLF0: if(in_externC?Cyc_Tc_okay_externC(
d->loc,(void*)_tmp14B->sc): 0)(void*)(_tmp14B->sc=(void*)((void*)4));Cyc_Tc_tcEnumdecl(
te,ge,loc,_tmp14B);goto _LLE0;_LLF1: if(*((int*)_tmp145)!= 8)goto _LLF3;_tmp14C=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp145)->f1;_tmp14D=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp145)->f2;_LLF2: {struct Cyc_List_List*_tmp15C=te->ns;struct Cyc_List_List*
_tmp15D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp15C,({struct Cyc_List_List*_tmp15E=_cycalloc(sizeof(*_tmp15E));
_tmp15E->hd=_tmp14C;_tmp15E->tl=0;_tmp15E;}));if(!((int(*)(struct Cyc_Set_Set*s,
struct _tagged_arr*elt))Cyc_Set_member)(ge->namespaces,_tmp14C)){ge->namespaces=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)(
ge->namespaces,_tmp14C);te->ae=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp15D,
Cyc_Tcenv_empty_genv());}te->ns=_tmp15D;Cyc_Tc_tc_decls(te,_tmp14D,in_externC,
check_var_init);te->ns=_tmp15C;goto _LLE0;}_LLF3: if(*((int*)_tmp145)!= 9)goto
_LLF5;_tmp14E=((struct Cyc_Absyn_Using_d_struct*)_tmp145)->f1;_tmp14F=*_tmp14E;
_tmp150=_tmp14F.f1;_tmp151=_tmp14F.f2;_tmp152=((struct Cyc_Absyn_Using_d_struct*)
_tmp145)->f2;_LLF4: {struct _tagged_arr*first;struct Cyc_List_List*rest;{void*
_tmp15F=_tmp150;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp161;struct
Cyc_List_List*_tmp162;struct Cyc_List_List _tmp163;struct _tagged_arr*_tmp164;
struct Cyc_List_List*_tmp165;struct Cyc_List_List*_tmp166;struct Cyc_List_List
_tmp167;struct _tagged_arr*_tmp168;struct Cyc_List_List*_tmp169;_LLFD: if((int)
_tmp15F != 0)goto _LLFF;_LLFE: goto _LL100;_LLFF: if(_tmp15F <= (void*)1?1:*((int*)
_tmp15F)!= 0)goto _LL101;_tmp160=((struct Cyc_Absyn_Rel_n_struct*)_tmp15F)->f1;if(
_tmp160 != 0)goto _LL101;_LL100: goto _LL102;_LL101: if(_tmp15F <= (void*)1?1:*((int*)
_tmp15F)!= 1)goto _LL103;_tmp161=((struct Cyc_Absyn_Abs_n_struct*)_tmp15F)->f1;if(
_tmp161 != 0)goto _LL103;_LL102: first=_tmp151;rest=0;goto _LLFC;_LL103: if(_tmp15F <= (
void*)1?1:*((int*)_tmp15F)!= 0)goto _LL105;_tmp162=((struct Cyc_Absyn_Rel_n_struct*)
_tmp15F)->f1;if(_tmp162 == 0)goto _LL105;_tmp163=*_tmp162;_tmp164=(struct
_tagged_arr*)_tmp163.hd;_tmp165=_tmp163.tl;_LL104: _tmp168=_tmp164;_tmp169=
_tmp165;goto _LL106;_LL105: if(_tmp15F <= (void*)1?1:*((int*)_tmp15F)!= 1)goto _LLFC;
_tmp166=((struct Cyc_Absyn_Abs_n_struct*)_tmp15F)->f1;if(_tmp166 == 0)goto _LLFC;
_tmp167=*_tmp166;_tmp168=(struct _tagged_arr*)_tmp167.hd;_tmp169=_tmp167.tl;
_LL106: first=_tmp168;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp169,({struct Cyc_List_List*_tmp16A=
_cycalloc(sizeof(*_tmp16A));_tmp16A->hd=_tmp151;_tmp16A->tl=0;_tmp16A;}));goto
_LLFC;_LLFC:;}{struct Cyc_List_List*_tmp16B=Cyc_Tcenv_resolve_namespace(te,loc,
first,rest);ge->availables=({struct Cyc_List_List*_tmp16C=_cycalloc(sizeof(*
_tmp16C));_tmp16C->hd=_tmp16B;_tmp16C->tl=ge->availables;_tmp16C;});Cyc_Tc_tc_decls(
te,_tmp152,in_externC,check_var_init);ge->availables=((struct Cyc_List_List*)
_check_null(ge->availables))->tl;goto _LLE0;}}_LLF5: if(*((int*)_tmp145)!= 10)goto
_LLE0;_tmp153=((struct Cyc_Absyn_ExternC_d_struct*)_tmp145)->f1;_LLF6: Cyc_Tc_tc_decls(
te,_tmp153,1,check_var_init);goto _LLE0;_LLE0:;}}void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*
te,int check_var_init,struct Cyc_List_List*ds){Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init);}struct Cyc_Tc_TreeshakeEnv{struct
_RegionHandle*rgn;struct Cyc_Dict_Dict*nsdict;};static int Cyc_Tc_vardecl_needed(
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);static struct Cyc_List_List*
Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){return((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,
env,ds);}struct _tuple8{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};static int
Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){void*
_tmp16D=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp16E;struct Cyc_List_List*_tmp16F;
struct Cyc_List_List**_tmp170;struct Cyc_List_List*_tmp171;struct Cyc_List_List**
_tmp172;struct Cyc_List_List*_tmp173;struct Cyc_List_List**_tmp174;_LL108: if(*((
int*)_tmp16D)!= 0)goto _LL10A;_tmp16E=((struct Cyc_Absyn_Var_d_struct*)_tmp16D)->f1;
_LL109: if((void*)_tmp16E->sc != (void*)3?(void*)_tmp16E->sc != (void*)4: 0)return 1;{
struct _tuple0 _tmp176;void*_tmp177;struct _tagged_arr*_tmp178;struct _tuple0*
_tmp175=_tmp16E->name;_tmp176=*_tmp175;_tmp177=_tmp176.f1;_tmp178=_tmp176.f2;{
struct Cyc_List_List*ns;{void*_tmp179=_tmp177;struct Cyc_List_List*_tmp17A;struct
Cyc_List_List*_tmp17B;_LL113: if((int)_tmp179 != 0)goto _LL115;_LL114: ns=0;goto
_LL112;_LL115: if(_tmp179 <= (void*)1?1:*((int*)_tmp179)!= 0)goto _LL117;_tmp17A=((
struct Cyc_Absyn_Rel_n_struct*)_tmp179)->f1;_LL116: ns=_tmp17A;goto _LL112;_LL117:
if(_tmp179 <= (void*)1?1:*((int*)_tmp179)!= 1)goto _LL112;_tmp17B=((struct Cyc_Absyn_Abs_n_struct*)
_tmp179)->f1;_LL118: ns=_tmp17B;goto _LL112;_LL112:;}{struct _tuple8*_tmp17C=((
struct _tuple8*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
env->nsdict,ns);struct Cyc_Tcenv_Genv*_tmp17D=(*_tmp17C).f1;int _tmp17E=(*((struct
_tuple5*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp17D->ordinaries,
_tmp178)).f2;if(!_tmp17E)(*_tmp17C).f2=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_rinsert)(env->rgn,(*
_tmp17C).f2,_tmp178);return _tmp17E;}}}_LL10A: if(*((int*)_tmp16D)!= 10)goto _LL10C;
_tmp16F=((struct Cyc_Absyn_ExternC_d_struct*)_tmp16D)->f1;_tmp170=(struct Cyc_List_List**)&((
struct Cyc_Absyn_ExternC_d_struct*)_tmp16D)->f1;_LL10B: _tmp172=_tmp170;goto _LL10D;
_LL10C: if(*((int*)_tmp16D)!= 9)goto _LL10E;_tmp171=((struct Cyc_Absyn_Using_d_struct*)
_tmp16D)->f2;_tmp172=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_struct*)
_tmp16D)->f2;_LL10D: _tmp174=_tmp172;goto _LL10F;_LL10E: if(*((int*)_tmp16D)!= 8)
goto _LL110;_tmp173=((struct Cyc_Absyn_Namespace_d_struct*)_tmp16D)->f2;_tmp174=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_struct*)_tmp16D)->f2;_LL10F:*
_tmp174=Cyc_Tc_treeshake_f(env,*_tmp174);return 1;_LL110:;_LL111: return 1;_LL107:;}
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _tagged_arr*x,
void*y){return !((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
set,x);}static struct _tuple8*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,
struct Cyc_Tcenv_Genv*ge){return({struct _tuple8*_tmp17F=_region_malloc(rgn,
sizeof(*_tmp17F));_tmp17F->f1=ge;_tmp17F->f2=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_rempty)(
rgn,Cyc_Std_strptrcmp);_tmp17F;});}struct _tuple9{struct Cyc_List_List*f1;struct
_tuple8*f2;};struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*ds){struct _RegionHandle _tmp180=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp180;_push_region(rgn);{struct Cyc_Tc_TreeshakeEnv _tmp181=({
struct Cyc_Tc_TreeshakeEnv _tmp18C;_tmp18C.rgn=rgn;_tmp18C.nsdict=((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*,struct _tuple8*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),
struct _RegionHandle*env,struct Cyc_Dict_Dict*d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,
rgn,te->ae);_tmp18C;});struct Cyc_List_List*_tmp182=Cyc_Tc_treeshake_f(& _tmp181,
ds);if(((int(*)(struct Cyc_Dict_Dict*d))Cyc_Dict_is_empty)(_tmp181.nsdict)){
struct Cyc_List_List*_tmp183=_tmp182;_npop_handler(0);return _tmp183;}{struct Cyc_Iter_Iter
_tmp184=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict*d))
Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp181.nsdict);struct _tuple9 _tmp185=*((
struct _tuple9*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict*d))Cyc_Dict_rchoose)(
rgn,_tmp181.nsdict);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple9*))Cyc_Iter_next)(
_tmp184,& _tmp185)){struct _tuple8*_tmp187;struct _tuple8 _tmp188;struct Cyc_Tcenv_Genv*
_tmp189;struct Cyc_Set_Set*_tmp18A;struct _tuple9 _tmp186=_tmp185;_tmp187=_tmp186.f2;
_tmp188=*_tmp187;_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;_tmp189->ordinaries=((
struct Cyc_Dict_Dict*(*)(int(*f)(struct Cyc_Set_Set*,struct _tagged_arr*,struct
_tuple5*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict*d))Cyc_Dict_filter_c)((int(*)(
struct Cyc_Set_Set*set,struct _tagged_arr*x,struct _tuple5*y))Cyc_Tc_treeshake_remove_f,
_tmp18A,_tmp189->ordinaries);}{struct Cyc_List_List*_tmp18B=_tmp182;_npop_handler(
0);return _tmp18B;}}};_pop_region(rgn);}
