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
char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_list_cmp(int(*cmp)(
void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
_tagged_arr*f1;};char Cyc_Absyn_EmptyAnnot[15]="\000\000\000\000EmptyAnnot";int
Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_force_kb(
void*kb);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;extern
void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;
extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(
int);extern void*Cyc_Absyn_empty_effect;extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;extern void*Cyc_Absyn_exn_typ;
extern struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar;extern struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar;
void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);
void*Cyc_Absyn_file_typ();extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,void*b);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);void*
Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_cstar_typ(
void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq);void*Cyc_Absyn_void_star_typ();struct Cyc_Core_Opt*Cyc_Absyn_void_star_typ_opt();
void*Cyc_Absyn_strct(struct _tagged_arr*name);void*Cyc_Absyn_strctq(struct _tuple0*
name);void*Cyc_Absyn_unionq_typ(struct _tuple0*name);void*Cyc_Absyn_union_typ(
struct _tagged_arr*name);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp(int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _tagged_arr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct
Cyc_Absyn_Exp*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_splice_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _tagged_arr*v,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct
Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct
Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct
_tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs);struct
Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct
Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct _tuple3{struct Cyc_Absyn_Tqual f1;
void*f2;};struct _tuple3*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
struct _tagged_arr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*
a);struct _tagged_arr*Cyc_Absyn_fieldname(int);struct _tuple4{void*f1;struct
_tuple0*f2;};struct _tuple4 Cyc_Absyn_aggr_kinded_name(void*);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_Absyn_is_union_type(void*);void
Cyc_Absyn_print_decls(struct Cyc_List_List*);struct Cyc_Typerep_Int_struct{int tag;
int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;
void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple5{
unsigned int f1;struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int
tag;struct _tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple6{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _tagged_arr f2;};struct _tuple7{unsigned int f1;struct _tagged_arr f2;};struct
Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct
_tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{int tag;struct _tagged_arr f1;
struct _tagged_arr f2;unsigned int f3;struct _tagged_arr f4;};struct _tuple8{struct
_tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{int tag;struct
_tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{int tag;struct
_tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{int tag;
struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern void*Cyc_decls_rep;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_printf(
struct _tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct _tuple9{void*f1;
void*f2;};struct _tuple9*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple9*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void*Cyc_Tcutil_compress(void*t);void Cyc_Marshal_print_type(void*
rep,void*val);struct _tuple10{struct Cyc_Dict_Dict*f1;int f2;};struct _tuple11{
struct _tagged_arr f1;int f2;};static int Cyc_Absyn_zstrlist_cmp(struct Cyc_List_List*
ss1,struct Cyc_List_List*ss2){return((int(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Std_zstrptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,
struct Cyc_List_List*vs2){if((int)vs1 == (int)vs2)return 0;return Cyc_Absyn_zstrlist_cmp(
vs1,vs2);}int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){void*_tmp0=(*
q1).f1;void*_tmp1=(*q2).f1;{struct _tuple9 _tmp3=({struct _tuple9 _tmp2;_tmp2.f1=
_tmp0;_tmp2.f2=_tmp1;_tmp2;});void*_tmp4;void*_tmp5;void*_tmp6;struct Cyc_List_List*
_tmp7;void*_tmp8;struct Cyc_List_List*_tmp9;void*_tmpA;struct Cyc_List_List*_tmpB;
void*_tmpC;struct Cyc_List_List*_tmpD;void*_tmpE;void*_tmpF;void*_tmp10;void*
_tmp11;_LL1: _tmp4=_tmp3.f1;if((int)_tmp4 != 0)goto _LL3;_tmp5=_tmp3.f2;if((int)
_tmp5 != 0)goto _LL3;_LL2: goto _LL0;_LL3: _tmp6=_tmp3.f1;if(_tmp6 <= (void*)1?1:*((
int*)_tmp6)!= 0)goto _LL5;_tmp7=((struct Cyc_Absyn_Rel_n_struct*)_tmp6)->f1;_tmp8=
_tmp3.f2;if(_tmp8 <= (void*)1?1:*((int*)_tmp8)!= 0)goto _LL5;_tmp9=((struct Cyc_Absyn_Rel_n_struct*)
_tmp8)->f1;_LL4: _tmpB=_tmp7;_tmpD=_tmp9;goto _LL6;_LL5: _tmpA=_tmp3.f1;if(_tmpA <= (
void*)1?1:*((int*)_tmpA)!= 1)goto _LL7;_tmpB=((struct Cyc_Absyn_Abs_n_struct*)
_tmpA)->f1;_tmpC=_tmp3.f2;if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 1)goto _LL7;
_tmpD=((struct Cyc_Absyn_Abs_n_struct*)_tmpC)->f1;_LL6: {int i=Cyc_Absyn_zstrlist_cmp(
_tmpB,_tmpD);if(i != 0)return i;goto _LL0;}_LL7: _tmpE=_tmp3.f1;if((int)_tmpE != 0)
goto _LL9;_LL8: return - 1;_LL9: _tmpF=_tmp3.f2;if((int)_tmpF != 0)goto _LLB;_LLA:
return 1;_LLB: _tmp10=_tmp3.f1;if(_tmp10 <= (void*)1?1:*((int*)_tmp10)!= 0)goto _LLD;
_LLC: return - 1;_LLD: _tmp11=_tmp3.f2;if(_tmp11 <= (void*)1?1:*((int*)_tmp11)!= 0)
goto _LL0;_LLE: return 1;_LL0:;}return Cyc_Std_zstrptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_Std_zstrptrcmp(tv1->name,
tv2->name);if(i != 0)return i;if(tv1->identity == tv2->identity)return 0;if(tv1->identity
!= 0?tv2->identity != 0: 0)return*((int*)_check_null(tv1->identity))- *((int*)
_check_null(tv2->identity));else{if(tv1->identity == 0)return - 1;else{return 1;}}}
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value={0,0};void*Cyc_Absyn_rel_ns_null=(
void*)& Cyc_Absyn_rel_ns_null_value;int Cyc_Absyn_is_qvar_qualified(struct _tuple0*
qv){void*_tmp13=(*qv).f1;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp15;
_LL10: if(_tmp13 <= (void*)1?1:*((int*)_tmp13)!= 0)goto _LL12;_tmp14=((struct Cyc_Absyn_Rel_n_struct*)
_tmp13)->f1;if(_tmp14 != 0)goto _LL12;_LL11: goto _LL13;_LL12: if(_tmp13 <= (void*)1?1:*((
int*)_tmp13)!= 1)goto _LL14;_tmp15=((struct Cyc_Absyn_Abs_n_struct*)_tmp13)->f1;
if(_tmp15 != 0)goto _LL14;_LL13: goto _LL15;_LL14: if((int)_tmp13 != 0)goto _LL16;_LL15:
return 0;_LL16:;_LL17: return 1;_LLF:;}static int Cyc_Absyn_new_type_counter=0;void*
Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){return(void*)({
struct Cyc_Absyn_Evar_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({struct
Cyc_Absyn_Evar_struct _tmp17;_tmp17.tag=0;_tmp17.f1=k;_tmp17.f2=0;_tmp17.f3=Cyc_Absyn_new_type_counter
++;_tmp17.f4=env;_tmp17;});_tmp16;});}static struct Cyc_Core_Opt Cyc_Absyn_mk={(
void*)((void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({struct Cyc_Absyn_Tqual
_tmp18;_tmp18.q_const=x.q_const?1: y.q_const;_tmp18.q_volatile=x.q_volatile?1: y.q_volatile;
_tmp18.q_restrict=x.q_restrict?1: y.q_restrict;_tmp18;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(){return({struct Cyc_Absyn_Tqual _tmp19;_tmp19.q_const=0;
_tmp19.q_volatile=0;_tmp19.q_restrict=0;_tmp19;});}struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(){
return({struct Cyc_Absyn_Tqual _tmp1A;_tmp1A.q_const=1;_tmp1A.q_volatile=0;_tmp1A.q_restrict=
0;_tmp1A;});}struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x){return({struct
Cyc_Absyn_Conref*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(void*)x;_tmp1D;});
_tmp1C;}));_tmp1B;});}struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref(){return({
struct Cyc_Absyn_Conref*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->v=(void*)((void*)
0);_tmp1E;});}static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_constraint={0,(
void*)1};static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_constraint={0,(
void*)0};struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={(void*)((void*)& Cyc_Absyn_true_constraint)};
struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(void*)((void*)& Cyc_Absyn_false_constraint)};
struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x){void*_tmp21=(void*)x->v;struct Cyc_Absyn_Conref*_tmp22;
struct Cyc_Absyn_Conref**_tmp23;_LL19: if((int)_tmp21 != 0)goto _LL1B;_LL1A: goto
_LL1C;_LL1B: if(_tmp21 <= (void*)1?1:*((int*)_tmp21)!= 0)goto _LL1D;_LL1C: return x;
_LL1D: if(_tmp21 <= (void*)1?1:*((int*)_tmp21)!= 1)goto _LL18;_tmp22=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp21)->f1;_tmp23=(struct Cyc_Absyn_Conref**)&((struct Cyc_Absyn_Forward_constr_struct*)
_tmp21)->f1;_LL1E: {struct Cyc_Absyn_Conref*_tmp24=Cyc_Absyn_compress_conref(*
_tmp23);*_tmp23=_tmp24;return _tmp24;}_LL18:;}void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x){void*_tmp25=(void*)(Cyc_Absyn_compress_conref(x))->v;void*_tmp26;_LL20: if(
_tmp25 <= (void*)1?1:*((int*)_tmp25)!= 0)goto _LL22;_tmp26=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp25)->f1;_LL21: return _tmp26;_LL22:;_LL23:({void*_tmp27[0]={};Cyc_Tcutil_impos(
_tag_arr("conref_val",sizeof(char),11),_tag_arr(_tmp27,sizeof(void*),0));});
_LL1F:;}void*Cyc_Absyn_conref_def(void*y,struct Cyc_Absyn_Conref*x){void*_tmp28=(
void*)(Cyc_Absyn_compress_conref(x))->v;void*_tmp29;_LL25: if(_tmp28 <= (void*)1?1:*((
int*)_tmp28)!= 0)goto _LL27;_tmp29=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp28)->f1;_LL26: return _tmp29;_LL27:;_LL28: return y;_LL24:;}void*Cyc_Absyn_compress_kb(
void*k){void*_tmp2A=k;struct Cyc_Core_Opt*_tmp2B;struct Cyc_Core_Opt*_tmp2C;struct
Cyc_Core_Opt*_tmp2D;struct Cyc_Core_Opt _tmp2E;void*_tmp2F;void**_tmp30;struct Cyc_Core_Opt*
_tmp31;struct Cyc_Core_Opt _tmp32;void*_tmp33;void**_tmp34;_LL2A: if(*((int*)_tmp2A)
!= 0)goto _LL2C;_LL2B: goto _LL2D;_LL2C: if(*((int*)_tmp2A)!= 1)goto _LL2E;_tmp2B=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp2A)->f1;if(_tmp2B != 0)goto _LL2E;_LL2D:
goto _LL2F;_LL2E: if(*((int*)_tmp2A)!= 2)goto _LL30;_tmp2C=((struct Cyc_Absyn_Less_kb_struct*)
_tmp2A)->f1;if(_tmp2C != 0)goto _LL30;_LL2F: return k;_LL30: if(*((int*)_tmp2A)!= 1)
goto _LL32;_tmp2D=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp2A)->f1;if(_tmp2D == 0)
goto _LL32;_tmp2E=*_tmp2D;_tmp2F=(void*)_tmp2E.v;_tmp30=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp2A)->f1).v;_LL31: _tmp34=_tmp30;goto _LL33;_LL32: if(*((int*)_tmp2A)!= 2)goto
_LL29;_tmp31=((struct Cyc_Absyn_Less_kb_struct*)_tmp2A)->f1;if(_tmp31 == 0)goto
_LL29;_tmp32=*_tmp31;_tmp33=(void*)_tmp32.v;_tmp34=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp2A)->f1).v;_LL33:*_tmp34=Cyc_Absyn_compress_kb(*_tmp34);return*_tmp34;_LL29:;}
void*Cyc_Absyn_force_kb(void*kb){void*_tmp35=Cyc_Absyn_compress_kb(kb);void*
_tmp36;struct Cyc_Core_Opt*_tmp37;struct Cyc_Core_Opt**_tmp38;struct Cyc_Core_Opt*
_tmp39;struct Cyc_Core_Opt**_tmp3A;void*_tmp3B;_LL35: if(*((int*)_tmp35)!= 0)goto
_LL37;_tmp36=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp35)->f1;_LL36: return
_tmp36;_LL37: if(*((int*)_tmp35)!= 1)goto _LL39;_tmp37=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp35)->f1;_tmp38=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp35)->f1;_LL38: _tmp3A=_tmp38;_tmp3B=(void*)2;goto _LL3A;_LL39: if(*((int*)
_tmp35)!= 2)goto _LL34;_tmp39=((struct Cyc_Absyn_Less_kb_struct*)_tmp35)->f1;
_tmp3A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp35)->f1;
_tmp3B=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp35)->f2;_LL3A:*_tmp3A=({
struct Cyc_Core_Opt*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->v=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({
struct Cyc_Absyn_Eq_kb_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(void*)_tmp3B;_tmp3E;});
_tmp3D;}));_tmp3C;});return _tmp3B;_LL34:;}static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_char_tt={5,(void*)((void*)2),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uchar_tt={5,(void*)((void*)1),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={5,(void*)((void*)1),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uint_tt={5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulong_tt={5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulonglong_tt={5,(void*)((void*)1),(void*)((void*)3)};void*Cyc_Absyn_char_typ=(
void*)& Cyc_Absyn_char_tt;void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;
void*Cyc_Absyn_ushort_typ=(void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(
void*)& Cyc_Absyn_uint_tt;void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;
void*Cyc_Absyn_ulonglong_typ=(void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_schar_tt={5,(void*)((void*)0),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={5,(void*)((void*)0),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sint_tt={5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slong_tt={5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slonglong_tt={5,(void*)((void*)0),(void*)((void*)3)};void*Cyc_Absyn_schar_typ=(
void*)& Cyc_Absyn_schar_tt;void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;
void*Cyc_Absyn_sint_typ=(void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)&
Cyc_Absyn_slong_tt;void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;
void*Cyc_Absyn_float_typ=(void*)1;void*Cyc_Absyn_double_typ(int b){return(void*)({
struct Cyc_Absyn_DoubleType_struct*_tmp4A=_cycalloc_atomic(sizeof(*_tmp4A));
_tmp4A[0]=({struct Cyc_Absyn_DoubleType_struct _tmp4B;_tmp4B.tag=6;_tmp4B.f1=b;
_tmp4B;});_tmp4A;});}static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_abs_null={1,0};
static char _tmp4D[4]="exn";static struct _tagged_arr Cyc_Absyn_exn_str={_tmp4D,
_tmp4D,_tmp4D + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={(void*)& Cyc_Absyn_abs_null,&
Cyc_Absyn_exn_str};struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static
char _tmp4E[15]="Null_Exception";static struct _tagged_arr Cyc_Absyn_Null_Exception_str={
_tmp4E,_tmp4E,_tmp4E + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={(void*)&
Cyc_Absyn_abs_null,& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr,0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp4F[13]="Array_bounds";
static struct _tagged_arr Cyc_Absyn_Array_bounds_str={_tmp4F,_tmp4F,_tmp4F + 13};
static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp50[16]="Match_Exception";static struct
_tagged_arr Cyc_Absyn_Match_Exception_str={_tmp50,_tmp50,_tmp50 + 16};static struct
_tuple0 Cyc_Absyn_Match_Exception_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Match_Exception_str};
struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;
static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp51[10]="Bad_alloc";static struct
_tagged_arr Cyc_Absyn_Bad_alloc_str={_tmp51,_tmp51,_tmp51 + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={(void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Bad_alloc_str};
struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Tunionfield
Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,(void*)((void*)3)};struct
Cyc_Absyn_Tunionfield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;static
struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,0};
static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={(void*)((void*)3),& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1};struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Absyn_KnownTunion_struct Cyc_Absyn_exn_tinfou={1,& Cyc_Absyn_exn_tud};
static struct Cyc_Absyn_TunionType_struct Cyc_Absyn_exn_typ_tt={2,{(void*)((void*)&
Cyc_Absyn_exn_tinfou),0,(void*)((void*)2)}};void*Cyc_Absyn_exn_typ=(void*)& Cyc_Absyn_exn_typ_tt;
static char _tmp54[9]="PrintArg";static struct _tagged_arr Cyc_Absyn_printarg_str={
_tmp54,_tmp54,_tmp54 + 9};static char _tmp55[9]="ScanfArg";static struct _tagged_arr
Cyc_Absyn_scanfarg_str={_tmp55,_tmp55,_tmp55 + 9};static char _tmp56[4]="Std";
static struct _tagged_arr Cyc_Absyn_std_str={_tmp56,_tmp56,_tmp56 + 4};static struct
Cyc_List_List Cyc_Absyn_std_list={(void*)& Cyc_Absyn_std_str,0};static struct Cyc_Absyn_Abs_n_struct
Cyc_Absyn_std_namespace={1,(struct Cyc_List_List*)& Cyc_Absyn_std_list};static
struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(void*)& Cyc_Absyn_std_namespace,&
Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_tunion_scanf_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar=&
Cyc_Absyn_tunion_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar=&
Cyc_Absyn_tunion_scanf_arg_qvar_p;static struct Cyc_Core_Opt*Cyc_Absyn_string_t_opt=
0;void*Cyc_Absyn_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(),(void*)0);else{if(Cyc_Absyn_string_t_opt
== 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(),(
void*)0);Cyc_Absyn_string_t_opt=({struct Cyc_Core_Opt*_tmp58=_cycalloc(sizeof(*
_tmp58));_tmp58->v=(void*)t;_tmp58;});}return(void*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Absyn_string_t_opt))->v;}}static struct Cyc_Core_Opt*Cyc_Absyn_const_string_t_opt=
0;void*Cyc_Absyn_const_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(),(void*)0);else{if(Cyc_Absyn_const_string_t_opt
== 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(),(
void*)0);Cyc_Absyn_const_string_t_opt=({struct Cyc_Core_Opt*_tmp59=_cycalloc(
sizeof(*_tmp59));_tmp59->v=(void*)t;_tmp59;});}return(void*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Absyn_const_string_t_opt))->v;}}static struct Cyc_Absyn_Int_c_struct
Cyc_Absyn_one_intc={2,(void*)((void*)1),1};static struct Cyc_Absyn_Const_e_struct
Cyc_Absyn_one_b_raw={0,(void*)((void*)& Cyc_Absyn_one_intc)};struct Cyc_Absyn_Exp
Cyc_Absyn_exp_unsigned_one_v={0,(void*)((void*)& Cyc_Absyn_one_b_raw),0,(void*)((
void*)Cyc_Absyn_EmptyAnnot)};struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={0,& Cyc_Absyn_exp_unsigned_one_v};
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;void*Cyc_Absyn_starb_typ(void*
t,void*r,struct Cyc_Absyn_Tqual tq,void*b){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Absyn_PointerType_struct
_tmp5E;_tmp5E.tag=4;_tmp5E.f1=({struct Cyc_Absyn_PtrInfo _tmp5F;_tmp5F.elt_typ=(
void*)t;_tmp5F.rgn_typ=(void*)r;_tmp5F.nullable=Cyc_Absyn_true_conref;_tmp5F.tq=
tq;_tmp5F.bounds=Cyc_Absyn_new_conref(b);_tmp5F;});_tmp5E;});_tmp5D;});}void*Cyc_Absyn_atb_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=({struct Cyc_Absyn_PointerType_struct
_tmp61;_tmp61.tag=4;_tmp61.f1=({struct Cyc_Absyn_PtrInfo _tmp62;_tmp62.elt_typ=(
void*)t;_tmp62.rgn_typ=(void*)r;_tmp62.nullable=Cyc_Absyn_false_conref;_tmp62.tq=
tq;_tmp62.bounds=Cyc_Absyn_new_conref(b);_tmp62;});_tmp61;});_tmp60;});}void*Cyc_Absyn_star_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq){return Cyc_Absyn_starb_typ(t,r,tq,Cyc_Absyn_bounds_one);}
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){return Cyc_Absyn_starb_typ(
t,(void*)2,tq,Cyc_Absyn_bounds_one);}void*Cyc_Absyn_at_typ(void*t,void*r,struct
Cyc_Absyn_Tqual tq){return Cyc_Absyn_atb_typ(t,r,tq,Cyc_Absyn_bounds_one);}void*
Cyc_Absyn_tagged_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({
struct Cyc_Absyn_PointerType_struct _tmp64;_tmp64.tag=4;_tmp64.f1=({struct Cyc_Absyn_PtrInfo
_tmp65;_tmp65.elt_typ=(void*)t;_tmp65.rgn_typ=(void*)r;_tmp65.nullable=Cyc_Absyn_true_conref;
_tmp65.tq=tq;_tmp65.bounds=Cyc_Absyn_new_conref((void*)0);_tmp65;});_tmp64;});
_tmp63;});}static struct Cyc_Core_Opt*Cyc_Absyn_file_t_opt=0;static char _tmp66[8]="__sFILE";
static struct _tagged_arr Cyc_Absyn_sf_str={_tmp66,_tmp66,_tmp66 + 8};static char
_tmp67[4]="Std";static struct _tagged_arr Cyc_Absyn_st_str={_tmp67,_tmp67,_tmp67 + 4};
static struct _tagged_arr*Cyc_Absyn_sf=& Cyc_Absyn_sf_str;static struct _tagged_arr*
Cyc_Absyn_st=& Cyc_Absyn_st_str;void*Cyc_Absyn_file_typ(){if(Cyc_Absyn_file_t_opt
== 0){struct _tuple0*file_t_name=({struct _tuple0*_tmp70=_cycalloc(sizeof(*_tmp70));
_tmp70->f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp71=_cycalloc(sizeof(*_tmp71));
_tmp71[0]=({struct Cyc_Absyn_Abs_n_struct _tmp72;_tmp72.tag=1;_tmp72.f1=({struct
_tagged_arr*_tmp73[1];_tmp73[0]=Cyc_Absyn_st;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp73,sizeof(struct _tagged_arr*),1));});
_tmp72;});_tmp71;});_tmp70->f2=Cyc_Absyn_sf;_tmp70;});struct Cyc_Absyn_Aggrdecl*
sd=({struct Cyc_Absyn_Aggrdecl*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->kind=(
void*)((void*)0);_tmp6F->sc=(void*)((void*)1);_tmp6F->name=file_t_name;_tmp6F->tvs=
0;_tmp6F->impl=0;_tmp6F->attributes=0;_tmp6F;});void*file_struct_typ=(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=({
struct Cyc_Absyn_AggrType_struct _tmp6A;_tmp6A.tag=10;_tmp6A.f1=({struct Cyc_Absyn_AggrInfo
_tmp6B;_tmp6B.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp6C=
_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp6D;
_tmp6D.tag=1;_tmp6D.f1=({struct Cyc_Absyn_Aggrdecl**_tmp6E=_cycalloc(sizeof(*
_tmp6E));_tmp6E[0]=sd;_tmp6E;});_tmp6D;});_tmp6C;}));_tmp6B.targs=0;_tmp6B;});
_tmp6A;});_tmp69;});Cyc_Absyn_file_t_opt=({struct Cyc_Core_Opt*_tmp68=_cycalloc(
sizeof(*_tmp68));_tmp68->v=(void*)Cyc_Absyn_at_typ(file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual());
_tmp68;});}return(void*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_file_t_opt))->v;}
static struct Cyc_Core_Opt*Cyc_Absyn_void_star_t_opt=0;void*Cyc_Absyn_void_star_typ(){
if(Cyc_Absyn_void_star_t_opt == 0)Cyc_Absyn_void_star_t_opt=({struct Cyc_Core_Opt*
_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->v=(void*)Cyc_Absyn_star_typ((void*)0,(
void*)2,Cyc_Absyn_empty_tqual());_tmp74;});return(void*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Absyn_void_star_t_opt))->v;}struct Cyc_Core_Opt*Cyc_Absyn_void_star_typ_opt(){
if(Cyc_Absyn_void_star_t_opt == 0)Cyc_Absyn_void_star_t_opt=({struct Cyc_Core_Opt*
_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->v=(void*)Cyc_Absyn_star_typ((void*)0,(
void*)2,Cyc_Absyn_empty_tqual());_tmp75;});return Cyc_Absyn_void_star_t_opt;}
static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={20,0};void*Cyc_Absyn_empty_effect=(
void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(void*k,struct _tagged_arr*name){
return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp77=_cycalloc(sizeof(*_tmp77));
_tmp77[0]=({struct Cyc_Absyn_AggrType_struct _tmp78;_tmp78.tag=10;_tmp78.f1=({
struct Cyc_Absyn_AggrInfo _tmp79;_tmp79.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp7B;_tmp7B.tag=0;_tmp7B.f1=(void*)k;_tmp7B.f2=({struct _tuple0*_tmp7C=
_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=Cyc_Absyn_rel_ns_null;_tmp7C->f2=name;
_tmp7C;});_tmp7B;});_tmp7A;}));_tmp79.targs=0;_tmp79;});_tmp78;});_tmp77;});}
void*Cyc_Absyn_strct(struct _tagged_arr*name){return Cyc_Absyn_aggr_typ((void*)0,
name);}void*Cyc_Absyn_union_typ(struct _tagged_arr*name){return Cyc_Absyn_aggr_typ((
void*)1,name);}void*Cyc_Absyn_strctq(struct _tuple0*name){return(void*)({struct
Cyc_Absyn_AggrType_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct
Cyc_Absyn_AggrType_struct _tmp7E;_tmp7E.tag=10;_tmp7E.f1=({struct Cyc_Absyn_AggrInfo
_tmp7F;_tmp7F.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmp81;_tmp81.tag=0;_tmp81.f1=(void*)((void*)0);_tmp81.f2=name;_tmp81;});_tmp80;}));
_tmp7F.targs=0;_tmp7F;});_tmp7E;});_tmp7D;});}void*Cyc_Absyn_unionq_typ(struct
_tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp82=_cycalloc(
sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Absyn_AggrType_struct _tmp83;_tmp83.tag=10;
_tmp83.f1=({struct Cyc_Absyn_AggrInfo _tmp84;_tmp84.aggr_info=(void*)((void*)({
struct Cyc_Absyn_UnknownAggr_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({
struct Cyc_Absyn_UnknownAggr_struct _tmp86;_tmp86.tag=0;_tmp86.f1=(void*)((void*)1);
_tmp86.f2=name;_tmp86;});_tmp85;}));_tmp84.targs=0;_tmp84;});_tmp83;});_tmp82;});}
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,struct Cyc_Position_Segment*loc){
return({struct Cyc_Absyn_Exp*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->topt=0;
_tmp87->r=(void*)r;_tmp87->loc=loc;_tmp87->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);
_tmp87;});}struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_New_e_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88[0]=({
struct Cyc_Absyn_New_e_struct _tmp89;_tmp89.tag=15;_tmp89.f1=rgn_handle;_tmp89.f2=
e;_tmp89;});_tmp88;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*
e){return({struct Cyc_Absyn_Exp*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=*e;
_tmp8A;});}struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(void*c,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*_tmp8B=
_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Absyn_Const_e_struct _tmp8C;
_tmp8C.tag=0;_tmp8C.f1=(void*)c;_tmp8C;});_tmp8B;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Const_e_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({
struct Cyc_Absyn_Const_e_struct _tmp8E;_tmp8E.tag=0;_tmp8E.f1=(void*)((void*)0);
_tmp8E;});_tmp8D;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*s,int i,
struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_Int_c_struct _tmp90;
_tmp90.tag=2;_tmp90.f1=(void*)s;_tmp90.f2=i;_tmp90;});_tmp8F;}),seg);}struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((
void*)0,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)1,(int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(
1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){
return Cyc_Absyn_bool_exp(0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Char_c_struct*
_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_Char_c_struct
_tmp92;_tmp92.tag=0;_tmp92.f1=(void*)((void*)2);_tmp92.f2=c;_tmp92;});_tmp91;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _tagged_arr f,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp((void*)({struct Cyc_Absyn_Float_c_struct*_tmp93=
_cycalloc(sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Absyn_Float_c_struct _tmp94;
_tmp94.tag=4;_tmp94.f1=f;_tmp94;});_tmp93;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _tagged_arr s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((
void*)({struct Cyc_Absyn_String_c_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[
0]=({struct Cyc_Absyn_String_c_struct _tmp96;_tmp96.tag=5;_tmp96.f1=s;_tmp96;});
_tmp95;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmp97=
_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Absyn_Var_e_struct _tmp98;_tmp98.tag=
1;_tmp98.f1=q;_tmp98.f2=(void*)((void*)0);_tmp98;});_tmp97;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmp99=_cycalloc(sizeof(*
_tmp99));_tmp99[0]=({struct Cyc_Absyn_Var_e_struct _tmp9A;_tmp9A.tag=1;_tmp9A.f1=q;
_tmp9A.f2=(void*)b;_tmp9A;});_tmp99;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp9C;_tmp9C.tag=2;_tmp9C.f1=q;_tmp9C;});
_tmp9B;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp9E;_tmp9E.tag=3;_tmp9E.f1=(void*)p;_tmp9E.f2=es;_tmp9E;});_tmp9D;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*
_tmp9F));_tmp9F->hd=e;_tmp9F->tl=0;_tmp9F;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpA0=_cycalloc(sizeof(*
_tmpA0));_tmpA0->hd=e1;_tmpA0->tl=({struct Cyc_List_List*_tmpA1=_cycalloc(sizeof(*
_tmpA1));_tmpA1->hd=e2;_tmpA1->tl=0;_tmpA1;});_tmpA0;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_prim2_exp((void*)0,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)1,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)3,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)5,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)6,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)7,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)8,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)9,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)10,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmpA2=
_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmpA3;
_tmpA3.tag=4;_tmpA3.f1=e1;_tmpA3.f2=popt;_tmpA3.f3=e2;_tmpA3;});_tmpA2;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_Increment_e_struct
_tmpA5;_tmpA5.tag=5;_tmpA5.f1=e;_tmpA5.f2=(void*)i;_tmpA5;});_tmpA4;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmpA7;_tmpA7.tag=6;_tmpA7.f1=e1;_tmpA7.f2=e2;_tmpA7.f3=e3;_tmpA7;});_tmpA6;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,e2,Cyc_Absyn_false_exp(
loc),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_conditional_exp(e1,Cyc_Absyn_true_exp(
loc),e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[
0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpA9;_tmpA9.tag=7;_tmpA9.f1=e1;_tmpA9.f2=
e2;_tmpA9;});_tmpA8;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*_tmpAA=
_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpAB;_tmpAB.tag=8;_tmpAB.f1=e;_tmpAB.f2=es;_tmpAB;});_tmpAA;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmpAC=
_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_Absyn_FnCall_e_struct _tmpAD;
_tmpAD.tag=9;_tmpAD.f1=e;_tmpAD.f2=es;_tmpAD.f3=0;_tmpAD;});_tmpAC;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpAF;_tmpAF.tag=11;_tmpAF.f1=e;_tmpAF;});_tmpAE;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpB1;_tmpB1.tag=12;_tmpB1.f1=e;_tmpB1.f2=ts;_tmpB1;});_tmpB0;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*_tmpB2=
_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_Cast_e_struct _tmpB3;
_tmpB3.tag=13;_tmpB3.f1=(void*)t;_tmpB3.f2=e;_tmpB3;});_tmpB2;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_struct*_tmpB4=_cycalloc(
sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_Throw_e_struct _tmpB5;_tmpB5.tag=10;
_tmpB5.f1=e;_tmpB5;});_tmpB4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Address_e_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));
_tmpB6[0]=({struct Cyc_Absyn_Address_e_struct _tmpB7;_tmpB7.tag=14;_tmpB7.f1=e;
_tmpB7;});_tmpB6;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpB9;_tmpB9.tag=16;_tmpB9.f1=(void*)t;_tmpB9;});_tmpB8;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*_tmpBA=
_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Absyn_Sizeofexp_e_struct _tmpBB;
_tmpBB.tag=17;_tmpBB.f1=e;_tmpBB;});_tmpBA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmpBD;_tmpBD.tag=18;_tmpBD.f1=(void*)t;_tmpBD.f2=(
void*)of;_tmpBD;});_tmpBC;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpBF;_tmpBF.tag=19;_tmpBF.f1=tvs;_tmpBF.f2=(
void*)t;_tmpBF;});_tmpBE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpC1;_tmpC1.tag=20;_tmpC1.f1=e;_tmpC1;});_tmpC0;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpC3;_tmpC3.tag=21;_tmpC3.f1=e;_tmpC3.f2=n;
_tmpC3;});_tmpC2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _tagged_arr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));
_tmpC4[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpC5;_tmpC5.tag=22;_tmpC5.f1=e;
_tmpC5.f2=n;_tmpC5;});_tmpC4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpC6=
_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpC7;
_tmpC7.tag=23;_tmpC7.f1=e1;_tmpC7.f2=e2;_tmpC7;});_tmpC6;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*_tmpC8=_cycalloc(
sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_Tuple_e_struct _tmpC9;_tmpC9.tag=24;
_tmpC9.f1=es;_tmpC9;});_tmpC8;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));
_tmpCA[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpCB;_tmpCB.tag=35;_tmpCB.f1=s;
_tmpCB;});_tmpCA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct _tuple12{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*
_tmpCC));_tmpCC->hd=({struct _tuple12*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->f1=
0;_tmpCD->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpCD;});_tmpCC->tl=dles;_tmpCC;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpCE=_cycalloc(
sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_Array_e_struct _tmpCF;_tmpCF.tag=26;
_tmpCF.f1=dles;_tmpCF;});_tmpCE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD0=
_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD1;_tmpD1.tag=34;_tmpD1.f1=n;_tmpD1.f2=dles;_tmpD1;});_tmpD0;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->r=(void*)s;_tmpD2->loc=
loc;_tmpD2->non_local_preds=0;_tmpD2->try_depth=0;_tmpD2->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpD2;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpD3=_cycalloc(
sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_Exp_s_struct _tmpD4;_tmpD4.tag=0;
_tmpD4.f1=e;_tmpD4;});_tmpD3;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0)return Cyc_Absyn_skip_stmt(
loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpD5=
_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_Return_s_struct _tmpD6;
_tmpD6.tag=2;_tmpD6.f1=e;_tmpD6;});_tmpD5;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpD8;_tmpD8.tag=3;_tmpD8.f1=e;_tmpD8.f2=s1;_tmpD8.f3=s2;_tmpD8;});_tmpD7;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_While_s_struct
_tmpDA;_tmpDA.tag=4;_tmpDA.f1=({struct _tuple2 _tmpDB;_tmpDB.f1=e;_tmpDB.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpDB;});_tmpDA.f2=s;_tmpDA;});_tmpD9;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_Break_s_struct
_tmpDD;_tmpDD.tag=5;_tmpDD.f1=0;_tmpDD;});_tmpDC;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));
_tmpDE[0]=({struct Cyc_Absyn_Continue_s_struct _tmpDF;_tmpDF.tag=6;_tmpDF.f1=0;
_tmpDF;});_tmpDE;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_For_s_struct _tmpE1;
_tmpE1.tag=8;_tmpE1.f1=e1;_tmpE1.f2=({struct _tuple2 _tmpE2;_tmpE2.f1=e2;_tmpE2.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpE2;});_tmpE1.f3=({struct _tuple2 _tmpE3;_tmpE3.f1=
e3;_tmpE3.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmpE3;});_tmpE1.f4=s;_tmpE1;});_tmpE0;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*
_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_Switch_s_struct
_tmpE5;_tmpE5.tag=9;_tmpE5.f1=e;_tmpE5.f2=scs;_tmpE5;});_tmpE4;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){struct _tuple9 _tmpE7=({struct _tuple9 _tmpE6;
_tmpE6.f1=(void*)s1->r;_tmpE6.f2=(void*)s2->r;_tmpE6;});void*_tmpE8;void*_tmpE9;
_LL3C: _tmpE8=_tmpE7.f1;if((int)_tmpE8 != 0)goto _LL3E;_LL3D: return s2;_LL3E: _tmpE9=
_tmpE7.f2;if((int)_tmpE9 != 0)goto _LL40;_LL3F: return s1;_LL40:;_LL41: return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Seq_s_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({
struct Cyc_Absyn_Seq_s_struct _tmpEB;_tmpEB.tag=1;_tmpEB.f1=s1;_tmpEB.f2=s2;_tmpEB;});
_tmpEA;}),loc);_LL3B:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*
el,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Fallthru_s_struct*
_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_Fallthru_s_struct
_tmpED;_tmpED.tag=11;_tmpED.f1=el;_tmpED.f2=0;_tmpED;});_tmpEC;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpEF;_tmpEF.tag=12;_tmpEF.f1=d;_tmpEF.f2=s;_tmpEF;});_tmpEE;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2[0]=({
struct Cyc_Absyn_Var_d_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=Cyc_Absyn_new_vardecl(
x,t,init);_tmpF3;});_tmpF2;}),loc);return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpF1;_tmpF1.tag=12;_tmpF1.f1=d;_tmpF1.f2=s;_tmpF1;});_tmpF0;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Cut_s_struct*_tmpF4=_cycalloc(
sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_Cut_s_struct _tmpF5;_tmpF5.tag=13;
_tmpF5.f1=s;_tmpF5;});_tmpF4;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Splice_s_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[
0]=({struct Cyc_Absyn_Splice_s_struct _tmpF7;_tmpF7.tag=14;_tmpF7.f1=s;_tmpF7;});
_tmpF6;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _tagged_arr*v,
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8[
0]=({struct Cyc_Absyn_Label_s_struct _tmpF9;_tmpF9.tag=15;_tmpF9.f1=v;_tmpF9.f2=s;
_tmpF9;});_tmpF8;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Do_s_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA[0]=({
struct Cyc_Absyn_Do_s_struct _tmpFB;_tmpFB.tag=16;_tmpFB.f1=s;_tmpFB.f2=({struct
_tuple2 _tmpFC;_tmpFC.f1=e;_tmpFC.f2=Cyc_Absyn_skip_stmt(e->loc);_tmpFC;});_tmpFB;});
_tmpFA;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*
s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_TryCatch_s_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));
_tmpFD[0]=({struct Cyc_Absyn_TryCatch_s_struct _tmpFE;_tmpFE.tag=17;_tmpFE.f1=s;
_tmpFE.f2=scs;_tmpFE;});_tmpFD;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _tagged_arr*lab,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Goto_s_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({
struct Cyc_Absyn_Goto_s_struct _tmp100;_tmp100.tag=7;_tmp100.f1=lab;_tmp100.f2=0;
_tmp100;});_tmpFF;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_forarray_stmt(
struct Cyc_List_List*d,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_ForArray_s_struct*
_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[0]=({struct Cyc_Absyn_ForArray_s_struct
_tmp102;_tmp102.tag=19;_tmp102.f1=({struct Cyc_Absyn_ForArrayInfo _tmp103;_tmp103.defns=
d;_tmp103.condition=({struct _tuple2 _tmp105;_tmp105.f1=e1;_tmp105.f2=Cyc_Absyn_skip_stmt(
e1->loc);_tmp105;});_tmp103.delta=({struct _tuple2 _tmp104;_tmp104.f1=e2;_tmp104.f2=
Cyc_Absyn_skip_stmt(e2->loc);_tmp104;});_tmp103.body=s;_tmp103;});_tmp102;});
_tmp101;}),loc);}struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*
s){return({struct Cyc_Absyn_Pat*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->r=(
void*)p;_tmp106->topt=0;_tmp106->loc=s;_tmp106;});}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp107=
_cycalloc(sizeof(*_tmp107));_tmp107->r=(void*)r;_tmp107->loc=loc;_tmp107;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*
_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108[0]=({struct Cyc_Absyn_Let_d_struct
_tmp109;_tmp109.tag=2;_tmp109.f1=p;_tmp109.f2=0;_tmp109.f3=e;_tmp109;});_tmp108;}),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp10A=
_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Absyn_Letv_d_struct _tmp10B;
_tmp10B.tag=3;_tmp10B.f1=vds;_tmp10B;});_tmp10A;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->sc=(void*)((
void*)2);_tmp10C->name=x;_tmp10C->tq=Cyc_Absyn_empty_tqual();_tmp10C->type=(void*)
t;_tmp10C->initializer=init;_tmp10C->rgn=0;_tmp10C->attributes=0;_tmp10C->escapes=
0;_tmp10C;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp10D=
_cycalloc(sizeof(*_tmp10D));_tmp10D->sc=(void*)((void*)0);_tmp10D->name=x;
_tmp10D->tq=Cyc_Absyn_empty_tqual();_tmp10D->type=(void*)t;_tmp10D->initializer=
init;_tmp10D->rgn=0;_tmp10D->attributes=0;_tmp10D->escapes=0;_tmp10D;});}struct
Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct
Cyc_List_List*po,struct Cyc_List_List*fs){return({struct Cyc_Absyn_AggrdeclImpl*
_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->exist_vars=exists;_tmp10E->rgn_po=
po;_tmp10E->fields=fs;_tmp10E;});}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*
k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));
_tmp10F[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp110;_tmp110.tag=4;_tmp110.f1=({
struct Cyc_Absyn_Aggrdecl*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->kind=(void*)
k;_tmp111->sc=(void*)s;_tmp111->name=n;_tmp111->tvs=ts;_tmp111->impl=i;_tmp111->attributes=
atts;_tmp111;});_tmp110;});_tmp10F;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((void*)1,s,n,ts,i,
atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_xtunion,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp112=
_cycalloc(sizeof(*_tmp112));_tmp112[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp113;
_tmp113.tag=5;_tmp113.f1=({struct Cyc_Absyn_Tuniondecl*_tmp114=_cycalloc(sizeof(*
_tmp114));_tmp114->sc=(void*)s;_tmp114->name=n;_tmp114->tvs=ts;_tmp114->fields=
fs;_tmp114->is_xtunion=is_xtunion;_tmp114;});_tmp113;});_tmp112;}),loc);}static
struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){return({struct _tuple1*
_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->f1=(*a).f1;_tmp115->f2=(*a).f2;
_tmp115->f3=Cyc_Absyn_pointer_expand((*a).f3);_tmp115;});}void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts){return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116[0]=({struct Cyc_Absyn_FnType_struct
_tmp117;_tmp117.tag=8;_tmp117.f1=({struct Cyc_Absyn_FnInfo _tmp118;_tmp118.tvars=
tvs;_tmp118.ret_typ=(void*)Cyc_Absyn_pointer_expand(ret_typ);_tmp118.effect=
eff_typ;_tmp118.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple1*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,args);_tmp118.c_varargs=
c_varargs;_tmp118.cyc_varargs=cyc_varargs;_tmp118.rgn_po=rgn_po;_tmp118.attributes=
atts;_tmp118;});_tmp117;});_tmp116;});}void*Cyc_Absyn_pointer_expand(void*t){
void*_tmp119=Cyc_Tcutil_compress(t);_LL43: if(_tmp119 <= (void*)3?1:*((int*)
_tmp119)!= 8)goto _LL45;_LL44: return Cyc_Absyn_at_typ(t,(void*)2,Cyc_Absyn_empty_tqual());
_LL45:;_LL46: return t;_LL42:;}int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){void*
_tmp11A=(void*)e->r;void*_tmp11B;void*_tmp11C;struct Cyc_Absyn_Vardecl*_tmp11D;
void*_tmp11E;struct Cyc_Absyn_Vardecl*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct
Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;_LL48: if(*((int*)_tmp11A)!= 1)
goto _LL4A;_tmp11B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11A)->f2;if(_tmp11B
<= (void*)1?1:*((int*)_tmp11B)!= 1)goto _LL4A;_LL49: return 0;_LL4A: if(*((int*)
_tmp11A)!= 1)goto _LL4C;_tmp11C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11A)->f2;
if(_tmp11C <= (void*)1?1:*((int*)_tmp11C)!= 0)goto _LL4C;_tmp11D=((struct Cyc_Absyn_Global_b_struct*)
_tmp11C)->f1;_LL4B: _tmp11F=_tmp11D;goto _LL4D;_LL4C: if(*((int*)_tmp11A)!= 1)goto
_LL4E;_tmp11E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11A)->f2;if(_tmp11E <= (
void*)1?1:*((int*)_tmp11E)!= 3)goto _LL4E;_tmp11F=((struct Cyc_Absyn_Local_b_struct*)
_tmp11E)->f1;_LL4D: {void*_tmp123=Cyc_Tcutil_compress((void*)_tmp11F->type);
_LL5F: if(_tmp123 <= (void*)3?1:*((int*)_tmp123)!= 7)goto _LL61;_LL60: return 0;_LL61:;
_LL62: return 1;_LL5E:;}_LL4E: if(*((int*)_tmp11A)!= 1)goto _LL50;_LL4F: goto _LL51;
_LL50: if(*((int*)_tmp11A)!= 22)goto _LL52;_LL51: goto _LL53;_LL52: if(*((int*)
_tmp11A)!= 20)goto _LL54;_LL53: goto _LL55;_LL54: if(*((int*)_tmp11A)!= 23)goto _LL56;
_LL55: return 1;_LL56: if(*((int*)_tmp11A)!= 21)goto _LL58;_tmp120=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp11A)->f1;_LL57: return Cyc_Absyn_is_lvalue(_tmp120);_LL58: if(*((int*)_tmp11A)
!= 12)goto _LL5A;_tmp121=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp11A)->f1;
_LL59: return Cyc_Absyn_is_lvalue(_tmp121);_LL5A: if(*((int*)_tmp11A)!= 11)goto
_LL5C;_tmp122=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp11A)->f1;_LL5B:
return Cyc_Absyn_is_lvalue(_tmp122);_LL5C:;_LL5D: return 0;_LL47:;}struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _tagged_arr*v){{struct
Cyc_List_List*_tmp124=fields;for(0;_tmp124 != 0;_tmp124=_tmp124->tl){if(Cyc_Std_zstrptrcmp(((
struct Cyc_Absyn_Aggrfield*)_tmp124->hd)->name,v)== 0)return(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_Absyn_Aggrfield*)_tmp124->hd);}}return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _tagged_arr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple3*
Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts
== 0)return 0;ts=ts->tl;}if(ts == 0)return 0;return(struct _tuple3*)((struct _tuple3*)
ts->hd);}struct _tagged_arr Cyc_Absyn_attribute2string(void*a){void*_tmp125=a;int
_tmp126;int _tmp127;struct _tagged_arr _tmp128;void*_tmp129;int _tmp12A;int _tmp12B;
void*_tmp12C;int _tmp12D;int _tmp12E;_LL64: if(_tmp125 <= (void*)16?1:*((int*)
_tmp125)!= 0)goto _LL66;_tmp126=((struct Cyc_Absyn_Regparm_att_struct*)_tmp125)->f1;
_LL65: return(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp130;_tmp130.tag=
1;_tmp130.f1=(int)((unsigned int)_tmp126);{void*_tmp12F[1]={& _tmp130};Cyc_Std_aprintf(
_tag_arr("regparm(%d)",sizeof(char),12),_tag_arr(_tmp12F,sizeof(void*),1));}});
_LL66: if((int)_tmp125 != 0)goto _LL68;_LL67: return _tag_arr("stdcall",sizeof(char),
8);_LL68: if((int)_tmp125 != 1)goto _LL6A;_LL69: return _tag_arr("cdecl",sizeof(char),
6);_LL6A: if((int)_tmp125 != 2)goto _LL6C;_LL6B: return _tag_arr("fastcall",sizeof(
char),9);_LL6C: if((int)_tmp125 != 3)goto _LL6E;_LL6D: return _tag_arr("noreturn",
sizeof(char),9);_LL6E: if((int)_tmp125 != 4)goto _LL70;_LL6F: return _tag_arr("const",
sizeof(char),6);_LL70: if(_tmp125 <= (void*)16?1:*((int*)_tmp125)!= 1)goto _LL72;
_tmp127=((struct Cyc_Absyn_Aligned_att_struct*)_tmp125)->f1;_LL71: if(_tmp127 == - 1)
return _tag_arr("aligned",sizeof(char),8);else{return(struct _tagged_arr)({struct
Cyc_Std_Int_pa_struct _tmp132;_tmp132.tag=1;_tmp132.f1=(int)((unsigned int)
_tmp127);{void*_tmp131[1]={& _tmp132};Cyc_Std_aprintf(_tag_arr("aligned(%d)",
sizeof(char),12),_tag_arr(_tmp131,sizeof(void*),1));}});}_LL72: if((int)_tmp125 != 
5)goto _LL74;_LL73: return _tag_arr("packed",sizeof(char),7);_LL74: if(_tmp125 <= (
void*)16?1:*((int*)_tmp125)!= 2)goto _LL76;_tmp128=((struct Cyc_Absyn_Section_att_struct*)
_tmp125)->f1;_LL75: return(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp134;_tmp134.tag=0;_tmp134.f1=(struct _tagged_arr)_tmp128;{void*_tmp133[1]={&
_tmp134};Cyc_Std_aprintf(_tag_arr("section(\"%s\")",sizeof(char),14),_tag_arr(
_tmp133,sizeof(void*),1));}});_LL76: if((int)_tmp125 != 6)goto _LL78;_LL77: return
_tag_arr("nocommon",sizeof(char),9);_LL78: if((int)_tmp125 != 7)goto _LL7A;_LL79:
return _tag_arr("shared",sizeof(char),7);_LL7A: if((int)_tmp125 != 8)goto _LL7C;
_LL7B: return _tag_arr("unused",sizeof(char),7);_LL7C: if((int)_tmp125 != 9)goto
_LL7E;_LL7D: return _tag_arr("weak",sizeof(char),5);_LL7E: if((int)_tmp125 != 10)
goto _LL80;_LL7F: return _tag_arr("dllimport",sizeof(char),10);_LL80: if((int)
_tmp125 != 11)goto _LL82;_LL81: return _tag_arr("dllexport",sizeof(char),10);_LL82:
if((int)_tmp125 != 12)goto _LL84;_LL83: return _tag_arr("no_instrument_function",
sizeof(char),23);_LL84: if((int)_tmp125 != 13)goto _LL86;_LL85: return _tag_arr("constructor",
sizeof(char),12);_LL86: if((int)_tmp125 != 14)goto _LL88;_LL87: return _tag_arr("destructor",
sizeof(char),11);_LL88: if((int)_tmp125 != 15)goto _LL8A;_LL89: return _tag_arr("no_check_memory_usage",
sizeof(char),22);_LL8A: if(_tmp125 <= (void*)16?1:*((int*)_tmp125)!= 3)goto _LL8C;
_tmp129=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp125)->f1;if((int)_tmp129
!= 0)goto _LL8C;_tmp12A=((struct Cyc_Absyn_Format_att_struct*)_tmp125)->f2;_tmp12B=((
struct Cyc_Absyn_Format_att_struct*)_tmp125)->f3;_LL8B: return(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp137;_tmp137.tag=1;_tmp137.f1=(unsigned int)
_tmp12B;{struct Cyc_Std_Int_pa_struct _tmp136;_tmp136.tag=1;_tmp136.f1=(
unsigned int)_tmp12A;{void*_tmp135[2]={& _tmp136,& _tmp137};Cyc_Std_aprintf(
_tag_arr("format(printf,%u,%u)",sizeof(char),21),_tag_arr(_tmp135,sizeof(void*),
2));}}});_LL8C: if(_tmp125 <= (void*)16?1:*((int*)_tmp125)!= 3)goto _LL63;_tmp12C=(
void*)((struct Cyc_Absyn_Format_att_struct*)_tmp125)->f1;if((int)_tmp12C != 1)goto
_LL63;_tmp12D=((struct Cyc_Absyn_Format_att_struct*)_tmp125)->f2;_tmp12E=((struct
Cyc_Absyn_Format_att_struct*)_tmp125)->f3;_LL8D: return(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp13A;_tmp13A.tag=1;_tmp13A.f1=(unsigned int)
_tmp12E;{struct Cyc_Std_Int_pa_struct _tmp139;_tmp139.tag=1;_tmp139.f1=(
unsigned int)_tmp12D;{void*_tmp138[2]={& _tmp139,& _tmp13A};Cyc_Std_aprintf(
_tag_arr("format(scanf,%u,%u)",sizeof(char),20),_tag_arr(_tmp138,sizeof(void*),2));}}});
_LL63:;}int Cyc_Absyn_fntype_att(void*a){void*_tmp13B=a;_LL8F: if(_tmp13B <= (void*)
16?1:*((int*)_tmp13B)!= 0)goto _LL91;_LL90: goto _LL92;_LL91: if((int)_tmp13B != 2)
goto _LL93;_LL92: goto _LL94;_LL93: if((int)_tmp13B != 0)goto _LL95;_LL94: goto _LL96;
_LL95: if((int)_tmp13B != 1)goto _LL97;_LL96: goto _LL98;_LL97: if((int)_tmp13B != 3)
goto _LL99;_LL98: goto _LL9A;_LL99: if(_tmp13B <= (void*)16?1:*((int*)_tmp13B)!= 3)
goto _LL9B;_LL9A: goto _LL9C;_LL9B: if((int)_tmp13B != 4)goto _LL9D;_LL9C: return 1;
_LL9D:;_LL9E: return 0;_LL8E:;}static char _tmp13C[3]="f0";static struct _tagged_arr
Cyc_Absyn_f0={_tmp13C,_tmp13C,_tmp13C + 3};static struct _tagged_arr*Cyc_Absyn_field_names_v[
1]={& Cyc_Absyn_f0};static struct _tagged_arr Cyc_Absyn_field_names={(void*)((struct
_tagged_arr**)Cyc_Absyn_field_names_v),(void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v),(
void*)((struct _tagged_arr**)Cyc_Absyn_field_names_v + 1)};struct _tagged_arr*Cyc_Absyn_fieldname(
int i){unsigned int fsz=_get_arr_size(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*));if(i >= fsz)Cyc_Absyn_field_names=({unsigned int _tmp13D=(
unsigned int)(i + 1);struct _tagged_arr**_tmp13E=(struct _tagged_arr**)_cycalloc(
_check_times(sizeof(struct _tagged_arr*),_tmp13D));struct _tagged_arr _tmp143=
_tag_arr(_tmp13E,sizeof(struct _tagged_arr*),(unsigned int)(i + 1));{unsigned int
_tmp13F=_tmp13D;unsigned int j;for(j=0;j < _tmp13F;j ++){_tmp13E[j]=j < fsz?*((
struct _tagged_arr**)_check_unknown_subscript(Cyc_Absyn_field_names,sizeof(struct
_tagged_arr*),(int)j)):({struct _tagged_arr*_tmp140=_cycalloc(sizeof(*_tmp140));
_tmp140[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp142;_tmp142.tag=1;
_tmp142.f1=(int)j;{void*_tmp141[1]={& _tmp142};Cyc_Std_aprintf(_tag_arr("f%d",
sizeof(char),4),_tag_arr(_tmp141,sizeof(void*),1));}});_tmp140;});}}_tmp143;});
return*((struct _tagged_arr**)_check_unknown_subscript(Cyc_Absyn_field_names,
sizeof(struct _tagged_arr*),i));}struct _tuple4 Cyc_Absyn_aggr_kinded_name(void*
info){void*_tmp144=info;void*_tmp145;struct _tuple0*_tmp146;struct Cyc_Absyn_Aggrdecl**
_tmp147;struct Cyc_Absyn_Aggrdecl*_tmp148;struct Cyc_Absyn_Aggrdecl _tmp149;void*
_tmp14A;struct _tuple0*_tmp14B;_LLA0: if(*((int*)_tmp144)!= 0)goto _LLA2;_tmp145=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp144)->f1;_tmp146=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp144)->f2;_LLA1: return({struct _tuple4 _tmp14C;_tmp14C.f1=_tmp145;_tmp14C.f2=
_tmp146;_tmp14C;});_LLA2: if(*((int*)_tmp144)!= 1)goto _LL9F;_tmp147=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp144)->f1;_tmp148=*_tmp147;_tmp149=*_tmp148;_tmp14A=(void*)_tmp149.kind;
_tmp14B=_tmp149.name;_LLA3: return({struct _tuple4 _tmp14D;_tmp14D.f1=_tmp14A;
_tmp14D.f2=_tmp14B;_tmp14D;});_LL9F:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info){void*_tmp14E=info;void*_tmp14F;struct _tuple0*_tmp150;struct Cyc_Absyn_Aggrdecl**
_tmp151;struct Cyc_Absyn_Aggrdecl*_tmp152;_LLA5: if(*((int*)_tmp14E)!= 0)goto _LLA7;
_tmp14F=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp14E)->f1;_tmp150=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp14E)->f2;_LLA6:({void*_tmp153[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unchecked aggrdecl",
sizeof(char),19),_tag_arr(_tmp153,sizeof(void*),0));});_LLA7: if(*((int*)_tmp14E)
!= 1)goto _LLA4;_tmp151=((struct Cyc_Absyn_KnownAggr_struct*)_tmp14E)->f1;_tmp152=*
_tmp151;_LLA8: return _tmp152;_LLA4:;}int Cyc_Absyn_is_union_type(void*t){void*
_tmp154=Cyc_Tcutil_compress(t);void*_tmp155;struct Cyc_Absyn_AggrInfo _tmp156;void*
_tmp157;_LLAA: if(_tmp154 <= (void*)3?1:*((int*)_tmp154)!= 11)goto _LLAC;_tmp155=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp154)->f1;if((int)_tmp155 != 1)
goto _LLAC;_LLAB: return 1;_LLAC: if(_tmp154 <= (void*)3?1:*((int*)_tmp154)!= 10)goto
_LLAE;_tmp156=((struct Cyc_Absyn_AggrType_struct*)_tmp154)->f1;_tmp157=(void*)
_tmp156.aggr_info;_LLAD: return(Cyc_Absyn_aggr_kinded_name(_tmp157)).f1 == (void*)
1;_LLAE:;_LLAF: return 0;_LLA9:;}void Cyc_Absyn_print_decls(struct Cyc_List_List*
decls){((void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(Cyc_decls_rep,&
decls);({void*_tmp158[0]={};Cyc_Std_printf(_tag_arr("\n",sizeof(char),2),
_tag_arr(_tmp158,sizeof(void*),0));});}extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_400;static struct Cyc_Typerep_Int_struct
Cyc__genrep_5={0,0,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep;extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;static char _tmp15A[7]="Static";
static struct _tuple7 Cyc__gentuple_134={0,{_tmp15A,_tmp15A,_tmp15A + 7}};static char
_tmp15B[9]="Abstract";static struct _tuple7 Cyc__gentuple_135={1,{_tmp15B,_tmp15B,
_tmp15B + 9}};static char _tmp15C[7]="Public";static struct _tuple7 Cyc__gentuple_136={
2,{_tmp15C,_tmp15C,_tmp15C + 7}};static char _tmp15D[7]="Extern";static struct
_tuple7 Cyc__gentuple_137={3,{_tmp15D,_tmp15D,_tmp15D + 7}};static char _tmp15E[8]="ExternC";
static struct _tuple7 Cyc__gentuple_138={4,{_tmp15E,_tmp15E,_tmp15E + 8}};static char
_tmp15F[9]="Register";static struct _tuple7 Cyc__gentuple_139={5,{_tmp15F,_tmp15F,
_tmp15F + 9}};static struct _tuple7*Cyc__genarr_140[6]={& Cyc__gentuple_134,& Cyc__gentuple_135,&
Cyc__gentuple_136,& Cyc__gentuple_137,& Cyc__gentuple_138,& Cyc__gentuple_139};
static struct _tuple5*Cyc__genarr_141[0]={};static char _tmp161[6]="Scope";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_scope_t_rep={5,{_tmp161,_tmp161,_tmp161 + 6},{(void*)((struct _tuple7**)
Cyc__genarr_140),(void*)((struct _tuple7**)Cyc__genarr_140),(void*)((struct
_tuple7**)Cyc__genarr_140 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_141),(void*)((
struct _tuple5**)Cyc__genarr_141),(void*)((struct _tuple5**)Cyc__genarr_141 + 0)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_11;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_17;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_18;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12;extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_13;static struct Cyc_Typerep_Int_struct Cyc__genrep_14={0,0,8};static
struct Cyc_Typerep_FatPtr_struct Cyc__genrep_13={2,(void*)((void*)& Cyc__genrep_14)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_12={1,1,(void*)((void*)& Cyc__genrep_13)};
static char _tmp165[5]="List";static struct _tagged_arr Cyc__genname_22={_tmp165,
_tmp165,_tmp165 + 5};static char _tmp166[3]="hd";static struct _tuple5 Cyc__gentuple_19={
offsetof(struct Cyc_List_List,hd),{_tmp166,_tmp166,_tmp166 + 3},(void*)& Cyc__genrep_12};
static char _tmp167[3]="tl";static struct _tuple5 Cyc__gentuple_20={offsetof(struct
Cyc_List_List,tl),{_tmp167,_tmp167,_tmp167 + 3},(void*)& Cyc__genrep_18};static
struct _tuple5*Cyc__genarr_21[2]={& Cyc__gentuple_19,& Cyc__gentuple_20};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_var_t46H2_rep={3,(struct _tagged_arr*)& Cyc__genname_22,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_21),(void*)((
struct _tuple5**)Cyc__genarr_21),(void*)((struct _tuple5**)Cyc__genarr_21 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_18={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};
struct _tuple13{unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple6 Cyc__gentuple_23={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_24={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_18};static struct _tuple6*Cyc__genarr_25[
2]={& Cyc__gentuple_23,& Cyc__gentuple_24};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_17={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_25),(void*)((
struct _tuple6**)Cyc__genarr_25),(void*)((struct _tuple6**)Cyc__genarr_25 + 2)}};
static char _tmp16B[6]="Loc_n";static struct _tuple7 Cyc__gentuple_15={0,{_tmp16B,
_tmp16B,_tmp16B + 6}};static struct _tuple7*Cyc__genarr_16[1]={& Cyc__gentuple_15};
static char _tmp16C[6]="Rel_n";static struct _tuple5 Cyc__gentuple_26={0,{_tmp16C,
_tmp16C,_tmp16C + 6},(void*)& Cyc__genrep_17};static char _tmp16D[6]="Abs_n";static
struct _tuple5 Cyc__gentuple_27={1,{_tmp16D,_tmp16D,_tmp16D + 6},(void*)& Cyc__genrep_17};
static struct _tuple5*Cyc__genarr_28[2]={& Cyc__gentuple_26,& Cyc__gentuple_27};
static char _tmp16F[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={
5,{_tmp16F,_tmp16F,_tmp16F + 8},{(void*)((struct _tuple7**)Cyc__genarr_16),(void*)((
struct _tuple7**)Cyc__genarr_16),(void*)((struct _tuple7**)Cyc__genarr_16 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_28),(void*)((struct _tuple5**)Cyc__genarr_28),(
void*)((struct _tuple5**)Cyc__genarr_28 + 2)}};static struct _tuple6 Cyc__gentuple_29={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_30={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_12};static
struct _tuple6*Cyc__genarr_31[2]={& Cyc__gentuple_29,& Cyc__gentuple_30};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_11={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_31),(void*)((struct _tuple6**)Cyc__genarr_31),(void*)((
struct _tuple6**)Cyc__genarr_31 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_10={
1,1,(void*)((void*)& Cyc__genrep_11)};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_131;
struct _tuple14{char f1;};static struct _tuple6 Cyc__gentuple_132={offsetof(struct
_tuple14,f1),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*Cyc__genarr_133[
1]={& Cyc__gentuple_132};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_131={4,
sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_133),(void*)((
struct _tuple6**)Cyc__genarr_133),(void*)((struct _tuple6**)Cyc__genarr_133 + 1)}};
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1068;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1073;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static char _tmp173[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_185={0,{_tmp173,_tmp173,_tmp173 + 8}};static char
_tmp174[8]="MemKind";static struct _tuple7 Cyc__gentuple_186={1,{_tmp174,_tmp174,
_tmp174 + 8}};static char _tmp175[8]="BoxKind";static struct _tuple7 Cyc__gentuple_187={
2,{_tmp175,_tmp175,_tmp175 + 8}};static char _tmp176[8]="RgnKind";static struct
_tuple7 Cyc__gentuple_188={3,{_tmp176,_tmp176,_tmp176 + 8}};static char _tmp177[8]="EffKind";
static struct _tuple7 Cyc__gentuple_189={4,{_tmp177,_tmp177,_tmp177 + 8}};static char
_tmp178[8]="IntKind";static struct _tuple7 Cyc__gentuple_190={5,{_tmp178,_tmp178,
_tmp178 + 8}};static struct _tuple7*Cyc__genarr_191[6]={& Cyc__gentuple_185,& Cyc__gentuple_186,&
Cyc__gentuple_187,& Cyc__gentuple_188,& Cyc__gentuple_189,& Cyc__gentuple_190};
static struct _tuple5*Cyc__genarr_192[0]={};static char _tmp17A[5]="Kind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kind_t_rep={5,{_tmp17A,_tmp17A,_tmp17A + 5},{(void*)((struct _tuple7**)
Cyc__genarr_191),(void*)((struct _tuple7**)Cyc__genarr_191),(void*)((struct
_tuple7**)Cyc__genarr_191 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_192),(void*)((
struct _tuple5**)Cyc__genarr_192),(void*)((struct _tuple5**)Cyc__genarr_192 + 0)}};
static char _tmp17B[4]="Opt";static struct _tagged_arr Cyc__genname_1076={_tmp17B,
_tmp17B,_tmp17B + 4};static char _tmp17C[2]="v";static struct _tuple5 Cyc__gentuple_1074={
offsetof(struct Cyc_Core_Opt,v),{_tmp17C,_tmp17C,_tmp17C + 2},(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple5*Cyc__genarr_1075[1]={& Cyc__gentuple_1074};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kind_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_1076,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1075),(void*)((
struct _tuple5**)Cyc__genarr_1075),(void*)((struct _tuple5**)Cyc__genarr_1075 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1073={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_43;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep;static char _tmp17F[4]="Opt";static struct
_tagged_arr Cyc__genname_1108={_tmp17F,_tmp17F,_tmp17F + 4};static char _tmp180[2]="v";
static struct _tuple5 Cyc__gentuple_1106={offsetof(struct Cyc_Core_Opt,v),{_tmp180,
_tmp180,_tmp180 + 2},(void*)& Cyc_Absyn_type_t_rep};static struct _tuple5*Cyc__genarr_1107[
1]={& Cyc__gentuple_1106};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep={
3,(struct _tagged_arr*)& Cyc__genname_1108,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_1107),(void*)((struct _tuple5**)Cyc__genarr_1107),(
void*)((struct _tuple5**)Cyc__genarr_1107 + 1)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_43={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};
static struct Cyc_Typerep_Int_struct Cyc__genrep_62={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1069;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_294;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_182;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_213;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_213={1,1,(void*)((void*)& Cyc__genrep_62)};extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_205;
static struct _tuple6 Cyc__gentuple_206={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_207={offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};
static struct _tuple6*Cyc__genarr_208[2]={& Cyc__gentuple_206,& Cyc__gentuple_207};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_205={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_208),(void*)((struct _tuple6**)Cyc__genarr_208),(
void*)((struct _tuple6**)Cyc__genarr_208 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_201;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_193;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;static
char _tmp186[4]="Opt";static struct _tagged_arr Cyc__genname_196={_tmp186,_tmp186,
_tmp186 + 4};static char _tmp187[2]="v";static struct _tuple5 Cyc__gentuple_194={
offsetof(struct Cyc_Core_Opt,v),{_tmp187,_tmp187,_tmp187 + 2},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_195[1]={& Cyc__gentuple_194};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_196,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_195),(void*)((
struct _tuple5**)Cyc__genarr_195),(void*)((struct _tuple5**)Cyc__genarr_195 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_193={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple15{unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_202={
offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_203={
offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_193};static struct _tuple6*Cyc__genarr_204[
2]={& Cyc__gentuple_202,& Cyc__gentuple_203};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_201={4,sizeof(struct _tuple15),{(void*)((struct _tuple6**)Cyc__genarr_204),(
void*)((struct _tuple6**)Cyc__genarr_204),(void*)((struct _tuple6**)Cyc__genarr_204
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_184;struct _tuple16{
unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};static struct _tuple6 Cyc__gentuple_197={
offsetof(struct _tuple16,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_198={
offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_193};static struct _tuple6 Cyc__gentuple_199={
offsetof(struct _tuple16,f3),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*
Cyc__genarr_200[3]={& Cyc__gentuple_197,& Cyc__gentuple_198,& Cyc__gentuple_199};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_184={4,sizeof(struct _tuple16),{(
void*)((struct _tuple6**)Cyc__genarr_200),(void*)((struct _tuple6**)Cyc__genarr_200),(
void*)((struct _tuple6**)Cyc__genarr_200 + 3)}};static struct _tuple7*Cyc__genarr_183[
0]={};static char _tmp18C[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_209={0,{
_tmp18C,_tmp18C,_tmp18C + 6},(void*)& Cyc__genrep_205};static char _tmp18D[11]="Unknown_kb";
static struct _tuple5 Cyc__gentuple_210={1,{_tmp18D,_tmp18D,_tmp18D + 11},(void*)&
Cyc__genrep_201};static char _tmp18E[8]="Less_kb";static struct _tuple5 Cyc__gentuple_211={
2,{_tmp18E,_tmp18E,_tmp18E + 8},(void*)& Cyc__genrep_184};static struct _tuple5*Cyc__genarr_212[
3]={& Cyc__gentuple_209,& Cyc__gentuple_210,& Cyc__gentuple_211};static char _tmp190[
10]="KindBound";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={5,{
_tmp190,_tmp190,_tmp190 + 10},{(void*)((struct _tuple7**)Cyc__genarr_183),(void*)((
struct _tuple7**)Cyc__genarr_183),(void*)((struct _tuple7**)Cyc__genarr_183 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_212),(void*)((struct _tuple5**)Cyc__genarr_212),(
void*)((struct _tuple5**)Cyc__genarr_212 + 3)}};static char _tmp191[5]="Tvar";static
struct _tagged_arr Cyc__genname_218={_tmp191,_tmp191,_tmp191 + 5};static char _tmp192[
5]="name";static struct _tuple5 Cyc__gentuple_214={offsetof(struct Cyc_Absyn_Tvar,name),{
_tmp192,_tmp192,_tmp192 + 5},(void*)& Cyc__genrep_12};static char _tmp193[9]="identity";
static struct _tuple5 Cyc__gentuple_215={offsetof(struct Cyc_Absyn_Tvar,identity),{
_tmp193,_tmp193,_tmp193 + 9},(void*)& Cyc__genrep_213};static char _tmp194[5]="kind";
static struct _tuple5 Cyc__gentuple_216={offsetof(struct Cyc_Absyn_Tvar,kind),{
_tmp194,_tmp194,_tmp194 + 5},(void*)& Cyc_Absyn_kindbound_t_rep};static struct
_tuple5*Cyc__genarr_217[3]={& Cyc__gentuple_214,& Cyc__gentuple_215,& Cyc__gentuple_216};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={3,(struct _tagged_arr*)&
Cyc__genname_218,sizeof(struct Cyc_Absyn_Tvar),{(void*)((struct _tuple5**)Cyc__genarr_217),(
void*)((struct _tuple5**)Cyc__genarr_217),(void*)((struct _tuple5**)Cyc__genarr_217
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_182={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tvar_rep)};static char _tmp197[5]="List";static struct _tagged_arr
Cyc__genname_298={_tmp197,_tmp197,_tmp197 + 5};static char _tmp198[3]="hd";static
struct _tuple5 Cyc__gentuple_295={offsetof(struct Cyc_List_List,hd),{_tmp198,
_tmp198,_tmp198 + 3},(void*)& Cyc__genrep_182};static char _tmp199[3]="tl";static
struct _tuple5 Cyc__gentuple_296={offsetof(struct Cyc_List_List,tl),{_tmp199,
_tmp199,_tmp199 + 3},(void*)& Cyc__genrep_294};static struct _tuple5*Cyc__genarr_297[
2]={& Cyc__gentuple_295,& Cyc__gentuple_296};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_298,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_297),(void*)((struct _tuple5**)Cyc__genarr_297),(void*)((
struct _tuple5**)Cyc__genarr_297 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_294={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static char
_tmp19C[4]="Opt";static struct _tagged_arr Cyc__genname_1072={_tmp19C,_tmp19C,
_tmp19C + 4};static char _tmp19D[2]="v";static struct _tuple5 Cyc__gentuple_1070={
offsetof(struct Cyc_Core_Opt,v),{_tmp19D,_tmp19D,_tmp19D + 2},(void*)& Cyc__genrep_294};
static struct _tuple5*Cyc__genarr_1071[1]={& Cyc__gentuple_1070};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_1072,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1071),(
void*)((struct _tuple5**)Cyc__genarr_1071),(void*)((struct _tuple5**)Cyc__genarr_1071
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1069={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};struct _tuple17{
unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;struct Cyc_Core_Opt*
f5;};static struct _tuple6 Cyc__gentuple_1077={offsetof(struct _tuple17,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1078={offsetof(struct _tuple17,f2),(
void*)& Cyc__genrep_1073};static struct _tuple6 Cyc__gentuple_1079={offsetof(struct
_tuple17,f3),(void*)& Cyc__genrep_43};static struct _tuple6 Cyc__gentuple_1080={
offsetof(struct _tuple17,f4),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_1081={offsetof(struct _tuple17,f5),(void*)& Cyc__genrep_1069};static
struct _tuple6*Cyc__genarr_1082[5]={& Cyc__gentuple_1077,& Cyc__gentuple_1078,& Cyc__gentuple_1079,&
Cyc__gentuple_1080,& Cyc__gentuple_1081};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1068={
4,sizeof(struct _tuple17),{(void*)((struct _tuple6**)Cyc__genarr_1082),(void*)((
struct _tuple6**)Cyc__genarr_1082),(void*)((struct _tuple6**)Cyc__genarr_1082 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1064;struct _tuple18{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1065={offsetof(
struct _tuple18,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1066={
offsetof(struct _tuple18,f2),(void*)& Cyc__genrep_182};static struct _tuple6*Cyc__genarr_1067[
2]={& Cyc__gentuple_1065,& Cyc__gentuple_1066};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1064={4,sizeof(struct _tuple18),{(void*)((struct _tuple6**)Cyc__genarr_1067),(
void*)((struct _tuple6**)Cyc__genarr_1067),(void*)((struct _tuple6**)Cyc__genarr_1067
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1038;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1045;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static char _tmp1A2[18]="UnknownTunionInfo";
static struct _tagged_arr Cyc__genname_1049={_tmp1A2,_tmp1A2,_tmp1A2 + 18};static
char _tmp1A3[5]="name";static struct _tuple5 Cyc__gentuple_1046={offsetof(struct Cyc_Absyn_UnknownTunionInfo,name),{
_tmp1A3,_tmp1A3,_tmp1A3 + 5},(void*)& Cyc__genrep_10};static char _tmp1A4[11]="is_xtunion";
static struct _tuple5 Cyc__gentuple_1047={offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),{
_tmp1A4,_tmp1A4,_tmp1A4 + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_1048[2]={& Cyc__gentuple_1046,& Cyc__gentuple_1047};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep={3,(struct _tagged_arr*)& Cyc__genname_1049,
sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(void*)((struct _tuple5**)Cyc__genarr_1048),(
void*)((struct _tuple5**)Cyc__genarr_1048),(void*)((struct _tuple5**)Cyc__genarr_1048
+ 2)}};struct _tuple19{unsigned int f1;struct Cyc_Absyn_UnknownTunionInfo f2;};
static struct _tuple6 Cyc__gentuple_1050={offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1051={offsetof(struct _tuple19,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple6*Cyc__genarr_1052[2]={& Cyc__gentuple_1050,& Cyc__gentuple_1051};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1045={4,sizeof(struct _tuple19),{(
void*)((struct _tuple6**)Cyc__genarr_1052),(void*)((struct _tuple6**)Cyc__genarr_1052),(
void*)((struct _tuple6**)Cyc__genarr_1052 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1040;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1041;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_284;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_285;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_286;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_267;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_268;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_269;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_270;static struct
_tuple6 Cyc__gentuple_271={offsetof(struct _tuple3,f1),(void*)& Cyc__genrep_131};
static struct _tuple6 Cyc__gentuple_272={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_273[2]={& Cyc__gentuple_271,&
Cyc__gentuple_272};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_270={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_273),(void*)((struct
_tuple6**)Cyc__genarr_273),(void*)((struct _tuple6**)Cyc__genarr_273 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_269={1,1,(void*)((void*)& Cyc__genrep_270)};
static char _tmp1A9[5]="List";static struct _tagged_arr Cyc__genname_277={_tmp1A9,
_tmp1A9,_tmp1A9 + 5};static char _tmp1AA[3]="hd";static struct _tuple5 Cyc__gentuple_274={
offsetof(struct Cyc_List_List,hd),{_tmp1AA,_tmp1AA,_tmp1AA + 3},(void*)& Cyc__genrep_269};
static char _tmp1AB[3]="tl";static struct _tuple5 Cyc__gentuple_275={offsetof(struct
Cyc_List_List,tl),{_tmp1AB,_tmp1AB,_tmp1AB + 3},(void*)& Cyc__genrep_268};static
struct _tuple5*Cyc__genarr_276[2]={& Cyc__gentuple_274,& Cyc__gentuple_275};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_277,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_276),(void*)((struct _tuple5**)Cyc__genarr_276),(void*)((
struct _tuple5**)Cyc__genarr_276 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_268={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Position_Segment_rep;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={
1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};static char _tmp1AF[12]="Tunionfield";
static struct _tagged_arr Cyc__genname_283={_tmp1AF,_tmp1AF,_tmp1AF + 12};static char
_tmp1B0[5]="name";static struct _tuple5 Cyc__gentuple_278={offsetof(struct Cyc_Absyn_Tunionfield,name),{
_tmp1B0,_tmp1B0,_tmp1B0 + 5},(void*)& Cyc__genrep_10};static char _tmp1B1[5]="typs";
static struct _tuple5 Cyc__gentuple_279={offsetof(struct Cyc_Absyn_Tunionfield,typs),{
_tmp1B1,_tmp1B1,_tmp1B1 + 5},(void*)& Cyc__genrep_268};static char _tmp1B2[4]="loc";
static struct _tuple5 Cyc__gentuple_280={offsetof(struct Cyc_Absyn_Tunionfield,loc),{
_tmp1B2,_tmp1B2,_tmp1B2 + 4},(void*)& Cyc__genrep_2};static char _tmp1B3[3]="sc";
static struct _tuple5 Cyc__gentuple_281={offsetof(struct Cyc_Absyn_Tunionfield,sc),{
_tmp1B3,_tmp1B3,_tmp1B3 + 3},(void*)& Cyc_Absyn_scope_t_rep};static struct _tuple5*
Cyc__genarr_282[4]={& Cyc__gentuple_278,& Cyc__gentuple_279,& Cyc__gentuple_280,&
Cyc__gentuple_281};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_283,sizeof(struct Cyc_Absyn_Tunionfield),{(
void*)((struct _tuple5**)Cyc__genarr_282),(void*)((struct _tuple5**)Cyc__genarr_282),(
void*)((struct _tuple5**)Cyc__genarr_282 + 4)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_267={1,1,(void*)((void*)& Cyc_struct_Absyn_Tunionfield_rep)};static
char _tmp1B6[5]="List";static struct _tagged_arr Cyc__genname_290={_tmp1B6,_tmp1B6,
_tmp1B6 + 5};static char _tmp1B7[3]="hd";static struct _tuple5 Cyc__gentuple_287={
offsetof(struct Cyc_List_List,hd),{_tmp1B7,_tmp1B7,_tmp1B7 + 3},(void*)& Cyc__genrep_267};
static char _tmp1B8[3]="tl";static struct _tuple5 Cyc__gentuple_288={offsetof(struct
Cyc_List_List,tl),{_tmp1B8,_tmp1B8,_tmp1B8 + 3},(void*)& Cyc__genrep_286};static
struct _tuple5*Cyc__genarr_289[2]={& Cyc__gentuple_287,& Cyc__gentuple_288};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_290,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_289),(void*)((struct _tuple5**)Cyc__genarr_289),(void*)((
struct _tuple5**)Cyc__genarr_289 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_286={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};static
char _tmp1BB[4]="Opt";static struct _tagged_arr Cyc__genname_293={_tmp1BB,_tmp1BB,
_tmp1BB + 4};static char _tmp1BC[2]="v";static struct _tuple5 Cyc__gentuple_291={
offsetof(struct Cyc_Core_Opt,v),{_tmp1BC,_tmp1BC,_tmp1BC + 2},(void*)& Cyc__genrep_286};
static struct _tuple5*Cyc__genarr_292[1]={& Cyc__gentuple_291};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={3,(struct
_tagged_arr*)& Cyc__genname_293,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_292),(void*)((struct _tuple5**)Cyc__genarr_292),(void*)((
struct _tuple5**)Cyc__genarr_292 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_285={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static char _tmp1BF[11]="Tuniondecl";static struct _tagged_arr Cyc__genname_305={
_tmp1BF,_tmp1BF,_tmp1BF + 11};static char _tmp1C0[3]="sc";static struct _tuple5 Cyc__gentuple_299={
offsetof(struct Cyc_Absyn_Tuniondecl,sc),{_tmp1C0,_tmp1C0,_tmp1C0 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp1C1[5]="name";static struct _tuple5 Cyc__gentuple_300={offsetof(
struct Cyc_Absyn_Tuniondecl,name),{_tmp1C1,_tmp1C1,_tmp1C1 + 5},(void*)& Cyc__genrep_10};
static char _tmp1C2[4]="tvs";static struct _tuple5 Cyc__gentuple_301={offsetof(struct
Cyc_Absyn_Tuniondecl,tvs),{_tmp1C2,_tmp1C2,_tmp1C2 + 4},(void*)& Cyc__genrep_294};
static char _tmp1C3[7]="fields";static struct _tuple5 Cyc__gentuple_302={offsetof(
struct Cyc_Absyn_Tuniondecl,fields),{_tmp1C3,_tmp1C3,_tmp1C3 + 7},(void*)& Cyc__genrep_285};
static char _tmp1C4[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_303={
offsetof(struct Cyc_Absyn_Tuniondecl,is_xtunion),{_tmp1C4,_tmp1C4,_tmp1C4 + 11},(
void*)((void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_304[5]={& Cyc__gentuple_299,&
Cyc__gentuple_300,& Cyc__gentuple_301,& Cyc__gentuple_302,& Cyc__gentuple_303};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tuniondecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_305,sizeof(struct Cyc_Absyn_Tuniondecl),{(void*)((
struct _tuple5**)Cyc__genarr_304),(void*)((struct _tuple5**)Cyc__genarr_304),(void*)((
struct _tuple5**)Cyc__genarr_304 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_284={
1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1041={1,1,(void*)((void*)& Cyc__genrep_284)};struct _tuple20{
unsigned int f1;struct Cyc_Absyn_Tuniondecl**f2;};static struct _tuple6 Cyc__gentuple_1042={
offsetof(struct _tuple20,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1043={
offsetof(struct _tuple20,f2),(void*)& Cyc__genrep_1041};static struct _tuple6*Cyc__genarr_1044[
2]={& Cyc__gentuple_1042,& Cyc__gentuple_1043};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1040={4,sizeof(struct _tuple20),{(void*)((struct _tuple6**)Cyc__genarr_1044),(
void*)((struct _tuple6**)Cyc__genarr_1044),(void*)((struct _tuple6**)Cyc__genarr_1044
+ 2)}};static struct _tuple7*Cyc__genarr_1039[0]={};static char _tmp1C9[14]="UnknownTunion";
static struct _tuple5 Cyc__gentuple_1053={0,{_tmp1C9,_tmp1C9,_tmp1C9 + 14},(void*)&
Cyc__genrep_1045};static char _tmp1CA[12]="KnownTunion";static struct _tuple5 Cyc__gentuple_1054={
1,{_tmp1CA,_tmp1CA,_tmp1CA + 12},(void*)& Cyc__genrep_1040};static struct _tuple5*
Cyc__genarr_1055[2]={& Cyc__gentuple_1053,& Cyc__gentuple_1054};static char _tmp1CC[
12]="TunionInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep={
5,{_tmp1CC,_tmp1CC,_tmp1CC + 12},{(void*)((struct _tuple7**)Cyc__genarr_1039),(
void*)((struct _tuple7**)Cyc__genarr_1039),(void*)((struct _tuple7**)Cyc__genarr_1039
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1055),(void*)((struct _tuple5**)Cyc__genarr_1055),(
void*)((struct _tuple5**)Cyc__genarr_1055 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_53;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep;
static char _tmp1CD[5]="List";static struct _tagged_arr Cyc__genname_57={_tmp1CD,
_tmp1CD,_tmp1CD + 5};static char _tmp1CE[3]="hd";static struct _tuple5 Cyc__gentuple_54={
offsetof(struct Cyc_List_List,hd),{_tmp1CE,_tmp1CE,_tmp1CE + 3},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp1CF[3]="tl";static struct _tuple5 Cyc__gentuple_55={
offsetof(struct Cyc_List_List,tl),{_tmp1CF,_tmp1CF,_tmp1CF + 3},(void*)& Cyc__genrep_53};
static struct _tuple5*Cyc__genarr_56[2]={& Cyc__gentuple_54,& Cyc__gentuple_55};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={3,(
struct _tagged_arr*)& Cyc__genname_57,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_56),(void*)((struct _tuple5**)Cyc__genarr_56),(void*)((
struct _tuple5**)Cyc__genarr_56 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_53={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};static char
_tmp1D2[11]="TunionInfo";static struct _tagged_arr Cyc__genname_1060={_tmp1D2,
_tmp1D2,_tmp1D2 + 11};static char _tmp1D3[12]="tunion_info";static struct _tuple5 Cyc__gentuple_1056={
offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),{_tmp1D3,_tmp1D3,_tmp1D3 + 12},(
void*)& Cyc_tunion_Absyn_TunionInfoU_rep};static char _tmp1D4[6]="targs";static
struct _tuple5 Cyc__gentuple_1057={offsetof(struct Cyc_Absyn_TunionInfo,targs),{
_tmp1D4,_tmp1D4,_tmp1D4 + 6},(void*)& Cyc__genrep_53};static char _tmp1D5[4]="rgn";
static struct _tuple5 Cyc__gentuple_1058={offsetof(struct Cyc_Absyn_TunionInfo,rgn),{
_tmp1D5,_tmp1D5,_tmp1D5 + 4},(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple5*Cyc__genarr_1059[3]={& Cyc__gentuple_1056,& Cyc__gentuple_1057,& Cyc__gentuple_1058};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_1060,sizeof(struct Cyc_Absyn_TunionInfo),{(void*)((struct _tuple5**)
Cyc__genarr_1059),(void*)((struct _tuple5**)Cyc__genarr_1059),(void*)((struct
_tuple5**)Cyc__genarr_1059 + 3)}};struct _tuple21{unsigned int f1;struct Cyc_Absyn_TunionInfo
f2;};static struct _tuple6 Cyc__gentuple_1061={offsetof(struct _tuple21,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1062={offsetof(struct _tuple21,f2),(
void*)& Cyc_Absyn_tunion_info_t_rep};static struct _tuple6*Cyc__genarr_1063[2]={&
Cyc__gentuple_1061,& Cyc__gentuple_1062};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1038={
4,sizeof(struct _tuple21),{(void*)((struct _tuple6**)Cyc__genarr_1063),(void*)((
struct _tuple6**)Cyc__genarr_1063),(void*)((struct _tuple6**)Cyc__genarr_1063 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1012;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_field_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1019;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static char _tmp1D8[23]="UnknownTunionFieldInfo";
static struct _tagged_arr Cyc__genname_1024={_tmp1D8,_tmp1D8,_tmp1D8 + 23};static
char _tmp1D9[12]="tunion_name";static struct _tuple5 Cyc__gentuple_1020={offsetof(
struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),{_tmp1D9,_tmp1D9,_tmp1D9 + 12},(
void*)& Cyc__genrep_10};static char _tmp1DA[11]="field_name";static struct _tuple5 Cyc__gentuple_1021={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),{_tmp1DA,_tmp1DA,
_tmp1DA + 11},(void*)& Cyc__genrep_10};static char _tmp1DB[11]="is_xtunion";static
struct _tuple5 Cyc__gentuple_1022={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),{
_tmp1DB,_tmp1DB,_tmp1DB + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_1023[3]={& Cyc__gentuple_1020,& Cyc__gentuple_1021,& Cyc__gentuple_1022};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={3,(
struct _tagged_arr*)& Cyc__genname_1024,sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1023),(void*)((struct _tuple5**)Cyc__genarr_1023),(
void*)((struct _tuple5**)Cyc__genarr_1023 + 3)}};struct _tuple22{unsigned int f1;
struct Cyc_Absyn_UnknownTunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1025={
offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1026={
offsetof(struct _tuple22,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1027[2]={& Cyc__gentuple_1025,& Cyc__gentuple_1026};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1019={4,sizeof(struct _tuple22),{(
void*)((struct _tuple6**)Cyc__genarr_1027),(void*)((struct _tuple6**)Cyc__genarr_1027),(
void*)((struct _tuple6**)Cyc__genarr_1027 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1014;struct _tuple23{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;};static struct _tuple6 Cyc__gentuple_1015={
offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1016={
offsetof(struct _tuple23,f2),(void*)((void*)& Cyc__genrep_284)};static struct
_tuple6 Cyc__gentuple_1017={offsetof(struct _tuple23,f3),(void*)& Cyc__genrep_267};
static struct _tuple6*Cyc__genarr_1018[3]={& Cyc__gentuple_1015,& Cyc__gentuple_1016,&
Cyc__gentuple_1017};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1014={4,
sizeof(struct _tuple23),{(void*)((struct _tuple6**)Cyc__genarr_1018),(void*)((
struct _tuple6**)Cyc__genarr_1018),(void*)((struct _tuple6**)Cyc__genarr_1018 + 3)}};
static struct _tuple7*Cyc__genarr_1013[0]={};static char _tmp1DF[19]="UnknownTunionfield";
static struct _tuple5 Cyc__gentuple_1028={0,{_tmp1DF,_tmp1DF,_tmp1DF + 19},(void*)&
Cyc__genrep_1019};static char _tmp1E0[17]="KnownTunionfield";static struct _tuple5
Cyc__gentuple_1029={1,{_tmp1E0,_tmp1E0,_tmp1E0 + 17},(void*)& Cyc__genrep_1014};
static struct _tuple5*Cyc__genarr_1030[2]={& Cyc__gentuple_1028,& Cyc__gentuple_1029};
static char _tmp1E2[17]="TunionFieldInfoU";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep={
5,{_tmp1E2,_tmp1E2,_tmp1E2 + 17},{(void*)((struct _tuple7**)Cyc__genarr_1013),(
void*)((struct _tuple7**)Cyc__genarr_1013),(void*)((struct _tuple7**)Cyc__genarr_1013
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1030),(void*)((struct _tuple5**)Cyc__genarr_1030),(
void*)((struct _tuple5**)Cyc__genarr_1030 + 2)}};static char _tmp1E3[16]="TunionFieldInfo";
static struct _tagged_arr Cyc__genname_1034={_tmp1E3,_tmp1E3,_tmp1E3 + 16};static
char _tmp1E4[11]="field_info";static struct _tuple5 Cyc__gentuple_1031={offsetof(
struct Cyc_Absyn_TunionFieldInfo,field_info),{_tmp1E4,_tmp1E4,_tmp1E4 + 11},(void*)&
Cyc_tunion_Absyn_TunionFieldInfoU_rep};static char _tmp1E5[6]="targs";static struct
_tuple5 Cyc__gentuple_1032={offsetof(struct Cyc_Absyn_TunionFieldInfo,targs),{
_tmp1E5,_tmp1E5,_tmp1E5 + 6},(void*)& Cyc__genrep_53};static struct _tuple5*Cyc__genarr_1033[
2]={& Cyc__gentuple_1031,& Cyc__gentuple_1032};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_1034,sizeof(struct Cyc_Absyn_TunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1033),(void*)((struct _tuple5**)Cyc__genarr_1033),(
void*)((struct _tuple5**)Cyc__genarr_1033 + 2)}};struct _tuple24{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1035={
offsetof(struct _tuple24,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1036={
offsetof(struct _tuple24,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1037[2]={& Cyc__gentuple_1035,& Cyc__gentuple_1036};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1012={4,sizeof(struct _tuple24),{(
void*)((struct _tuple6**)Cyc__genarr_1037),(void*)((struct _tuple6**)Cyc__genarr_1037),(
void*)((struct _tuple6**)Cyc__genarr_1037 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_976;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_998;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_984;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_bounds_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_80;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_81;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_835;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_850;extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep;static char _tmp1E8[7]="Signed";
static struct _tuple7 Cyc__gentuple_417={0,{_tmp1E8,_tmp1E8,_tmp1E8 + 7}};static char
_tmp1E9[9]="Unsigned";static struct _tuple7 Cyc__gentuple_418={1,{_tmp1E9,_tmp1E9,
_tmp1E9 + 9}};static char _tmp1EA[5]="None";static struct _tuple7 Cyc__gentuple_419={2,{
_tmp1EA,_tmp1EA,_tmp1EA + 5}};static struct _tuple7*Cyc__genarr_420[3]={& Cyc__gentuple_417,&
Cyc__gentuple_418,& Cyc__gentuple_419};static struct _tuple5*Cyc__genarr_421[0]={};
static char _tmp1EC[5]="Sign";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep={
5,{_tmp1EC,_tmp1EC,_tmp1EC + 5},{(void*)((struct _tuple7**)Cyc__genarr_420),(void*)((
struct _tuple7**)Cyc__genarr_420),(void*)((struct _tuple7**)Cyc__genarr_420 + 3)},{(
void*)((struct _tuple5**)Cyc__genarr_421),(void*)((struct _tuple5**)Cyc__genarr_421),(
void*)((struct _tuple5**)Cyc__genarr_421 + 0)}};struct _tuple25{unsigned int f1;void*
f2;char f3;};static struct _tuple6 Cyc__gentuple_851={offsetof(struct _tuple25,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_852={offsetof(struct
_tuple25,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_853={
offsetof(struct _tuple25,f3),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_854[3]={& Cyc__gentuple_851,& Cyc__gentuple_852,& Cyc__gentuple_853};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_850={4,sizeof(struct _tuple25),{(
void*)((struct _tuple6**)Cyc__genarr_854),(void*)((struct _tuple6**)Cyc__genarr_854),(
void*)((struct _tuple6**)Cyc__genarr_854 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_844;static struct Cyc_Typerep_Int_struct Cyc__genrep_845={0,1,16};
struct _tuple26{unsigned int f1;void*f2;short f3;};static struct _tuple6 Cyc__gentuple_846={
offsetof(struct _tuple26,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_847={
offsetof(struct _tuple26,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_848={
offsetof(struct _tuple26,f3),(void*)& Cyc__genrep_845};static struct _tuple6*Cyc__genarr_849[
3]={& Cyc__gentuple_846,& Cyc__gentuple_847,& Cyc__gentuple_848};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_844={4,sizeof(struct _tuple26),{(void*)((struct _tuple6**)Cyc__genarr_849),(
void*)((struct _tuple6**)Cyc__genarr_849),(void*)((struct _tuple6**)Cyc__genarr_849
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_416;struct _tuple27{
unsigned int f1;void*f2;int f3;};static struct _tuple6 Cyc__gentuple_422={offsetof(
struct _tuple27,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_423={
offsetof(struct _tuple27,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_424={
offsetof(struct _tuple27,f3),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6*
Cyc__genarr_425[3]={& Cyc__gentuple_422,& Cyc__gentuple_423,& Cyc__gentuple_424};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_416={4,sizeof(struct _tuple27),{(
void*)((struct _tuple6**)Cyc__genarr_425),(void*)((struct _tuple6**)Cyc__genarr_425),(
void*)((struct _tuple6**)Cyc__genarr_425 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_838;static struct Cyc_Typerep_Int_struct Cyc__genrep_839={0,1,64};
struct _tuple28{unsigned int f1;void*f2;long long f3;};static struct _tuple6 Cyc__gentuple_840={
offsetof(struct _tuple28,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_841={
offsetof(struct _tuple28,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_842={
offsetof(struct _tuple28,f3),(void*)& Cyc__genrep_839};static struct _tuple6*Cyc__genarr_843[
3]={& Cyc__gentuple_840,& Cyc__gentuple_841,& Cyc__gentuple_842};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_838={4,sizeof(struct _tuple28),{(void*)((struct _tuple6**)Cyc__genarr_843),(
void*)((struct _tuple6**)Cyc__genarr_843),(void*)((struct _tuple6**)Cyc__genarr_843
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_115;static struct _tuple6
Cyc__gentuple_116={offsetof(struct _tuple7,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_117={offsetof(struct _tuple7,f2),(void*)((void*)& Cyc__genrep_13)};
static struct _tuple6*Cyc__genarr_118[2]={& Cyc__gentuple_116,& Cyc__gentuple_117};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_115={4,sizeof(struct _tuple7),{(
void*)((struct _tuple6**)Cyc__genarr_118),(void*)((struct _tuple6**)Cyc__genarr_118),(
void*)((struct _tuple6**)Cyc__genarr_118 + 2)}};static char _tmp1F4[7]="Null_c";
static struct _tuple7 Cyc__gentuple_836={0,{_tmp1F4,_tmp1F4,_tmp1F4 + 7}};static
struct _tuple7*Cyc__genarr_837[1]={& Cyc__gentuple_836};static char _tmp1F5[7]="Char_c";
static struct _tuple5 Cyc__gentuple_855={0,{_tmp1F5,_tmp1F5,_tmp1F5 + 7},(void*)& Cyc__genrep_850};
static char _tmp1F6[8]="Short_c";static struct _tuple5 Cyc__gentuple_856={1,{_tmp1F6,
_tmp1F6,_tmp1F6 + 8},(void*)& Cyc__genrep_844};static char _tmp1F7[6]="Int_c";static
struct _tuple5 Cyc__gentuple_857={2,{_tmp1F7,_tmp1F7,_tmp1F7 + 6},(void*)& Cyc__genrep_416};
static char _tmp1F8[11]="LongLong_c";static struct _tuple5 Cyc__gentuple_858={3,{
_tmp1F8,_tmp1F8,_tmp1F8 + 11},(void*)& Cyc__genrep_838};static char _tmp1F9[8]="Float_c";
static struct _tuple5 Cyc__gentuple_859={4,{_tmp1F9,_tmp1F9,_tmp1F9 + 8},(void*)& Cyc__genrep_115};
static char _tmp1FA[9]="String_c";static struct _tuple5 Cyc__gentuple_860={5,{_tmp1FA,
_tmp1FA,_tmp1FA + 9},(void*)& Cyc__genrep_115};static struct _tuple5*Cyc__genarr_861[
6]={& Cyc__gentuple_855,& Cyc__gentuple_856,& Cyc__gentuple_857,& Cyc__gentuple_858,&
Cyc__gentuple_859,& Cyc__gentuple_860};static char _tmp1FC[5]="Cnst";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep={5,{_tmp1FC,_tmp1FC,_tmp1FC + 5},{(void*)((struct _tuple7**)
Cyc__genarr_837),(void*)((struct _tuple7**)Cyc__genarr_837),(void*)((struct
_tuple7**)Cyc__genarr_837 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_861),(void*)((
struct _tuple5**)Cyc__genarr_861),(void*)((struct _tuple5**)Cyc__genarr_861 + 6)}};
static struct _tuple6 Cyc__gentuple_862={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_863={offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_cnst_t_rep};
static struct _tuple6*Cyc__genarr_864[2]={& Cyc__gentuple_862,& Cyc__gentuple_863};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_835={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_864),(void*)((struct _tuple6**)Cyc__genarr_864),(
void*)((struct _tuple6**)Cyc__genarr_864 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_822;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_85;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_86;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_588;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_589;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_590;struct _tuple29{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*
f3;};static struct _tuple6 Cyc__gentuple_591={offsetof(struct _tuple29,f1),(void*)&
Cyc__genrep_12};static struct _tuple6 Cyc__gentuple_592={offsetof(struct _tuple29,f2),(
void*)& Cyc__genrep_131};static struct _tuple6 Cyc__gentuple_593={offsetof(struct
_tuple29,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_594[
3]={& Cyc__gentuple_591,& Cyc__gentuple_592,& Cyc__gentuple_593};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_590={4,sizeof(struct _tuple29),{(void*)((struct _tuple6**)Cyc__genarr_594),(
void*)((struct _tuple6**)Cyc__genarr_594),(void*)((struct _tuple6**)Cyc__genarr_594
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_589={1,1,(void*)((void*)&
Cyc__genrep_590)};static char _tmp200[5]="List";static struct _tagged_arr Cyc__genname_598={
_tmp200,_tmp200,_tmp200 + 5};static char _tmp201[3]="hd";static struct _tuple5 Cyc__gentuple_595={
offsetof(struct Cyc_List_List,hd),{_tmp201,_tmp201,_tmp201 + 3},(void*)& Cyc__genrep_589};
static char _tmp202[3]="tl";static struct _tuple5 Cyc__gentuple_596={offsetof(struct
Cyc_List_List,tl),{_tmp202,_tmp202,_tmp202 + 3},(void*)& Cyc__genrep_588};static
struct _tuple5*Cyc__genarr_597[2]={& Cyc__gentuple_595,& Cyc__gentuple_596};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_598,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_597),(void*)((struct _tuple5**)Cyc__genarr_597),(void*)((
struct _tuple5**)Cyc__genarr_597 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_588={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_577;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_578;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
char _tmp205[4]="Opt";static struct _tagged_arr Cyc__genname_581={_tmp205,_tmp205,
_tmp205 + 4};static char _tmp206[2]="v";static struct _tuple5 Cyc__gentuple_579={
offsetof(struct Cyc_Core_Opt,v),{_tmp206,_tmp206,_tmp206 + 2},(void*)& Cyc__genrep_12};
static struct _tuple5*Cyc__genarr_580[1]={& Cyc__gentuple_579};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_var_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_581,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_580),(void*)((
struct _tuple5**)Cyc__genarr_580),(void*)((struct _tuple5**)Cyc__genarr_580 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_578={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static char _tmp209[11]="VarargInfo";static struct _tagged_arr Cyc__genname_587={
_tmp209,_tmp209,_tmp209 + 11};static char _tmp20A[5]="name";static struct _tuple5 Cyc__gentuple_582={
offsetof(struct Cyc_Absyn_VarargInfo,name),{_tmp20A,_tmp20A,_tmp20A + 5},(void*)&
Cyc__genrep_578};static char _tmp20B[3]="tq";static struct _tuple5 Cyc__gentuple_583={
offsetof(struct Cyc_Absyn_VarargInfo,tq),{_tmp20B,_tmp20B,_tmp20B + 3},(void*)& Cyc__genrep_131};
static char _tmp20C[5]="type";static struct _tuple5 Cyc__gentuple_584={offsetof(
struct Cyc_Absyn_VarargInfo,type),{_tmp20C,_tmp20C,_tmp20C + 5},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp20D[7]="inject";static struct _tuple5 Cyc__gentuple_585={
offsetof(struct Cyc_Absyn_VarargInfo,inject),{_tmp20D,_tmp20D,_tmp20D + 7},(void*)((
void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_586[4]={& Cyc__gentuple_582,&
Cyc__gentuple_583,& Cyc__gentuple_584,& Cyc__gentuple_585};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_587,sizeof(
struct Cyc_Absyn_VarargInfo),{(void*)((struct _tuple5**)Cyc__genarr_586),(void*)((
struct _tuple5**)Cyc__genarr_586),(void*)((struct _tuple5**)Cyc__genarr_586 + 4)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_577={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_353;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_354;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_355;static struct
_tuple6 Cyc__gentuple_356={offsetof(struct _tuple9,f1),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6 Cyc__gentuple_357={offsetof(struct _tuple9,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_358[2]={& Cyc__gentuple_356,&
Cyc__gentuple_357};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_355={4,
sizeof(struct _tuple9),{(void*)((struct _tuple6**)Cyc__genarr_358),(void*)((struct
_tuple6**)Cyc__genarr_358),(void*)((struct _tuple6**)Cyc__genarr_358 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_354={1,1,(void*)((void*)& Cyc__genrep_355)};
static char _tmp212[5]="List";static struct _tagged_arr Cyc__genname_362={_tmp212,
_tmp212,_tmp212 + 5};static char _tmp213[3]="hd";static struct _tuple5 Cyc__gentuple_359={
offsetof(struct Cyc_List_List,hd),{_tmp213,_tmp213,_tmp213 + 3},(void*)& Cyc__genrep_354};
static char _tmp214[3]="tl";static struct _tuple5 Cyc__gentuple_360={offsetof(struct
Cyc_List_List,tl),{_tmp214,_tmp214,_tmp214 + 3},(void*)& Cyc__genrep_353};static
struct _tuple5*Cyc__genarr_361[2]={& Cyc__gentuple_359,& Cyc__gentuple_360};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_362,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_361),(void*)((struct _tuple5**)Cyc__genarr_361),(void*)((
struct _tuple5**)Cyc__genarr_361 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_353={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_543;struct _tuple30{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_544={
offsetof(struct _tuple30,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_545={
offsetof(struct _tuple30,f2),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_546={
offsetof(struct _tuple30,f3),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_547[
3]={& Cyc__gentuple_544,& Cyc__gentuple_545,& Cyc__gentuple_546};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_543={4,sizeof(struct _tuple30),{(void*)((struct _tuple6**)Cyc__genarr_547),(
void*)((struct _tuple6**)Cyc__genarr_547),(void*)((struct _tuple6**)Cyc__genarr_547
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_539;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_77;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_77={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple31{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_540={offsetof(struct _tuple31,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_541={offsetof(struct _tuple31,f2),(
void*)& Cyc__genrep_77};static struct _tuple6*Cyc__genarr_542[2]={& Cyc__gentuple_540,&
Cyc__gentuple_541};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_539={4,
sizeof(struct _tuple31),{(void*)((struct _tuple6**)Cyc__genarr_542),(void*)((
struct _tuple6**)Cyc__genarr_542),(void*)((struct _tuple6**)Cyc__genarr_542 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_533;struct _tuple32{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*f3;struct Cyc_Absyn_Stmt*f4;};
static struct _tuple6 Cyc__gentuple_534={offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_535={offsetof(struct _tuple32,f2),(void*)& Cyc__genrep_81};
static struct _tuple6 Cyc__gentuple_536={offsetof(struct _tuple32,f3),(void*)& Cyc__genrep_159};
static struct _tuple6 Cyc__gentuple_537={offsetof(struct _tuple32,f4),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_538[4]={& Cyc__gentuple_534,& Cyc__gentuple_535,&
Cyc__gentuple_536,& Cyc__gentuple_537};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_533={
4,sizeof(struct _tuple32),{(void*)((struct _tuple6**)Cyc__genarr_538),(void*)((
struct _tuple6**)Cyc__genarr_538),(void*)((struct _tuple6**)Cyc__genarr_538 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_528;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_168;static struct _tuple6 Cyc__gentuple_169={offsetof(struct _tuple2,f1),(
void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_170={offsetof(struct
_tuple2,f2),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_171[2]={&
Cyc__gentuple_169,& Cyc__gentuple_170};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_168={
4,sizeof(struct _tuple2),{(void*)((struct _tuple6**)Cyc__genarr_171),(void*)((
struct _tuple6**)Cyc__genarr_171),(void*)((struct _tuple6**)Cyc__genarr_171 + 2)}};
struct _tuple33{unsigned int f1;struct _tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static
struct _tuple6 Cyc__gentuple_529={offsetof(struct _tuple33,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_530={offsetof(struct _tuple33,f2),(void*)& Cyc__genrep_168};
static struct _tuple6 Cyc__gentuple_531={offsetof(struct _tuple33,f3),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_532[3]={& Cyc__gentuple_529,& Cyc__gentuple_530,&
Cyc__gentuple_531};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_528={4,
sizeof(struct _tuple33),{(void*)((struct _tuple6**)Cyc__genarr_532),(void*)((
struct _tuple6**)Cyc__genarr_532),(void*)((struct _tuple6**)Cyc__genarr_532 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_524;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_519;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_519={1,1,(void*)((
void*)& Cyc_struct_Absyn_Stmt_rep)};struct _tuple34{unsigned int f1;struct Cyc_Absyn_Stmt*
f2;};static struct _tuple6 Cyc__gentuple_525={offsetof(struct _tuple34,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_526={offsetof(struct _tuple34,f2),(
void*)& Cyc__genrep_519};static struct _tuple6*Cyc__genarr_527[2]={& Cyc__gentuple_525,&
Cyc__gentuple_526};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_524={4,
sizeof(struct _tuple34),{(void*)((struct _tuple6**)Cyc__genarr_527),(void*)((
struct _tuple6**)Cyc__genarr_527),(void*)((struct _tuple6**)Cyc__genarr_527 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_518;struct _tuple35{unsigned int
f1;struct _tagged_arr*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_520={
offsetof(struct _tuple35,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_521={
offsetof(struct _tuple35,f2),(void*)& Cyc__genrep_12};static struct _tuple6 Cyc__gentuple_522={
offsetof(struct _tuple35,f3),(void*)& Cyc__genrep_519};static struct _tuple6*Cyc__genarr_523[
3]={& Cyc__gentuple_520,& Cyc__gentuple_521,& Cyc__gentuple_522};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_518={4,sizeof(struct _tuple35),{(void*)((struct _tuple6**)Cyc__genarr_523),(
void*)((struct _tuple6**)Cyc__genarr_523),(void*)((struct _tuple6**)Cyc__genarr_523
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_511;struct _tuple36{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_512={offsetof(struct _tuple36,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_513={offsetof(struct _tuple36,f2),(
void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_514={offsetof(struct
_tuple36,f3),(void*)& Cyc__genrep_168};static struct _tuple6 Cyc__gentuple_515={
offsetof(struct _tuple36,f4),(void*)& Cyc__genrep_168};static struct _tuple6 Cyc__gentuple_516={
offsetof(struct _tuple36,f5),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_517[
5]={& Cyc__gentuple_512,& Cyc__gentuple_513,& Cyc__gentuple_514,& Cyc__gentuple_515,&
Cyc__gentuple_516};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_511={4,
sizeof(struct _tuple36),{(void*)((struct _tuple6**)Cyc__genarr_517),(void*)((
struct _tuple6**)Cyc__genarr_517),(void*)((struct _tuple6**)Cyc__genarr_517 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_506;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_226;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_227;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Switch_clause_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_228;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_412;
struct _tuple37{unsigned int f1;char f2;};static struct _tuple6 Cyc__gentuple_413={
offsetof(struct _tuple37,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_414={
offsetof(struct _tuple37,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_415[2]={& Cyc__gentuple_413,& Cyc__gentuple_414};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_412={4,sizeof(struct _tuple37),{(void*)((struct _tuple6**)Cyc__genarr_415),(
void*)((struct _tuple6**)Cyc__genarr_415),(void*)((struct _tuple6**)Cyc__genarr_415
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_408;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_233;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;
static char _tmp222[5]="List";static struct _tagged_arr Cyc__genname_237={_tmp222,
_tmp222,_tmp222 + 5};static char _tmp223[3]="hd";static struct _tuple5 Cyc__gentuple_234={
offsetof(struct Cyc_List_List,hd),{_tmp223,_tmp223,_tmp223 + 3},(void*)& Cyc__genrep_228};
static char _tmp224[3]="tl";static struct _tuple5 Cyc__gentuple_235={offsetof(struct
Cyc_List_List,tl),{_tmp224,_tmp224,_tmp224 + 3},(void*)& Cyc__genrep_233};static
struct _tuple5*Cyc__genarr_236[2]={& Cyc__gentuple_234,& Cyc__gentuple_235};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_237,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_236),(void*)((struct _tuple5**)Cyc__genarr_236),(void*)((
struct _tuple5**)Cyc__genarr_236 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_233={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};static struct
_tuple6 Cyc__gentuple_409={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_410={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_233};
static struct _tuple6*Cyc__genarr_411[2]={& Cyc__gentuple_409,& Cyc__gentuple_410};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_408={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_411),(void*)((struct _tuple6**)Cyc__genarr_411),(
void*)((struct _tuple6**)Cyc__genarr_411 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_404;struct _tuple38{unsigned int f1;struct Cyc_Absyn_Pat*f2;};static
struct _tuple6 Cyc__gentuple_405={offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_406={offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_228};
static struct _tuple6*Cyc__genarr_407[2]={& Cyc__gentuple_405,& Cyc__gentuple_406};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_404={4,sizeof(struct _tuple38),{(
void*)((struct _tuple6**)Cyc__genarr_407),(void*)((struct _tuple6**)Cyc__genarr_407),(
void*)((struct _tuple6**)Cyc__genarr_407 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_311;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_383;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep;
static char _tmp229[8]="StructA";static struct _tuple7 Cyc__gentuple_368={0,{_tmp229,
_tmp229,_tmp229 + 8}};static char _tmp22A[7]="UnionA";static struct _tuple7 Cyc__gentuple_369={
1,{_tmp22A,_tmp22A,_tmp22A + 7}};static struct _tuple7*Cyc__genarr_370[2]={& Cyc__gentuple_368,&
Cyc__gentuple_369};static struct _tuple5*Cyc__genarr_371[0]={};static char _tmp22C[9]="AggrKind";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep={5,{_tmp22C,_tmp22C,
_tmp22C + 9},{(void*)((struct _tuple7**)Cyc__genarr_370),(void*)((struct _tuple7**)
Cyc__genarr_370),(void*)((struct _tuple7**)Cyc__genarr_370 + 2)},{(void*)((struct
_tuple5**)Cyc__genarr_371),(void*)((struct _tuple5**)Cyc__genarr_371),(void*)((
struct _tuple5**)Cyc__genarr_371 + 0)}};struct _tuple39{unsigned int f1;void*f2;
struct _tuple0*f3;};static struct _tuple6 Cyc__gentuple_384={offsetof(struct _tuple39,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_385={offsetof(struct
_tuple39,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};static struct _tuple6 Cyc__gentuple_386={
offsetof(struct _tuple39,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_387[
3]={& Cyc__gentuple_384,& Cyc__gentuple_385,& Cyc__gentuple_386};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_383={4,sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_387),(
void*)((struct _tuple6**)Cyc__genarr_387),(void*)((struct _tuple6**)Cyc__genarr_387
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_336;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_337;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_338;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_339;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_340;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_341;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_87;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_attribute_t46H2_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_61;
struct _tuple40{unsigned int f1;int f2;};static struct _tuple6 Cyc__gentuple_63={
offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_64={
offsetof(struct _tuple40,f2),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6*
Cyc__genarr_65[2]={& Cyc__gentuple_63,& Cyc__gentuple_64};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_61={4,sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_65),(
void*)((struct _tuple6**)Cyc__genarr_65),(void*)((struct _tuple6**)Cyc__genarr_65 + 
2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_105;extern struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_Format_Type_rep;static char _tmp22F[10]="Printf_ft";static struct
_tuple7 Cyc__gentuple_106={0,{_tmp22F,_tmp22F,_tmp22F + 10}};static char _tmp230[9]="Scanf_ft";
static struct _tuple7 Cyc__gentuple_107={1,{_tmp230,_tmp230,_tmp230 + 9}};static
struct _tuple7*Cyc__genarr_108[2]={& Cyc__gentuple_106,& Cyc__gentuple_107};static
struct _tuple5*Cyc__genarr_109[0]={};static char _tmp232[12]="Format_Type";struct
Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep={5,{_tmp232,_tmp232,
_tmp232 + 12},{(void*)((struct _tuple7**)Cyc__genarr_108),(void*)((struct _tuple7**)
Cyc__genarr_108),(void*)((struct _tuple7**)Cyc__genarr_108 + 2)},{(void*)((struct
_tuple5**)Cyc__genarr_109),(void*)((struct _tuple5**)Cyc__genarr_109),(void*)((
struct _tuple5**)Cyc__genarr_109 + 0)}};struct _tuple41{unsigned int f1;void*f2;int
f3;int f4;};static struct _tuple6 Cyc__gentuple_110={offsetof(struct _tuple41,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_111={offsetof(struct
_tuple41,f2),(void*)& Cyc_tunion_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_112={
offsetof(struct _tuple41,f3),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_113={offsetof(struct _tuple41,f4),(void*)((void*)& Cyc__genrep_62)};
static struct _tuple6*Cyc__genarr_114[4]={& Cyc__gentuple_110,& Cyc__gentuple_111,&
Cyc__gentuple_112,& Cyc__gentuple_113};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_105={
4,sizeof(struct _tuple41),{(void*)((struct _tuple6**)Cyc__genarr_114),(void*)((
struct _tuple6**)Cyc__genarr_114),(void*)((struct _tuple6**)Cyc__genarr_114 + 4)}};
static char _tmp234[12]="Stdcall_att";static struct _tuple7 Cyc__gentuple_88={0,{
_tmp234,_tmp234,_tmp234 + 12}};static char _tmp235[10]="Cdecl_att";static struct
_tuple7 Cyc__gentuple_89={1,{_tmp235,_tmp235,_tmp235 + 10}};static char _tmp236[13]="Fastcall_att";
static struct _tuple7 Cyc__gentuple_90={2,{_tmp236,_tmp236,_tmp236 + 13}};static char
_tmp237[13]="Noreturn_att";static struct _tuple7 Cyc__gentuple_91={3,{_tmp237,
_tmp237,_tmp237 + 13}};static char _tmp238[10]="Const_att";static struct _tuple7 Cyc__gentuple_92={
4,{_tmp238,_tmp238,_tmp238 + 10}};static char _tmp239[11]="Packed_att";static struct
_tuple7 Cyc__gentuple_93={5,{_tmp239,_tmp239,_tmp239 + 11}};static char _tmp23A[13]="Nocommon_att";
static struct _tuple7 Cyc__gentuple_94={6,{_tmp23A,_tmp23A,_tmp23A + 13}};static char
_tmp23B[11]="Shared_att";static struct _tuple7 Cyc__gentuple_95={7,{_tmp23B,_tmp23B,
_tmp23B + 11}};static char _tmp23C[11]="Unused_att";static struct _tuple7 Cyc__gentuple_96={
8,{_tmp23C,_tmp23C,_tmp23C + 11}};static char _tmp23D[9]="Weak_att";static struct
_tuple7 Cyc__gentuple_97={9,{_tmp23D,_tmp23D,_tmp23D + 9}};static char _tmp23E[14]="Dllimport_att";
static struct _tuple7 Cyc__gentuple_98={10,{_tmp23E,_tmp23E,_tmp23E + 14}};static
char _tmp23F[14]="Dllexport_att";static struct _tuple7 Cyc__gentuple_99={11,{_tmp23F,
_tmp23F,_tmp23F + 14}};static char _tmp240[27]="No_instrument_function_att";static
struct _tuple7 Cyc__gentuple_100={12,{_tmp240,_tmp240,_tmp240 + 27}};static char
_tmp241[16]="Constructor_att";static struct _tuple7 Cyc__gentuple_101={13,{_tmp241,
_tmp241,_tmp241 + 16}};static char _tmp242[15]="Destructor_att";static struct _tuple7
Cyc__gentuple_102={14,{_tmp242,_tmp242,_tmp242 + 15}};static char _tmp243[26]="No_check_memory_usage_att";
static struct _tuple7 Cyc__gentuple_103={15,{_tmp243,_tmp243,_tmp243 + 26}};static
struct _tuple7*Cyc__genarr_104[16]={& Cyc__gentuple_88,& Cyc__gentuple_89,& Cyc__gentuple_90,&
Cyc__gentuple_91,& Cyc__gentuple_92,& Cyc__gentuple_93,& Cyc__gentuple_94,& Cyc__gentuple_95,&
Cyc__gentuple_96,& Cyc__gentuple_97,& Cyc__gentuple_98,& Cyc__gentuple_99,& Cyc__gentuple_100,&
Cyc__gentuple_101,& Cyc__gentuple_102,& Cyc__gentuple_103};static char _tmp244[12]="Regparm_att";
static struct _tuple5 Cyc__gentuple_119={0,{_tmp244,_tmp244,_tmp244 + 12},(void*)&
Cyc__genrep_61};static char _tmp245[12]="Aligned_att";static struct _tuple5 Cyc__gentuple_120={
1,{_tmp245,_tmp245,_tmp245 + 12},(void*)& Cyc__genrep_61};static char _tmp246[12]="Section_att";
static struct _tuple5 Cyc__gentuple_121={2,{_tmp246,_tmp246,_tmp246 + 12},(void*)&
Cyc__genrep_115};static char _tmp247[11]="Format_att";static struct _tuple5 Cyc__gentuple_122={
3,{_tmp247,_tmp247,_tmp247 + 11},(void*)& Cyc__genrep_105};static struct _tuple5*Cyc__genarr_123[
4]={& Cyc__gentuple_119,& Cyc__gentuple_120,& Cyc__gentuple_121,& Cyc__gentuple_122};
static char _tmp249[10]="Attribute";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={
5,{_tmp249,_tmp249,_tmp249 + 10},{(void*)((struct _tuple7**)Cyc__genarr_104),(void*)((
struct _tuple7**)Cyc__genarr_104),(void*)((struct _tuple7**)Cyc__genarr_104 + 16)},{(
void*)((struct _tuple5**)Cyc__genarr_123),(void*)((struct _tuple5**)Cyc__genarr_123),(
void*)((struct _tuple5**)Cyc__genarr_123 + 4)}};static char _tmp24A[5]="List";static
struct _tagged_arr Cyc__genname_127={_tmp24A,_tmp24A,_tmp24A + 5};static char _tmp24B[
3]="hd";static struct _tuple5 Cyc__gentuple_124={offsetof(struct Cyc_List_List,hd),{
_tmp24B,_tmp24B,_tmp24B + 3},(void*)& Cyc_Absyn_attribute_t_rep};static char _tmp24C[
3]="tl";static struct _tuple5 Cyc__gentuple_125={offsetof(struct Cyc_List_List,tl),{
_tmp24C,_tmp24C,_tmp24C + 3},(void*)& Cyc__genrep_87};static struct _tuple5*Cyc__genarr_126[
2]={& Cyc__gentuple_124,& Cyc__gentuple_125};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_127,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_126),(void*)((struct _tuple5**)Cyc__genarr_126),(void*)((
struct _tuple5**)Cyc__genarr_126 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_87={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};static char
_tmp24F[10]="Aggrfield";static struct _tagged_arr Cyc__genname_348={_tmp24F,_tmp24F,
_tmp24F + 10};static char _tmp250[5]="name";static struct _tuple5 Cyc__gentuple_342={
offsetof(struct Cyc_Absyn_Aggrfield,name),{_tmp250,_tmp250,_tmp250 + 5},(void*)&
Cyc__genrep_12};static char _tmp251[3]="tq";static struct _tuple5 Cyc__gentuple_343={
offsetof(struct Cyc_Absyn_Aggrfield,tq),{_tmp251,_tmp251,_tmp251 + 3},(void*)& Cyc__genrep_131};
static char _tmp252[5]="type";static struct _tuple5 Cyc__gentuple_344={offsetof(
struct Cyc_Absyn_Aggrfield,type),{_tmp252,_tmp252,_tmp252 + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp253[6]="width";static struct _tuple5 Cyc__gentuple_345={offsetof(
struct Cyc_Absyn_Aggrfield,width),{_tmp253,_tmp253,_tmp253 + 6},(void*)& Cyc__genrep_77};
static char _tmp254[11]="attributes";static struct _tuple5 Cyc__gentuple_346={
offsetof(struct Cyc_Absyn_Aggrfield,attributes),{_tmp254,_tmp254,_tmp254 + 11},(
void*)& Cyc__genrep_87};static struct _tuple5*Cyc__genarr_347[5]={& Cyc__gentuple_342,&
Cyc__gentuple_343,& Cyc__gentuple_344,& Cyc__gentuple_345,& Cyc__gentuple_346};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={3,(struct
_tagged_arr*)& Cyc__genname_348,sizeof(struct Cyc_Absyn_Aggrfield),{(void*)((
struct _tuple5**)Cyc__genarr_347),(void*)((struct _tuple5**)Cyc__genarr_347),(void*)((
struct _tuple5**)Cyc__genarr_347 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_341={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrfield_rep)};static char _tmp257[5]="List";
static struct _tagged_arr Cyc__genname_352={_tmp257,_tmp257,_tmp257 + 5};static char
_tmp258[3]="hd";static struct _tuple5 Cyc__gentuple_349={offsetof(struct Cyc_List_List,hd),{
_tmp258,_tmp258,_tmp258 + 3},(void*)& Cyc__genrep_341};static char _tmp259[3]="tl";
static struct _tuple5 Cyc__gentuple_350={offsetof(struct Cyc_List_List,tl),{_tmp259,
_tmp259,_tmp259 + 3},(void*)& Cyc__genrep_340};static struct _tuple5*Cyc__genarr_351[
2]={& Cyc__gentuple_349,& Cyc__gentuple_350};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_352,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_351),(void*)((struct _tuple5**)Cyc__genarr_351),(void*)((
struct _tuple5**)Cyc__genarr_351 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_340={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};static char
_tmp25C[13]="AggrdeclImpl";static struct _tagged_arr Cyc__genname_367={_tmp25C,
_tmp25C,_tmp25C + 13};static char _tmp25D[11]="exist_vars";static struct _tuple5 Cyc__gentuple_363={
offsetof(struct Cyc_Absyn_AggrdeclImpl,exist_vars),{_tmp25D,_tmp25D,_tmp25D + 11},(
void*)& Cyc__genrep_294};static char _tmp25E[7]="rgn_po";static struct _tuple5 Cyc__gentuple_364={
offsetof(struct Cyc_Absyn_AggrdeclImpl,rgn_po),{_tmp25E,_tmp25E,_tmp25E + 7},(void*)&
Cyc__genrep_353};static char _tmp25F[7]="fields";static struct _tuple5 Cyc__gentuple_365={
offsetof(struct Cyc_Absyn_AggrdeclImpl,fields),{_tmp25F,_tmp25F,_tmp25F + 7},(void*)&
Cyc__genrep_340};static struct _tuple5*Cyc__genarr_366[3]={& Cyc__gentuple_363,& Cyc__gentuple_364,&
Cyc__gentuple_365};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep={
3,(struct _tagged_arr*)& Cyc__genname_367,sizeof(struct Cyc_Absyn_AggrdeclImpl),{(
void*)((struct _tuple5**)Cyc__genarr_366),(void*)((struct _tuple5**)Cyc__genarr_366),(
void*)((struct _tuple5**)Cyc__genarr_366 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_339={1,1,(void*)((void*)& Cyc_struct_Absyn_AggrdeclImpl_rep)};static
char _tmp262[9]="Aggrdecl";static struct _tagged_arr Cyc__genname_379={_tmp262,
_tmp262,_tmp262 + 9};static char _tmp263[5]="kind";static struct _tuple5 Cyc__gentuple_372={
offsetof(struct Cyc_Absyn_Aggrdecl,kind),{_tmp263,_tmp263,_tmp263 + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};
static char _tmp264[3]="sc";static struct _tuple5 Cyc__gentuple_373={offsetof(struct
Cyc_Absyn_Aggrdecl,sc),{_tmp264,_tmp264,_tmp264 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp265[5]="name";static struct _tuple5 Cyc__gentuple_374={offsetof(
struct Cyc_Absyn_Aggrdecl,name),{_tmp265,_tmp265,_tmp265 + 5},(void*)& Cyc__genrep_10};
static char _tmp266[4]="tvs";static struct _tuple5 Cyc__gentuple_375={offsetof(struct
Cyc_Absyn_Aggrdecl,tvs),{_tmp266,_tmp266,_tmp266 + 4},(void*)& Cyc__genrep_294};
static char _tmp267[5]="impl";static struct _tuple5 Cyc__gentuple_376={offsetof(
struct Cyc_Absyn_Aggrdecl,impl),{_tmp267,_tmp267,_tmp267 + 5},(void*)& Cyc__genrep_339};
static char _tmp268[11]="attributes";static struct _tuple5 Cyc__gentuple_377={
offsetof(struct Cyc_Absyn_Aggrdecl,attributes),{_tmp268,_tmp268,_tmp268 + 11},(
void*)& Cyc__genrep_87};static struct _tuple5*Cyc__genarr_378[6]={& Cyc__gentuple_372,&
Cyc__gentuple_373,& Cyc__gentuple_374,& Cyc__gentuple_375,& Cyc__gentuple_376,& Cyc__gentuple_377};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep={3,(struct
_tagged_arr*)& Cyc__genname_379,sizeof(struct Cyc_Absyn_Aggrdecl),{(void*)((struct
_tuple5**)Cyc__genarr_378),(void*)((struct _tuple5**)Cyc__genarr_378),(void*)((
struct _tuple5**)Cyc__genarr_378 + 6)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_338={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_337={1,1,(void*)((void*)& Cyc__genrep_338)};struct _tuple42{
unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6 Cyc__gentuple_380={
offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_381={
offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_337};static struct _tuple6*Cyc__genarr_382[
2]={& Cyc__gentuple_380,& Cyc__gentuple_381};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_336={4,sizeof(struct _tuple42),{(void*)((struct _tuple6**)Cyc__genarr_382),(
void*)((struct _tuple6**)Cyc__genarr_382),(void*)((struct _tuple6**)Cyc__genarr_382
+ 2)}};static struct _tuple7*Cyc__genarr_335[0]={};static char _tmp26D[12]="UnknownAggr";
static struct _tuple5 Cyc__gentuple_388={0,{_tmp26D,_tmp26D,_tmp26D + 12},(void*)&
Cyc__genrep_383};static char _tmp26E[10]="KnownAggr";static struct _tuple5 Cyc__gentuple_389={
1,{_tmp26E,_tmp26E,_tmp26E + 10},(void*)& Cyc__genrep_336};static struct _tuple5*Cyc__genarr_390[
2]={& Cyc__gentuple_388,& Cyc__gentuple_389};static char _tmp270[10]="AggrInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep={5,{_tmp270,
_tmp270,_tmp270 + 10},{(void*)((struct _tuple7**)Cyc__genarr_335),(void*)((struct
_tuple7**)Cyc__genarr_335),(void*)((struct _tuple7**)Cyc__genarr_335 + 0)},{(void*)((
struct _tuple5**)Cyc__genarr_390),(void*)((struct _tuple5**)Cyc__genarr_390),(void*)((
struct _tuple5**)Cyc__genarr_390 + 2)}};static char _tmp271[9]="AggrInfo";static
struct _tagged_arr Cyc__genname_394={_tmp271,_tmp271,_tmp271 + 9};static char _tmp272[
10]="aggr_info";static struct _tuple5 Cyc__gentuple_391={offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),{
_tmp272,_tmp272,_tmp272 + 10},(void*)& Cyc_tunion_Absyn_AggrInfoU_rep};static char
_tmp273[6]="targs";static struct _tuple5 Cyc__gentuple_392={offsetof(struct Cyc_Absyn_AggrInfo,targs),{
_tmp273,_tmp273,_tmp273 + 6},(void*)& Cyc__genrep_53};static struct _tuple5*Cyc__genarr_393[
2]={& Cyc__gentuple_391,& Cyc__gentuple_392};struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_394,sizeof(struct Cyc_Absyn_AggrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_393),(void*)((struct _tuple5**)Cyc__genarr_393),(void*)((
struct _tuple5**)Cyc__genarr_393 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_312;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_313;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_314;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_315;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_317;struct _tuple43{unsigned int f1;struct _tagged_arr*f2;};static
struct _tuple6 Cyc__gentuple_318={offsetof(struct _tuple43,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_319={offsetof(struct _tuple43,f2),(void*)& Cyc__genrep_12};
static struct _tuple6*Cyc__genarr_320[2]={& Cyc__gentuple_318,& Cyc__gentuple_319};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_317={4,sizeof(struct _tuple43),{(
void*)((struct _tuple6**)Cyc__genarr_320),(void*)((struct _tuple6**)Cyc__genarr_320),(
void*)((struct _tuple6**)Cyc__genarr_320 + 2)}};static struct _tuple7*Cyc__genarr_316[
0]={};static char _tmp276[13]="ArrayElement";static struct _tuple5 Cyc__gentuple_321={
0,{_tmp276,_tmp276,_tmp276 + 13},(void*)& Cyc__genrep_80};static char _tmp277[10]="FieldName";
static struct _tuple5 Cyc__gentuple_322={1,{_tmp277,_tmp277,_tmp277 + 10},(void*)&
Cyc__genrep_317};static struct _tuple5*Cyc__genarr_323[2]={& Cyc__gentuple_321,& Cyc__gentuple_322};
static char _tmp279[11]="Designator";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={
5,{_tmp279,_tmp279,_tmp279 + 11},{(void*)((struct _tuple7**)Cyc__genarr_316),(void*)((
struct _tuple7**)Cyc__genarr_316),(void*)((struct _tuple7**)Cyc__genarr_316 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_323),(void*)((struct _tuple5**)Cyc__genarr_323),(
void*)((struct _tuple5**)Cyc__genarr_323 + 2)}};static char _tmp27A[5]="List";static
struct _tagged_arr Cyc__genname_327={_tmp27A,_tmp27A,_tmp27A + 5};static char _tmp27B[
3]="hd";static struct _tuple5 Cyc__gentuple_324={offsetof(struct Cyc_List_List,hd),{
_tmp27B,_tmp27B,_tmp27B + 3},(void*)& Cyc_Absyn_designator_t_rep};static char
_tmp27C[3]="tl";static struct _tuple5 Cyc__gentuple_325={offsetof(struct Cyc_List_List,tl),{
_tmp27C,_tmp27C,_tmp27C + 3},(void*)& Cyc__genrep_315};static struct _tuple5*Cyc__genarr_326[
2]={& Cyc__gentuple_324,& Cyc__gentuple_325};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_327,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_326),(void*)((struct _tuple5**)Cyc__genarr_326),(void*)((
struct _tuple5**)Cyc__genarr_326 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_315={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};struct
_tuple44{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_328={
offsetof(struct _tuple44,f1),(void*)& Cyc__genrep_315};static struct _tuple6 Cyc__gentuple_329={
offsetof(struct _tuple44,f2),(void*)& Cyc__genrep_228};static struct _tuple6*Cyc__genarr_330[
2]={& Cyc__gentuple_328,& Cyc__gentuple_329};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_314={4,sizeof(struct _tuple44),{(void*)((struct _tuple6**)Cyc__genarr_330),(
void*)((struct _tuple6**)Cyc__genarr_330),(void*)((struct _tuple6**)Cyc__genarr_330
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_313={1,1,(void*)((void*)&
Cyc__genrep_314)};static char _tmp281[5]="List";static struct _tagged_arr Cyc__genname_334={
_tmp281,_tmp281,_tmp281 + 5};static char _tmp282[3]="hd";static struct _tuple5 Cyc__gentuple_331={
offsetof(struct Cyc_List_List,hd),{_tmp282,_tmp282,_tmp282 + 3},(void*)& Cyc__genrep_313};
static char _tmp283[3]="tl";static struct _tuple5 Cyc__gentuple_332={offsetof(struct
Cyc_List_List,tl),{_tmp283,_tmp283,_tmp283 + 3},(void*)& Cyc__genrep_312};static
struct _tuple5*Cyc__genarr_333[2]={& Cyc__gentuple_331,& Cyc__gentuple_332};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_334,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_333),(void*)((struct _tuple5**)Cyc__genarr_333),(void*)((
struct _tuple5**)Cyc__genarr_333 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_312={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple45{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;};static struct _tuple6 Cyc__gentuple_395={offsetof(struct
_tuple45,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_396={
offsetof(struct _tuple45,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_397={offsetof(struct _tuple45,f3),(void*)& Cyc__genrep_294};
static struct _tuple6 Cyc__gentuple_398={offsetof(struct _tuple45,f4),(void*)& Cyc__genrep_312};
static struct _tuple6*Cyc__genarr_399[4]={& Cyc__gentuple_395,& Cyc__gentuple_396,&
Cyc__gentuple_397,& Cyc__gentuple_398};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_311={
4,sizeof(struct _tuple45),{(void*)((struct _tuple6**)Cyc__genarr_399),(void*)((
struct _tuple6**)Cyc__genarr_399),(void*)((struct _tuple6**)Cyc__genarr_399 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_266;struct _tuple46{unsigned int
f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;struct Cyc_List_List*
f4;};static struct _tuple6 Cyc__gentuple_306={offsetof(struct _tuple46,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_307={offsetof(struct _tuple46,f2),(
void*)((void*)& Cyc__genrep_284)};static struct _tuple6 Cyc__gentuple_308={offsetof(
struct _tuple46,f3),(void*)& Cyc__genrep_267};static struct _tuple6 Cyc__gentuple_309={
offsetof(struct _tuple46,f4),(void*)& Cyc__genrep_233};static struct _tuple6*Cyc__genarr_310[
4]={& Cyc__gentuple_306,& Cyc__gentuple_307,& Cyc__gentuple_308,& Cyc__gentuple_309};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_266={4,sizeof(struct _tuple46),{(
void*)((struct _tuple6**)Cyc__genarr_310),(void*)((struct _tuple6**)Cyc__genarr_310),(
void*)((struct _tuple6**)Cyc__genarr_310 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_251;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_252;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_253;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_75;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_76;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep;
static char _tmp288[10]="Enumfield";static struct _tagged_arr Cyc__genname_914={
_tmp288,_tmp288,_tmp288 + 10};static char _tmp289[5]="name";static struct _tuple5 Cyc__gentuple_910={
offsetof(struct Cyc_Absyn_Enumfield,name),{_tmp289,_tmp289,_tmp289 + 5},(void*)&
Cyc__genrep_10};static char _tmp28A[4]="tag";static struct _tuple5 Cyc__gentuple_911={
offsetof(struct Cyc_Absyn_Enumfield,tag),{_tmp28A,_tmp28A,_tmp28A + 4},(void*)& Cyc__genrep_77};
static char _tmp28B[4]="loc";static struct _tuple5 Cyc__gentuple_912={offsetof(struct
Cyc_Absyn_Enumfield,loc),{_tmp28B,_tmp28B,_tmp28B + 4},(void*)& Cyc__genrep_2};
static struct _tuple5*Cyc__genarr_913[3]={& Cyc__gentuple_910,& Cyc__gentuple_911,&
Cyc__gentuple_912};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={
3,(struct _tagged_arr*)& Cyc__genname_914,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple5**)Cyc__genarr_913),(void*)((struct _tuple5**)Cyc__genarr_913),(void*)((
struct _tuple5**)Cyc__genarr_913 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_76={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};static char _tmp28E[5]="List";
static struct _tagged_arr Cyc__genname_918={_tmp28E,_tmp28E,_tmp28E + 5};static char
_tmp28F[3]="hd";static struct _tuple5 Cyc__gentuple_915={offsetof(struct Cyc_List_List,hd),{
_tmp28F,_tmp28F,_tmp28F + 3},(void*)& Cyc__genrep_76};static char _tmp290[3]="tl";
static struct _tuple5 Cyc__gentuple_916={offsetof(struct Cyc_List_List,tl),{_tmp290,
_tmp290,_tmp290 + 3},(void*)& Cyc__genrep_75};static struct _tuple5*Cyc__genarr_917[
2]={& Cyc__gentuple_915,& Cyc__gentuple_916};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_918,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_917),(void*)((struct _tuple5**)Cyc__genarr_917),(void*)((
struct _tuple5**)Cyc__genarr_917 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_75={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static char
_tmp293[4]="Opt";static struct _tagged_arr Cyc__genname_256={_tmp293,_tmp293,
_tmp293 + 4};static char _tmp294[2]="v";static struct _tuple5 Cyc__gentuple_254={
offsetof(struct Cyc_Core_Opt,v),{_tmp294,_tmp294,_tmp294 + 2},(void*)& Cyc__genrep_75};
static struct _tuple5*Cyc__genarr_255[1]={& Cyc__gentuple_254};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_256,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_255),(
void*)((struct _tuple5**)Cyc__genarr_255),(void*)((struct _tuple5**)Cyc__genarr_255
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_253={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};static char _tmp297[9]="Enumdecl";
static struct _tagged_arr Cyc__genname_261={_tmp297,_tmp297,_tmp297 + 9};static char
_tmp298[3]="sc";static struct _tuple5 Cyc__gentuple_257={offsetof(struct Cyc_Absyn_Enumdecl,sc),{
_tmp298,_tmp298,_tmp298 + 3},(void*)& Cyc_Absyn_scope_t_rep};static char _tmp299[5]="name";
static struct _tuple5 Cyc__gentuple_258={offsetof(struct Cyc_Absyn_Enumdecl,name),{
_tmp299,_tmp299,_tmp299 + 5},(void*)& Cyc__genrep_10};static char _tmp29A[7]="fields";
static struct _tuple5 Cyc__gentuple_259={offsetof(struct Cyc_Absyn_Enumdecl,fields),{
_tmp29A,_tmp29A,_tmp29A + 7},(void*)& Cyc__genrep_253};static struct _tuple5*Cyc__genarr_260[
3]={& Cyc__gentuple_257,& Cyc__gentuple_258,& Cyc__gentuple_259};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumdecl_rep={3,(struct _tagged_arr*)& Cyc__genname_261,sizeof(
struct Cyc_Absyn_Enumdecl),{(void*)((struct _tuple5**)Cyc__genarr_260),(void*)((
struct _tuple5**)Cyc__genarr_260),(void*)((struct _tuple5**)Cyc__genarr_260 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_252={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
struct _tuple47{unsigned int f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*
f3;};static struct _tuple6 Cyc__gentuple_262={offsetof(struct _tuple47,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_263={offsetof(struct _tuple47,f2),(
void*)& Cyc__genrep_252};static struct _tuple6 Cyc__gentuple_264={offsetof(struct
_tuple47,f3),(void*)& Cyc__genrep_76};static struct _tuple6*Cyc__genarr_265[3]={&
Cyc__gentuple_262,& Cyc__gentuple_263,& Cyc__gentuple_264};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_251={4,sizeof(struct _tuple47),{(void*)((struct _tuple6**)Cyc__genarr_265),(
void*)((struct _tuple6**)Cyc__genarr_265),(void*)((struct _tuple6**)Cyc__genarr_265
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_246;struct _tuple48{
unsigned int f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_247={
offsetof(struct _tuple48,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_248={
offsetof(struct _tuple48,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_249={offsetof(struct _tuple48,f3),(void*)& Cyc__genrep_76};
static struct _tuple6*Cyc__genarr_250[3]={& Cyc__gentuple_247,& Cyc__gentuple_248,&
Cyc__gentuple_249};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_246={4,
sizeof(struct _tuple48),{(void*)((struct _tuple6**)Cyc__genarr_250),(void*)((
struct _tuple6**)Cyc__genarr_250),(void*)((struct _tuple6**)Cyc__genarr_250 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_242;struct _tuple49{unsigned int
f1;struct _tuple0*f2;};static struct _tuple6 Cyc__gentuple_243={offsetof(struct
_tuple49,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_244={
offsetof(struct _tuple49,f2),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_245[
2]={& Cyc__gentuple_243,& Cyc__gentuple_244};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_242={4,sizeof(struct _tuple49),{(void*)((struct _tuple6**)Cyc__genarr_245),(
void*)((struct _tuple6**)Cyc__genarr_245),(void*)((struct _tuple6**)Cyc__genarr_245
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_232;struct _tuple50{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;};static struct _tuple6 Cyc__gentuple_238={
offsetof(struct _tuple50,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_239={
offsetof(struct _tuple50,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_240={
offsetof(struct _tuple50,f3),(void*)& Cyc__genrep_233};static struct _tuple6*Cyc__genarr_241[
3]={& Cyc__gentuple_238,& Cyc__gentuple_239,& Cyc__gentuple_240};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_232={4,sizeof(struct _tuple50),{(void*)((struct _tuple6**)Cyc__genarr_241),(
void*)((struct _tuple6**)Cyc__genarr_241),(void*)((struct _tuple6**)Cyc__genarr_241
+ 3)}};static char _tmp2A1[7]="Wild_p";static struct _tuple7 Cyc__gentuple_229={0,{
_tmp2A1,_tmp2A1,_tmp2A1 + 7}};static char _tmp2A2[7]="Null_p";static struct _tuple7
Cyc__gentuple_230={1,{_tmp2A2,_tmp2A2,_tmp2A2 + 7}};static struct _tuple7*Cyc__genarr_231[
2]={& Cyc__gentuple_229,& Cyc__gentuple_230};static char _tmp2A3[6]="Var_p";static
struct _tuple5 Cyc__gentuple_426={0,{_tmp2A3,_tmp2A3,_tmp2A3 + 6},(void*)& Cyc__genrep_400};
static char _tmp2A4[6]="Int_p";static struct _tuple5 Cyc__gentuple_427={1,{_tmp2A4,
_tmp2A4,_tmp2A4 + 6},(void*)& Cyc__genrep_416};static char _tmp2A5[7]="Char_p";
static struct _tuple5 Cyc__gentuple_428={2,{_tmp2A5,_tmp2A5,_tmp2A5 + 7},(void*)& Cyc__genrep_412};
static char _tmp2A6[8]="Float_p";static struct _tuple5 Cyc__gentuple_429={3,{_tmp2A6,
_tmp2A6,_tmp2A6 + 8},(void*)& Cyc__genrep_115};static char _tmp2A7[8]="Tuple_p";
static struct _tuple5 Cyc__gentuple_430={4,{_tmp2A7,_tmp2A7,_tmp2A7 + 8},(void*)& Cyc__genrep_408};
static char _tmp2A8[10]="Pointer_p";static struct _tuple5 Cyc__gentuple_431={5,{
_tmp2A8,_tmp2A8,_tmp2A8 + 10},(void*)& Cyc__genrep_404};static char _tmp2A9[12]="Reference_p";
static struct _tuple5 Cyc__gentuple_432={6,{_tmp2A9,_tmp2A9,_tmp2A9 + 12},(void*)&
Cyc__genrep_400};static char _tmp2AA[7]="Aggr_p";static struct _tuple5 Cyc__gentuple_433={
7,{_tmp2AA,_tmp2AA,_tmp2AA + 7},(void*)& Cyc__genrep_311};static char _tmp2AB[9]="Tunion_p";
static struct _tuple5 Cyc__gentuple_434={8,{_tmp2AB,_tmp2AB,_tmp2AB + 9},(void*)& Cyc__genrep_266};
static char _tmp2AC[7]="Enum_p";static struct _tuple5 Cyc__gentuple_435={9,{_tmp2AC,
_tmp2AC,_tmp2AC + 7},(void*)& Cyc__genrep_251};static char _tmp2AD[11]="AnonEnum_p";
static struct _tuple5 Cyc__gentuple_436={10,{_tmp2AD,_tmp2AD,_tmp2AD + 11},(void*)&
Cyc__genrep_246};static char _tmp2AE[12]="UnknownId_p";static struct _tuple5 Cyc__gentuple_437={
11,{_tmp2AE,_tmp2AE,_tmp2AE + 12},(void*)& Cyc__genrep_242};static char _tmp2AF[14]="UnknownCall_p";
static struct _tuple5 Cyc__gentuple_438={12,{_tmp2AF,_tmp2AF,_tmp2AF + 14},(void*)&
Cyc__genrep_232};static struct _tuple5*Cyc__genarr_439[13]={& Cyc__gentuple_426,&
Cyc__gentuple_427,& Cyc__gentuple_428,& Cyc__gentuple_429,& Cyc__gentuple_430,& Cyc__gentuple_431,&
Cyc__gentuple_432,& Cyc__gentuple_433,& Cyc__gentuple_434,& Cyc__gentuple_435,& Cyc__gentuple_436,&
Cyc__gentuple_437,& Cyc__gentuple_438};static char _tmp2B1[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,{_tmp2B1,_tmp2B1,_tmp2B1 + 8},{(void*)((struct _tuple7**)
Cyc__genarr_231),(void*)((struct _tuple7**)Cyc__genarr_231),(void*)((struct
_tuple7**)Cyc__genarr_231 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_439),(void*)((
struct _tuple5**)Cyc__genarr_439),(void*)((struct _tuple5**)Cyc__genarr_439 + 13)}};
static char _tmp2B2[4]="Pat";static struct _tagged_arr Cyc__genname_444={_tmp2B2,
_tmp2B2,_tmp2B2 + 4};static char _tmp2B3[2]="r";static struct _tuple5 Cyc__gentuple_440={
offsetof(struct Cyc_Absyn_Pat,r),{_tmp2B3,_tmp2B3,_tmp2B3 + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};
static char _tmp2B4[5]="topt";static struct _tuple5 Cyc__gentuple_441={offsetof(
struct Cyc_Absyn_Pat,topt),{_tmp2B4,_tmp2B4,_tmp2B4 + 5},(void*)& Cyc__genrep_43};
static char _tmp2B5[4]="loc";static struct _tuple5 Cyc__gentuple_442={offsetof(struct
Cyc_Absyn_Pat,loc),{_tmp2B5,_tmp2B5,_tmp2B5 + 4},(void*)& Cyc__genrep_2};static
struct _tuple5*Cyc__genarr_443[3]={& Cyc__gentuple_440,& Cyc__gentuple_441,& Cyc__gentuple_442};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={3,(struct _tagged_arr*)&
Cyc__genname_444,sizeof(struct Cyc_Absyn_Pat),{(void*)((struct _tuple5**)Cyc__genarr_443),(
void*)((struct _tuple5**)Cyc__genarr_443),(void*)((struct _tuple5**)Cyc__genarr_443
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_228={1,1,(void*)((void*)&
Cyc_struct_Absyn_Pat_rep)};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_128;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_129;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;static char _tmp2B8[5]="List";static
struct _tagged_arr Cyc__genname_155={_tmp2B8,_tmp2B8,_tmp2B8 + 5};static char _tmp2B9[
3]="hd";static struct _tuple5 Cyc__gentuple_152={offsetof(struct Cyc_List_List,hd),{
_tmp2B9,_tmp2B9,_tmp2B9 + 3},(void*)& Cyc__genrep_130};static char _tmp2BA[3]="tl";
static struct _tuple5 Cyc__gentuple_153={offsetof(struct Cyc_List_List,tl),{_tmp2BA,
_tmp2BA,_tmp2BA + 3},(void*)& Cyc__genrep_129};static struct _tuple5*Cyc__genarr_154[
2]={& Cyc__gentuple_152,& Cyc__gentuple_153};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_155,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_154),(void*)((struct _tuple5**)Cyc__genarr_154),(void*)((
struct _tuple5**)Cyc__genarr_154 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_129={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static char
_tmp2BD[4]="Opt";static struct _tagged_arr Cyc__genname_158={_tmp2BD,_tmp2BD,
_tmp2BD + 4};static char _tmp2BE[2]="v";static struct _tuple5 Cyc__gentuple_156={
offsetof(struct Cyc_Core_Opt,v),{_tmp2BE,_tmp2BE,_tmp2BE + 2},(void*)& Cyc__genrep_129};
static struct _tuple5*Cyc__genarr_157[1]={& Cyc__gentuple_156};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={3,(struct _tagged_arr*)&
Cyc__genname_158,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_157),(
void*)((struct _tuple5**)Cyc__genarr_157),(void*)((struct _tuple5**)Cyc__genarr_157
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_128={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};static char _tmp2C1[14]="Switch_clause";
static struct _tagged_arr Cyc__genname_451={_tmp2C1,_tmp2C1,_tmp2C1 + 14};static char
_tmp2C2[8]="pattern";static struct _tuple5 Cyc__gentuple_445={offsetof(struct Cyc_Absyn_Switch_clause,pattern),{
_tmp2C2,_tmp2C2,_tmp2C2 + 8},(void*)& Cyc__genrep_228};static char _tmp2C3[9]="pat_vars";
static struct _tuple5 Cyc__gentuple_446={offsetof(struct Cyc_Absyn_Switch_clause,pat_vars),{
_tmp2C3,_tmp2C3,_tmp2C3 + 9},(void*)& Cyc__genrep_128};static char _tmp2C4[13]="where_clause";
static struct _tuple5 Cyc__gentuple_447={offsetof(struct Cyc_Absyn_Switch_clause,where_clause),{
_tmp2C4,_tmp2C4,_tmp2C4 + 13},(void*)& Cyc__genrep_77};static char _tmp2C5[5]="body";
static struct _tuple5 Cyc__gentuple_448={offsetof(struct Cyc_Absyn_Switch_clause,body),{
_tmp2C5,_tmp2C5,_tmp2C5 + 5},(void*)& Cyc__genrep_159};static char _tmp2C6[4]="loc";
static struct _tuple5 Cyc__gentuple_449={offsetof(struct Cyc_Absyn_Switch_clause,loc),{
_tmp2C6,_tmp2C6,_tmp2C6 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_450[
5]={& Cyc__gentuple_445,& Cyc__gentuple_446,& Cyc__gentuple_447,& Cyc__gentuple_448,&
Cyc__gentuple_449};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _tagged_arr*)& Cyc__genname_451,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_450),(void*)((struct _tuple5**)Cyc__genarr_450),(
void*)((struct _tuple5**)Cyc__genarr_450 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_227={1,1,(void*)((void*)& Cyc_struct_Absyn_Switch_clause_rep)};static
char _tmp2C9[5]="List";static struct _tagged_arr Cyc__genname_455={_tmp2C9,_tmp2C9,
_tmp2C9 + 5};static char _tmp2CA[3]="hd";static struct _tuple5 Cyc__gentuple_452={
offsetof(struct Cyc_List_List,hd),{_tmp2CA,_tmp2CA,_tmp2CA + 3},(void*)((void*)&
Cyc__genrep_227)};static char _tmp2CB[3]="tl";static struct _tuple5 Cyc__gentuple_453={
offsetof(struct Cyc_List_List,tl),{_tmp2CB,_tmp2CB,_tmp2CB + 3},(void*)& Cyc__genrep_226};
static struct _tuple5*Cyc__genarr_454[2]={& Cyc__gentuple_452,& Cyc__gentuple_453};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_455,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_454),(void*)((struct _tuple5**)Cyc__genarr_454),(void*)((
struct _tuple5**)Cyc__genarr_454 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_226={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};struct
_tuple51{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_507={offsetof(struct _tuple51,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_508={offsetof(struct _tuple51,f2),(void*)& Cyc__genrep_81};
static struct _tuple6 Cyc__gentuple_509={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_226};
static struct _tuple6*Cyc__genarr_510[3]={& Cyc__gentuple_507,& Cyc__gentuple_508,&
Cyc__gentuple_509};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_506={4,
sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_510),(void*)((
struct _tuple6**)Cyc__genarr_510),(void*)((struct _tuple6**)Cyc__genarr_510 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_490;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_491;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_492;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep;static char _tmp2CF[15]="SwitchC_clause";
static struct _tagged_arr Cyc__genname_497={_tmp2CF,_tmp2CF,_tmp2CF + 15};static char
_tmp2D0[9]="cnst_exp";static struct _tuple5 Cyc__gentuple_493={offsetof(struct Cyc_Absyn_SwitchC_clause,cnst_exp),{
_tmp2D0,_tmp2D0,_tmp2D0 + 9},(void*)& Cyc__genrep_77};static char _tmp2D1[5]="body";
static struct _tuple5 Cyc__gentuple_494={offsetof(struct Cyc_Absyn_SwitchC_clause,body),{
_tmp2D1,_tmp2D1,_tmp2D1 + 5},(void*)& Cyc__genrep_159};static char _tmp2D2[4]="loc";
static struct _tuple5 Cyc__gentuple_495={offsetof(struct Cyc_Absyn_SwitchC_clause,loc),{
_tmp2D2,_tmp2D2,_tmp2D2 + 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_496[
3]={& Cyc__gentuple_493,& Cyc__gentuple_494,& Cyc__gentuple_495};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_SwitchC_clause_rep={3,(struct _tagged_arr*)& Cyc__genname_497,
sizeof(struct Cyc_Absyn_SwitchC_clause),{(void*)((struct _tuple5**)Cyc__genarr_496),(
void*)((struct _tuple5**)Cyc__genarr_496),(void*)((struct _tuple5**)Cyc__genarr_496
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_492={1,1,(void*)((void*)&
Cyc_struct_Absyn_SwitchC_clause_rep)};static char _tmp2D5[5]="List";static struct
_tagged_arr Cyc__genname_501={_tmp2D5,_tmp2D5,_tmp2D5 + 5};static char _tmp2D6[3]="hd";
static struct _tuple5 Cyc__gentuple_498={offsetof(struct Cyc_List_List,hd),{_tmp2D6,
_tmp2D6,_tmp2D6 + 3},(void*)& Cyc__genrep_492};static char _tmp2D7[3]="tl";static
struct _tuple5 Cyc__gentuple_499={offsetof(struct Cyc_List_List,tl),{_tmp2D7,
_tmp2D7,_tmp2D7 + 3},(void*)& Cyc__genrep_491};static struct _tuple5*Cyc__genarr_500[
2]={& Cyc__gentuple_498,& Cyc__gentuple_499};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_501,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_500),(void*)((struct _tuple5**)Cyc__genarr_500),(void*)((
struct _tuple5**)Cyc__genarr_500 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_491={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep)};static
struct _tuple6 Cyc__gentuple_502={offsetof(struct _tuple51,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_503={offsetof(struct _tuple51,f2),(void*)& Cyc__genrep_81};
static struct _tuple6 Cyc__gentuple_504={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_491};
static struct _tuple6*Cyc__genarr_505[3]={& Cyc__gentuple_502,& Cyc__gentuple_503,&
Cyc__gentuple_504};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_490={4,
sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_505),(void*)((
struct _tuple6**)Cyc__genarr_505),(void*)((struct _tuple6**)Cyc__genarr_505 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_479;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_481;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep;
static char _tmp2DB[5]="List";static struct _tagged_arr Cyc__genname_485={_tmp2DB,
_tmp2DB,_tmp2DB + 5};static char _tmp2DC[3]="hd";static struct _tuple5 Cyc__gentuple_482={
offsetof(struct Cyc_List_List,hd),{_tmp2DC,_tmp2DC,_tmp2DC + 3},(void*)& Cyc__genrep_81};
static char _tmp2DD[3]="tl";static struct _tuple5 Cyc__gentuple_483={offsetof(struct
Cyc_List_List,tl),{_tmp2DD,_tmp2DD,_tmp2DD + 3},(void*)& Cyc__genrep_481};static
struct _tuple5*Cyc__genarr_484[2]={& Cyc__gentuple_482,& Cyc__gentuple_483};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={3,(struct
_tagged_arr*)& Cyc__genname_485,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_484),(void*)((struct _tuple5**)Cyc__genarr_484),(void*)((
struct _tuple5**)Cyc__genarr_484 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_481={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_480;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_480={1,1,(void*)((
void*)& Cyc__genrep_227)};struct _tuple52{unsigned int f1;struct Cyc_List_List*f2;
struct Cyc_Absyn_Switch_clause**f3;};static struct _tuple6 Cyc__gentuple_486={
offsetof(struct _tuple52,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_487={
offsetof(struct _tuple52,f2),(void*)& Cyc__genrep_481};static struct _tuple6 Cyc__gentuple_488={
offsetof(struct _tuple52,f3),(void*)& Cyc__genrep_480};static struct _tuple6*Cyc__genarr_489[
3]={& Cyc__gentuple_486,& Cyc__gentuple_487,& Cyc__gentuple_488};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_479={4,sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_489),(
void*)((struct _tuple6**)Cyc__genarr_489),(void*)((struct _tuple6**)Cyc__genarr_489
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_474;struct _tuple53{
unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};static struct
_tuple6 Cyc__gentuple_475={offsetof(struct _tuple53,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_476={offsetof(struct _tuple53,f2),(void*)& Cyc__genrep_1};
static struct _tuple6 Cyc__gentuple_477={offsetof(struct _tuple53,f3),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_478[3]={& Cyc__gentuple_475,& Cyc__gentuple_476,&
Cyc__gentuple_477};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_474={4,
sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_478),(void*)((
struct _tuple6**)Cyc__genarr_478),(void*)((struct _tuple6**)Cyc__genarr_478 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_470;static struct _tuple6 Cyc__gentuple_471={
offsetof(struct _tuple34,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_472={
offsetof(struct _tuple34,f2),(void*)& Cyc__genrep_159};static struct _tuple6*Cyc__genarr_473[
2]={& Cyc__gentuple_471,& Cyc__gentuple_472};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_470={4,sizeof(struct _tuple34),{(void*)((struct _tuple6**)Cyc__genarr_473),(
void*)((struct _tuple6**)Cyc__genarr_473),(void*)((struct _tuple6**)Cyc__genarr_473
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_465;static struct _tuple6
Cyc__gentuple_466={offsetof(struct _tuple35,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_467={offsetof(struct _tuple35,f2),(void*)& Cyc__genrep_12};
static struct _tuple6 Cyc__gentuple_468={offsetof(struct _tuple35,f3),(void*)& Cyc__genrep_159};
static struct _tuple6*Cyc__genarr_469[3]={& Cyc__gentuple_466,& Cyc__gentuple_467,&
Cyc__gentuple_468};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_465={4,
sizeof(struct _tuple35),{(void*)((struct _tuple6**)Cyc__genarr_469),(void*)((
struct _tuple6**)Cyc__genarr_469),(void*)((struct _tuple6**)Cyc__genarr_469 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_460;struct _tuple54{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static struct _tuple6 Cyc__gentuple_461={
offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_462={
offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_159};static struct _tuple6 Cyc__gentuple_463={
offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_168};static struct _tuple6*Cyc__genarr_464[
3]={& Cyc__gentuple_461,& Cyc__gentuple_462,& Cyc__gentuple_463};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_460={4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_464),(
void*)((struct _tuple6**)Cyc__genarr_464),(void*)((struct _tuple6**)Cyc__genarr_464
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_225;struct _tuple55{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_456={offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_457={offsetof(struct _tuple55,f2),(void*)& Cyc__genrep_159};
static struct _tuple6 Cyc__gentuple_458={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_226};
static struct _tuple6*Cyc__genarr_459[3]={& Cyc__gentuple_456,& Cyc__gentuple_457,&
Cyc__gentuple_458};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_225={4,
sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_459),(void*)((
struct _tuple6**)Cyc__genarr_459),(void*)((struct _tuple6**)Cyc__genarr_459 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_181;struct _tuple56{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;int f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_219={offsetof(struct _tuple56,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_220={offsetof(struct _tuple56,f2),(
void*)& Cyc__genrep_182};static struct _tuple6 Cyc__gentuple_221={offsetof(struct
_tuple56,f3),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_222={
offsetof(struct _tuple56,f4),(void*)((void*)& Cyc__genrep_62)};static struct _tuple6
Cyc__gentuple_223={offsetof(struct _tuple56,f5),(void*)& Cyc__genrep_159};static
struct _tuple6*Cyc__genarr_224[5]={& Cyc__gentuple_219,& Cyc__gentuple_220,& Cyc__gentuple_221,&
Cyc__gentuple_222,& Cyc__gentuple_223};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_181={
4,sizeof(struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_224),(void*)((
struct _tuple6**)Cyc__genarr_224),(void*)((struct _tuple6**)Cyc__genarr_224 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_167;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_forarray_info_t_rep;static char _tmp2E8[13]="ForArrayInfo";static struct
_tagged_arr Cyc__genname_177={_tmp2E8,_tmp2E8,_tmp2E8 + 13};static char _tmp2E9[6]="defns";
static struct _tuple5 Cyc__gentuple_172={offsetof(struct Cyc_Absyn_ForArrayInfo,defns),{
_tmp2E9,_tmp2E9,_tmp2E9 + 6},(void*)& Cyc__genrep_129};static char _tmp2EA[10]="condition";
static struct _tuple5 Cyc__gentuple_173={offsetof(struct Cyc_Absyn_ForArrayInfo,condition),{
_tmp2EA,_tmp2EA,_tmp2EA + 10},(void*)& Cyc__genrep_168};static char _tmp2EB[6]="delta";
static struct _tuple5 Cyc__gentuple_174={offsetof(struct Cyc_Absyn_ForArrayInfo,delta),{
_tmp2EB,_tmp2EB,_tmp2EB + 6},(void*)& Cyc__genrep_168};static char _tmp2EC[5]="body";
static struct _tuple5 Cyc__gentuple_175={offsetof(struct Cyc_Absyn_ForArrayInfo,body),{
_tmp2EC,_tmp2EC,_tmp2EC + 5},(void*)& Cyc__genrep_159};static struct _tuple5*Cyc__genarr_176[
4]={& Cyc__gentuple_172,& Cyc__gentuple_173,& Cyc__gentuple_174,& Cyc__gentuple_175};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_forarray_info_t_rep={3,(struct
_tagged_arr*)& Cyc__genname_177,sizeof(struct Cyc_Absyn_ForArrayInfo),{(void*)((
struct _tuple5**)Cyc__genarr_176),(void*)((struct _tuple5**)Cyc__genarr_176),(void*)((
struct _tuple5**)Cyc__genarr_176 + 4)}};struct _tuple57{unsigned int f1;struct Cyc_Absyn_ForArrayInfo
f2;};static struct _tuple6 Cyc__gentuple_178={offsetof(struct _tuple57,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_179={offsetof(struct _tuple57,f2),(
void*)& Cyc_Absyn_forarray_info_t_rep};static struct _tuple6*Cyc__genarr_180[2]={&
Cyc__gentuple_178,& Cyc__gentuple_179};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_167={
4,sizeof(struct _tuple57),{(void*)((struct _tuple6**)Cyc__genarr_180),(void*)((
struct _tuple6**)Cyc__genarr_180),(void*)((struct _tuple6**)Cyc__genarr_180 + 2)}};
static char _tmp2EF[7]="Skip_s";static struct _tuple7 Cyc__gentuple_165={0,{_tmp2EF,
_tmp2EF,_tmp2EF + 7}};static struct _tuple7*Cyc__genarr_166[1]={& Cyc__gentuple_165};
static char _tmp2F0[6]="Exp_s";static struct _tuple5 Cyc__gentuple_548={0,{_tmp2F0,
_tmp2F0,_tmp2F0 + 6},(void*)& Cyc__genrep_80};static char _tmp2F1[6]="Seq_s";static
struct _tuple5 Cyc__gentuple_549={1,{_tmp2F1,_tmp2F1,_tmp2F1 + 6},(void*)& Cyc__genrep_543};
static char _tmp2F2[9]="Return_s";static struct _tuple5 Cyc__gentuple_550={2,{_tmp2F2,
_tmp2F2,_tmp2F2 + 9},(void*)& Cyc__genrep_539};static char _tmp2F3[13]="IfThenElse_s";
static struct _tuple5 Cyc__gentuple_551={3,{_tmp2F3,_tmp2F3,_tmp2F3 + 13},(void*)&
Cyc__genrep_533};static char _tmp2F4[8]="While_s";static struct _tuple5 Cyc__gentuple_552={
4,{_tmp2F4,_tmp2F4,_tmp2F4 + 8},(void*)& Cyc__genrep_528};static char _tmp2F5[8]="Break_s";
static struct _tuple5 Cyc__gentuple_553={5,{_tmp2F5,_tmp2F5,_tmp2F5 + 8},(void*)& Cyc__genrep_524};
static char _tmp2F6[11]="Continue_s";static struct _tuple5 Cyc__gentuple_554={6,{
_tmp2F6,_tmp2F6,_tmp2F6 + 11},(void*)& Cyc__genrep_524};static char _tmp2F7[7]="Goto_s";
static struct _tuple5 Cyc__gentuple_555={7,{_tmp2F7,_tmp2F7,_tmp2F7 + 7},(void*)& Cyc__genrep_518};
static char _tmp2F8[6]="For_s";static struct _tuple5 Cyc__gentuple_556={8,{_tmp2F8,
_tmp2F8,_tmp2F8 + 6},(void*)& Cyc__genrep_511};static char _tmp2F9[9]="Switch_s";
static struct _tuple5 Cyc__gentuple_557={9,{_tmp2F9,_tmp2F9,_tmp2F9 + 9},(void*)& Cyc__genrep_506};
static char _tmp2FA[10]="SwitchC_s";static struct _tuple5 Cyc__gentuple_558={10,{
_tmp2FA,_tmp2FA,_tmp2FA + 10},(void*)& Cyc__genrep_490};static char _tmp2FB[11]="Fallthru_s";
static struct _tuple5 Cyc__gentuple_559={11,{_tmp2FB,_tmp2FB,_tmp2FB + 11},(void*)&
Cyc__genrep_479};static char _tmp2FC[7]="Decl_s";static struct _tuple5 Cyc__gentuple_560={
12,{_tmp2FC,_tmp2FC,_tmp2FC + 7},(void*)& Cyc__genrep_474};static char _tmp2FD[6]="Cut_s";
static struct _tuple5 Cyc__gentuple_561={13,{_tmp2FD,_tmp2FD,_tmp2FD + 6},(void*)&
Cyc__genrep_470};static char _tmp2FE[9]="Splice_s";static struct _tuple5 Cyc__gentuple_562={
14,{_tmp2FE,_tmp2FE,_tmp2FE + 9},(void*)& Cyc__genrep_470};static char _tmp2FF[8]="Label_s";
static struct _tuple5 Cyc__gentuple_563={15,{_tmp2FF,_tmp2FF,_tmp2FF + 8},(void*)&
Cyc__genrep_465};static char _tmp300[5]="Do_s";static struct _tuple5 Cyc__gentuple_564={
16,{_tmp300,_tmp300,_tmp300 + 5},(void*)& Cyc__genrep_460};static char _tmp301[11]="TryCatch_s";
static struct _tuple5 Cyc__gentuple_565={17,{_tmp301,_tmp301,_tmp301 + 11},(void*)&
Cyc__genrep_225};static char _tmp302[9]="Region_s";static struct _tuple5 Cyc__gentuple_566={
18,{_tmp302,_tmp302,_tmp302 + 9},(void*)& Cyc__genrep_181};static char _tmp303[11]="ForArray_s";
static struct _tuple5 Cyc__gentuple_567={19,{_tmp303,_tmp303,_tmp303 + 11},(void*)&
Cyc__genrep_167};static char _tmp304[14]="ResetRegion_s";static struct _tuple5 Cyc__gentuple_568={
20,{_tmp304,_tmp304,_tmp304 + 14},(void*)& Cyc__genrep_80};static struct _tuple5*Cyc__genarr_569[
21]={& Cyc__gentuple_548,& Cyc__gentuple_549,& Cyc__gentuple_550,& Cyc__gentuple_551,&
Cyc__gentuple_552,& Cyc__gentuple_553,& Cyc__gentuple_554,& Cyc__gentuple_555,& Cyc__gentuple_556,&
Cyc__gentuple_557,& Cyc__gentuple_558,& Cyc__gentuple_559,& Cyc__gentuple_560,& Cyc__gentuple_561,&
Cyc__gentuple_562,& Cyc__gentuple_563,& Cyc__gentuple_564,& Cyc__gentuple_565,& Cyc__gentuple_566,&
Cyc__gentuple_567,& Cyc__gentuple_568};static char _tmp306[9]="Raw_stmt";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_stmt_t_rep={5,{_tmp306,_tmp306,_tmp306 + 9},{(void*)((struct _tuple7**)
Cyc__genarr_166),(void*)((struct _tuple7**)Cyc__genarr_166),(void*)((struct
_tuple7**)Cyc__genarr_166 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_569),(void*)((
struct _tuple5**)Cyc__genarr_569),(void*)((struct _tuple5**)Cyc__genarr_569 + 21)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_160;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_stmt_t46H2_rep;static char _tmp307[5]="List";static
struct _tagged_arr Cyc__genname_164={_tmp307,_tmp307,_tmp307 + 5};static char _tmp308[
3]="hd";static struct _tuple5 Cyc__gentuple_161={offsetof(struct Cyc_List_List,hd),{
_tmp308,_tmp308,_tmp308 + 3},(void*)& Cyc__genrep_159};static char _tmp309[3]="tl";
static struct _tuple5 Cyc__gentuple_162={offsetof(struct Cyc_List_List,tl),{_tmp309,
_tmp309,_tmp309 + 3},(void*)& Cyc__genrep_160};static struct _tuple5*Cyc__genarr_163[
2]={& Cyc__gentuple_161,& Cyc__gentuple_162};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_164,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_163),(void*)((struct _tuple5**)Cyc__genarr_163),(void*)((
struct _tuple5**)Cyc__genarr_163 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_160={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple8*Cyc__genarr_78[0]={};static char
_tmp30D[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
7,{_tmp30D,_tmp30D,_tmp30D + 11},{(void*)((struct _tuple8**)Cyc__genarr_78),(void*)((
struct _tuple8**)Cyc__genarr_78),(void*)((struct _tuple8**)Cyc__genarr_78 + 0)}};
static char _tmp30E[5]="Stmt";static struct _tagged_arr Cyc__genname_576={_tmp30E,
_tmp30E,_tmp30E + 5};static char _tmp30F[2]="r";static struct _tuple5 Cyc__gentuple_570={
offsetof(struct Cyc_Absyn_Stmt,r),{_tmp30F,_tmp30F,_tmp30F + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};
static char _tmp310[4]="loc";static struct _tuple5 Cyc__gentuple_571={offsetof(struct
Cyc_Absyn_Stmt,loc),{_tmp310,_tmp310,_tmp310 + 4},(void*)& Cyc__genrep_2};static
char _tmp311[16]="non_local_preds";static struct _tuple5 Cyc__gentuple_572={
offsetof(struct Cyc_Absyn_Stmt,non_local_preds),{_tmp311,_tmp311,_tmp311 + 16},(
void*)& Cyc__genrep_160};static char _tmp312[10]="try_depth";static struct _tuple5 Cyc__gentuple_573={
offsetof(struct Cyc_Absyn_Stmt,try_depth),{_tmp312,_tmp312,_tmp312 + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp313[6]="annot";static struct _tuple5 Cyc__gentuple_574={
offsetof(struct Cyc_Absyn_Stmt,annot),{_tmp313,_tmp313,_tmp313 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};
static struct _tuple5*Cyc__genarr_575[5]={& Cyc__gentuple_570,& Cyc__gentuple_571,&
Cyc__gentuple_572,& Cyc__gentuple_573,& Cyc__gentuple_574};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep={3,(struct _tagged_arr*)& Cyc__genname_576,sizeof(struct
Cyc_Absyn_Stmt),{(void*)((struct _tuple5**)Cyc__genarr_575),(void*)((struct
_tuple5**)Cyc__genarr_575),(void*)((struct _tuple5**)Cyc__genarr_575 + 5)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159={1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};
static char _tmp316[7]="Fndecl";static struct _tagged_arr Cyc__genname_614={_tmp316,
_tmp316,_tmp316 + 7};static char _tmp317[3]="sc";static struct _tuple5 Cyc__gentuple_599={
offsetof(struct Cyc_Absyn_Fndecl,sc),{_tmp317,_tmp317,_tmp317 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp318[10]="is_inline";static struct _tuple5 Cyc__gentuple_600={
offsetof(struct Cyc_Absyn_Fndecl,is_inline),{_tmp318,_tmp318,_tmp318 + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp319[5]="name";static struct _tuple5 Cyc__gentuple_601={
offsetof(struct Cyc_Absyn_Fndecl,name),{_tmp319,_tmp319,_tmp319 + 5},(void*)& Cyc__genrep_10};
static char _tmp31A[4]="tvs";static struct _tuple5 Cyc__gentuple_602={offsetof(struct
Cyc_Absyn_Fndecl,tvs),{_tmp31A,_tmp31A,_tmp31A + 4},(void*)& Cyc__genrep_294};
static char _tmp31B[7]="effect";static struct _tuple5 Cyc__gentuple_603={offsetof(
struct Cyc_Absyn_Fndecl,effect),{_tmp31B,_tmp31B,_tmp31B + 7},(void*)& Cyc__genrep_43};
static char _tmp31C[9]="ret_type";static struct _tuple5 Cyc__gentuple_604={offsetof(
struct Cyc_Absyn_Fndecl,ret_type),{_tmp31C,_tmp31C,_tmp31C + 9},(void*)((void*)&
Cyc_Absyn_type_t_rep)};static char _tmp31D[5]="args";static struct _tuple5 Cyc__gentuple_605={
offsetof(struct Cyc_Absyn_Fndecl,args),{_tmp31D,_tmp31D,_tmp31D + 5},(void*)& Cyc__genrep_588};
static char _tmp31E[10]="c_varargs";static struct _tuple5 Cyc__gentuple_606={
offsetof(struct Cyc_Absyn_Fndecl,c_varargs),{_tmp31E,_tmp31E,_tmp31E + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp31F[12]="cyc_varargs";static struct _tuple5
Cyc__gentuple_607={offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),{_tmp31F,_tmp31F,
_tmp31F + 12},(void*)& Cyc__genrep_577};static char _tmp320[7]="rgn_po";static struct
_tuple5 Cyc__gentuple_608={offsetof(struct Cyc_Absyn_Fndecl,rgn_po),{_tmp320,
_tmp320,_tmp320 + 7},(void*)& Cyc__genrep_353};static char _tmp321[5]="body";static
struct _tuple5 Cyc__gentuple_609={offsetof(struct Cyc_Absyn_Fndecl,body),{_tmp321,
_tmp321,_tmp321 + 5},(void*)& Cyc__genrep_159};static char _tmp322[11]="cached_typ";
static struct _tuple5 Cyc__gentuple_610={offsetof(struct Cyc_Absyn_Fndecl,cached_typ),{
_tmp322,_tmp322,_tmp322 + 11},(void*)& Cyc__genrep_43};static char _tmp323[15]="param_vardecls";
static struct _tuple5 Cyc__gentuple_611={offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),{
_tmp323,_tmp323,_tmp323 + 15},(void*)& Cyc__genrep_128};static char _tmp324[11]="attributes";
static struct _tuple5 Cyc__gentuple_612={offsetof(struct Cyc_Absyn_Fndecl,attributes),{
_tmp324,_tmp324,_tmp324 + 11},(void*)& Cyc__genrep_87};static struct _tuple5*Cyc__genarr_613[
14]={& Cyc__gentuple_599,& Cyc__gentuple_600,& Cyc__gentuple_601,& Cyc__gentuple_602,&
Cyc__gentuple_603,& Cyc__gentuple_604,& Cyc__gentuple_605,& Cyc__gentuple_606,& Cyc__gentuple_607,&
Cyc__gentuple_608,& Cyc__gentuple_609,& Cyc__gentuple_610,& Cyc__gentuple_611,& Cyc__gentuple_612};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep={3,(struct _tagged_arr*)&
Cyc__genname_614,sizeof(struct Cyc_Absyn_Fndecl),{(void*)((struct _tuple5**)Cyc__genarr_613),(
void*)((struct _tuple5**)Cyc__genarr_613),(void*)((struct _tuple5**)Cyc__genarr_613
+ 14)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_86={1,1,(void*)((void*)&
Cyc_struct_Absyn_Fndecl_rep)};struct _tuple58{unsigned int f1;struct Cyc_Absyn_Fndecl*
f2;};static struct _tuple6 Cyc__gentuple_615={offsetof(struct _tuple58,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_616={offsetof(struct _tuple58,f2),(
void*)& Cyc__genrep_86};static struct _tuple6*Cyc__genarr_617[2]={& Cyc__gentuple_615,&
Cyc__gentuple_616};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_85={4,sizeof(
struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_617),(void*)((struct
_tuple6**)Cyc__genarr_617),(void*)((struct _tuple6**)Cyc__genarr_617 + 2)}};static
char _tmp328[13]="Unresolved_b";static struct _tuple7 Cyc__gentuple_823={0,{_tmp328,
_tmp328,_tmp328 + 13}};static struct _tuple7*Cyc__genarr_824[1]={& Cyc__gentuple_823};
static char _tmp329[9]="Global_b";static struct _tuple5 Cyc__gentuple_825={0,{_tmp329,
_tmp329,_tmp329 + 9},(void*)& Cyc__genrep_400};static char _tmp32A[10]="Funname_b";
static struct _tuple5 Cyc__gentuple_826={1,{_tmp32A,_tmp32A,_tmp32A + 10},(void*)&
Cyc__genrep_85};static char _tmp32B[8]="Param_b";static struct _tuple5 Cyc__gentuple_827={
2,{_tmp32B,_tmp32B,_tmp32B + 8},(void*)& Cyc__genrep_400};static char _tmp32C[8]="Local_b";
static struct _tuple5 Cyc__gentuple_828={3,{_tmp32C,_tmp32C,_tmp32C + 8},(void*)& Cyc__genrep_400};
static char _tmp32D[6]="Pat_b";static struct _tuple5 Cyc__gentuple_829={4,{_tmp32D,
_tmp32D,_tmp32D + 6},(void*)& Cyc__genrep_400};static struct _tuple5*Cyc__genarr_830[
5]={& Cyc__gentuple_825,& Cyc__gentuple_826,& Cyc__gentuple_827,& Cyc__gentuple_828,&
Cyc__gentuple_829};static char _tmp32F[8]="Binding";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep={5,{_tmp32F,_tmp32F,_tmp32F + 8},{(void*)((struct _tuple7**)
Cyc__genarr_824),(void*)((struct _tuple7**)Cyc__genarr_824),(void*)((struct
_tuple7**)Cyc__genarr_824 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_830),(void*)((
struct _tuple5**)Cyc__genarr_830),(void*)((struct _tuple5**)Cyc__genarr_830 + 5)}};
struct _tuple59{unsigned int f1;struct _tuple0*f2;void*f3;};static struct _tuple6 Cyc__gentuple_831={
offsetof(struct _tuple59,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_832={
offsetof(struct _tuple59,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_833={
offsetof(struct _tuple59,f3),(void*)& Cyc_Absyn_binding_t_rep};static struct _tuple6*
Cyc__genarr_834[3]={& Cyc__gentuple_831,& Cyc__gentuple_832,& Cyc__gentuple_833};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_822={4,sizeof(struct _tuple59),{(
void*)((struct _tuple6**)Cyc__genarr_834),(void*)((struct _tuple6**)Cyc__genarr_834),(
void*)((struct _tuple6**)Cyc__genarr_834 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_817;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep;
static char _tmp331[5]="Plus";static struct _tuple7 Cyc__gentuple_787={0,{_tmp331,
_tmp331,_tmp331 + 5}};static char _tmp332[6]="Times";static struct _tuple7 Cyc__gentuple_788={
1,{_tmp332,_tmp332,_tmp332 + 6}};static char _tmp333[6]="Minus";static struct _tuple7
Cyc__gentuple_789={2,{_tmp333,_tmp333,_tmp333 + 6}};static char _tmp334[4]="Div";
static struct _tuple7 Cyc__gentuple_790={3,{_tmp334,_tmp334,_tmp334 + 4}};static char
_tmp335[4]="Mod";static struct _tuple7 Cyc__gentuple_791={4,{_tmp335,_tmp335,
_tmp335 + 4}};static char _tmp336[3]="Eq";static struct _tuple7 Cyc__gentuple_792={5,{
_tmp336,_tmp336,_tmp336 + 3}};static char _tmp337[4]="Neq";static struct _tuple7 Cyc__gentuple_793={
6,{_tmp337,_tmp337,_tmp337 + 4}};static char _tmp338[3]="Gt";static struct _tuple7 Cyc__gentuple_794={
7,{_tmp338,_tmp338,_tmp338 + 3}};static char _tmp339[3]="Lt";static struct _tuple7 Cyc__gentuple_795={
8,{_tmp339,_tmp339,_tmp339 + 3}};static char _tmp33A[4]="Gte";static struct _tuple7
Cyc__gentuple_796={9,{_tmp33A,_tmp33A,_tmp33A + 4}};static char _tmp33B[4]="Lte";
static struct _tuple7 Cyc__gentuple_797={10,{_tmp33B,_tmp33B,_tmp33B + 4}};static
char _tmp33C[4]="Not";static struct _tuple7 Cyc__gentuple_798={11,{_tmp33C,_tmp33C,
_tmp33C + 4}};static char _tmp33D[7]="Bitnot";static struct _tuple7 Cyc__gentuple_799={
12,{_tmp33D,_tmp33D,_tmp33D + 7}};static char _tmp33E[7]="Bitand";static struct
_tuple7 Cyc__gentuple_800={13,{_tmp33E,_tmp33E,_tmp33E + 7}};static char _tmp33F[6]="Bitor";
static struct _tuple7 Cyc__gentuple_801={14,{_tmp33F,_tmp33F,_tmp33F + 6}};static
char _tmp340[7]="Bitxor";static struct _tuple7 Cyc__gentuple_802={15,{_tmp340,
_tmp340,_tmp340 + 7}};static char _tmp341[10]="Bitlshift";static struct _tuple7 Cyc__gentuple_803={
16,{_tmp341,_tmp341,_tmp341 + 10}};static char _tmp342[11]="Bitlrshift";static
struct _tuple7 Cyc__gentuple_804={17,{_tmp342,_tmp342,_tmp342 + 11}};static char
_tmp343[11]="Bitarshift";static struct _tuple7 Cyc__gentuple_805={18,{_tmp343,
_tmp343,_tmp343 + 11}};static char _tmp344[5]="Size";static struct _tuple7 Cyc__gentuple_806={
19,{_tmp344,_tmp344,_tmp344 + 5}};static struct _tuple7*Cyc__genarr_807[20]={& Cyc__gentuple_787,&
Cyc__gentuple_788,& Cyc__gentuple_789,& Cyc__gentuple_790,& Cyc__gentuple_791,& Cyc__gentuple_792,&
Cyc__gentuple_793,& Cyc__gentuple_794,& Cyc__gentuple_795,& Cyc__gentuple_796,& Cyc__gentuple_797,&
Cyc__gentuple_798,& Cyc__gentuple_799,& Cyc__gentuple_800,& Cyc__gentuple_801,& Cyc__gentuple_802,&
Cyc__gentuple_803,& Cyc__gentuple_804,& Cyc__gentuple_805,& Cyc__gentuple_806};
static struct _tuple5*Cyc__genarr_808[0]={};static char _tmp346[7]="Primop";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={5,{_tmp346,_tmp346,_tmp346 + 7},{(
void*)((struct _tuple7**)Cyc__genarr_807),(void*)((struct _tuple7**)Cyc__genarr_807),(
void*)((struct _tuple7**)Cyc__genarr_807 + 20)},{(void*)((struct _tuple5**)Cyc__genarr_808),(
void*)((struct _tuple5**)Cyc__genarr_808),(void*)((struct _tuple5**)Cyc__genarr_808
+ 0)}};struct _tuple60{unsigned int f1;void*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_818={offsetof(struct _tuple60,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_819={offsetof(struct _tuple60,f2),(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple6 Cyc__gentuple_820={offsetof(struct _tuple60,f3),(void*)& Cyc__genrep_481};
static struct _tuple6*Cyc__genarr_821[3]={& Cyc__gentuple_818,& Cyc__gentuple_819,&
Cyc__gentuple_820};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_817={4,
sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_821),(void*)((
struct _tuple6**)Cyc__genarr_821),(void*)((struct _tuple6**)Cyc__genarr_821 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_785;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_786;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;
static char _tmp348[4]="Opt";static struct _tagged_arr Cyc__genname_811={_tmp348,
_tmp348,_tmp348 + 4};static char _tmp349[2]="v";static struct _tuple5 Cyc__gentuple_809={
offsetof(struct Cyc_Core_Opt,v),{_tmp349,_tmp349,_tmp349 + 2},(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple5*Cyc__genarr_810[1]={& Cyc__gentuple_809};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_primop_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_811,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_810),(void*)((
struct _tuple5**)Cyc__genarr_810),(void*)((struct _tuple5**)Cyc__genarr_810 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_786={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple61{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_812={offsetof(struct
_tuple61,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_813={
offsetof(struct _tuple61,f2),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_814={
offsetof(struct _tuple61,f3),(void*)& Cyc__genrep_786};static struct _tuple6 Cyc__gentuple_815={
offsetof(struct _tuple61,f4),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_816[
4]={& Cyc__gentuple_812,& Cyc__gentuple_813,& Cyc__gentuple_814,& Cyc__gentuple_815};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_785={4,sizeof(struct _tuple61),{(
void*)((struct _tuple6**)Cyc__genarr_816),(void*)((struct _tuple6**)Cyc__genarr_816),(
void*)((struct _tuple6**)Cyc__genarr_816 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_774;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep;
static char _tmp34D[7]="PreInc";static struct _tuple7 Cyc__gentuple_775={0,{_tmp34D,
_tmp34D,_tmp34D + 7}};static char _tmp34E[8]="PostInc";static struct _tuple7 Cyc__gentuple_776={
1,{_tmp34E,_tmp34E,_tmp34E + 8}};static char _tmp34F[7]="PreDec";static struct
_tuple7 Cyc__gentuple_777={2,{_tmp34F,_tmp34F,_tmp34F + 7}};static char _tmp350[8]="PostDec";
static struct _tuple7 Cyc__gentuple_778={3,{_tmp350,_tmp350,_tmp350 + 8}};static
struct _tuple7*Cyc__genarr_779[4]={& Cyc__gentuple_775,& Cyc__gentuple_776,& Cyc__gentuple_777,&
Cyc__gentuple_778};static struct _tuple5*Cyc__genarr_780[0]={};static char _tmp352[
12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={5,{
_tmp352,_tmp352,_tmp352 + 12},{(void*)((struct _tuple7**)Cyc__genarr_779),(void*)((
struct _tuple7**)Cyc__genarr_779),(void*)((struct _tuple7**)Cyc__genarr_779 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_780),(void*)((struct _tuple5**)Cyc__genarr_780),(
void*)((struct _tuple5**)Cyc__genarr_780 + 0)}};struct _tuple62{unsigned int f1;
struct Cyc_Absyn_Exp*f2;void*f3;};static struct _tuple6 Cyc__gentuple_781={offsetof(
struct _tuple62,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_782={
offsetof(struct _tuple62,f2),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_783={
offsetof(struct _tuple62,f3),(void*)& Cyc_Absyn_incrementor_t_rep};static struct
_tuple6*Cyc__genarr_784[3]={& Cyc__gentuple_781,& Cyc__gentuple_782,& Cyc__gentuple_783};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_774={4,sizeof(struct _tuple62),{(
void*)((struct _tuple6**)Cyc__genarr_784),(void*)((struct _tuple6**)Cyc__genarr_784),(
void*)((struct _tuple6**)Cyc__genarr_784 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_768;struct _tuple63{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_769={offsetof(
struct _tuple63,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_770={
offsetof(struct _tuple63,f2),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_771={
offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_772={
offsetof(struct _tuple63,f4),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_773[
4]={& Cyc__gentuple_769,& Cyc__gentuple_770,& Cyc__gentuple_771,& Cyc__gentuple_772};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_768={4,sizeof(struct _tuple63),{(
void*)((struct _tuple6**)Cyc__genarr_773),(void*)((struct _tuple6**)Cyc__genarr_773),(
void*)((struct _tuple6**)Cyc__genarr_773 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_707;struct _tuple64{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*
f3;};static struct _tuple6 Cyc__gentuple_708={offsetof(struct _tuple64,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_709={offsetof(struct _tuple64,f2),(
void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_710={offsetof(struct
_tuple64,f3),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_711[3]={&
Cyc__gentuple_708,& Cyc__gentuple_709,& Cyc__gentuple_710};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_707={4,sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_711),(
void*)((struct _tuple6**)Cyc__genarr_711),(void*)((struct _tuple6**)Cyc__genarr_711
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_763;static struct _tuple6
Cyc__gentuple_764={offsetof(struct _tuple51,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_765={offsetof(struct _tuple51,f2),(void*)& Cyc__genrep_81};
static struct _tuple6 Cyc__gentuple_766={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_481};
static struct _tuple6*Cyc__genarr_767[3]={& Cyc__gentuple_764,& Cyc__gentuple_765,&
Cyc__gentuple_766};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_763={4,
sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_767),(void*)((
struct _tuple6**)Cyc__genarr_767),(void*)((struct _tuple6**)Cyc__genarr_767 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_750;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_751;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_752;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_752={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};static char
_tmp358[15]="VarargCallInfo";static struct _tagged_arr Cyc__genname_757={_tmp358,
_tmp358,_tmp358 + 15};static char _tmp359[12]="num_varargs";static struct _tuple5 Cyc__gentuple_753={
offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),{_tmp359,_tmp359,_tmp359 + 
12},(void*)((void*)& Cyc__genrep_62)};static char _tmp35A[10]="injectors";static
struct _tuple5 Cyc__gentuple_754={offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),{
_tmp35A,_tmp35A,_tmp35A + 10},(void*)& Cyc__genrep_286};static char _tmp35B[4]="vai";
static struct _tuple5 Cyc__gentuple_755={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),{
_tmp35B,_tmp35B,_tmp35B + 4},(void*)& Cyc__genrep_752};static struct _tuple5*Cyc__genarr_756[
3]={& Cyc__gentuple_753,& Cyc__gentuple_754,& Cyc__gentuple_755};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_call_info_t_rep={3,(struct _tagged_arr*)& Cyc__genname_757,
sizeof(struct Cyc_Absyn_VarargCallInfo),{(void*)((struct _tuple5**)Cyc__genarr_756),(
void*)((struct _tuple5**)Cyc__genarr_756),(void*)((struct _tuple5**)Cyc__genarr_756
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_751={1,1,(void*)((void*)&
Cyc_Absyn_vararg_call_info_t_rep)};struct _tuple65{unsigned int f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_VarargCallInfo*f4;};static struct
_tuple6 Cyc__gentuple_758={offsetof(struct _tuple65,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_759={offsetof(struct _tuple65,f2),(void*)& Cyc__genrep_81};
static struct _tuple6 Cyc__gentuple_760={offsetof(struct _tuple65,f3),(void*)& Cyc__genrep_481};
static struct _tuple6 Cyc__gentuple_761={offsetof(struct _tuple65,f4),(void*)& Cyc__genrep_751};
static struct _tuple6*Cyc__genarr_762[4]={& Cyc__gentuple_758,& Cyc__gentuple_759,&
Cyc__gentuple_760,& Cyc__gentuple_761};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_750={
4,sizeof(struct _tuple65),{(void*)((struct _tuple6**)Cyc__genarr_762),(void*)((
struct _tuple6**)Cyc__genarr_762),(void*)((struct _tuple6**)Cyc__genarr_762 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_745;static struct _tuple6 Cyc__gentuple_746={
offsetof(struct _tuple51,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_747={
offsetof(struct _tuple51,f2),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_748={
offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_53};static struct _tuple6*Cyc__genarr_749[
3]={& Cyc__gentuple_746,& Cyc__gentuple_747,& Cyc__gentuple_748};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_745={4,sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_749),(
void*)((struct _tuple6**)Cyc__genarr_749),(void*)((struct _tuple6**)Cyc__genarr_749
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_740;struct _tuple66{
unsigned int f1;void*f2;struct Cyc_Absyn_Exp*f3;};static struct _tuple6 Cyc__gentuple_741={
offsetof(struct _tuple66,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_742={
offsetof(struct _tuple66,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_743={offsetof(struct _tuple66,f3),(void*)& Cyc__genrep_81};
static struct _tuple6*Cyc__genarr_744[3]={& Cyc__gentuple_741,& Cyc__gentuple_742,&
Cyc__gentuple_743};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_740={4,
sizeof(struct _tuple66),{(void*)((struct _tuple6**)Cyc__genarr_744),(void*)((
struct _tuple6**)Cyc__genarr_744),(void*)((struct _tuple6**)Cyc__genarr_744 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_735;static struct _tuple6 Cyc__gentuple_736={
offsetof(struct _tuple64,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_737={
offsetof(struct _tuple64,f2),(void*)& Cyc__genrep_77};static struct _tuple6 Cyc__gentuple_738={
offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_739[
3]={& Cyc__gentuple_736,& Cyc__gentuple_737,& Cyc__gentuple_738};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_735={4,sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_739),(
void*)((struct _tuple6**)Cyc__genarr_739),(void*)((struct _tuple6**)Cyc__genarr_739
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_48;static struct _tuple6 Cyc__gentuple_49={
offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_50={
offsetof(struct _tuple6,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_51[2]={& Cyc__gentuple_49,& Cyc__gentuple_50};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_48={4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_51),(
void*)((struct _tuple6**)Cyc__genarr_51),(void*)((struct _tuple6**)Cyc__genarr_51 + 
2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_722;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_724;
struct _tuple67{unsigned int f1;unsigned int f2;};static struct _tuple6 Cyc__gentuple_725={
offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_726={
offsetof(struct _tuple67,f2),(void*)& Cyc__genrep_5};static struct _tuple6*Cyc__genarr_727[
2]={& Cyc__gentuple_725,& Cyc__gentuple_726};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_724={4,sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_727),(
void*)((struct _tuple6**)Cyc__genarr_727),(void*)((struct _tuple6**)Cyc__genarr_727
+ 2)}};static struct _tuple7*Cyc__genarr_723[0]={};static char _tmp364[12]="StructField";
static struct _tuple5 Cyc__gentuple_728={0,{_tmp364,_tmp364,_tmp364 + 12},(void*)&
Cyc__genrep_317};static char _tmp365[11]="TupleIndex";static struct _tuple5 Cyc__gentuple_729={
1,{_tmp365,_tmp365,_tmp365 + 11},(void*)& Cyc__genrep_724};static struct _tuple5*Cyc__genarr_730[
2]={& Cyc__gentuple_728,& Cyc__gentuple_729};static char _tmp367[14]="OffsetofField";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep={5,{_tmp367,
_tmp367,_tmp367 + 14},{(void*)((struct _tuple7**)Cyc__genarr_723),(void*)((struct
_tuple7**)Cyc__genarr_723),(void*)((struct _tuple7**)Cyc__genarr_723 + 0)},{(void*)((
struct _tuple5**)Cyc__genarr_730),(void*)((struct _tuple5**)Cyc__genarr_730),(void*)((
struct _tuple5**)Cyc__genarr_730 + 2)}};struct _tuple68{unsigned int f1;void*f2;void*
f3;};static struct _tuple6 Cyc__gentuple_731={offsetof(struct _tuple68,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_732={offsetof(struct _tuple68,f2),(
void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_733={
offsetof(struct _tuple68,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};static struct
_tuple6*Cyc__genarr_734[3]={& Cyc__gentuple_731,& Cyc__gentuple_732,& Cyc__gentuple_733};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_722={4,sizeof(struct _tuple68),{(
void*)((struct _tuple6**)Cyc__genarr_734),(void*)((struct _tuple6**)Cyc__genarr_734),(
void*)((struct _tuple6**)Cyc__genarr_734 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_717;struct _tuple69{unsigned int f1;struct Cyc_List_List*f2;void*f3;};
static struct _tuple6 Cyc__gentuple_718={offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_719={offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_294};
static struct _tuple6 Cyc__gentuple_720={offsetof(struct _tuple69,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_721[3]={& Cyc__gentuple_718,&
Cyc__gentuple_719,& Cyc__gentuple_720};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_717={
4,sizeof(struct _tuple69),{(void*)((struct _tuple6**)Cyc__genarr_721),(void*)((
struct _tuple6**)Cyc__genarr_721),(void*)((struct _tuple6**)Cyc__genarr_721 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_712;struct _tuple70{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct _tagged_arr*f3;};static struct _tuple6 Cyc__gentuple_713={
offsetof(struct _tuple70,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_714={
offsetof(struct _tuple70,f2),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_715={
offsetof(struct _tuple70,f3),(void*)& Cyc__genrep_12};static struct _tuple6*Cyc__genarr_716[
3]={& Cyc__gentuple_713,& Cyc__gentuple_714,& Cyc__gentuple_715};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_712={4,sizeof(struct _tuple70),{(void*)((struct _tuple6**)Cyc__genarr_716),(
void*)((struct _tuple6**)Cyc__genarr_716),(void*)((struct _tuple6**)Cyc__genarr_716
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_703;static struct _tuple6
Cyc__gentuple_704={offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_705={offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_481};
static struct _tuple6*Cyc__genarr_706[2]={& Cyc__gentuple_704,& Cyc__gentuple_705};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_703={4,sizeof(struct _tuple13),{(
void*)((struct _tuple6**)Cyc__genarr_706),(void*)((struct _tuple6**)Cyc__genarr_706),(
void*)((struct _tuple6**)Cyc__genarr_706 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_692;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_693;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_694;static struct _tuple6 Cyc__gentuple_695={
offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_578};static struct _tuple6 Cyc__gentuple_696={
offsetof(struct _tuple1,f2),(void*)& Cyc__genrep_131};static struct _tuple6 Cyc__gentuple_697={
offsetof(struct _tuple1,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_698[3]={& Cyc__gentuple_695,& Cyc__gentuple_696,& Cyc__gentuple_697};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_694={4,sizeof(struct _tuple1),{(
void*)((struct _tuple6**)Cyc__genarr_698),(void*)((struct _tuple6**)Cyc__genarr_698),(
void*)((struct _tuple6**)Cyc__genarr_698 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_693={1,1,(void*)((void*)& Cyc__genrep_694)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_619;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_620;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_621;static struct _tuple6 Cyc__gentuple_622={offsetof(struct _tuple12,f1),(
void*)& Cyc__genrep_315};static struct _tuple6 Cyc__gentuple_623={offsetof(struct
_tuple12,f2),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_624[2]={&
Cyc__gentuple_622,& Cyc__gentuple_623};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_621={
4,sizeof(struct _tuple12),{(void*)((struct _tuple6**)Cyc__genarr_624),(void*)((
struct _tuple6**)Cyc__genarr_624),(void*)((struct _tuple6**)Cyc__genarr_624 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_620={1,1,(void*)((void*)& Cyc__genrep_621)};
static char _tmp370[5]="List";static struct _tagged_arr Cyc__genname_628={_tmp370,
_tmp370,_tmp370 + 5};static char _tmp371[3]="hd";static struct _tuple5 Cyc__gentuple_625={
offsetof(struct Cyc_List_List,hd),{_tmp371,_tmp371,_tmp371 + 3},(void*)& Cyc__genrep_620};
static char _tmp372[3]="tl";static struct _tuple5 Cyc__gentuple_626={offsetof(struct
Cyc_List_List,tl),{_tmp372,_tmp372,_tmp372 + 3},(void*)& Cyc__genrep_619};static
struct _tuple5*Cyc__genarr_627[2]={& Cyc__gentuple_625,& Cyc__gentuple_626};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_628,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_627),(void*)((struct _tuple5**)Cyc__genarr_627),(void*)((
struct _tuple5**)Cyc__genarr_627 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_619={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple71{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_699={offsetof(struct _tuple71,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_700={offsetof(struct _tuple71,f2),(void*)& Cyc__genrep_693};
static struct _tuple6 Cyc__gentuple_701={offsetof(struct _tuple71,f3),(void*)& Cyc__genrep_619};
static struct _tuple6*Cyc__genarr_702[3]={& Cyc__gentuple_699,& Cyc__gentuple_700,&
Cyc__gentuple_701};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_692={4,
sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_702),(void*)((
struct _tuple6**)Cyc__genarr_702),(void*)((struct _tuple6**)Cyc__genarr_702 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_688;static struct _tuple6 Cyc__gentuple_689={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_690={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_619};static struct _tuple6*Cyc__genarr_691[
2]={& Cyc__gentuple_689,& Cyc__gentuple_690};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_688={4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_691),(
void*)((struct _tuple6**)Cyc__genarr_691),(void*)((struct _tuple6**)Cyc__genarr_691
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_682;struct _tuple72{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_683={offsetof(struct _tuple72,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_684={offsetof(struct _tuple72,f2),(
void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_685={offsetof(struct
_tuple72,f3),(void*)& Cyc__genrep_81};static struct _tuple6 Cyc__gentuple_686={
offsetof(struct _tuple72,f4),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_687[
4]={& Cyc__gentuple_683,& Cyc__gentuple_684,& Cyc__gentuple_685,& Cyc__gentuple_686};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_682={4,sizeof(struct _tuple72),{(
void*)((struct _tuple6**)Cyc__genarr_687),(void*)((struct _tuple6**)Cyc__genarr_687),(
void*)((struct _tuple6**)Cyc__genarr_687 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_674;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_675;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_675={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};
struct _tuple73{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*
f4;struct Cyc_Absyn_Aggrdecl*f5;};static struct _tuple6 Cyc__gentuple_676={offsetof(
struct _tuple73,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_677={
offsetof(struct _tuple73,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_678={
offsetof(struct _tuple73,f3),(void*)& Cyc__genrep_53};static struct _tuple6 Cyc__gentuple_679={
offsetof(struct _tuple73,f4),(void*)& Cyc__genrep_619};static struct _tuple6 Cyc__gentuple_680={
offsetof(struct _tuple73,f5),(void*)& Cyc__genrep_675};static struct _tuple6*Cyc__genarr_681[
5]={& Cyc__gentuple_676,& Cyc__gentuple_677,& Cyc__gentuple_678,& Cyc__gentuple_679,&
Cyc__gentuple_680};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_674={4,
sizeof(struct _tuple73),{(void*)((struct _tuple6**)Cyc__genarr_681),(void*)((
struct _tuple6**)Cyc__genarr_681),(void*)((struct _tuple6**)Cyc__genarr_681 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_669;static struct _tuple6 Cyc__gentuple_670={
offsetof(struct _tuple60,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_671={
offsetof(struct _tuple60,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_672={offsetof(struct _tuple60,f3),(void*)& Cyc__genrep_619};
static struct _tuple6*Cyc__genarr_673[3]={& Cyc__gentuple_670,& Cyc__gentuple_671,&
Cyc__gentuple_672};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_669={4,
sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_673),(void*)((
struct _tuple6**)Cyc__genarr_673),(void*)((struct _tuple6**)Cyc__genarr_673 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_663;struct _tuple74{unsigned int
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Tuniondecl*f3;struct Cyc_Absyn_Tunionfield*
f4;};static struct _tuple6 Cyc__gentuple_664={offsetof(struct _tuple74,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_665={offsetof(struct _tuple74,f2),(
void*)& Cyc__genrep_481};static struct _tuple6 Cyc__gentuple_666={offsetof(struct
_tuple74,f3),(void*)((void*)& Cyc__genrep_284)};static struct _tuple6 Cyc__gentuple_667={
offsetof(struct _tuple74,f4),(void*)& Cyc__genrep_267};static struct _tuple6*Cyc__genarr_668[
4]={& Cyc__gentuple_664,& Cyc__gentuple_665,& Cyc__gentuple_666,& Cyc__gentuple_667};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_663={4,sizeof(struct _tuple74),{(
void*)((struct _tuple6**)Cyc__genarr_668),(void*)((struct _tuple6**)Cyc__genarr_668),(
void*)((struct _tuple6**)Cyc__genarr_668 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_656;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_657;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_657={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_650;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_650={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple75{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_658={offsetof(struct _tuple75,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_659={offsetof(struct _tuple75,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_660={offsetof(struct
_tuple75,f3),(void*)& Cyc__genrep_657};static struct _tuple6 Cyc__gentuple_661={
offsetof(struct _tuple75,f4),(void*)& Cyc__genrep_650};static struct _tuple6*Cyc__genarr_662[
4]={& Cyc__gentuple_658,& Cyc__gentuple_659,& Cyc__gentuple_660,& Cyc__gentuple_661};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_656={4,sizeof(struct _tuple75),{(
void*)((struct _tuple6**)Cyc__genarr_662),(void*)((struct _tuple6**)Cyc__genarr_662),(
void*)((struct _tuple6**)Cyc__genarr_662 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_649;struct _tuple76{unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_651={offsetof(struct _tuple76,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_652={offsetof(struct _tuple76,f2),(
void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_653={offsetof(struct
_tuple76,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_654={
offsetof(struct _tuple76,f4),(void*)& Cyc__genrep_650};static struct _tuple6*Cyc__genarr_655[
4]={& Cyc__gentuple_651,& Cyc__gentuple_652,& Cyc__gentuple_653,& Cyc__gentuple_654};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_649={4,sizeof(struct _tuple76),{(
void*)((struct _tuple6**)Cyc__genarr_655),(void*)((struct _tuple6**)Cyc__genarr_655),(
void*)((struct _tuple6**)Cyc__genarr_655 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_637;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_638;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_638={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp381[11]="MallocInfo";
static struct _tagged_arr Cyc__genname_645={_tmp381,_tmp381,_tmp381 + 11};static char
_tmp382[10]="is_calloc";static struct _tuple5 Cyc__gentuple_639={offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),{
_tmp382,_tmp382,_tmp382 + 10},(void*)((void*)& Cyc__genrep_62)};static char _tmp383[
4]="rgn";static struct _tuple5 Cyc__gentuple_640={offsetof(struct Cyc_Absyn_MallocInfo,rgn),{
_tmp383,_tmp383,_tmp383 + 4},(void*)& Cyc__genrep_77};static char _tmp384[9]="elt_type";
static struct _tuple5 Cyc__gentuple_641={offsetof(struct Cyc_Absyn_MallocInfo,elt_type),{
_tmp384,_tmp384,_tmp384 + 9},(void*)& Cyc__genrep_638};static char _tmp385[9]="num_elts";
static struct _tuple5 Cyc__gentuple_642={offsetof(struct Cyc_Absyn_MallocInfo,num_elts),{
_tmp385,_tmp385,_tmp385 + 9},(void*)& Cyc__genrep_81};static char _tmp386[11]="fat_result";
static struct _tuple5 Cyc__gentuple_643={offsetof(struct Cyc_Absyn_MallocInfo,fat_result),{
_tmp386,_tmp386,_tmp386 + 11},(void*)((void*)& Cyc__genrep_62)};static struct
_tuple5*Cyc__genarr_644[5]={& Cyc__gentuple_639,& Cyc__gentuple_640,& Cyc__gentuple_641,&
Cyc__gentuple_642,& Cyc__gentuple_643};struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep={
3,(struct _tagged_arr*)& Cyc__genname_645,sizeof(struct Cyc_Absyn_MallocInfo),{(
void*)((struct _tuple5**)Cyc__genarr_644),(void*)((struct _tuple5**)Cyc__genarr_644),(
void*)((struct _tuple5**)Cyc__genarr_644 + 5)}};struct _tuple77{unsigned int f1;
struct Cyc_Absyn_MallocInfo f2;};static struct _tuple6 Cyc__gentuple_646={offsetof(
struct _tuple77,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_647={
offsetof(struct _tuple77,f2),(void*)& Cyc_Absyn_malloc_info_t_rep};static struct
_tuple6*Cyc__genarr_648[2]={& Cyc__gentuple_646,& Cyc__gentuple_647};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_637={4,sizeof(struct _tuple77),{(void*)((
struct _tuple6**)Cyc__genarr_648),(void*)((struct _tuple6**)Cyc__genarr_648),(void*)((
struct _tuple6**)Cyc__genarr_648 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_618;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_629;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;static char _tmp389[4]="Opt";static
struct _tagged_arr Cyc__genname_632={_tmp389,_tmp389,_tmp389 + 4};static char _tmp38A[
2]="v";static struct _tuple5 Cyc__gentuple_630={offsetof(struct Cyc_Core_Opt,v),{
_tmp38A,_tmp38A,_tmp38A + 2},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_631[
1]={& Cyc__gentuple_630};struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={
3,(struct _tagged_arr*)& Cyc__genname_632,sizeof(struct Cyc_Core_Opt),{(void*)((
struct _tuple5**)Cyc__genarr_631),(void*)((struct _tuple5**)Cyc__genarr_631),(void*)((
struct _tuple5**)Cyc__genarr_631 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_629={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};struct
_tuple78{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_633={offsetof(struct _tuple78,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_634={offsetof(struct _tuple78,f2),(void*)& Cyc__genrep_629};
static struct _tuple6 Cyc__gentuple_635={offsetof(struct _tuple78,f3),(void*)& Cyc__genrep_619};
static struct _tuple6*Cyc__genarr_636[3]={& Cyc__gentuple_633,& Cyc__gentuple_634,&
Cyc__gentuple_635};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_618={4,
sizeof(struct _tuple78),{(void*)((struct _tuple6**)Cyc__genarr_636),(void*)((
struct _tuple6**)Cyc__genarr_636),(void*)((struct _tuple6**)Cyc__genarr_636 + 3)}};
static struct _tuple7*Cyc__genarr_79[0]={};static char _tmp38E[8]="Const_e";static
struct _tuple5 Cyc__gentuple_865={0,{_tmp38E,_tmp38E,_tmp38E + 8},(void*)& Cyc__genrep_835};
static char _tmp38F[6]="Var_e";static struct _tuple5 Cyc__gentuple_866={1,{_tmp38F,
_tmp38F,_tmp38F + 6},(void*)& Cyc__genrep_822};static char _tmp390[12]="UnknownId_e";
static struct _tuple5 Cyc__gentuple_867={2,{_tmp390,_tmp390,_tmp390 + 12},(void*)&
Cyc__genrep_242};static char _tmp391[9]="Primop_e";static struct _tuple5 Cyc__gentuple_868={
3,{_tmp391,_tmp391,_tmp391 + 9},(void*)& Cyc__genrep_817};static char _tmp392[11]="AssignOp_e";
static struct _tuple5 Cyc__gentuple_869={4,{_tmp392,_tmp392,_tmp392 + 11},(void*)&
Cyc__genrep_785};static char _tmp393[12]="Increment_e";static struct _tuple5 Cyc__gentuple_870={
5,{_tmp393,_tmp393,_tmp393 + 12},(void*)& Cyc__genrep_774};static char _tmp394[14]="Conditional_e";
static struct _tuple5 Cyc__gentuple_871={6,{_tmp394,_tmp394,_tmp394 + 14},(void*)&
Cyc__genrep_768};static char _tmp395[9]="SeqExp_e";static struct _tuple5 Cyc__gentuple_872={
7,{_tmp395,_tmp395,_tmp395 + 9},(void*)& Cyc__genrep_707};static char _tmp396[14]="UnknownCall_e";
static struct _tuple5 Cyc__gentuple_873={8,{_tmp396,_tmp396,_tmp396 + 14},(void*)&
Cyc__genrep_763};static char _tmp397[9]="FnCall_e";static struct _tuple5 Cyc__gentuple_874={
9,{_tmp397,_tmp397,_tmp397 + 9},(void*)& Cyc__genrep_750};static char _tmp398[8]="Throw_e";
static struct _tuple5 Cyc__gentuple_875={10,{_tmp398,_tmp398,_tmp398 + 8},(void*)&
Cyc__genrep_80};static char _tmp399[16]="NoInstantiate_e";static struct _tuple5 Cyc__gentuple_876={
11,{_tmp399,_tmp399,_tmp399 + 16},(void*)& Cyc__genrep_80};static char _tmp39A[14]="Instantiate_e";
static struct _tuple5 Cyc__gentuple_877={12,{_tmp39A,_tmp39A,_tmp39A + 14},(void*)&
Cyc__genrep_745};static char _tmp39B[7]="Cast_e";static struct _tuple5 Cyc__gentuple_878={
13,{_tmp39B,_tmp39B,_tmp39B + 7},(void*)& Cyc__genrep_740};static char _tmp39C[10]="Address_e";
static struct _tuple5 Cyc__gentuple_879={14,{_tmp39C,_tmp39C,_tmp39C + 10},(void*)&
Cyc__genrep_80};static char _tmp39D[6]="New_e";static struct _tuple5 Cyc__gentuple_880={
15,{_tmp39D,_tmp39D,_tmp39D + 6},(void*)& Cyc__genrep_735};static char _tmp39E[12]="Sizeoftyp_e";
static struct _tuple5 Cyc__gentuple_881={16,{_tmp39E,_tmp39E,_tmp39E + 12},(void*)&
Cyc__genrep_48};static char _tmp39F[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_882={
17,{_tmp39F,_tmp39F,_tmp39F + 12},(void*)& Cyc__genrep_80};static char _tmp3A0[11]="Offsetof_e";
static struct _tuple5 Cyc__gentuple_883={18,{_tmp3A0,_tmp3A0,_tmp3A0 + 11},(void*)&
Cyc__genrep_722};static char _tmp3A1[9]="Gentyp_e";static struct _tuple5 Cyc__gentuple_884={
19,{_tmp3A1,_tmp3A1,_tmp3A1 + 9},(void*)& Cyc__genrep_717};static char _tmp3A2[8]="Deref_e";
static struct _tuple5 Cyc__gentuple_885={20,{_tmp3A2,_tmp3A2,_tmp3A2 + 8},(void*)&
Cyc__genrep_80};static char _tmp3A3[13]="AggrMember_e";static struct _tuple5 Cyc__gentuple_886={
21,{_tmp3A3,_tmp3A3,_tmp3A3 + 13},(void*)& Cyc__genrep_712};static char _tmp3A4[12]="AggrArrow_e";
static struct _tuple5 Cyc__gentuple_887={22,{_tmp3A4,_tmp3A4,_tmp3A4 + 12},(void*)&
Cyc__genrep_712};static char _tmp3A5[12]="Subscript_e";static struct _tuple5 Cyc__gentuple_888={
23,{_tmp3A5,_tmp3A5,_tmp3A5 + 12},(void*)& Cyc__genrep_707};static char _tmp3A6[8]="Tuple_e";
static struct _tuple5 Cyc__gentuple_889={24,{_tmp3A6,_tmp3A6,_tmp3A6 + 8},(void*)&
Cyc__genrep_703};static char _tmp3A7[14]="CompoundLit_e";static struct _tuple5 Cyc__gentuple_890={
25,{_tmp3A7,_tmp3A7,_tmp3A7 + 14},(void*)& Cyc__genrep_692};static char _tmp3A8[8]="Array_e";
static struct _tuple5 Cyc__gentuple_891={26,{_tmp3A8,_tmp3A8,_tmp3A8 + 8},(void*)&
Cyc__genrep_688};static char _tmp3A9[16]="Comprehension_e";static struct _tuple5 Cyc__gentuple_892={
27,{_tmp3A9,_tmp3A9,_tmp3A9 + 16},(void*)& Cyc__genrep_682};static char _tmp3AA[9]="Struct_e";
static struct _tuple5 Cyc__gentuple_893={28,{_tmp3AA,_tmp3AA,_tmp3AA + 9},(void*)&
Cyc__genrep_674};static char _tmp3AB[13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_894={
29,{_tmp3AB,_tmp3AB,_tmp3AB + 13},(void*)& Cyc__genrep_669};static char _tmp3AC[9]="Tunion_e";
static struct _tuple5 Cyc__gentuple_895={30,{_tmp3AC,_tmp3AC,_tmp3AC + 9},(void*)&
Cyc__genrep_663};static char _tmp3AD[7]="Enum_e";static struct _tuple5 Cyc__gentuple_896={
31,{_tmp3AD,_tmp3AD,_tmp3AD + 7},(void*)& Cyc__genrep_656};static char _tmp3AE[11]="AnonEnum_e";
static struct _tuple5 Cyc__gentuple_897={32,{_tmp3AE,_tmp3AE,_tmp3AE + 11},(void*)&
Cyc__genrep_649};static char _tmp3AF[9]="Malloc_e";static struct _tuple5 Cyc__gentuple_898={
33,{_tmp3AF,_tmp3AF,_tmp3AF + 9},(void*)& Cyc__genrep_637};static char _tmp3B0[16]="UnresolvedMem_e";
static struct _tuple5 Cyc__gentuple_899={34,{_tmp3B0,_tmp3B0,_tmp3B0 + 16},(void*)&
Cyc__genrep_618};static char _tmp3B1[10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_900={
35,{_tmp3B1,_tmp3B1,_tmp3B1 + 10},(void*)& Cyc__genrep_470};static char _tmp3B2[10]="Codegen_e";
static struct _tuple5 Cyc__gentuple_901={36,{_tmp3B2,_tmp3B2,_tmp3B2 + 10},(void*)&
Cyc__genrep_85};static char _tmp3B3[7]="Fill_e";static struct _tuple5 Cyc__gentuple_902={
37,{_tmp3B3,_tmp3B3,_tmp3B3 + 7},(void*)& Cyc__genrep_80};static struct _tuple5*Cyc__genarr_903[
38]={& Cyc__gentuple_865,& Cyc__gentuple_866,& Cyc__gentuple_867,& Cyc__gentuple_868,&
Cyc__gentuple_869,& Cyc__gentuple_870,& Cyc__gentuple_871,& Cyc__gentuple_872,& Cyc__gentuple_873,&
Cyc__gentuple_874,& Cyc__gentuple_875,& Cyc__gentuple_876,& Cyc__gentuple_877,& Cyc__gentuple_878,&
Cyc__gentuple_879,& Cyc__gentuple_880,& Cyc__gentuple_881,& Cyc__gentuple_882,& Cyc__gentuple_883,&
Cyc__gentuple_884,& Cyc__gentuple_885,& Cyc__gentuple_886,& Cyc__gentuple_887,& Cyc__gentuple_888,&
Cyc__gentuple_889,& Cyc__gentuple_890,& Cyc__gentuple_891,& Cyc__gentuple_892,& Cyc__gentuple_893,&
Cyc__gentuple_894,& Cyc__gentuple_895,& Cyc__gentuple_896,& Cyc__gentuple_897,& Cyc__gentuple_898,&
Cyc__gentuple_899,& Cyc__gentuple_900,& Cyc__gentuple_901,& Cyc__gentuple_902};
static char _tmp3B5[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep={
5,{_tmp3B5,_tmp3B5,_tmp3B5 + 8},{(void*)((struct _tuple7**)Cyc__genarr_79),(void*)((
struct _tuple7**)Cyc__genarr_79),(void*)((struct _tuple7**)Cyc__genarr_79 + 0)},{(
void*)((struct _tuple5**)Cyc__genarr_903),(void*)((struct _tuple5**)Cyc__genarr_903),(
void*)((struct _tuple5**)Cyc__genarr_903 + 38)}};static char _tmp3B6[4]="Exp";static
struct _tagged_arr Cyc__genname_909={_tmp3B6,_tmp3B6,_tmp3B6 + 4};static char _tmp3B7[
5]="topt";static struct _tuple5 Cyc__gentuple_904={offsetof(struct Cyc_Absyn_Exp,topt),{
_tmp3B7,_tmp3B7,_tmp3B7 + 5},(void*)& Cyc__genrep_43};static char _tmp3B8[2]="r";
static struct _tuple5 Cyc__gentuple_905={offsetof(struct Cyc_Absyn_Exp,r),{_tmp3B8,
_tmp3B8,_tmp3B8 + 2},(void*)& Cyc_Absyn_raw_exp_t_rep};static char _tmp3B9[4]="loc";
static struct _tuple5 Cyc__gentuple_906={offsetof(struct Cyc_Absyn_Exp,loc),{_tmp3B9,
_tmp3B9,_tmp3B9 + 4},(void*)& Cyc__genrep_2};static char _tmp3BA[6]="annot";static
struct _tuple5 Cyc__gentuple_907={offsetof(struct Cyc_Absyn_Exp,annot),{_tmp3BA,
_tmp3BA,_tmp3BA + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_908[
4]={& Cyc__gentuple_904,& Cyc__gentuple_905,& Cyc__gentuple_906,& Cyc__gentuple_907};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(struct _tagged_arr*)&
Cyc__genname_909,sizeof(struct Cyc_Absyn_Exp),{(void*)((struct _tuple5**)Cyc__genarr_908),(
void*)((struct _tuple5**)Cyc__genarr_908),(void*)((struct _tuple5**)Cyc__genarr_908
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_81={1,1,(void*)((void*)&
Cyc_struct_Absyn_Exp_rep)};static struct _tuple6 Cyc__gentuple_82={offsetof(struct
_tuple31,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_83={
offsetof(struct _tuple31,f2),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_84[
2]={& Cyc__gentuple_82,& Cyc__gentuple_83};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_80={
4,sizeof(struct _tuple31),{(void*)((struct _tuple6**)Cyc__genarr_84),(void*)((
struct _tuple6**)Cyc__genarr_84),(void*)((struct _tuple6**)Cyc__genarr_84 + 2)}};
static char _tmp3BE[10]="Unknown_b";static struct _tuple7 Cyc__gentuple_985={0,{
_tmp3BE,_tmp3BE,_tmp3BE + 10}};static struct _tuple7*Cyc__genarr_986[1]={& Cyc__gentuple_985};
static char _tmp3BF[8]="Upper_b";static struct _tuple5 Cyc__gentuple_987={0,{_tmp3BF,
_tmp3BF,_tmp3BF + 8},(void*)& Cyc__genrep_80};static struct _tuple5*Cyc__genarr_988[
1]={& Cyc__gentuple_987};static char _tmp3C1[7]="Bounds";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_bounds_t_rep={5,{_tmp3C1,_tmp3C1,_tmp3C1 + 7},{(void*)((struct _tuple7**)
Cyc__genarr_986),(void*)((struct _tuple7**)Cyc__genarr_986),(void*)((struct
_tuple7**)Cyc__genarr_986 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_988),(void*)((
struct _tuple5**)Cyc__genarr_988),(void*)((struct _tuple5**)Cyc__genarr_988 + 1)}};
static struct _tuple6 Cyc__gentuple_989={offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_990={offsetof(struct _tuple6,f2),(void*)& Cyc_Absyn_bounds_t_rep};
static struct _tuple6*Cyc__genarr_991[2]={& Cyc__gentuple_989,& Cyc__gentuple_990};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_984={4,sizeof(struct _tuple6),{(
void*)((struct _tuple6**)Cyc__genarr_991),(void*)((struct _tuple6**)Cyc__genarr_991),(
void*)((struct _tuple6**)Cyc__genarr_991 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_980;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_977;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;
static char _tmp3C3[7]="Conref";static struct _tagged_arr Cyc__genname_997={_tmp3C3,
_tmp3C3,_tmp3C3 + 7};static char _tmp3C4[2]="v";static struct _tuple5 Cyc__gentuple_995={
offsetof(struct Cyc_Absyn_Conref,v),{_tmp3C4,_tmp3C4,_tmp3C4 + 2},(void*)& Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep};
static struct _tuple5*Cyc__genarr_996[1]={& Cyc__gentuple_995};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={3,(struct _tagged_arr*)& Cyc__genname_997,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_996),(void*)((
struct _tuple5**)Cyc__genarr_996),(void*)((struct _tuple5**)Cyc__genarr_996 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_977={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};
struct _tuple79{unsigned int f1;struct Cyc_Absyn_Conref*f2;};static struct _tuple6 Cyc__gentuple_981={
offsetof(struct _tuple79,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_982={
offsetof(struct _tuple79,f2),(void*)& Cyc__genrep_977};static struct _tuple6*Cyc__genarr_983[
2]={& Cyc__gentuple_981,& Cyc__gentuple_982};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_980={4,sizeof(struct _tuple79),{(void*)((struct _tuple6**)Cyc__genarr_983),(
void*)((struct _tuple6**)Cyc__genarr_983),(void*)((struct _tuple6**)Cyc__genarr_983
+ 2)}};static char _tmp3C8[10]="No_constr";static struct _tuple7 Cyc__gentuple_978={0,{
_tmp3C8,_tmp3C8,_tmp3C8 + 10}};static struct _tuple7*Cyc__genarr_979[1]={& Cyc__gentuple_978};
static char _tmp3C9[10]="Eq_constr";static struct _tuple5 Cyc__gentuple_992={0,{
_tmp3C9,_tmp3C9,_tmp3C9 + 10},(void*)& Cyc__genrep_984};static char _tmp3CA[15]="Forward_constr";
static struct _tuple5 Cyc__gentuple_993={1,{_tmp3CA,_tmp3CA,_tmp3CA + 15},(void*)&
Cyc__genrep_980};static struct _tuple5*Cyc__genarr_994[2]={& Cyc__gentuple_992,& Cyc__gentuple_993};
static char _tmp3CC[11]="Constraint";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep={
5,{_tmp3CC,_tmp3CC,_tmp3CC + 11},{(void*)((struct _tuple7**)Cyc__genarr_979),(void*)((
struct _tuple7**)Cyc__genarr_979),(void*)((struct _tuple7**)Cyc__genarr_979 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_994),(void*)((struct _tuple5**)Cyc__genarr_994),(
void*)((struct _tuple5**)Cyc__genarr_994 + 2)}};static char _tmp3CD[7]="Conref";
static struct _tagged_arr Cyc__genname_1001={_tmp3CD,_tmp3CD,_tmp3CD + 7};static char
_tmp3CE[2]="v";static struct _tuple5 Cyc__gentuple_999={offsetof(struct Cyc_Absyn_Conref,v),{
_tmp3CE,_tmp3CE,_tmp3CE + 2},(void*)& Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep};
static struct _tuple5*Cyc__genarr_1000[1]={& Cyc__gentuple_999};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep={3,(struct _tagged_arr*)& Cyc__genname_1001,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_1000),(void*)((
struct _tuple5**)Cyc__genarr_1000),(void*)((struct _tuple5**)Cyc__genarr_1000 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_998={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0bool2_rep)};
static char _tmp3D1[8]="PtrInfo";static struct _tagged_arr Cyc__genname_1008={_tmp3D1,
_tmp3D1,_tmp3D1 + 8};static char _tmp3D2[8]="elt_typ";static struct _tuple5 Cyc__gentuple_1002={
offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),{_tmp3D2,_tmp3D2,_tmp3D2 + 8},(void*)((
void*)& Cyc_Absyn_type_t_rep)};static char _tmp3D3[8]="rgn_typ";static struct _tuple5
Cyc__gentuple_1003={offsetof(struct Cyc_Absyn_PtrInfo,rgn_typ),{_tmp3D3,_tmp3D3,
_tmp3D3 + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp3D4[9]="nullable";
static struct _tuple5 Cyc__gentuple_1004={offsetof(struct Cyc_Absyn_PtrInfo,nullable),{
_tmp3D4,_tmp3D4,_tmp3D4 + 9},(void*)& Cyc__genrep_998};static char _tmp3D5[3]="tq";
static struct _tuple5 Cyc__gentuple_1005={offsetof(struct Cyc_Absyn_PtrInfo,tq),{
_tmp3D5,_tmp3D5,_tmp3D5 + 3},(void*)& Cyc__genrep_131};static char _tmp3D6[7]="bounds";
static struct _tuple5 Cyc__gentuple_1006={offsetof(struct Cyc_Absyn_PtrInfo,bounds),{
_tmp3D6,_tmp3D6,_tmp3D6 + 7},(void*)& Cyc__genrep_977};static struct _tuple5*Cyc__genarr_1007[
5]={& Cyc__gentuple_1002,& Cyc__gentuple_1003,& Cyc__gentuple_1004,& Cyc__gentuple_1005,&
Cyc__gentuple_1006};struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep={3,(
struct _tagged_arr*)& Cyc__genname_1008,sizeof(struct Cyc_Absyn_PtrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1007),(void*)((struct _tuple5**)Cyc__genarr_1007),(
void*)((struct _tuple5**)Cyc__genarr_1007 + 5)}};struct _tuple80{unsigned int f1;
struct Cyc_Absyn_PtrInfo f2;};static struct _tuple6 Cyc__gentuple_1009={offsetof(
struct _tuple80,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1010={
offsetof(struct _tuple80,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};static struct
_tuple6*Cyc__genarr_1011[2]={& Cyc__gentuple_1009,& Cyc__gentuple_1010};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_976={4,sizeof(struct _tuple80),{(void*)((
struct _tuple6**)Cyc__genarr_1011),(void*)((struct _tuple6**)Cyc__genarr_1011),(
void*)((struct _tuple6**)Cyc__genarr_1011 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_965;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep;
static char _tmp3D9[3]="B1";static struct _tuple7 Cyc__gentuple_966={0,{_tmp3D9,
_tmp3D9,_tmp3D9 + 3}};static char _tmp3DA[3]="B2";static struct _tuple7 Cyc__gentuple_967={
1,{_tmp3DA,_tmp3DA,_tmp3DA + 3}};static char _tmp3DB[3]="B4";static struct _tuple7 Cyc__gentuple_968={
2,{_tmp3DB,_tmp3DB,_tmp3DB + 3}};static char _tmp3DC[3]="B8";static struct _tuple7 Cyc__gentuple_969={
3,{_tmp3DC,_tmp3DC,_tmp3DC + 3}};static struct _tuple7*Cyc__genarr_970[4]={& Cyc__gentuple_966,&
Cyc__gentuple_967,& Cyc__gentuple_968,& Cyc__gentuple_969};static struct _tuple5*Cyc__genarr_971[
0]={};static char _tmp3DE[8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={
5,{_tmp3DE,_tmp3DE,_tmp3DE + 8},{(void*)((struct _tuple7**)Cyc__genarr_970),(void*)((
struct _tuple7**)Cyc__genarr_970),(void*)((struct _tuple7**)Cyc__genarr_970 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_971),(void*)((struct _tuple5**)Cyc__genarr_971),(
void*)((struct _tuple5**)Cyc__genarr_971 + 0)}};static struct _tuple6 Cyc__gentuple_972={
offsetof(struct _tuple68,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_973={
offsetof(struct _tuple68,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_974={
offsetof(struct _tuple68,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple6*
Cyc__genarr_975[3]={& Cyc__gentuple_972,& Cyc__gentuple_973,& Cyc__gentuple_974};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_965={4,sizeof(struct _tuple68),{(
void*)((struct _tuple6**)Cyc__genarr_975),(void*)((struct _tuple6**)Cyc__genarr_975),(
void*)((struct _tuple6**)Cyc__genarr_975 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_959;struct _tuple81{unsigned int f1;void*f2;struct Cyc_Absyn_Tqual f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_960={offsetof(struct
_tuple81,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_961={
offsetof(struct _tuple81,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_962={offsetof(struct _tuple81,f3),(void*)& Cyc__genrep_131};
static struct _tuple6 Cyc__gentuple_963={offsetof(struct _tuple81,f4),(void*)& Cyc__genrep_77};
static struct _tuple6*Cyc__genarr_964[4]={& Cyc__gentuple_960,& Cyc__gentuple_961,&
Cyc__gentuple_962,& Cyc__gentuple_963};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_959={
4,sizeof(struct _tuple81),{(void*)((struct _tuple6**)Cyc__genarr_964),(void*)((
struct _tuple6**)Cyc__genarr_964),(void*)((struct _tuple6**)Cyc__genarr_964 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_940;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_fn_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_941;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static char _tmp3E1[5]="List";static struct _tagged_arr Cyc__genname_945={_tmp3E1,
_tmp3E1,_tmp3E1 + 5};static char _tmp3E2[3]="hd";static struct _tuple5 Cyc__gentuple_942={
offsetof(struct Cyc_List_List,hd),{_tmp3E2,_tmp3E2,_tmp3E2 + 3},(void*)& Cyc__genrep_693};
static char _tmp3E3[3]="tl";static struct _tuple5 Cyc__gentuple_943={offsetof(struct
Cyc_List_List,tl),{_tmp3E3,_tmp3E3,_tmp3E3 + 3},(void*)& Cyc__genrep_941};static
struct _tuple5*Cyc__genarr_944[2]={& Cyc__gentuple_942,& Cyc__gentuple_943};struct
Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_945,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_944),(void*)((struct _tuple5**)Cyc__genarr_944),(void*)((
struct _tuple5**)Cyc__genarr_944 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_941={
1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static char _tmp3E6[7]="FnInfo";static struct _tagged_arr Cyc__genname_955={_tmp3E6,
_tmp3E6,_tmp3E6 + 7};static char _tmp3E7[6]="tvars";static struct _tuple5 Cyc__gentuple_946={
offsetof(struct Cyc_Absyn_FnInfo,tvars),{_tmp3E7,_tmp3E7,_tmp3E7 + 6},(void*)& Cyc__genrep_294};
static char _tmp3E8[7]="effect";static struct _tuple5 Cyc__gentuple_947={offsetof(
struct Cyc_Absyn_FnInfo,effect),{_tmp3E8,_tmp3E8,_tmp3E8 + 7},(void*)& Cyc__genrep_43};
static char _tmp3E9[8]="ret_typ";static struct _tuple5 Cyc__gentuple_948={offsetof(
struct Cyc_Absyn_FnInfo,ret_typ),{_tmp3E9,_tmp3E9,_tmp3E9 + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp3EA[5]="args";static struct _tuple5 Cyc__gentuple_949={offsetof(
struct Cyc_Absyn_FnInfo,args),{_tmp3EA,_tmp3EA,_tmp3EA + 5},(void*)& Cyc__genrep_941};
static char _tmp3EB[10]="c_varargs";static struct _tuple5 Cyc__gentuple_950={
offsetof(struct Cyc_Absyn_FnInfo,c_varargs),{_tmp3EB,_tmp3EB,_tmp3EB + 10},(void*)((
void*)& Cyc__genrep_62)};static char _tmp3EC[12]="cyc_varargs";static struct _tuple5
Cyc__gentuple_951={offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),{_tmp3EC,_tmp3EC,
_tmp3EC + 12},(void*)& Cyc__genrep_577};static char _tmp3ED[7]="rgn_po";static struct
_tuple5 Cyc__gentuple_952={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),{_tmp3ED,
_tmp3ED,_tmp3ED + 7},(void*)& Cyc__genrep_353};static char _tmp3EE[11]="attributes";
static struct _tuple5 Cyc__gentuple_953={offsetof(struct Cyc_Absyn_FnInfo,attributes),{
_tmp3EE,_tmp3EE,_tmp3EE + 11},(void*)& Cyc__genrep_87};static struct _tuple5*Cyc__genarr_954[
8]={& Cyc__gentuple_946,& Cyc__gentuple_947,& Cyc__gentuple_948,& Cyc__gentuple_949,&
Cyc__gentuple_950,& Cyc__gentuple_951,& Cyc__gentuple_952,& Cyc__gentuple_953};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={3,(struct _tagged_arr*)&
Cyc__genname_955,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((struct _tuple5**)Cyc__genarr_954),(
void*)((struct _tuple5**)Cyc__genarr_954),(void*)((struct _tuple5**)Cyc__genarr_954
+ 8)}};struct _tuple82{unsigned int f1;struct Cyc_Absyn_FnInfo f2;};static struct
_tuple6 Cyc__gentuple_956={offsetof(struct _tuple82,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_957={offsetof(struct _tuple82,f2),(void*)& Cyc_Absyn_fn_info_t_rep};
static struct _tuple6*Cyc__genarr_958[2]={& Cyc__gentuple_956,& Cyc__gentuple_957};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_940={4,sizeof(struct _tuple82),{(
void*)((struct _tuple6**)Cyc__genarr_958),(void*)((struct _tuple6**)Cyc__genarr_958),(
void*)((struct _tuple6**)Cyc__genarr_958 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_936;static struct _tuple6 Cyc__gentuple_937={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_938={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_268};static struct _tuple6*Cyc__genarr_939[2]={&
Cyc__gentuple_937,& Cyc__gentuple_938};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_936={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_939),(void*)((
struct _tuple6**)Cyc__genarr_939),(void*)((struct _tuple6**)Cyc__genarr_939 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_932;struct _tuple83{unsigned int
f1;struct Cyc_Absyn_AggrInfo f2;};static struct _tuple6 Cyc__gentuple_933={offsetof(
struct _tuple83,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_934={
offsetof(struct _tuple83,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6*Cyc__genarr_935[2]={& Cyc__gentuple_933,& Cyc__gentuple_934};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_932={4,sizeof(struct _tuple83),{(void*)((
struct _tuple6**)Cyc__genarr_935),(void*)((struct _tuple6**)Cyc__genarr_935),(void*)((
struct _tuple6**)Cyc__genarr_935 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_927;
static struct _tuple6 Cyc__gentuple_928={offsetof(struct _tuple60,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_929={offsetof(struct _tuple60,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_930={offsetof(struct _tuple60,f3),(void*)& Cyc__genrep_340};
static struct _tuple6*Cyc__genarr_931[3]={& Cyc__gentuple_928,& Cyc__gentuple_929,&
Cyc__gentuple_930};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_927={4,
sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_931),(void*)((
struct _tuple6**)Cyc__genarr_931),(void*)((struct _tuple6**)Cyc__genarr_931 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_922;struct _tuple84{unsigned int
f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;};static struct _tuple6 Cyc__gentuple_923={
offsetof(struct _tuple84,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_924={
offsetof(struct _tuple84,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_925={
offsetof(struct _tuple84,f3),(void*)& Cyc__genrep_657};static struct _tuple6*Cyc__genarr_926[
3]={& Cyc__gentuple_923,& Cyc__gentuple_924,& Cyc__gentuple_925};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_922={4,sizeof(struct _tuple84),{(void*)((struct _tuple6**)Cyc__genarr_926),(
void*)((struct _tuple6**)Cyc__genarr_926),(void*)((struct _tuple6**)Cyc__genarr_926
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_74;static struct _tuple6 Cyc__gentuple_919={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_920={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_75};static struct _tuple6*Cyc__genarr_921[
2]={& Cyc__gentuple_919,& Cyc__gentuple_920};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_74={4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_921),(
void*)((struct _tuple6**)Cyc__genarr_921),(void*)((struct _tuple6**)Cyc__genarr_921
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_66;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_67;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;
static char _tmp3F6[12]="Typedefdecl";static struct _tagged_arr Cyc__genname_1114={
_tmp3F6,_tmp3F6,_tmp3F6 + 12};static char _tmp3F7[5]="name";static struct _tuple5 Cyc__gentuple_1109={
offsetof(struct Cyc_Absyn_Typedefdecl,name),{_tmp3F7,_tmp3F7,_tmp3F7 + 5},(void*)&
Cyc__genrep_10};static char _tmp3F8[4]="tvs";static struct _tuple5 Cyc__gentuple_1110={
offsetof(struct Cyc_Absyn_Typedefdecl,tvs),{_tmp3F8,_tmp3F8,_tmp3F8 + 4},(void*)&
Cyc__genrep_294};static char _tmp3F9[5]="kind";static struct _tuple5 Cyc__gentuple_1111={
offsetof(struct Cyc_Absyn_Typedefdecl,kind),{_tmp3F9,_tmp3F9,_tmp3F9 + 5},(void*)&
Cyc__genrep_1073};static char _tmp3FA[5]="defn";static struct _tuple5 Cyc__gentuple_1112={
offsetof(struct Cyc_Absyn_Typedefdecl,defn),{_tmp3FA,_tmp3FA,_tmp3FA + 5},(void*)&
Cyc__genrep_43};static struct _tuple5*Cyc__genarr_1113[4]={& Cyc__gentuple_1109,&
Cyc__gentuple_1110,& Cyc__gentuple_1111,& Cyc__gentuple_1112};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Typedefdecl_rep={3,(struct _tagged_arr*)& Cyc__genname_1114,
sizeof(struct Cyc_Absyn_Typedefdecl),{(void*)((struct _tuple5**)Cyc__genarr_1113),(
void*)((struct _tuple5**)Cyc__genarr_1113),(void*)((struct _tuple5**)Cyc__genarr_1113
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_67={1,1,(void*)((void*)&
Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple85{unsigned int f1;struct _tuple0*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Typedefdecl*f4;struct Cyc_Core_Opt*f5;}
;static struct _tuple6 Cyc__gentuple_68={offsetof(struct _tuple85,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_69={offsetof(struct _tuple85,f2),(void*)& Cyc__genrep_10};
static struct _tuple6 Cyc__gentuple_70={offsetof(struct _tuple85,f3),(void*)& Cyc__genrep_53};
static struct _tuple6 Cyc__gentuple_71={offsetof(struct _tuple85,f4),(void*)& Cyc__genrep_67};
static struct _tuple6 Cyc__gentuple_72={offsetof(struct _tuple85,f5),(void*)& Cyc__genrep_43};
static struct _tuple6*Cyc__genarr_73[5]={& Cyc__gentuple_68,& Cyc__gentuple_69,& Cyc__gentuple_70,&
Cyc__gentuple_71,& Cyc__gentuple_72};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_66={
4,sizeof(struct _tuple85),{(void*)((struct _tuple6**)Cyc__genarr_73),(void*)((
struct _tuple6**)Cyc__genarr_73),(void*)((struct _tuple6**)Cyc__genarr_73 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_52;static struct _tuple6 Cyc__gentuple_58={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_59={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_53};static struct _tuple6*Cyc__genarr_60[
2]={& Cyc__gentuple_58,& Cyc__gentuple_59};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_52={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_60),(void*)((
struct _tuple6**)Cyc__genarr_60),(void*)((struct _tuple6**)Cyc__genarr_60 + 2)}};
static char _tmp3FF[9]="VoidType";static struct _tuple7 Cyc__gentuple_44={0,{_tmp3FF,
_tmp3FF,_tmp3FF + 9}};static char _tmp400[10]="FloatType";static struct _tuple7 Cyc__gentuple_45={
1,{_tmp400,_tmp400,_tmp400 + 10}};static char _tmp401[8]="HeapRgn";static struct
_tuple7 Cyc__gentuple_46={2,{_tmp401,_tmp401,_tmp401 + 8}};static struct _tuple7*Cyc__genarr_47[
3]={& Cyc__gentuple_44,& Cyc__gentuple_45,& Cyc__gentuple_46};static char _tmp402[5]="Evar";
static struct _tuple5 Cyc__gentuple_1083={0,{_tmp402,_tmp402,_tmp402 + 5},(void*)&
Cyc__genrep_1068};static char _tmp403[8]="VarType";static struct _tuple5 Cyc__gentuple_1084={
1,{_tmp403,_tmp403,_tmp403 + 8},(void*)& Cyc__genrep_1064};static char _tmp404[11]="TunionType";
static struct _tuple5 Cyc__gentuple_1085={2,{_tmp404,_tmp404,_tmp404 + 11},(void*)&
Cyc__genrep_1038};static char _tmp405[16]="TunionFieldType";static struct _tuple5 Cyc__gentuple_1086={
3,{_tmp405,_tmp405,_tmp405 + 16},(void*)& Cyc__genrep_1012};static char _tmp406[12]="PointerType";
static struct _tuple5 Cyc__gentuple_1087={4,{_tmp406,_tmp406,_tmp406 + 12},(void*)&
Cyc__genrep_976};static char _tmp407[8]="IntType";static struct _tuple5 Cyc__gentuple_1088={
5,{_tmp407,_tmp407,_tmp407 + 8},(void*)& Cyc__genrep_965};static char _tmp408[11]="DoubleType";
static struct _tuple5 Cyc__gentuple_1089={6,{_tmp408,_tmp408,_tmp408 + 11},(void*)&
Cyc__genrep_61};static char _tmp409[10]="ArrayType";static struct _tuple5 Cyc__gentuple_1090={
7,{_tmp409,_tmp409,_tmp409 + 10},(void*)& Cyc__genrep_959};static char _tmp40A[7]="FnType";
static struct _tuple5 Cyc__gentuple_1091={8,{_tmp40A,_tmp40A,_tmp40A + 7},(void*)&
Cyc__genrep_940};static char _tmp40B[10]="TupleType";static struct _tuple5 Cyc__gentuple_1092={
9,{_tmp40B,_tmp40B,_tmp40B + 10},(void*)& Cyc__genrep_936};static char _tmp40C[9]="AggrType";
static struct _tuple5 Cyc__gentuple_1093={10,{_tmp40C,_tmp40C,_tmp40C + 9},(void*)&
Cyc__genrep_932};static char _tmp40D[13]="AnonAggrType";static struct _tuple5 Cyc__gentuple_1094={
11,{_tmp40D,_tmp40D,_tmp40D + 13},(void*)& Cyc__genrep_927};static char _tmp40E[9]="EnumType";
static struct _tuple5 Cyc__gentuple_1095={12,{_tmp40E,_tmp40E,_tmp40E + 9},(void*)&
Cyc__genrep_922};static char _tmp40F[13]="AnonEnumType";static struct _tuple5 Cyc__gentuple_1096={
13,{_tmp40F,_tmp40F,_tmp40F + 13},(void*)& Cyc__genrep_74};static char _tmp410[11]="SizeofType";
static struct _tuple5 Cyc__gentuple_1097={14,{_tmp410,_tmp410,_tmp410 + 11},(void*)&
Cyc__genrep_48};static char _tmp411[14]="RgnHandleType";static struct _tuple5 Cyc__gentuple_1098={
15,{_tmp411,_tmp411,_tmp411 + 14},(void*)& Cyc__genrep_48};static char _tmp412[12]="TypedefType";
static struct _tuple5 Cyc__gentuple_1099={16,{_tmp412,_tmp412,_tmp412 + 12},(void*)&
Cyc__genrep_66};static char _tmp413[8]="TagType";static struct _tuple5 Cyc__gentuple_1100={
17,{_tmp413,_tmp413,_tmp413 + 8},(void*)& Cyc__genrep_48};static char _tmp414[8]="TypeInt";
static struct _tuple5 Cyc__gentuple_1101={18,{_tmp414,_tmp414,_tmp414 + 8},(void*)&
Cyc__genrep_61};static char _tmp415[10]="AccessEff";static struct _tuple5 Cyc__gentuple_1102={
19,{_tmp415,_tmp415,_tmp415 + 10},(void*)& Cyc__genrep_48};static char _tmp416[8]="JoinEff";
static struct _tuple5 Cyc__gentuple_1103={20,{_tmp416,_tmp416,_tmp416 + 8},(void*)&
Cyc__genrep_52};static char _tmp417[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1104={
21,{_tmp417,_tmp417,_tmp417 + 8},(void*)& Cyc__genrep_48};static struct _tuple5*Cyc__genarr_1105[
22]={& Cyc__gentuple_1083,& Cyc__gentuple_1084,& Cyc__gentuple_1085,& Cyc__gentuple_1086,&
Cyc__gentuple_1087,& Cyc__gentuple_1088,& Cyc__gentuple_1089,& Cyc__gentuple_1090,&
Cyc__gentuple_1091,& Cyc__gentuple_1092,& Cyc__gentuple_1093,& Cyc__gentuple_1094,&
Cyc__gentuple_1095,& Cyc__gentuple_1096,& Cyc__gentuple_1097,& Cyc__gentuple_1098,&
Cyc__gentuple_1099,& Cyc__gentuple_1100,& Cyc__gentuple_1101,& Cyc__gentuple_1102,&
Cyc__gentuple_1103,& Cyc__gentuple_1104};static char _tmp419[5]="Type";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_type_t_rep={5,{_tmp419,_tmp419,_tmp419 + 5},{(void*)((struct _tuple7**)
Cyc__genarr_47),(void*)((struct _tuple7**)Cyc__genarr_47),(void*)((struct _tuple7**)
Cyc__genarr_47 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_1105),(void*)((struct
_tuple5**)Cyc__genarr_1105),(void*)((struct _tuple5**)Cyc__genarr_1105 + 22)}};
static char _tmp41A[8]="Vardecl";static struct _tagged_arr Cyc__genname_151={_tmp41A,
_tmp41A,_tmp41A + 8};static char _tmp41B[3]="sc";static struct _tuple5 Cyc__gentuple_142={
offsetof(struct Cyc_Absyn_Vardecl,sc),{_tmp41B,_tmp41B,_tmp41B + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp41C[5]="name";static struct _tuple5 Cyc__gentuple_143={offsetof(
struct Cyc_Absyn_Vardecl,name),{_tmp41C,_tmp41C,_tmp41C + 5},(void*)& Cyc__genrep_10};
static char _tmp41D[3]="tq";static struct _tuple5 Cyc__gentuple_144={offsetof(struct
Cyc_Absyn_Vardecl,tq),{_tmp41D,_tmp41D,_tmp41D + 3},(void*)& Cyc__genrep_131};
static char _tmp41E[5]="type";static struct _tuple5 Cyc__gentuple_145={offsetof(
struct Cyc_Absyn_Vardecl,type),{_tmp41E,_tmp41E,_tmp41E + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp41F[12]="initializer";static struct _tuple5 Cyc__gentuple_146={
offsetof(struct Cyc_Absyn_Vardecl,initializer),{_tmp41F,_tmp41F,_tmp41F + 12},(
void*)& Cyc__genrep_77};static char _tmp420[4]="rgn";static struct _tuple5 Cyc__gentuple_147={
offsetof(struct Cyc_Absyn_Vardecl,rgn),{_tmp420,_tmp420,_tmp420 + 4},(void*)& Cyc__genrep_43};
static char _tmp421[11]="attributes";static struct _tuple5 Cyc__gentuple_148={
offsetof(struct Cyc_Absyn_Vardecl,attributes),{_tmp421,_tmp421,_tmp421 + 11},(void*)&
Cyc__genrep_87};static char _tmp422[8]="escapes";static struct _tuple5 Cyc__gentuple_149={
offsetof(struct Cyc_Absyn_Vardecl,escapes),{_tmp422,_tmp422,_tmp422 + 8},(void*)((
void*)& Cyc__genrep_62)};static struct _tuple5*Cyc__genarr_150[8]={& Cyc__gentuple_142,&
Cyc__gentuple_143,& Cyc__gentuple_144,& Cyc__gentuple_145,& Cyc__gentuple_146,& Cyc__gentuple_147,&
Cyc__gentuple_148,& Cyc__gentuple_149};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={
3,(struct _tagged_arr*)& Cyc__genname_151,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((
struct _tuple5**)Cyc__genarr_150),(void*)((struct _tuple5**)Cyc__genarr_150),(void*)((
struct _tuple5**)Cyc__genarr_150 + 8)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130={
1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};struct _tuple86{unsigned int f1;
struct Cyc_Absyn_Vardecl*f2;};static struct _tuple6 Cyc__gentuple_401={offsetof(
struct _tuple86,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_402={
offsetof(struct _tuple86,f2),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_403[
2]={& Cyc__gentuple_401,& Cyc__gentuple_402};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_400={4,sizeof(struct _tuple86),{(void*)((struct _tuple6**)Cyc__genarr_403),(
void*)((struct _tuple6**)Cyc__genarr_403),(void*)((struct _tuple6**)Cyc__genarr_403
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1134;struct _tuple87{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_1135={offsetof(struct _tuple87,f1),(void*)&
Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1136={offsetof(struct _tuple87,f2),(
void*)& Cyc__genrep_228};static struct _tuple6 Cyc__gentuple_1137={offsetof(struct
_tuple87,f3),(void*)& Cyc__genrep_128};static struct _tuple6 Cyc__gentuple_1138={
offsetof(struct _tuple87,f4),(void*)& Cyc__genrep_81};static struct _tuple6*Cyc__genarr_1139[
4]={& Cyc__gentuple_1135,& Cyc__gentuple_1136,& Cyc__gentuple_1137,& Cyc__gentuple_1138};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1134={4,sizeof(struct _tuple87),{(
void*)((struct _tuple6**)Cyc__genarr_1139),(void*)((struct _tuple6**)Cyc__genarr_1139),(
void*)((struct _tuple6**)Cyc__genarr_1139 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1130;static struct _tuple6 Cyc__gentuple_1131={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1132={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_129};static struct _tuple6*Cyc__genarr_1133[2]={&
Cyc__gentuple_1131,& Cyc__gentuple_1132};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1130={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_1133),(void*)((
struct _tuple6**)Cyc__genarr_1133),(void*)((struct _tuple6**)Cyc__genarr_1133 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1126;struct _tuple88{unsigned int
f1;struct Cyc_Absyn_Aggrdecl*f2;};static struct _tuple6 Cyc__gentuple_1127={
offsetof(struct _tuple88,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1128={
offsetof(struct _tuple88,f2),(void*)((void*)& Cyc__genrep_338)};static struct
_tuple6*Cyc__genarr_1129[2]={& Cyc__gentuple_1127,& Cyc__gentuple_1128};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1126={4,sizeof(struct _tuple88),{(void*)((
struct _tuple6**)Cyc__genarr_1129),(void*)((struct _tuple6**)Cyc__genarr_1129),(
void*)((struct _tuple6**)Cyc__genarr_1129 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1122;struct _tuple89{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;};
static struct _tuple6 Cyc__gentuple_1123={offsetof(struct _tuple89,f1),(void*)& Cyc__genrep_5};
static struct _tuple6 Cyc__gentuple_1124={offsetof(struct _tuple89,f2),(void*)((void*)&
Cyc__genrep_284)};static struct _tuple6*Cyc__genarr_1125[2]={& Cyc__gentuple_1123,&
Cyc__gentuple_1124};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1122={4,
sizeof(struct _tuple89),{(void*)((struct _tuple6**)Cyc__genarr_1125),(void*)((
struct _tuple6**)Cyc__genarr_1125),(void*)((struct _tuple6**)Cyc__genarr_1125 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1118;struct _tuple90{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple6 Cyc__gentuple_1119={
offsetof(struct _tuple90,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1120={
offsetof(struct _tuple90,f2),(void*)& Cyc__genrep_252};static struct _tuple6*Cyc__genarr_1121[
2]={& Cyc__gentuple_1119,& Cyc__gentuple_1120};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1118={4,sizeof(struct _tuple90),{(void*)((struct _tuple6**)Cyc__genarr_1121),(
void*)((struct _tuple6**)Cyc__genarr_1121),(void*)((struct _tuple6**)Cyc__genarr_1121
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_41;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_42;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_42={1,1,(void*)((
void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple91{unsigned int f1;struct
Cyc_Absyn_Typedefdecl*f2;};static struct _tuple6 Cyc__gentuple_1115={offsetof(
struct _tuple91,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_1116={
offsetof(struct _tuple91,f2),(void*)& Cyc__genrep_42};static struct _tuple6*Cyc__genarr_1117[
2]={& Cyc__gentuple_1115,& Cyc__gentuple_1116};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_41={4,sizeof(struct _tuple91),{(void*)((struct _tuple6**)Cyc__genarr_1117),(
void*)((struct _tuple6**)Cyc__genarr_1117),(void*)((struct _tuple6**)Cyc__genarr_1117
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_36;struct _tuple92{
unsigned int f1;struct _tagged_arr*f2;struct Cyc_List_List*f3;};static struct _tuple6
Cyc__gentuple_37={offsetof(struct _tuple92,f1),(void*)& Cyc__genrep_5};static
struct _tuple6 Cyc__gentuple_38={offsetof(struct _tuple92,f2),(void*)& Cyc__genrep_12};
static struct _tuple6 Cyc__gentuple_39={offsetof(struct _tuple92,f3),(void*)& Cyc__genrep_0};
static struct _tuple6*Cyc__genarr_40[3]={& Cyc__gentuple_37,& Cyc__gentuple_38,& Cyc__gentuple_39};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_36={4,sizeof(struct _tuple92),{(
void*)((struct _tuple6**)Cyc__genarr_40),(void*)((struct _tuple6**)Cyc__genarr_40),(
void*)((struct _tuple6**)Cyc__genarr_40 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9;static struct _tuple6 Cyc__gentuple_32={offsetof(struct _tuple50,f1),(
void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_33={offsetof(struct
_tuple50,f2),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_34={
offsetof(struct _tuple50,f3),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_35[
3]={& Cyc__gentuple_32,& Cyc__gentuple_33,& Cyc__gentuple_34};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9={4,sizeof(struct _tuple50),{(void*)((struct _tuple6**)Cyc__genarr_35),(
void*)((struct _tuple6**)Cyc__genarr_35),(void*)((struct _tuple6**)Cyc__genarr_35 + 
3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_4;static struct _tuple6 Cyc__gentuple_6={
offsetof(struct _tuple13,f1),(void*)& Cyc__genrep_5};static struct _tuple6 Cyc__gentuple_7={
offsetof(struct _tuple13,f2),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_8[
2]={& Cyc__gentuple_6,& Cyc__gentuple_7};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_4={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_8),(void*)((
struct _tuple6**)Cyc__genarr_8),(void*)((struct _tuple6**)Cyc__genarr_8 + 2)}};
static struct _tuple7*Cyc__genarr_3[0]={};static char _tmp430[6]="Var_d";static
struct _tuple5 Cyc__gentuple_1140={0,{_tmp430,_tmp430,_tmp430 + 6},(void*)& Cyc__genrep_400};
static char _tmp431[5]="Fn_d";static struct _tuple5 Cyc__gentuple_1141={1,{_tmp431,
_tmp431,_tmp431 + 5},(void*)& Cyc__genrep_85};static char _tmp432[6]="Let_d";static
struct _tuple5 Cyc__gentuple_1142={2,{_tmp432,_tmp432,_tmp432 + 6},(void*)& Cyc__genrep_1134};
static char _tmp433[7]="Letv_d";static struct _tuple5 Cyc__gentuple_1143={3,{_tmp433,
_tmp433,_tmp433 + 7},(void*)& Cyc__genrep_1130};static char _tmp434[7]="Aggr_d";
static struct _tuple5 Cyc__gentuple_1144={4,{_tmp434,_tmp434,_tmp434 + 7},(void*)&
Cyc__genrep_1126};static char _tmp435[9]="Tunion_d";static struct _tuple5 Cyc__gentuple_1145={
5,{_tmp435,_tmp435,_tmp435 + 9},(void*)& Cyc__genrep_1122};static char _tmp436[7]="Enum_d";
static struct _tuple5 Cyc__gentuple_1146={6,{_tmp436,_tmp436,_tmp436 + 7},(void*)&
Cyc__genrep_1118};static char _tmp437[10]="Typedef_d";static struct _tuple5 Cyc__gentuple_1147={
7,{_tmp437,_tmp437,_tmp437 + 10},(void*)& Cyc__genrep_41};static char _tmp438[12]="Namespace_d";
static struct _tuple5 Cyc__gentuple_1148={8,{_tmp438,_tmp438,_tmp438 + 12},(void*)&
Cyc__genrep_36};static char _tmp439[8]="Using_d";static struct _tuple5 Cyc__gentuple_1149={
9,{_tmp439,_tmp439,_tmp439 + 8},(void*)& Cyc__genrep_9};static char _tmp43A[10]="ExternC_d";
static struct _tuple5 Cyc__gentuple_1150={10,{_tmp43A,_tmp43A,_tmp43A + 10},(void*)&
Cyc__genrep_4};static struct _tuple5*Cyc__genarr_1151[11]={& Cyc__gentuple_1140,&
Cyc__gentuple_1141,& Cyc__gentuple_1142,& Cyc__gentuple_1143,& Cyc__gentuple_1144,&
Cyc__gentuple_1145,& Cyc__gentuple_1146,& Cyc__gentuple_1147,& Cyc__gentuple_1148,&
Cyc__gentuple_1149,& Cyc__gentuple_1150};static char _tmp43C[9]="Raw_decl";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={5,{_tmp43C,_tmp43C,_tmp43C + 9},{(
void*)((struct _tuple7**)Cyc__genarr_3),(void*)((struct _tuple7**)Cyc__genarr_3),(
void*)((struct _tuple7**)Cyc__genarr_3 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_1151),(
void*)((struct _tuple5**)Cyc__genarr_1151),(void*)((struct _tuple5**)Cyc__genarr_1151
+ 11)}};static char _tmp43D[5]="Decl";static struct _tagged_arr Cyc__genname_1155={
_tmp43D,_tmp43D,_tmp43D + 5};static char _tmp43E[2]="r";static struct _tuple5 Cyc__gentuple_1152={
offsetof(struct Cyc_Absyn_Decl,r),{_tmp43E,_tmp43E,_tmp43E + 2},(void*)& Cyc_Absyn_raw_decl_t_rep};
static char _tmp43F[4]="loc";static struct _tuple5 Cyc__gentuple_1153={offsetof(
struct Cyc_Absyn_Decl,loc),{_tmp43F,_tmp43F,_tmp43F + 4},(void*)& Cyc__genrep_2};
static struct _tuple5*Cyc__genarr_1154[2]={& Cyc__gentuple_1152,& Cyc__gentuple_1153};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={3,(struct _tagged_arr*)&
Cyc__genname_1155,sizeof(struct Cyc_Absyn_Decl),{(void*)((struct _tuple5**)Cyc__genarr_1154),(
void*)((struct _tuple5**)Cyc__genarr_1154),(void*)((struct _tuple5**)Cyc__genarr_1154
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)&
Cyc_struct_Absyn_Decl_rep)};static char _tmp442[5]="List";static struct _tagged_arr
Cyc__genname_1159={_tmp442,_tmp442,_tmp442 + 5};static char _tmp443[3]="hd";static
struct _tuple5 Cyc__gentuple_1156={offsetof(struct Cyc_List_List,hd),{_tmp443,
_tmp443,_tmp443 + 3},(void*)& Cyc__genrep_1};static char _tmp444[3]="tl";static
struct _tuple5 Cyc__gentuple_1157={offsetof(struct Cyc_List_List,tl),{_tmp444,
_tmp444,_tmp444 + 3},(void*)& Cyc__genrep_0};static struct _tuple5*Cyc__genarr_1158[
2]={& Cyc__gentuple_1156,& Cyc__gentuple_1157};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={
3,(struct _tagged_arr*)& Cyc__genname_1159,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_1158),(void*)((struct _tuple5**)Cyc__genarr_1158),(
void*)((struct _tuple5**)Cyc__genarr_1158 + 2)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};
void*Cyc_decls_rep=(void*)& Cyc__genrep_0;
