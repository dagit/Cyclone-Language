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
extern void* _region_malloc(struct _RegionHandle *, unsigned);
extern void* _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void  _free_region(struct _RegionHandle *);

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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(
int,int);extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stderr;int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);extern unsigned char
Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[18];struct
Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{
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
void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,
struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*x,
int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(int(*compare)(void*,
void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l,void*x);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*,struct _tagged_arr);extern unsigned char Cyc_Position_Nocontext[
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Core_Opt*f3;};struct Cyc_Absyn_AccessEff_struct{
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
struct _tuple1*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
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
void*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
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
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
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
struct Cyc_Absyn_Fndecl*);struct _tuple6{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple6*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
struct _tuple7{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple6*Cyc_Tcutil_r_make_inst_var(
struct _tuple7*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
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
void*t,struct Cyc_Absyn_Exp*e);struct _tuple8{int f1;void*f2;};struct _tuple8 Cyc_Tcutil_addressof_props(
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
struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);unsigned char Cyc_Tcutil_Unify[
10]="\000\000\000\000Unify";void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(
void*)0;void*Cyc_Tcutil_t2_failure=(void*)0;struct _tagged_arr Cyc_Tcutil_failure_reason=(
struct _tagged_arr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_Std_fflush((
struct Cyc_Std___cycFILE*)Cyc_Std_stderr);{struct _tagged_arr s1=Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure);struct _tagged_arr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=8;({struct Cyc_Std_String_pa_struct _tmp1;_tmp1.tag=0;_tmp1.f1=(struct
_tagged_arr)s1;{void*_tmp0[1]={& _tmp1};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\t%s and ",
sizeof(unsigned char),9),_tag_arr(_tmp0,sizeof(void*),1));}});pos +=_get_arr_size(
s1,sizeof(unsigned char))+ 5;if(pos >= 80){({void*_tmp2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n\t",sizeof(unsigned char),3),_tag_arr(_tmp2,sizeof(void*),0));});pos=
8;}({struct Cyc_Std_String_pa_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _tagged_arr)
s2;{void*_tmp3[1]={& _tmp4};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s ",sizeof(
unsigned char),4),_tag_arr(_tmp3,sizeof(void*),1));}});pos +=_get_arr_size(s2,
sizeof(unsigned char))+ 1;if(pos >= 80){({void*_tmp5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n\t",sizeof(unsigned char),3),_tag_arr(_tmp5,sizeof(void*),0));});pos=
8;}({void*_tmp6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("failed to unify. ",
sizeof(unsigned char),18),_tag_arr(_tmp6,sizeof(void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr
!= ((struct _tagged_arr)_tag_arr(0,0,0)).curr){if(pos >= 80){({void*_tmp7[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n\t",sizeof(unsigned char),3),_tag_arr(
_tmp7,sizeof(void*),0));});pos=8;}({struct Cyc_Std_String_pa_struct _tmp9;_tmp9.tag=
0;_tmp9.f1=(struct _tagged_arr)Cyc_Tcutil_failure_reason;{void*_tmp8[1]={& _tmp9};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(
_tmp8,sizeof(void*),1));}});}({void*_tmpA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(_tmpA,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___cycFILE*)Cyc_Std_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap){struct _tagged_arr msg=(struct
_tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt,ap);({struct Cyc_Std_String_pa_struct
_tmpC;_tmpC.tag=0;_tmpC.f1=(struct _tagged_arr)msg;{void*_tmpB[1]={& _tmpC};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Compiler Error (Tcutil::impos): %s\n",sizeof(
unsigned char),36),_tag_arr(_tmpB,sizeof(void*),1));}});Cyc_Std_fflush((struct
Cyc_Std___cycFILE*)Cyc_Std_stderr);(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Core_Impossible_struct _tmpE;
_tmpE.tag=Cyc_Core_Impossible;_tmpE.f1=msg;_tmpE;});_tmpD;}));}static struct
_tagged_arr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){return*tv->name;}void
Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){({
struct Cyc_Std_String_pa_struct _tmp11;_tmp11.tag=0;_tmp11.f1=(struct _tagged_arr)
Cyc_Absynpp_kindbound2string((void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);{
struct Cyc_Std_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(struct _tagged_arr)
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd);{void*_tmpF[2]={& _tmp10,&
_tmp11};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::%s ",sizeof(unsigned char),
8),_tag_arr(_tmpF,sizeof(void*),2));}}});}({void*_tmp12[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(_tmp12,sizeof(
void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);}static
struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=
0;void Cyc_Tcutil_warn(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap){struct _tagged_arr msg=(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,
fmt,ap);Cyc_Tcutil_warning_segs=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*
_tmp13));_tmp13->hd=sg;_tmp13->tl=Cyc_Tcutil_warning_segs;_tmp13;});Cyc_Tcutil_warning_msgs=({
struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->hd=({struct
_tagged_arr*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=msg;_tmp15;});_tmp14->tl=
Cyc_Tcutil_warning_msgs;_tmp14;});}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs
== 0)return;({void*_tmp16[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("***Warnings***\n",
sizeof(unsigned char),16),_tag_arr(_tmp16,sizeof(void*),0));});{struct Cyc_List_List*
_tmp17=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){({struct Cyc_Std_String_pa_struct
_tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _tagged_arr)*((struct _tagged_arr*)((struct
Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd);{struct Cyc_Std_String_pa_struct
_tmp19;_tmp19.tag=0;_tmp19.f1=(struct _tagged_arr)*((struct _tagged_arr*)((struct
Cyc_List_List*)_check_null(_tmp17))->hd);{void*_tmp18[2]={& _tmp19,& _tmp1A};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s: %s\n",sizeof(unsigned char),8),_tag_arr(_tmp18,
sizeof(void*),2));}}});_tmp17=_tmp17->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}({void*_tmp1B[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("**************\n",sizeof(unsigned char),16),_tag_arr(
_tmp1B,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);}}
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;static int Cyc_Tcutil_fast_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){return*((int*)_check_null(tv1->identity))
- *((int*)_check_null(tv2->identity));}void*Cyc_Tcutil_compress(void*t){void*
_tmp1C=t;struct Cyc_Core_Opt*_tmp1D;struct Cyc_Core_Opt*_tmp1E;struct Cyc_Core_Opt*
_tmp1F;struct Cyc_Core_Opt**_tmp20;struct Cyc_Core_Opt*_tmp21;struct Cyc_Core_Opt**
_tmp22;_LL1: if(_tmp1C <= (void*)3?1:*((int*)_tmp1C)!= 0)goto _LL3;_tmp1D=((struct
Cyc_Absyn_Evar_struct*)_tmp1C)->f2;if(_tmp1D != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(
_tmp1C <= (void*)3?1:*((int*)_tmp1C)!= 16)goto _LL5;_tmp1E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1C)->f3;if(_tmp1E != 0)goto _LL5;_LL4: return t;_LL5: if(_tmp1C <= (void*)3?1:*((
int*)_tmp1C)!= 16)goto _LL7;_tmp1F=((struct Cyc_Absyn_TypedefType_struct*)_tmp1C)->f3;
_tmp20=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp1C)->f3;
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
_tmp3C->width=f->width;_tmp3C->attributes=f->attributes;_tmp3C;});}struct _tuple9{
void*f1;void*f2;};static struct _tuple9*Cyc_Tcutil_copy_rgncmp(struct _tuple9*x){
struct _tuple9 _tmp3E;void*_tmp3F;void*_tmp40;struct _tuple9*_tmp3D=x;_tmp3E=*
_tmp3D;_tmp3F=_tmp3E.f1;_tmp40=_tmp3E.f2;return({struct _tuple9*_tmp41=_cycalloc(
sizeof(*_tmp41));_tmp41->f1=Cyc_Tcutil_copy_type(_tmp3F);_tmp41->f2=Cyc_Tcutil_copy_type(
_tmp40);_tmp41;});}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*_tmp42=_cycalloc(
sizeof(*_tmp42));_tmp42->name=f->name;_tmp42->tag=f->tag;_tmp42->loc=f->loc;
_tmp42;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp43=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp44;struct Cyc_Absyn_TunionInfo _tmp45;void*_tmp46;struct
Cyc_List_List*_tmp47;void*_tmp48;struct Cyc_Absyn_TunionFieldInfo _tmp49;void*
_tmp4A;struct Cyc_List_List*_tmp4B;struct Cyc_Absyn_PtrInfo _tmp4C;void*_tmp4D;void*
_tmp4E;struct Cyc_Absyn_Conref*_tmp4F;struct Cyc_Absyn_Tqual _tmp50;struct Cyc_Absyn_Conref*
_tmp51;void*_tmp52;void*_tmp53;int _tmp54;void*_tmp55;struct Cyc_Absyn_Tqual _tmp56;
struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_FnInfo _tmp58;struct Cyc_List_List*
_tmp59;struct Cyc_Core_Opt*_tmp5A;void*_tmp5B;struct Cyc_List_List*_tmp5C;int
_tmp5D;struct Cyc_Absyn_VarargInfo*_tmp5E;struct Cyc_List_List*_tmp5F;struct Cyc_List_List*
_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_Absyn_AggrInfo _tmp62;void*_tmp63;
void*_tmp64;struct _tuple1*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_Absyn_AggrInfo
_tmp67;void*_tmp68;struct Cyc_Absyn_Aggrdecl**_tmp69;struct Cyc_List_List*_tmp6A;
void*_tmp6B;struct Cyc_List_List*_tmp6C;struct _tuple1*_tmp6D;struct Cyc_Absyn_Enumdecl*
_tmp6E;struct Cyc_List_List*_tmp6F;void*_tmp70;void*_tmp71;struct _tuple1*_tmp72;
struct Cyc_List_List*_tmp73;void*_tmp74;struct Cyc_List_List*_tmp75;void*_tmp76;
_LL1A: if((int)_tmp43 != 0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp43 <= (void*)3?1:*((
int*)_tmp43)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(_tmp43 <= (void*)3?1:*((int*)
_tmp43)!= 1)goto _LL20;_tmp44=((struct Cyc_Absyn_VarType_struct*)_tmp43)->f1;_LL1F:
return(void*)({struct Cyc_Absyn_VarType_struct*_tmp77=_cycalloc(sizeof(*_tmp77));
_tmp77[0]=({struct Cyc_Absyn_VarType_struct _tmp78;_tmp78.tag=1;_tmp78.f1=Cyc_Tcutil_copy_tvar(
_tmp44);_tmp78;});_tmp77;});_LL20: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 2)
goto _LL22;_tmp45=((struct Cyc_Absyn_TunionType_struct*)_tmp43)->f1;_tmp46=(void*)
_tmp45.tunion_info;_tmp47=_tmp45.targs;_tmp48=(void*)_tmp45.rgn;_LL21: return(
void*)({struct Cyc_Absyn_TunionType_struct*_tmp79=_cycalloc(sizeof(*_tmp79));
_tmp79[0]=({struct Cyc_Absyn_TunionType_struct _tmp7A;_tmp7A.tag=2;_tmp7A.f1=({
struct Cyc_Absyn_TunionInfo _tmp7B;_tmp7B.tunion_info=(void*)_tmp46;_tmp7B.targs=
Cyc_Tcutil_copy_types(_tmp47);_tmp7B.rgn=(void*)Cyc_Tcutil_copy_type(_tmp48);
_tmp7B;});_tmp7A;});_tmp79;});_LL22: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 3)
goto _LL24;_tmp49=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp43)->f1;_tmp4A=(
void*)_tmp49.field_info;_tmp4B=_tmp49.targs;_LL23: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp7D;_tmp7D.tag=3;_tmp7D.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp7E;_tmp7E.field_info=(
void*)_tmp4A;_tmp7E.targs=Cyc_Tcutil_copy_types(_tmp4B);_tmp7E;});_tmp7D;});
_tmp7C;});_LL24: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 4)goto _LL26;_tmp4C=((
struct Cyc_Absyn_PointerType_struct*)_tmp43)->f1;_tmp4D=(void*)_tmp4C.elt_typ;
_tmp4E=(void*)_tmp4C.rgn_typ;_tmp4F=_tmp4C.nullable;_tmp50=_tmp4C.tq;_tmp51=
_tmp4C.bounds;_LL25: {void*_tmp7F=Cyc_Tcutil_copy_type(_tmp4D);void*_tmp80=Cyc_Tcutil_copy_type(
_tmp4E);struct Cyc_Absyn_Conref*_tmp81=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp4F);struct Cyc_Absyn_Tqual _tmp82=_tmp50;struct Cyc_Absyn_Conref*
_tmp83=Cyc_Tcutil_copy_conref(_tmp51);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=({struct Cyc_Absyn_PointerType_struct
_tmp85;_tmp85.tag=4;_tmp85.f1=({struct Cyc_Absyn_PtrInfo _tmp86;_tmp86.elt_typ=(
void*)_tmp7F;_tmp86.rgn_typ=(void*)_tmp80;_tmp86.nullable=_tmp81;_tmp86.tq=
_tmp82;_tmp86.bounds=_tmp83;_tmp86;});_tmp85;});_tmp84;});}_LL26: if(_tmp43 <= (
void*)3?1:*((int*)_tmp43)!= 5)goto _LL28;_tmp52=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp43)->f1;_tmp53=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp43)->f2;_LL27:
return(void*)({struct Cyc_Absyn_IntType_struct*_tmp87=_cycalloc(sizeof(*_tmp87));
_tmp87[0]=({struct Cyc_Absyn_IntType_struct _tmp88;_tmp88.tag=5;_tmp88.f1=(void*)
_tmp52;_tmp88.f2=(void*)_tmp53;_tmp88;});_tmp87;});_LL28: if((int)_tmp43 != 1)goto
_LL2A;_LL29: return t;_LL2A: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 6)goto _LL2C;
_tmp54=((struct Cyc_Absyn_DoubleType_struct*)_tmp43)->f1;_LL2B: return(void*)({
struct Cyc_Absyn_DoubleType_struct*_tmp89=_cycalloc_atomic(sizeof(*_tmp89));
_tmp89[0]=({struct Cyc_Absyn_DoubleType_struct _tmp8A;_tmp8A.tag=6;_tmp8A.f1=
_tmp54;_tmp8A;});_tmp89;});_LL2C: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 7)goto
_LL2E;_tmp55=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp43)->f1;_tmp56=((
struct Cyc_Absyn_ArrayType_struct*)_tmp43)->f2;_tmp57=((struct Cyc_Absyn_ArrayType_struct*)
_tmp43)->f3;_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp8B=
_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Absyn_ArrayType_struct _tmp8C;
_tmp8C.tag=7;_tmp8C.f1=(void*)Cyc_Tcutil_copy_type(_tmp55);_tmp8C.f2=_tmp56;
_tmp8C.f3=_tmp57;_tmp8C;});_tmp8B;});_LL2E: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)
!= 8)goto _LL30;_tmp58=((struct Cyc_Absyn_FnType_struct*)_tmp43)->f1;_tmp59=_tmp58.tvars;
_tmp5A=_tmp58.effect;_tmp5B=(void*)_tmp58.ret_typ;_tmp5C=_tmp58.args;_tmp5D=
_tmp58.c_varargs;_tmp5E=_tmp58.cyc_varargs;_tmp5F=_tmp58.rgn_po;_tmp60=_tmp58.attributes;
_LL2F: {struct Cyc_List_List*_tmp8D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp59);struct Cyc_Core_Opt*_tmp8E=_tmp5A == 0?0:({struct Cyc_Core_Opt*_tmp98=
_cycalloc(sizeof(*_tmp98));_tmp98->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp5A->v);
_tmp98;});void*_tmp8F=Cyc_Tcutil_copy_type(_tmp5B);struct Cyc_List_List*_tmp90=((
struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp5C);int _tmp91=_tmp5D;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp5E != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp5E);cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp92=_cycalloc(
sizeof(*_tmp92));_tmp92->name=cv->name;_tmp92->tq=cv->tq;_tmp92->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmp92->inject=cv->inject;_tmp92;});}{struct Cyc_List_List*_tmp93=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp5F);struct Cyc_List_List*_tmp94=_tmp60;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmp95=_cycalloc(sizeof(*_tmp95));
_tmp95[0]=({struct Cyc_Absyn_FnType_struct _tmp96;_tmp96.tag=8;_tmp96.f1=({struct
Cyc_Absyn_FnInfo _tmp97;_tmp97.tvars=_tmp8D;_tmp97.effect=_tmp8E;_tmp97.ret_typ=(
void*)_tmp8F;_tmp97.args=_tmp90;_tmp97.c_varargs=_tmp91;_tmp97.cyc_varargs=
cyc_varargs2;_tmp97.rgn_po=_tmp93;_tmp97.attributes=_tmp94;_tmp97;});_tmp96;});
_tmp95;});}}_LL30: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 9)goto _LL32;_tmp61=((
struct Cyc_Absyn_TupleType_struct*)_tmp43)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_TupleType_struct
_tmp9A;_tmp9A.tag=9;_tmp9A.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(
struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp61);
_tmp9A;});_tmp99;});_LL32: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 10)goto _LL34;
_tmp62=((struct Cyc_Absyn_AggrType_struct*)_tmp43)->f1;_tmp63=(void*)_tmp62.aggr_info;
if(*((int*)_tmp63)!= 0)goto _LL34;_tmp64=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp63)->f1;_tmp65=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp63)->f2;_tmp66=
_tmp62.targs;_LL33: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp9B=
_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_Absyn_AggrType_struct _tmp9C;
_tmp9C.tag=10;_tmp9C.f1=({struct Cyc_Absyn_AggrInfo _tmp9D;_tmp9D.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));
_tmp9E[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=(
void*)_tmp64;_tmp9F.f2=_tmp65;_tmp9F;});_tmp9E;}));_tmp9D.targs=Cyc_Tcutil_copy_types(
_tmp66);_tmp9D;});_tmp9C;});_tmp9B;});_LL34: if(_tmp43 <= (void*)3?1:*((int*)
_tmp43)!= 10)goto _LL36;_tmp67=((struct Cyc_Absyn_AggrType_struct*)_tmp43)->f1;
_tmp68=(void*)_tmp67.aggr_info;if(*((int*)_tmp68)!= 1)goto _LL36;_tmp69=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp68)->f1;_tmp6A=_tmp67.targs;_LL35: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({
struct Cyc_Absyn_AggrType_struct _tmpA1;_tmpA1.tag=10;_tmpA1.f1=({struct Cyc_Absyn_AggrInfo
_tmpA2;_tmpA2.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpA3=
_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpA4;
_tmpA4.tag=1;_tmpA4.f1=_tmp69;_tmpA4;});_tmpA3;}));_tmpA2.targs=Cyc_Tcutil_copy_types(
_tmp6A);_tmpA2;});_tmpA1;});_tmpA0;});_LL36: if(_tmp43 <= (void*)3?1:*((int*)
_tmp43)!= 11)goto _LL38;_tmp6B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp43)->f1;_tmp6C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp43)->f2;_LL37:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpA5=_cycalloc(sizeof(*
_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpA6;_tmpA6.tag=11;
_tmpA6.f1=(void*)_tmp6B;_tmpA6.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp6C);_tmpA6;});_tmpA5;});_LL38: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 12)
goto _LL3A;_tmp6D=((struct Cyc_Absyn_EnumType_struct*)_tmp43)->f1;_tmp6E=((struct
Cyc_Absyn_EnumType_struct*)_tmp43)->f2;_LL39: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_EnumType_struct
_tmpA8;_tmpA8.tag=12;_tmpA8.f1=_tmp6D;_tmpA8.f2=_tmp6E;_tmpA8;});_tmpA7;});_LL3A:
if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 13)goto _LL3C;_tmp6F=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp43)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpA9=
_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpAA;
_tmpAA.tag=13;_tmpAA.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp6F);_tmpAA;});_tmpA9;});_LL3C: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 14)
goto _LL3E;_tmp70=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp43)->f1;_LL3D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));
_tmpAB[0]=({struct Cyc_Absyn_SizeofType_struct _tmpAC;_tmpAC.tag=14;_tmpAC.f1=(
void*)Cyc_Tcutil_copy_type(_tmp70);_tmpAC;});_tmpAB;});_LL3E: if(_tmp43 <= (void*)
3?1:*((int*)_tmp43)!= 15)goto _LL40;_tmp71=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp43)->f1;_LL3F: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpAD=
_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpAE;_tmpAE.tag=15;_tmpAE.f1=(void*)Cyc_Tcutil_copy_type(_tmp71);_tmpAE;});
_tmpAD;});_LL40: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 16)goto _LL42;_tmp72=((
struct Cyc_Absyn_TypedefType_struct*)_tmp43)->f1;_tmp73=((struct Cyc_Absyn_TypedefType_struct*)
_tmp43)->f2;_LL41: return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpAF=
_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_TypedefType_struct _tmpB0;
_tmpB0.tag=16;_tmpB0.f1=_tmp72;_tmpB0.f2=Cyc_Tcutil_copy_types(_tmp73);_tmpB0.f3=
0;_tmpB0;});_tmpAF;});_LL42: if((int)_tmp43 != 2)goto _LL44;_LL43: return t;_LL44: if(
_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 17)goto _LL46;_tmp74=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp43)->f1;_LL45: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpB1=
_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_AccessEff_struct _tmpB2;
_tmpB2.tag=17;_tmpB2.f1=(void*)Cyc_Tcutil_copy_type(_tmp74);_tmpB2;});_tmpB1;});
_LL46: if(_tmp43 <= (void*)3?1:*((int*)_tmp43)!= 18)goto _LL48;_tmp75=((struct Cyc_Absyn_JoinEff_struct*)
_tmp43)->f1;_LL47: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpB3=_cycalloc(
sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_JoinEff_struct _tmpB4;_tmpB4.tag=18;
_tmpB4.f1=Cyc_Tcutil_copy_types(_tmp75);_tmpB4;});_tmpB3;});_LL48: if(_tmp43 <= (
void*)3?1:*((int*)_tmp43)!= 19)goto _LL19;_tmp76=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp43)->f1;_LL49: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpB5=_cycalloc(
sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpB6;_tmpB6.tag=19;
_tmpB6.f1=(void*)Cyc_Tcutil_copy_type(_tmp76);_tmpB6;});_tmpB5;});_LL19:;}int Cyc_Tcutil_kind_leq(
void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple9 _tmpB8=({struct _tuple9 _tmpB7;
_tmpB7.f1=k1;_tmpB7.f2=k2;_tmpB7;});void*_tmpB9;void*_tmpBA;void*_tmpBB;void*
_tmpBC;void*_tmpBD;void*_tmpBE;_LL4B: _tmpB9=_tmpB8.f1;if((int)_tmpB9 != 2)goto
_LL4D;_tmpBA=_tmpB8.f2;if((int)_tmpBA != 1)goto _LL4D;_LL4C: goto _LL4E;_LL4D: _tmpBB=
_tmpB8.f1;if((int)_tmpBB != 2)goto _LL4F;_tmpBC=_tmpB8.f2;if((int)_tmpBC != 0)goto
_LL4F;_LL4E: goto _LL50;_LL4F: _tmpBD=_tmpB8.f1;if((int)_tmpBD != 1)goto _LL51;_tmpBE=
_tmpB8.f2;if((int)_tmpBE != 0)goto _LL51;_LL50: return 1;_LL51:;_LL52: return 0;_LL4A:;}}
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpBF=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpC0;void*_tmpC1;_LL54: if(*((int*)_tmpBF)!= 0)goto _LL56;
_tmpC0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpBF)->f1;_LL55: return _tmpC0;
_LL56: if(*((int*)_tmpBF)!= 2)goto _LL58;_tmpC1=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpBF)->f2;_LL57: return _tmpC1;_LL58:;_LL59:({void*_tmpC2[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("kind not suitably constrained!",
sizeof(unsigned char),31),_tag_arr(_tmpC2,sizeof(void*),0));});_LL53:;}void*Cyc_Tcutil_typ_kind(
void*t){void*_tmpC3=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpC4;struct Cyc_Core_Opt*
_tmpC5;struct Cyc_Absyn_Tvar*_tmpC6;void*_tmpC7;struct Cyc_Absyn_TunionFieldInfo
_tmpC8;void*_tmpC9;struct Cyc_Absyn_Tunionfield*_tmpCA;struct Cyc_Absyn_TunionFieldInfo
_tmpCB;void*_tmpCC;struct Cyc_Absyn_Enumdecl*_tmpCD;struct Cyc_Absyn_Enumdecl*
_tmpCE;struct Cyc_Absyn_PtrInfo _tmpCF;struct Cyc_Core_Opt*_tmpD0;_LL5B: if(_tmpC3 <= (
void*)3?1:*((int*)_tmpC3)!= 0)goto _LL5D;_tmpC4=((struct Cyc_Absyn_Evar_struct*)
_tmpC3)->f1;_tmpC5=((struct Cyc_Absyn_Evar_struct*)_tmpC3)->f2;_LL5C: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpC4))->v;_LL5D: if(_tmpC3 <= (void*)3?1:*((int*)
_tmpC3)!= 1)goto _LL5F;_tmpC6=((struct Cyc_Absyn_VarType_struct*)_tmpC3)->f1;_LL5E:
return Cyc_Tcutil_tvar_kind(_tmpC6);_LL5F: if((int)_tmpC3 != 0)goto _LL61;_LL60:
return(void*)1;_LL61: if(_tmpC3 <= (void*)3?1:*((int*)_tmpC3)!= 5)goto _LL63;_tmpC7=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpC3)->f2;_LL62: return _tmpC7 == (void*)
2?(void*)2:(void*)1;_LL63: if((int)_tmpC3 != 1)goto _LL65;_LL64: goto _LL66;_LL65: if(
_tmpC3 <= (void*)3?1:*((int*)_tmpC3)!= 6)goto _LL67;_LL66: goto _LL68;_LL67: if(
_tmpC3 <= (void*)3?1:*((int*)_tmpC3)!= 8)goto _LL69;_LL68: return(void*)1;_LL69: if(
_tmpC3 <= (void*)3?1:*((int*)_tmpC3)!= 15)goto _LL6B;_LL6A: return(void*)2;_LL6B:
if((int)_tmpC3 != 2)goto _LL6D;_LL6C: return(void*)3;_LL6D: if(_tmpC3 <= (void*)3?1:*((
int*)_tmpC3)!= 2)goto _LL6F;_LL6E: return(void*)2;_LL6F: if(_tmpC3 <= (void*)3?1:*((
int*)_tmpC3)!= 3)goto _LL71;_tmpC8=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpC3)->f1;_tmpC9=(void*)_tmpC8.field_info;if(*((int*)_tmpC9)!= 1)goto _LL71;
_tmpCA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpC9)->f2;_LL70: if(_tmpCA->typs
== 0)return(void*)2;else{return(void*)1;}_LL71: if(_tmpC3 <= (void*)3?1:*((int*)
_tmpC3)!= 3)goto _LL73;_tmpCB=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpC3)->f1;
_tmpCC=(void*)_tmpCB.field_info;if(*((int*)_tmpCC)!= 0)goto _LL73;_LL72:({void*
_tmpD1[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typ_kind: Unresolved TunionFieldType",sizeof(unsigned char),37),
_tag_arr(_tmpD1,sizeof(void*),0));});_LL73: if(_tmpC3 <= (void*)3?1:*((int*)_tmpC3)
!= 12)goto _LL75;_tmpCD=((struct Cyc_Absyn_EnumType_struct*)_tmpC3)->f2;if(_tmpCD
!= 0)goto _LL75;_LL74: return(void*)0;_LL75: if(_tmpC3 <= (void*)3?1:*((int*)_tmpC3)
!= 10)goto _LL77;_LL76: goto _LL78;_LL77: if(_tmpC3 <= (void*)3?1:*((int*)_tmpC3)!= 
11)goto _LL79;_LL78: goto _LL7A;_LL79: if(_tmpC3 <= (void*)3?1:*((int*)_tmpC3)!= 13)
goto _LL7B;_LL7A: return(void*)1;_LL7B: if(_tmpC3 <= (void*)3?1:*((int*)_tmpC3)!= 12)
goto _LL7D;_tmpCE=((struct Cyc_Absyn_EnumType_struct*)_tmpC3)->f2;_LL7C: if(_tmpCE->fields
== 0)return(void*)0;else{return(void*)1;}_LL7D: if(_tmpC3 <= (void*)3?1:*((int*)
_tmpC3)!= 4)goto _LL7F;_tmpCF=((struct Cyc_Absyn_PointerType_struct*)_tmpC3)->f1;
_LL7E: {void*_tmpD2=(void*)(Cyc_Absyn_compress_conref(_tmpCF.bounds))->v;void*
_tmpD3;void*_tmpD4;_LL8E: if(_tmpD2 <= (void*)1?1:*((int*)_tmpD2)!= 0)goto _LL90;
_tmpD3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpD2)->f1;if((int)_tmpD3 != 0)
goto _LL90;_LL8F: return(void*)1;_LL90: if(_tmpD2 <= (void*)1?1:*((int*)_tmpD2)!= 0)
goto _LL92;_tmpD4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpD2)->f1;if(
_tmpD4 <= (void*)1?1:*((int*)_tmpD4)!= 0)goto _LL92;_LL91: return(void*)2;_LL92: if((
int)_tmpD2 != 0)goto _LL94;_LL93: return(void*)1;_LL94: if(_tmpD2 <= (void*)1?1:*((
int*)_tmpD2)!= 1)goto _LL8D;_LL95:({void*_tmpD5[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof(unsigned char),43),_tag_arr(_tmpD5,sizeof(void*),0));});_LL8D:;}_LL7F: if(
_tmpC3 <= (void*)3?1:*((int*)_tmpC3)!= 14)goto _LL81;_LL80: return(void*)2;_LL81:
if(_tmpC3 <= (void*)3?1:*((int*)_tmpC3)!= 7)goto _LL83;_LL82: goto _LL84;_LL83: if(
_tmpC3 <= (void*)3?1:*((int*)_tmpC3)!= 9)goto _LL85;_LL84: return(void*)1;_LL85: if(
_tmpC3 <= (void*)3?1:*((int*)_tmpC3)!= 16)goto _LL87;_tmpD0=((struct Cyc_Absyn_TypedefType_struct*)
_tmpC3)->f3;_LL86:({struct Cyc_Std_String_pa_struct _tmpD7;_tmpD7.tag=0;_tmpD7.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpD6[1]={& _tmpD7};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typ_kind: typedef found: %s",
sizeof(unsigned char),28),_tag_arr(_tmpD6,sizeof(void*),1));}});_LL87: if(_tmpC3
<= (void*)3?1:*((int*)_tmpC3)!= 17)goto _LL89;_LL88: goto _LL8A;_LL89: if(_tmpC3 <= (
void*)3?1:*((int*)_tmpC3)!= 18)goto _LL8B;_LL8A: goto _LL8C;_LL8B: if(_tmpC3 <= (void*)
3?1:*((int*)_tmpC3)!= 19)goto _LL5A;_LL8C: return(void*)4;_LL5A:;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmpD8;_push_handler(& _tmpD8);{int _tmpDA=0;
if(setjmp(_tmpD8.handler))_tmpDA=1;if(!_tmpDA){Cyc_Tcutil_unify_it(t1,t2);{int
_tmpDB=1;_npop_handler(0);return _tmpDB;};_pop_handler();}else{void*_tmpD9=(void*)
_exn_thrown;void*_tmpDD=_tmpD9;_LL97: if(_tmpDD != Cyc_Tcutil_Unify)goto _LL99;
_LL98: return 0;_LL99:;_LL9A:(void)_throw(_tmpDD);_LL96:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t){void*_tmpDE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpDF;
struct Cyc_Core_Opt*_tmpE0;struct Cyc_Core_Opt*_tmpE1;struct Cyc_Core_Opt**_tmpE2;
struct Cyc_Absyn_PtrInfo _tmpE3;void*_tmpE4;struct Cyc_Absyn_FnInfo _tmpE5;struct Cyc_List_List*
_tmpE6;struct Cyc_Core_Opt*_tmpE7;void*_tmpE8;struct Cyc_List_List*_tmpE9;int
_tmpEA;struct Cyc_Absyn_VarargInfo*_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_List_List*
_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_Absyn_TunionInfo _tmpEF;struct Cyc_List_List*
_tmpF0;void*_tmpF1;struct Cyc_List_List*_tmpF2;struct Cyc_Core_Opt*_tmpF3;struct
Cyc_Absyn_TunionFieldInfo _tmpF4;struct Cyc_List_List*_tmpF5;struct Cyc_Absyn_AggrInfo
_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF8;void*_tmpF9;void*
_tmpFA;void*_tmpFB;void*_tmpFC;struct Cyc_List_List*_tmpFD;_LL9C: if(_tmpDE <= (
void*)3?1:*((int*)_tmpDE)!= 1)goto _LL9E;_tmpDF=((struct Cyc_Absyn_VarType_struct*)
_tmpDE)->f1;_LL9D: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmpDF)){Cyc_Tcutil_failure_reason=_tag_arr("(type variable would escape scope)",
sizeof(unsigned char),35);(int)_throw((void*)Cyc_Tcutil_Unify);}goto _LL9B;_LL9E:
if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 0)goto _LLA0;_tmpE0=((struct Cyc_Absyn_Evar_struct*)
_tmpDE)->f2;_tmpE1=((struct Cyc_Absyn_Evar_struct*)_tmpDE)->f4;_tmpE2=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmpDE)->f4;_LL9F: if(t == evar){Cyc_Tcutil_failure_reason=
_tag_arr("(occurs check)",sizeof(unsigned char),15);(int)_throw((void*)Cyc_Tcutil_Unify);}
else{if(_tmpE0 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpE0->v);else{int
problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpE2))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}if(problem){struct Cyc_List_List*_tmpFE=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpE2))->v;for(0;s != 
0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmpFE=({struct Cyc_List_List*_tmpFF=_cycalloc(
sizeof(*_tmpFF));_tmpFF->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmpFF->tl=_tmpFE;
_tmpFF;});}}*_tmpE2=({struct Cyc_Core_Opt*_tmp100=_cycalloc(sizeof(*_tmp100));
_tmp100->v=_tmpFE;_tmp100;});}}}goto _LL9B;_LLA0: if(_tmpDE <= (void*)3?1:*((int*)
_tmpDE)!= 4)goto _LLA2;_tmpE3=((struct Cyc_Absyn_PointerType_struct*)_tmpDE)->f1;
_LLA1: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpE3.elt_typ);Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmpE3.rgn_typ);goto _LL9B;_LLA2: if(_tmpDE <= (void*)3?1:*((int*)
_tmpDE)!= 7)goto _LLA4;_tmpE4=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpDE)->f1;
_LLA3: Cyc_Tcutil_occurs(evar,r,env,_tmpE4);goto _LL9B;_LLA4: if(_tmpDE <= (void*)3?
1:*((int*)_tmpDE)!= 8)goto _LLA6;_tmpE5=((struct Cyc_Absyn_FnType_struct*)_tmpDE)->f1;
_tmpE6=_tmpE5.tvars;_tmpE7=_tmpE5.effect;_tmpE8=(void*)_tmpE5.ret_typ;_tmpE9=
_tmpE5.args;_tmpEA=_tmpE5.c_varargs;_tmpEB=_tmpE5.cyc_varargs;_tmpEC=_tmpE5.rgn_po;
_tmpED=_tmpE5.attributes;_LLA5: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmpE6,env);if(
_tmpE7 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpE7->v);Cyc_Tcutil_occurs(evar,
r,env,_tmpE8);for(0;_tmpE9 != 0;_tmpE9=_tmpE9->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple2*)_tmpE9->hd)).f3);}if(_tmpEB != 0)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmpEB->type);for(0;_tmpEC != 0;_tmpEC=_tmpEC->tl){struct _tuple9 _tmp102;
void*_tmp103;void*_tmp104;struct _tuple9*_tmp101=(struct _tuple9*)_tmpEC->hd;
_tmp102=*_tmp101;_tmp103=_tmp102.f1;_tmp104=_tmp102.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp103);Cyc_Tcutil_occurs(evar,r,env,_tmp104);}goto _LL9B;_LLA6: if(_tmpDE <= (
void*)3?1:*((int*)_tmpDE)!= 9)goto _LLA8;_tmpEE=((struct Cyc_Absyn_TupleType_struct*)
_tmpDE)->f1;_LLA7: for(0;_tmpEE != 0;_tmpEE=_tmpEE->tl){Cyc_Tcutil_occurs(evar,r,
env,(*((struct _tuple4*)_tmpEE->hd)).f2);}goto _LL9B;_LLA8: if(_tmpDE <= (void*)3?1:*((
int*)_tmpDE)!= 2)goto _LLAA;_tmpEF=((struct Cyc_Absyn_TunionType_struct*)_tmpDE)->f1;
_tmpF0=_tmpEF.targs;_tmpF1=(void*)_tmpEF.rgn;_LLA9: Cyc_Tcutil_occurs(evar,r,env,
_tmpF1);Cyc_Tcutil_occurslist(evar,r,env,_tmpF0);goto _LL9B;_LLAA: if(_tmpDE <= (
void*)3?1:*((int*)_tmpDE)!= 16)goto _LLAC;_tmpF2=((struct Cyc_Absyn_TypedefType_struct*)
_tmpDE)->f2;_tmpF3=((struct Cyc_Absyn_TypedefType_struct*)_tmpDE)->f3;_LLAB:
_tmpF5=_tmpF2;goto _LLAD;_LLAC: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 3)goto
_LLAE;_tmpF4=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpDE)->f1;_tmpF5=_tmpF4.targs;
_LLAD: _tmpF7=_tmpF5;goto _LLAF;_LLAE: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 10)
goto _LLB0;_tmpF6=((struct Cyc_Absyn_AggrType_struct*)_tmpDE)->f1;_tmpF7=_tmpF6.targs;
_LLAF: Cyc_Tcutil_occurslist(evar,r,env,_tmpF7);goto _LL9B;_LLB0: if(_tmpDE <= (void*)
3?1:*((int*)_tmpDE)!= 11)goto _LLB2;_tmpF8=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpDE)->f2;_LLB1: for(0;_tmpF8 != 0;_tmpF8=_tmpF8->tl){Cyc_Tcutil_occurs(evar,r,
env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmpF8->hd)->type);}goto _LL9B;_LLB2: if(
_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 15)goto _LLB4;_tmpF9=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmpDE)->f1;_LLB3: _tmpFA=_tmpF9;goto _LLB5;_LLB4: if(_tmpDE <= (void*)3?1:*((int*)
_tmpDE)!= 14)goto _LLB6;_tmpFA=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmpDE)->f1;
_LLB5: _tmpFB=_tmpFA;goto _LLB7;_LLB6: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 17)
goto _LLB8;_tmpFB=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmpDE)->f1;_LLB7:
_tmpFC=_tmpFB;goto _LLB9;_LLB8: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 19)goto
_LLBA;_tmpFC=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmpDE)->f1;_LLB9: Cyc_Tcutil_occurs(
evar,r,env,_tmpFC);goto _LL9B;_LLBA: if(_tmpDE <= (void*)3?1:*((int*)_tmpDE)!= 18)
goto _LLBC;_tmpFD=((struct Cyc_Absyn_JoinEff_struct*)_tmpDE)->f1;_LLBB: Cyc_Tcutil_occurslist(
evar,r,env,_tmpFD);goto _LL9B;_LLBC:;_LLBD: goto _LL9B;_LL9B:;}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void
Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 
0?t2 != 0: 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0?1: t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){if((tq1.q_const != tq2.q_const?
1: tq1.q_volatile != tq2.q_volatile)?1: tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_failure_reason=
_tag_arr("(qualifiers don't match)",sizeof(unsigned char),25);(int)_throw((void*)
Cyc_Tcutil_Unify);}}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){return(tq1.q_const == tq2.q_const?tq1.q_volatile == tq2.q_volatile: 0)?tq1.q_restrict
== tq2.q_restrict: 0;}static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _tagged_arr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{void*_tmp105=(void*)x->v;void*
_tmp106;_LLBF: if((int)_tmp105 != 0)goto _LLC1;_LLC0:(void*)(x->v=(void*)((void*)({
struct Cyc_Absyn_Forward_constr_struct*_tmp107=_cycalloc(sizeof(*_tmp107));
_tmp107[0]=({struct Cyc_Absyn_Forward_constr_struct _tmp108;_tmp108.tag=1;_tmp108.f1=
y;_tmp108;});_tmp107;})));return;_LLC1: if(_tmp105 <= (void*)1?1:*((int*)_tmp105)
!= 1)goto _LLC3;_LLC2:({void*_tmp109[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress",
sizeof(unsigned char),37),_tag_arr(_tmp109,sizeof(void*),0));});_LLC3: if(_tmp105
<= (void*)1?1:*((int*)_tmp105)!= 0)goto _LLBE;_tmp106=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp105)->f1;_LLC4: {void*_tmp10A=(void*)y->v;void*_tmp10B;_LLC6: if((int)_tmp10A
!= 0)goto _LLC8;_LLC7:(void*)(y->v=(void*)((void*)x->v));return;_LLC8: if(_tmp10A
<= (void*)1?1:*((int*)_tmp10A)!= 1)goto _LLCA;_LLC9:({void*_tmp10C[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress(2)",
sizeof(unsigned char),40),_tag_arr(_tmp10C,sizeof(void*),0));});_LLCA: if(_tmp10A
<= (void*)1?1:*((int*)_tmp10A)!= 0)goto _LLC5;_tmp10B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10A)->f1;_LLCB: if(cmp(_tmp106,_tmp10B)!= 0){Cyc_Tcutil_failure_reason=reason;(
int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLC5:;}_LLBE:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp10D;_push_handler(& _tmp10D);{int _tmp10F=0;if(setjmp(_tmp10D.handler))
_tmp10F=1;if(!_tmp10F){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)
_tag_arr(0,0,0));{int _tmp110=1;_npop_handler(0);return _tmp110;};_pop_handler();}
else{void*_tmp10E=(void*)_exn_thrown;void*_tmp112=_tmp10E;_LLCD: if(_tmp112 != Cyc_Tcutil_Unify)
goto _LLCF;_LLCE: return 0;_LLCF:;_LLD0:(void)_throw(_tmp112);_LLCC:;}}}static int
Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple9 _tmp114=({struct _tuple9
_tmp113;_tmp113.f1=b1;_tmp113.f2=b2;_tmp113;});void*_tmp115;void*_tmp116;void*
_tmp117;void*_tmp118;void*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;void*_tmp11B;
struct Cyc_Absyn_Exp*_tmp11C;_LLD2: _tmp115=_tmp114.f1;if((int)_tmp115 != 0)goto
_LLD4;_tmp116=_tmp114.f2;if((int)_tmp116 != 0)goto _LLD4;_LLD3: return 0;_LLD4:
_tmp117=_tmp114.f1;if((int)_tmp117 != 0)goto _LLD6;_LLD5: return - 1;_LLD6: _tmp118=
_tmp114.f2;if((int)_tmp118 != 0)goto _LLD8;_LLD7: return 1;_LLD8: _tmp119=_tmp114.f1;
if(_tmp119 <= (void*)1?1:*((int*)_tmp119)!= 0)goto _LLD1;_tmp11A=((struct Cyc_Absyn_Upper_b_struct*)
_tmp119)->f1;_tmp11B=_tmp114.f2;if(_tmp11B <= (void*)1?1:*((int*)_tmp11B)!= 0)
goto _LLD1;_tmp11C=((struct Cyc_Absyn_Upper_b_struct*)_tmp11B)->f1;_LLD9: {int i1=(
int)Cyc_Evexp_eval_const_uint_exp(_tmp11A);int i2=(int)Cyc_Evexp_eval_const_uint_exp(
_tmp11C);if(i1 == i2)return 0;if(i1 < i2)return - 1;return 1;}_LLD1:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){if(a1 == a2)return 1;{struct _tuple9 _tmp11E=({struct _tuple9 _tmp11D;
_tmp11D.f1=a1;_tmp11D.f2=a2;_tmp11D;});void*_tmp11F;void*_tmp120;int _tmp121;int
_tmp122;void*_tmp123;void*_tmp124;int _tmp125;int _tmp126;void*_tmp127;int _tmp128;
void*_tmp129;int _tmp12A;void*_tmp12B;int _tmp12C;void*_tmp12D;int _tmp12E;void*
_tmp12F;struct _tagged_arr _tmp130;void*_tmp131;struct _tagged_arr _tmp132;_LLDB:
_tmp11F=_tmp11E.f1;if(_tmp11F <= (void*)16?1:*((int*)_tmp11F)!= 3)goto _LLDD;
_tmp120=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp11F)->f1;_tmp121=((
struct Cyc_Absyn_Format_att_struct*)_tmp11F)->f2;_tmp122=((struct Cyc_Absyn_Format_att_struct*)
_tmp11F)->f3;_tmp123=_tmp11E.f2;if(_tmp123 <= (void*)16?1:*((int*)_tmp123)!= 3)
goto _LLDD;_tmp124=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp123)->f1;
_tmp125=((struct Cyc_Absyn_Format_att_struct*)_tmp123)->f2;_tmp126=((struct Cyc_Absyn_Format_att_struct*)
_tmp123)->f3;_LLDC: return(_tmp120 == _tmp124?_tmp121 == _tmp125: 0)?_tmp122 == 
_tmp126: 0;_LLDD: _tmp127=_tmp11E.f1;if(_tmp127 <= (void*)16?1:*((int*)_tmp127)!= 0)
goto _LLDF;_tmp128=((struct Cyc_Absyn_Regparm_att_struct*)_tmp127)->f1;_tmp129=
_tmp11E.f2;if(_tmp129 <= (void*)16?1:*((int*)_tmp129)!= 0)goto _LLDF;_tmp12A=((
struct Cyc_Absyn_Regparm_att_struct*)_tmp129)->f1;_LLDE: _tmp12C=_tmp128;_tmp12E=
_tmp12A;goto _LLE0;_LLDF: _tmp12B=_tmp11E.f1;if(_tmp12B <= (void*)16?1:*((int*)
_tmp12B)!= 1)goto _LLE1;_tmp12C=((struct Cyc_Absyn_Aligned_att_struct*)_tmp12B)->f1;
_tmp12D=_tmp11E.f2;if(_tmp12D <= (void*)16?1:*((int*)_tmp12D)!= 1)goto _LLE1;
_tmp12E=((struct Cyc_Absyn_Aligned_att_struct*)_tmp12D)->f1;_LLE0: return _tmp12C == 
_tmp12E;_LLE1: _tmp12F=_tmp11E.f1;if(_tmp12F <= (void*)16?1:*((int*)_tmp12F)!= 2)
goto _LLE3;_tmp130=((struct Cyc_Absyn_Section_att_struct*)_tmp12F)->f1;_tmp131=
_tmp11E.f2;if(_tmp131 <= (void*)16?1:*((int*)_tmp131)!= 2)goto _LLE3;_tmp132=((
struct Cyc_Absyn_Section_att_struct*)_tmp131)->f1;_LLE2: return Cyc_Std_strcmp(
_tmp130,_tmp132)== 0;_LLE3:;_LLE4: return 0;_LLDA:;}}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return
0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_normalize_effect(void*e);
static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(struct
Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((void*)af->type);}static struct
_tuple6*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{void*
_tmp133=Cyc_Tcutil_tvar_kind(tv);_LLE6: if((int)_tmp133 != 3)goto _LLE8;_LLE7: t=(
void*)2;goto _LLE5;_LLE8: if((int)_tmp133 != 4)goto _LLEA;_LLE9: t=Cyc_Absyn_empty_effect;
goto _LLE5;_LLEA:;_LLEB: t=Cyc_Absyn_sint_t;goto _LLE5;_LLE5:;}return({struct
_tuple6*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->f1=tv;_tmp134->f2=t;_tmp134;});}
static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp135=Cyc_Tcutil_compress(t);void*
_tmp136;struct Cyc_Absyn_TunionInfo _tmp137;struct Cyc_List_List*_tmp138;void*
_tmp139;struct Cyc_Absyn_PtrInfo _tmp13A;void*_tmp13B;void*_tmp13C;void*_tmp13D;
struct Cyc_List_List*_tmp13E;struct Cyc_Absyn_TunionFieldInfo _tmp13F;struct Cyc_List_List*
_tmp140;struct Cyc_Absyn_AggrInfo _tmp141;struct Cyc_List_List*_tmp142;struct Cyc_List_List*
_tmp143;void*_tmp144;struct Cyc_Absyn_FnInfo _tmp145;struct Cyc_List_List*_tmp146;
struct Cyc_Core_Opt*_tmp147;void*_tmp148;struct Cyc_List_List*_tmp149;struct Cyc_Absyn_VarargInfo*
_tmp14A;struct Cyc_List_List*_tmp14B;void*_tmp14C;_LLED: if((int)_tmp135 != 0)goto
_LLEF;_LLEE: goto _LLF0;_LLEF: if((int)_tmp135 != 1)goto _LLF1;_LLF0: goto _LLF2;_LLF1:
if(_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 6)goto _LLF3;_LLF2: goto _LLF4;_LLF3: if(
_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 12)goto _LLF5;_LLF4: goto _LLF6;_LLF5: if(
_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 13)goto _LLF7;_LLF6: goto _LLF8;_LLF7: if(
_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 5)goto _LLF9;_LLF8: return Cyc_Absyn_empty_effect;
_LLF9: if(_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 0)goto _LLFB;_LLFA: goto _LLFC;
_LLFB: if(_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 1)goto _LLFD;_LLFC: {void*
_tmp14D=Cyc_Tcutil_typ_kind(t);_LL11C: if((int)_tmp14D != 3)goto _LL11E;_LL11D:
return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp14E=_cycalloc(sizeof(*
_tmp14E));_tmp14E[0]=({struct Cyc_Absyn_AccessEff_struct _tmp14F;_tmp14F.tag=17;
_tmp14F.f1=(void*)t;_tmp14F;});_tmp14E;});_LL11E: if((int)_tmp14D != 4)goto _LL120;
_LL11F: return t;_LL120:;_LL121: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp151;_tmp151.tag=19;_tmp151.f1=(void*)t;_tmp151;});_tmp150;});_LL11B:;}_LLFD:
if(_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 15)goto _LLFF;_tmp136=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp135)->f1;_LLFE: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp153;_tmp153.tag=17;_tmp153.f1=(void*)_tmp136;_tmp153;});_tmp152;});_LLFF: if(
_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 2)goto _LL101;_tmp137=((struct Cyc_Absyn_TunionType_struct*)
_tmp135)->f1;_tmp138=_tmp137.targs;_tmp139=(void*)_tmp137.rgn;_LL100: {struct Cyc_List_List*
ts=({struct Cyc_List_List*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp157=_cycalloc(sizeof(*_tmp157));
_tmp157[0]=({struct Cyc_Absyn_AccessEff_struct _tmp158;_tmp158.tag=17;_tmp158.f1=(
void*)_tmp139;_tmp158;});_tmp157;}));_tmp156->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp138);_tmp156;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp155;_tmp155.tag=18;_tmp155.f1=ts;_tmp155;});_tmp154;}));}_LL101: if(_tmp135 <= (
void*)3?1:*((int*)_tmp135)!= 4)goto _LL103;_tmp13A=((struct Cyc_Absyn_PointerType_struct*)
_tmp135)->f1;_tmp13B=(void*)_tmp13A.elt_typ;_tmp13C=(void*)_tmp13A.rgn_typ;
_LL102: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp15A;_tmp15A.tag=18;_tmp15A.f1=({void*_tmp15B[2];_tmp15B[1]=Cyc_Tcutil_rgns_of(
_tmp13B);_tmp15B[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp15C=_cycalloc(
sizeof(*_tmp15C));_tmp15C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp15D;_tmp15D.tag=
17;_tmp15D.f1=(void*)_tmp13C;_tmp15D;});_tmp15C;});Cyc_List_list(_tag_arr(
_tmp15B,sizeof(void*),2));});_tmp15A;});_tmp159;}));_LL103: if(_tmp135 <= (void*)3?
1:*((int*)_tmp135)!= 7)goto _LL105;_tmp13D=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp135)->f1;_LL104: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp13D));
_LL105: if(_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 9)goto _LL107;_tmp13E=((struct
Cyc_Absyn_TupleType_struct*)_tmp135)->f1;_LL106: {struct Cyc_List_List*_tmp15E=0;
for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){_tmp15E=({struct Cyc_List_List*_tmp15F=
_cycalloc(sizeof(*_tmp15F));_tmp15F->hd=(void*)(*((struct _tuple4*)_tmp13E->hd)).f2;
_tmp15F->tl=_tmp15E;_tmp15F;});}_tmp140=_tmp15E;goto _LL108;}_LL107: if(_tmp135 <= (
void*)3?1:*((int*)_tmp135)!= 3)goto _LL109;_tmp13F=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp135)->f1;_tmp140=_tmp13F.targs;_LL108: _tmp142=_tmp140;goto _LL10A;_LL109: if(
_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 10)goto _LL10B;_tmp141=((struct Cyc_Absyn_AggrType_struct*)
_tmp135)->f1;_tmp142=_tmp141.targs;_LL10A: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp160=_cycalloc(sizeof(*_tmp160));
_tmp160[0]=({struct Cyc_Absyn_JoinEff_struct _tmp161;_tmp161.tag=18;_tmp161.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp142);_tmp161;});_tmp160;}));_LL10B: if(_tmp135 <= (void*)3?
1:*((int*)_tmp135)!= 11)goto _LL10D;_tmp143=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp135)->f2;_LL10C: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp163;_tmp163.tag=18;_tmp163.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp143);_tmp163;});
_tmp162;}));_LL10D: if(_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 14)goto _LL10F;
_tmp144=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp135)->f1;_LL10E: return
Cyc_Tcutil_rgns_of(_tmp144);_LL10F: if(_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 8)
goto _LL111;_tmp145=((struct Cyc_Absyn_FnType_struct*)_tmp135)->f1;_tmp146=_tmp145.tvars;
_tmp147=_tmp145.effect;_tmp148=(void*)_tmp145.ret_typ;_tmp149=_tmp145.args;
_tmp14A=_tmp145.cyc_varargs;_tmp14B=_tmp145.rgn_po;_LL110: {void*_tmp164=Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp146),(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp147))->v);return Cyc_Tcutil_normalize_effect(_tmp164);}_LL111: if((
int)_tmp135 != 2)goto _LL113;_LL112: return Cyc_Absyn_empty_effect;_LL113: if(_tmp135
<= (void*)3?1:*((int*)_tmp135)!= 17)goto _LL115;_LL114: goto _LL116;_LL115: if(
_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 18)goto _LL117;_LL116: return t;_LL117: if(
_tmp135 <= (void*)3?1:*((int*)_tmp135)!= 19)goto _LL119;_tmp14C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp135)->f1;_LL118: return Cyc_Tcutil_rgns_of(_tmp14C);_LL119: if(_tmp135 <= (void*)
3?1:*((int*)_tmp135)!= 16)goto _LLEC;_LL11A:({void*_tmp165[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typedef in compressed type",
sizeof(unsigned char),27),_tag_arr(_tmp165,sizeof(void*),0));});_LLEC:;}static
void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*_tmp166=
e;struct Cyc_List_List*_tmp167;struct Cyc_List_List**_tmp168;void*_tmp169;_LL123:
if(_tmp166 <= (void*)3?1:*((int*)_tmp166)!= 18)goto _LL125;_tmp167=((struct Cyc_Absyn_JoinEff_struct*)
_tmp166)->f1;_tmp168=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)
_tmp166)->f1;_LL124: {int nested_join=1;{struct Cyc_List_List*effs=*_tmp168;for(0;
effs != 0;effs=effs->tl){void*_tmp16A=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect(_tmp16A)));{void*_tmp16B=(void*)effs->hd;_LL12A: if(
_tmp16B <= (void*)3?1:*((int*)_tmp16B)!= 18)goto _LL12C;_LL12B: nested_join=1;goto
_LL129;_LL12C:;_LL12D: goto _LL129;_LL129:;}}}if(!nested_join)return e;{struct Cyc_List_List*
effects=0;{struct Cyc_List_List*effs=*_tmp168;for(0;effs != 0;effs=effs->tl){void*
_tmp16C=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*_tmp16D;void*
_tmp16E;_LL12F: if(_tmp16C <= (void*)3?1:*((int*)_tmp16C)!= 18)goto _LL131;_tmp16D=((
struct Cyc_Absyn_JoinEff_struct*)_tmp16C)->f1;_LL130: effects=Cyc_List_revappend(
_tmp16D,effects);goto _LL12E;_LL131: if(_tmp16C <= (void*)3?1:*((int*)_tmp16C)!= 17)
goto _LL133;_tmp16E=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp16C)->f1;if((
int)_tmp16E != 2)goto _LL133;_LL132: goto _LL12E;_LL133:;_LL134: effects=({struct Cyc_List_List*
_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->hd=(void*)_tmp16C;_tmp16F->tl=
effects;_tmp16F;});goto _LL12E;_LL12E:;}}*_tmp168=Cyc_List_imp_rev(effects);
return e;}}_LL125: if(_tmp166 <= (void*)3?1:*((int*)_tmp166)!= 19)goto _LL127;
_tmp169=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp166)->f1;_LL126: return Cyc_Tcutil_rgns_of(
_tmp169);_LL127:;_LL128: return e;_LL122:;}}struct _tuple10{void*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;};static struct _tuple10*Cyc_Tcutil_get_effect_evar(void*
t){void*_tmp170=Cyc_Tcutil_compress(t);struct Cyc_List_List*_tmp171;struct Cyc_List_List
_tmp172;void*_tmp173;struct Cyc_List_List*_tmp174;struct Cyc_Core_Opt*_tmp175;
struct Cyc_Core_Opt*_tmp176;_LL136: if(_tmp170 <= (void*)3?1:*((int*)_tmp170)!= 18)
goto _LL138;_tmp171=((struct Cyc_Absyn_JoinEff_struct*)_tmp170)->f1;if(_tmp171 == 0)
goto _LL138;_tmp172=*_tmp171;_tmp173=(void*)_tmp172.hd;_tmp174=_tmp172.tl;_LL137: {
void*_tmp177=Cyc_Tcutil_compress(_tmp173);struct Cyc_Core_Opt*_tmp178;_LL13D: if(
_tmp177 <= (void*)3?1:*((int*)_tmp177)!= 0)goto _LL13F;_tmp178=((struct Cyc_Absyn_Evar_struct*)
_tmp177)->f4;_LL13E: return({struct _tuple10*_tmp179=_cycalloc(sizeof(*_tmp179));
_tmp179->f1=_tmp173;_tmp179->f2=_tmp174;_tmp179->f3=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp178))->v;_tmp179;});_LL13F:;_LL140: return 0;
_LL13C:;}_LL138: if(_tmp170 <= (void*)3?1:*((int*)_tmp170)!= 0)goto _LL13A;_tmp175=((
struct Cyc_Absyn_Evar_struct*)_tmp170)->f1;_tmp176=((struct Cyc_Absyn_Evar_struct*)
_tmp170)->f4;_LL139: if(_tmp175 == 0?1:(void*)_tmp175->v != (void*)4)({void*_tmp17A[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp17A,
sizeof(void*),0));});return({struct _tuple10*_tmp17B=_cycalloc(sizeof(*_tmp17B));
_tmp17B->f1=t;_tmp17B->f2=0;_tmp17B->f3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp176))->v;_tmp17B;});_LL13A:;_LL13B: return 0;_LL135:;}static struct
Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_struct*_tmp17C=({struct Cyc_Absyn_FnType_struct*
_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Absyn_FnType_struct
_tmp17E;_tmp17E.tag=8;_tmp17E.f1=({struct Cyc_Absyn_FnInfo _tmp17F;_tmp17F.tvars=0;
_tmp17F.effect=({struct Cyc_Core_Opt*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->v=(
void*)eff;_tmp180;});_tmp17F.ret_typ=(void*)((void*)0);_tmp17F.args=0;_tmp17F.c_varargs=
0;_tmp17F.cyc_varargs=0;_tmp17F.rgn_po=0;_tmp17F.attributes=0;_tmp17F;});_tmp17E;});
_tmp17D;});return Cyc_Absyn_atb_typ((void*)_tmp17C,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_bounds_one);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{void*_tmp181=Cyc_Tcutil_compress(
e);void*_tmp182;struct Cyc_List_List*_tmp183;void*_tmp184;struct Cyc_Core_Opt*
_tmp185;struct Cyc_Core_Opt*_tmp186;struct Cyc_Core_Opt**_tmp187;struct Cyc_Core_Opt*
_tmp188;_LL142: if(_tmp181 <= (void*)3?1:*((int*)_tmp181)!= 17)goto _LL144;_tmp182=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp181)->f1;_LL143: if(constrain)
return Cyc_Tcutil_unify(r,_tmp182);_tmp182=Cyc_Tcutil_compress(_tmp182);if(r == 
_tmp182)return 1;{struct _tuple9 _tmp18A=({struct _tuple9 _tmp189;_tmp189.f1=r;
_tmp189.f2=_tmp182;_tmp189;});void*_tmp18B;struct Cyc_Absyn_Tvar*_tmp18C;void*
_tmp18D;struct Cyc_Absyn_Tvar*_tmp18E;_LL14D: _tmp18B=_tmp18A.f1;if(_tmp18B <= (
void*)3?1:*((int*)_tmp18B)!= 1)goto _LL14F;_tmp18C=((struct Cyc_Absyn_VarType_struct*)
_tmp18B)->f1;_tmp18D=_tmp18A.f2;if(_tmp18D <= (void*)3?1:*((int*)_tmp18D)!= 1)
goto _LL14F;_tmp18E=((struct Cyc_Absyn_VarType_struct*)_tmp18D)->f1;_LL14E: return
Cyc_Absyn_tvar_cmp(_tmp18C,_tmp18E)== 0;_LL14F:;_LL150: return 0;_LL14C:;}_LL144:
if(_tmp181 <= (void*)3?1:*((int*)_tmp181)!= 18)goto _LL146;_tmp183=((struct Cyc_Absyn_JoinEff_struct*)
_tmp181)->f1;_LL145: for(0;_tmp183 != 0;_tmp183=_tmp183->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp183->hd))return 1;}return 0;_LL146: if(_tmp181 <= (void*)3?1:*((
int*)_tmp181)!= 19)goto _LL148;_tmp184=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp181)->f1;_LL147: {void*_tmp18F=Cyc_Tcutil_rgns_of(_tmp184);void*_tmp190;
_LL152: if(_tmp18F <= (void*)3?1:*((int*)_tmp18F)!= 19)goto _LL154;_tmp190=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp18F)->f1;_LL153: if(!constrain)return 0;{void*
_tmp191=Cyc_Tcutil_compress(_tmp190);struct Cyc_Core_Opt*_tmp192;struct Cyc_Core_Opt*
_tmp193;struct Cyc_Core_Opt**_tmp194;struct Cyc_Core_Opt*_tmp195;_LL157: if(_tmp191
<= (void*)3?1:*((int*)_tmp191)!= 0)goto _LL159;_tmp192=((struct Cyc_Absyn_Evar_struct*)
_tmp191)->f1;_tmp193=((struct Cyc_Absyn_Evar_struct*)_tmp191)->f2;_tmp194=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp191)->f2;_tmp195=((struct Cyc_Absyn_Evar_struct*)
_tmp191)->f4;_LL158: {void*_tmp196=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp195);Cyc_Tcutil_occurs(_tmp196,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp195))->v,r);{void*_tmp197=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp199=_cycalloc(sizeof(*_tmp199));
_tmp199[0]=({struct Cyc_Absyn_JoinEff_struct _tmp19A;_tmp19A.tag=18;_tmp19A.f1=({
void*_tmp19B[2];_tmp19B[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp19C=
_cycalloc(sizeof(*_tmp19C));_tmp19C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp19D;
_tmp19D.tag=17;_tmp19D.f1=(void*)r;_tmp19D;});_tmp19C;});_tmp19B[0]=_tmp196;Cyc_List_list(
_tag_arr(_tmp19B,sizeof(void*),2));});_tmp19A;});_tmp199;}));*_tmp194=({struct
Cyc_Core_Opt*_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198->v=(void*)_tmp197;
_tmp198;});return 1;}}_LL159:;_LL15A: return 0;_LL156:;}_LL154:;_LL155: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp18F);_LL151:;}_LL148: if(_tmp181 <= (void*)3?1:*((int*)_tmp181)!= 
0)goto _LL14A;_tmp185=((struct Cyc_Absyn_Evar_struct*)_tmp181)->f1;_tmp186=((
struct Cyc_Absyn_Evar_struct*)_tmp181)->f2;_tmp187=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp181)->f2;_tmp188=((struct Cyc_Absyn_Evar_struct*)
_tmp181)->f4;_LL149: if(_tmp185 == 0?1:(void*)_tmp185->v != (void*)4)({void*_tmp19E[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp19E,
sizeof(void*),0));});if(!constrain)return 0;{void*_tmp19F=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp188);Cyc_Tcutil_occurs(_tmp19F,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp188))->v,r);{struct
Cyc_Absyn_JoinEff_struct*_tmp1A0=({struct Cyc_Absyn_JoinEff_struct*_tmp1A2=
_cycalloc(sizeof(*_tmp1A2));_tmp1A2[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1A3;
_tmp1A3.tag=18;_tmp1A3.f1=({struct Cyc_List_List*_tmp1A4=_cycalloc(sizeof(*
_tmp1A4));_tmp1A4->hd=(void*)_tmp19F;_tmp1A4->tl=({struct Cyc_List_List*_tmp1A5=
_cycalloc(sizeof(*_tmp1A5));_tmp1A5->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1A7;_tmp1A7.tag=17;_tmp1A7.f1=(void*)r;_tmp1A7;});_tmp1A6;}));_tmp1A5->tl=0;
_tmp1A5;});_tmp1A4;});_tmp1A3;});_tmp1A2;});*_tmp187=({struct Cyc_Core_Opt*
_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->v=(void*)((void*)_tmp1A0);_tmp1A1;});
return 1;}}_LL14A:;_LL14B: return 0;_LL141:;}}static int Cyc_Tcutil_type_in_effect(
int constrain,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp1A8=Cyc_Tcutil_compress(
e);struct Cyc_List_List*_tmp1A9;void*_tmp1AA;struct Cyc_Core_Opt*_tmp1AB;struct Cyc_Core_Opt*
_tmp1AC;struct Cyc_Core_Opt**_tmp1AD;struct Cyc_Core_Opt*_tmp1AE;_LL15C: if(_tmp1A8
<= (void*)3?1:*((int*)_tmp1A8)!= 17)goto _LL15E;_LL15D: return 0;_LL15E: if(_tmp1A8
<= (void*)3?1:*((int*)_tmp1A8)!= 18)goto _LL160;_tmp1A9=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1A8)->f1;_LL15F: for(0;_tmp1A9 != 0;_tmp1A9=_tmp1A9->tl){if(Cyc_Tcutil_type_in_effect(
constrain,t,(void*)_tmp1A9->hd))return 1;}return 0;_LL160: if(_tmp1A8 <= (void*)3?1:*((
int*)_tmp1A8)!= 19)goto _LL162;_tmp1AA=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1A8)->f1;_LL161: _tmp1AA=Cyc_Tcutil_compress(_tmp1AA);if(t == _tmp1AA)return 1;
if(constrain)return Cyc_Tcutil_unify(t,_tmp1AA);{void*_tmp1AF=Cyc_Tcutil_rgns_of(
t);void*_tmp1B0;_LL167: if(_tmp1AF <= (void*)3?1:*((int*)_tmp1AF)!= 19)goto _LL169;
_tmp1B0=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1AF)->f1;_LL168: {struct
_tuple9 _tmp1B2=({struct _tuple9 _tmp1B1;_tmp1B1.f1=t;_tmp1B1.f2=Cyc_Tcutil_compress(
_tmp1B0);_tmp1B1;});void*_tmp1B3;struct Cyc_Absyn_Tvar*_tmp1B4;void*_tmp1B5;
struct Cyc_Absyn_Tvar*_tmp1B6;_LL16C: _tmp1B3=_tmp1B2.f1;if(_tmp1B3 <= (void*)3?1:*((
int*)_tmp1B3)!= 1)goto _LL16E;_tmp1B4=((struct Cyc_Absyn_VarType_struct*)_tmp1B3)->f1;
_tmp1B5=_tmp1B2.f2;if(_tmp1B5 <= (void*)3?1:*((int*)_tmp1B5)!= 1)goto _LL16E;
_tmp1B6=((struct Cyc_Absyn_VarType_struct*)_tmp1B5)->f1;_LL16D: return Cyc_Tcutil_unify(
t,_tmp1B0);_LL16E:;_LL16F: return t == _tmp1B0;_LL16B:;}_LL169:;_LL16A: return Cyc_Tcutil_type_in_effect(
constrain,t,_tmp1AF);_LL166:;}_LL162: if(_tmp1A8 <= (void*)3?1:*((int*)_tmp1A8)!= 
0)goto _LL164;_tmp1AB=((struct Cyc_Absyn_Evar_struct*)_tmp1A8)->f1;_tmp1AC=((
struct Cyc_Absyn_Evar_struct*)_tmp1A8)->f2;_tmp1AD=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1A8)->f2;_tmp1AE=((struct Cyc_Absyn_Evar_struct*)
_tmp1A8)->f4;_LL163: if(_tmp1AB == 0?1:(void*)_tmp1AB->v != (void*)4)({void*_tmp1B7[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp1B7,
sizeof(void*),0));});if(!constrain)return 0;{void*_tmp1B8=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1AE);Cyc_Tcutil_occurs(_tmp1B8,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1AE))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp1B9=({struct Cyc_Absyn_JoinEff_struct*_tmp1BB=
_cycalloc(sizeof(*_tmp1BB));_tmp1BB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1BC;
_tmp1BC.tag=18;_tmp1BC.f1=({struct Cyc_List_List*_tmp1BD=_cycalloc(sizeof(*
_tmp1BD));_tmp1BD->hd=(void*)_tmp1B8;_tmp1BD->tl=({struct Cyc_List_List*_tmp1BE=
_cycalloc(sizeof(*_tmp1BE));_tmp1BE->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1C0;_tmp1C0.tag=19;_tmp1C0.f1=(void*)t;_tmp1C0;});_tmp1BF;}));_tmp1BE->tl=0;
_tmp1BE;});_tmp1BD;});_tmp1BC;});_tmp1BB;});*_tmp1AD=({struct Cyc_Core_Opt*
_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA->v=(void*)((void*)_tmp1B9);_tmp1BA;});
return 1;}}_LL164:;_LL165: return 0;_LL15B:;}}static int Cyc_Tcutil_variable_in_effect(
int constrain,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp1C1=e;struct Cyc_Absyn_Tvar*_tmp1C2;struct Cyc_List_List*_tmp1C3;void*_tmp1C4;
struct Cyc_Core_Opt*_tmp1C5;struct Cyc_Core_Opt*_tmp1C6;struct Cyc_Core_Opt**
_tmp1C7;struct Cyc_Core_Opt*_tmp1C8;_LL171: if(_tmp1C1 <= (void*)3?1:*((int*)
_tmp1C1)!= 1)goto _LL173;_tmp1C2=((struct Cyc_Absyn_VarType_struct*)_tmp1C1)->f1;
_LL172: return Cyc_Absyn_tvar_cmp(v,_tmp1C2)== 0;_LL173: if(_tmp1C1 <= (void*)3?1:*((
int*)_tmp1C1)!= 18)goto _LL175;_tmp1C3=((struct Cyc_Absyn_JoinEff_struct*)_tmp1C1)->f1;
_LL174: for(0;_tmp1C3 != 0;_tmp1C3=_tmp1C3->tl){if(Cyc_Tcutil_variable_in_effect(
constrain,v,(void*)_tmp1C3->hd))return 1;}return 0;_LL175: if(_tmp1C1 <= (void*)3?1:*((
int*)_tmp1C1)!= 19)goto _LL177;_tmp1C4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1C1)->f1;_LL176: {void*_tmp1C9=Cyc_Tcutil_rgns_of(_tmp1C4);void*_tmp1CA;
_LL17C: if(_tmp1C9 <= (void*)3?1:*((int*)_tmp1C9)!= 19)goto _LL17E;_tmp1CA=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1C9)->f1;_LL17D: if(!constrain)return 0;{void*
_tmp1CB=Cyc_Tcutil_compress(_tmp1CA);struct Cyc_Core_Opt*_tmp1CC;struct Cyc_Core_Opt*
_tmp1CD;struct Cyc_Core_Opt**_tmp1CE;struct Cyc_Core_Opt*_tmp1CF;_LL181: if(_tmp1CB
<= (void*)3?1:*((int*)_tmp1CB)!= 0)goto _LL183;_tmp1CC=((struct Cyc_Absyn_Evar_struct*)
_tmp1CB)->f1;_tmp1CD=((struct Cyc_Absyn_Evar_struct*)_tmp1CB)->f2;_tmp1CE=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1CB)->f2;_tmp1CF=((struct Cyc_Absyn_Evar_struct*)
_tmp1CB)->f4;_LL182: {void*_tmp1D0=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1CF);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1CF))->v,v))return 0;{
void*_tmp1D1=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1D4;_tmp1D4.tag=18;_tmp1D4.f1=({void*_tmp1D5[2];_tmp1D5[1]=(void*)({struct
Cyc_Absyn_VarType_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6[0]=({struct
Cyc_Absyn_VarType_struct _tmp1D7;_tmp1D7.tag=1;_tmp1D7.f1=v;_tmp1D7;});_tmp1D6;});
_tmp1D5[0]=_tmp1D0;Cyc_List_list(_tag_arr(_tmp1D5,sizeof(void*),2));});_tmp1D4;});
_tmp1D3;}));*_tmp1CE=({struct Cyc_Core_Opt*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));
_tmp1D2->v=(void*)_tmp1D1;_tmp1D2;});return 1;}}_LL183:;_LL184: return 0;_LL180:;}
_LL17E:;_LL17F: return Cyc_Tcutil_variable_in_effect(constrain,v,_tmp1C9);_LL17B:;}
_LL177: if(_tmp1C1 <= (void*)3?1:*((int*)_tmp1C1)!= 0)goto _LL179;_tmp1C5=((struct
Cyc_Absyn_Evar_struct*)_tmp1C1)->f1;_tmp1C6=((struct Cyc_Absyn_Evar_struct*)
_tmp1C1)->f2;_tmp1C7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1C1)->f2;_tmp1C8=((struct Cyc_Absyn_Evar_struct*)_tmp1C1)->f4;_LL178: if(
_tmp1C5 == 0?1:(void*)_tmp1C5->v != (void*)4)({void*_tmp1D8[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",
sizeof(unsigned char),27),_tag_arr(_tmp1D8,sizeof(void*),0));});{void*_tmp1D9=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1C8);if(!((int(*)(int(*
compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C8))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp1DA=({struct Cyc_Absyn_JoinEff_struct*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));
_tmp1DC[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1DD;_tmp1DD.tag=18;_tmp1DD.f1=({
struct Cyc_List_List*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->hd=(void*)
_tmp1D9;_tmp1DE->tl=({struct Cyc_List_List*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));
_tmp1DF->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp1E0=_cycalloc(
sizeof(*_tmp1E0));_tmp1E0[0]=({struct Cyc_Absyn_VarType_struct _tmp1E1;_tmp1E1.tag=
1;_tmp1E1.f1=v;_tmp1E1;});_tmp1E0;}));_tmp1DF->tl=0;_tmp1DF;});_tmp1DE;});
_tmp1DD;});_tmp1DC;});*_tmp1C7=({struct Cyc_Core_Opt*_tmp1DB=_cycalloc(sizeof(*
_tmp1DB));_tmp1DB->v=(void*)((void*)_tmp1DA);_tmp1DB;});return 1;}}_LL179:;_LL17A:
return 0;_LL170:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1E2=e;struct Cyc_List_List*_tmp1E3;void*_tmp1E4;_LL186: if(_tmp1E2 <= (
void*)3?1:*((int*)_tmp1E2)!= 18)goto _LL188;_tmp1E3=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E2)->f1;_LL187: for(0;_tmp1E3 != 0;_tmp1E3=_tmp1E3->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp1E3->hd))return 1;}return 0;_LL188: if(_tmp1E2 <= (void*)3?1:*((int*)
_tmp1E2)!= 19)goto _LL18A;_tmp1E4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1E2)->f1;_LL189: {void*_tmp1E5=Cyc_Tcutil_rgns_of(_tmp1E4);void*_tmp1E6;
_LL18F: if(_tmp1E5 <= (void*)3?1:*((int*)_tmp1E5)!= 19)goto _LL191;_tmp1E6=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1E5)->f1;_LL190: return 0;_LL191:;_LL192:
return Cyc_Tcutil_evar_in_effect(evar,_tmp1E5);_LL18E:;}_LL18A: if(_tmp1E2 <= (void*)
3?1:*((int*)_tmp1E2)!= 0)goto _LL18C;_LL18B: return evar == e;_LL18C:;_LL18D: return 0;
_LL185:;}}int Cyc_Tcutil_subset_effect(int set_to_empty,void*e1,void*e2){void*
_tmp1E7=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp1E8;void*_tmp1E9;struct
Cyc_Absyn_Tvar*_tmp1EA;void*_tmp1EB;struct Cyc_Core_Opt*_tmp1EC;struct Cyc_Core_Opt**
_tmp1ED;struct Cyc_Core_Opt*_tmp1EE;_LL194: if(_tmp1E7 <= (void*)3?1:*((int*)
_tmp1E7)!= 18)goto _LL196;_tmp1E8=((struct Cyc_Absyn_JoinEff_struct*)_tmp1E7)->f1;
_LL195: for(0;_tmp1E8 != 0;_tmp1E8=_tmp1E8->tl){if(!Cyc_Tcutil_subset_effect(
set_to_empty,(void*)_tmp1E8->hd,e2))return 0;}return 1;_LL196: if(_tmp1E7 <= (void*)
3?1:*((int*)_tmp1E7)!= 17)goto _LL198;_tmp1E9=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1E7)->f1;_LL197: return(Cyc_Tcutil_region_in_effect(0,_tmp1E9,e2)?1: Cyc_Tcutil_region_in_effect(
1,_tmp1E9,e2))?1: Cyc_Tcutil_unify(_tmp1E9,(void*)2);_LL198: if(_tmp1E7 <= (void*)3?
1:*((int*)_tmp1E7)!= 1)goto _LL19A;_tmp1EA=((struct Cyc_Absyn_VarType_struct*)
_tmp1E7)->f1;_LL199: return Cyc_Tcutil_variable_in_effect(0,_tmp1EA,e2)?1: Cyc_Tcutil_variable_in_effect(
1,_tmp1EA,e2);_LL19A: if(_tmp1E7 <= (void*)3?1:*((int*)_tmp1E7)!= 19)goto _LL19C;
_tmp1EB=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E7)->f1;_LL19B: {void*
_tmp1EF=Cyc_Tcutil_rgns_of(_tmp1EB);void*_tmp1F0;_LL1A1: if(_tmp1EF <= (void*)3?1:*((
int*)_tmp1EF)!= 19)goto _LL1A3;_tmp1F0=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1EF)->f1;_LL1A2: return(Cyc_Tcutil_type_in_effect(0,_tmp1F0,e2)?1: Cyc_Tcutil_type_in_effect(
1,_tmp1F0,e2))?1: Cyc_Tcutil_unify(_tmp1F0,Cyc_Absyn_sint_t);_LL1A3:;_LL1A4:
return Cyc_Tcutil_subset_effect(set_to_empty,_tmp1EF,e2);_LL1A0:;}_LL19C: if(
_tmp1E7 <= (void*)3?1:*((int*)_tmp1E7)!= 0)goto _LL19E;_tmp1EC=((struct Cyc_Absyn_Evar_struct*)
_tmp1E7)->f2;_tmp1ED=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1E7)->f2;_tmp1EE=((struct Cyc_Absyn_Evar_struct*)_tmp1E7)->f4;_LL19D: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){if(set_to_empty)*_tmp1ED=({struct Cyc_Core_Opt*_tmp1F1=_cycalloc(sizeof(*
_tmp1F1));_tmp1F1->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1F2=
_cycalloc(sizeof(*_tmp1F2));_tmp1F2[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1F3;
_tmp1F3.tag=18;_tmp1F3.f1=0;_tmp1F3;});_tmp1F2;}));_tmp1F1;});else{Cyc_Tcutil_occurs(
e1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1EE))->v,e2);*_tmp1ED=({struct Cyc_Core_Opt*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));
_tmp1F4->v=(void*)e2;_tmp1F4;});}}return 1;_LL19E:;_LL19F:({struct Cyc_Std_String_pa_struct
_tmp1F6;_tmp1F6.tag=0;_tmp1F6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e1);{
void*_tmp1F5[1]={& _tmp1F6};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("subset_effect: bad effect: %s",sizeof(unsigned char),
30),_tag_arr(_tmp1F5,sizeof(void*),1));}});_LL193:;}struct _tuple11{struct
_tuple10*f1;struct _tuple10*f2;};static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(e2);{struct
_tuple11 _tmp1F8=({struct _tuple11 _tmp1F7;_tmp1F7.f1=Cyc_Tcutil_get_effect_evar(e1);
_tmp1F7.f2=Cyc_Tcutil_get_effect_evar(e2);_tmp1F7;});_LL1A6:;_LL1A7: return Cyc_Tcutil_subset_effect(
0,e1,e2)?Cyc_Tcutil_subset_effect(0,e2,e1): 0;_LL1A5:;}}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple9 _tmp1FA;void*_tmp1FB;void*_tmp1FC;struct
_tuple9*_tmp1F9=(struct _tuple9*)r1->hd;_tmp1FA=*_tmp1F9;_tmp1FB=_tmp1FA.f1;
_tmp1FC=_tmp1FA.f2;{int found=_tmp1FB == (void*)2;{struct Cyc_List_List*r2=rpo2;
for(0;r2 != 0?!found: 0;r2=r2->tl){struct _tuple9 _tmp1FE;void*_tmp1FF;void*_tmp200;
struct _tuple9*_tmp1FD=(struct _tuple9*)r2->hd;_tmp1FE=*_tmp1FD;_tmp1FF=_tmp1FE.f1;
_tmp200=_tmp1FE.f2;if(Cyc_Tcutil_unify(_tmp1FB,_tmp1FF)?Cyc_Tcutil_unify(_tmp1FC,
_tmp200): 0){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2)?Cyc_Tcutil_sub_rgnpo(rpo2,rpo1): 0;}struct _tuple12{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple13{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _tagged_arr)
_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)
return;{void*_tmp201=t1;struct Cyc_Core_Opt*_tmp202;struct Cyc_Core_Opt*_tmp203;
struct Cyc_Core_Opt**_tmp204;struct Cyc_Core_Opt*_tmp205;_LL1A9: if(_tmp201 <= (void*)
3?1:*((int*)_tmp201)!= 0)goto _LL1AB;_tmp202=((struct Cyc_Absyn_Evar_struct*)
_tmp201)->f1;_tmp203=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;_tmp204=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;_tmp205=((struct Cyc_Absyn_Evar_struct*)
_tmp201)->f4;_LL1AA: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp205))->v,t2);{void*_tmp206=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp206,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp202))->v)){*_tmp204=({struct Cyc_Core_Opt*_tmp207=_cycalloc(sizeof(*_tmp207));
_tmp207->v=(void*)t2;_tmp207;});return;}else{{void*_tmp208=t2;struct Cyc_Core_Opt*
_tmp209;struct Cyc_Core_Opt**_tmp20A;struct Cyc_Core_Opt*_tmp20B;struct Cyc_Absyn_PtrInfo
_tmp20C;_LL1AE: if(_tmp208 <= (void*)3?1:*((int*)_tmp208)!= 0)goto _LL1B0;_tmp209=((
struct Cyc_Absyn_Evar_struct*)_tmp208)->f2;_tmp20A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp208)->f2;_tmp20B=((struct Cyc_Absyn_Evar_struct*)
_tmp208)->f4;_LL1AF: {struct Cyc_List_List*_tmp20D=(struct Cyc_List_List*)_tmp205->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp20B))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp20D,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=
_tag_arr("(type variable would escape scope)",sizeof(unsigned char),35);(int)
_throw((void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)_tmp202->v,
_tmp206)){*_tmp20A=({struct Cyc_Core_Opt*_tmp20E=_cycalloc(sizeof(*_tmp20E));
_tmp20E->v=(void*)t1;_tmp20E;});return;}Cyc_Tcutil_failure_reason=_tag_arr("(kinds are incompatible)",
sizeof(unsigned char),25);goto _LL1AD;}_LL1B0: if(_tmp208 <= (void*)3?1:*((int*)
_tmp208)!= 4)goto _LL1B2;_tmp20C=((struct Cyc_Absyn_PointerType_struct*)_tmp208)->f1;
if(!((void*)_tmp202->v == (void*)2))goto _LL1B2;_LL1B1: {struct Cyc_Absyn_Conref*
_tmp20F=Cyc_Absyn_compress_conref(_tmp20C.bounds);{void*_tmp210=(void*)_tmp20F->v;
_LL1B5: if((int)_tmp210 != 0)goto _LL1B7;_LL1B6:(void*)(_tmp20F->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp212;_tmp212.tag=0;_tmp212.f1=(void*)((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp214;_tmp214.tag=0;_tmp214.f1=Cyc_Absyn_signed_int_exp(
1,0);_tmp214;});_tmp213;}));_tmp212;});_tmp211;})));*_tmp204=({struct Cyc_Core_Opt*
_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215->v=(void*)t2;_tmp215;});return;
_LL1B7:;_LL1B8: goto _LL1B4;_LL1B4:;}goto _LL1AD;}_LL1B2:;_LL1B3: goto _LL1AD;_LL1AD:;}
Cyc_Tcutil_failure_reason=_tag_arr("(kinds are incompatible)",sizeof(
unsigned char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}_LL1AB:;_LL1AC: goto
_LL1A8;_LL1A8:;}{struct _tuple9 _tmp217=({struct _tuple9 _tmp216;_tmp216.f1=t2;
_tmp216.f2=t1;_tmp216;});void*_tmp218;void*_tmp219;void*_tmp21A;void*_tmp21B;
struct Cyc_Absyn_Tvar*_tmp21C;void*_tmp21D;struct Cyc_Absyn_Tvar*_tmp21E;void*
_tmp21F;struct Cyc_Absyn_AggrInfo _tmp220;void*_tmp221;struct Cyc_List_List*_tmp222;
void*_tmp223;struct Cyc_Absyn_AggrInfo _tmp224;void*_tmp225;struct Cyc_List_List*
_tmp226;void*_tmp227;struct _tuple1*_tmp228;void*_tmp229;struct _tuple1*_tmp22A;
void*_tmp22B;struct Cyc_List_List*_tmp22C;void*_tmp22D;struct Cyc_List_List*
_tmp22E;void*_tmp22F;struct Cyc_Absyn_TunionInfo _tmp230;void*_tmp231;struct Cyc_Absyn_Tuniondecl**
_tmp232;struct Cyc_Absyn_Tuniondecl*_tmp233;struct Cyc_List_List*_tmp234;void*
_tmp235;void*_tmp236;struct Cyc_Absyn_TunionInfo _tmp237;void*_tmp238;struct Cyc_Absyn_Tuniondecl**
_tmp239;struct Cyc_Absyn_Tuniondecl*_tmp23A;struct Cyc_List_List*_tmp23B;void*
_tmp23C;void*_tmp23D;struct Cyc_Absyn_TunionFieldInfo _tmp23E;void*_tmp23F;struct
Cyc_Absyn_Tuniondecl*_tmp240;struct Cyc_Absyn_Tunionfield*_tmp241;struct Cyc_List_List*
_tmp242;void*_tmp243;struct Cyc_Absyn_TunionFieldInfo _tmp244;void*_tmp245;struct
Cyc_Absyn_Tuniondecl*_tmp246;struct Cyc_Absyn_Tunionfield*_tmp247;struct Cyc_List_List*
_tmp248;void*_tmp249;struct Cyc_Absyn_PtrInfo _tmp24A;void*_tmp24B;void*_tmp24C;
struct Cyc_Absyn_Conref*_tmp24D;struct Cyc_Absyn_Tqual _tmp24E;struct Cyc_Absyn_Conref*
_tmp24F;void*_tmp250;struct Cyc_Absyn_PtrInfo _tmp251;void*_tmp252;void*_tmp253;
struct Cyc_Absyn_Conref*_tmp254;struct Cyc_Absyn_Tqual _tmp255;struct Cyc_Absyn_Conref*
_tmp256;void*_tmp257;void*_tmp258;void*_tmp259;void*_tmp25A;void*_tmp25B;void*
_tmp25C;void*_tmp25D;void*_tmp25E;void*_tmp25F;int _tmp260;void*_tmp261;int
_tmp262;void*_tmp263;void*_tmp264;void*_tmp265;void*_tmp266;void*_tmp267;void*
_tmp268;struct Cyc_Absyn_Tqual _tmp269;struct Cyc_Absyn_Exp*_tmp26A;void*_tmp26B;
void*_tmp26C;struct Cyc_Absyn_Tqual _tmp26D;struct Cyc_Absyn_Exp*_tmp26E;void*
_tmp26F;struct Cyc_Absyn_FnInfo _tmp270;struct Cyc_List_List*_tmp271;struct Cyc_Core_Opt*
_tmp272;void*_tmp273;struct Cyc_List_List*_tmp274;int _tmp275;struct Cyc_Absyn_VarargInfo*
_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp278;void*_tmp279;
struct Cyc_Absyn_FnInfo _tmp27A;struct Cyc_List_List*_tmp27B;struct Cyc_Core_Opt*
_tmp27C;void*_tmp27D;struct Cyc_List_List*_tmp27E;int _tmp27F;struct Cyc_Absyn_VarargInfo*
_tmp280;struct Cyc_List_List*_tmp281;struct Cyc_List_List*_tmp282;void*_tmp283;
struct Cyc_List_List*_tmp284;void*_tmp285;struct Cyc_List_List*_tmp286;void*
_tmp287;void*_tmp288;struct Cyc_List_List*_tmp289;void*_tmp28A;void*_tmp28B;
struct Cyc_List_List*_tmp28C;void*_tmp28D;void*_tmp28E;void*_tmp28F;void*_tmp290;
void*_tmp291;void*_tmp292;void*_tmp293;void*_tmp294;void*_tmp295;void*_tmp296;
void*_tmp297;void*_tmp298;_LL1BA: _tmp218=_tmp217.f1;if(_tmp218 <= (void*)3?1:*((
int*)_tmp218)!= 0)goto _LL1BC;_LL1BB: Cyc_Tcutil_unify_it(t2,t1);return;_LL1BC:
_tmp219=_tmp217.f1;if((int)_tmp219 != 0)goto _LL1BE;_tmp21A=_tmp217.f2;if((int)
_tmp21A != 0)goto _LL1BE;_LL1BD: return;_LL1BE: _tmp21B=_tmp217.f1;if(_tmp21B <= (
void*)3?1:*((int*)_tmp21B)!= 1)goto _LL1C0;_tmp21C=((struct Cyc_Absyn_VarType_struct*)
_tmp21B)->f1;_tmp21D=_tmp217.f2;if(_tmp21D <= (void*)3?1:*((int*)_tmp21D)!= 1)
goto _LL1C0;_tmp21E=((struct Cyc_Absyn_VarType_struct*)_tmp21D)->f1;_LL1BF: {
struct _tagged_arr*_tmp299=_tmp21C->name;struct _tagged_arr*_tmp29A=_tmp21E->name;
int _tmp29B=*((int*)_check_null(_tmp21C->identity));int _tmp29C=*((int*)
_check_null(_tmp21E->identity));void*_tmp29D=Cyc_Tcutil_tvar_kind(_tmp21C);void*
_tmp29E=Cyc_Tcutil_tvar_kind(_tmp21E);if(_tmp29C == _tmp29B?Cyc_Std_zstrptrcmp(
_tmp299,_tmp29A)== 0: 0){if(_tmp29D != _tmp29E)({struct Cyc_Std_String_pa_struct
_tmp2A2;_tmp2A2.tag=0;_tmp2A2.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp29E);{struct Cyc_Std_String_pa_struct _tmp2A1;_tmp2A1.tag=0;_tmp2A1.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp29D);{struct Cyc_Std_String_pa_struct
_tmp2A0;_tmp2A0.tag=0;_tmp2A0.f1=(struct _tagged_arr)*_tmp299;{void*_tmp29F[3]={&
_tmp2A0,& _tmp2A1,& _tmp2A2};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("same type variable %s has kinds %s and %s",sizeof(
unsigned char),42),_tag_arr(_tmp29F,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=
_tag_arr("(variable types are not the same)",sizeof(unsigned char),34);goto
_LL1B9;}_LL1C0: _tmp21F=_tmp217.f1;if(_tmp21F <= (void*)3?1:*((int*)_tmp21F)!= 10)
goto _LL1C2;_tmp220=((struct Cyc_Absyn_AggrType_struct*)_tmp21F)->f1;_tmp221=(void*)
_tmp220.aggr_info;_tmp222=_tmp220.targs;_tmp223=_tmp217.f2;if(_tmp223 <= (void*)3?
1:*((int*)_tmp223)!= 10)goto _LL1C2;_tmp224=((struct Cyc_Absyn_AggrType_struct*)
_tmp223)->f1;_tmp225=(void*)_tmp224.aggr_info;_tmp226=_tmp224.targs;_LL1C1: {
void*_tmp2A4;struct _tuple1*_tmp2A5;struct _tuple5 _tmp2A3=Cyc_Absyn_aggr_kinded_name(
_tmp225);_tmp2A4=_tmp2A3.f1;_tmp2A5=_tmp2A3.f2;{void*_tmp2A7;struct _tuple1*
_tmp2A8;struct _tuple5 _tmp2A6=Cyc_Absyn_aggr_kinded_name(_tmp221);_tmp2A7=_tmp2A6.f1;
_tmp2A8=_tmp2A6.f2;if(_tmp2A4 != _tmp2A7){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(unsigned char),24);goto _LL1B9;}if(Cyc_Absyn_qvar_cmp(_tmp2A5,_tmp2A8)!= 0){
Cyc_Tcutil_failure_reason=_tag_arr("(different type name)",sizeof(unsigned char),
22);goto _LL1B9;}Cyc_Tcutil_unify_list(_tmp226,_tmp222);return;}}_LL1C2: _tmp227=
_tmp217.f1;if(_tmp227 <= (void*)3?1:*((int*)_tmp227)!= 12)goto _LL1C4;_tmp228=((
struct Cyc_Absyn_EnumType_struct*)_tmp227)->f1;_tmp229=_tmp217.f2;if(_tmp229 <= (
void*)3?1:*((int*)_tmp229)!= 12)goto _LL1C4;_tmp22A=((struct Cyc_Absyn_EnumType_struct*)
_tmp229)->f1;_LL1C3: if(Cyc_Absyn_qvar_cmp(_tmp228,_tmp22A)== 0)return;Cyc_Tcutil_failure_reason=
_tag_arr("(different enum types)",sizeof(unsigned char),23);goto _LL1B9;_LL1C4:
_tmp22B=_tmp217.f1;if(_tmp22B <= (void*)3?1:*((int*)_tmp22B)!= 13)goto _LL1C6;
_tmp22C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp22B)->f1;_tmp22D=_tmp217.f2;
if(_tmp22D <= (void*)3?1:*((int*)_tmp22D)!= 13)goto _LL1C6;_tmp22E=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp22D)->f1;_LL1C5: {int bad=0;for(0;_tmp22C != 0?_tmp22E != 0: 0;(_tmp22C=_tmp22C->tl,
_tmp22E=_tmp22E->tl)){struct Cyc_Absyn_Enumfield*_tmp2A9=(struct Cyc_Absyn_Enumfield*)
_tmp22C->hd;struct Cyc_Absyn_Enumfield*_tmp2AA=(struct Cyc_Absyn_Enumfield*)
_tmp22E->hd;if(Cyc_Absyn_qvar_cmp(_tmp2A9->name,_tmp2AA->name)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different names for enum fields)",sizeof(unsigned char),34);bad=1;
break;}if(_tmp2A9->tag == _tmp2AA->tag)continue;if(_tmp2A9->tag == 0?1: _tmp2AA->tag
== 0){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(unsigned char),39);bad=1;break;}if(Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp2A9->tag))!= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2AA->tag))){Cyc_Tcutil_failure_reason=
_tag_arr("(different tag values for enum fields)",sizeof(unsigned char),39);bad=
1;break;}}if(bad)goto _LL1B9;if(_tmp22C == 0?_tmp22E == 0: 0)return;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of fields for enums)",sizeof(unsigned char),39);goto
_LL1B9;}_LL1C6: _tmp22F=_tmp217.f1;if(_tmp22F <= (void*)3?1:*((int*)_tmp22F)!= 2)
goto _LL1C8;_tmp230=((struct Cyc_Absyn_TunionType_struct*)_tmp22F)->f1;_tmp231=(
void*)_tmp230.tunion_info;if(*((int*)_tmp231)!= 1)goto _LL1C8;_tmp232=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp231)->f1;_tmp233=*_tmp232;_tmp234=_tmp230.targs;_tmp235=(void*)_tmp230.rgn;
_tmp236=_tmp217.f2;if(_tmp236 <= (void*)3?1:*((int*)_tmp236)!= 2)goto _LL1C8;
_tmp237=((struct Cyc_Absyn_TunionType_struct*)_tmp236)->f1;_tmp238=(void*)_tmp237.tunion_info;
if(*((int*)_tmp238)!= 1)goto _LL1C8;_tmp239=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp238)->f1;_tmp23A=*_tmp239;_tmp23B=_tmp237.targs;_tmp23C=(void*)_tmp237.rgn;
_LL1C7: if(_tmp233 == _tmp23A?1: Cyc_Absyn_qvar_cmp(_tmp233->name,_tmp23A->name)== 
0){Cyc_Tcutil_unify_it(_tmp23C,_tmp235);Cyc_Tcutil_unify_list(_tmp23B,_tmp234);
return;}Cyc_Tcutil_failure_reason=_tag_arr("(different tunion types)",sizeof(
unsigned char),25);goto _LL1B9;_LL1C8: _tmp23D=_tmp217.f1;if(_tmp23D <= (void*)3?1:*((
int*)_tmp23D)!= 3)goto _LL1CA;_tmp23E=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp23D)->f1;_tmp23F=(void*)_tmp23E.field_info;if(*((int*)_tmp23F)!= 1)goto
_LL1CA;_tmp240=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp23F)->f1;_tmp241=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp23F)->f2;_tmp242=_tmp23E.targs;
_tmp243=_tmp217.f2;if(_tmp243 <= (void*)3?1:*((int*)_tmp243)!= 3)goto _LL1CA;
_tmp244=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp243)->f1;_tmp245=(void*)
_tmp244.field_info;if(*((int*)_tmp245)!= 1)goto _LL1CA;_tmp246=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp245)->f1;_tmp247=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp245)->f2;
_tmp248=_tmp244.targs;_LL1C9: if((_tmp240 == _tmp246?1: Cyc_Absyn_qvar_cmp(_tmp240->name,
_tmp246->name)== 0)?_tmp241 == _tmp247?1: Cyc_Absyn_qvar_cmp(_tmp241->name,_tmp247->name)
== 0: 0){Cyc_Tcutil_unify_list(_tmp248,_tmp242);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)",sizeof(unsigned char),31);goto _LL1B9;
_LL1CA: _tmp249=_tmp217.f1;if(_tmp249 <= (void*)3?1:*((int*)_tmp249)!= 4)goto
_LL1CC;_tmp24A=((struct Cyc_Absyn_PointerType_struct*)_tmp249)->f1;_tmp24B=(void*)
_tmp24A.elt_typ;_tmp24C=(void*)_tmp24A.rgn_typ;_tmp24D=_tmp24A.nullable;_tmp24E=
_tmp24A.tq;_tmp24F=_tmp24A.bounds;_tmp250=_tmp217.f2;if(_tmp250 <= (void*)3?1:*((
int*)_tmp250)!= 4)goto _LL1CC;_tmp251=((struct Cyc_Absyn_PointerType_struct*)
_tmp250)->f1;_tmp252=(void*)_tmp251.elt_typ;_tmp253=(void*)_tmp251.rgn_typ;
_tmp254=_tmp251.nullable;_tmp255=_tmp251.tq;_tmp256=_tmp251.bounds;_LL1CB: Cyc_Tcutil_unify_it(
_tmp252,_tmp24B);Cyc_Tcutil_unify_it(_tmp24C,_tmp253);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_unify_tqual(_tmp255,_tmp24E);Cyc_Tcutil_unify_it_conrefs(
Cyc_Tcutil_boundscmp,_tmp256,_tmp24F,_tag_arr("(different pointer bounds)",
sizeof(unsigned char),27));{void*_tmp2AB=(void*)(Cyc_Absyn_compress_conref(
_tmp256))->v;void*_tmp2AC;_LL1EF: if(_tmp2AB <= (void*)1?1:*((int*)_tmp2AB)!= 0)
goto _LL1F1;_tmp2AC=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2AB)->f1;if((
int)_tmp2AC != 0)goto _LL1F1;_LL1F0: return;_LL1F1:;_LL1F2: goto _LL1EE;_LL1EE:;}((
void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,
struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp254,
_tmp24D,_tag_arr("(different pointer types)",sizeof(unsigned char),26));return;
_LL1CC: _tmp257=_tmp217.f1;if(_tmp257 <= (void*)3?1:*((int*)_tmp257)!= 5)goto
_LL1CE;_tmp258=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp257)->f1;_tmp259=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp257)->f2;_tmp25A=_tmp217.f2;if(
_tmp25A <= (void*)3?1:*((int*)_tmp25A)!= 5)goto _LL1CE;_tmp25B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp25A)->f1;_tmp25C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp25A)->f2;
_LL1CD: if(_tmp25B == _tmp258?_tmp25C == _tmp259: 0)return;Cyc_Tcutil_failure_reason=
_tag_arr("(different integral types)",sizeof(unsigned char),27);goto _LL1B9;
_LL1CE: _tmp25D=_tmp217.f1;if((int)_tmp25D != 1)goto _LL1D0;_tmp25E=_tmp217.f2;if((
int)_tmp25E != 1)goto _LL1D0;_LL1CF: return;_LL1D0: _tmp25F=_tmp217.f1;if(_tmp25F <= (
void*)3?1:*((int*)_tmp25F)!= 6)goto _LL1D2;_tmp260=((struct Cyc_Absyn_DoubleType_struct*)
_tmp25F)->f1;_tmp261=_tmp217.f2;if(_tmp261 <= (void*)3?1:*((int*)_tmp261)!= 6)
goto _LL1D2;_tmp262=((struct Cyc_Absyn_DoubleType_struct*)_tmp261)->f1;_LL1D1: if(
_tmp260 == _tmp262)return;goto _LL1B9;_LL1D2: _tmp263=_tmp217.f1;if(_tmp263 <= (void*)
3?1:*((int*)_tmp263)!= 14)goto _LL1D4;_tmp264=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp263)->f1;_tmp265=_tmp217.f2;if(_tmp265 <= (void*)3?1:*((int*)_tmp265)!= 14)
goto _LL1D4;_tmp266=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp265)->f1;
_LL1D3: Cyc_Tcutil_unify_it(_tmp264,_tmp266);return;_LL1D4: _tmp267=_tmp217.f1;if(
_tmp267 <= (void*)3?1:*((int*)_tmp267)!= 7)goto _LL1D6;_tmp268=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp267)->f1;_tmp269=((struct Cyc_Absyn_ArrayType_struct*)_tmp267)->f2;_tmp26A=((
struct Cyc_Absyn_ArrayType_struct*)_tmp267)->f3;_tmp26B=_tmp217.f2;if(_tmp26B <= (
void*)3?1:*((int*)_tmp26B)!= 7)goto _LL1D6;_tmp26C=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp26B)->f1;_tmp26D=((struct Cyc_Absyn_ArrayType_struct*)_tmp26B)->f2;_tmp26E=((
struct Cyc_Absyn_ArrayType_struct*)_tmp26B)->f3;_LL1D5: Cyc_Tcutil_unify_tqual(
_tmp26D,_tmp269);Cyc_Tcutil_unify_it(_tmp26C,_tmp268);if(_tmp26A == _tmp26E)
return;if(_tmp26A == 0?1: _tmp26E == 0)goto _LL1B9;if(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp26A))== Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp26E)))return;Cyc_Tcutil_failure_reason=
_tag_arr("(different array sizes)",sizeof(unsigned char),24);goto _LL1B9;_LL1D6:
_tmp26F=_tmp217.f1;if(_tmp26F <= (void*)3?1:*((int*)_tmp26F)!= 8)goto _LL1D8;
_tmp270=((struct Cyc_Absyn_FnType_struct*)_tmp26F)->f1;_tmp271=_tmp270.tvars;
_tmp272=_tmp270.effect;_tmp273=(void*)_tmp270.ret_typ;_tmp274=_tmp270.args;
_tmp275=_tmp270.c_varargs;_tmp276=_tmp270.cyc_varargs;_tmp277=_tmp270.rgn_po;
_tmp278=_tmp270.attributes;_tmp279=_tmp217.f2;if(_tmp279 <= (void*)3?1:*((int*)
_tmp279)!= 8)goto _LL1D8;_tmp27A=((struct Cyc_Absyn_FnType_struct*)_tmp279)->f1;
_tmp27B=_tmp27A.tvars;_tmp27C=_tmp27A.effect;_tmp27D=(void*)_tmp27A.ret_typ;
_tmp27E=_tmp27A.args;_tmp27F=_tmp27A.c_varargs;_tmp280=_tmp27A.cyc_varargs;
_tmp281=_tmp27A.rgn_po;_tmp282=_tmp27A.attributes;_LL1D7: {int done=0;{struct
_RegionHandle _tmp2AD=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2AD;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp27B != 0){if(_tmp271 == 0){
Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too few type variables)",
sizeof(unsigned char),50);(int)_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp2AE=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp27B->hd);void*_tmp2AF=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp271->hd);if(_tmp2AE != _tmp2AF){Cyc_Tcutil_failure_reason=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp2B3;_tmp2B3.tag=0;_tmp2B3.f1=(
struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp2AF);{struct Cyc_Std_String_pa_struct
_tmp2B2;_tmp2B2.tag=0;_tmp2B2.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2AE);{struct Cyc_Std_String_pa_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(struct
_tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp27B->hd);{void*
_tmp2B0[3]={& _tmp2B1,& _tmp2B2,& _tmp2B3};Cyc_Std_aprintf(_tag_arr("(type var %s has different kinds %s and %s)",
sizeof(unsigned char),44),_tag_arr(_tmp2B0,sizeof(void*),3));}}}});(int)_throw((
void*)Cyc_Tcutil_Unify);}inst=({struct Cyc_List_List*_tmp2B4=_region_malloc(rgn,
sizeof(*_tmp2B4));_tmp2B4->hd=({struct _tuple6*_tmp2B5=_region_malloc(rgn,sizeof(*
_tmp2B5));_tmp2B5->f1=(struct Cyc_Absyn_Tvar*)_tmp271->hd;_tmp2B5->f2=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6[0]=({
struct Cyc_Absyn_VarType_struct _tmp2B7;_tmp2B7.tag=1;_tmp2B7.f1=(struct Cyc_Absyn_Tvar*)
_tmp27B->hd;_tmp2B7;});_tmp2B6;});_tmp2B5;});_tmp2B4->tl=inst;_tmp2B4;});_tmp27B=
_tmp27B->tl;_tmp271=_tmp271->tl;}}if(_tmp271 != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too many type variables)",sizeof(
unsigned char),51);_npop_handler(0);goto _LL1B9;}if(inst != 0){Cyc_Tcutil_unify_it((
void*)({struct Cyc_Absyn_FnType_struct*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));
_tmp2B8[0]=({struct Cyc_Absyn_FnType_struct _tmp2B9;_tmp2B9.tag=8;_tmp2B9.f1=({
struct Cyc_Absyn_FnInfo _tmp2BA;_tmp2BA.tvars=0;_tmp2BA.effect=_tmp27C;_tmp2BA.ret_typ=(
void*)_tmp27D;_tmp2BA.args=_tmp27E;_tmp2BA.c_varargs=_tmp27F;_tmp2BA.cyc_varargs=
_tmp280;_tmp2BA.rgn_po=_tmp281;_tmp2BA.attributes=_tmp282;_tmp2BA;});_tmp2B9;});
_tmp2B8;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB[0]=({struct Cyc_Absyn_FnType_struct
_tmp2BC;_tmp2BC.tag=8;_tmp2BC.f1=({struct Cyc_Absyn_FnInfo _tmp2BD;_tmp2BD.tvars=0;
_tmp2BD.effect=_tmp272;_tmp2BD.ret_typ=(void*)_tmp273;_tmp2BD.args=_tmp274;
_tmp2BD.c_varargs=_tmp275;_tmp2BD.cyc_varargs=_tmp276;_tmp2BD.rgn_po=_tmp277;
_tmp2BD.attributes=_tmp278;_tmp2BD;});_tmp2BC;});_tmp2BB;})));done=1;}};
_pop_region(rgn);}if(done)return;Cyc_Tcutil_unify_it(_tmp27D,_tmp273);for(0;
_tmp27E != 0?_tmp274 != 0: 0;(_tmp27E=_tmp27E->tl,_tmp274=_tmp274->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)_tmp27E->hd)).f2,(*((struct _tuple2*)_tmp274->hd)).f2);Cyc_Tcutil_unify_it((*((
struct _tuple2*)_tmp27E->hd)).f3,(*((struct _tuple2*)_tmp274->hd)).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp27E != 0?1: _tmp274 != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)",sizeof(
unsigned char),52);goto _LL1B9;}if(_tmp27F != _tmp275){Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes C varargs)",sizeof(unsigned char),41);
goto _LL1B9;}{int bad_cyc_vararg=0;{struct _tuple12 _tmp2BF=({struct _tuple12 _tmp2BE;
_tmp2BE.f1=_tmp280;_tmp2BE.f2=_tmp276;_tmp2BE;});struct Cyc_Absyn_VarargInfo*
_tmp2C0;struct Cyc_Absyn_VarargInfo*_tmp2C1;struct Cyc_Absyn_VarargInfo*_tmp2C2;
struct Cyc_Absyn_VarargInfo*_tmp2C3;struct Cyc_Absyn_VarargInfo*_tmp2C4;struct Cyc_Absyn_VarargInfo
_tmp2C5;struct Cyc_Core_Opt*_tmp2C6;struct Cyc_Absyn_Tqual _tmp2C7;void*_tmp2C8;int
_tmp2C9;struct Cyc_Absyn_VarargInfo*_tmp2CA;struct Cyc_Absyn_VarargInfo _tmp2CB;
struct Cyc_Core_Opt*_tmp2CC;struct Cyc_Absyn_Tqual _tmp2CD;void*_tmp2CE;int _tmp2CF;
_LL1F4: _tmp2C0=_tmp2BF.f1;if(_tmp2C0 != 0)goto _LL1F6;_tmp2C1=_tmp2BF.f2;if(
_tmp2C1 != 0)goto _LL1F6;_LL1F5: goto _LL1F3;_LL1F6: _tmp2C2=_tmp2BF.f1;if(_tmp2C2 != 
0)goto _LL1F8;_LL1F7: goto _LL1F9;_LL1F8: _tmp2C3=_tmp2BF.f2;if(_tmp2C3 != 0)goto
_LL1FA;_LL1F9: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes varargs)",
sizeof(unsigned char),39);goto _LL1F3;_LL1FA: _tmp2C4=_tmp2BF.f1;if(_tmp2C4 == 0)
goto _LL1F3;_tmp2C5=*_tmp2C4;_tmp2C6=_tmp2C5.name;_tmp2C7=_tmp2C5.tq;_tmp2C8=(
void*)_tmp2C5.type;_tmp2C9=_tmp2C5.inject;_tmp2CA=_tmp2BF.f2;if(_tmp2CA == 0)goto
_LL1F3;_tmp2CB=*_tmp2CA;_tmp2CC=_tmp2CB.name;_tmp2CD=_tmp2CB.tq;_tmp2CE=(void*)
_tmp2CB.type;_tmp2CF=_tmp2CB.inject;_LL1FB: Cyc_Tcutil_unify_tqual(_tmp2C7,
_tmp2CD);Cyc_Tcutil_unify_it(_tmp2C8,_tmp2CE);if(_tmp2C9 != _tmp2CF){
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=_tag_arr("(only one function type injects varargs)",
sizeof(unsigned char),41);}goto _LL1F3;_LL1F3:;}if(bad_cyc_vararg)goto _LL1B9;{int
bad_effect=0;{struct _tuple13 _tmp2D1=({struct _tuple13 _tmp2D0;_tmp2D0.f1=_tmp27C;
_tmp2D0.f2=_tmp272;_tmp2D0;});struct Cyc_Core_Opt*_tmp2D2;struct Cyc_Core_Opt*
_tmp2D3;struct Cyc_Core_Opt*_tmp2D4;struct Cyc_Core_Opt*_tmp2D5;_LL1FD: _tmp2D2=
_tmp2D1.f1;if(_tmp2D2 != 0)goto _LL1FF;_tmp2D3=_tmp2D1.f2;if(_tmp2D3 != 0)goto
_LL1FF;_LL1FE: goto _LL1FC;_LL1FF: _tmp2D4=_tmp2D1.f1;if(_tmp2D4 != 0)goto _LL201;
_LL200: goto _LL202;_LL201: _tmp2D5=_tmp2D1.f2;if(_tmp2D5 != 0)goto _LL203;_LL202:
bad_effect=1;goto _LL1FC;_LL203:;_LL204: bad_effect=!Cyc_Tcutil_unify_effect((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp27C))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp272))->v);goto _LL1FC;_LL1FC:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=_tag_arr("(function type effects do not unify)",
sizeof(unsigned char),37);goto _LL1B9;}if(!Cyc_Tcutil_same_atts(_tmp278,_tmp282)){
Cyc_Tcutil_failure_reason=_tag_arr("(function types have different attributes)",
sizeof(unsigned char),43);goto _LL1B9;}if(!Cyc_Tcutil_same_rgn_po(_tmp277,_tmp281)){
Cyc_Tcutil_failure_reason=_tag_arr("(function types have different region lifetime orderings)",
sizeof(unsigned char),58);goto _LL1B9;}return;}}}_LL1D8: _tmp283=_tmp217.f1;if(
_tmp283 <= (void*)3?1:*((int*)_tmp283)!= 9)goto _LL1DA;_tmp284=((struct Cyc_Absyn_TupleType_struct*)
_tmp283)->f1;_tmp285=_tmp217.f2;if(_tmp285 <= (void*)3?1:*((int*)_tmp285)!= 9)
goto _LL1DA;_tmp286=((struct Cyc_Absyn_TupleType_struct*)_tmp285)->f1;_LL1D9: for(0;
_tmp286 != 0?_tmp284 != 0: 0;(_tmp286=_tmp286->tl,_tmp284=_tmp284->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)_tmp286->hd)).f1,(*((struct _tuple4*)_tmp284->hd)).f1);Cyc_Tcutil_unify_it((*((
struct _tuple4*)_tmp286->hd)).f2,(*((struct _tuple4*)_tmp284->hd)).f2);}if(_tmp286
== 0?_tmp284 == 0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(tuple types have different numbers of components)",sizeof(
unsigned char),51);goto _LL1B9;_LL1DA: _tmp287=_tmp217.f1;if(_tmp287 <= (void*)3?1:*((
int*)_tmp287)!= 11)goto _LL1DC;_tmp288=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp287)->f1;_tmp289=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp287)->f2;_tmp28A=
_tmp217.f2;if(_tmp28A <= (void*)3?1:*((int*)_tmp28A)!= 11)goto _LL1DC;_tmp28B=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp28A)->f1;_tmp28C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp28A)->f2;_LL1DB: if(_tmp28B != _tmp288){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(unsigned char),24);goto _LL1B9;}for(0;_tmp28C != 0?_tmp289 != 0: 0;(_tmp28C=
_tmp28C->tl,_tmp289=_tmp289->tl)){struct Cyc_Absyn_Aggrfield*_tmp2D6=(struct Cyc_Absyn_Aggrfield*)
_tmp28C->hd;struct Cyc_Absyn_Aggrfield*_tmp2D7=(struct Cyc_Absyn_Aggrfield*)
_tmp289->hd;if(Cyc_Std_zstrptrcmp(_tmp2D6->name,_tmp2D7->name)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different member names)",sizeof(unsigned char),25);(int)_throw((void*)
Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_tqual(_tmp2D6->tq,_tmp2D7->tq);Cyc_Tcutil_unify_it((
void*)_tmp2D6->type,(void*)_tmp2D7->type);if(!Cyc_Tcutil_same_atts(_tmp2D6->attributes,
_tmp2D7->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different attributes on member)",sizeof(unsigned char),33);(int)
_throw((void*)Cyc_Tcutil_Unify);}if(((_tmp2D6->width != 0?_tmp2D7->width == 0: 0)?1:(
_tmp2D7->width != 0?_tmp2D6->width == 0: 0))?1:((_tmp2D6->width != 0?_tmp2D7->width
!= 0: 0)?Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp2D6->width))
!= Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp2D7->width)):
0)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different bitfield widths on member)",sizeof(unsigned char),38);(int)
_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp28C == 0?_tmp289 == 0: 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=_tag_arr("(different number of members)",
sizeof(unsigned char),30);goto _LL1B9;_LL1DC: _tmp28D=_tmp217.f1;if((int)_tmp28D != 
2)goto _LL1DE;_tmp28E=_tmp217.f2;if((int)_tmp28E != 2)goto _LL1DE;_LL1DD: return;
_LL1DE: _tmp28F=_tmp217.f1;if(_tmp28F <= (void*)3?1:*((int*)_tmp28F)!= 15)goto
_LL1E0;_tmp290=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp28F)->f1;
_tmp291=_tmp217.f2;if(_tmp291 <= (void*)3?1:*((int*)_tmp291)!= 15)goto _LL1E0;
_tmp292=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp291)->f1;_LL1DF: Cyc_Tcutil_unify_it(
_tmp290,_tmp292);return;_LL1E0: _tmp293=_tmp217.f1;if(_tmp293 <= (void*)3?1:*((int*)
_tmp293)!= 18)goto _LL1E2;_LL1E1: goto _LL1E3;_LL1E2: _tmp294=_tmp217.f2;if(_tmp294
<= (void*)3?1:*((int*)_tmp294)!= 18)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: _tmp295=
_tmp217.f1;if(_tmp295 <= (void*)3?1:*((int*)_tmp295)!= 17)goto _LL1E6;_LL1E5: goto
_LL1E7;_LL1E6: _tmp296=_tmp217.f1;if(_tmp296 <= (void*)3?1:*((int*)_tmp296)!= 19)
goto _LL1E8;_LL1E7: goto _LL1E9;_LL1E8: _tmp297=_tmp217.f2;if(_tmp297 <= (void*)3?1:*((
int*)_tmp297)!= 19)goto _LL1EA;_LL1E9: goto _LL1EB;_LL1EA: _tmp298=_tmp217.f2;if(
_tmp298 <= (void*)3?1:*((int*)_tmp298)!= 17)goto _LL1EC;_LL1EB: if(Cyc_Tcutil_unify_effect(
t1,t2))return;Cyc_Tcutil_failure_reason=_tag_arr("(effects don't unify)",sizeof(
unsigned char),22);goto _LL1B9;_LL1EC:;_LL1ED: goto _LL1B9;_LL1B9:;}(int)_throw((
void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_opt_cmp(int(*cmp)(void*,void*),struct Cyc_Core_Opt*
a1,struct Cyc_Core_Opt*a2){if(a1 == a2)return 0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 
0?a2 == 0: 0)return 1;return cmp((void*)((struct Cyc_Core_Opt*)_check_null(a1))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(a2))->v);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0?a2 != 0: 0)return -
1;if(a1 != 0?a2 == 0: 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(
a2));}int Cyc_Tcutil_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2){if(l1 == l2)return 0;for(0;l1 != 0?l2 != 0: 0;(l1=l1->tl,l2=
l2->tl)){int _tmp2D8=cmp((void*)l1->hd,(void*)l2->hd);if(_tmp2D8 != 0)return
_tmp2D8;}if(l2 != 0)return - 1;if(l1 != 0)return 1;return 0;}static int Cyc_Tcutil_tqual_cmp(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int _tmp2D9=(tq1.q_const + (tq1.q_volatile
<< 1))+ (tq1.q_restrict << 2);int _tmp2DA=(tq2.q_const + (tq2.q_volatile << 1))+ (
tq2.q_restrict << 2);return Cyc_Core_intcmp(_tmp2D9,_tmp2DA);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp2DB=(void*)x->v;
void*_tmp2DC;_LL206: if((int)_tmp2DB != 0)goto _LL208;_LL207: return - 1;_LL208: if(
_tmp2DB <= (void*)1?1:*((int*)_tmp2DB)!= 0)goto _LL20A;_tmp2DC=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2DB)->f1;_LL209: {void*_tmp2DD=(void*)y->v;void*_tmp2DE;_LL20D: if((int)
_tmp2DD != 0)goto _LL20F;_LL20E: return 1;_LL20F: if(_tmp2DD <= (void*)1?1:*((int*)
_tmp2DD)!= 0)goto _LL211;_tmp2DE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2DD)->f1;_LL210: return cmp(_tmp2DC,_tmp2DE);_LL211: if(_tmp2DD <= (void*)1?1:*((
int*)_tmp2DD)!= 1)goto _LL20C;_LL212:({void*_tmp2DF[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress(2)",
sizeof(unsigned char),40),_tag_arr(_tmp2DF,sizeof(void*),0));});_LL20C:;}_LL20A:
if(_tmp2DB <= (void*)1?1:*((int*)_tmp2DB)!= 1)goto _LL205;_LL20B:({void*_tmp2E0[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress",
sizeof(unsigned char),37),_tag_arr(_tmp2E0,sizeof(void*),0));});_LL205:;}}static
int Cyc_Tcutil_tqual_type_cmp(struct _tuple4*tqt1,struct _tuple4*tqt2){struct
_tuple4 _tmp2E2;struct Cyc_Absyn_Tqual _tmp2E3;void*_tmp2E4;struct _tuple4*_tmp2E1=
tqt1;_tmp2E2=*_tmp2E1;_tmp2E3=_tmp2E2.f1;_tmp2E4=_tmp2E2.f2;{struct _tuple4
_tmp2E6;struct Cyc_Absyn_Tqual _tmp2E7;void*_tmp2E8;struct _tuple4*_tmp2E5=tqt2;
_tmp2E6=*_tmp2E5;_tmp2E7=_tmp2E6.f1;_tmp2E8=_tmp2E6.f2;{int _tmp2E9=Cyc_Tcutil_tqual_cmp(
_tmp2E3,_tmp2E7);if(_tmp2E9 != 0)return _tmp2E9;return Cyc_Tcutil_typecmp(_tmp2E4,
_tmp2E8);}}}static int Cyc_Tcutil_const_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){return Cyc_Core_intcmp((int)Cyc_Evexp_eval_const_uint_exp(e1),(
int)Cyc_Evexp_eval_const_uint_exp(e2));}static int Cyc_Tcutil_attribute_case_number(
void*att){void*_tmp2EA=att;_LL214: if(_tmp2EA <= (void*)16?1:*((int*)_tmp2EA)!= 0)
goto _LL216;_LL215: return 0;_LL216: if((int)_tmp2EA != 0)goto _LL218;_LL217: return 1;
_LL218: if((int)_tmp2EA != 1)goto _LL21A;_LL219: return 2;_LL21A: if((int)_tmp2EA != 2)
goto _LL21C;_LL21B: return 3;_LL21C: if((int)_tmp2EA != 3)goto _LL21E;_LL21D: return 4;
_LL21E: if((int)_tmp2EA != 4)goto _LL220;_LL21F: return 5;_LL220: if(_tmp2EA <= (void*)
16?1:*((int*)_tmp2EA)!= 1)goto _LL222;_LL221: return 6;_LL222: if((int)_tmp2EA != 5)
goto _LL224;_LL223: return 7;_LL224: if(_tmp2EA <= (void*)16?1:*((int*)_tmp2EA)!= 2)
goto _LL226;_LL225: return 8;_LL226: if((int)_tmp2EA != 6)goto _LL228;_LL227: return 9;
_LL228: if((int)_tmp2EA != 7)goto _LL22A;_LL229: return 10;_LL22A: if((int)_tmp2EA != 8)
goto _LL22C;_LL22B: return 11;_LL22C: if((int)_tmp2EA != 9)goto _LL22E;_LL22D: return 12;
_LL22E: if((int)_tmp2EA != 10)goto _LL230;_LL22F: return 13;_LL230: if((int)_tmp2EA != 
11)goto _LL232;_LL231: return 14;_LL232: if((int)_tmp2EA != 12)goto _LL234;_LL233:
return 15;_LL234: if((int)_tmp2EA != 13)goto _LL236;_LL235: return 16;_LL236: if((int)
_tmp2EA != 14)goto _LL238;_LL237: return 17;_LL238: if((int)_tmp2EA != 15)goto _LL23A;
_LL239: return 18;_LL23A: if(_tmp2EA <= (void*)16?1:*((int*)_tmp2EA)!= 3)goto _LL213;
_LL23B: return 19;_LL213:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple9 _tmp2EC=({struct _tuple9 _tmp2EB;_tmp2EB.f1=att1;_tmp2EB.f2=att2;
_tmp2EB;});void*_tmp2ED;int _tmp2EE;void*_tmp2EF;int _tmp2F0;void*_tmp2F1;int
_tmp2F2;void*_tmp2F3;int _tmp2F4;void*_tmp2F5;struct _tagged_arr _tmp2F6;void*
_tmp2F7;struct _tagged_arr _tmp2F8;void*_tmp2F9;void*_tmp2FA;int _tmp2FB;int _tmp2FC;
void*_tmp2FD;void*_tmp2FE;int _tmp2FF;int _tmp300;_LL23D: _tmp2ED=_tmp2EC.f1;if(
_tmp2ED <= (void*)16?1:*((int*)_tmp2ED)!= 0)goto _LL23F;_tmp2EE=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp2ED)->f1;_tmp2EF=_tmp2EC.f2;if(_tmp2EF <= (void*)16?1:*((int*)_tmp2EF)!= 0)
goto _LL23F;_tmp2F0=((struct Cyc_Absyn_Regparm_att_struct*)_tmp2EF)->f1;_LL23E:
_tmp2F2=_tmp2EE;_tmp2F4=_tmp2F0;goto _LL240;_LL23F: _tmp2F1=_tmp2EC.f1;if(_tmp2F1
<= (void*)16?1:*((int*)_tmp2F1)!= 1)goto _LL241;_tmp2F2=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp2F1)->f1;_tmp2F3=_tmp2EC.f2;if(_tmp2F3 <= (void*)16?1:*((int*)_tmp2F3)!= 1)
goto _LL241;_tmp2F4=((struct Cyc_Absyn_Aligned_att_struct*)_tmp2F3)->f1;_LL240:
return Cyc_Core_intcmp(_tmp2F2,_tmp2F4);_LL241: _tmp2F5=_tmp2EC.f1;if(_tmp2F5 <= (
void*)16?1:*((int*)_tmp2F5)!= 2)goto _LL243;_tmp2F6=((struct Cyc_Absyn_Section_att_struct*)
_tmp2F5)->f1;_tmp2F7=_tmp2EC.f2;if(_tmp2F7 <= (void*)16?1:*((int*)_tmp2F7)!= 2)
goto _LL243;_tmp2F8=((struct Cyc_Absyn_Section_att_struct*)_tmp2F7)->f1;_LL242:
return Cyc_Std_strcmp(_tmp2F6,_tmp2F8);_LL243: _tmp2F9=_tmp2EC.f1;if(_tmp2F9 <= (
void*)16?1:*((int*)_tmp2F9)!= 3)goto _LL245;_tmp2FA=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp2F9)->f1;_tmp2FB=((struct Cyc_Absyn_Format_att_struct*)_tmp2F9)->f2;_tmp2FC=((
struct Cyc_Absyn_Format_att_struct*)_tmp2F9)->f3;_tmp2FD=_tmp2EC.f2;if(_tmp2FD <= (
void*)16?1:*((int*)_tmp2FD)!= 3)goto _LL245;_tmp2FE=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp2FD)->f1;_tmp2FF=((struct Cyc_Absyn_Format_att_struct*)_tmp2FD)->f2;_tmp300=((
struct Cyc_Absyn_Format_att_struct*)_tmp2FD)->f3;_LL244: {int _tmp301=Cyc_Core_intcmp((
int)((unsigned int)_tmp2FA),(int)((unsigned int)_tmp2FE));if(_tmp301 != 0)return
_tmp301;{int _tmp302=Cyc_Core_intcmp(_tmp2FB,_tmp2FF);if(_tmp302 != 0)return
_tmp302;return Cyc_Core_intcmp(_tmp2FC,_tmp300);}}_LL245:;_LL246: return Cyc_Core_intcmp(
Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));
_LL23C:;}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct
Cyc_Absyn_Aggrfield*f2){int _tmp303=Cyc_Std_zstrptrcmp(f1->name,f2->name);if(
_tmp303 != 0)return _tmp303;{int _tmp304=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp304 != 0)return _tmp304;{int _tmp305=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp305 != 0)return _tmp305;{int _tmp306=Cyc_Tcutil_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp306 != 0)return _tmp306;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,f1->width,f2->width);}}}}
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp307=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp307 != 0)return
_tmp307;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t){void*_tmp308=t;
_LL248: if((int)_tmp308 != 0)goto _LL24A;_LL249: return 0;_LL24A: if(_tmp308 <= (void*)
3?1:*((int*)_tmp308)!= 0)goto _LL24C;_LL24B: return 1;_LL24C: if(_tmp308 <= (void*)3?
1:*((int*)_tmp308)!= 1)goto _LL24E;_LL24D: return 2;_LL24E: if(_tmp308 <= (void*)3?1:*((
int*)_tmp308)!= 2)goto _LL250;_LL24F: return 3;_LL250: if(_tmp308 <= (void*)3?1:*((
int*)_tmp308)!= 3)goto _LL252;_LL251: return 4;_LL252: if(_tmp308 <= (void*)3?1:*((
int*)_tmp308)!= 4)goto _LL254;_LL253: return 5;_LL254: if(_tmp308 <= (void*)3?1:*((
int*)_tmp308)!= 5)goto _LL256;_LL255: return 6;_LL256: if((int)_tmp308 != 1)goto
_LL258;_LL257: return 7;_LL258: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 6)goto
_LL25A;_LL259: return 8;_LL25A: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 7)goto
_LL25C;_LL25B: return 9;_LL25C: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 8)goto
_LL25E;_LL25D: return 10;_LL25E: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 9)goto
_LL260;_LL25F: return 11;_LL260: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 10)goto
_LL262;_LL261: return 12;_LL262: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 11)goto
_LL264;_LL263: return 14;_LL264: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 12)goto
_LL266;_LL265: return 16;_LL266: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 13)goto
_LL268;_LL267: return 17;_LL268: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 15)goto
_LL26A;_LL269: return 18;_LL26A: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 16)goto
_LL26C;_LL26B: return 19;_LL26C: if((int)_tmp308 != 2)goto _LL26E;_LL26D: return 20;
_LL26E: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 17)goto _LL270;_LL26F: return 21;
_LL270: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 18)goto _LL272;_LL271: return 22;
_LL272: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 19)goto _LL274;_LL273: return 23;
_LL274: if(_tmp308 <= (void*)3?1:*((int*)_tmp308)!= 14)goto _LL247;_LL275: return 24;
_LL247:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp309=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(
t1),Cyc_Tcutil_type_case_number(t2));if(_tmp309 != 0)return _tmp309;{struct _tuple9
_tmp30B=({struct _tuple9 _tmp30A;_tmp30A.f1=t2;_tmp30A.f2=t1;_tmp30A;});void*
_tmp30C;void*_tmp30D;void*_tmp30E;void*_tmp30F;void*_tmp310;struct Cyc_Absyn_Tvar*
_tmp311;void*_tmp312;struct Cyc_Absyn_Tvar*_tmp313;void*_tmp314;struct Cyc_Absyn_AggrInfo
_tmp315;void*_tmp316;struct Cyc_List_List*_tmp317;void*_tmp318;struct Cyc_Absyn_AggrInfo
_tmp319;void*_tmp31A;struct Cyc_List_List*_tmp31B;void*_tmp31C;struct _tuple1*
_tmp31D;void*_tmp31E;struct _tuple1*_tmp31F;void*_tmp320;struct Cyc_List_List*
_tmp321;void*_tmp322;struct Cyc_List_List*_tmp323;void*_tmp324;struct Cyc_Absyn_TunionInfo
_tmp325;void*_tmp326;struct Cyc_Absyn_Tuniondecl**_tmp327;struct Cyc_Absyn_Tuniondecl*
_tmp328;struct Cyc_List_List*_tmp329;void*_tmp32A;void*_tmp32B;struct Cyc_Absyn_TunionInfo
_tmp32C;void*_tmp32D;struct Cyc_Absyn_Tuniondecl**_tmp32E;struct Cyc_Absyn_Tuniondecl*
_tmp32F;struct Cyc_List_List*_tmp330;void*_tmp331;void*_tmp332;struct Cyc_Absyn_TunionFieldInfo
_tmp333;void*_tmp334;struct Cyc_Absyn_Tuniondecl*_tmp335;struct Cyc_Absyn_Tunionfield*
_tmp336;struct Cyc_List_List*_tmp337;void*_tmp338;struct Cyc_Absyn_TunionFieldInfo
_tmp339;void*_tmp33A;struct Cyc_Absyn_Tuniondecl*_tmp33B;struct Cyc_Absyn_Tunionfield*
_tmp33C;struct Cyc_List_List*_tmp33D;void*_tmp33E;struct Cyc_Absyn_PtrInfo _tmp33F;
void*_tmp340;void*_tmp341;struct Cyc_Absyn_Conref*_tmp342;struct Cyc_Absyn_Tqual
_tmp343;struct Cyc_Absyn_Conref*_tmp344;void*_tmp345;struct Cyc_Absyn_PtrInfo
_tmp346;void*_tmp347;void*_tmp348;struct Cyc_Absyn_Conref*_tmp349;struct Cyc_Absyn_Tqual
_tmp34A;struct Cyc_Absyn_Conref*_tmp34B;void*_tmp34C;void*_tmp34D;void*_tmp34E;
void*_tmp34F;void*_tmp350;void*_tmp351;void*_tmp352;void*_tmp353;void*_tmp354;
int _tmp355;void*_tmp356;int _tmp357;void*_tmp358;void*_tmp359;struct Cyc_Absyn_Tqual
_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;void*_tmp35C;void*_tmp35D;struct Cyc_Absyn_Tqual
_tmp35E;struct Cyc_Absyn_Exp*_tmp35F;void*_tmp360;struct Cyc_Absyn_FnInfo _tmp361;
struct Cyc_List_List*_tmp362;struct Cyc_Core_Opt*_tmp363;void*_tmp364;struct Cyc_List_List*
_tmp365;int _tmp366;struct Cyc_Absyn_VarargInfo*_tmp367;struct Cyc_List_List*
_tmp368;struct Cyc_List_List*_tmp369;void*_tmp36A;struct Cyc_Absyn_FnInfo _tmp36B;
struct Cyc_List_List*_tmp36C;struct Cyc_Core_Opt*_tmp36D;void*_tmp36E;struct Cyc_List_List*
_tmp36F;int _tmp370;struct Cyc_Absyn_VarargInfo*_tmp371;struct Cyc_List_List*
_tmp372;struct Cyc_List_List*_tmp373;void*_tmp374;struct Cyc_List_List*_tmp375;
void*_tmp376;struct Cyc_List_List*_tmp377;void*_tmp378;void*_tmp379;struct Cyc_List_List*
_tmp37A;void*_tmp37B;void*_tmp37C;struct Cyc_List_List*_tmp37D;void*_tmp37E;void*
_tmp37F;void*_tmp380;void*_tmp381;void*_tmp382;void*_tmp383;void*_tmp384;void*
_tmp385;void*_tmp386;void*_tmp387;void*_tmp388;void*_tmp389;void*_tmp38A;void*
_tmp38B;void*_tmp38C;void*_tmp38D;_LL277: _tmp30C=_tmp30B.f1;if(_tmp30C <= (void*)
3?1:*((int*)_tmp30C)!= 0)goto _LL279;_tmp30D=_tmp30B.f2;if(_tmp30D <= (void*)3?1:*((
int*)_tmp30D)!= 0)goto _LL279;_LL278:({void*_tmp38E[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: can only compare closed types",
sizeof(unsigned char),39),_tag_arr(_tmp38E,sizeof(void*),0));});_LL279: _tmp30E=
_tmp30B.f1;if((int)_tmp30E != 0)goto _LL27B;_tmp30F=_tmp30B.f2;if((int)_tmp30F != 0)
goto _LL27B;_LL27A: return 0;_LL27B: _tmp310=_tmp30B.f1;if(_tmp310 <= (void*)3?1:*((
int*)_tmp310)!= 1)goto _LL27D;_tmp311=((struct Cyc_Absyn_VarType_struct*)_tmp310)->f1;
_tmp312=_tmp30B.f2;if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 1)goto _LL27D;
_tmp313=((struct Cyc_Absyn_VarType_struct*)_tmp312)->f1;_LL27C: return Cyc_Core_intcmp(*((
int*)_check_null(_tmp313->identity)),*((int*)_check_null(_tmp311->identity)));
_LL27D: _tmp314=_tmp30B.f1;if(_tmp314 <= (void*)3?1:*((int*)_tmp314)!= 10)goto
_LL27F;_tmp315=((struct Cyc_Absyn_AggrType_struct*)_tmp314)->f1;_tmp316=(void*)
_tmp315.aggr_info;_tmp317=_tmp315.targs;_tmp318=_tmp30B.f2;if(_tmp318 <= (void*)3?
1:*((int*)_tmp318)!= 10)goto _LL27F;_tmp319=((struct Cyc_Absyn_AggrType_struct*)
_tmp318)->f1;_tmp31A=(void*)_tmp319.aggr_info;_tmp31B=_tmp319.targs;_LL27E: {
struct _tuple1*_tmp390;struct _tuple5 _tmp38F=Cyc_Absyn_aggr_kinded_name(_tmp316);
_tmp390=_tmp38F.f2;{struct _tuple1*_tmp392;struct _tuple5 _tmp391=Cyc_Absyn_aggr_kinded_name(
_tmp31A);_tmp392=_tmp391.f2;{int _tmp393=Cyc_Absyn_qvar_cmp(_tmp390,_tmp392);if(
_tmp393 != 0)return _tmp393;else{return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp317,_tmp31B);}}}}_LL27F: _tmp31C=_tmp30B.f1;if(_tmp31C <= (void*)3?1:*((int*)
_tmp31C)!= 12)goto _LL281;_tmp31D=((struct Cyc_Absyn_EnumType_struct*)_tmp31C)->f1;
_tmp31E=_tmp30B.f2;if(_tmp31E <= (void*)3?1:*((int*)_tmp31E)!= 12)goto _LL281;
_tmp31F=((struct Cyc_Absyn_EnumType_struct*)_tmp31E)->f1;_LL280: return Cyc_Absyn_qvar_cmp(
_tmp31D,_tmp31F);_LL281: _tmp320=_tmp30B.f1;if(_tmp320 <= (void*)3?1:*((int*)
_tmp320)!= 13)goto _LL283;_tmp321=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp320)->f1;
_tmp322=_tmp30B.f2;if(_tmp322 <= (void*)3?1:*((int*)_tmp322)!= 13)goto _LL283;
_tmp323=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp322)->f1;_LL282: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp321,
_tmp323);_LL283: _tmp324=_tmp30B.f1;if(_tmp324 <= (void*)3?1:*((int*)_tmp324)!= 2)
goto _LL285;_tmp325=((struct Cyc_Absyn_TunionType_struct*)_tmp324)->f1;_tmp326=(
void*)_tmp325.tunion_info;if(*((int*)_tmp326)!= 1)goto _LL285;_tmp327=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp326)->f1;_tmp328=*_tmp327;_tmp329=_tmp325.targs;_tmp32A=(void*)_tmp325.rgn;
_tmp32B=_tmp30B.f2;if(_tmp32B <= (void*)3?1:*((int*)_tmp32B)!= 2)goto _LL285;
_tmp32C=((struct Cyc_Absyn_TunionType_struct*)_tmp32B)->f1;_tmp32D=(void*)_tmp32C.tunion_info;
if(*((int*)_tmp32D)!= 1)goto _LL285;_tmp32E=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp32D)->f1;_tmp32F=*_tmp32E;_tmp330=_tmp32C.targs;_tmp331=(void*)_tmp32C.rgn;
_LL284: if(_tmp32F == _tmp328)return 0;{int _tmp394=Cyc_Absyn_qvar_cmp(_tmp32F->name,
_tmp328->name);if(_tmp394 != 0)return _tmp394;{int _tmp395=Cyc_Tcutil_typecmp(
_tmp331,_tmp32A);if(_tmp395 != 0)return _tmp395;return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp330,_tmp329);}}_LL285: _tmp332=_tmp30B.f1;if(_tmp332 <= (void*)3?1:*((int*)
_tmp332)!= 3)goto _LL287;_tmp333=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp332)->f1;_tmp334=(void*)_tmp333.field_info;if(*((int*)_tmp334)!= 1)goto
_LL287;_tmp335=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp334)->f1;_tmp336=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp334)->f2;_tmp337=_tmp333.targs;
_tmp338=_tmp30B.f2;if(_tmp338 <= (void*)3?1:*((int*)_tmp338)!= 3)goto _LL287;
_tmp339=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp338)->f1;_tmp33A=(void*)
_tmp339.field_info;if(*((int*)_tmp33A)!= 1)goto _LL287;_tmp33B=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp33A)->f1;_tmp33C=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp33A)->f2;
_tmp33D=_tmp339.targs;_LL286: if(_tmp33B == _tmp335)return 0;{int _tmp396=Cyc_Absyn_qvar_cmp(
_tmp335->name,_tmp33B->name);if(_tmp396 != 0)return _tmp396;{int _tmp397=Cyc_Absyn_qvar_cmp(
_tmp336->name,_tmp33C->name);if(_tmp397 != 0)return _tmp397;return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp33D,_tmp337);}}_LL287: _tmp33E=_tmp30B.f1;if(_tmp33E <= (
void*)3?1:*((int*)_tmp33E)!= 4)goto _LL289;_tmp33F=((struct Cyc_Absyn_PointerType_struct*)
_tmp33E)->f1;_tmp340=(void*)_tmp33F.elt_typ;_tmp341=(void*)_tmp33F.rgn_typ;
_tmp342=_tmp33F.nullable;_tmp343=_tmp33F.tq;_tmp344=_tmp33F.bounds;_tmp345=
_tmp30B.f2;if(_tmp345 <= (void*)3?1:*((int*)_tmp345)!= 4)goto _LL289;_tmp346=((
struct Cyc_Absyn_PointerType_struct*)_tmp345)->f1;_tmp347=(void*)_tmp346.elt_typ;
_tmp348=(void*)_tmp346.rgn_typ;_tmp349=_tmp346.nullable;_tmp34A=_tmp346.tq;
_tmp34B=_tmp346.bounds;_LL288: {int _tmp398=Cyc_Tcutil_typecmp(_tmp347,_tmp340);
if(_tmp398 != 0)return _tmp398;{int _tmp399=Cyc_Tcutil_typecmp(_tmp348,_tmp341);if(
_tmp399 != 0)return _tmp399;{int _tmp39A=Cyc_Tcutil_tqual_cmp(_tmp34A,_tmp343);if(
_tmp39A != 0)return _tmp39A;{int _tmp39B=Cyc_Tcutil_conrefs_cmp(Cyc_Tcutil_boundscmp,
_tmp34B,_tmp344);if(_tmp39B != 0)return _tmp39B;{void*_tmp39C=(void*)(Cyc_Absyn_compress_conref(
_tmp34B))->v;void*_tmp39D;_LL2AC: if(_tmp39C <= (void*)1?1:*((int*)_tmp39C)!= 0)
goto _LL2AE;_tmp39D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp39C)->f1;if((
int)_tmp39D != 0)goto _LL2AE;_LL2AD: return 0;_LL2AE:;_LL2AF: goto _LL2AB;_LL2AB:;}
return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp349,_tmp342);}}}}_LL289: _tmp34C=
_tmp30B.f1;if(_tmp34C <= (void*)3?1:*((int*)_tmp34C)!= 5)goto _LL28B;_tmp34D=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp34C)->f1;_tmp34E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp34C)->f2;_tmp34F=_tmp30B.f2;if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 5)
goto _LL28B;_tmp350=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp34F)->f1;_tmp351=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp34F)->f2;_LL28A: if(_tmp350 != _tmp34D)
return Cyc_Core_intcmp((int)((unsigned int)_tmp350),(int)((unsigned int)_tmp34D));
if(_tmp351 != _tmp34E)return Cyc_Core_intcmp((int)((unsigned int)_tmp351),(int)((
unsigned int)_tmp34E));return 0;_LL28B: _tmp352=_tmp30B.f1;if((int)_tmp352 != 1)
goto _LL28D;_tmp353=_tmp30B.f2;if((int)_tmp353 != 1)goto _LL28D;_LL28C: return 0;
_LL28D: _tmp354=_tmp30B.f1;if(_tmp354 <= (void*)3?1:*((int*)_tmp354)!= 6)goto
_LL28F;_tmp355=((struct Cyc_Absyn_DoubleType_struct*)_tmp354)->f1;_tmp356=_tmp30B.f2;
if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 6)goto _LL28F;_tmp357=((struct Cyc_Absyn_DoubleType_struct*)
_tmp356)->f1;_LL28E: if(_tmp355 == _tmp357)return 0;else{if(_tmp355)return - 1;else{
return 1;}}_LL28F: _tmp358=_tmp30B.f1;if(_tmp358 <= (void*)3?1:*((int*)_tmp358)!= 7)
goto _LL291;_tmp359=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp358)->f1;
_tmp35A=((struct Cyc_Absyn_ArrayType_struct*)_tmp358)->f2;_tmp35B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp358)->f3;_tmp35C=_tmp30B.f2;if(_tmp35C <= (void*)3?1:*((int*)_tmp35C)!= 7)
goto _LL291;_tmp35D=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp35C)->f1;
_tmp35E=((struct Cyc_Absyn_ArrayType_struct*)_tmp35C)->f2;_tmp35F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp35C)->f3;_LL290: {int _tmp39E=Cyc_Tcutil_tqual_cmp(_tmp35E,_tmp35A);if(
_tmp39E != 0)return _tmp39E;{int _tmp39F=Cyc_Tcutil_typecmp(_tmp35D,_tmp359);if(
_tmp39F != 0)return _tmp39F;if(_tmp35B == _tmp35F)return 0;if(_tmp35B == 0?1: _tmp35F
== 0)({void*_tmp3A0[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("missing expression in array index",sizeof(unsigned char),34),_tag_arr(
_tmp3A0,sizeof(void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Tcutil_const_uint_exp_cmp,_tmp35B,_tmp35F);}}_LL291: _tmp360=_tmp30B.f1;if(
_tmp360 <= (void*)3?1:*((int*)_tmp360)!= 8)goto _LL293;_tmp361=((struct Cyc_Absyn_FnType_struct*)
_tmp360)->f1;_tmp362=_tmp361.tvars;_tmp363=_tmp361.effect;_tmp364=(void*)_tmp361.ret_typ;
_tmp365=_tmp361.args;_tmp366=_tmp361.c_varargs;_tmp367=_tmp361.cyc_varargs;
_tmp368=_tmp361.rgn_po;_tmp369=_tmp361.attributes;_tmp36A=_tmp30B.f2;if(_tmp36A
<= (void*)3?1:*((int*)_tmp36A)!= 8)goto _LL293;_tmp36B=((struct Cyc_Absyn_FnType_struct*)
_tmp36A)->f1;_tmp36C=_tmp36B.tvars;_tmp36D=_tmp36B.effect;_tmp36E=(void*)_tmp36B.ret_typ;
_tmp36F=_tmp36B.args;_tmp370=_tmp36B.c_varargs;_tmp371=_tmp36B.cyc_varargs;
_tmp372=_tmp36B.rgn_po;_tmp373=_tmp36B.attributes;_LL292:({void*_tmp3A1[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: function types not handled",
sizeof(unsigned char),36),_tag_arr(_tmp3A1,sizeof(void*),0));});_LL293: _tmp374=
_tmp30B.f1;if(_tmp374 <= (void*)3?1:*((int*)_tmp374)!= 9)goto _LL295;_tmp375=((
struct Cyc_Absyn_TupleType_struct*)_tmp374)->f1;_tmp376=_tmp30B.f2;if(_tmp376 <= (
void*)3?1:*((int*)_tmp376)!= 9)goto _LL295;_tmp377=((struct Cyc_Absyn_TupleType_struct*)
_tmp376)->f1;_LL294: return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp377,_tmp375);_LL295: _tmp378=_tmp30B.f1;if(_tmp378 <= (void*)3?1:*((int*)
_tmp378)!= 11)goto _LL297;_tmp379=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp378)->f1;_tmp37A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp378)->f2;_tmp37B=
_tmp30B.f2;if(_tmp37B <= (void*)3?1:*((int*)_tmp37B)!= 11)goto _LL297;_tmp37C=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp37B)->f1;_tmp37D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp37B)->f2;_LL296: if(_tmp37C != _tmp379){if(_tmp37C == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp37D,_tmp37A);_LL297: _tmp37E=_tmp30B.f1;if((int)_tmp37E != 2)goto _LL299;
_tmp37F=_tmp30B.f2;if((int)_tmp37F != 2)goto _LL299;_LL298: return 0;_LL299: _tmp380=
_tmp30B.f1;if(_tmp380 <= (void*)3?1:*((int*)_tmp380)!= 15)goto _LL29B;_tmp381=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp380)->f1;_tmp382=_tmp30B.f2;
if(_tmp382 <= (void*)3?1:*((int*)_tmp382)!= 15)goto _LL29B;_tmp383=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp382)->f1;_LL29A: return Cyc_Tcutil_typecmp(
_tmp381,_tmp383);_LL29B: _tmp384=_tmp30B.f1;if(_tmp384 <= (void*)3?1:*((int*)
_tmp384)!= 14)goto _LL29D;_tmp385=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp384)->f1;_tmp386=_tmp30B.f2;if(_tmp386 <= (void*)3?1:*((int*)_tmp386)!= 14)
goto _LL29D;_tmp387=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp386)->f1;
_LL29C: return Cyc_Tcutil_typecmp(_tmp385,_tmp387);_LL29D: _tmp388=_tmp30B.f1;if(
_tmp388 <= (void*)3?1:*((int*)_tmp388)!= 18)goto _LL29F;_LL29E: goto _LL2A0;_LL29F:
_tmp389=_tmp30B.f2;if(_tmp389 <= (void*)3?1:*((int*)_tmp389)!= 18)goto _LL2A1;
_LL2A0: goto _LL2A2;_LL2A1: _tmp38A=_tmp30B.f1;if(_tmp38A <= (void*)3?1:*((int*)
_tmp38A)!= 17)goto _LL2A3;_LL2A2: goto _LL2A4;_LL2A3: _tmp38B=_tmp30B.f1;if(_tmp38B
<= (void*)3?1:*((int*)_tmp38B)!= 19)goto _LL2A5;_LL2A4: goto _LL2A6;_LL2A5: _tmp38C=
_tmp30B.f2;if(_tmp38C <= (void*)3?1:*((int*)_tmp38C)!= 19)goto _LL2A7;_LL2A6: goto
_LL2A8;_LL2A7: _tmp38D=_tmp30B.f2;if(_tmp38D <= (void*)3?1:*((int*)_tmp38D)!= 17)
goto _LL2A9;_LL2A8:({void*_tmp3A2[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: effects not handled",sizeof(
unsigned char),29),_tag_arr(_tmp3A2,sizeof(void*),0));});_LL2A9:;_LL2AA:({void*
_tmp3A3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Unmatched case in typecmp",sizeof(unsigned char),26),_tag_arr(_tmp3A3,
sizeof(void*),0));});_LL276:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){void*
_tmp3A4=Cyc_Tcutil_compress(t);_LL2B1: if(_tmp3A4 <= (void*)3?1:*((int*)_tmp3A4)!= 
5)goto _LL2B3;_LL2B2: goto _LL2B4;_LL2B3: if((int)_tmp3A4 != 1)goto _LL2B5;_LL2B4: goto
_LL2B6;_LL2B5: if(_tmp3A4 <= (void*)3?1:*((int*)_tmp3A4)!= 6)goto _LL2B7;_LL2B6:
goto _LL2B8;_LL2B7: if(_tmp3A4 <= (void*)3?1:*((int*)_tmp3A4)!= 12)goto _LL2B9;
_LL2B8: goto _LL2BA;_LL2B9: if(_tmp3A4 <= (void*)3?1:*((int*)_tmp3A4)!= 13)goto
_LL2BB;_LL2BA: return 1;_LL2BB:;_LL2BC: return 0;_LL2B0:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple9 _tmp3A6=({struct _tuple9 _tmp3A5;_tmp3A5.f1=Cyc_Tcutil_compress(
t1);_tmp3A5.f2=Cyc_Tcutil_compress(t2);_tmp3A5;});void*_tmp3A7;int _tmp3A8;void*
_tmp3A9;int _tmp3AA;void*_tmp3AB;void*_tmp3AC;void*_tmp3AD;void*_tmp3AE;void*
_tmp3AF;void*_tmp3B0;void*_tmp3B1;void*_tmp3B2;void*_tmp3B3;void*_tmp3B4;void*
_tmp3B5;void*_tmp3B6;void*_tmp3B7;void*_tmp3B8;void*_tmp3B9;void*_tmp3BA;void*
_tmp3BB;void*_tmp3BC;void*_tmp3BD;void*_tmp3BE;void*_tmp3BF;void*_tmp3C0;void*
_tmp3C1;void*_tmp3C2;void*_tmp3C3;void*_tmp3C4;void*_tmp3C5;void*_tmp3C6;void*
_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*_tmp3CA;void*_tmp3CB;void*_tmp3CC;void*
_tmp3CD;void*_tmp3CE;void*_tmp3CF;_LL2BE: _tmp3A7=_tmp3A6.f1;if(_tmp3A7 <= (void*)
3?1:*((int*)_tmp3A7)!= 6)goto _LL2C0;_tmp3A8=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3A7)->f1;_tmp3A9=_tmp3A6.f2;if(_tmp3A9 <= (void*)3?1:*((int*)_tmp3A9)!= 6)
goto _LL2C0;_tmp3AA=((struct Cyc_Absyn_DoubleType_struct*)_tmp3A9)->f1;_LL2BF:
return !_tmp3AA?_tmp3A8: 0;_LL2C0: _tmp3AB=_tmp3A6.f1;if(_tmp3AB <= (void*)3?1:*((
int*)_tmp3AB)!= 6)goto _LL2C2;_tmp3AC=_tmp3A6.f2;if((int)_tmp3AC != 1)goto _LL2C2;
_LL2C1: goto _LL2C3;_LL2C2: _tmp3AD=_tmp3A6.f1;if(_tmp3AD <= (void*)3?1:*((int*)
_tmp3AD)!= 6)goto _LL2C4;_tmp3AE=_tmp3A6.f2;if(_tmp3AE <= (void*)3?1:*((int*)
_tmp3AE)!= 5)goto _LL2C4;_LL2C3: goto _LL2C5;_LL2C4: _tmp3AF=_tmp3A6.f1;if(_tmp3AF <= (
void*)3?1:*((int*)_tmp3AF)!= 6)goto _LL2C6;_tmp3B0=_tmp3A6.f2;if(_tmp3B0 <= (void*)
3?1:*((int*)_tmp3B0)!= 14)goto _LL2C6;_LL2C5: goto _LL2C7;_LL2C6: _tmp3B1=_tmp3A6.f1;
if((int)_tmp3B1 != 1)goto _LL2C8;_tmp3B2=_tmp3A6.f2;if(_tmp3B2 <= (void*)3?1:*((int*)
_tmp3B2)!= 14)goto _LL2C8;_LL2C7: goto _LL2C9;_LL2C8: _tmp3B3=_tmp3A6.f1;if((int)
_tmp3B3 != 1)goto _LL2CA;_tmp3B4=_tmp3A6.f2;if(_tmp3B4 <= (void*)3?1:*((int*)
_tmp3B4)!= 5)goto _LL2CA;_LL2C9: return 1;_LL2CA: _tmp3B5=_tmp3A6.f1;if(_tmp3B5 <= (
void*)3?1:*((int*)_tmp3B5)!= 5)goto _LL2CC;_tmp3B6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3B5)->f2;if((int)_tmp3B6 != 3)goto _LL2CC;_tmp3B7=_tmp3A6.f2;if(_tmp3B7 <= (
void*)3?1:*((int*)_tmp3B7)!= 5)goto _LL2CC;_tmp3B8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3B7)->f2;if((int)_tmp3B8 != 3)goto _LL2CC;_LL2CB: return 0;_LL2CC: _tmp3B9=
_tmp3A6.f1;if(_tmp3B9 <= (void*)3?1:*((int*)_tmp3B9)!= 5)goto _LL2CE;_tmp3BA=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3B9)->f2;if((int)_tmp3BA != 3)goto _LL2CE;
_LL2CD: goto _LL2CF;_LL2CE: _tmp3BB=_tmp3A6.f1;if(_tmp3BB <= (void*)3?1:*((int*)
_tmp3BB)!= 5)goto _LL2D0;_tmp3BC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3BB)->f2;
if((int)_tmp3BC != 2)goto _LL2D0;_tmp3BD=_tmp3A6.f2;if((int)_tmp3BD != 1)goto _LL2D0;
_LL2CF: goto _LL2D1;_LL2D0: _tmp3BE=_tmp3A6.f1;if(_tmp3BE <= (void*)3?1:*((int*)
_tmp3BE)!= 5)goto _LL2D2;_tmp3BF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3BE)->f2;
if((int)_tmp3BF != 2)goto _LL2D2;_tmp3C0=_tmp3A6.f2;if(_tmp3C0 <= (void*)3?1:*((int*)
_tmp3C0)!= 5)goto _LL2D2;_tmp3C1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C0)->f2;
if((int)_tmp3C1 != 1)goto _LL2D2;_LL2D1: goto _LL2D3;_LL2D2: _tmp3C2=_tmp3A6.f1;if(
_tmp3C2 <= (void*)3?1:*((int*)_tmp3C2)!= 5)goto _LL2D4;_tmp3C3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C2)->f2;if((int)_tmp3C3 != 2)goto _LL2D4;_tmp3C4=_tmp3A6.f2;if(_tmp3C4 <= (
void*)3?1:*((int*)_tmp3C4)!= 5)goto _LL2D4;_tmp3C5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C4)->f2;if((int)_tmp3C5 != 0)goto _LL2D4;_LL2D3: goto _LL2D5;_LL2D4: _tmp3C6=
_tmp3A6.f1;if(_tmp3C6 <= (void*)3?1:*((int*)_tmp3C6)!= 5)goto _LL2D6;_tmp3C7=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3C6)->f2;if((int)_tmp3C7 != 1)goto _LL2D6;
_tmp3C8=_tmp3A6.f2;if(_tmp3C8 <= (void*)3?1:*((int*)_tmp3C8)!= 5)goto _LL2D6;
_tmp3C9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C8)->f2;if((int)_tmp3C9 != 
0)goto _LL2D6;_LL2D5: goto _LL2D7;_LL2D6: _tmp3CA=_tmp3A6.f1;if(_tmp3CA <= (void*)3?1:*((
int*)_tmp3CA)!= 14)goto _LL2D8;_tmp3CB=_tmp3A6.f2;if(_tmp3CB <= (void*)3?1:*((int*)
_tmp3CB)!= 5)goto _LL2D8;_tmp3CC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3CB)->f2;
if((int)_tmp3CC != 1)goto _LL2D8;_LL2D7: goto _LL2D9;_LL2D8: _tmp3CD=_tmp3A6.f1;if(
_tmp3CD <= (void*)3?1:*((int*)_tmp3CD)!= 14)goto _LL2DA;_tmp3CE=_tmp3A6.f2;if(
_tmp3CE <= (void*)3?1:*((int*)_tmp3CE)!= 5)goto _LL2DA;_tmp3CF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3CE)->f2;if((int)_tmp3CF != 0)goto _LL2DA;_LL2D9: return 1;_LL2DA:;_LL2DB: return
0;_LL2BD:;}int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*
es){struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;
el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0?1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->v=(
void*)t1;_tmp3D0;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)((
struct Cyc_Core_Opt*)_check_null(max_arith_type))->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_Std_String_pa_struct _tmp3D3;_tmp3D3.tag=0;_tmp3D3.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp3D2;_tmp3D2.tag=0;_tmp3D2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp3D1[2]={& _tmp3D2,& _tmp3D3};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,
_tag_arr("type mismatch: expecting %s but found %s",sizeof(unsigned char),41),
_tag_arr(_tmp3D1,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp3D4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL2DD: if(_tmp3D4 <= (void*)3?1:*((int*)_tmp3D4)!= 4)goto _LL2DF;_LL2DE: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_t);goto _LL2DC;_LL2DF:;_LL2E0: return 0;_LL2DC:;}return 1;}int
Cyc_Tcutil_is_integral_type(void*t){void*_tmp3D5=Cyc_Tcutil_compress(t);_LL2E2:
if(_tmp3D5 <= (void*)3?1:*((int*)_tmp3D5)!= 5)goto _LL2E4;_LL2E3: goto _LL2E5;_LL2E4:
if(_tmp3D5 <= (void*)3?1:*((int*)_tmp3D5)!= 14)goto _LL2E6;_LL2E5: goto _LL2E7;
_LL2E6: if(_tmp3D5 <= (void*)3?1:*((int*)_tmp3D5)!= 12)goto _LL2E8;_LL2E7: goto
_LL2E9;_LL2E8: if(_tmp3D5 <= (void*)3?1:*((int*)_tmp3D5)!= 13)goto _LL2EA;_LL2E9:
return 1;_LL2EA:;_LL2EB: return 0;_LL2E1:;}int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_t))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_t))({void*
_tmp3D6[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(unsigned char),44),_tag_arr(_tmp3D6,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_t);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_t))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_t))({void*
_tmp3D7[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(unsigned char),44),_tag_arr(_tmp3D7,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_t);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple9 _tmp3D9=({struct _tuple9 _tmp3D8;_tmp3D8.f1=
t1;_tmp3D8.f2=t2;_tmp3D8;});void*_tmp3DA;struct Cyc_Absyn_PtrInfo _tmp3DB;void*
_tmp3DC;struct Cyc_Absyn_PtrInfo _tmp3DD;void*_tmp3DE;void*_tmp3DF;struct Cyc_Absyn_Tqual
_tmp3E0;struct Cyc_Absyn_Exp*_tmp3E1;void*_tmp3E2;void*_tmp3E3;struct Cyc_Absyn_Tqual
_tmp3E4;struct Cyc_Absyn_Exp*_tmp3E5;void*_tmp3E6;struct Cyc_Absyn_TunionFieldInfo
_tmp3E7;void*_tmp3E8;struct Cyc_Absyn_Tuniondecl*_tmp3E9;struct Cyc_Absyn_Tunionfield*
_tmp3EA;struct Cyc_List_List*_tmp3EB;void*_tmp3EC;struct Cyc_Absyn_TunionInfo
_tmp3ED;void*_tmp3EE;struct Cyc_Absyn_Tuniondecl**_tmp3EF;struct Cyc_Absyn_Tuniondecl*
_tmp3F0;struct Cyc_List_List*_tmp3F1;void*_tmp3F2;struct Cyc_Absyn_PtrInfo _tmp3F3;
void*_tmp3F4;void*_tmp3F5;struct Cyc_Absyn_Conref*_tmp3F6;struct Cyc_Absyn_Tqual
_tmp3F7;struct Cyc_Absyn_Conref*_tmp3F8;void*_tmp3F9;struct Cyc_Absyn_TunionInfo
_tmp3FA;void*_tmp3FB;struct Cyc_Absyn_Tuniondecl**_tmp3FC;struct Cyc_Absyn_Tuniondecl*
_tmp3FD;struct Cyc_List_List*_tmp3FE;void*_tmp3FF;void*_tmp400;void*_tmp401;void*
_tmp402;_LL2ED: _tmp3DA=_tmp3D9.f1;if(_tmp3DA <= (void*)3?1:*((int*)_tmp3DA)!= 4)
goto _LL2EF;_tmp3DB=((struct Cyc_Absyn_PointerType_struct*)_tmp3DA)->f1;_tmp3DC=
_tmp3D9.f2;if(_tmp3DC <= (void*)3?1:*((int*)_tmp3DC)!= 4)goto _LL2EF;_tmp3DD=((
struct Cyc_Absyn_PointerType_struct*)_tmp3DC)->f1;_LL2EE: {int okay=1;if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp3DB.nullable,_tmp3DD.nullable)){void*_tmp403=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp3DB.nullable))->v;int _tmp404;_LL2FA: if(_tmp403 <= (void*)1?1:*((int*)_tmp403)
!= 0)goto _LL2FC;_tmp404=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp403)->f1;
_LL2FB: okay=!_tmp404;goto _LL2F9;_LL2FC:;_LL2FD:({void*_tmp405[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("silent_castable conref not eq",
sizeof(unsigned char),30),_tag_arr(_tmp405,sizeof(void*),0));});_LL2F9:;}if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp3DB.bounds,_tmp3DD.bounds)){struct _tuple9 _tmp407=({
struct _tuple9 _tmp406;_tmp406.f1=(void*)(Cyc_Absyn_compress_conref(_tmp3DB.bounds))->v;
_tmp406.f2=(void*)(Cyc_Absyn_compress_conref(_tmp3DD.bounds))->v;_tmp406;});void*
_tmp408;void*_tmp409;void*_tmp40A;void*_tmp40B;void*_tmp40C;void*_tmp40D;struct
Cyc_Absyn_Exp*_tmp40E;void*_tmp40F;void*_tmp410;struct Cyc_Absyn_Exp*_tmp411;void*
_tmp412;void*_tmp413;void*_tmp414;void*_tmp415;_LL2FF: _tmp408=_tmp407.f1;if(
_tmp408 <= (void*)1?1:*((int*)_tmp408)!= 0)goto _LL301;_tmp409=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp408)->f1;if(_tmp409 <= (void*)1?1:*((int*)_tmp409)!= 0)goto _LL301;_tmp40A=
_tmp407.f2;if(_tmp40A <= (void*)1?1:*((int*)_tmp40A)!= 0)goto _LL301;_tmp40B=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp40A)->f1;if((int)_tmp40B != 0)goto _LL301;
_LL300: okay=1;goto _LL2FE;_LL301: _tmp40C=_tmp407.f1;if(_tmp40C <= (void*)1?1:*((
int*)_tmp40C)!= 0)goto _LL303;_tmp40D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp40C)->f1;if(_tmp40D <= (void*)1?1:*((int*)_tmp40D)!= 0)goto _LL303;_tmp40E=((
struct Cyc_Absyn_Upper_b_struct*)_tmp40D)->f1;_tmp40F=_tmp407.f2;if(_tmp40F <= (
void*)1?1:*((int*)_tmp40F)!= 0)goto _LL303;_tmp410=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp40F)->f1;if(_tmp410 <= (void*)1?1:*((int*)_tmp410)!= 0)goto _LL303;_tmp411=((
struct Cyc_Absyn_Upper_b_struct*)_tmp410)->f1;_LL302: okay=okay?Cyc_Evexp_eval_const_uint_exp(
_tmp40E)>= Cyc_Evexp_eval_const_uint_exp(_tmp411): 0;({void*_tmp416[0]={};Cyc_Tcutil_warn(
loc,_tag_arr("implicit cast to shorter array",sizeof(unsigned char),31),_tag_arr(
_tmp416,sizeof(void*),0));});goto _LL2FE;_LL303: _tmp412=_tmp407.f1;if(_tmp412 <= (
void*)1?1:*((int*)_tmp412)!= 0)goto _LL305;_tmp413=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp412)->f1;if((int)_tmp413 != 0)goto _LL305;_tmp414=_tmp407.f2;if(_tmp414 <= (
void*)1?1:*((int*)_tmp414)!= 0)goto _LL305;_tmp415=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp414)->f1;if((int)_tmp415 != 0)goto _LL305;_LL304: okay=1;goto _LL2FE;_LL305:;
_LL306: okay=0;goto _LL2FE;_LL2FE:;}okay=okay?Cyc_Tcutil_unify((void*)_tmp3DB.elt_typ,(
void*)_tmp3DD.elt_typ): 0;okay=okay?Cyc_Tcutil_unify((void*)_tmp3DB.rgn_typ,(void*)
_tmp3DD.rgn_typ)?1: Cyc_Tcenv_region_outlives(te,(void*)_tmp3DB.rgn_typ,(void*)
_tmp3DD.rgn_typ): 0;okay=okay?!(_tmp3DB.tq).q_const?1:(_tmp3DD.tq).q_const: 0;
return okay;}_LL2EF: _tmp3DE=_tmp3D9.f1;if(_tmp3DE <= (void*)3?1:*((int*)_tmp3DE)!= 
7)goto _LL2F1;_tmp3DF=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3DE)->f1;
_tmp3E0=((struct Cyc_Absyn_ArrayType_struct*)_tmp3DE)->f2;_tmp3E1=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3DE)->f3;_tmp3E2=_tmp3D9.f2;if(_tmp3E2 <= (void*)3?1:*((int*)_tmp3E2)!= 7)
goto _LL2F1;_tmp3E3=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3E2)->f1;
_tmp3E4=((struct Cyc_Absyn_ArrayType_struct*)_tmp3E2)->f2;_tmp3E5=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E2)->f3;_LL2F0: {int okay;okay=(_tmp3E1 != 0?_tmp3E5 != 0: 0)?Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp3E1))== Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp3E5)): 0;return(okay?Cyc_Tcutil_unify(
_tmp3DF,_tmp3E3): 0)?!_tmp3E0.q_const?1: _tmp3E4.q_const: 0;}_LL2F1: _tmp3E6=_tmp3D9.f1;
if(_tmp3E6 <= (void*)3?1:*((int*)_tmp3E6)!= 3)goto _LL2F3;_tmp3E7=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3E6)->f1;_tmp3E8=(void*)_tmp3E7.field_info;if(*((int*)_tmp3E8)!= 1)goto
_LL2F3;_tmp3E9=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3E8)->f1;_tmp3EA=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3E8)->f2;_tmp3EB=_tmp3E7.targs;
_tmp3EC=_tmp3D9.f2;if(_tmp3EC <= (void*)3?1:*((int*)_tmp3EC)!= 2)goto _LL2F3;
_tmp3ED=((struct Cyc_Absyn_TunionType_struct*)_tmp3EC)->f1;_tmp3EE=(void*)_tmp3ED.tunion_info;
if(*((int*)_tmp3EE)!= 1)goto _LL2F3;_tmp3EF=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3EE)->f1;_tmp3F0=*_tmp3EF;_tmp3F1=_tmp3ED.targs;_LL2F2: if((_tmp3E9 == _tmp3F0?
1: Cyc_Absyn_qvar_cmp(_tmp3E9->name,_tmp3F0->name)== 0)?_tmp3EA->typs == 0: 0){for(
0;_tmp3EB != 0?_tmp3F1 != 0: 0;(_tmp3EB=_tmp3EB->tl,_tmp3F1=_tmp3F1->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp3EB->hd,(void*)_tmp3F1->hd))break;}if(_tmp3EB == 0?_tmp3F1 == 0: 0)return
1;}return 0;_LL2F3: _tmp3F2=_tmp3D9.f1;if(_tmp3F2 <= (void*)3?1:*((int*)_tmp3F2)!= 
4)goto _LL2F5;_tmp3F3=((struct Cyc_Absyn_PointerType_struct*)_tmp3F2)->f1;_tmp3F4=(
void*)_tmp3F3.elt_typ;_tmp3F5=(void*)_tmp3F3.rgn_typ;_tmp3F6=_tmp3F3.nullable;
_tmp3F7=_tmp3F3.tq;_tmp3F8=_tmp3F3.bounds;_tmp3F9=_tmp3D9.f2;if(_tmp3F9 <= (void*)
3?1:*((int*)_tmp3F9)!= 2)goto _LL2F5;_tmp3FA=((struct Cyc_Absyn_TunionType_struct*)
_tmp3F9)->f1;_tmp3FB=(void*)_tmp3FA.tunion_info;if(*((int*)_tmp3FB)!= 1)goto
_LL2F5;_tmp3FC=((struct Cyc_Absyn_KnownTunion_struct*)_tmp3FB)->f1;_tmp3FD=*
_tmp3FC;_tmp3FE=_tmp3FA.targs;_tmp3FF=(void*)_tmp3FA.rgn;_LL2F4:{void*_tmp417=
Cyc_Tcutil_compress(_tmp3F4);struct Cyc_Absyn_TunionFieldInfo _tmp418;void*_tmp419;
struct Cyc_Absyn_Tuniondecl*_tmp41A;struct Cyc_Absyn_Tunionfield*_tmp41B;struct Cyc_List_List*
_tmp41C;_LL308: if(_tmp417 <= (void*)3?1:*((int*)_tmp417)!= 3)goto _LL30A;_tmp418=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp417)->f1;_tmp419=(void*)_tmp418.field_info;
if(*((int*)_tmp419)!= 1)goto _LL30A;_tmp41A=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp419)->f1;_tmp41B=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp419)->f2;
_tmp41C=_tmp418.targs;_LL309: if(!Cyc_Tcutil_unify(_tmp3F5,_tmp3FF)?!Cyc_Tcenv_region_outlives(
te,_tmp3F5,_tmp3FF): 0)return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp3F6,((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(0)))return 0;if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp3F8,Cyc_Absyn_new_conref(Cyc_Absyn_bounds_one)))return 0;
if(Cyc_Absyn_qvar_cmp(_tmp3FD->name,_tmp41A->name)== 0?_tmp41B->typs != 0: 0){int
okay=1;for(0;_tmp41C != 0?_tmp3FE != 0: 0;(_tmp41C=_tmp41C->tl,_tmp3FE=_tmp3FE->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp41C->hd,(void*)_tmp3FE->hd)){okay=0;break;}}if((
!okay?1: _tmp41C != 0)?1: _tmp3FE != 0)return 0;return 1;}goto _LL307;_LL30A:;_LL30B:
goto _LL307;_LL307:;}return 0;_LL2F5: _tmp400=_tmp3D9.f1;if(_tmp400 <= (void*)3?1:*((
int*)_tmp400)!= 14)goto _LL2F7;_tmp401=_tmp3D9.f2;if(_tmp401 <= (void*)3?1:*((int*)
_tmp401)!= 5)goto _LL2F7;_tmp402=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp401)->f2;
if((int)_tmp402 != 2)goto _LL2F7;_LL2F6: return 1;_LL2F7:;_LL2F8: return Cyc_Tcutil_unify(
t1,t2);_LL2EC:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp41D=Cyc_Tcutil_compress(
t);_LL30D: if(_tmp41D <= (void*)3?1:*((int*)_tmp41D)!= 4)goto _LL30F;_LL30E: return 1;
_LL30F:;_LL310: return 0;_LL30C:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp41E=(void*)e->r;void*_tmp41F;int _tmp420;_LL312: if(*((int*)_tmp41E)!= 0)
goto _LL314;_tmp41F=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp41E)->f1;if(
_tmp41F <= (void*)1?1:*((int*)_tmp41F)!= 2)goto _LL314;_tmp420=((struct Cyc_Absyn_Int_c_struct*)
_tmp41F)->f2;if(_tmp420 != 0)goto _LL314;_LL313: return 1;_LL314:;_LL315: return 0;
_LL311:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt
Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)
2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2)?Cyc_Tcutil_is_zero(e1): 0){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421[0]=({struct Cyc_Absyn_Const_e_struct
_tmp422;_tmp422.tag=0;_tmp422.f1=(void*)((void*)0);_tmp422;});_tmp421;})));{
struct Cyc_List_List*_tmp423=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp424=({struct Cyc_Absyn_PointerType_struct*_tmp425=_cycalloc(sizeof(*_tmp425));
_tmp425[0]=({struct Cyc_Absyn_PointerType_struct _tmp426;_tmp426.tag=4;_tmp426.f1=({
struct Cyc_Absyn_PtrInfo _tmp427;_tmp427.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp429=_cycalloc(sizeof(*
_tmp429));_tmp429->v=_tmp423;_tmp429;}));_tmp427.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp428=_cycalloc(
sizeof(*_tmp428));_tmp428->v=_tmp423;_tmp428;}));_tmp427.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmp427.tq=Cyc_Absyn_empty_tqual();_tmp427.bounds=
Cyc_Absyn_empty_conref();_tmp427;});_tmp426;});_tmp425;});(void*)(((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v=(void*)((void*)_tmp424));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}static int Cyc_Tcutil_is_sizeof_type(void*t1){void*_tmp42A=
Cyc_Tcutil_compress(t1);_LL317: if(_tmp42A <= (void*)3?1:*((int*)_tmp42A)!= 14)
goto _LL319;_LL318: return 1;_LL319:;_LL31A: return 0;_LL316:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);if(Cyc_Tcutil_unify(t1,t2))
return 1;if((Cyc_Tcutil_is_arithmetic_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2):
0)?1:(Cyc_Tcutil_is_sizeof_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0)){if(Cyc_Tcutil_will_lose_precision(
t1,t2))({struct Cyc_Std_String_pa_struct _tmp42D;_tmp42D.tag=0;_tmp42D.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmp42C;
_tmp42C.tag=0;_tmp42C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmp42B[2]={& _tmp42C,& _tmp42D};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; %s -> %s conversion supplied",
sizeof(unsigned char),53),_tag_arr(_tmp42B,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if(Cyc_Tcutil_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);({struct Cyc_Std_String_pa_struct
_tmp430;_tmp430.tag=0;_tmp430.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp42F;_tmp42F.tag=0;_tmp42F.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp42E[2]={& _tmp42F,& _tmp430};Cyc_Tcutil_warn(
e->loc,_tag_arr("implicit cast from %s to %s",sizeof(unsigned char),28),_tag_arr(
_tmp42E,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp431=Cyc_Tcutil_compress(t);
_LL31C: if(_tmp431 <= (void*)3?1:*((int*)_tmp431)!= 5)goto _LL31E;_LL31D: goto _LL31F;
_LL31E: if((int)_tmp431 != 1)goto _LL320;_LL31F: goto _LL321;_LL320: if(_tmp431 <= (
void*)3?1:*((int*)_tmp431)!= 6)goto _LL322;_LL321: return 1;_LL322:;_LL323: return 0;
_LL31B:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*inst,
struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp432=_cycalloc(sizeof(*
_tmp432));_tmp432->f1=x->tq;_tmp432->f2=Cyc_Tcutil_substitute(inst,(void*)x->type);
_tmp432;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp433=t1;struct Cyc_List_List*
_tmp434;struct Cyc_Absyn_AggrInfo _tmp435;void*_tmp436;struct Cyc_Absyn_Aggrdecl**
_tmp437;struct Cyc_Absyn_Aggrdecl*_tmp438;struct Cyc_List_List*_tmp439;void*
_tmp43A;struct Cyc_List_List*_tmp43B;_LL325: if((int)_tmp433 != 0)goto _LL327;_LL326:
return 0;_LL327: if(_tmp433 <= (void*)3?1:*((int*)_tmp433)!= 9)goto _LL329;_tmp434=((
struct Cyc_Absyn_TupleType_struct*)_tmp433)->f1;_LL328: return _tmp434;_LL329: if(
_tmp433 <= (void*)3?1:*((int*)_tmp433)!= 10)goto _LL32B;_tmp435=((struct Cyc_Absyn_AggrType_struct*)
_tmp433)->f1;_tmp436=(void*)_tmp435.aggr_info;if(*((int*)_tmp436)!= 1)goto _LL32B;
_tmp437=((struct Cyc_Absyn_KnownAggr_struct*)_tmp436)->f1;_tmp438=*_tmp437;
_tmp439=_tmp435.targs;_LL32A: if((void*)_tmp438->kind == (void*)1?1: _tmp438->fields
== 0)return({struct Cyc_List_List*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->hd=({
struct _tuple4*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->f1=Cyc_Absyn_empty_tqual();
_tmp43D->f2=t1;_tmp43D;});_tmp43C->tl=0;_tmp43C;});{struct Cyc_List_List*_tmp43E=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp438->tvs,_tmp439);return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp43E,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp438->fields))->v);}_LL32B: if(_tmp433 <= (void*)
3?1:*((int*)_tmp433)!= 11)goto _LL32D;_tmp43A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp433)->f1;if((int)_tmp43A != 0)goto _LL32D;_tmp43B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp433)->f2;_LL32C: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp43B);_LL32D:;_LL32E: return({struct Cyc_List_List*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->hd=({struct _tuple4*_tmp440=
_cycalloc(sizeof(*_tmp440));_tmp440->f1=Cyc_Absyn_empty_tqual();_tmp440->f2=t1;
_tmp440;});_tmp43F->tl=0;_tmp43F;});_LL324:;}}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple9*)a->hd)).f1)?Cyc_Tcutil_unify(t2,(*((struct _tuple9*)a->hd)).f2):
0)return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple9
_tmp442=({struct _tuple9 _tmp441;_tmp441.f1=t1;_tmp441.f2=t2;_tmp441;});void*
_tmp443;struct Cyc_Absyn_PtrInfo _tmp444;void*_tmp445;void*_tmp446;struct Cyc_Absyn_Conref*
_tmp447;struct Cyc_Absyn_Tqual _tmp448;struct Cyc_Absyn_Conref*_tmp449;void*_tmp44A;
struct Cyc_Absyn_PtrInfo _tmp44B;void*_tmp44C;void*_tmp44D;struct Cyc_Absyn_Conref*
_tmp44E;struct Cyc_Absyn_Tqual _tmp44F;struct Cyc_Absyn_Conref*_tmp450;_LL330:
_tmp443=_tmp442.f1;if(_tmp443 <= (void*)3?1:*((int*)_tmp443)!= 4)goto _LL332;
_tmp444=((struct Cyc_Absyn_PointerType_struct*)_tmp443)->f1;_tmp445=(void*)
_tmp444.elt_typ;_tmp446=(void*)_tmp444.rgn_typ;_tmp447=_tmp444.nullable;_tmp448=
_tmp444.tq;_tmp449=_tmp444.bounds;_tmp44A=_tmp442.f2;if(_tmp44A <= (void*)3?1:*((
int*)_tmp44A)!= 4)goto _LL332;_tmp44B=((struct Cyc_Absyn_PointerType_struct*)
_tmp44A)->f1;_tmp44C=(void*)_tmp44B.elt_typ;_tmp44D=(void*)_tmp44B.rgn_typ;
_tmp44E=_tmp44B.nullable;_tmp44F=_tmp44B.tq;_tmp450=_tmp44B.bounds;_LL331: if(
_tmp448.q_const?!_tmp44F.q_const: 0)return 0;if((!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp447,_tmp44E)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp447): 0)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp44E): 0)
return 0;if(!Cyc_Tcutil_unify(_tmp446,_tmp44D)?!Cyc_Tcenv_region_outlives(te,
_tmp446,_tmp44D): 0)return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp449,_tmp450)){struct _tuple9 _tmp452=({struct _tuple9 _tmp451;_tmp451.f1=Cyc_Absyn_conref_val(
_tmp449);_tmp451.f2=Cyc_Absyn_conref_val(_tmp450);_tmp451;});void*_tmp453;void*
_tmp454;void*_tmp455;struct Cyc_Absyn_Exp*_tmp456;void*_tmp457;struct Cyc_Absyn_Exp*
_tmp458;_LL335: _tmp453=_tmp452.f1;if(_tmp453 <= (void*)1?1:*((int*)_tmp453)!= 0)
goto _LL337;_tmp454=_tmp452.f2;if((int)_tmp454 != 0)goto _LL337;_LL336: goto _LL334;
_LL337: _tmp455=_tmp452.f1;if(_tmp455 <= (void*)1?1:*((int*)_tmp455)!= 0)goto
_LL339;_tmp456=((struct Cyc_Absyn_Upper_b_struct*)_tmp455)->f1;_tmp457=_tmp452.f2;
if(_tmp457 <= (void*)1?1:*((int*)_tmp457)!= 0)goto _LL339;_tmp458=((struct Cyc_Absyn_Upper_b_struct*)
_tmp457)->f1;_LL338: if(Cyc_Evexp_eval_const_uint_exp(_tmp456)< Cyc_Evexp_eval_const_uint_exp(
_tmp458))return 0;goto _LL334;_LL339:;_LL33A: return 0;_LL334:;}return Cyc_Tcutil_ptrsubtype(
te,({struct Cyc_List_List*_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459->hd=({
struct _tuple9*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->f1=t1;_tmp45A->f2=t2;
_tmp45A;});_tmp459->tl=assume;_tmp459;}),_tmp445,_tmp44C);_LL332:;_LL333: return 0;
_LL32F:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple9 _tmp45C=({
struct _tuple9 _tmp45B;_tmp45B.f1=Cyc_Tcutil_compress(t1);_tmp45B.f2=Cyc_Tcutil_compress(
t2);_tmp45B;});void*_tmp45D;void*_tmp45E;void*_tmp45F;void*_tmp460;_LL33C:
_tmp45D=_tmp45C.f1;if(_tmp45D <= (void*)3?1:*((int*)_tmp45D)!= 5)goto _LL33E;
_tmp45E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp45D)->f2;_tmp45F=_tmp45C.f2;
if(_tmp45F <= (void*)3?1:*((int*)_tmp45F)!= 5)goto _LL33E;_tmp460=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp45F)->f2;_LL33D: return _tmp45E == _tmp460;_LL33E:;
_LL33F: return 0;_LL33B:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp462;struct
Cyc_Absyn_Tqual _tmp463;void*_tmp464;struct _tuple4*_tmp461=(struct _tuple4*)tqs1->hd;
_tmp462=*_tmp461;_tmp463=_tmp462.f1;_tmp464=_tmp462.f2;{struct _tuple4 _tmp466;
struct Cyc_Absyn_Tqual _tmp467;void*_tmp468;struct _tuple4*_tmp465=(struct _tuple4*)
tqs2->hd;_tmp466=*_tmp465;_tmp467=_tmp466.f1;_tmp468=_tmp466.f2;if(_tmp467.q_const?
Cyc_Tcutil_subtype(te,assume,_tmp464,_tmp468): 0)continue;else{if(Cyc_Tcutil_unify(
_tmp464,_tmp468))continue;else{if(Cyc_Tcutil_isomorphic(_tmp464,_tmp468))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp469=Cyc_Tcutil_compress(t);void*_tmp46A;_LL341: if(_tmp469 <= (void*)3?1:*((
int*)_tmp469)!= 5)goto _LL343;_tmp46A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp469)->f2;if((int)_tmp46A != 0)goto _LL343;_LL342: return 1;_LL343:;_LL344: return
0;_LL340:;}int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{void*_tmp46B=t2;void*_tmp46C;_LL346: if(_tmp46B <= (
void*)3?1:*((int*)_tmp46B)!= 5)goto _LL348;_tmp46C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46B)->f2;if((int)_tmp46C != 2)goto _LL348;_LL347: if(Cyc_Tcutil_typ_kind(t2)== (
void*)2)return 1;goto _LL345;_LL348:;_LL349: goto _LL345;_LL345:;}{void*_tmp46D=t1;
struct Cyc_Absyn_PtrInfo _tmp46E;void*_tmp46F;void*_tmp470;struct Cyc_Absyn_Conref*
_tmp471;struct Cyc_Absyn_Tqual _tmp472;struct Cyc_Absyn_Conref*_tmp473;void*_tmp474;
struct Cyc_Absyn_Tqual _tmp475;struct Cyc_Absyn_Exp*_tmp476;struct Cyc_Absyn_Enumdecl*
_tmp477;_LL34B: if(_tmp46D <= (void*)3?1:*((int*)_tmp46D)!= 4)goto _LL34D;_tmp46E=((
struct Cyc_Absyn_PointerType_struct*)_tmp46D)->f1;_tmp46F=(void*)_tmp46E.elt_typ;
_tmp470=(void*)_tmp46E.rgn_typ;_tmp471=_tmp46E.nullable;_tmp472=_tmp46E.tq;
_tmp473=_tmp46E.bounds;_LL34C:{void*_tmp478=t2;struct Cyc_Absyn_PtrInfo _tmp479;
void*_tmp47A;void*_tmp47B;struct Cyc_Absyn_Conref*_tmp47C;struct Cyc_Absyn_Tqual
_tmp47D;struct Cyc_Absyn_Conref*_tmp47E;_LL35C: if(_tmp478 <= (void*)3?1:*((int*)
_tmp478)!= 4)goto _LL35E;_tmp479=((struct Cyc_Absyn_PointerType_struct*)_tmp478)->f1;
_tmp47A=(void*)_tmp479.elt_typ;_tmp47B=(void*)_tmp479.rgn_typ;_tmp47C=_tmp479.nullable;
_tmp47D=_tmp479.tq;_tmp47E=_tmp479.bounds;_LL35D: {struct Cyc_List_List*_tmp47F=({
struct Cyc_List_List*_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488->hd=({struct
_tuple9*_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->f1=t1;_tmp489->f2=t2;
_tmp489;});_tmp488->tl=0;_tmp488;});int _tmp480=Cyc_Tcutil_ptrsubtype(te,_tmp47F,
_tmp46F,_tmp47A)?!_tmp472.q_const?1: _tmp47D.q_const: 0;int _tmp481=_tmp480?0:((Cyc_Tcutil_bits_only(
_tmp46F)?Cyc_Tcutil_is_char_type(_tmp47A): 0)?_tmp47D.q_const?1: !_tmp472.q_const:
0);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp473,_tmp47E);
if(!bounds_ok?!_tmp481: 0){struct _tuple9 _tmp483=({struct _tuple9 _tmp482;_tmp482.f1=
Cyc_Absyn_conref_val(_tmp473);_tmp482.f2=Cyc_Absyn_conref_val(_tmp47E);_tmp482;});
void*_tmp484;struct Cyc_Absyn_Exp*_tmp485;void*_tmp486;struct Cyc_Absyn_Exp*
_tmp487;_LL363: _tmp484=_tmp483.f1;if(_tmp484 <= (void*)1?1:*((int*)_tmp484)!= 0)
goto _LL365;_tmp485=((struct Cyc_Absyn_Upper_b_struct*)_tmp484)->f1;_tmp486=
_tmp483.f2;if(_tmp486 <= (void*)1?1:*((int*)_tmp486)!= 0)goto _LL365;_tmp487=((
struct Cyc_Absyn_Upper_b_struct*)_tmp486)->f1;_LL364: if(Cyc_Evexp_eval_const_uint_exp(
_tmp485)>= Cyc_Evexp_eval_const_uint_exp(_tmp487))bounds_ok=1;goto _LL362;_LL365:;
_LL366: bounds_ok=1;goto _LL362;_LL362:;}return(bounds_ok?_tmp480?1: _tmp481: 0)?Cyc_Tcutil_unify(
_tmp470,_tmp47B)?1: Cyc_Tcenv_region_outlives(te,_tmp470,_tmp47B): 0;}_LL35E: if((
int)_tmp478 != 0)goto _LL360;_LL35F: return 1;_LL360:;_LL361: goto _LL35B;_LL35B:;}
return 0;_LL34D: if(_tmp46D <= (void*)3?1:*((int*)_tmp46D)!= 7)goto _LL34F;_tmp474=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp46D)->f1;_tmp475=((struct Cyc_Absyn_ArrayType_struct*)
_tmp46D)->f2;_tmp476=((struct Cyc_Absyn_ArrayType_struct*)_tmp46D)->f3;_LL34E:{
void*_tmp48A=t2;void*_tmp48B;struct Cyc_Absyn_Tqual _tmp48C;struct Cyc_Absyn_Exp*
_tmp48D;_LL368: if(_tmp48A <= (void*)3?1:*((int*)_tmp48A)!= 7)goto _LL36A;_tmp48B=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp48A)->f1;_tmp48C=((struct Cyc_Absyn_ArrayType_struct*)
_tmp48A)->f2;_tmp48D=((struct Cyc_Absyn_ArrayType_struct*)_tmp48A)->f3;_LL369: {
int okay;okay=(_tmp476 != 0?_tmp48D != 0: 0)?Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp476))>= Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp48D)): 0;return(okay?Cyc_Tcutil_unify(_tmp474,_tmp48B): 0)?!
_tmp475.q_const?1: _tmp48C.q_const: 0;}_LL36A:;_LL36B: return 0;_LL367:;}return 0;
_LL34F: if(_tmp46D <= (void*)3?1:*((int*)_tmp46D)!= 12)goto _LL351;_tmp477=((struct
Cyc_Absyn_EnumType_struct*)_tmp46D)->f2;_LL350:{void*_tmp48E=t2;struct Cyc_Absyn_Enumdecl*
_tmp48F;_LL36D: if(_tmp48E <= (void*)3?1:*((int*)_tmp48E)!= 12)goto _LL36F;_tmp48F=((
struct Cyc_Absyn_EnumType_struct*)_tmp48E)->f2;_LL36E: if((_tmp477->fields != 0?
_tmp48F->fields != 0: 0)?((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp477->fields))->v)>= ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp48F->fields))->v): 0)return 1;goto _LL36C;_LL36F:;_LL370: goto _LL36C;
_LL36C:;}goto _LL352;_LL351: if(_tmp46D <= (void*)3?1:*((int*)_tmp46D)!= 5)goto
_LL353;_LL352: goto _LL354;_LL353: if((int)_tmp46D != 1)goto _LL355;_LL354: goto _LL356;
_LL355: if(_tmp46D <= (void*)3?1:*((int*)_tmp46D)!= 6)goto _LL357;_LL356: return Cyc_Tcutil_coerceable(
t2)?1: t2 == (void*)0;_LL357: if((int)_tmp46D != 0)goto _LL359;_LL358: return t2 == (
void*)0;_LL359:;_LL35A: return 0;_LL34A:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp490=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp491=_cycalloc(
sizeof(*_tmp491));_tmp491[0]=({struct Cyc_Absyn_Cast_e_struct _tmp492;_tmp492.tag=
13;_tmp492.f1=(void*)t;_tmp492.f2=_tmp490;_tmp492;});_tmp491;})));e->topt=({
struct Cyc_Core_Opt*_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->v=(void*)t;
_tmp493;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*_tmp494=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL372: if(_tmp494 <= (void*)
3?1:*((int*)_tmp494)!= 5)goto _LL374;_LL373: goto _LL375;_LL374: if(_tmp494 <= (void*)
3?1:*((int*)_tmp494)!= 12)goto _LL376;_LL375: goto _LL377;_LL376: if(_tmp494 <= (void*)
3?1:*((int*)_tmp494)!= 13)goto _LL378;_LL377: goto _LL379;_LL378: if(_tmp494 <= (void*)
3?1:*((int*)_tmp494)!= 14)goto _LL37A;_LL379: return 1;_LL37A: if(_tmp494 <= (void*)3?
1:*((int*)_tmp494)!= 0)goto _LL37C;_LL37B: return Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_t);_LL37C:;_LL37D: return 0;
_LL371:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp495=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL37F: if((int)_tmp495 != 1)goto _LL381;_LL380: goto
_LL382;_LL381: if(_tmp495 <= (void*)3?1:*((int*)_tmp495)!= 6)goto _LL383;_LL382:
return 1;_LL383:;_LL384: return 0;_LL37E:;}}int Cyc_Tcutil_is_function_type(void*t){
void*_tmp496=Cyc_Tcutil_compress(t);_LL386: if(_tmp496 <= (void*)3?1:*((int*)
_tmp496)!= 8)goto _LL388;_LL387: return 1;_LL388:;_LL389: return 0;_LL385:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple9 _tmp498=({struct _tuple9 _tmp497;_tmp497.f1=t1;
_tmp497.f2=t2;_tmp497;});void*_tmp499;int _tmp49A;void*_tmp49B;int _tmp49C;void*
_tmp49D;void*_tmp49E;void*_tmp49F;void*_tmp4A0;void*_tmp4A1;void*_tmp4A2;void*
_tmp4A3;void*_tmp4A4;void*_tmp4A5;void*_tmp4A6;void*_tmp4A7;void*_tmp4A8;void*
_tmp4A9;void*_tmp4AA;void*_tmp4AB;void*_tmp4AC;void*_tmp4AD;void*_tmp4AE;void*
_tmp4AF;void*_tmp4B0;void*_tmp4B1;void*_tmp4B2;_LL38B: _tmp499=_tmp498.f1;if(
_tmp499 <= (void*)3?1:*((int*)_tmp499)!= 6)goto _LL38D;_tmp49A=((struct Cyc_Absyn_DoubleType_struct*)
_tmp499)->f1;_tmp49B=_tmp498.f2;if(_tmp49B <= (void*)3?1:*((int*)_tmp49B)!= 6)
goto _LL38D;_tmp49C=((struct Cyc_Absyn_DoubleType_struct*)_tmp49B)->f1;_LL38C: if(
_tmp49A)return t1;else{return t2;}_LL38D: _tmp49D=_tmp498.f1;if(_tmp49D <= (void*)3?
1:*((int*)_tmp49D)!= 6)goto _LL38F;_LL38E: return t1;_LL38F: _tmp49E=_tmp498.f2;if(
_tmp49E <= (void*)3?1:*((int*)_tmp49E)!= 6)goto _LL391;_LL390: return t2;_LL391:
_tmp49F=_tmp498.f1;if((int)_tmp49F != 1)goto _LL393;_LL392: goto _LL394;_LL393:
_tmp4A0=_tmp498.f2;if((int)_tmp4A0 != 1)goto _LL395;_LL394: return(void*)1;_LL395:
_tmp4A1=_tmp498.f1;if(_tmp4A1 <= (void*)3?1:*((int*)_tmp4A1)!= 5)goto _LL397;
_tmp4A2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A1)->f1;if((int)_tmp4A2 != 
1)goto _LL397;_tmp4A3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A1)->f2;if((
int)_tmp4A3 != 3)goto _LL397;_LL396: goto _LL398;_LL397: _tmp4A4=_tmp498.f2;if(
_tmp4A4 <= (void*)3?1:*((int*)_tmp4A4)!= 5)goto _LL399;_tmp4A5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A4)->f1;if((int)_tmp4A5 != 1)goto _LL399;_tmp4A6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A4)->f2;if((int)_tmp4A6 != 3)goto _LL399;_LL398: return Cyc_Absyn_ulonglong_t;
_LL399: _tmp4A7=_tmp498.f1;if(_tmp4A7 <= (void*)3?1:*((int*)_tmp4A7)!= 5)goto
_LL39B;_tmp4A8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A7)->f1;if((int)
_tmp4A8 != 0)goto _LL39B;_tmp4A9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A7)->f2;
if((int)_tmp4A9 != 3)goto _LL39B;_LL39A: goto _LL39C;_LL39B: _tmp4AA=_tmp498.f2;if(
_tmp4AA <= (void*)3?1:*((int*)_tmp4AA)!= 5)goto _LL39D;_tmp4AB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4AA)->f1;if((int)_tmp4AB != 0)goto _LL39D;_tmp4AC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4AA)->f2;if((int)_tmp4AC != 3)goto _LL39D;_LL39C: return Cyc_Absyn_slonglong_t;
_LL39D: _tmp4AD=_tmp498.f1;if(_tmp4AD <= (void*)3?1:*((int*)_tmp4AD)!= 5)goto
_LL39F;_tmp4AE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AD)->f1;if((int)
_tmp4AE != 1)goto _LL39F;_tmp4AF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AD)->f2;
if((int)_tmp4AF != 2)goto _LL39F;_LL39E: goto _LL3A0;_LL39F: _tmp4B0=_tmp498.f2;if(
_tmp4B0 <= (void*)3?1:*((int*)_tmp4B0)!= 5)goto _LL3A1;_tmp4B1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B0)->f1;if((int)_tmp4B1 != 1)goto _LL3A1;_tmp4B2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B0)->f2;if((int)_tmp4B2 != 2)goto _LL3A1;_LL3A0: return Cyc_Absyn_uint_t;_LL3A1:;
_LL3A2: return Cyc_Absyn_sint_t;_LL38A:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp4B3=(void*)e->r;struct Cyc_Core_Opt*_tmp4B4;
_LL3A4: if(*((int*)_tmp4B3)!= 4)goto _LL3A6;_tmp4B4=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4B3)->f2;if(_tmp4B4 != 0)goto _LL3A6;_LL3A5:({void*_tmp4B5[0]={};Cyc_Tcutil_warn(
e->loc,_tag_arr("assignment in test",sizeof(unsigned char),19),_tag_arr(_tmp4B5,
sizeof(void*),0));});goto _LL3A3;_LL3A6:;_LL3A7: goto _LL3A3;_LL3A3:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple9 _tmp4B7=({struct _tuple9 _tmp4B6;_tmp4B6.f1=c1;_tmp4B6.f2=c2;_tmp4B6;});
void*_tmp4B8;void*_tmp4B9;void*_tmp4BA;void*_tmp4BB;void*_tmp4BC;struct Cyc_Core_Opt*
_tmp4BD;struct Cyc_Core_Opt**_tmp4BE;void*_tmp4BF;struct Cyc_Core_Opt*_tmp4C0;
struct Cyc_Core_Opt**_tmp4C1;void*_tmp4C2;struct Cyc_Core_Opt*_tmp4C3;struct Cyc_Core_Opt**
_tmp4C4;void*_tmp4C5;void*_tmp4C6;void*_tmp4C7;void*_tmp4C8;void*_tmp4C9;void*
_tmp4CA;struct Cyc_Core_Opt*_tmp4CB;struct Cyc_Core_Opt**_tmp4CC;void*_tmp4CD;void*
_tmp4CE;struct Cyc_Core_Opt*_tmp4CF;struct Cyc_Core_Opt**_tmp4D0;void*_tmp4D1;void*
_tmp4D2;struct Cyc_Core_Opt*_tmp4D3;struct Cyc_Core_Opt**_tmp4D4;void*_tmp4D5;
_LL3A9: _tmp4B8=_tmp4B7.f1;if(*((int*)_tmp4B8)!= 0)goto _LL3AB;_tmp4B9=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp4B8)->f1;_tmp4BA=_tmp4B7.f2;if(*((int*)_tmp4BA)
!= 0)goto _LL3AB;_tmp4BB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4BA)->f1;
_LL3AA: return _tmp4B9 == _tmp4BB;_LL3AB: _tmp4BC=_tmp4B7.f2;if(*((int*)_tmp4BC)!= 1)
goto _LL3AD;_tmp4BD=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4BC)->f1;_tmp4BE=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4BC)->f1;_LL3AC:*
_tmp4BE=({struct Cyc_Core_Opt*_tmp4D6=_cycalloc(sizeof(*_tmp4D6));_tmp4D6->v=(
void*)c1;_tmp4D6;});return 1;_LL3AD: _tmp4BF=_tmp4B7.f1;if(*((int*)_tmp4BF)!= 1)
goto _LL3AF;_tmp4C0=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4BF)->f1;_tmp4C1=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4BF)->f1;_LL3AE:*
_tmp4C1=({struct Cyc_Core_Opt*_tmp4D7=_cycalloc(sizeof(*_tmp4D7));_tmp4D7->v=(
void*)c2;_tmp4D7;});return 1;_LL3AF: _tmp4C2=_tmp4B7.f1;if(*((int*)_tmp4C2)!= 2)
goto _LL3B1;_tmp4C3=((struct Cyc_Absyn_Less_kb_struct*)_tmp4C2)->f1;_tmp4C4=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp4C2)->f1;_tmp4C5=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4C2)->f2;_tmp4C6=_tmp4B7.f2;if(*((
int*)_tmp4C6)!= 0)goto _LL3B1;_tmp4C7=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp4C6)->f1;_LL3B0: if(Cyc_Tcutil_kind_leq(_tmp4C7,_tmp4C5)){*_tmp4C4=({struct
Cyc_Core_Opt*_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8->v=(void*)c2;_tmp4D8;});
return 1;}else{return 0;}_LL3B1: _tmp4C8=_tmp4B7.f1;if(*((int*)_tmp4C8)!= 0)goto
_LL3B3;_tmp4C9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4C8)->f1;_tmp4CA=
_tmp4B7.f2;if(*((int*)_tmp4CA)!= 2)goto _LL3B3;_tmp4CB=((struct Cyc_Absyn_Less_kb_struct*)
_tmp4CA)->f1;_tmp4CC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp4CA)->f1;_tmp4CD=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4CA)->f2;
_LL3B2: if(Cyc_Tcutil_kind_leq(_tmp4C9,_tmp4CD)){*_tmp4CC=({struct Cyc_Core_Opt*
_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9->v=(void*)c1;_tmp4D9;});return 1;}
else{return 0;}_LL3B3: _tmp4CE=_tmp4B7.f1;if(*((int*)_tmp4CE)!= 2)goto _LL3A8;
_tmp4CF=((struct Cyc_Absyn_Less_kb_struct*)_tmp4CE)->f1;_tmp4D0=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4CE)->f1;_tmp4D1=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4CE)->f2;_tmp4D2=_tmp4B7.f2;if(*((int*)_tmp4D2)!= 2)goto _LL3A8;_tmp4D3=((
struct Cyc_Absyn_Less_kb_struct*)_tmp4D2)->f1;_tmp4D4=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4D2)->f1;_tmp4D5=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4D2)->f2;_LL3B4: if(Cyc_Tcutil_kind_leq(_tmp4D1,_tmp4D5)){*_tmp4D4=({struct
Cyc_Core_Opt*_tmp4DA=_cycalloc(sizeof(*_tmp4DA));_tmp4DA->v=(void*)c1;_tmp4DA;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp4D5,_tmp4D1)){*_tmp4D0=({struct Cyc_Core_Opt*
_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->v=(void*)c2;_tmp4DB;});return 1;}
else{return 0;}}_LL3A8:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp4DC=_cycalloc_atomic(sizeof(*_tmp4DC));_tmp4DC[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp4DC;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp4E0;_tmp4E0.tag=1;_tmp4E0.f1=(int)((unsigned int)
i);{void*_tmp4DF[1]={& _tmp4E0};Cyc_Std_aprintf(_tag_arr("#%d",sizeof(
unsigned char),4),_tag_arr(_tmp4DF,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*
_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->name=({struct _tagged_arr*_tmp4DE=
_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp4DE[0]=s;_tmp4DE;});_tmp4DD->identity=
0;_tmp4DD->kind=(void*)k;_tmp4DD;});}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _tagged_arr _tmp4E1=*t->name;return*((const unsigned char*)
_check_unknown_subscript(_tmp4E1,sizeof(unsigned char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_Std_String_pa_struct _tmp4E3;_tmp4E3.tag=0;
_tmp4E3.f1=(struct _tagged_arr)*t->name;{void*_tmp4E2[1]={& _tmp4E3};Cyc_Std_printf(
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp4E2,sizeof(void*),1));}});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{struct _tagged_arr _tmp4E4=Cyc_Std_strconcat(
_tag_arr("`",sizeof(unsigned char),2),*t->name);*((unsigned char*)
_check_unknown_subscript(_tmp4E4,sizeof(unsigned char),1))='t';t->name=({struct
_tagged_arr*_tmp4E5=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp4E5[0]=(struct
_tagged_arr)_tmp4E4;_tmp4E5;});}}struct _tuple14{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(struct _tuple14*x){
return({struct _tuple2*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6->f1=(struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7->v=(*x).f1;
_tmp4E7;});_tmp4E6->f2=(*x).f2;_tmp4E6->f3=(*x).f3;_tmp4E6;});}void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp4E8=0;{
struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd))_tmp4E8=({struct Cyc_List_List*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));
_tmp4E9->hd=(void*)((void*)atts->hd);_tmp4E9->tl=_tmp4E8;_tmp4E9;});}}return(
void*)({struct Cyc_Absyn_FnType_struct*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));
_tmp4EA[0]=({struct Cyc_Absyn_FnType_struct _tmp4EB;_tmp4EB.tag=8;_tmp4EB.f1=({
struct Cyc_Absyn_FnInfo _tmp4EC;_tmp4EC.tvars=fd->tvs;_tmp4EC.effect=fd->effect;
_tmp4EC.ret_typ=(void*)((void*)fd->ret_type);_tmp4EC.args=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args);_tmp4EC.c_varargs=fd->c_varargs;_tmp4EC.cyc_varargs=fd->cyc_varargs;
_tmp4EC.rgn_po=fd->rgn_po;_tmp4EC.attributes=_tmp4E8;_tmp4EC;});_tmp4EB;});
_tmp4EA;});}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple15{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple15*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*
t).f2;}static struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({
struct _tuple4*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED->f1=(*pr).f1;_tmp4ED->f2=
t;_tmp4ED;});}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};
struct _tuple17{struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple2*y){return({struct _tuple17*_tmp4EE=
_region_malloc(rgn,sizeof(*_tmp4EE));_tmp4EE->f1=({struct _tuple16*_tmp4EF=
_region_malloc(rgn,sizeof(*_tmp4EF));_tmp4EF->f1=(*y).f1;_tmp4EF->f2=(*y).f2;
_tmp4EF;});_tmp4EE->f2=(*y).f3;_tmp4EE;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(
struct _tuple17*w){struct _tuple16*_tmp4F1;void*_tmp4F2;struct _tuple17 _tmp4F0=*w;
_tmp4F1=_tmp4F0.f1;_tmp4F2=_tmp4F0.f2;{struct Cyc_Core_Opt*_tmp4F4;struct Cyc_Absyn_Tqual
_tmp4F5;struct _tuple16 _tmp4F3=*_tmp4F1;_tmp4F4=_tmp4F3.f1;_tmp4F5=_tmp4F3.f2;
return({struct _tuple2*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->f1=_tmp4F4;
_tmp4F6->f2=_tmp4F5;_tmp4F6->f3=_tmp4F2;_tmp4F6;});}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*
_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->name=f->name;_tmp4F7->tq=f->tq;
_tmp4F7->type=(void*)t;_tmp4F7->width=f->width;_tmp4F7->attributes=f->attributes;
_tmp4F7;});}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){void*_tmp4F8=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp4F9;struct Cyc_Absyn_AggrInfo _tmp4FA;void*_tmp4FB;
struct Cyc_List_List*_tmp4FC;struct Cyc_Absyn_TunionInfo _tmp4FD;void*_tmp4FE;
struct Cyc_List_List*_tmp4FF;void*_tmp500;struct Cyc_Absyn_TunionFieldInfo _tmp501;
void*_tmp502;struct Cyc_List_List*_tmp503;struct _tuple1*_tmp504;struct Cyc_List_List*
_tmp505;struct Cyc_Core_Opt*_tmp506;void*_tmp507;struct Cyc_Absyn_Tqual _tmp508;
struct Cyc_Absyn_Exp*_tmp509;struct Cyc_Absyn_PtrInfo _tmp50A;void*_tmp50B;void*
_tmp50C;struct Cyc_Absyn_Conref*_tmp50D;struct Cyc_Absyn_Tqual _tmp50E;struct Cyc_Absyn_Conref*
_tmp50F;struct Cyc_Absyn_FnInfo _tmp510;struct Cyc_List_List*_tmp511;struct Cyc_Core_Opt*
_tmp512;void*_tmp513;struct Cyc_List_List*_tmp514;int _tmp515;struct Cyc_Absyn_VarargInfo*
_tmp516;struct Cyc_List_List*_tmp517;struct Cyc_List_List*_tmp518;struct Cyc_List_List*
_tmp519;void*_tmp51A;struct Cyc_List_List*_tmp51B;struct Cyc_Core_Opt*_tmp51C;void*
_tmp51D;void*_tmp51E;void*_tmp51F;void*_tmp520;struct Cyc_List_List*_tmp521;
_LL3B6: if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 1)goto _LL3B8;_tmp4F9=((struct
Cyc_Absyn_VarType_struct*)_tmp4F8)->f1;_LL3B7: {struct _handler_cons _tmp522;
_push_handler(& _tmp522);{int _tmp524=0;if(setjmp(_tmp522.handler))_tmp524=1;if(!
_tmp524){{void*_tmp525=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp4F9);_npop_handler(0);return _tmp525;};_pop_handler();}else{void*_tmp523=(
void*)_exn_thrown;void*_tmp527=_tmp523;_LL3E5: if(_tmp527 != Cyc_Core_Not_found)
goto _LL3E7;_LL3E6: return t;_LL3E7:;_LL3E8:(void)_throw(_tmp527);_LL3E4:;}}}_LL3B8:
if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 10)goto _LL3BA;_tmp4FA=((struct Cyc_Absyn_AggrType_struct*)
_tmp4F8)->f1;_tmp4FB=(void*)_tmp4FA.aggr_info;_tmp4FC=_tmp4FA.targs;_LL3B9: {
struct Cyc_List_List*_tmp528=Cyc_Tcutil_substs(rgn,inst,_tmp4FC);return _tmp528 == 
_tmp4FC?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp529=_cycalloc(sizeof(*
_tmp529));_tmp529[0]=({struct Cyc_Absyn_AggrType_struct _tmp52A;_tmp52A.tag=10;
_tmp52A.f1=({struct Cyc_Absyn_AggrInfo _tmp52B;_tmp52B.aggr_info=(void*)_tmp4FB;
_tmp52B.targs=_tmp528;_tmp52B;});_tmp52A;});_tmp529;});}_LL3BA: if(_tmp4F8 <= (
void*)3?1:*((int*)_tmp4F8)!= 2)goto _LL3BC;_tmp4FD=((struct Cyc_Absyn_TunionType_struct*)
_tmp4F8)->f1;_tmp4FE=(void*)_tmp4FD.tunion_info;_tmp4FF=_tmp4FD.targs;_tmp500=(
void*)_tmp4FD.rgn;_LL3BB: {struct Cyc_List_List*_tmp52C=Cyc_Tcutil_substs(rgn,
inst,_tmp4FF);void*_tmp52D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp500);return(
_tmp52C == _tmp4FF?_tmp52D == _tmp500: 0)?t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E[0]=({struct Cyc_Absyn_TunionType_struct
_tmp52F;_tmp52F.tag=2;_tmp52F.f1=({struct Cyc_Absyn_TunionInfo _tmp530;_tmp530.tunion_info=(
void*)_tmp4FE;_tmp530.targs=_tmp52C;_tmp530.rgn=(void*)_tmp52D;_tmp530;});
_tmp52F;});_tmp52E;});}_LL3BC: if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 3)goto
_LL3BE;_tmp501=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4F8)->f1;_tmp502=(
void*)_tmp501.field_info;_tmp503=_tmp501.targs;_LL3BD: {struct Cyc_List_List*
_tmp531=Cyc_Tcutil_substs(rgn,inst,_tmp503);return _tmp531 == _tmp503?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp532=_cycalloc(sizeof(*_tmp532));
_tmp532[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp533;_tmp533.tag=3;_tmp533.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp534;_tmp534.field_info=(void*)_tmp502;_tmp534.targs=
_tmp531;_tmp534;});_tmp533;});_tmp532;});}_LL3BE: if(_tmp4F8 <= (void*)3?1:*((int*)
_tmp4F8)!= 16)goto _LL3C0;_tmp504=((struct Cyc_Absyn_TypedefType_struct*)_tmp4F8)->f1;
_tmp505=((struct Cyc_Absyn_TypedefType_struct*)_tmp4F8)->f2;_tmp506=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4F8)->f3;_LL3BF: {struct Cyc_List_List*_tmp535=Cyc_Tcutil_substs(rgn,inst,
_tmp505);return _tmp535 == _tmp505?t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp536=_cycalloc(sizeof(*_tmp536));_tmp536[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp537;_tmp537.tag=16;_tmp537.f1=_tmp504;_tmp537.f2=_tmp535;_tmp537.f3=_tmp506;
_tmp537;});_tmp536;});}_LL3C0: if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 7)goto
_LL3C2;_tmp507=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp4F8)->f1;_tmp508=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4F8)->f2;_tmp509=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4F8)->f3;_LL3C1: {void*_tmp538=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp507);
return _tmp538 == _tmp507?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp539=
_cycalloc(sizeof(*_tmp539));_tmp539[0]=({struct Cyc_Absyn_ArrayType_struct _tmp53A;
_tmp53A.tag=7;_tmp53A.f1=(void*)_tmp538;_tmp53A.f2=_tmp508;_tmp53A.f3=_tmp509;
_tmp53A;});_tmp539;});}_LL3C2: if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 4)goto
_LL3C4;_tmp50A=((struct Cyc_Absyn_PointerType_struct*)_tmp4F8)->f1;_tmp50B=(void*)
_tmp50A.elt_typ;_tmp50C=(void*)_tmp50A.rgn_typ;_tmp50D=_tmp50A.nullable;_tmp50E=
_tmp50A.tq;_tmp50F=_tmp50A.bounds;_LL3C3: {void*_tmp53B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp50B);void*_tmp53C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp50C);if(
_tmp53B == _tmp50B?_tmp53C == _tmp50C: 0)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D[0]=({struct Cyc_Absyn_PointerType_struct
_tmp53E;_tmp53E.tag=4;_tmp53E.f1=({struct Cyc_Absyn_PtrInfo _tmp53F;_tmp53F.elt_typ=(
void*)_tmp53B;_tmp53F.rgn_typ=(void*)_tmp53C;_tmp53F.nullable=_tmp50D;_tmp53F.tq=
_tmp50E;_tmp53F.bounds=_tmp50F;_tmp53F;});_tmp53E;});_tmp53D;});}_LL3C4: if(
_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 8)goto _LL3C6;_tmp510=((struct Cyc_Absyn_FnType_struct*)
_tmp4F8)->f1;_tmp511=_tmp510.tvars;_tmp512=_tmp510.effect;_tmp513=(void*)_tmp510.ret_typ;
_tmp514=_tmp510.args;_tmp515=_tmp510.c_varargs;_tmp516=_tmp510.cyc_varargs;
_tmp517=_tmp510.rgn_po;_tmp518=_tmp510.attributes;_LL3C5:{struct Cyc_List_List*
_tmp540=_tmp511;for(0;_tmp540 != 0;_tmp540=_tmp540->tl){inst=({struct Cyc_List_List*
_tmp541=_region_malloc(rgn,sizeof(*_tmp541));_tmp541->hd=({struct _tuple6*_tmp542=
_region_malloc(rgn,sizeof(*_tmp542));_tmp542->f1=(struct Cyc_Absyn_Tvar*)_tmp540->hd;
_tmp542->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp543=_cycalloc(sizeof(*
_tmp543));_tmp543[0]=({struct Cyc_Absyn_VarType_struct _tmp544;_tmp544.tag=1;
_tmp544.f1=(struct Cyc_Absyn_Tvar*)_tmp540->hd;_tmp544;});_tmp543;});_tmp542;});
_tmp541->tl=inst;_tmp541;});}}{struct Cyc_List_List*_tmp546;struct Cyc_List_List*
_tmp547;struct _tuple0 _tmp545=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp514));_tmp546=_tmp545.f1;_tmp547=_tmp545.f2;{struct Cyc_List_List*_tmp548=
Cyc_Tcutil_substs(rgn,inst,_tmp547);struct Cyc_List_List*_tmp549=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp546,_tmp548));struct Cyc_Core_Opt*
eff2;if(_tmp512 == 0)eff2=0;else{void*_tmp54A=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp512->v);if(_tmp54A == (void*)_tmp512->v)eff2=_tmp512;else{eff2=({struct
Cyc_Core_Opt*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->v=(void*)_tmp54A;
_tmp54B;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp516 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp54D;struct Cyc_Absyn_Tqual _tmp54E;void*
_tmp54F;int _tmp550;struct Cyc_Absyn_VarargInfo _tmp54C=*_tmp516;_tmp54D=_tmp54C.name;
_tmp54E=_tmp54C.tq;_tmp54F=(void*)_tmp54C.type;_tmp550=_tmp54C.inject;{void*
_tmp551=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp54F);if(_tmp551 == _tmp54F)
cyc_varargs2=_tmp516;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp552=
_cycalloc(sizeof(*_tmp552));_tmp552->name=_tmp54D;_tmp552->tq=_tmp54E;_tmp552->type=(
void*)_tmp551;_tmp552->inject=_tmp550;_tmp552;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp554;struct Cyc_List_List*_tmp555;struct _tuple0 _tmp553=Cyc_List_rsplit(
rgn,rgn,_tmp517);_tmp554=_tmp553.f1;_tmp555=_tmp553.f2;{struct Cyc_List_List*
_tmp556=Cyc_Tcutil_substs(rgn,inst,_tmp554);struct Cyc_List_List*_tmp557=Cyc_Tcutil_substs(
rgn,inst,_tmp555);if(_tmp556 == _tmp554?_tmp557 == _tmp555: 0)rgn_po2=_tmp517;else{
rgn_po2=Cyc_List_zip(_tmp556,_tmp557);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558[0]=({struct Cyc_Absyn_FnType_struct
_tmp559;_tmp559.tag=8;_tmp559.f1=({struct Cyc_Absyn_FnInfo _tmp55A;_tmp55A.tvars=
_tmp511;_tmp55A.effect=eff2;_tmp55A.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp513);_tmp55A.args=_tmp549;_tmp55A.c_varargs=_tmp515;_tmp55A.cyc_varargs=
cyc_varargs2;_tmp55A.rgn_po=rgn_po2;_tmp55A.attributes=_tmp518;_tmp55A;});
_tmp559;});_tmp558;});}}}}}_LL3C6: if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 9)
goto _LL3C8;_tmp519=((struct Cyc_Absyn_TupleType_struct*)_tmp4F8)->f1;_LL3C7: {
struct Cyc_List_List*_tmp55B=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp519);struct Cyc_List_List*_tmp55C=Cyc_Tcutil_substs(rgn,inst,_tmp55B);if(
_tmp55C == _tmp55B)return t;{struct Cyc_List_List*_tmp55D=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp519,_tmp55C);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E[0]=({struct Cyc_Absyn_TupleType_struct
_tmp55F;_tmp55F.tag=9;_tmp55F.f1=_tmp55D;_tmp55F;});_tmp55E;});}}_LL3C8: if(
_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 11)goto _LL3CA;_tmp51A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4F8)->f1;_tmp51B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp4F8)->f2;_LL3C9: {
struct Cyc_List_List*_tmp560=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp51B);struct Cyc_List_List*_tmp561=Cyc_Tcutil_substs(rgn,inst,_tmp560);if(
_tmp561 == _tmp560)return t;{struct Cyc_List_List*_tmp562=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp51B,
_tmp561);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp563=_cycalloc(
sizeof(*_tmp563));_tmp563[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp564;
_tmp564.tag=11;_tmp564.f1=(void*)_tmp51A;_tmp564.f2=_tmp562;_tmp564;});_tmp563;});}}
_LL3CA: if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 0)goto _LL3CC;_tmp51C=((struct
Cyc_Absyn_Evar_struct*)_tmp4F8)->f2;_LL3CB: if(_tmp51C != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp51C->v);else{return t;}_LL3CC: if(_tmp4F8 <= (void*)3?1:*((int*)
_tmp4F8)!= 15)goto _LL3CE;_tmp51D=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4F8)->f1;_LL3CD: {void*_tmp565=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp51D);
return _tmp565 == _tmp51D?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp566=
_cycalloc(sizeof(*_tmp566));_tmp566[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp567;_tmp567.tag=15;_tmp567.f1=(void*)_tmp565;_tmp567;});_tmp566;});}_LL3CE:
if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 14)goto _LL3D0;_tmp51E=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp4F8)->f1;_LL3CF: {void*_tmp568=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp51E);return _tmp568 == _tmp51E?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp56A;_tmp56A.tag=14;_tmp56A.f1=(void*)_tmp568;_tmp56A;});_tmp569;});}_LL3D0:
if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 12)goto _LL3D2;_LL3D1: goto _LL3D3;
_LL3D2: if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 13)goto _LL3D4;_LL3D3: goto
_LL3D5;_LL3D4: if((int)_tmp4F8 != 0)goto _LL3D6;_LL3D5: goto _LL3D7;_LL3D6: if(_tmp4F8
<= (void*)3?1:*((int*)_tmp4F8)!= 5)goto _LL3D8;_LL3D7: goto _LL3D9;_LL3D8: if((int)
_tmp4F8 != 1)goto _LL3DA;_LL3D9: goto _LL3DB;_LL3DA: if(_tmp4F8 <= (void*)3?1:*((int*)
_tmp4F8)!= 6)goto _LL3DC;_LL3DB: goto _LL3DD;_LL3DC: if((int)_tmp4F8 != 2)goto _LL3DE;
_LL3DD: return t;_LL3DE: if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 19)goto _LL3E0;
_tmp51F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp4F8)->f1;_LL3DF: {void*
_tmp56B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp51F);return _tmp56B == _tmp51F?t:(void*)({
struct Cyc_Absyn_RgnsEff_struct*_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C[0]=({
struct Cyc_Absyn_RgnsEff_struct _tmp56D;_tmp56D.tag=19;_tmp56D.f1=(void*)_tmp56B;
_tmp56D;});_tmp56C;});}_LL3E0: if(_tmp4F8 <= (void*)3?1:*((int*)_tmp4F8)!= 17)goto
_LL3E2;_tmp520=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp4F8)->f1;_LL3E1: {
void*_tmp56E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp520);return _tmp56E == _tmp520?t:(
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));
_tmp56F[0]=({struct Cyc_Absyn_AccessEff_struct _tmp570;_tmp570.tag=17;_tmp570.f1=(
void*)_tmp56E;_tmp570;});_tmp56F;});}_LL3E2: if(_tmp4F8 <= (void*)3?1:*((int*)
_tmp4F8)!= 18)goto _LL3B5;_tmp521=((struct Cyc_Absyn_JoinEff_struct*)_tmp4F8)->f1;
_LL3E3: {struct Cyc_List_List*_tmp571=Cyc_Tcutil_substs(rgn,inst,_tmp521);return
_tmp571 == _tmp521?t:(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp572=_cycalloc(
sizeof(*_tmp572));_tmp572[0]=({struct Cyc_Absyn_JoinEff_struct _tmp573;_tmp573.tag=
18;_tmp573.f1=_tmp571;_tmp573;});_tmp572;});}_LL3B5:;}static struct Cyc_List_List*
Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*
ts){if(ts == 0)return 0;{void*_tmp574=(void*)ts->hd;struct Cyc_List_List*_tmp575=ts->tl;
void*_tmp576=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp574);struct Cyc_List_List*
_tmp577=Cyc_Tcutil_substs(rgn,inst,_tmp575);if(_tmp574 == _tmp576?_tmp575 == 
_tmp577: 0)return ts;return(struct Cyc_List_List*)({struct Cyc_List_List*_tmp578=
_cycalloc(sizeof(*_tmp578));_tmp578->hd=(void*)_tmp576;_tmp578->tl=_tmp577;
_tmp578;});}}extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}struct _tuple6*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){void*k=Cyc_Tcutil_tvar_kind(tv);
return({struct _tuple6*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->f1=tv;_tmp579->f2=
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp57A=_cycalloc(sizeof(*_tmp57A));
_tmp57A->v=(void*)k;_tmp57A;}),({struct Cyc_Core_Opt*_tmp57B=_cycalloc(sizeof(*
_tmp57B));_tmp57B->v=s;_tmp57B;}));_tmp579;});}struct _tuple6*Cyc_Tcutil_r_make_inst_var(
struct _tuple7*env,struct Cyc_Absyn_Tvar*tv){struct _tuple7 _tmp57D;struct Cyc_List_List*
_tmp57E;struct _RegionHandle*_tmp57F;struct _tuple7*_tmp57C=env;_tmp57D=*_tmp57C;
_tmp57E=_tmp57D.f1;_tmp57F=_tmp57D.f2;{void*k=Cyc_Tcutil_tvar_kind(tv);return({
struct _tuple6*_tmp580=_region_malloc(_tmp57F,sizeof(*_tmp580));_tmp580->f1=tv;
_tmp580->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp581=_cycalloc(sizeof(*
_tmp581));_tmp581->v=(void*)k;_tmp581;}),({struct Cyc_Core_Opt*_tmp582=_cycalloc(
sizeof(*_tmp582));_tmp582->v=_tmp57E;_tmp582;}));_tmp580;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_Std_zstrptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=(void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))({struct Cyc_Std_String_pa_struct _tmp586;
_tmp586.tag=0;_tmp586.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string(k2);{
struct Cyc_Std_String_pa_struct _tmp585;_tmp585.tag=0;_tmp585.f1=(struct
_tagged_arr)Cyc_Absynpp_kindbound2string(k1);{struct Cyc_Std_String_pa_struct
_tmp584;_tmp584.tag=0;_tmp584.f1=(struct _tagged_arr)*tv->name;{void*_tmp583[3]={&
_tmp584,& _tmp585,& _tmp586};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof(unsigned char),59),_tag_arr(_tmp583,sizeof(void*),3));}}}});if(tv->identity
== 0)tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)
_check_null(tv->identity))!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({
void*_tmp587[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("same type variable has different identity!",sizeof(unsigned char),43),
_tag_arr(_tmp587,sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->hd=tv;
_tmp588->tl=tvs;_tmp588;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*
_tmp589[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv",sizeof(unsigned char),39),
_tag_arr(_tmp589,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 
0;tvs2=tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*
_tmp58A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2",sizeof(unsigned char),41),
_tag_arr(_tmp58A,sizeof(void*),0));});if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity))== *((int*)_check_null(tv->identity)))return tvs;}}return({
struct Cyc_List_List*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->hd=tv;_tmp58B->tl=
tvs;_tmp58B;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*
tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_Std_String_pa_struct
_tmp58D;_tmp58D.tag=0;_tmp58D.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string(tv);{
void*_tmp58C[1]={& _tmp58D};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bound tvar id for %s is NULL",sizeof(unsigned char),
29),_tag_arr(_tmp58C,sizeof(void*),1));}});return({struct Cyc_List_List*_tmp58E=
_cycalloc(sizeof(*_tmp58E));_tmp58E->hd=tv;_tmp58E->tl=tvs;_tmp58E;});}static
struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){void*
_tmp58F=Cyc_Tcutil_compress(e);int _tmp590;_LL3EA: if(_tmp58F <= (void*)3?1:*((int*)
_tmp58F)!= 0)goto _LL3EC;_tmp590=((struct Cyc_Absyn_Evar_struct*)_tmp58F)->f3;
_LL3EB:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*_tmp591=Cyc_Tcutil_compress((
void*)es2->hd);int _tmp592;_LL3EF: if(_tmp591 <= (void*)3?1:*((int*)_tmp591)!= 0)
goto _LL3F1;_tmp592=((struct Cyc_Absyn_Evar_struct*)_tmp591)->f3;_LL3F0: if(_tmp590
== _tmp592)return es;goto _LL3EE;_LL3F1:;_LL3F2: goto _LL3EE;_LL3EE:;}}return({
struct Cyc_List_List*_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->hd=(void*)e;
_tmp593->tl=es;_tmp593;});_LL3EC:;_LL3ED: return es;_LL3E9:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))
== *((int*)_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;
break;}}}if(!present)r=({struct Cyc_List_List*_tmp594=_cycalloc(sizeof(*_tmp594));
_tmp594->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp594->tl=r;_tmp594;});}r=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(width)))({struct Cyc_Std_String_pa_struct
_tmp596;_tmp596.tag=0;_tmp596.f1=(struct _tagged_arr)*fn;{void*_tmp595[1]={&
_tmp596};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s does not have constant width",
sizeof(unsigned char),41),_tag_arr(_tmp595,sizeof(void*),1));}});else{w=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(width));}{void*_tmp597=Cyc_Tcutil_compress(
field_typ);void*_tmp598;_LL3F4: if(_tmp597 <= (void*)3?1:*((int*)_tmp597)!= 5)goto
_LL3F6;_tmp598=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp597)->f2;_LL3F5:{
void*_tmp599=_tmp598;_LL3F9: if((int)_tmp599 != 0)goto _LL3FB;_LL3FA: if(w > 8)({void*
_tmp59A[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",sizeof(
unsigned char),26),_tag_arr(_tmp59A,sizeof(void*),0));});goto _LL3F8;_LL3FB: if((
int)_tmp599 != 1)goto _LL3FD;_LL3FC: if(w > 16)({void*_tmp59B[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bitfield larger than type",sizeof(unsigned char),26),_tag_arr(
_tmp59B,sizeof(void*),0));});goto _LL3F8;_LL3FD: if((int)_tmp599 != 2)goto _LL3FF;
_LL3FE: if(w > 32)({void*_tmp59C[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmp59C,sizeof(void*),0));});goto _LL3F8;
_LL3FF: if((int)_tmp599 != 3)goto _LL3F8;_LL400: if(w > 64)({void*_tmp59D[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bitfield larger than type",sizeof(unsigned char),26),_tag_arr(
_tmp59D,sizeof(void*),0));});goto _LL3F8;_LL3F8:;}goto _LL3F3;_LL3F6:;_LL3F7:({
struct Cyc_Std_String_pa_struct _tmp5A0;_tmp5A0.tag=0;_tmp5A0.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(field_typ);{struct Cyc_Std_String_pa_struct
_tmp59F;_tmp59F.tag=0;_tmp59F.f1=(struct _tagged_arr)*fn;{void*_tmp59E[2]={&
_tmp59F,& _tmp5A0};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s must have integral type but has type %s",
sizeof(unsigned char),52),_tag_arr(_tmp59E,sizeof(void*),2));}}});goto _LL3F3;
_LL3F3:;}}}static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,
struct _tagged_arr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){
void*_tmp5A1=(void*)atts->hd;_LL402: if((int)_tmp5A1 != 5)goto _LL404;_LL403:
continue;_LL404: if(_tmp5A1 <= (void*)16?1:*((int*)_tmp5A1)!= 1)goto _LL406;_LL405:
continue;_LL406:;_LL407:({struct Cyc_Std_String_pa_struct _tmp5A4;_tmp5A4.tag=0;
_tmp5A4.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_String_pa_struct _tmp5A3;_tmp5A3.tag=
0;_tmp5A3.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd);{
void*_tmp5A2[2]={& _tmp5A3,& _tmp5A4};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s on member %s",
sizeof(unsigned char),30),_tag_arr(_tmp5A2,sizeof(void*),2));}}});_LL401:;}}
struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*kind_env;struct Cyc_List_List*
free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
struct _tuple18{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t){{void*_tmp5A5=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*
_tmp5A6;struct Cyc_Core_Opt**_tmp5A7;struct Cyc_Core_Opt*_tmp5A8;struct Cyc_Core_Opt**
_tmp5A9;struct Cyc_Absyn_Tvar*_tmp5AA;struct Cyc_List_List*_tmp5AB;struct _tuple1*
_tmp5AC;struct Cyc_Absyn_Enumdecl*_tmp5AD;struct Cyc_Absyn_Enumdecl**_tmp5AE;
struct Cyc_Absyn_TunionInfo _tmp5AF;void*_tmp5B0;void**_tmp5B1;struct Cyc_List_List*
_tmp5B2;struct Cyc_List_List**_tmp5B3;void*_tmp5B4;struct Cyc_Absyn_TunionFieldInfo
_tmp5B5;void*_tmp5B6;void**_tmp5B7;struct Cyc_List_List*_tmp5B8;struct Cyc_Absyn_PtrInfo
_tmp5B9;void*_tmp5BA;void*_tmp5BB;struct Cyc_Absyn_Conref*_tmp5BC;struct Cyc_Absyn_Tqual
_tmp5BD;struct Cyc_Absyn_Conref*_tmp5BE;void*_tmp5BF;void*_tmp5C0;struct Cyc_Absyn_Tqual
_tmp5C1;struct Cyc_Absyn_Exp*_tmp5C2;struct Cyc_Absyn_FnInfo _tmp5C3;struct Cyc_List_List*
_tmp5C4;struct Cyc_List_List**_tmp5C5;struct Cyc_Core_Opt*_tmp5C6;struct Cyc_Core_Opt**
_tmp5C7;void*_tmp5C8;struct Cyc_List_List*_tmp5C9;int _tmp5CA;struct Cyc_Absyn_VarargInfo*
_tmp5CB;struct Cyc_List_List*_tmp5CC;struct Cyc_List_List*_tmp5CD;struct Cyc_List_List*
_tmp5CE;void*_tmp5CF;struct Cyc_List_List*_tmp5D0;struct Cyc_Absyn_AggrInfo _tmp5D1;
void*_tmp5D2;void**_tmp5D3;struct Cyc_List_List*_tmp5D4;struct Cyc_List_List**
_tmp5D5;struct _tuple1*_tmp5D6;struct Cyc_List_List*_tmp5D7;struct Cyc_List_List**
_tmp5D8;struct Cyc_Core_Opt*_tmp5D9;struct Cyc_Core_Opt**_tmp5DA;void*_tmp5DB;void*
_tmp5DC;void*_tmp5DD;struct Cyc_List_List*_tmp5DE;_LL409: if((int)_tmp5A5 != 0)goto
_LL40B;_LL40A: goto _LL408;_LL40B: if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 0)
goto _LL40D;_tmp5A6=((struct Cyc_Absyn_Evar_struct*)_tmp5A5)->f1;_tmp5A7=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp5A5)->f1;_tmp5A8=((struct Cyc_Absyn_Evar_struct*)
_tmp5A5)->f2;_tmp5A9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp5A5)->f2;_LL40C: if(*_tmp5A7 == 0)*_tmp5A7=({struct Cyc_Core_Opt*_tmp5DF=
_cycalloc(sizeof(*_tmp5DF));_tmp5DF->v=(void*)expected_kind;_tmp5DF;});if((
cvtenv.fn_result?cvtenv.generalize_evars: 0)?expected_kind == (void*)3: 0)*_tmp5A9=({
struct Cyc_Core_Opt*_tmp5E0=_cycalloc(sizeof(*_tmp5E0));_tmp5E0->v=(void*)((void*)
2);_tmp5E0;});else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Tvar*_tmp5E1=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Less_kb_struct*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));
_tmp5E5[0]=({struct Cyc_Absyn_Less_kb_struct _tmp5E6;_tmp5E6.tag=2;_tmp5E6.f1=0;
_tmp5E6.f2=(void*)expected_kind;_tmp5E6;});_tmp5E5;}));*_tmp5A9=({struct Cyc_Core_Opt*
_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3[0]=({struct Cyc_Absyn_VarType_struct
_tmp5E4;_tmp5E4.tag=1;_tmp5E4.f1=_tmp5E1;_tmp5E4;});_tmp5E3;}));_tmp5E2;});
_tmp5AA=_tmp5E1;goto _LL40E;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.free_evars,t);}}goto _LL408;_LL40D: if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)
!= 1)goto _LL40F;_tmp5AA=((struct Cyc_Absyn_VarType_struct*)_tmp5A5)->f1;_LL40E:{
void*_tmp5E7=Cyc_Absyn_compress_kb((void*)_tmp5AA->kind);struct Cyc_Core_Opt*
_tmp5E8;struct Cyc_Core_Opt**_tmp5E9;_LL438: if(*((int*)_tmp5E7)!= 1)goto _LL43A;
_tmp5E8=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E7)->f1;_tmp5E9=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E7)->f1;_LL439:*_tmp5E9=({struct Cyc_Core_Opt*
_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp5EC;_tmp5EC.tag=2;_tmp5EC.f1=0;_tmp5EC.f2=(void*)expected_kind;_tmp5EC;});
_tmp5EB;}));_tmp5EA;});goto _LL437;_LL43A:;_LL43B: goto _LL437;_LL437:;}cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp5AA);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp5AA);goto _LL408;_LL40F: if(_tmp5A5 <= (void*)3?1:*((int*)
_tmp5A5)!= 13)goto _LL411;_tmp5AB=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp5A5)->f1;
_LL410: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp5ED=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp5ED;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp5AB != 0;_tmp5AB=_tmp5AB->tl){struct Cyc_Absyn_Enumfield*_tmp5EE=(
struct Cyc_Absyn_Enumfield*)_tmp5AB->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp5EE->name).f2))({struct Cyc_Std_String_pa_struct
_tmp5F0;_tmp5F0.tag=0;_tmp5F0.f1=(struct _tagged_arr)*(*_tmp5EE->name).f2;{void*
_tmp5EF[1]={& _tmp5F0};Cyc_Tcutil_terr(_tmp5EE->loc,_tag_arr("duplicate enum field name %s",
sizeof(unsigned char),29),_tag_arr(_tmp5EF,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp5F1=_region_malloc(uprev_rgn,sizeof(*
_tmp5F1));_tmp5F1->hd=(*_tmp5EE->name).f2;_tmp5F1->tl=prev_fields;_tmp5F1;});}
if(_tmp5EE->tag == 0)_tmp5EE->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp5EE->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp5EE->tag)))({struct Cyc_Std_String_pa_struct _tmp5F3;_tmp5F3.tag=0;
_tmp5F3.f1=(struct _tagged_arr)*(*_tmp5EE->name).f2;{void*_tmp5F2[1]={& _tmp5F3};
Cyc_Tcutil_terr(loc,_tag_arr("enum field %s: expression is not constant",sizeof(
unsigned char),42),_tag_arr(_tmp5F2,sizeof(void*),1));}});}{unsigned int t1=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5EE->tag));tag_count=t1 + 1;(*_tmp5EE->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp5F5;_tmp5F5.tag=1;_tmp5F5.f1=te->ns;_tmp5F5;});
_tmp5F4;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple18*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp5EE->name).f2,({
struct _tuple18*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp5F8;_tmp5F8.tag=4;_tmp5F8.f1=(void*)t;
_tmp5F8.f2=_tmp5EE;_tmp5F8;});_tmp5F7;});_tmp5F6->f2=1;_tmp5F6;}));}}};
_pop_region(uprev_rgn);}goto _LL408;}_LL411: if(_tmp5A5 <= (void*)3?1:*((int*)
_tmp5A5)!= 12)goto _LL413;_tmp5AC=((struct Cyc_Absyn_EnumType_struct*)_tmp5A5)->f1;
_tmp5AD=((struct Cyc_Absyn_EnumType_struct*)_tmp5A5)->f2;_tmp5AE=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp5A5)->f2;_LL412: if(*_tmp5AE == 0?1:((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp5AE))->fields == 0){struct _handler_cons
_tmp5F9;_push_handler(& _tmp5F9);{int _tmp5FB=0;if(setjmp(_tmp5F9.handler))_tmp5FB=
1;if(!_tmp5FB){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp5AC);*_tmp5AE=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp5FA=(void*)_exn_thrown;void*_tmp5FD=_tmp5FA;_LL43D: if(_tmp5FD != Cyc_Dict_Absent)
goto _LL43F;_LL43E: {struct Cyc_Tcenv_Genv*_tmp5FE=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp5FF=({struct Cyc_Absyn_Enumdecl*_tmp600=_cycalloc(
sizeof(*_tmp600));_tmp600->sc=(void*)((void*)3);_tmp600->name=_tmp5AC;_tmp600->fields=
0;_tmp600;});Cyc_Tc_tcEnumdecl(te,_tmp5FE,loc,_tmp5FF);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp5AC);*_tmp5AE=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL43C;}}_LL43F:;_LL440:(void)_throw(_tmp5FD);_LL43C:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp5AE);*_tmp5AC=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL408;}_LL413: if(_tmp5A5 <= (void*)3?1:*((
int*)_tmp5A5)!= 2)goto _LL415;_tmp5AF=((struct Cyc_Absyn_TunionType_struct*)
_tmp5A5)->f1;_tmp5B0=(void*)_tmp5AF.tunion_info;_tmp5B1=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp5A5)->f1).tunion_info;_tmp5B2=_tmp5AF.targs;_tmp5B3=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp5A5)->f1).targs;_tmp5B4=(void*)_tmp5AF.rgn;
_LL414: {struct Cyc_List_List*_tmp601=*_tmp5B3;{void*_tmp602=*_tmp5B1;struct Cyc_Absyn_UnknownTunionInfo
_tmp603;struct _tuple1*_tmp604;int _tmp605;struct Cyc_Absyn_Tuniondecl**_tmp606;
struct Cyc_Absyn_Tuniondecl*_tmp607;_LL442: if(*((int*)_tmp602)!= 0)goto _LL444;
_tmp603=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp602)->f1;_tmp604=_tmp603.name;
_tmp605=_tmp603.is_xtunion;_LL443: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp608;_push_handler(& _tmp608);{int _tmp60A=0;if(setjmp(_tmp608.handler))
_tmp60A=1;if(!_tmp60A){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp604);;
_pop_handler();}else{void*_tmp609=(void*)_exn_thrown;void*_tmp60C=_tmp609;_LL447:
if(_tmp60C != Cyc_Dict_Absent)goto _LL449;_LL448: {struct Cyc_Tcenv_Genv*_tmp60D=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp60E=({struct Cyc_Absyn_Tuniondecl*
_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->sc=(void*)((void*)3);_tmp612->name=
_tmp604;_tmp612->tvs=0;_tmp612->fields=0;_tmp612->is_xtunion=_tmp605;_tmp612;});
Cyc_Tc_tcTuniondecl(te,_tmp60D,loc,_tmp60E);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp604);if(_tmp601 != 0){({struct Cyc_Std_String_pa_struct _tmp611;_tmp611.tag=
0;_tmp611.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp604);{struct Cyc_Std_String_pa_struct
_tmp610;_tmp610.tag=0;_tmp610.f1=(struct _tagged_arr)(_tmp605?_tag_arr("xtunion",
sizeof(unsigned char),8): _tag_arr("tunion",sizeof(unsigned char),7));{void*
_tmp60F[2]={& _tmp610,& _tmp611};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized %s %s before using",
sizeof(unsigned char),48),_tag_arr(_tmp60F,sizeof(void*),2));}}});return cvtenv;}
goto _LL446;}_LL449:;_LL44A:(void)_throw(_tmp60C);_LL446:;}}}if((*tudp)->is_xtunion
!= _tmp605)({struct Cyc_Std_String_pa_struct _tmp614;_tmp614.tag=0;_tmp614.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp604);{void*_tmp613[1]={& _tmp614};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(unsigned char),48),_tag_arr(_tmp613,sizeof(void*),1));}});*_tmp5B1=(void*)({
struct Cyc_Absyn_KnownTunion_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615[0]=({
struct Cyc_Absyn_KnownTunion_struct _tmp616;_tmp616.tag=1;_tmp616.f1=tudp;_tmp616;});
_tmp615;});_tmp607=*tudp;goto _LL445;}_LL444: if(*((int*)_tmp602)!= 1)goto _LL441;
_tmp606=((struct Cyc_Absyn_KnownTunion_struct*)_tmp602)->f1;_tmp607=*_tmp606;
_LL445: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp5B4);{
struct Cyc_List_List*tvs=_tmp607->tvs;for(0;_tmp601 != 0?tvs != 0: 0;(_tmp601=
_tmp601->tl,tvs=tvs->tl)){void*t1=(void*)_tmp601->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1);}if(_tmp601 != 0)({struct Cyc_Std_String_pa_struct _tmp618;_tmp618.tag=
0;_tmp618.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp607->name);{void*
_tmp617[1]={& _tmp618};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s",
sizeof(unsigned char),38),_tag_arr(_tmp617,sizeof(void*),1));}});if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->hd=(void*)e;_tmp619->tl=
hidden_ts;_tmp619;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*
_tmp5B3=Cyc_List_imp_append(*_tmp5B3,Cyc_List_imp_rev(hidden_ts));}goto _LL441;}
_LL441:;}goto _LL408;}_LL415: if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 3)goto
_LL417;_tmp5B5=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp5A5)->f1;_tmp5B6=(
void*)_tmp5B5.field_info;_tmp5B7=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5A5)->f1).field_info;_tmp5B8=_tmp5B5.targs;_LL416:{void*_tmp61A=*_tmp5B7;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp61B;struct _tuple1*_tmp61C;struct
_tuple1*_tmp61D;int _tmp61E;struct Cyc_Absyn_Tuniondecl*_tmp61F;struct Cyc_Absyn_Tunionfield*
_tmp620;_LL44C: if(*((int*)_tmp61A)!= 0)goto _LL44E;_tmp61B=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp61A)->f1;_tmp61C=_tmp61B.tunion_name;_tmp61D=_tmp61B.field_name;_tmp61E=
_tmp61B.is_xtunion;_LL44D: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp621;_push_handler(& _tmp621);{int _tmp623=0;if(setjmp(
_tmp621.handler))_tmp623=1;if(!_tmp623){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp61C);;_pop_handler();}else{void*_tmp622=(void*)_exn_thrown;void*_tmp625=
_tmp622;_LL451: if(_tmp625 != Cyc_Dict_Absent)goto _LL453;_LL452:({struct Cyc_Std_String_pa_struct
_tmp627;_tmp627.tag=0;_tmp627.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp61C);{void*_tmp626[1]={& _tmp627};Cyc_Tcutil_terr(loc,_tag_arr("unbound type tunion %s",
sizeof(unsigned char),23),_tag_arr(_tmp626,sizeof(void*),1));}});return cvtenv;
_LL453:;_LL454:(void)_throw(_tmp625);_LL450:;}}}{struct _handler_cons _tmp628;
_push_handler(& _tmp628);{int _tmp62A=0;if(setjmp(_tmp628.handler))_tmp62A=1;if(!
_tmp62A){{void*_tmp62B=Cyc_Tcenv_lookup_ordinary(te,loc,_tmp61D);struct Cyc_Absyn_Tuniondecl*
_tmp62C;struct Cyc_Absyn_Tunionfield*_tmp62D;_LL456: if(*((int*)_tmp62B)!= 2)goto
_LL458;_tmp62C=((struct Cyc_Tcenv_TunionRes_struct*)_tmp62B)->f1;_tmp62D=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp62B)->f2;_LL457: tuf=_tmp62D;tud=_tmp62C;if(tud->is_xtunion
!= _tmp61E)({struct Cyc_Std_String_pa_struct _tmp62F;_tmp62F.tag=0;_tmp62F.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp61C);{void*_tmp62E[1]={& _tmp62F};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(unsigned char),48),_tag_arr(_tmp62E,sizeof(void*),1));}});goto _LL455;
_LL458:;_LL459:({struct Cyc_Std_String_pa_struct _tmp632;_tmp632.tag=0;_tmp632.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp61C);{struct Cyc_Std_String_pa_struct
_tmp631;_tmp631.tag=0;_tmp631.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp61D);{void*_tmp630[2]={& _tmp631,& _tmp632};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(unsigned char),35),_tag_arr(_tmp630,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv
_tmp633=cvtenv;_npop_handler(0);return _tmp633;}_LL455:;};_pop_handler();}else{
void*_tmp629=(void*)_exn_thrown;void*_tmp635=_tmp629;_LL45B: if(_tmp635 != Cyc_Dict_Absent)
goto _LL45D;_LL45C:({struct Cyc_Std_String_pa_struct _tmp638;_tmp638.tag=0;_tmp638.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp61C);{struct Cyc_Std_String_pa_struct
_tmp637;_tmp637.tag=0;_tmp637.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp61D);{void*_tmp636[2]={& _tmp637,& _tmp638};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(unsigned char),35),_tag_arr(_tmp636,sizeof(void*),2));}}});return cvtenv;
_LL45D:;_LL45E:(void)_throw(_tmp635);_LL45A:;}}}*_tmp5B7=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp63A;_tmp63A.tag=1;_tmp63A.f1=tud;_tmp63A.f2=tuf;_tmp63A;});_tmp639;});
_tmp61F=tud;_tmp620=tuf;goto _LL44F;}_LL44E: if(*((int*)_tmp61A)!= 1)goto _LL44B;
_tmp61F=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp61A)->f1;_tmp620=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp61A)->f2;_LL44F: {struct Cyc_List_List*tvs=
_tmp61F->tvs;for(0;_tmp5B8 != 0?tvs != 0: 0;(_tmp5B8=_tmp5B8->tl,tvs=tvs->tl)){void*
t1=(void*)_tmp5B8->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp5B8 != 0)({
struct Cyc_Std_String_pa_struct _tmp63D;_tmp63D.tag=0;_tmp63D.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp620->name);{struct Cyc_Std_String_pa_struct
_tmp63C;_tmp63C.tag=0;_tmp63C.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp61F->name);{void*_tmp63B[2]={& _tmp63C,& _tmp63D};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s.%s",
sizeof(unsigned char),41),_tag_arr(_tmp63B,sizeof(void*),2));}}});if(tvs != 0)({
struct Cyc_Std_String_pa_struct _tmp640;_tmp640.tag=0;_tmp640.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp620->name);{struct Cyc_Std_String_pa_struct
_tmp63F;_tmp63F.tag=0;_tmp63F.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp61F->name);{void*_tmp63E[2]={& _tmp63F,& _tmp640};Cyc_Tcutil_terr(loc,_tag_arr("too few type arguments for tunion %s.%s",
sizeof(unsigned char),40),_tag_arr(_tmp63E,sizeof(void*),2));}}});goto _LL44B;}
_LL44B:;}goto _LL408;_LL417: if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 4)goto
_LL419;_tmp5B9=((struct Cyc_Absyn_PointerType_struct*)_tmp5A5)->f1;_tmp5BA=(void*)
_tmp5B9.elt_typ;_tmp5BB=(void*)_tmp5B9.rgn_typ;_tmp5BC=_tmp5B9.nullable;_tmp5BD=
_tmp5B9.tq;_tmp5BE=_tmp5B9.bounds;_LL418: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp5BA);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5BB);{void*_tmp641=(void*)(Cyc_Absyn_compress_conref(_tmp5BE))->v;
void*_tmp642;struct Cyc_Absyn_Exp*_tmp643;_LL460: if(_tmp641 <= (void*)1?1:*((int*)
_tmp641)!= 0)goto _LL462;_tmp642=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp641)->f1;if(_tmp642 <= (void*)1?1:*((int*)_tmp642)!= 0)goto _LL462;_tmp643=((
struct Cyc_Absyn_Upper_b_struct*)_tmp642)->f1;_LL461: if(!Cyc_Tcutil_is_const_exp(
te,_tmp643))({void*_tmp644[0]={};Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not a constant",
sizeof(unsigned char),44),_tag_arr(_tmp644,sizeof(void*),0));});Cyc_Tcexp_tcExp(
te,0,_tmp643);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp643))({void*_tmp645[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not an unsigned int",
sizeof(unsigned char),49),_tag_arr(_tmp645,sizeof(void*),0));});Cyc_Evexp_eval_const_uint_exp(
_tmp643);goto _LL45F;_LL462:;_LL463: goto _LL45F;_LL45F:;}goto _LL408;_LL419: if(
_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 14)goto _LL41B;_tmp5BF=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp5A5)->f1;_LL41A: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,
_tmp5BF);goto _LL408;_LL41B: if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 5)goto
_LL41D;_LL41C: goto _LL41E;_LL41D: if((int)_tmp5A5 != 1)goto _LL41F;_LL41E: goto _LL420;
_LL41F: if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 6)goto _LL421;_LL420: goto _LL408;
_LL421: if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 7)goto _LL423;_tmp5C0=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp5A5)->f1;_tmp5C1=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5A5)->f2;_tmp5C2=((struct Cyc_Absyn_ArrayType_struct*)_tmp5A5)->f3;_LL422:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,_tmp5C0);if(_tmp5C2
== 0?1: !Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp5C2)))({
void*_tmp646[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array bounds expression is not constant",
sizeof(unsigned char),40),_tag_arr(_tmp646,sizeof(void*),0));});Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp5C2));if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmp5C2)))({void*_tmp647[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("array bounds expression is not an unsigned int",sizeof(
unsigned char),47),_tag_arr(_tmp647,sizeof(void*),0));});Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5C2));goto _LL408;_LL423: if(_tmp5A5 <= (void*)
3?1:*((int*)_tmp5A5)!= 8)goto _LL425;_tmp5C3=((struct Cyc_Absyn_FnType_struct*)
_tmp5A5)->f1;_tmp5C4=_tmp5C3.tvars;_tmp5C5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp5A5)->f1).tvars;_tmp5C6=_tmp5C3.effect;_tmp5C7=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp5A5)->f1).effect;_tmp5C8=(void*)_tmp5C3.ret_typ;
_tmp5C9=_tmp5C3.args;_tmp5CA=_tmp5C3.c_varargs;_tmp5CB=_tmp5C3.cyc_varargs;
_tmp5CC=_tmp5C3.rgn_po;_tmp5CD=_tmp5C3.attributes;_LL424: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp5CD != 0;_tmp5CD=_tmp5CD->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp5CD->hd))({struct Cyc_Std_String_pa_struct
_tmp649;_tmp649.tag=0;_tmp649.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp5CD->hd);{void*_tmp648[1]={& _tmp649};Cyc_Tcutil_terr(loc,_tag_arr("bad function type attribute %s",
sizeof(unsigned char),31),_tag_arr(_tmp648,sizeof(void*),1));}});{void*_tmp64A=(
void*)_tmp5CD->hd;void*_tmp64B;int _tmp64C;int _tmp64D;_LL465: if((int)_tmp64A != 0)
goto _LL467;_LL466: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL464;
_LL467: if((int)_tmp64A != 1)goto _LL469;_LL468: if(!seen_cdecl){seen_cdecl=1;++
num_convs;}goto _LL464;_LL469: if((int)_tmp64A != 2)goto _LL46B;_LL46A: if(!
seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL464;_LL46B: if(_tmp64A <= (void*)
16?1:*((int*)_tmp64A)!= 3)goto _LL46D;_tmp64B=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp64A)->f1;_tmp64C=((struct Cyc_Absyn_Format_att_struct*)_tmp64A)->f2;_tmp64D=((
struct Cyc_Absyn_Format_att_struct*)_tmp64A)->f3;_LL46C: if(!seen_format){
seen_format=1;ft=_tmp64B;fmt_desc_arg=_tmp64C;fmt_arg_start=_tmp64D;}else{({void*
_tmp64E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function can't have multiple format attributes",
sizeof(unsigned char),47),_tag_arr(_tmp64E,sizeof(void*),0));});}goto _LL464;
_LL46D:;_LL46E: goto _LL464;_LL464:;}}if(num_convs > 1)({void*_tmp64F[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("function can't have multiple calling conventions",sizeof(
unsigned char),49),_tag_arr(_tmp64F,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(
loc,*_tmp5C5);{struct Cyc_List_List*b=*_tmp5C5;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)
b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp650=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp651;_LL470: if(*((int*)
_tmp650)!= 0)goto _LL472;_tmp651=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp650)->f1;
if((int)_tmp651 != 1)goto _LL472;_LL471:({struct Cyc_Std_String_pa_struct _tmp653;
_tmp653.tag=0;_tmp653.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name;{
void*_tmp652[1]={& _tmp653};Cyc_Tcutil_terr(loc,_tag_arr("function attempts to abstract Mem type variable %s",
sizeof(unsigned char),51),_tag_arr(_tmp652,sizeof(void*),1));}});goto _LL46F;
_LL472:;_LL473: goto _LL46F;_LL46F:;}}}{struct Cyc_Tcutil_CVTEnv _tmp654=({struct Cyc_Tcutil_CVTEnv
_tmp6C3;_tmp6C3.kind_env=cvtenv.kind_env;_tmp6C3.free_vars=0;_tmp6C3.free_evars=
0;_tmp6C3.generalize_evars=cvtenv.generalize_evars;_tmp6C3.fn_result=1;_tmp6C3;});
_tmp654=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp654,(void*)1,_tmp5C8);_tmp654.fn_result=
0;{struct Cyc_List_List*a=_tmp5C9;for(0;a != 0;a=a->tl){_tmp654=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp654,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp5CB != 0){if(
_tmp5CA)({void*_tmp655[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("both c_vararg and cyc_vararg",sizeof(unsigned char),
29),_tag_arr(_tmp655,sizeof(void*),0));});{struct Cyc_Core_Opt*_tmp657;struct Cyc_Absyn_Tqual
_tmp658;void*_tmp659;int _tmp65A;struct Cyc_Absyn_VarargInfo _tmp656=*_tmp5CB;
_tmp657=_tmp656.name;_tmp658=_tmp656.tq;_tmp659=(void*)_tmp656.type;_tmp65A=
_tmp656.inject;_tmp654=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp654,(void*)1,
_tmp659);if(_tmp65A){void*_tmp65B=Cyc_Tcutil_compress(_tmp659);struct Cyc_Absyn_TunionInfo
_tmp65C;void*_tmp65D;void*_tmp65E;_LL475: if(_tmp65B <= (void*)3?1:*((int*)_tmp65B)
!= 2)goto _LL477;_tmp65C=((struct Cyc_Absyn_TunionType_struct*)_tmp65B)->f1;
_tmp65D=(void*)_tmp65C.tunion_info;if(*((int*)_tmp65D)!= 1)goto _LL477;_tmp65E=(
void*)_tmp65C.rgn;_LL476: goto _LL474;_LL477:;_LL478:({void*_tmp65F[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("can't inject a non-[x]tunion type",sizeof(unsigned char),34),
_tag_arr(_tmp65F,sizeof(void*),0));});goto _LL474;_LL474:;}}}if(seen_format){int
_tmp660=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5C9);if((((
fmt_desc_arg < 0?1: fmt_desc_arg > _tmp660)?1: fmt_arg_start < 0)?1:(_tmp5CB == 0?
fmt_arg_start != 0: 0))?1:(_tmp5CB != 0?fmt_arg_start != _tmp660 + 1: 0))({void*
_tmp661[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format descriptor",sizeof(
unsigned char),22),_tag_arr(_tmp661,sizeof(void*),0));});else{void*_tmp663;
struct _tuple2 _tmp662=*((struct _tuple2*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmp5C9,fmt_desc_arg - 1);_tmp663=_tmp662.f3;{void*_tmp664=Cyc_Tcutil_compress(
_tmp663);struct Cyc_Absyn_PtrInfo _tmp665;void*_tmp666;struct Cyc_Absyn_Conref*
_tmp667;_LL47A: if(_tmp664 <= (void*)3?1:*((int*)_tmp664)!= 4)goto _LL47C;_tmp665=((
struct Cyc_Absyn_PointerType_struct*)_tmp664)->f1;_tmp666=(void*)_tmp665.elt_typ;
_tmp667=_tmp665.bounds;_LL47B:{struct _tuple9 _tmp669=({struct _tuple9 _tmp668;
_tmp668.f1=Cyc_Tcutil_compress(_tmp666);_tmp668.f2=Cyc_Absyn_conref_def((void*)0,
_tmp667);_tmp668;});void*_tmp66A;void*_tmp66B;void*_tmp66C;void*_tmp66D;_LL47F:
_tmp66A=_tmp669.f1;if(_tmp66A <= (void*)3?1:*((int*)_tmp66A)!= 5)goto _LL481;
_tmp66B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp66A)->f1;if((int)_tmp66B != 
1)goto _LL481;_tmp66C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp66A)->f2;if((
int)_tmp66C != 0)goto _LL481;_tmp66D=_tmp669.f2;if((int)_tmp66D != 0)goto _LL481;
_LL480: goto _LL47E;_LL481:;_LL482:({void*_tmp66E[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("format descriptor is not a char ? type",sizeof(unsigned char),39),
_tag_arr(_tmp66E,sizeof(void*),0));});goto _LL47E;_LL47E:;}goto _LL479;_LL47C:;
_LL47D:({void*_tmp66F[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format descriptor is not a char ? type",
sizeof(unsigned char),39),_tag_arr(_tmp66F,sizeof(void*),0));});goto _LL479;
_LL479:;}if(fmt_arg_start != 0){void*_tmp670=Cyc_Tcutil_compress((void*)((struct
Cyc_Absyn_VarargInfo*)_check_null(_tmp5CB))->type);int problem;{void*_tmp671=ft;
_LL484: if((int)_tmp671 != 0)goto _LL486;_LL485:{void*_tmp672=_tmp670;struct Cyc_Absyn_TunionInfo
_tmp673;void*_tmp674;struct Cyc_Absyn_Tuniondecl**_tmp675;struct Cyc_Absyn_Tuniondecl*
_tmp676;_LL489: if(_tmp672 <= (void*)3?1:*((int*)_tmp672)!= 2)goto _LL48B;_tmp673=((
struct Cyc_Absyn_TunionType_struct*)_tmp672)->f1;_tmp674=(void*)_tmp673.tunion_info;
if(*((int*)_tmp674)!= 1)goto _LL48B;_tmp675=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp674)->f1;_tmp676=*_tmp675;_LL48A: problem=Cyc_Absyn_qvar_cmp(_tmp676->name,
Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL488;_LL48B:;_LL48C: problem=1;goto
_LL488;_LL488:;}goto _LL483;_LL486: if((int)_tmp671 != 1)goto _LL483;_LL487:{void*
_tmp677=_tmp670;struct Cyc_Absyn_TunionInfo _tmp678;void*_tmp679;struct Cyc_Absyn_Tuniondecl**
_tmp67A;struct Cyc_Absyn_Tuniondecl*_tmp67B;_LL48E: if(_tmp677 <= (void*)3?1:*((int*)
_tmp677)!= 2)goto _LL490;_tmp678=((struct Cyc_Absyn_TunionType_struct*)_tmp677)->f1;
_tmp679=(void*)_tmp678.tunion_info;if(*((int*)_tmp679)!= 1)goto _LL490;_tmp67A=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp679)->f1;_tmp67B=*_tmp67A;_LL48F: problem=
Cyc_Absyn_qvar_cmp(_tmp67B->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL48D;
_LL490:;_LL491: problem=1;goto _LL48D;_LL48D:;}goto _LL483;_LL483:;}if(problem)({
void*_tmp67C[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format attribute and vararg types don't match",
sizeof(unsigned char),46),_tag_arr(_tmp67C,sizeof(void*),0));});}}}{struct Cyc_List_List*
rpo=_tmp5CC;for(0;rpo != 0;rpo=rpo->tl){struct _tuple9 _tmp67E;void*_tmp67F;void*
_tmp680;struct _tuple9*_tmp67D=(struct _tuple9*)rpo->hd;_tmp67E=*_tmp67D;_tmp67F=
_tmp67E.f1;_tmp680=_tmp67E.f2;_tmp654=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp654,(void*)3,_tmp67F);_tmp654=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp654,(
void*)3,_tmp680);}}if(*_tmp5C7 != 0)_tmp654=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp654,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp5C7))->v);else{
if(cvtenv.generalize_evars);{struct Cyc_List_List*effect=0;{struct Cyc_List_List*
tvs=_tmp654.free_vars;for(0;tvs != 0;tvs=tvs->tl){void*_tmp681=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmp682;struct
Cyc_Core_Opt**_tmp683;void*_tmp684;void*_tmp685;struct Cyc_Core_Opt*_tmp686;
struct Cyc_Core_Opt**_tmp687;void*_tmp688;void*_tmp689;struct Cyc_Core_Opt*_tmp68A;
struct Cyc_Core_Opt**_tmp68B;_LL493: if(*((int*)_tmp681)!= 2)goto _LL495;_tmp682=((
struct Cyc_Absyn_Less_kb_struct*)_tmp681)->f1;_tmp683=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp681)->f1;_tmp684=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp681)->f2;if((int)_tmp684 != 3)goto _LL495;_LL494:*_tmp683=({struct Cyc_Core_Opt*
_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp68E;_tmp68E.tag=0;_tmp68E.f1=(void*)((void*)3);_tmp68E;});_tmp68D;}));
_tmp68C;});goto _LL496;_LL495: if(*((int*)_tmp681)!= 0)goto _LL497;_tmp685=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp681)->f1;if((int)_tmp685 != 3)goto _LL497;_LL496:
effect=({struct Cyc_List_List*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp690=_cycalloc(sizeof(*
_tmp690));_tmp690[0]=({struct Cyc_Absyn_AccessEff_struct _tmp691;_tmp691.tag=17;
_tmp691.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp692=_cycalloc(
sizeof(*_tmp692));_tmp692[0]=({struct Cyc_Absyn_VarType_struct _tmp693;_tmp693.tag=
1;_tmp693.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp693;});_tmp692;}));_tmp691;});
_tmp690;}));_tmp68F->tl=effect;_tmp68F;});goto _LL492;_LL497: if(*((int*)_tmp681)
!= 2)goto _LL499;_tmp686=((struct Cyc_Absyn_Less_kb_struct*)_tmp681)->f1;_tmp687=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp681)->f1;_tmp688=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp681)->f2;if((int)_tmp688 != 4)goto
_LL499;_LL498:*_tmp687=({struct Cyc_Core_Opt*_tmp694=_cycalloc(sizeof(*_tmp694));
_tmp694->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp695=_cycalloc(
sizeof(*_tmp695));_tmp695[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp696;_tmp696.tag=0;
_tmp696.f1=(void*)((void*)4);_tmp696;});_tmp695;}));_tmp694;});goto _LL49A;_LL499:
if(*((int*)_tmp681)!= 0)goto _LL49B;_tmp689=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp681)->f1;if((int)_tmp689 != 4)goto _LL49B;_LL49A: effect=({struct Cyc_List_List*
_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698[0]=({struct Cyc_Absyn_VarType_struct
_tmp699;_tmp699.tag=1;_tmp699.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp699;});
_tmp698;}));_tmp697->tl=effect;_tmp697;});goto _LL492;_LL49B: if(*((int*)_tmp681)
!= 1)goto _LL49D;_tmp68A=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp681)->f1;
_tmp68B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp681)->f1;
_LL49C:*_tmp68B=({struct Cyc_Core_Opt*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp69B=_cycalloc(sizeof(*_tmp69B));
_tmp69B[0]=({struct Cyc_Absyn_Less_kb_struct _tmp69C;_tmp69C.tag=2;_tmp69C.f1=0;
_tmp69C.f2=(void*)((void*)0);_tmp69C;});_tmp69B;}));_tmp69A;});goto _LL49E;_LL49D:;
_LL49E: effect=({struct Cyc_List_List*_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp69E=_cycalloc(sizeof(*_tmp69E));
_tmp69E[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp69F;_tmp69F.tag=19;_tmp69F.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));
_tmp6A0[0]=({struct Cyc_Absyn_VarType_struct _tmp6A1;_tmp6A1.tag=1;_tmp6A1.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6A1;});_tmp6A0;}));_tmp69F;});_tmp69E;}));
_tmp69D->tl=effect;_tmp69D;});goto _LL492;_LL492:;}}effect=Cyc_List_imp_rev(
effect);{struct Cyc_List_List*ts=_tmp654.free_evars;for(0;ts != 0;ts=ts->tl){void*
_tmp6A2=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL4A0: if((int)_tmp6A2 != 3)goto _LL4A2;
_LL4A1: effect=({struct Cyc_List_List*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6A4=_cycalloc(sizeof(*
_tmp6A4));_tmp6A4[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6A5;_tmp6A5.tag=17;
_tmp6A5.f1=(void*)((void*)ts->hd);_tmp6A5;});_tmp6A4;}));_tmp6A3->tl=effect;
_tmp6A3;});goto _LL49F;_LL4A2: if((int)_tmp6A2 != 4)goto _LL4A4;_LL4A3: effect=({
struct Cyc_List_List*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6->hd=(void*)((
void*)ts->hd);_tmp6A6->tl=effect;_tmp6A6;});goto _LL49F;_LL4A4:;_LL4A5: effect=({
struct Cyc_List_List*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7->hd=(void*)((
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));
_tmp6A8[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6A9;_tmp6A9.tag=19;_tmp6A9.f1=(
void*)((void*)ts->hd);_tmp6A9;});_tmp6A8;}));_tmp6A7->tl=effect;_tmp6A7;});goto
_LL49F;_LL49F:;}}*_tmp5C7=({struct Cyc_Core_Opt*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));
_tmp6AA->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6AB=_cycalloc(
sizeof(*_tmp6AB));_tmp6AB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6AC;_tmp6AC.tag=
18;_tmp6AC.f1=Cyc_List_imp_rev(effect);_tmp6AC;});_tmp6AB;}));_tmp6AA;});}}if(*
_tmp5C5 != 0){struct Cyc_List_List*bs=*_tmp5C5;for(0;bs != 0;bs=bs->tl){void*
_tmp6AD=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp6AE;struct Cyc_Core_Opt**_tmp6AF;struct Cyc_Core_Opt*
_tmp6B0;struct Cyc_Core_Opt**_tmp6B1;void*_tmp6B2;struct Cyc_Core_Opt*_tmp6B3;
struct Cyc_Core_Opt**_tmp6B4;void*_tmp6B5;struct Cyc_Core_Opt*_tmp6B6;struct Cyc_Core_Opt**
_tmp6B7;void*_tmp6B8;void*_tmp6B9;_LL4A7: if(*((int*)_tmp6AD)!= 1)goto _LL4A9;
_tmp6AE=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6AD)->f1;_tmp6AF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp6AD)->f1;_LL4A8:({struct Cyc_Std_String_pa_struct
_tmp6BB;_tmp6BB.tag=0;_tmp6BB.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name;{
void*_tmp6BA[1]={& _tmp6BB};Cyc_Tcutil_warn(loc,_tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof(unsigned char),47),_tag_arr(_tmp6BA,sizeof(void*),1));}});_tmp6B1=_tmp6AF;
goto _LL4AA;_LL4A9: if(*((int*)_tmp6AD)!= 2)goto _LL4AB;_tmp6B0=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6AD)->f1;_tmp6B1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6AD)->f1;_tmp6B2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f2;if((
int)_tmp6B2 != 1)goto _LL4AB;_LL4AA: _tmp6B4=_tmp6B1;goto _LL4AC;_LL4AB: if(*((int*)
_tmp6AD)!= 2)goto _LL4AD;_tmp6B3=((struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f1;
_tmp6B4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f1;
_tmp6B5=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f2;if((int)_tmp6B5 != 
0)goto _LL4AD;_LL4AC:*_tmp6B4=({struct Cyc_Core_Opt*_tmp6BC=_cycalloc(sizeof(*
_tmp6BC));_tmp6BC->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6BD=
_cycalloc(sizeof(*_tmp6BD));_tmp6BD[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6BE;
_tmp6BE.tag=0;_tmp6BE.f1=(void*)((void*)2);_tmp6BE;});_tmp6BD;}));_tmp6BC;});
goto _LL4A6;_LL4AD: if(*((int*)_tmp6AD)!= 2)goto _LL4AF;_tmp6B6=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6AD)->f1;_tmp6B7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6AD)->f1;_tmp6B8=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f2;
_LL4AE:*_tmp6B7=({struct Cyc_Core_Opt*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));
_tmp6C0[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6C1;_tmp6C1.tag=0;_tmp6C1.f1=(void*)
_tmp6B8;_tmp6C1;});_tmp6C0;}));_tmp6BF;});goto _LL4A6;_LL4AF: if(*((int*)_tmp6AD)
!= 0)goto _LL4B1;_tmp6B9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp6AD)->f1;if((
int)_tmp6B9 != 1)goto _LL4B1;_LL4B0:({void*_tmp6C2[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("function's can't abstract M types",sizeof(unsigned char),34),_tag_arr(
_tmp6C2,sizeof(void*),0));});goto _LL4A6;_LL4B1:;_LL4B2: goto _LL4A6;_LL4A6:;}}
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(_tmp654.kind_env,*_tmp5C5);_tmp654.free_vars=
Cyc_Tcutil_remove_bound_tvars(_tmp654.free_vars,*_tmp5C5);{struct Cyc_List_List*
tvs=_tmp654.free_vars;for(0;tvs != 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,(struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=
_tmp654.free_evars;for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.free_evars,(void*)evs->hd);}}goto _LL408;}}_LL425: if(_tmp5A5 <= (void*)3?1:*((
int*)_tmp5A5)!= 9)goto _LL427;_tmp5CE=((struct Cyc_Absyn_TupleType_struct*)_tmp5A5)->f1;
_LL426: for(0;_tmp5CE != 0;_tmp5CE=_tmp5CE->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,(*((struct _tuple4*)_tmp5CE->hd)).f2);}goto _LL408;_LL427:
if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 11)goto _LL429;_tmp5CF=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp5A5)->f1;_tmp5D0=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5A5)->f2;_LL428:{struct _RegionHandle _tmp6C4=_new_region("aprev_rgn");struct
_RegionHandle*aprev_rgn=& _tmp6C4;_push_region(aprev_rgn);{struct Cyc_List_List*
prev_fields=0;for(0;_tmp5D0 != 0;_tmp5D0=_tmp5D0->tl){struct Cyc_Absyn_Aggrfield
_tmp6C6;struct _tagged_arr*_tmp6C7;struct Cyc_Absyn_Tqual _tmp6C8;void*_tmp6C9;
struct Cyc_Absyn_Exp*_tmp6CA;struct Cyc_List_List*_tmp6CB;struct Cyc_Absyn_Aggrfield*
_tmp6C5=(struct Cyc_Absyn_Aggrfield*)_tmp5D0->hd;_tmp6C6=*_tmp6C5;_tmp6C7=_tmp6C6.name;
_tmp6C8=_tmp6C6.tq;_tmp6C9=(void*)_tmp6C6.type;_tmp6CA=_tmp6C6.width;_tmp6CB=
_tmp6C6.attributes;if(((int(*)(int(*compare)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,
prev_fields,_tmp6C7))({struct Cyc_Std_String_pa_struct _tmp6CD;_tmp6CD.tag=0;
_tmp6CD.f1=(struct _tagged_arr)*_tmp6C7;{void*_tmp6CC[1]={& _tmp6CD};Cyc_Tcutil_terr(
loc,_tag_arr("duplicate field %s",sizeof(unsigned char),19),_tag_arr(_tmp6CC,
sizeof(void*),1));}});if(Cyc_Std_strcmp(*_tmp6C7,_tag_arr("",sizeof(
unsigned char),1))!= 0)prev_fields=({struct Cyc_List_List*_tmp6CE=_region_malloc(
aprev_rgn,sizeof(*_tmp6CE));_tmp6CE->hd=_tmp6C7;_tmp6CE->tl=prev_fields;_tmp6CE;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,_tmp6C9);if(_tmp5CF
== (void*)1?!Cyc_Tcutil_bits_only(_tmp6C9): 0)({struct Cyc_Std_String_pa_struct
_tmp6D0;_tmp6D0.tag=0;_tmp6D0.f1=(struct _tagged_arr)*_tmp6C7;{void*_tmp6CF[1]={&
_tmp6D0};Cyc_Tcutil_terr(loc,_tag_arr("union member %s has a non-integral type",
sizeof(unsigned char),40),_tag_arr(_tmp6CF,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(
loc,te,_tmp6C9,_tmp6CA,_tmp6C7);Cyc_Tcutil_check_field_atts(loc,_tmp6C7,_tmp6CB);}};
_pop_region(aprev_rgn);}goto _LL408;_LL429: if(_tmp5A5 <= (void*)3?1:*((int*)
_tmp5A5)!= 10)goto _LL42B;_tmp5D1=((struct Cyc_Absyn_AggrType_struct*)_tmp5A5)->f1;
_tmp5D2=(void*)_tmp5D1.aggr_info;_tmp5D3=(void**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp5A5)->f1).aggr_info;_tmp5D4=_tmp5D1.targs;_tmp5D5=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp5A5)->f1).targs;_LL42A:{void*_tmp6D1=*
_tmp5D3;void*_tmp6D2;struct _tuple1*_tmp6D3;struct Cyc_Absyn_Aggrdecl**_tmp6D4;
struct Cyc_Absyn_Aggrdecl*_tmp6D5;_LL4B4: if(*((int*)_tmp6D1)!= 0)goto _LL4B6;
_tmp6D2=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp6D1)->f1;_tmp6D3=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp6D1)->f2;_LL4B5: {struct Cyc_Absyn_Aggrdecl**
adp;{struct _handler_cons _tmp6D6;_push_handler(& _tmp6D6);{int _tmp6D8=0;if(setjmp(
_tmp6D6.handler))_tmp6D8=1;if(!_tmp6D8){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,
_tmp6D3);*_tmp5D3=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp6D9=_cycalloc(
sizeof(*_tmp6D9));_tmp6D9[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp6DA;_tmp6DA.tag=
1;_tmp6DA.f1=adp;_tmp6DA;});_tmp6D9;});;_pop_handler();}else{void*_tmp6D7=(void*)
_exn_thrown;void*_tmp6DC=_tmp6D7;_LL4B9: if(_tmp6DC != Cyc_Dict_Absent)goto _LL4BB;
_LL4BA: {struct Cyc_Tcenv_Genv*_tmp6DD=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*
_tmp6DE=({struct Cyc_Absyn_Aggrdecl*_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3->kind=(
void*)_tmp6D2;_tmp6E3->sc=(void*)((void*)3);_tmp6E3->name=_tmp6D3;_tmp6E3->tvs=0;
_tmp6E3->exist_vars=0;_tmp6E3->fields=0;_tmp6E3->attributes=0;_tmp6E3;});Cyc_Tc_tcAggrdecl(
te,_tmp6DD,loc,_tmp6DE);adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp6D3);*_tmp5D3=(
void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp6DF=_cycalloc(sizeof(*_tmp6DF));
_tmp6DF[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp6E0;_tmp6E0.tag=1;_tmp6E0.f1=
adp;_tmp6E0;});_tmp6DF;});if(*_tmp5D5 != 0){({struct Cyc_Std_String_pa_struct
_tmp6E2;_tmp6E2.tag=0;_tmp6E2.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6D3);{void*_tmp6E1[1]={& _tmp6E2};Cyc_Tcutil_terr(loc,_tag_arr("declare parameterized type %s before using",
sizeof(unsigned char),43),_tag_arr(_tmp6E1,sizeof(void*),1));}});return cvtenv;}
goto _LL4B8;}_LL4BB:;_LL4BC:(void)_throw(_tmp6DC);_LL4B8:;}}}_tmp6D5=*adp;goto
_LL4B7;}_LL4B6: if(*((int*)_tmp6D1)!= 1)goto _LL4B3;_tmp6D4=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp6D1)->f1;_tmp6D5=*_tmp6D4;_LL4B7: {struct Cyc_List_List*tvs=_tmp6D5->tvs;
struct Cyc_List_List*ts=*_tmp5D5;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_Std_String_pa_struct
_tmp6E5;_tmp6E5.tag=0;_tmp6E5.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6D5->name);{void*_tmp6E4[1]={& _tmp6E5};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for type %s",
sizeof(unsigned char),32),_tag_arr(_tmp6E4,sizeof(void*),1));}});if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6->hd=(void*)e;_tmp6E6->tl=
hidden_ts;_tmp6E6;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*
_tmp5D5=Cyc_List_imp_append(*_tmp5D5,Cyc_List_imp_rev(hidden_ts));}}_LL4B3:;}
goto _LL408;_LL42B: if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 16)goto _LL42D;
_tmp5D6=((struct Cyc_Absyn_TypedefType_struct*)_tmp5A5)->f1;_tmp5D7=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5A5)->f2;_tmp5D8=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp5A5)->f2;_tmp5D9=((struct Cyc_Absyn_TypedefType_struct*)_tmp5A5)->f3;_tmp5DA=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp5A5)->f3;_LL42C: {
struct Cyc_List_List*targs=*_tmp5D8;struct Cyc_Absyn_Typedefdecl*td;{struct
_handler_cons _tmp6E7;_push_handler(& _tmp6E7);{int _tmp6E9=0;if(setjmp(_tmp6E7.handler))
_tmp6E9=1;if(!_tmp6E9){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp5D6);;
_pop_handler();}else{void*_tmp6E8=(void*)_exn_thrown;void*_tmp6EB=_tmp6E8;_LL4BE:
if(_tmp6EB != Cyc_Dict_Absent)goto _LL4C0;_LL4BF:({struct Cyc_Std_String_pa_struct
_tmp6ED;_tmp6ED.tag=0;_tmp6ED.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp5D6);{void*_tmp6EC[1]={& _tmp6ED};Cyc_Tcutil_terr(loc,_tag_arr("unbound typedef name %s",
sizeof(unsigned char),24),_tag_arr(_tmp6EC,sizeof(void*),1));}});return cvtenv;
_LL4C0:;_LL4C1:(void)_throw(_tmp6EB);_LL4BD:;}}}_tmp5D6[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);inst=({struct Cyc_List_List*_tmp6EE=_cycalloc(
sizeof(*_tmp6EE));_tmp6EE->hd=({struct _tuple6*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));
_tmp6EF->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6EF->f2=(void*)ts->hd;_tmp6EF;});
_tmp6EE->tl=inst;_tmp6EE;});}if(ts != 0)({struct Cyc_Std_String_pa_struct _tmp6F1;
_tmp6F1.tag=0;_tmp6F1.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp5D6);{
void*_tmp6F0[1]={& _tmp6F1};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for typedef %s",
sizeof(unsigned char),35),_tag_arr(_tmp6F0,sizeof(void*),1));}});if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2->hd=(void*)e;_tmp6F2->tl=
hidden_ts;_tmp6F2;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);
inst=({struct Cyc_List_List*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3->hd=({
struct _tuple6*_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp6F4->f2=e;_tmp6F4;});_tmp6F3->tl=inst;_tmp6F3;});}*_tmp5D8=Cyc_List_imp_append(
targs,Cyc_List_imp_rev(hidden_ts));}{void*new_typ=Cyc_Tcutil_substitute(inst,(
void*)td->defn);*_tmp5DA=({struct Cyc_Core_Opt*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));
_tmp6F5->v=(void*)new_typ;_tmp6F5;});goto _LL408;}}}_LL42D: if((int)_tmp5A5 != 2)
goto _LL42F;_LL42E: goto _LL408;_LL42F: if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 
15)goto _LL431;_tmp5DB=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp5A5)->f1;
_LL430: _tmp5DC=_tmp5DB;goto _LL432;_LL431: if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)
!= 17)goto _LL433;_tmp5DC=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp5A5)->f1;
_LL432: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp5DC);goto
_LL408;_LL433: if(_tmp5A5 <= (void*)3?1:*((int*)_tmp5A5)!= 19)goto _LL435;_tmp5DD=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp5A5)->f1;_LL434: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp5DD);goto _LL408;_LL435: if(_tmp5A5 <= (void*)3?1:*((int*)
_tmp5A5)!= 18)goto _LL408;_tmp5DE=((struct Cyc_Absyn_JoinEff_struct*)_tmp5A5)->f1;
_LL436: for(0;_tmp5DE != 0;_tmp5DE=_tmp5DE->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)4,(void*)_tmp5DE->hd);}goto _LL408;_LL408:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind))({struct Cyc_Std_String_pa_struct _tmp6F9;
_tmp6F9.tag=0;_tmp6F9.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
expected_kind);{struct Cyc_Std_String_pa_struct _tmp6F8;_tmp6F8.tag=0;_tmp6F8.f1=(
struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t));{struct Cyc_Std_String_pa_struct
_tmp6F7;_tmp6F7.tag=0;_tmp6F7.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp6F6[3]={& _tmp6F7,& _tmp6F8,& _tmp6F9};Cyc_Tcutil_terr(loc,_tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof(unsigned char),51),_tag_arr(_tmp6F6,sizeof(void*),3));}}}});return cvtenv;}
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp6FA=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp703;_tmp703.kind_env=kind_env;_tmp703.free_vars=
0;_tmp703.free_evars=0;_tmp703.generalize_evars=generalize_evars;_tmp703.fn_result=
0;_tmp703;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp6FA.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp6FA.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp6FA.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp6FB=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp6FC;struct Cyc_Core_Opt**_tmp6FD;_LL4C3: if(_tmp6FB <= (void*)3?1:*((int*)
_tmp6FB)!= 0)goto _LL4C5;_tmp6FC=((struct Cyc_Absyn_Evar_struct*)_tmp6FB)->f4;
_tmp6FD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp6FB)->f4;
_LL4C4: if(*_tmp6FD == 0)*_tmp6FD=({struct Cyc_Core_Opt*_tmp6FE=_cycalloc(sizeof(*
_tmp6FE));_tmp6FE->v=kind_env;_tmp6FE;});else{struct Cyc_List_List*_tmp6FF=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp6FD))->v;struct Cyc_List_List*
_tmp700=0;for(0;_tmp6FF != 0;_tmp6FF=_tmp6FF->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp6FF->hd))_tmp700=({struct Cyc_List_List*_tmp701=_cycalloc(sizeof(*_tmp701));
_tmp701->hd=(struct Cyc_Absyn_Tvar*)_tmp6FF->hd;_tmp701->tl=_tmp700;_tmp701;});}*
_tmp6FD=({struct Cyc_Core_Opt*_tmp702=_cycalloc(sizeof(*_tmp702));_tmp702->v=
_tmp700;_tmp702;});}goto _LL4C2;_LL4C5:;_LL4C6: goto _LL4C2;_LL4C2:;}}return _tmp6FA;}
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_Tcutil_CVTEnv _tmp704=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp705=_tmp704.free_vars;struct Cyc_List_List*
_tmp706=_tmp704.free_evars;{struct Cyc_List_List*x=_tmp705;for(0;x != 0;x=x->tl){
void*_tmp707=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp708;struct Cyc_Core_Opt**_tmp709;struct Cyc_Core_Opt*
_tmp70A;struct Cyc_Core_Opt**_tmp70B;void*_tmp70C;struct Cyc_Core_Opt*_tmp70D;
struct Cyc_Core_Opt**_tmp70E;void*_tmp70F;struct Cyc_Core_Opt*_tmp710;struct Cyc_Core_Opt**
_tmp711;void*_tmp712;void*_tmp713;_LL4C8: if(*((int*)_tmp707)!= 1)goto _LL4CA;
_tmp708=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp707)->f1;_tmp709=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp707)->f1;_LL4C9: _tmp70B=_tmp709;goto
_LL4CB;_LL4CA: if(*((int*)_tmp707)!= 2)goto _LL4CC;_tmp70A=((struct Cyc_Absyn_Less_kb_struct*)
_tmp707)->f1;_tmp70B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp707)->f1;_tmp70C=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f2;if((
int)_tmp70C != 1)goto _LL4CC;_LL4CB: _tmp70E=_tmp70B;goto _LL4CD;_LL4CC: if(*((int*)
_tmp707)!= 2)goto _LL4CE;_tmp70D=((struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f1;
_tmp70E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f1;
_tmp70F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f2;if((int)_tmp70F != 
0)goto _LL4CE;_LL4CD:*_tmp70E=({struct Cyc_Core_Opt*_tmp714=_cycalloc(sizeof(*
_tmp714));_tmp714->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp715=
_cycalloc(sizeof(*_tmp715));_tmp715[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp716;
_tmp716.tag=0;_tmp716.f1=(void*)((void*)2);_tmp716;});_tmp715;}));_tmp714;});
goto _LL4C7;_LL4CE: if(*((int*)_tmp707)!= 2)goto _LL4D0;_tmp710=((struct Cyc_Absyn_Less_kb_struct*)
_tmp707)->f1;_tmp711=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp707)->f1;_tmp712=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f2;
_LL4CF:*_tmp711=({struct Cyc_Core_Opt*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp718=_cycalloc(sizeof(*_tmp718));
_tmp718[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp719;_tmp719.tag=0;_tmp719.f1=(void*)
_tmp712;_tmp719;});_tmp718;}));_tmp717;});goto _LL4C7;_LL4D0: if(*((int*)_tmp707)
!= 0)goto _LL4D2;_tmp713=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp707)->f1;if((
int)_tmp713 != 1)goto _LL4D2;_LL4D1:({struct Cyc_Std_String_pa_struct _tmp71B;
_tmp71B.tag=0;_tmp71B.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd);{void*_tmp71A[1]={& _tmp71B};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s cannot have kind M",
sizeof(unsigned char),36),_tag_arr(_tmp71A,sizeof(void*),1));}});goto _LL4C7;
_LL4D2:;_LL4D3: goto _LL4C7;_LL4C7:;}}if(_tmp705 != 0?1: _tmp706 != 0){{void*_tmp71C=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp71D;struct Cyc_List_List*_tmp71E;
struct Cyc_List_List**_tmp71F;struct Cyc_Core_Opt*_tmp720;void*_tmp721;struct Cyc_List_List*
_tmp722;int _tmp723;struct Cyc_Absyn_VarargInfo*_tmp724;struct Cyc_List_List*
_tmp725;struct Cyc_List_List*_tmp726;_LL4D5: if(_tmp71C <= (void*)3?1:*((int*)
_tmp71C)!= 8)goto _LL4D7;_tmp71D=((struct Cyc_Absyn_FnType_struct*)_tmp71C)->f1;
_tmp71E=_tmp71D.tvars;_tmp71F=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp71C)->f1).tvars;_tmp720=_tmp71D.effect;_tmp721=(void*)_tmp71D.ret_typ;
_tmp722=_tmp71D.args;_tmp723=_tmp71D.c_varargs;_tmp724=_tmp71D.cyc_varargs;
_tmp725=_tmp71D.rgn_po;_tmp726=_tmp71D.attributes;_LL4D6: if(*_tmp71F == 0){*
_tmp71F=_tmp705;_tmp705=0;}goto _LL4D4;_LL4D7:;_LL4D8: goto _LL4D4;_LL4D4:;}if(
_tmp705 != 0)({struct Cyc_Std_String_pa_struct _tmp728;_tmp728.tag=0;_tmp728.f1=(
struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp705->hd)->name;{void*_tmp727[1]={&
_tmp728};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s",sizeof(
unsigned char),25),_tag_arr(_tmp727,sizeof(void*),1));}});if(_tmp706 != 0)for(0;
_tmp706 != 0;_tmp706=_tmp706->tl){void*e=(void*)_tmp706->hd;void*_tmp729=Cyc_Tcutil_typ_kind(
e);_LL4DA: if((int)_tmp729 != 3)goto _LL4DC;_LL4DB: if(!Cyc_Tcutil_unify(e,(void*)2))({
void*_tmp72A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with heap!",sizeof(unsigned char),28),_tag_arr(
_tmp72A,sizeof(void*),0));});goto _LL4D9;_LL4DC: if((int)_tmp729 != 4)goto _LL4DE;
_LL4DD: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp72B[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(unsigned char),26),_tag_arr(_tmp72B,sizeof(void*),0));});goto _LL4D9;
_LL4DE:;_LL4DF:({struct Cyc_Std_String_pa_struct _tmp72E;_tmp72E.tag=0;_tmp72E.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp72D;_tmp72D.tag=0;_tmp72D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{
void*_tmp72C[2]={& _tmp72D,& _tmp72E};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(unsigned char),52),_tag_arr(_tmp72C,sizeof(void*),2));}}});goto _LL4D9;
_LL4D9:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmp72F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp730;struct Cyc_List_List*_tmp731;struct Cyc_Core_Opt*
_tmp732;void*_tmp733;struct Cyc_List_List*_tmp734;int _tmp735;struct Cyc_Absyn_VarargInfo*
_tmp736;struct Cyc_List_List*_tmp737;struct Cyc_List_List*_tmp738;_LL4E1: if(
_tmp72F <= (void*)3?1:*((int*)_tmp72F)!= 8)goto _LL4E3;_tmp730=((struct Cyc_Absyn_FnType_struct*)
_tmp72F)->f1;_tmp731=_tmp730.tvars;_tmp732=_tmp730.effect;_tmp733=(void*)_tmp730.ret_typ;
_tmp734=_tmp730.args;_tmp735=_tmp730.c_varargs;_tmp736=_tmp730.cyc_varargs;
_tmp737=_tmp730.rgn_po;_tmp738=_tmp730.attributes;_LL4E2: fd->tvs=_tmp731;fd->effect=
_tmp732;goto _LL4E0;_LL4E3:;_LL4E4:({void*_tmp739[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_fndecl_valid_type: not a FnType",
sizeof(unsigned char),38),_tag_arr(_tmp739,sizeof(void*),0));});_LL4E0:;}{struct
_RegionHandle _tmp73A=_new_region("r");struct _RegionHandle*r=& _tmp73A;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct _tuple14*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct _tuple14*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,_tag_arr("function declaration has repeated parameter",sizeof(
unsigned char),44));;_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp73B=
_cycalloc(sizeof(*_tmp73B));_tmp73B->v=(void*)t;_tmp73B;});}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp73C=Cyc_Tcutil_check_valid_type(loc,te,bound_tvars,expected_kind,0,t);struct
Cyc_List_List*_tmp73D=Cyc_Tcutil_remove_bound_tvars(_tmp73C.free_vars,
bound_tvars);struct Cyc_List_List*_tmp73E=_tmp73C.free_evars;{struct Cyc_List_List*
fs=_tmp73D;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp73F=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;({struct Cyc_Std_String_pa_struct _tmp742;_tmp742.tag=0;_tmp742.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp741;_tmp741.tag=0;_tmp741.f1=(struct _tagged_arr)*_tmp73F;{void*_tmp740[2]={&
_tmp741,& _tmp742};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s in type %s",
sizeof(unsigned char),36),_tag_arr(_tmp740,sizeof(void*),2));}}});}}if(!
allow_evars?_tmp73E != 0: 0)for(0;_tmp73E != 0;_tmp73E=_tmp73E->tl){void*e=(void*)
_tmp73E->hd;void*_tmp743=Cyc_Tcutil_typ_kind(e);_LL4E6: if((int)_tmp743 != 3)goto
_LL4E8;_LL4E7: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp744[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(unsigned char),28),_tag_arr(_tmp744,sizeof(void*),0));});goto _LL4E5;
_LL4E8: if((int)_tmp743 != 4)goto _LL4EA;_LL4E9: if(!Cyc_Tcutil_unify(e,(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp746;_tmp746.tag=18;_tmp746.f1=0;_tmp746;});
_tmp745;})))({void*_tmp747[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",sizeof(unsigned char),
26),_tag_arr(_tmp747,sizeof(void*),0));});goto _LL4E5;_LL4EA:;_LL4EB:({struct Cyc_Std_String_pa_struct
_tmp74A;_tmp74A.tag=0;_tmp74A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp749;_tmp749.tag=0;_tmp749.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(e);{void*_tmp748[2]={& _tmp749,& _tmp74A};Cyc_Tcutil_terr(
loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",sizeof(
unsigned char),52),_tag_arr(_tmp748,sizeof(void*),2));}}});goto _LL4E5;_LL4E5:;}}
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=
Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*
tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _tagged_arr(*a2string)(void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0)({struct Cyc_Std_String_pa_struct _tmp74D;_tmp74D.tag=0;_tmp74D.f1=(
struct _tagged_arr)a2string((void*)vs->hd);{struct Cyc_Std_String_pa_struct _tmp74C;
_tmp74C.tag=0;_tmp74C.f1=(struct _tagged_arr)msg;{void*_tmp74B[2]={& _tmp74C,&
_tmp74D};Cyc_Tcutil_terr(loc,_tag_arr("%s: %s",sizeof(unsigned char),7),_tag_arr(
_tmp74B,sizeof(void*),2));}}});}}}static struct _tagged_arr Cyc_Tcutil_strptr2string(
struct _tagged_arr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _tagged_arr msg){((void(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr(*a2string)(struct _tagged_arr*),struct _tagged_arr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Std_zstrptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){((void(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr(*a2string)(struct Cyc_Absyn_Tvar*),struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,
_tag_arr("duplicate type variable",sizeof(unsigned char),24));}struct _tuple19{
struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple20{struct Cyc_List_List*f1;void*
f2;};struct _tuple21{struct Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){if(Cyc_Std_strcmp(*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,
_tag_arr("",sizeof(unsigned char),1))!= 0)fields=({struct Cyc_List_List*_tmp74E=
_cycalloc(sizeof(*_tmp74E));_tmp74E->hd=({struct _tuple19*_tmp74F=_cycalloc(
sizeof(*_tmp74F));_tmp74F->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp74F->f2=
0;_tmp74F;});_tmp74E->tl=fields;_tmp74E;});}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple20 _tmp751;struct Cyc_List_List*_tmp752;void*
_tmp753;struct _tuple20*_tmp750=(struct _tuple20*)des->hd;_tmp751=*_tmp750;_tmp752=
_tmp751.f1;_tmp753=_tmp751.f2;if(_tmp752 == 0){struct Cyc_List_List*_tmp754=fields;
for(0;_tmp754 != 0;_tmp754=_tmp754->tl){if(!(*((struct _tuple19*)_tmp754->hd)).f2){(*((
struct _tuple19*)_tmp754->hd)).f2=1;(*((struct _tuple20*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755->hd=(void*)((
void*)({struct Cyc_Absyn_FieldName_struct*_tmp756=_cycalloc(sizeof(*_tmp756));
_tmp756[0]=({struct Cyc_Absyn_FieldName_struct _tmp757;_tmp757.tag=1;_tmp757.f1=((*((
struct _tuple19*)_tmp754->hd)).f1)->name;_tmp757;});_tmp756;}));_tmp755->tl=0;
_tmp755;});ans=({struct Cyc_List_List*_tmp758=_region_malloc(rgn,sizeof(*_tmp758));
_tmp758->hd=({struct _tuple21*_tmp759=_region_malloc(rgn,sizeof(*_tmp759));
_tmp759->f1=(*((struct _tuple19*)_tmp754->hd)).f1;_tmp759->f2=_tmp753;_tmp759;});
_tmp758->tl=ans;_tmp758;});break;}}if(_tmp754 == 0)({void*_tmp75A[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("too many arguments to struct",sizeof(unsigned char),29),_tag_arr(
_tmp75A,sizeof(void*),0));});}else{if(_tmp752->tl != 0)({void*_tmp75B[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("multiple designators are not supported",sizeof(unsigned char),39),
_tag_arr(_tmp75B,sizeof(void*),0));});else{void*_tmp75C=(void*)_tmp752->hd;
struct _tagged_arr*_tmp75D;_LL4ED: if(*((int*)_tmp75C)!= 0)goto _LL4EF;_LL4EE:({
void*_tmp75E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array designator used in argument to struct",
sizeof(unsigned char),44),_tag_arr(_tmp75E,sizeof(void*),0));});goto _LL4EC;
_LL4EF: if(*((int*)_tmp75C)!= 1)goto _LL4EC;_tmp75D=((struct Cyc_Absyn_FieldName_struct*)
_tmp75C)->f1;_LL4F0: {struct Cyc_List_List*_tmp75F=fields;for(0;_tmp75F != 0;
_tmp75F=_tmp75F->tl){if(Cyc_Std_zstrptrcmp(_tmp75D,((*((struct _tuple19*)_tmp75F->hd)).f1)->name)
== 0){if((*((struct _tuple19*)_tmp75F->hd)).f2)({struct Cyc_Std_String_pa_struct
_tmp761;_tmp761.tag=0;_tmp761.f1=(struct _tagged_arr)*_tmp75D;{void*_tmp760[1]={&
_tmp761};Cyc_Tcutil_terr(loc,_tag_arr("field %s has already been used as an argument",
sizeof(unsigned char),46),_tag_arr(_tmp760,sizeof(void*),1));}});(*((struct
_tuple19*)_tmp75F->hd)).f2=1;ans=({struct Cyc_List_List*_tmp762=_region_malloc(
rgn,sizeof(*_tmp762));_tmp762->hd=({struct _tuple21*_tmp763=_region_malloc(rgn,
sizeof(*_tmp763));_tmp763->f1=(*((struct _tuple19*)_tmp75F->hd)).f1;_tmp763->f2=
_tmp753;_tmp763;});_tmp762->tl=ans;_tmp762;});break;}}if(_tmp75F == 0)({struct Cyc_Std_String_pa_struct
_tmp765;_tmp765.tag=0;_tmp765.f1=(struct _tagged_arr)*_tmp75D;{void*_tmp764[1]={&
_tmp765};Cyc_Tcutil_terr(loc,_tag_arr("bad field designator %s",sizeof(
unsigned char),24),_tag_arr(_tmp764,sizeof(void*),1));}});goto _LL4EC;}_LL4EC:;}}}
for(0;fields != 0;fields=fields->tl){if(!(*((struct _tuple19*)fields->hd)).f2){({
void*_tmp766[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too few arguments to struct",
sizeof(unsigned char),28),_tag_arr(_tmp766,sizeof(void*),0));});break;}}return((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp767=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp768;void*_tmp769;struct Cyc_Absyn_Conref*_tmp76A;_LL4F2: if(_tmp767 <= (void*)3?
1:*((int*)_tmp767)!= 4)goto _LL4F4;_tmp768=((struct Cyc_Absyn_PointerType_struct*)
_tmp767)->f1;_tmp769=(void*)_tmp768.elt_typ;_tmp76A=_tmp768.bounds;_LL4F3: {
struct Cyc_Absyn_Conref*_tmp76B=Cyc_Absyn_compress_conref(_tmp76A);void*_tmp76C=(
void*)(Cyc_Absyn_compress_conref(_tmp76B))->v;void*_tmp76D;_LL4F7: if(_tmp76C <= (
void*)1?1:*((int*)_tmp76C)!= 0)goto _LL4F9;_tmp76D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp76C)->f1;if((int)_tmp76D != 0)goto _LL4F9;_LL4F8:*elt_typ_dest=_tmp769;return 1;
_LL4F9: if((int)_tmp76C != 0)goto _LL4FB;_LL4FA:(void*)(_tmp76B->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp76F;_tmp76F.tag=0;_tmp76F.f1=(void*)((void*)
0);_tmp76F;});_tmp76E;})));*elt_typ_dest=_tmp769;return 1;_LL4FB:;_LL4FC: return 0;
_LL4F6:;}_LL4F4:;_LL4F5: return 0;_LL4F1:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple8 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple8 bogus_ans=({struct _tuple8 _tmp7AD;_tmp7AD.f1=0;
_tmp7AD.f2=(void*)2;_tmp7AD;});void*_tmp770=(void*)e->r;struct _tuple1*_tmp771;
void*_tmp772;struct Cyc_Absyn_Exp*_tmp773;struct _tagged_arr*_tmp774;struct Cyc_Absyn_Exp*
_tmp775;struct _tagged_arr*_tmp776;struct Cyc_Absyn_Exp*_tmp777;struct Cyc_Absyn_Exp*
_tmp778;struct Cyc_Absyn_Exp*_tmp779;_LL4FE: if(*((int*)_tmp770)!= 1)goto _LL500;
_tmp771=((struct Cyc_Absyn_Var_e_struct*)_tmp770)->f1;_tmp772=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp770)->f2;_LL4FF: {void*_tmp77A=_tmp772;struct Cyc_Absyn_Vardecl*_tmp77B;
struct Cyc_Absyn_Vardecl*_tmp77C;struct Cyc_Absyn_Vardecl*_tmp77D;struct Cyc_Absyn_Vardecl*
_tmp77E;_LL50B: if((int)_tmp77A != 0)goto _LL50D;_LL50C: goto _LL50E;_LL50D: if(
_tmp77A <= (void*)1?1:*((int*)_tmp77A)!= 1)goto _LL50F;_LL50E: return bogus_ans;
_LL50F: if(_tmp77A <= (void*)1?1:*((int*)_tmp77A)!= 0)goto _LL511;_tmp77B=((struct
Cyc_Absyn_Global_b_struct*)_tmp77A)->f1;_LL510: {void*_tmp77F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL518: if(_tmp77F <= (void*)
3?1:*((int*)_tmp77F)!= 7)goto _LL51A;_LL519: return({struct _tuple8 _tmp780;_tmp780.f1=
1;_tmp780.f2=(void*)2;_tmp780;});_LL51A:;_LL51B: return({struct _tuple8 _tmp781;
_tmp781.f1=(_tmp77B->tq).q_const;_tmp781.f2=(void*)2;_tmp781;});_LL517:;}_LL511:
if(_tmp77A <= (void*)1?1:*((int*)_tmp77A)!= 3)goto _LL513;_tmp77C=((struct Cyc_Absyn_Local_b_struct*)
_tmp77A)->f1;_LL512: {void*_tmp782=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL51D: if(_tmp782 <= (void*)3?1:*((int*)_tmp782)!= 7)
goto _LL51F;_LL51E: return({struct _tuple8 _tmp783;_tmp783.f1=1;_tmp783.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp77C->rgn))->v;_tmp783;});_LL51F:;_LL520:
_tmp77C->escapes=1;return({struct _tuple8 _tmp784;_tmp784.f1=(_tmp77C->tq).q_const;
_tmp784.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp77C->rgn))->v;_tmp784;});
_LL51C:;}_LL513: if(_tmp77A <= (void*)1?1:*((int*)_tmp77A)!= 4)goto _LL515;_tmp77D=((
struct Cyc_Absyn_Pat_b_struct*)_tmp77A)->f1;_LL514: _tmp77E=_tmp77D;goto _LL516;
_LL515: if(_tmp77A <= (void*)1?1:*((int*)_tmp77A)!= 2)goto _LL50A;_tmp77E=((struct
Cyc_Absyn_Param_b_struct*)_tmp77A)->f1;_LL516: _tmp77E->escapes=1;return({struct
_tuple8 _tmp785;_tmp785.f1=(_tmp77E->tq).q_const;_tmp785.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp77E->rgn))->v;_tmp785;});_LL50A:;}_LL500: if(*((int*)_tmp770)!= 
21)goto _LL502;_tmp773=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp770)->f1;
_tmp774=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp770)->f2;_LL501: {void*
_tmp786=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp773->topt))->v);
struct Cyc_List_List*_tmp787;struct Cyc_Absyn_AggrInfo _tmp788;void*_tmp789;struct
Cyc_Absyn_Aggrdecl**_tmp78A;struct Cyc_Absyn_Aggrdecl*_tmp78B;_LL522: if(_tmp786 <= (
void*)3?1:*((int*)_tmp786)!= 11)goto _LL524;_tmp787=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp786)->f2;_LL523: {struct Cyc_Absyn_Aggrfield*_tmp78C=Cyc_Absyn_lookup_field(
_tmp787,_tmp774);if(_tmp78C != 0?_tmp78C->width != 0: 0)return({struct _tuple8
_tmp78D;_tmp78D.f1=(_tmp78C->tq).q_const;_tmp78D.f2=(Cyc_Tcutil_addressof_props(
te,_tmp773)).f2;_tmp78D;});return bogus_ans;}_LL524: if(_tmp786 <= (void*)3?1:*((
int*)_tmp786)!= 10)goto _LL526;_tmp788=((struct Cyc_Absyn_AggrType_struct*)_tmp786)->f1;
_tmp789=(void*)_tmp788.aggr_info;if(*((int*)_tmp789)!= 1)goto _LL526;_tmp78A=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp789)->f1;_tmp78B=*_tmp78A;_LL525: {struct
Cyc_Absyn_Aggrfield*_tmp78E=Cyc_Absyn_lookup_decl_field(_tmp78B,_tmp774);if(
_tmp78E != 0?_tmp78E->width != 0: 0)return({struct _tuple8 _tmp78F;_tmp78F.f1=(
_tmp78E->tq).q_const;_tmp78F.f2=(Cyc_Tcutil_addressof_props(te,_tmp773)).f2;
_tmp78F;});return bogus_ans;}_LL526:;_LL527: return bogus_ans;_LL521:;}_LL502: if(*((
int*)_tmp770)!= 22)goto _LL504;_tmp775=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp770)->f1;_tmp776=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp770)->f2;_LL503: {
void*_tmp790=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp775->topt))->v);struct Cyc_Absyn_PtrInfo _tmp791;void*_tmp792;void*_tmp793;
_LL529: if(_tmp790 <= (void*)3?1:*((int*)_tmp790)!= 4)goto _LL52B;_tmp791=((struct
Cyc_Absyn_PointerType_struct*)_tmp790)->f1;_tmp792=(void*)_tmp791.elt_typ;
_tmp793=(void*)_tmp791.rgn_typ;_LL52A: {struct Cyc_Absyn_Aggrfield*finfo;{void*
_tmp794=Cyc_Tcutil_compress(_tmp792);struct Cyc_List_List*_tmp795;struct Cyc_Absyn_AggrInfo
_tmp796;void*_tmp797;struct Cyc_Absyn_Aggrdecl**_tmp798;struct Cyc_Absyn_Aggrdecl*
_tmp799;_LL52E: if(_tmp794 <= (void*)3?1:*((int*)_tmp794)!= 11)goto _LL530;_tmp795=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp794)->f2;_LL52F: finfo=Cyc_Absyn_lookup_field(
_tmp795,_tmp776);goto _LL52D;_LL530: if(_tmp794 <= (void*)3?1:*((int*)_tmp794)!= 10)
goto _LL532;_tmp796=((struct Cyc_Absyn_AggrType_struct*)_tmp794)->f1;_tmp797=(void*)
_tmp796.aggr_info;if(*((int*)_tmp797)!= 1)goto _LL532;_tmp798=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp797)->f1;_tmp799=*_tmp798;_LL531: finfo=Cyc_Absyn_lookup_decl_field(_tmp799,
_tmp776);goto _LL52D;_LL532:;_LL533: return bogus_ans;_LL52D:;}if(finfo != 0?finfo->width
!= 0: 0)return({struct _tuple8 _tmp79A;_tmp79A.f1=(finfo->tq).q_const;_tmp79A.f2=
_tmp793;_tmp79A;});return bogus_ans;}_LL52B:;_LL52C: return bogus_ans;_LL528:;}
_LL504: if(*((int*)_tmp770)!= 20)goto _LL506;_tmp777=((struct Cyc_Absyn_Deref_e_struct*)
_tmp770)->f1;_LL505: {void*_tmp79B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp777->topt))->v);struct Cyc_Absyn_PtrInfo _tmp79C;void*_tmp79D;
struct Cyc_Absyn_Tqual _tmp79E;_LL535: if(_tmp79B <= (void*)3?1:*((int*)_tmp79B)!= 4)
goto _LL537;_tmp79C=((struct Cyc_Absyn_PointerType_struct*)_tmp79B)->f1;_tmp79D=(
void*)_tmp79C.rgn_typ;_tmp79E=_tmp79C.tq;_LL536: return({struct _tuple8 _tmp79F;
_tmp79F.f1=_tmp79E.q_const;_tmp79F.f2=_tmp79D;_tmp79F;});_LL537:;_LL538: return
bogus_ans;_LL534:;}_LL506: if(*((int*)_tmp770)!= 23)goto _LL508;_tmp778=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp770)->f1;_tmp779=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp770)->f2;_LL507: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp778->topt))->v);void*_tmp7A0=t;struct Cyc_List_List*_tmp7A1;
struct Cyc_Absyn_PtrInfo _tmp7A2;void*_tmp7A3;void*_tmp7A4;struct Cyc_Absyn_Tqual
_tmp7A5;struct Cyc_Absyn_Conref*_tmp7A6;struct Cyc_Absyn_Tqual _tmp7A7;_LL53A: if(
_tmp7A0 <= (void*)3?1:*((int*)_tmp7A0)!= 9)goto _LL53C;_tmp7A1=((struct Cyc_Absyn_TupleType_struct*)
_tmp7A0)->f1;_LL53B: {struct _tuple4*_tmp7A8=Cyc_Absyn_lookup_tuple_field(_tmp7A1,(
int)Cyc_Evexp_eval_const_uint_exp(_tmp779));if(_tmp7A8 != 0)return({struct _tuple8
_tmp7A9;_tmp7A9.f1=((*_tmp7A8).f1).q_const;_tmp7A9.f2=(Cyc_Tcutil_addressof_props(
te,_tmp778)).f2;_tmp7A9;});return bogus_ans;}_LL53C: if(_tmp7A0 <= (void*)3?1:*((
int*)_tmp7A0)!= 4)goto _LL53E;_tmp7A2=((struct Cyc_Absyn_PointerType_struct*)
_tmp7A0)->f1;_tmp7A3=(void*)_tmp7A2.elt_typ;_tmp7A4=(void*)_tmp7A2.rgn_typ;
_tmp7A5=_tmp7A2.tq;_tmp7A6=_tmp7A2.bounds;_LL53D: return({struct _tuple8 _tmp7AA;
_tmp7AA.f1=_tmp7A5.q_const;_tmp7AA.f2=_tmp7A4;_tmp7AA;});_LL53E: if(_tmp7A0 <= (
void*)3?1:*((int*)_tmp7A0)!= 7)goto _LL540;_tmp7A7=((struct Cyc_Absyn_ArrayType_struct*)
_tmp7A0)->f2;_LL53F: return({struct _tuple8 _tmp7AB;_tmp7AB.f1=_tmp7A7.q_const;
_tmp7AB.f2=(Cyc_Tcutil_addressof_props(te,_tmp778)).f2;_tmp7AB;});_LL540:;_LL541:
return bogus_ans;_LL539:;}_LL508:;_LL509:({void*_tmp7AC[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("unary & applied to non-lvalue",sizeof(unsigned char),30),_tag_arr(
_tmp7AC,sizeof(void*),0));});return bogus_ans;_LL4FD:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp7AE=Cyc_Tcutil_compress(
e_typ);void*_tmp7AF;struct Cyc_Absyn_Tqual _tmp7B0;_LL543: if(_tmp7AE <= (void*)3?1:*((
int*)_tmp7AE)!= 7)goto _LL545;_tmp7AF=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp7AE)->f1;_tmp7B0=((struct Cyc_Absyn_ArrayType_struct*)_tmp7AE)->f2;_LL544: {
void*_tmp7B2;struct _tuple8 _tmp7B1=Cyc_Tcutil_addressof_props(te,e);_tmp7B2=
_tmp7B1.f2;return Cyc_Absyn_atb_typ(_tmp7AF,_tmp7B2,_tmp7B0,(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp7B3=_cycalloc(sizeof(*_tmp7B3));_tmp7B3[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp7B4;_tmp7B4.tag=0;_tmp7B4.f1=e;_tmp7B4;});_tmp7B3;}));}_LL545:;_LL546: return
e_typ;_LL542:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,
unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(b);{void*
_tmp7B5=(void*)b->v;void*_tmp7B6;void*_tmp7B7;struct Cyc_Absyn_Exp*_tmp7B8;_LL548:
if(_tmp7B5 <= (void*)1?1:*((int*)_tmp7B5)!= 0)goto _LL54A;_tmp7B6=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp7B5)->f1;if((int)_tmp7B6 != 0)goto _LL54A;_LL549:
return;_LL54A: if(_tmp7B5 <= (void*)1?1:*((int*)_tmp7B5)!= 0)goto _LL54C;_tmp7B7=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7B5)->f1;if(_tmp7B7 <= (void*)1?1:*((
int*)_tmp7B7)!= 0)goto _LL54C;_tmp7B8=((struct Cyc_Absyn_Upper_b_struct*)_tmp7B7)->f1;
_LL54B: if(Cyc_Evexp_eval_const_uint_exp(_tmp7B8)<= i)({struct Cyc_Std_Int_pa_struct
_tmp7BB;_tmp7BB.tag=1;_tmp7BB.f1=(int)i;{struct Cyc_Std_Int_pa_struct _tmp7BA;
_tmp7BA.tag=1;_tmp7BA.f1=(int)Cyc_Evexp_eval_const_uint_exp(_tmp7B8);{void*
_tmp7B9[2]={& _tmp7BA,& _tmp7BB};Cyc_Tcutil_terr(loc,_tag_arr("dereference is out of bounds: %d <= %d",
sizeof(unsigned char),39),_tag_arr(_tmp7B9,sizeof(void*),2));}}});return;_LL54C:;
_LL54D:(void*)(b->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp7BC=
_cycalloc(sizeof(*_tmp7BC));_tmp7BC[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp7BD;
_tmp7BD.tag=0;_tmp7BD.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp7BE=
_cycalloc(sizeof(*_tmp7BE));_tmp7BE[0]=({struct Cyc_Absyn_Upper_b_struct _tmp7BF;
_tmp7BF.tag=0;_tmp7BF.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmp7BF;});_tmp7BE;}));
_tmp7BD;});_tmp7BC;})));return;_LL547:;}}void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(
loc,0,b);}int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b){void*_tmp7C0=(
void*)(Cyc_Absyn_compress_conref(b))->v;void*_tmp7C1;struct Cyc_Absyn_Exp*_tmp7C2;
_LL54F: if(_tmp7C0 <= (void*)1?1:*((int*)_tmp7C0)!= 0)goto _LL551;_tmp7C1=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp7C0)->f1;if(_tmp7C1 <= (void*)1?1:*((int*)
_tmp7C1)!= 0)goto _LL551;_tmp7C2=((struct Cyc_Absyn_Upper_b_struct*)_tmp7C1)->f1;
_LL550: return Cyc_Evexp_eval_const_uint_exp(_tmp7C2)== 1;_LL551:;_LL552: return 0;
_LL54E:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp7C3=Cyc_Tcutil_compress(t);
void*_tmp7C4;struct Cyc_List_List*_tmp7C5;struct Cyc_Absyn_AggrInfo _tmp7C6;void*
_tmp7C7;void*_tmp7C8;struct Cyc_Absyn_AggrInfo _tmp7C9;void*_tmp7CA;void*_tmp7CB;
struct Cyc_Absyn_AggrInfo _tmp7CC;void*_tmp7CD;struct Cyc_Absyn_Aggrdecl**_tmp7CE;
struct Cyc_Absyn_Aggrdecl*_tmp7CF;struct Cyc_List_List*_tmp7D0;struct Cyc_List_List*
_tmp7D1;_LL554: if((int)_tmp7C3 != 0)goto _LL556;_LL555: goto _LL557;_LL556: if(
_tmp7C3 <= (void*)3?1:*((int*)_tmp7C3)!= 5)goto _LL558;_LL557: goto _LL559;_LL558:
if((int)_tmp7C3 != 1)goto _LL55A;_LL559: goto _LL55B;_LL55A: if(_tmp7C3 <= (void*)3?1:*((
int*)_tmp7C3)!= 6)goto _LL55C;_LL55B: return 1;_LL55C: if(_tmp7C3 <= (void*)3?1:*((
int*)_tmp7C3)!= 12)goto _LL55E;_LL55D: goto _LL55F;_LL55E: if(_tmp7C3 <= (void*)3?1:*((
int*)_tmp7C3)!= 13)goto _LL560;_LL55F: return 0;_LL560: if(_tmp7C3 <= (void*)3?1:*((
int*)_tmp7C3)!= 7)goto _LL562;_tmp7C4=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp7C3)->f1;_LL561: return Cyc_Tcutil_bits_only(_tmp7C4);_LL562: if(_tmp7C3 <= (
void*)3?1:*((int*)_tmp7C3)!= 9)goto _LL564;_tmp7C5=((struct Cyc_Absyn_TupleType_struct*)
_tmp7C3)->f1;_LL563: for(0;_tmp7C5 != 0;_tmp7C5=_tmp7C5->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp7C5->hd)).f2))return 0;}return 1;_LL564: if(_tmp7C3 <= (void*)3?1:*((
int*)_tmp7C3)!= 10)goto _LL566;_tmp7C6=((struct Cyc_Absyn_AggrType_struct*)_tmp7C3)->f1;
_tmp7C7=(void*)_tmp7C6.aggr_info;if(*((int*)_tmp7C7)!= 0)goto _LL566;_tmp7C8=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp7C7)->f1;if((int)_tmp7C8 != 1)
goto _LL566;_LL565: return 1;_LL566: if(_tmp7C3 <= (void*)3?1:*((int*)_tmp7C3)!= 10)
goto _LL568;_tmp7C9=((struct Cyc_Absyn_AggrType_struct*)_tmp7C3)->f1;_tmp7CA=(void*)
_tmp7C9.aggr_info;if(*((int*)_tmp7CA)!= 0)goto _LL568;_tmp7CB=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp7CA)->f1;if((int)_tmp7CB != 0)goto _LL568;_LL567: return 0;_LL568: if(_tmp7C3 <= (
void*)3?1:*((int*)_tmp7C3)!= 10)goto _LL56A;_tmp7CC=((struct Cyc_Absyn_AggrType_struct*)
_tmp7C3)->f1;_tmp7CD=(void*)_tmp7CC.aggr_info;if(*((int*)_tmp7CD)!= 1)goto _LL56A;
_tmp7CE=((struct Cyc_Absyn_KnownAggr_struct*)_tmp7CD)->f1;_tmp7CF=*_tmp7CE;
_tmp7D0=_tmp7CC.targs;_LL569: if((void*)_tmp7CF->kind == (void*)1)return 1;if(
_tmp7CF->fields == 0)return 0;{struct _RegionHandle _tmp7D2=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp7D2;_push_region(rgn);{struct Cyc_List_List*_tmp7D3=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp7CF->tvs,_tmp7D0);{struct Cyc_List_List*
fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7CF->fields))->v;
for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,
_tmp7D3,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp7D4=0;
_npop_handler(0);return _tmp7D4;}}}{int _tmp7D5=1;_npop_handler(0);return _tmp7D5;}};
_pop_region(rgn);}_LL56A: if(_tmp7C3 <= (void*)3?1:*((int*)_tmp7C3)!= 11)goto
_LL56C;_tmp7D1=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp7C3)->f2;_LL56B: for(0;
_tmp7D1 != 0;_tmp7D1=_tmp7D1->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp7D1->hd)->type))return 0;}return 1;_LL56C:;_LL56D: return 0;_LL553:;}struct
_tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp7D6=(void*)e->r;
struct _tuple1*_tmp7D7;void*_tmp7D8;struct Cyc_Absyn_Exp*_tmp7D9;struct Cyc_Absyn_Exp*
_tmp7DA;struct Cyc_Absyn_Exp*_tmp7DB;struct Cyc_Absyn_Exp*_tmp7DC;struct Cyc_Absyn_Exp*
_tmp7DD;struct Cyc_Absyn_Exp*_tmp7DE;struct Cyc_Absyn_Exp*_tmp7DF;void*_tmp7E0;
struct Cyc_Absyn_Exp*_tmp7E1;struct Cyc_Absyn_Exp*_tmp7E2;struct Cyc_Absyn_Exp*
_tmp7E3;struct Cyc_Absyn_Exp*_tmp7E4;struct Cyc_List_List*_tmp7E5;struct Cyc_List_List*
_tmp7E6;struct Cyc_List_List*_tmp7E7;void*_tmp7E8;struct Cyc_List_List*_tmp7E9;
struct Cyc_List_List*_tmp7EA;struct Cyc_List_List*_tmp7EB;_LL56F: if(*((int*)
_tmp7D6)!= 0)goto _LL571;_LL570: goto _LL572;_LL571: if(*((int*)_tmp7D6)!= 16)goto
_LL573;_LL572: goto _LL574;_LL573: if(*((int*)_tmp7D6)!= 17)goto _LL575;_LL574: goto
_LL576;_LL575: if(*((int*)_tmp7D6)!= 18)goto _LL577;_LL576: goto _LL578;_LL577: if(*((
int*)_tmp7D6)!= 19)goto _LL579;_LL578: goto _LL57A;_LL579: if(*((int*)_tmp7D6)!= 31)
goto _LL57B;_LL57A: goto _LL57C;_LL57B: if(*((int*)_tmp7D6)!= 32)goto _LL57D;_LL57C:
return 1;_LL57D: if(*((int*)_tmp7D6)!= 1)goto _LL57F;_tmp7D7=((struct Cyc_Absyn_Var_e_struct*)
_tmp7D6)->f1;_tmp7D8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp7D6)->f2;_LL57E: {
void*_tmp7EC=_tmp7D8;struct Cyc_Absyn_Vardecl*_tmp7ED;_LL59C: if(_tmp7EC <= (void*)
1?1:*((int*)_tmp7EC)!= 1)goto _LL59E;_LL59D: return 1;_LL59E: if(_tmp7EC <= (void*)1?
1:*((int*)_tmp7EC)!= 0)goto _LL5A0;_tmp7ED=((struct Cyc_Absyn_Global_b_struct*)
_tmp7EC)->f1;_LL59F: {void*_tmp7EE=Cyc_Tcutil_compress((void*)_tmp7ED->type);
_LL5A5: if(_tmp7EE <= (void*)3?1:*((int*)_tmp7EE)!= 7)goto _LL5A7;_LL5A6: goto _LL5A8;
_LL5A7: if(_tmp7EE <= (void*)3?1:*((int*)_tmp7EE)!= 8)goto _LL5A9;_LL5A8: return 1;
_LL5A9:;_LL5AA: return var_okay;_LL5A4:;}_LL5A0: if((int)_tmp7EC != 0)goto _LL5A2;
_LL5A1: return 0;_LL5A2:;_LL5A3: return var_okay;_LL59B:;}_LL57F: if(*((int*)_tmp7D6)
!= 6)goto _LL581;_tmp7D9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7D6)->f1;
_tmp7DA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7D6)->f2;_tmp7DB=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp7D6)->f3;_LL580: return(Cyc_Tcutil_cnst_exp(te,0,_tmp7D9)?Cyc_Tcutil_cnst_exp(
te,0,_tmp7DA): 0)?Cyc_Tcutil_cnst_exp(te,0,_tmp7DB): 0;_LL581: if(*((int*)_tmp7D6)
!= 7)goto _LL583;_tmp7DC=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp7D6)->f1;_tmp7DD=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp7D6)->f2;_LL582: return Cyc_Tcutil_cnst_exp(
te,0,_tmp7DC)?Cyc_Tcutil_cnst_exp(te,0,_tmp7DD): 0;_LL583: if(*((int*)_tmp7D6)!= 
11)goto _LL585;_tmp7DE=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp7D6)->f1;
_LL584: _tmp7DF=_tmp7DE;goto _LL586;_LL585: if(*((int*)_tmp7D6)!= 12)goto _LL587;
_tmp7DF=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp7D6)->f1;_LL586: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp7DF);_LL587: if(*((int*)_tmp7D6)!= 13)goto _LL589;_tmp7E0=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp7D6)->f1;_tmp7E1=((struct Cyc_Absyn_Cast_e_struct*)
_tmp7D6)->f2;_LL588: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp7E1);_LL589: if(*((
int*)_tmp7D6)!= 14)goto _LL58B;_tmp7E2=((struct Cyc_Absyn_Address_e_struct*)
_tmp7D6)->f1;_LL58A: return Cyc_Tcutil_cnst_exp(te,1,_tmp7E2);_LL58B: if(*((int*)
_tmp7D6)!= 27)goto _LL58D;_tmp7E3=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp7D6)->f2;_tmp7E4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7D6)->f3;
_LL58C: return Cyc_Tcutil_cnst_exp(te,0,_tmp7E3)?Cyc_Tcutil_cnst_exp(te,0,_tmp7E4):
0;_LL58D: if(*((int*)_tmp7D6)!= 26)goto _LL58F;_tmp7E5=((struct Cyc_Absyn_Array_e_struct*)
_tmp7D6)->f1;_LL58E: _tmp7E6=_tmp7E5;goto _LL590;_LL58F: if(*((int*)_tmp7D6)!= 29)
goto _LL591;_tmp7E6=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp7D6)->f2;_LL590:
_tmp7E7=_tmp7E6;goto _LL592;_LL591: if(*((int*)_tmp7D6)!= 28)goto _LL593;_tmp7E7=((
struct Cyc_Absyn_Struct_e_struct*)_tmp7D6)->f3;_LL592: for(0;_tmp7E7 != 0;_tmp7E7=
_tmp7E7->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple22*)_tmp7E7->hd)).f2))
return 0;}return 1;_LL593: if(*((int*)_tmp7D6)!= 3)goto _LL595;_tmp7E8=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp7D6)->f1;_tmp7E9=((struct Cyc_Absyn_Primop_e_struct*)
_tmp7D6)->f2;_LL594: _tmp7EA=_tmp7E9;goto _LL596;_LL595: if(*((int*)_tmp7D6)!= 24)
goto _LL597;_tmp7EA=((struct Cyc_Absyn_Tuple_e_struct*)_tmp7D6)->f1;_LL596: _tmp7EB=
_tmp7EA;goto _LL598;_LL597: if(*((int*)_tmp7D6)!= 30)goto _LL599;_tmp7EB=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp7D6)->f1;_LL598: for(0;_tmp7EB != 0;_tmp7EB=_tmp7EB->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp7EB->hd))return 0;}return 1;
_LL599:;_LL59A: return 0;_LL56E:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp7EF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp7F0;
struct Cyc_Absyn_Conref*_tmp7F1;struct Cyc_Absyn_Conref*_tmp7F2;void*_tmp7F3;
struct Cyc_List_List*_tmp7F4;struct Cyc_Absyn_AggrInfo _tmp7F5;void*_tmp7F6;struct
Cyc_List_List*_tmp7F7;struct Cyc_List_List*_tmp7F8;_LL5AC: if((int)_tmp7EF != 0)
goto _LL5AE;_LL5AD: goto _LL5AF;_LL5AE: if(_tmp7EF <= (void*)3?1:*((int*)_tmp7EF)!= 5)
goto _LL5B0;_LL5AF: goto _LL5B1;_LL5B0: if((int)_tmp7EF != 1)goto _LL5B2;_LL5B1: goto
_LL5B3;_LL5B2: if(_tmp7EF <= (void*)3?1:*((int*)_tmp7EF)!= 6)goto _LL5B4;_LL5B3:
return 1;_LL5B4: if(_tmp7EF <= (void*)3?1:*((int*)_tmp7EF)!= 4)goto _LL5B6;_tmp7F0=((
struct Cyc_Absyn_PointerType_struct*)_tmp7EF)->f1;_tmp7F1=_tmp7F0.nullable;
_tmp7F2=_tmp7F0.bounds;_LL5B5: {void*_tmp7F9=(void*)(Cyc_Absyn_compress_conref(
_tmp7F2))->v;void*_tmp7FA;void*_tmp7FB;_LL5C5: if(_tmp7F9 <= (void*)1?1:*((int*)
_tmp7F9)!= 0)goto _LL5C7;_tmp7FA=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7F9)->f1;if((int)_tmp7FA != 0)goto _LL5C7;_LL5C6: return 1;_LL5C7: if(_tmp7F9 <= (
void*)1?1:*((int*)_tmp7F9)!= 0)goto _LL5C9;_tmp7FB=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7F9)->f1;if(_tmp7FB <= (void*)1?1:*((int*)_tmp7FB)!= 0)goto _LL5C9;_LL5C8: {
void*_tmp7FC=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp7F1))->v;int _tmp7FD;_LL5CC: if(_tmp7FC <= (void*)1?1:*((int*)_tmp7FC)!= 0)goto
_LL5CE;_tmp7FD=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7FC)->f1;_LL5CD:
return _tmp7FD;_LL5CE:;_LL5CF: return 0;_LL5CB:;}_LL5C9:;_LL5CA: return 0;_LL5C4:;}
_LL5B6: if(_tmp7EF <= (void*)3?1:*((int*)_tmp7EF)!= 7)goto _LL5B8;_tmp7F3=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp7EF)->f1;_LL5B7: return Cyc_Tcutil_supports_default(
_tmp7F3);_LL5B8: if(_tmp7EF <= (void*)3?1:*((int*)_tmp7EF)!= 9)goto _LL5BA;_tmp7F4=((
struct Cyc_Absyn_TupleType_struct*)_tmp7EF)->f1;_LL5B9: for(0;_tmp7F4 != 0;_tmp7F4=
_tmp7F4->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple4*)_tmp7F4->hd)).f2))
return 0;}return 1;_LL5BA: if(_tmp7EF <= (void*)3?1:*((int*)_tmp7EF)!= 10)goto _LL5BC;
_tmp7F5=((struct Cyc_Absyn_AggrType_struct*)_tmp7EF)->f1;_tmp7F6=(void*)_tmp7F5.aggr_info;
_tmp7F7=_tmp7F5.targs;_LL5BB: {struct Cyc_Absyn_Aggrdecl*_tmp7FE=Cyc_Absyn_get_known_aggrdecl(
_tmp7F6);if(_tmp7FE->fields == 0)return 0;if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp7FE->exist_vars))->v != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp7FE->tvs,_tmp7F7,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp7FE->fields))->v);}_LL5BC: if(_tmp7EF <= (void*)3?1:*((int*)_tmp7EF)!= 11)goto
_LL5BE;_tmp7F8=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp7EF)->f2;_LL5BD: return
Cyc_Tcutil_fields_support_default(0,0,_tmp7F8);_LL5BE: if(_tmp7EF <= (void*)3?1:*((
int*)_tmp7EF)!= 13)goto _LL5C0;_LL5BF: goto _LL5C1;_LL5C0: if(_tmp7EF <= (void*)3?1:*((
int*)_tmp7EF)!= 12)goto _LL5C2;_LL5C1: return 1;_LL5C2:;_LL5C3: return 0;_LL5AB:;}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmp7FF=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp7FF;_push_region(rgn);{struct Cyc_List_List*_tmp800=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp800,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmp801=0;_npop_handler(0);return _tmp801;}}};
_pop_region(rgn);}return 1;}int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp802=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp803;void*_tmp804;struct Cyc_Absyn_FnInfo _tmp805;
struct Cyc_List_List*_tmp806;_LL5D1: if(_tmp802 <= (void*)3?1:*((int*)_tmp802)!= 4)
goto _LL5D3;_tmp803=((struct Cyc_Absyn_PointerType_struct*)_tmp802)->f1;_tmp804=(
void*)_tmp803.elt_typ;_LL5D2: return Cyc_Tcutil_is_noreturn(_tmp804);_LL5D3: if(
_tmp802 <= (void*)3?1:*((int*)_tmp802)!= 8)goto _LL5D5;_tmp805=((struct Cyc_Absyn_FnType_struct*)
_tmp802)->f1;_tmp806=_tmp805.attributes;_LL5D4: for(0;_tmp806 != 0;_tmp806=_tmp806->tl){
void*_tmp807=(void*)_tmp806->hd;_LL5D8: if((int)_tmp807 != 3)goto _LL5DA;_LL5D9:
return 1;_LL5DA:;_LL5DB: continue;_LL5D7:;}goto _LL5D0;_LL5D5:;_LL5D6: goto _LL5D0;
_LL5D0:;}return 0;}
