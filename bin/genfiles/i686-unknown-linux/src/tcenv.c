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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};typedef
struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc_Std___mbstate_t;
typedef struct{int __pos;Cyc_Std___mbstate_t __state;}Cyc_Std__G_fpos_t;typedef Cyc_Std__G_fpos_t
Cyc_Std_fpos_t;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;
struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,
struct _tagged_arr);int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);int Cyc_Std_fprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));int Cyc_Dict_member(
struct Cyc_Dict_Dict*d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);struct _tuple0{void*f1;void*f2;
};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_SlowDict_Dict;
extern char Cyc_SlowDict_Present[12];extern char Cyc_SlowDict_Absent[11];struct Cyc_SlowDict_Dict*
Cyc_SlowDict_empty(int(*cmp)(void*,void*));int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*
d);struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*k,
void*v);struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,
void*k);struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,
void*k);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;
int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple1{void*
f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};
struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_exn_name;extern
struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_tagged_typ(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq);struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct Cyc_RgnOrder_RgnPO;
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_env_err(struct _tagged_arr msg);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv();
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tagged_arr*,struct Cyc_List_List*);void*Cyc_Tcenv_lookup_ordinary(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_xtuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
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
struct _tagged_arr*,struct Cyc_Absyn_Stmt*);int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs);void*Cyc_Tcenv_curr_rgn(struct
Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*
te,void*r,int resetable);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*
Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void
Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,struct Cyc_List_List*);
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int*Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);char Cyc_Tcenv_Env_error[14]="\000\000\000\000Env_error";
void*Cyc_Tcenv_env_err(struct _tagged_arr msg){({struct Cyc_Std_String_pa_struct
_tmp1;_tmp1.tag=0;_tmp1.f1=(struct _tagged_arr)msg;{void*_tmp0[1]={& _tmp1};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: %s\n",sizeof(char),11),_tag_arr(_tmp0,sizeof(
void*),1));}});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);(int)
_throw((void*)Cyc_Tcenv_Env_error);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
struct Cyc_Tcenv_CtrlEnv{void*continue_stmt;void*break_stmt;struct _tuple4*
fallthru_clause;void*next_stmt;int try_depth;};struct Cyc_Tcenv_SharedFenv{void*
return_typ;struct Cyc_Dict_Dict*seen_labels;struct Cyc_SlowDict_Dict*needed_labels;
struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*
delayed_constraint_checks;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*
shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;
struct Cyc_Dict_Dict*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;};struct _tuple5{void*f1;int f2;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(){Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp2=
_cycalloc(sizeof(*_tmp2));_tmp2->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_zstrptrcmp);_tmp2;});{
struct Cyc_Tcenv_Genv*_tmp3=Cyc_Tcenv_empty_genv();struct Cyc_List_List*_tmp4=0;
_tmp3->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(_tmp3->tuniondecls,(*
Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Tuniondecl**_tmp5=_cycalloc(sizeof(*
_tmp5));_tmp5[0]=Cyc_Absyn_exn_tud;_tmp5;}));{struct Cyc_List_List*_tmp6=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){_tmp3->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple5*v))Cyc_Dict_insert)(
_tmp3->ordinaries,(*((struct Cyc_Absyn_Tunionfield*)_tmp6->hd)->name).f2,({struct
_tuple5*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*
_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=({struct Cyc_Tcenv_TunionRes_struct _tmp9;
_tmp9.tag=2;_tmp9.f1=Cyc_Absyn_exn_tud;_tmp9.f2=(struct Cyc_Absyn_Tunionfield*)
_tmp6->hd;_tmp9;});_tmp8;});_tmp7->f2=1;_tmp7;}));}}{struct Cyc_Dict_Dict*ae=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_List_List*,
struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),_tmp4,_tmp3);
return({struct Cyc_Tcenv_Tenv*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->ns=_tmp4;
_tmpA->ae=ae;_tmpA->le=0;_tmpA;});}}}static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tagged_arr*n,
struct Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,
loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*
ns){if(ns == 0)return((struct Cyc_List_List*(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv::outer_namespace",sizeof(char),23));return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2)return 1;for(0;n1 != 0;
n1=n1->tl){if(n2 == 0)return 0;if(Cyc_Std_zstrptrcmp((struct _tagged_arr*)n1->hd,(
struct _tagged_arr*)n2->hd)!= 0)return 0;n2=n2->tl;}if(n2 != 0)return 0;return 1;}
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v,
struct Cyc_List_List*cns,struct Cyc_List_List*nss){for(0;nss != 0;nss=nss->tl){if(!
Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)nss->hd);struct _handler_cons _tmpB;_push_handler(&
_tmpB);{int _tmpD=0;if(setjmp(_tmpB.handler))_tmpD=1;if(!_tmpD){lookup(ge2,v);({
struct Cyc_Std_String_pa_struct _tmpF;_tmpF.tag=0;_tmpF.f1=(struct _tagged_arr)*v;{
void*_tmpE[1]={& _tmpF};Cyc_Tcutil_terr(loc,_tag_arr("%s is ambiguous",sizeof(
char),16),_tag_arr(_tmpE,sizeof(void*),1));}});;_pop_handler();}else{void*_tmpC=(
void*)_exn_thrown;void*_tmp11=_tmpC;_LL1: if(_tmp11 != Cyc_Dict_Absent)goto _LL3;
_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(_tmp11);_LL0:;}}}}return;}static void*Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _tagged_arr*),struct _tagged_arr*v){struct Cyc_List_List*cns=te->ns;while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp12;
_push_handler(& _tmp12);{int _tmp14=0;if(setjmp(_tmp12.handler))_tmp14=1;if(!
_tmp14){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp15=result;_npop_handler(0);return _tmp15;}};_pop_handler();}else{void*
_tmp13=(void*)_exn_thrown;void*_tmp17=_tmp13;_LL6: if(_tmp17 != Cyc_Dict_Absent)
goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp17);_LL5:;}}}{struct Cyc_List_List*
_tmp18=ge->availables;for(0;_tmp18 != 0;_tmp18=_tmp18->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp18->hd);struct _handler_cons _tmp19;_push_handler(&
_tmp19);{int _tmp1B=0;if(setjmp(_tmp19.handler))_tmp1B=1;if(!_tmp1B){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp18->hd,_tmp18->tl);{void*_tmp1C=result;_npop_handler(0);return _tmp1C;}};
_pop_handler();}else{void*_tmp1A=(void*)_exn_thrown;void*_tmp1E=_tmp1A;_LLB: if(
_tmp1E != Cyc_Dict_Absent)goto _LLD;_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(_tmp1E);
_LLA:;}}}}if(cns == 0)(int)_throw((void*)Cyc_Dict_Absent);cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _tagged_arr*v){struct _tuple5*ans=((struct _tuple5*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,v);(*ans).f2=1;return(*ans).f1;}
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q){struct _tuple1 _tmp20;void*_tmp21;struct _tagged_arr*_tmp22;
struct _tuple1*_tmp1F=q;_tmp20=*_tmp1F;_tmp21=_tmp20.f1;_tmp22=_tmp20.f2;{void*
_tmp23=_tmp21;struct Cyc_List_List*_tmp24;struct Cyc_List_List*_tmp25;struct Cyc_List_List
_tmp26;struct _tagged_arr*_tmp27;struct Cyc_List_List*_tmp28;struct Cyc_List_List*
_tmp29;_LL10: if((int)_tmp23 != 0)goto _LL12;_LL11: goto _LL13;_LL12: if(_tmp23 <= (
void*)1?1:*((int*)_tmp23)!= 0)goto _LL14;_tmp24=((struct Cyc_Absyn_Rel_n_struct*)
_tmp23)->f1;if(_tmp24 != 0)goto _LL14;_LL13: return Cyc_Tcenv_scoped_lookup(te,loc,
Cyc_Tcenv_lookup_ordinary_global_f,_tmp22);_LL14: if(_tmp23 <= (void*)1?1:*((int*)
_tmp23)!= 0)goto _LL16;_tmp25=((struct Cyc_Absyn_Rel_n_struct*)_tmp23)->f1;if(
_tmp25 == 0)goto _LL16;_tmp26=*_tmp25;_tmp27=(struct _tagged_arr*)_tmp26.hd;_tmp28=
_tmp26.tl;_LL15: {struct Cyc_Tcenv_Genv*_tmp2A=Cyc_Tcenv_lookup_namespace(te,loc,
_tmp27,_tmp28);return Cyc_Tcenv_lookup_ordinary_global_f(_tmp2A,_tmp22);}_LL16:
if(_tmp23 <= (void*)1?1:*((int*)_tmp23)!= 1)goto _LLF;_tmp29=((struct Cyc_Absyn_Abs_n_struct*)
_tmp23)->f1;_LL17: return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp29),
_tmp22);_LLF:;}}struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tagged_arr*n,struct Cyc_List_List*ns){
struct Cyc_List_List*_tmp2B=te->ns;struct Cyc_List_List*_tmp2C=0;while(1){struct
Cyc_Tcenv_Genv*_tmp2D=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(te->ae,_tmp2B);{struct Cyc_List_List*_tmp2E=_tmp2D->availables;
for(0;_tmp2E != 0;_tmp2E=_tmp2E->tl){struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)
_tmp2E->hd);if(((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(
ge2->namespaces,n))_tmp2C=({struct Cyc_List_List*_tmp2F=_cycalloc(sizeof(*_tmp2F));
_tmp2F->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp2B,({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));
_tmp30->hd=n;_tmp30->tl=ns;_tmp30;}));_tmp2F->tl=_tmp2C;_tmp2F;});}}if(((int(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp2D->namespaces,n))
_tmp2C=({struct Cyc_List_List*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->hd=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp2B,({struct Cyc_List_List*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->hd=n;
_tmp32->tl=ns;_tmp32;}));_tmp31->tl=_tmp2C;_tmp31;});if(_tmp2C != 0){if(_tmp2C->tl
!= 0)({struct Cyc_Std_String_pa_struct _tmp34;_tmp34.tag=0;_tmp34.f1=(struct
_tagged_arr)*n;{void*_tmp33[1]={& _tmp34};Cyc_Tcutil_terr(loc,_tag_arr("%s is ambiguous",
sizeof(char),16),_tag_arr(_tmp33,sizeof(void*),1));}});return(struct Cyc_List_List*)
_tmp2C->hd;}if(_tmp2B == 0)(int)_throw((void*)Cyc_Dict_Absent);_tmp2B=Cyc_Tcenv_outer_namespace(
_tmp2B);}}static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*
ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->aggrdecls,v);}struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct _tuple1*q){struct _tuple1 _tmp36;void*_tmp37;struct _tagged_arr*_tmp38;struct
_tuple1*_tmp35=q;_tmp36=*_tmp35;_tmp37=_tmp36.f1;_tmp38=_tmp36.f2;{void*_tmp39=
_tmp37;struct Cyc_List_List*_tmp3A;struct Cyc_List_List*_tmp3B;struct Cyc_List_List*
_tmp3C;struct Cyc_List_List _tmp3D;struct _tagged_arr*_tmp3E;struct Cyc_List_List*
_tmp3F;_LL19: if((int)_tmp39 != 0)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp39 <= (
void*)1?1:*((int*)_tmp39)!= 0)goto _LL1D;_tmp3A=((struct Cyc_Absyn_Rel_n_struct*)
_tmp39)->f1;if(_tmp3A != 0)goto _LL1D;_LL1C: return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp38);_LL1D: if(_tmp39 <= (void*)1?1:*((int*)
_tmp39)!= 1)goto _LL1F;_tmp3B=((struct Cyc_Absyn_Abs_n_struct*)_tmp39)->f1;_LL1E: {
struct Cyc_Tcenv_Genv*_tmp40=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp3B);return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp40->aggrdecls,
_tmp38);}_LL1F: if(_tmp39 <= (void*)1?1:*((int*)_tmp39)!= 0)goto _LL18;_tmp3C=((
struct Cyc_Absyn_Rel_n_struct*)_tmp39)->f1;if(_tmp3C == 0)goto _LL18;_tmp3D=*_tmp3C;
_tmp3E=(struct _tagged_arr*)_tmp3D.hd;_tmp3F=_tmp3D.tl;_LL20: {struct Cyc_Tcenv_Genv*
_tmp41=Cyc_Tcenv_lookup_namespace(te,loc,_tmp3E,_tmp3F);return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp41->aggrdecls,
_tmp38);}_LL18:;}}static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv*ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->tuniondecls,v);}
struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp43;void*_tmp44;
struct _tagged_arr*_tmp45;struct _tuple1*_tmp42=q;_tmp43=*_tmp42;_tmp44=_tmp43.f1;
_tmp45=_tmp43.f2;{void*_tmp46=_tmp44;struct Cyc_List_List*_tmp47;struct Cyc_List_List*
_tmp48;struct Cyc_List_List _tmp49;struct _tagged_arr*_tmp4A;struct Cyc_List_List*
_tmp4B;struct Cyc_List_List*_tmp4C;_LL22: if((int)_tmp46 != 0)goto _LL24;_LL23: goto
_LL25;_LL24: if(_tmp46 <= (void*)1?1:*((int*)_tmp46)!= 0)goto _LL26;_tmp47=((struct
Cyc_Absyn_Rel_n_struct*)_tmp46)->f1;if(_tmp47 != 0)goto _LL26;_LL25: return((struct
Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Tuniondecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),
struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_tuniondecl_f,
_tmp45);_LL26: if(_tmp46 <= (void*)1?1:*((int*)_tmp46)!= 0)goto _LL28;_tmp48=((
struct Cyc_Absyn_Rel_n_struct*)_tmp46)->f1;if(_tmp48 == 0)goto _LL28;_tmp49=*_tmp48;
_tmp4A=(struct _tagged_arr*)_tmp49.hd;_tmp4B=_tmp49.tl;_LL27: {struct Cyc_Tcenv_Genv*
_tmp4D=Cyc_Tcenv_lookup_namespace(te,loc,_tmp4A,_tmp4B);return((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp4D->tuniondecls,
_tmp45);}_LL28: if(_tmp46 <= (void*)1?1:*((int*)_tmp46)!= 1)goto _LL21;_tmp4C=((
struct Cyc_Absyn_Abs_n_struct*)_tmp46)->f1;_LL29: {struct Cyc_Tcenv_Genv*_tmp4E=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp4C);return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp4E->tuniondecls,_tmp45);}_LL21:;}}
static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_xtuniondecl_f(struct Cyc_Tcenv_Genv*
ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->tuniondecls,v);}struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_xtuniondecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q){struct _tuple1 _tmp50;void*_tmp51;struct _tagged_arr*_tmp52;
struct _tuple1*_tmp4F=q;_tmp50=*_tmp4F;_tmp51=_tmp50.f1;_tmp52=_tmp50.f2;{void*
_tmp53=_tmp51;struct Cyc_List_List*_tmp54;struct Cyc_List_List*_tmp55;struct Cyc_List_List
_tmp56;struct _tagged_arr*_tmp57;struct Cyc_List_List*_tmp58;struct Cyc_List_List*
_tmp59;_LL2B: if(_tmp53 <= (void*)1?1:*((int*)_tmp53)!= 0)goto _LL2D;_tmp54=((
struct Cyc_Absyn_Rel_n_struct*)_tmp53)->f1;if(_tmp54 != 0)goto _LL2D;_LL2C: {struct
_handler_cons _tmp5A;_push_handler(& _tmp5A);{int _tmp5C=0;if(setjmp(_tmp5A.handler))
_tmp5C=1;if(!_tmp5C){{struct Cyc_Core_Opt*_tmp5E=({struct Cyc_Core_Opt*_tmp5D=
_cycalloc(sizeof(*_tmp5D));_tmp5D->v=((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl**(*lookup)(struct
Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_xtuniondecl_f,_tmp52);_tmp5D;});_npop_handler(0);return
_tmp5E;};_pop_handler();}else{void*_tmp5B=(void*)_exn_thrown;void*_tmp60=_tmp5B;
_LL34: if(_tmp60 != Cyc_Dict_Absent)goto _LL36;_LL35: return 0;_LL36:;_LL37:(void)
_throw(_tmp60);_LL33:;}}}_LL2D: if((int)_tmp53 != 0)goto _LL2F;_LL2E:({void*_tmp61[
0]={};Cyc_Tcutil_terr(loc,_tag_arr("lookup_xtuniondecl: impossible",sizeof(char),
31),_tag_arr(_tmp61,sizeof(void*),0));});return 0;_LL2F: if(_tmp53 <= (void*)1?1:*((
int*)_tmp53)!= 0)goto _LL31;_tmp55=((struct Cyc_Absyn_Rel_n_struct*)_tmp53)->f1;
if(_tmp55 == 0)goto _LL31;_tmp56=*_tmp55;_tmp57=(struct _tagged_arr*)_tmp56.hd;
_tmp58=_tmp56.tl;_LL30: {struct Cyc_Tcenv_Genv*ge;{struct _handler_cons _tmp62;
_push_handler(& _tmp62);{int _tmp64=0;if(setjmp(_tmp62.handler))_tmp64=1;if(!
_tmp64){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp57,_tmp58);;_pop_handler();}
else{void*_tmp63=(void*)_exn_thrown;void*_tmp66=_tmp63;_LL39: if(_tmp66 != Cyc_Dict_Absent)
goto _LL3B;_LL3A:({void*_tmp67[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad qualified name for xtunion",
sizeof(char),31),_tag_arr(_tmp67,sizeof(void*),0));});(int)_throw((void*)Cyc_Dict_Absent);
_LL3B:;_LL3C:(void)_throw(_tmp66);_LL38:;}}}return({struct Cyc_Core_Opt*_tmp68=
_cycalloc(sizeof(*_tmp68));_tmp68->v=((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->tuniondecls,_tmp52);_tmp68;});}_LL31:
if(_tmp53 <= (void*)1?1:*((int*)_tmp53)!= 1)goto _LL2A;_tmp59=((struct Cyc_Absyn_Abs_n_struct*)
_tmp53)->f1;_LL32: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp59);return({struct Cyc_Core_Opt*
_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->v=((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->tuniondecls,
_tmp52);_tmp69;});}_LL2A:;}}static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->enumdecls,v);}
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp6B;void*_tmp6C;
struct _tagged_arr*_tmp6D;struct _tuple1*_tmp6A=q;_tmp6B=*_tmp6A;_tmp6C=_tmp6B.f1;
_tmp6D=_tmp6B.f2;{void*_tmp6E=_tmp6C;struct Cyc_List_List*_tmp6F;struct Cyc_List_List*
_tmp70;struct Cyc_List_List _tmp71;struct _tagged_arr*_tmp72;struct Cyc_List_List*
_tmp73;struct Cyc_List_List*_tmp74;_LL3E: if((int)_tmp6E != 0)goto _LL40;_LL3F: goto
_LL41;_LL40: if(_tmp6E <= (void*)1?1:*((int*)_tmp6E)!= 0)goto _LL42;_tmp6F=((struct
Cyc_Absyn_Rel_n_struct*)_tmp6E)->f1;if(_tmp6F != 0)goto _LL42;_LL41: return((struct
Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),
struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,
_tmp6D);_LL42: if(_tmp6E <= (void*)1?1:*((int*)_tmp6E)!= 0)goto _LL44;_tmp70=((
struct Cyc_Absyn_Rel_n_struct*)_tmp6E)->f1;if(_tmp70 == 0)goto _LL44;_tmp71=*_tmp70;
_tmp72=(struct _tagged_arr*)_tmp71.hd;_tmp73=_tmp71.tl;_LL43: {struct Cyc_Tcenv_Genv*
_tmp75=Cyc_Tcenv_lookup_namespace(te,loc,_tmp72,_tmp73);return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp75->enumdecls,
_tmp6D);}_LL44: if(_tmp6E <= (void*)1?1:*((int*)_tmp6E)!= 1)goto _LL3D;_tmp74=((
struct Cyc_Absyn_Abs_n_struct*)_tmp6E)->f1;_LL45: {struct Cyc_Tcenv_Genv*_tmp76=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp74);return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp76->enumdecls,_tmp6D);}_LL3D:;}}static
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*
ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->typedefs,v);}struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q){struct _tuple1 _tmp78;void*_tmp79;struct _tagged_arr*_tmp7A;
struct _tuple1*_tmp77=q;_tmp78=*_tmp77;_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;{void*
_tmp7B=_tmp79;struct Cyc_List_List*_tmp7C;struct Cyc_List_List*_tmp7D;struct Cyc_List_List
_tmp7E;struct _tagged_arr*_tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_List_List*
_tmp81;_LL47: if((int)_tmp7B != 0)goto _LL49;_LL48: goto _LL4A;_LL49: if(_tmp7B <= (
void*)1?1:*((int*)_tmp7B)!= 0)goto _LL4B;_tmp7C=((struct Cyc_Absyn_Rel_n_struct*)
_tmp7B)->f1;if(_tmp7C != 0)goto _LL4B;_LL4A: return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*(*
lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp7A);_LL4B: if(_tmp7B <= (void*)1?1:*((
int*)_tmp7B)!= 0)goto _LL4D;_tmp7D=((struct Cyc_Absyn_Rel_n_struct*)_tmp7B)->f1;
if(_tmp7D == 0)goto _LL4D;_tmp7E=*_tmp7D;_tmp7F=(struct _tagged_arr*)_tmp7E.hd;
_tmp80=_tmp7E.tl;_LL4C: {struct Cyc_Tcenv_Genv*_tmp82=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp7F,_tmp80);return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp82->typedefs,_tmp7A);}_LL4D: if(
_tmp7B <= (void*)1?1:*((int*)_tmp7B)!= 1)goto _LL46;_tmp81=((struct Cyc_Absyn_Abs_n_struct*)
_tmp7B)->f1;_LL4E: {struct Cyc_Tcenv_Genv*_tmp83=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp81);
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k))Cyc_Dict_lookup)(_tmp83->typedefs,_tmp7A);}_LL46:;}}struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(){
return({struct Cyc_Tcenv_Genv*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->namespaces=(
struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)_check_null(Cyc_Tcutil_empty_var_set))->v;
_tmp84->aggrdecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp84->tuniondecls=((struct
Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp84->enumdecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp84->typedefs=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp84->ordinaries=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);
_tmp84->availables=0;_tmp84;});}void*Cyc_Tcenv_get_visible(void*f){while(1){void*
_tmp85=f;void*_tmp86;void*_tmp87;void*_tmp88;void*_tmp89;_LL50: if(*((int*)_tmp85)
!= 0)goto _LL52;_tmp86=(void*)((struct Cyc_Tcenv_Outermost_struct*)_tmp85)->f1;
_LL51: return _tmp86;_LL52: if(*((int*)_tmp85)!= 1)goto _LL54;_tmp87=(void*)((struct
Cyc_Tcenv_Frame_struct*)_tmp85)->f1;_LL53: return _tmp87;_LL54: if(*((int*)_tmp85)
!= 2)goto _LL4F;_tmp88=(void*)((struct Cyc_Tcenv_Hidden_struct*)_tmp85)->f1;_tmp89=(
void*)((struct Cyc_Tcenv_Hidden_struct*)_tmp85)->f2;_LL55: f=_tmp89;goto _LL4F;
_LL4F:;}}void*Cyc_Tcenv_put_visible(void*f,void*x){void*_tmp8A=f;void*_tmp8B;
void*_tmp8C;void*_tmp8D;void*_tmp8E;void*_tmp8F;_LL57: if(*((int*)_tmp8A)!= 0)
goto _LL59;_tmp8B=(void*)((struct Cyc_Tcenv_Outermost_struct*)_tmp8A)->f1;_LL58:
return(void*)({struct Cyc_Tcenv_Outermost_struct*_tmp90=_cycalloc(sizeof(*_tmp90));
_tmp90[0]=({struct Cyc_Tcenv_Outermost_struct _tmp91;_tmp91.tag=0;_tmp91.f1=(void*)
x;_tmp91;});_tmp90;});_LL59: if(*((int*)_tmp8A)!= 1)goto _LL5B;_tmp8C=(void*)((
struct Cyc_Tcenv_Frame_struct*)_tmp8A)->f1;_tmp8D=(void*)((struct Cyc_Tcenv_Frame_struct*)
_tmp8A)->f2;_LL5A: return(void*)({struct Cyc_Tcenv_Frame_struct*_tmp92=_cycalloc(
sizeof(*_tmp92));_tmp92[0]=({struct Cyc_Tcenv_Frame_struct _tmp93;_tmp93.tag=1;
_tmp93.f1=(void*)x;_tmp93.f2=(void*)_tmp8D;_tmp93;});_tmp92;});_LL5B: if(*((int*)
_tmp8A)!= 2)goto _LL56;_tmp8E=(void*)((struct Cyc_Tcenv_Hidden_struct*)_tmp8A)->f1;
_tmp8F=(void*)((struct Cyc_Tcenv_Hidden_struct*)_tmp8A)->f2;_LL5C: return(void*)({
struct Cyc_Tcenv_Hidden_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94[0]=({
struct Cyc_Tcenv_Hidden_struct _tmp95;_tmp95.tag=2;_tmp95.f1=(void*)_tmp8E;_tmp95.f2=(
void*)Cyc_Tcenv_put_visible(_tmp8F,x);_tmp95;});_tmp94;});_LL56:;}void*Cyc_Tcenv_apply_to_visible(
void*(*g)(void*),void*f){return Cyc_Tcenv_put_visible(f,g(Cyc_Tcenv_get_visible(f)));}
void*Cyc_Tcenv_map_frames(void*(*g)(void*),void*f){void*_tmp96=f;void*_tmp97;
void*_tmp98;void*_tmp99;void*_tmp9A;void*_tmp9B;_LL5E: if(*((int*)_tmp96)!= 0)
goto _LL60;_tmp97=(void*)((struct Cyc_Tcenv_Outermost_struct*)_tmp96)->f1;_LL5F:
return(void*)({struct Cyc_Tcenv_Outermost_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));
_tmp9C[0]=({struct Cyc_Tcenv_Outermost_struct _tmp9D;_tmp9D.tag=0;_tmp9D.f1=(void*)
g(_tmp97);_tmp9D;});_tmp9C;});_LL60: if(*((int*)_tmp96)!= 1)goto _LL62;_tmp98=(
void*)((struct Cyc_Tcenv_Frame_struct*)_tmp96)->f1;_tmp99=(void*)((struct Cyc_Tcenv_Frame_struct*)
_tmp96)->f2;_LL61: return(void*)({struct Cyc_Tcenv_Frame_struct*_tmp9E=_cycalloc(
sizeof(*_tmp9E));_tmp9E[0]=({struct Cyc_Tcenv_Frame_struct _tmp9F;_tmp9F.tag=1;
_tmp9F.f1=(void*)g(_tmp98);_tmp9F.f2=(void*)Cyc_Tcenv_map_frames(g,_tmp99);
_tmp9F;});_tmp9E;});_LL62: if(*((int*)_tmp96)!= 2)goto _LL5D;_tmp9A=(void*)((
struct Cyc_Tcenv_Hidden_struct*)_tmp96)->f1;_tmp9B=(void*)((struct Cyc_Tcenv_Hidden_struct*)
_tmp96)->f2;_LL63: return(void*)({struct Cyc_Tcenv_Hidden_struct*_tmpA0=_cycalloc(
sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_Tcenv_Hidden_struct _tmpA1;_tmpA1.tag=2;
_tmpA1.f1=(void*)g(_tmp9A);_tmpA1.f2=(void*)Cyc_Tcenv_map_frames(g,_tmp9B);
_tmpA1;});_tmpA0;});_LL5D:;}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct
Cyc_Tcenv_Tenv*te){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("get_fenv",sizeof(char),9));return((struct Cyc_Tcenv_Fenv*(*)(void*f))
Cyc_Tcenv_get_visible)((void*)((struct Cyc_Core_Opt*)_check_null(te->le))->v);}
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*
fe){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("put_fenv",
sizeof(char),9));return({struct Cyc_Tcenv_Tenv*_tmpA2=_cycalloc(sizeof(*_tmpA2));
_tmpA2->ns=te->ns;_tmpA2->ae=te->ae;_tmpA2->le=({struct Cyc_Core_Opt*_tmpA3=
_cycalloc(sizeof(*_tmpA3));_tmpA3->v=(void*)((void*(*)(void*f,struct Cyc_Tcenv_Fenv*
x))Cyc_Tcenv_put_visible)((void*)((struct Cyc_Core_Opt*)_check_null(te->le))->v,
fe);_tmpA3;});_tmpA2;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct Cyc_Tcenv_Fenv*f){return({struct Cyc_Tcenv_Fenv*_tmpA4=_cycalloc(sizeof(*
_tmpA4));_tmpA4[0]=*f;_tmpA4;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_Fenv*_tmpA5=({struct Cyc_Tcenv_Fenv*
_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=*f;_tmpA7;});_tmpA5->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=*f->ctrl_env;
_tmpA6;});return _tmpA5;}void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){if(te->le
== 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected return_typ",
sizeof(char),29));return(void*)((Cyc_Tcenv_get_fenv(te))->shared)->return_typ;}
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){if(te->le
== 0)return 0;return(Cyc_Tcenv_get_fenv(te))->type_vars;}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected add_type_vars",
sizeof(char),32));{struct Cyc_Tcenv_Fenv*_tmpA8=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(
te));Cyc_Tcutil_add_tvar_identities(tvs);{struct Cyc_List_List*_tmpA9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpA8->type_vars);
Cyc_Tcutil_check_unique_tvars(loc,_tmpA9);_tmpA8->type_vars=_tmpA9;return Cyc_Tcenv_put_fenv(
te,_tmpA8);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){if(te->le == 0)((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected add_local_var",
sizeof(char),32));{struct Cyc_Tcenv_Fenv*_tmpAA=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(
te));struct _tagged_arr*_tmpAB=(*vd->name).f2;{void*_tmpAC=(*vd->name).f1;_LL65:
if((int)_tmpAC != 0)goto _LL67;_LL66: goto _LL64;_LL67:;_LL68:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({
struct Cyc_Core_Impossible_struct _tmpAE;_tmpAE.tag=Cyc_Core_Impossible;_tmpAE.f1=
_tag_arr("add_local_var: called with Rel_n",sizeof(char),33);_tmpAE;});_tmpAD;}));
_LL64:;}_tmpAA->locals=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,void*v))Cyc_Dict_insert)(_tmpAA->locals,_tmpAB,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_Local_b_struct
_tmpB0;_tmpB0.tag=3;_tmpB0.f1=vd;_tmpB0;});_tmpAF;}));return Cyc_Tcenv_put_fenv(
te,_tmpAA);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){if(te->le == 0)((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected add_pat_var",
sizeof(char),30));{struct Cyc_Tcenv_Fenv*_tmpB1=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(
te));struct _tagged_arr*_tmpB2=(*vd->name).f2;_tmpB1->locals=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(_tmpB1->locals,
_tmpB2,(void*)({struct Cyc_Absyn_Pat_b_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));
_tmpB3[0]=({struct Cyc_Absyn_Pat_b_struct _tmpB4;_tmpB4.tag=4;_tmpB4.f1=vd;_tmpB4;});
_tmpB3;}));return Cyc_Tcenv_put_fenv(te,_tmpB1);}}void*Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmpB6;void*_tmpB7;struct _tagged_arr*_tmpB8;struct _tuple1*_tmpB5=q;_tmpB6=*
_tmpB5;_tmpB7=_tmpB6.f1;_tmpB8=_tmpB6.f2;{void*_tmpB9=_tmpB7;struct Cyc_List_List*
_tmpBA;_LL6A: if((int)_tmpB9 != 0)goto _LL6C;_LL6B: if(te->le == 0)(int)_throw((void*)
Cyc_Dict_Absent);goto _LL6D;_LL6C: if(_tmpB9 <= (void*)1?1:*((int*)_tmpB9)!= 0)goto
_LL6E;_tmpBA=((struct Cyc_Absyn_Rel_n_struct*)_tmpB9)->f1;if(_tmpBA != 0)goto _LL6E;
if(!(te->le != 0))goto _LL6E;_LL6D: {struct Cyc_Tcenv_Fenv*_tmpBB=Cyc_Tcenv_get_fenv(
te);struct _handler_cons _tmpBC;_push_handler(& _tmpBC);{int _tmpBE=0;if(setjmp(
_tmpBC.handler))_tmpBE=1;if(!_tmpBE){{void*_tmpC1=(void*)({struct Cyc_Tcenv_VarRes_struct*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Tcenv_VarRes_struct
_tmpC0;_tmpC0.tag=0;_tmpC0.f1=(void*)((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmpBB->locals,_tmpB8);_tmpC0;});_tmpBF;});
_npop_handler(0);return _tmpC1;};_pop_handler();}else{void*_tmpBD=(void*)
_exn_thrown;void*_tmpC3=_tmpBD;_LL71: if(_tmpC3 != Cyc_Dict_Absent)goto _LL73;_LL72:
return Cyc_Tcenv_lookup_ordinary_global(te,loc,q);_LL73:;_LL74:(void)_throw(
_tmpC3);_LL70:;}}}_LL6E:;_LL6F: {struct _handler_cons _tmpC4;_push_handler(& _tmpC4);{
int _tmpC6=0;if(setjmp(_tmpC4.handler))_tmpC6=1;if(!_tmpC6){{void*_tmpC7=Cyc_Tcenv_lookup_ordinary_global(
te,loc,q);_npop_handler(0);return _tmpC7;};_pop_handler();}else{void*_tmpC5=(void*)
_exn_thrown;void*_tmpC9=_tmpC5;_LL76: if(_tmpC9 != Cyc_Dict_Absent)goto _LL78;_LL77:
return(void*)({struct Cyc_Tcenv_VarRes_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));
_tmpCA[0]=({struct Cyc_Tcenv_VarRes_struct _tmpCB;_tmpCB.tag=0;_tmpCB.f1=(void*)((
void*)0);_tmpCB;});_tmpCA;});_LL78:;_LL79:(void)_throw(_tmpC9);_LL75:;}}}_LL69:;}}
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,
struct Cyc_Absyn_Stmt**sopt){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected process_continue",sizeof(char),35));{void*_tmpCC=(
void*)((Cyc_Tcenv_get_fenv(te))->ctrl_env)->continue_stmt;struct Cyc_Absyn_Stmt*
_tmpCD;_LL7B: if(_tmpCC <= (void*)3?1:*((int*)_tmpCC)!= 0)goto _LL7D;_tmpCD=((
struct Cyc_Tcenv_Stmt_j_struct*)_tmpCC)->f1;_LL7C: _tmpCD->non_local_preds=({
struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=s;_tmpCE->tl=
_tmpCD->non_local_preds;_tmpCE;});*sopt=(struct Cyc_Absyn_Stmt*)_tmpCD;return;
_LL7D: if((int)_tmpCC != 0)goto _LL7F;_LL7E:({void*_tmpCF[0]={};Cyc_Tcutil_terr(s->loc,
_tag_arr("continue not in a loop",sizeof(char),23),_tag_arr(_tmpCF,sizeof(void*),
0));});return;_LL7F: if((int)_tmpCC != 1)goto _LL81;_LL80: goto _LL82;_LL81: if((int)
_tmpCC != 2)goto _LL7A;_LL82:((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: bad continue destination",sizeof(char),32));_LL7A:;}}void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){if(te->le
== 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected process_break",
sizeof(char),32));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te);void*_tmpD0=(
void*)(fe->ctrl_env)->break_stmt;struct Cyc_Absyn_Stmt*_tmpD1;_LL84: if(_tmpD0 <= (
void*)3?1:*((int*)_tmpD0)!= 0)goto _LL86;_tmpD1=((struct Cyc_Tcenv_Stmt_j_struct*)
_tmpD0)->f1;_LL85: _tmpD1->non_local_preds=({struct Cyc_List_List*_tmpD2=_cycalloc(
sizeof(*_tmpD2));_tmpD2->hd=s;_tmpD2->tl=_tmpD1->non_local_preds;_tmpD2;});*sopt=(
struct Cyc_Absyn_Stmt*)_tmpD1;return;_LL86: if((int)_tmpD0 != 0)goto _LL88;_LL87:({
void*_tmpD3[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("break not in a loop or switch",
sizeof(char),30),_tag_arr(_tmpD3,sizeof(void*),0));});return;_LL88: if((int)
_tmpD0 != 2)goto _LL8A;_LL89: if((void*)(fe->shared)->return_typ != (void*)0)({void*
_tmpD4[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("break causes function not to return a value",
sizeof(char),44),_tag_arr(_tmpD4,sizeof(void*),0));});return;_LL8A: if((int)
_tmpD0 != 1)goto _LL83;_LL8B:({void*_tmpD5[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("break causes outer switch clause to implicitly fallthru",
sizeof(char),56),_tag_arr(_tmpD5,sizeof(void*),0));});return;_LL83:;}}void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _tagged_arr*l,struct Cyc_Absyn_Stmt**
sopt){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected process_goto",
sizeof(char),31));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te);struct Cyc_Core_Opt*
_tmpD6=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)((
fe->shared)->seen_labels,l);if(_tmpD6 == 0){struct Cyc_SlowDict_Dict*_tmpD7=(fe->shared)->needed_labels;
struct Cyc_Core_Opt*_tmpD8=((struct Cyc_Core_Opt*(*)(struct Cyc_SlowDict_Dict*d,
struct _tagged_arr*k))Cyc_SlowDict_lookup_opt)(_tmpD7,l);if(_tmpD8 == 0)_tmpD8=({
struct Cyc_Core_Opt*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->v=0;_tmpD9;});(fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)(struct Cyc_SlowDict_Dict*d,struct _tagged_arr*k,struct
Cyc_List_List*v))Cyc_SlowDict_insert)(_tmpD7,l,({struct Cyc_List_List*_tmpDA=
_cycalloc(sizeof(*_tmpDA));_tmpDA->hd=s;_tmpDA->tl=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpD8))->v;_tmpDA;}));}else{((struct Cyc_Absyn_Stmt*)
_tmpD6->v)->non_local_preds=({struct Cyc_List_List*_tmpDB=_cycalloc(sizeof(*
_tmpDB));_tmpDB->hd=s;_tmpDB->tl=((struct Cyc_Absyn_Stmt*)_tmpD6->v)->non_local_preds;
_tmpDB;});*sopt=(struct Cyc_Absyn_Stmt*)((struct Cyc_Absyn_Stmt*)_tmpD6->v);}}}
struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Switch_clause***clauseopt){if(te->le == 0)((int(*)(struct
_tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected process_fallthru",
sizeof(char),35));{struct Cyc_Tcenv_Fenv*_tmpDC=Cyc_Tcenv_get_fenv(te);struct
_tuple4*_tmpDD=(_tmpDC->ctrl_env)->fallthru_clause;if(_tmpDD != 0){(((*_tmpDD).f1)->body)->non_local_preds=({
struct Cyc_List_List*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->hd=s;_tmpDE->tl=(((*
_tmpDD).f1)->body)->non_local_preds;_tmpDE;});*clauseopt=(struct Cyc_Absyn_Switch_clause**)({
struct Cyc_Absyn_Switch_clause**_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=(*
_tmpDD).f1;_tmpDF;});}return _tmpDD;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause){if(te->le == 0)((int(*)(struct _tagged_arr
msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected set_fallthru",sizeof(char),
31));{struct Cyc_List_List*_tmpE0=0;for(0;vds != 0;vds=vds->tl){_tmpE0=({struct Cyc_List_List*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->hd=(void*)((void*)((struct Cyc_Absyn_Vardecl*)
vds->hd)->type);_tmpE1->tl=_tmpE0;_tmpE1;});}_tmpE0=Cyc_List_imp_rev(_tmpE0);{
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(Cyc_Tcenv_get_fenv(te));(fe->ctrl_env)->fallthru_clause=({
struct _tuple4*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->f1=clause;_tmpE2->f2=
new_tvs;_tmpE2->f3=_tmpE0;_tmpE2;});return Cyc_Tcenv_put_fenv(te,fe);}}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*te){if(te->le == 0)((int(*)(struct
_tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected clear_fallthru",
sizeof(char),33));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(Cyc_Tcenv_get_fenv(
te));(fe->ctrl_env)->fallthru_clause=0;return Cyc_Tcenv_put_fenv(te,fe);}}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
continue_dest){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_in_loop",sizeof(char),30));{struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(Cyc_Tcenv_get_fenv(te));(void*)((fe->ctrl_env)->continue_stmt=(
void*)((void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmpE3=_cycalloc(sizeof(*_tmpE3));
_tmpE3[0]=({struct Cyc_Tcenv_Stmt_j_struct _tmpE4;_tmpE4.tag=0;_tmpE4.f1=
continue_dest;_tmpE4;});_tmpE3;})));(void*)((fe->ctrl_env)->break_stmt=(void*)((
void*)(fe->ctrl_env)->next_stmt));(void*)((fe->ctrl_env)->next_stmt=(void*)((
void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5[0]=({
struct Cyc_Tcenv_Stmt_j_struct _tmpE6;_tmpE6.tag=0;_tmpE6.f1=continue_dest;_tmpE6;});
_tmpE5;})));return Cyc_Tcenv_put_fenv(te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct Cyc_Tcenv_Tenv*te){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected enter_try",sizeof(char),28));{struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(Cyc_Tcenv_get_fenv(te));++(fe->ctrl_env)->try_depth;
return Cyc_Tcenv_put_fenv(te,fe);}}int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*
te){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected get_try_depth",
sizeof(char),32));return((Cyc_Tcenv_get_fenv(te))->ctrl_env)->try_depth;}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct Cyc_Tcenv_Tenv*te){if(te->le == 0)((
int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected set_in_switch",
sizeof(char),32));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(Cyc_Tcenv_get_fenv(
te));(void*)((fe->ctrl_env)->break_stmt=(void*)((void*)(fe->ctrl_env)->next_stmt));(
void*)((fe->ctrl_env)->next_stmt=(void*)((void*)1));return Cyc_Tcenv_put_fenv(te,
fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct Cyc_Tcenv_Tenv*te,void*j){
if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected set_next",
sizeof(char),27));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(Cyc_Tcenv_get_fenv(
te));(void*)((fe->ctrl_env)->next_stmt=(void*)j);return Cyc_Tcenv_put_fenv(te,fe);}}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct
_tagged_arr*v,struct Cyc_Absyn_Stmt*s){if(te->le == 0)((int(*)(struct _tagged_arr
msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected add_label",sizeof(char),28));{
struct Cyc_Tcenv_Fenv*_tmpE7=Cyc_Tcenv_get_fenv(te);struct Cyc_SlowDict_Dict*
_tmpE8=(_tmpE7->shared)->needed_labels;struct Cyc_Core_Opt*_tmpE9=((struct Cyc_Core_Opt*(*)(
struct Cyc_SlowDict_Dict*d,struct _tagged_arr*k))Cyc_SlowDict_lookup_opt)(_tmpE8,v);
if(_tmpE9 != 0){(_tmpE7->shared)->needed_labels=((struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict*d,struct _tagged_arr*k))Cyc_SlowDict_delete)(_tmpE8,v);{
struct Cyc_List_List*_tmpEA=(struct Cyc_List_List*)_tmpE9->v;s->non_local_preds=
_tmpEA;for(0;_tmpEA != 0;_tmpEA=_tmpEA->tl){void*_tmpEB=(void*)((struct Cyc_Absyn_Stmt*)
_tmpEA->hd)->r;struct Cyc_Absyn_Stmt*_tmpEC;struct Cyc_Absyn_Stmt**_tmpED;_LL8D:
if(_tmpEB <= (void*)1?1:*((int*)_tmpEB)!= 7)goto _LL8F;_tmpEC=((struct Cyc_Absyn_Goto_s_struct*)
_tmpEB)->f2;_tmpED=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)
_tmpEB)->f2;_LL8E:*_tmpED=(struct Cyc_Absyn_Stmt*)s;goto _LL8C;_LL8F:;_LL90:((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: add_label backpatching of non-goto",
sizeof(char),42));goto _LL8C;_LL8C:;}}}if(((int(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_member)((_tmpE7->shared)->seen_labels,v))({struct Cyc_Std_String_pa_struct
_tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _tagged_arr)*v;{void*_tmpEE[1]={& _tmpEF};
Cyc_Tcutil_terr(s->loc,_tag_arr("Repeated label: %s",sizeof(char),19),_tag_arr(
_tmpEE,sizeof(void*),1));}});(_tmpE7->shared)->seen_labels=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)((
_tmpE7->shared)->seen_labels,v,s);return te;}}int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*te){return((int(*)(struct Cyc_SlowDict_Dict*d))Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv(te))->shared)->needed_labels);}struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*te){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected get_encloser",sizeof(char),31));return(Cyc_Tcenv_get_fenv(
te))->encloser;}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_encloser",sizeof(char),31));{struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(te));fe->encloser=s;return Cyc_Tcenv_put_fenv(
te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*
r,int resetable){if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv::add_region: unexpected add_region",sizeof(char),41));{struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(te));struct Cyc_Absyn_Tvar*tv;{
void*_tmpF0=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpF1;_LL92: if(_tmpF0 <= (
void*)3?1:*((int*)_tmpF0)!= 1)goto _LL94;_tmpF1=((struct Cyc_Absyn_VarType_struct*)
_tmpF0)->f1;_LL93: tv=_tmpF1;goto _LL91;_LL94:;_LL95: tv=((struct Cyc_Absyn_Tvar*(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("bad add region",sizeof(char),
15));goto _LL91;_LL91:;}fe->region_order=Cyc_RgnOrder_add_youngest(fe->region_order,
tv,resetable);(void*)(fe->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2[0]=({struct Cyc_Absyn_JoinEff_struct
_tmpF3;_tmpF3.tag=20;_tmpF3.f1=({struct Cyc_List_List*_tmpF4=_cycalloc(sizeof(*
_tmpF4));_tmpF4->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmpF6=
_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_AccessEff_struct _tmpF7;
_tmpF7.tag=19;_tmpF7.f1=(void*)r;_tmpF7;});_tmpF6;}));_tmpF4->tl=({struct Cyc_List_List*
_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->hd=(void*)((void*)fe->capability);
_tmpF5->tl=0;_tmpF5;});_tmpF4;});_tmpF3;});_tmpF2;})));return Cyc_Tcenv_put_fenv(
te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){if(te->le == 0)((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected new_block",
sizeof(char),28));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(
te));void*block_typ=(void*)({struct Cyc_Absyn_VarType_struct*_tmpFF=_cycalloc(
sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_VarType_struct _tmp100;_tmp100.tag=1;
_tmp100.f1=block_rgn;_tmp100;});_tmpFF;});fe->type_vars=({struct Cyc_List_List*
_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->hd=block_rgn;_tmpF8->tl=fe->type_vars;
_tmpF8;});Cyc_Tcutil_check_unique_tvars(loc,fe->type_vars);fe->region_order=Cyc_RgnOrder_add_youngest(
fe->region_order,block_rgn,0);(void*)(fe->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_JoinEff_struct
_tmpFA;_tmpFA.tag=20;_tmpFA.f1=({struct Cyc_List_List*_tmpFB=_cycalloc(sizeof(*
_tmpFB));_tmpFB->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmpFD=
_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_AccessEff_struct _tmpFE;
_tmpFE.tag=19;_tmpFE.f1=(void*)block_typ;_tmpFE;});_tmpFD;}));_tmpFB->tl=({
struct Cyc_List_List*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->hd=(void*)((void*)
fe->capability);_tmpFC->tl=0;_tmpFC;});_tmpFB;});_tmpFA;});_tmpF9;})));(void*)(
fe->curr_rgn=(void*)block_typ);return Cyc_Tcenv_put_fenv(te,fe);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp102;_tmp102.tag=0;_tmp102.f1=(void*)((void*)3);_tmp102;});_tmp101;}));Cyc_Tcutil_add_tvar_identity(
t);return Cyc_Tcenv_new_named_block(loc,te,t);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs){if(te->le == 0)((int(*)(struct
_tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected new_outlives_constraints",
sizeof(char),43));{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(
te));struct Cyc_RgnOrder_RgnPO*_tmp103=fe->region_order;for(0;cs != 0;cs=cs->tl){
_tmp103=Cyc_RgnOrder_add_outlives_constraint(_tmp103,(*((struct _tuple0*)cs->hd)).f1,(*((
struct _tuple0*)cs->hd)).f2);}fe->region_order=_tmp103;return Cyc_Tcenv_put_fenv(
te,fe);}}void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){if(te->le == 0)return(
void*)2;return(void*)(Cyc_Tcenv_get_fenv(te))->curr_rgn;}void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){if(te->le == 0)((
int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected check_rgn_accessible",
sizeof(char),39));{struct Cyc_Tcenv_Fenv*_tmp104=Cyc_Tcenv_get_fenv(te);void*
_tmp105=(void*)_tmp104->capability;if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp105)?
1: Cyc_Tcutil_region_in_effect(1,rgn,_tmp105))return;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp104->region_order,(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp106=
_cycalloc(sizeof(*_tmp106));_tmp106[0]=({struct Cyc_Absyn_AccessEff_struct _tmp107;
_tmp107.tag=19;_tmp107.f1=(void*)rgn;_tmp107;});_tmp106;}),_tmp105))return;({
struct Cyc_Std_String_pa_struct _tmp109;_tmp109.tag=0;_tmp109.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(rgn);{void*_tmp108[1]={& _tmp109};Cyc_Tcutil_terr(
loc,_tag_arr("Expression accesses unavailable region %s",sizeof(char),42),
_tag_arr(_tmp108,sizeof(void*),1));}});}}void Cyc_Tcenv_check_rgn_resetable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){if(te->le == 0)((
int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected check_rgn_resetable",
sizeof(char),38));Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{void*_tmp10A=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp10B;_LL97: if(_tmp10A <= (void*)3?1:*((int*)_tmp10A)
!= 1)goto _LL99;_tmp10B=((struct Cyc_Absyn_VarType_struct*)_tmp10A)->f1;_LL98: if(!
Cyc_RgnOrder_is_region_resetable((Cyc_Tcenv_get_fenv(te))->region_order,_tmp10B))({
struct Cyc_Std_String_pa_struct _tmp10D;_tmp10D.tag=0;_tmp10D.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(rgn);{void*_tmp10C[1]={& _tmp10D};Cyc_Tcutil_terr(
loc,_tag_arr("Region %s is not resetable",sizeof(char),27),_tag_arr(_tmp10C,
sizeof(void*),1));}});return;_LL99:;_LL9A:({void*_tmp10E[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_rgn_resetable",
sizeof(char),20),_tag_arr(_tmp10E,sizeof(void*),0));});_LL96:;}}int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){if(te->le == 0)return rt_a == (void*)2;
return Cyc_RgnOrder_effect_outlives((Cyc_Tcenv_get_fenv(te))->region_order,(void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp110;_tmp110.tag=19;_tmp110.f1=(void*)rt_a;
_tmp110;});_tmp10F;}),rt_b);}struct _tuple6{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*
f3;struct Cyc_Position_Segment*f4;};void Cyc_Tcenv_check_effect_accessible(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff){if(te->le == 0)((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected check_effect_accessible",
sizeof(char),42));{struct Cyc_Tcenv_Fenv*_tmp111=Cyc_Tcenv_get_fenv(te);void*
_tmp112=(void*)_tmp111->capability;if(Cyc_Tcutil_subset_effect(0,eff,_tmp112))
return;if(Cyc_RgnOrder_eff_outlives_eff(_tmp111->region_order,eff,_tmp112))
return;(_tmp111->shared)->delayed_effect_checks=({struct Cyc_List_List*_tmp113=
_cycalloc(sizeof(*_tmp113));_tmp113->hd=({struct _tuple6*_tmp114=_cycalloc(
sizeof(*_tmp114));_tmp114->f1=_tmp112;_tmp114->f2=eff;_tmp114->f3=_tmp111->region_order;
_tmp114->f4=loc;_tmp114;});_tmp113->tl=(_tmp111->shared)->delayed_effect_checks;
_tmp113;});}}void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){if(te->le
== 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected check_delayed_effects",
sizeof(char),40));{struct Cyc_List_List*_tmp115=((Cyc_Tcenv_get_fenv(te))->shared)->delayed_effect_checks;
for(0;_tmp115 != 0;_tmp115=_tmp115->tl){struct _tuple6 _tmp117;void*_tmp118;void*
_tmp119;struct Cyc_RgnOrder_RgnPO*_tmp11A;struct Cyc_Position_Segment*_tmp11B;
struct _tuple6*_tmp116=(struct _tuple6*)_tmp115->hd;_tmp117=*_tmp116;_tmp118=
_tmp117.f1;_tmp119=_tmp117.f2;_tmp11A=_tmp117.f3;_tmp11B=_tmp117.f4;if(Cyc_Tcutil_subset_effect(
1,_tmp119,_tmp118))continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp11A,_tmp119,
_tmp118))continue;({struct Cyc_Std_String_pa_struct _tmp11E;_tmp11E.tag=0;_tmp11E.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp119);{struct Cyc_Std_String_pa_struct
_tmp11D;_tmp11D.tag=0;_tmp11D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp118);{void*_tmp11C[2]={& _tmp11D,& _tmp11E};Cyc_Tcutil_terr(_tmp11B,_tag_arr("Capability \n%s\ndoes not cover function's effect\n%s",
sizeof(char),51),_tag_arr(_tmp11C,sizeof(void*),2));}}});}}}struct _tuple7{struct
Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;struct Cyc_Position_Segment*f3;};
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*po){if(te->le == 0){for(0;po != 0;po=po->tl){if(!Cyc_Tcutil_subset_effect(
1,(*((struct _tuple0*)po->hd)).f1,Cyc_Absyn_empty_effect)?1: !Cyc_Tcutil_subset_effect(
1,(*((struct _tuple0*)po->hd)).f2,Cyc_Absyn_empty_effect))({void*_tmp11F[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("the required region ordering is not satisfied here",
sizeof(char),51),_tag_arr(_tmp11F,sizeof(void*),0));});}return;}{struct Cyc_Tcenv_Fenv*
_tmp120=Cyc_Tcenv_get_fenv(te);if(!Cyc_RgnOrder_satisfies_constraints(_tmp120->region_order,
po,(void*)2,0))(_tmp120->shared)->delayed_constraint_checks=({struct Cyc_List_List*
_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->hd=({struct _tuple7*_tmp122=
_cycalloc(sizeof(*_tmp122));_tmp122->f1=_tmp120->region_order;_tmp122->f2=po;
_tmp122->f3=loc;_tmp122;});_tmp121->tl=(_tmp120->shared)->delayed_constraint_checks;
_tmp121;});}}void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
if(te->le == 0)((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected check_delayed_constraints",
sizeof(char),44));{struct Cyc_List_List*_tmp123=((Cyc_Tcenv_get_fenv(te))->shared)->delayed_constraint_checks;
for(0;_tmp123 != 0;_tmp123=_tmp123->tl){struct _tuple7 _tmp125;struct Cyc_RgnOrder_RgnPO*
_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_Position_Segment*_tmp128;struct
_tuple7*_tmp124=(struct _tuple7*)_tmp123->hd;_tmp125=*_tmp124;_tmp126=_tmp125.f1;
_tmp127=_tmp125.f2;_tmp128=_tmp125.f3;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp126,_tmp127,(void*)2,1))({void*_tmp129[0]={};Cyc_Tcutil_terr(_tmp128,
_tag_arr("the required region ordering is not satisfied here",sizeof(char),51),
_tag_arr(_tmp129,sizeof(void*),0));});}}}struct _tuple8{struct _tagged_arr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct
Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Dict_Dict*locals=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp154=
_cycalloc(sizeof(*_tmp154));_tmp154->name=({struct _tagged_arr*_tmp157=_cycalloc(
sizeof(*_tmp157));_tmp157[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp159;_tmp159.tag=0;_tmp159.f1=(struct _tagged_arr)*(*fd->name).f2;{void*
_tmp158[1]={& _tmp159};Cyc_Std_aprintf(_tag_arr("`%s",sizeof(char),4),_tag_arr(
_tmp158,sizeof(void*),1));}});_tmp157;});_tmp154->identity=Cyc_Tcutil_new_tvar_id();
_tmp154->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp155=_cycalloc(
sizeof(*_tmp155));_tmp155[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp156;_tmp156.tag=0;
_tmp156.f1=(void*)((void*)3);_tmp156;});_tmp155;}));_tmp154;});struct Cyc_List_List*
_tmp12A=({struct Cyc_List_List*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->hd=
rgn0;_tmp153->tl=fd->tvs;_tmp153;});Cyc_Tcutil_check_unique_tvars(loc,_tmp12A);{
struct Cyc_RgnOrder_RgnPO*_tmp12B=Cyc_RgnOrder_initial_fn_po(fd->tvs,fd->rgn_po,(
void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,rgn0);void*param_rgn=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp151=_cycalloc(sizeof(*_tmp151));
_tmp151[0]=({struct Cyc_Absyn_VarType_struct _tmp152;_tmp152.tag=1;_tmp152.f1=rgn0;
_tmp152;});_tmp151;});struct Cyc_List_List*_tmp12C=0;{struct Cyc_List_List*_tmp12D=
fd->args;for(0;_tmp12D != 0;_tmp12D=_tmp12D->tl){struct Cyc_Absyn_Vardecl*_tmp12E=({
struct Cyc_Absyn_Vardecl*_tmp133=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);
_tmp133[0]=({struct Cyc_Absyn_Vardecl _tmp134;_tmp134.sc=(void*)((void*)2);_tmp134.name=({
struct _tuple1*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->f1=(void*)0;_tmp136->f2=(*((
struct _tuple8*)_tmp12D->hd)).f1;_tmp136;});_tmp134.tq=(*((struct _tuple8*)_tmp12D->hd)).f2;
_tmp134.type=(void*)(*((struct _tuple8*)_tmp12D->hd)).f3;_tmp134.initializer=0;
_tmp134.rgn=({struct Cyc_Core_Opt*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->v=(
void*)param_rgn;_tmp135;});_tmp134.attributes=0;_tmp134.escapes=0;_tmp134;});
_tmp133;});_tmp12C=({struct Cyc_List_List*_tmp12F=_cycalloc(sizeof(struct Cyc_List_List)
* 1);_tmp12F[0]=({struct Cyc_List_List _tmp130;_tmp130.hd=_tmp12E;_tmp130.tl=
_tmp12C;_tmp130;});_tmp12F;});locals=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(locals,(*((struct _tuple8*)_tmp12D->hd)).f1,(
void*)({struct Cyc_Absyn_Param_b_struct*_tmp131=_cycalloc(sizeof(*_tmp131));
_tmp131[0]=({struct Cyc_Absyn_Param_b_struct _tmp132;_tmp132.tag=2;_tmp132.f1=
_tmp12E;_tmp132;});_tmp131;}));}}if(fd->cyc_varargs != 0){struct Cyc_Core_Opt*
_tmp138;struct Cyc_Absyn_Tqual _tmp139;void*_tmp13A;int _tmp13B;struct Cyc_Absyn_VarargInfo
_tmp137=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));_tmp138=
_tmp137.name;_tmp139=_tmp137.tq;_tmp13A=(void*)_tmp137.type;_tmp13B=_tmp137.inject;
if(_tmp138 != 0){void*_tmp13C=Cyc_Absyn_tagged_typ(_tmp13A,param_rgn,_tmp139);
struct Cyc_Absyn_Vardecl*_tmp13D=({struct Cyc_Absyn_Vardecl*_tmp141=_cycalloc(
sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp141[0]=({struct Cyc_Absyn_Vardecl _tmp142;
_tmp142.sc=(void*)((void*)2);_tmp142.name=({struct _tuple1*_tmp144=_cycalloc(
sizeof(*_tmp144));_tmp144->f1=(void*)0;_tmp144->f2=(struct _tagged_arr*)_tmp138->v;
_tmp144;});_tmp142.tq=Cyc_Absyn_empty_tqual();_tmp142.type=(void*)_tmp13C;
_tmp142.initializer=0;_tmp142.rgn=({struct Cyc_Core_Opt*_tmp143=_cycalloc(sizeof(*
_tmp143));_tmp143->v=(void*)param_rgn;_tmp143;});_tmp142.attributes=0;_tmp142.escapes=
0;_tmp142;});_tmp141;});_tmp12C=({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*
_tmp13E));_tmp13E->hd=_tmp13D;_tmp13E->tl=_tmp12C;_tmp13E;});locals=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(locals,(
struct _tagged_arr*)_tmp138->v,(void*)({struct Cyc_Absyn_Param_b_struct*_tmp13F=
_cycalloc(sizeof(*_tmp13F));_tmp13F[0]=({struct Cyc_Absyn_Param_b_struct _tmp140;
_tmp140.tag=2;_tmp140.f1=_tmp13D;_tmp140;});_tmp13F;}));}else{({void*_tmp145[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("missing name for varargs",sizeof(char),25),
_tag_arr(_tmp145,sizeof(void*),0));});}}fd->param_vardecls=({struct Cyc_Core_Opt*
_tmp146=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp146[0]=({struct Cyc_Core_Opt
_tmp147;_tmp147.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp12C);_tmp147;});_tmp146;});return({struct Cyc_Tcenv_Fenv*_tmp148=_cycalloc(
sizeof(*_tmp148));_tmp148->shared=({struct Cyc_Tcenv_SharedFenv*_tmp150=_cycalloc(
sizeof(*_tmp150));_tmp150->return_typ=(void*)((void*)fd->ret_type);_tmp150->seen_labels=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp150->needed_labels=((struct Cyc_SlowDict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_SlowDict_empty)(Cyc_Std_zstrptrcmp);
_tmp150->delayed_effect_checks=0;_tmp150->delayed_constraint_checks=0;_tmp150;});
_tmp148->type_vars=_tmp12A;_tmp148->region_order=_tmp12B;_tmp148->locals=locals;
_tmp148->encloser=fd->body;_tmp148->ctrl_env=({struct Cyc_Tcenv_CtrlEnv*_tmp14F=
_cycalloc(sizeof(*_tmp14F));_tmp14F->continue_stmt=(void*)((void*)0);_tmp14F->break_stmt=(
void*)((void*)0);_tmp14F->fallthru_clause=0;_tmp14F->next_stmt=(void*)((void*)2);
_tmp14F->try_depth=0;_tmp14F;});_tmp148->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp14A;_tmp14A.tag=20;_tmp14A.f1=({struct Cyc_List_List*_tmp14B=_cycalloc(
sizeof(*_tmp14B));_tmp14B->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp14E;_tmp14E.tag=19;_tmp14E.f1=(void*)param_rgn;_tmp14E;});_tmp14D;}));
_tmp14B->tl=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->hd=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v);_tmp14C->tl=0;
_tmp14C;});_tmp14B;});_tmp14A;});_tmp149;}));_tmp148->curr_rgn=(void*)param_rgn;
_tmp148;});}}
