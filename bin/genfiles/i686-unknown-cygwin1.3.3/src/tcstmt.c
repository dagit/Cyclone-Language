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
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);

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
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern unsigned char Cyc_List_Nth[8];
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;
void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
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
f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_ResetRegion_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
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
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;};struct Cyc_Absyn_Aggrdecl{void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_exn_typ;
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,
struct Cyc_Position_Segment*s);struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,
struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_PP_Ppstate;struct
Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int
qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
struct _tuple3{void*f1;void*f2;};struct _tuple3*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple3*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*
effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct
Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;
};struct Cyc_Tcenv_Frame_struct{int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_List_List*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*
clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_enter_try(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _tagged_arr*,struct Cyc_Absyn_Stmt**);
struct _tuple4{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*cs);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*r,int resetable);void Cyc_Tcenv_check_rgn_resetable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
int*Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _tagged_arr msg_part);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(
te));s->try_depth=Cyc_Tcenv_get_try_depth(te);}static void Cyc_Tcstmt_pattern_synth(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult
pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
struct _tuple6*_tmp1;struct Cyc_List_List*_tmp2;struct Cyc_Tcpat_TcPatResult _tmp0=
pat_res;_tmp1=_tmp0.tvars_and_bounds_opt;_tmp2=_tmp0.patvars;{struct Cyc_List_List*
_tmp3=_tmp1 == 0?0:(*_tmp1).f1;struct Cyc_List_List*_tmp4=_tmp1 == 0?0:(*_tmp1).f2;
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,_tmp3);te2=Cyc_Tcenv_new_outlives_constraints(
te2,_tmp4);if(new_block)te2=Cyc_Tcenv_new_block(loc,te2);{struct Cyc_Core_Opt*
_tmp5=({struct Cyc_Core_Opt*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->v=(void*)Cyc_Tcenv_curr_rgn(
te2);_tmp7;});{struct Cyc_List_List*_tmp6=_tmp2;for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
te2=Cyc_Tcenv_add_pat_var(loc,te2,(struct Cyc_Absyn_Vardecl*)_tmp6->hd);((struct
Cyc_Absyn_Vardecl*)_tmp6->hd)->rgn=_tmp5;}}if(where_opt != 0)Cyc_Tcexp_tcTest(te2,(
struct Cyc_Absyn_Exp*)_check_null(where_opt),_tag_arr("switch clause guard",
sizeof(unsigned char),20));if(_tmp2 != 0)te2=Cyc_Tcenv_set_encloser(te2,s);Cyc_Tcstmt_tcStmt(
te2,s,0);if(_tmp2 != 0)Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(
te));}}}struct _tuple7{struct Cyc_Tcenv_Tenv*f1;struct Cyc_List_List*f2;};struct
_tuple7 Cyc_Tcstmt_check_forarray_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*decls){({void*_tmp8[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_forarray_decls",sizeof(
unsigned char),21),_tag_arr(_tmp8,sizeof(void*),0));});return({struct _tuple7
_tmp9;_tmp9.f1=te;_tmp9.f2=0;_tmp9;});}void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s0,int new_block){Cyc_Tcstmt_decorate_stmt(te,s0);{void*
_tmpA=(void*)s0->r;struct Cyc_Absyn_Exp*_tmpB;struct Cyc_Absyn_Stmt*_tmpC;struct
Cyc_Absyn_Stmt*_tmpD;struct Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmpF;struct
Cyc_Absyn_Stmt*_tmp10;struct Cyc_Absyn_Stmt*_tmp11;struct _tuple2 _tmp12;struct Cyc_Absyn_Exp*
_tmp13;struct Cyc_Absyn_Stmt*_tmp14;struct Cyc_Absyn_Stmt*_tmp15;struct Cyc_Absyn_Exp*
_tmp16;struct _tuple2 _tmp17;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Stmt*
_tmp19;struct _tuple2 _tmp1A;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Stmt*
_tmp1C;struct Cyc_Absyn_Stmt*_tmp1D;struct Cyc_Absyn_ForArrayInfo _tmp1E;struct Cyc_List_List*
_tmp1F;struct _tuple2 _tmp20;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Stmt*
_tmp22;struct _tuple2 _tmp23;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Stmt*
_tmp25;struct Cyc_Absyn_Stmt*_tmp26;struct Cyc_Absyn_Stmt*_tmp27;struct _tuple2
_tmp28;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Stmt*_tmp2A;struct Cyc_Absyn_Stmt*
_tmp2B;struct Cyc_Absyn_Stmt**_tmp2C;struct Cyc_Absyn_Stmt*_tmp2D;struct Cyc_Absyn_Stmt**
_tmp2E;struct _tagged_arr*_tmp2F;struct Cyc_Absyn_Stmt*_tmp30;struct Cyc_Absyn_Stmt**
_tmp31;struct Cyc_List_List*_tmp32;struct Cyc_Absyn_Switch_clause**_tmp33;struct
Cyc_Absyn_Switch_clause***_tmp34;struct _tagged_arr*_tmp35;struct Cyc_Absyn_Stmt*
_tmp36;struct Cyc_Absyn_Exp*_tmp37;struct Cyc_List_List*_tmp38;struct Cyc_Absyn_Exp*
_tmp39;struct Cyc_List_List*_tmp3A;struct Cyc_Absyn_Stmt*_tmp3B;struct Cyc_List_List*
_tmp3C;struct Cyc_Absyn_Decl*_tmp3D;struct Cyc_Absyn_Stmt*_tmp3E;struct Cyc_Absyn_Tvar*
_tmp3F;struct Cyc_Absyn_Vardecl*_tmp40;int _tmp41;struct Cyc_Absyn_Stmt*_tmp42;
struct Cyc_Absyn_Exp*_tmp43;struct Cyc_Absyn_Stmt*_tmp44;struct Cyc_Absyn_Stmt*
_tmp45;_LL1: if((int)_tmpA != 0)goto _LL3;_LL2: return;_LL3: if(_tmpA <= (void*)1?1:*((
int*)_tmpA)!= 0)goto _LL5;_tmpB=((struct Cyc_Absyn_Exp_s_struct*)_tmpA)->f1;_LL4:
Cyc_Tcexp_tcExp(te,0,_tmpB);return;_LL5: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 1)
goto _LL7;_tmpC=((struct Cyc_Absyn_Seq_s_struct*)_tmpA)->f1;_tmpD=((struct Cyc_Absyn_Seq_s_struct*)
_tmpA)->f2;_LL6: {struct Cyc_Tcenv_Tenv*_tmp46=Cyc_Tcenv_set_next(te,(void*)({
struct Cyc_Tcenv_Stmt_j_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({
struct Cyc_Tcenv_Stmt_j_struct _tmp4D;_tmp4D.tag=0;_tmp4D.f1=_tmpD;_tmp4D;});
_tmp4C;}));Cyc_Tcstmt_tcStmt(_tmp46,_tmpC,1);while(1){void*_tmp47=(void*)_tmpD->r;
struct Cyc_Absyn_Stmt*_tmp48;struct Cyc_Absyn_Stmt*_tmp49;_LL2E: if(_tmp47 <= (void*)
1?1:*((int*)_tmp47)!= 1)goto _LL30;_tmp48=((struct Cyc_Absyn_Seq_s_struct*)_tmp47)->f1;
_tmp49=((struct Cyc_Absyn_Seq_s_struct*)_tmp47)->f2;_LL2F: Cyc_Tcstmt_decorate_stmt(
te,_tmpD);_tmp46=Cyc_Tcenv_set_next(te,(void*)({struct Cyc_Tcenv_Stmt_j_struct*
_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Tcenv_Stmt_j_struct
_tmp4B;_tmp4B.tag=0;_tmp4B.f1=_tmp49;_tmp4B;});_tmp4A;}));Cyc_Tcstmt_tcStmt(
_tmp46,_tmp48,1);_tmpD=_tmp49;continue;_LL30:;_LL31: goto seq_end;_LL2D:;}seq_end:
Cyc_Tcstmt_tcStmt(te,_tmpD,1);return;}_LL7: if(_tmpA <= (void*)1?1:*((int*)_tmpA)
!= 2)goto _LL9;_tmpE=((struct Cyc_Absyn_Return_s_struct*)_tmpA)->f1;_LL8: {void*t=
Cyc_Tcenv_return_typ(te);if(_tmpE == 0){void*_tmp4E=Cyc_Tcutil_compress(t);_LL33:
if((int)_tmp4E != 0)goto _LL35;_LL34: goto _LL32;_LL35:;_LL36:({struct Cyc_Std_String_pa_struct
_tmp50;_tmp50.tag=0;_tmp50.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmp4F[1]={& _tmp50};Cyc_Tcutil_terr(s0->loc,_tag_arr("must return a value of type %s",
sizeof(unsigned char),31),_tag_arr(_tmp4F,sizeof(void*),1));}});goto _LL32;_LL32:;}
else{struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmpE);Cyc_Tcexp_tcExp(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct
_tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp51[2]={& _tmp52,& _tmp53};Cyc_Tcutil_terr(s0->loc,_tag_arr("returns value of type %s but requires %s",
sizeof(unsigned char),41),_tag_arr(_tmp51,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
return;}_LL9: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 3)goto _LLB;_tmpF=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpA)->f1;_tmp10=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpA)->f2;_tmp11=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmpA)->f3;_LLA: Cyc_Tcexp_tcTest(te,_tmpF,
_tag_arr("if statement",sizeof(unsigned char),13));Cyc_Tcstmt_tcStmt(te,_tmp10,1);
Cyc_Tcstmt_tcStmt(te,_tmp11,1);return;_LLB: if(_tmpA <= (void*)1?1:*((int*)_tmpA)
!= 4)goto _LLD;_tmp12=((struct Cyc_Absyn_While_s_struct*)_tmpA)->f1;_tmp13=_tmp12.f1;
_tmp14=_tmp12.f2;_tmp15=((struct Cyc_Absyn_While_s_struct*)_tmpA)->f2;_LLC: Cyc_Tcstmt_decorate_stmt(
te,_tmp14);Cyc_Tcexp_tcTest(te,_tmp13,_tag_arr("while loop",sizeof(unsigned char),
11));Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(te,_tmp14),_tmp15,1);return;_LLD:
if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 8)goto _LLF;_tmp16=((struct Cyc_Absyn_For_s_struct*)
_tmpA)->f1;_tmp17=((struct Cyc_Absyn_For_s_struct*)_tmpA)->f2;_tmp18=_tmp17.f1;
_tmp19=_tmp17.f2;_tmp1A=((struct Cyc_Absyn_For_s_struct*)_tmpA)->f3;_tmp1B=_tmp1A.f1;
_tmp1C=_tmp1A.f2;_tmp1D=((struct Cyc_Absyn_For_s_struct*)_tmpA)->f4;_LLE: Cyc_Tcstmt_decorate_stmt(
te,_tmp19);Cyc_Tcstmt_decorate_stmt(te,_tmp1C);Cyc_Tcexp_tcExp(te,0,_tmp16);Cyc_Tcexp_tcTest(
te,_tmp18,_tag_arr("for loop",sizeof(unsigned char),9));te=Cyc_Tcenv_set_in_loop(
te,_tmp1C);Cyc_Tcstmt_tcStmt(te,_tmp1D,1);Cyc_Tcexp_tcExp(te,0,_tmp1B);return;
_LLF: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 19)goto _LL11;_tmp1E=((struct Cyc_Absyn_ForArray_s_struct*)
_tmpA)->f1;_tmp1F=_tmp1E.defns;_tmp20=_tmp1E.condition;_tmp21=_tmp20.f1;_tmp22=
_tmp20.f2;_tmp23=_tmp1E.delta;_tmp24=_tmp23.f1;_tmp25=_tmp23.f2;_tmp26=_tmp1E.body;
_LL10:({void*_tmp54[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("ForArray_s",sizeof(unsigned char),11),_tag_arr(_tmp54,sizeof(void*),0));});
return;_LL11: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 16)goto _LL13;_tmp27=((struct
Cyc_Absyn_Do_s_struct*)_tmpA)->f1;_tmp28=((struct Cyc_Absyn_Do_s_struct*)_tmpA)->f2;
_tmp29=_tmp28.f1;_tmp2A=_tmp28.f2;_LL12: Cyc_Tcstmt_decorate_stmt(te,_tmp2A);Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop(te,_tmp2A),_tmp27,1);Cyc_Tcexp_tcTest(te,_tmp29,_tag_arr("do loop",
sizeof(unsigned char),8));return;_LL13: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 5)
goto _LL15;_tmp2B=((struct Cyc_Absyn_Break_s_struct*)_tmpA)->f1;_tmp2C=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Break_s_struct*)_tmpA)->f1;_LL14: Cyc_Tcenv_process_break(te,s0,
_tmp2C);return;_LL15: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 6)goto _LL17;_tmp2D=((
struct Cyc_Absyn_Continue_s_struct*)_tmpA)->f1;_tmp2E=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Continue_s_struct*)_tmpA)->f1;_LL16: Cyc_Tcenv_process_continue(
te,s0,_tmp2E);return;_LL17: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 7)goto _LL19;
_tmp2F=((struct Cyc_Absyn_Goto_s_struct*)_tmpA)->f1;_tmp30=((struct Cyc_Absyn_Goto_s_struct*)
_tmpA)->f2;_tmp31=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)
_tmpA)->f2;_LL18: Cyc_Tcenv_process_goto(te,s0,_tmp2F,_tmp31);return;_LL19: if(
_tmpA <= (void*)1?1:*((int*)_tmpA)!= 11)goto _LL1B;_tmp32=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmpA)->f1;_tmp33=((struct Cyc_Absyn_Fallthru_s_struct*)_tmpA)->f2;_tmp34=(struct
Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_struct*)_tmpA)->f2;
_LL1A: {struct _tuple4*_tmp55=Cyc_Tcenv_process_fallthru(te,s0,_tmp34);if(_tmp55
== 0){({void*_tmp56[0]={};Cyc_Tcutil_terr(s0->loc,_tag_arr("fallthru not in a non-last case",
sizeof(unsigned char),32),_tag_arr(_tmp56,sizeof(void*),0));});return;}{struct
_tuple4 _tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List*_tmp5A;struct
_tuple4*_tmp57=(struct _tuple4*)_check_null(_tmp55);_tmp58=*_tmp57;_tmp59=_tmp58.f2;
_tmp5A=_tmp58.f3;{struct Cyc_List_List*instantiation=((struct Cyc_List_List*(*)(
struct _tuple5*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(
te),_tmp59);_tmp5A=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,
instantiation,_tmp5A);for(0;_tmp5A != 0?_tmp32 != 0: 0;(_tmp5A=_tmp5A->tl,_tmp32=
_tmp32->tl)){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp32->hd);if(!Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)_tmp32->hd,(void*)_tmp5A->hd)){({struct Cyc_Std_String_pa_struct
_tmp5D;_tmp5D.tag=0;_tmp5D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)
_tmp5A->hd);{struct Cyc_Std_String_pa_struct _tmp5C;_tmp5C.tag=0;_tmp5C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)_tmp32->hd)->topt))->v);{void*_tmp5B[2]={& _tmp5C,& _tmp5D};
Cyc_Tcutil_terr(s0->loc,_tag_arr("fallthru argument has type %s but pattern variable has type %s",
sizeof(unsigned char),63),_tag_arr(_tmp5B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
if(_tmp32 != 0)({void*_tmp5E[0]={};Cyc_Tcutil_terr(s0->loc,_tag_arr("too many arguments to explicit fallthru",
sizeof(unsigned char),40),_tag_arr(_tmp5E,sizeof(void*),0));});if(_tmp5A != 0)({
void*_tmp5F[0]={};Cyc_Tcutil_terr(s0->loc,_tag_arr("too few arguments to explicit fallthru",
sizeof(unsigned char),39),_tag_arr(_tmp5F,sizeof(void*),0));});return;}}}_LL1B:
if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 15)goto _LL1D;_tmp35=((struct Cyc_Absyn_Label_s_struct*)
_tmpA)->f1;_tmp36=((struct Cyc_Absyn_Label_s_struct*)_tmpA)->f2;_LL1C: Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_new_named_block(s0->loc,Cyc_Tcenv_add_label(te,_tmp35,_tmp36),({struct
Cyc_Absyn_Tvar*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->name=({struct
_tagged_arr*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp65;_tmp65.tag=0;_tmp65.f1=(struct _tagged_arr)*
_tmp35;{void*_tmp64[1]={& _tmp65};Cyc_Std_aprintf(_tag_arr("`%s",sizeof(
unsigned char),4),_tag_arr(_tmp64,sizeof(void*),1));}});_tmp63;});_tmp60->identity=
Cyc_Tcutil_new_tvar_id();_tmp60->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp62;
_tmp62.tag=0;_tmp62.f1=(void*)((void*)3);_tmp62;});_tmp61;}));_tmp60;})),_tmp36,
0);return;_LL1D: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 9)goto _LL1F;_tmp37=((
struct Cyc_Absyn_Switch_s_struct*)_tmpA)->f1;_tmp38=((struct Cyc_Absyn_Switch_s_struct*)
_tmpA)->f2;_LL1E: Cyc_Tcexp_tcExp(te,0,_tmp37);{void*_tmp66=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp37->topt))->v;te=Cyc_Tcenv_set_in_switch(te);te=Cyc_Tcenv_clear_fallthru(
te);{struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp38);for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Pat*_tmp67=((
struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Tcpat_TcPatResult
_tmp68=Cyc_Tcpat_tcPat(te,_tmp67,(void**)& _tmp66);struct Cyc_List_List*_tmp69=
_tmp68.tvars_and_bounds_opt == 0?0:(*_tmp68.tvars_and_bounds_opt).f1;if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp67->topt))->v,_tmp66)){({struct Cyc_Std_String_pa_struct
_tmp6C;_tmp6C.tag=0;_tmp6C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp67->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp6B;_tmp6B.tag=0;_tmp6B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp66);{
void*_tmp6A[2]={& _tmp6B,& _tmp6C};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,_tag_arr("switch on type %s, but case expects type %s",sizeof(
unsigned char),44),_tag_arr(_tmp6A,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(te,_tmp67);}((struct Cyc_Absyn_Switch_clause*)
scs->hd)->pat_vars=({struct Cyc_Core_Opt*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->v=
_tmp68.patvars;_tmp6D;});Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,te,_tmp68,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct
Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);if(_tmp68.tvars_and_bounds_opt
!= 0?(*_tmp68.tvars_and_bounds_opt).f2 != 0: 0)te=Cyc_Tcenv_clear_fallthru(te);
else{te=Cyc_Tcenv_set_fallthru(te,_tmp69,_tmp68.patvars,(struct Cyc_Absyn_Switch_clause*)
scs->hd);}}}Cyc_Tcpat_check_switch_exhaustive(s0->loc,_tmp38);return;}_LL1F: if(
_tmpA <= (void*)1?1:*((int*)_tmpA)!= 10)goto _LL21;_tmp39=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmpA)->f1;_tmp3A=((struct Cyc_Absyn_SwitchC_s_struct*)_tmpA)->f2;_LL20: {struct
Cyc_List_List*_tmp6E=0;for(0;_tmp3A != 0;_tmp3A=_tmp3A->tl){struct Cyc_Absyn_SwitchC_clause
_tmp70;struct Cyc_Absyn_Exp*_tmp71;struct Cyc_Absyn_Stmt*_tmp72;struct Cyc_Position_Segment*
_tmp73;struct Cyc_Absyn_SwitchC_clause*_tmp6F=(struct Cyc_Absyn_SwitchC_clause*)
_tmp3A->hd;_tmp70=*_tmp6F;_tmp71=_tmp70.cnst_exp;_tmp72=_tmp70.body;_tmp73=
_tmp70.loc;{struct Cyc_Absyn_Pat*pat;if(_tmp71 != 0){Cyc_Tcexp_tcExp(te,0,(struct
Cyc_Absyn_Exp*)_check_null(_tmp71));pat=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Absyn_Int_p_struct _tmp75;
_tmp75.tag=1;_tmp75.f1=(void*)((void*)0);_tmp75.f2=(int)Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp71));_tmp75;});_tmp74;}),_tmp71->loc);}
else{pat=Cyc_Absyn_new_pat((void*)0,_tmp73);}_tmp6E=({struct Cyc_List_List*_tmp76=
_cycalloc(sizeof(*_tmp76));_tmp76->hd=({struct Cyc_Absyn_Switch_clause*_tmp77=
_cycalloc(sizeof(*_tmp77));_tmp77->pattern=pat;_tmp77->pat_vars=0;_tmp77->where_clause=
0;_tmp77->body=_tmp72;_tmp77->loc=_tmp73;_tmp77;});_tmp76->tl=_tmp6E;_tmp76;});}}(
void*)(s0->r=(void*)((void*)(Cyc_Absyn_switch_stmt(_tmp39,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6E),0))->r));Cyc_Tcstmt_tcStmt(te,
s0,new_block);return;}_LL21: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 17)goto _LL23;
_tmp3B=((struct Cyc_Absyn_TryCatch_s_struct*)_tmpA)->f1;_tmp3C=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmpA)->f2;_LL22:(void*)(_tmp3B->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Seq_s_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=({
struct Cyc_Absyn_Seq_s_struct _tmp79;_tmp79.tag=1;_tmp79.f1=Cyc_Absyn_new_stmt((
void*)_tmp3B->r,_tmp3B->loc);_tmp79.f2=Cyc_Absyn_skip_stmt(_tmp3B->loc);_tmp79;});
_tmp78;}),_tmp3B->loc))->r));Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(Cyc_Tcenv_enter_try(
te),s0),_tmp3B,1);te=Cyc_Tcenv_set_in_switch(te);te=Cyc_Tcenv_clear_fallthru(te);{
struct Cyc_List_List*_tmp7A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
_tmp3C);for(0;_tmp7A != 0;_tmp7A=_tmp7A->tl){struct Cyc_Absyn_Pat*_tmp7B=((struct
Cyc_Absyn_Switch_clause*)_tmp7A->hd)->pattern;struct Cyc_Tcpat_TcPatResult _tmp7C=
Cyc_Tcpat_tcPat(te,_tmp7B,(void**)& Cyc_Absyn_exn_typ);struct Cyc_List_List*_tmp7D=
_tmp7C.tvars_and_bounds_opt == 0?0:(*_tmp7C.tvars_and_bounds_opt).f1;if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7B->topt))->v,Cyc_Absyn_exn_typ))({
struct Cyc_Std_String_pa_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7B->topt))->v);{
void*_tmp7E[1]={& _tmp7F};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmp7A->hd)->loc,
_tag_arr("expected xtunion exn but found %s",sizeof(unsigned char),34),_tag_arr(
_tmp7E,sizeof(void*),1));}});else{Cyc_Tcpat_check_pat_regions(te,_tmp7B);}((
struct Cyc_Absyn_Switch_clause*)_tmp7A->hd)->pat_vars=({struct Cyc_Core_Opt*_tmp80=
_cycalloc(sizeof(*_tmp80));_tmp80->v=_tmp7C.patvars;_tmp80;});Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)_tmp7A->hd)->loc,te,_tmp7C,((struct Cyc_Absyn_Switch_clause*)
_tmp7A->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp7A->hd)->where_clause,1);
if(_tmp7C.tvars_and_bounds_opt != 0?(*_tmp7C.tvars_and_bounds_opt).f2 != 0: 0)te=
Cyc_Tcenv_clear_fallthru(te);else{te=Cyc_Tcenv_set_fallthru(te,_tmp7D,_tmp7C.patvars,(
struct Cyc_Absyn_Switch_clause*)_tmp7A->hd);}}}Cyc_Tcpat_check_catch_overlap(s0->loc,
_tmp3C);return;_LL23: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 12)goto _LL25;_tmp3D=((
struct Cyc_Absyn_Decl_s_struct*)_tmpA)->f1;_tmp3E=((struct Cyc_Absyn_Decl_s_struct*)
_tmpA)->f2;_LL24: {struct _tagged_arr unimp_msg_part;if(new_block)te=Cyc_Tcenv_new_block(
s0->loc,te);{void*_tmp81=(void*)_tmp3D->r;struct Cyc_Absyn_Vardecl*_tmp82;struct
Cyc_Absyn_Pat*_tmp83;struct Cyc_Core_Opt*_tmp84;struct Cyc_Core_Opt**_tmp85;struct
Cyc_Absyn_Exp*_tmp86;struct Cyc_List_List*_tmp87;struct _tagged_arr*_tmp88;struct
Cyc_List_List*_tmp89;struct _tuple0*_tmp8A;struct Cyc_List_List*_tmp8B;_LL38: if(*((
int*)_tmp81)!= 0)goto _LL3A;_tmp82=((struct Cyc_Absyn_Var_d_struct*)_tmp81)->f1;
_LL39: {struct Cyc_Absyn_Vardecl _tmp8D;void*_tmp8E;struct _tuple0*_tmp8F;struct
_tuple0 _tmp90;void*_tmp91;struct _tagged_arr*_tmp92;struct Cyc_Absyn_Tqual _tmp93;
void*_tmp94;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Core_Opt*_tmp96;struct Cyc_Core_Opt**
_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Vardecl*_tmp8C=_tmp82;_tmp8D=*
_tmp8C;_tmp8E=(void*)_tmp8D.sc;_tmp8F=_tmp8D.name;_tmp90=*_tmp8F;_tmp91=_tmp90.f1;
_tmp92=_tmp90.f2;_tmp93=_tmp8D.tq;_tmp94=(void*)_tmp8D.type;_tmp95=_tmp8D.initializer;
_tmp96=_tmp8D.rgn;_tmp97=(struct Cyc_Core_Opt**)&(*_tmp8C).rgn;_tmp98=_tmp8D.attributes;{
void*_tmp99=Cyc_Tcenv_curr_rgn(te);int is_local;{void*_tmp9A=_tmp8E;_LL4F: if((int)
_tmp9A != 0)goto _LL51;_LL50: goto _LL52;_LL51: if((int)_tmp9A != 3)goto _LL53;_LL52:
goto _LL54;_LL53: if((int)_tmp9A != 4)goto _LL55;_LL54: is_local=0;goto _LL4E;_LL55:
if((int)_tmp9A != 1)goto _LL57;_LL56:({void*_tmp9B[0]={};Cyc_Tcutil_terr(_tmp3D->loc,
_tag_arr("bad abstract scope for local variable",sizeof(unsigned char),38),
_tag_arr(_tmp9B,sizeof(void*),0));});goto _LL58;_LL57: if((int)_tmp9A != 5)goto
_LL59;_LL58: goto _LL5A;_LL59: if((int)_tmp9A != 2)goto _LL4E;_LL5A: is_local=1;goto
_LL4E;_LL4E:;}*_tmp97=is_local?({struct Cyc_Core_Opt*_tmp9C=_cycalloc(sizeof(*
_tmp9C));_tmp9C->v=(void*)_tmp99;_tmp9C;}):({struct Cyc_Core_Opt*_tmp9D=_cycalloc(
sizeof(*_tmp9D));_tmp9D->v=(void*)((void*)2);_tmp9D;});{void*_tmp9E=_tmp91;
struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmpA0;_LL5C: if((int)_tmp9E != 0)
goto _LL5E;_LL5D: goto _LL5B;_LL5E: if(_tmp9E <= (void*)1?1:*((int*)_tmp9E)!= 0)goto
_LL60;_tmp9F=((struct Cyc_Absyn_Rel_n_struct*)_tmp9E)->f1;if(_tmp9F != 0)goto _LL60;
_LL5F:(*_tmp82->name).f1=(void*)0;goto _LL5B;_LL60: if(_tmp9E <= (void*)1?1:*((int*)
_tmp9E)!= 1)goto _LL62;_tmpA0=((struct Cyc_Absyn_Abs_n_struct*)_tmp9E)->f1;_LL61:(
int)_throw(({void*_tmpA1[0]={};Cyc_Tcutil_impos(_tag_arr("tcstmt: Abs_n declaration",
sizeof(unsigned char),26),_tag_arr(_tmpA1,sizeof(void*),0));}));_LL62:;_LL63:({
void*_tmpA2[0]={};Cyc_Tcutil_terr(_tmp3D->loc,_tag_arr("cannot declare a qualified local variable",
sizeof(unsigned char),42),_tag_arr(_tmpA2,sizeof(void*),0));});goto _LL5B;_LL5B:;}{
void*_tmpA3=Cyc_Tcutil_compress(_tmp94);void*_tmpA4;struct Cyc_Absyn_Tqual _tmpA5;
struct Cyc_Absyn_Exp*_tmpA6;_LL65: if(_tmpA3 <= (void*)3?1:*((int*)_tmpA3)!= 7)goto
_LL67;_tmpA4=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpA3)->f1;_tmpA5=((
struct Cyc_Absyn_ArrayType_struct*)_tmpA3)->f2;_tmpA6=((struct Cyc_Absyn_ArrayType_struct*)
_tmpA3)->f3;if(_tmpA6 != 0)goto _LL67;if(!(_tmp82->initializer != 0))goto _LL67;
_LL66:{void*_tmpA7=(void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp82->initializer))->r;
void*_tmpA8;struct _tagged_arr _tmpA9;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_List_List*
_tmpAB;struct Cyc_List_List*_tmpAC;_LL6A: if(*((int*)_tmpA7)!= 0)goto _LL6C;_tmpA8=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmpA7)->f1;if(_tmpA8 <= (void*)1?1:*((
int*)_tmpA8)!= 5)goto _LL6C;_tmpA9=((struct Cyc_Absyn_String_c_struct*)_tmpA8)->f1;
_LL6B: _tmp94=(void*)(_tmp82->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpAE;_tmpAE.tag=7;_tmpAE.f1=(void*)_tmpA4;_tmpAE.f2=_tmpA5;_tmpAE.f3=(struct
Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_arr_size(_tmpA9,sizeof(unsigned char)),0);
_tmpAE;});_tmpAD;})));goto _LL69;_LL6C: if(*((int*)_tmpA7)!= 27)goto _LL6E;_tmpAA=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpA7)->f2;_LL6D: _tmp94=(void*)(_tmp82->type=(
void*)((void*)({struct Cyc_Absyn_ArrayType_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));
_tmpAF[0]=({struct Cyc_Absyn_ArrayType_struct _tmpB0;_tmpB0.tag=7;_tmpB0.f1=(void*)
_tmpA4;_tmpB0.f2=_tmpA5;_tmpB0.f3=(struct Cyc_Absyn_Exp*)_tmpAA;_tmpB0;});_tmpAF;})));
goto _LL69;_LL6E: if(*((int*)_tmpA7)!= 34)goto _LL70;_tmpAB=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpA7)->f2;_LL6F: _tmpAC=_tmpAB;goto _LL71;_LL70: if(*((int*)_tmpA7)!= 26)goto
_LL72;_tmpAC=((struct Cyc_Absyn_Array_e_struct*)_tmpA7)->f1;_LL71: _tmp94=(void*)(
_tmp82->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*_tmpB1=_cycalloc(
sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_ArrayType_struct _tmpB2;_tmpB2.tag=7;
_tmpB2.f1=(void*)_tmpA4;_tmpB2.f2=_tmpA5;_tmpB2.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpAC),0);_tmpB2;});
_tmpB1;})));goto _LL69;_LL72:;_LL73: goto _LL69;_LL69:;}goto _LL64;_LL67:;_LL68: goto
_LL64;_LL64:;}{struct Cyc_List_List*_tmpB3=!is_local?0: Cyc_Tcenv_lookup_type_vars(
te);int _tmpB4=!is_local?0: 1;Cyc_Tcutil_check_type(s0->loc,te,_tmpB3,(void*)1,
_tmpB4,_tmp94);{struct Cyc_Tcenv_Tenv*_tmpB5=Cyc_Tcenv_add_local_var(_tmp3E->loc,
te,_tmp82);if(_tmp8E == (void*)3?1: _tmp8E == (void*)4)({void*_tmpB6[0]={};Cyc_Tcutil_terr(
_tmp3D->loc,_tag_arr("local extern declarations not yet supported",sizeof(
unsigned char),44),_tag_arr(_tmpB6,sizeof(void*),0));});if(_tmp95 != 0){Cyc_Tcexp_tcExpInitializer(
_tmpB5,(void**)& _tmp94,(struct Cyc_Absyn_Exp*)_check_null(_tmp95));if(!is_local?!
Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp95)): 0)({void*
_tmpB7[0]={};Cyc_Tcutil_terr(_tmp3D->loc,_tag_arr("initializer needs to be a constant expression",
sizeof(unsigned char),46),_tag_arr(_tmpB7,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_assign(
_tmpB5,(struct Cyc_Absyn_Exp*)_check_null(_tmp95),_tmp94)){({struct Cyc_Std_String_pa_struct
_tmpBB;_tmpBB.tag=0;_tmpBB.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp95->topt))->v);{struct Cyc_Std_String_pa_struct
_tmpBA;_tmpBA.tag=0;_tmpBA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp94);{
struct Cyc_Std_String_pa_struct _tmpB9;_tmpB9.tag=0;_tmpB9.f1=(struct _tagged_arr)*
_tmp92;{void*_tmpB8[3]={& _tmpB9,& _tmpBA,& _tmpBB};Cyc_Tcutil_terr(_tmp3D->loc,
_tag_arr("%s declared with type \n%s\n but initialized with type \n%s",sizeof(
unsigned char),57),_tag_arr(_tmpB8,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmpB5,s0),_tmp3E,0);return;}}}}_LL3A:
if(*((int*)_tmp81)!= 2)goto _LL3C;_tmp83=((struct Cyc_Absyn_Let_d_struct*)_tmp81)->f1;
_tmp84=((struct Cyc_Absyn_Let_d_struct*)_tmp81)->f2;_tmp85=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Let_d_struct*)_tmp81)->f2;_tmp86=((struct Cyc_Absyn_Let_d_struct*)
_tmp81)->f3;_LL3B: Cyc_Tcexp_tcExpInitializer(te,0,_tmp86);{struct Cyc_Tcpat_TcPatResult
_tmpBC=Cyc_Tcpat_tcPat(te,_tmp83,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(_tmp86->topt))->v)));*_tmp85=({struct Cyc_Core_Opt*_tmpBD=_cycalloc(
sizeof(*_tmpBD));_tmpBD->v=_tmpBC.patvars;_tmpBD;});if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp83->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp86->topt))->v)){({struct Cyc_Std_String_pa_struct _tmpC0;_tmpC0.tag=
0;_tmpC0.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp86->topt))->v);{struct Cyc_Std_String_pa_struct _tmpBF;_tmpBF.tag=
0;_tmpBF.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp83->topt))->v);{void*_tmpBE[2]={& _tmpBF,& _tmpC0};Cyc_Tcutil_terr(
_tmp3D->loc,_tag_arr("pattern type %s does not match definition type %s",sizeof(
unsigned char),50),_tag_arr(_tmpBE,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(te,_tmp83);}Cyc_Tcpat_check_let_pat_exhaustive(
_tmp83->loc,_tmp83);Cyc_Tcstmt_pattern_synth(s0->loc,te,_tmpBC,_tmp3E,0,0);
return;}_LL3C: if(*((int*)_tmp81)!= 3)goto _LL3E;_tmp87=((struct Cyc_Absyn_Letv_d_struct*)
_tmp81)->f1;_LL3D: {void*_tmpC1=Cyc_Tcenv_curr_rgn(te);struct Cyc_Tcenv_Tenv*
_tmpC2=te;for(0;_tmp87 != 0;_tmp87=_tmp87->tl){struct Cyc_Absyn_Vardecl*_tmpC3=(
struct Cyc_Absyn_Vardecl*)_tmp87->hd;struct Cyc_Absyn_Vardecl _tmpC5;struct _tuple0*
_tmpC6;struct _tuple0 _tmpC7;void*_tmpC8;void*_tmpC9;struct Cyc_Core_Opt*_tmpCA;
struct Cyc_Core_Opt**_tmpCB;struct Cyc_Absyn_Vardecl*_tmpC4=_tmpC3;_tmpC5=*_tmpC4;
_tmpC6=_tmpC5.name;_tmpC7=*_tmpC6;_tmpC8=_tmpC7.f1;_tmpC9=(void*)_tmpC5.type;
_tmpCA=_tmpC5.rgn;_tmpCB=(struct Cyc_Core_Opt**)&(*_tmpC4).rgn;*_tmpCB=({struct
Cyc_Core_Opt*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->v=(void*)_tmpC1;_tmpCC;});{
void*_tmpCD=_tmpC8;struct Cyc_List_List*_tmpCE;struct Cyc_List_List*_tmpCF;_LL75:
if((int)_tmpCD != 0)goto _LL77;_LL76: goto _LL74;_LL77: if(_tmpCD <= (void*)1?1:*((int*)
_tmpCD)!= 0)goto _LL79;_tmpCE=((struct Cyc_Absyn_Rel_n_struct*)_tmpCD)->f1;if(
_tmpCE != 0)goto _LL79;_LL78:(*_tmpC3->name).f1=(void*)0;goto _LL74;_LL79: if(_tmpCD
<= (void*)1?1:*((int*)_tmpCD)!= 1)goto _LL7B;_tmpCF=((struct Cyc_Absyn_Abs_n_struct*)
_tmpCD)->f1;_LL7A:(int)_throw(({void*_tmpD0[0]={};Cyc_Tcutil_impos(_tag_arr("tcstmt: Abs_n declaration",
sizeof(unsigned char),26),_tag_arr(_tmpD0,sizeof(void*),0));}));_LL7B:;_LL7C:({
void*_tmpD1[0]={};Cyc_Tcutil_terr(_tmp3D->loc,_tag_arr("cannot declare a qualified local variable",
sizeof(unsigned char),42),_tag_arr(_tmpD1,sizeof(void*),0));});goto _LL74;_LL74:;}
Cyc_Tcutil_check_type(s0->loc,_tmpC2,Cyc_Tcenv_lookup_type_vars(_tmpC2),(void*)1,
1,_tmpC9);_tmpC2=Cyc_Tcenv_add_local_var(_tmp3E->loc,_tmpC2,_tmpC3);}Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(_tmpC2,s0),_tmp3E,0);return;}_LL3E: if(*((int*)_tmp81)!= 8)
goto _LL40;_tmp88=((struct Cyc_Absyn_Namespace_d_struct*)_tmp81)->f1;_tmp89=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp81)->f2;_LL3F: unimp_msg_part=_tag_arr("namespace",
sizeof(unsigned char),10);goto _LL37;_LL40: if(*((int*)_tmp81)!= 9)goto _LL42;
_tmp8A=((struct Cyc_Absyn_Using_d_struct*)_tmp81)->f1;_tmp8B=((struct Cyc_Absyn_Using_d_struct*)
_tmp81)->f2;_LL41: unimp_msg_part=_tag_arr("using",sizeof(unsigned char),6);goto
_LL37;_LL42: if(*((int*)_tmp81)!= 1)goto _LL44;_LL43: unimp_msg_part=_tag_arr("function",
sizeof(unsigned char),9);goto _LL37;_LL44: if(*((int*)_tmp81)!= 4)goto _LL46;_LL45:
unimp_msg_part=_tag_arr("type",sizeof(unsigned char),5);goto _LL37;_LL46: if(*((
int*)_tmp81)!= 5)goto _LL48;_LL47: unimp_msg_part=_tag_arr("[x]tunion",sizeof(
unsigned char),10);goto _LL37;_LL48: if(*((int*)_tmp81)!= 6)goto _LL4A;_LL49:
unimp_msg_part=_tag_arr("enum",sizeof(unsigned char),5);goto _LL37;_LL4A: if(*((
int*)_tmp81)!= 7)goto _LL4C;_LL4B: unimp_msg_part=_tag_arr("typedef",sizeof(
unsigned char),8);goto _LL37;_LL4C: if(*((int*)_tmp81)!= 10)goto _LL37;_LL4D:
unimp_msg_part=_tag_arr("extern \"C\"",sizeof(unsigned char),11);goto _LL37;_LL37:;}(
int)_throw(({struct Cyc_Std_String_pa_struct _tmpD3;_tmpD3.tag=0;_tmpD3.f1=(struct
_tagged_arr)unimp_msg_part;{void*_tmpD2[1]={& _tmpD3};Cyc_Tcutil_impos(_tag_arr("tcStmt: nested %s declarations unimplemented",
sizeof(unsigned char),45),_tag_arr(_tmpD2,sizeof(void*),1));}}));}_LL25: if(_tmpA
<= (void*)1?1:*((int*)_tmpA)!= 18)goto _LL27;_tmp3F=((struct Cyc_Absyn_Region_s_struct*)
_tmpA)->f1;_tmp40=((struct Cyc_Absyn_Region_s_struct*)_tmpA)->f2;_tmp41=((struct
Cyc_Absyn_Region_s_struct*)_tmpA)->f3;_tmp42=((struct Cyc_Absyn_Region_s_struct*)
_tmpA)->f4;_LL26:(void*)(_tmp42->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Seq_s_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({
struct Cyc_Absyn_Seq_s_struct _tmpD5;_tmpD5.tag=1;_tmpD5.f1=Cyc_Absyn_new_stmt((
void*)_tmp42->r,_tmp42->loc);_tmpD5.f2=Cyc_Absyn_skip_stmt(_tmp42->loc);_tmpD5;});
_tmpD4;}),_tmp42->loc))->r));if(new_block)te=Cyc_Tcenv_new_block(s0->loc,te);{
struct Cyc_Absyn_Vardecl _tmpD7;void*_tmpD8;struct Cyc_Core_Opt*_tmpD9;struct Cyc_Core_Opt**
_tmpDA;struct Cyc_Absyn_Vardecl*_tmpD6=_tmp40;_tmpD7=*_tmpD6;_tmpD8=(void*)_tmpD7.type;
_tmpD9=_tmpD7.rgn;_tmpDA=(struct Cyc_Core_Opt**)&(*_tmpD6).rgn;{void*_tmpDB=Cyc_Tcenv_curr_rgn(
te);void*rgn_typ=(void*)({struct Cyc_Absyn_VarType_struct*_tmpE1=_cycalloc(
sizeof(*_tmpE1));_tmpE1[0]=({struct Cyc_Absyn_VarType_struct _tmpE2;_tmpE2.tag=1;
_tmpE2.f1=_tmp3F;_tmpE2;});_tmpE1;});*_tmpDA=({struct Cyc_Core_Opt*_tmpDC=
_cycalloc(sizeof(*_tmpDC));_tmpDC->v=(void*)_tmpDB;_tmpDC;});te=Cyc_Tcenv_add_type_vars(
s0->loc,te,({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->hd=
_tmp3F;_tmpDD->tl=0;_tmpDD;}));Cyc_Tcutil_check_type(s0->loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)2,1,_tmpD8);te=Cyc_Tcenv_add_region(te,rgn_typ,_tmp41);if(!Cyc_Tcutil_unify(
_tmpD8,(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpDE=_cycalloc(sizeof(*
_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpDF;_tmpDF.tag=15;
_tmpDF.f1=(void*)rgn_typ;_tmpDF;});_tmpDE;})))({void*_tmpE0[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("region stmt: type of region handle is wrong!",
sizeof(unsigned char),45),_tag_arr(_tmpE0,sizeof(void*),0));});Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(Cyc_Tcenv_add_local_var(_tmp42->loc,Cyc_Tcenv_enter_try(
te),_tmp40),s0),_tmp42,0);return;}}_LL27: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 
20)goto _LL29;_tmp43=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmpA)->f1;_LL28: {
void*rgn_type=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpE7=_cycalloc(sizeof(*
_tmpE7));_tmpE7->v=(void*)((void*)3);_tmpE7;}),({struct Cyc_Core_Opt*_tmpE8=
_cycalloc(sizeof(*_tmpE8));_tmpE8->v=Cyc_Tcenv_lookup_type_vars(te);_tmpE8;}));
void*etype=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpE5=_cycalloc(
sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpE6;_tmpE6.tag=
15;_tmpE6.f1=(void*)rgn_type;_tmpE6;});_tmpE5;});if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(
te,(void**)& etype,_tmp43),etype))({struct Cyc_Std_String_pa_struct _tmpE4;_tmpE4.tag=
0;_tmpE4.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp43->topt))->v);{void*_tmpE3[1]={& _tmpE4};Cyc_Tcutil_terr(_tmp43->loc,
_tag_arr("expecting region_t but found %s",sizeof(unsigned char),32),_tag_arr(
_tmpE3,sizeof(void*),1));}});Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);
return;}_LL29: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 13)goto _LL2B;_tmp44=((
struct Cyc_Absyn_Cut_s_struct*)_tmpA)->f1;_LL2A:(int)_throw(({void*_tmpE9[0]={};
Cyc_Tcutil_impos(_tag_arr("tcStmt: cut is not implemented",sizeof(unsigned char),
31),_tag_arr(_tmpE9,sizeof(void*),0));}));_LL2B: if(_tmpA <= (void*)1?1:*((int*)
_tmpA)!= 14)goto _LL0;_tmp45=((struct Cyc_Absyn_Splice_s_struct*)_tmpA)->f1;_LL2C:(
int)_throw(({void*_tmpEA[0]={};Cyc_Tcutil_impos(_tag_arr("tcStmt: splice is not implemented",
sizeof(unsigned char),34),_tag_arr(_tmpEA,sizeof(void*),0));}));_LL0:;}}
