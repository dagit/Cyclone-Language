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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stderr;extern int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);extern
unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_vfprintf(struct Cyc_Std___cycFILE*,
struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{int
tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);extern int Cyc_List_length(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern unsigned char Cyc_List_List_mismatch[18];extern void Cyc_List_iter_c(void(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_rrev(struct
_RegionHandle*,struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];extern void*Cyc_List_nth(
struct Cyc_List_List*x,int n);extern int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*
x);extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);extern
struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct
Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));extern
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);extern int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));extern struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);extern void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict*d,void*k);extern struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict*d,void*k);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;
struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
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
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Aggrdecl*f3;};
struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
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
f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;
int f2;};struct Cyc_Absyn_Char_p_struct{int tag;unsigned char f1;};struct Cyc_Absyn_Float_p_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct
Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;
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
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);extern struct Cyc_Absyn_Rel_n_struct
Cyc_Absyn_rel_ns_null_value;extern void*Cyc_Absyn_rel_ns_null;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_conref_def(
void*,struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_uchar_t;extern void*Cyc_Absyn_uint_t;
extern void*Cyc_Absyn_sint_t;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
extern void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_void_star_typ();
extern void*Cyc_Absyn_strct(struct _tagged_arr*name);extern void*Cyc_Absyn_strctq(
struct _tuple0*name);extern void*Cyc_Absyn_unionq_typ(struct _tuple0*name);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_char_exp(unsigned char c,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_float_exp(struct _tagged_arr f,struct Cyc_Position_Segment*);extern struct
Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _tagged_arr s,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(
void*s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct
Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct
Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(
struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _tagged_arr*v,struct
Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _tagged_arr*lab,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,
struct Cyc_Absyn_Exp*init);extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);extern int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(
struct Cyc_List_List*,struct _tagged_arr*);extern struct _tagged_arr*Cyc_Absyn_fieldname(
int);extern struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);extern
struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);extern struct
_tagged_arr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);extern struct
_tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Tcenv_VarRes_struct{
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
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*
Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);extern void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct
_tagged_arr ap);extern void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);extern
void*Cyc_Tcutil_typ_kind(void*t);extern void*Cyc_Tcutil_compress(void*t);extern
int Cyc_Tcutil_is_pointer_type(void*t);extern int Cyc_Tcutil_unify(void*,void*);
extern struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields);extern int Cyc_Tcutil_is_tagged_pointer_typ(void*);extern int
Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);struct _tuple3{
struct Cyc_Absyn_Tqual f1;void*f2;};extern void*Cyc_Tcutil_snd_tqt(struct _tuple3*);
extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};extern struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);extern struct _tuple0*Cyc_Toc_temp_var();int Cyc_Toc_warn_bounds_checks=
0;unsigned int Cyc_Toc_total_bounds_checks=0;unsigned int Cyc_Toc_bounds_checks_eliminated=
0;static struct Cyc_List_List*Cyc_Toc_result_decls=0;static struct Cyc_List_List*Cyc_Toc_tuple_types=
0;static int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;unsigned char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented";
unsigned char Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible";static
void*Cyc_Toc_unimp(struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Std_vfprintf(
Cyc_Std_stderr,fmt,ap);({void*_tmp0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(_tmp0,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___cycFILE*)Cyc_Std_stderr);(int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}
static void*Cyc_Toc_toc_impos(struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Std_vfprintf(
Cyc_Std_stderr,fmt,ap);({void*_tmp1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(_tmp1,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___cycFILE*)Cyc_Std_stderr);(int)_throw((void*)Cyc_Toc_Toc_Impossible);}
unsigned char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error";static
unsigned char _tmp2[5]="curr";static struct _tagged_arr Cyc_Toc_curr_string={_tmp2,
_tmp2,_tmp2 + 5};static struct _tagged_arr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static unsigned char _tmp3[4]="tag";static struct _tagged_arr Cyc_Toc_tag_string={
_tmp3,_tmp3,_tmp3 + 4};static struct _tagged_arr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static unsigned char _tmp4[14]="_handler_cons";static struct _tagged_arr Cyc_Toc__handler_cons_string={
_tmp4,_tmp4,_tmp4 + 14};static struct _tagged_arr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static unsigned char _tmp5[8]="handler";static struct _tagged_arr Cyc_Toc_handler_string={
_tmp5,_tmp5,_tmp5 + 8};static struct _tagged_arr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static unsigned char _tmp6[14]="_RegionHandle";static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_tmp6,_tmp6,_tmp6 + 14};static struct _tagged_arr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static unsigned char _tmp7[7]="_throw";static struct _tagged_arr Cyc_Toc__throw_str={
_tmp7,_tmp7,_tmp7 + 7};static struct _tuple0 Cyc_Toc__throw_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((void*)&
Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static unsigned char _tmp9[7]="setjmp";static
struct _tagged_arr Cyc_Toc_setjmp_str={_tmp9,_tmp9,_tmp9 + 7};static struct _tuple0
Cyc_Toc_setjmp_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)((void*)& Cyc_Toc_setjmp_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=&
Cyc_Toc_setjmp_ev;static unsigned char _tmpB[14]="_push_handler";static struct
_tagged_arr Cyc_Toc__push_handler_str={_tmpB,_tmpB,_tmpB + 14};static struct _tuple0
Cyc_Toc__push_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((
void*)& Cyc_Toc__push_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static
unsigned char _tmpD[13]="_pop_handler";static struct _tagged_arr Cyc_Toc__pop_handler_str={
_tmpD,_tmpD,_tmpD + 13};static struct _tuple0 Cyc_Toc__pop_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
unsigned char _tmpF[12]="_exn_thrown";static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_tmpF,_tmpF,_tmpF + 12};static struct _tuple0 Cyc_Toc__exn_thrown_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={
0,(void*)((void*)& Cyc_Toc__exn_thrown_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static
unsigned char _tmp11[14]="_npop_handler";static struct _tagged_arr Cyc_Toc__npop_handler_str={
_tmp11,_tmp11,_tmp11 + 14};static struct _tuple0 Cyc_Toc__npop_handler_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)((void*)& Cyc_Toc__npop_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev;static unsigned char _tmp13[12]="_check_null";static
struct _tagged_arr Cyc_Toc__check_null_str={_tmp13,_tmp13,_tmp13 + 12};static struct
_tuple0 Cyc_Toc__check_null_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((
void*)& Cyc_Toc__check_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static
unsigned char _tmp15[28]="_check_known_subscript_null";static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={
_tmp15,_tmp15,_tmp15 + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _tmp17[31]="_check_known_subscript_notnull";static struct
_tagged_arr Cyc_Toc__check_known_subscript_notnull_str={_tmp17,_tmp17,_tmp17 + 31};
static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static unsigned char _tmp19[25]="_check_unknown_subscript";
static struct _tagged_arr Cyc_Toc__check_unknown_subscript_str={_tmp19,_tmp19,
_tmp19 + 25};static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1,& Cyc_Toc__check_unknown_subscript_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_unknown_subscript_ev={0,(void*)((void*)& Cyc_Toc__check_unknown_subscript_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_unknown_subscript_e=&
Cyc_Toc__check_unknown_subscript_ev;static unsigned char _tmp1B[12]="_tagged_arr";
static struct _tagged_arr Cyc_Toc__tagged_arr_str={_tmp1B,_tmp1B,_tmp1B + 12};static
struct _tuple0 Cyc_Toc__tagged_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={1,& Cyc_Toc__tagged_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={0,(void*)((
void*)& Cyc_Toc__tagged_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;static
unsigned char _tmp1D[9]="_tag_arr";static struct _tagged_arr Cyc_Toc__tag_arr_str={
_tmp1D,_tmp1D,_tmp1D + 9};static struct _tuple0 Cyc_Toc__tag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={1,&
Cyc_Toc__tag_arr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,(void*)((void*)& Cyc_Toc__tag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_arr_e=& Cyc_Toc__tag_arr_ev;static
unsigned char _tmp1F[14]="_init_tag_arr";static struct _tagged_arr Cyc_Toc__init_tag_arr_str={
_tmp1F,_tmp1F,_tmp1F + 14};static struct _tuple0 Cyc_Toc__init_tag_arr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_tag_arr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__init_tag_arr_re={1,& Cyc_Toc__init_tag_arr_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__init_tag_arr_ev={0,(void*)((void*)& Cyc_Toc__init_tag_arr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__init_tag_arr_e=&
Cyc_Toc__init_tag_arr_ev;static unsigned char _tmp21[11]="_untag_arr";static struct
_tagged_arr Cyc_Toc__untag_arr_str={_tmp21,_tmp21,_tmp21 + 11};static struct _tuple0
Cyc_Toc__untag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_arr_re={1,& Cyc_Toc__untag_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={0,(void*)((
void*)& Cyc_Toc__untag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;static
unsigned char _tmp23[14]="_get_arr_size";static struct _tagged_arr Cyc_Toc__get_arr_size_str={
_tmp23,_tmp23,_tmp23 + 14};static struct _tuple0 Cyc_Toc__get_arr_size_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_arr_size_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_arr_size_re={1,& Cyc_Toc__get_arr_size_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={0,(void*)((void*)& Cyc_Toc__get_arr_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__get_arr_size_e=&
Cyc_Toc__get_arr_size_ev;static unsigned char _tmp25[17]="_tagged_arr_plus";static
struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={_tmp25,_tmp25,_tmp25 + 17};static
struct _tuple0 Cyc_Toc__tagged_arr_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tagged_arr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_plus_re={
1,& Cyc_Toc__tagged_arr_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={
0,(void*)((void*)& Cyc_Toc__tagged_arr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_plus_e=& Cyc_Toc__tagged_arr_plus_ev;
static unsigned char _tmp27[25]="_tagged_arr_inplace_plus";static struct _tagged_arr
Cyc_Toc__tagged_arr_inplace_plus_str={_tmp27,_tmp27,_tmp27 + 25};static struct
_tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tagged_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={
1,& Cyc_Toc__tagged_arr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_ev={0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_e=&
Cyc_Toc__tagged_arr_inplace_plus_ev;static unsigned char _tmp29[30]="_tagged_arr_inplace_plus_post";
static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={_tmp29,_tmp29,
_tmp29 + 30};static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={1,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _tmp2B[10]="_cycalloc";static struct _tagged_arr Cyc_Toc__cycalloc_str={
_tmp2B,_tmp2B,_tmp2B + 10};static struct _tuple0 Cyc_Toc__cycalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static
unsigned char _tmp2D[11]="_cyccalloc";static struct _tagged_arr Cyc_Toc__cyccalloc_str={
_tmp2D,_tmp2D,_tmp2D + 11};static struct _tuple0 Cyc_Toc__cyccalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={
1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static
unsigned char _tmp2F[17]="_cycalloc_atomic";static struct _tagged_arr Cyc_Toc__cycalloc_atomic_str={
_tmp2F,_tmp2F,_tmp2F + 17};static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=&
Cyc_Toc__cycalloc_atomic_ev;static unsigned char _tmp31[18]="_cyccalloc_atomic";
static struct _tagged_arr Cyc_Toc__cyccalloc_atomic_str={_tmp31,_tmp31,_tmp31 + 18};
static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static unsigned char _tmp33[15]="_region_malloc";static struct _tagged_arr Cyc_Toc__region_malloc_str={
_tmp33,_tmp33,_tmp33 + 15};static struct _tuple0 Cyc_Toc__region_malloc_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static unsigned char _tmp35[15]="_region_calloc";static
struct _tagged_arr Cyc_Toc__region_calloc_str={_tmp35,_tmp35,_tmp35 + 15};static
struct _tuple0 Cyc_Toc__region_calloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)((
void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static
unsigned char _tmp37[13]="_check_times";static struct _tagged_arr Cyc_Toc__check_times_str={
_tmp37,_tmp37,_tmp37 + 13};static struct _tuple0 Cyc_Toc__check_times_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static unsigned char _tmp39[12]="_new_region";static struct
_tagged_arr Cyc_Toc__new_region_str={_tmp39,_tmp39,_tmp39 + 12};static struct
_tuple0 Cyc_Toc__new_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)((
void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
unsigned char _tmp3B[13]="_push_region";static struct _tagged_arr Cyc_Toc__push_region_str={
_tmp3B,_tmp3B,_tmp3B + 13};static struct _tuple0 Cyc_Toc__push_region_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static unsigned char _tmp3D[12]="_pop_region";static struct
_tagged_arr Cyc_Toc__pop_region_str={_tmp3D,_tmp3D,_tmp3D + 12};static struct
_tuple0 Cyc_Toc__pop_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)((
void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static struct
Cyc_Absyn_UnknownAggr_struct Cyc_Toc_tagged_arr_typ_u={0,(void*)((void*)0),& Cyc_Toc__tagged_arr_pr};
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_tagged_arr_typ_v={10,{(void*)((void*)&
Cyc_Toc_tagged_arr_typ_u),0}};static void*Cyc_Toc_tagged_arr_typ=(void*)& Cyc_Toc_tagged_arr_typ_v;
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0};struct _tuple4{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmp41=(void*)e->r;void*_tmp42;unsigned char _tmp43;void*_tmp44;short _tmp45;void*
_tmp46;int _tmp47;void*_tmp48;long long _tmp49;void*_tmp4A;struct Cyc_Absyn_Exp*
_tmp4B;struct Cyc_List_List*_tmp4C;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*
_tmp4E;struct Cyc_List_List*_tmp4F;struct Cyc_List_List*_tmp50;_LL1: if(*((int*)
_tmp41)== 0){_LL19: _tmp42=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp41)->f1;
if((unsigned int)_tmp42 > 1?*((int*)_tmp42)== 0: 0){_LL1A: _tmp43=((struct Cyc_Absyn_Char_c_struct*)
_tmp42)->f2;goto _LL2;}else{goto _LL3;}}else{goto _LL3;}_LL3: if(*((int*)_tmp41)== 0){
_LL1B: _tmp44=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp41)->f1;if((
unsigned int)_tmp44 > 1?*((int*)_tmp44)== 1: 0){_LL1C: _tmp45=((struct Cyc_Absyn_Short_c_struct*)
_tmp44)->f2;goto _LL4;}else{goto _LL5;}}else{goto _LL5;}_LL5: if(*((int*)_tmp41)== 0){
_LL1D: _tmp46=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp41)->f1;if((
unsigned int)_tmp46 > 1?*((int*)_tmp46)== 2: 0){_LL1E: _tmp47=((struct Cyc_Absyn_Int_c_struct*)
_tmp46)->f2;goto _LL6;}else{goto _LL7;}}else{goto _LL7;}_LL7: if(*((int*)_tmp41)== 0){
_LL1F: _tmp48=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp41)->f1;if((
unsigned int)_tmp48 > 1?*((int*)_tmp48)== 3: 0){_LL20: _tmp49=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp48)->f2;goto _LL8;}else{goto _LL9;}}else{goto _LL9;}_LL9: if(*((int*)_tmp41)== 0){
_LL21: _tmp4A=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp41)->f1;if((int)_tmp4A
== 0){goto _LLA;}else{goto _LLB;}}else{goto _LLB;}_LLB: if(*((int*)_tmp41)== 13){
_LL22: _tmp4B=((struct Cyc_Absyn_Cast_e_struct*)_tmp41)->f2;goto _LLC;}else{goto
_LLD;}_LLD: if(*((int*)_tmp41)== 24){_LL23: _tmp4C=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp41)->f1;goto _LLE;}else{goto _LLF;}_LLF: if(*((int*)_tmp41)== 26){_LL24: _tmp4D=((
struct Cyc_Absyn_Array_e_struct*)_tmp41)->f1;goto _LL10;}else{goto _LL11;}_LL11: if(*((
int*)_tmp41)== 28){_LL25: _tmp4E=((struct Cyc_Absyn_Struct_e_struct*)_tmp41)->f2;
goto _LL12;}else{goto _LL13;}_LL13: if(*((int*)_tmp41)== 25){_LL26: _tmp4F=((struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp41)->f2;goto _LL14;}else{goto _LL15;}_LL15: if(*((
int*)_tmp41)== 34){_LL27: _tmp50=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp41)->f2;
goto _LL16;}else{goto _LL17;}_LL17: goto _LL18;_LL2: return _tmp43 == '\000';_LL4:
return _tmp45 == 0;_LL6: return _tmp47 == 0;_LL8: return _tmp49 == 0;_LLA: return 1;_LLC:
return Cyc_Toc_is_zero(_tmp4B);_LLE: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp4C);_LL10: _tmp4E=
_tmp4D;goto _LL12;_LL12: _tmp4F=_tmp4E;goto _LL14;_LL14: _tmp50=_tmp4F;goto _LL16;
_LL16: for(0;_tmp50 != 0;_tmp50=_tmp50->tl){if(! Cyc_Toc_is_zero((*((struct _tuple4*)
_tmp50->hd)).f2)){return 0;}}return 1;_LL18: return 0;_LL0:;}static int Cyc_Toc_is_nullable(
void*t){void*_tmp51=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp52;struct
Cyc_Absyn_Conref*_tmp53;_LL29: if((unsigned int)_tmp51 > 3?*((int*)_tmp51)== 4: 0){
_LL2D: _tmp52=((struct Cyc_Absyn_PointerType_struct*)_tmp51)->f1;_LL2E: _tmp53=
_tmp52.nullable;goto _LL2A;}else{goto _LL2B;}_LL2B: goto _LL2C;_LL2A: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp53);_LL2C:({void*
_tmp54[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("is_nullable",sizeof(unsigned char),12),_tag_arr(_tmp54,sizeof(void*),0));});
_LL28:;}static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_tagged_arr tag){return({struct _tuple0*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->f1=(*
x).f1;_tmp55->f2=({struct _tagged_arr*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56[0]=(
struct _tagged_arr)Cyc_Std_strconcat(*(*x).f2,tag);_tmp56;});_tmp55;});}struct
_tuple5{struct _tagged_arr*f1;struct Cyc_List_List*f2;};static struct _tagged_arr*
Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){{struct Cyc_List_List*_tmp57=Cyc_Toc_tuple_types;
for(0;_tmp57 != 0;_tmp57=_tmp57->tl){struct _tuple5 _tmp59;struct Cyc_List_List*
_tmp5A;struct _tagged_arr*_tmp5B;struct _tuple5*_tmp58=(struct _tuple5*)_tmp57->hd;
_tmp59=*_tmp58;_LL31: _tmp5B=_tmp59.f1;goto _LL30;_LL30: _tmp5A=_tmp59.f2;goto _LL2F;
_LL2F: {struct Cyc_List_List*_tmp5C=tqs0;for(0;_tmp5C != 0? _tmp5A != 0: 0;(_tmp5C=
_tmp5C->tl,_tmp5A=_tmp5A->tl)){if(! Cyc_Tcutil_unify((*((struct _tuple3*)_tmp5C->hd)).f2,(
void*)_tmp5A->hd)){break;}}if(_tmp5C == 0? _tmp5A == 0: 0){return _tmp5B;}}}}{struct
_tagged_arr*x=({struct _tagged_arr*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=(
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp6B;_tmp6B.tag=1;_tmp6B.f1=(
int)((unsigned int)Cyc_Toc_tuple_type_counter ++);{void*_tmp6A[1]={& _tmp6B};Cyc_Std_aprintf(
_tag_arr("_tuple%d",sizeof(unsigned char),9),_tag_arr(_tmp6A,sizeof(void*),1));}});
_tmp69;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple3*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct
Cyc_List_List*_tmp5D=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,
i ++)){_tmp5D=({struct Cyc_List_List*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->hd=({
struct Cyc_Absyn_Aggrfield*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->name=Cyc_Absyn_fieldname(
i);_tmp5F->tq=Cyc_Toc_mt_tq;_tmp5F->type=(void*)((void*)ts2->hd);_tmp5F->width=0;
_tmp5F->attributes=0;_tmp5F;});_tmp5E->tl=_tmp5D;_tmp5E;});}}_tmp5D=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5D);{struct Cyc_Absyn_Aggrdecl*
_tmp60=({struct Cyc_Absyn_Aggrdecl*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->kind=(
void*)((void*)0);_tmp66->sc=(void*)((void*)2);_tmp66->name=({struct _tuple0*
_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->f1=Cyc_Absyn_rel_ns_null;_tmp68->f2=x;
_tmp68;});_tmp66->tvs=0;_tmp66->exist_vars=0;_tmp66->fields=({struct Cyc_Core_Opt*
_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->v=_tmp5D;_tmp67;});_tmp66->attributes=
0;_tmp66;});Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp61=_cycalloc(sizeof(*
_tmp61));_tmp61->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp63;_tmp63.tag=4;_tmp63.f1=_tmp60;_tmp63;});_tmp62;}),0);_tmp61->tl=Cyc_Toc_result_decls;
_tmp61;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*
_tmp64));_tmp64->hd=({struct _tuple5*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->f1=
x;_tmp65->f2=ts;_tmp65;});_tmp64->tl=Cyc_Toc_tuple_types;_tmp64;});return x;}}}
struct _tuple0*Cyc_Toc_temp_var(){return({struct _tuple0*_tmp6C=_cycalloc(sizeof(*
_tmp6C));_tmp6C->f1=(void*)0;_tmp6C->f2=({struct _tagged_arr*_tmp6D=_cycalloc(
sizeof(*_tmp6D));_tmp6D[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp6F;_tmp6F.tag=1;_tmp6F.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*
_tmp6E[1]={& _tmp6F};Cyc_Std_aprintf(_tag_arr("_tmp%X",sizeof(unsigned char),7),
_tag_arr(_tmp6E,sizeof(void*),1));}});_tmp6D;});_tmp6C;});}static struct
_tagged_arr*Cyc_Toc_fresh_label(){return({struct _tagged_arr*_tmp70=_cycalloc(
sizeof(*_tmp70));_tmp70[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp72;_tmp72.tag=1;_tmp72.f1=(unsigned int)Cyc_Toc_fresh_label_counter ++;{void*
_tmp71[1]={& _tmp72};Cyc_Std_aprintf(_tag_arr("_LL%X",sizeof(unsigned char),6),
_tag_arr(_tmp71,sizeof(void*),1));}});_tmp70;});}static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(
struct Cyc_Absyn_Tuniondecl*td,struct _tuple0*name,int carries_value){int ans=0;
struct Cyc_List_List*_tmp73=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*)_check_null(_tmp73))->hd)->name)!= 0){if((carries_value?((
struct Cyc_Absyn_Tunionfield*)_tmp73->hd)->typs != 0: 0)? 1:(! carries_value?((struct
Cyc_Absyn_Tunionfield*)_tmp73->hd)->typs == 0: 0)){++ ans;}_tmp73=_tmp73->tl;}
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static int Cyc_Toc_num_void_tags(
struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*_tmp74=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;_tmp74 != 0;_tmp74=_tmp74->tl){
if(((struct Cyc_Absyn_Tunionfield*)_tmp74->hd)->typs == 0){++ ans;}}}return ans;}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple1*Cyc_Toc_arg_to_c(struct
_tuple1*a){void*_tmp76;struct Cyc_Absyn_Tqual _tmp77;struct Cyc_Core_Opt*_tmp78;
struct _tuple1 _tmp75=*a;_LL35: _tmp78=_tmp75.f1;goto _LL34;_LL34: _tmp77=_tmp75.f2;
goto _LL33;_LL33: _tmp76=_tmp75.f3;goto _LL32;_LL32: return({struct _tuple1*_tmp79=
_cycalloc(sizeof(*_tmp79));_tmp79->f1=_tmp78;_tmp79->f2=_tmp77;_tmp79->f3=Cyc_Toc_typ_to_c(
_tmp76);_tmp79;});}static struct _tuple3*Cyc_Toc_typ_to_c_f(struct _tuple3*x){void*
_tmp7B;struct Cyc_Absyn_Tqual _tmp7C;struct _tuple3 _tmp7A=*x;_LL38: _tmp7C=_tmp7A.f1;
goto _LL37;_LL37: _tmp7B=_tmp7A.f2;goto _LL36;_LL36: return({struct _tuple3*_tmp7D=
_cycalloc(sizeof(*_tmp7D));_tmp7D->f1=_tmp7C;_tmp7D->f2=Cyc_Toc_typ_to_c(_tmp7B);
_tmp7D;});}static void*Cyc_Toc_typ_to_c_array(void*t){void*_tmp7E=t;struct Cyc_Absyn_Exp*
_tmp7F;struct Cyc_Absyn_Tqual _tmp80;void*_tmp81;struct Cyc_Core_Opt*_tmp82;struct
Cyc_Core_Opt _tmp83;void*_tmp84;_LL3A: if((unsigned int)_tmp7E > 3?*((int*)_tmp7E)
== 7: 0){_LL42: _tmp81=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp7E)->f1;goto
_LL41;_LL41: _tmp80=((struct Cyc_Absyn_ArrayType_struct*)_tmp7E)->f2;goto _LL40;
_LL40: _tmp7F=((struct Cyc_Absyn_ArrayType_struct*)_tmp7E)->f3;goto _LL3B;}else{
goto _LL3C;}_LL3C: if((unsigned int)_tmp7E > 3?*((int*)_tmp7E)== 0: 0){_LL43: _tmp82=((
struct Cyc_Absyn_Evar_struct*)_tmp7E)->f2;if(_tmp82 == 0){goto _LL3E;}else{_tmp83=*
_tmp82;_LL44: _tmp84=(void*)_tmp83.v;goto _LL3D;}}else{goto _LL3E;}_LL3E: goto _LL3F;
_LL3B: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp85=_cycalloc(sizeof(*
_tmp85));_tmp85[0]=({struct Cyc_Absyn_ArrayType_struct _tmp86;_tmp86.tag=7;_tmp86.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp81);_tmp86.f2=_tmp80;_tmp86.f3=_tmp7F;_tmp86;});
_tmp85;});_LL3D: return Cyc_Toc_typ_to_c_array(_tmp84);_LL3F: return Cyc_Toc_typ_to_c(
t);_LL39:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->name=
f->name;_tmp87->tq=f->tq;_tmp87->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);
_tmp87->width=f->width;_tmp87->attributes=f->attributes;_tmp87;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){struct Cyc_List_List*_tmp88=fs;for(0;_tmp88 != 0;_tmp88=
_tmp88->tl){if(((struct Cyc_Absyn_Enumfield*)_tmp88->hd)->tag != 0){unsigned int i=
Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)
_tmp88->hd)->tag));((struct Cyc_Absyn_Enumfield*)_tmp88->hd)->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(i,0);}}}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp89=t;
struct Cyc_Core_Opt*_tmp8A;struct Cyc_Core_Opt*_tmp8B;struct Cyc_Core_Opt _tmp8C;
void*_tmp8D;struct Cyc_Absyn_Tvar*_tmp8E;struct Cyc_Absyn_TunionInfo _tmp8F;void*
_tmp90;struct Cyc_Absyn_TunionFieldInfo _tmp91;void*_tmp92;struct Cyc_Absyn_Tunionfield*
_tmp93;struct Cyc_Absyn_Tuniondecl*_tmp94;struct Cyc_Absyn_PtrInfo _tmp95;struct Cyc_Absyn_Conref*
_tmp96;struct Cyc_Absyn_Tqual _tmp97;void*_tmp98;struct Cyc_Absyn_Exp*_tmp99;struct
Cyc_Absyn_Tqual _tmp9A;void*_tmp9B;struct Cyc_Absyn_FnInfo _tmp9C;struct Cyc_List_List*
_tmp9D;struct Cyc_Absyn_VarargInfo*_tmp9E;int _tmp9F;struct Cyc_List_List*_tmpA0;
void*_tmpA1;struct Cyc_List_List*_tmpA2;struct Cyc_List_List*_tmpA3;void*_tmpA4;
struct Cyc_Absyn_AggrInfo _tmpA5;struct Cyc_List_List*_tmpA6;void*_tmpA7;struct
_tuple0*_tmpA8;struct Cyc_List_List*_tmpA9;struct Cyc_Core_Opt*_tmpAA;struct Cyc_List_List*
_tmpAB;struct _tuple0*_tmpAC;void*_tmpAD;_LL46: if((int)_tmp89 == 0){goto _LL47;}
else{goto _LL48;}_LL48: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 0: 0){_LL7A:
_tmp8A=((struct Cyc_Absyn_Evar_struct*)_tmp89)->f2;if(_tmp8A == 0){goto _LL49;}
else{goto _LL4A;}}else{goto _LL4A;}_LL4A: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)
== 0: 0){_LL7B: _tmp8B=((struct Cyc_Absyn_Evar_struct*)_tmp89)->f2;if(_tmp8B == 0){
goto _LL4C;}else{_tmp8C=*_tmp8B;_LL7C: _tmp8D=(void*)_tmp8C.v;goto _LL4B;}}else{
goto _LL4C;}_LL4C: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 1: 0){_LL7D: _tmp8E=((
struct Cyc_Absyn_VarType_struct*)_tmp89)->f1;goto _LL4D;}else{goto _LL4E;}_LL4E: if((
unsigned int)_tmp89 > 3?*((int*)_tmp89)== 2: 0){_LL7E: _tmp8F=((struct Cyc_Absyn_TunionType_struct*)
_tmp89)->f1;_LL7F: _tmp90=(void*)_tmp8F.tunion_info;if(*((int*)_tmp90)== 1){goto
_LL4F;}else{goto _LL50;}}else{goto _LL50;}_LL50: if((unsigned int)_tmp89 > 3?*((int*)
_tmp89)== 2: 0){goto _LL51;}else{goto _LL52;}_LL52: if((unsigned int)_tmp89 > 3?*((
int*)_tmp89)== 3: 0){_LL80: _tmp91=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp89)->f1;_LL81: _tmp92=(void*)_tmp91.field_info;if(*((int*)_tmp92)== 1){_LL83:
_tmp94=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp92)->f1;goto _LL82;_LL82:
_tmp93=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp92)->f2;goto _LL53;}else{
goto _LL54;}}else{goto _LL54;}_LL54: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 3:
0){goto _LL55;}else{goto _LL56;}_LL56: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 
4: 0){_LL84: _tmp95=((struct Cyc_Absyn_PointerType_struct*)_tmp89)->f1;_LL87: _tmp98=(
void*)_tmp95.elt_typ;goto _LL86;_LL86: _tmp97=_tmp95.tq;goto _LL85;_LL85: _tmp96=
_tmp95.bounds;goto _LL57;}else{goto _LL58;}_LL58: if((unsigned int)_tmp89 > 3?*((int*)
_tmp89)== 5: 0){goto _LL59;}else{goto _LL5A;}_LL5A: if((int)_tmp89 == 1){goto _LL5B;}
else{goto _LL5C;}_LL5C: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 6: 0){goto
_LL5D;}else{goto _LL5E;}_LL5E: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 7: 0){
_LL8A: _tmp9B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp89)->f1;goto _LL89;
_LL89: _tmp9A=((struct Cyc_Absyn_ArrayType_struct*)_tmp89)->f2;goto _LL88;_LL88:
_tmp99=((struct Cyc_Absyn_ArrayType_struct*)_tmp89)->f3;goto _LL5F;}else{goto _LL60;}
_LL60: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 8: 0){_LL8B: _tmp9C=((struct Cyc_Absyn_FnType_struct*)
_tmp89)->f1;_LL90: _tmpA1=(void*)_tmp9C.ret_typ;goto _LL8F;_LL8F: _tmpA0=_tmp9C.args;
goto _LL8E;_LL8E: _tmp9F=_tmp9C.c_varargs;goto _LL8D;_LL8D: _tmp9E=_tmp9C.cyc_varargs;
goto _LL8C;_LL8C: _tmp9D=_tmp9C.attributes;goto _LL61;}else{goto _LL62;}_LL62: if((
unsigned int)_tmp89 > 3?*((int*)_tmp89)== 9: 0){_LL91: _tmpA2=((struct Cyc_Absyn_TupleType_struct*)
_tmp89)->f1;goto _LL63;}else{goto _LL64;}_LL64: if((unsigned int)_tmp89 > 3?*((int*)
_tmp89)== 11: 0){_LL93: _tmpA4=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp89)->f1;goto _LL92;_LL92: _tmpA3=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp89)->f2;
goto _LL65;}else{goto _LL66;}_LL66: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 10:
0){_LL94: _tmpA5=((struct Cyc_Absyn_AggrType_struct*)_tmp89)->f1;_LL96: _tmpA7=(
void*)_tmpA5.aggr_info;goto _LL95;_LL95: _tmpA6=_tmpA5.targs;goto _LL67;}else{goto
_LL68;}_LL68: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 12: 0){_LL97: _tmpA8=((
struct Cyc_Absyn_EnumType_struct*)_tmp89)->f1;goto _LL69;}else{goto _LL6A;}_LL6A:
if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 13: 0){_LL98: _tmpA9=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp89)->f1;goto _LL6B;}else{goto _LL6C;}_LL6C: if((unsigned int)_tmp89 > 3?*((int*)
_tmp89)== 16: 0){_LL9B: _tmpAC=((struct Cyc_Absyn_TypedefType_struct*)_tmp89)->f1;
goto _LL9A;_LL9A: _tmpAB=((struct Cyc_Absyn_TypedefType_struct*)_tmp89)->f2;goto
_LL99;_LL99: _tmpAA=((struct Cyc_Absyn_TypedefType_struct*)_tmp89)->f3;goto _LL6D;}
else{goto _LL6E;}_LL6E: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 14: 0){goto
_LL6F;}else{goto _LL70;}_LL70: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 15: 0){
_LL9C: _tmpAD=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp89)->f1;goto
_LL71;}else{goto _LL72;}_LL72: if((int)_tmp89 == 2){goto _LL73;}else{goto _LL74;}
_LL74: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 17: 0){goto _LL75;}else{goto
_LL76;}_LL76: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 18: 0){goto _LL77;}else{
goto _LL78;}_LL78: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 19: 0){goto _LL79;}
else{goto _LL45;}_LL47: return t;_LL49: return Cyc_Absyn_sint_t;_LL4B: return Cyc_Toc_typ_to_c(
_tmp8D);_LL4D: if(Cyc_Tcutil_tvar_kind(_tmp8E)== (void*)0){return(void*)0;}else{
return Cyc_Absyn_void_star_typ();}_LL4F: return Cyc_Absyn_void_star_typ();_LL51:({
void*_tmpAE[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unresolved TunionType",sizeof(unsigned char),22),_tag_arr(_tmpAE,
sizeof(void*),0));});_LL53: if(_tmp93->typs == 0){if(_tmp94->is_xtunion){return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,(void*)2,Cyc_Toc_mt_tq);}else{return Cyc_Absyn_uint_t;}}else{
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp93->name,_tag_arr("_struct",
sizeof(unsigned char),8)));}_LL55:({void*_tmpAF[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unresolved TunionFieldType",
sizeof(unsigned char),27),_tag_arr(_tmpAF,sizeof(void*),0));});_LL57: _tmp98=Cyc_Toc_typ_to_c_array(
_tmp98);{void*_tmpB0=(void*)(Cyc_Absyn_compress_conref(_tmp96))->v;void*_tmpB1;
_LL9E: if((unsigned int)_tmpB0 > 1?*((int*)_tmpB0)== 0: 0){_LLA2: _tmpB1=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpB0)->f1;if((int)_tmpB1 == 0){goto _LL9F;}
else{goto _LLA0;}}else{goto _LLA0;}_LLA0: goto _LLA1;_LL9F: return Cyc_Toc_tagged_arr_typ;
_LLA1: return Cyc_Absyn_star_typ(_tmp98,(void*)2,_tmp97);_LL9D:;}_LL59: goto _LL5B;
_LL5B: goto _LL5D;_LL5D: return t;_LL5F: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpB3;_tmpB3.tag=7;_tmpB3.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp9B);_tmpB3.f2=
_tmp9A;_tmpB3.f3=_tmp99;_tmpB3;});_tmpB2;});_LL61: {struct Cyc_List_List*_tmpB4=0;
for(0;_tmp9D != 0;_tmp9D=_tmp9D->tl){void*_tmpB5=(void*)_tmp9D->hd;_LLA4: if((int)
_tmpB5 == 3){goto _LLA5;}else{goto _LLA6;}_LLA6: if((int)_tmpB5 == 4){goto _LLA7;}
else{goto _LLA8;}_LLA8: if((unsigned int)_tmpB5 > 16?*((int*)_tmpB5)== 3: 0){goto
_LLA9;}else{goto _LLAA;}_LLAA: goto _LLAB;_LLA5: goto _LLA7;_LLA7: goto _LLA9;_LLA9:
continue;_LLAB: _tmpB4=({struct Cyc_List_List*_tmpB6=_cycalloc(sizeof(*_tmpB6));
_tmpB6->hd=(void*)((void*)_tmp9D->hd);_tmpB6->tl=_tmpB4;_tmpB6;});goto _LLA3;
_LLA3:;}{struct Cyc_List_List*_tmpB7=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(
struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmpA0);
if(_tmp9E != 0){void*_tmpB8=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ((void*)_tmp9E->type,(
void*)2,Cyc_Toc_mt_tq));struct _tuple1*_tmpB9=({struct _tuple1*_tmpBB=_cycalloc(
sizeof(*_tmpBB));_tmpBB->f1=_tmp9E->name;_tmpBB->f2=_tmp9E->tq;_tmpBB->f3=_tmpB8;
_tmpBB;});_tmpB7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(_tmpB7,({struct Cyc_List_List*_tmpBA=_cycalloc(sizeof(*
_tmpBA));_tmpBA->hd=_tmpB9;_tmpBA->tl=0;_tmpBA;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_FnType_struct
_tmpBD;_tmpBD.tag=8;_tmpBD.f1=({struct Cyc_Absyn_FnInfo _tmpBE;_tmpBE.tvars=0;
_tmpBE.effect=0;_tmpBE.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmpA1);_tmpBE.args=
_tmpB7;_tmpBE.c_varargs=_tmp9F;_tmpBE.cyc_varargs=0;_tmpBE.rgn_po=0;_tmpBE.attributes=
_tmpB4;_tmpBE;});_tmpBD;});_tmpBC;});}}_LL63: _tmpA2=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct _tuple3*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,
_tmpA2);{struct _tagged_arr*_tmpBF=Cyc_Toc_add_tuple_type(_tmpA2);return Cyc_Absyn_strct(
_tmpBF);}_LL65: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpC0=
_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpC1;
_tmpC1.tag=11;_tmpC1.f1=(void*)_tmpA4;_tmpC1.f2=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_aggrfield_to_c,_tmpA3);_tmpC1;});_tmpC0;});_LL67: {struct Cyc_Absyn_Aggrdecl*
_tmpC2=Cyc_Absyn_get_known_aggrdecl(_tmpA7);if((void*)_tmpC2->kind == (void*)1){
return Cyc_Absyn_unionq_typ(_tmpC2->name);}return Cyc_Absyn_strctq(_tmpC2->name);}
_LL69: return t;_LL6B: Cyc_Toc_enumfields_to_c(_tmpA9);return t;_LL6D: if(_tmpAA == 0){({
void*_tmpC3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::typ_to_c: unresolved TypedefType",sizeof(unsigned char),38),
_tag_arr(_tmpC3,sizeof(void*),0));});}return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpC5;_tmpC5.tag=16;_tmpC5.f1=_tmpAC;_tmpC5.f2=0;_tmpC5.f3=({struct Cyc_Core_Opt*
_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->v=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmpAA->v);_tmpC6;});_tmpC5;});_tmpC4;});_LL6F: return Cyc_Absyn_uint_t;_LL71:
return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);
_LL73:({void*_tmpC7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::typ_to_c: type translation passed the heap region",sizeof(
unsigned char),55),_tag_arr(_tmpC7,sizeof(void*),0));});_LL75:({void*_tmpC8[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof(unsigned char),54),_tag_arr(_tmpC8,sizeof(void*),0));});_LL77:({void*
_tmpC9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::typ_to_c: type translation passed the join eff",sizeof(
unsigned char),52),_tag_arr(_tmpC9,sizeof(void*),0));});_LL79:({void*_tmpCA[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof(unsigned char),55),_tag_arr(_tmpCA,sizeof(void*),0));});_LL45:;}static
struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*l){void*_tmpCB=t;struct Cyc_Absyn_Tqual _tmpCC;void*
_tmpCD;_LLAD: if((unsigned int)_tmpCB > 3?*((int*)_tmpCB)== 7: 0){_LLB2: _tmpCD=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpCB)->f1;goto _LLB1;_LLB1: _tmpCC=((
struct Cyc_Absyn_ArrayType_struct*)_tmpCB)->f2;goto _LLAE;}else{goto _LLAF;}_LLAF:
goto _LLB0;_LLAE: return Cyc_Absyn_cast_exp(Cyc_Absyn_star_typ(_tmpCD,(void*)2,
_tmpCC),e,l);_LLB0: return Cyc_Absyn_cast_exp(t,e,l);_LLAC:;}static int Cyc_Toc_atomic_typ(
void*t){void*_tmpCE=Cyc_Tcutil_compress(t);void*_tmpCF;struct Cyc_Absyn_AggrInfo
_tmpD0;void*_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_Absyn_TunionFieldInfo
_tmpD3;void*_tmpD4;struct Cyc_Absyn_Tunionfield*_tmpD5;struct Cyc_Absyn_Tuniondecl*
_tmpD6;struct Cyc_List_List*_tmpD7;_LLB4: if((int)_tmpCE == 0){goto _LLB5;}else{goto
_LLB6;}_LLB6: if((unsigned int)_tmpCE > 3?*((int*)_tmpCE)== 1: 0){goto _LLB7;}else{
goto _LLB8;}_LLB8: if((unsigned int)_tmpCE > 3?*((int*)_tmpCE)== 5: 0){goto _LLB9;}
else{goto _LLBA;}_LLBA: if((unsigned int)_tmpCE > 3?*((int*)_tmpCE)== 12: 0){goto
_LLBB;}else{goto _LLBC;}_LLBC: if((unsigned int)_tmpCE > 3?*((int*)_tmpCE)== 13: 0){
goto _LLBD;}else{goto _LLBE;}_LLBE: if((int)_tmpCE == 1){goto _LLBF;}else{goto _LLC0;}
_LLC0: if((unsigned int)_tmpCE > 3?*((int*)_tmpCE)== 6: 0){goto _LLC1;}else{goto
_LLC2;}_LLC2: if((unsigned int)_tmpCE > 3?*((int*)_tmpCE)== 8: 0){goto _LLC3;}else{
goto _LLC4;}_LLC4: if((unsigned int)_tmpCE > 3?*((int*)_tmpCE)== 14: 0){goto _LLC5;}
else{goto _LLC6;}_LLC6: if((unsigned int)_tmpCE > 3?*((int*)_tmpCE)== 7: 0){_LLD8:
_tmpCF=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpCE)->f1;goto _LLC7;}else{
goto _LLC8;}_LLC8: if((unsigned int)_tmpCE > 3?*((int*)_tmpCE)== 10: 0){_LLD9: _tmpD0=((
struct Cyc_Absyn_AggrType_struct*)_tmpCE)->f1;_LLDA: _tmpD1=(void*)_tmpD0.aggr_info;
goto _LLC9;}else{goto _LLCA;}_LLCA: if((unsigned int)_tmpCE > 3?*((int*)_tmpCE)== 11:
0){_LLDB: _tmpD2=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpCE)->f2;goto _LLCB;}
else{goto _LLCC;}_LLCC: if((unsigned int)_tmpCE > 3?*((int*)_tmpCE)== 3: 0){_LLDC:
_tmpD3=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpCE)->f1;_LLDD: _tmpD4=(void*)
_tmpD3.field_info;if(*((int*)_tmpD4)== 1){_LLDF: _tmpD6=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpD4)->f1;goto _LLDE;_LLDE: _tmpD5=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpD4)->f2;goto _LLCD;}else{goto _LLCE;}}else{goto _LLCE;}_LLCE: if((unsigned int)
_tmpCE > 3?*((int*)_tmpCE)== 9: 0){_LLE0: _tmpD7=((struct Cyc_Absyn_TupleType_struct*)
_tmpCE)->f1;goto _LLCF;}else{goto _LLD0;}_LLD0: if((unsigned int)_tmpCE > 3?*((int*)
_tmpCE)== 2: 0){goto _LLD1;}else{goto _LLD2;}_LLD2: if((unsigned int)_tmpCE > 3?*((
int*)_tmpCE)== 4: 0){goto _LLD3;}else{goto _LLD4;}_LLD4: if((unsigned int)_tmpCE > 3?*((
int*)_tmpCE)== 15: 0){goto _LLD5;}else{goto _LLD6;}_LLD6: goto _LLD7;_LLB5: return 1;
_LLB7: return 0;_LLB9: goto _LLBB;_LLBB: goto _LLBD;_LLBD: goto _LLBF;_LLBF: goto _LLC1;
_LLC1: goto _LLC3;_LLC3: goto _LLC5;_LLC5: return 1;_LLC7: return Cyc_Toc_atomic_typ(
_tmpCF);_LLC9:{void*_tmpD8=_tmpD1;_LLE2: if(*((int*)_tmpD8)== 0){goto _LLE3;}else{
goto _LLE4;}_LLE4: goto _LLE5;_LLE3: return 0;_LLE5: goto _LLE1;_LLE1:;}{struct Cyc_Absyn_Aggrdecl*
_tmpD9=Cyc_Absyn_get_known_aggrdecl(_tmpD1);if(_tmpD9->fields == 0){return 0;}{
struct Cyc_List_List*_tmpDA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmpD9->fields))->v;for(0;_tmpDA != 0;_tmpDA=_tmpDA->tl){if(! Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpDA->hd)->type)){return 0;}}}return 1;}_LLCB:
for(0;_tmpD2 != 0;_tmpD2=_tmpD2->tl){if(! Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmpD2->hd)->type)){return 0;}}return 1;_LLCD: _tmpD7=_tmpD5->typs;goto _LLCF;_LLCF:
for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){if(! Cyc_Toc_atomic_typ((*((struct _tuple3*)
_tmpD7->hd)).f2)){return 0;}}return 1;_LLD1: goto _LLD3;_LLD3: goto _LLD5;_LLD5: return
0;_LLD7:({struct Cyc_Std_String_pa_struct _tmpDC;_tmpDC.tag=0;_tmpDC.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpDB[1]={& _tmpDC};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("atomic_typ:  bad type %s",
sizeof(unsigned char),25),_tag_arr(_tmpDB,sizeof(void*),1));}});_LLB3:;}static
int Cyc_Toc_is_void_star(void*t){void*_tmpDD=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpDE;void*_tmpDF;_LLE7: if((unsigned int)_tmpDD > 3?*((int*)_tmpDD)== 4: 0){_LLEB:
_tmpDE=((struct Cyc_Absyn_PointerType_struct*)_tmpDD)->f1;_LLEC: _tmpDF=(void*)
_tmpDE.elt_typ;goto _LLE8;}else{goto _LLE9;}_LLE9: goto _LLEA;_LLE8: {void*_tmpE0=
Cyc_Tcutil_compress(_tmpDF);_LLEE: if((int)_tmpE0 == 0){goto _LLEF;}else{goto _LLF0;}
_LLF0: goto _LLF1;_LLEF: return 1;_LLF1: return 0;_LLED:;}_LLEA: return 0;_LLE6:;}static
int Cyc_Toc_is_poly_field(void*t,struct _tagged_arr*f){void*_tmpE1=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmpE2;void*_tmpE3;struct Cyc_List_List*_tmpE4;_LLF3:
if((unsigned int)_tmpE1 > 3?*((int*)_tmpE1)== 10: 0){_LLF9: _tmpE2=((struct Cyc_Absyn_AggrType_struct*)
_tmpE1)->f1;_LLFA: _tmpE3=(void*)_tmpE2.aggr_info;goto _LLF4;}else{goto _LLF5;}
_LLF5: if((unsigned int)_tmpE1 > 3?*((int*)_tmpE1)== 11: 0){_LLFB: _tmpE4=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmpE1)->f2;goto _LLF6;}else{goto _LLF7;}_LLF7: goto
_LLF8;_LLF4: {struct Cyc_Absyn_Aggrdecl*_tmpE5=Cyc_Absyn_get_known_aggrdecl(
_tmpE3);if(_tmpE5->fields == 0){({void*_tmpE6[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: type missing fields",
sizeof(unsigned char),35),_tag_arr(_tmpE6,sizeof(void*),0));});}_tmpE4=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpE5->fields))->v;goto _LLF6;}
_LLF6: {struct Cyc_Absyn_Aggrfield*_tmpE7=Cyc_Absyn_lookup_field(_tmpE4,f);if(
_tmpE7 == 0){({struct Cyc_Std_String_pa_struct _tmpE9;_tmpE9.tag=0;_tmpE9.f1=(
struct _tagged_arr)*f;{void*_tmpE8[1]={& _tmpE9};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: bad field %s",
sizeof(unsigned char),28),_tag_arr(_tmpE8,sizeof(void*),1));}});}return Cyc_Toc_is_void_star((
void*)_tmpE7->type);}_LLF8:({struct Cyc_Std_String_pa_struct _tmpEB;_tmpEB.tag=0;
_tmpEB.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpEA[1]={& _tmpEB};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: bad type %s",
sizeof(unsigned char),27),_tag_arr(_tmpEA,sizeof(void*),1));}});_LLF2:;}static
int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*_tmpEC=(void*)e->r;struct
_tagged_arr*_tmpED;struct Cyc_Absyn_Exp*_tmpEE;struct _tagged_arr*_tmpEF;struct Cyc_Absyn_Exp*
_tmpF0;_LLFD: if(*((int*)_tmpEC)== 21){_LL104: _tmpEE=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpEC)->f1;goto _LL103;_LL103: _tmpED=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpEC)->f2;goto _LLFE;}else{goto _LLFF;}_LLFF: if(*((int*)_tmpEC)== 22){_LL106:
_tmpF0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpEC)->f1;goto _LL105;_LL105:
_tmpEF=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpEC)->f2;goto _LL100;}else{goto
_LL101;}_LL101: goto _LL102;_LLFE: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEE->topt))->v,_tmpED);_LL100: {void*_tmpF1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF0->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF2;void*_tmpF3;_LL108: if((unsigned int)_tmpF1 > 3?*((int*)_tmpF1)== 4: 0){
_LL10C: _tmpF2=((struct Cyc_Absyn_PointerType_struct*)_tmpF1)->f1;_LL10D: _tmpF3=(
void*)_tmpF2.elt_typ;goto _LL109;}else{goto _LL10A;}_LL10A: goto _LL10B;_LL109:
return Cyc_Toc_is_poly_field(_tmpF3,_tmpEF);_LL10B:({struct Cyc_Std_String_pa_struct
_tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF0->topt))->v);{void*_tmpF4[1]={& _tmpF5};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_project:  bad type %s",
sizeof(unsigned char),30),_tag_arr(_tmpF4,sizeof(void*),1));}});_LL107:;}_LL102:
return 0;_LLFC:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmpF6=
_cycalloc(sizeof(*_tmpF6));_tmpF6->hd=s;_tmpF6->tl=0;_tmpF6;}),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmpF7=_cycalloc(sizeof(*_tmpF7));
_tmpF7->hd=s;_tmpF7->tl=0;_tmpF7;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t)){return Cyc_Toc_malloc_atomic(
s);}return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({
struct Cyc_Absyn_Exp*_tmpF8[2];_tmpF8[1]=s;_tmpF8[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpF8,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmpF9[2];_tmpF9[1]=n;_tmpF9[0]=
s;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpF9,
sizeof(struct Cyc_Absyn_Exp*),2));}),0);}else{return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({
struct Cyc_Absyn_Exp*_tmpFA[2];_tmpFA[1]=n;_tmpFA[0]=s;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFA,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,
struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({
struct Cyc_Absyn_Exp*_tmpFB[3];_tmpFB[2]=n;_tmpFB[1]=s;_tmpFB[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFB,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return
Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmpFC=_cycalloc(
sizeof(*_tmpFC));_tmpFC->hd=e;_tmpFC->tl=0;_tmpFC;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*
e){int is_string=0;{void*_tmpFD=(void*)e->r;void*_tmpFE;_LL10F: if(*((int*)_tmpFD)
== 0){_LL113: _tmpFE=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpFD)->f1;if((
unsigned int)_tmpFE > 1?*((int*)_tmpFE)== 5: 0){goto _LL110;}else{goto _LL111;}}
else{goto _LL111;}_LL111: goto _LL112;_LL110: is_string=1;goto _LL10E;_LL112: goto
_LL10E;_LL10E:;}{struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*xplussz;if(
is_string){struct _tuple0*x=Cyc_Toc_temp_var();void*vd_typ=(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp103;_tmp103.tag=7;_tmp103.f1=(void*)Cyc_Absyn_uchar_t;_tmp103.f2=Cyc_Toc_mt_tq;
_tmp103.f3=(struct Cyc_Absyn_Exp*)sz;_tmp103;});_tmp102;});struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_static_vardecl(x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100[
0]=({struct Cyc_Absyn_Var_d_struct _tmp101;_tmp101.tag=0;_tmp101.f1=vd;_tmp101;});
_tmp100;}),0);_tmpFF->tl=Cyc_Toc_result_decls;_tmpFF;});xexp=Cyc_Absyn_var_exp(x,
0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
e,0);xplussz=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,
0),0);}{struct Cyc_Absyn_Exp*urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple4*_tmp104[3];_tmp104[2]=({struct _tuple4*_tmp107=_cycalloc(sizeof(*_tmp107));
_tmp107->f1=0;_tmp107->f2=xplussz;_tmp107;});_tmp104[1]=({struct _tuple4*_tmp106=
_cycalloc(sizeof(*_tmp106));_tmp106->f1=0;_tmp106->f2=xexp;_tmp106;});_tmp104[0]=({
struct _tuple4*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->f1=0;_tmp105->f2=xexp;
_tmp105;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp104,sizeof(struct _tuple4*),3));}),0);return urm_exp;}}}struct Cyc_Toc_Env{
struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;struct Cyc_Core_Opt*
fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp108=_cycalloc(sizeof(*_tmp108));
_tmp108->break_lab=0;_tmp108->continue_lab=0;_tmp108->fallthru_info=0;_tmp108->varmap=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp108->toplevel=1;_tmp108;});}static struct Cyc_Toc_Env*Cyc_Toc_copy_env(
struct Cyc_Toc_Env*e){return({struct Cyc_Toc_Env*_tmp109=_cycalloc(sizeof(*_tmp109));
_tmp109->break_lab=e->break_lab;_tmp109->continue_lab=e->continue_lab;_tmp109->fallthru_info=
e->fallthru_info;_tmp109->varmap=e->varmap;_tmp109->toplevel=e->toplevel;_tmp109;});}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*
ans=Cyc_Toc_copy_env(e);ans->toplevel=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{void*_tmp10A=(*x).f1;
_LL115: if((unsigned int)_tmp10A > 1?*((int*)_tmp10A)== 0: 0){goto _LL116;}else{goto
_LL117;}_LL117: goto _LL118;_LL116:({struct Cyc_Std_String_pa_struct _tmp10C;_tmp10C.tag=
0;_tmp10C.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(x);{void*_tmp10B[1]={&
_tmp10C};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::add_varmap on Rel_n: %s\n",sizeof(unsigned char),30),_tag_arr(
_tmp10B,sizeof(void*),1));}});_LL118: goto _LL114;_LL114:;}{struct Cyc_Toc_Env*ans=
Cyc_Toc_copy_env(e);ans->varmap=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(ans->varmap,x,y);return
ans;}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*
ans=Cyc_Toc_copy_env(e);ans->break_lab=0;ans->continue_lab=0;return ans;}struct
_tuple6{struct _tagged_arr*f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*f3;};
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct Cyc_Toc_Env*e,
struct _tagged_arr*break_l,struct _tagged_arr*fallthru_l,struct Cyc_List_List*
fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*_tmp10D=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){_tmp10D=({
struct Cyc_List_List*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp10E->tl=_tmp10D;_tmp10E;});}_tmp10D=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp10D);{struct Cyc_Toc_Env*ans=Cyc_Toc_copy_env(
e);ans->break_lab=({struct Cyc_Core_Opt*_tmp10F=_cycalloc(sizeof(*_tmp10F));
_tmp10F->v=break_l;_tmp10F;});ans->fallthru_info=({struct Cyc_Core_Opt*_tmp110=
_cycalloc(sizeof(*_tmp110));_tmp110->v=({struct _tuple6*_tmp111=_cycalloc(sizeof(*
_tmp111));_tmp111->f1=fallthru_l;_tmp111->f2=_tmp10D;_tmp111->f3=next_case_env->varmap;
_tmp111;});_tmp110;});return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*break_l){struct Cyc_Toc_Env*ans=Cyc_Toc_copy_env(
e);ans->break_lab=({struct Cyc_Core_Opt*_tmp112=_cycalloc(sizeof(*_tmp112));
_tmp112->v=break_l;_tmp112;});ans->fallthru_info=0;return ans;}static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env(struct Cyc_Toc_Env*e,struct _tagged_arr*next_l){
struct Cyc_Toc_Env*ans=Cyc_Toc_copy_env(e);ans->break_lab=0;ans->fallthru_info=({
struct Cyc_Core_Opt*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->v=({struct
_tuple6*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->f1=next_l;_tmp114->f2=0;
_tmp114->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))
Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp114;});_tmp113;});return ans;}static void
Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp115=(void*)e->annot;_LL11A: if(*((void**)_tmp115)
== Cyc_CfFlowInfo_UnknownZ){goto _LL11B;}else{goto _LL11C;}_LL11C: if(*((void**)
_tmp115)== Cyc_CfFlowInfo_NotZero){goto _LL11D;}else{goto _LL11E;}_LL11E: if(*((
void**)_tmp115)== Cyc_CfFlowInfo_IsZero){goto _LL11F;}else{goto _LL120;}_LL120: if(
_tmp115 == Cyc_Absyn_EmptyAnnot){goto _LL121;}else{goto _LL122;}_LL122: goto _LL123;
_LL11B: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL11D: return 0;_LL11F:({void*_tmp116[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(unsigned char),28),_tag_arr(_tmp116,sizeof(void*),0));});return 0;_LL121:({
void*_tmp117[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("!",sizeof(
unsigned char),2),_tag_arr(_tmp117,sizeof(void*),0));});({void*_tmp118[0]={};Cyc_Tcutil_warn(
e->loc,_tag_arr("unreachable dereference",sizeof(unsigned char),24),_tag_arr(
_tmp118,sizeof(void*),0));});return 0;_LL123:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Core_Impossible_struct
_tmp11A;_tmp11A.tag=Cyc_Core_Impossible;_tmp11A.f1=_tag_arr("need_null_check",
sizeof(unsigned char),16);_tmp11A;});_tmp119;}));_LL119:;}static struct Cyc_List_List*
Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp11B=(void*)e->annot;struct Cyc_List_List*
_tmp11C;struct Cyc_List_List*_tmp11D;_LL125: if(*((void**)_tmp11B)== Cyc_CfFlowInfo_UnknownZ){
_LL12F: _tmp11C=((struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp11B)->f1;goto _LL126;}
else{goto _LL127;}_LL127: if(*((void**)_tmp11B)== Cyc_CfFlowInfo_NotZero){_LL130:
_tmp11D=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmp11B)->f1;goto _LL128;}else{
goto _LL129;}_LL129: if(*((void**)_tmp11B)== Cyc_CfFlowInfo_IsZero){goto _LL12A;}
else{goto _LL12B;}_LL12B: if(_tmp11B == Cyc_Absyn_EmptyAnnot){goto _LL12C;}else{goto
_LL12D;}_LL12D: goto _LL12E;_LL126: return _tmp11C;_LL128: return _tmp11D;_LL12A:({
void*_tmp11E[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(unsigned char),28),_tag_arr(_tmp11E,sizeof(void*),0));});return 0;_LL12C:
return 0;_LL12E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp11F=
_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=({struct Cyc_Core_Impossible_struct _tmp120;
_tmp120.tag=Cyc_Core_Impossible;_tmp120.f1=_tag_arr("get_relns",sizeof(
unsigned char),10);_tmp120;});_tmp11F;}));_LL124:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp121=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp122;struct Cyc_Absyn_Conref*_tmp123;struct Cyc_Absyn_Exp*_tmp124;_LL132: if((
unsigned int)_tmp121 > 3?*((int*)_tmp121)== 4: 0){_LL138: _tmp122=((struct Cyc_Absyn_PointerType_struct*)
_tmp121)->f1;_LL139: _tmp123=_tmp122.bounds;goto _LL133;}else{goto _LL134;}_LL134:
if((unsigned int)_tmp121 > 3?*((int*)_tmp121)== 7: 0){_LL13A: _tmp124=((struct Cyc_Absyn_ArrayType_struct*)
_tmp121)->f3;goto _LL135;}else{goto _LL136;}_LL136: goto _LL137;_LL133: {void*
_tmp125=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp123);struct Cyc_Absyn_Exp*
_tmp126;_LL13C: if((int)_tmp125 == 0){goto _LL13D;}else{goto _LL13E;}_LL13E: if((
unsigned int)_tmp125 > 1?*((int*)_tmp125)== 0: 0){_LL140: _tmp126=((struct Cyc_Absyn_Upper_b_struct*)
_tmp125)->f1;goto _LL13F;}else{goto _LL13B;}_LL13D: return 0;_LL13F: {unsigned int j=
Cyc_Evexp_eval_const_uint_exp(_tmp126);return i <= j;}_LL13B:;}_LL135: if(_tmp124 == 
0){return 0;}{unsigned int j=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp124));return i <= j;}_LL137: return 0;_LL131:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp127=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp127->vd != y){continue;}{void*_tmp128=(void*)_tmp127->rop;struct
Cyc_Absyn_Vardecl*_tmp129;struct Cyc_Absyn_Vardecl*_tmp12A;_LL142: if(*((int*)
_tmp128)== 2){_LL148: _tmp129=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp128)->f1;
goto _LL143;}else{goto _LL144;}_LL144: if(*((int*)_tmp128)== 4){_LL149: _tmp12A=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp128)->f1;goto _LL145;}else{goto _LL146;}
_LL146: goto _LL147;_LL143: _tmp12A=_tmp129;goto _LL145;_LL145: if(_tmp12A == v){
return 1;}else{goto _LL141;}_LL147: continue;_LL141:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp12B=(void*)e->r;void*_tmp12C;struct Cyc_Absyn_Vardecl*_tmp12D;void*_tmp12E;
struct Cyc_Absyn_Vardecl*_tmp12F;void*_tmp130;struct Cyc_Absyn_Vardecl*_tmp131;
void*_tmp132;struct Cyc_Absyn_Vardecl*_tmp133;struct Cyc_List_List*_tmp134;struct
Cyc_List_List _tmp135;struct Cyc_Absyn_Exp*_tmp136;void*_tmp137;_LL14B: if(*((int*)
_tmp12B)== 1){_LL157: _tmp12C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12B)->f2;
if((unsigned int)_tmp12C > 1?*((int*)_tmp12C)== 4: 0){_LL158: _tmp12D=((struct Cyc_Absyn_Pat_b_struct*)
_tmp12C)->f1;goto _LL14C;}else{goto _LL14D;}}else{goto _LL14D;}_LL14D: if(*((int*)
_tmp12B)== 1){_LL159: _tmp12E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12B)->f2;
if((unsigned int)_tmp12E > 1?*((int*)_tmp12E)== 3: 0){_LL15A: _tmp12F=((struct Cyc_Absyn_Local_b_struct*)
_tmp12E)->f1;goto _LL14E;}else{goto _LL14F;}}else{goto _LL14F;}_LL14F: if(*((int*)
_tmp12B)== 1){_LL15B: _tmp130=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12B)->f2;
if((unsigned int)_tmp130 > 1?*((int*)_tmp130)== 0: 0){_LL15C: _tmp131=((struct Cyc_Absyn_Global_b_struct*)
_tmp130)->f1;goto _LL150;}else{goto _LL151;}}else{goto _LL151;}_LL151: if(*((int*)
_tmp12B)== 1){_LL15D: _tmp132=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12B)->f2;
if((unsigned int)_tmp132 > 1?*((int*)_tmp132)== 2: 0){_LL15E: _tmp133=((struct Cyc_Absyn_Param_b_struct*)
_tmp132)->f1;goto _LL152;}else{goto _LL153;}}else{goto _LL153;}_LL153: if(*((int*)
_tmp12B)== 3){_LL161: _tmp137=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp12B)->f1;
if((int)_tmp137 == 19){goto _LL15F;}else{goto _LL155;}_LL15F: _tmp134=((struct Cyc_Absyn_Primop_e_struct*)
_tmp12B)->f2;if(_tmp134 == 0){goto _LL155;}else{_tmp135=*_tmp134;_LL160: _tmp136=(
struct Cyc_Absyn_Exp*)_tmp135.hd;goto _LL154;}}else{goto _LL155;}_LL155: goto _LL156;
_LL14C: _tmp12F=_tmp12D;goto _LL14E;_LL14E: _tmp131=_tmp12F;goto _LL150;_LL150:
_tmp133=_tmp131;goto _LL152;_LL152: if(_tmp133->escapes){return 0;}if(Cyc_Toc_check_leq_size_var(
relns,v,_tmp133)){return 1;}goto _LL14A;_LL154:{void*_tmp138=(void*)_tmp136->r;
void*_tmp139;struct Cyc_Absyn_Vardecl*_tmp13A;void*_tmp13B;struct Cyc_Absyn_Vardecl*
_tmp13C;void*_tmp13D;struct Cyc_Absyn_Vardecl*_tmp13E;void*_tmp13F;struct Cyc_Absyn_Vardecl*
_tmp140;_LL163: if(*((int*)_tmp138)== 1){_LL16D: _tmp139=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp138)->f2;if((unsigned int)_tmp139 > 1?*((int*)_tmp139)== 4: 0){_LL16E: _tmp13A=((
struct Cyc_Absyn_Pat_b_struct*)_tmp139)->f1;goto _LL164;}else{goto _LL165;}}else{
goto _LL165;}_LL165: if(*((int*)_tmp138)== 1){_LL16F: _tmp13B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp138)->f2;if((unsigned int)_tmp13B > 1?*((int*)_tmp13B)== 3: 0){_LL170: _tmp13C=((
struct Cyc_Absyn_Local_b_struct*)_tmp13B)->f1;goto _LL166;}else{goto _LL167;}}else{
goto _LL167;}_LL167: if(*((int*)_tmp138)== 1){_LL171: _tmp13D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp138)->f2;if((unsigned int)_tmp13D > 1?*((int*)_tmp13D)== 0: 0){_LL172: _tmp13E=((
struct Cyc_Absyn_Global_b_struct*)_tmp13D)->f1;goto _LL168;}else{goto _LL169;}}
else{goto _LL169;}_LL169: if(*((int*)_tmp138)== 1){_LL173: _tmp13F=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp138)->f2;if((unsigned int)_tmp13F > 1?*((int*)_tmp13F)
== 2: 0){_LL174: _tmp140=((struct Cyc_Absyn_Param_b_struct*)_tmp13F)->f1;goto _LL16A;}
else{goto _LL16B;}}else{goto _LL16B;}_LL16B: goto _LL16C;_LL164: _tmp13C=_tmp13A;goto
_LL166;_LL166: _tmp13E=_tmp13C;goto _LL168;_LL168: _tmp140=_tmp13E;goto _LL16A;
_LL16A: return _tmp140 == v;_LL16C: goto _LL162;_LL162:;}goto _LL14A;_LL156: goto _LL14A;
_LL14A:;}return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct
Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp141=(void*)a->r;void*_tmp142;
struct Cyc_Absyn_Vardecl*_tmp143;void*_tmp144;struct Cyc_Absyn_Vardecl*_tmp145;
void*_tmp146;struct Cyc_Absyn_Vardecl*_tmp147;void*_tmp148;struct Cyc_Absyn_Vardecl*
_tmp149;_LL176: if(*((int*)_tmp141)== 1){_LL180: _tmp142=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp141)->f2;if((unsigned int)_tmp142 > 1?*((int*)_tmp142)== 4: 0){_LL181: _tmp143=((
struct Cyc_Absyn_Pat_b_struct*)_tmp142)->f1;goto _LL177;}else{goto _LL178;}}else{
goto _LL178;}_LL178: if(*((int*)_tmp141)== 1){_LL182: _tmp144=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp141)->f2;if((unsigned int)_tmp144 > 1?*((int*)_tmp144)== 3: 0){_LL183: _tmp145=((
struct Cyc_Absyn_Local_b_struct*)_tmp144)->f1;goto _LL179;}else{goto _LL17A;}}else{
goto _LL17A;}_LL17A: if(*((int*)_tmp141)== 1){_LL184: _tmp146=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp141)->f2;if((unsigned int)_tmp146 > 1?*((int*)_tmp146)== 0: 0){_LL185: _tmp147=((
struct Cyc_Absyn_Global_b_struct*)_tmp146)->f1;goto _LL17B;}else{goto _LL17C;}}
else{goto _LL17C;}_LL17C: if(*((int*)_tmp141)== 1){_LL186: _tmp148=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp141)->f2;if((unsigned int)_tmp148 > 1?*((int*)_tmp148)
== 2: 0){_LL187: _tmp149=((struct Cyc_Absyn_Param_b_struct*)_tmp148)->f1;goto _LL17D;}
else{goto _LL17E;}}else{goto _LL17E;}_LL17E: goto _LL17F;_LL177: _tmp145=_tmp143;goto
_LL179;_LL179: _tmp147=_tmp145;goto _LL17B;_LL17B: _tmp149=_tmp147;goto _LL17D;
_LL17D: if(_tmp149->escapes){return 0;}inner_loop: {void*_tmp14A=(void*)i->r;
struct Cyc_Absyn_Exp*_tmp14B;void*_tmp14C;void*_tmp14D;int _tmp14E;void*_tmp14F;
void*_tmp150;int _tmp151;void*_tmp152;struct Cyc_List_List*_tmp153;struct Cyc_List_List
_tmp154;struct Cyc_List_List*_tmp155;struct Cyc_List_List _tmp156;struct Cyc_Absyn_Exp*
_tmp157;struct Cyc_Absyn_Exp*_tmp158;void*_tmp159;void*_tmp15A;struct Cyc_Absyn_Vardecl*
_tmp15B;void*_tmp15C;struct Cyc_Absyn_Vardecl*_tmp15D;void*_tmp15E;struct Cyc_Absyn_Vardecl*
_tmp15F;void*_tmp160;struct Cyc_Absyn_Vardecl*_tmp161;_LL189: if(*((int*)_tmp14A)
== 13){_LL19C: _tmp14C=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp14A)->f1;goto
_LL19B;_LL19B: _tmp14B=((struct Cyc_Absyn_Cast_e_struct*)_tmp14A)->f2;goto _LL18A;}
else{goto _LL18B;}_LL18B: if(*((int*)_tmp14A)== 0){_LL19D: _tmp14D=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmp14A)->f1;if((unsigned int)_tmp14D > 1?*((int*)
_tmp14D)== 2: 0){_LL19F: _tmp14F=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp14D)->f1;
if((int)_tmp14F == 0){goto _LL19E;}else{goto _LL18D;}_LL19E: _tmp14E=((struct Cyc_Absyn_Int_c_struct*)
_tmp14D)->f2;goto _LL18C;}else{goto _LL18D;}}else{goto _LL18D;}_LL18D: if(*((int*)
_tmp14A)== 0){_LL1A0: _tmp150=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp14A)->f1;
if((unsigned int)_tmp150 > 1?*((int*)_tmp150)== 2: 0){_LL1A2: _tmp152=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp150)->f1;if((int)_tmp152 == 1){goto _LL1A1;}
else{goto _LL18F;}_LL1A1: _tmp151=((struct Cyc_Absyn_Int_c_struct*)_tmp150)->f2;
goto _LL18E;}else{goto _LL18F;}}else{goto _LL18F;}_LL18F: if(*((int*)_tmp14A)== 3){
_LL1A7: _tmp159=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp14A)->f1;if((int)
_tmp159 == 4){goto _LL1A3;}else{goto _LL191;}_LL1A3: _tmp153=((struct Cyc_Absyn_Primop_e_struct*)
_tmp14A)->f2;if(_tmp153 == 0){goto _LL191;}else{_tmp154=*_tmp153;_LL1A6: _tmp158=(
struct Cyc_Absyn_Exp*)_tmp154.hd;goto _LL1A4;_LL1A4: _tmp155=_tmp154.tl;if(_tmp155
== 0){goto _LL191;}else{_tmp156=*_tmp155;_LL1A5: _tmp157=(struct Cyc_Absyn_Exp*)
_tmp156.hd;goto _LL190;}}}else{goto _LL191;}_LL191: if(*((int*)_tmp14A)== 1){_LL1A8:
_tmp15A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp14A)->f2;if((unsigned int)
_tmp15A > 1?*((int*)_tmp15A)== 4: 0){_LL1A9: _tmp15B=((struct Cyc_Absyn_Pat_b_struct*)
_tmp15A)->f1;goto _LL192;}else{goto _LL193;}}else{goto _LL193;}_LL193: if(*((int*)
_tmp14A)== 1){_LL1AA: _tmp15C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp14A)->f2;
if((unsigned int)_tmp15C > 1?*((int*)_tmp15C)== 3: 0){_LL1AB: _tmp15D=((struct Cyc_Absyn_Local_b_struct*)
_tmp15C)->f1;goto _LL194;}else{goto _LL195;}}else{goto _LL195;}_LL195: if(*((int*)
_tmp14A)== 1){_LL1AC: _tmp15E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp14A)->f2;
if((unsigned int)_tmp15E > 1?*((int*)_tmp15E)== 0: 0){_LL1AD: _tmp15F=((struct Cyc_Absyn_Global_b_struct*)
_tmp15E)->f1;goto _LL196;}else{goto _LL197;}}else{goto _LL197;}_LL197: if(*((int*)
_tmp14A)== 1){_LL1AE: _tmp160=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp14A)->f2;
if((unsigned int)_tmp160 > 1?*((int*)_tmp160)== 2: 0){_LL1AF: _tmp161=((struct Cyc_Absyn_Param_b_struct*)
_tmp160)->f1;goto _LL198;}else{goto _LL199;}}else{goto _LL199;}_LL199: goto _LL19A;
_LL18A: i=_tmp14B;goto inner_loop;_LL18C: return _tmp14E >= 0? Cyc_Toc_check_const_array((
unsigned int)(_tmp14E + 1),(void*)_tmp149->type): 0;_LL18E: return Cyc_Toc_check_const_array((
unsigned int)(_tmp151 + 1),(void*)_tmp149->type);_LL190: return Cyc_Toc_check_leq_size(
relns,_tmp149,_tmp157);_LL192: _tmp15D=_tmp15B;goto _LL194;_LL194: _tmp15F=_tmp15D;
goto _LL196;_LL196: _tmp161=_tmp15F;goto _LL198;_LL198: if(_tmp161->escapes){return 0;}{
struct Cyc_List_List*_tmp162=relns;for(0;_tmp162 != 0;_tmp162=_tmp162->tl){struct
Cyc_CfFlowInfo_Reln*_tmp163=(struct Cyc_CfFlowInfo_Reln*)_tmp162->hd;if(_tmp163->vd
== _tmp161){void*_tmp164=(void*)_tmp163->rop;struct Cyc_Absyn_Vardecl*_tmp165;
struct Cyc_Absyn_Vardecl*_tmp166;unsigned int _tmp167;_LL1B1: if(*((int*)_tmp164)== 
2){_LL1B9: _tmp165=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp164)->f1;goto
_LL1B2;}else{goto _LL1B3;}_LL1B3: if(*((int*)_tmp164)== 1){_LL1BA: _tmp166=((struct
Cyc_CfFlowInfo_LessVar_struct*)_tmp164)->f1;goto _LL1B4;}else{goto _LL1B5;}_LL1B5:
if(*((int*)_tmp164)== 3){_LL1BB: _tmp167=((struct Cyc_CfFlowInfo_LessConst_struct*)
_tmp164)->f1;goto _LL1B6;}else{goto _LL1B7;}_LL1B7: goto _LL1B8;_LL1B2: if(_tmp149 == 
_tmp165){return 1;}else{goto _LL1B0;}_LL1B4:{struct Cyc_List_List*_tmp168=relns;
for(0;_tmp168 != 0;_tmp168=_tmp168->tl){struct Cyc_CfFlowInfo_Reln*_tmp169=(struct
Cyc_CfFlowInfo_Reln*)_tmp168->hd;if(_tmp169->vd == _tmp166){void*_tmp16A=(void*)
_tmp169->rop;struct Cyc_Absyn_Vardecl*_tmp16B;struct Cyc_Absyn_Vardecl*_tmp16C;
unsigned int _tmp16D;struct Cyc_Absyn_Vardecl*_tmp16E;_LL1BD: if(*((int*)_tmp16A)== 
4){_LL1C7: _tmp16B=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp16A)->f1;goto
_LL1BE;}else{goto _LL1BF;}_LL1BF: if(*((int*)_tmp16A)== 2){_LL1C8: _tmp16C=((struct
Cyc_CfFlowInfo_LessSize_struct*)_tmp16A)->f1;goto _LL1C0;}else{goto _LL1C1;}_LL1C1:
if(*((int*)_tmp16A)== 0){_LL1C9: _tmp16D=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp16A)->f1;goto _LL1C2;}else{goto _LL1C3;}_LL1C3: if(*((int*)_tmp16A)== 1){_LL1CA:
_tmp16E=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp16A)->f1;goto _LL1C4;}else{
goto _LL1C5;}_LL1C5: goto _LL1C6;_LL1BE: _tmp16C=_tmp16B;goto _LL1C0;_LL1C0: if(
_tmp149 == _tmp16C){return 1;}else{goto _LL1BC;}_LL1C2: return Cyc_Toc_check_const_array(
_tmp16D,(void*)_tmp149->type);_LL1C4: if(Cyc_Toc_check_leq_size_var(relns,_tmp149,
_tmp16E)){return 1;}else{goto _LL1BC;}_LL1C6: goto _LL1BC;_LL1BC:;}}}goto _LL1B0;
_LL1B6: return Cyc_Toc_check_const_array(_tmp167,(void*)_tmp149->type);_LL1B8: goto
_LL1B0;_LL1B0:;}}}goto _LL188;_LL19A: goto _LL188;_LL188:;}goto _LL175;_LL17F: goto
_LL175;_LL175:;}return 0;}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){if(
e->topt == 0){({void*_tmp16F[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",sizeof(
unsigned char),24),_tag_arr(_tmp16F,sizeof(void*),0));});}return Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){({void*_tmp170[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",
sizeof(unsigned char),24),_tag_arr(_tmp170,sizeof(void*),0));});}return(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple3*Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp*e){return({struct _tuple3*_tmp171=_cycalloc(sizeof(*_tmp171));
_tmp171->f1=Cyc_Toc_mt_tq;_tmp171->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp171;});}static struct _tuple4*Cyc_Toc_add_designator(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);return({
struct _tuple4*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->f1=0;_tmp172->f2=e;
_tmp172;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,
struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*
rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp173=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0){eo=(struct Cyc_Absyn_Exp*)(
is_atomic? Cyc_Toc_malloc_atomic(_tmp173): Cyc_Toc_malloc_ptr(_tmp173));}else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(rgnopt);Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp173);}}}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int e1_already_translated);static struct
Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp174=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp174 != 0;_tmp174=_tmp174->tl){struct _tuple4 _tmp176;struct Cyc_Absyn_Exp*
_tmp177;struct Cyc_List_List*_tmp178;struct _tuple4*_tmp175=(struct _tuple4*)
_tmp174->hd;_tmp176=*_tmp175;_LL1CD: _tmp178=_tmp176.f1;goto _LL1CC;_LL1CC: _tmp177=
_tmp176.f2;goto _LL1CB;_LL1CB: {struct Cyc_Absyn_Exp*e_index;if(_tmp178 == 0){
e_index=Cyc_Absyn_signed_int_exp(count --,0);}else{if(_tmp178->tl != 0){({void*
_tmp179[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("multiple designators in array",sizeof(unsigned char),30),_tag_arr(
_tmp179,sizeof(void*),0));});}{void*_tmp17A=(void*)_tmp178->hd;void*_tmp17B=
_tmp17A;struct Cyc_Absyn_Exp*_tmp17C;_LL1CF: if(*((int*)_tmp17B)== 0){_LL1D3:
_tmp17C=((struct Cyc_Absyn_ArrayElement_struct*)_tmp17B)->f1;goto _LL1D0;}else{
goto _LL1D1;}_LL1D1: if(*((int*)_tmp17B)== 1){goto _LL1D2;}else{goto _LL1CE;}_LL1D0:
Cyc_Toc_exp_to_c(nv,_tmp17C);e_index=_tmp17C;goto _LL1CE;_LL1D2:({void*_tmp17D[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("field name designators in array",
sizeof(unsigned char),32),_tag_arr(_tmp17D,sizeof(void*),0));});_LL1CE:;}}{
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp17E=(
void*)_tmp177->r;struct Cyc_List_List*_tmp17F;struct Cyc_Absyn_Exp*_tmp180;struct
Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Vardecl*_tmp182;struct Cyc_List_List*
_tmp183;void*_tmp184;_LL1D5: if(*((int*)_tmp17E)== 26){_LL1DD: _tmp17F=((struct Cyc_Absyn_Array_e_struct*)
_tmp17E)->f1;goto _LL1D6;}else{goto _LL1D7;}_LL1D7: if(*((int*)_tmp17E)== 27){
_LL1E0: _tmp182=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp17E)->f1;goto _LL1DF;
_LL1DF: _tmp181=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp17E)->f2;goto _LL1DE;
_LL1DE: _tmp180=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp17E)->f3;goto _LL1D8;}
else{goto _LL1D9;}_LL1D9: if(*((int*)_tmp17E)== 29){_LL1E2: _tmp184=(void*)((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp17E)->f1;goto _LL1E1;_LL1E1: _tmp183=((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp17E)->f2;goto _LL1DA;}else{goto _LL1DB;}_LL1DB:
goto _LL1DC;_LL1D6: s=Cyc_Toc_init_array(nv,lval,_tmp17F,s);goto _LL1D4;_LL1D8: s=
Cyc_Toc_init_comprehension(nv,lval,_tmp182,_tmp181,_tmp180,s,0);goto _LL1D4;
_LL1DA: s=Cyc_Toc_init_anon_struct(nv,lval,_tmp184,_tmp183,s);goto _LL1D4;_LL1DC:
Cyc_Toc_exp_to_c(nv,_tmp177);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp177,0),s,0);goto _LL1D4;_LL1D4:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
s,int e1_already_translated){struct _tuple0*_tmp185=vd->name;if(!
e1_already_translated){Cyc_Toc_exp_to_c(nv,e1);}{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
nv,_tmp185,Cyc_Absyn_varb_exp(_tmp185,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D[0]=({struct Cyc_Absyn_Local_b_struct
_tmp18E;_tmp18E.tag=3;_tmp18E.f1=vd;_tmp18E;});_tmp18D;}),0));struct _tuple0*max=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(
_tmp185,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp(_tmp185,0),Cyc_Absyn_var_exp(max,0),0);struct Cyc_Absyn_Exp*ec=
Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp185,0),0);struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp185,0),0);struct Cyc_Absyn_Stmt*
body;{void*_tmp186=(void*)e2->r;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_Exp*
_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Vardecl*_tmp18A;struct Cyc_List_List*
_tmp18B;void*_tmp18C;_LL1E4: if(*((int*)_tmp186)== 26){_LL1EC: _tmp187=((struct Cyc_Absyn_Array_e_struct*)
_tmp186)->f1;goto _LL1E5;}else{goto _LL1E6;}_LL1E6: if(*((int*)_tmp186)== 27){
_LL1EF: _tmp18A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp186)->f1;goto _LL1EE;
_LL1EE: _tmp189=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp186)->f2;goto _LL1ED;
_LL1ED: _tmp188=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp186)->f3;goto _LL1E7;}
else{goto _LL1E8;}_LL1E8: if(*((int*)_tmp186)== 29){_LL1F1: _tmp18C=(void*)((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp186)->f1;goto _LL1F0;_LL1F0: _tmp18B=((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp186)->f2;goto _LL1E9;}else{goto _LL1EA;}_LL1EA:
goto _LL1EB;_LL1E5: body=Cyc_Toc_init_array(nv2,lval,_tmp187,Cyc_Absyn_skip_stmt(0));
goto _LL1E3;_LL1E7: body=Cyc_Toc_init_comprehension(nv2,lval,_tmp18A,_tmp189,
_tmp188,Cyc_Absyn_skip_stmt(0),0);goto _LL1E3;_LL1E9: body=Cyc_Toc_init_anon_struct(
nv,lval,_tmp18C,_tmp18B,Cyc_Absyn_skip_stmt(0));goto _LL1E3;_LL1EB: Cyc_Toc_exp_to_c(
nv2,e2);body=Cyc_Absyn_assign_stmt(lval,e2,0);goto _LL1E3;_LL1E3:;}{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(
max,Cyc_Absyn_uint_t,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp185,Cyc_Absyn_uint_t,
0,s2,0),0),s,0);return s;}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s){{struct Cyc_List_List*_tmp18F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp18F != 0;_tmp18F=_tmp18F->tl){
struct _tuple4 _tmp191;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_List_List*_tmp193;
struct _tuple4*_tmp190=(struct _tuple4*)_tmp18F->hd;_tmp191=*_tmp190;_LL1F4:
_tmp193=_tmp191.f1;goto _LL1F3;_LL1F3: _tmp192=_tmp191.f2;goto _LL1F2;_LL1F2: if(
_tmp193 == 0){({void*_tmp194[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",sizeof(
unsigned char),22),_tag_arr(_tmp194,sizeof(void*),0));});}if(_tmp193->tl != 0){({
void*_tmp195[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("too many designators in anonymous struct",sizeof(unsigned char),41),
_tag_arr(_tmp195,sizeof(void*),0));});}{void*_tmp196=(void*)_tmp193->hd;struct
_tagged_arr*_tmp197;_LL1F6: if(*((int*)_tmp196)== 1){_LL1FA: _tmp197=((struct Cyc_Absyn_FieldName_struct*)
_tmp196)->f1;goto _LL1F7;}else{goto _LL1F8;}_LL1F8: goto _LL1F9;_LL1F7: {struct Cyc_Absyn_Exp*
lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp197,0);{void*_tmp198=(void*)_tmp192->r;
struct Cyc_List_List*_tmp199;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*
_tmp19B;struct Cyc_Absyn_Vardecl*_tmp19C;struct Cyc_List_List*_tmp19D;void*_tmp19E;
_LL1FC: if(*((int*)_tmp198)== 26){_LL204: _tmp199=((struct Cyc_Absyn_Array_e_struct*)
_tmp198)->f1;goto _LL1FD;}else{goto _LL1FE;}_LL1FE: if(*((int*)_tmp198)== 27){
_LL207: _tmp19C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp198)->f1;goto _LL206;
_LL206: _tmp19B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp198)->f2;goto _LL205;
_LL205: _tmp19A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp198)->f3;goto _LL1FF;}
else{goto _LL200;}_LL200: if(*((int*)_tmp198)== 29){_LL209: _tmp19E=(void*)((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp198)->f1;goto _LL208;_LL208: _tmp19D=((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp198)->f2;goto _LL201;}else{goto _LL202;}_LL202:
goto _LL203;_LL1FD: s=Cyc_Toc_init_array(nv,lval,_tmp199,s);goto _LL1FB;_LL1FF: s=
Cyc_Toc_init_comprehension(nv,lval,_tmp19C,_tmp19B,_tmp19A,s,0);goto _LL1FB;
_LL201: s=Cyc_Toc_init_anon_struct(nv,lval,_tmp19E,_tmp19D,s);goto _LL1FB;_LL203:
Cyc_Toc_exp_to_c(nv,_tmp192);if(Cyc_Toc_is_poly_field(struct_type,_tmp197)){
_tmp192=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp192,0);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp192,0),0),s,0);goto _LL1FB;
_LL1FB:;}goto _LL1F5;}_LL1F9:({void*_tmp19F[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(unsigned char),27),_tag_arr(_tmp19F,sizeof(void*),0));});_LL1F5:;}}}
return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int
pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct _RegionHandle
_tmp1A0=_new_region("r");struct _RegionHandle*r=& _tmp1A0;_push_region(r);{struct
Cyc_List_List*_tmp1A1=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple3*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,
es);struct _tagged_arr*_tmp1A2=Cyc_Toc_add_tuple_type(_tmp1A1);struct _tuple0*
_tmp1A3=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1A4=Cyc_Absyn_var_exp(_tmp1A3,
0);struct Cyc_Absyn_Stmt*_tmp1A5=Cyc_Absyn_exp_stmt(_tmp1A4,0);struct Cyc_Absyn_Exp*(*
_tmp1A6)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct
Cyc_List_List*_tmp1A7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp1A7);for(0;_tmp1A7 != 0;(_tmp1A7=_tmp1A7->tl,-- i)){struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)_tmp1A7->hd;struct Cyc_Absyn_Exp*lval=_tmp1A6(_tmp1A4,Cyc_Absyn_fieldname(
i),0);is_atomic=is_atomic? Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v): 0;{void*_tmp1A8=(void*)e->r;struct Cyc_List_List*
_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Vardecl*
_tmp1AC;_LL20B: if(*((int*)_tmp1A8)== 26){_LL211: _tmp1A9=((struct Cyc_Absyn_Array_e_struct*)
_tmp1A8)->f1;goto _LL20C;}else{goto _LL20D;}_LL20D: if(*((int*)_tmp1A8)== 27){
_LL214: _tmp1AC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A8)->f1;goto _LL213;
_LL213: _tmp1AB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A8)->f2;goto _LL212;
_LL212: _tmp1AA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A8)->f3;goto _LL20E;}
else{goto _LL20F;}_LL20F: goto _LL210;_LL20C: _tmp1A5=Cyc_Toc_init_array(nv,lval,
_tmp1A9,_tmp1A5);goto _LL20A;_LL20E: _tmp1A5=Cyc_Toc_init_comprehension(nv,lval,
_tmp1AC,_tmp1AB,_tmp1AA,_tmp1A5,0);goto _LL20A;_LL210: Cyc_Toc_exp_to_c(nv,e);
_tmp1A5=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1A6(
_tmp1A4,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp1A5,0);goto _LL20A;_LL20A:;}}}{
struct Cyc_Absyn_Exp*_tmp1AD=Cyc_Toc_make_struct(nv,_tmp1A3,Cyc_Absyn_strct(
_tmp1A2),_tmp1A5,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp1AD;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
dles,struct _tuple0*tdn){struct _tuple0*_tmp1AE=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp1AF=Cyc_Absyn_var_exp(_tmp1AE,0);struct Cyc_Absyn_Stmt*_tmp1B0=Cyc_Absyn_exp_stmt(
_tmp1AF,0);struct Cyc_Absyn_Exp*(*_tmp1B1)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,
struct Cyc_Position_Segment*)=pointer? Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct _RegionHandle _tmp1B2=_new_region("r");struct _RegionHandle*r=&
_tmp1B2;_push_region(r);{struct Cyc_List_List*_tmp1B3=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp1B3
!= 0;_tmp1B3=_tmp1B3->tl){struct _tuple4 _tmp1B5;struct Cyc_Absyn_Exp*_tmp1B6;
struct Cyc_List_List*_tmp1B7;struct _tuple4*_tmp1B4=(struct _tuple4*)_tmp1B3->hd;
_tmp1B5=*_tmp1B4;_LL217: _tmp1B7=_tmp1B5.f1;goto _LL216;_LL216: _tmp1B6=_tmp1B5.f2;
goto _LL215;_LL215: is_atomic=is_atomic? Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1B6->topt))->v): 0;if(_tmp1B7 == 0){({void*_tmp1B8[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",
sizeof(unsigned char),22),_tag_arr(_tmp1B8,sizeof(void*),0));});}if(_tmp1B7->tl
!= 0){struct _tuple0*_tmp1B9=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1BA=Cyc_Absyn_var_exp(
_tmp1B9,0);for(0;_tmp1B7 != 0;_tmp1B7=_tmp1B7->tl){void*_tmp1BB=(void*)_tmp1B7->hd;
struct _tagged_arr*_tmp1BC;_LL219: if(*((int*)_tmp1BB)== 1){_LL21D: _tmp1BC=((
struct Cyc_Absyn_FieldName_struct*)_tmp1BB)->f1;goto _LL21A;}else{goto _LL21B;}
_LL21B: goto _LL21C;_LL21A: if(Cyc_Toc_is_poly_field(struct_type,_tmp1BC)){_tmp1BA=
Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp1BA,0);}_tmp1B0=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1B1(_tmp1AF,_tmp1BC,0),_tmp1BA,0),0),
_tmp1B0,0);goto _LL218;_LL21C:({void*_tmp1BD[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(unsigned char),27),_tag_arr(_tmp1BD,sizeof(void*),0));});_LL218:;}Cyc_Toc_exp_to_c(
nv,_tmp1B6);_tmp1B0=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp1BA,_tmp1B6,0),0),_tmp1B0,0);}else{void*_tmp1BE=(void*)_tmp1B7->hd;struct
_tagged_arr*_tmp1BF;_LL21F: if(*((int*)_tmp1BE)== 1){_LL223: _tmp1BF=((struct Cyc_Absyn_FieldName_struct*)
_tmp1BE)->f1;goto _LL220;}else{goto _LL221;}_LL221: goto _LL222;_LL220: {struct Cyc_Absyn_Exp*
lval=_tmp1B1(_tmp1AF,_tmp1BF,0);{void*_tmp1C0=(void*)_tmp1B6->r;struct Cyc_List_List*
_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Vardecl*
_tmp1C4;struct Cyc_List_List*_tmp1C5;void*_tmp1C6;_LL225: if(*((int*)_tmp1C0)== 26){
_LL22D: _tmp1C1=((struct Cyc_Absyn_Array_e_struct*)_tmp1C0)->f1;goto _LL226;}else{
goto _LL227;}_LL227: if(*((int*)_tmp1C0)== 27){_LL230: _tmp1C4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1C0)->f1;goto _LL22F;_LL22F: _tmp1C3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1C0)->f2;goto _LL22E;_LL22E: _tmp1C2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1C0)->f3;goto _LL228;}else{goto _LL229;}_LL229: if(*((int*)_tmp1C0)== 29){
_LL232: _tmp1C6=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C0)->f1;goto
_LL231;_LL231: _tmp1C5=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1C0)->f2;goto
_LL22A;}else{goto _LL22B;}_LL22B: goto _LL22C;_LL226: _tmp1B0=Cyc_Toc_init_array(nv,
lval,_tmp1C1,_tmp1B0);goto _LL224;_LL228: _tmp1B0=Cyc_Toc_init_comprehension(nv,
lval,_tmp1C4,_tmp1C3,_tmp1C2,_tmp1B0,0);goto _LL224;_LL22A: _tmp1B0=Cyc_Toc_init_anon_struct(
nv,lval,_tmp1C6,_tmp1C5,_tmp1B0);goto _LL224;_LL22C: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1B6->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp1B6);if(Cyc_Toc_is_poly_field(struct_type,_tmp1BF)? ! was_ptr_type: 0){_tmp1B6=
Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp1B6,0);}_tmp1B0=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1B6,0),0),_tmp1B0,0);goto _LL224;}
_LL224:;}goto _LL21E;}_LL222:({void*_tmp1C7[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(unsigned char),27),_tag_arr(_tmp1C7,sizeof(void*),0));});_LL21E:;}}}{
struct Cyc_Absyn_Exp*_tmp1C8=Cyc_Toc_make_struct(nv,_tmp1AE,Cyc_Absyn_strctq(tdn),
_tmp1B0,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp1C8;};_pop_region(r);}
struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple7*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));
_tmp1C9[0]=({struct Cyc_Absyn_Increment_e_struct _tmp1CA;_tmp1CA.tag=5;_tmp1CA.f1=
e1;_tmp1CA.f2=(void*)incr;_tmp1CA;});_tmp1C9;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp1CB=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp1CC;struct Cyc_Absyn_Exp*
_tmp1CD;void*_tmp1CE;struct _tagged_arr*_tmp1CF;struct Cyc_Absyn_Exp*_tmp1D0;
_LL234: if(*((int*)_tmp1CB)== 35){_LL23C: _tmp1CC=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp1CB)->f1;goto _LL235;}else{goto _LL236;}_LL236: if(*((int*)_tmp1CB)== 13){
_LL23E: _tmp1CE=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1CB)->f1;goto _LL23D;
_LL23D: _tmp1CD=((struct Cyc_Absyn_Cast_e_struct*)_tmp1CB)->f2;goto _LL237;}else{
goto _LL238;}_LL238: if(*((int*)_tmp1CB)== 21){_LL240: _tmp1D0=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1CB)->f1;goto _LL23F;_LL23F: _tmp1CF=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1CB)->f2;goto _LL239;}else{goto _LL23A;}_LL23A: goto _LL23B;_LL235: Cyc_Toc_lvalue_assign_stmt(
_tmp1CC,fs,f,f_env);goto _LL233;_LL237: Cyc_Toc_lvalue_assign(_tmp1CD,fs,f,f_env);
goto _LL233;_LL239:(void*)(e1->r=(void*)((void*)_tmp1D0->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));
_tmp1D1->hd=_tmp1CF;_tmp1D1->tl=fs;_tmp1D1;}),f,f_env);goto _LL233;_LL23B: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _tagged_arr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL233;}_LL233:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp1D2=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp1D3;struct Cyc_Absyn_Stmt*_tmp1D4;struct Cyc_Absyn_Decl*
_tmp1D5;struct Cyc_Absyn_Stmt*_tmp1D6;_LL242: if((unsigned int)_tmp1D2 > 1?*((int*)
_tmp1D2)== 0: 0){_LL24A: _tmp1D3=((struct Cyc_Absyn_Exp_s_struct*)_tmp1D2)->f1;goto
_LL243;}else{goto _LL244;}_LL244: if((unsigned int)_tmp1D2 > 1?*((int*)_tmp1D2)== 
12: 0){_LL24C: _tmp1D5=((struct Cyc_Absyn_Decl_s_struct*)_tmp1D2)->f1;goto _LL24B;
_LL24B: _tmp1D4=((struct Cyc_Absyn_Decl_s_struct*)_tmp1D2)->f2;goto _LL245;}else{
goto _LL246;}_LL246: if((unsigned int)_tmp1D2 > 1?*((int*)_tmp1D2)== 1: 0){_LL24D:
_tmp1D6=((struct Cyc_Absyn_Seq_s_struct*)_tmp1D2)->f2;goto _LL247;}else{goto _LL248;}
_LL248: goto _LL249;_LL243: Cyc_Toc_lvalue_assign(_tmp1D3,fs,f,f_env);goto _LL241;
_LL245: Cyc_Toc_lvalue_assign_stmt(_tmp1D4,fs,f,f_env);goto _LL241;_LL247: Cyc_Toc_lvalue_assign_stmt(
_tmp1D6,fs,f,f_env);goto _LL241;_LL249:({struct Cyc_Std_String_pa_struct _tmp1D8;
_tmp1D8.tag=0;_tmp1D8.f1=(struct _tagged_arr)Cyc_Absynpp_stmt2string(s);{void*
_tmp1D7[1]={& _tmp1D8};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("lvalue_assign_stmt: %s",sizeof(unsigned char),23),_tag_arr(_tmp1D7,
sizeof(void*),1));}});goto _LL241;_LL241:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0){return 0;}result=({
struct Cyc_List_List*_tmp1D9=_region_malloc(r2,sizeof(*_tmp1D9));_tmp1D9->hd=(
void*)f((void*)x->hd,env);_tmp1D9->tl=0;_tmp1D9;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp1DA=_region_malloc(r2,sizeof(*_tmp1DA));_tmp1DA->hd=(void*)f((void*)x->hd,
env);_tmp1DA->tl=0;_tmp1DA;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple4*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple4*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB->f1=0;_tmp1DB->f2=e;_tmp1DB;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp1DC=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp1DD;_LL24F: if((unsigned int)_tmp1DC > 3?*((int*)
_tmp1DC)== 4: 0){_LL253: _tmp1DD=((struct Cyc_Absyn_PointerType_struct*)_tmp1DC)->f1;
goto _LL250;}else{goto _LL251;}_LL251: goto _LL252;_LL250: return _tmp1DD;_LL252:({
void*_tmp1DE[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("get_ptr_typ: not a pointer!",sizeof(unsigned char),28),_tag_arr(
_tmp1DE,sizeof(void*),0));});_LL24E:;}struct _tuple8{void*f1;void*f2;};struct
_tuple9{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple10{struct
Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp1DF=(void*)e->r;if(e->topt
== 0){({struct Cyc_Std_String_pa_struct _tmp1E1;_tmp1E1.tag=0;_tmp1E1.f1=(struct
_tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp1E0[1]={& _tmp1E1};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: no type for %s",
sizeof(unsigned char),25),_tag_arr(_tmp1E0,sizeof(void*),1));}});}{void*old_typ=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp1E2=_tmp1DF;void*
_tmp1E3;void*_tmp1E4;struct _tuple0*_tmp1E5;struct _tuple0*_tmp1E6;struct Cyc_List_List*
_tmp1E7;void*_tmp1E8;void*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*
_tmp1EB;struct Cyc_Core_Opt*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*
_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_Absyn_Exp*
_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_List_List*_tmp1F3;struct Cyc_Absyn_Exp*
_tmp1F4;struct Cyc_Absyn_VarargCallInfo*_tmp1F5;struct Cyc_List_List*_tmp1F6;
struct Cyc_Absyn_Exp*_tmp1F7;struct Cyc_Absyn_VarargCallInfo*_tmp1F8;struct Cyc_Absyn_VarargCallInfo
_tmp1F9;struct Cyc_Absyn_VarargInfo*_tmp1FA;struct Cyc_List_List*_tmp1FB;int
_tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_Absyn_Exp*_tmp1FE;struct Cyc_Absyn_Exp*
_tmp1FF;struct Cyc_Absyn_Exp*_tmp200;struct Cyc_List_List*_tmp201;struct Cyc_Absyn_Exp*
_tmp202;struct Cyc_Absyn_Exp*_tmp203;void*_tmp204;void**_tmp205;struct Cyc_Absyn_Exp*
_tmp206;struct Cyc_Absyn_Exp*_tmp207;struct Cyc_Absyn_Exp*_tmp208;struct Cyc_Absyn_Exp*
_tmp209;void*_tmp20A;void*_tmp20B;struct _tagged_arr*_tmp20C;void*_tmp20D;void*
_tmp20E;unsigned int _tmp20F;void*_tmp210;struct Cyc_Absyn_Exp*_tmp211;struct
_tagged_arr*_tmp212;struct Cyc_Absyn_Exp*_tmp213;struct _tagged_arr*_tmp214;struct
Cyc_Absyn_Exp*_tmp215;struct Cyc_Absyn_Exp*_tmp216;struct Cyc_Absyn_Exp*_tmp217;
struct Cyc_List_List*_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_Absyn_Exp*
_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Vardecl*_tmp21C;struct Cyc_Absyn_Aggrdecl*
_tmp21D;struct Cyc_List_List*_tmp21E;struct _tuple0*_tmp21F;struct Cyc_List_List*
_tmp220;void*_tmp221;struct Cyc_Absyn_Tunionfield*_tmp222;struct Cyc_Absyn_Tuniondecl*
_tmp223;struct Cyc_List_List*_tmp224;struct Cyc_Absyn_Tunionfield*_tmp225;struct
Cyc_Absyn_Tuniondecl*_tmp226;struct Cyc_List_List*_tmp227;struct Cyc_Absyn_MallocInfo
_tmp228;int _tmp229;struct Cyc_Absyn_Exp*_tmp22A;void**_tmp22B;struct Cyc_Absyn_Exp*
_tmp22C;int _tmp22D;struct Cyc_Absyn_Stmt*_tmp22E;struct Cyc_Absyn_Fndecl*_tmp22F;
_LL255: if(*((int*)_tmp1E2)== 0){_LL2A9: _tmp1E3=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1E2)->f1;if((int)_tmp1E3 == 0){goto _LL256;}else{goto _LL257;}}else{goto _LL257;}
_LL257: if(*((int*)_tmp1E2)== 0){goto _LL258;}else{goto _LL259;}_LL259: if(*((int*)
_tmp1E2)== 1){_LL2AB: _tmp1E5=((struct Cyc_Absyn_Var_e_struct*)_tmp1E2)->f1;goto
_LL2AA;_LL2AA: _tmp1E4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1E2)->f2;goto
_LL25A;}else{goto _LL25B;}_LL25B: if(*((int*)_tmp1E2)== 2){_LL2AC: _tmp1E6=((struct
Cyc_Absyn_UnknownId_e_struct*)_tmp1E2)->f1;goto _LL25C;}else{goto _LL25D;}_LL25D:
if(*((int*)_tmp1E2)== 3){_LL2AE: _tmp1E8=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1E2)->f1;goto _LL2AD;_LL2AD: _tmp1E7=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1E2)->f2;goto _LL25E;}else{goto _LL25F;}_LL25F: if(*((int*)_tmp1E2)== 5){_LL2B0:
_tmp1EA=((struct Cyc_Absyn_Increment_e_struct*)_tmp1E2)->f1;goto _LL2AF;_LL2AF:
_tmp1E9=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1E2)->f2;goto _LL260;}
else{goto _LL261;}_LL261: if(*((int*)_tmp1E2)== 4){_LL2B3: _tmp1ED=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1E2)->f1;goto _LL2B2;_LL2B2: _tmp1EC=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1E2)->f2;goto _LL2B1;_LL2B1: _tmp1EB=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1E2)->f3;goto _LL262;}else{goto _LL263;}_LL263: if(*((int*)_tmp1E2)== 6){_LL2B6:
_tmp1F0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1E2)->f1;goto _LL2B5;_LL2B5:
_tmp1EF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1E2)->f2;goto _LL2B4;_LL2B4:
_tmp1EE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1E2)->f3;goto _LL264;}else{
goto _LL265;}_LL265: if(*((int*)_tmp1E2)== 7){_LL2B8: _tmp1F2=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1E2)->f1;goto _LL2B7;_LL2B7: _tmp1F1=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1E2)->f2;goto _LL266;}else{goto _LL267;}_LL267: if(*((int*)_tmp1E2)== 8){_LL2BA:
_tmp1F4=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1E2)->f1;goto _LL2B9;_LL2B9:
_tmp1F3=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1E2)->f2;goto _LL268;}else{
goto _LL269;}_LL269: if(*((int*)_tmp1E2)== 9){_LL2BD: _tmp1F7=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E2)->f1;goto _LL2BC;_LL2BC: _tmp1F6=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E2)->f2;goto _LL2BB;_LL2BB: _tmp1F5=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E2)->f3;if(_tmp1F5 == 0){goto _LL26A;}else{goto _LL26B;}}else{goto _LL26B;}
_LL26B: if(*((int*)_tmp1E2)== 9){_LL2C3: _tmp1FE=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E2)->f1;goto _LL2C2;_LL2C2: _tmp1FD=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E2)->f2;goto _LL2BE;_LL2BE: _tmp1F8=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E2)->f3;if(_tmp1F8 == 0){goto _LL26D;}else{_tmp1F9=*_tmp1F8;_LL2C1: _tmp1FC=
_tmp1F9.num_varargs;goto _LL2C0;_LL2C0: _tmp1FB=_tmp1F9.injectors;goto _LL2BF;
_LL2BF: _tmp1FA=_tmp1F9.vai;goto _LL26C;}}else{goto _LL26D;}_LL26D: if(*((int*)
_tmp1E2)== 10){_LL2C4: _tmp1FF=((struct Cyc_Absyn_Throw_e_struct*)_tmp1E2)->f1;
goto _LL26E;}else{goto _LL26F;}_LL26F: if(*((int*)_tmp1E2)== 11){_LL2C5: _tmp200=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1E2)->f1;goto _LL270;}else{goto _LL271;}
_LL271: if(*((int*)_tmp1E2)== 12){_LL2C7: _tmp202=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1E2)->f1;goto _LL2C6;_LL2C6: _tmp201=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1E2)->f2;goto _LL272;}else{goto _LL273;}_LL273: if(*((int*)_tmp1E2)== 13){
_LL2C9: _tmp204=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1E2)->f1;_tmp205=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1E2)->f1);goto _LL2C8;_LL2C8:
_tmp203=((struct Cyc_Absyn_Cast_e_struct*)_tmp1E2)->f2;goto _LL274;}else{goto
_LL275;}_LL275: if(*((int*)_tmp1E2)== 14){_LL2CA: _tmp206=((struct Cyc_Absyn_Address_e_struct*)
_tmp1E2)->f1;goto _LL276;}else{goto _LL277;}_LL277: if(*((int*)_tmp1E2)== 15){
_LL2CC: _tmp208=((struct Cyc_Absyn_New_e_struct*)_tmp1E2)->f1;goto _LL2CB;_LL2CB:
_tmp207=((struct Cyc_Absyn_New_e_struct*)_tmp1E2)->f2;goto _LL278;}else{goto _LL279;}
_LL279: if(*((int*)_tmp1E2)== 17){_LL2CD: _tmp209=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp1E2)->f1;goto _LL27A;}else{goto _LL27B;}_LL27B: if(*((int*)_tmp1E2)== 16){
_LL2CE: _tmp20A=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1E2)->f1;goto
_LL27C;}else{goto _LL27D;}_LL27D: if(*((int*)_tmp1E2)== 19){goto _LL27E;}else{goto
_LL27F;}_LL27F: if(*((int*)_tmp1E2)== 18){_LL2D1: _tmp20D=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1E2)->f1;goto _LL2CF;_LL2CF: _tmp20B=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1E2)->f2;if(*((int*)_tmp20B)== 0){_LL2D0: _tmp20C=((struct Cyc_Absyn_StructField_struct*)
_tmp20B)->f1;goto _LL280;}else{goto _LL281;}}else{goto _LL281;}_LL281: if(*((int*)
_tmp1E2)== 18){_LL2D4: _tmp210=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1E2)->f1;goto _LL2D2;_LL2D2: _tmp20E=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1E2)->f2;if(*((int*)_tmp20E)== 1){_LL2D3: _tmp20F=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp20E)->f1;goto _LL282;}else{goto _LL283;}}else{goto _LL283;}_LL283: if(*((int*)
_tmp1E2)== 20){_LL2D5: _tmp211=((struct Cyc_Absyn_Deref_e_struct*)_tmp1E2)->f1;
goto _LL284;}else{goto _LL285;}_LL285: if(*((int*)_tmp1E2)== 21){_LL2D7: _tmp213=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1E2)->f1;goto _LL2D6;_LL2D6: _tmp212=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1E2)->f2;goto _LL286;}else{goto _LL287;}
_LL287: if(*((int*)_tmp1E2)== 22){_LL2D9: _tmp215=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1E2)->f1;goto _LL2D8;_LL2D8: _tmp214=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1E2)->f2;goto _LL288;}else{goto _LL289;}_LL289: if(*((int*)_tmp1E2)== 23){
_LL2DB: _tmp217=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1E2)->f1;goto _LL2DA;
_LL2DA: _tmp216=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1E2)->f2;goto _LL28A;}
else{goto _LL28B;}_LL28B: if(*((int*)_tmp1E2)== 24){_LL2DC: _tmp218=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1E2)->f1;goto _LL28C;}else{goto _LL28D;}_LL28D: if(*((int*)_tmp1E2)== 26){
_LL2DD: _tmp219=((struct Cyc_Absyn_Array_e_struct*)_tmp1E2)->f1;goto _LL28E;}else{
goto _LL28F;}_LL28F: if(*((int*)_tmp1E2)== 27){_LL2E0: _tmp21C=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1E2)->f1;goto _LL2DF;_LL2DF: _tmp21B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1E2)->f2;goto _LL2DE;_LL2DE: _tmp21A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1E2)->f3;goto _LL290;}else{goto _LL291;}_LL291: if(*((int*)_tmp1E2)== 28){
_LL2E3: _tmp21F=((struct Cyc_Absyn_Struct_e_struct*)_tmp1E2)->f1;goto _LL2E2;_LL2E2:
_tmp21E=((struct Cyc_Absyn_Struct_e_struct*)_tmp1E2)->f2;goto _LL2E1;_LL2E1:
_tmp21D=((struct Cyc_Absyn_Struct_e_struct*)_tmp1E2)->f3;goto _LL292;}else{goto
_LL293;}_LL293: if(*((int*)_tmp1E2)== 29){_LL2E5: _tmp221=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1E2)->f1;goto _LL2E4;_LL2E4: _tmp220=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1E2)->f2;goto _LL294;}else{goto _LL295;}_LL295: if(*((int*)_tmp1E2)== 30){
_LL2E8: _tmp224=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1E2)->f1;if(_tmp224 == 0){
goto _LL2E7;}else{goto _LL297;}_LL2E7: _tmp223=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1E2)->f2;goto _LL2E6;_LL2E6: _tmp222=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1E2)->f3;goto _LL296;}else{goto _LL297;}_LL297: if(*((int*)_tmp1E2)== 30){
_LL2EB: _tmp227=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1E2)->f1;goto _LL2EA;_LL2EA:
_tmp226=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1E2)->f2;goto _LL2E9;_LL2E9:
_tmp225=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1E2)->f3;goto _LL298;}else{goto
_LL299;}_LL299: if(*((int*)_tmp1E2)== 31){goto _LL29A;}else{goto _LL29B;}_LL29B: if(*((
int*)_tmp1E2)== 32){goto _LL29C;}else{goto _LL29D;}_LL29D: if(*((int*)_tmp1E2)== 33){
_LL2EC: _tmp228=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1E2)->f1;_LL2F1: _tmp22D=
_tmp228.is_calloc;goto _LL2F0;_LL2F0: _tmp22C=_tmp228.rgn;goto _LL2EF;_LL2EF:
_tmp22B=_tmp228.elt_type;goto _LL2EE;_LL2EE: _tmp22A=_tmp228.num_elts;goto _LL2ED;
_LL2ED: _tmp229=_tmp228.fat_result;goto _LL29E;}else{goto _LL29F;}_LL29F: if(*((int*)
_tmp1E2)== 35){_LL2F2: _tmp22E=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp1E2)->f1;
goto _LL2A0;}else{goto _LL2A1;}_LL2A1: if(*((int*)_tmp1E2)== 34){goto _LL2A2;}else{
goto _LL2A3;}_LL2A3: if(*((int*)_tmp1E2)== 25){goto _LL2A4;}else{goto _LL2A5;}_LL2A5:
if(*((int*)_tmp1E2)== 36){_LL2F3: _tmp22F=((struct Cyc_Absyn_Codegen_e_struct*)
_tmp1E2)->f1;goto _LL2A6;}else{goto _LL2A7;}_LL2A7: if(*((int*)_tmp1E2)== 37){goto
_LL2A8;}else{goto _LL254;}_LL256: {struct Cyc_Absyn_Exp*_tmp230=Cyc_Absyn_uint_exp(
0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(nv->toplevel){(void*)(e->r=(
void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_typ,_tmp230,_tmp230))->r));}
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp231[3];_tmp231[2]=_tmp230;_tmp231[1]=_tmp230;_tmp231[0]=
_tmp230;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp231,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(void*)((
void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL254;}_LL258: goto _LL254;_LL25A:{
struct _handler_cons _tmp232;_push_handler(& _tmp232);{int _tmp234=0;if(setjmp(
_tmp232.handler)){_tmp234=1;}if(! _tmp234){(void*)(e->r=(void*)((void*)(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)(nv->varmap,
_tmp1E5))->r));;_pop_handler();}else{void*_tmp233=(void*)_exn_thrown;void*
_tmp236=_tmp233;_LL2F5: if(_tmp236 == Cyc_Dict_Absent){goto _LL2F6;}else{goto _LL2F7;}
_LL2F7: goto _LL2F8;_LL2F6:({struct Cyc_Std_String_pa_struct _tmp238;_tmp238.tag=0;
_tmp238.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1E5);{void*_tmp237[1]={&
_tmp238};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Can't find %s in exp_to_c, Var\n",sizeof(unsigned char),32),_tag_arr(
_tmp237,sizeof(void*),1));}});_LL2F8:(void)_throw(_tmp236);_LL2F4:;}}}goto _LL254;
_LL25C:({void*_tmp239[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("unknownid",sizeof(unsigned char),10),_tag_arr(
_tmp239,sizeof(void*),0));});_LL25E: {struct Cyc_List_List*_tmp23A=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,
_tmp1E7);((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp1E7);{void*
_tmp23B=_tmp1E8;_LL2FA: if((int)_tmp23B == 19){goto _LL2FB;}else{goto _LL2FC;}_LL2FC:
if((int)_tmp23B == 0){goto _LL2FD;}else{goto _LL2FE;}_LL2FE: if((int)_tmp23B == 2){
goto _LL2FF;}else{goto _LL300;}_LL300: if((int)_tmp23B == 5){goto _LL301;}else{goto
_LL302;}_LL302: if((int)_tmp23B == 6){goto _LL303;}else{goto _LL304;}_LL304: if((int)
_tmp23B == 7){goto _LL305;}else{goto _LL306;}_LL306: if((int)_tmp23B == 9){goto _LL307;}
else{goto _LL308;}_LL308: if((int)_tmp23B == 8){goto _LL309;}else{goto _LL30A;}_LL30A:
if((int)_tmp23B == 10){goto _LL30B;}else{goto _LL30C;}_LL30C: goto _LL30D;_LL2FB: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1E7))->hd;{void*_tmp23C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_Exp*_tmp23D;struct Cyc_Absyn_PtrInfo
_tmp23E;struct Cyc_Absyn_Conref*_tmp23F;struct Cyc_Absyn_Conref*_tmp240;void*
_tmp241;_LL30F: if((unsigned int)_tmp23C > 3?*((int*)_tmp23C)== 7: 0){_LL315:
_tmp23D=((struct Cyc_Absyn_ArrayType_struct*)_tmp23C)->f3;goto _LL310;}else{goto
_LL311;}_LL311: if((unsigned int)_tmp23C > 3?*((int*)_tmp23C)== 4: 0){_LL316:
_tmp23E=((struct Cyc_Absyn_PointerType_struct*)_tmp23C)->f1;_LL319: _tmp241=(void*)
_tmp23E.elt_typ;goto _LL318;_LL318: _tmp240=_tmp23E.nullable;goto _LL317;_LL317:
_tmp23F=_tmp23E.bounds;goto _LL312;}else{goto _LL313;}_LL313: goto _LL314;_LL310:(
void*)(e->r=(void*)((void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp23D))->r));
goto _LL30E;_LL312:{void*_tmp242=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp23F);struct Cyc_Absyn_Exp*_tmp243;_LL31B: if((int)_tmp242 == 0){goto _LL31C;}
else{goto _LL31D;}_LL31D: if((unsigned int)_tmp242 > 1?*((int*)_tmp242)== 0: 0){
_LL31F: _tmp243=((struct Cyc_Absyn_Upper_b_struct*)_tmp242)->f1;goto _LL31E;}else{
goto _LL31A;}_LL31C:(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp244[2];_tmp244[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp241),0);_tmp244[0]=(struct Cyc_Absyn_Exp*)_tmp1E7->hd;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp244,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL31A;_LL31E: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp240)){(void*)(e->r=(void*)((void*)(Cyc_Absyn_conditional_exp(arg,_tmp243,
Cyc_Absyn_uint_exp(0,0),0))->r));}else{(void*)(e->r=(void*)((void*)_tmp243->r));
goto _LL31A;}_LL31A:;}goto _LL30E;_LL314:({struct Cyc_Std_String_pa_struct _tmp247;
_tmp247.tag=0;_tmp247.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp246;_tmp246.tag=0;_tmp246.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v);{void*_tmp245[2]={& _tmp246,&
_tmp247};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("size primop applied to non-array %s (%s)",sizeof(unsigned char),41),
_tag_arr(_tmp245,sizeof(void*),2));}}});_LL30E:;}goto _LL2F9;}_LL2FD: {void*
elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp23A))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1E7))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1E7->tl))->hd;(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp248[3];
_tmp248[2]=e2;_tmp248[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp248[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp248,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}goto _LL2F9;}
_LL2FF: {void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((
struct Cyc_List_List*)_check_null(_tmp23A))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E7))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E7->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(
_tmp23A->tl))->hd)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e->r=(void*)((
void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),0))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp249[3];_tmp249[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);
_tmp249[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp249[0]=e1;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp249,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}goto _LL2F9;}_LL301: goto _LL303;
_LL303: goto _LL305;_LL305: goto _LL307;_LL307: goto _LL309;_LL309: goto _LL30B;_LL30B: {
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1E7))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1E7->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)_check_null(
_tmp23A))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp23A->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ(t1)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));}if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){(void*)(e2->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));}goto _LL2F9;}_LL30D: goto _LL2F9;_LL2F9:;}
goto _LL254;}_LL260: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1EA->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp1EA);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*_tmp24A=(_tmp1E9 == (void*)1? 1: _tmp1E9 == (
void*)3)? Cyc_Toc__tagged_arr_inplace_plus_post_e: Cyc_Toc__tagged_arr_inplace_plus_e;
int _tmp24B=(_tmp1E9 == (void*)0? 1: _tmp1E9 == (void*)1)? 1: - 1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(_tmp24A,({struct Cyc_Absyn_Exp*_tmp24C[3];_tmp24C[2]=
Cyc_Absyn_signed_int_exp(_tmp24B,0);_tmp24C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0);_tmp24C[0]=Cyc_Absyn_address_exp(_tmp1EA,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp24C,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}if(elt_typ == (void*)0? ! Cyc_Absyn_is_lvalue(_tmp1EA): 0){Cyc_Toc_lvalue_assign(
_tmp1EA,0,Cyc_Toc_incr_lvalue,_tmp1E9);(void*)(e->r=(void*)((void*)_tmp1EA->r));}
goto _LL254;}}_LL262: {int e1_poly=Cyc_Toc_is_poly_project(_tmp1ED);void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1ED->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1EB->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp1ED);Cyc_Toc_exp_to_c(nv,_tmp1EB);{int done=0;if(_tmp1EC != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp24D=(void*)_tmp1EC->v;_LL321: if((int)_tmp24D == 0){goto _LL322;}
else{goto _LL323;}_LL323: if((int)_tmp24D == 2){goto _LL324;}else{goto _LL325;}_LL325:
goto _LL326;_LL322: change=_tmp1EB;goto _LL320;_LL324: change=Cyc_Absyn_prim1_exp((
void*)2,_tmp1EB,0);goto _LL320;_LL326:({void*_tmp24E[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("bad t ? pointer arithmetic",
sizeof(unsigned char),27),_tag_arr(_tmp24E,sizeof(void*),0));});_LL320:;}done=1;(
void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp24F[3];_tmp24F[2]=change;_tmp24F[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp24F[0]=Cyc_Absyn_address_exp(_tmp1ED,0);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp24F,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}if(! done){if(e1_poly){(void*)(
_tmp1EB->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp1EB->r,0),0))->r));}if(! Cyc_Absyn_is_lvalue(_tmp1ED)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
struct _tuple7*),struct _tuple7*f_env))Cyc_Toc_lvalue_assign)(_tmp1ED,0,Cyc_Toc_assignop_lvalue,({
struct _tuple7*_tmp250=_cycalloc(sizeof(struct _tuple7)* 1);_tmp250[0]=({struct
_tuple7 _tmp251;_tmp251.f1=_tmp1EC;_tmp251.f2=_tmp1EB;_tmp251;});_tmp250;}));(
void*)(e->r=(void*)((void*)_tmp1ED->r));}}goto _LL254;}}_LL264: Cyc_Toc_exp_to_c(
nv,_tmp1F0);Cyc_Toc_exp_to_c(nv,_tmp1EF);Cyc_Toc_exp_to_c(nv,_tmp1EE);goto _LL254;
_LL266: Cyc_Toc_exp_to_c(nv,_tmp1F2);Cyc_Toc_exp_to_c(nv,_tmp1F1);goto _LL254;
_LL268: _tmp1F7=_tmp1F4;_tmp1F6=_tmp1F3;goto _LL26A;_LL26A: Cyc_Toc_exp_to_c(nv,
_tmp1F7);((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp1F6);goto
_LL254;_LL26C:{struct _RegionHandle _tmp252=_new_region("r");struct _RegionHandle*r=&
_tmp252;_push_region(r);{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp1FC,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp1FA->type);
void*arr_type=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp266=_cycalloc(
sizeof(*_tmp266));_tmp266[0]=({struct Cyc_Absyn_ArrayType_struct _tmp267;_tmp267.tag=
7;_tmp267.f1=(void*)cva_type;_tmp267.f2=Cyc_Toc_mt_tq;_tmp267.f3=(struct Cyc_Absyn_Exp*)
num_varargs_exp;_tmp267;});_tmp266;});int num_args=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp1FD);int num_normargs=num_args - _tmp1FC;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp1FD=_tmp1FD->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1FD))->hd);
new_args=({struct Cyc_List_List*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->hd=(
struct Cyc_Absyn_Exp*)_tmp1FD->hd;_tmp253->tl=new_args;_tmp253;});}}new_args=({
struct Cyc_List_List*_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp255[3];_tmp255[2]=num_varargs_exp;
_tmp255[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp255[0]=argvexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp255,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);_tmp254->tl=new_args;_tmp254;});new_args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp1FE);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp1FE,new_args,
0),0);if(_tmp1FA->inject){struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp256=Cyc_Tcutil_compress((
void*)_tmp1FA->type);struct Cyc_Absyn_TunionInfo _tmp257;void*_tmp258;struct Cyc_Absyn_Tuniondecl**
_tmp259;struct Cyc_Absyn_Tuniondecl*_tmp25A;_LL328: if((unsigned int)_tmp256 > 3?*((
int*)_tmp256)== 2: 0){_LL32C: _tmp257=((struct Cyc_Absyn_TunionType_struct*)_tmp256)->f1;
_LL32D: _tmp258=(void*)_tmp257.tunion_info;if(*((int*)_tmp258)== 1){_LL32E:
_tmp259=((struct Cyc_Absyn_KnownTunion_struct*)_tmp258)->f1;_tmp25A=*_tmp259;goto
_LL329;}else{goto _LL32A;}}else{goto _LL32A;}_LL32A: goto _LL32B;_LL329: tud=_tmp25A;
goto _LL327;_LL32B:({void*_tmp25B[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("toc: unknown tunion in vararg with inject",
sizeof(unsigned char),42),_tag_arr(_tmp25B,sizeof(void*),0));});_LL327:;}{struct
_tagged_arr vs=({unsigned int _tmp262=(unsigned int)_tmp1FC;struct _tuple0**_tmp263=(
struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmp262));
struct _tagged_arr _tmp265=_tag_arr(_tmp263,sizeof(struct _tuple0*),(unsigned int)
_tmp1FC);{unsigned int _tmp264=_tmp262;unsigned int i;for(i=0;i < _tmp264;i ++){
_tmp263[i]=Cyc_Toc_temp_var();}};_tmp265;});struct Cyc_List_List*_tmp25C=0;{int i=
_tmp1FC - 1;for(0;i >= 0;-- i){_tmp25C=({struct Cyc_List_List*_tmp25D=_cycalloc(
sizeof(*_tmp25D));_tmp25D->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(((
struct _tuple0**)vs.curr)[i],0),0));_tmp25D->tl=_tmp25C;_tmp25D;});}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp25C,0),s,0);{
int i=0;for(0;_tmp1FD != 0;(((_tmp1FD=_tmp1FD->tl,_tmp1FB=_tmp1FB->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp1FD->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=((struct _tuple0**)
vs.curr)[i];struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield
_tmp25F;struct Cyc_List_List*_tmp260;struct _tuple0*_tmp261;struct Cyc_Absyn_Tunionfield*
_tmp25E=(struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp1FB))->hd;
_tmp25F=*_tmp25E;_LL331: _tmp261=_tmp25F.name;goto _LL330;_LL330: _tmp260=_tmp25F.typs;
goto _LL32F;_LL32F: {void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple3*)((struct
Cyc_List_List*)_check_null(_tmp260))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star(
field_typ)){arg=Cyc_Absyn_cast_exp(field_typ,arg,0);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(
tud,_tmp261,1),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp261,_tag_arr("_struct",sizeof(unsigned char),8))),0,s,0);}}}}}else{{int i=0;
for(0;_tmp1FD != 0;(_tmp1FD=_tmp1FD->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp1FD->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp1FD->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL254;_LL26E: Cyc_Toc_exp_to_c(
nv,_tmp1FF);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(
old_typ),Cyc_Toc_newthrow_exp(_tmp1FF),0))->r));goto _LL254;_LL270: Cyc_Toc_exp_to_c(
nv,_tmp200);goto _LL254;_LL272: Cyc_Toc_exp_to_c(nv,_tmp202);for(0;_tmp201 != 0;
_tmp201=_tmp201->tl){void*k=Cyc_Tcutil_typ_kind((void*)_tmp201->hd);if(k != (void*)
4? k != (void*)3: 0){{void*_tmp268=Cyc_Tcutil_compress((void*)_tmp201->hd);_LL333:
if((unsigned int)_tmp268 > 3?*((int*)_tmp268)== 1: 0){goto _LL334;}else{goto _LL335;}
_LL335: if((unsigned int)_tmp268 > 3?*((int*)_tmp268)== 2: 0){goto _LL336;}else{goto
_LL337;}_LL337: goto _LL338;_LL334: goto _LL336;_LL336: continue;_LL338:(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),_tmp202,0))->r));goto _LL332;_LL332:;}break;}}goto
_LL254;_LL274: {void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp203->topt))->v;
void*new_typ=*_tmp205;*_tmp205=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,
_tmp203);{struct _tuple8 _tmp26A=({struct _tuple8 _tmp269;_tmp269.f1=Cyc_Tcutil_compress(
old_t2);_tmp269.f2=Cyc_Tcutil_compress(new_typ);_tmp269;});void*_tmp26B;struct
Cyc_Absyn_PtrInfo _tmp26C;void*_tmp26D;struct Cyc_Absyn_PtrInfo _tmp26E;void*
_tmp26F;void*_tmp270;struct Cyc_Absyn_PtrInfo _tmp271;_LL33A: _LL342: _tmp26D=
_tmp26A.f1;if((unsigned int)_tmp26D > 3?*((int*)_tmp26D)== 4: 0){_LL343: _tmp26E=((
struct Cyc_Absyn_PointerType_struct*)_tmp26D)->f1;goto _LL340;}else{goto _LL33C;}
_LL340: _tmp26B=_tmp26A.f2;if((unsigned int)_tmp26B > 3?*((int*)_tmp26B)== 4: 0){
_LL341: _tmp26C=((struct Cyc_Absyn_PointerType_struct*)_tmp26B)->f1;goto _LL33B;}
else{goto _LL33C;}_LL33C: _LL345: _tmp270=_tmp26A.f1;if((unsigned int)_tmp270 > 3?*((
int*)_tmp270)== 4: 0){_LL346: _tmp271=((struct Cyc_Absyn_PointerType_struct*)
_tmp270)->f1;goto _LL344;}else{goto _LL33E;}_LL344: _tmp26F=_tmp26A.f2;if((
unsigned int)_tmp26F > 3?*((int*)_tmp26F)== 5: 0){goto _LL33D;}else{goto _LL33E;}
_LL33E: goto _LL33F;_LL33B: {int _tmp272=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp26E.nullable);int _tmp273=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp26C.nullable);void*_tmp274=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp26E.bounds);void*_tmp275=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp26C.bounds);{
struct _tuple8 _tmp277=({struct _tuple8 _tmp276;_tmp276.f1=_tmp274;_tmp276.f2=
_tmp275;_tmp276;});void*_tmp278;void*_tmp279;void*_tmp27A;void*_tmp27B;struct Cyc_Absyn_Exp*
_tmp27C;void*_tmp27D;struct Cyc_Absyn_Exp*_tmp27E;void*_tmp27F;_LL348: _LL351:
_tmp279=_tmp277.f1;if((unsigned int)_tmp279 > 1?*((int*)_tmp279)== 0: 0){goto
_LL350;}else{goto _LL34A;}_LL350: _tmp278=_tmp277.f2;if((unsigned int)_tmp278 > 1?*((
int*)_tmp278)== 0: 0){goto _LL349;}else{goto _LL34A;}_LL34A: _LL353: _tmp27B=_tmp277.f1;
if((unsigned int)_tmp27B > 1?*((int*)_tmp27B)== 0: 0){_LL354: _tmp27C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp27B)->f1;goto _LL352;}else{goto _LL34C;}_LL352: _tmp27A=_tmp277.f2;if((int)
_tmp27A == 0){goto _LL34B;}else{goto _LL34C;}_LL34C: _LL357: _tmp27F=_tmp277.f1;if((
int)_tmp27F == 0){goto _LL355;}else{goto _LL34E;}_LL355: _tmp27D=_tmp277.f2;if((
unsigned int)_tmp27D > 1?*((int*)_tmp27D)== 0: 0){_LL356: _tmp27E=((struct Cyc_Absyn_Upper_b_struct*)
_tmp27D)->f1;goto _LL34D;}else{goto _LL34E;}_LL34E: goto _LL34F;_LL349: if(_tmp272? !
_tmp273: 0){if(nv->toplevel){({void*_tmp280[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("can't do NULL-check conversion at top-level",
sizeof(unsigned char),44),_tag_arr(_tmp280,sizeof(void*),0));});}(void*)(e->r=(
void*)((void*)(Cyc_Absyn_cast_exp(*_tmp205,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281->hd=_tmp203;
_tmp281->tl=0;_tmp281;}),0),0))->r));}goto _LL347;_LL34B: if(nv->toplevel){(void*)(
e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_t2,_tmp27C,_tmp203))->r));}
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp282[3];_tmp282[2]=_tmp27C;_tmp282[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp26C.elt_typ),0);_tmp282[0]=_tmp203;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp282,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}goto _LL347;_LL34D: if(nv->toplevel){({void*_tmp283[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("can't coerce t? to t* or t@ at the top-level",
sizeof(unsigned char),45),_tag_arr(_tmp283,sizeof(void*),0));});}{struct Cyc_Absyn_Exp*
_tmp284=Cyc_Absyn_fncall_exp(Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*_tmp286[
3];_tmp286[2]=_tmp27E;_tmp286[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)
_tmp26E.elt_typ),0);_tmp286[0]=_tmp203;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp286,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);if(_tmp273){(void*)(_tmp284->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->hd=Cyc_Absyn_copy_exp(
_tmp284);_tmp285->tl=0;_tmp285;}),0))->r));}(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*
_tmp205,_tmp284,0))->r));goto _LL347;}_LL34F: goto _LL347;_LL347:;}goto _LL339;}
_LL33D:{void*_tmp287=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp271.bounds);
_LL359: if((int)_tmp287 == 0){goto _LL35A;}else{goto _LL35B;}_LL35B: goto _LL35C;
_LL35A:(void*)(_tmp203->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)_tmp203->r,_tmp203->loc),Cyc_Toc_curr_sp,0))->r));goto _LL358;_LL35C: goto
_LL358;_LL358:;}goto _LL339;_LL33F: goto _LL339;_LL339:;}goto _LL254;}_LL276:{void*
_tmp288=(void*)_tmp206->r;struct Cyc_List_List*_tmp289;struct _tuple0*_tmp28A;
struct Cyc_List_List*_tmp28B;_LL35E: if(*((int*)_tmp288)== 28){_LL365: _tmp28A=((
struct Cyc_Absyn_Struct_e_struct*)_tmp288)->f1;goto _LL364;_LL364: _tmp289=((struct
Cyc_Absyn_Struct_e_struct*)_tmp288)->f2;goto _LL35F;}else{goto _LL360;}_LL360: if(*((
int*)_tmp288)== 24){_LL366: _tmp28B=((struct Cyc_Absyn_Tuple_e_struct*)_tmp288)->f1;
goto _LL361;}else{goto _LL362;}_LL362: goto _LL363;_LL35F: if(nv->toplevel){({struct
Cyc_Std_String_pa_struct _tmp28D;_tmp28D.tag=0;_tmp28D.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp206->loc);{void*_tmp28C[1]={& _tmp28D};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("%s: & on non-identifiers at the top-level",
sizeof(unsigned char),42),_tag_arr(_tmp28C,sizeof(void*),1));}});}(void*)(e->r=(
void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp206->topt))->v,1,0,_tmp289,_tmp28A))->r));goto _LL35D;_LL361: if(nv->toplevel){({
struct Cyc_Std_String_pa_struct _tmp28F;_tmp28F.tag=0;_tmp28F.f1=(struct
_tagged_arr)Cyc_Position_string_of_segment(_tmp206->loc);{void*_tmp28E[1]={&
_tmp28F};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level",sizeof(unsigned char),42),
_tag_arr(_tmp28E,sizeof(void*),1));}});}(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp28B))->r));goto _LL35D;_LL363: Cyc_Toc_exp_to_c(nv,_tmp206);if(! Cyc_Absyn_is_lvalue(
_tmp206)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp206,0,Cyc_Toc_address_lvalue,
1);(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp206,0))->r));}goto _LL35D;
_LL35D:;}goto _LL254;_LL278: if(nv->toplevel){({struct Cyc_Std_String_pa_struct
_tmp291;_tmp291.tag=0;_tmp291.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp207->loc);{void*_tmp290[1]={& _tmp291};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("%s: new at top-level",sizeof(
unsigned char),21),_tag_arr(_tmp290,sizeof(void*),1));}});}{void*_tmp292=(void*)
_tmp207->r;struct Cyc_List_List*_tmp293;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Absyn_Exp*
_tmp295;struct Cyc_Absyn_Vardecl*_tmp296;struct Cyc_List_List*_tmp297;struct
_tuple0*_tmp298;struct Cyc_List_List*_tmp299;_LL368: if(*((int*)_tmp292)== 26){
_LL372: _tmp293=((struct Cyc_Absyn_Array_e_struct*)_tmp292)->f1;goto _LL369;}else{
goto _LL36A;}_LL36A: if(*((int*)_tmp292)== 27){_LL375: _tmp296=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp292)->f1;goto _LL374;_LL374: _tmp295=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp292)->f2;goto _LL373;_LL373: _tmp294=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp292)->f3;goto _LL36B;}else{goto _LL36C;}_LL36C: if(*((int*)_tmp292)== 28){
_LL377: _tmp298=((struct Cyc_Absyn_Struct_e_struct*)_tmp292)->f1;goto _LL376;_LL376:
_tmp297=((struct Cyc_Absyn_Struct_e_struct*)_tmp292)->f2;goto _LL36D;}else{goto
_LL36E;}_LL36E: if(*((int*)_tmp292)== 24){_LL378: _tmp299=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp292)->f1;goto _LL36F;}else{goto _LL370;}_LL370: goto _LL371;_LL369: {struct
_tuple0*_tmp29A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp29B=Cyc_Absyn_var_exp(
_tmp29A,0);struct Cyc_Absyn_Stmt*_tmp29C=Cyc_Toc_init_array(nv,_tmp29B,_tmp293,
Cyc_Absyn_exp_stmt(_tmp29B,0));void*old_elt_typ;{void*_tmp29D=Cyc_Tcutil_compress(
old_typ);struct Cyc_Absyn_PtrInfo _tmp29E;struct Cyc_Absyn_Tqual _tmp29F;void*
_tmp2A0;_LL37A: if((unsigned int)_tmp29D > 3?*((int*)_tmp29D)== 4: 0){_LL37E:
_tmp29E=((struct Cyc_Absyn_PointerType_struct*)_tmp29D)->f1;_LL380: _tmp2A0=(void*)
_tmp29E.elt_typ;goto _LL37F;_LL37F: _tmp29F=_tmp29E.tq;goto _LL37B;}else{goto _LL37C;}
_LL37C: goto _LL37D;_LL37B: old_elt_typ=_tmp2A0;goto _LL379;_LL37D: old_elt_typ=({
void*_tmp2A1[0]={};Cyc_Toc_toc_impos(_tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof(unsigned char),52),_tag_arr(_tmp2A1,sizeof(void*),0));});_LL379:;}{void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp2A2=Cyc_Absyn_cstar_typ(elt_typ,
Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp2A3=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp293),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp208 == 0){e1=Cyc_Toc_malloc_exp(
old_elt_typ,_tmp2A3);}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(_tmp208);Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp2A3);}(
void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp29A,
_tmp2A2,(struct Cyc_Absyn_Exp*)e1,_tmp29C,0),0))->r));goto _LL367;}}_LL36B: {int
is_tagged_ptr=0;{void*_tmp2A4=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp2A5;struct Cyc_Absyn_Conref*_tmp2A6;struct Cyc_Absyn_Tqual _tmp2A7;void*_tmp2A8;
_LL382: if((unsigned int)_tmp2A4 > 3?*((int*)_tmp2A4)== 4: 0){_LL386: _tmp2A5=((
struct Cyc_Absyn_PointerType_struct*)_tmp2A4)->f1;_LL389: _tmp2A8=(void*)_tmp2A5.elt_typ;
goto _LL388;_LL388: _tmp2A7=_tmp2A5.tq;goto _LL387;_LL387: _tmp2A6=_tmp2A5.bounds;
goto _LL383;}else{goto _LL384;}_LL384: goto _LL385;_LL383: is_tagged_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp2A6)== (void*)0;goto _LL381;_LL385:({void*_tmp2A9[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: comprehension not an array type",
sizeof(unsigned char),42),_tag_arr(_tmp2A9,sizeof(void*),0));});_LL381:;}{struct
_tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp294->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,
Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp295);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp296,Cyc_Absyn_var_exp(max,0),_tmp294,Cyc_Absyn_skip_stmt(
0),1);{struct _RegionHandle _tmp2AA=_new_region("r");struct _RegionHandle*r=&
_tmp2AA;_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp2BA=
_region_malloc(r,sizeof(*_tmp2BA));_tmp2BA->hd=({struct _tuple9*_tmp2BB=
_region_malloc(r,sizeof(*_tmp2BB));_tmp2BB->f1=max;_tmp2BB->f2=Cyc_Absyn_uint_t;
_tmp2BB->f3=(struct Cyc_Absyn_Exp*)_tmp295;_tmp2BB;});_tmp2BA->tl=0;_tmp2BA;});
struct Cyc_Absyn_Exp*ai;if(_tmp208 == 0){ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp2AB[2];_tmp2AB[1]=Cyc_Absyn_var_exp(
max,0);_tmp2AB[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2AB,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(_tmp208);
Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp2AC[2];_tmp2AC[1]=Cyc_Absyn_var_exp(max,0);_tmp2AC[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp2AC,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct
Cyc_Absyn_Exp*ainit=Cyc_Absyn_cast_exp(ptr_typ,ai,0);decls=({struct Cyc_List_List*
_tmp2AD=_region_malloc(r,sizeof(*_tmp2AD));_tmp2AD->hd=({struct _tuple9*_tmp2AE=
_region_malloc(r,sizeof(*_tmp2AE));_tmp2AE->f1=a;_tmp2AE->f2=ptr_typ;_tmp2AE->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp2AE;});_tmp2AD->tl=decls;_tmp2AD;});if(
is_tagged_ptr){struct _tuple0*_tmp2AF=Cyc_Toc_temp_var();void*_tmp2B0=Cyc_Toc_typ_to_c(
old_typ);struct Cyc_Absyn_Exp*_tmp2B1=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2B4[3];_tmp2B4[2]=_tmp295;_tmp2B4[1]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);_tmp2B4[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp2B4,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);decls=({struct Cyc_List_List*_tmp2B2=_region_malloc(r,sizeof(*_tmp2B2));
_tmp2B2->hd=({struct _tuple9*_tmp2B3=_region_malloc(r,sizeof(*_tmp2B3));_tmp2B3->f1=
_tmp2AF;_tmp2B3->f2=_tmp2B0;_tmp2B3->f3=(struct Cyc_Absyn_Exp*)_tmp2B1;_tmp2B3;});
_tmp2B2->tl=decls;_tmp2B2;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
_tmp2AF,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
a,0),0),0);}{struct Cyc_List_List*_tmp2B5=decls;for(0;_tmp2B5 != 0;_tmp2B5=_tmp2B5->tl){
struct Cyc_Absyn_Exp*_tmp2B7;void*_tmp2B8;struct _tuple0*_tmp2B9;struct _tuple9
_tmp2B6=*((struct _tuple9*)_tmp2B5->hd);_LL38D: _tmp2B9=_tmp2B6.f1;goto _LL38C;
_LL38C: _tmp2B8=_tmp2B6.f2;goto _LL38B;_LL38B: _tmp2B7=_tmp2B6.f3;goto _LL38A;_LL38A:
s=Cyc_Absyn_declare_stmt(_tmp2B9,_tmp2B8,_tmp2B7,s,0);}}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL367;}}}_LL36D:(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp207->topt))->v,1,_tmp208,_tmp297,_tmp298))->r));goto _LL367;
_LL36F:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp208,_tmp299))->r));
goto _LL367;_LL371: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp207->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple0*
_tmp2BC=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Absyn_var_exp(_tmp2BC,
0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp2BD,
0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp208 == 0){mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(
_tmp208);Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp2BE=(void*)_tmp207->r;struct Cyc_Absyn_Exp*_tmp2BF;void*_tmp2C0;_LL38F:
if(*((int*)_tmp2BE)== 13){_LL394: _tmp2C0=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp2BE)->f1;goto _LL393;_LL393: _tmp2BF=((struct Cyc_Absyn_Cast_e_struct*)_tmp2BE)->f2;
goto _LL390;}else{goto _LL391;}_LL391: goto _LL392;_LL390:{struct _tuple8 _tmp2C2=({
struct _tuple8 _tmp2C1;_tmp2C1.f1=Cyc_Tcutil_compress(_tmp2C0);_tmp2C1.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2BF->topt))->v);_tmp2C1;});void*
_tmp2C3;struct Cyc_Absyn_PtrInfo _tmp2C4;struct Cyc_Absyn_Conref*_tmp2C5;void*
_tmp2C6;struct Cyc_Absyn_PtrInfo _tmp2C7;struct Cyc_Absyn_Conref*_tmp2C8;void*
_tmp2C9;_LL396: _LL39D: _tmp2C6=_tmp2C2.f1;if((unsigned int)_tmp2C6 > 3?*((int*)
_tmp2C6)== 4: 0){_LL39E: _tmp2C7=((struct Cyc_Absyn_PointerType_struct*)_tmp2C6)->f1;
_LL3A0: _tmp2C9=(void*)_tmp2C7.elt_typ;goto _LL39F;_LL39F: _tmp2C8=_tmp2C7.bounds;
goto _LL39A;}else{goto _LL398;}_LL39A: _tmp2C3=_tmp2C2.f2;if((unsigned int)_tmp2C3 > 
3?*((int*)_tmp2C3)== 4: 0){_LL39B: _tmp2C4=((struct Cyc_Absyn_PointerType_struct*)
_tmp2C3)->f1;_LL39C: _tmp2C5=_tmp2C4.bounds;goto _LL397;}else{goto _LL398;}_LL398:
goto _LL399;_LL397:{struct _tuple8 _tmp2CB=({struct _tuple8 _tmp2CA;_tmp2CA.f1=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp2C8);_tmp2CA.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp2C5);_tmp2CA;});void*_tmp2CC;struct Cyc_Absyn_Exp*_tmp2CD;void*_tmp2CE;_LL3A2:
_LL3A8: _tmp2CE=_tmp2CB.f1;if((int)_tmp2CE == 0){goto _LL3A6;}else{goto _LL3A4;}
_LL3A6: _tmp2CC=_tmp2CB.f2;if((unsigned int)_tmp2CC > 1?*((int*)_tmp2CC)== 0: 0){
_LL3A7: _tmp2CD=((struct Cyc_Absyn_Upper_b_struct*)_tmp2CC)->f1;goto _LL3A3;}else{
goto _LL3A4;}_LL3A4: goto _LL3A5;_LL3A3:(void*)(inner_mexp->r=(void*)((void*)(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0))->r));done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2CF[4];_tmp2CF[3]=_tmp2CD;_tmp2CF[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp2C9,0);_tmp2CF[1]=_tmp2BF;_tmp2CF[0]=mexp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp2CF,sizeof(struct Cyc_Absyn_Exp*),4));}),
0))->r));goto _LL3A1;_LL3A5: goto _LL3A1;_LL3A1:;}goto _LL395;_LL399: goto _LL395;
_LL395:;}goto _LL38E;_LL392: goto _LL38E;_LL38E:;}if(! done){struct Cyc_Absyn_Stmt*
_tmp2D0=Cyc_Absyn_exp_stmt(_tmp2BD,0);struct Cyc_Absyn_Exp*_tmp2D1=Cyc_Absyn_signed_int_exp(
0,0);Cyc_Toc_exp_to_c(nv,_tmp207);_tmp2D0=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp(_tmp2BD,_tmp2D1,0),_tmp207,0),_tmp2D0,0);{void*_tmp2D2=
Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt(_tmp2BC,_tmp2D2,(struct Cyc_Absyn_Exp*)mexp,_tmp2D0,0),0))->r));}}
goto _LL367;}}_LL367:;}goto _LL254;_LL27A: Cyc_Toc_exp_to_c(nv,_tmp209);goto _LL254;
_LL27C:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp2D3=
_cycalloc(sizeof(*_tmp2D3));_tmp2D3[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp2D4;_tmp2D4.tag=16;_tmp2D4.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp20A);_tmp2D4;});
_tmp2D3;})));goto _LL254;_LL27E:({void*_tmp2D5[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("__gen() in code generator",
sizeof(unsigned char),26),_tag_arr(_tmp2D5,sizeof(void*),0));});_LL280:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2D6=_cycalloc(sizeof(*
_tmp2D6));_tmp2D6[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2D7;_tmp2D7.tag=18;
_tmp2D7.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp20D);_tmp2D7.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8[0]=({
struct Cyc_Absyn_StructField_struct _tmp2D9;_tmp2D9.tag=0;_tmp2D9.f1=_tmp20C;
_tmp2D9;});_tmp2D8;}));_tmp2D7;});_tmp2D6;})));goto _LL254;_LL282:{void*_tmp2DA=
Cyc_Tcutil_compress(_tmp210);struct Cyc_Absyn_AggrInfo _tmp2DB;void*_tmp2DC;struct
Cyc_List_List*_tmp2DD;_LL3AA: if((unsigned int)_tmp2DA > 3?*((int*)_tmp2DA)== 10: 0){
_LL3B4: _tmp2DB=((struct Cyc_Absyn_AggrType_struct*)_tmp2DA)->f1;_LL3B5: _tmp2DC=(
void*)_tmp2DB.aggr_info;goto _LL3AB;}else{goto _LL3AC;}_LL3AC: if((unsigned int)
_tmp2DA > 3?*((int*)_tmp2DA)== 11: 0){_LL3B6: _tmp2DD=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2DA)->f2;goto _LL3AD;}else{goto _LL3AE;}_LL3AE: if((unsigned int)_tmp2DA > 3?*((
int*)_tmp2DA)== 9: 0){goto _LL3AF;}else{goto _LL3B0;}_LL3B0: if((unsigned int)
_tmp2DA > 3?*((int*)_tmp2DA)== 3: 0){goto _LL3B1;}else{goto _LL3B2;}_LL3B2: goto
_LL3B3;_LL3AB: {struct Cyc_Absyn_Aggrdecl*_tmp2DE=Cyc_Absyn_get_known_aggrdecl(
_tmp2DC);if(_tmp2DE->fields == 0){({void*_tmp2DF[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("struct fields must be known",
sizeof(unsigned char),28),_tag_arr(_tmp2DF,sizeof(void*),0));});}_tmp2DD=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2DE->fields))->v;goto _LL3AD;}
_LL3AD: {struct Cyc_Absyn_Aggrfield*_tmp2E0=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2DD,(int)_tmp20F);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2E1=_cycalloc(sizeof(*
_tmp2E1));_tmp2E1[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2E2;_tmp2E2.tag=18;
_tmp2E2.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp210);_tmp2E2.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3[0]=({
struct Cyc_Absyn_StructField_struct _tmp2E4;_tmp2E4.tag=0;_tmp2E4.f1=_tmp2E0->name;
_tmp2E4;});_tmp2E3;}));_tmp2E2;});_tmp2E1;})));goto _LL3A9;}_LL3AF:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2E5=_cycalloc(sizeof(*
_tmp2E5));_tmp2E5[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2E6;_tmp2E6.tag=18;
_tmp2E6.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp210);_tmp2E6.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2E7=_cycalloc(sizeof(*_tmp2E7));_tmp2E7[0]=({
struct Cyc_Absyn_StructField_struct _tmp2E8;_tmp2E8.tag=0;_tmp2E8.f1=Cyc_Absyn_fieldname((
int)(_tmp20F + 1));_tmp2E8;});_tmp2E7;}));_tmp2E6;});_tmp2E5;})));goto _LL3A9;
_LL3B1: if(_tmp20F == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp2EA;_tmp2EA.tag=18;_tmp2EA.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp210);_tmp2EA.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2EB=_cycalloc(sizeof(*
_tmp2EB));_tmp2EB[0]=({struct Cyc_Absyn_StructField_struct _tmp2EC;_tmp2EC.tag=0;
_tmp2EC.f1=Cyc_Toc_tag_sp;_tmp2EC;});_tmp2EB;}));_tmp2EA;});_tmp2E9;})));}else{(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2ED=_cycalloc(
sizeof(*_tmp2ED));_tmp2ED[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2EE;_tmp2EE.tag=
18;_tmp2EE.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp210);_tmp2EE.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF[0]=({
struct Cyc_Absyn_StructField_struct _tmp2F0;_tmp2F0.tag=0;_tmp2F0.f1=Cyc_Absyn_fieldname((
int)_tmp20F);_tmp2F0;});_tmp2EF;}));_tmp2EE;});_tmp2ED;})));}goto _LL3A9;_LL3B3:({
void*_tmp2F1[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("impossible type for offsetof tuple index",sizeof(unsigned char),41),
_tag_arr(_tmp2F1,sizeof(void*),0));});_LL3A9:;}goto _LL254;_LL284: {void*_tmp2F2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp211->topt))->v);{
void*_tmp2F3=_tmp2F2;struct Cyc_Absyn_PtrInfo _tmp2F4;struct Cyc_Absyn_Conref*
_tmp2F5;struct Cyc_Absyn_Tqual _tmp2F6;struct Cyc_Absyn_Conref*_tmp2F7;void*_tmp2F8;
void*_tmp2F9;_LL3B8: if((unsigned int)_tmp2F3 > 3?*((int*)_tmp2F3)== 4: 0){_LL3BC:
_tmp2F4=((struct Cyc_Absyn_PointerType_struct*)_tmp2F3)->f1;_LL3C1: _tmp2F9=(void*)
_tmp2F4.elt_typ;goto _LL3C0;_LL3C0: _tmp2F8=(void*)_tmp2F4.rgn_typ;goto _LL3BF;
_LL3BF: _tmp2F7=_tmp2F4.nullable;goto _LL3BE;_LL3BE: _tmp2F6=_tmp2F4.tq;goto _LL3BD;
_LL3BD: _tmp2F5=_tmp2F4.bounds;goto _LL3B9;}else{goto _LL3BA;}_LL3BA: goto _LL3BB;
_LL3B9:{void*_tmp2FA=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp2F5);_LL3C3:
if((unsigned int)_tmp2FA > 1?*((int*)_tmp2FA)== 0: 0){goto _LL3C4;}else{goto _LL3C5;}
_LL3C5: if((int)_tmp2FA == 0){goto _LL3C6;}else{goto _LL3C2;}_LL3C4: {int
do_null_check=Cyc_Toc_need_null_check(_tmp211);Cyc_Toc_exp_to_c(nv,_tmp211);if(
do_null_check){(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp211->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp2FB=
_cycalloc(sizeof(*_tmp2FB));_tmp2FB->hd=_tmp211;_tmp2FB->tl=0;_tmp2FB;}),0),0),0))->r));}
goto _LL3C2;}_LL3C6: {struct Cyc_Absyn_Exp*_tmp2FC=Cyc_Absyn_uint_exp(0,0);_tmp2FC->topt=({
struct Cyc_Core_Opt*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD->v=(void*)Cyc_Absyn_uint_t;
_tmp2FD;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp211,_tmp2FC,
0))->r));Cyc_Toc_exp_to_c(nv,e);goto _LL3C2;}_LL3C2:;}goto _LL3B7;_LL3BB:({void*
_tmp2FE[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer",sizeof(unsigned char),29),_tag_arr(
_tmp2FE,sizeof(void*),0));});_LL3B7:;}goto _LL254;}_LL286: Cyc_Toc_exp_to_c(nv,
_tmp213);if(Cyc_Toc_is_poly_project(e)){(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((
void*)e->r,0),0))->r));}goto _LL254;_LL288: {void*e1typ=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp215->topt))->v);int do_null_check=Cyc_Toc_need_null_check(
_tmp215);Cyc_Toc_exp_to_c(nv,_tmp215);{int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_Conref*_tmp300;struct Cyc_Absyn_Tqual _tmp301;struct Cyc_Absyn_Conref*
_tmp302;void*_tmp303;void*_tmp304;struct Cyc_Absyn_PtrInfo _tmp2FF=Cyc_Toc_get_ptr_type(
e1typ);_LL3CC: _tmp304=(void*)_tmp2FF.elt_typ;goto _LL3CB;_LL3CB: _tmp303=(void*)
_tmp2FF.rgn_typ;goto _LL3CA;_LL3CA: _tmp302=_tmp2FF.nullable;goto _LL3C9;_LL3C9:
_tmp301=_tmp2FF.tq;goto _LL3C8;_LL3C8: _tmp300=_tmp2FF.bounds;goto _LL3C7;_LL3C7:{
void*_tmp305=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp300);struct Cyc_Absyn_Exp*
_tmp306;_LL3CE: if((unsigned int)_tmp305 > 1?*((int*)_tmp305)== 0: 0){_LL3D2:
_tmp306=((struct Cyc_Absyn_Upper_b_struct*)_tmp305)->f1;goto _LL3CF;}else{goto
_LL3D0;}_LL3D0: if((int)_tmp305 == 0){goto _LL3D1;}else{goto _LL3CD;}_LL3CF: if(Cyc_Evexp_eval_const_uint_exp(
_tmp306)< 1){({void*_tmp307[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("exp_to_c:  AggrArrow_e on pointer of size 0",
sizeof(unsigned char),44),_tag_arr(_tmp307,sizeof(void*),0));});}if(
do_null_check){(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp215->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp308=
_cycalloc(sizeof(*_tmp308));_tmp308->hd=_tmp215;_tmp308->tl=0;_tmp308;}),0),0),
_tmp214,0))->r));}goto _LL3CD;_LL3D1: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp304);(
void*)(_tmp215->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,
_tmp301),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*
_tmp309[3];_tmp309[2]=Cyc_Absyn_uint_exp(0,0);_tmp309[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp309[0]=Cyc_Absyn_copy_exp(_tmp215);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp309,sizeof(struct Cyc_Absyn_Exp*),3));}),
0),0))->r));goto _LL3CD;}_LL3CD:;}if(is_poly){(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((
void*)e->r,0),0))->r));}goto _LL254;}}_LL28A: {void*_tmp30A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp217->topt))->v);{void*_tmp30B=
_tmp30A;struct Cyc_List_List*_tmp30C;struct Cyc_Absyn_PtrInfo _tmp30D;struct Cyc_Absyn_Conref*
_tmp30E;struct Cyc_Absyn_Tqual _tmp30F;struct Cyc_Absyn_Conref*_tmp310;void*_tmp311;
void*_tmp312;_LL3D4: if((unsigned int)_tmp30B > 3?*((int*)_tmp30B)== 9: 0){_LL3DA:
_tmp30C=((struct Cyc_Absyn_TupleType_struct*)_tmp30B)->f1;goto _LL3D5;}else{goto
_LL3D6;}_LL3D6: if((unsigned int)_tmp30B > 3?*((int*)_tmp30B)== 4: 0){_LL3DB:
_tmp30D=((struct Cyc_Absyn_PointerType_struct*)_tmp30B)->f1;_LL3E0: _tmp312=(void*)
_tmp30D.elt_typ;goto _LL3DF;_LL3DF: _tmp311=(void*)_tmp30D.rgn_typ;goto _LL3DE;
_LL3DE: _tmp310=_tmp30D.nullable;goto _LL3DD;_LL3DD: _tmp30F=_tmp30D.tq;goto _LL3DC;
_LL3DC: _tmp30E=_tmp30D.bounds;goto _LL3D7;}else{goto _LL3D8;}_LL3D8: goto _LL3D9;
_LL3D5: Cyc_Toc_exp_to_c(nv,_tmp217);Cyc_Toc_exp_to_c(nv,_tmp216);{unsigned int i=
Cyc_Evexp_eval_const_uint_exp(_tmp216)+ 1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
_tmp217,Cyc_Absyn_fieldname((int)i),0))->r));goto _LL3D3;}_LL3D7: {struct Cyc_List_List*
_tmp313=Cyc_Toc_get_relns(_tmp217);int _tmp314=Cyc_Toc_check_bounds(_tmp313,
_tmp217,_tmp216);if(Cyc_Toc_warn_bounds_checks? ! _tmp314: 0){({struct Cyc_Std_String_pa_struct
_tmp316;_tmp316.tag=0;_tmp316.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e);{
void*_tmp315[1]={& _tmp316};Cyc_Tcutil_warn(e->loc,_tag_arr("bounds check necessary for %s",
sizeof(unsigned char),30),_tag_arr(_tmp315,sizeof(void*),1));}});}Cyc_Toc_exp_to_c(
nv,_tmp217);Cyc_Toc_exp_to_c(nv,_tmp216);Cyc_Toc_total_bounds_checks ++;{void*
_tmp317=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp30E);struct Cyc_Absyn_Exp*
_tmp318;_LL3E2: if((unsigned int)_tmp317 > 1?*((int*)_tmp317)== 0: 0){_LL3E6:
_tmp318=((struct Cyc_Absyn_Upper_b_struct*)_tmp317)->f1;goto _LL3E3;}else{goto
_LL3E4;}_LL3E4: if((int)_tmp317 == 0){goto _LL3E5;}else{goto _LL3E1;}_LL3E3: _tmp318=
Cyc_Absyn_uint_exp(Cyc_Evexp_eval_const_uint_exp(_tmp318),0);{int possibly_null=((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp310);void*ta1=
Cyc_Toc_typ_to_c(_tmp312);void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp30F);if(_tmp314){
Cyc_Toc_bounds_checks_eliminated ++;}else{if(possibly_null){(void*)(e->r=(void*)((
void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp319[4];_tmp319[3]=_tmp216;_tmp319[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp319[1]=_tmp318;_tmp319[0]=_tmp217;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp319,sizeof(struct Cyc_Absyn_Exp*),4));}),
0),0),0))->r));}else{(void*)(_tmp216->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp31A[2];_tmp31A[1]=Cyc_Absyn_copy_exp(_tmp216);_tmp31A[0]=
_tmp318;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp31A,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}goto _LL3E1;}_LL3E5: {void*
ta1=Cyc_Toc_typ_to_c_array(_tmp312);if(_tmp314){Cyc_Toc_bounds_checks_eliminated
++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
ta1,_tmp30F),Cyc_Absyn_aggrmember_exp(_tmp217,Cyc_Toc_curr_sp,0),0),_tmp216,0))->r));}
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
ta1,_tmp30F),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*
_tmp31B[3];_tmp31B[2]=_tmp216;_tmp31B[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp31B[
0]=_tmp217;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp31B,sizeof(struct Cyc_Absyn_Exp*),3));}),0),0),0))->r));}goto _LL3E1;}_LL3E1:;}
goto _LL3D3;}_LL3D9:({void*_tmp31C[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof(unsigned char),49),_tag_arr(_tmp31C,sizeof(void*),0));});_LL3D3:;}goto
_LL254;}_LL28C: if(! nv->toplevel){(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,0,0,_tmp218))->r));}else{struct Cyc_List_List*_tmp31D=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp218);struct _tagged_arr*_tmp31E=Cyc_Toc_add_tuple_type(
_tmp31D);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp218 != 0;(_tmp218=_tmp218->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp218->hd);dles=({struct Cyc_List_List*
_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F->hd=({struct _tuple4*_tmp320=
_cycalloc(sizeof(*_tmp320));_tmp320->f1=0;_tmp320->f2=(struct Cyc_Absyn_Exp*)
_tmp218->hd;_tmp320;});_tmp31F->tl=dles;_tmp31F;});}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,dles,0))->r));}goto _LL254;_LL28E:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,_tmp219,0))->r));{struct Cyc_List_List*_tmp321=_tmp219;for(0;_tmp321 != 0;
_tmp321=_tmp321->tl){struct _tuple4 _tmp323;struct Cyc_Absyn_Exp*_tmp324;struct
_tuple4*_tmp322=(struct _tuple4*)_tmp321->hd;_tmp323=*_tmp322;_LL3E8: _tmp324=
_tmp323.f2;goto _LL3E7;_LL3E7: Cyc_Toc_exp_to_c(nv,_tmp324);}}goto _LL254;_LL290: {
unsigned int _tmp325=Cyc_Evexp_eval_const_uint_exp(_tmp21B);Cyc_Toc_exp_to_c(nv,
_tmp21A);{struct Cyc_List_List*es=0;if(! Cyc_Toc_is_zero(_tmp21A)){unsigned int i=0;
for(0;i < _tmp325;i ++){es=({struct Cyc_List_List*_tmp326=_cycalloc(sizeof(*_tmp326));
_tmp326->hd=({struct _tuple4*_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327->f1=0;
_tmp327->f2=_tmp21A;_tmp327;});_tmp326->tl=es;_tmp326;});}}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));goto _LL254;}}_LL292: if(! nv->toplevel){(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,old_typ,0,0,_tmp21E,_tmp21F))->r));}
else{if(_tmp21D == 0){({void*_tmp328[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Struct_e: missing structdecl pointer",
sizeof(unsigned char),37),_tag_arr(_tmp328,sizeof(void*),0));});}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_check_null(_tmp21D);struct _RegionHandle _tmp329=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp329;_push_region(rgn);{struct Cyc_List_List*
_tmp32A=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,e->loc,_tmp21E,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sd2->fields))->v);
struct Cyc_List_List*_tmp32B=0;{struct Cyc_List_List*_tmp32C=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(sd2->fields))->v;for(0;_tmp32C != 0;_tmp32C=
_tmp32C->tl){struct Cyc_List_List*_tmp32D=_tmp32A;for(0;_tmp32D != 0;_tmp32D=
_tmp32D->tl){if((*((struct _tuple10*)_tmp32D->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)
_tmp32C->hd){struct _tuple10 _tmp32F;struct Cyc_Absyn_Exp*_tmp330;struct Cyc_Absyn_Aggrfield*
_tmp331;struct _tuple10*_tmp32E=(struct _tuple10*)_tmp32D->hd;_tmp32F=*_tmp32E;
_LL3EB: _tmp331=_tmp32F.f1;goto _LL3EA;_LL3EA: _tmp330=_tmp32F.f2;goto _LL3E9;_LL3E9: {
void*_tmp332=(void*)_tmp331->type;Cyc_Toc_exp_to_c(nv,_tmp330);if(Cyc_Toc_is_void_star(
_tmp332)){(void*)(_tmp330->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp330->r,0),0))->r));}_tmp32B=({struct Cyc_List_List*
_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333->hd=({struct _tuple4*_tmp334=
_cycalloc(sizeof(*_tmp334));_tmp334->f1=0;_tmp334->f2=_tmp330;_tmp334;});_tmp333->tl=
_tmp32B;_tmp333;});break;}}}}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp32B),0))->r));};
_pop_region(rgn);}}goto _LL254;_LL294: {struct Cyc_List_List*fs;{void*_tmp335=Cyc_Tcutil_compress(
_tmp221);struct Cyc_List_List*_tmp336;_LL3ED: if((unsigned int)_tmp335 > 3?*((int*)
_tmp335)== 11: 0){_LL3F1: _tmp336=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp335)->f2;
goto _LL3EE;}else{goto _LL3EF;}_LL3EF: goto _LL3F0;_LL3EE: fs=_tmp336;goto _LL3EC;
_LL3F0:({struct Cyc_Std_String_pa_struct _tmp338;_tmp338.tag=0;_tmp338.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(_tmp221);{void*_tmp337[1]={& _tmp338};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("anon struct has type %s",
sizeof(unsigned char),24),_tag_arr(_tmp337,sizeof(void*),1));}});_LL3EC:;}{
struct _RegionHandle _tmp339=_new_region("rgn");struct _RegionHandle*rgn=& _tmp339;
_push_region(rgn);{struct Cyc_List_List*_tmp33A=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp220,
fs);for(0;_tmp33A != 0;_tmp33A=_tmp33A->tl){struct _tuple10 _tmp33C;struct Cyc_Absyn_Exp*
_tmp33D;struct Cyc_Absyn_Aggrfield*_tmp33E;struct _tuple10*_tmp33B=(struct _tuple10*)
_tmp33A->hd;_tmp33C=*_tmp33B;_LL3F4: _tmp33E=_tmp33C.f1;goto _LL3F3;_LL3F3: _tmp33D=
_tmp33C.f2;goto _LL3F2;_LL3F2: {void*_tmp33F=(void*)_tmp33E->type;Cyc_Toc_exp_to_c(
nv,_tmp33D);if(Cyc_Toc_is_void_star(_tmp33F)){(void*)(_tmp33D->r=(void*)((void*)(
Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp33D->r,
0),0))->r));}}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp220,0))->r));};_pop_region(rgn);}goto _LL254;}_LL296: {struct _tuple0*qv=
_tmp222->name;struct Cyc_Absyn_Exp*tag_exp=_tmp223->is_xtunion? Cyc_Absyn_var_exp(
qv,0): Cyc_Toc_tunion_tag(_tmp223,qv,0);(void*)(e->r=(void*)((void*)tag_exp->r));
goto _LL254;}_LL298: {struct _tuple0*_tmp340=_tmp225->name;struct Cyc_List_List*
_tmp341=_tmp225->typs;struct _tuple0*_tmp342=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp343=Cyc_Absyn_var_exp(_tmp342,0);void*_tmp344=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp340,_tag_arr("_struct",sizeof(unsigned char),8)));struct Cyc_Absyn_Exp*
tag_exp=_tmp226->is_xtunion? Cyc_Absyn_var_exp(_tmp340,0): Cyc_Toc_tunion_tag(
_tmp226,_tmp340,1);if(nv->toplevel){struct Cyc_List_List*dles=0;for(0;_tmp227 != 0;(
_tmp227=_tmp227->tl,_tmp341=_tmp341->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp227->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple3*)((struct Cyc_List_List*)
_check_null(_tmp341))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ)){cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);}dles=({struct Cyc_List_List*
_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345->hd=({struct _tuple4*_tmp346=
_cycalloc(sizeof(*_tmp346));_tmp346->f1=0;_tmp346->f2=cur_e;_tmp346;});_tmp345->tl=
dles;_tmp345;});}dles=({struct Cyc_List_List*_tmp347=_cycalloc(sizeof(*_tmp347));
_tmp347->hd=({struct _tuple4*_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348->f1=0;
_tmp348->f2=tag_exp;_tmp348;});_tmp347->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp347;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt*_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349->v=Cyc_Toc_collapse_qvar_tag(
_tmp340,_tag_arr("_struct",sizeof(unsigned char),8));_tmp349;}),dles,0))->r));}
else{struct Cyc_List_List*_tmp34A=({struct Cyc_List_List*_tmp350=_cycalloc(sizeof(*
_tmp350));_tmp350->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp343,Cyc_Toc_tag_sp,
0),tag_exp,0);_tmp350->tl=0;_tmp350;});{int i=1;for(0;_tmp227 != 0;(((_tmp227=
_tmp227->tl,i ++)),_tmp341=_tmp341->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp227->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple3*)((struct Cyc_List_List*)
_check_null(_tmp341))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ)){cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);}{struct Cyc_Absyn_Stmt*
_tmp34B=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp343,Cyc_Absyn_fieldname(
i),0),cur_e,0);_tmp34A=({struct Cyc_List_List*_tmp34C=_cycalloc(sizeof(*_tmp34C));
_tmp34C->hd=_tmp34B;_tmp34C->tl=_tmp34A;_tmp34C;});}}}{struct Cyc_Absyn_Stmt*
_tmp34D=Cyc_Absyn_exp_stmt(_tmp343,0);struct Cyc_Absyn_Stmt*_tmp34E=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F->hd=_tmp34D;_tmp34F->tl=_tmp34A;
_tmp34F;})),0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp342,_tmp344,0,_tmp34E,0),0))->r));}}goto _LL254;}_LL29A: goto _LL29C;_LL29C:
goto _LL254;_LL29E: {void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp22B)));
Cyc_Toc_exp_to_c(nv,_tmp22A);if(_tmp229){struct _tuple0*_tmp351=Cyc_Toc_temp_var();
struct _tuple0*_tmp352=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*rexp;if(_tmp22D){xexp=_tmp22A;if(_tmp22C != 0){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp22C);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(
rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp351,0));}else{pexp=Cyc_Toc_calloc_exp(*
_tmp22B,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp351,0));}rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp353[3];_tmp353[2]=Cyc_Absyn_var_exp(
_tmp351,0);_tmp353[1]=Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp353[0]=Cyc_Absyn_var_exp(
_tmp352,0);((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp353,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp22A,0);if(_tmp22C != 0){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp22C);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(
rgn,Cyc_Absyn_var_exp(_tmp351,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp22B,Cyc_Absyn_var_exp(
_tmp351,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*
_tmp354[3];_tmp354[2]=Cyc_Absyn_var_exp(_tmp351,0);_tmp354[1]=Cyc_Absyn_uint_exp(
1,0);_tmp354[0]=Cyc_Absyn_var_exp(_tmp352,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp354,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}{struct Cyc_Absyn_Stmt*_tmp355=Cyc_Absyn_declare_stmt(_tmp351,Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp352,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp355,0))->r));}}else{if(_tmp22C != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp22C);Cyc_Toc_exp_to_c(
nv,rgn);(void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp22B,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL254;}_LL2A0: Cyc_Toc_stmt_to_c(nv,_tmp22E);goto _LL254;
_LL2A2:({void*_tmp356[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("UnresolvedMem",sizeof(unsigned char),14),_tag_arr(
_tmp356,sizeof(void*),0));});_LL2A4:({void*_tmp357[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("compoundlit",
sizeof(unsigned char),12),_tag_arr(_tmp357,sizeof(void*),0));});_LL2A6:({void*
_tmp358[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("codegen",sizeof(unsigned char),8),_tag_arr(_tmp358,sizeof(void*),0));});
_LL2A8:({void*_tmp359[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(_tag_arr("fill",sizeof(unsigned char),5),_tag_arr(_tmp359,sizeof(
void*),0));});_LL254:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct _tagged_arr*succ_lab,struct
_tagged_arr*fail_lab){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(e1,e2,0),
Cyc_Absyn_goto_stmt(succ_lab,0),Cyc_Absyn_goto_stmt(fail_lab,0),0);}struct
_tuple11{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple12{struct _tuple0*f1;void*f2;};struct _tuple13{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};static struct _tuple11 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct _tagged_arr*succ_lab,struct _tagged_arr*fail_lab,
struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{void*_tmp35A=(void*)p->r;
struct Cyc_Absyn_Vardecl*_tmp35B;struct Cyc_Absyn_Vardecl _tmp35C;struct _tuple0*
_tmp35D;struct Cyc_Absyn_Vardecl*_tmp35E;int _tmp35F;void*_tmp360;unsigned char
_tmp361;struct _tagged_arr _tmp362;struct Cyc_Absyn_Enumfield*_tmp363;struct Cyc_Absyn_Enumdecl*
_tmp364;struct Cyc_Absyn_Enumfield*_tmp365;void*_tmp366;struct Cyc_List_List*
_tmp367;struct Cyc_List_List*_tmp368;struct Cyc_Absyn_Tunionfield*_tmp369;struct
Cyc_Absyn_Tuniondecl*_tmp36A;struct Cyc_Absyn_Pat*_tmp36B;struct Cyc_Absyn_Pat
_tmp36C;void*_tmp36D;struct Cyc_List_List*_tmp36E;struct Cyc_List_List*_tmp36F;
struct Cyc_Absyn_Tunionfield*_tmp370;struct Cyc_Absyn_Tuniondecl*_tmp371;struct Cyc_List_List*
_tmp372;struct Cyc_List_List*_tmp373;struct Cyc_List_List*_tmp374;struct Cyc_Absyn_AggrInfo
_tmp375;void*_tmp376;struct Cyc_Absyn_Pat*_tmp377;_LL3F6: if((unsigned int)_tmp35A
> 2?*((int*)_tmp35A)== 0: 0){_LL418: _tmp35B=((struct Cyc_Absyn_Var_p_struct*)
_tmp35A)->f1;_tmp35C=*_tmp35B;_LL419: _tmp35D=_tmp35C.name;goto _LL3F7;}else{goto
_LL3F8;}_LL3F8: if((int)_tmp35A == 0){goto _LL3F9;}else{goto _LL3FA;}_LL3FA: if((
unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 6: 0){_LL41A: _tmp35E=((struct Cyc_Absyn_Reference_p_struct*)
_tmp35A)->f1;goto _LL3FB;}else{goto _LL3FC;}_LL3FC: if((int)_tmp35A == 1){goto _LL3FD;}
else{goto _LL3FE;}_LL3FE: if((unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 1: 0){
_LL41C: _tmp360=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp35A)->f1;goto _LL41B;
_LL41B: _tmp35F=((struct Cyc_Absyn_Int_p_struct*)_tmp35A)->f2;goto _LL3FF;}else{
goto _LL400;}_LL400: if((unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 2: 0){_LL41D:
_tmp361=((struct Cyc_Absyn_Char_p_struct*)_tmp35A)->f1;goto _LL401;}else{goto
_LL402;}_LL402: if((unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 3: 0){_LL41E:
_tmp362=((struct Cyc_Absyn_Float_p_struct*)_tmp35A)->f1;goto _LL403;}else{goto
_LL404;}_LL404: if((unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 9: 0){_LL420:
_tmp364=((struct Cyc_Absyn_Enum_p_struct*)_tmp35A)->f1;goto _LL41F;_LL41F: _tmp363=((
struct Cyc_Absyn_Enum_p_struct*)_tmp35A)->f2;goto _LL405;}else{goto _LL406;}_LL406:
if((unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 10: 0){_LL422: _tmp366=(void*)((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp35A)->f1;goto _LL421;_LL421: _tmp365=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp35A)->f2;goto _LL407;}else{goto _LL408;}
_LL408: if((unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 8: 0){_LL426: _tmp36A=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp35A)->f1;goto _LL425;_LL425: _tmp369=((struct
Cyc_Absyn_Tunion_p_struct*)_tmp35A)->f2;goto _LL424;_LL424: _tmp368=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp35A)->f3;goto _LL423;_LL423: _tmp367=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp35A)->f4;if(_tmp367 == 0){goto _LL409;}else{goto _LL40A;}}else{goto _LL40A;}
_LL40A: if((unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 5: 0){_LL428: _tmp36B=((
struct Cyc_Absyn_Pointer_p_struct*)_tmp35A)->f1;_tmp36C=*_tmp36B;_LL429: _tmp36D=(
void*)_tmp36C.r;if((unsigned int)_tmp36D > 2?*((int*)_tmp36D)== 8: 0){_LL42D:
_tmp371=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36D)->f1;goto _LL42C;_LL42C:
_tmp370=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36D)->f2;goto _LL42B;_LL42B:
_tmp36F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36D)->f3;goto _LL42A;_LL42A:
_tmp36E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36D)->f4;goto _LL427;}else{goto
_LL40C;}}else{goto _LL40C;}_LL427: if(_tmp36E != 0){goto _LL40B;}else{goto _LL40C;}
_LL40C: if((unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 8: 0){_LL42E: _tmp372=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp35A)->f4;goto _LL40D;}else{goto _LL40E;}
_LL40E: if((unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 4: 0){_LL42F: _tmp373=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp35A)->f1;goto _LL40F;}else{goto _LL410;}_LL410:
if((unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 7: 0){_LL431: _tmp375=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp35A)->f1;_LL432: _tmp376=(void*)_tmp375.aggr_info;goto _LL430;_LL430: _tmp374=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp35A)->f3;goto _LL411;}else{goto _LL412;}_LL412:
if((unsigned int)_tmp35A > 2?*((int*)_tmp35A)== 5: 0){_LL433: _tmp377=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp35A)->f1;goto _LL413;}else{goto _LL414;}_LL414: if((unsigned int)_tmp35A > 2?*((
int*)_tmp35A)== 11: 0){goto _LL415;}else{goto _LL416;}_LL416: if((unsigned int)
_tmp35A > 2?*((int*)_tmp35A)== 12: 0){goto _LL417;}else{goto _LL3F5;}_LL3F7: nv=Cyc_Toc_add_varmap(
nv,_tmp35D,r);goto _LL3F9;_LL3F9: s=Cyc_Absyn_goto_stmt(succ_lab,0);goto _LL3F5;
_LL3FB: {struct _tuple0*_tmp378=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp379=_region_malloc(rgn,sizeof(*_tmp379));_tmp379->hd=({struct _tuple12*
_tmp37A=_region_malloc(rgn,sizeof(*_tmp37A));_tmp37A->f1=_tmp378;_tmp37A->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp37A;});_tmp379->tl=decls;_tmp379;});nv=
Cyc_Toc_add_varmap(nv,_tmp35E->name,Cyc_Absyn_var_exp(_tmp378,0));s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp378,0),Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0),0),0),Cyc_Absyn_goto_stmt(
succ_lab,0),0);goto _LL3F5;}_LL3FD: s=Cyc_Toc_if_eq_goto_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),succ_lab,fail_lab);goto _LL3F5;_LL3FF: s=Cyc_Toc_if_eq_goto_stmt(r,Cyc_Absyn_int_exp(
_tmp360,_tmp35F,0),succ_lab,fail_lab);goto _LL3F5;_LL401: s=Cyc_Toc_if_eq_goto_stmt(
r,Cyc_Absyn_char_exp(_tmp361,0),succ_lab,fail_lab);goto _LL3F5;_LL403: s=Cyc_Toc_if_eq_goto_stmt(
r,Cyc_Absyn_float_exp(_tmp362,0),succ_lab,fail_lab);goto _LL3F5;_LL405: s=Cyc_Toc_if_eq_goto_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp37B=_cycalloc(
sizeof(*_tmp37B));_tmp37B[0]=({struct Cyc_Absyn_Enum_e_struct _tmp37C;_tmp37C.tag=
31;_tmp37C.f1=_tmp363->name;_tmp37C.f2=(struct Cyc_Absyn_Enumdecl*)_tmp364;
_tmp37C.f3=(struct Cyc_Absyn_Enumfield*)_tmp363;_tmp37C;});_tmp37B;}),0),succ_lab,
fail_lab);goto _LL3F5;_LL407: s=Cyc_Toc_if_eq_goto_stmt(r,Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AnonEnum_e_struct*_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D[0]=({
struct Cyc_Absyn_AnonEnum_e_struct _tmp37E;_tmp37E.tag=32;_tmp37E.f1=_tmp365->name;
_tmp37E.f2=(void*)_tmp366;_tmp37E.f3=(struct Cyc_Absyn_Enumfield*)_tmp365;_tmp37E;});
_tmp37D;}),0),succ_lab,fail_lab);goto _LL3F5;_LL409: {struct Cyc_Absyn_Exp*cmp_exp;
if(_tmp36A->is_xtunion){cmp_exp=Cyc_Absyn_var_exp(_tmp369->name,0);}else{cmp_exp=
Cyc_Toc_tunion_tag(_tmp36A,_tmp369->name,0);r=Cyc_Absyn_cast_exp(Cyc_Absyn_sint_t,
r,0);}s=Cyc_Toc_if_eq_goto_stmt(r,cmp_exp,succ_lab,fail_lab);goto _LL3F5;}_LL40B: {
struct Cyc_List_List*ss=0;int cnt=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp36E);struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp370->name,_tag_arr("_struct",
sizeof(unsigned char),8));struct Cyc_Absyn_Exp*rcast=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r,0);struct Cyc_List_List*_tmp37F=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
rgn,_tmp370->typs);struct Cyc_List_List*_tmp380=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(rgn,_tmp36E);for(0;_tmp380
!= 0;(((_tmp380=_tmp380->tl,_tmp37F=((struct Cyc_List_List*)_check_null(_tmp37F))->tl)),
cnt --)){struct Cyc_Absyn_Pat*_tmp381=(struct Cyc_Absyn_Pat*)_tmp380->hd;if((void*)
_tmp381->r == (void*)0){continue;}{void*_tmp382=(*((struct _tuple3*)((struct Cyc_List_List*)
_check_null(_tmp37F))->hd)).f2;struct _tuple0*_tmp383=Cyc_Toc_temp_var();void*
_tmp384=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp381->topt))->v;void*_tmp385=
Cyc_Toc_typ_to_c(_tmp384);struct _tagged_arr*_tmp386=Cyc_Toc_fresh_label();struct
Cyc_Absyn_Exp*_tmp387=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star(Cyc_Toc_typ_to_c(_tmp382))){_tmp387=Cyc_Absyn_cast_exp(
_tmp385,_tmp387,0);}decls=({struct Cyc_List_List*_tmp388=_region_malloc(rgn,
sizeof(*_tmp388));_tmp388->hd=({struct _tuple12*_tmp389=_region_malloc(rgn,
sizeof(*_tmp389));_tmp389->f1=_tmp383;_tmp389->f2=_tmp385;_tmp389;});_tmp388->tl=
decls;_tmp388;});{struct _tuple11 _tmp38A=Cyc_Toc_xlate_pat(nv,rgn,_tmp384,Cyc_Absyn_var_exp(
_tmp383,0),_tmp387,_tmp381,succ_lab,fail_lab,decls);nv=_tmp38A.f1;decls=_tmp38A.f2;{
struct Cyc_Absyn_Stmt*_tmp38B=_tmp38A.f3;struct Cyc_Absyn_Stmt*_tmp38C=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp383,0),_tmp387,0);struct Cyc_Absyn_Stmt*_tmp38D=Cyc_Absyn_seq_stmt(
_tmp38C,_tmp38B,0);ss=({struct Cyc_List_List*_tmp38E=_cycalloc(sizeof(*_tmp38E));
_tmp38E->hd=Cyc_Absyn_label_stmt(_tmp386,_tmp38D,0);_tmp38E->tl=ss;_tmp38E;});
succ_lab=_tmp386;}}}}if(ss == 0){ss=({struct Cyc_List_List*_tmp38F=_cycalloc(
sizeof(*_tmp38F));_tmp38F->hd=Cyc_Absyn_goto_stmt(succ_lab,0);_tmp38F->tl=0;
_tmp38F;});}{struct Cyc_Absyn_Exp*test_exp;if(_tmp371->is_xtunion){struct Cyc_Absyn_Exp*
e2=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),
r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp370->name,0);test_exp=Cyc_Absyn_eq_exp(e1,e,0);s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(e1,e,0),Cyc_Absyn_seq_stmts(ss,0),Cyc_Absyn_goto_stmt(fail_lab,
0),0);}else{int max_tag=Cyc_Toc_num_void_tags(_tmp371);struct Cyc_Absyn_Exp*
max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);struct Cyc_Absyn_Exp*e3=
Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_t,Cyc_Toc_mt_tq),r,0);
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*e5=Cyc_Absyn_gt_exp(
Cyc_Absyn_cast_exp(Cyc_Absyn_uint_t,r,0),max_tag_exp,0);struct Cyc_Absyn_Exp*e=
Cyc_Toc_tunion_tag(_tmp371,_tmp370->name,1);test_exp=max_tag > 0? Cyc_Absyn_and_exp(
e5,Cyc_Absyn_eq_exp(e1,e,0),0): Cyc_Absyn_eq_exp(e1,e,0);}s=Cyc_Absyn_ifthenelse_stmt(
test_exp,Cyc_Absyn_seq_stmts(ss,0),Cyc_Absyn_goto_stmt(fail_lab,0),0);goto _LL3F5;}}
_LL40D: _tmp373=_tmp372;goto _LL40F;_LL40F: {struct Cyc_List_List*ss=0;int cnt=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp373);{struct Cyc_List_List*_tmp390=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
rgn,_tmp373);for(0;_tmp390 != 0;(_tmp390=_tmp390->tl,cnt --)){struct Cyc_Absyn_Pat*
_tmp391=(struct Cyc_Absyn_Pat*)_tmp390->hd;if((void*)_tmp391->r == (void*)0){
continue;}{struct _tuple0*_tmp392=Cyc_Toc_temp_var();void*_tmp393=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp391->topt))->v;struct _tagged_arr*_tmp394=Cyc_Toc_fresh_label();
decls=({struct Cyc_List_List*_tmp395=_region_malloc(rgn,sizeof(*_tmp395));_tmp395->hd=({
struct _tuple12*_tmp396=_region_malloc(rgn,sizeof(*_tmp396));_tmp396->f1=_tmp392;
_tmp396->f2=Cyc_Toc_typ_to_c(_tmp393);_tmp396;});_tmp395->tl=decls;_tmp395;});{
struct _tuple11 _tmp397=Cyc_Toc_xlate_pat(nv,rgn,_tmp393,Cyc_Absyn_var_exp(_tmp392,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp391,succ_lab,
fail_lab,decls);nv=_tmp397.f1;decls=_tmp397.f2;{struct Cyc_Absyn_Stmt*_tmp398=
_tmp397.f3;struct Cyc_Absyn_Stmt*_tmp399=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp392,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),
0),0),_tmp398,0);ss=({struct Cyc_List_List*_tmp39A=_cycalloc(sizeof(*_tmp39A));
_tmp39A->hd=Cyc_Absyn_label_stmt(_tmp394,_tmp399,0);_tmp39A->tl=ss;_tmp39A;});
succ_lab=_tmp394;}}}}}if(ss == 0){ss=({struct Cyc_List_List*_tmp39B=_cycalloc(
sizeof(*_tmp39B));_tmp39B->hd=Cyc_Absyn_goto_stmt(succ_lab,0);_tmp39B->tl=0;
_tmp39B;});}s=Cyc_Absyn_seq_stmts(ss,0);goto _LL3F5;}_LL411: {struct Cyc_Absyn_Aggrdecl*
_tmp39C=Cyc_Absyn_get_known_aggrdecl(_tmp376);struct Cyc_List_List*ss=0;{struct
Cyc_List_List*_tmp39D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(rgn,_tmp374);for(0;_tmp39D != 0;_tmp39D=_tmp39D->tl){struct
_tuple13*_tmp39E=(struct _tuple13*)_tmp39D->hd;struct Cyc_Absyn_Pat*_tmp39F=(*
_tmp39E).f2;if((void*)_tmp39F->r == (void*)0){continue;}{struct _tagged_arr*f;{
void*_tmp3A0=(void*)((struct Cyc_List_List*)_check_null((*_tmp39E).f1))->hd;
struct _tagged_arr*_tmp3A1;_LL435: if(*((int*)_tmp3A0)== 1){_LL439: _tmp3A1=((
struct Cyc_Absyn_FieldName_struct*)_tmp3A0)->f1;goto _LL436;}else{goto _LL437;}
_LL437: goto _LL438;_LL436: f=_tmp3A1;goto _LL434;_LL438:(int)_throw((void*)Cyc_Toc_Match_error);
_LL434:;}{struct _tuple0*_tmp3A2=Cyc_Toc_temp_var();void*_tmp3A3=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp39F->topt))->v;void*_tmp3A4=Cyc_Toc_typ_to_c(
_tmp3A3);struct _tagged_arr*_tmp3A5=Cyc_Toc_fresh_label();decls=({struct Cyc_List_List*
_tmp3A6=_region_malloc(rgn,sizeof(*_tmp3A6));_tmp3A6->hd=({struct _tuple12*
_tmp3A7=_region_malloc(rgn,sizeof(*_tmp3A7));_tmp3A7->f1=_tmp3A2;_tmp3A7->f2=
_tmp3A4;_tmp3A7;});_tmp3A6->tl=decls;_tmp3A6;});{struct _tuple11 _tmp3A8=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3A3,Cyc_Absyn_var_exp(_tmp3A2,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp39F,succ_lab,fail_lab,decls);nv=_tmp3A8.f1;decls=_tmp3A8.f2;{struct Cyc_Absyn_Exp*
_tmp3A9=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((void*)((struct
Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp39C->fields))->v,f)))->type)){_tmp3A9=Cyc_Absyn_cast_exp(
_tmp3A4,_tmp3A9,0);}{struct Cyc_Absyn_Stmt*_tmp3AA=_tmp3A8.f3;struct Cyc_Absyn_Stmt*
_tmp3AB=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp3A2,0),
_tmp3A9,0),_tmp3AA,0);ss=({struct Cyc_List_List*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));
_tmp3AC->hd=Cyc_Absyn_label_stmt(_tmp3A5,_tmp3AB,0);_tmp3AC->tl=ss;_tmp3AC;});
succ_lab=_tmp3A5;}}}}}}}if(ss == 0){ss=({struct Cyc_List_List*_tmp3AD=_cycalloc(
sizeof(*_tmp3AD));_tmp3AD->hd=Cyc_Absyn_goto_stmt(succ_lab,0);_tmp3AD->tl=0;
_tmp3AD;});}s=Cyc_Absyn_seq_stmts(ss,0);goto _LL3F5;}_LL413: {struct _tuple0*
_tmp3AE=Cyc_Toc_temp_var();void*_tmp3AF=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp377->topt))->v;decls=({struct Cyc_List_List*_tmp3B0=_region_malloc(rgn,
sizeof(*_tmp3B0));_tmp3B0->hd=({struct _tuple12*_tmp3B1=_region_malloc(rgn,
sizeof(*_tmp3B1));_tmp3B1->f1=_tmp3AE;_tmp3B1->f2=Cyc_Toc_typ_to_c(_tmp3AF);
_tmp3B1;});_tmp3B0->tl=decls;_tmp3B0;});{struct _tuple11 _tmp3B2=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3AF,Cyc_Absyn_var_exp(_tmp3AE,0),Cyc_Absyn_deref_exp(path,0),_tmp377,
succ_lab,fail_lab,decls);nv=_tmp3B2.f1;decls=_tmp3B2.f2;{struct Cyc_Absyn_Stmt*
_tmp3B3=_tmp3B2.f3;struct Cyc_Absyn_Stmt*_tmp3B4=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp3AE,0),Cyc_Absyn_deref_exp(r,0),0),_tmp3B3,0);if(Cyc_Toc_is_nullable(
t)){s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),
0),Cyc_Absyn_goto_stmt(fail_lab,0),_tmp3B4,0);}else{s=_tmp3B4;}goto _LL3F5;}}}
_LL415:({void*_tmp3B5[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("unknownid",sizeof(unsigned char),10),_tag_arr(
_tmp3B5,sizeof(void*),0));});_LL417:({void*_tmp3B6[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unknowncall",
sizeof(unsigned char),12),_tag_arr(_tmp3B6,sizeof(void*),0));});_LL3F5:;}return({
struct _tuple11 _tmp3B7;_tmp3B7.f1=nv;_tmp3B7.f2=decls;_tmp3B7.f3=s;_tmp3B7;});}
struct _tuple14{struct _tagged_arr*f1;struct _tagged_arr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple14*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple14*_tmp3B8=_region_malloc(r,sizeof(*_tmp3B8));_tmp3B8->f1=
Cyc_Toc_fresh_label();_tmp3B8->f2=Cyc_Toc_fresh_label();_tmp3B8->f3=sc;_tmp3B8;});}
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*
_tmp3B9=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int leave_as_switch;{
void*_tmp3BA=Cyc_Tcutil_compress(_tmp3B9);_LL43B: if((unsigned int)_tmp3BA > 3?*((
int*)_tmp3BA)== 5: 0){goto _LL43C;}else{goto _LL43D;}_LL43D: if((unsigned int)
_tmp3BA > 3?*((int*)_tmp3BA)== 12: 0){goto _LL43E;}else{goto _LL43F;}_LL43F: goto
_LL440;_LL43C: goto _LL43E;_LL43E: leave_as_switch=1;goto _LL43A;_LL440:
leave_as_switch=0;goto _LL43A;_LL43A:;}{struct Cyc_List_List*_tmp3BB=scs;for(0;
_tmp3BB != 0;_tmp3BB=_tmp3BB->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp3BB->hd)->pat_vars))->v != 0? 1:((
struct Cyc_Absyn_Switch_clause*)_tmp3BB->hd)->where_clause != 0){leave_as_switch=0;
break;}}}if(leave_as_switch){struct _tagged_arr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp3BC=scs;for(0;_tmp3BC != 0;_tmp3BC=_tmp3BC->tl){struct Cyc_Absyn_Stmt*
_tmp3BD=((struct Cyc_Absyn_Switch_clause*)_tmp3BC->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp3BC->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp3BD,0);next_l=Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(nv,next_l),_tmp3BD);}}return;}{
struct _tuple0*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct _tagged_arr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp3BE=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3BE;
_push_region(rgn);{struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple14*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,
rgn,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp3BF=lscs;for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple14*)_tmp3BF->hd)).f3;struct
_tagged_arr*fail_lab=_tmp3BF->tl == 0? end_l:(*((struct _tuple14*)((struct Cyc_List_List*)
_check_null(_tmp3BF->tl))->hd)).f1;struct _tagged_arr*succ_lab=(*((struct _tuple14*)
_tmp3BF->hd)).f2;if(sc->where_clause == 0){struct _tuple11 _tmp3C0=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3B9,r,path,sc->pattern,succ_lab,fail_lab,decls);nvs=({struct Cyc_List_List*
_tmp3C1=_region_malloc(rgn,sizeof(*_tmp3C1));_tmp3C1->hd=_tmp3C0.f1;_tmp3C1->tl=
nvs;_tmp3C1;});decls=_tmp3C0.f2;test_stmts=({struct Cyc_List_List*_tmp3C2=
_region_malloc(rgn,sizeof(*_tmp3C2));_tmp3C2->hd=Cyc_Absyn_label_stmt((*((struct
_tuple14*)_tmp3BF->hd)).f1,_tmp3C0.f3,0);_tmp3C2->tl=test_stmts;_tmp3C2;});}
else{struct Cyc_Absyn_Exp*_tmp3C3=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
struct _tagged_arr*_tmp3C4=Cyc_Toc_fresh_label();struct _tuple11 _tmp3C5=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3B9,r,path,sc->pattern,_tmp3C4,fail_lab,decls);Cyc_Toc_exp_to_c(
_tmp3C5.f1,_tmp3C3);nvs=({struct Cyc_List_List*_tmp3C6=_region_malloc(rgn,sizeof(*
_tmp3C6));_tmp3C6->hd=_tmp3C5.f1;_tmp3C6->tl=nvs;_tmp3C6;});decls=_tmp3C5.f2;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp3C3,Cyc_Absyn_goto_stmt(
succ_lab,0),Cyc_Absyn_goto_stmt(fail_lab,0),0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_label_stmt(
_tmp3C4,s2,0);test_stmts=({struct Cyc_List_List*_tmp3C7=_region_malloc(rgn,
sizeof(*_tmp3C7));_tmp3C7->hd=Cyc_Absyn_label_stmt((*((struct _tuple14*)_tmp3BF->hd)).f1,
Cyc_Absyn_seq_stmt(_tmp3C5.f3,s3,0),0);_tmp3C7->tl=test_stmts;_tmp3C7;});}}}}{
struct Cyc_Absyn_Stmt*_tmp3C8=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts),0);nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);{struct Cyc_Absyn_Stmt*clauses=Cyc_Absyn_skip_stmt(
0);if(lscs != 0){for(0;lscs->tl != 0;(lscs=lscs->tl,nvs=nvs->tl)){struct Cyc_Toc_Env*
_tmp3C9=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;struct
Cyc_Absyn_Stmt*s=((*((struct _tuple14*)lscs->hd)).f3)->body;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp3C9,end_l,(*((struct _tuple14*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd)).f2,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((*((struct _tuple14*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd)).f3)->pat_vars))->v,(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);clauses=Cyc_Absyn_seq_stmt(
clauses,Cyc_Absyn_label_stmt((*((struct _tuple14*)lscs->hd)).f2,s,0),0);}{struct
Cyc_Toc_Env*_tmp3CA=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=((*((struct _tuple14*)lscs->hd)).f3)->body;Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env(_tmp3CA,end_l),s);clauses=Cyc_Absyn_seq_stmt(
clauses,Cyc_Absyn_label_stmt((*((struct _tuple14*)lscs->hd)).f2,s,0),0);}}{struct
Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(_tmp3C8,Cyc_Absyn_seq_stmt(clauses,Cyc_Absyn_label_stmt(
end_l,Cyc_Absyn_skip_stmt(0),0),0),0);for(decls;decls != 0;decls=decls->tl){
struct _tuple12 _tmp3CC;void*_tmp3CD;struct _tuple0*_tmp3CE;struct _tuple12*_tmp3CB=(
struct _tuple12*)((struct Cyc_List_List*)_check_null(decls))->hd;_tmp3CC=*_tmp3CB;
_LL443: _tmp3CE=_tmp3CC.f1;goto _LL442;_LL442: _tmp3CD=_tmp3CC.f2;goto _LL441;_LL441:
res=Cyc_Absyn_declare_stmt(_tmp3CE,_tmp3CD,0,res,0);}(void*)(whole_s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r));}}}};_pop_region(
rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,struct Cyc_Core_Opt*topt,struct Cyc_Absyn_Exp*e,int exhaust,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp3CF=
_cycalloc(sizeof(*_tmp3CF));_tmp3CF->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp3CF->tl=0;_tmp3CF;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0){(void*)(s->r=(void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp3D0=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_Absyn_Stmt*_tmp3D2;struct Cyc_Absyn_Stmt*
_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D4;struct Cyc_Absyn_Stmt*_tmp3D5;struct Cyc_Absyn_Stmt*
_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D7;struct Cyc_Absyn_Stmt*_tmp3D8;struct _tuple2
_tmp3D9;struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_Absyn_Stmt*_tmp3DB;struct Cyc_Absyn_Stmt*
_tmp3DC;struct Cyc_Absyn_Stmt*_tmp3DD;struct Cyc_Absyn_Stmt*_tmp3DE;struct _tuple2
_tmp3DF;struct Cyc_Absyn_Exp*_tmp3E0;struct _tuple2 _tmp3E1;struct Cyc_Absyn_Exp*
_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E3;struct Cyc_List_List*_tmp3E4;struct Cyc_Absyn_Exp*
_tmp3E5;struct Cyc_Absyn_Switch_clause**_tmp3E6;struct Cyc_List_List*_tmp3E7;
struct Cyc_Absyn_Stmt*_tmp3E8;struct Cyc_Absyn_Decl*_tmp3E9;struct Cyc_Absyn_Stmt*
_tmp3EA;struct _tagged_arr*_tmp3EB;struct _tuple2 _tmp3EC;struct Cyc_Absyn_Exp*
_tmp3ED;struct Cyc_Absyn_Stmt*_tmp3EE;struct Cyc_List_List*_tmp3EF;struct Cyc_Absyn_Stmt*
_tmp3F0;struct Cyc_Absyn_Stmt*_tmp3F1;struct Cyc_Absyn_Vardecl*_tmp3F2;struct Cyc_Absyn_Tvar*
_tmp3F3;_LL445: if((int)_tmp3D0 == 0){goto _LL446;}else{goto _LL447;}_LL447: if((
unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 0: 0){_LL46F: _tmp3D1=((struct Cyc_Absyn_Exp_s_struct*)
_tmp3D0)->f1;goto _LL448;}else{goto _LL449;}_LL449: if((unsigned int)_tmp3D0 > 1?*((
int*)_tmp3D0)== 1: 0){_LL471: _tmp3D3=((struct Cyc_Absyn_Seq_s_struct*)_tmp3D0)->f1;
goto _LL470;_LL470: _tmp3D2=((struct Cyc_Absyn_Seq_s_struct*)_tmp3D0)->f2;goto
_LL44A;}else{goto _LL44B;}_LL44B: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 2:
0){_LL472: _tmp3D4=((struct Cyc_Absyn_Return_s_struct*)_tmp3D0)->f1;goto _LL44C;}
else{goto _LL44D;}_LL44D: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 3: 0){
_LL475: _tmp3D7=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D0)->f1;goto _LL474;
_LL474: _tmp3D6=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D0)->f2;goto _LL473;
_LL473: _tmp3D5=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D0)->f3;goto _LL44E;}
else{goto _LL44F;}_LL44F: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 4: 0){
_LL477: _tmp3D9=((struct Cyc_Absyn_While_s_struct*)_tmp3D0)->f1;_LL478: _tmp3DA=
_tmp3D9.f1;goto _LL476;_LL476: _tmp3D8=((struct Cyc_Absyn_While_s_struct*)_tmp3D0)->f2;
goto _LL450;}else{goto _LL451;}_LL451: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)
== 5: 0){_LL479: _tmp3DB=((struct Cyc_Absyn_Break_s_struct*)_tmp3D0)->f1;goto _LL452;}
else{goto _LL453;}_LL453: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 6: 0){
_LL47A: _tmp3DC=((struct Cyc_Absyn_Continue_s_struct*)_tmp3D0)->f1;goto _LL454;}
else{goto _LL455;}_LL455: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 7: 0){
_LL47B: _tmp3DD=((struct Cyc_Absyn_Goto_s_struct*)_tmp3D0)->f2;goto _LL456;}else{
goto _LL457;}_LL457: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 8: 0){_LL481:
_tmp3E3=((struct Cyc_Absyn_For_s_struct*)_tmp3D0)->f1;goto _LL47F;_LL47F: _tmp3E1=((
struct Cyc_Absyn_For_s_struct*)_tmp3D0)->f2;_LL480: _tmp3E2=_tmp3E1.f1;goto _LL47D;
_LL47D: _tmp3DF=((struct Cyc_Absyn_For_s_struct*)_tmp3D0)->f3;_LL47E: _tmp3E0=
_tmp3DF.f1;goto _LL47C;_LL47C: _tmp3DE=((struct Cyc_Absyn_For_s_struct*)_tmp3D0)->f4;
goto _LL458;}else{goto _LL459;}_LL459: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)
== 19: 0){goto _LL45A;}else{goto _LL45B;}_LL45B: if((unsigned int)_tmp3D0 > 1?*((int*)
_tmp3D0)== 9: 0){_LL483: _tmp3E5=((struct Cyc_Absyn_Switch_s_struct*)_tmp3D0)->f1;
goto _LL482;_LL482: _tmp3E4=((struct Cyc_Absyn_Switch_s_struct*)_tmp3D0)->f2;goto
_LL45C;}else{goto _LL45D;}_LL45D: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 
11: 0){_LL485: _tmp3E7=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp3D0)->f1;goto
_LL484;_LL484: _tmp3E6=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp3D0)->f2;goto
_LL45E;}else{goto _LL45F;}_LL45F: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 
12: 0){_LL487: _tmp3E9=((struct Cyc_Absyn_Decl_s_struct*)_tmp3D0)->f1;goto _LL486;
_LL486: _tmp3E8=((struct Cyc_Absyn_Decl_s_struct*)_tmp3D0)->f2;goto _LL460;}else{
goto _LL461;}_LL461: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 15: 0){_LL489:
_tmp3EB=((struct Cyc_Absyn_Label_s_struct*)_tmp3D0)->f1;goto _LL488;_LL488: _tmp3EA=((
struct Cyc_Absyn_Label_s_struct*)_tmp3D0)->f2;goto _LL462;}else{goto _LL463;}_LL463:
if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 16: 0){_LL48C: _tmp3EE=((struct Cyc_Absyn_Do_s_struct*)
_tmp3D0)->f1;goto _LL48A;_LL48A: _tmp3EC=((struct Cyc_Absyn_Do_s_struct*)_tmp3D0)->f2;
_LL48B: _tmp3ED=_tmp3EC.f1;goto _LL464;}else{goto _LL465;}_LL465: if((unsigned int)
_tmp3D0 > 1?*((int*)_tmp3D0)== 17: 0){_LL48E: _tmp3F0=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3D0)->f1;goto _LL48D;_LL48D: _tmp3EF=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3D0)->f2;goto _LL466;}else{goto _LL467;}_LL467: if((unsigned int)_tmp3D0 > 1?*((
int*)_tmp3D0)== 18: 0){_LL491: _tmp3F3=((struct Cyc_Absyn_Region_s_struct*)_tmp3D0)->f1;
goto _LL490;_LL490: _tmp3F2=((struct Cyc_Absyn_Region_s_struct*)_tmp3D0)->f2;goto
_LL48F;_LL48F: _tmp3F1=((struct Cyc_Absyn_Region_s_struct*)_tmp3D0)->f3;goto _LL468;}
else{goto _LL469;}_LL469: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== 10: 0){
goto _LL46A;}else{goto _LL46B;}_LL46B: if((unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)
== 13: 0){goto _LL46C;}else{goto _LL46D;}_LL46D: if((unsigned int)_tmp3D0 > 1?*((int*)
_tmp3D0)== 14: 0){goto _LL46E;}else{goto _LL444;}_LL446: return;_LL448: Cyc_Toc_exp_to_c(
nv,_tmp3D1);return;_LL44A: Cyc_Toc_stmt_to_c(nv,_tmp3D3);s=_tmp3D2;continue;
_LL44C: {struct Cyc_Core_Opt*topt=0;if(_tmp3D4 != 0){topt=({struct Cyc_Core_Opt*
_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3D4->topt))->v);_tmp3F4;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp3D4));}if(s->try_depth > 0){if(topt != 0){
struct _tuple0*_tmp3F5=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp3F6=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp3F5,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp3F5,(void*)topt->v,_tmp3D4,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp3F6,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL44E: Cyc_Toc_exp_to_c(nv,_tmp3D7);Cyc_Toc_stmt_to_c(nv,_tmp3D6);s=
_tmp3D5;continue;_LL450: Cyc_Toc_exp_to_c(nv,_tmp3DA);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp3D8);return;_LL452: if(nv->break_lab != 0){(void*)(s->r=(void*)((void*)(
Cyc_Absyn_goto_stmt((struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,
0))->r));}{int dest_depth=_tmp3DB == 0? 0: _tmp3DB->try_depth;Cyc_Toc_do_npop_before(
s->try_depth - dest_depth,s);return;}_LL454: if(nv->continue_lab != 0){(void*)(s->r=(
void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((struct Cyc_Core_Opt*)
_check_null(nv->continue_lab))->v,0))->r));}_tmp3DD=_tmp3DC;goto _LL456;_LL456:
Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp3DD))->try_depth,
s);return;_LL458: Cyc_Toc_exp_to_c(nv,_tmp3E3);Cyc_Toc_exp_to_c(nv,_tmp3E2);Cyc_Toc_exp_to_c(
nv,_tmp3E0);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp3DE);return;_LL45A:({void*
_tmp3F7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("forarray not yet implemented",sizeof(unsigned char),29),_tag_arr(
_tmp3F7,sizeof(void*),0));});return;_LL45C: Cyc_Toc_xlate_switch(nv,s,_tmp3E5,
_tmp3E4);return;_LL45E: if(nv->fallthru_info == 0){({void*_tmp3F8[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("fallthru in unexpected place",
sizeof(unsigned char),29),_tag_arr(_tmp3F8,sizeof(void*),0));});}{struct _tuple6
_tmp3FA;struct Cyc_Dict_Dict*_tmp3FB;struct Cyc_List_List*_tmp3FC;struct
_tagged_arr*_tmp3FD;struct _tuple6*_tmp3F9=(struct _tuple6*)((struct Cyc_Core_Opt*)
_check_null(nv->fallthru_info))->v;_tmp3FA=*_tmp3F9;_LL495: _tmp3FD=_tmp3FA.f1;
goto _LL494;_LL494: _tmp3FC=_tmp3FA.f2;goto _LL493;_LL493: _tmp3FB=_tmp3FA.f3;goto
_LL492;_LL492: {struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp3FD,0);Cyc_Toc_do_npop_before(
s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp3E6)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp3FE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp3FC);struct Cyc_List_List*_tmp3FF=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp3E7);for(0;_tmp3FE != 0;(_tmp3FE=_tmp3FE->tl,
_tmp3FF=_tmp3FF->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3FF))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)(
_tmp3FB,(struct _tuple0*)_tmp3FE->hd),(struct Cyc_Absyn_Exp*)_tmp3FF->hd,0),s2,0);}(
void*)(s->r=(void*)((void*)s2->r));return;}}}_LL460:{void*_tmp400=(void*)_tmp3E9->r;
struct Cyc_Absyn_Vardecl*_tmp401;int _tmp402;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Core_Opt*
_tmp404;struct Cyc_Absyn_Pat*_tmp405;struct Cyc_List_List*_tmp406;_LL497: if(*((int*)
_tmp400)== 0){_LL49F: _tmp401=((struct Cyc_Absyn_Var_d_struct*)_tmp400)->f1;goto
_LL498;}else{goto _LL499;}_LL499: if(*((int*)_tmp400)== 2){_LL4A3: _tmp405=((struct
Cyc_Absyn_Let_d_struct*)_tmp400)->f1;goto _LL4A2;_LL4A2: _tmp404=((struct Cyc_Absyn_Let_d_struct*)
_tmp400)->f3;goto _LL4A1;_LL4A1: _tmp403=((struct Cyc_Absyn_Let_d_struct*)_tmp400)->f4;
goto _LL4A0;_LL4A0: _tmp402=((struct Cyc_Absyn_Let_d_struct*)_tmp400)->f5;goto
_LL49A;}else{goto _LL49B;}_LL49B: if(*((int*)_tmp400)== 3){_LL4A4: _tmp406=((struct
Cyc_Absyn_Letv_d_struct*)_tmp400)->f1;goto _LL49C;}else{goto _LL49D;}_LL49D: goto
_LL49E;_LL498: {struct Cyc_Toc_Env*_tmp407=Cyc_Toc_add_varmap(nv,_tmp401->name,
Cyc_Absyn_varb_exp(_tmp401->name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp408=
_cycalloc(sizeof(*_tmp408));_tmp408[0]=({struct Cyc_Absyn_Local_b_struct _tmp409;
_tmp409.tag=3;_tmp409.f1=_tmp401;_tmp409;});_tmp408;}),0));Cyc_Toc_local_decl_to_c(
_tmp407,_tmp407,_tmp401,_tmp3E8);goto _LL496;}_LL49A:{void*_tmp40A=(void*)_tmp405->r;
struct Cyc_Absyn_Vardecl*_tmp40B;_LL4A6: if((unsigned int)_tmp40A > 2?*((int*)
_tmp40A)== 0: 0){_LL4AA: _tmp40B=((struct Cyc_Absyn_Var_p_struct*)_tmp40A)->f1;goto
_LL4A7;}else{goto _LL4A8;}_LL4A8: goto _LL4A9;_LL4A7: {struct _tuple0*old_name=
_tmp40B->name;struct _tuple0*new_name=Cyc_Toc_temp_var();_tmp40B->name=new_name;
_tmp40B->initializer=(struct Cyc_Absyn_Exp*)_tmp403;(void*)(_tmp3E9->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C[
0]=({struct Cyc_Absyn_Var_d_struct _tmp40D;_tmp40D.tag=0;_tmp40D.f1=_tmp40B;
_tmp40D;});_tmp40C;})));{struct Cyc_Toc_Env*_tmp40E=Cyc_Toc_add_varmap(nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F[0]=({struct Cyc_Absyn_Local_b_struct
_tmp410;_tmp410.tag=3;_tmp410.f1=_tmp40B;_tmp410;});_tmp40F;}),0));Cyc_Toc_local_decl_to_c(
_tmp40E,nv,_tmp40B,_tmp3E8);goto _LL4A5;}}_LL4A9:(void*)(s->r=(void*)((void*)(Cyc_Toc_letdecl_to_c(
nv,_tmp405,_tmp404,_tmp403,_tmp402,_tmp3E8))->r));goto _LL4A5;_LL4A5:;}goto _LL496;
_LL49C: {struct Cyc_List_List*_tmp411=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp406);if(_tmp411 == 0){({void*_tmp412[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("empty Letv_d",
sizeof(unsigned char),13),_tag_arr(_tmp412,sizeof(void*),0));});}(void*)(_tmp3E9->r=(
void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp413=_cycalloc(sizeof(*_tmp413));
_tmp413[0]=({struct Cyc_Absyn_Var_d_struct _tmp414;_tmp414.tag=0;_tmp414.f1=(
struct Cyc_Absyn_Vardecl*)_tmp411->hd;_tmp414;});_tmp413;})));_tmp411=_tmp411->tl;
for(0;_tmp411 != 0;_tmp411=_tmp411->tl){struct Cyc_Absyn_Decl*_tmp415=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416[
0]=({struct Cyc_Absyn_Var_d_struct _tmp417;_tmp417.tag=0;_tmp417.f1=(struct Cyc_Absyn_Vardecl*)
_tmp411->hd;_tmp417;});_tmp416;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp415,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL496;}_LL49E:({void*_tmp418[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad nested declaration within function",
sizeof(unsigned char),39),_tag_arr(_tmp418,sizeof(void*),0));});goto _LL496;
_LL496:;}return;_LL462: s=_tmp3EA;continue;_LL464: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp3EE);Cyc_Toc_exp_to_c(nv,_tmp3ED);return;_LL466: {struct _tuple0*h_var=
Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_t);e_exp->topt=({struct Cyc_Core_Opt*
_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419->v=(void*)e_typ;_tmp419;});nv=Cyc_Toc_add_varmap(
nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp3F0);{struct Cyc_Absyn_Stmt*_tmp41A=Cyc_Absyn_seq_stmt(
_tmp3F0,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple0*_tmp41B=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp41C=Cyc_Absyn_var_exp(
_tmp41B,0);struct Cyc_Absyn_Vardecl*_tmp41D=Cyc_Absyn_new_vardecl(_tmp41B,Cyc_Absyn_exn_typ,
0);_tmp41C->topt=({struct Cyc_Core_Opt*_tmp41E=_cycalloc(sizeof(*_tmp41E));
_tmp41E->v=(void*)Cyc_Absyn_exn_typ;_tmp41E;});{struct Cyc_Absyn_Pat*_tmp41F=({
struct Cyc_Absyn_Pat*_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431[0]=({
struct Cyc_Absyn_Var_p_struct _tmp432;_tmp432.tag=0;_tmp432.f1=_tmp41D;_tmp432;});
_tmp431;}));_tmp42F->topt=({struct Cyc_Core_Opt*_tmp430=_cycalloc(sizeof(*_tmp430));
_tmp430->v=(void*)Cyc_Absyn_exn_typ;_tmp430;});_tmp42F->loc=0;_tmp42F;});struct
Cyc_Absyn_Exp*_tmp420=Cyc_Absyn_throw_exp(_tmp41C,0);_tmp420->topt=({struct Cyc_Core_Opt*
_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->v=(void*)((void*)0);_tmp421;});{
struct Cyc_Absyn_Stmt*_tmp422=Cyc_Absyn_exp_stmt(_tmp420,0);struct Cyc_Absyn_Switch_clause*
_tmp423=({struct Cyc_Absyn_Switch_clause*_tmp42C=_cycalloc(sizeof(*_tmp42C));
_tmp42C->pattern=_tmp41F;_tmp42C->pat_vars=({struct Cyc_Core_Opt*_tmp42D=
_cycalloc(sizeof(*_tmp42D));_tmp42D->v=({struct Cyc_List_List*_tmp42E=_cycalloc(
sizeof(*_tmp42E));_tmp42E->hd=_tmp41D;_tmp42E->tl=0;_tmp42E;});_tmp42D;});
_tmp42C->where_clause=0;_tmp42C->body=_tmp422;_tmp42C->loc=0;_tmp42C;});struct
Cyc_Absyn_Stmt*_tmp424=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp3EF,({struct
Cyc_List_List*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->hd=_tmp423;_tmp42B->tl=
0;_tmp42B;})),0);Cyc_Toc_stmt_to_c(nv,_tmp424);{struct Cyc_Absyn_Exp*_tmp425=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp42A=_cycalloc(sizeof(*_tmp42A));
_tmp42A->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp42A->tl=0;
_tmp42A;}),0);struct Cyc_Absyn_Stmt*_tmp426=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp429=_cycalloc(sizeof(*_tmp429));
_tmp429->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp429->tl=0;_tmp429;}),0),0);struct
Cyc_Absyn_Exp*_tmp427=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp428=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp426,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp427,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp425,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp428,0),Cyc_Absyn_skip_stmt(0),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp41A,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_cast_exp(
e_typ,Cyc_Toc__exn_thrown_e,0),_tmp424,0),0),0),0),0),0))->r));return;}}}}}
_LL468: {void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp3F2->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(nv,x_var,x_exp),_tmp3F1);(void*)(s->r=(
void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp433=
_cycalloc(sizeof(*_tmp433));_tmp433->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0);_tmp433->tl=0;_tmp433;}),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*
_tmp434[1];_tmp434[0]=x_exp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp434,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),Cyc_Absyn_seq_stmt(
_tmp3F1,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct
Cyc_Absyn_Exp*_tmp435[1];_tmp435[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp435,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL46A:({void*_tmp436[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("switchC_s",
sizeof(unsigned char),10),_tag_arr(_tmp436,sizeof(void*),0));});_LL46C:({void*
_tmp437[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("cut",sizeof(unsigned char),4),_tag_arr(_tmp437,sizeof(void*),0));});
_LL46E:({void*_tmp438[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(_tag_arr("splice",sizeof(unsigned char),7),_tag_arr(_tmp438,
sizeof(void*),0));});_LL444:;}}struct _tuple15{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f){f->tvs=0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((
void*)f->ret_type));{struct Cyc_List_List*_tmp439=f->args;for(0;_tmp439 != 0;
_tmp439=_tmp439->tl){(*((struct _tuple15*)_tmp439->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple15*)_tmp439->hd)).f3);{struct _tuple0*_tmp43A=({struct _tuple0*_tmp43B=
_cycalloc(sizeof(*_tmp43B));_tmp43B->f1=(void*)0;_tmp43B->f2=(*((struct _tuple15*)
_tmp439->hd)).f1;_tmp43B;});nv=Cyc_Toc_add_varmap(nv,_tmp43A,Cyc_Absyn_var_exp(
_tmp43A,0));}}}if((unsigned int)f->cyc_varargs?((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0: 0){int _tmp43D;void*_tmp43E;struct Cyc_Absyn_Tqual
_tmp43F;struct Cyc_Core_Opt*_tmp440;struct Cyc_Absyn_VarargInfo _tmp43C=*((struct
Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_LL4AF: _tmp440=_tmp43C.name;
goto _LL4AE;_LL4AE: _tmp43F=_tmp43C.tq;goto _LL4AD;_LL4AD: _tmp43E=(void*)_tmp43C.type;
goto _LL4AC;_LL4AC: _tmp43D=_tmp43C.inject;goto _LL4AB;_LL4AB: {void*_tmp441=Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ(_tmp43E,(void*)2,_tmp43F));struct _tuple0*_tmp442=({struct
_tuple0*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->f1=(void*)0;_tmp445->f2=(
struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null(_tmp440))->v;_tmp445;});f->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
f->args,({struct Cyc_List_List*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->hd=({
struct _tuple15*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->f1=(struct
_tagged_arr*)_tmp440->v;_tmp444->f2=_tmp43F;_tmp444->f3=_tmp441;_tmp444;});
_tmp443->tl=0;_tmp443;}));nv=Cyc_Toc_add_varmap(nv,_tmp442,Cyc_Absyn_var_exp(
_tmp442,0));f->cyc_varargs=0;}}{struct Cyc_List_List*_tmp446=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp446 != 0;_tmp446=
_tmp446->tl){(void*)(((struct Cyc_Absyn_Vardecl*)_tmp446->hd)->type=(void*)Cyc_Toc_typ_to_c((
void*)((struct Cyc_Absyn_Vardecl*)_tmp446->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(
nv),f->body);}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp447=s;_LL4B1: if((
int)_tmp447 == 1){goto _LL4B2;}else{goto _LL4B3;}_LL4B3: if((int)_tmp447 == 4){goto
_LL4B4;}else{goto _LL4B5;}_LL4B5: goto _LL4B6;_LL4B2: return(void*)2;_LL4B4: return(
void*)3;_LL4B6: return s;_LL4B0:;}struct Cyc_Core_Opt*Cyc_Toc_aggrs_so_far=0;static
void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple0*_tmp448=ad->name;
if(Cyc_Toc_aggrs_so_far == 0){Cyc_Toc_aggrs_so_far=({struct Cyc_Core_Opt*_tmp449=
_cycalloc(sizeof(*_tmp449));_tmp449->v=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);
_tmp449;});}{int seen_defn_before;struct Cyc_Core_Opt*_tmp44A=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*_tmp448).f2);if(
_tmp44A == 0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*_tmp448).f2,ad));}
else{if(((struct Cyc_Absyn_Aggrdecl*)_tmp44A->v)->fields == 0){(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*
_tmp448).f2,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(
void*)((void*)2));ad->tvs=0;ad->exist_vars=0;if(ad->fields != 0){if(
seen_defn_before){ad->fields=0;}else{struct Cyc_List_List*_tmp44B=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ad->fields))->v;for(0;_tmp44B != 0;_tmp44B=
_tmp44B->tl){(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp44B->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp44B->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0){Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple0*,struct _tuple0*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp44C;});}{
struct _tuple0*_tmp44D=tud->name;if(tud->fields == 0? 1:((int(*)(struct Cyc_Set_Set*
s,struct _tuple0*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp44D)){return;}(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp44D));{struct Cyc_List_List*
_tmp44E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp44E != 0;_tmp44E=_tmp44E->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp44E->hd;if(f->typs != 0){struct Cyc_List_List*_tmp44F=0;int i=1;{struct Cyc_List_List*
_tmp450=f->typs;for(0;_tmp450 != 0;(_tmp450=_tmp450->tl,i ++)){struct _tagged_arr*
_tmp451=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*_tmp452=({struct Cyc_Absyn_Aggrfield*
_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454->name=_tmp451;_tmp454->tq=(*((struct
_tuple3*)_tmp450->hd)).f1;_tmp454->type=(void*)Cyc_Toc_typ_to_c_array((*((struct
_tuple3*)_tmp450->hd)).f2);_tmp454->width=0;_tmp454->attributes=0;_tmp454;});
_tmp44F=({struct Cyc_List_List*_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453->hd=
_tmp452;_tmp453->tl=_tmp44F;_tmp453;});}}_tmp44F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp44F);_tmp44F=({struct Cyc_List_List*
_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->hd=({struct Cyc_Absyn_Aggrfield*
_tmp456=_cycalloc(sizeof(*_tmp456));_tmp456->name=Cyc_Toc_tag_sp;_tmp456->tq=Cyc_Toc_mt_tq;
_tmp456->type=(void*)Cyc_Absyn_sint_t;_tmp456->width=0;_tmp456->attributes=0;
_tmp456;});_tmp455->tl=_tmp44F;_tmp455;});{struct Cyc_Absyn_Aggrdecl*_tmp457=({
struct Cyc_Absyn_Aggrdecl*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->kind=(void*)((
void*)0);_tmp45B->sc=(void*)((void*)2);_tmp45B->name=Cyc_Toc_collapse_qvar_tag(f->name,
_tag_arr("_struct",sizeof(unsigned char),8));_tmp45B->tvs=0;_tmp45B->exist_vars=
0;_tmp45B->fields=({struct Cyc_Core_Opt*_tmp45C=_cycalloc(sizeof(*_tmp45C));
_tmp45C->v=_tmp44F;_tmp45C;});_tmp45B->attributes=0;_tmp45B;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp459=_cycalloc(sizeof(*_tmp459));
_tmp459[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp45A;_tmp45A.tag=4;_tmp45A.f1=
_tmp457;_tmp45A;});_tmp459;}),0);_tmp458->tl=Cyc_Toc_result_decls;_tmp458;});}}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0){Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Core_Opt*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp45D;});}if(xd->fields == 0){return;}{struct _tuple0*_tmp45E=xd->name;struct Cyc_List_List*
_tmp45F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp45F != 0;_tmp45F=_tmp45F->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp45F->hd;struct _tagged_arr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp460=Cyc_Absyn_uint_exp(
_get_arr_size(*fn,sizeof(unsigned char))+ 4,0);struct Cyc_Absyn_ArrayType_struct*
_tmp461=({struct Cyc_Absyn_ArrayType_struct*_tmp48A=_cycalloc(sizeof(*_tmp48A));
_tmp48A[0]=({struct Cyc_Absyn_ArrayType_struct _tmp48B;_tmp48B.tag=7;_tmp48B.f1=(
void*)Cyc_Absyn_uchar_t;_tmp48B.f2=Cyc_Toc_mt_tq;_tmp48B.f3=(struct Cyc_Absyn_Exp*)
_tmp460;_tmp48B;});_tmp48A;});struct Cyc_Core_Opt*_tmp462=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,f->name);
struct Cyc_Core_Opt _tmp463;int _tmp464;_LL4B8: if(_tmp462 == 0){goto _LL4B9;}else{
goto _LL4BA;}_LL4BA: if(_tmp462 == 0){goto _LL4BC;}else{_tmp463=*_tmp462;_LL4BE:
_tmp464=(int)_tmp463.v;if(_tmp464 == 0){goto _LL4BB;}else{goto _LL4BC;}}_LL4BC: goto
_LL4BD;_LL4B9: {struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3){
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _tagged_arr)({struct
Cyc_Std_String_pa_struct _tmp46A;_tmp46A.tag=0;_tmp46A.f1=(struct _tagged_arr)*fn;{
struct Cyc_Std_Int_pa_struct _tmp469;_tmp469.tag=1;_tmp469.f1=(int)((unsigned int)((
int)'\000'));{struct Cyc_Std_Int_pa_struct _tmp468;_tmp468.tag=1;_tmp468.f1=(int)((
unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct _tmp467;_tmp467.tag=1;
_tmp467.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp466;_tmp466.tag=1;_tmp466.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp465[5]={& _tmp466,& _tmp467,& _tmp468,& _tmp469,& _tmp46A};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(unsigned char),11),_tag_arr(_tmp465,sizeof(void*),5));}}}}}}),
0);}{struct Cyc_Absyn_Vardecl*_tmp46B=Cyc_Absyn_new_vardecl(f->name,(void*)
_tmp461,initopt);(void*)(_tmp46B->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D[
0]=({struct Cyc_Absyn_Var_d_struct _tmp46E;_tmp46E.tag=0;_tmp46E.f1=_tmp46B;
_tmp46E;});_tmp46D;}),0);_tmp46C->tl=Cyc_Toc_result_decls;_tmp46C;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp46F=f->typs;for(0;_tmp46F != 0;(_tmp46F=_tmp46F->tl,
i ++)){struct _tagged_arr*_tmp470=({struct _tagged_arr*_tmp474=_cycalloc(sizeof(*
_tmp474));_tmp474[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp476;
_tmp476.tag=1;_tmp476.f1=(int)((unsigned int)i);{void*_tmp475[1]={& _tmp476};Cyc_Std_aprintf(
_tag_arr("f%d",sizeof(unsigned char),4),_tag_arr(_tmp475,sizeof(void*),1));}});
_tmp474;});struct Cyc_Absyn_Aggrfield*_tmp471=({struct Cyc_Absyn_Aggrfield*_tmp473=
_cycalloc(sizeof(*_tmp473));_tmp473->name=_tmp470;_tmp473->tq=(*((struct _tuple3*)
_tmp46F->hd)).f1;_tmp473->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple3*)
_tmp46F->hd)).f2);_tmp473->width=0;_tmp473->attributes=0;_tmp473;});fields=({
struct Cyc_List_List*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->hd=_tmp471;
_tmp472->tl=fields;_tmp472;});}}fields=({struct Cyc_List_List*_tmp477=_cycalloc(
sizeof(*_tmp477));_tmp477->hd=({struct Cyc_Absyn_Aggrfield*_tmp478=_cycalloc(
sizeof(*_tmp478));_tmp478->name=Cyc_Toc_tag_sp;_tmp478->tq=Cyc_Toc_mt_tq;_tmp478->type=(
void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_t,Cyc_Toc_mt_tq);_tmp478->width=0;
_tmp478->attributes=0;_tmp478;});_tmp477->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);_tmp477;});{struct Cyc_Absyn_Aggrdecl*_tmp479=({
struct Cyc_Absyn_Aggrdecl*_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D->kind=(void*)((
void*)0);_tmp47D->sc=(void*)((void*)2);_tmp47D->name=Cyc_Toc_collapse_qvar_tag(f->name,
_tag_arr("_struct",sizeof(unsigned char),8));_tmp47D->tvs=0;_tmp47D->exist_vars=
0;_tmp47D->fields=({struct Cyc_Core_Opt*_tmp47E=_cycalloc(sizeof(*_tmp47E));
_tmp47E->v=fields;_tmp47E;});_tmp47D->attributes=0;_tmp47D;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));
_tmp47B[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp47C;_tmp47C.tag=4;_tmp47C.f1=
_tmp479;_tmp47C;});_tmp47B;}),0);_tmp47A->tl=Cyc_Toc_result_decls;_tmp47A;});}}
goto _LL4B7;}}_LL4BB: if((void*)f->sc != (void*)3){struct Cyc_Absyn_Exp*_tmp47F=Cyc_Absyn_string_exp((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp489;_tmp489.tag=0;_tmp489.f1=(
struct _tagged_arr)*fn;{struct Cyc_Std_Int_pa_struct _tmp488;_tmp488.tag=1;_tmp488.f1=(
int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct _tmp487;_tmp487.tag=
1;_tmp487.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp486;_tmp486.tag=1;_tmp486.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp485;_tmp485.tag=1;_tmp485.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp484[5]={& _tmp485,& _tmp486,& _tmp487,& _tmp488,& _tmp489};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(unsigned char),11),_tag_arr(_tmp484,sizeof(void*),5));}}}}}}),
0);struct Cyc_Absyn_Vardecl*_tmp480=Cyc_Absyn_new_vardecl(f->name,(void*)_tmp461,(
struct Cyc_Absyn_Exp*)_tmp47F);(void*)(_tmp480->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482[
0]=({struct Cyc_Absyn_Var_d_struct _tmp483;_tmp483.tag=0;_tmp483.f1=_tmp480;
_tmp483;});_tmp482;}),0);_tmp481->tl=Cyc_Toc_result_decls;_tmp481;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL4B7;_LL4BD: goto _LL4B7;_LL4B7:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){struct _tuple0*_tmp48C=ed->name;(
void*)(ed->sc=(void*)((void*)2));if(ed->fields != 0){Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}}static
void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array((
void*)vd->type));Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*
init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);void*_tmp48D=(void*)init->r;
struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_Absyn_Vardecl*
_tmp490;_LL4C0: if(*((int*)_tmp48D)== 27){_LL4C6: _tmp490=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp48D)->f1;goto _LL4C5;_LL4C5: _tmp48F=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp48D)->f2;goto _LL4C4;_LL4C4: _tmp48E=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp48D)->f3;goto _LL4C1;}else{goto _LL4C2;}_LL4C2: goto _LL4C3;_LL4C1: vd->initializer=
0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(
vd->name,0),_tmp490,_tmp48F,_tmp48E,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL4BF;_LL4C3: if((void*)vd->sc == (void*)0){struct Cyc_Toc_Env*_tmp491=Cyc_Toc_copy_env(
init_nv);_tmp491->toplevel=1;Cyc_Toc_exp_to_c(_tmp491,init);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL4BF;_LL4BF:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,struct Cyc_Core_Opt*topt,struct Cyc_Absyn_Exp*
e,int exhaust,struct Cyc_Absyn_Stmt*s){void*t=(void*)((struct Cyc_Core_Opt*)
_check_null(topt))->v;Cyc_Toc_exp_to_c(nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();
struct _tagged_arr*succ_lab=Cyc_Toc_fresh_label();struct _tagged_arr*fail_lab=
exhaust? succ_lab: Cyc_Toc_fresh_label();{struct _RegionHandle _tmp492=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp492;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp494;struct Cyc_List_List*_tmp495;struct Cyc_Toc_Env*_tmp496;struct _tuple11
_tmp493=Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),
p,succ_lab,fail_lab,0);_LL4CA: _tmp496=_tmp493.f1;goto _LL4C9;_LL4C9: _tmp495=
_tmp493.f2;goto _LL4C8;_LL4C8: _tmp494=_tmp493.f3;goto _LL4C7;_LL4C7: Cyc_Toc_stmt_to_c(
_tmp496,s);{struct Cyc_Absyn_Stmt*_tmp497=Cyc_Absyn_label_stmt(succ_lab,s,0);if(
exhaust){s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)e,
Cyc_Absyn_seq_stmt(_tmp494,_tmp497,0),0);}else{struct Cyc_Absyn_Exp*_tmp498=Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0));struct Cyc_Absyn_Stmt*_tmp499=Cyc_Absyn_label_stmt(
fail_lab,Cyc_Absyn_exp_stmt(_tmp498,0),0);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp494,Cyc_Absyn_seq_stmt(_tmp499,
_tmp497,0),0),0);}for(0;_tmp495 != 0;_tmp495=_tmp495->tl){struct _tuple12 _tmp49B;
void*_tmp49C;struct _tuple0*_tmp49D;struct _tuple12*_tmp49A=(struct _tuple12*)
_tmp495->hd;_tmp49B=*_tmp49A;_LL4CD: _tmp49D=_tmp49B.f1;goto _LL4CC;_LL4CC: _tmp49C=
_tmp49B.f2;goto _LL4CB;_LL4CB: s=Cyc_Absyn_declare_stmt(_tmp49D,_tmp49C,0,s,0);}}};
_pop_region(prgn);}return s;}}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top){for(0;ds != 0;ds=ds->tl){if(! nv->toplevel){({
void*_tmp49E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("decls_to_c: not at toplevel!",sizeof(unsigned char),29),_tag_arr(
_tmp49E,sizeof(void*),0));});}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp49F=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp4A0;struct Cyc_Absyn_Fndecl*
_tmp4A1;struct Cyc_Absyn_Aggrdecl*_tmp4A2;struct Cyc_Absyn_Tuniondecl*_tmp4A3;
struct Cyc_Absyn_Enumdecl*_tmp4A4;struct Cyc_Absyn_Typedefdecl*_tmp4A5;struct Cyc_List_List*
_tmp4A6;struct Cyc_List_List*_tmp4A7;struct Cyc_List_List*_tmp4A8;_LL4CF: if(*((int*)
_tmp49F)== 0){_LL4E5: _tmp4A0=((struct Cyc_Absyn_Var_d_struct*)_tmp49F)->f1;goto
_LL4D0;}else{goto _LL4D1;}_LL4D1: if(*((int*)_tmp49F)== 1){_LL4E6: _tmp4A1=((struct
Cyc_Absyn_Fn_d_struct*)_tmp49F)->f1;goto _LL4D2;}else{goto _LL4D3;}_LL4D3: if(*((
int*)_tmp49F)== 2){goto _LL4D4;}else{goto _LL4D5;}_LL4D5: if(*((int*)_tmp49F)== 3){
goto _LL4D6;}else{goto _LL4D7;}_LL4D7: if(*((int*)_tmp49F)== 4){_LL4E7: _tmp4A2=((
struct Cyc_Absyn_Aggr_d_struct*)_tmp49F)->f1;goto _LL4D8;}else{goto _LL4D9;}_LL4D9:
if(*((int*)_tmp49F)== 5){_LL4E8: _tmp4A3=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp49F)->f1;goto _LL4DA;}else{goto _LL4DB;}_LL4DB: if(*((int*)_tmp49F)== 6){_LL4E9:
_tmp4A4=((struct Cyc_Absyn_Enum_d_struct*)_tmp49F)->f1;goto _LL4DC;}else{goto
_LL4DD;}_LL4DD: if(*((int*)_tmp49F)== 7){_LL4EA: _tmp4A5=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp49F)->f1;goto _LL4DE;}else{goto _LL4DF;}_LL4DF: if(*((int*)_tmp49F)== 8){_LL4EB:
_tmp4A6=((struct Cyc_Absyn_Namespace_d_struct*)_tmp49F)->f2;goto _LL4E0;}else{goto
_LL4E1;}_LL4E1: if(*((int*)_tmp49F)== 9){_LL4EC: _tmp4A7=((struct Cyc_Absyn_Using_d_struct*)
_tmp49F)->f2;goto _LL4E2;}else{goto _LL4E3;}_LL4E3: if(*((int*)_tmp49F)== 10){
_LL4ED: _tmp4A8=((struct Cyc_Absyn_ExternC_d_struct*)_tmp49F)->f1;goto _LL4E4;}
else{goto _LL4CE;}_LL4D0: {struct _tuple0*_tmp4A9=_tmp4A0->name;if((void*)_tmp4A0->sc
== (void*)4){_tmp4A9=({struct _tuple0*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA->f1=
Cyc_Absyn_rel_ns_null;_tmp4AA->f2=(*_tmp4A9).f2;_tmp4AA;});}if(_tmp4A0->initializer
!= 0){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp4A0->initializer));}
nv=Cyc_Toc_add_varmap(nv,_tmp4A0->name,Cyc_Absyn_varb_exp(_tmp4A9,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB[0]=({
struct Cyc_Absyn_Global_b_struct _tmp4AC;_tmp4AC.tag=0;_tmp4AC.f1=_tmp4A0;_tmp4AC;});
_tmp4AB;}),0));_tmp4A0->name=_tmp4A9;(void*)(_tmp4A0->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp4A0->sc));(void*)(_tmp4A0->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp4A0->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4AD=_cycalloc(
sizeof(*_tmp4AD));_tmp4AD->hd=d;_tmp4AD->tl=Cyc_Toc_result_decls;_tmp4AD;});goto
_LL4CE;}_LL4D2: {struct _tuple0*_tmp4AE=_tmp4A1->name;if((void*)_tmp4A1->sc == (
void*)4){_tmp4AE=({struct _tuple0*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF->f1=
Cyc_Absyn_rel_ns_null;_tmp4AF->f2=(*_tmp4AE).f2;_tmp4AF;});}nv=Cyc_Toc_add_varmap(
nv,_tmp4A1->name,Cyc_Absyn_var_exp(_tmp4AE,0));_tmp4A1->name=_tmp4AE;Cyc_Toc_fndecl_to_c(
nv,_tmp4A1);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4B0=_cycalloc(sizeof(*
_tmp4B0));_tmp4B0->hd=d;_tmp4B0->tl=Cyc_Toc_result_decls;_tmp4B0;});goto _LL4CE;}
_LL4D4: goto _LL4D6;_LL4D6:({void*_tmp4B1[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("letdecl at toplevel",sizeof(
unsigned char),20),_tag_arr(_tmp4B1,sizeof(void*),0));});_LL4D8: Cyc_Toc_aggrdecl_to_c(
_tmp4A2);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4B2=_cycalloc(sizeof(*
_tmp4B2));_tmp4B2->hd=d;_tmp4B2->tl=Cyc_Toc_result_decls;_tmp4B2;});goto _LL4CE;
_LL4DA: if(_tmp4A3->is_xtunion){Cyc_Toc_xtuniondecl_to_c(_tmp4A3);}else{Cyc_Toc_tuniondecl_to_c(
_tmp4A3);}goto _LL4CE;_LL4DC: Cyc_Toc_enumdecl_to_c(nv,_tmp4A4);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3->hd=d;_tmp4B3->tl=
Cyc_Toc_result_decls;_tmp4B3;});goto _LL4CE;_LL4DE: _tmp4A5->name=_tmp4A5->name;
_tmp4A5->tvs=0;(void*)(_tmp4A5->defn=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp4A5->defn));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4B4=_cycalloc(
sizeof(*_tmp4B4));_tmp4B4->hd=d;_tmp4B4->tl=Cyc_Toc_result_decls;_tmp4B4;});goto
_LL4CE;_LL4E0: _tmp4A7=_tmp4A6;goto _LL4E2;_LL4E2: _tmp4A8=_tmp4A7;goto _LL4E4;
_LL4E4: nv=Cyc_Toc_decls_to_c(nv,_tmp4A8,top);goto _LL4CE;_LL4CE:;}}return nv;}
static void Cyc_Toc_init(){Cyc_Toc_result_decls=0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=
0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=
0;Cyc_Toc_tunions_so_far=0;Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(Cyc_Toc_empty_env(),ds,1);return((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
