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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};extern void*Cyc_Core_snd(
struct _tuple0*);extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);extern struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern unsigned char Cyc_List_Nth[8];struct Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_insert(
struct Cyc_Set_Set*s,void*elt);extern struct Cyc_Set_Set*Cyc_Set_union_two(struct
Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);extern unsigned char Cyc_Set_Absent[11];
struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char
Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,
void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
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
15];extern struct _tagged_arr*Cyc_Absyn_fieldname(int);extern int Cyc_Absyn_is_union_type(
void*);struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern
unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Tcenv_VarRes_struct{
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
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct
_tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);extern int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);extern int Cyc_Tcutil_is_noreturn(void*);extern
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};extern struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();
extern void*Cyc_CfFlowInfo_unknown_none;extern void*Cyc_CfFlowInfo_unknown_all;
extern int Cyc_CfFlowInfo_root_cmp(void*,void*);extern void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval);extern void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,
void*r);extern void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);extern int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2);extern
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);extern struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);extern struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);extern
struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**
all_changed,void*r);extern struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(struct
Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,
struct Cyc_CfFlowInfo_Place*place,void*r);extern void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**,void*,void*);extern void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**,
void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);extern void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);extern void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;void*flow;};static unsigned char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot";struct Cyc_NewControlFlow_CFAnnot_struct{
unsigned char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){(void*)(enclosee->annot=(
void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=_cycalloc(sizeof(*
_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;
_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;
_tmp2.visited=0;_tmp2.flow=(void*)((void*)0);_tmp2;});_tmp1;});_tmp0;})));}
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: if(*((void**)_tmp3)== Cyc_NewControlFlow_CFAnnot){
_LL5: _tmp4=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(struct
Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3)->f1;goto _LL2;}else{goto _LL3;}_LL3: goto _LL4;_LL2: return _tmp5;_LL4:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp6=_cycalloc(sizeof(*_tmp6));
_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;_tmp7.tag=Cyc_Core_Impossible;
_tmp7.f1=_tag_arr("ControlFlow -- wrong stmt annotation",sizeof(unsigned char),
37);_tmp7;});_tmp6;}));_LL0:;}struct Cyc_NewControlFlow_AnalEnv{int iterate_again;
int iteration_num;int in_try;void*tryflow;struct Cyc_Set_Set**all_changed;int
noreturn;};static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Stmt*);static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Decl*);static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Exp*);static struct
_tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,void*,
struct Cyc_Absyn_Exp*);static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Exp*);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp8=Cyc_NewControlFlow_get_stmt_annot(s);(void*)(
_tmp8->flow=(void*)Cyc_CfFlowInfo_join_flow(env->all_changed,inflow,(void*)_tmp8->flow));
_tmp8->visited=env->iteration_num;return _tmp8;}static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,void*new_flow){if(env->in_try){(void*)(env->tryflow=(
void*)Cyc_CfFlowInfo_join_flow(env->all_changed,new_flow,(void*)env->tryflow));}}
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_Absyn_Stmt*s,void*flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp9=Cyc_NewControlFlow_get_stmt_annot(
s);void*_tmpA=Cyc_CfFlowInfo_join_flow(env->all_changed,flow,(void*)_tmp9->flow);
if(! Cyc_CfFlowInfo_flow_lessthan_approx(_tmpA,(void*)_tmp9->flow)){(void*)(_tmp9->flow=(
void*)_tmpA);if(_tmp9->visited == env->iteration_num){env->iterate_again=1;}}}
static void*Cyc_NewControlFlow_add_vars(void*inflow,struct Cyc_List_List*vds,void*
leafval){void*_tmpB=inflow;struct Cyc_List_List*_tmpC;struct Cyc_Dict_Dict*_tmpD;
_LL7: if((int)_tmpB == 0){goto _LL8;}else{goto _LL9;}_LL9: if((unsigned int)_tmpB > 1?*((
int*)_tmpB)== 0: 0){_LLC: _tmpD=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpB)->f1;
goto _LLB;_LLB: _tmpC=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpB)->f2;goto
_LLA;}else{goto _LL6;}_LL8: return(void*)0;_LLA: for(0;vds != 0;vds=vds->tl){struct
Cyc_CfFlowInfo_VarRoot_struct*_tmpE=({struct Cyc_CfFlowInfo_VarRoot_struct*_tmpF=
_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp10;
_tmp10.tag=0;_tmp10.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp10;});_tmpF;});
_tmpD=Cyc_Dict_insert(_tmpD,(void*)_tmpE,Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp12;_tmp12.tag=0;_tmp12.f1=_tmpD;_tmp12.f2=_tmpC;_tmp12;});_tmp11;});_LL6:;}
static void*Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_Position_Segment*loc,void*inflow,void*r){void*_tmp13=inflow;struct Cyc_List_List*
_tmp14;struct Cyc_Dict_Dict*_tmp15;_LLE: if((int)_tmp13 == 0){goto _LLF;}else{goto
_LL10;}_LL10: if((unsigned int)_tmp13 > 1?*((int*)_tmp13)== 0: 0){_LL13: _tmp15=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp13)->f1;goto _LL12;_LL12: _tmp14=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp13)->f2;goto _LL11;}else{goto _LLD;}
_LLF: return(void*)0;_LL11: if(Cyc_CfFlowInfo_initlevel(_tmp15,r)!= (void*)2){({
void*_tmp16[0]={};Cyc_Tcutil_terr(loc,_tag_arr("expression may not be fully initialized",
sizeof(unsigned char),40),_tag_arr(_tmp16,sizeof(void*),0));});}{struct Cyc_Dict_Dict*
_tmp17=Cyc_CfFlowInfo_escape_deref(_tmp15,env->all_changed,r);if(_tmp15 == _tmp17){
return inflow;}{struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp18=({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1A;_tmp1A.tag=0;_tmp1A.f1=_tmp17;_tmp1A.f2=_tmp14;_tmp1A;});_tmp19;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp18);return(void*)_tmp18;}}_LLD:;}struct _tuple4{void*f1;struct Cyc_List_List*
f2;};static struct _tuple4 Cyc_NewControlFlow_anal_unordered_Rexps(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){if(es == 0){return({struct _tuple4 _tmp1B;
_tmp1B.f1=inflow;_tmp1B.f2=0;_tmp1B;});}if(es->tl == 0){void*_tmp1D;void*_tmp1E;
struct _tuple0 _tmp1C=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_LL16: _tmp1E=_tmp1C.f1;goto _LL15;_LL15: _tmp1D=_tmp1C.f2;goto _LL14;_LL14:
return({struct _tuple4 _tmp1F;_tmp1F.f1=_tmp1E;_tmp1F.f2=({struct Cyc_List_List*
_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->hd=(void*)_tmp1D;_tmp20->tl=0;_tmp20;});
_tmp1F;});}{struct Cyc_Set_Set**outer_all_changed=env->all_changed;struct Cyc_Set_Set*
this_all_changed;void*old_inflow;void*outflow;struct Cyc_List_List*rvals;do {
this_all_changed=Cyc_CfFlowInfo_mt_place_set();env->all_changed=({struct Cyc_Set_Set**
_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21[0]=Cyc_CfFlowInfo_mt_place_set();_tmp21;});{
void*_tmp23;void*_tmp24;struct _tuple0 _tmp22=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)es->hd);_LL19: _tmp24=_tmp22.f1;goto _LL18;_LL18:
_tmp23=_tmp22.f2;goto _LL17;_LL17: outflow=_tmp24;rvals=({struct Cyc_List_List*
_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->hd=(void*)_tmp23;_tmp25->tl=0;_tmp25;});
this_all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(this_all_changed,*((struct Cyc_Set_Set**)_check_null(env->all_changed)));{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({
struct Cyc_Set_Set**_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp26;});{void*_tmp28;void*_tmp29;struct _tuple0 _tmp27=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_LL1C: _tmp29=_tmp27.f1;goto _LL1B;_LL1B:
_tmp28=_tmp27.f2;goto _LL1A;_LL1A: rvals=({struct Cyc_List_List*_tmp2A=_cycalloc(
sizeof(*_tmp2A));_tmp2A->hd=(void*)_tmp28;_tmp2A->tl=rvals;_tmp2A;});outflow=Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& this_all_changed,outflow,_tmp29,this_all_changed,*((struct
Cyc_Set_Set**)_check_null(env->all_changed)));this_all_changed=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(this_all_changed,*((
struct Cyc_Set_Set**)_check_null(env->all_changed)));}}}old_inflow=inflow;inflow=
Cyc_CfFlowInfo_join_flow(outer_all_changed,inflow,outflow);}} while (! Cyc_CfFlowInfo_flow_lessthan_approx(
inflow,old_inflow));if(outer_all_changed == 0){env->all_changed=0;}else{env->all_changed=({
struct Cyc_Set_Set**_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B[0]=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*outer_all_changed,
this_all_changed);_tmp2B;});}Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple4 _tmp2C;_tmp2C.f1=outflow;_tmp2C.f2=Cyc_List_imp_rev(rvals);
_tmp2C;});}}static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){struct Cyc_List_List*_tmp2E;void*_tmp2F;
struct _tuple4 _tmp2D=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,es);_LL1F:
_tmp2F=_tmp2D.f1;goto _LL1E;_LL1E: _tmp2E=_tmp2D.f2;goto _LL1D;_LL1D:{void*_tmp30=
_tmp2F;struct Cyc_List_List*_tmp31;struct Cyc_Dict_Dict*_tmp32;_LL21: if((
unsigned int)_tmp30 > 1?*((int*)_tmp30)== 0: 0){_LL26: _tmp32=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp30)->f1;goto _LL25;_LL25: _tmp31=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp30)->f2;goto _LL22;}else{goto _LL23;}_LL23: if((int)_tmp30 == 0){goto _LL24;}
else{goto _LL20;}_LL22: for(0;_tmp2E != 0;(_tmp2E=_tmp2E->tl,es=((struct Cyc_List_List*)
_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(_tmp32,(void*)_tmp2E->hd)== (
void*)0){({void*_tmp33[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,_tag_arr("expression may not be initialized",sizeof(
unsigned char),34),_tag_arr(_tmp33,sizeof(void*),0));});}}goto _LL20;_LL24: goto
_LL20;_LL20:;}return({struct _tuple0 _tmp34;_tmp34.f1=_tmp2F;_tmp34.f2=Cyc_CfFlowInfo_unknown_all;
_tmp34;});}static void*Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp35=outflow;
struct Cyc_List_List*_tmp36;struct Cyc_Dict_Dict*_tmp37;_LL28: if((int)_tmp35 == 0){
goto _LL29;}else{goto _LL2A;}_LL2A: if((unsigned int)_tmp35 > 1?*((int*)_tmp35)== 0:
0){_LL2D: _tmp37=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp35)->f1;goto _LL2C;
_LL2C: _tmp36=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp35)->f2;goto _LL2B;}
else{goto _LL27;}_LL29: return outflow;_LL2B: {void*_tmp38=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp39;_LL2F: if((int)_tmp38 == 0){
goto _LL30;}else{goto _LL31;}_LL31: if((unsigned int)_tmp38 > 1?*((int*)_tmp38)== 0:
0){_LL33: _tmp39=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmp38)->f1;goto _LL32;}
else{goto _LL2E;}_LL30: return outflow;_LL32: {void*nzval=il == (void*)2?(void*)1:(
void*)2;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3A=_cycalloc(
sizeof(*_tmp3A));_tmp3A[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3B;
_tmp3B.tag=0;_tmp3B.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp37,env->all_changed,
_tmp39,nzval);_tmp3B.f2=_tmp36;_tmp3B;});_tmp3A;});}_LL2E:;}_LL27:;}static struct
_tuple0 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp3C=outflow;struct Cyc_List_List*
_tmp3D;struct Cyc_Dict_Dict*_tmp3E;_LL35: if((int)_tmp3C == 0){goto _LL36;}else{goto
_LL37;}_LL37: if((unsigned int)_tmp3C > 1?*((int*)_tmp3C)== 0: 0){_LL3A: _tmp3E=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3C)->f1;goto _LL39;_LL39: _tmp3D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3C)->f2;goto _LL38;}else{goto _LL34;}
_LL36: return({struct _tuple0 _tmp3F;_tmp3F.f1=outflow;_tmp3F.f2=outflow;_tmp3F;});
_LL38: {void*_tmp40=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*
_tmp41;_LL3C: if((int)_tmp40 == 0){goto _LL3D;}else{goto _LL3E;}_LL3E: if((
unsigned int)_tmp40 > 1?*((int*)_tmp40)== 0: 0){_LL40: _tmp41=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp40)->f1;goto _LL3F;}else{goto _LL3B;}_LL3D: return({struct _tuple0 _tmp42;_tmp42.f1=
outflow;_tmp42.f2=outflow;_tmp42;});_LL3F: {void*nzval=il == (void*)2?(void*)1:(
void*)2;return({struct _tuple0 _tmp43;_tmp43.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp47;_tmp47.tag=0;_tmp47.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp3E,env->all_changed,
_tmp41,nzval);_tmp47.f2=_tmp3D;_tmp47;});_tmp46;});_tmp43.f2=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp45;_tmp45.tag=0;_tmp45.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp3E,env->all_changed,
_tmp41,(void*)0);_tmp45.f2=_tmp3D;_tmp45;});_tmp44;});_tmp43;});}_LL3B:;}_LL34:;}
static struct _tuple0 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp48=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp49;struct Cyc_Absyn_Conref*_tmp4A;void*_tmp4B;_LL42: if((unsigned int)_tmp48 > 
3?*((int*)_tmp48)== 4: 0){_LL46: _tmp49=((struct Cyc_Absyn_PointerType_struct*)
_tmp48)->f1;_LL48: _tmp4B=(void*)_tmp49.elt_typ;goto _LL47;_LL47: _tmp4A=_tmp49.bounds;
goto _LL43;}else{goto _LL44;}_LL44: goto _LL45;_LL43: {void*_tmp4C=f;struct Cyc_List_List*
_tmp4D;struct Cyc_Dict_Dict*_tmp4E;_LL4A: if((int)_tmp4C == 0){goto _LL4B;}else{goto
_LL4C;}_LL4C: if((unsigned int)_tmp4C > 1?*((int*)_tmp4C)== 0: 0){_LL4F: _tmp4E=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4C)->f1;goto _LL4E;_LL4E: _tmp4D=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp4C)->f2;goto _LL4D;}else{goto _LL49;}
_LL4B: return({struct _tuple0 _tmp4F;_tmp4F.f1=f;_tmp4F.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4B,Cyc_CfFlowInfo_unknown_all);_tmp4F;});_LL4D: if(Cyc_Tcutil_is_bound_one(
_tmp4A)){void*_tmp50=r;struct Cyc_CfFlowInfo_Place*_tmp51;void*_tmp52;_LL51: if((
int)_tmp50 == 1){goto _LL52;}else{goto _LL53;}_LL53: if((int)_tmp50 == 2){goto _LL54;}
else{goto _LL55;}_LL55: if((unsigned int)_tmp50 > 3?*((int*)_tmp50)== 2: 0){_LL5D:
_tmp51=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp50)->f1;goto _LL56;}else{goto
_LL57;}_LL57: if((int)_tmp50 == 0){goto _LL58;}else{goto _LL59;}_LL59: if((
unsigned int)_tmp50 > 3?*((int*)_tmp50)== 0: 0){_LL5E: _tmp52=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp50)->f1;goto _LL5A;}else{goto _LL5B;}_LL5B: goto _LL5C;_LL52: goto _LL54;_LL54:(
void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp53=
_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp54;
_tmp54.tag=Cyc_CfFlowInfo_NotZero;_tmp54.f1=_tmp4D;_tmp54;});_tmp53;})));goto
_LL50;_LL56:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp56;_tmp56.tag=Cyc_CfFlowInfo_NotZero;_tmp56.f1=_tmp4D;_tmp56;});_tmp55;})));
return({struct _tuple0 _tmp57;_tmp57.f1=f;_tmp57.f2=Cyc_CfFlowInfo_lookup_place(
_tmp4E,_tmp51);_tmp57;});_LL58:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*
_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58[0]=({struct Cyc_CfFlowInfo_IsZero_struct
_tmp59;_tmp59.tag=Cyc_CfFlowInfo_IsZero;_tmp59.f1=_tmp4D;_tmp59;});_tmp58;})));
return({struct _tuple0 _tmp5A;_tmp5A.f1=(void*)0;_tmp5A.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4B,Cyc_CfFlowInfo_unknown_all);_tmp5A;});_LL5A: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmp52);goto _LL5C;_LL5C:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_UnknownZ_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmp5C;_tmp5C.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp5C.f1=_tmp4D;_tmp5C;});_tmp5B;})));_LL50:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));
_tmp5D[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp5E;_tmp5E.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp5E.f1=_tmp4D;_tmp5E;});_tmp5D;})));}{void*_tmp5F=Cyc_CfFlowInfo_initlevel(
_tmp4E,r);_LL60: if((int)_tmp5F == 0){goto _LL61;}else{goto _LL62;}_LL62: if((int)
_tmp5F == 2){goto _LL63;}else{goto _LL64;}_LL64: if((int)_tmp5F == 1){goto _LL65;}
else{goto _LL5F;}_LL61:({void*_tmp60[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of possibly uninitialized pointer",
sizeof(unsigned char),46),_tag_arr(_tmp60,sizeof(void*),0));});goto _LL63;_LL63:
return({struct _tuple0 _tmp61;_tmp61.f1=f;_tmp61.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp4B,Cyc_CfFlowInfo_unknown_all);_tmp61;});_LL65: return({struct _tuple0 _tmp62;
_tmp62.f1=f;_tmp62.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp4B,Cyc_CfFlowInfo_unknown_none);
_tmp62;});_LL5F:;}_LL49:;}_LL45:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Core_Impossible_struct
_tmp64;_tmp64.tag=Cyc_Core_Impossible;_tmp64.f1=_tag_arr("right deref of non-pointer-type",
sizeof(unsigned char),32);_tmp64;});_tmp63;}));_LL41:;}static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*_tmp65=(void*)e1->r;void*_tmp66;struct Cyc_Absyn_Vardecl*
_tmp67;void*_tmp68;struct Cyc_Absyn_Vardecl*_tmp69;void*_tmp6A;struct Cyc_Absyn_Vardecl*
_tmp6B;void*_tmp6C;struct Cyc_Absyn_Vardecl*_tmp6D;_LL67: if(*((int*)_tmp65)== 1){
_LL71: _tmp66=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if((unsigned int)
_tmp66 > 1?*((int*)_tmp66)== 4: 0){_LL72: _tmp67=((struct Cyc_Absyn_Pat_b_struct*)
_tmp66)->f1;goto _LL68;}else{goto _LL69;}}else{goto _LL69;}_LL69: if(*((int*)_tmp65)
== 1){_LL73: _tmp68=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if((
unsigned int)_tmp68 > 1?*((int*)_tmp68)== 3: 0){_LL74: _tmp69=((struct Cyc_Absyn_Local_b_struct*)
_tmp68)->f1;goto _LL6A;}else{goto _LL6B;}}else{goto _LL6B;}_LL6B: if(*((int*)_tmp65)
== 1){_LL75: _tmp6A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if((
unsigned int)_tmp6A > 1?*((int*)_tmp6A)== 2: 0){_LL76: _tmp6B=((struct Cyc_Absyn_Param_b_struct*)
_tmp6A)->f1;goto _LL6C;}else{goto _LL6D;}}else{goto _LL6D;}_LL6D: if(*((int*)_tmp65)
== 1){_LL77: _tmp6C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp65)->f2;if((
unsigned int)_tmp6C > 1?*((int*)_tmp6C)== 0: 0){_LL78: _tmp6D=((struct Cyc_Absyn_Global_b_struct*)
_tmp6C)->f1;goto _LL6E;}else{goto _LL6F;}}else{goto _LL6F;}_LL6F: goto _LL70;_LL68:
_tmp69=_tmp67;goto _LL6A;_LL6A: _tmp6B=_tmp69;goto _LL6C;_LL6C: _tmp6D=_tmp6B;goto
_LL6E;_LL6E: if(! _tmp6D->escapes){void*_tmp6E=(void*)e2->r;void*_tmp6F;struct Cyc_Absyn_Vardecl*
_tmp70;void*_tmp71;struct Cyc_Absyn_Vardecl*_tmp72;void*_tmp73;struct Cyc_Absyn_Vardecl*
_tmp74;void*_tmp75;struct Cyc_Absyn_Vardecl*_tmp76;_LL7A: if(*((int*)_tmp6E)== 1){
_LL84: _tmp6F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if((unsigned int)
_tmp6F > 1?*((int*)_tmp6F)== 4: 0){_LL85: _tmp70=((struct Cyc_Absyn_Pat_b_struct*)
_tmp6F)->f1;goto _LL7B;}else{goto _LL7C;}}else{goto _LL7C;}_LL7C: if(*((int*)_tmp6E)
== 1){_LL86: _tmp71=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if((
unsigned int)_tmp71 > 1?*((int*)_tmp71)== 3: 0){_LL87: _tmp72=((struct Cyc_Absyn_Local_b_struct*)
_tmp71)->f1;goto _LL7D;}else{goto _LL7E;}}else{goto _LL7E;}_LL7E: if(*((int*)_tmp6E)
== 1){_LL88: _tmp73=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if((
unsigned int)_tmp73 > 1?*((int*)_tmp73)== 2: 0){_LL89: _tmp74=((struct Cyc_Absyn_Param_b_struct*)
_tmp73)->f1;goto _LL7F;}else{goto _LL80;}}else{goto _LL80;}_LL80: if(*((int*)_tmp6E)
== 1){_LL8A: _tmp75=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6E)->f2;if((
unsigned int)_tmp75 > 1?*((int*)_tmp75)== 0: 0){_LL8B: _tmp76=((struct Cyc_Absyn_Global_b_struct*)
_tmp75)->f1;goto _LL81;}else{goto _LL82;}}else{goto _LL82;}_LL82: goto _LL83;_LL7B:
_tmp72=_tmp70;goto _LL7D;_LL7D: _tmp74=_tmp72;goto _LL7F;_LL7F: _tmp76=_tmp74;goto
_LL81;_LL81: if(! _tmp76->escapes){int found=0;{struct Cyc_List_List*_tmp77=relns;
for(0;_tmp77 != 0;_tmp77=_tmp77->tl){struct Cyc_CfFlowInfo_Reln*_tmp78=(struct Cyc_CfFlowInfo_Reln*)
_tmp77->hd;if(_tmp78->vd == _tmp76){void*_tmp79=(void*)_tmp78->rop;struct Cyc_Absyn_Vardecl*
_tmp7A;_LL8D: if(*((int*)_tmp79)== 2){_LL92: _tmp7A=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp79)->f1;goto _LL91;}else{goto _LL8F;}_LL91: if(_tmp7A == _tmp6D){goto _LL8E;}
else{goto _LL8F;}_LL8F: goto _LL90;_LL8E: return relns;_LL90: continue;_LL8C:;}}}if(!
found){return({struct Cyc_List_List*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->vd=_tmp76;
_tmp7C->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmp7D=
_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp7E;_tmp7E.tag=2;_tmp7E.f1=_tmp6D;_tmp7E;});_tmp7D;}));_tmp7C;});_tmp7B->tl=
relns;_tmp7B;});}}return relns;_LL83: return relns;_LL79:;}else{return relns;}_LL70:
return relns;_LL66:;}struct _tuple5{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;
};static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){struct Cyc_Dict_Dict*d;struct Cyc_List_List*
relns;{void*_tmp7F=inflow;struct Cyc_List_List*_tmp80;struct Cyc_Dict_Dict*_tmp81;
_LL94: if((int)_tmp7F == 0){goto _LL95;}else{goto _LL96;}_LL96: if((unsigned int)
_tmp7F > 1?*((int*)_tmp7F)== 0: 0){_LL99: _tmp81=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp7F)->f1;goto _LL98;_LL98: _tmp80=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp7F)->f2;goto _LL97;}else{goto _LL93;}_LL95: return({struct _tuple0 _tmp82;_tmp82.f1=(
void*)0;_tmp82.f2=Cyc_CfFlowInfo_unknown_all;_tmp82;});_LL97: d=_tmp81;relns=
_tmp80;_LL93:;}{void*_tmp83=(void*)e->r;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_Absyn_Exp*
_tmp85;struct Cyc_Absyn_Exp*_tmp86;void*_tmp87;void*_tmp88;int _tmp89;void*_tmp8A;
void*_tmp8B;void*_tmp8C;void*_tmp8D;struct Cyc_List_List*_tmp8E;void*_tmp8F;void*
_tmp90;struct Cyc_Absyn_Vardecl*_tmp91;void*_tmp92;struct Cyc_Absyn_Vardecl*_tmp93;
void*_tmp94;struct Cyc_Absyn_Vardecl*_tmp95;struct Cyc_List_List*_tmp96;struct Cyc_Absyn_Exp*
_tmp97;struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Core_Opt*_tmp99;struct Cyc_Core_Opt
_tmp9A;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Core_Opt*
_tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*
_tmpA0;struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_List_List*_tmpA2;struct Cyc_Absyn_Exp*
_tmpA3;struct Cyc_Absyn_MallocInfo _tmpA4;int _tmpA5;struct Cyc_Absyn_Exp*_tmpA6;
void**_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;int _tmpA9;struct Cyc_Absyn_Exp*_tmpAA;
struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_Absyn_Exp*_tmpAD;
struct _tagged_arr*_tmpAE;struct Cyc_Absyn_Exp*_tmpAF;struct _tagged_arr*_tmpB0;
struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Exp*_tmpB3;
struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_Absyn_Exp*_tmpB6;
struct Cyc_List_List*_tmpB7;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB9;
struct Cyc_List_List*_tmpBA;struct Cyc_List_List*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;
struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Vardecl*_tmpBE;struct Cyc_Absyn_Stmt*
_tmpBF;void*_tmpC0;_LL9B: if(*((int*)_tmp83)== 13){_LLFB: _tmp84=((struct Cyc_Absyn_Cast_e_struct*)
_tmp83)->f2;goto _LL9C;}else{goto _LL9D;}_LL9D: if(*((int*)_tmp83)== 11){_LLFC:
_tmp85=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp83)->f1;goto _LL9E;}else{
goto _LL9F;}_LL9F: if(*((int*)_tmp83)== 12){_LLFD: _tmp86=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp83)->f1;goto _LLA0;}else{goto _LLA1;}_LLA1: if(*((int*)_tmp83)== 0){_LLFE:
_tmp87=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp83)->f1;if((int)_tmp87 == 0){
goto _LLA2;}else{goto _LLA3;}}else{goto _LLA3;}_LLA3: if(*((int*)_tmp83)== 0){_LLFF:
_tmp88=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp83)->f1;if((unsigned int)
_tmp88 > 1?*((int*)_tmp88)== 2: 0){_LL101: _tmp8A=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp88)->f1;if((int)_tmp8A == 0){goto _LL100;}else{goto _LLA5;}_LL100: _tmp89=((
struct Cyc_Absyn_Int_c_struct*)_tmp88)->f2;if(_tmp89 == 0){goto _LLA4;}else{goto
_LLA5;}}else{goto _LLA5;}}else{goto _LLA5;}_LLA5: if(*((int*)_tmp83)== 0){_LL102:
_tmp8B=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp83)->f1;if((unsigned int)
_tmp8B > 1?*((int*)_tmp8B)== 2: 0){_LL103: _tmp8C=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp8B)->f1;if((int)_tmp8C == 0){goto _LLA6;}else{goto _LLA7;}}else{goto _LLA7;}}
else{goto _LLA7;}_LLA7: if(*((int*)_tmp83)== 1){_LL104: _tmp8D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp83)->f2;if((unsigned int)_tmp8D > 1?*((int*)_tmp8D)== 1: 0){goto _LLA8;}else{
goto _LLA9;}}else{goto _LLA9;}_LLA9: if(*((int*)_tmp83)== 30){_LL105: _tmp8E=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp83)->f1;if(_tmp8E == 0){goto _LLAA;}else{goto
_LLAB;}}else{goto _LLAB;}_LLAB: if(*((int*)_tmp83)== 0){goto _LLAC;}else{goto _LLAD;}
_LLAD: if(*((int*)_tmp83)== 17){goto _LLAE;}else{goto _LLAF;}_LLAF: if(*((int*)
_tmp83)== 16){goto _LLB0;}else{goto _LLB1;}_LLB1: if(*((int*)_tmp83)== 18){goto
_LLB2;}else{goto _LLB3;}_LLB3: if(*((int*)_tmp83)== 19){goto _LLB4;}else{goto _LLB5;}
_LLB5: if(*((int*)_tmp83)== 32){goto _LLB6;}else{goto _LLB7;}_LLB7: if(*((int*)
_tmp83)== 31){goto _LLB8;}else{goto _LLB9;}_LLB9: if(*((int*)_tmp83)== 1){_LL106:
_tmp8F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp83)->f2;if((unsigned int)
_tmp8F > 1?*((int*)_tmp8F)== 0: 0){goto _LLBA;}else{goto _LLBB;}}else{goto _LLBB;}
_LLBB: if(*((int*)_tmp83)== 1){_LL107: _tmp90=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp83)->f2;if((unsigned int)_tmp90 > 1?*((int*)_tmp90)== 2: 0){_LL108: _tmp91=((
struct Cyc_Absyn_Param_b_struct*)_tmp90)->f1;goto _LLBC;}else{goto _LLBD;}}else{
goto _LLBD;}_LLBD: if(*((int*)_tmp83)== 1){_LL109: _tmp92=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp83)->f2;if((unsigned int)_tmp92 > 1?*((int*)_tmp92)== 3: 0){_LL10A: _tmp93=((
struct Cyc_Absyn_Local_b_struct*)_tmp92)->f1;goto _LLBE;}else{goto _LLBF;}}else{
goto _LLBF;}_LLBF: if(*((int*)_tmp83)== 1){_LL10B: _tmp94=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp83)->f2;if((unsigned int)_tmp94 > 1?*((int*)_tmp94)== 4: 0){_LL10C: _tmp95=((
struct Cyc_Absyn_Pat_b_struct*)_tmp94)->f1;goto _LLC0;}else{goto _LLC1;}}else{goto
_LLC1;}_LLC1: if(*((int*)_tmp83)== 3){_LL10D: _tmp96=((struct Cyc_Absyn_Primop_e_struct*)
_tmp83)->f2;goto _LLC2;}else{goto _LLC3;}_LLC3: if(*((int*)_tmp83)== 5){_LL10E:
_tmp97=((struct Cyc_Absyn_Increment_e_struct*)_tmp83)->f1;goto _LLC4;}else{goto
_LLC5;}_LLC5: if(*((int*)_tmp83)== 4){_LL111: _tmp9B=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp83)->f1;goto _LL110;_LL110: _tmp99=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f2;
if(_tmp99 == 0){goto _LLC7;}else{_tmp9A=*_tmp99;goto _LL10F;}_LL10F: _tmp98=((struct
Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f3;goto _LLC6;}else{goto _LLC7;}_LLC7: if(*((
int*)_tmp83)== 4){_LL114: _tmp9E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f1;
goto _LL113;_LL113: _tmp9D=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp83)->f2;if(
_tmp9D == 0){goto _LL112;}else{goto _LLC9;}_LL112: _tmp9C=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp83)->f3;goto _LLC8;}else{goto _LLC9;}_LLC9: if(*((int*)_tmp83)== 7){_LL116:
_tmpA0=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp83)->f1;goto _LL115;_LL115: _tmp9F=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp83)->f2;goto _LLCA;}else{goto _LLCB;}_LLCB:
if(*((int*)_tmp83)== 10){_LL117: _tmpA1=((struct Cyc_Absyn_Throw_e_struct*)_tmp83)->f1;
goto _LLCC;}else{goto _LLCD;}_LLCD: if(*((int*)_tmp83)== 9){_LL119: _tmpA3=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp83)->f1;goto _LL118;_LL118: _tmpA2=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp83)->f2;goto _LLCE;}else{goto _LLCF;}_LLCF: if(*((int*)_tmp83)== 33){_LL11A:
_tmpA4=((struct Cyc_Absyn_Malloc_e_struct*)_tmp83)->f1;_LL11F: _tmpA9=_tmpA4.is_calloc;
goto _LL11E;_LL11E: _tmpA8=_tmpA4.rgn;goto _LL11D;_LL11D: _tmpA7=_tmpA4.elt_type;
goto _LL11C;_LL11C: _tmpA6=_tmpA4.num_elts;goto _LL11B;_LL11B: _tmpA5=_tmpA4.fat_result;
goto _LLD0;}else{goto _LLD1;}_LLD1: if(*((int*)_tmp83)== 15){_LL121: _tmpAB=((struct
Cyc_Absyn_New_e_struct*)_tmp83)->f1;goto _LL120;_LL120: _tmpAA=((struct Cyc_Absyn_New_e_struct*)
_tmp83)->f2;goto _LLD2;}else{goto _LLD3;}_LLD3: if(*((int*)_tmp83)== 14){_LL122:
_tmpAC=((struct Cyc_Absyn_Address_e_struct*)_tmp83)->f1;goto _LLD4;}else{goto _LLD5;}
_LLD5: if(*((int*)_tmp83)== 20){_LL123: _tmpAD=((struct Cyc_Absyn_Deref_e_struct*)
_tmp83)->f1;goto _LLD6;}else{goto _LLD7;}_LLD7: if(*((int*)_tmp83)== 21){_LL125:
_tmpAF=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp83)->f1;goto _LL124;_LL124:
_tmpAE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp83)->f2;goto _LLD8;}else{goto
_LLD9;}_LLD9: if(*((int*)_tmp83)== 22){_LL127: _tmpB1=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp83)->f1;goto _LL126;_LL126: _tmpB0=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp83)->f2;goto _LLDA;}else{goto _LLDB;}_LLDB: if(*((int*)_tmp83)== 6){_LL12A:
_tmpB4=((struct Cyc_Absyn_Conditional_e_struct*)_tmp83)->f1;goto _LL129;_LL129:
_tmpB3=((struct Cyc_Absyn_Conditional_e_struct*)_tmp83)->f2;goto _LL128;_LL128:
_tmpB2=((struct Cyc_Absyn_Conditional_e_struct*)_tmp83)->f3;goto _LLDC;}else{goto
_LLDD;}_LLDD: if(*((int*)_tmp83)== 23){_LL12C: _tmpB6=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp83)->f1;goto _LL12B;_LL12B: _tmpB5=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp83)->f2;goto _LLDE;}else{goto _LLDF;}_LLDF: if(*((int*)_tmp83)== 30){_LL12D:
_tmpB7=((struct Cyc_Absyn_Tunion_e_struct*)_tmp83)->f1;goto _LLE0;}else{goto _LLE1;}
_LLE1: if(*((int*)_tmp83)== 24){_LL12E: _tmpB8=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp83)->f1;goto _LLE2;}else{goto _LLE3;}_LLE3: if(*((int*)_tmp83)== 29){_LL12F:
_tmpB9=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp83)->f2;goto _LLE4;}else{goto
_LLE5;}_LLE5: if(*((int*)_tmp83)== 28){_LL130: _tmpBA=((struct Cyc_Absyn_Struct_e_struct*)
_tmp83)->f2;goto _LLE6;}else{goto _LLE7;}_LLE7: if(*((int*)_tmp83)== 26){_LL131:
_tmpBB=((struct Cyc_Absyn_Array_e_struct*)_tmp83)->f1;goto _LLE8;}else{goto _LLE9;}
_LLE9: if(*((int*)_tmp83)== 27){_LL134: _tmpBE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp83)->f1;goto _LL133;_LL133: _tmpBD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp83)->f2;goto _LL132;_LL132: _tmpBC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp83)->f3;goto _LLEA;}else{goto _LLEB;}_LLEB: if(*((int*)_tmp83)== 35){_LL135:
_tmpBF=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp83)->f1;goto _LLEC;}else{goto _LLED;}
_LLED: if(*((int*)_tmp83)== 1){_LL136: _tmpC0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp83)->f2;if((int)_tmpC0 == 0){goto _LLEE;}else{goto _LLEF;}}else{goto _LLEF;}
_LLEF: if(*((int*)_tmp83)== 2){goto _LLF0;}else{goto _LLF1;}_LLF1: if(*((int*)_tmp83)
== 8){goto _LLF2;}else{goto _LLF3;}_LLF3: if(*((int*)_tmp83)== 34){goto _LLF4;}else{
goto _LLF5;}_LLF5: if(*((int*)_tmp83)== 25){goto _LLF6;}else{goto _LLF7;}_LLF7: if(*((
int*)_tmp83)== 36){goto _LLF8;}else{goto _LLF9;}_LLF9: if(*((int*)_tmp83)== 37){
goto _LLFA;}else{goto _LL9A;}_LL9C: _tmp85=_tmp84;goto _LL9E;_LL9E: _tmp86=_tmp85;
goto _LLA0;_LLA0: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp86);_LLA2: goto
_LLA4;_LLA4: return({struct _tuple0 _tmpC1;_tmpC1.f1=inflow;_tmpC1.f2=(void*)0;
_tmpC1;});_LLA6: goto _LLA8;_LLA8: return({struct _tuple0 _tmpC2;_tmpC2.f1=inflow;
_tmpC2.f2=(void*)1;_tmpC2;});_LLAA: goto _LLAC;_LLAC: goto _LLAE;_LLAE: goto _LLB0;
_LLB0: goto _LLB2;_LLB2: goto _LLB4;_LLB4: goto _LLB6;_LLB6: goto _LLB8;_LLB8: return({
struct _tuple0 _tmpC3;_tmpC3.f1=inflow;_tmpC3.f2=Cyc_CfFlowInfo_unknown_all;_tmpC3;});
_LLBA: return({struct _tuple0 _tmpC4;_tmpC4.f1=inflow;_tmpC4.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmpC4;});_LLBC: _tmp93=_tmp91;goto _LLBE;_LLBE: _tmp95=_tmp93;goto _LLC0;_LLC0:
return({struct _tuple0 _tmpC5;_tmpC5.f1=inflow;_tmpC5.f2=Cyc_Dict_lookup(d,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _tmpC7;_tmpC7.tag=0;_tmpC7.f1=_tmp95;_tmpC7;});
_tmpC6;}));_tmpC5;});_LLC2: return Cyc_NewControlFlow_anal_use_ints(env,inflow,
_tmp96);_LLC4: {void*_tmpCA;struct _tuple0 _tmpC9=Cyc_NewControlFlow_anal_use_ints(
env,inflow,({struct Cyc_Absyn_Exp*_tmpC8[1];_tmpC8[0]=_tmp97;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpC8,sizeof(struct Cyc_Absyn_Exp*),1));}));
_LL138: _tmpCA=_tmpC9.f1;goto _LL137;_LL137: {void*_tmpCC;struct _tuple0 _tmpCB=Cyc_NewControlFlow_anal_Lexp(
env,_tmpCA,_tmp97);_LL13A: _tmpCC=_tmpCB.f2;goto _LL139;_LL139:{struct _tuple0
_tmpCE=({struct _tuple0 _tmpCD;_tmpCD.f1=_tmpCC;_tmpCD.f2=_tmpCA;_tmpCD;});void*
_tmpCF;struct Cyc_List_List*_tmpD0;struct Cyc_Dict_Dict*_tmpD1;void*_tmpD2;struct
Cyc_CfFlowInfo_Place*_tmpD3;_LL13C: _LL143: _tmpD2=_tmpCE.f1;if((unsigned int)
_tmpD2 > 1?*((int*)_tmpD2)== 0: 0){_LL144: _tmpD3=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmpD2)->f1;goto _LL140;}else{goto _LL13E;}_LL140: _tmpCF=_tmpCE.f2;if((
unsigned int)_tmpCF > 1?*((int*)_tmpCF)== 0: 0){_LL142: _tmpD1=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpCF)->f1;goto _LL141;_LL141: _tmpD0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpCF)->f2;goto _LL13D;}else{goto _LL13E;}_LL13E: goto _LL13F;_LL13D: _tmpD0=Cyc_CfFlowInfo_reln_kill_exp(
_tmpD0,_tmp97);_tmpCA=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmpD4=
_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpD5;_tmpD5.tag=0;_tmpD5.f1=Cyc_CfFlowInfo_assign_place(_tmp97->loc,_tmpD1,env->all_changed,
_tmpD3,Cyc_CfFlowInfo_unknown_all);_tmpD5.f2=_tmpD0;_tmpD5;});_tmpD4;});goto
_LL13B;_LL13F: goto _LL13B;_LL13B:;}return({struct _tuple0 _tmpD6;_tmpD6.f1=_tmpCA;
_tmpD6.f2=Cyc_CfFlowInfo_unknown_all;_tmpD6;});}}_LLC6: {void*_tmpD9;struct
_tuple0 _tmpD8=Cyc_NewControlFlow_anal_use_ints(env,inflow,({struct Cyc_Absyn_Exp*
_tmpD7[2];_tmpD7[1]=_tmp98;_tmpD7[0]=_tmp9B;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmpD7,sizeof(struct Cyc_Absyn_Exp*),2));}));
_LL146: _tmpD9=_tmpD8.f1;goto _LL145;_LL145: {void*_tmpDB;struct _tuple0 _tmpDA=Cyc_NewControlFlow_anal_Lexp(
env,_tmpD9,e);_LL148: _tmpDB=_tmpDA.f2;goto _LL147;_LL147:{struct _tuple0 _tmpDD=({
struct _tuple0 _tmpDC;_tmpDC.f1=_tmpDB;_tmpDC.f2=_tmpD9;_tmpDC;});void*_tmpDE;
struct Cyc_List_List*_tmpDF;struct Cyc_Dict_Dict*_tmpE0;void*_tmpE1;struct Cyc_CfFlowInfo_Place*
_tmpE2;_LL14A: _LL151: _tmpE1=_tmpDD.f1;if((unsigned int)_tmpE1 > 1?*((int*)_tmpE1)
== 0: 0){_LL152: _tmpE2=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmpE1)->f1;goto
_LL14E;}else{goto _LL14C;}_LL14E: _tmpDE=_tmpDD.f2;if((unsigned int)_tmpDE > 1?*((
int*)_tmpDE)== 0: 0){_LL150: _tmpE0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpDE)->f1;goto _LL14F;_LL14F: _tmpDF=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpDE)->f2;goto _LL14B;}else{goto _LL14C;}_LL14C: goto _LL14D;_LL14B: _tmpDF=Cyc_CfFlowInfo_reln_kill_exp(
_tmpDF,_tmp9B);_tmpD9=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmpE3=
_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpE4;_tmpE4.tag=0;_tmpE4.f1=Cyc_CfFlowInfo_assign_place(_tmp9B->loc,_tmpE0,env->all_changed,
_tmpE2,Cyc_CfFlowInfo_unknown_all);_tmpE4.f2=_tmpDF;_tmpE4;});_tmpE3;});goto
_LL149;_LL14D: goto _LL149;_LL149:;}return({struct _tuple0 _tmpE5;_tmpE5.f1=_tmpD9;
_tmpE5.f2=Cyc_CfFlowInfo_unknown_all;_tmpE5;});}}_LLC8: {struct Cyc_Set_Set**
_tmpE6=env->all_changed;while(1){env->all_changed=({struct Cyc_Set_Set**_tmpE7=
_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=Cyc_CfFlowInfo_mt_place_set();_tmpE7;});{
void*_tmpE9;void*_tmpEA;struct _tuple0 _tmpE8=Cyc_NewControlFlow_anal_Lexp(env,
inflow,_tmp9E);_LL155: _tmpEA=_tmpE8.f1;goto _LL154;_LL154: _tmpE9=_tmpE8.f2;goto
_LL153;_LL153: {struct Cyc_Set_Set*_tmpEB=*((struct Cyc_Set_Set**)_check_null(env->all_changed));
env->all_changed=({struct Cyc_Set_Set**_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=
Cyc_CfFlowInfo_mt_place_set();_tmpEC;});{void*_tmpEE;void*_tmpEF;struct _tuple0
_tmpED=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp9C);_LL158: _tmpEF=_tmpED.f1;
goto _LL157;_LL157: _tmpEE=_tmpED.f2;goto _LL156;_LL156: {struct Cyc_Set_Set*_tmpF0=*((
struct Cyc_Set_Set**)_check_null(env->all_changed));void*_tmpF1=Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _tmpEB,_tmpEA,_tmpEF,_tmpEB,_tmpF0);void*_tmpF2=Cyc_CfFlowInfo_join_flow(
_tmpE6,inflow,_tmpF1);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmpF2,inflow)){if(
_tmpE6 == 0){env->all_changed=0;}else{*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*
_tmpE6,((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmpEB,_tmpF0));}{void*_tmpF3=_tmpF1;struct Cyc_List_List*_tmpF4;struct Cyc_Dict_Dict*
_tmpF5;_LL15A: if((int)_tmpF3 == 0){goto _LL15B;}else{goto _LL15C;}_LL15C: if((
unsigned int)_tmpF3 > 1?*((int*)_tmpF3)== 0: 0){_LL15F: _tmpF5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpF3)->f1;goto _LL15E;_LL15E: _tmpF4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpF3)->f2;goto _LL15D;}else{goto _LL159;}_LL15B: return({struct _tuple0 _tmpF6;
_tmpF6.f1=(void*)0;_tmpF6.f2=_tmpEE;_tmpF6;});_LL15D: {void*_tmpF7=_tmpE9;struct
Cyc_CfFlowInfo_Place*_tmpF8;_LL161: if((unsigned int)_tmpF7 > 1?*((int*)_tmpF7)== 
0: 0){_LL165: _tmpF8=((struct Cyc_CfFlowInfo_PlaceL_struct*)_tmpF7)->f1;goto _LL162;}
else{goto _LL163;}_LL163: if((int)_tmpF7 == 0){goto _LL164;}else{goto _LL160;}_LL162:
_tmpF5=Cyc_CfFlowInfo_assign_place(e->loc,_tmpF5,env->all_changed,_tmpF8,_tmpEE);
_tmpF4=Cyc_CfFlowInfo_reln_assign_exp(_tmpF4,_tmp9E,_tmp9C);_tmpF1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[
0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmpFA;_tmpFA.tag=0;_tmpFA.f1=_tmpF5;
_tmpFA.f2=_tmpF4;_tmpFA;});_tmpF9;});Cyc_NewControlFlow_update_tryflow(env,
_tmpF1);return({struct _tuple0 _tmpFB;_tmpFB.f1=_tmpF1;_tmpFB.f2=_tmpEE;_tmpFB;});
_LL164: return({struct _tuple0 _tmpFC;_tmpFC.f1=Cyc_NewControlFlow_use_Rval(env,
_tmp9C->loc,_tmpF1,_tmpEE);_tmpFC.f2=_tmpEE;_tmpFC;});_LL160:;}_LL159:;}}inflow=
_tmpF2;}}}}}}_LLCA: return Cyc_NewControlFlow_anal_Rexp(env,(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpA0)).f1,_tmp9F);_LLCC: {void*_tmpFE;void*_tmpFF;struct _tuple0
_tmpFD=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpA1);_LL168: _tmpFF=_tmpFD.f1;
goto _LL167;_LL167: _tmpFE=_tmpFD.f2;goto _LL166;_LL166: Cyc_NewControlFlow_use_Rval(
env,_tmpA1->loc,_tmpFF,_tmpFE);return({struct _tuple0 _tmp100;_tmp100.f1=(void*)0;
_tmp100.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp100;});}_LLCE: _tmpA2=({
struct Cyc_List_List*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->hd=_tmpA3;
_tmp101->tl=_tmpA2;_tmp101;});{struct Cyc_List_List*_tmp103;void*_tmp104;struct
_tuple4 _tmp102=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmpA2);_LL16B:
_tmp104=_tmp102.f1;goto _LL16A;_LL16A: _tmp103=_tmp102.f2;goto _LL169;_LL169: for(0;
_tmp103 != 0;(_tmp103=_tmp103->tl,_tmpA2=((struct Cyc_List_List*)_check_null(
_tmpA2))->tl)){_tmp104=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmpA2))->hd)->loc,_tmp104,(void*)_tmp103->hd);}
if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA3->topt))->v)){
return({struct _tuple0 _tmp105;_tmp105.f1=(void*)0;_tmp105.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp105;});}else{return({struct _tuple0 _tmp106;_tmp106.f1=_tmp104;_tmp106.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp106;});}}_LLD0: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp118;_tmp118.tag=1;_tmp118.f1=_tmpA6;_tmp118;});_tmp117;});struct Cyc_CfFlowInfo_Place*
place=({struct Cyc_CfFlowInfo_Place*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->root=(
void*)root;_tmp116->fields=0;_tmp116;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp115;_tmp115.tag=2;_tmp115.f1=place;_tmp115;});_tmp114;});void*place_val=
_tmpA5?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(*((void**)_check_null(_tmpA7)),Cyc_CfFlowInfo_unknown_none);
void*outflow;if(env->all_changed != 0){*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((
struct Cyc_Set_Set**)_check_null(env->all_changed)),place);}if(_tmpA8 != 0){struct
Cyc_List_List*_tmp109;void*_tmp10A;struct _tuple4 _tmp108=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp107[2];_tmp107[1]=_tmpA6;_tmp107[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpA8);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp107,sizeof(struct Cyc_Absyn_Exp*),2));}));_LL16E:
_tmp10A=_tmp108.f1;goto _LL16D;_LL16D: _tmp109=_tmp108.f2;goto _LL16C;_LL16C:
outflow=_tmp10A;}else{void*_tmp10C;struct _tuple0 _tmp10B=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpA6);_LL170: _tmp10C=_tmp10B.f1;goto _LL16F;_LL16F: outflow=_tmp10C;}{
void*_tmp10D=outflow;struct Cyc_List_List*_tmp10E;struct Cyc_Dict_Dict*_tmp10F;
_LL172: if((int)_tmp10D == 0){goto _LL173;}else{goto _LL174;}_LL174: if((unsigned int)
_tmp10D > 1?*((int*)_tmp10D)== 0: 0){_LL177: _tmp10F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp10D)->f1;goto _LL176;_LL176: _tmp10E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp10D)->f2;goto _LL175;}else{goto _LL171;}_LL173: return({struct _tuple0 _tmp110;
_tmp110.f1=outflow;_tmp110.f2=rval;_tmp110;});_LL175: return({struct _tuple0
_tmp111;_tmp111.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp112=
_cycalloc(sizeof(*_tmp112));_tmp112[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp113;_tmp113.tag=0;_tmp113.f1=Cyc_Dict_insert(_tmp10F,root,place_val);_tmp113.f2=
_tmp10E;_tmp113;});_tmp112;});_tmp111.f2=rval;_tmp111;});_LL171:;}}_LLD2: {void*
root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*_tmp12A=_cycalloc(sizeof(*
_tmp12A));_tmp12A[0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp12B;_tmp12B.tag=1;
_tmp12B.f1=_tmpAA;_tmp12B;});_tmp12A;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->root=(
void*)root;_tmp129->fields=0;_tmp129;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp128;_tmp128.tag=2;_tmp128.f1=place;_tmp128;});_tmp127;});if(env->all_changed
!= 0){*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);}{void*outflow;void*place_val;if(_tmpAB != 
0){struct Cyc_List_List*_tmp11B;void*_tmp11C;struct _tuple4 _tmp11A=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp119[2];_tmp119[1]=_tmpAA;_tmp119[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmpAB);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp119,sizeof(struct Cyc_Absyn_Exp*),2));}));_LL17A:
_tmp11C=_tmp11A.f1;goto _LL179;_LL179: _tmp11B=_tmp11A.f2;goto _LL178;_LL178:
outflow=_tmp11C;place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp11B))->tl))->hd;}else{void*_tmp11E;void*_tmp11F;struct _tuple0
_tmp11D=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmpAA);_LL17D: _tmp11F=_tmp11D.f1;
goto _LL17C;_LL17C: _tmp11E=_tmp11D.f2;goto _LL17B;_LL17B: outflow=_tmp11F;place_val=
_tmp11E;}{void*_tmp120=outflow;struct Cyc_List_List*_tmp121;struct Cyc_Dict_Dict*
_tmp122;_LL17F: if((int)_tmp120 == 0){goto _LL180;}else{goto _LL181;}_LL181: if((
unsigned int)_tmp120 > 1?*((int*)_tmp120)== 0: 0){_LL184: _tmp122=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp120)->f1;goto _LL183;_LL183: _tmp121=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp120)->f2;goto _LL182;}else{goto _LL17E;}_LL180: return({struct _tuple0 _tmp123;
_tmp123.f1=outflow;_tmp123.f2=rval;_tmp123;});_LL182: return({struct _tuple0
_tmp124;_tmp124.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp125=
_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp126;_tmp126.tag=0;_tmp126.f1=Cyc_Dict_insert(_tmp122,root,place_val);_tmp126.f2=
_tmp121;_tmp126;});_tmp125;});_tmp124.f2=rval;_tmp124;});_LL17E:;}}}_LLD4: {void*
_tmp12D;void*_tmp12E;struct _tuple0 _tmp12C=Cyc_NewControlFlow_anal_Lexp(env,
inflow,_tmpAC);_LL187: _tmp12E=_tmp12C.f1;goto _LL186;_LL186: _tmp12D=_tmp12C.f2;
goto _LL185;_LL185: {void*_tmp12F=_tmp12D;struct Cyc_CfFlowInfo_Place*_tmp130;
_LL189: if((int)_tmp12F == 0){goto _LL18A;}else{goto _LL18B;}_LL18B: if((unsigned int)
_tmp12F > 1?*((int*)_tmp12F)== 0: 0){_LL18D: _tmp130=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp12F)->f1;goto _LL18C;}else{goto _LL188;}_LL18A: return({struct _tuple0 _tmp131;
_tmp131.f1=_tmp12E;_tmp131.f2=(void*)1;_tmp131;});_LL18C: return({struct _tuple0
_tmp132;_tmp132.f1=_tmp12E;_tmp132.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp134;_tmp134.tag=2;_tmp134.f1=_tmp130;_tmp134;});_tmp133;});_tmp132;});_LL188:;}}
_LLD6: {void*_tmp136;void*_tmp137;struct _tuple0 _tmp135=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpAD);_LL190: _tmp137=_tmp135.f1;goto _LL18F;_LL18F: _tmp136=_tmp135.f2;
goto _LL18E;_LL18E: return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp137,_tmpAD,
_tmp136);}_LLD8: {void*_tmp139;void*_tmp13A;struct _tuple0 _tmp138=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpAF);_LL193: _tmp13A=_tmp138.f1;goto _LL192;_LL192: _tmp139=_tmp138.f2;
goto _LL191;_LL191: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAF->topt))->v)){return({struct _tuple0 _tmp13B;_tmp13B.f1=_tmp13A;
_tmp13B.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp13B;});}{void*_tmp13C=
_tmp139;struct Cyc_Dict_Dict*_tmp13D;_LL195: if((unsigned int)_tmp13C > 3?*((int*)
_tmp13C)== 3: 0){_LL199: _tmp13D=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp13C)->f1;
goto _LL196;}else{goto _LL197;}_LL197: goto _LL198;_LL196: return({struct _tuple0
_tmp13E;_tmp13E.f1=_tmp13A;_tmp13E.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmp13D,_tmpAE);_tmp13E;});_LL198:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp13F=_cycalloc(sizeof(*_tmp13F));
_tmp13F[0]=({struct Cyc_Core_Impossible_struct _tmp140;_tmp140.tag=Cyc_Core_Impossible;
_tmp140.f1=_tag_arr("anal_Rexp: AggrMember",sizeof(unsigned char),22);_tmp140;});
_tmp13F;}));_LL194:;}}_LLDA: {void*_tmp142;void*_tmp143;struct _tuple0 _tmp141=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmpB1);_LL19C: _tmp143=_tmp141.f1;goto _LL19B;_LL19B: _tmp142=_tmp141.f2;
goto _LL19A;_LL19A: {void*_tmp145;void*_tmp146;struct _tuple0 _tmp144=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp143,_tmpB1,_tmp142);_LL19F: _tmp146=_tmp144.f1;goto _LL19E;_LL19E:
_tmp145=_tmp144.f2;goto _LL19D;_LL19D:{void*_tmp147=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpB1->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp148;void*_tmp149;_LL1A1: if((unsigned int)_tmp147 > 3?*((int*)_tmp147)== 4: 0){
_LL1A5: _tmp148=((struct Cyc_Absyn_PointerType_struct*)_tmp147)->f1;_LL1A6: _tmp149=(
void*)_tmp148.elt_typ;goto _LL1A2;}else{goto _LL1A3;}_LL1A3: goto _LL1A4;_LL1A2: if(
Cyc_Absyn_is_union_type(_tmp149)){return({struct _tuple0 _tmp14A;_tmp14A.f1=
_tmp146;_tmp14A.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp14A;});}goto _LL1A0;
_LL1A4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp14B=_cycalloc(
sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_Core_Impossible_struct _tmp14C;_tmp14C.tag=
Cyc_Core_Impossible;_tmp14C.f1=_tag_arr("anal_Rexp: AggrArrow ptr",sizeof(
unsigned char),25);_tmp14C;});_tmp14B;}));_LL1A0:;}{void*_tmp14D=_tmp145;struct
Cyc_Dict_Dict*_tmp14E;_LL1A8: if((unsigned int)_tmp14D > 3?*((int*)_tmp14D)== 3: 0){
_LL1AC: _tmp14E=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp14D)->f1;goto _LL1A9;}
else{goto _LL1AA;}_LL1AA: goto _LL1AB;_LL1A9: return({struct _tuple0 _tmp14F;_tmp14F.f1=
_tmp146;_tmp14F.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp14E,_tmpB0);_tmp14F;});_LL1AB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Core_Impossible_struct
_tmp151;_tmp151.tag=Cyc_Core_Impossible;_tmp151.f1=_tag_arr("anal_Rexp: AggrArrow",
sizeof(unsigned char),21);_tmp151;});_tmp150;}));_LL1A7:;}}}_LLDC: {void*_tmp153;
void*_tmp154;struct _tuple0 _tmp152=Cyc_NewControlFlow_anal_test(env,inflow,_tmpB4);
_LL1AF: _tmp154=_tmp152.f1;goto _LL1AE;_LL1AE: _tmp153=_tmp152.f2;goto _LL1AD;_LL1AD: {
void*_tmp156;void*_tmp157;struct _tuple0 _tmp155=Cyc_NewControlFlow_anal_Rexp(env,
_tmp154,_tmpB3);_LL1B2: _tmp157=_tmp155.f1;goto _LL1B1;_LL1B1: _tmp156=_tmp155.f2;
goto _LL1B0;_LL1B0: {void*_tmp159;void*_tmp15A;struct _tuple0 _tmp158=Cyc_NewControlFlow_anal_Rexp(
env,_tmp153,_tmpB2);_LL1B5: _tmp15A=_tmp158.f1;goto _LL1B4;_LL1B4: _tmp159=_tmp158.f2;
goto _LL1B3;_LL1B3: return({struct _tuple0 _tmp15B;_tmp15B.f1=Cyc_CfFlowInfo_join_flow(
env->all_changed,Cyc_NewControlFlow_use_Rval(env,_tmpB3->loc,_tmp157,_tmp156),
Cyc_NewControlFlow_use_Rval(env,_tmpB2->loc,_tmp15A,_tmp159));_tmp15B.f2=Cyc_CfFlowInfo_unknown_all;
_tmp15B;});}}}_LLDE: {struct Cyc_List_List*_tmp15E;void*_tmp15F;struct _tuple4
_tmp15D=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({struct Cyc_Absyn_Exp*
_tmp15C[2];_tmp15C[1]=_tmpB5;_tmp15C[0]=_tmpB6;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp15C,sizeof(struct Cyc_Absyn_Exp*),2));}));
_LL1B8: _tmp15F=_tmp15D.f1;goto _LL1B7;_LL1B7: _tmp15E=_tmp15D.f2;goto _LL1B6;_LL1B6:{
void*_tmp160=_tmp15F;struct Cyc_List_List*_tmp161;struct Cyc_Dict_Dict*_tmp162;
_LL1BA: if((unsigned int)_tmp160 > 1?*((int*)_tmp160)== 0: 0){_LL1BF: _tmp162=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp160)->f1;goto _LL1BE;_LL1BE: _tmp161=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp160)->f2;goto _LL1BB;}else{goto
_LL1BC;}_LL1BC: goto _LL1BD;_LL1BB: if(Cyc_CfFlowInfo_initlevel(_tmp162,(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp15E))->tl))->hd)
== (void*)0){({void*_tmp163[0]={};Cyc_Tcutil_terr(_tmpB5->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp163,sizeof(void*),0));});}{struct Cyc_List_List*
_tmp164=Cyc_NewControlFlow_add_subscript_reln(_tmp161,_tmpB6,_tmpB5);if(_tmp161
!= _tmp164){_tmp15F=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp165=
_cycalloc(sizeof(*_tmp165));_tmp165[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp166;_tmp166.tag=0;_tmp166.f1=_tmp162;_tmp166.f2=_tmp164;_tmp166;});_tmp165;});}
goto _LL1B9;}_LL1BD: goto _LL1B9;_LL1B9:;}{void*_tmp167=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpB6->topt))->v);_LL1C1: if((unsigned int)
_tmp167 > 3?*((int*)_tmp167)== 9: 0){goto _LL1C2;}else{goto _LL1C3;}_LL1C3: goto
_LL1C4;_LL1C2: {void*_tmp168=(void*)((struct Cyc_List_List*)_check_null(_tmp15E))->hd;
struct Cyc_Dict_Dict*_tmp169;_LL1C6: if((unsigned int)_tmp168 > 3?*((int*)_tmp168)
== 3: 0){_LL1CA: _tmp169=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp168)->f1;
goto _LL1C7;}else{goto _LL1C8;}_LL1C8: goto _LL1C9;_LL1C7: return({struct _tuple0
_tmp16A;_tmp16A.f1=_tmp15F;_tmp16A.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmp169,Cyc_Absyn_fieldname((int)Cyc_Evexp_eval_const_uint_exp(
_tmpB5)));_tmp16A;});_LL1C9:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B[0]=({struct Cyc_Core_Impossible_struct
_tmp16C;_tmp16C.tag=Cyc_Core_Impossible;_tmp16C.f1=_tag_arr("anal_Rexp: Subscript",
sizeof(unsigned char),21);_tmp16C;});_tmp16B;}));_LL1C5:;}_LL1C4: goto _LL1C0;
_LL1C0:;}return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp15F,_tmpB6,(void*)((
struct Cyc_List_List*)_check_null(_tmp15E))->hd);}_LLE0: _tmpB8=_tmpB7;goto _LLE2;
_LLE2: {struct Cyc_List_List*_tmp16E;void*_tmp16F;struct _tuple4 _tmp16D=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmpB8);_LL1CD: _tmp16F=_tmp16D.f1;goto _LL1CC;_LL1CC: _tmp16E=_tmp16D.f2;
goto _LL1CB;_LL1CB: {struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{
int i=0;for(0;_tmp16E != 0;(_tmp16E=_tmp16E->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,Cyc_Absyn_fieldname(
i),(void*)_tmp16E->hd);}}return({struct _tuple0 _tmp170;_tmp170.f1=_tmp16F;_tmp170.f2=(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp171=_cycalloc(sizeof(*_tmp171));
_tmp171[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp172;_tmp172.tag=3;_tmp172.f1=
aggrdict;_tmp172;});_tmp171;});_tmp170;});}}_LLE4: _tmpBA=_tmpB9;goto _LLE6;_LLE6: {
struct Cyc_List_List*_tmp174;void*_tmp175;struct _tuple4 _tmp173=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple5*))
Cyc_Core_snd,_tmpBA));_LL1D0: _tmp175=_tmp173.f1;goto _LL1CF;_LL1CF: _tmp174=
_tmp173.f2;goto _LL1CE;_LL1CE: {struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{
int i=0;for(0;_tmp174 != 0;(((_tmp174=_tmp174->tl,_tmpBA=_tmpBA->tl)),++ i)){struct
Cyc_List_List*ds=(*((struct _tuple5*)((struct Cyc_List_List*)_check_null(_tmpBA))->hd)).f1;
for(0;ds != 0;ds=ds->tl){void*_tmp176=(void*)ds->hd;struct _tagged_arr*_tmp177;
_LL1D2: if(*((int*)_tmp176)== 0){goto _LL1D3;}else{goto _LL1D4;}_LL1D4: if(*((int*)
_tmp176)== 1){_LL1D6: _tmp177=((struct Cyc_Absyn_FieldName_struct*)_tmp176)->f1;
goto _LL1D5;}else{goto _LL1D1;}_LL1D3:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178[0]=({struct Cyc_Core_Impossible_struct
_tmp179;_tmp179.tag=Cyc_Core_Impossible;_tmp179.f1=_tag_arr("anal_Rexp:Struct_e",
sizeof(unsigned char),19);_tmp179;});_tmp178;}));_LL1D5: aggrdict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,
_tmp177,(void*)_tmp174->hd);_LL1D1:;}}}return({struct _tuple0 _tmp17A;_tmp17A.f1=
_tmp175;_tmp17A.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp17B=
_cycalloc(sizeof(*_tmp17B));_tmp17B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp17C;_tmp17C.tag=3;_tmp17C.f1=aggrdict;_tmp17C;});_tmp17B;});_tmp17A;});}}
_LLE8: {struct Cyc_List_List*_tmp17D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(
struct _tuple5*))Cyc_Core_snd,_tmpBB);struct Cyc_List_List*_tmp17F;void*_tmp180;
struct _tuple4 _tmp17E=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp17D);
_LL1D9: _tmp180=_tmp17E.f1;goto _LL1D8;_LL1D8: _tmp17F=_tmp17E.f2;goto _LL1D7;_LL1D7:
for(0;_tmp17F != 0;(_tmp17F=_tmp17F->tl,_tmp17D=_tmp17D->tl)){_tmp180=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp17D))->hd)->loc,
_tmp180,(void*)_tmp17F->hd);}return({struct _tuple0 _tmp181;_tmp181.f1=_tmp180;
_tmp181.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp181;});}_LLEA: {void*
_tmp183;void*_tmp184;struct _tuple0 _tmp182=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmpBD);_LL1DC: _tmp184=_tmp182.f1;goto _LL1DB;_LL1DB: _tmp183=_tmp182.f2;
goto _LL1DA;_LL1DA: {void*_tmp185=_tmp184;struct Cyc_List_List*_tmp186;struct Cyc_Dict_Dict*
_tmp187;_LL1DE: if((int)_tmp185 == 0){goto _LL1DF;}else{goto _LL1E0;}_LL1E0: if((
unsigned int)_tmp185 > 1?*((int*)_tmp185)== 0: 0){_LL1E3: _tmp187=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp185)->f1;goto _LL1E2;_LL1E2: _tmp186=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp185)->f2;goto _LL1E1;}else{goto _LL1DD;}_LL1DF: return({struct _tuple0 _tmp188;
_tmp188.f1=_tmp184;_tmp188.f2=Cyc_CfFlowInfo_unknown_all;_tmp188;});_LL1E1: if(
Cyc_CfFlowInfo_initlevel(_tmp187,_tmp183)== (void*)0){({void*_tmp189[0]={};Cyc_Tcutil_terr(
_tmpBD->loc,_tag_arr("expression may not be initialized",sizeof(unsigned char),
34),_tag_arr(_tmp189,sizeof(void*),0));});}{struct Cyc_List_List*new_relns=
_tmp186;comp_loop: {void*_tmp18A=(void*)_tmpBD->r;struct Cyc_Absyn_Exp*_tmp18B;
void*_tmp18C;struct Cyc_Absyn_Vardecl*_tmp18D;void*_tmp18E;struct Cyc_Absyn_Vardecl*
_tmp18F;void*_tmp190;struct Cyc_Absyn_Vardecl*_tmp191;void*_tmp192;struct Cyc_Absyn_Vardecl*
_tmp193;void*_tmp194;int _tmp195;struct Cyc_List_List*_tmp196;struct Cyc_List_List
_tmp197;struct Cyc_Absyn_Exp*_tmp198;void*_tmp199;_LL1E5: if(*((int*)_tmp18A)== 13){
_LL1F5: _tmp18B=((struct Cyc_Absyn_Cast_e_struct*)_tmp18A)->f2;goto _LL1E6;}else{
goto _LL1E7;}_LL1E7: if(*((int*)_tmp18A)== 1){_LL1F7: _tmp18C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp18A)->f2;if((unsigned int)_tmp18C > 1?*((int*)_tmp18C)== 0: 0){_LL1F8: _tmp18D=((
struct Cyc_Absyn_Global_b_struct*)_tmp18C)->f1;goto _LL1F6;}else{goto _LL1E9;}}
else{goto _LL1E9;}_LL1F6: if(! _tmp18D->escapes){goto _LL1E8;}else{goto _LL1E9;}
_LL1E9: if(*((int*)_tmp18A)== 1){_LL1FA: _tmp18E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp18A)->f2;if((unsigned int)_tmp18E > 1?*((int*)_tmp18E)== 3: 0){_LL1FB: _tmp18F=((
struct Cyc_Absyn_Local_b_struct*)_tmp18E)->f1;goto _LL1F9;}else{goto _LL1EB;}}else{
goto _LL1EB;}_LL1F9: if(! _tmp18F->escapes){goto _LL1EA;}else{goto _LL1EB;}_LL1EB: if(*((
int*)_tmp18A)== 1){_LL1FD: _tmp190=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp18A)->f2;
if((unsigned int)_tmp190 > 1?*((int*)_tmp190)== 4: 0){_LL1FE: _tmp191=((struct Cyc_Absyn_Pat_b_struct*)
_tmp190)->f1;goto _LL1FC;}else{goto _LL1ED;}}else{goto _LL1ED;}_LL1FC: if(! _tmp191->escapes){
goto _LL1EC;}else{goto _LL1ED;}_LL1ED: if(*((int*)_tmp18A)== 1){_LL200: _tmp192=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp18A)->f2;if((unsigned int)_tmp192 > 1?*((
int*)_tmp192)== 2: 0){_LL201: _tmp193=((struct Cyc_Absyn_Param_b_struct*)_tmp192)->f1;
goto _LL1FF;}else{goto _LL1EF;}}else{goto _LL1EF;}_LL1FF: if(! _tmp193->escapes){goto
_LL1EE;}else{goto _LL1EF;}_LL1EF: if(*((int*)_tmp18A)== 0){_LL202: _tmp194=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp18A)->f1;if((unsigned int)_tmp194 > 1?*((int*)
_tmp194)== 2: 0){_LL203: _tmp195=((struct Cyc_Absyn_Int_c_struct*)_tmp194)->f2;goto
_LL1F0;}else{goto _LL1F1;}}else{goto _LL1F1;}_LL1F1: if(*((int*)_tmp18A)== 3){
_LL206: _tmp199=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp18A)->f1;if((int)
_tmp199 == 19){goto _LL204;}else{goto _LL1F3;}_LL204: _tmp196=((struct Cyc_Absyn_Primop_e_struct*)
_tmp18A)->f2;if(_tmp196 == 0){goto _LL1F3;}else{_tmp197=*_tmp196;_LL205: _tmp198=(
struct Cyc_Absyn_Exp*)_tmp197.hd;goto _LL1F2;}}else{goto _LL1F3;}_LL1F3: goto _LL1F4;
_LL1E6: _tmpBD=_tmp18B;goto comp_loop;_LL1E8: _tmp18F=_tmp18D;goto _LL1EA;_LL1EA:
_tmp191=_tmp18F;goto _LL1EC;_LL1EC: _tmp193=_tmp191;goto _LL1EE;_LL1EE: new_relns=({
struct Cyc_List_List*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->vd=_tmpBE;_tmp19B->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessVar_struct*_tmp19C=_cycalloc(sizeof(*_tmp19C));
_tmp19C[0]=({struct Cyc_CfFlowInfo_LessVar_struct _tmp19D;_tmp19D.tag=1;_tmp19D.f1=
_tmp193;_tmp19D;});_tmp19C;}));_tmp19B;});_tmp19A->tl=_tmp186;_tmp19A;});goto
_LL1E4;_LL1F0: new_relns=({struct Cyc_List_List*_tmp19E=_cycalloc(sizeof(*_tmp19E));
_tmp19E->hd=({struct Cyc_CfFlowInfo_Reln*_tmp19F=_cycalloc(sizeof(*_tmp19F));
_tmp19F->vd=_tmpBE;_tmp19F->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp1A0=_cycalloc_atomic(sizeof(*_tmp1A0));_tmp1A0[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp1A1;_tmp1A1.tag=3;_tmp1A1.f1=(unsigned int)_tmp195;_tmp1A1;});_tmp1A0;}));
_tmp19F;});_tmp19E->tl=_tmp186;_tmp19E;});goto _LL1E4;_LL1F2:{void*_tmp1A2=(void*)
_tmp198->r;void*_tmp1A3;struct Cyc_Absyn_Vardecl*_tmp1A4;void*_tmp1A5;struct Cyc_Absyn_Vardecl*
_tmp1A6;void*_tmp1A7;struct Cyc_Absyn_Vardecl*_tmp1A8;void*_tmp1A9;struct Cyc_Absyn_Vardecl*
_tmp1AA;_LL208: if(*((int*)_tmp1A2)== 1){_LL213: _tmp1A3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A2)->f2;if((unsigned int)_tmp1A3 > 1?*((int*)_tmp1A3)== 0: 0){_LL214: _tmp1A4=((
struct Cyc_Absyn_Global_b_struct*)_tmp1A3)->f1;goto _LL212;}else{goto _LL20A;}}
else{goto _LL20A;}_LL212: if(! _tmp1A4->escapes){goto _LL209;}else{goto _LL20A;}
_LL20A: if(*((int*)_tmp1A2)== 1){_LL216: _tmp1A5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A2)->f2;if((unsigned int)_tmp1A5 > 1?*((int*)_tmp1A5)== 3: 0){_LL217: _tmp1A6=((
struct Cyc_Absyn_Local_b_struct*)_tmp1A5)->f1;goto _LL215;}else{goto _LL20C;}}else{
goto _LL20C;}_LL215: if(! _tmp1A6->escapes){goto _LL20B;}else{goto _LL20C;}_LL20C: if(*((
int*)_tmp1A2)== 1){_LL219: _tmp1A7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A2)->f2;
if((unsigned int)_tmp1A7 > 1?*((int*)_tmp1A7)== 4: 0){_LL21A: _tmp1A8=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1A7)->f1;goto _LL218;}else{goto _LL20E;}}else{goto _LL20E;}_LL218: if(! _tmp1A8->escapes){
goto _LL20D;}else{goto _LL20E;}_LL20E: if(*((int*)_tmp1A2)== 1){_LL21C: _tmp1A9=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1A2)->f2;if((unsigned int)_tmp1A9 > 1?*((
int*)_tmp1A9)== 2: 0){_LL21D: _tmp1AA=((struct Cyc_Absyn_Param_b_struct*)_tmp1A9)->f1;
goto _LL21B;}else{goto _LL210;}}else{goto _LL210;}_LL21B: if(! _tmp1AA->escapes){goto
_LL20F;}else{goto _LL210;}_LL210: goto _LL211;_LL209: _tmp1A6=_tmp1A4;goto _LL20B;
_LL20B: _tmp1A8=_tmp1A6;goto _LL20D;_LL20D: _tmp1AA=_tmp1A8;goto _LL20F;_LL20F:
new_relns=({struct Cyc_List_List*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->vd=_tmpBE;
_tmp1AC->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmp1AD=
_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp1AE;_tmp1AE.tag=2;_tmp1AE.f1=_tmp1AA;_tmp1AE;});_tmp1AD;}));_tmp1AC;});
_tmp1AB->tl=_tmp186;_tmp1AB;});goto _LL207;_LL211: goto _LL207;_LL207:;}goto _LL1E4;
_LL1F4: goto _LL1E4;_LL1E4:;}if(_tmp186 != new_relns){_tmp184=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp1B0;_tmp1B0.tag=0;_tmp1B0.f1=_tmp187;_tmp1B0.f2=new_relns;_tmp1B0;});_tmp1AF;});}{
void*_tmp1B1=_tmp183;_LL21F: if((int)_tmp1B1 == 0){goto _LL220;}else{goto _LL221;}
_LL221: if((int)_tmp1B1 == 2){goto _LL222;}else{goto _LL223;}_LL223: if((int)_tmp1B1
== 1){goto _LL224;}else{goto _LL225;}_LL225: if((unsigned int)_tmp1B1 > 3?*((int*)
_tmp1B1)== 2: 0){goto _LL226;}else{goto _LL227;}_LL227: goto _LL228;_LL220: return({
struct _tuple0 _tmp1B2;_tmp1B2.f1=_tmp184;_tmp1B2.f2=Cyc_CfFlowInfo_unknown_all;
_tmp1B2;});_LL222: goto _LL224;_LL224: goto _LL226;_LL226: {struct Cyc_List_List
_tmp1B3=({struct Cyc_List_List _tmp1BC;_tmp1BC.hd=_tmpBE;_tmp1BC.tl=0;_tmp1BC;});
_tmp184=Cyc_NewControlFlow_add_vars(_tmp184,(struct Cyc_List_List*)& _tmp1B3,Cyc_CfFlowInfo_unknown_all);{
void*_tmp1B5;void*_tmp1B6;struct _tuple0 _tmp1B4=Cyc_NewControlFlow_anal_Rexp(env,
_tmp184,_tmpBC);_LL22B: _tmp1B6=_tmp1B4.f1;goto _LL22A;_LL22A: _tmp1B5=_tmp1B4.f2;
goto _LL229;_LL229:{void*_tmp1B7=_tmp1B6;struct Cyc_Dict_Dict*_tmp1B8;_LL22D: if((
int)_tmp1B7 == 0){goto _LL22E;}else{goto _LL22F;}_LL22F: if((unsigned int)_tmp1B7 > 1?*((
int*)_tmp1B7)== 0: 0){_LL231: _tmp1B8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1B7)->f1;goto _LL230;}else{goto _LL22C;}_LL22E: return({struct _tuple0 _tmp1B9;
_tmp1B9.f1=_tmp1B6;_tmp1B9.f2=Cyc_CfFlowInfo_unknown_all;_tmp1B9;});_LL230: if(
Cyc_CfFlowInfo_initlevel(_tmp1B8,_tmp1B5)!= (void*)2){({void*_tmp1BA[0]={};Cyc_Tcutil_terr(
_tmpBD->loc,_tag_arr("expression may not be initialized",sizeof(unsigned char),
34),_tag_arr(_tmp1BA,sizeof(void*),0));});return({struct _tuple0 _tmp1BB;_tmp1BB.f1=(
void*)0;_tmp1BB.f2=Cyc_CfFlowInfo_unknown_all;_tmp1BB;});}_LL22C:;}_tmp184=
_tmp1B6;goto _LL228;}}_LL228: while(1){struct Cyc_List_List _tmp1BD=({struct Cyc_List_List
_tmp1C6;_tmp1C6.hd=_tmpBE;_tmp1C6.tl=0;_tmp1C6;});_tmp184=Cyc_NewControlFlow_add_vars(
_tmp184,(struct Cyc_List_List*)& _tmp1BD,Cyc_CfFlowInfo_unknown_all);{void*_tmp1BF;
void*_tmp1C0;struct _tuple0 _tmp1BE=Cyc_NewControlFlow_anal_Rexp(env,_tmp184,
_tmpBC);_LL234: _tmp1C0=_tmp1BE.f1;goto _LL233;_LL233: _tmp1BF=_tmp1BE.f2;goto
_LL232;_LL232:{void*_tmp1C1=_tmp1C0;struct Cyc_Dict_Dict*_tmp1C2;_LL236: if((int)
_tmp1C1 == 0){goto _LL237;}else{goto _LL238;}_LL238: if((unsigned int)_tmp1C1 > 1?*((
int*)_tmp1C1)== 0: 0){_LL23A: _tmp1C2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1C1)->f1;goto _LL239;}else{goto _LL235;}_LL237: goto _LL235;_LL239: if(Cyc_CfFlowInfo_initlevel(
_tmp1C2,_tmp1BF)!= (void*)2){({void*_tmp1C3[0]={};Cyc_Tcutil_terr(_tmpBD->loc,
_tag_arr("expression may not be initialized",sizeof(unsigned char),34),_tag_arr(
_tmp1C3,sizeof(void*),0));});return({struct _tuple0 _tmp1C4;_tmp1C4.f1=(void*)0;
_tmp1C4.f2=Cyc_CfFlowInfo_unknown_all;_tmp1C4;});}_LL235:;}{void*_tmp1C5=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp184,_tmp1C0);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1C5,
_tmp184)){break;}_tmp184=_tmp1C5;}}}return({struct _tuple0 _tmp1C7;_tmp1C7.f1=
_tmp184;_tmp1C7.f2=Cyc_CfFlowInfo_unknown_all;_tmp1C7;});_LL21E:;}}_LL1DD:;}}
_LLEC: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmpBF))->flow;{void*_tmp1C8=(void*)_tmpBF->r;struct Cyc_Absyn_Stmt*_tmp1C9;
struct Cyc_Absyn_Stmt*_tmp1CA;struct Cyc_Absyn_Stmt*_tmp1CB;struct Cyc_Absyn_Decl*
_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CD;_LL23C: if((unsigned int)_tmp1C8 > 1?*((int*)
_tmp1C8)== 1: 0){_LL245: _tmp1CA=((struct Cyc_Absyn_Seq_s_struct*)_tmp1C8)->f1;goto
_LL244;_LL244: _tmp1C9=((struct Cyc_Absyn_Seq_s_struct*)_tmp1C8)->f2;goto _LL23D;}
else{goto _LL23E;}_LL23E: if((unsigned int)_tmp1C8 > 1?*((int*)_tmp1C8)== 12: 0){
_LL247: _tmp1CC=((struct Cyc_Absyn_Decl_s_struct*)_tmp1C8)->f1;goto _LL246;_LL246:
_tmp1CB=((struct Cyc_Absyn_Decl_s_struct*)_tmp1C8)->f2;goto _LL23F;}else{goto
_LL240;}_LL240: if((unsigned int)_tmp1C8 > 1?*((int*)_tmp1C8)== 0: 0){_LL248:
_tmp1CD=((struct Cyc_Absyn_Exp_s_struct*)_tmp1C8)->f1;goto _LL241;}else{goto _LL242;}
_LL242: goto _LL243;_LL23D: inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp1CA);
_tmpBF=_tmp1C9;goto _LL23B;_LL23F: inflow=Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp1CC);_tmpBF=_tmp1CB;goto _LL23B;_LL241: return Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1CD);_LL243:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE[0]=({struct Cyc_Core_Impossible_struct
_tmp1CF;_tmp1CF.tag=Cyc_Core_Impossible;_tmp1CF.f1=_tag_arr("analyze_Rexp: ill-formed StmtExp",
sizeof(unsigned char),33);_tmp1CF;});_tmp1CE;}));_LL23B:;}}_LLEE: goto _LLF0;_LLF0:
goto _LLF2;_LLF2: goto _LLF4;_LLF4: goto _LLF6;_LLF6: goto _LLF8;_LLF8: goto _LLFA;_LLFA:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1D0=_cycalloc(sizeof(*
_tmp1D0));_tmp1D0[0]=({struct Cyc_Core_Impossible_struct _tmp1D1;_tmp1D1.tag=Cyc_Core_Impossible;
_tmp1D1.f1=_tag_arr("anal_Rexp, unexpected exp form",sizeof(unsigned char),31);
_tmp1D1;});_tmp1D0;}));_LL9A:;}}static struct _tuple0 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,
void*r,struct Cyc_List_List*flds){void*_tmp1D2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1D3;
struct Cyc_Absyn_Conref*_tmp1D4;void*_tmp1D5;_LL24A: if((unsigned int)_tmp1D2 > 3?*((
int*)_tmp1D2)== 4: 0){_LL24E: _tmp1D3=((struct Cyc_Absyn_PointerType_struct*)
_tmp1D2)->f1;_LL250: _tmp1D5=(void*)_tmp1D3.elt_typ;goto _LL24F;_LL24F: _tmp1D4=
_tmp1D3.bounds;goto _LL24B;}else{goto _LL24C;}_LL24C: goto _LL24D;_LL24B: {void*
_tmp1D6=f;struct Cyc_List_List*_tmp1D7;struct Cyc_Dict_Dict*_tmp1D8;_LL252: if((int)
_tmp1D6 == 0){goto _LL253;}else{goto _LL254;}_LL254: if((unsigned int)_tmp1D6 > 1?*((
int*)_tmp1D6)== 0: 0){_LL257: _tmp1D8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1D6)->f1;goto _LL256;_LL256: _tmp1D7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1D6)->f2;goto _LL255;}else{goto _LL251;}_LL253: return({struct _tuple0 _tmp1D9;
_tmp1D9.f1=f;_tmp1D9.f2=(void*)0;_tmp1D9;});_LL255: if(Cyc_Tcutil_is_bound_one(
_tmp1D4)){void*_tmp1DA=r;struct Cyc_CfFlowInfo_Place*_tmp1DB;struct Cyc_CfFlowInfo_Place
_tmp1DC;struct Cyc_List_List*_tmp1DD;void*_tmp1DE;void*_tmp1DF;_LL259: if((int)
_tmp1DA == 1){goto _LL25A;}else{goto _LL25B;}_LL25B: if((int)_tmp1DA == 2){goto _LL25C;}
else{goto _LL25D;}_LL25D: if((unsigned int)_tmp1DA > 3?*((int*)_tmp1DA)== 2: 0){
_LL265: _tmp1DB=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1DA)->f1;_tmp1DC=*
_tmp1DB;_LL267: _tmp1DE=(void*)_tmp1DC.root;goto _LL266;_LL266: _tmp1DD=_tmp1DC.fields;
goto _LL25E;}else{goto _LL25F;}_LL25F: if((int)_tmp1DA == 0){goto _LL260;}else{goto
_LL261;}_LL261: if((unsigned int)_tmp1DA > 3?*((int*)_tmp1DA)== 0: 0){_LL268:
_tmp1DF=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp1DA)->f1;goto _LL262;}
else{goto _LL263;}_LL263: goto _LL264;_LL25A: goto _LL25C;_LL25C:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp1E0=_cycalloc(sizeof(*
_tmp1E0));_tmp1E0[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp1E1;_tmp1E1.tag=
Cyc_CfFlowInfo_NotZero;_tmp1E1.f1=_tmp1D7;_tmp1E1;});_tmp1E0;})));goto _LL258;
_LL25E:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp1E3;_tmp1E3.tag=Cyc_CfFlowInfo_NotZero;_tmp1E3.f1=_tmp1D7;_tmp1E3;});_tmp1E2;})));
return({struct _tuple0 _tmp1E4;_tmp1E4.f1=f;_tmp1E4.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp1E6;_tmp1E6.tag=0;_tmp1E6.f1=({struct Cyc_CfFlowInfo_Place*_tmp1E7=_cycalloc(
sizeof(*_tmp1E7));_tmp1E7->root=(void*)_tmp1DE;_tmp1E7->fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1DD,flds);
_tmp1E7;});_tmp1E6;});_tmp1E5;});_tmp1E4;});_LL260:(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_IsZero_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));
_tmp1E8[0]=({struct Cyc_CfFlowInfo_IsZero_struct _tmp1E9;_tmp1E9.tag=Cyc_CfFlowInfo_IsZero;
_tmp1E9.f1=_tmp1D7;_tmp1E9;});_tmp1E8;})));return({struct _tuple0 _tmp1EA;_tmp1EA.f1=(
void*)0;_tmp1EA.f2=(void*)0;_tmp1EA;});_LL262: f=Cyc_NewControlFlow_notzero(env,
inflow,f,e,_tmp1DF);goto _LL264;_LL264:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_UnknownZ_struct*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmp1EC;_tmp1EC.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp1EC.f1=_tmp1D7;_tmp1EC;});_tmp1EB;})));_LL258:;}else{(void*)(e->annot=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));
_tmp1ED[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp1EE;_tmp1EE.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp1EE.f1=_tmp1D7;_tmp1EE;});_tmp1ED;})));}if(Cyc_CfFlowInfo_initlevel(_tmp1D8,
r)== (void*)0){({void*_tmp1EF[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of possibly uninitialized pointer",
sizeof(unsigned char),46),_tag_arr(_tmp1EF,sizeof(void*),0));});}return({struct
_tuple0 _tmp1F0;_tmp1F0.f1=f;_tmp1F0.f2=(void*)0;_tmp1F0;});_LL251:;}_LL24D:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1F1=_cycalloc(sizeof(*
_tmp1F1));_tmp1F1[0]=({struct Cyc_Core_Impossible_struct _tmp1F2;_tmp1F2.tag=Cyc_Core_Impossible;
_tmp1F2.f1=_tag_arr("left deref of non-pointer-type",sizeof(unsigned char),31);
_tmp1F2;});_tmp1F1;}));_LL249:;}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict*d;{void*_tmp1F3=inflow;struct Cyc_List_List*_tmp1F4;
struct Cyc_Dict_Dict*_tmp1F5;_LL26A: if((int)_tmp1F3 == 0){goto _LL26B;}else{goto
_LL26C;}_LL26C: if((unsigned int)_tmp1F3 > 1?*((int*)_tmp1F3)== 0: 0){_LL26F:
_tmp1F5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F3)->f1;goto _LL26E;
_LL26E: _tmp1F4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F3)->f2;goto
_LL26D;}else{goto _LL269;}_LL26B: return({struct _tuple0 _tmp1F6;_tmp1F6.f1=(void*)0;
_tmp1F6.f2=(void*)0;_tmp1F6;});_LL26D: d=_tmp1F5;_LL269:;}{void*_tmp1F7=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp1F8;struct Cyc_Absyn_Exp*_tmp1F9;struct Cyc_Absyn_Exp*
_tmp1FA;void*_tmp1FB;struct Cyc_Absyn_Vardecl*_tmp1FC;void*_tmp1FD;struct Cyc_Absyn_Vardecl*
_tmp1FE;void*_tmp1FF;struct Cyc_Absyn_Vardecl*_tmp200;void*_tmp201;struct Cyc_Absyn_Vardecl*
_tmp202;struct _tagged_arr*_tmp203;struct Cyc_Absyn_Exp*_tmp204;struct Cyc_Absyn_Exp*
_tmp205;struct Cyc_Absyn_Exp*_tmp206;struct Cyc_Absyn_Exp*_tmp207;struct
_tagged_arr*_tmp208;struct Cyc_Absyn_Exp*_tmp209;_LL271: if(*((int*)_tmp1F7)== 13){
_LL289: _tmp1F8=((struct Cyc_Absyn_Cast_e_struct*)_tmp1F7)->f2;goto _LL272;}else{
goto _LL273;}_LL273: if(*((int*)_tmp1F7)== 11){_LL28A: _tmp1F9=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1F7)->f1;goto _LL274;}else{goto _LL275;}_LL275: if(*((int*)_tmp1F7)== 12){
_LL28B: _tmp1FA=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1F7)->f1;goto _LL276;}
else{goto _LL277;}_LL277: if(*((int*)_tmp1F7)== 1){_LL28C: _tmp1FB=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp1F7)->f2;if((unsigned int)_tmp1FB > 1?*((int*)_tmp1FB)
== 2: 0){_LL28D: _tmp1FC=((struct Cyc_Absyn_Param_b_struct*)_tmp1FB)->f1;goto _LL278;}
else{goto _LL279;}}else{goto _LL279;}_LL279: if(*((int*)_tmp1F7)== 1){_LL28E:
_tmp1FD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F7)->f2;if((unsigned int)
_tmp1FD > 1?*((int*)_tmp1FD)== 3: 0){_LL28F: _tmp1FE=((struct Cyc_Absyn_Local_b_struct*)
_tmp1FD)->f1;goto _LL27A;}else{goto _LL27B;}}else{goto _LL27B;}_LL27B: if(*((int*)
_tmp1F7)== 1){_LL290: _tmp1FF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F7)->f2;
if((unsigned int)_tmp1FF > 1?*((int*)_tmp1FF)== 4: 0){_LL291: _tmp200=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1FF)->f1;goto _LL27C;}else{goto _LL27D;}}else{goto _LL27D;}_LL27D: if(*((int*)
_tmp1F7)== 1){_LL292: _tmp201=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1F7)->f2;
if((unsigned int)_tmp201 > 1?*((int*)_tmp201)== 0: 0){_LL293: _tmp202=((struct Cyc_Absyn_Global_b_struct*)
_tmp201)->f1;goto _LL27E;}else{goto _LL27F;}}else{goto _LL27F;}_LL27F: if(*((int*)
_tmp1F7)== 22){_LL295: _tmp204=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1F7)->f1;
goto _LL294;_LL294: _tmp203=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1F7)->f2;
goto _LL280;}else{goto _LL281;}_LL281: if(*((int*)_tmp1F7)== 20){_LL296: _tmp205=((
struct Cyc_Absyn_Deref_e_struct*)_tmp1F7)->f1;goto _LL282;}else{goto _LL283;}_LL283:
if(*((int*)_tmp1F7)== 23){_LL298: _tmp207=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1F7)->f1;goto _LL297;_LL297: _tmp206=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1F7)->f2;goto _LL284;}else{goto _LL285;}_LL285: if(*((int*)_tmp1F7)== 21){
_LL29A: _tmp209=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1F7)->f1;goto _LL299;
_LL299: _tmp208=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1F7)->f2;goto _LL286;}
else{goto _LL287;}_LL287: goto _LL288;_LL272: _tmp1F9=_tmp1F8;goto _LL274;_LL274:
_tmp1FA=_tmp1F9;goto _LL276;_LL276: return Cyc_NewControlFlow_anal_Lexp_rec(env,
inflow,_tmp1FA,flds);_LL278: _tmp1FE=_tmp1FC;goto _LL27A;_LL27A: _tmp200=_tmp1FE;
goto _LL27C;_LL27C: return({struct _tuple0 _tmp20A;_tmp20A.f1=inflow;_tmp20A.f2=(
void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp20B=_cycalloc(sizeof(*_tmp20B));
_tmp20B[0]=({struct Cyc_CfFlowInfo_PlaceL_struct _tmp20C;_tmp20C.tag=0;_tmp20C.f1=({
struct Cyc_CfFlowInfo_Place*_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D->root=(
void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp20E=_cycalloc(sizeof(*
_tmp20E));_tmp20E[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp20F;_tmp20F.tag=0;
_tmp20F.f1=_tmp200;_tmp20F;});_tmp20E;}));_tmp20D->fields=flds;_tmp20D;});
_tmp20C;});_tmp20B;});_tmp20A;});_LL27E: return({struct _tuple0 _tmp210;_tmp210.f1=
inflow;_tmp210.f2=(void*)0;_tmp210;});_LL280: if(! Cyc_Absyn_is_union_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp204->topt))->v)){flds=({struct Cyc_List_List*
_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->hd=_tmp203;_tmp211->tl=flds;_tmp211;});}
_tmp205=_tmp204;goto _LL282;_LL282: {void*_tmp213;void*_tmp214;struct _tuple0
_tmp212=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp205);_LL29D: _tmp214=_tmp212.f1;
goto _LL29C;_LL29C: _tmp213=_tmp212.f2;goto _LL29B;_LL29B: return Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp214,_tmp205,_tmp213,flds);}_LL284:{void*_tmp215=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp207->topt))->v);_LL29F: if((
unsigned int)_tmp215 > 3?*((int*)_tmp215)== 9: 0){goto _LL2A0;}else{goto _LL2A1;}
_LL2A1: goto _LL2A2;_LL2A0: {struct _tagged_arr*_tmp216=Cyc_Absyn_fieldname((int)
Cyc_Evexp_eval_const_uint_exp(_tmp206));return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp207,({struct Cyc_List_List*_tmp217=_cycalloc(sizeof(*_tmp217));
_tmp217->hd=_tmp216;_tmp217->tl=flds;_tmp217;}));}_LL2A2: goto _LL29E;_LL29E:;}{
struct Cyc_List_List*_tmp21A;void*_tmp21B;struct _tuple4 _tmp219=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp218[2];_tmp218[1]=_tmp206;_tmp218[0]=
_tmp207;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp218,sizeof(struct Cyc_Absyn_Exp*),2));}));_LL2A5: _tmp21B=_tmp219.f1;goto
_LL2A4;_LL2A4: _tmp21A=_tmp219.f2;goto _LL2A3;_LL2A3:{void*_tmp21C=_tmp21B;struct
Cyc_List_List*_tmp21D;struct Cyc_Dict_Dict*_tmp21E;_LL2A7: if((unsigned int)
_tmp21C > 1?*((int*)_tmp21C)== 0: 0){_LL2AC: _tmp21E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp21C)->f1;goto _LL2AB;_LL2AB: _tmp21D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp21C)->f2;goto _LL2A8;}else{goto _LL2A9;}_LL2A9: goto _LL2AA;_LL2A8: if(Cyc_CfFlowInfo_initlevel(
_tmp21E,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp21A))->tl))->hd)== (void*)0){({void*_tmp21F[0]={};Cyc_Tcutil_terr(
_tmp206->loc,_tag_arr("expression may not be initialized",sizeof(unsigned char),
34),_tag_arr(_tmp21F,sizeof(void*),0));});}{struct Cyc_List_List*_tmp220=Cyc_NewControlFlow_add_subscript_reln(
_tmp21D,_tmp207,_tmp206);if(_tmp21D != _tmp220){_tmp21B=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp222;_tmp222.tag=0;_tmp222.f1=_tmp21E;_tmp222.f2=_tmp220;_tmp222;});_tmp221;});}
goto _LL2A6;}_LL2AA: goto _LL2A6;_LL2A6:;}return Cyc_NewControlFlow_anal_derefL(env,
inflow,_tmp21B,_tmp207,(void*)((struct Cyc_List_List*)_check_null(_tmp21A))->hd,
flds);}_LL286: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp209->topt))->v)){return({struct _tuple0 _tmp223;_tmp223.f1=inflow;
_tmp223.f2=(void*)0;_tmp223;});}return Cyc_NewControlFlow_anal_Lexp_rec(env,
inflow,_tmp209,({struct Cyc_List_List*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->hd=
_tmp208;_tmp224->tl=flds;_tmp224;}));_LL288: return({struct _tuple0 _tmp225;_tmp225.f1=(
void*)0;_tmp225.f2=(void*)0;_tmp225;});_LL270:;}}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e){return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,e,0);}static struct _tuple0 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e){{void*
_tmp226=(void*)e->r;struct Cyc_Absyn_Exp*_tmp227;struct Cyc_Absyn_Exp*_tmp228;
struct Cyc_Absyn_Exp*_tmp229;struct Cyc_Absyn_Exp*_tmp22A;struct Cyc_Absyn_Exp*
_tmp22B;struct Cyc_List_List*_tmp22C;struct Cyc_List_List _tmp22D;struct Cyc_List_List*
_tmp22E;struct Cyc_Absyn_Exp*_tmp22F;void*_tmp230;struct Cyc_List_List*_tmp231;
void*_tmp232;struct Cyc_List_List*_tmp233;void*_tmp234;struct Cyc_List_List*
_tmp235;void*_tmp236;struct Cyc_List_List*_tmp237;void*_tmp238;_LL2AE: if(*((int*)
_tmp226)== 6){_LL2C0: _tmp229=((struct Cyc_Absyn_Conditional_e_struct*)_tmp226)->f1;
goto _LL2BF;_LL2BF: _tmp228=((struct Cyc_Absyn_Conditional_e_struct*)_tmp226)->f2;
goto _LL2BE;_LL2BE: _tmp227=((struct Cyc_Absyn_Conditional_e_struct*)_tmp226)->f3;
goto _LL2AF;}else{goto _LL2B0;}_LL2B0: if(*((int*)_tmp226)== 7){_LL2C2: _tmp22B=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp226)->f1;goto _LL2C1;_LL2C1: _tmp22A=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp226)->f2;goto _LL2B1;}else{goto _LL2B2;}_LL2B2: if(*((
int*)_tmp226)== 3){_LL2C6: _tmp230=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp226)->f1;if((int)_tmp230 == 11){goto _LL2C3;}else{goto _LL2B4;}_LL2C3: _tmp22C=((
struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f2;if(_tmp22C == 0){goto _LL2B4;}else{
_tmp22D=*_tmp22C;_LL2C5: _tmp22F=(struct Cyc_Absyn_Exp*)_tmp22D.hd;goto _LL2C4;
_LL2C4: _tmp22E=_tmp22D.tl;if(_tmp22E == 0){goto _LL2B3;}else{goto _LL2B4;}}}else{
goto _LL2B4;}_LL2B4: if(*((int*)_tmp226)== 3){_LL2C8: _tmp232=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp226)->f1;if((int)_tmp232 == 5){goto _LL2C7;}else{goto _LL2B6;}_LL2C7: _tmp231=((
struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f2;goto _LL2B5;}else{goto _LL2B6;}
_LL2B6: if(*((int*)_tmp226)== 3){_LL2CA: _tmp234=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp226)->f1;if((int)_tmp234 == 6){goto _LL2C9;}else{goto _LL2B8;}_LL2C9: _tmp233=((
struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f2;goto _LL2B7;}else{goto _LL2B8;}
_LL2B8: if(*((int*)_tmp226)== 3){_LL2CC: _tmp236=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp226)->f1;if((int)_tmp236 == 8){goto _LL2CB;}else{goto _LL2BA;}_LL2CB: _tmp235=((
struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f2;goto _LL2B9;}else{goto _LL2BA;}
_LL2BA: if(*((int*)_tmp226)== 3){_LL2CE: _tmp238=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp226)->f1;if((int)_tmp238 == 10){goto _LL2CD;}else{goto _LL2BC;}_LL2CD: _tmp237=((
struct Cyc_Absyn_Primop_e_struct*)_tmp226)->f2;goto _LL2BB;}else{goto _LL2BC;}
_LL2BC: goto _LL2BD;_LL2AF: {void*_tmp23A;void*_tmp23B;struct _tuple0 _tmp239=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp229);_LL2D1: _tmp23B=_tmp239.f1;goto _LL2D0;_LL2D0: _tmp23A=_tmp239.f2;
goto _LL2CF;_LL2CF: {void*_tmp23D;void*_tmp23E;struct _tuple0 _tmp23C=Cyc_NewControlFlow_anal_test(
env,_tmp23B,_tmp228);_LL2D4: _tmp23E=_tmp23C.f1;goto _LL2D3;_LL2D3: _tmp23D=_tmp23C.f2;
goto _LL2D2;_LL2D2: {void*_tmp240;void*_tmp241;struct _tuple0 _tmp23F=Cyc_NewControlFlow_anal_test(
env,_tmp23A,_tmp227);_LL2D7: _tmp241=_tmp23F.f1;goto _LL2D6;_LL2D6: _tmp240=_tmp23F.f2;
goto _LL2D5;_LL2D5: return({struct _tuple0 _tmp242;_tmp242.f1=Cyc_CfFlowInfo_join_flow(
env->all_changed,_tmp23E,_tmp241);_tmp242.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,
_tmp23D,_tmp240);_tmp242;});}}}_LL2B1: return Cyc_NewControlFlow_anal_test(env,(
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp22B)).f1,_tmp22A);_LL2B3: {void*
_tmp244;void*_tmp245;struct _tuple0 _tmp243=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp22F);_LL2DA: _tmp245=_tmp243.f1;goto _LL2D9;_LL2D9: _tmp244=_tmp243.f2;
goto _LL2D8;_LL2D8: return({struct _tuple0 _tmp246;_tmp246.f1=_tmp244;_tmp246.f2=
_tmp245;_tmp246;});}_LL2B5: {struct Cyc_List_List*_tmp248;void*_tmp249;struct
_tuple4 _tmp247=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp231);
_LL2DD: _tmp249=_tmp247.f1;goto _LL2DC;_LL2DC: _tmp248=_tmp247.f2;goto _LL2DB;_LL2DB: {
void*_tmp24A=_tmp249;struct Cyc_List_List*_tmp24B;struct Cyc_Dict_Dict*_tmp24C;
_LL2DF: if((int)_tmp24A == 0){goto _LL2E0;}else{goto _LL2E1;}_LL2E1: if((unsigned int)
_tmp24A > 1?*((int*)_tmp24A)== 0: 0){_LL2E4: _tmp24C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp24A)->f1;goto _LL2E3;_LL2E3: _tmp24B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp24A)->f2;goto _LL2E2;}else{goto _LL2DE;}_LL2E0: return({struct _tuple0 _tmp24D;
_tmp24D.f1=_tmp249;_tmp24D.f2=_tmp249;_tmp24D;});_LL2E2: {void*_tmp24E=(void*)((
struct Cyc_List_List*)_check_null(_tmp248))->hd;void*_tmp24F=(void*)((struct Cyc_List_List*)
_check_null(_tmp248->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp24C,_tmp24E)== (
void*)0){({void*_tmp250[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp231))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp250,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp24C,_tmp24F)== (void*)0){({void*_tmp251[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp231))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(unsigned char),34),_tag_arr(
_tmp251,sizeof(void*),0));});}{struct _tuple0 _tmp253=({struct _tuple0 _tmp252;
_tmp252.f1=_tmp24E;_tmp252.f2=_tmp24F;_tmp252;});void*_tmp254;void*_tmp255;void*
_tmp256;void*_tmp257;void*_tmp258;void*_tmp259;_LL2E6: _LL2ED: _tmp255=_tmp253.f1;
if((unsigned int)_tmp255 > 3?*((int*)_tmp255)== 0: 0){_LL2EE: _tmp256=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp255)->f1;goto _LL2EC;}else{goto _LL2E8;}
_LL2EC: _tmp254=_tmp253.f2;if((int)_tmp254 == 0){goto _LL2E7;}else{goto _LL2E8;}
_LL2E8: _LL2F1: _tmp259=_tmp253.f1;if((int)_tmp259 == 0){goto _LL2EF;}else{goto
_LL2EA;}_LL2EF: _tmp257=_tmp253.f2;if((unsigned int)_tmp257 > 3?*((int*)_tmp257)== 
0: 0){_LL2F0: _tmp258=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp257)->f1;
goto _LL2E9;}else{goto _LL2EA;}_LL2EA: goto _LL2EB;_LL2E7: {void*_tmp25B;void*
_tmp25C;struct _tuple0 _tmp25A=Cyc_NewControlFlow_splitzero(env,_tmp249,_tmp249,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp231))->hd,_tmp256);
_LL2F4: _tmp25C=_tmp25A.f1;goto _LL2F3;_LL2F3: _tmp25B=_tmp25A.f2;goto _LL2F2;_LL2F2:
return({struct _tuple0 _tmp25D;_tmp25D.f1=_tmp25B;_tmp25D.f2=_tmp25C;_tmp25D;});}
_LL2E9: {void*_tmp25F;void*_tmp260;struct _tuple0 _tmp25E=Cyc_NewControlFlow_splitzero(
env,_tmp249,_tmp249,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(_tmp231))->tl))->hd,_tmp258);_LL2F7: _tmp260=
_tmp25E.f1;goto _LL2F6;_LL2F6: _tmp25F=_tmp25E.f2;goto _LL2F5;_LL2F5: return({struct
_tuple0 _tmp261;_tmp261.f1=_tmp25F;_tmp261.f2=_tmp260;_tmp261;});}_LL2EB: return({
struct _tuple0 _tmp262;_tmp262.f1=_tmp249;_tmp262.f2=_tmp249;_tmp262;});_LL2E5:;}}
_LL2DE:;}}_LL2B7: {struct Cyc_List_List*_tmp264;void*_tmp265;struct _tuple4 _tmp263=
Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp233);_LL2FA: _tmp265=
_tmp263.f1;goto _LL2F9;_LL2F9: _tmp264=_tmp263.f2;goto _LL2F8;_LL2F8: {void*_tmp266=
_tmp265;struct Cyc_List_List*_tmp267;struct Cyc_Dict_Dict*_tmp268;_LL2FC: if((int)
_tmp266 == 0){goto _LL2FD;}else{goto _LL2FE;}_LL2FE: if((unsigned int)_tmp266 > 1?*((
int*)_tmp266)== 0: 0){_LL301: _tmp268=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp266)->f1;goto _LL300;_LL300: _tmp267=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp266)->f2;goto _LL2FF;}else{goto _LL2FB;}_LL2FD: return({struct _tuple0 _tmp269;
_tmp269.f1=_tmp265;_tmp269.f2=_tmp265;_tmp269;});_LL2FF: {void*_tmp26A=(void*)((
struct Cyc_List_List*)_check_null(_tmp264))->hd;void*_tmp26B=(void*)((struct Cyc_List_List*)
_check_null(_tmp264->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp268,_tmp26A)== (
void*)0){({void*_tmp26C[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp233))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp26C,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp268,_tmp26B)== (void*)0){({void*_tmp26D[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp233))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(unsigned char),34),_tag_arr(
_tmp26D,sizeof(void*),0));});}{struct _tuple0 _tmp26F=({struct _tuple0 _tmp26E;
_tmp26E.f1=_tmp26A;_tmp26E.f2=_tmp26B;_tmp26E;});void*_tmp270;void*_tmp271;void*
_tmp272;void*_tmp273;void*_tmp274;void*_tmp275;_LL303: _LL30A: _tmp271=_tmp26F.f1;
if((unsigned int)_tmp271 > 3?*((int*)_tmp271)== 0: 0){_LL30B: _tmp272=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp271)->f1;goto _LL309;}else{goto _LL305;}
_LL309: _tmp270=_tmp26F.f2;if((int)_tmp270 == 0){goto _LL304;}else{goto _LL305;}
_LL305: _LL30E: _tmp275=_tmp26F.f1;if((int)_tmp275 == 0){goto _LL30C;}else{goto
_LL307;}_LL30C: _tmp273=_tmp26F.f2;if((unsigned int)_tmp273 > 3?*((int*)_tmp273)== 
0: 0){_LL30D: _tmp274=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp273)->f1;
goto _LL306;}else{goto _LL307;}_LL307: goto _LL308;_LL304: return Cyc_NewControlFlow_splitzero(
env,_tmp265,_tmp265,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp233))->hd,_tmp272);_LL306: return Cyc_NewControlFlow_splitzero(env,_tmp265,
_tmp265,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp233))->tl))->hd,_tmp274);_LL308: return({struct _tuple0 _tmp276;
_tmp276.f1=_tmp265;_tmp276.f2=_tmp265;_tmp276;});_LL302:;}}_LL2FB:;}}_LL2B9: {
struct Cyc_List_List*_tmp278;void*_tmp279;struct _tuple4 _tmp277=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp235);_LL311: _tmp279=_tmp277.f1;goto _LL310;_LL310: _tmp278=_tmp277.f2;
goto _LL30F;_LL30F: {void*_tmp27A=_tmp279;struct Cyc_List_List*_tmp27B;struct Cyc_Dict_Dict*
_tmp27C;_LL313: if((int)_tmp27A == 0){goto _LL314;}else{goto _LL315;}_LL315: if((
unsigned int)_tmp27A > 1?*((int*)_tmp27A)== 0: 0){_LL318: _tmp27C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp27A)->f1;goto _LL317;_LL317: _tmp27B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp27A)->f2;goto _LL316;}else{goto _LL312;}_LL314: return({struct _tuple0 _tmp27D;
_tmp27D.f1=_tmp279;_tmp27D.f2=_tmp279;_tmp27D;});_LL316: {void*_tmp27E=(void*)((
struct Cyc_List_List*)_check_null(_tmp278))->hd;void*_tmp27F=(void*)((struct Cyc_List_List*)
_check_null(_tmp278->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp27C,_tmp27E)== (
void*)0){({void*_tmp280[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp235))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp280,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp27C,_tmp27F)== (void*)0){({void*_tmp281[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp235))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(unsigned char),34),_tag_arr(
_tmp281,sizeof(void*),0));});}{struct Cyc_Absyn_Exp*_tmp282=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp235))->hd;struct Cyc_Absyn_Exp*_tmp283=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp235->tl))->hd;void*
_tmp284=(void*)_tmp282->r;void*_tmp285;struct Cyc_Absyn_Vardecl*_tmp286;void*
_tmp287;struct Cyc_Absyn_Vardecl*_tmp288;void*_tmp289;struct Cyc_Absyn_Vardecl*
_tmp28A;void*_tmp28B;struct Cyc_Absyn_Vardecl*_tmp28C;_LL31A: if(*((int*)_tmp284)
== 1){_LL325: _tmp285=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp284)->f2;if((
unsigned int)_tmp285 > 1?*((int*)_tmp285)== 0: 0){_LL326: _tmp286=((struct Cyc_Absyn_Global_b_struct*)
_tmp285)->f1;goto _LL324;}else{goto _LL31C;}}else{goto _LL31C;}_LL324: if(! _tmp286->escapes){
goto _LL31B;}else{goto _LL31C;}_LL31C: if(*((int*)_tmp284)== 1){_LL328: _tmp287=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp284)->f2;if((unsigned int)_tmp287 > 1?*((
int*)_tmp287)== 3: 0){_LL329: _tmp288=((struct Cyc_Absyn_Local_b_struct*)_tmp287)->f1;
goto _LL327;}else{goto _LL31E;}}else{goto _LL31E;}_LL327: if(! _tmp288->escapes){goto
_LL31D;}else{goto _LL31E;}_LL31E: if(*((int*)_tmp284)== 1){_LL32B: _tmp289=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp284)->f2;if((unsigned int)_tmp289 > 1?*((int*)
_tmp289)== 4: 0){_LL32C: _tmp28A=((struct Cyc_Absyn_Pat_b_struct*)_tmp289)->f1;goto
_LL32A;}else{goto _LL320;}}else{goto _LL320;}_LL32A: if(! _tmp28A->escapes){goto
_LL31F;}else{goto _LL320;}_LL320: if(*((int*)_tmp284)== 1){_LL32E: _tmp28B=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp284)->f2;if((unsigned int)_tmp28B > 1?*((int*)
_tmp28B)== 2: 0){_LL32F: _tmp28C=((struct Cyc_Absyn_Param_b_struct*)_tmp28B)->f1;
goto _LL32D;}else{goto _LL322;}}else{goto _LL322;}_LL32D: if(! _tmp28C->escapes){goto
_LL321;}else{goto _LL322;}_LL322: goto _LL323;_LL31B: _tmp288=_tmp286;goto _LL31D;
_LL31D: _tmp28A=_tmp288;goto _LL31F;_LL31F: _tmp28C=_tmp28A;goto _LL321;_LL321: {
void*_tmp28D=(void*)_tmp283->r;void*_tmp28E;struct Cyc_Absyn_Vardecl*_tmp28F;void*
_tmp290;struct Cyc_Absyn_Vardecl*_tmp291;void*_tmp292;struct Cyc_Absyn_Vardecl*
_tmp293;void*_tmp294;struct Cyc_Absyn_Vardecl*_tmp295;void*_tmp296;int _tmp297;
struct Cyc_List_List*_tmp298;struct Cyc_List_List _tmp299;struct Cyc_Absyn_Exp*
_tmp29A;void*_tmp29B;_LL331: if(*((int*)_tmp28D)== 1){_LL340: _tmp28E=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp28D)->f2;if((unsigned int)_tmp28E > 1?*((int*)
_tmp28E)== 0: 0){_LL341: _tmp28F=((struct Cyc_Absyn_Global_b_struct*)_tmp28E)->f1;
goto _LL33F;}else{goto _LL333;}}else{goto _LL333;}_LL33F: if(! _tmp28F->escapes){goto
_LL332;}else{goto _LL333;}_LL333: if(*((int*)_tmp28D)== 1){_LL343: _tmp290=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp28D)->f2;if((unsigned int)_tmp290 > 1?*((int*)
_tmp290)== 3: 0){_LL344: _tmp291=((struct Cyc_Absyn_Local_b_struct*)_tmp290)->f1;
goto _LL342;}else{goto _LL335;}}else{goto _LL335;}_LL342: if(! _tmp291->escapes){goto
_LL334;}else{goto _LL335;}_LL335: if(*((int*)_tmp28D)== 1){_LL346: _tmp292=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp28D)->f2;if((unsigned int)_tmp292 > 1?*((int*)
_tmp292)== 4: 0){_LL347: _tmp293=((struct Cyc_Absyn_Pat_b_struct*)_tmp292)->f1;goto
_LL345;}else{goto _LL337;}}else{goto _LL337;}_LL345: if(! _tmp293->escapes){goto
_LL336;}else{goto _LL337;}_LL337: if(*((int*)_tmp28D)== 1){_LL349: _tmp294=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp28D)->f2;if((unsigned int)_tmp294 > 1?*((int*)
_tmp294)== 2: 0){_LL34A: _tmp295=((struct Cyc_Absyn_Param_b_struct*)_tmp294)->f1;
goto _LL348;}else{goto _LL339;}}else{goto _LL339;}_LL348: if(! _tmp295->escapes){goto
_LL338;}else{goto _LL339;}_LL339: if(*((int*)_tmp28D)== 0){_LL34B: _tmp296=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp28D)->f1;if((unsigned int)_tmp296 > 1?*((int*)
_tmp296)== 2: 0){_LL34C: _tmp297=((struct Cyc_Absyn_Int_c_struct*)_tmp296)->f2;goto
_LL33A;}else{goto _LL33B;}}else{goto _LL33B;}_LL33B: if(*((int*)_tmp28D)== 3){
_LL34F: _tmp29B=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp28D)->f1;if((int)
_tmp29B == 19){goto _LL34D;}else{goto _LL33D;}_LL34D: _tmp298=((struct Cyc_Absyn_Primop_e_struct*)
_tmp28D)->f2;if(_tmp298 == 0){goto _LL33D;}else{_tmp299=*_tmp298;_LL34E: _tmp29A=(
struct Cyc_Absyn_Exp*)_tmp299.hd;goto _LL33C;}}else{goto _LL33D;}_LL33D: goto _LL33E;
_LL332: _tmp291=_tmp28F;goto _LL334;_LL334: _tmp293=_tmp291;goto _LL336;_LL336:
_tmp295=_tmp293;goto _LL338;_LL338:{struct _tuple0 _tmp29D=({struct _tuple0 _tmp29C;
_tmp29C.f1=Cyc_Tcutil_compress((void*)_tmp28C->type);_tmp29C.f2=Cyc_Tcutil_compress((
void*)_tmp295->type);_tmp29C;});void*_tmp29E;void*_tmp29F;void*_tmp2A0;void*
_tmp2A1;void*_tmp2A2;void*_tmp2A3;_LL351: _LL357: _tmp29E=_tmp29D.f1;if((
unsigned int)_tmp29E > 3?*((int*)_tmp29E)== 5: 0){_LL359: _tmp2A0=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp29E)->f1;if((int)_tmp2A0 == 1){goto _LL358;}else{
goto _LL353;}_LL358: _tmp29F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp29E)->f2;
if((int)_tmp29F == 2){goto _LL352;}else{goto _LL353;}}else{goto _LL353;}_LL353:
_LL35A: _tmp2A1=_tmp29D.f2;if((unsigned int)_tmp2A1 > 3?*((int*)_tmp2A1)== 5: 0){
_LL35C: _tmp2A3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2A1)->f1;if((int)
_tmp2A3 == 1){goto _LL35B;}else{goto _LL355;}_LL35B: _tmp2A2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2A1)->f2;if((int)_tmp2A2 == 2){goto _LL354;}else{goto _LL355;}}else{goto _LL355;}
_LL355: goto _LL356;_LL352: goto _LL350;_LL354: goto _LL350;_LL356: return({struct
_tuple0 _tmp2A4;_tmp2A4.f1=_tmp279;_tmp2A4.f2=_tmp279;_tmp2A4;});_LL350:;}{struct
Cyc_List_List*_tmp2A5=({struct Cyc_List_List*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));
_tmp2A9->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));
_tmp2AA->vd=_tmp28C;_tmp2AA->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp2AC;_tmp2AC.tag=1;_tmp2AC.f1=_tmp295;_tmp2AC;});_tmp2AB;}));_tmp2AA;});
_tmp2A9->tl=_tmp27B;_tmp2A9;});return({struct _tuple0 _tmp2A6;_tmp2A6.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));
_tmp2A7[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2A8;_tmp2A8.tag=0;
_tmp2A8.f1=_tmp27C;_tmp2A8.f2=_tmp2A5;_tmp2A8;});_tmp2A7;});_tmp2A6.f2=_tmp279;
_tmp2A6;});}_LL33A:{struct _tuple0 _tmp2AE=({struct _tuple0 _tmp2AD;_tmp2AD.f1=Cyc_Tcutil_compress((
void*)_tmp28C->type);_tmp2AD.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp283->topt))->v);_tmp2AD;});void*_tmp2AF;void*_tmp2B0;void*
_tmp2B1;void*_tmp2B2;void*_tmp2B3;void*_tmp2B4;_LL35E: _LL364: _tmp2AF=_tmp2AE.f1;
if((unsigned int)_tmp2AF > 3?*((int*)_tmp2AF)== 5: 0){_LL366: _tmp2B1=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp2AF)->f1;if((int)_tmp2B1 == 1){goto _LL365;}
else{goto _LL360;}_LL365: _tmp2B0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2AF)->f2;
if((int)_tmp2B0 == 2){goto _LL35F;}else{goto _LL360;}}else{goto _LL360;}_LL360:
_LL367: _tmp2B2=_tmp2AE.f2;if((unsigned int)_tmp2B2 > 3?*((int*)_tmp2B2)== 5: 0){
_LL369: _tmp2B4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2B2)->f1;if((int)
_tmp2B4 == 1){goto _LL368;}else{goto _LL362;}_LL368: _tmp2B3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2B2)->f2;if((int)_tmp2B3 == 2){goto _LL361;}else{goto _LL362;}}else{goto _LL362;}
_LL362: goto _LL363;_LL35F: goto _LL35D;_LL361: goto _LL35D;_LL363: return({struct
_tuple0 _tmp2B5;_tmp2B5.f1=_tmp279;_tmp2B5.f2=_tmp279;_tmp2B5;});_LL35D:;}{struct
Cyc_List_List*_tmp2B6=({struct Cyc_List_List*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));
_tmp2BA->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));
_tmp2BB->vd=_tmp28C;_tmp2BB->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp2BC=_cycalloc_atomic(sizeof(*_tmp2BC));_tmp2BC[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp2BD;_tmp2BD.tag=3;_tmp2BD.f1=(unsigned int)_tmp297;_tmp2BD;});_tmp2BC;}));
_tmp2BB;});_tmp2BA->tl=_tmp27B;_tmp2BA;});return({struct _tuple0 _tmp2B7;_tmp2B7.f1=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2B8=_cycalloc(sizeof(*
_tmp2B8));_tmp2B8[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2B9;_tmp2B9.tag=
0;_tmp2B9.f1=_tmp27C;_tmp2B9.f2=_tmp2B6;_tmp2B9;});_tmp2B8;});_tmp2B7.f2=_tmp279;
_tmp2B7;});}_LL33C: {void*_tmp2BE=(void*)_tmp29A->r;void*_tmp2BF;struct Cyc_Absyn_Vardecl*
_tmp2C0;void*_tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C2;void*_tmp2C3;struct Cyc_Absyn_Vardecl*
_tmp2C4;void*_tmp2C5;struct Cyc_Absyn_Vardecl*_tmp2C6;_LL36B: if(*((int*)_tmp2BE)
== 1){_LL376: _tmp2BF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2BE)->f2;if((
unsigned int)_tmp2BF > 1?*((int*)_tmp2BF)== 0: 0){_LL377: _tmp2C0=((struct Cyc_Absyn_Global_b_struct*)
_tmp2BF)->f1;goto _LL375;}else{goto _LL36D;}}else{goto _LL36D;}_LL375: if(! _tmp2C0->escapes){
goto _LL36C;}else{goto _LL36D;}_LL36D: if(*((int*)_tmp2BE)== 1){_LL379: _tmp2C1=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2BE)->f2;if((unsigned int)_tmp2C1 > 1?*((
int*)_tmp2C1)== 3: 0){_LL37A: _tmp2C2=((struct Cyc_Absyn_Local_b_struct*)_tmp2C1)->f1;
goto _LL378;}else{goto _LL36F;}}else{goto _LL36F;}_LL378: if(! _tmp2C2->escapes){goto
_LL36E;}else{goto _LL36F;}_LL36F: if(*((int*)_tmp2BE)== 1){_LL37C: _tmp2C3=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2BE)->f2;if((unsigned int)_tmp2C3 > 1?*((int*)
_tmp2C3)== 4: 0){_LL37D: _tmp2C4=((struct Cyc_Absyn_Pat_b_struct*)_tmp2C3)->f1;goto
_LL37B;}else{goto _LL371;}}else{goto _LL371;}_LL37B: if(! _tmp2C4->escapes){goto
_LL370;}else{goto _LL371;}_LL371: if(*((int*)_tmp2BE)== 1){_LL37F: _tmp2C5=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2BE)->f2;if((unsigned int)_tmp2C5 > 1?*((int*)
_tmp2C5)== 2: 0){_LL380: _tmp2C6=((struct Cyc_Absyn_Param_b_struct*)_tmp2C5)->f1;
goto _LL37E;}else{goto _LL373;}}else{goto _LL373;}_LL37E: if(! _tmp2C6->escapes){goto
_LL372;}else{goto _LL373;}_LL373: goto _LL374;_LL36C: _tmp2C2=_tmp2C0;goto _LL36E;
_LL36E: _tmp2C4=_tmp2C2;goto _LL370;_LL370: _tmp2C6=_tmp2C4;goto _LL372;_LL372: {
struct Cyc_List_List*_tmp2C7=({struct Cyc_List_List*_tmp2CB=_cycalloc(sizeof(*
_tmp2CB));_tmp2CB->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2CC=_cycalloc(sizeof(*
_tmp2CC));_tmp2CC->vd=_tmp28C;_tmp2CC->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp2CE;_tmp2CE.tag=2;_tmp2CE.f1=_tmp2C6;_tmp2CE;});_tmp2CD;}));_tmp2CC;});
_tmp2CB->tl=_tmp27B;_tmp2CB;});return({struct _tuple0 _tmp2C8;_tmp2C8.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));
_tmp2C9[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2CA;_tmp2CA.tag=0;
_tmp2CA.f1=_tmp27C;_tmp2CA.f2=_tmp2C7;_tmp2CA;});_tmp2C9;});_tmp2C8.f2=_tmp279;
_tmp2C8;});}_LL374: return({struct _tuple0 _tmp2CF;_tmp2CF.f1=_tmp279;_tmp2CF.f2=
_tmp279;_tmp2CF;});_LL36A:;}_LL33E: return({struct _tuple0 _tmp2D0;_tmp2D0.f1=
_tmp279;_tmp2D0.f2=_tmp279;_tmp2D0;});_LL330:;}_LL323: return({struct _tuple0
_tmp2D1;_tmp2D1.f1=_tmp279;_tmp2D1.f2=_tmp279;_tmp2D1;});_LL319:;}}_LL312:;}}
_LL2BB: {struct Cyc_List_List*_tmp2D3;void*_tmp2D4;struct _tuple4 _tmp2D2=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp237);_LL383: _tmp2D4=_tmp2D2.f1;goto _LL382;_LL382: _tmp2D3=_tmp2D2.f2;
goto _LL381;_LL381: {void*_tmp2D5=_tmp2D4;struct Cyc_List_List*_tmp2D6;struct Cyc_Dict_Dict*
_tmp2D7;_LL385: if((int)_tmp2D5 == 0){goto _LL386;}else{goto _LL387;}_LL387: if((
unsigned int)_tmp2D5 > 1?*((int*)_tmp2D5)== 0: 0){_LL38A: _tmp2D7=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2D5)->f1;goto _LL389;_LL389: _tmp2D6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2D5)->f2;goto _LL388;}else{goto _LL384;}_LL386: return({struct _tuple0 _tmp2D8;
_tmp2D8.f1=_tmp2D4;_tmp2D8.f2=_tmp2D4;_tmp2D8;});_LL388: {void*_tmp2D9=(void*)((
struct Cyc_List_List*)_check_null(_tmp2D3))->hd;void*_tmp2DA=(void*)((struct Cyc_List_List*)
_check_null(_tmp2D3->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp2D7,_tmp2D9)== (
void*)0){({void*_tmp2DB[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp237))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp2DB,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp2D7,_tmp2DA)== (void*)0){({void*_tmp2DC[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp237))->tl))->hd)->loc,
_tag_arr("expression may not be initialized",sizeof(unsigned char),34),_tag_arr(
_tmp2DC,sizeof(void*),0));});}{struct Cyc_Absyn_Exp*_tmp2DD=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp237))->hd;struct Cyc_Absyn_Exp*_tmp2DE=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp237->tl))->hd;void*
_tmp2DF=(void*)_tmp2DD->r;void*_tmp2E0;struct Cyc_Absyn_Vardecl*_tmp2E1;void*
_tmp2E2;struct Cyc_Absyn_Vardecl*_tmp2E3;void*_tmp2E4;struct Cyc_Absyn_Vardecl*
_tmp2E5;void*_tmp2E6;struct Cyc_Absyn_Vardecl*_tmp2E7;_LL38C: if(*((int*)_tmp2DF)
== 1){_LL397: _tmp2E0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2DF)->f2;if((
unsigned int)_tmp2E0 > 1?*((int*)_tmp2E0)== 0: 0){_LL398: _tmp2E1=((struct Cyc_Absyn_Global_b_struct*)
_tmp2E0)->f1;goto _LL396;}else{goto _LL38E;}}else{goto _LL38E;}_LL396: if(! _tmp2E1->escapes){
goto _LL38D;}else{goto _LL38E;}_LL38E: if(*((int*)_tmp2DF)== 1){_LL39A: _tmp2E2=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2DF)->f2;if((unsigned int)_tmp2E2 > 1?*((
int*)_tmp2E2)== 3: 0){_LL39B: _tmp2E3=((struct Cyc_Absyn_Local_b_struct*)_tmp2E2)->f1;
goto _LL399;}else{goto _LL390;}}else{goto _LL390;}_LL399: if(! _tmp2E3->escapes){goto
_LL38F;}else{goto _LL390;}_LL390: if(*((int*)_tmp2DF)== 1){_LL39D: _tmp2E4=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2DF)->f2;if((unsigned int)_tmp2E4 > 1?*((int*)
_tmp2E4)== 4: 0){_LL39E: _tmp2E5=((struct Cyc_Absyn_Pat_b_struct*)_tmp2E4)->f1;goto
_LL39C;}else{goto _LL392;}}else{goto _LL392;}_LL39C: if(! _tmp2E5->escapes){goto
_LL391;}else{goto _LL392;}_LL392: if(*((int*)_tmp2DF)== 1){_LL3A0: _tmp2E6=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2DF)->f2;if((unsigned int)_tmp2E6 > 1?*((int*)
_tmp2E6)== 2: 0){_LL3A1: _tmp2E7=((struct Cyc_Absyn_Param_b_struct*)_tmp2E6)->f1;
goto _LL39F;}else{goto _LL394;}}else{goto _LL394;}_LL39F: if(! _tmp2E7->escapes){goto
_LL393;}else{goto _LL394;}_LL394: goto _LL395;_LL38D: _tmp2E3=_tmp2E1;goto _LL38F;
_LL38F: _tmp2E5=_tmp2E3;goto _LL391;_LL391: _tmp2E7=_tmp2E5;goto _LL393;_LL393: {
void*_tmp2E8=(void*)_tmp2DE->r;struct Cyc_List_List*_tmp2E9;struct Cyc_List_List
_tmp2EA;struct Cyc_Absyn_Exp*_tmp2EB;void*_tmp2EC;_LL3A3: if(*((int*)_tmp2E8)== 3){
_LL3A9: _tmp2EC=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2E8)->f1;if((int)
_tmp2EC == 19){goto _LL3A7;}else{goto _LL3A5;}_LL3A7: _tmp2E9=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2E8)->f2;if(_tmp2E9 == 0){goto _LL3A5;}else{_tmp2EA=*_tmp2E9;_LL3A8: _tmp2EB=(
struct Cyc_Absyn_Exp*)_tmp2EA.hd;goto _LL3A4;}}else{goto _LL3A5;}_LL3A5: goto _LL3A6;
_LL3A4: {void*_tmp2ED=(void*)_tmp2EB->r;void*_tmp2EE;struct Cyc_Absyn_Vardecl*
_tmp2EF;void*_tmp2F0;struct Cyc_Absyn_Vardecl*_tmp2F1;void*_tmp2F2;struct Cyc_Absyn_Vardecl*
_tmp2F3;void*_tmp2F4;struct Cyc_Absyn_Vardecl*_tmp2F5;_LL3AB: if(*((int*)_tmp2ED)
== 1){_LL3B6: _tmp2EE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2ED)->f2;if((
unsigned int)_tmp2EE > 1?*((int*)_tmp2EE)== 0: 0){_LL3B7: _tmp2EF=((struct Cyc_Absyn_Global_b_struct*)
_tmp2EE)->f1;goto _LL3B5;}else{goto _LL3AD;}}else{goto _LL3AD;}_LL3B5: if(! _tmp2EF->escapes){
goto _LL3AC;}else{goto _LL3AD;}_LL3AD: if(*((int*)_tmp2ED)== 1){_LL3B9: _tmp2F0=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2ED)->f2;if((unsigned int)_tmp2F0 > 1?*((
int*)_tmp2F0)== 3: 0){_LL3BA: _tmp2F1=((struct Cyc_Absyn_Local_b_struct*)_tmp2F0)->f1;
goto _LL3B8;}else{goto _LL3AF;}}else{goto _LL3AF;}_LL3B8: if(! _tmp2F1->escapes){goto
_LL3AE;}else{goto _LL3AF;}_LL3AF: if(*((int*)_tmp2ED)== 1){_LL3BC: _tmp2F2=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2ED)->f2;if((unsigned int)_tmp2F2 > 1?*((int*)
_tmp2F2)== 4: 0){_LL3BD: _tmp2F3=((struct Cyc_Absyn_Pat_b_struct*)_tmp2F2)->f1;goto
_LL3BB;}else{goto _LL3B1;}}else{goto _LL3B1;}_LL3BB: if(! _tmp2F3->escapes){goto
_LL3B0;}else{goto _LL3B1;}_LL3B1: if(*((int*)_tmp2ED)== 1){_LL3BF: _tmp2F4=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2ED)->f2;if((unsigned int)_tmp2F4 > 1?*((int*)
_tmp2F4)== 2: 0){_LL3C0: _tmp2F5=((struct Cyc_Absyn_Param_b_struct*)_tmp2F4)->f1;
goto _LL3BE;}else{goto _LL3B3;}}else{goto _LL3B3;}_LL3BE: if(! _tmp2F5->escapes){goto
_LL3B2;}else{goto _LL3B3;}_LL3B3: goto _LL3B4;_LL3AC: _tmp2F1=_tmp2EF;goto _LL3AE;
_LL3AE: _tmp2F3=_tmp2F1;goto _LL3B0;_LL3B0: _tmp2F5=_tmp2F3;goto _LL3B2;_LL3B2: {
struct Cyc_List_List*_tmp2F6=({struct Cyc_List_List*_tmp2FA=_cycalloc(sizeof(*
_tmp2FA));_tmp2FA->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2FB=_cycalloc(sizeof(*
_tmp2FB));_tmp2FB->vd=_tmp2E7;_tmp2FB->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp2FD;_tmp2FD.tag=4;_tmp2FD.f1=_tmp2F5;_tmp2FD;});_tmp2FC;}));_tmp2FB;});
_tmp2FA->tl=_tmp2D6;_tmp2FA;});return({struct _tuple0 _tmp2F7;_tmp2F7.f1=(void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));
_tmp2F8[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp2F9;_tmp2F9.tag=0;
_tmp2F9.f1=_tmp2D7;_tmp2F9.f2=_tmp2F6;_tmp2F9;});_tmp2F8;});_tmp2F7.f2=_tmp2D4;
_tmp2F7;});}_LL3B4: return({struct _tuple0 _tmp2FE;_tmp2FE.f1=_tmp2D4;_tmp2FE.f2=
_tmp2D4;_tmp2FE;});_LL3AA:;}_LL3A6: return({struct _tuple0 _tmp2FF;_tmp2FF.f1=
_tmp2D4;_tmp2FF.f2=_tmp2D4;_tmp2FF;});_LL3A2:;}_LL395: return({struct _tuple0
_tmp300;_tmp300.f1=_tmp2D4;_tmp300.f2=_tmp2D4;_tmp300;});_LL38B:;}}_LL384:;}}
_LL2BD: goto _LL2AD;_LL2AD:;}{void*_tmp302;void*_tmp303;struct _tuple0 _tmp301=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_LL3C3: _tmp303=_tmp301.f1;goto _LL3C2;_LL3C2: _tmp302=_tmp301.f2;
goto _LL3C1;_LL3C1: {void*_tmp304=_tmp303;struct Cyc_List_List*_tmp305;struct Cyc_Dict_Dict*
_tmp306;_LL3C5: if((int)_tmp304 == 0){goto _LL3C6;}else{goto _LL3C7;}_LL3C7: if((
unsigned int)_tmp304 > 1?*((int*)_tmp304)== 0: 0){_LL3CA: _tmp306=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp304)->f1;goto _LL3C9;_LL3C9: _tmp305=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp304)->f2;goto _LL3C8;}else{goto _LL3C4;}_LL3C6: return({struct _tuple0 _tmp307;
_tmp307.f1=_tmp303;_tmp307.f2=_tmp303;_tmp307;});_LL3C8: {void*_tmp308=_tmp302;
void*_tmp309;void*_tmp30A;void*_tmp30B;_LL3CC: if((int)_tmp308 == 0){goto _LL3CD;}
else{goto _LL3CE;}_LL3CE: if((int)_tmp308 == 2){goto _LL3CF;}else{goto _LL3D0;}_LL3D0:
if((int)_tmp308 == 1){goto _LL3D1;}else{goto _LL3D2;}_LL3D2: if((unsigned int)
_tmp308 > 3?*((int*)_tmp308)== 2: 0){goto _LL3D3;}else{goto _LL3D4;}_LL3D4: if((
unsigned int)_tmp308 > 3?*((int*)_tmp308)== 0: 0){_LL3DE: _tmp309=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp308)->f1;if((int)_tmp309 == 0){goto _LL3D5;}
else{goto _LL3D6;}}else{goto _LL3D6;}_LL3D6: if((unsigned int)_tmp308 > 3?*((int*)
_tmp308)== 1: 0){_LL3DF: _tmp30A=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp308)->f1;
if((int)_tmp30A == 0){goto _LL3D7;}else{goto _LL3D8;}}else{goto _LL3D8;}_LL3D8: if((
unsigned int)_tmp308 > 3?*((int*)_tmp308)== 0: 0){_LL3E0: _tmp30B=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp308)->f1;goto _LL3D9;}else{goto _LL3DA;}_LL3DA:
if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 1: 0){goto _LL3DB;}else{goto _LL3DC;}
_LL3DC: if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 3: 0){goto _LL3DD;}else{goto
_LL3CB;}_LL3CD: return({struct _tuple0 _tmp30C;_tmp30C.f1=(void*)0;_tmp30C.f2=
_tmp303;_tmp30C;});_LL3CF: goto _LL3D1;_LL3D1: goto _LL3D3;_LL3D3: return({struct
_tuple0 _tmp30D;_tmp30D.f1=_tmp303;_tmp30D.f2=(void*)0;_tmp30D;});_LL3D5: goto
_LL3D7;_LL3D7:({void*_tmp30E[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp30E,sizeof(void*),0));});return({struct
_tuple0 _tmp30F;_tmp30F.f1=(void*)0;_tmp30F.f2=(void*)0;_tmp30F;});_LL3D9: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp303,e,_tmp30B);_LL3DB: return({struct
_tuple0 _tmp310;_tmp310.f1=_tmp303;_tmp310.f2=_tmp303;_tmp310;});_LL3DD:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp311=_cycalloc(sizeof(*
_tmp311));_tmp311[0]=({struct Cyc_Core_Impossible_struct _tmp312;_tmp312.tag=Cyc_Core_Impossible;
_tmp312.f1=_tag_arr("anal_test",sizeof(unsigned char),10);_tmp312;});_tmp311;}));
_LL3CB:;}_LL3C4:;}}}static void*Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause
_tmp314;struct Cyc_Absyn_Stmt*_tmp315;struct Cyc_Absyn_Exp*_tmp316;struct Cyc_Core_Opt*
_tmp317;struct Cyc_Absyn_Switch_clause*_tmp313=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp314=*_tmp313;_LL3E4: _tmp317=_tmp314.pat_vars;goto _LL3E3;_LL3E3:
_tmp316=_tmp314.where_clause;goto _LL3E2;_LL3E2: _tmp315=_tmp314.body;goto _LL3E1;
_LL3E1: {void*clause_inflow=Cyc_NewControlFlow_add_vars(inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp317))->v,Cyc_CfFlowInfo_unknown_all);void*
clause_outflow;if(_tmp316 != 0){void*_tmp319;void*_tmp31A;struct _tuple0 _tmp318=
Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)_check_null(
_tmp316));_LL3E7: _tmp31A=_tmp318.f1;goto _LL3E6;_LL3E6: _tmp319=_tmp318.f2;goto
_LL3E5;_LL3E5: inflow=_tmp319;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
_tmp31A,_tmp315);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp315);}{void*_tmp31B=clause_outflow;_LL3E9: if((int)_tmp31B == 0){
goto _LL3EA;}else{goto _LL3EB;}_LL3EB: goto _LL3EC;_LL3EA: goto _LL3E8;_LL3EC: if(scs->tl
== 0){return clause_outflow;}else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(
scs->tl))->hd)->pat_vars))->v != 0){({void*_tmp31C[0]={};Cyc_Tcutil_terr(_tmp315->loc,
_tag_arr("switch clause may implicitly fallthru",sizeof(unsigned char),38),
_tag_arr(_tmp31C,sizeof(void*),0));});}else{({void*_tmp31D[0]={};Cyc_Tcutil_warn(
_tmp315->loc,_tag_arr("switch clause may implicitly fallthru",sizeof(
unsigned char),38),_tag_arr(_tmp31D,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL3E8;_LL3E8:;}}}return(void*)0;}static void*Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp31E=Cyc_NewControlFlow_pre_stmt_check(env,
inflow,s);inflow=(void*)_tmp31E->flow;{void*_tmp31F=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp320;struct Cyc_Absyn_Exp*_tmp321;struct Cyc_Absyn_Exp*_tmp322;struct Cyc_Absyn_Stmt*
_tmp323;struct Cyc_Absyn_Stmt*_tmp324;struct Cyc_Absyn_Stmt*_tmp325;struct Cyc_Absyn_Stmt*
_tmp326;struct Cyc_Absyn_Exp*_tmp327;struct Cyc_Absyn_Stmt*_tmp328;struct _tuple3
_tmp329;struct Cyc_Absyn_Stmt*_tmp32A;struct Cyc_Absyn_Exp*_tmp32B;struct _tuple3
_tmp32C;struct Cyc_Absyn_Stmt*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_Stmt*
_tmp32F;struct Cyc_Absyn_Stmt*_tmp330;struct _tuple3 _tmp331;struct Cyc_Absyn_Stmt*
_tmp332;struct Cyc_Absyn_Exp*_tmp333;struct _tuple3 _tmp334;struct Cyc_Absyn_Stmt*
_tmp335;struct Cyc_Absyn_Exp*_tmp336;struct Cyc_Absyn_Exp*_tmp337;struct Cyc_Absyn_Stmt*
_tmp338;struct Cyc_Absyn_Switch_clause**_tmp339;struct Cyc_Absyn_Switch_clause*
_tmp33A;struct Cyc_List_List*_tmp33B;struct Cyc_Absyn_Stmt*_tmp33C;struct Cyc_Absyn_Stmt*
_tmp33D;struct Cyc_Absyn_Stmt*_tmp33E;struct Cyc_List_List*_tmp33F;struct Cyc_Absyn_Exp*
_tmp340;struct Cyc_List_List*_tmp341;struct Cyc_Absyn_Stmt*_tmp342;struct Cyc_Absyn_Stmt*
_tmp343;struct Cyc_Absyn_Decl*_tmp344;struct Cyc_Absyn_Stmt*_tmp345;struct Cyc_Absyn_Stmt*
_tmp346;struct Cyc_Absyn_Vardecl*_tmp347;struct Cyc_Absyn_Tvar*_tmp348;_LL3EE: if((
int)_tmp31F == 0){goto _LL3EF;}else{goto _LL3F0;}_LL3F0: if((unsigned int)_tmp31F > 1?*((
int*)_tmp31F)== 2: 0){_LL416: _tmp320=((struct Cyc_Absyn_Return_s_struct*)_tmp31F)->f1;
if(_tmp320 == 0){goto _LL3F1;}else{goto _LL3F2;}}else{goto _LL3F2;}_LL3F2: if((
unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 2: 0){_LL417: _tmp321=((struct Cyc_Absyn_Return_s_struct*)
_tmp31F)->f1;goto _LL3F3;}else{goto _LL3F4;}_LL3F4: if((unsigned int)_tmp31F > 1?*((
int*)_tmp31F)== 0: 0){_LL418: _tmp322=((struct Cyc_Absyn_Exp_s_struct*)_tmp31F)->f1;
goto _LL3F5;}else{goto _LL3F6;}_LL3F6: if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)
== 1: 0){_LL41A: _tmp324=((struct Cyc_Absyn_Seq_s_struct*)_tmp31F)->f1;goto _LL419;
_LL419: _tmp323=((struct Cyc_Absyn_Seq_s_struct*)_tmp31F)->f2;goto _LL3F7;}else{
goto _LL3F8;}_LL3F8: if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 3: 0){_LL41D:
_tmp327=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp31F)->f1;goto _LL41C;_LL41C:
_tmp326=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp31F)->f2;goto _LL41B;_LL41B:
_tmp325=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp31F)->f3;goto _LL3F9;}else{
goto _LL3FA;}_LL3FA: if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 4: 0){_LL41F:
_tmp329=((struct Cyc_Absyn_While_s_struct*)_tmp31F)->f1;_LL421: _tmp32B=_tmp329.f1;
goto _LL420;_LL420: _tmp32A=_tmp329.f2;goto _LL41E;_LL41E: _tmp328=((struct Cyc_Absyn_While_s_struct*)
_tmp31F)->f2;goto _LL3FB;}else{goto _LL3FC;}_LL3FC: if((unsigned int)_tmp31F > 1?*((
int*)_tmp31F)== 16: 0){_LL425: _tmp32F=((struct Cyc_Absyn_Do_s_struct*)_tmp31F)->f1;
goto _LL422;_LL422: _tmp32C=((struct Cyc_Absyn_Do_s_struct*)_tmp31F)->f2;_LL424:
_tmp32E=_tmp32C.f1;goto _LL423;_LL423: _tmp32D=_tmp32C.f2;goto _LL3FD;}else{goto
_LL3FE;}_LL3FE: if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 8: 0){_LL42D:
_tmp337=((struct Cyc_Absyn_For_s_struct*)_tmp31F)->f1;goto _LL42A;_LL42A: _tmp334=((
struct Cyc_Absyn_For_s_struct*)_tmp31F)->f2;_LL42C: _tmp336=_tmp334.f1;goto _LL42B;
_LL42B: _tmp335=_tmp334.f2;goto _LL427;_LL427: _tmp331=((struct Cyc_Absyn_For_s_struct*)
_tmp31F)->f3;_LL429: _tmp333=_tmp331.f1;goto _LL428;_LL428: _tmp332=_tmp331.f2;goto
_LL426;_LL426: _tmp330=((struct Cyc_Absyn_For_s_struct*)_tmp31F)->f4;goto _LL3FF;}
else{goto _LL400;}_LL400: if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 5: 0){
_LL42E: _tmp338=((struct Cyc_Absyn_Break_s_struct*)_tmp31F)->f1;if(_tmp338 == 0){
goto _LL401;}else{goto _LL402;}}else{goto _LL402;}_LL402: if((unsigned int)_tmp31F > 
1?*((int*)_tmp31F)== 11: 0){_LL430: _tmp33B=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp31F)->f1;goto _LL42F;_LL42F: _tmp339=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp31F)->f2;if(_tmp339 == 0){goto _LL404;}else{_tmp33A=*_tmp339;goto _LL403;}}
else{goto _LL404;}_LL404: if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 5: 0){
_LL431: _tmp33C=((struct Cyc_Absyn_Break_s_struct*)_tmp31F)->f1;goto _LL405;}else{
goto _LL406;}_LL406: if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 6: 0){_LL432:
_tmp33D=((struct Cyc_Absyn_Continue_s_struct*)_tmp31F)->f1;goto _LL407;}else{goto
_LL408;}_LL408: if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 7: 0){_LL433:
_tmp33E=((struct Cyc_Absyn_Goto_s_struct*)_tmp31F)->f2;goto _LL409;}else{goto
_LL40A;}_LL40A: if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 9: 0){_LL435:
_tmp340=((struct Cyc_Absyn_Switch_s_struct*)_tmp31F)->f1;goto _LL434;_LL434:
_tmp33F=((struct Cyc_Absyn_Switch_s_struct*)_tmp31F)->f2;goto _LL40B;}else{goto
_LL40C;}_LL40C: if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 17: 0){_LL437:
_tmp342=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp31F)->f1;goto _LL436;_LL436:
_tmp341=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp31F)->f2;goto _LL40D;}else{goto
_LL40E;}_LL40E: if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 12: 0){_LL439:
_tmp344=((struct Cyc_Absyn_Decl_s_struct*)_tmp31F)->f1;goto _LL438;_LL438: _tmp343=((
struct Cyc_Absyn_Decl_s_struct*)_tmp31F)->f2;goto _LL40F;}else{goto _LL410;}_LL410:
if((unsigned int)_tmp31F > 1?*((int*)_tmp31F)== 15: 0){_LL43A: _tmp345=((struct Cyc_Absyn_Label_s_struct*)
_tmp31F)->f2;goto _LL411;}else{goto _LL412;}_LL412: if((unsigned int)_tmp31F > 1?*((
int*)_tmp31F)== 18: 0){_LL43D: _tmp348=((struct Cyc_Absyn_Region_s_struct*)_tmp31F)->f1;
goto _LL43C;_LL43C: _tmp347=((struct Cyc_Absyn_Region_s_struct*)_tmp31F)->f2;goto
_LL43B;_LL43B: _tmp346=((struct Cyc_Absyn_Region_s_struct*)_tmp31F)->f3;goto _LL413;}
else{goto _LL414;}_LL414: goto _LL415;_LL3EF: return inflow;_LL3F1: if(env->noreturn){({
void*_tmp349[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("`noreturn' function might return",
sizeof(unsigned char),33),_tag_arr(_tmp349,sizeof(void*),0));});}return(void*)0;
_LL3F3: if(env->noreturn){({void*_tmp34A[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("`noreturn' function might return",
sizeof(unsigned char),33),_tag_arr(_tmp34A,sizeof(void*),0));});}{void*_tmp34C;
void*_tmp34D;struct _tuple0 _tmp34B=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)_check_null(_tmp321));_LL440: _tmp34D=_tmp34B.f1;goto _LL43F;
_LL43F: _tmp34C=_tmp34B.f2;goto _LL43E;_LL43E: Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)_check_null(_tmp321))->loc,_tmp34D,_tmp34C);return(void*)0;}
_LL3F5: return(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp322)).f1;_LL3F7: return
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp324),
_tmp323);_LL3F9: {void*_tmp34F;void*_tmp350;struct _tuple0 _tmp34E=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp327);_LL443: _tmp350=_tmp34E.f1;goto _LL442;_LL442: _tmp34F=_tmp34E.f2;
goto _LL441;_LL441: return Cyc_CfFlowInfo_join_flow(env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp350,_tmp326),Cyc_NewControlFlow_anal_stmt(env,_tmp34F,_tmp325));}_LL3FB: {
void*_tmp351=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp32A))->flow;
void*_tmp353;void*_tmp354;struct _tuple0 _tmp352=Cyc_NewControlFlow_anal_test(env,
_tmp351,_tmp32B);_LL446: _tmp354=_tmp352.f1;goto _LL445;_LL445: _tmp353=_tmp352.f2;
goto _LL444;_LL444: {void*_tmp355=Cyc_NewControlFlow_anal_stmt(env,_tmp354,
_tmp328);Cyc_NewControlFlow_update_flow(env,_tmp32A,_tmp355);return _tmp353;}}
_LL3FD: {void*_tmp356=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp32F);void*
_tmp357=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp356,_tmp32D))->flow;
void*_tmp359;void*_tmp35A;struct _tuple0 _tmp358=Cyc_NewControlFlow_anal_test(env,
_tmp357,_tmp32E);_LL449: _tmp35A=_tmp358.f1;goto _LL448;_LL448: _tmp359=_tmp358.f2;
goto _LL447;_LL447: Cyc_NewControlFlow_update_flow(env,_tmp32F,_tmp35A);return
_tmp359;}_LL3FF: {void*_tmp35B=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp337)).f1;
void*_tmp35C=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp35B,_tmp335))->flow;
void*_tmp35E;void*_tmp35F;struct _tuple0 _tmp35D=Cyc_NewControlFlow_anal_test(env,
_tmp35C,_tmp336);_LL44C: _tmp35F=_tmp35D.f1;goto _LL44B;_LL44B: _tmp35E=_tmp35D.f2;
goto _LL44A;_LL44A: {void*_tmp360=Cyc_NewControlFlow_anal_stmt(env,_tmp35F,
_tmp330);void*_tmp361=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp360,
_tmp332))->flow;void*_tmp362=(Cyc_NewControlFlow_anal_Rexp(env,_tmp361,_tmp333)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp335,_tmp362);return _tmp35E;}}_LL401:
return(void*)0;_LL403: {struct Cyc_List_List*_tmp364;void*_tmp365;struct _tuple4
_tmp363=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp33B);_LL44F:
_tmp365=_tmp363.f1;goto _LL44E;_LL44E: _tmp364=_tmp363.f2;goto _LL44D;_LL44D: for(0;
_tmp364 != 0;(_tmp364=_tmp364->tl,_tmp33B=_tmp33B->tl)){_tmp365=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp33B))->hd)->loc,
_tmp365,(void*)_tmp364->hd);}_tmp365=Cyc_NewControlFlow_add_vars(_tmp365,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp33A->pat_vars))->v,Cyc_CfFlowInfo_unknown_all);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp33A->body,_tmp365);
return(void*)0;}_LL405: _tmp33D=_tmp33C;goto _LL407;_LL407: _tmp33E=_tmp33D;goto
_LL409;_LL409: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp366=_tmp31E->encloser;
struct Cyc_Absyn_Stmt*_tmp367=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)
_check_null(_tmp33E)))->encloser;while(_tmp367 != _tmp366){struct Cyc_Absyn_Stmt*
_tmp368=(Cyc_NewControlFlow_get_stmt_annot(_tmp366))->encloser;if(_tmp368 == 
_tmp366){({void*_tmp369[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("goto enters local scope or exception handler",
sizeof(unsigned char),45),_tag_arr(_tmp369,sizeof(void*),0));});break;}_tmp366=
_tmp368;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(
_tmp33E),inflow);return(void*)0;_LL40B: {void*_tmp36B;void*_tmp36C;struct _tuple0
_tmp36A=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp340);_LL452: _tmp36C=_tmp36A.f1;
goto _LL451;_LL451: _tmp36B=_tmp36A.f2;goto _LL450;_LL450: _tmp36C=Cyc_NewControlFlow_use_Rval(
env,_tmp340->loc,_tmp36C,_tmp36B);return Cyc_NewControlFlow_anal_scs(env,_tmp36C,
_tmp33F);}_LL40D: {int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;
env->in_try=1;(void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp342);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp341);{
void*_tmp36D=scs_outflow;_LL454: if((int)_tmp36D == 0){goto _LL455;}else{goto _LL456;}
_LL456: goto _LL457;_LL455: goto _LL453;_LL457:({void*_tmp36E[0]={};Cyc_Tcutil_terr(
s->loc,_tag_arr("last catch clause may implicitly fallthru",sizeof(unsigned char),
42),_tag_arr(_tmp36E,sizeof(void*),0));});_LL453:;}return s1_outflow;}}}_LL40F:
return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp344),_tmp343);_LL411: return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp345);
_LL413: {struct Cyc_List_List _tmp36F=({struct Cyc_List_List _tmp370;_tmp370.hd=
_tmp347;_tmp370.tl=0;_tmp370;});inflow=Cyc_NewControlFlow_add_vars(inflow,(
struct Cyc_List_List*)& _tmp36F,Cyc_CfFlowInfo_unknown_all);return Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp346);}_LL415:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371[0]=({struct Cyc_Core_Impossible_struct
_tmp372;_tmp372.tag=Cyc_Core_Impossible;_tmp372.f1=_tag_arr("anal_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof(unsigned char),56);_tmp372;});_tmp371;}));_LL3ED:;}}static void*Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Decl*decl){void*
_tmp373=(void*)decl->r;struct Cyc_Absyn_Vardecl*_tmp374;struct Cyc_Absyn_Exp*
_tmp375;struct Cyc_Core_Opt*_tmp376;struct Cyc_Core_Opt _tmp377;struct Cyc_List_List*
_tmp378;struct Cyc_List_List*_tmp379;_LL459: if(*((int*)_tmp373)== 0){_LL461:
_tmp374=((struct Cyc_Absyn_Var_d_struct*)_tmp373)->f1;goto _LL45A;}else{goto _LL45B;}
_LL45B: if(*((int*)_tmp373)== 2){_LL463: _tmp376=((struct Cyc_Absyn_Let_d_struct*)
_tmp373)->f2;if(_tmp376 == 0){goto _LL45D;}else{_tmp377=*_tmp376;_LL464: _tmp378=(
struct Cyc_List_List*)_tmp377.v;goto _LL462;}_LL462: _tmp375=((struct Cyc_Absyn_Let_d_struct*)
_tmp373)->f4;goto _LL45C;}else{goto _LL45D;}_LL45D: if(*((int*)_tmp373)== 3){_LL465:
_tmp379=((struct Cyc_Absyn_Letv_d_struct*)_tmp373)->f1;goto _LL45E;}else{goto
_LL45F;}_LL45F: goto _LL460;_LL45A: {struct Cyc_List_List _tmp37A=({struct Cyc_List_List
_tmp388;_tmp388.hd=_tmp374;_tmp388.tl=0;_tmp388;});inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)& _tmp37A,Cyc_CfFlowInfo_unknown_none);{struct Cyc_Absyn_Exp*
_tmp37B=_tmp374->initializer;if(_tmp37B == 0){return inflow;}{void*_tmp37D;void*
_tmp37E;struct _tuple0 _tmp37C=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp37B));_LL468: _tmp37E=_tmp37C.f1;goto _LL467;_LL467: _tmp37D=
_tmp37C.f2;goto _LL466;_LL466: {void*_tmp37F=_tmp37E;struct Cyc_List_List*_tmp380;
struct Cyc_Dict_Dict*_tmp381;_LL46A: if((int)_tmp37F == 0){goto _LL46B;}else{goto
_LL46C;}_LL46C: if((unsigned int)_tmp37F > 1?*((int*)_tmp37F)== 0: 0){_LL46F:
_tmp381=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp37F)->f1;goto _LL46E;
_LL46E: _tmp380=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp37F)->f2;goto
_LL46D;}else{goto _LL469;}_LL46B: return(void*)0;_LL46D: _tmp381=Cyc_CfFlowInfo_assign_place(
decl->loc,_tmp381,env->all_changed,({struct Cyc_CfFlowInfo_Place*_tmp382=
_cycalloc(sizeof(*_tmp382));_tmp382->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp384;_tmp384.tag=0;_tmp384.f1=_tmp374;_tmp384;});_tmp383;}));_tmp382->fields=
0;_tmp382;}),_tmp37D);_tmp380=Cyc_CfFlowInfo_reln_assign_var(_tmp380,_tmp374,(
struct Cyc_Absyn_Exp*)_check_null(_tmp374->initializer));{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp385=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp386=_cycalloc(sizeof(*
_tmp386));_tmp386[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp387;_tmp387.tag=
0;_tmp387.f1=_tmp381;_tmp387.f2=_tmp380;_tmp387;});_tmp386;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp385);return(void*)_tmp385;}_LL469:;}}}}_LL45C: {void*_tmp38A;void*
_tmp38B;struct _tuple0 _tmp389=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp375);
_LL472: _tmp38B=_tmp389.f1;goto _LL471;_LL471: _tmp38A=_tmp389.f2;goto _LL470;_LL470:
_tmp38B=Cyc_NewControlFlow_use_Rval(env,_tmp375->loc,_tmp38B,_tmp38A);return Cyc_NewControlFlow_add_vars(
_tmp38B,_tmp378,Cyc_CfFlowInfo_unknown_all);}_LL45E: return Cyc_NewControlFlow_add_vars(
inflow,_tmp379,Cyc_CfFlowInfo_unknown_none);_LL460:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C[0]=({
struct Cyc_Core_Impossible_struct _tmp38D;_tmp38D.tag=Cyc_Core_Impossible;_tmp38D.f1=
_tag_arr("anal_decl: unexpected decl variant",sizeof(unsigned char),35);_tmp38D;});
_tmp38C;}));_LL458:;}static void Cyc_NewControlFlow_postpass(struct Cyc_Absyn_Stmt*
s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{void*_tmp38E=(void*)s->r;
struct Cyc_Absyn_Stmt*_tmp38F;struct _tuple3 _tmp390;struct Cyc_Absyn_Stmt*_tmp391;
struct _tuple3 _tmp392;struct Cyc_Absyn_Stmt*_tmp393;struct Cyc_Absyn_Stmt*_tmp394;
struct Cyc_Absyn_Stmt*_tmp395;struct Cyc_Absyn_Stmt*_tmp396;struct _tuple3 _tmp397;
struct Cyc_Absyn_Stmt*_tmp398;struct _tuple3 _tmp399;struct Cyc_Absyn_Stmt*_tmp39A;
struct Cyc_Absyn_Stmt*_tmp39B;struct Cyc_Absyn_Stmt*_tmp39C;struct Cyc_Absyn_Stmt*
_tmp39D;struct Cyc_Absyn_Stmt*_tmp39E;struct Cyc_Absyn_Stmt*_tmp39F;struct Cyc_Absyn_Stmt*
_tmp3A0;struct Cyc_List_List*_tmp3A1;struct Cyc_Absyn_Stmt*_tmp3A2;struct Cyc_List_List*
_tmp3A3;_LL474: if((unsigned int)_tmp38E > 1?*((int*)_tmp38E)== 8: 0){_LL48D:
_tmp392=((struct Cyc_Absyn_For_s_struct*)_tmp38E)->f2;_LL48E: _tmp393=_tmp392.f2;
goto _LL48B;_LL48B: _tmp390=((struct Cyc_Absyn_For_s_struct*)_tmp38E)->f3;_LL48C:
_tmp391=_tmp390.f2;goto _LL48A;_LL48A: _tmp38F=((struct Cyc_Absyn_For_s_struct*)
_tmp38E)->f4;goto _LL475;}else{goto _LL476;}_LL476: if((unsigned int)_tmp38E > 1?*((
int*)_tmp38E)== 1: 0){_LL490: _tmp395=((struct Cyc_Absyn_Seq_s_struct*)_tmp38E)->f1;
goto _LL48F;_LL48F: _tmp394=((struct Cyc_Absyn_Seq_s_struct*)_tmp38E)->f2;goto
_LL477;}else{goto _LL478;}_LL478: if((unsigned int)_tmp38E > 1?*((int*)_tmp38E)== 4:
0){_LL492: _tmp397=((struct Cyc_Absyn_While_s_struct*)_tmp38E)->f1;_LL493: _tmp398=
_tmp397.f2;goto _LL491;_LL491: _tmp396=((struct Cyc_Absyn_While_s_struct*)_tmp38E)->f2;
goto _LL479;}else{goto _LL47A;}_LL47A: if((unsigned int)_tmp38E > 1?*((int*)_tmp38E)
== 16: 0){_LL496: _tmp39B=((struct Cyc_Absyn_Do_s_struct*)_tmp38E)->f1;goto _LL494;
_LL494: _tmp399=((struct Cyc_Absyn_Do_s_struct*)_tmp38E)->f2;_LL495: _tmp39A=
_tmp399.f2;goto _LL47B;}else{goto _LL47C;}_LL47C: if((unsigned int)_tmp38E > 1?*((
int*)_tmp38E)== 3: 0){_LL498: _tmp39D=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp38E)->f2;goto _LL497;_LL497: _tmp39C=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp38E)->f3;goto _LL47D;}else{goto _LL47E;}_LL47E: if((unsigned int)_tmp38E > 1?*((
int*)_tmp38E)== 18: 0){_LL499: _tmp39E=((struct Cyc_Absyn_Region_s_struct*)_tmp38E)->f3;
goto _LL47F;}else{goto _LL480;}_LL480: if((unsigned int)_tmp38E > 1?*((int*)_tmp38E)
== 15: 0){_LL49A: _tmp39F=((struct Cyc_Absyn_Label_s_struct*)_tmp38E)->f2;goto
_LL481;}else{goto _LL482;}_LL482: if((unsigned int)_tmp38E > 1?*((int*)_tmp38E)== 
12: 0){_LL49B: _tmp3A0=((struct Cyc_Absyn_Decl_s_struct*)_tmp38E)->f2;goto _LL483;}
else{goto _LL484;}_LL484: if((unsigned int)_tmp38E > 1?*((int*)_tmp38E)== 17: 0){
_LL49D: _tmp3A2=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp38E)->f1;goto _LL49C;
_LL49C: _tmp3A1=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp38E)->f2;goto _LL485;}
else{goto _LL486;}_LL486: if((unsigned int)_tmp38E > 1?*((int*)_tmp38E)== 9: 0){
_LL49E: _tmp3A3=((struct Cyc_Absyn_Switch_s_struct*)_tmp38E)->f2;goto _LL487;}else{
goto _LL488;}_LL488: goto _LL489;_LL475: Cyc_NewControlFlow_postpass(_tmp391);
_tmp395=_tmp38F;_tmp394=_tmp393;goto _LL477;_LL477: _tmp398=_tmp395;_tmp396=
_tmp394;goto _LL479;_LL479: _tmp39B=_tmp396;_tmp39A=_tmp398;goto _LL47B;_LL47B:
_tmp39D=_tmp39B;_tmp39C=_tmp39A;goto _LL47D;_LL47D: Cyc_NewControlFlow_postpass(
_tmp39D);_tmp39E=_tmp39C;goto _LL47F;_LL47F: _tmp39F=_tmp39E;goto _LL481;_LL481:
_tmp3A0=_tmp39F;goto _LL483;_LL483: Cyc_NewControlFlow_postpass(_tmp3A0);return;
_LL485: Cyc_NewControlFlow_postpass(_tmp3A2);_tmp3A3=_tmp3A1;goto _LL487;_LL487:
for(0;_tmp3A3 != 0;_tmp3A3=_tmp3A3->tl){Cyc_NewControlFlow_postpass(((struct Cyc_Absyn_Switch_clause*)
_tmp3A3->hd)->body);}return;_LL489: return;_LL473:;}}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){void*inflow=Cyc_NewControlFlow_add_vars((void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));
_tmp3AA[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3AB;_tmp3AB.tag=0;
_tmp3AB.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp3AB.f2=0;_tmp3AB;});
_tmp3AA;}),(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,
Cyc_CfFlowInfo_unknown_all);int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->iterate_again=1;_tmp3A9->iteration_num=
0;_tmp3A9->in_try=0;_tmp3A9->tryflow=(void*)inflow;_tmp3A9->all_changed=0;
_tmp3A9->noreturn=noreturn;_tmp3A9;});void*outflow=inflow;while(env->iterate_again){
++ env->iteration_num;env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,fd->body);}{void*_tmp3A4=outflow;_LL4A0: if((int)_tmp3A4 == 0){goto
_LL4A1;}else{goto _LL4A2;}_LL4A2: goto _LL4A3;_LL4A1: goto _LL49F;_LL4A3: if(noreturn){({
void*_tmp3A5[0]={};Cyc_Tcutil_terr((fd->body)->loc,_tag_arr("`noreturn' function might (implicitly) return",
sizeof(unsigned char),46),_tag_arr(_tmp3A5,sizeof(void*),0));});}else{{void*
_tmp3A6=Cyc_Tcutil_compress((void*)fd->ret_type);_LL4A5: if((int)_tmp3A6 == 0){
goto _LL4A6;}else{goto _LL4A7;}_LL4A7: if((unsigned int)_tmp3A6 > 3?*((int*)_tmp3A6)
== 6: 0){goto _LL4A8;}else{goto _LL4A9;}_LL4A9: if((int)_tmp3A6 == 1){goto _LL4AA;}
else{goto _LL4AB;}_LL4AB: if((unsigned int)_tmp3A6 > 3?*((int*)_tmp3A6)== 5: 0){goto
_LL4AC;}else{goto _LL4AD;}_LL4AD: goto _LL4AE;_LL4A6: goto _LL4A4;_LL4A8: goto _LL4AA;
_LL4AA: goto _LL4AC;_LL4AC:({void*_tmp3A7[0]={};Cyc_Tcutil_warn((fd->body)->loc,
_tag_arr("function may not return a value",sizeof(unsigned char),32),_tag_arr(
_tmp3A7,sizeof(void*),0));});goto _LL4A4;_LL4AE:({void*_tmp3A8[0]={};Cyc_Tcutil_terr((
fd->body)->loc,_tag_arr("function may not return a value",sizeof(unsigned char),
32),_tag_arr(_tmp3A8,sizeof(void*),0));});goto _LL4A4;_LL4A4:;}goto _LL49F;}_LL49F:;}
Cyc_NewControlFlow_postpass(fd->body);}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*
ds){for(0;ds != 0;ds=ds->tl){void*_tmp3AC=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;
struct Cyc_Absyn_Fndecl*_tmp3AD;struct Cyc_List_List*_tmp3AE;struct Cyc_List_List*
_tmp3AF;struct Cyc_List_List*_tmp3B0;_LL4B0: if(*((int*)_tmp3AC)== 1){_LL4BA:
_tmp3AD=((struct Cyc_Absyn_Fn_d_struct*)_tmp3AC)->f1;goto _LL4B1;}else{goto _LL4B2;}
_LL4B2: if(*((int*)_tmp3AC)== 10){_LL4BB: _tmp3AE=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3AC)->f1;goto _LL4B3;}else{goto _LL4B4;}_LL4B4: if(*((int*)_tmp3AC)== 9){_LL4BC:
_tmp3AF=((struct Cyc_Absyn_Using_d_struct*)_tmp3AC)->f2;goto _LL4B5;}else{goto
_LL4B6;}_LL4B6: if(*((int*)_tmp3AC)== 8){_LL4BD: _tmp3B0=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3AC)->f2;goto _LL4B7;}else{goto _LL4B8;}_LL4B8: goto _LL4B9;_LL4B1: Cyc_NewControlFlow_check_fun(
_tmp3AD);goto _LL4AF;_LL4B3: _tmp3AF=_tmp3AE;goto _LL4B5;_LL4B5: _tmp3B0=_tmp3AF;
goto _LL4B7;_LL4B7: Cyc_NewControlFlow_cf_check(_tmp3B0);goto _LL4AF;_LL4B9: goto
_LL4AF;_LL4AF:;}}
