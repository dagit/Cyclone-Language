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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};struct
_tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rrev(struct
_RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct
_RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};
struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Dict_Dict;
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*
Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char
Cyc_Set_Absent[11];int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);
int Cyc_Std_zstrcmp(struct _tagged_arr,struct _tagged_arr);int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,
struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*
rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*
po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);
void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern struct Cyc_Core_Opt
Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);struct _tuple4{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*
Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr err_msg);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{
struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void
Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,struct Cyc_Absyn_Pat*
p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,struct Cyc_List_List*);
struct _tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static void Cyc_Tcpat_resolve_pat(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp0=(void*)p->r;struct
_tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*_tmp3;struct Cyc_Absyn_AggrInfo
_tmp4;void*_tmp5;void*_tmp6;struct _tuple1*_tmp7;struct Cyc_List_List*_tmp8;struct
Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;_LL1: if(_tmp0 <= (void*)2?1:*((int*)
_tmp0)!= 11)goto _LL3;_tmp1=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{
struct _handler_cons _tmpB;_push_handler(& _tmpB);{int _tmpD=0;if(setjmp(_tmpB.handler))
_tmpD=1;if(!_tmpD){{void*_tmpE=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp1);struct
Cyc_Absyn_Tuniondecl*_tmpF;struct Cyc_Absyn_Tunionfield*_tmp10;struct Cyc_Absyn_Enumdecl*
_tmp11;struct Cyc_Absyn_Enumfield*_tmp12;void*_tmp13;struct Cyc_Absyn_Enumfield*
_tmp14;_LLA: if(*((int*)_tmpE)!= 1)goto _LLC;_LLB:({void*_tmp15[0]={};Cyc_Tcutil_terr(
p->loc,_tag_arr("struct tag used without arguments in pattern",sizeof(char),45),
_tag_arr(_tmp15,sizeof(void*),0));});_npop_handler(0);return;_LLC: if(*((int*)
_tmpE)!= 2)goto _LLE;_tmpF=((struct Cyc_Tcenv_TunionRes_struct*)_tmpE)->f1;_tmp10=((
struct Cyc_Tcenv_TunionRes_struct*)_tmpE)->f2;_LLD:(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Tunion_p_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({
struct Cyc_Absyn_Tunion_p_struct _tmp17;_tmp17.tag=8;_tmp17.f1=_tmpF;_tmp17.f2=
_tmp10;_tmp17.f3=0;_tmp17;});_tmp16;})));_npop_handler(0);return;_LLE: if(*((int*)
_tmpE)!= 3)goto _LL10;_tmp11=((struct Cyc_Tcenv_EnumRes_struct*)_tmpE)->f1;_tmp12=((
struct Cyc_Tcenv_EnumRes_struct*)_tmpE)->f2;_LLF:(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Enum_p_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({
struct Cyc_Absyn_Enum_p_struct _tmp19;_tmp19.tag=9;_tmp19.f1=_tmp11;_tmp19.f2=
_tmp12;_tmp19;});_tmp18;})));_npop_handler(0);return;_LL10: if(*((int*)_tmpE)!= 4)
goto _LL12;_tmp13=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmpE)->f1;_tmp14=((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmpE)->f2;_LL11:(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_AnonEnum_p_struct*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[0]=({
struct Cyc_Absyn_AnonEnum_p_struct _tmp1B;_tmp1B.tag=10;_tmp1B.f1=(void*)_tmp13;
_tmp1B.f2=_tmp14;_tmp1B;});_tmp1A;})));_npop_handler(0);return;_LL12: if(*((int*)
_tmpE)!= 0)goto _LL9;_LL13: goto _LL9;_LL9:;};_pop_handler();}else{void*_tmpC=(void*)
_exn_thrown;void*_tmp1D=_tmpC;_LL15: if(_tmp1D != Cyc_Dict_Absent)goto _LL17;_LL16:
goto _LL14;_LL17:;_LL18:(void)_throw(_tmp1D);_LL14:;}}}{void*_tmp1E=(*_tmp1).f1;
struct Cyc_List_List*_tmp1F;_LL1A: if((int)_tmp1E != 0)goto _LL1C;_LL1B: goto _LL1D;
_LL1C: if(_tmp1E <= (void*)1?1:*((int*)_tmp1E)!= 0)goto _LL1E;_tmp1F=((struct Cyc_Absyn_Rel_n_struct*)
_tmp1E)->f1;if(_tmp1F != 0)goto _LL1E;_LL1D:(*_tmp1).f1=(void*)0;(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Var_p_struct*_tmp20=_cycalloc(sizeof(*_tmp20));
_tmp20[0]=({struct Cyc_Absyn_Var_p_struct _tmp21;_tmp21.tag=0;_tmp21.f1=Cyc_Absyn_new_vardecl(
_tmp1,(void*)0,0);_tmp21;});_tmp20;})));return;_LL1E:;_LL1F:({void*_tmp22[0]={};
Cyc_Tcutil_terr(p->loc,_tag_arr("qualified variable in pattern",sizeof(char),30),
_tag_arr(_tmp22,sizeof(void*),0));});return;_LL19:;}_LL3: if(_tmp0 <= (void*)2?1:*((
int*)_tmp0)!= 12)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f1;
_tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_LL4:{struct
_handler_cons _tmp23;_push_handler(& _tmp23);{int _tmp25=0;if(setjmp(_tmp23.handler))
_tmp25=1;if(!_tmp25){{void*_tmp26=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp2);
struct Cyc_Absyn_Aggrdecl*_tmp27;struct Cyc_Absyn_Tuniondecl*_tmp28;struct Cyc_Absyn_Tunionfield*
_tmp29;_LL21: if(*((int*)_tmp26)!= 1)goto _LL23;_tmp27=((struct Cyc_Tcenv_AggrRes_struct*)
_tmp26)->f1;_LL22: if((void*)_tmp27->kind == (void*)1){({void*_tmp2A[0]={};Cyc_Tcutil_terr(
p->loc,_tag_arr("cannot pattern-match a union",sizeof(char),29),_tag_arr(_tmp2A,
sizeof(void*),0));});_npop_handler(0);return;}{struct Cyc_List_List*_tmp2B=0;for(
0;_tmp3 != 0;_tmp3=_tmp3->tl){_tmp2B=({struct Cyc_List_List*_tmp2C=_cycalloc(
sizeof(*_tmp2C));_tmp2C->hd=({struct _tuple7*_tmp2D=_cycalloc(sizeof(*_tmp2D));
_tmp2D->f1=0;_tmp2D->f2=(struct Cyc_Absyn_Pat*)_tmp3->hd;_tmp2D;});_tmp2C->tl=
_tmp2B;_tmp2C;});}(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*
_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp2F;_tmp2F.tag=7;_tmp2F.f1=({struct Cyc_Absyn_AggrInfo _tmp30;_tmp30.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp31=_cycalloc(sizeof(*_tmp31));
_tmp31[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp32;_tmp32.tag=1;_tmp32.f1=({
struct Cyc_Absyn_Aggrdecl**_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33[0]=_tmp27;
_tmp33;});_tmp32;});_tmp31;}));_tmp30.targs=0;_tmp30;});_tmp2F.f2=0;_tmp2F.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2B);_tmp2F;});
_tmp2E;})));_npop_handler(0);return;}_LL23: if(*((int*)_tmp26)!= 2)goto _LL25;
_tmp28=((struct Cyc_Tcenv_TunionRes_struct*)_tmp26)->f1;_tmp29=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp26)->f2;_LL24:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp35;_tmp35.tag=8;_tmp35.f1=_tmp28;_tmp35.f2=_tmp29;_tmp35.f3=_tmp3;_tmp35;});
_tmp34;})));_npop_handler(0);return;_LL25: if(*((int*)_tmp26)!= 3)goto _LL27;_LL26:
goto _LL28;_LL27: if(*((int*)_tmp26)!= 4)goto _LL29;_LL28:({void*_tmp36[0]={};Cyc_Tcutil_terr(
p->loc,_tag_arr("enum tag used with arguments in pattern",sizeof(char),40),
_tag_arr(_tmp36,sizeof(void*),0));});_npop_handler(0);return;_LL29: if(*((int*)
_tmp26)!= 0)goto _LL20;_LL2A: goto _LL20;_LL20:;};_pop_handler();}else{void*_tmp24=(
void*)_exn_thrown;void*_tmp38=_tmp24;_LL2C: if(_tmp38 != Cyc_Dict_Absent)goto _LL2E;
_LL2D: goto _LL2B;_LL2E:;_LL2F:(void)_throw(_tmp38);_LL2B:;}}}({struct Cyc_Std_String_pa_struct
_tmp3A;_tmp3A.tag=0;_tmp3A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp2);{
void*_tmp39[1]={& _tmp3A};Cyc_Tcutil_terr(p->loc,_tag_arr("%s is not a constructor in pattern",
sizeof(char),35),_tag_arr(_tmp39,sizeof(void*),1));}});return;_LL5: if(_tmp0 <= (
void*)2?1:*((int*)_tmp0)!= 7)goto _LL7;_tmp4=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f1;_tmp5=(void*)_tmp4.aggr_info;if(*((int*)_tmp5)!= 0)goto _LL7;_tmp6=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp5)->f1;_tmp7=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp5)->f2;_tmp8=_tmp4.targs;_tmp9=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;
_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f3;_LL6:{struct _handler_cons
_tmp3B;_push_handler(& _tmp3B);{int _tmp3D=0;if(setjmp(_tmp3B.handler))_tmp3D=1;
if(!_tmp3D){{struct Cyc_Absyn_Aggrdecl**_tmp3E=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,
_tmp7);struct Cyc_Absyn_Aggrdecl*_tmp3F=*_tmp3E;if((void*)_tmp3F->kind == (void*)1){({
void*_tmp40[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("cannot pattern-match a union",
sizeof(char),29),_tag_arr(_tmp40,sizeof(void*),0));});_npop_handler(0);return;}
if(_tmp3F->impl == 0){({void*_tmp41[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("can't destructure an abstract struct",
sizeof(char),37),_tag_arr(_tmp41,sizeof(void*),0));});_npop_handler(0);return;}{
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3F->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp9);if(more_exists != 0)({void*_tmp42[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("wrong number of existentially bound type variables",
sizeof(char),51),_tag_arr(_tmp42,sizeof(void*),0));});(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Aggr_p_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=({
struct Cyc_Absyn_Aggr_p_struct _tmp44;_tmp44.tag=7;_tmp44.f1=({struct Cyc_Absyn_AggrInfo
_tmp45;_tmp45.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp46=
_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp47;
_tmp47.tag=1;_tmp47.f1=_tmp3E;_tmp47;});_tmp46;}));_tmp45.targs=_tmp8;_tmp45;});
_tmp44.f2=_tmp9;_tmp44.f3=_tmpA;_tmp44;});_tmp43;})));}};_pop_handler();}else{
void*_tmp3C=(void*)_exn_thrown;void*_tmp49=_tmp3C;_LL31: if(_tmp49 != Cyc_Dict_Absent)
goto _LL33;_LL32:({void*_tmp4A[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("Non-struct name has designator patterns",
sizeof(char),40),_tag_arr(_tmp4A,sizeof(void*),0));});goto _LL30;_LL33:;_LL34:(
void)_throw(_tmp49);_LL30:;}}}return;_LL7:;_LL8: return;_LL0:;}static struct
_tagged_arr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){if(topt != 0)
return*topt;return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->v=s;_tmp4B;}));}
static void*Cyc_Tcpat_num_type(void**topt,void*numt){if(topt != 0?Cyc_Tcutil_coerceable(*
topt): 0)return*topt;{void*_tmp4C=Cyc_Tcutil_compress(numt);_LL36: if(_tmp4C <= (
void*)3?1:*((int*)_tmp4C)!= 12)goto _LL38;_LL37: if(topt != 0)return*topt;goto _LL35;
_LL38:;_LL39: goto _LL35;_LL35:;}return numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_List_List**v_result_ptr,void*t){(void*)(vd->type=(void*)t);vd->tq=
Cyc_Absyn_empty_tqual();*v_result_ptr=(struct Cyc_List_List*)({struct Cyc_List_List*
_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->hd=vd;_tmp4D->tl=*v_result_ptr;_tmp4D;});}
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult
res1,struct Cyc_Tcpat_TcPatResult res2){struct _tuple6*_tmp4F;struct Cyc_List_List*
_tmp50;struct Cyc_Tcpat_TcPatResult _tmp4E=res1;_tmp4F=_tmp4E.tvars_and_bounds_opt;
_tmp50=_tmp4E.patvars;{struct _tuple6*_tmp52;struct Cyc_List_List*_tmp53;struct Cyc_Tcpat_TcPatResult
_tmp51=res2;_tmp52=_tmp51.tvars_and_bounds_opt;_tmp53=_tmp51.patvars;if(_tmp4F != 
0?1: _tmp52 != 0){if(_tmp4F == 0)_tmp4F=({struct _tuple6*_tmp54=_cycalloc(sizeof(*
_tmp54));_tmp54->f1=0;_tmp54->f2=0;_tmp54;});if(_tmp52 == 0)_tmp52=({struct
_tuple6*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->f1=0;_tmp55->f2=0;_tmp55;});
return({struct Cyc_Tcpat_TcPatResult _tmp56;_tmp56.tvars_and_bounds_opt=({struct
_tuple6*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp4F))).f1,(*((struct _tuple6*)_check_null(_tmp52))).f1);_tmp57->f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((
struct _tuple6*)_check_null(_tmp4F))).f2,(*((struct _tuple6*)_check_null(_tmp52))).f2);
_tmp57;});_tmp56.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp50,_tmp53);_tmp56;});}return({struct Cyc_Tcpat_TcPatResult
_tmp58;_tmp58.tvars_and_bounds_opt=0;_tmp58.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp50,_tmp53);
_tmp58;});}}struct _tuple8{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple9{struct
Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**
rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{void*t;struct Cyc_Tcpat_TcPatResult res=({
struct Cyc_Tcpat_TcPatResult _tmpFA;_tmpFA.tvars_and_bounds_opt=0;_tmpFA.patvars=0;
_tmpFA;});{void*_tmp59=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp5A;struct Cyc_Absyn_Vardecl*
_tmp5B;void*_tmp5C;void*_tmp5D;void*_tmp5E;struct Cyc_Absyn_Enumdecl*_tmp5F;void*
_tmp60;struct Cyc_Absyn_Pat*_tmp61;struct Cyc_List_List*_tmp62;struct Cyc_Absyn_AggrInfo
_tmp63;void*_tmp64;struct Cyc_Absyn_Aggrdecl**_tmp65;struct Cyc_Absyn_Aggrdecl*
_tmp66;struct Cyc_List_List*_tmp67;struct Cyc_List_List**_tmp68;struct Cyc_List_List*
_tmp69;struct Cyc_List_List*_tmp6A;struct Cyc_Absyn_Tuniondecl*_tmp6B;struct Cyc_Absyn_Tunionfield*
_tmp6C;struct Cyc_List_List*_tmp6D;struct Cyc_Absyn_AggrInfo _tmp6E;void*_tmp6F;
_LL3B: if((int)_tmp59 != 0)goto _LL3D;_LL3C: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);goto _LL3A;_LL3D: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 0)goto _LL3F;
_tmp5A=((struct Cyc_Absyn_Var_p_struct*)_tmp59)->f1;_LL3E: t=Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars(te),topt);Cyc_Tcpat_set_vd(_tmp5A,& res.patvars,t);
goto _LL3A;_LL3F: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 6)goto _LL41;_tmp5B=((
struct Cyc_Absyn_Reference_p_struct*)_tmp59)->f1;_LL40: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);if(rgn_opt == 0){({void*_tmp70[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("* pattern would point into an unknown/unallowed region",
sizeof(char),55),_tag_arr(_tmp70,sizeof(void*),0));});goto _LL3A;}Cyc_Tcpat_set_vd(
_tmp5B,& res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*_tmp71=_cycalloc(
sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_PointerType_struct _tmp72;_tmp72.tag=
4;_tmp72.f1=({struct Cyc_Absyn_PtrInfo _tmp73;_tmp73.elt_typ=(void*)t;_tmp73.rgn_typ=(
void*)*rgn_opt;_tmp73.nullable=((struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(
0);_tmp73.tq=Cyc_Absyn_empty_tqual();_tmp73.bounds=Cyc_Absyn_empty_conref();
_tmp73;});_tmp72;});_tmp71;}));goto _LL3A;_LL41: if(_tmp59 <= (void*)2?1:*((int*)
_tmp59)!= 1)goto _LL43;_tmp5C=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp59)->f1;
if((int)_tmp5C != 1)goto _LL43;_LL42: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);
goto _LL3A;_LL43: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 1)goto _LL45;_tmp5D=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp59)->f1;if((int)_tmp5D != 2)goto _LL45;
_LL44: goto _LL46;_LL45: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 1)goto _LL47;
_tmp5E=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp59)->f1;if((int)_tmp5E != 0)
goto _LL47;_LL46: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL3A;_LL47:
if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 2)goto _LL49;_LL48: t=Cyc_Tcpat_num_type(
topt,Cyc_Absyn_char_typ);goto _LL3A;_LL49: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)
!= 3)goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ);goto _LL3A;
_LL4B: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 9)goto _LL4D;_tmp5F=((struct Cyc_Absyn_Enum_p_struct*)
_tmp59)->f1;_LL4C: t=Cyc_Tcpat_num_type(topt,(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Absyn_EnumType_struct
_tmp75;_tmp75.tag=12;_tmp75.f1=_tmp5F->name;_tmp75.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp5F;_tmp75;});_tmp74;}));goto _LL3A;_LL4D: if(_tmp59 <= (void*)2?1:*((int*)
_tmp59)!= 10)goto _LL4F;_tmp60=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp59)->f1;
_LL4E: t=Cyc_Tcpat_num_type(topt,_tmp60);goto _LL3A;_LL4F: if((int)_tmp59 != 1)goto
_LL51;_LL50: if(topt != 0){void*_tmp76=Cyc_Tcutil_compress(*topt);_LL60: if(_tmp76
<= (void*)3?1:*((int*)_tmp76)!= 4)goto _LL62;_LL61: t=*topt;goto tcpat_end;_LL62:;
_LL63: goto _LL5F;_LL5F:;}{struct Cyc_List_List*_tmp77=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp78=_cycalloc(sizeof(*
_tmp78));_tmp78[0]=({struct Cyc_Absyn_PointerType_struct _tmp79;_tmp79.tag=4;
_tmp79.f1=({struct Cyc_Absyn_PtrInfo _tmp7A;_tmp7A.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp7C=_cycalloc(sizeof(*
_tmp7C));_tmp7C->v=_tmp77;_tmp7C;}));_tmp7A.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp7B=_cycalloc(sizeof(*
_tmp7B));_tmp7B->v=_tmp77;_tmp7B;}));_tmp7A.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmp7A.tq=Cyc_Absyn_empty_tqual();_tmp7A.bounds=
Cyc_Absyn_empty_conref();_tmp7A;});_tmp79;});_tmp78;});goto _LL3A;}_LL51: if(
_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 5)goto _LL53;_tmp61=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp59)->f1;_LL52: {void*inner_typ=(void*)0;void**_tmp7D=0;if(topt != 0){void*
_tmp7E=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp7F;void*_tmp80;
_LL65: if(_tmp7E <= (void*)3?1:*((int*)_tmp7E)!= 4)goto _LL67;_tmp7F=((struct Cyc_Absyn_PointerType_struct*)
_tmp7E)->f1;_tmp80=(void*)_tmp7F.elt_typ;_LL66: inner_typ=_tmp80;_tmp7D=(void**)&
inner_typ;goto _LL64;_LL67:;_LL68: goto _LL64;_LL64:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp93=_cycalloc(sizeof(*
_tmp93));_tmp93->v=Cyc_Tcenv_lookup_type_vars(te);_tmp93;}));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp61,_tmp7D,(void**)& ptr_rgn));if(_tmp7D == 0){void*
_tmp81=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp61->topt))->v);
struct Cyc_Absyn_TunionFieldInfo _tmp82;void*_tmp83;struct Cyc_Absyn_Tuniondecl*
_tmp84;struct Cyc_Absyn_Tunionfield*_tmp85;struct Cyc_List_List*_tmp86;_LL6A: if(
_tmp81 <= (void*)3?1:*((int*)_tmp81)!= 3)goto _LL6C;_tmp82=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp81)->f1;_tmp83=(void*)_tmp82.field_info;if(*((int*)_tmp83)!= 1)goto _LL6C;
_tmp84=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp83)->f1;_tmp85=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp83)->f2;_tmp86=_tmp82.targs;_LL6B: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_TunionType_struct
_tmp88;_tmp88.tag=2;_tmp88.f1=({struct Cyc_Absyn_TunionInfo _tmp89;_tmp89.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp8A=_cycalloc(sizeof(*
_tmp8A));_tmp8A[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp8B;_tmp8B.tag=1;
_tmp8B.f1=({struct Cyc_Absyn_Tuniondecl**_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[
0]=_tmp84;_tmp8C;});_tmp8B;});_tmp8A;}));_tmp89.targs=_tmp86;_tmp89.rgn=(void*)
ptr_rgn;_tmp89;});_tmp88;});_tmp87;});goto _LL69;_LL6C:;_LL6D: t=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({
struct Cyc_Absyn_PointerType_struct _tmp8E;_tmp8E.tag=4;_tmp8E.f1=({struct Cyc_Absyn_PtrInfo
_tmp8F;_tmp8F.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp61->topt))->v);
_tmp8F.rgn_typ=(void*)ptr_rgn;_tmp8F.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp8F.tq=Cyc_Absyn_empty_tqual();_tmp8F.bounds=Cyc_Absyn_empty_conref();_tmp8F;});
_tmp8E;});_tmp8D;});goto _LL69;_LL69:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({struct Cyc_Absyn_PointerType_struct
_tmp91;_tmp91.tag=4;_tmp91.f1=({struct Cyc_Absyn_PtrInfo _tmp92;_tmp92.elt_typ=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp61->topt))->v);_tmp92.rgn_typ=(
void*)ptr_rgn;_tmp92.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp92.tq=Cyc_Absyn_empty_tqual();_tmp92.bounds=Cyc_Absyn_empty_conref();_tmp92;});
_tmp91;});_tmp90;});}goto _LL3A;}}_LL53: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 
4)goto _LL55;_tmp62=((struct Cyc_Absyn_Tuple_p_struct*)_tmp59)->f1;_LL54: {struct
Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(topt != 0){void*_tmp94=
Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp95;_LL6F: if(_tmp94 <= (void*)3?
1:*((int*)_tmp94)!= 9)goto _LL71;_tmp95=((struct Cyc_Absyn_TupleType_struct*)
_tmp94)->f1;_LL70: topt_ts=_tmp95;goto _LL6E;_LL71:;_LL72: goto _LL6E;_LL6E:;}for(0;
_tmp62 != 0;_tmp62=_tmp62->tl){void**_tmp96=0;if(topt_ts != 0){_tmp96=(void**)&(*((
struct _tuple8*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp62->hd,_tmp96,rgn_opt));
pat_ts=({struct Cyc_List_List*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->hd=({
struct _tuple8*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->f1=Cyc_Absyn_empty_tqual();
_tmp98->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)
_tmp62->hd)->topt))->v;_tmp98;});_tmp97->tl=pat_ts;_tmp97;});}t=(void*)({struct
Cyc_Absyn_TupleType_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct
Cyc_Absyn_TupleType_struct _tmp9A;_tmp9A.tag=9;_tmp9A.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmp9A;});_tmp99;});goto _LL3A;}
_LL55: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 7)goto _LL57;_tmp63=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp59)->f1;_tmp64=(void*)_tmp63.aggr_info;if(*((int*)_tmp64)!= 1)goto _LL57;
_tmp65=((struct Cyc_Absyn_KnownAggr_struct*)_tmp64)->f1;_tmp66=*_tmp65;_tmp67=
_tmp63.targs;_tmp68=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)
_tmp59)->f1).targs;_tmp69=((struct Cyc_Absyn_Aggr_p_struct*)_tmp59)->f2;_tmp6A=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp59)->f3;_LL56: if(_tmp66->impl == 0){({void*
_tmp9B[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("can't destructure an abstract struct",
sizeof(char),37),_tag_arr(_tmp9B,sizeof(void*),0));});t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp9C;}));goto _LL3A;}{struct _RegionHandle _tmp9D=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp9D;_push_region(rgn);{struct Cyc_List_List*_tmp9E=0;struct
Cyc_List_List*outlives_constraints=0;struct Cyc_List_List*_tmp9F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp66->impl))->exist_vars;{struct Cyc_List_List*t=_tmp69;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp9F))->hd;_tmp9F=
_tmp9F->tl;{void*_tmpA0=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpA1=Cyc_Absyn_compress_kb((
void*)uv->kind);int error=0;void*k2;{void*_tmpA2=_tmpA1;void*_tmpA3;void*_tmpA4;
_LL74: if(*((int*)_tmpA2)!= 2)goto _LL76;_tmpA3=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpA2)->f2;_LL75: _tmpA4=_tmpA3;goto _LL77;_LL76: if(*((int*)_tmpA2)!= 0)goto _LL78;
_tmpA4=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpA2)->f1;_LL77: k2=_tmpA4;goto
_LL73;_LL78:;_LL79:({void*_tmpA5[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unconstrained existential type variable in struct",
sizeof(char),50),_tag_arr(_tmpA5,sizeof(void*),0));});_LL73:;}{void*_tmpA6=
_tmpA0;void*_tmpA7;struct Cyc_Core_Opt*_tmpA8;struct Cyc_Core_Opt**_tmpA9;void*
_tmpAA;struct Cyc_Core_Opt*_tmpAB;struct Cyc_Core_Opt**_tmpAC;_LL7B: if(*((int*)
_tmpA6)!= 0)goto _LL7D;_tmpA7=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpA6)->f1;
_LL7C: if(!Cyc_Tcutil_kind_leq(k2,_tmpA7))error=1;goto _LL7A;_LL7D: if(*((int*)
_tmpA6)!= 2)goto _LL7F;_tmpA8=((struct Cyc_Absyn_Less_kb_struct*)_tmpA6)->f1;
_tmpA9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpA6)->f1;
_tmpAA=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpA6)->f2;_LL7E: _tmpAC=_tmpA9;
goto _LL80;_LL7F: if(*((int*)_tmpA6)!= 1)goto _LL7A;_tmpAB=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA6)->f1;_tmpAC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA6)->f1;_LL80:*_tmpAC=({struct Cyc_Core_Opt*_tmpAD=_cycalloc(sizeof(*_tmpAD));
_tmpAD->v=(void*)_tmpA1;_tmpAD;});goto _LL7A;_LL7A:;}if(error)({struct Cyc_Std_String_pa_struct
_tmpB1;_tmpB1.tag=0;_tmpB1.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(k2);{
struct Cyc_Std_String_pa_struct _tmpB0;_tmpB0.tag=0;_tmpB0.f1=(struct _tagged_arr)
Cyc_Absynpp_kindbound2string(_tmpA0);{struct Cyc_Std_String_pa_struct _tmpAF;
_tmpAF.tag=0;_tmpAF.f1=(struct _tagged_arr)*tv->name;{void*_tmpAE[3]={& _tmpAF,&
_tmpB0,& _tmpB1};Cyc_Tcutil_terr(p->loc,_tag_arr("type variable %s has kind %s but must have at least kind %s",
sizeof(char),60),_tag_arr(_tmpAE,sizeof(void*),3));}}}});{void*vartype=(void*)({
struct Cyc_Absyn_VarType_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({
struct Cyc_Absyn_VarType_struct _tmpB6;_tmpB6.tag=1;_tmpB6.f1=tv;_tmpB6;});_tmpB5;});
_tmp9E=({struct Cyc_List_List*_tmpB2=_region_malloc(rgn,sizeof(*_tmpB2));_tmpB2->hd=(
void*)((void*)vartype);_tmpB2->tl=_tmp9E;_tmpB2;});if(k2 == (void*)3)
outlives_constraints=({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*_tmpB3));
_tmpB3->hd=({struct _tuple0*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->f1=Cyc_Absyn_empty_effect;
_tmpB4->f2=vartype;_tmpB4;});_tmpB3->tl=outlives_constraints;_tmpB3;});}}}}
_tmp9E=Cyc_List_imp_rev(_tmp9E);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
p->loc,te,_tmp69);struct Cyc_List_List*_tmpB7=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple4 _tmpB8=({struct _tuple4 _tmpD2;_tmpD2.f1=_tmpB7;_tmpD2.f2=rgn;_tmpD2;});
struct Cyc_List_List*_tmpB9=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpB8,_tmp66->tvs);struct
Cyc_List_List*_tmpBA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,
rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp66->impl))->exist_vars,
_tmp9E);struct Cyc_List_List*_tmpBB=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpB9);struct Cyc_List_List*_tmpBC=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpBA);struct Cyc_List_List*_tmpBD=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpB9,_tmpBA);
if(_tmp69 != 0?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp66->impl))->rgn_po
!= 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*
_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->f1=0;_tmpBE->f2=0;_tmpBE;});(*((struct
_tuple6*)_check_null(res.tvars_and_bounds_opt))).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(res.tvars_and_bounds_opt))).f1,_tmp69);(*res.tvars_and_bounds_opt).f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f2,outlives_constraints);{struct Cyc_List_List*_tmpBF=0;{
struct Cyc_List_List*_tmpC0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp66->impl))->rgn_po;
for(0;_tmpC0 != 0;_tmpC0=_tmpC0->tl){_tmpBF=({struct Cyc_List_List*_tmpC1=
_cycalloc(sizeof(*_tmpC1));_tmpC1->hd=({struct _tuple0*_tmpC2=_cycalloc(sizeof(*
_tmpC2));_tmpC2->f1=Cyc_Tcutil_rsubstitute(rgn,_tmpBD,(*((struct _tuple0*)_tmpC0->hd)).f1);
_tmpC2->f2=Cyc_Tcutil_rsubstitute(rgn,_tmpBD,(*((struct _tuple0*)_tmpC0->hd)).f2);
_tmpC2;});_tmpC1->tl=_tmpBF;_tmpC1;});}}_tmpBF=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBF);(*res.tvars_and_bounds_opt).f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f2,_tmpBF);}}*_tmp68=_tmpBB;t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_AggrType_struct
_tmpC4;_tmpC4.tag=10;_tmpC4.f1=({struct Cyc_Absyn_AggrInfo _tmpC5;_tmpC5.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));
_tmpC6[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpC7;_tmpC7.tag=1;_tmpC7.f1=({
struct Cyc_Absyn_Aggrdecl**_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=_tmp66;
_tmpC8;});_tmpC7;});_tmpC6;}));_tmpC5.targs=*_tmp68;_tmpC5;});_tmpC4;});_tmpC3;});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,_tmp6A,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp66->impl))->fields);for(0;fields != 0;fields=fields->tl){struct
_tuple9 _tmpCA;struct Cyc_Absyn_Aggrfield*_tmpCB;struct Cyc_Absyn_Pat*_tmpCC;struct
_tuple9*_tmpC9=(struct _tuple9*)fields->hd;_tmpCA=*_tmpC9;_tmpCB=_tmpCA.f1;_tmpCC=
_tmpCA.f2;{void*_tmpCD=Cyc_Tcutil_rsubstitute(rgn,_tmpBD,(void*)_tmpCB->type);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmpCC,(void**)& _tmpCD,
rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmpCC->topt))->v,
_tmpCD))({struct Cyc_Std_String_pa_struct _tmpD1;_tmpD1.tag=0;_tmpD1.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpCC->topt))->v);{struct Cyc_Std_String_pa_struct _tmpD0;_tmpD0.tag=0;_tmpD0.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpCD);{struct Cyc_Std_String_pa_struct
_tmpCF;_tmpCF.tag=0;_tmpCF.f1=(struct _tagged_arr)*_tmpCB->name;{void*_tmpCE[3]={&
_tmpCF,& _tmpD0,& _tmpD1};Cyc_Tcutil_terr(p->loc,_tag_arr("field %s of struct pattern expects type %s != %s",
sizeof(char),49),_tag_arr(_tmpCE,sizeof(void*),3));}}}});}}}}};_pop_region(rgn);}
goto _LL3A;_LL57: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 8)goto _LL59;_tmp6B=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp59)->f1;_tmp6C=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp59)->f2;_tmp6D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp59)->f3;_LL58:{struct
_RegionHandle _tmpD3=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD3;
_push_region(rgn);{struct Cyc_List_List*tqts=_tmp6C->typs;struct Cyc_List_List*
_tmpD4=Cyc_Tcenv_lookup_type_vars(te);struct _tuple4 _tmpD5=({struct _tuple4 _tmpF7;
_tmpF7.f1=_tmpD4;_tmpF7.f2=rgn;_tmpF7;});struct Cyc_List_List*_tmpD6=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmpD5,_tmp6B->tvs);struct Cyc_List_List*_tmpD7=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmpD6);if(tqts == 0)t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_TunionType_struct
_tmpD9;_tmpD9.tag=2;_tmpD9.f1=({struct Cyc_Absyn_TunionInfo _tmpDA;_tmpDA.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpDC=_cycalloc(sizeof(*
_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpDD;_tmpDD.tag=1;
_tmpDD.f1=({struct Cyc_Absyn_Tuniondecl**_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[
0]=_tmp6B;_tmpDE;});_tmpDD;});_tmpDC;}));_tmpDA.targs=_tmpD7;_tmpDA.rgn=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->v=_tmpD4;_tmpDB;}));_tmpDA;});_tmpD9;});
_tmpD8;});else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmpDF=
_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpE0;_tmpE0.tag=3;_tmpE0.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpE1;_tmpE1.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmpE2=_cycalloc(sizeof(*
_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmpE3;_tmpE3.tag=1;
_tmpE3.f1=_tmp6B;_tmpE3.f2=_tmp6C;_tmpE3;});_tmpE2;}));_tmpE1.targs=_tmpD7;
_tmpE1;});_tmpE0;});_tmpDF;});}if(topt != 0?tqts == 0: 0){void*_tmpE4=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_TunionInfo _tmpE5;struct Cyc_List_List*_tmpE6;_LL82: if(
_tmpE4 <= (void*)3?1:*((int*)_tmpE4)!= 3)goto _LL84;_LL83: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpE8;_tmpE8.tag=3;_tmpE8.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpE9;_tmpE9.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmpEA=_cycalloc(sizeof(*
_tmpEA));_tmpEA[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmpEB;_tmpEB.tag=1;
_tmpEB.f1=_tmp6B;_tmpEB.f2=_tmp6C;_tmpEB;});_tmpEA;}));_tmpE9.targs=_tmpD7;
_tmpE9;});_tmpE8;});_tmpE7;});goto _LL81;_LL84: if(_tmpE4 <= (void*)3?1:*((int*)
_tmpE4)!= 2)goto _LL86;_tmpE5=((struct Cyc_Absyn_TunionType_struct*)_tmpE4)->f1;
_tmpE6=_tmpE5.targs;_LL85: {struct Cyc_List_List*_tmpEC=_tmpD7;for(0;_tmpEC != 0?
_tmpE6 != 0: 0;(_tmpEC=_tmpEC->tl,_tmpE6=_tmpE6->tl)){Cyc_Tcutil_unify((void*)
_tmpEC->hd,(void*)_tmpE6->hd);}goto _LL81;}_LL86:;_LL87: goto _LL81;_LL81:;}for(0;
_tmp6D != 0?tqts != 0: 0;(_tmp6D=_tmp6D->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmpED=(struct Cyc_Absyn_Pat*)_tmp6D->hd;void*_tmpEE=Cyc_Tcutil_rsubstitute(rgn,
_tmpD6,(*((struct _tuple8*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmpED,(void**)& _tmpEE,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpED->topt))->v,_tmpEE))({struct Cyc_Std_String_pa_struct _tmpF2;
_tmpF2.tag=0;_tmpF2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpED->topt))->v);{struct Cyc_Std_String_pa_struct
_tmpF1;_tmpF1.tag=0;_tmpF1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpEE);{
struct Cyc_Std_String_pa_struct _tmpF0;_tmpF0.tag=0;_tmpF0.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp6B->name);{void*_tmpEF[3]={& _tmpF0,& _tmpF1,& _tmpF2};
Cyc_Tcutil_terr(_tmpED->loc,_tag_arr("%s expects argument type %s, not %s",
sizeof(char),36),_tag_arr(_tmpEF,sizeof(void*),3));}}}});}if(_tmp6D != 0)({struct
Cyc_Std_String_pa_struct _tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6B->name);{void*_tmpF3[1]={& _tmpF4};Cyc_Tcutil_terr(p->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(char),45),_tag_arr(_tmpF3,sizeof(void*),1));}});if(tqts != 0)({struct Cyc_Std_String_pa_struct
_tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp6B->name);{
void*_tmpF5[1]={& _tmpF6};Cyc_Tcutil_terr(p->loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(char),44),_tag_arr(_tmpF5,sizeof(void*),1));}});};_pop_region(rgn);}goto
_LL3A;_LL59: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 7)goto _LL5B;_tmp6E=((struct
Cyc_Absyn_Aggr_p_struct*)_tmp59)->f1;_tmp6F=(void*)_tmp6E.aggr_info;if(*((int*)
_tmp6F)!= 0)goto _LL5B;_LL5A: goto _LL5C;_LL5B: if(_tmp59 <= (void*)2?1:*((int*)
_tmp59)!= 11)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if(_tmp59 <= (void*)2?1:*((int*)
_tmp59)!= 12)goto _LL3A;_LL5E: t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpF8=
_cycalloc(sizeof(*_tmpF8));_tmpF8->v=Cyc_Tcenv_lookup_type_vars(te);_tmpF8;}));
goto _LL3A;_LL3A:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmpF9=_cycalloc(
sizeof(*_tmpF9));_tmpF9->v=(void*)t;_tmpF9;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmpFB=Cyc_Tcpat_tcPatRec(te,p,topt,0);{struct _RegionHandle
_tmpFC=_new_region("r");struct _RegionHandle*r=& _tmpFC;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmpFB.patvars),p->loc,
_tag_arr("pattern contains a repeated variable",sizeof(char),37));;_pop_region(r);}
return _tmpFB;}void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmpFD=(void*)p->r;struct Cyc_Absyn_Pat*_tmpFE;struct Cyc_List_List*_tmpFF;
struct Cyc_List_List*_tmp100;struct Cyc_List_List*_tmp101;_LL89: if(_tmpFD <= (void*)
2?1:*((int*)_tmpFD)!= 5)goto _LL8B;_tmpFE=((struct Cyc_Absyn_Pointer_p_struct*)
_tmpFD)->f1;_LL8A: Cyc_Tcpat_check_pat_regions(te,_tmpFE);{void*_tmp102=(void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_PtrInfo _tmp103;void*
_tmp104;struct Cyc_Absyn_TunionInfo _tmp105;void*_tmp106;_LL94: if(_tmp102 <= (void*)
3?1:*((int*)_tmp102)!= 4)goto _LL96;_tmp103=((struct Cyc_Absyn_PointerType_struct*)
_tmp102)->f1;_tmp104=(void*)_tmp103.rgn_typ;_LL95: _tmp106=_tmp104;goto _LL97;
_LL96: if(_tmp102 <= (void*)3?1:*((int*)_tmp102)!= 2)goto _LL98;_tmp105=((struct Cyc_Absyn_TunionType_struct*)
_tmp102)->f1;_tmp106=(void*)_tmp105.rgn;_LL97: Cyc_Tcenv_check_rgn_accessible(te,
p->loc,_tmp106);return;_LL98:;_LL99:({void*_tmp107[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_pat_regions: bad pointer type",
sizeof(char),36),_tag_arr(_tmp107,sizeof(void*),0));});_LL93:;}_LL8B: if(_tmpFD <= (
void*)2?1:*((int*)_tmpFD)!= 8)goto _LL8D;_tmpFF=((struct Cyc_Absyn_Tunion_p_struct*)
_tmpFD)->f3;_LL8C: for(0;_tmpFF != 0;_tmpFF=_tmpFF->tl){Cyc_Tcpat_check_pat_regions(
te,(struct Cyc_Absyn_Pat*)_tmpFF->hd);}{void*_tmp108=(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v;struct Cyc_Absyn_TunionInfo _tmp109;void*_tmp10A;_LL9B:
if(_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 2)goto _LL9D;_tmp109=((struct Cyc_Absyn_TunionType_struct*)
_tmp108)->f1;_tmp10A=(void*)_tmp109.rgn;_LL9C: Cyc_Tcenv_check_rgn_accessible(te,
p->loc,_tmp10A);return;_LL9D: if(_tmp108 <= (void*)3?1:*((int*)_tmp108)!= 3)goto
_LL9F;_LL9E: return;_LL9F:;_LLA0:({void*_tmp10B[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_pat_regions: bad tunion type",
sizeof(char),35),_tag_arr(_tmp10B,sizeof(void*),0));});_LL9A:;}_LL8D: if(_tmpFD <= (
void*)2?1:*((int*)_tmpFD)!= 7)goto _LL8F;_tmp100=((struct Cyc_Absyn_Aggr_p_struct*)
_tmpFD)->f3;_LL8E: for(0;_tmp100 != 0;_tmp100=_tmp100->tl){Cyc_Tcpat_check_pat_regions(
te,(*((struct _tuple7*)_tmp100->hd)).f2);}return;_LL8F: if(_tmpFD <= (void*)2?1:*((
int*)_tmpFD)!= 4)goto _LL91;_tmp101=((struct Cyc_Absyn_Tuple_p_struct*)_tmpFD)->f1;
_LL90: for(0;_tmp101 != 0;_tmp101=_tmp101->tl){Cyc_Tcpat_check_pat_regions(te,(
struct Cyc_Absyn_Pat*)_tmp101->hd);}return;_LL91:;_LL92: return;_LL88:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};struct
Cyc_Tcpat_Con_s{void*name;int arity;struct Cyc_Core_Opt*span;};struct Cyc_Tcpat_Con_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){void*_tmp10C=(void*)c1->name;
struct _tagged_arr _tmp10D;int _tmp10E;_LLA2: if(*((int*)_tmp10C)!= 0)goto _LLA4;
_tmp10D=((struct Cyc_Tcpat_Name_v_struct*)_tmp10C)->f1;_LLA3: {void*_tmp10F=(void*)
c2->name;struct _tagged_arr _tmp110;_LLA7: if(*((int*)_tmp10F)!= 0)goto _LLA9;
_tmp110=((struct Cyc_Tcpat_Name_v_struct*)_tmp10F)->f1;_LLA8: return Cyc_Std_zstrcmp(
_tmp10D,_tmp110);_LLA9: if(*((int*)_tmp10F)!= 1)goto _LLA6;_LLAA: return - 1;_LLA6:;}
_LLA4: if(*((int*)_tmp10C)!= 1)goto _LLA1;_tmp10E=((struct Cyc_Tcpat_Int_v_struct*)
_tmp10C)->f1;_LLA5: {void*_tmp111=(void*)c2->name;int _tmp112;_LLAC: if(*((int*)
_tmp111)!= 0)goto _LLAE;_LLAD: return 1;_LLAE: if(*((int*)_tmp111)!= 1)goto _LLAB;
_tmp112=((struct Cyc_Tcpat_Int_v_struct*)_tmp111)->f1;_LLAF: return _tmp10E - 
_tmp112;_LLAB:;}_LLA1:;}static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct
_RegionHandle*r){return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(
struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(
void*)2};static struct Cyc_Core_Opt*Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)&
Cyc_Tcpat_one_opt;static char _tmp114[5]="NULL";static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={0,{_tmp114,_tmp114,_tmp114 + 5}};static char _tmp116[4]="ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={0,{_tmp116,_tmp116,
_tmp116 + 4}};static char _tmp118[6]="tuple";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={
0,{_tmp118,_tmp118,_tmp118 + 6}};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)((void*)& Cyc_Tcpat_null_name_value),0,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={(void*)((void*)& Cyc_Tcpat_ptr_name_value),
1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)((void*)& Cyc_Tcpat_ptr_name_value),1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0;static struct
Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0;static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(
int i){return({struct Cyc_Tcpat_Con_s*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp11A=_cycalloc_atomic(sizeof(*
_tmp11A));_tmp11A[0]=({struct Cyc_Tcpat_Int_v_struct _tmp11B;_tmp11B.tag=1;_tmp11B.f1=
i;_tmp11B;});_tmp11A;}));_tmp119->arity=0;_tmp119->span=0;_tmp119;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _tagged_arr f){return({struct Cyc_Tcpat_Con_s*
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp11E;_tmp11E.tag=0;_tmp11E.f1=f;_tmp11E;});_tmp11D;}));_tmp11C->arity=0;
_tmp11C->span=0;_tmp11C;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c){
return({struct Cyc_Tcpat_Con_s*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp121=_cycalloc_atomic(sizeof(*
_tmp121));_tmp121[0]=({struct Cyc_Tcpat_Int_v_struct _tmp122;_tmp122.tag=1;_tmp122.f1=(
int)c;_tmp122;});_tmp121;}));_tmp11F->arity=0;_tmp11F->span=({struct Cyc_Core_Opt*
_tmp120=_cycalloc_atomic(sizeof(*_tmp120));_tmp120->v=(void*)256;_tmp120;});
_tmp11F;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i){return({struct
Cyc_Tcpat_Con_s*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->name=(void*)((void*)&
Cyc_Tcpat_tuple_name_value);_tmp123->arity=i;_tmp123->span=Cyc_Tcpat_one_opt_ptr;
_tmp123;});}static void*Cyc_Tcpat_null_pat(){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124[0]=({struct Cyc_Tcpat_Con_struct
_tmp125;_tmp125.tag=0;_tmp125.f1=Cyc_Tcpat_null_con;_tmp125.f2=0;_tmp125;});
_tmp124;});}static void*Cyc_Tcpat_int_pat(int i){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=({struct Cyc_Tcpat_Con_struct
_tmp127;_tmp127.tag=0;_tmp127.f1=Cyc_Tcpat_int_con(i);_tmp127.f2=0;_tmp127;});
_tmp126;});}static void*Cyc_Tcpat_char_pat(char c){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128[0]=({struct Cyc_Tcpat_Con_struct
_tmp129;_tmp129.tag=0;_tmp129.f1=Cyc_Tcpat_char_con(c);_tmp129.f2=0;_tmp129;});
_tmp128;});}static void*Cyc_Tcpat_float_pat(struct _tagged_arr f){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A[0]=({
struct Cyc_Tcpat_Con_struct _tmp12B;_tmp12B.tag=0;_tmp12B.f1=Cyc_Tcpat_float_con(f);
_tmp12B.f2=0;_tmp12B;});_tmp12A;});}static void*Cyc_Tcpat_null_ptr_pat(void*p){
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));
_tmp12C[0]=({struct Cyc_Tcpat_Con_struct _tmp12D;_tmp12D.tag=0;_tmp12D.f1=Cyc_Tcpat_null_ptr_con;
_tmp12D.f2=({struct Cyc_List_List*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->hd=(
void*)p;_tmp12E->tl=0;_tmp12E;});_tmp12D;});_tmp12C;});}static void*Cyc_Tcpat_ptr_pat(
void*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp12F=_cycalloc(sizeof(*
_tmp12F));_tmp12F[0]=({struct Cyc_Tcpat_Con_struct _tmp130;_tmp130.tag=0;_tmp130.f1=
Cyc_Tcpat_ptr_con;_tmp130.f2=({struct Cyc_List_List*_tmp131=_cycalloc(sizeof(*
_tmp131));_tmp131->hd=(void*)p;_tmp131->tl=0;_tmp131;});_tmp130;});_tmp12F;});}
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132[0]=({struct Cyc_Tcpat_Con_struct
_tmp133;_tmp133.tag=0;_tmp133.f1=Cyc_Tcpat_tuple_con(Cyc_List_length(ss));
_tmp133.f2=ss;_tmp133;});_tmp132;});}static void*Cyc_Tcpat_con_pat(struct
_tagged_arr con_name,struct Cyc_Core_Opt*span,struct Cyc_List_List*ps){struct Cyc_Tcpat_Con_s*
c=({struct Cyc_Tcpat_Con_s*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->name=(
void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137[0]=({struct Cyc_Tcpat_Name_v_struct _tmp138;_tmp138.tag=0;_tmp138.f1=
con_name;_tmp138;});_tmp137;}));_tmp136->arity=Cyc_List_length(ps);_tmp136->span=
span;_tmp136;});return(void*)({struct Cyc_Tcpat_Con_struct*_tmp134=_cycalloc(
sizeof(*_tmp134));_tmp134[0]=({struct Cyc_Tcpat_Con_struct _tmp135;_tmp135.tag=0;
_tmp135.f1=c;_tmp135.f2=ps;_tmp135;});_tmp134;});}static void*Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat*p){void*s;{void*_tmp139=(void*)p->r;void*_tmp13A;int _tmp13B;
char _tmp13C;struct _tagged_arr _tmp13D;struct Cyc_Absyn_Pat*_tmp13E;struct Cyc_Absyn_Tuniondecl*
_tmp13F;struct Cyc_Absyn_Tunionfield*_tmp140;struct Cyc_List_List*_tmp141;struct
Cyc_List_List*_tmp142;struct Cyc_List_List*_tmp143;struct Cyc_Absyn_AggrInfo
_tmp144;void*_tmp145;struct Cyc_Absyn_Aggrdecl**_tmp146;struct Cyc_Absyn_Aggrdecl*
_tmp147;struct Cyc_List_List*_tmp148;struct Cyc_Absyn_Enumdecl*_tmp149;struct Cyc_Absyn_Enumfield*
_tmp14A;void*_tmp14B;struct Cyc_Absyn_Enumfield*_tmp14C;_LLB1: if((int)_tmp139 != 0)
goto _LLB3;_LLB2: s=(void*)0;goto _LLB0;_LLB3: if(_tmp139 <= (void*)2?1:*((int*)
_tmp139)!= 0)goto _LLB5;_LLB4: s=(void*)0;goto _LLB0;_LLB5: if((int)_tmp139 != 1)goto
_LLB7;_LLB6: s=Cyc_Tcpat_null_pat();goto _LLB0;_LLB7: if(_tmp139 <= (void*)2?1:*((
int*)_tmp139)!= 1)goto _LLB9;_tmp13A=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp139)->f1;_tmp13B=((struct Cyc_Absyn_Int_p_struct*)_tmp139)->f2;_LLB8: s=Cyc_Tcpat_int_pat(
_tmp13B);goto _LLB0;_LLB9: if(_tmp139 <= (void*)2?1:*((int*)_tmp139)!= 2)goto _LLBB;
_tmp13C=((struct Cyc_Absyn_Char_p_struct*)_tmp139)->f1;_LLBA: s=Cyc_Tcpat_char_pat(
_tmp13C);goto _LLB0;_LLBB: if(_tmp139 <= (void*)2?1:*((int*)_tmp139)!= 3)goto _LLBD;
_tmp13D=((struct Cyc_Absyn_Float_p_struct*)_tmp139)->f1;_LLBC: s=Cyc_Tcpat_float_pat(
_tmp13D);goto _LLB0;_LLBD: if(_tmp139 <= (void*)2?1:*((int*)_tmp139)!= 6)goto _LLBF;
_LLBE: s=(void*)0;goto _LLB0;_LLBF: if(_tmp139 <= (void*)2?1:*((int*)_tmp139)!= 5)
goto _LLC1;_tmp13E=((struct Cyc_Absyn_Pointer_p_struct*)_tmp139)->f1;_LLC0:{void*
_tmp14D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp14E;struct Cyc_Absyn_Conref*_tmp14F;_LLD4: if(_tmp14D <= (
void*)3?1:*((int*)_tmp14D)!= 4)goto _LLD6;_tmp14E=((struct Cyc_Absyn_PointerType_struct*)
_tmp14D)->f1;_tmp14F=_tmp14E.nullable;_LLD5: {int is_nullable=0;int still_working=
1;while(still_working){void*_tmp150=(void*)_tmp14F->v;struct Cyc_Absyn_Conref*
_tmp151;int _tmp152;_LLDB: if(_tmp150 <= (void*)1?1:*((int*)_tmp150)!= 1)goto _LLDD;
_tmp151=((struct Cyc_Absyn_Forward_constr_struct*)_tmp150)->f1;_LLDC:(void*)(
_tmp14F->v=(void*)((void*)_tmp151->v));continue;_LLDD: if((int)_tmp150 != 0)goto
_LLDF;_LLDE:(void*)(_tmp14F->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp154;_tmp154.tag=0;_tmp154.f1=(void*)0;_tmp154;});_tmp153;})));is_nullable=0;
still_working=0;goto _LLDA;_LLDF: if(_tmp150 <= (void*)1?1:*((int*)_tmp150)!= 0)
goto _LLDA;_tmp152=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp150)->f1;_LLE0:
is_nullable=(int)_tmp152;still_working=0;goto _LLDA;_LLDA:;}{void*ss=Cyc_Tcpat_compile_pat(
_tmp13E);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(ss);else{s=Cyc_Tcpat_ptr_pat(ss);}
goto _LLD3;}}_LLD6: if(_tmp14D <= (void*)3?1:*((int*)_tmp14D)!= 2)goto _LLD8;_LLD7:{
void*_tmp155=(void*)_tmp13E->r;struct Cyc_Absyn_Tuniondecl*_tmp156;struct Cyc_Absyn_Tunionfield*
_tmp157;struct Cyc_List_List*_tmp158;_LLE2: if(_tmp155 <= (void*)2?1:*((int*)
_tmp155)!= 8)goto _LLE4;_tmp156=((struct Cyc_Absyn_Tunion_p_struct*)_tmp155)->f1;
_tmp157=((struct Cyc_Absyn_Tunion_p_struct*)_tmp155)->f2;_tmp158=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp155)->f3;_LLE3: {struct Cyc_Core_Opt*span;if(_tmp156->is_xtunion)span=0;else{
span=({struct Cyc_Core_Opt*_tmp159=_cycalloc_atomic(sizeof(*_tmp159));_tmp159->v=(
void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp156->fields))->v);_tmp159;});}s=Cyc_Tcpat_con_pat(*(*
_tmp157->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp158));goto _LLE1;}
_LLE4:;_LLE5:({void*_tmp15A[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("non-[x]tunion pattern has tunion type",
sizeof(char),38),_tag_arr(_tmp15A,sizeof(void*),0));});_LLE1:;}goto _LLD3;_LLD8:;
_LLD9:({void*_tmp15B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("pointer pattern does not have pointer type",sizeof(
char),43),_tag_arr(_tmp15B,sizeof(void*),0));});_LLD3:;}goto _LLB0;_LLC1: if(
_tmp139 <= (void*)2?1:*((int*)_tmp139)!= 8)goto _LLC3;_tmp13F=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp139)->f1;_tmp140=((struct Cyc_Absyn_Tunion_p_struct*)_tmp139)->f2;_tmp141=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp139)->f3;if(_tmp141 != 0)goto _LLC3;_LLC2: {
struct Cyc_Core_Opt*span;{void*_tmp15C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);_LLE7: if(_tmp15C <= (void*)3?1:*((int*)_tmp15C)!= 2)
goto _LLE9;_LLE8: if(_tmp13F->is_xtunion)span=0;else{span=({struct Cyc_Core_Opt*
_tmp15D=_cycalloc_atomic(sizeof(*_tmp15D));_tmp15D->v=(void*)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp13F->fields))->v);_tmp15D;});}goto _LLE6;_LLE9: if(_tmp15C <= (void*)3?1:*((int*)
_tmp15C)!= 3)goto _LLEB;_LLEA: span=({struct Cyc_Core_Opt*_tmp15E=_cycalloc_atomic(
sizeof(*_tmp15E));_tmp15E->v=(void*)1;_tmp15E;});goto _LLE6;_LLEB:;_LLEC: span=({
void*_tmp15F[0]={};((struct Cyc_Core_Opt*(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("void tunion pattern has bad type",
sizeof(char),33),_tag_arr(_tmp15F,sizeof(void*),0));});goto _LLE6;_LLE6:;}s=Cyc_Tcpat_con_pat(*(*
_tmp140->name).f2,span,0);goto _LLB0;}_LLC3: if(_tmp139 <= (void*)2?1:*((int*)
_tmp139)!= 8)goto _LLC5;_tmp142=((struct Cyc_Absyn_Tunion_p_struct*)_tmp139)->f3;
_LLC4: _tmp143=_tmp142;goto _LLC6;_LLC5: if(_tmp139 <= (void*)2?1:*((int*)_tmp139)!= 
4)goto _LLC7;_tmp143=((struct Cyc_Absyn_Tuple_p_struct*)_tmp139)->f1;_LLC6: s=Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp143));goto _LLB0;_LLC7: if(_tmp139 <= (void*)
2?1:*((int*)_tmp139)!= 7)goto _LLC9;_tmp144=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp139)->f1;_tmp145=(void*)_tmp144.aggr_info;if(*((int*)_tmp145)!= 1)goto _LLC9;
_tmp146=((struct Cyc_Absyn_KnownAggr_struct*)_tmp145)->f1;_tmp147=*_tmp146;
_tmp148=((struct Cyc_Absyn_Aggr_p_struct*)_tmp139)->f3;_LLC8: {struct Cyc_List_List*
ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp147->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=Cyc_Std_strcmp(*((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name,_tag_arr("",sizeof(char),1))== 0;{
struct Cyc_List_List*dlps0=_tmp148;for(0;!found?dlps0 != 0: 0;dlps0=dlps0->tl){
struct _tuple7 _tmp161;struct Cyc_List_List*_tmp162;struct Cyc_Absyn_Pat*_tmp163;
struct _tuple7*_tmp160=(struct _tuple7*)dlps0->hd;_tmp161=*_tmp160;_tmp162=_tmp161.f1;
_tmp163=_tmp161.f2;{struct Cyc_List_List*_tmp164=_tmp162;struct Cyc_List_List
_tmp165;void*_tmp166;struct _tagged_arr*_tmp167;struct Cyc_List_List*_tmp168;_LLEE:
if(_tmp164 == 0)goto _LLF0;_tmp165=*_tmp164;_tmp166=(void*)_tmp165.hd;if(*((int*)
_tmp166)!= 1)goto _LLF0;_tmp167=((struct Cyc_Absyn_FieldName_struct*)_tmp166)->f1;
_tmp168=_tmp165.tl;if(_tmp168 != 0)goto _LLF0;_LLEF: if(Cyc_Std_zstrptrcmp(_tmp167,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->hd=(void*)Cyc_Tcpat_compile_pat(
_tmp163);_tmp169->tl=ps;_tmp169;});found=1;}goto _LLED;_LLF0:;_LLF1:({void*
_tmp16A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("bad designator(s)",sizeof(char),18),_tag_arr(_tmp16A,sizeof(void*),0));});
_LLED:;}}}if(!found)({void*_tmp16B[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad designator",sizeof(char),15),
_tag_arr(_tmp16B,sizeof(void*),0));});}}s=Cyc_Tcpat_tuple_pat(ps);goto _LLB0;}
_LLC9: if(_tmp139 <= (void*)2?1:*((int*)_tmp139)!= 9)goto _LLCB;_tmp149=((struct Cyc_Absyn_Enum_p_struct*)
_tmp139)->f1;_tmp14A=((struct Cyc_Absyn_Enum_p_struct*)_tmp139)->f2;_LLCA: {int
span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp149->fields))->v);s=Cyc_Tcpat_con_pat(*(*
_tmp14A->name).f2,({struct Cyc_Core_Opt*_tmp16C=_cycalloc_atomic(sizeof(*_tmp16C));
_tmp16C->v=(void*)span;_tmp16C;}),0);goto _LLB0;}_LLCB: if(_tmp139 <= (void*)2?1:*((
int*)_tmp139)!= 10)goto _LLCD;_tmp14B=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp139)->f1;_tmp14C=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp139)->f2;_LLCC: {
struct Cyc_List_List*fields;{void*_tmp16D=Cyc_Tcutil_compress(_tmp14B);struct Cyc_List_List*
_tmp16E;_LLF3: if(_tmp16D <= (void*)3?1:*((int*)_tmp16D)!= 13)goto _LLF5;_tmp16E=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp16D)->f1;_LLF4: fields=_tmp16E;goto _LLF2;
_LLF5:;_LLF6:({void*_tmp16F[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad type in AnonEnum_p",sizeof(char),
23),_tag_arr(_tmp16F,sizeof(void*),0));});_LLF2:;}{int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*_tmp14C->name).f2,({struct Cyc_Core_Opt*
_tmp170=_cycalloc_atomic(sizeof(*_tmp170));_tmp170->v=(void*)span;_tmp170;}),0);
goto _LLB0;}}_LLCD: if(_tmp139 <= (void*)2?1:*((int*)_tmp139)!= 11)goto _LLCF;_LLCE:({
void*_tmp171[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("compile_pat: unknownid",sizeof(char),23),_tag_arr(_tmp171,sizeof(void*),
0));});_LLCF: if(_tmp139 <= (void*)2?1:*((int*)_tmp139)!= 12)goto _LLD1;_LLD0:({
void*_tmp172[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("compile_pat: unknowncall",sizeof(char),25),_tag_arr(_tmp172,sizeof(
void*),0));});_LLD1: if(_tmp139 <= (void*)2?1:*((int*)_tmp139)!= 7)goto _LLB0;_LLD2:({
void*_tmp173[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("compile_pat: bad aggr",sizeof(char),22),_tag_arr(_tmp173,sizeof(void*),
0));});_LLB0:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int tag;struct Cyc_Set_Set*
f1;};struct Cyc_Tcpat_Success_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct
_tuple10{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c){void*_tmp174=td;struct Cyc_Set_Set*_tmp175;_LLF8: if(*((int*)_tmp174)!= 1)goto
_LLFA;_tmp175=((struct Cyc_Tcpat_Neg_struct*)_tmp174)->f1;_LLF9: if(((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp175,c))({void*
_tmp176[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("add_neg called when constructor already in set",sizeof(char),47),
_tag_arr(_tmp176,sizeof(void*),0));});if(c->span != 0?((int(*)(struct Cyc_Set_Set*
s))Cyc_Set_cardinality)(_tmp175)+ 1 >= (int)((struct Cyc_Core_Opt*)_check_null(c->span))->v:
0)({void*_tmp177[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("add_neg called when |cs U {c}| >= span(c)",sizeof(char),42),_tag_arr(
_tmp177,sizeof(void*),0));});return(void*)({struct Cyc_Tcpat_Neg_struct*_tmp178=
_region_malloc(r,sizeof(*_tmp178));_tmp178[0]=({struct Cyc_Tcpat_Neg_struct
_tmp179;_tmp179.tag=1;_tmp179.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp175,c);
_tmp179;});_tmp178;});_LLFA: if(*((int*)_tmp174)!= 0)goto _LLF7;_LLFB:({void*
_tmp17A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("add_neg called when td is Positive",sizeof(char),35),_tag_arr(_tmp17A,
sizeof(void*),0));});_LLF7:;}static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*
c,void*td){void*_tmp17B=td;struct Cyc_Tcpat_Con_s*_tmp17C;struct Cyc_Set_Set*
_tmp17D;_LLFD: if(*((int*)_tmp17B)!= 0)goto _LLFF;_tmp17C=((struct Cyc_Tcpat_Pos_struct*)
_tmp17B)->f1;_LLFE: if(Cyc_Tcpat_compare_con(c,_tmp17C)== 0)return(void*)0;else{
return(void*)1;}_LLFF: if(*((int*)_tmp17B)!= 1)goto _LLFC;_tmp17D=((struct Cyc_Tcpat_Neg_struct*)
_tmp17B)->f1;_LL100: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp17D,c))return(void*)1;else{if(c->span != 0?(int)((struct Cyc_Core_Opt*)
_check_null(c->span))->v == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp17D)+ 1: 0)return(void*)0;else{return(void*)2;}}_LLFC:;}struct _tuple11{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*
_tmp17E=ctxt;struct Cyc_List_List _tmp17F;struct _tuple11*_tmp180;struct _tuple11
_tmp181;struct Cyc_Tcpat_Con_s*_tmp182;struct Cyc_List_List*_tmp183;struct Cyc_List_List*
_tmp184;_LL102: if(_tmp17E != 0)goto _LL104;_LL103: return 0;_LL104: if(_tmp17E == 0)
goto _LL101;_tmp17F=*_tmp17E;_tmp180=(struct _tuple11*)_tmp17F.hd;_tmp181=*_tmp180;
_tmp182=_tmp181.f1;_tmp183=_tmp181.f2;_tmp184=_tmp17F.tl;_LL105: return({struct
Cyc_List_List*_tmp185=_region_malloc(r,sizeof(*_tmp185));_tmp185->hd=({struct
_tuple11*_tmp186=_region_malloc(r,sizeof(*_tmp186));_tmp186->f1=_tmp182;_tmp186->f2=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp187=_region_malloc(r,sizeof(*
_tmp187));_tmp187->hd=(void*)dsc;_tmp187->tl=_tmp183;_tmp187;});_tmp186;});
_tmp185->tl=_tmp184;_tmp185;});_LL101:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp188=ctxt;
struct Cyc_List_List _tmp189;struct _tuple11*_tmp18A;struct _tuple11 _tmp18B;struct
Cyc_Tcpat_Con_s*_tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_List_List*_tmp18E;
_LL107: if(_tmp188 != 0)goto _LL109;_LL108:({void*_tmp18F[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("norm_context: empty context",
sizeof(char),28),_tag_arr(_tmp18F,sizeof(void*),0));});_LL109: if(_tmp188 == 0)
goto _LL106;_tmp189=*_tmp188;_tmp18A=(struct _tuple11*)_tmp189.hd;_tmp18B=*_tmp18A;
_tmp18C=_tmp18B.f1;_tmp18D=_tmp18B.f2;_tmp18E=_tmp189.tl;_LL10A: return Cyc_Tcpat_augment(
r,_tmp18E,(void*)({struct Cyc_Tcpat_Pos_struct*_tmp190=_region_malloc(r,sizeof(*
_tmp190));_tmp190[0]=({struct Cyc_Tcpat_Pos_struct _tmp191;_tmp191.tag=0;_tmp191.f1=
_tmp18C;_tmp191.f2=Cyc_List_rrev(r,_tmp18D);_tmp191;});_tmp190;}));_LL106:;}
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,
void*dsc,struct Cyc_List_List*work){struct _tuple6 _tmp193=({struct _tuple6 _tmp192;
_tmp192.f1=ctxt;_tmp192.f2=work;_tmp192;});struct Cyc_List_List*_tmp194;struct Cyc_List_List*
_tmp195;struct Cyc_List_List*_tmp196;struct Cyc_List_List*_tmp197;struct Cyc_List_List*
_tmp198;struct Cyc_List_List _tmp199;struct _tuple11*_tmp19A;struct _tuple11 _tmp19B;
struct Cyc_Tcpat_Con_s*_tmp19C;struct Cyc_List_List*_tmp19D;struct Cyc_List_List*
_tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_List_List _tmp1A0;struct _tuple10*
_tmp1A1;struct _tuple10 _tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_List_List*
_tmp1A4;_LL10C: _tmp194=_tmp193.f1;if(_tmp194 != 0)goto _LL10E;_tmp195=_tmp193.f2;
if(_tmp195 != 0)goto _LL10E;_LL10D: return dsc;_LL10E: _tmp196=_tmp193.f1;if(_tmp196
!= 0)goto _LL110;_LL10F: goto _LL111;_LL110: _tmp197=_tmp193.f2;if(_tmp197 != 0)goto
_LL112;_LL111:({void*_tmp1A5[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("build_desc: ctxt and work don't match",
sizeof(char),38),_tag_arr(_tmp1A5,sizeof(void*),0));});_LL112: _tmp198=_tmp193.f1;
if(_tmp198 == 0)goto _LL10B;_tmp199=*_tmp198;_tmp19A=(struct _tuple11*)_tmp199.hd;
_tmp19B=*_tmp19A;_tmp19C=_tmp19B.f1;_tmp19D=_tmp19B.f2;_tmp19E=_tmp199.tl;
_tmp19F=_tmp193.f2;if(_tmp19F == 0)goto _LL10B;_tmp1A0=*_tmp19F;_tmp1A1=(struct
_tuple10*)_tmp1A0.hd;_tmp1A2=*_tmp1A1;_tmp1A3=_tmp1A2.f3;_tmp1A4=_tmp1A0.tl;
_LL113: {struct Cyc_Tcpat_Pos_struct*_tmp1A6=({struct Cyc_Tcpat_Pos_struct*_tmp1A7=
_region_malloc(r,sizeof(*_tmp1A7));_tmp1A7[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1A8;_tmp1A8.tag=0;_tmp1A8.f1=_tmp19C;_tmp1A8.f2=Cyc_List_rappend(r,Cyc_List_rrev(
r,_tmp19D),({struct Cyc_List_List*_tmp1A9=_region_malloc(r,sizeof(*_tmp1A9));
_tmp1A9->hd=(void*)dsc;_tmp1A9->tl=_tmp1A3;_tmp1A9;}));_tmp1A8;});_tmp1A7;});
return Cyc_Tcpat_build_desc(r,_tmp19E,(void*)_tmp1A6,_tmp1A4);}_LL10B:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*,void*,struct Cyc_List_List*,void*,
struct Cyc_List_List*,struct Cyc_List_List*,void*,struct Cyc_List_List*);static void*
Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp1AA=allmrules;struct Cyc_List_List _tmp1AB;struct _tuple0*
_tmp1AC;struct _tuple0 _tmp1AD;void*_tmp1AE;void*_tmp1AF;struct Cyc_List_List*
_tmp1B0;_LL115: if(_tmp1AA != 0)goto _LL117;_LL116: return(void*)0;_LL117: if(_tmp1AA
== 0)goto _LL114;_tmp1AB=*_tmp1AA;_tmp1AC=(struct _tuple0*)_tmp1AB.hd;_tmp1AD=*
_tmp1AC;_tmp1AE=_tmp1AD.f1;_tmp1AF=_tmp1AD.f2;_tmp1B0=_tmp1AB.tl;_LL118: return
Cyc_Tcpat_match(r,_tmp1AE,0,dsc,0,0,_tmp1AF,_tmp1B0);_LL114:;}static void*Cyc_Tcpat_match_compile(
struct _RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(
void*)({struct Cyc_Tcpat_Neg_struct*_tmp1B1=_region_malloc(r,sizeof(*_tmp1B1));
_tmp1B1[0]=({struct Cyc_Tcpat_Neg_struct _tmp1B2;_tmp1B2.tag=1;_tmp1B2.f1=Cyc_Tcpat_empty_con_set(
r);_tmp1B2;});_tmp1B1;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp1B3=work;
struct Cyc_List_List _tmp1B4;struct _tuple10*_tmp1B5;struct _tuple10 _tmp1B6;struct
Cyc_List_List*_tmp1B7;struct Cyc_List_List*_tmp1B8;struct Cyc_List_List*_tmp1B9;
struct Cyc_List_List*_tmp1BA;struct Cyc_List_List _tmp1BB;struct _tuple10*_tmp1BC;
struct _tuple10 _tmp1BD;struct Cyc_List_List*_tmp1BE;struct Cyc_List_List*_tmp1BF;
struct Cyc_List_List*_tmp1C0;struct Cyc_List_List*_tmp1C1;_LL11A: if(_tmp1B3 != 0)
goto _LL11C;_LL11B: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp1C2=
_region_malloc(r,sizeof(*_tmp1C2));_tmp1C2[0]=({struct Cyc_Tcpat_Success_struct
_tmp1C3;_tmp1C3.tag=0;_tmp1C3.f1=(void*)right_hand_side;_tmp1C3;});_tmp1C2;});
_LL11C: if(_tmp1B3 == 0)goto _LL11E;_tmp1B4=*_tmp1B3;_tmp1B5=(struct _tuple10*)
_tmp1B4.hd;_tmp1B6=*_tmp1B5;_tmp1B7=_tmp1B6.f1;if(_tmp1B7 != 0)goto _LL11E;_tmp1B8=
_tmp1B6.f2;if(_tmp1B8 != 0)goto _LL11E;_tmp1B9=_tmp1B6.f3;if(_tmp1B9 != 0)goto
_LL11E;_tmp1BA=_tmp1B4.tl;_LL11D: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp1BA,right_hand_side,rules);_LL11E: if(_tmp1B3 == 0)goto _LL119;_tmp1BB=*
_tmp1B3;_tmp1BC=(struct _tuple10*)_tmp1BB.hd;_tmp1BD=*_tmp1BC;_tmp1BE=_tmp1BD.f1;
_tmp1BF=_tmp1BD.f2;_tmp1C0=_tmp1BD.f3;_tmp1C1=_tmp1BB.tl;_LL11F: if((_tmp1BE == 0?
1: _tmp1BF == 0)?1: _tmp1C0 == 0)({void*_tmp1C4[0]={};Cyc_Tcutil_impos(_tag_arr("tcpat:and_match: malformed work frame",
sizeof(char),38),_tag_arr(_tmp1C4,sizeof(void*),0));});{struct Cyc_List_List
_tmp1C6;void*_tmp1C7;struct Cyc_List_List*_tmp1C8;struct Cyc_List_List*_tmp1C5=(
struct Cyc_List_List*)_check_null(_tmp1BE);_tmp1C6=*_tmp1C5;_tmp1C7=(void*)
_tmp1C6.hd;_tmp1C8=_tmp1C6.tl;{struct Cyc_List_List _tmp1CA;struct Cyc_List_List*
_tmp1CB;struct Cyc_List_List*_tmp1CC;struct Cyc_List_List*_tmp1C9=(struct Cyc_List_List*)
_check_null(_tmp1BF);_tmp1CA=*_tmp1C9;_tmp1CB=(struct Cyc_List_List*)_tmp1CA.hd;
_tmp1CC=_tmp1CA.tl;{struct Cyc_List_List _tmp1CE;void*_tmp1CF;struct Cyc_List_List*
_tmp1D0;struct Cyc_List_List*_tmp1CD=(struct Cyc_List_List*)_check_null(_tmp1C0);
_tmp1CE=*_tmp1CD;_tmp1CF=(void*)_tmp1CE.hd;_tmp1D0=_tmp1CE.tl;{struct _tuple10*
_tmp1D1=({struct _tuple10*_tmp1D3=_region_malloc(r,sizeof(*_tmp1D3));_tmp1D3->f1=
_tmp1C8;_tmp1D3->f2=_tmp1CC;_tmp1D3->f3=_tmp1D0;_tmp1D3;});return Cyc_Tcpat_match(
r,_tmp1C7,_tmp1CB,_tmp1CF,ctx,({struct Cyc_List_List*_tmp1D2=_region_malloc(r,
sizeof(*_tmp1D2));_tmp1D2->hd=_tmp1D1;_tmp1D2->tl=_tmp1C1;_tmp1D2;}),
right_hand_side,rules);}}}}_LL119:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp1D4=dsc;
struct Cyc_Set_Set*_tmp1D5;struct Cyc_List_List*_tmp1D6;_LL121: if(*((int*)_tmp1D4)
!= 1)goto _LL123;_tmp1D5=((struct Cyc_Tcpat_Neg_struct*)_tmp1D4)->f1;_LL122: {void*
any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));
_tmp1D9[0]=({struct Cyc_Tcpat_Neg_struct _tmp1DA;_tmp1DA.tag=1;_tmp1DA.f1=Cyc_Tcpat_empty_con_set(
r);_tmp1DA;});_tmp1D9;});struct Cyc_List_List*_tmp1D7=0;{int i=0;for(0;i < pcon->arity;
++ i){_tmp1D7=({struct Cyc_List_List*_tmp1D8=_region_malloc(r,sizeof(*_tmp1D8));
_tmp1D8->hd=(void*)any;_tmp1D8->tl=_tmp1D7;_tmp1D8;});}}return _tmp1D7;}_LL123:
if(*((int*)_tmp1D4)!= 0)goto _LL120;_tmp1D6=((struct Cyc_Tcpat_Pos_struct*)_tmp1D4)->f2;
_LL124: return _tmp1D6;_LL120:;}struct _tuple12{struct _RegionHandle*f1;struct Cyc_List_List*
f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple12*env,int i){struct
_tuple12 _tmp1DC;struct _RegionHandle*_tmp1DD;struct Cyc_List_List*_tmp1DE;struct
_tuple12*_tmp1DB=env;_tmp1DC=*_tmp1DB;_tmp1DD=_tmp1DC.f1;_tmp1DE=_tmp1DC.f2;
return({struct Cyc_List_List*_tmp1DF=_region_malloc(_tmp1DD,sizeof(*_tmp1DF));
_tmp1DF->hd=(void*)(i + 1);_tmp1DF->tl=_tmp1DE;_tmp1DF;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*
obj){struct _tuple12 _tmp1E0=({struct _tuple12 _tmp1E1;_tmp1E1.f1=r;_tmp1E1.f2=obj;
_tmp1E1;});return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple12*,int),struct _tuple12*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp1E0);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp1E2=p;struct Cyc_Tcpat_Con_s*
_tmp1E3;struct Cyc_List_List*_tmp1E4;_LL126: if((int)_tmp1E2 != 0)goto _LL128;_LL127:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL128: if(_tmp1E2 <= (void*)1?1:*((int*)_tmp1E2)!= 0)goto _LL125;_tmp1E3=((
struct Cyc_Tcpat_Con_struct*)_tmp1E2)->f1;_tmp1E4=((struct Cyc_Tcpat_Con_struct*)
_tmp1E2)->f2;_LL129: {void*_tmp1E5=Cyc_Tcpat_static_match(_tmp1E3,dsc);_LL12B:
if((int)_tmp1E5 != 0)goto _LL12D;_LL12C: {struct Cyc_List_List*_tmp1E6=({struct Cyc_List_List*
_tmp1EB=_region_malloc(r,sizeof(*_tmp1EB));_tmp1EB->hd=({struct _tuple11*_tmp1EC=
_region_malloc(r,sizeof(*_tmp1EC));_tmp1EC->f1=_tmp1E3;_tmp1EC->f2=0;_tmp1EC;});
_tmp1EB->tl=ctx;_tmp1EB;});struct _tuple10*_tmp1E7=({struct _tuple10*_tmp1EA=
_region_malloc(r,sizeof(*_tmp1EA));_tmp1EA->f1=_tmp1E4;_tmp1EA->f2=Cyc_Tcpat_getoargs(
r,_tmp1E3,obj);_tmp1EA->f3=Cyc_Tcpat_getdargs(r,_tmp1E3,dsc);_tmp1EA;});struct
Cyc_List_List*_tmp1E8=({struct Cyc_List_List*_tmp1E9=_region_malloc(r,sizeof(*
_tmp1E9));_tmp1E9->hd=_tmp1E7;_tmp1E9->tl=work;_tmp1E9;});return Cyc_Tcpat_and_match(
r,_tmp1E6,_tmp1E8,right_hand_side,rules);}_LL12D: if((int)_tmp1E5 != 1)goto _LL12F;
_LL12E: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL12F: if((int)_tmp1E5 != 2)goto _LL12A;_LL130: {struct Cyc_List_List*_tmp1ED=({
struct Cyc_List_List*_tmp1F6=_region_malloc(r,sizeof(*_tmp1F6));_tmp1F6->hd=({
struct _tuple11*_tmp1F7=_region_malloc(r,sizeof(*_tmp1F7));_tmp1F7->f1=_tmp1E3;
_tmp1F7->f2=0;_tmp1F7;});_tmp1F6->tl=ctx;_tmp1F6;});struct _tuple10*_tmp1EE=({
struct _tuple10*_tmp1F5=_region_malloc(r,sizeof(*_tmp1F5));_tmp1F5->f1=_tmp1E4;
_tmp1F5->f2=Cyc_Tcpat_getoargs(r,_tmp1E3,obj);_tmp1F5->f3=Cyc_Tcpat_getdargs(r,
_tmp1E3,dsc);_tmp1F5;});struct Cyc_List_List*_tmp1EF=({struct Cyc_List_List*
_tmp1F4=_region_malloc(r,sizeof(*_tmp1F4));_tmp1F4->hd=_tmp1EE;_tmp1F4->tl=work;
_tmp1F4;});void*_tmp1F0=Cyc_Tcpat_and_match(r,_tmp1ED,_tmp1EF,right_hand_side,
rules);void*_tmp1F1=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(
r,dsc,_tmp1E3),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp1F2=
_region_malloc(r,sizeof(*_tmp1F2));_tmp1F2[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp1F3;_tmp1F3.tag=1;_tmp1F3.f1=obj;_tmp1F3.f2=_tmp1E3;_tmp1F3.f3=(void*)
_tmp1F0;_tmp1F3.f4=(void*)_tmp1F1;_tmp1F3;});_tmp1F2;});}_LL12A:;}_LL125:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*),void*
env1,void(*rhs_appears)(void*,void*),void*env2){void*_tmp1F8=d;void*_tmp1F9;void*
_tmp1FA;void*_tmp1FB;_LL132: if((int)_tmp1F8 != 0)goto _LL134;_LL133: not_exhaust(
env1);goto _LL131;_LL134: if(_tmp1F8 <= (void*)1?1:*((int*)_tmp1F8)!= 0)goto _LL136;
_tmp1F9=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp1F8)->f1;_LL135: rhs_appears(
env2,_tmp1F9);goto _LL131;_LL136: if(_tmp1F8 <= (void*)1?1:*((int*)_tmp1F8)!= 1)
goto _LL131;_tmp1FA=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp1F8)->f3;_tmp1FB=(
void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp1F8)->f4;_LL137: Cyc_Tcpat_check_exhaust_overlap(
_tmp1FA,not_exhaust,env1,rhs_appears,env2);Cyc_Tcpat_check_exhaust_overlap(
_tmp1FB,not_exhaust,env1,rhs_appears,env2);goto _LL131;_LL131:;}struct _tuple13{
int f1;struct Cyc_Position_Segment*f2;};struct _tuple14{void*f1;struct _tuple13*f2;}
;struct _tuple15{struct _RegionHandle*f1;int*f2;};static struct _tuple14*Cyc_Tcpat_get_match(
struct _tuple15*env,struct Cyc_Absyn_Switch_clause*swc){struct _tuple15 _tmp1FD;
struct _RegionHandle*_tmp1FE;int*_tmp1FF;struct _tuple15*_tmp1FC=env;_tmp1FD=*
_tmp1FC;_tmp1FE=_tmp1FD.f1;_tmp1FF=_tmp1FD.f2;{void*sp0=Cyc_Tcpat_compile_pat(
swc->pattern);struct _tuple13*rhs=({struct _tuple13*_tmp205=_region_malloc(_tmp1FE,
sizeof(*_tmp205));_tmp205->f1=0;_tmp205->f2=(swc->pattern)->loc;_tmp205;});void*
sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*
_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->hd=(void*)sp0;_tmp200->tl=({struct
Cyc_List_List*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->hd=(void*)Cyc_Tcpat_int_pat(*
_tmp1FF);_tmp201->tl=0;_tmp201;});_tmp200;}));*_tmp1FF=*_tmp1FF + 1;}else{sp=Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->hd=(void*)sp0;
_tmp202->tl=({struct Cyc_List_List*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203->hd=(
void*)((void*)0);_tmp203->tl=0;_tmp203;});_tmp202;}));}return({struct _tuple14*
_tmp204=_region_malloc(_tmp1FE,sizeof(*_tmp204));_tmp204->f1=sp;_tmp204->f2=rhs;
_tmp204;});}}static void Cyc_Tcpat_not_exhaust_err(struct Cyc_Position_Segment*loc){({
void*_tmp206[0]={};Cyc_Tcutil_terr(loc,_tag_arr("patterns may not be exhaustive",
sizeof(char),31),_tag_arr(_tmp206,sizeof(void*),0));});}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple13*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*swcs){struct _RegionHandle
_tmp207=_new_region("r");struct _RegionHandle*r=& _tmp207;_push_region(r);{int
_tmp208=0;struct _tuple15 _tmp209=({struct _tuple15 _tmp213;_tmp213.f1=r;_tmp213.f2=&
_tmp208;_tmp213;});struct Cyc_List_List*_tmp20A=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple14*(*f)(struct _tuple15*,struct Cyc_Absyn_Switch_clause*),
struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,&
_tmp209,swcs);void*_tmp20B=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(r,_tmp20A);((void(*)(void*d,void(*
not_exhaust)(struct Cyc_Position_Segment*),struct Cyc_Position_Segment*env1,void(*
rhs_appears)(int,struct _tuple13*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp20B,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp20A != 0;
_tmp20A=_tmp20A->tl){struct _tuple14 _tmp20D;struct _tuple13*_tmp20E;struct _tuple13
_tmp20F;int _tmp210;struct Cyc_Position_Segment*_tmp211;struct _tuple14*_tmp20C=(
struct _tuple14*)_tmp20A->hd;_tmp20D=*_tmp20C;_tmp20E=_tmp20D.f2;_tmp20F=*_tmp20E;
_tmp210=_tmp20F.f1;_tmp211=_tmp20F.f2;if(!_tmp210)({void*_tmp212[0]={};Cyc_Tcutil_terr(
_tmp211,_tag_arr("redundant pattern",sizeof(char),18),_tag_arr(_tmp212,sizeof(
void*),0));});}};_pop_region(r);}struct _tuple16{struct Cyc_Position_Segment*f1;
int f2;};static void Cyc_Tcpat_not_exhaust_warn(struct _tuple16*pr){(*pr).f2=0;({
void*_tmp214[0]={};Cyc_Tcutil_warn((*pr).f1,_tag_arr("pattern not exhaustive",
sizeof(char),23),_tag_arr(_tmp214,sizeof(void*),0));});}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple17{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp215=
_new_region("r");struct _RegionHandle*r=& _tmp215;_push_region(r);{struct Cyc_List_List*
_tmp216=({struct Cyc_List_List*_tmp21B=_region_malloc(r,sizeof(*_tmp21B));_tmp21B->hd=({
struct _tuple17*_tmp21C=_region_malloc(r,sizeof(*_tmp21C));_tmp21C->f1=Cyc_Tcpat_compile_pat(
p);_tmp21C->f2=0;_tmp21C;});_tmp21B->tl=0;_tmp21B;});void*_tmp217=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp216);struct _tuple16 _tmp218=({struct _tuple16 _tmp21A;_tmp21A.f1=loc;_tmp21A.f2=
1;_tmp21A;});((void(*)(void*d,void(*not_exhaust)(struct _tuple16*),struct _tuple16*
env1,void(*rhs_appears)(int,int),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp217,Cyc_Tcpat_not_exhaust_warn,& _tmp218,Cyc_Tcpat_dummy_fn,0);{int _tmp219=
_tmp218.f2;_npop_handler(0);return _tmp219;}};_pop_region(r);}static struct
_tuple14*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){void*sp0=Cyc_Tcpat_compile_pat(
swc->pattern);struct _tuple13*rhs=({struct _tuple13*_tmp21E=_cycalloc(sizeof(*
_tmp21E));_tmp21E->f1=0;_tmp21E->f2=(swc->pattern)->loc;_tmp21E;});return({
struct _tuple14*_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D->f1=sp0;_tmp21D->f2=
rhs;_tmp21D;});}static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment*
loc){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,struct
Cyc_List_List*swcs){struct Cyc_List_List*_tmp21F=((struct Cyc_List_List*(*)(struct
_tuple14*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp220=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(Cyc_Core_heap_region,_tmp21F);((
void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),struct Cyc_Position_Segment*
env1,void(*rhs_appears)(int,struct _tuple13*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp220,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp21F != 
0;_tmp21F=_tmp21F->tl){struct _tuple14 _tmp222;struct _tuple13*_tmp223;struct
_tuple13 _tmp224;int _tmp225;struct Cyc_Position_Segment*_tmp226;struct _tuple14*
_tmp221=(struct _tuple14*)_tmp21F->hd;_tmp222=*_tmp221;_tmp223=_tmp222.f2;_tmp224=*
_tmp223;_tmp225=_tmp224.f1;_tmp226=_tmp224.f2;if(!_tmp225)({void*_tmp227[0]={};
Cyc_Tcutil_terr(_tmp226,_tag_arr("redundant pattern",sizeof(char),18),_tag_arr(
_tmp227,sizeof(void*),0));});}}
