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
 struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Cstdio___abstractFILE;struct
Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;int Cyc_Std_fflush(
struct Cyc_Std___cycFILE*);extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_fprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);int Cyc_Std_printf(
struct _tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_vrprintf(struct
_RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_list(struct _tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*,struct _tagged_arr);extern char Cyc_Position_Nocontext[
14];extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple1{
void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
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
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct
Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{
int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct Cyc_Absyn_NonNullable_ps_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Nullable_ps_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{
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
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;struct
Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[
15];int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_t;extern
void*Cyc_Absyn_ulonglong_t;extern void*Cyc_Absyn_sint_t;extern void*Cyc_Absyn_slonglong_t;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple1*Cyc_Absyn_tunion_scanf_arg_qvar;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b);struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(
struct Cyc_List_List*,struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct _tuple4{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple4*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
struct _tagged_arr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*
a);struct _tuple5{void*f1;struct _tuple1*f2;};struct _tuple5 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct _tuple6{void*f1;void*f2;};struct _tuple6*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple6*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Tcenv_VarRes_struct{int tag;void*
f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*
f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*
aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct
Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*
availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);int Cyc_Std_strcmp(struct _tagged_arr s1,
struct _tagged_arr s2);int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);
struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_flush_warnings();extern
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*t);
int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int
Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*
Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();int
Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int set_to_empty,void*e1,
void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple7{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple7*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
struct _tuple8{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple7*Cyc_Tcutil_r_make_inst_var(
struct _tuple8*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_tagged_arr*fn);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);void
Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr err_msg);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,
unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*
b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);struct _tuple9{int f1;void*f2;};struct _tuple9 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int*Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Tcutil_bits_only(
void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_snd_tqt(struct _tuple4*);int Cyc_Tcutil_supports_default(void*);
int Cyc_Tcutil_is_noreturn(void*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify";
void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;
void*Cyc_Tcutil_t2_failure=(void*)0;struct _tagged_arr Cyc_Tcutil_failure_reason=(
struct _tagged_arr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_Std_fflush((
struct Cyc_Std___cycFILE*)Cyc_Std_stderr);{struct _tagged_arr s1=Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure);struct _tagged_arr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=8;({struct Cyc_Std_String_pa_struct _tmp1;_tmp1.tag=0;_tmp1.f1=(struct
_tagged_arr)s1;{void*_tmp0[1]={& _tmp1};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\t%s and ",
sizeof(char),9),_tag_arr(_tmp0,sizeof(void*),1));}});pos +=_get_arr_size(s1,
sizeof(char))+ 5;if(pos >= 80){({void*_tmp2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n\t",sizeof(char),3),_tag_arr(_tmp2,sizeof(void*),0));});pos=8;}({
struct Cyc_Std_String_pa_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _tagged_arr)s2;{
void*_tmp3[1]={& _tmp4};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s ",sizeof(char),
4),_tag_arr(_tmp3,sizeof(void*),1));}});pos +=_get_arr_size(s2,sizeof(char))+ 1;
if(pos >= 80){({void*_tmp5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n\t",
sizeof(char),3),_tag_arr(_tmp5,sizeof(void*),0));});pos=8;}({void*_tmp6[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("failed to unify. ",sizeof(char),18),
_tag_arr(_tmp6,sizeof(void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr != ((
struct _tagged_arr)_tag_arr(0,0,0)).curr){if(pos >= 80){({void*_tmp7[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\n\t",sizeof(char),3),_tag_arr(_tmp7,sizeof(void*),0));});
pos=8;}({struct Cyc_Std_String_pa_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(struct
_tagged_arr)Cyc_Tcutil_failure_reason;{void*_tmp8[1]={& _tmp9};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp8,sizeof(void*),1));}});}({
void*_tmpA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n",sizeof(char),2),
_tag_arr(_tmpA,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);}}
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*loc,struct _tagged_arr fmt,struct
_tagged_arr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct
_tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap){struct _tagged_arr msg=(struct
_tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt,ap);({struct Cyc_Std_String_pa_struct
_tmpC;_tmpC.tag=0;_tmpC.f1=(struct _tagged_arr)msg;{void*_tmpB[1]={& _tmpC};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Compiler Error (Tcutil::impos): %s\n",sizeof(char),36),
_tag_arr(_tmpB,sizeof(void*),1));}});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)
Cyc_Std_stderr);(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpD=
_cycalloc(sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Core_Impossible_struct _tmpE;_tmpE.tag=
Cyc_Core_Impossible;_tmpE.f1=msg;_tmpE;});_tmpD;}));}static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){({struct Cyc_Std_String_pa_struct _tmp11;_tmp11.tag=
0;_tmp11.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);{struct Cyc_Std_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(
struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd);{void*
_tmpF[2]={& _tmp10,& _tmp11};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::%s ",
sizeof(char),8),_tag_arr(_tmpF,sizeof(void*),2));}}});}({void*_tmp12[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\n",sizeof(char),2),_tag_arr(_tmp12,sizeof(void*),0));});
Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);}static struct Cyc_List_List*
Cyc_Tcutil_warning_segs=0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
void Cyc_Tcutil_warn(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap){struct _tagged_arr msg=(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,
fmt,ap);Cyc_Tcutil_warning_segs=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*
_tmp13));_tmp13->hd=sg;_tmp13->tl=Cyc_Tcutil_warning_segs;_tmp13;});Cyc_Tcutil_warning_msgs=({
struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->hd=({struct
_tagged_arr*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=msg;_tmp15;});_tmp14->tl=
Cyc_Tcutil_warning_msgs;_tmp14;});}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs
== 0)return;({void*_tmp16[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("***Warnings***\n",
sizeof(char),16),_tag_arr(_tmp16,sizeof(void*),0));});{struct Cyc_List_List*
_tmp17=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){({struct Cyc_Std_String_pa_struct
_tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _tagged_arr)*((struct _tagged_arr*)((struct
Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd);{struct Cyc_Std_String_pa_struct
_tmp19;_tmp19.tag=0;_tmp19.f1=(struct _tagged_arr)*((struct _tagged_arr*)((struct
Cyc_List_List*)_check_null(_tmp17))->hd);{void*_tmp18[2]={& _tmp19,& _tmp1A};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s: %s\n",sizeof(char),8),_tag_arr(_tmp18,sizeof(void*),
2));}}});_tmp17=_tmp17->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}({void*_tmp1B[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("**************\n",sizeof(char),16),_tag_arr(_tmp1B,
sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);}}
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;static int Cyc_Tcutil_fast_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){return*((int*)_check_null(tv1->identity))
- *((int*)_check_null(tv2->identity));}void*Cyc_Tcutil_compress(void*t){void*
_tmp1C=t;struct Cyc_Core_Opt*_tmp1D;struct Cyc_Core_Opt*_tmp1E;struct Cyc_Core_Opt*
_tmp1F;struct Cyc_Core_Opt**_tmp20;struct Cyc_Core_Opt*_tmp21;struct Cyc_Core_Opt**
_tmp22;_LL1: if(_tmp1C <= (void*)3?1:*((int*)_tmp1C)!= 0)goto _LL3;_tmp1D=((struct
Cyc_Absyn_Evar_struct*)_tmp1C)->f2;if(_tmp1D != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(
_tmp1C <= (void*)3?1:*((int*)_tmp1C)!= 16)goto _LL5;_tmp1E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1C)->f4;if(_tmp1E != 0)goto _LL5;_LL4: return t;_LL5: if(_tmp1C <= (void*)3?1:*((
int*)_tmp1C)!= 16)goto _LL7;_tmp1F=((struct Cyc_Absyn_TypedefType_struct*)_tmp1C)->f4;
_tmp20=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp1C)->f4;
_LL6: _tmp22=_tmp20;goto _LL8;_LL7: if(_tmp1C <= (void*)3?1:*((int*)_tmp1C)!= 0)goto
_LL9;_tmp21=((struct Cyc_Absyn_Evar_struct*)_tmp1C)->f2;_tmp22=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1C)->f2;_LL8: {void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp22))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp22))->v)*_tmp22=({struct Cyc_Core_Opt*_tmp23=_cycalloc(sizeof(*
_tmp23));_tmp23->v=(void*)t2;_tmp23;});return t2;}_LL9:;_LLA: return t;_LL0:;}void*
Cyc_Tcutil_copy_type(void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(
struct Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){void*
_tmp24=(void*)c->v;void*_tmp25;struct Cyc_Absyn_Conref*_tmp26;_LLC: if((int)_tmp24
!= 0)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if(_tmp24 <= (void*)1?1:*((
int*)_tmp24)!= 0)goto _LL10;_tmp25=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp24)->f1;_LLF: return Cyc_Absyn_new_conref(_tmp25);_LL10: if(_tmp24 <= (void*)1?1:*((
int*)_tmp24)!= 1)goto _LLB;_tmp26=((struct Cyc_Absyn_Forward_constr_struct*)_tmp24)->f1;
_LL11: return Cyc_Tcutil_copy_conref(_tmp26);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp27=Cyc_Absyn_compress_kb(kb);void*_tmp28;void*_tmp29;_LL13: if(*((
int*)_tmp27)!= 0)goto _LL15;_tmp28=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp27)->f1;
_LL14: return(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp2A=_cycalloc(sizeof(*
_tmp2A));_tmp2A[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp2B;_tmp2B.tag=0;_tmp2B.f1=(
void*)_tmp28;_tmp2B;});_tmp2A;});_LL15: if(*((int*)_tmp27)!= 1)goto _LL17;_LL16:
return(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));
_tmp2C[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp2D;_tmp2D.tag=1;_tmp2D.f1=0;
_tmp2D;});_tmp2C;});_LL17: if(*((int*)_tmp27)!= 2)goto _LL12;_tmp29=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp27)->f2;_LL18: return(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp2F;_tmp2F.tag=2;_tmp2F.f1=0;_tmp2F.f2=(void*)_tmp29;_tmp2F;});_tmp2E;});
_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->name=tv->name;
_tmp30->identity=0;_tmp30->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);
_tmp30;});}static struct _tuple2*Cyc_Tcutil_copy_arg(struct _tuple2*arg){struct
_tuple2 _tmp32;struct Cyc_Core_Opt*_tmp33;struct Cyc_Absyn_Tqual _tmp34;void*_tmp35;
struct _tuple2*_tmp31=arg;_tmp32=*_tmp31;_tmp33=_tmp32.f1;_tmp34=_tmp32.f2;_tmp35=
_tmp32.f3;return({struct _tuple2*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->f1=
_tmp33;_tmp36->f2=_tmp34;_tmp36->f3=Cyc_Tcutil_copy_type(_tmp35);_tmp36;});}
static struct _tuple4*Cyc_Tcutil_copy_tqt(struct _tuple4*arg){struct _tuple4 _tmp38;
struct Cyc_Absyn_Tqual _tmp39;void*_tmp3A;struct _tuple4*_tmp37=arg;_tmp38=*_tmp37;
_tmp39=_tmp38.f1;_tmp3A=_tmp38.f2;return({struct _tuple4*_tmp3B=_cycalloc(sizeof(*
_tmp3B));_tmp3B->f1=_tmp39;_tmp3B->f2=Cyc_Tcutil_copy_type(_tmp3A);_tmp3B;});}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->name=
f->name;_tmp3C->tq=f->tq;_tmp3C->type=(void*)Cyc_Tcutil_copy_type((void*)f->type);
_tmp3C->width=f->width;_tmp3C->attributes=f->attributes;_tmp3C;});}static struct
_tuple6*Cyc_Tcutil_copy_rgncmp(struct _tuple6*x){struct _tuple6 _tmp3E;void*_tmp3F;
void*_tmp40;struct _tuple6*_tmp3D=x;_tmp3E=*_tmp3D;_tmp3F=_tmp3E.f1;_tmp40=_tmp3E.f2;
return({struct _tuple6*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->f1=Cyc_Tcutil_copy_type(
_tmp3F);_tmp41->f2=Cyc_Tcutil_copy_type(_tmp40);_tmp41;});}static struct Cyc_Absyn_Enumfield*
Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*
_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->name=f->name;_tmp42->tag=f->tag;_tmp42->loc=
f->loc;_tmp42;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp43=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp44;struct Cyc_Absyn_TunionInfo _tmp45;void*_tmp46;
struct Cyc_List_List*_tmp47;void*_tmp48;struct Cyc_Absyn_TunionFieldInfo _tmp49;
void*_tmp4A;struct Cyc_List_List*_tmp4B;struct Cyc_Absyn_PtrInfo _tmp4C;void*_tmp4D;
void*_tmp4E;struct Cyc_Absyn_Conref*_tmp4F;struct Cyc_Absyn_Tqual _tmp50;struct Cyc_Absyn_Conref*
_tmp51;void*_tmp52;void*_tmp53;int _tmp54;void*_tmp55;struct Cyc_Absyn_Tqual _tmp56;
struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_FnInfo _tmp58;struct Cyc_List_List*
_tmp59;struct Cyc_Core_Opt*_tmp5A;void*_tmp5B;struct Cyc_List_List*_tmp5C;int
_tmp5D;struct Cyc_Absyn_VarargInfo*_tmp5E;struct Cyc_List_List*_tmp5F;struct Cyc_List_List*
_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_Absyn_AggrInfo _tmp62;void*_tmp63;
void*_tmp64;struct _tuple1*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_Absyn_AggrInfo
_tmp67;void*_tmp68;struct Cyc_Absyn_Aggrdecl**_tmp69;struct Cyc_List_List*_tmp6A;
void*_tmp6B;struct Cyc_List_List*_tmp6C;struct _tuple1*_tmp6D;struct Cyc_Absyn_Enumdecl*
_tmp6E;struct Cyc_List_List*_tmp6F;void*_tmp70;void*_tmp71;struct _tuple1*_tmp72;
struct Cyc_List_List*_tmp73;struct Cyc_Absyn_Typedefdecl*_tmp74;void*_tmp75;struct
Cyc_List_List*_tmp76;void*_tmp77;_LL1A: if((int)_tmp43 != 0)goto _LL1C;_LL1B: goto
_LL1D;_LL1C: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 0)goto _LL1E;_LL1D: return t;
_LL1E: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 1)goto _LL20;_tmp44=((struct Cyc_Absyn_VarType_struct*)
_tmp43)->f1;_LL1F: return(void*)({struct Cyc_Absyn_VarType_struct*_tmp78=_cycalloc(
sizeof(*_tmp78));_tmp78[0]=({struct Cyc_Absyn_VarType_struct _tmp79;_tmp79.tag=1;
_tmp79.f1=Cyc_Tcutil_copy_tvar(_tmp44);_tmp79;});_tmp78;});_LL20: if(_tmp43 <= (
void*)3?1:*((int*)_tmp43)!= 2)goto _LL22;_tmp45=((struct Cyc_Absyn_TunionType_struct*)
_tmp43)->f1;_tmp46=(void*)_tmp45.tunion_info;_tmp47=_tmp45.targs;_tmp48=(void*)
_tmp45.rgn;_LL21: return(void*)({struct Cyc_Absyn_TunionType_struct*_tmp7A=
_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({struct Cyc_Absyn_TunionType_struct _tmp7B;
_tmp7B.tag=2;_tmp7B.f1=({struct Cyc_Absyn_TunionInfo _tmp7C;_tmp7C.tunion_info=(
void*)_tmp46;_tmp7C.targs=Cyc_Tcutil_copy_types(_tmp47);_tmp7C.rgn=(void*)Cyc_Tcutil_copy_type(
_tmp48);_tmp7C;});_tmp7B;});_tmp7A;});_LL22: if(_tmp43 <= (void*)3?1:*((int*)
_tmp43)!= 3)goto _LL24;_tmp49=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp43)->f1;
_tmp4A=(void*)_tmp49.field_info;_tmp4B=_tmp49.targs;_LL23: return(void*)({struct
Cyc_Absyn_TunionFieldType_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({
struct Cyc_Absyn_TunionFieldType_struct _tmp7E;_tmp7E.tag=3;_tmp7E.f1=({struct Cyc_Absyn_TunionFieldInfo
_tmp7F;_tmp7F.field_info=(void*)_tmp4A;_tmp7F.targs=Cyc_Tcutil_copy_types(_tmp4B);
_tmp7F;});_tmp7E;});_tmp7D;});_LL24: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 4)
goto _LL26;_tmp4C=((struct Cyc_Absyn_PointerType_struct*)_tmp43)->f1;_tmp4D=(void*)
_tmp4C.elt_typ;_tmp4E=(void*)_tmp4C.rgn_typ;_tmp4F=_tmp4C.nullable;_tmp50=_tmp4C.tq;
_tmp51=_tmp4C.bounds;_LL25: {void*_tmp80=Cyc_Tcutil_copy_type(_tmp4D);void*
_tmp81=Cyc_Tcutil_copy_type(_tmp4E);struct Cyc_Absyn_Conref*_tmp82=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp4F);struct Cyc_Absyn_Tqual
_tmp83=_tmp50;struct Cyc_Absyn_Conref*_tmp84=Cyc_Tcutil_copy_conref(_tmp51);
return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp85=_cycalloc(sizeof(*
_tmp85));_tmp85[0]=({struct Cyc_Absyn_PointerType_struct _tmp86;_tmp86.tag=4;
_tmp86.f1=({struct Cyc_Absyn_PtrInfo _tmp87;_tmp87.elt_typ=(void*)_tmp80;_tmp87.rgn_typ=(
void*)_tmp81;_tmp87.nullable=_tmp82;_tmp87.tq=_tmp83;_tmp87.bounds=_tmp84;_tmp87;});
_tmp86;});_tmp85;});}_LL26: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 5)goto _LL28;
_tmp52=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp43)->f1;_tmp53=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp43)->f2;_LL27: return(void*)({struct Cyc_Absyn_IntType_struct*
_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88[0]=({struct Cyc_Absyn_IntType_struct
_tmp89;_tmp89.tag=5;_tmp89.f1=(void*)_tmp52;_tmp89.f2=(void*)_tmp53;_tmp89;});
_tmp88;});_LL28: if((int)_tmp43 != 1)goto _LL2A;_LL29: return t;_LL2A: if(_tmp43 <= (
void*)3?1:*((int*)_tmp43)!= 6)goto _LL2C;_tmp54=((struct Cyc_Absyn_DoubleType_struct*)
_tmp43)->f1;_LL2B: return(void*)({struct Cyc_Absyn_DoubleType_struct*_tmp8A=
_cycalloc_atomic(sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_Absyn_DoubleType_struct
_tmp8B;_tmp8B.tag=6;_tmp8B.f1=_tmp54;_tmp8B;});_tmp8A;});_LL2C: if(_tmp43 <= (void*)
3?1:*((int*)_tmp43)!= 7)goto _LL2E;_tmp55=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp43)->f1;_tmp56=((struct Cyc_Absyn_ArrayType_struct*)_tmp43)->f2;_tmp57=((
struct Cyc_Absyn_ArrayType_struct*)_tmp43)->f3;_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp8D;_tmp8D.tag=7;_tmp8D.f1=(void*)Cyc_Tcutil_copy_type(_tmp55);_tmp8D.f2=
_tmp56;_tmp8D.f3=_tmp57;_tmp8D;});_tmp8C;});_LL2E: if(_tmp43 <= (void*)3?1:*((int*)
_tmp43)!= 8)goto _LL30;_tmp58=((struct Cyc_Absyn_FnType_struct*)_tmp43)->f1;_tmp59=
_tmp58.tvars;_tmp5A=_tmp58.effect;_tmp5B=(void*)_tmp58.ret_typ;_tmp5C=_tmp58.args;
_tmp5D=_tmp58.c_varargs;_tmp5E=_tmp58.cyc_varargs;_tmp5F=_tmp58.rgn_po;_tmp60=
_tmp58.attributes;_LL2F: {struct Cyc_List_List*_tmp8E=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp59);struct Cyc_Core_Opt*_tmp8F=_tmp5A == 0?0:({struct Cyc_Core_Opt*
_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->v=(void*)Cyc_Tcutil_copy_type((void*)
_tmp5A->v);_tmp99;});void*_tmp90=Cyc_Tcutil_copy_type(_tmp5B);struct Cyc_List_List*
_tmp91=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp5C);int _tmp92=_tmp5D;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp5E != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp5E);cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp93=_cycalloc(
sizeof(*_tmp93));_tmp93->name=cv->name;_tmp93->tq=cv->tq;_tmp93->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmp93->inject=cv->inject;_tmp93;});}{struct Cyc_List_List*_tmp94=((
struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp5F);struct Cyc_List_List*_tmp95=_tmp60;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmp96=_cycalloc(sizeof(*_tmp96));
_tmp96[0]=({struct Cyc_Absyn_FnType_struct _tmp97;_tmp97.tag=8;_tmp97.f1=({struct
Cyc_Absyn_FnInfo _tmp98;_tmp98.tvars=_tmp8E;_tmp98.effect=_tmp8F;_tmp98.ret_typ=(
void*)_tmp90;_tmp98.args=_tmp91;_tmp98.c_varargs=_tmp92;_tmp98.cyc_varargs=
cyc_varargs2;_tmp98.rgn_po=_tmp94;_tmp98.attributes=_tmp95;_tmp98;});_tmp97;});
_tmp96;});}}_LL30: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 9)goto _LL32;_tmp61=((
struct Cyc_Absyn_TupleType_struct*)_tmp43)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_TupleType_struct
_tmp9B;_tmp9B.tag=9;_tmp9B.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(
struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp61);
_tmp9B;});_tmp9A;});_LL32: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 10)goto _LL34;
_tmp62=((struct Cyc_Absyn_AggrType_struct*)_tmp43)->f1;_tmp63=(void*)_tmp62.aggr_info;
if(*((int*)_tmp63)!= 0)goto _LL34;_tmp64=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp63)->f1;_tmp65=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp63)->f2;_tmp66=
_tmp62.targs;_LL33: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp9C=
_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Absyn_AggrType_struct _tmp9D;
_tmp9D.tag=10;_tmp9D.f1=({struct Cyc_Absyn_AggrInfo _tmp9E;_tmp9E.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));
_tmp9F[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmpA0;_tmpA0.tag=0;_tmpA0.f1=(
void*)_tmp64;_tmpA0.f2=_tmp65;_tmpA0;});_tmp9F;}));_tmp9E.targs=Cyc_Tcutil_copy_types(
_tmp66);_tmp9E;});_tmp9D;});_tmp9C;});_LL34: if(_tmp43 <= (void*)3?1:*((int*)
_tmp43)!= 10)goto _LL36;_tmp67=((struct Cyc_Absyn_AggrType_struct*)_tmp43)->f1;
_tmp68=(void*)_tmp67.aggr_info;if(*((int*)_tmp68)!= 1)goto _LL36;_tmp69=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp68)->f1;_tmp6A=_tmp67.targs;_LL35: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({
struct Cyc_Absyn_AggrType_struct _tmpA2;_tmpA2.tag=10;_tmpA2.f1=({struct Cyc_Absyn_AggrInfo
_tmpA3;_tmpA3.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpA4=
_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpA5;
_tmpA5.tag=1;_tmpA5.f1=_tmp69;_tmpA5;});_tmpA4;}));_tmpA3.targs=Cyc_Tcutil_copy_types(
_tmp6A);_tmpA3;});_tmpA2;});_tmpA1;});_LL36: if(_tmp43 <= (void*)3?1:*((int*)
_tmp43)!= 11)goto _LL38;_tmp6B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp43)->f1;_tmp6C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp43)->f2;_LL37:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpA6=_cycalloc(sizeof(*
_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpA7;_tmpA7.tag=11;
_tmpA7.f1=(void*)_tmp6B;_tmpA7.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp6C);_tmpA7;});_tmpA6;});_LL38: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 12)
goto _LL3A;_tmp6D=((struct Cyc_Absyn_EnumType_struct*)_tmp43)->f1;_tmp6E=((struct
Cyc_Absyn_EnumType_struct*)_tmp43)->f2;_LL39: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_EnumType_struct
_tmpA9;_tmpA9.tag=12;_tmpA9.f1=_tmp6D;_tmpA9.f2=_tmp6E;_tmpA9;});_tmpA8;});_LL3A:
if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 13)goto _LL3C;_tmp6F=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp43)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpAA=
_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpAB;
_tmpAB.tag=13;_tmpAB.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp6F);_tmpAB;});_tmpAA;});_LL3C: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 14)
goto _LL3E;_tmp70=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp43)->f1;_LL3D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));
_tmpAC[0]=({struct Cyc_Absyn_SizeofType_struct _tmpAD;_tmpAD.tag=14;_tmpAD.f1=(
void*)Cyc_Tcutil_copy_type(_tmp70);_tmpAD;});_tmpAC;});_LL3E: if(_tmp43 <= (void*)
3?1:*((int*)_tmp43)!= 15)goto _LL40;_tmp71=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp43)->f1;_LL3F: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpAE=
_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpAF;_tmpAF.tag=15;_tmpAF.f1=(void*)Cyc_Tcutil_copy_type(_tmp71);_tmpAF;});
_tmpAE;});_LL40: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 16)goto _LL42;_tmp72=((
struct Cyc_Absyn_TypedefType_struct*)_tmp43)->f1;_tmp73=((struct Cyc_Absyn_TypedefType_struct*)
_tmp43)->f2;_tmp74=((struct Cyc_Absyn_TypedefType_struct*)_tmp43)->f3;_LL41:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpB0=_cycalloc(sizeof(*
_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_TypedefType_struct _tmpB1;_tmpB1.tag=16;
_tmpB1.f1=_tmp72;_tmpB1.f2=Cyc_Tcutil_copy_types(_tmp73);_tmpB1.f3=_tmp74;_tmpB1.f4=
0;_tmpB1;});_tmpB0;});_LL42: if((int)_tmp43 != 2)goto _LL44;_LL43: return t;_LL44: if(
_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 17)goto _LL46;_tmp75=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp43)->f1;_LL45: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpB2=
_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_AccessEff_struct _tmpB3;
_tmpB3.tag=17;_tmpB3.f1=(void*)Cyc_Tcutil_copy_type(_tmp75);_tmpB3;});_tmpB2;});
_LL46: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 18)goto _LL48;_tmp76=((struct Cyc_Absyn_JoinEff_struct*)
_tmp43)->f1;_LL47: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpB4=_cycalloc(
sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_JoinEff_struct _tmpB5;_tmpB5.tag=18;
_tmpB5.f1=Cyc_Tcutil_copy_types(_tmp76);_tmpB5;});_tmpB4;});_LL48: if(_tmp43 <= (
void*)3?1:*((int*)_tmp43)!= 19)goto _LL19;_tmp77=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp43)->f1;_LL49: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpB6=_cycalloc(
sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpB7;_tmpB7.tag=19;
_tmpB7.f1=(void*)Cyc_Tcutil_copy_type(_tmp77);_tmpB7;});_tmpB6;});_LL19:;}int Cyc_Tcutil_kind_leq(
void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple6 _tmpB9=({struct _tuple6 _tmpB8;
_tmpB8.f1=k1;_tmpB8.f2=k2;_tmpB8;});void*_tmpBA;void*_tmpBB;void*_tmpBC;void*
_tmpBD;void*_tmpBE;void*_tmpBF;_LL4B: _tmpBA=_tmpB9.f1;if((int)_tmpBA != 2)goto
_LL4D;_tmpBB=_tmpB9.f2;if((int)_tmpBB != 1)goto _LL4D;_LL4C: goto _LL4E;_LL4D: _tmpBC=
_tmpB9.f1;if((int)_tmpBC != 2)goto _LL4F;_tmpBD=_tmpB9.f2;if((int)_tmpBD != 0)goto
_LL4F;_LL4E: goto _LL50;_LL4F: _tmpBE=_tmpB9.f1;if((int)_tmpBE != 1)goto _LL51;_tmpBF=
_tmpB9.f2;if((int)_tmpBF != 0)goto _LL51;_LL50: return 1;_LL51:;_LL52: return 0;_LL4A:;}}
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpC0=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpC1;void*_tmpC2;_LL54: if(*((int*)_tmpC0)!= 0)goto _LL56;
_tmpC1=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpC0)->f1;_LL55: return _tmpC1;
_LL56: if(*((int*)_tmpC0)!= 2)goto _LL58;_tmpC2=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpC0)->f2;_LL57: return _tmpC2;_LL58:;_LL59:({void*_tmpC3[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("kind not suitably constrained!",
sizeof(char),31),_tag_arr(_tmpC3,sizeof(void*),0));});_LL53:;}void*Cyc_Tcutil_typ_kind(
void*t){void*_tmpC4=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpC5;struct Cyc_Core_Opt*
_tmpC6;struct Cyc_Absyn_Tvar*_tmpC7;void*_tmpC8;struct Cyc_Absyn_TunionFieldInfo
_tmpC9;void*_tmpCA;struct Cyc_Absyn_Tunionfield*_tmpCB;struct Cyc_Absyn_TunionFieldInfo
_tmpCC;void*_tmpCD;struct Cyc_Absyn_Enumdecl*_tmpCE;struct Cyc_Absyn_Enumdecl*
_tmpCF;struct Cyc_Absyn_PtrInfo _tmpD0;struct Cyc_Absyn_Typedefdecl*_tmpD1;_LL5B:
if(_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 0)goto _LL5D;_tmpC5=((struct Cyc_Absyn_Evar_struct*)
_tmpC4)->f1;_tmpC6=((struct Cyc_Absyn_Evar_struct*)_tmpC4)->f2;_LL5C: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpC5))->v;_LL5D: if(_tmpC4 <= (void*)3?1:*((int*)
_tmpC4)!= 1)goto _LL5F;_tmpC7=((struct Cyc_Absyn_VarType_struct*)_tmpC4)->f1;_LL5E:
return Cyc_Tcutil_tvar_kind(_tmpC7);_LL5F: if((int)_tmpC4 != 0)goto _LL61;_LL60:
return(void*)1;_LL61: if(_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 5)goto _LL63;_tmpC8=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpC4)->f2;_LL62: return _tmpC8 == (void*)
2?(void*)2:(void*)1;_LL63: if((int)_tmpC4 != 1)goto _LL65;_LL64: goto _LL66;_LL65: if(
_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 6)goto _LL67;_LL66: goto _LL68;_LL67: if(
_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 8)goto _LL69;_LL68: return(void*)1;_LL69: if(
_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 15)goto _LL6B;_LL6A: return(void*)2;_LL6B:
if((int)_tmpC4 != 2)goto _LL6D;_LL6C: return(void*)3;_LL6D: if(_tmpC4 <= (void*)3?1:*((
int*)_tmpC4)!= 2)goto _LL6F;_LL6E: return(void*)2;_LL6F: if(_tmpC4 <= (void*)3?1:*((
int*)_tmpC4)!= 3)goto _LL71;_tmpC9=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpC4)->f1;_tmpCA=(void*)_tmpC9.field_info;if(*((int*)_tmpCA)!= 1)goto _LL71;
_tmpCB=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpCA)->f2;_LL70: if(_tmpCB->typs
== 0)return(void*)2;else{return(void*)1;}_LL71: if(_tmpC4 <= (void*)3?1:*((int*)
_tmpC4)!= 3)goto _LL73;_tmpCC=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpC4)->f1;
_tmpCD=(void*)_tmpCC.field_info;if(*((int*)_tmpCD)!= 0)goto _LL73;_LL72:({void*
_tmpD2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typ_kind: Unresolved TunionFieldType",sizeof(char),37),_tag_arr(_tmpD2,
sizeof(void*),0));});_LL73: if(_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 12)goto _LL75;
_tmpCE=((struct Cyc_Absyn_EnumType_struct*)_tmpC4)->f2;if(_tmpCE != 0)goto _LL75;
_LL74: return(void*)0;_LL75: if(_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 10)goto _LL77;
_LL76: goto _LL78;_LL77: if(_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 11)goto _LL79;
_LL78: goto _LL7A;_LL79: if(_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 13)goto _LL7B;
_LL7A: return(void*)1;_LL7B: if(_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 12)goto _LL7D;
_tmpCF=((struct Cyc_Absyn_EnumType_struct*)_tmpC4)->f2;_LL7C: if(_tmpCF->fields == 
0)return(void*)0;else{return(void*)1;}_LL7D: if(_tmpC4 <= (void*)3?1:*((int*)
_tmpC4)!= 4)goto _LL7F;_tmpD0=((struct Cyc_Absyn_PointerType_struct*)_tmpC4)->f1;
_LL7E: {void*_tmpD3=(void*)(Cyc_Absyn_compress_conref(_tmpD0.bounds))->v;void*
_tmpD4;void*_tmpD5;_LL8E: if(_tmpD3 <= (void*)1?1:*((int*)_tmpD3)!= 0)goto _LL90;
_tmpD4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpD3)->f1;if((int)_tmpD4 != 0)
goto _LL90;_LL8F: return(void*)1;_LL90: if(_tmpD3 <= (void*)1?1:*((int*)_tmpD3)!= 0)
goto _LL92;_tmpD5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpD3)->f1;if(
_tmpD5 <= (void*)1?1:*((int*)_tmpD5)!= 0)goto _LL92;_LL91: return(void*)2;_LL92: if((
int)_tmpD3 != 0)goto _LL94;_LL93: return(void*)1;_LL94: if(_tmpD3 <= (void*)1?1:*((
int*)_tmpD3)!= 1)goto _LL8D;_LL95:({void*_tmpD6[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof(char),43),_tag_arr(_tmpD6,sizeof(void*),0));});_LL8D:;}_LL7F: if(_tmpC4 <= (
void*)3?1:*((int*)_tmpC4)!= 14)goto _LL81;_LL80: return(void*)2;_LL81: if(_tmpC4 <= (
void*)3?1:*((int*)_tmpC4)!= 7)goto _LL83;_LL82: goto _LL84;_LL83: if(_tmpC4 <= (void*)
3?1:*((int*)_tmpC4)!= 9)goto _LL85;_LL84: return(void*)1;_LL85: if(_tmpC4 <= (void*)
3?1:*((int*)_tmpC4)!= 16)goto _LL87;_tmpD1=((struct Cyc_Absyn_TypedefType_struct*)
_tmpC4)->f3;_LL86: if(_tmpD1 == 0?1: _tmpD1->kind == 0)({struct Cyc_Std_String_pa_struct
_tmpD8;_tmpD8.tag=0;_tmpD8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpD7[1]={& _tmpD8};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typ_kind: typedef found: %s",sizeof(char),28),_tag_arr(_tmpD7,sizeof(
void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD1->kind))->v;
_LL87: if(_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 17)goto _LL89;_LL88: goto _LL8A;
_LL89: if(_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 18)goto _LL8B;_LL8A: goto _LL8C;
_LL8B: if(_tmpC4 <= (void*)3?1:*((int*)_tmpC4)!= 19)goto _LL5A;_LL8C: return(void*)4;
_LL5A:;}int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmpD9;
_push_handler(& _tmpD9);{int _tmpDB=0;if(setjmp(_tmpD9.handler))_tmpDB=1;if(!
_tmpDB){Cyc_Tcutil_unify_it(t1,t2);{int _tmpDC=1;_npop_handler(0);return _tmpDC;};
_pop_handler();}else{void*_tmpDA=(void*)_exn_thrown;void*_tmpDE=_tmpDA;_LL97: if(
_tmpDE != Cyc_Tcutil_Unify)goto _LL99;_LL98: return 0;_LL99:;_LL9A:(void)_throw(
_tmpDE);_LL96:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){void*_tmpDF=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmpE0;struct Cyc_Core_Opt*_tmpE1;struct Cyc_Core_Opt*
_tmpE2;struct Cyc_Core_Opt**_tmpE3;struct Cyc_Absyn_PtrInfo _tmpE4;void*_tmpE5;
struct Cyc_Absyn_FnInfo _tmpE6;struct Cyc_List_List*_tmpE7;struct Cyc_Core_Opt*
_tmpE8;void*_tmpE9;struct Cyc_List_List*_tmpEA;int _tmpEB;struct Cyc_Absyn_VarargInfo*
_tmpEC;struct Cyc_List_List*_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*
_tmpEF;struct Cyc_Absyn_TunionInfo _tmpF0;struct Cyc_List_List*_tmpF1;void*_tmpF2;
struct Cyc_List_List*_tmpF3;struct Cyc_Core_Opt*_tmpF4;struct Cyc_Absyn_TunionFieldInfo
_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_Absyn_AggrInfo _tmpF7;struct Cyc_List_List*
_tmpF8;struct Cyc_List_List*_tmpF9;void*_tmpFA;void*_tmpFB;void*_tmpFC;void*
_tmpFD;struct Cyc_List_List*_tmpFE;_LL9C: if(_tmpDF <= (void*)3?1:*((int*)_tmpDF)!= 
1)goto _LL9E;_tmpE0=((struct Cyc_Absyn_VarType_struct*)_tmpDF)->f1;_LL9D: if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmpE0)){Cyc_Tcutil_failure_reason=
_tag_arr("(type variable would escape scope)",sizeof(char),35);(int)_throw((void*)
Cyc_Tcutil_Unify);}goto _LL9B;_LL9E: if(_tmpDF <= (void*)3?1:*((int*)_tmpDF)!= 0)
goto _LLA0;_tmpE1=((struct Cyc_Absyn_Evar_struct*)_tmpDF)->f2;_tmpE2=((struct Cyc_Absyn_Evar_struct*)
_tmpDF)->f4;_tmpE3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmpDF)->f4;
_LL9F: if(t == evar){Cyc_Tcutil_failure_reason=_tag_arr("(occurs check)",sizeof(
char),15);(int)_throw((void*)Cyc_Tcutil_Unify);}else{if(_tmpE1 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmpE1->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmpE3))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmpFF=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpE3))->v;for(0;s != 
0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmpFF=({struct Cyc_List_List*_tmp100=_cycalloc(
sizeof(*_tmp100));_tmp100->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp100->tl=_tmpFF;
_tmp100;});}}*_tmpE3=({struct Cyc_Core_Opt*_tmp101=_cycalloc(sizeof(*_tmp101));
_tmp101->v=_tmpFF;_tmp101;});}}}goto _LL9B;_LLA0: if(_tmpDF <= (void*)3?1:*((int*)
_tmpDF)!= 4)goto _LLA2;_tmpE4=((struct Cyc_Absyn_PointerType_struct*)_tmpDF)->f1;
_LLA1: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpE4.elt_typ);Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmpE4.rgn_typ);goto _LL9B;_LLA2: if(_tmpDF <= (void*)3?1:*((int*)
_tmpDF)!= 7)goto _LLA4;_tmpE5=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpDF)->f1;
_LLA3: Cyc_Tcutil_occurs(evar,r,env,_tmpE5);goto _LL9B;_LLA4: if(_tmpDF <= (void*)3?
1:*((int*)_tmpDF)!= 8)goto _LLA6;_tmpE6=((struct Cyc_Absyn_FnType_struct*)_tmpDF)->f1;
_tmpE7=_tmpE6.tvars;_tmpE8=_tmpE6.effect;_tmpE9=(void*)_tmpE6.ret_typ;_tmpEA=
_tmpE6.args;_tmpEB=_tmpE6.c_varargs;_tmpEC=_tmpE6.cyc_varargs;_tmpED=_tmpE6.rgn_po;
_tmpEE=_tmpE6.attributes;_LLA5: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmpE7,env);if(
_tmpE8 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpE8->v);Cyc_Tcutil_occurs(evar,
r,env,_tmpE9);for(0;_tmpEA != 0;_tmpEA=_tmpEA->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple2*)_tmpEA->hd)).f3);}if(_tmpEC != 0)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmpEC->type);for(0;_tmpED != 0;_tmpED=_tmpED->tl){struct _tuple6 _tmp103;
void*_tmp104;void*_tmp105;struct _tuple6*_tmp102=(struct _tuple6*)_tmpED->hd;
_tmp103=*_tmp102;_tmp104=_tmp103.f1;_tmp105=_tmp103.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp104);Cyc_Tcutil_occurs(evar,r,env,_tmp105);}goto _LL9B;_LLA6: if(_tmpDF <= (
void*)3?1:*((int*)_tmpDF)!= 9)goto _LLA8;_tmpEF=((struct Cyc_Absyn_TupleType_struct*)
_tmpDF)->f1;_LLA7: for(0;_tmpEF != 0;_tmpEF=_tmpEF->tl){Cyc_Tcutil_occurs(evar,r,
env,(*((struct _tuple4*)_tmpEF->hd)).f2);}goto _LL9B;_LLA8: if(_tmpDF <= (void*)3?1:*((
int*)_tmpDF)!= 2)goto _LLAA;_tmpF0=((struct Cyc_Absyn_TunionType_struct*)_tmpDF)->f1;
_tmpF1=_tmpF0.targs;_tmpF2=(void*)_tmpF0.rgn;_LLA9: Cyc_Tcutil_occurs(evar,r,env,
_tmpF2);Cyc_Tcutil_occurslist(evar,r,env,_tmpF1);goto _LL9B;_LLAA: if(_tmpDF <= (
void*)3?1:*((int*)_tmpDF)!= 16)goto _LLAC;_tmpF3=((struct Cyc_Absyn_TypedefType_struct*)
_tmpDF)->f2;_tmpF4=((struct Cyc_Absyn_TypedefType_struct*)_tmpDF)->f4;_LLAB:
_tmpF6=_tmpF3;goto _LLAD;_LLAC: if(_tmpDF <= (void*)3?1:*((int*)_tmpDF)!= 3)goto
_LLAE;_tmpF5=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpDF)->f1;_tmpF6=_tmpF5.targs;
_LLAD: _tmpF8=_tmpF6;goto _LLAF;_LLAE: if(_tmpDF <= (void*)3?1:*((int*)_tmpDF)!= 10)
goto _LLB0;_tmpF7=((struct Cyc_Absyn_AggrType_struct*)_tmpDF)->f1;_tmpF8=_tmpF7.targs;
_LLAF: Cyc_Tcutil_occurslist(evar,r,env,_tmpF8);goto _LL9B;_LLB0: if(_tmpDF <= (void*)
3?1:*((int*)_tmpDF)!= 11)goto _LLB2;_tmpF9=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpDF)->f2;_LLB1: for(0;_tmpF9 != 0;_tmpF9=_tmpF9->tl){Cyc_Tcutil_occurs(evar,r,
env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmpF9->hd)->type);}goto _LL9B;_LLB2: if(
_tmpDF <= (void*)3?1:*((int*)_tmpDF)!= 15)goto _LLB4;_tmpFA=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmpDF)->f1;_LLB3: _tmpFB=_tmpFA;goto _LLB5;_LLB4: if(_tmpDF <= (void*)3?1:*((int*)
_tmpDF)!= 14)goto _LLB6;_tmpFB=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmpDF)->f1;
_LLB5: _tmpFC=_tmpFB;goto _LLB7;_LLB6: if(_tmpDF <= (void*)3?1:*((int*)_tmpDF)!= 17)
goto _LLB8;_tmpFC=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmpDF)->f1;_LLB7:
_tmpFD=_tmpFC;goto _LLB9;_LLB8: if(_tmpDF <= (void*)3?1:*((int*)_tmpDF)!= 19)goto
_LLBA;_tmpFD=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmpDF)->f1;_LLB9: Cyc_Tcutil_occurs(
evar,r,env,_tmpFD);goto _LL9B;_LLBA: if(_tmpDF <= (void*)3?1:*((int*)_tmpDF)!= 18)
goto _LLBC;_tmpFE=((struct Cyc_Absyn_JoinEff_struct*)_tmpDF)->f1;_LLBB: Cyc_Tcutil_occurslist(
evar,r,env,_tmpFE);goto _LL9B;_LLBC:;_LLBD: goto _LL9B;_LL9B:;}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 
0?t2 != 0: 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0?1: t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){if((tq1.q_const != tq2.q_const?
1: tq1.q_volatile != tq2.q_volatile)?1: tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_failure_reason=
_tag_arr("(qualifiers don't match)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.q_const == tq2.q_const?tq1.q_volatile == tq2.q_volatile: 0)?tq1.q_restrict
== tq2.q_restrict: 0;}static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _tagged_arr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{void*_tmp106=(void*)x->v;void*
_tmp107;_LLBF: if((int)_tmp106 != 0)goto _LLC1;_LLC0:(void*)(x->v=(void*)((void*)({
struct Cyc_Absyn_Forward_constr_struct*_tmp108=_cycalloc(sizeof(*_tmp108));
_tmp108[0]=({struct Cyc_Absyn_Forward_constr_struct _tmp109;_tmp109.tag=1;_tmp109.f1=
y;_tmp109;});_tmp108;})));return;_LLC1: if(_tmp106 <= (void*)1?1:*((int*)_tmp106)
!= 1)goto _LLC3;_LLC2:({void*_tmp10A[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress",
sizeof(char),37),_tag_arr(_tmp10A,sizeof(void*),0));});_LLC3: if(_tmp106 <= (void*)
1?1:*((int*)_tmp106)!= 0)goto _LLBE;_tmp107=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp106)->f1;_LLC4: {void*_tmp10B=(void*)y->v;void*_tmp10C;_LLC6: if((int)_tmp10B
!= 0)goto _LLC8;_LLC7:(void*)(y->v=(void*)((void*)x->v));return;_LLC8: if(_tmp10B
<= (void*)1?1:*((int*)_tmp10B)!= 1)goto _LLCA;_LLC9:({void*_tmp10D[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress(2)",
sizeof(char),40),_tag_arr(_tmp10D,sizeof(void*),0));});_LLCA: if(_tmp10B <= (void*)
1?1:*((int*)_tmp10B)!= 0)goto _LLC5;_tmp10C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10B)->f1;_LLCB: if(cmp(_tmp107,_tmp10C)!= 0){Cyc_Tcutil_failure_reason=reason;(
int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLC5:;}_LLBE:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp10E;_push_handler(& _tmp10E);{int _tmp110=0;if(setjmp(_tmp10E.handler))
_tmp110=1;if(!_tmp110){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)
_tag_arr(0,0,0));{int _tmp111=1;_npop_handler(0);return _tmp111;};_pop_handler();}
else{void*_tmp10F=(void*)_exn_thrown;void*_tmp113=_tmp10F;_LLCD: if(_tmp113 != Cyc_Tcutil_Unify)
goto _LLCF;_LLCE: return 0;_LLCF:;_LLD0:(void)_throw(_tmp113);_LLCC:;}}}static int
Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple6 _tmp115=({struct _tuple6
_tmp114;_tmp114.f1=b1;_tmp114.f2=b2;_tmp114;});void*_tmp116;void*_tmp117;void*
_tmp118;void*_tmp119;void*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;void*_tmp11C;
struct Cyc_Absyn_Exp*_tmp11D;_LLD2: _tmp116=_tmp115.f1;if((int)_tmp116 != 0)goto
_LLD4;_tmp117=_tmp115.f2;if((int)_tmp117 != 0)goto _LLD4;_LLD3: return 0;_LLD4:
_tmp118=_tmp115.f1;if((int)_tmp118 != 0)goto _LLD6;_LLD5: return - 1;_LLD6: _tmp119=
_tmp115.f2;if((int)_tmp119 != 0)goto _LLD8;_LLD7: return 1;_LLD8: _tmp11A=_tmp115.f1;
if(_tmp11A <= (void*)1?1:*((int*)_tmp11A)!= 0)goto _LLD1;_tmp11B=((struct Cyc_Absyn_Upper_b_struct*)
_tmp11A)->f1;_tmp11C=_tmp115.f2;if(_tmp11C <= (void*)1?1:*((int*)_tmp11C)!= 0)
goto _LLD1;_tmp11D=((struct Cyc_Absyn_Upper_b_struct*)_tmp11C)->f1;_LLD9: {int i1=(
int)Cyc_Evexp_eval_const_uint_exp(_tmp11B);int i2=(int)Cyc_Evexp_eval_const_uint_exp(
_tmp11D);if(i1 == i2)return 0;if(i1 < i2)return - 1;return 1;}_LLD1:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){if(a1 == a2)return 1;{struct _tuple6 _tmp11F=({struct _tuple6 _tmp11E;
_tmp11E.f1=a1;_tmp11E.f2=a2;_tmp11E;});void*_tmp120;void*_tmp121;int _tmp122;int
_tmp123;void*_tmp124;void*_tmp125;int _tmp126;int _tmp127;void*_tmp128;int _tmp129;
void*_tmp12A;int _tmp12B;void*_tmp12C;int _tmp12D;void*_tmp12E;int _tmp12F;void*
_tmp130;struct _tagged_arr _tmp131;void*_tmp132;struct _tagged_arr _tmp133;_LLDB:
_tmp120=_tmp11F.f1;if(_tmp120 <= (void*)16?1:*((int*)_tmp120)!= 3)goto _LLDD;
_tmp121=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp120)->f1;_tmp122=((
struct Cyc_Absyn_Format_att_struct*)_tmp120)->f2;_tmp123=((struct Cyc_Absyn_Format_att_struct*)
_tmp120)->f3;_tmp124=_tmp11F.f2;if(_tmp124 <= (void*)16?1:*((int*)_tmp124)!= 3)
goto _LLDD;_tmp125=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp124)->f1;
_tmp126=((struct Cyc_Absyn_Format_att_struct*)_tmp124)->f2;_tmp127=((struct Cyc_Absyn_Format_att_struct*)
_tmp124)->f3;_LLDC: return(_tmp121 == _tmp125?_tmp122 == _tmp126: 0)?_tmp123 == 
_tmp127: 0;_LLDD: _tmp128=_tmp11F.f1;if(_tmp128 <= (void*)16?1:*((int*)_tmp128)!= 0)
goto _LLDF;_tmp129=((struct Cyc_Absyn_Regparm_att_struct*)_tmp128)->f1;_tmp12A=
_tmp11F.f2;if(_tmp12A <= (void*)16?1:*((int*)_tmp12A)!= 0)goto _LLDF;_tmp12B=((
struct Cyc_Absyn_Regparm_att_struct*)_tmp12A)->f1;_LLDE: _tmp12D=_tmp129;_tmp12F=
_tmp12B;goto _LLE0;_LLDF: _tmp12C=_tmp11F.f1;if(_tmp12C <= (void*)16?1:*((int*)
_tmp12C)!= 1)goto _LLE1;_tmp12D=((struct Cyc_Absyn_Aligned_att_struct*)_tmp12C)->f1;
_tmp12E=_tmp11F.f2;if(_tmp12E <= (void*)16?1:*((int*)_tmp12E)!= 1)goto _LLE1;
_tmp12F=((struct Cyc_Absyn_Aligned_att_struct*)_tmp12E)->f1;_LLE0: return _tmp12D == 
_tmp12F;_LLE1: _tmp130=_tmp11F.f1;if(_tmp130 <= (void*)16?1:*((int*)_tmp130)!= 2)
goto _LLE3;_tmp131=((struct Cyc_Absyn_Section_att_struct*)_tmp130)->f1;_tmp132=
_tmp11F.f2;if(_tmp132 <= (void*)16?1:*((int*)_tmp132)!= 2)goto _LLE3;_tmp133=((
struct Cyc_Absyn_Section_att_struct*)_tmp132)->f1;_LLE2: return Cyc_Std_strcmp(
_tmp131,_tmp133)== 0;_LLE3:;_LLE4: return 0;_LLDA:;}}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return
0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_normalize_effect(void*e);
static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(struct
Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((void*)af->type);}static struct
_tuple7*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{void*
_tmp134=Cyc_Tcutil_tvar_kind(tv);_LLE6: if((int)_tmp134 != 3)goto _LLE8;_LLE7: t=(
void*)2;goto _LLE5;_LLE8: if((int)_tmp134 != 4)goto _LLEA;_LLE9: t=Cyc_Absyn_empty_effect;
goto _LLE5;_LLEA:;_LLEB: t=Cyc_Absyn_sint_t;goto _LLE5;_LLE5:;}return({struct
_tuple7*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->f1=tv;_tmp135->f2=t;_tmp135;});}
static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp136=Cyc_Tcutil_compress(t);void*
_tmp137;struct Cyc_Absyn_TunionInfo _tmp138;struct Cyc_List_List*_tmp139;void*
_tmp13A;struct Cyc_Absyn_PtrInfo _tmp13B;void*_tmp13C;void*_tmp13D;void*_tmp13E;
struct Cyc_List_List*_tmp13F;struct Cyc_Absyn_TunionFieldInfo _tmp140;struct Cyc_List_List*
_tmp141;struct Cyc_Absyn_AggrInfo _tmp142;struct Cyc_List_List*_tmp143;struct Cyc_List_List*
_tmp144;void*_tmp145;struct Cyc_Absyn_FnInfo _tmp146;struct Cyc_List_List*_tmp147;
struct Cyc_Core_Opt*_tmp148;void*_tmp149;struct Cyc_List_List*_tmp14A;struct Cyc_Absyn_VarargInfo*
_tmp14B;struct Cyc_List_List*_tmp14C;void*_tmp14D;struct Cyc_List_List*_tmp14E;
_LLED: if((int)_tmp136 != 0)goto _LLEF;_LLEE: goto _LLF0;_LLEF: if((int)_tmp136 != 1)
goto _LLF1;_LLF0: goto _LLF2;_LLF1: if(_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 6)
goto _LLF3;_LLF2: goto _LLF4;_LLF3: if(_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 12)
goto _LLF5;_LLF4: goto _LLF6;_LLF5: if(_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 13)
goto _LLF7;_LLF6: goto _LLF8;_LLF7: if(_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 5)
goto _LLF9;_LLF8: return Cyc_Absyn_empty_effect;_LLF9: if(_tmp136 <= (void*)3?1:*((
int*)_tmp136)!= 0)goto _LLFB;_LLFA: goto _LLFC;_LLFB: if(_tmp136 <= (void*)3?1:*((int*)
_tmp136)!= 1)goto _LLFD;_LLFC: {void*_tmp14F=Cyc_Tcutil_typ_kind(t);_LL11C: if((
int)_tmp14F != 3)goto _LL11E;_LL11D: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp151;_tmp151.tag=17;_tmp151.f1=(void*)t;_tmp151;});_tmp150;});_LL11E: if((int)
_tmp14F != 4)goto _LL120;_LL11F: return t;_LL120:;_LL121: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp153;_tmp153.tag=19;_tmp153.f1=(void*)t;_tmp153;});_tmp152;});_LL11B:;}_LLFD:
if(_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 15)goto _LLFF;_tmp137=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp136)->f1;_LLFE: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp155;_tmp155.tag=17;_tmp155.f1=(void*)_tmp137;_tmp155;});_tmp154;});_LLFF: if(
_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 2)goto _LL101;_tmp138=((struct Cyc_Absyn_TunionType_struct*)
_tmp136)->f1;_tmp139=_tmp138.targs;_tmp13A=(void*)_tmp138.rgn;_LL100: {struct Cyc_List_List*
ts=({struct Cyc_List_List*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp159=_cycalloc(sizeof(*_tmp159));
_tmp159[0]=({struct Cyc_Absyn_AccessEff_struct _tmp15A;_tmp15A.tag=17;_tmp15A.f1=(
void*)_tmp13A;_tmp15A;});_tmp159;}));_tmp158->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp139);_tmp158;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp157;_tmp157.tag=18;_tmp157.f1=ts;_tmp157;});_tmp156;}));}_LL101: if(_tmp136 <= (
void*)3?1:*((int*)_tmp136)!= 4)goto _LL103;_tmp13B=((struct Cyc_Absyn_PointerType_struct*)
_tmp136)->f1;_tmp13C=(void*)_tmp13B.elt_typ;_tmp13D=(void*)_tmp13B.rgn_typ;
_LL102: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp15C;_tmp15C.tag=18;_tmp15C.f1=({void*_tmp15D[2];_tmp15D[1]=Cyc_Tcutil_rgns_of(
_tmp13C);_tmp15D[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp15E=_cycalloc(
sizeof(*_tmp15E));_tmp15E[0]=({struct Cyc_Absyn_AccessEff_struct _tmp15F;_tmp15F.tag=
17;_tmp15F.f1=(void*)_tmp13D;_tmp15F;});_tmp15E;});Cyc_List_list(_tag_arr(
_tmp15D,sizeof(void*),2));});_tmp15C;});_tmp15B;}));_LL103: if(_tmp136 <= (void*)3?
1:*((int*)_tmp136)!= 7)goto _LL105;_tmp13E=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp136)->f1;_LL104: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp13E));
_LL105: if(_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 9)goto _LL107;_tmp13F=((struct
Cyc_Absyn_TupleType_struct*)_tmp136)->f1;_LL106: {struct Cyc_List_List*_tmp160=0;
for(0;_tmp13F != 0;_tmp13F=_tmp13F->tl){_tmp160=({struct Cyc_List_List*_tmp161=
_cycalloc(sizeof(*_tmp161));_tmp161->hd=(void*)(*((struct _tuple4*)_tmp13F->hd)).f2;
_tmp161->tl=_tmp160;_tmp161;});}_tmp141=_tmp160;goto _LL108;}_LL107: if(_tmp136 <= (
void*)3?1:*((int*)_tmp136)!= 3)goto _LL109;_tmp140=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp136)->f1;_tmp141=_tmp140.targs;_LL108: _tmp143=_tmp141;goto _LL10A;_LL109: if(
_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 10)goto _LL10B;_tmp142=((struct Cyc_Absyn_AggrType_struct*)
_tmp136)->f1;_tmp143=_tmp142.targs;_LL10A: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp162=_cycalloc(sizeof(*_tmp162));
_tmp162[0]=({struct Cyc_Absyn_JoinEff_struct _tmp163;_tmp163.tag=18;_tmp163.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp143);_tmp163;});_tmp162;}));_LL10B: if(_tmp136 <= (void*)3?
1:*((int*)_tmp136)!= 11)goto _LL10D;_tmp144=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp136)->f2;_LL10C: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp165;_tmp165.tag=18;_tmp165.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp144);_tmp165;});
_tmp164;}));_LL10D: if(_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 14)goto _LL10F;
_tmp145=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp136)->f1;_LL10E: return
Cyc_Tcutil_rgns_of(_tmp145);_LL10F: if(_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 8)
goto _LL111;_tmp146=((struct Cyc_Absyn_FnType_struct*)_tmp136)->f1;_tmp147=_tmp146.tvars;
_tmp148=_tmp146.effect;_tmp149=(void*)_tmp146.ret_typ;_tmp14A=_tmp146.args;
_tmp14B=_tmp146.cyc_varargs;_tmp14C=_tmp146.rgn_po;_LL110: {void*_tmp166=Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp147),(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp148))->v);return Cyc_Tcutil_normalize_effect(_tmp166);}_LL111: if((
int)_tmp136 != 2)goto _LL113;_LL112: return Cyc_Absyn_empty_effect;_LL113: if(_tmp136
<= (void*)3?1:*((int*)_tmp136)!= 17)goto _LL115;_LL114: goto _LL116;_LL115: if(
_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 18)goto _LL117;_LL116: return t;_LL117: if(
_tmp136 <= (void*)3?1:*((int*)_tmp136)!= 19)goto _LL119;_tmp14D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp136)->f1;_LL118: return Cyc_Tcutil_rgns_of(_tmp14D);_LL119: if(_tmp136 <= (void*)
3?1:*((int*)_tmp136)!= 16)goto _LLEC;_tmp14E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp136)->f2;_LL11A: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp168;_tmp168.tag=18;_tmp168.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp14E);
_tmp168;});_tmp167;}));_LLEC:;}static void*Cyc_Tcutil_normalize_effect(void*e){e=
Cyc_Tcutil_compress(e);{void*_tmp169=e;struct Cyc_List_List*_tmp16A;struct Cyc_List_List**
_tmp16B;void*_tmp16C;_LL123: if(_tmp169 <= (void*)3?1:*((int*)_tmp169)!= 18)goto
_LL125;_tmp16A=((struct Cyc_Absyn_JoinEff_struct*)_tmp169)->f1;_tmp16B=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp169)->f1;_LL124: {int nested_join=1;{struct
Cyc_List_List*effs=*_tmp16B;for(0;effs != 0;effs=effs->tl){void*_tmp16D=(void*)
effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
_tmp16D)));{void*_tmp16E=(void*)effs->hd;_LL12A: if(_tmp16E <= (void*)3?1:*((int*)
_tmp16E)!= 18)goto _LL12C;_LL12B: nested_join=1;goto _LL129;_LL12C:;_LL12D: goto
_LL129;_LL129:;}}}if(!nested_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp16B;for(0;effs != 0;effs=effs->tl){void*_tmp16F=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp170;void*_tmp171;_LL12F: if(_tmp16F <= (
void*)3?1:*((int*)_tmp16F)!= 18)goto _LL131;_tmp170=((struct Cyc_Absyn_JoinEff_struct*)
_tmp16F)->f1;_LL130: effects=Cyc_List_revappend(_tmp170,effects);goto _LL12E;
_LL131: if(_tmp16F <= (void*)3?1:*((int*)_tmp16F)!= 17)goto _LL133;_tmp171=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp16F)->f1;if((int)_tmp171 != 2)goto _LL133;
_LL132: goto _LL12E;_LL133:;_LL134: effects=({struct Cyc_List_List*_tmp172=_cycalloc(
sizeof(*_tmp172));_tmp172->hd=(void*)_tmp16F;_tmp172->tl=effects;_tmp172;});goto
_LL12E;_LL12E:;}}*_tmp16B=Cyc_List_imp_rev(effects);return e;}}_LL125: if(_tmp169
<= (void*)3?1:*((int*)_tmp169)!= 19)goto _LL127;_tmp16C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp169)->f1;_LL126: return Cyc_Tcutil_rgns_of(_tmp16C);_LL127:;_LL128: return e;
_LL122:;}}struct _tuple10{void*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;}
;static struct _tuple10*Cyc_Tcutil_get_effect_evar(void*t){void*_tmp173=Cyc_Tcutil_compress(
t);struct Cyc_List_List*_tmp174;struct Cyc_List_List _tmp175;void*_tmp176;struct Cyc_List_List*
_tmp177;struct Cyc_Core_Opt*_tmp178;struct Cyc_Core_Opt*_tmp179;_LL136: if(_tmp173
<= (void*)3?1:*((int*)_tmp173)!= 18)goto _LL138;_tmp174=((struct Cyc_Absyn_JoinEff_struct*)
_tmp173)->f1;if(_tmp174 == 0)goto _LL138;_tmp175=*_tmp174;_tmp176=(void*)_tmp175.hd;
_tmp177=_tmp175.tl;_LL137: {void*_tmp17A=Cyc_Tcutil_compress(_tmp176);struct Cyc_Core_Opt*
_tmp17B;_LL13D: if(_tmp17A <= (void*)3?1:*((int*)_tmp17A)!= 0)goto _LL13F;_tmp17B=((
struct Cyc_Absyn_Evar_struct*)_tmp17A)->f4;_LL13E: return({struct _tuple10*_tmp17C=
_cycalloc(sizeof(*_tmp17C));_tmp17C->f1=_tmp176;_tmp17C->f2=_tmp177;_tmp17C->f3=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp17B))->v;_tmp17C;});
_LL13F:;_LL140: return 0;_LL13C:;}_LL138: if(_tmp173 <= (void*)3?1:*((int*)_tmp173)
!= 0)goto _LL13A;_tmp178=((struct Cyc_Absyn_Evar_struct*)_tmp173)->f1;_tmp179=((
struct Cyc_Absyn_Evar_struct*)_tmp173)->f4;_LL139: if(_tmp178 == 0?1:(void*)_tmp178->v
!= (void*)4)({void*_tmp17D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",sizeof(char),27),
_tag_arr(_tmp17D,sizeof(void*),0));});return({struct _tuple10*_tmp17E=_cycalloc(
sizeof(*_tmp17E));_tmp17E->f1=t;_tmp17E->f2=0;_tmp17E->f3=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp179))->v;_tmp17E;});_LL13A:;_LL13B: return 0;
_LL135:;}static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*
Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*_tmp17F=({struct
Cyc_Absyn_FnType_struct*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180[0]=({struct
Cyc_Absyn_FnType_struct _tmp181;_tmp181.tag=8;_tmp181.f1=({struct Cyc_Absyn_FnInfo
_tmp182;_tmp182.tvars=0;_tmp182.effect=({struct Cyc_Core_Opt*_tmp183=_cycalloc(
sizeof(*_tmp183));_tmp183->v=(void*)eff;_tmp183;});_tmp182.ret_typ=(void*)((void*)
0);_tmp182.args=0;_tmp182.c_varargs=0;_tmp182.cyc_varargs=0;_tmp182.rgn_po=0;
_tmp182.attributes=0;_tmp182;});_tmp181;});_tmp180;});return Cyc_Absyn_atb_typ((
void*)_tmp17F,(void*)2,Cyc_Absyn_empty_tqual(),Cyc_Absyn_bounds_one);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{
void*_tmp184=Cyc_Tcutil_compress(e);void*_tmp185;struct Cyc_List_List*_tmp186;
void*_tmp187;struct Cyc_Core_Opt*_tmp188;struct Cyc_Core_Opt*_tmp189;struct Cyc_Core_Opt**
_tmp18A;struct Cyc_Core_Opt*_tmp18B;_LL142: if(_tmp184 <= (void*)3?1:*((int*)
_tmp184)!= 17)goto _LL144;_tmp185=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp184)->f1;_LL143: if(constrain)return Cyc_Tcutil_unify(r,_tmp185);_tmp185=Cyc_Tcutil_compress(
_tmp185);if(r == _tmp185)return 1;{struct _tuple6 _tmp18D=({struct _tuple6 _tmp18C;
_tmp18C.f1=r;_tmp18C.f2=_tmp185;_tmp18C;});void*_tmp18E;struct Cyc_Absyn_Tvar*
_tmp18F;void*_tmp190;struct Cyc_Absyn_Tvar*_tmp191;_LL14D: _tmp18E=_tmp18D.f1;if(
_tmp18E <= (void*)3?1:*((int*)_tmp18E)!= 1)goto _LL14F;_tmp18F=((struct Cyc_Absyn_VarType_struct*)
_tmp18E)->f1;_tmp190=_tmp18D.f2;if(_tmp190 <= (void*)3?1:*((int*)_tmp190)!= 1)
goto _LL14F;_tmp191=((struct Cyc_Absyn_VarType_struct*)_tmp190)->f1;_LL14E: return
Cyc_Absyn_tvar_cmp(_tmp18F,_tmp191)== 0;_LL14F:;_LL150: return 0;_LL14C:;}_LL144:
if(_tmp184 <= (void*)3?1:*((int*)_tmp184)!= 18)goto _LL146;_tmp186=((struct Cyc_Absyn_JoinEff_struct*)
_tmp184)->f1;_LL145: for(0;_tmp186 != 0;_tmp186=_tmp186->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp186->hd))return 1;}return 0;_LL146: if(_tmp184 <= (void*)3?1:*((
int*)_tmp184)!= 19)goto _LL148;_tmp187=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp184)->f1;_LL147: {void*_tmp192=Cyc_Tcutil_rgns_of(_tmp187);void*_tmp193;
_LL152: if(_tmp192 <= (void*)3?1:*((int*)_tmp192)!= 19)goto _LL154;_tmp193=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp192)->f1;_LL153: if(!constrain)return 0;{void*
_tmp194=Cyc_Tcutil_compress(_tmp193);struct Cyc_Core_Opt*_tmp195;struct Cyc_Core_Opt*
_tmp196;struct Cyc_Core_Opt**_tmp197;struct Cyc_Core_Opt*_tmp198;_LL157: if(_tmp194
<= (void*)3?1:*((int*)_tmp194)!= 0)goto _LL159;_tmp195=((struct Cyc_Absyn_Evar_struct*)
_tmp194)->f1;_tmp196=((struct Cyc_Absyn_Evar_struct*)_tmp194)->f2;_tmp197=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp194)->f2;_tmp198=((struct Cyc_Absyn_Evar_struct*)
_tmp194)->f4;_LL158: {void*_tmp199=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp198);Cyc_Tcutil_occurs(_tmp199,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp198))->v,r);{void*_tmp19A=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp19C=_cycalloc(sizeof(*_tmp19C));
_tmp19C[0]=({struct Cyc_Absyn_JoinEff_struct _tmp19D;_tmp19D.tag=18;_tmp19D.f1=({
void*_tmp19E[2];_tmp19E[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp19F=
_cycalloc(sizeof(*_tmp19F));_tmp19F[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1A0;
_tmp1A0.tag=17;_tmp1A0.f1=(void*)r;_tmp1A0;});_tmp19F;});_tmp19E[0]=_tmp199;Cyc_List_list(
_tag_arr(_tmp19E,sizeof(void*),2));});_tmp19D;});_tmp19C;}));*_tmp197=({struct
Cyc_Core_Opt*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->v=(void*)_tmp19A;
_tmp19B;});return 1;}}_LL159:;_LL15A: return 0;_LL156:;}_LL154:;_LL155: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp192);_LL151:;}_LL148: if(_tmp184 <= (void*)3?1:*((int*)_tmp184)!= 
0)goto _LL14A;_tmp188=((struct Cyc_Absyn_Evar_struct*)_tmp184)->f1;_tmp189=((
struct Cyc_Absyn_Evar_struct*)_tmp184)->f2;_tmp18A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp184)->f2;_tmp18B=((struct Cyc_Absyn_Evar_struct*)
_tmp184)->f4;_LL149: if(_tmp188 == 0?1:(void*)_tmp188->v != (void*)4)({void*_tmp1A1[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(char),27),_tag_arr(_tmp1A1,sizeof(
void*),0));});if(!constrain)return 0;{void*_tmp1A2=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp18B);Cyc_Tcutil_occurs(_tmp1A2,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp18B))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp1A3=({struct Cyc_Absyn_JoinEff_struct*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));
_tmp1A5[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1A6;_tmp1A6.tag=18;_tmp1A6.f1=({
struct Cyc_List_List*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->hd=(void*)
_tmp1A2;_tmp1A7->tl=({struct Cyc_List_List*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));
_tmp1A8->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1A9=_cycalloc(
sizeof(*_tmp1A9));_tmp1A9[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1AA;_tmp1AA.tag=
17;_tmp1AA.f1=(void*)r;_tmp1AA;});_tmp1A9;}));_tmp1A8->tl=0;_tmp1A8;});_tmp1A7;});
_tmp1A6;});_tmp1A5;});*_tmp18A=({struct Cyc_Core_Opt*_tmp1A4=_cycalloc(sizeof(*
_tmp1A4));_tmp1A4->v=(void*)((void*)_tmp1A3);_tmp1A4;});return 1;}}_LL14A:;_LL14B:
return 0;_LL141:;}}static int Cyc_Tcutil_type_in_effect(int constrain,void*t,void*e){
t=Cyc_Tcutil_compress(t);{void*_tmp1AB=Cyc_Tcutil_compress(e);struct Cyc_List_List*
_tmp1AC;void*_tmp1AD;struct Cyc_Core_Opt*_tmp1AE;struct Cyc_Core_Opt*_tmp1AF;
struct Cyc_Core_Opt**_tmp1B0;struct Cyc_Core_Opt*_tmp1B1;_LL15C: if(_tmp1AB <= (void*)
3?1:*((int*)_tmp1AB)!= 17)goto _LL15E;_LL15D: return 0;_LL15E: if(_tmp1AB <= (void*)3?
1:*((int*)_tmp1AB)!= 18)goto _LL160;_tmp1AC=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1AB)->f1;_LL15F: for(0;_tmp1AC != 0;_tmp1AC=_tmp1AC->tl){if(Cyc_Tcutil_type_in_effect(
constrain,t,(void*)_tmp1AC->hd))return 1;}return 0;_LL160: if(_tmp1AB <= (void*)3?1:*((
int*)_tmp1AB)!= 19)goto _LL162;_tmp1AD=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1AB)->f1;_LL161: _tmp1AD=Cyc_Tcutil_compress(_tmp1AD);if(t == _tmp1AD)return 1;
if(constrain)return Cyc_Tcutil_unify(t,_tmp1AD);{void*_tmp1B2=Cyc_Tcutil_rgns_of(
t);void*_tmp1B3;_LL167: if(_tmp1B2 <= (void*)3?1:*((int*)_tmp1B2)!= 19)goto _LL169;
_tmp1B3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1B2)->f1;_LL168: {struct
_tuple6 _tmp1B5=({struct _tuple6 _tmp1B4;_tmp1B4.f1=t;_tmp1B4.f2=Cyc_Tcutil_compress(
_tmp1B3);_tmp1B4;});void*_tmp1B6;struct Cyc_Absyn_Tvar*_tmp1B7;void*_tmp1B8;
struct Cyc_Absyn_Tvar*_tmp1B9;_LL16C: _tmp1B6=_tmp1B5.f1;if(_tmp1B6 <= (void*)3?1:*((
int*)_tmp1B6)!= 1)goto _LL16E;_tmp1B7=((struct Cyc_Absyn_VarType_struct*)_tmp1B6)->f1;
_tmp1B8=_tmp1B5.f2;if(_tmp1B8 <= (void*)3?1:*((int*)_tmp1B8)!= 1)goto _LL16E;
_tmp1B9=((struct Cyc_Absyn_VarType_struct*)_tmp1B8)->f1;_LL16D: return Cyc_Tcutil_unify(
t,_tmp1B3);_LL16E:;_LL16F: return t == _tmp1B3;_LL16B:;}_LL169:;_LL16A: return Cyc_Tcutil_type_in_effect(
constrain,t,_tmp1B2);_LL166:;}_LL162: if(_tmp1AB <= (void*)3?1:*((int*)_tmp1AB)!= 
0)goto _LL164;_tmp1AE=((struct Cyc_Absyn_Evar_struct*)_tmp1AB)->f1;_tmp1AF=((
struct Cyc_Absyn_Evar_struct*)_tmp1AB)->f2;_tmp1B0=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1AB)->f2;_tmp1B1=((struct Cyc_Absyn_Evar_struct*)
_tmp1AB)->f4;_LL163: if(_tmp1AE == 0?1:(void*)_tmp1AE->v != (void*)4)({void*_tmp1BA[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(char),27),_tag_arr(_tmp1BA,sizeof(
void*),0));});if(!constrain)return 0;{void*_tmp1BB=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp1B1);Cyc_Tcutil_occurs(_tmp1BB,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1B1))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp1BC=({struct Cyc_Absyn_JoinEff_struct*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));
_tmp1BE[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1BF;_tmp1BF.tag=18;_tmp1BF.f1=({
struct Cyc_List_List*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->hd=(void*)
_tmp1BB;_tmp1C0->tl=({struct Cyc_List_List*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));
_tmp1C1->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp1C2=_cycalloc(
sizeof(*_tmp1C2));_tmp1C2[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp1C3;_tmp1C3.tag=
19;_tmp1C3.f1=(void*)t;_tmp1C3;});_tmp1C2;}));_tmp1C1->tl=0;_tmp1C1;});_tmp1C0;});
_tmp1BF;});_tmp1BE;});*_tmp1B0=({struct Cyc_Core_Opt*_tmp1BD=_cycalloc(sizeof(*
_tmp1BD));_tmp1BD->v=(void*)((void*)_tmp1BC);_tmp1BD;});return 1;}}_LL164:;_LL165:
return 0;_LL15B:;}}static int Cyc_Tcutil_variable_in_effect(int constrain,struct Cyc_Absyn_Tvar*
v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1C4=e;struct Cyc_Absyn_Tvar*_tmp1C5;
struct Cyc_List_List*_tmp1C6;void*_tmp1C7;struct Cyc_Core_Opt*_tmp1C8;struct Cyc_Core_Opt*
_tmp1C9;struct Cyc_Core_Opt**_tmp1CA;struct Cyc_Core_Opt*_tmp1CB;_LL171: if(_tmp1C4
<= (void*)3?1:*((int*)_tmp1C4)!= 1)goto _LL173;_tmp1C5=((struct Cyc_Absyn_VarType_struct*)
_tmp1C4)->f1;_LL172: return Cyc_Absyn_tvar_cmp(v,_tmp1C5)== 0;_LL173: if(_tmp1C4 <= (
void*)3?1:*((int*)_tmp1C4)!= 18)goto _LL175;_tmp1C6=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1C4)->f1;_LL174: for(0;_tmp1C6 != 0;_tmp1C6=_tmp1C6->tl){if(Cyc_Tcutil_variable_in_effect(
constrain,v,(void*)_tmp1C6->hd))return 1;}return 0;_LL175: if(_tmp1C4 <= (void*)3?1:*((
int*)_tmp1C4)!= 19)goto _LL177;_tmp1C7=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1C4)->f1;_LL176: {void*_tmp1CC=Cyc_Tcutil_rgns_of(_tmp1C7);void*_tmp1CD;
_LL17C: if(_tmp1CC <= (void*)3?1:*((int*)_tmp1CC)!= 19)goto _LL17E;_tmp1CD=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1CC)->f1;_LL17D: if(!constrain)return 0;{void*
_tmp1CE=Cyc_Tcutil_compress(_tmp1CD);struct Cyc_Core_Opt*_tmp1CF;struct Cyc_Core_Opt*
_tmp1D0;struct Cyc_Core_Opt**_tmp1D1;struct Cyc_Core_Opt*_tmp1D2;_LL181: if(_tmp1CE
<= (void*)3?1:*((int*)_tmp1CE)!= 0)goto _LL183;_tmp1CF=((struct Cyc_Absyn_Evar_struct*)
_tmp1CE)->f1;_tmp1D0=((struct Cyc_Absyn_Evar_struct*)_tmp1CE)->f2;_tmp1D1=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1CE)->f2;_tmp1D2=((struct Cyc_Absyn_Evar_struct*)
_tmp1CE)->f4;_LL182: {void*_tmp1D3=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1D2);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D2))->v,v))return 0;{
void*_tmp1D4=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1D7;_tmp1D7.tag=18;_tmp1D7.f1=({void*_tmp1D8[2];_tmp1D8[1]=(void*)({struct
Cyc_Absyn_VarType_struct*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9[0]=({struct
Cyc_Absyn_VarType_struct _tmp1DA;_tmp1DA.tag=1;_tmp1DA.f1=v;_tmp1DA;});_tmp1D9;});
_tmp1D8[0]=_tmp1D3;Cyc_List_list(_tag_arr(_tmp1D8,sizeof(void*),2));});_tmp1D7;});
_tmp1D6;}));*_tmp1D1=({struct Cyc_Core_Opt*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));
_tmp1D5->v=(void*)_tmp1D4;_tmp1D5;});return 1;}}_LL183:;_LL184: return 0;_LL180:;}
_LL17E:;_LL17F: return Cyc_Tcutil_variable_in_effect(constrain,v,_tmp1CC);_LL17B:;}
_LL177: if(_tmp1C4 <= (void*)3?1:*((int*)_tmp1C4)!= 0)goto _LL179;_tmp1C8=((struct
Cyc_Absyn_Evar_struct*)_tmp1C4)->f1;_tmp1C9=((struct Cyc_Absyn_Evar_struct*)
_tmp1C4)->f2;_tmp1CA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1C4)->f2;_tmp1CB=((struct Cyc_Absyn_Evar_struct*)_tmp1C4)->f4;_LL178: if(
_tmp1C8 == 0?1:(void*)_tmp1C8->v != (void*)4)({void*_tmp1DB[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",
sizeof(char),27),_tag_arr(_tmp1DB,sizeof(void*),0));});{void*_tmp1DC=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1CB);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1CB))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*_tmp1DD=({struct Cyc_Absyn_JoinEff_struct*
_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E0;_tmp1E0.tag=18;_tmp1E0.f1=({struct Cyc_List_List*_tmp1E1=_cycalloc(
sizeof(*_tmp1E1));_tmp1E1->hd=(void*)_tmp1DC;_tmp1E1->tl=({struct Cyc_List_List*
_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3[0]=({struct Cyc_Absyn_VarType_struct
_tmp1E4;_tmp1E4.tag=1;_tmp1E4.f1=v;_tmp1E4;});_tmp1E3;}));_tmp1E2->tl=0;_tmp1E2;});
_tmp1E1;});_tmp1E0;});_tmp1DF;});*_tmp1CA=({struct Cyc_Core_Opt*_tmp1DE=_cycalloc(
sizeof(*_tmp1DE));_tmp1DE->v=(void*)((void*)_tmp1DD);_tmp1DE;});return 1;}}_LL179:;
_LL17A: return 0;_LL170:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=
Cyc_Tcutil_compress(e);{void*_tmp1E5=e;struct Cyc_List_List*_tmp1E6;void*_tmp1E7;
_LL186: if(_tmp1E5 <= (void*)3?1:*((int*)_tmp1E5)!= 18)goto _LL188;_tmp1E6=((struct
Cyc_Absyn_JoinEff_struct*)_tmp1E5)->f1;_LL187: for(0;_tmp1E6 != 0;_tmp1E6=_tmp1E6->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp1E6->hd))return 1;}return 0;_LL188:
if(_tmp1E5 <= (void*)3?1:*((int*)_tmp1E5)!= 19)goto _LL18A;_tmp1E7=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp1E5)->f1;_LL189: {void*_tmp1E8=Cyc_Tcutil_rgns_of(
_tmp1E7);void*_tmp1E9;_LL18F: if(_tmp1E8 <= (void*)3?1:*((int*)_tmp1E8)!= 19)goto
_LL191;_tmp1E9=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E8)->f1;_LL190:
return 0;_LL191:;_LL192: return Cyc_Tcutil_evar_in_effect(evar,_tmp1E8);_LL18E:;}
_LL18A: if(_tmp1E5 <= (void*)3?1:*((int*)_tmp1E5)!= 0)goto _LL18C;_LL18B: return evar
== e;_LL18C:;_LL18D: return 0;_LL185:;}}int Cyc_Tcutil_subset_effect(int
set_to_empty,void*e1,void*e2){void*_tmp1EA=Cyc_Tcutil_compress(e1);struct Cyc_List_List*
_tmp1EB;void*_tmp1EC;struct Cyc_Absyn_Tvar*_tmp1ED;void*_tmp1EE;struct Cyc_Core_Opt*
_tmp1EF;struct Cyc_Core_Opt**_tmp1F0;struct Cyc_Core_Opt*_tmp1F1;_LL194: if(_tmp1EA
<= (void*)3?1:*((int*)_tmp1EA)!= 18)goto _LL196;_tmp1EB=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1EA)->f1;_LL195: for(0;_tmp1EB != 0;_tmp1EB=_tmp1EB->tl){if(!Cyc_Tcutil_subset_effect(
set_to_empty,(void*)_tmp1EB->hd,e2))return 0;}return 1;_LL196: if(_tmp1EA <= (void*)
3?1:*((int*)_tmp1EA)!= 17)goto _LL198;_tmp1EC=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1EA)->f1;_LL197: return(Cyc_Tcutil_region_in_effect(0,_tmp1EC,e2)?1: Cyc_Tcutil_region_in_effect(
1,_tmp1EC,e2))?1: Cyc_Tcutil_unify(_tmp1EC,(void*)2);_LL198: if(_tmp1EA <= (void*)3?
1:*((int*)_tmp1EA)!= 1)goto _LL19A;_tmp1ED=((struct Cyc_Absyn_VarType_struct*)
_tmp1EA)->f1;_LL199: return Cyc_Tcutil_variable_in_effect(0,_tmp1ED,e2)?1: Cyc_Tcutil_variable_in_effect(
1,_tmp1ED,e2);_LL19A: if(_tmp1EA <= (void*)3?1:*((int*)_tmp1EA)!= 19)goto _LL19C;
_tmp1EE=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1EA)->f1;_LL19B: {void*
_tmp1F2=Cyc_Tcutil_rgns_of(_tmp1EE);void*_tmp1F3;_LL1A1: if(_tmp1F2 <= (void*)3?1:*((
int*)_tmp1F2)!= 19)goto _LL1A3;_tmp1F3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1F2)->f1;_LL1A2: return(Cyc_Tcutil_type_in_effect(0,_tmp1F3,e2)?1: Cyc_Tcutil_type_in_effect(
1,_tmp1F3,e2))?1: Cyc_Tcutil_unify(_tmp1F3,Cyc_Absyn_sint_t);_LL1A3:;_LL1A4:
return Cyc_Tcutil_subset_effect(set_to_empty,_tmp1F2,e2);_LL1A0:;}_LL19C: if(
_tmp1EA <= (void*)3?1:*((int*)_tmp1EA)!= 0)goto _LL19E;_tmp1EF=((struct Cyc_Absyn_Evar_struct*)
_tmp1EA)->f2;_tmp1F0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1EA)->f2;_tmp1F1=((struct Cyc_Absyn_Evar_struct*)_tmp1EA)->f4;_LL19D: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){if(set_to_empty)*_tmp1F0=({struct Cyc_Core_Opt*_tmp1F4=_cycalloc(sizeof(*
_tmp1F4));_tmp1F4->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1F5=
_cycalloc(sizeof(*_tmp1F5));_tmp1F5[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1F6;
_tmp1F6.tag=18;_tmp1F6.f1=0;_tmp1F6;});_tmp1F5;}));_tmp1F4;});else{Cyc_Tcutil_occurs(
e1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1F1))->v,e2);*_tmp1F0=({struct Cyc_Core_Opt*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));
_tmp1F7->v=(void*)e2;_tmp1F7;});}}return 1;_LL19E:;_LL19F:({struct Cyc_Std_String_pa_struct
_tmp1F9;_tmp1F9.tag=0;_tmp1F9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e1);{
void*_tmp1F8[1]={& _tmp1F9};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("subset_effect: bad effect: %s",sizeof(char),30),
_tag_arr(_tmp1F8,sizeof(void*),1));}});_LL193:;}struct _tuple11{struct _tuple10*f1;
struct _tuple10*f2;};static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);{struct _tuple11 _tmp1FB=({struct _tuple11
_tmp1FA;_tmp1FA.f1=Cyc_Tcutil_get_effect_evar(e1);_tmp1FA.f2=Cyc_Tcutil_get_effect_evar(
e2);_tmp1FA;});_LL1A6:;_LL1A7: return Cyc_Tcutil_subset_effect(0,e1,e2)?Cyc_Tcutil_subset_effect(
0,e2,e1): 0;_LL1A5:;}}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple6 _tmp1FD;void*_tmp1FE;void*_tmp1FF;struct _tuple6*_tmp1FC=(struct
_tuple6*)r1->hd;_tmp1FD=*_tmp1FC;_tmp1FE=_tmp1FD.f1;_tmp1FF=_tmp1FD.f2;{int found=
_tmp1FE == (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0?!found: 0;r2=r2->tl){
struct _tuple6 _tmp201;void*_tmp202;void*_tmp203;struct _tuple6*_tmp200=(struct
_tuple6*)r2->hd;_tmp201=*_tmp200;_tmp202=_tmp201.f1;_tmp203=_tmp201.f2;if(Cyc_Tcutil_unify(
_tmp1FE,_tmp202)?Cyc_Tcutil_unify(_tmp1FF,_tmp203): 0){found=1;break;}}}if(!found)
return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)?Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1): 0;}struct _tuple12{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple13{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _tagged_arr)_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp204=t1;struct Cyc_Core_Opt*_tmp205;struct Cyc_Core_Opt*
_tmp206;struct Cyc_Core_Opt**_tmp207;struct Cyc_Core_Opt*_tmp208;_LL1A9: if(_tmp204
<= (void*)3?1:*((int*)_tmp204)!= 0)goto _LL1AB;_tmp205=((struct Cyc_Absyn_Evar_struct*)
_tmp204)->f1;_tmp206=((struct Cyc_Absyn_Evar_struct*)_tmp204)->f2;_tmp207=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp204)->f2;_tmp208=((struct Cyc_Absyn_Evar_struct*)
_tmp204)->f4;_LL1AA: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp208))->v,t2);{void*_tmp209=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp209,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp205))->v)){*_tmp207=({struct Cyc_Core_Opt*_tmp20A=_cycalloc(sizeof(*_tmp20A));
_tmp20A->v=(void*)t2;_tmp20A;});return;}else{{void*_tmp20B=t2;struct Cyc_Core_Opt*
_tmp20C;struct Cyc_Core_Opt**_tmp20D;struct Cyc_Core_Opt*_tmp20E;struct Cyc_Absyn_PtrInfo
_tmp20F;_LL1AE: if(_tmp20B <= (void*)3?1:*((int*)_tmp20B)!= 0)goto _LL1B0;_tmp20C=((
struct Cyc_Absyn_Evar_struct*)_tmp20B)->f2;_tmp20D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp20B)->f2;_tmp20E=((struct Cyc_Absyn_Evar_struct*)
_tmp20B)->f4;_LL1AF: {struct Cyc_List_List*_tmp210=(struct Cyc_List_List*)_tmp208->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp20E))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp210,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=
_tag_arr("(type variable would escape scope)",sizeof(char),35);(int)_throw((void*)
Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)_tmp205->v,_tmp209)){*_tmp20D=({
struct Cyc_Core_Opt*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->v=(void*)t1;
_tmp211;});return;}Cyc_Tcutil_failure_reason=_tag_arr("(kinds are incompatible)",
sizeof(char),25);goto _LL1AD;}_LL1B0: if(_tmp20B <= (void*)3?1:*((int*)_tmp20B)!= 4)
goto _LL1B2;_tmp20F=((struct Cyc_Absyn_PointerType_struct*)_tmp20B)->f1;if(!((void*)
_tmp205->v == (void*)2))goto _LL1B2;_LL1B1: {struct Cyc_Absyn_Conref*_tmp212=Cyc_Absyn_compress_conref(
_tmp20F.bounds);{void*_tmp213=(void*)_tmp212->v;_LL1B5: if((int)_tmp213 != 0)goto
_LL1B7;_LL1B6:(void*)(_tmp212->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp214=_cycalloc(sizeof(*_tmp214));_tmp214[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp215;_tmp215.tag=0;_tmp215.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp217;_tmp217.tag=0;_tmp217.f1=Cyc_Absyn_signed_int_exp(1,0);_tmp217;});
_tmp216;}));_tmp215;});_tmp214;})));*_tmp207=({struct Cyc_Core_Opt*_tmp218=
_cycalloc(sizeof(*_tmp218));_tmp218->v=(void*)t2;_tmp218;});return;_LL1B7:;
_LL1B8: goto _LL1B4;_LL1B4:;}goto _LL1AD;}_LL1B2:;_LL1B3: goto _LL1AD;_LL1AD:;}Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}
_LL1AB:;_LL1AC: goto _LL1A8;_LL1A8:;}{struct _tuple6 _tmp21A=({struct _tuple6 _tmp219;
_tmp219.f1=t2;_tmp219.f2=t1;_tmp219;});void*_tmp21B;void*_tmp21C;void*_tmp21D;
void*_tmp21E;struct Cyc_Absyn_Tvar*_tmp21F;void*_tmp220;struct Cyc_Absyn_Tvar*
_tmp221;void*_tmp222;struct Cyc_Absyn_AggrInfo _tmp223;void*_tmp224;struct Cyc_List_List*
_tmp225;void*_tmp226;struct Cyc_Absyn_AggrInfo _tmp227;void*_tmp228;struct Cyc_List_List*
_tmp229;void*_tmp22A;struct _tuple1*_tmp22B;void*_tmp22C;struct _tuple1*_tmp22D;
void*_tmp22E;struct Cyc_List_List*_tmp22F;void*_tmp230;struct Cyc_List_List*
_tmp231;void*_tmp232;struct Cyc_Absyn_TunionInfo _tmp233;void*_tmp234;struct Cyc_Absyn_Tuniondecl**
_tmp235;struct Cyc_Absyn_Tuniondecl*_tmp236;struct Cyc_List_List*_tmp237;void*
_tmp238;void*_tmp239;struct Cyc_Absyn_TunionInfo _tmp23A;void*_tmp23B;struct Cyc_Absyn_Tuniondecl**
_tmp23C;struct Cyc_Absyn_Tuniondecl*_tmp23D;struct Cyc_List_List*_tmp23E;void*
_tmp23F;void*_tmp240;struct Cyc_Absyn_TunionFieldInfo _tmp241;void*_tmp242;struct
Cyc_Absyn_Tuniondecl*_tmp243;struct Cyc_Absyn_Tunionfield*_tmp244;struct Cyc_List_List*
_tmp245;void*_tmp246;struct Cyc_Absyn_TunionFieldInfo _tmp247;void*_tmp248;struct
Cyc_Absyn_Tuniondecl*_tmp249;struct Cyc_Absyn_Tunionfield*_tmp24A;struct Cyc_List_List*
_tmp24B;void*_tmp24C;struct Cyc_Absyn_PtrInfo _tmp24D;void*_tmp24E;void*_tmp24F;
struct Cyc_Absyn_Conref*_tmp250;struct Cyc_Absyn_Tqual _tmp251;struct Cyc_Absyn_Conref*
_tmp252;void*_tmp253;struct Cyc_Absyn_PtrInfo _tmp254;void*_tmp255;void*_tmp256;
struct Cyc_Absyn_Conref*_tmp257;struct Cyc_Absyn_Tqual _tmp258;struct Cyc_Absyn_Conref*
_tmp259;void*_tmp25A;void*_tmp25B;void*_tmp25C;void*_tmp25D;void*_tmp25E;void*
_tmp25F;void*_tmp260;void*_tmp261;void*_tmp262;int _tmp263;void*_tmp264;int
_tmp265;void*_tmp266;void*_tmp267;void*_tmp268;void*_tmp269;void*_tmp26A;void*
_tmp26B;struct Cyc_Absyn_Tqual _tmp26C;struct Cyc_Absyn_Exp*_tmp26D;void*_tmp26E;
void*_tmp26F;struct Cyc_Absyn_Tqual _tmp270;struct Cyc_Absyn_Exp*_tmp271;void*
_tmp272;struct Cyc_Absyn_FnInfo _tmp273;struct Cyc_List_List*_tmp274;struct Cyc_Core_Opt*
_tmp275;void*_tmp276;struct Cyc_List_List*_tmp277;int _tmp278;struct Cyc_Absyn_VarargInfo*
_tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_List_List*_tmp27B;void*_tmp27C;
struct Cyc_Absyn_FnInfo _tmp27D;struct Cyc_List_List*_tmp27E;struct Cyc_Core_Opt*
_tmp27F;void*_tmp280;struct Cyc_List_List*_tmp281;int _tmp282;struct Cyc_Absyn_VarargInfo*
_tmp283;struct Cyc_List_List*_tmp284;struct Cyc_List_List*_tmp285;void*_tmp286;
struct Cyc_List_List*_tmp287;void*_tmp288;struct Cyc_List_List*_tmp289;void*
_tmp28A;void*_tmp28B;struct Cyc_List_List*_tmp28C;void*_tmp28D;void*_tmp28E;
struct Cyc_List_List*_tmp28F;void*_tmp290;void*_tmp291;void*_tmp292;void*_tmp293;
void*_tmp294;void*_tmp295;void*_tmp296;void*_tmp297;void*_tmp298;void*_tmp299;
void*_tmp29A;void*_tmp29B;_LL1BA: _tmp21B=_tmp21A.f1;if(_tmp21B <= (void*)3?1:*((
int*)_tmp21B)!= 0)goto _LL1BC;_LL1BB: Cyc_Tcutil_unify_it(t2,t1);return;_LL1BC:
_tmp21C=_tmp21A.f1;if((int)_tmp21C != 0)goto _LL1BE;_tmp21D=_tmp21A.f2;if((int)
_tmp21D != 0)goto _LL1BE;_LL1BD: return;_LL1BE: _tmp21E=_tmp21A.f1;if(_tmp21E <= (
void*)3?1:*((int*)_tmp21E)!= 1)goto _LL1C0;_tmp21F=((struct Cyc_Absyn_VarType_struct*)
_tmp21E)->f1;_tmp220=_tmp21A.f2;if(_tmp220 <= (void*)3?1:*((int*)_tmp220)!= 1)
goto _LL1C0;_tmp221=((struct Cyc_Absyn_VarType_struct*)_tmp220)->f1;_LL1BF: {
struct _tagged_arr*_tmp29C=_tmp21F->name;struct _tagged_arr*_tmp29D=_tmp221->name;
int _tmp29E=*((int*)_check_null(_tmp21F->identity));int _tmp29F=*((int*)
_check_null(_tmp221->identity));void*_tmp2A0=Cyc_Tcutil_tvar_kind(_tmp21F);void*
_tmp2A1=Cyc_Tcutil_tvar_kind(_tmp221);if(_tmp29F == _tmp29E?Cyc_Std_zstrptrcmp(
_tmp29C,_tmp29D)== 0: 0){if(_tmp2A0 != _tmp2A1)({struct Cyc_Std_String_pa_struct
_tmp2A5;_tmp2A5.tag=0;_tmp2A5.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2A1);{struct Cyc_Std_String_pa_struct _tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2A0);{struct Cyc_Std_String_pa_struct
_tmp2A3;_tmp2A3.tag=0;_tmp2A3.f1=(struct _tagged_arr)*_tmp29C;{void*_tmp2A2[3]={&
_tmp2A3,& _tmp2A4,& _tmp2A5};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("same type variable %s has kinds %s and %s",sizeof(
char),42),_tag_arr(_tmp2A2,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=
_tag_arr("(variable types are not the same)",sizeof(char),34);goto _LL1B9;}_LL1C0:
_tmp222=_tmp21A.f1;if(_tmp222 <= (void*)3?1:*((int*)_tmp222)!= 10)goto _LL1C2;
_tmp223=((struct Cyc_Absyn_AggrType_struct*)_tmp222)->f1;_tmp224=(void*)_tmp223.aggr_info;
_tmp225=_tmp223.targs;_tmp226=_tmp21A.f2;if(_tmp226 <= (void*)3?1:*((int*)_tmp226)
!= 10)goto _LL1C2;_tmp227=((struct Cyc_Absyn_AggrType_struct*)_tmp226)->f1;_tmp228=(
void*)_tmp227.aggr_info;_tmp229=_tmp227.targs;_LL1C1: {void*_tmp2A7;struct
_tuple1*_tmp2A8;struct _tuple5 _tmp2A6=Cyc_Absyn_aggr_kinded_name(_tmp228);_tmp2A7=
_tmp2A6.f1;_tmp2A8=_tmp2A6.f2;{void*_tmp2AA;struct _tuple1*_tmp2AB;struct _tuple5
_tmp2A9=Cyc_Absyn_aggr_kinded_name(_tmp224);_tmp2AA=_tmp2A9.f1;_tmp2AB=_tmp2A9.f2;
if(_tmp2A7 != _tmp2AA){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1B9;}if(Cyc_Absyn_qvar_cmp(_tmp2A8,_tmp2AB)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different type name)",sizeof(char),22);goto _LL1B9;}Cyc_Tcutil_unify_list(
_tmp229,_tmp225);return;}}_LL1C2: _tmp22A=_tmp21A.f1;if(_tmp22A <= (void*)3?1:*((
int*)_tmp22A)!= 12)goto _LL1C4;_tmp22B=((struct Cyc_Absyn_EnumType_struct*)_tmp22A)->f1;
_tmp22C=_tmp21A.f2;if(_tmp22C <= (void*)3?1:*((int*)_tmp22C)!= 12)goto _LL1C4;
_tmp22D=((struct Cyc_Absyn_EnumType_struct*)_tmp22C)->f1;_LL1C3: if(Cyc_Absyn_qvar_cmp(
_tmp22B,_tmp22D)== 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different enum types)",
sizeof(char),23);goto _LL1B9;_LL1C4: _tmp22E=_tmp21A.f1;if(_tmp22E <= (void*)3?1:*((
int*)_tmp22E)!= 13)goto _LL1C6;_tmp22F=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp22E)->f1;_tmp230=_tmp21A.f2;if(_tmp230 <= (void*)3?1:*((int*)_tmp230)!= 13)
goto _LL1C6;_tmp231=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp230)->f1;_LL1C5: {
int bad=0;for(0;_tmp22F != 0?_tmp231 != 0: 0;(_tmp22F=_tmp22F->tl,_tmp231=_tmp231->tl)){
struct Cyc_Absyn_Enumfield*_tmp2AC=(struct Cyc_Absyn_Enumfield*)_tmp22F->hd;struct
Cyc_Absyn_Enumfield*_tmp2AD=(struct Cyc_Absyn_Enumfield*)_tmp231->hd;if(Cyc_Absyn_qvar_cmp(
_tmp2AC->name,_tmp2AD->name)!= 0){Cyc_Tcutil_failure_reason=_tag_arr("(different names for enum fields)",
sizeof(char),34);bad=1;break;}if(_tmp2AC->tag == _tmp2AD->tag)continue;if(_tmp2AC->tag
== 0?1: _tmp2AD->tag == 0){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(char),39);bad=1;break;}if(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp2AC->tag))!= Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp2AD->tag))){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(char),39);bad=1;break;}}if(bad)goto _LL1B9;if(_tmp22F == 0?_tmp231 == 0: 0)
return;Cyc_Tcutil_failure_reason=_tag_arr("(different number of fields for enums)",
sizeof(char),39);goto _LL1B9;}_LL1C6: _tmp232=_tmp21A.f1;if(_tmp232 <= (void*)3?1:*((
int*)_tmp232)!= 2)goto _LL1C8;_tmp233=((struct Cyc_Absyn_TunionType_struct*)
_tmp232)->f1;_tmp234=(void*)_tmp233.tunion_info;if(*((int*)_tmp234)!= 1)goto
_LL1C8;_tmp235=((struct Cyc_Absyn_KnownTunion_struct*)_tmp234)->f1;_tmp236=*
_tmp235;_tmp237=_tmp233.targs;_tmp238=(void*)_tmp233.rgn;_tmp239=_tmp21A.f2;if(
_tmp239 <= (void*)3?1:*((int*)_tmp239)!= 2)goto _LL1C8;_tmp23A=((struct Cyc_Absyn_TunionType_struct*)
_tmp239)->f1;_tmp23B=(void*)_tmp23A.tunion_info;if(*((int*)_tmp23B)!= 1)goto
_LL1C8;_tmp23C=((struct Cyc_Absyn_KnownTunion_struct*)_tmp23B)->f1;_tmp23D=*
_tmp23C;_tmp23E=_tmp23A.targs;_tmp23F=(void*)_tmp23A.rgn;_LL1C7: if(_tmp236 == 
_tmp23D?1: Cyc_Absyn_qvar_cmp(_tmp236->name,_tmp23D->name)== 0){Cyc_Tcutil_unify_it(
_tmp23F,_tmp238);Cyc_Tcutil_unify_list(_tmp23E,_tmp237);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion types)",sizeof(char),25);goto _LL1B9;_LL1C8: _tmp240=
_tmp21A.f1;if(_tmp240 <= (void*)3?1:*((int*)_tmp240)!= 3)goto _LL1CA;_tmp241=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp240)->f1;_tmp242=(void*)_tmp241.field_info;
if(*((int*)_tmp242)!= 1)goto _LL1CA;_tmp243=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp242)->f1;_tmp244=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp242)->f2;
_tmp245=_tmp241.targs;_tmp246=_tmp21A.f2;if(_tmp246 <= (void*)3?1:*((int*)_tmp246)
!= 3)goto _LL1CA;_tmp247=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp246)->f1;
_tmp248=(void*)_tmp247.field_info;if(*((int*)_tmp248)!= 1)goto _LL1CA;_tmp249=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp248)->f1;_tmp24A=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp248)->f2;_tmp24B=_tmp247.targs;_LL1C9: if((_tmp243 == _tmp249?1: Cyc_Absyn_qvar_cmp(
_tmp243->name,_tmp249->name)== 0)?_tmp244 == _tmp24A?1: Cyc_Absyn_qvar_cmp(_tmp244->name,
_tmp24A->name)== 0: 0){Cyc_Tcutil_unify_list(_tmp24B,_tmp245);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)",sizeof(char),31);goto _LL1B9;_LL1CA:
_tmp24C=_tmp21A.f1;if(_tmp24C <= (void*)3?1:*((int*)_tmp24C)!= 4)goto _LL1CC;
_tmp24D=((struct Cyc_Absyn_PointerType_struct*)_tmp24C)->f1;_tmp24E=(void*)
_tmp24D.elt_typ;_tmp24F=(void*)_tmp24D.rgn_typ;_tmp250=_tmp24D.nullable;_tmp251=
_tmp24D.tq;_tmp252=_tmp24D.bounds;_tmp253=_tmp21A.f2;if(_tmp253 <= (void*)3?1:*((
int*)_tmp253)!= 4)goto _LL1CC;_tmp254=((struct Cyc_Absyn_PointerType_struct*)
_tmp253)->f1;_tmp255=(void*)_tmp254.elt_typ;_tmp256=(void*)_tmp254.rgn_typ;
_tmp257=_tmp254.nullable;_tmp258=_tmp254.tq;_tmp259=_tmp254.bounds;_LL1CB: Cyc_Tcutil_unify_it(
_tmp255,_tmp24E);Cyc_Tcutil_unify_it(_tmp24F,_tmp256);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_unify_tqual(_tmp258,_tmp251);Cyc_Tcutil_unify_it_conrefs(
Cyc_Tcutil_boundscmp,_tmp259,_tmp252,_tag_arr("(different pointer bounds)",
sizeof(char),27));{void*_tmp2AE=(void*)(Cyc_Absyn_compress_conref(_tmp259))->v;
void*_tmp2AF;_LL1EF: if(_tmp2AE <= (void*)1?1:*((int*)_tmp2AE)!= 0)goto _LL1F1;
_tmp2AF=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2AE)->f1;if((int)_tmp2AF
!= 0)goto _LL1F1;_LL1F0: return;_LL1F1:;_LL1F2: goto _LL1EE;_LL1EE:;}((void(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp257,_tmp250,
_tag_arr("(different pointer types)",sizeof(char),26));return;_LL1CC: _tmp25A=
_tmp21A.f1;if(_tmp25A <= (void*)3?1:*((int*)_tmp25A)!= 5)goto _LL1CE;_tmp25B=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp25A)->f1;_tmp25C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp25A)->f2;_tmp25D=_tmp21A.f2;if(_tmp25D <= (void*)3?1:*((int*)_tmp25D)!= 5)
goto _LL1CE;_tmp25E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp25D)->f1;_tmp25F=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp25D)->f2;_LL1CD: if(_tmp25E == _tmp25B?
_tmp25F == _tmp25C: 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different integral types)",
sizeof(char),27);goto _LL1B9;_LL1CE: _tmp260=_tmp21A.f1;if((int)_tmp260 != 1)goto
_LL1D0;_tmp261=_tmp21A.f2;if((int)_tmp261 != 1)goto _LL1D0;_LL1CF: return;_LL1D0:
_tmp262=_tmp21A.f1;if(_tmp262 <= (void*)3?1:*((int*)_tmp262)!= 6)goto _LL1D2;
_tmp263=((struct Cyc_Absyn_DoubleType_struct*)_tmp262)->f1;_tmp264=_tmp21A.f2;if(
_tmp264 <= (void*)3?1:*((int*)_tmp264)!= 6)goto _LL1D2;_tmp265=((struct Cyc_Absyn_DoubleType_struct*)
_tmp264)->f1;_LL1D1: if(_tmp263 == _tmp265)return;goto _LL1B9;_LL1D2: _tmp266=
_tmp21A.f1;if(_tmp266 <= (void*)3?1:*((int*)_tmp266)!= 14)goto _LL1D4;_tmp267=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp266)->f1;_tmp268=_tmp21A.f2;if(
_tmp268 <= (void*)3?1:*((int*)_tmp268)!= 14)goto _LL1D4;_tmp269=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp268)->f1;_LL1D3: Cyc_Tcutil_unify_it(_tmp267,_tmp269);return;_LL1D4: _tmp26A=
_tmp21A.f1;if(_tmp26A <= (void*)3?1:*((int*)_tmp26A)!= 7)goto _LL1D6;_tmp26B=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp26A)->f1;_tmp26C=((struct Cyc_Absyn_ArrayType_struct*)
_tmp26A)->f2;_tmp26D=((struct Cyc_Absyn_ArrayType_struct*)_tmp26A)->f3;_tmp26E=
_tmp21A.f2;if(_tmp26E <= (void*)3?1:*((int*)_tmp26E)!= 7)goto _LL1D6;_tmp26F=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp26E)->f1;_tmp270=((struct Cyc_Absyn_ArrayType_struct*)
_tmp26E)->f2;_tmp271=((struct Cyc_Absyn_ArrayType_struct*)_tmp26E)->f3;_LL1D5: Cyc_Tcutil_unify_tqual(
_tmp270,_tmp26C);Cyc_Tcutil_unify_it(_tmp26F,_tmp26B);if(_tmp26D == _tmp271)
return;if(_tmp26D == 0?1: _tmp271 == 0)goto _LL1B9;if(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp26D))== Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp271)))return;Cyc_Tcutil_failure_reason=
_tag_arr("(different array sizes)",sizeof(char),24);goto _LL1B9;_LL1D6: _tmp272=
_tmp21A.f1;if(_tmp272 <= (void*)3?1:*((int*)_tmp272)!= 8)goto _LL1D8;_tmp273=((
struct Cyc_Absyn_FnType_struct*)_tmp272)->f1;_tmp274=_tmp273.tvars;_tmp275=
_tmp273.effect;_tmp276=(void*)_tmp273.ret_typ;_tmp277=_tmp273.args;_tmp278=
_tmp273.c_varargs;_tmp279=_tmp273.cyc_varargs;_tmp27A=_tmp273.rgn_po;_tmp27B=
_tmp273.attributes;_tmp27C=_tmp21A.f2;if(_tmp27C <= (void*)3?1:*((int*)_tmp27C)!= 
8)goto _LL1D8;_tmp27D=((struct Cyc_Absyn_FnType_struct*)_tmp27C)->f1;_tmp27E=
_tmp27D.tvars;_tmp27F=_tmp27D.effect;_tmp280=(void*)_tmp27D.ret_typ;_tmp281=
_tmp27D.args;_tmp282=_tmp27D.c_varargs;_tmp283=_tmp27D.cyc_varargs;_tmp284=
_tmp27D.rgn_po;_tmp285=_tmp27D.attributes;_LL1D7: {int done=0;{struct
_RegionHandle _tmp2B0=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2B0;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp27E != 0){if(_tmp274 == 0){
Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too few type variables)",
sizeof(char),50);(int)_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp2B1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp27E->hd);void*_tmp2B2=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp274->hd);if(_tmp2B1 != _tmp2B2){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp2B6;_tmp2B6.tag=0;_tmp2B6.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2B2);{struct Cyc_Std_String_pa_struct
_tmp2B5;_tmp2B5.tag=0;_tmp2B5.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2B1);{struct Cyc_Std_String_pa_struct _tmp2B4;_tmp2B4.tag=0;_tmp2B4.f1=(struct
_tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp27E->hd);{void*
_tmp2B3[3]={& _tmp2B4,& _tmp2B5,& _tmp2B6};Cyc_Std_aprintf(_tag_arr("(type var %s has different kinds %s and %s)",
sizeof(char),44),_tag_arr(_tmp2B3,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp2B7=_region_malloc(rgn,sizeof(*_tmp2B7));_tmp2B7->hd=({
struct _tuple7*_tmp2B8=_region_malloc(rgn,sizeof(*_tmp2B8));_tmp2B8->f1=(struct
Cyc_Absyn_Tvar*)_tmp274->hd;_tmp2B8->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9[0]=({struct Cyc_Absyn_VarType_struct
_tmp2BA;_tmp2BA.tag=1;_tmp2BA.f1=(struct Cyc_Absyn_Tvar*)_tmp27E->hd;_tmp2BA;});
_tmp2B9;});_tmp2B8;});_tmp2B7->tl=inst;_tmp2B7;});_tmp27E=_tmp27E->tl;_tmp274=
_tmp274->tl;}}if(_tmp274 != 0){Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too many type variables)",
sizeof(char),51);_npop_handler(0);goto _LL1B9;}if(inst != 0){Cyc_Tcutil_unify_it((
void*)({struct Cyc_Absyn_FnType_struct*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));
_tmp2BB[0]=({struct Cyc_Absyn_FnType_struct _tmp2BC;_tmp2BC.tag=8;_tmp2BC.f1=({
struct Cyc_Absyn_FnInfo _tmp2BD;_tmp2BD.tvars=0;_tmp2BD.effect=_tmp27F;_tmp2BD.ret_typ=(
void*)_tmp280;_tmp2BD.args=_tmp281;_tmp2BD.c_varargs=_tmp282;_tmp2BD.cyc_varargs=
_tmp283;_tmp2BD.rgn_po=_tmp284;_tmp2BD.attributes=_tmp285;_tmp2BD;});_tmp2BC;});
_tmp2BB;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE[0]=({struct Cyc_Absyn_FnType_struct
_tmp2BF;_tmp2BF.tag=8;_tmp2BF.f1=({struct Cyc_Absyn_FnInfo _tmp2C0;_tmp2C0.tvars=0;
_tmp2C0.effect=_tmp275;_tmp2C0.ret_typ=(void*)_tmp276;_tmp2C0.args=_tmp277;
_tmp2C0.c_varargs=_tmp278;_tmp2C0.cyc_varargs=_tmp279;_tmp2C0.rgn_po=_tmp27A;
_tmp2C0.attributes=_tmp27B;_tmp2C0;});_tmp2BF;});_tmp2BE;})));done=1;}};
_pop_region(rgn);}if(done)return;Cyc_Tcutil_unify_it(_tmp280,_tmp276);for(0;
_tmp281 != 0?_tmp277 != 0: 0;(_tmp281=_tmp281->tl,_tmp277=_tmp277->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)_tmp281->hd)).f2,(*((struct _tuple2*)_tmp277->hd)).f2);Cyc_Tcutil_unify_it((*((
struct _tuple2*)_tmp281->hd)).f3,(*((struct _tuple2*)_tmp277->hd)).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp281 != 0?1: _tmp277 != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)",sizeof(char),52);
goto _LL1B9;}if(_tmp282 != _tmp278){Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes C varargs)",
sizeof(char),41);goto _LL1B9;}{int bad_cyc_vararg=0;{struct _tuple12 _tmp2C2=({
struct _tuple12 _tmp2C1;_tmp2C1.f1=_tmp283;_tmp2C1.f2=_tmp279;_tmp2C1;});struct Cyc_Absyn_VarargInfo*
_tmp2C3;struct Cyc_Absyn_VarargInfo*_tmp2C4;struct Cyc_Absyn_VarargInfo*_tmp2C5;
struct Cyc_Absyn_VarargInfo*_tmp2C6;struct Cyc_Absyn_VarargInfo*_tmp2C7;struct Cyc_Absyn_VarargInfo
_tmp2C8;struct Cyc_Core_Opt*_tmp2C9;struct Cyc_Absyn_Tqual _tmp2CA;void*_tmp2CB;int
_tmp2CC;struct Cyc_Absyn_VarargInfo*_tmp2CD;struct Cyc_Absyn_VarargInfo _tmp2CE;
struct Cyc_Core_Opt*_tmp2CF;struct Cyc_Absyn_Tqual _tmp2D0;void*_tmp2D1;int _tmp2D2;
_LL1F4: _tmp2C3=_tmp2C2.f1;if(_tmp2C3 != 0)goto _LL1F6;_tmp2C4=_tmp2C2.f2;if(
_tmp2C4 != 0)goto _LL1F6;_LL1F5: goto _LL1F3;_LL1F6: _tmp2C5=_tmp2C2.f1;if(_tmp2C5 != 
0)goto _LL1F8;_LL1F7: goto _LL1F9;_LL1F8: _tmp2C6=_tmp2C2.f2;if(_tmp2C6 != 0)goto
_LL1FA;_LL1F9: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes varargs)",
sizeof(char),39);goto _LL1F3;_LL1FA: _tmp2C7=_tmp2C2.f1;if(_tmp2C7 == 0)goto _LL1F3;
_tmp2C8=*_tmp2C7;_tmp2C9=_tmp2C8.name;_tmp2CA=_tmp2C8.tq;_tmp2CB=(void*)_tmp2C8.type;
_tmp2CC=_tmp2C8.inject;_tmp2CD=_tmp2C2.f2;if(_tmp2CD == 0)goto _LL1F3;_tmp2CE=*
_tmp2CD;_tmp2CF=_tmp2CE.name;_tmp2D0=_tmp2CE.tq;_tmp2D1=(void*)_tmp2CE.type;
_tmp2D2=_tmp2CE.inject;_LL1FB: Cyc_Tcutil_unify_tqual(_tmp2CA,_tmp2D0);Cyc_Tcutil_unify_it(
_tmp2CB,_tmp2D1);if(_tmp2CC != _tmp2D2){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type injects varargs)",sizeof(char),41);}goto _LL1F3;
_LL1F3:;}if(bad_cyc_vararg)goto _LL1B9;{int bad_effect=0;{struct _tuple13 _tmp2D4=({
struct _tuple13 _tmp2D3;_tmp2D3.f1=_tmp27F;_tmp2D3.f2=_tmp275;_tmp2D3;});struct Cyc_Core_Opt*
_tmp2D5;struct Cyc_Core_Opt*_tmp2D6;struct Cyc_Core_Opt*_tmp2D7;struct Cyc_Core_Opt*
_tmp2D8;_LL1FD: _tmp2D5=_tmp2D4.f1;if(_tmp2D5 != 0)goto _LL1FF;_tmp2D6=_tmp2D4.f2;
if(_tmp2D6 != 0)goto _LL1FF;_LL1FE: goto _LL1FC;_LL1FF: _tmp2D7=_tmp2D4.f1;if(_tmp2D7
!= 0)goto _LL201;_LL200: goto _LL202;_LL201: _tmp2D8=_tmp2D4.f2;if(_tmp2D8 != 0)goto
_LL203;_LL202: bad_effect=1;goto _LL1FC;_LL203:;_LL204: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp27F))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp275))->v);goto _LL1FC;_LL1FC:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=_tag_arr("(function type effects do not unify)",
sizeof(char),37);goto _LL1B9;}if(!Cyc_Tcutil_same_atts(_tmp27B,_tmp285)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different attributes)",sizeof(char),43);goto
_LL1B9;}if(!Cyc_Tcutil_same_rgn_po(_tmp27A,_tmp284)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different region lifetime orderings)",sizeof(char),
58);goto _LL1B9;}return;}}}_LL1D8: _tmp286=_tmp21A.f1;if(_tmp286 <= (void*)3?1:*((
int*)_tmp286)!= 9)goto _LL1DA;_tmp287=((struct Cyc_Absyn_TupleType_struct*)_tmp286)->f1;
_tmp288=_tmp21A.f2;if(_tmp288 <= (void*)3?1:*((int*)_tmp288)!= 9)goto _LL1DA;
_tmp289=((struct Cyc_Absyn_TupleType_struct*)_tmp288)->f1;_LL1D9: for(0;_tmp289 != 
0?_tmp287 != 0: 0;(_tmp289=_tmp289->tl,_tmp287=_tmp287->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)_tmp289->hd)).f1,(*((struct _tuple4*)_tmp287->hd)).f1);Cyc_Tcutil_unify_it((*((
struct _tuple4*)_tmp289->hd)).f2,(*((struct _tuple4*)_tmp287->hd)).f2);}if(_tmp289
== 0?_tmp287 == 0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(tuple types have different numbers of components)",sizeof(char),51);
goto _LL1B9;_LL1DA: _tmp28A=_tmp21A.f1;if(_tmp28A <= (void*)3?1:*((int*)_tmp28A)!= 
11)goto _LL1DC;_tmp28B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp28A)->f1;
_tmp28C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp28A)->f2;_tmp28D=_tmp21A.f2;
if(_tmp28D <= (void*)3?1:*((int*)_tmp28D)!= 11)goto _LL1DC;_tmp28E=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp28D)->f1;_tmp28F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp28D)->f2;_LL1DB: if(_tmp28E != _tmp28B){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1B9;}for(0;_tmp28F != 0?_tmp28C != 0: 0;(_tmp28F=_tmp28F->tl,
_tmp28C=_tmp28C->tl)){struct Cyc_Absyn_Aggrfield*_tmp2D9=(struct Cyc_Absyn_Aggrfield*)
_tmp28F->hd;struct Cyc_Absyn_Aggrfield*_tmp2DA=(struct Cyc_Absyn_Aggrfield*)
_tmp28C->hd;if(Cyc_Std_zstrptrcmp(_tmp2D9->name,_tmp2DA->name)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different member names)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual(_tmp2D9->tq,_tmp2DA->tq);Cyc_Tcutil_unify_it((void*)
_tmp2D9->type,(void*)_tmp2DA->type);if(!Cyc_Tcutil_same_atts(_tmp2D9->attributes,
_tmp2DA->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different attributes on member)",sizeof(char),33);(int)_throw((void*)
Cyc_Tcutil_Unify);}if(((_tmp2D9->width != 0?_tmp2DA->width == 0: 0)?1:(_tmp2DA->width
!= 0?_tmp2D9->width == 0: 0))?1:((_tmp2D9->width != 0?_tmp2DA->width != 0: 0)?Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2D9->width))!= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2DA->width)): 0)){Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=_tag_arr("(different bitfield widths on member)",
sizeof(char),38);(int)_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp28F == 0?_tmp28C == 
0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of members)",sizeof(char),30);goto _LL1B9;_LL1DC:
_tmp290=_tmp21A.f1;if((int)_tmp290 != 2)goto _LL1DE;_tmp291=_tmp21A.f2;if((int)
_tmp291 != 2)goto _LL1DE;_LL1DD: return;_LL1DE: _tmp292=_tmp21A.f1;if(_tmp292 <= (
void*)3?1:*((int*)_tmp292)!= 15)goto _LL1E0;_tmp293=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp292)->f1;_tmp294=_tmp21A.f2;if(_tmp294 <= (void*)3?1:*((int*)_tmp294)!= 15)
goto _LL1E0;_tmp295=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp294)->f1;
_LL1DF: Cyc_Tcutil_unify_it(_tmp293,_tmp295);return;_LL1E0: _tmp296=_tmp21A.f1;if(
_tmp296 <= (void*)3?1:*((int*)_tmp296)!= 18)goto _LL1E2;_LL1E1: goto _LL1E3;_LL1E2:
_tmp297=_tmp21A.f2;if(_tmp297 <= (void*)3?1:*((int*)_tmp297)!= 18)goto _LL1E4;
_LL1E3: goto _LL1E5;_LL1E4: _tmp298=_tmp21A.f1;if(_tmp298 <= (void*)3?1:*((int*)
_tmp298)!= 17)goto _LL1E6;_LL1E5: goto _LL1E7;_LL1E6: _tmp299=_tmp21A.f1;if(_tmp299
<= (void*)3?1:*((int*)_tmp299)!= 19)goto _LL1E8;_LL1E7: goto _LL1E9;_LL1E8: _tmp29A=
_tmp21A.f2;if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 19)goto _LL1EA;_LL1E9: goto
_LL1EB;_LL1EA: _tmp29B=_tmp21A.f2;if(_tmp29B <= (void*)3?1:*((int*)_tmp29B)!= 17)
goto _LL1EC;_LL1EB: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)",sizeof(char),22);goto _LL1B9;_LL1EC:;_LL1ED: goto
_LL1B9;_LL1B9:;}(int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_opt_cmp(int(*
cmp)(void*,void*),struct Cyc_Core_Opt*a1,struct Cyc_Core_Opt*a2){if(a1 == a2)return
0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0: 0)return 1;return cmp((void*)((
struct Cyc_Core_Opt*)_check_null(a1))->v,(void*)((struct Cyc_Core_Opt*)_check_null(
a2))->v);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == 
a2)return 0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0: 0)return 1;return cmp((
void*)_check_null(a1),(void*)_check_null(a2));}int Cyc_Tcutil_list_cmp(int(*cmp)(
void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)return 0;
for(0;l1 != 0?l2 != 0: 0;(l1=l1->tl,l2=l2->tl)){int _tmp2DB=cmp((void*)l1->hd,(void*)
l2->hd);if(_tmp2DB != 0)return _tmp2DB;}if(l2 != 0)return - 1;if(l1 != 0)return 1;
return 0;}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){int _tmp2DC=(tq1.q_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int
_tmp2DD=(tq2.q_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp2DC,_tmp2DD);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp2DE=(void*)x->v;void*
_tmp2DF;_LL206: if((int)_tmp2DE != 0)goto _LL208;_LL207: return - 1;_LL208: if(_tmp2DE
<= (void*)1?1:*((int*)_tmp2DE)!= 0)goto _LL20A;_tmp2DF=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2DE)->f1;_LL209: {void*_tmp2E0=(void*)y->v;void*_tmp2E1;_LL20D: if((int)
_tmp2E0 != 0)goto _LL20F;_LL20E: return 1;_LL20F: if(_tmp2E0 <= (void*)1?1:*((int*)
_tmp2E0)!= 0)goto _LL211;_tmp2E1=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2E0)->f1;_LL210: return cmp(_tmp2DF,_tmp2E1);_LL211: if(_tmp2E0 <= (void*)1?1:*((
int*)_tmp2E0)!= 1)goto _LL20C;_LL212:({void*_tmp2E2[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress(2)",
sizeof(char),40),_tag_arr(_tmp2E2,sizeof(void*),0));});_LL20C:;}_LL20A: if(
_tmp2DE <= (void*)1?1:*((int*)_tmp2DE)!= 1)goto _LL205;_LL20B:({void*_tmp2E3[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress",
sizeof(char),37),_tag_arr(_tmp2E3,sizeof(void*),0));});_LL205:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp2E5;struct Cyc_Absyn_Tqual
_tmp2E6;void*_tmp2E7;struct _tuple4*_tmp2E4=tqt1;_tmp2E5=*_tmp2E4;_tmp2E6=_tmp2E5.f1;
_tmp2E7=_tmp2E5.f2;{struct _tuple4 _tmp2E9;struct Cyc_Absyn_Tqual _tmp2EA;void*
_tmp2EB;struct _tuple4*_tmp2E8=tqt2;_tmp2E9=*_tmp2E8;_tmp2EA=_tmp2E9.f1;_tmp2EB=
_tmp2E9.f2;{int _tmp2EC=Cyc_Tcutil_tqual_cmp(_tmp2E6,_tmp2EA);if(_tmp2EC != 0)
return _tmp2EC;return Cyc_Tcutil_typecmp(_tmp2E7,_tmp2EB);}}}static int Cyc_Tcutil_const_uint_exp_cmp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return Cyc_Core_intcmp((int)Cyc_Evexp_eval_const_uint_exp(
e1),(int)Cyc_Evexp_eval_const_uint_exp(e2));}static int Cyc_Tcutil_attribute_case_number(
void*att){void*_tmp2ED=att;_LL214: if(_tmp2ED <= (void*)16?1:*((int*)_tmp2ED)!= 0)
goto _LL216;_LL215: return 0;_LL216: if((int)_tmp2ED != 0)goto _LL218;_LL217: return 1;
_LL218: if((int)_tmp2ED != 1)goto _LL21A;_LL219: return 2;_LL21A: if((int)_tmp2ED != 2)
goto _LL21C;_LL21B: return 3;_LL21C: if((int)_tmp2ED != 3)goto _LL21E;_LL21D: return 4;
_LL21E: if((int)_tmp2ED != 4)goto _LL220;_LL21F: return 5;_LL220: if(_tmp2ED <= (void*)
16?1:*((int*)_tmp2ED)!= 1)goto _LL222;_LL221: return 6;_LL222: if((int)_tmp2ED != 5)
goto _LL224;_LL223: return 7;_LL224: if(_tmp2ED <= (void*)16?1:*((int*)_tmp2ED)!= 2)
goto _LL226;_LL225: return 8;_LL226: if((int)_tmp2ED != 6)goto _LL228;_LL227: return 9;
_LL228: if((int)_tmp2ED != 7)goto _LL22A;_LL229: return 10;_LL22A: if((int)_tmp2ED != 8)
goto _LL22C;_LL22B: return 11;_LL22C: if((int)_tmp2ED != 9)goto _LL22E;_LL22D: return 12;
_LL22E: if((int)_tmp2ED != 10)goto _LL230;_LL22F: return 13;_LL230: if((int)_tmp2ED != 
11)goto _LL232;_LL231: return 14;_LL232: if((int)_tmp2ED != 12)goto _LL234;_LL233:
return 15;_LL234: if((int)_tmp2ED != 13)goto _LL236;_LL235: return 16;_LL236: if((int)
_tmp2ED != 14)goto _LL238;_LL237: return 17;_LL238: if((int)_tmp2ED != 15)goto _LL23A;
_LL239: return 18;_LL23A: if(_tmp2ED <= (void*)16?1:*((int*)_tmp2ED)!= 3)goto _LL213;
_LL23B: return 19;_LL213:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple6 _tmp2EF=({struct _tuple6 _tmp2EE;_tmp2EE.f1=att1;_tmp2EE.f2=att2;
_tmp2EE;});void*_tmp2F0;int _tmp2F1;void*_tmp2F2;int _tmp2F3;void*_tmp2F4;int
_tmp2F5;void*_tmp2F6;int _tmp2F7;void*_tmp2F8;struct _tagged_arr _tmp2F9;void*
_tmp2FA;struct _tagged_arr _tmp2FB;void*_tmp2FC;void*_tmp2FD;int _tmp2FE;int _tmp2FF;
void*_tmp300;void*_tmp301;int _tmp302;int _tmp303;_LL23D: _tmp2F0=_tmp2EF.f1;if(
_tmp2F0 <= (void*)16?1:*((int*)_tmp2F0)!= 0)goto _LL23F;_tmp2F1=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp2F0)->f1;_tmp2F2=_tmp2EF.f2;if(_tmp2F2 <= (void*)16?1:*((int*)_tmp2F2)!= 0)
goto _LL23F;_tmp2F3=((struct Cyc_Absyn_Regparm_att_struct*)_tmp2F2)->f1;_LL23E:
_tmp2F5=_tmp2F1;_tmp2F7=_tmp2F3;goto _LL240;_LL23F: _tmp2F4=_tmp2EF.f1;if(_tmp2F4
<= (void*)16?1:*((int*)_tmp2F4)!= 1)goto _LL241;_tmp2F5=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp2F4)->f1;_tmp2F6=_tmp2EF.f2;if(_tmp2F6 <= (void*)16?1:*((int*)_tmp2F6)!= 1)
goto _LL241;_tmp2F7=((struct Cyc_Absyn_Aligned_att_struct*)_tmp2F6)->f1;_LL240:
return Cyc_Core_intcmp(_tmp2F5,_tmp2F7);_LL241: _tmp2F8=_tmp2EF.f1;if(_tmp2F8 <= (
void*)16?1:*((int*)_tmp2F8)!= 2)goto _LL243;_tmp2F9=((struct Cyc_Absyn_Section_att_struct*)
_tmp2F8)->f1;_tmp2FA=_tmp2EF.f2;if(_tmp2FA <= (void*)16?1:*((int*)_tmp2FA)!= 2)
goto _LL243;_tmp2FB=((struct Cyc_Absyn_Section_att_struct*)_tmp2FA)->f1;_LL242:
return Cyc_Std_strcmp(_tmp2F9,_tmp2FB);_LL243: _tmp2FC=_tmp2EF.f1;if(_tmp2FC <= (
void*)16?1:*((int*)_tmp2FC)!= 3)goto _LL245;_tmp2FD=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp2FC)->f1;_tmp2FE=((struct Cyc_Absyn_Format_att_struct*)_tmp2FC)->f2;_tmp2FF=((
struct Cyc_Absyn_Format_att_struct*)_tmp2FC)->f3;_tmp300=_tmp2EF.f2;if(_tmp300 <= (
void*)16?1:*((int*)_tmp300)!= 3)goto _LL245;_tmp301=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp300)->f1;_tmp302=((struct Cyc_Absyn_Format_att_struct*)_tmp300)->f2;_tmp303=((
struct Cyc_Absyn_Format_att_struct*)_tmp300)->f3;_LL244: {int _tmp304=Cyc_Core_intcmp((
int)((unsigned int)_tmp2FD),(int)((unsigned int)_tmp301));if(_tmp304 != 0)return
_tmp304;{int _tmp305=Cyc_Core_intcmp(_tmp2FE,_tmp302);if(_tmp305 != 0)return
_tmp305;return Cyc_Core_intcmp(_tmp2FF,_tmp303);}}_LL245:;_LL246: return Cyc_Core_intcmp(
Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));
_LL23C:;}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct
Cyc_Absyn_Aggrfield*f2){int _tmp306=Cyc_Std_zstrptrcmp(f1->name,f2->name);if(
_tmp306 != 0)return _tmp306;{int _tmp307=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp307 != 0)return _tmp307;{int _tmp308=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp308 != 0)return _tmp308;{int _tmp309=Cyc_Tcutil_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp309 != 0)return _tmp309;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,f1->width,f2->width);}}}}
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp30A=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp30A != 0)return
_tmp30A;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t){void*_tmp30B=t;
_LL248: if((int)_tmp30B != 0)goto _LL24A;_LL249: return 0;_LL24A: if(_tmp30B <= (void*)
3?1:*((int*)_tmp30B)!= 0)goto _LL24C;_LL24B: return 1;_LL24C: if(_tmp30B <= (void*)3?
1:*((int*)_tmp30B)!= 1)goto _LL24E;_LL24D: return 2;_LL24E: if(_tmp30B <= (void*)3?1:*((
int*)_tmp30B)!= 2)goto _LL250;_LL24F: return 3;_LL250: if(_tmp30B <= (void*)3?1:*((
int*)_tmp30B)!= 3)goto _LL252;_LL251: return 4;_LL252: if(_tmp30B <= (void*)3?1:*((
int*)_tmp30B)!= 4)goto _LL254;_LL253: return 5;_LL254: if(_tmp30B <= (void*)3?1:*((
int*)_tmp30B)!= 5)goto _LL256;_LL255: return 6;_LL256: if((int)_tmp30B != 1)goto
_LL258;_LL257: return 7;_LL258: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 6)goto
_LL25A;_LL259: return 8;_LL25A: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 7)goto
_LL25C;_LL25B: return 9;_LL25C: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 8)goto
_LL25E;_LL25D: return 10;_LL25E: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 9)goto
_LL260;_LL25F: return 11;_LL260: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 10)goto
_LL262;_LL261: return 12;_LL262: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 11)goto
_LL264;_LL263: return 14;_LL264: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 12)goto
_LL266;_LL265: return 16;_LL266: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 13)goto
_LL268;_LL267: return 17;_LL268: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 15)goto
_LL26A;_LL269: return 18;_LL26A: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 16)goto
_LL26C;_LL26B: return 19;_LL26C: if((int)_tmp30B != 2)goto _LL26E;_LL26D: return 20;
_LL26E: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 17)goto _LL270;_LL26F: return 21;
_LL270: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 18)goto _LL272;_LL271: return 22;
_LL272: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 19)goto _LL274;_LL273: return 23;
_LL274: if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 14)goto _LL247;_LL275: return 24;
_LL247:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp30C=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(
t1),Cyc_Tcutil_type_case_number(t2));if(_tmp30C != 0)return _tmp30C;{struct _tuple6
_tmp30E=({struct _tuple6 _tmp30D;_tmp30D.f1=t2;_tmp30D.f2=t1;_tmp30D;});void*
_tmp30F;void*_tmp310;void*_tmp311;void*_tmp312;void*_tmp313;struct Cyc_Absyn_Tvar*
_tmp314;void*_tmp315;struct Cyc_Absyn_Tvar*_tmp316;void*_tmp317;struct Cyc_Absyn_AggrInfo
_tmp318;void*_tmp319;struct Cyc_List_List*_tmp31A;void*_tmp31B;struct Cyc_Absyn_AggrInfo
_tmp31C;void*_tmp31D;struct Cyc_List_List*_tmp31E;void*_tmp31F;struct _tuple1*
_tmp320;void*_tmp321;struct _tuple1*_tmp322;void*_tmp323;struct Cyc_List_List*
_tmp324;void*_tmp325;struct Cyc_List_List*_tmp326;void*_tmp327;struct Cyc_Absyn_TunionInfo
_tmp328;void*_tmp329;struct Cyc_Absyn_Tuniondecl**_tmp32A;struct Cyc_Absyn_Tuniondecl*
_tmp32B;struct Cyc_List_List*_tmp32C;void*_tmp32D;void*_tmp32E;struct Cyc_Absyn_TunionInfo
_tmp32F;void*_tmp330;struct Cyc_Absyn_Tuniondecl**_tmp331;struct Cyc_Absyn_Tuniondecl*
_tmp332;struct Cyc_List_List*_tmp333;void*_tmp334;void*_tmp335;struct Cyc_Absyn_TunionFieldInfo
_tmp336;void*_tmp337;struct Cyc_Absyn_Tuniondecl*_tmp338;struct Cyc_Absyn_Tunionfield*
_tmp339;struct Cyc_List_List*_tmp33A;void*_tmp33B;struct Cyc_Absyn_TunionFieldInfo
_tmp33C;void*_tmp33D;struct Cyc_Absyn_Tuniondecl*_tmp33E;struct Cyc_Absyn_Tunionfield*
_tmp33F;struct Cyc_List_List*_tmp340;void*_tmp341;struct Cyc_Absyn_PtrInfo _tmp342;
void*_tmp343;void*_tmp344;struct Cyc_Absyn_Conref*_tmp345;struct Cyc_Absyn_Tqual
_tmp346;struct Cyc_Absyn_Conref*_tmp347;void*_tmp348;struct Cyc_Absyn_PtrInfo
_tmp349;void*_tmp34A;void*_tmp34B;struct Cyc_Absyn_Conref*_tmp34C;struct Cyc_Absyn_Tqual
_tmp34D;struct Cyc_Absyn_Conref*_tmp34E;void*_tmp34F;void*_tmp350;void*_tmp351;
void*_tmp352;void*_tmp353;void*_tmp354;void*_tmp355;void*_tmp356;void*_tmp357;
int _tmp358;void*_tmp359;int _tmp35A;void*_tmp35B;void*_tmp35C;struct Cyc_Absyn_Tqual
_tmp35D;struct Cyc_Absyn_Exp*_tmp35E;void*_tmp35F;void*_tmp360;struct Cyc_Absyn_Tqual
_tmp361;struct Cyc_Absyn_Exp*_tmp362;void*_tmp363;struct Cyc_Absyn_FnInfo _tmp364;
struct Cyc_List_List*_tmp365;struct Cyc_Core_Opt*_tmp366;void*_tmp367;struct Cyc_List_List*
_tmp368;int _tmp369;struct Cyc_Absyn_VarargInfo*_tmp36A;struct Cyc_List_List*
_tmp36B;struct Cyc_List_List*_tmp36C;void*_tmp36D;struct Cyc_Absyn_FnInfo _tmp36E;
struct Cyc_List_List*_tmp36F;struct Cyc_Core_Opt*_tmp370;void*_tmp371;struct Cyc_List_List*
_tmp372;int _tmp373;struct Cyc_Absyn_VarargInfo*_tmp374;struct Cyc_List_List*
_tmp375;struct Cyc_List_List*_tmp376;void*_tmp377;struct Cyc_List_List*_tmp378;
void*_tmp379;struct Cyc_List_List*_tmp37A;void*_tmp37B;void*_tmp37C;struct Cyc_List_List*
_tmp37D;void*_tmp37E;void*_tmp37F;struct Cyc_List_List*_tmp380;void*_tmp381;void*
_tmp382;void*_tmp383;void*_tmp384;void*_tmp385;void*_tmp386;void*_tmp387;void*
_tmp388;void*_tmp389;void*_tmp38A;void*_tmp38B;void*_tmp38C;void*_tmp38D;void*
_tmp38E;void*_tmp38F;void*_tmp390;_LL277: _tmp30F=_tmp30E.f1;if(_tmp30F <= (void*)
3?1:*((int*)_tmp30F)!= 0)goto _LL279;_tmp310=_tmp30E.f2;if(_tmp310 <= (void*)3?1:*((
int*)_tmp310)!= 0)goto _LL279;_LL278:({void*_tmp391[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: can only compare closed types",
sizeof(char),39),_tag_arr(_tmp391,sizeof(void*),0));});_LL279: _tmp311=_tmp30E.f1;
if((int)_tmp311 != 0)goto _LL27B;_tmp312=_tmp30E.f2;if((int)_tmp312 != 0)goto _LL27B;
_LL27A: return 0;_LL27B: _tmp313=_tmp30E.f1;if(_tmp313 <= (void*)3?1:*((int*)_tmp313)
!= 1)goto _LL27D;_tmp314=((struct Cyc_Absyn_VarType_struct*)_tmp313)->f1;_tmp315=
_tmp30E.f2;if(_tmp315 <= (void*)3?1:*((int*)_tmp315)!= 1)goto _LL27D;_tmp316=((
struct Cyc_Absyn_VarType_struct*)_tmp315)->f1;_LL27C: return Cyc_Core_intcmp(*((int*)
_check_null(_tmp316->identity)),*((int*)_check_null(_tmp314->identity)));_LL27D:
_tmp317=_tmp30E.f1;if(_tmp317 <= (void*)3?1:*((int*)_tmp317)!= 10)goto _LL27F;
_tmp318=((struct Cyc_Absyn_AggrType_struct*)_tmp317)->f1;_tmp319=(void*)_tmp318.aggr_info;
_tmp31A=_tmp318.targs;_tmp31B=_tmp30E.f2;if(_tmp31B <= (void*)3?1:*((int*)_tmp31B)
!= 10)goto _LL27F;_tmp31C=((struct Cyc_Absyn_AggrType_struct*)_tmp31B)->f1;_tmp31D=(
void*)_tmp31C.aggr_info;_tmp31E=_tmp31C.targs;_LL27E: {struct _tuple1*_tmp393;
struct _tuple5 _tmp392=Cyc_Absyn_aggr_kinded_name(_tmp319);_tmp393=_tmp392.f2;{
struct _tuple1*_tmp395;struct _tuple5 _tmp394=Cyc_Absyn_aggr_kinded_name(_tmp31D);
_tmp395=_tmp394.f2;{int _tmp396=Cyc_Absyn_qvar_cmp(_tmp393,_tmp395);if(_tmp396 != 
0)return _tmp396;else{return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,_tmp31A,
_tmp31E);}}}}_LL27F: _tmp31F=_tmp30E.f1;if(_tmp31F <= (void*)3?1:*((int*)_tmp31F)
!= 12)goto _LL281;_tmp320=((struct Cyc_Absyn_EnumType_struct*)_tmp31F)->f1;_tmp321=
_tmp30E.f2;if(_tmp321 <= (void*)3?1:*((int*)_tmp321)!= 12)goto _LL281;_tmp322=((
struct Cyc_Absyn_EnumType_struct*)_tmp321)->f1;_LL280: return Cyc_Absyn_qvar_cmp(
_tmp320,_tmp322);_LL281: _tmp323=_tmp30E.f1;if(_tmp323 <= (void*)3?1:*((int*)
_tmp323)!= 13)goto _LL283;_tmp324=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp323)->f1;
_tmp325=_tmp30E.f2;if(_tmp325 <= (void*)3?1:*((int*)_tmp325)!= 13)goto _LL283;
_tmp326=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp325)->f1;_LL282: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp324,
_tmp326);_LL283: _tmp327=_tmp30E.f1;if(_tmp327 <= (void*)3?1:*((int*)_tmp327)!= 2)
goto _LL285;_tmp328=((struct Cyc_Absyn_TunionType_struct*)_tmp327)->f1;_tmp329=(
void*)_tmp328.tunion_info;if(*((int*)_tmp329)!= 1)goto _LL285;_tmp32A=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp329)->f1;_tmp32B=*_tmp32A;_tmp32C=_tmp328.targs;_tmp32D=(void*)_tmp328.rgn;
_tmp32E=_tmp30E.f2;if(_tmp32E <= (void*)3?1:*((int*)_tmp32E)!= 2)goto _LL285;
_tmp32F=((struct Cyc_Absyn_TunionType_struct*)_tmp32E)->f1;_tmp330=(void*)_tmp32F.tunion_info;
if(*((int*)_tmp330)!= 1)goto _LL285;_tmp331=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp330)->f1;_tmp332=*_tmp331;_tmp333=_tmp32F.targs;_tmp334=(void*)_tmp32F.rgn;
_LL284: if(_tmp332 == _tmp32B)return 0;{int _tmp397=Cyc_Absyn_qvar_cmp(_tmp332->name,
_tmp32B->name);if(_tmp397 != 0)return _tmp397;{int _tmp398=Cyc_Tcutil_typecmp(
_tmp334,_tmp32D);if(_tmp398 != 0)return _tmp398;return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp333,_tmp32C);}}_LL285: _tmp335=_tmp30E.f1;if(_tmp335 <= (void*)3?1:*((int*)
_tmp335)!= 3)goto _LL287;_tmp336=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp335)->f1;_tmp337=(void*)_tmp336.field_info;if(*((int*)_tmp337)!= 1)goto
_LL287;_tmp338=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp337)->f1;_tmp339=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp337)->f2;_tmp33A=_tmp336.targs;
_tmp33B=_tmp30E.f2;if(_tmp33B <= (void*)3?1:*((int*)_tmp33B)!= 3)goto _LL287;
_tmp33C=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp33B)->f1;_tmp33D=(void*)
_tmp33C.field_info;if(*((int*)_tmp33D)!= 1)goto _LL287;_tmp33E=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp33D)->f1;_tmp33F=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp33D)->f2;
_tmp340=_tmp33C.targs;_LL286: if(_tmp33E == _tmp338)return 0;{int _tmp399=Cyc_Absyn_qvar_cmp(
_tmp338->name,_tmp33E->name);if(_tmp399 != 0)return _tmp399;{int _tmp39A=Cyc_Absyn_qvar_cmp(
_tmp339->name,_tmp33F->name);if(_tmp39A != 0)return _tmp39A;return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp340,_tmp33A);}}_LL287: _tmp341=_tmp30E.f1;if(_tmp341 <= (
void*)3?1:*((int*)_tmp341)!= 4)goto _LL289;_tmp342=((struct Cyc_Absyn_PointerType_struct*)
_tmp341)->f1;_tmp343=(void*)_tmp342.elt_typ;_tmp344=(void*)_tmp342.rgn_typ;
_tmp345=_tmp342.nullable;_tmp346=_tmp342.tq;_tmp347=_tmp342.bounds;_tmp348=
_tmp30E.f2;if(_tmp348 <= (void*)3?1:*((int*)_tmp348)!= 4)goto _LL289;_tmp349=((
struct Cyc_Absyn_PointerType_struct*)_tmp348)->f1;_tmp34A=(void*)_tmp349.elt_typ;
_tmp34B=(void*)_tmp349.rgn_typ;_tmp34C=_tmp349.nullable;_tmp34D=_tmp349.tq;
_tmp34E=_tmp349.bounds;_LL288: {int _tmp39B=Cyc_Tcutil_typecmp(_tmp34A,_tmp343);
if(_tmp39B != 0)return _tmp39B;{int _tmp39C=Cyc_Tcutil_typecmp(_tmp34B,_tmp344);if(
_tmp39C != 0)return _tmp39C;{int _tmp39D=Cyc_Tcutil_tqual_cmp(_tmp34D,_tmp346);if(
_tmp39D != 0)return _tmp39D;{int _tmp39E=Cyc_Tcutil_conrefs_cmp(Cyc_Tcutil_boundscmp,
_tmp34E,_tmp347);if(_tmp39E != 0)return _tmp39E;{void*_tmp39F=(void*)(Cyc_Absyn_compress_conref(
_tmp34E))->v;void*_tmp3A0;_LL2AC: if(_tmp39F <= (void*)1?1:*((int*)_tmp39F)!= 0)
goto _LL2AE;_tmp3A0=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp39F)->f1;if((
int)_tmp3A0 != 0)goto _LL2AE;_LL2AD: return 0;_LL2AE:;_LL2AF: goto _LL2AB;_LL2AB:;}
return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp34C,_tmp345);}}}}_LL289: _tmp34F=
_tmp30E.f1;if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 5)goto _LL28B;_tmp350=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp34F)->f1;_tmp351=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp34F)->f2;_tmp352=_tmp30E.f2;if(_tmp352 <= (void*)3?1:*((int*)_tmp352)!= 5)
goto _LL28B;_tmp353=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp352)->f1;_tmp354=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp352)->f2;_LL28A: if(_tmp353 != _tmp350)
return Cyc_Core_intcmp((int)((unsigned int)_tmp353),(int)((unsigned int)_tmp350));
if(_tmp354 != _tmp351)return Cyc_Core_intcmp((int)((unsigned int)_tmp354),(int)((
unsigned int)_tmp351));return 0;_LL28B: _tmp355=_tmp30E.f1;if((int)_tmp355 != 1)
goto _LL28D;_tmp356=_tmp30E.f2;if((int)_tmp356 != 1)goto _LL28D;_LL28C: return 0;
_LL28D: _tmp357=_tmp30E.f1;if(_tmp357 <= (void*)3?1:*((int*)_tmp357)!= 6)goto
_LL28F;_tmp358=((struct Cyc_Absyn_DoubleType_struct*)_tmp357)->f1;_tmp359=_tmp30E.f2;
if(_tmp359 <= (void*)3?1:*((int*)_tmp359)!= 6)goto _LL28F;_tmp35A=((struct Cyc_Absyn_DoubleType_struct*)
_tmp359)->f1;_LL28E: if(_tmp358 == _tmp35A)return 0;else{if(_tmp358)return - 1;else{
return 1;}}_LL28F: _tmp35B=_tmp30E.f1;if(_tmp35B <= (void*)3?1:*((int*)_tmp35B)!= 7)
goto _LL291;_tmp35C=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp35B)->f1;
_tmp35D=((struct Cyc_Absyn_ArrayType_struct*)_tmp35B)->f2;_tmp35E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp35B)->f3;_tmp35F=_tmp30E.f2;if(_tmp35F <= (void*)3?1:*((int*)_tmp35F)!= 7)
goto _LL291;_tmp360=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp35F)->f1;
_tmp361=((struct Cyc_Absyn_ArrayType_struct*)_tmp35F)->f2;_tmp362=((struct Cyc_Absyn_ArrayType_struct*)
_tmp35F)->f3;_LL290: {int _tmp3A1=Cyc_Tcutil_tqual_cmp(_tmp361,_tmp35D);if(
_tmp3A1 != 0)return _tmp3A1;{int _tmp3A2=Cyc_Tcutil_typecmp(_tmp360,_tmp35C);if(
_tmp3A2 != 0)return _tmp3A2;if(_tmp35E == _tmp362)return 0;if(_tmp35E == 0?1: _tmp362
== 0)({void*_tmp3A3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("missing expression in array index",sizeof(char),34),_tag_arr(_tmp3A3,
sizeof(void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,
_tmp35E,_tmp362);}}_LL291: _tmp363=_tmp30E.f1;if(_tmp363 <= (void*)3?1:*((int*)
_tmp363)!= 8)goto _LL293;_tmp364=((struct Cyc_Absyn_FnType_struct*)_tmp363)->f1;
_tmp365=_tmp364.tvars;_tmp366=_tmp364.effect;_tmp367=(void*)_tmp364.ret_typ;
_tmp368=_tmp364.args;_tmp369=_tmp364.c_varargs;_tmp36A=_tmp364.cyc_varargs;
_tmp36B=_tmp364.rgn_po;_tmp36C=_tmp364.attributes;_tmp36D=_tmp30E.f2;if(_tmp36D
<= (void*)3?1:*((int*)_tmp36D)!= 8)goto _LL293;_tmp36E=((struct Cyc_Absyn_FnType_struct*)
_tmp36D)->f1;_tmp36F=_tmp36E.tvars;_tmp370=_tmp36E.effect;_tmp371=(void*)_tmp36E.ret_typ;
_tmp372=_tmp36E.args;_tmp373=_tmp36E.c_varargs;_tmp374=_tmp36E.cyc_varargs;
_tmp375=_tmp36E.rgn_po;_tmp376=_tmp36E.attributes;_LL292:({void*_tmp3A4[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: function types not handled",
sizeof(char),36),_tag_arr(_tmp3A4,sizeof(void*),0));});_LL293: _tmp377=_tmp30E.f1;
if(_tmp377 <= (void*)3?1:*((int*)_tmp377)!= 9)goto _LL295;_tmp378=((struct Cyc_Absyn_TupleType_struct*)
_tmp377)->f1;_tmp379=_tmp30E.f2;if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 9)
goto _LL295;_tmp37A=((struct Cyc_Absyn_TupleType_struct*)_tmp379)->f1;_LL294:
return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp37A,
_tmp378);_LL295: _tmp37B=_tmp30E.f1;if(_tmp37B <= (void*)3?1:*((int*)_tmp37B)!= 11)
goto _LL297;_tmp37C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp37B)->f1;
_tmp37D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp37B)->f2;_tmp37E=_tmp30E.f2;
if(_tmp37E <= (void*)3?1:*((int*)_tmp37E)!= 11)goto _LL297;_tmp37F=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp37E)->f1;_tmp380=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp37E)->f2;_LL296: if(_tmp37F != _tmp37C){if(_tmp37F == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp380,_tmp37D);_LL297: _tmp381=_tmp30E.f1;if((int)_tmp381 != 2)goto _LL299;
_tmp382=_tmp30E.f2;if((int)_tmp382 != 2)goto _LL299;_LL298: return 0;_LL299: _tmp383=
_tmp30E.f1;if(_tmp383 <= (void*)3?1:*((int*)_tmp383)!= 15)goto _LL29B;_tmp384=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp383)->f1;_tmp385=_tmp30E.f2;
if(_tmp385 <= (void*)3?1:*((int*)_tmp385)!= 15)goto _LL29B;_tmp386=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp385)->f1;_LL29A: return Cyc_Tcutil_typecmp(
_tmp384,_tmp386);_LL29B: _tmp387=_tmp30E.f1;if(_tmp387 <= (void*)3?1:*((int*)
_tmp387)!= 14)goto _LL29D;_tmp388=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp387)->f1;_tmp389=_tmp30E.f2;if(_tmp389 <= (void*)3?1:*((int*)_tmp389)!= 14)
goto _LL29D;_tmp38A=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp389)->f1;
_LL29C: return Cyc_Tcutil_typecmp(_tmp388,_tmp38A);_LL29D: _tmp38B=_tmp30E.f1;if(
_tmp38B <= (void*)3?1:*((int*)_tmp38B)!= 18)goto _LL29F;_LL29E: goto _LL2A0;_LL29F:
_tmp38C=_tmp30E.f2;if(_tmp38C <= (void*)3?1:*((int*)_tmp38C)!= 18)goto _LL2A1;
_LL2A0: goto _LL2A2;_LL2A1: _tmp38D=_tmp30E.f1;if(_tmp38D <= (void*)3?1:*((int*)
_tmp38D)!= 17)goto _LL2A3;_LL2A2: goto _LL2A4;_LL2A3: _tmp38E=_tmp30E.f1;if(_tmp38E
<= (void*)3?1:*((int*)_tmp38E)!= 19)goto _LL2A5;_LL2A4: goto _LL2A6;_LL2A5: _tmp38F=
_tmp30E.f2;if(_tmp38F <= (void*)3?1:*((int*)_tmp38F)!= 19)goto _LL2A7;_LL2A6: goto
_LL2A8;_LL2A7: _tmp390=_tmp30E.f2;if(_tmp390 <= (void*)3?1:*((int*)_tmp390)!= 17)
goto _LL2A9;_LL2A8:({void*_tmp3A5[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: effects not handled",sizeof(
char),29),_tag_arr(_tmp3A5,sizeof(void*),0));});_LL2A9:;_LL2AA:({void*_tmp3A6[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Unmatched case in typecmp",
sizeof(char),26),_tag_arr(_tmp3A6,sizeof(void*),0));});_LL276:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp3A7=Cyc_Tcutil_compress(t);_LL2B1: if(_tmp3A7 <= (void*)3?1:*((
int*)_tmp3A7)!= 5)goto _LL2B3;_LL2B2: goto _LL2B4;_LL2B3: if((int)_tmp3A7 != 1)goto
_LL2B5;_LL2B4: goto _LL2B6;_LL2B5: if(_tmp3A7 <= (void*)3?1:*((int*)_tmp3A7)!= 6)
goto _LL2B7;_LL2B6: goto _LL2B8;_LL2B7: if(_tmp3A7 <= (void*)3?1:*((int*)_tmp3A7)!= 
12)goto _LL2B9;_LL2B8: goto _LL2BA;_LL2B9: if(_tmp3A7 <= (void*)3?1:*((int*)_tmp3A7)
!= 13)goto _LL2BB;_LL2BA: return 1;_LL2BB:;_LL2BC: return 0;_LL2B0:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp3A9=({struct _tuple6 _tmp3A8;_tmp3A8.f1=Cyc_Tcutil_compress(
t1);_tmp3A8.f2=Cyc_Tcutil_compress(t2);_tmp3A8;});void*_tmp3AA;int _tmp3AB;void*
_tmp3AC;int _tmp3AD;void*_tmp3AE;void*_tmp3AF;void*_tmp3B0;void*_tmp3B1;void*
_tmp3B2;void*_tmp3B3;void*_tmp3B4;void*_tmp3B5;void*_tmp3B6;void*_tmp3B7;void*
_tmp3B8;void*_tmp3B9;void*_tmp3BA;void*_tmp3BB;void*_tmp3BC;void*_tmp3BD;void*
_tmp3BE;void*_tmp3BF;void*_tmp3C0;void*_tmp3C1;void*_tmp3C2;void*_tmp3C3;void*
_tmp3C4;void*_tmp3C5;void*_tmp3C6;void*_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*
_tmp3CA;void*_tmp3CB;void*_tmp3CC;void*_tmp3CD;void*_tmp3CE;void*_tmp3CF;void*
_tmp3D0;void*_tmp3D1;void*_tmp3D2;_LL2BE: _tmp3AA=_tmp3A9.f1;if(_tmp3AA <= (void*)
3?1:*((int*)_tmp3AA)!= 6)goto _LL2C0;_tmp3AB=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3AA)->f1;_tmp3AC=_tmp3A9.f2;if(_tmp3AC <= (void*)3?1:*((int*)_tmp3AC)!= 6)
goto _LL2C0;_tmp3AD=((struct Cyc_Absyn_DoubleType_struct*)_tmp3AC)->f1;_LL2BF:
return !_tmp3AD?_tmp3AB: 0;_LL2C0: _tmp3AE=_tmp3A9.f1;if(_tmp3AE <= (void*)3?1:*((
int*)_tmp3AE)!= 6)goto _LL2C2;_tmp3AF=_tmp3A9.f2;if((int)_tmp3AF != 1)goto _LL2C2;
_LL2C1: goto _LL2C3;_LL2C2: _tmp3B0=_tmp3A9.f1;if(_tmp3B0 <= (void*)3?1:*((int*)
_tmp3B0)!= 6)goto _LL2C4;_tmp3B1=_tmp3A9.f2;if(_tmp3B1 <= (void*)3?1:*((int*)
_tmp3B1)!= 5)goto _LL2C4;_LL2C3: goto _LL2C5;_LL2C4: _tmp3B2=_tmp3A9.f1;if(_tmp3B2 <= (
void*)3?1:*((int*)_tmp3B2)!= 6)goto _LL2C6;_tmp3B3=_tmp3A9.f2;if(_tmp3B3 <= (void*)
3?1:*((int*)_tmp3B3)!= 14)goto _LL2C6;_LL2C5: goto _LL2C7;_LL2C6: _tmp3B4=_tmp3A9.f1;
if((int)_tmp3B4 != 1)goto _LL2C8;_tmp3B5=_tmp3A9.f2;if(_tmp3B5 <= (void*)3?1:*((int*)
_tmp3B5)!= 14)goto _LL2C8;_LL2C7: goto _LL2C9;_LL2C8: _tmp3B6=_tmp3A9.f1;if((int)
_tmp3B6 != 1)goto _LL2CA;_tmp3B7=_tmp3A9.f2;if(_tmp3B7 <= (void*)3?1:*((int*)
_tmp3B7)!= 5)goto _LL2CA;_LL2C9: return 1;_LL2CA: _tmp3B8=_tmp3A9.f1;if(_tmp3B8 <= (
void*)3?1:*((int*)_tmp3B8)!= 5)goto _LL2CC;_tmp3B9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3B8)->f2;if((int)_tmp3B9 != 3)goto _LL2CC;_tmp3BA=_tmp3A9.f2;if(_tmp3BA <= (
void*)3?1:*((int*)_tmp3BA)!= 5)goto _LL2CC;_tmp3BB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3BA)->f2;if((int)_tmp3BB != 3)goto _LL2CC;_LL2CB: return 0;_LL2CC: _tmp3BC=
_tmp3A9.f1;if(_tmp3BC <= (void*)3?1:*((int*)_tmp3BC)!= 5)goto _LL2CE;_tmp3BD=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3BC)->f2;if((int)_tmp3BD != 3)goto _LL2CE;
_LL2CD: goto _LL2CF;_LL2CE: _tmp3BE=_tmp3A9.f1;if(_tmp3BE <= (void*)3?1:*((int*)
_tmp3BE)!= 5)goto _LL2D0;_tmp3BF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3BE)->f2;
if((int)_tmp3BF != 2)goto _LL2D0;_tmp3C0=_tmp3A9.f2;if((int)_tmp3C0 != 1)goto _LL2D0;
_LL2CF: goto _LL2D1;_LL2D0: _tmp3C1=_tmp3A9.f1;if(_tmp3C1 <= (void*)3?1:*((int*)
_tmp3C1)!= 5)goto _LL2D2;_tmp3C2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C1)->f2;
if((int)_tmp3C2 != 2)goto _LL2D2;_tmp3C3=_tmp3A9.f2;if(_tmp3C3 <= (void*)3?1:*((int*)
_tmp3C3)!= 5)goto _LL2D2;_tmp3C4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C3)->f2;
if((int)_tmp3C4 != 1)goto _LL2D2;_LL2D1: goto _LL2D3;_LL2D2: _tmp3C5=_tmp3A9.f1;if(
_tmp3C5 <= (void*)3?1:*((int*)_tmp3C5)!= 5)goto _LL2D4;_tmp3C6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C5)->f2;if((int)_tmp3C6 != 2)goto _LL2D4;_tmp3C7=_tmp3A9.f2;if(_tmp3C7 <= (
void*)3?1:*((int*)_tmp3C7)!= 5)goto _LL2D4;_tmp3C8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C7)->f2;if((int)_tmp3C8 != 0)goto _LL2D4;_LL2D3: goto _LL2D5;_LL2D4: _tmp3C9=
_tmp3A9.f1;if(_tmp3C9 <= (void*)3?1:*((int*)_tmp3C9)!= 5)goto _LL2D6;_tmp3CA=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3C9)->f2;if((int)_tmp3CA != 1)goto _LL2D6;
_tmp3CB=_tmp3A9.f2;if(_tmp3CB <= (void*)3?1:*((int*)_tmp3CB)!= 5)goto _LL2D6;
_tmp3CC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3CB)->f2;if((int)_tmp3CC != 
0)goto _LL2D6;_LL2D5: goto _LL2D7;_LL2D6: _tmp3CD=_tmp3A9.f1;if(_tmp3CD <= (void*)3?1:*((
int*)_tmp3CD)!= 14)goto _LL2D8;_tmp3CE=_tmp3A9.f2;if(_tmp3CE <= (void*)3?1:*((int*)
_tmp3CE)!= 5)goto _LL2D8;_tmp3CF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3CE)->f2;
if((int)_tmp3CF != 1)goto _LL2D8;_LL2D7: goto _LL2D9;_LL2D8: _tmp3D0=_tmp3A9.f1;if(
_tmp3D0 <= (void*)3?1:*((int*)_tmp3D0)!= 14)goto _LL2DA;_tmp3D1=_tmp3A9.f2;if(
_tmp3D1 <= (void*)3?1:*((int*)_tmp3D1)!= 5)goto _LL2DA;_tmp3D2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D1)->f2;if((int)_tmp3D2 != 0)goto _LL2DA;_LL2D9: return 1;_LL2DA:;_LL2DB: return
0;_LL2BD:;}int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*
es){struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;
el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0?1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->v=(
void*)t1;_tmp3D3;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)((
struct Cyc_Core_Opt*)_check_null(max_arith_type))->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_Std_String_pa_struct _tmp3D6;_tmp3D6.tag=0;_tmp3D6.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp3D5;_tmp3D5.tag=0;_tmp3D5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp3D4[2]={& _tmp3D5,& _tmp3D6};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,
_tag_arr("type mismatch: expecting %s but found %s",sizeof(char),41),_tag_arr(
_tmp3D4,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp3D7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL2DD: if(_tmp3D7 <= (void*)3?1:*((int*)_tmp3D7)!= 4)goto _LL2DF;_LL2DE: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_t);goto _LL2DC;_LL2DF:;_LL2E0: return 0;_LL2DC:;}return 1;}int
Cyc_Tcutil_is_integral_type(void*t){void*_tmp3D8=Cyc_Tcutil_compress(t);_LL2E2:
if(_tmp3D8 <= (void*)3?1:*((int*)_tmp3D8)!= 5)goto _LL2E4;_LL2E3: goto _LL2E5;_LL2E4:
if(_tmp3D8 <= (void*)3?1:*((int*)_tmp3D8)!= 14)goto _LL2E6;_LL2E5: goto _LL2E7;
_LL2E6: if(_tmp3D8 <= (void*)3?1:*((int*)_tmp3D8)!= 12)goto _LL2E8;_LL2E7: goto
_LL2E9;_LL2E8: if(_tmp3D8 <= (void*)3?1:*((int*)_tmp3D8)!= 13)goto _LL2EA;_LL2E9:
return 1;_LL2EA:;_LL2EB: return 0;_LL2E1:;}int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_t))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_t))({void*
_tmp3D9[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3D9,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_t);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_t))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_t))({void*
_tmp3DA[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3DA,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_t);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp3DC=({struct _tuple6 _tmp3DB;_tmp3DB.f1=
t1;_tmp3DB.f2=t2;_tmp3DB;});void*_tmp3DD;struct Cyc_Absyn_PtrInfo _tmp3DE;void*
_tmp3DF;struct Cyc_Absyn_PtrInfo _tmp3E0;void*_tmp3E1;void*_tmp3E2;struct Cyc_Absyn_Tqual
_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E4;void*_tmp3E5;void*_tmp3E6;struct Cyc_Absyn_Tqual
_tmp3E7;struct Cyc_Absyn_Exp*_tmp3E8;void*_tmp3E9;struct Cyc_Absyn_TunionFieldInfo
_tmp3EA;void*_tmp3EB;struct Cyc_Absyn_Tuniondecl*_tmp3EC;struct Cyc_Absyn_Tunionfield*
_tmp3ED;struct Cyc_List_List*_tmp3EE;void*_tmp3EF;struct Cyc_Absyn_TunionInfo
_tmp3F0;void*_tmp3F1;struct Cyc_Absyn_Tuniondecl**_tmp3F2;struct Cyc_Absyn_Tuniondecl*
_tmp3F3;struct Cyc_List_List*_tmp3F4;void*_tmp3F5;struct Cyc_Absyn_PtrInfo _tmp3F6;
void*_tmp3F7;void*_tmp3F8;struct Cyc_Absyn_Conref*_tmp3F9;struct Cyc_Absyn_Tqual
_tmp3FA;struct Cyc_Absyn_Conref*_tmp3FB;void*_tmp3FC;struct Cyc_Absyn_TunionInfo
_tmp3FD;void*_tmp3FE;struct Cyc_Absyn_Tuniondecl**_tmp3FF;struct Cyc_Absyn_Tuniondecl*
_tmp400;struct Cyc_List_List*_tmp401;void*_tmp402;void*_tmp403;void*_tmp404;void*
_tmp405;_LL2ED: _tmp3DD=_tmp3DC.f1;if(_tmp3DD <= (void*)3?1:*((int*)_tmp3DD)!= 4)
goto _LL2EF;_tmp3DE=((struct Cyc_Absyn_PointerType_struct*)_tmp3DD)->f1;_tmp3DF=
_tmp3DC.f2;if(_tmp3DF <= (void*)3?1:*((int*)_tmp3DF)!= 4)goto _LL2EF;_tmp3E0=((
struct Cyc_Absyn_PointerType_struct*)_tmp3DF)->f1;_LL2EE: {int okay=1;if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp3DE.nullable,_tmp3E0.nullable)){void*_tmp406=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp3DE.nullable))->v;int _tmp407;_LL2FA: if(_tmp406 <= (void*)1?1:*((int*)_tmp406)
!= 0)goto _LL2FC;_tmp407=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp406)->f1;
_LL2FB: okay=!_tmp407;goto _LL2F9;_LL2FC:;_LL2FD:({void*_tmp408[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("silent_castable conref not eq",
sizeof(char),30),_tag_arr(_tmp408,sizeof(void*),0));});_LL2F9:;}if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp3DE.bounds,_tmp3E0.bounds)){struct _tuple6 _tmp40A=({
struct _tuple6 _tmp409;_tmp409.f1=(void*)(Cyc_Absyn_compress_conref(_tmp3DE.bounds))->v;
_tmp409.f2=(void*)(Cyc_Absyn_compress_conref(_tmp3E0.bounds))->v;_tmp409;});void*
_tmp40B;void*_tmp40C;void*_tmp40D;void*_tmp40E;void*_tmp40F;void*_tmp410;struct
Cyc_Absyn_Exp*_tmp411;void*_tmp412;void*_tmp413;struct Cyc_Absyn_Exp*_tmp414;void*
_tmp415;void*_tmp416;void*_tmp417;void*_tmp418;_LL2FF: _tmp40B=_tmp40A.f1;if(
_tmp40B <= (void*)1?1:*((int*)_tmp40B)!= 0)goto _LL301;_tmp40C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp40B)->f1;if(_tmp40C <= (void*)1?1:*((int*)_tmp40C)!= 0)goto _LL301;_tmp40D=
_tmp40A.f2;if(_tmp40D <= (void*)1?1:*((int*)_tmp40D)!= 0)goto _LL301;_tmp40E=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp40D)->f1;if((int)_tmp40E != 0)goto _LL301;
_LL300: okay=1;goto _LL2FE;_LL301: _tmp40F=_tmp40A.f1;if(_tmp40F <= (void*)1?1:*((
int*)_tmp40F)!= 0)goto _LL303;_tmp410=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp40F)->f1;if(_tmp410 <= (void*)1?1:*((int*)_tmp410)!= 0)goto _LL303;_tmp411=((
struct Cyc_Absyn_Upper_b_struct*)_tmp410)->f1;_tmp412=_tmp40A.f2;if(_tmp412 <= (
void*)1?1:*((int*)_tmp412)!= 0)goto _LL303;_tmp413=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp412)->f1;if(_tmp413 <= (void*)1?1:*((int*)_tmp413)!= 0)goto _LL303;_tmp414=((
struct Cyc_Absyn_Upper_b_struct*)_tmp413)->f1;_LL302: okay=okay?Cyc_Evexp_eval_const_uint_exp(
_tmp411)>= Cyc_Evexp_eval_const_uint_exp(_tmp414): 0;({void*_tmp419[0]={};Cyc_Tcutil_warn(
loc,_tag_arr("implicit cast to shorter array",sizeof(char),31),_tag_arr(_tmp419,
sizeof(void*),0));});goto _LL2FE;_LL303: _tmp415=_tmp40A.f1;if(_tmp415 <= (void*)1?
1:*((int*)_tmp415)!= 0)goto _LL305;_tmp416=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp415)->f1;if((int)_tmp416 != 0)goto _LL305;_tmp417=_tmp40A.f2;if(_tmp417 <= (
void*)1?1:*((int*)_tmp417)!= 0)goto _LL305;_tmp418=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp417)->f1;if((int)_tmp418 != 0)goto _LL305;_LL304: okay=1;goto _LL2FE;_LL305:;
_LL306: okay=0;goto _LL2FE;_LL2FE:;}okay=okay?Cyc_Tcutil_unify((void*)_tmp3DE.elt_typ,(
void*)_tmp3E0.elt_typ): 0;okay=okay?Cyc_Tcutil_unify((void*)_tmp3DE.rgn_typ,(void*)
_tmp3E0.rgn_typ)?1: Cyc_Tcenv_region_outlives(te,(void*)_tmp3DE.rgn_typ,(void*)
_tmp3E0.rgn_typ): 0;okay=okay?!(_tmp3DE.tq).q_const?1:(_tmp3E0.tq).q_const: 0;
return okay;}_LL2EF: _tmp3E1=_tmp3DC.f1;if(_tmp3E1 <= (void*)3?1:*((int*)_tmp3E1)!= 
7)goto _LL2F1;_tmp3E2=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3E1)->f1;
_tmp3E3=((struct Cyc_Absyn_ArrayType_struct*)_tmp3E1)->f2;_tmp3E4=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E1)->f3;_tmp3E5=_tmp3DC.f2;if(_tmp3E5 <= (void*)3?1:*((int*)_tmp3E5)!= 7)
goto _LL2F1;_tmp3E6=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3E5)->f1;
_tmp3E7=((struct Cyc_Absyn_ArrayType_struct*)_tmp3E5)->f2;_tmp3E8=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E5)->f3;_LL2F0: {int okay;okay=(_tmp3E4 != 0?_tmp3E8 != 0: 0)?Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp3E4))== Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp3E8)): 0;return(okay?Cyc_Tcutil_unify(
_tmp3E2,_tmp3E6): 0)?!_tmp3E3.q_const?1: _tmp3E7.q_const: 0;}_LL2F1: _tmp3E9=_tmp3DC.f1;
if(_tmp3E9 <= (void*)3?1:*((int*)_tmp3E9)!= 3)goto _LL2F3;_tmp3EA=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3E9)->f1;_tmp3EB=(void*)_tmp3EA.field_info;if(*((int*)_tmp3EB)!= 1)goto
_LL2F3;_tmp3EC=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3EB)->f1;_tmp3ED=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3EB)->f2;_tmp3EE=_tmp3EA.targs;
_tmp3EF=_tmp3DC.f2;if(_tmp3EF <= (void*)3?1:*((int*)_tmp3EF)!= 2)goto _LL2F3;
_tmp3F0=((struct Cyc_Absyn_TunionType_struct*)_tmp3EF)->f1;_tmp3F1=(void*)_tmp3F0.tunion_info;
if(*((int*)_tmp3F1)!= 1)goto _LL2F3;_tmp3F2=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3F1)->f1;_tmp3F3=*_tmp3F2;_tmp3F4=_tmp3F0.targs;_LL2F2: if((_tmp3EC == _tmp3F3?
1: Cyc_Absyn_qvar_cmp(_tmp3EC->name,_tmp3F3->name)== 0)?_tmp3ED->typs == 0: 0){for(
0;_tmp3EE != 0?_tmp3F4 != 0: 0;(_tmp3EE=_tmp3EE->tl,_tmp3F4=_tmp3F4->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp3EE->hd,(void*)_tmp3F4->hd))break;}if(_tmp3EE == 0?_tmp3F4 == 0: 0)return
1;}return 0;_LL2F3: _tmp3F5=_tmp3DC.f1;if(_tmp3F5 <= (void*)3?1:*((int*)_tmp3F5)!= 
4)goto _LL2F5;_tmp3F6=((struct Cyc_Absyn_PointerType_struct*)_tmp3F5)->f1;_tmp3F7=(
void*)_tmp3F6.elt_typ;_tmp3F8=(void*)_tmp3F6.rgn_typ;_tmp3F9=_tmp3F6.nullable;
_tmp3FA=_tmp3F6.tq;_tmp3FB=_tmp3F6.bounds;_tmp3FC=_tmp3DC.f2;if(_tmp3FC <= (void*)
3?1:*((int*)_tmp3FC)!= 2)goto _LL2F5;_tmp3FD=((struct Cyc_Absyn_TunionType_struct*)
_tmp3FC)->f1;_tmp3FE=(void*)_tmp3FD.tunion_info;if(*((int*)_tmp3FE)!= 1)goto
_LL2F5;_tmp3FF=((struct Cyc_Absyn_KnownTunion_struct*)_tmp3FE)->f1;_tmp400=*
_tmp3FF;_tmp401=_tmp3FD.targs;_tmp402=(void*)_tmp3FD.rgn;_LL2F4:{void*_tmp41A=
Cyc_Tcutil_compress(_tmp3F7);struct Cyc_Absyn_TunionFieldInfo _tmp41B;void*_tmp41C;
struct Cyc_Absyn_Tuniondecl*_tmp41D;struct Cyc_Absyn_Tunionfield*_tmp41E;struct Cyc_List_List*
_tmp41F;_LL308: if(_tmp41A <= (void*)3?1:*((int*)_tmp41A)!= 3)goto _LL30A;_tmp41B=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp41A)->f1;_tmp41C=(void*)_tmp41B.field_info;
if(*((int*)_tmp41C)!= 1)goto _LL30A;_tmp41D=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp41C)->f1;_tmp41E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp41C)->f2;
_tmp41F=_tmp41B.targs;_LL309: if(!Cyc_Tcutil_unify(_tmp3F8,_tmp402)?!Cyc_Tcenv_region_outlives(
te,_tmp3F8,_tmp402): 0)return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp3F9,((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(0)))return 0;if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp3FB,Cyc_Absyn_new_conref(Cyc_Absyn_bounds_one)))return 0;
if(Cyc_Absyn_qvar_cmp(_tmp400->name,_tmp41D->name)== 0?_tmp41E->typs != 0: 0){int
okay=1;for(0;_tmp41F != 0?_tmp401 != 0: 0;(_tmp41F=_tmp41F->tl,_tmp401=_tmp401->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp41F->hd,(void*)_tmp401->hd)){okay=0;break;}}if((
!okay?1: _tmp41F != 0)?1: _tmp401 != 0)return 0;return 1;}goto _LL307;_LL30A:;_LL30B:
goto _LL307;_LL307:;}return 0;_LL2F5: _tmp403=_tmp3DC.f1;if(_tmp403 <= (void*)3?1:*((
int*)_tmp403)!= 14)goto _LL2F7;_tmp404=_tmp3DC.f2;if(_tmp404 <= (void*)3?1:*((int*)
_tmp404)!= 5)goto _LL2F7;_tmp405=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp404)->f2;
if((int)_tmp405 != 2)goto _LL2F7;_LL2F6: return 1;_LL2F7:;_LL2F8: return Cyc_Tcutil_unify(
t1,t2);_LL2EC:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp420=Cyc_Tcutil_compress(
t);_LL30D: if(_tmp420 <= (void*)3?1:*((int*)_tmp420)!= 4)goto _LL30F;_LL30E: return 1;
_LL30F:;_LL310: return 0;_LL30C:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp421=(void*)e->r;void*_tmp422;int _tmp423;_LL312: if(*((int*)_tmp421)!= 0)
goto _LL314;_tmp422=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp421)->f1;if(
_tmp422 <= (void*)1?1:*((int*)_tmp422)!= 2)goto _LL314;_tmp423=((struct Cyc_Absyn_Int_c_struct*)
_tmp422)->f2;if(_tmp423 != 0)goto _LL314;_LL313: return 1;_LL314:;_LL315: return 0;
_LL311:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt
Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)
2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2)?Cyc_Tcutil_is_zero(e1): 0){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424[0]=({struct Cyc_Absyn_Const_e_struct
_tmp425;_tmp425.tag=0;_tmp425.f1=(void*)((void*)0);_tmp425;});_tmp424;})));{
struct Cyc_List_List*_tmp426=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp427=({struct Cyc_Absyn_PointerType_struct*_tmp428=_cycalloc(sizeof(*_tmp428));
_tmp428[0]=({struct Cyc_Absyn_PointerType_struct _tmp429;_tmp429.tag=4;_tmp429.f1=({
struct Cyc_Absyn_PtrInfo _tmp42A;_tmp42A.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp42C=_cycalloc(sizeof(*
_tmp42C));_tmp42C->v=_tmp426;_tmp42C;}));_tmp42A.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp42B=_cycalloc(
sizeof(*_tmp42B));_tmp42B->v=_tmp426;_tmp42B;}));_tmp42A.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmp42A.tq=Cyc_Absyn_empty_tqual();_tmp42A.bounds=
Cyc_Absyn_empty_conref();_tmp42A;});_tmp429;});_tmp428;});(void*)(((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v=(void*)((void*)_tmp427));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}static int Cyc_Tcutil_is_sizeof_type(void*t1){void*_tmp42D=
Cyc_Tcutil_compress(t1);_LL317: if(_tmp42D <= (void*)3?1:*((int*)_tmp42D)!= 14)
goto _LL319;_LL318: return 1;_LL319:;_LL31A: return 0;_LL316:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);if(Cyc_Tcutil_unify(t1,t2))
return 1;if((Cyc_Tcutil_is_arithmetic_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2):
0)?1:(Cyc_Tcutil_is_sizeof_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0)){if(Cyc_Tcutil_will_lose_precision(
t1,t2))({struct Cyc_Std_String_pa_struct _tmp430;_tmp430.tag=0;_tmp430.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmp42F;
_tmp42F.tag=0;_tmp42F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmp42E[2]={& _tmp42F,& _tmp430};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; %s -> %s conversion supplied",
sizeof(char),53),_tag_arr(_tmp42E,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if(Cyc_Tcutil_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);({struct Cyc_Std_String_pa_struct
_tmp433;_tmp433.tag=0;_tmp433.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp432;_tmp432.tag=0;_tmp432.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp431[2]={& _tmp432,& _tmp433};Cyc_Tcutil_warn(
e->loc,_tag_arr("implicit cast from %s to %s",sizeof(char),28),_tag_arr(_tmp431,
sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp434=Cyc_Tcutil_compress(t);
_LL31C: if(_tmp434 <= (void*)3?1:*((int*)_tmp434)!= 5)goto _LL31E;_LL31D: goto _LL31F;
_LL31E: if((int)_tmp434 != 1)goto _LL320;_LL31F: goto _LL321;_LL320: if(_tmp434 <= (
void*)3?1:*((int*)_tmp434)!= 6)goto _LL322;_LL321: return 1;_LL322:;_LL323: return 0;
_LL31B:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*inst,
struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp435=_cycalloc(sizeof(*
_tmp435));_tmp435->f1=x->tq;_tmp435->f2=Cyc_Tcutil_substitute(inst,(void*)x->type);
_tmp435;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp436=t1;struct Cyc_List_List*
_tmp437;struct Cyc_Absyn_AggrInfo _tmp438;void*_tmp439;struct Cyc_Absyn_Aggrdecl**
_tmp43A;struct Cyc_Absyn_Aggrdecl*_tmp43B;struct Cyc_List_List*_tmp43C;void*
_tmp43D;struct Cyc_List_List*_tmp43E;_LL325: if((int)_tmp436 != 0)goto _LL327;_LL326:
return 0;_LL327: if(_tmp436 <= (void*)3?1:*((int*)_tmp436)!= 9)goto _LL329;_tmp437=((
struct Cyc_Absyn_TupleType_struct*)_tmp436)->f1;_LL328: return _tmp437;_LL329: if(
_tmp436 <= (void*)3?1:*((int*)_tmp436)!= 10)goto _LL32B;_tmp438=((struct Cyc_Absyn_AggrType_struct*)
_tmp436)->f1;_tmp439=(void*)_tmp438.aggr_info;if(*((int*)_tmp439)!= 1)goto _LL32B;
_tmp43A=((struct Cyc_Absyn_KnownAggr_struct*)_tmp439)->f1;_tmp43B=*_tmp43A;
_tmp43C=_tmp438.targs;_LL32A: if((void*)_tmp43B->kind == (void*)1?1: _tmp43B->fields
== 0)return({struct Cyc_List_List*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->hd=({
struct _tuple4*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440->f1=Cyc_Absyn_empty_tqual();
_tmp440->f2=t1;_tmp440;});_tmp43F->tl=0;_tmp43F;});{struct Cyc_List_List*_tmp441=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp43B->tvs,_tmp43C);return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp441,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp43B->fields))->v);}_LL32B: if(_tmp436 <= (void*)
3?1:*((int*)_tmp436)!= 11)goto _LL32D;_tmp43D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp436)->f1;if((int)_tmp43D != 0)goto _LL32D;_tmp43E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp436)->f2;_LL32C: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp43E);_LL32D:;_LL32E: return({struct Cyc_List_List*
_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->hd=({struct _tuple4*_tmp443=
_cycalloc(sizeof(*_tmp443));_tmp443->f1=Cyc_Absyn_empty_tqual();_tmp443->f2=t1;
_tmp443;});_tmp442->tl=0;_tmp442;});_LL324:;}}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1)?Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2):
0)return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp445=({struct _tuple6 _tmp444;_tmp444.f1=t1;_tmp444.f2=t2;_tmp444;});void*
_tmp446;struct Cyc_Absyn_PtrInfo _tmp447;void*_tmp448;void*_tmp449;struct Cyc_Absyn_Conref*
_tmp44A;struct Cyc_Absyn_Tqual _tmp44B;struct Cyc_Absyn_Conref*_tmp44C;void*_tmp44D;
struct Cyc_Absyn_PtrInfo _tmp44E;void*_tmp44F;void*_tmp450;struct Cyc_Absyn_Conref*
_tmp451;struct Cyc_Absyn_Tqual _tmp452;struct Cyc_Absyn_Conref*_tmp453;_LL330:
_tmp446=_tmp445.f1;if(_tmp446 <= (void*)3?1:*((int*)_tmp446)!= 4)goto _LL332;
_tmp447=((struct Cyc_Absyn_PointerType_struct*)_tmp446)->f1;_tmp448=(void*)
_tmp447.elt_typ;_tmp449=(void*)_tmp447.rgn_typ;_tmp44A=_tmp447.nullable;_tmp44B=
_tmp447.tq;_tmp44C=_tmp447.bounds;_tmp44D=_tmp445.f2;if(_tmp44D <= (void*)3?1:*((
int*)_tmp44D)!= 4)goto _LL332;_tmp44E=((struct Cyc_Absyn_PointerType_struct*)
_tmp44D)->f1;_tmp44F=(void*)_tmp44E.elt_typ;_tmp450=(void*)_tmp44E.rgn_typ;
_tmp451=_tmp44E.nullable;_tmp452=_tmp44E.tq;_tmp453=_tmp44E.bounds;_LL331: if(
_tmp44B.q_const?!_tmp452.q_const: 0)return 0;if((!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp44A,_tmp451)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp44A): 0)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp451): 0)
return 0;if(!Cyc_Tcutil_unify(_tmp449,_tmp450)?!Cyc_Tcenv_region_outlives(te,
_tmp449,_tmp450): 0)return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp44C,_tmp453)){struct _tuple6 _tmp455=({struct _tuple6 _tmp454;_tmp454.f1=Cyc_Absyn_conref_val(
_tmp44C);_tmp454.f2=Cyc_Absyn_conref_val(_tmp453);_tmp454;});void*_tmp456;void*
_tmp457;void*_tmp458;struct Cyc_Absyn_Exp*_tmp459;void*_tmp45A;struct Cyc_Absyn_Exp*
_tmp45B;_LL335: _tmp456=_tmp455.f1;if(_tmp456 <= (void*)1?1:*((int*)_tmp456)!= 0)
goto _LL337;_tmp457=_tmp455.f2;if((int)_tmp457 != 0)goto _LL337;_LL336: goto _LL334;
_LL337: _tmp458=_tmp455.f1;if(_tmp458 <= (void*)1?1:*((int*)_tmp458)!= 0)goto
_LL339;_tmp459=((struct Cyc_Absyn_Upper_b_struct*)_tmp458)->f1;_tmp45A=_tmp455.f2;
if(_tmp45A <= (void*)1?1:*((int*)_tmp45A)!= 0)goto _LL339;_tmp45B=((struct Cyc_Absyn_Upper_b_struct*)
_tmp45A)->f1;_LL338: if(Cyc_Evexp_eval_const_uint_exp(_tmp459)< Cyc_Evexp_eval_const_uint_exp(
_tmp45B))return 0;goto _LL334;_LL339:;_LL33A: return 0;_LL334:;}return Cyc_Tcutil_ptrsubtype(
te,({struct Cyc_List_List*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->hd=({
struct _tuple6*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->f1=t1;_tmp45D->f2=t2;
_tmp45D;});_tmp45C->tl=assume;_tmp45C;}),_tmp448,_tmp44F);_LL332:;_LL333: return 0;
_LL32F:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp45F=({
struct _tuple6 _tmp45E;_tmp45E.f1=Cyc_Tcutil_compress(t1);_tmp45E.f2=Cyc_Tcutil_compress(
t2);_tmp45E;});void*_tmp460;void*_tmp461;void*_tmp462;void*_tmp463;_LL33C:
_tmp460=_tmp45F.f1;if(_tmp460 <= (void*)3?1:*((int*)_tmp460)!= 5)goto _LL33E;
_tmp461=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp460)->f2;_tmp462=_tmp45F.f2;
if(_tmp462 <= (void*)3?1:*((int*)_tmp462)!= 5)goto _LL33E;_tmp463=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp462)->f2;_LL33D: return _tmp461 == _tmp463;_LL33E:;
_LL33F: return 0;_LL33B:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp465;struct
Cyc_Absyn_Tqual _tmp466;void*_tmp467;struct _tuple4*_tmp464=(struct _tuple4*)tqs1->hd;
_tmp465=*_tmp464;_tmp466=_tmp465.f1;_tmp467=_tmp465.f2;{struct _tuple4 _tmp469;
struct Cyc_Absyn_Tqual _tmp46A;void*_tmp46B;struct _tuple4*_tmp468=(struct _tuple4*)
tqs2->hd;_tmp469=*_tmp468;_tmp46A=_tmp469.f1;_tmp46B=_tmp469.f2;if(_tmp46A.q_const?
Cyc_Tcutil_subtype(te,assume,_tmp467,_tmp46B): 0)continue;else{if(Cyc_Tcutil_unify(
_tmp467,_tmp46B))continue;else{if(Cyc_Tcutil_isomorphic(_tmp467,_tmp46B))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp46C=Cyc_Tcutil_compress(t);void*_tmp46D;_LL341: if(_tmp46C <= (void*)3?1:*((
int*)_tmp46C)!= 5)goto _LL343;_tmp46D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46C)->f2;if((int)_tmp46D != 0)goto _LL343;_LL342: return 1;_LL343:;_LL344: return
0;_LL340:;}int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{void*_tmp46E=t2;void*_tmp46F;_LL346: if(_tmp46E <= (
void*)3?1:*((int*)_tmp46E)!= 5)goto _LL348;_tmp46F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46E)->f2;if((int)_tmp46F != 2)goto _LL348;_LL347: if(Cyc_Tcutil_typ_kind(t2)== (
void*)2)return 1;goto _LL345;_LL348:;_LL349: goto _LL345;_LL345:;}{void*_tmp470=t1;
struct Cyc_Absyn_PtrInfo _tmp471;void*_tmp472;void*_tmp473;struct Cyc_Absyn_Conref*
_tmp474;struct Cyc_Absyn_Tqual _tmp475;struct Cyc_Absyn_Conref*_tmp476;void*_tmp477;
struct Cyc_Absyn_Tqual _tmp478;struct Cyc_Absyn_Exp*_tmp479;struct Cyc_Absyn_Enumdecl*
_tmp47A;_LL34B: if(_tmp470 <= (void*)3?1:*((int*)_tmp470)!= 4)goto _LL34D;_tmp471=((
struct Cyc_Absyn_PointerType_struct*)_tmp470)->f1;_tmp472=(void*)_tmp471.elt_typ;
_tmp473=(void*)_tmp471.rgn_typ;_tmp474=_tmp471.nullable;_tmp475=_tmp471.tq;
_tmp476=_tmp471.bounds;_LL34C:{void*_tmp47B=t2;struct Cyc_Absyn_PtrInfo _tmp47C;
void*_tmp47D;void*_tmp47E;struct Cyc_Absyn_Conref*_tmp47F;struct Cyc_Absyn_Tqual
_tmp480;struct Cyc_Absyn_Conref*_tmp481;_LL35C: if(_tmp47B <= (void*)3?1:*((int*)
_tmp47B)!= 4)goto _LL35E;_tmp47C=((struct Cyc_Absyn_PointerType_struct*)_tmp47B)->f1;
_tmp47D=(void*)_tmp47C.elt_typ;_tmp47E=(void*)_tmp47C.rgn_typ;_tmp47F=_tmp47C.nullable;
_tmp480=_tmp47C.tq;_tmp481=_tmp47C.bounds;_LL35D: {struct Cyc_List_List*_tmp482=({
struct Cyc_List_List*_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B->hd=({struct
_tuple6*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->f1=t1;_tmp48C->f2=t2;
_tmp48C;});_tmp48B->tl=0;_tmp48B;});int _tmp483=Cyc_Tcutil_ptrsubtype(te,_tmp482,
_tmp472,_tmp47D)?!_tmp475.q_const?1: _tmp480.q_const: 0;int _tmp484=_tmp483?0:((Cyc_Tcutil_bits_only(
_tmp472)?Cyc_Tcutil_is_char_type(_tmp47D): 0)?_tmp480.q_const?1: !_tmp475.q_const:
0);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp476,_tmp481);
if(!bounds_ok?!_tmp484: 0){struct _tuple6 _tmp486=({struct _tuple6 _tmp485;_tmp485.f1=
Cyc_Absyn_conref_val(_tmp476);_tmp485.f2=Cyc_Absyn_conref_val(_tmp481);_tmp485;});
void*_tmp487;struct Cyc_Absyn_Exp*_tmp488;void*_tmp489;struct Cyc_Absyn_Exp*
_tmp48A;_LL363: _tmp487=_tmp486.f1;if(_tmp487 <= (void*)1?1:*((int*)_tmp487)!= 0)
goto _LL365;_tmp488=((struct Cyc_Absyn_Upper_b_struct*)_tmp487)->f1;_tmp489=
_tmp486.f2;if(_tmp489 <= (void*)1?1:*((int*)_tmp489)!= 0)goto _LL365;_tmp48A=((
struct Cyc_Absyn_Upper_b_struct*)_tmp489)->f1;_LL364: if(Cyc_Evexp_eval_const_uint_exp(
_tmp488)>= Cyc_Evexp_eval_const_uint_exp(_tmp48A))bounds_ok=1;goto _LL362;_LL365:;
_LL366: bounds_ok=1;goto _LL362;_LL362:;}return(bounds_ok?_tmp483?1: _tmp484: 0)?Cyc_Tcutil_unify(
_tmp473,_tmp47E)?1: Cyc_Tcenv_region_outlives(te,_tmp473,_tmp47E): 0;}_LL35E: if((
int)_tmp47B != 0)goto _LL360;_LL35F: return 1;_LL360:;_LL361: goto _LL35B;_LL35B:;}
return 0;_LL34D: if(_tmp470 <= (void*)3?1:*((int*)_tmp470)!= 7)goto _LL34F;_tmp477=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp470)->f1;_tmp478=((struct Cyc_Absyn_ArrayType_struct*)
_tmp470)->f2;_tmp479=((struct Cyc_Absyn_ArrayType_struct*)_tmp470)->f3;_LL34E:{
void*_tmp48D=t2;void*_tmp48E;struct Cyc_Absyn_Tqual _tmp48F;struct Cyc_Absyn_Exp*
_tmp490;_LL368: if(_tmp48D <= (void*)3?1:*((int*)_tmp48D)!= 7)goto _LL36A;_tmp48E=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp48D)->f1;_tmp48F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp48D)->f2;_tmp490=((struct Cyc_Absyn_ArrayType_struct*)_tmp48D)->f3;_LL369: {
int okay;okay=(_tmp479 != 0?_tmp490 != 0: 0)?Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp479))>= Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp490)): 0;return(okay?Cyc_Tcutil_unify(_tmp477,_tmp48E): 0)?!
_tmp478.q_const?1: _tmp48F.q_const: 0;}_LL36A:;_LL36B: return 0;_LL367:;}return 0;
_LL34F: if(_tmp470 <= (void*)3?1:*((int*)_tmp470)!= 12)goto _LL351;_tmp47A=((struct
Cyc_Absyn_EnumType_struct*)_tmp470)->f2;_LL350:{void*_tmp491=t2;struct Cyc_Absyn_Enumdecl*
_tmp492;_LL36D: if(_tmp491 <= (void*)3?1:*((int*)_tmp491)!= 12)goto _LL36F;_tmp492=((
struct Cyc_Absyn_EnumType_struct*)_tmp491)->f2;_LL36E: if((_tmp47A->fields != 0?
_tmp492->fields != 0: 0)?((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp47A->fields))->v)>= ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp492->fields))->v): 0)return 1;goto _LL36C;_LL36F:;_LL370: goto _LL36C;
_LL36C:;}goto _LL352;_LL351: if(_tmp470 <= (void*)3?1:*((int*)_tmp470)!= 5)goto
_LL353;_LL352: goto _LL354;_LL353: if((int)_tmp470 != 1)goto _LL355;_LL354: goto _LL356;
_LL355: if(_tmp470 <= (void*)3?1:*((int*)_tmp470)!= 6)goto _LL357;_LL356: return Cyc_Tcutil_coerceable(
t2)?1: t2 == (void*)0;_LL357: if((int)_tmp470 != 0)goto _LL359;_LL358: return t2 == (
void*)0;_LL359:;_LL35A: return 0;_LL34A:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp493=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp494=_cycalloc(
sizeof(*_tmp494));_tmp494[0]=({struct Cyc_Absyn_Cast_e_struct _tmp495;_tmp495.tag=
13;_tmp495.f1=(void*)t;_tmp495.f2=_tmp493;_tmp495;});_tmp494;})));e->topt=({
struct Cyc_Core_Opt*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->v=(void*)t;
_tmp496;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*_tmp497=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL372: if(_tmp497 <= (void*)
3?1:*((int*)_tmp497)!= 5)goto _LL374;_LL373: goto _LL375;_LL374: if(_tmp497 <= (void*)
3?1:*((int*)_tmp497)!= 12)goto _LL376;_LL375: goto _LL377;_LL376: if(_tmp497 <= (void*)
3?1:*((int*)_tmp497)!= 13)goto _LL378;_LL377: goto _LL379;_LL378: if(_tmp497 <= (void*)
3?1:*((int*)_tmp497)!= 14)goto _LL37A;_LL379: return 1;_LL37A: if(_tmp497 <= (void*)3?
1:*((int*)_tmp497)!= 0)goto _LL37C;_LL37B: return Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_t);_LL37C:;_LL37D: return 0;
_LL371:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp498=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL37F: if((int)_tmp498 != 1)goto _LL381;_LL380: goto
_LL382;_LL381: if(_tmp498 <= (void*)3?1:*((int*)_tmp498)!= 6)goto _LL383;_LL382:
return 1;_LL383:;_LL384: return 0;_LL37E:;}}int Cyc_Tcutil_is_function_type(void*t){
void*_tmp499=Cyc_Tcutil_compress(t);_LL386: if(_tmp499 <= (void*)3?1:*((int*)
_tmp499)!= 8)goto _LL388;_LL387: return 1;_LL388:;_LL389: return 0;_LL385:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple6 _tmp49B=({struct _tuple6 _tmp49A;_tmp49A.f1=t1;
_tmp49A.f2=t2;_tmp49A;});void*_tmp49C;int _tmp49D;void*_tmp49E;int _tmp49F;void*
_tmp4A0;void*_tmp4A1;void*_tmp4A2;void*_tmp4A3;void*_tmp4A4;void*_tmp4A5;void*
_tmp4A6;void*_tmp4A7;void*_tmp4A8;void*_tmp4A9;void*_tmp4AA;void*_tmp4AB;void*
_tmp4AC;void*_tmp4AD;void*_tmp4AE;void*_tmp4AF;void*_tmp4B0;void*_tmp4B1;void*
_tmp4B2;void*_tmp4B3;void*_tmp4B4;void*_tmp4B5;_LL38B: _tmp49C=_tmp49B.f1;if(
_tmp49C <= (void*)3?1:*((int*)_tmp49C)!= 6)goto _LL38D;_tmp49D=((struct Cyc_Absyn_DoubleType_struct*)
_tmp49C)->f1;_tmp49E=_tmp49B.f2;if(_tmp49E <= (void*)3?1:*((int*)_tmp49E)!= 6)
goto _LL38D;_tmp49F=((struct Cyc_Absyn_DoubleType_struct*)_tmp49E)->f1;_LL38C: if(
_tmp49D)return t1;else{return t2;}_LL38D: _tmp4A0=_tmp49B.f1;if(_tmp4A0 <= (void*)3?
1:*((int*)_tmp4A0)!= 6)goto _LL38F;_LL38E: return t1;_LL38F: _tmp4A1=_tmp49B.f2;if(
_tmp4A1 <= (void*)3?1:*((int*)_tmp4A1)!= 6)goto _LL391;_LL390: return t2;_LL391:
_tmp4A2=_tmp49B.f1;if((int)_tmp4A2 != 1)goto _LL393;_LL392: goto _LL394;_LL393:
_tmp4A3=_tmp49B.f2;if((int)_tmp4A3 != 1)goto _LL395;_LL394: return(void*)1;_LL395:
_tmp4A4=_tmp49B.f1;if(_tmp4A4 <= (void*)3?1:*((int*)_tmp4A4)!= 5)goto _LL397;
_tmp4A5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A4)->f1;if((int)_tmp4A5 != 
1)goto _LL397;_tmp4A6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A4)->f2;if((
int)_tmp4A6 != 3)goto _LL397;_LL396: goto _LL398;_LL397: _tmp4A7=_tmp49B.f2;if(
_tmp4A7 <= (void*)3?1:*((int*)_tmp4A7)!= 5)goto _LL399;_tmp4A8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A7)->f1;if((int)_tmp4A8 != 1)goto _LL399;_tmp4A9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A7)->f2;if((int)_tmp4A9 != 3)goto _LL399;_LL398: return Cyc_Absyn_ulonglong_t;
_LL399: _tmp4AA=_tmp49B.f1;if(_tmp4AA <= (void*)3?1:*((int*)_tmp4AA)!= 5)goto
_LL39B;_tmp4AB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AA)->f1;if((int)
_tmp4AB != 0)goto _LL39B;_tmp4AC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AA)->f2;
if((int)_tmp4AC != 3)goto _LL39B;_LL39A: goto _LL39C;_LL39B: _tmp4AD=_tmp49B.f2;if(
_tmp4AD <= (void*)3?1:*((int*)_tmp4AD)!= 5)goto _LL39D;_tmp4AE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4AD)->f1;if((int)_tmp4AE != 0)goto _LL39D;_tmp4AF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4AD)->f2;if((int)_tmp4AF != 3)goto _LL39D;_LL39C: return Cyc_Absyn_slonglong_t;
_LL39D: _tmp4B0=_tmp49B.f1;if(_tmp4B0 <= (void*)3?1:*((int*)_tmp4B0)!= 5)goto
_LL39F;_tmp4B1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B0)->f1;if((int)
_tmp4B1 != 1)goto _LL39F;_tmp4B2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B0)->f2;
if((int)_tmp4B2 != 2)goto _LL39F;_LL39E: goto _LL3A0;_LL39F: _tmp4B3=_tmp49B.f2;if(
_tmp4B3 <= (void*)3?1:*((int*)_tmp4B3)!= 5)goto _LL3A1;_tmp4B4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B3)->f1;if((int)_tmp4B4 != 1)goto _LL3A1;_tmp4B5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B3)->f2;if((int)_tmp4B5 != 2)goto _LL3A1;_LL3A0: return Cyc_Absyn_uint_t;_LL3A1:;
_LL3A2: return Cyc_Absyn_sint_t;_LL38A:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp4B6=(void*)e->r;struct Cyc_Core_Opt*_tmp4B7;
_LL3A4: if(*((int*)_tmp4B6)!= 4)goto _LL3A6;_tmp4B7=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4B6)->f2;if(_tmp4B7 != 0)goto _LL3A6;_LL3A5:({void*_tmp4B8[0]={};Cyc_Tcutil_warn(
e->loc,_tag_arr("assignment in test",sizeof(char),19),_tag_arr(_tmp4B8,sizeof(
void*),0));});goto _LL3A3;_LL3A6:;_LL3A7: goto _LL3A3;_LL3A3:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp4BA=({struct _tuple6 _tmp4B9;_tmp4B9.f1=c1;_tmp4B9.f2=c2;_tmp4B9;});
void*_tmp4BB;void*_tmp4BC;void*_tmp4BD;void*_tmp4BE;void*_tmp4BF;struct Cyc_Core_Opt*
_tmp4C0;struct Cyc_Core_Opt**_tmp4C1;void*_tmp4C2;struct Cyc_Core_Opt*_tmp4C3;
struct Cyc_Core_Opt**_tmp4C4;void*_tmp4C5;struct Cyc_Core_Opt*_tmp4C6;struct Cyc_Core_Opt**
_tmp4C7;void*_tmp4C8;void*_tmp4C9;void*_tmp4CA;void*_tmp4CB;void*_tmp4CC;void*
_tmp4CD;struct Cyc_Core_Opt*_tmp4CE;struct Cyc_Core_Opt**_tmp4CF;void*_tmp4D0;void*
_tmp4D1;struct Cyc_Core_Opt*_tmp4D2;struct Cyc_Core_Opt**_tmp4D3;void*_tmp4D4;void*
_tmp4D5;struct Cyc_Core_Opt*_tmp4D6;struct Cyc_Core_Opt**_tmp4D7;void*_tmp4D8;
_LL3A9: _tmp4BB=_tmp4BA.f1;if(*((int*)_tmp4BB)!= 0)goto _LL3AB;_tmp4BC=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp4BB)->f1;_tmp4BD=_tmp4BA.f2;if(*((int*)_tmp4BD)
!= 0)goto _LL3AB;_tmp4BE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4BD)->f1;
_LL3AA: return _tmp4BC == _tmp4BE;_LL3AB: _tmp4BF=_tmp4BA.f2;if(*((int*)_tmp4BF)!= 1)
goto _LL3AD;_tmp4C0=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4BF)->f1;_tmp4C1=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4BF)->f1;_LL3AC:*
_tmp4C1=({struct Cyc_Core_Opt*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9->v=(
void*)c1;_tmp4D9;});return 1;_LL3AD: _tmp4C2=_tmp4BA.f1;if(*((int*)_tmp4C2)!= 1)
goto _LL3AF;_tmp4C3=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4C2)->f1;_tmp4C4=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4C2)->f1;_LL3AE:*
_tmp4C4=({struct Cyc_Core_Opt*_tmp4DA=_cycalloc(sizeof(*_tmp4DA));_tmp4DA->v=(
void*)c2;_tmp4DA;});return 1;_LL3AF: _tmp4C5=_tmp4BA.f1;if(*((int*)_tmp4C5)!= 2)
goto _LL3B1;_tmp4C6=((struct Cyc_Absyn_Less_kb_struct*)_tmp4C5)->f1;_tmp4C7=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp4C5)->f1;_tmp4C8=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4C5)->f2;_tmp4C9=_tmp4BA.f2;if(*((
int*)_tmp4C9)!= 0)goto _LL3B1;_tmp4CA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp4C9)->f1;_LL3B0: if(Cyc_Tcutil_kind_leq(_tmp4CA,_tmp4C8)){*_tmp4C7=({struct
Cyc_Core_Opt*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->v=(void*)c2;_tmp4DB;});
return 1;}else{return 0;}_LL3B1: _tmp4CB=_tmp4BA.f1;if(*((int*)_tmp4CB)!= 0)goto
_LL3B3;_tmp4CC=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4CB)->f1;_tmp4CD=
_tmp4BA.f2;if(*((int*)_tmp4CD)!= 2)goto _LL3B3;_tmp4CE=((struct Cyc_Absyn_Less_kb_struct*)
_tmp4CD)->f1;_tmp4CF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp4CD)->f1;_tmp4D0=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4CD)->f2;
_LL3B2: if(Cyc_Tcutil_kind_leq(_tmp4CC,_tmp4D0)){*_tmp4CF=({struct Cyc_Core_Opt*
_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->v=(void*)c1;_tmp4DC;});return 1;}
else{return 0;}_LL3B3: _tmp4D1=_tmp4BA.f1;if(*((int*)_tmp4D1)!= 2)goto _LL3A8;
_tmp4D2=((struct Cyc_Absyn_Less_kb_struct*)_tmp4D1)->f1;_tmp4D3=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4D1)->f1;_tmp4D4=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4D1)->f2;_tmp4D5=_tmp4BA.f2;if(*((int*)_tmp4D5)!= 2)goto _LL3A8;_tmp4D6=((
struct Cyc_Absyn_Less_kb_struct*)_tmp4D5)->f1;_tmp4D7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4D5)->f1;_tmp4D8=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4D5)->f2;_LL3B4: if(Cyc_Tcutil_kind_leq(_tmp4D4,_tmp4D8)){*_tmp4D7=({struct
Cyc_Core_Opt*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->v=(void*)c1;_tmp4DD;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp4D8,_tmp4D4)){*_tmp4D3=({struct Cyc_Core_Opt*
_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE->v=(void*)c2;_tmp4DE;});return 1;}
else{return 0;}}_LL3A8:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp4DF=_cycalloc_atomic(sizeof(*_tmp4DF));_tmp4DF[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp4DF;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp4E3;_tmp4E3.tag=1;_tmp4E3.f1=(int)((unsigned int)
i);{void*_tmp4E2[1]={& _tmp4E3};Cyc_Std_aprintf(_tag_arr("#%d",sizeof(char),4),
_tag_arr(_tmp4E2,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*_tmp4E0=
_cycalloc(sizeof(*_tmp4E0));_tmp4E0->name=({struct _tagged_arr*_tmp4E1=_cycalloc(
sizeof(struct _tagged_arr)* 1);_tmp4E1[0]=s;_tmp4E1;});_tmp4E0->identity=0;
_tmp4E0->kind=(void*)k;_tmp4E0;});}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _tagged_arr _tmp4E4=*t->name;return*((const char*)
_check_unknown_subscript(_tmp4E4,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_Std_String_pa_struct _tmp4E6;_tmp4E6.tag=0;
_tmp4E6.f1=(struct _tagged_arr)*t->name;{void*_tmp4E5[1]={& _tmp4E6};Cyc_Std_printf(
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp4E5,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp4E7=Cyc_Std_strconcat(_tag_arr("`",sizeof(char),
2),*t->name);*((char*)_check_unknown_subscript(_tmp4E7,sizeof(char),1))='t';t->name=({
struct _tagged_arr*_tmp4E8=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp4E8[0]=(
struct _tagged_arr)_tmp4E7;_tmp4E8;});}}struct _tuple14{struct _tagged_arr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple14*x){return({struct _tuple2*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));
_tmp4E9->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp4EA=_cycalloc(sizeof(*
_tmp4EA));_tmp4EA->v=(*x).f1;_tmp4EA;});_tmp4E9->f2=(*x).f2;_tmp4E9->f3=(*x).f3;
_tmp4E9;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp4EB=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp4EB=({
struct Cyc_List_List*_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC->hd=(void*)((
void*)atts->hd);_tmp4EC->tl=_tmp4EB;_tmp4EC;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED[0]=({struct Cyc_Absyn_FnType_struct
_tmp4EE;_tmp4EE.tag=8;_tmp4EE.f1=({struct Cyc_Absyn_FnInfo _tmp4EF;_tmp4EF.tvars=
fd->tvs;_tmp4EF.effect=fd->effect;_tmp4EF.ret_typ=(void*)((void*)fd->ret_type);
_tmp4EF.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp4EF.c_varargs=
fd->c_varargs;_tmp4EF.cyc_varargs=fd->cyc_varargs;_tmp4EF.rgn_po=fd->rgn_po;
_tmp4EF.attributes=_tmp4EB;_tmp4EF;});_tmp4EE;});_tmp4ED;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple15{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple15*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->f1=(*pr).f1;_tmp4F0->f2=t;_tmp4F0;});}
struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple17{
struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple17*_tmp4F1=_region_malloc(
rgn,sizeof(*_tmp4F1));_tmp4F1->f1=({struct _tuple16*_tmp4F2=_region_malloc(rgn,
sizeof(*_tmp4F2));_tmp4F2->f1=(*y).f1;_tmp4F2->f2=(*y).f2;_tmp4F2;});_tmp4F1->f2=(*
y).f3;_tmp4F1;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple17*w){
struct _tuple16*_tmp4F4;void*_tmp4F5;struct _tuple17 _tmp4F3=*w;_tmp4F4=_tmp4F3.f1;
_tmp4F5=_tmp4F3.f2;{struct Cyc_Core_Opt*_tmp4F7;struct Cyc_Absyn_Tqual _tmp4F8;
struct _tuple16 _tmp4F6=*_tmp4F4;_tmp4F7=_tmp4F6.f1;_tmp4F8=_tmp4F6.f2;return({
struct _tuple2*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->f1=_tmp4F7;_tmp4F9->f2=
_tmp4F8;_tmp4F9->f3=_tmp4F5;_tmp4F9;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp4FA=
_cycalloc(sizeof(*_tmp4FA));_tmp4FA->name=f->name;_tmp4FA->tq=f->tq;_tmp4FA->type=(
void*)t;_tmp4FA->width=f->width;_tmp4FA->attributes=f->attributes;_tmp4FA;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp4FB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp4FC;struct Cyc_Absyn_AggrInfo _tmp4FD;void*_tmp4FE;struct Cyc_List_List*_tmp4FF;
struct Cyc_Absyn_TunionInfo _tmp500;void*_tmp501;struct Cyc_List_List*_tmp502;void*
_tmp503;struct Cyc_Absyn_TunionFieldInfo _tmp504;void*_tmp505;struct Cyc_List_List*
_tmp506;struct _tuple1*_tmp507;struct Cyc_List_List*_tmp508;struct Cyc_Absyn_Typedefdecl*
_tmp509;struct Cyc_Core_Opt*_tmp50A;void*_tmp50B;struct Cyc_Absyn_Tqual _tmp50C;
struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_PtrInfo _tmp50E;void*_tmp50F;void*
_tmp510;struct Cyc_Absyn_Conref*_tmp511;struct Cyc_Absyn_Tqual _tmp512;struct Cyc_Absyn_Conref*
_tmp513;struct Cyc_Absyn_FnInfo _tmp514;struct Cyc_List_List*_tmp515;struct Cyc_Core_Opt*
_tmp516;void*_tmp517;struct Cyc_List_List*_tmp518;int _tmp519;struct Cyc_Absyn_VarargInfo*
_tmp51A;struct Cyc_List_List*_tmp51B;struct Cyc_List_List*_tmp51C;struct Cyc_List_List*
_tmp51D;void*_tmp51E;struct Cyc_List_List*_tmp51F;struct Cyc_Core_Opt*_tmp520;void*
_tmp521;void*_tmp522;void*_tmp523;void*_tmp524;struct Cyc_List_List*_tmp525;
_LL3B6: if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 1)goto _LL3B8;_tmp4FC=((struct
Cyc_Absyn_VarType_struct*)_tmp4FB)->f1;_LL3B7: {struct _handler_cons _tmp526;
_push_handler(& _tmp526);{int _tmp528=0;if(setjmp(_tmp526.handler))_tmp528=1;if(!
_tmp528){{void*_tmp529=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp4FC);_npop_handler(0);return _tmp529;};_pop_handler();}else{void*_tmp527=(
void*)_exn_thrown;void*_tmp52B=_tmp527;_LL3E5: if(_tmp52B != Cyc_Core_Not_found)
goto _LL3E7;_LL3E6: return t;_LL3E7:;_LL3E8:(void)_throw(_tmp52B);_LL3E4:;}}}_LL3B8:
if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 10)goto _LL3BA;_tmp4FD=((struct Cyc_Absyn_AggrType_struct*)
_tmp4FB)->f1;_tmp4FE=(void*)_tmp4FD.aggr_info;_tmp4FF=_tmp4FD.targs;_LL3B9: {
struct Cyc_List_List*_tmp52C=Cyc_Tcutil_substs(rgn,inst,_tmp4FF);return _tmp52C == 
_tmp4FF?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp52D=_cycalloc(sizeof(*
_tmp52D));_tmp52D[0]=({struct Cyc_Absyn_AggrType_struct _tmp52E;_tmp52E.tag=10;
_tmp52E.f1=({struct Cyc_Absyn_AggrInfo _tmp52F;_tmp52F.aggr_info=(void*)_tmp4FE;
_tmp52F.targs=_tmp52C;_tmp52F;});_tmp52E;});_tmp52D;});}_LL3BA: if(_tmp4FB <= (
void*)3?1:*((int*)_tmp4FB)!= 2)goto _LL3BC;_tmp500=((struct Cyc_Absyn_TunionType_struct*)
_tmp4FB)->f1;_tmp501=(void*)_tmp500.tunion_info;_tmp502=_tmp500.targs;_tmp503=(
void*)_tmp500.rgn;_LL3BB: {struct Cyc_List_List*_tmp530=Cyc_Tcutil_substs(rgn,
inst,_tmp502);void*_tmp531=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp503);return(
_tmp530 == _tmp502?_tmp531 == _tmp503: 0)?t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532[0]=({struct Cyc_Absyn_TunionType_struct
_tmp533;_tmp533.tag=2;_tmp533.f1=({struct Cyc_Absyn_TunionInfo _tmp534;_tmp534.tunion_info=(
void*)_tmp501;_tmp534.targs=_tmp530;_tmp534.rgn=(void*)_tmp531;_tmp534;});
_tmp533;});_tmp532;});}_LL3BC: if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 3)goto
_LL3BE;_tmp504=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4FB)->f1;_tmp505=(
void*)_tmp504.field_info;_tmp506=_tmp504.targs;_LL3BD: {struct Cyc_List_List*
_tmp535=Cyc_Tcutil_substs(rgn,inst,_tmp506);return _tmp535 == _tmp506?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp536=_cycalloc(sizeof(*_tmp536));
_tmp536[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp537;_tmp537.tag=3;_tmp537.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp538;_tmp538.field_info=(void*)_tmp505;_tmp538.targs=
_tmp535;_tmp538;});_tmp537;});_tmp536;});}_LL3BE: if(_tmp4FB <= (void*)3?1:*((int*)
_tmp4FB)!= 16)goto _LL3C0;_tmp507=((struct Cyc_Absyn_TypedefType_struct*)_tmp4FB)->f1;
_tmp508=((struct Cyc_Absyn_TypedefType_struct*)_tmp4FB)->f2;_tmp509=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4FB)->f3;_tmp50A=((struct Cyc_Absyn_TypedefType_struct*)_tmp4FB)->f4;_LL3BF: {
struct Cyc_List_List*_tmp539=Cyc_Tcutil_substs(rgn,inst,_tmp508);return _tmp539 == 
_tmp508?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp53A=_cycalloc(sizeof(*
_tmp53A));_tmp53A[0]=({struct Cyc_Absyn_TypedefType_struct _tmp53B;_tmp53B.tag=16;
_tmp53B.f1=_tmp507;_tmp53B.f2=_tmp539;_tmp53B.f3=_tmp509;_tmp53B.f4=_tmp50A;
_tmp53B;});_tmp53A;});}_LL3C0: if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 7)goto
_LL3C2;_tmp50B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp4FB)->f1;_tmp50C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4FB)->f2;_tmp50D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4FB)->f3;_LL3C1: {void*_tmp53C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp50B);
return _tmp53C == _tmp50B?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp53D=
_cycalloc(sizeof(*_tmp53D));_tmp53D[0]=({struct Cyc_Absyn_ArrayType_struct _tmp53E;
_tmp53E.tag=7;_tmp53E.f1=(void*)_tmp53C;_tmp53E.f2=_tmp50C;_tmp53E.f3=_tmp50D;
_tmp53E;});_tmp53D;});}_LL3C2: if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 4)goto
_LL3C4;_tmp50E=((struct Cyc_Absyn_PointerType_struct*)_tmp4FB)->f1;_tmp50F=(void*)
_tmp50E.elt_typ;_tmp510=(void*)_tmp50E.rgn_typ;_tmp511=_tmp50E.nullable;_tmp512=
_tmp50E.tq;_tmp513=_tmp50E.bounds;_LL3C3: {void*_tmp53F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp50F);void*_tmp540=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp510);if(
_tmp53F == _tmp50F?_tmp540 == _tmp510: 0)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541[0]=({struct Cyc_Absyn_PointerType_struct
_tmp542;_tmp542.tag=4;_tmp542.f1=({struct Cyc_Absyn_PtrInfo _tmp543;_tmp543.elt_typ=(
void*)_tmp53F;_tmp543.rgn_typ=(void*)_tmp540;_tmp543.nullable=_tmp511;_tmp543.tq=
_tmp512;_tmp543.bounds=_tmp513;_tmp543;});_tmp542;});_tmp541;});}_LL3C4: if(
_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 8)goto _LL3C6;_tmp514=((struct Cyc_Absyn_FnType_struct*)
_tmp4FB)->f1;_tmp515=_tmp514.tvars;_tmp516=_tmp514.effect;_tmp517=(void*)_tmp514.ret_typ;
_tmp518=_tmp514.args;_tmp519=_tmp514.c_varargs;_tmp51A=_tmp514.cyc_varargs;
_tmp51B=_tmp514.rgn_po;_tmp51C=_tmp514.attributes;_LL3C5:{struct Cyc_List_List*
_tmp544=_tmp515;for(0;_tmp544 != 0;_tmp544=_tmp544->tl){inst=({struct Cyc_List_List*
_tmp545=_region_malloc(rgn,sizeof(*_tmp545));_tmp545->hd=({struct _tuple7*_tmp546=
_region_malloc(rgn,sizeof(*_tmp546));_tmp546->f1=(struct Cyc_Absyn_Tvar*)_tmp544->hd;
_tmp546->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp547=_cycalloc(sizeof(*
_tmp547));_tmp547[0]=({struct Cyc_Absyn_VarType_struct _tmp548;_tmp548.tag=1;
_tmp548.f1=(struct Cyc_Absyn_Tvar*)_tmp544->hd;_tmp548;});_tmp547;});_tmp546;});
_tmp545->tl=inst;_tmp545;});}}{struct Cyc_List_List*_tmp54A;struct Cyc_List_List*
_tmp54B;struct _tuple0 _tmp549=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp518));_tmp54A=_tmp549.f1;_tmp54B=_tmp549.f2;{struct Cyc_List_List*_tmp54C=
Cyc_Tcutil_substs(rgn,inst,_tmp54B);struct Cyc_List_List*_tmp54D=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp54A,_tmp54C));struct Cyc_Core_Opt*
eff2;if(_tmp516 == 0)eff2=0;else{void*_tmp54E=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp516->v);if(_tmp54E == (void*)_tmp516->v)eff2=_tmp516;else{eff2=({struct
Cyc_Core_Opt*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->v=(void*)_tmp54E;
_tmp54F;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp51A == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp551;struct Cyc_Absyn_Tqual _tmp552;void*
_tmp553;int _tmp554;struct Cyc_Absyn_VarargInfo _tmp550=*_tmp51A;_tmp551=_tmp550.name;
_tmp552=_tmp550.tq;_tmp553=(void*)_tmp550.type;_tmp554=_tmp550.inject;{void*
_tmp555=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp553);if(_tmp555 == _tmp553)
cyc_varargs2=_tmp51A;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp556=
_cycalloc(sizeof(*_tmp556));_tmp556->name=_tmp551;_tmp556->tq=_tmp552;_tmp556->type=(
void*)_tmp555;_tmp556->inject=_tmp554;_tmp556;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp558;struct Cyc_List_List*_tmp559;struct _tuple0 _tmp557=Cyc_List_rsplit(
rgn,rgn,_tmp51B);_tmp558=_tmp557.f1;_tmp559=_tmp557.f2;{struct Cyc_List_List*
_tmp55A=Cyc_Tcutil_substs(rgn,inst,_tmp558);struct Cyc_List_List*_tmp55B=Cyc_Tcutil_substs(
rgn,inst,_tmp559);if(_tmp55A == _tmp558?_tmp55B == _tmp559: 0)rgn_po2=_tmp51B;else{
rgn_po2=Cyc_List_zip(_tmp55A,_tmp55B);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C[0]=({struct Cyc_Absyn_FnType_struct
_tmp55D;_tmp55D.tag=8;_tmp55D.f1=({struct Cyc_Absyn_FnInfo _tmp55E;_tmp55E.tvars=
_tmp515;_tmp55E.effect=eff2;_tmp55E.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp517);_tmp55E.args=_tmp54D;_tmp55E.c_varargs=_tmp519;_tmp55E.cyc_varargs=
cyc_varargs2;_tmp55E.rgn_po=rgn_po2;_tmp55E.attributes=_tmp51C;_tmp55E;});
_tmp55D;});_tmp55C;});}}}}}_LL3C6: if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 9)
goto _LL3C8;_tmp51D=((struct Cyc_Absyn_TupleType_struct*)_tmp4FB)->f1;_LL3C7: {
struct Cyc_List_List*_tmp55F=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp51D);struct Cyc_List_List*_tmp560=Cyc_Tcutil_substs(rgn,inst,_tmp55F);if(
_tmp560 == _tmp55F)return t;{struct Cyc_List_List*_tmp561=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp51D,_tmp560);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562[0]=({struct Cyc_Absyn_TupleType_struct
_tmp563;_tmp563.tag=9;_tmp563.f1=_tmp561;_tmp563;});_tmp562;});}}_LL3C8: if(
_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 11)goto _LL3CA;_tmp51E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4FB)->f1;_tmp51F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp4FB)->f2;_LL3C9: {
struct Cyc_List_List*_tmp564=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp51F);struct Cyc_List_List*_tmp565=Cyc_Tcutil_substs(rgn,inst,_tmp564);if(
_tmp565 == _tmp564)return t;{struct Cyc_List_List*_tmp566=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp51F,
_tmp565);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp567=_cycalloc(
sizeof(*_tmp567));_tmp567[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp568;
_tmp568.tag=11;_tmp568.f1=(void*)_tmp51E;_tmp568.f2=_tmp566;_tmp568;});_tmp567;});}}
_LL3CA: if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 0)goto _LL3CC;_tmp520=((struct
Cyc_Absyn_Evar_struct*)_tmp4FB)->f2;_LL3CB: if(_tmp520 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp520->v);else{return t;}_LL3CC: if(_tmp4FB <= (void*)3?1:*((int*)
_tmp4FB)!= 15)goto _LL3CE;_tmp521=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4FB)->f1;_LL3CD: {void*_tmp569=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp521);
return _tmp569 == _tmp521?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp56A=
_cycalloc(sizeof(*_tmp56A));_tmp56A[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp56B;_tmp56B.tag=15;_tmp56B.f1=(void*)_tmp569;_tmp56B;});_tmp56A;});}_LL3CE:
if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 14)goto _LL3D0;_tmp522=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp4FB)->f1;_LL3CF: {void*_tmp56C=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp522);return _tmp56C == _tmp522?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp56E;_tmp56E.tag=14;_tmp56E.f1=(void*)_tmp56C;_tmp56E;});_tmp56D;});}_LL3D0:
if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 12)goto _LL3D2;_LL3D1: goto _LL3D3;
_LL3D2: if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 13)goto _LL3D4;_LL3D3: goto
_LL3D5;_LL3D4: if((int)_tmp4FB != 0)goto _LL3D6;_LL3D5: goto _LL3D7;_LL3D6: if(_tmp4FB
<= (void*)3?1:*((int*)_tmp4FB)!= 5)goto _LL3D8;_LL3D7: goto _LL3D9;_LL3D8: if((int)
_tmp4FB != 1)goto _LL3DA;_LL3D9: goto _LL3DB;_LL3DA: if(_tmp4FB <= (void*)3?1:*((int*)
_tmp4FB)!= 6)goto _LL3DC;_LL3DB: goto _LL3DD;_LL3DC: if((int)_tmp4FB != 2)goto _LL3DE;
_LL3DD: return t;_LL3DE: if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 19)goto _LL3E0;
_tmp523=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp4FB)->f1;_LL3DF: {void*
_tmp56F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp523);return _tmp56F == _tmp523?t:(void*)({
struct Cyc_Absyn_RgnsEff_struct*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570[0]=({
struct Cyc_Absyn_RgnsEff_struct _tmp571;_tmp571.tag=19;_tmp571.f1=(void*)_tmp56F;
_tmp571;});_tmp570;});}_LL3E0: if(_tmp4FB <= (void*)3?1:*((int*)_tmp4FB)!= 17)goto
_LL3E2;_tmp524=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp4FB)->f1;_LL3E1: {
void*_tmp572=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp524);return _tmp572 == _tmp524?t:(
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp573=_cycalloc(sizeof(*_tmp573));
_tmp573[0]=({struct Cyc_Absyn_AccessEff_struct _tmp574;_tmp574.tag=17;_tmp574.f1=(
void*)_tmp572;_tmp574;});_tmp573;});}_LL3E2: if(_tmp4FB <= (void*)3?1:*((int*)
_tmp4FB)!= 18)goto _LL3B5;_tmp525=((struct Cyc_Absyn_JoinEff_struct*)_tmp4FB)->f1;
_LL3E3: {struct Cyc_List_List*_tmp575=Cyc_Tcutil_substs(rgn,inst,_tmp525);return
_tmp575 == _tmp525?t:(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp576=_cycalloc(
sizeof(*_tmp576));_tmp576[0]=({struct Cyc_Absyn_JoinEff_struct _tmp577;_tmp577.tag=
18;_tmp577.f1=_tmp575;_tmp577;});_tmp576;});}_LL3B5:;}static struct Cyc_List_List*
Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*
ts){if(ts == 0)return 0;{void*_tmp578=(void*)ts->hd;struct Cyc_List_List*_tmp579=ts->tl;
void*_tmp57A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp578);struct Cyc_List_List*
_tmp57B=Cyc_Tcutil_substs(rgn,inst,_tmp579);if(_tmp578 == _tmp57A?_tmp579 == 
_tmp57B: 0)return ts;return(struct Cyc_List_List*)({struct Cyc_List_List*_tmp57C=
_cycalloc(sizeof(*_tmp57C));_tmp57C->hd=(void*)_tmp57A;_tmp57C->tl=_tmp57B;
_tmp57C;});}}extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}struct _tuple7*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){void*k=Cyc_Tcutil_tvar_kind(tv);
return({struct _tuple7*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->f1=tv;_tmp57D->f2=
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp57E=_cycalloc(sizeof(*_tmp57E));
_tmp57E->v=(void*)k;_tmp57E;}),({struct Cyc_Core_Opt*_tmp57F=_cycalloc(sizeof(*
_tmp57F));_tmp57F->v=s;_tmp57F;}));_tmp57D;});}struct _tuple7*Cyc_Tcutil_r_make_inst_var(
struct _tuple8*env,struct Cyc_Absyn_Tvar*tv){struct _tuple8 _tmp581;struct Cyc_List_List*
_tmp582;struct _RegionHandle*_tmp583;struct _tuple8*_tmp580=env;_tmp581=*_tmp580;
_tmp582=_tmp581.f1;_tmp583=_tmp581.f2;{void*k=Cyc_Tcutil_tvar_kind(tv);return({
struct _tuple7*_tmp584=_region_malloc(_tmp583,sizeof(*_tmp584));_tmp584->f1=tv;
_tmp584->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp585=_cycalloc(sizeof(*
_tmp585));_tmp585->v=(void*)k;_tmp585;}),({struct Cyc_Core_Opt*_tmp586=_cycalloc(
sizeof(*_tmp586));_tmp586->v=_tmp582;_tmp586;}));_tmp584;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_Std_zstrptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=(void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))({struct Cyc_Std_String_pa_struct _tmp58A;
_tmp58A.tag=0;_tmp58A.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string(k2);{
struct Cyc_Std_String_pa_struct _tmp589;_tmp589.tag=0;_tmp589.f1=(struct
_tagged_arr)Cyc_Absynpp_kindbound2string(k1);{struct Cyc_Std_String_pa_struct
_tmp588;_tmp588.tag=0;_tmp588.f1=(struct _tagged_arr)*tv->name;{void*_tmp587[3]={&
_tmp588,& _tmp589,& _tmp58A};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof(char),59),_tag_arr(_tmp587,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp58B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("same type variable has different identity!",sizeof(char),43),_tag_arr(
_tmp58B,sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C->hd=tv;
_tmp58C->tl=tvs;_tmp58C;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*
_tmp58D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv",sizeof(char),39),_tag_arr(
_tmp58D,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp58E[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof(char),41),_tag_arr(_tmp58E,sizeof(void*),0));});if(*((int*)_check_null(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)_check_null(tv->identity)))
return tvs;}}return({struct Cyc_List_List*_tmp58F=_cycalloc(sizeof(*_tmp58F));
_tmp58F->hd=tv;_tmp58F->tl=tvs;_tmp58F;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_Std_String_pa_struct
_tmp591;_tmp591.tag=0;_tmp591.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string(tv);{
void*_tmp590[1]={& _tmp591};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bound tvar id for %s is NULL",sizeof(char),29),
_tag_arr(_tmp590,sizeof(void*),1));}});return({struct Cyc_List_List*_tmp592=
_cycalloc(sizeof(*_tmp592));_tmp592->hd=tv;_tmp592->tl=tvs;_tmp592;});}static
struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){void*
_tmp593=Cyc_Tcutil_compress(e);int _tmp594;_LL3EA: if(_tmp593 <= (void*)3?1:*((int*)
_tmp593)!= 0)goto _LL3EC;_tmp594=((struct Cyc_Absyn_Evar_struct*)_tmp593)->f3;
_LL3EB:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*_tmp595=Cyc_Tcutil_compress((
void*)es2->hd);int _tmp596;_LL3EF: if(_tmp595 <= (void*)3?1:*((int*)_tmp595)!= 0)
goto _LL3F1;_tmp596=((struct Cyc_Absyn_Evar_struct*)_tmp595)->f3;_LL3F0: if(_tmp594
== _tmp596)return es;goto _LL3EE;_LL3F1:;_LL3F2: goto _LL3EE;_LL3EE:;}}return({
struct Cyc_List_List*_tmp597=_cycalloc(sizeof(*_tmp597));_tmp597->hd=(void*)e;
_tmp597->tl=es;_tmp597;});_LL3EC:;_LL3ED: return es;_LL3E9:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))
== *((int*)_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;
break;}}}if(!present)r=({struct Cyc_List_List*_tmp598=_cycalloc(sizeof(*_tmp598));
_tmp598->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp598->tl=r;_tmp598;});}r=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(width)))({struct Cyc_Std_String_pa_struct
_tmp59A;_tmp59A.tag=0;_tmp59A.f1=(struct _tagged_arr)*fn;{void*_tmp599[1]={&
_tmp59A};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s does not have constant width",
sizeof(char),41),_tag_arr(_tmp599,sizeof(void*),1));}});else{w=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(width));}{void*_tmp59B=Cyc_Tcutil_compress(
field_typ);void*_tmp59C;_LL3F4: if(_tmp59B <= (void*)3?1:*((int*)_tmp59B)!= 5)goto
_LL3F6;_tmp59C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp59B)->f2;_LL3F5:{
void*_tmp59D=_tmp59C;_LL3F9: if((int)_tmp59D != 0)goto _LL3FB;_LL3FA: if(w > 8)({void*
_tmp59E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",sizeof(
char),26),_tag_arr(_tmp59E,sizeof(void*),0));});goto _LL3F8;_LL3FB: if((int)
_tmp59D != 1)goto _LL3FD;_LL3FC: if(w > 16)({void*_tmp59F[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp59F,sizeof(
void*),0));});goto _LL3F8;_LL3FD: if((int)_tmp59D != 2)goto _LL3FF;_LL3FE: if(w > 32)({
void*_tmp5A0[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(char),26),_tag_arr(_tmp5A0,sizeof(void*),0));});goto _LL3F8;_LL3FF: if((int)
_tmp59D != 3)goto _LL3F8;_LL400: if(w > 64)({void*_tmp5A1[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp5A1,sizeof(
void*),0));});goto _LL3F8;_LL3F8:;}goto _LL3F3;_LL3F6:;_LL3F7:({struct Cyc_Std_String_pa_struct
_tmp5A4;_tmp5A4.tag=0;_tmp5A4.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
field_typ);{struct Cyc_Std_String_pa_struct _tmp5A3;_tmp5A3.tag=0;_tmp5A3.f1=(
struct _tagged_arr)*fn;{void*_tmp5A2[2]={& _tmp5A3,& _tmp5A4};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield %s must have integral type but has type %s",sizeof(char),52),
_tag_arr(_tmp5A2,sizeof(void*),2));}}});goto _LL3F3;_LL3F3:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp5A5=(void*)atts->hd;_LL402: if((int)
_tmp5A5 != 5)goto _LL404;_LL403: continue;_LL404: if(_tmp5A5 <= (void*)16?1:*((int*)
_tmp5A5)!= 1)goto _LL406;_LL405: continue;_LL406:;_LL407:({struct Cyc_Std_String_pa_struct
_tmp5A8;_tmp5A8.tag=0;_tmp5A8.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_String_pa_struct
_tmp5A7;_tmp5A7.tag=0;_tmp5A7.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp5A6[2]={& _tmp5A7,& _tmp5A8};Cyc_Tcutil_terr(loc,
_tag_arr("bad attribute %s on member %s",sizeof(char),30),_tag_arr(_tmp5A6,
sizeof(void*),2));}}});_LL401:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};struct _tuple18{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t){{void*_tmp5A9=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp5AA;struct Cyc_Core_Opt**_tmp5AB;struct Cyc_Core_Opt*
_tmp5AC;struct Cyc_Core_Opt**_tmp5AD;struct Cyc_Absyn_Tvar*_tmp5AE;struct Cyc_List_List*
_tmp5AF;struct _tuple1*_tmp5B0;struct Cyc_Absyn_Enumdecl*_tmp5B1;struct Cyc_Absyn_Enumdecl**
_tmp5B2;struct Cyc_Absyn_TunionInfo _tmp5B3;void*_tmp5B4;void**_tmp5B5;struct Cyc_List_List*
_tmp5B6;struct Cyc_List_List**_tmp5B7;void*_tmp5B8;struct Cyc_Absyn_TunionFieldInfo
_tmp5B9;void*_tmp5BA;void**_tmp5BB;struct Cyc_List_List*_tmp5BC;struct Cyc_Absyn_PtrInfo
_tmp5BD;void*_tmp5BE;void*_tmp5BF;struct Cyc_Absyn_Conref*_tmp5C0;struct Cyc_Absyn_Tqual
_tmp5C1;struct Cyc_Absyn_Conref*_tmp5C2;void*_tmp5C3;void*_tmp5C4;struct Cyc_Absyn_Tqual
_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C6;struct Cyc_Absyn_FnInfo _tmp5C7;struct Cyc_List_List*
_tmp5C8;struct Cyc_List_List**_tmp5C9;struct Cyc_Core_Opt*_tmp5CA;struct Cyc_Core_Opt**
_tmp5CB;void*_tmp5CC;struct Cyc_List_List*_tmp5CD;int _tmp5CE;struct Cyc_Absyn_VarargInfo*
_tmp5CF;struct Cyc_List_List*_tmp5D0;struct Cyc_List_List*_tmp5D1;struct Cyc_List_List*
_tmp5D2;void*_tmp5D3;struct Cyc_List_List*_tmp5D4;struct Cyc_Absyn_AggrInfo _tmp5D5;
void*_tmp5D6;void**_tmp5D7;struct Cyc_List_List*_tmp5D8;struct Cyc_List_List**
_tmp5D9;struct _tuple1*_tmp5DA;struct Cyc_List_List*_tmp5DB;struct Cyc_List_List**
_tmp5DC;struct Cyc_Absyn_Typedefdecl*_tmp5DD;struct Cyc_Absyn_Typedefdecl**_tmp5DE;
struct Cyc_Core_Opt*_tmp5DF;struct Cyc_Core_Opt**_tmp5E0;void*_tmp5E1;void*_tmp5E2;
void*_tmp5E3;struct Cyc_List_List*_tmp5E4;_LL409: if((int)_tmp5A9 != 0)goto _LL40B;
_LL40A: goto _LL408;_LL40B: if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 0)goto _LL40D;
_tmp5AA=((struct Cyc_Absyn_Evar_struct*)_tmp5A9)->f1;_tmp5AB=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp5A9)->f1;_tmp5AC=((struct Cyc_Absyn_Evar_struct*)
_tmp5A9)->f2;_tmp5AD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp5A9)->f2;_LL40C: if(*_tmp5AB == 0)*_tmp5AB=({struct Cyc_Core_Opt*_tmp5E5=
_cycalloc(sizeof(*_tmp5E5));_tmp5E5->v=(void*)expected_kind;_tmp5E5;});if((
cvtenv.fn_result?cvtenv.generalize_evars: 0)?expected_kind == (void*)3: 0)*_tmp5AD=({
struct Cyc_Core_Opt*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6->v=(void*)((void*)
2);_tmp5E6;});else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Tvar*_tmp5E7=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Less_kb_struct*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));
_tmp5EB[0]=({struct Cyc_Absyn_Less_kb_struct _tmp5EC;_tmp5EC.tag=2;_tmp5EC.f1=0;
_tmp5EC.f2=(void*)expected_kind;_tmp5EC;});_tmp5EB;}));*_tmp5AD=({struct Cyc_Core_Opt*
_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9[0]=({struct Cyc_Absyn_VarType_struct
_tmp5EA;_tmp5EA.tag=1;_tmp5EA.f1=_tmp5E7;_tmp5EA;});_tmp5E9;}));_tmp5E8;});
_tmp5AE=_tmp5E7;goto _LL40E;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.free_evars,t);}}goto _LL408;_LL40D: if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)
!= 1)goto _LL40F;_tmp5AE=((struct Cyc_Absyn_VarType_struct*)_tmp5A9)->f1;_LL40E:{
void*_tmp5ED=Cyc_Absyn_compress_kb((void*)_tmp5AE->kind);struct Cyc_Core_Opt*
_tmp5EE;struct Cyc_Core_Opt**_tmp5EF;_LL438: if(*((int*)_tmp5ED)!= 1)goto _LL43A;
_tmp5EE=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5ED)->f1;_tmp5EF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp5ED)->f1;_LL439:*_tmp5EF=({struct Cyc_Core_Opt*
_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp5F2;_tmp5F2.tag=2;_tmp5F2.f1=0;_tmp5F2.f2=(void*)expected_kind;_tmp5F2;});
_tmp5F1;}));_tmp5F0;});goto _LL437;_LL43A:;_LL43B: goto _LL437;_LL437:;}cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp5AE);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp5AE);goto _LL408;_LL40F: if(_tmp5A9 <= (void*)3?1:*((int*)
_tmp5A9)!= 13)goto _LL411;_tmp5AF=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp5A9)->f1;
_LL410: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp5F3=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp5F3;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp5AF != 0;_tmp5AF=_tmp5AF->tl){struct Cyc_Absyn_Enumfield*_tmp5F4=(
struct Cyc_Absyn_Enumfield*)_tmp5AF->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp5F4->name).f2))({struct Cyc_Std_String_pa_struct
_tmp5F6;_tmp5F6.tag=0;_tmp5F6.f1=(struct _tagged_arr)*(*_tmp5F4->name).f2;{void*
_tmp5F5[1]={& _tmp5F6};Cyc_Tcutil_terr(_tmp5F4->loc,_tag_arr("duplicate enum field name %s",
sizeof(char),29),_tag_arr(_tmp5F5,sizeof(void*),1));}});else{prev_fields=({
struct Cyc_List_List*_tmp5F7=_region_malloc(uprev_rgn,sizeof(*_tmp5F7));_tmp5F7->hd=(*
_tmp5F4->name).f2;_tmp5F7->tl=prev_fields;_tmp5F7;});}if(_tmp5F4->tag == 0)
_tmp5F4->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp5F4->loc);
else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp5F4->tag)))({
struct Cyc_Std_String_pa_struct _tmp5F9;_tmp5F9.tag=0;_tmp5F9.f1=(struct
_tagged_arr)*(*_tmp5F4->name).f2;{void*_tmp5F8[1]={& _tmp5F9};Cyc_Tcutil_terr(loc,
_tag_arr("enum field %s: expression is not constant",sizeof(char),42),_tag_arr(
_tmp5F8,sizeof(void*),1));}});}{unsigned int t1=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5F4->tag));tag_count=t1 + 1;(*_tmp5F4->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp5FB;_tmp5FB.tag=1;_tmp5FB.f1=te->ns;_tmp5FB;});
_tmp5FA;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple18*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp5F4->name).f2,({
struct _tuple18*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp5FE;_tmp5FE.tag=4;_tmp5FE.f1=(void*)t;
_tmp5FE.f2=_tmp5F4;_tmp5FE;});_tmp5FD;});_tmp5FC->f2=1;_tmp5FC;}));}}};
_pop_region(uprev_rgn);}goto _LL408;}_LL411: if(_tmp5A9 <= (void*)3?1:*((int*)
_tmp5A9)!= 12)goto _LL413;_tmp5B0=((struct Cyc_Absyn_EnumType_struct*)_tmp5A9)->f1;
_tmp5B1=((struct Cyc_Absyn_EnumType_struct*)_tmp5A9)->f2;_tmp5B2=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp5A9)->f2;_LL412: if(*_tmp5B2 == 0?1:((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp5B2))->fields == 0){struct _handler_cons
_tmp5FF;_push_handler(& _tmp5FF);{int _tmp601=0;if(setjmp(_tmp5FF.handler))_tmp601=
1;if(!_tmp601){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp5B0);*_tmp5B2=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp600=(void*)_exn_thrown;void*_tmp603=_tmp600;_LL43D: if(_tmp603 != Cyc_Dict_Absent)
goto _LL43F;_LL43E: {struct Cyc_Tcenv_Genv*_tmp604=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp605=({struct Cyc_Absyn_Enumdecl*_tmp606=_cycalloc(
sizeof(*_tmp606));_tmp606->sc=(void*)((void*)3);_tmp606->name=_tmp5B0;_tmp606->fields=
0;_tmp606;});Cyc_Tc_tcEnumdecl(te,_tmp604,loc,_tmp605);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp5B0);*_tmp5B2=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL43C;}}_LL43F:;_LL440:(void)_throw(_tmp603);_LL43C:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp5B2);*_tmp5B0=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL408;}_LL413: if(_tmp5A9 <= (void*)3?1:*((
int*)_tmp5A9)!= 2)goto _LL415;_tmp5B3=((struct Cyc_Absyn_TunionType_struct*)
_tmp5A9)->f1;_tmp5B4=(void*)_tmp5B3.tunion_info;_tmp5B5=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp5A9)->f1).tunion_info;_tmp5B6=_tmp5B3.targs;_tmp5B7=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp5A9)->f1).targs;_tmp5B8=(void*)_tmp5B3.rgn;
_LL414: {struct Cyc_List_List*_tmp607=*_tmp5B7;{void*_tmp608=*_tmp5B5;struct Cyc_Absyn_UnknownTunionInfo
_tmp609;struct _tuple1*_tmp60A;int _tmp60B;struct Cyc_Absyn_Tuniondecl**_tmp60C;
struct Cyc_Absyn_Tuniondecl*_tmp60D;_LL442: if(*((int*)_tmp608)!= 0)goto _LL444;
_tmp609=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp608)->f1;_tmp60A=_tmp609.name;
_tmp60B=_tmp609.is_xtunion;_LL443: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp60E;_push_handler(& _tmp60E);{int _tmp610=0;if(setjmp(_tmp60E.handler))
_tmp610=1;if(!_tmp610){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp60A);;
_pop_handler();}else{void*_tmp60F=(void*)_exn_thrown;void*_tmp612=_tmp60F;_LL447:
if(_tmp612 != Cyc_Dict_Absent)goto _LL449;_LL448: {struct Cyc_Tcenv_Genv*_tmp613=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp614=({struct Cyc_Absyn_Tuniondecl*
_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->sc=(void*)((void*)3);_tmp618->name=
_tmp60A;_tmp618->tvs=0;_tmp618->fields=0;_tmp618->is_xtunion=_tmp60B;_tmp618;});
Cyc_Tc_tcTuniondecl(te,_tmp613,loc,_tmp614);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp60A);if(_tmp607 != 0){({struct Cyc_Std_String_pa_struct _tmp617;_tmp617.tag=
0;_tmp617.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp60A);{struct Cyc_Std_String_pa_struct
_tmp616;_tmp616.tag=0;_tmp616.f1=(struct _tagged_arr)(_tmp60B?_tag_arr("xtunion",
sizeof(char),8): _tag_arr("tunion",sizeof(char),7));{void*_tmp615[2]={& _tmp616,&
_tmp617};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized %s %s before using",
sizeof(char),48),_tag_arr(_tmp615,sizeof(void*),2));}}});return cvtenv;}goto
_LL446;}_LL449:;_LL44A:(void)_throw(_tmp612);_LL446:;}}}if((*tudp)->is_xtunion != 
_tmp60B)({struct Cyc_Std_String_pa_struct _tmp61A;_tmp61A.tag=0;_tmp61A.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp60A);{void*_tmp619[1]={& _tmp61A};Cyc_Tcutil_terr(
loc,_tag_arr("[x]tunion is different from type declaration %s",sizeof(char),48),
_tag_arr(_tmp619,sizeof(void*),1));}});*_tmp5B5=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp61C;_tmp61C.tag=1;_tmp61C.f1=tudp;_tmp61C;});_tmp61B;});_tmp60D=*tudp;goto
_LL445;}_LL444: if(*((int*)_tmp608)!= 1)goto _LL441;_tmp60C=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp608)->f1;_tmp60D=*_tmp60C;_LL445: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5B8);{struct Cyc_List_List*tvs=_tmp60D->tvs;for(0;_tmp607 != 0?
tvs != 0: 0;(_tmp607=_tmp607->tl,tvs=tvs->tl)){void*t1=(void*)_tmp607->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp607 != 0)({struct Cyc_Std_String_pa_struct _tmp61E;
_tmp61E.tag=0;_tmp61E.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp60D->name);{
void*_tmp61D[1]={& _tmp61E};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s",
sizeof(char),38),_tag_arr(_tmp61D,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp61F=
_cycalloc(sizeof(*_tmp61F));_tmp61F->hd=(void*)e;_tmp61F->tl=hidden_ts;_tmp61F;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*_tmp5B7=Cyc_List_imp_append(*
_tmp5B7,Cyc_List_imp_rev(hidden_ts));}goto _LL441;}_LL441:;}goto _LL408;}_LL415:
if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 3)goto _LL417;_tmp5B9=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5A9)->f1;_tmp5BA=(void*)_tmp5B9.field_info;_tmp5BB=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5A9)->f1).field_info;_tmp5BC=_tmp5B9.targs;_LL416:{void*_tmp620=*_tmp5BB;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp621;struct _tuple1*_tmp622;struct
_tuple1*_tmp623;int _tmp624;struct Cyc_Absyn_Tuniondecl*_tmp625;struct Cyc_Absyn_Tunionfield*
_tmp626;_LL44C: if(*((int*)_tmp620)!= 0)goto _LL44E;_tmp621=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp620)->f1;_tmp622=_tmp621.tunion_name;_tmp623=_tmp621.field_name;_tmp624=
_tmp621.is_xtunion;_LL44D: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp627;_push_handler(& _tmp627);{int _tmp629=0;if(setjmp(
_tmp627.handler))_tmp629=1;if(!_tmp629){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp622);;_pop_handler();}else{void*_tmp628=(void*)_exn_thrown;void*_tmp62B=
_tmp628;_LL451: if(_tmp62B != Cyc_Dict_Absent)goto _LL453;_LL452:({struct Cyc_Std_String_pa_struct
_tmp62D;_tmp62D.tag=0;_tmp62D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp622);{void*_tmp62C[1]={& _tmp62D};Cyc_Tcutil_terr(loc,_tag_arr("unbound type tunion %s",
sizeof(char),23),_tag_arr(_tmp62C,sizeof(void*),1));}});return cvtenv;_LL453:;
_LL454:(void)_throw(_tmp62B);_LL450:;}}}{struct _handler_cons _tmp62E;
_push_handler(& _tmp62E);{int _tmp630=0;if(setjmp(_tmp62E.handler))_tmp630=1;if(!
_tmp630){{void*_tmp631=Cyc_Tcenv_lookup_ordinary(te,loc,_tmp623);struct Cyc_Absyn_Tuniondecl*
_tmp632;struct Cyc_Absyn_Tunionfield*_tmp633;_LL456: if(*((int*)_tmp631)!= 2)goto
_LL458;_tmp632=((struct Cyc_Tcenv_TunionRes_struct*)_tmp631)->f1;_tmp633=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp631)->f2;_LL457: tuf=_tmp633;tud=_tmp632;if(tud->is_xtunion
!= _tmp624)({struct Cyc_Std_String_pa_struct _tmp635;_tmp635.tag=0;_tmp635.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp622);{void*_tmp634[1]={& _tmp635};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(char),48),_tag_arr(_tmp634,sizeof(void*),1));}});goto _LL455;_LL458:;
_LL459:({struct Cyc_Std_String_pa_struct _tmp638;_tmp638.tag=0;_tmp638.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp622);{struct Cyc_Std_String_pa_struct
_tmp637;_tmp637.tag=0;_tmp637.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp623);{void*_tmp636[2]={& _tmp637,& _tmp638};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp636,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv
_tmp639=cvtenv;_npop_handler(0);return _tmp639;}_LL455:;};_pop_handler();}else{
void*_tmp62F=(void*)_exn_thrown;void*_tmp63B=_tmp62F;_LL45B: if(_tmp63B != Cyc_Dict_Absent)
goto _LL45D;_LL45C:({struct Cyc_Std_String_pa_struct _tmp63E;_tmp63E.tag=0;_tmp63E.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp622);{struct Cyc_Std_String_pa_struct
_tmp63D;_tmp63D.tag=0;_tmp63D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp623);{void*_tmp63C[2]={& _tmp63D,& _tmp63E};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp63C,sizeof(void*),2));}}});return cvtenv;_LL45D:;
_LL45E:(void)_throw(_tmp63B);_LL45A:;}}}*_tmp5BB=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp640;_tmp640.tag=1;_tmp640.f1=tud;_tmp640.f2=tuf;_tmp640;});_tmp63F;});
_tmp625=tud;_tmp626=tuf;goto _LL44F;}_LL44E: if(*((int*)_tmp620)!= 1)goto _LL44B;
_tmp625=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp620)->f1;_tmp626=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp620)->f2;_LL44F: {struct Cyc_List_List*tvs=
_tmp625->tvs;for(0;_tmp5BC != 0?tvs != 0: 0;(_tmp5BC=_tmp5BC->tl,tvs=tvs->tl)){void*
t1=(void*)_tmp5BC->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp5BC != 0)({
struct Cyc_Std_String_pa_struct _tmp643;_tmp643.tag=0;_tmp643.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp626->name);{struct Cyc_Std_String_pa_struct
_tmp642;_tmp642.tag=0;_tmp642.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp625->name);{void*_tmp641[2]={& _tmp642,& _tmp643};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s.%s",
sizeof(char),41),_tag_arr(_tmp641,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_Std_String_pa_struct
_tmp646;_tmp646.tag=0;_tmp646.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp626->name);{struct Cyc_Std_String_pa_struct _tmp645;_tmp645.tag=0;_tmp645.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp625->name);{void*_tmp644[2]={&
_tmp645,& _tmp646};Cyc_Tcutil_terr(loc,_tag_arr("too few type arguments for tunion %s.%s",
sizeof(char),40),_tag_arr(_tmp644,sizeof(void*),2));}}});goto _LL44B;}_LL44B:;}
goto _LL408;_LL417: if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 4)goto _LL419;
_tmp5BD=((struct Cyc_Absyn_PointerType_struct*)_tmp5A9)->f1;_tmp5BE=(void*)
_tmp5BD.elt_typ;_tmp5BF=(void*)_tmp5BD.rgn_typ;_tmp5C0=_tmp5BD.nullable;_tmp5C1=
_tmp5BD.tq;_tmp5C2=_tmp5BD.bounds;_LL418: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp5BE);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5BF);{void*_tmp647=(void*)(Cyc_Absyn_compress_conref(_tmp5C2))->v;
void*_tmp648;struct Cyc_Absyn_Exp*_tmp649;_LL460: if(_tmp647 <= (void*)1?1:*((int*)
_tmp647)!= 0)goto _LL462;_tmp648=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp647)->f1;if(_tmp648 <= (void*)1?1:*((int*)_tmp648)!= 0)goto _LL462;_tmp649=((
struct Cyc_Absyn_Upper_b_struct*)_tmp648)->f1;_LL461: if(!Cyc_Tcutil_is_const_exp(
te,_tmp649))({void*_tmp64A[0]={};Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not a constant",
sizeof(char),44),_tag_arr(_tmp64A,sizeof(void*),0));});Cyc_Tcexp_tcExp(te,0,
_tmp649);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp649))({void*_tmp64B[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("pointer bounds expression is not an unsigned int",sizeof(char),49),
_tag_arr(_tmp64B,sizeof(void*),0));});Cyc_Evexp_eval_const_uint_exp(_tmp649);
goto _LL45F;_LL462:;_LL463: goto _LL45F;_LL45F:;}goto _LL408;_LL419: if(_tmp5A9 <= (
void*)3?1:*((int*)_tmp5A9)!= 14)goto _LL41B;_tmp5C3=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp5A9)->f1;_LL41A: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,
_tmp5C3);goto _LL408;_LL41B: if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 5)goto
_LL41D;_LL41C: goto _LL41E;_LL41D: if((int)_tmp5A9 != 1)goto _LL41F;_LL41E: goto _LL420;
_LL41F: if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 6)goto _LL421;_LL420: goto _LL408;
_LL421: if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 7)goto _LL423;_tmp5C4=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp5A9)->f1;_tmp5C5=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5A9)->f2;_tmp5C6=((struct Cyc_Absyn_ArrayType_struct*)_tmp5A9)->f3;_LL422:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,_tmp5C4);if(_tmp5C6
== 0?1: !Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp5C6)))({
void*_tmp64C[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array bounds expression is not constant",
sizeof(char),40),_tag_arr(_tmp64C,sizeof(void*),0));});Cyc_Tcexp_tcExp(te,0,(
struct Cyc_Absyn_Exp*)_check_null(_tmp5C6));if(!Cyc_Tcutil_coerce_uint_typ(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp5C6)))({void*_tmp64D[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("array bounds expression is not an unsigned int",sizeof(char),47),
_tag_arr(_tmp64D,sizeof(void*),0));});Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp5C6));goto _LL408;_LL423: if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)
!= 8)goto _LL425;_tmp5C7=((struct Cyc_Absyn_FnType_struct*)_tmp5A9)->f1;_tmp5C8=
_tmp5C7.tvars;_tmp5C9=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp5A9)->f1).tvars;_tmp5CA=_tmp5C7.effect;_tmp5CB=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp5A9)->f1).effect;_tmp5CC=(void*)_tmp5C7.ret_typ;
_tmp5CD=_tmp5C7.args;_tmp5CE=_tmp5C7.c_varargs;_tmp5CF=_tmp5C7.cyc_varargs;
_tmp5D0=_tmp5C7.rgn_po;_tmp5D1=_tmp5C7.attributes;_LL424: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp5D1 != 0;_tmp5D1=_tmp5D1->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp5D1->hd))({struct Cyc_Std_String_pa_struct
_tmp64F;_tmp64F.tag=0;_tmp64F.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp5D1->hd);{void*_tmp64E[1]={& _tmp64F};Cyc_Tcutil_terr(loc,_tag_arr("bad function type attribute %s",
sizeof(char),31),_tag_arr(_tmp64E,sizeof(void*),1));}});{void*_tmp650=(void*)
_tmp5D1->hd;void*_tmp651;int _tmp652;int _tmp653;_LL465: if((int)_tmp650 != 0)goto
_LL467;_LL466: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL464;_LL467:
if((int)_tmp650 != 1)goto _LL469;_LL468: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}
goto _LL464;_LL469: if((int)_tmp650 != 2)goto _LL46B;_LL46A: if(!seen_fastcall){
seen_fastcall=1;++ num_convs;}goto _LL464;_LL46B: if(_tmp650 <= (void*)16?1:*((int*)
_tmp650)!= 3)goto _LL46D;_tmp651=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp650)->f1;_tmp652=((struct Cyc_Absyn_Format_att_struct*)_tmp650)->f2;_tmp653=((
struct Cyc_Absyn_Format_att_struct*)_tmp650)->f3;_LL46C: if(!seen_format){
seen_format=1;ft=_tmp651;fmt_desc_arg=_tmp652;fmt_arg_start=_tmp653;}else{({void*
_tmp654[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function can't have multiple format attributes",
sizeof(char),47),_tag_arr(_tmp654,sizeof(void*),0));});}goto _LL464;_LL46D:;
_LL46E: goto _LL464;_LL464:;}}if(num_convs > 1)({void*_tmp655[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("function can't have multiple calling conventions",sizeof(char),49),
_tag_arr(_tmp655,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp5C9);{
struct Cyc_List_List*b=*_tmp5C9;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp656=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp657;_LL470: if(*((int*)_tmp656)!= 0)goto
_LL472;_tmp657=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp656)->f1;if((int)
_tmp657 != 1)goto _LL472;_LL471:({struct Cyc_Std_String_pa_struct _tmp659;_tmp659.tag=
0;_tmp659.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name;{void*
_tmp658[1]={& _tmp659};Cyc_Tcutil_terr(loc,_tag_arr("function attempts to abstract Mem type variable %s",
sizeof(char),51),_tag_arr(_tmp658,sizeof(void*),1));}});goto _LL46F;_LL472:;
_LL473: goto _LL46F;_LL46F:;}}}{struct Cyc_Tcutil_CVTEnv _tmp65A=({struct Cyc_Tcutil_CVTEnv
_tmp6C9;_tmp6C9.kind_env=cvtenv.kind_env;_tmp6C9.free_vars=0;_tmp6C9.free_evars=
0;_tmp6C9.generalize_evars=cvtenv.generalize_evars;_tmp6C9.fn_result=1;_tmp6C9;});
_tmp65A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp65A,(void*)1,_tmp5CC);_tmp65A.fn_result=
0;{struct Cyc_List_List*a=_tmp5CD;for(0;a != 0;a=a->tl){_tmp65A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp65A,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp5CF != 0){if(
_tmp5CE)({void*_tmp65B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("both c_vararg and cyc_vararg",sizeof(char),29),
_tag_arr(_tmp65B,sizeof(void*),0));});{struct Cyc_Core_Opt*_tmp65D;struct Cyc_Absyn_Tqual
_tmp65E;void*_tmp65F;int _tmp660;struct Cyc_Absyn_VarargInfo _tmp65C=*_tmp5CF;
_tmp65D=_tmp65C.name;_tmp65E=_tmp65C.tq;_tmp65F=(void*)_tmp65C.type;_tmp660=
_tmp65C.inject;_tmp65A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp65A,(void*)1,
_tmp65F);if(_tmp660){void*_tmp661=Cyc_Tcutil_compress(_tmp65F);struct Cyc_Absyn_TunionInfo
_tmp662;void*_tmp663;void*_tmp664;_LL475: if(_tmp661 <= (void*)3?1:*((int*)_tmp661)
!= 2)goto _LL477;_tmp662=((struct Cyc_Absyn_TunionType_struct*)_tmp661)->f1;
_tmp663=(void*)_tmp662.tunion_info;if(*((int*)_tmp663)!= 1)goto _LL477;_tmp664=(
void*)_tmp662.rgn;_LL476: goto _LL474;_LL477:;_LL478:({void*_tmp665[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("can't inject a non-[x]tunion type",sizeof(char),34),_tag_arr(
_tmp665,sizeof(void*),0));});goto _LL474;_LL474:;}}}if(seen_format){int _tmp666=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5CD);if((((fmt_desc_arg < 0?1:
fmt_desc_arg > _tmp666)?1: fmt_arg_start < 0)?1:(_tmp5CF == 0?fmt_arg_start != 0: 0))?
1:(_tmp5CF != 0?fmt_arg_start != _tmp666 + 1: 0))({void*_tmp667[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bad format descriptor",sizeof(char),22),_tag_arr(_tmp667,sizeof(
void*),0));});else{void*_tmp669;struct _tuple2 _tmp668=*((struct _tuple2*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5CD,fmt_desc_arg - 1);_tmp669=_tmp668.f3;{
void*_tmp66A=Cyc_Tcutil_compress(_tmp669);struct Cyc_Absyn_PtrInfo _tmp66B;void*
_tmp66C;struct Cyc_Absyn_Conref*_tmp66D;_LL47A: if(_tmp66A <= (void*)3?1:*((int*)
_tmp66A)!= 4)goto _LL47C;_tmp66B=((struct Cyc_Absyn_PointerType_struct*)_tmp66A)->f1;
_tmp66C=(void*)_tmp66B.elt_typ;_tmp66D=_tmp66B.bounds;_LL47B:{struct _tuple6
_tmp66F=({struct _tuple6 _tmp66E;_tmp66E.f1=Cyc_Tcutil_compress(_tmp66C);_tmp66E.f2=
Cyc_Absyn_conref_def((void*)0,_tmp66D);_tmp66E;});void*_tmp670;void*_tmp671;void*
_tmp672;void*_tmp673;_LL47F: _tmp670=_tmp66F.f1;if(_tmp670 <= (void*)3?1:*((int*)
_tmp670)!= 5)goto _LL481;_tmp671=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp670)->f1;
if((int)_tmp671 != 1)goto _LL481;_tmp672=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp670)->f2;if((int)_tmp672 != 0)goto _LL481;_tmp673=_tmp66F.f2;if((int)_tmp673 != 
0)goto _LL481;_LL480: goto _LL47E;_LL481:;_LL482:({void*_tmp674[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("format descriptor is not a char ? type",sizeof(char),39),_tag_arr(
_tmp674,sizeof(void*),0));});goto _LL47E;_LL47E:;}goto _LL479;_LL47C:;_LL47D:({
void*_tmp675[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format descriptor is not a char ? type",
sizeof(char),39),_tag_arr(_tmp675,sizeof(void*),0));});goto _LL479;_LL479:;}if(
fmt_arg_start != 0){void*_tmp676=Cyc_Tcutil_compress((void*)((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp5CF))->type);int problem;{void*_tmp677=ft;_LL484: if((int)_tmp677
!= 0)goto _LL486;_LL485:{void*_tmp678=_tmp676;struct Cyc_Absyn_TunionInfo _tmp679;
void*_tmp67A;struct Cyc_Absyn_Tuniondecl**_tmp67B;struct Cyc_Absyn_Tuniondecl*
_tmp67C;_LL489: if(_tmp678 <= (void*)3?1:*((int*)_tmp678)!= 2)goto _LL48B;_tmp679=((
struct Cyc_Absyn_TunionType_struct*)_tmp678)->f1;_tmp67A=(void*)_tmp679.tunion_info;
if(*((int*)_tmp67A)!= 1)goto _LL48B;_tmp67B=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp67A)->f1;_tmp67C=*_tmp67B;_LL48A: problem=Cyc_Absyn_qvar_cmp(_tmp67C->name,
Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL488;_LL48B:;_LL48C: problem=1;goto
_LL488;_LL488:;}goto _LL483;_LL486: if((int)_tmp677 != 1)goto _LL483;_LL487:{void*
_tmp67D=_tmp676;struct Cyc_Absyn_TunionInfo _tmp67E;void*_tmp67F;struct Cyc_Absyn_Tuniondecl**
_tmp680;struct Cyc_Absyn_Tuniondecl*_tmp681;_LL48E: if(_tmp67D <= (void*)3?1:*((int*)
_tmp67D)!= 2)goto _LL490;_tmp67E=((struct Cyc_Absyn_TunionType_struct*)_tmp67D)->f1;
_tmp67F=(void*)_tmp67E.tunion_info;if(*((int*)_tmp67F)!= 1)goto _LL490;_tmp680=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp67F)->f1;_tmp681=*_tmp680;_LL48F: problem=
Cyc_Absyn_qvar_cmp(_tmp681->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL48D;
_LL490:;_LL491: problem=1;goto _LL48D;_LL48D:;}goto _LL483;_LL483:;}if(problem)({
void*_tmp682[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format attribute and vararg types don't match",
sizeof(char),46),_tag_arr(_tmp682,sizeof(void*),0));});}}}{struct Cyc_List_List*
rpo=_tmp5D0;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp684;void*_tmp685;void*
_tmp686;struct _tuple6*_tmp683=(struct _tuple6*)rpo->hd;_tmp684=*_tmp683;_tmp685=
_tmp684.f1;_tmp686=_tmp684.f2;_tmp65A=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp65A,(void*)3,_tmp685);_tmp65A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp65A,(
void*)3,_tmp686);}}if(*_tmp5CB != 0)_tmp65A=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp65A,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp5CB))->v);else{
if(cvtenv.generalize_evars);{struct Cyc_List_List*effect=0;{struct Cyc_List_List*
tvs=_tmp65A.free_vars;for(0;tvs != 0;tvs=tvs->tl){void*_tmp687=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmp688;struct
Cyc_Core_Opt**_tmp689;void*_tmp68A;void*_tmp68B;struct Cyc_Core_Opt*_tmp68C;
struct Cyc_Core_Opt**_tmp68D;void*_tmp68E;void*_tmp68F;struct Cyc_Core_Opt*_tmp690;
struct Cyc_Core_Opt**_tmp691;_LL493: if(*((int*)_tmp687)!= 2)goto _LL495;_tmp688=((
struct Cyc_Absyn_Less_kb_struct*)_tmp687)->f1;_tmp689=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp687)->f1;_tmp68A=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp687)->f2;if((int)_tmp68A != 3)goto _LL495;_LL494:*_tmp689=({struct Cyc_Core_Opt*
_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp694;_tmp694.tag=0;_tmp694.f1=(void*)((void*)3);_tmp694;});_tmp693;}));
_tmp692;});goto _LL496;_LL495: if(*((int*)_tmp687)!= 0)goto _LL497;_tmp68B=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp687)->f1;if((int)_tmp68B != 3)goto _LL497;_LL496:
effect=({struct Cyc_List_List*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp696=_cycalloc(sizeof(*
_tmp696));_tmp696[0]=({struct Cyc_Absyn_AccessEff_struct _tmp697;_tmp697.tag=17;
_tmp697.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp698=_cycalloc(
sizeof(*_tmp698));_tmp698[0]=({struct Cyc_Absyn_VarType_struct _tmp699;_tmp699.tag=
1;_tmp699.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp699;});_tmp698;}));_tmp697;});
_tmp696;}));_tmp695->tl=effect;_tmp695;});goto _LL492;_LL497: if(*((int*)_tmp687)
!= 2)goto _LL499;_tmp68C=((struct Cyc_Absyn_Less_kb_struct*)_tmp687)->f1;_tmp68D=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp687)->f1;_tmp68E=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp687)->f2;if((int)_tmp68E != 4)goto
_LL499;_LL498:*_tmp68D=({struct Cyc_Core_Opt*_tmp69A=_cycalloc(sizeof(*_tmp69A));
_tmp69A->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp69B=_cycalloc(
sizeof(*_tmp69B));_tmp69B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp69C;_tmp69C.tag=0;
_tmp69C.f1=(void*)((void*)4);_tmp69C;});_tmp69B;}));_tmp69A;});goto _LL49A;_LL499:
if(*((int*)_tmp687)!= 0)goto _LL49B;_tmp68F=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp687)->f1;if((int)_tmp68F != 4)goto _LL49B;_LL49A: effect=({struct Cyc_List_List*
_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E[0]=({struct Cyc_Absyn_VarType_struct
_tmp69F;_tmp69F.tag=1;_tmp69F.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp69F;});
_tmp69E;}));_tmp69D->tl=effect;_tmp69D;});goto _LL492;_LL49B: if(*((int*)_tmp687)
!= 1)goto _LL49D;_tmp690=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp687)->f1;
_tmp691=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp687)->f1;
_LL49C:*_tmp691=({struct Cyc_Core_Opt*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));
_tmp6A1[0]=({struct Cyc_Absyn_Less_kb_struct _tmp6A2;_tmp6A2.tag=2;_tmp6A2.f1=0;
_tmp6A2.f2=(void*)((void*)0);_tmp6A2;});_tmp6A1;}));_tmp6A0;});goto _LL49E;_LL49D:;
_LL49E: effect=({struct Cyc_List_List*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));
_tmp6A4[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6A5;_tmp6A5.tag=19;_tmp6A5.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));
_tmp6A6[0]=({struct Cyc_Absyn_VarType_struct _tmp6A7;_tmp6A7.tag=1;_tmp6A7.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6A7;});_tmp6A6;}));_tmp6A5;});_tmp6A4;}));
_tmp6A3->tl=effect;_tmp6A3;});goto _LL492;_LL492:;}}effect=Cyc_List_imp_rev(
effect);{struct Cyc_List_List*ts=_tmp65A.free_evars;for(0;ts != 0;ts=ts->tl){void*
_tmp6A8=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL4A0: if((int)_tmp6A8 != 3)goto _LL4A2;
_LL4A1: effect=({struct Cyc_List_List*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6AA=_cycalloc(sizeof(*
_tmp6AA));_tmp6AA[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6AB;_tmp6AB.tag=17;
_tmp6AB.f1=(void*)((void*)ts->hd);_tmp6AB;});_tmp6AA;}));_tmp6A9->tl=effect;
_tmp6A9;});goto _LL49F;_LL4A2: if((int)_tmp6A8 != 4)goto _LL4A4;_LL4A3: effect=({
struct Cyc_List_List*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC->hd=(void*)((
void*)ts->hd);_tmp6AC->tl=effect;_tmp6AC;});goto _LL49F;_LL4A4:;_LL4A5: effect=({
struct Cyc_List_List*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->hd=(void*)((
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));
_tmp6AE[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6AF;_tmp6AF.tag=19;_tmp6AF.f1=(
void*)((void*)ts->hd);_tmp6AF;});_tmp6AE;}));_tmp6AD->tl=effect;_tmp6AD;});goto
_LL49F;_LL49F:;}}*_tmp5CB=({struct Cyc_Core_Opt*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));
_tmp6B0->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6B1=_cycalloc(
sizeof(*_tmp6B1));_tmp6B1[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6B2;_tmp6B2.tag=
18;_tmp6B2.f1=Cyc_List_imp_rev(effect);_tmp6B2;});_tmp6B1;}));_tmp6B0;});}}if(*
_tmp5C9 != 0){struct Cyc_List_List*bs=*_tmp5C9;for(0;bs != 0;bs=bs->tl){void*
_tmp6B3=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp6B4;struct Cyc_Core_Opt**_tmp6B5;struct Cyc_Core_Opt*
_tmp6B6;struct Cyc_Core_Opt**_tmp6B7;void*_tmp6B8;struct Cyc_Core_Opt*_tmp6B9;
struct Cyc_Core_Opt**_tmp6BA;void*_tmp6BB;struct Cyc_Core_Opt*_tmp6BC;struct Cyc_Core_Opt**
_tmp6BD;void*_tmp6BE;void*_tmp6BF;_LL4A7: if(*((int*)_tmp6B3)!= 1)goto _LL4A9;
_tmp6B4=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6B3)->f1;_tmp6B5=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp6B3)->f1;_LL4A8:({struct Cyc_Std_String_pa_struct
_tmp6C1;_tmp6C1.tag=0;_tmp6C1.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name;{
void*_tmp6C0[1]={& _tmp6C1};Cyc_Tcutil_warn(loc,_tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof(char),47),_tag_arr(_tmp6C0,sizeof(void*),1));}});_tmp6B7=_tmp6B5;goto
_LL4AA;_LL4A9: if(*((int*)_tmp6B3)!= 2)goto _LL4AB;_tmp6B6=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6B3)->f1;_tmp6B7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6B3)->f1;_tmp6B8=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6B3)->f2;if((
int)_tmp6B8 != 1)goto _LL4AB;_LL4AA: _tmp6BA=_tmp6B7;goto _LL4AC;_LL4AB: if(*((int*)
_tmp6B3)!= 2)goto _LL4AD;_tmp6B9=((struct Cyc_Absyn_Less_kb_struct*)_tmp6B3)->f1;
_tmp6BA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6B3)->f1;
_tmp6BB=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6B3)->f2;if((int)_tmp6BB != 
0)goto _LL4AD;_LL4AC:*_tmp6BA=({struct Cyc_Core_Opt*_tmp6C2=_cycalloc(sizeof(*
_tmp6C2));_tmp6C2->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6C3=
_cycalloc(sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6C4;
_tmp6C4.tag=0;_tmp6C4.f1=(void*)((void*)2);_tmp6C4;});_tmp6C3;}));_tmp6C2;});
goto _LL4A6;_LL4AD: if(*((int*)_tmp6B3)!= 2)goto _LL4AF;_tmp6BC=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6B3)->f1;_tmp6BD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6B3)->f1;_tmp6BE=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6B3)->f2;
_LL4AE:*_tmp6BD=({struct Cyc_Core_Opt*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));
_tmp6C6[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6C7;_tmp6C7.tag=0;_tmp6C7.f1=(void*)
_tmp6BE;_tmp6C7;});_tmp6C6;}));_tmp6C5;});goto _LL4A6;_LL4AF: if(*((int*)_tmp6B3)
!= 0)goto _LL4B1;_tmp6BF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp6B3)->f1;if((
int)_tmp6BF != 1)goto _LL4B1;_LL4B0:({void*_tmp6C8[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("function's can't abstract M types",sizeof(char),34),_tag_arr(_tmp6C8,
sizeof(void*),0));});goto _LL4A6;_LL4B1:;_LL4B2: goto _LL4A6;_LL4A6:;}}cvtenv.kind_env=
Cyc_Tcutil_remove_bound_tvars(_tmp65A.kind_env,*_tmp5C9);_tmp65A.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp65A.free_vars,*_tmp5C9);{struct Cyc_List_List*tvs=_tmp65A.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp65A.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL408;}}_LL425: if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 
9)goto _LL427;_tmp5D2=((struct Cyc_Absyn_TupleType_struct*)_tmp5A9)->f1;_LL426:
for(0;_tmp5D2 != 0;_tmp5D2=_tmp5D2->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,(void*)1,(*((struct _tuple4*)_tmp5D2->hd)).f2);}goto _LL408;_LL427: if(
_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 11)goto _LL429;_tmp5D3=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5A9)->f1;_tmp5D4=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5A9)->f2;_LL428:{
struct _RegionHandle _tmp6CA=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp6CA;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp5D4 != 0;_tmp5D4=_tmp5D4->tl){struct Cyc_Absyn_Aggrfield _tmp6CC;struct
_tagged_arr*_tmp6CD;struct Cyc_Absyn_Tqual _tmp6CE;void*_tmp6CF;struct Cyc_Absyn_Exp*
_tmp6D0;struct Cyc_List_List*_tmp6D1;struct Cyc_Absyn_Aggrfield*_tmp6CB=(struct Cyc_Absyn_Aggrfield*)
_tmp5D4->hd;_tmp6CC=*_tmp6CB;_tmp6CD=_tmp6CC.name;_tmp6CE=_tmp6CC.tq;_tmp6CF=(
void*)_tmp6CC.type;_tmp6D0=_tmp6CC.width;_tmp6D1=_tmp6CC.attributes;if(((int(*)(
int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,
struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,prev_fields,_tmp6CD))({
struct Cyc_Std_String_pa_struct _tmp6D3;_tmp6D3.tag=0;_tmp6D3.f1=(struct
_tagged_arr)*_tmp6CD;{void*_tmp6D2[1]={& _tmp6D3};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s",
sizeof(char),19),_tag_arr(_tmp6D2,sizeof(void*),1));}});if(Cyc_Std_strcmp(*
_tmp6CD,_tag_arr("",sizeof(char),1))!= 0)prev_fields=({struct Cyc_List_List*
_tmp6D4=_region_malloc(aprev_rgn,sizeof(*_tmp6D4));_tmp6D4->hd=_tmp6CD;_tmp6D4->tl=
prev_fields;_tmp6D4;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp6CF);if(_tmp5D3 == (void*)1?!Cyc_Tcutil_bits_only(_tmp6CF): 0)({struct Cyc_Std_String_pa_struct
_tmp6D6;_tmp6D6.tag=0;_tmp6D6.f1=(struct _tagged_arr)*_tmp6CD;{void*_tmp6D5[1]={&
_tmp6D6};Cyc_Tcutil_terr(loc,_tag_arr("union member %s has a non-integral type",
sizeof(char),40),_tag_arr(_tmp6D5,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(
loc,te,_tmp6CF,_tmp6D0,_tmp6CD);Cyc_Tcutil_check_field_atts(loc,_tmp6CD,_tmp6D1);}};
_pop_region(aprev_rgn);}goto _LL408;_LL429: if(_tmp5A9 <= (void*)3?1:*((int*)
_tmp5A9)!= 10)goto _LL42B;_tmp5D5=((struct Cyc_Absyn_AggrType_struct*)_tmp5A9)->f1;
_tmp5D6=(void*)_tmp5D5.aggr_info;_tmp5D7=(void**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp5A9)->f1).aggr_info;_tmp5D8=_tmp5D5.targs;_tmp5D9=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp5A9)->f1).targs;_LL42A:{void*_tmp6D7=*
_tmp5D7;void*_tmp6D8;struct _tuple1*_tmp6D9;struct Cyc_Absyn_Aggrdecl**_tmp6DA;
struct Cyc_Absyn_Aggrdecl*_tmp6DB;_LL4B4: if(*((int*)_tmp6D7)!= 0)goto _LL4B6;
_tmp6D8=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp6D7)->f1;_tmp6D9=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp6D7)->f2;_LL4B5: {struct Cyc_Absyn_Aggrdecl**
adp;{struct _handler_cons _tmp6DC;_push_handler(& _tmp6DC);{int _tmp6DE=0;if(setjmp(
_tmp6DC.handler))_tmp6DE=1;if(!_tmp6DE){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,
_tmp6D9);*_tmp5D7=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp6DF=_cycalloc(
sizeof(*_tmp6DF));_tmp6DF[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp6E0;_tmp6E0.tag=
1;_tmp6E0.f1=adp;_tmp6E0;});_tmp6DF;});;_pop_handler();}else{void*_tmp6DD=(void*)
_exn_thrown;void*_tmp6E2=_tmp6DD;_LL4B9: if(_tmp6E2 != Cyc_Dict_Absent)goto _LL4BB;
_LL4BA: {struct Cyc_Tcenv_Genv*_tmp6E3=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*
_tmp6E4=({struct Cyc_Absyn_Aggrdecl*_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9->kind=(
void*)_tmp6D8;_tmp6E9->sc=(void*)((void*)3);_tmp6E9->name=_tmp6D9;_tmp6E9->tvs=0;
_tmp6E9->exist_vars=0;_tmp6E9->fields=0;_tmp6E9->attributes=0;_tmp6E9;});Cyc_Tc_tcAggrdecl(
te,_tmp6E3,loc,_tmp6E4);adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp6D9);*_tmp5D7=(
void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));
_tmp6E5[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp6E6;_tmp6E6.tag=1;_tmp6E6.f1=
adp;_tmp6E6;});_tmp6E5;});if(*_tmp5D9 != 0){({struct Cyc_Std_String_pa_struct
_tmp6E8;_tmp6E8.tag=0;_tmp6E8.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6D9);{void*_tmp6E7[1]={& _tmp6E8};Cyc_Tcutil_terr(loc,_tag_arr("declare parameterized type %s before using",
sizeof(char),43),_tag_arr(_tmp6E7,sizeof(void*),1));}});return cvtenv;}goto _LL4B8;}
_LL4BB:;_LL4BC:(void)_throw(_tmp6E2);_LL4B8:;}}}_tmp6DB=*adp;goto _LL4B7;}_LL4B6:
if(*((int*)_tmp6D7)!= 1)goto _LL4B3;_tmp6DA=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp6D7)->f1;_tmp6DB=*_tmp6DA;_LL4B7: {struct Cyc_List_List*tvs=_tmp6DB->tvs;
struct Cyc_List_List*ts=*_tmp5D9;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_Std_String_pa_struct
_tmp6EB;_tmp6EB.tag=0;_tmp6EB.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6DB->name);{void*_tmp6EA[1]={& _tmp6EB};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for type %s",
sizeof(char),32),_tag_arr(_tmp6EA,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp6EC=
_cycalloc(sizeof(*_tmp6EC));_tmp6EC->hd=(void*)e;_tmp6EC->tl=hidden_ts;_tmp6EC;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*_tmp5D9=Cyc_List_imp_append(*
_tmp5D9,Cyc_List_imp_rev(hidden_ts));}}_LL4B3:;}goto _LL408;_LL42B: if(_tmp5A9 <= (
void*)3?1:*((int*)_tmp5A9)!= 16)goto _LL42D;_tmp5DA=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5A9)->f1;_tmp5DB=((struct Cyc_Absyn_TypedefType_struct*)_tmp5A9)->f2;_tmp5DC=(
struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp5A9)->f2;
_tmp5DD=((struct Cyc_Absyn_TypedefType_struct*)_tmp5A9)->f3;_tmp5DE=(struct Cyc_Absyn_Typedefdecl**)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp5A9)->f3;_tmp5DF=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5A9)->f4;_tmp5E0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp5A9)->f4;_LL42C: {struct Cyc_List_List*targs=*_tmp5DC;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmp6ED;_push_handler(& _tmp6ED);{int _tmp6EF=0;if(setjmp(
_tmp6ED.handler))_tmp6EF=1;if(!_tmp6EF){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmp5DA);;_pop_handler();}else{void*_tmp6EE=(void*)_exn_thrown;void*_tmp6F1=
_tmp6EE;_LL4BE: if(_tmp6F1 != Cyc_Dict_Absent)goto _LL4C0;_LL4BF:({struct Cyc_Std_String_pa_struct
_tmp6F3;_tmp6F3.tag=0;_tmp6F3.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp5DA);{void*_tmp6F2[1]={& _tmp6F3};Cyc_Tcutil_terr(loc,_tag_arr("unbound typedef name %s",
sizeof(char),24),_tag_arr(_tmp6F2,sizeof(void*),1));}});return cvtenv;_LL4C0:;
_LL4C1:(void)_throw(_tmp6F1);_LL4BD:;}}}*_tmp5DE=(struct Cyc_Absyn_Typedefdecl*)
td;_tmp5DA[0]=(td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0?
tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4->hd=({
struct _tuple7*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp6F5->f2=(void*)ts->hd;_tmp6F5;});_tmp6F4->tl=inst;_tmp6F4;});}if(ts
!= 0)({struct Cyc_Std_String_pa_struct _tmp6F7;_tmp6F7.tag=0;_tmp6F7.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp5DA);{void*_tmp6F6[1]={& _tmp6F7};Cyc_Tcutil_terr(
loc,_tag_arr("too many parameters for typedef %s",sizeof(char),35),_tag_arr(
_tmp6F6,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp6F8=_cycalloc(
sizeof(*_tmp6F8));_tmp6F8->hd=(void*)e;_tmp6F8->tl=hidden_ts;_tmp6F8;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9->hd=({struct _tuple7*_tmp6FA=
_cycalloc(sizeof(*_tmp6FA));_tmp6FA->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6FA->f2=
e;_tmp6FA;});_tmp6F9->tl=inst;_tmp6F9;});}*_tmp5DC=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp5E0=({struct Cyc_Core_Opt*
_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB->v=(void*)new_typ;_tmp6FB;});}goto
_LL408;}}_LL42D: if((int)_tmp5A9 != 2)goto _LL42F;_LL42E: goto _LL408;_LL42F: if(
_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 15)goto _LL431;_tmp5E1=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5A9)->f1;_LL430: _tmp5E2=_tmp5E1;goto _LL432;_LL431: if(_tmp5A9 <= (void*)3?1:*((
int*)_tmp5A9)!= 17)goto _LL433;_tmp5E2=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp5A9)->f1;_LL432: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp5E2);goto _LL408;_LL433: if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 19)goto
_LL435;_tmp5E3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp5A9)->f1;_LL434:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp5E3);goto _LL408;
_LL435: if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 18)goto _LL408;_tmp5E4=((struct
Cyc_Absyn_JoinEff_struct*)_tmp5A9)->f1;_LL436: for(0;_tmp5E4 != 0;_tmp5E4=_tmp5E4->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)_tmp5E4->hd);}
goto _LL408;_LL408:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))({
struct Cyc_Std_String_pa_struct _tmp6FF;_tmp6FF.tag=0;_tmp6FF.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(expected_kind);{struct Cyc_Std_String_pa_struct
_tmp6FE;_tmp6FE.tag=0;_tmp6FE.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t));{struct Cyc_Std_String_pa_struct _tmp6FD;_tmp6FD.tag=0;_tmp6FD.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp6FC[3]={& _tmp6FD,& _tmp6FE,&
_tmp6FF};Cyc_Tcutil_terr(loc,_tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof(char),51),_tag_arr(_tmp6FC,sizeof(void*),3));}}}});return cvtenv;}static
struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp700=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp709;_tmp709.kind_env=kind_env;_tmp709.free_vars=
0;_tmp709.free_evars=0;_tmp709.generalize_evars=generalize_evars;_tmp709.fn_result=
0;_tmp709;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp700.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp700.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp700.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp701=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp702;struct Cyc_Core_Opt**_tmp703;_LL4C3: if(_tmp701 <= (void*)3?1:*((int*)
_tmp701)!= 0)goto _LL4C5;_tmp702=((struct Cyc_Absyn_Evar_struct*)_tmp701)->f4;
_tmp703=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp701)->f4;
_LL4C4: if(*_tmp703 == 0)*_tmp703=({struct Cyc_Core_Opt*_tmp704=_cycalloc(sizeof(*
_tmp704));_tmp704->v=kind_env;_tmp704;});else{struct Cyc_List_List*_tmp705=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp703))->v;struct Cyc_List_List*
_tmp706=0;for(0;_tmp705 != 0;_tmp705=_tmp705->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp705->hd))_tmp706=({struct Cyc_List_List*_tmp707=_cycalloc(sizeof(*_tmp707));
_tmp707->hd=(struct Cyc_Absyn_Tvar*)_tmp705->hd;_tmp707->tl=_tmp706;_tmp707;});}*
_tmp703=({struct Cyc_Core_Opt*_tmp708=_cycalloc(sizeof(*_tmp708));_tmp708->v=
_tmp706;_tmp708;});}goto _LL4C2;_LL4C5:;_LL4C6: goto _LL4C2;_LL4C2:;}}return _tmp700;}
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_Tcutil_CVTEnv _tmp70A=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp70B=_tmp70A.free_vars;struct Cyc_List_List*
_tmp70C=_tmp70A.free_evars;{struct Cyc_List_List*x=_tmp70B;for(0;x != 0;x=x->tl){
void*_tmp70D=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp70E;struct Cyc_Core_Opt**_tmp70F;struct Cyc_Core_Opt*
_tmp710;struct Cyc_Core_Opt**_tmp711;void*_tmp712;struct Cyc_Core_Opt*_tmp713;
struct Cyc_Core_Opt**_tmp714;void*_tmp715;struct Cyc_Core_Opt*_tmp716;struct Cyc_Core_Opt**
_tmp717;void*_tmp718;void*_tmp719;_LL4C8: if(*((int*)_tmp70D)!= 1)goto _LL4CA;
_tmp70E=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp70D)->f1;_tmp70F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp70D)->f1;_LL4C9: _tmp711=_tmp70F;goto
_LL4CB;_LL4CA: if(*((int*)_tmp70D)!= 2)goto _LL4CC;_tmp710=((struct Cyc_Absyn_Less_kb_struct*)
_tmp70D)->f1;_tmp711=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp70D)->f1;_tmp712=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp70D)->f2;if((
int)_tmp712 != 1)goto _LL4CC;_LL4CB: _tmp714=_tmp711;goto _LL4CD;_LL4CC: if(*((int*)
_tmp70D)!= 2)goto _LL4CE;_tmp713=((struct Cyc_Absyn_Less_kb_struct*)_tmp70D)->f1;
_tmp714=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp70D)->f1;
_tmp715=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp70D)->f2;if((int)_tmp715 != 
0)goto _LL4CE;_LL4CD:*_tmp714=({struct Cyc_Core_Opt*_tmp71A=_cycalloc(sizeof(*
_tmp71A));_tmp71A->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp71B=
_cycalloc(sizeof(*_tmp71B));_tmp71B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp71C;
_tmp71C.tag=0;_tmp71C.f1=(void*)((void*)2);_tmp71C;});_tmp71B;}));_tmp71A;});
goto _LL4C7;_LL4CE: if(*((int*)_tmp70D)!= 2)goto _LL4D0;_tmp716=((struct Cyc_Absyn_Less_kb_struct*)
_tmp70D)->f1;_tmp717=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp70D)->f1;_tmp718=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp70D)->f2;
_LL4CF:*_tmp717=({struct Cyc_Core_Opt*_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp71E=_cycalloc(sizeof(*_tmp71E));
_tmp71E[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp71F;_tmp71F.tag=0;_tmp71F.f1=(void*)
_tmp718;_tmp71F;});_tmp71E;}));_tmp71D;});goto _LL4C7;_LL4D0: if(*((int*)_tmp70D)
!= 0)goto _LL4D2;_tmp719=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp70D)->f1;if((
int)_tmp719 != 1)goto _LL4D2;_LL4D1:({struct Cyc_Std_String_pa_struct _tmp721;
_tmp721.tag=0;_tmp721.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd);{void*_tmp720[1]={& _tmp721};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s cannot have kind M",
sizeof(char),36),_tag_arr(_tmp720,sizeof(void*),1));}});goto _LL4C7;_LL4D2:;
_LL4D3: goto _LL4C7;_LL4C7:;}}if(_tmp70B != 0?1: _tmp70C != 0){{void*_tmp722=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp723;struct Cyc_List_List*_tmp724;struct Cyc_List_List**
_tmp725;struct Cyc_Core_Opt*_tmp726;void*_tmp727;struct Cyc_List_List*_tmp728;int
_tmp729;struct Cyc_Absyn_VarargInfo*_tmp72A;struct Cyc_List_List*_tmp72B;struct Cyc_List_List*
_tmp72C;_LL4D5: if(_tmp722 <= (void*)3?1:*((int*)_tmp722)!= 8)goto _LL4D7;_tmp723=((
struct Cyc_Absyn_FnType_struct*)_tmp722)->f1;_tmp724=_tmp723.tvars;_tmp725=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp722)->f1).tvars;
_tmp726=_tmp723.effect;_tmp727=(void*)_tmp723.ret_typ;_tmp728=_tmp723.args;
_tmp729=_tmp723.c_varargs;_tmp72A=_tmp723.cyc_varargs;_tmp72B=_tmp723.rgn_po;
_tmp72C=_tmp723.attributes;_LL4D6: if(*_tmp725 == 0){*_tmp725=_tmp70B;_tmp70B=0;}
goto _LL4D4;_LL4D7:;_LL4D8: goto _LL4D4;_LL4D4:;}if(_tmp70B != 0)({struct Cyc_Std_String_pa_struct
_tmp72E;_tmp72E.tag=0;_tmp72E.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)
_tmp70B->hd)->name;{void*_tmp72D[1]={& _tmp72E};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s",
sizeof(char),25),_tag_arr(_tmp72D,sizeof(void*),1));}});if(_tmp70C != 0)for(0;
_tmp70C != 0;_tmp70C=_tmp70C->tl){void*e=(void*)_tmp70C->hd;void*_tmp72F=Cyc_Tcutil_typ_kind(
e);_LL4DA: if((int)_tmp72F != 3)goto _LL4DC;_LL4DB: if(!Cyc_Tcutil_unify(e,(void*)2))({
void*_tmp730[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with heap!",sizeof(char),28),_tag_arr(_tmp730,sizeof(
void*),0));});goto _LL4D9;_LL4DC: if((int)_tmp72F != 4)goto _LL4DE;_LL4DD: if(!Cyc_Tcutil_unify(
e,Cyc_Absyn_empty_effect))({void*_tmp731[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(char),26),_tag_arr(_tmp731,sizeof(void*),0));});goto _LL4D9;_LL4DE:;_LL4DF:({
struct Cyc_Std_String_pa_struct _tmp734;_tmp734.tag=0;_tmp734.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp733;
_tmp733.tag=0;_tmp733.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp732[2]={& _tmp733,& _tmp734};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp732,sizeof(void*),2));}}});goto _LL4D9;_LL4D9:;}}}
void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(
loc,te,t);{void*_tmp735=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp736;
struct Cyc_List_List*_tmp737;struct Cyc_Core_Opt*_tmp738;void*_tmp739;struct Cyc_List_List*
_tmp73A;int _tmp73B;struct Cyc_Absyn_VarargInfo*_tmp73C;struct Cyc_List_List*
_tmp73D;struct Cyc_List_List*_tmp73E;_LL4E1: if(_tmp735 <= (void*)3?1:*((int*)
_tmp735)!= 8)goto _LL4E3;_tmp736=((struct Cyc_Absyn_FnType_struct*)_tmp735)->f1;
_tmp737=_tmp736.tvars;_tmp738=_tmp736.effect;_tmp739=(void*)_tmp736.ret_typ;
_tmp73A=_tmp736.args;_tmp73B=_tmp736.c_varargs;_tmp73C=_tmp736.cyc_varargs;
_tmp73D=_tmp736.rgn_po;_tmp73E=_tmp736.attributes;_LL4E2: fd->tvs=_tmp737;fd->effect=
_tmp738;goto _LL4E0;_LL4E3:;_LL4E4:({void*_tmp73F[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_fndecl_valid_type: not a FnType",
sizeof(char),38),_tag_arr(_tmp73F,sizeof(void*),0));});_LL4E0:;}{struct
_RegionHandle _tmp740=_new_region("r");struct _RegionHandle*r=& _tmp740;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct _tuple14*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct _tuple14*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,_tag_arr("function declaration has repeated parameter",sizeof(char),
44));;_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp741=_cycalloc(
sizeof(*_tmp741));_tmp741->v=(void*)t;_tmp741;});}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp742=Cyc_Tcutil_check_valid_type(loc,te,bound_tvars,expected_kind,0,t);struct
Cyc_List_List*_tmp743=Cyc_Tcutil_remove_bound_tvars(_tmp742.free_vars,
bound_tvars);struct Cyc_List_List*_tmp744=_tmp742.free_evars;{struct Cyc_List_List*
fs=_tmp743;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp745=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;({struct Cyc_Std_String_pa_struct _tmp748;_tmp748.tag=0;_tmp748.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp747;_tmp747.tag=0;_tmp747.f1=(struct _tagged_arr)*_tmp745;{void*_tmp746[2]={&
_tmp747,& _tmp748};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s in type %s",
sizeof(char),36),_tag_arr(_tmp746,sizeof(void*),2));}}});}}if(!allow_evars?
_tmp744 != 0: 0)for(0;_tmp744 != 0;_tmp744=_tmp744->tl){void*e=(void*)_tmp744->hd;
void*_tmp749=Cyc_Tcutil_typ_kind(e);_LL4E6: if((int)_tmp749 != 3)goto _LL4E8;_LL4E7:
if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp74A[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(char),28),_tag_arr(_tmp74A,sizeof(void*),0));});goto _LL4E5;_LL4E8: if((int)
_tmp749 != 4)goto _LL4EA;_LL4E9: if(!Cyc_Tcutil_unify(e,(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp74B=_cycalloc(sizeof(*_tmp74B));_tmp74B[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp74C;_tmp74C.tag=18;_tmp74C.f1=0;_tmp74C;});_tmp74B;})))({void*_tmp74D[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(char),26),_tag_arr(_tmp74D,sizeof(void*),0));});goto _LL4E5;_LL4EA:;_LL4EB:({
struct Cyc_Std_String_pa_struct _tmp750;_tmp750.tag=0;_tmp750.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp74F;
_tmp74F.tag=0;_tmp74F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp74E[2]={& _tmp74F,& _tmp750};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp74E,sizeof(void*),2));}}});goto _LL4E5;_LL4E5:;}}
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=
Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*
tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _tagged_arr(*a2string)(void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0)({struct Cyc_Std_String_pa_struct _tmp753;_tmp753.tag=0;_tmp753.f1=(
struct _tagged_arr)a2string((void*)vs->hd);{struct Cyc_Std_String_pa_struct _tmp752;
_tmp752.tag=0;_tmp752.f1=(struct _tagged_arr)msg;{void*_tmp751[2]={& _tmp752,&
_tmp753};Cyc_Tcutil_terr(loc,_tag_arr("%s: %s",sizeof(char),7),_tag_arr(_tmp751,
sizeof(void*),2));}}});}}}static struct _tagged_arr Cyc_Tcutil_strptr2string(struct
_tagged_arr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _tagged_arr msg){((void(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr(*a2string)(struct _tagged_arr*),struct _tagged_arr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Std_zstrptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){((void(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr(*a2string)(struct Cyc_Absyn_Tvar*),struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,
_tag_arr("duplicate type variable",sizeof(char),24));}struct _tuple19{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple20{struct Cyc_List_List*f1;void*f2;};struct _tuple21{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_Std_strcmp(*((
struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,_tag_arr("",sizeof(char),1))!= 0)
fields=({struct Cyc_List_List*_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754->hd=({
struct _tuple19*_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp755->f2=0;_tmp755;});_tmp754->tl=fields;_tmp754;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple20 _tmp757;struct Cyc_List_List*
_tmp758;void*_tmp759;struct _tuple20*_tmp756=(struct _tuple20*)des->hd;_tmp757=*
_tmp756;_tmp758=_tmp757.f1;_tmp759=_tmp757.f2;if(_tmp758 == 0){struct Cyc_List_List*
_tmp75A=fields;for(0;_tmp75A != 0;_tmp75A=_tmp75A->tl){if(!(*((struct _tuple19*)
_tmp75A->hd)).f2){(*((struct _tuple19*)_tmp75A->hd)).f2=1;(*((struct _tuple20*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp75B=_cycalloc(sizeof(*_tmp75B));
_tmp75B->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp75C=_cycalloc(
sizeof(*_tmp75C));_tmp75C[0]=({struct Cyc_Absyn_FieldName_struct _tmp75D;_tmp75D.tag=
1;_tmp75D.f1=((*((struct _tuple19*)_tmp75A->hd)).f1)->name;_tmp75D;});_tmp75C;}));
_tmp75B->tl=0;_tmp75B;});ans=({struct Cyc_List_List*_tmp75E=_region_malloc(rgn,
sizeof(*_tmp75E));_tmp75E->hd=({struct _tuple21*_tmp75F=_region_malloc(rgn,
sizeof(*_tmp75F));_tmp75F->f1=(*((struct _tuple19*)_tmp75A->hd)).f1;_tmp75F->f2=
_tmp759;_tmp75F;});_tmp75E->tl=ans;_tmp75E;});break;}}if(_tmp75A == 0)({void*
_tmp760[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too many arguments to struct",
sizeof(char),29),_tag_arr(_tmp760,sizeof(void*),0));});}else{if(_tmp758->tl != 0)({
void*_tmp761[0]={};Cyc_Tcutil_terr(loc,_tag_arr("multiple designators are not supported",
sizeof(char),39),_tag_arr(_tmp761,sizeof(void*),0));});else{void*_tmp762=(void*)
_tmp758->hd;struct _tagged_arr*_tmp763;_LL4ED: if(*((int*)_tmp762)!= 0)goto _LL4EF;
_LL4EE:({void*_tmp764[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array designator used in argument to struct",
sizeof(char),44),_tag_arr(_tmp764,sizeof(void*),0));});goto _LL4EC;_LL4EF: if(*((
int*)_tmp762)!= 1)goto _LL4EC;_tmp763=((struct Cyc_Absyn_FieldName_struct*)_tmp762)->f1;
_LL4F0: {struct Cyc_List_List*_tmp765=fields;for(0;_tmp765 != 0;_tmp765=_tmp765->tl){
if(Cyc_Std_zstrptrcmp(_tmp763,((*((struct _tuple19*)_tmp765->hd)).f1)->name)== 0){
if((*((struct _tuple19*)_tmp765->hd)).f2)({struct Cyc_Std_String_pa_struct _tmp767;
_tmp767.tag=0;_tmp767.f1=(struct _tagged_arr)*_tmp763;{void*_tmp766[1]={& _tmp767};
Cyc_Tcutil_terr(loc,_tag_arr("field %s has already been used as an argument",
sizeof(char),46),_tag_arr(_tmp766,sizeof(void*),1));}});(*((struct _tuple19*)
_tmp765->hd)).f2=1;ans=({struct Cyc_List_List*_tmp768=_region_malloc(rgn,sizeof(*
_tmp768));_tmp768->hd=({struct _tuple21*_tmp769=_region_malloc(rgn,sizeof(*
_tmp769));_tmp769->f1=(*((struct _tuple19*)_tmp765->hd)).f1;_tmp769->f2=_tmp759;
_tmp769;});_tmp768->tl=ans;_tmp768;});break;}}if(_tmp765 == 0)({struct Cyc_Std_String_pa_struct
_tmp76B;_tmp76B.tag=0;_tmp76B.f1=(struct _tagged_arr)*_tmp763;{void*_tmp76A[1]={&
_tmp76B};Cyc_Tcutil_terr(loc,_tag_arr("bad field designator %s",sizeof(char),24),
_tag_arr(_tmp76A,sizeof(void*),1));}});goto _LL4EC;}_LL4EC:;}}}for(0;fields != 0;
fields=fields->tl){if(!(*((struct _tuple19*)fields->hd)).f2){({void*_tmp76C[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments to struct",sizeof(char),28),
_tag_arr(_tmp76C,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp76D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp76E;void*_tmp76F;struct Cyc_Absyn_Conref*_tmp770;_LL4F2: if(_tmp76D <= (void*)3?
1:*((int*)_tmp76D)!= 4)goto _LL4F4;_tmp76E=((struct Cyc_Absyn_PointerType_struct*)
_tmp76D)->f1;_tmp76F=(void*)_tmp76E.elt_typ;_tmp770=_tmp76E.bounds;_LL4F3: {
struct Cyc_Absyn_Conref*_tmp771=Cyc_Absyn_compress_conref(_tmp770);void*_tmp772=(
void*)(Cyc_Absyn_compress_conref(_tmp771))->v;void*_tmp773;_LL4F7: if(_tmp772 <= (
void*)1?1:*((int*)_tmp772)!= 0)goto _LL4F9;_tmp773=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp772)->f1;if((int)_tmp773 != 0)goto _LL4F9;_LL4F8:*elt_typ_dest=_tmp76F;return 1;
_LL4F9: if((int)_tmp772 != 0)goto _LL4FB;_LL4FA:(void*)(_tmp771->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp775;_tmp775.tag=0;_tmp775.f1=(void*)((void*)
0);_tmp775;});_tmp774;})));*elt_typ_dest=_tmp76F;return 1;_LL4FB:;_LL4FC: return 0;
_LL4F6:;}_LL4F4:;_LL4F5: return 0;_LL4F1:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple9 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple9 bogus_ans=({struct _tuple9 _tmp7B3;_tmp7B3.f1=0;
_tmp7B3.f2=(void*)2;_tmp7B3;});void*_tmp776=(void*)e->r;struct _tuple1*_tmp777;
void*_tmp778;struct Cyc_Absyn_Exp*_tmp779;struct _tagged_arr*_tmp77A;struct Cyc_Absyn_Exp*
_tmp77B;struct _tagged_arr*_tmp77C;struct Cyc_Absyn_Exp*_tmp77D;struct Cyc_Absyn_Exp*
_tmp77E;struct Cyc_Absyn_Exp*_tmp77F;_LL4FE: if(*((int*)_tmp776)!= 1)goto _LL500;
_tmp777=((struct Cyc_Absyn_Var_e_struct*)_tmp776)->f1;_tmp778=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp776)->f2;_LL4FF: {void*_tmp780=_tmp778;struct Cyc_Absyn_Vardecl*_tmp781;
struct Cyc_Absyn_Vardecl*_tmp782;struct Cyc_Absyn_Vardecl*_tmp783;struct Cyc_Absyn_Vardecl*
_tmp784;_LL50B: if((int)_tmp780 != 0)goto _LL50D;_LL50C: goto _LL50E;_LL50D: if(
_tmp780 <= (void*)1?1:*((int*)_tmp780)!= 1)goto _LL50F;_LL50E: return bogus_ans;
_LL50F: if(_tmp780 <= (void*)1?1:*((int*)_tmp780)!= 0)goto _LL511;_tmp781=((struct
Cyc_Absyn_Global_b_struct*)_tmp780)->f1;_LL510: {void*_tmp785=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL518: if(_tmp785 <= (void*)
3?1:*((int*)_tmp785)!= 7)goto _LL51A;_LL519: return({struct _tuple9 _tmp786;_tmp786.f1=
1;_tmp786.f2=(void*)2;_tmp786;});_LL51A:;_LL51B: return({struct _tuple9 _tmp787;
_tmp787.f1=(_tmp781->tq).q_const;_tmp787.f2=(void*)2;_tmp787;});_LL517:;}_LL511:
if(_tmp780 <= (void*)1?1:*((int*)_tmp780)!= 3)goto _LL513;_tmp782=((struct Cyc_Absyn_Local_b_struct*)
_tmp780)->f1;_LL512: {void*_tmp788=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL51D: if(_tmp788 <= (void*)3?1:*((int*)_tmp788)!= 7)
goto _LL51F;_LL51E: return({struct _tuple9 _tmp789;_tmp789.f1=1;_tmp789.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp782->rgn))->v;_tmp789;});_LL51F:;_LL520:
_tmp782->escapes=1;return({struct _tuple9 _tmp78A;_tmp78A.f1=(_tmp782->tq).q_const;
_tmp78A.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp782->rgn))->v;_tmp78A;});
_LL51C:;}_LL513: if(_tmp780 <= (void*)1?1:*((int*)_tmp780)!= 4)goto _LL515;_tmp783=((
struct Cyc_Absyn_Pat_b_struct*)_tmp780)->f1;_LL514: _tmp784=_tmp783;goto _LL516;
_LL515: if(_tmp780 <= (void*)1?1:*((int*)_tmp780)!= 2)goto _LL50A;_tmp784=((struct
Cyc_Absyn_Param_b_struct*)_tmp780)->f1;_LL516: _tmp784->escapes=1;return({struct
_tuple9 _tmp78B;_tmp78B.f1=(_tmp784->tq).q_const;_tmp78B.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp784->rgn))->v;_tmp78B;});_LL50A:;}_LL500: if(*((int*)_tmp776)!= 
21)goto _LL502;_tmp779=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp776)->f1;
_tmp77A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp776)->f2;_LL501: {void*
_tmp78C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp779->topt))->v);
struct Cyc_List_List*_tmp78D;struct Cyc_Absyn_AggrInfo _tmp78E;void*_tmp78F;struct
Cyc_Absyn_Aggrdecl**_tmp790;struct Cyc_Absyn_Aggrdecl*_tmp791;_LL522: if(_tmp78C <= (
void*)3?1:*((int*)_tmp78C)!= 11)goto _LL524;_tmp78D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp78C)->f2;_LL523: {struct Cyc_Absyn_Aggrfield*_tmp792=Cyc_Absyn_lookup_field(
_tmp78D,_tmp77A);if(_tmp792 != 0?_tmp792->width != 0: 0)return({struct _tuple9
_tmp793;_tmp793.f1=(_tmp792->tq).q_const;_tmp793.f2=(Cyc_Tcutil_addressof_props(
te,_tmp779)).f2;_tmp793;});return bogus_ans;}_LL524: if(_tmp78C <= (void*)3?1:*((
int*)_tmp78C)!= 10)goto _LL526;_tmp78E=((struct Cyc_Absyn_AggrType_struct*)_tmp78C)->f1;
_tmp78F=(void*)_tmp78E.aggr_info;if(*((int*)_tmp78F)!= 1)goto _LL526;_tmp790=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp78F)->f1;_tmp791=*_tmp790;_LL525: {struct
Cyc_Absyn_Aggrfield*_tmp794=Cyc_Absyn_lookup_decl_field(_tmp791,_tmp77A);if(
_tmp794 != 0?_tmp794->width != 0: 0)return({struct _tuple9 _tmp795;_tmp795.f1=(
_tmp794->tq).q_const;_tmp795.f2=(Cyc_Tcutil_addressof_props(te,_tmp779)).f2;
_tmp795;});return bogus_ans;}_LL526:;_LL527: return bogus_ans;_LL521:;}_LL502: if(*((
int*)_tmp776)!= 22)goto _LL504;_tmp77B=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp776)->f1;_tmp77C=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp776)->f2;_LL503: {
void*_tmp796=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp77B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp797;void*_tmp798;void*_tmp799;
_LL529: if(_tmp796 <= (void*)3?1:*((int*)_tmp796)!= 4)goto _LL52B;_tmp797=((struct
Cyc_Absyn_PointerType_struct*)_tmp796)->f1;_tmp798=(void*)_tmp797.elt_typ;
_tmp799=(void*)_tmp797.rgn_typ;_LL52A: {struct Cyc_Absyn_Aggrfield*finfo;{void*
_tmp79A=Cyc_Tcutil_compress(_tmp798);struct Cyc_List_List*_tmp79B;struct Cyc_Absyn_AggrInfo
_tmp79C;void*_tmp79D;struct Cyc_Absyn_Aggrdecl**_tmp79E;struct Cyc_Absyn_Aggrdecl*
_tmp79F;_LL52E: if(_tmp79A <= (void*)3?1:*((int*)_tmp79A)!= 11)goto _LL530;_tmp79B=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp79A)->f2;_LL52F: finfo=Cyc_Absyn_lookup_field(
_tmp79B,_tmp77C);goto _LL52D;_LL530: if(_tmp79A <= (void*)3?1:*((int*)_tmp79A)!= 10)
goto _LL532;_tmp79C=((struct Cyc_Absyn_AggrType_struct*)_tmp79A)->f1;_tmp79D=(void*)
_tmp79C.aggr_info;if(*((int*)_tmp79D)!= 1)goto _LL532;_tmp79E=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp79D)->f1;_tmp79F=*_tmp79E;_LL531: finfo=Cyc_Absyn_lookup_decl_field(_tmp79F,
_tmp77C);goto _LL52D;_LL532:;_LL533: return bogus_ans;_LL52D:;}if(finfo != 0?finfo->width
!= 0: 0)return({struct _tuple9 _tmp7A0;_tmp7A0.f1=(finfo->tq).q_const;_tmp7A0.f2=
_tmp799;_tmp7A0;});return bogus_ans;}_LL52B:;_LL52C: return bogus_ans;_LL528:;}
_LL504: if(*((int*)_tmp776)!= 20)goto _LL506;_tmp77D=((struct Cyc_Absyn_Deref_e_struct*)
_tmp776)->f1;_LL505: {void*_tmp7A1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp77D->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7A2;void*_tmp7A3;
struct Cyc_Absyn_Tqual _tmp7A4;_LL535: if(_tmp7A1 <= (void*)3?1:*((int*)_tmp7A1)!= 4)
goto _LL537;_tmp7A2=((struct Cyc_Absyn_PointerType_struct*)_tmp7A1)->f1;_tmp7A3=(
void*)_tmp7A2.rgn_typ;_tmp7A4=_tmp7A2.tq;_LL536: return({struct _tuple9 _tmp7A5;
_tmp7A5.f1=_tmp7A4.q_const;_tmp7A5.f2=_tmp7A3;_tmp7A5;});_LL537:;_LL538: return
bogus_ans;_LL534:;}_LL506: if(*((int*)_tmp776)!= 23)goto _LL508;_tmp77E=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp776)->f1;_tmp77F=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp776)->f2;_LL507: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp77E->topt))->v);void*_tmp7A6=t;struct Cyc_List_List*_tmp7A7;
struct Cyc_Absyn_PtrInfo _tmp7A8;void*_tmp7A9;void*_tmp7AA;struct Cyc_Absyn_Tqual
_tmp7AB;struct Cyc_Absyn_Conref*_tmp7AC;struct Cyc_Absyn_Tqual _tmp7AD;_LL53A: if(
_tmp7A6 <= (void*)3?1:*((int*)_tmp7A6)!= 9)goto _LL53C;_tmp7A7=((struct Cyc_Absyn_TupleType_struct*)
_tmp7A6)->f1;_LL53B: {struct _tuple4*_tmp7AE=Cyc_Absyn_lookup_tuple_field(_tmp7A7,(
int)Cyc_Evexp_eval_const_uint_exp(_tmp77F));if(_tmp7AE != 0)return({struct _tuple9
_tmp7AF;_tmp7AF.f1=((*_tmp7AE).f1).q_const;_tmp7AF.f2=(Cyc_Tcutil_addressof_props(
te,_tmp77E)).f2;_tmp7AF;});return bogus_ans;}_LL53C: if(_tmp7A6 <= (void*)3?1:*((
int*)_tmp7A6)!= 4)goto _LL53E;_tmp7A8=((struct Cyc_Absyn_PointerType_struct*)
_tmp7A6)->f1;_tmp7A9=(void*)_tmp7A8.elt_typ;_tmp7AA=(void*)_tmp7A8.rgn_typ;
_tmp7AB=_tmp7A8.tq;_tmp7AC=_tmp7A8.bounds;_LL53D: return({struct _tuple9 _tmp7B0;
_tmp7B0.f1=_tmp7AB.q_const;_tmp7B0.f2=_tmp7AA;_tmp7B0;});_LL53E: if(_tmp7A6 <= (
void*)3?1:*((int*)_tmp7A6)!= 7)goto _LL540;_tmp7AD=((struct Cyc_Absyn_ArrayType_struct*)
_tmp7A6)->f2;_LL53F: return({struct _tuple9 _tmp7B1;_tmp7B1.f1=_tmp7AD.q_const;
_tmp7B1.f2=(Cyc_Tcutil_addressof_props(te,_tmp77E)).f2;_tmp7B1;});_LL540:;_LL541:
return bogus_ans;_LL539:;}_LL508:;_LL509:({void*_tmp7B2[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("unary & applied to non-lvalue",sizeof(char),30),_tag_arr(_tmp7B2,
sizeof(void*),0));});return bogus_ans;_LL4FD:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp7B4=Cyc_Tcutil_compress(
e_typ);void*_tmp7B5;struct Cyc_Absyn_Tqual _tmp7B6;_LL543: if(_tmp7B4 <= (void*)3?1:*((
int*)_tmp7B4)!= 7)goto _LL545;_tmp7B5=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp7B4)->f1;_tmp7B6=((struct Cyc_Absyn_ArrayType_struct*)_tmp7B4)->f2;_LL544: {
void*_tmp7B8;struct _tuple9 _tmp7B7=Cyc_Tcutil_addressof_props(te,e);_tmp7B8=
_tmp7B7.f2;return Cyc_Absyn_atb_typ(_tmp7B5,_tmp7B8,_tmp7B6,(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp7B9=_cycalloc(sizeof(*_tmp7B9));_tmp7B9[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp7BA;_tmp7BA.tag=0;_tmp7BA.f1=e;_tmp7BA;});_tmp7B9;}));}_LL545:;_LL546: return
e_typ;_LL542:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,
unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(b);{void*
_tmp7BB=(void*)b->v;void*_tmp7BC;void*_tmp7BD;struct Cyc_Absyn_Exp*_tmp7BE;_LL548:
if(_tmp7BB <= (void*)1?1:*((int*)_tmp7BB)!= 0)goto _LL54A;_tmp7BC=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp7BB)->f1;if((int)_tmp7BC != 0)goto _LL54A;_LL549:
return;_LL54A: if(_tmp7BB <= (void*)1?1:*((int*)_tmp7BB)!= 0)goto _LL54C;_tmp7BD=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7BB)->f1;if(_tmp7BD <= (void*)1?1:*((
int*)_tmp7BD)!= 0)goto _LL54C;_tmp7BE=((struct Cyc_Absyn_Upper_b_struct*)_tmp7BD)->f1;
_LL54B: if(Cyc_Evexp_eval_const_uint_exp(_tmp7BE)<= i)({struct Cyc_Std_Int_pa_struct
_tmp7C1;_tmp7C1.tag=1;_tmp7C1.f1=(int)i;{struct Cyc_Std_Int_pa_struct _tmp7C0;
_tmp7C0.tag=1;_tmp7C0.f1=(int)Cyc_Evexp_eval_const_uint_exp(_tmp7BE);{void*
_tmp7BF[2]={& _tmp7C0,& _tmp7C1};Cyc_Tcutil_terr(loc,_tag_arr("dereference is out of bounds: %d <= %d",
sizeof(char),39),_tag_arr(_tmp7BF,sizeof(void*),2));}}});return;_LL54C:;_LL54D:(
void*)(b->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp7C2=_cycalloc(
sizeof(*_tmp7C2));_tmp7C2[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp7C3;_tmp7C3.tag=
0;_tmp7C3.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp7C4=_cycalloc(
sizeof(*_tmp7C4));_tmp7C4[0]=({struct Cyc_Absyn_Upper_b_struct _tmp7C5;_tmp7C5.tag=
0;_tmp7C5.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmp7C5;});_tmp7C4;}));_tmp7C3;});
_tmp7C2;})));return;_LL547:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp7C6=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp7C7;struct Cyc_Absyn_Exp*_tmp7C8;_LL54F: if(_tmp7C6 <= (void*)1?1:*((int*)
_tmp7C6)!= 0)goto _LL551;_tmp7C7=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7C6)->f1;if(_tmp7C7 <= (void*)1?1:*((int*)_tmp7C7)!= 0)goto _LL551;_tmp7C8=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7C7)->f1;_LL550: return Cyc_Evexp_eval_const_uint_exp(
_tmp7C8)== 1;_LL551:;_LL552: return 0;_LL54E:;}int Cyc_Tcutil_bits_only(void*t){
void*_tmp7C9=Cyc_Tcutil_compress(t);void*_tmp7CA;struct Cyc_List_List*_tmp7CB;
struct Cyc_Absyn_AggrInfo _tmp7CC;void*_tmp7CD;void*_tmp7CE;struct Cyc_Absyn_AggrInfo
_tmp7CF;void*_tmp7D0;void*_tmp7D1;struct Cyc_Absyn_AggrInfo _tmp7D2;void*_tmp7D3;
struct Cyc_Absyn_Aggrdecl**_tmp7D4;struct Cyc_Absyn_Aggrdecl*_tmp7D5;struct Cyc_List_List*
_tmp7D6;struct Cyc_List_List*_tmp7D7;_LL554: if((int)_tmp7C9 != 0)goto _LL556;_LL555:
goto _LL557;_LL556: if(_tmp7C9 <= (void*)3?1:*((int*)_tmp7C9)!= 5)goto _LL558;_LL557:
goto _LL559;_LL558: if((int)_tmp7C9 != 1)goto _LL55A;_LL559: goto _LL55B;_LL55A: if(
_tmp7C9 <= (void*)3?1:*((int*)_tmp7C9)!= 6)goto _LL55C;_LL55B: return 1;_LL55C: if(
_tmp7C9 <= (void*)3?1:*((int*)_tmp7C9)!= 12)goto _LL55E;_LL55D: goto _LL55F;_LL55E:
if(_tmp7C9 <= (void*)3?1:*((int*)_tmp7C9)!= 13)goto _LL560;_LL55F: return 0;_LL560:
if(_tmp7C9 <= (void*)3?1:*((int*)_tmp7C9)!= 7)goto _LL562;_tmp7CA=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp7C9)->f1;_LL561: return Cyc_Tcutil_bits_only(
_tmp7CA);_LL562: if(_tmp7C9 <= (void*)3?1:*((int*)_tmp7C9)!= 9)goto _LL564;_tmp7CB=((
struct Cyc_Absyn_TupleType_struct*)_tmp7C9)->f1;_LL563: for(0;_tmp7CB != 0;_tmp7CB=
_tmp7CB->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple4*)_tmp7CB->hd)).f2))
return 0;}return 1;_LL564: if(_tmp7C9 <= (void*)3?1:*((int*)_tmp7C9)!= 10)goto _LL566;
_tmp7CC=((struct Cyc_Absyn_AggrType_struct*)_tmp7C9)->f1;_tmp7CD=(void*)_tmp7CC.aggr_info;
if(*((int*)_tmp7CD)!= 0)goto _LL566;_tmp7CE=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp7CD)->f1;if((int)_tmp7CE != 1)goto _LL566;_LL565: return 1;_LL566: if(_tmp7C9 <= (
void*)3?1:*((int*)_tmp7C9)!= 10)goto _LL568;_tmp7CF=((struct Cyc_Absyn_AggrType_struct*)
_tmp7C9)->f1;_tmp7D0=(void*)_tmp7CF.aggr_info;if(*((int*)_tmp7D0)!= 0)goto _LL568;
_tmp7D1=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp7D0)->f1;if((int)
_tmp7D1 != 0)goto _LL568;_LL567: return 0;_LL568: if(_tmp7C9 <= (void*)3?1:*((int*)
_tmp7C9)!= 10)goto _LL56A;_tmp7D2=((struct Cyc_Absyn_AggrType_struct*)_tmp7C9)->f1;
_tmp7D3=(void*)_tmp7D2.aggr_info;if(*((int*)_tmp7D3)!= 1)goto _LL56A;_tmp7D4=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp7D3)->f1;_tmp7D5=*_tmp7D4;_tmp7D6=_tmp7D2.targs;
_LL569: if((void*)_tmp7D5->kind == (void*)1)return 1;if(_tmp7D5->fields == 0)return 0;{
struct _RegionHandle _tmp7D8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7D8;
_push_region(rgn);{struct Cyc_List_List*_tmp7D9=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp7D5->tvs,_tmp7D6);{struct Cyc_List_List*fs=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7D5->fields))->v;for(0;fs != 
0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp7D9,(void*)((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp7DA=0;_npop_handler(0);return
_tmp7DA;}}}{int _tmp7DB=1;_npop_handler(0);return _tmp7DB;}};_pop_region(rgn);}
_LL56A: if(_tmp7C9 <= (void*)3?1:*((int*)_tmp7C9)!= 11)goto _LL56C;_tmp7D7=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp7C9)->f2;_LL56B: for(0;_tmp7D7 != 0;_tmp7D7=
_tmp7D7->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp7D7->hd)->type))return 0;}return 1;_LL56C:;_LL56D: return 0;_LL553:;}struct
_tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp7DC=(void*)e->r;
struct _tuple1*_tmp7DD;void*_tmp7DE;struct Cyc_Absyn_Exp*_tmp7DF;struct Cyc_Absyn_Exp*
_tmp7E0;struct Cyc_Absyn_Exp*_tmp7E1;struct Cyc_Absyn_Exp*_tmp7E2;struct Cyc_Absyn_Exp*
_tmp7E3;struct Cyc_Absyn_Exp*_tmp7E4;struct Cyc_Absyn_Exp*_tmp7E5;void*_tmp7E6;
struct Cyc_Absyn_Exp*_tmp7E7;struct Cyc_Absyn_Exp*_tmp7E8;struct Cyc_Absyn_Exp*
_tmp7E9;struct Cyc_Absyn_Exp*_tmp7EA;struct Cyc_List_List*_tmp7EB;struct Cyc_List_List*
_tmp7EC;struct Cyc_List_List*_tmp7ED;void*_tmp7EE;struct Cyc_List_List*_tmp7EF;
struct Cyc_List_List*_tmp7F0;struct Cyc_List_List*_tmp7F1;_LL56F: if(*((int*)
_tmp7DC)!= 0)goto _LL571;_LL570: goto _LL572;_LL571: if(*((int*)_tmp7DC)!= 16)goto
_LL573;_LL572: goto _LL574;_LL573: if(*((int*)_tmp7DC)!= 17)goto _LL575;_LL574: goto
_LL576;_LL575: if(*((int*)_tmp7DC)!= 18)goto _LL577;_LL576: goto _LL578;_LL577: if(*((
int*)_tmp7DC)!= 19)goto _LL579;_LL578: goto _LL57A;_LL579: if(*((int*)_tmp7DC)!= 31)
goto _LL57B;_LL57A: goto _LL57C;_LL57B: if(*((int*)_tmp7DC)!= 32)goto _LL57D;_LL57C:
return 1;_LL57D: if(*((int*)_tmp7DC)!= 1)goto _LL57F;_tmp7DD=((struct Cyc_Absyn_Var_e_struct*)
_tmp7DC)->f1;_tmp7DE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp7DC)->f2;_LL57E: {
void*_tmp7F2=_tmp7DE;struct Cyc_Absyn_Vardecl*_tmp7F3;_LL59C: if(_tmp7F2 <= (void*)
1?1:*((int*)_tmp7F2)!= 1)goto _LL59E;_LL59D: return 1;_LL59E: if(_tmp7F2 <= (void*)1?
1:*((int*)_tmp7F2)!= 0)goto _LL5A0;_tmp7F3=((struct Cyc_Absyn_Global_b_struct*)
_tmp7F2)->f1;_LL59F: {void*_tmp7F4=Cyc_Tcutil_compress((void*)_tmp7F3->type);
_LL5A5: if(_tmp7F4 <= (void*)3?1:*((int*)_tmp7F4)!= 7)goto _LL5A7;_LL5A6: goto _LL5A8;
_LL5A7: if(_tmp7F4 <= (void*)3?1:*((int*)_tmp7F4)!= 8)goto _LL5A9;_LL5A8: return 1;
_LL5A9:;_LL5AA: return var_okay;_LL5A4:;}_LL5A0: if((int)_tmp7F2 != 0)goto _LL5A2;
_LL5A1: return 0;_LL5A2:;_LL5A3: return var_okay;_LL59B:;}_LL57F: if(*((int*)_tmp7DC)
!= 6)goto _LL581;_tmp7DF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7DC)->f1;
_tmp7E0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7DC)->f2;_tmp7E1=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp7DC)->f3;_LL580: return(Cyc_Tcutil_cnst_exp(te,0,_tmp7DF)?Cyc_Tcutil_cnst_exp(
te,0,_tmp7E0): 0)?Cyc_Tcutil_cnst_exp(te,0,_tmp7E1): 0;_LL581: if(*((int*)_tmp7DC)
!= 7)goto _LL583;_tmp7E2=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp7DC)->f1;_tmp7E3=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp7DC)->f2;_LL582: return Cyc_Tcutil_cnst_exp(
te,0,_tmp7E2)?Cyc_Tcutil_cnst_exp(te,0,_tmp7E3): 0;_LL583: if(*((int*)_tmp7DC)!= 
11)goto _LL585;_tmp7E4=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp7DC)->f1;
_LL584: _tmp7E5=_tmp7E4;goto _LL586;_LL585: if(*((int*)_tmp7DC)!= 12)goto _LL587;
_tmp7E5=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp7DC)->f1;_LL586: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp7E5);_LL587: if(*((int*)_tmp7DC)!= 13)goto _LL589;_tmp7E6=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp7DC)->f1;_tmp7E7=((struct Cyc_Absyn_Cast_e_struct*)
_tmp7DC)->f2;_LL588: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp7E7);_LL589: if(*((
int*)_tmp7DC)!= 14)goto _LL58B;_tmp7E8=((struct Cyc_Absyn_Address_e_struct*)
_tmp7DC)->f1;_LL58A: return Cyc_Tcutil_cnst_exp(te,1,_tmp7E8);_LL58B: if(*((int*)
_tmp7DC)!= 27)goto _LL58D;_tmp7E9=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp7DC)->f2;_tmp7EA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7DC)->f3;
_LL58C: return Cyc_Tcutil_cnst_exp(te,0,_tmp7E9)?Cyc_Tcutil_cnst_exp(te,0,_tmp7EA):
0;_LL58D: if(*((int*)_tmp7DC)!= 26)goto _LL58F;_tmp7EB=((struct Cyc_Absyn_Array_e_struct*)
_tmp7DC)->f1;_LL58E: _tmp7EC=_tmp7EB;goto _LL590;_LL58F: if(*((int*)_tmp7DC)!= 29)
goto _LL591;_tmp7EC=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp7DC)->f2;_LL590:
_tmp7ED=_tmp7EC;goto _LL592;_LL591: if(*((int*)_tmp7DC)!= 28)goto _LL593;_tmp7ED=((
struct Cyc_Absyn_Struct_e_struct*)_tmp7DC)->f3;_LL592: for(0;_tmp7ED != 0;_tmp7ED=
_tmp7ED->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple22*)_tmp7ED->hd)).f2))
return 0;}return 1;_LL593: if(*((int*)_tmp7DC)!= 3)goto _LL595;_tmp7EE=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp7DC)->f1;_tmp7EF=((struct Cyc_Absyn_Primop_e_struct*)
_tmp7DC)->f2;_LL594: _tmp7F0=_tmp7EF;goto _LL596;_LL595: if(*((int*)_tmp7DC)!= 24)
goto _LL597;_tmp7F0=((struct Cyc_Absyn_Tuple_e_struct*)_tmp7DC)->f1;_LL596: _tmp7F1=
_tmp7F0;goto _LL598;_LL597: if(*((int*)_tmp7DC)!= 30)goto _LL599;_tmp7F1=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp7DC)->f1;_LL598: for(0;_tmp7F1 != 0;_tmp7F1=_tmp7F1->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp7F1->hd))return 0;}return 1;
_LL599:;_LL59A: return 0;_LL56E:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp7F5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp7F6;
struct Cyc_Absyn_Conref*_tmp7F7;struct Cyc_Absyn_Conref*_tmp7F8;void*_tmp7F9;
struct Cyc_List_List*_tmp7FA;struct Cyc_Absyn_AggrInfo _tmp7FB;void*_tmp7FC;struct
Cyc_List_List*_tmp7FD;struct Cyc_List_List*_tmp7FE;_LL5AC: if((int)_tmp7F5 != 0)
goto _LL5AE;_LL5AD: goto _LL5AF;_LL5AE: if(_tmp7F5 <= (void*)3?1:*((int*)_tmp7F5)!= 5)
goto _LL5B0;_LL5AF: goto _LL5B1;_LL5B0: if((int)_tmp7F5 != 1)goto _LL5B2;_LL5B1: goto
_LL5B3;_LL5B2: if(_tmp7F5 <= (void*)3?1:*((int*)_tmp7F5)!= 6)goto _LL5B4;_LL5B3:
return 1;_LL5B4: if(_tmp7F5 <= (void*)3?1:*((int*)_tmp7F5)!= 4)goto _LL5B6;_tmp7F6=((
struct Cyc_Absyn_PointerType_struct*)_tmp7F5)->f1;_tmp7F7=_tmp7F6.nullable;
_tmp7F8=_tmp7F6.bounds;_LL5B5: {void*_tmp7FF=(void*)(Cyc_Absyn_compress_conref(
_tmp7F8))->v;void*_tmp800;void*_tmp801;_LL5C5: if(_tmp7FF <= (void*)1?1:*((int*)
_tmp7FF)!= 0)goto _LL5C7;_tmp800=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7FF)->f1;if((int)_tmp800 != 0)goto _LL5C7;_LL5C6: return 1;_LL5C7: if(_tmp7FF <= (
void*)1?1:*((int*)_tmp7FF)!= 0)goto _LL5C9;_tmp801=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7FF)->f1;if(_tmp801 <= (void*)1?1:*((int*)_tmp801)!= 0)goto _LL5C9;_LL5C8: {
void*_tmp802=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp7F7))->v;int _tmp803;_LL5CC: if(_tmp802 <= (void*)1?1:*((int*)_tmp802)!= 0)goto
_LL5CE;_tmp803=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp802)->f1;_LL5CD:
return _tmp803;_LL5CE:;_LL5CF: return 0;_LL5CB:;}_LL5C9:;_LL5CA: return 0;_LL5C4:;}
_LL5B6: if(_tmp7F5 <= (void*)3?1:*((int*)_tmp7F5)!= 7)goto _LL5B8;_tmp7F9=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp7F5)->f1;_LL5B7: return Cyc_Tcutil_supports_default(
_tmp7F9);_LL5B8: if(_tmp7F5 <= (void*)3?1:*((int*)_tmp7F5)!= 9)goto _LL5BA;_tmp7FA=((
struct Cyc_Absyn_TupleType_struct*)_tmp7F5)->f1;_LL5B9: for(0;_tmp7FA != 0;_tmp7FA=
_tmp7FA->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple4*)_tmp7FA->hd)).f2))
return 0;}return 1;_LL5BA: if(_tmp7F5 <= (void*)3?1:*((int*)_tmp7F5)!= 10)goto _LL5BC;
_tmp7FB=((struct Cyc_Absyn_AggrType_struct*)_tmp7F5)->f1;_tmp7FC=(void*)_tmp7FB.aggr_info;
_tmp7FD=_tmp7FB.targs;_LL5BB: {struct Cyc_Absyn_Aggrdecl*_tmp804=Cyc_Absyn_get_known_aggrdecl(
_tmp7FC);if(_tmp804->fields == 0)return 0;if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp804->exist_vars))->v != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp804->tvs,_tmp7FD,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp804->fields))->v);}_LL5BC: if(_tmp7F5 <= (void*)3?1:*((int*)_tmp7F5)!= 11)goto
_LL5BE;_tmp7FE=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp7F5)->f2;_LL5BD: return
Cyc_Tcutil_fields_support_default(0,0,_tmp7FE);_LL5BE: if(_tmp7F5 <= (void*)3?1:*((
int*)_tmp7F5)!= 13)goto _LL5C0;_LL5BF: goto _LL5C1;_LL5C0: if(_tmp7F5 <= (void*)3?1:*((
int*)_tmp7F5)!= 12)goto _LL5C2;_LL5C1: return 1;_LL5C2:;_LL5C3: return 0;_LL5AB:;}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmp805=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp805;_push_region(rgn);{struct Cyc_List_List*_tmp806=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp806,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmp807=0;_npop_handler(0);return _tmp807;}}};
_pop_region(rgn);}return 1;}int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp808=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp809;void*_tmp80A;struct Cyc_Absyn_FnInfo _tmp80B;
struct Cyc_List_List*_tmp80C;_LL5D1: if(_tmp808 <= (void*)3?1:*((int*)_tmp808)!= 4)
goto _LL5D3;_tmp809=((struct Cyc_Absyn_PointerType_struct*)_tmp808)->f1;_tmp80A=(
void*)_tmp809.elt_typ;_LL5D2: return Cyc_Tcutil_is_noreturn(_tmp80A);_LL5D3: if(
_tmp808 <= (void*)3?1:*((int*)_tmp808)!= 8)goto _LL5D5;_tmp80B=((struct Cyc_Absyn_FnType_struct*)
_tmp808)->f1;_tmp80C=_tmp80B.attributes;_LL5D4: for(0;_tmp80C != 0;_tmp80C=_tmp80C->tl){
void*_tmp80D=(void*)_tmp80C->hd;_LL5D8: if((int)_tmp80D != 3)goto _LL5DA;_LL5D9:
return 1;_LL5DA:;_LL5DB: continue;_LL5D7:;}goto _LL5D0;_LL5D5:;_LL5D6: goto _LL5D0;
_LL5D0:;}return 0;}
