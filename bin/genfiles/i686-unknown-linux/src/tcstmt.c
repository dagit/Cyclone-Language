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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;
struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct
Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];
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
struct Cyc_Absyn_Format_att_struct{int tag;void*f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{
int tag;int f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pointer_mod_struct{int tag;void*f1;void*f2;struct Cyc_Absyn_Tqual
f3;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{
int tag;void*f1;char f2;};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};
struct Cyc_Absyn_Int_c_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{
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
int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_exn_typ;
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,
struct Cyc_Position_Segment*s);typedef struct{int __count;union{unsigned int __wch;
char __wchb[4];}__value;}Cyc_Std___mbstate_t;typedef struct{int __pos;Cyc_Std___mbstate_t
__state;}Cyc_Std__G_fpos_t;typedef Cyc_Std__G_fpos_t Cyc_Std_fpos_t;struct Cyc_Std___cycFILE;
struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,
struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
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
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};extern char Cyc_CfFlowInfo_IsZero[11];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct
Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int
tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple7{
unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));s->try_depth=
Cyc_Tcenv_get_try_depth(te);}static void Cyc_Tcstmt_pattern_synth(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Exp*where_opt,int new_block){struct _tuple6*_tmp1;struct Cyc_List_List*
_tmp2;struct Cyc_Tcpat_TcPatResult _tmp0=pat_res;_tmp1=_tmp0.tvars_and_bounds_opt;
_tmp2=_tmp0.patvars;{struct Cyc_List_List*_tmp3=_tmp1 == 0?0:(*_tmp1).f1;struct Cyc_List_List*
_tmp4=_tmp1 == 0?0:(*_tmp1).f2;struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
loc,te,_tmp3);te2=Cyc_Tcenv_new_outlives_constraints(te2,_tmp4);if(new_block)te2=
Cyc_Tcenv_new_block(loc,te2);{struct Cyc_Core_Opt*_tmp5=({struct Cyc_Core_Opt*
_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->v=(void*)Cyc_Tcenv_curr_rgn(te2);_tmp7;});{
struct Cyc_List_List*_tmp6=_tmp2;for(0;_tmp6 != 0;_tmp6=_tmp6->tl){te2=Cyc_Tcenv_add_pat_var(
loc,te2,(struct Cyc_Absyn_Vardecl*)_tmp6->hd);((struct Cyc_Absyn_Vardecl*)_tmp6->hd)->rgn=
_tmp5;}}if(where_opt != 0)Cyc_Tcexp_tcTest(te2,(struct Cyc_Absyn_Exp*)_check_null(
where_opt),_tag_arr("switch clause guard",sizeof(char),20));if(_tmp2 != 0)te2=Cyc_Tcenv_set_encloser(
te2,s);Cyc_Tcstmt_tcStmt(te2,s,0);if(_tmp2 != 0)Cyc_NewControlFlow_set_encloser(s,
Cyc_Tcenv_get_encloser(te));}}}struct _tuple8{struct Cyc_Tcenv_Tenv*f1;struct Cyc_List_List*
f2;};struct _tuple8 Cyc_Tcstmt_check_forarray_decls(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct Cyc_List_List*decls){({void*_tmp8[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_forarray_decls",
sizeof(char),21),_tag_arr(_tmp8,sizeof(void*),0));});return({struct _tuple8 _tmp9;
_tmp9.f1=te;_tmp9.f2=0;_tmp9;});}void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s0,int new_block){Cyc_Tcstmt_decorate_stmt(te,s0);{void*
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
sizeof(char),31),_tag_arr(_tmp4F,sizeof(void*),1));}});goto _LL32;_LL32:;}else{
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmpE);Cyc_Tcexp_tcExp(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct
_tmp53;_tmp53.tag=0;_tmp53.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp51[2]={& _tmp52,& _tmp53};Cyc_Tcutil_terr(s0->loc,_tag_arr("returns value of type %s but requires %s",
sizeof(char),41),_tag_arr(_tmp51,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
return;}_LL9: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 3)goto _LLB;_tmpF=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpA)->f1;_tmp10=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpA)->f2;_tmp11=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmpA)->f3;_LLA: Cyc_Tcexp_tcTest(te,_tmpF,
_tag_arr("if statement",sizeof(char),13));Cyc_Tcstmt_tcStmt(te,_tmp10,1);Cyc_Tcstmt_tcStmt(
te,_tmp11,1);return;_LLB: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 4)goto _LLD;
_tmp12=((struct Cyc_Absyn_While_s_struct*)_tmpA)->f1;_tmp13=_tmp12.f1;_tmp14=
_tmp12.f2;_tmp15=((struct Cyc_Absyn_While_s_struct*)_tmpA)->f2;_LLC: Cyc_Tcstmt_decorate_stmt(
te,_tmp14);Cyc_Tcexp_tcTest(te,_tmp13,_tag_arr("while loop",sizeof(char),11));
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(te,_tmp14),_tmp15,1);return;_LLD: if(
_tmpA <= (void*)1?1:*((int*)_tmpA)!= 8)goto _LLF;_tmp16=((struct Cyc_Absyn_For_s_struct*)
_tmpA)->f1;_tmp17=((struct Cyc_Absyn_For_s_struct*)_tmpA)->f2;_tmp18=_tmp17.f1;
_tmp19=_tmp17.f2;_tmp1A=((struct Cyc_Absyn_For_s_struct*)_tmpA)->f3;_tmp1B=_tmp1A.f1;
_tmp1C=_tmp1A.f2;_tmp1D=((struct Cyc_Absyn_For_s_struct*)_tmpA)->f4;_LLE: Cyc_Tcstmt_decorate_stmt(
te,_tmp19);Cyc_Tcstmt_decorate_stmt(te,_tmp1C);Cyc_Tcexp_tcExp(te,0,_tmp16);Cyc_Tcexp_tcTest(
te,_tmp18,_tag_arr("for loop",sizeof(char),9));te=Cyc_Tcenv_set_in_loop(te,
_tmp1C);Cyc_Tcstmt_tcStmt(te,_tmp1D,1);Cyc_Tcexp_tcExp(te,0,_tmp1B);return;_LLF:
if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 19)goto _LL11;_tmp1E=((struct Cyc_Absyn_ForArray_s_struct*)
_tmpA)->f1;_tmp1F=_tmp1E.defns;_tmp20=_tmp1E.condition;_tmp21=_tmp20.f1;_tmp22=
_tmp20.f2;_tmp23=_tmp1E.delta;_tmp24=_tmp23.f1;_tmp25=_tmp23.f2;_tmp26=_tmp1E.body;
_LL10:({void*_tmp54[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("ForArray_s",sizeof(char),11),_tag_arr(_tmp54,sizeof(void*),0));});
return;_LL11: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 16)goto _LL13;_tmp27=((struct
Cyc_Absyn_Do_s_struct*)_tmpA)->f1;_tmp28=((struct Cyc_Absyn_Do_s_struct*)_tmpA)->f2;
_tmp29=_tmp28.f1;_tmp2A=_tmp28.f2;_LL12: Cyc_Tcstmt_decorate_stmt(te,_tmp2A);Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop(te,_tmp2A),_tmp27,1);Cyc_Tcexp_tcTest(te,_tmp29,_tag_arr("do loop",
sizeof(char),8));return;_LL13: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 5)goto _LL15;
_tmp2B=((struct Cyc_Absyn_Break_s_struct*)_tmpA)->f1;_tmp2C=(struct Cyc_Absyn_Stmt**)&((
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
sizeof(char),32),_tag_arr(_tmp56,sizeof(void*),0));});return;}{struct _tuple4
_tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List*_tmp5A;struct _tuple4*
_tmp57=(struct _tuple4*)_check_null(_tmp55);_tmp58=*_tmp57;_tmp59=_tmp58.f2;
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
sizeof(char),63),_tag_arr(_tmp5B,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
if(_tmp32 != 0)({void*_tmp5E[0]={};Cyc_Tcutil_terr(s0->loc,_tag_arr("too many arguments to explicit fallthru",
sizeof(char),40),_tag_arr(_tmp5E,sizeof(void*),0));});if(_tmp5A != 0)({void*
_tmp5F[0]={};Cyc_Tcutil_terr(s0->loc,_tag_arr("too few arguments to explicit fallthru",
sizeof(char),39),_tag_arr(_tmp5F,sizeof(void*),0));});return;}}}_LL1B: if(_tmpA <= (
void*)1?1:*((int*)_tmpA)!= 15)goto _LL1D;_tmp35=((struct Cyc_Absyn_Label_s_struct*)
_tmpA)->f1;_tmp36=((struct Cyc_Absyn_Label_s_struct*)_tmpA)->f2;_LL1C: Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_new_named_block(s0->loc,Cyc_Tcenv_add_label(te,_tmp35,_tmp36),({struct
Cyc_Absyn_Tvar*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->name=({struct
_tagged_arr*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp65;_tmp65.tag=0;_tmp65.f1=(struct _tagged_arr)*
_tmp35;{void*_tmp64[1]={& _tmp65};Cyc_Std_aprintf(_tag_arr("`%s",sizeof(char),4),
_tag_arr(_tmp64,sizeof(void*),1));}});_tmp63;});_tmp60->identity=Cyc_Tcutil_new_tvar_id();
_tmp60->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp61=_cycalloc(
sizeof(*_tmp61));_tmp61[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp62;_tmp62.tag=0;
_tmp62.f1=(void*)((void*)3);_tmp62;});_tmp61;}));_tmp60;})),_tmp36,0);return;
_LL1D: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 9)goto _LL1F;_tmp37=((struct Cyc_Absyn_Switch_s_struct*)
_tmpA)->f1;_tmp38=((struct Cyc_Absyn_Switch_s_struct*)_tmpA)->f2;_LL1E: Cyc_Tcexp_tcExp(
te,0,_tmp37);{void*_tmp66=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp37->topt))->v;
te=Cyc_Tcenv_set_in_switch(te);te=Cyc_Tcenv_clear_fallthru(te);{struct Cyc_List_List*
scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp38);for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Pat*_tmp67=((struct Cyc_Absyn_Switch_clause*)
scs->hd)->pattern;struct Cyc_Tcpat_TcPatResult _tmp68=Cyc_Tcpat_tcPat(te,_tmp67,(
void**)& _tmp66);struct Cyc_List_List*_tmp69=_tmp68.tvars_and_bounds_opt == 0?0:(*
_tmp68.tvars_and_bounds_opt).f1;if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp67->topt))->v,_tmp66)){({struct Cyc_Std_String_pa_struct _tmp6C;
_tmp6C.tag=0;_tmp6C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp67->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp6B;_tmp6B.tag=0;_tmp6B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp66);{
void*_tmp6A[2]={& _tmp6B,& _tmp6C};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,_tag_arr("switch on type %s, but case expects type %s",sizeof(char),
44),_tag_arr(_tmp6A,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}else{
Cyc_Tcpat_check_pat_regions(te,_tmp67);}((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=({
struct Cyc_Core_Opt*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->v=_tmp68.patvars;
_tmp6D;});Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,
te,_tmp68,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)
scs->hd)->where_clause,1);if(_tmp68.tvars_and_bounds_opt != 0?(*_tmp68.tvars_and_bounds_opt).f2
!= 0: 0)te=Cyc_Tcenv_clear_fallthru(te);else{te=Cyc_Tcenv_set_fallthru(te,_tmp69,
_tmp68.patvars,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}}Cyc_Tcpat_check_switch_exhaustive(
s0->loc,_tmp38);return;}_LL1F: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 10)goto
_LL21;_tmp39=((struct Cyc_Absyn_SwitchC_s_struct*)_tmpA)->f1;_tmp3A=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmpA)->f2;_LL20: {struct Cyc_List_List*_tmp6E=0;for(0;_tmp3A != 0;_tmp3A=_tmp3A->tl){
struct Cyc_Absyn_SwitchC_clause _tmp70;struct Cyc_Absyn_Exp*_tmp71;struct Cyc_Absyn_Stmt*
_tmp72;struct Cyc_Position_Segment*_tmp73;struct Cyc_Absyn_SwitchC_clause*_tmp6F=(
struct Cyc_Absyn_SwitchC_clause*)_tmp3A->hd;_tmp70=*_tmp6F;_tmp71=_tmp70.cnst_exp;
_tmp72=_tmp70.body;_tmp73=_tmp70.loc;{struct Cyc_Absyn_Pat*pat;if(_tmp71 != 0){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp71));{unsigned int _tmp75;int _tmp76;
struct _tuple7 _tmp74=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp71));_tmp75=_tmp74.f1;_tmp76=_tmp74.f2;if(!_tmp76)({void*_tmp77[
0]={};Cyc_Tcutil_terr(_tmp71->loc,_tag_arr("Cyclone cases do not support sizeof or offsetof",
sizeof(char),48),_tag_arr(_tmp77,sizeof(void*),0));});pat=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Int_p_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=({
struct Cyc_Absyn_Int_p_struct _tmp79;_tmp79.tag=1;_tmp79.f1=(void*)((void*)0);
_tmp79.f2=(int)_tmp75;_tmp79;});_tmp78;}),_tmp71->loc);}}else{pat=Cyc_Absyn_new_pat((
void*)0,_tmp73);}_tmp6E=({struct Cyc_List_List*_tmp7A=_cycalloc(sizeof(*_tmp7A));
_tmp7A->hd=({struct Cyc_Absyn_Switch_clause*_tmp7B=_cycalloc(sizeof(*_tmp7B));
_tmp7B->pattern=pat;_tmp7B->pat_vars=0;_tmp7B->where_clause=0;_tmp7B->body=
_tmp72;_tmp7B->loc=_tmp73;_tmp7B;});_tmp7A->tl=_tmp6E;_tmp7A;});}}(void*)(s0->r=(
void*)((void*)(Cyc_Absyn_switch_stmt(_tmp39,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp6E),0))->r));Cyc_Tcstmt_tcStmt(te,s0,new_block);return;}
_LL21: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 17)goto _LL23;_tmp3B=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmpA)->f1;_tmp3C=((struct Cyc_Absyn_TryCatch_s_struct*)_tmpA)->f2;_LL22:(void*)(
_tmp3B->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Seq_s_struct*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Absyn_Seq_s_struct _tmp7D;
_tmp7D.tag=1;_tmp7D.f1=Cyc_Absyn_new_stmt((void*)_tmp3B->r,_tmp3B->loc);_tmp7D.f2=
Cyc_Absyn_skip_stmt(_tmp3B->loc);_tmp7D;});_tmp7C;}),_tmp3B->loc))->r));Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(Cyc_Tcenv_enter_try(te),s0),_tmp3B,1);te=Cyc_Tcenv_set_in_switch(
te);te=Cyc_Tcenv_clear_fallthru(te);{struct Cyc_List_List*_tmp7E=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp3C);for(0;_tmp7E != 0;_tmp7E=_tmp7E->tl){
struct Cyc_Absyn_Pat*_tmp7F=((struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->pattern;
struct Cyc_Tcpat_TcPatResult _tmp80=Cyc_Tcpat_tcPat(te,_tmp7F,(void**)& Cyc_Absyn_exn_typ);
struct Cyc_List_List*_tmp81=_tmp80.tvars_and_bounds_opt == 0?0:(*_tmp80.tvars_and_bounds_opt).f1;
if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F->topt))->v,
Cyc_Absyn_exn_typ))({struct Cyc_Std_String_pa_struct _tmp83;_tmp83.tag=0;_tmp83.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7F->topt))->v);{void*_tmp82[1]={& _tmp83};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->loc,_tag_arr("expected xtunion exn but found %s",
sizeof(char),34),_tag_arr(_tmp82,sizeof(void*),1));}});else{Cyc_Tcpat_check_pat_regions(
te,_tmp7F);}((struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->pat_vars=({struct Cyc_Core_Opt*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->v=_tmp80.patvars;_tmp84;});Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->loc,te,_tmp80,((struct Cyc_Absyn_Switch_clause*)
_tmp7E->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->where_clause,1);
if(_tmp80.tvars_and_bounds_opt != 0?(*_tmp80.tvars_and_bounds_opt).f2 != 0: 0)te=
Cyc_Tcenv_clear_fallthru(te);else{te=Cyc_Tcenv_set_fallthru(te,_tmp81,_tmp80.patvars,(
struct Cyc_Absyn_Switch_clause*)_tmp7E->hd);}}}Cyc_Tcpat_check_catch_overlap(s0->loc,
_tmp3C);return;_LL23: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 12)goto _LL25;_tmp3D=((
struct Cyc_Absyn_Decl_s_struct*)_tmpA)->f1;_tmp3E=((struct Cyc_Absyn_Decl_s_struct*)
_tmpA)->f2;_LL24: {struct _tagged_arr unimp_msg_part;if(new_block)te=Cyc_Tcenv_new_block(
s0->loc,te);{void*_tmp85=(void*)_tmp3D->r;struct Cyc_Absyn_Vardecl*_tmp86;struct
Cyc_Absyn_Pat*_tmp87;struct Cyc_Core_Opt*_tmp88;struct Cyc_Core_Opt**_tmp89;struct
Cyc_Absyn_Exp*_tmp8A;struct Cyc_List_List*_tmp8B;struct _tagged_arr*_tmp8C;struct
Cyc_List_List*_tmp8D;struct _tuple0*_tmp8E;struct Cyc_List_List*_tmp8F;_LL38: if(*((
int*)_tmp85)!= 0)goto _LL3A;_tmp86=((struct Cyc_Absyn_Var_d_struct*)_tmp85)->f1;
_LL39: {struct Cyc_Absyn_Vardecl _tmp91;void*_tmp92;struct _tuple0*_tmp93;struct
_tuple0 _tmp94;void*_tmp95;struct _tagged_arr*_tmp96;struct Cyc_Absyn_Tqual _tmp97;
void*_tmp98;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Core_Opt*_tmp9A;struct Cyc_Core_Opt**
_tmp9B;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Vardecl*_tmp90=_tmp86;_tmp91=*
_tmp90;_tmp92=(void*)_tmp91.sc;_tmp93=_tmp91.name;_tmp94=*_tmp93;_tmp95=_tmp94.f1;
_tmp96=_tmp94.f2;_tmp97=_tmp91.tq;_tmp98=(void*)_tmp91.type;_tmp99=_tmp91.initializer;
_tmp9A=_tmp91.rgn;_tmp9B=(struct Cyc_Core_Opt**)&(*_tmp90).rgn;_tmp9C=_tmp91.attributes;{
void*_tmp9D=Cyc_Tcenv_curr_rgn(te);int is_local;{void*_tmp9E=_tmp92;_LL4F: if((int)
_tmp9E != 0)goto _LL51;_LL50: goto _LL52;_LL51: if((int)_tmp9E != 3)goto _LL53;_LL52:
goto _LL54;_LL53: if((int)_tmp9E != 4)goto _LL55;_LL54: is_local=0;goto _LL4E;_LL55:
if((int)_tmp9E != 1)goto _LL57;_LL56:({void*_tmp9F[0]={};Cyc_Tcutil_terr(_tmp3D->loc,
_tag_arr("bad abstract scope for local variable",sizeof(char),38),_tag_arr(
_tmp9F,sizeof(void*),0));});goto _LL58;_LL57: if((int)_tmp9E != 5)goto _LL59;_LL58:
goto _LL5A;_LL59: if((int)_tmp9E != 2)goto _LL4E;_LL5A: is_local=1;goto _LL4E;_LL4E:;}*
_tmp9B=is_local?({struct Cyc_Core_Opt*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->v=(
void*)_tmp9D;_tmpA0;}):({struct Cyc_Core_Opt*_tmpA1=_cycalloc(sizeof(*_tmpA1));
_tmpA1->v=(void*)((void*)2);_tmpA1;});{void*_tmpA2=_tmp95;struct Cyc_List_List*
_tmpA3;struct Cyc_List_List*_tmpA4;_LL5C: if((int)_tmpA2 != 0)goto _LL5E;_LL5D: goto
_LL5B;_LL5E: if(_tmpA2 <= (void*)1?1:*((int*)_tmpA2)!= 0)goto _LL60;_tmpA3=((struct
Cyc_Absyn_Rel_n_struct*)_tmpA2)->f1;if(_tmpA3 != 0)goto _LL60;_LL5F:(*_tmp86->name).f1=(
void*)0;goto _LL5B;_LL60: if(_tmpA2 <= (void*)1?1:*((int*)_tmpA2)!= 1)goto _LL62;
_tmpA4=((struct Cyc_Absyn_Abs_n_struct*)_tmpA2)->f1;_LL61:(int)_throw(({void*
_tmpA5[0]={};Cyc_Tcutil_impos(_tag_arr("tcstmt: Abs_n declaration",sizeof(char),
26),_tag_arr(_tmpA5,sizeof(void*),0));}));_LL62:;_LL63:({void*_tmpA6[0]={};Cyc_Tcutil_terr(
_tmp3D->loc,_tag_arr("cannot declare a qualified local variable",sizeof(char),42),
_tag_arr(_tmpA6,sizeof(void*),0));});goto _LL5B;_LL5B:;}{void*_tmpA7=Cyc_Tcutil_compress(
_tmp98);void*_tmpA8;struct Cyc_Absyn_Tqual _tmpA9;struct Cyc_Absyn_Exp*_tmpAA;_LL65:
if(_tmpA7 <= (void*)3?1:*((int*)_tmpA7)!= 7)goto _LL67;_tmpA8=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmpA7)->f1;_tmpA9=((struct Cyc_Absyn_ArrayType_struct*)_tmpA7)->f2;_tmpAA=((
struct Cyc_Absyn_ArrayType_struct*)_tmpA7)->f3;if(_tmpAA != 0)goto _LL67;if(!(
_tmp86->initializer != 0))goto _LL67;_LL66:{void*_tmpAB=(void*)((struct Cyc_Absyn_Exp*)
_check_null(_tmp86->initializer))->r;void*_tmpAC;struct _tagged_arr _tmpAD;struct
Cyc_Absyn_Exp*_tmpAE;struct Cyc_List_List*_tmpAF;struct Cyc_List_List*_tmpB0;_LL6A:
if(*((int*)_tmpAB)!= 0)goto _LL6C;_tmpAC=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmpAB)->f1;if(_tmpAC <= (void*)1?1:*((int*)_tmpAC)!= 5)goto _LL6C;_tmpAD=((struct
Cyc_Absyn_String_c_struct*)_tmpAC)->f1;_LL6B: _tmp98=(void*)(_tmp86->type=(void*)((
void*)({struct Cyc_Absyn_ArrayType_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));
_tmpB1[0]=({struct Cyc_Absyn_ArrayType_struct _tmpB2;_tmpB2.tag=7;_tmpB2.f1=(void*)
_tmpA8;_tmpB2.f2=_tmpA9;_tmpB2.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
_get_arr_size(_tmpAD,sizeof(char)),0);_tmpB2;});_tmpB1;})));goto _LL69;_LL6C: if(*((
int*)_tmpAB)!= 27)goto _LL6E;_tmpAE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpAB)->f2;_LL6D: _tmp98=(void*)(_tmp86->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpB4;_tmpB4.tag=7;_tmpB4.f1=(void*)_tmpA8;_tmpB4.f2=_tmpA9;_tmpB4.f3=(struct
Cyc_Absyn_Exp*)_tmpAE;_tmpB4;});_tmpB3;})));goto _LL69;_LL6E: if(*((int*)_tmpAB)!= 
34)goto _LL70;_tmpAF=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpAB)->f2;_LL6F:
_tmpB0=_tmpAF;goto _LL71;_LL70: if(*((int*)_tmpAB)!= 26)goto _LL72;_tmpB0=((struct
Cyc_Absyn_Array_e_struct*)_tmpAB)->f1;_LL71: _tmp98=(void*)(_tmp86->type=(void*)((
void*)({struct Cyc_Absyn_ArrayType_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));
_tmpB5[0]=({struct Cyc_Absyn_ArrayType_struct _tmpB6;_tmpB6.tag=7;_tmpB6.f1=(void*)
_tmpA8;_tmpB6.f2=_tmpA9;_tmpB6.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB0),0);_tmpB6;});
_tmpB5;})));goto _LL69;_LL72:;_LL73: goto _LL69;_LL69:;}goto _LL64;_LL67:;_LL68: goto
_LL64;_LL64:;}{struct Cyc_List_List*_tmpB7=!is_local?0: Cyc_Tcenv_lookup_type_vars(
te);int _tmpB8=!is_local?0: 1;Cyc_Tcutil_check_type(s0->loc,te,_tmpB7,(void*)1,
_tmpB8,_tmp98);{struct Cyc_Tcenv_Tenv*_tmpB9=Cyc_Tcenv_add_local_var(_tmp3E->loc,
te,_tmp86);if(_tmp92 == (void*)3?1: _tmp92 == (void*)4)({void*_tmpBA[0]={};Cyc_Tcutil_terr(
_tmp3D->loc,_tag_arr("local extern declarations not yet supported",sizeof(char),
44),_tag_arr(_tmpBA,sizeof(void*),0));});if(_tmp99 != 0){Cyc_Tcexp_tcExpInitializer(
_tmpB9,(void**)& _tmp98,(struct Cyc_Absyn_Exp*)_check_null(_tmp99));if(!is_local?!
Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp99)): 0)({void*
_tmpBB[0]={};Cyc_Tcutil_terr(_tmp3D->loc,_tag_arr("initializer needs to be a constant expression",
sizeof(char),46),_tag_arr(_tmpBB,sizeof(void*),0));});if(!Cyc_Tcutil_coerce_assign(
_tmpB9,(struct Cyc_Absyn_Exp*)_check_null(_tmp99),_tmp98)){({struct Cyc_Std_String_pa_struct
_tmpBF;_tmpBF.tag=0;_tmpBF.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp99->topt))->v);{struct Cyc_Std_String_pa_struct
_tmpBE;_tmpBE.tag=0;_tmpBE.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp98);{
struct Cyc_Std_String_pa_struct _tmpBD;_tmpBD.tag=0;_tmpBD.f1=(struct _tagged_arr)*
_tmp96;{void*_tmpBC[3]={& _tmpBD,& _tmpBE,& _tmpBF};Cyc_Tcutil_terr(_tmp3D->loc,
_tag_arr("%s declared with type \n%s\n but initialized with type \n%s",sizeof(
char),57),_tag_arr(_tmpBC,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmpB9,s0),_tmp3E,0);return;}}}}_LL3A:
if(*((int*)_tmp85)!= 2)goto _LL3C;_tmp87=((struct Cyc_Absyn_Let_d_struct*)_tmp85)->f1;
_tmp88=((struct Cyc_Absyn_Let_d_struct*)_tmp85)->f2;_tmp89=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Let_d_struct*)_tmp85)->f2;_tmp8A=((struct Cyc_Absyn_Let_d_struct*)
_tmp85)->f3;_LL3B: Cyc_Tcexp_tcExpInitializer(te,0,_tmp8A);{struct Cyc_Tcpat_TcPatResult
_tmpC0=Cyc_Tcpat_tcPat(te,_tmp87,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(_tmp8A->topt))->v)));*_tmp89=({struct Cyc_Core_Opt*_tmpC1=_cycalloc(
sizeof(*_tmpC1));_tmpC1->v=_tmpC0.patvars;_tmpC1;});if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp87->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp8A->topt))->v)){({struct Cyc_Std_String_pa_struct _tmpC4;_tmpC4.tag=
0;_tmpC4.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp8A->topt))->v);{struct Cyc_Std_String_pa_struct _tmpC3;_tmpC3.tag=
0;_tmpC3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp87->topt))->v);{void*_tmpC2[2]={& _tmpC3,& _tmpC4};Cyc_Tcutil_terr(
_tmp3D->loc,_tag_arr("pattern type %s does not match definition type %s",sizeof(
char),50),_tag_arr(_tmpC2,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(te,_tmp87);}Cyc_Tcpat_check_let_pat_exhaustive(
_tmp87->loc,_tmp87);Cyc_Tcstmt_pattern_synth(s0->loc,te,_tmpC0,_tmp3E,0,0);
return;}_LL3C: if(*((int*)_tmp85)!= 3)goto _LL3E;_tmp8B=((struct Cyc_Absyn_Letv_d_struct*)
_tmp85)->f1;_LL3D: {void*_tmpC5=Cyc_Tcenv_curr_rgn(te);struct Cyc_Tcenv_Tenv*
_tmpC6=te;for(0;_tmp8B != 0;_tmp8B=_tmp8B->tl){struct Cyc_Absyn_Vardecl*_tmpC7=(
struct Cyc_Absyn_Vardecl*)_tmp8B->hd;struct Cyc_Absyn_Vardecl _tmpC9;struct _tuple0*
_tmpCA;struct _tuple0 _tmpCB;void*_tmpCC;void*_tmpCD;struct Cyc_Core_Opt*_tmpCE;
struct Cyc_Core_Opt**_tmpCF;struct Cyc_Absyn_Vardecl*_tmpC8=_tmpC7;_tmpC9=*_tmpC8;
_tmpCA=_tmpC9.name;_tmpCB=*_tmpCA;_tmpCC=_tmpCB.f1;_tmpCD=(void*)_tmpC9.type;
_tmpCE=_tmpC9.rgn;_tmpCF=(struct Cyc_Core_Opt**)&(*_tmpC8).rgn;*_tmpCF=({struct
Cyc_Core_Opt*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->v=(void*)_tmpC5;_tmpD0;});{
void*_tmpD1=_tmpCC;struct Cyc_List_List*_tmpD2;struct Cyc_List_List*_tmpD3;_LL75:
if((int)_tmpD1 != 0)goto _LL77;_LL76: goto _LL74;_LL77: if(_tmpD1 <= (void*)1?1:*((int*)
_tmpD1)!= 0)goto _LL79;_tmpD2=((struct Cyc_Absyn_Rel_n_struct*)_tmpD1)->f1;if(
_tmpD2 != 0)goto _LL79;_LL78:(*_tmpC7->name).f1=(void*)0;goto _LL74;_LL79: if(_tmpD1
<= (void*)1?1:*((int*)_tmpD1)!= 1)goto _LL7B;_tmpD3=((struct Cyc_Absyn_Abs_n_struct*)
_tmpD1)->f1;_LL7A:(int)_throw(({void*_tmpD4[0]={};Cyc_Tcutil_impos(_tag_arr("tcstmt: Abs_n declaration",
sizeof(char),26),_tag_arr(_tmpD4,sizeof(void*),0));}));_LL7B:;_LL7C:({void*
_tmpD5[0]={};Cyc_Tcutil_terr(_tmp3D->loc,_tag_arr("cannot declare a qualified local variable",
sizeof(char),42),_tag_arr(_tmpD5,sizeof(void*),0));});goto _LL74;_LL74:;}Cyc_Tcutil_check_type(
s0->loc,_tmpC6,Cyc_Tcenv_lookup_type_vars(_tmpC6),(void*)1,1,_tmpCD);_tmpC6=Cyc_Tcenv_add_local_var(
_tmp3E->loc,_tmpC6,_tmpC7);}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmpC6,s0),
_tmp3E,0);return;}_LL3E: if(*((int*)_tmp85)!= 8)goto _LL40;_tmp8C=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp85)->f1;_tmp8D=((struct Cyc_Absyn_Namespace_d_struct*)_tmp85)->f2;_LL3F:
unimp_msg_part=_tag_arr("namespace",sizeof(char),10);goto _LL37;_LL40: if(*((int*)
_tmp85)!= 9)goto _LL42;_tmp8E=((struct Cyc_Absyn_Using_d_struct*)_tmp85)->f1;
_tmp8F=((struct Cyc_Absyn_Using_d_struct*)_tmp85)->f2;_LL41: unimp_msg_part=
_tag_arr("using",sizeof(char),6);goto _LL37;_LL42: if(*((int*)_tmp85)!= 1)goto
_LL44;_LL43: unimp_msg_part=_tag_arr("function",sizeof(char),9);goto _LL37;_LL44:
if(*((int*)_tmp85)!= 4)goto _LL46;_LL45: unimp_msg_part=_tag_arr("type",sizeof(
char),5);goto _LL37;_LL46: if(*((int*)_tmp85)!= 5)goto _LL48;_LL47: unimp_msg_part=
_tag_arr("[x]tunion",sizeof(char),10);goto _LL37;_LL48: if(*((int*)_tmp85)!= 6)
goto _LL4A;_LL49: unimp_msg_part=_tag_arr("enum",sizeof(char),5);goto _LL37;_LL4A:
if(*((int*)_tmp85)!= 7)goto _LL4C;_LL4B: unimp_msg_part=_tag_arr("typedef",sizeof(
char),8);goto _LL37;_LL4C: if(*((int*)_tmp85)!= 10)goto _LL37;_LL4D: unimp_msg_part=
_tag_arr("extern \"C\"",sizeof(char),11);goto _LL37;_LL37:;}(int)_throw(({struct
Cyc_Std_String_pa_struct _tmpD7;_tmpD7.tag=0;_tmpD7.f1=(struct _tagged_arr)
unimp_msg_part;{void*_tmpD6[1]={& _tmpD7};Cyc_Tcutil_impos(_tag_arr("tcStmt: nested %s declarations unimplemented",
sizeof(char),45),_tag_arr(_tmpD6,sizeof(void*),1));}}));}_LL25: if(_tmpA <= (void*)
1?1:*((int*)_tmpA)!= 18)goto _LL27;_tmp3F=((struct Cyc_Absyn_Region_s_struct*)
_tmpA)->f1;_tmp40=((struct Cyc_Absyn_Region_s_struct*)_tmpA)->f2;_tmp41=((struct
Cyc_Absyn_Region_s_struct*)_tmpA)->f3;_tmp42=((struct Cyc_Absyn_Region_s_struct*)
_tmpA)->f4;_LL26:(void*)(_tmp42->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Seq_s_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({
struct Cyc_Absyn_Seq_s_struct _tmpD9;_tmpD9.tag=1;_tmpD9.f1=Cyc_Absyn_new_stmt((
void*)_tmp42->r,_tmp42->loc);_tmpD9.f2=Cyc_Absyn_skip_stmt(_tmp42->loc);_tmpD9;});
_tmpD8;}),_tmp42->loc))->r));if(new_block)te=Cyc_Tcenv_new_block(s0->loc,te);{
struct Cyc_Absyn_Vardecl _tmpDB;void*_tmpDC;struct Cyc_Core_Opt*_tmpDD;struct Cyc_Core_Opt**
_tmpDE;struct Cyc_Absyn_Vardecl*_tmpDA=_tmp40;_tmpDB=*_tmpDA;_tmpDC=(void*)_tmpDB.type;
_tmpDD=_tmpDB.rgn;_tmpDE=(struct Cyc_Core_Opt**)&(*_tmpDA).rgn;{void*_tmpDF=Cyc_Tcenv_curr_rgn(
te);void*rgn_typ=(void*)({struct Cyc_Absyn_VarType_struct*_tmpE5=_cycalloc(
sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Absyn_VarType_struct _tmpE6;_tmpE6.tag=1;
_tmpE6.f1=_tmp3F;_tmpE6;});_tmpE5;});*_tmpDE=({struct Cyc_Core_Opt*_tmpE0=
_cycalloc(sizeof(*_tmpE0));_tmpE0->v=(void*)_tmpDF;_tmpE0;});te=Cyc_Tcenv_add_type_vars(
s0->loc,te,({struct Cyc_List_List*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->hd=
_tmp3F;_tmpE1->tl=0;_tmpE1;}));Cyc_Tcutil_check_type(s0->loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)2,1,_tmpDC);te=Cyc_Tcenv_add_region(te,rgn_typ,_tmp41);if(!Cyc_Tcutil_unify(
_tmpDC,(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpE2=_cycalloc(sizeof(*
_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpE3;_tmpE3.tag=15;
_tmpE3.f1=(void*)rgn_typ;_tmpE3;});_tmpE2;})))({void*_tmpE4[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("region stmt: type of region handle is wrong!",
sizeof(char),45),_tag_arr(_tmpE4,sizeof(void*),0));});Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
Cyc_Tcenv_add_local_var(_tmp42->loc,Cyc_Tcenv_enter_try(te),_tmp40),s0),_tmp42,0);
return;}}_LL27: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 20)goto _LL29;_tmp43=((
struct Cyc_Absyn_ResetRegion_s_struct*)_tmpA)->f1;_LL28: {void*rgn_type=Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->v=(void*)((void*)3);
_tmpEB;}),({struct Cyc_Core_Opt*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpEC;}));void*etype=(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpE9=
_cycalloc(sizeof(*_tmpE9));_tmpE9[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpEA;_tmpEA.tag=15;_tmpEA.f1=(void*)rgn_type;_tmpEA;});_tmpE9;});if(!Cyc_Tcutil_unify(
Cyc_Tcexp_tcExp(te,(void**)& etype,_tmp43),etype))({struct Cyc_Std_String_pa_struct
_tmpE8;_tmpE8.tag=0;_tmpE8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp43->topt))->v);{void*_tmpE7[1]={& _tmpE8};Cyc_Tcutil_terr(
_tmp43->loc,_tag_arr("expecting region_t but found %s",sizeof(char),32),_tag_arr(
_tmpE7,sizeof(void*),1));}});Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);
return;}_LL29: if(_tmpA <= (void*)1?1:*((int*)_tmpA)!= 13)goto _LL2B;_tmp44=((
struct Cyc_Absyn_Cut_s_struct*)_tmpA)->f1;_LL2A:(int)_throw(({void*_tmpED[0]={};
Cyc_Tcutil_impos(_tag_arr("tcStmt: cut is not implemented",sizeof(char),31),
_tag_arr(_tmpED,sizeof(void*),0));}));_LL2B: if(_tmpA <= (void*)1?1:*((int*)_tmpA)
!= 14)goto _LL0;_tmp45=((struct Cyc_Absyn_Splice_s_struct*)_tmpA)->f1;_LL2C:(int)
_throw(({void*_tmpEE[0]={};Cyc_Tcutil_impos(_tag_arr("tcStmt: splice is not implemented",
sizeof(char),34),_tag_arr(_tmpEE,sizeof(void*),0));}));_LL0:;}}
