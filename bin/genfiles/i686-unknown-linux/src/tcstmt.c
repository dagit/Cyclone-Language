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
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern int Cyc_List_length(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern struct
Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern unsigned char Cyc_List_Nth[
8];struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
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
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;void*f1;struct Cyc_Absyn_Tqual f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_FnType_struct{
int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_StructType_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};struct Cyc_Absyn_UnionType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};
struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_AnonStructType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
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
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_StructMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_StructArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Struct_p_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;};struct Cyc_Absyn_Tunion_p_struct{int tag;struct Cyc_Absyn_Tuniondecl*
f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;
};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_Absyn_UnknownFields_p_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;
struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structfield{
struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Struct_d_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Absyn_Union_d_struct{int tag;struct Cyc_Absyn_Uniondecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern void*Cyc_Absyn_exn_typ;extern struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(
void*s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*
p,struct Cyc_Position_Segment*s);struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr
fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
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
le;};extern void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Vardecl*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);extern struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);extern struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
continue_dest);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*
clause);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct Cyc_Tcenv_Tenv*,void*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct Cyc_Tcenv_Tenv*te);extern
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);extern void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);extern void
Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
extern void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct _tagged_arr*,struct Cyc_Absyn_Stmt**);struct _tuple3{struct Cyc_Absyn_Switch_clause*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};extern struct _tuple3*Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);extern
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct
_tagged_arr*,struct Cyc_Absyn_Stmt*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_named_block(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Tvar*name);extern void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*r);
extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(struct
_tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
extern int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);extern void Cyc_Tcutil_explain_failure();extern int Cyc_Tcutil_unify(void*,
void*);extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);struct
_tuple4{struct Cyc_Absyn_Tvar*f1;void*f2;};extern struct _tuple4*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);extern void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*
bound_tvars,void*k,int allow_evars,void*);extern int*Cyc_Tcutil_new_tvar_id();
extern int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
extern void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
extern void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
extern void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_tagged_arr msg_part);struct _tuple5{struct Cyc_List_List*f1;struct Cyc_List_List*f2;
};extern struct _tuple5 Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p,void**topt,void**region_opt);extern void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);extern void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);extern int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);extern void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};extern unsigned char Cyc_CfFlowInfo_IsZero[11];struct Cyc_CfFlowInfo_IsZero_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_CfFlowInfo_NotZero[
12];struct Cyc_CfFlowInfo_NotZero_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
unsigned char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int
tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};extern void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);extern void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);extern unsigned int
Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);static void Cyc_Tcstmt_decorate_stmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));s->try_depth=Cyc_Tcenv_get_try_depth(te);}static
void Cyc_Tcstmt_pattern_synth(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*tvs,struct Cyc_List_List*vs,struct Cyc_Absyn_Stmt*s,struct
Cyc_Absyn_Exp*where_opt,int new_block){struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
loc,te,tvs);if(new_block){te2=Cyc_Tcenv_new_block(loc,te2);}{struct Cyc_Core_Opt*
_tmp0=({struct Cyc_Core_Opt*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->v=(void*)Cyc_Tcenv_curr_rgn(
te2);_tmp2;});{struct Cyc_List_List*_tmp1=vs;for(0;_tmp1 != 0;_tmp1=_tmp1->tl){te2=
Cyc_Tcenv_add_pat_var(loc,te2,(struct Cyc_Absyn_Vardecl*)_tmp1->hd);((struct Cyc_Absyn_Vardecl*)
_tmp1->hd)->rgn=_tmp0;}}if(where_opt != 0){Cyc_Tcexp_tcTest(te2,(struct Cyc_Absyn_Exp*)
_check_null(where_opt),_tag_arr("switch clause guard",sizeof(unsigned char),20));}
if(vs != 0){te2=Cyc_Tcenv_set_encloser(te2,s);}Cyc_Tcstmt_tcStmt(te2,s,0);if(vs != 
0){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));}}}struct
_tuple6{struct Cyc_Tcenv_Tenv*f1;struct Cyc_List_List*f2;};struct _tuple6 Cyc_Tcstmt_check_forarray_decls(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*decls){({
void*_tmp3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("check_forarray_decls",sizeof(unsigned char),21),_tag_arr(_tmp3,sizeof(
void*),0));});return({struct _tuple6 _tmp4;_tmp4.f1=te;_tmp4.f2=0;_tmp4;});}void
Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
Cyc_Tcstmt_decorate_stmt(te,s0);{void*_tmp5=(void*)s0->r;struct Cyc_Absyn_Exp*
_tmp6;struct Cyc_Absyn_Stmt*_tmp7;struct Cyc_Absyn_Stmt*_tmp8;struct Cyc_Absyn_Exp*
_tmp9;struct Cyc_Absyn_Stmt*_tmpA;struct Cyc_Absyn_Stmt*_tmpB;struct Cyc_Absyn_Exp*
_tmpC;struct Cyc_Absyn_Stmt*_tmpD;struct _tuple2 _tmpE;struct Cyc_Absyn_Stmt*_tmpF;
struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Stmt*_tmp11;struct _tuple2 _tmp12;
struct Cyc_Absyn_Stmt*_tmp13;struct Cyc_Absyn_Exp*_tmp14;struct _tuple2 _tmp15;
struct Cyc_Absyn_Stmt*_tmp16;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*
_tmp18;struct Cyc_Absyn_ForArrayInfo _tmp19;struct Cyc_Absyn_Stmt*_tmp1A;struct
_tuple2 _tmp1B;struct Cyc_Absyn_Stmt*_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct
_tuple2 _tmp1E;struct Cyc_Absyn_Stmt*_tmp1F;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_List_List*
_tmp21;struct _tuple2 _tmp22;struct Cyc_Absyn_Stmt*_tmp23;struct Cyc_Absyn_Exp*
_tmp24;struct Cyc_Absyn_Stmt*_tmp25;struct Cyc_Absyn_Stmt*_tmp26;struct Cyc_Absyn_Stmt**
_tmp27;struct Cyc_Absyn_Stmt*_tmp28;struct Cyc_Absyn_Stmt**_tmp29;struct Cyc_Absyn_Stmt*
_tmp2A;struct Cyc_Absyn_Stmt**_tmp2B;struct _tagged_arr*_tmp2C;struct Cyc_Absyn_Switch_clause**
_tmp2D;struct Cyc_Absyn_Switch_clause***_tmp2E;struct Cyc_List_List*_tmp2F;struct
Cyc_Absyn_Stmt*_tmp30;struct _tagged_arr*_tmp31;struct Cyc_List_List*_tmp32;struct
Cyc_Absyn_Exp*_tmp33;struct Cyc_List_List*_tmp34;struct Cyc_Absyn_Exp*_tmp35;
struct Cyc_List_List*_tmp36;struct Cyc_Absyn_Stmt*_tmp37;struct Cyc_Absyn_Stmt*
_tmp38;struct Cyc_Absyn_Decl*_tmp39;struct Cyc_Absyn_Stmt*_tmp3A;struct Cyc_Absyn_Vardecl*
_tmp3B;struct Cyc_Absyn_Tvar*_tmp3C;struct Cyc_Absyn_Stmt*_tmp3D;struct Cyc_Absyn_Stmt*
_tmp3E;_LL1: if((int)_tmp5 == 0){goto _LL2;}else{goto _LL3;}_LL3: if((unsigned int)
_tmp5 > 1?*((int*)_tmp5)== 0: 0){_LL2B: _tmp6=((struct Cyc_Absyn_Exp_s_struct*)_tmp5)->f1;
goto _LL4;}else{goto _LL5;}_LL5: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== 1: 0){
_LL2D: _tmp8=((struct Cyc_Absyn_Seq_s_struct*)_tmp5)->f1;goto _LL2C;_LL2C: _tmp7=((
struct Cyc_Absyn_Seq_s_struct*)_tmp5)->f2;goto _LL6;}else{goto _LL7;}_LL7: if((
unsigned int)_tmp5 > 1?*((int*)_tmp5)== 2: 0){_LL2E: _tmp9=((struct Cyc_Absyn_Return_s_struct*)
_tmp5)->f1;goto _LL8;}else{goto _LL9;}_LL9: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)
== 3: 0){_LL31: _tmpC=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5)->f1;goto _LL30;
_LL30: _tmpB=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5)->f2;goto _LL2F;_LL2F:
_tmpA=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5)->f3;goto _LLA;}else{goto _LLB;}
_LLB: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== 4: 0){_LL33: _tmpE=((struct Cyc_Absyn_While_s_struct*)
_tmp5)->f1;_LL35: _tmp10=_tmpE.f1;goto _LL34;_LL34: _tmpF=_tmpE.f2;goto _LL32;_LL32:
_tmpD=((struct Cyc_Absyn_While_s_struct*)_tmp5)->f2;goto _LLC;}else{goto _LLD;}_LLD:
if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== 8: 0){_LL3D: _tmp18=((struct Cyc_Absyn_For_s_struct*)
_tmp5)->f1;goto _LL3A;_LL3A: _tmp15=((struct Cyc_Absyn_For_s_struct*)_tmp5)->f2;
_LL3C: _tmp17=_tmp15.f1;goto _LL3B;_LL3B: _tmp16=_tmp15.f2;goto _LL37;_LL37: _tmp12=((
struct Cyc_Absyn_For_s_struct*)_tmp5)->f3;_LL39: _tmp14=_tmp12.f1;goto _LL38;_LL38:
_tmp13=_tmp12.f2;goto _LL36;_LL36: _tmp11=((struct Cyc_Absyn_For_s_struct*)_tmp5)->f4;
goto _LLE;}else{goto _LLF;}_LLF: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== 19: 0){
_LL3E: _tmp19=((struct Cyc_Absyn_ForArray_s_struct*)_tmp5)->f1;_LL46: _tmp21=_tmp19.defns;
goto _LL43;_LL43: _tmp1E=_tmp19.condition;_LL45: _tmp20=_tmp1E.f1;goto _LL44;_LL44:
_tmp1F=_tmp1E.f2;goto _LL40;_LL40: _tmp1B=_tmp19.delta;_LL42: _tmp1D=_tmp1B.f1;goto
_LL41;_LL41: _tmp1C=_tmp1B.f2;goto _LL3F;_LL3F: _tmp1A=_tmp19.body;goto _LL10;}else{
goto _LL11;}_LL11: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== 16: 0){_LL4A: _tmp25=((
struct Cyc_Absyn_Do_s_struct*)_tmp5)->f1;goto _LL47;_LL47: _tmp22=((struct Cyc_Absyn_Do_s_struct*)
_tmp5)->f2;_LL49: _tmp24=_tmp22.f1;goto _LL48;_LL48: _tmp23=_tmp22.f2;goto _LL12;}
else{goto _LL13;}_LL13: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== 5: 0){_LL4B:
_tmp26=((struct Cyc_Absyn_Break_s_struct*)_tmp5)->f1;_tmp27=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Break_s_struct*)_tmp5)->f1;goto _LL14;}else{goto _LL15;}_LL15: if((
unsigned int)_tmp5 > 1?*((int*)_tmp5)== 6: 0){_LL4C: _tmp28=((struct Cyc_Absyn_Continue_s_struct*)
_tmp5)->f1;_tmp29=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Continue_s_struct*)
_tmp5)->f1;goto _LL16;}else{goto _LL17;}_LL17: if((unsigned int)_tmp5 > 1?*((int*)
_tmp5)== 7: 0){_LL4E: _tmp2C=((struct Cyc_Absyn_Goto_s_struct*)_tmp5)->f1;goto _LL4D;
_LL4D: _tmp2A=((struct Cyc_Absyn_Goto_s_struct*)_tmp5)->f2;_tmp2B=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Goto_s_struct*)_tmp5)->f2;goto _LL18;}else{goto _LL19;}_LL19: if((
unsigned int)_tmp5 > 1?*((int*)_tmp5)== 11: 0){_LL50: _tmp2F=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp5)->f1;goto _LL4F;_LL4F: _tmp2D=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp5)->f2;
_tmp2E=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp5)->f2;goto _LL1A;}else{goto _LL1B;}_LL1B: if((unsigned int)_tmp5 > 1?*((int*)
_tmp5)== 15: 0){_LL52: _tmp31=((struct Cyc_Absyn_Label_s_struct*)_tmp5)->f1;goto
_LL51;_LL51: _tmp30=((struct Cyc_Absyn_Label_s_struct*)_tmp5)->f2;goto _LL1C;}else{
goto _LL1D;}_LL1D: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== 9: 0){_LL54: _tmp33=((
struct Cyc_Absyn_Switch_s_struct*)_tmp5)->f1;goto _LL53;_LL53: _tmp32=((struct Cyc_Absyn_Switch_s_struct*)
_tmp5)->f2;goto _LL1E;}else{goto _LL1F;}_LL1F: if((unsigned int)_tmp5 > 1?*((int*)
_tmp5)== 10: 0){_LL56: _tmp35=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp5)->f1;goto
_LL55;_LL55: _tmp34=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp5)->f2;goto _LL20;}
else{goto _LL21;}_LL21: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== 17: 0){_LL58:
_tmp37=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp5)->f1;goto _LL57;_LL57: _tmp36=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp5)->f2;goto _LL22;}else{goto _LL23;}_LL23:
if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== 12: 0){_LL5A: _tmp39=((struct Cyc_Absyn_Decl_s_struct*)
_tmp5)->f1;goto _LL59;_LL59: _tmp38=((struct Cyc_Absyn_Decl_s_struct*)_tmp5)->f2;
goto _LL24;}else{goto _LL25;}_LL25: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== 18: 0){
_LL5D: _tmp3C=((struct Cyc_Absyn_Region_s_struct*)_tmp5)->f1;goto _LL5C;_LL5C:
_tmp3B=((struct Cyc_Absyn_Region_s_struct*)_tmp5)->f2;goto _LL5B;_LL5B: _tmp3A=((
struct Cyc_Absyn_Region_s_struct*)_tmp5)->f3;goto _LL26;}else{goto _LL27;}_LL27: if((
unsigned int)_tmp5 > 1?*((int*)_tmp5)== 13: 0){_LL5E: _tmp3D=((struct Cyc_Absyn_Cut_s_struct*)
_tmp5)->f1;goto _LL28;}else{goto _LL29;}_LL29: if((unsigned int)_tmp5 > 1?*((int*)
_tmp5)== 14: 0){_LL5F: _tmp3E=((struct Cyc_Absyn_Splice_s_struct*)_tmp5)->f1;goto
_LL2A;}else{goto _LL0;}_LL2: return;_LL4: Cyc_Tcexp_tcExp(te,0,_tmp6);return;_LL6: {
struct Cyc_Tcenv_Tenv*_tmp3F=Cyc_Tcenv_set_next(te,(void*)({struct Cyc_Tcenv_Stmt_j_struct*
_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Tcenv_Stmt_j_struct
_tmp46;_tmp46.tag=0;_tmp46.f1=_tmp7;_tmp46;});_tmp45;}));Cyc_Tcstmt_tcStmt(
_tmp3F,_tmp8,1);while(1){void*_tmp40=(void*)_tmp7->r;struct Cyc_Absyn_Stmt*_tmp41;
struct Cyc_Absyn_Stmt*_tmp42;_LL61: if((unsigned int)_tmp40 > 1?*((int*)_tmp40)== 1:
0){_LL66: _tmp42=((struct Cyc_Absyn_Seq_s_struct*)_tmp40)->f1;goto _LL65;_LL65:
_tmp41=((struct Cyc_Absyn_Seq_s_struct*)_tmp40)->f2;goto _LL62;}else{goto _LL63;}
_LL63: goto _LL64;_LL62: Cyc_Tcstmt_decorate_stmt(te,_tmp7);_tmp3F=Cyc_Tcenv_set_next(
te,(void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmp43=_cycalloc(sizeof(*_tmp43));
_tmp43[0]=({struct Cyc_Tcenv_Stmt_j_struct _tmp44;_tmp44.tag=0;_tmp44.f1=_tmp41;
_tmp44;});_tmp43;}));Cyc_Tcstmt_tcStmt(_tmp3F,_tmp42,1);_tmp7=_tmp41;continue;
_LL64: goto seq_end;_LL60:;}seq_end: Cyc_Tcstmt_tcStmt(te,_tmp7,1);return;}_LL8: {
void*t=Cyc_Tcenv_return_typ(te);if(_tmp9 == 0){void*_tmp47=Cyc_Tcutil_compress(t);
_LL68: if((int)_tmp47 == 0){goto _LL69;}else{goto _LL6A;}_LL6A: goto _LL6B;_LL69: goto
_LL67;_LL6B:({struct Cyc_Std_String_pa_struct _tmp49;_tmp49.tag=0;_tmp49.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp48[1]={& _tmp49};Cyc_Tcutil_terr(
s0->loc,_tag_arr("must return a value of type %s",sizeof(unsigned char),31),
_tag_arr(_tmp48,sizeof(void*),1));}});goto _LL67;_LL67:;}else{struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_check_null(_tmp9);Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!
Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct _tmp4C;_tmp4C.tag=
0;_tmp4C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp4B;_tmp4B.tag=0;_tmp4B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp4A[2]={& _tmp4B,& _tmp4C};
Cyc_Tcutil_terr(s0->loc,_tag_arr("returns value of type %s but requires %s",
sizeof(unsigned char),41),_tag_arr(_tmp4A,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
return;}_LLA: Cyc_Tcexp_tcTest(te,_tmpC,_tag_arr("if statement",sizeof(
unsigned char),13));Cyc_Tcstmt_tcStmt(te,_tmpB,1);Cyc_Tcstmt_tcStmt(te,_tmpA,1);
return;_LLC: Cyc_Tcstmt_decorate_stmt(te,_tmpF);Cyc_Tcexp_tcTest(te,_tmp10,
_tag_arr("while loop",sizeof(unsigned char),11));Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(
te,_tmpF),_tmpD,1);return;_LLE: Cyc_Tcstmt_decorate_stmt(te,_tmp16);Cyc_Tcstmt_decorate_stmt(
te,_tmp13);Cyc_Tcexp_tcExp(te,0,_tmp18);Cyc_Tcexp_tcTest(te,_tmp17,_tag_arr("for loop",
sizeof(unsigned char),9));te=Cyc_Tcenv_set_in_loop(te,_tmp13);Cyc_Tcstmt_tcStmt(
te,_tmp11,1);Cyc_Tcexp_tcExp(te,0,_tmp14);return;_LL10:({void*_tmp4D[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("ForArray_s",
sizeof(unsigned char),11),_tag_arr(_tmp4D,sizeof(void*),0));});return;_LL12: Cyc_Tcstmt_decorate_stmt(
te,_tmp23);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(te,_tmp23),_tmp25,1);Cyc_Tcexp_tcTest(
te,_tmp24,_tag_arr("do loop",sizeof(unsigned char),8));return;_LL14: Cyc_Tcenv_process_break(
te,s0,_tmp27);return;_LL16: Cyc_Tcenv_process_continue(te,s0,_tmp29);return;_LL18:
Cyc_Tcenv_process_goto(te,s0,_tmp2C,_tmp2B);return;_LL1A: {struct _tuple3*_tmp4E=
Cyc_Tcenv_process_fallthru(te,s0,_tmp2E);if(_tmp4E == 0){({void*_tmp4F[0]={};Cyc_Tcutil_terr(
s0->loc,_tag_arr("fallthru not in a non-last case",sizeof(unsigned char),32),
_tag_arr(_tmp4F,sizeof(void*),0));});return;}{struct _tuple3 _tmp51;struct Cyc_List_List*
_tmp52;struct Cyc_List_List*_tmp53;struct _tuple3*_tmp50=(struct _tuple3*)
_check_null(_tmp4E);_tmp51=*_tmp50;_LL6E: _tmp53=_tmp51.f2;goto _LL6D;_LL6D: _tmp52=
_tmp51.f3;goto _LL6C;_LL6C: {struct Cyc_List_List*instantiation=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(
te),_tmp53);_tmp52=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,
instantiation,_tmp52);for(0;_tmp52 != 0? _tmp2F != 0: 0;(_tmp52=_tmp52->tl,_tmp2F=
_tmp2F->tl)){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp2F->hd);if(! Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)_tmp2F->hd,(void*)_tmp52->hd)){({struct Cyc_Std_String_pa_struct
_tmp56;_tmp56.tag=0;_tmp56.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
_tmp52->hd);{struct Cyc_Std_String_pa_struct _tmp55;_tmp55.tag=0;_tmp55.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)_tmp2F->hd)->topt))->v);{void*_tmp54[2]={& _tmp55,& _tmp56};
Cyc_Tcutil_terr(s0->loc,_tag_arr("fallthru argument has type %s but pattern variable has type %s",
sizeof(unsigned char),63),_tag_arr(_tmp54,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
if(_tmp2F != 0){({void*_tmp57[0]={};Cyc_Tcutil_terr(s0->loc,_tag_arr("too many arguments to explicit fallthru",
sizeof(unsigned char),40),_tag_arr(_tmp57,sizeof(void*),0));});}if(_tmp52 != 0){({
void*_tmp58[0]={};Cyc_Tcutil_terr(s0->loc,_tag_arr("too few arguments to explicit fallthru",
sizeof(unsigned char),39),_tag_arr(_tmp58,sizeof(void*),0));});}return;}}}_LL1C:
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(s0->loc,Cyc_Tcenv_add_label(te,
_tmp31,_tmp30),({struct Cyc_Absyn_Tvar*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->name=({
struct _tagged_arr*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C[0]=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct _tagged_arr)*
_tmp31;{void*_tmp5D[1]={& _tmp5E};Cyc_Std_aprintf(_tag_arr("`%s",sizeof(
unsigned char),4),_tag_arr(_tmp5D,sizeof(void*),1));}});_tmp5C;});_tmp59->identity=
Cyc_Tcutil_new_tvar_id();_tmp59->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp5B;
_tmp5B.tag=0;_tmp5B.f1=(void*)((void*)3);_tmp5B;});_tmp5A;}));_tmp59;})),_tmp30,
0);return;_LL1E: Cyc_Tcexp_tcExp(te,0,_tmp33);{void*_tmp5F=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp33->topt))->v;te=Cyc_Tcenv_set_in_switch(te);te=Cyc_Tcenv_clear_fallthru(
te);{struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp32);for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Pat*_tmp60=((
struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_List_List*_tmp62;
struct Cyc_List_List*_tmp63;struct _tuple5 _tmp61=Cyc_Tcpat_tcPat(te,_tmp60,(void**)&
_tmp5F,0);_LL71: _tmp63=_tmp61.f1;goto _LL70;_LL70: _tmp62=_tmp61.f2;goto _LL6F;
_LL6F: if(! Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp60->topt))->v,
_tmp5F)){({struct Cyc_Std_String_pa_struct _tmp66;_tmp66.tag=0;_tmp66.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp60->topt))->v);{struct Cyc_Std_String_pa_struct _tmp65;_tmp65.tag=0;_tmp65.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp5F);{void*_tmp64[2]={& _tmp65,&
_tmp66};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tag_arr("switch on type %s, but case expects type %s",
sizeof(unsigned char),44),_tag_arr(_tmp64,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(te,_tmp60);}((struct Cyc_Absyn_Switch_clause*)
scs->hd)->pat_vars=({struct Cyc_Core_Opt*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->v=
_tmp62;_tmp67;});Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,
te,_tmp63,_tmp62,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)
scs->hd)->where_clause,1);te=Cyc_Tcenv_set_fallthru(te,_tmp63,_tmp62,(struct Cyc_Absyn_Switch_clause*)
scs->hd);}}Cyc_Tcpat_check_switch_exhaustive(s0->loc,_tmp32);return;}_LL20: {
struct Cyc_List_List*_tmp68=0;for(0;_tmp34 != 0;_tmp34=_tmp34->tl){struct Cyc_Absyn_SwitchC_clause
_tmp6A;struct Cyc_Position_Segment*_tmp6B;struct Cyc_Absyn_Stmt*_tmp6C;struct Cyc_Absyn_Exp*
_tmp6D;struct Cyc_Absyn_SwitchC_clause*_tmp69=(struct Cyc_Absyn_SwitchC_clause*)
_tmp34->hd;_tmp6A=*_tmp69;_LL75: _tmp6D=_tmp6A.cnst_exp;goto _LL74;_LL74: _tmp6C=
_tmp6A.body;goto _LL73;_LL73: _tmp6B=_tmp6A.loc;goto _LL72;_LL72: {struct Cyc_Absyn_Pat*
pat;if(_tmp6D != 0){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp6D));
pat=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp6E=_cycalloc(
sizeof(*_tmp6E));_tmp6E[0]=({struct Cyc_Absyn_Int_p_struct _tmp6F;_tmp6F.tag=1;
_tmp6F.f1=(void*)((void*)0);_tmp6F.f2=(int)Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp6D));_tmp6F;});_tmp6E;}),_tmp6D->loc);}else{pat=
Cyc_Absyn_new_pat((void*)0,_tmp6B);}_tmp68=({struct Cyc_List_List*_tmp70=
_cycalloc(sizeof(*_tmp70));_tmp70->hd=({struct Cyc_Absyn_Switch_clause*_tmp71=
_cycalloc(sizeof(*_tmp71));_tmp71->pattern=pat;_tmp71->pat_vars=0;_tmp71->where_clause=
0;_tmp71->body=_tmp6C;_tmp71->loc=_tmp6B;_tmp71;});_tmp70->tl=_tmp68;_tmp70;});}}(
void*)(s0->r=(void*)((void*)(Cyc_Absyn_switch_stmt(_tmp35,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp68),0))->r));Cyc_Tcstmt_tcStmt(te,
s0,new_block);return;}_LL22:(void*)(_tmp37->r=(void*)((void*)(Cyc_Absyn_seq_stmt(
Cyc_Absyn_new_stmt((void*)_tmp37->r,_tmp37->loc),Cyc_Absyn_skip_stmt(_tmp37->loc),
_tmp37->loc))->r));Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(Cyc_Tcenv_enter_try(
te),s0),_tmp37,1);te=Cyc_Tcenv_set_in_switch(te);te=Cyc_Tcenv_clear_fallthru(te);{
struct Cyc_List_List*_tmp72=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
_tmp36);for(0;_tmp72 != 0;_tmp72=_tmp72->tl){struct Cyc_Absyn_Pat*_tmp73=((struct
Cyc_Absyn_Switch_clause*)_tmp72->hd)->pattern;struct Cyc_List_List*_tmp75;struct
Cyc_List_List*_tmp76;struct _tuple5 _tmp74=Cyc_Tcpat_tcPat(te,_tmp73,(void**)& Cyc_Absyn_exn_typ,
0);_LL78: _tmp76=_tmp74.f1;goto _LL77;_LL77: _tmp75=_tmp74.f2;goto _LL76;_LL76: if(!
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp73->topt))->v,Cyc_Absyn_exn_typ)){({
struct Cyc_Std_String_pa_struct _tmp78;_tmp78.tag=0;_tmp78.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp73->topt))->v);{
void*_tmp77[1]={& _tmp78};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmp72->hd)->loc,
_tag_arr("expected xtunion exn but found %s",sizeof(unsigned char),34),_tag_arr(
_tmp77,sizeof(void*),1));}});}else{Cyc_Tcpat_check_pat_regions(te,_tmp73);}((
struct Cyc_Absyn_Switch_clause*)_tmp72->hd)->pat_vars=({struct Cyc_Core_Opt*_tmp79=
_cycalloc(sizeof(*_tmp79));_tmp79->v=_tmp75;_tmp79;});Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)_tmp72->hd)->loc,te,_tmp76,_tmp75,((struct Cyc_Absyn_Switch_clause*)
_tmp72->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp72->hd)->where_clause,1);
te=Cyc_Tcenv_set_fallthru(te,_tmp76,_tmp75,(struct Cyc_Absyn_Switch_clause*)
_tmp72->hd);}}Cyc_Tcpat_check_catch_overlap(s0->loc,_tmp36);return;_LL24: {
struct _tagged_arr unimp_msg_part;if(new_block){te=Cyc_Tcenv_new_block(s0->loc,te);}{
void*_tmp7A=(void*)_tmp39->r;struct Cyc_Absyn_Vardecl*_tmp7B;int _tmp7C;int*_tmp7D;
struct Cyc_Absyn_Exp*_tmp7E;struct Cyc_Core_Opt*_tmp7F;struct Cyc_Core_Opt**_tmp80;
struct Cyc_Core_Opt*_tmp81;struct Cyc_Core_Opt**_tmp82;struct Cyc_Absyn_Pat*_tmp83;
struct Cyc_List_List*_tmp84;struct Cyc_List_List*_tmp85;struct _tagged_arr*_tmp86;
struct Cyc_List_List*_tmp87;struct _tuple0*_tmp88;_LL7A: if(*((int*)_tmp7A)== 0){
_LL92: _tmp7B=((struct Cyc_Absyn_Var_d_struct*)_tmp7A)->f1;goto _LL7B;}else{goto
_LL7C;}_LL7C: if(*((int*)_tmp7A)== 2){_LL97: _tmp83=((struct Cyc_Absyn_Let_d_struct*)
_tmp7A)->f1;goto _LL96;_LL96: _tmp81=((struct Cyc_Absyn_Let_d_struct*)_tmp7A)->f2;
_tmp82=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_struct*)_tmp7A)->f2;goto
_LL95;_LL95: _tmp7F=((struct Cyc_Absyn_Let_d_struct*)_tmp7A)->f3;_tmp80=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Let_d_struct*)_tmp7A)->f3;goto _LL94;_LL94: _tmp7E=((struct Cyc_Absyn_Let_d_struct*)
_tmp7A)->f4;goto _LL93;_LL93: _tmp7C=((struct Cyc_Absyn_Let_d_struct*)_tmp7A)->f5;
_tmp7D=(int*)&((struct Cyc_Absyn_Let_d_struct*)_tmp7A)->f5;goto _LL7D;}else{goto
_LL7E;}_LL7E: if(*((int*)_tmp7A)== 3){_LL98: _tmp84=((struct Cyc_Absyn_Letv_d_struct*)
_tmp7A)->f1;goto _LL7F;}else{goto _LL80;}_LL80: if(*((int*)_tmp7A)== 9){_LL9A:
_tmp86=((struct Cyc_Absyn_Namespace_d_struct*)_tmp7A)->f1;goto _LL99;_LL99: _tmp85=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp7A)->f2;goto _LL81;}else{goto _LL82;}_LL82:
if(*((int*)_tmp7A)== 10){_LL9C: _tmp88=((struct Cyc_Absyn_Using_d_struct*)_tmp7A)->f1;
goto _LL9B;_LL9B: _tmp87=((struct Cyc_Absyn_Using_d_struct*)_tmp7A)->f2;goto _LL83;}
else{goto _LL84;}_LL84: if(*((int*)_tmp7A)== 1){goto _LL85;}else{goto _LL86;}_LL86:
if(*((int*)_tmp7A)== 4){goto _LL87;}else{goto _LL88;}_LL88: if(*((int*)_tmp7A)== 5){
goto _LL89;}else{goto _LL8A;}_LL8A: if(*((int*)_tmp7A)== 6){goto _LL8B;}else{goto
_LL8C;}_LL8C: if(*((int*)_tmp7A)== 7){goto _LL8D;}else{goto _LL8E;}_LL8E: if(*((int*)
_tmp7A)== 8){goto _LL8F;}else{goto _LL90;}_LL90: if(*((int*)_tmp7A)== 11){goto _LL91;}
else{goto _LL79;}_LL7B: {struct Cyc_Absyn_Vardecl _tmp8A;struct Cyc_List_List*_tmp8B;
struct Cyc_Core_Opt*_tmp8C;struct Cyc_Core_Opt**_tmp8D;struct Cyc_Absyn_Exp*_tmp8E;
void*_tmp8F;struct Cyc_Absyn_Tqual _tmp90;struct _tuple0*_tmp91;struct _tuple0 _tmp92;
struct _tagged_arr*_tmp93;void*_tmp94;void*_tmp95;struct Cyc_Absyn_Vardecl*_tmp89=
_tmp7B;_tmp8A=*_tmp89;_LLA6: _tmp95=(void*)_tmp8A.sc;goto _LLA3;_LLA3: _tmp91=
_tmp8A.name;_tmp92=*_tmp91;_LLA5: _tmp94=_tmp92.f1;goto _LLA4;_LLA4: _tmp93=_tmp92.f2;
goto _LLA2;_LLA2: _tmp90=_tmp8A.tq;goto _LLA1;_LLA1: _tmp8F=(void*)_tmp8A.type;goto
_LLA0;_LLA0: _tmp8E=_tmp8A.initializer;goto _LL9F;_LL9F: _tmp8C=_tmp8A.rgn;_tmp8D=(
struct Cyc_Core_Opt**)&(*_tmp89).rgn;goto _LL9E;_LL9E: _tmp8B=_tmp8A.attributes;
goto _LL9D;_LL9D: {void*_tmp96=Cyc_Tcenv_curr_rgn(te);int is_local;{void*_tmp97=
_tmp95;_LLA8: if((int)_tmp97 == 0){goto _LLA9;}else{goto _LLAA;}_LLAA: if((int)_tmp97
== 3){goto _LLAB;}else{goto _LLAC;}_LLAC: if((int)_tmp97 == 4){goto _LLAD;}else{goto
_LLAE;}_LLAE: if((int)_tmp97 == 1){goto _LLAF;}else{goto _LLB0;}_LLB0: if((int)_tmp97
== 2){goto _LLB1;}else{goto _LLA7;}_LLA9: goto _LLAB;_LLAB: goto _LLAD;_LLAD: is_local=
0;goto _LLA7;_LLAF:({void*_tmp98[0]={};Cyc_Tcutil_terr(_tmp39->loc,_tag_arr("bad abstract scope for local variable",
sizeof(unsigned char),38),_tag_arr(_tmp98,sizeof(void*),0));});goto _LLB1;_LLB1:
is_local=1;goto _LLA7;_LLA7:;}*_tmp8D=is_local?({struct Cyc_Core_Opt*_tmp99=
_cycalloc(sizeof(*_tmp99));_tmp99->v=(void*)_tmp96;_tmp99;}):({struct Cyc_Core_Opt*
_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->v=(void*)((void*)2);_tmp9A;});{void*
_tmp9B=_tmp94;struct Cyc_List_List*_tmp9C;struct Cyc_List_List*_tmp9D;_LLB3: if((
int)_tmp9B == 0){goto _LLB4;}else{goto _LLB5;}_LLB5: if((unsigned int)_tmp9B > 1?*((
int*)_tmp9B)== 0: 0){_LLBB: _tmp9C=((struct Cyc_Absyn_Rel_n_struct*)_tmp9B)->f1;if(
_tmp9C == 0){goto _LLB6;}else{goto _LLB7;}}else{goto _LLB7;}_LLB7: if((unsigned int)
_tmp9B > 1?*((int*)_tmp9B)== 1: 0){_LLBC: _tmp9D=((struct Cyc_Absyn_Abs_n_struct*)
_tmp9B)->f1;goto _LLB8;}else{goto _LLB9;}_LLB9: goto _LLBA;_LLB4: goto _LLB2;_LLB6:(*
_tmp7B->name).f1=(void*)0;goto _LLB2;_LLB8:(int)_throw(({void*_tmp9E[0]={};Cyc_Tcutil_impos(
_tag_arr("tcstmt: Abs_n declaration",sizeof(unsigned char),26),_tag_arr(_tmp9E,
sizeof(void*),0));}));_LLBA:({void*_tmp9F[0]={};Cyc_Tcutil_terr(_tmp39->loc,
_tag_arr("cannot declare a qualified local variable",sizeof(unsigned char),42),
_tag_arr(_tmp9F,sizeof(void*),0));});goto _LLB2;_LLB2:;}{void*_tmpA0=Cyc_Tcutil_compress(
_tmp8F);struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_Absyn_Tqual _tmpA2;void*_tmpA3;_LLBE:
if((unsigned int)_tmpA0 > 3?*((int*)_tmpA0)== 7: 0){_LLC5: _tmpA3=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmpA0)->f1;goto _LLC4;_LLC4: _tmpA2=((struct Cyc_Absyn_ArrayType_struct*)
_tmpA0)->f2;goto _LLC3;_LLC3: _tmpA1=((struct Cyc_Absyn_ArrayType_struct*)_tmpA0)->f3;
if(_tmpA1 == 0){goto _LLC2;}else{goto _LLC0;}}else{goto _LLC0;}_LLC2: if(_tmp7B->initializer
!= 0){goto _LLBF;}else{goto _LLC0;}_LLC0: goto _LLC1;_LLBF:{void*_tmpA4=(void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp7B->initializer))->r;void*_tmpA5;struct
_tagged_arr _tmpA6;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_List_List*_tmpA8;struct
Cyc_List_List*_tmpA9;_LLC7: if(*((int*)_tmpA4)== 0){_LLD1: _tmpA5=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmpA4)->f1;if((unsigned int)_tmpA5 > 1?*((int*)_tmpA5)
== 5: 0){_LLD2: _tmpA6=((struct Cyc_Absyn_String_c_struct*)_tmpA5)->f1;goto _LLC8;}
else{goto _LLC9;}}else{goto _LLC9;}_LLC9: if(*((int*)_tmpA4)== 27){_LLD3: _tmpA7=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpA4)->f2;goto _LLCA;}else{goto _LLCB;}
_LLCB: if(*((int*)_tmpA4)== 34){_LLD4: _tmpA8=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpA4)->f2;goto _LLCC;}else{goto _LLCD;}_LLCD: if(*((int*)_tmpA4)== 26){_LLD5:
_tmpA9=((struct Cyc_Absyn_Array_e_struct*)_tmpA4)->f1;goto _LLCE;}else{goto _LLCF;}
_LLCF: goto _LLD0;_LLC8: _tmp8F=(void*)(_tmp7B->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpAB;_tmpAB.tag=7;_tmpAB.f1=(void*)_tmpA3;_tmpAB.f2=_tmpA2;_tmpAB.f3=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_arr_size(_tmpA6,sizeof(unsigned char)),0);
_tmpAB;});_tmpAA;})));goto _LLC6;_LLCA: _tmp8F=(void*)(_tmp7B->type=(void*)((void*)({
struct Cyc_Absyn_ArrayType_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({
struct Cyc_Absyn_ArrayType_struct _tmpAD;_tmpAD.tag=7;_tmpAD.f1=(void*)_tmpA3;
_tmpAD.f2=_tmpA2;_tmpAD.f3=(struct Cyc_Absyn_Exp*)_tmpA7;_tmpAD;});_tmpAC;})));
goto _LLC6;_LLCC: _tmpA9=_tmpA8;goto _LLCE;_LLCE: _tmp8F=(void*)(_tmp7B->type=(void*)((
void*)({struct Cyc_Absyn_ArrayType_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));
_tmpAE[0]=({struct Cyc_Absyn_ArrayType_struct _tmpAF;_tmpAF.tag=7;_tmpAF.f1=(void*)
_tmpA3;_tmpAF.f2=_tmpA2;_tmpAF.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA9),0);_tmpAF;});
_tmpAE;})));goto _LLC6;_LLD0: goto _LLC6;_LLC6:;}goto _LLBD;_LLC1: goto _LLBD;_LLBD:;}{
struct Cyc_List_List*_tmpB0=! is_local? 0: Cyc_Tcenv_lookup_type_vars(te);int _tmpB1=
! is_local? 0: 1;Cyc_Tcutil_check_type(s0->loc,te,_tmpB0,(void*)1,_tmpB1,_tmp8F);{
struct Cyc_Tcenv_Tenv*_tmpB2=Cyc_Tcenv_add_local_var(_tmp38->loc,te,_tmp7B);if(
_tmp95 == (void*)3? 1: _tmp95 == (void*)4){({void*_tmpB3[0]={};Cyc_Tcutil_terr(
_tmp39->loc,_tag_arr("local extern declarations not yet supported",sizeof(
unsigned char),44),_tag_arr(_tmpB3,sizeof(void*),0));});}if(_tmp8E != 0){Cyc_Tcexp_tcExpInitializer(
_tmpB2,(void**)& _tmp8F,(struct Cyc_Absyn_Exp*)_check_null(_tmp8E));if(! is_local? !
Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp8E)): 0){({void*
_tmpB4[0]={};Cyc_Tcutil_terr(_tmp39->loc,_tag_arr("initializer needs to be a constant expression",
sizeof(unsigned char),46),_tag_arr(_tmpB4,sizeof(void*),0));});}if(! Cyc_Tcutil_coerce_assign(
_tmpB2,(struct Cyc_Absyn_Exp*)_check_null(_tmp8E),_tmp8F)){({struct Cyc_Std_String_pa_struct
_tmpB8;_tmpB8.tag=0;_tmpB8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp8E->topt))->v);{struct Cyc_Std_String_pa_struct
_tmpB7;_tmpB7.tag=0;_tmpB7.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp8F);{
struct Cyc_Std_String_pa_struct _tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct _tagged_arr)*
_tmp93;{void*_tmpB5[3]={& _tmpB6,& _tmpB7,& _tmpB8};Cyc_Tcutil_terr(_tmp39->loc,
_tag_arr("%s declared with type \n%s\n but initialized with type \n%s",sizeof(
unsigned char),57),_tag_arr(_tmpB5,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmpB2,s0),_tmp38,0);return;}}}}_LL7D: {
struct Cyc_Core_Opt*_tmpB9=*_tmp80;Cyc_Tcexp_tcExpInitializer(te,0,_tmp7E);*
_tmp80=_tmp7E->topt;{struct Cyc_List_List*_tmpBB;struct Cyc_List_List*_tmpBC;
struct _tuple5 _tmpBA=Cyc_Tcpat_tcPat(te,_tmp83,(void**)((void**)((void*)&((struct
Cyc_Core_Opt*)_check_null(_tmp7E->topt))->v)),0);_LLD8: _tmpBC=_tmpBA.f1;goto
_LLD7;_LLD7: _tmpBB=_tmpBA.f2;goto _LLD6;_LLD6:*_tmp82=({struct Cyc_Core_Opt*_tmpBD=
_cycalloc(sizeof(*_tmpBD));_tmpBD->v=_tmpBB;_tmpBD;});if(_tmpB9 != 0? ! Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp83->topt))->v,(void*)_tmpB9->v): 0){({
void*_tmpBE[0]={};Cyc_Tcutil_terr(_tmp39->loc,_tag_arr("type of pattern does not match declared type",
sizeof(unsigned char),45),_tag_arr(_tmpBE,sizeof(void*),0));});}if(! Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp83->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7E->topt))->v)){({struct Cyc_Std_String_pa_struct _tmpC1;_tmpC1.tag=
0;_tmpC1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7E->topt))->v);{struct Cyc_Std_String_pa_struct _tmpC0;_tmpC0.tag=
0;_tmpC0.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp83->topt))->v);{void*_tmpBF[2]={& _tmpC0,& _tmpC1};Cyc_Tcutil_terr(
_tmp39->loc,_tag_arr("pattern type %s does not match definition type %s",sizeof(
unsigned char),50),_tag_arr(_tmpBF,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(te,_tmp83);}*_tmp7D=Cyc_Tcpat_check_let_pat_exhaustive(
_tmp83->loc,_tmp83);Cyc_Tcstmt_pattern_synth(s0->loc,te,_tmpBC,_tmpBB,_tmp38,0,0);
return;}}_LL7F: {void*_tmpC2=Cyc_Tcenv_curr_rgn(te);struct Cyc_Tcenv_Tenv*_tmpC3=
te;for(0;_tmp84 != 0;_tmp84=_tmp84->tl){struct Cyc_Absyn_Vardecl*_tmpC4=(struct Cyc_Absyn_Vardecl*)
_tmp84->hd;struct Cyc_Absyn_Vardecl _tmpC6;struct Cyc_Core_Opt*_tmpC7;struct Cyc_Core_Opt**
_tmpC8;void*_tmpC9;struct _tuple0*_tmpCA;struct _tuple0 _tmpCB;void*_tmpCC;struct
Cyc_Absyn_Vardecl*_tmpC5=_tmpC4;_tmpC6=*_tmpC5;_LLDC: _tmpCA=_tmpC6.name;_tmpCB=*
_tmpCA;_LLDD: _tmpCC=_tmpCB.f1;goto _LLDB;_LLDB: _tmpC9=(void*)_tmpC6.type;goto
_LLDA;_LLDA: _tmpC7=_tmpC6.rgn;_tmpC8=(struct Cyc_Core_Opt**)&(*_tmpC5).rgn;goto
_LLD9;_LLD9:*_tmpC8=({struct Cyc_Core_Opt*_tmpCD=_cycalloc(sizeof(*_tmpCD));
_tmpCD->v=(void*)_tmpC2;_tmpCD;});{void*_tmpCE=_tmpCC;struct Cyc_List_List*_tmpCF;
struct Cyc_List_List*_tmpD0;_LLDF: if((int)_tmpCE == 0){goto _LLE0;}else{goto _LLE1;}
_LLE1: if((unsigned int)_tmpCE > 1?*((int*)_tmpCE)== 0: 0){_LLE7: _tmpCF=((struct Cyc_Absyn_Rel_n_struct*)
_tmpCE)->f1;if(_tmpCF == 0){goto _LLE2;}else{goto _LLE3;}}else{goto _LLE3;}_LLE3: if((
unsigned int)_tmpCE > 1?*((int*)_tmpCE)== 1: 0){_LLE8: _tmpD0=((struct Cyc_Absyn_Abs_n_struct*)
_tmpCE)->f1;goto _LLE4;}else{goto _LLE5;}_LLE5: goto _LLE6;_LLE0: goto _LLDE;_LLE2:(*
_tmpC4->name).f1=(void*)0;goto _LLDE;_LLE4:(int)_throw(({void*_tmpD1[0]={};Cyc_Tcutil_impos(
_tag_arr("tcstmt: Abs_n declaration",sizeof(unsigned char),26),_tag_arr(_tmpD1,
sizeof(void*),0));}));_LLE6:({void*_tmpD2[0]={};Cyc_Tcutil_terr(_tmp39->loc,
_tag_arr("cannot declare a qualified local variable",sizeof(unsigned char),42),
_tag_arr(_tmpD2,sizeof(void*),0));});goto _LLDE;_LLDE:;}Cyc_Tcutil_check_type(s0->loc,
_tmpC3,Cyc_Tcenv_lookup_type_vars(_tmpC3),(void*)1,1,_tmpC9);_tmpC3=Cyc_Tcenv_add_local_var(
_tmp38->loc,_tmpC3,_tmpC4);}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmpC3,s0),
_tmp38,0);return;}_LL81: unimp_msg_part=_tag_arr("namespace",sizeof(unsigned char),
10);goto _LL79;_LL83: unimp_msg_part=_tag_arr("using",sizeof(unsigned char),6);
goto _LL79;_LL85: unimp_msg_part=_tag_arr("function",sizeof(unsigned char),9);goto
_LL79;_LL87: unimp_msg_part=_tag_arr("struct",sizeof(unsigned char),7);goto _LL79;
_LL89: unimp_msg_part=_tag_arr("union",sizeof(unsigned char),6);goto _LL79;_LL8B:
unimp_msg_part=_tag_arr("[x]tunion",sizeof(unsigned char),10);goto _LL79;_LL8D:
unimp_msg_part=_tag_arr("enum",sizeof(unsigned char),5);goto _LL79;_LL8F:
unimp_msg_part=_tag_arr("typedef",sizeof(unsigned char),8);goto _LL79;_LL91:
unimp_msg_part=_tag_arr("extern \"C\"",sizeof(unsigned char),11);goto _LL79;_LL79:;}(
int)_throw(({struct Cyc_Std_String_pa_struct _tmpD4;_tmpD4.tag=0;_tmpD4.f1=(struct
_tagged_arr)unimp_msg_part;{void*_tmpD3[1]={& _tmpD4};Cyc_Tcutil_impos(_tag_arr("tcStmt: nested %s declarations unimplemented",
sizeof(unsigned char),45),_tag_arr(_tmpD3,sizeof(void*),1));}}));}_LL26:(void*)(
_tmp3A->r=(void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Absyn_new_stmt((void*)_tmp3A->r,
_tmp3A->loc),Cyc_Absyn_skip_stmt(_tmp3A->loc),_tmp3A->loc))->r));if(new_block){
te=Cyc_Tcenv_new_block(s0->loc,te);}{struct Cyc_Absyn_Vardecl _tmpD6;struct Cyc_Core_Opt*
_tmpD7;struct Cyc_Core_Opt**_tmpD8;void*_tmpD9;struct Cyc_Absyn_Vardecl*_tmpD5=
_tmp3B;_tmpD6=*_tmpD5;_LLEB: _tmpD9=(void*)_tmpD6.type;goto _LLEA;_LLEA: _tmpD7=
_tmpD6.rgn;_tmpD8=(struct Cyc_Core_Opt**)&(*_tmpD5).rgn;goto _LLE9;_LLE9: {void*
_tmpDA=Cyc_Tcenv_curr_rgn(te);void*rgn_typ=(void*)({struct Cyc_Absyn_VarType_struct*
_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_VarType_struct
_tmpE1;_tmpE1.tag=1;_tmpE1.f1=_tmp3C;_tmpE1;});_tmpE0;});*_tmpD8=({struct Cyc_Core_Opt*
_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->v=(void*)_tmpDA;_tmpDB;});te=Cyc_Tcenv_add_type_vars(
s0->loc,te,({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->hd=
_tmp3C;_tmpDC->tl=0;_tmpDC;}));Cyc_Tcutil_check_type(s0->loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)2,1,_tmpD9);te=Cyc_Tcenv_add_region(te,rgn_typ);if(! Cyc_Tcutil_unify(
_tmpD9,(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpDD=_cycalloc(sizeof(*
_tmpDD));_tmpDD[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpDE;_tmpDE.tag=17;
_tmpDE.f1=(void*)rgn_typ;_tmpDE;});_tmpDD;}))){({void*_tmpDF[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("region stmt: type of region handle is wrong!",
sizeof(unsigned char),45),_tag_arr(_tmpDF,sizeof(void*),0));});}Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(Cyc_Tcenv_add_local_var(_tmp3A->loc,Cyc_Tcenv_enter_try(
te),_tmp3B),s0),_tmp3A,0);return;}}_LL28:(int)_throw(({void*_tmpE2[0]={};Cyc_Tcutil_impos(
_tag_arr("tcStmt: cut is not implemented",sizeof(unsigned char),31),_tag_arr(
_tmpE2,sizeof(void*),0));}));_LL2A:(int)_throw(({void*_tmpE3[0]={};Cyc_Tcutil_impos(
_tag_arr("tcStmt: splice is not implemented",sizeof(unsigned char),34),_tag_arr(
_tmpE3,sizeof(void*),0));}));_LL0:;}}
