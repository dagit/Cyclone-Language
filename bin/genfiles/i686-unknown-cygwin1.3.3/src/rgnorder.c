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
 struct Cyc_Std___cycFILE;struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);extern char Cyc_List_List_mismatch[18];extern char Cyc_List_Nth[8];
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;};struct Cyc_Absyn_Aggrdecl{void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_uint_typ;extern void*
Cyc_Absyn_empty_effect;struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Tcenv_VarRes_struct{
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_normalize_effect(void*e);
struct Cyc_RgnOrder_RgnInfo{void*these_outlive;int resetable;};struct Cyc_RgnOrder_RgnPO{
struct Cyc_Dict_Dict*d;void*these_outlive_heap;struct Cyc_Absyn_Tvar*
youngest_unresettable;};struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){struct Cyc_RgnOrder_RgnPO*_tmp0=({
struct Cyc_RgnOrder_RgnPO*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE[0]=*po;_tmpE;});
void*_tmp1=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp2;_LL1: if(_tmp1 <= (
void*)3?1:*((int*)_tmp1)!= 1)goto _LL3;_tmp2=((struct Cyc_Absyn_VarType_struct*)
_tmp1)->f1;_LL2: {struct Cyc_Dict_Dict*_tmp3=po->d;if(((int(*)(struct Cyc_Dict_Dict*
d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(_tmp3,_tmp2)){void*_tmp4=(void*)(((
struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict*d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(
_tmp3,_tmp2))->these_outlive;_tmp3=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(_tmp3,
_tmp2,({struct Cyc_RgnOrder_RgnInfo*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->these_outlive=(
void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6=_cycalloc(sizeof(*_tmp6));
_tmp6[0]=({struct Cyc_Absyn_JoinEff_struct _tmp7;_tmp7.tag=20;_tmp7.f1=({void*
_tmp8[2];_tmp8[1]=_tmp4;_tmp8[0]=eff;Cyc_List_list(_tag_arr(_tmp8,sizeof(void*),
2));});_tmp7;});_tmp6;}));_tmp5->resetable=0;_tmp5;}));}else{_tmp3=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(
_tmp3,_tmp2,({struct Cyc_RgnOrder_RgnInfo*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->these_outlive=(
void*)eff;_tmp9->resetable=0;_tmp9;}));}_tmp0->d=_tmp3;return _tmp0;}_LL3: if((int)
_tmp1 != 2)goto _LL5;_LL4:(void*)(_tmp0->these_outlive_heap=(void*)((void*)({
struct Cyc_Absyn_JoinEff_struct*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA[0]=({struct
Cyc_Absyn_JoinEff_struct _tmpB;_tmpB.tag=20;_tmpB.f1=({void*_tmpC[2];_tmpC[1]=(
void*)po->these_outlive_heap;_tmpC[0]=eff;Cyc_List_list(_tag_arr(_tmpC,sizeof(
void*),2));});_tmpB;});_tmpA;})));return _tmp0;_LL5:;_LL6:({void*_tmpD[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("RgnOrder::add_outlives_constraint passed a bad region",
sizeof(char),54),_tag_arr(_tmpD,sizeof(void*),0));});_LL0:;}struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,
int resettable){if(((int(*)(struct Cyc_Dict_Dict*d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(
po->d,rgn))({void*_tmpF[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("RgnOrder::add_youngest: repeated region",sizeof(char),
40),_tag_arr(_tmpF,sizeof(void*),0));});{struct Cyc_RgnOrder_RgnPO*_tmp10=({
struct Cyc_RgnOrder_RgnPO*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=*po;_tmp17;});
struct Cyc_Absyn_AccessEff_struct*_tmp11=({struct Cyc_Absyn_AccessEff_struct*
_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp14;_tmp14.tag=19;_tmp14.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=({struct Cyc_Absyn_VarType_struct
_tmp16;_tmp16.tag=1;_tmp16.f1=po->youngest_unresettable;_tmp16;});_tmp15;}));
_tmp14;});_tmp13;});_tmp10->d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(po->d,rgn,({
struct Cyc_RgnOrder_RgnInfo*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->these_outlive=(
void*)((void*)_tmp11);_tmp12->resetable=resettable;_tmp12;}));if(!resettable)
_tmp10->youngest_unresettable=rgn;return _tmp10;}}struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn){struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*)))Cyc_Dict_empty)(Cyc_Absyn_tvar_cmp);{struct Cyc_List_List*
_tmp18=tvs;for(0;_tmp18 != 0;_tmp18=_tmp18->tl){void*_tmp19=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp18->hd);_LL8: if((int)_tmp19 != 3)goto _LLA;_LL9: d=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*
v))Cyc_Dict_insert)(d,(struct Cyc_Absyn_Tvar*)_tmp18->hd,({struct Cyc_RgnOrder_RgnInfo*
_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->these_outlive=(void*)Cyc_Absyn_empty_effect;
_tmp1A->resetable=0;_tmp1A;}));goto _LL7;_LLA:;_LLB: goto _LL7;_LL7:;}}d=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*
v))Cyc_Dict_insert)(d,fst_rgn,({struct Cyc_RgnOrder_RgnInfo*_tmp1B=_cycalloc(
sizeof(*_tmp1B));_tmp1B->these_outlive=(void*)effect;_tmp1B->resetable=0;_tmp1B;}));{
struct Cyc_RgnOrder_RgnPO*_tmp1C=({struct Cyc_RgnOrder_RgnPO*_tmp1D=_cycalloc(
sizeof(*_tmp1D));_tmp1D->d=d;_tmp1D->these_outlive_heap=(void*)Cyc_Absyn_empty_effect;
_tmp1D->youngest_unresettable=fst_rgn;_tmp1D;});for(0;po != 0;po=po->tl){_tmp1C=
Cyc_RgnOrder_add_outlives_constraint(_tmp1C,(*((struct _tuple0*)po->hd)).f1,(*((
struct _tuple0*)po->hd)).f2);}return _tmp1C;}}int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r){return(((struct Cyc_RgnOrder_RgnInfo*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,r))->resetable;}
static int Cyc_RgnOrder_contains_rgnseff(struct Cyc_Absyn_Tvar*rgns_of_var,void*eff){
void*_tmp1E=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(eff));void*_tmp1F;
struct Cyc_List_List*_tmp20;_LLD: if(_tmp1E <= (void*)3?1:*((int*)_tmp1E)!= 21)goto
_LLF;_tmp1F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E)->f1;_LLE: {void*
_tmp21=Cyc_Tcutil_compress(_tmp1F);struct Cyc_Absyn_Tvar*_tmp22;_LL14: if(_tmp21 <= (
void*)3?1:*((int*)_tmp21)!= 1)goto _LL16;_tmp22=((struct Cyc_Absyn_VarType_struct*)
_tmp21)->f1;_LL15: return Cyc_Absyn_tvar_cmp(_tmp22,rgns_of_var)== 0;_LL16:;_LL17:
return 0;_LL13:;}_LLF: if(_tmp1E <= (void*)3?1:*((int*)_tmp1E)!= 20)goto _LL11;
_tmp20=((struct Cyc_Absyn_JoinEff_struct*)_tmp1E)->f1;_LL10: for(0;_tmp20 != 0;
_tmp20=_tmp20->tl){if(Cyc_RgnOrder_contains_rgnseff(rgns_of_var,(void*)_tmp20->hd))
return 1;}return 0;_LL11:;_LL12: return 0;_LLC:;}struct Cyc_RgnOrder_OutlivesEnv{
struct _RegionHandle*r;struct Cyc_List_List*seen;struct Cyc_List_List*todo;};static
void Cyc_RgnOrder_add_to_search(struct Cyc_RgnOrder_OutlivesEnv*env,void*eff){void*
_tmp23=Cyc_Tcutil_compress(eff);void*_tmp24;struct Cyc_Absyn_Tvar*_tmp25;struct
Cyc_List_List*_tmp26;_LL19: if(_tmp23 <= (void*)3?1:*((int*)_tmp23)!= 19)goto _LL1B;
_tmp24=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp23)->f1;if(_tmp24 <= (void*)
3?1:*((int*)_tmp24)!= 1)goto _LL1B;_tmp25=((struct Cyc_Absyn_VarType_struct*)
_tmp24)->f1;_LL1A:{struct Cyc_List_List*_tmp27=env->seen;for(0;_tmp27 != 0;_tmp27=
_tmp27->tl){if(Cyc_Absyn_tvar_cmp(_tmp25,(struct Cyc_Absyn_Tvar*)_tmp27->hd)== 0)
return;}}env->seen=({struct Cyc_List_List*_tmp28=_region_malloc(env->r,sizeof(*
_tmp28));_tmp28->hd=_tmp25;_tmp28->tl=env->seen;_tmp28;});env->todo=({struct Cyc_List_List*
_tmp29=_region_malloc(env->r,sizeof(*_tmp29));_tmp29->hd=_tmp25;_tmp29->tl=env->todo;
_tmp29;});return;_LL1B: if(_tmp23 <= (void*)3?1:*((int*)_tmp23)!= 20)goto _LL1D;
_tmp26=((struct Cyc_Absyn_JoinEff_struct*)_tmp23)->f1;_LL1C: for(0;_tmp26 != 0;
_tmp26=_tmp26->tl){Cyc_RgnOrder_add_to_search(env,(void*)_tmp26->hd);}return;
_LL1D:;_LL1E: return;_LL18:;}static struct Cyc_RgnOrder_OutlivesEnv Cyc_RgnOrder_initial_env(
struct _RegionHandle*listrgn,struct Cyc_RgnOrder_RgnPO*po,void*rgn){struct Cyc_RgnOrder_OutlivesEnv
_tmp2A=({struct Cyc_RgnOrder_OutlivesEnv _tmp2F;_tmp2F.r=listrgn;_tmp2F.seen=0;
_tmp2F.todo=0;_tmp2F;});Cyc_RgnOrder_add_to_search(& _tmp2A,(void*)po->these_outlive_heap);{
void*_tmp2B=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp2C;_LL20: if(_tmp2B
<= (void*)3?1:*((int*)_tmp2B)!= 1)goto _LL22;_tmp2C=((struct Cyc_Absyn_VarType_struct*)
_tmp2B)->f1;_LL21: _tmp2A.seen=({struct Cyc_List_List*_tmp2D=_region_malloc(
listrgn,sizeof(*_tmp2D));_tmp2D->hd=_tmp2C;_tmp2D->tl=_tmp2A.seen;_tmp2D;});
_tmp2A.todo=({struct Cyc_List_List*_tmp2E=_region_malloc(listrgn,sizeof(*_tmp2E));
_tmp2E->hd=_tmp2C;_tmp2E->tl=_tmp2A.todo;_tmp2E;});goto _LL23;_LL22:;_LL23: return
_tmp2A;_LL1F:;}}static int Cyc_RgnOrder_atomic_effect_outlives(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn){struct _RegionHandle _tmp30=_new_region("listrgn");struct
_RegionHandle*listrgn=& _tmp30;_push_region(listrgn);{void*_tmp31=Cyc_Tcutil_compress(
eff);void*_tmp32;void*_tmp33;_LL25: if(_tmp31 <= (void*)3?1:*((int*)_tmp31)!= 19)
goto _LL27;_tmp32=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp31)->f1;_LL26: {
void*_tmp34=Cyc_Tcutil_compress(_tmp32);struct Cyc_Absyn_Tvar*_tmp35;_LL2C: if((
int)_tmp34 != 2)goto _LL2E;_LL2D: {int _tmp36=1;_npop_handler(0);return _tmp36;}
_LL2E: if(_tmp34 <= (void*)3?1:*((int*)_tmp34)!= 1)goto _LL30;_tmp35=((struct Cyc_Absyn_VarType_struct*)
_tmp34)->f1;_LL2F: {struct Cyc_RgnOrder_OutlivesEnv _tmp37=Cyc_RgnOrder_initial_env(
listrgn,po,rgn);while(_tmp37.todo != 0){struct Cyc_Absyn_Tvar*_tmp38=(struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp37.todo))->hd;_tmp37.todo=((struct Cyc_List_List*)
_check_null(_tmp37.todo))->tl;if(Cyc_Absyn_tvar_cmp(_tmp38,_tmp35)== 0){int
_tmp39=1;_npop_handler(0);return _tmp39;}if(((int(*)(struct Cyc_Dict_Dict*d,struct
Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp38))Cyc_RgnOrder_add_to_search(&
_tmp37,(void*)(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict*d,struct Cyc_Absyn_Tvar*
k))Cyc_Dict_lookup)(po->d,_tmp38))->these_outlive);else{({void*_tmp3A[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("RgnOrder: type variable not in dict",
sizeof(char),36),_tag_arr(_tmp3A,sizeof(void*),0));});}}{int _tmp3B=0;
_npop_handler(0);return _tmp3B;}}_LL30:;_LL31: {int _tmp3C=0;_npop_handler(0);
return _tmp3C;}_LL2B:;}_LL27: if(_tmp31 <= (void*)3?1:*((int*)_tmp31)!= 21)goto
_LL29;_tmp33=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp31)->f1;_LL28: {void*
_tmp3D=Cyc_Tcutil_compress(_tmp33);struct Cyc_Absyn_Tvar*_tmp3E;_LL33: if(_tmp3D <= (
void*)3?1:*((int*)_tmp3D)!= 1)goto _LL35;_tmp3E=((struct Cyc_Absyn_VarType_struct*)
_tmp3D)->f1;_LL34: {struct Cyc_RgnOrder_OutlivesEnv _tmp3F=Cyc_RgnOrder_initial_env(
listrgn,po,rgn);if(Cyc_RgnOrder_contains_rgnseff(_tmp3E,(void*)po->these_outlive_heap)){
int _tmp40=1;_npop_handler(0);return _tmp40;}while(_tmp3F.todo != 0){struct Cyc_Absyn_Tvar*
_tmp41=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp3F.todo))->hd;
_tmp3F.todo=((struct Cyc_List_List*)_check_null(_tmp3F.todo))->tl;if(((int(*)(
struct Cyc_Dict_Dict*d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp41)){
void*_tmp42=(void*)(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,_tmp41))->these_outlive;if(Cyc_RgnOrder_contains_rgnseff(
_tmp3E,_tmp42)){int _tmp43=1;_npop_handler(0);return _tmp43;}Cyc_RgnOrder_add_to_search(&
_tmp3F,_tmp42);}else{({void*_tmp44[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("RgnOrder: type variable not in dict",
sizeof(char),36),_tag_arr(_tmp44,sizeof(void*),0));});}}{int _tmp45=0;
_npop_handler(0);return _tmp45;}}_LL35:;_LL36: {int _tmp46=0;_npop_handler(0);
return _tmp46;}_LL32:;}_LL29:;_LL2A: {int _tmp47=0;_npop_handler(0);return _tmp47;}
_LL24:;};_pop_region(listrgn);}int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn){eff=Cyc_Tcutil_normalize_effect(eff);{void*_tmp48=Cyc_Tcutil_compress(
eff);struct Cyc_List_List*_tmp49;_LL38: if(_tmp48 <= (void*)3?1:*((int*)_tmp48)!= 
20)goto _LL3A;_tmp49=((struct Cyc_Absyn_JoinEff_struct*)_tmp48)->f1;_LL39: for(0;
_tmp49 != 0;_tmp49=_tmp49->tl){if(!Cyc_RgnOrder_effect_outlives(po,(void*)_tmp49->hd,
rgn))return 0;}return 1;_LL3A:;_LL3B: return Cyc_RgnOrder_atomic_effect_outlives(po,
eff,rgn);_LL37:;}}static void Cyc_RgnOrder_pin_effect(void*eff,void*bd){eff=Cyc_Tcutil_normalize_effect(
eff);{void*_tmp4A=Cyc_Tcutil_compress(eff);struct Cyc_List_List*_tmp4B;void*
_tmp4C;void*_tmp4D;_LL3D: if(_tmp4A <= (void*)3?1:*((int*)_tmp4A)!= 20)goto _LL3F;
_tmp4B=((struct Cyc_Absyn_JoinEff_struct*)_tmp4A)->f1;_LL3E: for(0;_tmp4B != 0;
_tmp4B=_tmp4B->tl){Cyc_RgnOrder_pin_effect((void*)_tmp4B->hd,bd);}return;_LL3F:
if(_tmp4A <= (void*)3?1:*((int*)_tmp4A)!= 21)goto _LL41;_tmp4C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp4A)->f1;_LL40: {void*_tmp4E=Cyc_Tcutil_compress(_tmp4C);_LL48: if(_tmp4E <= (
void*)3?1:*((int*)_tmp4E)!= 0)goto _LL4A;_LL49: Cyc_Tcutil_unify(_tmp4C,Cyc_Absyn_uint_typ);
return;_LL4A:;_LL4B: return;_LL47:;}_LL41: if(_tmp4A <= (void*)3?1:*((int*)_tmp4A)
!= 19)goto _LL43;_tmp4D=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp4A)->f1;
_LL42: {void*_tmp4F=Cyc_Tcutil_compress(_tmp4D);_LL4D: if(_tmp4F <= (void*)3?1:*((
int*)_tmp4F)!= 0)goto _LL4F;_LL4E: Cyc_Tcutil_unify(_tmp4D,bd);return;_LL4F:;_LL50:
return;_LL4C:;}_LL43: if(_tmp4A <= (void*)3?1:*((int*)_tmp4A)!= 0)goto _LL45;_LL44:
Cyc_Tcutil_unify(eff,Cyc_Absyn_empty_effect);return;_LL45:;_LL46: return;_LL3C:;}}
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin){{struct Cyc_List_List*_tmp50=
constraints;for(0;_tmp50 != 0;_tmp50=_tmp50->tl){struct _tuple0 _tmp52;void*_tmp53;
struct _tuple0*_tmp51=(struct _tuple0*)_tmp50->hd;_tmp52=*_tmp51;_tmp53=_tmp52.f2;{
void*_tmp54=Cyc_Tcutil_compress(_tmp53);_LL52: if(_tmp54 <= (void*)3?1:*((int*)
_tmp54)!= 0)goto _LL54;_LL53: if(do_pin)Cyc_Tcutil_unify(_tmp53,default_bound);
goto _LL51;_LL54:;_LL55: goto _LL51;_LL51:;}}}{struct Cyc_List_List*_tmp55=
constraints;for(0;_tmp55 != 0;_tmp55=_tmp55->tl){struct _tuple0 _tmp57;void*_tmp58;
void*_tmp59;struct _tuple0*_tmp56=(struct _tuple0*)_tmp55->hd;_tmp57=*_tmp56;
_tmp58=_tmp57.f1;_tmp59=_tmp57.f2;if(do_pin)Cyc_RgnOrder_pin_effect(_tmp58,
_tmp59);if(!Cyc_RgnOrder_effect_outlives(po,_tmp58,_tmp59))return 0;}}return 1;}
static int Cyc_RgnOrder_eff_outlives_atomic_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2){eff2=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff2));{
void*_tmp5A=eff2;struct Cyc_List_List*_tmp5B;void*_tmp5C;void*_tmp5D;_LL57: if(
_tmp5A <= (void*)3?1:*((int*)_tmp5A)!= 20)goto _LL59;_tmp5B=((struct Cyc_Absyn_JoinEff_struct*)
_tmp5A)->f1;_LL58: for(0;_tmp5B != 0;_tmp5B=_tmp5B->tl){if(Cyc_RgnOrder_eff_outlives_atomic_eff(
po,eff1,(void*)_tmp5B->hd))return 1;}return 0;_LL59: if(_tmp5A <= (void*)3?1:*((int*)
_tmp5A)!= 19)goto _LL5B;_tmp5C=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp5A)->f1;
_LL5A: return Cyc_RgnOrder_effect_outlives(po,eff1,_tmp5C);_LL5B: if(_tmp5A <= (void*)
3?1:*((int*)_tmp5A)!= 21)goto _LL5D;_tmp5D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp5A)->f1;_LL5C: {void*_tmp5E=eff1;void*_tmp5F;_LL60: if(_tmp5E <= (void*)3?1:*((
int*)_tmp5E)!= 21)goto _LL62;_tmp5F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp5E)->f1;_LL61:{struct _tuple0 _tmp61=({struct _tuple0 _tmp60;_tmp60.f1=Cyc_Tcutil_compress(
_tmp5D);_tmp60.f2=Cyc_Tcutil_compress(_tmp5F);_tmp60;});void*_tmp62;struct Cyc_Absyn_Tvar*
_tmp63;void*_tmp64;struct Cyc_Absyn_Tvar*_tmp65;_LL65: _tmp62=_tmp61.f1;if(_tmp62
<= (void*)3?1:*((int*)_tmp62)!= 1)goto _LL67;_tmp63=((struct Cyc_Absyn_VarType_struct*)
_tmp62)->f1;_tmp64=_tmp61.f2;if(_tmp64 <= (void*)3?1:*((int*)_tmp64)!= 1)goto
_LL67;_tmp65=((struct Cyc_Absyn_VarType_struct*)_tmp64)->f1;_LL66: return Cyc_Absyn_tvar_cmp(
_tmp63,_tmp65)== 0;_LL67:;_LL68: goto _LL64;_LL64:;}goto _LL63;_LL62:;_LL63: return
eff1 == (void*)2;_LL5F:;}_LL5D:;_LL5E: return eff1 == (void*)2;_LL56:;}}int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){eff1=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
eff1));{void*_tmp66=eff1;struct Cyc_List_List*_tmp67;_LL6A: if(_tmp66 <= (void*)3?1:*((
int*)_tmp66)!= 20)goto _LL6C;_tmp67=((struct Cyc_Absyn_JoinEff_struct*)_tmp66)->f1;
_LL6B: for(0;_tmp67 != 0;_tmp67=_tmp67->tl){if(!Cyc_RgnOrder_eff_outlives_eff(po,(
void*)_tmp67->hd,eff2))return 0;}return 1;_LL6C:;_LL6D: return Cyc_RgnOrder_eff_outlives_atomic_eff(
po,eff1,eff2);_LL69:;}}
