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
void*hd;struct Cyc_List_List*tl;};void*Cyc_List_hd(struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Iter_Iter
Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);struct Cyc_Std___cycFILE;
extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr,
struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);void*Cyc_Dict_fold_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d,void*accum);void Cyc_Dict_iter_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*
Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*
Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2);struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(void*(*f)(
void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,
struct Cyc_Dict_Dict*d2);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);int Cyc_Std_zstrptrcmp(struct
_tagged_arr*,struct _tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr
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
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct _tagged_arr*Cyc_Absyn_fieldname(int);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);int Cyc_Absyn_is_union_type(
void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
ae;struct Cyc_Core_Opt*le;};void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero";struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero";
struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[
13]="\000\000\000\000UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};
struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*
f2;};struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();extern void*Cyc_CfFlowInfo_unknown_none;
extern void*Cyc_CfFlowInfo_unknown_this;extern void*Cyc_CfFlowInfo_unknown_all;
extern void*Cyc_CfFlowInfo_esc_none;extern void*Cyc_CfFlowInfo_esc_this;extern void*
Cyc_CfFlowInfo_esc_all;int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r);struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,
struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**,void*,void*);void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**,
void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);void*Cyc_CfFlowInfo_kill_flow_region(
void*f,void*rgn);static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_none_v={
0,(void*)((void*)0)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_this_v={
0,(void*)((void*)1)};static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_all_v={
0,(void*)((void*)2)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_none_v={
1,(void*)((void*)0)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_this_v={
1,(void*)((void*)1)};static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_all_v={
1,(void*)((void*)2)};void*Cyc_CfFlowInfo_unknown_none=(void*)& Cyc_CfFlowInfo_unknown_none_v;
void*Cyc_CfFlowInfo_unknown_this=(void*)& Cyc_CfFlowInfo_unknown_this_v;void*Cyc_CfFlowInfo_unknown_all=(
void*)& Cyc_CfFlowInfo_unknown_all_v;void*Cyc_CfFlowInfo_esc_none=(void*)& Cyc_CfFlowInfo_esc_none_v;
void*Cyc_CfFlowInfo_esc_this=(void*)& Cyc_CfFlowInfo_esc_this_v;void*Cyc_CfFlowInfo_esc_all=(
void*)& Cyc_CfFlowInfo_esc_all_v;static struct Cyc_Set_Set**Cyc_CfFlowInfo_mt_place_set_opt=
0;struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set(){if(Cyc_CfFlowInfo_mt_place_set_opt
== 0)Cyc_CfFlowInfo_mt_place_set_opt=({struct Cyc_Set_Set**_tmp6=_cycalloc(
sizeof(*_tmp6));_tmp6[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*)))Cyc_Set_empty)(Cyc_CfFlowInfo_place_cmp);_tmp6;});
return*((struct Cyc_Set_Set**)_check_null(Cyc_CfFlowInfo_mt_place_set_opt));}int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{struct
_tuple0 _tmp8=({struct _tuple0 _tmp7;_tmp7.f1=r1;_tmp7.f2=r2;_tmp7;});void*_tmp9;
struct Cyc_Absyn_Vardecl*_tmpA;void*_tmpB;struct Cyc_Absyn_Vardecl*_tmpC;void*
_tmpD;void*_tmpE;void*_tmpF;struct Cyc_Absyn_Exp*_tmp10;void*_tmp11;struct Cyc_Absyn_Exp*
_tmp12;void*_tmp13;void*_tmp14;void*_tmp15;int _tmp16;void*_tmp17;int _tmp18;_LL1:
_tmp9=_tmp8.f1;if(*((int*)_tmp9)!= 0)goto _LL3;_tmpA=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp9)->f1;_tmpB=_tmp8.f2;if(*((int*)_tmpB)!= 0)goto _LL3;_tmpC=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmpB)->f1;_LL2: return(int)_tmpA - (int)_tmpC;_LL3: _tmpD=_tmp8.f1;if(*((int*)
_tmpD)!= 0)goto _LL5;_LL4: return - 1;_LL5: _tmpE=_tmp8.f2;if(*((int*)_tmpE)!= 0)goto
_LL7;_LL6: return 1;_LL7: _tmpF=_tmp8.f1;if(*((int*)_tmpF)!= 1)goto _LL9;_tmp10=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmpF)->f1;_tmp11=_tmp8.f2;if(*((int*)
_tmp11)!= 1)goto _LL9;_tmp12=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp11)->f1;
_LL8: return(int)_tmp10 - (int)_tmp12;_LL9: _tmp13=_tmp8.f1;if(*((int*)_tmp13)!= 1)
goto _LLB;_LLA: return - 1;_LLB: _tmp14=_tmp8.f2;if(*((int*)_tmp14)!= 1)goto _LLD;_LLC:
return 1;_LLD: _tmp15=_tmp8.f1;if(*((int*)_tmp15)!= 2)goto _LL0;_tmp16=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp15)->f1;_tmp17=_tmp8.f2;if(*((int*)_tmp17)!= 2)goto _LL0;_tmp18=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp17)->f1;_LLE: return _tmp16 - _tmp18;_LL0:;}}int Cyc_CfFlowInfo_place_cmp(struct
Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)
return 0;{int i=Cyc_CfFlowInfo_root_cmp((void*)p1->root,(void*)p2->root);if(i != 0)
return i;return((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct
Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Std_zstrptrcmp,
p1->fields,p2->fields);}}static struct _tagged_arr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*sl=0;{void*_tmp19=(void*)p->root;
struct Cyc_Absyn_Vardecl*_tmp1A;struct Cyc_Absyn_Exp*_tmp1B;int _tmp1C;_LL10: if(*((
int*)_tmp19)!= 0)goto _LL12;_tmp1A=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp19)->f1;
_LL11: sl=({struct Cyc_List_List*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->hd=({
struct _tagged_arr*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct Cyc_Std_String_pa_struct
_tmp20;_tmp20.tag=0;_tmp20.f1=(struct _tagged_arr)*(*_tmp1A->name).f2;{void*
_tmp1F[1]={& _tmp20};Cyc_Std_aprintf(_tag_arr("%s",sizeof(char),3),_tag_arr(
_tmp1F,sizeof(void*),1));}});_tmp1E;});_tmp1D->tl=sl;_tmp1D;});goto _LLF;_LL12:
if(*((int*)_tmp19)!= 1)goto _LL14;_tmp1B=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp19)->f1;_LL13: sl=({struct Cyc_List_List*_tmp21=_cycalloc(sizeof(*_tmp21));
_tmp21->hd=({struct _tagged_arr*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=({
struct Cyc_Std_Int_pa_struct _tmp24;_tmp24.tag=1;_tmp24.f1=(int)((unsigned int)((
int)_tmp1B));{void*_tmp23[1]={& _tmp24};Cyc_Std_aprintf(_tag_arr("mpt%d",sizeof(
char),6),_tag_arr(_tmp23,sizeof(void*),1));}});_tmp22;});_tmp21->tl=sl;_tmp21;});
goto _LLF;_LL14: if(*((int*)_tmp19)!= 2)goto _LLF;_tmp1C=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp19)->f1;_LL15: sl=({struct Cyc_List_List*_tmp25=_cycalloc(sizeof(*_tmp25));
_tmp25->hd=({struct _tagged_arr*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26[0]=({
struct Cyc_Std_Int_pa_struct _tmp28;_tmp28.tag=1;_tmp28.f1=(int)((unsigned int)
_tmp1C);{void*_tmp27[1]={& _tmp28};Cyc_Std_aprintf(_tag_arr("param%d",sizeof(char),
8),_tag_arr(_tmp27,sizeof(void*),1));}});_tmp26;});_tmp25->tl=sl;_tmp25;});goto
_LLF;_LLF:;}{struct Cyc_List_List*fields=p->fields;for(0;fields != 0;fields=fields->tl){
sl=({struct Cyc_List_List*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->hd=({struct
_tagged_arr*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Std_String_pa_struct
_tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)*((struct _tagged_arr*)fields->hd);{
void*_tmp2B[1]={& _tmp2C};Cyc_Std_aprintf(_tag_arr("%s",sizeof(char),3),_tag_arr(
_tmp2B,sizeof(void*),1));}});_tmp2A;});_tmp29->tl=sl;_tmp29;});}}{struct
_tagged_arr*_tmp2D=({struct _tagged_arr*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31[
0]=({struct Cyc_Std_String_pa_struct _tmp33;_tmp33.tag=0;_tmp33.f1=_tag_arr("",
sizeof(char),1);{void*_tmp32[1]={& _tmp33};Cyc_Std_aprintf(_tag_arr("%s",sizeof(
char),3),_tag_arr(_tmp32,sizeof(void*),1));}});_tmp31;});for(0;sl != 0;sl=sl->tl){*
_tmp2D=({struct Cyc_Std_String_pa_struct _tmp30;_tmp30.tag=0;_tmp30.f1=(struct
_tagged_arr)*_tmp2D;{struct Cyc_Std_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(
struct _tagged_arr)*((struct _tagged_arr*)sl->hd);{void*_tmp2E[2]={& _tmp2F,& _tmp30};
Cyc_Std_aprintf(_tag_arr("%s.%s",sizeof(char),6),_tag_arr(_tmp2E,sizeof(void*),2));}}});}
return _tmp2D;}}struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval){if(!Cyc_Absyn_is_union_type(t)){void*_tmp34=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_TunionFieldInfo _tmp35;void*_tmp36;struct Cyc_Absyn_Tunionfield*
_tmp37;struct Cyc_List_List*_tmp38;struct Cyc_Absyn_AggrInfo _tmp39;void*_tmp3A;
void*_tmp3B;struct Cyc_List_List*_tmp3C;_LL17: if(_tmp34 <= (void*)3?1:*((int*)
_tmp34)!= 3)goto _LL19;_tmp35=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp34)->f1;
_tmp36=(void*)_tmp35.field_info;if(*((int*)_tmp36)!= 1)goto _LL19;_tmp37=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp36)->f2;_LL18: if(_tmp37->typs == 0)return
leafval;_tmp38=_tmp37->typs;goto _LL1A;_LL19: if(_tmp34 <= (void*)3?1:*((int*)
_tmp34)!= 9)goto _LL1B;_tmp38=((struct Cyc_Absyn_TupleType_struct*)_tmp34)->f1;
_LL1A: {struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{int i=0;
for(0;_tmp38 != 0;(_tmp38=_tmp38->tl,++ i)){d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,Cyc_Absyn_fieldname(i),Cyc_CfFlowInfo_typ_to_absrval((*((
struct _tuple4*)_tmp38->hd)).f2,leafval));}}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp3E;_tmp3E.tag=3;_tmp3E.f1=d;_tmp3E;});_tmp3D;});}_LL1B: if(_tmp34 <= (void*)3?
1:*((int*)_tmp34)!= 10)goto _LL1D;_tmp39=((struct Cyc_Absyn_AggrType_struct*)
_tmp34)->f1;_tmp3A=(void*)_tmp39.aggr_info;_LL1C: {struct Cyc_Absyn_Aggrdecl*
_tmp3F=Cyc_Absyn_get_known_aggrdecl(_tmp3A);if(_tmp3F->impl == 0)goto _LL16;_tmp3C=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3F->impl))->fields;goto _LL1E;}
_LL1D: if(_tmp34 <= (void*)3?1:*((int*)_tmp34)!= 11)goto _LL1F;_tmp3B=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp34)->f1;if((int)_tmp3B != 0)goto _LL1F;
_tmp3C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp34)->f2;_LL1E: {struct Cyc_Dict_Dict*
d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))
Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);for(0;_tmp3C != 0;_tmp3C=_tmp3C->tl){struct
Cyc_Absyn_Aggrfield _tmp41;struct _tagged_arr*_tmp42;void*_tmp43;struct Cyc_Absyn_Aggrfield*
_tmp40=(struct Cyc_Absyn_Aggrfield*)_tmp3C->hd;_tmp41=*_tmp40;_tmp42=_tmp41.name;
_tmp43=(void*)_tmp41.type;if(_get_arr_size(*_tmp42,sizeof(char))!= 1)d=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(
d,_tmp42,Cyc_CfFlowInfo_typ_to_absrval(_tmp43,leafval));}return(void*)({struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp45;_tmp45.tag=3;_tmp45.f1=d;_tmp45;});
_tmp44;});}_LL1F:;_LL20: goto _LL16;_LL16:;}return Cyc_Tcutil_bits_only(t)?Cyc_CfFlowInfo_unknown_all:
leafval;}static int Cyc_CfFlowInfo_prefix_of_member(struct _RegionHandle*r,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Set_Set*set){struct Cyc_CfFlowInfo_Place*
_tmp46=place;struct Cyc_Iter_Iter _tmp47=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(r,set);while(((int(*)(
struct Cyc_Iter_Iter,struct Cyc_CfFlowInfo_Place**))Cyc_Iter_next)(_tmp47,& _tmp46)){
if(Cyc_CfFlowInfo_root_cmp((void*)place->root,(void*)_tmp46->root)!= 0)continue;{
struct Cyc_List_List*_tmp48=place->fields;struct Cyc_List_List*_tmp49=_tmp46->fields;
for(0;_tmp48 != 0?_tmp49 != 0: 0;(_tmp48=_tmp48->tl,_tmp49=_tmp49->tl)){if(Cyc_Std_zstrptrcmp((
struct _tagged_arr*)_tmp48->hd,(struct _tagged_arr*)_tmp49->hd)!= 0)break;}if(
_tmp48 == 0)return 1;}}return 0;}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*
rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))pile->places=({struct
Cyc_List_List*_tmp4A=_region_malloc(pile->rgn,sizeof(*_tmp4A));_tmp4A->hd=place;
_tmp4A->tl=pile->places;_tmp4A;});}static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*a,void*r){void*_tmp4B=r;struct Cyc_CfFlowInfo_Place*_tmp4C;struct Cyc_Dict_Dict*
_tmp4D;_LL22: if(_tmp4B <= (void*)3?1:*((int*)_tmp4B)!= 2)goto _LL24;_tmp4C=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4B)->f1;_LL23: Cyc_CfFlowInfo_add_place(
pile,_tmp4C);return;_LL24: if(_tmp4B <= (void*)3?1:*((int*)_tmp4B)!= 3)goto _LL26;
_tmp4D=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp4B)->f1;_LL25:((void(*)(void(*
f)(struct Cyc_CfFlowInfo_EscPile*,struct _tagged_arr*,void*),struct Cyc_CfFlowInfo_EscPile*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)((void(*)(struct Cyc_CfFlowInfo_EscPile*
pile,struct _tagged_arr*a,void*r))Cyc_CfFlowInfo_add_places,pile,_tmp4D);return;
_LL26:;_LL27: return;_LL21:;}static void*Cyc_CfFlowInfo_insert_place_inner(void*
new_val,void*old_val){void*_tmp4E=old_val;struct Cyc_Dict_Dict*_tmp4F;_LL29: if(
_tmp4E <= (void*)3?1:*((int*)_tmp4E)!= 3)goto _LL2B;_tmp4F=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp4E)->f1;_LL2A: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp50=
_cycalloc(sizeof(*_tmp50));_tmp50[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp51;_tmp51.tag=3;_tmp51.f1=((struct Cyc_Dict_Dict*(*)(void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_insert_place_inner,
new_val,_tmp4F);_tmp51;});_tmp50;});_LL2B:;_LL2C: return new_val;_LL28:;}struct
_tuple5{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
new_val,old_val);{struct _tuple5 _tmp53=({struct _tuple5 _tmp52;_tmp52.f1=fs;_tmp52.f2=
old_val;_tmp52;});struct Cyc_List_List*_tmp54;struct Cyc_List_List _tmp55;struct
_tagged_arr*_tmp56;struct Cyc_List_List*_tmp57;void*_tmp58;struct Cyc_Dict_Dict*
_tmp59;_LL2E: _tmp54=_tmp53.f1;if(_tmp54 == 0)goto _LL30;_tmp55=*_tmp54;_tmp56=(
struct _tagged_arr*)_tmp55.hd;_tmp57=_tmp55.tl;_tmp58=_tmp53.f2;if(_tmp58 <= (void*)
3?1:*((int*)_tmp58)!= 3)goto _LL30;_tmp59=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp58)->f1;_LL2F: {void*_tmp5A=Cyc_CfFlowInfo_insert_place_outer(_tmp57,((void*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp59,_tmp56),
new_val);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp5B=_cycalloc(
sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp5C;_tmp5C.tag=
3;_tmp5C.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,
void*v))Cyc_Dict_insert)(_tmp59,_tmp56,_tmp5A);_tmp5C;});_tmp5B;});}_LL30:;_LL31:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp5D=_cycalloc(sizeof(*
_tmp5D));_tmp5D[0]=({struct Cyc_Core_Impossible_struct _tmp5E;_tmp5E.tag=Cyc_Core_Impossible;
_tmp5E.f1=_tag_arr("bad insert place",sizeof(char),17);_tmp5E;});_tmp5D;}));
_LL2D:;}}static struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_Set_Set**all_changed,struct Cyc_Dict_Dict*d){while(pile->places != 
0){struct Cyc_CfFlowInfo_Place*_tmp5F=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)
_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)_check_null(
pile->places))->tl;if(all_changed != 0)*all_changed=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*all_changed,
_tmp5F);{void*oldval;void*newval;{struct _handler_cons _tmp60;_push_handler(&
_tmp60);{int _tmp62=0;if(setjmp(_tmp60.handler))_tmp62=1;if(!_tmp62){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp5F);;_pop_handler();}else{void*_tmp61=(void*)_exn_thrown;void*_tmp64=
_tmp61;_LL33: if(_tmp64 != Cyc_Dict_Absent)goto _LL35;_LL34: continue;_LL35:;_LL36:(
void)_throw(_tmp64);_LL32:;}}}{void*_tmp65=Cyc_CfFlowInfo_initlevel(d,oldval);
_LL38: if((int)_tmp65 != 2)goto _LL3A;_LL39: newval=Cyc_CfFlowInfo_esc_all;goto _LL37;
_LL3A: if((int)_tmp65 != 1)goto _LL3C;_LL3B: newval=Cyc_CfFlowInfo_esc_this;goto
_LL37;_LL3C: if((int)_tmp65 != 0)goto _LL37;_LL3D: newval=Cyc_CfFlowInfo_esc_none;
goto _LL37;_LL37:;}((void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,oldval);d=Cyc_Dict_insert(d,(void*)_tmp5F->root,Cyc_CfFlowInfo_insert_place_outer(
_tmp5F->fields,Cyc_Dict_lookup(d,(void*)_tmp5F->root),newval));}}return d;}struct
Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict*d;struct Cyc_List_List*seen;};
static char Cyc_CfFlowInfo_dummy_str_v[1]="";static struct _tagged_arr Cyc_CfFlowInfo_dummy_str={(
void*)((char*)Cyc_CfFlowInfo_dummy_str_v),(void*)((char*)Cyc_CfFlowInfo_dummy_str_v),(
void*)((char*)Cyc_CfFlowInfo_dummy_str_v + 1)};static void*Cyc_CfFlowInfo_initlevel_approx(
void*r){void*_tmp66=r;void*_tmp67;void*_tmp68;_LL3F: if(_tmp66 <= (void*)3?1:*((
int*)_tmp66)!= 0)goto _LL41;_tmp67=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp66)->f1;_LL40: return _tmp67;_LL41: if(_tmp66 <= (void*)3?1:*((int*)_tmp66)!= 1)
goto _LL43;_tmp68=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp66)->f1;_LL42:
return _tmp68;_LL43: if((int)_tmp66 != 0)goto _LL45;_LL44: goto _LL46;_LL45: if((int)
_tmp66 != 1)goto _LL47;_LL46: return(void*)2;_LL47: if((int)_tmp66 != 2)goto _LL49;
_LL48: return(void*)1;_LL49:;_LL4A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=({struct Cyc_Core_Impossible_struct
_tmp6A;_tmp6A.tag=Cyc_Core_Impossible;_tmp6A.f1=_tag_arr("initlevel_approx",
sizeof(char),17);_tmp6A;});_tmp69;}));_LL3E:;}static void*Cyc_CfFlowInfo_initlevel_rec(
struct Cyc_CfFlowInfo_InitlevelEnv*env,void*a,void*r,void*acc){void*this_ans;if(
acc == (void*)0)return(void*)0;{void*_tmp6B=r;struct Cyc_Dict_Dict*_tmp6C;struct
Cyc_CfFlowInfo_Place*_tmp6D;_LL4C: if(_tmp6B <= (void*)3?1:*((int*)_tmp6B)!= 3)
goto _LL4E;_tmp6C=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp6B)->f1;_LL4D:
this_ans=((void*(*)(void*(*f)(struct Cyc_CfFlowInfo_InitlevelEnv*,struct
_tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_InitlevelEnv*env,struct Cyc_Dict_Dict*
d,void*accum))Cyc_Dict_fold_c)((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,
struct _tagged_arr*a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec,env,_tmp6C,(
void*)2);goto _LL4B;_LL4E: if(_tmp6B <= (void*)3?1:*((int*)_tmp6B)!= 2)goto _LL50;
_tmp6D=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp6B)->f1;_LL4F: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp6D))this_ans=(void*)2;else{env->seen=({struct Cyc_List_List*_tmp6E=_cycalloc(
sizeof(*_tmp6E));_tmp6E->hd=_tmp6D;_tmp6E->tl=env->seen;_tmp6E;});this_ans=((
void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(
env,0,Cyc_CfFlowInfo_lookup_place(env->d,_tmp6D),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL4B;
_LL50:;_LL51: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL4B:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1?1: acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmp6F=({struct Cyc_CfFlowInfo_InitlevelEnv
_tmp70;_tmp70.d=d;_tmp70.seen=0;_tmp70;});return((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(& _tmp6F,0,r,(void*)2);}
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place){struct Cyc_CfFlowInfo_Place _tmp72;void*_tmp73;struct Cyc_List_List*_tmp74;
struct Cyc_CfFlowInfo_Place*_tmp71=place;_tmp72=*_tmp71;_tmp73=(void*)_tmp72.root;
_tmp74=_tmp72.fields;{void*_tmp75=Cyc_Dict_lookup(d,_tmp73);for(0;_tmp74 != 0;
_tmp74=_tmp74->tl){struct _tuple1 _tmp77=({struct _tuple1 _tmp76;_tmp76.f1=_tmp75;
_tmp76.f2=(struct _tagged_arr*)_tmp74->hd;_tmp76;});void*_tmp78;struct Cyc_Dict_Dict*
_tmp79;struct _tagged_arr*_tmp7A;_LL53: _tmp78=_tmp77.f1;if(_tmp78 <= (void*)3?1:*((
int*)_tmp78)!= 3)goto _LL55;_tmp79=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp78)->f1;_tmp7A=_tmp77.f2;_LL54: _tmp75=((void*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp79,_tmp7A);goto _LL52;_LL55:;_LL56:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));
_tmp7B[0]=({struct Cyc_Core_Impossible_struct _tmp7C;_tmp7C.tag=Cyc_Core_Impossible;
_tmp7C.f1=_tag_arr("bad lookup_place",sizeof(char),17);_tmp7C;});_tmp7B;}));
_LL52:;}return _tmp75;}}static int Cyc_CfFlowInfo_is_rval_unescaped(void*a,void*b,
void*rval){void*_tmp7D=rval;struct Cyc_Dict_Dict*_tmp7E;_LL58: if(_tmp7D <= (void*)
3?1:*((int*)_tmp7D)!= 1)goto _LL5A;_LL59: return 0;_LL5A: if(_tmp7D <= (void*)3?1:*((
int*)_tmp7D)!= 3)goto _LL5C;_tmp7E=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp7D)->f1;_LL5B: return((int(*)(int(*f)(int,struct _tagged_arr*,void*),int env,
struct Cyc_Dict_Dict*d))Cyc_Dict_forall_c)((int(*)(int a,struct _tagged_arr*b,void*
rval))Cyc_CfFlowInfo_is_rval_unescaped,0,_tmp7E);_LL5C:;_LL5D: return 1;_LL57:;}
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place){return((int(*)(int a,int b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped)(0,0,
Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r){struct _RegionHandle
_tmp7F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7F;_push_region(rgn);{
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp81=
_region_malloc(rgn,sizeof(*_tmp81));_tmp81->rgn=rgn;_tmp81->places=0;_tmp81;});((
void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,r);{struct Cyc_Dict_Dict*_tmp80=Cyc_CfFlowInfo_escape_these(pile,
all_changed,d);_npop_handler(0);return _tmp80;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict*d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*a,void*oldval,void*newval){struct _tuple0 _tmp83=({struct _tuple0 _tmp82;
_tmp82.f1=oldval;_tmp82.f2=newval;_tmp82;});void*_tmp84;void*_tmp85;struct Cyc_CfFlowInfo_Place*
_tmp86;void*_tmp87;void*_tmp88;struct Cyc_Dict_Dict*_tmp89;void*_tmp8A;struct Cyc_Dict_Dict*
_tmp8B;void*_tmp8C;void*_tmp8D;_LL5F: _tmp84=_tmp83.f1;if(_tmp84 <= (void*)3?1:*((
int*)_tmp84)!= 1)goto _LL61;_tmp85=_tmp83.f2;if(_tmp85 <= (void*)3?1:*((int*)
_tmp85)!= 2)goto _LL61;_tmp86=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp85)->f1;
_LL60: Cyc_CfFlowInfo_add_place(env->pile,_tmp86);goto _LL62;_LL61: _tmp87=_tmp83.f1;
if(_tmp87 <= (void*)3?1:*((int*)_tmp87)!= 1)goto _LL63;_LL62: if(Cyc_CfFlowInfo_initlevel(
env->d,newval)!= (void*)2)({void*_tmp8E[0]={};Cyc_Tcutil_terr(env->loc,_tag_arr("assignment puts possibly-uninitialized data in an escaped location",
sizeof(char),67),_tag_arr(_tmp8E,sizeof(void*),0));});return Cyc_CfFlowInfo_esc_all;
_LL63: _tmp88=_tmp83.f1;if(_tmp88 <= (void*)3?1:*((int*)_tmp88)!= 3)goto _LL65;
_tmp89=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp88)->f1;_tmp8A=_tmp83.f2;if(
_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 3)goto _LL65;_tmp8B=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp8A)->f1;_LL64: {struct Cyc_Dict_Dict*_tmp8F=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_AssignEnv*,struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,struct _tagged_arr*a,void*oldval,void*newval))
Cyc_CfFlowInfo_assign_place_inner,env,_tmp89,_tmp8B);if(_tmp8F == _tmp89)return
oldval;if(_tmp8F == _tmp8B)return newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp91;_tmp91.tag=3;_tmp91.f1=_tmp8F;_tmp91;});_tmp90;});}_LL65: _tmp8C=_tmp83.f2;
if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 1)goto _LL67;_tmp8D=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp8C)->f1;_LL66: {void*_tmp92=_tmp8D;_LL6A: if((int)_tmp92 != 0)goto _LL6C;_LL6B:
return Cyc_CfFlowInfo_unknown_none;_LL6C: if((int)_tmp92 != 1)goto _LL6E;_LL6D:
return Cyc_CfFlowInfo_unknown_this;_LL6E: if((int)_tmp92 != 2)goto _LL69;_LL6F:
return Cyc_CfFlowInfo_unknown_all;_LL69:;}_LL67:;_LL68: return newval;_LL5E:;}
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,
struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,int a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner)(
env,0,oldval,newval);{struct _tuple5 _tmp94=({struct _tuple5 _tmp93;_tmp93.f1=fs;
_tmp93.f2=oldval;_tmp93;});struct Cyc_List_List*_tmp95;struct Cyc_List_List _tmp96;
struct _tagged_arr*_tmp97;struct Cyc_List_List*_tmp98;void*_tmp99;struct Cyc_Dict_Dict*
_tmp9A;_LL71: _tmp95=_tmp94.f1;if(_tmp95 == 0)goto _LL73;_tmp96=*_tmp95;_tmp97=(
struct _tagged_arr*)_tmp96.hd;_tmp98=_tmp96.tl;_tmp99=_tmp94.f2;if(_tmp99 <= (void*)
3?1:*((int*)_tmp99)!= 3)goto _LL73;_tmp9A=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp99)->f1;_LL72: {void*_tmp9B=Cyc_CfFlowInfo_assign_place_outer(env,_tmp98,((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp9A,
_tmp97),newval);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp9C=
_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp9D;_tmp9D.tag=3;_tmp9D.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(_tmp9A,_tmp97,_tmp9B);_tmp9D;});
_tmp9C;});}_LL73:;_LL74:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({struct Cyc_Core_Impossible_struct
_tmp9F;_tmp9F.tag=Cyc_Core_Impossible;_tmp9F.f1=_tag_arr("bad insert place",
sizeof(char),17);_tmp9F;});_tmp9E;}));_LL70:;}}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){if(all_changed != 0)*
all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*
elt))Cyc_Set_insert)(*all_changed,place);{struct _RegionHandle _tmpA0=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpA0;_push_region(rgn);{struct Cyc_CfFlowInfo_Place
_tmpA2;void*_tmpA3;struct Cyc_List_List*_tmpA4;struct Cyc_CfFlowInfo_Place*_tmpA1=
place;_tmpA2=*_tmpA1;_tmpA3=(void*)_tmpA2.root;_tmpA4=_tmpA2.fields;{struct Cyc_CfFlowInfo_AssignEnv
env=({struct Cyc_CfFlowInfo_AssignEnv _tmpA6;_tmpA6.pile=({struct Cyc_CfFlowInfo_EscPile*
_tmpA7=_region_malloc(rgn,sizeof(*_tmpA7));_tmpA7->rgn=rgn;_tmpA7->places=0;
_tmpA7;});_tmpA6.d=d;_tmpA6.loc=loc;_tmpA6;});void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmpA4,Cyc_Dict_lookup(d,_tmpA3),r);struct Cyc_Dict_Dict*_tmpA5=Cyc_CfFlowInfo_escape_these(
env.pile,all_changed,Cyc_Dict_insert(d,_tmpA3,newval));_npop_handler(0);return
_tmpA5;}};_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict*d1;struct Cyc_Dict_Dict*d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Set_Set*
chg1;struct Cyc_Set_Set*chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static void*Cyc_CfFlowInfo_join_absRval(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{
struct _tuple0 _tmpA9=({struct _tuple0 _tmpA8;_tmpA8.f1=r1;_tmpA8.f2=r2;_tmpA8;});
void*_tmpAA;struct Cyc_CfFlowInfo_Place*_tmpAB;void*_tmpAC;struct Cyc_CfFlowInfo_Place*
_tmpAD;void*_tmpAE;struct Cyc_CfFlowInfo_Place*_tmpAF;void*_tmpB0;struct Cyc_CfFlowInfo_Place*
_tmpB1;void*_tmpB2;void*_tmpB3;void*_tmpB4;void*_tmpB5;void*_tmpB6;struct Cyc_Dict_Dict*
_tmpB7;void*_tmpB8;struct Cyc_Dict_Dict*_tmpB9;_LL76: _tmpAA=_tmpA9.f1;if(_tmpAA <= (
void*)3?1:*((int*)_tmpAA)!= 2)goto _LL78;_tmpAB=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpAA)->f1;_tmpAC=_tmpA9.f2;if(_tmpAC <= (void*)3?1:*((int*)_tmpAC)!= 2)goto
_LL78;_tmpAD=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpAC)->f1;_LL77: if(Cyc_CfFlowInfo_place_cmp(
_tmpAB,_tmpAD)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmpAB);Cyc_CfFlowInfo_add_place(
env->pile,_tmpAD);goto _LL75;_LL78: _tmpAE=_tmpA9.f1;if(_tmpAE <= (void*)3?1:*((int*)
_tmpAE)!= 2)goto _LL7A;_tmpAF=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpAE)->f1;
_LL79: Cyc_CfFlowInfo_add_place(env->pile,_tmpAF);goto _LL75;_LL7A: _tmpB0=_tmpA9.f2;
if(_tmpB0 <= (void*)3?1:*((int*)_tmpB0)!= 2)goto _LL7C;_tmpB1=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpB0)->f1;_LL7B: Cyc_CfFlowInfo_add_place(env->pile,_tmpB1);goto _LL75;_LL7C:
_tmpB2=_tmpA9.f1;if((int)_tmpB2 != 1)goto _LL7E;_tmpB3=_tmpA9.f2;if((int)_tmpB3 != 
2)goto _LL7E;_LL7D: goto _LL7F;_LL7E: _tmpB4=_tmpA9.f1;if((int)_tmpB4 != 2)goto _LL80;
_tmpB5=_tmpA9.f2;if((int)_tmpB5 != 1)goto _LL80;_LL7F: return(void*)2;_LL80: _tmpB6=
_tmpA9.f1;if(_tmpB6 <= (void*)3?1:*((int*)_tmpB6)!= 3)goto _LL82;_tmpB7=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmpB6)->f1;_tmpB8=_tmpA9.f2;if(_tmpB8 <= (void*)
3?1:*((int*)_tmpB8)!= 3)goto _LL82;_tmpB9=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpB8)->f1;_LL81: {struct Cyc_Dict_Dict*_tmpBA=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_JoinEnv*,struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,struct _tagged_arr*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,
env,_tmpB7,_tmpB9);if(_tmpBA == _tmpB7)return r1;if(_tmpBA == _tmpB9)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpBB=_cycalloc(sizeof(*
_tmpBB));_tmpBB[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpBC;_tmpBC.tag=3;
_tmpBC.f1=_tmpBA;_tmpBC;});_tmpBB;});}_LL82:;_LL83: goto _LL75;_LL75:;}{void*il1=
Cyc_CfFlowInfo_initlevel(env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(env->d2,
r2);struct _tuple0 _tmpBE=({struct _tuple0 _tmpBD;_tmpBD.f1=r1;_tmpBD.f2=r2;_tmpBD;});
void*_tmpBF;void*_tmpC0;_LL85: _tmpBF=_tmpBE.f1;if(_tmpBF <= (void*)3?1:*((int*)
_tmpBF)!= 1)goto _LL87;_LL86: goto _LL88;_LL87: _tmpC0=_tmpBE.f2;if(_tmpC0 <= (void*)
3?1:*((int*)_tmpC0)!= 1)goto _LL89;_LL88: {struct _tuple0 _tmpC2=({struct _tuple0
_tmpC1;_tmpC1.f1=il1;_tmpC1.f2=il2;_tmpC1;});void*_tmpC3;void*_tmpC4;void*_tmpC5;
void*_tmpC6;_LL8C: _tmpC3=_tmpC2.f2;if((int)_tmpC3 != 0)goto _LL8E;_LL8D: goto _LL8F;
_LL8E: _tmpC4=_tmpC2.f1;if((int)_tmpC4 != 0)goto _LL90;_LL8F: return Cyc_CfFlowInfo_esc_none;
_LL90: _tmpC5=_tmpC2.f2;if((int)_tmpC5 != 1)goto _LL92;_LL91: goto _LL93;_LL92: _tmpC6=
_tmpC2.f1;if((int)_tmpC6 != 1)goto _LL94;_LL93: return Cyc_CfFlowInfo_esc_this;_LL94:;
_LL95: return Cyc_CfFlowInfo_esc_all;_LL8B:;}_LL89:;_LL8A: {struct _tuple0 _tmpC8=({
struct _tuple0 _tmpC7;_tmpC7.f1=il1;_tmpC7.f2=il2;_tmpC7;});void*_tmpC9;void*
_tmpCA;void*_tmpCB;void*_tmpCC;_LL97: _tmpC9=_tmpC8.f2;if((int)_tmpC9 != 0)goto
_LL99;_LL98: goto _LL9A;_LL99: _tmpCA=_tmpC8.f1;if((int)_tmpCA != 0)goto _LL9B;_LL9A:
return Cyc_CfFlowInfo_unknown_none;_LL9B: _tmpCB=_tmpC8.f2;if((int)_tmpCB != 1)goto
_LL9D;_LL9C: goto _LL9E;_LL9D: _tmpCC=_tmpC8.f1;if((int)_tmpCC != 1)goto _LL9F;_LL9E:
return Cyc_CfFlowInfo_unknown_this;_LL9F:;_LLA0: return Cyc_CfFlowInfo_unknown_all;
_LL96:;}_LL84:;}}int Cyc_CfFlowInfo_same_relop(void*r1,void*r2){if(r1 == r2)return
1;{struct _tuple0 _tmpCE=({struct _tuple0 _tmpCD;_tmpCD.f1=r1;_tmpCD.f2=r2;_tmpCD;});
void*_tmpCF;unsigned int _tmpD0;void*_tmpD1;unsigned int _tmpD2;void*_tmpD3;struct
Cyc_Absyn_Vardecl*_tmpD4;void*_tmpD5;struct Cyc_Absyn_Vardecl*_tmpD6;void*_tmpD7;
struct Cyc_Absyn_Vardecl*_tmpD8;void*_tmpD9;struct Cyc_Absyn_Vardecl*_tmpDA;void*
_tmpDB;unsigned int _tmpDC;void*_tmpDD;unsigned int _tmpDE;void*_tmpDF;struct Cyc_Absyn_Vardecl*
_tmpE0;void*_tmpE1;struct Cyc_Absyn_Vardecl*_tmpE2;_LLA2: _tmpCF=_tmpCE.f1;if(*((
int*)_tmpCF)!= 0)goto _LLA4;_tmpD0=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmpCF)->f1;_tmpD1=_tmpCE.f2;if(*((int*)_tmpD1)!= 0)goto _LLA4;_tmpD2=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmpD1)->f1;_LLA3: return _tmpD0 == _tmpD2;_LLA4: _tmpD3=_tmpCE.f1;if(*((int*)_tmpD3)
!= 1)goto _LLA6;_tmpD4=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmpD3)->f1;_tmpD5=
_tmpCE.f2;if(*((int*)_tmpD5)!= 1)goto _LLA6;_tmpD6=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmpD5)->f1;_LLA5: return _tmpD4 == _tmpD6;_LLA6: _tmpD7=_tmpCE.f1;if(*((int*)_tmpD7)
!= 2)goto _LLA8;_tmpD8=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmpD7)->f1;_tmpD9=
_tmpCE.f2;if(*((int*)_tmpD9)!= 2)goto _LLA8;_tmpDA=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmpD9)->f1;_LLA7: return _tmpD8 == _tmpDA;_LLA8: _tmpDB=_tmpCE.f1;if(*((int*)_tmpDB)
!= 3)goto _LLAA;_tmpDC=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmpDB)->f1;
_tmpDD=_tmpCE.f2;if(*((int*)_tmpDD)!= 3)goto _LLAA;_tmpDE=((struct Cyc_CfFlowInfo_LessConst_struct*)
_tmpDD)->f1;_LLA9: return _tmpDC == _tmpDE;_LLAA: _tmpDF=_tmpCE.f1;if(*((int*)_tmpDF)
!= 4)goto _LLAC;_tmpE0=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmpDF)->f1;
_tmpE1=_tmpCE.f2;if(*((int*)_tmpE1)!= 4)goto _LLAC;_tmpE2=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmpE1)->f1;_LLAB: return _tmpE0 == _tmpE2;_LLAC:;_LLAD: return 0;_LLA1:;}}struct Cyc_List_List*
Cyc_CfFlowInfo_join_relns(struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(
r1s == r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*
_tmpE3=r1s;for(0;_tmpE3 != 0;_tmpE3=_tmpE3->tl){struct Cyc_CfFlowInfo_Reln*_tmpE4=(
struct Cyc_CfFlowInfo_Reln*)_tmpE3->hd;int found=0;{struct Cyc_List_List*_tmpE5=r2s;
for(0;_tmpE5 != 0;_tmpE5=_tmpE5->tl){struct Cyc_CfFlowInfo_Reln*_tmpE6=(struct Cyc_CfFlowInfo_Reln*)
_tmpE5->hd;if(_tmpE4 == _tmpE6?1:(_tmpE4->vd == _tmpE6->vd?Cyc_CfFlowInfo_same_relop((
void*)_tmpE4->rop,(void*)_tmpE6->rop): 0)){res=({struct Cyc_List_List*_tmpE7=
_cycalloc(sizeof(*_tmpE7));_tmpE7->hd=_tmpE4;_tmpE7->tl=res;_tmpE7;});found=1;
break;}}}if(!found)diff=1;}}if(!diff)res=r1s;return res;}}void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**all_changed,void*f1,void*f2){if(f1 == f2)return f1;{struct
_tuple0 _tmpE9=({struct _tuple0 _tmpE8;_tmpE8.f1=f1;_tmpE8.f2=f2;_tmpE8;});void*
_tmpEA;void*_tmpEB;void*_tmpEC;struct Cyc_Dict_Dict*_tmpED;struct Cyc_List_List*
_tmpEE;void*_tmpEF;struct Cyc_Dict_Dict*_tmpF0;struct Cyc_List_List*_tmpF1;_LLAF:
_tmpEA=_tmpE9.f1;if((int)_tmpEA != 0)goto _LLB1;_LLB0: return f2;_LLB1: _tmpEB=_tmpE9.f2;
if((int)_tmpEB != 0)goto _LLB3;_LLB2: return f1;_LLB3: _tmpEC=_tmpE9.f1;if(_tmpEC <= (
void*)1?1:*((int*)_tmpEC)!= 0)goto _LLAE;_tmpED=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpEC)->f1;_tmpEE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmpEC)->f2;_tmpEF=
_tmpE9.f2;if(_tmpEF <= (void*)1?1:*((int*)_tmpEF)!= 0)goto _LLAE;_tmpF0=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmpEF)->f1;_tmpF1=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmpEF)->f2;_LLB4: if(_tmpED == _tmpF0?_tmpEE == _tmpF1: 0)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle _tmpF2=_new_region("rgn");struct _RegionHandle*rgn=& _tmpF2;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmpF3=({struct Cyc_CfFlowInfo_JoinEnv
_tmpF9;_tmpF9.pile=({struct Cyc_CfFlowInfo_EscPile*_tmpFA=_region_malloc(rgn,
sizeof(*_tmpFA));_tmpFA->rgn=rgn;_tmpFA->places=0;_tmpFA;});_tmpF9.d1=_tmpED;
_tmpF9.d2=_tmpF0;_tmpF9;});struct Cyc_Dict_Dict*_tmpF4=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,&
_tmpF3,_tmpED,_tmpF0);struct Cyc_List_List*_tmpF5=Cyc_CfFlowInfo_join_relns(
_tmpEE,_tmpF1);void*_tmpF8=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmpF7;_tmpF7.tag=0;_tmpF7.f1=Cyc_CfFlowInfo_escape_these(_tmpF3.pile,
all_changed,_tmpF4);_tmpF7.f2=_tmpF5;_tmpF7;});_tmpF6;});_npop_handler(0);return
_tmpF8;};_pop_region(rgn);}_LLAE:;}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,struct _tagged_arr*,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){int changed1=env->changed == 
Cyc_CfFlowInfo_One?1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))
Cyc_Set_member)(env->chg1,env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two?
1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_member)(
env->chg2,env->curr_place);if(changed1?changed2: 0)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(((env->joinenv).pile)->rgn,env->curr_place,
env->chg2))return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(((
env->joinenv).pile)->rgn,env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{
struct _tuple0 _tmpFC=({struct _tuple0 _tmpFB;_tmpFB.f1=r1;_tmpFB.f2=r2;_tmpFB;});
void*_tmpFD;struct Cyc_Dict_Dict*_tmpFE;void*_tmpFF;struct Cyc_Dict_Dict*_tmp100;
_LLB6: _tmpFD=_tmpFC.f1;if(_tmpFD <= (void*)3?1:*((int*)_tmpFD)!= 3)goto _LLB8;
_tmpFE=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpFD)->f1;_tmpFF=_tmpFC.f2;if(
_tmpFF <= (void*)3?1:*((int*)_tmpFF)!= 3)goto _LLB8;_tmp100=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpFF)->f1;_LLB7: {struct Cyc_Dict_Dict*_tmp101=((struct Cyc_Dict_Dict*(*)(void*(*
f)(struct Cyc_CfFlowInfo_AfterEnv*,struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_absRval_d,
env,_tmpFE,_tmp100);if(_tmp101 == _tmpFE)return r1;if(_tmp101 == _tmp100)return r2;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp102=_cycalloc(sizeof(*
_tmp102));_tmp102[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp103;_tmp103.tag=
3;_tmp103.f1=_tmp101;_tmp103;});_tmp102;});}_LLB8:;_LLB9:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104[0]=({
struct Cyc_Core_Impossible_struct _tmp105;_tmp105.tag=Cyc_Core_Impossible;_tmp105.f1=
_tag_arr("after_pathinfo -- non-aggregates!",sizeof(char),34);_tmp105;});_tmp104;}));
_LLB5:;}}static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*
env,struct _tagged_arr*key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**
_tmp106=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged _tmp107=env->changed;*
env->last_field_cell=({struct Cyc_List_List*_tmp108=_cycalloc(sizeof(*_tmp108));
_tmp108->hd=key;_tmp108->tl=0;_tmp108;});env->last_field_cell=&((struct Cyc_List_List*)
_check_null(*env->last_field_cell))->tl;{void*_tmp109=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp106;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp107;return _tmp109;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*
env->curr_place=({struct Cyc_CfFlowInfo_Place _tmp10A;_tmp10A.root=(void*)root;
_tmp10A.fields=0;_tmp10A;});env->last_field_cell=&(env->curr_place)->fields;env->changed=
Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Set_Set**all_changed,void*f1,void*f2,struct Cyc_Set_Set*chg1,struct Cyc_Set_Set*
chg2){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(void*)((void*)0)};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((void*)& dummy_rawexp),0,(void*)((
void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={1,& dummy_exp,(void*)((void*)0)};if(f1 == f2)return f1;{struct _tuple0
_tmp10C=({struct _tuple0 _tmp10B;_tmp10B.f1=f1;_tmp10B.f2=f2;_tmp10B;});void*
_tmp10D;void*_tmp10E;void*_tmp10F;struct Cyc_Dict_Dict*_tmp110;struct Cyc_List_List*
_tmp111;void*_tmp112;struct Cyc_Dict_Dict*_tmp113;struct Cyc_List_List*_tmp114;
_LLBB: _tmp10D=_tmp10C.f1;if((int)_tmp10D != 0)goto _LLBD;_LLBC: goto _LLBE;_LLBD:
_tmp10E=_tmp10C.f2;if((int)_tmp10E != 0)goto _LLBF;_LLBE: return(void*)0;_LLBF:
_tmp10F=_tmp10C.f1;if(_tmp10F <= (void*)1?1:*((int*)_tmp10F)!= 0)goto _LLBA;
_tmp110=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp10F)->f1;_tmp111=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp10F)->f2;_tmp112=_tmp10C.f2;if(_tmp112 <= (
void*)1?1:*((int*)_tmp112)!= 0)goto _LLBA;_tmp113=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp112)->f1;_tmp114=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp112)->f2;
_LLC0: if(_tmp110 == _tmp113?_tmp111 == _tmp114: 0)return f1;{struct _RegionHandle
_tmp115=_new_region("rgn");struct _RegionHandle*rgn=& _tmp115;_push_region(rgn);{
struct Cyc_CfFlowInfo_Place*_tmp116=({struct Cyc_CfFlowInfo_Place*_tmp120=
_cycalloc(sizeof(*_tmp120));_tmp120->root=(void*)((void*)& dummy_root);_tmp120->fields=
0;_tmp120;});struct Cyc_CfFlowInfo_AfterEnv _tmp117=({struct Cyc_CfFlowInfo_AfterEnv
_tmp11D;_tmp11D.joinenv=({struct Cyc_CfFlowInfo_JoinEnv _tmp11E;_tmp11E.pile=({
struct Cyc_CfFlowInfo_EscPile*_tmp11F=_region_malloc(rgn,sizeof(*_tmp11F));
_tmp11F->rgn=rgn;_tmp11F->places=0;_tmp11F;});_tmp11E.d1=_tmp110;_tmp11E.d2=
_tmp113;_tmp11E;});_tmp11D.chg1=chg1;_tmp11D.chg2=chg2;_tmp11D.curr_place=
_tmp116;_tmp11D.last_field_cell=& _tmp116->fields;_tmp11D.changed=Cyc_CfFlowInfo_Neither;
_tmp11D;});struct Cyc_Dict_Dict*_tmp118=((struct Cyc_Dict_Dict*(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp117,_tmp110,_tmp113);struct Cyc_List_List*_tmp119=Cyc_CfFlowInfo_join_relns(
_tmp111,_tmp114);void*_tmp11C=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp11B;_tmp11B.tag=0;_tmp11B.f1=Cyc_CfFlowInfo_escape_these((_tmp117.joinenv).pile,
all_changed,_tmp118);_tmp11B.f2=_tmp119;_tmp11B;});_tmp11A;});_npop_handler(0);
return _tmp11C;};_pop_region(rgn);}_LLBA:;}}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp124=({struct
_tuple0 _tmp123;_tmp123.f1=r1;_tmp123.f2=r2;_tmp123;});void*_tmp125;struct Cyc_CfFlowInfo_Place*
_tmp126;void*_tmp127;struct Cyc_CfFlowInfo_Place*_tmp128;void*_tmp129;void*
_tmp12A;void*_tmp12B;struct Cyc_Dict_Dict*_tmp12C;void*_tmp12D;struct Cyc_Dict_Dict*
_tmp12E;void*_tmp12F;void*_tmp130;void*_tmp131;void*_tmp132;void*_tmp133;void*
_tmp134;_LLC2: _tmp125=_tmp124.f1;if(_tmp125 <= (void*)3?1:*((int*)_tmp125)!= 2)
goto _LLC4;_tmp126=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp125)->f1;_tmp127=
_tmp124.f2;if(_tmp127 <= (void*)3?1:*((int*)_tmp127)!= 2)goto _LLC4;_tmp128=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp127)->f1;_LLC3: return Cyc_CfFlowInfo_place_cmp(
_tmp126,_tmp128)== 0;_LLC4: _tmp129=_tmp124.f1;if(_tmp129 <= (void*)3?1:*((int*)
_tmp129)!= 2)goto _LLC6;_LLC5: goto _LLC7;_LLC6: _tmp12A=_tmp124.f2;if(_tmp12A <= (
void*)3?1:*((int*)_tmp12A)!= 2)goto _LLC8;_LLC7: return 0;_LLC8: _tmp12B=_tmp124.f1;
if(_tmp12B <= (void*)3?1:*((int*)_tmp12B)!= 3)goto _LLCA;_tmp12C=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp12B)->f1;_tmp12D=_tmp124.f2;if(_tmp12D <= (void*)3?1:*((int*)_tmp12D)!= 3)
goto _LLCA;_tmp12E=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp12D)->f1;_LLC9:
return _tmp12C == _tmp12E?1:((int(*)(int(*f)(struct _tagged_arr*,void*,void*),
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_forall_intersect)((int(*)(
struct _tagged_arr*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp12C,_tmp12E);_LLCA: _tmp12F=_tmp124.f2;if((int)_tmp12F != 2)goto _LLCC;_LLCB:
return r1 == (void*)1;_LLCC: _tmp130=_tmp124.f2;if((int)_tmp130 != 0)goto _LLCE;_LLCD:
goto _LLCF;_LLCE: _tmp131=_tmp124.f2;if((int)_tmp131 != 1)goto _LLD0;_LLCF: return 0;
_LLD0: _tmp132=_tmp124.f1;if(_tmp132 <= (void*)3?1:*((int*)_tmp132)!= 1)goto _LLD2;
_tmp133=_tmp124.f2;if(_tmp133 <= (void*)3?1:*((int*)_tmp133)!= 1)goto _LLD2;_LLD1:
goto _LLC1;_LLD2: _tmp134=_tmp124.f1;if(_tmp134 <= (void*)3?1:*((int*)_tmp134)!= 1)
goto _LLD4;_LLD3: return 0;_LLD4:;_LLD5: goto _LLC1;_LLC1:;}{struct _tuple0 _tmp136=({
struct _tuple0 _tmp135;_tmp135.f1=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp135.f2=
Cyc_CfFlowInfo_initlevel_approx(r2);_tmp135;});void*_tmp137;void*_tmp138;void*
_tmp139;void*_tmp13A;void*_tmp13B;void*_tmp13C;_LLD7: _tmp137=_tmp136.f1;if((int)
_tmp137 != 2)goto _LLD9;_tmp138=_tmp136.f2;if((int)_tmp138 != 2)goto _LLD9;_LLD8:
return 1;_LLD9: _tmp139=_tmp136.f2;if((int)_tmp139 != 0)goto _LLDB;_LLDA: return 1;
_LLDB: _tmp13A=_tmp136.f1;if((int)_tmp13A != 0)goto _LLDD;_LLDC: return 0;_LLDD:
_tmp13B=_tmp136.f2;if((int)_tmp13B != 1)goto _LLDF;_LLDE: return 1;_LLDF: _tmp13C=
_tmp136.f1;if((int)_tmp13C != 1)goto _LLD6;_LLE0: return 0;_LLD6:;}}int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s
!= 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp13D=(struct Cyc_CfFlowInfo_Reln*)
r1s->hd;int found=0;{struct Cyc_List_List*_tmp13E=r2s;for(0;_tmp13E != 0;_tmp13E=
_tmp13E->tl){struct Cyc_CfFlowInfo_Reln*_tmp13F=(struct Cyc_CfFlowInfo_Reln*)
_tmp13E->hd;if(_tmp13D == _tmp13F?1:(_tmp13D->vd == _tmp13F->vd?Cyc_CfFlowInfo_same_relop((
void*)_tmp13D->rop,(void*)_tmp13F->rop): 0)){found=1;break;}}}if(!found)return 0;}
return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2){if(f1 == f2)
return 1;{struct _tuple0 _tmp141=({struct _tuple0 _tmp140;_tmp140.f1=f1;_tmp140.f2=f2;
_tmp140;});void*_tmp142;void*_tmp143;void*_tmp144;struct Cyc_Dict_Dict*_tmp145;
struct Cyc_List_List*_tmp146;void*_tmp147;struct Cyc_Dict_Dict*_tmp148;struct Cyc_List_List*
_tmp149;_LLE2: _tmp142=_tmp141.f1;if((int)_tmp142 != 0)goto _LLE4;_LLE3: return 1;
_LLE4: _tmp143=_tmp141.f2;if((int)_tmp143 != 0)goto _LLE6;_LLE5: return 0;_LLE6:
_tmp144=_tmp141.f1;if(_tmp144 <= (void*)1?1:*((int*)_tmp144)!= 0)goto _LLE1;
_tmp145=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp144)->f1;_tmp146=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp144)->f2;_tmp147=_tmp141.f2;if(_tmp147 <= (
void*)1?1:*((int*)_tmp147)!= 0)goto _LLE1;_tmp148=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp147)->f1;_tmp149=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp147)->f2;
_LLE7: if(_tmp145 == _tmp148?_tmp146 == _tmp149: 0)return 1;return Cyc_Dict_forall_intersect(
Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp145,_tmp148)?Cyc_CfFlowInfo_relns_approx(
_tmp146,_tmp149): 0;_LLE1:;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=
0;for(p=rs;!found?p != 0: 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp14A=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp14A->vd == v){found=1;break;}{void*_tmp14B=(void*)_tmp14A->rop;
struct Cyc_Absyn_Vardecl*_tmp14C;struct Cyc_Absyn_Vardecl*_tmp14D;struct Cyc_Absyn_Vardecl*
_tmp14E;_LLE9: if(*((int*)_tmp14B)!= 1)goto _LLEB;_tmp14C=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp14B)->f1;_LLEA: _tmp14D=_tmp14C;goto _LLEC;_LLEB: if(*((int*)_tmp14B)!= 2)goto
_LLED;_tmp14D=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp14B)->f1;_LLEC: _tmp14E=
_tmp14D;goto _LLEE;_LLED: if(*((int*)_tmp14B)!= 4)goto _LLEF;_tmp14E=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp14B)->f1;_LLEE: if(v == _tmp14E)found=1;goto _LLE8;_LLEF:;_LLF0: goto _LLE8;_LLE8:;}}
if(!found)return rs;{struct Cyc_List_List*_tmp14F=0;for(p=rs;p != 0;p=p->tl){struct
Cyc_CfFlowInfo_Reln*_tmp150=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp150->vd != v){{
void*_tmp151=(void*)_tmp150->rop;struct Cyc_Absyn_Vardecl*_tmp152;struct Cyc_Absyn_Vardecl*
_tmp153;struct Cyc_Absyn_Vardecl*_tmp154;_LLF2: if(*((int*)_tmp151)!= 1)goto _LLF4;
_tmp152=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp151)->f1;_LLF3: _tmp153=
_tmp152;goto _LLF5;_LLF4: if(*((int*)_tmp151)!= 2)goto _LLF6;_tmp153=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp151)->f1;_LLF5: _tmp154=_tmp153;goto _LLF7;_LLF6: if(*((int*)_tmp151)!= 4)goto
_LLF8;_tmp154=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp151)->f1;_LLF7: if(v
== _tmp154)continue;goto _LLF1;_LLF8:;_LLF9: goto _LLF1;_LLF1:;}_tmp14F=({struct Cyc_List_List*
_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->hd=_tmp150;_tmp155->tl=_tmp14F;
_tmp155;});}}return _tmp14F;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp156=(void*)e->r;void*
_tmp157;struct Cyc_Absyn_Vardecl*_tmp158;void*_tmp159;struct Cyc_Absyn_Vardecl*
_tmp15A;void*_tmp15B;struct Cyc_Absyn_Vardecl*_tmp15C;void*_tmp15D;struct Cyc_Absyn_Vardecl*
_tmp15E;_LLFB: if(*((int*)_tmp156)!= 1)goto _LLFD;_tmp157=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp156)->f2;if(_tmp157 <= (void*)1?1:*((int*)_tmp157)!= 0)goto _LLFD;_tmp158=((
struct Cyc_Absyn_Global_b_struct*)_tmp157)->f1;_LLFC: _tmp15A=_tmp158;goto _LLFE;
_LLFD: if(*((int*)_tmp156)!= 1)goto _LLFF;_tmp159=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp156)->f2;if(_tmp159 <= (void*)1?1:*((int*)_tmp159)!= 2)goto _LLFF;_tmp15A=((
struct Cyc_Absyn_Param_b_struct*)_tmp159)->f1;_LLFE: _tmp15C=_tmp15A;goto _LL100;
_LLFF: if(*((int*)_tmp156)!= 1)goto _LL101;_tmp15B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp156)->f2;if(_tmp15B <= (void*)1?1:*((int*)_tmp15B)!= 3)goto _LL101;_tmp15C=((
struct Cyc_Absyn_Local_b_struct*)_tmp15B)->f1;_LL100: _tmp15E=_tmp15C;goto _LL102;
_LL101: if(*((int*)_tmp156)!= 1)goto _LL103;_tmp15D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp156)->f2;if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 4)goto _LL103;_tmp15E=((
struct Cyc_Absyn_Pat_b_struct*)_tmp15D)->f1;_LL102: if(!_tmp15E->escapes)return Cyc_CfFlowInfo_reln_kill_var(
r,_tmp15E);goto _LLFA;_LL103:;_LL104: goto _LLFA;_LLFA:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(r,v);{
void*_tmp15F=(void*)e->r;struct Cyc_Absyn_MallocInfo _tmp160;struct Cyc_Absyn_Exp*
_tmp161;int _tmp162;_LL106: if(*((int*)_tmp15F)!= 33)goto _LL108;_tmp160=((struct
Cyc_Absyn_Malloc_e_struct*)_tmp15F)->f1;_tmp161=_tmp160.num_elts;_tmp162=_tmp160.fat_result;
if(_tmp162 != 1)goto _LL108;_LL107: malloc_loop: {void*_tmp163=(void*)_tmp161->r;
struct Cyc_Absyn_Exp*_tmp164;void*_tmp165;struct Cyc_Absyn_Vardecl*_tmp166;void*
_tmp167;struct Cyc_Absyn_Vardecl*_tmp168;void*_tmp169;struct Cyc_Absyn_Vardecl*
_tmp16A;void*_tmp16B;struct Cyc_Absyn_Vardecl*_tmp16C;_LL10B: if(*((int*)_tmp163)
!= 13)goto _LL10D;_tmp164=((struct Cyc_Absyn_Cast_e_struct*)_tmp163)->f2;_LL10C:
_tmp161=_tmp164;goto malloc_loop;_LL10D: if(*((int*)_tmp163)!= 1)goto _LL10F;
_tmp165=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp163)->f2;if(_tmp165 <= (void*)
1?1:*((int*)_tmp165)!= 4)goto _LL10F;_tmp166=((struct Cyc_Absyn_Pat_b_struct*)
_tmp165)->f1;_LL10E: _tmp168=_tmp166;goto _LL110;_LL10F: if(*((int*)_tmp163)!= 1)
goto _LL111;_tmp167=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp163)->f2;if(
_tmp167 <= (void*)1?1:*((int*)_tmp167)!= 3)goto _LL111;_tmp168=((struct Cyc_Absyn_Local_b_struct*)
_tmp167)->f1;_LL110: _tmp16A=_tmp168;goto _LL112;_LL111: if(*((int*)_tmp163)!= 1)
goto _LL113;_tmp169=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp163)->f2;if(
_tmp169 <= (void*)1?1:*((int*)_tmp169)!= 2)goto _LL113;_tmp16A=((struct Cyc_Absyn_Param_b_struct*)
_tmp169)->f1;_LL112: _tmp16C=_tmp16A;goto _LL114;_LL113: if(*((int*)_tmp163)!= 1)
goto _LL115;_tmp16B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp163)->f2;if(
_tmp16B <= (void*)1?1:*((int*)_tmp16B)!= 0)goto _LL115;_tmp16C=((struct Cyc_Absyn_Global_b_struct*)
_tmp16B)->f1;_LL114: if(_tmp16C->escapes)return r;return({struct Cyc_List_List*
_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->vd=_tmp16C;_tmp16E->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));
_tmp16F[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp170;_tmp170.tag=4;_tmp170.f1=
v;_tmp170;});_tmp16F;}));_tmp16E;});_tmp16D->tl=r;_tmp16D;});_LL115:;_LL116:
return r;_LL10A:;}_LL108:;_LL109: goto _LL105;_LL105:;}{void*_tmp171=Cyc_Tcutil_compress((
void*)v->type);_LL118: if(_tmp171 <= (void*)3?1:*((int*)_tmp171)!= 5)goto _LL11A;
_LL119: goto _LL117;_LL11A:;_LL11B: return r;_LL117:;}loop: {void*_tmp172=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp173;void*_tmp174;int _tmp175;void*_tmp176;struct Cyc_List_List*
_tmp177;struct Cyc_List_List _tmp178;struct Cyc_List_List*_tmp179;struct Cyc_List_List
_tmp17A;struct Cyc_Absyn_Exp*_tmp17B;void*_tmp17C;struct Cyc_List_List*_tmp17D;
struct Cyc_List_List _tmp17E;struct Cyc_Absyn_Exp*_tmp17F;_LL11D: if(*((int*)_tmp172)
!= 13)goto _LL11F;_tmp173=((struct Cyc_Absyn_Cast_e_struct*)_tmp172)->f2;_LL11E: e=
_tmp173;goto loop;_LL11F: if(*((int*)_tmp172)!= 0)goto _LL121;_tmp174=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp172)->f1;if(_tmp174 <= (void*)1?1:*((int*)
_tmp174)!= 2)goto _LL121;_tmp175=((struct Cyc_Absyn_Int_c_struct*)_tmp174)->f2;
_LL120: return({struct Cyc_List_List*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->vd=v;
_tmp181->rop=(void*)((void*)({struct Cyc_CfFlowInfo_EqualConst_struct*_tmp182=
_cycalloc_atomic(sizeof(*_tmp182));_tmp182[0]=({struct Cyc_CfFlowInfo_EqualConst_struct
_tmp183;_tmp183.tag=0;_tmp183.f1=(unsigned int)_tmp175;_tmp183;});_tmp182;}));
_tmp181;});_tmp180->tl=r;_tmp180;});_LL121: if(*((int*)_tmp172)!= 3)goto _LL123;
_tmp176=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp172)->f1;if((int)_tmp176 != 
4)goto _LL123;_tmp177=((struct Cyc_Absyn_Primop_e_struct*)_tmp172)->f2;if(_tmp177
== 0)goto _LL123;_tmp178=*_tmp177;_tmp179=_tmp178.tl;if(_tmp179 == 0)goto _LL123;
_tmp17A=*_tmp179;_tmp17B=(struct Cyc_Absyn_Exp*)_tmp17A.hd;_LL122:{void*_tmp184=(
void*)_tmp17B->r;void*_tmp185;struct Cyc_List_List*_tmp186;struct Cyc_List_List
_tmp187;struct Cyc_Absyn_Exp*_tmp188;_LL128: if(*((int*)_tmp184)!= 3)goto _LL12A;
_tmp185=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp184)->f1;if((int)_tmp185 != 
19)goto _LL12A;_tmp186=((struct Cyc_Absyn_Primop_e_struct*)_tmp184)->f2;if(_tmp186
== 0)goto _LL12A;_tmp187=*_tmp186;_tmp188=(struct Cyc_Absyn_Exp*)_tmp187.hd;_LL129:{
void*_tmp189=(void*)_tmp188->r;void*_tmp18A;struct Cyc_Absyn_Vardecl*_tmp18B;void*
_tmp18C;struct Cyc_Absyn_Vardecl*_tmp18D;void*_tmp18E;struct Cyc_Absyn_Vardecl*
_tmp18F;void*_tmp190;struct Cyc_Absyn_Vardecl*_tmp191;_LL12D: if(*((int*)_tmp189)
!= 1)goto _LL12F;_tmp18A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp189)->f2;if(
_tmp18A <= (void*)1?1:*((int*)_tmp18A)!= 0)goto _LL12F;_tmp18B=((struct Cyc_Absyn_Global_b_struct*)
_tmp18A)->f1;_LL12E: _tmp18D=_tmp18B;goto _LL130;_LL12F: if(*((int*)_tmp189)!= 1)
goto _LL131;_tmp18C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp189)->f2;if(
_tmp18C <= (void*)1?1:*((int*)_tmp18C)!= 3)goto _LL131;_tmp18D=((struct Cyc_Absyn_Local_b_struct*)
_tmp18C)->f1;_LL130: _tmp18F=_tmp18D;goto _LL132;_LL131: if(*((int*)_tmp189)!= 1)
goto _LL133;_tmp18E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp189)->f2;if(
_tmp18E <= (void*)1?1:*((int*)_tmp18E)!= 2)goto _LL133;_tmp18F=((struct Cyc_Absyn_Param_b_struct*)
_tmp18E)->f1;_LL132: _tmp191=_tmp18F;goto _LL134;_LL133: if(*((int*)_tmp189)!= 1)
goto _LL135;_tmp190=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp189)->f2;if(
_tmp190 <= (void*)1?1:*((int*)_tmp190)!= 4)goto _LL135;_tmp191=((struct Cyc_Absyn_Pat_b_struct*)
_tmp190)->f1;_LL134: if(_tmp191->escapes)return r;return({struct Cyc_List_List*
_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->vd=v;_tmp193->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessSize_struct*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194[
0]=({struct Cyc_CfFlowInfo_LessSize_struct _tmp195;_tmp195.tag=2;_tmp195.f1=
_tmp191;_tmp195;});_tmp194;}));_tmp193;});_tmp192->tl=r;_tmp192;});_LL135:;
_LL136: goto _LL12C;_LL12C:;}goto _LL127;_LL12A:;_LL12B: goto _LL127;_LL127:;}goto
_LL11C;_LL123: if(*((int*)_tmp172)!= 3)goto _LL125;_tmp17C=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp172)->f1;if((int)_tmp17C != 19)goto _LL125;_tmp17D=((struct Cyc_Absyn_Primop_e_struct*)
_tmp172)->f2;if(_tmp17D == 0)goto _LL125;_tmp17E=*_tmp17D;_tmp17F=(struct Cyc_Absyn_Exp*)
_tmp17E.hd;_LL124:{void*_tmp196=(void*)_tmp17F->r;void*_tmp197;struct Cyc_Absyn_Vardecl*
_tmp198;void*_tmp199;struct Cyc_Absyn_Vardecl*_tmp19A;void*_tmp19B;struct Cyc_Absyn_Vardecl*
_tmp19C;void*_tmp19D;struct Cyc_Absyn_Vardecl*_tmp19E;_LL138: if(*((int*)_tmp196)
!= 1)goto _LL13A;_tmp197=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp196)->f2;if(
_tmp197 <= (void*)1?1:*((int*)_tmp197)!= 0)goto _LL13A;_tmp198=((struct Cyc_Absyn_Global_b_struct*)
_tmp197)->f1;_LL139: _tmp19A=_tmp198;goto _LL13B;_LL13A: if(*((int*)_tmp196)!= 1)
goto _LL13C;_tmp199=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp196)->f2;if(
_tmp199 <= (void*)1?1:*((int*)_tmp199)!= 3)goto _LL13C;_tmp19A=((struct Cyc_Absyn_Local_b_struct*)
_tmp199)->f1;_LL13B: _tmp19C=_tmp19A;goto _LL13D;_LL13C: if(*((int*)_tmp196)!= 1)
goto _LL13E;_tmp19B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp196)->f2;if(
_tmp19B <= (void*)1?1:*((int*)_tmp19B)!= 2)goto _LL13E;_tmp19C=((struct Cyc_Absyn_Param_b_struct*)
_tmp19B)->f1;_LL13D: _tmp19E=_tmp19C;goto _LL13F;_LL13E: if(*((int*)_tmp196)!= 1)
goto _LL140;_tmp19D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp196)->f2;if(
_tmp19D <= (void*)1?1:*((int*)_tmp19D)!= 4)goto _LL140;_tmp19E=((struct Cyc_Absyn_Pat_b_struct*)
_tmp19D)->f1;_LL13F: if(_tmp19E->escapes)return r;return({struct Cyc_List_List*
_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->vd=v;_tmp1A0->rop=(void*)((void*)({
struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));
_tmp1A1[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct _tmp1A2;_tmp1A2.tag=4;_tmp1A2.f1=
_tmp19E;_tmp1A2;});_tmp1A1;}));_tmp1A0;});_tmp19F->tl=r;_tmp19F;});_LL140:;
_LL141: goto _LL137;_LL137:;}goto _LL11C;_LL125:;_LL126: goto _LL11C;_LL11C:;}return r;}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct Cyc_List_List*r,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp1A3=(void*)e1->r;void*_tmp1A4;
struct Cyc_Absyn_Vardecl*_tmp1A5;void*_tmp1A6;struct Cyc_Absyn_Vardecl*_tmp1A7;
void*_tmp1A8;struct Cyc_Absyn_Vardecl*_tmp1A9;void*_tmp1AA;struct Cyc_Absyn_Vardecl*
_tmp1AB;_LL143: if(*((int*)_tmp1A3)!= 1)goto _LL145;_tmp1A4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A3)->f2;if(_tmp1A4 <= (void*)1?1:*((int*)_tmp1A4)!= 0)goto _LL145;_tmp1A5=((
struct Cyc_Absyn_Global_b_struct*)_tmp1A4)->f1;_LL144: _tmp1A7=_tmp1A5;goto _LL146;
_LL145: if(*((int*)_tmp1A3)!= 1)goto _LL147;_tmp1A6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A3)->f2;if(_tmp1A6 <= (void*)1?1:*((int*)_tmp1A6)!= 2)goto _LL147;_tmp1A7=((
struct Cyc_Absyn_Param_b_struct*)_tmp1A6)->f1;_LL146: _tmp1A9=_tmp1A7;goto _LL148;
_LL147: if(*((int*)_tmp1A3)!= 1)goto _LL149;_tmp1A8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A3)->f2;if(_tmp1A8 <= (void*)1?1:*((int*)_tmp1A8)!= 3)goto _LL149;_tmp1A9=((
struct Cyc_Absyn_Local_b_struct*)_tmp1A8)->f1;_LL148: _tmp1AB=_tmp1A9;goto _LL14A;
_LL149: if(*((int*)_tmp1A3)!= 1)goto _LL14B;_tmp1AA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1A3)->f2;if(_tmp1AA <= (void*)1?1:*((int*)_tmp1AA)!= 4)goto _LL14B;_tmp1AB=((
struct Cyc_Absyn_Pat_b_struct*)_tmp1AA)->f1;_LL14A: if(!_tmp1AB->escapes)return Cyc_CfFlowInfo_reln_assign_var(
r,_tmp1AB,e2);goto _LL142;_LL14B:;_LL14C: goto _LL142;_LL142:;}return r;}void Cyc_CfFlowInfo_print_reln(
struct Cyc_CfFlowInfo_Reln*r){({struct Cyc_Std_String_pa_struct _tmp1AD;_tmp1AD.tag=
0;_tmp1AD.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((r->vd)->name);{void*
_tmp1AC[1]={& _tmp1AD};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s",sizeof(char),
3),_tag_arr(_tmp1AC,sizeof(void*),1));}});{void*_tmp1AE=(void*)r->rop;
unsigned int _tmp1AF;struct Cyc_Absyn_Vardecl*_tmp1B0;struct Cyc_Absyn_Vardecl*
_tmp1B1;unsigned int _tmp1B2;struct Cyc_Absyn_Vardecl*_tmp1B3;_LL14E: if(*((int*)
_tmp1AE)!= 0)goto _LL150;_tmp1AF=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp1AE)->f1;_LL14F:({struct Cyc_Std_Int_pa_struct _tmp1B5;_tmp1B5.tag=1;_tmp1B5.f1=(
int)_tmp1AF;{void*_tmp1B4[1]={& _tmp1B5};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("==%d",
sizeof(char),5),_tag_arr(_tmp1B4,sizeof(void*),1));}});goto _LL14D;_LL150: if(*((
int*)_tmp1AE)!= 1)goto _LL152;_tmp1B0=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp1AE)->f1;_LL151:({struct Cyc_Std_String_pa_struct _tmp1B7;_tmp1B7.tag=0;
_tmp1B7.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1B0->name);{void*
_tmp1B6[1]={& _tmp1B7};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<%s",sizeof(char),
4),_tag_arr(_tmp1B6,sizeof(void*),1));}});goto _LL14D;_LL152: if(*((int*)_tmp1AE)
!= 2)goto _LL154;_tmp1B1=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1AE)->f1;
_LL153:({struct Cyc_Std_String_pa_struct _tmp1B9;_tmp1B9.tag=0;_tmp1B9.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp1B1->name);{void*_tmp1B8[1]={& _tmp1B9};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<%s.size",sizeof(char),9),_tag_arr(
_tmp1B8,sizeof(void*),1));}});goto _LL14D;_LL154: if(*((int*)_tmp1AE)!= 3)goto
_LL156;_tmp1B2=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp1AE)->f1;_LL155:({
struct Cyc_Std_Int_pa_struct _tmp1BB;_tmp1BB.tag=1;_tmp1BB.f1=(int)_tmp1B2;{void*
_tmp1BA[1]={& _tmp1BB};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<%d",sizeof(char),
4),_tag_arr(_tmp1BA,sizeof(void*),1));}});goto _LL14D;_LL156: if(*((int*)_tmp1AE)
!= 4)goto _LL14D;_tmp1B3=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp1AE)->f1;
_LL157:({struct Cyc_Std_String_pa_struct _tmp1BD;_tmp1BD.tag=0;_tmp1BD.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp1B3->name);{void*_tmp1BC[1]={& _tmp1BD};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<=%s.size",sizeof(char),10),_tag_arr(
_tmp1BC,sizeof(void*),1));}});goto _LL14D;_LL14D:;}}void Cyc_CfFlowInfo_print_relns(
struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)
r->hd);if(r->tl != 0)({void*_tmp1BE[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr(",",
sizeof(char),2),_tag_arr(_tmp1BE,sizeof(void*),0));});}}static int Cyc_CfFlowInfo_contains_region(
struct Cyc_Absyn_Tvar*rgn,void*t){void*_tmp1BF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp1C0;struct Cyc_Absyn_TunionInfo _tmp1C1;struct Cyc_List_List*_tmp1C2;void*
_tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_Absyn_AggrInfo _tmp1C5;struct Cyc_List_List*
_tmp1C6;struct Cyc_Absyn_TunionFieldInfo _tmp1C7;struct Cyc_List_List*_tmp1C8;
struct Cyc_List_List*_tmp1C9;struct Cyc_Absyn_PtrInfo _tmp1CA;void*_tmp1CB;void*
_tmp1CC;struct Cyc_List_List*_tmp1CD;struct Cyc_List_List*_tmp1CE;void*_tmp1CF;
void*_tmp1D0;void*_tmp1D1;void*_tmp1D2;_LL159: if((int)_tmp1BF != 0)goto _LL15B;
_LL15A: goto _LL15C;_LL15B: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 5)goto _LL15D;
_LL15C: goto _LL15E;_LL15D: if((int)_tmp1BF != 1)goto _LL15F;_LL15E: goto _LL160;_LL15F:
if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 6)goto _LL161;_LL160: goto _LL162;_LL161:
if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 12)goto _LL163;_LL162: goto _LL164;
_LL163: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 13)goto _LL165;_LL164: goto
_LL166;_LL165: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 14)goto _LL167;_LL166:
goto _LL168;_LL167: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 17)goto _LL169;
_LL168: goto _LL16A;_LL169: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 18)goto
_LL16B;_LL16A: goto _LL16C;_LL16B: if((int)_tmp1BF != 2)goto _LL16D;_LL16C: goto _LL16E;
_LL16D: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 0)goto _LL16F;_LL16E: return 0;
_LL16F: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 1)goto _LL171;_tmp1C0=((struct
Cyc_Absyn_VarType_struct*)_tmp1BF)->f1;_LL170: return Cyc_Absyn_tvar_cmp(_tmp1C0,
rgn)== 0;_LL171: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 2)goto _LL173;_tmp1C1=((
struct Cyc_Absyn_TunionType_struct*)_tmp1BF)->f1;_tmp1C2=_tmp1C1.targs;_tmp1C3=(
void*)_tmp1C1.rgn;_LL172: if(Cyc_CfFlowInfo_contains_region(rgn,_tmp1C3))return 1;
_tmp1C4=_tmp1C2;goto _LL174;_LL173: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 16)
goto _LL175;_tmp1C4=((struct Cyc_Absyn_TypedefType_struct*)_tmp1BF)->f2;_LL174:
_tmp1C6=_tmp1C4;goto _LL176;_LL175: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 10)
goto _LL177;_tmp1C5=((struct Cyc_Absyn_AggrType_struct*)_tmp1BF)->f1;_tmp1C6=
_tmp1C5.targs;_LL176: _tmp1C8=_tmp1C6;goto _LL178;_LL177: if(_tmp1BF <= (void*)3?1:*((
int*)_tmp1BF)!= 3)goto _LL179;_tmp1C7=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1BF)->f1;_tmp1C8=_tmp1C7.targs;_LL178: _tmp1C9=_tmp1C8;goto _LL17A;_LL179: if(
_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 20)goto _LL17B;_tmp1C9=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1BF)->f1;_LL17A: return((int(*)(int(*pred)(struct Cyc_Absyn_Tvar*,void*),
struct Cyc_Absyn_Tvar*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,
rgn,_tmp1C9);_LL17B: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 4)goto _LL17D;
_tmp1CA=((struct Cyc_Absyn_PointerType_struct*)_tmp1BF)->f1;_tmp1CB=(void*)
_tmp1CA.elt_typ;_tmp1CC=(void*)_tmp1CA.rgn_typ;_LL17C: return Cyc_CfFlowInfo_contains_region(
rgn,_tmp1CC)?1: Cyc_CfFlowInfo_contains_region(rgn,_tmp1CB);_LL17D: if(_tmp1BF <= (
void*)3?1:*((int*)_tmp1BF)!= 8)goto _LL17F;_LL17E: return 0;_LL17F: if(_tmp1BF <= (
void*)3?1:*((int*)_tmp1BF)!= 9)goto _LL181;_tmp1CD=((struct Cyc_Absyn_TupleType_struct*)
_tmp1BF)->f1;_LL180: for(0;_tmp1CD != 0;_tmp1CD=_tmp1CD->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(*((struct _tuple4*)_tmp1CD->hd)).f2))return 1;}return 0;_LL181: if(_tmp1BF <= (
void*)3?1:*((int*)_tmp1BF)!= 11)goto _LL183;_tmp1CE=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1BF)->f2;_LL182: for(0;_tmp1CE != 0;_tmp1CE=_tmp1CE->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp1CE->hd)->type))return 1;}return 0;
_LL183: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 7)goto _LL185;_tmp1CF=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp1BF)->f1;_LL184: _tmp1D0=_tmp1CF;goto _LL186;
_LL185: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 19)goto _LL187;_tmp1D0=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp1BF)->f1;_LL186: _tmp1D1=_tmp1D0;goto _LL188;
_LL187: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 21)goto _LL189;_tmp1D1=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1BF)->f1;_LL188: return Cyc_CfFlowInfo_contains_region(
rgn,_tmp1D1);_LL189: if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 15)goto _LL158;
_tmp1D2=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp1BF)->f1;_LL18A:
return 0;_LL158:;}struct _tuple6{struct Cyc_Dict_Dict*f1;struct Cyc_Absyn_Tvar*f2;};
static void Cyc_CfFlowInfo_kill_root(struct _tuple6*env,void*root,void*rval){struct
_tuple6 _tmp1D4;struct Cyc_Dict_Dict*_tmp1D5;struct Cyc_Dict_Dict**_tmp1D6;struct
Cyc_Absyn_Tvar*_tmp1D7;struct _tuple6*_tmp1D3=env;_tmp1D4=*_tmp1D3;_tmp1D5=
_tmp1D4.f1;_tmp1D6=(struct Cyc_Dict_Dict**)&(*_tmp1D3).f1;_tmp1D7=_tmp1D4.f2;{
void*_tmp1D8=root;struct Cyc_Absyn_Vardecl*_tmp1D9;void*_tmp1DA;_LL18C: if(*((int*)
_tmp1D8)!= 0)goto _LL18E;_tmp1D9=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp1D8)->f1;
_LL18D: if(Cyc_CfFlowInfo_contains_region(_tmp1D7,(void*)_tmp1D9->type))rval=Cyc_CfFlowInfo_typ_to_absrval((
void*)_tmp1D9->type,Cyc_CfFlowInfo_unknown_none);*_tmp1D6=Cyc_Dict_insert(*
_tmp1D6,root,rval);goto _LL18B;_LL18E: if(*((int*)_tmp1D8)!= 1)goto _LL190;_tmp1DA=(
void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp1D8)->f2;_LL18F: if(!Cyc_CfFlowInfo_contains_region(
_tmp1D7,_tmp1DA))*_tmp1D6=Cyc_Dict_insert(*_tmp1D6,root,rval);goto _LL18B;_LL190:
if(*((int*)_tmp1D8)!= 2)goto _LL18B;_LL191: goto _LL18B;_LL18B:;}}static struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_Dict_Dict*fd,void*rgn){struct Cyc_Absyn_Tvar*
rgn_tvar;{void*_tmp1DB=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp1DC;
_LL193: if(_tmp1DB <= (void*)3?1:*((int*)_tmp1DB)!= 1)goto _LL195;_tmp1DC=((struct
Cyc_Absyn_VarType_struct*)_tmp1DB)->f1;_LL194: rgn_tvar=_tmp1DC;goto _LL192;_LL195:;
_LL196:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1DD=_cycalloc(
sizeof(*_tmp1DD));_tmp1DD[0]=({struct Cyc_Core_Impossible_struct _tmp1DE;_tmp1DE.tag=
Cyc_Core_Impossible;_tmp1DE.f1=_tag_arr("kill_flowdict_region",sizeof(char),21);
_tmp1DE;});_tmp1DD;}));goto _LL192;_LL192:;}{struct _tuple6 env=({struct _tuple6
_tmp1DF;_tmp1DF.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp1DF.f2=rgn_tvar;
_tmp1DF;});((void(*)(void(*f)(struct _tuple6*,void*,void*),struct _tuple6*env,
struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f1;}}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*
relns,void*rgn){return 0;}void*Cyc_CfFlowInfo_kill_flow_region(void*f,void*rgn){
void*_tmp1E0=f;struct Cyc_Dict_Dict*_tmp1E1;struct Cyc_List_List*_tmp1E2;_LL198:
if((int)_tmp1E0 != 0)goto _LL19A;_LL199: return f;_LL19A: if(_tmp1E0 <= (void*)1?1:*((
int*)_tmp1E0)!= 0)goto _LL197;_tmp1E1=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1E0)->f1;_tmp1E2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1E0)->f2;
_LL19B: {struct Cyc_Dict_Dict*_tmp1E3=Cyc_CfFlowInfo_kill_flowdict_region(_tmp1E1,
rgn);struct Cyc_List_List*_tmp1E4=Cyc_CfFlowInfo_kill_relns_region(_tmp1E2,rgn);
return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp1E5=_cycalloc(sizeof(*
_tmp1E5));_tmp1E5[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1E6;_tmp1E6.tag=
0;_tmp1E6.f1=_tmp1E3;_tmp1E6.f2=_tmp1E4;_tmp1E6;});_tmp1E5;});}_LL197:;}
