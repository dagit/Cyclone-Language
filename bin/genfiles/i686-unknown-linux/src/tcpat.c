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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(
struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
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
Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct _tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
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
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_coerceable(void*);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct
Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_unify(
void*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);struct _tuple4{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct
_tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_r_make_inst_var(
struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _tagged_arr err_msg);struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields);struct _tuple6{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple6*
tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_TcPatResult
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
struct Cyc_Tcpat_TcPatResult _tmpF7;_tmpF7.tvars_and_bounds_opt=0;_tmpF7.patvars=0;
_tmpF7;});{void*_tmp59=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp5A;struct Cyc_Absyn_Vardecl*
_tmp5B;void*_tmp5C;void*_tmp5D;struct Cyc_Absyn_Enumdecl*_tmp5E;void*_tmp5F;
struct Cyc_Absyn_Pat*_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_Absyn_AggrInfo
_tmp62;void*_tmp63;struct Cyc_Absyn_Aggrdecl**_tmp64;struct Cyc_Absyn_Aggrdecl*
_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_List_List**_tmp67;struct Cyc_List_List*
_tmp68;struct Cyc_List_List*_tmp69;struct Cyc_Absyn_Tuniondecl*_tmp6A;struct Cyc_Absyn_Tunionfield*
_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_Absyn_AggrInfo _tmp6D;void*_tmp6E;
_LL3B: if((int)_tmp59 != 0)goto _LL3D;_LL3C: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);goto _LL3A;_LL3D: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 0)goto _LL3F;
_tmp5A=((struct Cyc_Absyn_Var_p_struct*)_tmp59)->f1;_LL3E: t=Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars(te),topt);Cyc_Tcpat_set_vd(_tmp5A,& res.patvars,t);
goto _LL3A;_LL3F: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 6)goto _LL41;_tmp5B=((
struct Cyc_Absyn_Reference_p_struct*)_tmp59)->f1;_LL40: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);if(rgn_opt == 0){({void*_tmp6F[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("* pattern would point into an unknown/unallowed region",
sizeof(char),55),_tag_arr(_tmp6F,sizeof(void*),0));});goto _LL3A;}Cyc_Tcpat_set_vd(
_tmp5B,& res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*_tmp70=_cycalloc(
sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Absyn_PointerType_struct _tmp71;_tmp71.tag=
4;_tmp71.f1=({struct Cyc_Absyn_PtrInfo _tmp72;_tmp72.elt_typ=(void*)t;_tmp72.rgn_typ=(
void*)*rgn_opt;_tmp72.nullable=((struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(
0);_tmp72.tq=Cyc_Absyn_empty_tqual();_tmp72.bounds=Cyc_Absyn_empty_conref();
_tmp72;});_tmp71;});_tmp70;}));goto _LL3A;_LL41: if(_tmp59 <= (void*)2?1:*((int*)
_tmp59)!= 1)goto _LL43;_tmp5C=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp59)->f1;
if((int)_tmp5C != 1)goto _LL43;_LL42: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);
goto _LL3A;_LL43: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 1)goto _LL45;_tmp5D=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp59)->f1;if((int)_tmp5D != 0)goto _LL45;
_LL44: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL3A;_LL45: if(_tmp59 <= (
void*)2?1:*((int*)_tmp59)!= 2)goto _LL47;_LL46: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uchar_typ);
goto _LL3A;_LL47: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 3)goto _LL49;_LL48: t=Cyc_Tcpat_num_type(
topt,Cyc_Absyn_float_typ);goto _LL3A;_LL49: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)
!= 9)goto _LL4B;_tmp5E=((struct Cyc_Absyn_Enum_p_struct*)_tmp59)->f1;_LL4A: t=Cyc_Tcpat_num_type(
topt,(void*)({struct Cyc_Absyn_EnumType_struct*_tmp73=_cycalloc(sizeof(*_tmp73));
_tmp73[0]=({struct Cyc_Absyn_EnumType_struct _tmp74;_tmp74.tag=12;_tmp74.f1=_tmp5E->name;
_tmp74.f2=(struct Cyc_Absyn_Enumdecl*)_tmp5E;_tmp74;});_tmp73;}));goto _LL3A;_LL4B:
if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 10)goto _LL4D;_tmp5F=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp59)->f1;_LL4C: t=Cyc_Tcpat_num_type(topt,_tmp5F);goto _LL3A;_LL4D: if((int)
_tmp59 != 1)goto _LL4F;_LL4E: if(topt != 0){void*_tmp75=Cyc_Tcutil_compress(*topt);
_LL5E: if(_tmp75 <= (void*)3?1:*((int*)_tmp75)!= 4)goto _LL60;_LL5F: t=*topt;goto
tcpat_end;_LL60:;_LL61: goto _LL5D;_LL5D:;}{struct Cyc_List_List*_tmp76=Cyc_Tcenv_lookup_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp77=_cycalloc(sizeof(*
_tmp77));_tmp77[0]=({struct Cyc_Absyn_PointerType_struct _tmp78;_tmp78.tag=4;
_tmp78.f1=({struct Cyc_Absyn_PtrInfo _tmp79;_tmp79.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp7B=_cycalloc(sizeof(*
_tmp7B));_tmp7B->v=_tmp76;_tmp7B;}));_tmp79.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp7A=_cycalloc(sizeof(*
_tmp7A));_tmp7A->v=_tmp76;_tmp7A;}));_tmp79.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmp79.tq=Cyc_Absyn_empty_tqual();_tmp79.bounds=
Cyc_Absyn_empty_conref();_tmp79;});_tmp78;});_tmp77;});goto _LL3A;}_LL4F: if(
_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 5)goto _LL51;_tmp60=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp59)->f1;_LL50: {void*inner_typ=(void*)0;void**_tmp7C=0;if(topt != 0){void*
_tmp7D=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp7E;void*_tmp7F;
_LL63: if(_tmp7D <= (void*)3?1:*((int*)_tmp7D)!= 4)goto _LL65;_tmp7E=((struct Cyc_Absyn_PointerType_struct*)
_tmp7D)->f1;_tmp7F=(void*)_tmp7E.elt_typ;_LL64: inner_typ=_tmp7F;_tmp7C=(void**)&
inner_typ;goto _LL62;_LL65:;_LL66: goto _LL62;_LL62:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp92=_cycalloc(sizeof(*
_tmp92));_tmp92->v=Cyc_Tcenv_lookup_type_vars(te);_tmp92;}));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp60,_tmp7C,(void**)& ptr_rgn));if(_tmp7C == 0){void*
_tmp80=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp60->topt))->v);
struct Cyc_Absyn_TunionFieldInfo _tmp81;void*_tmp82;struct Cyc_Absyn_Tuniondecl*
_tmp83;struct Cyc_Absyn_Tunionfield*_tmp84;struct Cyc_List_List*_tmp85;_LL68: if(
_tmp80 <= (void*)3?1:*((int*)_tmp80)!= 3)goto _LL6A;_tmp81=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp80)->f1;_tmp82=(void*)_tmp81.field_info;if(*((int*)_tmp82)!= 1)goto _LL6A;
_tmp83=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp82)->f1;_tmp84=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp82)->f2;_tmp85=_tmp81.targs;_LL69: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({struct Cyc_Absyn_TunionType_struct
_tmp87;_tmp87.tag=2;_tmp87.f1=({struct Cyc_Absyn_TunionInfo _tmp88;_tmp88.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmp89=_cycalloc(sizeof(*
_tmp89));_tmp89[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp8A;_tmp8A.tag=1;
_tmp8A.f1=({struct Cyc_Absyn_Tuniondecl**_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[
0]=_tmp83;_tmp8B;});_tmp8A;});_tmp89;}));_tmp88.targs=_tmp85;_tmp88.rgn=(void*)
ptr_rgn;_tmp88;});_tmp87;});_tmp86;});goto _LL67;_LL6A:;_LL6B: t=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({
struct Cyc_Absyn_PointerType_struct _tmp8D;_tmp8D.tag=4;_tmp8D.f1=({struct Cyc_Absyn_PtrInfo
_tmp8E;_tmp8E.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp60->topt))->v);
_tmp8E.rgn_typ=(void*)ptr_rgn;_tmp8E.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp8E.tq=Cyc_Absyn_empty_tqual();_tmp8E.bounds=Cyc_Absyn_empty_conref();_tmp8E;});
_tmp8D;});_tmp8C;});goto _LL67;_LL67:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_PointerType_struct
_tmp90;_tmp90.tag=4;_tmp90.f1=({struct Cyc_Absyn_PtrInfo _tmp91;_tmp91.elt_typ=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp60->topt))->v);_tmp91.rgn_typ=(
void*)ptr_rgn;_tmp91.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp91.tq=Cyc_Absyn_empty_tqual();_tmp91.bounds=Cyc_Absyn_empty_conref();_tmp91;});
_tmp90;});_tmp8F;});}goto _LL3A;}}_LL51: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 
4)goto _LL53;_tmp61=((struct Cyc_Absyn_Tuple_p_struct*)_tmp59)->f1;_LL52: {struct
Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(topt != 0){void*_tmp93=
Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp94;_LL6D: if(_tmp93 <= (void*)3?
1:*((int*)_tmp93)!= 9)goto _LL6F;_tmp94=((struct Cyc_Absyn_TupleType_struct*)
_tmp93)->f1;_LL6E: topt_ts=_tmp94;goto _LL6C;_LL6F:;_LL70: goto _LL6C;_LL6C:;}for(0;
_tmp61 != 0;_tmp61=_tmp61->tl){void**_tmp95=0;if(topt_ts != 0){_tmp95=(void**)&(*((
struct _tuple8*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp61->hd,_tmp95,rgn_opt));
pat_ts=({struct Cyc_List_List*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->hd=({
struct _tuple8*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->f1=Cyc_Absyn_empty_tqual();
_tmp97->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)
_tmp61->hd)->topt))->v;_tmp97;});_tmp96->tl=pat_ts;_tmp96;});}t=(void*)({struct
Cyc_Absyn_TupleType_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({struct
Cyc_Absyn_TupleType_struct _tmp99;_tmp99.tag=9;_tmp99.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmp99;});_tmp98;});goto _LL3A;}
_LL53: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 7)goto _LL55;_tmp62=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp59)->f1;_tmp63=(void*)_tmp62.aggr_info;if(*((int*)_tmp63)!= 1)goto _LL55;
_tmp64=((struct Cyc_Absyn_KnownAggr_struct*)_tmp63)->f1;_tmp65=*_tmp64;_tmp66=
_tmp62.targs;_tmp67=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)
_tmp59)->f1).targs;_tmp68=((struct Cyc_Absyn_Aggr_p_struct*)_tmp59)->f2;_tmp69=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp59)->f3;_LL54: if(_tmp65->impl == 0){({void*
_tmp9A[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("can't destructure an abstract struct",
sizeof(char),37),_tag_arr(_tmp9A,sizeof(void*),0));});t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp9B;}));goto _LL3A;}{struct _RegionHandle _tmp9C=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp9C;_push_region(rgn);{struct Cyc_List_List*_tmp9D=0;{
struct Cyc_List_List*t=_tmp68;for(0;t != 0;t=t->tl){struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)t->hd;{void*_tmp9E=Cyc_Absyn_compress_kb((void*)tv->kind);
void*_tmp9F;struct Cyc_Core_Opt*_tmpA0;struct Cyc_Core_Opt**_tmpA1;struct Cyc_Core_Opt*
_tmpA2;struct Cyc_Core_Opt**_tmpA3;void*_tmpA4;struct Cyc_Core_Opt*_tmpA5;struct
Cyc_Core_Opt**_tmpA6;void*_tmpA7;void*_tmpA8;void*_tmpA9;_LL72: if(*((int*)_tmp9E)
!= 0)goto _LL74;_tmp9F=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp9E)->f1;if((int)
_tmp9F != 2)goto _LL74;_LL73: goto _LL71;_LL74: if(*((int*)_tmp9E)!= 1)goto _LL76;
_tmpA0=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp9E)->f1;_tmpA1=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp9E)->f1;_LL75: _tmpA3=_tmpA1;goto _LL77;
_LL76: if(*((int*)_tmp9E)!= 2)goto _LL78;_tmpA2=((struct Cyc_Absyn_Less_kb_struct*)
_tmp9E)->f1;_tmpA3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp9E)->f1;_tmpA4=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp9E)->f2;if((int)
_tmpA4 != 1)goto _LL78;_LL77: _tmpA6=_tmpA3;goto _LL79;_LL78: if(*((int*)_tmp9E)!= 2)
goto _LL7A;_tmpA5=((struct Cyc_Absyn_Less_kb_struct*)_tmp9E)->f1;_tmpA6=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp9E)->f1;_tmpA7=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp9E)->f2;if((int)_tmpA7 != 0)goto _LL7A;_LL79:*_tmpA6=({struct Cyc_Core_Opt*
_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpAC;
_tmpAC.tag=0;_tmpAC.f1=(void*)((void*)2);_tmpAC;});_tmpAB;}));_tmpAA;});goto
_LL71;_LL7A: if(*((int*)_tmp9E)!= 0)goto _LL7C;_tmpA8=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp9E)->f1;_LL7B: _tmpA9=_tmpA8;goto _LL7D;_LL7C: if(*((int*)_tmp9E)!= 2)goto _LL71;
_tmpA9=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp9E)->f2;_LL7D:({struct Cyc_Std_String_pa_struct
_tmpB0;_tmpB0.tag=0;_tmpB0.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(_tmpA9);{
struct Cyc_Std_String_pa_struct _tmpAF;_tmpAF.tag=0;_tmpAF.f1=(struct _tagged_arr)*
tv->name;{struct Cyc_Std_String_pa_struct _tmpAE;_tmpAE.tag=0;_tmpAE.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp65->name);{void*_tmpAD[3]={& _tmpAE,&
_tmpAF,& _tmpB0};Cyc_Tcutil_warn(p->loc,_tag_arr("struct %s abstracts type variable %s of kind %s != B",
sizeof(char),53),_tag_arr(_tmpAD,sizeof(void*),3));}}}});goto _LL71;_LL71:;}
_tmp9D=({struct Cyc_List_List*_tmpB1=_region_malloc(rgn,sizeof(*_tmpB1));_tmpB1->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));
_tmpB2[0]=({struct Cyc_Absyn_VarType_struct _tmpB3;_tmpB3.tag=1;_tmpB3.f1=tv;
_tmpB3;});_tmpB2;}));_tmpB1->tl=_tmp9D;_tmpB1;});}}_tmp9D=Cyc_List_imp_rev(
_tmp9D);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmp68);
struct Cyc_List_List*_tmpB4=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4 _tmpB5=({
struct _tuple4 _tmpCF;_tmpCF.f1=_tmpB4;_tmpCF.f2=rgn;_tmpCF;});struct Cyc_List_List*
_tmpB6=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmpB5,_tmp65->tvs);struct Cyc_List_List*_tmpB7=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp65->impl))->exist_vars,_tmp9D);struct Cyc_List_List*_tmpB8=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpB6);struct Cyc_List_List*_tmpB9=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpB7);struct Cyc_List_List*_tmpBA=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(rgn,_tmpB6,_tmpB7);if(_tmp68 != 0?1:((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp65->impl))->rgn_po != 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({
struct _tuple6*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->f1=0;_tmpBB->f2=0;_tmpBB;});(*((
struct _tuple6*)_check_null(res.tvars_and_bounds_opt))).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(res.tvars_and_bounds_opt))).f1,_tmp68);{struct Cyc_List_List*_tmpBC=0;{
struct Cyc_List_List*_tmpBD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp65->impl))->rgn_po;
for(0;_tmpBD != 0;_tmpBD=_tmpBD->tl){_tmpBC=({struct Cyc_List_List*_tmpBE=
_cycalloc(sizeof(*_tmpBE));_tmpBE->hd=({struct _tuple0*_tmpBF=_cycalloc(sizeof(*
_tmpBF));_tmpBF->f1=Cyc_Tcutil_rsubstitute(rgn,_tmpBA,(*((struct _tuple0*)_tmpBD->hd)).f1);
_tmpBF->f2=Cyc_Tcutil_rsubstitute(rgn,_tmpBA,(*((struct _tuple0*)_tmpBD->hd)).f2);
_tmpBF;});_tmpBE->tl=_tmpBC;_tmpBE;});}}_tmpBC=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBC);(*res.tvars_and_bounds_opt).f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f2,_tmpBC);}}*_tmp67=_tmpB8;t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_AggrType_struct
_tmpC1;_tmpC1.tag=10;_tmpC1.f1=({struct Cyc_Absyn_AggrInfo _tmpC2;_tmpC2.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));
_tmpC3[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpC4;_tmpC4.tag=1;_tmpC4.f1=({
struct Cyc_Absyn_Aggrdecl**_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=_tmp65;
_tmpC5;});_tmpC4;});_tmpC3;}));_tmpC2.targs=*_tmp67;_tmpC2;});_tmpC1;});_tmpC0;});{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,_tmp69,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp65->impl))->fields);for(0;fields != 0;fields=fields->tl){struct
_tuple9 _tmpC7;struct Cyc_Absyn_Aggrfield*_tmpC8;struct Cyc_Absyn_Pat*_tmpC9;struct
_tuple9*_tmpC6=(struct _tuple9*)fields->hd;_tmpC7=*_tmpC6;_tmpC8=_tmpC7.f1;_tmpC9=
_tmpC7.f2;{void*_tmpCA=Cyc_Tcutil_rsubstitute(rgn,_tmpBA,(void*)_tmpC8->type);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmpC9,(void**)& _tmpCA,
rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmpC9->topt))->v,
_tmpCA))({struct Cyc_Std_String_pa_struct _tmpCE;_tmpCE.tag=0;_tmpCE.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpC9->topt))->v);{struct Cyc_Std_String_pa_struct _tmpCD;_tmpCD.tag=0;_tmpCD.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpCA);{struct Cyc_Std_String_pa_struct
_tmpCC;_tmpCC.tag=0;_tmpCC.f1=(struct _tagged_arr)*_tmpC8->name;{void*_tmpCB[3]={&
_tmpCC,& _tmpCD,& _tmpCE};Cyc_Tcutil_terr(p->loc,_tag_arr("field %s of struct pattern expects type %s != %s",
sizeof(char),49),_tag_arr(_tmpCB,sizeof(void*),3));}}}});}}}}};_pop_region(rgn);}
goto _LL3A;_LL55: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 8)goto _LL57;_tmp6A=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp59)->f1;_tmp6B=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp59)->f2;_tmp6C=((struct Cyc_Absyn_Tunion_p_struct*)_tmp59)->f3;_LL56:{struct
_RegionHandle _tmpD0=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD0;
_push_region(rgn);{struct Cyc_List_List*tqts=_tmp6B->typs;struct Cyc_List_List*
_tmpD1=Cyc_Tcenv_lookup_type_vars(te);struct _tuple4 _tmpD2=({struct _tuple4 _tmpF4;
_tmpF4.f1=_tmpD1;_tmpF4.f2=rgn;_tmpF4;});struct Cyc_List_List*_tmpD3=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmpD2,_tmp6A->tvs);struct Cyc_List_List*_tmpD4=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmpD3);if(tqts == 0)t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_TunionType_struct
_tmpD6;_tmpD6.tag=2;_tmpD6.f1=({struct Cyc_Absyn_TunionInfo _tmpD7;_tmpD7.tunion_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*_tmpD9=_cycalloc(sizeof(*
_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpDA;_tmpDA.tag=1;
_tmpDA.f1=({struct Cyc_Absyn_Tuniondecl**_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[
0]=_tmp6A;_tmpDB;});_tmpDA;});_tmpD9;}));_tmpD7.targs=_tmpD4;_tmpD7.rgn=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->v=_tmpD1;_tmpD8;}));_tmpD7;});_tmpD6;});
_tmpD5;});else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmpDC=
_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpDD;_tmpDD.tag=3;_tmpDD.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpDE;_tmpDE.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmpDF=_cycalloc(sizeof(*
_tmpDF));_tmpDF[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmpE0;_tmpE0.tag=1;
_tmpE0.f1=_tmp6A;_tmpE0.f2=_tmp6B;_tmpE0;});_tmpDF;}));_tmpDE.targs=_tmpD4;
_tmpDE;});_tmpDD;});_tmpDC;});}if(topt != 0?tqts == 0: 0){void*_tmpE1=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_TunionInfo _tmpE2;struct Cyc_List_List*_tmpE3;_LL7F: if(
_tmpE1 <= (void*)3?1:*((int*)_tmpE1)!= 3)goto _LL81;_LL80: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpE5;_tmpE5.tag=3;_tmpE5.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpE6;_tmpE6.field_info=(
void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmpE7=_cycalloc(sizeof(*
_tmpE7));_tmpE7[0]=({struct Cyc_Absyn_KnownTunionfield_struct _tmpE8;_tmpE8.tag=1;
_tmpE8.f1=_tmp6A;_tmpE8.f2=_tmp6B;_tmpE8;});_tmpE7;}));_tmpE6.targs=_tmpD4;
_tmpE6;});_tmpE5;});_tmpE4;});goto _LL7E;_LL81: if(_tmpE1 <= (void*)3?1:*((int*)
_tmpE1)!= 2)goto _LL83;_tmpE2=((struct Cyc_Absyn_TunionType_struct*)_tmpE1)->f1;
_tmpE3=_tmpE2.targs;_LL82: {struct Cyc_List_List*_tmpE9=_tmpD4;for(0;_tmpE9 != 0?
_tmpE3 != 0: 0;(_tmpE9=_tmpE9->tl,_tmpE3=_tmpE3->tl)){Cyc_Tcutil_unify((void*)
_tmpE9->hd,(void*)_tmpE3->hd);}goto _LL7E;}_LL83:;_LL84: goto _LL7E;_LL7E:;}for(0;
_tmp6C != 0?tqts != 0: 0;(_tmp6C=_tmp6C->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmpEA=(struct Cyc_Absyn_Pat*)_tmp6C->hd;void*_tmpEB=Cyc_Tcutil_rsubstitute(rgn,
_tmpD3,(*((struct _tuple8*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmpEA,(void**)& _tmpEB,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEA->topt))->v,_tmpEB))({struct Cyc_Std_String_pa_struct _tmpEF;
_tmpEF.tag=0;_tmpEF.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpEA->topt))->v);{struct Cyc_Std_String_pa_struct
_tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpEB);{
struct Cyc_Std_String_pa_struct _tmpED;_tmpED.tag=0;_tmpED.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp6A->name);{void*_tmpEC[3]={& _tmpED,& _tmpEE,& _tmpEF};
Cyc_Tcutil_terr(_tmpEA->loc,_tag_arr("%s expects argument type %s, not %s",
sizeof(char),36),_tag_arr(_tmpEC,sizeof(void*),3));}}}});}if(_tmp6C != 0)({struct
Cyc_Std_String_pa_struct _tmpF1;_tmpF1.tag=0;_tmpF1.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6A->name);{void*_tmpF0[1]={& _tmpF1};Cyc_Tcutil_terr(p->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(char),45),_tag_arr(_tmpF0,sizeof(void*),1));}});if(tqts != 0)({struct Cyc_Std_String_pa_struct
_tmpF3;_tmpF3.tag=0;_tmpF3.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp6A->name);{
void*_tmpF2[1]={& _tmpF3};Cyc_Tcutil_terr(p->loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(char),44),_tag_arr(_tmpF2,sizeof(void*),1));}});};_pop_region(rgn);}goto
_LL3A;_LL57: if(_tmp59 <= (void*)2?1:*((int*)_tmp59)!= 7)goto _LL59;_tmp6D=((struct
Cyc_Absyn_Aggr_p_struct*)_tmp59)->f1;_tmp6E=(void*)_tmp6D.aggr_info;if(*((int*)
_tmp6E)!= 0)goto _LL59;_LL58: goto _LL5A;_LL59: if(_tmp59 <= (void*)2?1:*((int*)
_tmp59)!= 11)goto _LL5B;_LL5A: goto _LL5C;_LL5B: if(_tmp59 <= (void*)2?1:*((int*)
_tmp59)!= 12)goto _LL3A;_LL5C: t=Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmpF5=
_cycalloc(sizeof(*_tmpF5));_tmpF5->v=Cyc_Tcenv_lookup_type_vars(te);_tmpF5;}));
goto _LL3A;_LL3A:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmpF6=_cycalloc(
sizeof(*_tmpF6));_tmpF6->v=(void*)t;_tmpF6;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmpF8=Cyc_Tcpat_tcPatRec(te,p,topt,0);{struct _RegionHandle
_tmpF9=_new_region("r");struct _RegionHandle*r=& _tmpF9;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmpF8.patvars),p->loc,
_tag_arr("pattern contains a repeated variable",sizeof(char),37));;_pop_region(r);}
return _tmpF8;}void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmpFA=(void*)p->r;struct Cyc_Absyn_Pat*_tmpFB;struct Cyc_List_List*_tmpFC;
struct Cyc_List_List*_tmpFD;struct Cyc_List_List*_tmpFE;_LL86: if(_tmpFA <= (void*)2?
1:*((int*)_tmpFA)!= 5)goto _LL88;_tmpFB=((struct Cyc_Absyn_Pointer_p_struct*)
_tmpFA)->f1;_LL87: Cyc_Tcpat_check_pat_regions(te,_tmpFB);{void*_tmpFF=(void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_PtrInfo _tmp100;void*
_tmp101;struct Cyc_Absyn_TunionInfo _tmp102;void*_tmp103;_LL91: if(_tmpFF <= (void*)
3?1:*((int*)_tmpFF)!= 4)goto _LL93;_tmp100=((struct Cyc_Absyn_PointerType_struct*)
_tmpFF)->f1;_tmp101=(void*)_tmp100.rgn_typ;_LL92: _tmp103=_tmp101;goto _LL94;_LL93:
if(_tmpFF <= (void*)3?1:*((int*)_tmpFF)!= 2)goto _LL95;_tmp102=((struct Cyc_Absyn_TunionType_struct*)
_tmpFF)->f1;_tmp103=(void*)_tmp102.rgn;_LL94: Cyc_Tcenv_check_rgn_accessible(te,p->loc,
_tmp103);return;_LL95:;_LL96:({void*_tmp104[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_pat_regions: bad pointer type",
sizeof(char),36),_tag_arr(_tmp104,sizeof(void*),0));});_LL90:;}_LL88: if(_tmpFA <= (
void*)2?1:*((int*)_tmpFA)!= 8)goto _LL8A;_tmpFC=((struct Cyc_Absyn_Tunion_p_struct*)
_tmpFA)->f3;_LL89: for(0;_tmpFC != 0;_tmpFC=_tmpFC->tl){Cyc_Tcpat_check_pat_regions(
te,(struct Cyc_Absyn_Pat*)_tmpFC->hd);}{void*_tmp105=(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v;struct Cyc_Absyn_TunionInfo _tmp106;void*_tmp107;_LL98:
if(_tmp105 <= (void*)3?1:*((int*)_tmp105)!= 2)goto _LL9A;_tmp106=((struct Cyc_Absyn_TunionType_struct*)
_tmp105)->f1;_tmp107=(void*)_tmp106.rgn;_LL99: Cyc_Tcenv_check_rgn_accessible(te,
p->loc,_tmp107);return;_LL9A: if(_tmp105 <= (void*)3?1:*((int*)_tmp105)!= 3)goto
_LL9C;_LL9B: return;_LL9C:;_LL9D:({void*_tmp108[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_pat_regions: bad tunion type",
sizeof(char),35),_tag_arr(_tmp108,sizeof(void*),0));});_LL97:;}_LL8A: if(_tmpFA <= (
void*)2?1:*((int*)_tmpFA)!= 7)goto _LL8C;_tmpFD=((struct Cyc_Absyn_Aggr_p_struct*)
_tmpFA)->f3;_LL8B: for(0;_tmpFD != 0;_tmpFD=_tmpFD->tl){Cyc_Tcpat_check_pat_regions(
te,(*((struct _tuple7*)_tmpFD->hd)).f2);}return;_LL8C: if(_tmpFA <= (void*)2?1:*((
int*)_tmpFA)!= 4)goto _LL8E;_tmpFE=((struct Cyc_Absyn_Tuple_p_struct*)_tmpFA)->f1;
_LL8D: for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){Cyc_Tcpat_check_pat_regions(te,(struct
Cyc_Absyn_Pat*)_tmpFE->hd);}return;_LL8E:;_LL8F: return;_LL85:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};struct
Cyc_Tcpat_Con_s{void*name;int arity;struct Cyc_Core_Opt*span;};struct Cyc_Tcpat_Con_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){void*_tmp109=(void*)c1->name;
struct _tagged_arr _tmp10A;int _tmp10B;_LL9F: if(*((int*)_tmp109)!= 0)goto _LLA1;
_tmp10A=((struct Cyc_Tcpat_Name_v_struct*)_tmp109)->f1;_LLA0: {void*_tmp10C=(void*)
c2->name;struct _tagged_arr _tmp10D;_LLA4: if(*((int*)_tmp10C)!= 0)goto _LLA6;
_tmp10D=((struct Cyc_Tcpat_Name_v_struct*)_tmp10C)->f1;_LLA5: return Cyc_Std_zstrcmp(
_tmp10A,_tmp10D);_LLA6: if(*((int*)_tmp10C)!= 1)goto _LLA3;_LLA7: return - 1;_LLA3:;}
_LLA1: if(*((int*)_tmp109)!= 1)goto _LL9E;_tmp10B=((struct Cyc_Tcpat_Int_v_struct*)
_tmp109)->f1;_LLA2: {void*_tmp10E=(void*)c2->name;int _tmp10F;_LLA9: if(*((int*)
_tmp10E)!= 0)goto _LLAB;_LLAA: return 1;_LLAB: if(*((int*)_tmp10E)!= 1)goto _LLA8;
_tmp10F=((struct Cyc_Tcpat_Int_v_struct*)_tmp10E)->f1;_LLAC: return _tmp10B - 
_tmp10F;_LLA8:;}_LL9E:;}static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct
_RegionHandle*r){return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(
struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(
void*)2};static struct Cyc_Core_Opt*Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)&
Cyc_Tcpat_one_opt;static char _tmp111[5]="NULL";static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={0,{_tmp111,_tmp111,_tmp111 + 5}};static char _tmp113[4]="ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={0,{_tmp113,_tmp113,
_tmp113 + 4}};static char _tmp115[6]="tuple";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={
0,{_tmp115,_tmp115,_tmp115 + 6}};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)((void*)& Cyc_Tcpat_null_name_value),0,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={(void*)((void*)& Cyc_Tcpat_ptr_name_value),
1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)((void*)& Cyc_Tcpat_ptr_name_value),1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0;static struct
Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0;static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(
int i){return({struct Cyc_Tcpat_Con_s*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp117=_cycalloc_atomic(sizeof(*
_tmp117));_tmp117[0]=({struct Cyc_Tcpat_Int_v_struct _tmp118;_tmp118.tag=1;_tmp118.f1=
i;_tmp118;});_tmp117;}));_tmp116->arity=0;_tmp116->span=0;_tmp116;});}static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _tagged_arr f){return({struct Cyc_Tcpat_Con_s*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->name=(void*)((void*)({struct Cyc_Tcpat_Name_v_struct*
_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Tcpat_Name_v_struct
_tmp11B;_tmp11B.tag=0;_tmp11B.f1=f;_tmp11B;});_tmp11A;}));_tmp119->arity=0;
_tmp119->span=0;_tmp119;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c){
return({struct Cyc_Tcpat_Con_s*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->name=(
void*)((void*)({struct Cyc_Tcpat_Int_v_struct*_tmp11E=_cycalloc_atomic(sizeof(*
_tmp11E));_tmp11E[0]=({struct Cyc_Tcpat_Int_v_struct _tmp11F;_tmp11F.tag=1;_tmp11F.f1=(
int)c;_tmp11F;});_tmp11E;}));_tmp11C->arity=0;_tmp11C->span=({struct Cyc_Core_Opt*
_tmp11D=_cycalloc_atomic(sizeof(*_tmp11D));_tmp11D->v=(void*)256;_tmp11D;});
_tmp11C;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i){return({struct
Cyc_Tcpat_Con_s*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->name=(void*)((void*)&
Cyc_Tcpat_tuple_name_value);_tmp120->arity=i;_tmp120->span=Cyc_Tcpat_one_opt_ptr;
_tmp120;});}static void*Cyc_Tcpat_null_pat(){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121[0]=({struct Cyc_Tcpat_Con_struct
_tmp122;_tmp122.tag=0;_tmp122.f1=Cyc_Tcpat_null_con;_tmp122.f2=0;_tmp122;});
_tmp121;});}static void*Cyc_Tcpat_int_pat(int i){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123[0]=({struct Cyc_Tcpat_Con_struct
_tmp124;_tmp124.tag=0;_tmp124.f1=Cyc_Tcpat_int_con(i);_tmp124.f2=0;_tmp124;});
_tmp123;});}static void*Cyc_Tcpat_char_pat(char c){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_Tcpat_Con_struct
_tmp126;_tmp126.tag=0;_tmp126.f1=Cyc_Tcpat_char_con(c);_tmp126.f2=0;_tmp126;});
_tmp125;});}static void*Cyc_Tcpat_float_pat(struct _tagged_arr f){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=({
struct Cyc_Tcpat_Con_struct _tmp128;_tmp128.tag=0;_tmp128.f1=Cyc_Tcpat_float_con(f);
_tmp128.f2=0;_tmp128;});_tmp127;});}static void*Cyc_Tcpat_null_ptr_pat(void*p){
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp129=_cycalloc(sizeof(*_tmp129));
_tmp129[0]=({struct Cyc_Tcpat_Con_struct _tmp12A;_tmp12A.tag=0;_tmp12A.f1=Cyc_Tcpat_null_ptr_con;
_tmp12A.f2=({struct Cyc_List_List*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->hd=(
void*)p;_tmp12B->tl=0;_tmp12B;});_tmp12A;});_tmp129;});}static void*Cyc_Tcpat_ptr_pat(
void*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp12C=_cycalloc(sizeof(*
_tmp12C));_tmp12C[0]=({struct Cyc_Tcpat_Con_struct _tmp12D;_tmp12D.tag=0;_tmp12D.f1=
Cyc_Tcpat_ptr_con;_tmp12D.f2=({struct Cyc_List_List*_tmp12E=_cycalloc(sizeof(*
_tmp12E));_tmp12E->hd=(void*)p;_tmp12E->tl=0;_tmp12E;});_tmp12D;});_tmp12C;});}
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F[0]=({struct Cyc_Tcpat_Con_struct
_tmp130;_tmp130.tag=0;_tmp130.f1=Cyc_Tcpat_tuple_con(Cyc_List_length(ss));
_tmp130.f2=ss;_tmp130;});_tmp12F;});}static void*Cyc_Tcpat_con_pat(struct
_tagged_arr con_name,struct Cyc_Core_Opt*span,struct Cyc_List_List*ps){struct Cyc_Tcpat_Con_s*
c=({struct Cyc_Tcpat_Con_s*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->name=(
void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp134=_cycalloc(sizeof(*_tmp134));
_tmp134[0]=({struct Cyc_Tcpat_Name_v_struct _tmp135;_tmp135.tag=0;_tmp135.f1=
con_name;_tmp135;});_tmp134;}));_tmp133->arity=Cyc_List_length(ps);_tmp133->span=
span;_tmp133;});return(void*)({struct Cyc_Tcpat_Con_struct*_tmp131=_cycalloc(
sizeof(*_tmp131));_tmp131[0]=({struct Cyc_Tcpat_Con_struct _tmp132;_tmp132.tag=0;
_tmp132.f1=c;_tmp132.f2=ps;_tmp132;});_tmp131;});}static void*Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat*p){void*s;{void*_tmp136=(void*)p->r;void*_tmp137;int _tmp138;
char _tmp139;struct _tagged_arr _tmp13A;struct Cyc_Absyn_Pat*_tmp13B;struct Cyc_Absyn_Tuniondecl*
_tmp13C;struct Cyc_Absyn_Tunionfield*_tmp13D;struct Cyc_List_List*_tmp13E;struct
Cyc_List_List*_tmp13F;struct Cyc_List_List*_tmp140;struct Cyc_Absyn_AggrInfo
_tmp141;void*_tmp142;struct Cyc_Absyn_Aggrdecl**_tmp143;struct Cyc_Absyn_Aggrdecl*
_tmp144;struct Cyc_List_List*_tmp145;struct Cyc_Absyn_Enumdecl*_tmp146;struct Cyc_Absyn_Enumfield*
_tmp147;void*_tmp148;struct Cyc_Absyn_Enumfield*_tmp149;_LLAE: if((int)_tmp136 != 0)
goto _LLB0;_LLAF: s=(void*)0;goto _LLAD;_LLB0: if(_tmp136 <= (void*)2?1:*((int*)
_tmp136)!= 0)goto _LLB2;_LLB1: s=(void*)0;goto _LLAD;_LLB2: if((int)_tmp136 != 1)goto
_LLB4;_LLB3: s=Cyc_Tcpat_null_pat();goto _LLAD;_LLB4: if(_tmp136 <= (void*)2?1:*((
int*)_tmp136)!= 1)goto _LLB6;_tmp137=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp136)->f1;_tmp138=((struct Cyc_Absyn_Int_p_struct*)_tmp136)->f2;_LLB5: s=Cyc_Tcpat_int_pat(
_tmp138);goto _LLAD;_LLB6: if(_tmp136 <= (void*)2?1:*((int*)_tmp136)!= 2)goto _LLB8;
_tmp139=((struct Cyc_Absyn_Char_p_struct*)_tmp136)->f1;_LLB7: s=Cyc_Tcpat_char_pat(
_tmp139);goto _LLAD;_LLB8: if(_tmp136 <= (void*)2?1:*((int*)_tmp136)!= 3)goto _LLBA;
_tmp13A=((struct Cyc_Absyn_Float_p_struct*)_tmp136)->f1;_LLB9: s=Cyc_Tcpat_float_pat(
_tmp13A);goto _LLAD;_LLBA: if(_tmp136 <= (void*)2?1:*((int*)_tmp136)!= 6)goto _LLBC;
_LLBB: s=(void*)0;goto _LLAD;_LLBC: if(_tmp136 <= (void*)2?1:*((int*)_tmp136)!= 5)
goto _LLBE;_tmp13B=((struct Cyc_Absyn_Pointer_p_struct*)_tmp136)->f1;_LLBD:{void*
_tmp14A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp14B;struct Cyc_Absyn_Conref*_tmp14C;_LLD1: if(_tmp14A <= (
void*)3?1:*((int*)_tmp14A)!= 4)goto _LLD3;_tmp14B=((struct Cyc_Absyn_PointerType_struct*)
_tmp14A)->f1;_tmp14C=_tmp14B.nullable;_LLD2: {int is_nullable=0;int still_working=
1;while(still_working){void*_tmp14D=(void*)_tmp14C->v;struct Cyc_Absyn_Conref*
_tmp14E;int _tmp14F;_LLD8: if(_tmp14D <= (void*)1?1:*((int*)_tmp14D)!= 1)goto _LLDA;
_tmp14E=((struct Cyc_Absyn_Forward_constr_struct*)_tmp14D)->f1;_LLD9:(void*)(
_tmp14C->v=(void*)((void*)_tmp14E->v));continue;_LLDA: if((int)_tmp14D != 0)goto
_LLDC;_LLDB:(void*)(_tmp14C->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp151;_tmp151.tag=0;_tmp151.f1=(void*)0;_tmp151;});_tmp150;})));is_nullable=0;
still_working=0;goto _LLD7;_LLDC: if(_tmp14D <= (void*)1?1:*((int*)_tmp14D)!= 0)
goto _LLD7;_tmp14F=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp14D)->f1;_LLDD:
is_nullable=(int)_tmp14F;still_working=0;goto _LLD7;_LLD7:;}{void*ss=Cyc_Tcpat_compile_pat(
_tmp13B);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(ss);else{s=Cyc_Tcpat_ptr_pat(ss);}
goto _LLD0;}}_LLD3: if(_tmp14A <= (void*)3?1:*((int*)_tmp14A)!= 2)goto _LLD5;_LLD4:{
void*_tmp152=(void*)_tmp13B->r;struct Cyc_Absyn_Tuniondecl*_tmp153;struct Cyc_Absyn_Tunionfield*
_tmp154;struct Cyc_List_List*_tmp155;_LLDF: if(_tmp152 <= (void*)2?1:*((int*)
_tmp152)!= 8)goto _LLE1;_tmp153=((struct Cyc_Absyn_Tunion_p_struct*)_tmp152)->f1;
_tmp154=((struct Cyc_Absyn_Tunion_p_struct*)_tmp152)->f2;_tmp155=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp152)->f3;_LLE0: {struct Cyc_Core_Opt*span;if(_tmp153->is_xtunion)span=0;else{
span=({struct Cyc_Core_Opt*_tmp156=_cycalloc_atomic(sizeof(*_tmp156));_tmp156->v=(
void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp153->fields))->v);_tmp156;});}s=Cyc_Tcpat_con_pat(*(*
_tmp154->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp155));goto _LLDE;}
_LLE1:;_LLE2:({void*_tmp157[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("non-[x]tunion pattern has tunion type",
sizeof(char),38),_tag_arr(_tmp157,sizeof(void*),0));});_LLDE:;}goto _LLD0;_LLD5:;
_LLD6:({void*_tmp158[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("pointer pattern does not have pointer type",sizeof(
char),43),_tag_arr(_tmp158,sizeof(void*),0));});_LLD0:;}goto _LLAD;_LLBE: if(
_tmp136 <= (void*)2?1:*((int*)_tmp136)!= 8)goto _LLC0;_tmp13C=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp136)->f1;_tmp13D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp136)->f2;_tmp13E=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp136)->f3;if(_tmp13E != 0)goto _LLC0;_LLBF: {
struct Cyc_Core_Opt*span;{void*_tmp159=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);_LLE4: if(_tmp159 <= (void*)3?1:*((int*)_tmp159)!= 2)
goto _LLE6;_LLE5: if(_tmp13C->is_xtunion)span=0;else{span=({struct Cyc_Core_Opt*
_tmp15A=_cycalloc_atomic(sizeof(*_tmp15A));_tmp15A->v=(void*)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp13C->fields))->v);_tmp15A;});}goto _LLE3;_LLE6: if(_tmp159 <= (void*)3?1:*((int*)
_tmp159)!= 3)goto _LLE8;_LLE7: span=({struct Cyc_Core_Opt*_tmp15B=_cycalloc_atomic(
sizeof(*_tmp15B));_tmp15B->v=(void*)1;_tmp15B;});goto _LLE3;_LLE8:;_LLE9: span=({
void*_tmp15C[0]={};((struct Cyc_Core_Opt*(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("void tunion pattern has bad type",
sizeof(char),33),_tag_arr(_tmp15C,sizeof(void*),0));});goto _LLE3;_LLE3:;}s=Cyc_Tcpat_con_pat(*(*
_tmp13D->name).f2,span,0);goto _LLAD;}_LLC0: if(_tmp136 <= (void*)2?1:*((int*)
_tmp136)!= 8)goto _LLC2;_tmp13F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp136)->f3;
_LLC1: _tmp140=_tmp13F;goto _LLC3;_LLC2: if(_tmp136 <= (void*)2?1:*((int*)_tmp136)!= 
4)goto _LLC4;_tmp140=((struct Cyc_Absyn_Tuple_p_struct*)_tmp136)->f1;_LLC3: s=Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp140));goto _LLAD;_LLC4: if(_tmp136 <= (void*)
2?1:*((int*)_tmp136)!= 7)goto _LLC6;_tmp141=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp136)->f1;_tmp142=(void*)_tmp141.aggr_info;if(*((int*)_tmp142)!= 1)goto _LLC6;
_tmp143=((struct Cyc_Absyn_KnownAggr_struct*)_tmp142)->f1;_tmp144=*_tmp143;
_tmp145=((struct Cyc_Absyn_Aggr_p_struct*)_tmp136)->f3;_LLC5: {struct Cyc_List_List*
ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp144->impl))->fields;for(0;fields != 0;fields=fields->tl){int found=Cyc_Std_strcmp(*((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name,_tag_arr("",sizeof(char),1))== 0;{
struct Cyc_List_List*dlps0=_tmp145;for(0;!found?dlps0 != 0: 0;dlps0=dlps0->tl){
struct _tuple7 _tmp15E;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Pat*_tmp160;
struct _tuple7*_tmp15D=(struct _tuple7*)dlps0->hd;_tmp15E=*_tmp15D;_tmp15F=_tmp15E.f1;
_tmp160=_tmp15E.f2;{struct Cyc_List_List*_tmp161=_tmp15F;struct Cyc_List_List
_tmp162;void*_tmp163;struct _tagged_arr*_tmp164;struct Cyc_List_List*_tmp165;_LLEB:
if(_tmp161 == 0)goto _LLED;_tmp162=*_tmp161;_tmp163=(void*)_tmp162.hd;if(*((int*)
_tmp163)!= 1)goto _LLED;_tmp164=((struct Cyc_Absyn_FieldName_struct*)_tmp163)->f1;
_tmp165=_tmp162.tl;if(_tmp165 != 0)goto _LLED;_LLEC: if(Cyc_Std_zstrptrcmp(_tmp164,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->hd=(void*)Cyc_Tcpat_compile_pat(
_tmp160);_tmp166->tl=ps;_tmp166;});found=1;}goto _LLEA;_LLED:;_LLEE:({void*
_tmp167[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("bad designator(s)",sizeof(char),18),_tag_arr(_tmp167,sizeof(void*),0));});
_LLEA:;}}}if(!found)({void*_tmp168[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad designator",sizeof(char),15),
_tag_arr(_tmp168,sizeof(void*),0));});}}s=Cyc_Tcpat_tuple_pat(ps);goto _LLAD;}
_LLC6: if(_tmp136 <= (void*)2?1:*((int*)_tmp136)!= 9)goto _LLC8;_tmp146=((struct Cyc_Absyn_Enum_p_struct*)
_tmp136)->f1;_tmp147=((struct Cyc_Absyn_Enum_p_struct*)_tmp136)->f2;_LLC7: {int
span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp146->fields))->v);s=Cyc_Tcpat_con_pat(*(*
_tmp147->name).f2,({struct Cyc_Core_Opt*_tmp169=_cycalloc_atomic(sizeof(*_tmp169));
_tmp169->v=(void*)span;_tmp169;}),0);goto _LLAD;}_LLC8: if(_tmp136 <= (void*)2?1:*((
int*)_tmp136)!= 10)goto _LLCA;_tmp148=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp136)->f1;_tmp149=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp136)->f2;_LLC9: {
struct Cyc_List_List*fields;{void*_tmp16A=Cyc_Tcutil_compress(_tmp148);struct Cyc_List_List*
_tmp16B;_LLF0: if(_tmp16A <= (void*)3?1:*((int*)_tmp16A)!= 13)goto _LLF2;_tmp16B=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp16A)->f1;_LLF1: fields=_tmp16B;goto _LLEF;
_LLF2:;_LLF3:({void*_tmp16C[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad type in AnonEnum_p",sizeof(char),
23),_tag_arr(_tmp16C,sizeof(void*),0));});_LLEF:;}{int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*_tmp149->name).f2,({struct Cyc_Core_Opt*
_tmp16D=_cycalloc_atomic(sizeof(*_tmp16D));_tmp16D->v=(void*)span;_tmp16D;}),0);
goto _LLAD;}}_LLCA: if(_tmp136 <= (void*)2?1:*((int*)_tmp136)!= 11)goto _LLCC;_LLCB:({
void*_tmp16E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("compile_pat: unknownid",sizeof(char),23),_tag_arr(_tmp16E,sizeof(void*),
0));});_LLCC: if(_tmp136 <= (void*)2?1:*((int*)_tmp136)!= 12)goto _LLCE;_LLCD:({
void*_tmp16F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("compile_pat: unknowncall",sizeof(char),25),_tag_arr(_tmp16F,sizeof(
void*),0));});_LLCE: if(_tmp136 <= (void*)2?1:*((int*)_tmp136)!= 7)goto _LLAD;_LLCF:({
void*_tmp170[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("compile_pat: bad aggr",sizeof(char),22),_tag_arr(_tmp170,sizeof(void*),
0));});_LLAD:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int tag;struct Cyc_Set_Set*
f1;};struct Cyc_Tcpat_Success_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct
_tuple10{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c){void*_tmp171=td;struct Cyc_Set_Set*_tmp172;_LLF5: if(*((int*)_tmp171)!= 1)goto
_LLF7;_tmp172=((struct Cyc_Tcpat_Neg_struct*)_tmp171)->f1;_LLF6: if(((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp172,c))({void*
_tmp173[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("add_neg called when constructor already in set",sizeof(char),47),
_tag_arr(_tmp173,sizeof(void*),0));});if(c->span != 0?((int(*)(struct Cyc_Set_Set*
s))Cyc_Set_cardinality)(_tmp172)+ 1 >= (int)((struct Cyc_Core_Opt*)_check_null(c->span))->v:
0)({void*_tmp174[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("add_neg called when |cs U {c}| >= span(c)",sizeof(char),42),_tag_arr(
_tmp174,sizeof(void*),0));});return(void*)({struct Cyc_Tcpat_Neg_struct*_tmp175=
_region_malloc(r,sizeof(*_tmp175));_tmp175[0]=({struct Cyc_Tcpat_Neg_struct
_tmp176;_tmp176.tag=1;_tmp176.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp172,c);
_tmp176;});_tmp175;});_LLF7: if(*((int*)_tmp171)!= 0)goto _LLF4;_LLF8:({void*
_tmp177[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("add_neg called when td is Positive",sizeof(char),35),_tag_arr(_tmp177,
sizeof(void*),0));});_LLF4:;}static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*
c,void*td){void*_tmp178=td;struct Cyc_Tcpat_Con_s*_tmp179;struct Cyc_Set_Set*
_tmp17A;_LLFA: if(*((int*)_tmp178)!= 0)goto _LLFC;_tmp179=((struct Cyc_Tcpat_Pos_struct*)
_tmp178)->f1;_LLFB: if(Cyc_Tcpat_compare_con(c,_tmp179)== 0)return(void*)0;else{
return(void*)1;}_LLFC: if(*((int*)_tmp178)!= 1)goto _LLF9;_tmp17A=((struct Cyc_Tcpat_Neg_struct*)
_tmp178)->f1;_LLFD: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp17A,c))return(void*)1;else{if(c->span != 0?(int)((struct Cyc_Core_Opt*)
_check_null(c->span))->v == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp17A)+ 1: 0)return(void*)0;else{return(void*)2;}}_LLF9:;}struct _tuple11{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*
_tmp17B=ctxt;struct Cyc_List_List _tmp17C;struct _tuple11*_tmp17D;struct _tuple11
_tmp17E;struct Cyc_Tcpat_Con_s*_tmp17F;struct Cyc_List_List*_tmp180;struct Cyc_List_List*
_tmp181;_LLFF: if(_tmp17B != 0)goto _LL101;_LL100: return 0;_LL101: if(_tmp17B == 0)
goto _LLFE;_tmp17C=*_tmp17B;_tmp17D=(struct _tuple11*)_tmp17C.hd;_tmp17E=*_tmp17D;
_tmp17F=_tmp17E.f1;_tmp180=_tmp17E.f2;_tmp181=_tmp17C.tl;_LL102: return({struct
Cyc_List_List*_tmp182=_region_malloc(r,sizeof(*_tmp182));_tmp182->hd=({struct
_tuple11*_tmp183=_region_malloc(r,sizeof(*_tmp183));_tmp183->f1=_tmp17F;_tmp183->f2=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp184=_region_malloc(r,sizeof(*
_tmp184));_tmp184->hd=(void*)dsc;_tmp184->tl=_tmp180;_tmp184;});_tmp183;});
_tmp182->tl=_tmp181;_tmp182;});_LLFE:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp185=ctxt;
struct Cyc_List_List _tmp186;struct _tuple11*_tmp187;struct _tuple11 _tmp188;struct
Cyc_Tcpat_Con_s*_tmp189;struct Cyc_List_List*_tmp18A;struct Cyc_List_List*_tmp18B;
_LL104: if(_tmp185 != 0)goto _LL106;_LL105:({void*_tmp18C[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("norm_context: empty context",
sizeof(char),28),_tag_arr(_tmp18C,sizeof(void*),0));});_LL106: if(_tmp185 == 0)
goto _LL103;_tmp186=*_tmp185;_tmp187=(struct _tuple11*)_tmp186.hd;_tmp188=*_tmp187;
_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;_tmp18B=_tmp186.tl;_LL107: return Cyc_Tcpat_augment(
r,_tmp18B,(void*)({struct Cyc_Tcpat_Pos_struct*_tmp18D=_region_malloc(r,sizeof(*
_tmp18D));_tmp18D[0]=({struct Cyc_Tcpat_Pos_struct _tmp18E;_tmp18E.tag=0;_tmp18E.f1=
_tmp189;_tmp18E.f2=Cyc_List_rrev(r,_tmp18A);_tmp18E;});_tmp18D;}));_LL103:;}
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,
void*dsc,struct Cyc_List_List*work){struct _tuple6 _tmp190=({struct _tuple6 _tmp18F;
_tmp18F.f1=ctxt;_tmp18F.f2=work;_tmp18F;});struct Cyc_List_List*_tmp191;struct Cyc_List_List*
_tmp192;struct Cyc_List_List*_tmp193;struct Cyc_List_List*_tmp194;struct Cyc_List_List*
_tmp195;struct Cyc_List_List _tmp196;struct _tuple11*_tmp197;struct _tuple11 _tmp198;
struct Cyc_Tcpat_Con_s*_tmp199;struct Cyc_List_List*_tmp19A;struct Cyc_List_List*
_tmp19B;struct Cyc_List_List*_tmp19C;struct Cyc_List_List _tmp19D;struct _tuple10*
_tmp19E;struct _tuple10 _tmp19F;struct Cyc_List_List*_tmp1A0;struct Cyc_List_List*
_tmp1A1;_LL109: _tmp191=_tmp190.f1;if(_tmp191 != 0)goto _LL10B;_tmp192=_tmp190.f2;
if(_tmp192 != 0)goto _LL10B;_LL10A: return dsc;_LL10B: _tmp193=_tmp190.f1;if(_tmp193
!= 0)goto _LL10D;_LL10C: goto _LL10E;_LL10D: _tmp194=_tmp190.f2;if(_tmp194 != 0)goto
_LL10F;_LL10E:({void*_tmp1A2[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("build_desc: ctxt and work don't match",
sizeof(char),38),_tag_arr(_tmp1A2,sizeof(void*),0));});_LL10F: _tmp195=_tmp190.f1;
if(_tmp195 == 0)goto _LL108;_tmp196=*_tmp195;_tmp197=(struct _tuple11*)_tmp196.hd;
_tmp198=*_tmp197;_tmp199=_tmp198.f1;_tmp19A=_tmp198.f2;_tmp19B=_tmp196.tl;
_tmp19C=_tmp190.f2;if(_tmp19C == 0)goto _LL108;_tmp19D=*_tmp19C;_tmp19E=(struct
_tuple10*)_tmp19D.hd;_tmp19F=*_tmp19E;_tmp1A0=_tmp19F.f3;_tmp1A1=_tmp19D.tl;
_LL110: {struct Cyc_Tcpat_Pos_struct*_tmp1A3=({struct Cyc_Tcpat_Pos_struct*_tmp1A4=
_region_malloc(r,sizeof(*_tmp1A4));_tmp1A4[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1A5;_tmp1A5.tag=0;_tmp1A5.f1=_tmp199;_tmp1A5.f2=Cyc_List_rappend(r,Cyc_List_rrev(
r,_tmp19A),({struct Cyc_List_List*_tmp1A6=_region_malloc(r,sizeof(*_tmp1A6));
_tmp1A6->hd=(void*)dsc;_tmp1A6->tl=_tmp1A0;_tmp1A6;}));_tmp1A5;});_tmp1A4;});
return Cyc_Tcpat_build_desc(r,_tmp19B,(void*)_tmp1A3,_tmp1A1);}_LL108:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*,void*,struct Cyc_List_List*,void*,
struct Cyc_List_List*,struct Cyc_List_List*,void*,struct Cyc_List_List*);static void*
Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp1A7=allmrules;struct Cyc_List_List _tmp1A8;struct _tuple0*
_tmp1A9;struct _tuple0 _tmp1AA;void*_tmp1AB;void*_tmp1AC;struct Cyc_List_List*
_tmp1AD;_LL112: if(_tmp1A7 != 0)goto _LL114;_LL113: return(void*)0;_LL114: if(_tmp1A7
== 0)goto _LL111;_tmp1A8=*_tmp1A7;_tmp1A9=(struct _tuple0*)_tmp1A8.hd;_tmp1AA=*
_tmp1A9;_tmp1AB=_tmp1AA.f1;_tmp1AC=_tmp1AA.f2;_tmp1AD=_tmp1A8.tl;_LL115: return
Cyc_Tcpat_match(r,_tmp1AB,0,dsc,0,0,_tmp1AC,_tmp1AD);_LL111:;}static void*Cyc_Tcpat_match_compile(
struct _RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(
void*)({struct Cyc_Tcpat_Neg_struct*_tmp1AE=_region_malloc(r,sizeof(*_tmp1AE));
_tmp1AE[0]=({struct Cyc_Tcpat_Neg_struct _tmp1AF;_tmp1AF.tag=1;_tmp1AF.f1=Cyc_Tcpat_empty_con_set(
r);_tmp1AF;});_tmp1AE;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp1B0=work;
struct Cyc_List_List _tmp1B1;struct _tuple10*_tmp1B2;struct _tuple10 _tmp1B3;struct
Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B5;struct Cyc_List_List*_tmp1B6;
struct Cyc_List_List*_tmp1B7;struct Cyc_List_List _tmp1B8;struct _tuple10*_tmp1B9;
struct _tuple10 _tmp1BA;struct Cyc_List_List*_tmp1BB;struct Cyc_List_List*_tmp1BC;
struct Cyc_List_List*_tmp1BD;struct Cyc_List_List*_tmp1BE;_LL117: if(_tmp1B0 != 0)
goto _LL119;_LL118: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp1BF=
_region_malloc(r,sizeof(*_tmp1BF));_tmp1BF[0]=({struct Cyc_Tcpat_Success_struct
_tmp1C0;_tmp1C0.tag=0;_tmp1C0.f1=(void*)right_hand_side;_tmp1C0;});_tmp1BF;});
_LL119: if(_tmp1B0 == 0)goto _LL11B;_tmp1B1=*_tmp1B0;_tmp1B2=(struct _tuple10*)
_tmp1B1.hd;_tmp1B3=*_tmp1B2;_tmp1B4=_tmp1B3.f1;if(_tmp1B4 != 0)goto _LL11B;_tmp1B5=
_tmp1B3.f2;if(_tmp1B5 != 0)goto _LL11B;_tmp1B6=_tmp1B3.f3;if(_tmp1B6 != 0)goto
_LL11B;_tmp1B7=_tmp1B1.tl;_LL11A: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp1B7,right_hand_side,rules);_LL11B: if(_tmp1B0 == 0)goto _LL116;_tmp1B8=*
_tmp1B0;_tmp1B9=(struct _tuple10*)_tmp1B8.hd;_tmp1BA=*_tmp1B9;_tmp1BB=_tmp1BA.f1;
_tmp1BC=_tmp1BA.f2;_tmp1BD=_tmp1BA.f3;_tmp1BE=_tmp1B8.tl;_LL11C: if((_tmp1BB == 0?
1: _tmp1BC == 0)?1: _tmp1BD == 0)({void*_tmp1C1[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcpat:and_match: malformed work frame",
sizeof(char),38),_tag_arr(_tmp1C1,sizeof(void*),0));});{struct Cyc_List_List
_tmp1C3;void*_tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_List_List*_tmp1C2=(
struct Cyc_List_List*)_check_null(_tmp1BB);_tmp1C3=*_tmp1C2;_tmp1C4=(void*)
_tmp1C3.hd;_tmp1C5=_tmp1C3.tl;{struct Cyc_List_List _tmp1C7;struct Cyc_List_List*
_tmp1C8;struct Cyc_List_List*_tmp1C9;struct Cyc_List_List*_tmp1C6=(struct Cyc_List_List*)
_check_null(_tmp1BC);_tmp1C7=*_tmp1C6;_tmp1C8=(struct Cyc_List_List*)_tmp1C7.hd;
_tmp1C9=_tmp1C7.tl;{struct Cyc_List_List _tmp1CB;void*_tmp1CC;struct Cyc_List_List*
_tmp1CD;struct Cyc_List_List*_tmp1CA=(struct Cyc_List_List*)_check_null(_tmp1BD);
_tmp1CB=*_tmp1CA;_tmp1CC=(void*)_tmp1CB.hd;_tmp1CD=_tmp1CB.tl;{struct _tuple10*
_tmp1CE=({struct _tuple10*_tmp1D0=_region_malloc(r,sizeof(*_tmp1D0));_tmp1D0->f1=
_tmp1C5;_tmp1D0->f2=_tmp1C9;_tmp1D0->f3=_tmp1CD;_tmp1D0;});return Cyc_Tcpat_match(
r,_tmp1C4,_tmp1C8,_tmp1CC,ctx,({struct Cyc_List_List*_tmp1CF=_region_malloc(r,
sizeof(*_tmp1CF));_tmp1CF->hd=_tmp1CE;_tmp1CF->tl=_tmp1BE;_tmp1CF;}),
right_hand_side,rules);}}}}_LL116:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp1D1=dsc;
struct Cyc_Set_Set*_tmp1D2;struct Cyc_List_List*_tmp1D3;_LL11E: if(*((int*)_tmp1D1)
!= 1)goto _LL120;_tmp1D2=((struct Cyc_Tcpat_Neg_struct*)_tmp1D1)->f1;_LL11F: {void*
any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp1D6=_region_malloc(r,sizeof(*_tmp1D6));
_tmp1D6[0]=({struct Cyc_Tcpat_Neg_struct _tmp1D7;_tmp1D7.tag=1;_tmp1D7.f1=Cyc_Tcpat_empty_con_set(
r);_tmp1D7;});_tmp1D6;});struct Cyc_List_List*_tmp1D4=0;{int i=0;for(0;i < pcon->arity;
++ i){_tmp1D4=({struct Cyc_List_List*_tmp1D5=_region_malloc(r,sizeof(*_tmp1D5));
_tmp1D5->hd=(void*)any;_tmp1D5->tl=_tmp1D4;_tmp1D5;});}}return _tmp1D4;}_LL120:
if(*((int*)_tmp1D1)!= 0)goto _LL11D;_tmp1D3=((struct Cyc_Tcpat_Pos_struct*)_tmp1D1)->f2;
_LL121: return _tmp1D3;_LL11D:;}struct _tuple12{struct _RegionHandle*f1;struct Cyc_List_List*
f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple12*env,int i){struct
_tuple12 _tmp1D9;struct _RegionHandle*_tmp1DA;struct Cyc_List_List*_tmp1DB;struct
_tuple12*_tmp1D8=env;_tmp1D9=*_tmp1D8;_tmp1DA=_tmp1D9.f1;_tmp1DB=_tmp1D9.f2;
return({struct Cyc_List_List*_tmp1DC=_region_malloc(_tmp1DA,sizeof(*_tmp1DC));
_tmp1DC->hd=(void*)(i + 1);_tmp1DC->tl=_tmp1DB;_tmp1DC;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*
obj){struct _tuple12 _tmp1DD=({struct _tuple12 _tmp1DE;_tmp1DE.f1=r;_tmp1DE.f2=obj;
_tmp1DE;});return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple12*,int),struct _tuple12*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp1DD);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp1DF=p;struct Cyc_Tcpat_Con_s*
_tmp1E0;struct Cyc_List_List*_tmp1E1;_LL123: if((int)_tmp1DF != 0)goto _LL125;_LL124:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL125: if(_tmp1DF <= (void*)1?1:*((int*)_tmp1DF)!= 0)goto _LL122;_tmp1E0=((
struct Cyc_Tcpat_Con_struct*)_tmp1DF)->f1;_tmp1E1=((struct Cyc_Tcpat_Con_struct*)
_tmp1DF)->f2;_LL126: {void*_tmp1E2=Cyc_Tcpat_static_match(_tmp1E0,dsc);_LL128:
if((int)_tmp1E2 != 0)goto _LL12A;_LL129: {struct Cyc_List_List*_tmp1E3=({struct Cyc_List_List*
_tmp1E8=_region_malloc(r,sizeof(*_tmp1E8));_tmp1E8->hd=({struct _tuple11*_tmp1E9=
_region_malloc(r,sizeof(*_tmp1E9));_tmp1E9->f1=_tmp1E0;_tmp1E9->f2=0;_tmp1E9;});
_tmp1E8->tl=ctx;_tmp1E8;});struct _tuple10*_tmp1E4=({struct _tuple10*_tmp1E7=
_region_malloc(r,sizeof(*_tmp1E7));_tmp1E7->f1=_tmp1E1;_tmp1E7->f2=Cyc_Tcpat_getoargs(
r,_tmp1E0,obj);_tmp1E7->f3=Cyc_Tcpat_getdargs(r,_tmp1E0,dsc);_tmp1E7;});struct
Cyc_List_List*_tmp1E5=({struct Cyc_List_List*_tmp1E6=_region_malloc(r,sizeof(*
_tmp1E6));_tmp1E6->hd=_tmp1E4;_tmp1E6->tl=work;_tmp1E6;});return Cyc_Tcpat_and_match(
r,_tmp1E3,_tmp1E5,right_hand_side,rules);}_LL12A: if((int)_tmp1E2 != 1)goto _LL12C;
_LL12B: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL12C: if((int)_tmp1E2 != 2)goto _LL127;_LL12D: {struct Cyc_List_List*_tmp1EA=({
struct Cyc_List_List*_tmp1F3=_region_malloc(r,sizeof(*_tmp1F3));_tmp1F3->hd=({
struct _tuple11*_tmp1F4=_region_malloc(r,sizeof(*_tmp1F4));_tmp1F4->f1=_tmp1E0;
_tmp1F4->f2=0;_tmp1F4;});_tmp1F3->tl=ctx;_tmp1F3;});struct _tuple10*_tmp1EB=({
struct _tuple10*_tmp1F2=_region_malloc(r,sizeof(*_tmp1F2));_tmp1F2->f1=_tmp1E1;
_tmp1F2->f2=Cyc_Tcpat_getoargs(r,_tmp1E0,obj);_tmp1F2->f3=Cyc_Tcpat_getdargs(r,
_tmp1E0,dsc);_tmp1F2;});struct Cyc_List_List*_tmp1EC=({struct Cyc_List_List*
_tmp1F1=_region_malloc(r,sizeof(*_tmp1F1));_tmp1F1->hd=_tmp1EB;_tmp1F1->tl=work;
_tmp1F1;});void*_tmp1ED=Cyc_Tcpat_and_match(r,_tmp1EA,_tmp1EC,right_hand_side,
rules);void*_tmp1EE=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(
r,dsc,_tmp1E0),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp1EF=
_region_malloc(r,sizeof(*_tmp1EF));_tmp1EF[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp1F0;_tmp1F0.tag=1;_tmp1F0.f1=obj;_tmp1F0.f2=_tmp1E0;_tmp1F0.f3=(void*)
_tmp1ED;_tmp1F0.f4=(void*)_tmp1EE;_tmp1F0;});_tmp1EF;});}_LL127:;}_LL122:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*),void*
env1,void(*rhs_appears)(void*,void*),void*env2){void*_tmp1F5=d;void*_tmp1F6;void*
_tmp1F7;void*_tmp1F8;_LL12F: if((int)_tmp1F5 != 0)goto _LL131;_LL130: not_exhaust(
env1);goto _LL12E;_LL131: if(_tmp1F5 <= (void*)1?1:*((int*)_tmp1F5)!= 0)goto _LL133;
_tmp1F6=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp1F5)->f1;_LL132: rhs_appears(
env2,_tmp1F6);goto _LL12E;_LL133: if(_tmp1F5 <= (void*)1?1:*((int*)_tmp1F5)!= 1)
goto _LL12E;_tmp1F7=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp1F5)->f3;_tmp1F8=(
void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp1F5)->f4;_LL134: Cyc_Tcpat_check_exhaust_overlap(
_tmp1F7,not_exhaust,env1,rhs_appears,env2);Cyc_Tcpat_check_exhaust_overlap(
_tmp1F8,not_exhaust,env1,rhs_appears,env2);goto _LL12E;_LL12E:;}struct _tuple13{
int f1;struct Cyc_Position_Segment*f2;};struct _tuple14{void*f1;struct _tuple13*f2;}
;struct _tuple15{struct _RegionHandle*f1;int*f2;};static struct _tuple14*Cyc_Tcpat_get_match(
struct _tuple15*env,struct Cyc_Absyn_Switch_clause*swc){struct _tuple15 _tmp1FA;
struct _RegionHandle*_tmp1FB;int*_tmp1FC;struct _tuple15*_tmp1F9=env;_tmp1FA=*
_tmp1F9;_tmp1FB=_tmp1FA.f1;_tmp1FC=_tmp1FA.f2;{void*sp0=Cyc_Tcpat_compile_pat(
swc->pattern);struct _tuple13*rhs=({struct _tuple13*_tmp202=_region_malloc(_tmp1FB,
sizeof(*_tmp202));_tmp202->f1=0;_tmp202->f2=(swc->pattern)->loc;_tmp202;});void*
sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*
_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->hd=(void*)sp0;_tmp1FD->tl=({struct
Cyc_List_List*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->hd=(void*)Cyc_Tcpat_int_pat(*
_tmp1FC);_tmp1FE->tl=0;_tmp1FE;});_tmp1FD;}));*_tmp1FC=*_tmp1FC + 1;}else{sp=Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->hd=(void*)sp0;
_tmp1FF->tl=({struct Cyc_List_List*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->hd=(
void*)((void*)0);_tmp200->tl=0;_tmp200;});_tmp1FF;}));}return({struct _tuple14*
_tmp201=_region_malloc(_tmp1FB,sizeof(*_tmp201));_tmp201->f1=sp;_tmp201->f2=rhs;
_tmp201;});}}static void Cyc_Tcpat_not_exhaust_err(struct Cyc_Position_Segment*loc){({
void*_tmp203[0]={};Cyc_Tcutil_terr(loc,_tag_arr("patterns may not be exhaustive",
sizeof(char),31),_tag_arr(_tmp203,sizeof(void*),0));});}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple13*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*swcs){struct _RegionHandle
_tmp204=_new_region("r");struct _RegionHandle*r=& _tmp204;_push_region(r);{int
_tmp205=0;struct _tuple15 _tmp206=({struct _tuple15 _tmp210;_tmp210.f1=r;_tmp210.f2=&
_tmp205;_tmp210;});struct Cyc_List_List*_tmp207=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple14*(*f)(struct _tuple15*,struct Cyc_Absyn_Switch_clause*),
struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,&
_tmp206,swcs);void*_tmp208=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(r,_tmp207);((void(*)(void*d,void(*
not_exhaust)(struct Cyc_Position_Segment*),struct Cyc_Position_Segment*env1,void(*
rhs_appears)(int,struct _tuple13*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp208,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp207 != 0;
_tmp207=_tmp207->tl){struct _tuple14 _tmp20A;struct _tuple13*_tmp20B;struct _tuple13
_tmp20C;int _tmp20D;struct Cyc_Position_Segment*_tmp20E;struct _tuple14*_tmp209=(
struct _tuple14*)_tmp207->hd;_tmp20A=*_tmp209;_tmp20B=_tmp20A.f2;_tmp20C=*_tmp20B;
_tmp20D=_tmp20C.f1;_tmp20E=_tmp20C.f2;if(!_tmp20D)({void*_tmp20F[0]={};Cyc_Tcutil_terr(
_tmp20E,_tag_arr("redundant pattern",sizeof(char),18),_tag_arr(_tmp20F,sizeof(
void*),0));});}};_pop_region(r);}struct _tuple16{struct Cyc_Position_Segment*f1;
int f2;};static void Cyc_Tcpat_not_exhaust_warn(struct _tuple16*pr){(*pr).f2=0;({
void*_tmp211[0]={};Cyc_Tcutil_warn((*pr).f1,_tag_arr("pattern not exhaustive",
sizeof(char),23),_tag_arr(_tmp211,sizeof(void*),0));});}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple17{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp212=
_new_region("r");struct _RegionHandle*r=& _tmp212;_push_region(r);{struct Cyc_List_List*
_tmp213=({struct Cyc_List_List*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->hd=({
struct _tuple17*_tmp219=_region_malloc(r,sizeof(*_tmp219));_tmp219->f1=Cyc_Tcpat_compile_pat(
p);_tmp219->f2=0;_tmp219;});_tmp218->tl=0;_tmp218;});void*_tmp214=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp213);struct _tuple16 _tmp215=({struct _tuple16 _tmp217;_tmp217.f1=loc;_tmp217.f2=
1;_tmp217;});((void(*)(void*d,void(*not_exhaust)(struct _tuple16*),struct _tuple16*
env1,void(*rhs_appears)(int,int),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp214,Cyc_Tcpat_not_exhaust_warn,& _tmp215,Cyc_Tcpat_dummy_fn,0);{int _tmp216=
_tmp215.f2;_npop_handler(0);return _tmp216;}};_pop_region(r);}static struct
_tuple14*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){void*sp0=Cyc_Tcpat_compile_pat(
swc->pattern);struct _tuple13*rhs=({struct _tuple13*_tmp21B=_cycalloc(sizeof(*
_tmp21B));_tmp21B->f1=0;_tmp21B->f2=(swc->pattern)->loc;_tmp21B;});return({
struct _tuple14*_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A->f1=sp0;_tmp21A->f2=
rhs;_tmp21A;});}static void Cyc_Tcpat_not_exhaust_err2(struct Cyc_Position_Segment*
loc){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,struct
Cyc_List_List*swcs){struct Cyc_List_List*_tmp21C=((struct Cyc_List_List*(*)(struct
_tuple14*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp21D=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(Cyc_Core_heap_region,_tmp21C);((
void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),struct Cyc_Position_Segment*
env1,void(*rhs_appears)(int,struct _tuple13*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp21D,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp21C != 
0;_tmp21C=_tmp21C->tl){struct _tuple14 _tmp21F;struct _tuple13*_tmp220;struct
_tuple13 _tmp221;int _tmp222;struct Cyc_Position_Segment*_tmp223;struct _tuple14*
_tmp21E=(struct _tuple14*)_tmp21C->hd;_tmp21F=*_tmp21E;_tmp220=_tmp21F.f2;_tmp221=*
_tmp220;_tmp222=_tmp221.f1;_tmp223=_tmp221.f2;if(!_tmp222)({void*_tmp224[0]={};
Cyc_Tcutil_terr(_tmp223,_tag_arr("redundant pattern",sizeof(char),18),_tag_arr(
_tmp224,sizeof(void*),0));});}}
