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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple1*,struct _tuple1*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple1*Cyc_Absyn_tunion_print_arg_qvar;extern struct _tuple1*Cyc_Absyn_tunion_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct
Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct
_tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,
struct _tagged_arr*);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};struct
_tuple4*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);struct _tagged_arr
Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*a);struct _tuple5{
void*f1;struct _tuple1*f2;};struct _tuple5 Cyc_Absyn_aggr_kinded_name(void*);struct
Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
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
_tmp6E;struct Cyc_List_List*_tmp6F;void*_tmp70;int _tmp71;void*_tmp72;void*_tmp73;
struct _tuple1*_tmp74;struct Cyc_List_List*_tmp75;struct Cyc_Absyn_Typedefdecl*
_tmp76;void*_tmp77;struct Cyc_List_List*_tmp78;void*_tmp79;_LL1A: if((int)_tmp43 != 
0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 0)
goto _LL1E;_LL1D: return t;_LL1E: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 1)goto
_LL20;_tmp44=((struct Cyc_Absyn_VarType_struct*)_tmp43)->f1;_LL1F: return(void*)({
struct Cyc_Absyn_VarType_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({
struct Cyc_Absyn_VarType_struct _tmp7B;_tmp7B.tag=1;_tmp7B.f1=Cyc_Tcutil_copy_tvar(
_tmp44);_tmp7B;});_tmp7A;});_LL20: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 2)
goto _LL22;_tmp45=((struct Cyc_Absyn_TunionType_struct*)_tmp43)->f1;_tmp46=(void*)
_tmp45.tunion_info;_tmp47=_tmp45.targs;_tmp48=(void*)_tmp45.rgn;_LL21: return(
void*)({struct Cyc_Absyn_TunionType_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));
_tmp7C[0]=({struct Cyc_Absyn_TunionType_struct _tmp7D;_tmp7D.tag=2;_tmp7D.f1=({
struct Cyc_Absyn_TunionInfo _tmp7E;_tmp7E.tunion_info=(void*)_tmp46;_tmp7E.targs=
Cyc_Tcutil_copy_types(_tmp47);_tmp7E.rgn=(void*)Cyc_Tcutil_copy_type(_tmp48);
_tmp7E;});_tmp7D;});_tmp7C;});_LL22: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 3)
goto _LL24;_tmp49=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp43)->f1;_tmp4A=(
void*)_tmp49.field_info;_tmp4B=_tmp49.targs;_LL23: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp80;_tmp80.tag=3;_tmp80.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp81;_tmp81.field_info=(
void*)_tmp4A;_tmp81.targs=Cyc_Tcutil_copy_types(_tmp4B);_tmp81;});_tmp80;});
_tmp7F;});_LL24: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 4)goto _LL26;_tmp4C=((
struct Cyc_Absyn_PointerType_struct*)_tmp43)->f1;_tmp4D=(void*)_tmp4C.elt_typ;
_tmp4E=(void*)_tmp4C.rgn_typ;_tmp4F=_tmp4C.nullable;_tmp50=_tmp4C.tq;_tmp51=
_tmp4C.bounds;_LL25: {void*_tmp82=Cyc_Tcutil_copy_type(_tmp4D);void*_tmp83=Cyc_Tcutil_copy_type(
_tmp4E);struct Cyc_Absyn_Conref*_tmp84=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp4F);struct Cyc_Absyn_Tqual _tmp85=_tmp50;struct Cyc_Absyn_Conref*
_tmp86=Cyc_Tcutil_copy_conref(_tmp51);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_PointerType_struct
_tmp88;_tmp88.tag=4;_tmp88.f1=({struct Cyc_Absyn_PtrInfo _tmp89;_tmp89.elt_typ=(
void*)_tmp82;_tmp89.rgn_typ=(void*)_tmp83;_tmp89.nullable=_tmp84;_tmp89.tq=
_tmp85;_tmp89.bounds=_tmp86;_tmp89;});_tmp88;});_tmp87;});}_LL26: if(_tmp43 <= (
void*)3?1:*((int*)_tmp43)!= 5)goto _LL28;_tmp52=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp43)->f1;_tmp53=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp43)->f2;_LL27:
return(void*)({struct Cyc_Absyn_IntType_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));
_tmp8A[0]=({struct Cyc_Absyn_IntType_struct _tmp8B;_tmp8B.tag=5;_tmp8B.f1=(void*)
_tmp52;_tmp8B.f2=(void*)_tmp53;_tmp8B;});_tmp8A;});_LL28: if((int)_tmp43 != 1)goto
_LL2A;_LL29: return t;_LL2A: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 6)goto _LL2C;
_tmp54=((struct Cyc_Absyn_DoubleType_struct*)_tmp43)->f1;_LL2B: return(void*)({
struct Cyc_Absyn_DoubleType_struct*_tmp8C=_cycalloc_atomic(sizeof(*_tmp8C));
_tmp8C[0]=({struct Cyc_Absyn_DoubleType_struct _tmp8D;_tmp8D.tag=6;_tmp8D.f1=
_tmp54;_tmp8D;});_tmp8C;});_LL2C: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 7)goto
_LL2E;_tmp55=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp43)->f1;_tmp56=((
struct Cyc_Absyn_ArrayType_struct*)_tmp43)->f2;_tmp57=((struct Cyc_Absyn_ArrayType_struct*)
_tmp43)->f3;_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp8E=
_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=({struct Cyc_Absyn_ArrayType_struct _tmp8F;
_tmp8F.tag=7;_tmp8F.f1=(void*)Cyc_Tcutil_copy_type(_tmp55);_tmp8F.f2=_tmp56;
_tmp8F.f3=_tmp57;_tmp8F;});_tmp8E;});_LL2E: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)
!= 8)goto _LL30;_tmp58=((struct Cyc_Absyn_FnType_struct*)_tmp43)->f1;_tmp59=_tmp58.tvars;
_tmp5A=_tmp58.effect;_tmp5B=(void*)_tmp58.ret_typ;_tmp5C=_tmp58.args;_tmp5D=
_tmp58.c_varargs;_tmp5E=_tmp58.cyc_varargs;_tmp5F=_tmp58.rgn_po;_tmp60=_tmp58.attributes;
_LL2F: {struct Cyc_List_List*_tmp90=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp59);struct Cyc_Core_Opt*_tmp91=_tmp5A == 0?0:({struct Cyc_Core_Opt*_tmp9B=
_cycalloc(sizeof(*_tmp9B));_tmp9B->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp5A->v);
_tmp9B;});void*_tmp92=Cyc_Tcutil_copy_type(_tmp5B);struct Cyc_List_List*_tmp93=((
struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp5C);int _tmp94=_tmp5D;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp5E != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp5E);cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp95=_cycalloc(
sizeof(*_tmp95));_tmp95->name=cv->name;_tmp95->tq=cv->tq;_tmp95->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmp95->inject=cv->inject;_tmp95;});}{struct Cyc_List_List*_tmp96=((
struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp5F);struct Cyc_List_List*_tmp97=_tmp60;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmp98=_cycalloc(sizeof(*_tmp98));
_tmp98[0]=({struct Cyc_Absyn_FnType_struct _tmp99;_tmp99.tag=8;_tmp99.f1=({struct
Cyc_Absyn_FnInfo _tmp9A;_tmp9A.tvars=_tmp90;_tmp9A.effect=_tmp91;_tmp9A.ret_typ=(
void*)_tmp92;_tmp9A.args=_tmp93;_tmp9A.c_varargs=_tmp94;_tmp9A.cyc_varargs=
cyc_varargs2;_tmp9A.rgn_po=_tmp96;_tmp9A.attributes=_tmp97;_tmp9A;});_tmp99;});
_tmp98;});}}_LL30: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 9)goto _LL32;_tmp61=((
struct Cyc_Absyn_TupleType_struct*)_tmp43)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Absyn_TupleType_struct
_tmp9D;_tmp9D.tag=9;_tmp9D.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(
struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp61);
_tmp9D;});_tmp9C;});_LL32: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 10)goto _LL34;
_tmp62=((struct Cyc_Absyn_AggrType_struct*)_tmp43)->f1;_tmp63=(void*)_tmp62.aggr_info;
if(*((int*)_tmp63)!= 0)goto _LL34;_tmp64=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp63)->f1;_tmp65=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp63)->f2;_tmp66=
_tmp62.targs;_LL33: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp9E=
_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({struct Cyc_Absyn_AggrType_struct _tmp9F;
_tmp9F.tag=10;_tmp9F.f1=({struct Cyc_Absyn_AggrInfo _tmpA0;_tmpA0.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));
_tmpA1[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmpA2;_tmpA2.tag=0;_tmpA2.f1=(
void*)_tmp64;_tmpA2.f2=_tmp65;_tmpA2;});_tmpA1;}));_tmpA0.targs=Cyc_Tcutil_copy_types(
_tmp66);_tmpA0;});_tmp9F;});_tmp9E;});_LL34: if(_tmp43 <= (void*)3?1:*((int*)
_tmp43)!= 10)goto _LL36;_tmp67=((struct Cyc_Absyn_AggrType_struct*)_tmp43)->f1;
_tmp68=(void*)_tmp67.aggr_info;if(*((int*)_tmp68)!= 1)goto _LL36;_tmp69=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp68)->f1;_tmp6A=_tmp67.targs;_LL35: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({
struct Cyc_Absyn_AggrType_struct _tmpA4;_tmpA4.tag=10;_tmpA4.f1=({struct Cyc_Absyn_AggrInfo
_tmpA5;_tmpA5.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpA6=
_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpA7;
_tmpA7.tag=1;_tmpA7.f1=_tmp69;_tmpA7;});_tmpA6;}));_tmpA5.targs=Cyc_Tcutil_copy_types(
_tmp6A);_tmpA5;});_tmpA4;});_tmpA3;});_LL36: if(_tmp43 <= (void*)3?1:*((int*)
_tmp43)!= 11)goto _LL38;_tmp6B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp43)->f1;_tmp6C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp43)->f2;_LL37:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpA8=_cycalloc(sizeof(*
_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpA9;_tmpA9.tag=11;
_tmpA9.f1=(void*)_tmp6B;_tmpA9.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp6C);_tmpA9;});_tmpA8;});_LL38: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 12)
goto _LL3A;_tmp6D=((struct Cyc_Absyn_EnumType_struct*)_tmp43)->f1;_tmp6E=((struct
Cyc_Absyn_EnumType_struct*)_tmp43)->f2;_LL39: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Absyn_EnumType_struct
_tmpAB;_tmpAB.tag=12;_tmpAB.f1=_tmp6D;_tmpAB.f2=_tmp6E;_tmpAB;});_tmpAA;});_LL3A:
if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 13)goto _LL3C;_tmp6F=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp43)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpAC=
_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpAD;
_tmpAD.tag=13;_tmpAD.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp6F);_tmpAD;});_tmpAC;});_LL3C: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 14)
goto _LL3E;_tmp70=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp43)->f1;_LL3D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));
_tmpAE[0]=({struct Cyc_Absyn_SizeofType_struct _tmpAF;_tmpAF.tag=14;_tmpAF.f1=(
void*)Cyc_Tcutil_copy_type(_tmp70);_tmpAF;});_tmpAE;});_LL3E: if(_tmp43 <= (void*)
3?1:*((int*)_tmp43)!= 18)goto _LL40;_tmp71=((struct Cyc_Absyn_TypeInt_struct*)
_tmp43)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpB0=
_cycalloc_atomic(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpB1;_tmpB1.tag=18;_tmpB1.f1=_tmp71;_tmpB1;});_tmpB0;});_LL40: if(_tmp43 <= (
void*)3?1:*((int*)_tmp43)!= 17)goto _LL42;_tmp72=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp43)->f1;_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpB2=_cycalloc(
sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_TagType_struct _tmpB3;_tmpB3.tag=17;
_tmpB3.f1=(void*)Cyc_Tcutil_copy_type(_tmp72);_tmpB3;});_tmpB2;});_LL42: if(
_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 15)goto _LL44;_tmp73=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp43)->f1;_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpB4=
_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpB5;_tmpB5.tag=15;_tmpB5.f1=(void*)Cyc_Tcutil_copy_type(_tmp73);_tmpB5;});
_tmpB4;});_LL44: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 16)goto _LL46;_tmp74=((
struct Cyc_Absyn_TypedefType_struct*)_tmp43)->f1;_tmp75=((struct Cyc_Absyn_TypedefType_struct*)
_tmp43)->f2;_tmp76=((struct Cyc_Absyn_TypedefType_struct*)_tmp43)->f3;_LL45:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpB6=_cycalloc(sizeof(*
_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_TypedefType_struct _tmpB7;_tmpB7.tag=16;
_tmpB7.f1=_tmp74;_tmpB7.f2=Cyc_Tcutil_copy_types(_tmp75);_tmpB7.f3=_tmp76;_tmpB7.f4=
0;_tmpB7;});_tmpB6;});_LL46: if((int)_tmp43 != 2)goto _LL48;_LL47: return t;_LL48: if(
_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 19)goto _LL4A;_tmp77=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp43)->f1;_LL49: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpB8=
_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_AccessEff_struct _tmpB9;
_tmpB9.tag=19;_tmpB9.f1=(void*)Cyc_Tcutil_copy_type(_tmp77);_tmpB9;});_tmpB8;});
_LL4A: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 20)goto _LL4C;_tmp78=((struct Cyc_Absyn_JoinEff_struct*)
_tmp43)->f1;_LL4B: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpBA=_cycalloc(
sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Absyn_JoinEff_struct _tmpBB;_tmpBB.tag=20;
_tmpBB.f1=Cyc_Tcutil_copy_types(_tmp78);_tmpBB;});_tmpBA;});_LL4C: if(_tmp43 <= (
void*)3?1:*((int*)_tmp43)!= 21)goto _LL19;_tmp79=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp43)->f1;_LL4D: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpBC=_cycalloc(
sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpBD;_tmpBD.tag=21;
_tmpBD.f1=(void*)Cyc_Tcutil_copy_type(_tmp79);_tmpBD;});_tmpBC;});_LL19:;}int Cyc_Tcutil_kind_leq(
void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple6 _tmpBF=({struct _tuple6 _tmpBE;
_tmpBE.f1=k1;_tmpBE.f2=k2;_tmpBE;});void*_tmpC0;void*_tmpC1;void*_tmpC2;void*
_tmpC3;void*_tmpC4;void*_tmpC5;_LL4F: _tmpC0=_tmpBF.f1;if((int)_tmpC0 != 2)goto
_LL51;_tmpC1=_tmpBF.f2;if((int)_tmpC1 != 1)goto _LL51;_LL50: goto _LL52;_LL51: _tmpC2=
_tmpBF.f1;if((int)_tmpC2 != 2)goto _LL53;_tmpC3=_tmpBF.f2;if((int)_tmpC3 != 0)goto
_LL53;_LL52: goto _LL54;_LL53: _tmpC4=_tmpBF.f1;if((int)_tmpC4 != 1)goto _LL55;_tmpC5=
_tmpBF.f2;if((int)_tmpC5 != 0)goto _LL55;_LL54: return 1;_LL55:;_LL56: return 0;_LL4E:;}}
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpC6=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpC7;void*_tmpC8;_LL58: if(*((int*)_tmpC6)!= 0)goto _LL5A;
_tmpC7=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpC6)->f1;_LL59: return _tmpC7;
_LL5A: if(*((int*)_tmpC6)!= 2)goto _LL5C;_tmpC8=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpC6)->f2;_LL5B: return _tmpC8;_LL5C:;_LL5D:({void*_tmpC9[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("kind not suitably constrained!",
sizeof(char),31),_tag_arr(_tmpC9,sizeof(void*),0));});_LL57:;}void*Cyc_Tcutil_typ_kind(
void*t){void*_tmpCA=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpCB;struct Cyc_Core_Opt*
_tmpCC;struct Cyc_Absyn_Tvar*_tmpCD;void*_tmpCE;struct Cyc_Absyn_TunionFieldInfo
_tmpCF;void*_tmpD0;struct Cyc_Absyn_Tunionfield*_tmpD1;struct Cyc_Absyn_TunionFieldInfo
_tmpD2;void*_tmpD3;struct Cyc_Absyn_Enumdecl*_tmpD4;struct Cyc_Absyn_Enumdecl*
_tmpD5;struct Cyc_Absyn_PtrInfo _tmpD6;struct Cyc_Absyn_Typedefdecl*_tmpD7;_LL5F:
if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 0)goto _LL61;_tmpCB=((struct Cyc_Absyn_Evar_struct*)
_tmpCA)->f1;_tmpCC=((struct Cyc_Absyn_Evar_struct*)_tmpCA)->f2;_LL60: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpCB))->v;_LL61: if(_tmpCA <= (void*)3?1:*((int*)
_tmpCA)!= 1)goto _LL63;_tmpCD=((struct Cyc_Absyn_VarType_struct*)_tmpCA)->f1;_LL62:
return Cyc_Tcutil_tvar_kind(_tmpCD);_LL63: if((int)_tmpCA != 0)goto _LL65;_LL64:
return(void*)1;_LL65: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 5)goto _LL67;_tmpCE=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpCA)->f2;_LL66: return _tmpCE == (void*)
2?(void*)2:(void*)1;_LL67: if((int)_tmpCA != 1)goto _LL69;_LL68: goto _LL6A;_LL69: if(
_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 6)goto _LL6B;_LL6A: goto _LL6C;_LL6B: if(
_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 8)goto _LL6D;_LL6C: return(void*)1;_LL6D: if(
_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 15)goto _LL6F;_LL6E: return(void*)2;_LL6F:
if((int)_tmpCA != 2)goto _LL71;_LL70: return(void*)3;_LL71: if(_tmpCA <= (void*)3?1:*((
int*)_tmpCA)!= 2)goto _LL73;_LL72: return(void*)2;_LL73: if(_tmpCA <= (void*)3?1:*((
int*)_tmpCA)!= 3)goto _LL75;_tmpCF=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpCA)->f1;_tmpD0=(void*)_tmpCF.field_info;if(*((int*)_tmpD0)!= 1)goto _LL75;
_tmpD1=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpD0)->f2;_LL74: if(_tmpD1->typs
== 0)return(void*)2;else{return(void*)1;}_LL75: if(_tmpCA <= (void*)3?1:*((int*)
_tmpCA)!= 3)goto _LL77;_tmpD2=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpCA)->f1;
_tmpD3=(void*)_tmpD2.field_info;if(*((int*)_tmpD3)!= 0)goto _LL77;_LL76:({void*
_tmpD8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typ_kind: Unresolved TunionFieldType",sizeof(char),37),_tag_arr(_tmpD8,
sizeof(void*),0));});_LL77: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 12)goto _LL79;
_tmpD4=((struct Cyc_Absyn_EnumType_struct*)_tmpCA)->f2;if(_tmpD4 != 0)goto _LL79;
_LL78: return(void*)0;_LL79: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 10)goto _LL7B;
_LL7A: goto _LL7C;_LL7B: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 11)goto _LL7D;
_LL7C: goto _LL7E;_LL7D: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 13)goto _LL7F;
_LL7E: return(void*)1;_LL7F: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 12)goto _LL81;
_tmpD5=((struct Cyc_Absyn_EnumType_struct*)_tmpCA)->f2;_LL80: if(_tmpD5->fields == 
0)return(void*)0;else{return(void*)1;}_LL81: if(_tmpCA <= (void*)3?1:*((int*)
_tmpCA)!= 4)goto _LL83;_tmpD6=((struct Cyc_Absyn_PointerType_struct*)_tmpCA)->f1;
_LL82: {void*_tmpD9=(void*)(Cyc_Absyn_compress_conref(_tmpD6.bounds))->v;void*
_tmpDA;void*_tmpDB;_LL96: if(_tmpD9 <= (void*)1?1:*((int*)_tmpD9)!= 0)goto _LL98;
_tmpDA=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpD9)->f1;if((int)_tmpDA != 0)
goto _LL98;_LL97: return(void*)1;_LL98: if(_tmpD9 <= (void*)1?1:*((int*)_tmpD9)!= 0)
goto _LL9A;_tmpDB=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpD9)->f1;if(
_tmpDB <= (void*)1?1:*((int*)_tmpDB)!= 0)goto _LL9A;_LL99: return(void*)2;_LL9A: if((
int)_tmpD9 != 0)goto _LL9C;_LL9B: return(void*)1;_LL9C: if(_tmpD9 <= (void*)1?1:*((
int*)_tmpD9)!= 1)goto _LL95;_LL9D:({void*_tmpDC[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof(char),43),_tag_arr(_tmpDC,sizeof(void*),0));});_LL95:;}_LL83: if(_tmpCA <= (
void*)3?1:*((int*)_tmpCA)!= 14)goto _LL85;_LL84: return(void*)2;_LL85: if(_tmpCA <= (
void*)3?1:*((int*)_tmpCA)!= 18)goto _LL87;_LL86: return(void*)5;_LL87: if(_tmpCA <= (
void*)3?1:*((int*)_tmpCA)!= 17)goto _LL89;_LL88: return(void*)2;_LL89: if(_tmpCA <= (
void*)3?1:*((int*)_tmpCA)!= 7)goto _LL8B;_LL8A: goto _LL8C;_LL8B: if(_tmpCA <= (void*)
3?1:*((int*)_tmpCA)!= 9)goto _LL8D;_LL8C: return(void*)1;_LL8D: if(_tmpCA <= (void*)
3?1:*((int*)_tmpCA)!= 16)goto _LL8F;_tmpD7=((struct Cyc_Absyn_TypedefType_struct*)
_tmpCA)->f3;_LL8E: if(_tmpD7 == 0?1: _tmpD7->kind == 0)({struct Cyc_Std_String_pa_struct
_tmpDE;_tmpDE.tag=0;_tmpDE.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpDD[1]={& _tmpDE};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typ_kind: typedef found: %s",sizeof(char),28),_tag_arr(_tmpDD,sizeof(
void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD7->kind))->v;
_LL8F: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 19)goto _LL91;_LL90: goto _LL92;
_LL91: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 20)goto _LL93;_LL92: goto _LL94;
_LL93: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 21)goto _LL5E;_LL94: return(void*)4;
_LL5E:;}int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmpDF;
_push_handler(& _tmpDF);{int _tmpE1=0;if(setjmp(_tmpDF.handler))_tmpE1=1;if(!
_tmpE1){Cyc_Tcutil_unify_it(t1,t2);{int _tmpE2=1;_npop_handler(0);return _tmpE2;};
_pop_handler();}else{void*_tmpE0=(void*)_exn_thrown;void*_tmpE4=_tmpE0;_LL9F: if(
_tmpE4 != Cyc_Tcutil_Unify)goto _LLA1;_LLA0: return 0;_LLA1:;_LLA2:(void)_throw(
_tmpE4);_LL9E:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){void*_tmpE5=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmpE6;struct Cyc_Core_Opt*_tmpE7;struct Cyc_Core_Opt*
_tmpE8;struct Cyc_Core_Opt**_tmpE9;struct Cyc_Absyn_PtrInfo _tmpEA;void*_tmpEB;
struct Cyc_Absyn_FnInfo _tmpEC;struct Cyc_List_List*_tmpED;struct Cyc_Core_Opt*
_tmpEE;void*_tmpEF;struct Cyc_List_List*_tmpF0;int _tmpF1;struct Cyc_Absyn_VarargInfo*
_tmpF2;struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_List_List*
_tmpF5;struct Cyc_Absyn_TunionInfo _tmpF6;struct Cyc_List_List*_tmpF7;void*_tmpF8;
struct Cyc_List_List*_tmpF9;struct Cyc_Core_Opt*_tmpFA;struct Cyc_Absyn_TunionFieldInfo
_tmpFB;struct Cyc_List_List*_tmpFC;struct Cyc_Absyn_AggrInfo _tmpFD;struct Cyc_List_List*
_tmpFE;struct Cyc_List_List*_tmpFF;void*_tmp100;void*_tmp101;void*_tmp102;void*
_tmp103;struct Cyc_List_List*_tmp104;_LLA4: if(_tmpE5 <= (void*)3?1:*((int*)_tmpE5)
!= 1)goto _LLA6;_tmpE6=((struct Cyc_Absyn_VarType_struct*)_tmpE5)->f1;_LLA5: if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmpE6)){
Cyc_Tcutil_failure_reason=_tag_arr("(type variable would escape scope)",sizeof(
char),35);(int)_throw((void*)Cyc_Tcutil_Unify);}goto _LLA3;_LLA6: if(_tmpE5 <= (
void*)3?1:*((int*)_tmpE5)!= 0)goto _LLA8;_tmpE7=((struct Cyc_Absyn_Evar_struct*)
_tmpE5)->f2;_tmpE8=((struct Cyc_Absyn_Evar_struct*)_tmpE5)->f4;_tmpE9=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmpE5)->f4;_LLA7: if(t == evar){Cyc_Tcutil_failure_reason=
_tag_arr("(occurs check)",sizeof(char),15);(int)_throw((void*)Cyc_Tcutil_Unify);}
else{if(_tmpE7 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpE7->v);else{int
problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpE9))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp105=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpE9))->v;for(0;s != 
0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp105=({struct Cyc_List_List*_tmp106=
_cycalloc(sizeof(*_tmp106));_tmp106->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp106->tl=
_tmp105;_tmp106;});}}*_tmpE9=({struct Cyc_Core_Opt*_tmp107=_cycalloc(sizeof(*
_tmp107));_tmp107->v=_tmp105;_tmp107;});}}}goto _LLA3;_LLA8: if(_tmpE5 <= (void*)3?
1:*((int*)_tmpE5)!= 4)goto _LLAA;_tmpEA=((struct Cyc_Absyn_PointerType_struct*)
_tmpE5)->f1;_LLA9: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpEA.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmpEA.rgn_typ);goto _LLA3;_LLAA: if(_tmpE5 <= (void*)3?1:*((int*)
_tmpE5)!= 7)goto _LLAC;_tmpEB=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpE5)->f1;
_LLAB: Cyc_Tcutil_occurs(evar,r,env,_tmpEB);goto _LLA3;_LLAC: if(_tmpE5 <= (void*)3?
1:*((int*)_tmpE5)!= 8)goto _LLAE;_tmpEC=((struct Cyc_Absyn_FnType_struct*)_tmpE5)->f1;
_tmpED=_tmpEC.tvars;_tmpEE=_tmpEC.effect;_tmpEF=(void*)_tmpEC.ret_typ;_tmpF0=
_tmpEC.args;_tmpF1=_tmpEC.c_varargs;_tmpF2=_tmpEC.cyc_varargs;_tmpF3=_tmpEC.rgn_po;
_tmpF4=_tmpEC.attributes;_LLAD: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmpED,env);if(
_tmpEE != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpEE->v);Cyc_Tcutil_occurs(evar,
r,env,_tmpEF);for(0;_tmpF0 != 0;_tmpF0=_tmpF0->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple2*)_tmpF0->hd)).f3);}if(_tmpF2 != 0)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmpF2->type);for(0;_tmpF3 != 0;_tmpF3=_tmpF3->tl){struct _tuple6 _tmp109;
void*_tmp10A;void*_tmp10B;struct _tuple6*_tmp108=(struct _tuple6*)_tmpF3->hd;
_tmp109=*_tmp108;_tmp10A=_tmp109.f1;_tmp10B=_tmp109.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp10A);Cyc_Tcutil_occurs(evar,r,env,_tmp10B);}goto _LLA3;_LLAE: if(_tmpE5 <= (
void*)3?1:*((int*)_tmpE5)!= 9)goto _LLB0;_tmpF5=((struct Cyc_Absyn_TupleType_struct*)
_tmpE5)->f1;_LLAF: for(0;_tmpF5 != 0;_tmpF5=_tmpF5->tl){Cyc_Tcutil_occurs(evar,r,
env,(*((struct _tuple4*)_tmpF5->hd)).f2);}goto _LLA3;_LLB0: if(_tmpE5 <= (void*)3?1:*((
int*)_tmpE5)!= 2)goto _LLB2;_tmpF6=((struct Cyc_Absyn_TunionType_struct*)_tmpE5)->f1;
_tmpF7=_tmpF6.targs;_tmpF8=(void*)_tmpF6.rgn;_LLB1: Cyc_Tcutil_occurs(evar,r,env,
_tmpF8);Cyc_Tcutil_occurslist(evar,r,env,_tmpF7);goto _LLA3;_LLB2: if(_tmpE5 <= (
void*)3?1:*((int*)_tmpE5)!= 16)goto _LLB4;_tmpF9=((struct Cyc_Absyn_TypedefType_struct*)
_tmpE5)->f2;_tmpFA=((struct Cyc_Absyn_TypedefType_struct*)_tmpE5)->f4;_LLB3:
_tmpFC=_tmpF9;goto _LLB5;_LLB4: if(_tmpE5 <= (void*)3?1:*((int*)_tmpE5)!= 3)goto
_LLB6;_tmpFB=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpE5)->f1;_tmpFC=_tmpFB.targs;
_LLB5: _tmpFE=_tmpFC;goto _LLB7;_LLB6: if(_tmpE5 <= (void*)3?1:*((int*)_tmpE5)!= 10)
goto _LLB8;_tmpFD=((struct Cyc_Absyn_AggrType_struct*)_tmpE5)->f1;_tmpFE=_tmpFD.targs;
_LLB7: Cyc_Tcutil_occurslist(evar,r,env,_tmpFE);goto _LLA3;_LLB8: if(_tmpE5 <= (void*)
3?1:*((int*)_tmpE5)!= 11)goto _LLBA;_tmpFF=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE5)->f2;_LLB9: for(0;_tmpFF != 0;_tmpFF=_tmpFF->tl){Cyc_Tcutil_occurs(evar,r,
env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmpFF->hd)->type);}goto _LLA3;_LLBA: if(
_tmpE5 <= (void*)3?1:*((int*)_tmpE5)!= 15)goto _LLBC;_tmp100=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmpE5)->f1;_LLBB: _tmp101=_tmp100;goto _LLBD;_LLBC: if(_tmpE5 <= (void*)3?1:*((int*)
_tmpE5)!= 14)goto _LLBE;_tmp101=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmpE5)->f1;_LLBD: _tmp102=_tmp101;goto _LLBF;_LLBE: if(_tmpE5 <= (void*)3?1:*((int*)
_tmpE5)!= 19)goto _LLC0;_tmp102=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmpE5)->f1;
_LLBF: _tmp103=_tmp102;goto _LLC1;_LLC0: if(_tmpE5 <= (void*)3?1:*((int*)_tmpE5)!= 
21)goto _LLC2;_tmp103=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmpE5)->f1;_LLC1:
Cyc_Tcutil_occurs(evar,r,env,_tmp103);goto _LLA3;_LLC2: if(_tmpE5 <= (void*)3?1:*((
int*)_tmpE5)!= 20)goto _LLC4;_tmp104=((struct Cyc_Absyn_JoinEff_struct*)_tmpE5)->f1;
_LLC3: Cyc_Tcutil_occurslist(evar,r,env,_tmp104);goto _LLA3;_LLC4:;_LLC5: goto _LLA3;
_LLA3:;}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(
evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*
t1,struct Cyc_List_List*t2){for(0;t1 != 0?t2 != 0: 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((
void*)t1->hd,(void*)t2->hd);}if(t1 != 0?1: t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){if((tq1.q_const != tq2.q_const?1: tq1.q_volatile != tq2.q_volatile)?1: tq1.q_restrict
!= tq2.q_restrict){Cyc_Tcutil_failure_reason=_tag_arr("(qualifiers don't match)",
sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.q_const == tq2.q_const?
tq1.q_volatile == tq2.q_volatile: 0)?tq1.q_restrict == tq2.q_restrict: 0;}static void
Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y,struct _tagged_arr reason){x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);if(x == y)return;{void*_tmp10C=(void*)x->v;void*
_tmp10D;_LLC7: if((int)_tmp10C != 0)goto _LLC9;_LLC8:(void*)(x->v=(void*)((void*)({
struct Cyc_Absyn_Forward_constr_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));
_tmp10E[0]=({struct Cyc_Absyn_Forward_constr_struct _tmp10F;_tmp10F.tag=1;_tmp10F.f1=
y;_tmp10F;});_tmp10E;})));return;_LLC9: if(_tmp10C <= (void*)1?1:*((int*)_tmp10C)
!= 1)goto _LLCB;_LLCA:({void*_tmp110[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress",
sizeof(char),37),_tag_arr(_tmp110,sizeof(void*),0));});_LLCB: if(_tmp10C <= (void*)
1?1:*((int*)_tmp10C)!= 0)goto _LLC6;_tmp10D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10C)->f1;_LLCC: {void*_tmp111=(void*)y->v;void*_tmp112;_LLCE: if((int)_tmp111
!= 0)goto _LLD0;_LLCF:(void*)(y->v=(void*)((void*)x->v));return;_LLD0: if(_tmp111
<= (void*)1?1:*((int*)_tmp111)!= 1)goto _LLD2;_LLD1:({void*_tmp113[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress(2)",
sizeof(char),40),_tag_arr(_tmp113,sizeof(void*),0));});_LLD2: if(_tmp111 <= (void*)
1?1:*((int*)_tmp111)!= 0)goto _LLCD;_tmp112=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp111)->f1;_LLD3: if(cmp(_tmp10D,_tmp112)!= 0){Cyc_Tcutil_failure_reason=reason;(
int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLCD:;}_LLC6:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp114;_push_handler(& _tmp114);{int _tmp116=0;if(setjmp(_tmp114.handler))
_tmp116=1;if(!_tmp116){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)
_tag_arr(0,0,0));{int _tmp117=1;_npop_handler(0);return _tmp117;};_pop_handler();}
else{void*_tmp115=(void*)_exn_thrown;void*_tmp119=_tmp115;_LLD5: if(_tmp119 != Cyc_Tcutil_Unify)
goto _LLD7;_LLD6: return 0;_LLD7:;_LLD8:(void)_throw(_tmp119);_LLD4:;}}}static int
Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple6 _tmp11B=({struct _tuple6
_tmp11A;_tmp11A.f1=b1;_tmp11A.f2=b2;_tmp11A;});void*_tmp11C;void*_tmp11D;void*
_tmp11E;void*_tmp11F;void*_tmp120;struct Cyc_Absyn_Exp*_tmp121;void*_tmp122;
struct Cyc_Absyn_Exp*_tmp123;_LLDA: _tmp11C=_tmp11B.f1;if((int)_tmp11C != 0)goto
_LLDC;_tmp11D=_tmp11B.f2;if((int)_tmp11D != 0)goto _LLDC;_LLDB: return 0;_LLDC:
_tmp11E=_tmp11B.f1;if((int)_tmp11E != 0)goto _LLDE;_LLDD: return - 1;_LLDE: _tmp11F=
_tmp11B.f2;if((int)_tmp11F != 0)goto _LLE0;_LLDF: return 1;_LLE0: _tmp120=_tmp11B.f1;
if(_tmp120 <= (void*)1?1:*((int*)_tmp120)!= 0)goto _LLD9;_tmp121=((struct Cyc_Absyn_Upper_b_struct*)
_tmp120)->f1;_tmp122=_tmp11B.f2;if(_tmp122 <= (void*)1?1:*((int*)_tmp122)!= 0)
goto _LLD9;_tmp123=((struct Cyc_Absyn_Upper_b_struct*)_tmp122)->f1;_LLE1: {int i1=(
int)Cyc_Evexp_eval_const_uint_exp(_tmp121);int i2=(int)Cyc_Evexp_eval_const_uint_exp(
_tmp123);if(i1 == i2)return 0;if(i1 < i2)return - 1;return 1;}_LLD9:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){if(a1 == a2)return 1;{struct _tuple6 _tmp125=({struct _tuple6 _tmp124;
_tmp124.f1=a1;_tmp124.f2=a2;_tmp124;});void*_tmp126;void*_tmp127;int _tmp128;int
_tmp129;void*_tmp12A;void*_tmp12B;int _tmp12C;int _tmp12D;void*_tmp12E;int _tmp12F;
void*_tmp130;int _tmp131;void*_tmp132;int _tmp133;void*_tmp134;int _tmp135;void*
_tmp136;struct _tagged_arr _tmp137;void*_tmp138;struct _tagged_arr _tmp139;_LLE3:
_tmp126=_tmp125.f1;if(_tmp126 <= (void*)16?1:*((int*)_tmp126)!= 3)goto _LLE5;
_tmp127=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp126)->f1;_tmp128=((
struct Cyc_Absyn_Format_att_struct*)_tmp126)->f2;_tmp129=((struct Cyc_Absyn_Format_att_struct*)
_tmp126)->f3;_tmp12A=_tmp125.f2;if(_tmp12A <= (void*)16?1:*((int*)_tmp12A)!= 3)
goto _LLE5;_tmp12B=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp12A)->f1;
_tmp12C=((struct Cyc_Absyn_Format_att_struct*)_tmp12A)->f2;_tmp12D=((struct Cyc_Absyn_Format_att_struct*)
_tmp12A)->f3;_LLE4: return(_tmp127 == _tmp12B?_tmp128 == _tmp12C: 0)?_tmp129 == 
_tmp12D: 0;_LLE5: _tmp12E=_tmp125.f1;if(_tmp12E <= (void*)16?1:*((int*)_tmp12E)!= 0)
goto _LLE7;_tmp12F=((struct Cyc_Absyn_Regparm_att_struct*)_tmp12E)->f1;_tmp130=
_tmp125.f2;if(_tmp130 <= (void*)16?1:*((int*)_tmp130)!= 0)goto _LLE7;_tmp131=((
struct Cyc_Absyn_Regparm_att_struct*)_tmp130)->f1;_LLE6: _tmp133=_tmp12F;_tmp135=
_tmp131;goto _LLE8;_LLE7: _tmp132=_tmp125.f1;if(_tmp132 <= (void*)16?1:*((int*)
_tmp132)!= 1)goto _LLE9;_tmp133=((struct Cyc_Absyn_Aligned_att_struct*)_tmp132)->f1;
_tmp134=_tmp125.f2;if(_tmp134 <= (void*)16?1:*((int*)_tmp134)!= 1)goto _LLE9;
_tmp135=((struct Cyc_Absyn_Aligned_att_struct*)_tmp134)->f1;_LLE8: return _tmp133 == 
_tmp135;_LLE9: _tmp136=_tmp125.f1;if(_tmp136 <= (void*)16?1:*((int*)_tmp136)!= 2)
goto _LLEB;_tmp137=((struct Cyc_Absyn_Section_att_struct*)_tmp136)->f1;_tmp138=
_tmp125.f2;if(_tmp138 <= (void*)16?1:*((int*)_tmp138)!= 2)goto _LLEB;_tmp139=((
struct Cyc_Absyn_Section_att_struct*)_tmp138)->f1;_LLEA: return Cyc_Std_strcmp(
_tmp137,_tmp139)== 0;_LLEB:;_LLEC: return 0;_LLE2:;}}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return
0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_normalize_effect(void*e);
static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(struct
Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((void*)af->type);}static struct
_tuple7*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{void*
_tmp13A=Cyc_Tcutil_tvar_kind(tv);_LLEE: if((int)_tmp13A != 3)goto _LLF0;_LLEF: t=(
void*)2;goto _LLED;_LLF0: if((int)_tmp13A != 4)goto _LLF2;_LLF1: t=Cyc_Absyn_empty_effect;
goto _LLED;_LLF2:;_LLF3: t=Cyc_Absyn_sint_typ;goto _LLED;_LLED:;}return({struct
_tuple7*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->f1=tv;_tmp13B->f2=t;_tmp13B;});}
static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp13C=Cyc_Tcutil_compress(t);void*
_tmp13D;struct Cyc_Absyn_TunionInfo _tmp13E;struct Cyc_List_List*_tmp13F;void*
_tmp140;struct Cyc_Absyn_PtrInfo _tmp141;void*_tmp142;void*_tmp143;void*_tmp144;
struct Cyc_List_List*_tmp145;struct Cyc_Absyn_TunionFieldInfo _tmp146;struct Cyc_List_List*
_tmp147;struct Cyc_Absyn_AggrInfo _tmp148;struct Cyc_List_List*_tmp149;struct Cyc_List_List*
_tmp14A;void*_tmp14B;struct Cyc_Absyn_FnInfo _tmp14C;struct Cyc_List_List*_tmp14D;
struct Cyc_Core_Opt*_tmp14E;void*_tmp14F;struct Cyc_List_List*_tmp150;struct Cyc_Absyn_VarargInfo*
_tmp151;struct Cyc_List_List*_tmp152;void*_tmp153;struct Cyc_List_List*_tmp154;
_LLF5: if((int)_tmp13C != 0)goto _LLF7;_LLF6: goto _LLF8;_LLF7: if((int)_tmp13C != 1)
goto _LLF9;_LLF8: goto _LLFA;_LLF9: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 6)
goto _LLFB;_LLFA: goto _LLFC;_LLFB: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 12)
goto _LLFD;_LLFC: goto _LLFE;_LLFD: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 13)
goto _LLFF;_LLFE: goto _LL100;_LLFF: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 18)
goto _LL101;_LL100: goto _LL102;_LL101: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 5)
goto _LL103;_LL102: return Cyc_Absyn_empty_effect;_LL103: if(_tmp13C <= (void*)3?1:*((
int*)_tmp13C)!= 0)goto _LL105;_LL104: goto _LL106;_LL105: if(_tmp13C <= (void*)3?1:*((
int*)_tmp13C)!= 1)goto _LL107;_LL106: {void*_tmp155=Cyc_Tcutil_typ_kind(t);_LL128:
if((int)_tmp155 != 3)goto _LL12A;_LL129: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp157;_tmp157.tag=19;_tmp157.f1=(void*)t;_tmp157;});_tmp156;});_LL12A: if((int)
_tmp155 != 4)goto _LL12C;_LL12B: return t;_LL12C: if((int)_tmp155 != 5)goto _LL12E;
_LL12D: return Cyc_Absyn_empty_effect;_LL12E:;_LL12F: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp159;_tmp159.tag=21;_tmp159.f1=(void*)t;_tmp159;});_tmp158;});_LL127:;}_LL107:
if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 15)goto _LL109;_tmp13D=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp13C)->f1;_LL108: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp15B;_tmp15B.tag=19;_tmp15B.f1=(void*)_tmp13D;_tmp15B;});_tmp15A;});_LL109:
if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 2)goto _LL10B;_tmp13E=((struct Cyc_Absyn_TunionType_struct*)
_tmp13C)->f1;_tmp13F=_tmp13E.targs;_tmp140=(void*)_tmp13E.rgn;_LL10A: {struct Cyc_List_List*
ts=({struct Cyc_List_List*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp15F=_cycalloc(sizeof(*_tmp15F));
_tmp15F[0]=({struct Cyc_Absyn_AccessEff_struct _tmp160;_tmp160.tag=19;_tmp160.f1=(
void*)_tmp140;_tmp160;});_tmp15F;}));_tmp15E->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp13F);_tmp15E;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp15D;_tmp15D.tag=20;_tmp15D.f1=ts;_tmp15D;});_tmp15C;}));}_LL10B: if(_tmp13C <= (
void*)3?1:*((int*)_tmp13C)!= 4)goto _LL10D;_tmp141=((struct Cyc_Absyn_PointerType_struct*)
_tmp13C)->f1;_tmp142=(void*)_tmp141.elt_typ;_tmp143=(void*)_tmp141.rgn_typ;
_LL10C: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp162;_tmp162.tag=20;_tmp162.f1=({void*_tmp163[2];_tmp163[1]=Cyc_Tcutil_rgns_of(
_tmp142);_tmp163[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp164=_cycalloc(
sizeof(*_tmp164));_tmp164[0]=({struct Cyc_Absyn_AccessEff_struct _tmp165;_tmp165.tag=
19;_tmp165.f1=(void*)_tmp143;_tmp165;});_tmp164;});Cyc_List_list(_tag_arr(
_tmp163,sizeof(void*),2));});_tmp162;});_tmp161;}));_LL10D: if(_tmp13C <= (void*)3?
1:*((int*)_tmp13C)!= 7)goto _LL10F;_tmp144=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp13C)->f1;_LL10E: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp144));
_LL10F: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 9)goto _LL111;_tmp145=((struct
Cyc_Absyn_TupleType_struct*)_tmp13C)->f1;_LL110: {struct Cyc_List_List*_tmp166=0;
for(0;_tmp145 != 0;_tmp145=_tmp145->tl){_tmp166=({struct Cyc_List_List*_tmp167=
_cycalloc(sizeof(*_tmp167));_tmp167->hd=(void*)(*((struct _tuple4*)_tmp145->hd)).f2;
_tmp167->tl=_tmp166;_tmp167;});}_tmp147=_tmp166;goto _LL112;}_LL111: if(_tmp13C <= (
void*)3?1:*((int*)_tmp13C)!= 3)goto _LL113;_tmp146=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp13C)->f1;_tmp147=_tmp146.targs;_LL112: _tmp149=_tmp147;goto _LL114;_LL113: if(
_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 10)goto _LL115;_tmp148=((struct Cyc_Absyn_AggrType_struct*)
_tmp13C)->f1;_tmp149=_tmp148.targs;_LL114: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp168=_cycalloc(sizeof(*_tmp168));
_tmp168[0]=({struct Cyc_Absyn_JoinEff_struct _tmp169;_tmp169.tag=20;_tmp169.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp149);_tmp169;});_tmp168;}));_LL115: if(_tmp13C <= (void*)3?
1:*((int*)_tmp13C)!= 11)goto _LL117;_tmp14A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp13C)->f2;_LL116: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp16B;_tmp16B.tag=20;_tmp16B.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp14A);_tmp16B;});
_tmp16A;}));_LL117: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 14)goto _LL119;
_tmp14B=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp13C)->f1;_LL118: return
Cyc_Tcutil_rgns_of(_tmp14B);_LL119: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 17)
goto _LL11B;_LL11A: return Cyc_Tcutil_rgns_of(t);_LL11B: if(_tmp13C <= (void*)3?1:*((
int*)_tmp13C)!= 8)goto _LL11D;_tmp14C=((struct Cyc_Absyn_FnType_struct*)_tmp13C)->f1;
_tmp14D=_tmp14C.tvars;_tmp14E=_tmp14C.effect;_tmp14F=(void*)_tmp14C.ret_typ;
_tmp150=_tmp14C.args;_tmp151=_tmp14C.cyc_varargs;_tmp152=_tmp14C.rgn_po;_LL11C: {
void*_tmp16C=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple7*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp14D),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp14E))->v);return Cyc_Tcutil_normalize_effect(
_tmp16C);}_LL11D: if((int)_tmp13C != 2)goto _LL11F;_LL11E: return Cyc_Absyn_empty_effect;
_LL11F: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 19)goto _LL121;_LL120: goto
_LL122;_LL121: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 20)goto _LL123;_LL122:
return t;_LL123: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 21)goto _LL125;_tmp153=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp13C)->f1;_LL124: return Cyc_Tcutil_rgns_of(
_tmp153);_LL125: if(_tmp13C <= (void*)3?1:*((int*)_tmp13C)!= 16)goto _LLF4;_tmp154=((
struct Cyc_Absyn_TypedefType_struct*)_tmp13C)->f2;_LL126: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp16D=_cycalloc(sizeof(*_tmp16D));
_tmp16D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp16E;_tmp16E.tag=20;_tmp16E.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp154);_tmp16E;});_tmp16D;}));_LLF4:;}static void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp16F=e;struct Cyc_List_List*_tmp170;
struct Cyc_List_List**_tmp171;void*_tmp172;_LL131: if(_tmp16F <= (void*)3?1:*((int*)
_tmp16F)!= 20)goto _LL133;_tmp170=((struct Cyc_Absyn_JoinEff_struct*)_tmp16F)->f1;
_tmp171=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)_tmp16F)->f1;
_LL132: {int redo_join=0;{struct Cyc_List_List*effs=*_tmp171;for(0;effs != 0;effs=
effs->tl){void*_tmp173=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect(_tmp173)));{void*_tmp174=(void*)effs->hd;void*
_tmp175;_LL138: if(_tmp174 <= (void*)3?1:*((int*)_tmp174)!= 20)goto _LL13A;_LL139:
goto _LL13B;_LL13A: if(_tmp174 <= (void*)3?1:*((int*)_tmp174)!= 19)goto _LL13C;
_tmp175=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp174)->f1;if((int)_tmp175
!= 2)goto _LL13C;_LL13B: redo_join=1;goto _LL137;_LL13C:;_LL13D: goto _LL137;_LL137:;}}}
if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp171;for(0;effs != 0;effs=effs->tl){void*_tmp176=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp177;void*_tmp178;_LL13F: if(_tmp176 <= (void*)3?
1:*((int*)_tmp176)!= 20)goto _LL141;_tmp177=((struct Cyc_Absyn_JoinEff_struct*)
_tmp176)->f1;_LL140: effects=Cyc_List_revappend(_tmp177,effects);goto _LL13E;
_LL141: if(_tmp176 <= (void*)3?1:*((int*)_tmp176)!= 19)goto _LL143;_tmp178=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp176)->f1;if((int)_tmp178 != 2)goto _LL143;
_LL142: goto _LL13E;_LL143:;_LL144: effects=({struct Cyc_List_List*_tmp179=_cycalloc(
sizeof(*_tmp179));_tmp179->hd=(void*)_tmp176;_tmp179->tl=effects;_tmp179;});goto
_LL13E;_LL13E:;}}*_tmp171=Cyc_List_imp_rev(effects);return e;}}_LL133: if(_tmp16F
<= (void*)3?1:*((int*)_tmp16F)!= 21)goto _LL135;_tmp172=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp16F)->f1;_LL134: return Cyc_Tcutil_rgns_of(_tmp172);_LL135:;_LL136: return e;
_LL130:;}}struct _tuple10{void*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;}
;static struct _tuple10*Cyc_Tcutil_get_effect_evar(void*t){void*_tmp17A=Cyc_Tcutil_compress(
t);struct Cyc_List_List*_tmp17B;struct Cyc_List_List _tmp17C;void*_tmp17D;struct Cyc_List_List*
_tmp17E;struct Cyc_Core_Opt*_tmp17F;struct Cyc_Core_Opt*_tmp180;_LL146: if(_tmp17A
<= (void*)3?1:*((int*)_tmp17A)!= 20)goto _LL148;_tmp17B=((struct Cyc_Absyn_JoinEff_struct*)
_tmp17A)->f1;if(_tmp17B == 0)goto _LL148;_tmp17C=*_tmp17B;_tmp17D=(void*)_tmp17C.hd;
_tmp17E=_tmp17C.tl;_LL147: {void*_tmp181=Cyc_Tcutil_compress(_tmp17D);struct Cyc_Core_Opt*
_tmp182;_LL14D: if(_tmp181 <= (void*)3?1:*((int*)_tmp181)!= 0)goto _LL14F;_tmp182=((
struct Cyc_Absyn_Evar_struct*)_tmp181)->f4;_LL14E: return({struct _tuple10*_tmp183=
_cycalloc(sizeof(*_tmp183));_tmp183->f1=_tmp17D;_tmp183->f2=_tmp17E;_tmp183->f3=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp182))->v;_tmp183;});
_LL14F:;_LL150: return 0;_LL14C:;}_LL148: if(_tmp17A <= (void*)3?1:*((int*)_tmp17A)
!= 0)goto _LL14A;_tmp17F=((struct Cyc_Absyn_Evar_struct*)_tmp17A)->f1;_tmp180=((
struct Cyc_Absyn_Evar_struct*)_tmp17A)->f4;_LL149: if(_tmp17F == 0?1:(void*)_tmp17F->v
!= (void*)4)({void*_tmp184[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",sizeof(char),27),
_tag_arr(_tmp184,sizeof(void*),0));});return({struct _tuple10*_tmp185=_cycalloc(
sizeof(*_tmp185));_tmp185->f1=t;_tmp185->f2=0;_tmp185->f3=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp180))->v;_tmp185;});_LL14A:;_LL14B: return 0;
_LL145:;}static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*
Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*_tmp186=({struct
Cyc_Absyn_FnType_struct*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct
Cyc_Absyn_FnType_struct _tmp188;_tmp188.tag=8;_tmp188.f1=({struct Cyc_Absyn_FnInfo
_tmp189;_tmp189.tvars=0;_tmp189.effect=({struct Cyc_Core_Opt*_tmp18A=_cycalloc(
sizeof(*_tmp18A));_tmp18A->v=(void*)eff;_tmp18A;});_tmp189.ret_typ=(void*)((void*)
0);_tmp189.args=0;_tmp189.c_varargs=0;_tmp189.cyc_varargs=0;_tmp189.rgn_po=0;
_tmp189.attributes=0;_tmp189;});_tmp188;});_tmp187;});return Cyc_Absyn_atb_typ((
void*)_tmp186,(void*)2,Cyc_Absyn_empty_tqual(),Cyc_Absyn_bounds_one);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{
void*_tmp18B=Cyc_Tcutil_compress(e);void*_tmp18C;struct Cyc_List_List*_tmp18D;
void*_tmp18E;struct Cyc_Core_Opt*_tmp18F;struct Cyc_Core_Opt*_tmp190;struct Cyc_Core_Opt**
_tmp191;struct Cyc_Core_Opt*_tmp192;_LL152: if(_tmp18B <= (void*)3?1:*((int*)
_tmp18B)!= 19)goto _LL154;_tmp18C=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp18B)->f1;_LL153: if(constrain)return Cyc_Tcutil_unify(r,_tmp18C);_tmp18C=Cyc_Tcutil_compress(
_tmp18C);if(r == _tmp18C)return 1;{struct _tuple6 _tmp194=({struct _tuple6 _tmp193;
_tmp193.f1=r;_tmp193.f2=_tmp18C;_tmp193;});void*_tmp195;struct Cyc_Absyn_Tvar*
_tmp196;void*_tmp197;struct Cyc_Absyn_Tvar*_tmp198;_LL15D: _tmp195=_tmp194.f1;if(
_tmp195 <= (void*)3?1:*((int*)_tmp195)!= 1)goto _LL15F;_tmp196=((struct Cyc_Absyn_VarType_struct*)
_tmp195)->f1;_tmp197=_tmp194.f2;if(_tmp197 <= (void*)3?1:*((int*)_tmp197)!= 1)
goto _LL15F;_tmp198=((struct Cyc_Absyn_VarType_struct*)_tmp197)->f1;_LL15E: return
Cyc_Absyn_tvar_cmp(_tmp196,_tmp198)== 0;_LL15F:;_LL160: return 0;_LL15C:;}_LL154:
if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 20)goto _LL156;_tmp18D=((struct Cyc_Absyn_JoinEff_struct*)
_tmp18B)->f1;_LL155: for(0;_tmp18D != 0;_tmp18D=_tmp18D->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp18D->hd))return 1;}return 0;_LL156: if(_tmp18B <= (void*)3?1:*((
int*)_tmp18B)!= 21)goto _LL158;_tmp18E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp18B)->f1;_LL157: {void*_tmp199=Cyc_Tcutil_rgns_of(_tmp18E);void*_tmp19A;
_LL162: if(_tmp199 <= (void*)3?1:*((int*)_tmp199)!= 21)goto _LL164;_tmp19A=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp199)->f1;_LL163: if(!constrain)return 0;{void*
_tmp19B=Cyc_Tcutil_compress(_tmp19A);struct Cyc_Core_Opt*_tmp19C;struct Cyc_Core_Opt*
_tmp19D;struct Cyc_Core_Opt**_tmp19E;struct Cyc_Core_Opt*_tmp19F;_LL167: if(_tmp19B
<= (void*)3?1:*((int*)_tmp19B)!= 0)goto _LL169;_tmp19C=((struct Cyc_Absyn_Evar_struct*)
_tmp19B)->f1;_tmp19D=((struct Cyc_Absyn_Evar_struct*)_tmp19B)->f2;_tmp19E=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp19B)->f2;_tmp19F=((struct Cyc_Absyn_Evar_struct*)
_tmp19B)->f4;_LL168: {void*_tmp1A0=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp19F);Cyc_Tcutil_occurs(_tmp1A0,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp19F))->v,r);{void*_tmp1A1=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));
_tmp1A3[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1A4;_tmp1A4.tag=20;_tmp1A4.f1=({
void*_tmp1A5[2];_tmp1A5[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1A6=
_cycalloc(sizeof(*_tmp1A6));_tmp1A6[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1A7;
_tmp1A7.tag=19;_tmp1A7.f1=(void*)r;_tmp1A7;});_tmp1A6;});_tmp1A5[0]=_tmp1A0;Cyc_List_list(
_tag_arr(_tmp1A5,sizeof(void*),2));});_tmp1A4;});_tmp1A3;}));*_tmp19E=({struct
Cyc_Core_Opt*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->v=(void*)_tmp1A1;
_tmp1A2;});return 1;}}_LL169:;_LL16A: return 0;_LL166:;}_LL164:;_LL165: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp199);_LL161:;}_LL158: if(_tmp18B <= (void*)3?1:*((int*)_tmp18B)!= 
0)goto _LL15A;_tmp18F=((struct Cyc_Absyn_Evar_struct*)_tmp18B)->f1;_tmp190=((
struct Cyc_Absyn_Evar_struct*)_tmp18B)->f2;_tmp191=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp18B)->f2;_tmp192=((struct Cyc_Absyn_Evar_struct*)
_tmp18B)->f4;_LL159: if(_tmp18F == 0?1:(void*)_tmp18F->v != (void*)4)({void*_tmp1A8[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(char),27),_tag_arr(_tmp1A8,sizeof(
void*),0));});if(!constrain)return 0;{void*_tmp1A9=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp192);Cyc_Tcutil_occurs(_tmp1A9,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp192))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp1AA=({struct Cyc_Absyn_JoinEff_struct*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));
_tmp1AC[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1AD;_tmp1AD.tag=20;_tmp1AD.f1=({
struct Cyc_List_List*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->hd=(void*)
_tmp1A9;_tmp1AE->tl=({struct Cyc_List_List*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));
_tmp1AF->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1B0=_cycalloc(
sizeof(*_tmp1B0));_tmp1B0[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1B1;_tmp1B1.tag=
19;_tmp1B1.f1=(void*)r;_tmp1B1;});_tmp1B0;}));_tmp1AF->tl=0;_tmp1AF;});_tmp1AE;});
_tmp1AD;});_tmp1AC;});*_tmp191=({struct Cyc_Core_Opt*_tmp1AB=_cycalloc(sizeof(*
_tmp1AB));_tmp1AB->v=(void*)((void*)_tmp1AA);_tmp1AB;});return 1;}}_LL15A:;_LL15B:
return 0;_LL151:;}}static int Cyc_Tcutil_type_in_effect(int constrain,void*t,void*e){
t=Cyc_Tcutil_compress(t);{void*_tmp1B2=Cyc_Tcutil_compress(e);struct Cyc_List_List*
_tmp1B3;void*_tmp1B4;struct Cyc_Core_Opt*_tmp1B5;struct Cyc_Core_Opt*_tmp1B6;
struct Cyc_Core_Opt**_tmp1B7;struct Cyc_Core_Opt*_tmp1B8;_LL16C: if(_tmp1B2 <= (void*)
3?1:*((int*)_tmp1B2)!= 19)goto _LL16E;_LL16D: return 0;_LL16E: if(_tmp1B2 <= (void*)3?
1:*((int*)_tmp1B2)!= 20)goto _LL170;_tmp1B3=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1B2)->f1;_LL16F: for(0;_tmp1B3 != 0;_tmp1B3=_tmp1B3->tl){if(Cyc_Tcutil_type_in_effect(
constrain,t,(void*)_tmp1B3->hd))return 1;}return 0;_LL170: if(_tmp1B2 <= (void*)3?1:*((
int*)_tmp1B2)!= 21)goto _LL172;_tmp1B4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1B2)->f1;_LL171: _tmp1B4=Cyc_Tcutil_compress(_tmp1B4);if(t == _tmp1B4)return 1;
if(constrain)return Cyc_Tcutil_unify(t,_tmp1B4);{void*_tmp1B9=Cyc_Tcutil_rgns_of(
t);void*_tmp1BA;_LL177: if(_tmp1B9 <= (void*)3?1:*((int*)_tmp1B9)!= 21)goto _LL179;
_tmp1BA=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1B9)->f1;_LL178: {struct
_tuple6 _tmp1BC=({struct _tuple6 _tmp1BB;_tmp1BB.f1=t;_tmp1BB.f2=Cyc_Tcutil_compress(
_tmp1BA);_tmp1BB;});void*_tmp1BD;struct Cyc_Absyn_Tvar*_tmp1BE;void*_tmp1BF;
struct Cyc_Absyn_Tvar*_tmp1C0;_LL17C: _tmp1BD=_tmp1BC.f1;if(_tmp1BD <= (void*)3?1:*((
int*)_tmp1BD)!= 1)goto _LL17E;_tmp1BE=((struct Cyc_Absyn_VarType_struct*)_tmp1BD)->f1;
_tmp1BF=_tmp1BC.f2;if(_tmp1BF <= (void*)3?1:*((int*)_tmp1BF)!= 1)goto _LL17E;
_tmp1C0=((struct Cyc_Absyn_VarType_struct*)_tmp1BF)->f1;_LL17D: return Cyc_Tcutil_unify(
t,_tmp1BA);_LL17E:;_LL17F: return t == _tmp1BA;_LL17B:;}_LL179:;_LL17A: return Cyc_Tcutil_type_in_effect(
constrain,t,_tmp1B9);_LL176:;}_LL172: if(_tmp1B2 <= (void*)3?1:*((int*)_tmp1B2)!= 
0)goto _LL174;_tmp1B5=((struct Cyc_Absyn_Evar_struct*)_tmp1B2)->f1;_tmp1B6=((
struct Cyc_Absyn_Evar_struct*)_tmp1B2)->f2;_tmp1B7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1B2)->f2;_tmp1B8=((struct Cyc_Absyn_Evar_struct*)
_tmp1B2)->f4;_LL173: if(_tmp1B5 == 0?1:(void*)_tmp1B5->v != (void*)4)({void*_tmp1C1[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(char),27),_tag_arr(_tmp1C1,sizeof(
void*),0));});if(!constrain)return 0;{void*_tmp1C2=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp1B8);Cyc_Tcutil_occurs(_tmp1C2,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1B8))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp1C3=({struct Cyc_Absyn_JoinEff_struct*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));
_tmp1C5[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1C6;_tmp1C6.tag=20;_tmp1C6.f1=({
struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->hd=(void*)
_tmp1C2;_tmp1C7->tl=({struct Cyc_List_List*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));
_tmp1C8->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp1C9=_cycalloc(
sizeof(*_tmp1C9));_tmp1C9[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp1CA;_tmp1CA.tag=
21;_tmp1CA.f1=(void*)t;_tmp1CA;});_tmp1C9;}));_tmp1C8->tl=0;_tmp1C8;});_tmp1C7;});
_tmp1C6;});_tmp1C5;});*_tmp1B7=({struct Cyc_Core_Opt*_tmp1C4=_cycalloc(sizeof(*
_tmp1C4));_tmp1C4->v=(void*)((void*)_tmp1C3);_tmp1C4;});return 1;}}_LL174:;_LL175:
return 0;_LL16B:;}}static int Cyc_Tcutil_variable_in_effect(int constrain,struct Cyc_Absyn_Tvar*
v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1CB=e;struct Cyc_Absyn_Tvar*_tmp1CC;
struct Cyc_List_List*_tmp1CD;void*_tmp1CE;struct Cyc_Core_Opt*_tmp1CF;struct Cyc_Core_Opt*
_tmp1D0;struct Cyc_Core_Opt**_tmp1D1;struct Cyc_Core_Opt*_tmp1D2;_LL181: if(_tmp1CB
<= (void*)3?1:*((int*)_tmp1CB)!= 1)goto _LL183;_tmp1CC=((struct Cyc_Absyn_VarType_struct*)
_tmp1CB)->f1;_LL182: return Cyc_Absyn_tvar_cmp(v,_tmp1CC)== 0;_LL183: if(_tmp1CB <= (
void*)3?1:*((int*)_tmp1CB)!= 20)goto _LL185;_tmp1CD=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1CB)->f1;_LL184: for(0;_tmp1CD != 0;_tmp1CD=_tmp1CD->tl){if(Cyc_Tcutil_variable_in_effect(
constrain,v,(void*)_tmp1CD->hd))return 1;}return 0;_LL185: if(_tmp1CB <= (void*)3?1:*((
int*)_tmp1CB)!= 21)goto _LL187;_tmp1CE=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1CB)->f1;_LL186: {void*_tmp1D3=Cyc_Tcutil_rgns_of(_tmp1CE);void*_tmp1D4;
_LL18C: if(_tmp1D3 <= (void*)3?1:*((int*)_tmp1D3)!= 21)goto _LL18E;_tmp1D4=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1D3)->f1;_LL18D: if(!constrain)return 0;{void*
_tmp1D5=Cyc_Tcutil_compress(_tmp1D4);struct Cyc_Core_Opt*_tmp1D6;struct Cyc_Core_Opt*
_tmp1D7;struct Cyc_Core_Opt**_tmp1D8;struct Cyc_Core_Opt*_tmp1D9;_LL191: if(_tmp1D5
<= (void*)3?1:*((int*)_tmp1D5)!= 0)goto _LL193;_tmp1D6=((struct Cyc_Absyn_Evar_struct*)
_tmp1D5)->f1;_tmp1D7=((struct Cyc_Absyn_Evar_struct*)_tmp1D5)->f2;_tmp1D8=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1D5)->f2;_tmp1D9=((struct Cyc_Absyn_Evar_struct*)
_tmp1D5)->f4;_LL192: {void*_tmp1DA=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1D9);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D9))->v,v))return 0;{
void*_tmp1DB=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1DE;_tmp1DE.tag=20;_tmp1DE.f1=({void*_tmp1DF[2];_tmp1DF[1]=(void*)({struct
Cyc_Absyn_VarType_struct*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0[0]=({struct
Cyc_Absyn_VarType_struct _tmp1E1;_tmp1E1.tag=1;_tmp1E1.f1=v;_tmp1E1;});_tmp1E0;});
_tmp1DF[0]=_tmp1DA;Cyc_List_list(_tag_arr(_tmp1DF,sizeof(void*),2));});_tmp1DE;});
_tmp1DD;}));*_tmp1D8=({struct Cyc_Core_Opt*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));
_tmp1DC->v=(void*)_tmp1DB;_tmp1DC;});return 1;}}_LL193:;_LL194: return 0;_LL190:;}
_LL18E:;_LL18F: return Cyc_Tcutil_variable_in_effect(constrain,v,_tmp1D3);_LL18B:;}
_LL187: if(_tmp1CB <= (void*)3?1:*((int*)_tmp1CB)!= 0)goto _LL189;_tmp1CF=((struct
Cyc_Absyn_Evar_struct*)_tmp1CB)->f1;_tmp1D0=((struct Cyc_Absyn_Evar_struct*)
_tmp1CB)->f2;_tmp1D1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1CB)->f2;_tmp1D2=((struct Cyc_Absyn_Evar_struct*)_tmp1CB)->f4;_LL188: if(
_tmp1CF == 0?1:(void*)_tmp1CF->v != (void*)4)({void*_tmp1E2[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",
sizeof(char),27),_tag_arr(_tmp1E2,sizeof(void*),0));});{void*_tmp1E3=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1D2);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1D2))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*_tmp1E4=({struct Cyc_Absyn_JoinEff_struct*
_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E7;_tmp1E7.tag=20;_tmp1E7.f1=({struct Cyc_List_List*_tmp1E8=_cycalloc(
sizeof(*_tmp1E8));_tmp1E8->hd=(void*)_tmp1E3;_tmp1E8->tl=({struct Cyc_List_List*
_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=({struct Cyc_Absyn_VarType_struct
_tmp1EB;_tmp1EB.tag=1;_tmp1EB.f1=v;_tmp1EB;});_tmp1EA;}));_tmp1E9->tl=0;_tmp1E9;});
_tmp1E8;});_tmp1E7;});_tmp1E6;});*_tmp1D1=({struct Cyc_Core_Opt*_tmp1E5=_cycalloc(
sizeof(*_tmp1E5));_tmp1E5->v=(void*)((void*)_tmp1E4);_tmp1E5;});return 1;}}_LL189:;
_LL18A: return 0;_LL180:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=
Cyc_Tcutil_compress(e);{void*_tmp1EC=e;struct Cyc_List_List*_tmp1ED;void*_tmp1EE;
_LL196: if(_tmp1EC <= (void*)3?1:*((int*)_tmp1EC)!= 20)goto _LL198;_tmp1ED=((struct
Cyc_Absyn_JoinEff_struct*)_tmp1EC)->f1;_LL197: for(0;_tmp1ED != 0;_tmp1ED=_tmp1ED->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp1ED->hd))return 1;}return 0;_LL198:
if(_tmp1EC <= (void*)3?1:*((int*)_tmp1EC)!= 21)goto _LL19A;_tmp1EE=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp1EC)->f1;_LL199: {void*_tmp1EF=Cyc_Tcutil_rgns_of(
_tmp1EE);void*_tmp1F0;_LL19F: if(_tmp1EF <= (void*)3?1:*((int*)_tmp1EF)!= 21)goto
_LL1A1;_tmp1F0=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1EF)->f1;_LL1A0:
return 0;_LL1A1:;_LL1A2: return Cyc_Tcutil_evar_in_effect(evar,_tmp1EF);_LL19E:;}
_LL19A: if(_tmp1EC <= (void*)3?1:*((int*)_tmp1EC)!= 0)goto _LL19C;_LL19B: return evar
== e;_LL19C:;_LL19D: return 0;_LL195:;}}int Cyc_Tcutil_subset_effect(int
set_to_empty,void*e1,void*e2){void*_tmp1F1=Cyc_Tcutil_compress(e1);struct Cyc_List_List*
_tmp1F2;void*_tmp1F3;struct Cyc_Absyn_Tvar*_tmp1F4;void*_tmp1F5;struct Cyc_Core_Opt*
_tmp1F6;struct Cyc_Core_Opt**_tmp1F7;struct Cyc_Core_Opt*_tmp1F8;_LL1A4: if(_tmp1F1
<= (void*)3?1:*((int*)_tmp1F1)!= 20)goto _LL1A6;_tmp1F2=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1F1)->f1;_LL1A5: for(0;_tmp1F2 != 0;_tmp1F2=_tmp1F2->tl){if(!Cyc_Tcutil_subset_effect(
set_to_empty,(void*)_tmp1F2->hd,e2))return 0;}return 1;_LL1A6: if(_tmp1F1 <= (void*)
3?1:*((int*)_tmp1F1)!= 19)goto _LL1A8;_tmp1F3=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1F1)->f1;_LL1A7: return(Cyc_Tcutil_region_in_effect(0,_tmp1F3,e2)?1: Cyc_Tcutil_region_in_effect(
1,_tmp1F3,e2))?1: Cyc_Tcutil_unify(_tmp1F3,(void*)2);_LL1A8: if(_tmp1F1 <= (void*)3?
1:*((int*)_tmp1F1)!= 1)goto _LL1AA;_tmp1F4=((struct Cyc_Absyn_VarType_struct*)
_tmp1F1)->f1;_LL1A9: return Cyc_Tcutil_variable_in_effect(0,_tmp1F4,e2)?1: Cyc_Tcutil_variable_in_effect(
1,_tmp1F4,e2);_LL1AA: if(_tmp1F1 <= (void*)3?1:*((int*)_tmp1F1)!= 21)goto _LL1AC;
_tmp1F5=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1F1)->f1;_LL1AB: {void*
_tmp1F9=Cyc_Tcutil_rgns_of(_tmp1F5);void*_tmp1FA;_LL1B1: if(_tmp1F9 <= (void*)3?1:*((
int*)_tmp1F9)!= 21)goto _LL1B3;_tmp1FA=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1F9)->f1;_LL1B2: return(Cyc_Tcutil_type_in_effect(0,_tmp1FA,e2)?1: Cyc_Tcutil_type_in_effect(
1,_tmp1FA,e2))?1: Cyc_Tcutil_unify(_tmp1FA,Cyc_Absyn_sint_typ);_LL1B3:;_LL1B4:
return Cyc_Tcutil_subset_effect(set_to_empty,_tmp1F9,e2);_LL1B0:;}_LL1AC: if(
_tmp1F1 <= (void*)3?1:*((int*)_tmp1F1)!= 0)goto _LL1AE;_tmp1F6=((struct Cyc_Absyn_Evar_struct*)
_tmp1F1)->f2;_tmp1F7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1F1)->f2;_tmp1F8=((struct Cyc_Absyn_Evar_struct*)_tmp1F1)->f4;_LL1AD: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){if(set_to_empty)*_tmp1F7=({struct Cyc_Core_Opt*_tmp1FB=_cycalloc(sizeof(*
_tmp1FB));_tmp1FB->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1FC=
_cycalloc(sizeof(*_tmp1FC));_tmp1FC[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1FD;
_tmp1FD.tag=20;_tmp1FD.f1=0;_tmp1FD;});_tmp1FC;}));_tmp1FB;});else{Cyc_Tcutil_occurs(
e1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1F8))->v,e2);*_tmp1F7=({struct Cyc_Core_Opt*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));
_tmp1FE->v=(void*)e2;_tmp1FE;});}}return 1;_LL1AE:;_LL1AF:({struct Cyc_Std_String_pa_struct
_tmp200;_tmp200.tag=0;_tmp200.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e1);{
void*_tmp1FF[1]={& _tmp200};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("subset_effect: bad effect: %s",sizeof(char),30),
_tag_arr(_tmp1FF,sizeof(void*),1));}});_LL1A3:;}struct _tuple11{struct _tuple10*f1;
struct _tuple10*f2;};static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);{struct _tuple11 _tmp202=({struct _tuple11
_tmp201;_tmp201.f1=Cyc_Tcutil_get_effect_evar(e1);_tmp201.f2=Cyc_Tcutil_get_effect_evar(
e2);_tmp201;});_LL1B6:;_LL1B7: return Cyc_Tcutil_subset_effect(0,e1,e2)?Cyc_Tcutil_subset_effect(
0,e2,e1): 0;_LL1B5:;}}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple6 _tmp204;void*_tmp205;void*_tmp206;struct _tuple6*_tmp203=(struct
_tuple6*)r1->hd;_tmp204=*_tmp203;_tmp205=_tmp204.f1;_tmp206=_tmp204.f2;{int found=
_tmp205 == (void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0?!found: 0;r2=r2->tl){
struct _tuple6 _tmp208;void*_tmp209;void*_tmp20A;struct _tuple6*_tmp207=(struct
_tuple6*)r2->hd;_tmp208=*_tmp207;_tmp209=_tmp208.f1;_tmp20A=_tmp208.f2;if(Cyc_Tcutil_unify(
_tmp205,_tmp209)?Cyc_Tcutil_unify(_tmp206,_tmp20A): 0){found=1;break;}}}if(!found)
return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)?Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1): 0;}struct _tuple12{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple13{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _tagged_arr)_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp20B=t1;struct Cyc_Core_Opt*_tmp20C;struct Cyc_Core_Opt*
_tmp20D;struct Cyc_Core_Opt**_tmp20E;struct Cyc_Core_Opt*_tmp20F;_LL1B9: if(_tmp20B
<= (void*)3?1:*((int*)_tmp20B)!= 0)goto _LL1BB;_tmp20C=((struct Cyc_Absyn_Evar_struct*)
_tmp20B)->f1;_tmp20D=((struct Cyc_Absyn_Evar_struct*)_tmp20B)->f2;_tmp20E=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp20B)->f2;_tmp20F=((struct Cyc_Absyn_Evar_struct*)
_tmp20B)->f4;_LL1BA: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp20F))->v,t2);{void*_tmp210=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp210,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp20C))->v)){*_tmp20E=({struct Cyc_Core_Opt*_tmp211=_cycalloc(sizeof(*_tmp211));
_tmp211->v=(void*)t2;_tmp211;});return;}else{{void*_tmp212=t2;struct Cyc_Core_Opt*
_tmp213;struct Cyc_Core_Opt**_tmp214;struct Cyc_Core_Opt*_tmp215;struct Cyc_Absyn_PtrInfo
_tmp216;_LL1BE: if(_tmp212 <= (void*)3?1:*((int*)_tmp212)!= 0)goto _LL1C0;_tmp213=((
struct Cyc_Absyn_Evar_struct*)_tmp212)->f2;_tmp214=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp212)->f2;_tmp215=((struct Cyc_Absyn_Evar_struct*)
_tmp212)->f4;_LL1BF: {struct Cyc_List_List*_tmp217=(struct Cyc_List_List*)_tmp20F->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp215))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp217,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=
_tag_arr("(type variable would escape scope)",sizeof(char),35);(int)_throw((void*)
Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)_tmp20C->v,_tmp210)){*_tmp214=({
struct Cyc_Core_Opt*_tmp218=_cycalloc(sizeof(*_tmp218));_tmp218->v=(void*)t1;
_tmp218;});return;}Cyc_Tcutil_failure_reason=_tag_arr("(kinds are incompatible)",
sizeof(char),25);goto _LL1BD;}_LL1C0: if(_tmp212 <= (void*)3?1:*((int*)_tmp212)!= 4)
goto _LL1C2;_tmp216=((struct Cyc_Absyn_PointerType_struct*)_tmp212)->f1;if(!((void*)
_tmp20C->v == (void*)2))goto _LL1C2;_LL1C1: {struct Cyc_Absyn_Conref*_tmp219=Cyc_Absyn_compress_conref(
_tmp216.bounds);{void*_tmp21A=(void*)_tmp219->v;_LL1C5: if((int)_tmp21A != 0)goto
_LL1C7;_LL1C6:(void*)(_tmp219->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp21C;_tmp21C.tag=0;_tmp21C.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp21E;_tmp21E.tag=0;_tmp21E.f1=Cyc_Absyn_signed_int_exp(1,0);_tmp21E;});
_tmp21D;}));_tmp21C;});_tmp21B;})));*_tmp20E=({struct Cyc_Core_Opt*_tmp21F=
_cycalloc(sizeof(*_tmp21F));_tmp21F->v=(void*)t2;_tmp21F;});return;_LL1C7:;
_LL1C8: goto _LL1C4;_LL1C4:;}goto _LL1BD;}_LL1C2:;_LL1C3: goto _LL1BD;_LL1BD:;}Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}
_LL1BB:;_LL1BC: goto _LL1B8;_LL1B8:;}{struct _tuple6 _tmp221=({struct _tuple6 _tmp220;
_tmp220.f1=t2;_tmp220.f2=t1;_tmp220;});void*_tmp222;void*_tmp223;void*_tmp224;
void*_tmp225;struct Cyc_Absyn_Tvar*_tmp226;void*_tmp227;struct Cyc_Absyn_Tvar*
_tmp228;void*_tmp229;struct Cyc_Absyn_AggrInfo _tmp22A;void*_tmp22B;struct Cyc_List_List*
_tmp22C;void*_tmp22D;struct Cyc_Absyn_AggrInfo _tmp22E;void*_tmp22F;struct Cyc_List_List*
_tmp230;void*_tmp231;struct _tuple1*_tmp232;void*_tmp233;struct _tuple1*_tmp234;
void*_tmp235;struct Cyc_List_List*_tmp236;void*_tmp237;struct Cyc_List_List*
_tmp238;void*_tmp239;struct Cyc_Absyn_TunionInfo _tmp23A;void*_tmp23B;struct Cyc_Absyn_Tuniondecl**
_tmp23C;struct Cyc_Absyn_Tuniondecl*_tmp23D;struct Cyc_List_List*_tmp23E;void*
_tmp23F;void*_tmp240;struct Cyc_Absyn_TunionInfo _tmp241;void*_tmp242;struct Cyc_Absyn_Tuniondecl**
_tmp243;struct Cyc_Absyn_Tuniondecl*_tmp244;struct Cyc_List_List*_tmp245;void*
_tmp246;void*_tmp247;struct Cyc_Absyn_TunionFieldInfo _tmp248;void*_tmp249;struct
Cyc_Absyn_Tuniondecl*_tmp24A;struct Cyc_Absyn_Tunionfield*_tmp24B;struct Cyc_List_List*
_tmp24C;void*_tmp24D;struct Cyc_Absyn_TunionFieldInfo _tmp24E;void*_tmp24F;struct
Cyc_Absyn_Tuniondecl*_tmp250;struct Cyc_Absyn_Tunionfield*_tmp251;struct Cyc_List_List*
_tmp252;void*_tmp253;struct Cyc_Absyn_PtrInfo _tmp254;void*_tmp255;void*_tmp256;
struct Cyc_Absyn_Conref*_tmp257;struct Cyc_Absyn_Tqual _tmp258;struct Cyc_Absyn_Conref*
_tmp259;void*_tmp25A;struct Cyc_Absyn_PtrInfo _tmp25B;void*_tmp25C;void*_tmp25D;
struct Cyc_Absyn_Conref*_tmp25E;struct Cyc_Absyn_Tqual _tmp25F;struct Cyc_Absyn_Conref*
_tmp260;void*_tmp261;void*_tmp262;void*_tmp263;void*_tmp264;void*_tmp265;void*
_tmp266;void*_tmp267;void*_tmp268;void*_tmp269;int _tmp26A;void*_tmp26B;int
_tmp26C;void*_tmp26D;void*_tmp26E;void*_tmp26F;void*_tmp270;void*_tmp271;int
_tmp272;void*_tmp273;int _tmp274;void*_tmp275;void*_tmp276;void*_tmp277;void*
_tmp278;void*_tmp279;void*_tmp27A;struct Cyc_Absyn_Tqual _tmp27B;struct Cyc_Absyn_Exp*
_tmp27C;void*_tmp27D;void*_tmp27E;struct Cyc_Absyn_Tqual _tmp27F;struct Cyc_Absyn_Exp*
_tmp280;void*_tmp281;struct Cyc_Absyn_FnInfo _tmp282;struct Cyc_List_List*_tmp283;
struct Cyc_Core_Opt*_tmp284;void*_tmp285;struct Cyc_List_List*_tmp286;int _tmp287;
struct Cyc_Absyn_VarargInfo*_tmp288;struct Cyc_List_List*_tmp289;struct Cyc_List_List*
_tmp28A;void*_tmp28B;struct Cyc_Absyn_FnInfo _tmp28C;struct Cyc_List_List*_tmp28D;
struct Cyc_Core_Opt*_tmp28E;void*_tmp28F;struct Cyc_List_List*_tmp290;int _tmp291;
struct Cyc_Absyn_VarargInfo*_tmp292;struct Cyc_List_List*_tmp293;struct Cyc_List_List*
_tmp294;void*_tmp295;struct Cyc_List_List*_tmp296;void*_tmp297;struct Cyc_List_List*
_tmp298;void*_tmp299;void*_tmp29A;struct Cyc_List_List*_tmp29B;void*_tmp29C;void*
_tmp29D;struct Cyc_List_List*_tmp29E;void*_tmp29F;void*_tmp2A0;void*_tmp2A1;void*
_tmp2A2;void*_tmp2A3;void*_tmp2A4;void*_tmp2A5;void*_tmp2A6;void*_tmp2A7;void*
_tmp2A8;void*_tmp2A9;void*_tmp2AA;_LL1CA: _tmp222=_tmp221.f1;if(_tmp222 <= (void*)
3?1:*((int*)_tmp222)!= 0)goto _LL1CC;_LL1CB: Cyc_Tcutil_unify_it(t2,t1);return;
_LL1CC: _tmp223=_tmp221.f1;if((int)_tmp223 != 0)goto _LL1CE;_tmp224=_tmp221.f2;if((
int)_tmp224 != 0)goto _LL1CE;_LL1CD: return;_LL1CE: _tmp225=_tmp221.f1;if(_tmp225 <= (
void*)3?1:*((int*)_tmp225)!= 1)goto _LL1D0;_tmp226=((struct Cyc_Absyn_VarType_struct*)
_tmp225)->f1;_tmp227=_tmp221.f2;if(_tmp227 <= (void*)3?1:*((int*)_tmp227)!= 1)
goto _LL1D0;_tmp228=((struct Cyc_Absyn_VarType_struct*)_tmp227)->f1;_LL1CF: {
struct _tagged_arr*_tmp2AB=_tmp226->name;struct _tagged_arr*_tmp2AC=_tmp228->name;
int _tmp2AD=*((int*)_check_null(_tmp226->identity));int _tmp2AE=*((int*)
_check_null(_tmp228->identity));void*_tmp2AF=Cyc_Tcutil_tvar_kind(_tmp226);void*
_tmp2B0=Cyc_Tcutil_tvar_kind(_tmp228);if(_tmp2AE == _tmp2AD?Cyc_Std_zstrptrcmp(
_tmp2AB,_tmp2AC)== 0: 0){if(_tmp2AF != _tmp2B0)({struct Cyc_Std_String_pa_struct
_tmp2B4;_tmp2B4.tag=0;_tmp2B4.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2B0);{struct Cyc_Std_String_pa_struct _tmp2B3;_tmp2B3.tag=0;_tmp2B3.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2AF);{struct Cyc_Std_String_pa_struct
_tmp2B2;_tmp2B2.tag=0;_tmp2B2.f1=(struct _tagged_arr)*_tmp2AB;{void*_tmp2B1[3]={&
_tmp2B2,& _tmp2B3,& _tmp2B4};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("same type variable %s has kinds %s and %s",sizeof(
char),42),_tag_arr(_tmp2B1,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=
_tag_arr("(variable types are not the same)",sizeof(char),34);goto _LL1C9;}_LL1D0:
_tmp229=_tmp221.f1;if(_tmp229 <= (void*)3?1:*((int*)_tmp229)!= 10)goto _LL1D2;
_tmp22A=((struct Cyc_Absyn_AggrType_struct*)_tmp229)->f1;_tmp22B=(void*)_tmp22A.aggr_info;
_tmp22C=_tmp22A.targs;_tmp22D=_tmp221.f2;if(_tmp22D <= (void*)3?1:*((int*)_tmp22D)
!= 10)goto _LL1D2;_tmp22E=((struct Cyc_Absyn_AggrType_struct*)_tmp22D)->f1;_tmp22F=(
void*)_tmp22E.aggr_info;_tmp230=_tmp22E.targs;_LL1D1: {void*_tmp2B6;struct
_tuple1*_tmp2B7;struct _tuple5 _tmp2B5=Cyc_Absyn_aggr_kinded_name(_tmp22F);_tmp2B6=
_tmp2B5.f1;_tmp2B7=_tmp2B5.f2;{void*_tmp2B9;struct _tuple1*_tmp2BA;struct _tuple5
_tmp2B8=Cyc_Absyn_aggr_kinded_name(_tmp22B);_tmp2B9=_tmp2B8.f1;_tmp2BA=_tmp2B8.f2;
if(_tmp2B6 != _tmp2B9){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1C9;}if(Cyc_Absyn_qvar_cmp(_tmp2B7,_tmp2BA)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different type name)",sizeof(char),22);goto _LL1C9;}Cyc_Tcutil_unify_list(
_tmp230,_tmp22C);return;}}_LL1D2: _tmp231=_tmp221.f1;if(_tmp231 <= (void*)3?1:*((
int*)_tmp231)!= 12)goto _LL1D4;_tmp232=((struct Cyc_Absyn_EnumType_struct*)_tmp231)->f1;
_tmp233=_tmp221.f2;if(_tmp233 <= (void*)3?1:*((int*)_tmp233)!= 12)goto _LL1D4;
_tmp234=((struct Cyc_Absyn_EnumType_struct*)_tmp233)->f1;_LL1D3: if(Cyc_Absyn_qvar_cmp(
_tmp232,_tmp234)== 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different enum types)",
sizeof(char),23);goto _LL1C9;_LL1D4: _tmp235=_tmp221.f1;if(_tmp235 <= (void*)3?1:*((
int*)_tmp235)!= 13)goto _LL1D6;_tmp236=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp235)->f1;_tmp237=_tmp221.f2;if(_tmp237 <= (void*)3?1:*((int*)_tmp237)!= 13)
goto _LL1D6;_tmp238=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp237)->f1;_LL1D5: {
int bad=0;for(0;_tmp236 != 0?_tmp238 != 0: 0;(_tmp236=_tmp236->tl,_tmp238=_tmp238->tl)){
struct Cyc_Absyn_Enumfield*_tmp2BB=(struct Cyc_Absyn_Enumfield*)_tmp236->hd;struct
Cyc_Absyn_Enumfield*_tmp2BC=(struct Cyc_Absyn_Enumfield*)_tmp238->hd;if(Cyc_Absyn_qvar_cmp(
_tmp2BB->name,_tmp2BC->name)!= 0){Cyc_Tcutil_failure_reason=_tag_arr("(different names for enum fields)",
sizeof(char),34);bad=1;break;}if(_tmp2BB->tag == _tmp2BC->tag)continue;if(_tmp2BB->tag
== 0?1: _tmp2BC->tag == 0){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(char),39);bad=1;break;}if(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp2BB->tag))!= Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp2BC->tag))){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(char),39);bad=1;break;}}if(bad)goto _LL1C9;if(_tmp236 == 0?_tmp238 == 0: 0)
return;Cyc_Tcutil_failure_reason=_tag_arr("(different number of fields for enums)",
sizeof(char),39);goto _LL1C9;}_LL1D6: _tmp239=_tmp221.f1;if(_tmp239 <= (void*)3?1:*((
int*)_tmp239)!= 2)goto _LL1D8;_tmp23A=((struct Cyc_Absyn_TunionType_struct*)
_tmp239)->f1;_tmp23B=(void*)_tmp23A.tunion_info;if(*((int*)_tmp23B)!= 1)goto
_LL1D8;_tmp23C=((struct Cyc_Absyn_KnownTunion_struct*)_tmp23B)->f1;_tmp23D=*
_tmp23C;_tmp23E=_tmp23A.targs;_tmp23F=(void*)_tmp23A.rgn;_tmp240=_tmp221.f2;if(
_tmp240 <= (void*)3?1:*((int*)_tmp240)!= 2)goto _LL1D8;_tmp241=((struct Cyc_Absyn_TunionType_struct*)
_tmp240)->f1;_tmp242=(void*)_tmp241.tunion_info;if(*((int*)_tmp242)!= 1)goto
_LL1D8;_tmp243=((struct Cyc_Absyn_KnownTunion_struct*)_tmp242)->f1;_tmp244=*
_tmp243;_tmp245=_tmp241.targs;_tmp246=(void*)_tmp241.rgn;_LL1D7: if(_tmp23D == 
_tmp244?1: Cyc_Absyn_qvar_cmp(_tmp23D->name,_tmp244->name)== 0){Cyc_Tcutil_unify_it(
_tmp246,_tmp23F);Cyc_Tcutil_unify_list(_tmp245,_tmp23E);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion types)",sizeof(char),25);goto _LL1C9;_LL1D8: _tmp247=
_tmp221.f1;if(_tmp247 <= (void*)3?1:*((int*)_tmp247)!= 3)goto _LL1DA;_tmp248=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp247)->f1;_tmp249=(void*)_tmp248.field_info;
if(*((int*)_tmp249)!= 1)goto _LL1DA;_tmp24A=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp249)->f1;_tmp24B=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp249)->f2;
_tmp24C=_tmp248.targs;_tmp24D=_tmp221.f2;if(_tmp24D <= (void*)3?1:*((int*)_tmp24D)
!= 3)goto _LL1DA;_tmp24E=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp24D)->f1;
_tmp24F=(void*)_tmp24E.field_info;if(*((int*)_tmp24F)!= 1)goto _LL1DA;_tmp250=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp24F)->f1;_tmp251=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp24F)->f2;_tmp252=_tmp24E.targs;_LL1D9: if((_tmp24A == _tmp250?1: Cyc_Absyn_qvar_cmp(
_tmp24A->name,_tmp250->name)== 0)?_tmp24B == _tmp251?1: Cyc_Absyn_qvar_cmp(_tmp24B->name,
_tmp251->name)== 0: 0){Cyc_Tcutil_unify_list(_tmp252,_tmp24C);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)",sizeof(char),31);goto _LL1C9;_LL1DA:
_tmp253=_tmp221.f1;if(_tmp253 <= (void*)3?1:*((int*)_tmp253)!= 4)goto _LL1DC;
_tmp254=((struct Cyc_Absyn_PointerType_struct*)_tmp253)->f1;_tmp255=(void*)
_tmp254.elt_typ;_tmp256=(void*)_tmp254.rgn_typ;_tmp257=_tmp254.nullable;_tmp258=
_tmp254.tq;_tmp259=_tmp254.bounds;_tmp25A=_tmp221.f2;if(_tmp25A <= (void*)3?1:*((
int*)_tmp25A)!= 4)goto _LL1DC;_tmp25B=((struct Cyc_Absyn_PointerType_struct*)
_tmp25A)->f1;_tmp25C=(void*)_tmp25B.elt_typ;_tmp25D=(void*)_tmp25B.rgn_typ;
_tmp25E=_tmp25B.nullable;_tmp25F=_tmp25B.tq;_tmp260=_tmp25B.bounds;_LL1DB: Cyc_Tcutil_unify_it(
_tmp25C,_tmp255);Cyc_Tcutil_unify_it(_tmp256,_tmp25D);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_unify_tqual(_tmp25F,_tmp258);Cyc_Tcutil_unify_it_conrefs(
Cyc_Tcutil_boundscmp,_tmp260,_tmp259,_tag_arr("(different pointer bounds)",
sizeof(char),27));{void*_tmp2BD=(void*)(Cyc_Absyn_compress_conref(_tmp260))->v;
void*_tmp2BE;_LL203: if(_tmp2BD <= (void*)1?1:*((int*)_tmp2BD)!= 0)goto _LL205;
_tmp2BE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2BD)->f1;if((int)_tmp2BE
!= 0)goto _LL205;_LL204: return;_LL205:;_LL206: goto _LL202;_LL202:;}((void(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp25E,_tmp257,
_tag_arr("(different pointer types)",sizeof(char),26));return;_LL1DC: _tmp261=
_tmp221.f1;if(_tmp261 <= (void*)3?1:*((int*)_tmp261)!= 5)goto _LL1DE;_tmp262=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp261)->f1;_tmp263=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp261)->f2;_tmp264=_tmp221.f2;if(_tmp264 <= (void*)3?1:*((int*)_tmp264)!= 5)
goto _LL1DE;_tmp265=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp264)->f1;_tmp266=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp264)->f2;_LL1DD: if(_tmp265 == _tmp262?
_tmp266 == _tmp263: 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different integral types)",
sizeof(char),27);goto _LL1C9;_LL1DE: _tmp267=_tmp221.f1;if((int)_tmp267 != 1)goto
_LL1E0;_tmp268=_tmp221.f2;if((int)_tmp268 != 1)goto _LL1E0;_LL1DF: return;_LL1E0:
_tmp269=_tmp221.f1;if(_tmp269 <= (void*)3?1:*((int*)_tmp269)!= 6)goto _LL1E2;
_tmp26A=((struct Cyc_Absyn_DoubleType_struct*)_tmp269)->f1;_tmp26B=_tmp221.f2;if(
_tmp26B <= (void*)3?1:*((int*)_tmp26B)!= 6)goto _LL1E2;_tmp26C=((struct Cyc_Absyn_DoubleType_struct*)
_tmp26B)->f1;_LL1E1: if(_tmp26A == _tmp26C)return;goto _LL1C9;_LL1E2: _tmp26D=
_tmp221.f1;if(_tmp26D <= (void*)3?1:*((int*)_tmp26D)!= 14)goto _LL1E4;_tmp26E=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp26D)->f1;_tmp26F=_tmp221.f2;if(
_tmp26F <= (void*)3?1:*((int*)_tmp26F)!= 14)goto _LL1E4;_tmp270=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp26F)->f1;_LL1E3: Cyc_Tcutil_unify_it(_tmp26E,_tmp270);return;_LL1E4: _tmp271=
_tmp221.f1;if(_tmp271 <= (void*)3?1:*((int*)_tmp271)!= 18)goto _LL1E6;_tmp272=((
struct Cyc_Absyn_TypeInt_struct*)_tmp271)->f1;_tmp273=_tmp221.f2;if(_tmp273 <= (
void*)3?1:*((int*)_tmp273)!= 18)goto _LL1E6;_tmp274=((struct Cyc_Absyn_TypeInt_struct*)
_tmp273)->f1;_LL1E5: if(_tmp272 == _tmp274)return;Cyc_Tcutil_failure_reason=
_tag_arr("(different type integers)",sizeof(char),26);goto _LL1C9;_LL1E6: _tmp275=
_tmp221.f1;if(_tmp275 <= (void*)3?1:*((int*)_tmp275)!= 17)goto _LL1E8;_tmp276=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp275)->f1;_tmp277=_tmp221.f2;if(
_tmp277 <= (void*)3?1:*((int*)_tmp277)!= 17)goto _LL1E8;_tmp278=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp277)->f1;_LL1E7: Cyc_Tcutil_unify_it(_tmp276,_tmp278);return;_LL1E8: _tmp279=
_tmp221.f1;if(_tmp279 <= (void*)3?1:*((int*)_tmp279)!= 7)goto _LL1EA;_tmp27A=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp279)->f1;_tmp27B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp279)->f2;_tmp27C=((struct Cyc_Absyn_ArrayType_struct*)_tmp279)->f3;_tmp27D=
_tmp221.f2;if(_tmp27D <= (void*)3?1:*((int*)_tmp27D)!= 7)goto _LL1EA;_tmp27E=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp27D)->f1;_tmp27F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp27D)->f2;_tmp280=((struct Cyc_Absyn_ArrayType_struct*)_tmp27D)->f3;_LL1E9: Cyc_Tcutil_unify_tqual(
_tmp27F,_tmp27B);Cyc_Tcutil_unify_it(_tmp27E,_tmp27A);if(_tmp27C == _tmp280)
return;if(_tmp27C == 0?1: _tmp280 == 0)goto _LL1C9;if(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp27C))== Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp280)))return;Cyc_Tcutil_failure_reason=
_tag_arr("(different array sizes)",sizeof(char),24);goto _LL1C9;_LL1EA: _tmp281=
_tmp221.f1;if(_tmp281 <= (void*)3?1:*((int*)_tmp281)!= 8)goto _LL1EC;_tmp282=((
struct Cyc_Absyn_FnType_struct*)_tmp281)->f1;_tmp283=_tmp282.tvars;_tmp284=
_tmp282.effect;_tmp285=(void*)_tmp282.ret_typ;_tmp286=_tmp282.args;_tmp287=
_tmp282.c_varargs;_tmp288=_tmp282.cyc_varargs;_tmp289=_tmp282.rgn_po;_tmp28A=
_tmp282.attributes;_tmp28B=_tmp221.f2;if(_tmp28B <= (void*)3?1:*((int*)_tmp28B)!= 
8)goto _LL1EC;_tmp28C=((struct Cyc_Absyn_FnType_struct*)_tmp28B)->f1;_tmp28D=
_tmp28C.tvars;_tmp28E=_tmp28C.effect;_tmp28F=(void*)_tmp28C.ret_typ;_tmp290=
_tmp28C.args;_tmp291=_tmp28C.c_varargs;_tmp292=_tmp28C.cyc_varargs;_tmp293=
_tmp28C.rgn_po;_tmp294=_tmp28C.attributes;_LL1EB: {int done=0;{struct
_RegionHandle _tmp2BF=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2BF;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp28D != 0){if(_tmp283 == 0){
Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too few type variables)",
sizeof(char),50);(int)_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp2C0=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp28D->hd);void*_tmp2C1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp283->hd);if(_tmp2C0 != _tmp2C1){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp2C5;_tmp2C5.tag=0;_tmp2C5.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2C1);{struct Cyc_Std_String_pa_struct
_tmp2C4;_tmp2C4.tag=0;_tmp2C4.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2C0);{struct Cyc_Std_String_pa_struct _tmp2C3;_tmp2C3.tag=0;_tmp2C3.f1=(struct
_tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp28D->hd);{void*
_tmp2C2[3]={& _tmp2C3,& _tmp2C4,& _tmp2C5};Cyc_Std_aprintf(_tag_arr("(type var %s has different kinds %s and %s)",
sizeof(char),44),_tag_arr(_tmp2C2,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp2C6=_region_malloc(rgn,sizeof(*_tmp2C6));_tmp2C6->hd=({
struct _tuple7*_tmp2C7=_region_malloc(rgn,sizeof(*_tmp2C7));_tmp2C7->f1=(struct
Cyc_Absyn_Tvar*)_tmp283->hd;_tmp2C7->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8[0]=({struct Cyc_Absyn_VarType_struct
_tmp2C9;_tmp2C9.tag=1;_tmp2C9.f1=(struct Cyc_Absyn_Tvar*)_tmp28D->hd;_tmp2C9;});
_tmp2C8;});_tmp2C7;});_tmp2C6->tl=inst;_tmp2C6;});_tmp28D=_tmp28D->tl;_tmp283=
_tmp283->tl;}}if(_tmp283 != 0){Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too many type variables)",
sizeof(char),51);_npop_handler(0);goto _LL1C9;}if(inst != 0){Cyc_Tcutil_unify_it((
void*)({struct Cyc_Absyn_FnType_struct*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));
_tmp2CA[0]=({struct Cyc_Absyn_FnType_struct _tmp2CB;_tmp2CB.tag=8;_tmp2CB.f1=({
struct Cyc_Absyn_FnInfo _tmp2CC;_tmp2CC.tvars=0;_tmp2CC.effect=_tmp28E;_tmp2CC.ret_typ=(
void*)_tmp28F;_tmp2CC.args=_tmp290;_tmp2CC.c_varargs=_tmp291;_tmp2CC.cyc_varargs=
_tmp292;_tmp2CC.rgn_po=_tmp293;_tmp2CC.attributes=_tmp294;_tmp2CC;});_tmp2CB;});
_tmp2CA;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD[0]=({struct Cyc_Absyn_FnType_struct
_tmp2CE;_tmp2CE.tag=8;_tmp2CE.f1=({struct Cyc_Absyn_FnInfo _tmp2CF;_tmp2CF.tvars=0;
_tmp2CF.effect=_tmp284;_tmp2CF.ret_typ=(void*)_tmp285;_tmp2CF.args=_tmp286;
_tmp2CF.c_varargs=_tmp287;_tmp2CF.cyc_varargs=_tmp288;_tmp2CF.rgn_po=_tmp289;
_tmp2CF.attributes=_tmp28A;_tmp2CF;});_tmp2CE;});_tmp2CD;})));done=1;}};
_pop_region(rgn);}if(done)return;Cyc_Tcutil_unify_it(_tmp28F,_tmp285);for(0;
_tmp290 != 0?_tmp286 != 0: 0;(_tmp290=_tmp290->tl,_tmp286=_tmp286->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)_tmp290->hd)).f2,(*((struct _tuple2*)_tmp286->hd)).f2);Cyc_Tcutil_unify_it((*((
struct _tuple2*)_tmp290->hd)).f3,(*((struct _tuple2*)_tmp286->hd)).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp290 != 0?1: _tmp286 != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)",sizeof(char),52);
goto _LL1C9;}if(_tmp291 != _tmp287){Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes C varargs)",
sizeof(char),41);goto _LL1C9;}{int bad_cyc_vararg=0;{struct _tuple12 _tmp2D1=({
struct _tuple12 _tmp2D0;_tmp2D0.f1=_tmp292;_tmp2D0.f2=_tmp288;_tmp2D0;});struct Cyc_Absyn_VarargInfo*
_tmp2D2;struct Cyc_Absyn_VarargInfo*_tmp2D3;struct Cyc_Absyn_VarargInfo*_tmp2D4;
struct Cyc_Absyn_VarargInfo*_tmp2D5;struct Cyc_Absyn_VarargInfo*_tmp2D6;struct Cyc_Absyn_VarargInfo
_tmp2D7;struct Cyc_Core_Opt*_tmp2D8;struct Cyc_Absyn_Tqual _tmp2D9;void*_tmp2DA;int
_tmp2DB;struct Cyc_Absyn_VarargInfo*_tmp2DC;struct Cyc_Absyn_VarargInfo _tmp2DD;
struct Cyc_Core_Opt*_tmp2DE;struct Cyc_Absyn_Tqual _tmp2DF;void*_tmp2E0;int _tmp2E1;
_LL208: _tmp2D2=_tmp2D1.f1;if(_tmp2D2 != 0)goto _LL20A;_tmp2D3=_tmp2D1.f2;if(
_tmp2D3 != 0)goto _LL20A;_LL209: goto _LL207;_LL20A: _tmp2D4=_tmp2D1.f1;if(_tmp2D4 != 
0)goto _LL20C;_LL20B: goto _LL20D;_LL20C: _tmp2D5=_tmp2D1.f2;if(_tmp2D5 != 0)goto
_LL20E;_LL20D: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes varargs)",
sizeof(char),39);goto _LL207;_LL20E: _tmp2D6=_tmp2D1.f1;if(_tmp2D6 == 0)goto _LL207;
_tmp2D7=*_tmp2D6;_tmp2D8=_tmp2D7.name;_tmp2D9=_tmp2D7.tq;_tmp2DA=(void*)_tmp2D7.type;
_tmp2DB=_tmp2D7.inject;_tmp2DC=_tmp2D1.f2;if(_tmp2DC == 0)goto _LL207;_tmp2DD=*
_tmp2DC;_tmp2DE=_tmp2DD.name;_tmp2DF=_tmp2DD.tq;_tmp2E0=(void*)_tmp2DD.type;
_tmp2E1=_tmp2DD.inject;_LL20F: Cyc_Tcutil_unify_tqual(_tmp2D9,_tmp2DF);Cyc_Tcutil_unify_it(
_tmp2DA,_tmp2E0);if(_tmp2DB != _tmp2E1){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type injects varargs)",sizeof(char),41);}goto _LL207;
_LL207:;}if(bad_cyc_vararg)goto _LL1C9;{int bad_effect=0;{struct _tuple13 _tmp2E3=({
struct _tuple13 _tmp2E2;_tmp2E2.f1=_tmp28E;_tmp2E2.f2=_tmp284;_tmp2E2;});struct Cyc_Core_Opt*
_tmp2E4;struct Cyc_Core_Opt*_tmp2E5;struct Cyc_Core_Opt*_tmp2E6;struct Cyc_Core_Opt*
_tmp2E7;_LL211: _tmp2E4=_tmp2E3.f1;if(_tmp2E4 != 0)goto _LL213;_tmp2E5=_tmp2E3.f2;
if(_tmp2E5 != 0)goto _LL213;_LL212: goto _LL210;_LL213: _tmp2E6=_tmp2E3.f1;if(_tmp2E6
!= 0)goto _LL215;_LL214: goto _LL216;_LL215: _tmp2E7=_tmp2E3.f2;if(_tmp2E7 != 0)goto
_LL217;_LL216: bad_effect=1;goto _LL210;_LL217:;_LL218: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp28E))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp284))->v);goto _LL210;_LL210:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=_tag_arr("(function type effects do not unify)",
sizeof(char),37);goto _LL1C9;}if(!Cyc_Tcutil_same_atts(_tmp28A,_tmp294)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different attributes)",sizeof(char),43);goto
_LL1C9;}if(!Cyc_Tcutil_same_rgn_po(_tmp289,_tmp293)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different region lifetime orderings)",sizeof(char),
58);goto _LL1C9;}return;}}}_LL1EC: _tmp295=_tmp221.f1;if(_tmp295 <= (void*)3?1:*((
int*)_tmp295)!= 9)goto _LL1EE;_tmp296=((struct Cyc_Absyn_TupleType_struct*)_tmp295)->f1;
_tmp297=_tmp221.f2;if(_tmp297 <= (void*)3?1:*((int*)_tmp297)!= 9)goto _LL1EE;
_tmp298=((struct Cyc_Absyn_TupleType_struct*)_tmp297)->f1;_LL1ED: for(0;_tmp298 != 
0?_tmp296 != 0: 0;(_tmp298=_tmp298->tl,_tmp296=_tmp296->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)_tmp298->hd)).f1,(*((struct _tuple4*)_tmp296->hd)).f1);Cyc_Tcutil_unify_it((*((
struct _tuple4*)_tmp298->hd)).f2,(*((struct _tuple4*)_tmp296->hd)).f2);}if(_tmp298
== 0?_tmp296 == 0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(tuple types have different numbers of components)",sizeof(char),51);
goto _LL1C9;_LL1EE: _tmp299=_tmp221.f1;if(_tmp299 <= (void*)3?1:*((int*)_tmp299)!= 
11)goto _LL1F0;_tmp29A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp299)->f1;
_tmp29B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp299)->f2;_tmp29C=_tmp221.f2;
if(_tmp29C <= (void*)3?1:*((int*)_tmp29C)!= 11)goto _LL1F0;_tmp29D=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp29C)->f1;_tmp29E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp29C)->f2;_LL1EF: if(_tmp29D != _tmp29A){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1C9;}for(0;_tmp29E != 0?_tmp29B != 0: 0;(_tmp29E=_tmp29E->tl,
_tmp29B=_tmp29B->tl)){struct Cyc_Absyn_Aggrfield*_tmp2E8=(struct Cyc_Absyn_Aggrfield*)
_tmp29E->hd;struct Cyc_Absyn_Aggrfield*_tmp2E9=(struct Cyc_Absyn_Aggrfield*)
_tmp29B->hd;if(Cyc_Std_zstrptrcmp(_tmp2E8->name,_tmp2E9->name)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different member names)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual(_tmp2E8->tq,_tmp2E9->tq);Cyc_Tcutil_unify_it((void*)
_tmp2E8->type,(void*)_tmp2E9->type);if(!Cyc_Tcutil_same_atts(_tmp2E8->attributes,
_tmp2E9->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different attributes on member)",sizeof(char),33);(int)_throw((void*)
Cyc_Tcutil_Unify);}if(((_tmp2E8->width != 0?_tmp2E9->width == 0: 0)?1:(_tmp2E9->width
!= 0?_tmp2E8->width == 0: 0))?1:((_tmp2E8->width != 0?_tmp2E9->width != 0: 0)?Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2E8->width))!= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2E9->width)): 0)){Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=_tag_arr("(different bitfield widths on member)",
sizeof(char),38);(int)_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp29E == 0?_tmp29B == 
0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of members)",sizeof(char),30);goto _LL1C9;_LL1F0:
_tmp29F=_tmp221.f1;if((int)_tmp29F != 2)goto _LL1F2;_tmp2A0=_tmp221.f2;if((int)
_tmp2A0 != 2)goto _LL1F2;_LL1F1: return;_LL1F2: _tmp2A1=_tmp221.f1;if(_tmp2A1 <= (
void*)3?1:*((int*)_tmp2A1)!= 15)goto _LL1F4;_tmp2A2=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp2A1)->f1;_tmp2A3=_tmp221.f2;if(_tmp2A3 <= (void*)3?1:*((int*)_tmp2A3)!= 15)
goto _LL1F4;_tmp2A4=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2A3)->f1;
_LL1F3: Cyc_Tcutil_unify_it(_tmp2A2,_tmp2A4);return;_LL1F4: _tmp2A5=_tmp221.f1;if(
_tmp2A5 <= (void*)3?1:*((int*)_tmp2A5)!= 20)goto _LL1F6;_LL1F5: goto _LL1F7;_LL1F6:
_tmp2A6=_tmp221.f2;if(_tmp2A6 <= (void*)3?1:*((int*)_tmp2A6)!= 20)goto _LL1F8;
_LL1F7: goto _LL1F9;_LL1F8: _tmp2A7=_tmp221.f1;if(_tmp2A7 <= (void*)3?1:*((int*)
_tmp2A7)!= 19)goto _LL1FA;_LL1F9: goto _LL1FB;_LL1FA: _tmp2A8=_tmp221.f1;if(_tmp2A8
<= (void*)3?1:*((int*)_tmp2A8)!= 21)goto _LL1FC;_LL1FB: goto _LL1FD;_LL1FC: _tmp2A9=
_tmp221.f2;if(_tmp2A9 <= (void*)3?1:*((int*)_tmp2A9)!= 21)goto _LL1FE;_LL1FD: goto
_LL1FF;_LL1FE: _tmp2AA=_tmp221.f2;if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 19)
goto _LL200;_LL1FF: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)",sizeof(char),22);goto _LL1C9;_LL200:;_LL201: goto
_LL1C9;_LL1C9:;}(int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_opt_cmp(int(*
cmp)(void*,void*),struct Cyc_Core_Opt*a1,struct Cyc_Core_Opt*a2){if(a1 == a2)return
0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0: 0)return 1;return cmp((void*)((
struct Cyc_Core_Opt*)_check_null(a1))->v,(void*)((struct Cyc_Core_Opt*)_check_null(
a2))->v);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == 
a2)return 0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0: 0)return 1;return cmp((
void*)_check_null(a1),(void*)_check_null(a2));}int Cyc_Tcutil_list_cmp(int(*cmp)(
void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)return 0;
for(0;l1 != 0?l2 != 0: 0;(l1=l1->tl,l2=l2->tl)){int _tmp2EA=cmp((void*)l1->hd,(void*)
l2->hd);if(_tmp2EA != 0)return _tmp2EA;}if(l2 != 0)return - 1;if(l1 != 0)return 1;
return 0;}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){int _tmp2EB=(tq1.q_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int
_tmp2EC=(tq2.q_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp2EB,_tmp2EC);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp2ED=(void*)x->v;void*
_tmp2EE;_LL21A: if((int)_tmp2ED != 0)goto _LL21C;_LL21B: return - 1;_LL21C: if(_tmp2ED
<= (void*)1?1:*((int*)_tmp2ED)!= 0)goto _LL21E;_tmp2EE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2ED)->f1;_LL21D: {void*_tmp2EF=(void*)y->v;void*_tmp2F0;_LL221: if((int)
_tmp2EF != 0)goto _LL223;_LL222: return 1;_LL223: if(_tmp2EF <= (void*)1?1:*((int*)
_tmp2EF)!= 0)goto _LL225;_tmp2F0=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2EF)->f1;_LL224: return cmp(_tmp2EE,_tmp2F0);_LL225: if(_tmp2EF <= (void*)1?1:*((
int*)_tmp2EF)!= 1)goto _LL220;_LL226:({void*_tmp2F1[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress(2)",
sizeof(char),40),_tag_arr(_tmp2F1,sizeof(void*),0));});_LL220:;}_LL21E: if(
_tmp2ED <= (void*)1?1:*((int*)_tmp2ED)!= 1)goto _LL219;_LL21F:({void*_tmp2F2[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress",
sizeof(char),37),_tag_arr(_tmp2F2,sizeof(void*),0));});_LL219:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp2F4;struct Cyc_Absyn_Tqual
_tmp2F5;void*_tmp2F6;struct _tuple4*_tmp2F3=tqt1;_tmp2F4=*_tmp2F3;_tmp2F5=_tmp2F4.f1;
_tmp2F6=_tmp2F4.f2;{struct _tuple4 _tmp2F8;struct Cyc_Absyn_Tqual _tmp2F9;void*
_tmp2FA;struct _tuple4*_tmp2F7=tqt2;_tmp2F8=*_tmp2F7;_tmp2F9=_tmp2F8.f1;_tmp2FA=
_tmp2F8.f2;{int _tmp2FB=Cyc_Tcutil_tqual_cmp(_tmp2F5,_tmp2F9);if(_tmp2FB != 0)
return _tmp2FB;return Cyc_Tcutil_typecmp(_tmp2F6,_tmp2FA);}}}static int Cyc_Tcutil_const_uint_exp_cmp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return Cyc_Core_intcmp((int)Cyc_Evexp_eval_const_uint_exp(
e1),(int)Cyc_Evexp_eval_const_uint_exp(e2));}static int Cyc_Tcutil_attribute_case_number(
void*att){void*_tmp2FC=att;_LL228: if(_tmp2FC <= (void*)16?1:*((int*)_tmp2FC)!= 0)
goto _LL22A;_LL229: return 0;_LL22A: if((int)_tmp2FC != 0)goto _LL22C;_LL22B: return 1;
_LL22C: if((int)_tmp2FC != 1)goto _LL22E;_LL22D: return 2;_LL22E: if((int)_tmp2FC != 2)
goto _LL230;_LL22F: return 3;_LL230: if((int)_tmp2FC != 3)goto _LL232;_LL231: return 4;
_LL232: if((int)_tmp2FC != 4)goto _LL234;_LL233: return 5;_LL234: if(_tmp2FC <= (void*)
16?1:*((int*)_tmp2FC)!= 1)goto _LL236;_LL235: return 6;_LL236: if((int)_tmp2FC != 5)
goto _LL238;_LL237: return 7;_LL238: if(_tmp2FC <= (void*)16?1:*((int*)_tmp2FC)!= 2)
goto _LL23A;_LL239: return 8;_LL23A: if((int)_tmp2FC != 6)goto _LL23C;_LL23B: return 9;
_LL23C: if((int)_tmp2FC != 7)goto _LL23E;_LL23D: return 10;_LL23E: if((int)_tmp2FC != 8)
goto _LL240;_LL23F: return 11;_LL240: if((int)_tmp2FC != 9)goto _LL242;_LL241: return 12;
_LL242: if((int)_tmp2FC != 10)goto _LL244;_LL243: return 13;_LL244: if((int)_tmp2FC != 
11)goto _LL246;_LL245: return 14;_LL246: if((int)_tmp2FC != 12)goto _LL248;_LL247:
return 15;_LL248: if((int)_tmp2FC != 13)goto _LL24A;_LL249: return 16;_LL24A: if((int)
_tmp2FC != 14)goto _LL24C;_LL24B: return 17;_LL24C: if((int)_tmp2FC != 15)goto _LL24E;
_LL24D: return 18;_LL24E: if(_tmp2FC <= (void*)16?1:*((int*)_tmp2FC)!= 3)goto _LL227;
_LL24F: return 19;_LL227:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple6 _tmp2FE=({struct _tuple6 _tmp2FD;_tmp2FD.f1=att1;_tmp2FD.f2=att2;
_tmp2FD;});void*_tmp2FF;int _tmp300;void*_tmp301;int _tmp302;void*_tmp303;int
_tmp304;void*_tmp305;int _tmp306;void*_tmp307;struct _tagged_arr _tmp308;void*
_tmp309;struct _tagged_arr _tmp30A;void*_tmp30B;void*_tmp30C;int _tmp30D;int _tmp30E;
void*_tmp30F;void*_tmp310;int _tmp311;int _tmp312;_LL251: _tmp2FF=_tmp2FE.f1;if(
_tmp2FF <= (void*)16?1:*((int*)_tmp2FF)!= 0)goto _LL253;_tmp300=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp2FF)->f1;_tmp301=_tmp2FE.f2;if(_tmp301 <= (void*)16?1:*((int*)_tmp301)!= 0)
goto _LL253;_tmp302=((struct Cyc_Absyn_Regparm_att_struct*)_tmp301)->f1;_LL252:
_tmp304=_tmp300;_tmp306=_tmp302;goto _LL254;_LL253: _tmp303=_tmp2FE.f1;if(_tmp303
<= (void*)16?1:*((int*)_tmp303)!= 1)goto _LL255;_tmp304=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp303)->f1;_tmp305=_tmp2FE.f2;if(_tmp305 <= (void*)16?1:*((int*)_tmp305)!= 1)
goto _LL255;_tmp306=((struct Cyc_Absyn_Aligned_att_struct*)_tmp305)->f1;_LL254:
return Cyc_Core_intcmp(_tmp304,_tmp306);_LL255: _tmp307=_tmp2FE.f1;if(_tmp307 <= (
void*)16?1:*((int*)_tmp307)!= 2)goto _LL257;_tmp308=((struct Cyc_Absyn_Section_att_struct*)
_tmp307)->f1;_tmp309=_tmp2FE.f2;if(_tmp309 <= (void*)16?1:*((int*)_tmp309)!= 2)
goto _LL257;_tmp30A=((struct Cyc_Absyn_Section_att_struct*)_tmp309)->f1;_LL256:
return Cyc_Std_strcmp(_tmp308,_tmp30A);_LL257: _tmp30B=_tmp2FE.f1;if(_tmp30B <= (
void*)16?1:*((int*)_tmp30B)!= 3)goto _LL259;_tmp30C=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp30B)->f1;_tmp30D=((struct Cyc_Absyn_Format_att_struct*)_tmp30B)->f2;_tmp30E=((
struct Cyc_Absyn_Format_att_struct*)_tmp30B)->f3;_tmp30F=_tmp2FE.f2;if(_tmp30F <= (
void*)16?1:*((int*)_tmp30F)!= 3)goto _LL259;_tmp310=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp30F)->f1;_tmp311=((struct Cyc_Absyn_Format_att_struct*)_tmp30F)->f2;_tmp312=((
struct Cyc_Absyn_Format_att_struct*)_tmp30F)->f3;_LL258: {int _tmp313=Cyc_Core_intcmp((
int)((unsigned int)_tmp30C),(int)((unsigned int)_tmp310));if(_tmp313 != 0)return
_tmp313;{int _tmp314=Cyc_Core_intcmp(_tmp30D,_tmp311);if(_tmp314 != 0)return
_tmp314;return Cyc_Core_intcmp(_tmp30E,_tmp312);}}_LL259:;_LL25A: return Cyc_Core_intcmp(
Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));
_LL250:;}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct
Cyc_Absyn_Aggrfield*f2){int _tmp315=Cyc_Std_zstrptrcmp(f1->name,f2->name);if(
_tmp315 != 0)return _tmp315;{int _tmp316=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp316 != 0)return _tmp316;{int _tmp317=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp317 != 0)return _tmp317;{int _tmp318=Cyc_Tcutil_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp318 != 0)return _tmp318;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,f1->width,f2->width);}}}}
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp319=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp319 != 0)return
_tmp319;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t){void*_tmp31A=t;
_LL25C: if((int)_tmp31A != 0)goto _LL25E;_LL25D: return 0;_LL25E: if(_tmp31A <= (void*)
3?1:*((int*)_tmp31A)!= 0)goto _LL260;_LL25F: return 1;_LL260: if(_tmp31A <= (void*)3?
1:*((int*)_tmp31A)!= 1)goto _LL262;_LL261: return 2;_LL262: if(_tmp31A <= (void*)3?1:*((
int*)_tmp31A)!= 2)goto _LL264;_LL263: return 3;_LL264: if(_tmp31A <= (void*)3?1:*((
int*)_tmp31A)!= 3)goto _LL266;_LL265: return 4;_LL266: if(_tmp31A <= (void*)3?1:*((
int*)_tmp31A)!= 4)goto _LL268;_LL267: return 5;_LL268: if(_tmp31A <= (void*)3?1:*((
int*)_tmp31A)!= 5)goto _LL26A;_LL269: return 6;_LL26A: if((int)_tmp31A != 1)goto
_LL26C;_LL26B: return 7;_LL26C: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 6)goto
_LL26E;_LL26D: return 8;_LL26E: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 7)goto
_LL270;_LL26F: return 9;_LL270: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 8)goto
_LL272;_LL271: return 10;_LL272: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 9)goto
_LL274;_LL273: return 11;_LL274: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 10)goto
_LL276;_LL275: return 12;_LL276: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 11)goto
_LL278;_LL277: return 14;_LL278: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 12)goto
_LL27A;_LL279: return 16;_LL27A: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 13)goto
_LL27C;_LL27B: return 17;_LL27C: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 15)goto
_LL27E;_LL27D: return 18;_LL27E: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 16)goto
_LL280;_LL27F: return 19;_LL280: if((int)_tmp31A != 2)goto _LL282;_LL281: return 20;
_LL282: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 19)goto _LL284;_LL283: return 21;
_LL284: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 20)goto _LL286;_LL285: return 22;
_LL286: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 21)goto _LL288;_LL287: return 23;
_LL288: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 14)goto _LL28A;_LL289: return 24;
_LL28A: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 18)goto _LL28C;_LL28B: return 25;
_LL28C: if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 17)goto _LL25B;_LL28D: return 26;
_LL25B:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp31B=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(
t1),Cyc_Tcutil_type_case_number(t2));if(_tmp31B != 0)return _tmp31B;{struct _tuple6
_tmp31D=({struct _tuple6 _tmp31C;_tmp31C.f1=t2;_tmp31C.f2=t1;_tmp31C;});void*
_tmp31E;void*_tmp31F;void*_tmp320;void*_tmp321;void*_tmp322;struct Cyc_Absyn_Tvar*
_tmp323;void*_tmp324;struct Cyc_Absyn_Tvar*_tmp325;void*_tmp326;struct Cyc_Absyn_AggrInfo
_tmp327;void*_tmp328;struct Cyc_List_List*_tmp329;void*_tmp32A;struct Cyc_Absyn_AggrInfo
_tmp32B;void*_tmp32C;struct Cyc_List_List*_tmp32D;void*_tmp32E;struct _tuple1*
_tmp32F;void*_tmp330;struct _tuple1*_tmp331;void*_tmp332;struct Cyc_List_List*
_tmp333;void*_tmp334;struct Cyc_List_List*_tmp335;void*_tmp336;struct Cyc_Absyn_TunionInfo
_tmp337;void*_tmp338;struct Cyc_Absyn_Tuniondecl**_tmp339;struct Cyc_Absyn_Tuniondecl*
_tmp33A;struct Cyc_List_List*_tmp33B;void*_tmp33C;void*_tmp33D;struct Cyc_Absyn_TunionInfo
_tmp33E;void*_tmp33F;struct Cyc_Absyn_Tuniondecl**_tmp340;struct Cyc_Absyn_Tuniondecl*
_tmp341;struct Cyc_List_List*_tmp342;void*_tmp343;void*_tmp344;struct Cyc_Absyn_TunionFieldInfo
_tmp345;void*_tmp346;struct Cyc_Absyn_Tuniondecl*_tmp347;struct Cyc_Absyn_Tunionfield*
_tmp348;struct Cyc_List_List*_tmp349;void*_tmp34A;struct Cyc_Absyn_TunionFieldInfo
_tmp34B;void*_tmp34C;struct Cyc_Absyn_Tuniondecl*_tmp34D;struct Cyc_Absyn_Tunionfield*
_tmp34E;struct Cyc_List_List*_tmp34F;void*_tmp350;struct Cyc_Absyn_PtrInfo _tmp351;
void*_tmp352;void*_tmp353;struct Cyc_Absyn_Conref*_tmp354;struct Cyc_Absyn_Tqual
_tmp355;struct Cyc_Absyn_Conref*_tmp356;void*_tmp357;struct Cyc_Absyn_PtrInfo
_tmp358;void*_tmp359;void*_tmp35A;struct Cyc_Absyn_Conref*_tmp35B;struct Cyc_Absyn_Tqual
_tmp35C;struct Cyc_Absyn_Conref*_tmp35D;void*_tmp35E;void*_tmp35F;void*_tmp360;
void*_tmp361;void*_tmp362;void*_tmp363;void*_tmp364;void*_tmp365;void*_tmp366;
int _tmp367;void*_tmp368;int _tmp369;void*_tmp36A;void*_tmp36B;struct Cyc_Absyn_Tqual
_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;void*_tmp36E;void*_tmp36F;struct Cyc_Absyn_Tqual
_tmp370;struct Cyc_Absyn_Exp*_tmp371;void*_tmp372;struct Cyc_Absyn_FnInfo _tmp373;
struct Cyc_List_List*_tmp374;struct Cyc_Core_Opt*_tmp375;void*_tmp376;struct Cyc_List_List*
_tmp377;int _tmp378;struct Cyc_Absyn_VarargInfo*_tmp379;struct Cyc_List_List*
_tmp37A;struct Cyc_List_List*_tmp37B;void*_tmp37C;struct Cyc_Absyn_FnInfo _tmp37D;
struct Cyc_List_List*_tmp37E;struct Cyc_Core_Opt*_tmp37F;void*_tmp380;struct Cyc_List_List*
_tmp381;int _tmp382;struct Cyc_Absyn_VarargInfo*_tmp383;struct Cyc_List_List*
_tmp384;struct Cyc_List_List*_tmp385;void*_tmp386;struct Cyc_List_List*_tmp387;
void*_tmp388;struct Cyc_List_List*_tmp389;void*_tmp38A;void*_tmp38B;struct Cyc_List_List*
_tmp38C;void*_tmp38D;void*_tmp38E;struct Cyc_List_List*_tmp38F;void*_tmp390;void*
_tmp391;void*_tmp392;void*_tmp393;void*_tmp394;void*_tmp395;void*_tmp396;void*
_tmp397;void*_tmp398;void*_tmp399;void*_tmp39A;void*_tmp39B;void*_tmp39C;void*
_tmp39D;void*_tmp39E;int _tmp39F;void*_tmp3A0;int _tmp3A1;void*_tmp3A2;void*
_tmp3A3;void*_tmp3A4;void*_tmp3A5;void*_tmp3A6;void*_tmp3A7;_LL28F: _tmp31E=
_tmp31D.f1;if(_tmp31E <= (void*)3?1:*((int*)_tmp31E)!= 0)goto _LL291;_tmp31F=
_tmp31D.f2;if(_tmp31F <= (void*)3?1:*((int*)_tmp31F)!= 0)goto _LL291;_LL290:({void*
_tmp3A8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typecmp: can only compare closed types",sizeof(char),39),_tag_arr(
_tmp3A8,sizeof(void*),0));});_LL291: _tmp320=_tmp31D.f1;if((int)_tmp320 != 0)goto
_LL293;_tmp321=_tmp31D.f2;if((int)_tmp321 != 0)goto _LL293;_LL292: return 0;_LL293:
_tmp322=_tmp31D.f1;if(_tmp322 <= (void*)3?1:*((int*)_tmp322)!= 1)goto _LL295;
_tmp323=((struct Cyc_Absyn_VarType_struct*)_tmp322)->f1;_tmp324=_tmp31D.f2;if(
_tmp324 <= (void*)3?1:*((int*)_tmp324)!= 1)goto _LL295;_tmp325=((struct Cyc_Absyn_VarType_struct*)
_tmp324)->f1;_LL294: return Cyc_Core_intcmp(*((int*)_check_null(_tmp325->identity)),*((
int*)_check_null(_tmp323->identity)));_LL295: _tmp326=_tmp31D.f1;if(_tmp326 <= (
void*)3?1:*((int*)_tmp326)!= 10)goto _LL297;_tmp327=((struct Cyc_Absyn_AggrType_struct*)
_tmp326)->f1;_tmp328=(void*)_tmp327.aggr_info;_tmp329=_tmp327.targs;_tmp32A=
_tmp31D.f2;if(_tmp32A <= (void*)3?1:*((int*)_tmp32A)!= 10)goto _LL297;_tmp32B=((
struct Cyc_Absyn_AggrType_struct*)_tmp32A)->f1;_tmp32C=(void*)_tmp32B.aggr_info;
_tmp32D=_tmp32B.targs;_LL296: {struct _tuple1*_tmp3AA;struct _tuple5 _tmp3A9=Cyc_Absyn_aggr_kinded_name(
_tmp328);_tmp3AA=_tmp3A9.f2;{struct _tuple1*_tmp3AC;struct _tuple5 _tmp3AB=Cyc_Absyn_aggr_kinded_name(
_tmp32C);_tmp3AC=_tmp3AB.f2;{int _tmp3AD=Cyc_Absyn_qvar_cmp(_tmp3AA,_tmp3AC);if(
_tmp3AD != 0)return _tmp3AD;else{return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp329,_tmp32D);}}}}_LL297: _tmp32E=_tmp31D.f1;if(_tmp32E <= (void*)3?1:*((int*)
_tmp32E)!= 12)goto _LL299;_tmp32F=((struct Cyc_Absyn_EnumType_struct*)_tmp32E)->f1;
_tmp330=_tmp31D.f2;if(_tmp330 <= (void*)3?1:*((int*)_tmp330)!= 12)goto _LL299;
_tmp331=((struct Cyc_Absyn_EnumType_struct*)_tmp330)->f1;_LL298: return Cyc_Absyn_qvar_cmp(
_tmp32F,_tmp331);_LL299: _tmp332=_tmp31D.f1;if(_tmp332 <= (void*)3?1:*((int*)
_tmp332)!= 13)goto _LL29B;_tmp333=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp332)->f1;
_tmp334=_tmp31D.f2;if(_tmp334 <= (void*)3?1:*((int*)_tmp334)!= 13)goto _LL29B;
_tmp335=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp334)->f1;_LL29A: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp333,
_tmp335);_LL29B: _tmp336=_tmp31D.f1;if(_tmp336 <= (void*)3?1:*((int*)_tmp336)!= 2)
goto _LL29D;_tmp337=((struct Cyc_Absyn_TunionType_struct*)_tmp336)->f1;_tmp338=(
void*)_tmp337.tunion_info;if(*((int*)_tmp338)!= 1)goto _LL29D;_tmp339=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp338)->f1;_tmp33A=*_tmp339;_tmp33B=_tmp337.targs;_tmp33C=(void*)_tmp337.rgn;
_tmp33D=_tmp31D.f2;if(_tmp33D <= (void*)3?1:*((int*)_tmp33D)!= 2)goto _LL29D;
_tmp33E=((struct Cyc_Absyn_TunionType_struct*)_tmp33D)->f1;_tmp33F=(void*)_tmp33E.tunion_info;
if(*((int*)_tmp33F)!= 1)goto _LL29D;_tmp340=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp33F)->f1;_tmp341=*_tmp340;_tmp342=_tmp33E.targs;_tmp343=(void*)_tmp33E.rgn;
_LL29C: if(_tmp341 == _tmp33A)return 0;{int _tmp3AE=Cyc_Absyn_qvar_cmp(_tmp341->name,
_tmp33A->name);if(_tmp3AE != 0)return _tmp3AE;{int _tmp3AF=Cyc_Tcutil_typecmp(
_tmp343,_tmp33C);if(_tmp3AF != 0)return _tmp3AF;return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp342,_tmp33B);}}_LL29D: _tmp344=_tmp31D.f1;if(_tmp344 <= (void*)3?1:*((int*)
_tmp344)!= 3)goto _LL29F;_tmp345=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp344)->f1;_tmp346=(void*)_tmp345.field_info;if(*((int*)_tmp346)!= 1)goto
_LL29F;_tmp347=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp346)->f1;_tmp348=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp346)->f2;_tmp349=_tmp345.targs;
_tmp34A=_tmp31D.f2;if(_tmp34A <= (void*)3?1:*((int*)_tmp34A)!= 3)goto _LL29F;
_tmp34B=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp34A)->f1;_tmp34C=(void*)
_tmp34B.field_info;if(*((int*)_tmp34C)!= 1)goto _LL29F;_tmp34D=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp34C)->f1;_tmp34E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp34C)->f2;
_tmp34F=_tmp34B.targs;_LL29E: if(_tmp34D == _tmp347)return 0;{int _tmp3B0=Cyc_Absyn_qvar_cmp(
_tmp347->name,_tmp34D->name);if(_tmp3B0 != 0)return _tmp3B0;{int _tmp3B1=Cyc_Absyn_qvar_cmp(
_tmp348->name,_tmp34E->name);if(_tmp3B1 != 0)return _tmp3B1;return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp34F,_tmp349);}}_LL29F: _tmp350=_tmp31D.f1;if(_tmp350 <= (
void*)3?1:*((int*)_tmp350)!= 4)goto _LL2A1;_tmp351=((struct Cyc_Absyn_PointerType_struct*)
_tmp350)->f1;_tmp352=(void*)_tmp351.elt_typ;_tmp353=(void*)_tmp351.rgn_typ;
_tmp354=_tmp351.nullable;_tmp355=_tmp351.tq;_tmp356=_tmp351.bounds;_tmp357=
_tmp31D.f2;if(_tmp357 <= (void*)3?1:*((int*)_tmp357)!= 4)goto _LL2A1;_tmp358=((
struct Cyc_Absyn_PointerType_struct*)_tmp357)->f1;_tmp359=(void*)_tmp358.elt_typ;
_tmp35A=(void*)_tmp358.rgn_typ;_tmp35B=_tmp358.nullable;_tmp35C=_tmp358.tq;
_tmp35D=_tmp358.bounds;_LL2A0: {int _tmp3B2=Cyc_Tcutil_typecmp(_tmp359,_tmp352);
if(_tmp3B2 != 0)return _tmp3B2;{int _tmp3B3=Cyc_Tcutil_typecmp(_tmp35A,_tmp353);if(
_tmp3B3 != 0)return _tmp3B3;{int _tmp3B4=Cyc_Tcutil_tqual_cmp(_tmp35C,_tmp355);if(
_tmp3B4 != 0)return _tmp3B4;{int _tmp3B5=Cyc_Tcutil_conrefs_cmp(Cyc_Tcutil_boundscmp,
_tmp35D,_tmp356);if(_tmp3B5 != 0)return _tmp3B5;{void*_tmp3B6=(void*)(Cyc_Absyn_compress_conref(
_tmp35D))->v;void*_tmp3B7;_LL2C8: if(_tmp3B6 <= (void*)1?1:*((int*)_tmp3B6)!= 0)
goto _LL2CA;_tmp3B7=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp3B6)->f1;if((
int)_tmp3B7 != 0)goto _LL2CA;_LL2C9: return 0;_LL2CA:;_LL2CB: goto _LL2C7;_LL2C7:;}
return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp35B,_tmp354);}}}}_LL2A1: _tmp35E=
_tmp31D.f1;if(_tmp35E <= (void*)3?1:*((int*)_tmp35E)!= 5)goto _LL2A3;_tmp35F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp35E)->f1;_tmp360=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp35E)->f2;_tmp361=_tmp31D.f2;if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 5)
goto _LL2A3;_tmp362=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp361)->f1;_tmp363=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp361)->f2;_LL2A2: if(_tmp362 != _tmp35F)
return Cyc_Core_intcmp((int)((unsigned int)_tmp362),(int)((unsigned int)_tmp35F));
if(_tmp363 != _tmp360)return Cyc_Core_intcmp((int)((unsigned int)_tmp363),(int)((
unsigned int)_tmp360));return 0;_LL2A3: _tmp364=_tmp31D.f1;if((int)_tmp364 != 1)
goto _LL2A5;_tmp365=_tmp31D.f2;if((int)_tmp365 != 1)goto _LL2A5;_LL2A4: return 0;
_LL2A5: _tmp366=_tmp31D.f1;if(_tmp366 <= (void*)3?1:*((int*)_tmp366)!= 6)goto
_LL2A7;_tmp367=((struct Cyc_Absyn_DoubleType_struct*)_tmp366)->f1;_tmp368=_tmp31D.f2;
if(_tmp368 <= (void*)3?1:*((int*)_tmp368)!= 6)goto _LL2A7;_tmp369=((struct Cyc_Absyn_DoubleType_struct*)
_tmp368)->f1;_LL2A6: if(_tmp367 == _tmp369)return 0;else{if(_tmp367)return - 1;else{
return 1;}}_LL2A7: _tmp36A=_tmp31D.f1;if(_tmp36A <= (void*)3?1:*((int*)_tmp36A)!= 7)
goto _LL2A9;_tmp36B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp36A)->f1;
_tmp36C=((struct Cyc_Absyn_ArrayType_struct*)_tmp36A)->f2;_tmp36D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp36A)->f3;_tmp36E=_tmp31D.f2;if(_tmp36E <= (void*)3?1:*((int*)_tmp36E)!= 7)
goto _LL2A9;_tmp36F=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp36E)->f1;
_tmp370=((struct Cyc_Absyn_ArrayType_struct*)_tmp36E)->f2;_tmp371=((struct Cyc_Absyn_ArrayType_struct*)
_tmp36E)->f3;_LL2A8: {int _tmp3B8=Cyc_Tcutil_tqual_cmp(_tmp370,_tmp36C);if(
_tmp3B8 != 0)return _tmp3B8;{int _tmp3B9=Cyc_Tcutil_typecmp(_tmp36F,_tmp36B);if(
_tmp3B9 != 0)return _tmp3B9;if(_tmp36D == _tmp371)return 0;if(_tmp36D == 0?1: _tmp371
== 0)({void*_tmp3BA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("missing expression in array index",sizeof(char),34),_tag_arr(_tmp3BA,
sizeof(void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,
_tmp36D,_tmp371);}}_LL2A9: _tmp372=_tmp31D.f1;if(_tmp372 <= (void*)3?1:*((int*)
_tmp372)!= 8)goto _LL2AB;_tmp373=((struct Cyc_Absyn_FnType_struct*)_tmp372)->f1;
_tmp374=_tmp373.tvars;_tmp375=_tmp373.effect;_tmp376=(void*)_tmp373.ret_typ;
_tmp377=_tmp373.args;_tmp378=_tmp373.c_varargs;_tmp379=_tmp373.cyc_varargs;
_tmp37A=_tmp373.rgn_po;_tmp37B=_tmp373.attributes;_tmp37C=_tmp31D.f2;if(_tmp37C
<= (void*)3?1:*((int*)_tmp37C)!= 8)goto _LL2AB;_tmp37D=((struct Cyc_Absyn_FnType_struct*)
_tmp37C)->f1;_tmp37E=_tmp37D.tvars;_tmp37F=_tmp37D.effect;_tmp380=(void*)_tmp37D.ret_typ;
_tmp381=_tmp37D.args;_tmp382=_tmp37D.c_varargs;_tmp383=_tmp37D.cyc_varargs;
_tmp384=_tmp37D.rgn_po;_tmp385=_tmp37D.attributes;_LL2AA:({void*_tmp3BB[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: function types not handled",
sizeof(char),36),_tag_arr(_tmp3BB,sizeof(void*),0));});_LL2AB: _tmp386=_tmp31D.f1;
if(_tmp386 <= (void*)3?1:*((int*)_tmp386)!= 9)goto _LL2AD;_tmp387=((struct Cyc_Absyn_TupleType_struct*)
_tmp386)->f1;_tmp388=_tmp31D.f2;if(_tmp388 <= (void*)3?1:*((int*)_tmp388)!= 9)
goto _LL2AD;_tmp389=((struct Cyc_Absyn_TupleType_struct*)_tmp388)->f1;_LL2AC:
return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp389,
_tmp387);_LL2AD: _tmp38A=_tmp31D.f1;if(_tmp38A <= (void*)3?1:*((int*)_tmp38A)!= 11)
goto _LL2AF;_tmp38B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp38A)->f1;
_tmp38C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp38A)->f2;_tmp38D=_tmp31D.f2;
if(_tmp38D <= (void*)3?1:*((int*)_tmp38D)!= 11)goto _LL2AF;_tmp38E=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp38D)->f1;_tmp38F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp38D)->f2;_LL2AE: if(_tmp38E != _tmp38B){if(_tmp38E == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp38F,_tmp38C);_LL2AF: _tmp390=_tmp31D.f1;if((int)_tmp390 != 2)goto _LL2B1;
_tmp391=_tmp31D.f2;if((int)_tmp391 != 2)goto _LL2B1;_LL2B0: return 0;_LL2B1: _tmp392=
_tmp31D.f1;if(_tmp392 <= (void*)3?1:*((int*)_tmp392)!= 15)goto _LL2B3;_tmp393=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp392)->f1;_tmp394=_tmp31D.f2;
if(_tmp394 <= (void*)3?1:*((int*)_tmp394)!= 15)goto _LL2B3;_tmp395=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp394)->f1;_LL2B2: return Cyc_Tcutil_typecmp(
_tmp393,_tmp395);_LL2B3: _tmp396=_tmp31D.f1;if(_tmp396 <= (void*)3?1:*((int*)
_tmp396)!= 14)goto _LL2B5;_tmp397=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp396)->f1;_tmp398=_tmp31D.f2;if(_tmp398 <= (void*)3?1:*((int*)_tmp398)!= 14)
goto _LL2B5;_tmp399=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp398)->f1;
_LL2B4: return Cyc_Tcutil_typecmp(_tmp397,_tmp399);_LL2B5: _tmp39A=_tmp31D.f1;if(
_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 17)goto _LL2B7;_tmp39B=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp39A)->f1;_tmp39C=_tmp31D.f2;if(_tmp39C <= (void*)3?1:*((int*)_tmp39C)!= 17)
goto _LL2B7;_tmp39D=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp39C)->f1;_LL2B6:
return Cyc_Tcutil_typecmp(_tmp39B,_tmp39D);_LL2B7: _tmp39E=_tmp31D.f1;if(_tmp39E <= (
void*)3?1:*((int*)_tmp39E)!= 18)goto _LL2B9;_tmp39F=((struct Cyc_Absyn_TypeInt_struct*)
_tmp39E)->f1;_tmp3A0=_tmp31D.f2;if(_tmp3A0 <= (void*)3?1:*((int*)_tmp3A0)!= 18)
goto _LL2B9;_tmp3A1=((struct Cyc_Absyn_TypeInt_struct*)_tmp3A0)->f1;_LL2B8: return
Cyc_Core_intcmp(_tmp39F,_tmp3A1);_LL2B9: _tmp3A2=_tmp31D.f1;if(_tmp3A2 <= (void*)3?
1:*((int*)_tmp3A2)!= 20)goto _LL2BB;_LL2BA: goto _LL2BC;_LL2BB: _tmp3A3=_tmp31D.f2;
if(_tmp3A3 <= (void*)3?1:*((int*)_tmp3A3)!= 20)goto _LL2BD;_LL2BC: goto _LL2BE;
_LL2BD: _tmp3A4=_tmp31D.f1;if(_tmp3A4 <= (void*)3?1:*((int*)_tmp3A4)!= 19)goto
_LL2BF;_LL2BE: goto _LL2C0;_LL2BF: _tmp3A5=_tmp31D.f1;if(_tmp3A5 <= (void*)3?1:*((
int*)_tmp3A5)!= 21)goto _LL2C1;_LL2C0: goto _LL2C2;_LL2C1: _tmp3A6=_tmp31D.f2;if(
_tmp3A6 <= (void*)3?1:*((int*)_tmp3A6)!= 21)goto _LL2C3;_LL2C2: goto _LL2C4;_LL2C3:
_tmp3A7=_tmp31D.f2;if(_tmp3A7 <= (void*)3?1:*((int*)_tmp3A7)!= 19)goto _LL2C5;
_LL2C4:({void*_tmp3BC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("typecmp: effects not handled",sizeof(char),29),
_tag_arr(_tmp3BC,sizeof(void*),0));});_LL2C5:;_LL2C6:({void*_tmp3BD[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Unmatched case in typecmp",
sizeof(char),26),_tag_arr(_tmp3BD,sizeof(void*),0));});_LL28E:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp3BE=Cyc_Tcutil_compress(t);_LL2CD: if(_tmp3BE <= (void*)3?1:*((
int*)_tmp3BE)!= 5)goto _LL2CF;_LL2CE: goto _LL2D0;_LL2CF: if((int)_tmp3BE != 1)goto
_LL2D1;_LL2D0: goto _LL2D2;_LL2D1: if(_tmp3BE <= (void*)3?1:*((int*)_tmp3BE)!= 6)
goto _LL2D3;_LL2D2: goto _LL2D4;_LL2D3: if(_tmp3BE <= (void*)3?1:*((int*)_tmp3BE)!= 
12)goto _LL2D5;_LL2D4: goto _LL2D6;_LL2D5: if(_tmp3BE <= (void*)3?1:*((int*)_tmp3BE)
!= 13)goto _LL2D7;_LL2D6: return 1;_LL2D7:;_LL2D8: return 0;_LL2CC:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp3C0=({struct _tuple6 _tmp3BF;_tmp3BF.f1=Cyc_Tcutil_compress(
t1);_tmp3BF.f2=Cyc_Tcutil_compress(t2);_tmp3BF;});void*_tmp3C1;int _tmp3C2;void*
_tmp3C3;int _tmp3C4;void*_tmp3C5;void*_tmp3C6;void*_tmp3C7;void*_tmp3C8;void*
_tmp3C9;void*_tmp3CA;void*_tmp3CB;void*_tmp3CC;void*_tmp3CD;void*_tmp3CE;void*
_tmp3CF;void*_tmp3D0;void*_tmp3D1;void*_tmp3D2;void*_tmp3D3;void*_tmp3D4;void*
_tmp3D5;void*_tmp3D6;void*_tmp3D7;void*_tmp3D8;void*_tmp3D9;void*_tmp3DA;void*
_tmp3DB;void*_tmp3DC;void*_tmp3DD;void*_tmp3DE;void*_tmp3DF;void*_tmp3E0;void*
_tmp3E1;void*_tmp3E2;void*_tmp3E3;void*_tmp3E4;void*_tmp3E5;void*_tmp3E6;void*
_tmp3E7;void*_tmp3E8;void*_tmp3E9;void*_tmp3EA;void*_tmp3EB;void*_tmp3EC;void*
_tmp3ED;void*_tmp3EE;void*_tmp3EF;void*_tmp3F0;void*_tmp3F1;void*_tmp3F2;void*
_tmp3F3;_LL2DA: _tmp3C1=_tmp3C0.f1;if(_tmp3C1 <= (void*)3?1:*((int*)_tmp3C1)!= 6)
goto _LL2DC;_tmp3C2=((struct Cyc_Absyn_DoubleType_struct*)_tmp3C1)->f1;_tmp3C3=
_tmp3C0.f2;if(_tmp3C3 <= (void*)3?1:*((int*)_tmp3C3)!= 6)goto _LL2DC;_tmp3C4=((
struct Cyc_Absyn_DoubleType_struct*)_tmp3C3)->f1;_LL2DB: return !_tmp3C4?_tmp3C2: 0;
_LL2DC: _tmp3C5=_tmp3C0.f1;if(_tmp3C5 <= (void*)3?1:*((int*)_tmp3C5)!= 6)goto
_LL2DE;_tmp3C6=_tmp3C0.f2;if((int)_tmp3C6 != 1)goto _LL2DE;_LL2DD: goto _LL2DF;
_LL2DE: _tmp3C7=_tmp3C0.f1;if(_tmp3C7 <= (void*)3?1:*((int*)_tmp3C7)!= 6)goto
_LL2E0;_tmp3C8=_tmp3C0.f2;if(_tmp3C8 <= (void*)3?1:*((int*)_tmp3C8)!= 5)goto
_LL2E0;_LL2DF: goto _LL2E1;_LL2E0: _tmp3C9=_tmp3C0.f1;if(_tmp3C9 <= (void*)3?1:*((
int*)_tmp3C9)!= 6)goto _LL2E2;_tmp3CA=_tmp3C0.f2;if(_tmp3CA <= (void*)3?1:*((int*)
_tmp3CA)!= 14)goto _LL2E2;_LL2E1: goto _LL2E3;_LL2E2: _tmp3CB=_tmp3C0.f1;if((int)
_tmp3CB != 1)goto _LL2E4;_tmp3CC=_tmp3C0.f2;if(_tmp3CC <= (void*)3?1:*((int*)
_tmp3CC)!= 14)goto _LL2E4;_LL2E3: goto _LL2E5;_LL2E4: _tmp3CD=_tmp3C0.f1;if(_tmp3CD
<= (void*)3?1:*((int*)_tmp3CD)!= 6)goto _LL2E6;_tmp3CE=_tmp3C0.f2;if(_tmp3CE <= (
void*)3?1:*((int*)_tmp3CE)!= 17)goto _LL2E6;_LL2E5: goto _LL2E7;_LL2E6: _tmp3CF=
_tmp3C0.f1;if((int)_tmp3CF != 1)goto _LL2E8;_tmp3D0=_tmp3C0.f2;if(_tmp3D0 <= (void*)
3?1:*((int*)_tmp3D0)!= 17)goto _LL2E8;_LL2E7: goto _LL2E9;_LL2E8: _tmp3D1=_tmp3C0.f1;
if((int)_tmp3D1 != 1)goto _LL2EA;_tmp3D2=_tmp3C0.f2;if(_tmp3D2 <= (void*)3?1:*((int*)
_tmp3D2)!= 5)goto _LL2EA;_LL2E9: return 1;_LL2EA: _tmp3D3=_tmp3C0.f1;if(_tmp3D3 <= (
void*)3?1:*((int*)_tmp3D3)!= 5)goto _LL2EC;_tmp3D4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D3)->f2;if((int)_tmp3D4 != 3)goto _LL2EC;_tmp3D5=_tmp3C0.f2;if(_tmp3D5 <= (
void*)3?1:*((int*)_tmp3D5)!= 5)goto _LL2EC;_tmp3D6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D5)->f2;if((int)_tmp3D6 != 3)goto _LL2EC;_LL2EB: return 0;_LL2EC: _tmp3D7=
_tmp3C0.f1;if(_tmp3D7 <= (void*)3?1:*((int*)_tmp3D7)!= 5)goto _LL2EE;_tmp3D8=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3D7)->f2;if((int)_tmp3D8 != 3)goto _LL2EE;
_LL2ED: goto _LL2EF;_LL2EE: _tmp3D9=_tmp3C0.f1;if(_tmp3D9 <= (void*)3?1:*((int*)
_tmp3D9)!= 5)goto _LL2F0;_tmp3DA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D9)->f2;
if((int)_tmp3DA != 2)goto _LL2F0;_tmp3DB=_tmp3C0.f2;if((int)_tmp3DB != 1)goto _LL2F0;
_LL2EF: goto _LL2F1;_LL2F0: _tmp3DC=_tmp3C0.f1;if(_tmp3DC <= (void*)3?1:*((int*)
_tmp3DC)!= 5)goto _LL2F2;_tmp3DD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3DC)->f2;
if((int)_tmp3DD != 2)goto _LL2F2;_tmp3DE=_tmp3C0.f2;if(_tmp3DE <= (void*)3?1:*((int*)
_tmp3DE)!= 5)goto _LL2F2;_tmp3DF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3DE)->f2;
if((int)_tmp3DF != 1)goto _LL2F2;_LL2F1: goto _LL2F3;_LL2F2: _tmp3E0=_tmp3C0.f1;if(
_tmp3E0 <= (void*)3?1:*((int*)_tmp3E0)!= 5)goto _LL2F4;_tmp3E1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3E0)->f2;if((int)_tmp3E1 != 2)goto _LL2F4;_tmp3E2=_tmp3C0.f2;if(_tmp3E2 <= (
void*)3?1:*((int*)_tmp3E2)!= 5)goto _LL2F4;_tmp3E3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3E2)->f2;if((int)_tmp3E3 != 0)goto _LL2F4;_LL2F3: goto _LL2F5;_LL2F4: _tmp3E4=
_tmp3C0.f1;if(_tmp3E4 <= (void*)3?1:*((int*)_tmp3E4)!= 5)goto _LL2F6;_tmp3E5=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3E4)->f2;if((int)_tmp3E5 != 1)goto _LL2F6;
_tmp3E6=_tmp3C0.f2;if(_tmp3E6 <= (void*)3?1:*((int*)_tmp3E6)!= 5)goto _LL2F6;
_tmp3E7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E6)->f2;if((int)_tmp3E7 != 
0)goto _LL2F6;_LL2F5: goto _LL2F7;_LL2F6: _tmp3E8=_tmp3C0.f1;if(_tmp3E8 <= (void*)3?1:*((
int*)_tmp3E8)!= 17)goto _LL2F8;_tmp3E9=_tmp3C0.f2;if(_tmp3E9 <= (void*)3?1:*((int*)
_tmp3E9)!= 5)goto _LL2F8;_tmp3EA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E9)->f2;
if((int)_tmp3EA != 1)goto _LL2F8;_LL2F7: goto _LL2F9;_LL2F8: _tmp3EB=_tmp3C0.f1;if(
_tmp3EB <= (void*)3?1:*((int*)_tmp3EB)!= 17)goto _LL2FA;_tmp3EC=_tmp3C0.f2;if(
_tmp3EC <= (void*)3?1:*((int*)_tmp3EC)!= 5)goto _LL2FA;_tmp3ED=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3EC)->f2;if((int)_tmp3ED != 0)goto _LL2FA;_LL2F9: goto _LL2FB;_LL2FA: _tmp3EE=
_tmp3C0.f1;if(_tmp3EE <= (void*)3?1:*((int*)_tmp3EE)!= 14)goto _LL2FC;_tmp3EF=
_tmp3C0.f2;if(_tmp3EF <= (void*)3?1:*((int*)_tmp3EF)!= 5)goto _LL2FC;_tmp3F0=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3EF)->f2;if((int)_tmp3F0 != 1)goto _LL2FC;
_LL2FB: goto _LL2FD;_LL2FC: _tmp3F1=_tmp3C0.f1;if(_tmp3F1 <= (void*)3?1:*((int*)
_tmp3F1)!= 14)goto _LL2FE;_tmp3F2=_tmp3C0.f2;if(_tmp3F2 <= (void*)3?1:*((int*)
_tmp3F2)!= 5)goto _LL2FE;_tmp3F3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3F2)->f2;
if((int)_tmp3F3 != 0)goto _LL2FE;_LL2FD: return 1;_LL2FE:;_LL2FF: return 0;_LL2D9:;}
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=
el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0?1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4->v=(
void*)t1;_tmp3F4;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)((
struct Cyc_Core_Opt*)_check_null(max_arith_type))->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_Std_String_pa_struct _tmp3F7;_tmp3F7.tag=0;_tmp3F7.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp3F6;_tmp3F6.tag=0;_tmp3F6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp3F5[2]={& _tmp3F6,& _tmp3F7};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,
_tag_arr("type mismatch: expecting %s but found %s",sizeof(char),41),_tag_arr(
_tmp3F5,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp3F8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL301: if(_tmp3F8 <= (void*)3?1:*((int*)_tmp3F8)!= 4)goto _LL303;_LL302: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ);goto _LL300;_LL303:;_LL304: return 0;_LL300:;}return 1;}int
Cyc_Tcutil_is_integral_type(void*t){void*_tmp3F9=Cyc_Tcutil_compress(t);_LL306:
if(_tmp3F9 <= (void*)3?1:*((int*)_tmp3F9)!= 5)goto _LL308;_LL307: goto _LL309;_LL308:
if(_tmp3F9 <= (void*)3?1:*((int*)_tmp3F9)!= 14)goto _LL30A;_LL309: goto _LL30B;
_LL30A: if(_tmp3F9 <= (void*)3?1:*((int*)_tmp3F9)!= 17)goto _LL30C;_LL30B: goto
_LL30D;_LL30C: if(_tmp3F9 <= (void*)3?1:*((int*)_tmp3F9)!= 12)goto _LL30E;_LL30D:
goto _LL30F;_LL30E: if(_tmp3F9 <= (void*)3?1:*((int*)_tmp3F9)!= 13)goto _LL310;
_LL30F: return 1;_LL310:;_LL311: return 0;_LL305:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp3FA[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3FA,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp3FB[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3FB,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp3FD=({struct _tuple6 _tmp3FC;
_tmp3FC.f1=t1;_tmp3FC.f2=t2;_tmp3FC;});void*_tmp3FE;struct Cyc_Absyn_PtrInfo
_tmp3FF;void*_tmp400;struct Cyc_Absyn_PtrInfo _tmp401;void*_tmp402;void*_tmp403;
struct Cyc_Absyn_Tqual _tmp404;struct Cyc_Absyn_Exp*_tmp405;void*_tmp406;void*
_tmp407;struct Cyc_Absyn_Tqual _tmp408;struct Cyc_Absyn_Exp*_tmp409;void*_tmp40A;
struct Cyc_Absyn_TunionFieldInfo _tmp40B;void*_tmp40C;struct Cyc_Absyn_Tuniondecl*
_tmp40D;struct Cyc_Absyn_Tunionfield*_tmp40E;struct Cyc_List_List*_tmp40F;void*
_tmp410;struct Cyc_Absyn_TunionInfo _tmp411;void*_tmp412;struct Cyc_Absyn_Tuniondecl**
_tmp413;struct Cyc_Absyn_Tuniondecl*_tmp414;struct Cyc_List_List*_tmp415;void*
_tmp416;struct Cyc_Absyn_PtrInfo _tmp417;void*_tmp418;void*_tmp419;struct Cyc_Absyn_Conref*
_tmp41A;struct Cyc_Absyn_Tqual _tmp41B;struct Cyc_Absyn_Conref*_tmp41C;void*_tmp41D;
struct Cyc_Absyn_TunionInfo _tmp41E;void*_tmp41F;struct Cyc_Absyn_Tuniondecl**
_tmp420;struct Cyc_Absyn_Tuniondecl*_tmp421;struct Cyc_List_List*_tmp422;void*
_tmp423;void*_tmp424;void*_tmp425;void*_tmp426;void*_tmp427;void*_tmp428;void*
_tmp429;_LL313: _tmp3FE=_tmp3FD.f1;if(_tmp3FE <= (void*)3?1:*((int*)_tmp3FE)!= 4)
goto _LL315;_tmp3FF=((struct Cyc_Absyn_PointerType_struct*)_tmp3FE)->f1;_tmp400=
_tmp3FD.f2;if(_tmp400 <= (void*)3?1:*((int*)_tmp400)!= 4)goto _LL315;_tmp401=((
struct Cyc_Absyn_PointerType_struct*)_tmp400)->f1;_LL314: {int okay=1;if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp3FF.nullable,_tmp401.nullable)){void*_tmp42A=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp3FF.nullable))->v;int _tmp42B;_LL322: if(_tmp42A <= (void*)1?1:*((int*)_tmp42A)
!= 0)goto _LL324;_tmp42B=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp42A)->f1;
_LL323: okay=!_tmp42B;goto _LL321;_LL324:;_LL325:({void*_tmp42C[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("silent_castable conref not eq",
sizeof(char),30),_tag_arr(_tmp42C,sizeof(void*),0));});_LL321:;}if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp3FF.bounds,_tmp401.bounds)){struct _tuple6 _tmp42E=({
struct _tuple6 _tmp42D;_tmp42D.f1=(void*)(Cyc_Absyn_compress_conref(_tmp3FF.bounds))->v;
_tmp42D.f2=(void*)(Cyc_Absyn_compress_conref(_tmp401.bounds))->v;_tmp42D;});void*
_tmp42F;void*_tmp430;void*_tmp431;void*_tmp432;void*_tmp433;void*_tmp434;struct
Cyc_Absyn_Exp*_tmp435;void*_tmp436;void*_tmp437;struct Cyc_Absyn_Exp*_tmp438;void*
_tmp439;void*_tmp43A;void*_tmp43B;void*_tmp43C;_LL327: _tmp42F=_tmp42E.f1;if(
_tmp42F <= (void*)1?1:*((int*)_tmp42F)!= 0)goto _LL329;_tmp430=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp42F)->f1;if(_tmp430 <= (void*)1?1:*((int*)_tmp430)!= 0)goto _LL329;_tmp431=
_tmp42E.f2;if(_tmp431 <= (void*)1?1:*((int*)_tmp431)!= 0)goto _LL329;_tmp432=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp431)->f1;if((int)_tmp432 != 0)goto _LL329;
_LL328: okay=1;goto _LL326;_LL329: _tmp433=_tmp42E.f1;if(_tmp433 <= (void*)1?1:*((
int*)_tmp433)!= 0)goto _LL32B;_tmp434=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp433)->f1;if(_tmp434 <= (void*)1?1:*((int*)_tmp434)!= 0)goto _LL32B;_tmp435=((
struct Cyc_Absyn_Upper_b_struct*)_tmp434)->f1;_tmp436=_tmp42E.f2;if(_tmp436 <= (
void*)1?1:*((int*)_tmp436)!= 0)goto _LL32B;_tmp437=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp436)->f1;if(_tmp437 <= (void*)1?1:*((int*)_tmp437)!= 0)goto _LL32B;_tmp438=((
struct Cyc_Absyn_Upper_b_struct*)_tmp437)->f1;_LL32A: okay=okay?Cyc_Evexp_eval_const_uint_exp(
_tmp435)>= Cyc_Evexp_eval_const_uint_exp(_tmp438): 0;({void*_tmp43D[0]={};Cyc_Tcutil_warn(
loc,_tag_arr("implicit cast to shorter array",sizeof(char),31),_tag_arr(_tmp43D,
sizeof(void*),0));});goto _LL326;_LL32B: _tmp439=_tmp42E.f1;if(_tmp439 <= (void*)1?
1:*((int*)_tmp439)!= 0)goto _LL32D;_tmp43A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp439)->f1;if((int)_tmp43A != 0)goto _LL32D;_tmp43B=_tmp42E.f2;if(_tmp43B <= (
void*)1?1:*((int*)_tmp43B)!= 0)goto _LL32D;_tmp43C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp43B)->f1;if((int)_tmp43C != 0)goto _LL32D;_LL32C: okay=1;goto _LL326;_LL32D:;
_LL32E: okay=0;goto _LL326;_LL326:;}okay=okay?Cyc_Tcutil_unify((void*)_tmp3FF.elt_typ,(
void*)_tmp401.elt_typ): 0;okay=okay?Cyc_Tcutil_unify((void*)_tmp3FF.rgn_typ,(void*)
_tmp401.rgn_typ)?1: Cyc_Tcenv_region_outlives(te,(void*)_tmp3FF.rgn_typ,(void*)
_tmp401.rgn_typ): 0;okay=okay?!(_tmp3FF.tq).q_const?1:(_tmp401.tq).q_const: 0;
return okay;}_LL315: _tmp402=_tmp3FD.f1;if(_tmp402 <= (void*)3?1:*((int*)_tmp402)!= 
7)goto _LL317;_tmp403=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp402)->f1;
_tmp404=((struct Cyc_Absyn_ArrayType_struct*)_tmp402)->f2;_tmp405=((struct Cyc_Absyn_ArrayType_struct*)
_tmp402)->f3;_tmp406=_tmp3FD.f2;if(_tmp406 <= (void*)3?1:*((int*)_tmp406)!= 7)
goto _LL317;_tmp407=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp406)->f1;
_tmp408=((struct Cyc_Absyn_ArrayType_struct*)_tmp406)->f2;_tmp409=((struct Cyc_Absyn_ArrayType_struct*)
_tmp406)->f3;_LL316: {int okay;okay=(_tmp405 != 0?_tmp409 != 0: 0)?Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp405))== Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp409)): 0;return(okay?Cyc_Tcutil_unify(
_tmp403,_tmp407): 0)?!_tmp404.q_const?1: _tmp408.q_const: 0;}_LL317: _tmp40A=_tmp3FD.f1;
if(_tmp40A <= (void*)3?1:*((int*)_tmp40A)!= 3)goto _LL319;_tmp40B=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp40A)->f1;_tmp40C=(void*)_tmp40B.field_info;if(*((int*)_tmp40C)!= 1)goto
_LL319;_tmp40D=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp40C)->f1;_tmp40E=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp40C)->f2;_tmp40F=_tmp40B.targs;
_tmp410=_tmp3FD.f2;if(_tmp410 <= (void*)3?1:*((int*)_tmp410)!= 2)goto _LL319;
_tmp411=((struct Cyc_Absyn_TunionType_struct*)_tmp410)->f1;_tmp412=(void*)_tmp411.tunion_info;
if(*((int*)_tmp412)!= 1)goto _LL319;_tmp413=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp412)->f1;_tmp414=*_tmp413;_tmp415=_tmp411.targs;_LL318: if((_tmp40D == _tmp414?
1: Cyc_Absyn_qvar_cmp(_tmp40D->name,_tmp414->name)== 0)?_tmp40E->typs == 0: 0){for(
0;_tmp40F != 0?_tmp415 != 0: 0;(_tmp40F=_tmp40F->tl,_tmp415=_tmp415->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp40F->hd,(void*)_tmp415->hd))break;}if(_tmp40F == 0?_tmp415 == 0: 0)return
1;}return 0;_LL319: _tmp416=_tmp3FD.f1;if(_tmp416 <= (void*)3?1:*((int*)_tmp416)!= 
4)goto _LL31B;_tmp417=((struct Cyc_Absyn_PointerType_struct*)_tmp416)->f1;_tmp418=(
void*)_tmp417.elt_typ;_tmp419=(void*)_tmp417.rgn_typ;_tmp41A=_tmp417.nullable;
_tmp41B=_tmp417.tq;_tmp41C=_tmp417.bounds;_tmp41D=_tmp3FD.f2;if(_tmp41D <= (void*)
3?1:*((int*)_tmp41D)!= 2)goto _LL31B;_tmp41E=((struct Cyc_Absyn_TunionType_struct*)
_tmp41D)->f1;_tmp41F=(void*)_tmp41E.tunion_info;if(*((int*)_tmp41F)!= 1)goto
_LL31B;_tmp420=((struct Cyc_Absyn_KnownTunion_struct*)_tmp41F)->f1;_tmp421=*
_tmp420;_tmp422=_tmp41E.targs;_tmp423=(void*)_tmp41E.rgn;_LL31A:{void*_tmp43E=
Cyc_Tcutil_compress(_tmp418);struct Cyc_Absyn_TunionFieldInfo _tmp43F;void*_tmp440;
struct Cyc_Absyn_Tuniondecl*_tmp441;struct Cyc_Absyn_Tunionfield*_tmp442;struct Cyc_List_List*
_tmp443;_LL330: if(_tmp43E <= (void*)3?1:*((int*)_tmp43E)!= 3)goto _LL332;_tmp43F=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp43E)->f1;_tmp440=(void*)_tmp43F.field_info;
if(*((int*)_tmp440)!= 1)goto _LL332;_tmp441=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp440)->f1;_tmp442=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp440)->f2;
_tmp443=_tmp43F.targs;_LL331: if(!Cyc_Tcutil_unify(_tmp419,_tmp423)?!Cyc_Tcenv_region_outlives(
te,_tmp419,_tmp423): 0)return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp41A,((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(0)))return 0;if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp41C,Cyc_Absyn_new_conref(Cyc_Absyn_bounds_one)))return 0;
if(Cyc_Absyn_qvar_cmp(_tmp421->name,_tmp441->name)== 0?_tmp442->typs != 0: 0){int
okay=1;for(0;_tmp443 != 0?_tmp422 != 0: 0;(_tmp443=_tmp443->tl,_tmp422=_tmp422->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp443->hd,(void*)_tmp422->hd)){okay=0;break;}}if((
!okay?1: _tmp443 != 0)?1: _tmp422 != 0)return 0;return 1;}goto _LL32F;_LL332:;_LL333:
goto _LL32F;_LL32F:;}return 0;_LL31B: _tmp424=_tmp3FD.f1;if(_tmp424 <= (void*)3?1:*((
int*)_tmp424)!= 14)goto _LL31D;_tmp425=_tmp3FD.f2;if(_tmp425 <= (void*)3?1:*((int*)
_tmp425)!= 5)goto _LL31D;_tmp426=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp425)->f2;
if((int)_tmp426 != 2)goto _LL31D;_LL31C: return 1;_LL31D: _tmp427=_tmp3FD.f1;if(
_tmp427 <= (void*)3?1:*((int*)_tmp427)!= 17)goto _LL31F;_tmp428=_tmp3FD.f2;if(
_tmp428 <= (void*)3?1:*((int*)_tmp428)!= 5)goto _LL31F;_tmp429=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp428)->f2;if((int)_tmp429 != 2)goto _LL31F;_LL31E: return 1;_LL31F:;_LL320: return
Cyc_Tcutil_unify(t1,t2);_LL312:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*
_tmp444=Cyc_Tcutil_compress(t);_LL335: if(_tmp444 <= (void*)3?1:*((int*)_tmp444)!= 
4)goto _LL337;_LL336: return 1;_LL337:;_LL338: return 0;_LL334:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp445=(void*)e->r;void*_tmp446;int _tmp447;_LL33A:
if(*((int*)_tmp445)!= 0)goto _LL33C;_tmp446=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp445)->f1;if(_tmp446 <= (void*)1?1:*((int*)_tmp446)!= 2)goto _LL33C;_tmp447=((
struct Cyc_Absyn_Int_c_struct*)_tmp446)->f2;if(_tmp447 != 0)goto _LL33C;_LL33B:
return 1;_LL33C:;_LL33D: return 0;_LL339:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((
void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt
Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)
1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*
e1){if(Cyc_Tcutil_is_pointer_type(t2)?Cyc_Tcutil_is_zero(e1): 0){(void*)(e1->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp448=_cycalloc(sizeof(*_tmp448));
_tmp448[0]=({struct Cyc_Absyn_Const_e_struct _tmp449;_tmp449.tag=0;_tmp449.f1=(
void*)((void*)0);_tmp449;});_tmp448;})));{struct Cyc_List_List*_tmp44A=Cyc_Tcenv_lookup_type_vars(
te);struct Cyc_Absyn_PointerType_struct*_tmp44B=({struct Cyc_Absyn_PointerType_struct*
_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C[0]=({struct Cyc_Absyn_PointerType_struct
_tmp44D;_tmp44D.tag=4;_tmp44D.f1=({struct Cyc_Absyn_PtrInfo _tmp44E;_tmp44E.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->v=_tmp44A;_tmp450;}));_tmp44E.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F->v=_tmp44A;_tmp44F;}));_tmp44E.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp44E.tq=Cyc_Absyn_empty_tqual();
_tmp44E.bounds=Cyc_Absyn_empty_conref();_tmp44E;});_tmp44D;});_tmp44C;});(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp44B));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}static int Cyc_Tcutil_is_sizeof_type(void*t1){void*_tmp451=
Cyc_Tcutil_compress(t1);_LL33F: if(_tmp451 <= (void*)3?1:*((int*)_tmp451)!= 14)
goto _LL341;_LL340: return 1;_LL341:;_LL342: return 0;_LL33E:;}static int Cyc_Tcutil_is_tag_type(
void*t1){void*_tmp452=Cyc_Tcutil_compress(t1);_LL344: if(_tmp452 <= (void*)3?1:*((
int*)_tmp452)!= 17)goto _LL346;_LL345: return 1;_LL346:;_LL347: return 0;_LL343:;}int
Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
if(Cyc_Tcutil_unify(t1,t2))return 1;if(((Cyc_Tcutil_is_arithmetic_type(t1)?Cyc_Tcutil_is_arithmetic_type(
t2): 0)?1:(Cyc_Tcutil_is_sizeof_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0))?1:(
Cyc_Tcutil_is_tag_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0)){if(Cyc_Tcutil_will_lose_precision(
t1,t2))({struct Cyc_Std_String_pa_struct _tmp455;_tmp455.tag=0;_tmp455.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmp454;
_tmp454.tag=0;_tmp454.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmp453[2]={& _tmp454,& _tmp455};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; %s -> %s conversion supplied",
sizeof(char),53),_tag_arr(_tmp453,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if(Cyc_Tcutil_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);({struct Cyc_Std_String_pa_struct
_tmp458;_tmp458.tag=0;_tmp458.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp457;_tmp457.tag=0;_tmp457.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp456[2]={& _tmp457,& _tmp458};Cyc_Tcutil_warn(
e->loc,_tag_arr("implicit cast from %s to %s",sizeof(char),28),_tag_arr(_tmp456,
sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp459=Cyc_Tcutil_compress(t);
_LL349: if(_tmp459 <= (void*)3?1:*((int*)_tmp459)!= 5)goto _LL34B;_LL34A: goto _LL34C;
_LL34B: if((int)_tmp459 != 1)goto _LL34D;_LL34C: goto _LL34E;_LL34D: if(_tmp459 <= (
void*)3?1:*((int*)_tmp459)!= 6)goto _LL34F;_LL34E: return 1;_LL34F:;_LL350: return 0;
_LL348:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*inst,
struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp45A=_cycalloc(sizeof(*
_tmp45A));_tmp45A->f1=x->tq;_tmp45A->f2=Cyc_Tcutil_substitute(inst,(void*)x->type);
_tmp45A;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp45B=t1;struct Cyc_List_List*
_tmp45C;struct Cyc_Absyn_AggrInfo _tmp45D;void*_tmp45E;struct Cyc_Absyn_Aggrdecl**
_tmp45F;struct Cyc_Absyn_Aggrdecl*_tmp460;struct Cyc_List_List*_tmp461;void*
_tmp462;struct Cyc_List_List*_tmp463;_LL352: if((int)_tmp45B != 0)goto _LL354;_LL353:
return 0;_LL354: if(_tmp45B <= (void*)3?1:*((int*)_tmp45B)!= 9)goto _LL356;_tmp45C=((
struct Cyc_Absyn_TupleType_struct*)_tmp45B)->f1;_LL355: return _tmp45C;_LL356: if(
_tmp45B <= (void*)3?1:*((int*)_tmp45B)!= 10)goto _LL358;_tmp45D=((struct Cyc_Absyn_AggrType_struct*)
_tmp45B)->f1;_tmp45E=(void*)_tmp45D.aggr_info;if(*((int*)_tmp45E)!= 1)goto _LL358;
_tmp45F=((struct Cyc_Absyn_KnownAggr_struct*)_tmp45E)->f1;_tmp460=*_tmp45F;
_tmp461=_tmp45D.targs;_LL357: if((void*)_tmp460->kind == (void*)1?1: _tmp460->impl
== 0)return({struct Cyc_List_List*_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464->hd=({
struct _tuple4*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->f1=Cyc_Absyn_empty_tqual();
_tmp465->f2=t1;_tmp465;});_tmp464->tl=0;_tmp464;});{struct Cyc_List_List*_tmp466=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp460->tvs,_tmp461);return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp466,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp460->impl))->fields);}_LL358: if(_tmp45B <= (void*)3?1:*((int*)
_tmp45B)!= 11)goto _LL35A;_tmp462=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp45B)->f1;if((int)_tmp462 != 0)goto _LL35A;_tmp463=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp45B)->f2;_LL359: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp463);_LL35A:;_LL35B: return({struct Cyc_List_List*
_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->hd=({struct _tuple4*_tmp468=
_cycalloc(sizeof(*_tmp468));_tmp468->f1=Cyc_Absyn_empty_tqual();_tmp468->f2=t1;
_tmp468;});_tmp467->tl=0;_tmp467;});_LL351:;}}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1)?Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2):
0)return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp46A=({struct _tuple6 _tmp469;_tmp469.f1=t1;_tmp469.f2=t2;_tmp469;});void*
_tmp46B;struct Cyc_Absyn_PtrInfo _tmp46C;void*_tmp46D;void*_tmp46E;struct Cyc_Absyn_Conref*
_tmp46F;struct Cyc_Absyn_Tqual _tmp470;struct Cyc_Absyn_Conref*_tmp471;void*_tmp472;
struct Cyc_Absyn_PtrInfo _tmp473;void*_tmp474;void*_tmp475;struct Cyc_Absyn_Conref*
_tmp476;struct Cyc_Absyn_Tqual _tmp477;struct Cyc_Absyn_Conref*_tmp478;_LL35D:
_tmp46B=_tmp46A.f1;if(_tmp46B <= (void*)3?1:*((int*)_tmp46B)!= 4)goto _LL35F;
_tmp46C=((struct Cyc_Absyn_PointerType_struct*)_tmp46B)->f1;_tmp46D=(void*)
_tmp46C.elt_typ;_tmp46E=(void*)_tmp46C.rgn_typ;_tmp46F=_tmp46C.nullable;_tmp470=
_tmp46C.tq;_tmp471=_tmp46C.bounds;_tmp472=_tmp46A.f2;if(_tmp472 <= (void*)3?1:*((
int*)_tmp472)!= 4)goto _LL35F;_tmp473=((struct Cyc_Absyn_PointerType_struct*)
_tmp472)->f1;_tmp474=(void*)_tmp473.elt_typ;_tmp475=(void*)_tmp473.rgn_typ;
_tmp476=_tmp473.nullable;_tmp477=_tmp473.tq;_tmp478=_tmp473.bounds;_LL35E: if(
_tmp470.q_const?!_tmp477.q_const: 0)return 0;if((!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp46F,_tmp476)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp46F): 0)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp476): 0)
return 0;if(!Cyc_Tcutil_unify(_tmp46E,_tmp475)?!Cyc_Tcenv_region_outlives(te,
_tmp46E,_tmp475): 0)return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp471,_tmp478)){struct _tuple6 _tmp47A=({struct _tuple6 _tmp479;_tmp479.f1=Cyc_Absyn_conref_val(
_tmp471);_tmp479.f2=Cyc_Absyn_conref_val(_tmp478);_tmp479;});void*_tmp47B;void*
_tmp47C;void*_tmp47D;struct Cyc_Absyn_Exp*_tmp47E;void*_tmp47F;struct Cyc_Absyn_Exp*
_tmp480;_LL362: _tmp47B=_tmp47A.f1;if(_tmp47B <= (void*)1?1:*((int*)_tmp47B)!= 0)
goto _LL364;_tmp47C=_tmp47A.f2;if((int)_tmp47C != 0)goto _LL364;_LL363: goto _LL361;
_LL364: _tmp47D=_tmp47A.f1;if(_tmp47D <= (void*)1?1:*((int*)_tmp47D)!= 0)goto
_LL366;_tmp47E=((struct Cyc_Absyn_Upper_b_struct*)_tmp47D)->f1;_tmp47F=_tmp47A.f2;
if(_tmp47F <= (void*)1?1:*((int*)_tmp47F)!= 0)goto _LL366;_tmp480=((struct Cyc_Absyn_Upper_b_struct*)
_tmp47F)->f1;_LL365: if(Cyc_Evexp_eval_const_uint_exp(_tmp47E)< Cyc_Evexp_eval_const_uint_exp(
_tmp480))return 0;goto _LL361;_LL366:;_LL367: return 0;_LL361:;}return Cyc_Tcutil_ptrsubtype(
te,({struct Cyc_List_List*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->hd=({
struct _tuple6*_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482->f1=t1;_tmp482->f2=t2;
_tmp482;});_tmp481->tl=assume;_tmp481;}),_tmp46D,_tmp474);_LL35F:;_LL360: return 0;
_LL35C:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp484=({
struct _tuple6 _tmp483;_tmp483.f1=Cyc_Tcutil_compress(t1);_tmp483.f2=Cyc_Tcutil_compress(
t2);_tmp483;});void*_tmp485;void*_tmp486;void*_tmp487;void*_tmp488;_LL369:
_tmp485=_tmp484.f1;if(_tmp485 <= (void*)3?1:*((int*)_tmp485)!= 5)goto _LL36B;
_tmp486=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp485)->f2;_tmp487=_tmp484.f2;
if(_tmp487 <= (void*)3?1:*((int*)_tmp487)!= 5)goto _LL36B;_tmp488=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp487)->f2;_LL36A: return _tmp486 == _tmp488;_LL36B:;
_LL36C: return 0;_LL368:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp48A;struct
Cyc_Absyn_Tqual _tmp48B;void*_tmp48C;struct _tuple4*_tmp489=(struct _tuple4*)tqs1->hd;
_tmp48A=*_tmp489;_tmp48B=_tmp48A.f1;_tmp48C=_tmp48A.f2;{struct _tuple4 _tmp48E;
struct Cyc_Absyn_Tqual _tmp48F;void*_tmp490;struct _tuple4*_tmp48D=(struct _tuple4*)
tqs2->hd;_tmp48E=*_tmp48D;_tmp48F=_tmp48E.f1;_tmp490=_tmp48E.f2;if(_tmp48F.q_const?
Cyc_Tcutil_subtype(te,assume,_tmp48C,_tmp490): 0)continue;else{if(Cyc_Tcutil_unify(
_tmp48C,_tmp490))continue;else{if(Cyc_Tcutil_isomorphic(_tmp48C,_tmp490))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp491=Cyc_Tcutil_compress(t);void*_tmp492;_LL36E: if(_tmp491 <= (void*)3?1:*((
int*)_tmp491)!= 5)goto _LL370;_tmp492=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp491)->f2;if((int)_tmp492 != 0)goto _LL370;_LL36F: return 1;_LL370:;_LL371: return
0;_LL36D:;}int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{void*_tmp493=t2;void*_tmp494;_LL373: if(_tmp493 <= (
void*)3?1:*((int*)_tmp493)!= 5)goto _LL375;_tmp494=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp493)->f2;if((int)_tmp494 != 2)goto _LL375;_LL374: if(Cyc_Tcutil_typ_kind(t2)== (
void*)2)return 1;goto _LL372;_LL375:;_LL376: goto _LL372;_LL372:;}{void*_tmp495=t1;
struct Cyc_Absyn_PtrInfo _tmp496;void*_tmp497;void*_tmp498;struct Cyc_Absyn_Conref*
_tmp499;struct Cyc_Absyn_Tqual _tmp49A;struct Cyc_Absyn_Conref*_tmp49B;void*_tmp49C;
struct Cyc_Absyn_Tqual _tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Enumdecl*
_tmp49F;_LL378: if(_tmp495 <= (void*)3?1:*((int*)_tmp495)!= 4)goto _LL37A;_tmp496=((
struct Cyc_Absyn_PointerType_struct*)_tmp495)->f1;_tmp497=(void*)_tmp496.elt_typ;
_tmp498=(void*)_tmp496.rgn_typ;_tmp499=_tmp496.nullable;_tmp49A=_tmp496.tq;
_tmp49B=_tmp496.bounds;_LL379:{void*_tmp4A0=t2;struct Cyc_Absyn_PtrInfo _tmp4A1;
void*_tmp4A2;void*_tmp4A3;struct Cyc_Absyn_Conref*_tmp4A4;struct Cyc_Absyn_Tqual
_tmp4A5;struct Cyc_Absyn_Conref*_tmp4A6;_LL389: if(_tmp4A0 <= (void*)3?1:*((int*)
_tmp4A0)!= 4)goto _LL38B;_tmp4A1=((struct Cyc_Absyn_PointerType_struct*)_tmp4A0)->f1;
_tmp4A2=(void*)_tmp4A1.elt_typ;_tmp4A3=(void*)_tmp4A1.rgn_typ;_tmp4A4=_tmp4A1.nullable;
_tmp4A5=_tmp4A1.tq;_tmp4A6=_tmp4A1.bounds;_LL38A: {struct Cyc_List_List*_tmp4A7=({
struct Cyc_List_List*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0->hd=({struct
_tuple6*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->f1=t1;_tmp4B1->f2=t2;
_tmp4B1;});_tmp4B0->tl=0;_tmp4B0;});int _tmp4A8=Cyc_Tcutil_ptrsubtype(te,_tmp4A7,
_tmp497,_tmp4A2)?!_tmp49A.q_const?1: _tmp4A5.q_const: 0;int _tmp4A9=_tmp4A8?0:((Cyc_Tcutil_bits_only(
_tmp497)?Cyc_Tcutil_is_char_type(_tmp4A2): 0)?_tmp4A5.q_const?1: !_tmp49A.q_const:
0);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp49B,_tmp4A6);
if(!bounds_ok?!_tmp4A9: 0){struct _tuple6 _tmp4AB=({struct _tuple6 _tmp4AA;_tmp4AA.f1=
Cyc_Absyn_conref_val(_tmp49B);_tmp4AA.f2=Cyc_Absyn_conref_val(_tmp4A6);_tmp4AA;});
void*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AD;void*_tmp4AE;struct Cyc_Absyn_Exp*
_tmp4AF;_LL390: _tmp4AC=_tmp4AB.f1;if(_tmp4AC <= (void*)1?1:*((int*)_tmp4AC)!= 0)
goto _LL392;_tmp4AD=((struct Cyc_Absyn_Upper_b_struct*)_tmp4AC)->f1;_tmp4AE=
_tmp4AB.f2;if(_tmp4AE <= (void*)1?1:*((int*)_tmp4AE)!= 0)goto _LL392;_tmp4AF=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4AE)->f1;_LL391: if(Cyc_Evexp_eval_const_uint_exp(
_tmp4AD)>= Cyc_Evexp_eval_const_uint_exp(_tmp4AF))bounds_ok=1;goto _LL38F;_LL392:;
_LL393: bounds_ok=1;goto _LL38F;_LL38F:;}return(bounds_ok?_tmp4A8?1: _tmp4A9: 0)?Cyc_Tcutil_unify(
_tmp498,_tmp4A3)?1: Cyc_Tcenv_region_outlives(te,_tmp498,_tmp4A3): 0;}_LL38B: if((
int)_tmp4A0 != 0)goto _LL38D;_LL38C: return 1;_LL38D:;_LL38E: goto _LL388;_LL388:;}
return 0;_LL37A: if(_tmp495 <= (void*)3?1:*((int*)_tmp495)!= 7)goto _LL37C;_tmp49C=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp495)->f1;_tmp49D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp495)->f2;_tmp49E=((struct Cyc_Absyn_ArrayType_struct*)_tmp495)->f3;_LL37B:{
void*_tmp4B2=t2;void*_tmp4B3;struct Cyc_Absyn_Tqual _tmp4B4;struct Cyc_Absyn_Exp*
_tmp4B5;_LL395: if(_tmp4B2 <= (void*)3?1:*((int*)_tmp4B2)!= 7)goto _LL397;_tmp4B3=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp4B2)->f1;_tmp4B4=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4B2)->f2;_tmp4B5=((struct Cyc_Absyn_ArrayType_struct*)_tmp4B2)->f3;_LL396: {
int okay;okay=(_tmp49E != 0?_tmp4B5 != 0: 0)?Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp49E))>= Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp4B5)): 0;return(okay?Cyc_Tcutil_unify(_tmp49C,_tmp4B3): 0)?!
_tmp49D.q_const?1: _tmp4B4.q_const: 0;}_LL397:;_LL398: return 0;_LL394:;}return 0;
_LL37C: if(_tmp495 <= (void*)3?1:*((int*)_tmp495)!= 12)goto _LL37E;_tmp49F=((struct
Cyc_Absyn_EnumType_struct*)_tmp495)->f2;_LL37D:{void*_tmp4B6=t2;struct Cyc_Absyn_Enumdecl*
_tmp4B7;_LL39A: if(_tmp4B6 <= (void*)3?1:*((int*)_tmp4B6)!= 12)goto _LL39C;_tmp4B7=((
struct Cyc_Absyn_EnumType_struct*)_tmp4B6)->f2;_LL39B: if((_tmp49F->fields != 0?
_tmp4B7->fields != 0: 0)?((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp49F->fields))->v)>= ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B7->fields))->v): 0)return 1;goto _LL399;_LL39C:;_LL39D: goto _LL399;
_LL399:;}goto _LL37F;_LL37E: if(_tmp495 <= (void*)3?1:*((int*)_tmp495)!= 5)goto
_LL380;_LL37F: goto _LL381;_LL380: if((int)_tmp495 != 1)goto _LL382;_LL381: goto _LL383;
_LL382: if(_tmp495 <= (void*)3?1:*((int*)_tmp495)!= 6)goto _LL384;_LL383: return Cyc_Tcutil_coerceable(
t2)?1: t2 == (void*)0;_LL384: if((int)_tmp495 != 0)goto _LL386;_LL385: return t2 == (
void*)0;_LL386:;_LL387: return 0;_LL377:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp4B8=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp4B9=_cycalloc(
sizeof(*_tmp4B9));_tmp4B9[0]=({struct Cyc_Absyn_Cast_e_struct _tmp4BA;_tmp4BA.tag=
13;_tmp4BA.f1=(void*)t;_tmp4BA.f2=_tmp4B8;_tmp4BA;});_tmp4B9;})));e->topt=({
struct Cyc_Core_Opt*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB->v=(void*)t;
_tmp4BB;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*_tmp4BC=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL39F: if(_tmp4BC <= (void*)
3?1:*((int*)_tmp4BC)!= 5)goto _LL3A1;_LL3A0: goto _LL3A2;_LL3A1: if(_tmp4BC <= (void*)
3?1:*((int*)_tmp4BC)!= 12)goto _LL3A3;_LL3A2: goto _LL3A4;_LL3A3: if(_tmp4BC <= (void*)
3?1:*((int*)_tmp4BC)!= 13)goto _LL3A5;_LL3A4: goto _LL3A6;_LL3A5: if(_tmp4BC <= (void*)
3?1:*((int*)_tmp4BC)!= 17)goto _LL3A7;_LL3A6: goto _LL3A8;_LL3A7: if(_tmp4BC <= (void*)
3?1:*((int*)_tmp4BC)!= 14)goto _LL3A9;_LL3A8: return 1;_LL3A9: if(_tmp4BC <= (void*)3?
1:*((int*)_tmp4BC)!= 0)goto _LL3AB;_LL3AA: return Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3AB:;_LL3AC: return
0;_LL39E:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp4BD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3AE: if((int)_tmp4BD != 1)goto _LL3B0;_LL3AF: goto
_LL3B1;_LL3B0: if(_tmp4BD <= (void*)3?1:*((int*)_tmp4BD)!= 6)goto _LL3B2;_LL3B1:
return 1;_LL3B2:;_LL3B3: return 0;_LL3AD:;}}int Cyc_Tcutil_is_function_type(void*t){
void*_tmp4BE=Cyc_Tcutil_compress(t);_LL3B5: if(_tmp4BE <= (void*)3?1:*((int*)
_tmp4BE)!= 8)goto _LL3B7;_LL3B6: return 1;_LL3B7:;_LL3B8: return 0;_LL3B4:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple6 _tmp4C0=({struct _tuple6 _tmp4BF;_tmp4BF.f1=t1;
_tmp4BF.f2=t2;_tmp4BF;});void*_tmp4C1;int _tmp4C2;void*_tmp4C3;int _tmp4C4;void*
_tmp4C5;void*_tmp4C6;void*_tmp4C7;void*_tmp4C8;void*_tmp4C9;void*_tmp4CA;void*
_tmp4CB;void*_tmp4CC;void*_tmp4CD;void*_tmp4CE;void*_tmp4CF;void*_tmp4D0;void*
_tmp4D1;void*_tmp4D2;void*_tmp4D3;void*_tmp4D4;void*_tmp4D5;void*_tmp4D6;void*
_tmp4D7;void*_tmp4D8;void*_tmp4D9;void*_tmp4DA;_LL3BA: _tmp4C1=_tmp4C0.f1;if(
_tmp4C1 <= (void*)3?1:*((int*)_tmp4C1)!= 6)goto _LL3BC;_tmp4C2=((struct Cyc_Absyn_DoubleType_struct*)
_tmp4C1)->f1;_tmp4C3=_tmp4C0.f2;if(_tmp4C3 <= (void*)3?1:*((int*)_tmp4C3)!= 6)
goto _LL3BC;_tmp4C4=((struct Cyc_Absyn_DoubleType_struct*)_tmp4C3)->f1;_LL3BB: if(
_tmp4C2)return t1;else{return t2;}_LL3BC: _tmp4C5=_tmp4C0.f1;if(_tmp4C5 <= (void*)3?
1:*((int*)_tmp4C5)!= 6)goto _LL3BE;_LL3BD: return t1;_LL3BE: _tmp4C6=_tmp4C0.f2;if(
_tmp4C6 <= (void*)3?1:*((int*)_tmp4C6)!= 6)goto _LL3C0;_LL3BF: return t2;_LL3C0:
_tmp4C7=_tmp4C0.f1;if((int)_tmp4C7 != 1)goto _LL3C2;_LL3C1: goto _LL3C3;_LL3C2:
_tmp4C8=_tmp4C0.f2;if((int)_tmp4C8 != 1)goto _LL3C4;_LL3C3: return(void*)1;_LL3C4:
_tmp4C9=_tmp4C0.f1;if(_tmp4C9 <= (void*)3?1:*((int*)_tmp4C9)!= 5)goto _LL3C6;
_tmp4CA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C9)->f1;if((int)_tmp4CA != 
1)goto _LL3C6;_tmp4CB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C9)->f2;if((
int)_tmp4CB != 3)goto _LL3C6;_LL3C5: goto _LL3C7;_LL3C6: _tmp4CC=_tmp4C0.f2;if(
_tmp4CC <= (void*)3?1:*((int*)_tmp4CC)!= 5)goto _LL3C8;_tmp4CD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4CC)->f1;if((int)_tmp4CD != 1)goto _LL3C8;_tmp4CE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4CC)->f2;if((int)_tmp4CE != 3)goto _LL3C8;_LL3C7: return Cyc_Absyn_ulonglong_typ;
_LL3C8: _tmp4CF=_tmp4C0.f1;if(_tmp4CF <= (void*)3?1:*((int*)_tmp4CF)!= 5)goto
_LL3CA;_tmp4D0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4CF)->f1;if((int)
_tmp4D0 != 0)goto _LL3CA;_tmp4D1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4CF)->f2;
if((int)_tmp4D1 != 3)goto _LL3CA;_LL3C9: goto _LL3CB;_LL3CA: _tmp4D2=_tmp4C0.f2;if(
_tmp4D2 <= (void*)3?1:*((int*)_tmp4D2)!= 5)goto _LL3CC;_tmp4D3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4D2)->f1;if((int)_tmp4D3 != 0)goto _LL3CC;_tmp4D4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4D2)->f2;if((int)_tmp4D4 != 3)goto _LL3CC;_LL3CB: return Cyc_Absyn_slonglong_typ;
_LL3CC: _tmp4D5=_tmp4C0.f1;if(_tmp4D5 <= (void*)3?1:*((int*)_tmp4D5)!= 5)goto
_LL3CE;_tmp4D6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4D5)->f1;if((int)
_tmp4D6 != 1)goto _LL3CE;_tmp4D7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4D5)->f2;
if((int)_tmp4D7 != 2)goto _LL3CE;_LL3CD: goto _LL3CF;_LL3CE: _tmp4D8=_tmp4C0.f2;if(
_tmp4D8 <= (void*)3?1:*((int*)_tmp4D8)!= 5)goto _LL3D0;_tmp4D9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4D8)->f1;if((int)_tmp4D9 != 1)goto _LL3D0;_tmp4DA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4D8)->f2;if((int)_tmp4DA != 2)goto _LL3D0;_LL3CF: return Cyc_Absyn_uint_typ;
_LL3D0:;_LL3D1: return Cyc_Absyn_sint_typ;_LL3B9:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp4DB=(void*)e->r;struct Cyc_Core_Opt*_tmp4DC;
_LL3D3: if(*((int*)_tmp4DB)!= 4)goto _LL3D5;_tmp4DC=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4DB)->f2;if(_tmp4DC != 0)goto _LL3D5;_LL3D4:({void*_tmp4DD[0]={};Cyc_Tcutil_warn(
e->loc,_tag_arr("assignment in test",sizeof(char),19),_tag_arr(_tmp4DD,sizeof(
void*),0));});goto _LL3D2;_LL3D5:;_LL3D6: goto _LL3D2;_LL3D2:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp4DF=({struct _tuple6 _tmp4DE;_tmp4DE.f1=c1;_tmp4DE.f2=c2;_tmp4DE;});
void*_tmp4E0;void*_tmp4E1;void*_tmp4E2;void*_tmp4E3;void*_tmp4E4;struct Cyc_Core_Opt*
_tmp4E5;struct Cyc_Core_Opt**_tmp4E6;void*_tmp4E7;struct Cyc_Core_Opt*_tmp4E8;
struct Cyc_Core_Opt**_tmp4E9;void*_tmp4EA;struct Cyc_Core_Opt*_tmp4EB;struct Cyc_Core_Opt**
_tmp4EC;void*_tmp4ED;void*_tmp4EE;void*_tmp4EF;void*_tmp4F0;void*_tmp4F1;void*
_tmp4F2;struct Cyc_Core_Opt*_tmp4F3;struct Cyc_Core_Opt**_tmp4F4;void*_tmp4F5;void*
_tmp4F6;struct Cyc_Core_Opt*_tmp4F7;struct Cyc_Core_Opt**_tmp4F8;void*_tmp4F9;void*
_tmp4FA;struct Cyc_Core_Opt*_tmp4FB;struct Cyc_Core_Opt**_tmp4FC;void*_tmp4FD;
_LL3D8: _tmp4E0=_tmp4DF.f1;if(*((int*)_tmp4E0)!= 0)goto _LL3DA;_tmp4E1=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp4E0)->f1;_tmp4E2=_tmp4DF.f2;if(*((int*)_tmp4E2)
!= 0)goto _LL3DA;_tmp4E3=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4E2)->f1;
_LL3D9: return _tmp4E1 == _tmp4E3;_LL3DA: _tmp4E4=_tmp4DF.f2;if(*((int*)_tmp4E4)!= 1)
goto _LL3DC;_tmp4E5=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4E4)->f1;_tmp4E6=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4E4)->f1;_LL3DB:*
_tmp4E6=({struct Cyc_Core_Opt*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->v=(
void*)c1;_tmp4FE;});return 1;_LL3DC: _tmp4E7=_tmp4DF.f1;if(*((int*)_tmp4E7)!= 1)
goto _LL3DE;_tmp4E8=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4E7)->f1;_tmp4E9=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4E7)->f1;_LL3DD:*
_tmp4E9=({struct Cyc_Core_Opt*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->v=(
void*)c2;_tmp4FF;});return 1;_LL3DE: _tmp4EA=_tmp4DF.f1;if(*((int*)_tmp4EA)!= 2)
goto _LL3E0;_tmp4EB=((struct Cyc_Absyn_Less_kb_struct*)_tmp4EA)->f1;_tmp4EC=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp4EA)->f1;_tmp4ED=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4EA)->f2;_tmp4EE=_tmp4DF.f2;if(*((
int*)_tmp4EE)!= 0)goto _LL3E0;_tmp4EF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp4EE)->f1;_LL3DF: if(Cyc_Tcutil_kind_leq(_tmp4EF,_tmp4ED)){*_tmp4EC=({struct
Cyc_Core_Opt*_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->v=(void*)c2;_tmp500;});
return 1;}else{return 0;}_LL3E0: _tmp4F0=_tmp4DF.f1;if(*((int*)_tmp4F0)!= 0)goto
_LL3E2;_tmp4F1=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4F0)->f1;_tmp4F2=
_tmp4DF.f2;if(*((int*)_tmp4F2)!= 2)goto _LL3E2;_tmp4F3=((struct Cyc_Absyn_Less_kb_struct*)
_tmp4F2)->f1;_tmp4F4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp4F2)->f1;_tmp4F5=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4F2)->f2;
_LL3E1: if(Cyc_Tcutil_kind_leq(_tmp4F1,_tmp4F5)){*_tmp4F4=({struct Cyc_Core_Opt*
_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501->v=(void*)c1;_tmp501;});return 1;}
else{return 0;}_LL3E2: _tmp4F6=_tmp4DF.f1;if(*((int*)_tmp4F6)!= 2)goto _LL3D7;
_tmp4F7=((struct Cyc_Absyn_Less_kb_struct*)_tmp4F6)->f1;_tmp4F8=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4F6)->f1;_tmp4F9=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4F6)->f2;_tmp4FA=_tmp4DF.f2;if(*((int*)_tmp4FA)!= 2)goto _LL3D7;_tmp4FB=((
struct Cyc_Absyn_Less_kb_struct*)_tmp4FA)->f1;_tmp4FC=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4FA)->f1;_tmp4FD=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4FA)->f2;_LL3E3: if(Cyc_Tcutil_kind_leq(_tmp4F9,_tmp4FD)){*_tmp4FC=({struct
Cyc_Core_Opt*_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->v=(void*)c1;_tmp502;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp4FD,_tmp4F9)){*_tmp4F8=({struct Cyc_Core_Opt*
_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503->v=(void*)c2;_tmp503;});return 1;}
else{return 0;}}_LL3D7:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp504=_cycalloc_atomic(sizeof(*_tmp504));_tmp504[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp504;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp508;_tmp508.tag=1;_tmp508.f1=(int)((unsigned int)
i);{void*_tmp507[1]={& _tmp508};Cyc_Std_aprintf(_tag_arr("#%d",sizeof(char),4),
_tag_arr(_tmp507,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*_tmp505=
_cycalloc(sizeof(*_tmp505));_tmp505->name=({struct _tagged_arr*_tmp506=_cycalloc(
sizeof(struct _tagged_arr)* 1);_tmp506[0]=s;_tmp506;});_tmp505->identity=0;
_tmp505->kind=(void*)k;_tmp505;});}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _tagged_arr _tmp509=*t->name;return*((const char*)
_check_unknown_subscript(_tmp509,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_Std_String_pa_struct _tmp50B;_tmp50B.tag=0;
_tmp50B.f1=(struct _tagged_arr)*t->name;{void*_tmp50A[1]={& _tmp50B};Cyc_Std_printf(
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp50A,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp50C=Cyc_Std_strconcat(_tag_arr("`",sizeof(char),
2),*t->name);*((char*)_check_unknown_subscript(_tmp50C,sizeof(char),1))='t';t->name=({
struct _tagged_arr*_tmp50D=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp50D[0]=(
struct _tagged_arr)_tmp50C;_tmp50D;});}}struct _tuple14{struct _tagged_arr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple14*x){return({struct _tuple2*_tmp50E=_cycalloc(sizeof(*_tmp50E));
_tmp50E->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp50F=_cycalloc(sizeof(*
_tmp50F));_tmp50F->v=(*x).f1;_tmp50F;});_tmp50E->f2=(*x).f2;_tmp50E->f3=(*x).f3;
_tmp50E;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp510=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp510=({
struct Cyc_List_List*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->hd=(void*)((
void*)atts->hd);_tmp511->tl=_tmp510;_tmp511;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512[0]=({struct Cyc_Absyn_FnType_struct
_tmp513;_tmp513.tag=8;_tmp513.f1=({struct Cyc_Absyn_FnInfo _tmp514;_tmp514.tvars=
fd->tvs;_tmp514.effect=fd->effect;_tmp514.ret_typ=(void*)((void*)fd->ret_type);
_tmp514.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp514.c_varargs=
fd->c_varargs;_tmp514.cyc_varargs=fd->cyc_varargs;_tmp514.rgn_po=fd->rgn_po;
_tmp514.attributes=_tmp510;_tmp514;});_tmp513;});_tmp512;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple15{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple15*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515->f1=(*pr).f1;_tmp515->f2=t;_tmp515;});}
struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple17{
struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple17*_tmp516=_region_malloc(
rgn,sizeof(*_tmp516));_tmp516->f1=({struct _tuple16*_tmp517=_region_malloc(rgn,
sizeof(*_tmp517));_tmp517->f1=(*y).f1;_tmp517->f2=(*y).f2;_tmp517;});_tmp516->f2=(*
y).f3;_tmp516;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple17*w){
struct _tuple16*_tmp519;void*_tmp51A;struct _tuple17 _tmp518=*w;_tmp519=_tmp518.f1;
_tmp51A=_tmp518.f2;{struct Cyc_Core_Opt*_tmp51C;struct Cyc_Absyn_Tqual _tmp51D;
struct _tuple16 _tmp51B=*_tmp519;_tmp51C=_tmp51B.f1;_tmp51D=_tmp51B.f2;return({
struct _tuple2*_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->f1=_tmp51C;_tmp51E->f2=
_tmp51D;_tmp51E->f3=_tmp51A;_tmp51E;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp51F=
_cycalloc(sizeof(*_tmp51F));_tmp51F->name=f->name;_tmp51F->tq=f->tq;_tmp51F->type=(
void*)t;_tmp51F->width=f->width;_tmp51F->attributes=f->attributes;_tmp51F;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp520=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp521;struct Cyc_Absyn_AggrInfo _tmp522;void*_tmp523;struct Cyc_List_List*_tmp524;
struct Cyc_Absyn_TunionInfo _tmp525;void*_tmp526;struct Cyc_List_List*_tmp527;void*
_tmp528;struct Cyc_Absyn_TunionFieldInfo _tmp529;void*_tmp52A;struct Cyc_List_List*
_tmp52B;struct _tuple1*_tmp52C;struct Cyc_List_List*_tmp52D;struct Cyc_Absyn_Typedefdecl*
_tmp52E;struct Cyc_Core_Opt*_tmp52F;void*_tmp530;struct Cyc_Absyn_Tqual _tmp531;
struct Cyc_Absyn_Exp*_tmp532;struct Cyc_Absyn_PtrInfo _tmp533;void*_tmp534;void*
_tmp535;struct Cyc_Absyn_Conref*_tmp536;struct Cyc_Absyn_Tqual _tmp537;struct Cyc_Absyn_Conref*
_tmp538;struct Cyc_Absyn_FnInfo _tmp539;struct Cyc_List_List*_tmp53A;struct Cyc_Core_Opt*
_tmp53B;void*_tmp53C;struct Cyc_List_List*_tmp53D;int _tmp53E;struct Cyc_Absyn_VarargInfo*
_tmp53F;struct Cyc_List_List*_tmp540;struct Cyc_List_List*_tmp541;struct Cyc_List_List*
_tmp542;void*_tmp543;struct Cyc_List_List*_tmp544;struct Cyc_Core_Opt*_tmp545;void*
_tmp546;void*_tmp547;void*_tmp548;void*_tmp549;void*_tmp54A;struct Cyc_List_List*
_tmp54B;_LL3E5: if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 1)goto _LL3E7;_tmp521=((
struct Cyc_Absyn_VarType_struct*)_tmp520)->f1;_LL3E6: {struct _handler_cons _tmp54C;
_push_handler(& _tmp54C);{int _tmp54E=0;if(setjmp(_tmp54C.handler))_tmp54E=1;if(!
_tmp54E){{void*_tmp54F=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp521);_npop_handler(0);return _tmp54F;};_pop_handler();}else{void*_tmp54D=(
void*)_exn_thrown;void*_tmp551=_tmp54D;_LL418: if(_tmp551 != Cyc_Core_Not_found)
goto _LL41A;_LL419: return t;_LL41A:;_LL41B:(void)_throw(_tmp551);_LL417:;}}}_LL3E7:
if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 10)goto _LL3E9;_tmp522=((struct Cyc_Absyn_AggrType_struct*)
_tmp520)->f1;_tmp523=(void*)_tmp522.aggr_info;_tmp524=_tmp522.targs;_LL3E8: {
struct Cyc_List_List*_tmp552=Cyc_Tcutil_substs(rgn,inst,_tmp524);return _tmp552 == 
_tmp524?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp553=_cycalloc(sizeof(*
_tmp553));_tmp553[0]=({struct Cyc_Absyn_AggrType_struct _tmp554;_tmp554.tag=10;
_tmp554.f1=({struct Cyc_Absyn_AggrInfo _tmp555;_tmp555.aggr_info=(void*)_tmp523;
_tmp555.targs=_tmp552;_tmp555;});_tmp554;});_tmp553;});}_LL3E9: if(_tmp520 <= (
void*)3?1:*((int*)_tmp520)!= 2)goto _LL3EB;_tmp525=((struct Cyc_Absyn_TunionType_struct*)
_tmp520)->f1;_tmp526=(void*)_tmp525.tunion_info;_tmp527=_tmp525.targs;_tmp528=(
void*)_tmp525.rgn;_LL3EA: {struct Cyc_List_List*_tmp556=Cyc_Tcutil_substs(rgn,
inst,_tmp527);void*_tmp557=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp528);return(
_tmp556 == _tmp527?_tmp557 == _tmp528: 0)?t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558[0]=({struct Cyc_Absyn_TunionType_struct
_tmp559;_tmp559.tag=2;_tmp559.f1=({struct Cyc_Absyn_TunionInfo _tmp55A;_tmp55A.tunion_info=(
void*)_tmp526;_tmp55A.targs=_tmp556;_tmp55A.rgn=(void*)_tmp557;_tmp55A;});
_tmp559;});_tmp558;});}_LL3EB: if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 3)goto
_LL3ED;_tmp529=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp520)->f1;_tmp52A=(
void*)_tmp529.field_info;_tmp52B=_tmp529.targs;_LL3EC: {struct Cyc_List_List*
_tmp55B=Cyc_Tcutil_substs(rgn,inst,_tmp52B);return _tmp55B == _tmp52B?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp55C=_cycalloc(sizeof(*_tmp55C));
_tmp55C[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp55D;_tmp55D.tag=3;_tmp55D.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp55E;_tmp55E.field_info=(void*)_tmp52A;_tmp55E.targs=
_tmp55B;_tmp55E;});_tmp55D;});_tmp55C;});}_LL3ED: if(_tmp520 <= (void*)3?1:*((int*)
_tmp520)!= 16)goto _LL3EF;_tmp52C=((struct Cyc_Absyn_TypedefType_struct*)_tmp520)->f1;
_tmp52D=((struct Cyc_Absyn_TypedefType_struct*)_tmp520)->f2;_tmp52E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp520)->f3;_tmp52F=((struct Cyc_Absyn_TypedefType_struct*)_tmp520)->f4;_LL3EE: {
struct Cyc_List_List*_tmp55F=Cyc_Tcutil_substs(rgn,inst,_tmp52D);return _tmp55F == 
_tmp52D?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp560=_cycalloc(sizeof(*
_tmp560));_tmp560[0]=({struct Cyc_Absyn_TypedefType_struct _tmp561;_tmp561.tag=16;
_tmp561.f1=_tmp52C;_tmp561.f2=_tmp55F;_tmp561.f3=_tmp52E;_tmp561.f4=_tmp52F;
_tmp561;});_tmp560;});}_LL3EF: if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 7)goto
_LL3F1;_tmp530=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp520)->f1;_tmp531=((
struct Cyc_Absyn_ArrayType_struct*)_tmp520)->f2;_tmp532=((struct Cyc_Absyn_ArrayType_struct*)
_tmp520)->f3;_LL3F0: {void*_tmp562=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp530);
return _tmp562 == _tmp530?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp563=
_cycalloc(sizeof(*_tmp563));_tmp563[0]=({struct Cyc_Absyn_ArrayType_struct _tmp564;
_tmp564.tag=7;_tmp564.f1=(void*)_tmp562;_tmp564.f2=_tmp531;_tmp564.f3=_tmp532;
_tmp564;});_tmp563;});}_LL3F1: if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 4)goto
_LL3F3;_tmp533=((struct Cyc_Absyn_PointerType_struct*)_tmp520)->f1;_tmp534=(void*)
_tmp533.elt_typ;_tmp535=(void*)_tmp533.rgn_typ;_tmp536=_tmp533.nullable;_tmp537=
_tmp533.tq;_tmp538=_tmp533.bounds;_LL3F2: {void*_tmp565=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp534);void*_tmp566=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp535);if(
_tmp565 == _tmp534?_tmp566 == _tmp535: 0)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567[0]=({struct Cyc_Absyn_PointerType_struct
_tmp568;_tmp568.tag=4;_tmp568.f1=({struct Cyc_Absyn_PtrInfo _tmp569;_tmp569.elt_typ=(
void*)_tmp565;_tmp569.rgn_typ=(void*)_tmp566;_tmp569.nullable=_tmp536;_tmp569.tq=
_tmp537;_tmp569.bounds=_tmp538;_tmp569;});_tmp568;});_tmp567;});}_LL3F3: if(
_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 8)goto _LL3F5;_tmp539=((struct Cyc_Absyn_FnType_struct*)
_tmp520)->f1;_tmp53A=_tmp539.tvars;_tmp53B=_tmp539.effect;_tmp53C=(void*)_tmp539.ret_typ;
_tmp53D=_tmp539.args;_tmp53E=_tmp539.c_varargs;_tmp53F=_tmp539.cyc_varargs;
_tmp540=_tmp539.rgn_po;_tmp541=_tmp539.attributes;_LL3F4:{struct Cyc_List_List*
_tmp56A=_tmp53A;for(0;_tmp56A != 0;_tmp56A=_tmp56A->tl){inst=({struct Cyc_List_List*
_tmp56B=_region_malloc(rgn,sizeof(*_tmp56B));_tmp56B->hd=({struct _tuple7*_tmp56C=
_region_malloc(rgn,sizeof(*_tmp56C));_tmp56C->f1=(struct Cyc_Absyn_Tvar*)_tmp56A->hd;
_tmp56C->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp56D=_cycalloc(sizeof(*
_tmp56D));_tmp56D[0]=({struct Cyc_Absyn_VarType_struct _tmp56E;_tmp56E.tag=1;
_tmp56E.f1=(struct Cyc_Absyn_Tvar*)_tmp56A->hd;_tmp56E;});_tmp56D;});_tmp56C;});
_tmp56B->tl=inst;_tmp56B;});}}{struct Cyc_List_List*_tmp570;struct Cyc_List_List*
_tmp571;struct _tuple0 _tmp56F=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp53D));_tmp570=_tmp56F.f1;_tmp571=_tmp56F.f2;{struct Cyc_List_List*_tmp572=
Cyc_Tcutil_substs(rgn,inst,_tmp571);struct Cyc_List_List*_tmp573=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp570,_tmp572));struct Cyc_Core_Opt*
eff2;if(_tmp53B == 0)eff2=0;else{void*_tmp574=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp53B->v);if(_tmp574 == (void*)_tmp53B->v)eff2=_tmp53B;else{eff2=({struct
Cyc_Core_Opt*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->v=(void*)_tmp574;
_tmp575;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp53F == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp577;struct Cyc_Absyn_Tqual _tmp578;void*
_tmp579;int _tmp57A;struct Cyc_Absyn_VarargInfo _tmp576=*_tmp53F;_tmp577=_tmp576.name;
_tmp578=_tmp576.tq;_tmp579=(void*)_tmp576.type;_tmp57A=_tmp576.inject;{void*
_tmp57B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp579);if(_tmp57B == _tmp579)
cyc_varargs2=_tmp53F;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp57C=
_cycalloc(sizeof(*_tmp57C));_tmp57C->name=_tmp577;_tmp57C->tq=_tmp578;_tmp57C->type=(
void*)_tmp57B;_tmp57C->inject=_tmp57A;_tmp57C;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp57E;struct Cyc_List_List*_tmp57F;struct _tuple0 _tmp57D=Cyc_List_rsplit(
rgn,rgn,_tmp540);_tmp57E=_tmp57D.f1;_tmp57F=_tmp57D.f2;{struct Cyc_List_List*
_tmp580=Cyc_Tcutil_substs(rgn,inst,_tmp57E);struct Cyc_List_List*_tmp581=Cyc_Tcutil_substs(
rgn,inst,_tmp57F);if(_tmp580 == _tmp57E?_tmp581 == _tmp57F: 0)rgn_po2=_tmp540;else{
rgn_po2=Cyc_List_zip(_tmp580,_tmp581);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582[0]=({struct Cyc_Absyn_FnType_struct
_tmp583;_tmp583.tag=8;_tmp583.f1=({struct Cyc_Absyn_FnInfo _tmp584;_tmp584.tvars=
_tmp53A;_tmp584.effect=eff2;_tmp584.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp53C);_tmp584.args=_tmp573;_tmp584.c_varargs=_tmp53E;_tmp584.cyc_varargs=
cyc_varargs2;_tmp584.rgn_po=rgn_po2;_tmp584.attributes=_tmp541;_tmp584;});
_tmp583;});_tmp582;});}}}}}_LL3F5: if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 9)
goto _LL3F7;_tmp542=((struct Cyc_Absyn_TupleType_struct*)_tmp520)->f1;_LL3F6: {
struct Cyc_List_List*_tmp585=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp542);struct Cyc_List_List*_tmp586=Cyc_Tcutil_substs(rgn,inst,_tmp585);if(
_tmp586 == _tmp585)return t;{struct Cyc_List_List*_tmp587=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp542,_tmp586);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588[0]=({struct Cyc_Absyn_TupleType_struct
_tmp589;_tmp589.tag=9;_tmp589.f1=_tmp587;_tmp589;});_tmp588;});}}_LL3F7: if(
_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 11)goto _LL3F9;_tmp543=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp520)->f1;_tmp544=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp520)->f2;_LL3F8: {
struct Cyc_List_List*_tmp58A=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp544);struct Cyc_List_List*_tmp58B=Cyc_Tcutil_substs(rgn,inst,_tmp58A);if(
_tmp58B == _tmp58A)return t;{struct Cyc_List_List*_tmp58C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp544,
_tmp58B);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp58D=_cycalloc(
sizeof(*_tmp58D));_tmp58D[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp58E;
_tmp58E.tag=11;_tmp58E.f1=(void*)_tmp543;_tmp58E.f2=_tmp58C;_tmp58E;});_tmp58D;});}}
_LL3F9: if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 0)goto _LL3FB;_tmp545=((struct
Cyc_Absyn_Evar_struct*)_tmp520)->f2;_LL3FA: if(_tmp545 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp545->v);else{return t;}_LL3FB: if(_tmp520 <= (void*)3?1:*((int*)
_tmp520)!= 15)goto _LL3FD;_tmp546=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp520)->f1;_LL3FC: {void*_tmp58F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp546);
return _tmp58F == _tmp546?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp590=
_cycalloc(sizeof(*_tmp590));_tmp590[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp591;_tmp591.tag=15;_tmp591.f1=(void*)_tmp58F;_tmp591;});_tmp590;});}_LL3FD:
if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 14)goto _LL3FF;_tmp547=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp520)->f1;_LL3FE: {void*_tmp592=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp547);return _tmp592 == _tmp547?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp594;_tmp594.tag=14;_tmp594.f1=(void*)_tmp592;_tmp594;});_tmp593;});}_LL3FF:
if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 17)goto _LL401;_tmp548=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp520)->f1;_LL400: {void*_tmp595=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp548);return _tmp595 == _tmp548?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596[0]=({struct Cyc_Absyn_TagType_struct
_tmp597;_tmp597.tag=17;_tmp597.f1=(void*)_tmp595;_tmp597;});_tmp596;});}_LL401:
if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 18)goto _LL403;_LL402: goto _LL404;
_LL403: if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 12)goto _LL405;_LL404: goto
_LL406;_LL405: if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 13)goto _LL407;_LL406:
goto _LL408;_LL407: if((int)_tmp520 != 0)goto _LL409;_LL408: goto _LL40A;_LL409: if(
_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 5)goto _LL40B;_LL40A: goto _LL40C;_LL40B:
if((int)_tmp520 != 1)goto _LL40D;_LL40C: goto _LL40E;_LL40D: if(_tmp520 <= (void*)3?1:*((
int*)_tmp520)!= 6)goto _LL40F;_LL40E: goto _LL410;_LL40F: if((int)_tmp520 != 2)goto
_LL411;_LL410: return t;_LL411: if(_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 21)goto
_LL413;_tmp549=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp520)->f1;_LL412: {
void*_tmp598=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp549);return _tmp598 == _tmp549?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp599=_cycalloc(sizeof(*_tmp599));
_tmp599[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp59A;_tmp59A.tag=21;_tmp59A.f1=(
void*)_tmp598;_tmp59A;});_tmp599;});}_LL413: if(_tmp520 <= (void*)3?1:*((int*)
_tmp520)!= 19)goto _LL415;_tmp54A=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp520)->f1;_LL414: {void*_tmp59B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp54A);
return _tmp59B == _tmp54A?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp59C=
_cycalloc(sizeof(*_tmp59C));_tmp59C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp59D;
_tmp59D.tag=19;_tmp59D.f1=(void*)_tmp59B;_tmp59D;});_tmp59C;});}_LL415: if(
_tmp520 <= (void*)3?1:*((int*)_tmp520)!= 20)goto _LL3E4;_tmp54B=((struct Cyc_Absyn_JoinEff_struct*)
_tmp520)->f1;_LL416: {struct Cyc_List_List*_tmp59E=Cyc_Tcutil_substs(rgn,inst,
_tmp54B);return _tmp59E == _tmp54B?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp5A0;_tmp5A0.tag=20;_tmp5A0.f1=_tmp59E;_tmp5A0;});_tmp59F;});}_LL3E4:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp5A1=(void*)ts->hd;
struct Cyc_List_List*_tmp5A2=ts->tl;void*_tmp5A3=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp5A1);struct Cyc_List_List*_tmp5A4=Cyc_Tcutil_substs(rgn,inst,_tmp5A2);if(
_tmp5A1 == _tmp5A3?_tmp5A2 == _tmp5A4: 0)return ts;return(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->hd=(void*)
_tmp5A3;_tmp5A5->tl=_tmp5A4;_tmp5A5;});}}extern void*Cyc_Tcutil_substitute(struct
Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple7*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple7*_tmp5A6=_cycalloc(
sizeof(*_tmp5A6));_tmp5A6->f1=tv;_tmp5A6->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->v=(void*)k;_tmp5A7;}),({struct Cyc_Core_Opt*
_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->v=s;_tmp5A8;}));_tmp5A6;});}struct
_tuple7*Cyc_Tcutil_r_make_inst_var(struct _tuple8*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple8 _tmp5AA;struct Cyc_List_List*_tmp5AB;struct _RegionHandle*_tmp5AC;
struct _tuple8*_tmp5A9=env;_tmp5AA=*_tmp5A9;_tmp5AB=_tmp5AA.f1;_tmp5AC=_tmp5AA.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple7*_tmp5AD=_region_malloc(
_tmp5AC,sizeof(*_tmp5AD));_tmp5AD->f1=tv;_tmp5AD->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->v=(void*)k;_tmp5AE;}),({
struct Cyc_Core_Opt*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->v=_tmp5AB;
_tmp5AF;}));_tmp5AD;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_Std_zstrptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_Std_String_pa_struct _tmp5B3;_tmp5B3.tag=0;_tmp5B3.f1=(struct
_tagged_arr)Cyc_Absynpp_kindbound2string(k2);{struct Cyc_Std_String_pa_struct
_tmp5B2;_tmp5B2.tag=0;_tmp5B2.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k1);{struct Cyc_Std_String_pa_struct _tmp5B1;_tmp5B1.tag=0;_tmp5B1.f1=(struct
_tagged_arr)*tv->name;{void*_tmp5B0[3]={& _tmp5B1,& _tmp5B2,& _tmp5B3};Cyc_Tcutil_terr(
loc,_tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof(char),59),_tag_arr(_tmp5B0,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp5B4[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("same type variable has different identity!",sizeof(char),43),_tag_arr(
_tmp5B4,sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->hd=tv;
_tmp5B5->tl=tvs;_tmp5B5;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*
_tmp5B6[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv",sizeof(char),39),_tag_arr(
_tmp5B6,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp5B7[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof(char),41),_tag_arr(_tmp5B7,sizeof(void*),0));});if(*((int*)_check_null(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)_check_null(tv->identity)))
return tvs;}}return({struct Cyc_List_List*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));
_tmp5B8->hd=tv;_tmp5B8->tl=tvs;_tmp5B8;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_Std_String_pa_struct
_tmp5BA;_tmp5BA.tag=0;_tmp5BA.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string(tv);{
void*_tmp5B9[1]={& _tmp5BA};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bound tvar id for %s is NULL",sizeof(char),29),
_tag_arr(_tmp5B9,sizeof(void*),1));}});return({struct Cyc_List_List*_tmp5BB=
_cycalloc(sizeof(*_tmp5BB));_tmp5BB->hd=tv;_tmp5BB->tl=tvs;_tmp5BB;});}static
struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){void*
_tmp5BC=Cyc_Tcutil_compress(e);int _tmp5BD;_LL41D: if(_tmp5BC <= (void*)3?1:*((int*)
_tmp5BC)!= 0)goto _LL41F;_tmp5BD=((struct Cyc_Absyn_Evar_struct*)_tmp5BC)->f3;
_LL41E:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*_tmp5BE=Cyc_Tcutil_compress((
void*)es2->hd);int _tmp5BF;_LL422: if(_tmp5BE <= (void*)3?1:*((int*)_tmp5BE)!= 0)
goto _LL424;_tmp5BF=((struct Cyc_Absyn_Evar_struct*)_tmp5BE)->f3;_LL423: if(_tmp5BD
== _tmp5BF)return es;goto _LL421;_LL424:;_LL425: goto _LL421;_LL421:;}}return({
struct Cyc_List_List*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->hd=(void*)e;
_tmp5C0->tl=es;_tmp5C0;});_LL41F:;_LL420: return es;_LL41C:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))
== *((int*)_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;
break;}}}if(!present)r=({struct Cyc_List_List*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));
_tmp5C1->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp5C1->tl=r;_tmp5C1;});}r=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(width)))({struct Cyc_Std_String_pa_struct
_tmp5C3;_tmp5C3.tag=0;_tmp5C3.f1=(struct _tagged_arr)*fn;{void*_tmp5C2[1]={&
_tmp5C3};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s does not have constant width",
sizeof(char),41),_tag_arr(_tmp5C2,sizeof(void*),1));}});else{w=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(width));}{void*_tmp5C4=Cyc_Tcutil_compress(
field_typ);void*_tmp5C5;_LL427: if(_tmp5C4 <= (void*)3?1:*((int*)_tmp5C4)!= 5)goto
_LL429;_tmp5C5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C4)->f2;_LL428:{
void*_tmp5C6=_tmp5C5;_LL42C: if((int)_tmp5C6 != 0)goto _LL42E;_LL42D: if(w > 8)({void*
_tmp5C7[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",sizeof(
char),26),_tag_arr(_tmp5C7,sizeof(void*),0));});goto _LL42B;_LL42E: if((int)
_tmp5C6 != 1)goto _LL430;_LL42F: if(w > 16)({void*_tmp5C8[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp5C8,sizeof(
void*),0));});goto _LL42B;_LL430: if((int)_tmp5C6 != 2)goto _LL432;_LL431: if(w > 32)({
void*_tmp5C9[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(char),26),_tag_arr(_tmp5C9,sizeof(void*),0));});goto _LL42B;_LL432: if((int)
_tmp5C6 != 3)goto _LL42B;_LL433: if(w > 64)({void*_tmp5CA[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp5CA,sizeof(
void*),0));});goto _LL42B;_LL42B:;}goto _LL426;_LL429:;_LL42A:({struct Cyc_Std_String_pa_struct
_tmp5CD;_tmp5CD.tag=0;_tmp5CD.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
field_typ);{struct Cyc_Std_String_pa_struct _tmp5CC;_tmp5CC.tag=0;_tmp5CC.f1=(
struct _tagged_arr)*fn;{void*_tmp5CB[2]={& _tmp5CC,& _tmp5CD};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield %s must have integral type but has type %s",sizeof(char),52),
_tag_arr(_tmp5CB,sizeof(void*),2));}}});goto _LL426;_LL426:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp5CE=(void*)atts->hd;_LL435: if((int)
_tmp5CE != 5)goto _LL437;_LL436: continue;_LL437: if(_tmp5CE <= (void*)16?1:*((int*)
_tmp5CE)!= 1)goto _LL439;_LL438: continue;_LL439:;_LL43A:({struct Cyc_Std_String_pa_struct
_tmp5D1;_tmp5D1.tag=0;_tmp5D1.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_String_pa_struct
_tmp5D0;_tmp5D0.tag=0;_tmp5D0.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp5CF[2]={& _tmp5D0,& _tmp5D1};Cyc_Tcutil_terr(loc,
_tag_arr("bad attribute %s on member %s",sizeof(char),30),_tag_arr(_tmp5CF,
sizeof(void*),2));}}});_LL434:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};struct _tuple18{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t){{void*_tmp5D2=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp5D3;struct Cyc_Core_Opt**_tmp5D4;struct Cyc_Core_Opt*
_tmp5D5;struct Cyc_Core_Opt**_tmp5D6;struct Cyc_Absyn_Tvar*_tmp5D7;struct Cyc_List_List*
_tmp5D8;struct _tuple1*_tmp5D9;struct Cyc_Absyn_Enumdecl*_tmp5DA;struct Cyc_Absyn_Enumdecl**
_tmp5DB;struct Cyc_Absyn_TunionInfo _tmp5DC;void*_tmp5DD;void**_tmp5DE;struct Cyc_List_List*
_tmp5DF;struct Cyc_List_List**_tmp5E0;void*_tmp5E1;struct Cyc_Absyn_TunionFieldInfo
_tmp5E2;void*_tmp5E3;void**_tmp5E4;struct Cyc_List_List*_tmp5E5;struct Cyc_Absyn_PtrInfo
_tmp5E6;void*_tmp5E7;void*_tmp5E8;struct Cyc_Absyn_Conref*_tmp5E9;struct Cyc_Absyn_Tqual
_tmp5EA;struct Cyc_Absyn_Conref*_tmp5EB;void*_tmp5EC;void*_tmp5ED;void*_tmp5EE;
struct Cyc_Absyn_Tqual _tmp5EF;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_FnInfo
_tmp5F1;struct Cyc_List_List*_tmp5F2;struct Cyc_List_List**_tmp5F3;struct Cyc_Core_Opt*
_tmp5F4;struct Cyc_Core_Opt**_tmp5F5;void*_tmp5F6;struct Cyc_List_List*_tmp5F7;int
_tmp5F8;struct Cyc_Absyn_VarargInfo*_tmp5F9;struct Cyc_List_List*_tmp5FA;struct Cyc_List_List*
_tmp5FB;struct Cyc_List_List*_tmp5FC;void*_tmp5FD;struct Cyc_List_List*_tmp5FE;
struct Cyc_Absyn_AggrInfo _tmp5FF;void*_tmp600;void**_tmp601;struct Cyc_List_List*
_tmp602;struct Cyc_List_List**_tmp603;struct _tuple1*_tmp604;struct Cyc_List_List*
_tmp605;struct Cyc_List_List**_tmp606;struct Cyc_Absyn_Typedefdecl*_tmp607;struct
Cyc_Absyn_Typedefdecl**_tmp608;struct Cyc_Core_Opt*_tmp609;struct Cyc_Core_Opt**
_tmp60A;void*_tmp60B;void*_tmp60C;void*_tmp60D;struct Cyc_List_List*_tmp60E;
_LL43C: if((int)_tmp5D2 != 0)goto _LL43E;_LL43D: goto _LL43B;_LL43E: if(_tmp5D2 <= (
void*)3?1:*((int*)_tmp5D2)!= 0)goto _LL440;_tmp5D3=((struct Cyc_Absyn_Evar_struct*)
_tmp5D2)->f1;_tmp5D4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp5D2)->f1;_tmp5D5=((struct Cyc_Absyn_Evar_struct*)_tmp5D2)->f2;_tmp5D6=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp5D2)->f2;_LL43F: if(*_tmp5D4 == 
0)*_tmp5D4=({struct Cyc_Core_Opt*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->v=(
void*)expected_kind;_tmp60F;});if((cvtenv.fn_result?cvtenv.generalize_evars: 0)?
expected_kind == (void*)3: 0)*_tmp5D6=({struct Cyc_Core_Opt*_tmp610=_cycalloc(
sizeof(*_tmp610));_tmp610->v=(void*)((void*)2);_tmp610;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp611=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp616;_tmp616.tag=2;_tmp616.f1=0;_tmp616.f2=(void*)expected_kind;_tmp616;});
_tmp615;}));*_tmp5D6=({struct Cyc_Core_Opt*_tmp612=_cycalloc(sizeof(*_tmp612));
_tmp612->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp613=_cycalloc(
sizeof(*_tmp613));_tmp613[0]=({struct Cyc_Absyn_VarType_struct _tmp614;_tmp614.tag=
1;_tmp614.f1=_tmp611;_tmp614;});_tmp613;}));_tmp612;});_tmp5D7=_tmp611;goto
_LL441;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL43B;_LL440: if(_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 1)goto _LL442;
_tmp5D7=((struct Cyc_Absyn_VarType_struct*)_tmp5D2)->f1;_LL441:{void*_tmp617=Cyc_Absyn_compress_kb((
void*)_tmp5D7->kind);struct Cyc_Core_Opt*_tmp618;struct Cyc_Core_Opt**_tmp619;
_LL46F: if(*((int*)_tmp617)!= 1)goto _LL471;_tmp618=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp617)->f1;_tmp619=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp617)->f1;_LL470:*_tmp619=({struct Cyc_Core_Opt*_tmp61A=_cycalloc(sizeof(*
_tmp61A));_tmp61A->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp61B=
_cycalloc(sizeof(*_tmp61B));_tmp61B[0]=({struct Cyc_Absyn_Less_kb_struct _tmp61C;
_tmp61C.tag=2;_tmp61C.f1=0;_tmp61C.f2=(void*)expected_kind;_tmp61C;});_tmp61B;}));
_tmp61A;});goto _LL46E;_LL471:;_LL472: goto _LL46E;_LL46E:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp5D7);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp5D7);goto _LL43B;_LL442: if(_tmp5D2 <= (void*)3?1:*((int*)
_tmp5D2)!= 13)goto _LL444;_tmp5D8=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp5D2)->f1;
_LL443: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp61D=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp61D;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp5D8 != 0;_tmp5D8=_tmp5D8->tl){struct Cyc_Absyn_Enumfield*_tmp61E=(
struct Cyc_Absyn_Enumfield*)_tmp5D8->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp61E->name).f2))({struct Cyc_Std_String_pa_struct
_tmp620;_tmp620.tag=0;_tmp620.f1=(struct _tagged_arr)*(*_tmp61E->name).f2;{void*
_tmp61F[1]={& _tmp620};Cyc_Tcutil_terr(_tmp61E->loc,_tag_arr("duplicate enum field name %s",
sizeof(char),29),_tag_arr(_tmp61F,sizeof(void*),1));}});else{prev_fields=({
struct Cyc_List_List*_tmp621=_region_malloc(uprev_rgn,sizeof(*_tmp621));_tmp621->hd=(*
_tmp61E->name).f2;_tmp621->tl=prev_fields;_tmp621;});}if(_tmp61E->tag == 0)
_tmp61E->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp61E->loc);
else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp61E->tag)))({
struct Cyc_Std_String_pa_struct _tmp623;_tmp623.tag=0;_tmp623.f1=(struct
_tagged_arr)*(*_tmp61E->name).f2;{void*_tmp622[1]={& _tmp623};Cyc_Tcutil_terr(loc,
_tag_arr("enum field %s: expression is not constant",sizeof(char),42),_tag_arr(
_tmp622,sizeof(void*),1));}});}{unsigned int t1=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp61E->tag));tag_count=t1 + 1;(*_tmp61E->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp625;_tmp625.tag=1;_tmp625.f1=te->ns;_tmp625;});
_tmp624;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple18*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp61E->name).f2,({
struct _tuple18*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp628;_tmp628.tag=4;_tmp628.f1=(void*)t;
_tmp628.f2=_tmp61E;_tmp628;});_tmp627;});_tmp626->f2=1;_tmp626;}));}}};
_pop_region(uprev_rgn);}goto _LL43B;}_LL444: if(_tmp5D2 <= (void*)3?1:*((int*)
_tmp5D2)!= 12)goto _LL446;_tmp5D9=((struct Cyc_Absyn_EnumType_struct*)_tmp5D2)->f1;
_tmp5DA=((struct Cyc_Absyn_EnumType_struct*)_tmp5D2)->f2;_tmp5DB=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp5D2)->f2;_LL445: if(*_tmp5DB == 0?1:((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp5DB))->fields == 0){struct _handler_cons
_tmp629;_push_handler(& _tmp629);{int _tmp62B=0;if(setjmp(_tmp629.handler))_tmp62B=
1;if(!_tmp62B){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp5D9);*_tmp5DB=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp62A=(void*)_exn_thrown;void*_tmp62D=_tmp62A;_LL474: if(_tmp62D != Cyc_Dict_Absent)
goto _LL476;_LL475: {struct Cyc_Tcenv_Genv*_tmp62E=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp62F=({struct Cyc_Absyn_Enumdecl*_tmp630=_cycalloc(
sizeof(*_tmp630));_tmp630->sc=(void*)((void*)3);_tmp630->name=_tmp5D9;_tmp630->fields=
0;_tmp630;});Cyc_Tc_tcEnumdecl(te,_tmp62E,loc,_tmp62F);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp5D9);*_tmp5DB=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL473;}}_LL476:;_LL477:(void)_throw(_tmp62D);_LL473:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp5DB);*_tmp5D9=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL43B;}_LL446: if(_tmp5D2 <= (void*)3?1:*((
int*)_tmp5D2)!= 2)goto _LL448;_tmp5DC=((struct Cyc_Absyn_TunionType_struct*)
_tmp5D2)->f1;_tmp5DD=(void*)_tmp5DC.tunion_info;_tmp5DE=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp5D2)->f1).tunion_info;_tmp5DF=_tmp5DC.targs;_tmp5E0=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp5D2)->f1).targs;_tmp5E1=(void*)_tmp5DC.rgn;
_LL447: {struct Cyc_List_List*_tmp631=*_tmp5E0;{void*_tmp632=*_tmp5DE;struct Cyc_Absyn_UnknownTunionInfo
_tmp633;struct _tuple1*_tmp634;int _tmp635;struct Cyc_Absyn_Tuniondecl**_tmp636;
struct Cyc_Absyn_Tuniondecl*_tmp637;_LL479: if(*((int*)_tmp632)!= 0)goto _LL47B;
_tmp633=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp632)->f1;_tmp634=_tmp633.name;
_tmp635=_tmp633.is_xtunion;_LL47A: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp638;_push_handler(& _tmp638);{int _tmp63A=0;if(setjmp(_tmp638.handler))
_tmp63A=1;if(!_tmp63A){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp634);;
_pop_handler();}else{void*_tmp639=(void*)_exn_thrown;void*_tmp63C=_tmp639;_LL47E:
if(_tmp63C != Cyc_Dict_Absent)goto _LL480;_LL47F: {struct Cyc_Tcenv_Genv*_tmp63D=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp63E=({struct Cyc_Absyn_Tuniondecl*
_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->sc=(void*)((void*)3);_tmp642->name=
_tmp634;_tmp642->tvs=0;_tmp642->fields=0;_tmp642->is_xtunion=_tmp635;_tmp642;});
Cyc_Tc_tcTuniondecl(te,_tmp63D,loc,_tmp63E);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp634);if(_tmp631 != 0){({struct Cyc_Std_String_pa_struct _tmp641;_tmp641.tag=
0;_tmp641.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp634);{struct Cyc_Std_String_pa_struct
_tmp640;_tmp640.tag=0;_tmp640.f1=(struct _tagged_arr)(_tmp635?_tag_arr("xtunion",
sizeof(char),8): _tag_arr("tunion",sizeof(char),7));{void*_tmp63F[2]={& _tmp640,&
_tmp641};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized %s %s before using",
sizeof(char),48),_tag_arr(_tmp63F,sizeof(void*),2));}}});return cvtenv;}goto
_LL47D;}_LL480:;_LL481:(void)_throw(_tmp63C);_LL47D:;}}}if((*tudp)->is_xtunion != 
_tmp635)({struct Cyc_Std_String_pa_struct _tmp644;_tmp644.tag=0;_tmp644.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp634);{void*_tmp643[1]={& _tmp644};Cyc_Tcutil_terr(
loc,_tag_arr("[x]tunion is different from type declaration %s",sizeof(char),48),
_tag_arr(_tmp643,sizeof(void*),1));}});*_tmp5DE=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp646;_tmp646.tag=1;_tmp646.f1=tudp;_tmp646;});_tmp645;});_tmp637=*tudp;goto
_LL47C;}_LL47B: if(*((int*)_tmp632)!= 1)goto _LL478;_tmp636=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp632)->f1;_tmp637=*_tmp636;_LL47C: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5E1);{struct Cyc_List_List*tvs=_tmp637->tvs;for(0;_tmp631 != 0?
tvs != 0: 0;(_tmp631=_tmp631->tl,tvs=tvs->tl)){void*t1=(void*)_tmp631->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp631 != 0)({struct Cyc_Std_String_pa_struct _tmp648;
_tmp648.tag=0;_tmp648.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp637->name);{
void*_tmp647[1]={& _tmp648};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s",
sizeof(char),38),_tag_arr(_tmp647,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp649=
_cycalloc(sizeof(*_tmp649));_tmp649->hd=(void*)e;_tmp649->tl=hidden_ts;_tmp649;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*_tmp5E0=Cyc_List_imp_append(*
_tmp5E0,Cyc_List_imp_rev(hidden_ts));}goto _LL478;}_LL478:;}goto _LL43B;}_LL448:
if(_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 3)goto _LL44A;_tmp5E2=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5D2)->f1;_tmp5E3=(void*)_tmp5E2.field_info;_tmp5E4=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5D2)->f1).field_info;_tmp5E5=_tmp5E2.targs;_LL449:{void*_tmp64A=*_tmp5E4;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp64B;struct _tuple1*_tmp64C;struct
_tuple1*_tmp64D;int _tmp64E;struct Cyc_Absyn_Tuniondecl*_tmp64F;struct Cyc_Absyn_Tunionfield*
_tmp650;_LL483: if(*((int*)_tmp64A)!= 0)goto _LL485;_tmp64B=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp64A)->f1;_tmp64C=_tmp64B.tunion_name;_tmp64D=_tmp64B.field_name;_tmp64E=
_tmp64B.is_xtunion;_LL484: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp651;_push_handler(& _tmp651);{int _tmp653=0;if(setjmp(
_tmp651.handler))_tmp653=1;if(!_tmp653){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp64C);;_pop_handler();}else{void*_tmp652=(void*)_exn_thrown;void*_tmp655=
_tmp652;_LL488: if(_tmp655 != Cyc_Dict_Absent)goto _LL48A;_LL489:({struct Cyc_Std_String_pa_struct
_tmp657;_tmp657.tag=0;_tmp657.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp64C);{void*_tmp656[1]={& _tmp657};Cyc_Tcutil_terr(loc,_tag_arr("unbound type tunion %s",
sizeof(char),23),_tag_arr(_tmp656,sizeof(void*),1));}});return cvtenv;_LL48A:;
_LL48B:(void)_throw(_tmp655);_LL487:;}}}{struct _handler_cons _tmp658;
_push_handler(& _tmp658);{int _tmp65A=0;if(setjmp(_tmp658.handler))_tmp65A=1;if(!
_tmp65A){{void*_tmp65B=Cyc_Tcenv_lookup_ordinary(te,loc,_tmp64D);struct Cyc_Absyn_Tuniondecl*
_tmp65C;struct Cyc_Absyn_Tunionfield*_tmp65D;_LL48D: if(*((int*)_tmp65B)!= 2)goto
_LL48F;_tmp65C=((struct Cyc_Tcenv_TunionRes_struct*)_tmp65B)->f1;_tmp65D=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp65B)->f2;_LL48E: tuf=_tmp65D;tud=_tmp65C;if(tud->is_xtunion
!= _tmp64E)({struct Cyc_Std_String_pa_struct _tmp65F;_tmp65F.tag=0;_tmp65F.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp64C);{void*_tmp65E[1]={& _tmp65F};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(char),48),_tag_arr(_tmp65E,sizeof(void*),1));}});goto _LL48C;_LL48F:;
_LL490:({struct Cyc_Std_String_pa_struct _tmp662;_tmp662.tag=0;_tmp662.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp64C);{struct Cyc_Std_String_pa_struct
_tmp661;_tmp661.tag=0;_tmp661.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp64D);{void*_tmp660[2]={& _tmp661,& _tmp662};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp660,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv
_tmp663=cvtenv;_npop_handler(0);return _tmp663;}_LL48C:;};_pop_handler();}else{
void*_tmp659=(void*)_exn_thrown;void*_tmp665=_tmp659;_LL492: if(_tmp665 != Cyc_Dict_Absent)
goto _LL494;_LL493:({struct Cyc_Std_String_pa_struct _tmp668;_tmp668.tag=0;_tmp668.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp64C);{struct Cyc_Std_String_pa_struct
_tmp667;_tmp667.tag=0;_tmp667.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp64D);{void*_tmp666[2]={& _tmp667,& _tmp668};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp666,sizeof(void*),2));}}});return cvtenv;_LL494:;
_LL495:(void)_throw(_tmp665);_LL491:;}}}*_tmp5E4=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp66A;_tmp66A.tag=1;_tmp66A.f1=tud;_tmp66A.f2=tuf;_tmp66A;});_tmp669;});
_tmp64F=tud;_tmp650=tuf;goto _LL486;}_LL485: if(*((int*)_tmp64A)!= 1)goto _LL482;
_tmp64F=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp64A)->f1;_tmp650=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp64A)->f2;_LL486: {struct Cyc_List_List*tvs=
_tmp64F->tvs;for(0;_tmp5E5 != 0?tvs != 0: 0;(_tmp5E5=_tmp5E5->tl,tvs=tvs->tl)){void*
t1=(void*)_tmp5E5->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp5E5 != 0)({
struct Cyc_Std_String_pa_struct _tmp66D;_tmp66D.tag=0;_tmp66D.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp650->name);{struct Cyc_Std_String_pa_struct
_tmp66C;_tmp66C.tag=0;_tmp66C.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp64F->name);{void*_tmp66B[2]={& _tmp66C,& _tmp66D};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s.%s",
sizeof(char),41),_tag_arr(_tmp66B,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_Std_String_pa_struct
_tmp670;_tmp670.tag=0;_tmp670.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp650->name);{struct Cyc_Std_String_pa_struct _tmp66F;_tmp66F.tag=0;_tmp66F.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp64F->name);{void*_tmp66E[2]={&
_tmp66F,& _tmp670};Cyc_Tcutil_terr(loc,_tag_arr("too few type arguments for tunion %s.%s",
sizeof(char),40),_tag_arr(_tmp66E,sizeof(void*),2));}}});goto _LL482;}_LL482:;}
goto _LL43B;_LL44A: if(_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 4)goto _LL44C;
_tmp5E6=((struct Cyc_Absyn_PointerType_struct*)_tmp5D2)->f1;_tmp5E7=(void*)
_tmp5E6.elt_typ;_tmp5E8=(void*)_tmp5E6.rgn_typ;_tmp5E9=_tmp5E6.nullable;_tmp5EA=
_tmp5E6.tq;_tmp5EB=_tmp5E6.bounds;_LL44B: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp5E7);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5E8);{void*_tmp671=(void*)(Cyc_Absyn_compress_conref(_tmp5EB))->v;
void*_tmp672;struct Cyc_Absyn_Exp*_tmp673;_LL497: if(_tmp671 <= (void*)1?1:*((int*)
_tmp671)!= 0)goto _LL499;_tmp672=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp671)->f1;if(_tmp672 <= (void*)1?1:*((int*)_tmp672)!= 0)goto _LL499;_tmp673=((
struct Cyc_Absyn_Upper_b_struct*)_tmp672)->f1;_LL498: if(!Cyc_Tcutil_is_const_exp(
te,_tmp673))({void*_tmp674[0]={};Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not a constant",
sizeof(char),44),_tag_arr(_tmp674,sizeof(void*),0));});Cyc_Tcexp_tcExp(te,0,
_tmp673);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp673))({void*_tmp675[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("pointer bounds expression is not an unsigned int",sizeof(char),49),
_tag_arr(_tmp675,sizeof(void*),0));});Cyc_Evexp_eval_const_uint_exp(_tmp673);
goto _LL496;_LL499:;_LL49A: goto _LL496;_LL496:;}goto _LL43B;_LL44C: if(_tmp5D2 <= (
void*)3?1:*((int*)_tmp5D2)!= 14)goto _LL44E;_tmp5EC=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp5D2)->f1;_LL44D: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,
_tmp5EC);goto _LL43B;_LL44E: if(_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 17)goto
_LL450;_tmp5ED=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp5D2)->f1;_LL44F:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp5ED);goto _LL43B;
_LL450: if(_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 18)goto _LL452;_LL451: goto
_LL453;_LL452: if(_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 5)goto _LL454;_LL453:
goto _LL455;_LL454: if((int)_tmp5D2 != 1)goto _LL456;_LL455: goto _LL457;_LL456: if(
_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 6)goto _LL458;_LL457: goto _LL43B;_LL458:
if(_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 7)goto _LL45A;_tmp5EE=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp5D2)->f1;_tmp5EF=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5D2)->f2;_tmp5F0=((struct Cyc_Absyn_ArrayType_struct*)_tmp5D2)->f3;_LL459:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,_tmp5EE);if(_tmp5F0
== 0?1: !Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp5F0)))({
void*_tmp676[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array bounds expression is not constant",
sizeof(char),40),_tag_arr(_tmp676,sizeof(void*),0));});Cyc_Tcexp_tcExp(te,0,(
struct Cyc_Absyn_Exp*)_check_null(_tmp5F0));if(!Cyc_Tcutil_coerce_uint_typ(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp5F0)))({void*_tmp677[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("array bounds expression is not an unsigned int",sizeof(char),47),
_tag_arr(_tmp677,sizeof(void*),0));});Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp5F0));goto _LL43B;_LL45A: if(_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)
!= 8)goto _LL45C;_tmp5F1=((struct Cyc_Absyn_FnType_struct*)_tmp5D2)->f1;_tmp5F2=
_tmp5F1.tvars;_tmp5F3=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp5D2)->f1).tvars;_tmp5F4=_tmp5F1.effect;_tmp5F5=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp5D2)->f1).effect;_tmp5F6=(void*)_tmp5F1.ret_typ;
_tmp5F7=_tmp5F1.args;_tmp5F8=_tmp5F1.c_varargs;_tmp5F9=_tmp5F1.cyc_varargs;
_tmp5FA=_tmp5F1.rgn_po;_tmp5FB=_tmp5F1.attributes;_LL45B: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp5FB != 0;_tmp5FB=_tmp5FB->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp5FB->hd))({struct Cyc_Std_String_pa_struct
_tmp679;_tmp679.tag=0;_tmp679.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp5FB->hd);{void*_tmp678[1]={& _tmp679};Cyc_Tcutil_terr(loc,_tag_arr("bad function type attribute %s",
sizeof(char),31),_tag_arr(_tmp678,sizeof(void*),1));}});{void*_tmp67A=(void*)
_tmp5FB->hd;void*_tmp67B;int _tmp67C;int _tmp67D;_LL49C: if((int)_tmp67A != 0)goto
_LL49E;_LL49D: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL49B;_LL49E:
if((int)_tmp67A != 1)goto _LL4A0;_LL49F: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}
goto _LL49B;_LL4A0: if((int)_tmp67A != 2)goto _LL4A2;_LL4A1: if(!seen_fastcall){
seen_fastcall=1;++ num_convs;}goto _LL49B;_LL4A2: if(_tmp67A <= (void*)16?1:*((int*)
_tmp67A)!= 3)goto _LL4A4;_tmp67B=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp67A)->f1;_tmp67C=((struct Cyc_Absyn_Format_att_struct*)_tmp67A)->f2;_tmp67D=((
struct Cyc_Absyn_Format_att_struct*)_tmp67A)->f3;_LL4A3: if(!seen_format){
seen_format=1;ft=_tmp67B;fmt_desc_arg=_tmp67C;fmt_arg_start=_tmp67D;}else{({void*
_tmp67E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function can't have multiple format attributes",
sizeof(char),47),_tag_arr(_tmp67E,sizeof(void*),0));});}goto _LL49B;_LL4A4:;
_LL4A5: goto _LL49B;_LL49B:;}}if(num_convs > 1)({void*_tmp67F[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("function can't have multiple calling conventions",sizeof(char),49),
_tag_arr(_tmp67F,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp5F3);{
struct Cyc_List_List*b=*_tmp5F3;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp680=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp681;_LL4A7: if(*((int*)_tmp680)!= 0)goto
_LL4A9;_tmp681=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp680)->f1;if((int)
_tmp681 != 1)goto _LL4A9;_LL4A8:({struct Cyc_Std_String_pa_struct _tmp683;_tmp683.tag=
0;_tmp683.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name;{void*
_tmp682[1]={& _tmp683};Cyc_Tcutil_terr(loc,_tag_arr("function attempts to abstract Mem type variable %s",
sizeof(char),51),_tag_arr(_tmp682,sizeof(void*),1));}});goto _LL4A6;_LL4A9:;
_LL4AA: goto _LL4A6;_LL4A6:;}}}{struct Cyc_Tcutil_CVTEnv _tmp684=({struct Cyc_Tcutil_CVTEnv
_tmp6F3;_tmp6F3.kind_env=cvtenv.kind_env;_tmp6F3.free_vars=0;_tmp6F3.free_evars=
0;_tmp6F3.generalize_evars=cvtenv.generalize_evars;_tmp6F3.fn_result=1;_tmp6F3;});
_tmp684=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp684,(void*)1,_tmp5F6);_tmp684.fn_result=
0;{struct Cyc_List_List*a=_tmp5F7;for(0;a != 0;a=a->tl){_tmp684=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp684,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp5F9 != 0){if(
_tmp5F8)({void*_tmp685[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("both c_vararg and cyc_vararg",sizeof(char),29),
_tag_arr(_tmp685,sizeof(void*),0));});{struct Cyc_Core_Opt*_tmp687;struct Cyc_Absyn_Tqual
_tmp688;void*_tmp689;int _tmp68A;struct Cyc_Absyn_VarargInfo _tmp686=*_tmp5F9;
_tmp687=_tmp686.name;_tmp688=_tmp686.tq;_tmp689=(void*)_tmp686.type;_tmp68A=
_tmp686.inject;_tmp684=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp684,(void*)1,
_tmp689);if(_tmp68A){void*_tmp68B=Cyc_Tcutil_compress(_tmp689);struct Cyc_Absyn_TunionInfo
_tmp68C;void*_tmp68D;void*_tmp68E;_LL4AC: if(_tmp68B <= (void*)3?1:*((int*)_tmp68B)
!= 2)goto _LL4AE;_tmp68C=((struct Cyc_Absyn_TunionType_struct*)_tmp68B)->f1;
_tmp68D=(void*)_tmp68C.tunion_info;if(*((int*)_tmp68D)!= 1)goto _LL4AE;_tmp68E=(
void*)_tmp68C.rgn;_LL4AD: goto _LL4AB;_LL4AE:;_LL4AF:({void*_tmp68F[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("can't inject a non-[x]tunion type",sizeof(char),34),_tag_arr(
_tmp68F,sizeof(void*),0));});goto _LL4AB;_LL4AB:;}}}if(seen_format){int _tmp690=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5F7);if((((fmt_desc_arg < 0?1:
fmt_desc_arg > _tmp690)?1: fmt_arg_start < 0)?1:(_tmp5F9 == 0?fmt_arg_start != 0: 0))?
1:(_tmp5F9 != 0?fmt_arg_start != _tmp690 + 1: 0))({void*_tmp691[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bad format descriptor",sizeof(char),22),_tag_arr(_tmp691,sizeof(
void*),0));});else{void*_tmp693;struct _tuple2 _tmp692=*((struct _tuple2*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5F7,fmt_desc_arg - 1);_tmp693=_tmp692.f3;{
void*_tmp694=Cyc_Tcutil_compress(_tmp693);struct Cyc_Absyn_PtrInfo _tmp695;void*
_tmp696;struct Cyc_Absyn_Conref*_tmp697;_LL4B1: if(_tmp694 <= (void*)3?1:*((int*)
_tmp694)!= 4)goto _LL4B3;_tmp695=((struct Cyc_Absyn_PointerType_struct*)_tmp694)->f1;
_tmp696=(void*)_tmp695.elt_typ;_tmp697=_tmp695.bounds;_LL4B2:{struct _tuple6
_tmp699=({struct _tuple6 _tmp698;_tmp698.f1=Cyc_Tcutil_compress(_tmp696);_tmp698.f2=
Cyc_Absyn_conref_def((void*)0,_tmp697);_tmp698;});void*_tmp69A;void*_tmp69B;void*
_tmp69C;void*_tmp69D;_LL4B6: _tmp69A=_tmp699.f1;if(_tmp69A <= (void*)3?1:*((int*)
_tmp69A)!= 5)goto _LL4B8;_tmp69B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp69A)->f1;
if((int)_tmp69B != 1)goto _LL4B8;_tmp69C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp69A)->f2;if((int)_tmp69C != 0)goto _LL4B8;_tmp69D=_tmp699.f2;if((int)_tmp69D != 
0)goto _LL4B8;_LL4B7: goto _LL4B5;_LL4B8:;_LL4B9:({void*_tmp69E[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("format descriptor is not a char ? type",sizeof(char),39),_tag_arr(
_tmp69E,sizeof(void*),0));});goto _LL4B5;_LL4B5:;}goto _LL4B0;_LL4B3:;_LL4B4:({
void*_tmp69F[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format descriptor is not a char ? type",
sizeof(char),39),_tag_arr(_tmp69F,sizeof(void*),0));});goto _LL4B0;_LL4B0:;}if(
fmt_arg_start != 0){void*_tmp6A0=Cyc_Tcutil_compress((void*)((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp5F9))->type);int problem;{void*_tmp6A1=ft;_LL4BB: if((int)_tmp6A1
!= 0)goto _LL4BD;_LL4BC:{void*_tmp6A2=_tmp6A0;struct Cyc_Absyn_TunionInfo _tmp6A3;
void*_tmp6A4;struct Cyc_Absyn_Tuniondecl**_tmp6A5;struct Cyc_Absyn_Tuniondecl*
_tmp6A6;_LL4C0: if(_tmp6A2 <= (void*)3?1:*((int*)_tmp6A2)!= 2)goto _LL4C2;_tmp6A3=((
struct Cyc_Absyn_TunionType_struct*)_tmp6A2)->f1;_tmp6A4=(void*)_tmp6A3.tunion_info;
if(*((int*)_tmp6A4)!= 1)goto _LL4C2;_tmp6A5=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp6A4)->f1;_tmp6A6=*_tmp6A5;_LL4C1: problem=Cyc_Absyn_qvar_cmp(_tmp6A6->name,
Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL4BF;_LL4C2:;_LL4C3: problem=1;goto
_LL4BF;_LL4BF:;}goto _LL4BA;_LL4BD: if((int)_tmp6A1 != 1)goto _LL4BA;_LL4BE:{void*
_tmp6A7=_tmp6A0;struct Cyc_Absyn_TunionInfo _tmp6A8;void*_tmp6A9;struct Cyc_Absyn_Tuniondecl**
_tmp6AA;struct Cyc_Absyn_Tuniondecl*_tmp6AB;_LL4C5: if(_tmp6A7 <= (void*)3?1:*((int*)
_tmp6A7)!= 2)goto _LL4C7;_tmp6A8=((struct Cyc_Absyn_TunionType_struct*)_tmp6A7)->f1;
_tmp6A9=(void*)_tmp6A8.tunion_info;if(*((int*)_tmp6A9)!= 1)goto _LL4C7;_tmp6AA=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp6A9)->f1;_tmp6AB=*_tmp6AA;_LL4C6: problem=
Cyc_Absyn_qvar_cmp(_tmp6AB->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL4C4;
_LL4C7:;_LL4C8: problem=1;goto _LL4C4;_LL4C4:;}goto _LL4BA;_LL4BA:;}if(problem)({
void*_tmp6AC[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format attribute and vararg types don't match",
sizeof(char),46),_tag_arr(_tmp6AC,sizeof(void*),0));});}}}{struct Cyc_List_List*
rpo=_tmp5FA;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp6AE;void*_tmp6AF;void*
_tmp6B0;struct _tuple6*_tmp6AD=(struct _tuple6*)rpo->hd;_tmp6AE=*_tmp6AD;_tmp6AF=
_tmp6AE.f1;_tmp6B0=_tmp6AE.f2;_tmp684=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp684,(void*)3,_tmp6AF);_tmp684=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp684,(
void*)3,_tmp6B0);}}if(*_tmp5F5 != 0)_tmp684=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp684,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp5F5))->v);else{
if(cvtenv.generalize_evars);{struct Cyc_List_List*effect=0;{struct Cyc_List_List*
tvs=_tmp684.free_vars;for(0;tvs != 0;tvs=tvs->tl){void*_tmp6B1=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmp6B2;struct
Cyc_Core_Opt**_tmp6B3;void*_tmp6B4;void*_tmp6B5;struct Cyc_Core_Opt*_tmp6B6;
struct Cyc_Core_Opt**_tmp6B7;void*_tmp6B8;void*_tmp6B9;struct Cyc_Core_Opt*_tmp6BA;
struct Cyc_Core_Opt**_tmp6BB;_LL4CA: if(*((int*)_tmp6B1)!= 2)goto _LL4CC;_tmp6B2=((
struct Cyc_Absyn_Less_kb_struct*)_tmp6B1)->f1;_tmp6B3=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp6B1)->f1;_tmp6B4=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp6B1)->f2;if((int)_tmp6B4 != 3)goto _LL4CC;_LL4CB:*_tmp6B3=({struct Cyc_Core_Opt*
_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp6BE;_tmp6BE.tag=0;_tmp6BE.f1=(void*)((void*)3);_tmp6BE;});_tmp6BD;}));
_tmp6BC;});goto _LL4CD;_LL4CC: if(*((int*)_tmp6B1)!= 0)goto _LL4CE;_tmp6B5=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp6B1)->f1;if((int)_tmp6B5 != 3)goto _LL4CE;_LL4CD:
effect=({struct Cyc_List_List*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6C0=_cycalloc(sizeof(*
_tmp6C0));_tmp6C0[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6C1;_tmp6C1.tag=19;
_tmp6C1.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6C2=_cycalloc(
sizeof(*_tmp6C2));_tmp6C2[0]=({struct Cyc_Absyn_VarType_struct _tmp6C3;_tmp6C3.tag=
1;_tmp6C3.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6C3;});_tmp6C2;}));_tmp6C1;});
_tmp6C0;}));_tmp6BF->tl=effect;_tmp6BF;});goto _LL4C9;_LL4CE: if(*((int*)_tmp6B1)
!= 2)goto _LL4D0;_tmp6B6=((struct Cyc_Absyn_Less_kb_struct*)_tmp6B1)->f1;_tmp6B7=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6B1)->f1;_tmp6B8=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6B1)->f2;if((int)_tmp6B8 != 4)goto
_LL4D0;_LL4CF:*_tmp6B7=({struct Cyc_Core_Opt*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));
_tmp6C4->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6C5=_cycalloc(
sizeof(*_tmp6C5));_tmp6C5[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6C6;_tmp6C6.tag=0;
_tmp6C6.f1=(void*)((void*)4);_tmp6C6;});_tmp6C5;}));_tmp6C4;});goto _LL4D1;_LL4D0:
if(*((int*)_tmp6B1)!= 0)goto _LL4D2;_tmp6B9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp6B1)->f1;if((int)_tmp6B9 != 4)goto _LL4D2;_LL4D1: effect=({struct Cyc_List_List*
_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_Absyn_VarType_struct
_tmp6C9;_tmp6C9.tag=1;_tmp6C9.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6C9;});
_tmp6C8;}));_tmp6C7->tl=effect;_tmp6C7;});goto _LL4C9;_LL4D2: if(*((int*)_tmp6B1)
!= 1)goto _LL4D4;_tmp6BA=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6B1)->f1;
_tmp6BB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6B1)->f1;
_LL4D3:*_tmp6BB=({struct Cyc_Core_Opt*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));
_tmp6CB[0]=({struct Cyc_Absyn_Less_kb_struct _tmp6CC;_tmp6CC.tag=2;_tmp6CC.f1=0;
_tmp6CC.f2=(void*)((void*)0);_tmp6CC;});_tmp6CB;}));_tmp6CA;});goto _LL4D5;_LL4D4:;
_LL4D5: effect=({struct Cyc_List_List*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));
_tmp6CE[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6CF;_tmp6CF.tag=21;_tmp6CF.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));
_tmp6D0[0]=({struct Cyc_Absyn_VarType_struct _tmp6D1;_tmp6D1.tag=1;_tmp6D1.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6D1;});_tmp6D0;}));_tmp6CF;});_tmp6CE;}));
_tmp6CD->tl=effect;_tmp6CD;});goto _LL4C9;_LL4C9:;}}effect=Cyc_List_imp_rev(
effect);{struct Cyc_List_List*ts=_tmp684.free_evars;for(0;ts != 0;ts=ts->tl){void*
_tmp6D2=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL4D7: if((int)_tmp6D2 != 3)goto _LL4D9;
_LL4D8: effect=({struct Cyc_List_List*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6D4=_cycalloc(sizeof(*
_tmp6D4));_tmp6D4[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6D5;_tmp6D5.tag=19;
_tmp6D5.f1=(void*)((void*)ts->hd);_tmp6D5;});_tmp6D4;}));_tmp6D3->tl=effect;
_tmp6D3;});goto _LL4D6;_LL4D9: if((int)_tmp6D2 != 4)goto _LL4DB;_LL4DA: effect=({
struct Cyc_List_List*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6->hd=(void*)((
void*)ts->hd);_tmp6D6->tl=effect;_tmp6D6;});goto _LL4D6;_LL4DB:;_LL4DC: effect=({
struct Cyc_List_List*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7->hd=(void*)((
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6D8=_cycalloc(sizeof(*_tmp6D8));
_tmp6D8[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6D9;_tmp6D9.tag=21;_tmp6D9.f1=(
void*)((void*)ts->hd);_tmp6D9;});_tmp6D8;}));_tmp6D7->tl=effect;_tmp6D7;});goto
_LL4D6;_LL4D6:;}}*_tmp5F5=({struct Cyc_Core_Opt*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));
_tmp6DA->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6DB=_cycalloc(
sizeof(*_tmp6DB));_tmp6DB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6DC;_tmp6DC.tag=
20;_tmp6DC.f1=Cyc_List_imp_rev(effect);_tmp6DC;});_tmp6DB;}));_tmp6DA;});}}if(*
_tmp5F3 != 0){struct Cyc_List_List*bs=*_tmp5F3;for(0;bs != 0;bs=bs->tl){void*
_tmp6DD=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp6DE;struct Cyc_Core_Opt**_tmp6DF;struct Cyc_Core_Opt*
_tmp6E0;struct Cyc_Core_Opt**_tmp6E1;void*_tmp6E2;struct Cyc_Core_Opt*_tmp6E3;
struct Cyc_Core_Opt**_tmp6E4;void*_tmp6E5;struct Cyc_Core_Opt*_tmp6E6;struct Cyc_Core_Opt**
_tmp6E7;void*_tmp6E8;void*_tmp6E9;_LL4DE: if(*((int*)_tmp6DD)!= 1)goto _LL4E0;
_tmp6DE=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6DD)->f1;_tmp6DF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp6DD)->f1;_LL4DF:({struct Cyc_Std_String_pa_struct
_tmp6EB;_tmp6EB.tag=0;_tmp6EB.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name;{
void*_tmp6EA[1]={& _tmp6EB};Cyc_Tcutil_warn(loc,_tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof(char),47),_tag_arr(_tmp6EA,sizeof(void*),1));}});_tmp6E1=_tmp6DF;goto
_LL4E1;_LL4E0: if(*((int*)_tmp6DD)!= 2)goto _LL4E2;_tmp6E0=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6DD)->f1;_tmp6E1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6DD)->f1;_tmp6E2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6DD)->f2;if((
int)_tmp6E2 != 1)goto _LL4E2;_LL4E1: _tmp6E4=_tmp6E1;goto _LL4E3;_LL4E2: if(*((int*)
_tmp6DD)!= 2)goto _LL4E4;_tmp6E3=((struct Cyc_Absyn_Less_kb_struct*)_tmp6DD)->f1;
_tmp6E4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6DD)->f1;
_tmp6E5=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6DD)->f2;if((int)_tmp6E5 != 
0)goto _LL4E4;_LL4E3:*_tmp6E4=({struct Cyc_Core_Opt*_tmp6EC=_cycalloc(sizeof(*
_tmp6EC));_tmp6EC->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6ED=
_cycalloc(sizeof(*_tmp6ED));_tmp6ED[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6EE;
_tmp6EE.tag=0;_tmp6EE.f1=(void*)((void*)2);_tmp6EE;});_tmp6ED;}));_tmp6EC;});
goto _LL4DD;_LL4E4: if(*((int*)_tmp6DD)!= 2)goto _LL4E6;_tmp6E6=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6DD)->f1;_tmp6E7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6DD)->f1;_tmp6E8=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6DD)->f2;
_LL4E5:*_tmp6E7=({struct Cyc_Core_Opt*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));
_tmp6F0[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6F1;_tmp6F1.tag=0;_tmp6F1.f1=(void*)
_tmp6E8;_tmp6F1;});_tmp6F0;}));_tmp6EF;});goto _LL4DD;_LL4E6: if(*((int*)_tmp6DD)
!= 0)goto _LL4E8;_tmp6E9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp6DD)->f1;if((
int)_tmp6E9 != 1)goto _LL4E8;_LL4E7:({void*_tmp6F2[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("function's can't abstract M types",sizeof(char),34),_tag_arr(_tmp6F2,
sizeof(void*),0));});goto _LL4DD;_LL4E8:;_LL4E9: goto _LL4DD;_LL4DD:;}}cvtenv.kind_env=
Cyc_Tcutil_remove_bound_tvars(_tmp684.kind_env,*_tmp5F3);_tmp684.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp684.free_vars,*_tmp5F3);{struct Cyc_List_List*tvs=_tmp684.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp684.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL43B;}}_LL45C: if(_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 
9)goto _LL45E;_tmp5FC=((struct Cyc_Absyn_TupleType_struct*)_tmp5D2)->f1;_LL45D:
for(0;_tmp5FC != 0;_tmp5FC=_tmp5FC->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,(void*)1,(*((struct _tuple4*)_tmp5FC->hd)).f2);}goto _LL43B;_LL45E: if(
_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 11)goto _LL460;_tmp5FD=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5D2)->f1;_tmp5FE=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5D2)->f2;_LL45F:{
struct _RegionHandle _tmp6F4=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp6F4;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp5FE != 0;_tmp5FE=_tmp5FE->tl){struct Cyc_Absyn_Aggrfield _tmp6F6;struct
_tagged_arr*_tmp6F7;struct Cyc_Absyn_Tqual _tmp6F8;void*_tmp6F9;struct Cyc_Absyn_Exp*
_tmp6FA;struct Cyc_List_List*_tmp6FB;struct Cyc_Absyn_Aggrfield*_tmp6F5=(struct Cyc_Absyn_Aggrfield*)
_tmp5FE->hd;_tmp6F6=*_tmp6F5;_tmp6F7=_tmp6F6.name;_tmp6F8=_tmp6F6.tq;_tmp6F9=(
void*)_tmp6F6.type;_tmp6FA=_tmp6F6.width;_tmp6FB=_tmp6F6.attributes;if(((int(*)(
int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,
struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,prev_fields,_tmp6F7))({
struct Cyc_Std_String_pa_struct _tmp6FD;_tmp6FD.tag=0;_tmp6FD.f1=(struct
_tagged_arr)*_tmp6F7;{void*_tmp6FC[1]={& _tmp6FD};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s",
sizeof(char),19),_tag_arr(_tmp6FC,sizeof(void*),1));}});if(Cyc_Std_strcmp(*
_tmp6F7,_tag_arr("",sizeof(char),1))!= 0)prev_fields=({struct Cyc_List_List*
_tmp6FE=_region_malloc(aprev_rgn,sizeof(*_tmp6FE));_tmp6FE->hd=_tmp6F7;_tmp6FE->tl=
prev_fields;_tmp6FE;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp6F9);if(_tmp5FD == (void*)1?!Cyc_Tcutil_bits_only(_tmp6F9): 0)({struct Cyc_Std_String_pa_struct
_tmp700;_tmp700.tag=0;_tmp700.f1=(struct _tagged_arr)*_tmp6F7;{void*_tmp6FF[1]={&
_tmp700};Cyc_Tcutil_terr(loc,_tag_arr("union member %s has a non-integral type",
sizeof(char),40),_tag_arr(_tmp6FF,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(
loc,te,_tmp6F9,_tmp6FA,_tmp6F7);Cyc_Tcutil_check_field_atts(loc,_tmp6F7,_tmp6FB);}};
_pop_region(aprev_rgn);}goto _LL43B;_LL460: if(_tmp5D2 <= (void*)3?1:*((int*)
_tmp5D2)!= 10)goto _LL462;_tmp5FF=((struct Cyc_Absyn_AggrType_struct*)_tmp5D2)->f1;
_tmp600=(void*)_tmp5FF.aggr_info;_tmp601=(void**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp5D2)->f1).aggr_info;_tmp602=_tmp5FF.targs;_tmp603=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp5D2)->f1).targs;_LL461:{void*_tmp701=*
_tmp601;void*_tmp702;struct _tuple1*_tmp703;struct Cyc_Absyn_Aggrdecl**_tmp704;
struct Cyc_Absyn_Aggrdecl*_tmp705;_LL4EB: if(*((int*)_tmp701)!= 0)goto _LL4ED;
_tmp702=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp701)->f1;_tmp703=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp701)->f2;_LL4EC: {struct Cyc_Absyn_Aggrdecl**
adp;{struct _handler_cons _tmp706;_push_handler(& _tmp706);{int _tmp708=0;if(setjmp(
_tmp706.handler))_tmp708=1;if(!_tmp708){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,
_tmp703);*_tmp601=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp709=_cycalloc(
sizeof(*_tmp709));_tmp709[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp70A;_tmp70A.tag=
1;_tmp70A.f1=adp;_tmp70A;});_tmp709;});;_pop_handler();}else{void*_tmp707=(void*)
_exn_thrown;void*_tmp70C=_tmp707;_LL4F0: if(_tmp70C != Cyc_Dict_Absent)goto _LL4F2;
_LL4F1: {struct Cyc_Tcenv_Genv*_tmp70D=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*
_tmp70E=({struct Cyc_Absyn_Aggrdecl*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713->kind=(
void*)_tmp702;_tmp713->sc=(void*)((void*)3);_tmp713->name=_tmp703;_tmp713->tvs=0;
_tmp713->impl=0;_tmp713->attributes=0;_tmp713;});Cyc_Tc_tcAggrdecl(te,_tmp70D,
loc,_tmp70E);adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp703);*_tmp601=(void*)({
struct Cyc_Absyn_KnownAggr_struct*_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F[0]=({
struct Cyc_Absyn_KnownAggr_struct _tmp710;_tmp710.tag=1;_tmp710.f1=adp;_tmp710;});
_tmp70F;});if(*_tmp603 != 0){({struct Cyc_Std_String_pa_struct _tmp712;_tmp712.tag=
0;_tmp712.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp703);{void*_tmp711[1]={&
_tmp712};Cyc_Tcutil_terr(loc,_tag_arr("declare parameterized type %s before using",
sizeof(char),43),_tag_arr(_tmp711,sizeof(void*),1));}});return cvtenv;}goto _LL4EF;}
_LL4F2:;_LL4F3:(void)_throw(_tmp70C);_LL4EF:;}}}_tmp705=*adp;goto _LL4EE;}_LL4ED:
if(*((int*)_tmp701)!= 1)goto _LL4EA;_tmp704=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp701)->f1;_tmp705=*_tmp704;_LL4EE: {struct Cyc_List_List*tvs=_tmp705->tvs;
struct Cyc_List_List*ts=*_tmp603;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_Std_String_pa_struct
_tmp715;_tmp715.tag=0;_tmp715.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp705->name);{void*_tmp714[1]={& _tmp715};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for type %s",
sizeof(char),32),_tag_arr(_tmp714,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp716=
_cycalloc(sizeof(*_tmp716));_tmp716->hd=(void*)e;_tmp716->tl=hidden_ts;_tmp716;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*_tmp603=Cyc_List_imp_append(*
_tmp603,Cyc_List_imp_rev(hidden_ts));}}_LL4EA:;}goto _LL43B;_LL462: if(_tmp5D2 <= (
void*)3?1:*((int*)_tmp5D2)!= 16)goto _LL464;_tmp604=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5D2)->f1;_tmp605=((struct Cyc_Absyn_TypedefType_struct*)_tmp5D2)->f2;_tmp606=(
struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp5D2)->f2;
_tmp607=((struct Cyc_Absyn_TypedefType_struct*)_tmp5D2)->f3;_tmp608=(struct Cyc_Absyn_Typedefdecl**)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp5D2)->f3;_tmp609=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5D2)->f4;_tmp60A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp5D2)->f4;_LL463: {struct Cyc_List_List*targs=*_tmp606;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmp717;_push_handler(& _tmp717);{int _tmp719=0;if(setjmp(
_tmp717.handler))_tmp719=1;if(!_tmp719){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmp604);;_pop_handler();}else{void*_tmp718=(void*)_exn_thrown;void*_tmp71B=
_tmp718;_LL4F5: if(_tmp71B != Cyc_Dict_Absent)goto _LL4F7;_LL4F6:({struct Cyc_Std_String_pa_struct
_tmp71D;_tmp71D.tag=0;_tmp71D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp604);{void*_tmp71C[1]={& _tmp71D};Cyc_Tcutil_terr(loc,_tag_arr("unbound typedef name %s",
sizeof(char),24),_tag_arr(_tmp71C,sizeof(void*),1));}});return cvtenv;_LL4F7:;
_LL4F8:(void)_throw(_tmp71B);_LL4F4:;}}}*_tmp608=(struct Cyc_Absyn_Typedefdecl*)
td;_tmp604[0]=(td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0?
tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->hd=({
struct _tuple7*_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp71F->f2=(void*)ts->hd;_tmp71F;});_tmp71E->tl=inst;_tmp71E;});}if(ts
!= 0)({struct Cyc_Std_String_pa_struct _tmp721;_tmp721.tag=0;_tmp721.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp604);{void*_tmp720[1]={& _tmp721};Cyc_Tcutil_terr(
loc,_tag_arr("too many parameters for typedef %s",sizeof(char),35),_tag_arr(
_tmp720,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp722=_cycalloc(
sizeof(*_tmp722));_tmp722->hd=(void*)e;_tmp722->tl=hidden_ts;_tmp722;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723->hd=({struct _tuple7*_tmp724=
_cycalloc(sizeof(*_tmp724));_tmp724->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp724->f2=
e;_tmp724;});_tmp723->tl=inst;_tmp723;});}*_tmp606=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp60A=({struct Cyc_Core_Opt*
_tmp725=_cycalloc(sizeof(*_tmp725));_tmp725->v=(void*)new_typ;_tmp725;});}goto
_LL43B;}}_LL464: if((int)_tmp5D2 != 2)goto _LL466;_LL465: goto _LL43B;_LL466: if(
_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 15)goto _LL468;_tmp60B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5D2)->f1;_LL467: _tmp60C=_tmp60B;goto _LL469;_LL468: if(_tmp5D2 <= (void*)3?1:*((
int*)_tmp5D2)!= 19)goto _LL46A;_tmp60C=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp5D2)->f1;_LL469: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp60C);goto _LL43B;_LL46A: if(_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 21)goto
_LL46C;_tmp60D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp5D2)->f1;_LL46B:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp60D);goto _LL43B;
_LL46C: if(_tmp5D2 <= (void*)3?1:*((int*)_tmp5D2)!= 20)goto _LL43B;_tmp60E=((struct
Cyc_Absyn_JoinEff_struct*)_tmp5D2)->f1;_LL46D: for(0;_tmp60E != 0;_tmp60E=_tmp60E->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)_tmp60E->hd);}
goto _LL43B;_LL43B:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))({
struct Cyc_Std_String_pa_struct _tmp729;_tmp729.tag=0;_tmp729.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(expected_kind);{struct Cyc_Std_String_pa_struct
_tmp728;_tmp728.tag=0;_tmp728.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t));{struct Cyc_Std_String_pa_struct _tmp727;_tmp727.tag=0;_tmp727.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp726[3]={& _tmp727,& _tmp728,&
_tmp729};Cyc_Tcutil_terr(loc,_tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof(char),51),_tag_arr(_tmp726,sizeof(void*),3));}}}});return cvtenv;}static
struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp72A=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp733;_tmp733.kind_env=kind_env;_tmp733.free_vars=
0;_tmp733.free_evars=0;_tmp733.generalize_evars=generalize_evars;_tmp733.fn_result=
0;_tmp733;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp72A.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp72A.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp72A.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp72B=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp72C;struct Cyc_Core_Opt**_tmp72D;_LL4FA: if(_tmp72B <= (void*)3?1:*((int*)
_tmp72B)!= 0)goto _LL4FC;_tmp72C=((struct Cyc_Absyn_Evar_struct*)_tmp72B)->f4;
_tmp72D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp72B)->f4;
_LL4FB: if(*_tmp72D == 0)*_tmp72D=({struct Cyc_Core_Opt*_tmp72E=_cycalloc(sizeof(*
_tmp72E));_tmp72E->v=kind_env;_tmp72E;});else{struct Cyc_List_List*_tmp72F=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp72D))->v;struct Cyc_List_List*
_tmp730=0;for(0;_tmp72F != 0;_tmp72F=_tmp72F->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp72F->hd))_tmp730=({struct Cyc_List_List*_tmp731=_cycalloc(sizeof(*_tmp731));
_tmp731->hd=(struct Cyc_Absyn_Tvar*)_tmp72F->hd;_tmp731->tl=_tmp730;_tmp731;});}*
_tmp72D=({struct Cyc_Core_Opt*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732->v=
_tmp730;_tmp732;});}goto _LL4F9;_LL4FC:;_LL4FD: goto _LL4F9;_LL4F9:;}}return _tmp72A;}
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_Tcutil_CVTEnv _tmp734=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp735=_tmp734.free_vars;struct Cyc_List_List*
_tmp736=_tmp734.free_evars;{struct Cyc_List_List*x=_tmp735;for(0;x != 0;x=x->tl){
void*_tmp737=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp738;struct Cyc_Core_Opt**_tmp739;struct Cyc_Core_Opt*
_tmp73A;struct Cyc_Core_Opt**_tmp73B;void*_tmp73C;struct Cyc_Core_Opt*_tmp73D;
struct Cyc_Core_Opt**_tmp73E;void*_tmp73F;struct Cyc_Core_Opt*_tmp740;struct Cyc_Core_Opt**
_tmp741;void*_tmp742;void*_tmp743;_LL4FF: if(*((int*)_tmp737)!= 1)goto _LL501;
_tmp738=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp737)->f1;_tmp739=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp737)->f1;_LL500: _tmp73B=_tmp739;goto
_LL502;_LL501: if(*((int*)_tmp737)!= 2)goto _LL503;_tmp73A=((struct Cyc_Absyn_Less_kb_struct*)
_tmp737)->f1;_tmp73B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp737)->f1;_tmp73C=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp737)->f2;if((
int)_tmp73C != 1)goto _LL503;_LL502: _tmp73E=_tmp73B;goto _LL504;_LL503: if(*((int*)
_tmp737)!= 2)goto _LL505;_tmp73D=((struct Cyc_Absyn_Less_kb_struct*)_tmp737)->f1;
_tmp73E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp737)->f1;
_tmp73F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp737)->f2;if((int)_tmp73F != 
0)goto _LL505;_LL504:*_tmp73E=({struct Cyc_Core_Opt*_tmp744=_cycalloc(sizeof(*
_tmp744));_tmp744->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp745=
_cycalloc(sizeof(*_tmp745));_tmp745[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp746;
_tmp746.tag=0;_tmp746.f1=(void*)((void*)2);_tmp746;});_tmp745;}));_tmp744;});
goto _LL4FE;_LL505: if(*((int*)_tmp737)!= 2)goto _LL507;_tmp740=((struct Cyc_Absyn_Less_kb_struct*)
_tmp737)->f1;_tmp741=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp737)->f1;_tmp742=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp737)->f2;
_LL506:*_tmp741=({struct Cyc_Core_Opt*_tmp747=_cycalloc(sizeof(*_tmp747));_tmp747->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp748=_cycalloc(sizeof(*_tmp748));
_tmp748[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp749;_tmp749.tag=0;_tmp749.f1=(void*)
_tmp742;_tmp749;});_tmp748;}));_tmp747;});goto _LL4FE;_LL507: if(*((int*)_tmp737)
!= 0)goto _LL509;_tmp743=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp737)->f1;if((
int)_tmp743 != 1)goto _LL509;_LL508:({struct Cyc_Std_String_pa_struct _tmp74B;
_tmp74B.tag=0;_tmp74B.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd);{void*_tmp74A[1]={& _tmp74B};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s cannot have kind M",
sizeof(char),36),_tag_arr(_tmp74A,sizeof(void*),1));}});goto _LL4FE;_LL509:;
_LL50A: goto _LL4FE;_LL4FE:;}}if(_tmp735 != 0?1: _tmp736 != 0){{void*_tmp74C=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp74D;struct Cyc_List_List*_tmp74E;struct Cyc_List_List**
_tmp74F;struct Cyc_Core_Opt*_tmp750;void*_tmp751;struct Cyc_List_List*_tmp752;int
_tmp753;struct Cyc_Absyn_VarargInfo*_tmp754;struct Cyc_List_List*_tmp755;struct Cyc_List_List*
_tmp756;_LL50C: if(_tmp74C <= (void*)3?1:*((int*)_tmp74C)!= 8)goto _LL50E;_tmp74D=((
struct Cyc_Absyn_FnType_struct*)_tmp74C)->f1;_tmp74E=_tmp74D.tvars;_tmp74F=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp74C)->f1).tvars;
_tmp750=_tmp74D.effect;_tmp751=(void*)_tmp74D.ret_typ;_tmp752=_tmp74D.args;
_tmp753=_tmp74D.c_varargs;_tmp754=_tmp74D.cyc_varargs;_tmp755=_tmp74D.rgn_po;
_tmp756=_tmp74D.attributes;_LL50D: if(*_tmp74F == 0){*_tmp74F=_tmp735;_tmp735=0;}
goto _LL50B;_LL50E:;_LL50F: goto _LL50B;_LL50B:;}if(_tmp735 != 0)({struct Cyc_Std_String_pa_struct
_tmp758;_tmp758.tag=0;_tmp758.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)
_tmp735->hd)->name;{void*_tmp757[1]={& _tmp758};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s",
sizeof(char),25),_tag_arr(_tmp757,sizeof(void*),1));}});if(_tmp736 != 0)for(0;
_tmp736 != 0;_tmp736=_tmp736->tl){void*e=(void*)_tmp736->hd;void*_tmp759=Cyc_Tcutil_typ_kind(
e);_LL511: if((int)_tmp759 != 3)goto _LL513;_LL512: if(!Cyc_Tcutil_unify(e,(void*)2))({
void*_tmp75A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with heap!",sizeof(char),28),_tag_arr(_tmp75A,sizeof(
void*),0));});goto _LL510;_LL513: if((int)_tmp759 != 4)goto _LL515;_LL514: if(!Cyc_Tcutil_unify(
e,Cyc_Absyn_empty_effect))({void*_tmp75B[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(char),26),_tag_arr(_tmp75B,sizeof(void*),0));});goto _LL510;_LL515:;_LL516:({
struct Cyc_Std_String_pa_struct _tmp75E;_tmp75E.tag=0;_tmp75E.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp75D;
_tmp75D.tag=0;_tmp75D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp75C[2]={& _tmp75D,& _tmp75E};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp75C,sizeof(void*),2));}}});goto _LL510;_LL510:;}}}
void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(
loc,te,t);{void*_tmp75F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp760;
struct Cyc_List_List*_tmp761;struct Cyc_Core_Opt*_tmp762;void*_tmp763;struct Cyc_List_List*
_tmp764;int _tmp765;struct Cyc_Absyn_VarargInfo*_tmp766;struct Cyc_List_List*
_tmp767;struct Cyc_List_List*_tmp768;_LL518: if(_tmp75F <= (void*)3?1:*((int*)
_tmp75F)!= 8)goto _LL51A;_tmp760=((struct Cyc_Absyn_FnType_struct*)_tmp75F)->f1;
_tmp761=_tmp760.tvars;_tmp762=_tmp760.effect;_tmp763=(void*)_tmp760.ret_typ;
_tmp764=_tmp760.args;_tmp765=_tmp760.c_varargs;_tmp766=_tmp760.cyc_varargs;
_tmp767=_tmp760.rgn_po;_tmp768=_tmp760.attributes;_LL519: fd->tvs=_tmp761;fd->effect=
_tmp762;goto _LL517;_LL51A:;_LL51B:({void*_tmp769[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_fndecl_valid_type: not a FnType",
sizeof(char),38),_tag_arr(_tmp769,sizeof(void*),0));});_LL517:;}{struct
_RegionHandle _tmp76A=_new_region("r");struct _RegionHandle*r=& _tmp76A;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct _tuple14*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct _tuple14*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,_tag_arr("function declaration has repeated parameter",sizeof(char),
44));;_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp76B=_cycalloc(
sizeof(*_tmp76B));_tmp76B->v=(void*)t;_tmp76B;});}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp76C=Cyc_Tcutil_check_valid_type(loc,te,bound_tvars,expected_kind,0,t);struct
Cyc_List_List*_tmp76D=Cyc_Tcutil_remove_bound_tvars(_tmp76C.free_vars,
bound_tvars);struct Cyc_List_List*_tmp76E=_tmp76C.free_evars;{struct Cyc_List_List*
fs=_tmp76D;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp76F=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;({struct Cyc_Std_String_pa_struct _tmp772;_tmp772.tag=0;_tmp772.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp771;_tmp771.tag=0;_tmp771.f1=(struct _tagged_arr)*_tmp76F;{void*_tmp770[2]={&
_tmp771,& _tmp772};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s in type %s",
sizeof(char),36),_tag_arr(_tmp770,sizeof(void*),2));}}});}}if(!allow_evars?
_tmp76E != 0: 0)for(0;_tmp76E != 0;_tmp76E=_tmp76E->tl){void*e=(void*)_tmp76E->hd;
void*_tmp773=Cyc_Tcutil_typ_kind(e);_LL51D: if((int)_tmp773 != 3)goto _LL51F;_LL51E:
if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp774[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(char),28),_tag_arr(_tmp774,sizeof(void*),0));});goto _LL51C;_LL51F: if((int)
_tmp773 != 4)goto _LL521;_LL520: if(!Cyc_Tcutil_unify(e,(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp776;_tmp776.tag=20;_tmp776.f1=0;_tmp776;});_tmp775;})))({void*_tmp777[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(char),26),_tag_arr(_tmp777,sizeof(void*),0));});goto _LL51C;_LL521:;_LL522:({
struct Cyc_Std_String_pa_struct _tmp77A;_tmp77A.tag=0;_tmp77A.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp779;
_tmp779.tag=0;_tmp779.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp778[2]={& _tmp779,& _tmp77A};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp778,sizeof(void*),2));}}});goto _LL51C;_LL51C:;}}
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=
Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*
tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _tagged_arr(*a2string)(void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0)({struct Cyc_Std_String_pa_struct _tmp77D;_tmp77D.tag=0;_tmp77D.f1=(
struct _tagged_arr)a2string((void*)vs->hd);{struct Cyc_Std_String_pa_struct _tmp77C;
_tmp77C.tag=0;_tmp77C.f1=(struct _tagged_arr)msg;{void*_tmp77B[2]={& _tmp77C,&
_tmp77D};Cyc_Tcutil_terr(loc,_tag_arr("%s: %s",sizeof(char),7),_tag_arr(_tmp77B,
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
fields=({struct Cyc_List_List*_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E->hd=({
struct _tuple19*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp77F->f2=0;_tmp77F;});_tmp77E->tl=fields;_tmp77E;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple20 _tmp781;struct Cyc_List_List*
_tmp782;void*_tmp783;struct _tuple20*_tmp780=(struct _tuple20*)des->hd;_tmp781=*
_tmp780;_tmp782=_tmp781.f1;_tmp783=_tmp781.f2;if(_tmp782 == 0){struct Cyc_List_List*
_tmp784=fields;for(0;_tmp784 != 0;_tmp784=_tmp784->tl){if(!(*((struct _tuple19*)
_tmp784->hd)).f2){(*((struct _tuple19*)_tmp784->hd)).f2=1;(*((struct _tuple20*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp785=_cycalloc(sizeof(*_tmp785));
_tmp785->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp786=_cycalloc(
sizeof(*_tmp786));_tmp786[0]=({struct Cyc_Absyn_FieldName_struct _tmp787;_tmp787.tag=
1;_tmp787.f1=((*((struct _tuple19*)_tmp784->hd)).f1)->name;_tmp787;});_tmp786;}));
_tmp785->tl=0;_tmp785;});ans=({struct Cyc_List_List*_tmp788=_region_malloc(rgn,
sizeof(*_tmp788));_tmp788->hd=({struct _tuple21*_tmp789=_region_malloc(rgn,
sizeof(*_tmp789));_tmp789->f1=(*((struct _tuple19*)_tmp784->hd)).f1;_tmp789->f2=
_tmp783;_tmp789;});_tmp788->tl=ans;_tmp788;});break;}}if(_tmp784 == 0)({void*
_tmp78A[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too many arguments to struct",
sizeof(char),29),_tag_arr(_tmp78A,sizeof(void*),0));});}else{if(_tmp782->tl != 0)({
void*_tmp78B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("multiple designators are not supported",
sizeof(char),39),_tag_arr(_tmp78B,sizeof(void*),0));});else{void*_tmp78C=(void*)
_tmp782->hd;struct _tagged_arr*_tmp78D;_LL524: if(*((int*)_tmp78C)!= 0)goto _LL526;
_LL525:({void*_tmp78E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array designator used in argument to struct",
sizeof(char),44),_tag_arr(_tmp78E,sizeof(void*),0));});goto _LL523;_LL526: if(*((
int*)_tmp78C)!= 1)goto _LL523;_tmp78D=((struct Cyc_Absyn_FieldName_struct*)_tmp78C)->f1;
_LL527: {struct Cyc_List_List*_tmp78F=fields;for(0;_tmp78F != 0;_tmp78F=_tmp78F->tl){
if(Cyc_Std_zstrptrcmp(_tmp78D,((*((struct _tuple19*)_tmp78F->hd)).f1)->name)== 0){
if((*((struct _tuple19*)_tmp78F->hd)).f2)({struct Cyc_Std_String_pa_struct _tmp791;
_tmp791.tag=0;_tmp791.f1=(struct _tagged_arr)*_tmp78D;{void*_tmp790[1]={& _tmp791};
Cyc_Tcutil_terr(loc,_tag_arr("field %s has already been used as an argument",
sizeof(char),46),_tag_arr(_tmp790,sizeof(void*),1));}});(*((struct _tuple19*)
_tmp78F->hd)).f2=1;ans=({struct Cyc_List_List*_tmp792=_region_malloc(rgn,sizeof(*
_tmp792));_tmp792->hd=({struct _tuple21*_tmp793=_region_malloc(rgn,sizeof(*
_tmp793));_tmp793->f1=(*((struct _tuple19*)_tmp78F->hd)).f1;_tmp793->f2=_tmp783;
_tmp793;});_tmp792->tl=ans;_tmp792;});break;}}if(_tmp78F == 0)({struct Cyc_Std_String_pa_struct
_tmp795;_tmp795.tag=0;_tmp795.f1=(struct _tagged_arr)*_tmp78D;{void*_tmp794[1]={&
_tmp795};Cyc_Tcutil_terr(loc,_tag_arr("bad field designator %s",sizeof(char),24),
_tag_arr(_tmp794,sizeof(void*),1));}});goto _LL523;}_LL523:;}}}for(0;fields != 0;
fields=fields->tl){if(!(*((struct _tuple19*)fields->hd)).f2){({void*_tmp796[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments to struct",sizeof(char),28),
_tag_arr(_tmp796,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp797=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp798;void*_tmp799;struct Cyc_Absyn_Conref*_tmp79A;_LL529: if(_tmp797 <= (void*)3?
1:*((int*)_tmp797)!= 4)goto _LL52B;_tmp798=((struct Cyc_Absyn_PointerType_struct*)
_tmp797)->f1;_tmp799=(void*)_tmp798.elt_typ;_tmp79A=_tmp798.bounds;_LL52A: {
struct Cyc_Absyn_Conref*_tmp79B=Cyc_Absyn_compress_conref(_tmp79A);void*_tmp79C=(
void*)(Cyc_Absyn_compress_conref(_tmp79B))->v;void*_tmp79D;_LL52E: if(_tmp79C <= (
void*)1?1:*((int*)_tmp79C)!= 0)goto _LL530;_tmp79D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp79C)->f1;if((int)_tmp79D != 0)goto _LL530;_LL52F:*elt_typ_dest=_tmp799;return 1;
_LL530: if((int)_tmp79C != 0)goto _LL532;_LL531:(void*)(_tmp79B->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp79E=_cycalloc(sizeof(*_tmp79E));_tmp79E[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp79F;_tmp79F.tag=0;_tmp79F.f1=(void*)((void*)
0);_tmp79F;});_tmp79E;})));*elt_typ_dest=_tmp799;return 1;_LL532:;_LL533: return 0;
_LL52D:;}_LL52B:;_LL52C: return 0;_LL528:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple9 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple9 bogus_ans=({struct _tuple9 _tmp7DD;_tmp7DD.f1=0;
_tmp7DD.f2=(void*)2;_tmp7DD;});void*_tmp7A0=(void*)e->r;struct _tuple1*_tmp7A1;
void*_tmp7A2;struct Cyc_Absyn_Exp*_tmp7A3;struct _tagged_arr*_tmp7A4;struct Cyc_Absyn_Exp*
_tmp7A5;struct _tagged_arr*_tmp7A6;struct Cyc_Absyn_Exp*_tmp7A7;struct Cyc_Absyn_Exp*
_tmp7A8;struct Cyc_Absyn_Exp*_tmp7A9;_LL535: if(*((int*)_tmp7A0)!= 1)goto _LL537;
_tmp7A1=((struct Cyc_Absyn_Var_e_struct*)_tmp7A0)->f1;_tmp7A2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp7A0)->f2;_LL536: {void*_tmp7AA=_tmp7A2;struct Cyc_Absyn_Vardecl*_tmp7AB;
struct Cyc_Absyn_Vardecl*_tmp7AC;struct Cyc_Absyn_Vardecl*_tmp7AD;struct Cyc_Absyn_Vardecl*
_tmp7AE;_LL542: if((int)_tmp7AA != 0)goto _LL544;_LL543: goto _LL545;_LL544: if(
_tmp7AA <= (void*)1?1:*((int*)_tmp7AA)!= 1)goto _LL546;_LL545: return bogus_ans;
_LL546: if(_tmp7AA <= (void*)1?1:*((int*)_tmp7AA)!= 0)goto _LL548;_tmp7AB=((struct
Cyc_Absyn_Global_b_struct*)_tmp7AA)->f1;_LL547: {void*_tmp7AF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL54F: if(_tmp7AF <= (void*)
3?1:*((int*)_tmp7AF)!= 7)goto _LL551;_LL550: return({struct _tuple9 _tmp7B0;_tmp7B0.f1=
1;_tmp7B0.f2=(void*)2;_tmp7B0;});_LL551:;_LL552: return({struct _tuple9 _tmp7B1;
_tmp7B1.f1=(_tmp7AB->tq).q_const;_tmp7B1.f2=(void*)2;_tmp7B1;});_LL54E:;}_LL548:
if(_tmp7AA <= (void*)1?1:*((int*)_tmp7AA)!= 3)goto _LL54A;_tmp7AC=((struct Cyc_Absyn_Local_b_struct*)
_tmp7AA)->f1;_LL549: {void*_tmp7B2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL554: if(_tmp7B2 <= (void*)3?1:*((int*)_tmp7B2)!= 7)
goto _LL556;_LL555: return({struct _tuple9 _tmp7B3;_tmp7B3.f1=1;_tmp7B3.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp7AC->rgn))->v;_tmp7B3;});_LL556:;_LL557:
_tmp7AC->escapes=1;return({struct _tuple9 _tmp7B4;_tmp7B4.f1=(_tmp7AC->tq).q_const;
_tmp7B4.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7AC->rgn))->v;_tmp7B4;});
_LL553:;}_LL54A: if(_tmp7AA <= (void*)1?1:*((int*)_tmp7AA)!= 4)goto _LL54C;_tmp7AD=((
struct Cyc_Absyn_Pat_b_struct*)_tmp7AA)->f1;_LL54B: _tmp7AE=_tmp7AD;goto _LL54D;
_LL54C: if(_tmp7AA <= (void*)1?1:*((int*)_tmp7AA)!= 2)goto _LL541;_tmp7AE=((struct
Cyc_Absyn_Param_b_struct*)_tmp7AA)->f1;_LL54D: _tmp7AE->escapes=1;return({struct
_tuple9 _tmp7B5;_tmp7B5.f1=(_tmp7AE->tq).q_const;_tmp7B5.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7AE->rgn))->v;_tmp7B5;});_LL541:;}_LL537: if(*((int*)_tmp7A0)!= 
21)goto _LL539;_tmp7A3=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp7A0)->f1;
_tmp7A4=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp7A0)->f2;_LL538: {void*
_tmp7B6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7A3->topt))->v);
struct Cyc_List_List*_tmp7B7;struct Cyc_Absyn_AggrInfo _tmp7B8;void*_tmp7B9;struct
Cyc_Absyn_Aggrdecl**_tmp7BA;struct Cyc_Absyn_Aggrdecl*_tmp7BB;_LL559: if(_tmp7B6 <= (
void*)3?1:*((int*)_tmp7B6)!= 11)goto _LL55B;_tmp7B7=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp7B6)->f2;_LL55A: {struct Cyc_Absyn_Aggrfield*_tmp7BC=Cyc_Absyn_lookup_field(
_tmp7B7,_tmp7A4);if(_tmp7BC != 0?_tmp7BC->width != 0: 0)return({struct _tuple9
_tmp7BD;_tmp7BD.f1=(_tmp7BC->tq).q_const;_tmp7BD.f2=(Cyc_Tcutil_addressof_props(
te,_tmp7A3)).f2;_tmp7BD;});return bogus_ans;}_LL55B: if(_tmp7B6 <= (void*)3?1:*((
int*)_tmp7B6)!= 10)goto _LL55D;_tmp7B8=((struct Cyc_Absyn_AggrType_struct*)_tmp7B6)->f1;
_tmp7B9=(void*)_tmp7B8.aggr_info;if(*((int*)_tmp7B9)!= 1)goto _LL55D;_tmp7BA=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp7B9)->f1;_tmp7BB=*_tmp7BA;_LL55C: {struct
Cyc_Absyn_Aggrfield*_tmp7BE=Cyc_Absyn_lookup_decl_field(_tmp7BB,_tmp7A4);if(
_tmp7BE != 0?_tmp7BE->width != 0: 0)return({struct _tuple9 _tmp7BF;_tmp7BF.f1=(
_tmp7BE->tq).q_const;_tmp7BF.f2=(Cyc_Tcutil_addressof_props(te,_tmp7A3)).f2;
_tmp7BF;});return bogus_ans;}_LL55D:;_LL55E: return bogus_ans;_LL558:;}_LL539: if(*((
int*)_tmp7A0)!= 22)goto _LL53B;_tmp7A5=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp7A0)->f1;_tmp7A6=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp7A0)->f2;_LL53A: {
void*_tmp7C0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7A5->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7C1;void*_tmp7C2;void*_tmp7C3;
_LL560: if(_tmp7C0 <= (void*)3?1:*((int*)_tmp7C0)!= 4)goto _LL562;_tmp7C1=((struct
Cyc_Absyn_PointerType_struct*)_tmp7C0)->f1;_tmp7C2=(void*)_tmp7C1.elt_typ;
_tmp7C3=(void*)_tmp7C1.rgn_typ;_LL561: {struct Cyc_Absyn_Aggrfield*finfo;{void*
_tmp7C4=Cyc_Tcutil_compress(_tmp7C2);struct Cyc_List_List*_tmp7C5;struct Cyc_Absyn_AggrInfo
_tmp7C6;void*_tmp7C7;struct Cyc_Absyn_Aggrdecl**_tmp7C8;struct Cyc_Absyn_Aggrdecl*
_tmp7C9;_LL565: if(_tmp7C4 <= (void*)3?1:*((int*)_tmp7C4)!= 11)goto _LL567;_tmp7C5=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp7C4)->f2;_LL566: finfo=Cyc_Absyn_lookup_field(
_tmp7C5,_tmp7A6);goto _LL564;_LL567: if(_tmp7C4 <= (void*)3?1:*((int*)_tmp7C4)!= 10)
goto _LL569;_tmp7C6=((struct Cyc_Absyn_AggrType_struct*)_tmp7C4)->f1;_tmp7C7=(void*)
_tmp7C6.aggr_info;if(*((int*)_tmp7C7)!= 1)goto _LL569;_tmp7C8=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp7C7)->f1;_tmp7C9=*_tmp7C8;_LL568: finfo=Cyc_Absyn_lookup_decl_field(_tmp7C9,
_tmp7A6);goto _LL564;_LL569:;_LL56A: return bogus_ans;_LL564:;}if(finfo != 0?finfo->width
!= 0: 0)return({struct _tuple9 _tmp7CA;_tmp7CA.f1=(finfo->tq).q_const;_tmp7CA.f2=
_tmp7C3;_tmp7CA;});return bogus_ans;}_LL562:;_LL563: return bogus_ans;_LL55F:;}
_LL53B: if(*((int*)_tmp7A0)!= 20)goto _LL53D;_tmp7A7=((struct Cyc_Absyn_Deref_e_struct*)
_tmp7A0)->f1;_LL53C: {void*_tmp7CB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7A7->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7CC;void*_tmp7CD;
struct Cyc_Absyn_Tqual _tmp7CE;_LL56C: if(_tmp7CB <= (void*)3?1:*((int*)_tmp7CB)!= 4)
goto _LL56E;_tmp7CC=((struct Cyc_Absyn_PointerType_struct*)_tmp7CB)->f1;_tmp7CD=(
void*)_tmp7CC.rgn_typ;_tmp7CE=_tmp7CC.tq;_LL56D: return({struct _tuple9 _tmp7CF;
_tmp7CF.f1=_tmp7CE.q_const;_tmp7CF.f2=_tmp7CD;_tmp7CF;});_LL56E:;_LL56F: return
bogus_ans;_LL56B:;}_LL53D: if(*((int*)_tmp7A0)!= 23)goto _LL53F;_tmp7A8=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp7A0)->f1;_tmp7A9=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp7A0)->f2;_LL53E: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7A8->topt))->v);void*_tmp7D0=t;struct Cyc_List_List*_tmp7D1;
struct Cyc_Absyn_PtrInfo _tmp7D2;void*_tmp7D3;void*_tmp7D4;struct Cyc_Absyn_Tqual
_tmp7D5;struct Cyc_Absyn_Conref*_tmp7D6;struct Cyc_Absyn_Tqual _tmp7D7;_LL571: if(
_tmp7D0 <= (void*)3?1:*((int*)_tmp7D0)!= 9)goto _LL573;_tmp7D1=((struct Cyc_Absyn_TupleType_struct*)
_tmp7D0)->f1;_LL572: {struct _tuple4*_tmp7D8=Cyc_Absyn_lookup_tuple_field(_tmp7D1,(
int)Cyc_Evexp_eval_const_uint_exp(_tmp7A9));if(_tmp7D8 != 0)return({struct _tuple9
_tmp7D9;_tmp7D9.f1=((*_tmp7D8).f1).q_const;_tmp7D9.f2=(Cyc_Tcutil_addressof_props(
te,_tmp7A8)).f2;_tmp7D9;});return bogus_ans;}_LL573: if(_tmp7D0 <= (void*)3?1:*((
int*)_tmp7D0)!= 4)goto _LL575;_tmp7D2=((struct Cyc_Absyn_PointerType_struct*)
_tmp7D0)->f1;_tmp7D3=(void*)_tmp7D2.elt_typ;_tmp7D4=(void*)_tmp7D2.rgn_typ;
_tmp7D5=_tmp7D2.tq;_tmp7D6=_tmp7D2.bounds;_LL574: return({struct _tuple9 _tmp7DA;
_tmp7DA.f1=_tmp7D5.q_const;_tmp7DA.f2=_tmp7D4;_tmp7DA;});_LL575: if(_tmp7D0 <= (
void*)3?1:*((int*)_tmp7D0)!= 7)goto _LL577;_tmp7D7=((struct Cyc_Absyn_ArrayType_struct*)
_tmp7D0)->f2;_LL576: return({struct _tuple9 _tmp7DB;_tmp7DB.f1=_tmp7D7.q_const;
_tmp7DB.f2=(Cyc_Tcutil_addressof_props(te,_tmp7A8)).f2;_tmp7DB;});_LL577:;_LL578:
return bogus_ans;_LL570:;}_LL53F:;_LL540:({void*_tmp7DC[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("unary & applied to non-lvalue",sizeof(char),30),_tag_arr(_tmp7DC,
sizeof(void*),0));});return bogus_ans;_LL534:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp7DE=Cyc_Tcutil_compress(
e_typ);void*_tmp7DF;struct Cyc_Absyn_Tqual _tmp7E0;_LL57A: if(_tmp7DE <= (void*)3?1:*((
int*)_tmp7DE)!= 7)goto _LL57C;_tmp7DF=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp7DE)->f1;_tmp7E0=((struct Cyc_Absyn_ArrayType_struct*)_tmp7DE)->f2;_LL57B: {
void*_tmp7E2;struct _tuple9 _tmp7E1=Cyc_Tcutil_addressof_props(te,e);_tmp7E2=
_tmp7E1.f2;return Cyc_Absyn_atb_typ(_tmp7DF,_tmp7E2,_tmp7E0,(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp7E3=_cycalloc(sizeof(*_tmp7E3));_tmp7E3[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp7E4;_tmp7E4.tag=0;_tmp7E4.f1=e;_tmp7E4;});_tmp7E3;}));}_LL57C:;_LL57D: return
e_typ;_LL579:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,
unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(b);{void*
_tmp7E5=(void*)b->v;void*_tmp7E6;void*_tmp7E7;struct Cyc_Absyn_Exp*_tmp7E8;_LL57F:
if(_tmp7E5 <= (void*)1?1:*((int*)_tmp7E5)!= 0)goto _LL581;_tmp7E6=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp7E5)->f1;if((int)_tmp7E6 != 0)goto _LL581;_LL580:
return;_LL581: if(_tmp7E5 <= (void*)1?1:*((int*)_tmp7E5)!= 0)goto _LL583;_tmp7E7=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7E5)->f1;if(_tmp7E7 <= (void*)1?1:*((
int*)_tmp7E7)!= 0)goto _LL583;_tmp7E8=((struct Cyc_Absyn_Upper_b_struct*)_tmp7E7)->f1;
_LL582: if(Cyc_Evexp_eval_const_uint_exp(_tmp7E8)<= i)({struct Cyc_Std_Int_pa_struct
_tmp7EB;_tmp7EB.tag=1;_tmp7EB.f1=(int)i;{struct Cyc_Std_Int_pa_struct _tmp7EA;
_tmp7EA.tag=1;_tmp7EA.f1=(int)Cyc_Evexp_eval_const_uint_exp(_tmp7E8);{void*
_tmp7E9[2]={& _tmp7EA,& _tmp7EB};Cyc_Tcutil_terr(loc,_tag_arr("dereference is out of bounds: %d <= %d",
sizeof(char),39),_tag_arr(_tmp7E9,sizeof(void*),2));}}});return;_LL583:;_LL584:(
void*)(b->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp7EC=_cycalloc(
sizeof(*_tmp7EC));_tmp7EC[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp7ED;_tmp7ED.tag=
0;_tmp7ED.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp7EE=_cycalloc(
sizeof(*_tmp7EE));_tmp7EE[0]=({struct Cyc_Absyn_Upper_b_struct _tmp7EF;_tmp7EF.tag=
0;_tmp7EF.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmp7EF;});_tmp7EE;}));_tmp7ED;});
_tmp7EC;})));return;_LL57E:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp7F0=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp7F1;struct Cyc_Absyn_Exp*_tmp7F2;_LL586: if(_tmp7F0 <= (void*)1?1:*((int*)
_tmp7F0)!= 0)goto _LL588;_tmp7F1=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7F0)->f1;if(_tmp7F1 <= (void*)1?1:*((int*)_tmp7F1)!= 0)goto _LL588;_tmp7F2=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7F1)->f1;_LL587: return Cyc_Evexp_eval_const_uint_exp(
_tmp7F2)== 1;_LL588:;_LL589: return 0;_LL585:;}int Cyc_Tcutil_bits_only(void*t){
void*_tmp7F3=Cyc_Tcutil_compress(t);void*_tmp7F4;struct Cyc_List_List*_tmp7F5;
struct Cyc_Absyn_AggrInfo _tmp7F6;void*_tmp7F7;void*_tmp7F8;struct Cyc_Absyn_AggrInfo
_tmp7F9;void*_tmp7FA;void*_tmp7FB;struct Cyc_Absyn_AggrInfo _tmp7FC;void*_tmp7FD;
struct Cyc_Absyn_Aggrdecl**_tmp7FE;struct Cyc_Absyn_Aggrdecl*_tmp7FF;struct Cyc_List_List*
_tmp800;struct Cyc_List_List*_tmp801;_LL58B: if((int)_tmp7F3 != 0)goto _LL58D;_LL58C:
goto _LL58E;_LL58D: if(_tmp7F3 <= (void*)3?1:*((int*)_tmp7F3)!= 5)goto _LL58F;_LL58E:
goto _LL590;_LL58F: if((int)_tmp7F3 != 1)goto _LL591;_LL590: goto _LL592;_LL591: if(
_tmp7F3 <= (void*)3?1:*((int*)_tmp7F3)!= 6)goto _LL593;_LL592: return 1;_LL593: if(
_tmp7F3 <= (void*)3?1:*((int*)_tmp7F3)!= 12)goto _LL595;_LL594: goto _LL596;_LL595:
if(_tmp7F3 <= (void*)3?1:*((int*)_tmp7F3)!= 13)goto _LL597;_LL596: return 0;_LL597:
if(_tmp7F3 <= (void*)3?1:*((int*)_tmp7F3)!= 7)goto _LL599;_tmp7F4=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp7F3)->f1;_LL598: return Cyc_Tcutil_bits_only(
_tmp7F4);_LL599: if(_tmp7F3 <= (void*)3?1:*((int*)_tmp7F3)!= 9)goto _LL59B;_tmp7F5=((
struct Cyc_Absyn_TupleType_struct*)_tmp7F3)->f1;_LL59A: for(0;_tmp7F5 != 0;_tmp7F5=
_tmp7F5->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple4*)_tmp7F5->hd)).f2))
return 0;}return 1;_LL59B: if(_tmp7F3 <= (void*)3?1:*((int*)_tmp7F3)!= 10)goto _LL59D;
_tmp7F6=((struct Cyc_Absyn_AggrType_struct*)_tmp7F3)->f1;_tmp7F7=(void*)_tmp7F6.aggr_info;
if(*((int*)_tmp7F7)!= 0)goto _LL59D;_tmp7F8=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp7F7)->f1;if((int)_tmp7F8 != 1)goto _LL59D;_LL59C: return 1;_LL59D: if(_tmp7F3 <= (
void*)3?1:*((int*)_tmp7F3)!= 10)goto _LL59F;_tmp7F9=((struct Cyc_Absyn_AggrType_struct*)
_tmp7F3)->f1;_tmp7FA=(void*)_tmp7F9.aggr_info;if(*((int*)_tmp7FA)!= 0)goto _LL59F;
_tmp7FB=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp7FA)->f1;if((int)
_tmp7FB != 0)goto _LL59F;_LL59E: return 0;_LL59F: if(_tmp7F3 <= (void*)3?1:*((int*)
_tmp7F3)!= 10)goto _LL5A1;_tmp7FC=((struct Cyc_Absyn_AggrType_struct*)_tmp7F3)->f1;
_tmp7FD=(void*)_tmp7FC.aggr_info;if(*((int*)_tmp7FD)!= 1)goto _LL5A1;_tmp7FE=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp7FD)->f1;_tmp7FF=*_tmp7FE;_tmp800=_tmp7FC.targs;
_LL5A0: if((void*)_tmp7FF->kind == (void*)1)return 1;if(_tmp7FF->impl == 0)return 0;{
struct _RegionHandle _tmp802=_new_region("rgn");struct _RegionHandle*rgn=& _tmp802;
_push_region(rgn);{struct Cyc_List_List*_tmp803=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp7FF->tvs,_tmp800);{struct Cyc_List_List*fs=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7FF->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp803,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmp804=0;_npop_handler(0);return _tmp804;}}}{int _tmp805=1;
_npop_handler(0);return _tmp805;}};_pop_region(rgn);}_LL5A1: if(_tmp7F3 <= (void*)3?
1:*((int*)_tmp7F3)!= 11)goto _LL5A3;_tmp801=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp7F3)->f2;_LL5A2: for(0;_tmp801 != 0;_tmp801=_tmp801->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp801->hd)->type))return 0;}return 1;_LL5A3:;
_LL5A4: return 0;_LL58A:;}struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmp806=(void*)e->r;struct _tuple1*_tmp807;void*_tmp808;struct Cyc_Absyn_Exp*
_tmp809;struct Cyc_Absyn_Exp*_tmp80A;struct Cyc_Absyn_Exp*_tmp80B;struct Cyc_Absyn_Exp*
_tmp80C;struct Cyc_Absyn_Exp*_tmp80D;struct Cyc_Absyn_Exp*_tmp80E;struct Cyc_Absyn_Exp*
_tmp80F;void*_tmp810;struct Cyc_Absyn_Exp*_tmp811;struct Cyc_Absyn_Exp*_tmp812;
struct Cyc_Absyn_Exp*_tmp813;struct Cyc_Absyn_Exp*_tmp814;struct Cyc_List_List*
_tmp815;struct Cyc_List_List*_tmp816;struct Cyc_List_List*_tmp817;void*_tmp818;
struct Cyc_List_List*_tmp819;struct Cyc_List_List*_tmp81A;struct Cyc_List_List*
_tmp81B;_LL5A6: if(*((int*)_tmp806)!= 0)goto _LL5A8;_LL5A7: goto _LL5A9;_LL5A8: if(*((
int*)_tmp806)!= 16)goto _LL5AA;_LL5A9: goto _LL5AB;_LL5AA: if(*((int*)_tmp806)!= 17)
goto _LL5AC;_LL5AB: goto _LL5AD;_LL5AC: if(*((int*)_tmp806)!= 18)goto _LL5AE;_LL5AD:
goto _LL5AF;_LL5AE: if(*((int*)_tmp806)!= 19)goto _LL5B0;_LL5AF: goto _LL5B1;_LL5B0:
if(*((int*)_tmp806)!= 31)goto _LL5B2;_LL5B1: goto _LL5B3;_LL5B2: if(*((int*)_tmp806)
!= 32)goto _LL5B4;_LL5B3: return 1;_LL5B4: if(*((int*)_tmp806)!= 1)goto _LL5B6;
_tmp807=((struct Cyc_Absyn_Var_e_struct*)_tmp806)->f1;_tmp808=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp806)->f2;_LL5B5: {void*_tmp81C=_tmp808;struct Cyc_Absyn_Vardecl*_tmp81D;
_LL5D3: if(_tmp81C <= (void*)1?1:*((int*)_tmp81C)!= 1)goto _LL5D5;_LL5D4: return 1;
_LL5D5: if(_tmp81C <= (void*)1?1:*((int*)_tmp81C)!= 0)goto _LL5D7;_tmp81D=((struct
Cyc_Absyn_Global_b_struct*)_tmp81C)->f1;_LL5D6: {void*_tmp81E=Cyc_Tcutil_compress((
void*)_tmp81D->type);_LL5DC: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 7)goto
_LL5DE;_LL5DD: goto _LL5DF;_LL5DE: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 8)
goto _LL5E0;_LL5DF: return 1;_LL5E0:;_LL5E1: return var_okay;_LL5DB:;}_LL5D7: if((int)
_tmp81C != 0)goto _LL5D9;_LL5D8: return 0;_LL5D9:;_LL5DA: return var_okay;_LL5D2:;}
_LL5B6: if(*((int*)_tmp806)!= 6)goto _LL5B8;_tmp809=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp806)->f1;_tmp80A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp806)->f2;
_tmp80B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp806)->f3;_LL5B7: return(Cyc_Tcutil_cnst_exp(
te,0,_tmp809)?Cyc_Tcutil_cnst_exp(te,0,_tmp80A): 0)?Cyc_Tcutil_cnst_exp(te,0,
_tmp80B): 0;_LL5B8: if(*((int*)_tmp806)!= 7)goto _LL5BA;_tmp80C=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp806)->f1;_tmp80D=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp806)->f2;_LL5B9:
return Cyc_Tcutil_cnst_exp(te,0,_tmp80C)?Cyc_Tcutil_cnst_exp(te,0,_tmp80D): 0;
_LL5BA: if(*((int*)_tmp806)!= 11)goto _LL5BC;_tmp80E=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp806)->f1;_LL5BB: _tmp80F=_tmp80E;goto _LL5BD;_LL5BC: if(*((int*)_tmp806)!= 12)
goto _LL5BE;_tmp80F=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp806)->f1;_LL5BD:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp80F);_LL5BE: if(*((int*)_tmp806)!= 13)
goto _LL5C0;_tmp810=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp806)->f1;_tmp811=((
struct Cyc_Absyn_Cast_e_struct*)_tmp806)->f2;_LL5BF: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmp811);_LL5C0: if(*((int*)_tmp806)!= 14)goto _LL5C2;_tmp812=((struct Cyc_Absyn_Address_e_struct*)
_tmp806)->f1;_LL5C1: return Cyc_Tcutil_cnst_exp(te,1,_tmp812);_LL5C2: if(*((int*)
_tmp806)!= 27)goto _LL5C4;_tmp813=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp806)->f2;_tmp814=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp806)->f3;
_LL5C3: return Cyc_Tcutil_cnst_exp(te,0,_tmp813)?Cyc_Tcutil_cnst_exp(te,0,_tmp814):
0;_LL5C4: if(*((int*)_tmp806)!= 26)goto _LL5C6;_tmp815=((struct Cyc_Absyn_Array_e_struct*)
_tmp806)->f1;_LL5C5: _tmp816=_tmp815;goto _LL5C7;_LL5C6: if(*((int*)_tmp806)!= 29)
goto _LL5C8;_tmp816=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp806)->f2;_LL5C7:
_tmp817=_tmp816;goto _LL5C9;_LL5C8: if(*((int*)_tmp806)!= 28)goto _LL5CA;_tmp817=((
struct Cyc_Absyn_Struct_e_struct*)_tmp806)->f3;_LL5C9: for(0;_tmp817 != 0;_tmp817=
_tmp817->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple22*)_tmp817->hd)).f2))
return 0;}return 1;_LL5CA: if(*((int*)_tmp806)!= 3)goto _LL5CC;_tmp818=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp806)->f1;_tmp819=((struct Cyc_Absyn_Primop_e_struct*)
_tmp806)->f2;_LL5CB: _tmp81A=_tmp819;goto _LL5CD;_LL5CC: if(*((int*)_tmp806)!= 24)
goto _LL5CE;_tmp81A=((struct Cyc_Absyn_Tuple_e_struct*)_tmp806)->f1;_LL5CD: _tmp81B=
_tmp81A;goto _LL5CF;_LL5CE: if(*((int*)_tmp806)!= 30)goto _LL5D0;_tmp81B=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp806)->f1;_LL5CF: for(0;_tmp81B != 0;_tmp81B=_tmp81B->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp81B->hd))return 0;}return 1;
_LL5D0:;_LL5D1: return 0;_LL5A5:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp81F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp820;
struct Cyc_Absyn_Conref*_tmp821;struct Cyc_Absyn_Conref*_tmp822;void*_tmp823;
struct Cyc_List_List*_tmp824;struct Cyc_Absyn_AggrInfo _tmp825;void*_tmp826;struct
Cyc_List_List*_tmp827;struct Cyc_List_List*_tmp828;_LL5E3: if((int)_tmp81F != 0)
goto _LL5E5;_LL5E4: goto _LL5E6;_LL5E5: if(_tmp81F <= (void*)3?1:*((int*)_tmp81F)!= 5)
goto _LL5E7;_LL5E6: goto _LL5E8;_LL5E7: if((int)_tmp81F != 1)goto _LL5E9;_LL5E8: goto
_LL5EA;_LL5E9: if(_tmp81F <= (void*)3?1:*((int*)_tmp81F)!= 6)goto _LL5EB;_LL5EA:
return 1;_LL5EB: if(_tmp81F <= (void*)3?1:*((int*)_tmp81F)!= 4)goto _LL5ED;_tmp820=((
struct Cyc_Absyn_PointerType_struct*)_tmp81F)->f1;_tmp821=_tmp820.nullable;
_tmp822=_tmp820.bounds;_LL5EC: {void*_tmp829=(void*)(Cyc_Absyn_compress_conref(
_tmp822))->v;void*_tmp82A;void*_tmp82B;_LL5FC: if(_tmp829 <= (void*)1?1:*((int*)
_tmp829)!= 0)goto _LL5FE;_tmp82A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp829)->f1;if((int)_tmp82A != 0)goto _LL5FE;_LL5FD: return 1;_LL5FE: if(_tmp829 <= (
void*)1?1:*((int*)_tmp829)!= 0)goto _LL600;_tmp82B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp829)->f1;if(_tmp82B <= (void*)1?1:*((int*)_tmp82B)!= 0)goto _LL600;_LL5FF: {
void*_tmp82C=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp821))->v;int _tmp82D;_LL603: if(_tmp82C <= (void*)1?1:*((int*)_tmp82C)!= 0)goto
_LL605;_tmp82D=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp82C)->f1;_LL604:
return _tmp82D;_LL605:;_LL606: return 0;_LL602:;}_LL600:;_LL601: return 0;_LL5FB:;}
_LL5ED: if(_tmp81F <= (void*)3?1:*((int*)_tmp81F)!= 7)goto _LL5EF;_tmp823=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp81F)->f1;_LL5EE: return Cyc_Tcutil_supports_default(
_tmp823);_LL5EF: if(_tmp81F <= (void*)3?1:*((int*)_tmp81F)!= 9)goto _LL5F1;_tmp824=((
struct Cyc_Absyn_TupleType_struct*)_tmp81F)->f1;_LL5F0: for(0;_tmp824 != 0;_tmp824=
_tmp824->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple4*)_tmp824->hd)).f2))
return 0;}return 1;_LL5F1: if(_tmp81F <= (void*)3?1:*((int*)_tmp81F)!= 10)goto _LL5F3;
_tmp825=((struct Cyc_Absyn_AggrType_struct*)_tmp81F)->f1;_tmp826=(void*)_tmp825.aggr_info;
_tmp827=_tmp825.targs;_LL5F2: {struct Cyc_Absyn_Aggrdecl*_tmp82E=Cyc_Absyn_get_known_aggrdecl(
_tmp826);if(_tmp82E->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp82E->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp82E->tvs,_tmp827,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82E->impl))->fields);}
_LL5F3: if(_tmp81F <= (void*)3?1:*((int*)_tmp81F)!= 11)goto _LL5F5;_tmp828=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp81F)->f2;_LL5F4: return Cyc_Tcutil_fields_support_default(
0,0,_tmp828);_LL5F5: if(_tmp81F <= (void*)3?1:*((int*)_tmp81F)!= 13)goto _LL5F7;
_LL5F6: goto _LL5F8;_LL5F7: if(_tmp81F <= (void*)3?1:*((int*)_tmp81F)!= 12)goto
_LL5F9;_LL5F8: return 1;_LL5F9:;_LL5FA: return 0;_LL5E2:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct
_RegionHandle _tmp82F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp82F;
_push_region(rgn);{struct Cyc_List_List*_tmp830=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp830,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp831=0;_npop_handler(0);return _tmp831;}}};_pop_region(rgn);}return 1;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp832=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp833;void*_tmp834;struct Cyc_Absyn_FnInfo _tmp835;struct Cyc_List_List*_tmp836;
_LL608: if(_tmp832 <= (void*)3?1:*((int*)_tmp832)!= 4)goto _LL60A;_tmp833=((struct
Cyc_Absyn_PointerType_struct*)_tmp832)->f1;_tmp834=(void*)_tmp833.elt_typ;_LL609:
return Cyc_Tcutil_is_noreturn(_tmp834);_LL60A: if(_tmp832 <= (void*)3?1:*((int*)
_tmp832)!= 8)goto _LL60C;_tmp835=((struct Cyc_Absyn_FnType_struct*)_tmp832)->f1;
_tmp836=_tmp835.attributes;_LL60B: for(0;_tmp836 != 0;_tmp836=_tmp836->tl){void*
_tmp837=(void*)_tmp836->hd;_LL60F: if((int)_tmp837 != 3)goto _LL611;_LL610: return 1;
_LL611:;_LL612: continue;_LL60E:;}goto _LL607;_LL60C:;_LL60D: goto _LL607;_LL607:;}
return 0;}
