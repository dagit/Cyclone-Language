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
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;void*f1;struct Cyc_Absyn_Tqual f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_FnType_struct{
int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_StructType_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};struct Cyc_Absyn_UnionType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};
struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_AnonStructType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
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
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_StructMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_StructArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple1*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Struct_p_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;};struct Cyc_Absyn_Tunion_p_struct{int tag;struct Cyc_Absyn_Tuniondecl*
f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;
};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_Absyn_UnknownFields_p_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;
struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structfield{
struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Struct_d_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Absyn_Union_d_struct{int tag;struct Cyc_Absyn_Uniondecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
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
int tag;void*f1;};struct Cyc_Tcenv_StructRes_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;
struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*uniondecls;struct
Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};extern void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);extern struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);extern unsigned char
Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
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
_tmp6;struct _tuple1*_tmp7;_LL1: if((unsigned int)_tmp0 > 2?*((int*)_tmp0)== 11: 0){
_LL9: _tmp1=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;goto _LL2;}else{goto
_LL3;}_LL3: if((unsigned int)_tmp0 > 2?*((int*)_tmp0)== 12: 0){_LLC: _tmp4=((struct
Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f1;goto _LLB;_LLB: _tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f2;goto _LLA;_LLA: _tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;
goto _LL4;}else{goto _LL5;}_LL5: if((unsigned int)_tmp0 > 2?*((int*)_tmp0)== 13: 0){
_LLF: _tmp7=((struct Cyc_Absyn_UnknownFields_p_struct*)_tmp0)->f1;goto _LLE;_LLE:
_tmp6=((struct Cyc_Absyn_UnknownFields_p_struct*)_tmp0)->f2;goto _LLD;_LLD: _tmp5=((
struct Cyc_Absyn_UnknownFields_p_struct*)_tmp0)->f3;goto _LL6;}else{goto _LL7;}_LL7:
goto _LL8;_LL2:{struct _handler_cons _tmp8;_push_handler(& _tmp8);{int _tmpA=0;if(
setjmp(_tmp8.handler)){_tmpA=1;}if(! _tmpA){{void*_tmpB=Cyc_Tcenv_lookup_ordinary(
te,p->loc,_tmp1);struct Cyc_Absyn_Tunionfield*_tmpC;struct Cyc_Absyn_Tuniondecl*
_tmpD;struct Cyc_Absyn_Enumfield*_tmpE;struct Cyc_Absyn_Enumdecl*_tmpF;struct Cyc_Absyn_Enumfield*
_tmp10;void*_tmp11;_LL11: if(*((int*)_tmpB)== 1){goto _LL12;}else{goto _LL13;}_LL13:
if(*((int*)_tmpB)== 2){_LL1C: _tmpD=((struct Cyc_Tcenv_TunionRes_struct*)_tmpB)->f1;
goto _LL1B;_LL1B: _tmpC=((struct Cyc_Tcenv_TunionRes_struct*)_tmpB)->f2;goto _LL14;}
else{goto _LL15;}_LL15: if(*((int*)_tmpB)== 3){_LL1E: _tmpF=((struct Cyc_Tcenv_EnumRes_struct*)
_tmpB)->f1;goto _LL1D;_LL1D: _tmpE=((struct Cyc_Tcenv_EnumRes_struct*)_tmpB)->f2;
goto _LL16;}else{goto _LL17;}_LL17: if(*((int*)_tmpB)== 4){_LL20: _tmp11=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmpB)->f1;goto _LL1F;_LL1F: _tmp10=((struct
Cyc_Tcenv_AnonEnumRes_struct*)_tmpB)->f2;goto _LL18;}else{goto _LL19;}_LL19: if(*((
int*)_tmpB)== 0){goto _LL1A;}else{goto _LL10;}_LL12:({void*_tmp12[0]={};Cyc_Tcutil_terr(
p->loc,_tag_arr("struct tag used without arguments in pattern",sizeof(
unsigned char),45),_tag_arr(_tmp12,sizeof(void*),0));});_npop_handler(0);return;
_LL14:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*_tmp13=
_cycalloc(sizeof(*_tmp13));_tmp13[0]=({struct Cyc_Absyn_Tunion_p_struct _tmp14;
_tmp14.tag=8;_tmp14.f1=_tmpD;_tmp14.f2=_tmpC;_tmp14.f3=0;_tmp14.f4=0;_tmp14;});
_tmp13;})));_npop_handler(0);return;_LL16:(void*)(p->r=(void*)((void*)({struct
Cyc_Absyn_Enum_p_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=({struct Cyc_Absyn_Enum_p_struct
_tmp16;_tmp16.tag=9;_tmp16.f1=_tmpF;_tmp16.f2=_tmpE;_tmp16;});_tmp15;})));
_npop_handler(0);return;_LL18:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=({struct Cyc_Absyn_AnonEnum_p_struct
_tmp18;_tmp18.tag=10;_tmp18.f1=(void*)_tmp11;_tmp18.f2=_tmp10;_tmp18;});_tmp17;})));
_npop_handler(0);return;_LL1A: goto _LL10;_LL10:;};_pop_handler();}else{void*_tmp9=(
void*)_exn_thrown;void*_tmp1A=_tmp9;_LL22: if(_tmp1A == Cyc_Dict_Absent){goto _LL23;}
else{goto _LL24;}_LL24: goto _LL25;_LL23: goto _LL21;_LL25:(void)_throw(_tmp1A);_LL21:;}}}{
void*_tmp1B=(*_tmp1).f1;struct Cyc_List_List*_tmp1C;_LL27: if((int)_tmp1B == 0){
goto _LL28;}else{goto _LL29;}_LL29: if((unsigned int)_tmp1B > 1?*((int*)_tmp1B)== 0:
0){_LL2D: _tmp1C=((struct Cyc_Absyn_Rel_n_struct*)_tmp1B)->f1;if(_tmp1C == 0){goto
_LL2A;}else{goto _LL2B;}}else{goto _LL2B;}_LL2B: goto _LL2C;_LL28: goto _LL2A;_LL2A:(*
_tmp1).f1=(void*)0;(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*
_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D[0]=({struct Cyc_Absyn_Var_p_struct _tmp1E;
_tmp1E.tag=0;_tmp1E.f1=Cyc_Absyn_new_vardecl(_tmp1,(void*)0,0);_tmp1E;});_tmp1D;})));
return;_LL2C:({void*_tmp1F[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("qualified variable in pattern",
sizeof(unsigned char),30),_tag_arr(_tmp1F,sizeof(void*),0));});return;_LL26:;}
_LL4:{struct _handler_cons _tmp20;_push_handler(& _tmp20);{int _tmp22=0;if(setjmp(
_tmp20.handler)){_tmp22=1;}if(! _tmp22){{void*_tmp23=Cyc_Tcenv_lookup_ordinary(te,
p->loc,_tmp4);struct Cyc_Absyn_Structdecl*_tmp24;struct Cyc_Absyn_Tunionfield*
_tmp25;struct Cyc_Absyn_Tuniondecl*_tmp26;_LL2F: if(*((int*)_tmp23)== 1){_LL39:
_tmp24=((struct Cyc_Tcenv_StructRes_struct*)_tmp23)->f1;goto _LL30;}else{goto _LL31;}
_LL31: if(*((int*)_tmp23)== 2){_LL3B: _tmp26=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp23)->f1;goto _LL3A;_LL3A: _tmp25=((struct Cyc_Tcenv_TunionRes_struct*)_tmp23)->f2;
goto _LL32;}else{goto _LL33;}_LL33: if(*((int*)_tmp23)== 3){goto _LL34;}else{goto
_LL35;}_LL35: if(*((int*)_tmp23)== 4){goto _LL36;}else{goto _LL37;}_LL37: if(*((int*)
_tmp23)== 0){goto _LL38;}else{goto _LL2E;}_LL30: {struct Cyc_List_List*_tmp27=0;
for(0;_tmp2 != 0;_tmp2=_tmp2->tl){_tmp27=({struct Cyc_List_List*_tmp28=_cycalloc(
sizeof(*_tmp28));_tmp28->hd=({struct _tuple7*_tmp29=_cycalloc(sizeof(*_tmp29));
_tmp29->f1=0;_tmp29->f2=(struct Cyc_Absyn_Pat*)_tmp2->hd;_tmp29;});_tmp28->tl=
_tmp27;_tmp28;});}(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Struct_p_struct*
_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Absyn_Struct_p_struct
_tmp2B;_tmp2B.tag=7;_tmp2B.f1=_tmp24;_tmp2B.f2=0;_tmp2B.f3=_tmp3;_tmp2B.f4=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp27);_tmp2B;});
_tmp2A;})));_npop_handler(0);return;}_LL32:(void*)(p->r=(void*)((void*)({struct
Cyc_Absyn_Tunion_p_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C[0]=({struct
Cyc_Absyn_Tunion_p_struct _tmp2D;_tmp2D.tag=8;_tmp2D.f1=_tmp26;_tmp2D.f2=_tmp25;
_tmp2D.f3=_tmp3;_tmp2D.f4=_tmp2;_tmp2D;});_tmp2C;})));_npop_handler(0);return;
_LL34: goto _LL36;_LL36:({struct Cyc_Std_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp4);{void*_tmp2E[1]={& _tmp2F};Cyc_Tcutil_terr(
p->loc,_tag_arr("bad enum constructor %s in pattern",sizeof(unsigned char),35),
_tag_arr(_tmp2E,sizeof(void*),1));}});_npop_handler(0);return;_LL38: goto _LL2E;
_LL2E:;};_pop_handler();}else{void*_tmp21=(void*)_exn_thrown;void*_tmp31=_tmp21;
_LL3D: if(_tmp31 == Cyc_Dict_Absent){goto _LL3E;}else{goto _LL3F;}_LL3F: goto _LL40;
_LL3E: goto _LL3C;_LL40:(void)_throw(_tmp31);_LL3C:;}}}({struct Cyc_Std_String_pa_struct
_tmp33;_tmp33.tag=0;_tmp33.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp4);{
void*_tmp32[1]={& _tmp33};Cyc_Tcutil_terr(p->loc,_tag_arr("%s is not a constructor in pattern",
sizeof(unsigned char),35),_tag_arr(_tmp32,sizeof(void*),1));}});return;_LL6:{
struct _handler_cons _tmp34;_push_handler(& _tmp34);{int _tmp36=0;if(setjmp(_tmp34.handler)){
_tmp36=1;}if(! _tmp36){{void*_tmp37=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp7);
struct Cyc_Absyn_Structdecl*_tmp38;_LL42: if(*((int*)_tmp37)== 1){_LL46: _tmp38=((
struct Cyc_Tcenv_StructRes_struct*)_tmp37)->f1;goto _LL43;}else{goto _LL44;}_LL44:
goto _LL45;_LL43:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Struct_p_struct*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_Struct_p_struct
_tmp3A;_tmp3A.tag=7;_tmp3A.f1=_tmp38;_tmp3A.f2=0;_tmp3A.f3=_tmp6;_tmp3A.f4=_tmp5;
_tmp3A;});_tmp39;})));_npop_handler(0);return;_LL45: goto _LL41;_LL41:;};
_pop_handler();}else{void*_tmp35=(void*)_exn_thrown;void*_tmp3C=_tmp35;_LL48: if(
_tmp3C == Cyc_Dict_Absent){goto _LL49;}else{goto _LL4A;}_LL4A: goto _LL4B;_LL49: goto
_LL47;_LL4B:(void)_throw(_tmp3C);_LL47:;}}}({struct Cyc_Std_String_pa_struct
_tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp7);{
void*_tmp3D[1]={& _tmp3E};Cyc_Tcutil_terr(p->loc,_tag_arr("%s is not a struct name, but pattern has designators",
sizeof(unsigned char),53),_tag_arr(_tmp3D,sizeof(void*),1));}});return;_LL8:
return;_LL0:;}static struct _tagged_arr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*
vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,
void**topt){if(topt != 0){return*topt;}return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->v=s;
_tmp3F;}));}static void*Cyc_Tcpat_num_type(void**topt,void*numt){if(topt != 0? Cyc_Tcutil_coerceable(*
topt): 0){return*topt;}{void*_tmp40=Cyc_Tcutil_compress(numt);_LL4D: if((
unsigned int)_tmp40 > 3?*((int*)_tmp40)== 12: 0){goto _LL4E;}else{goto _LL4F;}_LL4F:
goto _LL50;_LL4E: if(topt != 0){return*topt;}goto _LL4C;_LL50: goto _LL4C;_LL4C:;}
return numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual();*
v_result_ptr=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp41=_cycalloc(
sizeof(*_tmp41));_tmp41->hd=vd;_tmp41->tl=*v_result_ptr;_tmp41;});}struct _tuple8{
struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple9{struct Cyc_Absyn_Structfield*f1;
struct Cyc_Absyn_Pat*f2;};struct _tuple6 Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt,void**region_opt){Cyc_Tcpat_resolve_pat(te,p);{
void*t;struct Cyc_List_List*tv_result=0;struct Cyc_List_List*v_result=0;{void*
_tmp42=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp43;struct Cyc_Absyn_Vardecl*_tmp44;
void*_tmp45;void*_tmp46;struct Cyc_Absyn_Enumfield*_tmp47;struct Cyc_Absyn_Enumdecl*
_tmp48;struct Cyc_Absyn_Enumfield*_tmp49;void*_tmp4A;struct Cyc_Absyn_Pat*_tmp4B;
struct Cyc_List_List*_tmp4C;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*_tmp4E;
struct Cyc_Core_Opt*_tmp4F;struct Cyc_Core_Opt**_tmp50;struct Cyc_Absyn_Structdecl*
_tmp51;struct Cyc_List_List*_tmp52;struct Cyc_List_List*_tmp53;struct Cyc_Absyn_Tunionfield*
_tmp54;struct Cyc_Absyn_Tuniondecl*_tmp55;_LL52: if((int)_tmp42 == 0){goto _LL53;}
else{goto _LL54;}_LL54: if((unsigned int)_tmp42 > 2?*((int*)_tmp42)== 0: 0){_LL74:
_tmp43=((struct Cyc_Absyn_Var_p_struct*)_tmp42)->f1;goto _LL55;}else{goto _LL56;}
_LL56: if((unsigned int)_tmp42 > 2?*((int*)_tmp42)== 6: 0){_LL75: _tmp44=((struct Cyc_Absyn_Reference_p_struct*)
_tmp42)->f1;goto _LL57;}else{goto _LL58;}_LL58: if((unsigned int)_tmp42 > 2?*((int*)
_tmp42)== 1: 0){_LL76: _tmp45=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp42)->f1;
if((int)_tmp45 == 1){goto _LL59;}else{goto _LL5A;}}else{goto _LL5A;}_LL5A: if((
unsigned int)_tmp42 > 2?*((int*)_tmp42)== 1: 0){_LL77: _tmp46=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp42)->f1;if((int)_tmp46 == 0){goto _LL5B;}else{goto _LL5C;}}else{goto _LL5C;}
_LL5C: if((unsigned int)_tmp42 > 2?*((int*)_tmp42)== 2: 0){goto _LL5D;}else{goto
_LL5E;}_LL5E: if((unsigned int)_tmp42 > 2?*((int*)_tmp42)== 3: 0){goto _LL5F;}else{
goto _LL60;}_LL60: if((unsigned int)_tmp42 > 2?*((int*)_tmp42)== 9: 0){_LL79: _tmp48=((
struct Cyc_Absyn_Enum_p_struct*)_tmp42)->f1;goto _LL78;_LL78: _tmp47=((struct Cyc_Absyn_Enum_p_struct*)
_tmp42)->f2;goto _LL61;}else{goto _LL62;}_LL62: if((unsigned int)_tmp42 > 2?*((int*)
_tmp42)== 10: 0){_LL7B: _tmp4A=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp42)->f1;
goto _LL7A;_LL7A: _tmp49=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp42)->f2;goto
_LL63;}else{goto _LL64;}_LL64: if((int)_tmp42 == 1){goto _LL65;}else{goto _LL66;}
_LL66: if((unsigned int)_tmp42 > 2?*((int*)_tmp42)== 5: 0){_LL7C: _tmp4B=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp42)->f1;goto _LL67;}else{goto _LL68;}_LL68: if((unsigned int)_tmp42 > 2?*((int*)
_tmp42)== 4: 0){_LL7D: _tmp4C=((struct Cyc_Absyn_Tuple_p_struct*)_tmp42)->f1;goto
_LL69;}else{goto _LL6A;}_LL6A: if((unsigned int)_tmp42 > 2?*((int*)_tmp42)== 7: 0){
_LL81: _tmp51=((struct Cyc_Absyn_Struct_p_struct*)_tmp42)->f1;goto _LL80;_LL80:
_tmp4F=((struct Cyc_Absyn_Struct_p_struct*)_tmp42)->f2;_tmp50=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Struct_p_struct*)_tmp42)->f2;goto _LL7F;_LL7F: _tmp4E=((struct Cyc_Absyn_Struct_p_struct*)
_tmp42)->f3;goto _LL7E;_LL7E: _tmp4D=((struct Cyc_Absyn_Struct_p_struct*)_tmp42)->f4;
goto _LL6B;}else{goto _LL6C;}_LL6C: if((unsigned int)_tmp42 > 2?*((int*)_tmp42)== 8:
0){_LL85: _tmp55=((struct Cyc_Absyn_Tunion_p_struct*)_tmp42)->f1;goto _LL84;_LL84:
_tmp54=((struct Cyc_Absyn_Tunion_p_struct*)_tmp42)->f2;goto _LL83;_LL83: _tmp53=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp42)->f3;goto _LL82;_LL82: _tmp52=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp42)->f4;goto _LL6D;}else{goto _LL6E;}_LL6E: if((unsigned int)_tmp42 > 2?*((int*)
_tmp42)== 11: 0){goto _LL6F;}else{goto _LL70;}_LL70: if((unsigned int)_tmp42 > 2?*((
int*)_tmp42)== 12: 0){goto _LL71;}else{goto _LL72;}_LL72: if((unsigned int)_tmp42 > 2?*((
int*)_tmp42)== 13: 0){goto _LL73;}else{goto _LL51;}_LL53: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);goto _LL51;_LL55: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),
topt);Cyc_Tcpat_set_vd(_tmp43,& v_result,t);goto _LL51;_LL57: t=Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars(te),topt);if(region_opt == 0){({void*_tmp56[0]={};Cyc_Tcutil_terr(
p->loc,_tag_arr("* pattern would point into an unknown region",sizeof(
unsigned char),45),_tag_arr(_tmp56,sizeof(void*),0));});goto _LL51;}Cyc_Tcpat_set_vd(
_tmp44,& v_result,(void*)({struct Cyc_Absyn_PointerType_struct*_tmp57=_cycalloc(
sizeof(*_tmp57));_tmp57[0]=({struct Cyc_Absyn_PointerType_struct _tmp58;_tmp58.tag=
4;_tmp58.f1=({struct Cyc_Absyn_PtrInfo _tmp59;_tmp59.elt_typ=(void*)t;_tmp59.rgn_typ=(
void*)*region_opt;_tmp59.nullable=((struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(
0);_tmp59.tq=Cyc_Absyn_empty_tqual();_tmp59.bounds=Cyc_Absyn_empty_conref();
_tmp59;});_tmp58;});_tmp57;}));goto _LL51;_LL59: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_t);
goto _LL51;_LL5B: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_t);goto _LL51;_LL5D: t=
Cyc_Tcpat_num_type(topt,Cyc_Absyn_uchar_t);goto _LL51;_LL5F: t=Cyc_Tcpat_num_type(
topt,Cyc_Absyn_float_typ);goto _LL51;_LL61: t=Cyc_Tcpat_num_type(topt,(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A[0]=({
struct Cyc_Absyn_EnumType_struct _tmp5B;_tmp5B.tag=12;_tmp5B.f1=_tmp48->name;
_tmp5B.f2=(struct Cyc_Absyn_Enumdecl*)_tmp48;_tmp5B;});_tmp5A;}));goto _LL51;_LL63:
t=Cyc_Tcpat_num_type(topt,_tmp4A);goto _LL51;_LL65: if(topt != 0){void*_tmp5C=Cyc_Tcutil_compress(*
topt);_LL87: if((unsigned int)_tmp5C > 3?*((int*)_tmp5C)== 4: 0){goto _LL88;}else{
goto _LL89;}_LL89: goto _LL8A;_LL88: t=*topt;goto tcpat_end;_LL8A: goto _LL86;_LL86:;}{
struct Cyc_List_List*_tmp5D=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=({struct Cyc_Absyn_PointerType_struct
_tmp5F;_tmp5F.tag=4;_tmp5F.f1=({struct Cyc_Absyn_PtrInfo _tmp60;_tmp60.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->v=_tmp5D;_tmp62;}));_tmp60.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->v=_tmp5D;_tmp61;}));_tmp60.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp60.tq=Cyc_Absyn_empty_tqual();
_tmp60.bounds=Cyc_Absyn_empty_conref();_tmp60;});_tmp5F;});_tmp5E;});goto _LL51;}
_LL67: {void*inner_typ=(void*)0;void**_tmp63=0;if(topt != 0){void*_tmp64=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp65;void*_tmp66;_LL8C: if((unsigned int)_tmp64 > 3?*((
int*)_tmp64)== 4: 0){_LL90: _tmp65=((struct Cyc_Absyn_PointerType_struct*)_tmp64)->f1;
_LL91: _tmp66=(void*)_tmp65.elt_typ;goto _LL8D;}else{goto _LL8E;}_LL8E: goto _LL8F;
_LL8D: inner_typ=_tmp66;_tmp63=(void**)& inner_typ;goto _LL8B;_LL8F: goto _LL8B;_LL8B:;}{
void*ptr_rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp7C;}));struct Cyc_List_List*_tmp68;struct Cyc_List_List*_tmp69;struct _tuple6
_tmp67=Cyc_Tcpat_tcPatRec(te,_tmp4B,_tmp63,(void**)& ptr_rgn);_LL94: _tmp69=_tmp67.f1;
goto _LL93;_LL93: _tmp68=_tmp67.f2;goto _LL92;_LL92: tv_result=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tv_result,_tmp69);
v_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(v_result,_tmp68);if(_tmp63 == 0){void*_tmp6A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4B->topt))->v);struct Cyc_Absyn_TunionFieldInfo
_tmp6B;struct Cyc_List_List*_tmp6C;void*_tmp6D;struct Cyc_Absyn_Tunionfield*_tmp6E;
struct Cyc_Absyn_Tuniondecl*_tmp6F;_LL96: if((unsigned int)_tmp6A > 3?*((int*)
_tmp6A)== 3: 0){_LL9A: _tmp6B=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp6A)->f1;
_LL9C: _tmp6D=(void*)_tmp6B.field_info;if(*((int*)_tmp6D)== 1){_LL9E: _tmp6F=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp6D)->f1;goto _LL9D;_LL9D: _tmp6E=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp6D)->f2;goto _LL9B;}else{goto _LL98;}
_LL9B: _tmp6C=_tmp6B.targs;goto _LL97;}else{goto _LL98;}_LL98: goto _LL99;_LL97: t=(
void*)({struct Cyc_Absyn_TunionType_struct*_tmp70=_cycalloc(sizeof(*_tmp70));
_tmp70[0]=({struct Cyc_Absyn_TunionType_struct _tmp71;_tmp71.tag=2;_tmp71.f1=({
struct Cyc_Absyn_TunionInfo _tmp72;_tmp72.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp74;_tmp74.tag=1;_tmp74.f1=({struct Cyc_Absyn_Tuniondecl**_tmp75=_cycalloc(
sizeof(*_tmp75));_tmp75[0]=_tmp6F;_tmp75;});_tmp74;});_tmp73;}));_tmp72.targs=
_tmp6C;_tmp72.rgn=(void*)ptr_rgn;_tmp72;});_tmp71;});_tmp70;});goto _LL95;_LL99: t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp76=_cycalloc(sizeof(*_tmp76));
_tmp76[0]=({struct Cyc_Absyn_PointerType_struct _tmp77;_tmp77.tag=4;_tmp77.f1=({
struct Cyc_Absyn_PtrInfo _tmp78;_tmp78.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B->topt))->v);_tmp78.rgn_typ=(void*)ptr_rgn;_tmp78.nullable=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp78.tq=Cyc_Absyn_empty_tqual();
_tmp78.bounds=Cyc_Absyn_empty_conref();_tmp78;});_tmp77;});_tmp76;});goto _LL95;
_LL95:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp79=_cycalloc(
sizeof(*_tmp79));_tmp79[0]=({struct Cyc_Absyn_PointerType_struct _tmp7A;_tmp7A.tag=
4;_tmp7A.f1=({struct Cyc_Absyn_PtrInfo _tmp7B;_tmp7B.elt_typ=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4B->topt))->v);_tmp7B.rgn_typ=(void*)ptr_rgn;
_tmp7B.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp7B.tq=
Cyc_Absyn_empty_tqual();_tmp7B.bounds=Cyc_Absyn_empty_conref();_tmp7B;});_tmp7A;});
_tmp79;});}goto _LL51;}}_LL69: {struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*
topt_ts=0;if(topt != 0){void*_tmp7D=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*
_tmp7E;_LLA0: if((unsigned int)_tmp7D > 3?*((int*)_tmp7D)== 9: 0){_LLA4: _tmp7E=((
struct Cyc_Absyn_TupleType_struct*)_tmp7D)->f1;goto _LLA1;}else{goto _LLA2;}_LLA2:
goto _LLA3;_LLA1: topt_ts=_tmp7E;goto _LL9F;_LLA3: goto _LL9F;_LL9F:;}for(0;_tmp4C != 
0;_tmp4C=_tmp4C->tl){void**_tmp7F=0;if(topt_ts != 0){_tmp7F=(void**)&(*((struct
_tuple8*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}{struct Cyc_List_List*_tmp81;
struct Cyc_List_List*_tmp82;struct _tuple6 _tmp80=Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)
_tmp4C->hd,_tmp7F,region_opt);_LLA7: _tmp82=_tmp80.f1;goto _LLA6;_LLA6: _tmp81=
_tmp80.f2;goto _LLA5;_LLA5: tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(tv_result,_tmp82);v_result=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(v_result,_tmp81);
pat_ts=({struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->hd=({
struct _tuple8*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->f1=Cyc_Absyn_empty_tqual();
_tmp84->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)
_tmp4C->hd)->topt))->v;_tmp84;});_tmp83->tl=pat_ts;_tmp83;});}}t=(void*)({struct
Cyc_Absyn_TupleType_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({struct
Cyc_Absyn_TupleType_struct _tmp86;_tmp86.tag=9;_tmp86.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmp86;});_tmp85;});goto _LL51;}
_LL6B: if(_tmp4E != 0){(int)_throw(({void*_tmp87[0]={};Cyc_Tcutil_impos(_tag_arr("tcPat: struct<...> not implemented",
sizeof(unsigned char),35),_tag_arr(_tmp87,sizeof(void*),0));}));}{struct
_RegionHandle _tmp88=_new_region("rgn");struct _RegionHandle*rgn=& _tmp88;
_push_region(rgn);{struct _tuple4 _tmp89=({struct _tuple4 _tmp9D;_tmp9D.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp9D.f2=rgn;_tmp9D;});struct Cyc_List_List*_tmp8A=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp89,_tmp51->tvs);struct Cyc_List_List*_tmp8B=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp8A);t=(void*)({struct Cyc_Absyn_StructType_struct*_tmp8C=
_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_StructType_struct _tmp8D;
_tmp8D.tag=10;_tmp8D.f1=(struct _tuple1*)((struct _tuple1*)((struct Cyc_Core_Opt*)
_check_null(_tmp51->name))->v);_tmp8D.f2=_tmp8B;_tmp8D.f3=({struct Cyc_Absyn_Structdecl**
_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=_tmp51;_tmp8E;});_tmp8D;});_tmp8C;});
if(topt != 0){Cyc_Tcutil_unify(t,*topt);}*_tmp50=({struct Cyc_Core_Opt*_tmp8F=
_cycalloc(sizeof(*_tmp8F));_tmp8F->v=_tmp8B;_tmp8F;});if(_tmp51->fields == 0){({
void*_tmp90[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("can't destructure an abstract struct",
sizeof(unsigned char),37),_tag_arr(_tmp90,sizeof(void*),0));});_npop_handler(0);
goto _LL51;}{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,_tmp4D,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp51->fields))->v);for(
0;fields != 0;fields=fields->tl){struct _tuple9 _tmp92;struct Cyc_Absyn_Pat*_tmp93;
struct Cyc_Absyn_Structfield*_tmp94;struct _tuple9*_tmp91=(struct _tuple9*)fields->hd;
_tmp92=*_tmp91;_LLAA: _tmp94=_tmp92.f1;goto _LLA9;_LLA9: _tmp93=_tmp92.f2;goto _LLA8;
_LLA8: {void*_tmp95=Cyc_Tcutil_rsubstitute(rgn,_tmp8A,(void*)_tmp94->type);
struct Cyc_List_List*_tmp97;struct Cyc_List_List*_tmp98;struct _tuple6 _tmp96=Cyc_Tcpat_tcPatRec(
te,_tmp93,(void**)& _tmp95,region_opt);_LLAD: _tmp98=_tmp96.f1;goto _LLAC;_LLAC:
_tmp97=_tmp96.f2;goto _LLAB;_LLAB: tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(tv_result,_tmp98);v_result=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(v_result,_tmp97);
if(! Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp93->topt))->v,
_tmp95)){({struct Cyc_Std_String_pa_struct _tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp93->topt))->v);{struct Cyc_Std_String_pa_struct _tmp9B;_tmp9B.tag=0;_tmp9B.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp95);{struct Cyc_Std_String_pa_struct
_tmp9A;_tmp9A.tag=0;_tmp9A.f1=(struct _tagged_arr)*_tmp94->name;{void*_tmp99[3]={&
_tmp9A,& _tmp9B,& _tmp9C};Cyc_Tcutil_terr(p->loc,_tag_arr("field %s of struct pattern expects type %s != %s",
sizeof(unsigned char),49),_tag_arr(_tmp99,sizeof(void*),3));}}}});}}}}};
_pop_region(rgn);}goto _LL51;_LL6D: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp53)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp54->tvs)){({void*
_tmp9E[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("wrong number of existential type variables in pattern",
sizeof(unsigned char),54),_tag_arr(_tmp9E,sizeof(void*),0));});}if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp53)!= 0){region_opt=0;}{struct
_RegionHandle _tmp9F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9F;
_push_region(rgn);{struct Cyc_List_List*_tmpA0=0;{struct Cyc_List_List*t=_tmp53;
for(0;t != 0;t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;{void*
_tmpA1=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpA2;struct Cyc_Core_Opt*
_tmpA3;struct Cyc_Core_Opt**_tmpA4;void*_tmpA5;struct Cyc_Core_Opt*_tmpA6;struct
Cyc_Core_Opt**_tmpA7;void*_tmpA8;struct Cyc_Core_Opt*_tmpA9;struct Cyc_Core_Opt**
_tmpAA;void*_tmpAB;void*_tmpAC;_LLAF: if(*((int*)_tmpA1)== 0){_LLBB: _tmpA2=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmpA1)->f1;if((int)_tmpA2 == 2){goto _LLB0;}else{
goto _LLB1;}}else{goto _LLB1;}_LLB1: if(*((int*)_tmpA1)== 1){_LLBC: _tmpA3=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpA1)->f1;_tmpA4=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Unknown_kb_struct*)_tmpA1)->f1;goto _LLB2;}else{goto _LLB3;}_LLB3: if(*((
int*)_tmpA1)== 2){_LLBE: _tmpA6=((struct Cyc_Absyn_Less_kb_struct*)_tmpA1)->f1;
_tmpA7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpA1)->f1;goto
_LLBD;_LLBD: _tmpA5=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpA1)->f2;if((int)
_tmpA5 == 1){goto _LLB4;}else{goto _LLB5;}}else{goto _LLB5;}_LLB5: if(*((int*)_tmpA1)
== 2){_LLC0: _tmpA9=((struct Cyc_Absyn_Less_kb_struct*)_tmpA1)->f1;_tmpAA=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpA1)->f1;goto _LLBF;_LLBF:
_tmpA8=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpA1)->f2;if((int)_tmpA8 == 0){
goto _LLB6;}else{goto _LLB7;}}else{goto _LLB7;}_LLB7: if(*((int*)_tmpA1)== 0){_LLC1:
_tmpAB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpA1)->f1;goto _LLB8;}else{goto
_LLB9;}_LLB9: if(*((int*)_tmpA1)== 2){_LLC2: _tmpAC=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpA1)->f2;goto _LLBA;}else{goto _LLAE;}_LLB0: goto _LLAE;_LLB2: _tmpA7=_tmpA4;goto
_LLB4;_LLB4: _tmpAA=_tmpA7;goto _LLB6;_LLB6:*_tmpAA=({struct Cyc_Core_Opt*_tmpAD=
_cycalloc(sizeof(*_tmpAD));_tmpAD->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpAF;
_tmpAF.tag=0;_tmpAF.f1=(void*)((void*)2);_tmpAF;});_tmpAE;}));_tmpAD;});goto
_LLAE;_LLB8: _tmpAC=_tmpAB;goto _LLBA;_LLBA:({struct Cyc_Std_String_pa_struct _tmpB3;
_tmpB3.tag=0;_tmpB3.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(_tmpAC);{
struct Cyc_Std_String_pa_struct _tmpB2;_tmpB2.tag=0;_tmpB2.f1=(struct _tagged_arr)*
tv->name;{struct Cyc_Std_String_pa_struct _tmpB1;_tmpB1.tag=0;_tmpB1.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp55->name);{void*_tmpB0[3]={& _tmpB1,&
_tmpB2,& _tmpB3};Cyc_Tcutil_terr(p->loc,_tag_arr("tunion member %s abstracts type variable %s of kind %s != B",
sizeof(unsigned char),60),_tag_arr(_tmpB0,sizeof(void*),3));}}}});goto _LLAE;
_LLAE:;}_tmpA0=({struct Cyc_List_List*_tmpB4=_region_malloc(rgn,sizeof(*_tmpB4));
_tmpB4->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmpB5=_cycalloc(
sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_VarType_struct _tmpB6;_tmpB6.tag=1;
_tmpB6.f1=tv;_tmpB6;});_tmpB5;}));_tmpB4->tl=_tmpA0;_tmpB4;});}}_tmpA0=Cyc_List_imp_rev(
_tmpA0);tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(tv_result,_tmp53);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
p->loc,te,_tmp53);struct Cyc_List_List*tqts=_tmp54->typs;struct Cyc_List_List*
_tmpB7=Cyc_Tcenv_lookup_type_vars(te);struct _tuple4 _tmpB8=({struct _tuple4 _tmpE0;
_tmpE0.f1=_tmpB7;_tmpE0.f2=rgn;_tmpE0;});struct Cyc_List_List*_tmpB9=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmpB8,_tmp55->tvs);struct Cyc_List_List*_tmpBA=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp54->tvs,_tmpA0);struct Cyc_List_List*_tmpBB=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpB9);struct Cyc_List_List*_tmpBC=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpBA);struct Cyc_List_List*_tmpBD=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(rgn,_tmpB9,_tmpBA);if(tqts == 0){t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_Absyn_TunionType_struct
_tmpBF;_tmpBF.tag=2;_tmpBF.f1=({struct Cyc_Absyn_TunionInfo _tmpC0;_tmpC0.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpC2=_cycalloc(sizeof(*
_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpC3;_tmpC3.tag=1;
_tmpC3.f1=({struct Cyc_Absyn_Tuniondecl**_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[
0]=_tmp55;_tmpC4;});_tmpC3;});_tmpC2;}));_tmpC0.targs=_tmpBB;_tmpC0.rgn=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->v=_tmpB7;_tmpC1;}));_tmpC0;});_tmpBF;});
_tmpBE;});}else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmpC5=
_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpC6;_tmpC6.tag=3;_tmpC6.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpC7;_tmpC7.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmpC8=_cycalloc(sizeof(*
_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmpC9;_tmpC9.tag=1;
_tmpC9.f1=_tmp55;_tmpC9.f2=_tmp54;_tmpC9;});_tmpC8;}));_tmpC7.targs=_tmpBB;
_tmpC7;});_tmpC6;});_tmpC5;});}if(topt != 0? tqts == 0: 0){void*_tmpCA=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_TunionInfo _tmpCB;struct Cyc_List_List*_tmpCC;_LLC4: if((
unsigned int)_tmpCA > 3?*((int*)_tmpCA)== 3: 0){goto _LLC5;}else{goto _LLC6;}_LLC6:
if((unsigned int)_tmpCA > 3?*((int*)_tmpCA)== 2: 0){_LLCA: _tmpCB=((struct Cyc_Absyn_TunionType_struct*)
_tmpCA)->f1;_LLCB: _tmpCC=_tmpCB.targs;goto _LLC7;}else{goto _LLC8;}_LLC8: goto _LLC9;
_LLC5: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmpCD=_cycalloc(sizeof(*
_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmpCE;_tmpCE.tag=3;
_tmpCE.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpCF;_tmpCF.field_info=(void*)((
void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));
_tmpD0[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmpD1;_tmpD1.tag=1;_tmpD1.f1=
_tmp55;_tmpD1.f2=_tmp54;_tmpD1;});_tmpD0;}));_tmpCF.targs=_tmpBB;_tmpCF;});
_tmpCE;});_tmpCD;});goto _LLC3;_LLC7: {struct Cyc_List_List*_tmpD2=_tmpBB;for(0;
_tmpD2 != 0? _tmpCC != 0: 0;(_tmpD2=_tmpD2->tl,_tmpCC=_tmpCC->tl)){Cyc_Tcutil_unify((
void*)_tmpD2->hd,(void*)_tmpCC->hd);}goto _LLC3;}_LLC9: goto _LLC3;_LLC3:;}for(0;
_tmp52 != 0? tqts != 0: 0;(_tmp52=_tmp52->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmpD3=(struct Cyc_Absyn_Pat*)_tmp52->hd;void*_tmpD4=Cyc_Tcutil_rsubstitute(rgn,
_tmpBD,(*((struct _tuple8*)tqts->hd)).f2);struct Cyc_List_List*_tmpD6;struct Cyc_List_List*
_tmpD7;struct _tuple6 _tmpD5=Cyc_Tcpat_tcPatRec(te2,_tmpD3,(void**)& _tmpD4,
region_opt);_LLCE: _tmpD7=_tmpD5.f1;goto _LLCD;_LLCD: _tmpD6=_tmpD5.f2;goto _LLCC;
_LLCC: tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(tv_result,_tmpD7);v_result=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(v_result,_tmpD6);if(! Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD3->topt))->v,_tmpD4)){({struct Cyc_Std_String_pa_struct
_tmpDB;_tmpDB.tag=0;_tmpDB.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpD3->topt))->v);{struct Cyc_Std_String_pa_struct
_tmpDA;_tmpDA.tag=0;_tmpDA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpD4);{
struct Cyc_Std_String_pa_struct _tmpD9;_tmpD9.tag=0;_tmpD9.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp55->name);{void*_tmpD8[3]={& _tmpD9,& _tmpDA,& _tmpDB};
Cyc_Tcutil_terr(_tmpD3->loc,_tag_arr("%s expects argument type %s, not %s",
sizeof(unsigned char),36),_tag_arr(_tmpD8,sizeof(void*),3));}}}});}}if(_tmp52 != 
0){({struct Cyc_Std_String_pa_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp55->name);{void*_tmpDC[1]={& _tmpDD};Cyc_Tcutil_terr(
p->loc,_tag_arr("too many arguments for tunion constructor %s",sizeof(
unsigned char),45),_tag_arr(_tmpDC,sizeof(void*),1));}});}if(tqts != 0){({struct
Cyc_Std_String_pa_struct _tmpDF;_tmpDF.tag=0;_tmpDF.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp55->name);{void*_tmpDE[1]={& _tmpDF};Cyc_Tcutil_terr(p->loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(unsigned char),44),_tag_arr(_tmpDE,sizeof(void*),1));}});}}};_pop_region(
rgn);}goto _LL51;_LL6F: goto _LL71;_LL71: goto _LL73;_LL73: t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpE1;}));goto _LL51;_LL51:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmpE2=
_cycalloc(sizeof(*_tmpE2));_tmpE2->v=(void*)t;_tmpE2;});return({struct _tuple6
_tmpE3;_tmpE3.f1=tv_result;_tmpE3.f2=v_result;_tmpE3;});}}struct _tuple6 Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**region_opt){
struct _tuple6 _tmpE4=Cyc_Tcpat_tcPatRec(te,p,topt,region_opt);{struct
_RegionHandle _tmpE5=_new_region("r");struct _RegionHandle*r=& _tmpE5;_push_region(
r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tagged_arr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(
r,Cyc_Tcpat_get_name,_tmpE4.f2),p->loc,_tag_arr("pattern contains a repeated variable",
sizeof(unsigned char),37));;_pop_region(r);}return _tmpE4;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmpE6=(void*)p->r;struct Cyc_Absyn_Pat*
_tmpE7;struct Cyc_List_List*_tmpE8;struct Cyc_List_List*_tmpE9;struct Cyc_List_List*
_tmpEA;_LLD0: if((unsigned int)_tmpE6 > 2?*((int*)_tmpE6)== 5: 0){_LLDA: _tmpE7=((
struct Cyc_Absyn_Pointer_p_struct*)_tmpE6)->f1;goto _LLD1;}else{goto _LLD2;}_LLD2:
if((unsigned int)_tmpE6 > 2?*((int*)_tmpE6)== 8: 0){_LLDB: _tmpE8=((struct Cyc_Absyn_Tunion_p_struct*)
_tmpE6)->f4;goto _LLD3;}else{goto _LLD4;}_LLD4: if((unsigned int)_tmpE6 > 2?*((int*)
_tmpE6)== 7: 0){_LLDC: _tmpE9=((struct Cyc_Absyn_Struct_p_struct*)_tmpE6)->f4;goto
_LLD5;}else{goto _LLD6;}_LLD6: if((unsigned int)_tmpE6 > 2?*((int*)_tmpE6)== 4: 0){
_LLDD: _tmpEA=((struct Cyc_Absyn_Tuple_p_struct*)_tmpE6)->f1;goto _LLD7;}else{goto
_LLD8;}_LLD8: goto _LLD9;_LLD1: Cyc_Tcpat_check_pat_regions(te,_tmpE7);{void*_tmpEB=(
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_PtrInfo
_tmpEC;void*_tmpED;struct Cyc_Absyn_TunionInfo _tmpEE;void*_tmpEF;_LLDF: if((
unsigned int)_tmpEB > 3?*((int*)_tmpEB)== 4: 0){_LLE5: _tmpEC=((struct Cyc_Absyn_PointerType_struct*)
_tmpEB)->f1;_LLE6: _tmpED=(void*)_tmpEC.rgn_typ;goto _LLE0;}else{goto _LLE1;}_LLE1:
if((unsigned int)_tmpEB > 3?*((int*)_tmpEB)== 2: 0){_LLE7: _tmpEE=((struct Cyc_Absyn_TunionType_struct*)
_tmpEB)->f1;_LLE8: _tmpEF=(void*)_tmpEE.rgn;goto _LLE2;}else{goto _LLE3;}_LLE3: goto
_LLE4;_LLE0: _tmpEF=_tmpED;goto _LLE2;_LLE2: Cyc_Tcenv_check_rgn_accessible(te,p->loc,
_tmpEF);return;_LLE4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Core_Impossible_struct
_tmpF1;_tmpF1.tag=Cyc_Core_Impossible;_tmpF1.f1=_tag_arr("check_pat_regions: bad pointer type",
sizeof(unsigned char),36);_tmpF1;});_tmpF0;}));_LLDE:;}_LLD3: for(0;_tmpE8 != 0;
_tmpE8=_tmpE8->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmpE8->hd);}{
void*_tmpF2=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_TunionInfo
_tmpF3;void*_tmpF4;_LLEA: if((unsigned int)_tmpF2 > 3?*((int*)_tmpF2)== 2: 0){_LLF0:
_tmpF3=((struct Cyc_Absyn_TunionType_struct*)_tmpF2)->f1;_LLF1: _tmpF4=(void*)
_tmpF3.rgn;goto _LLEB;}else{goto _LLEC;}_LLEC: if((unsigned int)_tmpF2 > 3?*((int*)
_tmpF2)== 3: 0){goto _LLED;}else{goto _LLEE;}_LLEE: goto _LLEF;_LLEB: Cyc_Tcenv_check_rgn_accessible(
te,p->loc,_tmpF4);return;_LLED: return;_LLEF:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[0]=({struct Cyc_Core_Impossible_struct
_tmpF6;_tmpF6.tag=Cyc_Core_Impossible;_tmpF6.f1=_tag_arr("check_pat_regions: bad tunion type",
sizeof(unsigned char),35);_tmpF6;});_tmpF5;}));_LLE9:;}_LLD5: _tmpEA=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Pat*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Pat*(*)(struct _tuple7*))Cyc_Core_snd,_tmpE9);goto _LLD7;_LLD7:
for(0;_tmpEA != 0;_tmpEA=_tmpEA->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmpEA->hd);}return;_LLD9: return;_LLCF:;}struct Cyc_Tcpat_Name_v_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};struct Cyc_Tcpat_Con_s{
void*name;int arity;struct Cyc_Core_Opt*span;};struct Cyc_Tcpat_Con_struct{int tag;
struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){void*_tmpF7=(void*)c1->name;
struct _tagged_arr _tmpF8;int _tmpF9;_LLF3: if(*((int*)_tmpF7)== 0){_LLF7: _tmpF8=((
struct Cyc_Tcpat_Name_v_struct*)_tmpF7)->f1;goto _LLF4;}else{goto _LLF5;}_LLF5: if(*((
int*)_tmpF7)== 1){_LLF8: _tmpF9=((struct Cyc_Tcpat_Int_v_struct*)_tmpF7)->f1;goto
_LLF6;}else{goto _LLF2;}_LLF4: {void*_tmpFA=(void*)c2->name;struct _tagged_arr
_tmpFB;_LLFA: if(*((int*)_tmpFA)== 0){_LLFE: _tmpFB=((struct Cyc_Tcpat_Name_v_struct*)
_tmpFA)->f1;goto _LLFB;}else{goto _LLFC;}_LLFC: if(*((int*)_tmpFA)== 1){goto _LLFD;}
else{goto _LLF9;}_LLFB: return Cyc_Std_zstrcmp(_tmpF8,_tmpFB);_LLFD: return - 1;_LLF9:;}
_LLF6: {void*_tmpFC=(void*)c2->name;int _tmpFD;_LL100: if(*((int*)_tmpFC)== 0){
goto _LL101;}else{goto _LL102;}_LL102: if(*((int*)_tmpFC)== 1){_LL104: _tmpFD=((
struct Cyc_Tcpat_Int_v_struct*)_tmpFC)->f1;goto _LL103;}else{goto _LLFF;}_LL101:
return 1;_LL103: return _tmpF9 - _tmpFD;_LLFF:;}_LLF2:;}static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){
return((struct Cyc_Set_Set*(*)(int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))
Cyc_Set_empty)(Cyc_Tcpat_compare_con);}static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(
void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(void*)2};static struct Cyc_Core_Opt*
Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;static
unsigned char _tmpFF[5]="NULL";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={
0,{_tmpFF,_tmpFF,_tmpFF + 5}};static unsigned char _tmp101[4]="ptr";static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_ptr_name_value={0,{_tmp101,_tmp101,_tmp101 + 4}};static unsigned char
_tmp103[6]="tuple";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={
0,{_tmp103,_tmp103,_tmp103 + 6}};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)((void*)& Cyc_Tcpat_null_name_value),0,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={(void*)((void*)& Cyc_Tcpat_ptr_name_value),
1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)((void*)& Cyc_Tcpat_ptr_name_value),1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0;static struct
Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0;static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(
int i){return({struct Cyc_Tcpat_Con_s*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp105=_cycalloc_atomic(sizeof(*
_tmp105));_tmp105[0]=({struct Cyc_Tcpat_Int_v_struct _tmp106;_tmp106.tag=1;_tmp106.f1=
i;_tmp106;});_tmp105;}));_tmp104->arity=0;_tmp104->span=0;_tmp104;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _tagged_arr f){return({struct Cyc_Tcpat_Con_s*
_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp109;_tmp109.tag=0;_tmp109.f1=f;_tmp109;});_tmp108;}));_tmp107->arity=0;
_tmp107->span=0;_tmp107;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(
unsigned char c){return({struct Cyc_Tcpat_Con_s*_tmp10A=_cycalloc(sizeof(*_tmp10A));
_tmp10A->name=(void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp10C=
_cycalloc_atomic(sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_Tcpat_Int_v_struct
_tmp10D;_tmp10D.tag=1;_tmp10D.f1=(int)c;_tmp10D;});_tmp10C;}));_tmp10A->arity=0;
_tmp10A->span=({struct Cyc_Core_Opt*_tmp10B=_cycalloc_atomic(sizeof(*_tmp10B));
_tmp10B->v=(void*)256;_tmp10B;});_tmp10A;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(
int i){return({struct Cyc_Tcpat_Con_s*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->name=(
void*)((void*)& Cyc_Tcpat_tuple_name_value);_tmp10E->arity=i;_tmp10E->span=Cyc_Tcpat_one_opt_ptr;
_tmp10E;});}static void*Cyc_Tcpat_null_pat(){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F[0]=({struct Cyc_Tcpat_Con_struct
_tmp110;_tmp110.tag=0;_tmp110.f1=Cyc_Tcpat_null_con;_tmp110.f2=0;_tmp110;});
_tmp10F;});}static void*Cyc_Tcpat_int_pat(int i){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=({struct Cyc_Tcpat_Con_struct
_tmp112;_tmp112.tag=0;_tmp112.f1=Cyc_Tcpat_int_con(i);_tmp112.f2=0;_tmp112;});
_tmp111;});}static void*Cyc_Tcpat_char_pat(unsigned char c){return(void*)({struct
Cyc_Tcpat_Con_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Tcpat_Con_struct
_tmp114;_tmp114.tag=0;_tmp114.f1=Cyc_Tcpat_char_con(c);_tmp114.f2=0;_tmp114;});
_tmp113;});}static void*Cyc_Tcpat_float_pat(struct _tagged_arr f){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115[0]=({
struct Cyc_Tcpat_Con_struct _tmp116;_tmp116.tag=0;_tmp116.f1=Cyc_Tcpat_float_con(f);
_tmp116.f2=0;_tmp116;});_tmp115;});}static void*Cyc_Tcpat_null_ptr_pat(void*p){
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp117=_cycalloc(sizeof(*_tmp117));
_tmp117[0]=({struct Cyc_Tcpat_Con_struct _tmp118;_tmp118.tag=0;_tmp118.f1=Cyc_Tcpat_null_ptr_con;
_tmp118.f2=({struct Cyc_List_List*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->hd=(
void*)p;_tmp119->tl=0;_tmp119;});_tmp118;});_tmp117;});}static void*Cyc_Tcpat_ptr_pat(
void*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp11A=_cycalloc(sizeof(*
_tmp11A));_tmp11A[0]=({struct Cyc_Tcpat_Con_struct _tmp11B;_tmp11B.tag=0;_tmp11B.f1=
Cyc_Tcpat_ptr_con;_tmp11B.f2=({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*
_tmp11C));_tmp11C->hd=(void*)p;_tmp11C->tl=0;_tmp11C;});_tmp11B;});_tmp11A;});}
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({struct Cyc_Tcpat_Con_struct
_tmp11E;_tmp11E.tag=0;_tmp11E.f1=Cyc_Tcpat_tuple_con(Cyc_List_length(ss));
_tmp11E.f2=ss;_tmp11E;});_tmp11D;});}static void*Cyc_Tcpat_con_pat(struct
_tagged_arr con_name,struct Cyc_Core_Opt*span,struct Cyc_List_List*ps){struct Cyc_Tcpat_Con_s*
c=({struct Cyc_Tcpat_Con_s*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->name=(
void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp122=_cycalloc(sizeof(*_tmp122));
_tmp122[0]=({struct Cyc_Tcpat_Name_v_struct _tmp123;_tmp123.tag=0;_tmp123.f1=
con_name;_tmp123;});_tmp122;}));_tmp121->arity=Cyc_List_length(ps);_tmp121->span=
span;_tmp121;});return(void*)({struct Cyc_Tcpat_Con_struct*_tmp11F=_cycalloc(
sizeof(*_tmp11F));_tmp11F[0]=({struct Cyc_Tcpat_Con_struct _tmp120;_tmp120.tag=0;
_tmp120.f1=c;_tmp120.f2=ps;_tmp120;});_tmp11F;});}static void*Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat*p){void*s;{void*_tmp124=(void*)p->r;int _tmp125;void*_tmp126;
unsigned char _tmp127;struct _tagged_arr _tmp128;struct Cyc_Absyn_Pat*_tmp129;struct
Cyc_List_List*_tmp12A;struct Cyc_Absyn_Tunionfield*_tmp12B;struct Cyc_Absyn_Tuniondecl*
_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_List_List*
_tmp12F;struct Cyc_Absyn_Structdecl*_tmp130;struct Cyc_Absyn_Enumfield*_tmp131;
struct Cyc_Absyn_Enumdecl*_tmp132;struct Cyc_Absyn_Enumfield*_tmp133;void*_tmp134;
_LL106: if((int)_tmp124 == 0){goto _LL107;}else{goto _LL108;}_LL108: if((unsigned int)
_tmp124 > 2?*((int*)_tmp124)== 0: 0){goto _LL109;}else{goto _LL10A;}_LL10A: if((int)
_tmp124 == 1){goto _LL10B;}else{goto _LL10C;}_LL10C: if((unsigned int)_tmp124 > 2?*((
int*)_tmp124)== 1: 0){_LL129: _tmp126=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp124)->f1;goto _LL128;_LL128: _tmp125=((struct Cyc_Absyn_Int_p_struct*)_tmp124)->f2;
goto _LL10D;}else{goto _LL10E;}_LL10E: if((unsigned int)_tmp124 > 2?*((int*)_tmp124)
== 2: 0){_LL12A: _tmp127=((struct Cyc_Absyn_Char_p_struct*)_tmp124)->f1;goto _LL10F;}
else{goto _LL110;}_LL110: if((unsigned int)_tmp124 > 2?*((int*)_tmp124)== 3: 0){
_LL12B: _tmp128=((struct Cyc_Absyn_Float_p_struct*)_tmp124)->f1;goto _LL111;}else{
goto _LL112;}_LL112: if((unsigned int)_tmp124 > 2?*((int*)_tmp124)== 6: 0){goto
_LL113;}else{goto _LL114;}_LL114: if((unsigned int)_tmp124 > 2?*((int*)_tmp124)== 5:
0){_LL12C: _tmp129=((struct Cyc_Absyn_Pointer_p_struct*)_tmp124)->f1;goto _LL115;}
else{goto _LL116;}_LL116: if((unsigned int)_tmp124 > 2?*((int*)_tmp124)== 8: 0){
_LL12F: _tmp12C=((struct Cyc_Absyn_Tunion_p_struct*)_tmp124)->f1;goto _LL12E;_LL12E:
_tmp12B=((struct Cyc_Absyn_Tunion_p_struct*)_tmp124)->f2;goto _LL12D;_LL12D:
_tmp12A=((struct Cyc_Absyn_Tunion_p_struct*)_tmp124)->f4;if(_tmp12A == 0){goto
_LL117;}else{goto _LL118;}}else{goto _LL118;}_LL118: if((unsigned int)_tmp124 > 2?*((
int*)_tmp124)== 8: 0){_LL130: _tmp12D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp124)->f4;
goto _LL119;}else{goto _LL11A;}_LL11A: if((unsigned int)_tmp124 > 2?*((int*)_tmp124)
== 4: 0){_LL131: _tmp12E=((struct Cyc_Absyn_Tuple_p_struct*)_tmp124)->f1;goto _LL11B;}
else{goto _LL11C;}_LL11C: if((unsigned int)_tmp124 > 2?*((int*)_tmp124)== 7: 0){
_LL133: _tmp130=((struct Cyc_Absyn_Struct_p_struct*)_tmp124)->f1;goto _LL132;_LL132:
_tmp12F=((struct Cyc_Absyn_Struct_p_struct*)_tmp124)->f4;goto _LL11D;}else{goto
_LL11E;}_LL11E: if((unsigned int)_tmp124 > 2?*((int*)_tmp124)== 9: 0){_LL135:
_tmp132=((struct Cyc_Absyn_Enum_p_struct*)_tmp124)->f1;goto _LL134;_LL134: _tmp131=((
struct Cyc_Absyn_Enum_p_struct*)_tmp124)->f2;goto _LL11F;}else{goto _LL120;}_LL120:
if((unsigned int)_tmp124 > 2?*((int*)_tmp124)== 10: 0){_LL137: _tmp134=(void*)((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp124)->f1;goto _LL136;_LL136: _tmp133=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp124)->f2;goto _LL121;}else{goto _LL122;}
_LL122: if((unsigned int)_tmp124 > 2?*((int*)_tmp124)== 11: 0){goto _LL123;}else{
goto _LL124;}_LL124: if((unsigned int)_tmp124 > 2?*((int*)_tmp124)== 12: 0){goto
_LL125;}else{goto _LL126;}_LL126: if((unsigned int)_tmp124 > 2?*((int*)_tmp124)== 
13: 0){goto _LL127;}else{goto _LL105;}_LL107: s=(void*)0;goto _LL105;_LL109: s=(void*)
0;goto _LL105;_LL10B: s=Cyc_Tcpat_null_pat();goto _LL105;_LL10D: s=Cyc_Tcpat_int_pat(
_tmp125);goto _LL105;_LL10F: s=Cyc_Tcpat_char_pat(_tmp127);goto _LL105;_LL111: s=Cyc_Tcpat_float_pat(
_tmp128);goto _LL105;_LL113: s=(void*)0;goto _LL105;_LL115:{void*_tmp135=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp136;struct Cyc_Absyn_Conref*_tmp137;_LL139: if((unsigned int)_tmp135 > 3?*((int*)
_tmp135)== 4: 0){_LL13F: _tmp136=((struct Cyc_Absyn_PointerType_struct*)_tmp135)->f1;
_LL140: _tmp137=_tmp136.nullable;goto _LL13A;}else{goto _LL13B;}_LL13B: if((
unsigned int)_tmp135 > 3?*((int*)_tmp135)== 2: 0){goto _LL13C;}else{goto _LL13D;}
_LL13D: goto _LL13E;_LL13A: {int is_nullable=0;int still_working=1;while(
still_working){void*_tmp138=(void*)_tmp137->v;struct Cyc_Absyn_Conref*_tmp139;int
_tmp13A;_LL142: if((unsigned int)_tmp138 > 1?*((int*)_tmp138)== 1: 0){_LL148:
_tmp139=((struct Cyc_Absyn_Forward_constr_struct*)_tmp138)->f1;goto _LL143;}else{
goto _LL144;}_LL144: if((int)_tmp138 == 0){goto _LL145;}else{goto _LL146;}_LL146: if((
unsigned int)_tmp138 > 1?*((int*)_tmp138)== 0: 0){_LL149: _tmp13A=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp138)->f1;goto _LL147;}else{goto _LL141;}_LL143:(void*)(_tmp137->v=(void*)((
void*)_tmp139->v));continue;_LL145:(void*)(_tmp137->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp13C;_tmp13C.tag=0;_tmp13C.f1=(void*)0;_tmp13C;});_tmp13B;})));is_nullable=0;
still_working=0;goto _LL141;_LL147: is_nullable=(int)_tmp13A;still_working=0;goto
_LL141;_LL141:;}{void*ss=Cyc_Tcpat_compile_pat(_tmp129);if(is_nullable){s=Cyc_Tcpat_null_ptr_pat(
ss);}else{s=Cyc_Tcpat_ptr_pat(ss);}goto _LL138;}}_LL13C:{void*_tmp13D=(void*)
_tmp129->r;struct Cyc_List_List*_tmp13E;struct Cyc_Absyn_Tunionfield*_tmp13F;
struct Cyc_Absyn_Tuniondecl*_tmp140;_LL14B: if((unsigned int)_tmp13D > 2?*((int*)
_tmp13D)== 8: 0){_LL151: _tmp140=((struct Cyc_Absyn_Tunion_p_struct*)_tmp13D)->f1;
goto _LL150;_LL150: _tmp13F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp13D)->f2;goto
_LL14F;_LL14F: _tmp13E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp13D)->f4;goto _LL14C;}
else{goto _LL14D;}_LL14D: goto _LL14E;_LL14C: {struct Cyc_Core_Opt*span;if(_tmp140->is_xtunion){
span=0;}else{span=({struct Cyc_Core_Opt*_tmp141=_cycalloc_atomic(sizeof(*_tmp141));
_tmp141->v=(void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp140->fields))->v);_tmp141;});}s=Cyc_Tcpat_con_pat(*(*
_tmp13F->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp13E));goto _LL14A;}
_LL14E: s=({void*_tmp142[0]={};Cyc_Tcutil_impos(_tag_arr("non-[x]tunion pattern has tunion type",
sizeof(unsigned char),38),_tag_arr(_tmp142,sizeof(void*),0));});goto _LL14A;
_LL14A:;}goto _LL138;_LL13E: s=({void*_tmp143[0]={};Cyc_Tcutil_impos(_tag_arr("pointer pattern does not have pointer type!",
sizeof(unsigned char),44),_tag_arr(_tmp143,sizeof(void*),0));});goto _LL138;
_LL138:;}goto _LL105;_LL117: {struct Cyc_Core_Opt*span;{void*_tmp144=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);_LL153: if((unsigned int)
_tmp144 > 3?*((int*)_tmp144)== 2: 0){goto _LL154;}else{goto _LL155;}_LL155: if((
unsigned int)_tmp144 > 3?*((int*)_tmp144)== 3: 0){goto _LL156;}else{goto _LL157;}
_LL157: goto _LL158;_LL154: if(_tmp12C->is_xtunion){span=0;}else{span=({struct Cyc_Core_Opt*
_tmp145=_cycalloc_atomic(sizeof(*_tmp145));_tmp145->v=(void*)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp12C->fields))->v);_tmp145;});}goto _LL152;_LL156: span=({struct Cyc_Core_Opt*
_tmp146=_cycalloc_atomic(sizeof(*_tmp146));_tmp146->v=(void*)1;_tmp146;});goto
_LL152;_LL158: span=({void*_tmp147[0]={};((struct Cyc_Core_Opt*(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("void tunion pattern has bad type",
sizeof(unsigned char),33),_tag_arr(_tmp147,sizeof(void*),0));});goto _LL152;
_LL152:;}s=Cyc_Tcpat_con_pat(*(*_tmp12B->name).f2,span,0);goto _LL105;}_LL119:
_tmp12E=_tmp12D;goto _LL11B;_LL11B: s=Cyc_Tcpat_tuple_pat(((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,
_tmp12E));goto _LL105;_LL11D: {struct Cyc_List_List*ps=0;{struct Cyc_List_List*
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp130->fields))->v;
for(0;fields != 0;fields=fields->tl){int found=Cyc_Std_strcmp(*((struct Cyc_Absyn_Structfield*)
fields->hd)->name,_tag_arr("",sizeof(unsigned char),1))== 0;{struct Cyc_List_List*
dlps0=_tmp12F;for(0;! found? dlps0 != 0: 0;dlps0=dlps0->tl){struct _tuple7 _tmp149;
struct Cyc_Absyn_Pat*_tmp14A;struct Cyc_List_List*_tmp14B;struct _tuple7*_tmp148=(
struct _tuple7*)dlps0->hd;_tmp149=*_tmp148;_LL15B: _tmp14B=_tmp149.f1;goto _LL15A;
_LL15A: _tmp14A=_tmp149.f2;goto _LL159;_LL159: {struct Cyc_List_List*_tmp14C=
_tmp14B;struct Cyc_List_List _tmp14D;struct Cyc_List_List*_tmp14E;void*_tmp14F;
struct _tagged_arr*_tmp150;_LL15D: if(_tmp14C == 0){goto _LL15F;}else{_tmp14D=*
_tmp14C;_LL162: _tmp14F=(void*)_tmp14D.hd;if(*((int*)_tmp14F)== 1){_LL163: _tmp150=((
struct Cyc_Absyn_FieldName_struct*)_tmp14F)->f1;goto _LL161;}else{goto _LL15F;}
_LL161: _tmp14E=_tmp14D.tl;if(_tmp14E == 0){goto _LL15E;}else{goto _LL15F;}}_LL15F:
goto _LL160;_LL15E: if(Cyc_Std_zstrptrcmp(_tmp150,((struct Cyc_Absyn_Structfield*)
fields->hd)->name)== 0){ps=({struct Cyc_List_List*_tmp151=_cycalloc(sizeof(*
_tmp151));_tmp151->hd=(void*)Cyc_Tcpat_compile_pat(_tmp14A);_tmp151->tl=ps;
_tmp151;});found=1;}goto _LL15C;_LL160:({void*_tmp152[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad designator(s)",
sizeof(unsigned char),18),_tag_arr(_tmp152,sizeof(void*),0));});goto _LL15C;
_LL15C:;}}}if(! found){({void*_tmp153[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad designator",sizeof(unsigned char),
15),_tag_arr(_tmp153,sizeof(void*),0));});}}}s=Cyc_Tcpat_tuple_pat(ps);goto
_LL105;}_LL11F: {int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp132->fields))->v);s=
Cyc_Tcpat_con_pat(*(*_tmp131->name).f2,({struct Cyc_Core_Opt*_tmp154=
_cycalloc_atomic(sizeof(*_tmp154));_tmp154->v=(void*)span;_tmp154;}),0);goto
_LL105;}_LL121: {struct Cyc_List_List*fields;{void*_tmp155=Cyc_Tcutil_compress(
_tmp134);struct Cyc_List_List*_tmp156;_LL165: if((unsigned int)_tmp155 > 3?*((int*)
_tmp155)== 16: 0){_LL169: _tmp156=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp155)->f1;
goto _LL166;}else{goto _LL167;}_LL167: goto _LL168;_LL166: fields=_tmp156;goto _LL164;
_LL168: fields=({void*_tmp157[0]={};((struct Cyc_List_List*(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad type in AnonEnum_p",
sizeof(unsigned char),23),_tag_arr(_tmp157,sizeof(void*),0));});goto _LL164;
_LL164:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*
_tmp133->name).f2,({struct Cyc_Core_Opt*_tmp158=_cycalloc_atomic(sizeof(*_tmp158));
_tmp158->v=(void*)span;_tmp158;}),0);goto _LL105;}}_LL123: s=({void*_tmp159[0]={};
Cyc_Tcutil_impos(_tag_arr("compile_pat: unknownid",sizeof(unsigned char),23),
_tag_arr(_tmp159,sizeof(void*),0));});goto _LL105;_LL125: s=({void*_tmp15A[0]={};
Cyc_Tcutil_impos(_tag_arr("compile_pat: unknowncall",sizeof(unsigned char),25),
_tag_arr(_tmp15A,sizeof(void*),0));});goto _LL105;_LL127: s=({void*_tmp15B[0]={};
Cyc_Tcutil_impos(_tag_arr("compile_pat: unknownfields",sizeof(unsigned char),27),
_tag_arr(_tmp15B,sizeof(void*),0));});goto _LL105;_LL105:;}return s;}struct Cyc_Tcpat_Pos_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{
int tag;struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Success_struct{int tag;void*f1;};
struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*
f2;void*f3;void*f4;};struct _tuple10{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;};static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*
c){void*_tmp15C=td;struct Cyc_Set_Set*_tmp15D;_LL16B: if(*((int*)_tmp15C)== 1){
_LL16F: _tmp15D=((struct Cyc_Tcpat_Neg_struct*)_tmp15C)->f1;goto _LL16C;}else{goto
_LL16D;}_LL16D: if(*((int*)_tmp15C)== 0){goto _LL16E;}else{goto _LL16A;}_LL16C: if(((
int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp15D,c)){({
void*_tmp15E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("add_neg called when constructor already in set",sizeof(unsigned char),
47),_tag_arr(_tmp15E,sizeof(void*),0));});}if(c->span != 0?((int(*)(struct Cyc_Set_Set*
s))Cyc_Set_cardinality)(_tmp15D)+ 1 >= (int)((struct Cyc_Core_Opt*)_check_null(c->span))->v:
0){({void*_tmp15F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("add_neg called when |cs U {c}| >= span(c)",sizeof(unsigned char),42),
_tag_arr(_tmp15F,sizeof(void*),0));});}return(void*)({struct Cyc_Tcpat_Neg_struct*
_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160[0]=({struct Cyc_Tcpat_Neg_struct
_tmp161;_tmp161.tag=1;_tmp161.f1=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,
struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp15D,c);_tmp161;});_tmp160;});
_LL16E: return({void*_tmp162[0]={};Cyc_Tcutil_impos(_tag_arr("add_neg called when td is Positive",
sizeof(unsigned char),35),_tag_arr(_tmp162,sizeof(void*),0));});_LL16A:;}static
void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){void*_tmp163=td;
struct Cyc_Tcpat_Con_s*_tmp164;struct Cyc_Set_Set*_tmp165;_LL171: if(*((int*)
_tmp163)== 0){_LL175: _tmp164=((struct Cyc_Tcpat_Pos_struct*)_tmp163)->f1;goto
_LL172;}else{goto _LL173;}_LL173: if(*((int*)_tmp163)== 1){_LL176: _tmp165=((struct
Cyc_Tcpat_Neg_struct*)_tmp163)->f1;goto _LL174;}else{goto _LL170;}_LL172: if(Cyc_Tcpat_compare_con(
c,_tmp164)== 0){return(void*)0;}else{return(void*)1;}_LL174: if(((int(*)(struct
Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp165,c)){return(void*)
1;}else{if(c->span != 0?(int)((struct Cyc_Core_Opt*)_check_null(c->span))->v == ((
int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp165)+ 1: 0){return(void*)0;}
else{return(void*)2;}}_LL170:;}struct _tuple11{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*
f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*
dsc){struct Cyc_List_List*_tmp166=ctxt;struct Cyc_List_List _tmp167;struct Cyc_List_List*
_tmp168;struct _tuple11*_tmp169;struct _tuple11 _tmp16A;struct Cyc_List_List*_tmp16B;
struct Cyc_Tcpat_Con_s*_tmp16C;_LL178: if(_tmp166 == 0){goto _LL179;}else{goto _LL17A;}
_LL17A: if(_tmp166 == 0){goto _LL177;}else{_tmp167=*_tmp166;_LL17D: _tmp169=(struct
_tuple11*)_tmp167.hd;_tmp16A=*_tmp169;_LL17F: _tmp16C=_tmp16A.f1;goto _LL17E;
_LL17E: _tmp16B=_tmp16A.f2;goto _LL17C;_LL17C: _tmp168=_tmp167.tl;goto _LL17B;}
_LL179: return 0;_LL17B: return({struct Cyc_List_List*_tmp16D=_cycalloc(sizeof(*
_tmp16D));_tmp16D->hd=({struct _tuple11*_tmp16E=_cycalloc(sizeof(*_tmp16E));
_tmp16E->f1=_tmp16C;_tmp16E->f2=(struct Cyc_List_List*)({struct Cyc_List_List*
_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->hd=(void*)dsc;_tmp16F->tl=_tmp16B;
_tmp16F;});_tmp16E;});_tmp16D->tl=_tmp168;_tmp16D;});_LL177:;}static struct Cyc_List_List*
Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp170=
ctxt;struct Cyc_List_List _tmp171;struct Cyc_List_List*_tmp172;struct _tuple11*
_tmp173;struct _tuple11 _tmp174;struct Cyc_List_List*_tmp175;struct Cyc_Tcpat_Con_s*
_tmp176;_LL181: if(_tmp170 == 0){goto _LL182;}else{goto _LL183;}_LL183: if(_tmp170 == 
0){goto _LL180;}else{_tmp171=*_tmp170;_LL186: _tmp173=(struct _tuple11*)_tmp171.hd;
_tmp174=*_tmp173;_LL188: _tmp176=_tmp174.f1;goto _LL187;_LL187: _tmp175=_tmp174.f2;
goto _LL185;_LL185: _tmp172=_tmp171.tl;goto _LL184;}_LL182: return({void*_tmp177[0]={};((
struct Cyc_List_List*(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("norm_context: empty context",sizeof(unsigned char),28),_tag_arr(
_tmp177,sizeof(void*),0));});_LL184: return Cyc_Tcpat_augment(_tmp172,(void*)({
struct Cyc_Tcpat_Pos_struct*_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178[0]=({
struct Cyc_Tcpat_Pos_struct _tmp179;_tmp179.tag=0;_tmp179.f1=_tmp176;_tmp179.f2=
Cyc_List_rev(_tmp175);_tmp179;});_tmp178;}));_LL180:;}static void*Cyc_Tcpat_build_desc(
struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){struct _tuple6 _tmp17B=({
struct _tuple6 _tmp17A;_tmp17A.f1=ctxt;_tmp17A.f2=work;_tmp17A;});struct Cyc_List_List*
_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_List_List*_tmp17E;struct Cyc_List_List*
_tmp17F;struct Cyc_List_List*_tmp180;struct Cyc_List_List _tmp181;struct Cyc_List_List*
_tmp182;struct _tuple10*_tmp183;struct _tuple10 _tmp184;struct Cyc_List_List*_tmp185;
struct Cyc_List_List*_tmp186;struct Cyc_List_List _tmp187;struct Cyc_List_List*
_tmp188;struct _tuple11*_tmp189;struct _tuple11 _tmp18A;struct Cyc_List_List*_tmp18B;
struct Cyc_Tcpat_Con_s*_tmp18C;_LL18A: _LL193: _tmp17D=_tmp17B.f1;if(_tmp17D == 0){
goto _LL192;}else{goto _LL18C;}_LL192: _tmp17C=_tmp17B.f2;if(_tmp17C == 0){goto
_LL18B;}else{goto _LL18C;}_LL18C: _LL194: _tmp17E=_tmp17B.f1;if(_tmp17E == 0){goto
_LL18D;}else{goto _LL18E;}_LL18E: _LL195: _tmp17F=_tmp17B.f2;if(_tmp17F == 0){goto
_LL18F;}else{goto _LL190;}_LL190: _LL19A: _tmp186=_tmp17B.f1;if(_tmp186 == 0){goto
_LL189;}else{_tmp187=*_tmp186;_LL19C: _tmp189=(struct _tuple11*)_tmp187.hd;_tmp18A=*
_tmp189;_LL19E: _tmp18C=_tmp18A.f1;goto _LL19D;_LL19D: _tmp18B=_tmp18A.f2;goto
_LL19B;_LL19B: _tmp188=_tmp187.tl;goto _LL196;}_LL196: _tmp180=_tmp17B.f2;if(
_tmp180 == 0){goto _LL189;}else{_tmp181=*_tmp180;_LL198: _tmp183=(struct _tuple10*)
_tmp181.hd;_tmp184=*_tmp183;_LL199: _tmp185=_tmp184.f3;goto _LL197;_LL197: _tmp182=
_tmp181.tl;goto _LL191;}_LL18B: return dsc;_LL18D: return({void*_tmp18D[0]={};Cyc_Tcutil_impos(
_tag_arr("build_desc: ctxt and work don't match",sizeof(unsigned char),38),
_tag_arr(_tmp18D,sizeof(void*),0));});_LL18F: return({void*_tmp18E[0]={};Cyc_Tcutil_impos(
_tag_arr("build_desc: ctxt and work don't match",sizeof(unsigned char),38),
_tag_arr(_tmp18E,sizeof(void*),0));});_LL191: {struct Cyc_Tcpat_Pos_struct*
_tmp18F=({struct Cyc_Tcpat_Pos_struct*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190[
0]=({struct Cyc_Tcpat_Pos_struct _tmp191;_tmp191.tag=0;_tmp191.f1=_tmp18C;_tmp191.f2=
Cyc_List_append(Cyc_List_rev(_tmp18B),({struct Cyc_List_List*_tmp192=_cycalloc(
sizeof(*_tmp192));_tmp192->hd=(void*)dsc;_tmp192->tl=_tmp185;_tmp192;}));_tmp191;});
_tmp190;});return Cyc_Tcpat_build_desc(_tmp188,(void*)_tmp18F,_tmp182);}_LL189:;}
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*
ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*
_tmp193=allmrules;struct Cyc_List_List _tmp194;struct Cyc_List_List*_tmp195;struct
_tuple0*_tmp196;struct _tuple0 _tmp197;void*_tmp198;void*_tmp199;_LL1A0: if(_tmp193
== 0){goto _LL1A1;}else{goto _LL1A2;}_LL1A2: if(_tmp193 == 0){goto _LL19F;}else{
_tmp194=*_tmp193;_LL1A5: _tmp196=(struct _tuple0*)_tmp194.hd;_tmp197=*_tmp196;
_LL1A7: _tmp199=_tmp197.f1;goto _LL1A6;_LL1A6: _tmp198=_tmp197.f2;goto _LL1A4;_LL1A4:
_tmp195=_tmp194.tl;goto _LL1A3;}_LL1A1: return(void*)0;_LL1A3: return Cyc_Tcpat_match(
_tmp199,0,dsc,0,0,_tmp198,_tmp195);_LL19F:;}static void*Cyc_Tcpat_match_compile(
struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match((void*)({struct Cyc_Tcpat_Neg_struct*
_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A[0]=({struct Cyc_Tcpat_Neg_struct
_tmp19B;_tmp19B.tag=1;_tmp19B.f1=Cyc_Tcpat_empty_con_set();_tmp19B;});_tmp19A;}),
allmrules);}static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp19C=
work;struct Cyc_List_List _tmp19D;struct Cyc_List_List*_tmp19E;struct _tuple10*
_tmp19F;struct _tuple10 _tmp1A0;struct Cyc_List_List*_tmp1A1;struct Cyc_List_List*
_tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_List_List _tmp1A4;struct Cyc_List_List*
_tmp1A5;struct _tuple10*_tmp1A6;struct _tuple10 _tmp1A7;struct Cyc_List_List*_tmp1A8;
struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1AA;_LL1A9: if(_tmp19C == 0){
goto _LL1AA;}else{goto _LL1AB;}_LL1AB: if(_tmp19C == 0){goto _LL1AD;}else{_tmp19D=*
_tmp19C;_LL1B0: _tmp19F=(struct _tuple10*)_tmp19D.hd;_tmp1A0=*_tmp19F;_LL1B3:
_tmp1A3=_tmp1A0.f1;if(_tmp1A3 == 0){goto _LL1B2;}else{goto _LL1AD;}_LL1B2: _tmp1A2=
_tmp1A0.f2;if(_tmp1A2 == 0){goto _LL1B1;}else{goto _LL1AD;}_LL1B1: _tmp1A1=_tmp1A0.f3;
if(_tmp1A1 == 0){goto _LL1AF;}else{goto _LL1AD;}_LL1AF: _tmp19E=_tmp19D.tl;goto
_LL1AC;}_LL1AD: if(_tmp19C == 0){goto _LL1A8;}else{_tmp1A4=*_tmp19C;_LL1B5: _tmp1A6=(
struct _tuple10*)_tmp1A4.hd;_tmp1A7=*_tmp1A6;_LL1B8: _tmp1AA=_tmp1A7.f1;goto _LL1B7;
_LL1B7: _tmp1A9=_tmp1A7.f2;goto _LL1B6;_LL1B6: _tmp1A8=_tmp1A7.f3;goto _LL1B4;_LL1B4:
_tmp1A5=_tmp1A4.tl;goto _LL1AE;}_LL1AA: return(void*)({struct Cyc_Tcpat_Success_struct*
_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB[0]=({struct Cyc_Tcpat_Success_struct
_tmp1AC;_tmp1AC.tag=0;_tmp1AC.f1=(void*)right_hand_side;_tmp1AC;});_tmp1AB;});
_LL1AC: return Cyc_Tcpat_and_match(Cyc_Tcpat_norm_context(ctx),_tmp19E,
right_hand_side,rules);_LL1AE: if((_tmp1AA == 0? 1: _tmp1A9 == 0)? 1: _tmp1A8 == 0){
return({void*_tmp1AD[0]={};Cyc_Tcutil_impos(_tag_arr("tcpat:and_match: malformed work frame",
sizeof(unsigned char),38),_tag_arr(_tmp1AD,sizeof(void*),0));});}{struct Cyc_List_List
_tmp1AF;struct Cyc_List_List*_tmp1B0;void*_tmp1B1;struct Cyc_List_List*_tmp1AE=(
struct Cyc_List_List*)_check_null(_tmp1AA);_tmp1AF=*_tmp1AE;_LL1BB: _tmp1B1=(void*)
_tmp1AF.hd;goto _LL1BA;_LL1BA: _tmp1B0=_tmp1AF.tl;goto _LL1B9;_LL1B9: {struct Cyc_List_List
_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B5;struct Cyc_List_List*
_tmp1B2=(struct Cyc_List_List*)_check_null(_tmp1A9);_tmp1B3=*_tmp1B2;_LL1BE:
_tmp1B5=(struct Cyc_List_List*)_tmp1B3.hd;goto _LL1BD;_LL1BD: _tmp1B4=_tmp1B3.tl;
goto _LL1BC;_LL1BC: {struct Cyc_List_List _tmp1B7;struct Cyc_List_List*_tmp1B8;void*
_tmp1B9;struct Cyc_List_List*_tmp1B6=(struct Cyc_List_List*)_check_null(_tmp1A8);
_tmp1B7=*_tmp1B6;_LL1C1: _tmp1B9=(void*)_tmp1B7.hd;goto _LL1C0;_LL1C0: _tmp1B8=
_tmp1B7.tl;goto _LL1BF;_LL1BF: {struct _tuple10*_tmp1BA=({struct _tuple10*_tmp1BC=
_cycalloc(sizeof(*_tmp1BC));_tmp1BC->f1=_tmp1B0;_tmp1BC->f2=_tmp1B4;_tmp1BC->f3=
_tmp1B8;_tmp1BC;});return Cyc_Tcpat_match(_tmp1B1,_tmp1B5,_tmp1B9,ctx,({struct Cyc_List_List*
_tmp1BB=_cycalloc(sizeof(*_tmp1BB));_tmp1BB->hd=_tmp1BA;_tmp1BB->tl=_tmp1A5;
_tmp1BB;}),right_hand_side,rules);}}}}_LL1A8:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp1BD=dsc;struct Cyc_Set_Set*_tmp1BE;
struct Cyc_List_List*_tmp1BF;_LL1C3: if(*((int*)_tmp1BD)== 1){_LL1C7: _tmp1BE=((
struct Cyc_Tcpat_Neg_struct*)_tmp1BD)->f1;goto _LL1C4;}else{goto _LL1C5;}_LL1C5: if(*((
int*)_tmp1BD)== 0){_LL1C8: _tmp1BF=((struct Cyc_Tcpat_Pos_struct*)_tmp1BD)->f2;
goto _LL1C6;}else{goto _LL1C2;}_LL1C4: {void*any=(void*)({struct Cyc_Tcpat_Neg_struct*
_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2[0]=({struct Cyc_Tcpat_Neg_struct
_tmp1C3;_tmp1C3.tag=1;_tmp1C3.f1=Cyc_Tcpat_empty_con_set();_tmp1C3;});_tmp1C2;});
struct Cyc_List_List*_tmp1C0=0;{int i=0;for(0;i < pcon->arity;++ i){_tmp1C0=({struct
Cyc_List_List*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->hd=(void*)any;_tmp1C1->tl=
_tmp1C0;_tmp1C1;});}}return _tmp1C0;}_LL1C6: return _tmp1BF;_LL1C2:;}static struct
Cyc_List_List*Cyc_Tcpat_getoarg(struct Cyc_List_List*obj,int i){return({struct Cyc_List_List*
_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=(void*)(i + 1);_tmp1C4->tl=obj;
_tmp1C4;});}static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*
pcon,struct Cyc_List_List*obj){return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*
f)(struct Cyc_List_List*,int),struct Cyc_List_List*env))Cyc_List_tabulate_c)(pcon->arity,
Cyc_Tcpat_getoarg,obj);}static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*
obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){void*_tmp1C5=p;struct Cyc_List_List*
_tmp1C6;struct Cyc_Tcpat_Con_s*_tmp1C7;_LL1CA: if((int)_tmp1C5 == 0){goto _LL1CB;}
else{goto _LL1CC;}_LL1CC: if((unsigned int)_tmp1C5 > 1?*((int*)_tmp1C5)== 0: 0){
_LL1CF: _tmp1C7=((struct Cyc_Tcpat_Con_struct*)_tmp1C5)->f1;goto _LL1CE;_LL1CE:
_tmp1C6=((struct Cyc_Tcpat_Con_struct*)_tmp1C5)->f2;goto _LL1CD;}else{goto _LL1C9;}
_LL1CB: return Cyc_Tcpat_and_match(Cyc_Tcpat_augment(ctx,dsc),work,right_hand_side,
rules);_LL1CD: {void*_tmp1C8=Cyc_Tcpat_static_match(_tmp1C7,dsc);_LL1D1: if((int)
_tmp1C8 == 0){goto _LL1D2;}else{goto _LL1D3;}_LL1D3: if((int)_tmp1C8 == 1){goto _LL1D4;}
else{goto _LL1D5;}_LL1D5: if((int)_tmp1C8 == 2){goto _LL1D6;}else{goto _LL1D0;}_LL1D2: {
struct Cyc_List_List*_tmp1C9=({struct Cyc_List_List*_tmp1CE=_cycalloc(sizeof(*
_tmp1CE));_tmp1CE->hd=({struct _tuple11*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));
_tmp1CF->f1=_tmp1C7;_tmp1CF->f2=0;_tmp1CF;});_tmp1CE->tl=ctx;_tmp1CE;});struct
_tuple10*_tmp1CA=({struct _tuple10*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->f1=
_tmp1C6;_tmp1CD->f2=Cyc_Tcpat_getoargs(_tmp1C7,obj);_tmp1CD->f3=Cyc_Tcpat_getdargs(
_tmp1C7,dsc);_tmp1CD;});struct Cyc_List_List*_tmp1CB=({struct Cyc_List_List*
_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->hd=_tmp1CA;_tmp1CC->tl=work;_tmp1CC;});
return Cyc_Tcpat_and_match(_tmp1C9,_tmp1CB,right_hand_side,rules);}_LL1D4: return
Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(ctx,dsc,work),rules);_LL1D6: {struct Cyc_List_List*
_tmp1D0=({struct Cyc_List_List*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->hd=({
struct _tuple11*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->f1=_tmp1C7;_tmp1DA->f2=
0;_tmp1DA;});_tmp1D9->tl=ctx;_tmp1D9;});struct _tuple10*_tmp1D1=({struct _tuple10*
_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->f1=_tmp1C6;_tmp1D8->f2=Cyc_Tcpat_getoargs(
_tmp1C7,obj);_tmp1D8->f3=Cyc_Tcpat_getdargs(_tmp1C7,dsc);_tmp1D8;});struct Cyc_List_List*
_tmp1D2=({struct Cyc_List_List*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->hd=
_tmp1D1;_tmp1D7->tl=work;_tmp1D7;});void*_tmp1D3=Cyc_Tcpat_and_match(_tmp1D0,
_tmp1D2,right_hand_side,rules);void*_tmp1D4=Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(
ctx,Cyc_Tcpat_add_neg(dsc,_tmp1C7),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*
_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp1D6;_tmp1D6.tag=1;_tmp1D6.f1=obj;_tmp1D6.f2=_tmp1C7;_tmp1D6.f3=(void*)
_tmp1D3;_tmp1D6.f4=(void*)_tmp1D4;_tmp1D6;});_tmp1D5;});}_LL1D0:;}_LL1C9:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*),void*
env1,void(*rhs_appears)(void*,void*),void*env2){void*_tmp1DB=d;void*_tmp1DC;void*
_tmp1DD;void*_tmp1DE;_LL1D8: if((int)_tmp1DB == 0){goto _LL1D9;}else{goto _LL1DA;}
_LL1DA: if((unsigned int)_tmp1DB > 1?*((int*)_tmp1DB)== 0: 0){_LL1DE: _tmp1DC=(void*)((
struct Cyc_Tcpat_Success_struct*)_tmp1DB)->f1;goto _LL1DB;}else{goto _LL1DC;}_LL1DC:
if((unsigned int)_tmp1DB > 1?*((int*)_tmp1DB)== 1: 0){_LL1E0: _tmp1DE=(void*)((
struct Cyc_Tcpat_IfEq_struct*)_tmp1DB)->f3;goto _LL1DF;_LL1DF: _tmp1DD=(void*)((
struct Cyc_Tcpat_IfEq_struct*)_tmp1DB)->f4;goto _LL1DD;}else{goto _LL1D7;}_LL1D9:
not_exhaust(env1);goto _LL1D7;_LL1DB: rhs_appears(env2,_tmp1DC);goto _LL1D7;_LL1DD:
Cyc_Tcpat_check_exhaust_overlap(_tmp1DE,not_exhaust,env1,rhs_appears,env2);Cyc_Tcpat_check_exhaust_overlap(
_tmp1DD,not_exhaust,env1,rhs_appears,env2);goto _LL1D7;_LL1D7:;}struct _tuple12{
int f1;struct Cyc_Position_Segment*f2;};struct _tuple13{void*f1;struct _tuple12*f2;}
;static struct _tuple13*Cyc_Tcpat_get_match(int*r,struct Cyc_Absyn_Switch_clause*
swc){void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple12*rhs=({struct
_tuple12*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4->f1=0;_tmp1E4->f2=(swc->pattern)->loc;
_tmp1E4;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*
_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF->hd=(void*)sp0;_tmp1DF->tl=({struct
Cyc_List_List*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->hd=(void*)Cyc_Tcpat_int_pat(*
r);_tmp1E0->tl=0;_tmp1E0;});_tmp1DF;}));*r=*r + 1;}else{sp=Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->hd=(void*)sp0;
_tmp1E1->tl=({struct Cyc_List_List*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2->hd=(
void*)((void*)0);_tmp1E2->tl=0;_tmp1E2;});_tmp1E1;}));}return({struct _tuple13*
_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->f1=sp;_tmp1E3->f2=rhs;_tmp1E3;});}
static void Cyc_Tcpat_not_exhaust_err(struct Cyc_Position_Segment*loc){({void*
_tmp1E5[0]={};Cyc_Tcutil_terr(loc,_tag_arr("patterns may not be exhaustive",
sizeof(unsigned char),31),_tag_arr(_tmp1E5,sizeof(void*),0));});}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple12*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*swcs){int _tmp1E6=0;struct Cyc_List_List*
_tmp1E7=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(int*,struct Cyc_Absyn_Switch_clause*),
int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,& _tmp1E6,swcs);
void*_tmp1E8=((void*(*)(struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(
_tmp1E7);((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),struct
Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple12*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp1E8,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp1E7 != 0;
_tmp1E7=_tmp1E7->tl){struct _tuple13 _tmp1EA;struct _tuple12*_tmp1EB;struct _tuple12
_tmp1EC;struct Cyc_Position_Segment*_tmp1ED;int _tmp1EE;struct _tuple13*_tmp1E9=(
struct _tuple13*)_tmp1E7->hd;_tmp1EA=*_tmp1E9;_LL1E2: _tmp1EB=_tmp1EA.f2;_tmp1EC=*
_tmp1EB;_LL1E4: _tmp1EE=_tmp1EC.f1;goto _LL1E3;_LL1E3: _tmp1ED=_tmp1EC.f2;goto
_LL1E1;_LL1E1: if(! _tmp1EE){({void*_tmp1EF[0]={};Cyc_Tcutil_terr(_tmp1ED,_tag_arr("redundant pattern",
sizeof(unsigned char),18),_tag_arr(_tmp1EF,sizeof(void*),0));});}}}struct
_tuple14{struct Cyc_Position_Segment*f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(
struct _tuple14*pr){(*pr).f2=0;({void*_tmp1F0[0]={};Cyc_Tcutil_warn((*pr).f1,
_tag_arr("pattern not exhaustive",sizeof(unsigned char),23),_tag_arr(_tmp1F0,
sizeof(void*),0));});}static void Cyc_Tcpat_dummy_fn(int i,int j){return;}struct
_tuple15{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Pat*p){struct Cyc_List_List*_tmp1F1=({struct Cyc_List_List*
_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->hd=({struct _tuple15*_tmp1F6=
_cycalloc(sizeof(*_tmp1F6));_tmp1F6->f1=Cyc_Tcpat_compile_pat(p);_tmp1F6->f2=0;
_tmp1F6;});_tmp1F5->tl=0;_tmp1F5;});void*_tmp1F2=((void*(*)(struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp1F1);struct _tuple14*_tmp1F3=({struct
_tuple14*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4->f1=loc;_tmp1F4->f2=1;
_tmp1F4;});((void(*)(void*d,void(*not_exhaust)(struct _tuple14*),struct _tuple14*
env1,void(*rhs_appears)(int,int),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp1F2,Cyc_Tcpat_not_exhaust_warn,_tmp1F3,Cyc_Tcpat_dummy_fn,0);return(*_tmp1F3).f2;}
static struct _tuple13*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple12*rhs=({struct _tuple12*
_tmp1F8=_cycalloc(sizeof(*_tmp1F8));_tmp1F8->f1=0;_tmp1F8->f2=(swc->pattern)->loc;
_tmp1F8;});return({struct _tuple13*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7->f1=
sp0;_tmp1F7->f2=rhs;_tmp1F7;});}static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment*
loc){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,struct
Cyc_List_List*swcs){struct Cyc_List_List*_tmp1F9=((struct Cyc_List_List*(*)(struct
_tuple13*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp1FA=((void*(*)(struct Cyc_List_List*allmrules))
Cyc_Tcpat_match_compile)(_tmp1F9);((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),
struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple12*),int env2))
Cyc_Tcpat_check_exhaust_overlap)(_tmp1FA,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,
0);for(0;_tmp1F9 != 0;_tmp1F9=_tmp1F9->tl){struct _tuple13 _tmp1FC;struct _tuple12*
_tmp1FD;struct _tuple12 _tmp1FE;struct Cyc_Position_Segment*_tmp1FF;int _tmp200;
struct _tuple13*_tmp1FB=(struct _tuple13*)_tmp1F9->hd;_tmp1FC=*_tmp1FB;_LL1E6:
_tmp1FD=_tmp1FC.f2;_tmp1FE=*_tmp1FD;_LL1E8: _tmp200=_tmp1FE.f1;goto _LL1E7;_LL1E7:
_tmp1FF=_tmp1FE.f2;goto _LL1E5;_LL1E5: if(! _tmp200){({void*_tmp201[0]={};Cyc_Tcutil_terr(
_tmp1FF,_tag_arr("redundant pattern",sizeof(unsigned char),18),_tag_arr(_tmp201,
sizeof(void*),0));});}}}
