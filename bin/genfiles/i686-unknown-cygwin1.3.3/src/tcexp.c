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
Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);extern struct Cyc_List_List*
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
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _tagged_arr*);extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_struct_field(struct Cyc_Absyn_Structdecl*,struct _tagged_arr*);
extern struct Cyc_Absyn_Structfield*Cyc_Absyn_lookup_union_field(struct Cyc_Absyn_Uniondecl*,
struct _tagged_arr*);extern int Cyc_Std_strcmp(struct _tagged_arr s1,struct
_tagged_arr s2);extern int Cyc_Std_zstrcmp(struct _tagged_arr,struct _tagged_arr);
struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
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
void*f1;};struct Cyc_Tcenv_StructRes_struct{int tag;struct Cyc_Absyn_Structdecl*f1;
};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct
Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
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
struct _tuple1*);extern struct Cyc_Absyn_Structdecl**Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);extern struct
Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);extern
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct Cyc_Position_Segment*,
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
Cyc_Absyn_Tunion_e_struct _tmp14;_tmp14.tag=30;_tmp14.f1=0;_tmp14.f2=0;_tmp14.f3=
0;_tmp14.f4=_tmpC;_tmp14.f5=_tmpB;_tmp14;});_tmp13;})));goto _LL0;_LLA:({struct
Cyc_Std_String_pa_struct _tmp16;_tmp16.tag=0;_tmp16.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
q);{void*_tmp15[1]={& _tmp16};Cyc_Tcutil_terr(e->loc,_tag_arr("bad occurrence of struct name %s",
sizeof(unsigned char),33),_tag_arr(_tmp15,sizeof(void*),1));}});(void*)(e->r=(
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
struct Cyc_Absyn_Structdecl*_tmp27;_LL1E: if(*((int*)_tmp23)== 0){_LL28: _tmp24=(
void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp23)->f1;goto _LL1F;}else{goto _LL20;}
_LL20: if(*((int*)_tmp23)== 2){_LL2A: _tmp26=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp23)->f1;goto _LL29;_LL29: _tmp25=((struct Cyc_Tcenv_TunionRes_struct*)_tmp23)->f2;
goto _LL21;}else{goto _LL22;}_LL22: if(*((int*)_tmp23)== 1){_LL2B: _tmp27=((struct
Cyc_Tcenv_StructRes_struct*)_tmp23)->f1;goto _LL23;}else{goto _LL24;}_LL24: if(*((
int*)_tmp23)== 4){goto _LL25;}else{goto _LL26;}_LL26: if(*((int*)_tmp23)== 3){goto
_LL27;}else{goto _LL1D;}_LL1F:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp29;_tmp29.tag=9;_tmp29.f1=e1;_tmp29.f2=es;_tmp29.f3=0;_tmp29;});_tmp28;})));
_npop_handler(0);return;_LL21: if(_tmp25->typs == 0){({struct Cyc_Std_String_pa_struct
_tmp2B;_tmp2B.tag=0;_tmp2B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp25->name);{
void*_tmp2A[1]={& _tmp2B};Cyc_Tcutil_terr(e->loc,_tag_arr("%s is a constant, not a function",
sizeof(unsigned char),33),_tag_arr(_tmp2A,sizeof(void*),1));}});}(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));
_tmp2C[0]=({struct Cyc_Absyn_Tunion_e_struct _tmp2D;_tmp2D.tag=30;_tmp2D.f1=0;
_tmp2D.f2=0;_tmp2D.f3=es;_tmp2D.f4=_tmp26;_tmp2D.f5=_tmp25;_tmp2D;});_tmp2C;})));
_npop_handler(0);return;_npop_handler(0);return;_LL23: {struct Cyc_List_List*
_tmp2E=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);if(_tmp27->name != 0){(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp2F=_cycalloc(
sizeof(*_tmp2F));_tmp2F[0]=({struct Cyc_Absyn_Struct_e_struct _tmp30;_tmp30.tag=28;
_tmp30.f1=(struct _tuple1*)((struct Cyc_Core_Opt*)_check_null(_tmp27->name))->v;
_tmp30.f2=0;_tmp30.f3=_tmp2E;_tmp30.f4=(struct Cyc_Absyn_Structdecl*)_tmp27;
_tmp30;});_tmp2F;})));}else{({void*_tmp31[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("missing struct name",
sizeof(unsigned char),20),_tag_arr(_tmp31,sizeof(void*),0));});(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp32=_cycalloc(sizeof(*_tmp32));
_tmp32[0]=({struct Cyc_Absyn_Struct_e_struct _tmp33;_tmp33.tag=28;_tmp33.f1=_tmp1F;
_tmp33.f2=0;_tmp33.f3=_tmp2E;_tmp33.f4=(struct Cyc_Absyn_Structdecl*)_tmp27;
_tmp33;});_tmp32;})));}_npop_handler(0);return;}_LL25: goto _LL27;_LL27:({struct
Cyc_Std_String_pa_struct _tmp35;_tmp35.tag=0;_tmp35.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp1F);{void*_tmp34[1]={& _tmp35};Cyc_Tcutil_terr(e->loc,_tag_arr("%s is an enum constructor, not a function",
sizeof(unsigned char),42),_tag_arr(_tmp34,sizeof(void*),1));}});_npop_handler(0);
return;_LL1D:;};_pop_handler();}else{void*_tmp21=(void*)_exn_thrown;void*_tmp37=
_tmp21;_LL2D: if(_tmp37 == Cyc_Dict_Absent){goto _LL2E;}else{goto _LL2F;}_LL2F: goto
_LL30;_LL2E:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp38=
_cycalloc(sizeof(*_tmp38));_tmp38[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp39;
_tmp39.tag=9;_tmp39.f1=e1;_tmp39.f2=es;_tmp39.f3=0;_tmp39;});_tmp38;})));return;
_LL30:(void)_throw(_tmp37);_LL2C:;}}}_LL1B:(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_FnCall_e_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[0]=({struct
Cyc_Absyn_FnCall_e_struct _tmp3B;_tmp3B.tag=9;_tmp3B.f1=e1;_tmp3B.f2=es;_tmp3B.f3=
0;_tmp3B;});_tmp3A;})));return;_LL17:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3D;_tmp3D.tag=26;_tmp3D.f1=des;_tmp3D;});_tmp3C;})));return;}{void*t=*topt;
void*_tmp3E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tqual _tmp3F;void*_tmp40;
struct Cyc_Absyn_Structdecl**_tmp41;struct Cyc_List_List*_tmp42;struct _tuple1*
_tmp43;_LL32: if((unsigned int)_tmp3E > 3?*((int*)_tmp3E)== 7: 0){_LL3B: _tmp40=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3E)->f1;goto _LL3A;_LL3A: _tmp3F=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3E)->f2;goto _LL33;}else{goto _LL34;}_LL34:
if((unsigned int)_tmp3E > 3?*((int*)_tmp3E)== 10: 0){_LL3E: _tmp43=((struct Cyc_Absyn_StructType_struct*)
_tmp3E)->f1;goto _LL3D;_LL3D: _tmp42=((struct Cyc_Absyn_StructType_struct*)_tmp3E)->f2;
goto _LL3C;_LL3C: _tmp41=((struct Cyc_Absyn_StructType_struct*)_tmp3E)->f3;goto
_LL35;}else{goto _LL36;}_LL36: if((unsigned int)_tmp3E > 3?*((int*)_tmp3E)== 14: 0){
goto _LL37;}else{goto _LL38;}_LL38: goto _LL39;_LL33:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Array_e_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({
struct Cyc_Absyn_Array_e_struct _tmp45;_tmp45.tag=26;_tmp45.f1=des;_tmp45;});
_tmp44;})));goto _LL31;_LL35: if(_tmp41 == 0? 1: _tmp43 == 0){({void*_tmp46[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("Compiler bug: struct type not properly set",sizeof(unsigned char),
43),_tag_arr(_tmp46,sizeof(void*),0));});}(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_Struct_e_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({struct
Cyc_Absyn_Struct_e_struct _tmp48;_tmp48.tag=28;_tmp48.f1=(struct _tuple1*)
_check_null(_tmp43);_tmp48.f2=0;_tmp48.f3=des;_tmp48.f4=(struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**)_check_null(_tmp41));_tmp48;});_tmp47;})));goto
_LL31;_LL37:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*
_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_AnonStruct_e_struct
_tmp4A;_tmp4A.tag=29;_tmp4A.f1=(void*)t;_tmp4A.f2=des;_tmp4A;});_tmp49;})));goto
_LL31;_LL39:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp4B=
_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=({struct Cyc_Absyn_Array_e_struct _tmp4C;
_tmp4C.tag=26;_tmp4C.f1=des;_tmp4C;});_tmp4B;})));goto _LL31;_LL31:;}}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_tagged_arr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(te,(
void**)& Cyc_Absyn_sint_t,e);if(! Cyc_Tcutil_coerce_to_bool(te,e)){({struct Cyc_Std_String_pa_struct
_tmp4F;_tmp4F.tag=0;_tmp4F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp4E;_tmp4E.tag=0;_tmp4E.f1=(struct _tagged_arr)msg_part;{void*_tmp4D[2]={&
_tmp4E,& _tmp4F};Cyc_Tcutil_terr(e->loc,_tag_arr("test of %s has type %s instead of integral or * type",
sizeof(unsigned char),53),_tag_arr(_tmp4D,sizeof(void*),2));}}});}}static void*
Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,void*c,struct Cyc_Absyn_Exp*e){void*t;{void*_tmp50=c;void*_tmp51;void*_tmp52;
void*_tmp53;int _tmp54;void*_tmp55;struct _tagged_arr _tmp56;_LL40: if((unsigned int)
_tmp50 > 1?*((int*)_tmp50)== 0: 0){_LL4E: _tmp51=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp50)->f1;goto _LL41;}else{goto _LL42;}_LL42: if((unsigned int)_tmp50 > 1?*((int*)
_tmp50)== 1: 0){_LL4F: _tmp52=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp50)->f1;
goto _LL43;}else{goto _LL44;}_LL44: if((unsigned int)_tmp50 > 1?*((int*)_tmp50)== 3:
0){_LL50: _tmp53=(void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp50)->f1;goto
_LL45;}else{goto _LL46;}_LL46: if((unsigned int)_tmp50 > 1?*((int*)_tmp50)== 4: 0){
goto _LL47;}else{goto _LL48;}_LL48: if((unsigned int)_tmp50 > 1?*((int*)_tmp50)== 2:
0){_LL52: _tmp55=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp50)->f1;goto _LL51;
_LL51: _tmp54=((struct Cyc_Absyn_Int_c_struct*)_tmp50)->f2;goto _LL49;}else{goto
_LL4A;}_LL4A: if((unsigned int)_tmp50 > 1?*((int*)_tmp50)== 5: 0){_LL53: _tmp56=((
struct Cyc_Absyn_String_c_struct*)_tmp50)->f1;goto _LL4B;}else{goto _LL4C;}_LL4C:
if((int)_tmp50 == 0){goto _LL4D;}else{goto _LL3F;}_LL41: t=_tmp51 == (void*)1? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t;goto _LL3F;_LL43: t=_tmp52 == (void*)1? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL3F;_LL45: t=_tmp53 == (void*)1? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t;
goto _LL3F;_LL47: t=Cyc_Absyn_float_typ;goto _LL3F;_LL49: if(topt == 0){t=_tmp55 == (
void*)1? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}else{void*_tmp57=Cyc_Tcutil_compress(*
topt);void*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;void*_tmp5C;_LL55: if((
unsigned int)_tmp57 > 3?*((int*)_tmp57)== 5: 0){_LL60: _tmp59=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp57)->f1;goto _LL5F;_LL5F: _tmp58=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp57)->f2;if((int)_tmp58 == 0){goto _LL56;}else{goto _LL57;}}else{goto _LL57;}
_LL57: if((unsigned int)_tmp57 > 3?*((int*)_tmp57)== 5: 0){_LL62: _tmp5B=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp57)->f1;goto _LL61;_LL61: _tmp5A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp57)->f2;if((int)_tmp5A == 1){goto _LL58;}else{
goto _LL59;}}else{goto _LL59;}_LL59: if((unsigned int)_tmp57 > 3?*((int*)_tmp57)== 5:
0){_LL63: _tmp5C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp57)->f1;goto _LL5A;}
else{goto _LL5B;}_LL5B: if((unsigned int)_tmp57 > 3?*((int*)_tmp57)== 4: 0){goto
_LL64;}else{goto _LL5D;}_LL64: if(_tmp54 == 0){goto _LL5C;}else{goto _LL5D;}_LL5D:
goto _LL5E;_LL56: t=_tmp59 == (void*)1? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp5D=_cycalloc(sizeof(*
_tmp5D));_tmp5D[0]=({struct Cyc_Absyn_Const_e_struct _tmp5E;_tmp5E.tag=0;_tmp5E.f1=(
void*)((void*)({struct Cyc_Absyn_Char_c_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));
_tmp5F[0]=({struct Cyc_Absyn_Char_c_struct _tmp60;_tmp60.tag=0;_tmp60.f1=(void*)
_tmp59;_tmp60.f2=(unsigned char)_tmp54;_tmp60;});_tmp5F;}));_tmp5E;});_tmp5D;})));
goto _LL54;_LL58: t=_tmp5B == (void*)1? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp61=_cycalloc(sizeof(*
_tmp61));_tmp61[0]=({struct Cyc_Absyn_Const_e_struct _tmp62;_tmp62.tag=0;_tmp62.f1=(
void*)((void*)({struct Cyc_Absyn_Short_c_struct*_tmp63=_cycalloc(sizeof(*_tmp63));
_tmp63[0]=({struct Cyc_Absyn_Short_c_struct _tmp64;_tmp64.tag=1;_tmp64.f1=(void*)
_tmp5B;_tmp64.f2=(short)_tmp54;_tmp64;});_tmp63;}));_tmp62;});_tmp61;})));goto
_LL54;_LL5A: t=_tmp5C == (void*)1? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;goto _LL54;
_LL5C:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp65=
_cycalloc(sizeof(*_tmp65));_tmp65[0]=({struct Cyc_Absyn_Const_e_struct _tmp66;
_tmp66.tag=0;_tmp66.f1=(void*)((void*)0);_tmp66;});_tmp65;})));{struct Cyc_List_List*
_tmp67=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68[0]=({struct Cyc_Absyn_PointerType_struct
_tmp69;_tmp69.tag=4;_tmp69.f1=({struct Cyc_Absyn_PtrInfo _tmp6A;_tmp6A.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->v=_tmp67;_tmp6C;}));_tmp6A.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->v=_tmp67;_tmp6B;}));_tmp6A.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp6A.tq=Cyc_Absyn_empty_tqual();
_tmp6A.bounds=Cyc_Absyn_empty_conref();_tmp6A;});_tmp69;});_tmp68;});goto _LL54;}
_LL5E: t=_tmp55 == (void*)1? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;goto _LL54;_LL54:;}
goto _LL3F;_LL4B: {int len=(int)_get_arr_size(_tmp56,sizeof(unsigned char));struct
Cyc_Absyn_Const_e_struct*_tmp6D=({struct Cyc_Absyn_Const_e_struct*_tmp82=
_cycalloc(sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Absyn_Const_e_struct _tmp83;
_tmp83.tag=0;_tmp83.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp84=
_cycalloc(sizeof(*_tmp84));_tmp84[0]=({struct Cyc_Absyn_Int_c_struct _tmp85;_tmp85.tag=
2;_tmp85.f1=(void*)((void*)1);_tmp85.f2=len;_tmp85;});_tmp84;}));_tmp83;});
_tmp82;});struct Cyc_Absyn_Exp*elen=Cyc_Absyn_new_exp((void*)_tmp6D,loc);elen->topt=({
struct Cyc_Core_Opt*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->v=(void*)Cyc_Absyn_uint_t;
_tmp6E;});t=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,(void*)2,Cyc_Absyn_const_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[
0]=({struct Cyc_Absyn_Upper_b_struct _tmp70;_tmp70.tag=0;_tmp70.f1=elen;_tmp70;});
_tmp6F;}));if(topt != 0){void*_tmp71=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Exp*
_tmp72;struct Cyc_Absyn_Tqual _tmp73;void*_tmp74;struct Cyc_Absyn_PtrInfo _tmp75;
struct Cyc_Absyn_Conref*_tmp76;struct Cyc_Absyn_Tqual _tmp77;struct Cyc_Absyn_Conref*
_tmp78;void*_tmp79;void*_tmp7A;_LL66: if((unsigned int)_tmp71 > 3?*((int*)_tmp71)
== 7: 0){_LL6E: _tmp74=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp71)->f1;goto
_LL6D;_LL6D: _tmp73=((struct Cyc_Absyn_ArrayType_struct*)_tmp71)->f2;goto _LL6C;
_LL6C: _tmp72=((struct Cyc_Absyn_ArrayType_struct*)_tmp71)->f3;goto _LL67;}else{
goto _LL68;}_LL68: if((unsigned int)_tmp71 > 3?*((int*)_tmp71)== 4: 0){_LL6F: _tmp75=((
struct Cyc_Absyn_PointerType_struct*)_tmp71)->f1;_LL74: _tmp7A=(void*)_tmp75.elt_typ;
goto _LL73;_LL73: _tmp79=(void*)_tmp75.rgn_typ;goto _LL72;_LL72: _tmp78=_tmp75.nullable;
goto _LL71;_LL71: _tmp77=_tmp75.tq;goto _LL70;_LL70: _tmp76=_tmp75.bounds;goto _LL69;}
else{goto _LL6A;}_LL6A: goto _LL6B;_LL67: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp7C;_tmp7C.tag=7;_tmp7C.f1=(void*)Cyc_Absyn_uchar_t;_tmp7C.f2=_tmp73;_tmp7C.f3=(
struct Cyc_Absyn_Exp*)elen;_tmp7C;});_tmp7B;});_LL69: if(! Cyc_Tcutil_unify(*topt,t)?
Cyc_Tcutil_silent_castable(te,loc,t,*topt): 0){e->topt=({struct Cyc_Core_Opt*
_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->v=(void*)t;_tmp7D;});Cyc_Tcutil_unchecked_cast(
te,e,*topt);t=*topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp7E=_cycalloc(sizeof(*
_tmp7E));_tmp7E->v=Cyc_Tcenv_lookup_type_vars(te);_tmp7E;})),Cyc_Absyn_const_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[
0]=({struct Cyc_Absyn_Upper_b_struct _tmp80;_tmp80.tag=0;_tmp80.f1=elen;_tmp80;});
_tmp7F;}));if(! Cyc_Tcutil_unify(*topt,t)? Cyc_Tcutil_silent_castable(te,loc,t,*
topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->v=(
void*)t;_tmp81;});Cyc_Tcutil_unchecked_cast(te,e,*topt);t=*topt;}}goto _LL65;
_LL6B: goto _LL65;_LL65:;}return t;}_LL4D: {struct Cyc_List_List*_tmp86=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp87=_cycalloc(sizeof(*
_tmp87));_tmp87[0]=({struct Cyc_Absyn_PointerType_struct _tmp88;_tmp88.tag=4;
_tmp88.f1=({struct Cyc_Absyn_PtrInfo _tmp89;_tmp89.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp8B=_cycalloc(sizeof(*
_tmp8B));_tmp8B->v=_tmp86;_tmp8B;}));_tmp89.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp8A=_cycalloc(sizeof(*
_tmp8A));_tmp8A->v=_tmp86;_tmp8A;}));_tmp89.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmp89.tq=Cyc_Absyn_empty_tqual();_tmp89.bounds=
Cyc_Absyn_empty_conref();_tmp89;});_tmp88;});_tmp87;});goto _LL3F;}_LL3F:;}return
t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp8C=b;struct Cyc_Absyn_Vardecl*_tmp8D;struct
Cyc_Absyn_Fndecl*_tmp8E;struct Cyc_Absyn_Vardecl*_tmp8F;struct Cyc_Absyn_Vardecl*
_tmp90;struct Cyc_Absyn_Vardecl*_tmp91;_LL76: if((int)_tmp8C == 0){goto _LL77;}else{
goto _LL78;}_LL78: if((unsigned int)_tmp8C > 1?*((int*)_tmp8C)== 0: 0){_LL82: _tmp8D=((
struct Cyc_Absyn_Global_b_struct*)_tmp8C)->f1;goto _LL79;}else{goto _LL7A;}_LL7A:
if((unsigned int)_tmp8C > 1?*((int*)_tmp8C)== 1: 0){_LL83: _tmp8E=((struct Cyc_Absyn_Funname_b_struct*)
_tmp8C)->f1;goto _LL7B;}else{goto _LL7C;}_LL7C: if((unsigned int)_tmp8C > 1?*((int*)
_tmp8C)== 4: 0){_LL84: _tmp8F=((struct Cyc_Absyn_Pat_b_struct*)_tmp8C)->f1;goto
_LL7D;}else{goto _LL7E;}_LL7E: if((unsigned int)_tmp8C > 1?*((int*)_tmp8C)== 3: 0){
_LL85: _tmp90=((struct Cyc_Absyn_Local_b_struct*)_tmp8C)->f1;goto _LL7F;}else{goto
_LL80;}_LL80: if((unsigned int)_tmp8C > 1?*((int*)_tmp8C)== 2: 0){_LL86: _tmp91=((
struct Cyc_Absyn_Param_b_struct*)_tmp8C)->f1;goto _LL81;}else{goto _LL75;}_LL77:
return({struct Cyc_Std_String_pa_struct _tmp93;_tmp93.tag=0;_tmp93.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(q);{void*_tmp92[1]={& _tmp93};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("undeclared identifier: %s",sizeof(unsigned char),26),_tag_arr(
_tmp92,sizeof(void*),1));}});_LL79: q[0]=(_tmp8D->name)[
_check_known_subscript_notnull(1,0)];return(void*)_tmp8D->type;_LL7B: q[0]=(
_tmp8E->name)[_check_known_subscript_notnull(1,0)];return Cyc_Tcutil_fndecl2typ(
_tmp8E);_LL7D: _tmp90=_tmp8F;goto _LL7F;_LL7F: _tmp91=_tmp90;goto _LL81;_LL81:(*q).f1=(
void*)0;return(void*)_tmp91->type;_LL75:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _tagged_arr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmp94=(void*)
fmt->r;void*_tmp95;struct _tagged_arr _tmp96;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp
_tmp98;void*_tmp99;void*_tmp9A;struct _tagged_arr _tmp9B;_LL88: if(*((int*)_tmp94)
== 0){_LL8E: _tmp95=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp94)->f1;if((
unsigned int)_tmp95 > 1?*((int*)_tmp95)== 5: 0){_LL8F: _tmp96=((struct Cyc_Absyn_String_c_struct*)
_tmp95)->f1;goto _LL89;}else{goto _LL8A;}}else{goto _LL8A;}_LL8A: if(*((int*)_tmp94)
== 13){_LL90: _tmp97=((struct Cyc_Absyn_Cast_e_struct*)_tmp94)->f2;_tmp98=*_tmp97;
_LL91: _tmp99=(void*)_tmp98.r;if(*((int*)_tmp99)== 0){_LL92: _tmp9A=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp99)->f1;if((unsigned int)_tmp9A > 1?*((int*)
_tmp9A)== 5: 0){_LL93: _tmp9B=((struct Cyc_Absyn_String_c_struct*)_tmp9A)->f1;goto
_LL8B;}else{goto _LL8C;}}else{goto _LL8C;}}else{goto _LL8C;}_LL8C: goto _LL8D;_LL89:
_tmp9B=_tmp96;goto _LL8B;_LL8B: desc_types=type_getter(te,(struct _tagged_arr)
_tmp9B,fmt->loc);goto _LL87;_LL8D: return;_LL87:;}if(opt_args != 0){struct Cyc_List_List*
_tmp9C=(struct Cyc_List_List*)opt_args->v;for(0;desc_types != 0? _tmp9C != 0: 0;(
desc_types=desc_types->tl,_tmp9C=_tmp9C->tl)){void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp9C->hd;if(! Cyc_Tcutil_coerce_arg(
te,e,t)){({struct Cyc_Std_String_pa_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
struct Cyc_Std_String_pa_struct _tmp9E;_tmp9E.tag=0;_tmp9E.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t);{void*_tmp9D[2]={& _tmp9E,& _tmp9F};Cyc_Tcutil_terr(e->loc,
_tag_arr("descriptor has type \n%s\n but argument has type \n%s",sizeof(
unsigned char),51),_tag_arr(_tmp9D,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
if(desc_types != 0){({void*_tmpA0[0]={};Cyc_Tcutil_terr(fmt->loc,_tag_arr("too few arguments",
sizeof(unsigned char),18),_tag_arr(_tmpA0,sizeof(void*),0));});}if(_tmp9C != 0){({
void*_tmpA1[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp9C->hd)->loc,
_tag_arr("too many arguments",sizeof(unsigned char),19),_tag_arr(_tmpA1,sizeof(
void*),0));});}}}static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmpA2=p;_LL95: if((
int)_tmpA2 == 0){goto _LL96;}else{goto _LL97;}_LL97: if((int)_tmpA2 == 2){goto _LL98;}
else{goto _LL99;}_LL99: if((int)_tmpA2 == 11){goto _LL9A;}else{goto _LL9B;}_LL9B: if((
int)_tmpA2 == 12){goto _LL9C;}else{goto _LL9D;}_LL9D: if((int)_tmpA2 == 19){goto _LL9E;}
else{goto _LL9F;}_LL9F: goto _LLA0;_LL96: goto _LL98;_LL98: if(! Cyc_Tcutil_is_numeric(
e)){({struct Cyc_Std_String_pa_struct _tmpA4;_tmpA4.tag=0;_tmpA4.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpA3[1]={& _tmpA4};Cyc_Tcutil_terr(
loc,_tag_arr("expecting arithmetic type but found %s",sizeof(unsigned char),39),
_tag_arr(_tmpA3,sizeof(void*),1));}});}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL9A: Cyc_Tcutil_check_contains_assign(e);if(! Cyc_Tcutil_coerce_to_bool(
te,e)){({struct Cyc_Std_String_pa_struct _tmpA6;_tmpA6.tag=0;_tmpA6.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpA5[1]={& _tmpA6};Cyc_Tcutil_terr(
loc,_tag_arr("expecting integral or * type but found %s",sizeof(unsigned char),
42),_tag_arr(_tmpA5,sizeof(void*),1));}});}return Cyc_Absyn_sint_t;_LL9C: if(! Cyc_Tcutil_is_integral(
e)){({struct Cyc_Std_String_pa_struct _tmpA8;_tmpA8.tag=0;_tmpA8.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpA7[1]={& _tmpA8};Cyc_Tcutil_terr(
loc,_tag_arr("expecting integral type but found %s",sizeof(unsigned char),37),
_tag_arr(_tmpA7,sizeof(void*),1));}});}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;_LL9E:{void*_tmpA9=t;struct Cyc_Absyn_PtrInfo _tmpAA;
_LLA2: if((unsigned int)_tmpA9 > 3?*((int*)_tmpA9)== 7: 0){goto _LLA3;}else{goto
_LLA4;}_LLA4: if((unsigned int)_tmpA9 > 3?*((int*)_tmpA9)== 4: 0){_LLA8: _tmpAA=((
struct Cyc_Absyn_PointerType_struct*)_tmpA9)->f1;goto _LLA5;}else{goto _LLA6;}_LLA6:
goto _LLA7;_LLA3: goto _LLA1;_LLA5: goto _LLA1;_LLA7:({struct Cyc_Std_String_pa_struct
_tmpAC;_tmpAC.tag=0;_tmpAC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpAB[1]={& _tmpAC};Cyc_Tcutil_terr(loc,_tag_arr("size requires pointer or array type, not %s",
sizeof(unsigned char),44),_tag_arr(_tmpAB,sizeof(void*),1));}});goto _LLA1;_LLA1:;}
return Cyc_Absyn_uint_t;_LLA0: return({void*_tmpAD[0]={};Cyc_Tcutil_impos(_tag_arr("Non-unary primop",
sizeof(unsigned char),17),_tag_arr(_tmpAD,sizeof(void*),0));});_LL94:;}static
void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){if(! checker(e1)){({
struct Cyc_Std_String_pa_struct _tmpAF;_tmpAF.tag=0;_tmpAF.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{
void*_tmpAE[1]={& _tmpAF};Cyc_Tcutil_terr(e1->loc,_tag_arr("type %s cannot be used here",
sizeof(unsigned char),28),_tag_arr(_tmpAE,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpB0;}));}if(! checker(e2)){({struct Cyc_Std_String_pa_struct _tmpB2;_tmpB2.tag=
0;_tmpB2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{void*_tmpB1[1]={& _tmpB2};Cyc_Tcutil_terr(e2->loc,
_tag_arr("type %s cannot be used here",sizeof(unsigned char),28),_tag_arr(_tmpB1,
sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpB3=
_cycalloc(sizeof(*_tmpB3));_tmpB3->v=Cyc_Tcenv_lookup_type_vars(te);_tmpB3;}));}{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpB4=t1;struct Cyc_Absyn_PtrInfo
_tmpB5;struct Cyc_Absyn_Conref*_tmpB6;struct Cyc_Absyn_Tqual _tmpB7;struct Cyc_Absyn_Conref*
_tmpB8;void*_tmpB9;void*_tmpBA;_LLAA: if((unsigned int)_tmpB4 > 3?*((int*)_tmpB4)
== 4: 0){_LLAE: _tmpB5=((struct Cyc_Absyn_PointerType_struct*)_tmpB4)->f1;_LLB3:
_tmpBA=(void*)_tmpB5.elt_typ;goto _LLB2;_LLB2: _tmpB9=(void*)_tmpB5.rgn_typ;goto
_LLB1;_LLB1: _tmpB8=_tmpB5.nullable;goto _LLB0;_LLB0: _tmpB7=_tmpB5.tq;goto _LLAF;
_LLAF: _tmpB6=_tmpB5.bounds;goto _LLAB;}else{goto _LLAC;}_LLAC: goto _LLAD;_LLAB: if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpBA),(void*)1)){({void*_tmpBB[0]={};
Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(unsigned char),50),_tag_arr(_tmpBB,sizeof(void*),0));});}if(! Cyc_Tcutil_coerce_sint_typ(
te,e2)){({struct Cyc_Std_String_pa_struct _tmpBD;_tmpBD.tag=0;_tmpBD.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmpBC[1]={& _tmpBD};Cyc_Tcutil_terr(
e2->loc,_tag_arr("expecting int but found %s",sizeof(unsigned char),27),_tag_arr(
_tmpBC,sizeof(void*),1));}});}_tmpB6=Cyc_Absyn_compress_conref(_tmpB6);{void*
_tmpBE=(void*)_tmpB6->v;void*_tmpBF;void*_tmpC0;struct Cyc_Absyn_Exp*_tmpC1;_LLB5:
if((unsigned int)_tmpBE > 1?*((int*)_tmpBE)== 0: 0){_LLBB: _tmpBF=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmpBE)->f1;if((int)_tmpBF == 0){goto _LLB6;}else{goto
_LLB7;}}else{goto _LLB7;}_LLB7: if((unsigned int)_tmpBE > 1?*((int*)_tmpBE)== 0: 0){
_LLBC: _tmpC0=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpBE)->f1;if((
unsigned int)_tmpC0 > 1?*((int*)_tmpC0)== 0: 0){_LLBD: _tmpC1=((struct Cyc_Absyn_Upper_b_struct*)
_tmpC0)->f1;goto _LLB8;}else{goto _LLB9;}}else{goto _LLB9;}_LLB9: goto _LLBA;_LLB6:
return t1;_LLB8: {struct Cyc_Absyn_PointerType_struct*_tmpC2=({struct Cyc_Absyn_PointerType_struct*
_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_PointerType_struct
_tmpC4;_tmpC4.tag=4;_tmpC4.f1=({struct Cyc_Absyn_PtrInfo _tmpC5;_tmpC5.elt_typ=(
void*)_tmpBA;_tmpC5.rgn_typ=(void*)_tmpB9;_tmpC5.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmpC5.tq=_tmpB7;_tmpC5.bounds=Cyc_Absyn_new_conref((
void*)0);_tmpC5;});_tmpC4;});_tmpC3;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)
_tmpC2);return(void*)_tmpC2;}_LLBA:(void*)(_tmpB6->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmpC7;_tmpC7.tag=0;_tmpC7.f1=(void*)((void*)0);_tmpC7;});_tmpC6;})));return t1;
_LLB4:;}_LLAD: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLA9:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)
0;void*t2_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(
Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(! Cyc_Tcutil_unify(t1,t2)){({struct Cyc_Std_String_pa_struct
_tmpCA;_tmpCA.tag=0;_tmpCA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct Cyc_Std_String_pa_struct
_tmpC9;_tmpC9.tag=0;_tmpC9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{void*_tmpC8[2]={& _tmpC9,& _tmpCA};
Cyc_Tcutil_terr(e1->loc,_tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof(unsigned char),59),_tag_arr(_tmpC8,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_t;}else{if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),(
void*)1)){({void*_tmpCB[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(unsigned char),50),_tag_arr(_tmpCB,sizeof(void*),0));});}if(! Cyc_Tcutil_coerce_sint_typ(
te,e2)){({struct Cyc_Std_String_pa_struct _tmpCE;_tmpCE.tag=0;_tmpCE.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmpCD;
_tmpCD.tag=0;_tmpCD.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmpCC[2]={& _tmpCD,& _tmpCE};Cyc_Tcutil_terr(e2->loc,_tag_arr("expecting either %s or int but found %s",
sizeof(unsigned char),40),_tag_arr(_tmpCC,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t1;}}else{return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num? e2_is_num: 0){return
Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_unify(t1,t2)? Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({struct Cyc_Core_Opt*_tmpCF=_cycalloc(sizeof(*
_tmpCF));_tmpCF->v=Cyc_Tcenv_lookup_type_vars(te);_tmpCF;}))): 0){return Cyc_Absyn_sint_t;}
else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,
e2,t1);return Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2);return Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e1)? 1: Cyc_Tcutil_zero_to_null(te,t1,e2)){return Cyc_Absyn_sint_t;}else{{
struct _tuple0 _tmpD1=({struct _tuple0 _tmpD0;_tmpD0.f1=Cyc_Tcutil_compress(t1);
_tmpD0.f2=Cyc_Tcutil_compress(t2);_tmpD0;});void*_tmpD2;struct Cyc_Absyn_PtrInfo
_tmpD3;void*_tmpD4;void*_tmpD5;struct Cyc_Absyn_PtrInfo _tmpD6;void*_tmpD7;_LLBF:
_LLC6: _tmpD5=_tmpD1.f1;if((unsigned int)_tmpD5 > 3?*((int*)_tmpD5)== 4: 0){_LLC7:
_tmpD6=((struct Cyc_Absyn_PointerType_struct*)_tmpD5)->f1;_LLC8: _tmpD7=(void*)
_tmpD6.elt_typ;goto _LLC3;}else{goto _LLC1;}_LLC3: _tmpD2=_tmpD1.f2;if((
unsigned int)_tmpD2 > 3?*((int*)_tmpD2)== 4: 0){_LLC4: _tmpD3=((struct Cyc_Absyn_PointerType_struct*)
_tmpD2)->f1;_LLC5: _tmpD4=(void*)_tmpD3.elt_typ;goto _LLC0;}else{goto _LLC1;}_LLC1:
goto _LLC2;_LLC0: if(Cyc_Tcutil_unify(_tmpD7,_tmpD4)){return Cyc_Absyn_sint_t;}goto
_LLBE;_LLC2: goto _LLBE;_LLBE:;}FAIL:({struct Cyc_Std_String_pa_struct _tmpDA;_tmpDA.tag=
0;_tmpDA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct
_tmpD9;_tmpD9.tag=0;_tmpD9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{
void*_tmpD8[2]={& _tmpD9,& _tmpDA};Cyc_Tcutil_terr(loc,_tag_arr("comparison not allowed between %s and %s",
sizeof(unsigned char),41),_tag_arr(_tmpD8,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpDB=_cycalloc(sizeof(*_tmpDB));
_tmpDB->v=Cyc_Tcenv_lookup_type_vars(te);_tmpDB;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpDC=p;_LLCA: if((int)_tmpDC == 0){
goto _LLCB;}else{goto _LLCC;}_LLCC: if((int)_tmpDC == 2){goto _LLCD;}else{goto _LLCE;}
_LLCE: if((int)_tmpDC == 1){goto _LLCF;}else{goto _LLD0;}_LLD0: if((int)_tmpDC == 3){
goto _LLD1;}else{goto _LLD2;}_LLD2: if((int)_tmpDC == 4){goto _LLD3;}else{goto _LLD4;}
_LLD4: if((int)_tmpDC == 13){goto _LLD5;}else{goto _LLD6;}_LLD6: if((int)_tmpDC == 14){
goto _LLD7;}else{goto _LLD8;}_LLD8: if((int)_tmpDC == 15){goto _LLD9;}else{goto _LLDA;}
_LLDA: if((int)_tmpDC == 16){goto _LLDB;}else{goto _LLDC;}_LLDC: if((int)_tmpDC == 17){
goto _LLDD;}else{goto _LLDE;}_LLDE: if((int)_tmpDC == 18){goto _LLDF;}else{goto _LLE0;}
_LLE0: if((int)_tmpDC == 5){goto _LLE1;}else{goto _LLE2;}_LLE2: if((int)_tmpDC == 6){
goto _LLE3;}else{goto _LLE4;}_LLE4: if((int)_tmpDC == 7){goto _LLE5;}else{goto _LLE6;}
_LLE6: if((int)_tmpDC == 8){goto _LLE7;}else{goto _LLE8;}_LLE8: if((int)_tmpDC == 9){
goto _LLE9;}else{goto _LLEA;}_LLEA: if((int)_tmpDC == 10){goto _LLEB;}else{goto _LLEC;}
_LLEC: goto _LLED;_LLCB: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLCD: return Cyc_Tcexp_tcMinus(
te,e1,e2);_LLCF: goto _LLD1;_LLD1: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LLD3: goto _LLD5;_LLD5: goto _LLD7;_LLD7: goto _LLD9;_LLD9: goto _LLDB;_LLDB: goto _LLDD;
_LLDD: goto _LLDF;_LLDF: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);
_LLE1: goto _LLE3;_LLE3: goto _LLE5;_LLE5: goto _LLE7;_LLE7: goto _LLE9;_LLE9: goto _LLEB;
_LLEB: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLED:(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD[0]=({struct Cyc_Core_Failure_struct
_tmpDE;_tmpDE.tag=Cyc_Core_Failure;_tmpDE.f1=_tag_arr("bad binary primop",
sizeof(unsigned char),18);_tmpDE;});_tmpDD;}));_LLC9:;}static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)2?((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es)== 1: 0){return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);}Cyc_Tcexp_tcExpList(te,es);{void*t;switch(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLEE: return({void*_tmpDF[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("primitive operator has 0 arguments",sizeof(unsigned char),35),
_tag_arr(_tmpDF,sizeof(void*),0));});case 1: _LLEF: t=Cyc_Tcexp_tcUnPrimop(te,loc,
topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;
case 2: _LLF0: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLF1: return({void*_tmpE0[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("primitive operator has > 2 arguments",
sizeof(unsigned char),37),_tag_arr(_tmpE0,sizeof(void*),0));});}return t;}}struct
_tuple8{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmpE1=(void*)e->r;void*
_tmpE2;struct Cyc_Absyn_Vardecl*_tmpE3;void*_tmpE4;struct Cyc_Absyn_Vardecl*_tmpE5;
void*_tmpE6;struct Cyc_Absyn_Vardecl*_tmpE7;void*_tmpE8;struct Cyc_Absyn_Vardecl*
_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct _tagged_arr*
_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct _tagged_arr*_tmpEE;struct Cyc_Absyn_Exp*
_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*
_tmpF2;_LLF4: if(*((int*)_tmpE1)== 1){_LL10A: _tmpE2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpE1)->f2;if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 2: 0){_LL10B: _tmpE3=((
struct Cyc_Absyn_Param_b_struct*)_tmpE2)->f1;goto _LLF5;}else{goto _LLF6;}}else{
goto _LLF6;}_LLF6: if(*((int*)_tmpE1)== 1){_LL10C: _tmpE4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpE1)->f2;if((unsigned int)_tmpE4 > 1?*((int*)_tmpE4)== 3: 0){_LL10D: _tmpE5=((
struct Cyc_Absyn_Local_b_struct*)_tmpE4)->f1;goto _LLF7;}else{goto _LLF8;}}else{
goto _LLF8;}_LLF8: if(*((int*)_tmpE1)== 1){_LL10E: _tmpE6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpE1)->f2;if((unsigned int)_tmpE6 > 1?*((int*)_tmpE6)== 4: 0){_LL10F: _tmpE7=((
struct Cyc_Absyn_Pat_b_struct*)_tmpE6)->f1;goto _LLF9;}else{goto _LLFA;}}else{goto
_LLFA;}_LLFA: if(*((int*)_tmpE1)== 1){_LL110: _tmpE8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpE1)->f2;if((unsigned int)_tmpE8 > 1?*((int*)_tmpE8)== 0: 0){_LL111: _tmpE9=((
struct Cyc_Absyn_Global_b_struct*)_tmpE8)->f1;goto _LLFB;}else{goto _LLFC;}}else{
goto _LLFC;}_LLFC: if(*((int*)_tmpE1)== 23){_LL113: _tmpEB=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpE1)->f1;goto _LL112;_LL112: _tmpEA=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpE1)->f2;goto _LLFD;}else{goto _LLFE;}_LLFE: if(*((int*)_tmpE1)== 21){_LL115:
_tmpED=((struct Cyc_Absyn_StructMember_e_struct*)_tmpE1)->f1;goto _LL114;_LL114:
_tmpEC=((struct Cyc_Absyn_StructMember_e_struct*)_tmpE1)->f2;goto _LLFF;}else{goto
_LL100;}_LL100: if(*((int*)_tmpE1)== 22){_LL117: _tmpEF=((struct Cyc_Absyn_StructArrow_e_struct*)
_tmpE1)->f1;goto _LL116;_LL116: _tmpEE=((struct Cyc_Absyn_StructArrow_e_struct*)
_tmpE1)->f2;goto _LL101;}else{goto _LL102;}_LL102: if(*((int*)_tmpE1)== 20){_LL118:
_tmpF0=((struct Cyc_Absyn_Deref_e_struct*)_tmpE1)->f1;goto _LL103;}else{goto _LL104;}
_LL104: if(*((int*)_tmpE1)== 11){_LL119: _tmpF1=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpE1)->f1;goto _LL105;}else{goto _LL106;}_LL106: if(*((int*)_tmpE1)== 12){_LL11A:
_tmpF2=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpE1)->f1;goto _LL107;}else{goto
_LL108;}_LL108: goto _LL109;_LLF5: _tmpE5=_tmpE3;goto _LLF7;_LLF7: _tmpE7=_tmpE5;goto
_LLF9;_LLF9: _tmpE9=_tmpE7;goto _LLFB;_LLFB: if(!(_tmpE9->tq).q_const){return;}goto
_LLF3;_LLFD:{void*_tmpF3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEB->topt))->v);struct Cyc_Absyn_PtrInfo _tmpF4;struct Cyc_Absyn_Tqual
_tmpF5;struct Cyc_Absyn_Tqual _tmpF6;struct Cyc_List_List*_tmpF7;_LL11C: if((
unsigned int)_tmpF3 > 3?*((int*)_tmpF3)== 4: 0){_LL124: _tmpF4=((struct Cyc_Absyn_PointerType_struct*)
_tmpF3)->f1;_LL125: _tmpF5=_tmpF4.tq;goto _LL11D;}else{goto _LL11E;}_LL11E: if((
unsigned int)_tmpF3 > 3?*((int*)_tmpF3)== 7: 0){_LL126: _tmpF6=((struct Cyc_Absyn_ArrayType_struct*)
_tmpF3)->f2;goto _LL11F;}else{goto _LL120;}_LL120: if((unsigned int)_tmpF3 > 3?*((
int*)_tmpF3)== 9: 0){_LL127: _tmpF7=((struct Cyc_Absyn_TupleType_struct*)_tmpF3)->f1;
goto _LL121;}else{goto _LL122;}_LL122: goto _LL123;_LL11D: _tmpF6=_tmpF5;goto _LL11F;
_LL11F: if(! _tmpF6.q_const){return;}goto _LL11B;_LL121: {unsigned int i=Cyc_Evexp_eval_const_uint_exp(
_tmpEA);{struct _handler_cons _tmpF8;_push_handler(& _tmpF8);{int _tmpFA=0;if(setjmp(
_tmpF8.handler)){_tmpFA=1;}if(! _tmpFA){{struct _tuple8 _tmpFC;struct Cyc_Absyn_Tqual
_tmpFD;struct _tuple8*_tmpFB=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmpF7,(int)i);_tmpFC=*_tmpFB;_LL129: _tmpFD=_tmpFC.f1;goto _LL128;_LL128: if(!
_tmpFD.q_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmpF9=(
void*)_exn_thrown;void*_tmpFF=_tmpF9;_LL12B: if(_tmpFF == Cyc_List_Nth){goto _LL12C;}
else{goto _LL12D;}_LL12D: goto _LL12E;_LL12C: return;_LL12E:(void)_throw(_tmpFF);
_LL12A:;}}}goto _LL11B;}_LL123: goto _LL11B;_LL11B:;}goto _LLF3;_LLFF:{void*_tmp100=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpED->topt))->v);
struct Cyc_Absyn_Structdecl**_tmp101;struct Cyc_List_List*_tmp102;struct Cyc_List_List*
_tmp103;struct Cyc_Absyn_Uniondecl**_tmp104;_LL130: if((unsigned int)_tmp100 > 3?*((
int*)_tmp100)== 10: 0){_LL13A: _tmp101=((struct Cyc_Absyn_StructType_struct*)
_tmp100)->f3;goto _LL131;}else{goto _LL132;}_LL132: if((unsigned int)_tmp100 > 3?*((
int*)_tmp100)== 15: 0){_LL13B: _tmp102=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp100)->f1;goto _LL133;}else{goto _LL134;}_LL134: if((unsigned int)_tmp100 > 3?*((
int*)_tmp100)== 14: 0){_LL13C: _tmp103=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp100)->f1;goto _LL135;}else{goto _LL136;}_LL136: if((unsigned int)_tmp100 > 3?*((
int*)_tmp100)== 11: 0){_LL13D: _tmp104=((struct Cyc_Absyn_UnionType_struct*)_tmp100)->f3;
goto _LL137;}else{goto _LL138;}_LL138: goto _LL139;_LL131: {struct Cyc_Absyn_Structfield*
sf=_tmp101 == 0? 0: Cyc_Absyn_lookup_struct_field((struct Cyc_Absyn_Structdecl*)*
_tmp101,_tmpEC);if(sf == 0? 1: !(sf->tq).q_const){return;}goto _LL12F;}_LL133:
_tmp103=_tmp102;goto _LL135;_LL135: {struct Cyc_Absyn_Structfield*sf=Cyc_Absyn_lookup_field(
_tmp103,_tmpEC);if(sf == 0? 1: !(sf->tq).q_const){return;}goto _LL12F;}_LL137: {
struct Cyc_Absyn_Structfield*sf=_tmp104 == 0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)*_tmp104,_tmpEC);if(sf == 0? 1: !(sf->tq).q_const){
return;}goto _LL12F;}_LL139: goto _LL12F;_LL12F:;}goto _LLF3;_LL101:{void*_tmp105=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpEF->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp106;void*_tmp107;_LL13F: if((unsigned int)_tmp105 > 3?*((
int*)_tmp105)== 4: 0){_LL143: _tmp106=((struct Cyc_Absyn_PointerType_struct*)
_tmp105)->f1;_LL144: _tmp107=(void*)_tmp106.elt_typ;goto _LL140;}else{goto _LL141;}
_LL141: goto _LL142;_LL140:{void*_tmp108=Cyc_Tcutil_compress(_tmp107);struct Cyc_Absyn_Structdecl**
_tmp109;struct Cyc_Absyn_Uniondecl**_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*
_tmp10C;_LL146: if((unsigned int)_tmp108 > 3?*((int*)_tmp108)== 10: 0){_LL150:
_tmp109=((struct Cyc_Absyn_StructType_struct*)_tmp108)->f3;goto _LL147;}else{goto
_LL148;}_LL148: if((unsigned int)_tmp108 > 3?*((int*)_tmp108)== 11: 0){_LL151:
_tmp10A=((struct Cyc_Absyn_UnionType_struct*)_tmp108)->f3;goto _LL149;}else{goto
_LL14A;}_LL14A: if((unsigned int)_tmp108 > 3?*((int*)_tmp108)== 14: 0){_LL152:
_tmp10B=((struct Cyc_Absyn_AnonStructType_struct*)_tmp108)->f1;goto _LL14B;}else{
goto _LL14C;}_LL14C: if((unsigned int)_tmp108 > 3?*((int*)_tmp108)== 15: 0){_LL153:
_tmp10C=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp108)->f1;goto _LL14D;}else{
goto _LL14E;}_LL14E: goto _LL14F;_LL147: {struct Cyc_Absyn_Structfield*sf=_tmp109 == 
0? 0: Cyc_Absyn_lookup_struct_field((struct Cyc_Absyn_Structdecl*)*_tmp109,_tmpEE);
if(sf == 0? 1: !(sf->tq).q_const){return;}goto _LL145;}_LL149: {struct Cyc_Absyn_Structfield*
sf=_tmp10A == 0? 0: Cyc_Absyn_lookup_union_field((struct Cyc_Absyn_Uniondecl*)*
_tmp10A,_tmpEE);if(sf == 0? 1: !(sf->tq).q_const){return;}goto _LL145;}_LL14B:
_tmp10C=_tmp10B;goto _LL14D;_LL14D: {struct Cyc_Absyn_Structfield*sf=Cyc_Absyn_lookup_field(
_tmp10C,_tmpEE);if(sf == 0? 1: !(sf->tq).q_const){return;}goto _LL145;}_LL14F: goto
_LL145;_LL145:;}goto _LL13E;_LL142: goto _LL13E;_LL13E:;}goto _LLF3;_LL103:{void*
_tmp10D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF0->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp10E;struct Cyc_Absyn_Tqual _tmp10F;struct Cyc_Absyn_Tqual
_tmp110;_LL155: if((unsigned int)_tmp10D > 3?*((int*)_tmp10D)== 4: 0){_LL15B:
_tmp10E=((struct Cyc_Absyn_PointerType_struct*)_tmp10D)->f1;_LL15C: _tmp10F=
_tmp10E.tq;goto _LL156;}else{goto _LL157;}_LL157: if((unsigned int)_tmp10D > 3?*((
int*)_tmp10D)== 7: 0){_LL15D: _tmp110=((struct Cyc_Absyn_ArrayType_struct*)_tmp10D)->f2;
goto _LL158;}else{goto _LL159;}_LL159: goto _LL15A;_LL156: _tmp110=_tmp10F;goto _LL158;
_LL158: if(! _tmp110.q_const){return;}goto _LL154;_LL15A: goto _LL154;_LL154:;}goto
_LLF3;_LL105: _tmpF2=_tmpF1;goto _LL107;_LL107: Cyc_Tcexp_check_writable(te,_tmpF2);
return;_LL109: goto _LLF3;_LLF3:;}({struct Cyc_Std_String_pa_struct _tmp112;_tmp112.tag=
0;_tmp112.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp111[1]={&
_tmp112};Cyc_Tcutil_terr(e->loc,_tag_arr("attempt to write a const location: %s",
sizeof(unsigned char),38),_tag_arr(_tmp111,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(! Cyc_Absyn_is_lvalue(e)){return({
void*_tmp113[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("increment/decrement of non-lvalue",
sizeof(unsigned char),34),_tag_arr(_tmp113,sizeof(void*),0));});}Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(! Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt)){if(! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(telt),(void*)1)){({void*_tmp114[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("can't perform arithmetic on abstract pointer type",sizeof(
unsigned char),50),_tag_arr(_tmp114,sizeof(void*),0));});}}else{({struct Cyc_Std_String_pa_struct
_tmp116;_tmp116.tag=0;_tmp116.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp115[1]={& _tmp116};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting arithmetic or ? type but found %s",
sizeof(unsigned char),44),_tag_arr(_tmp115,sizeof(void*),1));}});}}return t;}}
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){Cyc_Tcexp_tcTest(te,e1,_tag_arr("conditional expression",sizeof(
unsigned char),23));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{
void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*
_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp11E;}));struct Cyc_List_List*_tmp117=({struct Cyc_List_List*_tmp11D=_cycalloc(
sizeof(*_tmp11D));_tmp11D->hd=e3;_tmp11D->tl=0;_tmp11D;});struct Cyc_List_List*
_tmp118=({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->hd=e2;
_tmp11C->tl=_tmp117;_tmp11C;});if(! Cyc_Tcutil_coerce_list(te,t,_tmp118)){({
struct Cyc_Std_String_pa_struct _tmp11B;_tmp11B.tag=0;_tmp11B.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v);{
struct Cyc_Std_String_pa_struct _tmp11A;_tmp11A.tag=0;_tmp11A.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{
void*_tmp119[2]={& _tmp11A,& _tmp11B};Cyc_Tcutil_terr(loc,_tag_arr("conditional clause types do not match: %s != %s",
sizeof(unsigned char),48),_tag_arr(_tmp119,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t;}}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExpNoPromote(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp11F=Cyc_Tcutil_compress(t1);_LL15F: if((unsigned int)_tmp11F > 3?*((int*)
_tmp11F)== 7: 0){goto _LL160;}else{goto _LL161;}_LL161: goto _LL162;_LL160:({void*
_tmp120[0]={};Cyc_Tcutil_terr(loc,_tag_arr("cannot assign to an array",sizeof(
unsigned char),26),_tag_arr(_tmp120,sizeof(void*),0));});goto _LL15E;_LL162: goto
_LL15E;_LL15E:;}if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),(void*)1)){({
void*_tmp121[0]={};Cyc_Tcutil_terr(loc,_tag_arr("type is abstract (can't determine size).",
sizeof(unsigned char),41),_tag_arr(_tmp121,sizeof(void*),0));});}if(! Cyc_Absyn_is_lvalue(
e1)){return({void*_tmp122[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("assignment to non-lvalue",
sizeof(unsigned char),25),_tag_arr(_tmp122,sizeof(void*),0));});}Cyc_Tcexp_check_writable(
te,e1);if(po == 0){if(! Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp123=({struct
Cyc_Std_String_pa_struct _tmp126;_tmp126.tag=0;_tmp126.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t2);{struct Cyc_Std_String_pa_struct _tmp125;_tmp125.tag=0;_tmp125.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp124[2]={& _tmp125,& _tmp126};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type mismatch: %s != %s",sizeof(unsigned char),24),_tag_arr(
_tmp124,sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp123;}}else{void*_tmp127=(void*)po->v;void*_tmp128=Cyc_Tcexp_tcBinPrimop(
te,loc,0,_tmp127,e1,e2);if(!(Cyc_Tcutil_unify(_tmp128,t1)? 1:(Cyc_Tcutil_coerceable(
_tmp128)? Cyc_Tcutil_coerceable(t1): 0))){void*_tmp129=({struct Cyc_Std_String_pa_struct
_tmp12C;_tmp12C.tag=0;_tmp12C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp12B;_tmp12B.tag=0;_tmp12B.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp12A[2]={& _tmp12B,& _tmp12C};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof(unsigned char),82),_tag_arr(_tmp12A,sizeof(void*),2));}}});Cyc_Tcutil_unify(
_tmp128,t1);Cyc_Tcutil_explain_failure();return _tmp129;}return _tmp128;}return t1;}}
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(
e2->topt))->v;}static struct Cyc_Absyn_DoubleType_struct Cyc_Tcexp_dbl={6,0};static
void*Cyc_Tcexp_dbl_t=(void*)& Cyc_Tcexp_dbl;static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){struct Cyc_List_List*fields;void*
t1=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp12E=Cyc_Tcutil_compress(
t1);void*_tmp12F;void*_tmp130;_LL164: if((int)_tmp12E == 1){goto _LL165;}else{goto
_LL166;}_LL166: if((unsigned int)_tmp12E > 3?*((int*)_tmp12E)== 5: 0){_LL16C:
_tmp12F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp12E)->f2;if((int)_tmp12F == 
0){goto _LL167;}else{goto _LL168;}}else{goto _LL168;}_LL168: if((unsigned int)
_tmp12E > 3?*((int*)_tmp12E)== 5: 0){_LL16D: _tmp130=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp12E)->f2;if((int)_tmp130 == 1){goto _LL169;}else{goto _LL16A;}}else{goto _LL16A;}
_LL16A: goto _LL16B;_LL165: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Tcexp_dbl_t);t1=Cyc_Tcexp_dbl_t;
goto _LL163;_LL167: goto _LL169;_LL169: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_t);
t1=Cyc_Absyn_sint_t;goto _LL163;_LL16B: goto _LL163;_LL163:;}for(fields=fs;fields != 
0;fields=fields->tl){void*_tmp132;struct Cyc_Position_Segment*_tmp133;struct Cyc_List_List*
_tmp134;struct Cyc_List_List*_tmp135;struct _tuple1*_tmp136;struct Cyc_Absyn_Tunionfield
_tmp131=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_LL173: _tmp136=_tmp131.name;
goto _LL172;_LL172: _tmp135=_tmp131.tvs;goto _LL171;_LL171: _tmp134=_tmp131.typs;
goto _LL170;_LL170: _tmp133=_tmp131.loc;goto _LL16F;_LL16F: _tmp132=(void*)_tmp131.sc;
goto _LL16E;_LL16E: if((_tmp134 == 0? 1: _tmp134->tl != 0)? 1: _tmp135 != 0){continue;}{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple8*)_tmp134->hd)).f2);if(
Cyc_Tcutil_unify(t1,t2)){return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}}for(fields=fs;fields != 0;fields=fields->tl){void*_tmp138;struct
Cyc_Position_Segment*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_List_List*
_tmp13B;struct _tuple1*_tmp13C;struct Cyc_Absyn_Tunionfield _tmp137=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_LL179: _tmp13C=_tmp137.name;goto _LL178;_LL178: _tmp13B=_tmp137.tvs;
goto _LL177;_LL177: _tmp13A=_tmp137.typs;goto _LL176;_LL176: _tmp139=_tmp137.loc;
goto _LL175;_LL175: _tmp138=(void*)_tmp137.sc;goto _LL174;_LL174: if((_tmp13A == 0? 1:
_tmp13A->tl != 0)? 1: _tmp13B != 0){continue;}{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((
struct _tuple8*)_tmp13A->hd)).f2);if(Cyc_Tcutil_coerce_arg(te,e,t2)){return(
struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}}({
struct Cyc_Std_String_pa_struct _tmp13F;_tmp13F.tag=0;_tmp13F.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{struct Cyc_Std_String_pa_struct _tmp13E;
_tmp13E.tag=0;_tmp13E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(tu);{void*
_tmp13D[2]={& _tmp13E,& _tmp13F};Cyc_Tcutil_terr(e->loc,_tag_arr("can't find a field in %s to inject a value of type %s",
sizeof(unsigned char),54),_tag_arr(_tmp13D,sizeof(void*),2));}}});return 0;}
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info){struct Cyc_List_List*_tmp140=args;te=Cyc_Tcenv_new_block(loc,te);
Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp141=t;struct Cyc_Absyn_PtrInfo _tmp142;struct
Cyc_Absyn_Conref*_tmp143;struct Cyc_Absyn_Tqual _tmp144;struct Cyc_Absyn_Conref*
_tmp145;void*_tmp146;void*_tmp147;_LL17B: if((unsigned int)_tmp141 > 3?*((int*)
_tmp141)== 4: 0){_LL17F: _tmp142=((struct Cyc_Absyn_PointerType_struct*)_tmp141)->f1;
_LL184: _tmp147=(void*)_tmp142.elt_typ;goto _LL183;_LL183: _tmp146=(void*)_tmp142.rgn_typ;
goto _LL182;_LL182: _tmp145=_tmp142.nullable;goto _LL181;_LL181: _tmp144=_tmp142.tq;
goto _LL180;_LL180: _tmp143=_tmp142.bounds;goto _LL17C;}else{goto _LL17D;}_LL17D:
goto _LL17E;_LL17C: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp146);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp143);{void*_tmp148=Cyc_Tcutil_compress(_tmp147);struct Cyc_Absyn_FnInfo
_tmp149;struct Cyc_List_List*_tmp14A;struct Cyc_List_List*_tmp14B;struct Cyc_Absyn_VarargInfo*
_tmp14C;int _tmp14D;struct Cyc_List_List*_tmp14E;void*_tmp14F;struct Cyc_Core_Opt*
_tmp150;struct Cyc_List_List*_tmp151;_LL186: if((unsigned int)_tmp148 > 3?*((int*)
_tmp148)== 8: 0){_LL18A: _tmp149=((struct Cyc_Absyn_FnType_struct*)_tmp148)->f1;
_LL192: _tmp151=_tmp149.tvars;goto _LL191;_LL191: _tmp150=_tmp149.effect;goto _LL190;
_LL190: _tmp14F=(void*)_tmp149.ret_typ;goto _LL18F;_LL18F: _tmp14E=_tmp149.args;
goto _LL18E;_LL18E: _tmp14D=_tmp149.c_varargs;goto _LL18D;_LL18D: _tmp14C=_tmp149.cyc_varargs;
goto _LL18C;_LL18C: _tmp14B=_tmp149.rgn_po;goto _LL18B;_LL18B: _tmp14A=_tmp149.attributes;
goto _LL187;}else{goto _LL188;}_LL188: goto _LL189;_LL187: if(topt != 0){Cyc_Tcutil_unify(
_tmp14F,*topt);}while(_tmp140 != 0? _tmp14E != 0: 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp140->hd;void*t2=(*((struct _tuple2*)_tmp14E->hd)).f3;Cyc_Tcexp_tcExp(
te,(void**)& t2,e1);if(! Cyc_Tcutil_coerce_arg(te,e1,t2)){({struct Cyc_Std_String_pa_struct
_tmp154;_tmp154.tag=0;_tmp154.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp153;_tmp153.tag=0;_tmp153.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{
void*_tmp152[2]={& _tmp153,& _tmp154};Cyc_Tcutil_terr(e1->loc,_tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof(unsigned char),54),_tag_arr(_tmp152,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
_tmp140=_tmp140->tl;_tmp14E=_tmp14E->tl;}if(_tmp14E != 0){({void*_tmp155[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments for function",sizeof(
unsigned char),31),_tag_arr(_tmp155,sizeof(void*),0));});}else{if((_tmp140 != 0? 1:
_tmp14D)? 1: _tmp14C != 0){if(_tmp14D){for(0;_tmp140 != 0;_tmp140=_tmp140->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmp140->hd);}}else{if(_tmp14C == 0){({void*_tmp156[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too many arguments for function",sizeof(
unsigned char),32),_tag_arr(_tmp156,sizeof(void*),0));});}else{int _tmp158;void*
_tmp159;struct Cyc_Absyn_VarargInfo _tmp157=*_tmp14C;_LL195: _tmp159=(void*)_tmp157.type;
goto _LL194;_LL194: _tmp158=_tmp157.inject;goto _LL193;_LL193: {struct Cyc_Absyn_VarargCallInfo*
_tmp15A=({struct Cyc_Absyn_VarargCallInfo*_tmp16E=_cycalloc(sizeof(*_tmp16E));
_tmp16E->num_varargs=0;_tmp16E->injectors=0;_tmp16E->vai=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp14C);_tmp16E;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp15A;if(! _tmp158){for(0;_tmp140 != 0;_tmp140=_tmp140->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp140->hd;_tmp15A->num_varargs ++;Cyc_Tcexp_tcExp(te,(
void**)& _tmp159,e1);if(! Cyc_Tcutil_coerce_arg(te,e1,_tmp159)){({struct Cyc_Std_String_pa_struct
_tmp15D;_tmp15D.tag=0;_tmp15D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp159);{void*_tmp15B[2]={& _tmp15C,& _tmp15D};Cyc_Tcutil_terr(loc,_tag_arr("vararg requires type %s but argument has type %s",
sizeof(unsigned char),49),_tag_arr(_tmp15B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}}
else{void*_tmp15E=Cyc_Tcutil_compress(_tmp159);struct Cyc_Absyn_TunionInfo _tmp15F;
void*_tmp160;struct Cyc_List_List*_tmp161;void*_tmp162;struct Cyc_Absyn_Tuniondecl**
_tmp163;struct Cyc_Absyn_Tuniondecl*_tmp164;_LL197: if((unsigned int)_tmp15E > 3?*((
int*)_tmp15E)== 2: 0){_LL19B: _tmp15F=((struct Cyc_Absyn_TunionType_struct*)_tmp15E)->f1;
_LL19E: _tmp162=(void*)_tmp15F.tunion_info;if(*((int*)_tmp162)== 1){_LL19F:
_tmp163=((struct Cyc_Absyn_KnownTunion_struct*)_tmp162)->f1;_tmp164=*_tmp163;goto
_LL19D;}else{goto _LL199;}_LL19D: _tmp161=_tmp15F.targs;goto _LL19C;_LL19C: _tmp160=(
void*)_tmp15F.rgn;goto _LL198;}else{goto _LL199;}_LL199: goto _LL19A;_LL198: {struct
Cyc_Absyn_Tuniondecl*_tmp165=*Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp164->name);
struct Cyc_List_List*fields=0;if(_tmp165->fields == 0){({struct Cyc_Std_String_pa_struct
_tmp167;_tmp167.tag=0;_tmp167.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp159);{void*_tmp166[1]={& _tmp167};Cyc_Tcutil_terr(loc,_tag_arr("can't inject into %s",
sizeof(unsigned char),21),_tag_arr(_tmp166,sizeof(void*),1));}});}else{fields=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp165->fields))->v;}if(
! Cyc_Tcutil_unify(_tmp160,Cyc_Tcenv_curr_rgn(te))){({void*_tmp168[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bad region for injected varargs",sizeof(unsigned char),32),
_tag_arr(_tmp168,sizeof(void*),0));});}{struct _RegionHandle _tmp169=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp169;_push_region(rgn);{struct Cyc_List_List*_tmp16A=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp165->tvs,_tmp161);for(0;
_tmp140 != 0;_tmp140=_tmp140->tl){_tmp15A->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp140->hd;Cyc_Tcexp_tcExp(te,0,e1);{struct Cyc_Absyn_Tunionfield*
_tmp16B=Cyc_Tcexp_tcInjection(te,e1,_tmp159,rgn,_tmp16A,fields);if(_tmp16B != 0){
_tmp15A->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp15A->injectors,({struct Cyc_List_List*_tmp16C=_cycalloc(
sizeof(*_tmp16C));_tmp16C->hd=(struct Cyc_Absyn_Tunionfield*)_check_null(_tmp16B);
_tmp16C->tl=0;_tmp16C;}));}}}}};_pop_region(rgn);}goto _LL196;}_LL19A:({void*
_tmp16D[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad inject vararg type",sizeof(
unsigned char),23),_tag_arr(_tmp16D,sizeof(void*),0));});goto _LL196;_LL196:;}}}}}}{
struct Cyc_List_List*a=_tmp14A;for(0;a != 0;a=a->tl){void*_tmp16F=(void*)a->hd;int
_tmp170;int _tmp171;void*_tmp172;_LL1A1: if((unsigned int)_tmp16F > 16?*((int*)
_tmp16F)== 3: 0){_LL1A7: _tmp172=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp16F)->f1;goto _LL1A6;_LL1A6: _tmp171=((struct Cyc_Absyn_Format_att_struct*)
_tmp16F)->f2;goto _LL1A5;_LL1A5: _tmp170=((struct Cyc_Absyn_Format_att_struct*)
_tmp16F)->f3;goto _LL1A2;}else{goto _LL1A3;}_LL1A3: goto _LL1A4;_LL1A2:{struct
_handler_cons _tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(_tmp173.handler)){
_tmp175=1;}if(! _tmp175){{struct Cyc_Absyn_Exp*_tmp176=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp171 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp170 == 0){fmt_args=0;}else{fmt_args=({struct Cyc_Core_Opt*_tmp177=
_cycalloc(sizeof(*_tmp177));_tmp177->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,int i))Cyc_List_nth_tail)(args,_tmp170 - 1);_tmp177;});}{void*_tmp178=_tmp172;
_LL1A9: if((int)_tmp178 == 0){goto _LL1AA;}else{goto _LL1AB;}_LL1AB: if((int)_tmp178
== 1){goto _LL1AC;}else{goto _LL1A8;}_LL1AA: Cyc_Tcexp_check_format_args(te,_tmp176,
fmt_args,Cyc_Formatstr_get_format_typs);goto _LL1A8;_LL1AC: Cyc_Tcexp_check_format_args(
te,_tmp176,fmt_args,Cyc_Formatstr_get_scanf_typs);goto _LL1A8;_LL1A8:;}};
_pop_handler();}else{void*_tmp174=(void*)_exn_thrown;void*_tmp17A=_tmp174;_LL1AE:
if(_tmp17A == Cyc_List_Nth){goto _LL1AF;}else{goto _LL1B0;}_LL1B0: goto _LL1B1;_LL1AF:({
void*_tmp17B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format arguments",sizeof(
unsigned char),21),_tag_arr(_tmp17B,sizeof(void*),0));});goto _LL1AD;_LL1B1:(void)
_throw(_tmp17A);_LL1AD:;}}}goto _LL1A0;_LL1A4: goto _LL1A0;_LL1A0:;}}Cyc_Tcenv_check_effect_accessible(
te,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp150))->v);Cyc_Tcenv_check_rgn_partial_order(
te,loc,_tmp14B);return _tmp14F;_LL189: return({void*_tmp17C[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expected pointer to function",sizeof(unsigned char),29),
_tag_arr(_tmp17C,sizeof(void*),0));});_LL185:;}_LL17E: return({void*_tmp17D[0]={};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("expected pointer to function",sizeof(
unsigned char),29),_tag_arr(_tmp17D,sizeof(void*),0));});_LL17A:;}}static void*
Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,e);if(
! Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ)){({struct Cyc_Std_String_pa_struct
_tmp17F;_tmp17F.tag=0;_tmp17F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp17E[1]={& _tmp17F};Cyc_Tcutil_terr(
loc,_tag_arr("expected xtunion exn but found %s",sizeof(unsigned char),34),
_tag_arr(_tmp17E,sizeof(void*),1));}});}return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp180;}));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp181=t1;struct Cyc_Absyn_PtrInfo _tmp182;struct Cyc_Absyn_Conref*_tmp183;
struct Cyc_Absyn_Tqual _tmp184;struct Cyc_Absyn_Conref*_tmp185;void*_tmp186;void*
_tmp187;_LL1B3: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)== 4: 0){_LL1B7:
_tmp182=((struct Cyc_Absyn_PointerType_struct*)_tmp181)->f1;_LL1BC: _tmp187=(void*)
_tmp182.elt_typ;goto _LL1BB;_LL1BB: _tmp186=(void*)_tmp182.rgn_typ;goto _LL1BA;
_LL1BA: _tmp185=_tmp182.nullable;goto _LL1B9;_LL1B9: _tmp184=_tmp182.tq;goto _LL1B8;
_LL1B8: _tmp183=_tmp182.bounds;goto _LL1B4;}else{goto _LL1B5;}_LL1B5: goto _LL1B6;
_LL1B4:{void*_tmp188=Cyc_Tcutil_compress(_tmp187);struct Cyc_Absyn_FnInfo _tmp189;
struct Cyc_List_List*_tmp18A;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_VarargInfo*
_tmp18C;int _tmp18D;struct Cyc_List_List*_tmp18E;void*_tmp18F;struct Cyc_Core_Opt*
_tmp190;struct Cyc_List_List*_tmp191;_LL1BE: if((unsigned int)_tmp188 > 3?*((int*)
_tmp188)== 8: 0){_LL1C2: _tmp189=((struct Cyc_Absyn_FnType_struct*)_tmp188)->f1;
_LL1CA: _tmp191=_tmp189.tvars;goto _LL1C9;_LL1C9: _tmp190=_tmp189.effect;goto _LL1C8;
_LL1C8: _tmp18F=(void*)_tmp189.ret_typ;goto _LL1C7;_LL1C7: _tmp18E=_tmp189.args;
goto _LL1C6;_LL1C6: _tmp18D=_tmp189.c_varargs;goto _LL1C5;_LL1C5: _tmp18C=_tmp189.cyc_varargs;
goto _LL1C4;_LL1C4: _tmp18B=_tmp189.rgn_po;goto _LL1C3;_LL1C3: _tmp18A=_tmp189.attributes;
goto _LL1BF;}else{goto _LL1C0;}_LL1C0: goto _LL1C1;_LL1BF: {struct Cyc_List_List*
instantiation=0;if(Cyc_List_length(ts)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp191)){return({void*_tmp192[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type instantiation mismatch",
sizeof(unsigned char),28),_tag_arr(_tmp192,sizeof(void*),0));});}for(0;ts != 0;(
ts=ts->tl,_tmp191=_tmp191->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp191))->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp193=_cycalloc(
sizeof(*_tmp193));_tmp193->hd=({struct _tuple5*_tmp194=_cycalloc(sizeof(*_tmp194));
_tmp194->f1=(struct Cyc_Absyn_Tvar*)_tmp191->hd;_tmp194->f2=(void*)ts->hd;_tmp194;});
_tmp193->tl=instantiation;_tmp193;});}{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp198=_cycalloc(sizeof(*
_tmp198));_tmp198[0]=({struct Cyc_Absyn_FnType_struct _tmp199;_tmp199.tag=8;
_tmp199.f1=({struct Cyc_Absyn_FnInfo _tmp19A;_tmp19A.tvars=0;_tmp19A.effect=
_tmp190;_tmp19A.ret_typ=(void*)_tmp18F;_tmp19A.args=_tmp18E;_tmp19A.c_varargs=
_tmp18D;_tmp19A.cyc_varargs=_tmp18C;_tmp19A.rgn_po=_tmp18B;_tmp19A.attributes=
_tmp18A;_tmp19A;});_tmp199;});_tmp198;}));void*new_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195[0]=({struct Cyc_Absyn_PointerType_struct
_tmp196;_tmp196.tag=4;_tmp196.f1=({struct Cyc_Absyn_PtrInfo _tmp197;_tmp197.elt_typ=(
void*)new_fn_typ;_tmp197.rgn_typ=(void*)_tmp186;_tmp197.nullable=_tmp185;_tmp197.tq=
_tmp184;_tmp197.bounds=_tmp183;_tmp197;});_tmp196;});_tmp195;});return new_typ;}}
_LL1C1: goto _LL1BD;_LL1BD:;}goto _LL1B2;_LL1B6: goto _LL1B2;_LL1B2:;}return({struct
Cyc_Std_String_pa_struct _tmp19C;_tmp19C.tag=0;_tmp19C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{void*_tmp19B[1]={& _tmp19C};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting polymorphic type but found %s",
sizeof(unsigned char),40),_tag_arr(_tmp19B,sizeof(void*),1));}});}}static void*
Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,void*t,struct Cyc_Absyn_Exp*e){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,0,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if((! Cyc_Tcutil_silent_castable(te,loc,t2,t)? ! Cyc_Tcutil_castable(
te,loc,t2,t): 0)? ! Cyc_Tcutil_zero_to_null(te,t,e): 0){Cyc_Tcutil_unify(t2,t);{void*
_tmp19D=({struct Cyc_Std_String_pa_struct _tmp1A0;_tmp1A0.tag=0;_tmp1A0.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp19F;
_tmp19F.tag=0;_tmp19F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*
_tmp19E[2]={& _tmp19F,& _tmp1A0};Cyc_Tcexp_expr_err(te,loc,_tag_arr("cannot cast %s to %s",
sizeof(unsigned char),21),_tag_arr(_tmp19E,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return _tmp19D;}}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp1A1=0;struct Cyc_Absyn_Tqual _tmp1A2=Cyc_Absyn_empty_tqual();if(topt
!= 0){void*_tmp1A3=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp1A4;
struct Cyc_Absyn_Tqual _tmp1A5;void*_tmp1A6;_LL1CC: if((unsigned int)_tmp1A3 > 3?*((
int*)_tmp1A3)== 4: 0){_LL1D0: _tmp1A4=((struct Cyc_Absyn_PointerType_struct*)
_tmp1A3)->f1;_LL1D2: _tmp1A6=(void*)_tmp1A4.elt_typ;goto _LL1D1;_LL1D1: _tmp1A5=
_tmp1A4.tq;goto _LL1CD;}else{goto _LL1CE;}_LL1CE: goto _LL1CF;_LL1CD: _tmp1A1=({void**
_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7[0]=_tmp1A6;_tmp1A7;});_tmp1A2=
_tmp1A5;goto _LL1CB;_LL1CF: goto _LL1CB;_LL1CB:;}Cyc_Tcexp_tcExpNoInst(te,_tmp1A1,e);{
void*_tmp1A8=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;
_LL1D4: if(*((int*)_tmp1A8)== 28){goto _LL1D5;}else{goto _LL1D6;}_LL1D6: if(*((int*)
_tmp1A8)== 24){goto _LL1D7;}else{goto _LL1D8;}_LL1D8: if(*((int*)_tmp1A8)== 23){
_LL1DD: _tmp1AA=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1A8)->f1;goto _LL1DC;
_LL1DC: _tmp1A9=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1A8)->f2;goto _LL1D9;}
else{goto _LL1DA;}_LL1DA: goto _LL1DB;_LL1D5: goto _LL1D7;_LL1D7:({void*_tmp1AB[0]={};
Cyc_Tcutil_warn(loc,_tag_arr("& used to allocate",sizeof(unsigned char),19),
_tag_arr(_tmp1AB,sizeof(void*),0));});{void*(*_tmp1AC)(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq)=Cyc_Absyn_at_typ;if(topt != 0){void*_tmp1AD=Cyc_Tcutil_compress(*topt);struct
Cyc_Absyn_PtrInfo _tmp1AE;struct Cyc_Absyn_Conref*_tmp1AF;_LL1DF: if((unsigned int)
_tmp1AD > 3?*((int*)_tmp1AD)== 4: 0){_LL1E3: _tmp1AE=((struct Cyc_Absyn_PointerType_struct*)
_tmp1AD)->f1;_LL1E4: _tmp1AF=_tmp1AE.nullable;goto _LL1E0;}else{goto _LL1E1;}_LL1E1:
goto _LL1E2;_LL1E0: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp1AF)){_tmp1AC=Cyc_Absyn_star_typ;}goto _LL1DE;_LL1E2: goto _LL1DE;_LL1DE:;}
return _tmp1AC((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)2,
_tmp1A2);}_LL1D9:{void*_tmp1B0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1AA->topt))->v);_LL1E6: if((unsigned int)_tmp1B0 > 3?*((int*)
_tmp1B0)== 9: 0){goto _LL1E7;}else{goto _LL1E8;}_LL1E8: goto _LL1E9;_LL1E7: goto _LL1E5;
_LL1E9:(void*)(e0->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*_tmp1B1=
_cycalloc(sizeof(*_tmp1B1));_tmp1B1[0]=({struct Cyc_Absyn_Primop_e_struct _tmp1B2;
_tmp1B2.tag=3;_tmp1B2.f1=(void*)((void*)0);_tmp1B2.f2=({struct Cyc_List_List*
_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->hd=_tmp1AA;_tmp1B3->tl=({struct Cyc_List_List*
_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4->hd=_tmp1A9;_tmp1B4->tl=0;_tmp1B4;});
_tmp1B3;});_tmp1B2;});_tmp1B1;})));return Cyc_Tcexp_tcPlus(te,_tmp1AA,_tmp1A9);
_LL1E5:;}goto _LL1D3;_LL1DB: goto _LL1D3;_LL1D3:;}{void*_tmp1B6;int _tmp1B7;struct
_tuple6 _tmp1B5=Cyc_Tcutil_addressof_props(te,e);_LL1EC: _tmp1B7=_tmp1B5.f1;goto
_LL1EB;_LL1EB: _tmp1B6=_tmp1B5.f2;goto _LL1EA;_LL1EA: {struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual();
if(_tmp1B7){tq.q_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp1B6,tq);return t;}}}}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(! Cyc_Evexp_okay_szofarg(t)){({
struct Cyc_Std_String_pa_struct _tmp1B9;_tmp1B9.tag=0;_tmp1B9.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1B8[1]={& _tmp1B9};Cyc_Tcutil_terr(
loc,_tag_arr("sizeof applied to type %s, which has unknown size here",sizeof(
unsigned char),55),_tag_arr(_tmp1B8,sizeof(void*),1));}});}if(topt != 0){void*
_tmp1BA=Cyc_Tcutil_compress(*topt);_LL1EE: if((unsigned int)_tmp1BA > 3?*((int*)
_tmp1BA)== 13: 0){goto _LL1EF;}else{goto _LL1F0;}_LL1F0: goto _LL1F1;_LL1EF: return(
void*)({struct Cyc_Absyn_SizeofType_struct*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));
_tmp1BB[0]=({struct Cyc_Absyn_SizeofType_struct _tmp1BC;_tmp1BC.tag=13;_tmp1BC.f1=(
void*)t;_tmp1BC;});_tmp1BB;});_LL1F1: goto _LL1ED;_LL1ED:;}return Cyc_Absyn_uint_t;}
int Cyc_Tcexp_structfield_has_name(struct _tagged_arr*n,struct Cyc_Absyn_Structfield*
sf){return Cyc_Std_strcmp(*n,*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp1BD=n;struct _tagged_arr*_tmp1BE;unsigned int _tmp1BF;_LL1F3: if(*((int*)
_tmp1BD)== 0){_LL1F7: _tmp1BE=((struct Cyc_Absyn_StructField_struct*)_tmp1BD)->f1;
goto _LL1F4;}else{goto _LL1F5;}_LL1F5: if(*((int*)_tmp1BD)== 1){_LL1F8: _tmp1BF=((
struct Cyc_Absyn_TupleIndex_struct*)_tmp1BD)->f1;goto _LL1F6;}else{goto _LL1F2;}
_LL1F4: {int bad_type=1;{void*_tmp1C0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Structdecl**
_tmp1C1;struct _tuple1*_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_Absyn_Uniondecl**
_tmp1C4;struct _tuple1*_tmp1C5;struct Cyc_List_List*_tmp1C6;_LL1FA: if((
unsigned int)_tmp1C0 > 3?*((int*)_tmp1C0)== 10: 0){_LL205: _tmp1C2=((struct Cyc_Absyn_StructType_struct*)
_tmp1C0)->f1;goto _LL204;_LL204: _tmp1C1=((struct Cyc_Absyn_StructType_struct*)
_tmp1C0)->f3;goto _LL1FB;}else{goto _LL1FC;}_LL1FC: if((unsigned int)_tmp1C0 > 3?*((
int*)_tmp1C0)== 14: 0){_LL206: _tmp1C3=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp1C0)->f1;goto _LL1FD;}else{goto _LL1FE;}_LL1FE: if((unsigned int)_tmp1C0 > 3?*((
int*)_tmp1C0)== 11: 0){_LL208: _tmp1C5=((struct Cyc_Absyn_UnionType_struct*)_tmp1C0)->f1;
goto _LL207;_LL207: _tmp1C4=((struct Cyc_Absyn_UnionType_struct*)_tmp1C0)->f3;goto
_LL1FF;}else{goto _LL200;}_LL200: if((unsigned int)_tmp1C0 > 3?*((int*)_tmp1C0)== 
15: 0){_LL209: _tmp1C6=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp1C0)->f1;goto
_LL201;}else{goto _LL202;}_LL202: goto _LL203;_LL1FB: if(_tmp1C1 == 0){return({void*
_tmp1C7[0]={};Cyc_Tcutil_impos(_tag_arr("offsetof on unchecked StructType",
sizeof(unsigned char),33),_tag_arr(_tmp1C7,sizeof(void*),0));});}if((*_tmp1C1)->fields
== 0){goto _LL1F9;}if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Structfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1BE,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*_tmp1C1)->fields))->v)){({
struct Cyc_Std_String_pa_struct _tmp1C9;_tmp1C9.tag=0;_tmp1C9.f1=(struct
_tagged_arr)*_tmp1BE;{void*_tmp1C8[1]={& _tmp1C9};Cyc_Tcutil_terr(loc,_tag_arr("no field of struct has name %s",
sizeof(unsigned char),31),_tag_arr(_tmp1C8,sizeof(void*),1));}});}bad_type=0;
goto _LL1F9;_LL1FD: if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Structfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1BE,_tmp1C3)){({struct Cyc_Std_String_pa_struct _tmp1CB;_tmp1CB.tag=0;_tmp1CB.f1=(
struct _tagged_arr)*_tmp1BE;{void*_tmp1CA[1]={& _tmp1CB};Cyc_Tcutil_terr(loc,
_tag_arr("no field of struct has name %s",sizeof(unsigned char),31),_tag_arr(
_tmp1CA,sizeof(void*),1));}});}bad_type=0;goto _LL1F9;_LL1FF: if(_tmp1C4 == 0){
return({void*_tmp1CC[0]={};Cyc_Tcutil_impos(_tag_arr("offsetof on unchecked UnionType",
sizeof(unsigned char),32),_tag_arr(_tmp1CC,sizeof(void*),0));});}if((*_tmp1C4)->fields
== 0){goto _LL1F9;}if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Structfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1BE,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*_tmp1C4)->fields))->v)){({
struct Cyc_Std_String_pa_struct _tmp1CE;_tmp1CE.tag=0;_tmp1CE.f1=(struct
_tagged_arr)*_tmp1BE;{void*_tmp1CD[1]={& _tmp1CE};Cyc_Tcutil_terr(loc,_tag_arr("no field of union has name %s",
sizeof(unsigned char),30),_tag_arr(_tmp1CD,sizeof(void*),1));}});}bad_type=0;
goto _LL1F9;_LL201: if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Structfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp1BE,_tmp1C6)){({struct Cyc_Std_String_pa_struct _tmp1D0;_tmp1D0.tag=0;_tmp1D0.f1=(
struct _tagged_arr)*_tmp1BE;{void*_tmp1CF[1]={& _tmp1D0};Cyc_Tcutil_terr(loc,
_tag_arr("no field of anonymous union has name %s",sizeof(unsigned char),40),
_tag_arr(_tmp1CF,sizeof(void*),1));}});}bad_type=0;goto _LL1F9;_LL203: goto _LL1F9;
_LL1F9:;}if(bad_type){({struct Cyc_Std_String_pa_struct _tmp1D2;_tmp1D2.tag=0;
_tmp1D2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1D1[1]={&
_tmp1D2};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known struct/union type",
sizeof(unsigned char),36),_tag_arr(_tmp1D1,sizeof(void*),1));}});}goto _LL1F2;}
_LL1F6: {int bad_type=1;{void*_tmp1D3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Structdecl**
_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_Absyn_Uniondecl**_tmp1D6;struct Cyc_List_List*
_tmp1D7;struct Cyc_List_List*_tmp1D8;struct Cyc_Absyn_TunionFieldInfo _tmp1D9;void*
_tmp1DA;struct Cyc_Absyn_Tunionfield*_tmp1DB;struct Cyc_Absyn_Tuniondecl*_tmp1DC;
_LL20B: if((unsigned int)_tmp1D3 > 3?*((int*)_tmp1D3)== 10: 0){_LL219: _tmp1D4=((
struct Cyc_Absyn_StructType_struct*)_tmp1D3)->f3;goto _LL20C;}else{goto _LL20D;}
_LL20D: if((unsigned int)_tmp1D3 > 3?*((int*)_tmp1D3)== 14: 0){_LL21A: _tmp1D5=((
struct Cyc_Absyn_AnonStructType_struct*)_tmp1D3)->f1;goto _LL20E;}else{goto _LL20F;}
_LL20F: if((unsigned int)_tmp1D3 > 3?*((int*)_tmp1D3)== 11: 0){_LL21B: _tmp1D6=((
struct Cyc_Absyn_UnionType_struct*)_tmp1D3)->f3;goto _LL210;}else{goto _LL211;}
_LL211: if((unsigned int)_tmp1D3 > 3?*((int*)_tmp1D3)== 15: 0){_LL21C: _tmp1D7=((
struct Cyc_Absyn_AnonUnionType_struct*)_tmp1D3)->f1;goto _LL212;}else{goto _LL213;}
_LL213: if((unsigned int)_tmp1D3 > 3?*((int*)_tmp1D3)== 9: 0){_LL21D: _tmp1D8=((
struct Cyc_Absyn_TupleType_struct*)_tmp1D3)->f1;goto _LL214;}else{goto _LL215;}
_LL215: if((unsigned int)_tmp1D3 > 3?*((int*)_tmp1D3)== 3: 0){_LL21E: _tmp1D9=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp1D3)->f1;_LL21F: _tmp1DA=(void*)
_tmp1D9.field_info;if(*((int*)_tmp1DA)== 1){_LL221: _tmp1DC=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp1DA)->f1;goto _LL220;_LL220: _tmp1DB=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp1DA)->f2;goto _LL216;}else{goto _LL217;}}else{goto _LL217;}_LL217: goto _LL218;
_LL20C: if(_tmp1D4 == 0){return({void*_tmp1DD[0]={};Cyc_Tcutil_impos(_tag_arr("offsetof on unchecked StructType",
sizeof(unsigned char),33),_tag_arr(_tmp1DD,sizeof(void*),0));});}if((*_tmp1D4)->fields
== 0){goto _LL20A;}if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null((*_tmp1D4)->fields))->v)<= _tmp1BF){({struct Cyc_Std_Int_pa_struct
_tmp1E0;_tmp1E0.tag=1;_tmp1E0.f1=(int)_tmp1BF;{struct Cyc_Std_Int_pa_struct
_tmp1DF;_tmp1DF.tag=1;_tmp1DF.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*
_tmp1D4)->fields))->v));{void*_tmp1DE[2]={& _tmp1DF,& _tmp1E0};Cyc_Tcutil_terr(loc,
_tag_arr("struct has too few components: %d <= %d",sizeof(unsigned char),40),
_tag_arr(_tmp1DE,sizeof(void*),2));}}});}bad_type=0;goto _LL20A;_LL20E: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1D5)<= _tmp1BF){({struct Cyc_Std_Int_pa_struct
_tmp1E3;_tmp1E3.tag=1;_tmp1E3.f1=(int)_tmp1BF;{struct Cyc_Std_Int_pa_struct
_tmp1E2;_tmp1E2.tag=1;_tmp1E2.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp1D5));{void*_tmp1E1[2]={& _tmp1E2,& _tmp1E3};Cyc_Tcutil_terr(
loc,_tag_arr("struct has too few components: %d <= %d",sizeof(unsigned char),40),
_tag_arr(_tmp1E1,sizeof(void*),2));}}});}bad_type=0;goto _LL20A;_LL210: if(_tmp1D6
== 0){return({void*_tmp1E4[0]={};Cyc_Tcutil_impos(_tag_arr("offsetof on unchecked UnionType",
sizeof(unsigned char),32),_tag_arr(_tmp1E4,sizeof(void*),0));});}if((*_tmp1D6)->fields
== 0){goto _LL20A;}if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null((*_tmp1D6)->fields))->v)<= _tmp1BF){({struct Cyc_Std_Int_pa_struct
_tmp1E7;_tmp1E7.tag=1;_tmp1E7.f1=(int)_tmp1BF;{struct Cyc_Std_Int_pa_struct
_tmp1E6;_tmp1E6.tag=1;_tmp1E6.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*
_tmp1D6)->fields))->v));{void*_tmp1E5[2]={& _tmp1E6,& _tmp1E7};Cyc_Tcutil_terr(loc,
_tag_arr("union has too few components: %d <= %d",sizeof(unsigned char),39),
_tag_arr(_tmp1E5,sizeof(void*),2));}}});}bad_type=0;goto _LL20A;_LL212: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1D7)<= _tmp1BF){({struct Cyc_Std_Int_pa_struct
_tmp1EA;_tmp1EA.tag=1;_tmp1EA.f1=(int)_tmp1BF;{struct Cyc_Std_Int_pa_struct
_tmp1E9;_tmp1E9.tag=1;_tmp1E9.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp1D7));{void*_tmp1E8[2]={& _tmp1E9,& _tmp1EA};Cyc_Tcutil_terr(
loc,_tag_arr("union has too few components: %d <= %d",sizeof(unsigned char),39),
_tag_arr(_tmp1E8,sizeof(void*),2));}}});}bad_type=0;goto _LL20A;_LL214: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1D8)<= _tmp1BF){({struct Cyc_Std_Int_pa_struct
_tmp1ED;_tmp1ED.tag=1;_tmp1ED.f1=(int)_tmp1BF;{struct Cyc_Std_Int_pa_struct
_tmp1EC;_tmp1EC.tag=1;_tmp1EC.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp1D8));{void*_tmp1EB[2]={& _tmp1EC,& _tmp1ED};Cyc_Tcutil_terr(
loc,_tag_arr("tuple has too few components: %d <= %d",sizeof(unsigned char),39),
_tag_arr(_tmp1EB,sizeof(void*),2));}}});}bad_type=0;goto _LL20A;_LL216: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp1DB->typs)< _tmp1BF){({struct Cyc_Std_Int_pa_struct
_tmp1F0;_tmp1F0.tag=1;_tmp1F0.f1=(int)_tmp1BF;{struct Cyc_Std_Int_pa_struct
_tmp1EF;_tmp1EF.tag=1;_tmp1EF.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp1DB->typs));{void*_tmp1EE[2]={& _tmp1EF,& _tmp1F0};Cyc_Tcutil_terr(
loc,_tag_arr("tunionfield has too few components: %d < %d",sizeof(unsigned char),
44),_tag_arr(_tmp1EE,sizeof(void*),2));}}});}bad_type=0;goto _LL20A;_LL218: goto
_LL20A;_LL20A:;}if(bad_type){({struct Cyc_Std_String_pa_struct _tmp1F2;_tmp1F2.tag=
0;_tmp1F2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1F1[1]={&
_tmp1F2};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known struct/union/tuple/tunion type",
sizeof(unsigned char),49),_tag_arr(_tmp1F1,sizeof(void*),1));}});}goto _LL1F2;}
_LL1F2:;}return Cyc_Absyn_uint_t;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1F3=t;struct Cyc_Absyn_PtrInfo _tmp1F4;struct Cyc_Absyn_Conref*_tmp1F5;
void*_tmp1F6;void*_tmp1F7;_LL223: if((unsigned int)_tmp1F3 > 3?*((int*)_tmp1F3)== 
4: 0){_LL227: _tmp1F4=((struct Cyc_Absyn_PointerType_struct*)_tmp1F3)->f1;_LL22A:
_tmp1F7=(void*)_tmp1F4.elt_typ;goto _LL229;_LL229: _tmp1F6=(void*)_tmp1F4.rgn_typ;
goto _LL228;_LL228: _tmp1F5=_tmp1F4.bounds;goto _LL224;}else{goto _LL225;}_LL225:
goto _LL226;_LL224: Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp1F6);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp1F5);if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp1F7),(void*)1)){({
void*_tmp1F8[0]={};Cyc_Tcutil_terr(loc,_tag_arr("can't dereference abstract pointer type",
sizeof(unsigned char),40),_tag_arr(_tmp1F8,sizeof(void*),0));});}return _tmp1F7;
_LL226: return({struct Cyc_Std_String_pa_struct _tmp1FA;_tmp1FA.tag=0;_tmp1FA.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1F9[1]={& _tmp1FA};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting * or @ type but found %s",sizeof(unsigned char),35),
_tag_arr(_tmp1F9,sizeof(void*),1));}});_LL222:;}}static void*Cyc_Tcexp_tcStructMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,
e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1FB=t;struct Cyc_Absyn_Structdecl**_tmp1FC;struct Cyc_Absyn_Structdecl*
_tmp1FD;struct Cyc_List_List*_tmp1FE;struct _tuple1*_tmp1FF;struct Cyc_List_List*
_tmp200;struct Cyc_List_List*_tmp201;struct Cyc_Absyn_Uniondecl**_tmp202;struct Cyc_Absyn_Uniondecl*
_tmp203;struct Cyc_List_List*_tmp204;struct _tuple1*_tmp205;_LL22C: if((
unsigned int)_tmp1FB > 3?*((int*)_tmp1FB)== 10: 0){_LL23C: _tmp1FF=((struct Cyc_Absyn_StructType_struct*)
_tmp1FB)->f1;goto _LL23B;_LL23B: _tmp1FE=((struct Cyc_Absyn_StructType_struct*)
_tmp1FB)->f2;goto _LL23A;_LL23A: _tmp1FC=((struct Cyc_Absyn_StructType_struct*)
_tmp1FB)->f3;if(_tmp1FC == 0){goto _LL22E;}else{_tmp1FD=*_tmp1FC;goto _LL22D;}}
else{goto _LL22E;}_LL22E: if((unsigned int)_tmp1FB > 3?*((int*)_tmp1FB)== 14: 0){
_LL23D: _tmp200=((struct Cyc_Absyn_AnonStructType_struct*)_tmp1FB)->f1;goto _LL22F;}
else{goto _LL230;}_LL230: if((unsigned int)_tmp1FB > 3?*((int*)_tmp1FB)== 15: 0){
_LL23E: _tmp201=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp1FB)->f1;goto _LL231;}
else{goto _LL232;}_LL232: if((unsigned int)_tmp1FB > 3?*((int*)_tmp1FB)== 11: 0){
_LL241: _tmp205=((struct Cyc_Absyn_UnionType_struct*)_tmp1FB)->f1;goto _LL240;
_LL240: _tmp204=((struct Cyc_Absyn_UnionType_struct*)_tmp1FB)->f2;goto _LL23F;
_LL23F: _tmp202=((struct Cyc_Absyn_UnionType_struct*)_tmp1FB)->f3;if(_tmp202 == 0){
goto _LL234;}else{_tmp203=*_tmp202;goto _LL233;}}else{goto _LL234;}_LL234: if((
unsigned int)_tmp1FB > 3?*((int*)_tmp1FB)== 7: 0){goto _LL242;}else{goto _LL236;}
_LL242: if(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(unsigned char),5))== 0){goto
_LL235;}else{goto _LL236;}_LL236: if((unsigned int)_tmp1FB > 3?*((int*)_tmp1FB)== 4:
0){goto _LL243;}else{goto _LL238;}_LL243: if(Cyc_Std_zstrcmp(*f,_tag_arr("size",
sizeof(unsigned char),5))== 0){goto _LL237;}else{goto _LL238;}_LL238: goto _LL239;
_LL22D: if(_tmp1FF == 0){return({void*_tmp206[0]={};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("unresolved struct type (COMPILER ERROR)",sizeof(unsigned char),40),
_tag_arr(_tmp206,sizeof(void*),0));});}{struct Cyc_Absyn_Structfield*_tmp207=Cyc_Absyn_lookup_struct_field(
_tmp1FD,f);if(_tmp207 == 0){return({struct Cyc_Std_String_pa_struct _tmp20A;_tmp20A.tag=
0;_tmp20A.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct _tmp209;
_tmp209.tag=0;_tmp209.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((struct
_tuple1*)_check_null(_tmp1FF));{void*_tmp208[2]={& _tmp209,& _tmp20A};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("struct %s has no %s field",sizeof(unsigned char),26),_tag_arr(
_tmp208,sizeof(void*),2));}}});}{void*t2;{struct _RegionHandle _tmp20B=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp20B;_push_region(rgn);{struct Cyc_List_List*_tmp20C=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1FD->tvs,_tmp1FE);t2=Cyc_Tcutil_rsubstitute(
rgn,_tmp20C,(void*)_tmp207->type);};_pop_region(rgn);}return t2;}}_LL22F: {struct
Cyc_Absyn_Structfield*_tmp20D=Cyc_Absyn_lookup_field(_tmp200,f);if(_tmp20D == 0){
return({struct Cyc_Std_String_pa_struct _tmp20F;_tmp20F.tag=0;_tmp20F.f1=(struct
_tagged_arr)*f;{void*_tmp20E[1]={& _tmp20F};Cyc_Tcexp_expr_err(te,loc,_tag_arr("struct has no %s field",
sizeof(unsigned char),23),_tag_arr(_tmp20E,sizeof(void*),1));}});}return(void*)
_tmp20D->type;}_LL231: {struct Cyc_Absyn_Structfield*_tmp210=Cyc_Absyn_lookup_field(
_tmp201,f);if(_tmp210 == 0){return({struct Cyc_Std_String_pa_struct _tmp212;_tmp212.tag=
0;_tmp212.f1=(struct _tagged_arr)*f;{void*_tmp211[1]={& _tmp212};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("union has no %s field",sizeof(unsigned char),22),_tag_arr(
_tmp211,sizeof(void*),1));}});}return(void*)_tmp210->type;}_LL233: if(_tmp205 == 0){
return({void*_tmp213[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("unresolved union type (COMPILER ERROR)",
sizeof(unsigned char),39),_tag_arr(_tmp213,sizeof(void*),0));});}{struct Cyc_Absyn_Structfield*
_tmp214=Cyc_Absyn_lookup_union_field(_tmp203,f);if(_tmp214 == 0){return({struct
Cyc_Std_String_pa_struct _tmp217;_tmp217.tag=0;_tmp217.f1=(struct _tagged_arr)*f;{
struct Cyc_Std_String_pa_struct _tmp216;_tmp216.tag=0;_tmp216.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string((struct _tuple1*)_check_null(_tmp205));{void*
_tmp215[2]={& _tmp216,& _tmp217};Cyc_Tcexp_expr_err(te,loc,_tag_arr("union %s has no %s field",
sizeof(unsigned char),25),_tag_arr(_tmp215,sizeof(void*),2));}}});}{struct Cyc_List_List*
_tmp218=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmp203->tvs,_tmp204);void*_tmp219=Cyc_Tcutil_substitute(_tmp218,(
void*)_tmp214->type);return _tmp219;}}_LL235:(void*)(outer_e->r=(void*)((void*)({
struct Cyc_Absyn_Primop_e_struct*_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A[0]=({
struct Cyc_Absyn_Primop_e_struct _tmp21B;_tmp21B.tag=3;_tmp21B.f1=(void*)((void*)
19);_tmp21B.f2=({struct Cyc_List_List*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->hd=
e;_tmp21C->tl=0;_tmp21C;});_tmp21B;});_tmp21A;})));return Cyc_Absyn_uint_t;_LL237:(
void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*_tmp21D=
_cycalloc(sizeof(*_tmp21D));_tmp21D[0]=({struct Cyc_Absyn_Primop_e_struct _tmp21E;
_tmp21E.tag=3;_tmp21E.f1=(void*)((void*)19);_tmp21E.f2=({struct Cyc_List_List*
_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->hd=e;_tmp21F->tl=0;_tmp21F;});
_tmp21E;});_tmp21D;})));return Cyc_Absyn_uint_t;_LL239: if(Cyc_Std_zstrcmp(*f,
_tag_arr("size",sizeof(unsigned char),5))== 0){return({struct Cyc_Std_String_pa_struct
_tmp221;_tmp221.tag=0;_tmp221.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp220[1]={& _tmp221};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting struct, union or array, found %s",
sizeof(unsigned char),43),_tag_arr(_tmp220,sizeof(void*),1));}});}else{return({
struct Cyc_Std_String_pa_struct _tmp223;_tmp223.tag=0;_tmp223.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp222[1]={& _tmp223};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct or union, found %s",sizeof(unsigned char),36),
_tag_arr(_tmp222,sizeof(void*),1));}});}_LL22B:;}}static void*Cyc_Tcexp_tcStructArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _tagged_arr*f){Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp224=t;struct Cyc_Absyn_PtrInfo
_tmp225;struct Cyc_Absyn_Conref*_tmp226;void*_tmp227;void*_tmp228;_LL245: if((
unsigned int)_tmp224 > 3?*((int*)_tmp224)== 4: 0){_LL249: _tmp225=((struct Cyc_Absyn_PointerType_struct*)
_tmp224)->f1;_LL24C: _tmp228=(void*)_tmp225.elt_typ;goto _LL24B;_LL24B: _tmp227=(
void*)_tmp225.rgn_typ;goto _LL24A;_LL24A: _tmp226=_tmp225.bounds;goto _LL246;}else{
goto _LL247;}_LL247: goto _LL248;_LL246: Cyc_Tcutil_check_nonzero_bound(loc,_tmp226);{
void*_tmp229=Cyc_Tcutil_compress(_tmp228);struct Cyc_Absyn_Structdecl**_tmp22A;
struct Cyc_Absyn_Structdecl*_tmp22B;struct Cyc_List_List*_tmp22C;struct _tuple1*
_tmp22D;struct Cyc_Absyn_Uniondecl**_tmp22E;struct Cyc_Absyn_Uniondecl*_tmp22F;
struct Cyc_List_List*_tmp230;struct _tuple1*_tmp231;struct Cyc_List_List*_tmp232;
struct Cyc_List_List*_tmp233;_LL24E: if((unsigned int)_tmp229 > 3?*((int*)_tmp229)
== 10: 0){_LL25A: _tmp22D=((struct Cyc_Absyn_StructType_struct*)_tmp229)->f1;goto
_LL259;_LL259: _tmp22C=((struct Cyc_Absyn_StructType_struct*)_tmp229)->f2;goto
_LL258;_LL258: _tmp22A=((struct Cyc_Absyn_StructType_struct*)_tmp229)->f3;if(
_tmp22A == 0){goto _LL250;}else{_tmp22B=*_tmp22A;goto _LL24F;}}else{goto _LL250;}
_LL250: if((unsigned int)_tmp229 > 3?*((int*)_tmp229)== 11: 0){_LL25D: _tmp231=((
struct Cyc_Absyn_UnionType_struct*)_tmp229)->f1;goto _LL25C;_LL25C: _tmp230=((
struct Cyc_Absyn_UnionType_struct*)_tmp229)->f2;goto _LL25B;_LL25B: _tmp22E=((
struct Cyc_Absyn_UnionType_struct*)_tmp229)->f3;if(_tmp22E == 0){goto _LL252;}else{
_tmp22F=*_tmp22E;goto _LL251;}}else{goto _LL252;}_LL252: if((unsigned int)_tmp229 > 
3?*((int*)_tmp229)== 14: 0){_LL25E: _tmp232=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp229)->f1;goto _LL253;}else{goto _LL254;}_LL254: if((unsigned int)_tmp229 > 3?*((
int*)_tmp229)== 15: 0){_LL25F: _tmp233=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp229)->f1;goto _LL255;}else{goto _LL256;}_LL256: goto _LL257;_LL24F: {struct Cyc_Absyn_Structfield*
_tmp234=Cyc_Absyn_lookup_struct_field(_tmp22B,f);if(_tmp234 == 0){return({struct
Cyc_Std_String_pa_struct _tmp237;_tmp237.tag=0;_tmp237.f1=(struct _tagged_arr)*f;{
struct Cyc_Std_String_pa_struct _tmp236;_tmp236.tag=0;_tmp236.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string((struct _tuple1*)_check_null(_tmp22D));{void*
_tmp235[2]={& _tmp236,& _tmp237};Cyc_Tcexp_expr_err(te,loc,_tag_arr("struct %s has no %s field",
sizeof(unsigned char),26),_tag_arr(_tmp235,sizeof(void*),2));}}});}{void*t3;{
struct _RegionHandle _tmp238=_new_region("rgn");struct _RegionHandle*rgn=& _tmp238;
_push_region(rgn);{struct Cyc_List_List*_tmp239=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp22B->tvs,_tmp22C);t3=Cyc_Tcutil_rsubstitute(rgn,
_tmp239,(void*)_tmp234->type);};_pop_region(rgn);}Cyc_Tcenv_check_rgn_accessible(
te,loc,_tmp227);return t3;}}_LL251: {struct Cyc_Absyn_Structfield*_tmp23A=Cyc_Absyn_lookup_union_field(
_tmp22F,f);if(_tmp23A == 0){return({struct Cyc_Std_String_pa_struct _tmp23D;_tmp23D.tag=
0;_tmp23D.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct _tmp23C;
_tmp23C.tag=0;_tmp23C.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((struct
_tuple1*)_check_null(_tmp231));{void*_tmp23B[2]={& _tmp23C,& _tmp23D};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("union %s has no %s field",sizeof(unsigned char),25),_tag_arr(
_tmp23B,sizeof(void*),2));}}});}{struct Cyc_List_List*_tmp23E=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp22F->tvs,_tmp230);
void*_tmp23F=Cyc_Tcutil_substitute(_tmp23E,(void*)_tmp23A->type);Cyc_Tcenv_check_rgn_accessible(
te,loc,_tmp227);return _tmp23F;}}_LL253: {struct Cyc_Absyn_Structfield*_tmp240=Cyc_Absyn_lookup_field(
_tmp232,f);if(_tmp240 == 0){return({struct Cyc_Std_String_pa_struct _tmp242;_tmp242.tag=
0;_tmp242.f1=(struct _tagged_arr)*f;{void*_tmp241[1]={& _tmp242};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("struct has no %s field",sizeof(unsigned char),23),_tag_arr(
_tmp241,sizeof(void*),1));}});}return(void*)_tmp240->type;}_LL255: {struct Cyc_Absyn_Structfield*
_tmp243=Cyc_Absyn_lookup_field(_tmp233,f);if(_tmp243 == 0){return({struct Cyc_Std_String_pa_struct
_tmp245;_tmp245.tag=0;_tmp245.f1=(struct _tagged_arr)*f;{void*_tmp244[1]={&
_tmp245};Cyc_Tcexp_expr_err(te,loc,_tag_arr("union has no %s field",sizeof(
unsigned char),22),_tag_arr(_tmp244,sizeof(void*),1));}});}return(void*)_tmp243->type;}
_LL257: return({struct Cyc_Std_String_pa_struct _tmp247;_tmp247.tag=0;_tmp247.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp246[1]={& _tmp247};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct pointer, found %s",sizeof(unsigned char),35),
_tag_arr(_tmp246,sizeof(void*),1));}});_LL24D:;}_LL248: return({struct Cyc_Std_String_pa_struct
_tmp249;_tmp249.tag=0;_tmp249.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp248[1]={& _tmp249};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting struct pointer, found %s",
sizeof(unsigned char),35),_tag_arr(_tmp248,sizeof(void*),1));}});_LL244:;}}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int i=Cyc_Evexp_eval_const_uint_exp(
index);struct _handler_cons _tmp24A;_push_handler(& _tmp24A);{int _tmp24C=0;if(
setjmp(_tmp24A.handler)){_tmp24C=1;}if(! _tmp24C){{void*_tmp24D=(*((struct _tuple8*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0);return
_tmp24D;};_pop_handler();}else{void*_tmp24B=(void*)_exn_thrown;void*_tmp24F=
_tmp24B;_LL261: if(_tmp24F == Cyc_List_Nth){goto _LL262;}else{goto _LL263;}_LL263:
goto _LL264;_LL262: return({struct Cyc_Std_Int_pa_struct _tmp252;_tmp252.tag=1;
_tmp252.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts));{struct Cyc_Std_Int_pa_struct _tmp251;_tmp251.tag=1;_tmp251.f1=(int)i;{void*
_tmp250[2]={& _tmp251,& _tmp252};Cyc_Tcexp_expr_err(te,loc,_tag_arr("index is %d but tuple has only %d fields",
sizeof(unsigned char),41),_tag_arr(_tmp250,sizeof(void*),2));}}});_LL264:(void)
_throw(_tmp24F);_LL260:;}}}static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(! Cyc_Tcutil_coerce_sint_typ(
te,e2)){return({struct Cyc_Std_String_pa_struct _tmp254;_tmp254.tag=0;_tmp254.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp253[1]={& _tmp254};Cyc_Tcexp_expr_err(
te,e2->loc,_tag_arr("expecting int subscript, found %s",sizeof(unsigned char),34),
_tag_arr(_tmp253,sizeof(void*),1));}});}{void*_tmp255=t1;struct Cyc_Absyn_PtrInfo
_tmp256;struct Cyc_Absyn_Conref*_tmp257;struct Cyc_Absyn_Tqual _tmp258;void*_tmp259;
void*_tmp25A;struct Cyc_List_List*_tmp25B;_LL266: if((unsigned int)_tmp255 > 3?*((
int*)_tmp255)== 4: 0){_LL26C: _tmp256=((struct Cyc_Absyn_PointerType_struct*)
_tmp255)->f1;_LL270: _tmp25A=(void*)_tmp256.elt_typ;goto _LL26F;_LL26F: _tmp259=(
void*)_tmp256.rgn_typ;goto _LL26E;_LL26E: _tmp258=_tmp256.tq;goto _LL26D;_LL26D:
_tmp257=_tmp256.bounds;goto _LL267;}else{goto _LL268;}_LL268: if((unsigned int)
_tmp255 > 3?*((int*)_tmp255)== 9: 0){_LL271: _tmp25B=((struct Cyc_Absyn_TupleType_struct*)
_tmp255)->f1;goto _LL269;}else{goto _LL26A;}_LL26A: goto _LL26B;_LL267: if(Cyc_Tcutil_is_const_exp(
te,e2)){Cyc_Tcutil_check_bound(loc,Cyc_Evexp_eval_const_uint_exp(e2),_tmp257);}
else{if(Cyc_Tcutil_is_bound_one(_tmp257)){({void*_tmp25C[0]={};Cyc_Tcutil_warn(
e1->loc,_tag_arr("subscript applied to pointer to one object",sizeof(
unsigned char),43),_tag_arr(_tmp25C,sizeof(void*),0));});}Cyc_Tcutil_check_nonzero_bound(
loc,_tmp257);}Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp259);if(! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp25A),(void*)1)){({void*_tmp25D[0]={};Cyc_Tcutil_terr(e1->loc,
_tag_arr("can't subscript an abstract pointer",sizeof(unsigned char),36),
_tag_arr(_tmp25D,sizeof(void*),0));});}return _tmp25A;_LL269: return Cyc_Tcexp_ithTupleType(
te,loc,_tmp25B,e2);_LL26B: return({struct Cyc_Std_String_pa_struct _tmp25F;_tmp25F.tag=
0;_tmp25F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp25E[1]={&
_tmp25F};Cyc_Tcexp_expr_err(te,loc,_tag_arr("subscript applied to %s",sizeof(
unsigned char),24),_tag_arr(_tmp25E,sizeof(void*),1));}});_LL265:;}}}static void*
Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;if(topt != 0){
void*_tmp260=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp261;_LL273: if((
unsigned int)_tmp260 > 3?*((int*)_tmp260)== 9: 0){_LL277: _tmp261=((struct Cyc_Absyn_TupleType_struct*)
_tmp260)->f1;goto _LL274;}else{goto _LL275;}_LL275: goto _LL276;_LL274: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp261)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es)){goto _LL272;}for(0;es != 0;(es=es->tl,_tmp261=_tmp261->tl)){
void*_tmp262=(*((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp261))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp262,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->hd=({
struct _tuple8*_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264->f1=(*((struct _tuple8*)
_tmp261->hd)).f1;_tmp264->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp264;});_tmp263->tl=fields;_tmp263;});}done=
1;goto _LL272;_LL276: goto _LL272;_LL272:;}if(! done){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(
te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*_tmp265=
_cycalloc(sizeof(*_tmp265));_tmp265->hd=({struct _tuple8*_tmp266=_cycalloc(
sizeof(*_tmp266));_tmp266->f1=Cyc_Absyn_empty_tqual();_tmp266->f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp266;});_tmp265->tl=fields;_tmp265;});}}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=({struct Cyc_Absyn_TupleType_struct
_tmp268;_tmp268.tag=9;_tmp268.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp268;});_tmp267;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp269[0]={};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("tcCompoundLit",sizeof(unsigned char),14),_tag_arr(_tmp269,sizeof(void*),
0));});}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,struct Cyc_List_List*des){struct Cyc_List_List*es=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple7*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp27A=_cycalloc(
sizeof(*_tmp27A));_tmp27A->v=Cyc_Tcenv_lookup_type_vars(te);_tmp27A;}));struct
Cyc_Absyn_Const_e_struct*_tmp26A=({struct Cyc_Absyn_Const_e_struct*_tmp276=
_cycalloc(sizeof(*_tmp276));_tmp276[0]=({struct Cyc_Absyn_Const_e_struct _tmp277;
_tmp277.tag=0;_tmp277.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp278=
_cycalloc(sizeof(*_tmp278));_tmp278[0]=({struct Cyc_Absyn_Int_c_struct _tmp279;
_tmp279.tag=2;_tmp279.f1=(void*)((void*)1);_tmp279.f2=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es);_tmp279;});_tmp278;}));_tmp277;});_tmp276;});struct Cyc_Absyn_Exp*
sz_exp=Cyc_Absyn_new_exp((void*)_tmp26A,loc);sz_exp->topt=({struct Cyc_Core_Opt*
_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->v=(void*)Cyc_Absyn_uint_t;_tmp26B;});{
void*res_t2=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp274=_cycalloc(sizeof(*
_tmp274));_tmp274[0]=({struct Cyc_Absyn_ArrayType_struct _tmp275;_tmp275.tag=7;
_tmp275.f1=(void*)res;_tmp275.f2=Cyc_Absyn_empty_tqual();_tmp275.f3=(struct Cyc_Absyn_Exp*)
sz_exp;_tmp275;});_tmp274;});{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(! Cyc_Tcutil_coerce_list(
te,res,es)){({struct Cyc_Std_String_pa_struct _tmp26D;_tmp26D.tag=0;_tmp26D.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(res);{void*_tmp26C[1]={& _tmp26D};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,_tag_arr("elements of array do not all have the same type (%s)",
sizeof(unsigned char),53),_tag_arr(_tmp26C,sizeof(void*),1));}});}{int offset=0;
for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple7*)
des->hd)).f1;if(ds != 0){void*_tmp26E=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp26F;
_LL279: if(*((int*)_tmp26E)== 1){goto _LL27A;}else{goto _LL27B;}_LL27B: if(*((int*)
_tmp26E)== 0){_LL27D: _tmp26F=((struct Cyc_Absyn_ArrayElement_struct*)_tmp26E)->f1;
goto _LL27C;}else{goto _LL278;}_LL27A:({void*_tmp270[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("only array index designators are supported",sizeof(unsigned char),43),
_tag_arr(_tmp270,sizeof(void*),0));});goto _LL278;_LL27C: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp26F);{unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmp26F);if(i != 
offset){({struct Cyc_Std_Int_pa_struct _tmp273;_tmp273.tag=1;_tmp273.f1=(int)i;{
struct Cyc_Std_Int_pa_struct _tmp272;_tmp272.tag=1;_tmp272.f1=(int)((unsigned int)
offset);{void*_tmp271[2]={& _tmp272,& _tmp273};Cyc_Tcutil_terr(_tmp26F->loc,
_tag_arr("expecting index designator %d but found %d",sizeof(unsigned char),43),
_tag_arr(_tmp271,sizeof(void*),2));}}});}goto _LL278;}_LL278:;}}}return res_t2;}}
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body){Cyc_Tcexp_tcExp(te,0,bound);if(! Cyc_Tcutil_coerce_uint_typ(te,bound)){({
struct Cyc_Std_String_pa_struct _tmp27C;_tmp27C.tag=0;_tmp27C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
bound->topt))->v);{void*_tmp27B[1]={& _tmp27C};Cyc_Tcutil_terr(bound->loc,
_tag_arr("expecting unsigned int, found %s",sizeof(unsigned char),33),_tag_arr(
_tmp27B,sizeof(void*),1));}});}if(!(vd->tq).q_const){({void*_tmp27D[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("comprehension index variable is not declared const!",
sizeof(unsigned char),52),_tag_arr(_tmp27D,sizeof(void*),0));});}if(te->le != 0){
te=Cyc_Tcenv_new_block(loc,te);te=Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp27E=0;struct Cyc_Absyn_Tqual*_tmp27F=0;if(topt != 0){void*_tmp280=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp281;struct Cyc_Absyn_Exp*
_tmp282;struct Cyc_Absyn_Tqual _tmp283;struct Cyc_Absyn_Tqual*_tmp284;void*_tmp285;
void**_tmp286;_LL27F: if((unsigned int)_tmp280 > 3?*((int*)_tmp280)== 4: 0){_LL285:
_tmp281=((struct Cyc_Absyn_PointerType_struct*)_tmp280)->f1;goto _LL280;}else{goto
_LL281;}_LL281: if((unsigned int)_tmp280 > 3?*((int*)_tmp280)== 7: 0){_LL288:
_tmp285=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp280)->f1;_tmp286=(void**)&((
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp280)->f1);goto _LL287;_LL287:
_tmp283=((struct Cyc_Absyn_ArrayType_struct*)_tmp280)->f2;_tmp284=(struct Cyc_Absyn_Tqual*)&((
struct Cyc_Absyn_ArrayType_struct*)_tmp280)->f2;goto _LL286;_LL286: _tmp282=((
struct Cyc_Absyn_ArrayType_struct*)_tmp280)->f3;goto _LL282;}else{goto _LL283;}
_LL283: goto _LL284;_LL280: pinfo=_tmp281;_tmp27E=(void**)((void**)((void*)& pinfo.elt_typ));
_tmp27F=(struct Cyc_Absyn_Tqual*)& pinfo.tq;goto _LL27E;_LL282: _tmp27E=(void**)
_tmp286;_tmp27F=(struct Cyc_Absyn_Tqual*)_tmp284;goto _LL27E;_LL284: goto _LL27E;
_LL27E:;}{void*t=Cyc_Tcexp_tcExp(te,_tmp27E,body);if(te->le == 0){if(! Cyc_Tcutil_is_const_exp(
te,bound)){({void*_tmp287[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("bound is not constant",
sizeof(unsigned char),22),_tag_arr(_tmp287,sizeof(void*),0));});}if(! Cyc_Tcutil_is_const_exp(
te,body)){({void*_tmp288[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("body is not constant",
sizeof(unsigned char),21),_tag_arr(_tmp288,sizeof(void*),0));});}}return(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp289=_cycalloc(sizeof(*_tmp289));_tmp289[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp28A;_tmp28A.tag=7;_tmp28A.f1=(void*)t;
_tmp28A.f2=_tmp27F == 0? Cyc_Absyn_empty_tqual():*_tmp27F;_tmp28A.f3=(struct Cyc_Absyn_Exp*)
bound;_tmp28A;});_tmp289;});}}}struct _tuple9{struct Cyc_Absyn_Structfield*f1;
struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_Core_Opt**
otyps,struct Cyc_List_List*args,struct Cyc_Absyn_Structdecl**sd_opt){struct Cyc_Absyn_Structdecl*
sd;if(*sd_opt != 0){sd=(struct Cyc_Absyn_Structdecl*)_check_null(*sd_opt);}else{{
struct _handler_cons _tmp28B;_push_handler(& _tmp28B);{int _tmp28D=0;if(setjmp(
_tmp28B.handler)){_tmp28D=1;}if(! _tmp28D){sd=*Cyc_Tcenv_lookup_structdecl(te,loc,*
tn);;_pop_handler();}else{void*_tmp28C=(void*)_exn_thrown;void*_tmp28F=_tmp28C;
_LL28A: if(_tmp28F == Cyc_Dict_Absent){goto _LL28B;}else{goto _LL28C;}_LL28C: goto
_LL28D;_LL28B:({struct Cyc_Std_String_pa_struct _tmp291;_tmp291.tag=0;_tmp291.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{void*_tmp290[1]={& _tmp291};Cyc_Tcutil_terr(
loc,_tag_arr("unbound struct name %s",sizeof(unsigned char),23),_tag_arr(_tmp290,
sizeof(void*),1));}});return topt != 0?*topt:(void*)0;_LL28D:(void)_throw(_tmp28F);
_LL289:;}}}*sd_opt=(struct Cyc_Absyn_Structdecl*)sd;if(sd->name != 0){*tn=(struct
_tuple1*)((struct Cyc_Core_Opt*)_check_null(sd->name))->v;}}{struct _RegionHandle
_tmp292=_new_region("rgn");struct _RegionHandle*rgn=& _tmp292;_push_region(rgn);{
struct _tuple4 _tmp293=({struct _tuple4 _tmp2A8;_tmp2A8.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp2A8.f2=rgn;_tmp2A8;});struct Cyc_List_List*_tmp294=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp293,sd->tvs);struct Cyc_List_List*_tmp295=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp294);struct Cyc_Absyn_StructType_struct*_tmp296=({struct Cyc_Absyn_StructType_struct*
_tmp2A5=_cycalloc(sizeof(*_tmp2A5));_tmp2A5[0]=({struct Cyc_Absyn_StructType_struct
_tmp2A6;_tmp2A6.tag=10;_tmp2A6.f1=(struct _tuple1*)*tn;_tmp2A6.f2=_tmp295;_tmp2A6.f3=({
struct Cyc_Absyn_Structdecl**_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7[0]=sd;
_tmp2A7;});_tmp2A6;});_tmp2A5;});if(topt != 0){Cyc_Tcutil_unify((void*)_tmp296,*
topt);}*otyps=({struct Cyc_Core_Opt*_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297->v=
_tmp295;_tmp297;});if(sd->fields == 0){({void*_tmp298[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("can't build abstract struct",sizeof(unsigned char),28),_tag_arr(
_tmp298,sizeof(void*),0));});{void*_tmp299=(void*)_tmp296;_npop_handler(0);
return _tmp299;}}{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sd->fields))->v);for(0;
fields != 0;fields=fields->tl){struct _tuple9 _tmp29B;struct Cyc_Absyn_Exp*_tmp29C;
struct Cyc_Absyn_Structfield*_tmp29D;struct _tuple9*_tmp29A=(struct _tuple9*)fields->hd;
_tmp29B=*_tmp29A;_LL290: _tmp29D=_tmp29B.f1;goto _LL28F;_LL28F: _tmp29C=_tmp29B.f2;
goto _LL28E;_LL28E: {void*_tmp29E=Cyc_Tcutil_rsubstitute(rgn,_tmp294,(void*)
_tmp29D->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp29E,_tmp29C);if(! Cyc_Tcutil_coerce_arg(
te,_tmp29C,_tmp29E)){({struct Cyc_Std_String_pa_struct _tmp2A3;_tmp2A3.tag=0;
_tmp2A3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp29C->topt))->v);{struct Cyc_Std_String_pa_struct _tmp2A2;_tmp2A2.tag=
0;_tmp2A2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp29E);{struct Cyc_Std_String_pa_struct
_tmp2A1;_tmp2A1.tag=0;_tmp2A1.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{
struct Cyc_Std_String_pa_struct _tmp2A0;_tmp2A0.tag=0;_tmp2A0.f1=(struct
_tagged_arr)*_tmp29D->name;{void*_tmp29F[4]={& _tmp2A0,& _tmp2A1,& _tmp2A2,& _tmp2A3};
Cyc_Tcutil_terr(_tmp29C->loc,_tag_arr("field %s of struct %s expects type %s != %s",
sizeof(unsigned char),44),_tag_arr(_tmp29F,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{
void*_tmp2A4=(void*)_tmp296;_npop_handler(0);return _tmp2A4;}}};_pop_region(rgn);}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){{struct _RegionHandle _tmp2A9=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp2A9;_push_region(rgn);{void*_tmp2AA=Cyc_Tcutil_compress(
ts);struct Cyc_List_List*_tmp2AB;_LL292: if((unsigned int)_tmp2AA > 3?*((int*)
_tmp2AA)== 14: 0){_LL296: _tmp2AB=((struct Cyc_Absyn_AnonStructType_struct*)_tmp2AA)->f1;
goto _LL293;}else{goto _LL294;}_LL294: goto _LL295;_LL293: {struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,_tmp2AB);for(0;fields != 0;fields=fields->tl){struct _tuple9 _tmp2AD;
struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Structfield*_tmp2AF;struct _tuple9*
_tmp2AC=(struct _tuple9*)fields->hd;_tmp2AD=*_tmp2AC;_LL299: _tmp2AF=_tmp2AD.f1;
goto _LL298;_LL298: _tmp2AE=_tmp2AD.f2;goto _LL297;_LL297: Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp2AF->type)),_tmp2AE);if(! Cyc_Tcutil_coerce_arg(
te,_tmp2AE,(void*)_tmp2AF->type)){({struct Cyc_Std_String_pa_struct _tmp2B3;
_tmp2B3.tag=0;_tmp2B3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp2AE->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp2B2;_tmp2B2.tag=0;_tmp2B2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
_tmp2AF->type);{struct Cyc_Std_String_pa_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(
struct _tagged_arr)*_tmp2AF->name;{void*_tmp2B0[3]={& _tmp2B1,& _tmp2B2,& _tmp2B3};
Cyc_Tcutil_terr(_tmp2AE->loc,_tag_arr("field %s of struct expects type %s != %s",
sizeof(unsigned char),41),_tag_arr(_tmp2B0,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
goto _LL291;}_LL295:({void*_tmp2B4[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof(unsigned char),37),_tag_arr(_tmp2B4,sizeof(void*),0));});goto _LL291;
_LL291:;};_pop_region(rgn);}return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_Core_Opt**
all_ref,struct Cyc_Core_Opt**exist_ref,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp2B5=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp2B5;_push_region(rgn);{struct _tuple4 _tmp2B6=({
struct _tuple4 _tmp2DA;_tmp2DA.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp2DA.f2=rgn;
_tmp2DA;});struct Cyc_List_List*_tmp2B7=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp2B6,tud->tvs);struct Cyc_List_List*_tmp2B8=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp2B6,tuf->tvs);struct Cyc_List_List*_tmp2B9=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp2B7);struct Cyc_List_List*_tmp2BA=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp2B8);struct Cyc_List_List*_tmp2BB=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp2B7,_tmp2B8);*all_ref=({struct Cyc_Core_Opt*_tmp2BC=_cycalloc(sizeof(*
_tmp2BC));_tmp2BC->v=_tmp2B9;_tmp2BC;});*exist_ref=({struct Cyc_Core_Opt*_tmp2BD=
_cycalloc(sizeof(*_tmp2BD));_tmp2BD->v=_tmp2BA;_tmp2BD;});{void*res=(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));
_tmp2D5[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp2D6;_tmp2D6.tag=3;_tmp2D6.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp2D7;_tmp2D7.field_info=(void*)((void*)({
struct Cyc_Absyn_KnownTunionfield_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));
_tmp2D8[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmp2D9;_tmp2D9.tag=1;
_tmp2D9.f1=tud;_tmp2D9.f2=tuf;_tmp2D9;});_tmp2D8;}));_tmp2D7.targs=_tmp2B9;
_tmp2D7;});_tmp2D6;});_tmp2D5;});if(topt != 0){void*_tmp2BE=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_TunionInfo _tmp2BF;void*_tmp2C0;struct Cyc_List_List*_tmp2C1;
_LL29B: if((unsigned int)_tmp2BE > 3?*((int*)_tmp2BE)== 3: 0){goto _LL29C;}else{goto
_LL29D;}_LL29D: if((unsigned int)_tmp2BE > 3?*((int*)_tmp2BE)== 2: 0){_LL2A1:
_tmp2BF=((struct Cyc_Absyn_TunionType_struct*)_tmp2BE)->f1;_LL2A3: _tmp2C1=_tmp2BF.targs;
goto _LL2A2;_LL2A2: _tmp2C0=(void*)_tmp2BF.rgn;goto _LL29E;}else{goto _LL29F;}_LL29F:
goto _LL2A0;_LL29C: Cyc_Tcutil_unify(*topt,res);goto _LL29A;_LL29E:{struct Cyc_List_List*
a=_tmp2B9;for(0;a != 0? _tmp2C1 != 0: 0;(a=a->tl,_tmp2C1=_tmp2C1->tl)){Cyc_Tcutil_unify((
void*)a->hd,(void*)_tmp2C1->hd);}}if(tuf->typs == 0? es == 0: 0){e->topt=({struct Cyc_Core_Opt*
_tmp2C2=_cycalloc(sizeof(*_tmp2C2));_tmp2C2->v=(void*)res;_tmp2C2;});res=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp2C3=_cycalloc(sizeof(*_tmp2C3));_tmp2C3[0]=({
struct Cyc_Absyn_TunionType_struct _tmp2C4;_tmp2C4.tag=2;_tmp2C4.f1=({struct Cyc_Absyn_TunionInfo
_tmp2C5;_tmp2C5.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp2C6=_cycalloc(sizeof(*_tmp2C6));_tmp2C6[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp2C7;_tmp2C7.tag=1;_tmp2C7.f1=({struct Cyc_Absyn_Tuniondecl**_tmp2C8=_cycalloc(
sizeof(*_tmp2C8));_tmp2C8[0]=tud;_tmp2C8;});_tmp2C7;});_tmp2C6;}));_tmp2C5.targs=
_tmp2B9;_tmp2C5.rgn=(void*)_tmp2C0;_tmp2C5;});_tmp2C4;});_tmp2C3;});Cyc_Tcutil_unchecked_cast(
te,e,res);{void*_tmp2C9=res;_npop_handler(0);return _tmp2C9;}}goto _LL29A;_LL2A0:
goto _LL29A;_LL29A:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0? ts != 0: 0;(es=
es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp2BB,(*((struct _tuple8*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)&
t,e);if(! Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct _tmp2CD;
_tmp2CD.tag=0;_tmp2CD.f1=(struct _tagged_arr)(e->topt == 0? _tag_arr("?",sizeof(
unsigned char),2): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{struct Cyc_Std_String_pa_struct _tmp2CC;_tmp2CC.tag=0;
_tmp2CC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp2CB;_tmp2CB.tag=0;_tmp2CB.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp2CA[3]={& _tmp2CB,& _tmp2CC,& _tmp2CD};Cyc_Tcutil_terr(e->loc,_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof(unsigned char),80),_tag_arr(_tmp2CA,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp2D0=({struct Cyc_Std_String_pa_struct _tmp2CF;_tmp2CF.tag=0;
_tmp2CF.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp2CE[1]={&
_tmp2CF};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(unsigned char),45),_tag_arr(_tmp2CE,sizeof(void*),1));}});_npop_handler(0);
return _tmp2D0;}if(ts != 0){void*_tmp2D3=({struct Cyc_Std_String_pa_struct _tmp2D2;
_tmp2D2.tag=0;_tmp2D2.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp2D1[1]={& _tmp2D2};Cyc_Tcexp_expr_err(te,loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(unsigned char),44),_tag_arr(_tmp2D1,sizeof(void*),1));}});_npop_handler(0);
return _tmp2D3;}{void*_tmp2D4=res;_npop_handler(0);return _tmp2D4;}}}};_pop_region(
rgn);}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp2DB=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp2DC;struct Cyc_Absyn_Conref*_tmp2DD;void*_tmp2DE;
struct Cyc_List_List*_tmp2DF;struct Cyc_Absyn_Structdecl**_tmp2E0;struct Cyc_List_List*
_tmp2E1;struct Cyc_List_List*_tmp2E2;_LL2A5: if((int)_tmp2DB == 0){goto _LL2A6;}
else{goto _LL2A7;}_LL2A7: if((unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)== 0: 0){goto
_LL2A8;}else{goto _LL2A9;}_LL2A9: if((unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)== 1:
0){goto _LL2AA;}else{goto _LL2AB;}_LL2AB: if((unsigned int)_tmp2DB > 3?*((int*)
_tmp2DB)== 2: 0){goto _LL2AC;}else{goto _LL2AD;}_LL2AD: if((unsigned int)_tmp2DB > 3?*((
int*)_tmp2DB)== 3: 0){goto _LL2AE;}else{goto _LL2AF;}_LL2AF: if((unsigned int)
_tmp2DB > 3?*((int*)_tmp2DB)== 4: 0){_LL2D7: _tmp2DC=((struct Cyc_Absyn_PointerType_struct*)
_tmp2DB)->f1;_LL2D8: _tmp2DD=_tmp2DC.nullable;goto _LL2B0;}else{goto _LL2B1;}_LL2B1:
if((unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)== 5: 0){goto _LL2B2;}else{goto _LL2B3;}
_LL2B3: if((int)_tmp2DB == 1){goto _LL2B4;}else{goto _LL2B5;}_LL2B5: if((unsigned int)
_tmp2DB > 3?*((int*)_tmp2DB)== 6: 0){goto _LL2B6;}else{goto _LL2B7;}_LL2B7: if((
unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)== 7: 0){_LL2D9: _tmp2DE=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp2DB)->f1;goto _LL2B8;}else{goto _LL2B9;}_LL2B9: if((
unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)== 8: 0){goto _LL2BA;}else{goto _LL2BB;}
_LL2BB: if((unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)== 9: 0){_LL2DA: _tmp2DF=((
struct Cyc_Absyn_TupleType_struct*)_tmp2DB)->f1;goto _LL2BC;}else{goto _LL2BD;}
_LL2BD: if((unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)== 10: 0){_LL2DC: _tmp2E1=((
struct Cyc_Absyn_StructType_struct*)_tmp2DB)->f2;goto _LL2DB;_LL2DB: _tmp2E0=((
struct Cyc_Absyn_StructType_struct*)_tmp2DB)->f3;goto _LL2BE;}else{goto _LL2BF;}
_LL2BF: if((unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)== 11: 0){goto _LL2C0;}else{
goto _LL2C1;}_LL2C1: if((unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)== 12: 0){goto
_LL2C2;}else{goto _LL2C3;}_LL2C3: if((unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)== 
13: 0){goto _LL2C4;}else{goto _LL2C5;}_LL2C5: if((unsigned int)_tmp2DB > 3?*((int*)
_tmp2DB)== 14: 0){_LL2DD: _tmp2E2=((struct Cyc_Absyn_AnonStructType_struct*)_tmp2DB)->f1;
goto _LL2C6;}else{goto _LL2C7;}_LL2C7: if((unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)
== 15: 0){goto _LL2C8;}else{goto _LL2C9;}_LL2C9: if((unsigned int)_tmp2DB > 3?*((int*)
_tmp2DB)== 16: 0){goto _LL2CA;}else{goto _LL2CB;}_LL2CB: if((unsigned int)_tmp2DB > 3?*((
int*)_tmp2DB)== 17: 0){goto _LL2CC;}else{goto _LL2CD;}_LL2CD: if((unsigned int)
_tmp2DB > 3?*((int*)_tmp2DB)== 18: 0){goto _LL2CE;}else{goto _LL2CF;}_LL2CF: if((int)
_tmp2DB == 2){goto _LL2D0;}else{goto _LL2D1;}_LL2D1: if((unsigned int)_tmp2DB > 3?*((
int*)_tmp2DB)== 19: 0){goto _LL2D2;}else{goto _LL2D3;}_LL2D3: if((unsigned int)
_tmp2DB > 3?*((int*)_tmp2DB)== 20: 0){goto _LL2D4;}else{goto _LL2D5;}_LL2D5: if((
unsigned int)_tmp2DB > 3?*((int*)_tmp2DB)== 21: 0){goto _LL2D6;}else{goto _LL2A4;}
_LL2A6: return 1;_LL2A8: goto _LL2AA;_LL2AA: goto _LL2AC;_LL2AC: goto _LL2AE;_LL2AE:
return 0;_LL2B0: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmp2DD);_LL2B2: goto _LL2B4;_LL2B4: goto _LL2B6;_LL2B6: return 1;_LL2B8: return Cyc_Tcexp_zeroable_type(
_tmp2DE);_LL2BA: return 0;_LL2BC: for(0;(unsigned int)_tmp2DF;_tmp2DF=_tmp2DF->tl){
if(! Cyc_Tcexp_zeroable_type((*((struct _tuple8*)_tmp2DF->hd)).f2)){return 0;}}
return 1;_LL2BE: if(_tmp2E0 == 0){return 0;}{struct Cyc_Absyn_Structdecl*_tmp2E3=*
_tmp2E0;if(_tmp2E3->fields == 0){return 0;}{struct _RegionHandle _tmp2E4=_new_region("r");
struct _RegionHandle*r=& _tmp2E4;_push_region(r);{struct Cyc_List_List*_tmp2E5=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp2E3->tvs,_tmp2E1);{struct Cyc_List_List*
fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2E3->fields))->v;
for(0;fs != 0;fs=fs->tl){if(! Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,
_tmp2E5,(void*)((struct Cyc_Absyn_Structfield*)fs->hd)->type))){int _tmp2E6=0;
_npop_handler(0);return _tmp2E6;}}}{int _tmp2E7=1;_npop_handler(0);return _tmp2E7;}};
_pop_region(r);}}_LL2C0: return 1;_LL2C2: return 1;_LL2C4: return 1;_LL2C6: for(0;
_tmp2E2 != 0;_tmp2E2=_tmp2E2->tl){if(! Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Structfield*)
_tmp2E2->hd)->type)){return 0;}}return 1;_LL2C8: return 1;_LL2CA: return 1;_LL2CC:
return 0;_LL2CE: goto _LL2D0;_LL2D0: goto _LL2D2;_LL2D2: goto _LL2D4;_LL2D4: goto _LL2D6;
_LL2D6: return({struct Cyc_Std_String_pa_struct _tmp2E9;_tmp2E9.tag=0;_tmp2E9.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp2E8[1]={& _tmp2E9};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad type `%s' in zeroable type",
sizeof(unsigned char),31),_tag_arr(_tmp2E8,sizeof(void*),1));}});_LL2A4:;}static
void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*loc,
void**topt,void*t){if(Cyc_Tcutil_bits_only(t)? 1:(allow_zero? Cyc_Tcexp_zeroable_type(
t): 0)){return;}if(topt != 0){void*_tmp2EA=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp2EB;void*_tmp2EC;_LL2DF: if((unsigned int)_tmp2EA > 3?*((int*)_tmp2EA)== 4: 0){
_LL2E3: _tmp2EB=((struct Cyc_Absyn_PointerType_struct*)_tmp2EA)->f1;_LL2E4: _tmp2EC=(
void*)_tmp2EB.elt_typ;goto _LL2E0;}else{goto _LL2E1;}_LL2E1: goto _LL2E2;_LL2E0: Cyc_Tcutil_unify(
_tmp2EC,t);if(Cyc_Tcutil_bits_only(t)? 1:(allow_zero? Cyc_Tcexp_zeroable_type(t): 0)){
return;}goto _LL2DE;_LL2E2: goto _LL2DE;_LL2DE:;}({struct Cyc_Std_String_pa_struct
_tmp2EF;_tmp2EF.tag=0;_tmp2EF.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp2EE;_tmp2EE.tag=0;_tmp2EE.f1=(struct
_tagged_arr)(allow_zero? _tag_arr("calloc",sizeof(unsigned char),7): _tag_arr("malloc",
sizeof(unsigned char),7));{void*_tmp2ED[2]={& _tmp2EE,& _tmp2EF};Cyc_Tcutil_terr(
loc,_tag_arr("%s cannot be used with type %s\n\t(type needs initialization)",
sizeof(unsigned char),60),_tag_arr(_tmp2ED,sizeof(void*),2));}}});}static void*
Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*ropt,void***t,struct Cyc_Absyn_Exp**e,int is_calloc,int*
is_fat){void*rgn=(void*)2;if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp2F5;_tmp2F5.tag=17;_tmp2F5.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp2F6=_cycalloc(sizeof(*_tmp2F6));_tmp2F6->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp2F6;}));_tmp2F5;});_tmp2F4;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(ropt));void*_tmp2F0=Cyc_Tcutil_compress(handle_type);void*_tmp2F1;
_LL2E6: if((unsigned int)_tmp2F0 > 3?*((int*)_tmp2F0)== 17: 0){_LL2EA: _tmp2F1=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp2F0)->f1;goto _LL2E7;}else{goto _LL2E8;}
_LL2E8: goto _LL2E9;_LL2E7: rgn=_tmp2F1;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL2E5;_LL2E9:({struct Cyc_Std_String_pa_struct _tmp2F3;_tmp2F3.tag=0;_tmp2F3.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type);{void*_tmp2F2[1]={& _tmp2F3};
Cyc_Tcutil_terr(ropt->loc,_tag_arr("expecting region_t type but found %s",
sizeof(unsigned char),37),_tag_arr(_tmp2F2,sizeof(void*),1));}});goto _LL2E5;
_LL2E5:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_t,*e);{void*elt_type;struct
Cyc_Absyn_Exp*num_elts;int one_elt;if(is_calloc){if(*t == 0){({void*_tmp2F7[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("calloc with empty type",
sizeof(unsigned char),23),_tag_arr(_tmp2F7,sizeof(void*),0));});}elt_type=*((
void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp2F8=(void*)(*e)->r;void*_tmp2F9;struct Cyc_List_List*
_tmp2FA;struct Cyc_List_List _tmp2FB;struct Cyc_List_List*_tmp2FC;struct Cyc_List_List
_tmp2FD;struct Cyc_List_List*_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;struct Cyc_Absyn_Exp*
_tmp300;void*_tmp301;_LL2EC: if(*((int*)_tmp2F8)== 16){_LL2F2: _tmp2F9=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp2F8)->f1;goto _LL2ED;}else{goto _LL2EE;}
_LL2EE: if(*((int*)_tmp2F8)== 3){_LL2F8: _tmp301=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2F8)->f1;if((int)_tmp301 == 1){goto _LL2F3;}else{goto _LL2F0;}_LL2F3: _tmp2FA=((
struct Cyc_Absyn_Primop_e_struct*)_tmp2F8)->f2;if(_tmp2FA == 0){goto _LL2F0;}else{
_tmp2FB=*_tmp2FA;_LL2F7: _tmp300=(struct Cyc_Absyn_Exp*)_tmp2FB.hd;goto _LL2F4;
_LL2F4: _tmp2FC=_tmp2FB.tl;if(_tmp2FC == 0){goto _LL2F0;}else{_tmp2FD=*_tmp2FC;
_LL2F6: _tmp2FF=(struct Cyc_Absyn_Exp*)_tmp2FD.hd;goto _LL2F5;_LL2F5: _tmp2FE=
_tmp2FD.tl;if(_tmp2FE == 0){goto _LL2EF;}else{goto _LL2F0;}}}}else{goto _LL2F0;}
_LL2F0: goto _LL2F1;_LL2ED: elt_type=_tmp2F9;*t=(void**)({void**_tmp302=_cycalloc(
sizeof(*_tmp302));_tmp302[0]=elt_type;_tmp302;});num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_t,num_elts);one_elt=1;goto _LL2EB;
_LL2EF:{struct _tuple0 _tmp304=({struct _tuple0 _tmp303;_tmp303.f1=(void*)_tmp300->r;
_tmp303.f2=(void*)_tmp2FF->r;_tmp303;});void*_tmp305;void*_tmp306;void*_tmp307;
void*_tmp308;_LL2FA: _LL300: _tmp305=_tmp304.f1;if(*((int*)_tmp305)== 16){_LL301:
_tmp306=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp305)->f1;goto _LL2FB;}
else{goto _LL2FC;}_LL2FC: _LL302: _tmp307=_tmp304.f2;if(*((int*)_tmp307)== 16){
_LL303: _tmp308=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp307)->f1;goto
_LL2FD;}else{goto _LL2FE;}_LL2FE: goto _LL2FF;_LL2FB: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp306);elt_type=_tmp306;*t=(void**)({void**_tmp309=_cycalloc(sizeof(*
_tmp309));_tmp309[0]=elt_type;_tmp309;});num_elts=_tmp2FF;one_elt=0;goto _LL2F9;
_LL2FD: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp308);elt_type=_tmp308;*t=(void**)({
void**_tmp30A=_cycalloc(sizeof(*_tmp30A));_tmp30A[0]=elt_type;_tmp30A;});
num_elts=_tmp300;one_elt=0;goto _LL2F9;_LL2FF: goto No_sizeof;_LL2F9:;}goto _LL2EB;
_LL2F1: No_sizeof: elt_type=Cyc_Absyn_uchar_t;*t=(void**)({void**_tmp30B=_cycalloc(
sizeof(*_tmp30B));_tmp30B[0]=elt_type;_tmp30B;});num_elts=*e;one_elt=0;goto
_LL2EB;_LL2EB:;}*e=num_elts;*is_fat=! one_elt;{void*_tmp30C=elt_type;struct Cyc_Absyn_TunionFieldInfo
_tmp30D;void*_tmp30E;struct Cyc_Absyn_Tunionfield*_tmp30F;_LL305: if((unsigned int)
_tmp30C > 3?*((int*)_tmp30C)== 3: 0){_LL309: _tmp30D=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp30C)->f1;_LL30A: _tmp30E=(void*)_tmp30D.field_info;if(*((int*)_tmp30E)== 1){
_LL30B: _tmp30F=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp30E)->f2;goto
_LL306;}else{goto _LL307;}}else{goto _LL307;}_LL307: goto _LL308;_LL306: if(_tmp30F->tvs
!= 0){({void*_tmp310[0]={};Cyc_Tcutil_terr(loc,_tag_arr("malloc with existential types not yet implemented",
sizeof(unsigned char),50),_tag_arr(_tmp310,sizeof(void*),0));});}goto _LL304;
_LL308: goto _LL304;_LL304:;}{void*(*_tmp311)(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq)=Cyc_Absyn_at_typ;if(topt != 0){void*_tmp312=Cyc_Tcutil_compress(*topt);struct
Cyc_Absyn_PtrInfo _tmp313;struct Cyc_Absyn_Conref*_tmp314;_LL30D: if((unsigned int)
_tmp312 > 3?*((int*)_tmp312)== 4: 0){_LL311: _tmp313=((struct Cyc_Absyn_PointerType_struct*)
_tmp312)->f1;_LL312: _tmp314=_tmp313.nullable;goto _LL30E;}else{goto _LL30F;}_LL30F:
goto _LL310;_LL30E: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp314)){_tmp311=Cyc_Absyn_star_typ;}goto _LL30C;_LL310: goto _LL30C;_LL30C:;}if(!
one_elt){_tmp311=Cyc_Absyn_tagged_typ;}return _tmp311(elt_type,rgn,Cyc_Absyn_empty_tqual());}}}
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
te,s),s,1);Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));while(1){
void*_tmp315=(void*)s->r;struct Cyc_Absyn_Exp*_tmp316;struct Cyc_Absyn_Stmt*
_tmp317;struct Cyc_Absyn_Stmt*_tmp318;struct Cyc_Absyn_Stmt*_tmp319;struct Cyc_Absyn_Decl*
_tmp31A;_LL314: if((unsigned int)_tmp315 > 1?*((int*)_tmp315)== 0: 0){_LL31C:
_tmp316=((struct Cyc_Absyn_Exp_s_struct*)_tmp315)->f1;goto _LL315;}else{goto _LL316;}
_LL316: if((unsigned int)_tmp315 > 1?*((int*)_tmp315)== 1: 0){_LL31E: _tmp318=((
struct Cyc_Absyn_Seq_s_struct*)_tmp315)->f1;goto _LL31D;_LL31D: _tmp317=((struct Cyc_Absyn_Seq_s_struct*)
_tmp315)->f2;goto _LL317;}else{goto _LL318;}_LL318: if((unsigned int)_tmp315 > 1?*((
int*)_tmp315)== 12: 0){_LL320: _tmp31A=((struct Cyc_Absyn_Decl_s_struct*)_tmp315)->f1;
goto _LL31F;_LL31F: _tmp319=((struct Cyc_Absyn_Decl_s_struct*)_tmp315)->f2;goto
_LL319;}else{goto _LL31A;}_LL31A: goto _LL31B;_LL315: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp316->topt))->v;_LL317: s=_tmp317;continue;_LL319: s=_tmp319;
continue;_LL31B: return({void*_tmp31B[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("statement expression must end with expression",
sizeof(unsigned char),46),_tag_arr(_tmp31B,sizeof(void*),0));});_LL313:;}}static
void*Cyc_Tcexp_tcCodegen(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Fndecl*fd){return({void*_tmp31C[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("tcCodegen",sizeof(unsigned char),10),_tag_arr(_tmp31C,sizeof(
void*),0));});}static void*Cyc_Tcexp_tcFill(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){return({void*_tmp31D[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("tcFill",sizeof(unsigned char),7),_tag_arr(_tmp31D,sizeof(void*),
0));});}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp323;_tmp323.tag=17;_tmp323.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324->v=
Cyc_Tcenv_lookup_type_vars(te);_tmp324;}));_tmp323;});_tmp322;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(rgn_handle));void*_tmp31E=Cyc_Tcutil_compress(handle_type);void*
_tmp31F;_LL322: if((unsigned int)_tmp31E > 3?*((int*)_tmp31E)== 17: 0){_LL326:
_tmp31F=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp31E)->f1;goto _LL323;}
else{goto _LL324;}_LL324: goto _LL325;_LL323: rgn=_tmp31F;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL321;_LL325:({struct Cyc_Std_String_pa_struct _tmp321;_tmp321.tag=
0;_tmp321.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(handle_type);{void*
_tmp320[1]={& _tmp321};Cyc_Tcutil_terr(rgn_handle->loc,_tag_arr("expecting region_t type but found %s",
sizeof(unsigned char),37),_tag_arr(_tmp320,sizeof(void*),1));}});goto _LL321;
_LL321:;}{void*_tmp325=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp326;struct Cyc_Absyn_Exp*
_tmp327;struct Cyc_Absyn_Vardecl*_tmp328;struct Cyc_List_List*_tmp329;struct Cyc_Core_Opt*
_tmp32A;struct Cyc_List_List*_tmp32B;void*_tmp32C;struct _tagged_arr _tmp32D;_LL328:
if(*((int*)_tmp325)== 27){_LL334: _tmp328=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp325)->f1;goto _LL333;_LL333: _tmp327=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp325)->f2;goto _LL332;_LL332: _tmp326=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp325)->f3;goto _LL329;}else{goto _LL32A;}_LL32A: if(*((int*)_tmp325)== 34){
_LL336: _tmp32A=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp325)->f1;goto _LL335;
_LL335: _tmp329=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp325)->f2;goto _LL32B;}
else{goto _LL32C;}_LL32C: if(*((int*)_tmp325)== 26){_LL337: _tmp32B=((struct Cyc_Absyn_Array_e_struct*)
_tmp325)->f1;goto _LL32D;}else{goto _LL32E;}_LL32E: if(*((int*)_tmp325)== 0){_LL338:
_tmp32C=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp325)->f1;if((unsigned int)
_tmp32C > 1?*((int*)_tmp32C)== 5: 0){_LL339: _tmp32D=((struct Cyc_Absyn_String_c_struct*)
_tmp32C)->f1;goto _LL32F;}else{goto _LL330;}}else{goto _LL330;}_LL330: goto _LL331;
_LL329: {void*_tmp32E=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*_tmp32F=Cyc_Tcutil_compress(
_tmp32E);struct Cyc_Absyn_Tqual _tmp330;void*_tmp331;_LL33B: if((unsigned int)
_tmp32F > 3?*((int*)_tmp32F)== 7: 0){_LL340: _tmp331=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp32F)->f1;goto _LL33F;_LL33F: _tmp330=((struct Cyc_Absyn_ArrayType_struct*)
_tmp32F)->f2;goto _LL33C;}else{goto _LL33D;}_LL33D: goto _LL33E;_LL33C: {void*b=Cyc_Tcutil_is_const_exp(
te,_tmp327)?(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp336=_cycalloc(sizeof(*
_tmp336));_tmp336[0]=({struct Cyc_Absyn_Upper_b_struct _tmp337;_tmp337.tag=0;
_tmp337.f1=_tmp327;_tmp337;});_tmp336;}):(void*)0;void*res_typ=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333[0]=({
struct Cyc_Absyn_PointerType_struct _tmp334;_tmp334.tag=4;_tmp334.f1=({struct Cyc_Absyn_PtrInfo
_tmp335;_tmp335.elt_typ=(void*)_tmp331;_tmp335.rgn_typ=(void*)rgn;_tmp335.nullable=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp335.tq=_tmp330;
_tmp335.bounds=Cyc_Absyn_new_conref(b);_tmp335;});_tmp334;});_tmp333;});if(topt
!= 0){if(! Cyc_Tcutil_unify(*topt,res_typ)? Cyc_Tcutil_silent_castable(te,loc,
res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp332=_cycalloc(sizeof(*_tmp332));
_tmp332->v=(void*)res_typ;_tmp332;});Cyc_Tcutil_unchecked_cast(te,e,*topt);
res_typ=*topt;}}return res_typ;}_LL33E: return({void*_tmp338[0]={};Cyc_Tcutil_impos(
_tag_arr("tcNew: comprehension returned non-array type",sizeof(unsigned char),45),
_tag_arr(_tmp338,sizeof(void*),0));});_LL33A:;}_LL32B:(void*)(e1->r=(void*)((
void*)({struct Cyc_Absyn_Array_e_struct*_tmp339=_cycalloc(sizeof(*_tmp339));
_tmp339[0]=({struct Cyc_Absyn_Array_e_struct _tmp33A;_tmp33A.tag=26;_tmp33A.f1=
_tmp329;_tmp33A;});_tmp339;})));_tmp32B=_tmp329;goto _LL32D;_LL32D: {void**
elt_typ_opt=0;if(topt != 0){void*_tmp33B=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp33C;struct Cyc_Absyn_Tqual _tmp33D;void*_tmp33E;void**_tmp33F;_LL342: if((
unsigned int)_tmp33B > 3?*((int*)_tmp33B)== 4: 0){_LL346: _tmp33C=((struct Cyc_Absyn_PointerType_struct*)
_tmp33B)->f1;_LL348: _tmp33E=(void*)_tmp33C.elt_typ;_tmp33F=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp33B)->f1).elt_typ;goto _LL347;_LL347: _tmp33D=_tmp33C.tq;goto _LL343;}else{goto
_LL344;}_LL344: goto _LL345;_LL343: elt_typ_opt=(void**)_tmp33F;goto _LL341;_LL345:
goto _LL341;_LL341:;}{void*_tmp340=Cyc_Tcexp_tcExpNoPromote(te,elt_typ_opt,e1);
void*res_typ;{void*_tmp341=Cyc_Tcutil_compress(_tmp340);struct Cyc_Absyn_Exp*
_tmp342;struct Cyc_Absyn_Tqual _tmp343;void*_tmp344;_LL34A: if((unsigned int)
_tmp341 > 3?*((int*)_tmp341)== 7: 0){_LL350: _tmp344=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp341)->f1;goto _LL34F;_LL34F: _tmp343=((struct Cyc_Absyn_ArrayType_struct*)
_tmp341)->f2;goto _LL34E;_LL34E: _tmp342=((struct Cyc_Absyn_ArrayType_struct*)
_tmp341)->f3;goto _LL34B;}else{goto _LL34C;}_LL34C: goto _LL34D;_LL34B: res_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345[0]=({
struct Cyc_Absyn_PointerType_struct _tmp346;_tmp346.tag=4;_tmp346.f1=({struct Cyc_Absyn_PtrInfo
_tmp347;_tmp347.elt_typ=(void*)_tmp344;_tmp347.rgn_typ=(void*)rgn;_tmp347.nullable=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp347.tq=_tmp343;
_tmp347.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp349;_tmp349.tag=0;_tmp349.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp342);
_tmp349;});_tmp348;}));_tmp347;});_tmp346;});_tmp345;});if(topt != 0){if(! Cyc_Tcutil_unify(*
topt,res_typ)? Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt): 0){e->topt=({
struct Cyc_Core_Opt*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A->v=(void*)res_typ;
_tmp34A;});Cyc_Tcutil_unchecked_cast(te,e,*topt);res_typ=*topt;}}goto _LL349;
_LL34D: return({void*_tmp34B[0]={};Cyc_Tcutil_impos(_tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof(unsigned char),50),_tag_arr(_tmp34B,sizeof(void*),0));});_LL349:;}return
res_typ;}}_LL32F: {void*_tmp34C=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,rgn,Cyc_Absyn_const_tqual(),(
void*)0);void*_tmp34D=Cyc_Tcexp_tcExp(te,(void**)& _tmp34C,e1);return Cyc_Absyn_atb_typ(
_tmp34D,rgn,Cyc_Absyn_empty_tqual(),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp34F;_tmp34F.tag=0;_tmp34F.f1=Cyc_Absyn_uint_exp(1,0);_tmp34F;});_tmp34E;}));}
_LL331: {void**topt2=0;if(topt != 0){void*_tmp350=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp351;struct Cyc_Absyn_Conref*_tmp352;void*_tmp353;void**
_tmp354;_LL352: if((unsigned int)_tmp350 > 3?*((int*)_tmp350)== 4: 0){_LL358:
_tmp351=((struct Cyc_Absyn_PointerType_struct*)_tmp350)->f1;_LL35A: _tmp353=(void*)
_tmp351.elt_typ;_tmp354=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp350)->f1).elt_typ;
goto _LL359;_LL359: _tmp352=_tmp351.nullable;goto _LL353;}else{goto _LL354;}_LL354:
if((unsigned int)_tmp350 > 3?*((int*)_tmp350)== 2: 0){goto _LL355;}else{goto _LL356;}
_LL356: goto _LL357;_LL353: topt2=(void**)_tmp354;goto _LL351;_LL355: topt2=({void**
_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355[0]=*topt;_tmp355;});goto _LL351;
_LL357: goto _LL351;_LL351:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp357=_cycalloc(sizeof(*_tmp357));
_tmp357[0]=({struct Cyc_Absyn_PointerType_struct _tmp358;_tmp358.tag=4;_tmp358.f1=({
struct Cyc_Absyn_PtrInfo _tmp359;_tmp359.elt_typ=(void*)telt;_tmp359.rgn_typ=(void*)
rgn;_tmp359.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp359.tq=Cyc_Absyn_empty_tqual();_tmp359.bounds=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp35B;_tmp35B.tag=0;_tmp35B.f1=Cyc_Absyn_uint_exp(
1,0);_tmp35B;});_tmp35A;}));_tmp359;});_tmp358;});_tmp357;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ)? Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt):
0){e->topt=({struct Cyc_Core_Opt*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356->v=(
void*)res_typ;_tmp356;});Cyc_Tcutil_unchecked_cast(te,e,*topt);res_typ=*topt;}}
return res_typ;}}_LL327:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp35C=t;struct Cyc_Absyn_Exp*_tmp35D;struct Cyc_Absyn_Tqual
_tmp35E;void*_tmp35F;_LL35C: if((unsigned int)_tmp35C > 3?*((int*)_tmp35C)== 7: 0){
_LL362: _tmp35F=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp35C)->f1;goto
_LL361;_LL361: _tmp35E=((struct Cyc_Absyn_ArrayType_struct*)_tmp35C)->f2;goto
_LL360;_LL360: _tmp35D=((struct Cyc_Absyn_ArrayType_struct*)_tmp35C)->f3;goto
_LL35D;}else{goto _LL35E;}_LL35E: goto _LL35F;_LL35D: {void*_tmp361;struct _tuple6
_tmp360=Cyc_Tcutil_addressof_props(te,e);_LL364: _tmp361=_tmp360.f2;goto _LL363;
_LL363: {void*_tmp362=_tmp35D == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp363=_cycalloc(sizeof(*_tmp363));_tmp363[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp364;_tmp364.tag=0;_tmp364.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp35D);
_tmp364;});_tmp363;});t=Cyc_Absyn_atb_typ(_tmp35F,_tmp361,_tmp35E,_tmp362);(void*)(((
struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);return t;}}_LL35F: return t;
_LL35B:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,
struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);void*_tmp365=(
void*)e->r;void*_tmp366;_LL366: if(*((int*)_tmp365)== 26){goto _LL367;}else{goto
_LL368;}_LL368: if(*((int*)_tmp365)== 27){goto _LL369;}else{goto _LL36A;}_LL36A: if(*((
int*)_tmp365)== 0){_LL36E: _tmp366=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp365)->f1;if((unsigned int)_tmp366 > 1?*((int*)_tmp366)== 5: 0){goto _LL36B;}
else{goto _LL36C;}}else{goto _LL36C;}_LL36C: goto _LL36D;_LL367: goto _LL369;_LL369:
goto _LL36B;_LL36B: return t;_LL36D: t=Cyc_Tcutil_compress(t);{void*_tmp367=t;struct
Cyc_Absyn_Exp*_tmp368;struct Cyc_Absyn_Tqual _tmp369;void*_tmp36A;_LL370: if((
unsigned int)_tmp367 > 3?*((int*)_tmp367)== 7: 0){_LL376: _tmp36A=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp367)->f1;goto _LL375;_LL375: _tmp369=((struct Cyc_Absyn_ArrayType_struct*)
_tmp367)->f2;goto _LL374;_LL374: _tmp368=((struct Cyc_Absyn_ArrayType_struct*)
_tmp367)->f3;goto _LL371;}else{goto _LL372;}_LL372: goto _LL373;_LL371: {void*
_tmp36C;struct _tuple6 _tmp36B=Cyc_Tcutil_addressof_props(te,e);_LL378: _tmp36C=
_tmp36B.f2;goto _LL377;_LL377: {void*b=_tmp368 == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp36E;_tmp36E.tag=0;_tmp36E.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp368);
_tmp36E;});_tmp36D;});t=Cyc_Absyn_atb_typ(_tmp36A,_tmp36C,_tmp369,b);Cyc_Tcutil_unchecked_cast(
te,e,t);return t;}}_LL373: return t;_LL36F:;}_LL365:;}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp36F=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp370;_LL37A: if(*((int*)_tmp36F)== 11){_LL37E: _tmp370=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp36F)->f1;goto _LL37B;}else{goto _LL37C;}
_LL37C: goto _LL37D;_LL37B: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp370);(void*)(((struct
Cyc_Core_Opt*)_check_null(_tmp370->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp370->topt))->v));e->topt=_tmp370->topt;
goto _LL379;_LL37D: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{void*_tmp371=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp372;struct Cyc_Absyn_Conref*
_tmp373;struct Cyc_Absyn_Tqual _tmp374;struct Cyc_Absyn_Conref*_tmp375;void*_tmp376;
void*_tmp377;_LL380: if((unsigned int)_tmp371 > 3?*((int*)_tmp371)== 4: 0){_LL384:
_tmp372=((struct Cyc_Absyn_PointerType_struct*)_tmp371)->f1;_LL389: _tmp377=(void*)
_tmp372.elt_typ;goto _LL388;_LL388: _tmp376=(void*)_tmp372.rgn_typ;goto _LL387;
_LL387: _tmp375=_tmp372.nullable;goto _LL386;_LL386: _tmp374=_tmp372.tq;goto _LL385;
_LL385: _tmp373=_tmp372.bounds;goto _LL381;}else{goto _LL382;}_LL382: goto _LL383;
_LL381:{void*_tmp378=Cyc_Tcutil_compress(_tmp377);struct Cyc_Absyn_FnInfo _tmp379;
struct Cyc_List_List*_tmp37A;struct Cyc_List_List*_tmp37B;struct Cyc_Absyn_VarargInfo*
_tmp37C;int _tmp37D;struct Cyc_List_List*_tmp37E;void*_tmp37F;struct Cyc_Core_Opt*
_tmp380;struct Cyc_List_List*_tmp381;_LL38B: if((unsigned int)_tmp378 > 3?*((int*)
_tmp378)== 8: 0){_LL38F: _tmp379=((struct Cyc_Absyn_FnType_struct*)_tmp378)->f1;
_LL397: _tmp381=_tmp379.tvars;goto _LL396;_LL396: _tmp380=_tmp379.effect;goto _LL395;
_LL395: _tmp37F=(void*)_tmp379.ret_typ;goto _LL394;_LL394: _tmp37E=_tmp379.args;
goto _LL393;_LL393: _tmp37D=_tmp379.c_varargs;goto _LL392;_LL392: _tmp37C=_tmp379.cyc_varargs;
goto _LL391;_LL391: _tmp37B=_tmp379.rgn_po;goto _LL390;_LL390: _tmp37A=_tmp379.attributes;
goto _LL38C;}else{goto _LL38D;}_LL38D: goto _LL38E;_LL38C: if(_tmp381 != 0){struct
_RegionHandle _tmp382=_new_region("rgn");struct _RegionHandle*rgn=& _tmp382;
_push_region(rgn);{struct _tuple4 _tmp383=({struct _tuple4 _tmp38D;_tmp38D.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp38D.f2=rgn;_tmp38D;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp383,_tmp381);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A[0]=({struct Cyc_Absyn_FnType_struct
_tmp38B;_tmp38B.tag=8;_tmp38B.f1=({struct Cyc_Absyn_FnInfo _tmp38C;_tmp38C.tvars=0;
_tmp38C.effect=_tmp380;_tmp38C.ret_typ=(void*)_tmp37F;_tmp38C.args=_tmp37E;
_tmp38C.c_varargs=_tmp37D;_tmp38C.cyc_varargs=_tmp37C;_tmp38C.rgn_po=_tmp37B;
_tmp38C.attributes=_tmp37A;_tmp38C;});_tmp38B;});_tmp38A;}));void*new_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387[0]=({
struct Cyc_Absyn_PointerType_struct _tmp388;_tmp388.tag=4;_tmp388.f1=({struct Cyc_Absyn_PtrInfo
_tmp389;_tmp389.elt_typ=(void*)ftyp;_tmp389.rgn_typ=(void*)_tmp376;_tmp389.nullable=
_tmp375;_tmp389.tq=_tmp374;_tmp389.bounds=_tmp373;_tmp389;});_tmp388;});_tmp387;});
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Instantiate_e_struct*_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384[
0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp385;_tmp385.tag=12;_tmp385.f1=inner;
_tmp385.f2=ts;_tmp385;});_tmp384;})));e->topt=({struct Cyc_Core_Opt*_tmp386=
_cycalloc(sizeof(*_tmp386));_tmp386->v=(void*)new_typ;_tmp386;});};_pop_region(
rgn);}goto _LL38A;_LL38E: goto _LL38A;_LL38A:;}goto _LL37F;_LL383: goto _LL37F;_LL37F:;}
goto _LL379;_LL379:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp38E=(void*)e->r;struct
Cyc_Absyn_Exp*_tmp38F;struct _tuple1*_tmp390;struct Cyc_List_List*_tmp391;struct
Cyc_Absyn_Exp*_tmp392;struct Cyc_List_List*_tmp393;struct Cyc_Core_Opt*_tmp394;
void*_tmp395;void*_tmp396;struct _tuple1*_tmp397;struct Cyc_List_List*_tmp398;void*
_tmp399;void*_tmp39A;struct Cyc_Absyn_Exp*_tmp39B;struct Cyc_Absyn_Exp*_tmp39C;
struct Cyc_Core_Opt*_tmp39D;struct Cyc_Absyn_Exp*_tmp39E;struct Cyc_Absyn_Exp*
_tmp39F;struct Cyc_Absyn_Exp*_tmp3A0;struct Cyc_Absyn_Exp*_tmp3A1;struct Cyc_Absyn_Exp*
_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A3;struct Cyc_Absyn_VarargCallInfo*_tmp3A4;
struct Cyc_Absyn_VarargCallInfo**_tmp3A5;struct Cyc_List_List*_tmp3A6;struct Cyc_Absyn_Exp*
_tmp3A7;struct Cyc_Absyn_Exp*_tmp3A8;struct Cyc_List_List*_tmp3A9;struct Cyc_Absyn_Exp*
_tmp3AA;struct Cyc_Absyn_Exp*_tmp3AB;void*_tmp3AC;struct Cyc_Absyn_Exp*_tmp3AD;
struct Cyc_Absyn_Exp*_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_Absyn_Exp*
_tmp3B0;void*_tmp3B1;void*_tmp3B2;void*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B4;
struct _tagged_arr*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B6;struct _tagged_arr*_tmp3B7;
struct Cyc_Absyn_Exp*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Exp*
_tmp3BA;struct Cyc_List_List*_tmp3BB;struct Cyc_List_List*_tmp3BC;struct _tuple2*
_tmp3BD;struct Cyc_List_List*_tmp3BE;struct Cyc_Absyn_Stmt*_tmp3BF;struct Cyc_Absyn_Fndecl*
_tmp3C0;struct Cyc_Absyn_Exp*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Exp*
_tmp3C3;struct Cyc_Absyn_Vardecl*_tmp3C4;struct Cyc_Absyn_Structdecl*_tmp3C5;
struct Cyc_Absyn_Structdecl**_tmp3C6;struct Cyc_List_List*_tmp3C7;struct Cyc_Core_Opt*
_tmp3C8;struct Cyc_Core_Opt**_tmp3C9;struct _tuple1*_tmp3CA;struct _tuple1**_tmp3CB;
struct Cyc_List_List*_tmp3CC;void*_tmp3CD;struct Cyc_Absyn_Tunionfield*_tmp3CE;
struct Cyc_Absyn_Tuniondecl*_tmp3CF;struct Cyc_List_List*_tmp3D0;struct Cyc_Core_Opt*
_tmp3D1;struct Cyc_Core_Opt**_tmp3D2;struct Cyc_Core_Opt*_tmp3D3;struct Cyc_Core_Opt**
_tmp3D4;struct Cyc_Absyn_Enumfield*_tmp3D5;struct Cyc_Absyn_Enumdecl*_tmp3D6;
struct _tuple1*_tmp3D7;struct _tuple1**_tmp3D8;struct Cyc_Absyn_Enumfield*_tmp3D9;
void*_tmp3DA;struct _tuple1*_tmp3DB;struct _tuple1**_tmp3DC;struct Cyc_Absyn_MallocInfo
_tmp3DD;int _tmp3DE;int*_tmp3DF;struct Cyc_Absyn_Exp*_tmp3E0;struct Cyc_Absyn_Exp**
_tmp3E1;void**_tmp3E2;void***_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E4;int _tmp3E5;
_LL399: if(*((int*)_tmp38E)== 11){_LL3E5: _tmp38F=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp38E)->f1;goto _LL39A;}else{goto _LL39B;}_LL39B: if(*((int*)_tmp38E)== 2){_LL3E6:
_tmp390=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp38E)->f1;goto _LL39C;}else{goto
_LL39D;}_LL39D: if(*((int*)_tmp38E)== 8){_LL3E8: _tmp392=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp38E)->f1;goto _LL3E7;_LL3E7: _tmp391=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp38E)->f2;goto _LL39E;}else{goto _LL39F;}_LL39F: if(*((int*)_tmp38E)== 34){
_LL3EA: _tmp394=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp38E)->f1;goto _LL3E9;
_LL3E9: _tmp393=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp38E)->f2;goto _LL3A0;}
else{goto _LL3A1;}_LL3A1: if(*((int*)_tmp38E)== 0){_LL3EB: _tmp395=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmp38E)->f1;goto _LL3A2;}else{goto _LL3A3;}_LL3A3: if(*((
int*)_tmp38E)== 1){_LL3ED: _tmp397=((struct Cyc_Absyn_Var_e_struct*)_tmp38E)->f1;
goto _LL3EC;_LL3EC: _tmp396=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp38E)->f2;
goto _LL3A4;}else{goto _LL3A5;}_LL3A5: if(*((int*)_tmp38E)== 3){_LL3EF: _tmp399=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp38E)->f1;goto _LL3EE;_LL3EE: _tmp398=((
struct Cyc_Absyn_Primop_e_struct*)_tmp38E)->f2;goto _LL3A6;}else{goto _LL3A7;}
_LL3A7: if(*((int*)_tmp38E)== 5){_LL3F1: _tmp39B=((struct Cyc_Absyn_Increment_e_struct*)
_tmp38E)->f1;goto _LL3F0;_LL3F0: _tmp39A=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp38E)->f2;goto _LL3A8;}else{goto _LL3A9;}_LL3A9: if(*((int*)_tmp38E)== 4){_LL3F4:
_tmp39E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp38E)->f1;goto _LL3F3;_LL3F3:
_tmp39D=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp38E)->f2;goto _LL3F2;_LL3F2:
_tmp39C=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp38E)->f3;goto _LL3AA;}else{goto
_LL3AB;}_LL3AB: if(*((int*)_tmp38E)== 6){_LL3F7: _tmp3A1=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp38E)->f1;goto _LL3F6;_LL3F6: _tmp3A0=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp38E)->f2;goto _LL3F5;_LL3F5: _tmp39F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp38E)->f3;goto _LL3AC;}else{goto _LL3AD;}_LL3AD: if(*((int*)_tmp38E)== 7){_LL3F9:
_tmp3A3=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp38E)->f1;goto _LL3F8;_LL3F8:
_tmp3A2=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp38E)->f2;goto _LL3AE;}else{goto
_LL3AF;}_LL3AF: if(*((int*)_tmp38E)== 9){_LL3FC: _tmp3A7=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp38E)->f1;goto _LL3FB;_LL3FB: _tmp3A6=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp38E)->f2;goto _LL3FA;_LL3FA: _tmp3A4=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp38E)->f3;_tmp3A5=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp38E)->f3;goto _LL3B0;}else{goto _LL3B1;}_LL3B1: if(*((int*)_tmp38E)== 10){
_LL3FD: _tmp3A8=((struct Cyc_Absyn_Throw_e_struct*)_tmp38E)->f1;goto _LL3B2;}else{
goto _LL3B3;}_LL3B3: if(*((int*)_tmp38E)== 12){_LL3FF: _tmp3AA=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp38E)->f1;goto _LL3FE;_LL3FE: _tmp3A9=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp38E)->f2;goto _LL3B4;}else{goto _LL3B5;}_LL3B5: if(*((int*)_tmp38E)== 13){
_LL401: _tmp3AC=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp38E)->f1;goto _LL400;
_LL400: _tmp3AB=((struct Cyc_Absyn_Cast_e_struct*)_tmp38E)->f2;goto _LL3B6;}else{
goto _LL3B7;}_LL3B7: if(*((int*)_tmp38E)== 14){_LL402: _tmp3AD=((struct Cyc_Absyn_Address_e_struct*)
_tmp38E)->f1;goto _LL3B8;}else{goto _LL3B9;}_LL3B9: if(*((int*)_tmp38E)== 15){
_LL404: _tmp3AF=((struct Cyc_Absyn_New_e_struct*)_tmp38E)->f1;goto _LL403;_LL403:
_tmp3AE=((struct Cyc_Absyn_New_e_struct*)_tmp38E)->f2;goto _LL3BA;}else{goto _LL3BB;}
_LL3BB: if(*((int*)_tmp38E)== 17){_LL405: _tmp3B0=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp38E)->f1;goto _LL3BC;}else{goto _LL3BD;}_LL3BD: if(*((int*)_tmp38E)== 16){
_LL406: _tmp3B1=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp38E)->f1;goto
_LL3BE;}else{goto _LL3BF;}_LL3BF: if(*((int*)_tmp38E)== 18){_LL408: _tmp3B3=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp38E)->f1;goto _LL407;_LL407: _tmp3B2=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp38E)->f2;goto _LL3C0;}else{goto _LL3C1;}
_LL3C1: if(*((int*)_tmp38E)== 19){goto _LL3C2;}else{goto _LL3C3;}_LL3C3: if(*((int*)
_tmp38E)== 20){_LL409: _tmp3B4=((struct Cyc_Absyn_Deref_e_struct*)_tmp38E)->f1;
goto _LL3C4;}else{goto _LL3C5;}_LL3C5: if(*((int*)_tmp38E)== 21){_LL40B: _tmp3B6=((
struct Cyc_Absyn_StructMember_e_struct*)_tmp38E)->f1;goto _LL40A;_LL40A: _tmp3B5=((
struct Cyc_Absyn_StructMember_e_struct*)_tmp38E)->f2;goto _LL3C6;}else{goto _LL3C7;}
_LL3C7: if(*((int*)_tmp38E)== 22){_LL40D: _tmp3B8=((struct Cyc_Absyn_StructArrow_e_struct*)
_tmp38E)->f1;goto _LL40C;_LL40C: _tmp3B7=((struct Cyc_Absyn_StructArrow_e_struct*)
_tmp38E)->f2;goto _LL3C8;}else{goto _LL3C9;}_LL3C9: if(*((int*)_tmp38E)== 23){
_LL40F: _tmp3BA=((struct Cyc_Absyn_Subscript_e_struct*)_tmp38E)->f1;goto _LL40E;
_LL40E: _tmp3B9=((struct Cyc_Absyn_Subscript_e_struct*)_tmp38E)->f2;goto _LL3CA;}
else{goto _LL3CB;}_LL3CB: if(*((int*)_tmp38E)== 24){_LL410: _tmp3BB=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp38E)->f1;goto _LL3CC;}else{goto _LL3CD;}_LL3CD: if(*((int*)_tmp38E)== 25){
_LL412: _tmp3BD=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp38E)->f1;goto _LL411;
_LL411: _tmp3BC=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp38E)->f2;goto _LL3CE;}
else{goto _LL3CF;}_LL3CF: if(*((int*)_tmp38E)== 26){_LL413: _tmp3BE=((struct Cyc_Absyn_Array_e_struct*)
_tmp38E)->f1;goto _LL3D0;}else{goto _LL3D1;}_LL3D1: if(*((int*)_tmp38E)== 35){
_LL414: _tmp3BF=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp38E)->f1;goto _LL3D2;}
else{goto _LL3D3;}_LL3D3: if(*((int*)_tmp38E)== 36){_LL415: _tmp3C0=((struct Cyc_Absyn_Codegen_e_struct*)
_tmp38E)->f1;goto _LL3D4;}else{goto _LL3D5;}_LL3D5: if(*((int*)_tmp38E)== 37){
_LL416: _tmp3C1=((struct Cyc_Absyn_Fill_e_struct*)_tmp38E)->f1;goto _LL3D6;}else{
goto _LL3D7;}_LL3D7: if(*((int*)_tmp38E)== 27){_LL419: _tmp3C4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp38E)->f1;goto _LL418;_LL418: _tmp3C3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp38E)->f2;goto _LL417;_LL417: _tmp3C2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp38E)->f3;goto _LL3D8;}else{goto _LL3D9;}_LL3D9: if(*((int*)_tmp38E)== 28){
_LL41D: _tmp3CA=((struct Cyc_Absyn_Struct_e_struct*)_tmp38E)->f1;_tmp3CB=(struct
_tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp38E)->f1;goto _LL41C;_LL41C:
_tmp3C8=((struct Cyc_Absyn_Struct_e_struct*)_tmp38E)->f2;_tmp3C9=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Struct_e_struct*)_tmp38E)->f2;goto _LL41B;_LL41B: _tmp3C7=((struct
Cyc_Absyn_Struct_e_struct*)_tmp38E)->f3;goto _LL41A;_LL41A: _tmp3C5=((struct Cyc_Absyn_Struct_e_struct*)
_tmp38E)->f4;_tmp3C6=(struct Cyc_Absyn_Structdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp38E)->f4;goto _LL3DA;}else{goto _LL3DB;}_LL3DB: if(*((int*)_tmp38E)== 29){
_LL41F: _tmp3CD=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp38E)->f1;goto
_LL41E;_LL41E: _tmp3CC=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp38E)->f2;goto
_LL3DC;}else{goto _LL3DD;}_LL3DD: if(*((int*)_tmp38E)== 30){_LL424: _tmp3D3=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp38E)->f1;_tmp3D4=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Tunion_e_struct*)_tmp38E)->f1;goto _LL423;_LL423: _tmp3D1=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp38E)->f2;_tmp3D2=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Tunion_e_struct*)_tmp38E)->f2;goto _LL422;_LL422: _tmp3D0=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp38E)->f3;goto _LL421;_LL421: _tmp3CF=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp38E)->f4;goto _LL420;_LL420: _tmp3CE=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp38E)->f5;goto _LL3DE;}else{goto _LL3DF;}_LL3DF: if(*((int*)_tmp38E)== 31){
_LL427: _tmp3D7=((struct Cyc_Absyn_Enum_e_struct*)_tmp38E)->f1;_tmp3D8=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp38E)->f1;goto _LL426;_LL426:
_tmp3D6=((struct Cyc_Absyn_Enum_e_struct*)_tmp38E)->f2;goto _LL425;_LL425: _tmp3D5=((
struct Cyc_Absyn_Enum_e_struct*)_tmp38E)->f3;goto _LL3E0;}else{goto _LL3E1;}_LL3E1:
if(*((int*)_tmp38E)== 32){_LL42A: _tmp3DB=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp38E)->f1;_tmp3DC=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp38E)->f1;goto _LL429;_LL429: _tmp3DA=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp38E)->f2;goto _LL428;_LL428: _tmp3D9=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp38E)->f3;goto _LL3E2;}else{goto _LL3E3;}_LL3E3: if(*((int*)_tmp38E)== 33){
_LL42B: _tmp3DD=((struct Cyc_Absyn_Malloc_e_struct*)_tmp38E)->f1;_LL430: _tmp3E5=
_tmp3DD.is_calloc;goto _LL42F;_LL42F: _tmp3E4=_tmp3DD.rgn;goto _LL42E;_LL42E:
_tmp3E2=_tmp3DD.elt_type;_tmp3E3=(void***)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp38E)->f1).elt_type;goto _LL42D;_LL42D: _tmp3E0=_tmp3DD.num_elts;_tmp3E1=(
struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp38E)->f1).num_elts;
goto _LL42C;_LL42C: _tmp3DE=_tmp3DD.fat_result;_tmp3DF=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp38E)->f1).fat_result;goto _LL3E4;}else{goto _LL398;}_LL39A: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp38F);return;_LL39C: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp390);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL39E: Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp392,_tmp391);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL3A0: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp393);
Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL3A2: t=Cyc_Tcexp_tcConst(te,loc,topt,
_tmp395,e);goto _LL398;_LL3A4: t=Cyc_Tcexp_tcVar(te,loc,_tmp397,_tmp396);goto
_LL398;_LL3A6: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp399,_tmp398);goto _LL398;
_LL3A8: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp39B,_tmp39A);goto _LL398;_LL3AA: t=
Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp39E,_tmp39D,_tmp39C);goto _LL398;_LL3AC: t=
Cyc_Tcexp_tcConditional(te,loc,topt,_tmp3A1,_tmp3A0,_tmp39F);goto _LL398;_LL3AE: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp3A3,_tmp3A2);goto _LL398;_LL3B0: t=Cyc_Tcexp_tcFnCall(
te,loc,topt,_tmp3A7,_tmp3A6,_tmp3A5);goto _LL398;_LL3B2: t=Cyc_Tcexp_tcThrow(te,
loc,topt,_tmp3A8);goto _LL398;_LL3B4: t=Cyc_Tcexp_tcInstantiate(te,loc,topt,
_tmp3AA,_tmp3A9);goto _LL398;_LL3B6: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp3AC,
_tmp3AB);goto _LL398;_LL3B8: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp3AD);goto
_LL398;_LL3BA: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp3AF,e,_tmp3AE);goto _LL398;_LL3BC: {
void*_tmp3E6=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp3B0);t=Cyc_Tcexp_tcSizeof(te,loc,
topt,_tmp3E6);goto _LL398;}_LL3BE: t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp3B1);goto
_LL398;_LL3C0: t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp3B3,_tmp3B2);goto _LL398;
_LL3C2:({void*_tmp3E7[0]={};Cyc_Tcutil_terr(loc,_tag_arr("gen() not in top-level initializer",
sizeof(unsigned char),35),_tag_arr(_tmp3E7,sizeof(void*),0));});return;_LL3C4: t=
Cyc_Tcexp_tcDeref(te,loc,topt,_tmp3B4);goto _LL398;_LL3C6: t=Cyc_Tcexp_tcStructMember(
te,loc,topt,e,_tmp3B6,_tmp3B5);goto _LL398;_LL3C8: t=Cyc_Tcexp_tcStructArrow(te,
loc,topt,_tmp3B8,_tmp3B7);goto _LL398;_LL3CA: t=Cyc_Tcexp_tcSubscript(te,loc,topt,
_tmp3BA,_tmp3B9);goto _LL398;_LL3CC: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp3BB);goto
_LL398;_LL3CE: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp3BD,_tmp3BC);goto _LL398;
_LL3D0: {void**elt_topt=0;if(topt != 0){void*_tmp3E8=Cyc_Tcutil_compress(*topt);
void*_tmp3E9;void**_tmp3EA;_LL432: if((unsigned int)_tmp3E8 > 3?*((int*)_tmp3E8)== 
7: 0){_LL436: _tmp3E9=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3E8)->f1;
_tmp3EA=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3E8)->f1);goto
_LL433;}else{goto _LL434;}_LL434: goto _LL435;_LL433: elt_topt=(void**)_tmp3EA;goto
_LL431;_LL435: goto _LL431;_LL431:;}t=Cyc_Tcexp_tcArray(te,loc,elt_topt,_tmp3BE);
goto _LL398;}_LL3D2: t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp3BF);goto _LL398;_LL3D4:
t=Cyc_Tcexp_tcCodegen(te,loc,topt,_tmp3C0);goto _LL398;_LL3D6: t=Cyc_Tcexp_tcFill(
te,loc,topt,_tmp3C1);goto _LL398;_LL3D8: t=Cyc_Tcexp_tcComprehension(te,loc,topt,
_tmp3C4,_tmp3C3,_tmp3C2);goto _LL398;_LL3DA: t=Cyc_Tcexp_tcStruct(te,loc,topt,
_tmp3CB,_tmp3C9,_tmp3C7,_tmp3C6);goto _LL398;_LL3DC: t=Cyc_Tcexp_tcAnonStruct(te,
loc,_tmp3CD,_tmp3CC);goto _LL398;_LL3DE: t=Cyc_Tcexp_tcTunion(te,loc,topt,e,
_tmp3D4,_tmp3D2,_tmp3D0,_tmp3CF,_tmp3CE);goto _LL398;_LL3E0:*_tmp3D8=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp3D5))->name;t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp3EB=
_cycalloc(sizeof(*_tmp3EB));_tmp3EB[0]=({struct Cyc_Absyn_EnumType_struct _tmp3EC;
_tmp3EC.tag=12;_tmp3EC.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp3D6))->name;
_tmp3EC.f2=_tmp3D6;_tmp3EC;});_tmp3EB;});goto _LL398;_LL3E2:*_tmp3DC=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp3D9))->name;t=_tmp3DA;goto _LL398;_LL3E4: t=Cyc_Tcexp_tcMalloc(te,
loc,topt,_tmp3E4,_tmp3E3,_tmp3E1,_tmp3E5,_tmp3DF);goto _LL398;_LL398:;}e->topt=({
struct Cyc_Core_Opt*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED->v=(void*)t;
_tmp3ED;});}
