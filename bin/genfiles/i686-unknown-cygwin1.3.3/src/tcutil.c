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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(
int,int);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
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
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_normalize_effect(
void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int*Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_snd_tqt(struct _tuple4*);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_is_noreturn(void*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(
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
!= 1)goto _LLCB;_LLCA:({void*_tmp110[0]={};Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress",
sizeof(char),37),_tag_arr(_tmp110,sizeof(void*),0));});_LLCB: if(_tmp10C <= (void*)
1?1:*((int*)_tmp10C)!= 0)goto _LLC6;_tmp10D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10C)->f1;_LLCC: {void*_tmp111=(void*)y->v;void*_tmp112;_LLCE: if((int)_tmp111
!= 0)goto _LLD0;_LLCF:(void*)(y->v=(void*)((void*)x->v));return;_LLD0: if(_tmp111
<= (void*)1?1:*((int*)_tmp111)!= 1)goto _LLD2;_LLD1:({void*_tmp113[0]={};Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)",sizeof(char),40),_tag_arr(
_tmp113,sizeof(void*),0));});_LLD2: if(_tmp111 <= (void*)1?1:*((int*)_tmp111)!= 0)
goto _LLCD;_tmp112=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp111)->f1;_LLD3:
if(cmp(_tmp10D,_tmp112)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)
Cyc_Tcutil_Unify);}return;_LLCD:;}_LLC6:;}}static int Cyc_Tcutil_unify_conrefs(int(*
cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
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
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((
void*)af->type);}static struct _tuple7*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*
tv){void*t;{void*_tmp13A=Cyc_Tcutil_tvar_kind(tv);_LLEE: if((int)_tmp13A != 3)goto
_LLF0;_LLEF: t=(void*)2;goto _LLED;_LLF0: if((int)_tmp13A != 4)goto _LLF2;_LLF1: t=Cyc_Absyn_empty_effect;
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
Cyc_Tcutil_rgns_of,_tmp154);_tmp16E;});_tmp16D;}));_LLF4:;}void*Cyc_Tcutil_normalize_effect(
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
_tmp16F)->f1;_LL134: {void*_tmp17A=Cyc_Tcutil_compress(_tmp172);_LL146: if(
_tmp17A <= (void*)3?1:*((int*)_tmp17A)!= 0)goto _LL148;_LL147: goto _LL149;_LL148:
if(_tmp17A <= (void*)3?1:*((int*)_tmp17A)!= 1)goto _LL14A;_LL149: return e;_LL14A:;
_LL14B: return Cyc_Tcutil_rgns_of(_tmp172);_LL145:;}_LL135:;_LL136: return e;_LL130:;}}
static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_struct*_tmp17B=({struct Cyc_Absyn_FnType_struct*
_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C[0]=({struct Cyc_Absyn_FnType_struct
_tmp17D;_tmp17D.tag=8;_tmp17D.f1=({struct Cyc_Absyn_FnInfo _tmp17E;_tmp17E.tvars=0;
_tmp17E.effect=({struct Cyc_Core_Opt*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->v=(
void*)eff;_tmp17F;});_tmp17E.ret_typ=(void*)((void*)0);_tmp17E.args=0;_tmp17E.c_varargs=
0;_tmp17E.cyc_varargs=0;_tmp17E.rgn_po=0;_tmp17E.attributes=0;_tmp17E;});_tmp17D;});
_tmp17C;});return Cyc_Absyn_atb_typ((void*)_tmp17B,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_bounds_one);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{void*_tmp180=Cyc_Tcutil_compress(
e);void*_tmp181;struct Cyc_List_List*_tmp182;void*_tmp183;struct Cyc_Core_Opt*
_tmp184;struct Cyc_Core_Opt*_tmp185;struct Cyc_Core_Opt**_tmp186;struct Cyc_Core_Opt*
_tmp187;_LL14D: if(_tmp180 <= (void*)3?1:*((int*)_tmp180)!= 19)goto _LL14F;_tmp181=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp180)->f1;_LL14E: if(constrain)
return Cyc_Tcutil_unify(r,_tmp181);_tmp181=Cyc_Tcutil_compress(_tmp181);if(r == 
_tmp181)return 1;{struct _tuple6 _tmp189=({struct _tuple6 _tmp188;_tmp188.f1=r;
_tmp188.f2=_tmp181;_tmp188;});void*_tmp18A;struct Cyc_Absyn_Tvar*_tmp18B;void*
_tmp18C;struct Cyc_Absyn_Tvar*_tmp18D;_LL158: _tmp18A=_tmp189.f1;if(_tmp18A <= (
void*)3?1:*((int*)_tmp18A)!= 1)goto _LL15A;_tmp18B=((struct Cyc_Absyn_VarType_struct*)
_tmp18A)->f1;_tmp18C=_tmp189.f2;if(_tmp18C <= (void*)3?1:*((int*)_tmp18C)!= 1)
goto _LL15A;_tmp18D=((struct Cyc_Absyn_VarType_struct*)_tmp18C)->f1;_LL159: return
Cyc_Absyn_tvar_cmp(_tmp18B,_tmp18D)== 0;_LL15A:;_LL15B: return 0;_LL157:;}_LL14F:
if(_tmp180 <= (void*)3?1:*((int*)_tmp180)!= 20)goto _LL151;_tmp182=((struct Cyc_Absyn_JoinEff_struct*)
_tmp180)->f1;_LL150: for(0;_tmp182 != 0;_tmp182=_tmp182->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp182->hd))return 1;}return 0;_LL151: if(_tmp180 <= (void*)3?1:*((
int*)_tmp180)!= 21)goto _LL153;_tmp183=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp180)->f1;_LL152: {void*_tmp18E=Cyc_Tcutil_rgns_of(_tmp183);void*_tmp18F;
_LL15D: if(_tmp18E <= (void*)3?1:*((int*)_tmp18E)!= 21)goto _LL15F;_tmp18F=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp18E)->f1;_LL15E: if(!constrain)return 0;{void*
_tmp190=Cyc_Tcutil_compress(_tmp18F);struct Cyc_Core_Opt*_tmp191;struct Cyc_Core_Opt*
_tmp192;struct Cyc_Core_Opt**_tmp193;struct Cyc_Core_Opt*_tmp194;_LL162: if(_tmp190
<= (void*)3?1:*((int*)_tmp190)!= 0)goto _LL164;_tmp191=((struct Cyc_Absyn_Evar_struct*)
_tmp190)->f1;_tmp192=((struct Cyc_Absyn_Evar_struct*)_tmp190)->f2;_tmp193=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp190)->f2;_tmp194=((struct Cyc_Absyn_Evar_struct*)
_tmp190)->f4;_LL163: {void*_tmp195=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp194);Cyc_Tcutil_occurs(_tmp195,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp194))->v,r);{void*_tmp196=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp198=_cycalloc(sizeof(*_tmp198));
_tmp198[0]=({struct Cyc_Absyn_JoinEff_struct _tmp199;_tmp199.tag=20;_tmp199.f1=({
void*_tmp19A[2];_tmp19A[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp19B=
_cycalloc(sizeof(*_tmp19B));_tmp19B[0]=({struct Cyc_Absyn_AccessEff_struct _tmp19C;
_tmp19C.tag=19;_tmp19C.f1=(void*)r;_tmp19C;});_tmp19B;});_tmp19A[0]=_tmp195;Cyc_List_list(
_tag_arr(_tmp19A,sizeof(void*),2));});_tmp199;});_tmp198;}));*_tmp193=({struct
Cyc_Core_Opt*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197->v=(void*)_tmp196;
_tmp197;});return 1;}}_LL164:;_LL165: return 0;_LL161:;}_LL15F:;_LL160: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp18E);_LL15C:;}_LL153: if(_tmp180 <= (void*)3?1:*((int*)_tmp180)!= 
0)goto _LL155;_tmp184=((struct Cyc_Absyn_Evar_struct*)_tmp180)->f1;_tmp185=((
struct Cyc_Absyn_Evar_struct*)_tmp180)->f2;_tmp186=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp180)->f2;_tmp187=((struct Cyc_Absyn_Evar_struct*)
_tmp180)->f4;_LL154: if(_tmp184 == 0?1:(void*)_tmp184->v != (void*)4)({void*_tmp19D[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(char),27),_tag_arr(_tmp19D,sizeof(
void*),0));});if(!constrain)return 0;{void*_tmp19E=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp187);Cyc_Tcutil_occurs(_tmp19E,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp187))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp19F=({struct Cyc_Absyn_JoinEff_struct*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));
_tmp1A1[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1A2;_tmp1A2.tag=20;_tmp1A2.f1=({
struct Cyc_List_List*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->hd=(void*)
_tmp19E;_tmp1A3->tl=({struct Cyc_List_List*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));
_tmp1A4->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1A5=_cycalloc(
sizeof(*_tmp1A5));_tmp1A5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1A6;_tmp1A6.tag=
19;_tmp1A6.f1=(void*)r;_tmp1A6;});_tmp1A5;}));_tmp1A4->tl=0;_tmp1A4;});_tmp1A3;});
_tmp1A2;});_tmp1A1;});*_tmp186=({struct Cyc_Core_Opt*_tmp1A0=_cycalloc(sizeof(*
_tmp1A0));_tmp1A0->v=(void*)((void*)_tmp19F);_tmp1A0;});return 1;}}_LL155:;_LL156:
return 0;_LL14C:;}}static int Cyc_Tcutil_type_in_effect(int constrain,void*t,void*e){
t=Cyc_Tcutil_compress(t);{void*_tmp1A7=Cyc_Tcutil_compress(e);struct Cyc_List_List*
_tmp1A8;void*_tmp1A9;struct Cyc_Core_Opt*_tmp1AA;struct Cyc_Core_Opt*_tmp1AB;
struct Cyc_Core_Opt**_tmp1AC;struct Cyc_Core_Opt*_tmp1AD;_LL167: if(_tmp1A7 <= (void*)
3?1:*((int*)_tmp1A7)!= 19)goto _LL169;_LL168: return 0;_LL169: if(_tmp1A7 <= (void*)3?
1:*((int*)_tmp1A7)!= 20)goto _LL16B;_tmp1A8=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1A7)->f1;_LL16A: for(0;_tmp1A8 != 0;_tmp1A8=_tmp1A8->tl){if(Cyc_Tcutil_type_in_effect(
constrain,t,(void*)_tmp1A8->hd))return 1;}return 0;_LL16B: if(_tmp1A7 <= (void*)3?1:*((
int*)_tmp1A7)!= 21)goto _LL16D;_tmp1A9=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1A7)->f1;_LL16C: _tmp1A9=Cyc_Tcutil_compress(_tmp1A9);if(t == _tmp1A9)return 1;
if(constrain)return Cyc_Tcutil_unify(t,_tmp1A9);{void*_tmp1AE=Cyc_Tcutil_rgns_of(
t);void*_tmp1AF;_LL172: if(_tmp1AE <= (void*)3?1:*((int*)_tmp1AE)!= 21)goto _LL174;
_tmp1AF=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1AE)->f1;_LL173: {struct
_tuple6 _tmp1B1=({struct _tuple6 _tmp1B0;_tmp1B0.f1=Cyc_Tcutil_compress(_tmp1AF);
_tmp1B0.f2=_tmp1A9;_tmp1B0;});void*_tmp1B2;struct Cyc_Absyn_Tvar*_tmp1B3;void*
_tmp1B4;struct Cyc_Absyn_Tvar*_tmp1B5;_LL177: _tmp1B2=_tmp1B1.f1;if(_tmp1B2 <= (
void*)3?1:*((int*)_tmp1B2)!= 1)goto _LL179;_tmp1B3=((struct Cyc_Absyn_VarType_struct*)
_tmp1B2)->f1;_tmp1B4=_tmp1B1.f2;if(_tmp1B4 <= (void*)3?1:*((int*)_tmp1B4)!= 1)
goto _LL179;_tmp1B5=((struct Cyc_Absyn_VarType_struct*)_tmp1B4)->f1;_LL178: return
Cyc_Tcutil_unify(t,_tmp1A9);_LL179:;_LL17A: return _tmp1AF == _tmp1A9;_LL176:;}
_LL174:;_LL175: return Cyc_Tcutil_type_in_effect(constrain,t,_tmp1AE);_LL171:;}
_LL16D: if(_tmp1A7 <= (void*)3?1:*((int*)_tmp1A7)!= 0)goto _LL16F;_tmp1AA=((struct
Cyc_Absyn_Evar_struct*)_tmp1A7)->f1;_tmp1AB=((struct Cyc_Absyn_Evar_struct*)
_tmp1A7)->f2;_tmp1AC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1A7)->f2;_tmp1AD=((struct Cyc_Absyn_Evar_struct*)_tmp1A7)->f4;_LL16E: if(
_tmp1AA == 0?1:(void*)_tmp1AA->v != (void*)4)({void*_tmp1B6[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",
sizeof(char),27),_tag_arr(_tmp1B6,sizeof(void*),0));});if(!constrain)return 0;{
void*_tmp1B7=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1AD);Cyc_Tcutil_occurs(
_tmp1B7,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1AD))->v,t);{struct Cyc_Absyn_JoinEff_struct*_tmp1B8=({struct Cyc_Absyn_JoinEff_struct*
_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1BB;_tmp1BB.tag=20;_tmp1BB.f1=({struct Cyc_List_List*_tmp1BC=_cycalloc(
sizeof(*_tmp1BC));_tmp1BC->hd=(void*)_tmp1B7;_tmp1BC->tl=({struct Cyc_List_List*
_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1BF;_tmp1BF.tag=21;_tmp1BF.f1=(void*)t;_tmp1BF;});_tmp1BE;}));_tmp1BD->tl=0;
_tmp1BD;});_tmp1BC;});_tmp1BB;});_tmp1BA;});*_tmp1AC=({struct Cyc_Core_Opt*
_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9->v=(void*)((void*)_tmp1B8);_tmp1B9;});
return 1;}}_LL16F:;_LL170: return 0;_LL166:;}}static int Cyc_Tcutil_variable_in_effect(
int constrain,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp1C0=e;struct Cyc_Absyn_Tvar*_tmp1C1;struct Cyc_List_List*_tmp1C2;void*_tmp1C3;
struct Cyc_Core_Opt*_tmp1C4;struct Cyc_Core_Opt*_tmp1C5;struct Cyc_Core_Opt**
_tmp1C6;struct Cyc_Core_Opt*_tmp1C7;_LL17C: if(_tmp1C0 <= (void*)3?1:*((int*)
_tmp1C0)!= 1)goto _LL17E;_tmp1C1=((struct Cyc_Absyn_VarType_struct*)_tmp1C0)->f1;
_LL17D: return Cyc_Absyn_tvar_cmp(v,_tmp1C1)== 0;_LL17E: if(_tmp1C0 <= (void*)3?1:*((
int*)_tmp1C0)!= 20)goto _LL180;_tmp1C2=((struct Cyc_Absyn_JoinEff_struct*)_tmp1C0)->f1;
_LL17F: for(0;_tmp1C2 != 0;_tmp1C2=_tmp1C2->tl){if(Cyc_Tcutil_variable_in_effect(
constrain,v,(void*)_tmp1C2->hd))return 1;}return 0;_LL180: if(_tmp1C0 <= (void*)3?1:*((
int*)_tmp1C0)!= 21)goto _LL182;_tmp1C3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1C0)->f1;_LL181: {void*_tmp1C8=Cyc_Tcutil_rgns_of(_tmp1C3);void*_tmp1C9;
_LL187: if(_tmp1C8 <= (void*)3?1:*((int*)_tmp1C8)!= 21)goto _LL189;_tmp1C9=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1C8)->f1;_LL188: if(!constrain)return 0;{void*
_tmp1CA=Cyc_Tcutil_compress(_tmp1C9);struct Cyc_Core_Opt*_tmp1CB;struct Cyc_Core_Opt*
_tmp1CC;struct Cyc_Core_Opt**_tmp1CD;struct Cyc_Core_Opt*_tmp1CE;_LL18C: if(_tmp1CA
<= (void*)3?1:*((int*)_tmp1CA)!= 0)goto _LL18E;_tmp1CB=((struct Cyc_Absyn_Evar_struct*)
_tmp1CA)->f1;_tmp1CC=((struct Cyc_Absyn_Evar_struct*)_tmp1CA)->f2;_tmp1CD=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1CA)->f2;_tmp1CE=((struct Cyc_Absyn_Evar_struct*)
_tmp1CA)->f4;_LL18D: {void*_tmp1CF=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1CE);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1CE))->v,v))return 0;{
void*_tmp1D0=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1D3;_tmp1D3.tag=20;_tmp1D3.f1=({void*_tmp1D4[2];_tmp1D4[1]=(void*)({struct
Cyc_Absyn_VarType_struct*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5[0]=({struct
Cyc_Absyn_VarType_struct _tmp1D6;_tmp1D6.tag=1;_tmp1D6.f1=v;_tmp1D6;});_tmp1D5;});
_tmp1D4[0]=_tmp1CF;Cyc_List_list(_tag_arr(_tmp1D4,sizeof(void*),2));});_tmp1D3;});
_tmp1D2;}));*_tmp1CD=({struct Cyc_Core_Opt*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));
_tmp1D1->v=(void*)_tmp1D0;_tmp1D1;});return 1;}}_LL18E:;_LL18F: return 0;_LL18B:;}
_LL189:;_LL18A: return Cyc_Tcutil_variable_in_effect(constrain,v,_tmp1C8);_LL186:;}
_LL182: if(_tmp1C0 <= (void*)3?1:*((int*)_tmp1C0)!= 0)goto _LL184;_tmp1C4=((struct
Cyc_Absyn_Evar_struct*)_tmp1C0)->f1;_tmp1C5=((struct Cyc_Absyn_Evar_struct*)
_tmp1C0)->f2;_tmp1C6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1C0)->f2;_tmp1C7=((struct Cyc_Absyn_Evar_struct*)_tmp1C0)->f4;_LL183: if(
_tmp1C4 == 0?1:(void*)_tmp1C4->v != (void*)4)({void*_tmp1D7[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",
sizeof(char),27),_tag_arr(_tmp1D7,sizeof(void*),0));});{void*_tmp1D8=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1C7);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1C7))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*_tmp1D9=({struct Cyc_Absyn_JoinEff_struct*
_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1DC;_tmp1DC.tag=20;_tmp1DC.f1=({struct Cyc_List_List*_tmp1DD=_cycalloc(
sizeof(*_tmp1DD));_tmp1DD->hd=(void*)_tmp1D8;_tmp1DD->tl=({struct Cyc_List_List*
_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_Absyn_VarType_struct
_tmp1E0;_tmp1E0.tag=1;_tmp1E0.f1=v;_tmp1E0;});_tmp1DF;}));_tmp1DE->tl=0;_tmp1DE;});
_tmp1DD;});_tmp1DC;});_tmp1DB;});*_tmp1C6=({struct Cyc_Core_Opt*_tmp1DA=_cycalloc(
sizeof(*_tmp1DA));_tmp1DA->v=(void*)((void*)_tmp1D9);_tmp1DA;});return 1;}}_LL184:;
_LL185: return 0;_LL17B:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=
Cyc_Tcutil_compress(e);{void*_tmp1E1=e;struct Cyc_List_List*_tmp1E2;void*_tmp1E3;
_LL191: if(_tmp1E1 <= (void*)3?1:*((int*)_tmp1E1)!= 20)goto _LL193;_tmp1E2=((struct
Cyc_Absyn_JoinEff_struct*)_tmp1E1)->f1;_LL192: for(0;_tmp1E2 != 0;_tmp1E2=_tmp1E2->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp1E2->hd))return 1;}return 0;_LL193:
if(_tmp1E1 <= (void*)3?1:*((int*)_tmp1E1)!= 21)goto _LL195;_tmp1E3=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp1E1)->f1;_LL194: {void*_tmp1E4=Cyc_Tcutil_rgns_of(
_tmp1E3);void*_tmp1E5;_LL19A: if(_tmp1E4 <= (void*)3?1:*((int*)_tmp1E4)!= 21)goto
_LL19C;_tmp1E5=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E4)->f1;_LL19B:
return 0;_LL19C:;_LL19D: return Cyc_Tcutil_evar_in_effect(evar,_tmp1E4);_LL199:;}
_LL195: if(_tmp1E1 <= (void*)3?1:*((int*)_tmp1E1)!= 0)goto _LL197;_LL196: return evar
== e;_LL197:;_LL198: return 0;_LL190:;}}int Cyc_Tcutil_subset_effect(int
set_to_empty,void*e1,void*e2){void*_tmp1E6=Cyc_Tcutil_compress(e1);struct Cyc_List_List*
_tmp1E7;void*_tmp1E8;struct Cyc_Absyn_Tvar*_tmp1E9;void*_tmp1EA;struct Cyc_Core_Opt*
_tmp1EB;struct Cyc_Core_Opt**_tmp1EC;struct Cyc_Core_Opt*_tmp1ED;_LL19F: if(_tmp1E6
<= (void*)3?1:*((int*)_tmp1E6)!= 20)goto _LL1A1;_tmp1E7=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E6)->f1;_LL1A0: for(0;_tmp1E7 != 0;_tmp1E7=_tmp1E7->tl){if(!Cyc_Tcutil_subset_effect(
set_to_empty,(void*)_tmp1E7->hd,e2))return 0;}return 1;_LL1A1: if(_tmp1E6 <= (void*)
3?1:*((int*)_tmp1E6)!= 19)goto _LL1A3;_tmp1E8=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1E6)->f1;_LL1A2: return(Cyc_Tcutil_region_in_effect(0,_tmp1E8,e2)?1: Cyc_Tcutil_region_in_effect(
1,_tmp1E8,e2))?1: Cyc_Tcutil_unify(_tmp1E8,(void*)2);_LL1A3: if(_tmp1E6 <= (void*)3?
1:*((int*)_tmp1E6)!= 1)goto _LL1A5;_tmp1E9=((struct Cyc_Absyn_VarType_struct*)
_tmp1E6)->f1;_LL1A4: return Cyc_Tcutil_variable_in_effect(0,_tmp1E9,e2)?1: Cyc_Tcutil_variable_in_effect(
1,_tmp1E9,e2);_LL1A5: if(_tmp1E6 <= (void*)3?1:*((int*)_tmp1E6)!= 21)goto _LL1A7;
_tmp1EA=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E6)->f1;_LL1A6: {void*
_tmp1EE=Cyc_Tcutil_rgns_of(_tmp1EA);void*_tmp1EF;_LL1AC: if(_tmp1EE <= (void*)3?1:*((
int*)_tmp1EE)!= 21)goto _LL1AE;_tmp1EF=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1EE)->f1;_LL1AD: return(Cyc_Tcutil_type_in_effect(0,_tmp1EF,e2)?1: Cyc_Tcutil_type_in_effect(
1,_tmp1EF,e2))?1: Cyc_Tcutil_unify(_tmp1EF,Cyc_Absyn_sint_typ);_LL1AE:;_LL1AF:
return Cyc_Tcutil_subset_effect(set_to_empty,_tmp1EE,e2);_LL1AB:;}_LL1A7: if(
_tmp1E6 <= (void*)3?1:*((int*)_tmp1E6)!= 0)goto _LL1A9;_tmp1EB=((struct Cyc_Absyn_Evar_struct*)
_tmp1E6)->f2;_tmp1EC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1E6)->f2;_tmp1ED=((struct Cyc_Absyn_Evar_struct*)_tmp1E6)->f4;_LL1A8: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){if(set_to_empty)*_tmp1EC=({struct Cyc_Core_Opt*_tmp1F0=_cycalloc(sizeof(*
_tmp1F0));_tmp1F0->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1F1=
_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1F2;
_tmp1F2.tag=20;_tmp1F2.f1=0;_tmp1F2;});_tmp1F1;}));_tmp1F0;});else{Cyc_Tcutil_occurs(
e1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1ED))->v,e2);*_tmp1EC=({struct Cyc_Core_Opt*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));
_tmp1F3->v=(void*)e2;_tmp1F3;});}}return 1;_LL1A9:;_LL1AA:({struct Cyc_Std_String_pa_struct
_tmp1F5;_tmp1F5.tag=0;_tmp1F5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e1);{
void*_tmp1F4[1]={& _tmp1F5};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("subset_effect: bad effect: %s",sizeof(char),30),
_tag_arr(_tmp1F4,sizeof(void*),1));}});_LL19E:;}static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);return Cyc_Tcutil_subset_effect(0,e1,e2)?Cyc_Tcutil_subset_effect(0,e2,e1): 0;}
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{
struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){struct _tuple6 _tmp1F7;void*
_tmp1F8;void*_tmp1F9;struct _tuple6*_tmp1F6=(struct _tuple6*)r1->hd;_tmp1F7=*
_tmp1F6;_tmp1F8=_tmp1F7.f1;_tmp1F9=_tmp1F7.f2;{int found=_tmp1F8 == (void*)2;{
struct Cyc_List_List*r2=rpo2;for(0;r2 != 0?!found: 0;r2=r2->tl){struct _tuple6
_tmp1FB;void*_tmp1FC;void*_tmp1FD;struct _tuple6*_tmp1FA=(struct _tuple6*)r2->hd;
_tmp1FB=*_tmp1FA;_tmp1FC=_tmp1FB.f1;_tmp1FD=_tmp1FB.f2;if(Cyc_Tcutil_unify(
_tmp1F8,_tmp1FC)?Cyc_Tcutil_unify(_tmp1F9,_tmp1FD): 0){found=1;break;}}}if(!found)
return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)?Cyc_Tcutil_sub_rgnpo(
rpo2,rpo1): 0;}struct _tuple10{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*
f2;};struct _tuple11{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _tagged_arr)_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp1FE=t1;struct Cyc_Core_Opt*_tmp1FF;struct Cyc_Core_Opt*
_tmp200;struct Cyc_Core_Opt**_tmp201;struct Cyc_Core_Opt*_tmp202;_LL1B1: if(_tmp1FE
<= (void*)3?1:*((int*)_tmp1FE)!= 0)goto _LL1B3;_tmp1FF=((struct Cyc_Absyn_Evar_struct*)
_tmp1FE)->f1;_tmp200=((struct Cyc_Absyn_Evar_struct*)_tmp1FE)->f2;_tmp201=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1FE)->f2;_tmp202=((struct Cyc_Absyn_Evar_struct*)
_tmp1FE)->f4;_LL1B2: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp202))->v,t2);{void*_tmp203=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp203,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1FF))->v)){*_tmp201=({struct Cyc_Core_Opt*_tmp204=_cycalloc(sizeof(*_tmp204));
_tmp204->v=(void*)t2;_tmp204;});return;}else{{void*_tmp205=t2;struct Cyc_Core_Opt*
_tmp206;struct Cyc_Core_Opt**_tmp207;struct Cyc_Core_Opt*_tmp208;struct Cyc_Absyn_PtrInfo
_tmp209;_LL1B6: if(_tmp205 <= (void*)3?1:*((int*)_tmp205)!= 0)goto _LL1B8;_tmp206=((
struct Cyc_Absyn_Evar_struct*)_tmp205)->f2;_tmp207=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp205)->f2;_tmp208=((struct Cyc_Absyn_Evar_struct*)
_tmp205)->f4;_LL1B7: {struct Cyc_List_List*_tmp20A=(struct Cyc_List_List*)_tmp202->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp208))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp20A,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=
_tag_arr("(type variable would escape scope)",sizeof(char),35);(int)_throw((void*)
Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)_tmp1FF->v,_tmp203)){*_tmp207=({
struct Cyc_Core_Opt*_tmp20B=_cycalloc(sizeof(*_tmp20B));_tmp20B->v=(void*)t1;
_tmp20B;});return;}Cyc_Tcutil_failure_reason=_tag_arr("(kinds are incompatible)",
sizeof(char),25);goto _LL1B5;}_LL1B8: if(_tmp205 <= (void*)3?1:*((int*)_tmp205)!= 4)
goto _LL1BA;_tmp209=((struct Cyc_Absyn_PointerType_struct*)_tmp205)->f1;if(!((void*)
_tmp1FF->v == (void*)2))goto _LL1BA;_LL1B9: {struct Cyc_Absyn_Conref*_tmp20C=Cyc_Absyn_compress_conref(
_tmp209.bounds);{void*_tmp20D=(void*)_tmp20C->v;_LL1BD: if((int)_tmp20D != 0)goto
_LL1BF;_LL1BE:(void*)(_tmp20C->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp20F;_tmp20F.tag=0;_tmp20F.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp211;_tmp211.tag=0;_tmp211.f1=Cyc_Absyn_signed_int_exp(1,0);_tmp211;});
_tmp210;}));_tmp20F;});_tmp20E;})));*_tmp201=({struct Cyc_Core_Opt*_tmp212=
_cycalloc(sizeof(*_tmp212));_tmp212->v=(void*)t2;_tmp212;});return;_LL1BF:;
_LL1C0: goto _LL1BC;_LL1BC:;}goto _LL1B5;}_LL1BA:;_LL1BB: goto _LL1B5;_LL1B5:;}Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}
_LL1B3:;_LL1B4: goto _LL1B0;_LL1B0:;}{struct _tuple6 _tmp214=({struct _tuple6 _tmp213;
_tmp213.f1=t2;_tmp213.f2=t1;_tmp213;});void*_tmp215;void*_tmp216;void*_tmp217;
void*_tmp218;struct Cyc_Absyn_Tvar*_tmp219;void*_tmp21A;struct Cyc_Absyn_Tvar*
_tmp21B;void*_tmp21C;struct Cyc_Absyn_AggrInfo _tmp21D;void*_tmp21E;struct Cyc_List_List*
_tmp21F;void*_tmp220;struct Cyc_Absyn_AggrInfo _tmp221;void*_tmp222;struct Cyc_List_List*
_tmp223;void*_tmp224;struct _tuple1*_tmp225;void*_tmp226;struct _tuple1*_tmp227;
void*_tmp228;struct Cyc_List_List*_tmp229;void*_tmp22A;struct Cyc_List_List*
_tmp22B;void*_tmp22C;struct Cyc_Absyn_TunionInfo _tmp22D;void*_tmp22E;struct Cyc_Absyn_Tuniondecl**
_tmp22F;struct Cyc_Absyn_Tuniondecl*_tmp230;struct Cyc_List_List*_tmp231;void*
_tmp232;void*_tmp233;struct Cyc_Absyn_TunionInfo _tmp234;void*_tmp235;struct Cyc_Absyn_Tuniondecl**
_tmp236;struct Cyc_Absyn_Tuniondecl*_tmp237;struct Cyc_List_List*_tmp238;void*
_tmp239;void*_tmp23A;struct Cyc_Absyn_TunionFieldInfo _tmp23B;void*_tmp23C;struct
Cyc_Absyn_Tuniondecl*_tmp23D;struct Cyc_Absyn_Tunionfield*_tmp23E;struct Cyc_List_List*
_tmp23F;void*_tmp240;struct Cyc_Absyn_TunionFieldInfo _tmp241;void*_tmp242;struct
Cyc_Absyn_Tuniondecl*_tmp243;struct Cyc_Absyn_Tunionfield*_tmp244;struct Cyc_List_List*
_tmp245;void*_tmp246;struct Cyc_Absyn_PtrInfo _tmp247;void*_tmp248;void*_tmp249;
struct Cyc_Absyn_Conref*_tmp24A;struct Cyc_Absyn_Tqual _tmp24B;struct Cyc_Absyn_Conref*
_tmp24C;void*_tmp24D;struct Cyc_Absyn_PtrInfo _tmp24E;void*_tmp24F;void*_tmp250;
struct Cyc_Absyn_Conref*_tmp251;struct Cyc_Absyn_Tqual _tmp252;struct Cyc_Absyn_Conref*
_tmp253;void*_tmp254;void*_tmp255;void*_tmp256;void*_tmp257;void*_tmp258;void*
_tmp259;void*_tmp25A;void*_tmp25B;void*_tmp25C;int _tmp25D;void*_tmp25E;int
_tmp25F;void*_tmp260;void*_tmp261;void*_tmp262;void*_tmp263;void*_tmp264;int
_tmp265;void*_tmp266;int _tmp267;void*_tmp268;void*_tmp269;void*_tmp26A;void*
_tmp26B;void*_tmp26C;void*_tmp26D;struct Cyc_Absyn_Tqual _tmp26E;struct Cyc_Absyn_Exp*
_tmp26F;void*_tmp270;void*_tmp271;struct Cyc_Absyn_Tqual _tmp272;struct Cyc_Absyn_Exp*
_tmp273;void*_tmp274;struct Cyc_Absyn_FnInfo _tmp275;struct Cyc_List_List*_tmp276;
struct Cyc_Core_Opt*_tmp277;void*_tmp278;struct Cyc_List_List*_tmp279;int _tmp27A;
struct Cyc_Absyn_VarargInfo*_tmp27B;struct Cyc_List_List*_tmp27C;struct Cyc_List_List*
_tmp27D;void*_tmp27E;struct Cyc_Absyn_FnInfo _tmp27F;struct Cyc_List_List*_tmp280;
struct Cyc_Core_Opt*_tmp281;void*_tmp282;struct Cyc_List_List*_tmp283;int _tmp284;
struct Cyc_Absyn_VarargInfo*_tmp285;struct Cyc_List_List*_tmp286;struct Cyc_List_List*
_tmp287;void*_tmp288;struct Cyc_List_List*_tmp289;void*_tmp28A;struct Cyc_List_List*
_tmp28B;void*_tmp28C;void*_tmp28D;struct Cyc_List_List*_tmp28E;void*_tmp28F;void*
_tmp290;struct Cyc_List_List*_tmp291;void*_tmp292;void*_tmp293;void*_tmp294;void*
_tmp295;void*_tmp296;void*_tmp297;void*_tmp298;void*_tmp299;void*_tmp29A;void*
_tmp29B;void*_tmp29C;void*_tmp29D;_LL1C2: _tmp215=_tmp214.f1;if(_tmp215 <= (void*)
3?1:*((int*)_tmp215)!= 0)goto _LL1C4;_LL1C3: Cyc_Tcutil_unify_it(t2,t1);return;
_LL1C4: _tmp216=_tmp214.f1;if((int)_tmp216 != 0)goto _LL1C6;_tmp217=_tmp214.f2;if((
int)_tmp217 != 0)goto _LL1C6;_LL1C5: return;_LL1C6: _tmp218=_tmp214.f1;if(_tmp218 <= (
void*)3?1:*((int*)_tmp218)!= 1)goto _LL1C8;_tmp219=((struct Cyc_Absyn_VarType_struct*)
_tmp218)->f1;_tmp21A=_tmp214.f2;if(_tmp21A <= (void*)3?1:*((int*)_tmp21A)!= 1)
goto _LL1C8;_tmp21B=((struct Cyc_Absyn_VarType_struct*)_tmp21A)->f1;_LL1C7: {
struct _tagged_arr*_tmp29E=_tmp219->name;struct _tagged_arr*_tmp29F=_tmp21B->name;
int _tmp2A0=*((int*)_check_null(_tmp219->identity));int _tmp2A1=*((int*)
_check_null(_tmp21B->identity));void*_tmp2A2=Cyc_Tcutil_tvar_kind(_tmp219);void*
_tmp2A3=Cyc_Tcutil_tvar_kind(_tmp21B);if(_tmp2A1 == _tmp2A0?Cyc_Std_zstrptrcmp(
_tmp29E,_tmp29F)== 0: 0){if(_tmp2A2 != _tmp2A3)({struct Cyc_Std_String_pa_struct
_tmp2A7;_tmp2A7.tag=0;_tmp2A7.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2A3);{struct Cyc_Std_String_pa_struct _tmp2A6;_tmp2A6.tag=0;_tmp2A6.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2A2);{struct Cyc_Std_String_pa_struct
_tmp2A5;_tmp2A5.tag=0;_tmp2A5.f1=(struct _tagged_arr)*_tmp29E;{void*_tmp2A4[3]={&
_tmp2A5,& _tmp2A6,& _tmp2A7};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("same type variable %s has kinds %s and %s",sizeof(
char),42),_tag_arr(_tmp2A4,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=
_tag_arr("(variable types are not the same)",sizeof(char),34);goto _LL1C1;}_LL1C8:
_tmp21C=_tmp214.f1;if(_tmp21C <= (void*)3?1:*((int*)_tmp21C)!= 10)goto _LL1CA;
_tmp21D=((struct Cyc_Absyn_AggrType_struct*)_tmp21C)->f1;_tmp21E=(void*)_tmp21D.aggr_info;
_tmp21F=_tmp21D.targs;_tmp220=_tmp214.f2;if(_tmp220 <= (void*)3?1:*((int*)_tmp220)
!= 10)goto _LL1CA;_tmp221=((struct Cyc_Absyn_AggrType_struct*)_tmp220)->f1;_tmp222=(
void*)_tmp221.aggr_info;_tmp223=_tmp221.targs;_LL1C9: {void*_tmp2A9;struct
_tuple1*_tmp2AA;struct _tuple5 _tmp2A8=Cyc_Absyn_aggr_kinded_name(_tmp222);_tmp2A9=
_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;{void*_tmp2AC;struct _tuple1*_tmp2AD;struct _tuple5
_tmp2AB=Cyc_Absyn_aggr_kinded_name(_tmp21E);_tmp2AC=_tmp2AB.f1;_tmp2AD=_tmp2AB.f2;
if(_tmp2A9 != _tmp2AC){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1C1;}if(Cyc_Absyn_qvar_cmp(_tmp2AA,_tmp2AD)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different type name)",sizeof(char),22);goto _LL1C1;}Cyc_Tcutil_unify_list(
_tmp223,_tmp21F);return;}}_LL1CA: _tmp224=_tmp214.f1;if(_tmp224 <= (void*)3?1:*((
int*)_tmp224)!= 12)goto _LL1CC;_tmp225=((struct Cyc_Absyn_EnumType_struct*)_tmp224)->f1;
_tmp226=_tmp214.f2;if(_tmp226 <= (void*)3?1:*((int*)_tmp226)!= 12)goto _LL1CC;
_tmp227=((struct Cyc_Absyn_EnumType_struct*)_tmp226)->f1;_LL1CB: if(Cyc_Absyn_qvar_cmp(
_tmp225,_tmp227)== 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different enum types)",
sizeof(char),23);goto _LL1C1;_LL1CC: _tmp228=_tmp214.f1;if(_tmp228 <= (void*)3?1:*((
int*)_tmp228)!= 13)goto _LL1CE;_tmp229=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp228)->f1;_tmp22A=_tmp214.f2;if(_tmp22A <= (void*)3?1:*((int*)_tmp22A)!= 13)
goto _LL1CE;_tmp22B=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp22A)->f1;_LL1CD: {
int bad=0;for(0;_tmp229 != 0?_tmp22B != 0: 0;(_tmp229=_tmp229->tl,_tmp22B=_tmp22B->tl)){
struct Cyc_Absyn_Enumfield*_tmp2AE=(struct Cyc_Absyn_Enumfield*)_tmp229->hd;struct
Cyc_Absyn_Enumfield*_tmp2AF=(struct Cyc_Absyn_Enumfield*)_tmp22B->hd;if(Cyc_Absyn_qvar_cmp(
_tmp2AE->name,_tmp2AF->name)!= 0){Cyc_Tcutil_failure_reason=_tag_arr("(different names for enum fields)",
sizeof(char),34);bad=1;break;}if(_tmp2AE->tag == _tmp2AF->tag)continue;if(_tmp2AE->tag
== 0?1: _tmp2AF->tag == 0){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(char),39);bad=1;break;}if(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp2AE->tag))!= Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp2AF->tag))){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(char),39);bad=1;break;}}if(bad)goto _LL1C1;if(_tmp229 == 0?_tmp22B == 0: 0)
return;Cyc_Tcutil_failure_reason=_tag_arr("(different number of fields for enums)",
sizeof(char),39);goto _LL1C1;}_LL1CE: _tmp22C=_tmp214.f1;if(_tmp22C <= (void*)3?1:*((
int*)_tmp22C)!= 2)goto _LL1D0;_tmp22D=((struct Cyc_Absyn_TunionType_struct*)
_tmp22C)->f1;_tmp22E=(void*)_tmp22D.tunion_info;if(*((int*)_tmp22E)!= 1)goto
_LL1D0;_tmp22F=((struct Cyc_Absyn_KnownTunion_struct*)_tmp22E)->f1;_tmp230=*
_tmp22F;_tmp231=_tmp22D.targs;_tmp232=(void*)_tmp22D.rgn;_tmp233=_tmp214.f2;if(
_tmp233 <= (void*)3?1:*((int*)_tmp233)!= 2)goto _LL1D0;_tmp234=((struct Cyc_Absyn_TunionType_struct*)
_tmp233)->f1;_tmp235=(void*)_tmp234.tunion_info;if(*((int*)_tmp235)!= 1)goto
_LL1D0;_tmp236=((struct Cyc_Absyn_KnownTunion_struct*)_tmp235)->f1;_tmp237=*
_tmp236;_tmp238=_tmp234.targs;_tmp239=(void*)_tmp234.rgn;_LL1CF: if(_tmp230 == 
_tmp237?1: Cyc_Absyn_qvar_cmp(_tmp230->name,_tmp237->name)== 0){Cyc_Tcutil_unify_it(
_tmp239,_tmp232);Cyc_Tcutil_unify_list(_tmp238,_tmp231);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion types)",sizeof(char),25);goto _LL1C1;_LL1D0: _tmp23A=
_tmp214.f1;if(_tmp23A <= (void*)3?1:*((int*)_tmp23A)!= 3)goto _LL1D2;_tmp23B=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp23A)->f1;_tmp23C=(void*)_tmp23B.field_info;
if(*((int*)_tmp23C)!= 1)goto _LL1D2;_tmp23D=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp23C)->f1;_tmp23E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp23C)->f2;
_tmp23F=_tmp23B.targs;_tmp240=_tmp214.f2;if(_tmp240 <= (void*)3?1:*((int*)_tmp240)
!= 3)goto _LL1D2;_tmp241=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp240)->f1;
_tmp242=(void*)_tmp241.field_info;if(*((int*)_tmp242)!= 1)goto _LL1D2;_tmp243=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp242)->f1;_tmp244=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp242)->f2;_tmp245=_tmp241.targs;_LL1D1: if((_tmp23D == _tmp243?1: Cyc_Absyn_qvar_cmp(
_tmp23D->name,_tmp243->name)== 0)?_tmp23E == _tmp244?1: Cyc_Absyn_qvar_cmp(_tmp23E->name,
_tmp244->name)== 0: 0){Cyc_Tcutil_unify_list(_tmp245,_tmp23F);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)",sizeof(char),31);goto _LL1C1;_LL1D2:
_tmp246=_tmp214.f1;if(_tmp246 <= (void*)3?1:*((int*)_tmp246)!= 4)goto _LL1D4;
_tmp247=((struct Cyc_Absyn_PointerType_struct*)_tmp246)->f1;_tmp248=(void*)
_tmp247.elt_typ;_tmp249=(void*)_tmp247.rgn_typ;_tmp24A=_tmp247.nullable;_tmp24B=
_tmp247.tq;_tmp24C=_tmp247.bounds;_tmp24D=_tmp214.f2;if(_tmp24D <= (void*)3?1:*((
int*)_tmp24D)!= 4)goto _LL1D4;_tmp24E=((struct Cyc_Absyn_PointerType_struct*)
_tmp24D)->f1;_tmp24F=(void*)_tmp24E.elt_typ;_tmp250=(void*)_tmp24E.rgn_typ;
_tmp251=_tmp24E.nullable;_tmp252=_tmp24E.tq;_tmp253=_tmp24E.bounds;_LL1D3: Cyc_Tcutil_unify_it(
_tmp24F,_tmp248);Cyc_Tcutil_unify_it(_tmp249,_tmp250);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_unify_tqual(_tmp252,_tmp24B);Cyc_Tcutil_unify_it_conrefs(
Cyc_Tcutil_boundscmp,_tmp253,_tmp24C,_tag_arr("(different pointer bounds)",
sizeof(char),27));{void*_tmp2B0=(void*)(Cyc_Absyn_compress_conref(_tmp253))->v;
void*_tmp2B1;_LL1FB: if(_tmp2B0 <= (void*)1?1:*((int*)_tmp2B0)!= 0)goto _LL1FD;
_tmp2B1=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2B0)->f1;if((int)_tmp2B1
!= 0)goto _LL1FD;_LL1FC: return;_LL1FD:;_LL1FE: goto _LL1FA;_LL1FA:;}((void(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp251,_tmp24A,
_tag_arr("(different pointer types)",sizeof(char),26));return;_LL1D4: _tmp254=
_tmp214.f1;if(_tmp254 <= (void*)3?1:*((int*)_tmp254)!= 5)goto _LL1D6;_tmp255=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp254)->f1;_tmp256=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp254)->f2;_tmp257=_tmp214.f2;if(_tmp257 <= (void*)3?1:*((int*)_tmp257)!= 5)
goto _LL1D6;_tmp258=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp257)->f1;_tmp259=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp257)->f2;_LL1D5: if(_tmp258 == _tmp255?
_tmp259 == _tmp256: 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different integral types)",
sizeof(char),27);goto _LL1C1;_LL1D6: _tmp25A=_tmp214.f1;if((int)_tmp25A != 1)goto
_LL1D8;_tmp25B=_tmp214.f2;if((int)_tmp25B != 1)goto _LL1D8;_LL1D7: return;_LL1D8:
_tmp25C=_tmp214.f1;if(_tmp25C <= (void*)3?1:*((int*)_tmp25C)!= 6)goto _LL1DA;
_tmp25D=((struct Cyc_Absyn_DoubleType_struct*)_tmp25C)->f1;_tmp25E=_tmp214.f2;if(
_tmp25E <= (void*)3?1:*((int*)_tmp25E)!= 6)goto _LL1DA;_tmp25F=((struct Cyc_Absyn_DoubleType_struct*)
_tmp25E)->f1;_LL1D9: if(_tmp25D == _tmp25F)return;goto _LL1C1;_LL1DA: _tmp260=
_tmp214.f1;if(_tmp260 <= (void*)3?1:*((int*)_tmp260)!= 14)goto _LL1DC;_tmp261=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp260)->f1;_tmp262=_tmp214.f2;if(
_tmp262 <= (void*)3?1:*((int*)_tmp262)!= 14)goto _LL1DC;_tmp263=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp262)->f1;_LL1DB: Cyc_Tcutil_unify_it(_tmp261,_tmp263);return;_LL1DC: _tmp264=
_tmp214.f1;if(_tmp264 <= (void*)3?1:*((int*)_tmp264)!= 18)goto _LL1DE;_tmp265=((
struct Cyc_Absyn_TypeInt_struct*)_tmp264)->f1;_tmp266=_tmp214.f2;if(_tmp266 <= (
void*)3?1:*((int*)_tmp266)!= 18)goto _LL1DE;_tmp267=((struct Cyc_Absyn_TypeInt_struct*)
_tmp266)->f1;_LL1DD: if(_tmp265 == _tmp267)return;Cyc_Tcutil_failure_reason=
_tag_arr("(different type integers)",sizeof(char),26);goto _LL1C1;_LL1DE: _tmp268=
_tmp214.f1;if(_tmp268 <= (void*)3?1:*((int*)_tmp268)!= 17)goto _LL1E0;_tmp269=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp268)->f1;_tmp26A=_tmp214.f2;if(
_tmp26A <= (void*)3?1:*((int*)_tmp26A)!= 17)goto _LL1E0;_tmp26B=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp26A)->f1;_LL1DF: Cyc_Tcutil_unify_it(_tmp269,_tmp26B);return;_LL1E0: _tmp26C=
_tmp214.f1;if(_tmp26C <= (void*)3?1:*((int*)_tmp26C)!= 7)goto _LL1E2;_tmp26D=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp26C)->f1;_tmp26E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp26C)->f2;_tmp26F=((struct Cyc_Absyn_ArrayType_struct*)_tmp26C)->f3;_tmp270=
_tmp214.f2;if(_tmp270 <= (void*)3?1:*((int*)_tmp270)!= 7)goto _LL1E2;_tmp271=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp270)->f1;_tmp272=((struct Cyc_Absyn_ArrayType_struct*)
_tmp270)->f2;_tmp273=((struct Cyc_Absyn_ArrayType_struct*)_tmp270)->f3;_LL1E1: Cyc_Tcutil_unify_tqual(
_tmp272,_tmp26E);Cyc_Tcutil_unify_it(_tmp271,_tmp26D);if(_tmp26F == _tmp273)
return;if(_tmp26F == 0?1: _tmp273 == 0)goto _LL1C1;if(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp26F))== Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp273)))return;Cyc_Tcutil_failure_reason=
_tag_arr("(different array sizes)",sizeof(char),24);goto _LL1C1;_LL1E2: _tmp274=
_tmp214.f1;if(_tmp274 <= (void*)3?1:*((int*)_tmp274)!= 8)goto _LL1E4;_tmp275=((
struct Cyc_Absyn_FnType_struct*)_tmp274)->f1;_tmp276=_tmp275.tvars;_tmp277=
_tmp275.effect;_tmp278=(void*)_tmp275.ret_typ;_tmp279=_tmp275.args;_tmp27A=
_tmp275.c_varargs;_tmp27B=_tmp275.cyc_varargs;_tmp27C=_tmp275.rgn_po;_tmp27D=
_tmp275.attributes;_tmp27E=_tmp214.f2;if(_tmp27E <= (void*)3?1:*((int*)_tmp27E)!= 
8)goto _LL1E4;_tmp27F=((struct Cyc_Absyn_FnType_struct*)_tmp27E)->f1;_tmp280=
_tmp27F.tvars;_tmp281=_tmp27F.effect;_tmp282=(void*)_tmp27F.ret_typ;_tmp283=
_tmp27F.args;_tmp284=_tmp27F.c_varargs;_tmp285=_tmp27F.cyc_varargs;_tmp286=
_tmp27F.rgn_po;_tmp287=_tmp27F.attributes;_LL1E3: {int done=0;{struct
_RegionHandle _tmp2B2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2B2;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp280 != 0){if(_tmp276 == 0){
Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too few type variables)",
sizeof(char),50);(int)_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp2B3=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp280->hd);void*_tmp2B4=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp276->hd);if(_tmp2B3 != _tmp2B4){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp2B8;_tmp2B8.tag=0;_tmp2B8.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2B4);{struct Cyc_Std_String_pa_struct
_tmp2B7;_tmp2B7.tag=0;_tmp2B7.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2B3);{struct Cyc_Std_String_pa_struct _tmp2B6;_tmp2B6.tag=0;_tmp2B6.f1=(struct
_tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp280->hd);{void*
_tmp2B5[3]={& _tmp2B6,& _tmp2B7,& _tmp2B8};Cyc_Std_aprintf(_tag_arr("(type var %s has different kinds %s and %s)",
sizeof(char),44),_tag_arr(_tmp2B5,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp2B9=_region_malloc(rgn,sizeof(*_tmp2B9));_tmp2B9->hd=({
struct _tuple7*_tmp2BA=_region_malloc(rgn,sizeof(*_tmp2BA));_tmp2BA->f1=(struct
Cyc_Absyn_Tvar*)_tmp276->hd;_tmp2BA->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB[0]=({struct Cyc_Absyn_VarType_struct
_tmp2BC;_tmp2BC.tag=1;_tmp2BC.f1=(struct Cyc_Absyn_Tvar*)_tmp280->hd;_tmp2BC;});
_tmp2BB;});_tmp2BA;});_tmp2B9->tl=inst;_tmp2B9;});_tmp280=_tmp280->tl;_tmp276=
_tmp276->tl;}}if(_tmp276 != 0){Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too many type variables)",
sizeof(char),51);_npop_handler(0);goto _LL1C1;}if(inst != 0){Cyc_Tcutil_unify_it((
void*)({struct Cyc_Absyn_FnType_struct*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));
_tmp2BD[0]=({struct Cyc_Absyn_FnType_struct _tmp2BE;_tmp2BE.tag=8;_tmp2BE.f1=({
struct Cyc_Absyn_FnInfo _tmp2BF;_tmp2BF.tvars=0;_tmp2BF.effect=_tmp281;_tmp2BF.ret_typ=(
void*)_tmp282;_tmp2BF.args=_tmp283;_tmp2BF.c_varargs=_tmp284;_tmp2BF.cyc_varargs=
_tmp285;_tmp2BF.rgn_po=_tmp286;_tmp2BF.attributes=_tmp287;_tmp2BF;});_tmp2BE;});
_tmp2BD;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0[0]=({struct Cyc_Absyn_FnType_struct
_tmp2C1;_tmp2C1.tag=8;_tmp2C1.f1=({struct Cyc_Absyn_FnInfo _tmp2C2;_tmp2C2.tvars=0;
_tmp2C2.effect=_tmp277;_tmp2C2.ret_typ=(void*)_tmp278;_tmp2C2.args=_tmp279;
_tmp2C2.c_varargs=_tmp27A;_tmp2C2.cyc_varargs=_tmp27B;_tmp2C2.rgn_po=_tmp27C;
_tmp2C2.attributes=_tmp27D;_tmp2C2;});_tmp2C1;});_tmp2C0;})));done=1;}};
_pop_region(rgn);}if(done)return;Cyc_Tcutil_unify_it(_tmp282,_tmp278);for(0;
_tmp283 != 0?_tmp279 != 0: 0;(_tmp283=_tmp283->tl,_tmp279=_tmp279->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)_tmp283->hd)).f2,(*((struct _tuple2*)_tmp279->hd)).f2);Cyc_Tcutil_unify_it((*((
struct _tuple2*)_tmp283->hd)).f3,(*((struct _tuple2*)_tmp279->hd)).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp283 != 0?1: _tmp279 != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)",sizeof(char),52);
goto _LL1C1;}if(_tmp284 != _tmp27A){Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes C varargs)",
sizeof(char),41);goto _LL1C1;}{int bad_cyc_vararg=0;{struct _tuple10 _tmp2C4=({
struct _tuple10 _tmp2C3;_tmp2C3.f1=_tmp285;_tmp2C3.f2=_tmp27B;_tmp2C3;});struct Cyc_Absyn_VarargInfo*
_tmp2C5;struct Cyc_Absyn_VarargInfo*_tmp2C6;struct Cyc_Absyn_VarargInfo*_tmp2C7;
struct Cyc_Absyn_VarargInfo*_tmp2C8;struct Cyc_Absyn_VarargInfo*_tmp2C9;struct Cyc_Absyn_VarargInfo
_tmp2CA;struct Cyc_Core_Opt*_tmp2CB;struct Cyc_Absyn_Tqual _tmp2CC;void*_tmp2CD;int
_tmp2CE;struct Cyc_Absyn_VarargInfo*_tmp2CF;struct Cyc_Absyn_VarargInfo _tmp2D0;
struct Cyc_Core_Opt*_tmp2D1;struct Cyc_Absyn_Tqual _tmp2D2;void*_tmp2D3;int _tmp2D4;
_LL200: _tmp2C5=_tmp2C4.f1;if(_tmp2C5 != 0)goto _LL202;_tmp2C6=_tmp2C4.f2;if(
_tmp2C6 != 0)goto _LL202;_LL201: goto _LL1FF;_LL202: _tmp2C7=_tmp2C4.f1;if(_tmp2C7 != 
0)goto _LL204;_LL203: goto _LL205;_LL204: _tmp2C8=_tmp2C4.f2;if(_tmp2C8 != 0)goto
_LL206;_LL205: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes varargs)",
sizeof(char),39);goto _LL1FF;_LL206: _tmp2C9=_tmp2C4.f1;if(_tmp2C9 == 0)goto _LL1FF;
_tmp2CA=*_tmp2C9;_tmp2CB=_tmp2CA.name;_tmp2CC=_tmp2CA.tq;_tmp2CD=(void*)_tmp2CA.type;
_tmp2CE=_tmp2CA.inject;_tmp2CF=_tmp2C4.f2;if(_tmp2CF == 0)goto _LL1FF;_tmp2D0=*
_tmp2CF;_tmp2D1=_tmp2D0.name;_tmp2D2=_tmp2D0.tq;_tmp2D3=(void*)_tmp2D0.type;
_tmp2D4=_tmp2D0.inject;_LL207: Cyc_Tcutil_unify_tqual(_tmp2CC,_tmp2D2);Cyc_Tcutil_unify_it(
_tmp2CD,_tmp2D3);if(_tmp2CE != _tmp2D4){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type injects varargs)",sizeof(char),41);}goto _LL1FF;
_LL1FF:;}if(bad_cyc_vararg)goto _LL1C1;{int bad_effect=0;{struct _tuple11 _tmp2D6=({
struct _tuple11 _tmp2D5;_tmp2D5.f1=_tmp281;_tmp2D5.f2=_tmp277;_tmp2D5;});struct Cyc_Core_Opt*
_tmp2D7;struct Cyc_Core_Opt*_tmp2D8;struct Cyc_Core_Opt*_tmp2D9;struct Cyc_Core_Opt*
_tmp2DA;_LL209: _tmp2D7=_tmp2D6.f1;if(_tmp2D7 != 0)goto _LL20B;_tmp2D8=_tmp2D6.f2;
if(_tmp2D8 != 0)goto _LL20B;_LL20A: goto _LL208;_LL20B: _tmp2D9=_tmp2D6.f1;if(_tmp2D9
!= 0)goto _LL20D;_LL20C: goto _LL20E;_LL20D: _tmp2DA=_tmp2D6.f2;if(_tmp2DA != 0)goto
_LL20F;_LL20E: bad_effect=1;goto _LL208;_LL20F:;_LL210: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp281))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp277))->v);goto _LL208;_LL208:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=_tag_arr("(function type effects do not unify)",
sizeof(char),37);goto _LL1C1;}if(!Cyc_Tcutil_same_atts(_tmp27D,_tmp287)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different attributes)",sizeof(char),43);goto
_LL1C1;}if(!Cyc_Tcutil_same_rgn_po(_tmp27C,_tmp286)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different region lifetime orderings)",sizeof(char),
58);goto _LL1C1;}return;}}}_LL1E4: _tmp288=_tmp214.f1;if(_tmp288 <= (void*)3?1:*((
int*)_tmp288)!= 9)goto _LL1E6;_tmp289=((struct Cyc_Absyn_TupleType_struct*)_tmp288)->f1;
_tmp28A=_tmp214.f2;if(_tmp28A <= (void*)3?1:*((int*)_tmp28A)!= 9)goto _LL1E6;
_tmp28B=((struct Cyc_Absyn_TupleType_struct*)_tmp28A)->f1;_LL1E5: for(0;_tmp28B != 
0?_tmp289 != 0: 0;(_tmp28B=_tmp28B->tl,_tmp289=_tmp289->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)_tmp28B->hd)).f1,(*((struct _tuple4*)_tmp289->hd)).f1);Cyc_Tcutil_unify_it((*((
struct _tuple4*)_tmp28B->hd)).f2,(*((struct _tuple4*)_tmp289->hd)).f2);}if(_tmp28B
== 0?_tmp289 == 0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(tuple types have different numbers of components)",sizeof(char),51);
goto _LL1C1;_LL1E6: _tmp28C=_tmp214.f1;if(_tmp28C <= (void*)3?1:*((int*)_tmp28C)!= 
11)goto _LL1E8;_tmp28D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp28C)->f1;
_tmp28E=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp28C)->f2;_tmp28F=_tmp214.f2;
if(_tmp28F <= (void*)3?1:*((int*)_tmp28F)!= 11)goto _LL1E8;_tmp290=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp28F)->f1;_tmp291=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp28F)->f2;_LL1E7: if(_tmp290 != _tmp28D){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1C1;}for(0;_tmp291 != 0?_tmp28E != 0: 0;(_tmp291=_tmp291->tl,
_tmp28E=_tmp28E->tl)){struct Cyc_Absyn_Aggrfield*_tmp2DB=(struct Cyc_Absyn_Aggrfield*)
_tmp291->hd;struct Cyc_Absyn_Aggrfield*_tmp2DC=(struct Cyc_Absyn_Aggrfield*)
_tmp28E->hd;if(Cyc_Std_zstrptrcmp(_tmp2DB->name,_tmp2DC->name)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different member names)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual(_tmp2DB->tq,_tmp2DC->tq);Cyc_Tcutil_unify_it((void*)
_tmp2DB->type,(void*)_tmp2DC->type);if(!Cyc_Tcutil_same_atts(_tmp2DB->attributes,
_tmp2DC->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different attributes on member)",sizeof(char),33);(int)_throw((void*)
Cyc_Tcutil_Unify);}if(((_tmp2DB->width != 0?_tmp2DC->width == 0: 0)?1:(_tmp2DC->width
!= 0?_tmp2DB->width == 0: 0))?1:((_tmp2DB->width != 0?_tmp2DC->width != 0: 0)?Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2DB->width))!= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2DC->width)): 0)){Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=_tag_arr("(different bitfield widths on member)",
sizeof(char),38);(int)_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp291 == 0?_tmp28E == 
0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of members)",sizeof(char),30);goto _LL1C1;_LL1E8:
_tmp292=_tmp214.f1;if((int)_tmp292 != 2)goto _LL1EA;_tmp293=_tmp214.f2;if((int)
_tmp293 != 2)goto _LL1EA;_LL1E9: return;_LL1EA: _tmp294=_tmp214.f1;if(_tmp294 <= (
void*)3?1:*((int*)_tmp294)!= 15)goto _LL1EC;_tmp295=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp294)->f1;_tmp296=_tmp214.f2;if(_tmp296 <= (void*)3?1:*((int*)_tmp296)!= 15)
goto _LL1EC;_tmp297=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp296)->f1;
_LL1EB: Cyc_Tcutil_unify_it(_tmp295,_tmp297);return;_LL1EC: _tmp298=_tmp214.f1;if(
_tmp298 <= (void*)3?1:*((int*)_tmp298)!= 20)goto _LL1EE;_LL1ED: goto _LL1EF;_LL1EE:
_tmp299=_tmp214.f2;if(_tmp299 <= (void*)3?1:*((int*)_tmp299)!= 20)goto _LL1F0;
_LL1EF: goto _LL1F1;_LL1F0: _tmp29A=_tmp214.f1;if(_tmp29A <= (void*)3?1:*((int*)
_tmp29A)!= 19)goto _LL1F2;_LL1F1: goto _LL1F3;_LL1F2: _tmp29B=_tmp214.f1;if(_tmp29B
<= (void*)3?1:*((int*)_tmp29B)!= 21)goto _LL1F4;_LL1F3: goto _LL1F5;_LL1F4: _tmp29C=
_tmp214.f2;if(_tmp29C <= (void*)3?1:*((int*)_tmp29C)!= 21)goto _LL1F6;_LL1F5: goto
_LL1F7;_LL1F6: _tmp29D=_tmp214.f2;if(_tmp29D <= (void*)3?1:*((int*)_tmp29D)!= 19)
goto _LL1F8;_LL1F7: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)",sizeof(char),22);goto _LL1C1;_LL1F8:;_LL1F9: goto
_LL1C1;_LL1C1:;}(int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_opt_cmp(int(*
cmp)(void*,void*),struct Cyc_Core_Opt*a1,struct Cyc_Core_Opt*a2){if(a1 == a2)return
0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0: 0)return 1;return cmp((void*)((
struct Cyc_Core_Opt*)_check_null(a1))->v,(void*)((struct Cyc_Core_Opt*)_check_null(
a2))->v);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == 
a2)return 0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0: 0)return 1;return cmp((
void*)_check_null(a1),(void*)_check_null(a2));}int Cyc_Tcutil_list_cmp(int(*cmp)(
void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)return 0;
for(0;l1 != 0?l2 != 0: 0;(l1=l1->tl,l2=l2->tl)){int _tmp2DD=cmp((void*)l1->hd,(void*)
l2->hd);if(_tmp2DD != 0)return _tmp2DD;}if(l2 != 0)return - 1;if(l1 != 0)return 1;
return 0;}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){int _tmp2DE=(tq1.q_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int
_tmp2DF=(tq2.q_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp2DE,_tmp2DF);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp2E0=(void*)x->v;void*
_tmp2E1;_LL212: if((int)_tmp2E0 != 0)goto _LL214;_LL213: return - 1;_LL214: if(_tmp2E0
<= (void*)1?1:*((int*)_tmp2E0)!= 0)goto _LL216;_tmp2E1=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2E0)->f1;_LL215: {void*_tmp2E2=(void*)y->v;void*_tmp2E3;_LL219: if((int)
_tmp2E2 != 0)goto _LL21B;_LL21A: return 1;_LL21B: if(_tmp2E2 <= (void*)1?1:*((int*)
_tmp2E2)!= 0)goto _LL21D;_tmp2E3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2E2)->f1;_LL21C: return cmp(_tmp2E1,_tmp2E3);_LL21D: if(_tmp2E2 <= (void*)1?1:*((
int*)_tmp2E2)!= 1)goto _LL218;_LL21E:({void*_tmp2E4[0]={};Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)",sizeof(char),40),_tag_arr(
_tmp2E4,sizeof(void*),0));});_LL218:;}_LL216: if(_tmp2E0 <= (void*)1?1:*((int*)
_tmp2E0)!= 1)goto _LL211;_LL217:({void*_tmp2E5[0]={};Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress",
sizeof(char),37),_tag_arr(_tmp2E5,sizeof(void*),0));});_LL211:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp2E7;struct Cyc_Absyn_Tqual
_tmp2E8;void*_tmp2E9;struct _tuple4*_tmp2E6=tqt1;_tmp2E7=*_tmp2E6;_tmp2E8=_tmp2E7.f1;
_tmp2E9=_tmp2E7.f2;{struct _tuple4 _tmp2EB;struct Cyc_Absyn_Tqual _tmp2EC;void*
_tmp2ED;struct _tuple4*_tmp2EA=tqt2;_tmp2EB=*_tmp2EA;_tmp2EC=_tmp2EB.f1;_tmp2ED=
_tmp2EB.f2;{int _tmp2EE=Cyc_Tcutil_tqual_cmp(_tmp2E8,_tmp2EC);if(_tmp2EE != 0)
return _tmp2EE;return Cyc_Tcutil_typecmp(_tmp2E9,_tmp2ED);}}}static int Cyc_Tcutil_const_uint_exp_cmp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return Cyc_Core_intcmp((int)Cyc_Evexp_eval_const_uint_exp(
e1),(int)Cyc_Evexp_eval_const_uint_exp(e2));}static int Cyc_Tcutil_attribute_case_number(
void*att){void*_tmp2EF=att;_LL220: if(_tmp2EF <= (void*)16?1:*((int*)_tmp2EF)!= 0)
goto _LL222;_LL221: return 0;_LL222: if((int)_tmp2EF != 0)goto _LL224;_LL223: return 1;
_LL224: if((int)_tmp2EF != 1)goto _LL226;_LL225: return 2;_LL226: if((int)_tmp2EF != 2)
goto _LL228;_LL227: return 3;_LL228: if((int)_tmp2EF != 3)goto _LL22A;_LL229: return 4;
_LL22A: if((int)_tmp2EF != 4)goto _LL22C;_LL22B: return 5;_LL22C: if(_tmp2EF <= (void*)
16?1:*((int*)_tmp2EF)!= 1)goto _LL22E;_LL22D: return 6;_LL22E: if((int)_tmp2EF != 5)
goto _LL230;_LL22F: return 7;_LL230: if(_tmp2EF <= (void*)16?1:*((int*)_tmp2EF)!= 2)
goto _LL232;_LL231: return 8;_LL232: if((int)_tmp2EF != 6)goto _LL234;_LL233: return 9;
_LL234: if((int)_tmp2EF != 7)goto _LL236;_LL235: return 10;_LL236: if((int)_tmp2EF != 8)
goto _LL238;_LL237: return 11;_LL238: if((int)_tmp2EF != 9)goto _LL23A;_LL239: return 12;
_LL23A: if((int)_tmp2EF != 10)goto _LL23C;_LL23B: return 13;_LL23C: if((int)_tmp2EF != 
11)goto _LL23E;_LL23D: return 14;_LL23E: if((int)_tmp2EF != 12)goto _LL240;_LL23F:
return 15;_LL240: if((int)_tmp2EF != 13)goto _LL242;_LL241: return 16;_LL242: if((int)
_tmp2EF != 14)goto _LL244;_LL243: return 17;_LL244: if((int)_tmp2EF != 15)goto _LL246;
_LL245: return 18;_LL246: if(_tmp2EF <= (void*)16?1:*((int*)_tmp2EF)!= 3)goto _LL21F;
_LL247: return 19;_LL21F:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple6 _tmp2F1=({struct _tuple6 _tmp2F0;_tmp2F0.f1=att1;_tmp2F0.f2=att2;
_tmp2F0;});void*_tmp2F2;int _tmp2F3;void*_tmp2F4;int _tmp2F5;void*_tmp2F6;int
_tmp2F7;void*_tmp2F8;int _tmp2F9;void*_tmp2FA;struct _tagged_arr _tmp2FB;void*
_tmp2FC;struct _tagged_arr _tmp2FD;void*_tmp2FE;void*_tmp2FF;int _tmp300;int _tmp301;
void*_tmp302;void*_tmp303;int _tmp304;int _tmp305;_LL249: _tmp2F2=_tmp2F1.f1;if(
_tmp2F2 <= (void*)16?1:*((int*)_tmp2F2)!= 0)goto _LL24B;_tmp2F3=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp2F2)->f1;_tmp2F4=_tmp2F1.f2;if(_tmp2F4 <= (void*)16?1:*((int*)_tmp2F4)!= 0)
goto _LL24B;_tmp2F5=((struct Cyc_Absyn_Regparm_att_struct*)_tmp2F4)->f1;_LL24A:
_tmp2F7=_tmp2F3;_tmp2F9=_tmp2F5;goto _LL24C;_LL24B: _tmp2F6=_tmp2F1.f1;if(_tmp2F6
<= (void*)16?1:*((int*)_tmp2F6)!= 1)goto _LL24D;_tmp2F7=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp2F6)->f1;_tmp2F8=_tmp2F1.f2;if(_tmp2F8 <= (void*)16?1:*((int*)_tmp2F8)!= 1)
goto _LL24D;_tmp2F9=((struct Cyc_Absyn_Aligned_att_struct*)_tmp2F8)->f1;_LL24C:
return Cyc_Core_intcmp(_tmp2F7,_tmp2F9);_LL24D: _tmp2FA=_tmp2F1.f1;if(_tmp2FA <= (
void*)16?1:*((int*)_tmp2FA)!= 2)goto _LL24F;_tmp2FB=((struct Cyc_Absyn_Section_att_struct*)
_tmp2FA)->f1;_tmp2FC=_tmp2F1.f2;if(_tmp2FC <= (void*)16?1:*((int*)_tmp2FC)!= 2)
goto _LL24F;_tmp2FD=((struct Cyc_Absyn_Section_att_struct*)_tmp2FC)->f1;_LL24E:
return Cyc_Std_strcmp(_tmp2FB,_tmp2FD);_LL24F: _tmp2FE=_tmp2F1.f1;if(_tmp2FE <= (
void*)16?1:*((int*)_tmp2FE)!= 3)goto _LL251;_tmp2FF=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp2FE)->f1;_tmp300=((struct Cyc_Absyn_Format_att_struct*)_tmp2FE)->f2;_tmp301=((
struct Cyc_Absyn_Format_att_struct*)_tmp2FE)->f3;_tmp302=_tmp2F1.f2;if(_tmp302 <= (
void*)16?1:*((int*)_tmp302)!= 3)goto _LL251;_tmp303=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp302)->f1;_tmp304=((struct Cyc_Absyn_Format_att_struct*)_tmp302)->f2;_tmp305=((
struct Cyc_Absyn_Format_att_struct*)_tmp302)->f3;_LL250: {int _tmp306=Cyc_Core_intcmp((
int)((unsigned int)_tmp2FF),(int)((unsigned int)_tmp303));if(_tmp306 != 0)return
_tmp306;{int _tmp307=Cyc_Core_intcmp(_tmp300,_tmp304);if(_tmp307 != 0)return
_tmp307;return Cyc_Core_intcmp(_tmp301,_tmp305);}}_LL251:;_LL252: return Cyc_Core_intcmp(
Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));
_LL248:;}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct
Cyc_Absyn_Aggrfield*f2){int _tmp308=Cyc_Std_zstrptrcmp(f1->name,f2->name);if(
_tmp308 != 0)return _tmp308;{int _tmp309=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp309 != 0)return _tmp309;{int _tmp30A=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp30A != 0)return _tmp30A;{int _tmp30B=Cyc_Tcutil_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp30B != 0)return _tmp30B;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,f1->width,f2->width);}}}}
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp30C=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp30C != 0)return
_tmp30C;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t){void*_tmp30D=t;
_LL254: if((int)_tmp30D != 0)goto _LL256;_LL255: return 0;_LL256: if(_tmp30D <= (void*)
3?1:*((int*)_tmp30D)!= 0)goto _LL258;_LL257: return 1;_LL258: if(_tmp30D <= (void*)3?
1:*((int*)_tmp30D)!= 1)goto _LL25A;_LL259: return 2;_LL25A: if(_tmp30D <= (void*)3?1:*((
int*)_tmp30D)!= 2)goto _LL25C;_LL25B: return 3;_LL25C: if(_tmp30D <= (void*)3?1:*((
int*)_tmp30D)!= 3)goto _LL25E;_LL25D: return 4;_LL25E: if(_tmp30D <= (void*)3?1:*((
int*)_tmp30D)!= 4)goto _LL260;_LL25F: return 5;_LL260: if(_tmp30D <= (void*)3?1:*((
int*)_tmp30D)!= 5)goto _LL262;_LL261: return 6;_LL262: if((int)_tmp30D != 1)goto
_LL264;_LL263: return 7;_LL264: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 6)goto
_LL266;_LL265: return 8;_LL266: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 7)goto
_LL268;_LL267: return 9;_LL268: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 8)goto
_LL26A;_LL269: return 10;_LL26A: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 9)goto
_LL26C;_LL26B: return 11;_LL26C: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 10)goto
_LL26E;_LL26D: return 12;_LL26E: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 11)goto
_LL270;_LL26F: return 14;_LL270: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 12)goto
_LL272;_LL271: return 16;_LL272: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 13)goto
_LL274;_LL273: return 17;_LL274: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 15)goto
_LL276;_LL275: return 18;_LL276: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 16)goto
_LL278;_LL277: return 19;_LL278: if((int)_tmp30D != 2)goto _LL27A;_LL279: return 20;
_LL27A: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 19)goto _LL27C;_LL27B: return 21;
_LL27C: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 20)goto _LL27E;_LL27D: return 22;
_LL27E: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 21)goto _LL280;_LL27F: return 23;
_LL280: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 14)goto _LL282;_LL281: return 24;
_LL282: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 18)goto _LL284;_LL283: return 25;
_LL284: if(_tmp30D <= (void*)3?1:*((int*)_tmp30D)!= 17)goto _LL253;_LL285: return 26;
_LL253:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp30E=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(
t1),Cyc_Tcutil_type_case_number(t2));if(_tmp30E != 0)return _tmp30E;{struct _tuple6
_tmp310=({struct _tuple6 _tmp30F;_tmp30F.f1=t2;_tmp30F.f2=t1;_tmp30F;});void*
_tmp311;void*_tmp312;void*_tmp313;void*_tmp314;void*_tmp315;struct Cyc_Absyn_Tvar*
_tmp316;void*_tmp317;struct Cyc_Absyn_Tvar*_tmp318;void*_tmp319;struct Cyc_Absyn_AggrInfo
_tmp31A;void*_tmp31B;struct Cyc_List_List*_tmp31C;void*_tmp31D;struct Cyc_Absyn_AggrInfo
_tmp31E;void*_tmp31F;struct Cyc_List_List*_tmp320;void*_tmp321;struct _tuple1*
_tmp322;void*_tmp323;struct _tuple1*_tmp324;void*_tmp325;struct Cyc_List_List*
_tmp326;void*_tmp327;struct Cyc_List_List*_tmp328;void*_tmp329;struct Cyc_Absyn_TunionInfo
_tmp32A;void*_tmp32B;struct Cyc_Absyn_Tuniondecl**_tmp32C;struct Cyc_Absyn_Tuniondecl*
_tmp32D;struct Cyc_List_List*_tmp32E;void*_tmp32F;void*_tmp330;struct Cyc_Absyn_TunionInfo
_tmp331;void*_tmp332;struct Cyc_Absyn_Tuniondecl**_tmp333;struct Cyc_Absyn_Tuniondecl*
_tmp334;struct Cyc_List_List*_tmp335;void*_tmp336;void*_tmp337;struct Cyc_Absyn_TunionFieldInfo
_tmp338;void*_tmp339;struct Cyc_Absyn_Tuniondecl*_tmp33A;struct Cyc_Absyn_Tunionfield*
_tmp33B;struct Cyc_List_List*_tmp33C;void*_tmp33D;struct Cyc_Absyn_TunionFieldInfo
_tmp33E;void*_tmp33F;struct Cyc_Absyn_Tuniondecl*_tmp340;struct Cyc_Absyn_Tunionfield*
_tmp341;struct Cyc_List_List*_tmp342;void*_tmp343;struct Cyc_Absyn_PtrInfo _tmp344;
void*_tmp345;void*_tmp346;struct Cyc_Absyn_Conref*_tmp347;struct Cyc_Absyn_Tqual
_tmp348;struct Cyc_Absyn_Conref*_tmp349;void*_tmp34A;struct Cyc_Absyn_PtrInfo
_tmp34B;void*_tmp34C;void*_tmp34D;struct Cyc_Absyn_Conref*_tmp34E;struct Cyc_Absyn_Tqual
_tmp34F;struct Cyc_Absyn_Conref*_tmp350;void*_tmp351;void*_tmp352;void*_tmp353;
void*_tmp354;void*_tmp355;void*_tmp356;void*_tmp357;void*_tmp358;void*_tmp359;
int _tmp35A;void*_tmp35B;int _tmp35C;void*_tmp35D;void*_tmp35E;struct Cyc_Absyn_Tqual
_tmp35F;struct Cyc_Absyn_Exp*_tmp360;void*_tmp361;void*_tmp362;struct Cyc_Absyn_Tqual
_tmp363;struct Cyc_Absyn_Exp*_tmp364;void*_tmp365;struct Cyc_Absyn_FnInfo _tmp366;
struct Cyc_List_List*_tmp367;struct Cyc_Core_Opt*_tmp368;void*_tmp369;struct Cyc_List_List*
_tmp36A;int _tmp36B;struct Cyc_Absyn_VarargInfo*_tmp36C;struct Cyc_List_List*
_tmp36D;struct Cyc_List_List*_tmp36E;void*_tmp36F;struct Cyc_Absyn_FnInfo _tmp370;
struct Cyc_List_List*_tmp371;struct Cyc_Core_Opt*_tmp372;void*_tmp373;struct Cyc_List_List*
_tmp374;int _tmp375;struct Cyc_Absyn_VarargInfo*_tmp376;struct Cyc_List_List*
_tmp377;struct Cyc_List_List*_tmp378;void*_tmp379;struct Cyc_List_List*_tmp37A;
void*_tmp37B;struct Cyc_List_List*_tmp37C;void*_tmp37D;void*_tmp37E;struct Cyc_List_List*
_tmp37F;void*_tmp380;void*_tmp381;struct Cyc_List_List*_tmp382;void*_tmp383;void*
_tmp384;void*_tmp385;void*_tmp386;void*_tmp387;void*_tmp388;void*_tmp389;void*
_tmp38A;void*_tmp38B;void*_tmp38C;void*_tmp38D;void*_tmp38E;void*_tmp38F;void*
_tmp390;void*_tmp391;int _tmp392;void*_tmp393;int _tmp394;void*_tmp395;void*
_tmp396;void*_tmp397;void*_tmp398;void*_tmp399;void*_tmp39A;_LL287: _tmp311=
_tmp310.f1;if(_tmp311 <= (void*)3?1:*((int*)_tmp311)!= 0)goto _LL289;_tmp312=
_tmp310.f2;if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 0)goto _LL289;_LL288:({void*
_tmp39B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typecmp: can only compare closed types",sizeof(char),39),_tag_arr(
_tmp39B,sizeof(void*),0));});_LL289: _tmp313=_tmp310.f1;if((int)_tmp313 != 0)goto
_LL28B;_tmp314=_tmp310.f2;if((int)_tmp314 != 0)goto _LL28B;_LL28A: return 0;_LL28B:
_tmp315=_tmp310.f1;if(_tmp315 <= (void*)3?1:*((int*)_tmp315)!= 1)goto _LL28D;
_tmp316=((struct Cyc_Absyn_VarType_struct*)_tmp315)->f1;_tmp317=_tmp310.f2;if(
_tmp317 <= (void*)3?1:*((int*)_tmp317)!= 1)goto _LL28D;_tmp318=((struct Cyc_Absyn_VarType_struct*)
_tmp317)->f1;_LL28C: return Cyc_Core_intcmp(*((int*)_check_null(_tmp318->identity)),*((
int*)_check_null(_tmp316->identity)));_LL28D: _tmp319=_tmp310.f1;if(_tmp319 <= (
void*)3?1:*((int*)_tmp319)!= 10)goto _LL28F;_tmp31A=((struct Cyc_Absyn_AggrType_struct*)
_tmp319)->f1;_tmp31B=(void*)_tmp31A.aggr_info;_tmp31C=_tmp31A.targs;_tmp31D=
_tmp310.f2;if(_tmp31D <= (void*)3?1:*((int*)_tmp31D)!= 10)goto _LL28F;_tmp31E=((
struct Cyc_Absyn_AggrType_struct*)_tmp31D)->f1;_tmp31F=(void*)_tmp31E.aggr_info;
_tmp320=_tmp31E.targs;_LL28E: {struct _tuple1*_tmp39D;struct _tuple5 _tmp39C=Cyc_Absyn_aggr_kinded_name(
_tmp31B);_tmp39D=_tmp39C.f2;{struct _tuple1*_tmp39F;struct _tuple5 _tmp39E=Cyc_Absyn_aggr_kinded_name(
_tmp31F);_tmp39F=_tmp39E.f2;{int _tmp3A0=Cyc_Absyn_qvar_cmp(_tmp39D,_tmp39F);if(
_tmp3A0 != 0)return _tmp3A0;else{return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp31C,_tmp320);}}}}_LL28F: _tmp321=_tmp310.f1;if(_tmp321 <= (void*)3?1:*((int*)
_tmp321)!= 12)goto _LL291;_tmp322=((struct Cyc_Absyn_EnumType_struct*)_tmp321)->f1;
_tmp323=_tmp310.f2;if(_tmp323 <= (void*)3?1:*((int*)_tmp323)!= 12)goto _LL291;
_tmp324=((struct Cyc_Absyn_EnumType_struct*)_tmp323)->f1;_LL290: return Cyc_Absyn_qvar_cmp(
_tmp322,_tmp324);_LL291: _tmp325=_tmp310.f1;if(_tmp325 <= (void*)3?1:*((int*)
_tmp325)!= 13)goto _LL293;_tmp326=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp325)->f1;
_tmp327=_tmp310.f2;if(_tmp327 <= (void*)3?1:*((int*)_tmp327)!= 13)goto _LL293;
_tmp328=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp327)->f1;_LL292: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp326,
_tmp328);_LL293: _tmp329=_tmp310.f1;if(_tmp329 <= (void*)3?1:*((int*)_tmp329)!= 2)
goto _LL295;_tmp32A=((struct Cyc_Absyn_TunionType_struct*)_tmp329)->f1;_tmp32B=(
void*)_tmp32A.tunion_info;if(*((int*)_tmp32B)!= 1)goto _LL295;_tmp32C=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp32B)->f1;_tmp32D=*_tmp32C;_tmp32E=_tmp32A.targs;_tmp32F=(void*)_tmp32A.rgn;
_tmp330=_tmp310.f2;if(_tmp330 <= (void*)3?1:*((int*)_tmp330)!= 2)goto _LL295;
_tmp331=((struct Cyc_Absyn_TunionType_struct*)_tmp330)->f1;_tmp332=(void*)_tmp331.tunion_info;
if(*((int*)_tmp332)!= 1)goto _LL295;_tmp333=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp332)->f1;_tmp334=*_tmp333;_tmp335=_tmp331.targs;_tmp336=(void*)_tmp331.rgn;
_LL294: if(_tmp334 == _tmp32D)return 0;{int _tmp3A1=Cyc_Absyn_qvar_cmp(_tmp334->name,
_tmp32D->name);if(_tmp3A1 != 0)return _tmp3A1;{int _tmp3A2=Cyc_Tcutil_typecmp(
_tmp336,_tmp32F);if(_tmp3A2 != 0)return _tmp3A2;return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp335,_tmp32E);}}_LL295: _tmp337=_tmp310.f1;if(_tmp337 <= (void*)3?1:*((int*)
_tmp337)!= 3)goto _LL297;_tmp338=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp337)->f1;_tmp339=(void*)_tmp338.field_info;if(*((int*)_tmp339)!= 1)goto
_LL297;_tmp33A=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp339)->f1;_tmp33B=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp339)->f2;_tmp33C=_tmp338.targs;
_tmp33D=_tmp310.f2;if(_tmp33D <= (void*)3?1:*((int*)_tmp33D)!= 3)goto _LL297;
_tmp33E=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp33D)->f1;_tmp33F=(void*)
_tmp33E.field_info;if(*((int*)_tmp33F)!= 1)goto _LL297;_tmp340=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp33F)->f1;_tmp341=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp33F)->f2;
_tmp342=_tmp33E.targs;_LL296: if(_tmp340 == _tmp33A)return 0;{int _tmp3A3=Cyc_Absyn_qvar_cmp(
_tmp33A->name,_tmp340->name);if(_tmp3A3 != 0)return _tmp3A3;{int _tmp3A4=Cyc_Absyn_qvar_cmp(
_tmp33B->name,_tmp341->name);if(_tmp3A4 != 0)return _tmp3A4;return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp342,_tmp33C);}}_LL297: _tmp343=_tmp310.f1;if(_tmp343 <= (
void*)3?1:*((int*)_tmp343)!= 4)goto _LL299;_tmp344=((struct Cyc_Absyn_PointerType_struct*)
_tmp343)->f1;_tmp345=(void*)_tmp344.elt_typ;_tmp346=(void*)_tmp344.rgn_typ;
_tmp347=_tmp344.nullable;_tmp348=_tmp344.tq;_tmp349=_tmp344.bounds;_tmp34A=
_tmp310.f2;if(_tmp34A <= (void*)3?1:*((int*)_tmp34A)!= 4)goto _LL299;_tmp34B=((
struct Cyc_Absyn_PointerType_struct*)_tmp34A)->f1;_tmp34C=(void*)_tmp34B.elt_typ;
_tmp34D=(void*)_tmp34B.rgn_typ;_tmp34E=_tmp34B.nullable;_tmp34F=_tmp34B.tq;
_tmp350=_tmp34B.bounds;_LL298: {int _tmp3A5=Cyc_Tcutil_typecmp(_tmp34C,_tmp345);
if(_tmp3A5 != 0)return _tmp3A5;{int _tmp3A6=Cyc_Tcutil_typecmp(_tmp34D,_tmp346);if(
_tmp3A6 != 0)return _tmp3A6;{int _tmp3A7=Cyc_Tcutil_tqual_cmp(_tmp34F,_tmp348);if(
_tmp3A7 != 0)return _tmp3A7;{int _tmp3A8=Cyc_Tcutil_conrefs_cmp(Cyc_Tcutil_boundscmp,
_tmp350,_tmp349);if(_tmp3A8 != 0)return _tmp3A8;{void*_tmp3A9=(void*)(Cyc_Absyn_compress_conref(
_tmp350))->v;void*_tmp3AA;_LL2C0: if(_tmp3A9 <= (void*)1?1:*((int*)_tmp3A9)!= 0)
goto _LL2C2;_tmp3AA=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp3A9)->f1;if((
int)_tmp3AA != 0)goto _LL2C2;_LL2C1: return 0;_LL2C2:;_LL2C3: goto _LL2BF;_LL2BF:;}
return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp34E,_tmp347);}}}}_LL299: _tmp351=
_tmp310.f1;if(_tmp351 <= (void*)3?1:*((int*)_tmp351)!= 5)goto _LL29B;_tmp352=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp351)->f1;_tmp353=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp351)->f2;_tmp354=_tmp310.f2;if(_tmp354 <= (void*)3?1:*((int*)_tmp354)!= 5)
goto _LL29B;_tmp355=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp354)->f1;_tmp356=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp354)->f2;_LL29A: if(_tmp355 != _tmp352)
return Cyc_Core_intcmp((int)((unsigned int)_tmp355),(int)((unsigned int)_tmp352));
if(_tmp356 != _tmp353)return Cyc_Core_intcmp((int)((unsigned int)_tmp356),(int)((
unsigned int)_tmp353));return 0;_LL29B: _tmp357=_tmp310.f1;if((int)_tmp357 != 1)
goto _LL29D;_tmp358=_tmp310.f2;if((int)_tmp358 != 1)goto _LL29D;_LL29C: return 0;
_LL29D: _tmp359=_tmp310.f1;if(_tmp359 <= (void*)3?1:*((int*)_tmp359)!= 6)goto
_LL29F;_tmp35A=((struct Cyc_Absyn_DoubleType_struct*)_tmp359)->f1;_tmp35B=_tmp310.f2;
if(_tmp35B <= (void*)3?1:*((int*)_tmp35B)!= 6)goto _LL29F;_tmp35C=((struct Cyc_Absyn_DoubleType_struct*)
_tmp35B)->f1;_LL29E: if(_tmp35A == _tmp35C)return 0;else{if(_tmp35A)return - 1;else{
return 1;}}_LL29F: _tmp35D=_tmp310.f1;if(_tmp35D <= (void*)3?1:*((int*)_tmp35D)!= 7)
goto _LL2A1;_tmp35E=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp35D)->f1;
_tmp35F=((struct Cyc_Absyn_ArrayType_struct*)_tmp35D)->f2;_tmp360=((struct Cyc_Absyn_ArrayType_struct*)
_tmp35D)->f3;_tmp361=_tmp310.f2;if(_tmp361 <= (void*)3?1:*((int*)_tmp361)!= 7)
goto _LL2A1;_tmp362=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp361)->f1;
_tmp363=((struct Cyc_Absyn_ArrayType_struct*)_tmp361)->f2;_tmp364=((struct Cyc_Absyn_ArrayType_struct*)
_tmp361)->f3;_LL2A0: {int _tmp3AB=Cyc_Tcutil_tqual_cmp(_tmp363,_tmp35F);if(
_tmp3AB != 0)return _tmp3AB;{int _tmp3AC=Cyc_Tcutil_typecmp(_tmp362,_tmp35E);if(
_tmp3AC != 0)return _tmp3AC;if(_tmp360 == _tmp364)return 0;if(_tmp360 == 0?1: _tmp364
== 0)({void*_tmp3AD[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("missing expression in array index",sizeof(char),34),_tag_arr(_tmp3AD,
sizeof(void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,
_tmp360,_tmp364);}}_LL2A1: _tmp365=_tmp310.f1;if(_tmp365 <= (void*)3?1:*((int*)
_tmp365)!= 8)goto _LL2A3;_tmp366=((struct Cyc_Absyn_FnType_struct*)_tmp365)->f1;
_tmp367=_tmp366.tvars;_tmp368=_tmp366.effect;_tmp369=(void*)_tmp366.ret_typ;
_tmp36A=_tmp366.args;_tmp36B=_tmp366.c_varargs;_tmp36C=_tmp366.cyc_varargs;
_tmp36D=_tmp366.rgn_po;_tmp36E=_tmp366.attributes;_tmp36F=_tmp310.f2;if(_tmp36F
<= (void*)3?1:*((int*)_tmp36F)!= 8)goto _LL2A3;_tmp370=((struct Cyc_Absyn_FnType_struct*)
_tmp36F)->f1;_tmp371=_tmp370.tvars;_tmp372=_tmp370.effect;_tmp373=(void*)_tmp370.ret_typ;
_tmp374=_tmp370.args;_tmp375=_tmp370.c_varargs;_tmp376=_tmp370.cyc_varargs;
_tmp377=_tmp370.rgn_po;_tmp378=_tmp370.attributes;_LL2A2:({void*_tmp3AE[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: function types not handled",
sizeof(char),36),_tag_arr(_tmp3AE,sizeof(void*),0));});_LL2A3: _tmp379=_tmp310.f1;
if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 9)goto _LL2A5;_tmp37A=((struct Cyc_Absyn_TupleType_struct*)
_tmp379)->f1;_tmp37B=_tmp310.f2;if(_tmp37B <= (void*)3?1:*((int*)_tmp37B)!= 9)
goto _LL2A5;_tmp37C=((struct Cyc_Absyn_TupleType_struct*)_tmp37B)->f1;_LL2A4:
return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp37C,
_tmp37A);_LL2A5: _tmp37D=_tmp310.f1;if(_tmp37D <= (void*)3?1:*((int*)_tmp37D)!= 11)
goto _LL2A7;_tmp37E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp37D)->f1;
_tmp37F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp37D)->f2;_tmp380=_tmp310.f2;
if(_tmp380 <= (void*)3?1:*((int*)_tmp380)!= 11)goto _LL2A7;_tmp381=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp380)->f1;_tmp382=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp380)->f2;_LL2A6: if(_tmp381 != _tmp37E){if(_tmp381 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp382,_tmp37F);_LL2A7: _tmp383=_tmp310.f1;if((int)_tmp383 != 2)goto _LL2A9;
_tmp384=_tmp310.f2;if((int)_tmp384 != 2)goto _LL2A9;_LL2A8: return 0;_LL2A9: _tmp385=
_tmp310.f1;if(_tmp385 <= (void*)3?1:*((int*)_tmp385)!= 15)goto _LL2AB;_tmp386=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp385)->f1;_tmp387=_tmp310.f2;
if(_tmp387 <= (void*)3?1:*((int*)_tmp387)!= 15)goto _LL2AB;_tmp388=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp387)->f1;_LL2AA: return Cyc_Tcutil_typecmp(
_tmp386,_tmp388);_LL2AB: _tmp389=_tmp310.f1;if(_tmp389 <= (void*)3?1:*((int*)
_tmp389)!= 14)goto _LL2AD;_tmp38A=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp389)->f1;_tmp38B=_tmp310.f2;if(_tmp38B <= (void*)3?1:*((int*)_tmp38B)!= 14)
goto _LL2AD;_tmp38C=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp38B)->f1;
_LL2AC: return Cyc_Tcutil_typecmp(_tmp38A,_tmp38C);_LL2AD: _tmp38D=_tmp310.f1;if(
_tmp38D <= (void*)3?1:*((int*)_tmp38D)!= 17)goto _LL2AF;_tmp38E=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp38D)->f1;_tmp38F=_tmp310.f2;if(_tmp38F <= (void*)3?1:*((int*)_tmp38F)!= 17)
goto _LL2AF;_tmp390=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp38F)->f1;_LL2AE:
return Cyc_Tcutil_typecmp(_tmp38E,_tmp390);_LL2AF: _tmp391=_tmp310.f1;if(_tmp391 <= (
void*)3?1:*((int*)_tmp391)!= 18)goto _LL2B1;_tmp392=((struct Cyc_Absyn_TypeInt_struct*)
_tmp391)->f1;_tmp393=_tmp310.f2;if(_tmp393 <= (void*)3?1:*((int*)_tmp393)!= 18)
goto _LL2B1;_tmp394=((struct Cyc_Absyn_TypeInt_struct*)_tmp393)->f1;_LL2B0: return
Cyc_Core_intcmp(_tmp392,_tmp394);_LL2B1: _tmp395=_tmp310.f1;if(_tmp395 <= (void*)3?
1:*((int*)_tmp395)!= 20)goto _LL2B3;_LL2B2: goto _LL2B4;_LL2B3: _tmp396=_tmp310.f2;
if(_tmp396 <= (void*)3?1:*((int*)_tmp396)!= 20)goto _LL2B5;_LL2B4: goto _LL2B6;
_LL2B5: _tmp397=_tmp310.f1;if(_tmp397 <= (void*)3?1:*((int*)_tmp397)!= 19)goto
_LL2B7;_LL2B6: goto _LL2B8;_LL2B7: _tmp398=_tmp310.f1;if(_tmp398 <= (void*)3?1:*((
int*)_tmp398)!= 21)goto _LL2B9;_LL2B8: goto _LL2BA;_LL2B9: _tmp399=_tmp310.f2;if(
_tmp399 <= (void*)3?1:*((int*)_tmp399)!= 21)goto _LL2BB;_LL2BA: goto _LL2BC;_LL2BB:
_tmp39A=_tmp310.f2;if(_tmp39A <= (void*)3?1:*((int*)_tmp39A)!= 19)goto _LL2BD;
_LL2BC:({void*_tmp3AF[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("typecmp: effects not handled",sizeof(char),29),
_tag_arr(_tmp3AF,sizeof(void*),0));});_LL2BD:;_LL2BE:({void*_tmp3B0[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Unmatched case in typecmp",
sizeof(char),26),_tag_arr(_tmp3B0,sizeof(void*),0));});_LL286:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp3B1=Cyc_Tcutil_compress(t);_LL2C5: if(_tmp3B1 <= (void*)3?1:*((
int*)_tmp3B1)!= 5)goto _LL2C7;_LL2C6: goto _LL2C8;_LL2C7: if((int)_tmp3B1 != 1)goto
_LL2C9;_LL2C8: goto _LL2CA;_LL2C9: if(_tmp3B1 <= (void*)3?1:*((int*)_tmp3B1)!= 6)
goto _LL2CB;_LL2CA: goto _LL2CC;_LL2CB: if(_tmp3B1 <= (void*)3?1:*((int*)_tmp3B1)!= 
12)goto _LL2CD;_LL2CC: goto _LL2CE;_LL2CD: if(_tmp3B1 <= (void*)3?1:*((int*)_tmp3B1)
!= 13)goto _LL2CF;_LL2CE: return 1;_LL2CF:;_LL2D0: return 0;_LL2C4:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp3B3=({struct _tuple6 _tmp3B2;_tmp3B2.f1=Cyc_Tcutil_compress(
t1);_tmp3B2.f2=Cyc_Tcutil_compress(t2);_tmp3B2;});void*_tmp3B4;int _tmp3B5;void*
_tmp3B6;int _tmp3B7;void*_tmp3B8;void*_tmp3B9;void*_tmp3BA;void*_tmp3BB;void*
_tmp3BC;void*_tmp3BD;void*_tmp3BE;void*_tmp3BF;void*_tmp3C0;void*_tmp3C1;void*
_tmp3C2;void*_tmp3C3;void*_tmp3C4;void*_tmp3C5;void*_tmp3C6;void*_tmp3C7;void*
_tmp3C8;void*_tmp3C9;void*_tmp3CA;void*_tmp3CB;void*_tmp3CC;void*_tmp3CD;void*
_tmp3CE;void*_tmp3CF;void*_tmp3D0;void*_tmp3D1;void*_tmp3D2;void*_tmp3D3;void*
_tmp3D4;void*_tmp3D5;void*_tmp3D6;void*_tmp3D7;void*_tmp3D8;void*_tmp3D9;void*
_tmp3DA;void*_tmp3DB;void*_tmp3DC;void*_tmp3DD;void*_tmp3DE;void*_tmp3DF;void*
_tmp3E0;void*_tmp3E1;void*_tmp3E2;void*_tmp3E3;void*_tmp3E4;void*_tmp3E5;void*
_tmp3E6;_LL2D2: _tmp3B4=_tmp3B3.f1;if(_tmp3B4 <= (void*)3?1:*((int*)_tmp3B4)!= 6)
goto _LL2D4;_tmp3B5=((struct Cyc_Absyn_DoubleType_struct*)_tmp3B4)->f1;_tmp3B6=
_tmp3B3.f2;if(_tmp3B6 <= (void*)3?1:*((int*)_tmp3B6)!= 6)goto _LL2D4;_tmp3B7=((
struct Cyc_Absyn_DoubleType_struct*)_tmp3B6)->f1;_LL2D3: return !_tmp3B7?_tmp3B5: 0;
_LL2D4: _tmp3B8=_tmp3B3.f1;if(_tmp3B8 <= (void*)3?1:*((int*)_tmp3B8)!= 6)goto
_LL2D6;_tmp3B9=_tmp3B3.f2;if((int)_tmp3B9 != 1)goto _LL2D6;_LL2D5: goto _LL2D7;
_LL2D6: _tmp3BA=_tmp3B3.f1;if(_tmp3BA <= (void*)3?1:*((int*)_tmp3BA)!= 6)goto
_LL2D8;_tmp3BB=_tmp3B3.f2;if(_tmp3BB <= (void*)3?1:*((int*)_tmp3BB)!= 5)goto
_LL2D8;_LL2D7: goto _LL2D9;_LL2D8: _tmp3BC=_tmp3B3.f1;if(_tmp3BC <= (void*)3?1:*((
int*)_tmp3BC)!= 6)goto _LL2DA;_tmp3BD=_tmp3B3.f2;if(_tmp3BD <= (void*)3?1:*((int*)
_tmp3BD)!= 14)goto _LL2DA;_LL2D9: goto _LL2DB;_LL2DA: _tmp3BE=_tmp3B3.f1;if((int)
_tmp3BE != 1)goto _LL2DC;_tmp3BF=_tmp3B3.f2;if(_tmp3BF <= (void*)3?1:*((int*)
_tmp3BF)!= 14)goto _LL2DC;_LL2DB: goto _LL2DD;_LL2DC: _tmp3C0=_tmp3B3.f1;if(_tmp3C0
<= (void*)3?1:*((int*)_tmp3C0)!= 6)goto _LL2DE;_tmp3C1=_tmp3B3.f2;if(_tmp3C1 <= (
void*)3?1:*((int*)_tmp3C1)!= 17)goto _LL2DE;_LL2DD: goto _LL2DF;_LL2DE: _tmp3C2=
_tmp3B3.f1;if((int)_tmp3C2 != 1)goto _LL2E0;_tmp3C3=_tmp3B3.f2;if(_tmp3C3 <= (void*)
3?1:*((int*)_tmp3C3)!= 17)goto _LL2E0;_LL2DF: goto _LL2E1;_LL2E0: _tmp3C4=_tmp3B3.f1;
if((int)_tmp3C4 != 1)goto _LL2E2;_tmp3C5=_tmp3B3.f2;if(_tmp3C5 <= (void*)3?1:*((int*)
_tmp3C5)!= 5)goto _LL2E2;_LL2E1: return 1;_LL2E2: _tmp3C6=_tmp3B3.f1;if(_tmp3C6 <= (
void*)3?1:*((int*)_tmp3C6)!= 5)goto _LL2E4;_tmp3C7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C6)->f2;if((int)_tmp3C7 != 3)goto _LL2E4;_tmp3C8=_tmp3B3.f2;if(_tmp3C8 <= (
void*)3?1:*((int*)_tmp3C8)!= 5)goto _LL2E4;_tmp3C9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C8)->f2;if((int)_tmp3C9 != 3)goto _LL2E4;_LL2E3: return 0;_LL2E4: _tmp3CA=
_tmp3B3.f1;if(_tmp3CA <= (void*)3?1:*((int*)_tmp3CA)!= 5)goto _LL2E6;_tmp3CB=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3CA)->f2;if((int)_tmp3CB != 3)goto _LL2E6;
_LL2E5: goto _LL2E7;_LL2E6: _tmp3CC=_tmp3B3.f1;if(_tmp3CC <= (void*)3?1:*((int*)
_tmp3CC)!= 5)goto _LL2E8;_tmp3CD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3CC)->f2;
if((int)_tmp3CD != 2)goto _LL2E8;_tmp3CE=_tmp3B3.f2;if((int)_tmp3CE != 1)goto _LL2E8;
_LL2E7: goto _LL2E9;_LL2E8: _tmp3CF=_tmp3B3.f1;if(_tmp3CF <= (void*)3?1:*((int*)
_tmp3CF)!= 5)goto _LL2EA;_tmp3D0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3CF)->f2;
if((int)_tmp3D0 != 2)goto _LL2EA;_tmp3D1=_tmp3B3.f2;if(_tmp3D1 <= (void*)3?1:*((int*)
_tmp3D1)!= 5)goto _LL2EA;_tmp3D2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D1)->f2;
if((int)_tmp3D2 != 1)goto _LL2EA;_LL2E9: goto _LL2EB;_LL2EA: _tmp3D3=_tmp3B3.f1;if(
_tmp3D3 <= (void*)3?1:*((int*)_tmp3D3)!= 5)goto _LL2EC;_tmp3D4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D3)->f2;if((int)_tmp3D4 != 2)goto _LL2EC;_tmp3D5=_tmp3B3.f2;if(_tmp3D5 <= (
void*)3?1:*((int*)_tmp3D5)!= 5)goto _LL2EC;_tmp3D6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D5)->f2;if((int)_tmp3D6 != 0)goto _LL2EC;_LL2EB: goto _LL2ED;_LL2EC: _tmp3D7=
_tmp3B3.f1;if(_tmp3D7 <= (void*)3?1:*((int*)_tmp3D7)!= 5)goto _LL2EE;_tmp3D8=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3D7)->f2;if((int)_tmp3D8 != 1)goto _LL2EE;
_tmp3D9=_tmp3B3.f2;if(_tmp3D9 <= (void*)3?1:*((int*)_tmp3D9)!= 5)goto _LL2EE;
_tmp3DA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D9)->f2;if((int)_tmp3DA != 
0)goto _LL2EE;_LL2ED: goto _LL2EF;_LL2EE: _tmp3DB=_tmp3B3.f1;if(_tmp3DB <= (void*)3?1:*((
int*)_tmp3DB)!= 17)goto _LL2F0;_tmp3DC=_tmp3B3.f2;if(_tmp3DC <= (void*)3?1:*((int*)
_tmp3DC)!= 5)goto _LL2F0;_tmp3DD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3DC)->f2;
if((int)_tmp3DD != 1)goto _LL2F0;_LL2EF: goto _LL2F1;_LL2F0: _tmp3DE=_tmp3B3.f1;if(
_tmp3DE <= (void*)3?1:*((int*)_tmp3DE)!= 17)goto _LL2F2;_tmp3DF=_tmp3B3.f2;if(
_tmp3DF <= (void*)3?1:*((int*)_tmp3DF)!= 5)goto _LL2F2;_tmp3E0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3DF)->f2;if((int)_tmp3E0 != 0)goto _LL2F2;_LL2F1: goto _LL2F3;_LL2F2: _tmp3E1=
_tmp3B3.f1;if(_tmp3E1 <= (void*)3?1:*((int*)_tmp3E1)!= 14)goto _LL2F4;_tmp3E2=
_tmp3B3.f2;if(_tmp3E2 <= (void*)3?1:*((int*)_tmp3E2)!= 5)goto _LL2F4;_tmp3E3=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3E2)->f2;if((int)_tmp3E3 != 1)goto _LL2F4;
_LL2F3: goto _LL2F5;_LL2F4: _tmp3E4=_tmp3B3.f1;if(_tmp3E4 <= (void*)3?1:*((int*)
_tmp3E4)!= 14)goto _LL2F6;_tmp3E5=_tmp3B3.f2;if(_tmp3E5 <= (void*)3?1:*((int*)
_tmp3E5)!= 5)goto _LL2F6;_tmp3E6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E5)->f2;
if((int)_tmp3E6 != 0)goto _LL2F6;_LL2F5: return 1;_LL2F6:;_LL2F7: return 0;_LL2D1:;}
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=
el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0?1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->v=(
void*)t1;_tmp3E7;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)((
struct Cyc_Core_Opt*)_check_null(max_arith_type))->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_Std_String_pa_struct _tmp3EA;_tmp3EA.tag=0;_tmp3EA.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp3E9;_tmp3E9.tag=0;_tmp3E9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp3E8[2]={& _tmp3E9,& _tmp3EA};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,
_tag_arr("type mismatch: expecting %s but found %s",sizeof(char),41),_tag_arr(
_tmp3E8,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp3EB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL2F9: if(_tmp3EB <= (void*)3?1:*((int*)_tmp3EB)!= 4)goto _LL2FB;_LL2FA: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ);goto _LL2F8;_LL2FB:;_LL2FC: return 0;_LL2F8:;}return 1;}int
Cyc_Tcutil_is_integral_type(void*t){void*_tmp3EC=Cyc_Tcutil_compress(t);_LL2FE:
if(_tmp3EC <= (void*)3?1:*((int*)_tmp3EC)!= 5)goto _LL300;_LL2FF: goto _LL301;_LL300:
if(_tmp3EC <= (void*)3?1:*((int*)_tmp3EC)!= 14)goto _LL302;_LL301: goto _LL303;
_LL302: if(_tmp3EC <= (void*)3?1:*((int*)_tmp3EC)!= 17)goto _LL304;_LL303: goto
_LL305;_LL304: if(_tmp3EC <= (void*)3?1:*((int*)_tmp3EC)!= 12)goto _LL306;_LL305:
goto _LL307;_LL306: if(_tmp3EC <= (void*)3?1:*((int*)_tmp3EC)!= 13)goto _LL308;
_LL307: return 1;_LL308:;_LL309: return 0;_LL2FD:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp3ED[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3ED,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp3EE[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3EE,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp3F0=({struct _tuple6 _tmp3EF;
_tmp3EF.f1=t1;_tmp3EF.f2=t2;_tmp3EF;});void*_tmp3F1;struct Cyc_Absyn_PtrInfo
_tmp3F2;void*_tmp3F3;struct Cyc_Absyn_PtrInfo _tmp3F4;void*_tmp3F5;void*_tmp3F6;
struct Cyc_Absyn_Tqual _tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;void*_tmp3F9;void*
_tmp3FA;struct Cyc_Absyn_Tqual _tmp3FB;struct Cyc_Absyn_Exp*_tmp3FC;void*_tmp3FD;
struct Cyc_Absyn_TunionFieldInfo _tmp3FE;void*_tmp3FF;struct Cyc_Absyn_Tuniondecl*
_tmp400;struct Cyc_Absyn_Tunionfield*_tmp401;struct Cyc_List_List*_tmp402;void*
_tmp403;struct Cyc_Absyn_TunionInfo _tmp404;void*_tmp405;struct Cyc_Absyn_Tuniondecl**
_tmp406;struct Cyc_Absyn_Tuniondecl*_tmp407;struct Cyc_List_List*_tmp408;void*
_tmp409;struct Cyc_Absyn_PtrInfo _tmp40A;void*_tmp40B;void*_tmp40C;struct Cyc_Absyn_Conref*
_tmp40D;struct Cyc_Absyn_Tqual _tmp40E;struct Cyc_Absyn_Conref*_tmp40F;void*_tmp410;
struct Cyc_Absyn_TunionInfo _tmp411;void*_tmp412;struct Cyc_Absyn_Tuniondecl**
_tmp413;struct Cyc_Absyn_Tuniondecl*_tmp414;struct Cyc_List_List*_tmp415;void*
_tmp416;void*_tmp417;void*_tmp418;void*_tmp419;void*_tmp41A;void*_tmp41B;void*
_tmp41C;_LL30B: _tmp3F1=_tmp3F0.f1;if(_tmp3F1 <= (void*)3?1:*((int*)_tmp3F1)!= 4)
goto _LL30D;_tmp3F2=((struct Cyc_Absyn_PointerType_struct*)_tmp3F1)->f1;_tmp3F3=
_tmp3F0.f2;if(_tmp3F3 <= (void*)3?1:*((int*)_tmp3F3)!= 4)goto _LL30D;_tmp3F4=((
struct Cyc_Absyn_PointerType_struct*)_tmp3F3)->f1;_LL30C: {int okay=1;if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp3F2.nullable,_tmp3F4.nullable)){void*_tmp41D=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp3F2.nullable))->v;int _tmp41E;_LL31A: if(_tmp41D <= (void*)1?1:*((int*)_tmp41D)
!= 0)goto _LL31C;_tmp41E=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp41D)->f1;
_LL31B: okay=!_tmp41E;goto _LL319;_LL31C:;_LL31D:({void*_tmp41F[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("silent_castable conref not eq",
sizeof(char),30),_tag_arr(_tmp41F,sizeof(void*),0));});_LL319:;}if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp3F2.bounds,_tmp3F4.bounds)){struct _tuple6 _tmp421=({
struct _tuple6 _tmp420;_tmp420.f1=(void*)(Cyc_Absyn_compress_conref(_tmp3F2.bounds))->v;
_tmp420.f2=(void*)(Cyc_Absyn_compress_conref(_tmp3F4.bounds))->v;_tmp420;});void*
_tmp422;void*_tmp423;void*_tmp424;void*_tmp425;void*_tmp426;void*_tmp427;struct
Cyc_Absyn_Exp*_tmp428;void*_tmp429;void*_tmp42A;struct Cyc_Absyn_Exp*_tmp42B;void*
_tmp42C;void*_tmp42D;void*_tmp42E;void*_tmp42F;_LL31F: _tmp422=_tmp421.f1;if(
_tmp422 <= (void*)1?1:*((int*)_tmp422)!= 0)goto _LL321;_tmp423=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp422)->f1;if(_tmp423 <= (void*)1?1:*((int*)_tmp423)!= 0)goto _LL321;_tmp424=
_tmp421.f2;if(_tmp424 <= (void*)1?1:*((int*)_tmp424)!= 0)goto _LL321;_tmp425=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp424)->f1;if((int)_tmp425 != 0)goto _LL321;
_LL320: okay=1;goto _LL31E;_LL321: _tmp426=_tmp421.f1;if(_tmp426 <= (void*)1?1:*((
int*)_tmp426)!= 0)goto _LL323;_tmp427=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp426)->f1;if(_tmp427 <= (void*)1?1:*((int*)_tmp427)!= 0)goto _LL323;_tmp428=((
struct Cyc_Absyn_Upper_b_struct*)_tmp427)->f1;_tmp429=_tmp421.f2;if(_tmp429 <= (
void*)1?1:*((int*)_tmp429)!= 0)goto _LL323;_tmp42A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp429)->f1;if(_tmp42A <= (void*)1?1:*((int*)_tmp42A)!= 0)goto _LL323;_tmp42B=((
struct Cyc_Absyn_Upper_b_struct*)_tmp42A)->f1;_LL322: okay=okay?Cyc_Evexp_eval_const_uint_exp(
_tmp428)>= Cyc_Evexp_eval_const_uint_exp(_tmp42B): 0;({void*_tmp430[0]={};Cyc_Tcutil_warn(
loc,_tag_arr("implicit cast to shorter array",sizeof(char),31),_tag_arr(_tmp430,
sizeof(void*),0));});goto _LL31E;_LL323: _tmp42C=_tmp421.f1;if(_tmp42C <= (void*)1?
1:*((int*)_tmp42C)!= 0)goto _LL325;_tmp42D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp42C)->f1;if((int)_tmp42D != 0)goto _LL325;_tmp42E=_tmp421.f2;if(_tmp42E <= (
void*)1?1:*((int*)_tmp42E)!= 0)goto _LL325;_tmp42F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp42E)->f1;if((int)_tmp42F != 0)goto _LL325;_LL324: okay=1;goto _LL31E;_LL325:;
_LL326: okay=0;goto _LL31E;_LL31E:;}okay=okay?Cyc_Tcutil_unify((void*)_tmp3F2.elt_typ,(
void*)_tmp3F4.elt_typ): 0;okay=okay?Cyc_Tcutil_unify((void*)_tmp3F2.rgn_typ,(void*)
_tmp3F4.rgn_typ)?1: Cyc_Tcenv_region_outlives(te,(void*)_tmp3F2.rgn_typ,(void*)
_tmp3F4.rgn_typ): 0;okay=okay?!(_tmp3F2.tq).q_const?1:(_tmp3F4.tq).q_const: 0;
return okay;}_LL30D: _tmp3F5=_tmp3F0.f1;if(_tmp3F5 <= (void*)3?1:*((int*)_tmp3F5)!= 
7)goto _LL30F;_tmp3F6=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3F5)->f1;
_tmp3F7=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F5)->f2;_tmp3F8=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3F5)->f3;_tmp3F9=_tmp3F0.f2;if(_tmp3F9 <= (void*)3?1:*((int*)_tmp3F9)!= 7)
goto _LL30F;_tmp3FA=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3F9)->f1;
_tmp3FB=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F9)->f2;_tmp3FC=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3F9)->f3;_LL30E: {int okay;okay=(_tmp3F8 != 0?_tmp3FC != 0: 0)?Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp3F8))== Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp3FC)): 0;return(okay?Cyc_Tcutil_unify(
_tmp3F6,_tmp3FA): 0)?!_tmp3F7.q_const?1: _tmp3FB.q_const: 0;}_LL30F: _tmp3FD=_tmp3F0.f1;
if(_tmp3FD <= (void*)3?1:*((int*)_tmp3FD)!= 3)goto _LL311;_tmp3FE=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3FD)->f1;_tmp3FF=(void*)_tmp3FE.field_info;if(*((int*)_tmp3FF)!= 1)goto
_LL311;_tmp400=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3FF)->f1;_tmp401=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3FF)->f2;_tmp402=_tmp3FE.targs;
_tmp403=_tmp3F0.f2;if(_tmp403 <= (void*)3?1:*((int*)_tmp403)!= 2)goto _LL311;
_tmp404=((struct Cyc_Absyn_TunionType_struct*)_tmp403)->f1;_tmp405=(void*)_tmp404.tunion_info;
if(*((int*)_tmp405)!= 1)goto _LL311;_tmp406=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp405)->f1;_tmp407=*_tmp406;_tmp408=_tmp404.targs;_LL310: if((_tmp400 == _tmp407?
1: Cyc_Absyn_qvar_cmp(_tmp400->name,_tmp407->name)== 0)?_tmp401->typs == 0: 0){for(
0;_tmp402 != 0?_tmp408 != 0: 0;(_tmp402=_tmp402->tl,_tmp408=_tmp408->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp402->hd,(void*)_tmp408->hd))break;}if(_tmp402 == 0?_tmp408 == 0: 0)return
1;}return 0;_LL311: _tmp409=_tmp3F0.f1;if(_tmp409 <= (void*)3?1:*((int*)_tmp409)!= 
4)goto _LL313;_tmp40A=((struct Cyc_Absyn_PointerType_struct*)_tmp409)->f1;_tmp40B=(
void*)_tmp40A.elt_typ;_tmp40C=(void*)_tmp40A.rgn_typ;_tmp40D=_tmp40A.nullable;
_tmp40E=_tmp40A.tq;_tmp40F=_tmp40A.bounds;_tmp410=_tmp3F0.f2;if(_tmp410 <= (void*)
3?1:*((int*)_tmp410)!= 2)goto _LL313;_tmp411=((struct Cyc_Absyn_TunionType_struct*)
_tmp410)->f1;_tmp412=(void*)_tmp411.tunion_info;if(*((int*)_tmp412)!= 1)goto
_LL313;_tmp413=((struct Cyc_Absyn_KnownTunion_struct*)_tmp412)->f1;_tmp414=*
_tmp413;_tmp415=_tmp411.targs;_tmp416=(void*)_tmp411.rgn;_LL312:{void*_tmp431=
Cyc_Tcutil_compress(_tmp40B);struct Cyc_Absyn_TunionFieldInfo _tmp432;void*_tmp433;
struct Cyc_Absyn_Tuniondecl*_tmp434;struct Cyc_Absyn_Tunionfield*_tmp435;struct Cyc_List_List*
_tmp436;_LL328: if(_tmp431 <= (void*)3?1:*((int*)_tmp431)!= 3)goto _LL32A;_tmp432=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp431)->f1;_tmp433=(void*)_tmp432.field_info;
if(*((int*)_tmp433)!= 1)goto _LL32A;_tmp434=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp433)->f1;_tmp435=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp433)->f2;
_tmp436=_tmp432.targs;_LL329: if(!Cyc_Tcutil_unify(_tmp40C,_tmp416)?!Cyc_Tcenv_region_outlives(
te,_tmp40C,_tmp416): 0)return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp40D,((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(0)))return 0;if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp40F,Cyc_Absyn_new_conref(Cyc_Absyn_bounds_one)))return 0;
if(Cyc_Absyn_qvar_cmp(_tmp414->name,_tmp434->name)== 0?_tmp435->typs != 0: 0){int
okay=1;for(0;_tmp436 != 0?_tmp415 != 0: 0;(_tmp436=_tmp436->tl,_tmp415=_tmp415->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp436->hd,(void*)_tmp415->hd)){okay=0;break;}}if((
!okay?1: _tmp436 != 0)?1: _tmp415 != 0)return 0;return 1;}goto _LL327;_LL32A:;_LL32B:
goto _LL327;_LL327:;}return 0;_LL313: _tmp417=_tmp3F0.f1;if(_tmp417 <= (void*)3?1:*((
int*)_tmp417)!= 14)goto _LL315;_tmp418=_tmp3F0.f2;if(_tmp418 <= (void*)3?1:*((int*)
_tmp418)!= 5)goto _LL315;_tmp419=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp418)->f2;
if((int)_tmp419 != 2)goto _LL315;_LL314: goto _LL316;_LL315: _tmp41A=_tmp3F0.f1;if(
_tmp41A <= (void*)3?1:*((int*)_tmp41A)!= 17)goto _LL317;_tmp41B=_tmp3F0.f2;if(
_tmp41B <= (void*)3?1:*((int*)_tmp41B)!= 5)goto _LL317;_tmp41C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp41B)->f2;if((int)_tmp41C != 2)goto _LL317;_LL316: return 1;_LL317:;_LL318: return
Cyc_Tcutil_unify(t1,t2);_LL30A:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*
_tmp437=Cyc_Tcutil_compress(t);_LL32D: if(_tmp437 <= (void*)3?1:*((int*)_tmp437)!= 
4)goto _LL32F;_LL32E: return 1;_LL32F:;_LL330: return 0;_LL32C:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp438=(void*)e->r;void*_tmp439;int _tmp43A;_LL332:
if(*((int*)_tmp438)!= 0)goto _LL334;_tmp439=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp438)->f1;if(_tmp439 <= (void*)1?1:*((int*)_tmp439)!= 2)goto _LL334;_tmp43A=((
struct Cyc_Absyn_Int_c_struct*)_tmp439)->f2;if(_tmp43A != 0)goto _LL334;_LL333:
return 1;_LL334:;_LL335: return 0;_LL331:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((
void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt
Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)
1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*
e1){if(Cyc_Tcutil_is_pointer_type(t2)?Cyc_Tcutil_is_zero(e1): 0){(void*)(e1->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp43B=_cycalloc(sizeof(*_tmp43B));
_tmp43B[0]=({struct Cyc_Absyn_Const_e_struct _tmp43C;_tmp43C.tag=0;_tmp43C.f1=(
void*)((void*)0);_tmp43C;});_tmp43B;})));{struct Cyc_List_List*_tmp43D=Cyc_Tcenv_lookup_type_vars(
te);struct Cyc_Absyn_PointerType_struct*_tmp43E=({struct Cyc_Absyn_PointerType_struct*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F[0]=({struct Cyc_Absyn_PointerType_struct
_tmp440;_tmp440.tag=4;_tmp440.f1=({struct Cyc_Absyn_PtrInfo _tmp441;_tmp441.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->v=_tmp43D;_tmp443;}));_tmp441.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->v=_tmp43D;_tmp442;}));_tmp441.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp441.tq=Cyc_Absyn_empty_tqual();
_tmp441.bounds=Cyc_Absyn_empty_conref();_tmp441;});_tmp440;});_tmp43F;});(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp43E));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}static int Cyc_Tcutil_is_sizeof_type(void*t1){void*_tmp444=
Cyc_Tcutil_compress(t1);_LL337: if(_tmp444 <= (void*)3?1:*((int*)_tmp444)!= 14)
goto _LL339;_LL338: return 1;_LL339:;_LL33A: return 0;_LL336:;}static int Cyc_Tcutil_is_tag_type(
void*t1){void*_tmp445=Cyc_Tcutil_compress(t1);_LL33C: if(_tmp445 <= (void*)3?1:*((
int*)_tmp445)!= 17)goto _LL33E;_LL33D: return 1;_LL33E:;_LL33F: return 0;_LL33B:;}int
Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
if(Cyc_Tcutil_unify(t1,t2))return 1;if(((Cyc_Tcutil_is_arithmetic_type(t1)?Cyc_Tcutil_is_arithmetic_type(
t2): 0)?1:(Cyc_Tcutil_is_sizeof_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0))?1:(
Cyc_Tcutil_is_tag_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0)){if(Cyc_Tcutil_will_lose_precision(
t1,t2))({struct Cyc_Std_String_pa_struct _tmp448;_tmp448.tag=0;_tmp448.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmp447;
_tmp447.tag=0;_tmp447.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmp446[2]={& _tmp447,& _tmp448};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; %s -> %s conversion supplied",
sizeof(char),53),_tag_arr(_tmp446,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if(Cyc_Tcutil_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);({struct Cyc_Std_String_pa_struct
_tmp44B;_tmp44B.tag=0;_tmp44B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp44A;_tmp44A.tag=0;_tmp44A.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp449[2]={& _tmp44A,& _tmp44B};Cyc_Tcutil_warn(
e->loc,_tag_arr("implicit cast from %s to %s",sizeof(char),28),_tag_arr(_tmp449,
sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp44C=Cyc_Tcutil_compress(t);
_LL341: if(_tmp44C <= (void*)3?1:*((int*)_tmp44C)!= 5)goto _LL343;_LL342: goto _LL344;
_LL343: if((int)_tmp44C != 1)goto _LL345;_LL344: goto _LL346;_LL345: if(_tmp44C <= (
void*)3?1:*((int*)_tmp44C)!= 6)goto _LL347;_LL346: return 1;_LL347:;_LL348: return 0;
_LL340:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*inst,
struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp44D=_cycalloc(sizeof(*
_tmp44D));_tmp44D->f1=x->tq;_tmp44D->f2=Cyc_Tcutil_substitute(inst,(void*)x->type);
_tmp44D;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp44E=t1;struct Cyc_List_List*
_tmp44F;struct Cyc_Absyn_AggrInfo _tmp450;void*_tmp451;struct Cyc_Absyn_Aggrdecl**
_tmp452;struct Cyc_Absyn_Aggrdecl*_tmp453;struct Cyc_List_List*_tmp454;void*
_tmp455;struct Cyc_List_List*_tmp456;_LL34A: if((int)_tmp44E != 0)goto _LL34C;_LL34B:
return 0;_LL34C: if(_tmp44E <= (void*)3?1:*((int*)_tmp44E)!= 9)goto _LL34E;_tmp44F=((
struct Cyc_Absyn_TupleType_struct*)_tmp44E)->f1;_LL34D: return _tmp44F;_LL34E: if(
_tmp44E <= (void*)3?1:*((int*)_tmp44E)!= 10)goto _LL350;_tmp450=((struct Cyc_Absyn_AggrType_struct*)
_tmp44E)->f1;_tmp451=(void*)_tmp450.aggr_info;if(*((int*)_tmp451)!= 1)goto _LL350;
_tmp452=((struct Cyc_Absyn_KnownAggr_struct*)_tmp451)->f1;_tmp453=*_tmp452;
_tmp454=_tmp450.targs;_LL34F: if((void*)_tmp453->kind == (void*)1?1: _tmp453->impl
== 0)return({struct Cyc_List_List*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->hd=({
struct _tuple4*_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->f1=Cyc_Absyn_empty_tqual();
_tmp458->f2=t1;_tmp458;});_tmp457->tl=0;_tmp457;});{struct Cyc_List_List*_tmp459=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp453->tvs,_tmp454);return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp459,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp453->impl))->fields);}_LL350: if(_tmp44E <= (void*)3?1:*((int*)
_tmp44E)!= 11)goto _LL352;_tmp455=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp44E)->f1;if((int)_tmp455 != 0)goto _LL352;_tmp456=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp44E)->f2;_LL351: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp456);_LL352:;_LL353: return({struct Cyc_List_List*
_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->hd=({struct _tuple4*_tmp45B=
_cycalloc(sizeof(*_tmp45B));_tmp45B->f1=Cyc_Absyn_empty_tqual();_tmp45B->f2=t1;
_tmp45B;});_tmp45A->tl=0;_tmp45A;});_LL349:;}}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1)?Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2):
0)return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp45D=({struct _tuple6 _tmp45C;_tmp45C.f1=t1;_tmp45C.f2=t2;_tmp45C;});void*
_tmp45E;struct Cyc_Absyn_PtrInfo _tmp45F;void*_tmp460;void*_tmp461;struct Cyc_Absyn_Conref*
_tmp462;struct Cyc_Absyn_Tqual _tmp463;struct Cyc_Absyn_Conref*_tmp464;void*_tmp465;
struct Cyc_Absyn_PtrInfo _tmp466;void*_tmp467;void*_tmp468;struct Cyc_Absyn_Conref*
_tmp469;struct Cyc_Absyn_Tqual _tmp46A;struct Cyc_Absyn_Conref*_tmp46B;_LL355:
_tmp45E=_tmp45D.f1;if(_tmp45E <= (void*)3?1:*((int*)_tmp45E)!= 4)goto _LL357;
_tmp45F=((struct Cyc_Absyn_PointerType_struct*)_tmp45E)->f1;_tmp460=(void*)
_tmp45F.elt_typ;_tmp461=(void*)_tmp45F.rgn_typ;_tmp462=_tmp45F.nullable;_tmp463=
_tmp45F.tq;_tmp464=_tmp45F.bounds;_tmp465=_tmp45D.f2;if(_tmp465 <= (void*)3?1:*((
int*)_tmp465)!= 4)goto _LL357;_tmp466=((struct Cyc_Absyn_PointerType_struct*)
_tmp465)->f1;_tmp467=(void*)_tmp466.elt_typ;_tmp468=(void*)_tmp466.rgn_typ;
_tmp469=_tmp466.nullable;_tmp46A=_tmp466.tq;_tmp46B=_tmp466.bounds;_LL356: if(
_tmp463.q_const?!_tmp46A.q_const: 0)return 0;if((!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp462,_tmp469)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp462): 0)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp469): 0)
return 0;if(!Cyc_Tcutil_unify(_tmp461,_tmp468)?!Cyc_Tcenv_region_outlives(te,
_tmp461,_tmp468): 0)return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp464,_tmp46B)){struct _tuple6 _tmp46D=({struct _tuple6 _tmp46C;_tmp46C.f1=Cyc_Absyn_conref_val(
_tmp464);_tmp46C.f2=Cyc_Absyn_conref_val(_tmp46B);_tmp46C;});void*_tmp46E;void*
_tmp46F;void*_tmp470;struct Cyc_Absyn_Exp*_tmp471;void*_tmp472;struct Cyc_Absyn_Exp*
_tmp473;_LL35A: _tmp46E=_tmp46D.f1;if(_tmp46E <= (void*)1?1:*((int*)_tmp46E)!= 0)
goto _LL35C;_tmp46F=_tmp46D.f2;if((int)_tmp46F != 0)goto _LL35C;_LL35B: goto _LL359;
_LL35C: _tmp470=_tmp46D.f1;if(_tmp470 <= (void*)1?1:*((int*)_tmp470)!= 0)goto
_LL35E;_tmp471=((struct Cyc_Absyn_Upper_b_struct*)_tmp470)->f1;_tmp472=_tmp46D.f2;
if(_tmp472 <= (void*)1?1:*((int*)_tmp472)!= 0)goto _LL35E;_tmp473=((struct Cyc_Absyn_Upper_b_struct*)
_tmp472)->f1;_LL35D: if(Cyc_Evexp_eval_const_uint_exp(_tmp471)< Cyc_Evexp_eval_const_uint_exp(
_tmp473))return 0;goto _LL359;_LL35E:;_LL35F: return 0;_LL359:;}return Cyc_Tcutil_ptrsubtype(
te,({struct Cyc_List_List*_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474->hd=({
struct _tuple6*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475->f1=t1;_tmp475->f2=t2;
_tmp475;});_tmp474->tl=assume;_tmp474;}),_tmp460,_tmp467);_LL357:;_LL358: return 0;
_LL354:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp477=({
struct _tuple6 _tmp476;_tmp476.f1=Cyc_Tcutil_compress(t1);_tmp476.f2=Cyc_Tcutil_compress(
t2);_tmp476;});void*_tmp478;void*_tmp479;void*_tmp47A;void*_tmp47B;_LL361:
_tmp478=_tmp477.f1;if(_tmp478 <= (void*)3?1:*((int*)_tmp478)!= 5)goto _LL363;
_tmp479=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp478)->f2;_tmp47A=_tmp477.f2;
if(_tmp47A <= (void*)3?1:*((int*)_tmp47A)!= 5)goto _LL363;_tmp47B=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp47A)->f2;_LL362: return _tmp479 == _tmp47B;_LL363:;
_LL364: return 0;_LL360:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp47D;struct
Cyc_Absyn_Tqual _tmp47E;void*_tmp47F;struct _tuple4*_tmp47C=(struct _tuple4*)tqs1->hd;
_tmp47D=*_tmp47C;_tmp47E=_tmp47D.f1;_tmp47F=_tmp47D.f2;{struct _tuple4 _tmp481;
struct Cyc_Absyn_Tqual _tmp482;void*_tmp483;struct _tuple4*_tmp480=(struct _tuple4*)
tqs2->hd;_tmp481=*_tmp480;_tmp482=_tmp481.f1;_tmp483=_tmp481.f2;if(_tmp482.q_const?
Cyc_Tcutil_subtype(te,assume,_tmp47F,_tmp483): 0)continue;else{if(Cyc_Tcutil_unify(
_tmp47F,_tmp483))continue;else{if(Cyc_Tcutil_isomorphic(_tmp47F,_tmp483))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp484=Cyc_Tcutil_compress(t);void*_tmp485;_LL366: if(_tmp484 <= (void*)3?1:*((
int*)_tmp484)!= 5)goto _LL368;_tmp485=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp484)->f2;if((int)_tmp485 != 0)goto _LL368;_LL367: return 1;_LL368:;_LL369: return
0;_LL365:;}int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{void*_tmp486=t2;void*_tmp487;_LL36B: if(_tmp486 <= (
void*)3?1:*((int*)_tmp486)!= 5)goto _LL36D;_tmp487=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp486)->f2;if((int)_tmp487 != 2)goto _LL36D;_LL36C: if(Cyc_Tcutil_typ_kind(t2)== (
void*)2)return 1;goto _LL36A;_LL36D:;_LL36E: goto _LL36A;_LL36A:;}{void*_tmp488=t1;
struct Cyc_Absyn_PtrInfo _tmp489;void*_tmp48A;void*_tmp48B;struct Cyc_Absyn_Conref*
_tmp48C;struct Cyc_Absyn_Tqual _tmp48D;struct Cyc_Absyn_Conref*_tmp48E;void*_tmp48F;
struct Cyc_Absyn_Tqual _tmp490;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Enumdecl*
_tmp492;_LL370: if(_tmp488 <= (void*)3?1:*((int*)_tmp488)!= 4)goto _LL372;_tmp489=((
struct Cyc_Absyn_PointerType_struct*)_tmp488)->f1;_tmp48A=(void*)_tmp489.elt_typ;
_tmp48B=(void*)_tmp489.rgn_typ;_tmp48C=_tmp489.nullable;_tmp48D=_tmp489.tq;
_tmp48E=_tmp489.bounds;_LL371:{void*_tmp493=t2;struct Cyc_Absyn_PtrInfo _tmp494;
void*_tmp495;void*_tmp496;struct Cyc_Absyn_Conref*_tmp497;struct Cyc_Absyn_Tqual
_tmp498;struct Cyc_Absyn_Conref*_tmp499;_LL381: if(_tmp493 <= (void*)3?1:*((int*)
_tmp493)!= 4)goto _LL383;_tmp494=((struct Cyc_Absyn_PointerType_struct*)_tmp493)->f1;
_tmp495=(void*)_tmp494.elt_typ;_tmp496=(void*)_tmp494.rgn_typ;_tmp497=_tmp494.nullable;
_tmp498=_tmp494.tq;_tmp499=_tmp494.bounds;_LL382: {struct Cyc_List_List*_tmp49A=({
struct Cyc_List_List*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3->hd=({struct
_tuple6*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->f1=t1;_tmp4A4->f2=t2;
_tmp4A4;});_tmp4A3->tl=0;_tmp4A3;});int _tmp49B=Cyc_Tcutil_ptrsubtype(te,_tmp49A,
_tmp48A,_tmp495)?!_tmp48D.q_const?1: _tmp498.q_const: 0;int _tmp49C=_tmp49B?0:((Cyc_Tcutil_bits_only(
_tmp48A)?Cyc_Tcutil_is_char_type(_tmp495): 0)?_tmp498.q_const?1: !_tmp48D.q_const:
0);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp48E,_tmp499);
if(!bounds_ok?!_tmp49C: 0){struct _tuple6 _tmp49E=({struct _tuple6 _tmp49D;_tmp49D.f1=
Cyc_Absyn_conref_val(_tmp48E);_tmp49D.f2=Cyc_Absyn_conref_val(_tmp499);_tmp49D;});
void*_tmp49F;struct Cyc_Absyn_Exp*_tmp4A0;void*_tmp4A1;struct Cyc_Absyn_Exp*
_tmp4A2;_LL388: _tmp49F=_tmp49E.f1;if(_tmp49F <= (void*)1?1:*((int*)_tmp49F)!= 0)
goto _LL38A;_tmp4A0=((struct Cyc_Absyn_Upper_b_struct*)_tmp49F)->f1;_tmp4A1=
_tmp49E.f2;if(_tmp4A1 <= (void*)1?1:*((int*)_tmp4A1)!= 0)goto _LL38A;_tmp4A2=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4A1)->f1;_LL389: if(Cyc_Evexp_eval_const_uint_exp(
_tmp4A0)>= Cyc_Evexp_eval_const_uint_exp(_tmp4A2))bounds_ok=1;goto _LL387;_LL38A:;
_LL38B: bounds_ok=1;goto _LL387;_LL387:;}return(bounds_ok?_tmp49B?1: _tmp49C: 0)?Cyc_Tcutil_unify(
_tmp48B,_tmp496)?1: Cyc_Tcenv_region_outlives(te,_tmp48B,_tmp496): 0;}_LL383: if((
int)_tmp493 != 0)goto _LL385;_LL384: return 1;_LL385:;_LL386: goto _LL380;_LL380:;}
return 0;_LL372: if(_tmp488 <= (void*)3?1:*((int*)_tmp488)!= 7)goto _LL374;_tmp48F=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp488)->f1;_tmp490=((struct Cyc_Absyn_ArrayType_struct*)
_tmp488)->f2;_tmp491=((struct Cyc_Absyn_ArrayType_struct*)_tmp488)->f3;_LL373:{
void*_tmp4A5=t2;void*_tmp4A6;struct Cyc_Absyn_Tqual _tmp4A7;struct Cyc_Absyn_Exp*
_tmp4A8;_LL38D: if(_tmp4A5 <= (void*)3?1:*((int*)_tmp4A5)!= 7)goto _LL38F;_tmp4A6=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp4A5)->f1;_tmp4A7=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4A5)->f2;_tmp4A8=((struct Cyc_Absyn_ArrayType_struct*)_tmp4A5)->f3;_LL38E: {
int okay;okay=(_tmp491 != 0?_tmp4A8 != 0: 0)?Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp491))>= Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp4A8)): 0;return(okay?Cyc_Tcutil_unify(_tmp48F,_tmp4A6): 0)?!
_tmp490.q_const?1: _tmp4A7.q_const: 0;}_LL38F:;_LL390: return 0;_LL38C:;}return 0;
_LL374: if(_tmp488 <= (void*)3?1:*((int*)_tmp488)!= 12)goto _LL376;_tmp492=((struct
Cyc_Absyn_EnumType_struct*)_tmp488)->f2;_LL375:{void*_tmp4A9=t2;struct Cyc_Absyn_Enumdecl*
_tmp4AA;_LL392: if(_tmp4A9 <= (void*)3?1:*((int*)_tmp4A9)!= 12)goto _LL394;_tmp4AA=((
struct Cyc_Absyn_EnumType_struct*)_tmp4A9)->f2;_LL393: if((_tmp492->fields != 0?
_tmp4AA->fields != 0: 0)?((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp492->fields))->v)>= ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp4AA->fields))->v): 0)return 1;goto _LL391;_LL394:;_LL395: goto _LL391;
_LL391:;}goto _LL377;_LL376: if(_tmp488 <= (void*)3?1:*((int*)_tmp488)!= 5)goto
_LL378;_LL377: goto _LL379;_LL378: if((int)_tmp488 != 1)goto _LL37A;_LL379: goto _LL37B;
_LL37A: if(_tmp488 <= (void*)3?1:*((int*)_tmp488)!= 6)goto _LL37C;_LL37B: return Cyc_Tcutil_coerceable(
t2)?1: t2 == (void*)0;_LL37C: if((int)_tmp488 != 0)goto _LL37E;_LL37D: return t2 == (
void*)0;_LL37E:;_LL37F: return 0;_LL36F:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp4AB=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp4AC=_cycalloc(
sizeof(*_tmp4AC));_tmp4AC[0]=({struct Cyc_Absyn_Cast_e_struct _tmp4AD;_tmp4AD.tag=
13;_tmp4AD.f1=(void*)t;_tmp4AD.f2=_tmp4AB;_tmp4AD;});_tmp4AC;})));e->topt=({
struct Cyc_Core_Opt*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE->v=(void*)t;
_tmp4AE;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*_tmp4AF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL397: if(_tmp4AF <= (void*)
3?1:*((int*)_tmp4AF)!= 5)goto _LL399;_LL398: goto _LL39A;_LL399: if(_tmp4AF <= (void*)
3?1:*((int*)_tmp4AF)!= 12)goto _LL39B;_LL39A: goto _LL39C;_LL39B: if(_tmp4AF <= (void*)
3?1:*((int*)_tmp4AF)!= 13)goto _LL39D;_LL39C: goto _LL39E;_LL39D: if(_tmp4AF <= (void*)
3?1:*((int*)_tmp4AF)!= 17)goto _LL39F;_LL39E: goto _LL3A0;_LL39F: if(_tmp4AF <= (void*)
3?1:*((int*)_tmp4AF)!= 14)goto _LL3A1;_LL3A0: return 1;_LL3A1: if(_tmp4AF <= (void*)3?
1:*((int*)_tmp4AF)!= 0)goto _LL3A3;_LL3A2: return Cyc_Tcutil_unify((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3A3:;_LL3A4: return
0;_LL396:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp4B0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3A6: if((int)_tmp4B0 != 1)goto _LL3A8;_LL3A7: goto
_LL3A9;_LL3A8: if(_tmp4B0 <= (void*)3?1:*((int*)_tmp4B0)!= 6)goto _LL3AA;_LL3A9:
return 1;_LL3AA:;_LL3AB: return 0;_LL3A5:;}}int Cyc_Tcutil_is_function_type(void*t){
void*_tmp4B1=Cyc_Tcutil_compress(t);_LL3AD: if(_tmp4B1 <= (void*)3?1:*((int*)
_tmp4B1)!= 8)goto _LL3AF;_LL3AE: return 1;_LL3AF:;_LL3B0: return 0;_LL3AC:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple6 _tmp4B3=({struct _tuple6 _tmp4B2;_tmp4B2.f1=t1;
_tmp4B2.f2=t2;_tmp4B2;});void*_tmp4B4;int _tmp4B5;void*_tmp4B6;int _tmp4B7;void*
_tmp4B8;void*_tmp4B9;void*_tmp4BA;void*_tmp4BB;void*_tmp4BC;void*_tmp4BD;void*
_tmp4BE;void*_tmp4BF;void*_tmp4C0;void*_tmp4C1;void*_tmp4C2;void*_tmp4C3;void*
_tmp4C4;void*_tmp4C5;void*_tmp4C6;void*_tmp4C7;void*_tmp4C8;void*_tmp4C9;void*
_tmp4CA;void*_tmp4CB;void*_tmp4CC;void*_tmp4CD;_LL3B2: _tmp4B4=_tmp4B3.f1;if(
_tmp4B4 <= (void*)3?1:*((int*)_tmp4B4)!= 6)goto _LL3B4;_tmp4B5=((struct Cyc_Absyn_DoubleType_struct*)
_tmp4B4)->f1;_tmp4B6=_tmp4B3.f2;if(_tmp4B6 <= (void*)3?1:*((int*)_tmp4B6)!= 6)
goto _LL3B4;_tmp4B7=((struct Cyc_Absyn_DoubleType_struct*)_tmp4B6)->f1;_LL3B3: if(
_tmp4B5)return t1;else{return t2;}_LL3B4: _tmp4B8=_tmp4B3.f1;if(_tmp4B8 <= (void*)3?
1:*((int*)_tmp4B8)!= 6)goto _LL3B6;_LL3B5: return t1;_LL3B6: _tmp4B9=_tmp4B3.f2;if(
_tmp4B9 <= (void*)3?1:*((int*)_tmp4B9)!= 6)goto _LL3B8;_LL3B7: return t2;_LL3B8:
_tmp4BA=_tmp4B3.f1;if((int)_tmp4BA != 1)goto _LL3BA;_LL3B9: goto _LL3BB;_LL3BA:
_tmp4BB=_tmp4B3.f2;if((int)_tmp4BB != 1)goto _LL3BC;_LL3BB: return(void*)1;_LL3BC:
_tmp4BC=_tmp4B3.f1;if(_tmp4BC <= (void*)3?1:*((int*)_tmp4BC)!= 5)goto _LL3BE;
_tmp4BD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4BC)->f1;if((int)_tmp4BD != 
1)goto _LL3BE;_tmp4BE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4BC)->f2;if((
int)_tmp4BE != 3)goto _LL3BE;_LL3BD: goto _LL3BF;_LL3BE: _tmp4BF=_tmp4B3.f2;if(
_tmp4BF <= (void*)3?1:*((int*)_tmp4BF)!= 5)goto _LL3C0;_tmp4C0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4BF)->f1;if((int)_tmp4C0 != 1)goto _LL3C0;_tmp4C1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4BF)->f2;if((int)_tmp4C1 != 3)goto _LL3C0;_LL3BF: return Cyc_Absyn_ulonglong_typ;
_LL3C0: _tmp4C2=_tmp4B3.f1;if(_tmp4C2 <= (void*)3?1:*((int*)_tmp4C2)!= 5)goto
_LL3C2;_tmp4C3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C2)->f1;if((int)
_tmp4C3 != 0)goto _LL3C2;_tmp4C4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C2)->f2;
if((int)_tmp4C4 != 3)goto _LL3C2;_LL3C1: goto _LL3C3;_LL3C2: _tmp4C5=_tmp4B3.f2;if(
_tmp4C5 <= (void*)3?1:*((int*)_tmp4C5)!= 5)goto _LL3C4;_tmp4C6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C5)->f1;if((int)_tmp4C6 != 0)goto _LL3C4;_tmp4C7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C5)->f2;if((int)_tmp4C7 != 3)goto _LL3C4;_LL3C3: return Cyc_Absyn_slonglong_typ;
_LL3C4: _tmp4C8=_tmp4B3.f1;if(_tmp4C8 <= (void*)3?1:*((int*)_tmp4C8)!= 5)goto
_LL3C6;_tmp4C9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C8)->f1;if((int)
_tmp4C9 != 1)goto _LL3C6;_tmp4CA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C8)->f2;
if((int)_tmp4CA != 2)goto _LL3C6;_LL3C5: goto _LL3C7;_LL3C6: _tmp4CB=_tmp4B3.f2;if(
_tmp4CB <= (void*)3?1:*((int*)_tmp4CB)!= 5)goto _LL3C8;_tmp4CC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4CB)->f1;if((int)_tmp4CC != 1)goto _LL3C8;_tmp4CD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4CB)->f2;if((int)_tmp4CD != 2)goto _LL3C8;_LL3C7: return Cyc_Absyn_uint_typ;
_LL3C8:;_LL3C9: return Cyc_Absyn_sint_typ;_LL3B1:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp4CE=(void*)e->r;struct Cyc_Core_Opt*_tmp4CF;
_LL3CB: if(*((int*)_tmp4CE)!= 4)goto _LL3CD;_tmp4CF=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4CE)->f2;if(_tmp4CF != 0)goto _LL3CD;_LL3CC:({void*_tmp4D0[0]={};Cyc_Tcutil_warn(
e->loc,_tag_arr("assignment in test",sizeof(char),19),_tag_arr(_tmp4D0,sizeof(
void*),0));});goto _LL3CA;_LL3CD:;_LL3CE: goto _LL3CA;_LL3CA:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp4D2=({struct _tuple6 _tmp4D1;_tmp4D1.f1=c1;_tmp4D1.f2=c2;_tmp4D1;});
void*_tmp4D3;void*_tmp4D4;void*_tmp4D5;void*_tmp4D6;void*_tmp4D7;struct Cyc_Core_Opt*
_tmp4D8;struct Cyc_Core_Opt**_tmp4D9;void*_tmp4DA;struct Cyc_Core_Opt*_tmp4DB;
struct Cyc_Core_Opt**_tmp4DC;void*_tmp4DD;struct Cyc_Core_Opt*_tmp4DE;struct Cyc_Core_Opt**
_tmp4DF;void*_tmp4E0;void*_tmp4E1;void*_tmp4E2;void*_tmp4E3;void*_tmp4E4;void*
_tmp4E5;struct Cyc_Core_Opt*_tmp4E6;struct Cyc_Core_Opt**_tmp4E7;void*_tmp4E8;void*
_tmp4E9;struct Cyc_Core_Opt*_tmp4EA;struct Cyc_Core_Opt**_tmp4EB;void*_tmp4EC;void*
_tmp4ED;struct Cyc_Core_Opt*_tmp4EE;struct Cyc_Core_Opt**_tmp4EF;void*_tmp4F0;
_LL3D0: _tmp4D3=_tmp4D2.f1;if(*((int*)_tmp4D3)!= 0)goto _LL3D2;_tmp4D4=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp4D3)->f1;_tmp4D5=_tmp4D2.f2;if(*((int*)_tmp4D5)
!= 0)goto _LL3D2;_tmp4D6=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4D5)->f1;
_LL3D1: return _tmp4D4 == _tmp4D6;_LL3D2: _tmp4D7=_tmp4D2.f2;if(*((int*)_tmp4D7)!= 1)
goto _LL3D4;_tmp4D8=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4D7)->f1;_tmp4D9=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4D7)->f1;_LL3D3:*
_tmp4D9=({struct Cyc_Core_Opt*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1->v=(
void*)c1;_tmp4F1;});return 1;_LL3D4: _tmp4DA=_tmp4D2.f1;if(*((int*)_tmp4DA)!= 1)
goto _LL3D6;_tmp4DB=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4DA)->f1;_tmp4DC=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4DA)->f1;_LL3D5:*
_tmp4DC=({struct Cyc_Core_Opt*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->v=(
void*)c2;_tmp4F2;});return 1;_LL3D6: _tmp4DD=_tmp4D2.f1;if(*((int*)_tmp4DD)!= 2)
goto _LL3D8;_tmp4DE=((struct Cyc_Absyn_Less_kb_struct*)_tmp4DD)->f1;_tmp4DF=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp4DD)->f1;_tmp4E0=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4DD)->f2;_tmp4E1=_tmp4D2.f2;if(*((
int*)_tmp4E1)!= 0)goto _LL3D8;_tmp4E2=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp4E1)->f1;_LL3D7: if(Cyc_Tcutil_kind_leq(_tmp4E2,_tmp4E0)){*_tmp4DF=({struct
Cyc_Core_Opt*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3->v=(void*)c2;_tmp4F3;});
return 1;}else{return 0;}_LL3D8: _tmp4E3=_tmp4D2.f1;if(*((int*)_tmp4E3)!= 0)goto
_LL3DA;_tmp4E4=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4E3)->f1;_tmp4E5=
_tmp4D2.f2;if(*((int*)_tmp4E5)!= 2)goto _LL3DA;_tmp4E6=((struct Cyc_Absyn_Less_kb_struct*)
_tmp4E5)->f1;_tmp4E7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp4E5)->f1;_tmp4E8=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4E5)->f2;
_LL3D9: if(Cyc_Tcutil_kind_leq(_tmp4E4,_tmp4E8)){*_tmp4E7=({struct Cyc_Core_Opt*
_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->v=(void*)c1;_tmp4F4;});return 1;}
else{return 0;}_LL3DA: _tmp4E9=_tmp4D2.f1;if(*((int*)_tmp4E9)!= 2)goto _LL3CF;
_tmp4EA=((struct Cyc_Absyn_Less_kb_struct*)_tmp4E9)->f1;_tmp4EB=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4E9)->f1;_tmp4EC=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4E9)->f2;_tmp4ED=_tmp4D2.f2;if(*((int*)_tmp4ED)!= 2)goto _LL3CF;_tmp4EE=((
struct Cyc_Absyn_Less_kb_struct*)_tmp4ED)->f1;_tmp4EF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4ED)->f1;_tmp4F0=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4ED)->f2;_LL3DB: if(Cyc_Tcutil_kind_leq(_tmp4EC,_tmp4F0)){*_tmp4EF=({struct
Cyc_Core_Opt*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->v=(void*)c1;_tmp4F5;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp4F0,_tmp4EC)){*_tmp4EB=({struct Cyc_Core_Opt*
_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->v=(void*)c2;_tmp4F6;});return 1;}
else{return 0;}}_LL3CF:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp4F7=_cycalloc_atomic(sizeof(*_tmp4F7));_tmp4F7[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp4F7;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp4FB;_tmp4FB.tag=1;_tmp4FB.f1=(int)((unsigned int)
i);{void*_tmp4FA[1]={& _tmp4FB};Cyc_Std_aprintf(_tag_arr("#%d",sizeof(char),4),
_tag_arr(_tmp4FA,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*_tmp4F8=
_cycalloc(sizeof(*_tmp4F8));_tmp4F8->name=({struct _tagged_arr*_tmp4F9=_cycalloc(
sizeof(struct _tagged_arr)* 1);_tmp4F9[0]=s;_tmp4F9;});_tmp4F8->identity=0;
_tmp4F8->kind=(void*)k;_tmp4F8;});}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _tagged_arr _tmp4FC=*t->name;return*((const char*)
_check_unknown_subscript(_tmp4FC,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_Std_String_pa_struct _tmp4FE;_tmp4FE.tag=0;
_tmp4FE.f1=(struct _tagged_arr)*t->name;{void*_tmp4FD[1]={& _tmp4FE};Cyc_Std_printf(
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp4FD,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp4FF=Cyc_Std_strconcat(_tag_arr("`",sizeof(char),
2),*t->name);*((char*)_check_unknown_subscript(_tmp4FF,sizeof(char),1))='t';t->name=({
struct _tagged_arr*_tmp500=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp500[0]=(
struct _tagged_arr)_tmp4FF;_tmp500;});}}struct _tuple12{struct _tagged_arr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple12*x){return({struct _tuple2*_tmp501=_cycalloc(sizeof(*_tmp501));
_tmp501->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp502=_cycalloc(sizeof(*
_tmp502));_tmp502->v=(*x).f1;_tmp502;});_tmp501->f2=(*x).f2;_tmp501->f3=(*x).f3;
_tmp501;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp503=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp503=({
struct Cyc_List_List*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->hd=(void*)((
void*)atts->hd);_tmp504->tl=_tmp503;_tmp504;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505[0]=({struct Cyc_Absyn_FnType_struct
_tmp506;_tmp506.tag=8;_tmp506.f1=({struct Cyc_Absyn_FnInfo _tmp507;_tmp507.tvars=
fd->tvs;_tmp507.effect=fd->effect;_tmp507.ret_typ=(void*)((void*)fd->ret_type);
_tmp507.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp507.c_varargs=
fd->c_varargs;_tmp507.cyc_varargs=fd->cyc_varargs;_tmp507.rgn_po=fd->rgn_po;
_tmp507.attributes=_tmp503;_tmp507;});_tmp506;});_tmp505;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple13{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple13*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508->f1=(*pr).f1;_tmp508->f2=t;_tmp508;});}
struct _tuple14{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple15{
struct _tuple14*f1;void*f2;};static struct _tuple15*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple15*_tmp509=_region_malloc(
rgn,sizeof(*_tmp509));_tmp509->f1=({struct _tuple14*_tmp50A=_region_malloc(rgn,
sizeof(*_tmp50A));_tmp50A->f1=(*y).f1;_tmp50A->f2=(*y).f2;_tmp50A;});_tmp509->f2=(*
y).f3;_tmp509;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple15*w){
struct _tuple14*_tmp50C;void*_tmp50D;struct _tuple15 _tmp50B=*w;_tmp50C=_tmp50B.f1;
_tmp50D=_tmp50B.f2;{struct Cyc_Core_Opt*_tmp50F;struct Cyc_Absyn_Tqual _tmp510;
struct _tuple14 _tmp50E=*_tmp50C;_tmp50F=_tmp50E.f1;_tmp510=_tmp50E.f2;return({
struct _tuple2*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->f1=_tmp50F;_tmp511->f2=
_tmp510;_tmp511->f3=_tmp50D;_tmp511;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp512=
_cycalloc(sizeof(*_tmp512));_tmp512->name=f->name;_tmp512->tq=f->tq;_tmp512->type=(
void*)t;_tmp512->width=f->width;_tmp512->attributes=f->attributes;_tmp512;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp513=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp514;struct Cyc_Absyn_AggrInfo _tmp515;void*_tmp516;struct Cyc_List_List*_tmp517;
struct Cyc_Absyn_TunionInfo _tmp518;void*_tmp519;struct Cyc_List_List*_tmp51A;void*
_tmp51B;struct Cyc_Absyn_TunionFieldInfo _tmp51C;void*_tmp51D;struct Cyc_List_List*
_tmp51E;struct _tuple1*_tmp51F;struct Cyc_List_List*_tmp520;struct Cyc_Absyn_Typedefdecl*
_tmp521;struct Cyc_Core_Opt*_tmp522;void*_tmp523;struct Cyc_Absyn_Tqual _tmp524;
struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_PtrInfo _tmp526;void*_tmp527;void*
_tmp528;struct Cyc_Absyn_Conref*_tmp529;struct Cyc_Absyn_Tqual _tmp52A;struct Cyc_Absyn_Conref*
_tmp52B;struct Cyc_Absyn_FnInfo _tmp52C;struct Cyc_List_List*_tmp52D;struct Cyc_Core_Opt*
_tmp52E;void*_tmp52F;struct Cyc_List_List*_tmp530;int _tmp531;struct Cyc_Absyn_VarargInfo*
_tmp532;struct Cyc_List_List*_tmp533;struct Cyc_List_List*_tmp534;struct Cyc_List_List*
_tmp535;void*_tmp536;struct Cyc_List_List*_tmp537;struct Cyc_Core_Opt*_tmp538;void*
_tmp539;void*_tmp53A;void*_tmp53B;void*_tmp53C;void*_tmp53D;struct Cyc_List_List*
_tmp53E;_LL3DD: if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 1)goto _LL3DF;_tmp514=((
struct Cyc_Absyn_VarType_struct*)_tmp513)->f1;_LL3DE: {struct _handler_cons _tmp53F;
_push_handler(& _tmp53F);{int _tmp541=0;if(setjmp(_tmp53F.handler))_tmp541=1;if(!
_tmp541){{void*_tmp542=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp514);_npop_handler(0);return _tmp542;};_pop_handler();}else{void*_tmp540=(
void*)_exn_thrown;void*_tmp544=_tmp540;_LL410: if(_tmp544 != Cyc_Core_Not_found)
goto _LL412;_LL411: return t;_LL412:;_LL413:(void)_throw(_tmp544);_LL40F:;}}}_LL3DF:
if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 10)goto _LL3E1;_tmp515=((struct Cyc_Absyn_AggrType_struct*)
_tmp513)->f1;_tmp516=(void*)_tmp515.aggr_info;_tmp517=_tmp515.targs;_LL3E0: {
struct Cyc_List_List*_tmp545=Cyc_Tcutil_substs(rgn,inst,_tmp517);return _tmp545 == 
_tmp517?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp546=_cycalloc(sizeof(*
_tmp546));_tmp546[0]=({struct Cyc_Absyn_AggrType_struct _tmp547;_tmp547.tag=10;
_tmp547.f1=({struct Cyc_Absyn_AggrInfo _tmp548;_tmp548.aggr_info=(void*)_tmp516;
_tmp548.targs=_tmp545;_tmp548;});_tmp547;});_tmp546;});}_LL3E1: if(_tmp513 <= (
void*)3?1:*((int*)_tmp513)!= 2)goto _LL3E3;_tmp518=((struct Cyc_Absyn_TunionType_struct*)
_tmp513)->f1;_tmp519=(void*)_tmp518.tunion_info;_tmp51A=_tmp518.targs;_tmp51B=(
void*)_tmp518.rgn;_LL3E2: {struct Cyc_List_List*_tmp549=Cyc_Tcutil_substs(rgn,
inst,_tmp51A);void*_tmp54A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp51B);return(
_tmp549 == _tmp51A?_tmp54A == _tmp51B: 0)?t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B[0]=({struct Cyc_Absyn_TunionType_struct
_tmp54C;_tmp54C.tag=2;_tmp54C.f1=({struct Cyc_Absyn_TunionInfo _tmp54D;_tmp54D.tunion_info=(
void*)_tmp519;_tmp54D.targs=_tmp549;_tmp54D.rgn=(void*)_tmp54A;_tmp54D;});
_tmp54C;});_tmp54B;});}_LL3E3: if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 3)goto
_LL3E5;_tmp51C=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp513)->f1;_tmp51D=(
void*)_tmp51C.field_info;_tmp51E=_tmp51C.targs;_LL3E4: {struct Cyc_List_List*
_tmp54E=Cyc_Tcutil_substs(rgn,inst,_tmp51E);return _tmp54E == _tmp51E?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));
_tmp54F[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp550;_tmp550.tag=3;_tmp550.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp551;_tmp551.field_info=(void*)_tmp51D;_tmp551.targs=
_tmp54E;_tmp551;});_tmp550;});_tmp54F;});}_LL3E5: if(_tmp513 <= (void*)3?1:*((int*)
_tmp513)!= 16)goto _LL3E7;_tmp51F=((struct Cyc_Absyn_TypedefType_struct*)_tmp513)->f1;
_tmp520=((struct Cyc_Absyn_TypedefType_struct*)_tmp513)->f2;_tmp521=((struct Cyc_Absyn_TypedefType_struct*)
_tmp513)->f3;_tmp522=((struct Cyc_Absyn_TypedefType_struct*)_tmp513)->f4;_LL3E6: {
struct Cyc_List_List*_tmp552=Cyc_Tcutil_substs(rgn,inst,_tmp520);return _tmp552 == 
_tmp520?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp553=_cycalloc(sizeof(*
_tmp553));_tmp553[0]=({struct Cyc_Absyn_TypedefType_struct _tmp554;_tmp554.tag=16;
_tmp554.f1=_tmp51F;_tmp554.f2=_tmp552;_tmp554.f3=_tmp521;_tmp554.f4=_tmp522;
_tmp554;});_tmp553;});}_LL3E7: if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 7)goto
_LL3E9;_tmp523=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp513)->f1;_tmp524=((
struct Cyc_Absyn_ArrayType_struct*)_tmp513)->f2;_tmp525=((struct Cyc_Absyn_ArrayType_struct*)
_tmp513)->f3;_LL3E8: {void*_tmp555=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp523);
return _tmp555 == _tmp523?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp556=
_cycalloc(sizeof(*_tmp556));_tmp556[0]=({struct Cyc_Absyn_ArrayType_struct _tmp557;
_tmp557.tag=7;_tmp557.f1=(void*)_tmp555;_tmp557.f2=_tmp524;_tmp557.f3=_tmp525;
_tmp557;});_tmp556;});}_LL3E9: if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 4)goto
_LL3EB;_tmp526=((struct Cyc_Absyn_PointerType_struct*)_tmp513)->f1;_tmp527=(void*)
_tmp526.elt_typ;_tmp528=(void*)_tmp526.rgn_typ;_tmp529=_tmp526.nullable;_tmp52A=
_tmp526.tq;_tmp52B=_tmp526.bounds;_LL3EA: {void*_tmp558=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp527);void*_tmp559=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp528);if(
_tmp558 == _tmp527?_tmp559 == _tmp528: 0)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A[0]=({struct Cyc_Absyn_PointerType_struct
_tmp55B;_tmp55B.tag=4;_tmp55B.f1=({struct Cyc_Absyn_PtrInfo _tmp55C;_tmp55C.elt_typ=(
void*)_tmp558;_tmp55C.rgn_typ=(void*)_tmp559;_tmp55C.nullable=_tmp529;_tmp55C.tq=
_tmp52A;_tmp55C.bounds=_tmp52B;_tmp55C;});_tmp55B;});_tmp55A;});}_LL3EB: if(
_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 8)goto _LL3ED;_tmp52C=((struct Cyc_Absyn_FnType_struct*)
_tmp513)->f1;_tmp52D=_tmp52C.tvars;_tmp52E=_tmp52C.effect;_tmp52F=(void*)_tmp52C.ret_typ;
_tmp530=_tmp52C.args;_tmp531=_tmp52C.c_varargs;_tmp532=_tmp52C.cyc_varargs;
_tmp533=_tmp52C.rgn_po;_tmp534=_tmp52C.attributes;_LL3EC:{struct Cyc_List_List*
_tmp55D=_tmp52D;for(0;_tmp55D != 0;_tmp55D=_tmp55D->tl){inst=({struct Cyc_List_List*
_tmp55E=_region_malloc(rgn,sizeof(*_tmp55E));_tmp55E->hd=({struct _tuple7*_tmp55F=
_region_malloc(rgn,sizeof(*_tmp55F));_tmp55F->f1=(struct Cyc_Absyn_Tvar*)_tmp55D->hd;
_tmp55F->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp560=_cycalloc(sizeof(*
_tmp560));_tmp560[0]=({struct Cyc_Absyn_VarType_struct _tmp561;_tmp561.tag=1;
_tmp561.f1=(struct Cyc_Absyn_Tvar*)_tmp55D->hd;_tmp561;});_tmp560;});_tmp55F;});
_tmp55E->tl=inst;_tmp55E;});}}{struct Cyc_List_List*_tmp563;struct Cyc_List_List*
_tmp564;struct _tuple0 _tmp562=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple15*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp530));_tmp563=_tmp562.f1;_tmp564=_tmp562.f2;{struct Cyc_List_List*_tmp565=
Cyc_Tcutil_substs(rgn,inst,_tmp564);struct Cyc_List_List*_tmp566=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp563,_tmp565));struct Cyc_Core_Opt*
eff2;if(_tmp52E == 0)eff2=0;else{void*_tmp567=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp52E->v);if(_tmp567 == (void*)_tmp52E->v)eff2=_tmp52E;else{eff2=({struct
Cyc_Core_Opt*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->v=(void*)_tmp567;
_tmp568;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp532 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp56A;struct Cyc_Absyn_Tqual _tmp56B;void*
_tmp56C;int _tmp56D;struct Cyc_Absyn_VarargInfo _tmp569=*_tmp532;_tmp56A=_tmp569.name;
_tmp56B=_tmp569.tq;_tmp56C=(void*)_tmp569.type;_tmp56D=_tmp569.inject;{void*
_tmp56E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp56C);if(_tmp56E == _tmp56C)
cyc_varargs2=_tmp532;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp56F=
_cycalloc(sizeof(*_tmp56F));_tmp56F->name=_tmp56A;_tmp56F->tq=_tmp56B;_tmp56F->type=(
void*)_tmp56E;_tmp56F->inject=_tmp56D;_tmp56F;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp571;struct Cyc_List_List*_tmp572;struct _tuple0 _tmp570=Cyc_List_rsplit(
rgn,rgn,_tmp533);_tmp571=_tmp570.f1;_tmp572=_tmp570.f2;{struct Cyc_List_List*
_tmp573=Cyc_Tcutil_substs(rgn,inst,_tmp571);struct Cyc_List_List*_tmp574=Cyc_Tcutil_substs(
rgn,inst,_tmp572);if(_tmp573 == _tmp571?_tmp574 == _tmp572: 0)rgn_po2=_tmp533;else{
rgn_po2=Cyc_List_zip(_tmp573,_tmp574);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575[0]=({struct Cyc_Absyn_FnType_struct
_tmp576;_tmp576.tag=8;_tmp576.f1=({struct Cyc_Absyn_FnInfo _tmp577;_tmp577.tvars=
_tmp52D;_tmp577.effect=eff2;_tmp577.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp52F);_tmp577.args=_tmp566;_tmp577.c_varargs=_tmp531;_tmp577.cyc_varargs=
cyc_varargs2;_tmp577.rgn_po=rgn_po2;_tmp577.attributes=_tmp534;_tmp577;});
_tmp576;});_tmp575;});}}}}}_LL3ED: if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 9)
goto _LL3EF;_tmp535=((struct Cyc_Absyn_TupleType_struct*)_tmp513)->f1;_LL3EE: {
struct Cyc_List_List*_tmp578=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp535);struct Cyc_List_List*_tmp579=Cyc_Tcutil_substs(rgn,inst,_tmp578);if(
_tmp579 == _tmp578)return t;{struct Cyc_List_List*_tmp57A=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp535,_tmp579);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B[0]=({struct Cyc_Absyn_TupleType_struct
_tmp57C;_tmp57C.tag=9;_tmp57C.f1=_tmp57A;_tmp57C;});_tmp57B;});}}_LL3EF: if(
_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 11)goto _LL3F1;_tmp536=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp513)->f1;_tmp537=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp513)->f2;_LL3F0: {
struct Cyc_List_List*_tmp57D=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp537);struct Cyc_List_List*_tmp57E=Cyc_Tcutil_substs(rgn,inst,_tmp57D);if(
_tmp57E == _tmp57D)return t;{struct Cyc_List_List*_tmp57F=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp537,
_tmp57E);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp580=_cycalloc(
sizeof(*_tmp580));_tmp580[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp581;
_tmp581.tag=11;_tmp581.f1=(void*)_tmp536;_tmp581.f2=_tmp57F;_tmp581;});_tmp580;});}}
_LL3F1: if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 0)goto _LL3F3;_tmp538=((struct
Cyc_Absyn_Evar_struct*)_tmp513)->f2;_LL3F2: if(_tmp538 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp538->v);else{return t;}_LL3F3: if(_tmp513 <= (void*)3?1:*((int*)
_tmp513)!= 15)goto _LL3F5;_tmp539=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp513)->f1;_LL3F4: {void*_tmp582=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp539);
return _tmp582 == _tmp539?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp583=
_cycalloc(sizeof(*_tmp583));_tmp583[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp584;_tmp584.tag=15;_tmp584.f1=(void*)_tmp582;_tmp584;});_tmp583;});}_LL3F5:
if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 14)goto _LL3F7;_tmp53A=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp513)->f1;_LL3F6: {void*_tmp585=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp53A);return _tmp585 == _tmp53A?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp587;_tmp587.tag=14;_tmp587.f1=(void*)_tmp585;_tmp587;});_tmp586;});}_LL3F7:
if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 17)goto _LL3F9;_tmp53B=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp513)->f1;_LL3F8: {void*_tmp588=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp53B);return _tmp588 == _tmp53B?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589[0]=({struct Cyc_Absyn_TagType_struct
_tmp58A;_tmp58A.tag=17;_tmp58A.f1=(void*)_tmp588;_tmp58A;});_tmp589;});}_LL3F9:
if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 18)goto _LL3FB;_LL3FA: goto _LL3FC;
_LL3FB: if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 12)goto _LL3FD;_LL3FC: goto
_LL3FE;_LL3FD: if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 13)goto _LL3FF;_LL3FE:
goto _LL400;_LL3FF: if((int)_tmp513 != 0)goto _LL401;_LL400: goto _LL402;_LL401: if(
_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 5)goto _LL403;_LL402: goto _LL404;_LL403:
if((int)_tmp513 != 1)goto _LL405;_LL404: goto _LL406;_LL405: if(_tmp513 <= (void*)3?1:*((
int*)_tmp513)!= 6)goto _LL407;_LL406: goto _LL408;_LL407: if((int)_tmp513 != 2)goto
_LL409;_LL408: return t;_LL409: if(_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 21)goto
_LL40B;_tmp53C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp513)->f1;_LL40A: {
void*_tmp58B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp53C);return _tmp58B == _tmp53C?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp58C=_cycalloc(sizeof(*_tmp58C));
_tmp58C[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp58D;_tmp58D.tag=21;_tmp58D.f1=(
void*)_tmp58B;_tmp58D;});_tmp58C;});}_LL40B: if(_tmp513 <= (void*)3?1:*((int*)
_tmp513)!= 19)goto _LL40D;_tmp53D=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp513)->f1;_LL40C: {void*_tmp58E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp53D);
return _tmp58E == _tmp53D?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp58F=
_cycalloc(sizeof(*_tmp58F));_tmp58F[0]=({struct Cyc_Absyn_AccessEff_struct _tmp590;
_tmp590.tag=19;_tmp590.f1=(void*)_tmp58E;_tmp590;});_tmp58F;});}_LL40D: if(
_tmp513 <= (void*)3?1:*((int*)_tmp513)!= 20)goto _LL3DC;_tmp53E=((struct Cyc_Absyn_JoinEff_struct*)
_tmp513)->f1;_LL40E: {struct Cyc_List_List*_tmp591=Cyc_Tcutil_substs(rgn,inst,
_tmp53E);return _tmp591 == _tmp53E?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp593;_tmp593.tag=20;_tmp593.f1=_tmp591;_tmp593;});_tmp592;});}_LL3DC:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp594=(void*)ts->hd;
struct Cyc_List_List*_tmp595=ts->tl;void*_tmp596=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp594);struct Cyc_List_List*_tmp597=Cyc_Tcutil_substs(rgn,inst,_tmp595);if(
_tmp594 == _tmp596?_tmp595 == _tmp597: 0)return ts;return(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598->hd=(void*)
_tmp596;_tmp598->tl=_tmp597;_tmp598;});}}extern void*Cyc_Tcutil_substitute(struct
Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple7*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple7*_tmp599=_cycalloc(
sizeof(*_tmp599));_tmp599->f1=tv;_tmp599->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->v=(void*)k;_tmp59A;}),({struct Cyc_Core_Opt*
_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->v=s;_tmp59B;}));_tmp599;});}struct
_tuple7*Cyc_Tcutil_r_make_inst_var(struct _tuple8*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple8 _tmp59D;struct Cyc_List_List*_tmp59E;struct _RegionHandle*_tmp59F;
struct _tuple8*_tmp59C=env;_tmp59D=*_tmp59C;_tmp59E=_tmp59D.f1;_tmp59F=_tmp59D.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple7*_tmp5A0=_region_malloc(
_tmp59F,sizeof(*_tmp5A0));_tmp5A0->f1=tv;_tmp5A0->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->v=(void*)k;_tmp5A1;}),({
struct Cyc_Core_Opt*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->v=_tmp59E;
_tmp5A2;}));_tmp5A0;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_Std_zstrptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_Std_String_pa_struct _tmp5A6;_tmp5A6.tag=0;_tmp5A6.f1=(struct
_tagged_arr)Cyc_Absynpp_kindbound2string(k2);{struct Cyc_Std_String_pa_struct
_tmp5A5;_tmp5A5.tag=0;_tmp5A5.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k1);{struct Cyc_Std_String_pa_struct _tmp5A4;_tmp5A4.tag=0;_tmp5A4.f1=(struct
_tagged_arr)*tv->name;{void*_tmp5A3[3]={& _tmp5A4,& _tmp5A5,& _tmp5A6};Cyc_Tcutil_terr(
loc,_tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof(char),59),_tag_arr(_tmp5A3,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp5A7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("same type variable has different identity!",sizeof(char),43),_tag_arr(
_tmp5A7,sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->hd=tv;
_tmp5A8->tl=tvs;_tmp5A8;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*
_tmp5A9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv",sizeof(char),39),_tag_arr(
_tmp5A9,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp5AA[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof(char),41),_tag_arr(_tmp5AA,sizeof(void*),0));});if(*((int*)_check_null(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)_check_null(tv->identity)))
return tvs;}}return({struct Cyc_List_List*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));
_tmp5AB->hd=tv;_tmp5AB->tl=tvs;_tmp5AB;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_Std_String_pa_struct
_tmp5AD;_tmp5AD.tag=0;_tmp5AD.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string(tv);{
void*_tmp5AC[1]={& _tmp5AD};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bound tvar id for %s is NULL",sizeof(char),29),
_tag_arr(_tmp5AC,sizeof(void*),1));}});return({struct Cyc_List_List*_tmp5AE=
_cycalloc(sizeof(*_tmp5AE));_tmp5AE->hd=tv;_tmp5AE->tl=tvs;_tmp5AE;});}static
struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){void*
_tmp5AF=Cyc_Tcutil_compress(e);int _tmp5B0;_LL415: if(_tmp5AF <= (void*)3?1:*((int*)
_tmp5AF)!= 0)goto _LL417;_tmp5B0=((struct Cyc_Absyn_Evar_struct*)_tmp5AF)->f3;
_LL416:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*_tmp5B1=Cyc_Tcutil_compress((
void*)es2->hd);int _tmp5B2;_LL41A: if(_tmp5B1 <= (void*)3?1:*((int*)_tmp5B1)!= 0)
goto _LL41C;_tmp5B2=((struct Cyc_Absyn_Evar_struct*)_tmp5B1)->f3;_LL41B: if(_tmp5B0
== _tmp5B2)return es;goto _LL419;_LL41C:;_LL41D: goto _LL419;_LL419:;}}return({
struct Cyc_List_List*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->hd=(void*)e;
_tmp5B3->tl=es;_tmp5B3;});_LL417:;_LL418: return es;_LL414:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))
== *((int*)_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;
break;}}}if(!present)r=({struct Cyc_List_List*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));
_tmp5B4->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp5B4->tl=r;_tmp5B4;});}r=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(width)))({struct Cyc_Std_String_pa_struct
_tmp5B6;_tmp5B6.tag=0;_tmp5B6.f1=(struct _tagged_arr)*fn;{void*_tmp5B5[1]={&
_tmp5B6};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s does not have constant width",
sizeof(char),41),_tag_arr(_tmp5B5,sizeof(void*),1));}});else{w=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(width));}{void*_tmp5B7=Cyc_Tcutil_compress(
field_typ);void*_tmp5B8;_LL41F: if(_tmp5B7 <= (void*)3?1:*((int*)_tmp5B7)!= 5)goto
_LL421;_tmp5B8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5B7)->f2;_LL420:{
void*_tmp5B9=_tmp5B8;_LL424: if((int)_tmp5B9 != 0)goto _LL426;_LL425: if(w > 8)({void*
_tmp5BA[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",sizeof(
char),26),_tag_arr(_tmp5BA,sizeof(void*),0));});goto _LL423;_LL426: if((int)
_tmp5B9 != 1)goto _LL428;_LL427: if(w > 16)({void*_tmp5BB[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp5BB,sizeof(
void*),0));});goto _LL423;_LL428: if((int)_tmp5B9 != 2)goto _LL42A;_LL429: if(w > 32)({
void*_tmp5BC[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(char),26),_tag_arr(_tmp5BC,sizeof(void*),0));});goto _LL423;_LL42A: if((int)
_tmp5B9 != 3)goto _LL423;_LL42B: if(w > 64)({void*_tmp5BD[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp5BD,sizeof(
void*),0));});goto _LL423;_LL423:;}goto _LL41E;_LL421:;_LL422:({struct Cyc_Std_String_pa_struct
_tmp5C0;_tmp5C0.tag=0;_tmp5C0.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
field_typ);{struct Cyc_Std_String_pa_struct _tmp5BF;_tmp5BF.tag=0;_tmp5BF.f1=(
struct _tagged_arr)*fn;{void*_tmp5BE[2]={& _tmp5BF,& _tmp5C0};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield %s must have integral type but has type %s",sizeof(char),52),
_tag_arr(_tmp5BE,sizeof(void*),2));}}});goto _LL41E;_LL41E:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp5C1=(void*)atts->hd;_LL42D: if((int)
_tmp5C1 != 5)goto _LL42F;_LL42E: continue;_LL42F: if(_tmp5C1 <= (void*)16?1:*((int*)
_tmp5C1)!= 1)goto _LL431;_LL430: continue;_LL431:;_LL432:({struct Cyc_Std_String_pa_struct
_tmp5C4;_tmp5C4.tag=0;_tmp5C4.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_String_pa_struct
_tmp5C3;_tmp5C3.tag=0;_tmp5C3.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp5C2[2]={& _tmp5C3,& _tmp5C4};Cyc_Tcutil_terr(loc,
_tag_arr("bad attribute %s on member %s",sizeof(char),30),_tag_arr(_tmp5C2,
sizeof(void*),2));}}});_LL42C:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};struct _tuple16{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t){{void*_tmp5C5=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp5C6;struct Cyc_Core_Opt**_tmp5C7;struct Cyc_Core_Opt*
_tmp5C8;struct Cyc_Core_Opt**_tmp5C9;struct Cyc_Absyn_Tvar*_tmp5CA;struct Cyc_List_List*
_tmp5CB;struct _tuple1*_tmp5CC;struct Cyc_Absyn_Enumdecl*_tmp5CD;struct Cyc_Absyn_Enumdecl**
_tmp5CE;struct Cyc_Absyn_TunionInfo _tmp5CF;void*_tmp5D0;void**_tmp5D1;struct Cyc_List_List*
_tmp5D2;struct Cyc_List_List**_tmp5D3;void*_tmp5D4;struct Cyc_Absyn_TunionFieldInfo
_tmp5D5;void*_tmp5D6;void**_tmp5D7;struct Cyc_List_List*_tmp5D8;struct Cyc_Absyn_PtrInfo
_tmp5D9;void*_tmp5DA;void*_tmp5DB;struct Cyc_Absyn_Conref*_tmp5DC;struct Cyc_Absyn_Tqual
_tmp5DD;struct Cyc_Absyn_Conref*_tmp5DE;void*_tmp5DF;void*_tmp5E0;void*_tmp5E1;
struct Cyc_Absyn_Tqual _tmp5E2;struct Cyc_Absyn_Exp*_tmp5E3;struct Cyc_Absyn_FnInfo
_tmp5E4;struct Cyc_List_List*_tmp5E5;struct Cyc_List_List**_tmp5E6;struct Cyc_Core_Opt*
_tmp5E7;struct Cyc_Core_Opt**_tmp5E8;void*_tmp5E9;struct Cyc_List_List*_tmp5EA;int
_tmp5EB;struct Cyc_Absyn_VarargInfo*_tmp5EC;struct Cyc_List_List*_tmp5ED;struct Cyc_List_List*
_tmp5EE;struct Cyc_List_List*_tmp5EF;void*_tmp5F0;struct Cyc_List_List*_tmp5F1;
struct Cyc_Absyn_AggrInfo _tmp5F2;void*_tmp5F3;void**_tmp5F4;struct Cyc_List_List*
_tmp5F5;struct Cyc_List_List**_tmp5F6;struct _tuple1*_tmp5F7;struct Cyc_List_List*
_tmp5F8;struct Cyc_List_List**_tmp5F9;struct Cyc_Absyn_Typedefdecl*_tmp5FA;struct
Cyc_Absyn_Typedefdecl**_tmp5FB;struct Cyc_Core_Opt*_tmp5FC;struct Cyc_Core_Opt**
_tmp5FD;void*_tmp5FE;void*_tmp5FF;void*_tmp600;struct Cyc_List_List*_tmp601;
_LL434: if((int)_tmp5C5 != 0)goto _LL436;_LL435: goto _LL433;_LL436: if(_tmp5C5 <= (
void*)3?1:*((int*)_tmp5C5)!= 0)goto _LL438;_tmp5C6=((struct Cyc_Absyn_Evar_struct*)
_tmp5C5)->f1;_tmp5C7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp5C5)->f1;_tmp5C8=((struct Cyc_Absyn_Evar_struct*)_tmp5C5)->f2;_tmp5C9=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp5C5)->f2;_LL437: if(*_tmp5C7 == 
0)*_tmp5C7=({struct Cyc_Core_Opt*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->v=(
void*)expected_kind;_tmp602;});if((cvtenv.fn_result?cvtenv.generalize_evars: 0)?
expected_kind == (void*)3: 0)*_tmp5C9=({struct Cyc_Core_Opt*_tmp603=_cycalloc(
sizeof(*_tmp603));_tmp603->v=(void*)((void*)2);_tmp603;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp604=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp609;_tmp609.tag=2;_tmp609.f1=0;_tmp609.f2=(void*)expected_kind;_tmp609;});
_tmp608;}));*_tmp5C9=({struct Cyc_Core_Opt*_tmp605=_cycalloc(sizeof(*_tmp605));
_tmp605->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp606=_cycalloc(
sizeof(*_tmp606));_tmp606[0]=({struct Cyc_Absyn_VarType_struct _tmp607;_tmp607.tag=
1;_tmp607.f1=_tmp604;_tmp607;});_tmp606;}));_tmp605;});_tmp5CA=_tmp604;goto
_LL439;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL433;_LL438: if(_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 1)goto _LL43A;
_tmp5CA=((struct Cyc_Absyn_VarType_struct*)_tmp5C5)->f1;_LL439:{void*_tmp60A=Cyc_Absyn_compress_kb((
void*)_tmp5CA->kind);struct Cyc_Core_Opt*_tmp60B;struct Cyc_Core_Opt**_tmp60C;
_LL467: if(*((int*)_tmp60A)!= 1)goto _LL469;_tmp60B=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp60A)->f1;_tmp60C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp60A)->f1;_LL468:*_tmp60C=({struct Cyc_Core_Opt*_tmp60D=_cycalloc(sizeof(*
_tmp60D));_tmp60D->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp60E=
_cycalloc(sizeof(*_tmp60E));_tmp60E[0]=({struct Cyc_Absyn_Less_kb_struct _tmp60F;
_tmp60F.tag=2;_tmp60F.f1=0;_tmp60F.f2=(void*)expected_kind;_tmp60F;});_tmp60E;}));
_tmp60D;});goto _LL466;_LL469:;_LL46A: goto _LL466;_LL466:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp5CA);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp5CA);goto _LL433;_LL43A: if(_tmp5C5 <= (void*)3?1:*((int*)
_tmp5C5)!= 13)goto _LL43C;_tmp5CB=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp5C5)->f1;
_LL43B: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp610=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp610;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp5CB != 0;_tmp5CB=_tmp5CB->tl){struct Cyc_Absyn_Enumfield*_tmp611=(
struct Cyc_Absyn_Enumfield*)_tmp5CB->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp611->name).f2))({struct Cyc_Std_String_pa_struct
_tmp613;_tmp613.tag=0;_tmp613.f1=(struct _tagged_arr)*(*_tmp611->name).f2;{void*
_tmp612[1]={& _tmp613};Cyc_Tcutil_terr(_tmp611->loc,_tag_arr("duplicate enum field name %s",
sizeof(char),29),_tag_arr(_tmp612,sizeof(void*),1));}});else{prev_fields=({
struct Cyc_List_List*_tmp614=_region_malloc(uprev_rgn,sizeof(*_tmp614));_tmp614->hd=(*
_tmp611->name).f2;_tmp614->tl=prev_fields;_tmp614;});}if(_tmp611->tag == 0)
_tmp611->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp611->loc);
else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp611->tag)))({
struct Cyc_Std_String_pa_struct _tmp616;_tmp616.tag=0;_tmp616.f1=(struct
_tagged_arr)*(*_tmp611->name).f2;{void*_tmp615[1]={& _tmp616};Cyc_Tcutil_terr(loc,
_tag_arr("enum field %s: expression is not constant",sizeof(char),42),_tag_arr(
_tmp615,sizeof(void*),1));}});}{unsigned int t1=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp611->tag));tag_count=t1 + 1;(*_tmp611->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp618;_tmp618.tag=1;_tmp618.f1=te->ns;_tmp618;});
_tmp617;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple16*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp611->name).f2,({
struct _tuple16*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp61B;_tmp61B.tag=4;_tmp61B.f1=(void*)t;
_tmp61B.f2=_tmp611;_tmp61B;});_tmp61A;});_tmp619->f2=1;_tmp619;}));}}};
_pop_region(uprev_rgn);}goto _LL433;}_LL43C: if(_tmp5C5 <= (void*)3?1:*((int*)
_tmp5C5)!= 12)goto _LL43E;_tmp5CC=((struct Cyc_Absyn_EnumType_struct*)_tmp5C5)->f1;
_tmp5CD=((struct Cyc_Absyn_EnumType_struct*)_tmp5C5)->f2;_tmp5CE=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp5C5)->f2;_LL43D: if(*_tmp5CE == 0?1:((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp5CE))->fields == 0){struct _handler_cons
_tmp61C;_push_handler(& _tmp61C);{int _tmp61E=0;if(setjmp(_tmp61C.handler))_tmp61E=
1;if(!_tmp61E){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp5CC);*_tmp5CE=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp61D=(void*)_exn_thrown;void*_tmp620=_tmp61D;_LL46C: if(_tmp620 != Cyc_Dict_Absent)
goto _LL46E;_LL46D: {struct Cyc_Tcenv_Genv*_tmp621=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp622=({struct Cyc_Absyn_Enumdecl*_tmp623=_cycalloc(
sizeof(*_tmp623));_tmp623->sc=(void*)((void*)3);_tmp623->name=_tmp5CC;_tmp623->fields=
0;_tmp623;});Cyc_Tc_tcEnumdecl(te,_tmp621,loc,_tmp622);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp5CC);*_tmp5CE=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL46B;}}_LL46E:;_LL46F:(void)_throw(_tmp620);_LL46B:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp5CE);*_tmp5CC=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL433;}_LL43E: if(_tmp5C5 <= (void*)3?1:*((
int*)_tmp5C5)!= 2)goto _LL440;_tmp5CF=((struct Cyc_Absyn_TunionType_struct*)
_tmp5C5)->f1;_tmp5D0=(void*)_tmp5CF.tunion_info;_tmp5D1=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp5C5)->f1).tunion_info;_tmp5D2=_tmp5CF.targs;_tmp5D3=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp5C5)->f1).targs;_tmp5D4=(void*)_tmp5CF.rgn;
_LL43F: {struct Cyc_List_List*_tmp624=*_tmp5D3;{void*_tmp625=*_tmp5D1;struct Cyc_Absyn_UnknownTunionInfo
_tmp626;struct _tuple1*_tmp627;int _tmp628;struct Cyc_Absyn_Tuniondecl**_tmp629;
struct Cyc_Absyn_Tuniondecl*_tmp62A;_LL471: if(*((int*)_tmp625)!= 0)goto _LL473;
_tmp626=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp625)->f1;_tmp627=_tmp626.name;
_tmp628=_tmp626.is_xtunion;_LL472: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp62B;_push_handler(& _tmp62B);{int _tmp62D=0;if(setjmp(_tmp62B.handler))
_tmp62D=1;if(!_tmp62D){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp627);;
_pop_handler();}else{void*_tmp62C=(void*)_exn_thrown;void*_tmp62F=_tmp62C;_LL476:
if(_tmp62F != Cyc_Dict_Absent)goto _LL478;_LL477: {struct Cyc_Tcenv_Genv*_tmp630=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp631=({struct Cyc_Absyn_Tuniondecl*
_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->sc=(void*)((void*)3);_tmp635->name=
_tmp627;_tmp635->tvs=0;_tmp635->fields=0;_tmp635->is_xtunion=_tmp628;_tmp635;});
Cyc_Tc_tcTuniondecl(te,_tmp630,loc,_tmp631);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp627);if(_tmp624 != 0){({struct Cyc_Std_String_pa_struct _tmp634;_tmp634.tag=
0;_tmp634.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp627);{struct Cyc_Std_String_pa_struct
_tmp633;_tmp633.tag=0;_tmp633.f1=(struct _tagged_arr)(_tmp628?_tag_arr("xtunion",
sizeof(char),8): _tag_arr("tunion",sizeof(char),7));{void*_tmp632[2]={& _tmp633,&
_tmp634};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized %s %s before using",
sizeof(char),48),_tag_arr(_tmp632,sizeof(void*),2));}}});return cvtenv;}goto
_LL475;}_LL478:;_LL479:(void)_throw(_tmp62F);_LL475:;}}}if((*tudp)->is_xtunion != 
_tmp628)({struct Cyc_Std_String_pa_struct _tmp637;_tmp637.tag=0;_tmp637.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp627);{void*_tmp636[1]={& _tmp637};Cyc_Tcutil_terr(
loc,_tag_arr("[x]tunion is different from type declaration %s",sizeof(char),48),
_tag_arr(_tmp636,sizeof(void*),1));}});*_tmp5D1=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp639;_tmp639.tag=1;_tmp639.f1=tudp;_tmp639;});_tmp638;});_tmp62A=*tudp;goto
_LL474;}_LL473: if(*((int*)_tmp625)!= 1)goto _LL470;_tmp629=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp625)->f1;_tmp62A=*_tmp629;_LL474: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5D4);{struct Cyc_List_List*tvs=_tmp62A->tvs;for(0;_tmp624 != 0?
tvs != 0: 0;(_tmp624=_tmp624->tl,tvs=tvs->tl)){void*t1=(void*)_tmp624->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp624 != 0)({struct Cyc_Std_String_pa_struct _tmp63B;
_tmp63B.tag=0;_tmp63B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp62A->name);{
void*_tmp63A[1]={& _tmp63B};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s",
sizeof(char),38),_tag_arr(_tmp63A,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp63C=
_cycalloc(sizeof(*_tmp63C));_tmp63C->hd=(void*)e;_tmp63C->tl=hidden_ts;_tmp63C;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*_tmp5D3=Cyc_List_imp_append(*
_tmp5D3,Cyc_List_imp_rev(hidden_ts));}goto _LL470;}_LL470:;}goto _LL433;}_LL440:
if(_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 3)goto _LL442;_tmp5D5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5C5)->f1;_tmp5D6=(void*)_tmp5D5.field_info;_tmp5D7=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5C5)->f1).field_info;_tmp5D8=_tmp5D5.targs;_LL441:{void*_tmp63D=*_tmp5D7;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp63E;struct _tuple1*_tmp63F;struct
_tuple1*_tmp640;int _tmp641;struct Cyc_Absyn_Tuniondecl*_tmp642;struct Cyc_Absyn_Tunionfield*
_tmp643;_LL47B: if(*((int*)_tmp63D)!= 0)goto _LL47D;_tmp63E=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp63D)->f1;_tmp63F=_tmp63E.tunion_name;_tmp640=_tmp63E.field_name;_tmp641=
_tmp63E.is_xtunion;_LL47C: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp644;_push_handler(& _tmp644);{int _tmp646=0;if(setjmp(
_tmp644.handler))_tmp646=1;if(!_tmp646){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp63F);;_pop_handler();}else{void*_tmp645=(void*)_exn_thrown;void*_tmp648=
_tmp645;_LL480: if(_tmp648 != Cyc_Dict_Absent)goto _LL482;_LL481:({struct Cyc_Std_String_pa_struct
_tmp64A;_tmp64A.tag=0;_tmp64A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp63F);{void*_tmp649[1]={& _tmp64A};Cyc_Tcutil_terr(loc,_tag_arr("unbound type tunion %s",
sizeof(char),23),_tag_arr(_tmp649,sizeof(void*),1));}});return cvtenv;_LL482:;
_LL483:(void)_throw(_tmp648);_LL47F:;}}}{struct _handler_cons _tmp64B;
_push_handler(& _tmp64B);{int _tmp64D=0;if(setjmp(_tmp64B.handler))_tmp64D=1;if(!
_tmp64D){{void*_tmp64E=Cyc_Tcenv_lookup_ordinary(te,loc,_tmp640);struct Cyc_Absyn_Tuniondecl*
_tmp64F;struct Cyc_Absyn_Tunionfield*_tmp650;_LL485: if(*((int*)_tmp64E)!= 2)goto
_LL487;_tmp64F=((struct Cyc_Tcenv_TunionRes_struct*)_tmp64E)->f1;_tmp650=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp64E)->f2;_LL486: tuf=_tmp650;tud=_tmp64F;if(tud->is_xtunion
!= _tmp641)({struct Cyc_Std_String_pa_struct _tmp652;_tmp652.tag=0;_tmp652.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp63F);{void*_tmp651[1]={& _tmp652};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(char),48),_tag_arr(_tmp651,sizeof(void*),1));}});goto _LL484;_LL487:;
_LL488:({struct Cyc_Std_String_pa_struct _tmp655;_tmp655.tag=0;_tmp655.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp63F);{struct Cyc_Std_String_pa_struct
_tmp654;_tmp654.tag=0;_tmp654.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp640);{void*_tmp653[2]={& _tmp654,& _tmp655};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp653,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv
_tmp656=cvtenv;_npop_handler(0);return _tmp656;}_LL484:;};_pop_handler();}else{
void*_tmp64C=(void*)_exn_thrown;void*_tmp658=_tmp64C;_LL48A: if(_tmp658 != Cyc_Dict_Absent)
goto _LL48C;_LL48B:({struct Cyc_Std_String_pa_struct _tmp65B;_tmp65B.tag=0;_tmp65B.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp63F);{struct Cyc_Std_String_pa_struct
_tmp65A;_tmp65A.tag=0;_tmp65A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp640);{void*_tmp659[2]={& _tmp65A,& _tmp65B};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp659,sizeof(void*),2));}}});return cvtenv;_LL48C:;
_LL48D:(void)_throw(_tmp658);_LL489:;}}}*_tmp5D7=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp65D;_tmp65D.tag=1;_tmp65D.f1=tud;_tmp65D.f2=tuf;_tmp65D;});_tmp65C;});
_tmp642=tud;_tmp643=tuf;goto _LL47E;}_LL47D: if(*((int*)_tmp63D)!= 1)goto _LL47A;
_tmp642=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp63D)->f1;_tmp643=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp63D)->f2;_LL47E: {struct Cyc_List_List*tvs=
_tmp642->tvs;for(0;_tmp5D8 != 0?tvs != 0: 0;(_tmp5D8=_tmp5D8->tl,tvs=tvs->tl)){void*
t1=(void*)_tmp5D8->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp5D8 != 0)({
struct Cyc_Std_String_pa_struct _tmp660;_tmp660.tag=0;_tmp660.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp643->name);{struct Cyc_Std_String_pa_struct
_tmp65F;_tmp65F.tag=0;_tmp65F.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp642->name);{void*_tmp65E[2]={& _tmp65F,& _tmp660};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s.%s",
sizeof(char),41),_tag_arr(_tmp65E,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_Std_String_pa_struct
_tmp663;_tmp663.tag=0;_tmp663.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp643->name);{struct Cyc_Std_String_pa_struct _tmp662;_tmp662.tag=0;_tmp662.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp642->name);{void*_tmp661[2]={&
_tmp662,& _tmp663};Cyc_Tcutil_terr(loc,_tag_arr("too few type arguments for tunion %s.%s",
sizeof(char),40),_tag_arr(_tmp661,sizeof(void*),2));}}});goto _LL47A;}_LL47A:;}
goto _LL433;_LL442: if(_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 4)goto _LL444;
_tmp5D9=((struct Cyc_Absyn_PointerType_struct*)_tmp5C5)->f1;_tmp5DA=(void*)
_tmp5D9.elt_typ;_tmp5DB=(void*)_tmp5D9.rgn_typ;_tmp5DC=_tmp5D9.nullable;_tmp5DD=
_tmp5D9.tq;_tmp5DE=_tmp5D9.bounds;_LL443: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp5DA);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5DB);{void*_tmp664=(void*)(Cyc_Absyn_compress_conref(_tmp5DE))->v;
void*_tmp665;struct Cyc_Absyn_Exp*_tmp666;_LL48F: if(_tmp664 <= (void*)1?1:*((int*)
_tmp664)!= 0)goto _LL491;_tmp665=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp664)->f1;if(_tmp665 <= (void*)1?1:*((int*)_tmp665)!= 0)goto _LL491;_tmp666=((
struct Cyc_Absyn_Upper_b_struct*)_tmp665)->f1;_LL490: if(!Cyc_Tcutil_is_const_exp(
te,_tmp666))({void*_tmp667[0]={};Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not a constant",
sizeof(char),44),_tag_arr(_tmp667,sizeof(void*),0));});Cyc_Tcexp_tcExp(te,0,
_tmp666);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp666))({void*_tmp668[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("pointer bounds expression is not an unsigned int",sizeof(char),49),
_tag_arr(_tmp668,sizeof(void*),0));});Cyc_Evexp_eval_const_uint_exp(_tmp666);
goto _LL48E;_LL491:;_LL492: goto _LL48E;_LL48E:;}goto _LL433;_LL444: if(_tmp5C5 <= (
void*)3?1:*((int*)_tmp5C5)!= 14)goto _LL446;_tmp5DF=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp5C5)->f1;_LL445: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,
_tmp5DF);goto _LL433;_LL446: if(_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 17)goto
_LL448;_tmp5E0=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp5C5)->f1;_LL447:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp5E0);goto _LL433;
_LL448: if(_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 18)goto _LL44A;_LL449: goto
_LL44B;_LL44A: if(_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 5)goto _LL44C;_LL44B:
goto _LL44D;_LL44C: if((int)_tmp5C5 != 1)goto _LL44E;_LL44D: goto _LL44F;_LL44E: if(
_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 6)goto _LL450;_LL44F: goto _LL433;_LL450:
if(_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 7)goto _LL452;_tmp5E1=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp5C5)->f1;_tmp5E2=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5C5)->f2;_tmp5E3=((struct Cyc_Absyn_ArrayType_struct*)_tmp5C5)->f3;_LL451:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,_tmp5E1);if(_tmp5E3
== 0?1: !Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp5E3)))({
void*_tmp669[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array bounds expression is not constant",
sizeof(char),40),_tag_arr(_tmp669,sizeof(void*),0));});Cyc_Tcexp_tcExp(te,0,(
struct Cyc_Absyn_Exp*)_check_null(_tmp5E3));if(!Cyc_Tcutil_coerce_uint_typ(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp5E3)))({void*_tmp66A[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("array bounds expression is not an unsigned int",sizeof(char),47),
_tag_arr(_tmp66A,sizeof(void*),0));});Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp5E3));goto _LL433;_LL452: if(_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)
!= 8)goto _LL454;_tmp5E4=((struct Cyc_Absyn_FnType_struct*)_tmp5C5)->f1;_tmp5E5=
_tmp5E4.tvars;_tmp5E6=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp5C5)->f1).tvars;_tmp5E7=_tmp5E4.effect;_tmp5E8=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp5C5)->f1).effect;_tmp5E9=(void*)_tmp5E4.ret_typ;
_tmp5EA=_tmp5E4.args;_tmp5EB=_tmp5E4.c_varargs;_tmp5EC=_tmp5E4.cyc_varargs;
_tmp5ED=_tmp5E4.rgn_po;_tmp5EE=_tmp5E4.attributes;_LL453: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp5EE != 0;_tmp5EE=_tmp5EE->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp5EE->hd))({struct Cyc_Std_String_pa_struct
_tmp66C;_tmp66C.tag=0;_tmp66C.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp5EE->hd);{void*_tmp66B[1]={& _tmp66C};Cyc_Tcutil_terr(loc,_tag_arr("bad function type attribute %s",
sizeof(char),31),_tag_arr(_tmp66B,sizeof(void*),1));}});{void*_tmp66D=(void*)
_tmp5EE->hd;void*_tmp66E;int _tmp66F;int _tmp670;_LL494: if((int)_tmp66D != 0)goto
_LL496;_LL495: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL493;_LL496:
if((int)_tmp66D != 1)goto _LL498;_LL497: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}
goto _LL493;_LL498: if((int)_tmp66D != 2)goto _LL49A;_LL499: if(!seen_fastcall){
seen_fastcall=1;++ num_convs;}goto _LL493;_LL49A: if(_tmp66D <= (void*)16?1:*((int*)
_tmp66D)!= 3)goto _LL49C;_tmp66E=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp66D)->f1;_tmp66F=((struct Cyc_Absyn_Format_att_struct*)_tmp66D)->f2;_tmp670=((
struct Cyc_Absyn_Format_att_struct*)_tmp66D)->f3;_LL49B: if(!seen_format){
seen_format=1;ft=_tmp66E;fmt_desc_arg=_tmp66F;fmt_arg_start=_tmp670;}else{({void*
_tmp671[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function can't have multiple format attributes",
sizeof(char),47),_tag_arr(_tmp671,sizeof(void*),0));});}goto _LL493;_LL49C:;
_LL49D: goto _LL493;_LL493:;}}if(num_convs > 1)({void*_tmp672[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("function can't have multiple calling conventions",sizeof(char),49),
_tag_arr(_tmp672,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp5E6);{
struct Cyc_List_List*b=*_tmp5E6;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp673=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp674;_LL49F: if(*((int*)_tmp673)!= 0)goto
_LL4A1;_tmp674=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp673)->f1;if((int)
_tmp674 != 1)goto _LL4A1;_LL4A0:({struct Cyc_Std_String_pa_struct _tmp676;_tmp676.tag=
0;_tmp676.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name;{void*
_tmp675[1]={& _tmp676};Cyc_Tcutil_terr(loc,_tag_arr("function attempts to abstract Mem type variable %s",
sizeof(char),51),_tag_arr(_tmp675,sizeof(void*),1));}});goto _LL49E;_LL4A1:;
_LL4A2: goto _LL49E;_LL49E:;}}}{struct Cyc_Tcutil_CVTEnv _tmp677=({struct Cyc_Tcutil_CVTEnv
_tmp6E5;_tmp6E5.kind_env=cvtenv.kind_env;_tmp6E5.free_vars=0;_tmp6E5.free_evars=
0;_tmp6E5.generalize_evars=cvtenv.generalize_evars;_tmp6E5.fn_result=1;_tmp6E5;});
_tmp677=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp677,(void*)1,_tmp5E9);_tmp677.fn_result=
0;{struct Cyc_List_List*a=_tmp5EA;for(0;a != 0;a=a->tl){_tmp677=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp677,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp5EC != 0){if(
_tmp5EB)({void*_tmp678[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("both c_vararg and cyc_vararg",sizeof(char),29),
_tag_arr(_tmp678,sizeof(void*),0));});{void*_tmp67A;int _tmp67B;struct Cyc_Absyn_VarargInfo
_tmp679=*_tmp5EC;_tmp67A=(void*)_tmp679.type;_tmp67B=_tmp679.inject;_tmp677=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp677,(void*)1,_tmp67A);if(_tmp67B){void*_tmp67C=Cyc_Tcutil_compress(
_tmp67A);struct Cyc_Absyn_TunionInfo _tmp67D;void*_tmp67E;_LL4A4: if(_tmp67C <= (
void*)3?1:*((int*)_tmp67C)!= 2)goto _LL4A6;_tmp67D=((struct Cyc_Absyn_TunionType_struct*)
_tmp67C)->f1;_tmp67E=(void*)_tmp67D.tunion_info;if(*((int*)_tmp67E)!= 1)goto
_LL4A6;_LL4A5: goto _LL4A3;_LL4A6:;_LL4A7:({void*_tmp67F[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("can't inject a non-[x]tunion type",sizeof(char),34),_tag_arr(_tmp67F,
sizeof(void*),0));});goto _LL4A3;_LL4A3:;}}}if(seen_format){int _tmp680=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp5EA);if((((fmt_desc_arg < 0?1:
fmt_desc_arg > _tmp680)?1: fmt_arg_start < 0)?1:(_tmp5EC == 0?fmt_arg_start != 0: 0))?
1:(_tmp5EC != 0?fmt_arg_start != _tmp680 + 1: 0))({void*_tmp681[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bad format descriptor",sizeof(char),22),_tag_arr(_tmp681,sizeof(
void*),0));});else{void*_tmp683;struct _tuple2 _tmp682=*((struct _tuple2*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5EA,fmt_desc_arg - 1);_tmp683=_tmp682.f3;{
void*_tmp684=Cyc_Tcutil_compress(_tmp683);struct Cyc_Absyn_PtrInfo _tmp685;void*
_tmp686;struct Cyc_Absyn_Conref*_tmp687;_LL4A9: if(_tmp684 <= (void*)3?1:*((int*)
_tmp684)!= 4)goto _LL4AB;_tmp685=((struct Cyc_Absyn_PointerType_struct*)_tmp684)->f1;
_tmp686=(void*)_tmp685.elt_typ;_tmp687=_tmp685.bounds;_LL4AA:{struct _tuple6
_tmp689=({struct _tuple6 _tmp688;_tmp688.f1=Cyc_Tcutil_compress(_tmp686);_tmp688.f2=
Cyc_Absyn_conref_def((void*)0,_tmp687);_tmp688;});void*_tmp68A;void*_tmp68B;void*
_tmp68C;void*_tmp68D;_LL4AE: _tmp68A=_tmp689.f1;if(_tmp68A <= (void*)3?1:*((int*)
_tmp68A)!= 5)goto _LL4B0;_tmp68B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp68A)->f1;
if((int)_tmp68B != 1)goto _LL4B0;_tmp68C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp68A)->f2;if((int)_tmp68C != 0)goto _LL4B0;_tmp68D=_tmp689.f2;if((int)_tmp68D != 
0)goto _LL4B0;_LL4AF: goto _LL4AD;_LL4B0:;_LL4B1:({void*_tmp68E[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("format descriptor is not a char ? type",sizeof(char),39),_tag_arr(
_tmp68E,sizeof(void*),0));});goto _LL4AD;_LL4AD:;}goto _LL4A8;_LL4AB:;_LL4AC:({
void*_tmp68F[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format descriptor is not a char ? type",
sizeof(char),39),_tag_arr(_tmp68F,sizeof(void*),0));});goto _LL4A8;_LL4A8:;}if(
fmt_arg_start != 0){int problem;{struct _tuple6 _tmp691=({struct _tuple6 _tmp690;
_tmp690.f1=ft;_tmp690.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp5EC))->type);_tmp690;});void*_tmp692;void*_tmp693;struct Cyc_Absyn_TunionInfo
_tmp694;void*_tmp695;struct Cyc_Absyn_Tuniondecl**_tmp696;struct Cyc_Absyn_Tuniondecl*
_tmp697;void*_tmp698;void*_tmp699;struct Cyc_Absyn_TunionInfo _tmp69A;void*_tmp69B;
struct Cyc_Absyn_Tuniondecl**_tmp69C;struct Cyc_Absyn_Tuniondecl*_tmp69D;_LL4B3:
_tmp692=_tmp691.f1;if((int)_tmp692 != 0)goto _LL4B5;_tmp693=_tmp691.f2;if(_tmp693
<= (void*)3?1:*((int*)_tmp693)!= 2)goto _LL4B5;_tmp694=((struct Cyc_Absyn_TunionType_struct*)
_tmp693)->f1;_tmp695=(void*)_tmp694.tunion_info;if(*((int*)_tmp695)!= 1)goto
_LL4B5;_tmp696=((struct Cyc_Absyn_KnownTunion_struct*)_tmp695)->f1;_tmp697=*
_tmp696;_LL4B4: problem=Cyc_Absyn_qvar_cmp(_tmp697->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL4B2;_LL4B5: _tmp698=_tmp691.f1;if((int)_tmp698 != 1)goto _LL4B7;_tmp699=
_tmp691.f2;if(_tmp699 <= (void*)3?1:*((int*)_tmp699)!= 2)goto _LL4B7;_tmp69A=((
struct Cyc_Absyn_TunionType_struct*)_tmp699)->f1;_tmp69B=(void*)_tmp69A.tunion_info;
if(*((int*)_tmp69B)!= 1)goto _LL4B7;_tmp69C=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp69B)->f1;_tmp69D=*_tmp69C;_LL4B6: problem=Cyc_Absyn_qvar_cmp(_tmp69D->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL4B2;_LL4B7:;_LL4B8: problem=1;goto
_LL4B2;_LL4B2:;}if(problem)({void*_tmp69E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format attribute and vararg types don't match",
sizeof(char),46),_tag_arr(_tmp69E,sizeof(void*),0));});}}}{struct Cyc_List_List*
rpo=_tmp5ED;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp6A0;void*_tmp6A1;void*
_tmp6A2;struct _tuple6*_tmp69F=(struct _tuple6*)rpo->hd;_tmp6A0=*_tmp69F;_tmp6A1=
_tmp6A0.f1;_tmp6A2=_tmp6A0.f2;_tmp677=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp677,(void*)4,_tmp6A1);_tmp677=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp677,(
void*)3,_tmp6A2);}}if(*_tmp5E8 != 0)_tmp677=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp677,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp5E8))->v);else{
struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp677.free_vars;for(0;
tvs != 0;tvs=tvs->tl){void*_tmp6A3=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmp6A4;struct Cyc_Core_Opt**_tmp6A5;void*
_tmp6A6;void*_tmp6A7;struct Cyc_Core_Opt*_tmp6A8;struct Cyc_Core_Opt**_tmp6A9;void*
_tmp6AA;void*_tmp6AB;struct Cyc_Core_Opt*_tmp6AC;struct Cyc_Core_Opt**_tmp6AD;
_LL4BA: if(*((int*)_tmp6A3)!= 2)goto _LL4BC;_tmp6A4=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6A3)->f1;_tmp6A5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6A3)->f1;_tmp6A6=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6A3)->f2;if((
int)_tmp6A6 != 3)goto _LL4BC;_LL4BB:*_tmp6A5=({struct Cyc_Core_Opt*_tmp6AE=
_cycalloc(sizeof(*_tmp6AE));_tmp6AE->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp6B0;_tmp6B0.tag=0;_tmp6B0.f1=(void*)((void*)3);_tmp6B0;});_tmp6AF;}));
_tmp6AE;});goto _LL4BD;_LL4BC: if(*((int*)_tmp6A3)!= 0)goto _LL4BE;_tmp6A7=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp6A3)->f1;if((int)_tmp6A7 != 3)goto _LL4BE;_LL4BD:
effect=({struct Cyc_List_List*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6B2=_cycalloc(sizeof(*
_tmp6B2));_tmp6B2[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6B3;_tmp6B3.tag=19;
_tmp6B3.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6B4=_cycalloc(
sizeof(*_tmp6B4));_tmp6B4[0]=({struct Cyc_Absyn_VarType_struct _tmp6B5;_tmp6B5.tag=
1;_tmp6B5.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6B5;});_tmp6B4;}));_tmp6B3;});
_tmp6B2;}));_tmp6B1->tl=effect;_tmp6B1;});goto _LL4B9;_LL4BE: if(*((int*)_tmp6A3)
!= 2)goto _LL4C0;_tmp6A8=((struct Cyc_Absyn_Less_kb_struct*)_tmp6A3)->f1;_tmp6A9=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6A3)->f1;_tmp6AA=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6A3)->f2;if((int)_tmp6AA != 4)goto
_LL4C0;_LL4BF:*_tmp6A9=({struct Cyc_Core_Opt*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));
_tmp6B6->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6B7=_cycalloc(
sizeof(*_tmp6B7));_tmp6B7[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6B8;_tmp6B8.tag=0;
_tmp6B8.f1=(void*)((void*)4);_tmp6B8;});_tmp6B7;}));_tmp6B6;});goto _LL4C1;_LL4C0:
if(*((int*)_tmp6A3)!= 0)goto _LL4C2;_tmp6AB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp6A3)->f1;if((int)_tmp6AB != 4)goto _LL4C2;_LL4C1: effect=({struct Cyc_List_List*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA[0]=({struct Cyc_Absyn_VarType_struct
_tmp6BB;_tmp6BB.tag=1;_tmp6BB.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6BB;});
_tmp6BA;}));_tmp6B9->tl=effect;_tmp6B9;});goto _LL4B9;_LL4C2: if(*((int*)_tmp6A3)
!= 1)goto _LL4C4;_tmp6AC=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6A3)->f1;
_tmp6AD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6A3)->f1;
_LL4C3:*_tmp6AD=({struct Cyc_Core_Opt*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));
_tmp6BD[0]=({struct Cyc_Absyn_Less_kb_struct _tmp6BE;_tmp6BE.tag=2;_tmp6BE.f1=0;
_tmp6BE.f2=(void*)((void*)0);_tmp6BE;});_tmp6BD;}));_tmp6BC;});goto _LL4C5;_LL4C4:;
_LL4C5: effect=({struct Cyc_List_List*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));
_tmp6C0[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6C1;_tmp6C1.tag=21;_tmp6C1.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));
_tmp6C2[0]=({struct Cyc_Absyn_VarType_struct _tmp6C3;_tmp6C3.tag=1;_tmp6C3.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6C3;});_tmp6C2;}));_tmp6C1;});_tmp6C0;}));
_tmp6BF->tl=effect;_tmp6BF;});goto _LL4B9;_LL4B9:;}}{struct Cyc_List_List*ts=
_tmp677.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp6C4=Cyc_Tcutil_typ_kind((
void*)ts->hd);_LL4C7: if((int)_tmp6C4 != 3)goto _LL4C9;_LL4C8: effect=({struct Cyc_List_List*
_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp6C7;_tmp6C7.tag=19;_tmp6C7.f1=(void*)((void*)ts->hd);_tmp6C7;});_tmp6C6;}));
_tmp6C5->tl=effect;_tmp6C5;});goto _LL4C6;_LL4C9: if((int)_tmp6C4 != 4)goto _LL4CB;
_LL4CA: effect=({struct Cyc_List_List*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8->hd=(
void*)((void*)ts->hd);_tmp6C8->tl=effect;_tmp6C8;});goto _LL4C6;_LL4CB:;_LL4CC:
effect=({struct Cyc_List_List*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));
_tmp6CA[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6CB;_tmp6CB.tag=21;_tmp6CB.f1=(
void*)((void*)ts->hd);_tmp6CB;});_tmp6CA;}));_tmp6C9->tl=effect;_tmp6C9;});goto
_LL4C6;_LL4C6:;}}*_tmp5E8=({struct Cyc_Core_Opt*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));
_tmp6CC->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6CD=_cycalloc(
sizeof(*_tmp6CD));_tmp6CD[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6CE;_tmp6CE.tag=
20;_tmp6CE.f1=effect;_tmp6CE;});_tmp6CD;}));_tmp6CC;});}if(*_tmp5E6 != 0){struct
Cyc_List_List*bs=*_tmp5E6;for(0;bs != 0;bs=bs->tl){void*_tmp6CF=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmp6D0;struct
Cyc_Core_Opt**_tmp6D1;struct Cyc_Core_Opt*_tmp6D2;struct Cyc_Core_Opt**_tmp6D3;
void*_tmp6D4;struct Cyc_Core_Opt*_tmp6D5;struct Cyc_Core_Opt**_tmp6D6;void*_tmp6D7;
struct Cyc_Core_Opt*_tmp6D8;struct Cyc_Core_Opt**_tmp6D9;void*_tmp6DA;void*_tmp6DB;
_LL4CE: if(*((int*)_tmp6CF)!= 1)goto _LL4D0;_tmp6D0=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6CF)->f1;_tmp6D1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6CF)->f1;_LL4CF:({struct Cyc_Std_String_pa_struct _tmp6DD;_tmp6DD.tag=0;
_tmp6DD.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name;{void*
_tmp6DC[1]={& _tmp6DD};Cyc_Tcutil_warn(loc,_tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof(char),47),_tag_arr(_tmp6DC,sizeof(void*),1));}});_tmp6D3=_tmp6D1;goto
_LL4D1;_LL4D0: if(*((int*)_tmp6CF)!= 2)goto _LL4D2;_tmp6D2=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6CF)->f1;_tmp6D3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6CF)->f1;_tmp6D4=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6CF)->f2;if((
int)_tmp6D4 != 1)goto _LL4D2;_LL4D1: _tmp6D6=_tmp6D3;goto _LL4D3;_LL4D2: if(*((int*)
_tmp6CF)!= 2)goto _LL4D4;_tmp6D5=((struct Cyc_Absyn_Less_kb_struct*)_tmp6CF)->f1;
_tmp6D6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6CF)->f1;
_tmp6D7=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6CF)->f2;if((int)_tmp6D7 != 
0)goto _LL4D4;_LL4D3:*_tmp6D6=({struct Cyc_Core_Opt*_tmp6DE=_cycalloc(sizeof(*
_tmp6DE));_tmp6DE->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6DF=
_cycalloc(sizeof(*_tmp6DF));_tmp6DF[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6E0;
_tmp6E0.tag=0;_tmp6E0.f1=(void*)((void*)2);_tmp6E0;});_tmp6DF;}));_tmp6DE;});
goto _LL4CD;_LL4D4: if(*((int*)_tmp6CF)!= 2)goto _LL4D6;_tmp6D8=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6CF)->f1;_tmp6D9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6CF)->f1;_tmp6DA=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6CF)->f2;
_LL4D5:*_tmp6D9=({struct Cyc_Core_Opt*_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));
_tmp6E2[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6E3;_tmp6E3.tag=0;_tmp6E3.f1=(void*)
_tmp6DA;_tmp6E3;});_tmp6E2;}));_tmp6E1;});goto _LL4CD;_LL4D6: if(*((int*)_tmp6CF)
!= 0)goto _LL4D8;_tmp6DB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp6CF)->f1;if((
int)_tmp6DB != 1)goto _LL4D8;_LL4D7:({void*_tmp6E4[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("functions can't abstract M types",sizeof(char),33),_tag_arr(_tmp6E4,
sizeof(void*),0));});goto _LL4CD;_LL4D8:;_LL4D9: goto _LL4CD;_LL4CD:;}}cvtenv.kind_env=
Cyc_Tcutil_remove_bound_tvars(_tmp677.kind_env,*_tmp5E6);_tmp677.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp677.free_vars,*_tmp5E6);{struct Cyc_List_List*tvs=_tmp677.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp677.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL433;}}_LL454: if(_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 
9)goto _LL456;_tmp5EF=((struct Cyc_Absyn_TupleType_struct*)_tmp5C5)->f1;_LL455:
for(0;_tmp5EF != 0;_tmp5EF=_tmp5EF->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,(void*)1,(*((struct _tuple4*)_tmp5EF->hd)).f2);}goto _LL433;_LL456: if(
_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 11)goto _LL458;_tmp5F0=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5C5)->f1;_tmp5F1=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5C5)->f2;_LL457:{
struct _RegionHandle _tmp6E6=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp6E6;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp5F1 != 0;_tmp5F1=_tmp5F1->tl){struct Cyc_Absyn_Aggrfield _tmp6E8;struct
_tagged_arr*_tmp6E9;struct Cyc_Absyn_Tqual _tmp6EA;void*_tmp6EB;struct Cyc_Absyn_Exp*
_tmp6EC;struct Cyc_List_List*_tmp6ED;struct Cyc_Absyn_Aggrfield*_tmp6E7=(struct Cyc_Absyn_Aggrfield*)
_tmp5F1->hd;_tmp6E8=*_tmp6E7;_tmp6E9=_tmp6E8.name;_tmp6EA=_tmp6E8.tq;_tmp6EB=(
void*)_tmp6E8.type;_tmp6EC=_tmp6E8.width;_tmp6ED=_tmp6E8.attributes;if(((int(*)(
int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,
struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,prev_fields,_tmp6E9))({
struct Cyc_Std_String_pa_struct _tmp6EF;_tmp6EF.tag=0;_tmp6EF.f1=(struct
_tagged_arr)*_tmp6E9;{void*_tmp6EE[1]={& _tmp6EF};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s",
sizeof(char),19),_tag_arr(_tmp6EE,sizeof(void*),1));}});if(Cyc_Std_strcmp(*
_tmp6E9,_tag_arr("",sizeof(char),1))!= 0)prev_fields=({struct Cyc_List_List*
_tmp6F0=_region_malloc(aprev_rgn,sizeof(*_tmp6F0));_tmp6F0->hd=_tmp6E9;_tmp6F0->tl=
prev_fields;_tmp6F0;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp6EB);if(_tmp5F0 == (void*)1?!Cyc_Tcutil_bits_only(_tmp6EB): 0)({struct Cyc_Std_String_pa_struct
_tmp6F2;_tmp6F2.tag=0;_tmp6F2.f1=(struct _tagged_arr)*_tmp6E9;{void*_tmp6F1[1]={&
_tmp6F2};Cyc_Tcutil_terr(loc,_tag_arr("union member %s has a non-integral type",
sizeof(char),40),_tag_arr(_tmp6F1,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(
loc,te,_tmp6EB,_tmp6EC,_tmp6E9);Cyc_Tcutil_check_field_atts(loc,_tmp6E9,_tmp6ED);}};
_pop_region(aprev_rgn);}goto _LL433;_LL458: if(_tmp5C5 <= (void*)3?1:*((int*)
_tmp5C5)!= 10)goto _LL45A;_tmp5F2=((struct Cyc_Absyn_AggrType_struct*)_tmp5C5)->f1;
_tmp5F3=(void*)_tmp5F2.aggr_info;_tmp5F4=(void**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp5C5)->f1).aggr_info;_tmp5F5=_tmp5F2.targs;_tmp5F6=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp5C5)->f1).targs;_LL459:{void*_tmp6F3=*
_tmp5F4;void*_tmp6F4;struct _tuple1*_tmp6F5;struct Cyc_Absyn_Aggrdecl**_tmp6F6;
struct Cyc_Absyn_Aggrdecl*_tmp6F7;_LL4DB: if(*((int*)_tmp6F3)!= 0)goto _LL4DD;
_tmp6F4=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp6F3)->f1;_tmp6F5=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp6F3)->f2;_LL4DC: {struct Cyc_Absyn_Aggrdecl**
adp;{struct _handler_cons _tmp6F8;_push_handler(& _tmp6F8);{int _tmp6FA=0;if(setjmp(
_tmp6F8.handler))_tmp6FA=1;if(!_tmp6FA){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,
_tmp6F5);*_tmp5F4=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp6FB=_cycalloc(
sizeof(*_tmp6FB));_tmp6FB[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp6FC;_tmp6FC.tag=
1;_tmp6FC.f1=adp;_tmp6FC;});_tmp6FB;});;_pop_handler();}else{void*_tmp6F9=(void*)
_exn_thrown;void*_tmp6FE=_tmp6F9;_LL4E0: if(_tmp6FE != Cyc_Dict_Absent)goto _LL4E2;
_LL4E1: {struct Cyc_Tcenv_Genv*_tmp6FF=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*
_tmp700=({struct Cyc_Absyn_Aggrdecl*_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705->kind=(
void*)_tmp6F4;_tmp705->sc=(void*)((void*)3);_tmp705->name=_tmp6F5;_tmp705->tvs=0;
_tmp705->impl=0;_tmp705->attributes=0;_tmp705;});Cyc_Tc_tcAggrdecl(te,_tmp6FF,
loc,_tmp700);adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp6F5);*_tmp5F4=(void*)({
struct Cyc_Absyn_KnownAggr_struct*_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701[0]=({
struct Cyc_Absyn_KnownAggr_struct _tmp702;_tmp702.tag=1;_tmp702.f1=adp;_tmp702;});
_tmp701;});if(*_tmp5F6 != 0){({struct Cyc_Std_String_pa_struct _tmp704;_tmp704.tag=
0;_tmp704.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp6F5);{void*_tmp703[1]={&
_tmp704};Cyc_Tcutil_terr(loc,_tag_arr("declare parameterized type %s before using",
sizeof(char),43),_tag_arr(_tmp703,sizeof(void*),1));}});return cvtenv;}goto _LL4DF;}
_LL4E2:;_LL4E3:(void)_throw(_tmp6FE);_LL4DF:;}}}_tmp6F7=*adp;goto _LL4DE;}_LL4DD:
if(*((int*)_tmp6F3)!= 1)goto _LL4DA;_tmp6F6=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp6F3)->f1;_tmp6F7=*_tmp6F6;_LL4DE: {struct Cyc_List_List*tvs=_tmp6F7->tvs;
struct Cyc_List_List*ts=*_tmp5F6;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_Std_String_pa_struct
_tmp707;_tmp707.tag=0;_tmp707.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6F7->name);{void*_tmp706[1]={& _tmp707};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for type %s",
sizeof(char),32),_tag_arr(_tmp706,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp708=
_cycalloc(sizeof(*_tmp708));_tmp708->hd=(void*)e;_tmp708->tl=hidden_ts;_tmp708;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*_tmp5F6=Cyc_List_imp_append(*
_tmp5F6,Cyc_List_imp_rev(hidden_ts));}}_LL4DA:;}goto _LL433;_LL45A: if(_tmp5C5 <= (
void*)3?1:*((int*)_tmp5C5)!= 16)goto _LL45C;_tmp5F7=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5C5)->f1;_tmp5F8=((struct Cyc_Absyn_TypedefType_struct*)_tmp5C5)->f2;_tmp5F9=(
struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp5C5)->f2;
_tmp5FA=((struct Cyc_Absyn_TypedefType_struct*)_tmp5C5)->f3;_tmp5FB=(struct Cyc_Absyn_Typedefdecl**)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp5C5)->f3;_tmp5FC=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5C5)->f4;_tmp5FD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp5C5)->f4;_LL45B: {struct Cyc_List_List*targs=*_tmp5F9;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmp709;_push_handler(& _tmp709);{int _tmp70B=0;if(setjmp(
_tmp709.handler))_tmp70B=1;if(!_tmp70B){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmp5F7);;_pop_handler();}else{void*_tmp70A=(void*)_exn_thrown;void*_tmp70D=
_tmp70A;_LL4E5: if(_tmp70D != Cyc_Dict_Absent)goto _LL4E7;_LL4E6:({struct Cyc_Std_String_pa_struct
_tmp70F;_tmp70F.tag=0;_tmp70F.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp5F7);{void*_tmp70E[1]={& _tmp70F};Cyc_Tcutil_terr(loc,_tag_arr("unbound typedef name %s",
sizeof(char),24),_tag_arr(_tmp70E,sizeof(void*),1));}});return cvtenv;_LL4E7:;
_LL4E8:(void)_throw(_tmp70D);_LL4E4:;}}}*_tmp5FB=(struct Cyc_Absyn_Typedefdecl*)
td;_tmp5F7[0]=(td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0?
tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710->hd=({
struct _tuple7*_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp711->f2=(void*)ts->hd;_tmp711;});_tmp710->tl=inst;_tmp710;});}if(ts
!= 0)({struct Cyc_Std_String_pa_struct _tmp713;_tmp713.tag=0;_tmp713.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp5F7);{void*_tmp712[1]={& _tmp713};Cyc_Tcutil_terr(
loc,_tag_arr("too many parameters for typedef %s",sizeof(char),35),_tag_arr(
_tmp712,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp714=_cycalloc(
sizeof(*_tmp714));_tmp714->hd=(void*)e;_tmp714->tl=hidden_ts;_tmp714;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp715=_cycalloc(sizeof(*_tmp715));_tmp715->hd=({struct _tuple7*_tmp716=
_cycalloc(sizeof(*_tmp716));_tmp716->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp716->f2=
e;_tmp716;});_tmp715->tl=inst;_tmp715;});}*_tmp5F9=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp5FD=({struct Cyc_Core_Opt*
_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717->v=(void*)new_typ;_tmp717;});}goto
_LL433;}}_LL45C: if((int)_tmp5C5 != 2)goto _LL45E;_LL45D: goto _LL433;_LL45E: if(
_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 15)goto _LL460;_tmp5FE=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5C5)->f1;_LL45F: _tmp5FF=_tmp5FE;goto _LL461;_LL460: if(_tmp5C5 <= (void*)3?1:*((
int*)_tmp5C5)!= 19)goto _LL462;_tmp5FF=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp5C5)->f1;_LL461: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp5FF);goto _LL433;_LL462: if(_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 21)goto
_LL464;_tmp600=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp5C5)->f1;_LL463:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp600);goto _LL433;
_LL464: if(_tmp5C5 <= (void*)3?1:*((int*)_tmp5C5)!= 20)goto _LL433;_tmp601=((struct
Cyc_Absyn_JoinEff_struct*)_tmp5C5)->f1;_LL465: for(0;_tmp601 != 0;_tmp601=_tmp601->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)_tmp601->hd);}
goto _LL433;_LL433:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))({
struct Cyc_Std_String_pa_struct _tmp71B;_tmp71B.tag=0;_tmp71B.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(expected_kind);{struct Cyc_Std_String_pa_struct
_tmp71A;_tmp71A.tag=0;_tmp71A.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t));{struct Cyc_Std_String_pa_struct _tmp719;_tmp719.tag=0;_tmp719.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp718[3]={& _tmp719,& _tmp71A,&
_tmp71B};Cyc_Tcutil_terr(loc,_tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof(char),51),_tag_arr(_tmp718,sizeof(void*),3));}}}});return cvtenv;}static
struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp71C=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp725;_tmp725.kind_env=kind_env;_tmp725.free_vars=
0;_tmp725.free_evars=0;_tmp725.generalize_evars=generalize_evars;_tmp725.fn_result=
0;_tmp725;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp71C.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp71C.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp71C.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp71D=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp71E;struct Cyc_Core_Opt**_tmp71F;_LL4EA: if(_tmp71D <= (void*)3?1:*((int*)
_tmp71D)!= 0)goto _LL4EC;_tmp71E=((struct Cyc_Absyn_Evar_struct*)_tmp71D)->f4;
_tmp71F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp71D)->f4;
_LL4EB: if(*_tmp71F == 0)*_tmp71F=({struct Cyc_Core_Opt*_tmp720=_cycalloc(sizeof(*
_tmp720));_tmp720->v=kind_env;_tmp720;});else{struct Cyc_List_List*_tmp721=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp71F))->v;struct Cyc_List_List*
_tmp722=0;for(0;_tmp721 != 0;_tmp721=_tmp721->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp721->hd))_tmp722=({struct Cyc_List_List*_tmp723=_cycalloc(sizeof(*_tmp723));
_tmp723->hd=(struct Cyc_Absyn_Tvar*)_tmp721->hd;_tmp723->tl=_tmp722;_tmp723;});}*
_tmp71F=({struct Cyc_Core_Opt*_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724->v=
_tmp722;_tmp724;});}goto _LL4E9;_LL4EC:;_LL4ED: goto _LL4E9;_LL4E9:;}}return _tmp71C;}
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_Tcutil_CVTEnv _tmp726=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp727=_tmp726.free_vars;struct Cyc_List_List*
_tmp728=_tmp726.free_evars;{struct Cyc_List_List*x=_tmp727;for(0;x != 0;x=x->tl){
void*_tmp729=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp72A;struct Cyc_Core_Opt**_tmp72B;struct Cyc_Core_Opt*
_tmp72C;struct Cyc_Core_Opt**_tmp72D;void*_tmp72E;struct Cyc_Core_Opt*_tmp72F;
struct Cyc_Core_Opt**_tmp730;void*_tmp731;struct Cyc_Core_Opt*_tmp732;struct Cyc_Core_Opt**
_tmp733;void*_tmp734;void*_tmp735;_LL4EF: if(*((int*)_tmp729)!= 1)goto _LL4F1;
_tmp72A=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp729)->f1;_tmp72B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp729)->f1;_LL4F0: _tmp72D=_tmp72B;goto
_LL4F2;_LL4F1: if(*((int*)_tmp729)!= 2)goto _LL4F3;_tmp72C=((struct Cyc_Absyn_Less_kb_struct*)
_tmp729)->f1;_tmp72D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp729)->f1;_tmp72E=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp729)->f2;if((
int)_tmp72E != 1)goto _LL4F3;_LL4F2: _tmp730=_tmp72D;goto _LL4F4;_LL4F3: if(*((int*)
_tmp729)!= 2)goto _LL4F5;_tmp72F=((struct Cyc_Absyn_Less_kb_struct*)_tmp729)->f1;
_tmp730=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp729)->f1;
_tmp731=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp729)->f2;if((int)_tmp731 != 
0)goto _LL4F5;_LL4F4:*_tmp730=({struct Cyc_Core_Opt*_tmp736=_cycalloc(sizeof(*
_tmp736));_tmp736->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp737=
_cycalloc(sizeof(*_tmp737));_tmp737[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp738;
_tmp738.tag=0;_tmp738.f1=(void*)((void*)2);_tmp738;});_tmp737;}));_tmp736;});
goto _LL4EE;_LL4F5: if(*((int*)_tmp729)!= 2)goto _LL4F7;_tmp732=((struct Cyc_Absyn_Less_kb_struct*)
_tmp729)->f1;_tmp733=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp729)->f1;_tmp734=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp729)->f2;
_LL4F6:*_tmp733=({struct Cyc_Core_Opt*_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp73A=_cycalloc(sizeof(*_tmp73A));
_tmp73A[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp73B;_tmp73B.tag=0;_tmp73B.f1=(void*)
_tmp734;_tmp73B;});_tmp73A;}));_tmp739;});goto _LL4EE;_LL4F7: if(*((int*)_tmp729)
!= 0)goto _LL4F9;_tmp735=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp729)->f1;if((
int)_tmp735 != 1)goto _LL4F9;_LL4F8:({struct Cyc_Std_String_pa_struct _tmp73D;
_tmp73D.tag=0;_tmp73D.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd);{void*_tmp73C[1]={& _tmp73D};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s cannot have kind M",
sizeof(char),36),_tag_arr(_tmp73C,sizeof(void*),1));}});goto _LL4EE;_LL4F9:;
_LL4FA: goto _LL4EE;_LL4EE:;}}if(_tmp727 != 0?1: _tmp728 != 0){{void*_tmp73E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp73F;struct Cyc_List_List*_tmp740;struct Cyc_List_List**
_tmp741;struct Cyc_Core_Opt*_tmp742;void*_tmp743;struct Cyc_List_List*_tmp744;int
_tmp745;struct Cyc_Absyn_VarargInfo*_tmp746;struct Cyc_List_List*_tmp747;struct Cyc_List_List*
_tmp748;_LL4FC: if(_tmp73E <= (void*)3?1:*((int*)_tmp73E)!= 8)goto _LL4FE;_tmp73F=((
struct Cyc_Absyn_FnType_struct*)_tmp73E)->f1;_tmp740=_tmp73F.tvars;_tmp741=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp73E)->f1).tvars;
_tmp742=_tmp73F.effect;_tmp743=(void*)_tmp73F.ret_typ;_tmp744=_tmp73F.args;
_tmp745=_tmp73F.c_varargs;_tmp746=_tmp73F.cyc_varargs;_tmp747=_tmp73F.rgn_po;
_tmp748=_tmp73F.attributes;_LL4FD: if(*_tmp741 == 0){*_tmp741=_tmp727;_tmp727=0;}
goto _LL4FB;_LL4FE:;_LL4FF: goto _LL4FB;_LL4FB:;}if(_tmp727 != 0)({struct Cyc_Std_String_pa_struct
_tmp74A;_tmp74A.tag=0;_tmp74A.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)
_tmp727->hd)->name;{void*_tmp749[1]={& _tmp74A};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s",
sizeof(char),25),_tag_arr(_tmp749,sizeof(void*),1));}});if(_tmp728 != 0)for(0;
_tmp728 != 0;_tmp728=_tmp728->tl){void*e=(void*)_tmp728->hd;void*_tmp74B=Cyc_Tcutil_typ_kind(
e);_LL501: if((int)_tmp74B != 3)goto _LL503;_LL502: if(!Cyc_Tcutil_unify(e,(void*)2))({
void*_tmp74C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with heap!",sizeof(char),28),_tag_arr(_tmp74C,sizeof(
void*),0));});goto _LL500;_LL503: if((int)_tmp74B != 4)goto _LL505;_LL504: if(!Cyc_Tcutil_unify(
e,Cyc_Absyn_empty_effect))({void*_tmp74D[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(char),26),_tag_arr(_tmp74D,sizeof(void*),0));});goto _LL500;_LL505:;_LL506:({
struct Cyc_Std_String_pa_struct _tmp750;_tmp750.tag=0;_tmp750.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp74F;
_tmp74F.tag=0;_tmp74F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp74E[2]={& _tmp74F,& _tmp750};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp74E,sizeof(void*),2));}}});goto _LL500;_LL500:;}}}
void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(
loc,te,t);{void*_tmp751=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp752;
struct Cyc_List_List*_tmp753;struct Cyc_Core_Opt*_tmp754;_LL508: if(_tmp751 <= (void*)
3?1:*((int*)_tmp751)!= 8)goto _LL50A;_tmp752=((struct Cyc_Absyn_FnType_struct*)
_tmp751)->f1;_tmp753=_tmp752.tvars;_tmp754=_tmp752.effect;_LL509: fd->tvs=_tmp753;
fd->effect=_tmp754;goto _LL507;_LL50A:;_LL50B:({void*_tmp755[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_fndecl_valid_type: not a FnType",
sizeof(char),38),_tag_arr(_tmp755,sizeof(void*),0));});_LL507:;}{struct
_RegionHandle _tmp756=_new_region("r");struct _RegionHandle*r=& _tmp756;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct _tuple12*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct _tuple12*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,_tag_arr("function declaration has repeated parameter",sizeof(char),
44));;_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp757=_cycalloc(
sizeof(*_tmp757));_tmp757->v=(void*)t;_tmp757;});}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp758=Cyc_Tcutil_check_valid_type(loc,te,bound_tvars,expected_kind,0,t);struct
Cyc_List_List*_tmp759=Cyc_Tcutil_remove_bound_tvars(_tmp758.free_vars,
bound_tvars);struct Cyc_List_List*_tmp75A=_tmp758.free_evars;{struct Cyc_List_List*
fs=_tmp759;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp75B=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;({struct Cyc_Std_String_pa_struct _tmp75E;_tmp75E.tag=0;_tmp75E.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp75D;_tmp75D.tag=0;_tmp75D.f1=(struct _tagged_arr)*_tmp75B;{void*_tmp75C[2]={&
_tmp75D,& _tmp75E};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s in type %s",
sizeof(char),36),_tag_arr(_tmp75C,sizeof(void*),2));}}});}}if(!allow_evars?
_tmp75A != 0: 0)for(0;_tmp75A != 0;_tmp75A=_tmp75A->tl){void*e=(void*)_tmp75A->hd;
void*_tmp75F=Cyc_Tcutil_typ_kind(e);_LL50D: if((int)_tmp75F != 3)goto _LL50F;_LL50E:
if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp760[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(char),28),_tag_arr(_tmp760,sizeof(void*),0));});goto _LL50C;_LL50F: if((int)
_tmp75F != 4)goto _LL511;_LL510: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp761[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!",sizeof(char),26),_tag_arr(_tmp761,sizeof(
void*),0));});goto _LL50C;_LL511:;_LL512:({struct Cyc_Std_String_pa_struct _tmp764;
_tmp764.tag=0;_tmp764.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp763;_tmp763.tag=0;_tmp763.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{
void*_tmp762[2]={& _tmp763,& _tmp764};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp762,sizeof(void*),2));}}});goto _LL50C;_LL50C:;}}
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=
Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*
tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _tagged_arr(*a2string)(void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0)({struct Cyc_Std_String_pa_struct _tmp767;_tmp767.tag=0;_tmp767.f1=(
struct _tagged_arr)a2string((void*)vs->hd);{struct Cyc_Std_String_pa_struct _tmp766;
_tmp766.tag=0;_tmp766.f1=(struct _tagged_arr)msg;{void*_tmp765[2]={& _tmp766,&
_tmp767};Cyc_Tcutil_terr(loc,_tag_arr("%s: %s",sizeof(char),7),_tag_arr(_tmp765,
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
_tag_arr("duplicate type variable",sizeof(char),24));}struct _tuple17{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple18{struct Cyc_List_List*f1;void*f2;};struct _tuple19{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_Std_strcmp(*((
struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,_tag_arr("",sizeof(char),1))!= 0)
fields=({struct Cyc_List_List*_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768->hd=({
struct _tuple17*_tmp769=_cycalloc(sizeof(*_tmp769));_tmp769->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp769->f2=0;_tmp769;});_tmp768->tl=fields;_tmp768;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple18 _tmp76B;struct Cyc_List_List*
_tmp76C;void*_tmp76D;struct _tuple18*_tmp76A=(struct _tuple18*)des->hd;_tmp76B=*
_tmp76A;_tmp76C=_tmp76B.f1;_tmp76D=_tmp76B.f2;if(_tmp76C == 0){struct Cyc_List_List*
_tmp76E=fields;for(0;_tmp76E != 0;_tmp76E=_tmp76E->tl){if(!(*((struct _tuple17*)
_tmp76E->hd)).f2){(*((struct _tuple17*)_tmp76E->hd)).f2=1;(*((struct _tuple18*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp76F=_cycalloc(sizeof(*_tmp76F));
_tmp76F->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp770=_cycalloc(
sizeof(*_tmp770));_tmp770[0]=({struct Cyc_Absyn_FieldName_struct _tmp771;_tmp771.tag=
1;_tmp771.f1=((*((struct _tuple17*)_tmp76E->hd)).f1)->name;_tmp771;});_tmp770;}));
_tmp76F->tl=0;_tmp76F;});ans=({struct Cyc_List_List*_tmp772=_region_malloc(rgn,
sizeof(*_tmp772));_tmp772->hd=({struct _tuple19*_tmp773=_region_malloc(rgn,
sizeof(*_tmp773));_tmp773->f1=(*((struct _tuple17*)_tmp76E->hd)).f1;_tmp773->f2=
_tmp76D;_tmp773;});_tmp772->tl=ans;_tmp772;});break;}}if(_tmp76E == 0)({void*
_tmp774[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too many arguments to struct",
sizeof(char),29),_tag_arr(_tmp774,sizeof(void*),0));});}else{if(_tmp76C->tl != 0)({
void*_tmp775[0]={};Cyc_Tcutil_terr(loc,_tag_arr("multiple designators are not supported",
sizeof(char),39),_tag_arr(_tmp775,sizeof(void*),0));});else{void*_tmp776=(void*)
_tmp76C->hd;struct _tagged_arr*_tmp777;_LL514: if(*((int*)_tmp776)!= 0)goto _LL516;
_LL515:({void*_tmp778[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array designator used in argument to struct",
sizeof(char),44),_tag_arr(_tmp778,sizeof(void*),0));});goto _LL513;_LL516: if(*((
int*)_tmp776)!= 1)goto _LL513;_tmp777=((struct Cyc_Absyn_FieldName_struct*)_tmp776)->f1;
_LL517: {struct Cyc_List_List*_tmp779=fields;for(0;_tmp779 != 0;_tmp779=_tmp779->tl){
if(Cyc_Std_zstrptrcmp(_tmp777,((*((struct _tuple17*)_tmp779->hd)).f1)->name)== 0){
if((*((struct _tuple17*)_tmp779->hd)).f2)({struct Cyc_Std_String_pa_struct _tmp77B;
_tmp77B.tag=0;_tmp77B.f1=(struct _tagged_arr)*_tmp777;{void*_tmp77A[1]={& _tmp77B};
Cyc_Tcutil_terr(loc,_tag_arr("field %s has already been used as an argument",
sizeof(char),46),_tag_arr(_tmp77A,sizeof(void*),1));}});(*((struct _tuple17*)
_tmp779->hd)).f2=1;ans=({struct Cyc_List_List*_tmp77C=_region_malloc(rgn,sizeof(*
_tmp77C));_tmp77C->hd=({struct _tuple19*_tmp77D=_region_malloc(rgn,sizeof(*
_tmp77D));_tmp77D->f1=(*((struct _tuple17*)_tmp779->hd)).f1;_tmp77D->f2=_tmp76D;
_tmp77D;});_tmp77C->tl=ans;_tmp77C;});break;}}if(_tmp779 == 0)({struct Cyc_Std_String_pa_struct
_tmp77F;_tmp77F.tag=0;_tmp77F.f1=(struct _tagged_arr)*_tmp777;{void*_tmp77E[1]={&
_tmp77F};Cyc_Tcutil_terr(loc,_tag_arr("bad field designator %s",sizeof(char),24),
_tag_arr(_tmp77E,sizeof(void*),1));}});goto _LL513;}_LL513:;}}}for(0;fields != 0;
fields=fields->tl){if(!(*((struct _tuple17*)fields->hd)).f2){({void*_tmp780[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments to struct",sizeof(char),28),
_tag_arr(_tmp780,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp781=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp782;void*_tmp783;struct Cyc_Absyn_Conref*_tmp784;_LL519: if(_tmp781 <= (void*)3?
1:*((int*)_tmp781)!= 4)goto _LL51B;_tmp782=((struct Cyc_Absyn_PointerType_struct*)
_tmp781)->f1;_tmp783=(void*)_tmp782.elt_typ;_tmp784=_tmp782.bounds;_LL51A: {
struct Cyc_Absyn_Conref*_tmp785=Cyc_Absyn_compress_conref(_tmp784);void*_tmp786=(
void*)(Cyc_Absyn_compress_conref(_tmp785))->v;void*_tmp787;_LL51E: if(_tmp786 <= (
void*)1?1:*((int*)_tmp786)!= 0)goto _LL520;_tmp787=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp786)->f1;if((int)_tmp787 != 0)goto _LL520;_LL51F:*elt_typ_dest=_tmp783;return 1;
_LL520: if((int)_tmp786 != 0)goto _LL522;_LL521:(void*)(_tmp785->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp788=_cycalloc(sizeof(*_tmp788));_tmp788[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp789;_tmp789.tag=0;_tmp789.f1=(void*)((void*)
0);_tmp789;});_tmp788;})));*elt_typ_dest=_tmp783;return 1;_LL522:;_LL523: return 0;
_LL51D:;}_LL51B:;_LL51C: return 0;_LL518:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple9 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple9 bogus_ans=({struct _tuple9 _tmp7C7;_tmp7C7.f1=0;
_tmp7C7.f2=(void*)2;_tmp7C7;});void*_tmp78A=(void*)e->r;struct _tuple1*_tmp78B;
void*_tmp78C;struct Cyc_Absyn_Exp*_tmp78D;struct _tagged_arr*_tmp78E;struct Cyc_Absyn_Exp*
_tmp78F;struct _tagged_arr*_tmp790;struct Cyc_Absyn_Exp*_tmp791;struct Cyc_Absyn_Exp*
_tmp792;struct Cyc_Absyn_Exp*_tmp793;_LL525: if(*((int*)_tmp78A)!= 1)goto _LL527;
_tmp78B=((struct Cyc_Absyn_Var_e_struct*)_tmp78A)->f1;_tmp78C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp78A)->f2;_LL526: {void*_tmp794=_tmp78C;struct Cyc_Absyn_Vardecl*_tmp795;
struct Cyc_Absyn_Vardecl*_tmp796;struct Cyc_Absyn_Vardecl*_tmp797;struct Cyc_Absyn_Vardecl*
_tmp798;_LL532: if((int)_tmp794 != 0)goto _LL534;_LL533: goto _LL535;_LL534: if(
_tmp794 <= (void*)1?1:*((int*)_tmp794)!= 1)goto _LL536;_LL535: return bogus_ans;
_LL536: if(_tmp794 <= (void*)1?1:*((int*)_tmp794)!= 0)goto _LL538;_tmp795=((struct
Cyc_Absyn_Global_b_struct*)_tmp794)->f1;_LL537: {void*_tmp799=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL53F: if(_tmp799 <= (void*)
3?1:*((int*)_tmp799)!= 7)goto _LL541;_LL540: return({struct _tuple9 _tmp79A;_tmp79A.f1=
1;_tmp79A.f2=(void*)2;_tmp79A;});_LL541:;_LL542: return({struct _tuple9 _tmp79B;
_tmp79B.f1=(_tmp795->tq).q_const;_tmp79B.f2=(void*)2;_tmp79B;});_LL53E:;}_LL538:
if(_tmp794 <= (void*)1?1:*((int*)_tmp794)!= 3)goto _LL53A;_tmp796=((struct Cyc_Absyn_Local_b_struct*)
_tmp794)->f1;_LL539: {void*_tmp79C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL544: if(_tmp79C <= (void*)3?1:*((int*)_tmp79C)!= 7)
goto _LL546;_LL545: return({struct _tuple9 _tmp79D;_tmp79D.f1=1;_tmp79D.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp796->rgn))->v;_tmp79D;});_LL546:;_LL547:
_tmp796->escapes=1;return({struct _tuple9 _tmp79E;_tmp79E.f1=(_tmp796->tq).q_const;
_tmp79E.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp796->rgn))->v;_tmp79E;});
_LL543:;}_LL53A: if(_tmp794 <= (void*)1?1:*((int*)_tmp794)!= 4)goto _LL53C;_tmp797=((
struct Cyc_Absyn_Pat_b_struct*)_tmp794)->f1;_LL53B: _tmp798=_tmp797;goto _LL53D;
_LL53C: if(_tmp794 <= (void*)1?1:*((int*)_tmp794)!= 2)goto _LL531;_tmp798=((struct
Cyc_Absyn_Param_b_struct*)_tmp794)->f1;_LL53D: _tmp798->escapes=1;return({struct
_tuple9 _tmp79F;_tmp79F.f1=(_tmp798->tq).q_const;_tmp79F.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp798->rgn))->v;_tmp79F;});_LL531:;}_LL527: if(*((int*)_tmp78A)!= 
21)goto _LL529;_tmp78D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp78A)->f1;
_tmp78E=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp78A)->f2;_LL528: {void*
_tmp7A0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp78D->topt))->v);
struct Cyc_List_List*_tmp7A1;struct Cyc_Absyn_AggrInfo _tmp7A2;void*_tmp7A3;struct
Cyc_Absyn_Aggrdecl**_tmp7A4;struct Cyc_Absyn_Aggrdecl*_tmp7A5;_LL549: if(_tmp7A0 <= (
void*)3?1:*((int*)_tmp7A0)!= 11)goto _LL54B;_tmp7A1=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp7A0)->f2;_LL54A: {struct Cyc_Absyn_Aggrfield*_tmp7A6=Cyc_Absyn_lookup_field(
_tmp7A1,_tmp78E);if(_tmp7A6 != 0?_tmp7A6->width != 0: 0)return({struct _tuple9
_tmp7A7;_tmp7A7.f1=(_tmp7A6->tq).q_const;_tmp7A7.f2=(Cyc_Tcutil_addressof_props(
te,_tmp78D)).f2;_tmp7A7;});return bogus_ans;}_LL54B: if(_tmp7A0 <= (void*)3?1:*((
int*)_tmp7A0)!= 10)goto _LL54D;_tmp7A2=((struct Cyc_Absyn_AggrType_struct*)_tmp7A0)->f1;
_tmp7A3=(void*)_tmp7A2.aggr_info;if(*((int*)_tmp7A3)!= 1)goto _LL54D;_tmp7A4=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp7A3)->f1;_tmp7A5=*_tmp7A4;_LL54C: {struct
Cyc_Absyn_Aggrfield*_tmp7A8=Cyc_Absyn_lookup_decl_field(_tmp7A5,_tmp78E);if(
_tmp7A8 != 0?_tmp7A8->width != 0: 0)return({struct _tuple9 _tmp7A9;_tmp7A9.f1=(
_tmp7A8->tq).q_const;_tmp7A9.f2=(Cyc_Tcutil_addressof_props(te,_tmp78D)).f2;
_tmp7A9;});return bogus_ans;}_LL54D:;_LL54E: return bogus_ans;_LL548:;}_LL529: if(*((
int*)_tmp78A)!= 22)goto _LL52B;_tmp78F=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp78A)->f1;_tmp790=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp78A)->f2;_LL52A: {
void*_tmp7AA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp78F->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7AB;void*_tmp7AC;void*_tmp7AD;
_LL550: if(_tmp7AA <= (void*)3?1:*((int*)_tmp7AA)!= 4)goto _LL552;_tmp7AB=((struct
Cyc_Absyn_PointerType_struct*)_tmp7AA)->f1;_tmp7AC=(void*)_tmp7AB.elt_typ;
_tmp7AD=(void*)_tmp7AB.rgn_typ;_LL551: {struct Cyc_Absyn_Aggrfield*finfo;{void*
_tmp7AE=Cyc_Tcutil_compress(_tmp7AC);struct Cyc_List_List*_tmp7AF;struct Cyc_Absyn_AggrInfo
_tmp7B0;void*_tmp7B1;struct Cyc_Absyn_Aggrdecl**_tmp7B2;struct Cyc_Absyn_Aggrdecl*
_tmp7B3;_LL555: if(_tmp7AE <= (void*)3?1:*((int*)_tmp7AE)!= 11)goto _LL557;_tmp7AF=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp7AE)->f2;_LL556: finfo=Cyc_Absyn_lookup_field(
_tmp7AF,_tmp790);goto _LL554;_LL557: if(_tmp7AE <= (void*)3?1:*((int*)_tmp7AE)!= 10)
goto _LL559;_tmp7B0=((struct Cyc_Absyn_AggrType_struct*)_tmp7AE)->f1;_tmp7B1=(void*)
_tmp7B0.aggr_info;if(*((int*)_tmp7B1)!= 1)goto _LL559;_tmp7B2=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp7B1)->f1;_tmp7B3=*_tmp7B2;_LL558: finfo=Cyc_Absyn_lookup_decl_field(_tmp7B3,
_tmp790);goto _LL554;_LL559:;_LL55A: return bogus_ans;_LL554:;}if(finfo != 0?finfo->width
!= 0: 0)return({struct _tuple9 _tmp7B4;_tmp7B4.f1=(finfo->tq).q_const;_tmp7B4.f2=
_tmp7AD;_tmp7B4;});return bogus_ans;}_LL552:;_LL553: return bogus_ans;_LL54F:;}
_LL52B: if(*((int*)_tmp78A)!= 20)goto _LL52D;_tmp791=((struct Cyc_Absyn_Deref_e_struct*)
_tmp78A)->f1;_LL52C: {void*_tmp7B5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp791->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7B6;void*_tmp7B7;
struct Cyc_Absyn_Tqual _tmp7B8;_LL55C: if(_tmp7B5 <= (void*)3?1:*((int*)_tmp7B5)!= 4)
goto _LL55E;_tmp7B6=((struct Cyc_Absyn_PointerType_struct*)_tmp7B5)->f1;_tmp7B7=(
void*)_tmp7B6.rgn_typ;_tmp7B8=_tmp7B6.tq;_LL55D: return({struct _tuple9 _tmp7B9;
_tmp7B9.f1=_tmp7B8.q_const;_tmp7B9.f2=_tmp7B7;_tmp7B9;});_LL55E:;_LL55F: return
bogus_ans;_LL55B:;}_LL52D: if(*((int*)_tmp78A)!= 23)goto _LL52F;_tmp792=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp78A)->f1;_tmp793=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp78A)->f2;_LL52E: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp792->topt))->v);void*_tmp7BA=t;struct Cyc_List_List*_tmp7BB;
struct Cyc_Absyn_PtrInfo _tmp7BC;void*_tmp7BD;void*_tmp7BE;struct Cyc_Absyn_Tqual
_tmp7BF;struct Cyc_Absyn_Conref*_tmp7C0;struct Cyc_Absyn_Tqual _tmp7C1;_LL561: if(
_tmp7BA <= (void*)3?1:*((int*)_tmp7BA)!= 9)goto _LL563;_tmp7BB=((struct Cyc_Absyn_TupleType_struct*)
_tmp7BA)->f1;_LL562: {struct _tuple4*_tmp7C2=Cyc_Absyn_lookup_tuple_field(_tmp7BB,(
int)Cyc_Evexp_eval_const_uint_exp(_tmp793));if(_tmp7C2 != 0)return({struct _tuple9
_tmp7C3;_tmp7C3.f1=((*_tmp7C2).f1).q_const;_tmp7C3.f2=(Cyc_Tcutil_addressof_props(
te,_tmp792)).f2;_tmp7C3;});return bogus_ans;}_LL563: if(_tmp7BA <= (void*)3?1:*((
int*)_tmp7BA)!= 4)goto _LL565;_tmp7BC=((struct Cyc_Absyn_PointerType_struct*)
_tmp7BA)->f1;_tmp7BD=(void*)_tmp7BC.elt_typ;_tmp7BE=(void*)_tmp7BC.rgn_typ;
_tmp7BF=_tmp7BC.tq;_tmp7C0=_tmp7BC.bounds;_LL564: return({struct _tuple9 _tmp7C4;
_tmp7C4.f1=_tmp7BF.q_const;_tmp7C4.f2=_tmp7BE;_tmp7C4;});_LL565: if(_tmp7BA <= (
void*)3?1:*((int*)_tmp7BA)!= 7)goto _LL567;_tmp7C1=((struct Cyc_Absyn_ArrayType_struct*)
_tmp7BA)->f2;_LL566: return({struct _tuple9 _tmp7C5;_tmp7C5.f1=_tmp7C1.q_const;
_tmp7C5.f2=(Cyc_Tcutil_addressof_props(te,_tmp792)).f2;_tmp7C5;});_LL567:;_LL568:
return bogus_ans;_LL560:;}_LL52F:;_LL530:({void*_tmp7C6[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("unary & applied to non-lvalue",sizeof(char),30),_tag_arr(_tmp7C6,
sizeof(void*),0));});return bogus_ans;_LL524:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp7C8=Cyc_Tcutil_compress(
e_typ);void*_tmp7C9;struct Cyc_Absyn_Tqual _tmp7CA;_LL56A: if(_tmp7C8 <= (void*)3?1:*((
int*)_tmp7C8)!= 7)goto _LL56C;_tmp7C9=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp7C8)->f1;_tmp7CA=((struct Cyc_Absyn_ArrayType_struct*)_tmp7C8)->f2;_LL56B: {
void*_tmp7CC;struct _tuple9 _tmp7CB=Cyc_Tcutil_addressof_props(te,e);_tmp7CC=
_tmp7CB.f2;return Cyc_Absyn_atb_typ(_tmp7C9,_tmp7CC,_tmp7CA,(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp7CD=_cycalloc(sizeof(*_tmp7CD));_tmp7CD[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp7CE;_tmp7CE.tag=0;_tmp7CE.f1=e;_tmp7CE;});_tmp7CD;}));}_LL56C:;_LL56D: return
e_typ;_LL569:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,
unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(b);{void*
_tmp7CF=(void*)b->v;void*_tmp7D0;void*_tmp7D1;struct Cyc_Absyn_Exp*_tmp7D2;_LL56F:
if(_tmp7CF <= (void*)1?1:*((int*)_tmp7CF)!= 0)goto _LL571;_tmp7D0=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp7CF)->f1;if((int)_tmp7D0 != 0)goto _LL571;_LL570:
return;_LL571: if(_tmp7CF <= (void*)1?1:*((int*)_tmp7CF)!= 0)goto _LL573;_tmp7D1=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7CF)->f1;if(_tmp7D1 <= (void*)1?1:*((
int*)_tmp7D1)!= 0)goto _LL573;_tmp7D2=((struct Cyc_Absyn_Upper_b_struct*)_tmp7D1)->f1;
_LL572: if(Cyc_Evexp_eval_const_uint_exp(_tmp7D2)<= i)({struct Cyc_Std_Int_pa_struct
_tmp7D5;_tmp7D5.tag=1;_tmp7D5.f1=(int)i;{struct Cyc_Std_Int_pa_struct _tmp7D4;
_tmp7D4.tag=1;_tmp7D4.f1=(int)Cyc_Evexp_eval_const_uint_exp(_tmp7D2);{void*
_tmp7D3[2]={& _tmp7D4,& _tmp7D5};Cyc_Tcutil_terr(loc,_tag_arr("dereference is out of bounds: %d <= %d",
sizeof(char),39),_tag_arr(_tmp7D3,sizeof(void*),2));}}});return;_LL573:;_LL574:(
void*)(b->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp7D6=_cycalloc(
sizeof(*_tmp7D6));_tmp7D6[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp7D7;_tmp7D7.tag=
0;_tmp7D7.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp7D8=_cycalloc(
sizeof(*_tmp7D8));_tmp7D8[0]=({struct Cyc_Absyn_Upper_b_struct _tmp7D9;_tmp7D9.tag=
0;_tmp7D9.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmp7D9;});_tmp7D8;}));_tmp7D7;});
_tmp7D6;})));return;_LL56E:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp7DA=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp7DB;struct Cyc_Absyn_Exp*_tmp7DC;_LL576: if(_tmp7DA <= (void*)1?1:*((int*)
_tmp7DA)!= 0)goto _LL578;_tmp7DB=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7DA)->f1;if(_tmp7DB <= (void*)1?1:*((int*)_tmp7DB)!= 0)goto _LL578;_tmp7DC=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7DB)->f1;_LL577: return Cyc_Evexp_eval_const_uint_exp(
_tmp7DC)== 1;_LL578:;_LL579: return 0;_LL575:;}int Cyc_Tcutil_bits_only(void*t){
void*_tmp7DD=Cyc_Tcutil_compress(t);void*_tmp7DE;struct Cyc_List_List*_tmp7DF;
struct Cyc_Absyn_AggrInfo _tmp7E0;void*_tmp7E1;void*_tmp7E2;struct Cyc_Absyn_AggrInfo
_tmp7E3;void*_tmp7E4;void*_tmp7E5;struct Cyc_Absyn_AggrInfo _tmp7E6;void*_tmp7E7;
struct Cyc_Absyn_Aggrdecl**_tmp7E8;struct Cyc_Absyn_Aggrdecl*_tmp7E9;struct Cyc_List_List*
_tmp7EA;struct Cyc_List_List*_tmp7EB;_LL57B: if((int)_tmp7DD != 0)goto _LL57D;_LL57C:
goto _LL57E;_LL57D: if(_tmp7DD <= (void*)3?1:*((int*)_tmp7DD)!= 5)goto _LL57F;_LL57E:
goto _LL580;_LL57F: if((int)_tmp7DD != 1)goto _LL581;_LL580: goto _LL582;_LL581: if(
_tmp7DD <= (void*)3?1:*((int*)_tmp7DD)!= 6)goto _LL583;_LL582: return 1;_LL583: if(
_tmp7DD <= (void*)3?1:*((int*)_tmp7DD)!= 12)goto _LL585;_LL584: goto _LL586;_LL585:
if(_tmp7DD <= (void*)3?1:*((int*)_tmp7DD)!= 13)goto _LL587;_LL586: return 0;_LL587:
if(_tmp7DD <= (void*)3?1:*((int*)_tmp7DD)!= 7)goto _LL589;_tmp7DE=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp7DD)->f1;_LL588: return Cyc_Tcutil_bits_only(
_tmp7DE);_LL589: if(_tmp7DD <= (void*)3?1:*((int*)_tmp7DD)!= 9)goto _LL58B;_tmp7DF=((
struct Cyc_Absyn_TupleType_struct*)_tmp7DD)->f1;_LL58A: for(0;_tmp7DF != 0;_tmp7DF=
_tmp7DF->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple4*)_tmp7DF->hd)).f2))
return 0;}return 1;_LL58B: if(_tmp7DD <= (void*)3?1:*((int*)_tmp7DD)!= 10)goto _LL58D;
_tmp7E0=((struct Cyc_Absyn_AggrType_struct*)_tmp7DD)->f1;_tmp7E1=(void*)_tmp7E0.aggr_info;
if(*((int*)_tmp7E1)!= 0)goto _LL58D;_tmp7E2=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp7E1)->f1;if((int)_tmp7E2 != 1)goto _LL58D;_LL58C: return 1;_LL58D: if(_tmp7DD <= (
void*)3?1:*((int*)_tmp7DD)!= 10)goto _LL58F;_tmp7E3=((struct Cyc_Absyn_AggrType_struct*)
_tmp7DD)->f1;_tmp7E4=(void*)_tmp7E3.aggr_info;if(*((int*)_tmp7E4)!= 0)goto _LL58F;
_tmp7E5=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp7E4)->f1;if((int)
_tmp7E5 != 0)goto _LL58F;_LL58E: return 0;_LL58F: if(_tmp7DD <= (void*)3?1:*((int*)
_tmp7DD)!= 10)goto _LL591;_tmp7E6=((struct Cyc_Absyn_AggrType_struct*)_tmp7DD)->f1;
_tmp7E7=(void*)_tmp7E6.aggr_info;if(*((int*)_tmp7E7)!= 1)goto _LL591;_tmp7E8=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp7E7)->f1;_tmp7E9=*_tmp7E8;_tmp7EA=_tmp7E6.targs;
_LL590: if((void*)_tmp7E9->kind == (void*)1)return 1;if(_tmp7E9->impl == 0)return 0;{
struct _RegionHandle _tmp7EC=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7EC;
_push_region(rgn);{struct Cyc_List_List*_tmp7ED=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp7E9->tvs,_tmp7EA);{struct Cyc_List_List*fs=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7E9->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp7ED,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmp7EE=0;_npop_handler(0);return _tmp7EE;}}}{int _tmp7EF=1;
_npop_handler(0);return _tmp7EF;}};_pop_region(rgn);}_LL591: if(_tmp7DD <= (void*)3?
1:*((int*)_tmp7DD)!= 11)goto _LL593;_tmp7EB=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp7DD)->f2;_LL592: for(0;_tmp7EB != 0;_tmp7EB=_tmp7EB->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp7EB->hd)->type))return 0;}return 1;_LL593:;
_LL594: return 0;_LL57A:;}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmp7F0=(void*)e->r;struct _tuple1*_tmp7F1;void*_tmp7F2;struct Cyc_Absyn_Exp*
_tmp7F3;struct Cyc_Absyn_Exp*_tmp7F4;struct Cyc_Absyn_Exp*_tmp7F5;struct Cyc_Absyn_Exp*
_tmp7F6;struct Cyc_Absyn_Exp*_tmp7F7;struct Cyc_Absyn_Exp*_tmp7F8;struct Cyc_Absyn_Exp*
_tmp7F9;void*_tmp7FA;struct Cyc_Absyn_Exp*_tmp7FB;struct Cyc_Absyn_Exp*_tmp7FC;
struct Cyc_Absyn_Exp*_tmp7FD;struct Cyc_Absyn_Exp*_tmp7FE;struct Cyc_List_List*
_tmp7FF;struct Cyc_List_List*_tmp800;struct Cyc_List_List*_tmp801;void*_tmp802;
struct Cyc_List_List*_tmp803;struct Cyc_List_List*_tmp804;struct Cyc_List_List*
_tmp805;_LL596: if(*((int*)_tmp7F0)!= 0)goto _LL598;_LL597: goto _LL599;_LL598: if(*((
int*)_tmp7F0)!= 16)goto _LL59A;_LL599: goto _LL59B;_LL59A: if(*((int*)_tmp7F0)!= 17)
goto _LL59C;_LL59B: goto _LL59D;_LL59C: if(*((int*)_tmp7F0)!= 18)goto _LL59E;_LL59D:
goto _LL59F;_LL59E: if(*((int*)_tmp7F0)!= 19)goto _LL5A0;_LL59F: goto _LL5A1;_LL5A0:
if(*((int*)_tmp7F0)!= 31)goto _LL5A2;_LL5A1: goto _LL5A3;_LL5A2: if(*((int*)_tmp7F0)
!= 32)goto _LL5A4;_LL5A3: return 1;_LL5A4: if(*((int*)_tmp7F0)!= 1)goto _LL5A6;
_tmp7F1=((struct Cyc_Absyn_Var_e_struct*)_tmp7F0)->f1;_tmp7F2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp7F0)->f2;_LL5A5: {void*_tmp806=_tmp7F2;struct Cyc_Absyn_Vardecl*_tmp807;
_LL5C3: if(_tmp806 <= (void*)1?1:*((int*)_tmp806)!= 1)goto _LL5C5;_LL5C4: return 1;
_LL5C5: if(_tmp806 <= (void*)1?1:*((int*)_tmp806)!= 0)goto _LL5C7;_tmp807=((struct
Cyc_Absyn_Global_b_struct*)_tmp806)->f1;_LL5C6: {void*_tmp808=Cyc_Tcutil_compress((
void*)_tmp807->type);_LL5CC: if(_tmp808 <= (void*)3?1:*((int*)_tmp808)!= 7)goto
_LL5CE;_LL5CD: goto _LL5CF;_LL5CE: if(_tmp808 <= (void*)3?1:*((int*)_tmp808)!= 8)
goto _LL5D0;_LL5CF: return 1;_LL5D0:;_LL5D1: return var_okay;_LL5CB:;}_LL5C7: if((int)
_tmp806 != 0)goto _LL5C9;_LL5C8: return 0;_LL5C9:;_LL5CA: return var_okay;_LL5C2:;}
_LL5A6: if(*((int*)_tmp7F0)!= 6)goto _LL5A8;_tmp7F3=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp7F0)->f1;_tmp7F4=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7F0)->f2;
_tmp7F5=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7F0)->f3;_LL5A7: return(Cyc_Tcutil_cnst_exp(
te,0,_tmp7F3)?Cyc_Tcutil_cnst_exp(te,0,_tmp7F4): 0)?Cyc_Tcutil_cnst_exp(te,0,
_tmp7F5): 0;_LL5A8: if(*((int*)_tmp7F0)!= 7)goto _LL5AA;_tmp7F6=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp7F0)->f1;_tmp7F7=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp7F0)->f2;_LL5A9:
return Cyc_Tcutil_cnst_exp(te,0,_tmp7F6)?Cyc_Tcutil_cnst_exp(te,0,_tmp7F7): 0;
_LL5AA: if(*((int*)_tmp7F0)!= 11)goto _LL5AC;_tmp7F8=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp7F0)->f1;_LL5AB: _tmp7F9=_tmp7F8;goto _LL5AD;_LL5AC: if(*((int*)_tmp7F0)!= 12)
goto _LL5AE;_tmp7F9=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp7F0)->f1;_LL5AD:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp7F9);_LL5AE: if(*((int*)_tmp7F0)!= 13)
goto _LL5B0;_tmp7FA=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp7F0)->f1;_tmp7FB=((
struct Cyc_Absyn_Cast_e_struct*)_tmp7F0)->f2;_LL5AF: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmp7FB);_LL5B0: if(*((int*)_tmp7F0)!= 14)goto _LL5B2;_tmp7FC=((struct Cyc_Absyn_Address_e_struct*)
_tmp7F0)->f1;_LL5B1: return Cyc_Tcutil_cnst_exp(te,1,_tmp7FC);_LL5B2: if(*((int*)
_tmp7F0)!= 27)goto _LL5B4;_tmp7FD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp7F0)->f2;_tmp7FE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7F0)->f3;
_LL5B3: return Cyc_Tcutil_cnst_exp(te,0,_tmp7FD)?Cyc_Tcutil_cnst_exp(te,0,_tmp7FE):
0;_LL5B4: if(*((int*)_tmp7F0)!= 26)goto _LL5B6;_tmp7FF=((struct Cyc_Absyn_Array_e_struct*)
_tmp7F0)->f1;_LL5B5: _tmp800=_tmp7FF;goto _LL5B7;_LL5B6: if(*((int*)_tmp7F0)!= 29)
goto _LL5B8;_tmp800=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp7F0)->f2;_LL5B7:
_tmp801=_tmp800;goto _LL5B9;_LL5B8: if(*((int*)_tmp7F0)!= 28)goto _LL5BA;_tmp801=((
struct Cyc_Absyn_Struct_e_struct*)_tmp7F0)->f3;_LL5B9: for(0;_tmp801 != 0;_tmp801=
_tmp801->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple20*)_tmp801->hd)).f2))
return 0;}return 1;_LL5BA: if(*((int*)_tmp7F0)!= 3)goto _LL5BC;_tmp802=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp7F0)->f1;_tmp803=((struct Cyc_Absyn_Primop_e_struct*)
_tmp7F0)->f2;_LL5BB: _tmp804=_tmp803;goto _LL5BD;_LL5BC: if(*((int*)_tmp7F0)!= 24)
goto _LL5BE;_tmp804=((struct Cyc_Absyn_Tuple_e_struct*)_tmp7F0)->f1;_LL5BD: _tmp805=
_tmp804;goto _LL5BF;_LL5BE: if(*((int*)_tmp7F0)!= 30)goto _LL5C0;_tmp805=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp7F0)->f1;_LL5BF: for(0;_tmp805 != 0;_tmp805=_tmp805->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp805->hd))return 0;}return 1;
_LL5C0:;_LL5C1: return 0;_LL595:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp809=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp80A;
struct Cyc_Absyn_Conref*_tmp80B;struct Cyc_Absyn_Conref*_tmp80C;void*_tmp80D;
struct Cyc_List_List*_tmp80E;struct Cyc_Absyn_AggrInfo _tmp80F;void*_tmp810;struct
Cyc_List_List*_tmp811;struct Cyc_List_List*_tmp812;_LL5D3: if((int)_tmp809 != 0)
goto _LL5D5;_LL5D4: goto _LL5D6;_LL5D5: if(_tmp809 <= (void*)3?1:*((int*)_tmp809)!= 5)
goto _LL5D7;_LL5D6: goto _LL5D8;_LL5D7: if((int)_tmp809 != 1)goto _LL5D9;_LL5D8: goto
_LL5DA;_LL5D9: if(_tmp809 <= (void*)3?1:*((int*)_tmp809)!= 6)goto _LL5DB;_LL5DA:
return 1;_LL5DB: if(_tmp809 <= (void*)3?1:*((int*)_tmp809)!= 4)goto _LL5DD;_tmp80A=((
struct Cyc_Absyn_PointerType_struct*)_tmp809)->f1;_tmp80B=_tmp80A.nullable;
_tmp80C=_tmp80A.bounds;_LL5DC: {void*_tmp813=(void*)(Cyc_Absyn_compress_conref(
_tmp80C))->v;void*_tmp814;void*_tmp815;_LL5EC: if(_tmp813 <= (void*)1?1:*((int*)
_tmp813)!= 0)goto _LL5EE;_tmp814=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp813)->f1;if((int)_tmp814 != 0)goto _LL5EE;_LL5ED: return 1;_LL5EE: if(_tmp813 <= (
void*)1?1:*((int*)_tmp813)!= 0)goto _LL5F0;_tmp815=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp813)->f1;if(_tmp815 <= (void*)1?1:*((int*)_tmp815)!= 0)goto _LL5F0;_LL5EF: {
void*_tmp816=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp80B))->v;int _tmp817;_LL5F3: if(_tmp816 <= (void*)1?1:*((int*)_tmp816)!= 0)goto
_LL5F5;_tmp817=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp816)->f1;_LL5F4:
return _tmp817;_LL5F5:;_LL5F6: return 0;_LL5F2:;}_LL5F0:;_LL5F1: return 0;_LL5EB:;}
_LL5DD: if(_tmp809 <= (void*)3?1:*((int*)_tmp809)!= 7)goto _LL5DF;_tmp80D=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp809)->f1;_LL5DE: return Cyc_Tcutil_supports_default(
_tmp80D);_LL5DF: if(_tmp809 <= (void*)3?1:*((int*)_tmp809)!= 9)goto _LL5E1;_tmp80E=((
struct Cyc_Absyn_TupleType_struct*)_tmp809)->f1;_LL5E0: for(0;_tmp80E != 0;_tmp80E=
_tmp80E->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple4*)_tmp80E->hd)).f2))
return 0;}return 1;_LL5E1: if(_tmp809 <= (void*)3?1:*((int*)_tmp809)!= 10)goto _LL5E3;
_tmp80F=((struct Cyc_Absyn_AggrType_struct*)_tmp809)->f1;_tmp810=(void*)_tmp80F.aggr_info;
_tmp811=_tmp80F.targs;_LL5E2: {struct Cyc_Absyn_Aggrdecl*_tmp818=Cyc_Absyn_get_known_aggrdecl(
_tmp810);if(_tmp818->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp818->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp818->tvs,_tmp811,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp818->impl))->fields);}
_LL5E3: if(_tmp809 <= (void*)3?1:*((int*)_tmp809)!= 11)goto _LL5E5;_tmp812=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp809)->f2;_LL5E4: return Cyc_Tcutil_fields_support_default(
0,0,_tmp812);_LL5E5: if(_tmp809 <= (void*)3?1:*((int*)_tmp809)!= 13)goto _LL5E7;
_LL5E6: goto _LL5E8;_LL5E7: if(_tmp809 <= (void*)3?1:*((int*)_tmp809)!= 12)goto
_LL5E9;_LL5E8: return 1;_LL5E9:;_LL5EA: return 0;_LL5D2:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct
_RegionHandle _tmp819=_new_region("rgn");struct _RegionHandle*rgn=& _tmp819;
_push_region(rgn);{struct Cyc_List_List*_tmp81A=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp81A,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp81B=0;_npop_handler(0);return _tmp81B;}}};_pop_region(rgn);}return 1;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp81C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp81D;void*_tmp81E;struct Cyc_Absyn_FnInfo _tmp81F;struct Cyc_List_List*_tmp820;
_LL5F8: if(_tmp81C <= (void*)3?1:*((int*)_tmp81C)!= 4)goto _LL5FA;_tmp81D=((struct
Cyc_Absyn_PointerType_struct*)_tmp81C)->f1;_tmp81E=(void*)_tmp81D.elt_typ;_LL5F9:
return Cyc_Tcutil_is_noreturn(_tmp81E);_LL5FA: if(_tmp81C <= (void*)3?1:*((int*)
_tmp81C)!= 8)goto _LL5FC;_tmp81F=((struct Cyc_Absyn_FnType_struct*)_tmp81C)->f1;
_tmp820=_tmp81F.attributes;_LL5FB: for(0;_tmp820 != 0;_tmp820=_tmp820->tl){void*
_tmp821=(void*)_tmp820->hd;_LL5FF: if((int)_tmp821 != 3)goto _LL601;_LL600: return 1;
_LL601:;_LL602: continue;_LL5FE:;}goto _LL5F7;_LL5FC:;_LL5FD: goto _LL5F7;_LL5F7:;}
return 0;}
