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
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;extern int
Cyc_Std_fflush(struct Cyc_Std___cycFILE*);extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int
Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);
extern int Cyc_Std_vfprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct
_tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};extern struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);extern int Cyc_List_length(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,
void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];
extern void*Cyc_List_nth(struct Cyc_List_List*x,int n);extern int Cyc_List_forall(int(*
pred)(void*),struct Cyc_List_List*x);extern int Cyc_Std_zstrptrcmp(struct
_tagged_arr*,struct _tagged_arr*);extern struct _tagged_arr Cyc_Std_strconcat(struct
_tagged_arr,struct _tagged_arr);struct Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_empty(
int(*cmp)(void*,void*));extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*
s,void*elt);extern int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern
unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(
int(*cmp)(void*,void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern
struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;extern struct
_tagged_arr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
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
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_structarrow_exp(struct Cyc_Absyn_Exp*,struct
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
struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Structfield*Cyc_Absyn_lookup_field(
struct Cyc_List_List*,struct _tagged_arr*);extern struct _tagged_arr*Cyc_Absyn_fieldname(
int);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);extern
struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);extern struct
_tagged_arr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);extern struct
_tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_StructRes_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;
struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*uniondecls;struct
Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_tvar_kind(struct
Cyc_Absyn_Tvar*t);extern void*Cyc_Tcutil_typ_kind(void*t);extern void*Cyc_Tcutil_compress(
void*t);extern int Cyc_Tcutil_is_pointer_type(void*t);extern int Cyc_Tcutil_unify(
void*,void*);extern struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);extern int Cyc_Tcutil_is_tagged_pointer_typ(void*);
extern int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);struct
_tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};extern void*Cyc_Tcutil_snd_tqt(struct
_tuple3*);extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
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
Cyc_Absyn_StructType_struct Cyc_Toc_tagged_arr_typ_v={10,(struct _tuple0*)& Cyc_Toc__tagged_arr_pr,
0,0};static void*Cyc_Toc_tagged_arr_typ=(void*)& Cyc_Toc_tagged_arr_typ_v;static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0};struct _tuple4{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmp40=(void*)e->r;void*_tmp41;unsigned char _tmp42;void*_tmp43;short _tmp44;void*
_tmp45;int _tmp46;void*_tmp47;long long _tmp48;void*_tmp49;struct Cyc_Absyn_Exp*
_tmp4A;struct Cyc_List_List*_tmp4B;struct Cyc_List_List*_tmp4C;struct Cyc_List_List*
_tmp4D;struct Cyc_List_List*_tmp4E;struct Cyc_List_List*_tmp4F;_LL1: if(*((int*)
_tmp40)== 0){_LL19: _tmp41=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp40)->f1;
if((unsigned int)_tmp41 > 1?*((int*)_tmp41)== 0: 0){_LL1A: _tmp42=((struct Cyc_Absyn_Char_c_struct*)
_tmp41)->f2;goto _LL2;}else{goto _LL3;}}else{goto _LL3;}_LL3: if(*((int*)_tmp40)== 0){
_LL1B: _tmp43=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp40)->f1;if((
unsigned int)_tmp43 > 1?*((int*)_tmp43)== 1: 0){_LL1C: _tmp44=((struct Cyc_Absyn_Short_c_struct*)
_tmp43)->f2;goto _LL4;}else{goto _LL5;}}else{goto _LL5;}_LL5: if(*((int*)_tmp40)== 0){
_LL1D: _tmp45=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp40)->f1;if((
unsigned int)_tmp45 > 1?*((int*)_tmp45)== 2: 0){_LL1E: _tmp46=((struct Cyc_Absyn_Int_c_struct*)
_tmp45)->f2;goto _LL6;}else{goto _LL7;}}else{goto _LL7;}_LL7: if(*((int*)_tmp40)== 0){
_LL1F: _tmp47=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp40)->f1;if((
unsigned int)_tmp47 > 1?*((int*)_tmp47)== 3: 0){_LL20: _tmp48=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp47)->f2;goto _LL8;}else{goto _LL9;}}else{goto _LL9;}_LL9: if(*((int*)_tmp40)== 0){
_LL21: _tmp49=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp40)->f1;if((int)_tmp49
== 0){goto _LLA;}else{goto _LLB;}}else{goto _LLB;}_LLB: if(*((int*)_tmp40)== 13){
_LL22: _tmp4A=((struct Cyc_Absyn_Cast_e_struct*)_tmp40)->f2;goto _LLC;}else{goto
_LLD;}_LLD: if(*((int*)_tmp40)== 24){_LL23: _tmp4B=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp40)->f1;goto _LLE;}else{goto _LLF;}_LLF: if(*((int*)_tmp40)== 26){_LL24: _tmp4C=((
struct Cyc_Absyn_Array_e_struct*)_tmp40)->f1;goto _LL10;}else{goto _LL11;}_LL11: if(*((
int*)_tmp40)== 28){_LL25: _tmp4D=((struct Cyc_Absyn_Struct_e_struct*)_tmp40)->f3;
goto _LL12;}else{goto _LL13;}_LL13: if(*((int*)_tmp40)== 25){_LL26: _tmp4E=((struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp40)->f2;goto _LL14;}else{goto _LL15;}_LL15: if(*((
int*)_tmp40)== 34){_LL27: _tmp4F=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp40)->f2;
goto _LL16;}else{goto _LL17;}_LL17: goto _LL18;_LL2: return _tmp42 == '\000';_LL4:
return _tmp44 == 0;_LL6: return _tmp46 == 0;_LL8: return _tmp48 == 0;_LLA: return 1;_LLC:
return Cyc_Toc_is_zero(_tmp4A);_LLE: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp4B);_LL10: _tmp4D=
_tmp4C;goto _LL12;_LL12: _tmp4E=_tmp4D;goto _LL14;_LL14: _tmp4F=_tmp4E;goto _LL16;
_LL16: for(0;_tmp4F != 0;_tmp4F=_tmp4F->tl){if(! Cyc_Toc_is_zero((*((struct _tuple4*)
_tmp4F->hd)).f2)){return 0;}}return 1;_LL18: return 0;_LL0:;}static int Cyc_Toc_is_nullable(
void*t){void*_tmp50=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp51;struct
Cyc_Absyn_Conref*_tmp52;_LL29: if((unsigned int)_tmp50 > 3?*((int*)_tmp50)== 4: 0){
_LL2D: _tmp51=((struct Cyc_Absyn_PointerType_struct*)_tmp50)->f1;_LL2E: _tmp52=
_tmp51.nullable;goto _LL2A;}else{goto _LL2B;}_LL2B: goto _LL2C;_LL2A: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp52);_LL2C:({void*
_tmp53[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("is_nullable",sizeof(unsigned char),12),_tag_arr(_tmp53,sizeof(void*),0));});
_LL28:;}static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,struct
_tagged_arr tag){return({struct _tuple0*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->f1=(*
x).f1;_tmp54->f2=({struct _tagged_arr*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55[0]=(
struct _tagged_arr)Cyc_Std_strconcat(*(*x).f2,tag);_tmp55;});_tmp54;});}struct
_tuple5{struct _tagged_arr*f1;struct Cyc_List_List*f2;};static struct _tagged_arr*
Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){{struct Cyc_List_List*_tmp56=Cyc_Toc_tuple_types;
for(0;_tmp56 != 0;_tmp56=_tmp56->tl){struct _tuple5 _tmp58;struct Cyc_List_List*
_tmp59;struct _tagged_arr*_tmp5A;struct _tuple5*_tmp57=(struct _tuple5*)_tmp56->hd;
_tmp58=*_tmp57;_LL31: _tmp5A=_tmp58.f1;goto _LL30;_LL30: _tmp59=_tmp58.f2;goto _LL2F;
_LL2F: {struct Cyc_List_List*_tmp5B=tqs0;for(0;_tmp5B != 0? _tmp59 != 0: 0;(_tmp5B=
_tmp5B->tl,_tmp59=_tmp59->tl)){if(! Cyc_Tcutil_unify((*((struct _tuple3*)_tmp5B->hd)).f2,(
void*)_tmp59->hd)){break;}}if(_tmp5B == 0? _tmp59 == 0: 0){return _tmp5A;}}}}{struct
_tagged_arr*x=({struct _tagged_arr*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=(
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp6B;_tmp6B.tag=1;_tmp6B.f1=(
int)((unsigned int)Cyc_Toc_tuple_type_counter ++);{void*_tmp6A[1]={& _tmp6B};Cyc_Std_aprintf(
_tag_arr("_tuple%d",sizeof(unsigned char),9),_tag_arr(_tmp6A,sizeof(void*),1));}});
_tmp69;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple3*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct
Cyc_List_List*_tmp5C=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,
i ++)){_tmp5C=({struct Cyc_List_List*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->hd=({
struct Cyc_Absyn_Structfield*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->name=Cyc_Absyn_fieldname(
i);_tmp5E->tq=Cyc_Toc_mt_tq;_tmp5E->type=(void*)((void*)ts2->hd);_tmp5E->width=0;
_tmp5E->attributes=0;_tmp5E;});_tmp5D->tl=_tmp5C;_tmp5D;});}}_tmp5C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5C);{struct Cyc_Absyn_Structdecl*
_tmp5F=({struct Cyc_Absyn_Structdecl*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->sc=(
void*)((void*)2);_tmp65->name=({struct Cyc_Core_Opt*_tmp67=_cycalloc(sizeof(*
_tmp67));_tmp67->v=({struct _tuple0*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->f1=
Cyc_Absyn_rel_ns_null;_tmp68->f2=x;_tmp68;});_tmp67;});_tmp65->tvs=0;_tmp65->fields=({
struct Cyc_Core_Opt*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->v=_tmp5C;_tmp66;});
_tmp65->attributes=0;_tmp65;});Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp60=
_cycalloc(sizeof(*_tmp60));_tmp60->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Struct_d_struct*
_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=({struct Cyc_Absyn_Struct_d_struct
_tmp62;_tmp62.tag=4;_tmp62.f1=_tmp5F;_tmp62;});_tmp61;}),0);_tmp60->tl=Cyc_Toc_result_decls;
_tmp60;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmp63=_cycalloc(sizeof(*
_tmp63));_tmp63->hd=({struct _tuple5*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->f1=
x;_tmp64->f2=ts;_tmp64;});_tmp63->tl=Cyc_Toc_tuple_types;_tmp63;});return x;}}}
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
t);_LL39:;}static struct Cyc_Absyn_Structfield*Cyc_Toc_structfield_to_c(struct Cyc_Absyn_Structfield*
f){return({struct Cyc_Absyn_Structfield*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->name=
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
void*_tmpA1;struct Cyc_List_List*_tmpA2;struct Cyc_List_List*_tmpA3;struct Cyc_List_List*
_tmpA4;struct _tuple0*_tmpA5;struct _tuple0*_tmpA6;struct _tuple0*_tmpA7;struct Cyc_List_List*
_tmpA8;struct Cyc_Core_Opt*_tmpA9;struct Cyc_List_List*_tmpAA;struct _tuple0*_tmpAB;
void*_tmpAC;_LL46: if((int)_tmp89 == 0){goto _LL47;}else{goto _LL48;}_LL48: if((
unsigned int)_tmp89 > 3?*((int*)_tmp89)== 0: 0){_LL7E: _tmp8A=((struct Cyc_Absyn_Evar_struct*)
_tmp89)->f2;if(_tmp8A == 0){goto _LL49;}else{goto _LL4A;}}else{goto _LL4A;}_LL4A: if((
unsigned int)_tmp89 > 3?*((int*)_tmp89)== 0: 0){_LL7F: _tmp8B=((struct Cyc_Absyn_Evar_struct*)
_tmp89)->f2;if(_tmp8B == 0){goto _LL4C;}else{_tmp8C=*_tmp8B;_LL80: _tmp8D=(void*)
_tmp8C.v;goto _LL4B;}}else{goto _LL4C;}_LL4C: if((unsigned int)_tmp89 > 3?*((int*)
_tmp89)== 1: 0){_LL81: _tmp8E=((struct Cyc_Absyn_VarType_struct*)_tmp89)->f1;goto
_LL4D;}else{goto _LL4E;}_LL4E: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 2: 0){
_LL82: _tmp8F=((struct Cyc_Absyn_TunionType_struct*)_tmp89)->f1;_LL83: _tmp90=(void*)
_tmp8F.tunion_info;if(*((int*)_tmp90)== 1){goto _LL4F;}else{goto _LL50;}}else{goto
_LL50;}_LL50: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 2: 0){goto _LL51;}else{
goto _LL52;}_LL52: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 3: 0){_LL84: _tmp91=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp89)->f1;_LL85: _tmp92=(void*)_tmp91.field_info;
if(*((int*)_tmp92)== 1){_LL87: _tmp94=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp92)->f1;goto _LL86;_LL86: _tmp93=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp92)->f2;goto _LL53;}else{goto _LL54;}}else{goto _LL54;}_LL54: if((unsigned int)
_tmp89 > 3?*((int*)_tmp89)== 3: 0){goto _LL55;}else{goto _LL56;}_LL56: if((
unsigned int)_tmp89 > 3?*((int*)_tmp89)== 4: 0){_LL88: _tmp95=((struct Cyc_Absyn_PointerType_struct*)
_tmp89)->f1;_LL8B: _tmp98=(void*)_tmp95.elt_typ;goto _LL8A;_LL8A: _tmp97=_tmp95.tq;
goto _LL89;_LL89: _tmp96=_tmp95.bounds;goto _LL57;}else{goto _LL58;}_LL58: if((
unsigned int)_tmp89 > 3?*((int*)_tmp89)== 5: 0){goto _LL59;}else{goto _LL5A;}_LL5A:
if((int)_tmp89 == 1){goto _LL5B;}else{goto _LL5C;}_LL5C: if((unsigned int)_tmp89 > 3?*((
int*)_tmp89)== 6: 0){goto _LL5D;}else{goto _LL5E;}_LL5E: if((unsigned int)_tmp89 > 3?*((
int*)_tmp89)== 7: 0){_LL8E: _tmp9B=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp89)->f1;goto _LL8D;_LL8D: _tmp9A=((struct Cyc_Absyn_ArrayType_struct*)_tmp89)->f2;
goto _LL8C;_LL8C: _tmp99=((struct Cyc_Absyn_ArrayType_struct*)_tmp89)->f3;goto _LL5F;}
else{goto _LL60;}_LL60: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 8: 0){_LL8F:
_tmp9C=((struct Cyc_Absyn_FnType_struct*)_tmp89)->f1;_LL94: _tmpA1=(void*)_tmp9C.ret_typ;
goto _LL93;_LL93: _tmpA0=_tmp9C.args;goto _LL92;_LL92: _tmp9F=_tmp9C.c_varargs;goto
_LL91;_LL91: _tmp9E=_tmp9C.cyc_varargs;goto _LL90;_LL90: _tmp9D=_tmp9C.attributes;
goto _LL61;}else{goto _LL62;}_LL62: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 9:
0){_LL95: _tmpA2=((struct Cyc_Absyn_TupleType_struct*)_tmp89)->f1;goto _LL63;}else{
goto _LL64;}_LL64: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 14: 0){_LL96: _tmpA3=((
struct Cyc_Absyn_AnonStructType_struct*)_tmp89)->f1;goto _LL65;}else{goto _LL66;}
_LL66: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 15: 0){_LL97: _tmpA4=((struct
Cyc_Absyn_AnonUnionType_struct*)_tmp89)->f1;goto _LL67;}else{goto _LL68;}_LL68: if((
unsigned int)_tmp89 > 3?*((int*)_tmp89)== 11: 0){_LL98: _tmpA5=((struct Cyc_Absyn_UnionType_struct*)
_tmp89)->f1;goto _LL69;}else{goto _LL6A;}_LL6A: if((unsigned int)_tmp89 > 3?*((int*)
_tmp89)== 10: 0){_LL99: _tmpA6=((struct Cyc_Absyn_StructType_struct*)_tmp89)->f1;
goto _LL6B;}else{goto _LL6C;}_LL6C: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 12:
0){_LL9A: _tmpA7=((struct Cyc_Absyn_EnumType_struct*)_tmp89)->f1;goto _LL6D;}else{
goto _LL6E;}_LL6E: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 16: 0){_LL9B: _tmpA8=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp89)->f1;goto _LL6F;}else{goto _LL70;}
_LL70: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 18: 0){_LL9E: _tmpAB=((struct
Cyc_Absyn_TypedefType_struct*)_tmp89)->f1;goto _LL9D;_LL9D: _tmpAA=((struct Cyc_Absyn_TypedefType_struct*)
_tmp89)->f2;goto _LL9C;_LL9C: _tmpA9=((struct Cyc_Absyn_TypedefType_struct*)_tmp89)->f3;
goto _LL71;}else{goto _LL72;}_LL72: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 13:
0){goto _LL73;}else{goto _LL74;}_LL74: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 
17: 0){_LL9F: _tmpAC=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp89)->f1;
goto _LL75;}else{goto _LL76;}_LL76: if((int)_tmp89 == 2){goto _LL77;}else{goto _LL78;}
_LL78: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 19: 0){goto _LL79;}else{goto
_LL7A;}_LL7A: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 20: 0){goto _LL7B;}else{
goto _LL7C;}_LL7C: if((unsigned int)_tmp89 > 3?*((int*)_tmp89)== 21: 0){goto _LL7D;}
else{goto _LL45;}_LL47: return t;_LL49: return Cyc_Absyn_sint_t;_LL4B: return Cyc_Toc_typ_to_c(
_tmp8D);_LL4D: if(Cyc_Tcutil_tvar_kind(_tmp8E)== (void*)0){return(void*)0;}else{
return Cyc_Absyn_void_star_typ();}_LL4F: return Cyc_Absyn_void_star_typ();_LL51:({
void*_tmpAD[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unresolved TunionType",sizeof(unsigned char),22),_tag_arr(_tmpAD,
sizeof(void*),0));});_LL53: if(_tmp93->typs == 0){if(_tmp94->is_xtunion){return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,(void*)2,Cyc_Toc_mt_tq);}else{return Cyc_Absyn_uint_t;}}else{
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp93->name,_tag_arr("_struct",
sizeof(unsigned char),8)));}_LL55:({void*_tmpAE[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unresolved TunionFieldType",
sizeof(unsigned char),27),_tag_arr(_tmpAE,sizeof(void*),0));});_LL57: _tmp98=Cyc_Toc_typ_to_c_array(
_tmp98);{void*_tmpAF=(void*)(Cyc_Absyn_compress_conref(_tmp96))->v;void*_tmpB0;
_LLA1: if((unsigned int)_tmpAF > 1?*((int*)_tmpAF)== 0: 0){_LLA5: _tmpB0=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpAF)->f1;if((int)_tmpB0 == 0){goto _LLA2;}
else{goto _LLA3;}}else{goto _LLA3;}_LLA3: goto _LLA4;_LLA2: return Cyc_Toc_tagged_arr_typ;
_LLA4: return Cyc_Absyn_star_typ(_tmp98,(void*)2,_tmp97);_LLA0:;}_LL59: return t;
_LL5B: return t;_LL5D: return t;_LL5F: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpB2;_tmpB2.tag=7;_tmpB2.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp9B);_tmpB2.f2=
_tmp9A;_tmpB2.f3=_tmp99;_tmpB2;});_tmpB1;});_LL61: {struct Cyc_List_List*_tmpB3=0;
for(0;_tmp9D != 0;_tmp9D=_tmp9D->tl){void*_tmpB4=(void*)_tmp9D->hd;_LLA7: if((int)
_tmpB4 == 3){goto _LLA8;}else{goto _LLA9;}_LLA9: if((int)_tmpB4 == 4){goto _LLAA;}
else{goto _LLAB;}_LLAB: if((unsigned int)_tmpB4 > 16?*((int*)_tmpB4)== 3: 0){goto
_LLAC;}else{goto _LLAD;}_LLAD: goto _LLAE;_LLA8: goto _LLAA;_LLAA: goto _LLAC;_LLAC:
continue;_LLAE: _tmpB3=({struct Cyc_List_List*_tmpB5=_cycalloc(sizeof(*_tmpB5));
_tmpB5->hd=(void*)((void*)_tmp9D->hd);_tmpB5->tl=_tmpB3;_tmpB5;});goto _LLA6;
_LLA6:;}{struct Cyc_List_List*_tmpB6=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(
struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmpA0);
if(_tmp9E != 0){void*_tmpB7=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ((void*)_tmp9E->type,(
void*)2,Cyc_Toc_mt_tq));struct _tuple1*_tmpB8=({struct _tuple1*_tmpBA=_cycalloc(
sizeof(*_tmpBA));_tmpBA->f1=_tmp9E->name;_tmpBA->f2=_tmp9E->tq;_tmpBA->f3=_tmpB7;
_tmpBA;});_tmpB6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(_tmpB6,({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*
_tmpB9));_tmpB9->hd=_tmpB8;_tmpB9->tl=0;_tmpB9;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_FnType_struct
_tmpBC;_tmpBC.tag=8;_tmpBC.f1=({struct Cyc_Absyn_FnInfo _tmpBD;_tmpBD.tvars=0;
_tmpBD.effect=0;_tmpBD.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmpA1);_tmpBD.args=
_tmpB6;_tmpBD.c_varargs=_tmp9F;_tmpBD.cyc_varargs=0;_tmpBD.rgn_po=0;_tmpBD.attributes=
_tmpB3;_tmpBD;});_tmpBC;});_tmpBB;});}}_LL63: _tmpA2=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct _tuple3*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,
_tmpA2);{struct _tagged_arr*_tmpBE=Cyc_Toc_add_tuple_type(_tmpA2);return Cyc_Absyn_strct(
_tmpBE);}_LL65: return(void*)({struct Cyc_Absyn_AnonStructType_struct*_tmpBF=
_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_AnonStructType_struct
_tmpC0;_tmpC0.tag=14;_tmpC0.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*
f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_structfield_to_c,
_tmpA3);_tmpC0;});_tmpBF;});_LL67: return(void*)({struct Cyc_Absyn_AnonUnionType_struct*
_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_AnonUnionType_struct
_tmpC2;_tmpC2.tag=15;_tmpC2.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*
f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_structfield_to_c,
_tmpA4);_tmpC2;});_tmpC1;});_LL69: if(_tmpA5 == 0){({void*_tmpC3[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("anonymous union",
sizeof(unsigned char),16),_tag_arr(_tmpC3,sizeof(void*),0));});}return Cyc_Absyn_unionq_typ((
struct _tuple0*)_check_null(_tmpA5));_LL6B: if(_tmpA6 == 0){({void*_tmpC4[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("anonymous struct",
sizeof(unsigned char),17),_tag_arr(_tmpC4,sizeof(void*),0));});}return Cyc_Absyn_strctq((
struct _tuple0*)_check_null(_tmpA6));_LL6D: return t;_LL6F: Cyc_Toc_enumfields_to_c(
_tmpA8);return t;_LL71: if(_tmpA9 == 0){({void*_tmpC5[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof(unsigned char),38),_tag_arr(_tmpC5,sizeof(void*),0));});}return(void*)({
struct Cyc_Absyn_TypedefType_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({
struct Cyc_Absyn_TypedefType_struct _tmpC7;_tmpC7.tag=18;_tmpC7.f1=_tmpAB;_tmpC7.f2=
0;_tmpC7.f3=({struct Cyc_Core_Opt*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->v=(
void*)Cyc_Toc_typ_to_c_array((void*)_tmpA9->v);_tmpC8;});_tmpC7;});_tmpC6;});
_LL73: return Cyc_Absyn_uint_t;_LL75: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL77:({void*_tmpC9[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof(unsigned char),55),_tag_arr(_tmpC9,sizeof(void*),0));});_LL79:({void*
_tmpCA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::typ_to_c: type translation passed the access eff",sizeof(
unsigned char),54),_tag_arr(_tmpCA,sizeof(void*),0));});_LL7B:({void*_tmpCB[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof(unsigned char),52),_tag_arr(_tmpCB,sizeof(void*),0));});_LL7D:({void*
_tmpCC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::typ_to_c: type translation passed the regions eff",sizeof(
unsigned char),55),_tag_arr(_tmpCC,sizeof(void*),0));});_LL45:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmpCD=t;struct Cyc_Absyn_Tqual _tmpCE;void*_tmpCF;_LLB0: if((unsigned int)
_tmpCD > 3?*((int*)_tmpCD)== 7: 0){_LLB5: _tmpCF=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmpCD)->f1;goto _LLB4;_LLB4: _tmpCE=((struct Cyc_Absyn_ArrayType_struct*)_tmpCD)->f2;
goto _LLB1;}else{goto _LLB2;}_LLB2: goto _LLB3;_LLB1: return Cyc_Absyn_cast_exp(Cyc_Absyn_star_typ(
_tmpCF,(void*)2,_tmpCE),e,l);_LLB3: return Cyc_Absyn_cast_exp(t,e,l);_LLAF:;}
static int Cyc_Toc_atomic_typ(void*t){void*_tmpD0=Cyc_Tcutil_compress(t);void*
_tmpD1;struct Cyc_Absyn_Structdecl**_tmpD2;struct Cyc_List_List*_tmpD3;struct Cyc_List_List*
_tmpD4;struct Cyc_Absyn_TunionFieldInfo _tmpD5;void*_tmpD6;struct Cyc_Absyn_Tunionfield*
_tmpD7;struct Cyc_Absyn_Tuniondecl*_tmpD8;struct Cyc_List_List*_tmpD9;_LLB7: if((
int)_tmpD0 == 0){goto _LLB8;}else{goto _LLB9;}_LLB9: if((unsigned int)_tmpD0 > 3?*((
int*)_tmpD0)== 1: 0){goto _LLBA;}else{goto _LLBB;}_LLBB: if((unsigned int)_tmpD0 > 3?*((
int*)_tmpD0)== 5: 0){goto _LLBC;}else{goto _LLBD;}_LLBD: if((unsigned int)_tmpD0 > 3?*((
int*)_tmpD0)== 12: 0){goto _LLBE;}else{goto _LLBF;}_LLBF: if((unsigned int)_tmpD0 > 3?*((
int*)_tmpD0)== 16: 0){goto _LLC0;}else{goto _LLC1;}_LLC1: if((int)_tmpD0 == 1){goto
_LLC2;}else{goto _LLC3;}_LLC3: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 6: 0){
goto _LLC4;}else{goto _LLC5;}_LLC5: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 8:
0){goto _LLC6;}else{goto _LLC7;}_LLC7: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 
13: 0){goto _LLC8;}else{goto _LLC9;}_LLC9: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)
== 7: 0){_LLDF: _tmpD1=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpD0)->f1;goto
_LLCA;}else{goto _LLCB;}_LLCB: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 10: 0){
_LLE0: _tmpD2=((struct Cyc_Absyn_StructType_struct*)_tmpD0)->f3;goto _LLCC;}else{
goto _LLCD;}_LLCD: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 14: 0){_LLE1: _tmpD3=((
struct Cyc_Absyn_AnonStructType_struct*)_tmpD0)->f1;goto _LLCE;}else{goto _LLCF;}
_LLCF: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 15: 0){_LLE2: _tmpD4=((struct
Cyc_Absyn_AnonUnionType_struct*)_tmpD0)->f1;goto _LLD0;}else{goto _LLD1;}_LLD1: if((
unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 3: 0){_LLE3: _tmpD5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpD0)->f1;_LLE4: _tmpD6=(void*)_tmpD5.field_info;if(*((int*)_tmpD6)== 1){_LLE6:
_tmpD8=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpD6)->f1;goto _LLE5;_LLE5:
_tmpD7=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpD6)->f2;goto _LLD2;}else{
goto _LLD3;}}else{goto _LLD3;}_LLD3: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 9:
0){_LLE7: _tmpD9=((struct Cyc_Absyn_TupleType_struct*)_tmpD0)->f1;goto _LLD4;}else{
goto _LLD5;}_LLD5: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 2: 0){goto _LLD6;}
else{goto _LLD7;}_LLD7: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 11: 0){goto
_LLD8;}else{goto _LLD9;}_LLD9: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 4: 0){
goto _LLDA;}else{goto _LLDB;}_LLDB: if((unsigned int)_tmpD0 > 3?*((int*)_tmpD0)== 17:
0){goto _LLDC;}else{goto _LLDD;}_LLDD: goto _LLDE;_LLB8: return 1;_LLBA: return 0;_LLBC:
return 1;_LLBE: return 1;_LLC0: return 1;_LLC2: return 1;_LLC4: return 1;_LLC6: return 1;
_LLC8: return 1;_LLCA: return Cyc_Toc_atomic_typ(_tmpD1);_LLCC: if(_tmpD2 == 0? 1:(*
_tmpD2)->fields == 0){return 0;}{struct Cyc_List_List*_tmpDA=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null((*_tmpD2)->fields))->v;for(0;_tmpDA != 0;_tmpDA=
_tmpDA->tl){if(! Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Structfield*)_tmpDA->hd)->type)){
return 0;}}}return 1;_LLCE: _tmpD4=_tmpD3;goto _LLD0;_LLD0: for(0;_tmpD4 != 0;_tmpD4=
_tmpD4->tl){if(! Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Structfield*)_tmpD4->hd)->type)){
return 0;}}return 1;_LLD2: _tmpD9=_tmpD7->typs;goto _LLD4;_LLD4: for(0;_tmpD9 != 0;
_tmpD9=_tmpD9->tl){if(! Cyc_Toc_atomic_typ((*((struct _tuple3*)_tmpD9->hd)).f2)){
return 0;}}return 1;_LLD6: goto _LLD8;_LLD8: goto _LLDA;_LLDA: goto _LLDC;_LLDC: return 0;
_LLDE:({struct Cyc_Std_String_pa_struct _tmpDC;_tmpDC.tag=0;_tmpDC.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpDB[1]={& _tmpDC};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("atomic_typ:  bad type %s",
sizeof(unsigned char),25),_tag_arr(_tmpDB,sizeof(void*),1));}});_LLB6:;}static
int Cyc_Toc_is_void_star(void*t){void*_tmpDD=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpDE;void*_tmpDF;_LLE9: if((unsigned int)_tmpDD > 3?*((int*)_tmpDD)== 4: 0){_LLED:
_tmpDE=((struct Cyc_Absyn_PointerType_struct*)_tmpDD)->f1;_LLEE: _tmpDF=(void*)
_tmpDE.elt_typ;goto _LLEA;}else{goto _LLEB;}_LLEB: goto _LLEC;_LLEA: {void*_tmpE0=
Cyc_Tcutil_compress(_tmpDF);_LLF0: if((int)_tmpE0 == 0){goto _LLF1;}else{goto _LLF2;}
_LLF2: goto _LLF3;_LLF1: return 1;_LLF3: return 0;_LLEF:;}_LLEC: return 0;_LLE8:;}static
int Cyc_Toc_is_poly_field(void*t,struct _tagged_arr*f){void*_tmpE1=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Structdecl**_tmpE2;struct Cyc_Absyn_Structdecl*_tmpE3;struct
Cyc_List_List*_tmpE4;_LLF5: if((unsigned int)_tmpE1 > 3?*((int*)_tmpE1)== 10: 0){
_LLFF: _tmpE2=((struct Cyc_Absyn_StructType_struct*)_tmpE1)->f3;if(_tmpE2 == 0){
goto _LLF7;}else{_tmpE3=*_tmpE2;goto _LLF6;}}else{goto _LLF7;}_LLF7: if((
unsigned int)_tmpE1 > 3?*((int*)_tmpE1)== 14: 0){_LL100: _tmpE4=((struct Cyc_Absyn_AnonStructType_struct*)
_tmpE1)->f1;goto _LLF8;}else{goto _LLF9;}_LLF9: if((unsigned int)_tmpE1 > 3?*((int*)
_tmpE1)== 15: 0){goto _LLFA;}else{goto _LLFB;}_LLFB: if((unsigned int)_tmpE1 > 3?*((
int*)_tmpE1)== 11: 0){goto _LLFC;}else{goto _LLFD;}_LLFD: goto _LLFE;_LLF6: if(_tmpE3->fields
== 0){({void*_tmpE5[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: struct missing fields",sizeof(unsigned char),37),
_tag_arr(_tmpE5,sizeof(void*),0));});}_tmpE4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmpE3->fields))->v;goto _LLF8;_LLF8: {struct Cyc_Absyn_Structfield*
_tmpE6=Cyc_Absyn_lookup_field(_tmpE4,f);if(_tmpE6 == 0){({struct Cyc_Std_String_pa_struct
_tmpE8;_tmpE8.tag=0;_tmpE8.f1=(struct _tagged_arr)*f;{void*_tmpE7[1]={& _tmpE8};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: bad field %s",
sizeof(unsigned char),28),_tag_arr(_tmpE7,sizeof(void*),1));}});}return Cyc_Toc_is_void_star((
void*)_tmpE6->type);}_LLFA: goto _LLFC;_LLFC: return 0;_LLFE:({struct Cyc_Std_String_pa_struct
_tmpEA;_tmpEA.tag=0;_tmpEA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpE9[1]={& _tmpEA};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: bad type %s",sizeof(unsigned char),27),_tag_arr(_tmpE9,
sizeof(void*),1));}});_LLF4:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*
e){void*_tmpEB=(void*)e->r;struct _tagged_arr*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;
struct _tagged_arr*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;_LL102: if(*((int*)_tmpEB)== 
21){_LL109: _tmpED=((struct Cyc_Absyn_StructMember_e_struct*)_tmpEB)->f1;goto
_LL108;_LL108: _tmpEC=((struct Cyc_Absyn_StructMember_e_struct*)_tmpEB)->f2;goto
_LL103;}else{goto _LL104;}_LL104: if(*((int*)_tmpEB)== 22){_LL10B: _tmpEF=((struct
Cyc_Absyn_StructArrow_e_struct*)_tmpEB)->f1;goto _LL10A;_LL10A: _tmpEE=((struct Cyc_Absyn_StructArrow_e_struct*)
_tmpEB)->f2;goto _LL105;}else{goto _LL106;}_LL106: goto _LL107;_LL103: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpED->topt))->v,_tmpEC);_LL105: {void*
_tmpF0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpEF->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpF1;void*_tmpF2;_LL10D: if((unsigned int)_tmpF0 > 3?*((
int*)_tmpF0)== 4: 0){_LL111: _tmpF1=((struct Cyc_Absyn_PointerType_struct*)_tmpF0)->f1;
_LL112: _tmpF2=(void*)_tmpF1.elt_typ;goto _LL10E;}else{goto _LL10F;}_LL10F: goto
_LL110;_LL10E: return Cyc_Toc_is_poly_field(_tmpF2,_tmpEE);_LL110:({struct Cyc_Std_String_pa_struct
_tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpEF->topt))->v);{void*_tmpF3[1]={& _tmpF4};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_project:  bad type %s",
sizeof(unsigned char),30),_tag_arr(_tmpF3,sizeof(void*),1));}});_LL10C:;}_LL107:
return 0;_LL101:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmpF5=
_cycalloc(sizeof(*_tmpF5));_tmpF5->hd=s;_tmpF5->tl=0;_tmpF5;}),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmpF6=_cycalloc(sizeof(*_tmpF6));
_tmpF6->hd=s;_tmpF6->tl=0;_tmpF6;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t)){return Cyc_Toc_malloc_atomic(
s);}return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({
struct Cyc_Absyn_Exp*_tmpF7[2];_tmpF7[1]=s;_tmpF7[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpF7,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmpF8[2];_tmpF8[1]=n;_tmpF8[0]=
s;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpF8,
sizeof(struct Cyc_Absyn_Exp*),2));}),0);}else{return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({
struct Cyc_Absyn_Exp*_tmpF9[2];_tmpF9[1]=n;_tmpF9[0]=s;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpF9,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,
struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({
struct Cyc_Absyn_Exp*_tmpFA[3];_tmpFA[2]=n;_tmpFA[1]=s;_tmpFA[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFA,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return
Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmpFB=_cycalloc(
sizeof(*_tmpFB));_tmpFB->hd=e;_tmpFB->tl=0;_tmpFB;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*
e){int is_string=0;{void*_tmpFC=(void*)e->r;void*_tmpFD;_LL114: if(*((int*)_tmpFC)
== 0){_LL118: _tmpFD=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpFC)->f1;if((
unsigned int)_tmpFD > 1?*((int*)_tmpFD)== 5: 0){goto _LL115;}else{goto _LL116;}}
else{goto _LL116;}_LL116: goto _LL117;_LL115: is_string=1;goto _LL113;_LL117: goto
_LL113;_LL113:;}{struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*xplussz;if(
is_string){struct _tuple0*x=Cyc_Toc_temp_var();void*vd_typ=(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp102;_tmp102.tag=7;_tmp102.f1=(void*)Cyc_Absyn_uchar_t;_tmp102.f2=Cyc_Toc_mt_tq;
_tmp102.f3=(struct Cyc_Absyn_Exp*)sz;_tmp102;});_tmp101;});struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_static_vardecl(x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({
struct Cyc_Absyn_Var_d_struct _tmp100;_tmp100.tag=0;_tmp100.f1=vd;_tmp100;});
_tmpFF;}),0);_tmpFE->tl=Cyc_Toc_result_decls;_tmpFE;});xexp=Cyc_Absyn_var_exp(x,
0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
e,0);xplussz=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,
0),0);}{struct Cyc_Absyn_Exp*urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple4*_tmp103[3];_tmp103[2]=({struct _tuple4*_tmp106=_cycalloc(sizeof(*_tmp106));
_tmp106->f1=0;_tmp106->f2=xplussz;_tmp106;});_tmp103[1]=({struct _tuple4*_tmp105=
_cycalloc(sizeof(*_tmp105));_tmp105->f1=0;_tmp105->f2=xexp;_tmp105;});_tmp103[0]=({
struct _tuple4*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->f1=0;_tmp104->f2=xexp;
_tmp104;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp103,sizeof(struct _tuple4*),3));}),0);return urm_exp;}}}struct Cyc_Toc_Env{
struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;struct Cyc_Core_Opt*
fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp107=_cycalloc(sizeof(*_tmp107));
_tmp107->break_lab=0;_tmp107->continue_lab=0;_tmp107->fallthru_info=0;_tmp107->varmap=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp107->toplevel=1;_tmp107;});}static struct Cyc_Toc_Env*Cyc_Toc_copy_env(
struct Cyc_Toc_Env*e){return({struct Cyc_Toc_Env*_tmp108=_cycalloc(sizeof(*_tmp108));
_tmp108->break_lab=e->break_lab;_tmp108->continue_lab=e->continue_lab;_tmp108->fallthru_info=
e->fallthru_info;_tmp108->varmap=e->varmap;_tmp108->toplevel=e->toplevel;_tmp108;});}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*
ans=Cyc_Toc_copy_env(e);ans->toplevel=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{void*_tmp109=(*x).f1;
_LL11A: if((unsigned int)_tmp109 > 1?*((int*)_tmp109)== 0: 0){goto _LL11B;}else{goto
_LL11C;}_LL11C: goto _LL11D;_LL11B:({struct Cyc_Std_String_pa_struct _tmp10B;_tmp10B.tag=
0;_tmp10B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(x);{void*_tmp10A[1]={&
_tmp10B};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::add_varmap on Rel_n: %s\n",sizeof(unsigned char),30),_tag_arr(
_tmp10A,sizeof(void*),1));}});_LL11D: goto _LL119;_LL119:;}{struct Cyc_Toc_Env*ans=
Cyc_Toc_copy_env(e);ans->varmap=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(ans->varmap,x,y);return
ans;}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*
ans=Cyc_Toc_copy_env(e);ans->break_lab=0;ans->continue_lab=0;return ans;}struct
_tuple6{struct _tagged_arr*f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*f3;};
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct Cyc_Toc_Env*e,
struct _tagged_arr*break_l,struct _tagged_arr*fallthru_l,struct Cyc_List_List*
fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*_tmp10C=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){_tmp10C=({
struct Cyc_List_List*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp10D->tl=_tmp10C;_tmp10D;});}_tmp10C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp10C);{struct Cyc_Toc_Env*ans=Cyc_Toc_copy_env(
e);ans->break_lab=({struct Cyc_Core_Opt*_tmp10E=_cycalloc(sizeof(*_tmp10E));
_tmp10E->v=break_l;_tmp10E;});ans->fallthru_info=({struct Cyc_Core_Opt*_tmp10F=
_cycalloc(sizeof(*_tmp10F));_tmp10F->v=({struct _tuple6*_tmp110=_cycalloc(sizeof(*
_tmp110));_tmp110->f1=fallthru_l;_tmp110->f2=_tmp10C;_tmp110->f3=next_case_env->varmap;
_tmp110;});_tmp10F;});return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*break_l){struct Cyc_Toc_Env*ans=Cyc_Toc_copy_env(
e);ans->break_lab=({struct Cyc_Core_Opt*_tmp111=_cycalloc(sizeof(*_tmp111));
_tmp111->v=break_l;_tmp111;});ans->fallthru_info=0;return ans;}static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env(struct Cyc_Toc_Env*e,struct _tagged_arr*next_l){
struct Cyc_Toc_Env*ans=Cyc_Toc_copy_env(e);ans->break_lab=0;ans->fallthru_info=({
struct Cyc_Core_Opt*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->v=({struct
_tuple6*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->f1=next_l;_tmp113->f2=0;
_tmp113->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))
Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp113;});_tmp112;});return ans;}static void
Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp114=(void*)e->annot;_LL11F: if(*((void**)_tmp114)
== Cyc_CfFlowInfo_UnknownZ){goto _LL120;}else{goto _LL121;}_LL121: if(*((void**)
_tmp114)== Cyc_CfFlowInfo_NotZero){goto _LL122;}else{goto _LL123;}_LL123: if(*((
void**)_tmp114)== Cyc_CfFlowInfo_IsZero){goto _LL124;}else{goto _LL125;}_LL125: if(
_tmp114 == Cyc_Absyn_EmptyAnnot){goto _LL126;}else{goto _LL127;}_LL127: goto _LL128;
_LL120: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL122: return 0;_LL124:({void*_tmp115[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(unsigned char),28),_tag_arr(_tmp115,sizeof(void*),0));});return 0;_LL126:({
void*_tmp116[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("!",sizeof(
unsigned char),2),_tag_arr(_tmp116,sizeof(void*),0));});({void*_tmp117[0]={};Cyc_Tcutil_warn(
e->loc,_tag_arr("unreachable dereference",sizeof(unsigned char),24),_tag_arr(
_tmp117,sizeof(void*),0));});return 0;_LL128:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118[0]=({struct Cyc_Core_Impossible_struct
_tmp119;_tmp119.tag=Cyc_Core_Impossible;_tmp119.f1=_tag_arr("need_null_check",
sizeof(unsigned char),16);_tmp119;});_tmp118;}));_LL11E:;}static struct Cyc_List_List*
Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp11A=(void*)e->annot;struct Cyc_List_List*
_tmp11B;struct Cyc_List_List*_tmp11C;_LL12A: if(*((void**)_tmp11A)== Cyc_CfFlowInfo_UnknownZ){
_LL134: _tmp11B=((struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp11A)->f1;goto _LL12B;}
else{goto _LL12C;}_LL12C: if(*((void**)_tmp11A)== Cyc_CfFlowInfo_NotZero){_LL135:
_tmp11C=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmp11A)->f1;goto _LL12D;}else{
goto _LL12E;}_LL12E: if(*((void**)_tmp11A)== Cyc_CfFlowInfo_IsZero){goto _LL12F;}
else{goto _LL130;}_LL130: if(_tmp11A == Cyc_Absyn_EmptyAnnot){goto _LL131;}else{goto
_LL132;}_LL132: goto _LL133;_LL12B: return _tmp11B;_LL12D: return _tmp11C;_LL12F:({
void*_tmp11D[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(unsigned char),28),_tag_arr(_tmp11D,sizeof(void*),0));});return 0;_LL131:
return 0;_LL133:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp11E=
_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_Core_Impossible_struct _tmp11F;
_tmp11F.tag=Cyc_Core_Impossible;_tmp11F.f1=_tag_arr("get_relns",sizeof(
unsigned char),10);_tmp11F;});_tmp11E;}));_LL129:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp120=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp121;struct Cyc_Absyn_Conref*_tmp122;struct Cyc_Absyn_Exp*_tmp123;_LL137: if((
unsigned int)_tmp120 > 3?*((int*)_tmp120)== 4: 0){_LL13D: _tmp121=((struct Cyc_Absyn_PointerType_struct*)
_tmp120)->f1;_LL13E: _tmp122=_tmp121.bounds;goto _LL138;}else{goto _LL139;}_LL139:
if((unsigned int)_tmp120 > 3?*((int*)_tmp120)== 7: 0){_LL13F: _tmp123=((struct Cyc_Absyn_ArrayType_struct*)
_tmp120)->f3;goto _LL13A;}else{goto _LL13B;}_LL13B: goto _LL13C;_LL138: {void*
_tmp124=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp122);struct Cyc_Absyn_Exp*
_tmp125;_LL141: if((int)_tmp124 == 0){goto _LL142;}else{goto _LL143;}_LL143: if((
unsigned int)_tmp124 > 1?*((int*)_tmp124)== 0: 0){_LL145: _tmp125=((struct Cyc_Absyn_Upper_b_struct*)
_tmp124)->f1;goto _LL144;}else{goto _LL140;}_LL142: return 0;_LL144: {unsigned int j=
Cyc_Evexp_eval_const_uint_exp(_tmp125);return i <= j;}_LL140:;}_LL13A: if(_tmp123 == 
0){return 0;}{unsigned int j=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp123));return i <= j;}_LL13C: return 0;_LL136:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp126=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp126->vd != y){continue;}{void*_tmp127=(void*)_tmp126->rop;struct
Cyc_Absyn_Vardecl*_tmp128;struct Cyc_Absyn_Vardecl*_tmp129;_LL147: if(*((int*)
_tmp127)== 2){_LL14D: _tmp128=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp127)->f1;
goto _LL148;}else{goto _LL149;}_LL149: if(*((int*)_tmp127)== 4){_LL14E: _tmp129=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp127)->f1;goto _LL14A;}else{goto _LL14B;}
_LL14B: goto _LL14C;_LL148: _tmp129=_tmp128;goto _LL14A;_LL14A: if(_tmp129 == v){
return 1;}else{goto _LL146;}_LL14C: continue;_LL146:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp12A=(void*)e->r;void*_tmp12B;struct Cyc_Absyn_Vardecl*_tmp12C;void*_tmp12D;
struct Cyc_Absyn_Vardecl*_tmp12E;void*_tmp12F;struct Cyc_Absyn_Vardecl*_tmp130;
void*_tmp131;struct Cyc_Absyn_Vardecl*_tmp132;struct Cyc_List_List*_tmp133;struct
Cyc_List_List _tmp134;struct Cyc_Absyn_Exp*_tmp135;void*_tmp136;_LL150: if(*((int*)
_tmp12A)== 1){_LL15C: _tmp12B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12A)->f2;
if((unsigned int)_tmp12B > 1?*((int*)_tmp12B)== 4: 0){_LL15D: _tmp12C=((struct Cyc_Absyn_Pat_b_struct*)
_tmp12B)->f1;goto _LL151;}else{goto _LL152;}}else{goto _LL152;}_LL152: if(*((int*)
_tmp12A)== 1){_LL15E: _tmp12D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12A)->f2;
if((unsigned int)_tmp12D > 1?*((int*)_tmp12D)== 3: 0){_LL15F: _tmp12E=((struct Cyc_Absyn_Local_b_struct*)
_tmp12D)->f1;goto _LL153;}else{goto _LL154;}}else{goto _LL154;}_LL154: if(*((int*)
_tmp12A)== 1){_LL160: _tmp12F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12A)->f2;
if((unsigned int)_tmp12F > 1?*((int*)_tmp12F)== 0: 0){_LL161: _tmp130=((struct Cyc_Absyn_Global_b_struct*)
_tmp12F)->f1;goto _LL155;}else{goto _LL156;}}else{goto _LL156;}_LL156: if(*((int*)
_tmp12A)== 1){_LL162: _tmp131=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12A)->f2;
if((unsigned int)_tmp131 > 1?*((int*)_tmp131)== 2: 0){_LL163: _tmp132=((struct Cyc_Absyn_Param_b_struct*)
_tmp131)->f1;goto _LL157;}else{goto _LL158;}}else{goto _LL158;}_LL158: if(*((int*)
_tmp12A)== 3){_LL166: _tmp136=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp12A)->f1;
if((int)_tmp136 == 19){goto _LL164;}else{goto _LL15A;}_LL164: _tmp133=((struct Cyc_Absyn_Primop_e_struct*)
_tmp12A)->f2;if(_tmp133 == 0){goto _LL15A;}else{_tmp134=*_tmp133;_LL165: _tmp135=(
struct Cyc_Absyn_Exp*)_tmp134.hd;goto _LL159;}}else{goto _LL15A;}_LL15A: goto _LL15B;
_LL151: _tmp12E=_tmp12C;goto _LL153;_LL153: _tmp130=_tmp12E;goto _LL155;_LL155:
_tmp132=_tmp130;goto _LL157;_LL157: if(_tmp132->escapes){return 0;}if(Cyc_Toc_check_leq_size_var(
relns,v,_tmp132)){return 1;}goto _LL14F;_LL159:{void*_tmp137=(void*)_tmp135->r;
void*_tmp138;struct Cyc_Absyn_Vardecl*_tmp139;void*_tmp13A;struct Cyc_Absyn_Vardecl*
_tmp13B;void*_tmp13C;struct Cyc_Absyn_Vardecl*_tmp13D;void*_tmp13E;struct Cyc_Absyn_Vardecl*
_tmp13F;_LL168: if(*((int*)_tmp137)== 1){_LL172: _tmp138=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp137)->f2;if((unsigned int)_tmp138 > 1?*((int*)_tmp138)== 4: 0){_LL173: _tmp139=((
struct Cyc_Absyn_Pat_b_struct*)_tmp138)->f1;goto _LL169;}else{goto _LL16A;}}else{
goto _LL16A;}_LL16A: if(*((int*)_tmp137)== 1){_LL174: _tmp13A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp137)->f2;if((unsigned int)_tmp13A > 1?*((int*)_tmp13A)== 3: 0){_LL175: _tmp13B=((
struct Cyc_Absyn_Local_b_struct*)_tmp13A)->f1;goto _LL16B;}else{goto _LL16C;}}else{
goto _LL16C;}_LL16C: if(*((int*)_tmp137)== 1){_LL176: _tmp13C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp137)->f2;if((unsigned int)_tmp13C > 1?*((int*)_tmp13C)== 0: 0){_LL177: _tmp13D=((
struct Cyc_Absyn_Global_b_struct*)_tmp13C)->f1;goto _LL16D;}else{goto _LL16E;}}
else{goto _LL16E;}_LL16E: if(*((int*)_tmp137)== 1){_LL178: _tmp13E=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp137)->f2;if((unsigned int)_tmp13E > 1?*((int*)_tmp13E)
== 2: 0){_LL179: _tmp13F=((struct Cyc_Absyn_Param_b_struct*)_tmp13E)->f1;goto _LL16F;}
else{goto _LL170;}}else{goto _LL170;}_LL170: goto _LL171;_LL169: _tmp13B=_tmp139;goto
_LL16B;_LL16B: _tmp13D=_tmp13B;goto _LL16D;_LL16D: _tmp13F=_tmp13D;goto _LL16F;
_LL16F: return _tmp13F == v;_LL171: goto _LL167;_LL167:;}goto _LL14F;_LL15B: goto _LL14F;
_LL14F:;}return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct
Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp140=(void*)a->r;void*_tmp141;
struct Cyc_Absyn_Vardecl*_tmp142;void*_tmp143;struct Cyc_Absyn_Vardecl*_tmp144;
void*_tmp145;struct Cyc_Absyn_Vardecl*_tmp146;void*_tmp147;struct Cyc_Absyn_Vardecl*
_tmp148;_LL17B: if(*((int*)_tmp140)== 1){_LL185: _tmp141=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp140)->f2;if((unsigned int)_tmp141 > 1?*((int*)_tmp141)== 4: 0){_LL186: _tmp142=((
struct Cyc_Absyn_Pat_b_struct*)_tmp141)->f1;goto _LL17C;}else{goto _LL17D;}}else{
goto _LL17D;}_LL17D: if(*((int*)_tmp140)== 1){_LL187: _tmp143=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp140)->f2;if((unsigned int)_tmp143 > 1?*((int*)_tmp143)== 3: 0){_LL188: _tmp144=((
struct Cyc_Absyn_Local_b_struct*)_tmp143)->f1;goto _LL17E;}else{goto _LL17F;}}else{
goto _LL17F;}_LL17F: if(*((int*)_tmp140)== 1){_LL189: _tmp145=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp140)->f2;if((unsigned int)_tmp145 > 1?*((int*)_tmp145)== 0: 0){_LL18A: _tmp146=((
struct Cyc_Absyn_Global_b_struct*)_tmp145)->f1;goto _LL180;}else{goto _LL181;}}
else{goto _LL181;}_LL181: if(*((int*)_tmp140)== 1){_LL18B: _tmp147=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp140)->f2;if((unsigned int)_tmp147 > 1?*((int*)_tmp147)
== 2: 0){_LL18C: _tmp148=((struct Cyc_Absyn_Param_b_struct*)_tmp147)->f1;goto _LL182;}
else{goto _LL183;}}else{goto _LL183;}_LL183: goto _LL184;_LL17C: _tmp144=_tmp142;goto
_LL17E;_LL17E: _tmp146=_tmp144;goto _LL180;_LL180: _tmp148=_tmp146;goto _LL182;
_LL182: if(_tmp148->escapes){return 0;}inner_loop: {void*_tmp149=(void*)i->r;
struct Cyc_Absyn_Exp*_tmp14A;void*_tmp14B;void*_tmp14C;int _tmp14D;void*_tmp14E;
void*_tmp14F;int _tmp150;void*_tmp151;struct Cyc_List_List*_tmp152;struct Cyc_List_List
_tmp153;struct Cyc_List_List*_tmp154;struct Cyc_List_List _tmp155;struct Cyc_Absyn_Exp*
_tmp156;struct Cyc_Absyn_Exp*_tmp157;void*_tmp158;void*_tmp159;struct Cyc_Absyn_Vardecl*
_tmp15A;void*_tmp15B;struct Cyc_Absyn_Vardecl*_tmp15C;void*_tmp15D;struct Cyc_Absyn_Vardecl*
_tmp15E;void*_tmp15F;struct Cyc_Absyn_Vardecl*_tmp160;_LL18E: if(*((int*)_tmp149)
== 13){_LL1A1: _tmp14B=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp149)->f1;goto
_LL1A0;_LL1A0: _tmp14A=((struct Cyc_Absyn_Cast_e_struct*)_tmp149)->f2;goto _LL18F;}
else{goto _LL190;}_LL190: if(*((int*)_tmp149)== 0){_LL1A2: _tmp14C=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmp149)->f1;if((unsigned int)_tmp14C > 1?*((int*)
_tmp14C)== 2: 0){_LL1A4: _tmp14E=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp14C)->f1;
if((int)_tmp14E == 0){goto _LL1A3;}else{goto _LL192;}_LL1A3: _tmp14D=((struct Cyc_Absyn_Int_c_struct*)
_tmp14C)->f2;goto _LL191;}else{goto _LL192;}}else{goto _LL192;}_LL192: if(*((int*)
_tmp149)== 0){_LL1A5: _tmp14F=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp149)->f1;
if((unsigned int)_tmp14F > 1?*((int*)_tmp14F)== 2: 0){_LL1A7: _tmp151=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp14F)->f1;if((int)_tmp151 == 1){goto _LL1A6;}
else{goto _LL194;}_LL1A6: _tmp150=((struct Cyc_Absyn_Int_c_struct*)_tmp14F)->f2;
goto _LL193;}else{goto _LL194;}}else{goto _LL194;}_LL194: if(*((int*)_tmp149)== 3){
_LL1AC: _tmp158=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp149)->f1;if((int)
_tmp158 == 4){goto _LL1A8;}else{goto _LL196;}_LL1A8: _tmp152=((struct Cyc_Absyn_Primop_e_struct*)
_tmp149)->f2;if(_tmp152 == 0){goto _LL196;}else{_tmp153=*_tmp152;_LL1AB: _tmp157=(
struct Cyc_Absyn_Exp*)_tmp153.hd;goto _LL1A9;_LL1A9: _tmp154=_tmp153.tl;if(_tmp154
== 0){goto _LL196;}else{_tmp155=*_tmp154;_LL1AA: _tmp156=(struct Cyc_Absyn_Exp*)
_tmp155.hd;goto _LL195;}}}else{goto _LL196;}_LL196: if(*((int*)_tmp149)== 1){_LL1AD:
_tmp159=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp149)->f2;if((unsigned int)
_tmp159 > 1?*((int*)_tmp159)== 4: 0){_LL1AE: _tmp15A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp159)->f1;goto _LL197;}else{goto _LL198;}}else{goto _LL198;}_LL198: if(*((int*)
_tmp149)== 1){_LL1AF: _tmp15B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp149)->f2;
if((unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 3: 0){_LL1B0: _tmp15C=((struct Cyc_Absyn_Local_b_struct*)
_tmp15B)->f1;goto _LL199;}else{goto _LL19A;}}else{goto _LL19A;}_LL19A: if(*((int*)
_tmp149)== 1){_LL1B1: _tmp15D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp149)->f2;
if((unsigned int)_tmp15D > 1?*((int*)_tmp15D)== 0: 0){_LL1B2: _tmp15E=((struct Cyc_Absyn_Global_b_struct*)
_tmp15D)->f1;goto _LL19B;}else{goto _LL19C;}}else{goto _LL19C;}_LL19C: if(*((int*)
_tmp149)== 1){_LL1B3: _tmp15F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp149)->f2;
if((unsigned int)_tmp15F > 1?*((int*)_tmp15F)== 2: 0){_LL1B4: _tmp160=((struct Cyc_Absyn_Param_b_struct*)
_tmp15F)->f1;goto _LL19D;}else{goto _LL19E;}}else{goto _LL19E;}_LL19E: goto _LL19F;
_LL18F: i=_tmp14A;goto inner_loop;_LL191: return _tmp14D >= 0? Cyc_Toc_check_const_array((
unsigned int)(_tmp14D + 1),(void*)_tmp148->type): 0;_LL193: return Cyc_Toc_check_const_array((
unsigned int)(_tmp150 + 1),(void*)_tmp148->type);_LL195: return Cyc_Toc_check_leq_size(
relns,_tmp148,_tmp156);_LL197: _tmp15C=_tmp15A;goto _LL199;_LL199: _tmp15E=_tmp15C;
goto _LL19B;_LL19B: _tmp160=_tmp15E;goto _LL19D;_LL19D: if(_tmp160->escapes){return 0;}{
struct Cyc_List_List*_tmp161=relns;for(0;_tmp161 != 0;_tmp161=_tmp161->tl){struct
Cyc_CfFlowInfo_Reln*_tmp162=(struct Cyc_CfFlowInfo_Reln*)_tmp161->hd;if(_tmp162->vd
== _tmp160){void*_tmp163=(void*)_tmp162->rop;struct Cyc_Absyn_Vardecl*_tmp164;
struct Cyc_Absyn_Vardecl*_tmp165;unsigned int _tmp166;_LL1B6: if(*((int*)_tmp163)== 
2){_LL1BE: _tmp164=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp163)->f1;goto
_LL1B7;}else{goto _LL1B8;}_LL1B8: if(*((int*)_tmp163)== 1){_LL1BF: _tmp165=((struct
Cyc_CfFlowInfo_LessVar_struct*)_tmp163)->f1;goto _LL1B9;}else{goto _LL1BA;}_LL1BA:
if(*((int*)_tmp163)== 3){_LL1C0: _tmp166=((struct Cyc_CfFlowInfo_LessConst_struct*)
_tmp163)->f1;goto _LL1BB;}else{goto _LL1BC;}_LL1BC: goto _LL1BD;_LL1B7: if(_tmp148 == 
_tmp164){return 1;}else{goto _LL1B5;}_LL1B9:{struct Cyc_List_List*_tmp167=relns;
for(0;_tmp167 != 0;_tmp167=_tmp167->tl){struct Cyc_CfFlowInfo_Reln*_tmp168=(struct
Cyc_CfFlowInfo_Reln*)_tmp167->hd;if(_tmp168->vd == _tmp165){void*_tmp169=(void*)
_tmp168->rop;struct Cyc_Absyn_Vardecl*_tmp16A;struct Cyc_Absyn_Vardecl*_tmp16B;
unsigned int _tmp16C;struct Cyc_Absyn_Vardecl*_tmp16D;_LL1C2: if(*((int*)_tmp169)== 
4){_LL1CC: _tmp16A=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp169)->f1;goto
_LL1C3;}else{goto _LL1C4;}_LL1C4: if(*((int*)_tmp169)== 2){_LL1CD: _tmp16B=((struct
Cyc_CfFlowInfo_LessSize_struct*)_tmp169)->f1;goto _LL1C5;}else{goto _LL1C6;}_LL1C6:
if(*((int*)_tmp169)== 0){_LL1CE: _tmp16C=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp169)->f1;goto _LL1C7;}else{goto _LL1C8;}_LL1C8: if(*((int*)_tmp169)== 1){_LL1CF:
_tmp16D=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp169)->f1;goto _LL1C9;}else{
goto _LL1CA;}_LL1CA: goto _LL1CB;_LL1C3: _tmp16B=_tmp16A;goto _LL1C5;_LL1C5: if(
_tmp148 == _tmp16B){return 1;}else{goto _LL1C1;}_LL1C7: return Cyc_Toc_check_const_array(
_tmp16C,(void*)_tmp148->type);_LL1C9: if(Cyc_Toc_check_leq_size_var(relns,_tmp148,
_tmp16D)){return 1;}else{goto _LL1C1;}_LL1CB: goto _LL1C1;_LL1C1:;}}}goto _LL1B5;
_LL1BB: return Cyc_Toc_check_const_array(_tmp166,(void*)_tmp148->type);_LL1BD: goto
_LL1B5;_LL1B5:;}}}goto _LL18D;_LL19F: goto _LL18D;_LL18D:;}goto _LL17A;_LL184: goto
_LL17A;_LL17A:;}return 0;}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){if(
e->topt == 0){({void*_tmp16E[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",sizeof(
unsigned char),24),_tag_arr(_tmp16E,sizeof(void*),0));});}return Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){({void*_tmp16F[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",
sizeof(unsigned char),24),_tag_arr(_tmp16F,sizeof(void*),0));});}return(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple3*Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp*e){return({struct _tuple3*_tmp170=_cycalloc(sizeof(*_tmp170));
_tmp170->f1=Cyc_Toc_mt_tq;_tmp170->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp170;});}static struct _tuple4*Cyc_Toc_add_designator(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);return({
struct _tuple4*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->f1=0;_tmp171->f2=e;
_tmp171;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,
struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*
rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp172=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0){eo=(struct Cyc_Absyn_Exp*)(
is_atomic? Cyc_Toc_malloc_atomic(_tmp172): Cyc_Toc_malloc_ptr(_tmp172));}else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(rgnopt);Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp172);}}}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int e1_already_translated);static struct
Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp173=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp173 != 0;_tmp173=_tmp173->tl){struct _tuple4 _tmp175;struct Cyc_Absyn_Exp*
_tmp176;struct Cyc_List_List*_tmp177;struct _tuple4*_tmp174=(struct _tuple4*)
_tmp173->hd;_tmp175=*_tmp174;_LL1D2: _tmp177=_tmp175.f1;goto _LL1D1;_LL1D1: _tmp176=
_tmp175.f2;goto _LL1D0;_LL1D0: {struct Cyc_Absyn_Exp*e_index;if(_tmp177 == 0){
e_index=Cyc_Absyn_signed_int_exp(count --,0);}else{if(_tmp177->tl != 0){({void*
_tmp178[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("multiple designators in array",sizeof(unsigned char),30),_tag_arr(
_tmp178,sizeof(void*),0));});}{void*_tmp179=(void*)_tmp177->hd;void*_tmp17A=
_tmp179;struct Cyc_Absyn_Exp*_tmp17B;_LL1D4: if(*((int*)_tmp17A)== 0){_LL1D8:
_tmp17B=((struct Cyc_Absyn_ArrayElement_struct*)_tmp17A)->f1;goto _LL1D5;}else{
goto _LL1D6;}_LL1D6: if(*((int*)_tmp17A)== 1){goto _LL1D7;}else{goto _LL1D3;}_LL1D5:
Cyc_Toc_exp_to_c(nv,_tmp17B);e_index=_tmp17B;goto _LL1D3;_LL1D7:({void*_tmp17C[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("field name designators in array",
sizeof(unsigned char),32),_tag_arr(_tmp17C,sizeof(void*),0));});_LL1D3:;}}{
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp17D=(
void*)_tmp176->r;struct Cyc_List_List*_tmp17E;struct Cyc_Absyn_Exp*_tmp17F;struct
Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Vardecl*_tmp181;struct Cyc_List_List*
_tmp182;void*_tmp183;_LL1DA: if(*((int*)_tmp17D)== 26){_LL1E2: _tmp17E=((struct Cyc_Absyn_Array_e_struct*)
_tmp17D)->f1;goto _LL1DB;}else{goto _LL1DC;}_LL1DC: if(*((int*)_tmp17D)== 27){
_LL1E5: _tmp181=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp17D)->f1;goto _LL1E4;
_LL1E4: _tmp180=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp17D)->f2;goto _LL1E3;
_LL1E3: _tmp17F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp17D)->f3;goto _LL1DD;}
else{goto _LL1DE;}_LL1DE: if(*((int*)_tmp17D)== 29){_LL1E7: _tmp183=(void*)((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp17D)->f1;goto _LL1E6;_LL1E6: _tmp182=((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp17D)->f2;goto _LL1DF;}else{goto _LL1E0;}_LL1E0:
goto _LL1E1;_LL1DB: s=Cyc_Toc_init_array(nv,lval,_tmp17E,s);goto _LL1D9;_LL1DD: s=
Cyc_Toc_init_comprehension(nv,lval,_tmp181,_tmp180,_tmp17F,s,0);goto _LL1D9;
_LL1DF: s=Cyc_Toc_init_anon_struct(nv,lval,_tmp183,_tmp182,s);goto _LL1D9;_LL1E1:
Cyc_Toc_exp_to_c(nv,_tmp176);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp176,0),s,0);goto _LL1D9;_LL1D9:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
s,int e1_already_translated){struct _tuple0*_tmp184=vd->name;if(!
e1_already_translated){Cyc_Toc_exp_to_c(nv,e1);}{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
nv,_tmp184,Cyc_Absyn_varb_exp(_tmp184,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C[0]=({struct Cyc_Absyn_Local_b_struct
_tmp18D;_tmp18D.tag=3;_tmp18D.f1=vd;_tmp18D;});_tmp18C;}),0));struct _tuple0*max=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(
_tmp184,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(
Cyc_Absyn_var_exp(_tmp184,0),Cyc_Absyn_var_exp(max,0),0);struct Cyc_Absyn_Exp*ec=
Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp184,0),0);struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp184,0),0);struct Cyc_Absyn_Stmt*
body;{void*_tmp185=(void*)e2->r;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Exp*
_tmp187;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Vardecl*_tmp189;struct Cyc_List_List*
_tmp18A;void*_tmp18B;_LL1E9: if(*((int*)_tmp185)== 26){_LL1F1: _tmp186=((struct Cyc_Absyn_Array_e_struct*)
_tmp185)->f1;goto _LL1EA;}else{goto _LL1EB;}_LL1EB: if(*((int*)_tmp185)== 27){
_LL1F4: _tmp189=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp185)->f1;goto _LL1F3;
_LL1F3: _tmp188=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp185)->f2;goto _LL1F2;
_LL1F2: _tmp187=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp185)->f3;goto _LL1EC;}
else{goto _LL1ED;}_LL1ED: if(*((int*)_tmp185)== 29){_LL1F6: _tmp18B=(void*)((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp185)->f1;goto _LL1F5;_LL1F5: _tmp18A=((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp185)->f2;goto _LL1EE;}else{goto _LL1EF;}_LL1EF:
goto _LL1F0;_LL1EA: body=Cyc_Toc_init_array(nv2,lval,_tmp186,Cyc_Absyn_skip_stmt(0));
goto _LL1E8;_LL1EC: body=Cyc_Toc_init_comprehension(nv2,lval,_tmp189,_tmp188,
_tmp187,Cyc_Absyn_skip_stmt(0),0);goto _LL1E8;_LL1EE: body=Cyc_Toc_init_anon_struct(
nv,lval,_tmp18B,_tmp18A,Cyc_Absyn_skip_stmt(0));goto _LL1E8;_LL1F0: Cyc_Toc_exp_to_c(
nv2,e2);body=Cyc_Absyn_assign_stmt(lval,e2,0);goto _LL1E8;_LL1E8:;}{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(
max,Cyc_Absyn_uint_t,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp184,Cyc_Absyn_uint_t,
0,s2,0),0),s,0);return s;}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s){{struct Cyc_List_List*_tmp18E=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp18E != 0;_tmp18E=_tmp18E->tl){
struct _tuple4 _tmp190;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_List_List*_tmp192;
struct _tuple4*_tmp18F=(struct _tuple4*)_tmp18E->hd;_tmp190=*_tmp18F;_LL1F9:
_tmp192=_tmp190.f1;goto _LL1F8;_LL1F8: _tmp191=_tmp190.f2;goto _LL1F7;_LL1F7: if(
_tmp192 == 0){({void*_tmp193[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",sizeof(
unsigned char),22),_tag_arr(_tmp193,sizeof(void*),0));});}if(_tmp192->tl != 0){({
void*_tmp194[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("too many designators in anonymous struct",sizeof(unsigned char),41),
_tag_arr(_tmp194,sizeof(void*),0));});}{void*_tmp195=(void*)_tmp192->hd;struct
_tagged_arr*_tmp196;_LL1FB: if(*((int*)_tmp195)== 1){_LL1FF: _tmp196=((struct Cyc_Absyn_FieldName_struct*)
_tmp195)->f1;goto _LL1FC;}else{goto _LL1FD;}_LL1FD: goto _LL1FE;_LL1FC: {struct Cyc_Absyn_Exp*
lval=Cyc_Absyn_structmember_exp(lhs,_tmp196,0);{void*_tmp197=(void*)_tmp191->r;
struct Cyc_List_List*_tmp198;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*
_tmp19A;struct Cyc_Absyn_Vardecl*_tmp19B;struct Cyc_List_List*_tmp19C;void*_tmp19D;
_LL201: if(*((int*)_tmp197)== 26){_LL209: _tmp198=((struct Cyc_Absyn_Array_e_struct*)
_tmp197)->f1;goto _LL202;}else{goto _LL203;}_LL203: if(*((int*)_tmp197)== 27){
_LL20C: _tmp19B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp197)->f1;goto _LL20B;
_LL20B: _tmp19A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp197)->f2;goto _LL20A;
_LL20A: _tmp199=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp197)->f3;goto _LL204;}
else{goto _LL205;}_LL205: if(*((int*)_tmp197)== 29){_LL20E: _tmp19D=(void*)((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp197)->f1;goto _LL20D;_LL20D: _tmp19C=((struct
Cyc_Absyn_AnonStruct_e_struct*)_tmp197)->f2;goto _LL206;}else{goto _LL207;}_LL207:
goto _LL208;_LL202: s=Cyc_Toc_init_array(nv,lval,_tmp198,s);goto _LL200;_LL204: s=
Cyc_Toc_init_comprehension(nv,lval,_tmp19B,_tmp19A,_tmp199,s,0);goto _LL200;
_LL206: s=Cyc_Toc_init_anon_struct(nv,lval,_tmp19D,_tmp19C,s);goto _LL200;_LL208:
Cyc_Toc_exp_to_c(nv,_tmp191);if(Cyc_Toc_is_poly_field(struct_type,_tmp196)){
_tmp191=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp191,0);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp191,0),0),s,0);goto _LL200;
_LL200:;}goto _LL1FA;}_LL1FE:({void*_tmp19E[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(unsigned char),27),_tag_arr(_tmp19E,sizeof(void*),0));});_LL1FA:;}}}
return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int
pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct _RegionHandle
_tmp19F=_new_region("r");struct _RegionHandle*r=& _tmp19F;_push_region(r);{struct
Cyc_List_List*_tmp1A0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple3*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,
es);struct _tagged_arr*_tmp1A1=Cyc_Toc_add_tuple_type(_tmp1A0);struct _tuple0*
_tmp1A2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1A3=Cyc_Absyn_var_exp(_tmp1A2,
0);struct Cyc_Absyn_Stmt*_tmp1A4=Cyc_Absyn_exp_stmt(_tmp1A3,0);struct Cyc_Absyn_Exp*(*
_tmp1A5)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;int is_atomic=1;
struct Cyc_List_List*_tmp1A6=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp1A6);for(0;_tmp1A6 != 0;(_tmp1A6=_tmp1A6->tl,-- i)){struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_tmp1A6->hd;struct Cyc_Absyn_Exp*lval=_tmp1A5(_tmp1A3,Cyc_Absyn_fieldname(
i),0);is_atomic=is_atomic? Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v): 0;{void*_tmp1A7=(void*)e->r;struct Cyc_List_List*
_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Vardecl*
_tmp1AB;_LL210: if(*((int*)_tmp1A7)== 26){_LL216: _tmp1A8=((struct Cyc_Absyn_Array_e_struct*)
_tmp1A7)->f1;goto _LL211;}else{goto _LL212;}_LL212: if(*((int*)_tmp1A7)== 27){
_LL219: _tmp1AB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A7)->f1;goto _LL218;
_LL218: _tmp1AA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A7)->f2;goto _LL217;
_LL217: _tmp1A9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A7)->f3;goto _LL213;}
else{goto _LL214;}_LL214: goto _LL215;_LL211: _tmp1A4=Cyc_Toc_init_array(nv,lval,
_tmp1A8,_tmp1A4);goto _LL20F;_LL213: _tmp1A4=Cyc_Toc_init_comprehension(nv,lval,
_tmp1AB,_tmp1AA,_tmp1A9,_tmp1A4,0);goto _LL20F;_LL215: Cyc_Toc_exp_to_c(nv,e);
_tmp1A4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1A5(
_tmp1A3,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp1A4,0);goto _LL20F;_LL20F:;}}}{
struct Cyc_Absyn_Exp*_tmp1AC=Cyc_Toc_make_struct(nv,_tmp1A2,Cyc_Absyn_strct(
_tmp1A1),_tmp1A4,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp1AC;}};
_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
dles,struct _tuple0*tdn){struct _tuple0*_tmp1AD=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp1AE=Cyc_Absyn_var_exp(_tmp1AD,0);struct Cyc_Absyn_Stmt*_tmp1AF=Cyc_Absyn_exp_stmt(
_tmp1AE,0);struct Cyc_Absyn_Exp*(*_tmp1B0)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,
struct Cyc_Position_Segment*)=pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic=1;struct _RegionHandle _tmp1B1=_new_region("r");struct _RegionHandle*r=&
_tmp1B1;_push_region(r);{struct Cyc_List_List*_tmp1B2=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp1B2
!= 0;_tmp1B2=_tmp1B2->tl){struct _tuple4 _tmp1B4;struct Cyc_Absyn_Exp*_tmp1B5;
struct Cyc_List_List*_tmp1B6;struct _tuple4*_tmp1B3=(struct _tuple4*)_tmp1B2->hd;
_tmp1B4=*_tmp1B3;_LL21C: _tmp1B6=_tmp1B4.f1;goto _LL21B;_LL21B: _tmp1B5=_tmp1B4.f2;
goto _LL21A;_LL21A: is_atomic=is_atomic? Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1B5->topt))->v): 0;if(_tmp1B6 == 0){({void*_tmp1B7[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",
sizeof(unsigned char),22),_tag_arr(_tmp1B7,sizeof(void*),0));});}if(_tmp1B6->tl
!= 0){struct _tuple0*_tmp1B8=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1B9=Cyc_Absyn_var_exp(
_tmp1B8,0);for(0;_tmp1B6 != 0;_tmp1B6=_tmp1B6->tl){void*_tmp1BA=(void*)_tmp1B6->hd;
struct _tagged_arr*_tmp1BB;_LL21E: if(*((int*)_tmp1BA)== 1){_LL222: _tmp1BB=((
struct Cyc_Absyn_FieldName_struct*)_tmp1BA)->f1;goto _LL21F;}else{goto _LL220;}
_LL220: goto _LL221;_LL21F: if(Cyc_Toc_is_poly_field(struct_type,_tmp1BB)){_tmp1B9=
Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp1B9,0);}_tmp1AF=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1B0(_tmp1AE,_tmp1BB,0),_tmp1B9,0),0),
_tmp1AF,0);goto _LL21D;_LL221:({void*_tmp1BC[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(unsigned char),27),_tag_arr(_tmp1BC,sizeof(void*),0));});_LL21D:;}Cyc_Toc_exp_to_c(
nv,_tmp1B5);_tmp1AF=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp1B9,_tmp1B5,0),0),_tmp1AF,0);}else{void*_tmp1BD=(void*)_tmp1B6->hd;struct
_tagged_arr*_tmp1BE;_LL224: if(*((int*)_tmp1BD)== 1){_LL228: _tmp1BE=((struct Cyc_Absyn_FieldName_struct*)
_tmp1BD)->f1;goto _LL225;}else{goto _LL226;}_LL226: goto _LL227;_LL225: {struct Cyc_Absyn_Exp*
lval=_tmp1B0(_tmp1AE,_tmp1BE,0);{void*_tmp1BF=(void*)_tmp1B5->r;struct Cyc_List_List*
_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Vardecl*
_tmp1C3;struct Cyc_List_List*_tmp1C4;void*_tmp1C5;_LL22A: if(*((int*)_tmp1BF)== 26){
_LL232: _tmp1C0=((struct Cyc_Absyn_Array_e_struct*)_tmp1BF)->f1;goto _LL22B;}else{
goto _LL22C;}_LL22C: if(*((int*)_tmp1BF)== 27){_LL235: _tmp1C3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1BF)->f1;goto _LL234;_LL234: _tmp1C2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1BF)->f2;goto _LL233;_LL233: _tmp1C1=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1BF)->f3;goto _LL22D;}else{goto _LL22E;}_LL22E: if(*((int*)_tmp1BF)== 29){
_LL237: _tmp1C5=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1BF)->f1;goto
_LL236;_LL236: _tmp1C4=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1BF)->f2;goto
_LL22F;}else{goto _LL230;}_LL230: goto _LL231;_LL22B: _tmp1AF=Cyc_Toc_init_array(nv,
lval,_tmp1C0,_tmp1AF);goto _LL229;_LL22D: _tmp1AF=Cyc_Toc_init_comprehension(nv,
lval,_tmp1C3,_tmp1C2,_tmp1C1,_tmp1AF,0);goto _LL229;_LL22F: _tmp1AF=Cyc_Toc_init_anon_struct(
nv,lval,_tmp1C5,_tmp1C4,_tmp1AF);goto _LL229;_LL231: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1B5->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp1B5);if(Cyc_Toc_is_poly_field(struct_type,_tmp1BE)? ! was_ptr_type: 0){_tmp1B5=
Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp1B5,0);}_tmp1AF=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1B5,0),0),_tmp1AF,0);goto _LL229;}
_LL229:;}goto _LL223;}_LL227:({void*_tmp1C6[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(unsigned char),27),_tag_arr(_tmp1C6,sizeof(void*),0));});_LL223:;}}}{
struct Cyc_Absyn_Exp*_tmp1C7=Cyc_Toc_make_struct(nv,_tmp1AD,Cyc_Absyn_strctq(tdn),
_tmp1AF,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp1C7;};_pop_region(r);}
struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple7*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));
_tmp1C8[0]=({struct Cyc_Absyn_Increment_e_struct _tmp1C9;_tmp1C9.tag=5;_tmp1C9.f1=
e1;_tmp1C9.f2=(void*)incr;_tmp1C9;});_tmp1C8;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp1CA=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp1CB;struct Cyc_Absyn_Exp*
_tmp1CC;void*_tmp1CD;struct _tagged_arr*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1CF;
_LL239: if(*((int*)_tmp1CA)== 35){_LL241: _tmp1CB=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp1CA)->f1;goto _LL23A;}else{goto _LL23B;}_LL23B: if(*((int*)_tmp1CA)== 13){
_LL243: _tmp1CD=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1CA)->f1;goto _LL242;
_LL242: _tmp1CC=((struct Cyc_Absyn_Cast_e_struct*)_tmp1CA)->f2;goto _LL23C;}else{
goto _LL23D;}_LL23D: if(*((int*)_tmp1CA)== 21){_LL245: _tmp1CF=((struct Cyc_Absyn_StructMember_e_struct*)
_tmp1CA)->f1;goto _LL244;_LL244: _tmp1CE=((struct Cyc_Absyn_StructMember_e_struct*)
_tmp1CA)->f2;goto _LL23E;}else{goto _LL23F;}_LL23F: goto _LL240;_LL23A: Cyc_Toc_lvalue_assign_stmt(
_tmp1CB,fs,f,f_env);goto _LL238;_LL23C: Cyc_Toc_lvalue_assign(_tmp1CC,fs,f,f_env);
goto _LL238;_LL23E:(void*)(e1->r=(void*)((void*)_tmp1CF->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));
_tmp1D0->hd=_tmp1CE;_tmp1D0->tl=fs;_tmp1D0;}),f,f_env);goto _LL238;_LL240: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_structmember_exp(e1_copy,(struct _tagged_arr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL238;}_LL238:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp1D1=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Stmt*_tmp1D3;struct Cyc_Absyn_Decl*
_tmp1D4;struct Cyc_Absyn_Stmt*_tmp1D5;_LL247: if((unsigned int)_tmp1D1 > 1?*((int*)
_tmp1D1)== 0: 0){_LL24F: _tmp1D2=((struct Cyc_Absyn_Exp_s_struct*)_tmp1D1)->f1;goto
_LL248;}else{goto _LL249;}_LL249: if((unsigned int)_tmp1D1 > 1?*((int*)_tmp1D1)== 
12: 0){_LL251: _tmp1D4=((struct Cyc_Absyn_Decl_s_struct*)_tmp1D1)->f1;goto _LL250;
_LL250: _tmp1D3=((struct Cyc_Absyn_Decl_s_struct*)_tmp1D1)->f2;goto _LL24A;}else{
goto _LL24B;}_LL24B: if((unsigned int)_tmp1D1 > 1?*((int*)_tmp1D1)== 1: 0){_LL252:
_tmp1D5=((struct Cyc_Absyn_Seq_s_struct*)_tmp1D1)->f2;goto _LL24C;}else{goto _LL24D;}
_LL24D: goto _LL24E;_LL248: Cyc_Toc_lvalue_assign(_tmp1D2,fs,f,f_env);goto _LL246;
_LL24A: Cyc_Toc_lvalue_assign_stmt(_tmp1D3,fs,f,f_env);goto _LL246;_LL24C: Cyc_Toc_lvalue_assign_stmt(
_tmp1D5,fs,f,f_env);goto _LL246;_LL24E:({struct Cyc_Std_String_pa_struct _tmp1D7;
_tmp1D7.tag=0;_tmp1D7.f1=(struct _tagged_arr)Cyc_Absynpp_stmt2string(s);{void*
_tmp1D6[1]={& _tmp1D7};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("lvalue_assign_stmt: %s",sizeof(unsigned char),23),_tag_arr(_tmp1D6,
sizeof(void*),1));}});goto _LL246;_LL246:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0){return 0;}result=({
struct Cyc_List_List*_tmp1D8=_region_malloc(r2,sizeof(*_tmp1D8));_tmp1D8->hd=(
void*)f((void*)x->hd,env);_tmp1D8->tl=0;_tmp1D8;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp1D9=_region_malloc(r2,sizeof(*_tmp1D9));_tmp1D9->hd=(void*)f((void*)x->hd,
env);_tmp1D9->tl=0;_tmp1D9;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple4*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple4*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->f1=0;_tmp1DA->f2=e;_tmp1DA;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp1DB=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp1DC;_LL254: if((unsigned int)_tmp1DB > 3?*((int*)
_tmp1DB)== 4: 0){_LL258: _tmp1DC=((struct Cyc_Absyn_PointerType_struct*)_tmp1DB)->f1;
goto _LL255;}else{goto _LL256;}_LL256: goto _LL257;_LL255: return _tmp1DC;_LL257:({
void*_tmp1DD[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("get_ptr_typ: not a pointer!",sizeof(unsigned char),28),_tag_arr(
_tmp1DD,sizeof(void*),0));});_LL253:;}struct _tuple8{void*f1;void*f2;};struct
_tuple9{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple10{struct
Cyc_Absyn_Structfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp1DE=(void*)e->r;if(e->topt
== 0){({struct Cyc_Std_String_pa_struct _tmp1E0;_tmp1E0.tag=0;_tmp1E0.f1=(struct
_tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp1DF[1]={& _tmp1E0};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: no type for %s",
sizeof(unsigned char),25),_tag_arr(_tmp1DF,sizeof(void*),1));}});}{void*old_typ=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp1E1=_tmp1DE;void*
_tmp1E2;void*_tmp1E3;struct _tuple0*_tmp1E4;struct _tuple0*_tmp1E5;struct Cyc_List_List*
_tmp1E6;void*_tmp1E7;void*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*
_tmp1EA;struct Cyc_Core_Opt*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*
_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_Absyn_Exp*
_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_List_List*_tmp1F2;struct Cyc_Absyn_Exp*
_tmp1F3;struct Cyc_Absyn_VarargCallInfo*_tmp1F4;struct Cyc_List_List*_tmp1F5;
struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_VarargCallInfo*_tmp1F7;struct Cyc_Absyn_VarargCallInfo
_tmp1F8;struct Cyc_Absyn_VarargInfo*_tmp1F9;struct Cyc_List_List*_tmp1FA;int
_tmp1FB;struct Cyc_List_List*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;struct Cyc_Absyn_Exp*
_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_List_List*_tmp200;struct Cyc_Absyn_Exp*
_tmp201;struct Cyc_Absyn_Exp*_tmp202;void*_tmp203;void**_tmp204;struct Cyc_Absyn_Exp*
_tmp205;struct Cyc_Absyn_Exp*_tmp206;struct Cyc_Absyn_Exp*_tmp207;struct Cyc_Absyn_Exp*
_tmp208;void*_tmp209;void*_tmp20A;struct _tagged_arr*_tmp20B;void*_tmp20C;void*
_tmp20D;unsigned int _tmp20E;void*_tmp20F;struct Cyc_Absyn_Exp*_tmp210;struct
_tagged_arr*_tmp211;struct Cyc_Absyn_Exp*_tmp212;struct _tagged_arr*_tmp213;struct
Cyc_Absyn_Exp*_tmp214;struct Cyc_Absyn_Exp*_tmp215;struct Cyc_Absyn_Exp*_tmp216;
struct Cyc_List_List*_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_Absyn_Exp*
_tmp219;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Vardecl*_tmp21B;struct Cyc_Absyn_Structdecl*
_tmp21C;struct Cyc_List_List*_tmp21D;struct Cyc_Core_Opt*_tmp21E;struct _tuple0*
_tmp21F;struct Cyc_List_List*_tmp220;void*_tmp221;struct Cyc_Absyn_Tunionfield*
_tmp222;struct Cyc_Absyn_Tuniondecl*_tmp223;struct Cyc_List_List*_tmp224;struct Cyc_Absyn_Tunionfield*
_tmp225;struct Cyc_Absyn_Tuniondecl*_tmp226;struct Cyc_List_List*_tmp227;struct Cyc_Absyn_MallocInfo
_tmp228;int _tmp229;struct Cyc_Absyn_Exp*_tmp22A;void**_tmp22B;struct Cyc_Absyn_Exp*
_tmp22C;int _tmp22D;struct Cyc_Absyn_Stmt*_tmp22E;struct Cyc_Absyn_Fndecl*_tmp22F;
_LL25A: if(*((int*)_tmp1E1)== 0){_LL2AE: _tmp1E2=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1E1)->f1;if((int)_tmp1E2 == 0){goto _LL25B;}else{goto _LL25C;}}else{goto _LL25C;}
_LL25C: if(*((int*)_tmp1E1)== 0){goto _LL25D;}else{goto _LL25E;}_LL25E: if(*((int*)
_tmp1E1)== 1){_LL2B0: _tmp1E4=((struct Cyc_Absyn_Var_e_struct*)_tmp1E1)->f1;goto
_LL2AF;_LL2AF: _tmp1E3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1E1)->f2;goto
_LL25F;}else{goto _LL260;}_LL260: if(*((int*)_tmp1E1)== 2){_LL2B1: _tmp1E5=((struct
Cyc_Absyn_UnknownId_e_struct*)_tmp1E1)->f1;goto _LL261;}else{goto _LL262;}_LL262:
if(*((int*)_tmp1E1)== 3){_LL2B3: _tmp1E7=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1E1)->f1;goto _LL2B2;_LL2B2: _tmp1E6=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1E1)->f2;goto _LL263;}else{goto _LL264;}_LL264: if(*((int*)_tmp1E1)== 5){_LL2B5:
_tmp1E9=((struct Cyc_Absyn_Increment_e_struct*)_tmp1E1)->f1;goto _LL2B4;_LL2B4:
_tmp1E8=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1E1)->f2;goto _LL265;}
else{goto _LL266;}_LL266: if(*((int*)_tmp1E1)== 4){_LL2B8: _tmp1EC=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1E1)->f1;goto _LL2B7;_LL2B7: _tmp1EB=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1E1)->f2;goto _LL2B6;_LL2B6: _tmp1EA=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1E1)->f3;goto _LL267;}else{goto _LL268;}_LL268: if(*((int*)_tmp1E1)== 6){_LL2BB:
_tmp1EF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1E1)->f1;goto _LL2BA;_LL2BA:
_tmp1EE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1E1)->f2;goto _LL2B9;_LL2B9:
_tmp1ED=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1E1)->f3;goto _LL269;}else{
goto _LL26A;}_LL26A: if(*((int*)_tmp1E1)== 7){_LL2BD: _tmp1F1=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1E1)->f1;goto _LL2BC;_LL2BC: _tmp1F0=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1E1)->f2;goto _LL26B;}else{goto _LL26C;}_LL26C: if(*((int*)_tmp1E1)== 8){_LL2BF:
_tmp1F3=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1E1)->f1;goto _LL2BE;_LL2BE:
_tmp1F2=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1E1)->f2;goto _LL26D;}else{
goto _LL26E;}_LL26E: if(*((int*)_tmp1E1)== 9){_LL2C2: _tmp1F6=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E1)->f1;goto _LL2C1;_LL2C1: _tmp1F5=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E1)->f2;goto _LL2C0;_LL2C0: _tmp1F4=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E1)->f3;if(_tmp1F4 == 0){goto _LL26F;}else{goto _LL270;}}else{goto _LL270;}
_LL270: if(*((int*)_tmp1E1)== 9){_LL2C8: _tmp1FD=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E1)->f1;goto _LL2C7;_LL2C7: _tmp1FC=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E1)->f2;goto _LL2C3;_LL2C3: _tmp1F7=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1E1)->f3;if(_tmp1F7 == 0){goto _LL272;}else{_tmp1F8=*_tmp1F7;_LL2C6: _tmp1FB=
_tmp1F8.num_varargs;goto _LL2C5;_LL2C5: _tmp1FA=_tmp1F8.injectors;goto _LL2C4;
_LL2C4: _tmp1F9=_tmp1F8.vai;goto _LL271;}}else{goto _LL272;}_LL272: if(*((int*)
_tmp1E1)== 10){_LL2C9: _tmp1FE=((struct Cyc_Absyn_Throw_e_struct*)_tmp1E1)->f1;
goto _LL273;}else{goto _LL274;}_LL274: if(*((int*)_tmp1E1)== 11){_LL2CA: _tmp1FF=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1E1)->f1;goto _LL275;}else{goto _LL276;}
_LL276: if(*((int*)_tmp1E1)== 12){_LL2CC: _tmp201=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1E1)->f1;goto _LL2CB;_LL2CB: _tmp200=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1E1)->f2;goto _LL277;}else{goto _LL278;}_LL278: if(*((int*)_tmp1E1)== 13){
_LL2CE: _tmp203=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1E1)->f1;_tmp204=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1E1)->f1);goto _LL2CD;_LL2CD:
_tmp202=((struct Cyc_Absyn_Cast_e_struct*)_tmp1E1)->f2;goto _LL279;}else{goto
_LL27A;}_LL27A: if(*((int*)_tmp1E1)== 14){_LL2CF: _tmp205=((struct Cyc_Absyn_Address_e_struct*)
_tmp1E1)->f1;goto _LL27B;}else{goto _LL27C;}_LL27C: if(*((int*)_tmp1E1)== 15){
_LL2D1: _tmp207=((struct Cyc_Absyn_New_e_struct*)_tmp1E1)->f1;goto _LL2D0;_LL2D0:
_tmp206=((struct Cyc_Absyn_New_e_struct*)_tmp1E1)->f2;goto _LL27D;}else{goto _LL27E;}
_LL27E: if(*((int*)_tmp1E1)== 17){_LL2D2: _tmp208=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp1E1)->f1;goto _LL27F;}else{goto _LL280;}_LL280: if(*((int*)_tmp1E1)== 16){
_LL2D3: _tmp209=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1E1)->f1;goto
_LL281;}else{goto _LL282;}_LL282: if(*((int*)_tmp1E1)== 19){goto _LL283;}else{goto
_LL284;}_LL284: if(*((int*)_tmp1E1)== 18){_LL2D6: _tmp20C=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1E1)->f1;goto _LL2D4;_LL2D4: _tmp20A=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1E1)->f2;if(*((int*)_tmp20A)== 0){_LL2D5: _tmp20B=((struct Cyc_Absyn_StructField_struct*)
_tmp20A)->f1;goto _LL285;}else{goto _LL286;}}else{goto _LL286;}_LL286: if(*((int*)
_tmp1E1)== 18){_LL2D9: _tmp20F=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1E1)->f1;goto _LL2D7;_LL2D7: _tmp20D=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1E1)->f2;if(*((int*)_tmp20D)== 1){_LL2D8: _tmp20E=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp20D)->f1;goto _LL287;}else{goto _LL288;}}else{goto _LL288;}_LL288: if(*((int*)
_tmp1E1)== 20){_LL2DA: _tmp210=((struct Cyc_Absyn_Deref_e_struct*)_tmp1E1)->f1;
goto _LL289;}else{goto _LL28A;}_LL28A: if(*((int*)_tmp1E1)== 21){_LL2DC: _tmp212=((
struct Cyc_Absyn_StructMember_e_struct*)_tmp1E1)->f1;goto _LL2DB;_LL2DB: _tmp211=((
struct Cyc_Absyn_StructMember_e_struct*)_tmp1E1)->f2;goto _LL28B;}else{goto _LL28C;}
_LL28C: if(*((int*)_tmp1E1)== 22){_LL2DE: _tmp214=((struct Cyc_Absyn_StructArrow_e_struct*)
_tmp1E1)->f1;goto _LL2DD;_LL2DD: _tmp213=((struct Cyc_Absyn_StructArrow_e_struct*)
_tmp1E1)->f2;goto _LL28D;}else{goto _LL28E;}_LL28E: if(*((int*)_tmp1E1)== 23){
_LL2E0: _tmp216=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1E1)->f1;goto _LL2DF;
_LL2DF: _tmp215=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1E1)->f2;goto _LL28F;}
else{goto _LL290;}_LL290: if(*((int*)_tmp1E1)== 24){_LL2E1: _tmp217=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1E1)->f1;goto _LL291;}else{goto _LL292;}_LL292: if(*((int*)_tmp1E1)== 26){
_LL2E2: _tmp218=((struct Cyc_Absyn_Array_e_struct*)_tmp1E1)->f1;goto _LL293;}else{
goto _LL294;}_LL294: if(*((int*)_tmp1E1)== 27){_LL2E5: _tmp21B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1E1)->f1;goto _LL2E4;_LL2E4: _tmp21A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1E1)->f2;goto _LL2E3;_LL2E3: _tmp219=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1E1)->f3;goto _LL295;}else{goto _LL296;}_LL296: if(*((int*)_tmp1E1)== 28){
_LL2E9: _tmp21F=((struct Cyc_Absyn_Struct_e_struct*)_tmp1E1)->f1;goto _LL2E8;_LL2E8:
_tmp21E=((struct Cyc_Absyn_Struct_e_struct*)_tmp1E1)->f2;goto _LL2E7;_LL2E7:
_tmp21D=((struct Cyc_Absyn_Struct_e_struct*)_tmp1E1)->f3;goto _LL2E6;_LL2E6:
_tmp21C=((struct Cyc_Absyn_Struct_e_struct*)_tmp1E1)->f4;goto _LL297;}else{goto
_LL298;}_LL298: if(*((int*)_tmp1E1)== 29){_LL2EB: _tmp221=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1E1)->f1;goto _LL2EA;_LL2EA: _tmp220=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1E1)->f2;goto _LL299;}else{goto _LL29A;}_LL29A: if(*((int*)_tmp1E1)== 30){
_LL2EE: _tmp224=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1E1)->f3;if(_tmp224 == 0){
goto _LL2ED;}else{goto _LL29C;}_LL2ED: _tmp223=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1E1)->f4;goto _LL2EC;_LL2EC: _tmp222=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1E1)->f5;goto _LL29B;}else{goto _LL29C;}_LL29C: if(*((int*)_tmp1E1)== 30){
_LL2F1: _tmp227=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1E1)->f3;goto _LL2F0;_LL2F0:
_tmp226=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1E1)->f4;goto _LL2EF;_LL2EF:
_tmp225=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1E1)->f5;goto _LL29D;}else{goto
_LL29E;}_LL29E: if(*((int*)_tmp1E1)== 31){goto _LL29F;}else{goto _LL2A0;}_LL2A0: if(*((
int*)_tmp1E1)== 32){goto _LL2A1;}else{goto _LL2A2;}_LL2A2: if(*((int*)_tmp1E1)== 33){
_LL2F2: _tmp228=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1E1)->f1;_LL2F7: _tmp22D=
_tmp228.is_calloc;goto _LL2F6;_LL2F6: _tmp22C=_tmp228.rgn;goto _LL2F5;_LL2F5:
_tmp22B=_tmp228.elt_type;goto _LL2F4;_LL2F4: _tmp22A=_tmp228.num_elts;goto _LL2F3;
_LL2F3: _tmp229=_tmp228.fat_result;goto _LL2A3;}else{goto _LL2A4;}_LL2A4: if(*((int*)
_tmp1E1)== 35){_LL2F8: _tmp22E=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp1E1)->f1;
goto _LL2A5;}else{goto _LL2A6;}_LL2A6: if(*((int*)_tmp1E1)== 34){goto _LL2A7;}else{
goto _LL2A8;}_LL2A8: if(*((int*)_tmp1E1)== 25){goto _LL2A9;}else{goto _LL2AA;}_LL2AA:
if(*((int*)_tmp1E1)== 36){_LL2F9: _tmp22F=((struct Cyc_Absyn_Codegen_e_struct*)
_tmp1E1)->f1;goto _LL2AB;}else{goto _LL2AC;}_LL2AC: if(*((int*)_tmp1E1)== 37){goto
_LL2AD;}else{goto _LL259;}_LL25B: {struct Cyc_Absyn_Exp*_tmp230=Cyc_Absyn_uint_exp(
0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(nv->toplevel){(void*)(e->r=(
void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_typ,_tmp230,_tmp230))->r));}
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp231[3];_tmp231[2]=_tmp230;_tmp231[1]=_tmp230;_tmp231[0]=
_tmp230;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp231,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(void*)((
void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL259;}_LL25D: goto _LL259;_LL25F:{
struct _handler_cons _tmp232;_push_handler(& _tmp232);{int _tmp234=0;if(setjmp(
_tmp232.handler)){_tmp234=1;}if(! _tmp234){(void*)(e->r=(void*)((void*)(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)(nv->varmap,
_tmp1E4))->r));;_pop_handler();}else{void*_tmp233=(void*)_exn_thrown;void*
_tmp236=_tmp233;_LL2FB: if(_tmp236 == Cyc_Dict_Absent){goto _LL2FC;}else{goto _LL2FD;}
_LL2FD: goto _LL2FE;_LL2FC:({struct Cyc_Std_String_pa_struct _tmp238;_tmp238.tag=0;
_tmp238.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1E4);{void*_tmp237[1]={&
_tmp238};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Can't find %s in exp_to_c, Var\n",sizeof(unsigned char),32),_tag_arr(
_tmp237,sizeof(void*),1));}});_LL2FE:(void)_throw(_tmp236);_LL2FA:;}}}goto _LL259;
_LL261:({void*_tmp239[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("unknownid",sizeof(unsigned char),10),_tag_arr(
_tmp239,sizeof(void*),0));});_LL263: {struct Cyc_List_List*_tmp23A=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,
_tmp1E6);((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp1E6);{void*
_tmp23B=_tmp1E7;_LL300: if((int)_tmp23B == 19){goto _LL301;}else{goto _LL302;}_LL302:
if((int)_tmp23B == 0){goto _LL303;}else{goto _LL304;}_LL304: if((int)_tmp23B == 2){
goto _LL305;}else{goto _LL306;}_LL306: if((int)_tmp23B == 5){goto _LL307;}else{goto
_LL308;}_LL308: if((int)_tmp23B == 6){goto _LL309;}else{goto _LL30A;}_LL30A: if((int)
_tmp23B == 7){goto _LL30B;}else{goto _LL30C;}_LL30C: if((int)_tmp23B == 9){goto _LL30D;}
else{goto _LL30E;}_LL30E: if((int)_tmp23B == 8){goto _LL30F;}else{goto _LL310;}_LL310:
if((int)_tmp23B == 10){goto _LL311;}else{goto _LL312;}_LL312: goto _LL313;_LL301: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1E6))->hd;{void*_tmp23C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_Exp*_tmp23D;struct Cyc_Absyn_PtrInfo
_tmp23E;struct Cyc_Absyn_Conref*_tmp23F;struct Cyc_Absyn_Conref*_tmp240;void*
_tmp241;_LL315: if((unsigned int)_tmp23C > 3?*((int*)_tmp23C)== 7: 0){_LL31B:
_tmp23D=((struct Cyc_Absyn_ArrayType_struct*)_tmp23C)->f3;goto _LL316;}else{goto
_LL317;}_LL317: if((unsigned int)_tmp23C > 3?*((int*)_tmp23C)== 4: 0){_LL31C:
_tmp23E=((struct Cyc_Absyn_PointerType_struct*)_tmp23C)->f1;_LL31F: _tmp241=(void*)
_tmp23E.elt_typ;goto _LL31E;_LL31E: _tmp240=_tmp23E.nullable;goto _LL31D;_LL31D:
_tmp23F=_tmp23E.bounds;goto _LL318;}else{goto _LL319;}_LL319: goto _LL31A;_LL316:(
void*)(e->r=(void*)((void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp23D))->r));
goto _LL314;_LL318:{void*_tmp242=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp23F);struct Cyc_Absyn_Exp*_tmp243;_LL321: if((int)_tmp242 == 0){goto _LL322;}
else{goto _LL323;}_LL323: if((unsigned int)_tmp242 > 1?*((int*)_tmp242)== 0: 0){
_LL325: _tmp243=((struct Cyc_Absyn_Upper_b_struct*)_tmp242)->f1;goto _LL324;}else{
goto _LL320;}_LL322:(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp244[2];_tmp244[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp241),0);_tmp244[0]=(struct Cyc_Absyn_Exp*)_tmp1E6->hd;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp244,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL320;_LL324: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp240)){(void*)(e->r=(void*)((void*)(Cyc_Absyn_conditional_exp(arg,_tmp243,
Cyc_Absyn_uint_exp(0,0),0))->r));}else{(void*)(e->r=(void*)((void*)_tmp243->r));
goto _LL320;}_LL320:;}goto _LL314;_LL31A:({struct Cyc_Std_String_pa_struct _tmp247;
_tmp247.tag=0;_tmp247.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp246;_tmp246.tag=0;_tmp246.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v);{void*_tmp245[2]={& _tmp246,&
_tmp247};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("size primop applied to non-array %s (%s)",sizeof(unsigned char),41),
_tag_arr(_tmp245,sizeof(void*),2));}}});_LL314:;}goto _LL2FF;}_LL303: {void*
elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp23A))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1E6))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1E6->tl))->hd;(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp248[3];
_tmp248[2]=e2;_tmp248[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp248[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp248,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}goto _LL2FF;}
_LL305: {void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((
struct Cyc_List_List*)_check_null(_tmp23A))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E6))->hd;struct Cyc_Absyn_Exp*
e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E6->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(
_tmp23A->tl))->hd)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_structmember_exp(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((void*)(Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e->r=(void*)((
void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),0))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp249[3];_tmp249[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);
_tmp249[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp249[0]=e1;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp249,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}goto _LL2FF;}_LL307: goto _LL309;
_LL309: goto _LL30B;_LL30B: goto _LL30D;_LL30D: goto _LL30F;_LL30F: goto _LL311;_LL311: {
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1E6))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1E6->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)_check_null(
_tmp23A))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp23A->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ(t1)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));}if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){(void*)(e2->r=(void*)((void*)(Cyc_Absyn_structmember_exp(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));}goto _LL2FF;}_LL313: goto _LL2FF;_LL2FF:;}
goto _LL259;}_LL265: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1E9->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp1E9);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*_tmp24A=(_tmp1E8 == (void*)1? 1: _tmp1E8 == (
void*)3)? Cyc_Toc__tagged_arr_inplace_plus_post_e: Cyc_Toc__tagged_arr_inplace_plus_e;
int _tmp24B=(_tmp1E8 == (void*)0? 1: _tmp1E8 == (void*)1)? 1: - 1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(_tmp24A,({struct Cyc_Absyn_Exp*_tmp24C[3];_tmp24C[2]=
Cyc_Absyn_signed_int_exp(_tmp24B,0);_tmp24C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0);_tmp24C[0]=Cyc_Absyn_address_exp(_tmp1E9,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp24C,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}if(elt_typ == (void*)0? ! Cyc_Absyn_is_lvalue(_tmp1E9): 0){Cyc_Toc_lvalue_assign(
_tmp1E9,0,Cyc_Toc_incr_lvalue,_tmp1E8);(void*)(e->r=(void*)((void*)_tmp1E9->r));}
goto _LL259;}}_LL267: {int e1_poly=Cyc_Toc_is_poly_project(_tmp1EC);void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1EC->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1EA->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp1EC);Cyc_Toc_exp_to_c(nv,_tmp1EA);{int done=0;if(_tmp1EB != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp24D=(void*)_tmp1EB->v;_LL327: if((int)_tmp24D == 0){goto _LL328;}
else{goto _LL329;}_LL329: if((int)_tmp24D == 2){goto _LL32A;}else{goto _LL32B;}_LL32B:
goto _LL32C;_LL328: change=_tmp1EA;goto _LL326;_LL32A: change=Cyc_Absyn_prim1_exp((
void*)2,_tmp1EA,0);goto _LL326;_LL32C:({void*_tmp24E[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("bad t ? pointer arithmetic",
sizeof(unsigned char),27),_tag_arr(_tmp24E,sizeof(void*),0));});_LL326:;}done=1;(
void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp24F[3];_tmp24F[2]=change;_tmp24F[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp24F[0]=Cyc_Absyn_address_exp(_tmp1EC,0);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp24F,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}if(! done){if(e1_poly){(void*)(
_tmp1EA->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp1EA->r,0),0))->r));}if(! Cyc_Absyn_is_lvalue(_tmp1EC)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
struct _tuple7*),struct _tuple7*f_env))Cyc_Toc_lvalue_assign)(_tmp1EC,0,Cyc_Toc_assignop_lvalue,({
struct _tuple7*_tmp250=_cycalloc(sizeof(struct _tuple7)* 1);_tmp250[0]=({struct
_tuple7 _tmp251;_tmp251.f1=_tmp1EB;_tmp251.f2=_tmp1EA;_tmp251;});_tmp250;}));(
void*)(e->r=(void*)((void*)_tmp1EC->r));}}goto _LL259;}}_LL269: Cyc_Toc_exp_to_c(
nv,_tmp1EF);Cyc_Toc_exp_to_c(nv,_tmp1EE);Cyc_Toc_exp_to_c(nv,_tmp1ED);goto _LL259;
_LL26B: Cyc_Toc_exp_to_c(nv,_tmp1F1);Cyc_Toc_exp_to_c(nv,_tmp1F0);goto _LL259;
_LL26D: _tmp1F6=_tmp1F3;_tmp1F5=_tmp1F2;goto _LL26F;_LL26F: Cyc_Toc_exp_to_c(nv,
_tmp1F6);((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp1F5);goto
_LL259;_LL271:{struct _RegionHandle _tmp252=_new_region("r");struct _RegionHandle*r=&
_tmp252;_push_region(r);{struct _tuple0*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp1FB,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp1F9->type);
void*arr_type=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp266=_cycalloc(
sizeof(*_tmp266));_tmp266[0]=({struct Cyc_Absyn_ArrayType_struct _tmp267;_tmp267.tag=
7;_tmp267.f1=(void*)cva_type;_tmp267.f2=Cyc_Toc_mt_tq;_tmp267.f3=(struct Cyc_Absyn_Exp*)
num_varargs_exp;_tmp267;});_tmp266;});int num_args=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp1FC);int num_normargs=num_args - _tmp1FB;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp1FC=_tmp1FC->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1FC))->hd);
new_args=({struct Cyc_List_List*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->hd=(
struct Cyc_Absyn_Exp*)_tmp1FC->hd;_tmp253->tl=new_args;_tmp253;});}}new_args=({
struct Cyc_List_List*_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp255[3];_tmp255[2]=num_varargs_exp;
_tmp255[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp255[0]=argvexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp255,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);_tmp254->tl=new_args;_tmp254;});new_args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp1FD);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp1FD,new_args,
0),0);if(_tmp1F9->inject){struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp256=Cyc_Tcutil_compress((
void*)_tmp1F9->type);struct Cyc_Absyn_TunionInfo _tmp257;void*_tmp258;struct Cyc_Absyn_Tuniondecl**
_tmp259;struct Cyc_Absyn_Tuniondecl*_tmp25A;_LL32E: if((unsigned int)_tmp256 > 3?*((
int*)_tmp256)== 2: 0){_LL332: _tmp257=((struct Cyc_Absyn_TunionType_struct*)_tmp256)->f1;
_LL333: _tmp258=(void*)_tmp257.tunion_info;if(*((int*)_tmp258)== 1){_LL334:
_tmp259=((struct Cyc_Absyn_KnownTunion_struct*)_tmp258)->f1;_tmp25A=*_tmp259;goto
_LL32F;}else{goto _LL330;}}else{goto _LL330;}_LL330: goto _LL331;_LL32F: tud=_tmp25A;
goto _LL32D;_LL331:({void*_tmp25B[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("toc: unknown tunion in vararg with inject",
sizeof(unsigned char),42),_tag_arr(_tmp25B,sizeof(void*),0));});_LL32D:;}{struct
_tagged_arr vs=({unsigned int _tmp262=(unsigned int)_tmp1FB;struct _tuple0**_tmp263=(
struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmp262));
struct _tagged_arr _tmp265=_tag_arr(_tmp263,sizeof(struct _tuple0*),(unsigned int)
_tmp1FB);{unsigned int _tmp264=_tmp262;unsigned int i;for(i=0;i < _tmp264;i ++){
_tmp263[i]=Cyc_Toc_temp_var();}};_tmp265;});struct Cyc_List_List*_tmp25C=0;{int i=
_tmp1FB - 1;for(0;i >= 0;-- i){_tmp25C=({struct Cyc_List_List*_tmp25D=_cycalloc(
sizeof(*_tmp25D));_tmp25D->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(((
struct _tuple0**)vs.curr)[i],0),0));_tmp25D->tl=_tmp25C;_tmp25D;});}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp25C,0),s,0);{
int i=0;for(0;_tmp1FC != 0;(((_tmp1FC=_tmp1FC->tl,_tmp1FA=_tmp1FA->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp1FC->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple0*var=((struct _tuple0**)
vs.curr)[i];struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield
_tmp25F;struct Cyc_List_List*_tmp260;struct _tuple0*_tmp261;struct Cyc_Absyn_Tunionfield*
_tmp25E=(struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp1FA))->hd;
_tmp25F=*_tmp25E;_LL337: _tmp261=_tmp25F.name;goto _LL336;_LL336: _tmp260=_tmp25F.typs;
goto _LL335;_LL335: {void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple3*)((struct
Cyc_List_List*)_check_null(_tmp260))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star(
field_typ)){arg=Cyc_Absyn_cast_exp(field_typ,arg,0);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_structmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(
tud,_tmp261,1),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp261,_tag_arr("_struct",sizeof(unsigned char),8))),0,s,0);}}}}}else{{int i=0;
for(0;_tmp1FC != 0;(_tmp1FC=_tmp1FC->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp1FC->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp1FC->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL259;_LL273: Cyc_Toc_exp_to_c(
nv,_tmp1FE);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(
old_typ),Cyc_Toc_newthrow_exp(_tmp1FE),0))->r));goto _LL259;_LL275: Cyc_Toc_exp_to_c(
nv,_tmp1FF);goto _LL259;_LL277: Cyc_Toc_exp_to_c(nv,_tmp201);for(0;_tmp200 != 0;
_tmp200=_tmp200->tl){void*k=Cyc_Tcutil_typ_kind((void*)_tmp200->hd);if(k != (void*)
4? k != (void*)3: 0){{void*_tmp268=Cyc_Tcutil_compress((void*)_tmp200->hd);_LL339:
if((unsigned int)_tmp268 > 3?*((int*)_tmp268)== 1: 0){goto _LL33A;}else{goto _LL33B;}
_LL33B: if((unsigned int)_tmp268 > 3?*((int*)_tmp268)== 2: 0){goto _LL33C;}else{goto
_LL33D;}_LL33D: goto _LL33E;_LL33A: goto _LL33C;_LL33C: continue;_LL33E:(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),_tmp201,0))->r));goto _LL338;_LL338:;}break;}}goto
_LL259;_LL279: {void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp202->topt))->v;
void*new_typ=*_tmp204;*_tmp204=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,
_tmp202);{struct _tuple8 _tmp26A=({struct _tuple8 _tmp269;_tmp269.f1=Cyc_Tcutil_compress(
old_t2);_tmp269.f2=Cyc_Tcutil_compress(new_typ);_tmp269;});void*_tmp26B;struct
Cyc_Absyn_PtrInfo _tmp26C;void*_tmp26D;struct Cyc_Absyn_PtrInfo _tmp26E;void*
_tmp26F;void*_tmp270;struct Cyc_Absyn_PtrInfo _tmp271;_LL340: _LL348: _tmp26D=
_tmp26A.f1;if((unsigned int)_tmp26D > 3?*((int*)_tmp26D)== 4: 0){_LL349: _tmp26E=((
struct Cyc_Absyn_PointerType_struct*)_tmp26D)->f1;goto _LL346;}else{goto _LL342;}
_LL346: _tmp26B=_tmp26A.f2;if((unsigned int)_tmp26B > 3?*((int*)_tmp26B)== 4: 0){
_LL347: _tmp26C=((struct Cyc_Absyn_PointerType_struct*)_tmp26B)->f1;goto _LL341;}
else{goto _LL342;}_LL342: _LL34B: _tmp270=_tmp26A.f1;if((unsigned int)_tmp270 > 3?*((
int*)_tmp270)== 4: 0){_LL34C: _tmp271=((struct Cyc_Absyn_PointerType_struct*)
_tmp270)->f1;goto _LL34A;}else{goto _LL344;}_LL34A: _tmp26F=_tmp26A.f2;if((
unsigned int)_tmp26F > 3?*((int*)_tmp26F)== 5: 0){goto _LL343;}else{goto _LL344;}
_LL344: goto _LL345;_LL341: {int _tmp272=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp26E.nullable);int _tmp273=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp26C.nullable);void*_tmp274=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp26E.bounds);void*_tmp275=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp26C.bounds);{
struct _tuple8 _tmp277=({struct _tuple8 _tmp276;_tmp276.f1=_tmp274;_tmp276.f2=
_tmp275;_tmp276;});void*_tmp278;void*_tmp279;void*_tmp27A;void*_tmp27B;struct Cyc_Absyn_Exp*
_tmp27C;void*_tmp27D;struct Cyc_Absyn_Exp*_tmp27E;void*_tmp27F;_LL34E: _LL357:
_tmp279=_tmp277.f1;if((unsigned int)_tmp279 > 1?*((int*)_tmp279)== 0: 0){goto
_LL356;}else{goto _LL350;}_LL356: _tmp278=_tmp277.f2;if((unsigned int)_tmp278 > 1?*((
int*)_tmp278)== 0: 0){goto _LL34F;}else{goto _LL350;}_LL350: _LL359: _tmp27B=_tmp277.f1;
if((unsigned int)_tmp27B > 1?*((int*)_tmp27B)== 0: 0){_LL35A: _tmp27C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp27B)->f1;goto _LL358;}else{goto _LL352;}_LL358: _tmp27A=_tmp277.f2;if((int)
_tmp27A == 0){goto _LL351;}else{goto _LL352;}_LL352: _LL35D: _tmp27F=_tmp277.f1;if((
int)_tmp27F == 0){goto _LL35B;}else{goto _LL354;}_LL35B: _tmp27D=_tmp277.f2;if((
unsigned int)_tmp27D > 1?*((int*)_tmp27D)== 0: 0){_LL35C: _tmp27E=((struct Cyc_Absyn_Upper_b_struct*)
_tmp27D)->f1;goto _LL353;}else{goto _LL354;}_LL354: goto _LL355;_LL34F: if(_tmp272? !
_tmp273: 0){if(nv->toplevel){({void*_tmp280[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("can't do NULL-check conversion at top-level",
sizeof(unsigned char),44),_tag_arr(_tmp280,sizeof(void*),0));});}(void*)(e->r=(
void*)((void*)(Cyc_Absyn_cast_exp(*_tmp204,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281->hd=_tmp202;
_tmp281->tl=0;_tmp281;}),0),0))->r));}goto _LL34D;_LL351: if(nv->toplevel){(void*)(
e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_t2,_tmp27C,_tmp202))->r));}
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp282[3];_tmp282[2]=_tmp27C;_tmp282[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp26C.elt_typ),0);_tmp282[0]=_tmp202;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp282,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}goto _LL34D;_LL353: if(nv->toplevel){({void*_tmp283[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("can't coerce t? to t* or t@ at the top-level",
sizeof(unsigned char),45),_tag_arr(_tmp283,sizeof(void*),0));});}{struct Cyc_Absyn_Exp*
_tmp284=Cyc_Absyn_fncall_exp(Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*_tmp286[
3];_tmp286[2]=_tmp27E;_tmp286[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)
_tmp26E.elt_typ),0);_tmp286[0]=_tmp202;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp286,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);if(_tmp273){(void*)(_tmp284->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->hd=Cyc_Absyn_copy_exp(
_tmp284);_tmp285->tl=0;_tmp285;}),0))->r));}(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*
_tmp204,_tmp284,0))->r));goto _LL34D;}_LL355: goto _LL34D;_LL34D:;}goto _LL33F;}
_LL343:{void*_tmp287=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp271.bounds);
_LL35F: if((int)_tmp287 == 0){goto _LL360;}else{goto _LL361;}_LL361: goto _LL362;
_LL360:(void*)(_tmp202->r=(void*)((void*)(Cyc_Absyn_structmember_exp(Cyc_Absyn_new_exp((
void*)_tmp202->r,_tmp202->loc),Cyc_Toc_curr_sp,0))->r));goto _LL35E;_LL362: goto
_LL35E;_LL35E:;}goto _LL33F;_LL345: goto _LL33F;_LL33F:;}goto _LL259;}_LL27B:{void*
_tmp288=(void*)_tmp205->r;struct Cyc_List_List*_tmp289;struct _tuple0*_tmp28A;
struct Cyc_List_List*_tmp28B;_LL364: if(*((int*)_tmp288)== 28){_LL36B: _tmp28A=((
struct Cyc_Absyn_Struct_e_struct*)_tmp288)->f1;goto _LL36A;_LL36A: _tmp289=((struct
Cyc_Absyn_Struct_e_struct*)_tmp288)->f3;goto _LL365;}else{goto _LL366;}_LL366: if(*((
int*)_tmp288)== 24){_LL36C: _tmp28B=((struct Cyc_Absyn_Tuple_e_struct*)_tmp288)->f1;
goto _LL367;}else{goto _LL368;}_LL368: goto _LL369;_LL365: if(nv->toplevel){({struct
Cyc_Std_String_pa_struct _tmp28D;_tmp28D.tag=0;_tmp28D.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp205->loc);{void*_tmp28C[1]={& _tmp28D};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("%s: & on non-identifiers at the top-level",
sizeof(unsigned char),42),_tag_arr(_tmp28C,sizeof(void*),1));}});}(void*)(e->r=(
void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp205->topt))->v,1,0,_tmp289,_tmp28A))->r));goto _LL363;_LL367: if(nv->toplevel){({
struct Cyc_Std_String_pa_struct _tmp28F;_tmp28F.tag=0;_tmp28F.f1=(struct
_tagged_arr)Cyc_Position_string_of_segment(_tmp205->loc);{void*_tmp28E[1]={&
_tmp28F};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level",sizeof(unsigned char),42),
_tag_arr(_tmp28E,sizeof(void*),1));}});}(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp28B))->r));goto _LL363;_LL369: Cyc_Toc_exp_to_c(nv,_tmp205);if(! Cyc_Absyn_is_lvalue(
_tmp205)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp205,0,Cyc_Toc_address_lvalue,
1);(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp205,0))->r));}goto _LL363;
_LL363:;}goto _LL259;_LL27D: if(nv->toplevel){({struct Cyc_Std_String_pa_struct
_tmp291;_tmp291.tag=0;_tmp291.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp206->loc);{void*_tmp290[1]={& _tmp291};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("%s: new at top-level",sizeof(
unsigned char),21),_tag_arr(_tmp290,sizeof(void*),1));}});}{void*_tmp292=(void*)
_tmp206->r;struct Cyc_List_List*_tmp293;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Absyn_Exp*
_tmp295;struct Cyc_Absyn_Vardecl*_tmp296;struct Cyc_List_List*_tmp297;struct
_tuple0*_tmp298;struct Cyc_List_List*_tmp299;_LL36E: if(*((int*)_tmp292)== 26){
_LL378: _tmp293=((struct Cyc_Absyn_Array_e_struct*)_tmp292)->f1;goto _LL36F;}else{
goto _LL370;}_LL370: if(*((int*)_tmp292)== 27){_LL37B: _tmp296=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp292)->f1;goto _LL37A;_LL37A: _tmp295=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp292)->f2;goto _LL379;_LL379: _tmp294=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp292)->f3;goto _LL371;}else{goto _LL372;}_LL372: if(*((int*)_tmp292)== 28){
_LL37D: _tmp298=((struct Cyc_Absyn_Struct_e_struct*)_tmp292)->f1;goto _LL37C;_LL37C:
_tmp297=((struct Cyc_Absyn_Struct_e_struct*)_tmp292)->f3;goto _LL373;}else{goto
_LL374;}_LL374: if(*((int*)_tmp292)== 24){_LL37E: _tmp299=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp292)->f1;goto _LL375;}else{goto _LL376;}_LL376: goto _LL377;_LL36F: {struct
_tuple0*_tmp29A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp29B=Cyc_Absyn_var_exp(
_tmp29A,0);struct Cyc_Absyn_Stmt*_tmp29C=Cyc_Toc_init_array(nv,_tmp29B,_tmp293,
Cyc_Absyn_exp_stmt(_tmp29B,0));void*old_elt_typ;{void*_tmp29D=Cyc_Tcutil_compress(
old_typ);struct Cyc_Absyn_PtrInfo _tmp29E;struct Cyc_Absyn_Tqual _tmp29F;void*
_tmp2A0;_LL380: if((unsigned int)_tmp29D > 3?*((int*)_tmp29D)== 4: 0){_LL384:
_tmp29E=((struct Cyc_Absyn_PointerType_struct*)_tmp29D)->f1;_LL386: _tmp2A0=(void*)
_tmp29E.elt_typ;goto _LL385;_LL385: _tmp29F=_tmp29E.tq;goto _LL381;}else{goto _LL382;}
_LL382: goto _LL383;_LL381: old_elt_typ=_tmp2A0;goto _LL37F;_LL383: old_elt_typ=({
void*_tmp2A1[0]={};Cyc_Toc_toc_impos(_tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof(unsigned char),52),_tag_arr(_tmp2A1,sizeof(void*),0));});_LL37F:;}{void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp2A2=Cyc_Absyn_cstar_typ(elt_typ,
Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp2A3=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp293),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp207 == 0){e1=Cyc_Toc_malloc_exp(
old_elt_typ,_tmp2A3);}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(_tmp207);Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp2A3);}(
void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp29A,
_tmp2A2,(struct Cyc_Absyn_Exp*)e1,_tmp29C,0),0))->r));goto _LL36D;}}_LL371: {int
is_tagged_ptr=0;{void*_tmp2A4=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp2A5;struct Cyc_Absyn_Conref*_tmp2A6;struct Cyc_Absyn_Tqual _tmp2A7;void*_tmp2A8;
_LL388: if((unsigned int)_tmp2A4 > 3?*((int*)_tmp2A4)== 4: 0){_LL38C: _tmp2A5=((
struct Cyc_Absyn_PointerType_struct*)_tmp2A4)->f1;_LL38F: _tmp2A8=(void*)_tmp2A5.elt_typ;
goto _LL38E;_LL38E: _tmp2A7=_tmp2A5.tq;goto _LL38D;_LL38D: _tmp2A6=_tmp2A5.bounds;
goto _LL389;}else{goto _LL38A;}_LL38A: goto _LL38B;_LL389: is_tagged_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp2A6)== (void*)0;goto _LL387;_LL38B:({void*_tmp2A9[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: comprehension not an array type",
sizeof(unsigned char),42),_tag_arr(_tmp2A9,sizeof(void*),0));});_LL387:;}{struct
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
struct Cyc_Absyn_Exp*ai;if(_tmp207 == 0){ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp2AB[2];_tmp2AB[1]=Cyc_Absyn_var_exp(
max,0);_tmp2AB[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2AB,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(_tmp207);
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
_tmp2B6=*((struct _tuple9*)_tmp2B5->hd);_LL393: _tmp2B9=_tmp2B6.f1;goto _LL392;
_LL392: _tmp2B8=_tmp2B6.f2;goto _LL391;_LL391: _tmp2B7=_tmp2B6.f3;goto _LL390;_LL390:
s=Cyc_Absyn_declare_stmt(_tmp2B9,_tmp2B8,_tmp2B7,s,0);}}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL36D;}}}_LL373:(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp206->topt))->v,1,_tmp207,_tmp297,_tmp298))->r));goto _LL36D;
_LL375:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp207,_tmp299))->r));
goto _LL36D;_LL377: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp206->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple0*
_tmp2BC=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Absyn_var_exp(_tmp2BC,
0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp2BD,
0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp207 == 0){mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(
_tmp207);Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp2BE=(void*)_tmp206->r;struct Cyc_Absyn_Exp*_tmp2BF;void*_tmp2C0;_LL395:
if(*((int*)_tmp2BE)== 13){_LL39A: _tmp2C0=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp2BE)->f1;goto _LL399;_LL399: _tmp2BF=((struct Cyc_Absyn_Cast_e_struct*)_tmp2BE)->f2;
goto _LL396;}else{goto _LL397;}_LL397: goto _LL398;_LL396:{struct _tuple8 _tmp2C2=({
struct _tuple8 _tmp2C1;_tmp2C1.f1=Cyc_Tcutil_compress(_tmp2C0);_tmp2C1.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp2BF->topt))->v);_tmp2C1;});void*
_tmp2C3;struct Cyc_Absyn_PtrInfo _tmp2C4;struct Cyc_Absyn_Conref*_tmp2C5;void*
_tmp2C6;struct Cyc_Absyn_PtrInfo _tmp2C7;struct Cyc_Absyn_Conref*_tmp2C8;void*
_tmp2C9;_LL39C: _LL3A3: _tmp2C6=_tmp2C2.f1;if((unsigned int)_tmp2C6 > 3?*((int*)
_tmp2C6)== 4: 0){_LL3A4: _tmp2C7=((struct Cyc_Absyn_PointerType_struct*)_tmp2C6)->f1;
_LL3A6: _tmp2C9=(void*)_tmp2C7.elt_typ;goto _LL3A5;_LL3A5: _tmp2C8=_tmp2C7.bounds;
goto _LL3A0;}else{goto _LL39E;}_LL3A0: _tmp2C3=_tmp2C2.f2;if((unsigned int)_tmp2C3 > 
3?*((int*)_tmp2C3)== 4: 0){_LL3A1: _tmp2C4=((struct Cyc_Absyn_PointerType_struct*)
_tmp2C3)->f1;_LL3A2: _tmp2C5=_tmp2C4.bounds;goto _LL39D;}else{goto _LL39E;}_LL39E:
goto _LL39F;_LL39D:{struct _tuple8 _tmp2CB=({struct _tuple8 _tmp2CA;_tmp2CA.f1=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp2C8);_tmp2CA.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp2C5);_tmp2CA;});void*_tmp2CC;struct Cyc_Absyn_Exp*_tmp2CD;void*_tmp2CE;_LL3A8:
_LL3AE: _tmp2CE=_tmp2CB.f1;if((int)_tmp2CE == 0){goto _LL3AC;}else{goto _LL3AA;}
_LL3AC: _tmp2CC=_tmp2CB.f2;if((unsigned int)_tmp2CC > 1?*((int*)_tmp2CC)== 0: 0){
_LL3AD: _tmp2CD=((struct Cyc_Absyn_Upper_b_struct*)_tmp2CC)->f1;goto _LL3A9;}else{
goto _LL3AA;}_LL3AA: goto _LL3AB;_LL3A9:(void*)(inner_mexp->r=(void*)((void*)(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0))->r));done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2CF[4];_tmp2CF[3]=_tmp2CD;_tmp2CF[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp2C9,0);_tmp2CF[1]=_tmp2BF;_tmp2CF[0]=mexp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp2CF,sizeof(struct Cyc_Absyn_Exp*),4));}),
0))->r));goto _LL3A7;_LL3AB: goto _LL3A7;_LL3A7:;}goto _LL39B;_LL39F: goto _LL39B;
_LL39B:;}goto _LL394;_LL398: goto _LL394;_LL394:;}if(! done){struct Cyc_Absyn_Stmt*
_tmp2D0=Cyc_Absyn_exp_stmt(_tmp2BD,0);struct Cyc_Absyn_Exp*_tmp2D1=Cyc_Absyn_signed_int_exp(
0,0);Cyc_Toc_exp_to_c(nv,_tmp206);_tmp2D0=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp(_tmp2BD,_tmp2D1,0),_tmp206,0),_tmp2D0,0);{void*_tmp2D2=
Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt(_tmp2BC,_tmp2D2,(struct Cyc_Absyn_Exp*)mexp,_tmp2D0,0),0))->r));}}
goto _LL36D;}}_LL36D:;}goto _LL259;_LL27F: Cyc_Toc_exp_to_c(nv,_tmp208);goto _LL259;
_LL281:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp2D3=
_cycalloc(sizeof(*_tmp2D3));_tmp2D3[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp2D4;_tmp2D4.tag=16;_tmp2D4.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp209);_tmp2D4;});
_tmp2D3;})));goto _LL259;_LL283:({void*_tmp2D5[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("__gen() in code generator",
sizeof(unsigned char),26),_tag_arr(_tmp2D5,sizeof(void*),0));});_LL285:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2D6=_cycalloc(sizeof(*
_tmp2D6));_tmp2D6[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2D7;_tmp2D7.tag=18;
_tmp2D7.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp20C);_tmp2D7.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8[0]=({
struct Cyc_Absyn_StructField_struct _tmp2D9;_tmp2D9.tag=0;_tmp2D9.f1=_tmp20B;
_tmp2D9;});_tmp2D8;}));_tmp2D7;});_tmp2D6;})));goto _LL259;_LL287:{void*_tmp2DA=
Cyc_Tcutil_compress(_tmp20F);struct Cyc_Absyn_Structdecl**_tmp2DB;struct Cyc_List_List*
_tmp2DC;struct Cyc_Absyn_Uniondecl**_tmp2DD;struct Cyc_List_List*_tmp2DE;_LL3B0:
if((unsigned int)_tmp2DA > 3?*((int*)_tmp2DA)== 10: 0){_LL3BE: _tmp2DB=((struct Cyc_Absyn_StructType_struct*)
_tmp2DA)->f3;goto _LL3B1;}else{goto _LL3B2;}_LL3B2: if((unsigned int)_tmp2DA > 3?*((
int*)_tmp2DA)== 14: 0){_LL3BF: _tmp2DC=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp2DA)->f1;goto _LL3B3;}else{goto _LL3B4;}_LL3B4: if((unsigned int)_tmp2DA > 3?*((
int*)_tmp2DA)== 11: 0){_LL3C0: _tmp2DD=((struct Cyc_Absyn_UnionType_struct*)_tmp2DA)->f3;
goto _LL3B5;}else{goto _LL3B6;}_LL3B6: if((unsigned int)_tmp2DA > 3?*((int*)_tmp2DA)
== 15: 0){_LL3C1: _tmp2DE=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp2DA)->f1;
goto _LL3B7;}else{goto _LL3B8;}_LL3B8: if((unsigned int)_tmp2DA > 3?*((int*)_tmp2DA)
== 9: 0){goto _LL3B9;}else{goto _LL3BA;}_LL3BA: if((unsigned int)_tmp2DA > 3?*((int*)
_tmp2DA)== 3: 0){goto _LL3BB;}else{goto _LL3BC;}_LL3BC: goto _LL3BD;_LL3B1: if(_tmp2DB
== 0? 1:(*_tmp2DB)->fields == 0){({void*_tmp2DF[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("struct fields must be known",
sizeof(unsigned char),28),_tag_arr(_tmp2DF,sizeof(void*),0));});}_tmp2DC=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*_tmp2DB)->fields))->v;goto
_LL3B3;_LL3B3: {struct Cyc_Absyn_Structfield*_tmp2E0=((struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2DC,(int)_tmp20E);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2E1=_cycalloc(sizeof(*
_tmp2E1));_tmp2E1[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2E2;_tmp2E2.tag=18;
_tmp2E2.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp20F);_tmp2E2.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3[0]=({
struct Cyc_Absyn_StructField_struct _tmp2E4;_tmp2E4.tag=0;_tmp2E4.f1=_tmp2E0->name;
_tmp2E4;});_tmp2E3;}));_tmp2E2;});_tmp2E1;})));goto _LL3AF;}_LL3B5: if(_tmp2DD == 0?
1:(*_tmp2DD)->fields == 0){({void*_tmp2E5[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("union fields must be known",
sizeof(unsigned char),27),_tag_arr(_tmp2E5,sizeof(void*),0));});}_tmp2DE=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*_tmp2DD)->fields))->v;goto
_LL3B7;_LL3B7: {struct Cyc_Absyn_Structfield*_tmp2E6=((struct Cyc_Absyn_Structfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2DE,(int)_tmp20E);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2E7=_cycalloc(sizeof(*
_tmp2E7));_tmp2E7[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2E8;_tmp2E8.tag=18;
_tmp2E8.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp20F);_tmp2E8.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9[0]=({
struct Cyc_Absyn_StructField_struct _tmp2EA;_tmp2EA.tag=0;_tmp2EA.f1=_tmp2E6->name;
_tmp2EA;});_tmp2E9;}));_tmp2E8;});_tmp2E7;})));goto _LL3AF;}_LL3B9:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2EB=_cycalloc(sizeof(*
_tmp2EB));_tmp2EB[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2EC;_tmp2EC.tag=18;
_tmp2EC.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp20F);_tmp2EC.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED[0]=({
struct Cyc_Absyn_StructField_struct _tmp2EE;_tmp2EE.tag=0;_tmp2EE.f1=Cyc_Absyn_fieldname((
int)(_tmp20E + 1));_tmp2EE;});_tmp2ED;}));_tmp2EC;});_tmp2EB;})));goto _LL3AF;
_LL3BB: if(_tmp20E == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp2F0;_tmp2F0.tag=18;_tmp2F0.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp20F);_tmp2F0.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2F1=_cycalloc(sizeof(*
_tmp2F1));_tmp2F1[0]=({struct Cyc_Absyn_StructField_struct _tmp2F2;_tmp2F2.tag=0;
_tmp2F2.f1=Cyc_Toc_tag_sp;_tmp2F2;});_tmp2F1;}));_tmp2F0;});_tmp2EF;})));}else{(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2F3=_cycalloc(
sizeof(*_tmp2F3));_tmp2F3[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2F4;_tmp2F4.tag=
18;_tmp2F4.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp20F);_tmp2F4.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));_tmp2F5[0]=({
struct Cyc_Absyn_StructField_struct _tmp2F6;_tmp2F6.tag=0;_tmp2F6.f1=Cyc_Absyn_fieldname((
int)_tmp20E);_tmp2F6;});_tmp2F5;}));_tmp2F4;});_tmp2F3;})));}goto _LL3AF;_LL3BD:({
void*_tmp2F7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("impossible type for offsetof tuple index",sizeof(unsigned char),41),
_tag_arr(_tmp2F7,sizeof(void*),0));});_LL3AF:;}goto _LL259;_LL289: {void*_tmp2F8=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp210->topt))->v);{
void*_tmp2F9=_tmp2F8;struct Cyc_Absyn_PtrInfo _tmp2FA;struct Cyc_Absyn_Conref*
_tmp2FB;struct Cyc_Absyn_Tqual _tmp2FC;struct Cyc_Absyn_Conref*_tmp2FD;void*_tmp2FE;
void*_tmp2FF;_LL3C3: if((unsigned int)_tmp2F9 > 3?*((int*)_tmp2F9)== 4: 0){_LL3C7:
_tmp2FA=((struct Cyc_Absyn_PointerType_struct*)_tmp2F9)->f1;_LL3CC: _tmp2FF=(void*)
_tmp2FA.elt_typ;goto _LL3CB;_LL3CB: _tmp2FE=(void*)_tmp2FA.rgn_typ;goto _LL3CA;
_LL3CA: _tmp2FD=_tmp2FA.nullable;goto _LL3C9;_LL3C9: _tmp2FC=_tmp2FA.tq;goto _LL3C8;
_LL3C8: _tmp2FB=_tmp2FA.bounds;goto _LL3C4;}else{goto _LL3C5;}_LL3C5: goto _LL3C6;
_LL3C4:{void*_tmp300=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp2FB);_LL3CE:
if((unsigned int)_tmp300 > 1?*((int*)_tmp300)== 0: 0){goto _LL3CF;}else{goto _LL3D0;}
_LL3D0: if((int)_tmp300 == 0){goto _LL3D1;}else{goto _LL3CD;}_LL3CF: {int
do_null_check=Cyc_Toc_need_null_check(_tmp210);Cyc_Toc_exp_to_c(nv,_tmp210);if(
do_null_check){(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp210->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp301=
_cycalloc(sizeof(*_tmp301));_tmp301->hd=_tmp210;_tmp301->tl=0;_tmp301;}),0),0),0))->r));}
goto _LL3CD;}_LL3D1: {struct Cyc_Absyn_Exp*_tmp302=Cyc_Absyn_uint_exp(0,0);_tmp302->topt=({
struct Cyc_Core_Opt*_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303->v=(void*)Cyc_Absyn_uint_t;
_tmp303;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp210,_tmp302,
0))->r));Cyc_Toc_exp_to_c(nv,e);goto _LL3CD;}_LL3CD:;}goto _LL3C2;_LL3C6:({void*
_tmp304[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer",sizeof(unsigned char),29),_tag_arr(
_tmp304,sizeof(void*),0));});_LL3C2:;}goto _LL259;}_LL28B: Cyc_Toc_exp_to_c(nv,
_tmp212);if(Cyc_Toc_is_poly_project(e)){(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((
void*)e->r,0),0))->r));}goto _LL259;_LL28D: {void*e1typ=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp214->topt))->v);int do_null_check=Cyc_Toc_need_null_check(
_tmp214);Cyc_Toc_exp_to_c(nv,_tmp214);{int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_Conref*_tmp306;struct Cyc_Absyn_Tqual _tmp307;struct Cyc_Absyn_Conref*
_tmp308;void*_tmp309;void*_tmp30A;struct Cyc_Absyn_PtrInfo _tmp305=Cyc_Toc_get_ptr_type(
e1typ);_LL3D7: _tmp30A=(void*)_tmp305.elt_typ;goto _LL3D6;_LL3D6: _tmp309=(void*)
_tmp305.rgn_typ;goto _LL3D5;_LL3D5: _tmp308=_tmp305.nullable;goto _LL3D4;_LL3D4:
_tmp307=_tmp305.tq;goto _LL3D3;_LL3D3: _tmp306=_tmp305.bounds;goto _LL3D2;_LL3D2:{
void*_tmp30B=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp306);struct Cyc_Absyn_Exp*
_tmp30C;_LL3D9: if((unsigned int)_tmp30B > 1?*((int*)_tmp30B)== 0: 0){_LL3DD:
_tmp30C=((struct Cyc_Absyn_Upper_b_struct*)_tmp30B)->f1;goto _LL3DA;}else{goto
_LL3DB;}_LL3DB: if((int)_tmp30B == 0){goto _LL3DC;}else{goto _LL3D8;}_LL3DA: if(Cyc_Evexp_eval_const_uint_exp(
_tmp30C)< 1){({void*_tmp30D[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof(unsigned char),46),_tag_arr(_tmp30D,sizeof(void*),0));});}if(
do_null_check){(void*)(e->r=(void*)((void*)(Cyc_Absyn_structarrow_exp(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp214->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp30E=
_cycalloc(sizeof(*_tmp30E));_tmp30E->hd=_tmp214;_tmp30E->tl=0;_tmp30E;}),0),0),
_tmp213,0))->r));}goto _LL3D8;_LL3DC: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp30A);(
void*)(_tmp214->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,
_tmp307),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*
_tmp30F[3];_tmp30F[2]=Cyc_Absyn_uint_exp(0,0);_tmp30F[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp30F[0]=Cyc_Absyn_copy_exp(_tmp214);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp30F,sizeof(struct Cyc_Absyn_Exp*),3));}),
0),0))->r));goto _LL3D8;}_LL3D8:;}if(is_poly){(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((
void*)e->r,0),0))->r));}goto _LL259;}}_LL28F: {void*_tmp310=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp216->topt))->v);{void*_tmp311=
_tmp310;struct Cyc_List_List*_tmp312;struct Cyc_Absyn_PtrInfo _tmp313;struct Cyc_Absyn_Conref*
_tmp314;struct Cyc_Absyn_Tqual _tmp315;struct Cyc_Absyn_Conref*_tmp316;void*_tmp317;
void*_tmp318;_LL3DF: if((unsigned int)_tmp311 > 3?*((int*)_tmp311)== 9: 0){_LL3E5:
_tmp312=((struct Cyc_Absyn_TupleType_struct*)_tmp311)->f1;goto _LL3E0;}else{goto
_LL3E1;}_LL3E1: if((unsigned int)_tmp311 > 3?*((int*)_tmp311)== 4: 0){_LL3E6:
_tmp313=((struct Cyc_Absyn_PointerType_struct*)_tmp311)->f1;_LL3EB: _tmp318=(void*)
_tmp313.elt_typ;goto _LL3EA;_LL3EA: _tmp317=(void*)_tmp313.rgn_typ;goto _LL3E9;
_LL3E9: _tmp316=_tmp313.nullable;goto _LL3E8;_LL3E8: _tmp315=_tmp313.tq;goto _LL3E7;
_LL3E7: _tmp314=_tmp313.bounds;goto _LL3E2;}else{goto _LL3E3;}_LL3E3: goto _LL3E4;
_LL3E0: Cyc_Toc_exp_to_c(nv,_tmp216);Cyc_Toc_exp_to_c(nv,_tmp215);{unsigned int i=
Cyc_Evexp_eval_const_uint_exp(_tmp215)+ 1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_structmember_exp(
_tmp216,Cyc_Absyn_fieldname((int)i),0))->r));goto _LL3DE;}_LL3E2: {struct Cyc_List_List*
_tmp319=Cyc_Toc_get_relns(_tmp216);int _tmp31A=Cyc_Toc_check_bounds(_tmp319,
_tmp216,_tmp215);if(Cyc_Toc_warn_bounds_checks? ! _tmp31A: 0){({struct Cyc_Std_String_pa_struct
_tmp31C;_tmp31C.tag=0;_tmp31C.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e);{
void*_tmp31B[1]={& _tmp31C};Cyc_Tcutil_warn(e->loc,_tag_arr("bounds check necessary for %s",
sizeof(unsigned char),30),_tag_arr(_tmp31B,sizeof(void*),1));}});}Cyc_Toc_exp_to_c(
nv,_tmp216);Cyc_Toc_exp_to_c(nv,_tmp215);Cyc_Toc_total_bounds_checks ++;{void*
_tmp31D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp314);struct Cyc_Absyn_Exp*
_tmp31E;_LL3ED: if((unsigned int)_tmp31D > 1?*((int*)_tmp31D)== 0: 0){_LL3F1:
_tmp31E=((struct Cyc_Absyn_Upper_b_struct*)_tmp31D)->f1;goto _LL3EE;}else{goto
_LL3EF;}_LL3EF: if((int)_tmp31D == 0){goto _LL3F0;}else{goto _LL3EC;}_LL3EE: _tmp31E=
Cyc_Absyn_uint_exp(Cyc_Evexp_eval_const_uint_exp(_tmp31E),0);{int possibly_null=((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp316);void*ta1=
Cyc_Toc_typ_to_c(_tmp318);void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp315);if(_tmp31A){
Cyc_Toc_bounds_checks_eliminated ++;}else{if(possibly_null){(void*)(e->r=(void*)((
void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp31F[4];_tmp31F[3]=_tmp215;_tmp31F[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp31F[1]=_tmp31E;_tmp31F[0]=_tmp216;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp31F,sizeof(struct Cyc_Absyn_Exp*),4));}),
0),0),0))->r));}else{(void*)(_tmp215->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp320[2];_tmp320[1]=Cyc_Absyn_copy_exp(_tmp215);_tmp320[0]=
_tmp31E;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp320,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}goto _LL3EC;}_LL3F0: {void*
ta1=Cyc_Toc_typ_to_c_array(_tmp318);if(_tmp31A){Cyc_Toc_bounds_checks_eliminated
++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
ta1,_tmp315),Cyc_Absyn_structmember_exp(_tmp216,Cyc_Toc_curr_sp,0),0),_tmp215,0))->r));}
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
ta1,_tmp315),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*
_tmp321[3];_tmp321[2]=_tmp215;_tmp321[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp321[
0]=_tmp216;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp321,sizeof(struct Cyc_Absyn_Exp*),3));}),0),0),0))->r));}goto _LL3EC;}_LL3EC:;}
goto _LL3DE;}_LL3E4:({void*_tmp322[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof(unsigned char),49),_tag_arr(_tmp322,sizeof(void*),0));});_LL3DE:;}goto
_LL259;}_LL291: if(! nv->toplevel){(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,0,0,_tmp217))->r));}else{struct Cyc_List_List*_tmp323=((struct Cyc_List_List*(*)(
struct _tuple3*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp217);struct _tagged_arr*_tmp324=Cyc_Toc_add_tuple_type(
_tmp323);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp217 != 0;(_tmp217=_tmp217->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp217->hd);dles=({struct Cyc_List_List*
_tmp325=_cycalloc(sizeof(*_tmp325));_tmp325->hd=({struct _tuple4*_tmp326=
_cycalloc(sizeof(*_tmp326));_tmp326->f1=0;_tmp326->f2=(struct Cyc_Absyn_Exp*)
_tmp217->hd;_tmp326;});_tmp325->tl=dles;_tmp325;});}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,dles,0))->r));}goto _LL259;_LL293:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,_tmp218,0))->r));{struct Cyc_List_List*_tmp327=_tmp218;for(0;_tmp327 != 0;
_tmp327=_tmp327->tl){struct _tuple4 _tmp329;struct Cyc_Absyn_Exp*_tmp32A;struct
_tuple4*_tmp328=(struct _tuple4*)_tmp327->hd;_tmp329=*_tmp328;_LL3F3: _tmp32A=
_tmp329.f2;goto _LL3F2;_LL3F2: Cyc_Toc_exp_to_c(nv,_tmp32A);}}goto _LL259;_LL295: {
unsigned int _tmp32B=Cyc_Evexp_eval_const_uint_exp(_tmp21A);Cyc_Toc_exp_to_c(nv,
_tmp219);{struct Cyc_List_List*es=0;if(! Cyc_Toc_is_zero(_tmp219)){unsigned int i=0;
for(0;i < _tmp32B;i ++){es=({struct Cyc_List_List*_tmp32C=_cycalloc(sizeof(*_tmp32C));
_tmp32C->hd=({struct _tuple4*_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D->f1=0;
_tmp32D->f2=_tmp219;_tmp32D;});_tmp32C->tl=es;_tmp32C;});}}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));goto _LL259;}}_LL297: if(! nv->toplevel){(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,old_typ,0,0,_tmp21D,_tmp21F))->r));}
else{if(_tmp21C == 0){({void*_tmp32E[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Struct_e: missing structdecl pointer",
sizeof(unsigned char),37),_tag_arr(_tmp32E,sizeof(void*),0));});}{struct Cyc_Absyn_Structdecl*
sd2=(struct Cyc_Absyn_Structdecl*)_check_null(_tmp21C);struct _RegionHandle _tmp32F=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp32F;_push_region(rgn);{struct Cyc_List_List*
_tmp330=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,e->loc,_tmp21D,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sd2->fields))->v);
struct Cyc_List_List*_tmp331=0;{struct Cyc_List_List*_tmp332=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(sd2->fields))->v;for(0;_tmp332 != 0;_tmp332=
_tmp332->tl){struct Cyc_List_List*_tmp333=_tmp330;for(0;_tmp333 != 0;_tmp333=
_tmp333->tl){if((*((struct _tuple10*)_tmp333->hd)).f1 == (struct Cyc_Absyn_Structfield*)
_tmp332->hd){struct _tuple10 _tmp335;struct Cyc_Absyn_Exp*_tmp336;struct Cyc_Absyn_Structfield*
_tmp337;struct _tuple10*_tmp334=(struct _tuple10*)_tmp333->hd;_tmp335=*_tmp334;
_LL3F6: _tmp337=_tmp335.f1;goto _LL3F5;_LL3F5: _tmp336=_tmp335.f2;goto _LL3F4;_LL3F4: {
void*_tmp338=(void*)_tmp337->type;Cyc_Toc_exp_to_c(nv,_tmp336);if(Cyc_Toc_is_void_star(
_tmp338)){(void*)(_tmp336->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp336->r,0),0))->r));}_tmp331=({struct Cyc_List_List*
_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->hd=({struct _tuple4*_tmp33A=
_cycalloc(sizeof(*_tmp33A));_tmp33A->f1=0;_tmp33A->f2=_tmp336;_tmp33A;});_tmp339->tl=
_tmp331;_tmp339;});break;}}}}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp331),0))->r));};
_pop_region(rgn);}}goto _LL259;_LL299: {struct Cyc_List_List*fs;{void*_tmp33B=Cyc_Tcutil_compress(
_tmp221);struct Cyc_List_List*_tmp33C;_LL3F8: if((unsigned int)_tmp33B > 3?*((int*)
_tmp33B)== 14: 0){_LL3FC: _tmp33C=((struct Cyc_Absyn_AnonStructType_struct*)_tmp33B)->f1;
goto _LL3F9;}else{goto _LL3FA;}_LL3FA: goto _LL3FB;_LL3F9: fs=_tmp33C;goto _LL3F7;
_LL3FB:({struct Cyc_Std_String_pa_struct _tmp33E;_tmp33E.tag=0;_tmp33E.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(_tmp221);{void*_tmp33D[1]={& _tmp33E};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("anon struct has type %s",
sizeof(unsigned char),24),_tag_arr(_tmp33D,sizeof(void*),1));}});_LL3F7:;}{
struct _RegionHandle _tmp33F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp33F;
_push_region(rgn);{struct Cyc_List_List*_tmp340=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp220,
fs);for(0;_tmp340 != 0;_tmp340=_tmp340->tl){struct _tuple10 _tmp342;struct Cyc_Absyn_Exp*
_tmp343;struct Cyc_Absyn_Structfield*_tmp344;struct _tuple10*_tmp341=(struct
_tuple10*)_tmp340->hd;_tmp342=*_tmp341;_LL3FF: _tmp344=_tmp342.f1;goto _LL3FE;
_LL3FE: _tmp343=_tmp342.f2;goto _LL3FD;_LL3FD: {void*_tmp345=(void*)_tmp344->type;
Cyc_Toc_exp_to_c(nv,_tmp343);if(Cyc_Toc_is_void_star(_tmp345)){(void*)(_tmp343->r=(
void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp343->r,0),0))->r));}}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,_tmp220,0))->r));};_pop_region(rgn);}goto _LL259;}_LL29B: {struct _tuple0*qv=
_tmp222->name;struct Cyc_Absyn_Exp*tag_exp=_tmp223->is_xtunion? Cyc_Absyn_var_exp(
qv,0): Cyc_Toc_tunion_tag(_tmp223,qv,0);(void*)(e->r=(void*)((void*)tag_exp->r));
goto _LL259;}_LL29D: {struct _tuple0*_tmp346=_tmp225->name;struct Cyc_List_List*
_tmp347=_tmp225->typs;struct _tuple0*_tmp348=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp349=Cyc_Absyn_var_exp(_tmp348,0);void*_tmp34A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp346,_tag_arr("_struct",sizeof(unsigned char),8)));struct Cyc_Absyn_Exp*
tag_exp=_tmp226->is_xtunion? Cyc_Absyn_var_exp(_tmp346,0): Cyc_Toc_tunion_tag(
_tmp226,_tmp346,1);if(nv->toplevel){struct Cyc_List_List*dles=0;for(0;_tmp227 != 0;(
_tmp227=_tmp227->tl,_tmp347=_tmp347->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp227->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple3*)((struct Cyc_List_List*)
_check_null(_tmp347))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ)){cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);}dles=({struct Cyc_List_List*
_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->hd=({struct _tuple4*_tmp34C=
_cycalloc(sizeof(*_tmp34C));_tmp34C->f1=0;_tmp34C->f2=cur_e;_tmp34C;});_tmp34B->tl=
dles;_tmp34B;});}dles=({struct Cyc_List_List*_tmp34D=_cycalloc(sizeof(*_tmp34D));
_tmp34D->hd=({struct _tuple4*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E->f1=0;
_tmp34E->f2=tag_exp;_tmp34E;});_tmp34D->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp34D;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt*_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F->v=Cyc_Toc_collapse_qvar_tag(
_tmp346,_tag_arr("_struct",sizeof(unsigned char),8));_tmp34F;}),dles,0))->r));}
else{struct Cyc_List_List*_tmp350=({struct Cyc_List_List*_tmp356=_cycalloc(sizeof(*
_tmp356));_tmp356->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_structmember_exp(_tmp349,
Cyc_Toc_tag_sp,0),tag_exp,0);_tmp356->tl=0;_tmp356;});{int i=1;for(0;_tmp227 != 0;(((
_tmp227=_tmp227->tl,i ++)),_tmp347=_tmp347->tl)){struct Cyc_Absyn_Exp*cur_e=(
struct Cyc_Absyn_Exp*)_tmp227->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct
_tuple3*)((struct Cyc_List_List*)_check_null(_tmp347))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ)){cur_e=Cyc_Absyn_cast_exp(field_typ,
cur_e,0);}{struct Cyc_Absyn_Stmt*_tmp351=Cyc_Absyn_assign_stmt(Cyc_Absyn_structmember_exp(
_tmp349,Cyc_Absyn_fieldname(i),0),cur_e,0);_tmp350=({struct Cyc_List_List*_tmp352=
_cycalloc(sizeof(*_tmp352));_tmp352->hd=_tmp351;_tmp352->tl=_tmp350;_tmp352;});}}}{
struct Cyc_Absyn_Stmt*_tmp353=Cyc_Absyn_exp_stmt(_tmp349,0);struct Cyc_Absyn_Stmt*
_tmp354=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({
struct Cyc_List_List*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355->hd=_tmp353;
_tmp355->tl=_tmp350;_tmp355;})),0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt(_tmp348,_tmp34A,0,_tmp354,0),0))->r));}}goto _LL259;}
_LL29F: goto _LL259;_LL2A1: goto _LL259;_LL2A3: {void*t_c=Cyc_Toc_typ_to_c(*((void**)
_check_null(_tmp22B)));Cyc_Toc_exp_to_c(nv,_tmp22A);if(_tmp229){struct _tuple0*
_tmp357=Cyc_Toc_temp_var();struct _tuple0*_tmp358=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(_tmp22D){xexp=_tmp22A;
if(_tmp22C != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_check_null(
_tmp22C);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp357,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp22B,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp357,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp359[3];_tmp359[2]=Cyc_Absyn_var_exp(_tmp357,0);_tmp359[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp359[0]=Cyc_Absyn_var_exp(_tmp358,0);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp359,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp22A,0);if(_tmp22C != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp22C);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp357,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp22B,Cyc_Absyn_var_exp(_tmp357,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp35A[3];
_tmp35A[2]=Cyc_Absyn_var_exp(_tmp357,0);_tmp35A[1]=Cyc_Absyn_uint_exp(1,0);
_tmp35A[0]=Cyc_Absyn_var_exp(_tmp358,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp35A,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}{struct Cyc_Absyn_Stmt*_tmp35B=Cyc_Absyn_declare_stmt(_tmp357,Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp358,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp35B,0))->r));}}else{if(_tmp22C != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp22C);Cyc_Toc_exp_to_c(
nv,rgn);(void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp22B,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL259;}_LL2A5: Cyc_Toc_stmt_to_c(nv,_tmp22E);goto _LL259;
_LL2A7:({void*_tmp35C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("UnresolvedMem",sizeof(unsigned char),14),_tag_arr(
_tmp35C,sizeof(void*),0));});_LL2A9:({void*_tmp35D[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("compoundlit",
sizeof(unsigned char),12),_tag_arr(_tmp35D,sizeof(void*),0));});_LL2AB:({void*
_tmp35E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("codegen",sizeof(unsigned char),8),_tag_arr(_tmp35E,sizeof(void*),0));});
_LL2AD:({void*_tmp35F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(_tag_arr("fill",sizeof(unsigned char),5),_tag_arr(_tmp35F,sizeof(
void*),0));});_LL259:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct _tagged_arr*succ_lab,struct
_tagged_arr*fail_lab){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(e1,e2,0),
Cyc_Absyn_goto_stmt(succ_lab,0),Cyc_Absyn_goto_stmt(fail_lab,0),0);}struct
_tuple11{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple12{struct _tuple0*f1;void*f2;};struct _tuple13{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};static struct _tuple11 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*
nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct _tagged_arr*succ_lab,struct _tagged_arr*fail_lab,
struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{void*_tmp360=(void*)p->r;
struct Cyc_Absyn_Vardecl*_tmp361;struct Cyc_Absyn_Vardecl _tmp362;struct _tuple0*
_tmp363;struct Cyc_Absyn_Vardecl*_tmp364;int _tmp365;void*_tmp366;unsigned char
_tmp367;struct _tagged_arr _tmp368;struct Cyc_Absyn_Enumfield*_tmp369;struct Cyc_Absyn_Enumdecl*
_tmp36A;struct Cyc_Absyn_Enumfield*_tmp36B;void*_tmp36C;struct Cyc_List_List*
_tmp36D;struct Cyc_List_List*_tmp36E;struct Cyc_Absyn_Tunionfield*_tmp36F;struct
Cyc_Absyn_Tuniondecl*_tmp370;struct Cyc_Absyn_Pat*_tmp371;struct Cyc_Absyn_Pat
_tmp372;void*_tmp373;struct Cyc_List_List*_tmp374;struct Cyc_List_List*_tmp375;
struct Cyc_Absyn_Tunionfield*_tmp376;struct Cyc_Absyn_Tuniondecl*_tmp377;struct Cyc_List_List*
_tmp378;struct Cyc_List_List*_tmp379;struct Cyc_List_List*_tmp37A;struct Cyc_List_List*
_tmp37B;struct Cyc_Core_Opt*_tmp37C;struct Cyc_Absyn_Structdecl*_tmp37D;struct Cyc_Absyn_Pat*
_tmp37E;_LL401: if((unsigned int)_tmp360 > 2?*((int*)_tmp360)== 0: 0){_LL425:
_tmp361=((struct Cyc_Absyn_Var_p_struct*)_tmp360)->f1;_tmp362=*_tmp361;_LL426:
_tmp363=_tmp362.name;goto _LL402;}else{goto _LL403;}_LL403: if((int)_tmp360 == 0){
goto _LL404;}else{goto _LL405;}_LL405: if((unsigned int)_tmp360 > 2?*((int*)_tmp360)
== 6: 0){_LL427: _tmp364=((struct Cyc_Absyn_Reference_p_struct*)_tmp360)->f1;goto
_LL406;}else{goto _LL407;}_LL407: if((int)_tmp360 == 1){goto _LL408;}else{goto _LL409;}
_LL409: if((unsigned int)_tmp360 > 2?*((int*)_tmp360)== 1: 0){_LL429: _tmp366=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp360)->f1;goto _LL428;_LL428: _tmp365=((struct Cyc_Absyn_Int_p_struct*)
_tmp360)->f2;goto _LL40A;}else{goto _LL40B;}_LL40B: if((unsigned int)_tmp360 > 2?*((
int*)_tmp360)== 2: 0){_LL42A: _tmp367=((struct Cyc_Absyn_Char_p_struct*)_tmp360)->f1;
goto _LL40C;}else{goto _LL40D;}_LL40D: if((unsigned int)_tmp360 > 2?*((int*)_tmp360)
== 3: 0){_LL42B: _tmp368=((struct Cyc_Absyn_Float_p_struct*)_tmp360)->f1;goto _LL40E;}
else{goto _LL40F;}_LL40F: if((unsigned int)_tmp360 > 2?*((int*)_tmp360)== 9: 0){
_LL42D: _tmp36A=((struct Cyc_Absyn_Enum_p_struct*)_tmp360)->f1;goto _LL42C;_LL42C:
_tmp369=((struct Cyc_Absyn_Enum_p_struct*)_tmp360)->f2;goto _LL410;}else{goto
_LL411;}_LL411: if((unsigned int)_tmp360 > 2?*((int*)_tmp360)== 10: 0){_LL42F:
_tmp36C=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp360)->f1;goto _LL42E;
_LL42E: _tmp36B=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp360)->f2;goto _LL412;}
else{goto _LL413;}_LL413: if((unsigned int)_tmp360 > 2?*((int*)_tmp360)== 8: 0){
_LL433: _tmp370=((struct Cyc_Absyn_Tunion_p_struct*)_tmp360)->f1;goto _LL432;_LL432:
_tmp36F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp360)->f2;goto _LL431;_LL431:
_tmp36E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp360)->f3;goto _LL430;_LL430:
_tmp36D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp360)->f4;if(_tmp36D == 0){goto
_LL414;}else{goto _LL415;}}else{goto _LL415;}_LL415: if((unsigned int)_tmp360 > 2?*((
int*)_tmp360)== 5: 0){_LL435: _tmp371=((struct Cyc_Absyn_Pointer_p_struct*)_tmp360)->f1;
_tmp372=*_tmp371;_LL436: _tmp373=(void*)_tmp372.r;if((unsigned int)_tmp373 > 2?*((
int*)_tmp373)== 8: 0){_LL43A: _tmp377=((struct Cyc_Absyn_Tunion_p_struct*)_tmp373)->f1;
goto _LL439;_LL439: _tmp376=((struct Cyc_Absyn_Tunion_p_struct*)_tmp373)->f2;goto
_LL438;_LL438: _tmp375=((struct Cyc_Absyn_Tunion_p_struct*)_tmp373)->f3;goto _LL437;
_LL437: _tmp374=((struct Cyc_Absyn_Tunion_p_struct*)_tmp373)->f4;goto _LL434;}else{
goto _LL417;}}else{goto _LL417;}_LL434: if(_tmp374 != 0){goto _LL416;}else{goto _LL417;}
_LL417: if((unsigned int)_tmp360 > 2?*((int*)_tmp360)== 8: 0){_LL43B: _tmp378=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp360)->f4;goto _LL418;}else{goto _LL419;}
_LL419: if((unsigned int)_tmp360 > 2?*((int*)_tmp360)== 4: 0){_LL43C: _tmp379=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp360)->f1;goto _LL41A;}else{goto _LL41B;}_LL41B:
if((unsigned int)_tmp360 > 2?*((int*)_tmp360)== 7: 0){_LL440: _tmp37D=((struct Cyc_Absyn_Struct_p_struct*)
_tmp360)->f1;goto _LL43F;_LL43F: _tmp37C=((struct Cyc_Absyn_Struct_p_struct*)
_tmp360)->f2;goto _LL43E;_LL43E: _tmp37B=((struct Cyc_Absyn_Struct_p_struct*)
_tmp360)->f3;goto _LL43D;_LL43D: _tmp37A=((struct Cyc_Absyn_Struct_p_struct*)
_tmp360)->f4;goto _LL41C;}else{goto _LL41D;}_LL41D: if((unsigned int)_tmp360 > 2?*((
int*)_tmp360)== 5: 0){_LL441: _tmp37E=((struct Cyc_Absyn_Pointer_p_struct*)_tmp360)->f1;
goto _LL41E;}else{goto _LL41F;}_LL41F: if((unsigned int)_tmp360 > 2?*((int*)_tmp360)
== 11: 0){goto _LL420;}else{goto _LL421;}_LL421: if((unsigned int)_tmp360 > 2?*((int*)
_tmp360)== 12: 0){goto _LL422;}else{goto _LL423;}_LL423: if((unsigned int)_tmp360 > 2?*((
int*)_tmp360)== 13: 0){goto _LL424;}else{goto _LL400;}_LL402: nv=Cyc_Toc_add_varmap(
nv,_tmp363,r);goto _LL404;_LL404: s=Cyc_Absyn_goto_stmt(succ_lab,0);goto _LL400;
_LL406: {struct _tuple0*_tmp37F=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp380=_region_malloc(rgn,sizeof(*_tmp380));_tmp380->hd=({struct _tuple12*
_tmp381=_region_malloc(rgn,sizeof(*_tmp381));_tmp381->f1=_tmp37F;_tmp381->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp381;});_tmp380->tl=decls;_tmp380;});nv=
Cyc_Toc_add_varmap(nv,_tmp364->name,Cyc_Absyn_var_exp(_tmp37F,0));s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp37F,0),Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0),0),0),Cyc_Absyn_goto_stmt(
succ_lab,0),0);goto _LL400;}_LL408: s=Cyc_Toc_if_eq_goto_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),succ_lab,fail_lab);goto _LL400;_LL40A: s=Cyc_Toc_if_eq_goto_stmt(r,Cyc_Absyn_int_exp(
_tmp366,_tmp365,0),succ_lab,fail_lab);goto _LL400;_LL40C: s=Cyc_Toc_if_eq_goto_stmt(
r,Cyc_Absyn_char_exp(_tmp367,0),succ_lab,fail_lab);goto _LL400;_LL40E: s=Cyc_Toc_if_eq_goto_stmt(
r,Cyc_Absyn_float_exp(_tmp368,0),succ_lab,fail_lab);goto _LL400;_LL410: s=Cyc_Toc_if_eq_goto_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp382=_cycalloc(
sizeof(*_tmp382));_tmp382[0]=({struct Cyc_Absyn_Enum_e_struct _tmp383;_tmp383.tag=
31;_tmp383.f1=_tmp369->name;_tmp383.f2=(struct Cyc_Absyn_Enumdecl*)_tmp36A;
_tmp383.f3=(struct Cyc_Absyn_Enumfield*)_tmp369;_tmp383;});_tmp382;}),0),succ_lab,
fail_lab);goto _LL400;_LL412: s=Cyc_Toc_if_eq_goto_stmt(r,Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AnonEnum_e_struct*_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384[0]=({
struct Cyc_Absyn_AnonEnum_e_struct _tmp385;_tmp385.tag=32;_tmp385.f1=_tmp36B->name;
_tmp385.f2=(void*)_tmp36C;_tmp385.f3=(struct Cyc_Absyn_Enumfield*)_tmp36B;_tmp385;});
_tmp384;}),0),succ_lab,fail_lab);goto _LL400;_LL414: {struct Cyc_Absyn_Exp*cmp_exp;
if(_tmp370->is_xtunion){cmp_exp=Cyc_Absyn_var_exp(_tmp36F->name,0);}else{cmp_exp=
Cyc_Toc_tunion_tag(_tmp370,_tmp36F->name,0);r=Cyc_Absyn_cast_exp(Cyc_Absyn_sint_t,
r,0);}s=Cyc_Toc_if_eq_goto_stmt(r,cmp_exp,succ_lab,fail_lab);goto _LL400;}_LL416: {
struct Cyc_List_List*ss=0;int cnt=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp374);struct _tuple0*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp376->name,_tag_arr("_struct",
sizeof(unsigned char),8));struct Cyc_Absyn_Exp*rcast=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r,0);struct Cyc_List_List*_tmp386=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
rgn,_tmp376->typs);struct Cyc_List_List*_tmp387=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(rgn,_tmp374);for(0;_tmp387
!= 0;(((_tmp387=_tmp387->tl,_tmp386=((struct Cyc_List_List*)_check_null(_tmp386))->tl)),
cnt --)){struct Cyc_Absyn_Pat*_tmp388=(struct Cyc_Absyn_Pat*)_tmp387->hd;if((void*)
_tmp388->r == (void*)0){continue;}{void*_tmp389=(*((struct _tuple3*)((struct Cyc_List_List*)
_check_null(_tmp386))->hd)).f2;struct _tuple0*_tmp38A=Cyc_Toc_temp_var();void*
_tmp38B=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp388->topt))->v;void*_tmp38C=
Cyc_Toc_typ_to_c(_tmp38B);struct _tagged_arr*_tmp38D=Cyc_Toc_fresh_label();struct
Cyc_Absyn_Exp*_tmp38E=Cyc_Absyn_structarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star(Cyc_Toc_typ_to_c(_tmp389))){_tmp38E=Cyc_Absyn_cast_exp(
_tmp38C,_tmp38E,0);}decls=({struct Cyc_List_List*_tmp38F=_region_malloc(rgn,
sizeof(*_tmp38F));_tmp38F->hd=({struct _tuple12*_tmp390=_region_malloc(rgn,
sizeof(*_tmp390));_tmp390->f1=_tmp38A;_tmp390->f2=_tmp38C;_tmp390;});_tmp38F->tl=
decls;_tmp38F;});{struct _tuple11 _tmp391=Cyc_Toc_xlate_pat(nv,rgn,_tmp38B,Cyc_Absyn_var_exp(
_tmp38A,0),_tmp38E,_tmp388,succ_lab,fail_lab,decls);nv=_tmp391.f1;decls=_tmp391.f2;{
struct Cyc_Absyn_Stmt*_tmp392=_tmp391.f3;struct Cyc_Absyn_Stmt*_tmp393=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp38A,0),_tmp38E,0);struct Cyc_Absyn_Stmt*_tmp394=Cyc_Absyn_seq_stmt(
_tmp393,_tmp392,0);ss=({struct Cyc_List_List*_tmp395=_cycalloc(sizeof(*_tmp395));
_tmp395->hd=Cyc_Absyn_label_stmt(_tmp38D,_tmp394,0);_tmp395->tl=ss;_tmp395;});
succ_lab=_tmp38D;}}}}if(ss == 0){ss=({struct Cyc_List_List*_tmp396=_cycalloc(
sizeof(*_tmp396));_tmp396->hd=Cyc_Absyn_goto_stmt(succ_lab,0);_tmp396->tl=0;
_tmp396;});}{struct Cyc_Absyn_Exp*test_exp;if(_tmp377->is_xtunion){struct Cyc_Absyn_Exp*
e2=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),
r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp376->name,0);test_exp=Cyc_Absyn_eq_exp(e1,e,0);s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(e1,e,0),Cyc_Absyn_seq_stmts(ss,0),Cyc_Absyn_goto_stmt(fail_lab,
0),0);}else{int max_tag=Cyc_Toc_num_void_tags(_tmp377);struct Cyc_Absyn_Exp*
max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);struct Cyc_Absyn_Exp*e3=
Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_t,Cyc_Toc_mt_tq),r,0);
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*e5=Cyc_Absyn_gt_exp(
Cyc_Absyn_cast_exp(Cyc_Absyn_uint_t,r,0),max_tag_exp,0);struct Cyc_Absyn_Exp*e=
Cyc_Toc_tunion_tag(_tmp377,_tmp376->name,1);test_exp=max_tag > 0? Cyc_Absyn_and_exp(
e5,Cyc_Absyn_eq_exp(e1,e,0),0): Cyc_Absyn_eq_exp(e1,e,0);}s=Cyc_Absyn_ifthenelse_stmt(
test_exp,Cyc_Absyn_seq_stmts(ss,0),Cyc_Absyn_goto_stmt(fail_lab,0),0);goto _LL400;}}
_LL418: _tmp379=_tmp378;goto _LL41A;_LL41A: {struct Cyc_List_List*ss=0;int cnt=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp379);{struct Cyc_List_List*_tmp397=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
rgn,_tmp379);for(0;_tmp397 != 0;(_tmp397=_tmp397->tl,cnt --)){struct Cyc_Absyn_Pat*
_tmp398=(struct Cyc_Absyn_Pat*)_tmp397->hd;if((void*)_tmp398->r == (void*)0){
continue;}{struct _tuple0*_tmp399=Cyc_Toc_temp_var();void*_tmp39A=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp398->topt))->v;struct _tagged_arr*_tmp39B=Cyc_Toc_fresh_label();
decls=({struct Cyc_List_List*_tmp39C=_region_malloc(rgn,sizeof(*_tmp39C));_tmp39C->hd=({
struct _tuple12*_tmp39D=_region_malloc(rgn,sizeof(*_tmp39D));_tmp39D->f1=_tmp399;
_tmp39D->f2=Cyc_Toc_typ_to_c(_tmp39A);_tmp39D;});_tmp39C->tl=decls;_tmp39C;});{
struct _tuple11 _tmp39E=Cyc_Toc_xlate_pat(nv,rgn,_tmp39A,Cyc_Absyn_var_exp(_tmp399,
0),Cyc_Absyn_structmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp398,succ_lab,
fail_lab,decls);nv=_tmp39E.f1;decls=_tmp39E.f2;{struct Cyc_Absyn_Stmt*_tmp39F=
_tmp39E.f3;struct Cyc_Absyn_Stmt*_tmp3A0=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp399,0),Cyc_Absyn_structmember_exp(r,Cyc_Absyn_fieldname(
cnt),0),0),_tmp39F,0);ss=({struct Cyc_List_List*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));
_tmp3A1->hd=Cyc_Absyn_label_stmt(_tmp39B,_tmp3A0,0);_tmp3A1->tl=ss;_tmp3A1;});
succ_lab=_tmp39B;}}}}}if(ss == 0){ss=({struct Cyc_List_List*_tmp3A2=_cycalloc(
sizeof(*_tmp3A2));_tmp3A2->hd=Cyc_Absyn_goto_stmt(succ_lab,0);_tmp3A2->tl=0;
_tmp3A2;});}s=Cyc_Absyn_seq_stmts(ss,0);goto _LL400;}_LL41C: {struct Cyc_List_List*
ss=0;{struct Cyc_List_List*_tmp3A3=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rrev)(rgn,_tmp37A);for(0;_tmp3A3 != 0;_tmp3A3=
_tmp3A3->tl){struct _tuple13*_tmp3A4=(struct _tuple13*)_tmp3A3->hd;struct Cyc_Absyn_Pat*
_tmp3A5=(*_tmp3A4).f2;if((void*)_tmp3A5->r == (void*)0){continue;}{struct
_tagged_arr*f;{void*_tmp3A6=(void*)((struct Cyc_List_List*)_check_null((*_tmp3A4).f1))->hd;
struct _tagged_arr*_tmp3A7;_LL443: if(*((int*)_tmp3A6)== 1){_LL447: _tmp3A7=((
struct Cyc_Absyn_FieldName_struct*)_tmp3A6)->f1;goto _LL444;}else{goto _LL445;}
_LL445: goto _LL446;_LL444: f=_tmp3A7;goto _LL442;_LL446:(int)_throw((void*)Cyc_Toc_Match_error);
_LL442:;}{struct _tuple0*_tmp3A8=Cyc_Toc_temp_var();void*_tmp3A9=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp3A5->topt))->v;void*_tmp3AA=Cyc_Toc_typ_to_c(
_tmp3A9);struct _tagged_arr*_tmp3AB=Cyc_Toc_fresh_label();decls=({struct Cyc_List_List*
_tmp3AC=_region_malloc(rgn,sizeof(*_tmp3AC));_tmp3AC->hd=({struct _tuple12*
_tmp3AD=_region_malloc(rgn,sizeof(*_tmp3AD));_tmp3AD->f1=_tmp3A8;_tmp3AD->f2=
_tmp3AA;_tmp3AD;});_tmp3AC->tl=decls;_tmp3AC;});{struct _tuple11 _tmp3AE=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3A9,Cyc_Absyn_var_exp(_tmp3A8,0),Cyc_Absyn_structmember_exp(path,f,0),
_tmp3A5,succ_lab,fail_lab,decls);nv=_tmp3AE.f1;decls=_tmp3AE.f2;{struct Cyc_Absyn_Exp*
_tmp3AF=Cyc_Absyn_structmember_exp(r,f,0);if(Cyc_Toc_is_void_star((void*)((
struct Cyc_Absyn_Structfield*)_check_null(Cyc_Absyn_lookup_field((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp37D->fields))->v,f)))->type)){_tmp3AF=Cyc_Absyn_cast_exp(
_tmp3AA,_tmp3AF,0);}{struct Cyc_Absyn_Stmt*_tmp3B0=_tmp3AE.f3;struct Cyc_Absyn_Stmt*
_tmp3B1=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp3A8,0),
_tmp3AF,0),_tmp3B0,0);ss=({struct Cyc_List_List*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));
_tmp3B2->hd=Cyc_Absyn_label_stmt(_tmp3AB,_tmp3B1,0);_tmp3B2->tl=ss;_tmp3B2;});
succ_lab=_tmp3AB;}}}}}}}if(ss == 0){ss=({struct Cyc_List_List*_tmp3B3=_cycalloc(
sizeof(*_tmp3B3));_tmp3B3->hd=Cyc_Absyn_goto_stmt(succ_lab,0);_tmp3B3->tl=0;
_tmp3B3;});}s=Cyc_Absyn_seq_stmts(ss,0);goto _LL400;}_LL41E: {struct _tuple0*
_tmp3B4=Cyc_Toc_temp_var();void*_tmp3B5=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp37E->topt))->v;decls=({struct Cyc_List_List*_tmp3B6=_region_malloc(rgn,
sizeof(*_tmp3B6));_tmp3B6->hd=({struct _tuple12*_tmp3B7=_region_malloc(rgn,
sizeof(*_tmp3B7));_tmp3B7->f1=_tmp3B4;_tmp3B7->f2=Cyc_Toc_typ_to_c(_tmp3B5);
_tmp3B7;});_tmp3B6->tl=decls;_tmp3B6;});{struct _tuple11 _tmp3B8=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3B5,Cyc_Absyn_var_exp(_tmp3B4,0),Cyc_Absyn_deref_exp(path,0),_tmp37E,
succ_lab,fail_lab,decls);nv=_tmp3B8.f1;decls=_tmp3B8.f2;{struct Cyc_Absyn_Stmt*
_tmp3B9=_tmp3B8.f3;struct Cyc_Absyn_Stmt*_tmp3BA=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp3B4,0),Cyc_Absyn_deref_exp(r,0),0),_tmp3B9,0);if(Cyc_Toc_is_nullable(
t)){s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),
0),Cyc_Absyn_goto_stmt(fail_lab,0),_tmp3BA,0);}else{s=_tmp3BA;}goto _LL400;}}}
_LL420:({void*_tmp3BB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("unknownid",sizeof(unsigned char),10),_tag_arr(
_tmp3BB,sizeof(void*),0));});_LL422:({void*_tmp3BC[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unknowncall",
sizeof(unsigned char),12),_tag_arr(_tmp3BC,sizeof(void*),0));});_LL424:({void*
_tmp3BD[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unknownfields",sizeof(unsigned char),14),_tag_arr(_tmp3BD,sizeof(void*),
0));});_LL400:;}return({struct _tuple11 _tmp3BE;_tmp3BE.f1=nv;_tmp3BE.f2=decls;
_tmp3BE.f3=s;_tmp3BE;});}struct _tuple14{struct _tagged_arr*f1;struct _tagged_arr*
f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple14*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){return({struct _tuple14*
_tmp3BF=_region_malloc(r,sizeof(*_tmp3BF));_tmp3BF->f1=Cyc_Toc_fresh_label();
_tmp3BF->f2=Cyc_Toc_fresh_label();_tmp3BF->f3=sc;_tmp3BF;});}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp3C0=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp3C1=Cyc_Tcutil_compress(
_tmp3C0);_LL449: if((unsigned int)_tmp3C1 > 3?*((int*)_tmp3C1)== 5: 0){goto _LL44A;}
else{goto _LL44B;}_LL44B: if((unsigned int)_tmp3C1 > 3?*((int*)_tmp3C1)== 12: 0){
goto _LL44C;}else{goto _LL44D;}_LL44D: goto _LL44E;_LL44A: goto _LL44C;_LL44C:
leave_as_switch=1;goto _LL448;_LL44E: leave_as_switch=0;goto _LL448;_LL448:;}{
struct Cyc_List_List*_tmp3C2=scs;for(0;_tmp3C2 != 0;_tmp3C2=_tmp3C2->tl){if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)
_tmp3C2->hd)->pat_vars))->v != 0? 1:((struct Cyc_Absyn_Switch_clause*)_tmp3C2->hd)->where_clause
!= 0){leave_as_switch=0;break;}}}if(leave_as_switch){struct _tagged_arr*next_l=
Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp3C3=scs;for(0;_tmp3C3 != 0;_tmp3C3=
_tmp3C3->tl){struct Cyc_Absyn_Stmt*_tmp3C4=((struct Cyc_Absyn_Switch_clause*)
_tmp3C3->hd)->body;((struct Cyc_Absyn_Switch_clause*)_tmp3C3->hd)->body=Cyc_Absyn_label_stmt(
next_l,_tmp3C4,0);next_l=Cyc_Toc_fresh_label();Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
nv,next_l),_tmp3C4);}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct
_tagged_arr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp3C5=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp3C5;_push_region(rgn);{struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp3C6=lscs;for(0;_tmp3C6 != 0;_tmp3C6=_tmp3C6->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple14*)_tmp3C6->hd)).f3;struct _tagged_arr*fail_lab=_tmp3C6->tl == 
0? end_l:(*((struct _tuple14*)((struct Cyc_List_List*)_check_null(_tmp3C6->tl))->hd)).f1;
struct _tagged_arr*succ_lab=(*((struct _tuple14*)_tmp3C6->hd)).f2;if(sc->where_clause
== 0){struct _tuple11 _tmp3C7=Cyc_Toc_xlate_pat(nv,rgn,_tmp3C0,r,path,sc->pattern,
succ_lab,fail_lab,decls);nvs=({struct Cyc_List_List*_tmp3C8=_region_malloc(rgn,
sizeof(*_tmp3C8));_tmp3C8->hd=_tmp3C7.f1;_tmp3C8->tl=nvs;_tmp3C8;});decls=
_tmp3C7.f2;test_stmts=({struct Cyc_List_List*_tmp3C9=_region_malloc(rgn,sizeof(*
_tmp3C9));_tmp3C9->hd=Cyc_Absyn_label_stmt((*((struct _tuple14*)_tmp3C6->hd)).f1,
_tmp3C7.f3,0);_tmp3C9->tl=test_stmts;_tmp3C9;});}else{struct Cyc_Absyn_Exp*
_tmp3CA=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);struct _tagged_arr*
_tmp3CB=Cyc_Toc_fresh_label();struct _tuple11 _tmp3CC=Cyc_Toc_xlate_pat(nv,rgn,
_tmp3C0,r,path,sc->pattern,_tmp3CB,fail_lab,decls);Cyc_Toc_exp_to_c(_tmp3CC.f1,
_tmp3CA);nvs=({struct Cyc_List_List*_tmp3CD=_region_malloc(rgn,sizeof(*_tmp3CD));
_tmp3CD->hd=_tmp3CC.f1;_tmp3CD->tl=nvs;_tmp3CD;});decls=_tmp3CC.f2;{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp3CA,Cyc_Absyn_goto_stmt(succ_lab,0),Cyc_Absyn_goto_stmt(
fail_lab,0),0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_label_stmt(_tmp3CB,s2,0);
test_stmts=({struct Cyc_List_List*_tmp3CE=_region_malloc(rgn,sizeof(*_tmp3CE));
_tmp3CE->hd=Cyc_Absyn_label_stmt((*((struct _tuple14*)_tmp3C6->hd)).f1,Cyc_Absyn_seq_stmt(
_tmp3CC.f3,s3,0),0);_tmp3CE->tl=test_stmts;_tmp3CE;});}}}}{struct Cyc_Absyn_Stmt*
_tmp3CF=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
test_stmts),0);nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
nvs);{struct Cyc_Absyn_Stmt*clauses=Cyc_Absyn_skip_stmt(0);if(lscs != 0){for(0;
lscs->tl != 0;(lscs=lscs->tl,nvs=nvs->tl)){struct Cyc_Toc_Env*_tmp3D0=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=((*((struct
_tuple14*)lscs->hd)).f3)->body;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp3D0,end_l,(*((struct _tuple14*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd)).f2,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((*((struct _tuple14*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd)).f3)->pat_vars))->v,(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);clauses=Cyc_Absyn_seq_stmt(
clauses,Cyc_Absyn_label_stmt((*((struct _tuple14*)lscs->hd)).f2,s,0),0);}{struct
Cyc_Toc_Env*_tmp3D1=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=((*((struct _tuple14*)lscs->hd)).f3)->body;Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env(_tmp3D1,end_l),s);clauses=Cyc_Absyn_seq_stmt(
clauses,Cyc_Absyn_label_stmt((*((struct _tuple14*)lscs->hd)).f2,s,0),0);}}{struct
Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(_tmp3CF,Cyc_Absyn_seq_stmt(clauses,Cyc_Absyn_label_stmt(
end_l,Cyc_Absyn_skip_stmt(0),0),0),0);for(decls;decls != 0;decls=decls->tl){
struct _tuple12 _tmp3D3;void*_tmp3D4;struct _tuple0*_tmp3D5;struct _tuple12*_tmp3D2=(
struct _tuple12*)((struct Cyc_List_List*)_check_null(decls))->hd;_tmp3D3=*_tmp3D2;
_LL451: _tmp3D5=_tmp3D3.f1;goto _LL450;_LL450: _tmp3D4=_tmp3D3.f2;goto _LL44F;_LL44F:
res=Cyc_Absyn_declare_stmt(_tmp3D5,_tmp3D4,0,res,0);}(void*)(whole_s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r));}}}};_pop_region(
rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,struct Cyc_Core_Opt*topt,struct Cyc_Absyn_Exp*e,int exhaust,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp3D6=
_cycalloc(sizeof(*_tmp3D6));_tmp3D6->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp3D6->tl=0;_tmp3D6;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0){(void*)(s->r=(void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp3D7=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Absyn_Stmt*_tmp3D9;struct Cyc_Absyn_Stmt*
_tmp3DA;struct Cyc_Absyn_Exp*_tmp3DB;struct Cyc_Absyn_Stmt*_tmp3DC;struct Cyc_Absyn_Stmt*
_tmp3DD;struct Cyc_Absyn_Exp*_tmp3DE;struct Cyc_Absyn_Stmt*_tmp3DF;struct _tuple2
_tmp3E0;struct Cyc_Absyn_Exp*_tmp3E1;struct Cyc_Absyn_Stmt*_tmp3E2;struct Cyc_Absyn_Stmt*
_tmp3E3;struct Cyc_Absyn_Stmt*_tmp3E4;struct Cyc_Absyn_Stmt*_tmp3E5;struct _tuple2
_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;struct _tuple2 _tmp3E8;struct Cyc_Absyn_Exp*
_tmp3E9;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_List_List*_tmp3EB;struct Cyc_Absyn_Exp*
_tmp3EC;struct Cyc_Absyn_Switch_clause**_tmp3ED;struct Cyc_List_List*_tmp3EE;
struct Cyc_Absyn_Stmt*_tmp3EF;struct Cyc_Absyn_Decl*_tmp3F0;struct Cyc_Absyn_Stmt*
_tmp3F1;struct _tagged_arr*_tmp3F2;struct _tuple2 _tmp3F3;struct Cyc_Absyn_Exp*
_tmp3F4;struct Cyc_Absyn_Stmt*_tmp3F5;struct Cyc_List_List*_tmp3F6;struct Cyc_Absyn_Stmt*
_tmp3F7;struct Cyc_Absyn_Stmt*_tmp3F8;struct Cyc_Absyn_Vardecl*_tmp3F9;struct Cyc_Absyn_Tvar*
_tmp3FA;_LL453: if((int)_tmp3D7 == 0){goto _LL454;}else{goto _LL455;}_LL455: if((
unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 0: 0){_LL47D: _tmp3D8=((struct Cyc_Absyn_Exp_s_struct*)
_tmp3D7)->f1;goto _LL456;}else{goto _LL457;}_LL457: if((unsigned int)_tmp3D7 > 1?*((
int*)_tmp3D7)== 1: 0){_LL47F: _tmp3DA=((struct Cyc_Absyn_Seq_s_struct*)_tmp3D7)->f1;
goto _LL47E;_LL47E: _tmp3D9=((struct Cyc_Absyn_Seq_s_struct*)_tmp3D7)->f2;goto
_LL458;}else{goto _LL459;}_LL459: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 2:
0){_LL480: _tmp3DB=((struct Cyc_Absyn_Return_s_struct*)_tmp3D7)->f1;goto _LL45A;}
else{goto _LL45B;}_LL45B: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 3: 0){
_LL483: _tmp3DE=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D7)->f1;goto _LL482;
_LL482: _tmp3DD=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D7)->f2;goto _LL481;
_LL481: _tmp3DC=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D7)->f3;goto _LL45C;}
else{goto _LL45D;}_LL45D: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 4: 0){
_LL485: _tmp3E0=((struct Cyc_Absyn_While_s_struct*)_tmp3D7)->f1;_LL486: _tmp3E1=
_tmp3E0.f1;goto _LL484;_LL484: _tmp3DF=((struct Cyc_Absyn_While_s_struct*)_tmp3D7)->f2;
goto _LL45E;}else{goto _LL45F;}_LL45F: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)
== 5: 0){_LL487: _tmp3E2=((struct Cyc_Absyn_Break_s_struct*)_tmp3D7)->f1;goto _LL460;}
else{goto _LL461;}_LL461: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 6: 0){
_LL488: _tmp3E3=((struct Cyc_Absyn_Continue_s_struct*)_tmp3D7)->f1;goto _LL462;}
else{goto _LL463;}_LL463: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 7: 0){
_LL489: _tmp3E4=((struct Cyc_Absyn_Goto_s_struct*)_tmp3D7)->f2;goto _LL464;}else{
goto _LL465;}_LL465: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 8: 0){_LL48F:
_tmp3EA=((struct Cyc_Absyn_For_s_struct*)_tmp3D7)->f1;goto _LL48D;_LL48D: _tmp3E8=((
struct Cyc_Absyn_For_s_struct*)_tmp3D7)->f2;_LL48E: _tmp3E9=_tmp3E8.f1;goto _LL48B;
_LL48B: _tmp3E6=((struct Cyc_Absyn_For_s_struct*)_tmp3D7)->f3;_LL48C: _tmp3E7=
_tmp3E6.f1;goto _LL48A;_LL48A: _tmp3E5=((struct Cyc_Absyn_For_s_struct*)_tmp3D7)->f4;
goto _LL466;}else{goto _LL467;}_LL467: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)
== 19: 0){goto _LL468;}else{goto _LL469;}_LL469: if((unsigned int)_tmp3D7 > 1?*((int*)
_tmp3D7)== 9: 0){_LL491: _tmp3EC=((struct Cyc_Absyn_Switch_s_struct*)_tmp3D7)->f1;
goto _LL490;_LL490: _tmp3EB=((struct Cyc_Absyn_Switch_s_struct*)_tmp3D7)->f2;goto
_LL46A;}else{goto _LL46B;}_LL46B: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 
11: 0){_LL493: _tmp3EE=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp3D7)->f1;goto
_LL492;_LL492: _tmp3ED=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp3D7)->f2;goto
_LL46C;}else{goto _LL46D;}_LL46D: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 
12: 0){_LL495: _tmp3F0=((struct Cyc_Absyn_Decl_s_struct*)_tmp3D7)->f1;goto _LL494;
_LL494: _tmp3EF=((struct Cyc_Absyn_Decl_s_struct*)_tmp3D7)->f2;goto _LL46E;}else{
goto _LL46F;}_LL46F: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 15: 0){_LL497:
_tmp3F2=((struct Cyc_Absyn_Label_s_struct*)_tmp3D7)->f1;goto _LL496;_LL496: _tmp3F1=((
struct Cyc_Absyn_Label_s_struct*)_tmp3D7)->f2;goto _LL470;}else{goto _LL471;}_LL471:
if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 16: 0){_LL49A: _tmp3F5=((struct Cyc_Absyn_Do_s_struct*)
_tmp3D7)->f1;goto _LL498;_LL498: _tmp3F3=((struct Cyc_Absyn_Do_s_struct*)_tmp3D7)->f2;
_LL499: _tmp3F4=_tmp3F3.f1;goto _LL472;}else{goto _LL473;}_LL473: if((unsigned int)
_tmp3D7 > 1?*((int*)_tmp3D7)== 17: 0){_LL49C: _tmp3F7=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3D7)->f1;goto _LL49B;_LL49B: _tmp3F6=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3D7)->f2;goto _LL474;}else{goto _LL475;}_LL475: if((unsigned int)_tmp3D7 > 1?*((
int*)_tmp3D7)== 18: 0){_LL49F: _tmp3FA=((struct Cyc_Absyn_Region_s_struct*)_tmp3D7)->f1;
goto _LL49E;_LL49E: _tmp3F9=((struct Cyc_Absyn_Region_s_struct*)_tmp3D7)->f2;goto
_LL49D;_LL49D: _tmp3F8=((struct Cyc_Absyn_Region_s_struct*)_tmp3D7)->f3;goto _LL476;}
else{goto _LL477;}_LL477: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)== 10: 0){
goto _LL478;}else{goto _LL479;}_LL479: if((unsigned int)_tmp3D7 > 1?*((int*)_tmp3D7)
== 13: 0){goto _LL47A;}else{goto _LL47B;}_LL47B: if((unsigned int)_tmp3D7 > 1?*((int*)
_tmp3D7)== 14: 0){goto _LL47C;}else{goto _LL452;}_LL454: return;_LL456: Cyc_Toc_exp_to_c(
nv,_tmp3D8);return;_LL458: Cyc_Toc_stmt_to_c(nv,_tmp3DA);s=_tmp3D9;continue;
_LL45A: {struct Cyc_Core_Opt*topt=0;if(_tmp3DB != 0){topt=({struct Cyc_Core_Opt*
_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3DB->topt))->v);_tmp3FB;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp3DB));}if(s->try_depth > 0){if(topt != 0){
struct _tuple0*_tmp3FC=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp3FD=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp3FC,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp3FC,(void*)topt->v,_tmp3DB,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp3FD,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL45C: Cyc_Toc_exp_to_c(nv,_tmp3DE);Cyc_Toc_stmt_to_c(nv,_tmp3DD);s=
_tmp3DC;continue;_LL45E: Cyc_Toc_exp_to_c(nv,_tmp3E1);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp3DF);return;_LL460: if(nv->break_lab != 0){(void*)(s->r=(void*)((void*)(
Cyc_Absyn_goto_stmt((struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,
0))->r));}{int dest_depth=_tmp3E2 == 0? 0: _tmp3E2->try_depth;Cyc_Toc_do_npop_before(
s->try_depth - dest_depth,s);return;}_LL462: if(nv->continue_lab != 0){(void*)(s->r=(
void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((struct Cyc_Core_Opt*)
_check_null(nv->continue_lab))->v,0))->r));}_tmp3E4=_tmp3E3;goto _LL464;_LL464:
Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp3E4))->try_depth,
s);return;_LL466: Cyc_Toc_exp_to_c(nv,_tmp3EA);Cyc_Toc_exp_to_c(nv,_tmp3E9);Cyc_Toc_exp_to_c(
nv,_tmp3E7);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp3E5);return;_LL468:({void*
_tmp3FE[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("forarray not yet implemented",sizeof(unsigned char),29),_tag_arr(
_tmp3FE,sizeof(void*),0));});return;_LL46A: Cyc_Toc_xlate_switch(nv,s,_tmp3EC,
_tmp3EB);return;_LL46C: if(nv->fallthru_info == 0){({void*_tmp3FF[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("fallthru in unexpected place",
sizeof(unsigned char),29),_tag_arr(_tmp3FF,sizeof(void*),0));});}{struct _tuple6
_tmp401;struct Cyc_Dict_Dict*_tmp402;struct Cyc_List_List*_tmp403;struct
_tagged_arr*_tmp404;struct _tuple6*_tmp400=(struct _tuple6*)((struct Cyc_Core_Opt*)
_check_null(nv->fallthru_info))->v;_tmp401=*_tmp400;_LL4A3: _tmp404=_tmp401.f1;
goto _LL4A2;_LL4A2: _tmp403=_tmp401.f2;goto _LL4A1;_LL4A1: _tmp402=_tmp401.f3;goto
_LL4A0;_LL4A0: {struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp404,0);Cyc_Toc_do_npop_before(
s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp3ED)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp405=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp403);struct Cyc_List_List*_tmp406=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp3EE);for(0;_tmp405 != 0;(_tmp405=_tmp405->tl,
_tmp406=_tmp406->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp406))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)(
_tmp402,(struct _tuple0*)_tmp405->hd),(struct Cyc_Absyn_Exp*)_tmp406->hd,0),s2,0);}(
void*)(s->r=(void*)((void*)s2->r));return;}}}_LL46E:{void*_tmp407=(void*)_tmp3F0->r;
struct Cyc_Absyn_Vardecl*_tmp408;int _tmp409;struct Cyc_Absyn_Exp*_tmp40A;struct Cyc_Core_Opt*
_tmp40B;struct Cyc_Absyn_Pat*_tmp40C;struct Cyc_List_List*_tmp40D;_LL4A5: if(*((int*)
_tmp407)== 0){_LL4AD: _tmp408=((struct Cyc_Absyn_Var_d_struct*)_tmp407)->f1;goto
_LL4A6;}else{goto _LL4A7;}_LL4A7: if(*((int*)_tmp407)== 2){_LL4B1: _tmp40C=((struct
Cyc_Absyn_Let_d_struct*)_tmp407)->f1;goto _LL4B0;_LL4B0: _tmp40B=((struct Cyc_Absyn_Let_d_struct*)
_tmp407)->f3;goto _LL4AF;_LL4AF: _tmp40A=((struct Cyc_Absyn_Let_d_struct*)_tmp407)->f4;
goto _LL4AE;_LL4AE: _tmp409=((struct Cyc_Absyn_Let_d_struct*)_tmp407)->f5;goto
_LL4A8;}else{goto _LL4A9;}_LL4A9: if(*((int*)_tmp407)== 3){_LL4B2: _tmp40D=((struct
Cyc_Absyn_Letv_d_struct*)_tmp407)->f1;goto _LL4AA;}else{goto _LL4AB;}_LL4AB: goto
_LL4AC;_LL4A6: {struct Cyc_Toc_Env*_tmp40E=Cyc_Toc_add_varmap(nv,_tmp408->name,
Cyc_Absyn_varb_exp(_tmp408->name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp40F=
_cycalloc(sizeof(*_tmp40F));_tmp40F[0]=({struct Cyc_Absyn_Local_b_struct _tmp410;
_tmp410.tag=3;_tmp410.f1=_tmp408;_tmp410;});_tmp40F;}),0));Cyc_Toc_local_decl_to_c(
_tmp40E,_tmp40E,_tmp408,_tmp3EF);goto _LL4A4;}_LL4A8:{void*_tmp411=(void*)_tmp40C->r;
struct Cyc_Absyn_Vardecl*_tmp412;_LL4B4: if((unsigned int)_tmp411 > 2?*((int*)
_tmp411)== 0: 0){_LL4B8: _tmp412=((struct Cyc_Absyn_Var_p_struct*)_tmp411)->f1;goto
_LL4B5;}else{goto _LL4B6;}_LL4B6: goto _LL4B7;_LL4B5: {struct _tuple0*old_name=
_tmp412->name;struct _tuple0*new_name=Cyc_Toc_temp_var();_tmp412->name=new_name;
_tmp412->initializer=(struct Cyc_Absyn_Exp*)_tmp40A;(void*)(_tmp3F0->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413[
0]=({struct Cyc_Absyn_Var_d_struct _tmp414;_tmp414.tag=0;_tmp414.f1=_tmp412;
_tmp414;});_tmp413;})));{struct Cyc_Toc_Env*_tmp415=Cyc_Toc_add_varmap(nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416[0]=({struct Cyc_Absyn_Local_b_struct
_tmp417;_tmp417.tag=3;_tmp417.f1=_tmp412;_tmp417;});_tmp416;}),0));Cyc_Toc_local_decl_to_c(
_tmp415,nv,_tmp412,_tmp3EF);goto _LL4B3;}}_LL4B7:(void*)(s->r=(void*)((void*)(Cyc_Toc_letdecl_to_c(
nv,_tmp40C,_tmp40B,_tmp40A,_tmp409,_tmp3EF))->r));goto _LL4B3;_LL4B3:;}goto _LL4A4;
_LL4AA: {struct Cyc_List_List*_tmp418=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp40D);if(_tmp418 == 0){({void*_tmp419[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("empty Letv_d",
sizeof(unsigned char),13),_tag_arr(_tmp419,sizeof(void*),0));});}(void*)(_tmp3F0->r=(
void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp41A=_cycalloc(sizeof(*_tmp41A));
_tmp41A[0]=({struct Cyc_Absyn_Var_d_struct _tmp41B;_tmp41B.tag=0;_tmp41B.f1=(
struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(_tmp418))->hd;
_tmp41B;});_tmp41A;})));_tmp418=_tmp418->tl;for(0;_tmp418 != 0;_tmp418=_tmp418->tl){
struct Cyc_Absyn_Decl*_tmp41C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=({struct Cyc_Absyn_Var_d_struct
_tmp41E;_tmp41E.tag=0;_tmp41E.f1=(struct Cyc_Absyn_Vardecl*)_tmp418->hd;_tmp41E;});
_tmp41D;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(_tmp41C,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto _LL4A4;}_LL4AC:({void*
_tmp41F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("bad nested declaration within function",sizeof(unsigned char),39),
_tag_arr(_tmp41F,sizeof(void*),0));});goto _LL4A4;_LL4A4:;}return;_LL470: s=
_tmp3F1;continue;_LL472: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp3F5);Cyc_Toc_exp_to_c(
nv,_tmp3F4);return;_LL474: {struct _tuple0*h_var=Cyc_Toc_temp_var();struct _tuple0*
e_var=Cyc_Toc_temp_var();struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();struct
Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(
e_var,0);struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ);void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_t);e_exp->topt=({
struct Cyc_Core_Opt*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->v=(void*)e_typ;
_tmp420;});nv=Cyc_Toc_add_varmap(nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp3F7);{
struct Cyc_Absyn_Stmt*_tmp421=Cyc_Absyn_seq_stmt(_tmp3F7,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_handler_e,0,0),0),0);struct _tuple0*_tmp422=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp423=Cyc_Absyn_var_exp(_tmp422,0);struct Cyc_Absyn_Vardecl*
_tmp424=Cyc_Absyn_new_vardecl(_tmp422,Cyc_Absyn_exn_typ,0);_tmp423->topt=({
struct Cyc_Core_Opt*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->v=(void*)Cyc_Absyn_exn_typ;
_tmp425;});{struct Cyc_Absyn_Pat*_tmp426=({struct Cyc_Absyn_Pat*_tmp436=_cycalloc(
sizeof(*_tmp436));_tmp436->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*
_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438[0]=({struct Cyc_Absyn_Var_p_struct
_tmp439;_tmp439.tag=0;_tmp439.f1=_tmp424;_tmp439;});_tmp438;}));_tmp436->topt=({
struct Cyc_Core_Opt*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->v=(void*)Cyc_Absyn_exn_typ;
_tmp437;});_tmp436->loc=0;_tmp436;});struct Cyc_Absyn_Exp*_tmp427=Cyc_Absyn_throw_exp(
_tmp423,0);_tmp427->topt=({struct Cyc_Core_Opt*_tmp428=_cycalloc(sizeof(*_tmp428));
_tmp428->v=(void*)((void*)0);_tmp428;});{struct Cyc_Absyn_Stmt*_tmp429=Cyc_Absyn_exp_stmt(
_tmp427,0);struct Cyc_Absyn_Switch_clause*_tmp42A=({struct Cyc_Absyn_Switch_clause*
_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->pattern=_tmp426;_tmp433->pat_vars=({
struct Cyc_Core_Opt*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->v=({struct Cyc_List_List*
_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->hd=_tmp424;_tmp435->tl=0;_tmp435;});
_tmp434;});_tmp433->where_clause=0;_tmp433->body=_tmp429;_tmp433->loc=0;_tmp433;});
struct Cyc_Absyn_Stmt*_tmp42B=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp3F6,({struct
Cyc_List_List*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->hd=_tmp42A;_tmp432->tl=
0;_tmp432;})),0);Cyc_Toc_stmt_to_c(nv,_tmp42B);{struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp431=_cycalloc(sizeof(*_tmp431));
_tmp431->hd=Cyc_Absyn_structmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp431->tl=0;
_tmp431;}),0);struct Cyc_Absyn_Stmt*_tmp42D=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp430=_cycalloc(sizeof(*_tmp430));
_tmp430->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp430->tl=0;_tmp430;}),0),0);struct
Cyc_Absyn_Exp*_tmp42E=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp42F=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp42D,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp42E,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp42C,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp42F,0),Cyc_Absyn_skip_stmt(0),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp421,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_cast_exp(
e_typ,Cyc_Toc__exn_thrown_e,0),_tmp42B,0),0),0),0),0),0))->r));return;}}}}}
_LL476: {void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple0*
rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp3F9->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(nv,x_var,x_exp),_tmp3F8);(void*)(s->r=(
void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp43A=
_cycalloc(sizeof(*_tmp43A));_tmp43A->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0);_tmp43A->tl=0;_tmp43A;}),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*
_tmp43B[1];_tmp43B[0]=x_exp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp43B,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),Cyc_Absyn_seq_stmt(
_tmp3F8,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct
Cyc_Absyn_Exp*_tmp43C[1];_tmp43C[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp43C,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL478:({void*_tmp43D[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("switchC_s",
sizeof(unsigned char),10),_tag_arr(_tmp43D,sizeof(void*),0));});_LL47A:({void*
_tmp43E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("cut",sizeof(unsigned char),4),_tag_arr(_tmp43E,sizeof(void*),0));});
_LL47C:({void*_tmp43F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(_tag_arr("splice",sizeof(unsigned char),7),_tag_arr(_tmp43F,
sizeof(void*),0));});_LL452:;}}struct _tuple15{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f){f->tvs=0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((
void*)f->ret_type));{struct Cyc_List_List*_tmp440=f->args;for(0;_tmp440 != 0;
_tmp440=_tmp440->tl){(*((struct _tuple15*)_tmp440->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple15*)_tmp440->hd)).f3);{struct _tuple0*_tmp441=({struct _tuple0*_tmp442=
_cycalloc(sizeof(*_tmp442));_tmp442->f1=(void*)0;_tmp442->f2=(*((struct _tuple15*)
_tmp440->hd)).f1;_tmp442;});nv=Cyc_Toc_add_varmap(nv,_tmp441,Cyc_Absyn_var_exp(
_tmp441,0));}}}if((unsigned int)f->cyc_varargs?((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0: 0){int _tmp444;void*_tmp445;struct Cyc_Absyn_Tqual
_tmp446;struct Cyc_Core_Opt*_tmp447;struct Cyc_Absyn_VarargInfo _tmp443=*((struct
Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_LL4BD: _tmp447=_tmp443.name;
goto _LL4BC;_LL4BC: _tmp446=_tmp443.tq;goto _LL4BB;_LL4BB: _tmp445=(void*)_tmp443.type;
goto _LL4BA;_LL4BA: _tmp444=_tmp443.inject;goto _LL4B9;_LL4B9: {void*_tmp448=Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ(_tmp445,(void*)2,_tmp446));struct _tuple0*_tmp449=({struct
_tuple0*_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->f1=(void*)0;_tmp44C->f2=(
struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null(_tmp447))->v;_tmp44C;});f->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
f->args,({struct Cyc_List_List*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A->hd=({
struct _tuple15*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->f1=(struct
_tagged_arr*)_tmp447->v;_tmp44B->f2=_tmp446;_tmp44B->f3=_tmp448;_tmp44B;});
_tmp44A->tl=0;_tmp44A;}));nv=Cyc_Toc_add_varmap(nv,_tmp449,Cyc_Absyn_var_exp(
_tmp449,0));f->cyc_varargs=0;}}{struct Cyc_List_List*_tmp44D=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp44D != 0;_tmp44D=
_tmp44D->tl){(void*)(((struct Cyc_Absyn_Vardecl*)_tmp44D->hd)->type=(void*)Cyc_Toc_typ_to_c((
void*)((struct Cyc_Absyn_Vardecl*)_tmp44D->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(
nv),f->body);}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp44E=s;_LL4BF: if((
int)_tmp44E == 1){goto _LL4C0;}else{goto _LL4C1;}_LL4C1: if((int)_tmp44E == 4){goto
_LL4C2;}else{goto _LL4C3;}_LL4C3: goto _LL4C4;_LL4C0: return(void*)2;_LL4C2: return(
void*)3;_LL4C4: return s;_LL4BE:;}struct Cyc_Core_Opt*Cyc_Toc_structs_so_far=0;
static void Cyc_Toc_structdecl_to_c(struct Cyc_Absyn_Structdecl*s){struct _tuple0*
_tmp44F=(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(s->name))->v;if(Cyc_Toc_structs_so_far
== 0){Cyc_Toc_structs_so_far=({struct Cyc_Core_Opt*_tmp450=_cycalloc(sizeof(*
_tmp450));_tmp450->v=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,
struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp450;});}{int
seen_defn_before;struct Cyc_Core_Opt*_tmp451=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_structs_so_far))->v,(*_tmp44F).f2);if(_tmp451 == 0){
seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_structs_so_far))->v=(
void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct
Cyc_Absyn_Structdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_structs_so_far))->v,(*_tmp44F).f2,s));}else{if(((struct Cyc_Absyn_Structdecl*)
_tmp451->v)->fields == 0){(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_structs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Structdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_structs_so_far))->v,(*
_tmp44F).f2,s));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(s->sc=(
void*)((void*)2));s->tvs=0;s->name=({struct Cyc_Core_Opt*_tmp452=_cycalloc(
sizeof(*_tmp452));_tmp452->v=_tmp44F;_tmp452;});if(s->fields != 0){if(
seen_defn_before){s->fields=0;}else{struct Cyc_List_List*_tmp453=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(s->fields))->v;for(0;_tmp453 != 0;_tmp453=_tmp453->tl){(
void*)(((struct Cyc_Absyn_Structfield*)_tmp453->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Structfield*)_tmp453->hd)->type));}}}}}struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far=0;static void Cyc_Toc_uniondecl_to_c(struct Cyc_Absyn_Uniondecl*
u){struct _tuple0*_tmp454=(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(u->name))->v;
if(Cyc_Toc_unions_so_far == 0){Cyc_Toc_unions_so_far=({struct Cyc_Core_Opt*_tmp455=
_cycalloc(sizeof(*_tmp455));_tmp455->v=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);
_tmp455;});}{int seen_defn_before;struct Cyc_Core_Opt*_tmp456=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_unions_so_far))->v,(*_tmp454).f2);if(
_tmp456 == 0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_unions_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_unions_so_far))->v,(*
_tmp454).f2,u));}else{if(((struct Cyc_Absyn_Uniondecl*)_tmp456->v)->fields == 0){(
struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_unions_so_far))->v=(
void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct
Cyc_Absyn_Uniondecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_unions_so_far))->v,(*_tmp454).f2,u));seen_defn_before=0;}
else{seen_defn_before=1;}}(void*)(u->sc=(void*)((void*)2));u->tvs=0;u->name=({
struct Cyc_Core_Opt*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->v=_tmp454;
_tmp457;});if(u->fields != 0){if(seen_defn_before){u->fields=0;}else{struct Cyc_List_List*
_tmp458=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(u->fields))->v;
for(0;_tmp458 != 0;_tmp458=_tmp458->tl){(void*)(((struct Cyc_Absyn_Structfield*)
_tmp458->hd)->type=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Absyn_Structfield*)
_tmp458->hd)->type));}}}}}static struct Cyc_Core_Opt*Cyc_Toc_tunions_so_far=0;
static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*tud){if(Cyc_Toc_tunions_so_far
== 0){Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*_tmp459=_cycalloc(sizeof(*
_tmp459));_tmp459->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _tuple0*,struct
_tuple0*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp459;});}{struct _tuple0*_tmp45A=
tud->name;if(tud->fields == 0){return;}if(((int(*)(struct Cyc_Set_Set*s,struct
_tuple0*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp45A)){return;}(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp45A));{int
tag_count=0;int box_count=0;struct Cyc_List_List*_tmp45B=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp45B != 0;_tmp45B=
_tmp45B->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)_tmp45B->hd;
if(f->typs == 0){;}else{struct Cyc_List_List*_tmp45C=0;int i=1;{struct Cyc_List_List*
_tmp45D=f->typs;for(0;_tmp45D != 0;(_tmp45D=_tmp45D->tl,i ++)){struct _tagged_arr*
_tmp45E=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Structfield*_tmp45F=({struct Cyc_Absyn_Structfield*
_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461->name=_tmp45E;_tmp461->tq=(*((struct
_tuple3*)_tmp45D->hd)).f1;_tmp461->type=(void*)Cyc_Toc_typ_to_c_array((*((struct
_tuple3*)_tmp45D->hd)).f2);_tmp461->width=0;_tmp461->attributes=0;_tmp461;});
_tmp45C=({struct Cyc_List_List*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->hd=
_tmp45F;_tmp460->tl=_tmp45C;_tmp460;});}}_tmp45C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp45C);_tmp45C=({struct Cyc_List_List*
_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->hd=({struct Cyc_Absyn_Structfield*
_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463->name=Cyc_Toc_tag_sp;_tmp463->tq=Cyc_Toc_mt_tq;
_tmp463->type=(void*)Cyc_Absyn_sint_t;_tmp463->width=0;_tmp463->attributes=0;
_tmp463;});_tmp462->tl=_tmp45C;_tmp462;});{struct Cyc_Absyn_Structdecl*_tmp464=({
struct Cyc_Absyn_Structdecl*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->sc=(void*)((
void*)2);_tmp468->name=({struct Cyc_Core_Opt*_tmp46A=_cycalloc(sizeof(*_tmp46A));
_tmp46A->v=Cyc_Toc_collapse_qvar_tag(f->name,_tag_arr("_struct",sizeof(
unsigned char),8));_tmp46A;});_tmp468->tvs=0;_tmp468->fields=({struct Cyc_Core_Opt*
_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469->v=_tmp45C;_tmp469;});_tmp468->attributes=
0;_tmp468;});Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp465=_cycalloc(
sizeof(*_tmp465));_tmp465->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Struct_d_struct*
_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466[0]=({struct Cyc_Absyn_Struct_d_struct
_tmp467;_tmp467.tag=4;_tmp467.f1=_tmp464;_tmp467;});_tmp466;}),0);_tmp465->tl=
Cyc_Toc_result_decls;_tmp465;});}}}}}}static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=
0;static void Cyc_Toc_xtuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far
== 0){Cyc_Toc_xtunion_fields_so_far=({struct Cyc_Core_Opt*_tmp46B=_cycalloc(
sizeof(*_tmp46B));_tmp46B->v=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,
struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp46B;});}if(xd->fields == 
0){return;}{struct _tuple0*_tmp46C=xd->name;struct Cyc_List_List*_tmp46D=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp46D != 
0;_tmp46D=_tmp46D->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp46D->hd;struct _tagged_arr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp46E=Cyc_Absyn_uint_exp(
_get_arr_size(*fn,sizeof(unsigned char))+ 4,0);struct Cyc_Absyn_ArrayType_struct*
_tmp46F=({struct Cyc_Absyn_ArrayType_struct*_tmp499=_cycalloc(sizeof(*_tmp499));
_tmp499[0]=({struct Cyc_Absyn_ArrayType_struct _tmp49A;_tmp49A.tag=7;_tmp49A.f1=(
void*)Cyc_Absyn_uchar_t;_tmp49A.f2=Cyc_Toc_mt_tq;_tmp49A.f3=(struct Cyc_Absyn_Exp*)
_tmp46E;_tmp49A;});_tmp499;});struct Cyc_Core_Opt*_tmp470=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,f->name);
struct Cyc_Core_Opt _tmp471;int _tmp472;_LL4C6: if(_tmp470 == 0){goto _LL4C7;}else{
goto _LL4C8;}_LL4C8: if(_tmp470 == 0){goto _LL4CA;}else{_tmp471=*_tmp470;_LL4CC:
_tmp472=(int)_tmp471.v;if(_tmp472 == 0){goto _LL4C9;}else{goto _LL4CA;}}_LL4CA: goto
_LL4CB;_LL4C7: {struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3){
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _tagged_arr)({struct
Cyc_Std_String_pa_struct _tmp478;_tmp478.tag=0;_tmp478.f1=(struct _tagged_arr)*fn;{
struct Cyc_Std_Int_pa_struct _tmp477;_tmp477.tag=1;_tmp477.f1=(int)((unsigned int)((
int)'\000'));{struct Cyc_Std_Int_pa_struct _tmp476;_tmp476.tag=1;_tmp476.f1=(int)((
unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct _tmp475;_tmp475.tag=1;
_tmp475.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp474;_tmp474.tag=1;_tmp474.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp473[5]={& _tmp474,& _tmp475,& _tmp476,& _tmp477,& _tmp478};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(unsigned char),11),_tag_arr(_tmp473,sizeof(void*),5));}}}}}}),
0);}{struct Cyc_Absyn_Vardecl*_tmp479=Cyc_Absyn_new_vardecl(f->name,(void*)
_tmp46F,initopt);(void*)(_tmp479->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B[
0]=({struct Cyc_Absyn_Var_d_struct _tmp47C;_tmp47C.tag=0;_tmp47C.f1=_tmp479;
_tmp47C;});_tmp47B;}),0);_tmp47A->tl=Cyc_Toc_result_decls;_tmp47A;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp47D=f->typs;for(0;_tmp47D != 0;(_tmp47D=_tmp47D->tl,
i ++)){struct _tagged_arr*_tmp47E=({struct _tagged_arr*_tmp482=_cycalloc(sizeof(*
_tmp482));_tmp482[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp484;
_tmp484.tag=1;_tmp484.f1=(int)((unsigned int)i);{void*_tmp483[1]={& _tmp484};Cyc_Std_aprintf(
_tag_arr("f%d",sizeof(unsigned char),4),_tag_arr(_tmp483,sizeof(void*),1));}});
_tmp482;});struct Cyc_Absyn_Structfield*_tmp47F=({struct Cyc_Absyn_Structfield*
_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->name=_tmp47E;_tmp481->tq=(*((struct
_tuple3*)_tmp47D->hd)).f1;_tmp481->type=(void*)Cyc_Toc_typ_to_c_array((*((struct
_tuple3*)_tmp47D->hd)).f2);_tmp481->width=0;_tmp481->attributes=0;_tmp481;});
fields=({struct Cyc_List_List*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->hd=
_tmp47F;_tmp480->tl=fields;_tmp480;});}}fields=({struct Cyc_List_List*_tmp485=
_cycalloc(sizeof(*_tmp485));_tmp485->hd=({struct Cyc_Absyn_Structfield*_tmp486=
_cycalloc(sizeof(*_tmp486));_tmp486->name=Cyc_Toc_tag_sp;_tmp486->tq=Cyc_Toc_mt_tq;
_tmp486->type=(void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_t,Cyc_Toc_mt_tq);
_tmp486->width=0;_tmp486->attributes=0;_tmp486;});_tmp485->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp485;});{struct Cyc_Absyn_Structdecl*
_tmp487=({struct Cyc_Absyn_Structdecl*_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B->sc=(
void*)((void*)2);_tmp48B->name=({struct Cyc_Core_Opt*_tmp48D=_cycalloc(sizeof(*
_tmp48D));_tmp48D->v=Cyc_Toc_collapse_qvar_tag(f->name,_tag_arr("_struct",
sizeof(unsigned char),8));_tmp48D;});_tmp48B->tvs=0;_tmp48B->fields=({struct Cyc_Core_Opt*
_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->v=fields;_tmp48C;});_tmp48B->attributes=
0;_tmp48B;});Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp488=_cycalloc(
sizeof(*_tmp488));_tmp488->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Struct_d_struct*
_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489[0]=({struct Cyc_Absyn_Struct_d_struct
_tmp48A;_tmp48A.tag=4;_tmp48A.f1=_tmp487;_tmp48A;});_tmp489;}),0);_tmp488->tl=
Cyc_Toc_result_decls;_tmp488;});}}goto _LL4C5;}}_LL4C9: if((void*)f->sc != (void*)3){
struct Cyc_Absyn_Exp*_tmp48E=Cyc_Absyn_string_exp((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp498;_tmp498.tag=0;_tmp498.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_Int_pa_struct
_tmp497;_tmp497.tag=1;_tmp497.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp496;_tmp496.tag=1;_tmp496.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp495;_tmp495.tag=1;_tmp495.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp494;_tmp494.tag=1;_tmp494.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp493[5]={& _tmp494,& _tmp495,& _tmp496,& _tmp497,& _tmp498};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(unsigned char),11),_tag_arr(_tmp493,sizeof(void*),5));}}}}}}),
0);struct Cyc_Absyn_Vardecl*_tmp48F=Cyc_Absyn_new_vardecl(f->name,(void*)_tmp46F,(
struct Cyc_Absyn_Exp*)_tmp48E);(void*)(_tmp48F->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491[
0]=({struct Cyc_Absyn_Var_d_struct _tmp492;_tmp492.tag=0;_tmp492.f1=_tmp48F;
_tmp492;});_tmp491;}),0);_tmp490->tl=Cyc_Toc_result_decls;_tmp490;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL4C5;_LL4CB: goto _LL4C5;_LL4C5:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){struct _tuple0*_tmp49B=ed->name;(
void*)(ed->sc=(void*)((void*)2));if(ed->fields != 0){Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}}static
void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array((
void*)vd->type));Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*
init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);void*_tmp49C=(void*)init->r;
struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Vardecl*
_tmp49F;_LL4CE: if(*((int*)_tmp49C)== 27){_LL4D4: _tmp49F=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp49C)->f1;goto _LL4D3;_LL4D3: _tmp49E=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp49C)->f2;goto _LL4D2;_LL4D2: _tmp49D=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp49C)->f3;goto _LL4CF;}else{goto _LL4D0;}_LL4D0: goto _LL4D1;_LL4CF: vd->initializer=
0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(
vd->name,0),_tmp49F,_tmp49E,_tmp49D,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL4CD;_LL4D1: if((void*)vd->sc == (void*)0){struct Cyc_Toc_Env*_tmp4A0=Cyc_Toc_copy_env(
init_nv);_tmp4A0->toplevel=1;Cyc_Toc_exp_to_c(_tmp4A0,init);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL4CD;_LL4CD:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,struct Cyc_Core_Opt*topt,struct Cyc_Absyn_Exp*
e,int exhaust,struct Cyc_Absyn_Stmt*s){void*t=(void*)((struct Cyc_Core_Opt*)
_check_null(topt))->v;Cyc_Toc_exp_to_c(nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();
struct _tagged_arr*succ_lab=Cyc_Toc_fresh_label();struct _tagged_arr*fail_lab=
exhaust? succ_lab: Cyc_Toc_fresh_label();{struct _RegionHandle _tmp4A1=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp4A1;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp4A3;struct Cyc_List_List*_tmp4A4;struct Cyc_Toc_Env*_tmp4A5;struct _tuple11
_tmp4A2=Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),
p,succ_lab,fail_lab,0);_LL4D8: _tmp4A5=_tmp4A2.f1;goto _LL4D7;_LL4D7: _tmp4A4=
_tmp4A2.f2;goto _LL4D6;_LL4D6: _tmp4A3=_tmp4A2.f3;goto _LL4D5;_LL4D5: Cyc_Toc_stmt_to_c(
_tmp4A5,s);{struct Cyc_Absyn_Stmt*_tmp4A6=Cyc_Absyn_label_stmt(succ_lab,s,0);if(
exhaust){s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)e,
Cyc_Absyn_seq_stmt(_tmp4A3,_tmp4A6,0),0);}else{struct Cyc_Absyn_Exp*_tmp4A7=Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0));struct Cyc_Absyn_Stmt*_tmp4A8=Cyc_Absyn_label_stmt(
fail_lab,Cyc_Absyn_exp_stmt(_tmp4A7,0),0);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp4A3,Cyc_Absyn_seq_stmt(_tmp4A8,
_tmp4A6,0),0),0);}for(0;_tmp4A4 != 0;_tmp4A4=_tmp4A4->tl){struct _tuple12 _tmp4AA;
void*_tmp4AB;struct _tuple0*_tmp4AC;struct _tuple12*_tmp4A9=(struct _tuple12*)
_tmp4A4->hd;_tmp4AA=*_tmp4A9;_LL4DB: _tmp4AC=_tmp4AA.f1;goto _LL4DA;_LL4DA: _tmp4AB=
_tmp4AA.f2;goto _LL4D9;_LL4D9: s=Cyc_Absyn_declare_stmt(_tmp4AC,_tmp4AB,0,s,0);}}};
_pop_region(prgn);}return s;}}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top){for(0;ds != 0;ds=ds->tl){if(! nv->toplevel){({
void*_tmp4AD[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("decls_to_c: not at toplevel!",sizeof(unsigned char),29),_tag_arr(
_tmp4AD,sizeof(void*),0));});}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp4AE=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp4AF;struct Cyc_Absyn_Fndecl*
_tmp4B0;struct Cyc_Absyn_Structdecl*_tmp4B1;struct Cyc_Absyn_Uniondecl*_tmp4B2;
struct Cyc_Absyn_Tuniondecl*_tmp4B3;struct Cyc_Absyn_Enumdecl*_tmp4B4;struct Cyc_Absyn_Typedefdecl*
_tmp4B5;struct Cyc_List_List*_tmp4B6;struct Cyc_List_List*_tmp4B7;struct Cyc_List_List*
_tmp4B8;_LL4DD: if(*((int*)_tmp4AE)== 0){_LL4F5: _tmp4AF=((struct Cyc_Absyn_Var_d_struct*)
_tmp4AE)->f1;goto _LL4DE;}else{goto _LL4DF;}_LL4DF: if(*((int*)_tmp4AE)== 1){_LL4F6:
_tmp4B0=((struct Cyc_Absyn_Fn_d_struct*)_tmp4AE)->f1;goto _LL4E0;}else{goto _LL4E1;}
_LL4E1: if(*((int*)_tmp4AE)== 2){goto _LL4E2;}else{goto _LL4E3;}_LL4E3: if(*((int*)
_tmp4AE)== 3){goto _LL4E4;}else{goto _LL4E5;}_LL4E5: if(*((int*)_tmp4AE)== 4){
_LL4F7: _tmp4B1=((struct Cyc_Absyn_Struct_d_struct*)_tmp4AE)->f1;goto _LL4E6;}else{
goto _LL4E7;}_LL4E7: if(*((int*)_tmp4AE)== 5){_LL4F8: _tmp4B2=((struct Cyc_Absyn_Union_d_struct*)
_tmp4AE)->f1;goto _LL4E8;}else{goto _LL4E9;}_LL4E9: if(*((int*)_tmp4AE)== 6){_LL4F9:
_tmp4B3=((struct Cyc_Absyn_Tunion_d_struct*)_tmp4AE)->f1;goto _LL4EA;}else{goto
_LL4EB;}_LL4EB: if(*((int*)_tmp4AE)== 7){_LL4FA: _tmp4B4=((struct Cyc_Absyn_Enum_d_struct*)
_tmp4AE)->f1;goto _LL4EC;}else{goto _LL4ED;}_LL4ED: if(*((int*)_tmp4AE)== 8){_LL4FB:
_tmp4B5=((struct Cyc_Absyn_Typedef_d_struct*)_tmp4AE)->f1;goto _LL4EE;}else{goto
_LL4EF;}_LL4EF: if(*((int*)_tmp4AE)== 9){_LL4FC: _tmp4B6=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp4AE)->f2;goto _LL4F0;}else{goto _LL4F1;}_LL4F1: if(*((int*)_tmp4AE)== 10){
_LL4FD: _tmp4B7=((struct Cyc_Absyn_Using_d_struct*)_tmp4AE)->f2;goto _LL4F2;}else{
goto _LL4F3;}_LL4F3: if(*((int*)_tmp4AE)== 11){_LL4FE: _tmp4B8=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp4AE)->f1;goto _LL4F4;}else{goto _LL4DC;}_LL4DE: {struct _tuple0*_tmp4B9=_tmp4AF->name;
if((void*)_tmp4AF->sc == (void*)4){_tmp4B9=({struct _tuple0*_tmp4BA=_cycalloc(
sizeof(*_tmp4BA));_tmp4BA->f1=Cyc_Absyn_rel_ns_null;_tmp4BA->f2=(*_tmp4B9).f2;
_tmp4BA;});}if(_tmp4AF->initializer != 0){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_check_null(_tmp4AF->initializer));}nv=Cyc_Toc_add_varmap(nv,_tmp4AF->name,Cyc_Absyn_varb_exp(
_tmp4B9,(void*)({struct Cyc_Absyn_Global_b_struct*_tmp4BB=_cycalloc(sizeof(*
_tmp4BB));_tmp4BB[0]=({struct Cyc_Absyn_Global_b_struct _tmp4BC;_tmp4BC.tag=0;
_tmp4BC.f1=_tmp4AF;_tmp4BC;});_tmp4BB;}),0));_tmp4AF->name=_tmp4B9;(void*)(
_tmp4AF->sc=(void*)Cyc_Toc_scope_to_c((void*)_tmp4AF->sc));(void*)(_tmp4AF->type=(
void*)Cyc_Toc_typ_to_c_array((void*)_tmp4AF->type));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->hd=d;_tmp4BD->tl=
Cyc_Toc_result_decls;_tmp4BD;});goto _LL4DC;}_LL4E0: {struct _tuple0*_tmp4BE=
_tmp4B0->name;if((void*)_tmp4B0->sc == (void*)4){_tmp4BE=({struct _tuple0*_tmp4BF=
_cycalloc(sizeof(*_tmp4BF));_tmp4BF->f1=Cyc_Absyn_rel_ns_null;_tmp4BF->f2=(*
_tmp4BE).f2;_tmp4BF;});}nv=Cyc_Toc_add_varmap(nv,_tmp4B0->name,Cyc_Absyn_var_exp(
_tmp4BE,0));_tmp4B0->name=_tmp4BE;Cyc_Toc_fndecl_to_c(nv,_tmp4B0);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->hd=d;_tmp4C0->tl=
Cyc_Toc_result_decls;_tmp4C0;});goto _LL4DC;}_LL4E2: goto _LL4E4;_LL4E4:({void*
_tmp4C1[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("letdecl at toplevel",sizeof(unsigned char),20),_tag_arr(_tmp4C1,
sizeof(void*),0));});_LL4E6: Cyc_Toc_structdecl_to_c(_tmp4B1);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));_tmp4C2->hd=d;_tmp4C2->tl=
Cyc_Toc_result_decls;_tmp4C2;});goto _LL4DC;_LL4E8: Cyc_Toc_uniondecl_to_c(_tmp4B2);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));
_tmp4C3->hd=d;_tmp4C3->tl=Cyc_Toc_result_decls;_tmp4C3;});goto _LL4DC;_LL4EA: if(
_tmp4B3->is_xtunion){Cyc_Toc_xtuniondecl_to_c(_tmp4B3);}else{Cyc_Toc_tuniondecl_to_c(
_tmp4B3);}goto _LL4DC;_LL4EC: Cyc_Toc_enumdecl_to_c(nv,_tmp4B4);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));_tmp4C4->hd=d;_tmp4C4->tl=
Cyc_Toc_result_decls;_tmp4C4;});goto _LL4DC;_LL4EE: _tmp4B5->name=_tmp4B5->name;
_tmp4B5->tvs=0;(void*)(_tmp4B5->defn=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp4B5->defn));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4C5=_cycalloc(
sizeof(*_tmp4C5));_tmp4C5->hd=d;_tmp4C5->tl=Cyc_Toc_result_decls;_tmp4C5;});goto
_LL4DC;_LL4F0: _tmp4B7=_tmp4B6;goto _LL4F2;_LL4F2: _tmp4B8=_tmp4B7;goto _LL4F4;
_LL4F4: nv=Cyc_Toc_decls_to_c(nv,_tmp4B8,top);goto _LL4DC;_LL4DC:;}}return nv;}
static void Cyc_Toc_init(){Cyc_Toc_result_decls=0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=
0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_structs_so_far=
0;Cyc_Toc_tunions_so_far=0;Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(Cyc_Toc_empty_env(),ds,1);return((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
