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
 struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stderr;struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_Std_fflush(
struct Cyc_Std___cycFILE*);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;
};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};int Cyc_Std_printf(
struct _tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_Std_vrprintf(struct
_RegionHandle*,struct _tagged_arr,struct _tagged_arr);extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(int(*compare)(void*,
void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l,void*x);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
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
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*
effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);int Cyc_Std_strcmp(struct _tagged_arr s1,
struct _tagged_arr s2);int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);
struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct
_tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);void*
Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*
t);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
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
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple8{struct Cyc_Absyn_Tvar*
f1;void*f2;};struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct
Cyc_Absyn_Tvar*);struct _tuple9{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _tagged_arr*fn);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _tagged_arr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);struct _tuple10{int f1;void*f2;};struct _tuple10 Cyc_Tcutil_addressof_props(
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
_tmpD2;void*_tmpD3;struct Cyc_Absyn_Enumdecl*_tmpD4;struct Cyc_Absyn_AggrInfo
_tmpD5;void*_tmpD6;struct Cyc_Absyn_AggrInfo _tmpD7;void*_tmpD8;struct Cyc_Absyn_Aggrdecl**
_tmpD9;struct Cyc_Absyn_Aggrdecl*_tmpDA;struct Cyc_Absyn_Aggrdecl _tmpDB;struct Cyc_Absyn_AggrdeclImpl*
_tmpDC;struct Cyc_Absyn_Enumdecl*_tmpDD;struct Cyc_Absyn_PtrInfo _tmpDE;struct Cyc_Absyn_Typedefdecl*
_tmpDF;_LL5F: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 0)goto _LL61;_tmpCB=((
struct Cyc_Absyn_Evar_struct*)_tmpCA)->f1;_tmpCC=((struct Cyc_Absyn_Evar_struct*)
_tmpCA)->f2;_LL60: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpCB))->v;
_LL61: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 1)goto _LL63;_tmpCD=((struct Cyc_Absyn_VarType_struct*)
_tmpCA)->f1;_LL62: return Cyc_Tcutil_tvar_kind(_tmpCD);_LL63: if((int)_tmpCA != 0)
goto _LL65;_LL64: return(void*)1;_LL65: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 5)
goto _LL67;_tmpCE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpCA)->f2;_LL66:
return _tmpCE == (void*)2?(void*)2:(void*)1;_LL67: if((int)_tmpCA != 1)goto _LL69;
_LL68: goto _LL6A;_LL69: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 6)goto _LL6B;_LL6A:
goto _LL6C;_LL6B: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 8)goto _LL6D;_LL6C:
return(void*)1;_LL6D: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 15)goto _LL6F;_LL6E:
return(void*)2;_LL6F: if((int)_tmpCA != 2)goto _LL71;_LL70: return(void*)3;_LL71: if(
_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 2)goto _LL73;_LL72: return(void*)2;_LL73: if(
_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 3)goto _LL75;_tmpCF=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpCA)->f1;_tmpD0=(void*)_tmpCF.field_info;if(*((int*)_tmpD0)!= 1)goto _LL75;
_tmpD1=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpD0)->f2;_LL74: if(_tmpD1->typs
== 0)return(void*)2;else{return(void*)1;}_LL75: if(_tmpCA <= (void*)3?1:*((int*)
_tmpCA)!= 3)goto _LL77;_tmpD2=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpCA)->f1;
_tmpD3=(void*)_tmpD2.field_info;if(*((int*)_tmpD3)!= 0)goto _LL77;_LL76:({void*
_tmpE0[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typ_kind: Unresolved TunionFieldType",sizeof(char),37),_tag_arr(_tmpE0,
sizeof(void*),0));});_LL77: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 12)goto _LL79;
_tmpD4=((struct Cyc_Absyn_EnumType_struct*)_tmpCA)->f2;if(_tmpD4 != 0)goto _LL79;
_LL78: goto _LL7A;_LL79: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 10)goto _LL7B;
_tmpD5=((struct Cyc_Absyn_AggrType_struct*)_tmpCA)->f1;_tmpD6=(void*)_tmpD5.aggr_info;
if(*((int*)_tmpD6)!= 0)goto _LL7B;_LL7A: return(void*)0;_LL7B: if(_tmpCA <= (void*)3?
1:*((int*)_tmpCA)!= 10)goto _LL7D;_tmpD7=((struct Cyc_Absyn_AggrType_struct*)
_tmpCA)->f1;_tmpD8=(void*)_tmpD7.aggr_info;if(*((int*)_tmpD8)!= 1)goto _LL7D;
_tmpD9=((struct Cyc_Absyn_KnownAggr_struct*)_tmpD8)->f1;_tmpDA=*_tmpD9;_tmpDB=*
_tmpDA;_tmpDC=_tmpDB.impl;_LL7C: return _tmpDC == 0?(void*)0:(void*)1;_LL7D: if(
_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 11)goto _LL7F;_LL7E: goto _LL80;_LL7F: if(
_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 13)goto _LL81;_LL80: return(void*)1;_LL81:
if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 12)goto _LL83;_tmpDD=((struct Cyc_Absyn_EnumType_struct*)
_tmpCA)->f2;_LL82: if(_tmpDD->fields == 0)return(void*)0;else{return(void*)1;}
_LL83: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 4)goto _LL85;_tmpDE=((struct Cyc_Absyn_PointerType_struct*)
_tmpCA)->f1;_LL84: {void*_tmpE1=(void*)(Cyc_Absyn_compress_conref(_tmpDE.bounds))->v;
void*_tmpE2;void*_tmpE3;_LL98: if(_tmpE1 <= (void*)1?1:*((int*)_tmpE1)!= 0)goto
_LL9A;_tmpE2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpE1)->f1;if((int)
_tmpE2 != 0)goto _LL9A;_LL99: return(void*)1;_LL9A: if(_tmpE1 <= (void*)1?1:*((int*)
_tmpE1)!= 0)goto _LL9C;_tmpE3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpE1)->f1;
if(_tmpE3 <= (void*)1?1:*((int*)_tmpE3)!= 0)goto _LL9C;_LL9B: return(void*)2;_LL9C:
if((int)_tmpE1 != 0)goto _LL9E;_LL9D: return(void*)1;_LL9E: if(_tmpE1 <= (void*)1?1:*((
int*)_tmpE1)!= 1)goto _LL97;_LL9F:({void*_tmpE4[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof(char),43),_tag_arr(_tmpE4,sizeof(void*),0));});_LL97:;}_LL85: if(_tmpCA <= (
void*)3?1:*((int*)_tmpCA)!= 14)goto _LL87;_LL86: return(void*)2;_LL87: if(_tmpCA <= (
void*)3?1:*((int*)_tmpCA)!= 18)goto _LL89;_LL88: return(void*)5;_LL89: if(_tmpCA <= (
void*)3?1:*((int*)_tmpCA)!= 17)goto _LL8B;_LL8A: return(void*)2;_LL8B: if(_tmpCA <= (
void*)3?1:*((int*)_tmpCA)!= 7)goto _LL8D;_LL8C: goto _LL8E;_LL8D: if(_tmpCA <= (void*)
3?1:*((int*)_tmpCA)!= 9)goto _LL8F;_LL8E: return(void*)1;_LL8F: if(_tmpCA <= (void*)
3?1:*((int*)_tmpCA)!= 16)goto _LL91;_tmpDF=((struct Cyc_Absyn_TypedefType_struct*)
_tmpCA)->f3;_LL90: if(_tmpDF == 0?1: _tmpDF->kind == 0)({struct Cyc_Std_String_pa_struct
_tmpE6;_tmpE6.tag=0;_tmpE6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpE5[1]={& _tmpE6};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typ_kind: typedef found: %s",sizeof(char),28),_tag_arr(_tmpE5,sizeof(
void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpDF->kind))->v;
_LL91: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 19)goto _LL93;_LL92: goto _LL94;
_LL93: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 20)goto _LL95;_LL94: goto _LL96;
_LL95: if(_tmpCA <= (void*)3?1:*((int*)_tmpCA)!= 21)goto _LL5E;_LL96: return(void*)4;
_LL5E:;}int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmpE7;
_push_handler(& _tmpE7);{int _tmpE9=0;if(setjmp(_tmpE7.handler))_tmpE9=1;if(!
_tmpE9){Cyc_Tcutil_unify_it(t1,t2);{int _tmpEA=1;_npop_handler(0);return _tmpEA;};
_pop_handler();}else{void*_tmpE8=(void*)_exn_thrown;void*_tmpEC=_tmpE8;_LLA1: if(
_tmpEC != Cyc_Tcutil_Unify)goto _LLA3;_LLA2: return 0;_LLA3:;_LLA4:(void)_throw(
_tmpEC);_LLA0:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){void*_tmpED=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmpEE;struct Cyc_Core_Opt*_tmpEF;struct Cyc_Core_Opt*
_tmpF0;struct Cyc_Core_Opt**_tmpF1;struct Cyc_Absyn_PtrInfo _tmpF2;void*_tmpF3;
struct Cyc_Absyn_FnInfo _tmpF4;struct Cyc_List_List*_tmpF5;struct Cyc_Core_Opt*
_tmpF6;void*_tmpF7;struct Cyc_List_List*_tmpF8;int _tmpF9;struct Cyc_Absyn_VarargInfo*
_tmpFA;struct Cyc_List_List*_tmpFB;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*
_tmpFD;struct Cyc_Absyn_TunionInfo _tmpFE;struct Cyc_List_List*_tmpFF;void*_tmp100;
struct Cyc_List_List*_tmp101;struct Cyc_Core_Opt*_tmp102;struct Cyc_Absyn_TunionFieldInfo
_tmp103;struct Cyc_List_List*_tmp104;struct Cyc_Absyn_AggrInfo _tmp105;struct Cyc_List_List*
_tmp106;struct Cyc_List_List*_tmp107;void*_tmp108;void*_tmp109;void*_tmp10A;void*
_tmp10B;struct Cyc_List_List*_tmp10C;_LLA6: if(_tmpED <= (void*)3?1:*((int*)_tmpED)
!= 1)goto _LLA8;_tmpEE=((struct Cyc_Absyn_VarType_struct*)_tmpED)->f1;_LLA7: if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmpEE)){
Cyc_Tcutil_failure_reason=_tag_arr("(type variable would escape scope)",sizeof(
char),35);(int)_throw((void*)Cyc_Tcutil_Unify);}goto _LLA5;_LLA8: if(_tmpED <= (
void*)3?1:*((int*)_tmpED)!= 0)goto _LLAA;_tmpEF=((struct Cyc_Absyn_Evar_struct*)
_tmpED)->f2;_tmpF0=((struct Cyc_Absyn_Evar_struct*)_tmpED)->f4;_tmpF1=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmpED)->f4;_LLA9: if(t == evar){Cyc_Tcutil_failure_reason=
_tag_arr("(occurs check)",sizeof(char),15);(int)_throw((void*)Cyc_Tcutil_Unify);}
else{if(_tmpEF != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpEF->v);else{int
problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpF1))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp10D=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpF1))->v;for(0;s != 
0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp10D=({struct Cyc_List_List*_tmp10E=
_cycalloc(sizeof(*_tmp10E));_tmp10E->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp10E->tl=
_tmp10D;_tmp10E;});}}*_tmpF1=({struct Cyc_Core_Opt*_tmp10F=_cycalloc(sizeof(*
_tmp10F));_tmp10F->v=_tmp10D;_tmp10F;});}}}goto _LLA5;_LLAA: if(_tmpED <= (void*)3?
1:*((int*)_tmpED)!= 4)goto _LLAC;_tmpF2=((struct Cyc_Absyn_PointerType_struct*)
_tmpED)->f1;_LLAB: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpF2.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmpF2.rgn_typ);goto _LLA5;_LLAC: if(_tmpED <= (void*)3?1:*((int*)
_tmpED)!= 7)goto _LLAE;_tmpF3=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpED)->f1;
_LLAD: Cyc_Tcutil_occurs(evar,r,env,_tmpF3);goto _LLA5;_LLAE: if(_tmpED <= (void*)3?
1:*((int*)_tmpED)!= 8)goto _LLB0;_tmpF4=((struct Cyc_Absyn_FnType_struct*)_tmpED)->f1;
_tmpF5=_tmpF4.tvars;_tmpF6=_tmpF4.effect;_tmpF7=(void*)_tmpF4.ret_typ;_tmpF8=
_tmpF4.args;_tmpF9=_tmpF4.c_varargs;_tmpFA=_tmpF4.cyc_varargs;_tmpFB=_tmpF4.rgn_po;
_tmpFC=_tmpF4.attributes;_LLAF: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmpF5,env);if(
_tmpF6 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpF6->v);Cyc_Tcutil_occurs(evar,
r,env,_tmpF7);for(0;_tmpF8 != 0;_tmpF8=_tmpF8->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple2*)_tmpF8->hd)).f3);}if(_tmpFA != 0)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmpFA->type);for(0;_tmpFB != 0;_tmpFB=_tmpFB->tl){struct _tuple6 _tmp111;
void*_tmp112;void*_tmp113;struct _tuple6*_tmp110=(struct _tuple6*)_tmpFB->hd;
_tmp111=*_tmp110;_tmp112=_tmp111.f1;_tmp113=_tmp111.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp112);Cyc_Tcutil_occurs(evar,r,env,_tmp113);}goto _LLA5;_LLB0: if(_tmpED <= (
void*)3?1:*((int*)_tmpED)!= 9)goto _LLB2;_tmpFD=((struct Cyc_Absyn_TupleType_struct*)
_tmpED)->f1;_LLB1: for(0;_tmpFD != 0;_tmpFD=_tmpFD->tl){Cyc_Tcutil_occurs(evar,r,
env,(*((struct _tuple4*)_tmpFD->hd)).f2);}goto _LLA5;_LLB2: if(_tmpED <= (void*)3?1:*((
int*)_tmpED)!= 2)goto _LLB4;_tmpFE=((struct Cyc_Absyn_TunionType_struct*)_tmpED)->f1;
_tmpFF=_tmpFE.targs;_tmp100=(void*)_tmpFE.rgn;_LLB3: Cyc_Tcutil_occurs(evar,r,env,
_tmp100);Cyc_Tcutil_occurslist(evar,r,env,_tmpFF);goto _LLA5;_LLB4: if(_tmpED <= (
void*)3?1:*((int*)_tmpED)!= 16)goto _LLB6;_tmp101=((struct Cyc_Absyn_TypedefType_struct*)
_tmpED)->f2;_tmp102=((struct Cyc_Absyn_TypedefType_struct*)_tmpED)->f4;_LLB5:
_tmp104=_tmp101;goto _LLB7;_LLB6: if(_tmpED <= (void*)3?1:*((int*)_tmpED)!= 3)goto
_LLB8;_tmp103=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpED)->f1;_tmp104=
_tmp103.targs;_LLB7: _tmp106=_tmp104;goto _LLB9;_LLB8: if(_tmpED <= (void*)3?1:*((
int*)_tmpED)!= 10)goto _LLBA;_tmp105=((struct Cyc_Absyn_AggrType_struct*)_tmpED)->f1;
_tmp106=_tmp105.targs;_LLB9: Cyc_Tcutil_occurslist(evar,r,env,_tmp106);goto _LLA5;
_LLBA: if(_tmpED <= (void*)3?1:*((int*)_tmpED)!= 11)goto _LLBC;_tmp107=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpED)->f2;_LLBB: for(0;_tmp107 != 0;_tmp107=_tmp107->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp107->hd)->type);}goto _LLA5;_LLBC:
if(_tmpED <= (void*)3?1:*((int*)_tmpED)!= 15)goto _LLBE;_tmp108=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmpED)->f1;_LLBD: _tmp109=_tmp108;goto _LLBF;_LLBE: if(_tmpED <= (void*)3?1:*((int*)
_tmpED)!= 14)goto _LLC0;_tmp109=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmpED)->f1;_LLBF: _tmp10A=_tmp109;goto _LLC1;_LLC0: if(_tmpED <= (void*)3?1:*((int*)
_tmpED)!= 19)goto _LLC2;_tmp10A=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmpED)->f1;
_LLC1: _tmp10B=_tmp10A;goto _LLC3;_LLC2: if(_tmpED <= (void*)3?1:*((int*)_tmpED)!= 
21)goto _LLC4;_tmp10B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmpED)->f1;_LLC3:
Cyc_Tcutil_occurs(evar,r,env,_tmp10B);goto _LLA5;_LLC4: if(_tmpED <= (void*)3?1:*((
int*)_tmpED)!= 20)goto _LLC6;_tmp10C=((struct Cyc_Absyn_JoinEff_struct*)_tmpED)->f1;
_LLC5: Cyc_Tcutil_occurslist(evar,r,env,_tmp10C);goto _LLA5;_LLC6:;_LLC7: goto _LLA5;
_LLA5:;}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
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
y=Cyc_Absyn_compress_conref(y);if(x == y)return;{void*_tmp114=(void*)x->v;void*
_tmp115;_LLC9: if((int)_tmp114 != 0)goto _LLCB;_LLCA:(void*)(x->v=(void*)((void*)({
struct Cyc_Absyn_Forward_constr_struct*_tmp116=_cycalloc(sizeof(*_tmp116));
_tmp116[0]=({struct Cyc_Absyn_Forward_constr_struct _tmp117;_tmp117.tag=1;_tmp117.f1=
y;_tmp117;});_tmp116;})));return;_LLCB: if(_tmp114 <= (void*)1?1:*((int*)_tmp114)
!= 1)goto _LLCD;_LLCC:({void*_tmp118[0]={};Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress",
sizeof(char),37),_tag_arr(_tmp118,sizeof(void*),0));});_LLCD: if(_tmp114 <= (void*)
1?1:*((int*)_tmp114)!= 0)goto _LLC8;_tmp115=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp114)->f1;_LLCE: {void*_tmp119=(void*)y->v;void*_tmp11A;_LLD0: if((int)_tmp119
!= 0)goto _LLD2;_LLD1:(void*)(y->v=(void*)((void*)x->v));return;_LLD2: if(_tmp119
<= (void*)1?1:*((int*)_tmp119)!= 1)goto _LLD4;_LLD3:({void*_tmp11B[0]={};Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)",sizeof(char),40),_tag_arr(
_tmp11B,sizeof(void*),0));});_LLD4: if(_tmp119 <= (void*)1?1:*((int*)_tmp119)!= 0)
goto _LLCF;_tmp11A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp119)->f1;_LLD5:
if(cmp(_tmp115,_tmp11A)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)
Cyc_Tcutil_Unify);}return;_LLCF:;}_LLC8:;}}static int Cyc_Tcutil_unify_conrefs(int(*
cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp11C;_push_handler(& _tmp11C);{int _tmp11E=0;if(setjmp(_tmp11C.handler))
_tmp11E=1;if(!_tmp11E){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)
_tag_arr(0,0,0));{int _tmp11F=1;_npop_handler(0);return _tmp11F;};_pop_handler();}
else{void*_tmp11D=(void*)_exn_thrown;void*_tmp121=_tmp11D;_LLD7: if(_tmp121 != Cyc_Tcutil_Unify)
goto _LLD9;_LLD8: return 0;_LLD9:;_LLDA:(void)_throw(_tmp121);_LLD6:;}}}static int
Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple6 _tmp123=({struct _tuple6
_tmp122;_tmp122.f1=b1;_tmp122.f2=b2;_tmp122;});void*_tmp124;void*_tmp125;void*
_tmp126;void*_tmp127;void*_tmp128;struct Cyc_Absyn_Exp*_tmp129;void*_tmp12A;
struct Cyc_Absyn_Exp*_tmp12B;_LLDC: _tmp124=_tmp123.f1;if((int)_tmp124 != 0)goto
_LLDE;_tmp125=_tmp123.f2;if((int)_tmp125 != 0)goto _LLDE;_LLDD: return 0;_LLDE:
_tmp126=_tmp123.f1;if((int)_tmp126 != 0)goto _LLE0;_LLDF: return - 1;_LLE0: _tmp127=
_tmp123.f2;if((int)_tmp127 != 0)goto _LLE2;_LLE1: return 1;_LLE2: _tmp128=_tmp123.f1;
if(_tmp128 <= (void*)1?1:*((int*)_tmp128)!= 0)goto _LLDB;_tmp129=((struct Cyc_Absyn_Upper_b_struct*)
_tmp128)->f1;_tmp12A=_tmp123.f2;if(_tmp12A <= (void*)1?1:*((int*)_tmp12A)!= 0)
goto _LLDB;_tmp12B=((struct Cyc_Absyn_Upper_b_struct*)_tmp12A)->f1;_LLE3: return Cyc_Evexp_const_exp_cmp(
_tmp129,_tmp12B);_LLDB:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){if(a1 == 
a2)return 1;{struct _tuple6 _tmp12D=({struct _tuple6 _tmp12C;_tmp12C.f1=a1;_tmp12C.f2=
a2;_tmp12C;});void*_tmp12E;void*_tmp12F;int _tmp130;int _tmp131;void*_tmp132;void*
_tmp133;int _tmp134;int _tmp135;void*_tmp136;int _tmp137;void*_tmp138;int _tmp139;
void*_tmp13A;int _tmp13B;void*_tmp13C;int _tmp13D;void*_tmp13E;struct _tagged_arr
_tmp13F;void*_tmp140;struct _tagged_arr _tmp141;_LLE5: _tmp12E=_tmp12D.f1;if(
_tmp12E <= (void*)16?1:*((int*)_tmp12E)!= 3)goto _LLE7;_tmp12F=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp12E)->f1;_tmp130=((struct Cyc_Absyn_Format_att_struct*)_tmp12E)->f2;_tmp131=((
struct Cyc_Absyn_Format_att_struct*)_tmp12E)->f3;_tmp132=_tmp12D.f2;if(_tmp132 <= (
void*)16?1:*((int*)_tmp132)!= 3)goto _LLE7;_tmp133=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp132)->f1;_tmp134=((struct Cyc_Absyn_Format_att_struct*)_tmp132)->f2;_tmp135=((
struct Cyc_Absyn_Format_att_struct*)_tmp132)->f3;_LLE6: return(_tmp12F == _tmp133?
_tmp130 == _tmp134: 0)?_tmp131 == _tmp135: 0;_LLE7: _tmp136=_tmp12D.f1;if(_tmp136 <= (
void*)16?1:*((int*)_tmp136)!= 0)goto _LLE9;_tmp137=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp136)->f1;_tmp138=_tmp12D.f2;if(_tmp138 <= (void*)16?1:*((int*)_tmp138)!= 0)
goto _LLE9;_tmp139=((struct Cyc_Absyn_Regparm_att_struct*)_tmp138)->f1;_LLE8:
_tmp13B=_tmp137;_tmp13D=_tmp139;goto _LLEA;_LLE9: _tmp13A=_tmp12D.f1;if(_tmp13A <= (
void*)16?1:*((int*)_tmp13A)!= 1)goto _LLEB;_tmp13B=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp13A)->f1;_tmp13C=_tmp12D.f2;if(_tmp13C <= (void*)16?1:*((int*)_tmp13C)!= 1)
goto _LLEB;_tmp13D=((struct Cyc_Absyn_Aligned_att_struct*)_tmp13C)->f1;_LLEA:
return _tmp13B == _tmp13D;_LLEB: _tmp13E=_tmp12D.f1;if(_tmp13E <= (void*)16?1:*((int*)
_tmp13E)!= 2)goto _LLED;_tmp13F=((struct Cyc_Absyn_Section_att_struct*)_tmp13E)->f1;
_tmp140=_tmp12D.f2;if(_tmp140 <= (void*)16?1:*((int*)_tmp140)!= 2)goto _LLED;
_tmp141=((struct Cyc_Absyn_Section_att_struct*)_tmp140)->f1;_LLEC: return Cyc_Std_strcmp(
_tmp13F,_tmp141)== 0;_LLED:;_LLEE: return 0;_LLE4:;}}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return
0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((
void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*
tv){void*t;{void*_tmp142=Cyc_Tcutil_tvar_kind(tv);_LLF0: if((int)_tmp142 != 3)goto
_LLF2;_LLF1: t=(void*)2;goto _LLEF;_LLF2: if((int)_tmp142 != 4)goto _LLF4;_LLF3: t=Cyc_Absyn_empty_effect;
goto _LLEF;_LLF4:;_LLF5: t=Cyc_Absyn_sint_typ;goto _LLEF;_LLEF:;}return({struct
_tuple8*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->f1=tv;_tmp143->f2=t;_tmp143;});}
static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp144=Cyc_Tcutil_compress(t);void*
_tmp145;struct Cyc_Absyn_TunionInfo _tmp146;struct Cyc_List_List*_tmp147;void*
_tmp148;struct Cyc_Absyn_PtrInfo _tmp149;void*_tmp14A;void*_tmp14B;void*_tmp14C;
struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_TunionFieldInfo _tmp14E;struct Cyc_List_List*
_tmp14F;struct Cyc_Absyn_AggrInfo _tmp150;struct Cyc_List_List*_tmp151;struct Cyc_List_List*
_tmp152;void*_tmp153;struct Cyc_Absyn_FnInfo _tmp154;struct Cyc_List_List*_tmp155;
struct Cyc_Core_Opt*_tmp156;void*_tmp157;struct Cyc_List_List*_tmp158;struct Cyc_Absyn_VarargInfo*
_tmp159;struct Cyc_List_List*_tmp15A;void*_tmp15B;struct Cyc_List_List*_tmp15C;
_LLF7: if((int)_tmp144 != 0)goto _LLF9;_LLF8: goto _LLFA;_LLF9: if((int)_tmp144 != 1)
goto _LLFB;_LLFA: goto _LLFC;_LLFB: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 6)
goto _LLFD;_LLFC: goto _LLFE;_LLFD: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 12)
goto _LLFF;_LLFE: goto _LL100;_LLFF: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 13)
goto _LL101;_LL100: goto _LL102;_LL101: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 
18)goto _LL103;_LL102: goto _LL104;_LL103: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)
!= 5)goto _LL105;_LL104: return Cyc_Absyn_empty_effect;_LL105: if(_tmp144 <= (void*)3?
1:*((int*)_tmp144)!= 0)goto _LL107;_LL106: goto _LL108;_LL107: if(_tmp144 <= (void*)3?
1:*((int*)_tmp144)!= 1)goto _LL109;_LL108: {void*_tmp15D=Cyc_Tcutil_typ_kind(t);
_LL12A: if((int)_tmp15D != 3)goto _LL12C;_LL12B: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp15F;_tmp15F.tag=19;_tmp15F.f1=(void*)t;_tmp15F;});_tmp15E;});_LL12C: if((int)
_tmp15D != 4)goto _LL12E;_LL12D: return t;_LL12E: if((int)_tmp15D != 5)goto _LL130;
_LL12F: return Cyc_Absyn_empty_effect;_LL130:;_LL131: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp161;_tmp161.tag=21;_tmp161.f1=(void*)t;_tmp161;});_tmp160;});_LL129:;}_LL109:
if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 15)goto _LL10B;_tmp145=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp144)->f1;_LL10A: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp163;_tmp163.tag=19;_tmp163.f1=(void*)_tmp145;_tmp163;});_tmp162;});_LL10B:
if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 2)goto _LL10D;_tmp146=((struct Cyc_Absyn_TunionType_struct*)
_tmp144)->f1;_tmp147=_tmp146.targs;_tmp148=(void*)_tmp146.rgn;_LL10C: {struct Cyc_List_List*
ts=({struct Cyc_List_List*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp167=_cycalloc(sizeof(*_tmp167));
_tmp167[0]=({struct Cyc_Absyn_AccessEff_struct _tmp168;_tmp168.tag=19;_tmp168.f1=(
void*)_tmp148;_tmp168;});_tmp167;}));_tmp166->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp147);_tmp166;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp165;_tmp165.tag=20;_tmp165.f1=ts;_tmp165;});_tmp164;}));}_LL10D: if(_tmp144 <= (
void*)3?1:*((int*)_tmp144)!= 4)goto _LL10F;_tmp149=((struct Cyc_Absyn_PointerType_struct*)
_tmp144)->f1;_tmp14A=(void*)_tmp149.elt_typ;_tmp14B=(void*)_tmp149.rgn_typ;
_LL10E: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp16A;_tmp16A.tag=20;_tmp16A.f1=({void*_tmp16B[2];_tmp16B[1]=Cyc_Tcutil_rgns_of(
_tmp14A);_tmp16B[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp16C=_cycalloc(
sizeof(*_tmp16C));_tmp16C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp16D;_tmp16D.tag=
19;_tmp16D.f1=(void*)_tmp14B;_tmp16D;});_tmp16C;});Cyc_List_list(_tag_arr(
_tmp16B,sizeof(void*),2));});_tmp16A;});_tmp169;}));_LL10F: if(_tmp144 <= (void*)3?
1:*((int*)_tmp144)!= 7)goto _LL111;_tmp14C=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp144)->f1;_LL110: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp14C));
_LL111: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 9)goto _LL113;_tmp14D=((struct
Cyc_Absyn_TupleType_struct*)_tmp144)->f1;_LL112: {struct Cyc_List_List*_tmp16E=0;
for(0;_tmp14D != 0;_tmp14D=_tmp14D->tl){_tmp16E=({struct Cyc_List_List*_tmp16F=
_cycalloc(sizeof(*_tmp16F));_tmp16F->hd=(void*)(*((struct _tuple4*)_tmp14D->hd)).f2;
_tmp16F->tl=_tmp16E;_tmp16F;});}_tmp14F=_tmp16E;goto _LL114;}_LL113: if(_tmp144 <= (
void*)3?1:*((int*)_tmp144)!= 3)goto _LL115;_tmp14E=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp144)->f1;_tmp14F=_tmp14E.targs;_LL114: _tmp151=_tmp14F;goto _LL116;_LL115: if(
_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 10)goto _LL117;_tmp150=((struct Cyc_Absyn_AggrType_struct*)
_tmp144)->f1;_tmp151=_tmp150.targs;_LL116: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp170=_cycalloc(sizeof(*_tmp170));
_tmp170[0]=({struct Cyc_Absyn_JoinEff_struct _tmp171;_tmp171.tag=20;_tmp171.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp151);_tmp171;});_tmp170;}));_LL117: if(_tmp144 <= (void*)3?
1:*((int*)_tmp144)!= 11)goto _LL119;_tmp152=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp144)->f2;_LL118: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp173;_tmp173.tag=20;_tmp173.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp152);_tmp173;});
_tmp172;}));_LL119: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 14)goto _LL11B;
_tmp153=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp144)->f1;_LL11A: return
Cyc_Tcutil_rgns_of(_tmp153);_LL11B: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 17)
goto _LL11D;_LL11C: return Cyc_Tcutil_rgns_of(t);_LL11D: if(_tmp144 <= (void*)3?1:*((
int*)_tmp144)!= 8)goto _LL11F;_tmp154=((struct Cyc_Absyn_FnType_struct*)_tmp144)->f1;
_tmp155=_tmp154.tvars;_tmp156=_tmp154.effect;_tmp157=(void*)_tmp154.ret_typ;
_tmp158=_tmp154.args;_tmp159=_tmp154.cyc_varargs;_tmp15A=_tmp154.rgn_po;_LL11E: {
void*_tmp174=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp155),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp156))->v);return Cyc_Tcutil_normalize_effect(
_tmp174);}_LL11F: if((int)_tmp144 != 2)goto _LL121;_LL120: return Cyc_Absyn_empty_effect;
_LL121: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 19)goto _LL123;_LL122: goto
_LL124;_LL123: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 20)goto _LL125;_LL124:
return t;_LL125: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 21)goto _LL127;_tmp15B=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp144)->f1;_LL126: return Cyc_Tcutil_rgns_of(
_tmp15B);_LL127: if(_tmp144 <= (void*)3?1:*((int*)_tmp144)!= 16)goto _LLF6;_tmp15C=((
struct Cyc_Absyn_TypedefType_struct*)_tmp144)->f2;_LL128: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp175=_cycalloc(sizeof(*_tmp175));
_tmp175[0]=({struct Cyc_Absyn_JoinEff_struct _tmp176;_tmp176.tag=20;_tmp176.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp15C);_tmp176;});_tmp175;}));_LLF6:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp177=e;struct Cyc_List_List*_tmp178;
struct Cyc_List_List**_tmp179;void*_tmp17A;_LL133: if(_tmp177 <= (void*)3?1:*((int*)
_tmp177)!= 20)goto _LL135;_tmp178=((struct Cyc_Absyn_JoinEff_struct*)_tmp177)->f1;
_tmp179=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)_tmp177)->f1;
_LL134: {int redo_join=0;{struct Cyc_List_List*effs=*_tmp179;for(0;effs != 0;effs=
effs->tl){void*_tmp17B=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect(_tmp17B)));{void*_tmp17C=(void*)effs->hd;void*
_tmp17D;_LL13A: if(_tmp17C <= (void*)3?1:*((int*)_tmp17C)!= 20)goto _LL13C;_LL13B:
goto _LL13D;_LL13C: if(_tmp17C <= (void*)3?1:*((int*)_tmp17C)!= 19)goto _LL13E;
_tmp17D=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp17C)->f1;if((int)_tmp17D
!= 2)goto _LL13E;_LL13D: redo_join=1;goto _LL139;_LL13E:;_LL13F: goto _LL139;_LL139:;}}}
if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp179;for(0;effs != 0;effs=effs->tl){void*_tmp17E=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp17F;void*_tmp180;_LL141: if(_tmp17E <= (void*)3?
1:*((int*)_tmp17E)!= 20)goto _LL143;_tmp17F=((struct Cyc_Absyn_JoinEff_struct*)
_tmp17E)->f1;_LL142: effects=Cyc_List_revappend(_tmp17F,effects);goto _LL140;
_LL143: if(_tmp17E <= (void*)3?1:*((int*)_tmp17E)!= 19)goto _LL145;_tmp180=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp17E)->f1;if((int)_tmp180 != 2)goto _LL145;
_LL144: goto _LL140;_LL145:;_LL146: effects=({struct Cyc_List_List*_tmp181=_cycalloc(
sizeof(*_tmp181));_tmp181->hd=(void*)_tmp17E;_tmp181->tl=effects;_tmp181;});goto
_LL140;_LL140:;}}*_tmp179=Cyc_List_imp_rev(effects);return e;}}_LL135: if(_tmp177
<= (void*)3?1:*((int*)_tmp177)!= 21)goto _LL137;_tmp17A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp177)->f1;_LL136: {void*_tmp182=Cyc_Tcutil_compress(_tmp17A);_LL148: if(
_tmp182 <= (void*)3?1:*((int*)_tmp182)!= 0)goto _LL14A;_LL149: goto _LL14B;_LL14A:
if(_tmp182 <= (void*)3?1:*((int*)_tmp182)!= 1)goto _LL14C;_LL14B: return e;_LL14C:;
_LL14D: return Cyc_Tcutil_rgns_of(_tmp17A);_LL147:;}_LL137:;_LL138: return e;_LL132:;}}
static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_struct*_tmp183=({struct Cyc_Absyn_FnType_struct*
_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184[0]=({struct Cyc_Absyn_FnType_struct
_tmp185;_tmp185.tag=8;_tmp185.f1=({struct Cyc_Absyn_FnInfo _tmp186;_tmp186.tvars=0;
_tmp186.effect=({struct Cyc_Core_Opt*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187->v=(
void*)eff;_tmp187;});_tmp186.ret_typ=(void*)((void*)0);_tmp186.args=0;_tmp186.c_varargs=
0;_tmp186.cyc_varargs=0;_tmp186.rgn_po=0;_tmp186.attributes=0;_tmp186;});_tmp185;});
_tmp184;});return Cyc_Absyn_atb_typ((void*)_tmp183,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_bounds_one);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{void*_tmp188=Cyc_Tcutil_compress(
e);void*_tmp189;struct Cyc_List_List*_tmp18A;void*_tmp18B;struct Cyc_Core_Opt*
_tmp18C;struct Cyc_Core_Opt*_tmp18D;struct Cyc_Core_Opt**_tmp18E;struct Cyc_Core_Opt*
_tmp18F;_LL14F: if(_tmp188 <= (void*)3?1:*((int*)_tmp188)!= 19)goto _LL151;_tmp189=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp188)->f1;_LL150: if(constrain)
return Cyc_Tcutil_unify(r,_tmp189);_tmp189=Cyc_Tcutil_compress(_tmp189);if(r == 
_tmp189)return 1;{struct _tuple6 _tmp191=({struct _tuple6 _tmp190;_tmp190.f1=r;
_tmp190.f2=_tmp189;_tmp190;});void*_tmp192;struct Cyc_Absyn_Tvar*_tmp193;void*
_tmp194;struct Cyc_Absyn_Tvar*_tmp195;_LL15A: _tmp192=_tmp191.f1;if(_tmp192 <= (
void*)3?1:*((int*)_tmp192)!= 1)goto _LL15C;_tmp193=((struct Cyc_Absyn_VarType_struct*)
_tmp192)->f1;_tmp194=_tmp191.f2;if(_tmp194 <= (void*)3?1:*((int*)_tmp194)!= 1)
goto _LL15C;_tmp195=((struct Cyc_Absyn_VarType_struct*)_tmp194)->f1;_LL15B: return
Cyc_Absyn_tvar_cmp(_tmp193,_tmp195)== 0;_LL15C:;_LL15D: return 0;_LL159:;}_LL151:
if(_tmp188 <= (void*)3?1:*((int*)_tmp188)!= 20)goto _LL153;_tmp18A=((struct Cyc_Absyn_JoinEff_struct*)
_tmp188)->f1;_LL152: for(0;_tmp18A != 0;_tmp18A=_tmp18A->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp18A->hd))return 1;}return 0;_LL153: if(_tmp188 <= (void*)3?1:*((
int*)_tmp188)!= 21)goto _LL155;_tmp18B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp188)->f1;_LL154: {void*_tmp196=Cyc_Tcutil_rgns_of(_tmp18B);void*_tmp197;
_LL15F: if(_tmp196 <= (void*)3?1:*((int*)_tmp196)!= 21)goto _LL161;_tmp197=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp196)->f1;_LL160: if(!constrain)return 0;{void*
_tmp198=Cyc_Tcutil_compress(_tmp197);struct Cyc_Core_Opt*_tmp199;struct Cyc_Core_Opt*
_tmp19A;struct Cyc_Core_Opt**_tmp19B;struct Cyc_Core_Opt*_tmp19C;_LL164: if(_tmp198
<= (void*)3?1:*((int*)_tmp198)!= 0)goto _LL166;_tmp199=((struct Cyc_Absyn_Evar_struct*)
_tmp198)->f1;_tmp19A=((struct Cyc_Absyn_Evar_struct*)_tmp198)->f2;_tmp19B=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp198)->f2;_tmp19C=((struct Cyc_Absyn_Evar_struct*)
_tmp198)->f4;_LL165: {void*_tmp19D=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp19C);Cyc_Tcutil_occurs(_tmp19D,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp19C))->v,r);{void*_tmp19E=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));
_tmp1A0[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1A1;_tmp1A1.tag=20;_tmp1A1.f1=({
void*_tmp1A2[2];_tmp1A2[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1A3=
_cycalloc(sizeof(*_tmp1A3));_tmp1A3[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1A4;
_tmp1A4.tag=19;_tmp1A4.f1=(void*)r;_tmp1A4;});_tmp1A3;});_tmp1A2[0]=_tmp19D;Cyc_List_list(
_tag_arr(_tmp1A2,sizeof(void*),2));});_tmp1A1;});_tmp1A0;}));*_tmp19B=({struct
Cyc_Core_Opt*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->v=(void*)_tmp19E;
_tmp19F;});return 1;}}_LL166:;_LL167: return 0;_LL163:;}_LL161:;_LL162: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp196);_LL15E:;}_LL155: if(_tmp188 <= (void*)3?1:*((int*)_tmp188)!= 
0)goto _LL157;_tmp18C=((struct Cyc_Absyn_Evar_struct*)_tmp188)->f1;_tmp18D=((
struct Cyc_Absyn_Evar_struct*)_tmp188)->f2;_tmp18E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp188)->f2;_tmp18F=((struct Cyc_Absyn_Evar_struct*)
_tmp188)->f4;_LL156: if(_tmp18C == 0?1:(void*)_tmp18C->v != (void*)4)({void*_tmp1A5[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(char),27),_tag_arr(_tmp1A5,sizeof(
void*),0));});if(!constrain)return 0;{void*_tmp1A6=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp18F);Cyc_Tcutil_occurs(_tmp1A6,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp18F))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp1A7=({struct Cyc_Absyn_JoinEff_struct*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));
_tmp1A9[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1AA;_tmp1AA.tag=20;_tmp1AA.f1=({
struct Cyc_List_List*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->hd=(void*)
_tmp1A6;_tmp1AB->tl=({struct Cyc_List_List*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));
_tmp1AC->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1AD=_cycalloc(
sizeof(*_tmp1AD));_tmp1AD[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1AE;_tmp1AE.tag=
19;_tmp1AE.f1=(void*)r;_tmp1AE;});_tmp1AD;}));_tmp1AC->tl=0;_tmp1AC;});_tmp1AB;});
_tmp1AA;});_tmp1A9;});*_tmp18E=({struct Cyc_Core_Opt*_tmp1A8=_cycalloc(sizeof(*
_tmp1A8));_tmp1A8->v=(void*)((void*)_tmp1A7);_tmp1A8;});return 1;}}_LL157:;_LL158:
return 0;_LL14E:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp1AF=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp1B0;void*_tmp1B1;struct Cyc_Core_Opt*_tmp1B2;struct
Cyc_Core_Opt*_tmp1B3;struct Cyc_Core_Opt**_tmp1B4;struct Cyc_Core_Opt*_tmp1B5;
_LL169: if(_tmp1AF <= (void*)3?1:*((int*)_tmp1AF)!= 19)goto _LL16B;_LL16A: return 0;
_LL16B: if(_tmp1AF <= (void*)3?1:*((int*)_tmp1AF)!= 20)goto _LL16D;_tmp1B0=((struct
Cyc_Absyn_JoinEff_struct*)_tmp1AF)->f1;_LL16C: for(0;_tmp1B0 != 0;_tmp1B0=_tmp1B0->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp1B0->hd))return 1;}
return 0;_LL16D: if(_tmp1AF <= (void*)3?1:*((int*)_tmp1AF)!= 21)goto _LL16F;_tmp1B1=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1AF)->f1;_LL16E: _tmp1B1=Cyc_Tcutil_compress(
_tmp1B1);if(t == _tmp1B1)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,
_tmp1B1);{void*_tmp1B6=Cyc_Tcutil_rgns_of(t);void*_tmp1B7;_LL174: if(_tmp1B6 <= (
void*)3?1:*((int*)_tmp1B6)!= 21)goto _LL176;_tmp1B7=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1B6)->f1;_LL175: {struct _tuple6 _tmp1B9=({struct _tuple6 _tmp1B8;_tmp1B8.f1=Cyc_Tcutil_compress(
_tmp1B7);_tmp1B8.f2=_tmp1B1;_tmp1B8;});void*_tmp1BA;struct Cyc_Absyn_Tvar*_tmp1BB;
void*_tmp1BC;struct Cyc_Absyn_Tvar*_tmp1BD;_LL179: _tmp1BA=_tmp1B9.f1;if(_tmp1BA <= (
void*)3?1:*((int*)_tmp1BA)!= 1)goto _LL17B;_tmp1BB=((struct Cyc_Absyn_VarType_struct*)
_tmp1BA)->f1;_tmp1BC=_tmp1B9.f2;if(_tmp1BC <= (void*)3?1:*((int*)_tmp1BC)!= 1)
goto _LL17B;_tmp1BD=((struct Cyc_Absyn_VarType_struct*)_tmp1BC)->f1;_LL17A: return
Cyc_Tcutil_unify(t,_tmp1B1);_LL17B:;_LL17C: return _tmp1B7 == _tmp1B1;_LL178:;}
_LL176:;_LL177: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp1B6);
_LL173:;}_LL16F: if(_tmp1AF <= (void*)3?1:*((int*)_tmp1AF)!= 0)goto _LL171;_tmp1B2=((
struct Cyc_Absyn_Evar_struct*)_tmp1AF)->f1;_tmp1B3=((struct Cyc_Absyn_Evar_struct*)
_tmp1AF)->f2;_tmp1B4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1AF)->f2;_tmp1B5=((struct Cyc_Absyn_Evar_struct*)_tmp1AF)->f4;_LL170: if(
_tmp1B2 == 0?1:(void*)_tmp1B2->v != (void*)4)({void*_tmp1BE[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",
sizeof(char),27),_tag_arr(_tmp1BE,sizeof(void*),0));});if(!may_constrain_evars)
return 0;{void*_tmp1BF=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1B5);Cyc_Tcutil_occurs(_tmp1BF,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1B5))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp1C0=({struct Cyc_Absyn_JoinEff_struct*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));
_tmp1C2[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1C3;_tmp1C3.tag=20;_tmp1C3.f1=({
struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=(void*)
_tmp1BF;_tmp1C4->tl=({struct Cyc_List_List*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));
_tmp1C5->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp1C6=_cycalloc(
sizeof(*_tmp1C6));_tmp1C6[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp1C7;_tmp1C7.tag=
21;_tmp1C7.f1=(void*)t;_tmp1C7;});_tmp1C6;}));_tmp1C5->tl=0;_tmp1C5;});_tmp1C4;});
_tmp1C3;});_tmp1C2;});*_tmp1B4=({struct Cyc_Core_Opt*_tmp1C1=_cycalloc(sizeof(*
_tmp1C1));_tmp1C1->v=(void*)((void*)_tmp1C0);_tmp1C1;});return 1;}}_LL171:;_LL172:
return 0;_LL168:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1C8=e;struct
Cyc_Absyn_Tvar*_tmp1C9;struct Cyc_List_List*_tmp1CA;void*_tmp1CB;struct Cyc_Core_Opt*
_tmp1CC;struct Cyc_Core_Opt*_tmp1CD;struct Cyc_Core_Opt**_tmp1CE;struct Cyc_Core_Opt*
_tmp1CF;_LL17E: if(_tmp1C8 <= (void*)3?1:*((int*)_tmp1C8)!= 1)goto _LL180;_tmp1C9=((
struct Cyc_Absyn_VarType_struct*)_tmp1C8)->f1;_LL17F: return Cyc_Absyn_tvar_cmp(v,
_tmp1C9)== 0;_LL180: if(_tmp1C8 <= (void*)3?1:*((int*)_tmp1C8)!= 20)goto _LL182;
_tmp1CA=((struct Cyc_Absyn_JoinEff_struct*)_tmp1C8)->f1;_LL181: for(0;_tmp1CA != 0;
_tmp1CA=_tmp1CA->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp1CA->hd))return 1;}return 0;_LL182: if(_tmp1C8 <= (void*)3?1:*((int*)
_tmp1C8)!= 21)goto _LL184;_tmp1CB=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1C8)->f1;_LL183: {void*_tmp1D0=Cyc_Tcutil_rgns_of(_tmp1CB);void*_tmp1D1;
_LL189: if(_tmp1D0 <= (void*)3?1:*((int*)_tmp1D0)!= 21)goto _LL18B;_tmp1D1=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1D0)->f1;_LL18A: if(!may_constrain_evars)
return 0;{void*_tmp1D2=Cyc_Tcutil_compress(_tmp1D1);struct Cyc_Core_Opt*_tmp1D3;
struct Cyc_Core_Opt*_tmp1D4;struct Cyc_Core_Opt**_tmp1D5;struct Cyc_Core_Opt*
_tmp1D6;_LL18E: if(_tmp1D2 <= (void*)3?1:*((int*)_tmp1D2)!= 0)goto _LL190;_tmp1D3=((
struct Cyc_Absyn_Evar_struct*)_tmp1D2)->f1;_tmp1D4=((struct Cyc_Absyn_Evar_struct*)
_tmp1D2)->f2;_tmp1D5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1D2)->f2;_tmp1D6=((struct Cyc_Absyn_Evar_struct*)_tmp1D2)->f4;_LL18F: {void*
_tmp1D7=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1D6);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D6))->v,v))return 0;{void*_tmp1D8=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));
_tmp1DA[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1DB;_tmp1DB.tag=20;_tmp1DB.f1=({
void*_tmp1DC[2];_tmp1DC[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp1DD=
_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=({struct Cyc_Absyn_VarType_struct _tmp1DE;
_tmp1DE.tag=1;_tmp1DE.f1=v;_tmp1DE;});_tmp1DD;});_tmp1DC[0]=_tmp1D7;Cyc_List_list(
_tag_arr(_tmp1DC,sizeof(void*),2));});_tmp1DB;});_tmp1DA;}));*_tmp1D5=({struct
Cyc_Core_Opt*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->v=(void*)_tmp1D8;
_tmp1D9;});return 1;}}_LL190:;_LL191: return 0;_LL18D:;}_LL18B:;_LL18C: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp1D0);_LL188:;}_LL184: if(_tmp1C8 <= (void*)3?1:*((int*)
_tmp1C8)!= 0)goto _LL186;_tmp1CC=((struct Cyc_Absyn_Evar_struct*)_tmp1C8)->f1;
_tmp1CD=((struct Cyc_Absyn_Evar_struct*)_tmp1C8)->f2;_tmp1CE=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1C8)->f2;_tmp1CF=((struct Cyc_Absyn_Evar_struct*)
_tmp1C8)->f4;_LL185: if(_tmp1CC == 0?1:(void*)_tmp1CC->v != (void*)4)({void*_tmp1DF[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(char),27),_tag_arr(_tmp1DF,sizeof(
void*),0));});{void*_tmp1E0=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1CF);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1CF))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct*_tmp1E1=({struct Cyc_Absyn_JoinEff_struct*_tmp1E3=
_cycalloc(sizeof(*_tmp1E3));_tmp1E3[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1E4;
_tmp1E4.tag=20;_tmp1E4.f1=({struct Cyc_List_List*_tmp1E5=_cycalloc(sizeof(*
_tmp1E5));_tmp1E5->hd=(void*)_tmp1E0;_tmp1E5->tl=({struct Cyc_List_List*_tmp1E6=
_cycalloc(sizeof(*_tmp1E6));_tmp1E6->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7[0]=({struct Cyc_Absyn_VarType_struct
_tmp1E8;_tmp1E8.tag=1;_tmp1E8.f1=v;_tmp1E8;});_tmp1E7;}));_tmp1E6->tl=0;_tmp1E6;});
_tmp1E5;});_tmp1E4;});_tmp1E3;});*_tmp1CE=({struct Cyc_Core_Opt*_tmp1E2=_cycalloc(
sizeof(*_tmp1E2));_tmp1E2->v=(void*)((void*)_tmp1E1);_tmp1E2;});return 1;}}_LL186:;
_LL187: return 0;_LL17D:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=
Cyc_Tcutil_compress(e);{void*_tmp1E9=e;struct Cyc_List_List*_tmp1EA;void*_tmp1EB;
_LL193: if(_tmp1E9 <= (void*)3?1:*((int*)_tmp1E9)!= 20)goto _LL195;_tmp1EA=((struct
Cyc_Absyn_JoinEff_struct*)_tmp1E9)->f1;_LL194: for(0;_tmp1EA != 0;_tmp1EA=_tmp1EA->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp1EA->hd))return 1;}return 0;_LL195:
if(_tmp1E9 <= (void*)3?1:*((int*)_tmp1E9)!= 21)goto _LL197;_tmp1EB=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp1E9)->f1;_LL196: {void*_tmp1EC=Cyc_Tcutil_rgns_of(
_tmp1EB);void*_tmp1ED;_LL19C: if(_tmp1EC <= (void*)3?1:*((int*)_tmp1EC)!= 21)goto
_LL19E;_tmp1ED=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1EC)->f1;_LL19D:
return 0;_LL19E:;_LL19F: return Cyc_Tcutil_evar_in_effect(evar,_tmp1EC);_LL19B:;}
_LL197: if(_tmp1E9 <= (void*)3?1:*((int*)_tmp1E9)!= 0)goto _LL199;_LL198: return evar
== e;_LL199:;_LL19A: return 0;_LL192:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp1EE=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp1EF;void*_tmp1F0;struct Cyc_Absyn_Tvar*_tmp1F1;void*_tmp1F2;
struct Cyc_Core_Opt*_tmp1F3;struct Cyc_Core_Opt**_tmp1F4;struct Cyc_Core_Opt*
_tmp1F5;_LL1A1: if(_tmp1EE <= (void*)3?1:*((int*)_tmp1EE)!= 20)goto _LL1A3;_tmp1EF=((
struct Cyc_Absyn_JoinEff_struct*)_tmp1EE)->f1;_LL1A2: for(0;_tmp1EF != 0;_tmp1EF=
_tmp1EF->tl){if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp1EF->hd,
e2))return 0;}return 1;_LL1A3: if(_tmp1EE <= (void*)3?1:*((int*)_tmp1EE)!= 19)goto
_LL1A5;_tmp1F0=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1EE)->f1;_LL1A4:
return Cyc_Tcutil_region_in_effect(0,_tmp1F0,e2)?1:(may_constrain_evars?Cyc_Tcutil_unify(
_tmp1F0,(void*)2): 0);_LL1A5: if(_tmp1EE <= (void*)3?1:*((int*)_tmp1EE)!= 1)goto
_LL1A7;_tmp1F1=((struct Cyc_Absyn_VarType_struct*)_tmp1EE)->f1;_LL1A6: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,_tmp1F1,e2);_LL1A7: if(_tmp1EE <= (void*)3?1:*((int*)_tmp1EE)
!= 21)goto _LL1A9;_tmp1F2=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1EE)->f1;
_LL1A8: {void*_tmp1F6=Cyc_Tcutil_rgns_of(_tmp1F2);void*_tmp1F7;_LL1AE: if(_tmp1F6
<= (void*)3?1:*((int*)_tmp1F6)!= 21)goto _LL1B0;_tmp1F7=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1F6)->f1;_LL1AF: return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp1F7,
e2)?1:(may_constrain_evars?Cyc_Tcutil_unify(_tmp1F7,Cyc_Absyn_sint_typ): 0);
_LL1B0:;_LL1B1: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp1F6,e2);
_LL1AD:;}_LL1A9: if(_tmp1EE <= (void*)3?1:*((int*)_tmp1EE)!= 0)goto _LL1AB;_tmp1F3=((
struct Cyc_Absyn_Evar_struct*)_tmp1EE)->f2;_tmp1F4=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1EE)->f2;_tmp1F5=((struct Cyc_Absyn_Evar_struct*)
_tmp1EE)->f4;_LL1AA: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp1F4=({struct Cyc_Core_Opt*
_tmp1F8=_cycalloc(sizeof(*_tmp1F8));_tmp1F8->v=(void*)Cyc_Absyn_empty_effect;
_tmp1F8;});return 1;_LL1AB:;_LL1AC:({struct Cyc_Std_String_pa_struct _tmp1FA;
_tmp1FA.tag=0;_tmp1FA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e1);{void*
_tmp1F9[1]={& _tmp1FA};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("subset_effect: bad effect: %s",sizeof(char),30),_tag_arr(_tmp1F9,
sizeof(void*),1));}});_LL1A0:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(
0,e1,e2)?Cyc_Tcutil_subset_effect(0,e2,e1): 0)return 1;if(Cyc_Tcutil_subset_effect(
1,e1,e2)?Cyc_Tcutil_subset_effect(1,e2,e1): 0)return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple6 _tmp1FC;void*_tmp1FD;void*_tmp1FE;struct
_tuple6*_tmp1FB=(struct _tuple6*)r1->hd;_tmp1FC=*_tmp1FB;_tmp1FD=_tmp1FC.f1;
_tmp1FE=_tmp1FC.f2;{int found=_tmp1FD == (void*)2;{struct Cyc_List_List*r2=rpo2;
for(0;r2 != 0?!found: 0;r2=r2->tl){struct _tuple6 _tmp200;void*_tmp201;void*_tmp202;
struct _tuple6*_tmp1FF=(struct _tuple6*)r2->hd;_tmp200=*_tmp1FF;_tmp201=_tmp200.f1;
_tmp202=_tmp200.f2;if(Cyc_Tcutil_unify(_tmp1FD,_tmp201)?Cyc_Tcutil_unify(_tmp1FE,
_tmp202): 0){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2)?Cyc_Tcutil_sub_rgnpo(rpo2,rpo1): 0;}struct _tuple11{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _tagged_arr)
_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)
return;{void*_tmp203=t1;struct Cyc_Core_Opt*_tmp204;struct Cyc_Core_Opt*_tmp205;
struct Cyc_Core_Opt**_tmp206;struct Cyc_Core_Opt*_tmp207;_LL1B3: if(_tmp203 <= (void*)
3?1:*((int*)_tmp203)!= 0)goto _LL1B5;_tmp204=((struct Cyc_Absyn_Evar_struct*)
_tmp203)->f1;_tmp205=((struct Cyc_Absyn_Evar_struct*)_tmp203)->f2;_tmp206=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp203)->f2;_tmp207=((struct Cyc_Absyn_Evar_struct*)
_tmp203)->f4;_LL1B4: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp207))->v,t2);{void*_tmp208=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp208,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp204))->v)){*_tmp206=({struct Cyc_Core_Opt*_tmp209=_cycalloc(sizeof(*_tmp209));
_tmp209->v=(void*)t2;_tmp209;});return;}else{{void*_tmp20A=t2;struct Cyc_Core_Opt*
_tmp20B;struct Cyc_Core_Opt**_tmp20C;struct Cyc_Core_Opt*_tmp20D;struct Cyc_Absyn_PtrInfo
_tmp20E;_LL1B8: if(_tmp20A <= (void*)3?1:*((int*)_tmp20A)!= 0)goto _LL1BA;_tmp20B=((
struct Cyc_Absyn_Evar_struct*)_tmp20A)->f2;_tmp20C=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp20A)->f2;_tmp20D=((struct Cyc_Absyn_Evar_struct*)
_tmp20A)->f4;_LL1B9: {struct Cyc_List_List*_tmp20F=(struct Cyc_List_List*)_tmp207->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp20D))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp20F,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=
_tag_arr("(type variable would escape scope)",sizeof(char),35);(int)_throw((void*)
Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)_tmp204->v,_tmp208)){*_tmp20C=({
struct Cyc_Core_Opt*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->v=(void*)t1;
_tmp210;});return;}Cyc_Tcutil_failure_reason=_tag_arr("(kinds are incompatible)",
sizeof(char),25);goto _LL1B7;}_LL1BA: if(_tmp20A <= (void*)3?1:*((int*)_tmp20A)!= 4)
goto _LL1BC;_tmp20E=((struct Cyc_Absyn_PointerType_struct*)_tmp20A)->f1;if(!((void*)
_tmp204->v == (void*)2))goto _LL1BC;_LL1BB: {struct Cyc_Absyn_Conref*_tmp211=Cyc_Absyn_compress_conref(
_tmp20E.bounds);{void*_tmp212=(void*)_tmp211->v;_LL1BF: if((int)_tmp212 != 0)goto
_LL1C1;_LL1C0:(void*)(_tmp211->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp214;_tmp214.tag=0;_tmp214.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp216;_tmp216.tag=0;_tmp216.f1=Cyc_Absyn_signed_int_exp(1,0);_tmp216;});
_tmp215;}));_tmp214;});_tmp213;})));*_tmp206=({struct Cyc_Core_Opt*_tmp217=
_cycalloc(sizeof(*_tmp217));_tmp217->v=(void*)t2;_tmp217;});return;_LL1C1:;
_LL1C2: goto _LL1BE;_LL1BE:;}goto _LL1B7;}_LL1BC:;_LL1BD: goto _LL1B7;_LL1B7:;}Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}
_LL1B5:;_LL1B6: goto _LL1B2;_LL1B2:;}{struct _tuple6 _tmp219=({struct _tuple6 _tmp218;
_tmp218.f1=t2;_tmp218.f2=t1;_tmp218;});void*_tmp21A;void*_tmp21B;void*_tmp21C;
void*_tmp21D;struct Cyc_Absyn_Tvar*_tmp21E;void*_tmp21F;struct Cyc_Absyn_Tvar*
_tmp220;void*_tmp221;struct Cyc_Absyn_AggrInfo _tmp222;void*_tmp223;struct Cyc_List_List*
_tmp224;void*_tmp225;struct Cyc_Absyn_AggrInfo _tmp226;void*_tmp227;struct Cyc_List_List*
_tmp228;void*_tmp229;struct _tuple1*_tmp22A;void*_tmp22B;struct _tuple1*_tmp22C;
void*_tmp22D;struct Cyc_List_List*_tmp22E;void*_tmp22F;struct Cyc_List_List*
_tmp230;void*_tmp231;struct Cyc_Absyn_TunionInfo _tmp232;void*_tmp233;struct Cyc_Absyn_Tuniondecl**
_tmp234;struct Cyc_Absyn_Tuniondecl*_tmp235;struct Cyc_List_List*_tmp236;void*
_tmp237;void*_tmp238;struct Cyc_Absyn_TunionInfo _tmp239;void*_tmp23A;struct Cyc_Absyn_Tuniondecl**
_tmp23B;struct Cyc_Absyn_Tuniondecl*_tmp23C;struct Cyc_List_List*_tmp23D;void*
_tmp23E;void*_tmp23F;struct Cyc_Absyn_TunionFieldInfo _tmp240;void*_tmp241;struct
Cyc_Absyn_Tuniondecl*_tmp242;struct Cyc_Absyn_Tunionfield*_tmp243;struct Cyc_List_List*
_tmp244;void*_tmp245;struct Cyc_Absyn_TunionFieldInfo _tmp246;void*_tmp247;struct
Cyc_Absyn_Tuniondecl*_tmp248;struct Cyc_Absyn_Tunionfield*_tmp249;struct Cyc_List_List*
_tmp24A;void*_tmp24B;struct Cyc_Absyn_PtrInfo _tmp24C;void*_tmp24D;void*_tmp24E;
struct Cyc_Absyn_Conref*_tmp24F;struct Cyc_Absyn_Tqual _tmp250;struct Cyc_Absyn_Conref*
_tmp251;void*_tmp252;struct Cyc_Absyn_PtrInfo _tmp253;void*_tmp254;void*_tmp255;
struct Cyc_Absyn_Conref*_tmp256;struct Cyc_Absyn_Tqual _tmp257;struct Cyc_Absyn_Conref*
_tmp258;void*_tmp259;void*_tmp25A;void*_tmp25B;void*_tmp25C;void*_tmp25D;void*
_tmp25E;void*_tmp25F;void*_tmp260;void*_tmp261;int _tmp262;void*_tmp263;int
_tmp264;void*_tmp265;void*_tmp266;void*_tmp267;void*_tmp268;void*_tmp269;int
_tmp26A;void*_tmp26B;int _tmp26C;void*_tmp26D;void*_tmp26E;void*_tmp26F;void*
_tmp270;void*_tmp271;void*_tmp272;struct Cyc_Absyn_Tqual _tmp273;struct Cyc_Absyn_Exp*
_tmp274;void*_tmp275;void*_tmp276;struct Cyc_Absyn_Tqual _tmp277;struct Cyc_Absyn_Exp*
_tmp278;void*_tmp279;struct Cyc_Absyn_FnInfo _tmp27A;struct Cyc_List_List*_tmp27B;
struct Cyc_Core_Opt*_tmp27C;void*_tmp27D;struct Cyc_List_List*_tmp27E;int _tmp27F;
struct Cyc_Absyn_VarargInfo*_tmp280;struct Cyc_List_List*_tmp281;struct Cyc_List_List*
_tmp282;void*_tmp283;struct Cyc_Absyn_FnInfo _tmp284;struct Cyc_List_List*_tmp285;
struct Cyc_Core_Opt*_tmp286;void*_tmp287;struct Cyc_List_List*_tmp288;int _tmp289;
struct Cyc_Absyn_VarargInfo*_tmp28A;struct Cyc_List_List*_tmp28B;struct Cyc_List_List*
_tmp28C;void*_tmp28D;struct Cyc_List_List*_tmp28E;void*_tmp28F;struct Cyc_List_List*
_tmp290;void*_tmp291;void*_tmp292;struct Cyc_List_List*_tmp293;void*_tmp294;void*
_tmp295;struct Cyc_List_List*_tmp296;void*_tmp297;void*_tmp298;void*_tmp299;void*
_tmp29A;void*_tmp29B;void*_tmp29C;void*_tmp29D;void*_tmp29E;void*_tmp29F;void*
_tmp2A0;void*_tmp2A1;void*_tmp2A2;_LL1C4: _tmp21A=_tmp219.f1;if(_tmp21A <= (void*)
3?1:*((int*)_tmp21A)!= 0)goto _LL1C6;_LL1C5: Cyc_Tcutil_unify_it(t2,t1);return;
_LL1C6: _tmp21B=_tmp219.f1;if((int)_tmp21B != 0)goto _LL1C8;_tmp21C=_tmp219.f2;if((
int)_tmp21C != 0)goto _LL1C8;_LL1C7: return;_LL1C8: _tmp21D=_tmp219.f1;if(_tmp21D <= (
void*)3?1:*((int*)_tmp21D)!= 1)goto _LL1CA;_tmp21E=((struct Cyc_Absyn_VarType_struct*)
_tmp21D)->f1;_tmp21F=_tmp219.f2;if(_tmp21F <= (void*)3?1:*((int*)_tmp21F)!= 1)
goto _LL1CA;_tmp220=((struct Cyc_Absyn_VarType_struct*)_tmp21F)->f1;_LL1C9: {
struct _tagged_arr*_tmp2A3=_tmp21E->name;struct _tagged_arr*_tmp2A4=_tmp220->name;
int _tmp2A5=*((int*)_check_null(_tmp21E->identity));int _tmp2A6=*((int*)
_check_null(_tmp220->identity));void*_tmp2A7=Cyc_Tcutil_tvar_kind(_tmp21E);void*
_tmp2A8=Cyc_Tcutil_tvar_kind(_tmp220);if(_tmp2A6 == _tmp2A5?Cyc_Std_zstrptrcmp(
_tmp2A3,_tmp2A4)== 0: 0){if(_tmp2A7 != _tmp2A8)({struct Cyc_Std_String_pa_struct
_tmp2AC;_tmp2AC.tag=0;_tmp2AC.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2A8);{struct Cyc_Std_String_pa_struct _tmp2AB;_tmp2AB.tag=0;_tmp2AB.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2A7);{struct Cyc_Std_String_pa_struct
_tmp2AA;_tmp2AA.tag=0;_tmp2AA.f1=(struct _tagged_arr)*_tmp2A3;{void*_tmp2A9[3]={&
_tmp2AA,& _tmp2AB,& _tmp2AC};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("same type variable %s has kinds %s and %s",sizeof(
char),42),_tag_arr(_tmp2A9,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=
_tag_arr("(variable types are not the same)",sizeof(char),34);goto _LL1C3;}_LL1CA:
_tmp221=_tmp219.f1;if(_tmp221 <= (void*)3?1:*((int*)_tmp221)!= 10)goto _LL1CC;
_tmp222=((struct Cyc_Absyn_AggrType_struct*)_tmp221)->f1;_tmp223=(void*)_tmp222.aggr_info;
_tmp224=_tmp222.targs;_tmp225=_tmp219.f2;if(_tmp225 <= (void*)3?1:*((int*)_tmp225)
!= 10)goto _LL1CC;_tmp226=((struct Cyc_Absyn_AggrType_struct*)_tmp225)->f1;_tmp227=(
void*)_tmp226.aggr_info;_tmp228=_tmp226.targs;_LL1CB: {void*_tmp2AE;struct
_tuple1*_tmp2AF;struct _tuple5 _tmp2AD=Cyc_Absyn_aggr_kinded_name(_tmp227);_tmp2AE=
_tmp2AD.f1;_tmp2AF=_tmp2AD.f2;{void*_tmp2B1;struct _tuple1*_tmp2B2;struct _tuple5
_tmp2B0=Cyc_Absyn_aggr_kinded_name(_tmp223);_tmp2B1=_tmp2B0.f1;_tmp2B2=_tmp2B0.f2;
if(_tmp2AE != _tmp2B1){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1C3;}if(Cyc_Absyn_qvar_cmp(_tmp2AF,_tmp2B2)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different type name)",sizeof(char),22);goto _LL1C3;}Cyc_Tcutil_unify_list(
_tmp228,_tmp224);return;}}_LL1CC: _tmp229=_tmp219.f1;if(_tmp229 <= (void*)3?1:*((
int*)_tmp229)!= 12)goto _LL1CE;_tmp22A=((struct Cyc_Absyn_EnumType_struct*)_tmp229)->f1;
_tmp22B=_tmp219.f2;if(_tmp22B <= (void*)3?1:*((int*)_tmp22B)!= 12)goto _LL1CE;
_tmp22C=((struct Cyc_Absyn_EnumType_struct*)_tmp22B)->f1;_LL1CD: if(Cyc_Absyn_qvar_cmp(
_tmp22A,_tmp22C)== 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different enum types)",
sizeof(char),23);goto _LL1C3;_LL1CE: _tmp22D=_tmp219.f1;if(_tmp22D <= (void*)3?1:*((
int*)_tmp22D)!= 13)goto _LL1D0;_tmp22E=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp22D)->f1;_tmp22F=_tmp219.f2;if(_tmp22F <= (void*)3?1:*((int*)_tmp22F)!= 13)
goto _LL1D0;_tmp230=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp22F)->f1;_LL1CF: {
int bad=0;for(0;_tmp22E != 0?_tmp230 != 0: 0;(_tmp22E=_tmp22E->tl,_tmp230=_tmp230->tl)){
struct Cyc_Absyn_Enumfield*_tmp2B3=(struct Cyc_Absyn_Enumfield*)_tmp22E->hd;struct
Cyc_Absyn_Enumfield*_tmp2B4=(struct Cyc_Absyn_Enumfield*)_tmp230->hd;if(Cyc_Absyn_qvar_cmp(
_tmp2B3->name,_tmp2B4->name)!= 0){Cyc_Tcutil_failure_reason=_tag_arr("(different names for enum fields)",
sizeof(char),34);bad=1;break;}if(_tmp2B3->tag == _tmp2B4->tag)continue;if(_tmp2B3->tag
== 0?1: _tmp2B4->tag == 0){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(char),39);bad=1;break;}if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp2B3->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp2B4->tag))){Cyc_Tcutil_failure_reason=
_tag_arr("(different tag values for enum fields)",sizeof(char),39);bad=1;break;}}
if(bad)goto _LL1C3;if(_tmp22E == 0?_tmp230 == 0: 0)return;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of fields for enums)",sizeof(char),39);goto _LL1C3;}
_LL1D0: _tmp231=_tmp219.f1;if(_tmp231 <= (void*)3?1:*((int*)_tmp231)!= 2)goto
_LL1D2;_tmp232=((struct Cyc_Absyn_TunionType_struct*)_tmp231)->f1;_tmp233=(void*)
_tmp232.tunion_info;if(*((int*)_tmp233)!= 1)goto _LL1D2;_tmp234=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp233)->f1;_tmp235=*_tmp234;_tmp236=_tmp232.targs;_tmp237=(void*)_tmp232.rgn;
_tmp238=_tmp219.f2;if(_tmp238 <= (void*)3?1:*((int*)_tmp238)!= 2)goto _LL1D2;
_tmp239=((struct Cyc_Absyn_TunionType_struct*)_tmp238)->f1;_tmp23A=(void*)_tmp239.tunion_info;
if(*((int*)_tmp23A)!= 1)goto _LL1D2;_tmp23B=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp23A)->f1;_tmp23C=*_tmp23B;_tmp23D=_tmp239.targs;_tmp23E=(void*)_tmp239.rgn;
_LL1D1: if(_tmp235 == _tmp23C?1: Cyc_Absyn_qvar_cmp(_tmp235->name,_tmp23C->name)== 
0){Cyc_Tcutil_unify_it(_tmp23E,_tmp237);Cyc_Tcutil_unify_list(_tmp23D,_tmp236);
return;}Cyc_Tcutil_failure_reason=_tag_arr("(different tunion types)",sizeof(
char),25);goto _LL1C3;_LL1D2: _tmp23F=_tmp219.f1;if(_tmp23F <= (void*)3?1:*((int*)
_tmp23F)!= 3)goto _LL1D4;_tmp240=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp23F)->f1;_tmp241=(void*)_tmp240.field_info;if(*((int*)_tmp241)!= 1)goto
_LL1D4;_tmp242=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp241)->f1;_tmp243=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp241)->f2;_tmp244=_tmp240.targs;
_tmp245=_tmp219.f2;if(_tmp245 <= (void*)3?1:*((int*)_tmp245)!= 3)goto _LL1D4;
_tmp246=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp245)->f1;_tmp247=(void*)
_tmp246.field_info;if(*((int*)_tmp247)!= 1)goto _LL1D4;_tmp248=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp247)->f1;_tmp249=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp247)->f2;
_tmp24A=_tmp246.targs;_LL1D3: if((_tmp242 == _tmp248?1: Cyc_Absyn_qvar_cmp(_tmp242->name,
_tmp248->name)== 0)?_tmp243 == _tmp249?1: Cyc_Absyn_qvar_cmp(_tmp243->name,_tmp249->name)
== 0: 0){Cyc_Tcutil_unify_list(_tmp24A,_tmp244);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)",sizeof(char),31);goto _LL1C3;_LL1D4:
_tmp24B=_tmp219.f1;if(_tmp24B <= (void*)3?1:*((int*)_tmp24B)!= 4)goto _LL1D6;
_tmp24C=((struct Cyc_Absyn_PointerType_struct*)_tmp24B)->f1;_tmp24D=(void*)
_tmp24C.elt_typ;_tmp24E=(void*)_tmp24C.rgn_typ;_tmp24F=_tmp24C.nullable;_tmp250=
_tmp24C.tq;_tmp251=_tmp24C.bounds;_tmp252=_tmp219.f2;if(_tmp252 <= (void*)3?1:*((
int*)_tmp252)!= 4)goto _LL1D6;_tmp253=((struct Cyc_Absyn_PointerType_struct*)
_tmp252)->f1;_tmp254=(void*)_tmp253.elt_typ;_tmp255=(void*)_tmp253.rgn_typ;
_tmp256=_tmp253.nullable;_tmp257=_tmp253.tq;_tmp258=_tmp253.bounds;_LL1D5: Cyc_Tcutil_unify_it(
_tmp254,_tmp24D);Cyc_Tcutil_unify_it(_tmp24E,_tmp255);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_unify_tqual(_tmp257,_tmp250);Cyc_Tcutil_unify_it_conrefs(
Cyc_Tcutil_boundscmp,_tmp258,_tmp251,_tag_arr("(different pointer bounds)",
sizeof(char),27));{void*_tmp2B5=(void*)(Cyc_Absyn_compress_conref(_tmp258))->v;
void*_tmp2B6;_LL1FD: if(_tmp2B5 <= (void*)1?1:*((int*)_tmp2B5)!= 0)goto _LL1FF;
_tmp2B6=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2B5)->f1;if((int)_tmp2B6
!= 0)goto _LL1FF;_LL1FE: return;_LL1FF:;_LL200: goto _LL1FC;_LL1FC:;}((void(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp256,_tmp24F,
_tag_arr("(different pointer types)",sizeof(char),26));return;_LL1D6: _tmp259=
_tmp219.f1;if(_tmp259 <= (void*)3?1:*((int*)_tmp259)!= 5)goto _LL1D8;_tmp25A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp259)->f1;_tmp25B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp259)->f2;_tmp25C=_tmp219.f2;if(_tmp25C <= (void*)3?1:*((int*)_tmp25C)!= 5)
goto _LL1D8;_tmp25D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp25C)->f1;_tmp25E=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp25C)->f2;_LL1D7: if(_tmp25D == _tmp25A?
_tmp25E == _tmp25B: 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different integral types)",
sizeof(char),27);goto _LL1C3;_LL1D8: _tmp25F=_tmp219.f1;if((int)_tmp25F != 1)goto
_LL1DA;_tmp260=_tmp219.f2;if((int)_tmp260 != 1)goto _LL1DA;_LL1D9: return;_LL1DA:
_tmp261=_tmp219.f1;if(_tmp261 <= (void*)3?1:*((int*)_tmp261)!= 6)goto _LL1DC;
_tmp262=((struct Cyc_Absyn_DoubleType_struct*)_tmp261)->f1;_tmp263=_tmp219.f2;if(
_tmp263 <= (void*)3?1:*((int*)_tmp263)!= 6)goto _LL1DC;_tmp264=((struct Cyc_Absyn_DoubleType_struct*)
_tmp263)->f1;_LL1DB: if(_tmp262 == _tmp264)return;goto _LL1C3;_LL1DC: _tmp265=
_tmp219.f1;if(_tmp265 <= (void*)3?1:*((int*)_tmp265)!= 14)goto _LL1DE;_tmp266=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp265)->f1;_tmp267=_tmp219.f2;if(
_tmp267 <= (void*)3?1:*((int*)_tmp267)!= 14)goto _LL1DE;_tmp268=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp267)->f1;_LL1DD: Cyc_Tcutil_unify_it(_tmp266,_tmp268);return;_LL1DE: _tmp269=
_tmp219.f1;if(_tmp269 <= (void*)3?1:*((int*)_tmp269)!= 18)goto _LL1E0;_tmp26A=((
struct Cyc_Absyn_TypeInt_struct*)_tmp269)->f1;_tmp26B=_tmp219.f2;if(_tmp26B <= (
void*)3?1:*((int*)_tmp26B)!= 18)goto _LL1E0;_tmp26C=((struct Cyc_Absyn_TypeInt_struct*)
_tmp26B)->f1;_LL1DF: if(_tmp26A == _tmp26C)return;Cyc_Tcutil_failure_reason=
_tag_arr("(different type integers)",sizeof(char),26);goto _LL1C3;_LL1E0: _tmp26D=
_tmp219.f1;if(_tmp26D <= (void*)3?1:*((int*)_tmp26D)!= 17)goto _LL1E2;_tmp26E=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp26D)->f1;_tmp26F=_tmp219.f2;if(
_tmp26F <= (void*)3?1:*((int*)_tmp26F)!= 17)goto _LL1E2;_tmp270=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp26F)->f1;_LL1E1: Cyc_Tcutil_unify_it(_tmp26E,_tmp270);return;_LL1E2: _tmp271=
_tmp219.f1;if(_tmp271 <= (void*)3?1:*((int*)_tmp271)!= 7)goto _LL1E4;_tmp272=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp271)->f1;_tmp273=((struct Cyc_Absyn_ArrayType_struct*)
_tmp271)->f2;_tmp274=((struct Cyc_Absyn_ArrayType_struct*)_tmp271)->f3;_tmp275=
_tmp219.f2;if(_tmp275 <= (void*)3?1:*((int*)_tmp275)!= 7)goto _LL1E4;_tmp276=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp275)->f1;_tmp277=((struct Cyc_Absyn_ArrayType_struct*)
_tmp275)->f2;_tmp278=((struct Cyc_Absyn_ArrayType_struct*)_tmp275)->f3;_LL1E3: Cyc_Tcutil_unify_tqual(
_tmp277,_tmp273);Cyc_Tcutil_unify_it(_tmp276,_tmp272);if(_tmp274 == _tmp278)
return;if(_tmp274 == 0?1: _tmp278 == 0)goto _LL1C3;if(Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp274),(struct Cyc_Absyn_Exp*)_check_null(
_tmp278)))return;Cyc_Tcutil_failure_reason=_tag_arr("(different array sizes)",
sizeof(char),24);goto _LL1C3;_LL1E4: _tmp279=_tmp219.f1;if(_tmp279 <= (void*)3?1:*((
int*)_tmp279)!= 8)goto _LL1E6;_tmp27A=((struct Cyc_Absyn_FnType_struct*)_tmp279)->f1;
_tmp27B=_tmp27A.tvars;_tmp27C=_tmp27A.effect;_tmp27D=(void*)_tmp27A.ret_typ;
_tmp27E=_tmp27A.args;_tmp27F=_tmp27A.c_varargs;_tmp280=_tmp27A.cyc_varargs;
_tmp281=_tmp27A.rgn_po;_tmp282=_tmp27A.attributes;_tmp283=_tmp219.f2;if(_tmp283
<= (void*)3?1:*((int*)_tmp283)!= 8)goto _LL1E6;_tmp284=((struct Cyc_Absyn_FnType_struct*)
_tmp283)->f1;_tmp285=_tmp284.tvars;_tmp286=_tmp284.effect;_tmp287=(void*)_tmp284.ret_typ;
_tmp288=_tmp284.args;_tmp289=_tmp284.c_varargs;_tmp28A=_tmp284.cyc_varargs;
_tmp28B=_tmp284.rgn_po;_tmp28C=_tmp284.attributes;_LL1E5: {int done=0;{struct
_RegionHandle _tmp2B7=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2B7;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp285 != 0){if(_tmp27B == 0){
Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too few type variables)",
sizeof(char),50);(int)_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp2B8=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp285->hd);void*_tmp2B9=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp27B->hd);if(_tmp2B8 != _tmp2B9){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp2BD;_tmp2BD.tag=0;_tmp2BD.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2B9);{struct Cyc_Std_String_pa_struct
_tmp2BC;_tmp2BC.tag=0;_tmp2BC.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2B8);{struct Cyc_Std_String_pa_struct _tmp2BB;_tmp2BB.tag=0;_tmp2BB.f1=(struct
_tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp285->hd);{void*
_tmp2BA[3]={& _tmp2BB,& _tmp2BC,& _tmp2BD};Cyc_Std_aprintf(_tag_arr("(type var %s has different kinds %s and %s)",
sizeof(char),44),_tag_arr(_tmp2BA,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp2BE=_region_malloc(rgn,sizeof(*_tmp2BE));_tmp2BE->hd=({
struct _tuple8*_tmp2BF=_region_malloc(rgn,sizeof(*_tmp2BF));_tmp2BF->f1=(struct
Cyc_Absyn_Tvar*)_tmp27B->hd;_tmp2BF->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0[0]=({struct Cyc_Absyn_VarType_struct
_tmp2C1;_tmp2C1.tag=1;_tmp2C1.f1=(struct Cyc_Absyn_Tvar*)_tmp285->hd;_tmp2C1;});
_tmp2C0;});_tmp2BF;});_tmp2BE->tl=inst;_tmp2BE;});_tmp285=_tmp285->tl;_tmp27B=
_tmp27B->tl;}}if(_tmp27B != 0){Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too many type variables)",
sizeof(char),51);_npop_handler(0);goto _LL1C3;}if(inst != 0){Cyc_Tcutil_unify_it((
void*)({struct Cyc_Absyn_FnType_struct*_tmp2C2=_cycalloc(sizeof(*_tmp2C2));
_tmp2C2[0]=({struct Cyc_Absyn_FnType_struct _tmp2C3;_tmp2C3.tag=8;_tmp2C3.f1=({
struct Cyc_Absyn_FnInfo _tmp2C4;_tmp2C4.tvars=0;_tmp2C4.effect=_tmp286;_tmp2C4.ret_typ=(
void*)_tmp287;_tmp2C4.args=_tmp288;_tmp2C4.c_varargs=_tmp289;_tmp2C4.cyc_varargs=
_tmp28A;_tmp2C4.rgn_po=_tmp28B;_tmp2C4.attributes=_tmp28C;_tmp2C4;});_tmp2C3;});
_tmp2C2;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp2C5=_cycalloc(sizeof(*_tmp2C5));_tmp2C5[0]=({struct Cyc_Absyn_FnType_struct
_tmp2C6;_tmp2C6.tag=8;_tmp2C6.f1=({struct Cyc_Absyn_FnInfo _tmp2C7;_tmp2C7.tvars=0;
_tmp2C7.effect=_tmp27C;_tmp2C7.ret_typ=(void*)_tmp27D;_tmp2C7.args=_tmp27E;
_tmp2C7.c_varargs=_tmp27F;_tmp2C7.cyc_varargs=_tmp280;_tmp2C7.rgn_po=_tmp281;
_tmp2C7.attributes=_tmp282;_tmp2C7;});_tmp2C6;});_tmp2C5;})));done=1;}};
_pop_region(rgn);}if(done)return;Cyc_Tcutil_unify_it(_tmp287,_tmp27D);for(0;
_tmp288 != 0?_tmp27E != 0: 0;(_tmp288=_tmp288->tl,_tmp27E=_tmp27E->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)_tmp288->hd)).f2,(*((struct _tuple2*)_tmp27E->hd)).f2);Cyc_Tcutil_unify_it((*((
struct _tuple2*)_tmp288->hd)).f3,(*((struct _tuple2*)_tmp27E->hd)).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp288 != 0?1: _tmp27E != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)",sizeof(char),52);
goto _LL1C3;}if(_tmp289 != _tmp27F){Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes C varargs)",
sizeof(char),41);goto _LL1C3;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp2C9=({
struct _tuple11 _tmp2C8;_tmp2C8.f1=_tmp28A;_tmp2C8.f2=_tmp280;_tmp2C8;});struct Cyc_Absyn_VarargInfo*
_tmp2CA;struct Cyc_Absyn_VarargInfo*_tmp2CB;struct Cyc_Absyn_VarargInfo*_tmp2CC;
struct Cyc_Absyn_VarargInfo*_tmp2CD;struct Cyc_Absyn_VarargInfo*_tmp2CE;struct Cyc_Absyn_VarargInfo
_tmp2CF;struct Cyc_Core_Opt*_tmp2D0;struct Cyc_Absyn_Tqual _tmp2D1;void*_tmp2D2;int
_tmp2D3;struct Cyc_Absyn_VarargInfo*_tmp2D4;struct Cyc_Absyn_VarargInfo _tmp2D5;
struct Cyc_Core_Opt*_tmp2D6;struct Cyc_Absyn_Tqual _tmp2D7;void*_tmp2D8;int _tmp2D9;
_LL202: _tmp2CA=_tmp2C9.f1;if(_tmp2CA != 0)goto _LL204;_tmp2CB=_tmp2C9.f2;if(
_tmp2CB != 0)goto _LL204;_LL203: goto _LL201;_LL204: _tmp2CC=_tmp2C9.f1;if(_tmp2CC != 
0)goto _LL206;_LL205: goto _LL207;_LL206: _tmp2CD=_tmp2C9.f2;if(_tmp2CD != 0)goto
_LL208;_LL207: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes varargs)",
sizeof(char),39);goto _LL201;_LL208: _tmp2CE=_tmp2C9.f1;if(_tmp2CE == 0)goto _LL201;
_tmp2CF=*_tmp2CE;_tmp2D0=_tmp2CF.name;_tmp2D1=_tmp2CF.tq;_tmp2D2=(void*)_tmp2CF.type;
_tmp2D3=_tmp2CF.inject;_tmp2D4=_tmp2C9.f2;if(_tmp2D4 == 0)goto _LL201;_tmp2D5=*
_tmp2D4;_tmp2D6=_tmp2D5.name;_tmp2D7=_tmp2D5.tq;_tmp2D8=(void*)_tmp2D5.type;
_tmp2D9=_tmp2D5.inject;_LL209: Cyc_Tcutil_unify_tqual(_tmp2D1,_tmp2D7);Cyc_Tcutil_unify_it(
_tmp2D2,_tmp2D8);if(_tmp2D3 != _tmp2D9){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type injects varargs)",sizeof(char),41);}goto _LL201;
_LL201:;}if(bad_cyc_vararg)goto _LL1C3;{int bad_effect=0;{struct _tuple12 _tmp2DB=({
struct _tuple12 _tmp2DA;_tmp2DA.f1=_tmp286;_tmp2DA.f2=_tmp27C;_tmp2DA;});struct Cyc_Core_Opt*
_tmp2DC;struct Cyc_Core_Opt*_tmp2DD;struct Cyc_Core_Opt*_tmp2DE;struct Cyc_Core_Opt*
_tmp2DF;_LL20B: _tmp2DC=_tmp2DB.f1;if(_tmp2DC != 0)goto _LL20D;_tmp2DD=_tmp2DB.f2;
if(_tmp2DD != 0)goto _LL20D;_LL20C: goto _LL20A;_LL20D: _tmp2DE=_tmp2DB.f1;if(_tmp2DE
!= 0)goto _LL20F;_LL20E: goto _LL210;_LL20F: _tmp2DF=_tmp2DB.f2;if(_tmp2DF != 0)goto
_LL211;_LL210: bad_effect=1;goto _LL20A;_LL211:;_LL212: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp286))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp27C))->v);goto _LL20A;_LL20A:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=_tag_arr("(function type effects do not unify)",
sizeof(char),37);goto _LL1C3;}if(!Cyc_Tcutil_same_atts(_tmp282,_tmp28C)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different attributes)",sizeof(char),43);goto
_LL1C3;}if(!Cyc_Tcutil_same_rgn_po(_tmp281,_tmp28B)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different region lifetime orderings)",sizeof(char),
58);goto _LL1C3;}return;}}}_LL1E6: _tmp28D=_tmp219.f1;if(_tmp28D <= (void*)3?1:*((
int*)_tmp28D)!= 9)goto _LL1E8;_tmp28E=((struct Cyc_Absyn_TupleType_struct*)_tmp28D)->f1;
_tmp28F=_tmp219.f2;if(_tmp28F <= (void*)3?1:*((int*)_tmp28F)!= 9)goto _LL1E8;
_tmp290=((struct Cyc_Absyn_TupleType_struct*)_tmp28F)->f1;_LL1E7: for(0;_tmp290 != 
0?_tmp28E != 0: 0;(_tmp290=_tmp290->tl,_tmp28E=_tmp28E->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)_tmp290->hd)).f1,(*((struct _tuple4*)_tmp28E->hd)).f1);Cyc_Tcutil_unify_it((*((
struct _tuple4*)_tmp290->hd)).f2,(*((struct _tuple4*)_tmp28E->hd)).f2);}if(_tmp290
== 0?_tmp28E == 0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(tuple types have different numbers of components)",sizeof(char),51);
goto _LL1C3;_LL1E8: _tmp291=_tmp219.f1;if(_tmp291 <= (void*)3?1:*((int*)_tmp291)!= 
11)goto _LL1EA;_tmp292=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp291)->f1;
_tmp293=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp291)->f2;_tmp294=_tmp219.f2;
if(_tmp294 <= (void*)3?1:*((int*)_tmp294)!= 11)goto _LL1EA;_tmp295=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp294)->f1;_tmp296=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp294)->f2;_LL1E9: if(_tmp295 != _tmp292){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1C3;}for(0;_tmp296 != 0?_tmp293 != 0: 0;(_tmp296=_tmp296->tl,
_tmp293=_tmp293->tl)){struct Cyc_Absyn_Aggrfield*_tmp2E0=(struct Cyc_Absyn_Aggrfield*)
_tmp296->hd;struct Cyc_Absyn_Aggrfield*_tmp2E1=(struct Cyc_Absyn_Aggrfield*)
_tmp293->hd;if(Cyc_Std_zstrptrcmp(_tmp2E0->name,_tmp2E1->name)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different member names)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual(_tmp2E0->tq,_tmp2E1->tq);Cyc_Tcutil_unify_it((void*)
_tmp2E0->type,(void*)_tmp2E1->type);if(!Cyc_Tcutil_same_atts(_tmp2E0->attributes,
_tmp2E1->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different attributes on member)",sizeof(char),33);(int)_throw((void*)
Cyc_Tcutil_Unify);}if(((_tmp2E0->width != 0?_tmp2E1->width == 0: 0)?1:(_tmp2E1->width
!= 0?_tmp2E0->width == 0: 0))?1:((_tmp2E0->width != 0?_tmp2E1->width != 0: 0)?!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2E0->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp2E1->width)): 0)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=_tag_arr("(different bitfield widths on member)",
sizeof(char),38);(int)_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp296 == 0?_tmp293 == 
0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of members)",sizeof(char),30);goto _LL1C3;_LL1EA:
_tmp297=_tmp219.f1;if((int)_tmp297 != 2)goto _LL1EC;_tmp298=_tmp219.f2;if((int)
_tmp298 != 2)goto _LL1EC;_LL1EB: return;_LL1EC: _tmp299=_tmp219.f1;if(_tmp299 <= (
void*)3?1:*((int*)_tmp299)!= 15)goto _LL1EE;_tmp29A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp299)->f1;_tmp29B=_tmp219.f2;if(_tmp29B <= (void*)3?1:*((int*)_tmp29B)!= 15)
goto _LL1EE;_tmp29C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp29B)->f1;
_LL1ED: Cyc_Tcutil_unify_it(_tmp29A,_tmp29C);return;_LL1EE: _tmp29D=_tmp219.f1;if(
_tmp29D <= (void*)3?1:*((int*)_tmp29D)!= 20)goto _LL1F0;_LL1EF: goto _LL1F1;_LL1F0:
_tmp29E=_tmp219.f2;if(_tmp29E <= (void*)3?1:*((int*)_tmp29E)!= 20)goto _LL1F2;
_LL1F1: goto _LL1F3;_LL1F2: _tmp29F=_tmp219.f1;if(_tmp29F <= (void*)3?1:*((int*)
_tmp29F)!= 19)goto _LL1F4;_LL1F3: goto _LL1F5;_LL1F4: _tmp2A0=_tmp219.f1;if(_tmp2A0
<= (void*)3?1:*((int*)_tmp2A0)!= 21)goto _LL1F6;_LL1F5: goto _LL1F7;_LL1F6: _tmp2A1=
_tmp219.f2;if(_tmp2A1 <= (void*)3?1:*((int*)_tmp2A1)!= 21)goto _LL1F8;_LL1F7: goto
_LL1F9;_LL1F8: _tmp2A2=_tmp219.f2;if(_tmp2A2 <= (void*)3?1:*((int*)_tmp2A2)!= 19)
goto _LL1FA;_LL1F9: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)",sizeof(char),22);goto _LL1C3;_LL1FA:;_LL1FB: goto
_LL1C3;_LL1C3:;}(int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_opt_cmp(int(*
cmp)(void*,void*),struct Cyc_Core_Opt*a1,struct Cyc_Core_Opt*a2){if(a1 == a2)return
0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0: 0)return 1;return cmp((void*)((
struct Cyc_Core_Opt*)_check_null(a1))->v,(void*)((struct Cyc_Core_Opt*)_check_null(
a2))->v);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == 
a2)return 0;if(a1 == 0?a2 != 0: 0)return - 1;if(a1 != 0?a2 == 0: 0)return 1;return cmp((
void*)_check_null(a1),(void*)_check_null(a2));}int Cyc_Tcutil_list_cmp(int(*cmp)(
void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)return 0;
for(0;l1 != 0?l2 != 0: 0;(l1=l1->tl,l2=l2->tl)){int _tmp2E2=cmp((void*)l1->hd,(void*)
l2->hd);if(_tmp2E2 != 0)return _tmp2E2;}if(l2 != 0)return - 1;if(l1 != 0)return 1;
return 0;}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){int _tmp2E3=(tq1.q_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int
_tmp2E4=(tq2.q_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp2E3,_tmp2E4);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp2E5=(void*)x->v;void*
_tmp2E6;_LL214: if((int)_tmp2E5 != 0)goto _LL216;_LL215: return - 1;_LL216: if(_tmp2E5
<= (void*)1?1:*((int*)_tmp2E5)!= 0)goto _LL218;_tmp2E6=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2E5)->f1;_LL217: {void*_tmp2E7=(void*)y->v;void*_tmp2E8;_LL21B: if((int)
_tmp2E7 != 0)goto _LL21D;_LL21C: return 1;_LL21D: if(_tmp2E7 <= (void*)1?1:*((int*)
_tmp2E7)!= 0)goto _LL21F;_tmp2E8=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2E7)->f1;_LL21E: return cmp(_tmp2E6,_tmp2E8);_LL21F: if(_tmp2E7 <= (void*)1?1:*((
int*)_tmp2E7)!= 1)goto _LL21A;_LL220:({void*_tmp2E9[0]={};Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)",sizeof(char),40),_tag_arr(
_tmp2E9,sizeof(void*),0));});_LL21A:;}_LL218: if(_tmp2E5 <= (void*)1?1:*((int*)
_tmp2E5)!= 1)goto _LL213;_LL219:({void*_tmp2EA[0]={};Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress",
sizeof(char),37),_tag_arr(_tmp2EA,sizeof(void*),0));});_LL213:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp2EC;struct Cyc_Absyn_Tqual
_tmp2ED;void*_tmp2EE;struct _tuple4*_tmp2EB=tqt1;_tmp2EC=*_tmp2EB;_tmp2ED=_tmp2EC.f1;
_tmp2EE=_tmp2EC.f2;{struct _tuple4 _tmp2F0;struct Cyc_Absyn_Tqual _tmp2F1;void*
_tmp2F2;struct _tuple4*_tmp2EF=tqt2;_tmp2F0=*_tmp2EF;_tmp2F1=_tmp2F0.f1;_tmp2F2=
_tmp2F0.f2;{int _tmp2F3=Cyc_Tcutil_tqual_cmp(_tmp2ED,_tmp2F1);if(_tmp2F3 != 0)
return _tmp2F3;return Cyc_Tcutil_typecmp(_tmp2EE,_tmp2F2);}}}static int Cyc_Tcutil_attribute_case_number(
void*att){void*_tmp2F4=att;_LL222: if(_tmp2F4 <= (void*)16?1:*((int*)_tmp2F4)!= 0)
goto _LL224;_LL223: return 0;_LL224: if((int)_tmp2F4 != 0)goto _LL226;_LL225: return 1;
_LL226: if((int)_tmp2F4 != 1)goto _LL228;_LL227: return 2;_LL228: if((int)_tmp2F4 != 2)
goto _LL22A;_LL229: return 3;_LL22A: if((int)_tmp2F4 != 3)goto _LL22C;_LL22B: return 4;
_LL22C: if((int)_tmp2F4 != 4)goto _LL22E;_LL22D: return 5;_LL22E: if(_tmp2F4 <= (void*)
16?1:*((int*)_tmp2F4)!= 1)goto _LL230;_LL22F: return 6;_LL230: if((int)_tmp2F4 != 5)
goto _LL232;_LL231: return 7;_LL232: if(_tmp2F4 <= (void*)16?1:*((int*)_tmp2F4)!= 2)
goto _LL234;_LL233: return 8;_LL234: if((int)_tmp2F4 != 6)goto _LL236;_LL235: return 9;
_LL236: if((int)_tmp2F4 != 7)goto _LL238;_LL237: return 10;_LL238: if((int)_tmp2F4 != 8)
goto _LL23A;_LL239: return 11;_LL23A: if((int)_tmp2F4 != 9)goto _LL23C;_LL23B: return 12;
_LL23C: if((int)_tmp2F4 != 10)goto _LL23E;_LL23D: return 13;_LL23E: if((int)_tmp2F4 != 
11)goto _LL240;_LL23F: return 14;_LL240: if((int)_tmp2F4 != 12)goto _LL242;_LL241:
return 15;_LL242: if((int)_tmp2F4 != 13)goto _LL244;_LL243: return 16;_LL244: if((int)
_tmp2F4 != 14)goto _LL246;_LL245: return 17;_LL246: if((int)_tmp2F4 != 15)goto _LL248;
_LL247: return 18;_LL248: if(_tmp2F4 <= (void*)16?1:*((int*)_tmp2F4)!= 3)goto _LL221;
_LL249: return 19;_LL221:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple6 _tmp2F6=({struct _tuple6 _tmp2F5;_tmp2F5.f1=att1;_tmp2F5.f2=att2;
_tmp2F5;});void*_tmp2F7;int _tmp2F8;void*_tmp2F9;int _tmp2FA;void*_tmp2FB;int
_tmp2FC;void*_tmp2FD;int _tmp2FE;void*_tmp2FF;struct _tagged_arr _tmp300;void*
_tmp301;struct _tagged_arr _tmp302;void*_tmp303;void*_tmp304;int _tmp305;int _tmp306;
void*_tmp307;void*_tmp308;int _tmp309;int _tmp30A;_LL24B: _tmp2F7=_tmp2F6.f1;if(
_tmp2F7 <= (void*)16?1:*((int*)_tmp2F7)!= 0)goto _LL24D;_tmp2F8=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp2F7)->f1;_tmp2F9=_tmp2F6.f2;if(_tmp2F9 <= (void*)16?1:*((int*)_tmp2F9)!= 0)
goto _LL24D;_tmp2FA=((struct Cyc_Absyn_Regparm_att_struct*)_tmp2F9)->f1;_LL24C:
_tmp2FC=_tmp2F8;_tmp2FE=_tmp2FA;goto _LL24E;_LL24D: _tmp2FB=_tmp2F6.f1;if(_tmp2FB
<= (void*)16?1:*((int*)_tmp2FB)!= 1)goto _LL24F;_tmp2FC=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp2FB)->f1;_tmp2FD=_tmp2F6.f2;if(_tmp2FD <= (void*)16?1:*((int*)_tmp2FD)!= 1)
goto _LL24F;_tmp2FE=((struct Cyc_Absyn_Aligned_att_struct*)_tmp2FD)->f1;_LL24E:
return Cyc_Core_intcmp(_tmp2FC,_tmp2FE);_LL24F: _tmp2FF=_tmp2F6.f1;if(_tmp2FF <= (
void*)16?1:*((int*)_tmp2FF)!= 2)goto _LL251;_tmp300=((struct Cyc_Absyn_Section_att_struct*)
_tmp2FF)->f1;_tmp301=_tmp2F6.f2;if(_tmp301 <= (void*)16?1:*((int*)_tmp301)!= 2)
goto _LL251;_tmp302=((struct Cyc_Absyn_Section_att_struct*)_tmp301)->f1;_LL250:
return Cyc_Std_strcmp(_tmp300,_tmp302);_LL251: _tmp303=_tmp2F6.f1;if(_tmp303 <= (
void*)16?1:*((int*)_tmp303)!= 3)goto _LL253;_tmp304=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp303)->f1;_tmp305=((struct Cyc_Absyn_Format_att_struct*)_tmp303)->f2;_tmp306=((
struct Cyc_Absyn_Format_att_struct*)_tmp303)->f3;_tmp307=_tmp2F6.f2;if(_tmp307 <= (
void*)16?1:*((int*)_tmp307)!= 3)goto _LL253;_tmp308=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp307)->f1;_tmp309=((struct Cyc_Absyn_Format_att_struct*)_tmp307)->f2;_tmp30A=((
struct Cyc_Absyn_Format_att_struct*)_tmp307)->f3;_LL252: {int _tmp30B=Cyc_Core_intcmp((
int)((unsigned int)_tmp304),(int)((unsigned int)_tmp308));if(_tmp30B != 0)return
_tmp30B;{int _tmp30C=Cyc_Core_intcmp(_tmp305,_tmp309);if(_tmp30C != 0)return
_tmp30C;return Cyc_Core_intcmp(_tmp306,_tmp30A);}}_LL253:;_LL254: return Cyc_Core_intcmp(
Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));
_LL24A:;}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct
Cyc_Absyn_Aggrfield*f2){int _tmp30D=Cyc_Std_zstrptrcmp(f1->name,f2->name);if(
_tmp30D != 0)return _tmp30D;{int _tmp30E=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp30E != 0)return _tmp30E;{int _tmp30F=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp30F != 0)return _tmp30F;{int _tmp310=Cyc_Tcutil_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp310 != 0)return _tmp310;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);}}}}static
int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp311=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp311 != 0)return
_tmp311;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t){void*_tmp312=t;
_LL256: if((int)_tmp312 != 0)goto _LL258;_LL257: return 0;_LL258: if(_tmp312 <= (void*)
3?1:*((int*)_tmp312)!= 0)goto _LL25A;_LL259: return 1;_LL25A: if(_tmp312 <= (void*)3?
1:*((int*)_tmp312)!= 1)goto _LL25C;_LL25B: return 2;_LL25C: if(_tmp312 <= (void*)3?1:*((
int*)_tmp312)!= 2)goto _LL25E;_LL25D: return 3;_LL25E: if(_tmp312 <= (void*)3?1:*((
int*)_tmp312)!= 3)goto _LL260;_LL25F: return 4;_LL260: if(_tmp312 <= (void*)3?1:*((
int*)_tmp312)!= 4)goto _LL262;_LL261: return 5;_LL262: if(_tmp312 <= (void*)3?1:*((
int*)_tmp312)!= 5)goto _LL264;_LL263: return 6;_LL264: if((int)_tmp312 != 1)goto
_LL266;_LL265: return 7;_LL266: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 6)goto
_LL268;_LL267: return 8;_LL268: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 7)goto
_LL26A;_LL269: return 9;_LL26A: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 8)goto
_LL26C;_LL26B: return 10;_LL26C: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 9)goto
_LL26E;_LL26D: return 11;_LL26E: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 10)goto
_LL270;_LL26F: return 12;_LL270: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 11)goto
_LL272;_LL271: return 14;_LL272: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 12)goto
_LL274;_LL273: return 16;_LL274: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 13)goto
_LL276;_LL275: return 17;_LL276: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 15)goto
_LL278;_LL277: return 18;_LL278: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 16)goto
_LL27A;_LL279: return 19;_LL27A: if((int)_tmp312 != 2)goto _LL27C;_LL27B: return 20;
_LL27C: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 19)goto _LL27E;_LL27D: return 21;
_LL27E: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 20)goto _LL280;_LL27F: return 22;
_LL280: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 21)goto _LL282;_LL281: return 23;
_LL282: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 14)goto _LL284;_LL283: return 24;
_LL284: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 18)goto _LL286;_LL285: return 25;
_LL286: if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 17)goto _LL255;_LL287: return 26;
_LL255:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp313=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(
t1),Cyc_Tcutil_type_case_number(t2));if(_tmp313 != 0)return _tmp313;{struct _tuple6
_tmp315=({struct _tuple6 _tmp314;_tmp314.f1=t2;_tmp314.f2=t1;_tmp314;});void*
_tmp316;void*_tmp317;void*_tmp318;void*_tmp319;void*_tmp31A;struct Cyc_Absyn_Tvar*
_tmp31B;void*_tmp31C;struct Cyc_Absyn_Tvar*_tmp31D;void*_tmp31E;struct Cyc_Absyn_AggrInfo
_tmp31F;void*_tmp320;struct Cyc_List_List*_tmp321;void*_tmp322;struct Cyc_Absyn_AggrInfo
_tmp323;void*_tmp324;struct Cyc_List_List*_tmp325;void*_tmp326;struct _tuple1*
_tmp327;void*_tmp328;struct _tuple1*_tmp329;void*_tmp32A;struct Cyc_List_List*
_tmp32B;void*_tmp32C;struct Cyc_List_List*_tmp32D;void*_tmp32E;struct Cyc_Absyn_TunionInfo
_tmp32F;void*_tmp330;struct Cyc_Absyn_Tuniondecl**_tmp331;struct Cyc_Absyn_Tuniondecl*
_tmp332;struct Cyc_List_List*_tmp333;void*_tmp334;void*_tmp335;struct Cyc_Absyn_TunionInfo
_tmp336;void*_tmp337;struct Cyc_Absyn_Tuniondecl**_tmp338;struct Cyc_Absyn_Tuniondecl*
_tmp339;struct Cyc_List_List*_tmp33A;void*_tmp33B;void*_tmp33C;struct Cyc_Absyn_TunionFieldInfo
_tmp33D;void*_tmp33E;struct Cyc_Absyn_Tuniondecl*_tmp33F;struct Cyc_Absyn_Tunionfield*
_tmp340;struct Cyc_List_List*_tmp341;void*_tmp342;struct Cyc_Absyn_TunionFieldInfo
_tmp343;void*_tmp344;struct Cyc_Absyn_Tuniondecl*_tmp345;struct Cyc_Absyn_Tunionfield*
_tmp346;struct Cyc_List_List*_tmp347;void*_tmp348;struct Cyc_Absyn_PtrInfo _tmp349;
void*_tmp34A;void*_tmp34B;struct Cyc_Absyn_Conref*_tmp34C;struct Cyc_Absyn_Tqual
_tmp34D;struct Cyc_Absyn_Conref*_tmp34E;void*_tmp34F;struct Cyc_Absyn_PtrInfo
_tmp350;void*_tmp351;void*_tmp352;struct Cyc_Absyn_Conref*_tmp353;struct Cyc_Absyn_Tqual
_tmp354;struct Cyc_Absyn_Conref*_tmp355;void*_tmp356;void*_tmp357;void*_tmp358;
void*_tmp359;void*_tmp35A;void*_tmp35B;void*_tmp35C;void*_tmp35D;void*_tmp35E;
int _tmp35F;void*_tmp360;int _tmp361;void*_tmp362;void*_tmp363;struct Cyc_Absyn_Tqual
_tmp364;struct Cyc_Absyn_Exp*_tmp365;void*_tmp366;void*_tmp367;struct Cyc_Absyn_Tqual
_tmp368;struct Cyc_Absyn_Exp*_tmp369;void*_tmp36A;struct Cyc_Absyn_FnInfo _tmp36B;
struct Cyc_List_List*_tmp36C;struct Cyc_Core_Opt*_tmp36D;void*_tmp36E;struct Cyc_List_List*
_tmp36F;int _tmp370;struct Cyc_Absyn_VarargInfo*_tmp371;struct Cyc_List_List*
_tmp372;struct Cyc_List_List*_tmp373;void*_tmp374;struct Cyc_Absyn_FnInfo _tmp375;
struct Cyc_List_List*_tmp376;struct Cyc_Core_Opt*_tmp377;void*_tmp378;struct Cyc_List_List*
_tmp379;int _tmp37A;struct Cyc_Absyn_VarargInfo*_tmp37B;struct Cyc_List_List*
_tmp37C;struct Cyc_List_List*_tmp37D;void*_tmp37E;struct Cyc_List_List*_tmp37F;
void*_tmp380;struct Cyc_List_List*_tmp381;void*_tmp382;void*_tmp383;struct Cyc_List_List*
_tmp384;void*_tmp385;void*_tmp386;struct Cyc_List_List*_tmp387;void*_tmp388;void*
_tmp389;void*_tmp38A;void*_tmp38B;void*_tmp38C;void*_tmp38D;void*_tmp38E;void*
_tmp38F;void*_tmp390;void*_tmp391;void*_tmp392;void*_tmp393;void*_tmp394;void*
_tmp395;void*_tmp396;int _tmp397;void*_tmp398;int _tmp399;void*_tmp39A;void*
_tmp39B;void*_tmp39C;void*_tmp39D;void*_tmp39E;void*_tmp39F;_LL289: _tmp316=
_tmp315.f1;if(_tmp316 <= (void*)3?1:*((int*)_tmp316)!= 0)goto _LL28B;_tmp317=
_tmp315.f2;if(_tmp317 <= (void*)3?1:*((int*)_tmp317)!= 0)goto _LL28B;_LL28A:({void*
_tmp3A0[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typecmp: can only compare closed types",sizeof(char),39),_tag_arr(
_tmp3A0,sizeof(void*),0));});_LL28B: _tmp318=_tmp315.f1;if((int)_tmp318 != 0)goto
_LL28D;_tmp319=_tmp315.f2;if((int)_tmp319 != 0)goto _LL28D;_LL28C: return 0;_LL28D:
_tmp31A=_tmp315.f1;if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 1)goto _LL28F;
_tmp31B=((struct Cyc_Absyn_VarType_struct*)_tmp31A)->f1;_tmp31C=_tmp315.f2;if(
_tmp31C <= (void*)3?1:*((int*)_tmp31C)!= 1)goto _LL28F;_tmp31D=((struct Cyc_Absyn_VarType_struct*)
_tmp31C)->f1;_LL28E: return Cyc_Core_intcmp(*((int*)_check_null(_tmp31D->identity)),*((
int*)_check_null(_tmp31B->identity)));_LL28F: _tmp31E=_tmp315.f1;if(_tmp31E <= (
void*)3?1:*((int*)_tmp31E)!= 10)goto _LL291;_tmp31F=((struct Cyc_Absyn_AggrType_struct*)
_tmp31E)->f1;_tmp320=(void*)_tmp31F.aggr_info;_tmp321=_tmp31F.targs;_tmp322=
_tmp315.f2;if(_tmp322 <= (void*)3?1:*((int*)_tmp322)!= 10)goto _LL291;_tmp323=((
struct Cyc_Absyn_AggrType_struct*)_tmp322)->f1;_tmp324=(void*)_tmp323.aggr_info;
_tmp325=_tmp323.targs;_LL290: {struct _tuple1*_tmp3A2;struct _tuple5 _tmp3A1=Cyc_Absyn_aggr_kinded_name(
_tmp320);_tmp3A2=_tmp3A1.f2;{struct _tuple1*_tmp3A4;struct _tuple5 _tmp3A3=Cyc_Absyn_aggr_kinded_name(
_tmp324);_tmp3A4=_tmp3A3.f2;{int _tmp3A5=Cyc_Absyn_qvar_cmp(_tmp3A2,_tmp3A4);if(
_tmp3A5 != 0)return _tmp3A5;else{return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp321,_tmp325);}}}}_LL291: _tmp326=_tmp315.f1;if(_tmp326 <= (void*)3?1:*((int*)
_tmp326)!= 12)goto _LL293;_tmp327=((struct Cyc_Absyn_EnumType_struct*)_tmp326)->f1;
_tmp328=_tmp315.f2;if(_tmp328 <= (void*)3?1:*((int*)_tmp328)!= 12)goto _LL293;
_tmp329=((struct Cyc_Absyn_EnumType_struct*)_tmp328)->f1;_LL292: return Cyc_Absyn_qvar_cmp(
_tmp327,_tmp329);_LL293: _tmp32A=_tmp315.f1;if(_tmp32A <= (void*)3?1:*((int*)
_tmp32A)!= 13)goto _LL295;_tmp32B=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp32A)->f1;
_tmp32C=_tmp315.f2;if(_tmp32C <= (void*)3?1:*((int*)_tmp32C)!= 13)goto _LL295;
_tmp32D=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp32C)->f1;_LL294: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp32B,
_tmp32D);_LL295: _tmp32E=_tmp315.f1;if(_tmp32E <= (void*)3?1:*((int*)_tmp32E)!= 2)
goto _LL297;_tmp32F=((struct Cyc_Absyn_TunionType_struct*)_tmp32E)->f1;_tmp330=(
void*)_tmp32F.tunion_info;if(*((int*)_tmp330)!= 1)goto _LL297;_tmp331=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp330)->f1;_tmp332=*_tmp331;_tmp333=_tmp32F.targs;_tmp334=(void*)_tmp32F.rgn;
_tmp335=_tmp315.f2;if(_tmp335 <= (void*)3?1:*((int*)_tmp335)!= 2)goto _LL297;
_tmp336=((struct Cyc_Absyn_TunionType_struct*)_tmp335)->f1;_tmp337=(void*)_tmp336.tunion_info;
if(*((int*)_tmp337)!= 1)goto _LL297;_tmp338=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp337)->f1;_tmp339=*_tmp338;_tmp33A=_tmp336.targs;_tmp33B=(void*)_tmp336.rgn;
_LL296: if(_tmp339 == _tmp332)return 0;{int _tmp3A6=Cyc_Absyn_qvar_cmp(_tmp339->name,
_tmp332->name);if(_tmp3A6 != 0)return _tmp3A6;{int _tmp3A7=Cyc_Tcutil_typecmp(
_tmp33B,_tmp334);if(_tmp3A7 != 0)return _tmp3A7;return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp33A,_tmp333);}}_LL297: _tmp33C=_tmp315.f1;if(_tmp33C <= (void*)3?1:*((int*)
_tmp33C)!= 3)goto _LL299;_tmp33D=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp33C)->f1;_tmp33E=(void*)_tmp33D.field_info;if(*((int*)_tmp33E)!= 1)goto
_LL299;_tmp33F=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp33E)->f1;_tmp340=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp33E)->f2;_tmp341=_tmp33D.targs;
_tmp342=_tmp315.f2;if(_tmp342 <= (void*)3?1:*((int*)_tmp342)!= 3)goto _LL299;
_tmp343=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp342)->f1;_tmp344=(void*)
_tmp343.field_info;if(*((int*)_tmp344)!= 1)goto _LL299;_tmp345=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp344)->f1;_tmp346=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp344)->f2;
_tmp347=_tmp343.targs;_LL298: if(_tmp345 == _tmp33F)return 0;{int _tmp3A8=Cyc_Absyn_qvar_cmp(
_tmp33F->name,_tmp345->name);if(_tmp3A8 != 0)return _tmp3A8;{int _tmp3A9=Cyc_Absyn_qvar_cmp(
_tmp340->name,_tmp346->name);if(_tmp3A9 != 0)return _tmp3A9;return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp347,_tmp341);}}_LL299: _tmp348=_tmp315.f1;if(_tmp348 <= (
void*)3?1:*((int*)_tmp348)!= 4)goto _LL29B;_tmp349=((struct Cyc_Absyn_PointerType_struct*)
_tmp348)->f1;_tmp34A=(void*)_tmp349.elt_typ;_tmp34B=(void*)_tmp349.rgn_typ;
_tmp34C=_tmp349.nullable;_tmp34D=_tmp349.tq;_tmp34E=_tmp349.bounds;_tmp34F=
_tmp315.f2;if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 4)goto _LL29B;_tmp350=((
struct Cyc_Absyn_PointerType_struct*)_tmp34F)->f1;_tmp351=(void*)_tmp350.elt_typ;
_tmp352=(void*)_tmp350.rgn_typ;_tmp353=_tmp350.nullable;_tmp354=_tmp350.tq;
_tmp355=_tmp350.bounds;_LL29A: {int _tmp3AA=Cyc_Tcutil_typecmp(_tmp351,_tmp34A);
if(_tmp3AA != 0)return _tmp3AA;{int _tmp3AB=Cyc_Tcutil_typecmp(_tmp352,_tmp34B);if(
_tmp3AB != 0)return _tmp3AB;{int _tmp3AC=Cyc_Tcutil_tqual_cmp(_tmp354,_tmp34D);if(
_tmp3AC != 0)return _tmp3AC;{int _tmp3AD=Cyc_Tcutil_conrefs_cmp(Cyc_Tcutil_boundscmp,
_tmp355,_tmp34E);if(_tmp3AD != 0)return _tmp3AD;{void*_tmp3AE=(void*)(Cyc_Absyn_compress_conref(
_tmp355))->v;void*_tmp3AF;_LL2C2: if(_tmp3AE <= (void*)1?1:*((int*)_tmp3AE)!= 0)
goto _LL2C4;_tmp3AF=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp3AE)->f1;if((
int)_tmp3AF != 0)goto _LL2C4;_LL2C3: return 0;_LL2C4:;_LL2C5: goto _LL2C1;_LL2C1:;}
return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp353,_tmp34C);}}}}_LL29B: _tmp356=
_tmp315.f1;if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 5)goto _LL29D;_tmp357=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp356)->f1;_tmp358=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp356)->f2;_tmp359=_tmp315.f2;if(_tmp359 <= (void*)3?1:*((int*)_tmp359)!= 5)
goto _LL29D;_tmp35A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp359)->f1;_tmp35B=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp359)->f2;_LL29C: if(_tmp35A != _tmp357)
return Cyc_Core_intcmp((int)((unsigned int)_tmp35A),(int)((unsigned int)_tmp357));
if(_tmp35B != _tmp358)return Cyc_Core_intcmp((int)((unsigned int)_tmp35B),(int)((
unsigned int)_tmp358));return 0;_LL29D: _tmp35C=_tmp315.f1;if((int)_tmp35C != 1)
goto _LL29F;_tmp35D=_tmp315.f2;if((int)_tmp35D != 1)goto _LL29F;_LL29E: return 0;
_LL29F: _tmp35E=_tmp315.f1;if(_tmp35E <= (void*)3?1:*((int*)_tmp35E)!= 6)goto
_LL2A1;_tmp35F=((struct Cyc_Absyn_DoubleType_struct*)_tmp35E)->f1;_tmp360=_tmp315.f2;
if(_tmp360 <= (void*)3?1:*((int*)_tmp360)!= 6)goto _LL2A1;_tmp361=((struct Cyc_Absyn_DoubleType_struct*)
_tmp360)->f1;_LL2A0: if(_tmp35F == _tmp361)return 0;else{if(_tmp35F)return - 1;else{
return 1;}}_LL2A1: _tmp362=_tmp315.f1;if(_tmp362 <= (void*)3?1:*((int*)_tmp362)!= 7)
goto _LL2A3;_tmp363=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp362)->f1;
_tmp364=((struct Cyc_Absyn_ArrayType_struct*)_tmp362)->f2;_tmp365=((struct Cyc_Absyn_ArrayType_struct*)
_tmp362)->f3;_tmp366=_tmp315.f2;if(_tmp366 <= (void*)3?1:*((int*)_tmp366)!= 7)
goto _LL2A3;_tmp367=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp366)->f1;
_tmp368=((struct Cyc_Absyn_ArrayType_struct*)_tmp366)->f2;_tmp369=((struct Cyc_Absyn_ArrayType_struct*)
_tmp366)->f3;_LL2A2: {int _tmp3B0=Cyc_Tcutil_tqual_cmp(_tmp368,_tmp364);if(
_tmp3B0 != 0)return _tmp3B0;{int _tmp3B1=Cyc_Tcutil_typecmp(_tmp367,_tmp363);if(
_tmp3B1 != 0)return _tmp3B1;if(_tmp365 == _tmp369)return 0;if(_tmp365 == 0?1: _tmp369
== 0)({void*_tmp3B2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("missing expression in array index",sizeof(char),34),_tag_arr(_tmp3B2,
sizeof(void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp365,_tmp369);}}_LL2A3: _tmp36A=_tmp315.f1;if(_tmp36A <= (void*)3?1:*((int*)
_tmp36A)!= 8)goto _LL2A5;_tmp36B=((struct Cyc_Absyn_FnType_struct*)_tmp36A)->f1;
_tmp36C=_tmp36B.tvars;_tmp36D=_tmp36B.effect;_tmp36E=(void*)_tmp36B.ret_typ;
_tmp36F=_tmp36B.args;_tmp370=_tmp36B.c_varargs;_tmp371=_tmp36B.cyc_varargs;
_tmp372=_tmp36B.rgn_po;_tmp373=_tmp36B.attributes;_tmp374=_tmp315.f2;if(_tmp374
<= (void*)3?1:*((int*)_tmp374)!= 8)goto _LL2A5;_tmp375=((struct Cyc_Absyn_FnType_struct*)
_tmp374)->f1;_tmp376=_tmp375.tvars;_tmp377=_tmp375.effect;_tmp378=(void*)_tmp375.ret_typ;
_tmp379=_tmp375.args;_tmp37A=_tmp375.c_varargs;_tmp37B=_tmp375.cyc_varargs;
_tmp37C=_tmp375.rgn_po;_tmp37D=_tmp375.attributes;_LL2A4:({void*_tmp3B3[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: function types not handled",
sizeof(char),36),_tag_arr(_tmp3B3,sizeof(void*),0));});_LL2A5: _tmp37E=_tmp315.f1;
if(_tmp37E <= (void*)3?1:*((int*)_tmp37E)!= 9)goto _LL2A7;_tmp37F=((struct Cyc_Absyn_TupleType_struct*)
_tmp37E)->f1;_tmp380=_tmp315.f2;if(_tmp380 <= (void*)3?1:*((int*)_tmp380)!= 9)
goto _LL2A7;_tmp381=((struct Cyc_Absyn_TupleType_struct*)_tmp380)->f1;_LL2A6:
return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp381,
_tmp37F);_LL2A7: _tmp382=_tmp315.f1;if(_tmp382 <= (void*)3?1:*((int*)_tmp382)!= 11)
goto _LL2A9;_tmp383=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp382)->f1;
_tmp384=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp382)->f2;_tmp385=_tmp315.f2;
if(_tmp385 <= (void*)3?1:*((int*)_tmp385)!= 11)goto _LL2A9;_tmp386=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp385)->f1;_tmp387=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp385)->f2;_LL2A8: if(_tmp386 != _tmp383){if(_tmp386 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp387,_tmp384);_LL2A9: _tmp388=_tmp315.f1;if((int)_tmp388 != 2)goto _LL2AB;
_tmp389=_tmp315.f2;if((int)_tmp389 != 2)goto _LL2AB;_LL2AA: return 0;_LL2AB: _tmp38A=
_tmp315.f1;if(_tmp38A <= (void*)3?1:*((int*)_tmp38A)!= 15)goto _LL2AD;_tmp38B=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp38A)->f1;_tmp38C=_tmp315.f2;
if(_tmp38C <= (void*)3?1:*((int*)_tmp38C)!= 15)goto _LL2AD;_tmp38D=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp38C)->f1;_LL2AC: return Cyc_Tcutil_typecmp(
_tmp38B,_tmp38D);_LL2AD: _tmp38E=_tmp315.f1;if(_tmp38E <= (void*)3?1:*((int*)
_tmp38E)!= 14)goto _LL2AF;_tmp38F=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp38E)->f1;_tmp390=_tmp315.f2;if(_tmp390 <= (void*)3?1:*((int*)_tmp390)!= 14)
goto _LL2AF;_tmp391=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp390)->f1;
_LL2AE: return Cyc_Tcutil_typecmp(_tmp38F,_tmp391);_LL2AF: _tmp392=_tmp315.f1;if(
_tmp392 <= (void*)3?1:*((int*)_tmp392)!= 17)goto _LL2B1;_tmp393=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp392)->f1;_tmp394=_tmp315.f2;if(_tmp394 <= (void*)3?1:*((int*)_tmp394)!= 17)
goto _LL2B1;_tmp395=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp394)->f1;_LL2B0:
return Cyc_Tcutil_typecmp(_tmp393,_tmp395);_LL2B1: _tmp396=_tmp315.f1;if(_tmp396 <= (
void*)3?1:*((int*)_tmp396)!= 18)goto _LL2B3;_tmp397=((struct Cyc_Absyn_TypeInt_struct*)
_tmp396)->f1;_tmp398=_tmp315.f2;if(_tmp398 <= (void*)3?1:*((int*)_tmp398)!= 18)
goto _LL2B3;_tmp399=((struct Cyc_Absyn_TypeInt_struct*)_tmp398)->f1;_LL2B2: return
Cyc_Core_intcmp(_tmp397,_tmp399);_LL2B3: _tmp39A=_tmp315.f1;if(_tmp39A <= (void*)3?
1:*((int*)_tmp39A)!= 20)goto _LL2B5;_LL2B4: goto _LL2B6;_LL2B5: _tmp39B=_tmp315.f2;
if(_tmp39B <= (void*)3?1:*((int*)_tmp39B)!= 20)goto _LL2B7;_LL2B6: goto _LL2B8;
_LL2B7: _tmp39C=_tmp315.f1;if(_tmp39C <= (void*)3?1:*((int*)_tmp39C)!= 19)goto
_LL2B9;_LL2B8: goto _LL2BA;_LL2B9: _tmp39D=_tmp315.f1;if(_tmp39D <= (void*)3?1:*((
int*)_tmp39D)!= 21)goto _LL2BB;_LL2BA: goto _LL2BC;_LL2BB: _tmp39E=_tmp315.f2;if(
_tmp39E <= (void*)3?1:*((int*)_tmp39E)!= 21)goto _LL2BD;_LL2BC: goto _LL2BE;_LL2BD:
_tmp39F=_tmp315.f2;if(_tmp39F <= (void*)3?1:*((int*)_tmp39F)!= 19)goto _LL2BF;
_LL2BE:({void*_tmp3B4[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("typecmp: effects not handled",sizeof(char),29),
_tag_arr(_tmp3B4,sizeof(void*),0));});_LL2BF:;_LL2C0:({void*_tmp3B5[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Unmatched case in typecmp",
sizeof(char),26),_tag_arr(_tmp3B5,sizeof(void*),0));});_LL288:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp3B6=Cyc_Tcutil_compress(t);_LL2C7: if(_tmp3B6 <= (void*)3?1:*((
int*)_tmp3B6)!= 5)goto _LL2C9;_LL2C8: goto _LL2CA;_LL2C9: if((int)_tmp3B6 != 1)goto
_LL2CB;_LL2CA: goto _LL2CC;_LL2CB: if(_tmp3B6 <= (void*)3?1:*((int*)_tmp3B6)!= 6)
goto _LL2CD;_LL2CC: goto _LL2CE;_LL2CD: if(_tmp3B6 <= (void*)3?1:*((int*)_tmp3B6)!= 
12)goto _LL2CF;_LL2CE: goto _LL2D0;_LL2CF: if(_tmp3B6 <= (void*)3?1:*((int*)_tmp3B6)
!= 13)goto _LL2D1;_LL2D0: return 1;_LL2D1:;_LL2D2: return 0;_LL2C6:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp3B8=({struct _tuple6 _tmp3B7;_tmp3B7.f1=Cyc_Tcutil_compress(
t1);_tmp3B7.f2=Cyc_Tcutil_compress(t2);_tmp3B7;});void*_tmp3B9;int _tmp3BA;void*
_tmp3BB;int _tmp3BC;void*_tmp3BD;void*_tmp3BE;void*_tmp3BF;void*_tmp3C0;void*
_tmp3C1;void*_tmp3C2;void*_tmp3C3;void*_tmp3C4;void*_tmp3C5;void*_tmp3C6;void*
_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*_tmp3CA;void*_tmp3CB;void*_tmp3CC;void*
_tmp3CD;void*_tmp3CE;void*_tmp3CF;void*_tmp3D0;void*_tmp3D1;void*_tmp3D2;void*
_tmp3D3;void*_tmp3D4;void*_tmp3D5;void*_tmp3D6;void*_tmp3D7;void*_tmp3D8;void*
_tmp3D9;void*_tmp3DA;void*_tmp3DB;void*_tmp3DC;void*_tmp3DD;void*_tmp3DE;void*
_tmp3DF;void*_tmp3E0;void*_tmp3E1;void*_tmp3E2;void*_tmp3E3;void*_tmp3E4;void*
_tmp3E5;void*_tmp3E6;void*_tmp3E7;void*_tmp3E8;void*_tmp3E9;void*_tmp3EA;void*
_tmp3EB;_LL2D4: _tmp3B9=_tmp3B8.f1;if(_tmp3B9 <= (void*)3?1:*((int*)_tmp3B9)!= 6)
goto _LL2D6;_tmp3BA=((struct Cyc_Absyn_DoubleType_struct*)_tmp3B9)->f1;_tmp3BB=
_tmp3B8.f2;if(_tmp3BB <= (void*)3?1:*((int*)_tmp3BB)!= 6)goto _LL2D6;_tmp3BC=((
struct Cyc_Absyn_DoubleType_struct*)_tmp3BB)->f1;_LL2D5: return !_tmp3BC?_tmp3BA: 0;
_LL2D6: _tmp3BD=_tmp3B8.f1;if(_tmp3BD <= (void*)3?1:*((int*)_tmp3BD)!= 6)goto
_LL2D8;_tmp3BE=_tmp3B8.f2;if((int)_tmp3BE != 1)goto _LL2D8;_LL2D7: goto _LL2D9;
_LL2D8: _tmp3BF=_tmp3B8.f1;if(_tmp3BF <= (void*)3?1:*((int*)_tmp3BF)!= 6)goto
_LL2DA;_tmp3C0=_tmp3B8.f2;if(_tmp3C0 <= (void*)3?1:*((int*)_tmp3C0)!= 5)goto
_LL2DA;_LL2D9: goto _LL2DB;_LL2DA: _tmp3C1=_tmp3B8.f1;if(_tmp3C1 <= (void*)3?1:*((
int*)_tmp3C1)!= 6)goto _LL2DC;_tmp3C2=_tmp3B8.f2;if(_tmp3C2 <= (void*)3?1:*((int*)
_tmp3C2)!= 14)goto _LL2DC;_LL2DB: goto _LL2DD;_LL2DC: _tmp3C3=_tmp3B8.f1;if((int)
_tmp3C3 != 1)goto _LL2DE;_tmp3C4=_tmp3B8.f2;if(_tmp3C4 <= (void*)3?1:*((int*)
_tmp3C4)!= 14)goto _LL2DE;_LL2DD: goto _LL2DF;_LL2DE: _tmp3C5=_tmp3B8.f1;if(_tmp3C5
<= (void*)3?1:*((int*)_tmp3C5)!= 6)goto _LL2E0;_tmp3C6=_tmp3B8.f2;if(_tmp3C6 <= (
void*)3?1:*((int*)_tmp3C6)!= 17)goto _LL2E0;_LL2DF: goto _LL2E1;_LL2E0: _tmp3C7=
_tmp3B8.f1;if((int)_tmp3C7 != 1)goto _LL2E2;_tmp3C8=_tmp3B8.f2;if(_tmp3C8 <= (void*)
3?1:*((int*)_tmp3C8)!= 17)goto _LL2E2;_LL2E1: goto _LL2E3;_LL2E2: _tmp3C9=_tmp3B8.f1;
if((int)_tmp3C9 != 1)goto _LL2E4;_tmp3CA=_tmp3B8.f2;if(_tmp3CA <= (void*)3?1:*((int*)
_tmp3CA)!= 5)goto _LL2E4;_LL2E3: return 1;_LL2E4: _tmp3CB=_tmp3B8.f1;if(_tmp3CB <= (
void*)3?1:*((int*)_tmp3CB)!= 5)goto _LL2E6;_tmp3CC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3CB)->f2;if((int)_tmp3CC != 3)goto _LL2E6;_tmp3CD=_tmp3B8.f2;if(_tmp3CD <= (
void*)3?1:*((int*)_tmp3CD)!= 5)goto _LL2E6;_tmp3CE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3CD)->f2;if((int)_tmp3CE != 3)goto _LL2E6;_LL2E5: return 0;_LL2E6: _tmp3CF=
_tmp3B8.f1;if(_tmp3CF <= (void*)3?1:*((int*)_tmp3CF)!= 5)goto _LL2E8;_tmp3D0=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3CF)->f2;if((int)_tmp3D0 != 3)goto _LL2E8;
_LL2E7: goto _LL2E9;_LL2E8: _tmp3D1=_tmp3B8.f1;if(_tmp3D1 <= (void*)3?1:*((int*)
_tmp3D1)!= 5)goto _LL2EA;_tmp3D2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D1)->f2;
if((int)_tmp3D2 != 2)goto _LL2EA;_tmp3D3=_tmp3B8.f2;if((int)_tmp3D3 != 1)goto _LL2EA;
_LL2E9: goto _LL2EB;_LL2EA: _tmp3D4=_tmp3B8.f1;if(_tmp3D4 <= (void*)3?1:*((int*)
_tmp3D4)!= 5)goto _LL2EC;_tmp3D5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D4)->f2;
if((int)_tmp3D5 != 2)goto _LL2EC;_tmp3D6=_tmp3B8.f2;if(_tmp3D6 <= (void*)3?1:*((int*)
_tmp3D6)!= 5)goto _LL2EC;_tmp3D7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D6)->f2;
if((int)_tmp3D7 != 1)goto _LL2EC;_LL2EB: goto _LL2ED;_LL2EC: _tmp3D8=_tmp3B8.f1;if(
_tmp3D8 <= (void*)3?1:*((int*)_tmp3D8)!= 5)goto _LL2EE;_tmp3D9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D8)->f2;if((int)_tmp3D9 != 2)goto _LL2EE;_tmp3DA=_tmp3B8.f2;if(_tmp3DA <= (
void*)3?1:*((int*)_tmp3DA)!= 5)goto _LL2EE;_tmp3DB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3DA)->f2;if((int)_tmp3DB != 0)goto _LL2EE;_LL2ED: goto _LL2EF;_LL2EE: _tmp3DC=
_tmp3B8.f1;if(_tmp3DC <= (void*)3?1:*((int*)_tmp3DC)!= 5)goto _LL2F0;_tmp3DD=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3DC)->f2;if((int)_tmp3DD != 1)goto _LL2F0;
_tmp3DE=_tmp3B8.f2;if(_tmp3DE <= (void*)3?1:*((int*)_tmp3DE)!= 5)goto _LL2F0;
_tmp3DF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3DE)->f2;if((int)_tmp3DF != 
0)goto _LL2F0;_LL2EF: goto _LL2F1;_LL2F0: _tmp3E0=_tmp3B8.f1;if(_tmp3E0 <= (void*)3?1:*((
int*)_tmp3E0)!= 17)goto _LL2F2;_tmp3E1=_tmp3B8.f2;if(_tmp3E1 <= (void*)3?1:*((int*)
_tmp3E1)!= 5)goto _LL2F2;_tmp3E2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E1)->f2;
if((int)_tmp3E2 != 1)goto _LL2F2;_LL2F1: goto _LL2F3;_LL2F2: _tmp3E3=_tmp3B8.f1;if(
_tmp3E3 <= (void*)3?1:*((int*)_tmp3E3)!= 17)goto _LL2F4;_tmp3E4=_tmp3B8.f2;if(
_tmp3E4 <= (void*)3?1:*((int*)_tmp3E4)!= 5)goto _LL2F4;_tmp3E5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3E4)->f2;if((int)_tmp3E5 != 0)goto _LL2F4;_LL2F3: goto _LL2F5;_LL2F4: _tmp3E6=
_tmp3B8.f1;if(_tmp3E6 <= (void*)3?1:*((int*)_tmp3E6)!= 14)goto _LL2F6;_tmp3E7=
_tmp3B8.f2;if(_tmp3E7 <= (void*)3?1:*((int*)_tmp3E7)!= 5)goto _LL2F6;_tmp3E8=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3E7)->f2;if((int)_tmp3E8 != 1)goto _LL2F6;
_LL2F5: goto _LL2F7;_LL2F6: _tmp3E9=_tmp3B8.f1;if(_tmp3E9 <= (void*)3?1:*((int*)
_tmp3E9)!= 14)goto _LL2F8;_tmp3EA=_tmp3B8.f2;if(_tmp3EA <= (void*)3?1:*((int*)
_tmp3EA)!= 5)goto _LL2F8;_tmp3EB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3EA)->f2;
if((int)_tmp3EB != 0)goto _LL2F8;_LL2F7: return 1;_LL2F8:;_LL2F9: return 0;_LL2D3:;}
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=
el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0?1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC->v=(
void*)t1;_tmp3EC;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)((
struct Cyc_Core_Opt*)_check_null(max_arith_type))->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_Std_String_pa_struct _tmp3EF;_tmp3EF.tag=0;_tmp3EF.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp3EE;_tmp3EE.tag=0;_tmp3EE.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp3ED[2]={& _tmp3EE,& _tmp3EF};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,
_tag_arr("type mismatch: expecting %s but found %s",sizeof(char),41),_tag_arr(
_tmp3ED,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp3F0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL2FB: if(_tmp3F0 <= (void*)3?1:*((int*)_tmp3F0)!= 4)goto _LL2FD;_LL2FC: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ);goto _LL2FA;_LL2FD:;_LL2FE: return 0;_LL2FA:;}return 1;}int
Cyc_Tcutil_is_integral_type(void*t){void*_tmp3F1=Cyc_Tcutil_compress(t);_LL300:
if(_tmp3F1 <= (void*)3?1:*((int*)_tmp3F1)!= 5)goto _LL302;_LL301: goto _LL303;_LL302:
if(_tmp3F1 <= (void*)3?1:*((int*)_tmp3F1)!= 14)goto _LL304;_LL303: goto _LL305;
_LL304: if(_tmp3F1 <= (void*)3?1:*((int*)_tmp3F1)!= 17)goto _LL306;_LL305: goto
_LL307;_LL306: if(_tmp3F1 <= (void*)3?1:*((int*)_tmp3F1)!= 12)goto _LL308;_LL307:
goto _LL309;_LL308: if(_tmp3F1 <= (void*)3?1:*((int*)_tmp3F1)!= 13)goto _LL30A;
_LL309: return 1;_LL30A:;_LL30B: return 0;_LL2FF:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp3F2[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3F2,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp3F3[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3F3,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp3F5=({struct _tuple6 _tmp3F4;
_tmp3F4.f1=t1;_tmp3F4.f2=t2;_tmp3F4;});void*_tmp3F6;struct Cyc_Absyn_PtrInfo
_tmp3F7;void*_tmp3F8;struct Cyc_Absyn_PtrInfo _tmp3F9;void*_tmp3FA;void*_tmp3FB;
struct Cyc_Absyn_Tqual _tmp3FC;struct Cyc_Absyn_Exp*_tmp3FD;void*_tmp3FE;void*
_tmp3FF;struct Cyc_Absyn_Tqual _tmp400;struct Cyc_Absyn_Exp*_tmp401;void*_tmp402;
struct Cyc_Absyn_TunionFieldInfo _tmp403;void*_tmp404;struct Cyc_Absyn_Tuniondecl*
_tmp405;struct Cyc_Absyn_Tunionfield*_tmp406;struct Cyc_List_List*_tmp407;void*
_tmp408;struct Cyc_Absyn_TunionInfo _tmp409;void*_tmp40A;struct Cyc_Absyn_Tuniondecl**
_tmp40B;struct Cyc_Absyn_Tuniondecl*_tmp40C;struct Cyc_List_List*_tmp40D;void*
_tmp40E;struct Cyc_Absyn_PtrInfo _tmp40F;void*_tmp410;void*_tmp411;struct Cyc_Absyn_Conref*
_tmp412;struct Cyc_Absyn_Tqual _tmp413;struct Cyc_Absyn_Conref*_tmp414;void*_tmp415;
struct Cyc_Absyn_TunionInfo _tmp416;void*_tmp417;struct Cyc_Absyn_Tuniondecl**
_tmp418;struct Cyc_Absyn_Tuniondecl*_tmp419;struct Cyc_List_List*_tmp41A;void*
_tmp41B;void*_tmp41C;void*_tmp41D;void*_tmp41E;void*_tmp41F;void*_tmp420;void*
_tmp421;_LL30D: _tmp3F6=_tmp3F5.f1;if(_tmp3F6 <= (void*)3?1:*((int*)_tmp3F6)!= 4)
goto _LL30F;_tmp3F7=((struct Cyc_Absyn_PointerType_struct*)_tmp3F6)->f1;_tmp3F8=
_tmp3F5.f2;if(_tmp3F8 <= (void*)3?1:*((int*)_tmp3F8)!= 4)goto _LL30F;_tmp3F9=((
struct Cyc_Absyn_PointerType_struct*)_tmp3F8)->f1;_LL30E: {int okay=1;if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp3F7.nullable,_tmp3F9.nullable)){void*_tmp422=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp3F7.nullable))->v;int _tmp423;_LL31C: if(_tmp422 <= (void*)1?1:*((int*)_tmp422)
!= 0)goto _LL31E;_tmp423=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp422)->f1;
_LL31D: okay=!_tmp423;goto _LL31B;_LL31E:;_LL31F:({void*_tmp424[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("silent_castable conref not eq",
sizeof(char),30),_tag_arr(_tmp424,sizeof(void*),0));});_LL31B:;}if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp3F7.bounds,_tmp3F9.bounds)){struct _tuple6 _tmp426=({
struct _tuple6 _tmp425;_tmp425.f1=(void*)(Cyc_Absyn_compress_conref(_tmp3F7.bounds))->v;
_tmp425.f2=(void*)(Cyc_Absyn_compress_conref(_tmp3F9.bounds))->v;_tmp425;});void*
_tmp427;void*_tmp428;void*_tmp429;void*_tmp42A;void*_tmp42B;void*_tmp42C;struct
Cyc_Absyn_Exp*_tmp42D;void*_tmp42E;void*_tmp42F;struct Cyc_Absyn_Exp*_tmp430;void*
_tmp431;void*_tmp432;void*_tmp433;void*_tmp434;_LL321: _tmp427=_tmp426.f1;if(
_tmp427 <= (void*)1?1:*((int*)_tmp427)!= 0)goto _LL323;_tmp428=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp427)->f1;if(_tmp428 <= (void*)1?1:*((int*)_tmp428)!= 0)goto _LL323;_tmp429=
_tmp426.f2;if(_tmp429 <= (void*)1?1:*((int*)_tmp429)!= 0)goto _LL323;_tmp42A=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp429)->f1;if((int)_tmp42A != 0)goto _LL323;
_LL322: okay=1;goto _LL320;_LL323: _tmp42B=_tmp426.f1;if(_tmp42B <= (void*)1?1:*((
int*)_tmp42B)!= 0)goto _LL325;_tmp42C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp42B)->f1;if(_tmp42C <= (void*)1?1:*((int*)_tmp42C)!= 0)goto _LL325;_tmp42D=((
struct Cyc_Absyn_Upper_b_struct*)_tmp42C)->f1;_tmp42E=_tmp426.f2;if(_tmp42E <= (
void*)1?1:*((int*)_tmp42E)!= 0)goto _LL325;_tmp42F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp42E)->f1;if(_tmp42F <= (void*)1?1:*((int*)_tmp42F)!= 0)goto _LL325;_tmp430=((
struct Cyc_Absyn_Upper_b_struct*)_tmp42F)->f1;_LL324: okay=okay?Cyc_Evexp_lte_const_exp(
_tmp430,_tmp42D): 0;({void*_tmp435[0]={};Cyc_Tcutil_warn(loc,_tag_arr("implicit cast to shorter array",
sizeof(char),31),_tag_arr(_tmp435,sizeof(void*),0));});goto _LL320;_LL325: _tmp431=
_tmp426.f1;if(_tmp431 <= (void*)1?1:*((int*)_tmp431)!= 0)goto _LL327;_tmp432=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp431)->f1;if((int)_tmp432 != 0)goto _LL327;
_tmp433=_tmp426.f2;if(_tmp433 <= (void*)1?1:*((int*)_tmp433)!= 0)goto _LL327;
_tmp434=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp433)->f1;if((int)_tmp434
!= 0)goto _LL327;_LL326: okay=1;goto _LL320;_LL327:;_LL328: okay=0;goto _LL320;_LL320:;}
okay=okay?Cyc_Tcutil_unify((void*)_tmp3F7.elt_typ,(void*)_tmp3F9.elt_typ): 0;okay=
okay?Cyc_Tcutil_unify((void*)_tmp3F7.rgn_typ,(void*)_tmp3F9.rgn_typ)?1: Cyc_Tcenv_region_outlives(
te,(void*)_tmp3F7.rgn_typ,(void*)_tmp3F9.rgn_typ): 0;okay=okay?!(_tmp3F7.tq).q_const?
1:(_tmp3F9.tq).q_const: 0;return okay;}_LL30F: _tmp3FA=_tmp3F5.f1;if(_tmp3FA <= (
void*)3?1:*((int*)_tmp3FA)!= 7)goto _LL311;_tmp3FB=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp3FA)->f1;_tmp3FC=((struct Cyc_Absyn_ArrayType_struct*)_tmp3FA)->f2;_tmp3FD=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3FA)->f3;_tmp3FE=_tmp3F5.f2;if(_tmp3FE <= (
void*)3?1:*((int*)_tmp3FE)!= 7)goto _LL311;_tmp3FF=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp3FE)->f1;_tmp400=((struct Cyc_Absyn_ArrayType_struct*)_tmp3FE)->f2;_tmp401=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3FE)->f3;_LL310: {int okay;okay=(_tmp3FD != 
0?_tmp401 != 0: 0)?Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp3FD),(struct Cyc_Absyn_Exp*)_check_null(_tmp401)): 0;return(okay?Cyc_Tcutil_unify(
_tmp3FB,_tmp3FF): 0)?!_tmp3FC.q_const?1: _tmp400.q_const: 0;}_LL311: _tmp402=_tmp3F5.f1;
if(_tmp402 <= (void*)3?1:*((int*)_tmp402)!= 3)goto _LL313;_tmp403=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp402)->f1;_tmp404=(void*)_tmp403.field_info;if(*((int*)_tmp404)!= 1)goto
_LL313;_tmp405=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp404)->f1;_tmp406=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp404)->f2;_tmp407=_tmp403.targs;
_tmp408=_tmp3F5.f2;if(_tmp408 <= (void*)3?1:*((int*)_tmp408)!= 2)goto _LL313;
_tmp409=((struct Cyc_Absyn_TunionType_struct*)_tmp408)->f1;_tmp40A=(void*)_tmp409.tunion_info;
if(*((int*)_tmp40A)!= 1)goto _LL313;_tmp40B=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp40A)->f1;_tmp40C=*_tmp40B;_tmp40D=_tmp409.targs;_LL312: if((_tmp405 == _tmp40C?
1: Cyc_Absyn_qvar_cmp(_tmp405->name,_tmp40C->name)== 0)?_tmp406->typs == 0: 0){for(
0;_tmp407 != 0?_tmp40D != 0: 0;(_tmp407=_tmp407->tl,_tmp40D=_tmp40D->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp407->hd,(void*)_tmp40D->hd))break;}if(_tmp407 == 0?_tmp40D == 0: 0)return
1;}return 0;_LL313: _tmp40E=_tmp3F5.f1;if(_tmp40E <= (void*)3?1:*((int*)_tmp40E)!= 
4)goto _LL315;_tmp40F=((struct Cyc_Absyn_PointerType_struct*)_tmp40E)->f1;_tmp410=(
void*)_tmp40F.elt_typ;_tmp411=(void*)_tmp40F.rgn_typ;_tmp412=_tmp40F.nullable;
_tmp413=_tmp40F.tq;_tmp414=_tmp40F.bounds;_tmp415=_tmp3F5.f2;if(_tmp415 <= (void*)
3?1:*((int*)_tmp415)!= 2)goto _LL315;_tmp416=((struct Cyc_Absyn_TunionType_struct*)
_tmp415)->f1;_tmp417=(void*)_tmp416.tunion_info;if(*((int*)_tmp417)!= 1)goto
_LL315;_tmp418=((struct Cyc_Absyn_KnownTunion_struct*)_tmp417)->f1;_tmp419=*
_tmp418;_tmp41A=_tmp416.targs;_tmp41B=(void*)_tmp416.rgn;_LL314:{void*_tmp436=
Cyc_Tcutil_compress(_tmp410);struct Cyc_Absyn_TunionFieldInfo _tmp437;void*_tmp438;
struct Cyc_Absyn_Tuniondecl*_tmp439;struct Cyc_Absyn_Tunionfield*_tmp43A;struct Cyc_List_List*
_tmp43B;_LL32A: if(_tmp436 <= (void*)3?1:*((int*)_tmp436)!= 3)goto _LL32C;_tmp437=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp436)->f1;_tmp438=(void*)_tmp437.field_info;
if(*((int*)_tmp438)!= 1)goto _LL32C;_tmp439=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp438)->f1;_tmp43A=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp438)->f2;
_tmp43B=_tmp437.targs;_LL32B: if(!Cyc_Tcutil_unify(_tmp411,_tmp41B)?!Cyc_Tcenv_region_outlives(
te,_tmp411,_tmp41B): 0)return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp412,((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(0)))return 0;if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp414,Cyc_Absyn_new_conref(Cyc_Absyn_bounds_one)))return 0;
if(Cyc_Absyn_qvar_cmp(_tmp419->name,_tmp439->name)== 0?_tmp43A->typs != 0: 0){int
okay=1;for(0;_tmp43B != 0?_tmp41A != 0: 0;(_tmp43B=_tmp43B->tl,_tmp41A=_tmp41A->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp43B->hd,(void*)_tmp41A->hd)){okay=0;break;}}if((
!okay?1: _tmp43B != 0)?1: _tmp41A != 0)return 0;return 1;}goto _LL329;_LL32C:;_LL32D:
goto _LL329;_LL329:;}return 0;_LL315: _tmp41C=_tmp3F5.f1;if(_tmp41C <= (void*)3?1:*((
int*)_tmp41C)!= 14)goto _LL317;_tmp41D=_tmp3F5.f2;if(_tmp41D <= (void*)3?1:*((int*)
_tmp41D)!= 5)goto _LL317;_tmp41E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp41D)->f2;
if((int)_tmp41E != 2)goto _LL317;_LL316: goto _LL318;_LL317: _tmp41F=_tmp3F5.f1;if(
_tmp41F <= (void*)3?1:*((int*)_tmp41F)!= 17)goto _LL319;_tmp420=_tmp3F5.f2;if(
_tmp420 <= (void*)3?1:*((int*)_tmp420)!= 5)goto _LL319;_tmp421=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp420)->f2;if((int)_tmp421 != 2)goto _LL319;_LL318: return 1;_LL319:;_LL31A: return
Cyc_Tcutil_unify(t1,t2);_LL30C:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*
_tmp43C=Cyc_Tcutil_compress(t);_LL32F: if(_tmp43C <= (void*)3?1:*((int*)_tmp43C)!= 
4)goto _LL331;_LL330: return 1;_LL331:;_LL332: return 0;_LL32E:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp43D=(void*)e->r;void*_tmp43E;int _tmp43F;_LL334:
if(*((int*)_tmp43D)!= 0)goto _LL336;_tmp43E=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp43D)->f1;if(_tmp43E <= (void*)1?1:*((int*)_tmp43E)!= 2)goto _LL336;_tmp43F=((
struct Cyc_Absyn_Int_c_struct*)_tmp43E)->f2;if(_tmp43F != 0)goto _LL336;_LL335:
return 1;_LL336:;_LL337: return 0;_LL333:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((
void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt
Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)
1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*
e1){if(Cyc_Tcutil_is_pointer_type(t2)?Cyc_Tcutil_is_zero(e1): 0){(void*)(e1->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp440=_cycalloc(sizeof(*_tmp440));
_tmp440[0]=({struct Cyc_Absyn_Const_e_struct _tmp441;_tmp441.tag=0;_tmp441.f1=(
void*)((void*)0);_tmp441;});_tmp440;})));{struct Cyc_List_List*_tmp442=Cyc_Tcenv_lookup_type_vars(
te);struct Cyc_Absyn_PointerType_struct*_tmp443=({struct Cyc_Absyn_PointerType_struct*
_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444[0]=({struct Cyc_Absyn_PointerType_struct
_tmp445;_tmp445.tag=4;_tmp445.f1=({struct Cyc_Absyn_PtrInfo _tmp446;_tmp446.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->v=_tmp442;_tmp448;}));_tmp446.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->v=_tmp442;_tmp447;}));_tmp446.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp446.tq=Cyc_Absyn_empty_tqual();
_tmp446.bounds=Cyc_Absyn_empty_conref();_tmp446;});_tmp445;});_tmp444;});(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp443));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}static int Cyc_Tcutil_is_sizeof_type(void*t1){void*_tmp449=
Cyc_Tcutil_compress(t1);_LL339: if(_tmp449 <= (void*)3?1:*((int*)_tmp449)!= 14)
goto _LL33B;_LL33A: return 1;_LL33B:;_LL33C: return 0;_LL338:;}static int Cyc_Tcutil_is_tag_type(
void*t1){void*_tmp44A=Cyc_Tcutil_compress(t1);_LL33E: if(_tmp44A <= (void*)3?1:*((
int*)_tmp44A)!= 17)goto _LL340;_LL33F: return 1;_LL340:;_LL341: return 0;_LL33D:;}int
Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
if(Cyc_Tcutil_unify(t1,t2))return 1;if(((Cyc_Tcutil_is_arithmetic_type(t1)?Cyc_Tcutil_is_arithmetic_type(
t2): 0)?1:(Cyc_Tcutil_is_sizeof_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0))?1:(
Cyc_Tcutil_is_tag_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0)){if(Cyc_Tcutil_will_lose_precision(
t1,t2))({struct Cyc_Std_String_pa_struct _tmp44D;_tmp44D.tag=0;_tmp44D.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmp44C;
_tmp44C.tag=0;_tmp44C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmp44B[2]={& _tmp44C,& _tmp44D};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; %s -> %s conversion supplied",
sizeof(char),53),_tag_arr(_tmp44B,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if(Cyc_Tcutil_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);({struct Cyc_Std_String_pa_struct
_tmp450;_tmp450.tag=0;_tmp450.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp44F;_tmp44F.tag=0;_tmp44F.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp44E[2]={& _tmp44F,& _tmp450};Cyc_Tcutil_warn(
e->loc,_tag_arr("implicit cast from %s to %s",sizeof(char),28),_tag_arr(_tmp44E,
sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp451=Cyc_Tcutil_compress(t);
_LL343: if(_tmp451 <= (void*)3?1:*((int*)_tmp451)!= 5)goto _LL345;_LL344: goto _LL346;
_LL345: if((int)_tmp451 != 1)goto _LL347;_LL346: goto _LL348;_LL347: if(_tmp451 <= (
void*)3?1:*((int*)_tmp451)!= 6)goto _LL349;_LL348: return 1;_LL349:;_LL34A: return 0;
_LL342:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*inst,
struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp452=_cycalloc(sizeof(*
_tmp452));_tmp452->f1=x->tq;_tmp452->f2=Cyc_Tcutil_substitute(inst,(void*)x->type);
_tmp452;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp453=t1;struct Cyc_List_List*
_tmp454;struct Cyc_Absyn_AggrInfo _tmp455;void*_tmp456;struct Cyc_Absyn_Aggrdecl**
_tmp457;struct Cyc_Absyn_Aggrdecl*_tmp458;struct Cyc_List_List*_tmp459;void*
_tmp45A;struct Cyc_List_List*_tmp45B;_LL34C: if((int)_tmp453 != 0)goto _LL34E;_LL34D:
return 0;_LL34E: if(_tmp453 <= (void*)3?1:*((int*)_tmp453)!= 9)goto _LL350;_tmp454=((
struct Cyc_Absyn_TupleType_struct*)_tmp453)->f1;_LL34F: return _tmp454;_LL350: if(
_tmp453 <= (void*)3?1:*((int*)_tmp453)!= 10)goto _LL352;_tmp455=((struct Cyc_Absyn_AggrType_struct*)
_tmp453)->f1;_tmp456=(void*)_tmp455.aggr_info;if(*((int*)_tmp456)!= 1)goto _LL352;
_tmp457=((struct Cyc_Absyn_KnownAggr_struct*)_tmp456)->f1;_tmp458=*_tmp457;
_tmp459=_tmp455.targs;_LL351: if((((void*)_tmp458->kind == (void*)1?1: _tmp458->impl
== 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp458->impl))->exist_vars
!= 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp458->impl))->rgn_po != 0)
return({struct Cyc_List_List*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->hd=({
struct _tuple4*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->f1=Cyc_Absyn_empty_tqual();
_tmp45D->f2=t1;_tmp45D;});_tmp45C->tl=0;_tmp45C;});{struct Cyc_List_List*_tmp45E=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp458->tvs,_tmp459);return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp45E,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp458->impl))->fields);}_LL352: if(_tmp453 <= (void*)3?1:*((int*)
_tmp453)!= 11)goto _LL354;_tmp45A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp453)->f1;if((int)_tmp45A != 0)goto _LL354;_tmp45B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp453)->f2;_LL353: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp45B);_LL354:;_LL355: return({struct Cyc_List_List*
_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->hd=({struct _tuple4*_tmp460=
_cycalloc(sizeof(*_tmp460));_tmp460->f1=Cyc_Absyn_empty_tqual();_tmp460->f2=t1;
_tmp460;});_tmp45F->tl=0;_tmp45F;});_LL34B:;}}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1)?Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2):
0)return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp462=({struct _tuple6 _tmp461;_tmp461.f1=t1;_tmp461.f2=t2;_tmp461;});void*
_tmp463;struct Cyc_Absyn_PtrInfo _tmp464;void*_tmp465;void*_tmp466;struct Cyc_Absyn_Conref*
_tmp467;struct Cyc_Absyn_Tqual _tmp468;struct Cyc_Absyn_Conref*_tmp469;void*_tmp46A;
struct Cyc_Absyn_PtrInfo _tmp46B;void*_tmp46C;void*_tmp46D;struct Cyc_Absyn_Conref*
_tmp46E;struct Cyc_Absyn_Tqual _tmp46F;struct Cyc_Absyn_Conref*_tmp470;_LL357:
_tmp463=_tmp462.f1;if(_tmp463 <= (void*)3?1:*((int*)_tmp463)!= 4)goto _LL359;
_tmp464=((struct Cyc_Absyn_PointerType_struct*)_tmp463)->f1;_tmp465=(void*)
_tmp464.elt_typ;_tmp466=(void*)_tmp464.rgn_typ;_tmp467=_tmp464.nullable;_tmp468=
_tmp464.tq;_tmp469=_tmp464.bounds;_tmp46A=_tmp462.f2;if(_tmp46A <= (void*)3?1:*((
int*)_tmp46A)!= 4)goto _LL359;_tmp46B=((struct Cyc_Absyn_PointerType_struct*)
_tmp46A)->f1;_tmp46C=(void*)_tmp46B.elt_typ;_tmp46D=(void*)_tmp46B.rgn_typ;
_tmp46E=_tmp46B.nullable;_tmp46F=_tmp46B.tq;_tmp470=_tmp46B.bounds;_LL358: if(
_tmp468.q_const?!_tmp46F.q_const: 0)return 0;if((!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp467,_tmp46E)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp467): 0)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp46E): 0)
return 0;if(!Cyc_Tcutil_unify(_tmp466,_tmp46D)?!Cyc_Tcenv_region_outlives(te,
_tmp466,_tmp46D): 0)return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp469,_tmp470)){struct _tuple6 _tmp472=({struct _tuple6 _tmp471;_tmp471.f1=Cyc_Absyn_conref_val(
_tmp469);_tmp471.f2=Cyc_Absyn_conref_val(_tmp470);_tmp471;});void*_tmp473;void*
_tmp474;void*_tmp475;struct Cyc_Absyn_Exp*_tmp476;void*_tmp477;struct Cyc_Absyn_Exp*
_tmp478;_LL35C: _tmp473=_tmp472.f1;if(_tmp473 <= (void*)1?1:*((int*)_tmp473)!= 0)
goto _LL35E;_tmp474=_tmp472.f2;if((int)_tmp474 != 0)goto _LL35E;_LL35D: goto _LL35B;
_LL35E: _tmp475=_tmp472.f1;if(_tmp475 <= (void*)1?1:*((int*)_tmp475)!= 0)goto
_LL360;_tmp476=((struct Cyc_Absyn_Upper_b_struct*)_tmp475)->f1;_tmp477=_tmp472.f2;
if(_tmp477 <= (void*)1?1:*((int*)_tmp477)!= 0)goto _LL360;_tmp478=((struct Cyc_Absyn_Upper_b_struct*)
_tmp477)->f1;_LL35F: if(!Cyc_Evexp_lte_const_exp(_tmp478,_tmp476))return 0;goto
_LL35B;_LL360:;_LL361: return 0;_LL35B:;}return Cyc_Tcutil_ptrsubtype(te,({struct
Cyc_List_List*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->hd=({struct _tuple6*
_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->f1=t1;_tmp47A->f2=t2;_tmp47A;});
_tmp479->tl=assume;_tmp479;}),_tmp465,_tmp46C);_LL359:;_LL35A: return 0;_LL356:;}}
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp47C=({struct
_tuple6 _tmp47B;_tmp47B.f1=Cyc_Tcutil_compress(t1);_tmp47B.f2=Cyc_Tcutil_compress(
t2);_tmp47B;});void*_tmp47D;void*_tmp47E;void*_tmp47F;void*_tmp480;_LL363:
_tmp47D=_tmp47C.f1;if(_tmp47D <= (void*)3?1:*((int*)_tmp47D)!= 5)goto _LL365;
_tmp47E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47D)->f2;_tmp47F=_tmp47C.f2;
if(_tmp47F <= (void*)3?1:*((int*)_tmp47F)!= 5)goto _LL365;_tmp480=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp47F)->f2;_LL364: return _tmp47E == _tmp480;_LL365:;
_LL366: return 0;_LL362:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp482;struct
Cyc_Absyn_Tqual _tmp483;void*_tmp484;struct _tuple4*_tmp481=(struct _tuple4*)tqs1->hd;
_tmp482=*_tmp481;_tmp483=_tmp482.f1;_tmp484=_tmp482.f2;{struct _tuple4 _tmp486;
struct Cyc_Absyn_Tqual _tmp487;void*_tmp488;struct _tuple4*_tmp485=(struct _tuple4*)
tqs2->hd;_tmp486=*_tmp485;_tmp487=_tmp486.f1;_tmp488=_tmp486.f2;if(_tmp487.q_const?
Cyc_Tcutil_subtype(te,assume,_tmp484,_tmp488): 0)continue;else{if(Cyc_Tcutil_unify(
_tmp484,_tmp488))continue;else{if(Cyc_Tcutil_isomorphic(_tmp484,_tmp488))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp489=Cyc_Tcutil_compress(t);void*_tmp48A;_LL368: if(_tmp489 <= (void*)3?1:*((
int*)_tmp489)!= 5)goto _LL36A;_tmp48A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp489)->f2;if((int)_tmp48A != 0)goto _LL36A;_LL369: return 1;_LL36A:;_LL36B: return
0;_LL367:;}int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{void*_tmp48B=t2;void*_tmp48C;_LL36D: if(_tmp48B <= (
void*)3?1:*((int*)_tmp48B)!= 5)goto _LL36F;_tmp48C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48B)->f2;if((int)_tmp48C != 2)goto _LL36F;_LL36E: if(Cyc_Tcutil_typ_kind(t2)== (
void*)2)return 1;goto _LL36C;_LL36F:;_LL370: goto _LL36C;_LL36C:;}{void*_tmp48D=t1;
struct Cyc_Absyn_PtrInfo _tmp48E;void*_tmp48F;void*_tmp490;struct Cyc_Absyn_Conref*
_tmp491;struct Cyc_Absyn_Tqual _tmp492;struct Cyc_Absyn_Conref*_tmp493;void*_tmp494;
struct Cyc_Absyn_Tqual _tmp495;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_Absyn_Enumdecl*
_tmp497;_LL372: if(_tmp48D <= (void*)3?1:*((int*)_tmp48D)!= 4)goto _LL374;_tmp48E=((
struct Cyc_Absyn_PointerType_struct*)_tmp48D)->f1;_tmp48F=(void*)_tmp48E.elt_typ;
_tmp490=(void*)_tmp48E.rgn_typ;_tmp491=_tmp48E.nullable;_tmp492=_tmp48E.tq;
_tmp493=_tmp48E.bounds;_LL373:{void*_tmp498=t2;struct Cyc_Absyn_PtrInfo _tmp499;
void*_tmp49A;void*_tmp49B;struct Cyc_Absyn_Conref*_tmp49C;struct Cyc_Absyn_Tqual
_tmp49D;struct Cyc_Absyn_Conref*_tmp49E;_LL383: if(_tmp498 <= (void*)3?1:*((int*)
_tmp498)!= 4)goto _LL385;_tmp499=((struct Cyc_Absyn_PointerType_struct*)_tmp498)->f1;
_tmp49A=(void*)_tmp499.elt_typ;_tmp49B=(void*)_tmp499.rgn_typ;_tmp49C=_tmp499.nullable;
_tmp49D=_tmp499.tq;_tmp49E=_tmp499.bounds;_LL384: {struct Cyc_List_List*_tmp49F=({
struct Cyc_List_List*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));_tmp4A8->hd=({struct
_tuple6*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9->f1=t1;_tmp4A9->f2=t2;
_tmp4A9;});_tmp4A8->tl=0;_tmp4A8;});int _tmp4A0=Cyc_Tcutil_ptrsubtype(te,_tmp49F,
_tmp48F,_tmp49A)?!_tmp492.q_const?1: _tmp49D.q_const: 0;int _tmp4A1=_tmp4A0?0:((Cyc_Tcutil_bits_only(
_tmp48F)?Cyc_Tcutil_is_char_type(_tmp49A): 0)?_tmp49D.q_const?1: !_tmp492.q_const:
0);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp493,_tmp49E);
if(!bounds_ok?!_tmp4A1: 0){struct _tuple6 _tmp4A3=({struct _tuple6 _tmp4A2;_tmp4A2.f1=
Cyc_Absyn_conref_val(_tmp493);_tmp4A2.f2=Cyc_Absyn_conref_val(_tmp49E);_tmp4A2;});
void*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;void*_tmp4A6;struct Cyc_Absyn_Exp*
_tmp4A7;_LL38A: _tmp4A4=_tmp4A3.f1;if(_tmp4A4 <= (void*)1?1:*((int*)_tmp4A4)!= 0)
goto _LL38C;_tmp4A5=((struct Cyc_Absyn_Upper_b_struct*)_tmp4A4)->f1;_tmp4A6=
_tmp4A3.f2;if(_tmp4A6 <= (void*)1?1:*((int*)_tmp4A6)!= 0)goto _LL38C;_tmp4A7=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4A6)->f1;_LL38B: if(Cyc_Evexp_lte_const_exp(
_tmp4A7,_tmp4A5))bounds_ok=1;goto _LL389;_LL38C:;_LL38D: bounds_ok=1;goto _LL389;
_LL389:;}return(bounds_ok?_tmp4A0?1: _tmp4A1: 0)?Cyc_Tcutil_unify(_tmp490,_tmp49B)?
1: Cyc_Tcenv_region_outlives(te,_tmp490,_tmp49B): 0;}_LL385: if((int)_tmp498 != 0)
goto _LL387;_LL386: return 1;_LL387:;_LL388: goto _LL382;_LL382:;}return 0;_LL374: if(
_tmp48D <= (void*)3?1:*((int*)_tmp48D)!= 7)goto _LL376;_tmp494=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp48D)->f1;_tmp495=((struct Cyc_Absyn_ArrayType_struct*)_tmp48D)->f2;_tmp496=((
struct Cyc_Absyn_ArrayType_struct*)_tmp48D)->f3;_LL375:{void*_tmp4AA=t2;void*
_tmp4AB;struct Cyc_Absyn_Tqual _tmp4AC;struct Cyc_Absyn_Exp*_tmp4AD;_LL38F: if(
_tmp4AA <= (void*)3?1:*((int*)_tmp4AA)!= 7)goto _LL391;_tmp4AB=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp4AA)->f1;_tmp4AC=((struct Cyc_Absyn_ArrayType_struct*)_tmp4AA)->f2;_tmp4AD=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4AA)->f3;_LL390: {int okay;okay=(_tmp496 != 
0?_tmp4AD != 0: 0)?Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp4AD),(struct Cyc_Absyn_Exp*)_check_null(_tmp496)): 0;return(okay?Cyc_Tcutil_unify(
_tmp494,_tmp4AB): 0)?!_tmp495.q_const?1: _tmp4AC.q_const: 0;}_LL391:;_LL392: return 0;
_LL38E:;}return 0;_LL376: if(_tmp48D <= (void*)3?1:*((int*)_tmp48D)!= 12)goto _LL378;
_tmp497=((struct Cyc_Absyn_EnumType_struct*)_tmp48D)->f2;_LL377:{void*_tmp4AE=t2;
struct Cyc_Absyn_Enumdecl*_tmp4AF;_LL394: if(_tmp4AE <= (void*)3?1:*((int*)_tmp4AE)
!= 12)goto _LL396;_tmp4AF=((struct Cyc_Absyn_EnumType_struct*)_tmp4AE)->f2;_LL395:
if((_tmp497->fields != 0?_tmp4AF->fields != 0: 0)?((int(*)(struct Cyc_List_List*x))
Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp497->fields))->v)
>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4AF->fields))->v): 0)return 1;goto _LL393;
_LL396:;_LL397: goto _LL393;_LL393:;}goto _LL379;_LL378: if(_tmp48D <= (void*)3?1:*((
int*)_tmp48D)!= 5)goto _LL37A;_LL379: goto _LL37B;_LL37A: if((int)_tmp48D != 1)goto
_LL37C;_LL37B: goto _LL37D;_LL37C: if(_tmp48D <= (void*)3?1:*((int*)_tmp48D)!= 6)
goto _LL37E;_LL37D: return Cyc_Tcutil_coerceable(t2)?1: t2 == (void*)0;_LL37E: if((int)
_tmp48D != 0)goto _LL380;_LL37F: return t2 == (void*)0;_LL380:;_LL381: return 0;_LL371:;}}
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*
t){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){
struct Cyc_Absyn_Exp*_tmp4B0=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Cast_e_struct*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1[0]=({
struct Cyc_Absyn_Cast_e_struct _tmp4B2;_tmp4B2.tag=13;_tmp4B2.f1=(void*)t;_tmp4B2.f2=
_tmp4B0;_tmp4B2;});_tmp4B1;})));e->topt=({struct Cyc_Core_Opt*_tmp4B3=_cycalloc(
sizeof(*_tmp4B3));_tmp4B3->v=(void*)t;_tmp4B3;});}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp4B4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL399: if(_tmp4B4 <= (void*)3?1:*((int*)_tmp4B4)!= 5)
goto _LL39B;_LL39A: goto _LL39C;_LL39B: if(_tmp4B4 <= (void*)3?1:*((int*)_tmp4B4)!= 
12)goto _LL39D;_LL39C: goto _LL39E;_LL39D: if(_tmp4B4 <= (void*)3?1:*((int*)_tmp4B4)
!= 13)goto _LL39F;_LL39E: goto _LL3A0;_LL39F: if(_tmp4B4 <= (void*)3?1:*((int*)
_tmp4B4)!= 17)goto _LL3A1;_LL3A0: goto _LL3A2;_LL3A1: if(_tmp4B4 <= (void*)3?1:*((int*)
_tmp4B4)!= 14)goto _LL3A3;_LL3A2: return 1;_LL3A3: if(_tmp4B4 <= (void*)3?1:*((int*)
_tmp4B4)!= 0)goto _LL3A5;_LL3A4: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3A5:;_LL3A6: return 0;_LL398:;}int
Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return
1;{void*_tmp4B5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3A8: if((int)_tmp4B5 != 1)goto _LL3AA;_LL3A9: goto _LL3AB;_LL3AA: if(_tmp4B5 <= (
void*)3?1:*((int*)_tmp4B5)!= 6)goto _LL3AC;_LL3AB: return 1;_LL3AC:;_LL3AD: return 0;
_LL3A7:;}}int Cyc_Tcutil_is_function_type(void*t){void*_tmp4B6=Cyc_Tcutil_compress(
t);_LL3AF: if(_tmp4B6 <= (void*)3?1:*((int*)_tmp4B6)!= 8)goto _LL3B1;_LL3B0: return 1;
_LL3B1:;_LL3B2: return 0;_LL3AE:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*
t2){struct _tuple6 _tmp4B8=({struct _tuple6 _tmp4B7;_tmp4B7.f1=t1;_tmp4B7.f2=t2;
_tmp4B7;});void*_tmp4B9;int _tmp4BA;void*_tmp4BB;int _tmp4BC;void*_tmp4BD;void*
_tmp4BE;void*_tmp4BF;void*_tmp4C0;void*_tmp4C1;void*_tmp4C2;void*_tmp4C3;void*
_tmp4C4;void*_tmp4C5;void*_tmp4C6;void*_tmp4C7;void*_tmp4C8;void*_tmp4C9;void*
_tmp4CA;void*_tmp4CB;void*_tmp4CC;void*_tmp4CD;void*_tmp4CE;void*_tmp4CF;void*
_tmp4D0;void*_tmp4D1;void*_tmp4D2;void*_tmp4D3;void*_tmp4D4;void*_tmp4D5;void*
_tmp4D6;void*_tmp4D7;void*_tmp4D8;_LL3B4: _tmp4B9=_tmp4B8.f1;if(_tmp4B9 <= (void*)
3?1:*((int*)_tmp4B9)!= 6)goto _LL3B6;_tmp4BA=((struct Cyc_Absyn_DoubleType_struct*)
_tmp4B9)->f1;_tmp4BB=_tmp4B8.f2;if(_tmp4BB <= (void*)3?1:*((int*)_tmp4BB)!= 6)
goto _LL3B6;_tmp4BC=((struct Cyc_Absyn_DoubleType_struct*)_tmp4BB)->f1;_LL3B5: if(
_tmp4BA)return t1;else{return t2;}_LL3B6: _tmp4BD=_tmp4B8.f1;if(_tmp4BD <= (void*)3?
1:*((int*)_tmp4BD)!= 6)goto _LL3B8;_LL3B7: return t1;_LL3B8: _tmp4BE=_tmp4B8.f2;if(
_tmp4BE <= (void*)3?1:*((int*)_tmp4BE)!= 6)goto _LL3BA;_LL3B9: return t2;_LL3BA:
_tmp4BF=_tmp4B8.f1;if((int)_tmp4BF != 1)goto _LL3BC;_LL3BB: goto _LL3BD;_LL3BC:
_tmp4C0=_tmp4B8.f2;if((int)_tmp4C0 != 1)goto _LL3BE;_LL3BD: return(void*)1;_LL3BE:
_tmp4C1=_tmp4B8.f1;if(_tmp4C1 <= (void*)3?1:*((int*)_tmp4C1)!= 5)goto _LL3C0;
_tmp4C2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C1)->f1;if((int)_tmp4C2 != 
1)goto _LL3C0;_tmp4C3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C1)->f2;if((
int)_tmp4C3 != 3)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0: _tmp4C4=_tmp4B8.f2;if(
_tmp4C4 <= (void*)3?1:*((int*)_tmp4C4)!= 5)goto _LL3C2;_tmp4C5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C4)->f1;if((int)_tmp4C5 != 1)goto _LL3C2;_tmp4C6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C4)->f2;if((int)_tmp4C6 != 3)goto _LL3C2;_LL3C1: return Cyc_Absyn_ulonglong_typ;
_LL3C2: _tmp4C7=_tmp4B8.f1;if(_tmp4C7 <= (void*)3?1:*((int*)_tmp4C7)!= 5)goto
_LL3C4;_tmp4C8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C7)->f1;if((int)
_tmp4C8 != 2)goto _LL3C4;_tmp4C9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C7)->f2;
if((int)_tmp4C9 != 3)goto _LL3C4;_LL3C3: goto _LL3C5;_LL3C4: _tmp4CA=_tmp4B8.f2;if(
_tmp4CA <= (void*)3?1:*((int*)_tmp4CA)!= 5)goto _LL3C6;_tmp4CB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4CA)->f1;if((int)_tmp4CB != 2)goto _LL3C6;_tmp4CC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4CA)->f2;if((int)_tmp4CC != 3)goto _LL3C6;_LL3C5: goto _LL3C7;_LL3C6: _tmp4CD=
_tmp4B8.f1;if(_tmp4CD <= (void*)3?1:*((int*)_tmp4CD)!= 5)goto _LL3C8;_tmp4CE=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4CD)->f1;if((int)_tmp4CE != 0)goto _LL3C8;
_tmp4CF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4CD)->f2;if((int)_tmp4CF != 
3)goto _LL3C8;_LL3C7: goto _LL3C9;_LL3C8: _tmp4D0=_tmp4B8.f2;if(_tmp4D0 <= (void*)3?1:*((
int*)_tmp4D0)!= 5)goto _LL3CA;_tmp4D1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4D0)->f1;if((int)_tmp4D1 != 0)goto _LL3CA;_tmp4D2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4D0)->f2;if((int)_tmp4D2 != 3)goto _LL3CA;_LL3C9: return Cyc_Absyn_slonglong_typ;
_LL3CA: _tmp4D3=_tmp4B8.f1;if(_tmp4D3 <= (void*)3?1:*((int*)_tmp4D3)!= 5)goto
_LL3CC;_tmp4D4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4D3)->f1;if((int)
_tmp4D4 != 1)goto _LL3CC;_tmp4D5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4D3)->f2;
if((int)_tmp4D5 != 2)goto _LL3CC;_LL3CB: goto _LL3CD;_LL3CC: _tmp4D6=_tmp4B8.f2;if(
_tmp4D6 <= (void*)3?1:*((int*)_tmp4D6)!= 5)goto _LL3CE;_tmp4D7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4D6)->f1;if((int)_tmp4D7 != 1)goto _LL3CE;_tmp4D8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4D6)->f2;if((int)_tmp4D8 != 2)goto _LL3CE;_LL3CD: return Cyc_Absyn_uint_typ;
_LL3CE:;_LL3CF: return Cyc_Absyn_sint_typ;_LL3B3:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp4D9=(void*)e->r;struct Cyc_Core_Opt*_tmp4DA;
_LL3D1: if(*((int*)_tmp4D9)!= 4)goto _LL3D3;_tmp4DA=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4D9)->f2;if(_tmp4DA != 0)goto _LL3D3;_LL3D2:({void*_tmp4DB[0]={};Cyc_Tcutil_warn(
e->loc,_tag_arr("assignment in test",sizeof(char),19),_tag_arr(_tmp4DB,sizeof(
void*),0));});goto _LL3D0;_LL3D3:;_LL3D4: goto _LL3D0;_LL3D0:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp4DD=({struct _tuple6 _tmp4DC;_tmp4DC.f1=c1;_tmp4DC.f2=c2;_tmp4DC;});
void*_tmp4DE;void*_tmp4DF;void*_tmp4E0;void*_tmp4E1;void*_tmp4E2;struct Cyc_Core_Opt*
_tmp4E3;struct Cyc_Core_Opt**_tmp4E4;void*_tmp4E5;struct Cyc_Core_Opt*_tmp4E6;
struct Cyc_Core_Opt**_tmp4E7;void*_tmp4E8;struct Cyc_Core_Opt*_tmp4E9;struct Cyc_Core_Opt**
_tmp4EA;void*_tmp4EB;void*_tmp4EC;void*_tmp4ED;void*_tmp4EE;void*_tmp4EF;void*
_tmp4F0;struct Cyc_Core_Opt*_tmp4F1;struct Cyc_Core_Opt**_tmp4F2;void*_tmp4F3;void*
_tmp4F4;struct Cyc_Core_Opt*_tmp4F5;struct Cyc_Core_Opt**_tmp4F6;void*_tmp4F7;void*
_tmp4F8;struct Cyc_Core_Opt*_tmp4F9;struct Cyc_Core_Opt**_tmp4FA;void*_tmp4FB;
_LL3D6: _tmp4DE=_tmp4DD.f1;if(*((int*)_tmp4DE)!= 0)goto _LL3D8;_tmp4DF=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp4DE)->f1;_tmp4E0=_tmp4DD.f2;if(*((int*)_tmp4E0)
!= 0)goto _LL3D8;_tmp4E1=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4E0)->f1;
_LL3D7: return _tmp4DF == _tmp4E1;_LL3D8: _tmp4E2=_tmp4DD.f2;if(*((int*)_tmp4E2)!= 1)
goto _LL3DA;_tmp4E3=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4E2)->f1;_tmp4E4=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4E2)->f1;_LL3D9:*
_tmp4E4=({struct Cyc_Core_Opt*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC->v=(
void*)c1;_tmp4FC;});return 1;_LL3DA: _tmp4E5=_tmp4DD.f1;if(*((int*)_tmp4E5)!= 1)
goto _LL3DC;_tmp4E6=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4E5)->f1;_tmp4E7=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4E5)->f1;_LL3DB:*
_tmp4E7=({struct Cyc_Core_Opt*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->v=(
void*)c2;_tmp4FD;});return 1;_LL3DC: _tmp4E8=_tmp4DD.f1;if(*((int*)_tmp4E8)!= 2)
goto _LL3DE;_tmp4E9=((struct Cyc_Absyn_Less_kb_struct*)_tmp4E8)->f1;_tmp4EA=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp4E8)->f1;_tmp4EB=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4E8)->f2;_tmp4EC=_tmp4DD.f2;if(*((
int*)_tmp4EC)!= 0)goto _LL3DE;_tmp4ED=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp4EC)->f1;_LL3DD: if(Cyc_Tcutil_kind_leq(_tmp4ED,_tmp4EB)){*_tmp4EA=({struct
Cyc_Core_Opt*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->v=(void*)c2;_tmp4FE;});
return 1;}else{return 0;}_LL3DE: _tmp4EE=_tmp4DD.f1;if(*((int*)_tmp4EE)!= 0)goto
_LL3E0;_tmp4EF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4EE)->f1;_tmp4F0=
_tmp4DD.f2;if(*((int*)_tmp4F0)!= 2)goto _LL3E0;_tmp4F1=((struct Cyc_Absyn_Less_kb_struct*)
_tmp4F0)->f1;_tmp4F2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp4F0)->f1;_tmp4F3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4F0)->f2;
_LL3DF: if(Cyc_Tcutil_kind_leq(_tmp4EF,_tmp4F3)){*_tmp4F2=({struct Cyc_Core_Opt*
_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->v=(void*)c1;_tmp4FF;});return 1;}
else{return 0;}_LL3E0: _tmp4F4=_tmp4DD.f1;if(*((int*)_tmp4F4)!= 2)goto _LL3D5;
_tmp4F5=((struct Cyc_Absyn_Less_kb_struct*)_tmp4F4)->f1;_tmp4F6=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4F4)->f1;_tmp4F7=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4F4)->f2;_tmp4F8=_tmp4DD.f2;if(*((int*)_tmp4F8)!= 2)goto _LL3D5;_tmp4F9=((
struct Cyc_Absyn_Less_kb_struct*)_tmp4F8)->f1;_tmp4FA=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4F8)->f1;_tmp4FB=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4F8)->f2;_LL3E1: if(Cyc_Tcutil_kind_leq(_tmp4F7,_tmp4FB)){*_tmp4FA=({struct
Cyc_Core_Opt*_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->v=(void*)c1;_tmp500;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp4FB,_tmp4F7)){*_tmp4F6=({struct Cyc_Core_Opt*
_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501->v=(void*)c2;_tmp501;});return 1;}
else{return 0;}}_LL3D5:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp502=_cycalloc_atomic(sizeof(*_tmp502));_tmp502[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp502;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp506;_tmp506.tag=1;_tmp506.f1=(int)((unsigned int)
i);{void*_tmp505[1]={& _tmp506};Cyc_Std_aprintf(_tag_arr("#%d",sizeof(char),4),
_tag_arr(_tmp505,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*_tmp503=
_cycalloc(sizeof(*_tmp503));_tmp503->name=({struct _tagged_arr*_tmp504=_cycalloc(
sizeof(struct _tagged_arr)* 1);_tmp504[0]=s;_tmp504;});_tmp503->identity=0;
_tmp503->kind=(void*)k;_tmp503;});}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _tagged_arr _tmp507=*t->name;return*((const char*)
_check_unknown_subscript(_tmp507,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_Std_String_pa_struct _tmp509;_tmp509.tag=0;
_tmp509.f1=(struct _tagged_arr)*t->name;{void*_tmp508[1]={& _tmp509};Cyc_Std_printf(
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp508,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp50A=Cyc_Std_strconcat(_tag_arr("`",sizeof(char),
2),*t->name);*((char*)_check_unknown_subscript(_tmp50A,sizeof(char),1))='t';t->name=({
struct _tagged_arr*_tmp50B=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp50B[0]=(
struct _tagged_arr)_tmp50A;_tmp50B;});}}struct _tuple13{struct _tagged_arr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple13*x){return({struct _tuple2*_tmp50C=_cycalloc(sizeof(*_tmp50C));
_tmp50C->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp50D=_cycalloc(sizeof(*
_tmp50D));_tmp50D->v=(*x).f1;_tmp50D;});_tmp50C->f2=(*x).f2;_tmp50C->f3=(*x).f3;
_tmp50C;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp50E=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp50E=({
struct Cyc_List_List*_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F->hd=(void*)((
void*)atts->hd);_tmp50F->tl=_tmp50E;_tmp50F;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510[0]=({struct Cyc_Absyn_FnType_struct
_tmp511;_tmp511.tag=8;_tmp511.f1=({struct Cyc_Absyn_FnInfo _tmp512;_tmp512.tvars=
fd->tvs;_tmp512.effect=fd->effect;_tmp512.ret_typ=(void*)((void*)fd->ret_type);
_tmp512.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp512.c_varargs=
fd->c_varargs;_tmp512.cyc_varargs=fd->cyc_varargs;_tmp512.rgn_po=fd->rgn_po;
_tmp512.attributes=_tmp50E;_tmp512;});_tmp511;});_tmp510;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple14{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple14*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513->f1=(*pr).f1;_tmp513->f2=t;_tmp513;});}
struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple16{
struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp514=_region_malloc(
rgn,sizeof(*_tmp514));_tmp514->f1=({struct _tuple15*_tmp515=_region_malloc(rgn,
sizeof(*_tmp515));_tmp515->f1=(*y).f1;_tmp515->f2=(*y).f2;_tmp515;});_tmp514->f2=(*
y).f3;_tmp514;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple16*w){
struct _tuple15*_tmp517;void*_tmp518;struct _tuple16 _tmp516=*w;_tmp517=_tmp516.f1;
_tmp518=_tmp516.f2;{struct Cyc_Core_Opt*_tmp51A;struct Cyc_Absyn_Tqual _tmp51B;
struct _tuple15 _tmp519=*_tmp517;_tmp51A=_tmp519.f1;_tmp51B=_tmp519.f2;return({
struct _tuple2*_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C->f1=_tmp51A;_tmp51C->f2=
_tmp51B;_tmp51C->f3=_tmp518;_tmp51C;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp51D=
_cycalloc(sizeof(*_tmp51D));_tmp51D->name=f->name;_tmp51D->tq=f->tq;_tmp51D->type=(
void*)t;_tmp51D->width=f->width;_tmp51D->attributes=f->attributes;_tmp51D;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp51E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp51F;struct Cyc_Absyn_AggrInfo _tmp520;void*_tmp521;struct Cyc_List_List*_tmp522;
struct Cyc_Absyn_TunionInfo _tmp523;void*_tmp524;struct Cyc_List_List*_tmp525;void*
_tmp526;struct Cyc_Absyn_TunionFieldInfo _tmp527;void*_tmp528;struct Cyc_List_List*
_tmp529;struct _tuple1*_tmp52A;struct Cyc_List_List*_tmp52B;struct Cyc_Absyn_Typedefdecl*
_tmp52C;struct Cyc_Core_Opt*_tmp52D;void*_tmp52E;struct Cyc_Absyn_Tqual _tmp52F;
struct Cyc_Absyn_Exp*_tmp530;struct Cyc_Absyn_PtrInfo _tmp531;void*_tmp532;void*
_tmp533;struct Cyc_Absyn_Conref*_tmp534;struct Cyc_Absyn_Tqual _tmp535;struct Cyc_Absyn_Conref*
_tmp536;struct Cyc_Absyn_FnInfo _tmp537;struct Cyc_List_List*_tmp538;struct Cyc_Core_Opt*
_tmp539;void*_tmp53A;struct Cyc_List_List*_tmp53B;int _tmp53C;struct Cyc_Absyn_VarargInfo*
_tmp53D;struct Cyc_List_List*_tmp53E;struct Cyc_List_List*_tmp53F;struct Cyc_List_List*
_tmp540;void*_tmp541;struct Cyc_List_List*_tmp542;struct Cyc_Core_Opt*_tmp543;void*
_tmp544;void*_tmp545;void*_tmp546;void*_tmp547;void*_tmp548;struct Cyc_List_List*
_tmp549;_LL3E3: if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 1)goto _LL3E5;_tmp51F=((
struct Cyc_Absyn_VarType_struct*)_tmp51E)->f1;_LL3E4: {struct _handler_cons _tmp54A;
_push_handler(& _tmp54A);{int _tmp54C=0;if(setjmp(_tmp54A.handler))_tmp54C=1;if(!
_tmp54C){{void*_tmp54D=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp51F);_npop_handler(0);return _tmp54D;};_pop_handler();}else{void*_tmp54B=(
void*)_exn_thrown;void*_tmp54F=_tmp54B;_LL416: if(_tmp54F != Cyc_Core_Not_found)
goto _LL418;_LL417: return t;_LL418:;_LL419:(void)_throw(_tmp54F);_LL415:;}}}_LL3E5:
if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 10)goto _LL3E7;_tmp520=((struct Cyc_Absyn_AggrType_struct*)
_tmp51E)->f1;_tmp521=(void*)_tmp520.aggr_info;_tmp522=_tmp520.targs;_LL3E6: {
struct Cyc_List_List*_tmp550=Cyc_Tcutil_substs(rgn,inst,_tmp522);return _tmp550 == 
_tmp522?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp551=_cycalloc(sizeof(*
_tmp551));_tmp551[0]=({struct Cyc_Absyn_AggrType_struct _tmp552;_tmp552.tag=10;
_tmp552.f1=({struct Cyc_Absyn_AggrInfo _tmp553;_tmp553.aggr_info=(void*)_tmp521;
_tmp553.targs=_tmp550;_tmp553;});_tmp552;});_tmp551;});}_LL3E7: if(_tmp51E <= (
void*)3?1:*((int*)_tmp51E)!= 2)goto _LL3E9;_tmp523=((struct Cyc_Absyn_TunionType_struct*)
_tmp51E)->f1;_tmp524=(void*)_tmp523.tunion_info;_tmp525=_tmp523.targs;_tmp526=(
void*)_tmp523.rgn;_LL3E8: {struct Cyc_List_List*_tmp554=Cyc_Tcutil_substs(rgn,
inst,_tmp525);void*_tmp555=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp526);return(
_tmp554 == _tmp525?_tmp555 == _tmp526: 0)?t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556[0]=({struct Cyc_Absyn_TunionType_struct
_tmp557;_tmp557.tag=2;_tmp557.f1=({struct Cyc_Absyn_TunionInfo _tmp558;_tmp558.tunion_info=(
void*)_tmp524;_tmp558.targs=_tmp554;_tmp558.rgn=(void*)_tmp555;_tmp558;});
_tmp557;});_tmp556;});}_LL3E9: if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 3)goto
_LL3EB;_tmp527=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp51E)->f1;_tmp528=(
void*)_tmp527.field_info;_tmp529=_tmp527.targs;_LL3EA: {struct Cyc_List_List*
_tmp559=Cyc_Tcutil_substs(rgn,inst,_tmp529);return _tmp559 == _tmp529?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp55A=_cycalloc(sizeof(*_tmp55A));
_tmp55A[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp55B;_tmp55B.tag=3;_tmp55B.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp55C;_tmp55C.field_info=(void*)_tmp528;_tmp55C.targs=
_tmp559;_tmp55C;});_tmp55B;});_tmp55A;});}_LL3EB: if(_tmp51E <= (void*)3?1:*((int*)
_tmp51E)!= 16)goto _LL3ED;_tmp52A=((struct Cyc_Absyn_TypedefType_struct*)_tmp51E)->f1;
_tmp52B=((struct Cyc_Absyn_TypedefType_struct*)_tmp51E)->f2;_tmp52C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp51E)->f3;_tmp52D=((struct Cyc_Absyn_TypedefType_struct*)_tmp51E)->f4;_LL3EC: {
struct Cyc_List_List*_tmp55D=Cyc_Tcutil_substs(rgn,inst,_tmp52B);return _tmp55D == 
_tmp52B?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp55E=_cycalloc(sizeof(*
_tmp55E));_tmp55E[0]=({struct Cyc_Absyn_TypedefType_struct _tmp55F;_tmp55F.tag=16;
_tmp55F.f1=_tmp52A;_tmp55F.f2=_tmp55D;_tmp55F.f3=_tmp52C;_tmp55F.f4=_tmp52D;
_tmp55F;});_tmp55E;});}_LL3ED: if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 7)goto
_LL3EF;_tmp52E=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp51E)->f1;_tmp52F=((
struct Cyc_Absyn_ArrayType_struct*)_tmp51E)->f2;_tmp530=((struct Cyc_Absyn_ArrayType_struct*)
_tmp51E)->f3;_LL3EE: {void*_tmp560=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp52E);
return _tmp560 == _tmp52E?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp561=
_cycalloc(sizeof(*_tmp561));_tmp561[0]=({struct Cyc_Absyn_ArrayType_struct _tmp562;
_tmp562.tag=7;_tmp562.f1=(void*)_tmp560;_tmp562.f2=_tmp52F;_tmp562.f3=_tmp530;
_tmp562;});_tmp561;});}_LL3EF: if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 4)goto
_LL3F1;_tmp531=((struct Cyc_Absyn_PointerType_struct*)_tmp51E)->f1;_tmp532=(void*)
_tmp531.elt_typ;_tmp533=(void*)_tmp531.rgn_typ;_tmp534=_tmp531.nullable;_tmp535=
_tmp531.tq;_tmp536=_tmp531.bounds;_LL3F0: {void*_tmp563=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp532);void*_tmp564=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp533);if(
_tmp563 == _tmp532?_tmp564 == _tmp533: 0)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565[0]=({struct Cyc_Absyn_PointerType_struct
_tmp566;_tmp566.tag=4;_tmp566.f1=({struct Cyc_Absyn_PtrInfo _tmp567;_tmp567.elt_typ=(
void*)_tmp563;_tmp567.rgn_typ=(void*)_tmp564;_tmp567.nullable=_tmp534;_tmp567.tq=
_tmp535;_tmp567.bounds=_tmp536;_tmp567;});_tmp566;});_tmp565;});}_LL3F1: if(
_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 8)goto _LL3F3;_tmp537=((struct Cyc_Absyn_FnType_struct*)
_tmp51E)->f1;_tmp538=_tmp537.tvars;_tmp539=_tmp537.effect;_tmp53A=(void*)_tmp537.ret_typ;
_tmp53B=_tmp537.args;_tmp53C=_tmp537.c_varargs;_tmp53D=_tmp537.cyc_varargs;
_tmp53E=_tmp537.rgn_po;_tmp53F=_tmp537.attributes;_LL3F2:{struct Cyc_List_List*
_tmp568=_tmp538;for(0;_tmp568 != 0;_tmp568=_tmp568->tl){inst=({struct Cyc_List_List*
_tmp569=_region_malloc(rgn,sizeof(*_tmp569));_tmp569->hd=({struct _tuple8*_tmp56A=
_region_malloc(rgn,sizeof(*_tmp56A));_tmp56A->f1=(struct Cyc_Absyn_Tvar*)_tmp568->hd;
_tmp56A->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp56B=_cycalloc(sizeof(*
_tmp56B));_tmp56B[0]=({struct Cyc_Absyn_VarType_struct _tmp56C;_tmp56C.tag=1;
_tmp56C.f1=(struct Cyc_Absyn_Tvar*)_tmp568->hd;_tmp56C;});_tmp56B;});_tmp56A;});
_tmp569->tl=inst;_tmp569;});}}{struct Cyc_List_List*_tmp56E;struct Cyc_List_List*
_tmp56F;struct _tuple0 _tmp56D=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple16*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp53B));_tmp56E=_tmp56D.f1;_tmp56F=_tmp56D.f2;{struct Cyc_List_List*_tmp570=
Cyc_Tcutil_substs(rgn,inst,_tmp56F);struct Cyc_List_List*_tmp571=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp56E,_tmp570));struct Cyc_Core_Opt*
eff2;if(_tmp539 == 0)eff2=0;else{void*_tmp572=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp539->v);if(_tmp572 == (void*)_tmp539->v)eff2=_tmp539;else{eff2=({struct
Cyc_Core_Opt*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->v=(void*)_tmp572;
_tmp573;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp53D == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp575;struct Cyc_Absyn_Tqual _tmp576;void*
_tmp577;int _tmp578;struct Cyc_Absyn_VarargInfo _tmp574=*_tmp53D;_tmp575=_tmp574.name;
_tmp576=_tmp574.tq;_tmp577=(void*)_tmp574.type;_tmp578=_tmp574.inject;{void*
_tmp579=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp577);if(_tmp579 == _tmp577)
cyc_varargs2=_tmp53D;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp57A=
_cycalloc(sizeof(*_tmp57A));_tmp57A->name=_tmp575;_tmp57A->tq=_tmp576;_tmp57A->type=(
void*)_tmp579;_tmp57A->inject=_tmp578;_tmp57A;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp57C;struct Cyc_List_List*_tmp57D;struct _tuple0 _tmp57B=Cyc_List_rsplit(
rgn,rgn,_tmp53E);_tmp57C=_tmp57B.f1;_tmp57D=_tmp57B.f2;{struct Cyc_List_List*
_tmp57E=Cyc_Tcutil_substs(rgn,inst,_tmp57C);struct Cyc_List_List*_tmp57F=Cyc_Tcutil_substs(
rgn,inst,_tmp57D);if(_tmp57E == _tmp57C?_tmp57F == _tmp57D: 0)rgn_po2=_tmp53E;else{
rgn_po2=Cyc_List_zip(_tmp57E,_tmp57F);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580[0]=({struct Cyc_Absyn_FnType_struct
_tmp581;_tmp581.tag=8;_tmp581.f1=({struct Cyc_Absyn_FnInfo _tmp582;_tmp582.tvars=
_tmp538;_tmp582.effect=eff2;_tmp582.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp53A);_tmp582.args=_tmp571;_tmp582.c_varargs=_tmp53C;_tmp582.cyc_varargs=
cyc_varargs2;_tmp582.rgn_po=rgn_po2;_tmp582.attributes=_tmp53F;_tmp582;});
_tmp581;});_tmp580;});}}}}}_LL3F3: if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 9)
goto _LL3F5;_tmp540=((struct Cyc_Absyn_TupleType_struct*)_tmp51E)->f1;_LL3F4: {
struct Cyc_List_List*_tmp583=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp540);struct Cyc_List_List*_tmp584=Cyc_Tcutil_substs(rgn,inst,_tmp583);if(
_tmp584 == _tmp583)return t;{struct Cyc_List_List*_tmp585=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp540,_tmp584);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586[0]=({struct Cyc_Absyn_TupleType_struct
_tmp587;_tmp587.tag=9;_tmp587.f1=_tmp585;_tmp587;});_tmp586;});}}_LL3F5: if(
_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 11)goto _LL3F7;_tmp541=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp51E)->f1;_tmp542=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp51E)->f2;_LL3F6: {
struct Cyc_List_List*_tmp588=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp542);struct Cyc_List_List*_tmp589=Cyc_Tcutil_substs(rgn,inst,_tmp588);if(
_tmp589 == _tmp588)return t;{struct Cyc_List_List*_tmp58A=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp542,
_tmp589);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp58B=_cycalloc(
sizeof(*_tmp58B));_tmp58B[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp58C;
_tmp58C.tag=11;_tmp58C.f1=(void*)_tmp541;_tmp58C.f2=_tmp58A;_tmp58C;});_tmp58B;});}}
_LL3F7: if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 0)goto _LL3F9;_tmp543=((struct
Cyc_Absyn_Evar_struct*)_tmp51E)->f2;_LL3F8: if(_tmp543 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp543->v);else{return t;}_LL3F9: if(_tmp51E <= (void*)3?1:*((int*)
_tmp51E)!= 15)goto _LL3FB;_tmp544=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp51E)->f1;_LL3FA: {void*_tmp58D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp544);
return _tmp58D == _tmp544?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp58E=
_cycalloc(sizeof(*_tmp58E));_tmp58E[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp58F;_tmp58F.tag=15;_tmp58F.f1=(void*)_tmp58D;_tmp58F;});_tmp58E;});}_LL3FB:
if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 14)goto _LL3FD;_tmp545=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp51E)->f1;_LL3FC: {void*_tmp590=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp545);return _tmp590 == _tmp545?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp592;_tmp592.tag=14;_tmp592.f1=(void*)_tmp590;_tmp592;});_tmp591;});}_LL3FD:
if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 17)goto _LL3FF;_tmp546=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp51E)->f1;_LL3FE: {void*_tmp593=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp546);return _tmp593 == _tmp546?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594[0]=({struct Cyc_Absyn_TagType_struct
_tmp595;_tmp595.tag=17;_tmp595.f1=(void*)_tmp593;_tmp595;});_tmp594;});}_LL3FF:
if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 18)goto _LL401;_LL400: goto _LL402;
_LL401: if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 12)goto _LL403;_LL402: goto
_LL404;_LL403: if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 13)goto _LL405;_LL404:
goto _LL406;_LL405: if((int)_tmp51E != 0)goto _LL407;_LL406: goto _LL408;_LL407: if(
_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 5)goto _LL409;_LL408: goto _LL40A;_LL409:
if((int)_tmp51E != 1)goto _LL40B;_LL40A: goto _LL40C;_LL40B: if(_tmp51E <= (void*)3?1:*((
int*)_tmp51E)!= 6)goto _LL40D;_LL40C: goto _LL40E;_LL40D: if((int)_tmp51E != 2)goto
_LL40F;_LL40E: return t;_LL40F: if(_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 21)goto
_LL411;_tmp547=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp51E)->f1;_LL410: {
void*_tmp596=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp547);return _tmp596 == _tmp547?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp597=_cycalloc(sizeof(*_tmp597));
_tmp597[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp598;_tmp598.tag=21;_tmp598.f1=(
void*)_tmp596;_tmp598;});_tmp597;});}_LL411: if(_tmp51E <= (void*)3?1:*((int*)
_tmp51E)!= 19)goto _LL413;_tmp548=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp51E)->f1;_LL412: {void*_tmp599=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp548);
return _tmp599 == _tmp548?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp59A=
_cycalloc(sizeof(*_tmp59A));_tmp59A[0]=({struct Cyc_Absyn_AccessEff_struct _tmp59B;
_tmp59B.tag=19;_tmp59B.f1=(void*)_tmp599;_tmp59B;});_tmp59A;});}_LL413: if(
_tmp51E <= (void*)3?1:*((int*)_tmp51E)!= 20)goto _LL3E2;_tmp549=((struct Cyc_Absyn_JoinEff_struct*)
_tmp51E)->f1;_LL414: {struct Cyc_List_List*_tmp59C=Cyc_Tcutil_substs(rgn,inst,
_tmp549);return _tmp59C == _tmp549?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp59E;_tmp59E.tag=20;_tmp59E.f1=_tmp59C;_tmp59E;});_tmp59D;});}_LL3E2:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp59F=(void*)ts->hd;
struct Cyc_List_List*_tmp5A0=ts->tl;void*_tmp5A1=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp59F);struct Cyc_List_List*_tmp5A2=Cyc_Tcutil_substs(rgn,inst,_tmp5A0);if(
_tmp59F == _tmp5A1?_tmp5A0 == _tmp5A2: 0)return ts;return(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->hd=(void*)
_tmp5A1;_tmp5A3->tl=_tmp5A2;_tmp5A3;});}}extern void*Cyc_Tcutil_substitute(struct
Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp5A4=_cycalloc(
sizeof(*_tmp5A4));_tmp5A4->f1=tv;_tmp5A4->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->v=(void*)k;_tmp5A5;}),({struct Cyc_Core_Opt*
_tmp5A6=_cycalloc(sizeof(*_tmp5A6));_tmp5A6->v=s;_tmp5A6;}));_tmp5A4;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp5A8;struct Cyc_List_List*_tmp5A9;struct _RegionHandle*_tmp5AA;
struct _tuple9*_tmp5A7=env;_tmp5A8=*_tmp5A7;_tmp5A9=_tmp5A8.f1;_tmp5AA=_tmp5A8.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp5AB=_region_malloc(
_tmp5AA,sizeof(*_tmp5AB));_tmp5AB->f1=tv;_tmp5AB->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC->v=(void*)k;_tmp5AC;}),({
struct Cyc_Core_Opt*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD->v=_tmp5A9;
_tmp5AD;}));_tmp5AB;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_Std_zstrptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_Std_String_pa_struct _tmp5B1;_tmp5B1.tag=0;_tmp5B1.f1=(struct
_tagged_arr)Cyc_Absynpp_kindbound2string(k2);{struct Cyc_Std_String_pa_struct
_tmp5B0;_tmp5B0.tag=0;_tmp5B0.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k1);{struct Cyc_Std_String_pa_struct _tmp5AF;_tmp5AF.tag=0;_tmp5AF.f1=(struct
_tagged_arr)*tv->name;{void*_tmp5AE[3]={& _tmp5AF,& _tmp5B0,& _tmp5B1};Cyc_Tcutil_terr(
loc,_tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof(char),59),_tag_arr(_tmp5AE,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp5B2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("same type variable has different identity!",sizeof(char),43),_tag_arr(
_tmp5B2,sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->hd=tv;
_tmp5B3->tl=tvs;_tmp5B3;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*
_tmp5B4[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv",sizeof(char),39),_tag_arr(
_tmp5B4,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp5B5[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof(char),41),_tag_arr(_tmp5B5,sizeof(void*),0));});if(*((int*)_check_null(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)_check_null(tv->identity)))
return tvs;}}return({struct Cyc_List_List*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));
_tmp5B6->hd=tv;_tmp5B6->tl=tvs;_tmp5B6;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_Std_String_pa_struct
_tmp5B8;_tmp5B8.tag=0;_tmp5B8.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string(tv);{
void*_tmp5B7[1]={& _tmp5B8};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bound tvar id for %s is NULL",sizeof(char),29),
_tag_arr(_tmp5B7,sizeof(void*),1));}});return({struct Cyc_List_List*_tmp5B9=
_cycalloc(sizeof(*_tmp5B9));_tmp5B9->hd=tv;_tmp5B9->tl=tvs;_tmp5B9;});}static
struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){void*
_tmp5BA=Cyc_Tcutil_compress(e);int _tmp5BB;_LL41B: if(_tmp5BA <= (void*)3?1:*((int*)
_tmp5BA)!= 0)goto _LL41D;_tmp5BB=((struct Cyc_Absyn_Evar_struct*)_tmp5BA)->f3;
_LL41C:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*_tmp5BC=Cyc_Tcutil_compress((
void*)es2->hd);int _tmp5BD;_LL420: if(_tmp5BC <= (void*)3?1:*((int*)_tmp5BC)!= 0)
goto _LL422;_tmp5BD=((struct Cyc_Absyn_Evar_struct*)_tmp5BC)->f3;_LL421: if(_tmp5BB
== _tmp5BD)return es;goto _LL41F;_LL422:;_LL423: goto _LL41F;_LL41F:;}}return({
struct Cyc_List_List*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->hd=(void*)e;
_tmp5BE->tl=es;_tmp5BE;});_LL41D:;_LL41E: return es;_LL41A:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))
== *((int*)_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;
break;}}}if(!present)r=({struct Cyc_List_List*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));
_tmp5BF->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp5BF->tl=r;_tmp5BF;});}r=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(width)))({struct Cyc_Std_String_pa_struct
_tmp5C1;_tmp5C1.tag=0;_tmp5C1.f1=(struct _tagged_arr)*fn;{void*_tmp5C0[1]={&
_tmp5C1};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s does not have constant width",
sizeof(char),41),_tag_arr(_tmp5C0,sizeof(void*),1));}});else{unsigned int _tmp5C3;
int _tmp5C4;struct _tuple7 _tmp5C2=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(width));_tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;if(!_tmp5C4)({void*
_tmp5C5[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield width cannot use sizeof or offsetof",
sizeof(char),45),_tag_arr(_tmp5C5,sizeof(void*),0));});w=_tmp5C3;}{void*_tmp5C6=
Cyc_Tcutil_compress(field_typ);void*_tmp5C7;_LL425: if(_tmp5C6 <= (void*)3?1:*((
int*)_tmp5C6)!= 5)goto _LL427;_tmp5C7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C6)->f2;_LL426:{void*_tmp5C8=_tmp5C7;_LL42A: if((int)_tmp5C8 != 0)goto _LL42C;
_LL42B: if(w > 8)({void*_tmp5C9[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(char),26),_tag_arr(_tmp5C9,sizeof(void*),0));});goto _LL429;_LL42C: if((int)
_tmp5C8 != 1)goto _LL42E;_LL42D: if(w > 16)({void*_tmp5CA[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp5CA,sizeof(
void*),0));});goto _LL429;_LL42E: if((int)_tmp5C8 != 2)goto _LL430;_LL42F: if(w > 32)({
void*_tmp5CB[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(char),26),_tag_arr(_tmp5CB,sizeof(void*),0));});goto _LL429;_LL430: if((int)
_tmp5C8 != 3)goto _LL429;_LL431: if(w > 64)({void*_tmp5CC[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp5CC,sizeof(
void*),0));});goto _LL429;_LL429:;}goto _LL424;_LL427:;_LL428:({struct Cyc_Std_String_pa_struct
_tmp5CF;_tmp5CF.tag=0;_tmp5CF.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
field_typ);{struct Cyc_Std_String_pa_struct _tmp5CE;_tmp5CE.tag=0;_tmp5CE.f1=(
struct _tagged_arr)*fn;{void*_tmp5CD[2]={& _tmp5CE,& _tmp5CF};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield %s must have integral type but has type %s",sizeof(char),52),
_tag_arr(_tmp5CD,sizeof(void*),2));}}});goto _LL424;_LL424:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp5D0=(void*)atts->hd;_LL433: if((int)
_tmp5D0 != 5)goto _LL435;_LL434: continue;_LL435: if(_tmp5D0 <= (void*)16?1:*((int*)
_tmp5D0)!= 1)goto _LL437;_LL436: continue;_LL437:;_LL438:({struct Cyc_Std_String_pa_struct
_tmp5D3;_tmp5D3.tag=0;_tmp5D3.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_String_pa_struct
_tmp5D2;_tmp5D2.tag=0;_tmp5D2.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp5D1[2]={& _tmp5D2,& _tmp5D3};Cyc_Tcutil_terr(loc,
_tag_arr("bad attribute %s on member %s",sizeof(char),30),_tag_arr(_tmp5D1,
sizeof(void*),2));}}});_LL432:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t){{void*_tmp5D4=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp5D5;struct Cyc_Core_Opt**_tmp5D6;struct Cyc_Core_Opt*
_tmp5D7;struct Cyc_Core_Opt**_tmp5D8;struct Cyc_Absyn_Tvar*_tmp5D9;struct Cyc_List_List*
_tmp5DA;struct _tuple1*_tmp5DB;struct Cyc_Absyn_Enumdecl*_tmp5DC;struct Cyc_Absyn_Enumdecl**
_tmp5DD;struct Cyc_Absyn_TunionInfo _tmp5DE;void*_tmp5DF;void**_tmp5E0;struct Cyc_List_List*
_tmp5E1;struct Cyc_List_List**_tmp5E2;void*_tmp5E3;struct Cyc_Absyn_TunionFieldInfo
_tmp5E4;void*_tmp5E5;void**_tmp5E6;struct Cyc_List_List*_tmp5E7;struct Cyc_Absyn_PtrInfo
_tmp5E8;void*_tmp5E9;void*_tmp5EA;struct Cyc_Absyn_Conref*_tmp5EB;struct Cyc_Absyn_Tqual
_tmp5EC;struct Cyc_Absyn_Conref*_tmp5ED;void*_tmp5EE;void*_tmp5EF;void*_tmp5F0;
struct Cyc_Absyn_Tqual _tmp5F1;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_FnInfo
_tmp5F3;struct Cyc_List_List*_tmp5F4;struct Cyc_List_List**_tmp5F5;struct Cyc_Core_Opt*
_tmp5F6;struct Cyc_Core_Opt**_tmp5F7;void*_tmp5F8;struct Cyc_List_List*_tmp5F9;int
_tmp5FA;struct Cyc_Absyn_VarargInfo*_tmp5FB;struct Cyc_List_List*_tmp5FC;struct Cyc_List_List*
_tmp5FD;struct Cyc_List_List*_tmp5FE;void*_tmp5FF;struct Cyc_List_List*_tmp600;
struct Cyc_Absyn_AggrInfo _tmp601;void*_tmp602;void**_tmp603;struct Cyc_List_List*
_tmp604;struct Cyc_List_List**_tmp605;struct _tuple1*_tmp606;struct Cyc_List_List*
_tmp607;struct Cyc_List_List**_tmp608;struct Cyc_Absyn_Typedefdecl*_tmp609;struct
Cyc_Absyn_Typedefdecl**_tmp60A;struct Cyc_Core_Opt*_tmp60B;struct Cyc_Core_Opt**
_tmp60C;void*_tmp60D;void*_tmp60E;void*_tmp60F;struct Cyc_List_List*_tmp610;
_LL43A: if((int)_tmp5D4 != 0)goto _LL43C;_LL43B: goto _LL439;_LL43C: if(_tmp5D4 <= (
void*)3?1:*((int*)_tmp5D4)!= 0)goto _LL43E;_tmp5D5=((struct Cyc_Absyn_Evar_struct*)
_tmp5D4)->f1;_tmp5D6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp5D4)->f1;_tmp5D7=((struct Cyc_Absyn_Evar_struct*)_tmp5D4)->f2;_tmp5D8=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp5D4)->f2;_LL43D: if(*_tmp5D6 == 
0)*_tmp5D6=({struct Cyc_Core_Opt*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->v=(
void*)expected_kind;_tmp611;});if((cvtenv.fn_result?cvtenv.generalize_evars: 0)?
expected_kind == (void*)3: 0)*_tmp5D8=({struct Cyc_Core_Opt*_tmp612=_cycalloc(
sizeof(*_tmp612));_tmp612->v=(void*)((void*)2);_tmp612;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp613=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp618;_tmp618.tag=2;_tmp618.f1=0;_tmp618.f2=(void*)expected_kind;_tmp618;});
_tmp617;}));*_tmp5D8=({struct Cyc_Core_Opt*_tmp614=_cycalloc(sizeof(*_tmp614));
_tmp614->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp615=_cycalloc(
sizeof(*_tmp615));_tmp615[0]=({struct Cyc_Absyn_VarType_struct _tmp616;_tmp616.tag=
1;_tmp616.f1=_tmp613;_tmp616;});_tmp615;}));_tmp614;});_tmp5D9=_tmp613;goto
_LL43F;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL439;_LL43E: if(_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 1)goto _LL440;
_tmp5D9=((struct Cyc_Absyn_VarType_struct*)_tmp5D4)->f1;_LL43F:{void*_tmp619=Cyc_Absyn_compress_kb((
void*)_tmp5D9->kind);struct Cyc_Core_Opt*_tmp61A;struct Cyc_Core_Opt**_tmp61B;
_LL46D: if(*((int*)_tmp619)!= 1)goto _LL46F;_tmp61A=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp619)->f1;_tmp61B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp619)->f1;_LL46E:*_tmp61B=({struct Cyc_Core_Opt*_tmp61C=_cycalloc(sizeof(*
_tmp61C));_tmp61C->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp61D=
_cycalloc(sizeof(*_tmp61D));_tmp61D[0]=({struct Cyc_Absyn_Less_kb_struct _tmp61E;
_tmp61E.tag=2;_tmp61E.f1=0;_tmp61E.f2=(void*)expected_kind;_tmp61E;});_tmp61D;}));
_tmp61C;});goto _LL46C;_LL46F:;_LL470: goto _LL46C;_LL46C:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp5D9);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp5D9);goto _LL439;_LL440: if(_tmp5D4 <= (void*)3?1:*((int*)
_tmp5D4)!= 13)goto _LL442;_tmp5DA=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp5D4)->f1;
_LL441: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp61F=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp61F;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp5DA != 0;_tmp5DA=_tmp5DA->tl){struct Cyc_Absyn_Enumfield*_tmp620=(
struct Cyc_Absyn_Enumfield*)_tmp5DA->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp620->name).f2))({struct Cyc_Std_String_pa_struct
_tmp622;_tmp622.tag=0;_tmp622.f1=(struct _tagged_arr)*(*_tmp620->name).f2;{void*
_tmp621[1]={& _tmp622};Cyc_Tcutil_terr(_tmp620->loc,_tag_arr("duplicate enum field name %s",
sizeof(char),29),_tag_arr(_tmp621,sizeof(void*),1));}});else{prev_fields=({
struct Cyc_List_List*_tmp623=_region_malloc(uprev_rgn,sizeof(*_tmp623));_tmp623->hd=(*
_tmp620->name).f2;_tmp623->tl=prev_fields;_tmp623;});}if(_tmp620->tag == 0)
_tmp620->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp620->loc);
else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp620->tag)))({
struct Cyc_Std_String_pa_struct _tmp625;_tmp625.tag=0;_tmp625.f1=(struct
_tagged_arr)*(*_tmp620->name).f2;{void*_tmp624[1]={& _tmp625};Cyc_Tcutil_terr(loc,
_tag_arr("enum field %s: expression is not constant",sizeof(char),42),_tag_arr(
_tmp624,sizeof(void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp620->tag))).f1;tag_count=t1 + 1;(*_tmp620->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp627;_tmp627.tag=1;_tmp627.f1=te->ns;_tmp627;});
_tmp626;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp620->name).f2,({
struct _tuple17*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp62A;_tmp62A.tag=4;_tmp62A.f1=(void*)t;
_tmp62A.f2=_tmp620;_tmp62A;});_tmp629;});_tmp628->f2=1;_tmp628;}));}}};
_pop_region(uprev_rgn);}goto _LL439;}_LL442: if(_tmp5D4 <= (void*)3?1:*((int*)
_tmp5D4)!= 12)goto _LL444;_tmp5DB=((struct Cyc_Absyn_EnumType_struct*)_tmp5D4)->f1;
_tmp5DC=((struct Cyc_Absyn_EnumType_struct*)_tmp5D4)->f2;_tmp5DD=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp5D4)->f2;_LL443: if(*_tmp5DD == 0?1:((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp5DD))->fields == 0){struct _handler_cons
_tmp62B;_push_handler(& _tmp62B);{int _tmp62D=0;if(setjmp(_tmp62B.handler))_tmp62D=
1;if(!_tmp62D){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp5DB);*_tmp5DD=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp62C=(void*)_exn_thrown;void*_tmp62F=_tmp62C;_LL472: if(_tmp62F != Cyc_Dict_Absent)
goto _LL474;_LL473: {struct Cyc_Tcenv_Genv*_tmp630=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp631=({struct Cyc_Absyn_Enumdecl*_tmp632=_cycalloc(
sizeof(*_tmp632));_tmp632->sc=(void*)((void*)3);_tmp632->name=_tmp5DB;_tmp632->fields=
0;_tmp632;});Cyc_Tc_tcEnumdecl(te,_tmp630,loc,_tmp631);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp5DB);*_tmp5DD=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL471;}}_LL474:;_LL475:(void)_throw(_tmp62F);_LL471:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp5DD);*_tmp5DB=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL439;}_LL444: if(_tmp5D4 <= (void*)3?1:*((
int*)_tmp5D4)!= 2)goto _LL446;_tmp5DE=((struct Cyc_Absyn_TunionType_struct*)
_tmp5D4)->f1;_tmp5DF=(void*)_tmp5DE.tunion_info;_tmp5E0=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp5D4)->f1).tunion_info;_tmp5E1=_tmp5DE.targs;_tmp5E2=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp5D4)->f1).targs;_tmp5E3=(void*)_tmp5DE.rgn;
_LL445: {struct Cyc_List_List*_tmp633=*_tmp5E2;{void*_tmp634=*_tmp5E0;struct Cyc_Absyn_UnknownTunionInfo
_tmp635;struct _tuple1*_tmp636;int _tmp637;struct Cyc_Absyn_Tuniondecl**_tmp638;
struct Cyc_Absyn_Tuniondecl*_tmp639;_LL477: if(*((int*)_tmp634)!= 0)goto _LL479;
_tmp635=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp634)->f1;_tmp636=_tmp635.name;
_tmp637=_tmp635.is_xtunion;_LL478: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp63A;_push_handler(& _tmp63A);{int _tmp63C=0;if(setjmp(_tmp63A.handler))
_tmp63C=1;if(!_tmp63C){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp636);;
_pop_handler();}else{void*_tmp63B=(void*)_exn_thrown;void*_tmp63E=_tmp63B;_LL47C:
if(_tmp63E != Cyc_Dict_Absent)goto _LL47E;_LL47D: {struct Cyc_Tcenv_Genv*_tmp63F=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp640=({struct Cyc_Absyn_Tuniondecl*
_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644->sc=(void*)((void*)3);_tmp644->name=
_tmp636;_tmp644->tvs=0;_tmp644->fields=0;_tmp644->is_xtunion=_tmp637;_tmp644;});
Cyc_Tc_tcTuniondecl(te,_tmp63F,loc,_tmp640);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp636);if(_tmp633 != 0){({struct Cyc_Std_String_pa_struct _tmp643;_tmp643.tag=
0;_tmp643.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp636);{struct Cyc_Std_String_pa_struct
_tmp642;_tmp642.tag=0;_tmp642.f1=(struct _tagged_arr)(_tmp637?_tag_arr("xtunion",
sizeof(char),8): _tag_arr("tunion",sizeof(char),7));{void*_tmp641[2]={& _tmp642,&
_tmp643};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized %s %s before using",
sizeof(char),48),_tag_arr(_tmp641,sizeof(void*),2));}}});return cvtenv;}goto
_LL47B;}_LL47E:;_LL47F:(void)_throw(_tmp63E);_LL47B:;}}}if((*tudp)->is_xtunion != 
_tmp637)({struct Cyc_Std_String_pa_struct _tmp646;_tmp646.tag=0;_tmp646.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp636);{void*_tmp645[1]={& _tmp646};Cyc_Tcutil_terr(
loc,_tag_arr("[x]tunion is different from type declaration %s",sizeof(char),48),
_tag_arr(_tmp645,sizeof(void*),1));}});*_tmp5E0=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp648;_tmp648.tag=1;_tmp648.f1=tudp;_tmp648;});_tmp647;});_tmp639=*tudp;goto
_LL47A;}_LL479: if(*((int*)_tmp634)!= 1)goto _LL476;_tmp638=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp634)->f1;_tmp639=*_tmp638;_LL47A: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5E3);{struct Cyc_List_List*tvs=_tmp639->tvs;for(0;_tmp633 != 0?
tvs != 0: 0;(_tmp633=_tmp633->tl,tvs=tvs->tl)){void*t1=(void*)_tmp633->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp633 != 0)({struct Cyc_Std_String_pa_struct _tmp64A;
_tmp64A.tag=0;_tmp64A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp639->name);{
void*_tmp649[1]={& _tmp64A};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s",
sizeof(char),38),_tag_arr(_tmp649,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp64B=
_cycalloc(sizeof(*_tmp64B));_tmp64B->hd=(void*)e;_tmp64B->tl=hidden_ts;_tmp64B;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*_tmp5E2=Cyc_List_imp_append(*
_tmp5E2,Cyc_List_imp_rev(hidden_ts));}goto _LL476;}_LL476:;}goto _LL439;}_LL446:
if(_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 3)goto _LL448;_tmp5E4=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5D4)->f1;_tmp5E5=(void*)_tmp5E4.field_info;_tmp5E6=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5D4)->f1).field_info;_tmp5E7=_tmp5E4.targs;_LL447:{void*_tmp64C=*_tmp5E6;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp64D;struct _tuple1*_tmp64E;struct
_tuple1*_tmp64F;int _tmp650;struct Cyc_Absyn_Tuniondecl*_tmp651;struct Cyc_Absyn_Tunionfield*
_tmp652;_LL481: if(*((int*)_tmp64C)!= 0)goto _LL483;_tmp64D=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp64C)->f1;_tmp64E=_tmp64D.tunion_name;_tmp64F=_tmp64D.field_name;_tmp650=
_tmp64D.is_xtunion;_LL482: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp653;_push_handler(& _tmp653);{int _tmp655=0;if(setjmp(
_tmp653.handler))_tmp655=1;if(!_tmp655){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp64E);;_pop_handler();}else{void*_tmp654=(void*)_exn_thrown;void*_tmp657=
_tmp654;_LL486: if(_tmp657 != Cyc_Dict_Absent)goto _LL488;_LL487:({struct Cyc_Std_String_pa_struct
_tmp659;_tmp659.tag=0;_tmp659.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp64E);{void*_tmp658[1]={& _tmp659};Cyc_Tcutil_terr(loc,_tag_arr("unbound type tunion %s",
sizeof(char),23),_tag_arr(_tmp658,sizeof(void*),1));}});return cvtenv;_LL488:;
_LL489:(void)_throw(_tmp657);_LL485:;}}}{struct _handler_cons _tmp65A;
_push_handler(& _tmp65A);{int _tmp65C=0;if(setjmp(_tmp65A.handler))_tmp65C=1;if(!
_tmp65C){{void*_tmp65D=Cyc_Tcenv_lookup_ordinary(te,loc,_tmp64F);struct Cyc_Absyn_Tuniondecl*
_tmp65E;struct Cyc_Absyn_Tunionfield*_tmp65F;_LL48B: if(*((int*)_tmp65D)!= 2)goto
_LL48D;_tmp65E=((struct Cyc_Tcenv_TunionRes_struct*)_tmp65D)->f1;_tmp65F=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp65D)->f2;_LL48C: tuf=_tmp65F;tud=_tmp65E;if(tud->is_xtunion
!= _tmp650)({struct Cyc_Std_String_pa_struct _tmp661;_tmp661.tag=0;_tmp661.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp64E);{void*_tmp660[1]={& _tmp661};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(char),48),_tag_arr(_tmp660,sizeof(void*),1));}});goto _LL48A;_LL48D:;
_LL48E:({struct Cyc_Std_String_pa_struct _tmp664;_tmp664.tag=0;_tmp664.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp64E);{struct Cyc_Std_String_pa_struct
_tmp663;_tmp663.tag=0;_tmp663.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp64F);{void*_tmp662[2]={& _tmp663,& _tmp664};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp662,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv
_tmp665=cvtenv;_npop_handler(0);return _tmp665;}_LL48A:;};_pop_handler();}else{
void*_tmp65B=(void*)_exn_thrown;void*_tmp667=_tmp65B;_LL490: if(_tmp667 != Cyc_Dict_Absent)
goto _LL492;_LL491:({struct Cyc_Std_String_pa_struct _tmp66A;_tmp66A.tag=0;_tmp66A.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp64E);{struct Cyc_Std_String_pa_struct
_tmp669;_tmp669.tag=0;_tmp669.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp64F);{void*_tmp668[2]={& _tmp669,& _tmp66A};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp668,sizeof(void*),2));}}});return cvtenv;_LL492:;
_LL493:(void)_throw(_tmp667);_LL48F:;}}}*_tmp5E6=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp66C;_tmp66C.tag=1;_tmp66C.f1=tud;_tmp66C.f2=tuf;_tmp66C;});_tmp66B;});
_tmp651=tud;_tmp652=tuf;goto _LL484;}_LL483: if(*((int*)_tmp64C)!= 1)goto _LL480;
_tmp651=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp64C)->f1;_tmp652=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp64C)->f2;_LL484: {struct Cyc_List_List*tvs=
_tmp651->tvs;for(0;_tmp5E7 != 0?tvs != 0: 0;(_tmp5E7=_tmp5E7->tl,tvs=tvs->tl)){void*
t1=(void*)_tmp5E7->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp5E7 != 0)({
struct Cyc_Std_String_pa_struct _tmp66F;_tmp66F.tag=0;_tmp66F.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp652->name);{struct Cyc_Std_String_pa_struct
_tmp66E;_tmp66E.tag=0;_tmp66E.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp651->name);{void*_tmp66D[2]={& _tmp66E,& _tmp66F};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s.%s",
sizeof(char),41),_tag_arr(_tmp66D,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_Std_String_pa_struct
_tmp672;_tmp672.tag=0;_tmp672.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp652->name);{struct Cyc_Std_String_pa_struct _tmp671;_tmp671.tag=0;_tmp671.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp651->name);{void*_tmp670[2]={&
_tmp671,& _tmp672};Cyc_Tcutil_terr(loc,_tag_arr("too few type arguments for tunion %s.%s",
sizeof(char),40),_tag_arr(_tmp670,sizeof(void*),2));}}});goto _LL480;}_LL480:;}
goto _LL439;_LL448: if(_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 4)goto _LL44A;
_tmp5E8=((struct Cyc_Absyn_PointerType_struct*)_tmp5D4)->f1;_tmp5E9=(void*)
_tmp5E8.elt_typ;_tmp5EA=(void*)_tmp5E8.rgn_typ;_tmp5EB=_tmp5E8.nullable;_tmp5EC=
_tmp5E8.tq;_tmp5ED=_tmp5E8.bounds;_LL449: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp5E9);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5EA);{void*_tmp673=(void*)(Cyc_Absyn_compress_conref(_tmp5ED))->v;
void*_tmp674;struct Cyc_Absyn_Exp*_tmp675;_LL495: if(_tmp673 <= (void*)1?1:*((int*)
_tmp673)!= 0)goto _LL497;_tmp674=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp673)->f1;if(_tmp674 <= (void*)1?1:*((int*)_tmp674)!= 0)goto _LL497;_tmp675=((
struct Cyc_Absyn_Upper_b_struct*)_tmp674)->f1;_LL496: Cyc_Tcexp_tcExp(te,0,_tmp675);
Cyc_Evexp_eval_const_uint_exp(_tmp675);goto _LL494;_LL497:;_LL498: goto _LL494;
_LL494:;}goto _LL439;_LL44A: if(_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 14)goto
_LL44C;_tmp5EE=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp5D4)->f1;_LL44B:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp5EE);goto _LL439;
_LL44C: if(_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 17)goto _LL44E;_tmp5EF=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp5D4)->f1;_LL44D: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)5,_tmp5EF);goto _LL439;_LL44E: if(_tmp5D4 <= (void*)3?1:*((int*)
_tmp5D4)!= 18)goto _LL450;_LL44F: goto _LL451;_LL450: if(_tmp5D4 <= (void*)3?1:*((int*)
_tmp5D4)!= 5)goto _LL452;_LL451: goto _LL453;_LL452: if((int)_tmp5D4 != 1)goto _LL454;
_LL453: goto _LL455;_LL454: if(_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 6)goto _LL456;
_LL455: goto _LL439;_LL456: if(_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 7)goto _LL458;
_tmp5F0=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp5D4)->f1;_tmp5F1=((struct
Cyc_Absyn_ArrayType_struct*)_tmp5D4)->f2;_tmp5F2=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5D4)->f3;_LL457: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp5F0);if(_tmp5F2 == 0)({void*_tmp676[0]={};Cyc_Tcutil_terr(loc,_tag_arr("an explicit array bound is required here",
sizeof(char),41),_tag_arr(_tmp676,sizeof(void*),0));});else{Cyc_Tcexp_tcExp(te,0,(
struct Cyc_Absyn_Exp*)_check_null(_tmp5F2));Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp5F2));}goto _LL439;_LL458: if(_tmp5D4 <= (void*)3?1:*((
int*)_tmp5D4)!= 8)goto _LL45A;_tmp5F3=((struct Cyc_Absyn_FnType_struct*)_tmp5D4)->f1;
_tmp5F4=_tmp5F3.tvars;_tmp5F5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp5D4)->f1).tvars;_tmp5F6=_tmp5F3.effect;_tmp5F7=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp5D4)->f1).effect;_tmp5F8=(void*)_tmp5F3.ret_typ;
_tmp5F9=_tmp5F3.args;_tmp5FA=_tmp5F3.c_varargs;_tmp5FB=_tmp5F3.cyc_varargs;
_tmp5FC=_tmp5F3.rgn_po;_tmp5FD=_tmp5F3.attributes;_LL459: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp5FD != 0;_tmp5FD=_tmp5FD->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp5FD->hd))({struct Cyc_Std_String_pa_struct
_tmp678;_tmp678.tag=0;_tmp678.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp5FD->hd);{void*_tmp677[1]={& _tmp678};Cyc_Tcutil_terr(loc,_tag_arr("bad function type attribute %s",
sizeof(char),31),_tag_arr(_tmp677,sizeof(void*),1));}});{void*_tmp679=(void*)
_tmp5FD->hd;void*_tmp67A;int _tmp67B;int _tmp67C;_LL49A: if((int)_tmp679 != 0)goto
_LL49C;_LL49B: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL499;_LL49C:
if((int)_tmp679 != 1)goto _LL49E;_LL49D: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}
goto _LL499;_LL49E: if((int)_tmp679 != 2)goto _LL4A0;_LL49F: if(!seen_fastcall){
seen_fastcall=1;++ num_convs;}goto _LL499;_LL4A0: if(_tmp679 <= (void*)16?1:*((int*)
_tmp679)!= 3)goto _LL4A2;_tmp67A=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp679)->f1;_tmp67B=((struct Cyc_Absyn_Format_att_struct*)_tmp679)->f2;_tmp67C=((
struct Cyc_Absyn_Format_att_struct*)_tmp679)->f3;_LL4A1: if(!seen_format){
seen_format=1;ft=_tmp67A;fmt_desc_arg=_tmp67B;fmt_arg_start=_tmp67C;}else{({void*
_tmp67D[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function can't have multiple format attributes",
sizeof(char),47),_tag_arr(_tmp67D,sizeof(void*),0));});}goto _LL499;_LL4A2:;
_LL4A3: goto _LL499;_LL499:;}}if(num_convs > 1)({void*_tmp67E[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("function can't have multiple calling conventions",sizeof(char),49),
_tag_arr(_tmp67E,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp5F5);{
struct Cyc_List_List*b=*_tmp5F5;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp67F=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp680;_LL4A5: if(*((int*)_tmp67F)!= 0)goto
_LL4A7;_tmp680=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp67F)->f1;if((int)
_tmp680 != 1)goto _LL4A7;_LL4A6:({struct Cyc_Std_String_pa_struct _tmp682;_tmp682.tag=
0;_tmp682.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name;{void*
_tmp681[1]={& _tmp682};Cyc_Tcutil_terr(loc,_tag_arr("function attempts to abstract Mem type variable %s",
sizeof(char),51),_tag_arr(_tmp681,sizeof(void*),1));}});goto _LL4A4;_LL4A7:;
_LL4A8: goto _LL4A4;_LL4A4:;}}}{struct Cyc_Tcutil_CVTEnv _tmp683=({struct Cyc_Tcutil_CVTEnv
_tmp6F1;_tmp6F1.kind_env=cvtenv.kind_env;_tmp6F1.free_vars=0;_tmp6F1.free_evars=
0;_tmp6F1.generalize_evars=cvtenv.generalize_evars;_tmp6F1.fn_result=1;_tmp6F1;});
_tmp683=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp683,(void*)1,_tmp5F8);_tmp683.fn_result=
0;{struct Cyc_List_List*a=_tmp5F9;for(0;a != 0;a=a->tl){_tmp683=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp683,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp5FB != 0){if(
_tmp5FA)({void*_tmp684[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("both c_vararg and cyc_vararg",sizeof(char),29),
_tag_arr(_tmp684,sizeof(void*),0));});{void*_tmp686;int _tmp687;struct Cyc_Absyn_VarargInfo
_tmp685=*_tmp5FB;_tmp686=(void*)_tmp685.type;_tmp687=_tmp685.inject;_tmp683=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp683,(void*)1,_tmp686);if(_tmp687){void*_tmp688=Cyc_Tcutil_compress(
_tmp686);struct Cyc_Absyn_TunionInfo _tmp689;void*_tmp68A;_LL4AA: if(_tmp688 <= (
void*)3?1:*((int*)_tmp688)!= 2)goto _LL4AC;_tmp689=((struct Cyc_Absyn_TunionType_struct*)
_tmp688)->f1;_tmp68A=(void*)_tmp689.tunion_info;if(*((int*)_tmp68A)!= 1)goto
_LL4AC;_LL4AB: goto _LL4A9;_LL4AC:;_LL4AD:({void*_tmp68B[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("can't inject a non-[x]tunion type",sizeof(char),34),_tag_arr(_tmp68B,
sizeof(void*),0));});goto _LL4A9;_LL4A9:;}}}if(seen_format){int _tmp68C=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp5F9);if((((fmt_desc_arg < 0?1:
fmt_desc_arg > _tmp68C)?1: fmt_arg_start < 0)?1:(_tmp5FB == 0?fmt_arg_start != 0: 0))?
1:(_tmp5FB != 0?fmt_arg_start != _tmp68C + 1: 0))({void*_tmp68D[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bad format descriptor",sizeof(char),22),_tag_arr(_tmp68D,sizeof(
void*),0));});else{void*_tmp68F;struct _tuple2 _tmp68E=*((struct _tuple2*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5F9,fmt_desc_arg - 1);_tmp68F=_tmp68E.f3;{
void*_tmp690=Cyc_Tcutil_compress(_tmp68F);struct Cyc_Absyn_PtrInfo _tmp691;void*
_tmp692;struct Cyc_Absyn_Conref*_tmp693;_LL4AF: if(_tmp690 <= (void*)3?1:*((int*)
_tmp690)!= 4)goto _LL4B1;_tmp691=((struct Cyc_Absyn_PointerType_struct*)_tmp690)->f1;
_tmp692=(void*)_tmp691.elt_typ;_tmp693=_tmp691.bounds;_LL4B0:{struct _tuple6
_tmp695=({struct _tuple6 _tmp694;_tmp694.f1=Cyc_Tcutil_compress(_tmp692);_tmp694.f2=
Cyc_Absyn_conref_def((void*)0,_tmp693);_tmp694;});void*_tmp696;void*_tmp697;void*
_tmp698;void*_tmp699;_LL4B4: _tmp696=_tmp695.f1;if(_tmp696 <= (void*)3?1:*((int*)
_tmp696)!= 5)goto _LL4B6;_tmp697=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp696)->f1;
if((int)_tmp697 != 2)goto _LL4B6;_tmp698=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp696)->f2;if((int)_tmp698 != 0)goto _LL4B6;_tmp699=_tmp695.f2;if((int)_tmp699 != 
0)goto _LL4B6;_LL4B5: goto _LL4B3;_LL4B6:;_LL4B7:({void*_tmp69A[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("format descriptor is not a char ? type",sizeof(char),39),_tag_arr(
_tmp69A,sizeof(void*),0));});goto _LL4B3;_LL4B3:;}goto _LL4AE;_LL4B1:;_LL4B2:({
void*_tmp69B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format descriptor is not a char ? type",
sizeof(char),39),_tag_arr(_tmp69B,sizeof(void*),0));});goto _LL4AE;_LL4AE:;}if(
fmt_arg_start != 0){int problem;{struct _tuple6 _tmp69D=({struct _tuple6 _tmp69C;
_tmp69C.f1=ft;_tmp69C.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp5FB))->type);_tmp69C;});void*_tmp69E;void*_tmp69F;struct Cyc_Absyn_TunionInfo
_tmp6A0;void*_tmp6A1;struct Cyc_Absyn_Tuniondecl**_tmp6A2;struct Cyc_Absyn_Tuniondecl*
_tmp6A3;void*_tmp6A4;void*_tmp6A5;struct Cyc_Absyn_TunionInfo _tmp6A6;void*_tmp6A7;
struct Cyc_Absyn_Tuniondecl**_tmp6A8;struct Cyc_Absyn_Tuniondecl*_tmp6A9;_LL4B9:
_tmp69E=_tmp69D.f1;if((int)_tmp69E != 0)goto _LL4BB;_tmp69F=_tmp69D.f2;if(_tmp69F
<= (void*)3?1:*((int*)_tmp69F)!= 2)goto _LL4BB;_tmp6A0=((struct Cyc_Absyn_TunionType_struct*)
_tmp69F)->f1;_tmp6A1=(void*)_tmp6A0.tunion_info;if(*((int*)_tmp6A1)!= 1)goto
_LL4BB;_tmp6A2=((struct Cyc_Absyn_KnownTunion_struct*)_tmp6A1)->f1;_tmp6A3=*
_tmp6A2;_LL4BA: problem=Cyc_Absyn_qvar_cmp(_tmp6A3->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL4B8;_LL4BB: _tmp6A4=_tmp69D.f1;if((int)_tmp6A4 != 1)goto _LL4BD;_tmp6A5=
_tmp69D.f2;if(_tmp6A5 <= (void*)3?1:*((int*)_tmp6A5)!= 2)goto _LL4BD;_tmp6A6=((
struct Cyc_Absyn_TunionType_struct*)_tmp6A5)->f1;_tmp6A7=(void*)_tmp6A6.tunion_info;
if(*((int*)_tmp6A7)!= 1)goto _LL4BD;_tmp6A8=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp6A7)->f1;_tmp6A9=*_tmp6A8;_LL4BC: problem=Cyc_Absyn_qvar_cmp(_tmp6A9->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL4B8;_LL4BD:;_LL4BE: problem=1;goto
_LL4B8;_LL4B8:;}if(problem)({void*_tmp6AA[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format attribute and vararg types don't match",
sizeof(char),46),_tag_arr(_tmp6AA,sizeof(void*),0));});}}}{struct Cyc_List_List*
rpo=_tmp5FC;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp6AC;void*_tmp6AD;void*
_tmp6AE;struct _tuple6*_tmp6AB=(struct _tuple6*)rpo->hd;_tmp6AC=*_tmp6AB;_tmp6AD=
_tmp6AC.f1;_tmp6AE=_tmp6AC.f2;_tmp683=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp683,(void*)4,_tmp6AD);_tmp683=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp683,(
void*)3,_tmp6AE);}}if(*_tmp5F7 != 0)_tmp683=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp683,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp5F7))->v);else{
struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp683.free_vars;for(0;
tvs != 0;tvs=tvs->tl){void*_tmp6AF=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmp6B0;struct Cyc_Core_Opt**_tmp6B1;void*
_tmp6B2;void*_tmp6B3;struct Cyc_Core_Opt*_tmp6B4;struct Cyc_Core_Opt**_tmp6B5;void*
_tmp6B6;void*_tmp6B7;struct Cyc_Core_Opt*_tmp6B8;struct Cyc_Core_Opt**_tmp6B9;
_LL4C0: if(*((int*)_tmp6AF)!= 2)goto _LL4C2;_tmp6B0=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6AF)->f1;_tmp6B1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6AF)->f1;_tmp6B2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6AF)->f2;if((
int)_tmp6B2 != 3)goto _LL4C2;_LL4C1:*_tmp6B1=({struct Cyc_Core_Opt*_tmp6BA=
_cycalloc(sizeof(*_tmp6BA));_tmp6BA->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp6BC;_tmp6BC.tag=0;_tmp6BC.f1=(void*)((void*)3);_tmp6BC;});_tmp6BB;}));
_tmp6BA;});goto _LL4C3;_LL4C2: if(*((int*)_tmp6AF)!= 0)goto _LL4C4;_tmp6B3=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp6AF)->f1;if((int)_tmp6B3 != 3)goto _LL4C4;_LL4C3:
effect=({struct Cyc_List_List*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6BE=_cycalloc(sizeof(*
_tmp6BE));_tmp6BE[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6BF;_tmp6BF.tag=19;
_tmp6BF.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6C0=_cycalloc(
sizeof(*_tmp6C0));_tmp6C0[0]=({struct Cyc_Absyn_VarType_struct _tmp6C1;_tmp6C1.tag=
1;_tmp6C1.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6C1;});_tmp6C0;}));_tmp6BF;});
_tmp6BE;}));_tmp6BD->tl=effect;_tmp6BD;});goto _LL4BF;_LL4C4: if(*((int*)_tmp6AF)
!= 2)goto _LL4C6;_tmp6B4=((struct Cyc_Absyn_Less_kb_struct*)_tmp6AF)->f1;_tmp6B5=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6AF)->f1;_tmp6B6=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6AF)->f2;if((int)_tmp6B6 != 4)goto
_LL4C6;_LL4C5:*_tmp6B5=({struct Cyc_Core_Opt*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));
_tmp6C2->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6C3=_cycalloc(
sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6C4;_tmp6C4.tag=0;
_tmp6C4.f1=(void*)((void*)4);_tmp6C4;});_tmp6C3;}));_tmp6C2;});goto _LL4C7;_LL4C6:
if(*((int*)_tmp6AF)!= 0)goto _LL4C8;_tmp6B7=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp6AF)->f1;if((int)_tmp6B7 != 4)goto _LL4C8;_LL4C7: effect=({struct Cyc_List_List*
_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6[0]=({struct Cyc_Absyn_VarType_struct
_tmp6C7;_tmp6C7.tag=1;_tmp6C7.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6C7;});
_tmp6C6;}));_tmp6C5->tl=effect;_tmp6C5;});goto _LL4BF;_LL4C8: if(*((int*)_tmp6AF)
!= 1)goto _LL4CA;_tmp6B8=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6AF)->f1;
_tmp6B9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6AF)->f1;
_LL4C9:*_tmp6B9=({struct Cyc_Core_Opt*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));
_tmp6C9[0]=({struct Cyc_Absyn_Less_kb_struct _tmp6CA;_tmp6CA.tag=2;_tmp6CA.f1=0;
_tmp6CA.f2=(void*)((void*)0);_tmp6CA;});_tmp6C9;}));_tmp6C8;});goto _LL4CB;_LL4CA:;
_LL4CB: effect=({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));
_tmp6CC[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6CD;_tmp6CD.tag=21;_tmp6CD.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));
_tmp6CE[0]=({struct Cyc_Absyn_VarType_struct _tmp6CF;_tmp6CF.tag=1;_tmp6CF.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6CF;});_tmp6CE;}));_tmp6CD;});_tmp6CC;}));
_tmp6CB->tl=effect;_tmp6CB;});goto _LL4BF;_LL4BF:;}}{struct Cyc_List_List*ts=
_tmp683.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp6D0=Cyc_Tcutil_typ_kind((
void*)ts->hd);_LL4CD: if((int)_tmp6D0 != 3)goto _LL4CF;_LL4CE: effect=({struct Cyc_List_List*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp6D3;_tmp6D3.tag=19;_tmp6D3.f1=(void*)((void*)ts->hd);_tmp6D3;});_tmp6D2;}));
_tmp6D1->tl=effect;_tmp6D1;});goto _LL4CC;_LL4CF: if((int)_tmp6D0 != 4)goto _LL4D1;
_LL4D0: effect=({struct Cyc_List_List*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->hd=(
void*)((void*)ts->hd);_tmp6D4->tl=effect;_tmp6D4;});goto _LL4CC;_LL4D1:;_LL4D2:
effect=({struct Cyc_List_List*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));
_tmp6D6[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6D7;_tmp6D7.tag=21;_tmp6D7.f1=(
void*)((void*)ts->hd);_tmp6D7;});_tmp6D6;}));_tmp6D5->tl=effect;_tmp6D5;});goto
_LL4CC;_LL4CC:;}}*_tmp5F7=({struct Cyc_Core_Opt*_tmp6D8=_cycalloc(sizeof(*_tmp6D8));
_tmp6D8->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6D9=_cycalloc(
sizeof(*_tmp6D9));_tmp6D9[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6DA;_tmp6DA.tag=
20;_tmp6DA.f1=effect;_tmp6DA;});_tmp6D9;}));_tmp6D8;});}if(*_tmp5F5 != 0){struct
Cyc_List_List*bs=*_tmp5F5;for(0;bs != 0;bs=bs->tl){void*_tmp6DB=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmp6DC;struct
Cyc_Core_Opt**_tmp6DD;struct Cyc_Core_Opt*_tmp6DE;struct Cyc_Core_Opt**_tmp6DF;
void*_tmp6E0;struct Cyc_Core_Opt*_tmp6E1;struct Cyc_Core_Opt**_tmp6E2;void*_tmp6E3;
struct Cyc_Core_Opt*_tmp6E4;struct Cyc_Core_Opt**_tmp6E5;void*_tmp6E6;void*_tmp6E7;
_LL4D4: if(*((int*)_tmp6DB)!= 1)goto _LL4D6;_tmp6DC=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6DB)->f1;_tmp6DD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6DB)->f1;_LL4D5:({struct Cyc_Std_String_pa_struct _tmp6E9;_tmp6E9.tag=0;
_tmp6E9.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name;{void*
_tmp6E8[1]={& _tmp6E9};Cyc_Tcutil_warn(loc,_tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof(char),47),_tag_arr(_tmp6E8,sizeof(void*),1));}});_tmp6DF=_tmp6DD;goto
_LL4D7;_LL4D6: if(*((int*)_tmp6DB)!= 2)goto _LL4D8;_tmp6DE=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6DB)->f1;_tmp6DF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6DB)->f1;_tmp6E0=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6DB)->f2;if((
int)_tmp6E0 != 1)goto _LL4D8;_LL4D7: _tmp6E2=_tmp6DF;goto _LL4D9;_LL4D8: if(*((int*)
_tmp6DB)!= 2)goto _LL4DA;_tmp6E1=((struct Cyc_Absyn_Less_kb_struct*)_tmp6DB)->f1;
_tmp6E2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6DB)->f1;
_tmp6E3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6DB)->f2;if((int)_tmp6E3 != 
0)goto _LL4DA;_LL4D9:*_tmp6E2=({struct Cyc_Core_Opt*_tmp6EA=_cycalloc(sizeof(*
_tmp6EA));_tmp6EA->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6EB=
_cycalloc(sizeof(*_tmp6EB));_tmp6EB[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6EC;
_tmp6EC.tag=0;_tmp6EC.f1=(void*)((void*)2);_tmp6EC;});_tmp6EB;}));_tmp6EA;});
goto _LL4D3;_LL4DA: if(*((int*)_tmp6DB)!= 2)goto _LL4DC;_tmp6E4=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6DB)->f1;_tmp6E5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6DB)->f1;_tmp6E6=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6DB)->f2;
_LL4DB:*_tmp6E5=({struct Cyc_Core_Opt*_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6EE=_cycalloc(sizeof(*_tmp6EE));
_tmp6EE[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6EF;_tmp6EF.tag=0;_tmp6EF.f1=(void*)
_tmp6E6;_tmp6EF;});_tmp6EE;}));_tmp6ED;});goto _LL4D3;_LL4DC: if(*((int*)_tmp6DB)
!= 0)goto _LL4DE;_tmp6E7=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp6DB)->f1;if((
int)_tmp6E7 != 1)goto _LL4DE;_LL4DD:({void*_tmp6F0[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("functions can't abstract M types",sizeof(char),33),_tag_arr(_tmp6F0,
sizeof(void*),0));});goto _LL4D3;_LL4DE:;_LL4DF: goto _LL4D3;_LL4D3:;}}cvtenv.kind_env=
Cyc_Tcutil_remove_bound_tvars(_tmp683.kind_env,*_tmp5F5);_tmp683.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp683.free_vars,*_tmp5F5);{struct Cyc_List_List*tvs=_tmp683.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp683.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL439;}}_LL45A: if(_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 
9)goto _LL45C;_tmp5FE=((struct Cyc_Absyn_TupleType_struct*)_tmp5D4)->f1;_LL45B:
for(0;_tmp5FE != 0;_tmp5FE=_tmp5FE->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,(void*)1,(*((struct _tuple4*)_tmp5FE->hd)).f2);}goto _LL439;_LL45C: if(
_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 11)goto _LL45E;_tmp5FF=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5D4)->f1;_tmp600=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5D4)->f2;_LL45D:{
struct _RegionHandle _tmp6F2=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp6F2;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp600 != 0;_tmp600=_tmp600->tl){struct Cyc_Absyn_Aggrfield _tmp6F4;struct
_tagged_arr*_tmp6F5;struct Cyc_Absyn_Tqual _tmp6F6;void*_tmp6F7;struct Cyc_Absyn_Exp*
_tmp6F8;struct Cyc_List_List*_tmp6F9;struct Cyc_Absyn_Aggrfield*_tmp6F3=(struct Cyc_Absyn_Aggrfield*)
_tmp600->hd;_tmp6F4=*_tmp6F3;_tmp6F5=_tmp6F4.name;_tmp6F6=_tmp6F4.tq;_tmp6F7=(
void*)_tmp6F4.type;_tmp6F8=_tmp6F4.width;_tmp6F9=_tmp6F4.attributes;if(((int(*)(
int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,
struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,prev_fields,_tmp6F5))({
struct Cyc_Std_String_pa_struct _tmp6FB;_tmp6FB.tag=0;_tmp6FB.f1=(struct
_tagged_arr)*_tmp6F5;{void*_tmp6FA[1]={& _tmp6FB};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s",
sizeof(char),19),_tag_arr(_tmp6FA,sizeof(void*),1));}});if(Cyc_Std_strcmp(*
_tmp6F5,_tag_arr("",sizeof(char),1))!= 0)prev_fields=({struct Cyc_List_List*
_tmp6FC=_region_malloc(aprev_rgn,sizeof(*_tmp6FC));_tmp6FC->hd=_tmp6F5;_tmp6FC->tl=
prev_fields;_tmp6FC;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp6F7);if(_tmp5FF == (void*)1?!Cyc_Tcutil_bits_only(_tmp6F7): 0)({struct Cyc_Std_String_pa_struct
_tmp6FE;_tmp6FE.tag=0;_tmp6FE.f1=(struct _tagged_arr)*_tmp6F5;{void*_tmp6FD[1]={&
_tmp6FE};Cyc_Tcutil_terr(loc,_tag_arr("union member %s has a non-integral type",
sizeof(char),40),_tag_arr(_tmp6FD,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(
loc,te,_tmp6F7,_tmp6F8,_tmp6F5);Cyc_Tcutil_check_field_atts(loc,_tmp6F5,_tmp6F9);}};
_pop_region(aprev_rgn);}goto _LL439;_LL45E: if(_tmp5D4 <= (void*)3?1:*((int*)
_tmp5D4)!= 10)goto _LL460;_tmp601=((struct Cyc_Absyn_AggrType_struct*)_tmp5D4)->f1;
_tmp602=(void*)_tmp601.aggr_info;_tmp603=(void**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp5D4)->f1).aggr_info;_tmp604=_tmp601.targs;_tmp605=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp5D4)->f1).targs;_LL45F:{void*_tmp6FF=*
_tmp603;void*_tmp700;struct _tuple1*_tmp701;struct Cyc_Absyn_Aggrdecl**_tmp702;
struct Cyc_Absyn_Aggrdecl*_tmp703;_LL4E1: if(*((int*)_tmp6FF)!= 0)goto _LL4E3;
_tmp700=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp6FF)->f1;_tmp701=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp6FF)->f2;_LL4E2: {struct Cyc_Absyn_Aggrdecl**
adp;{struct _handler_cons _tmp704;_push_handler(& _tmp704);{int _tmp706=0;if(setjmp(
_tmp704.handler))_tmp706=1;if(!_tmp706){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,
_tmp701);*_tmp603=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp707=_cycalloc(
sizeof(*_tmp707));_tmp707[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp708;_tmp708.tag=
1;_tmp708.f1=adp;_tmp708;});_tmp707;});;_pop_handler();}else{void*_tmp705=(void*)
_exn_thrown;void*_tmp70A=_tmp705;_LL4E6: if(_tmp70A != Cyc_Dict_Absent)goto _LL4E8;
_LL4E7: {struct Cyc_Tcenv_Genv*_tmp70B=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*
_tmp70C=({struct Cyc_Absyn_Aggrdecl*_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711->kind=(
void*)_tmp700;_tmp711->sc=(void*)((void*)3);_tmp711->name=_tmp701;_tmp711->tvs=0;
_tmp711->impl=0;_tmp711->attributes=0;_tmp711;});Cyc_Tc_tcAggrdecl(te,_tmp70B,
loc,_tmp70C);adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp701);*_tmp603=(void*)({
struct Cyc_Absyn_KnownAggr_struct*_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[0]=({
struct Cyc_Absyn_KnownAggr_struct _tmp70E;_tmp70E.tag=1;_tmp70E.f1=adp;_tmp70E;});
_tmp70D;});if(*_tmp605 != 0){({struct Cyc_Std_String_pa_struct _tmp710;_tmp710.tag=
0;_tmp710.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp701);{void*_tmp70F[1]={&
_tmp710};Cyc_Tcutil_terr(loc,_tag_arr("declare parameterized type %s before using",
sizeof(char),43),_tag_arr(_tmp70F,sizeof(void*),1));}});return cvtenv;}goto _LL4E5;}
_LL4E8:;_LL4E9:(void)_throw(_tmp70A);_LL4E5:;}}}_tmp703=*adp;goto _LL4E4;}_LL4E3:
if(*((int*)_tmp6FF)!= 1)goto _LL4E0;_tmp702=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp6FF)->f1;_tmp703=*_tmp702;_LL4E4: {struct Cyc_List_List*tvs=_tmp703->tvs;
struct Cyc_List_List*ts=*_tmp605;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_Std_String_pa_struct
_tmp713;_tmp713.tag=0;_tmp713.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp703->name);{void*_tmp712[1]={& _tmp713};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for type %s",
sizeof(char),32),_tag_arr(_tmp712,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp714=
_cycalloc(sizeof(*_tmp714));_tmp714->hd=(void*)e;_tmp714->tl=hidden_ts;_tmp714;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*_tmp605=Cyc_List_imp_append(*
_tmp605,Cyc_List_imp_rev(hidden_ts));}}_LL4E0:;}goto _LL439;_LL460: if(_tmp5D4 <= (
void*)3?1:*((int*)_tmp5D4)!= 16)goto _LL462;_tmp606=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5D4)->f1;_tmp607=((struct Cyc_Absyn_TypedefType_struct*)_tmp5D4)->f2;_tmp608=(
struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp5D4)->f2;
_tmp609=((struct Cyc_Absyn_TypedefType_struct*)_tmp5D4)->f3;_tmp60A=(struct Cyc_Absyn_Typedefdecl**)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp5D4)->f3;_tmp60B=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5D4)->f4;_tmp60C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp5D4)->f4;_LL461: {struct Cyc_List_List*targs=*_tmp608;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmp715;_push_handler(& _tmp715);{int _tmp717=0;if(setjmp(
_tmp715.handler))_tmp717=1;if(!_tmp717){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmp606);;_pop_handler();}else{void*_tmp716=(void*)_exn_thrown;void*_tmp719=
_tmp716;_LL4EB: if(_tmp719 != Cyc_Dict_Absent)goto _LL4ED;_LL4EC:({struct Cyc_Std_String_pa_struct
_tmp71B;_tmp71B.tag=0;_tmp71B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp606);{void*_tmp71A[1]={& _tmp71B};Cyc_Tcutil_terr(loc,_tag_arr("unbound typedef name %s",
sizeof(char),24),_tag_arr(_tmp71A,sizeof(void*),1));}});return cvtenv;_LL4ED:;
_LL4EE:(void)_throw(_tmp719);_LL4EA:;}}}*_tmp60A=(struct Cyc_Absyn_Typedefdecl*)
td;_tmp606[0]=(td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0?
tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C->hd=({
struct _tuple8*_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp71D->f2=(void*)ts->hd;_tmp71D;});_tmp71C->tl=inst;_tmp71C;});}if(ts
!= 0)({struct Cyc_Std_String_pa_struct _tmp71F;_tmp71F.tag=0;_tmp71F.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp606);{void*_tmp71E[1]={& _tmp71F};Cyc_Tcutil_terr(
loc,_tag_arr("too many parameters for typedef %s",sizeof(char),35),_tag_arr(
_tmp71E,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp720=_cycalloc(
sizeof(*_tmp720));_tmp720->hd=(void*)e;_tmp720->tl=hidden_ts;_tmp720;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp721=_cycalloc(sizeof(*_tmp721));_tmp721->hd=({struct _tuple8*_tmp722=
_cycalloc(sizeof(*_tmp722));_tmp722->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp722->f2=
e;_tmp722;});_tmp721->tl=inst;_tmp721;});}*_tmp608=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp60C=({struct Cyc_Core_Opt*
_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723->v=(void*)new_typ;_tmp723;});}goto
_LL439;}}_LL462: if((int)_tmp5D4 != 2)goto _LL464;_LL463: goto _LL439;_LL464: if(
_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 15)goto _LL466;_tmp60D=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5D4)->f1;_LL465: _tmp60E=_tmp60D;goto _LL467;_LL466: if(_tmp5D4 <= (void*)3?1:*((
int*)_tmp5D4)!= 19)goto _LL468;_tmp60E=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp5D4)->f1;_LL467: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp60E);goto _LL439;_LL468: if(_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 21)goto
_LL46A;_tmp60F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp5D4)->f1;_LL469:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp60F);goto _LL439;
_LL46A: if(_tmp5D4 <= (void*)3?1:*((int*)_tmp5D4)!= 20)goto _LL439;_tmp610=((struct
Cyc_Absyn_JoinEff_struct*)_tmp5D4)->f1;_LL46B: for(0;_tmp610 != 0;_tmp610=_tmp610->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)_tmp610->hd);}
goto _LL439;_LL439:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))({
struct Cyc_Std_String_pa_struct _tmp727;_tmp727.tag=0;_tmp727.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(expected_kind);{struct Cyc_Std_String_pa_struct
_tmp726;_tmp726.tag=0;_tmp726.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t));{struct Cyc_Std_String_pa_struct _tmp725;_tmp725.tag=0;_tmp725.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp724[3]={& _tmp725,& _tmp726,&
_tmp727};Cyc_Tcutil_terr(loc,_tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof(char),51),_tag_arr(_tmp724,sizeof(void*),3));}}}});return cvtenv;}static
struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp728=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp731;_tmp731.kind_env=kind_env;_tmp731.free_vars=
0;_tmp731.free_evars=0;_tmp731.generalize_evars=generalize_evars;_tmp731.fn_result=
0;_tmp731;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp728.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp728.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp728.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp729=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp72A;struct Cyc_Core_Opt**_tmp72B;_LL4F0: if(_tmp729 <= (void*)3?1:*((int*)
_tmp729)!= 0)goto _LL4F2;_tmp72A=((struct Cyc_Absyn_Evar_struct*)_tmp729)->f4;
_tmp72B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp729)->f4;
_LL4F1: if(*_tmp72B == 0)*_tmp72B=({struct Cyc_Core_Opt*_tmp72C=_cycalloc(sizeof(*
_tmp72C));_tmp72C->v=kind_env;_tmp72C;});else{struct Cyc_List_List*_tmp72D=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp72B))->v;struct Cyc_List_List*
_tmp72E=0;for(0;_tmp72D != 0;_tmp72D=_tmp72D->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp72D->hd))_tmp72E=({struct Cyc_List_List*_tmp72F=_cycalloc(sizeof(*_tmp72F));
_tmp72F->hd=(struct Cyc_Absyn_Tvar*)_tmp72D->hd;_tmp72F->tl=_tmp72E;_tmp72F;});}*
_tmp72B=({struct Cyc_Core_Opt*_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730->v=
_tmp72E;_tmp730;});}goto _LL4EF;_LL4F2:;_LL4F3: goto _LL4EF;_LL4EF:;}}return _tmp728;}
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_Tcutil_CVTEnv _tmp732=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp733=_tmp732.free_vars;struct Cyc_List_List*
_tmp734=_tmp732.free_evars;{struct Cyc_List_List*x=_tmp733;for(0;x != 0;x=x->tl){
void*_tmp735=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp736;struct Cyc_Core_Opt**_tmp737;struct Cyc_Core_Opt*
_tmp738;struct Cyc_Core_Opt**_tmp739;void*_tmp73A;struct Cyc_Core_Opt*_tmp73B;
struct Cyc_Core_Opt**_tmp73C;void*_tmp73D;struct Cyc_Core_Opt*_tmp73E;struct Cyc_Core_Opt**
_tmp73F;void*_tmp740;void*_tmp741;_LL4F5: if(*((int*)_tmp735)!= 1)goto _LL4F7;
_tmp736=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp735)->f1;_tmp737=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp735)->f1;_LL4F6: _tmp739=_tmp737;goto
_LL4F8;_LL4F7: if(*((int*)_tmp735)!= 2)goto _LL4F9;_tmp738=((struct Cyc_Absyn_Less_kb_struct*)
_tmp735)->f1;_tmp739=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp735)->f1;_tmp73A=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp735)->f2;if((
int)_tmp73A != 1)goto _LL4F9;_LL4F8: _tmp73C=_tmp739;goto _LL4FA;_LL4F9: if(*((int*)
_tmp735)!= 2)goto _LL4FB;_tmp73B=((struct Cyc_Absyn_Less_kb_struct*)_tmp735)->f1;
_tmp73C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp735)->f1;
_tmp73D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp735)->f2;if((int)_tmp73D != 
0)goto _LL4FB;_LL4FA:*_tmp73C=({struct Cyc_Core_Opt*_tmp742=_cycalloc(sizeof(*
_tmp742));_tmp742->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp743=
_cycalloc(sizeof(*_tmp743));_tmp743[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp744;
_tmp744.tag=0;_tmp744.f1=(void*)((void*)2);_tmp744;});_tmp743;}));_tmp742;});
goto _LL4F4;_LL4FB: if(*((int*)_tmp735)!= 2)goto _LL4FD;_tmp73E=((struct Cyc_Absyn_Less_kb_struct*)
_tmp735)->f1;_tmp73F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp735)->f1;_tmp740=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp735)->f2;
_LL4FC:*_tmp73F=({struct Cyc_Core_Opt*_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp746=_cycalloc(sizeof(*_tmp746));
_tmp746[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp747;_tmp747.tag=0;_tmp747.f1=(void*)
_tmp740;_tmp747;});_tmp746;}));_tmp745;});goto _LL4F4;_LL4FD: if(*((int*)_tmp735)
!= 0)goto _LL4FF;_tmp741=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp735)->f1;if((
int)_tmp741 != 1)goto _LL4FF;_LL4FE:({struct Cyc_Std_String_pa_struct _tmp749;
_tmp749.tag=0;_tmp749.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd);{void*_tmp748[1]={& _tmp749};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s cannot have kind M",
sizeof(char),36),_tag_arr(_tmp748,sizeof(void*),1));}});goto _LL4F4;_LL4FF:;
_LL500: goto _LL4F4;_LL4F4:;}}if(_tmp733 != 0?1: _tmp734 != 0){{void*_tmp74A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp74B;struct Cyc_List_List*_tmp74C;struct Cyc_List_List**
_tmp74D;struct Cyc_Core_Opt*_tmp74E;void*_tmp74F;struct Cyc_List_List*_tmp750;int
_tmp751;struct Cyc_Absyn_VarargInfo*_tmp752;struct Cyc_List_List*_tmp753;struct Cyc_List_List*
_tmp754;_LL502: if(_tmp74A <= (void*)3?1:*((int*)_tmp74A)!= 8)goto _LL504;_tmp74B=((
struct Cyc_Absyn_FnType_struct*)_tmp74A)->f1;_tmp74C=_tmp74B.tvars;_tmp74D=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp74A)->f1).tvars;
_tmp74E=_tmp74B.effect;_tmp74F=(void*)_tmp74B.ret_typ;_tmp750=_tmp74B.args;
_tmp751=_tmp74B.c_varargs;_tmp752=_tmp74B.cyc_varargs;_tmp753=_tmp74B.rgn_po;
_tmp754=_tmp74B.attributes;_LL503: if(*_tmp74D == 0){*_tmp74D=_tmp733;_tmp733=0;}
goto _LL501;_LL504:;_LL505: goto _LL501;_LL501:;}if(_tmp733 != 0)({struct Cyc_Std_String_pa_struct
_tmp756;_tmp756.tag=0;_tmp756.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)
_tmp733->hd)->name;{void*_tmp755[1]={& _tmp756};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s",
sizeof(char),25),_tag_arr(_tmp755,sizeof(void*),1));}});if(_tmp734 != 0)for(0;
_tmp734 != 0;_tmp734=_tmp734->tl){void*e=(void*)_tmp734->hd;void*_tmp757=Cyc_Tcutil_typ_kind(
e);_LL507: if((int)_tmp757 != 3)goto _LL509;_LL508: if(!Cyc_Tcutil_unify(e,(void*)2))({
void*_tmp758[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with heap!",sizeof(char),28),_tag_arr(_tmp758,sizeof(
void*),0));});goto _LL506;_LL509: if((int)_tmp757 != 4)goto _LL50B;_LL50A: if(!Cyc_Tcutil_unify(
e,Cyc_Absyn_empty_effect))({void*_tmp759[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(char),26),_tag_arr(_tmp759,sizeof(void*),0));});goto _LL506;_LL50B:;_LL50C:({
struct Cyc_Std_String_pa_struct _tmp75C;_tmp75C.tag=0;_tmp75C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp75B;
_tmp75B.tag=0;_tmp75B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp75A[2]={& _tmp75B,& _tmp75C};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp75A,sizeof(void*),2));}}});goto _LL506;_LL506:;}}}
void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(
loc,te,t);{void*_tmp75D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp75E;
struct Cyc_List_List*_tmp75F;struct Cyc_Core_Opt*_tmp760;_LL50E: if(_tmp75D <= (void*)
3?1:*((int*)_tmp75D)!= 8)goto _LL510;_tmp75E=((struct Cyc_Absyn_FnType_struct*)
_tmp75D)->f1;_tmp75F=_tmp75E.tvars;_tmp760=_tmp75E.effect;_LL50F: fd->tvs=_tmp75F;
fd->effect=_tmp760;goto _LL50D;_LL510:;_LL511:({void*_tmp761[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_fndecl_valid_type: not a FnType",
sizeof(char),38),_tag_arr(_tmp761,sizeof(void*),0));});_LL50D:;}{struct
_RegionHandle _tmp762=_new_region("r");struct _RegionHandle*r=& _tmp762;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct _tuple13*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct _tuple13*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,_tag_arr("function declaration has repeated parameter",sizeof(char),
44));;_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp763=_cycalloc(
sizeof(*_tmp763));_tmp763->v=(void*)t;_tmp763;});}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp764=Cyc_Tcutil_check_valid_type(loc,te,bound_tvars,expected_kind,0,t);struct
Cyc_List_List*_tmp765=Cyc_Tcutil_remove_bound_tvars(_tmp764.free_vars,
bound_tvars);struct Cyc_List_List*_tmp766=_tmp764.free_evars;{struct Cyc_List_List*
fs=_tmp765;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp767=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;({struct Cyc_Std_String_pa_struct _tmp76A;_tmp76A.tag=0;_tmp76A.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp769;_tmp769.tag=0;_tmp769.f1=(struct _tagged_arr)*_tmp767;{void*_tmp768[2]={&
_tmp769,& _tmp76A};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s in type %s",
sizeof(char),36),_tag_arr(_tmp768,sizeof(void*),2));}}});}}if(!allow_evars?
_tmp766 != 0: 0)for(0;_tmp766 != 0;_tmp766=_tmp766->tl){void*e=(void*)_tmp766->hd;
void*_tmp76B=Cyc_Tcutil_typ_kind(e);_LL513: if((int)_tmp76B != 3)goto _LL515;_LL514:
if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp76C[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(char),28),_tag_arr(_tmp76C,sizeof(void*),0));});goto _LL512;_LL515: if((int)
_tmp76B != 4)goto _LL517;_LL516: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp76D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!",sizeof(char),26),_tag_arr(_tmp76D,sizeof(
void*),0));});goto _LL512;_LL517:;_LL518:({struct Cyc_Std_String_pa_struct _tmp770;
_tmp770.tag=0;_tmp770.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp76F;_tmp76F.tag=0;_tmp76F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{
void*_tmp76E[2]={& _tmp76F,& _tmp770};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp76E,sizeof(void*),2));}}});goto _LL512;_LL512:;}}
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=
Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*
tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _tagged_arr(*a2string)(void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0)({struct Cyc_Std_String_pa_struct _tmp773;_tmp773.tag=0;_tmp773.f1=(
struct _tagged_arr)a2string((void*)vs->hd);{struct Cyc_Std_String_pa_struct _tmp772;
_tmp772.tag=0;_tmp772.f1=(struct _tagged_arr)msg;{void*_tmp771[2]={& _tmp772,&
_tmp773};Cyc_Tcutil_terr(loc,_tag_arr("%s: %s",sizeof(char),7),_tag_arr(_tmp771,
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
_tag_arr("duplicate type variable",sizeof(char),24));}struct _tuple18{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple19{struct Cyc_List_List*f1;void*f2;};struct _tuple20{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_Std_strcmp(*((
struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,_tag_arr("",sizeof(char),1))!= 0)
fields=({struct Cyc_List_List*_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774->hd=({
struct _tuple18*_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp775->f2=0;_tmp775;});_tmp774->tl=fields;_tmp774;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple19 _tmp777;struct Cyc_List_List*
_tmp778;void*_tmp779;struct _tuple19*_tmp776=(struct _tuple19*)des->hd;_tmp777=*
_tmp776;_tmp778=_tmp777.f1;_tmp779=_tmp777.f2;if(_tmp778 == 0){struct Cyc_List_List*
_tmp77A=fields;for(0;_tmp77A != 0;_tmp77A=_tmp77A->tl){if(!(*((struct _tuple18*)
_tmp77A->hd)).f2){(*((struct _tuple18*)_tmp77A->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp77B=_cycalloc(sizeof(*_tmp77B));
_tmp77B->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp77C=_cycalloc(
sizeof(*_tmp77C));_tmp77C[0]=({struct Cyc_Absyn_FieldName_struct _tmp77D;_tmp77D.tag=
1;_tmp77D.f1=((*((struct _tuple18*)_tmp77A->hd)).f1)->name;_tmp77D;});_tmp77C;}));
_tmp77B->tl=0;_tmp77B;});ans=({struct Cyc_List_List*_tmp77E=_region_malloc(rgn,
sizeof(*_tmp77E));_tmp77E->hd=({struct _tuple20*_tmp77F=_region_malloc(rgn,
sizeof(*_tmp77F));_tmp77F->f1=(*((struct _tuple18*)_tmp77A->hd)).f1;_tmp77F->f2=
_tmp779;_tmp77F;});_tmp77E->tl=ans;_tmp77E;});break;}}if(_tmp77A == 0)({void*
_tmp780[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too many arguments to struct",
sizeof(char),29),_tag_arr(_tmp780,sizeof(void*),0));});}else{if(_tmp778->tl != 0)({
void*_tmp781[0]={};Cyc_Tcutil_terr(loc,_tag_arr("multiple designators are not supported",
sizeof(char),39),_tag_arr(_tmp781,sizeof(void*),0));});else{void*_tmp782=(void*)
_tmp778->hd;struct _tagged_arr*_tmp783;_LL51A: if(*((int*)_tmp782)!= 0)goto _LL51C;
_LL51B:({void*_tmp784[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array designator used in argument to struct",
sizeof(char),44),_tag_arr(_tmp784,sizeof(void*),0));});goto _LL519;_LL51C: if(*((
int*)_tmp782)!= 1)goto _LL519;_tmp783=((struct Cyc_Absyn_FieldName_struct*)_tmp782)->f1;
_LL51D: {struct Cyc_List_List*_tmp785=fields;for(0;_tmp785 != 0;_tmp785=_tmp785->tl){
if(Cyc_Std_zstrptrcmp(_tmp783,((*((struct _tuple18*)_tmp785->hd)).f1)->name)== 0){
if((*((struct _tuple18*)_tmp785->hd)).f2)({struct Cyc_Std_String_pa_struct _tmp787;
_tmp787.tag=0;_tmp787.f1=(struct _tagged_arr)*_tmp783;{void*_tmp786[1]={& _tmp787};
Cyc_Tcutil_terr(loc,_tag_arr("field %s has already been used as an argument",
sizeof(char),46),_tag_arr(_tmp786,sizeof(void*),1));}});(*((struct _tuple18*)
_tmp785->hd)).f2=1;ans=({struct Cyc_List_List*_tmp788=_region_malloc(rgn,sizeof(*
_tmp788));_tmp788->hd=({struct _tuple20*_tmp789=_region_malloc(rgn,sizeof(*
_tmp789));_tmp789->f1=(*((struct _tuple18*)_tmp785->hd)).f1;_tmp789->f2=_tmp779;
_tmp789;});_tmp788->tl=ans;_tmp788;});break;}}if(_tmp785 == 0)({struct Cyc_Std_String_pa_struct
_tmp78B;_tmp78B.tag=0;_tmp78B.f1=(struct _tagged_arr)*_tmp783;{void*_tmp78A[1]={&
_tmp78B};Cyc_Tcutil_terr(loc,_tag_arr("bad field designator %s",sizeof(char),24),
_tag_arr(_tmp78A,sizeof(void*),1));}});goto _LL519;}_LL519:;}}}for(0;fields != 0;
fields=fields->tl){if(!(*((struct _tuple18*)fields->hd)).f2){({void*_tmp78C[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments to struct",sizeof(char),28),
_tag_arr(_tmp78C,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp78D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp78E;void*_tmp78F;struct Cyc_Absyn_Conref*_tmp790;_LL51F: if(_tmp78D <= (void*)3?
1:*((int*)_tmp78D)!= 4)goto _LL521;_tmp78E=((struct Cyc_Absyn_PointerType_struct*)
_tmp78D)->f1;_tmp78F=(void*)_tmp78E.elt_typ;_tmp790=_tmp78E.bounds;_LL520: {
struct Cyc_Absyn_Conref*_tmp791=Cyc_Absyn_compress_conref(_tmp790);void*_tmp792=(
void*)(Cyc_Absyn_compress_conref(_tmp791))->v;void*_tmp793;_LL524: if(_tmp792 <= (
void*)1?1:*((int*)_tmp792)!= 0)goto _LL526;_tmp793=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp792)->f1;if((int)_tmp793 != 0)goto _LL526;_LL525:*elt_typ_dest=_tmp78F;return 1;
_LL526: if((int)_tmp792 != 0)goto _LL528;_LL527:(void*)(_tmp791->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp794=_cycalloc(sizeof(*_tmp794));_tmp794[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp795;_tmp795.tag=0;_tmp795.f1=(void*)((void*)
0);_tmp795;});_tmp794;})));*elt_typ_dest=_tmp78F;return 1;_LL528:;_LL529: return 0;
_LL523:;}_LL521:;_LL522: return 0;_LL51E:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp7D6;_tmp7D6.f1=0;
_tmp7D6.f2=(void*)2;_tmp7D6;});void*_tmp796=(void*)e->r;struct _tuple1*_tmp797;
void*_tmp798;struct Cyc_Absyn_Exp*_tmp799;struct _tagged_arr*_tmp79A;struct Cyc_Absyn_Exp*
_tmp79B;struct _tagged_arr*_tmp79C;struct Cyc_Absyn_Exp*_tmp79D;struct Cyc_Absyn_Exp*
_tmp79E;struct Cyc_Absyn_Exp*_tmp79F;_LL52B: if(*((int*)_tmp796)!= 1)goto _LL52D;
_tmp797=((struct Cyc_Absyn_Var_e_struct*)_tmp796)->f1;_tmp798=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp796)->f2;_LL52C: {void*_tmp7A0=_tmp798;struct Cyc_Absyn_Vardecl*_tmp7A1;
struct Cyc_Absyn_Vardecl*_tmp7A2;struct Cyc_Absyn_Vardecl*_tmp7A3;struct Cyc_Absyn_Vardecl*
_tmp7A4;_LL538: if((int)_tmp7A0 != 0)goto _LL53A;_LL539: goto _LL53B;_LL53A: if(
_tmp7A0 <= (void*)1?1:*((int*)_tmp7A0)!= 1)goto _LL53C;_LL53B: return bogus_ans;
_LL53C: if(_tmp7A0 <= (void*)1?1:*((int*)_tmp7A0)!= 0)goto _LL53E;_tmp7A1=((struct
Cyc_Absyn_Global_b_struct*)_tmp7A0)->f1;_LL53D: {void*_tmp7A5=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL545: if(_tmp7A5 <= (void*)
3?1:*((int*)_tmp7A5)!= 7)goto _LL547;_LL546: return({struct _tuple10 _tmp7A6;_tmp7A6.f1=
1;_tmp7A6.f2=(void*)2;_tmp7A6;});_LL547:;_LL548: return({struct _tuple10 _tmp7A7;
_tmp7A7.f1=(_tmp7A1->tq).q_const;_tmp7A7.f2=(void*)2;_tmp7A7;});_LL544:;}_LL53E:
if(_tmp7A0 <= (void*)1?1:*((int*)_tmp7A0)!= 3)goto _LL540;_tmp7A2=((struct Cyc_Absyn_Local_b_struct*)
_tmp7A0)->f1;_LL53F: {void*_tmp7A8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL54A: if(_tmp7A8 <= (void*)3?1:*((int*)_tmp7A8)!= 7)
goto _LL54C;_LL54B: return({struct _tuple10 _tmp7A9;_tmp7A9.f1=1;_tmp7A9.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp7A2->rgn))->v;_tmp7A9;});_LL54C:;_LL54D:
_tmp7A2->escapes=1;return({struct _tuple10 _tmp7AA;_tmp7AA.f1=(_tmp7A2->tq).q_const;
_tmp7AA.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7A2->rgn))->v;_tmp7AA;});
_LL549:;}_LL540: if(_tmp7A0 <= (void*)1?1:*((int*)_tmp7A0)!= 4)goto _LL542;_tmp7A3=((
struct Cyc_Absyn_Pat_b_struct*)_tmp7A0)->f1;_LL541: _tmp7A4=_tmp7A3;goto _LL543;
_LL542: if(_tmp7A0 <= (void*)1?1:*((int*)_tmp7A0)!= 2)goto _LL537;_tmp7A4=((struct
Cyc_Absyn_Param_b_struct*)_tmp7A0)->f1;_LL543: _tmp7A4->escapes=1;return({struct
_tuple10 _tmp7AB;_tmp7AB.f1=(_tmp7A4->tq).q_const;_tmp7AB.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7A4->rgn))->v;_tmp7AB;});_LL537:;}_LL52D: if(*((int*)_tmp796)!= 
21)goto _LL52F;_tmp799=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp796)->f1;
_tmp79A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp796)->f2;_LL52E: {void*
_tmp7AC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp799->topt))->v);
struct Cyc_List_List*_tmp7AD;struct Cyc_Absyn_AggrInfo _tmp7AE;void*_tmp7AF;struct
Cyc_Absyn_Aggrdecl**_tmp7B0;struct Cyc_Absyn_Aggrdecl*_tmp7B1;_LL54F: if(_tmp7AC <= (
void*)3?1:*((int*)_tmp7AC)!= 11)goto _LL551;_tmp7AD=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp7AC)->f2;_LL550: {struct Cyc_Absyn_Aggrfield*_tmp7B2=Cyc_Absyn_lookup_field(
_tmp7AD,_tmp79A);if(_tmp7B2 != 0?_tmp7B2->width != 0: 0)return({struct _tuple10
_tmp7B3;_tmp7B3.f1=(_tmp7B2->tq).q_const;_tmp7B3.f2=(Cyc_Tcutil_addressof_props(
te,_tmp799)).f2;_tmp7B3;});return bogus_ans;}_LL551: if(_tmp7AC <= (void*)3?1:*((
int*)_tmp7AC)!= 10)goto _LL553;_tmp7AE=((struct Cyc_Absyn_AggrType_struct*)_tmp7AC)->f1;
_tmp7AF=(void*)_tmp7AE.aggr_info;if(*((int*)_tmp7AF)!= 1)goto _LL553;_tmp7B0=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp7AF)->f1;_tmp7B1=*_tmp7B0;_LL552: {struct
Cyc_Absyn_Aggrfield*_tmp7B4=Cyc_Absyn_lookup_decl_field(_tmp7B1,_tmp79A);if(
_tmp7B4 != 0?_tmp7B4->width != 0: 0)return({struct _tuple10 _tmp7B5;_tmp7B5.f1=(
_tmp7B4->tq).q_const;_tmp7B5.f2=(Cyc_Tcutil_addressof_props(te,_tmp799)).f2;
_tmp7B5;});return bogus_ans;}_LL553:;_LL554: return bogus_ans;_LL54E:;}_LL52F: if(*((
int*)_tmp796)!= 22)goto _LL531;_tmp79B=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp796)->f1;_tmp79C=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp796)->f2;_LL530: {
void*_tmp7B6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp79B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7B7;void*_tmp7B8;void*_tmp7B9;
_LL556: if(_tmp7B6 <= (void*)3?1:*((int*)_tmp7B6)!= 4)goto _LL558;_tmp7B7=((struct
Cyc_Absyn_PointerType_struct*)_tmp7B6)->f1;_tmp7B8=(void*)_tmp7B7.elt_typ;
_tmp7B9=(void*)_tmp7B7.rgn_typ;_LL557: {struct Cyc_Absyn_Aggrfield*finfo;{void*
_tmp7BA=Cyc_Tcutil_compress(_tmp7B8);struct Cyc_List_List*_tmp7BB;struct Cyc_Absyn_AggrInfo
_tmp7BC;void*_tmp7BD;struct Cyc_Absyn_Aggrdecl**_tmp7BE;struct Cyc_Absyn_Aggrdecl*
_tmp7BF;_LL55B: if(_tmp7BA <= (void*)3?1:*((int*)_tmp7BA)!= 11)goto _LL55D;_tmp7BB=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp7BA)->f2;_LL55C: finfo=Cyc_Absyn_lookup_field(
_tmp7BB,_tmp79C);goto _LL55A;_LL55D: if(_tmp7BA <= (void*)3?1:*((int*)_tmp7BA)!= 10)
goto _LL55F;_tmp7BC=((struct Cyc_Absyn_AggrType_struct*)_tmp7BA)->f1;_tmp7BD=(void*)
_tmp7BC.aggr_info;if(*((int*)_tmp7BD)!= 1)goto _LL55F;_tmp7BE=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp7BD)->f1;_tmp7BF=*_tmp7BE;_LL55E: finfo=Cyc_Absyn_lookup_decl_field(_tmp7BF,
_tmp79C);goto _LL55A;_LL55F:;_LL560: return bogus_ans;_LL55A:;}if(finfo != 0?finfo->width
!= 0: 0)return({struct _tuple10 _tmp7C0;_tmp7C0.f1=(finfo->tq).q_const;_tmp7C0.f2=
_tmp7B9;_tmp7C0;});return bogus_ans;}_LL558:;_LL559: return bogus_ans;_LL555:;}
_LL531: if(*((int*)_tmp796)!= 20)goto _LL533;_tmp79D=((struct Cyc_Absyn_Deref_e_struct*)
_tmp796)->f1;_LL532: {void*_tmp7C1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp79D->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7C2;void*_tmp7C3;
struct Cyc_Absyn_Tqual _tmp7C4;_LL562: if(_tmp7C1 <= (void*)3?1:*((int*)_tmp7C1)!= 4)
goto _LL564;_tmp7C2=((struct Cyc_Absyn_PointerType_struct*)_tmp7C1)->f1;_tmp7C3=(
void*)_tmp7C2.rgn_typ;_tmp7C4=_tmp7C2.tq;_LL563: return({struct _tuple10 _tmp7C5;
_tmp7C5.f1=_tmp7C4.q_const;_tmp7C5.f2=_tmp7C3;_tmp7C5;});_LL564:;_LL565: return
bogus_ans;_LL561:;}_LL533: if(*((int*)_tmp796)!= 23)goto _LL535;_tmp79E=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp796)->f1;_tmp79F=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp796)->f2;_LL534: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp79E->topt))->v);void*_tmp7C6=t;struct Cyc_List_List*_tmp7C7;
struct Cyc_Absyn_PtrInfo _tmp7C8;void*_tmp7C9;void*_tmp7CA;struct Cyc_Absyn_Tqual
_tmp7CB;struct Cyc_Absyn_Conref*_tmp7CC;struct Cyc_Absyn_Tqual _tmp7CD;_LL567: if(
_tmp7C6 <= (void*)3?1:*((int*)_tmp7C6)!= 9)goto _LL569;_tmp7C7=((struct Cyc_Absyn_TupleType_struct*)
_tmp7C6)->f1;_LL568: {unsigned int _tmp7CF;int _tmp7D0;struct _tuple7 _tmp7CE=Cyc_Evexp_eval_const_uint_exp(
_tmp79F);_tmp7CF=_tmp7CE.f1;_tmp7D0=_tmp7CE.f2;if(!_tmp7D0)return bogus_ans;{
struct _tuple4*_tmp7D1=Cyc_Absyn_lookup_tuple_field(_tmp7C7,(int)_tmp7CF);if(
_tmp7D1 != 0)return({struct _tuple10 _tmp7D2;_tmp7D2.f1=((*_tmp7D1).f1).q_const;
_tmp7D2.f2=(Cyc_Tcutil_addressof_props(te,_tmp79E)).f2;_tmp7D2;});return
bogus_ans;}}_LL569: if(_tmp7C6 <= (void*)3?1:*((int*)_tmp7C6)!= 4)goto _LL56B;
_tmp7C8=((struct Cyc_Absyn_PointerType_struct*)_tmp7C6)->f1;_tmp7C9=(void*)
_tmp7C8.elt_typ;_tmp7CA=(void*)_tmp7C8.rgn_typ;_tmp7CB=_tmp7C8.tq;_tmp7CC=
_tmp7C8.bounds;_LL56A: return({struct _tuple10 _tmp7D3;_tmp7D3.f1=_tmp7CB.q_const;
_tmp7D3.f2=_tmp7CA;_tmp7D3;});_LL56B: if(_tmp7C6 <= (void*)3?1:*((int*)_tmp7C6)!= 
7)goto _LL56D;_tmp7CD=((struct Cyc_Absyn_ArrayType_struct*)_tmp7C6)->f2;_LL56C:
return({struct _tuple10 _tmp7D4;_tmp7D4.f1=_tmp7CD.q_const;_tmp7D4.f2=(Cyc_Tcutil_addressof_props(
te,_tmp79E)).f2;_tmp7D4;});_LL56D:;_LL56E: return bogus_ans;_LL566:;}_LL535:;
_LL536:({void*_tmp7D5[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("unary & applied to non-lvalue",
sizeof(char),30),_tag_arr(_tmp7D5,sizeof(void*),0));});return bogus_ans;_LL52A:;}
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*
e){void*_tmp7D7=Cyc_Tcutil_compress(e_typ);void*_tmp7D8;struct Cyc_Absyn_Tqual
_tmp7D9;_LL570: if(_tmp7D7 <= (void*)3?1:*((int*)_tmp7D7)!= 7)goto _LL572;_tmp7D8=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp7D7)->f1;_tmp7D9=((struct Cyc_Absyn_ArrayType_struct*)
_tmp7D7)->f2;_LL571: {void*_tmp7DB;struct _tuple10 _tmp7DA=Cyc_Tcutil_addressof_props(
te,e);_tmp7DB=_tmp7DA.f2;return Cyc_Absyn_atb_typ(_tmp7D8,_tmp7DB,_tmp7D9,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp7DC=_cycalloc(sizeof(*_tmp7DC));_tmp7DC[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp7DD;_tmp7DD.tag=0;_tmp7DD.f1=e;_tmp7DD;});
_tmp7DC;}));}_LL572:;_LL573: return e_typ;_LL56F:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{void*_tmp7DE=(void*)b->v;void*_tmp7DF;void*_tmp7E0;struct Cyc_Absyn_Exp*
_tmp7E1;_LL575: if(_tmp7DE <= (void*)1?1:*((int*)_tmp7DE)!= 0)goto _LL577;_tmp7DF=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7DE)->f1;if((int)_tmp7DF != 0)goto
_LL577;_LL576: return;_LL577: if(_tmp7DE <= (void*)1?1:*((int*)_tmp7DE)!= 0)goto
_LL579;_tmp7E0=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7DE)->f1;if(
_tmp7E0 <= (void*)1?1:*((int*)_tmp7E0)!= 0)goto _LL579;_tmp7E1=((struct Cyc_Absyn_Upper_b_struct*)
_tmp7E0)->f1;_LL578: {unsigned int _tmp7E3;int _tmp7E4;struct _tuple7 _tmp7E2=Cyc_Evexp_eval_const_uint_exp(
_tmp7E1);_tmp7E3=_tmp7E2.f1;_tmp7E4=_tmp7E2.f2;if(_tmp7E4?_tmp7E3 <= i: 0)({struct
Cyc_Std_Int_pa_struct _tmp7E7;_tmp7E7.tag=1;_tmp7E7.f1=(int)i;{struct Cyc_Std_Int_pa_struct
_tmp7E6;_tmp7E6.tag=1;_tmp7E6.f1=(int)_tmp7E3;{void*_tmp7E5[2]={& _tmp7E6,&
_tmp7E7};Cyc_Tcutil_terr(loc,_tag_arr("dereference is out of bounds: %d <= %d",
sizeof(char),39),_tag_arr(_tmp7E5,sizeof(void*),2));}}});return;}_LL579:;_LL57A:(
void*)(b->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp7E8=_cycalloc(
sizeof(*_tmp7E8));_tmp7E8[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp7E9;_tmp7E9.tag=
0;_tmp7E9.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp7EA=_cycalloc(
sizeof(*_tmp7EA));_tmp7EA[0]=({struct Cyc_Absyn_Upper_b_struct _tmp7EB;_tmp7EB.tag=
0;_tmp7EB.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmp7EB;});_tmp7EA;}));_tmp7E9;});
_tmp7E8;})));return;_LL574:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp7EC=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp7ED;struct Cyc_Absyn_Exp*_tmp7EE;_LL57C: if(_tmp7EC <= (void*)1?1:*((int*)
_tmp7EC)!= 0)goto _LL57E;_tmp7ED=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7EC)->f1;if(_tmp7ED <= (void*)1?1:*((int*)_tmp7ED)!= 0)goto _LL57E;_tmp7EE=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7ED)->f1;_LL57D: {unsigned int _tmp7F0;int
_tmp7F1;struct _tuple7 _tmp7EF=Cyc_Evexp_eval_const_uint_exp(_tmp7EE);_tmp7F0=
_tmp7EF.f1;_tmp7F1=_tmp7EF.f2;return _tmp7F1?_tmp7F0 == 1: 0;}_LL57E:;_LL57F: return
0;_LL57B:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp7F2=Cyc_Tcutil_compress(t);
void*_tmp7F3;struct Cyc_List_List*_tmp7F4;struct Cyc_Absyn_AggrInfo _tmp7F5;void*
_tmp7F6;void*_tmp7F7;struct Cyc_Absyn_AggrInfo _tmp7F8;void*_tmp7F9;void*_tmp7FA;
struct Cyc_Absyn_AggrInfo _tmp7FB;void*_tmp7FC;struct Cyc_Absyn_Aggrdecl**_tmp7FD;
struct Cyc_Absyn_Aggrdecl*_tmp7FE;struct Cyc_List_List*_tmp7FF;struct Cyc_List_List*
_tmp800;_LL581: if((int)_tmp7F2 != 0)goto _LL583;_LL582: goto _LL584;_LL583: if(
_tmp7F2 <= (void*)3?1:*((int*)_tmp7F2)!= 5)goto _LL585;_LL584: goto _LL586;_LL585:
if((int)_tmp7F2 != 1)goto _LL587;_LL586: goto _LL588;_LL587: if(_tmp7F2 <= (void*)3?1:*((
int*)_tmp7F2)!= 6)goto _LL589;_LL588: return 1;_LL589: if(_tmp7F2 <= (void*)3?1:*((
int*)_tmp7F2)!= 12)goto _LL58B;_LL58A: goto _LL58C;_LL58B: if(_tmp7F2 <= (void*)3?1:*((
int*)_tmp7F2)!= 13)goto _LL58D;_LL58C: return 0;_LL58D: if(_tmp7F2 <= (void*)3?1:*((
int*)_tmp7F2)!= 7)goto _LL58F;_tmp7F3=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp7F2)->f1;_LL58E: return Cyc_Tcutil_bits_only(_tmp7F3);_LL58F: if(_tmp7F2 <= (
void*)3?1:*((int*)_tmp7F2)!= 9)goto _LL591;_tmp7F4=((struct Cyc_Absyn_TupleType_struct*)
_tmp7F2)->f1;_LL590: for(0;_tmp7F4 != 0;_tmp7F4=_tmp7F4->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp7F4->hd)).f2))return 0;}return 1;_LL591: if(_tmp7F2 <= (void*)3?1:*((
int*)_tmp7F2)!= 10)goto _LL593;_tmp7F5=((struct Cyc_Absyn_AggrType_struct*)_tmp7F2)->f1;
_tmp7F6=(void*)_tmp7F5.aggr_info;if(*((int*)_tmp7F6)!= 0)goto _LL593;_tmp7F7=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp7F6)->f1;if((int)_tmp7F7 != 1)
goto _LL593;_LL592: return 1;_LL593: if(_tmp7F2 <= (void*)3?1:*((int*)_tmp7F2)!= 10)
goto _LL595;_tmp7F8=((struct Cyc_Absyn_AggrType_struct*)_tmp7F2)->f1;_tmp7F9=(void*)
_tmp7F8.aggr_info;if(*((int*)_tmp7F9)!= 0)goto _LL595;_tmp7FA=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp7F9)->f1;if((int)_tmp7FA != 0)goto _LL595;_LL594: return 0;_LL595: if(_tmp7F2 <= (
void*)3?1:*((int*)_tmp7F2)!= 10)goto _LL597;_tmp7FB=((struct Cyc_Absyn_AggrType_struct*)
_tmp7F2)->f1;_tmp7FC=(void*)_tmp7FB.aggr_info;if(*((int*)_tmp7FC)!= 1)goto _LL597;
_tmp7FD=((struct Cyc_Absyn_KnownAggr_struct*)_tmp7FC)->f1;_tmp7FE=*_tmp7FD;
_tmp7FF=_tmp7FB.targs;_LL596: if((void*)_tmp7FE->kind == (void*)1)return 1;if(
_tmp7FE->impl == 0)return 0;{struct _RegionHandle _tmp801=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp801;_push_region(rgn);{struct Cyc_List_List*_tmp802=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp7FE->tvs,_tmp7FF);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7FE->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp802,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp803=0;_npop_handler(0);
return _tmp803;}}}{int _tmp804=1;_npop_handler(0);return _tmp804;}};_pop_region(rgn);}
_LL597: if(_tmp7F2 <= (void*)3?1:*((int*)_tmp7F2)!= 11)goto _LL599;_tmp800=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp7F2)->f2;_LL598: for(0;_tmp800 != 0;_tmp800=
_tmp800->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp800->hd)->type))return 0;}return 1;_LL599:;_LL59A: return 0;_LL580:;}struct
_tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp805=(void*)e->r;
struct _tuple1*_tmp806;void*_tmp807;struct Cyc_Absyn_Exp*_tmp808;struct Cyc_Absyn_Exp*
_tmp809;struct Cyc_Absyn_Exp*_tmp80A;struct Cyc_Absyn_Exp*_tmp80B;struct Cyc_Absyn_Exp*
_tmp80C;struct Cyc_Absyn_Exp*_tmp80D;struct Cyc_Absyn_Exp*_tmp80E;void*_tmp80F;
struct Cyc_Absyn_Exp*_tmp810;struct Cyc_Absyn_Exp*_tmp811;struct Cyc_Absyn_Exp*
_tmp812;struct Cyc_Absyn_Exp*_tmp813;struct Cyc_List_List*_tmp814;struct Cyc_List_List*
_tmp815;struct Cyc_List_List*_tmp816;void*_tmp817;struct Cyc_List_List*_tmp818;
struct Cyc_List_List*_tmp819;struct Cyc_List_List*_tmp81A;_LL59C: if(*((int*)
_tmp805)!= 0)goto _LL59E;_LL59D: goto _LL59F;_LL59E: if(*((int*)_tmp805)!= 16)goto
_LL5A0;_LL59F: goto _LL5A1;_LL5A0: if(*((int*)_tmp805)!= 17)goto _LL5A2;_LL5A1: goto
_LL5A3;_LL5A2: if(*((int*)_tmp805)!= 18)goto _LL5A4;_LL5A3: goto _LL5A5;_LL5A4: if(*((
int*)_tmp805)!= 19)goto _LL5A6;_LL5A5: goto _LL5A7;_LL5A6: if(*((int*)_tmp805)!= 31)
goto _LL5A8;_LL5A7: goto _LL5A9;_LL5A8: if(*((int*)_tmp805)!= 32)goto _LL5AA;_LL5A9:
return 1;_LL5AA: if(*((int*)_tmp805)!= 1)goto _LL5AC;_tmp806=((struct Cyc_Absyn_Var_e_struct*)
_tmp805)->f1;_tmp807=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp805)->f2;_LL5AB: {
void*_tmp81B=_tmp807;struct Cyc_Absyn_Vardecl*_tmp81C;_LL5C9: if(_tmp81B <= (void*)
1?1:*((int*)_tmp81B)!= 1)goto _LL5CB;_LL5CA: return 1;_LL5CB: if(_tmp81B <= (void*)1?
1:*((int*)_tmp81B)!= 0)goto _LL5CD;_tmp81C=((struct Cyc_Absyn_Global_b_struct*)
_tmp81B)->f1;_LL5CC: {void*_tmp81D=Cyc_Tcutil_compress((void*)_tmp81C->type);
_LL5D2: if(_tmp81D <= (void*)3?1:*((int*)_tmp81D)!= 7)goto _LL5D4;_LL5D3: goto _LL5D5;
_LL5D4: if(_tmp81D <= (void*)3?1:*((int*)_tmp81D)!= 8)goto _LL5D6;_LL5D5: return 1;
_LL5D6:;_LL5D7: return var_okay;_LL5D1:;}_LL5CD: if((int)_tmp81B != 0)goto _LL5CF;
_LL5CE: return 0;_LL5CF:;_LL5D0: return var_okay;_LL5C8:;}_LL5AC: if(*((int*)_tmp805)
!= 6)goto _LL5AE;_tmp808=((struct Cyc_Absyn_Conditional_e_struct*)_tmp805)->f1;
_tmp809=((struct Cyc_Absyn_Conditional_e_struct*)_tmp805)->f2;_tmp80A=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp805)->f3;_LL5AD: return(Cyc_Tcutil_cnst_exp(te,0,_tmp808)?Cyc_Tcutil_cnst_exp(
te,0,_tmp809): 0)?Cyc_Tcutil_cnst_exp(te,0,_tmp80A): 0;_LL5AE: if(*((int*)_tmp805)
!= 7)goto _LL5B0;_tmp80B=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp805)->f1;_tmp80C=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp805)->f2;_LL5AF: return Cyc_Tcutil_cnst_exp(
te,0,_tmp80B)?Cyc_Tcutil_cnst_exp(te,0,_tmp80C): 0;_LL5B0: if(*((int*)_tmp805)!= 
11)goto _LL5B2;_tmp80D=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp805)->f1;
_LL5B1: _tmp80E=_tmp80D;goto _LL5B3;_LL5B2: if(*((int*)_tmp805)!= 12)goto _LL5B4;
_tmp80E=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp805)->f1;_LL5B3: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp80E);_LL5B4: if(*((int*)_tmp805)!= 13)goto _LL5B6;_tmp80F=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp805)->f1;_tmp810=((struct Cyc_Absyn_Cast_e_struct*)
_tmp805)->f2;_LL5B5: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp810);_LL5B6: if(*((
int*)_tmp805)!= 14)goto _LL5B8;_tmp811=((struct Cyc_Absyn_Address_e_struct*)
_tmp805)->f1;_LL5B7: return Cyc_Tcutil_cnst_exp(te,1,_tmp811);_LL5B8: if(*((int*)
_tmp805)!= 27)goto _LL5BA;_tmp812=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp805)->f2;_tmp813=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp805)->f3;
_LL5B9: return Cyc_Tcutil_cnst_exp(te,0,_tmp812)?Cyc_Tcutil_cnst_exp(te,0,_tmp813):
0;_LL5BA: if(*((int*)_tmp805)!= 26)goto _LL5BC;_tmp814=((struct Cyc_Absyn_Array_e_struct*)
_tmp805)->f1;_LL5BB: _tmp815=_tmp814;goto _LL5BD;_LL5BC: if(*((int*)_tmp805)!= 29)
goto _LL5BE;_tmp815=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp805)->f2;_LL5BD:
_tmp816=_tmp815;goto _LL5BF;_LL5BE: if(*((int*)_tmp805)!= 28)goto _LL5C0;_tmp816=((
struct Cyc_Absyn_Struct_e_struct*)_tmp805)->f3;_LL5BF: for(0;_tmp816 != 0;_tmp816=
_tmp816->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp816->hd)).f2))
return 0;}return 1;_LL5C0: if(*((int*)_tmp805)!= 3)goto _LL5C2;_tmp817=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp805)->f1;_tmp818=((struct Cyc_Absyn_Primop_e_struct*)
_tmp805)->f2;_LL5C1: _tmp819=_tmp818;goto _LL5C3;_LL5C2: if(*((int*)_tmp805)!= 24)
goto _LL5C4;_tmp819=((struct Cyc_Absyn_Tuple_e_struct*)_tmp805)->f1;_LL5C3: _tmp81A=
_tmp819;goto _LL5C5;_LL5C4: if(*((int*)_tmp805)!= 30)goto _LL5C6;_tmp81A=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp805)->f1;_LL5C5: for(0;_tmp81A != 0;_tmp81A=_tmp81A->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp81A->hd))return 0;}return 1;
_LL5C6:;_LL5C7: return 0;_LL59B:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp81E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp81F;
struct Cyc_Absyn_Conref*_tmp820;struct Cyc_Absyn_Conref*_tmp821;void*_tmp822;
struct Cyc_List_List*_tmp823;struct Cyc_Absyn_AggrInfo _tmp824;void*_tmp825;struct
Cyc_List_List*_tmp826;struct Cyc_List_List*_tmp827;_LL5D9: if((int)_tmp81E != 0)
goto _LL5DB;_LL5DA: goto _LL5DC;_LL5DB: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 5)
goto _LL5DD;_LL5DC: goto _LL5DE;_LL5DD: if((int)_tmp81E != 1)goto _LL5DF;_LL5DE: goto
_LL5E0;_LL5DF: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 6)goto _LL5E1;_LL5E0:
return 1;_LL5E1: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 4)goto _LL5E3;_tmp81F=((
struct Cyc_Absyn_PointerType_struct*)_tmp81E)->f1;_tmp820=_tmp81F.nullable;
_tmp821=_tmp81F.bounds;_LL5E2: {void*_tmp828=(void*)(Cyc_Absyn_compress_conref(
_tmp821))->v;void*_tmp829;void*_tmp82A;_LL5F2: if(_tmp828 <= (void*)1?1:*((int*)
_tmp828)!= 0)goto _LL5F4;_tmp829=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp828)->f1;if((int)_tmp829 != 0)goto _LL5F4;_LL5F3: return 1;_LL5F4: if(_tmp828 <= (
void*)1?1:*((int*)_tmp828)!= 0)goto _LL5F6;_tmp82A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp828)->f1;if(_tmp82A <= (void*)1?1:*((int*)_tmp82A)!= 0)goto _LL5F6;_LL5F5: {
void*_tmp82B=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp820))->v;int _tmp82C;_LL5F9: if(_tmp82B <= (void*)1?1:*((int*)_tmp82B)!= 0)goto
_LL5FB;_tmp82C=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp82B)->f1;_LL5FA:
return _tmp82C;_LL5FB:;_LL5FC: return 0;_LL5F8:;}_LL5F6:;_LL5F7: return 0;_LL5F1:;}
_LL5E3: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 7)goto _LL5E5;_tmp822=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp81E)->f1;_LL5E4: return Cyc_Tcutil_supports_default(
_tmp822);_LL5E5: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 9)goto _LL5E7;_tmp823=((
struct Cyc_Absyn_TupleType_struct*)_tmp81E)->f1;_LL5E6: for(0;_tmp823 != 0;_tmp823=
_tmp823->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple4*)_tmp823->hd)).f2))
return 0;}return 1;_LL5E7: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 10)goto _LL5E9;
_tmp824=((struct Cyc_Absyn_AggrType_struct*)_tmp81E)->f1;_tmp825=(void*)_tmp824.aggr_info;
_tmp826=_tmp824.targs;_LL5E8: {struct Cyc_Absyn_Aggrdecl*_tmp82D=Cyc_Absyn_get_known_aggrdecl(
_tmp825);if(_tmp82D->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp82D->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp82D->tvs,_tmp826,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82D->impl))->fields);}
_LL5E9: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 11)goto _LL5EB;_tmp827=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp81E)->f2;_LL5EA: return Cyc_Tcutil_fields_support_default(
0,0,_tmp827);_LL5EB: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 13)goto _LL5ED;
_LL5EC: goto _LL5EE;_LL5ED: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 12)goto
_LL5EF;_LL5EE: return 1;_LL5EF:;_LL5F0: return 0;_LL5D8:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct
_RegionHandle _tmp82E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp82E;
_push_region(rgn);{struct Cyc_List_List*_tmp82F=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp82F,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp830=0;_npop_handler(0);return _tmp830;}}};_pop_region(rgn);}return 1;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp831=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp832;void*_tmp833;struct Cyc_Absyn_FnInfo _tmp834;struct Cyc_List_List*_tmp835;
_LL5FE: if(_tmp831 <= (void*)3?1:*((int*)_tmp831)!= 4)goto _LL600;_tmp832=((struct
Cyc_Absyn_PointerType_struct*)_tmp831)->f1;_tmp833=(void*)_tmp832.elt_typ;_LL5FF:
return Cyc_Tcutil_is_noreturn(_tmp833);_LL600: if(_tmp831 <= (void*)3?1:*((int*)
_tmp831)!= 8)goto _LL602;_tmp834=((struct Cyc_Absyn_FnType_struct*)_tmp831)->f1;
_tmp835=_tmp834.attributes;_LL601: for(0;_tmp835 != 0;_tmp835=_tmp835->tl){void*
_tmp836=(void*)_tmp835->hd;_LL605: if((int)_tmp836 != 3)goto _LL607;_LL606: return 1;
_LL607:;_LL608: continue;_LL604:;}goto _LL5FD;_LL602:;_LL603: goto _LL5FD;_LL5FD:;}
return 0;}
