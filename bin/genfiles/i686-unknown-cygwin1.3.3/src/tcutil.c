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
goto _LLD9;_tmp123=((struct Cyc_Absyn_Upper_b_struct*)_tmp122)->f1;_LLE1: return Cyc_Evexp_const_exp_cmp(
_tmp121,_tmp123);_LLD9:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){if(a1 == 
a2)return 1;{struct _tuple6 _tmp125=({struct _tuple6 _tmp124;_tmp124.f1=a1;_tmp124.f2=
a2;_tmp124;});void*_tmp126;void*_tmp127;int _tmp128;int _tmp129;void*_tmp12A;void*
_tmp12B;int _tmp12C;int _tmp12D;void*_tmp12E;int _tmp12F;void*_tmp130;int _tmp131;
void*_tmp132;int _tmp133;void*_tmp134;int _tmp135;void*_tmp136;struct _tagged_arr
_tmp137;void*_tmp138;struct _tagged_arr _tmp139;_LLE3: _tmp126=_tmp125.f1;if(
_tmp126 <= (void*)16?1:*((int*)_tmp126)!= 3)goto _LLE5;_tmp127=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp126)->f1;_tmp128=((struct Cyc_Absyn_Format_att_struct*)_tmp126)->f2;_tmp129=((
struct Cyc_Absyn_Format_att_struct*)_tmp126)->f3;_tmp12A=_tmp125.f2;if(_tmp12A <= (
void*)16?1:*((int*)_tmp12A)!= 3)goto _LLE5;_tmp12B=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp12A)->f1;_tmp12C=((struct Cyc_Absyn_Format_att_struct*)_tmp12A)->f2;_tmp12D=((
struct Cyc_Absyn_Format_att_struct*)_tmp12A)->f3;_LLE4: return(_tmp127 == _tmp12B?
_tmp128 == _tmp12C: 0)?_tmp129 == _tmp12D: 0;_LLE5: _tmp12E=_tmp125.f1;if(_tmp12E <= (
void*)16?1:*((int*)_tmp12E)!= 0)goto _LLE7;_tmp12F=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp12E)->f1;_tmp130=_tmp125.f2;if(_tmp130 <= (void*)16?1:*((int*)_tmp130)!= 0)
goto _LLE7;_tmp131=((struct Cyc_Absyn_Regparm_att_struct*)_tmp130)->f1;_LLE6:
_tmp133=_tmp12F;_tmp135=_tmp131;goto _LLE8;_LLE7: _tmp132=_tmp125.f1;if(_tmp132 <= (
void*)16?1:*((int*)_tmp132)!= 1)goto _LLE9;_tmp133=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp132)->f1;_tmp134=_tmp125.f2;if(_tmp134 <= (void*)16?1:*((int*)_tmp134)!= 1)
goto _LLE9;_tmp135=((struct Cyc_Absyn_Aligned_att_struct*)_tmp134)->f1;_LLE8:
return _tmp133 == _tmp135;_LLE9: _tmp136=_tmp125.f1;if(_tmp136 <= (void*)16?1:*((int*)
_tmp136)!= 2)goto _LLEB;_tmp137=((struct Cyc_Absyn_Section_att_struct*)_tmp136)->f1;
_tmp138=_tmp125.f2;if(_tmp138 <= (void*)16?1:*((int*)_tmp138)!= 2)goto _LLEB;
_tmp139=((struct Cyc_Absyn_Section_att_struct*)_tmp138)->f1;_LLEA: return Cyc_Std_strcmp(
_tmp137,_tmp139)== 0;_LLEB:;_LLEC: return 0;_LLE2:;}}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return
0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((
void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*
tv){void*t;{void*_tmp13A=Cyc_Tcutil_tvar_kind(tv);_LLEE: if((int)_tmp13A != 3)goto
_LLF0;_LLEF: t=(void*)2;goto _LLED;_LLF0: if((int)_tmp13A != 4)goto _LLF2;_LLF1: t=Cyc_Absyn_empty_effect;
goto _LLED;_LLF2:;_LLF3: t=Cyc_Absyn_sint_typ;goto _LLED;_LLED:;}return({struct
_tuple8*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->f1=tv;_tmp13B->f2=t;_tmp13B;});}
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
void*_tmp16C=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
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
return 0;_LL14C:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp1A7=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp1A8;void*_tmp1A9;struct Cyc_Core_Opt*_tmp1AA;struct
Cyc_Core_Opt*_tmp1AB;struct Cyc_Core_Opt**_tmp1AC;struct Cyc_Core_Opt*_tmp1AD;
_LL167: if(_tmp1A7 <= (void*)3?1:*((int*)_tmp1A7)!= 19)goto _LL169;_LL168: return 0;
_LL169: if(_tmp1A7 <= (void*)3?1:*((int*)_tmp1A7)!= 20)goto _LL16B;_tmp1A8=((struct
Cyc_Absyn_JoinEff_struct*)_tmp1A7)->f1;_LL16A: for(0;_tmp1A8 != 0;_tmp1A8=_tmp1A8->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp1A8->hd))return 1;}
return 0;_LL16B: if(_tmp1A7 <= (void*)3?1:*((int*)_tmp1A7)!= 21)goto _LL16D;_tmp1A9=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1A7)->f1;_LL16C: _tmp1A9=Cyc_Tcutil_compress(
_tmp1A9);if(t == _tmp1A9)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,
_tmp1A9);{void*_tmp1AE=Cyc_Tcutil_rgns_of(t);void*_tmp1AF;_LL172: if(_tmp1AE <= (
void*)3?1:*((int*)_tmp1AE)!= 21)goto _LL174;_tmp1AF=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1AE)->f1;_LL173: {struct _tuple6 _tmp1B1=({struct _tuple6 _tmp1B0;_tmp1B0.f1=Cyc_Tcutil_compress(
_tmp1AF);_tmp1B0.f2=_tmp1A9;_tmp1B0;});void*_tmp1B2;struct Cyc_Absyn_Tvar*_tmp1B3;
void*_tmp1B4;struct Cyc_Absyn_Tvar*_tmp1B5;_LL177: _tmp1B2=_tmp1B1.f1;if(_tmp1B2 <= (
void*)3?1:*((int*)_tmp1B2)!= 1)goto _LL179;_tmp1B3=((struct Cyc_Absyn_VarType_struct*)
_tmp1B2)->f1;_tmp1B4=_tmp1B1.f2;if(_tmp1B4 <= (void*)3?1:*((int*)_tmp1B4)!= 1)
goto _LL179;_tmp1B5=((struct Cyc_Absyn_VarType_struct*)_tmp1B4)->f1;_LL178: return
Cyc_Tcutil_unify(t,_tmp1A9);_LL179:;_LL17A: return _tmp1AF == _tmp1A9;_LL176:;}
_LL174:;_LL175: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp1AE);
_LL171:;}_LL16D: if(_tmp1A7 <= (void*)3?1:*((int*)_tmp1A7)!= 0)goto _LL16F;_tmp1AA=((
struct Cyc_Absyn_Evar_struct*)_tmp1A7)->f1;_tmp1AB=((struct Cyc_Absyn_Evar_struct*)
_tmp1A7)->f2;_tmp1AC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1A7)->f2;_tmp1AD=((struct Cyc_Absyn_Evar_struct*)_tmp1A7)->f4;_LL16E: if(
_tmp1AA == 0?1:(void*)_tmp1AA->v != (void*)4)({void*_tmp1B6[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",
sizeof(char),27),_tag_arr(_tmp1B6,sizeof(void*),0));});if(!may_constrain_evars)
return 0;{void*_tmp1B7=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1AD);Cyc_Tcutil_occurs(_tmp1B7,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1AD))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp1B8=({struct Cyc_Absyn_JoinEff_struct*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));
_tmp1BA[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1BB;_tmp1BB.tag=20;_tmp1BB.f1=({
struct Cyc_List_List*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->hd=(void*)
_tmp1B7;_tmp1BC->tl=({struct Cyc_List_List*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));
_tmp1BD->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp1BE=_cycalloc(
sizeof(*_tmp1BE));_tmp1BE[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp1BF;_tmp1BF.tag=
21;_tmp1BF.f1=(void*)t;_tmp1BF;});_tmp1BE;}));_tmp1BD->tl=0;_tmp1BD;});_tmp1BC;});
_tmp1BB;});_tmp1BA;});*_tmp1AC=({struct Cyc_Core_Opt*_tmp1B9=_cycalloc(sizeof(*
_tmp1B9));_tmp1B9->v=(void*)((void*)_tmp1B8);_tmp1B9;});return 1;}}_LL16F:;_LL170:
return 0;_LL166:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1C0=e;struct
Cyc_Absyn_Tvar*_tmp1C1;struct Cyc_List_List*_tmp1C2;void*_tmp1C3;struct Cyc_Core_Opt*
_tmp1C4;struct Cyc_Core_Opt*_tmp1C5;struct Cyc_Core_Opt**_tmp1C6;struct Cyc_Core_Opt*
_tmp1C7;_LL17C: if(_tmp1C0 <= (void*)3?1:*((int*)_tmp1C0)!= 1)goto _LL17E;_tmp1C1=((
struct Cyc_Absyn_VarType_struct*)_tmp1C0)->f1;_LL17D: return Cyc_Absyn_tvar_cmp(v,
_tmp1C1)== 0;_LL17E: if(_tmp1C0 <= (void*)3?1:*((int*)_tmp1C0)!= 20)goto _LL180;
_tmp1C2=((struct Cyc_Absyn_JoinEff_struct*)_tmp1C0)->f1;_LL17F: for(0;_tmp1C2 != 0;
_tmp1C2=_tmp1C2->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp1C2->hd))return 1;}return 0;_LL180: if(_tmp1C0 <= (void*)3?1:*((int*)
_tmp1C0)!= 21)goto _LL182;_tmp1C3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1C0)->f1;_LL181: {void*_tmp1C8=Cyc_Tcutil_rgns_of(_tmp1C3);void*_tmp1C9;
_LL187: if(_tmp1C8 <= (void*)3?1:*((int*)_tmp1C8)!= 21)goto _LL189;_tmp1C9=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1C8)->f1;_LL188: if(!may_constrain_evars)
return 0;{void*_tmp1CA=Cyc_Tcutil_compress(_tmp1C9);struct Cyc_Core_Opt*_tmp1CB;
struct Cyc_Core_Opt*_tmp1CC;struct Cyc_Core_Opt**_tmp1CD;struct Cyc_Core_Opt*
_tmp1CE;_LL18C: if(_tmp1CA <= (void*)3?1:*((int*)_tmp1CA)!= 0)goto _LL18E;_tmp1CB=((
struct Cyc_Absyn_Evar_struct*)_tmp1CA)->f1;_tmp1CC=((struct Cyc_Absyn_Evar_struct*)
_tmp1CA)->f2;_tmp1CD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1CA)->f2;_tmp1CE=((struct Cyc_Absyn_Evar_struct*)_tmp1CA)->f4;_LL18D: {void*
_tmp1CF=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1CE);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1CE))->v,v))return 0;{void*_tmp1D0=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));
_tmp1D2[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1D3;_tmp1D3.tag=20;_tmp1D3.f1=({
void*_tmp1D4[2];_tmp1D4[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp1D5=
_cycalloc(sizeof(*_tmp1D5));_tmp1D5[0]=({struct Cyc_Absyn_VarType_struct _tmp1D6;
_tmp1D6.tag=1;_tmp1D6.f1=v;_tmp1D6;});_tmp1D5;});_tmp1D4[0]=_tmp1CF;Cyc_List_list(
_tag_arr(_tmp1D4,sizeof(void*),2));});_tmp1D3;});_tmp1D2;}));*_tmp1CD=({struct
Cyc_Core_Opt*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->v=(void*)_tmp1D0;
_tmp1D1;});return 1;}}_LL18E:;_LL18F: return 0;_LL18B:;}_LL189:;_LL18A: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp1C8);_LL186:;}_LL182: if(_tmp1C0 <= (void*)3?1:*((int*)
_tmp1C0)!= 0)goto _LL184;_tmp1C4=((struct Cyc_Absyn_Evar_struct*)_tmp1C0)->f1;
_tmp1C5=((struct Cyc_Absyn_Evar_struct*)_tmp1C0)->f2;_tmp1C6=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1C0)->f2;_tmp1C7=((struct Cyc_Absyn_Evar_struct*)
_tmp1C0)->f4;_LL183: if(_tmp1C4 == 0?1:(void*)_tmp1C4->v != (void*)4)({void*_tmp1D7[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(char),27),_tag_arr(_tmp1D7,sizeof(
void*),0));});{void*_tmp1D8=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1C7);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1C7))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct*_tmp1D9=({struct Cyc_Absyn_JoinEff_struct*_tmp1DB=
_cycalloc(sizeof(*_tmp1DB));_tmp1DB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1DC;
_tmp1DC.tag=20;_tmp1DC.f1=({struct Cyc_List_List*_tmp1DD=_cycalloc(sizeof(*
_tmp1DD));_tmp1DD->hd=(void*)_tmp1D8;_tmp1DD->tl=({struct Cyc_List_List*_tmp1DE=
_cycalloc(sizeof(*_tmp1DE));_tmp1DE->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
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
may_constrain_evars,void*e1,void*e2){void*_tmp1E6=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp1E7;void*_tmp1E8;struct Cyc_Absyn_Tvar*_tmp1E9;void*_tmp1EA;
struct Cyc_Core_Opt*_tmp1EB;struct Cyc_Core_Opt**_tmp1EC;struct Cyc_Core_Opt*
_tmp1ED;_LL19F: if(_tmp1E6 <= (void*)3?1:*((int*)_tmp1E6)!= 20)goto _LL1A1;_tmp1E7=((
struct Cyc_Absyn_JoinEff_struct*)_tmp1E6)->f1;_LL1A0: for(0;_tmp1E7 != 0;_tmp1E7=
_tmp1E7->tl){if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp1E7->hd,
e2))return 0;}return 1;_LL1A1: if(_tmp1E6 <= (void*)3?1:*((int*)_tmp1E6)!= 19)goto
_LL1A3;_tmp1E8=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1E6)->f1;_LL1A2:
return Cyc_Tcutil_region_in_effect(0,_tmp1E8,e2)?1:(may_constrain_evars?Cyc_Tcutil_unify(
_tmp1E8,(void*)2): 0);_LL1A3: if(_tmp1E6 <= (void*)3?1:*((int*)_tmp1E6)!= 1)goto
_LL1A5;_tmp1E9=((struct Cyc_Absyn_VarType_struct*)_tmp1E6)->f1;_LL1A4: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,_tmp1E9,e2);_LL1A5: if(_tmp1E6 <= (void*)3?1:*((int*)_tmp1E6)
!= 21)goto _LL1A7;_tmp1EA=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E6)->f1;
_LL1A6: {void*_tmp1EE=Cyc_Tcutil_rgns_of(_tmp1EA);void*_tmp1EF;_LL1AC: if(_tmp1EE
<= (void*)3?1:*((int*)_tmp1EE)!= 21)goto _LL1AE;_tmp1EF=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1EE)->f1;_LL1AD: return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp1EF,
e2)?1:(may_constrain_evars?Cyc_Tcutil_unify(_tmp1EF,Cyc_Absyn_sint_typ): 0);
_LL1AE:;_LL1AF: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp1EE,e2);
_LL1AB:;}_LL1A7: if(_tmp1E6 <= (void*)3?1:*((int*)_tmp1E6)!= 0)goto _LL1A9;_tmp1EB=((
struct Cyc_Absyn_Evar_struct*)_tmp1E6)->f2;_tmp1EC=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1E6)->f2;_tmp1ED=((struct Cyc_Absyn_Evar_struct*)
_tmp1E6)->f4;_LL1A8: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp1EC=({struct Cyc_Core_Opt*
_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->v=(void*)Cyc_Absyn_empty_effect;
_tmp1F0;});return 1;_LL1A9:;_LL1AA:({struct Cyc_Std_String_pa_struct _tmp1F2;
_tmp1F2.tag=0;_tmp1F2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e1);{void*
_tmp1F1[1]={& _tmp1F2};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("subset_effect: bad effect: %s",sizeof(char),30),_tag_arr(_tmp1F1,
sizeof(void*),1));}});_LL19E:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(
0,e1,e2)?Cyc_Tcutil_subset_effect(0,e2,e1): 0)return 1;if(Cyc_Tcutil_subset_effect(
1,e1,e2)?Cyc_Tcutil_subset_effect(1,e2,e1): 0)return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple6 _tmp1F4;void*_tmp1F5;void*_tmp1F6;struct
_tuple6*_tmp1F3=(struct _tuple6*)r1->hd;_tmp1F4=*_tmp1F3;_tmp1F5=_tmp1F4.f1;
_tmp1F6=_tmp1F4.f2;{int found=_tmp1F5 == (void*)2;{struct Cyc_List_List*r2=rpo2;
for(0;r2 != 0?!found: 0;r2=r2->tl){struct _tuple6 _tmp1F8;void*_tmp1F9;void*_tmp1FA;
struct _tuple6*_tmp1F7=(struct _tuple6*)r2->hd;_tmp1F8=*_tmp1F7;_tmp1F9=_tmp1F8.f1;
_tmp1FA=_tmp1F8.f2;if(Cyc_Tcutil_unify(_tmp1F5,_tmp1F9)?Cyc_Tcutil_unify(_tmp1F6,
_tmp1FA): 0){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2)?Cyc_Tcutil_sub_rgnpo(rpo2,rpo1): 0;}struct _tuple11{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _tagged_arr)
_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)
return;{void*_tmp1FB=t1;struct Cyc_Core_Opt*_tmp1FC;struct Cyc_Core_Opt*_tmp1FD;
struct Cyc_Core_Opt**_tmp1FE;struct Cyc_Core_Opt*_tmp1FF;_LL1B1: if(_tmp1FB <= (void*)
3?1:*((int*)_tmp1FB)!= 0)goto _LL1B3;_tmp1FC=((struct Cyc_Absyn_Evar_struct*)
_tmp1FB)->f1;_tmp1FD=((struct Cyc_Absyn_Evar_struct*)_tmp1FB)->f2;_tmp1FE=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1FB)->f2;_tmp1FF=((struct Cyc_Absyn_Evar_struct*)
_tmp1FB)->f4;_LL1B2: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1FF))->v,t2);{void*_tmp200=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp200,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1FC))->v)){*_tmp1FE=({struct Cyc_Core_Opt*_tmp201=_cycalloc(sizeof(*_tmp201));
_tmp201->v=(void*)t2;_tmp201;});return;}else{{void*_tmp202=t2;struct Cyc_Core_Opt*
_tmp203;struct Cyc_Core_Opt**_tmp204;struct Cyc_Core_Opt*_tmp205;struct Cyc_Absyn_PtrInfo
_tmp206;_LL1B6: if(_tmp202 <= (void*)3?1:*((int*)_tmp202)!= 0)goto _LL1B8;_tmp203=((
struct Cyc_Absyn_Evar_struct*)_tmp202)->f2;_tmp204=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp202)->f2;_tmp205=((struct Cyc_Absyn_Evar_struct*)
_tmp202)->f4;_LL1B7: {struct Cyc_List_List*_tmp207=(struct Cyc_List_List*)_tmp1FF->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp205))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp207,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=
_tag_arr("(type variable would escape scope)",sizeof(char),35);(int)_throw((void*)
Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)_tmp1FC->v,_tmp200)){*_tmp204=({
struct Cyc_Core_Opt*_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208->v=(void*)t1;
_tmp208;});return;}Cyc_Tcutil_failure_reason=_tag_arr("(kinds are incompatible)",
sizeof(char),25);goto _LL1B5;}_LL1B8: if(_tmp202 <= (void*)3?1:*((int*)_tmp202)!= 4)
goto _LL1BA;_tmp206=((struct Cyc_Absyn_PointerType_struct*)_tmp202)->f1;if(!((void*)
_tmp1FC->v == (void*)2))goto _LL1BA;_LL1B9: {struct Cyc_Absyn_Conref*_tmp209=Cyc_Absyn_compress_conref(
_tmp206.bounds);{void*_tmp20A=(void*)_tmp209->v;_LL1BD: if((int)_tmp20A != 0)goto
_LL1BF;_LL1BE:(void*)(_tmp209->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp20B=_cycalloc(sizeof(*_tmp20B));_tmp20B[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp20C;_tmp20C.tag=0;_tmp20C.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp20E;_tmp20E.tag=0;_tmp20E.f1=Cyc_Absyn_signed_int_exp(1,0);_tmp20E;});
_tmp20D;}));_tmp20C;});_tmp20B;})));*_tmp1FE=({struct Cyc_Core_Opt*_tmp20F=
_cycalloc(sizeof(*_tmp20F));_tmp20F->v=(void*)t2;_tmp20F;});return;_LL1BF:;
_LL1C0: goto _LL1BC;_LL1BC:;}goto _LL1B5;}_LL1BA:;_LL1BB: goto _LL1B5;_LL1B5:;}Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}
_LL1B3:;_LL1B4: goto _LL1B0;_LL1B0:;}{struct _tuple6 _tmp211=({struct _tuple6 _tmp210;
_tmp210.f1=t2;_tmp210.f2=t1;_tmp210;});void*_tmp212;void*_tmp213;void*_tmp214;
void*_tmp215;struct Cyc_Absyn_Tvar*_tmp216;void*_tmp217;struct Cyc_Absyn_Tvar*
_tmp218;void*_tmp219;struct Cyc_Absyn_AggrInfo _tmp21A;void*_tmp21B;struct Cyc_List_List*
_tmp21C;void*_tmp21D;struct Cyc_Absyn_AggrInfo _tmp21E;void*_tmp21F;struct Cyc_List_List*
_tmp220;void*_tmp221;struct _tuple1*_tmp222;void*_tmp223;struct _tuple1*_tmp224;
void*_tmp225;struct Cyc_List_List*_tmp226;void*_tmp227;struct Cyc_List_List*
_tmp228;void*_tmp229;struct Cyc_Absyn_TunionInfo _tmp22A;void*_tmp22B;struct Cyc_Absyn_Tuniondecl**
_tmp22C;struct Cyc_Absyn_Tuniondecl*_tmp22D;struct Cyc_List_List*_tmp22E;void*
_tmp22F;void*_tmp230;struct Cyc_Absyn_TunionInfo _tmp231;void*_tmp232;struct Cyc_Absyn_Tuniondecl**
_tmp233;struct Cyc_Absyn_Tuniondecl*_tmp234;struct Cyc_List_List*_tmp235;void*
_tmp236;void*_tmp237;struct Cyc_Absyn_TunionFieldInfo _tmp238;void*_tmp239;struct
Cyc_Absyn_Tuniondecl*_tmp23A;struct Cyc_Absyn_Tunionfield*_tmp23B;struct Cyc_List_List*
_tmp23C;void*_tmp23D;struct Cyc_Absyn_TunionFieldInfo _tmp23E;void*_tmp23F;struct
Cyc_Absyn_Tuniondecl*_tmp240;struct Cyc_Absyn_Tunionfield*_tmp241;struct Cyc_List_List*
_tmp242;void*_tmp243;struct Cyc_Absyn_PtrInfo _tmp244;void*_tmp245;void*_tmp246;
struct Cyc_Absyn_Conref*_tmp247;struct Cyc_Absyn_Tqual _tmp248;struct Cyc_Absyn_Conref*
_tmp249;void*_tmp24A;struct Cyc_Absyn_PtrInfo _tmp24B;void*_tmp24C;void*_tmp24D;
struct Cyc_Absyn_Conref*_tmp24E;struct Cyc_Absyn_Tqual _tmp24F;struct Cyc_Absyn_Conref*
_tmp250;void*_tmp251;void*_tmp252;void*_tmp253;void*_tmp254;void*_tmp255;void*
_tmp256;void*_tmp257;void*_tmp258;void*_tmp259;int _tmp25A;void*_tmp25B;int
_tmp25C;void*_tmp25D;void*_tmp25E;void*_tmp25F;void*_tmp260;void*_tmp261;int
_tmp262;void*_tmp263;int _tmp264;void*_tmp265;void*_tmp266;void*_tmp267;void*
_tmp268;void*_tmp269;void*_tmp26A;struct Cyc_Absyn_Tqual _tmp26B;struct Cyc_Absyn_Exp*
_tmp26C;void*_tmp26D;void*_tmp26E;struct Cyc_Absyn_Tqual _tmp26F;struct Cyc_Absyn_Exp*
_tmp270;void*_tmp271;struct Cyc_Absyn_FnInfo _tmp272;struct Cyc_List_List*_tmp273;
struct Cyc_Core_Opt*_tmp274;void*_tmp275;struct Cyc_List_List*_tmp276;int _tmp277;
struct Cyc_Absyn_VarargInfo*_tmp278;struct Cyc_List_List*_tmp279;struct Cyc_List_List*
_tmp27A;void*_tmp27B;struct Cyc_Absyn_FnInfo _tmp27C;struct Cyc_List_List*_tmp27D;
struct Cyc_Core_Opt*_tmp27E;void*_tmp27F;struct Cyc_List_List*_tmp280;int _tmp281;
struct Cyc_Absyn_VarargInfo*_tmp282;struct Cyc_List_List*_tmp283;struct Cyc_List_List*
_tmp284;void*_tmp285;struct Cyc_List_List*_tmp286;void*_tmp287;struct Cyc_List_List*
_tmp288;void*_tmp289;void*_tmp28A;struct Cyc_List_List*_tmp28B;void*_tmp28C;void*
_tmp28D;struct Cyc_List_List*_tmp28E;void*_tmp28F;void*_tmp290;void*_tmp291;void*
_tmp292;void*_tmp293;void*_tmp294;void*_tmp295;void*_tmp296;void*_tmp297;void*
_tmp298;void*_tmp299;void*_tmp29A;_LL1C2: _tmp212=_tmp211.f1;if(_tmp212 <= (void*)
3?1:*((int*)_tmp212)!= 0)goto _LL1C4;_LL1C3: Cyc_Tcutil_unify_it(t2,t1);return;
_LL1C4: _tmp213=_tmp211.f1;if((int)_tmp213 != 0)goto _LL1C6;_tmp214=_tmp211.f2;if((
int)_tmp214 != 0)goto _LL1C6;_LL1C5: return;_LL1C6: _tmp215=_tmp211.f1;if(_tmp215 <= (
void*)3?1:*((int*)_tmp215)!= 1)goto _LL1C8;_tmp216=((struct Cyc_Absyn_VarType_struct*)
_tmp215)->f1;_tmp217=_tmp211.f2;if(_tmp217 <= (void*)3?1:*((int*)_tmp217)!= 1)
goto _LL1C8;_tmp218=((struct Cyc_Absyn_VarType_struct*)_tmp217)->f1;_LL1C7: {
struct _tagged_arr*_tmp29B=_tmp216->name;struct _tagged_arr*_tmp29C=_tmp218->name;
int _tmp29D=*((int*)_check_null(_tmp216->identity));int _tmp29E=*((int*)
_check_null(_tmp218->identity));void*_tmp29F=Cyc_Tcutil_tvar_kind(_tmp216);void*
_tmp2A0=Cyc_Tcutil_tvar_kind(_tmp218);if(_tmp29E == _tmp29D?Cyc_Std_zstrptrcmp(
_tmp29B,_tmp29C)== 0: 0){if(_tmp29F != _tmp2A0)({struct Cyc_Std_String_pa_struct
_tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2A0);{struct Cyc_Std_String_pa_struct _tmp2A3;_tmp2A3.tag=0;_tmp2A3.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp29F);{struct Cyc_Std_String_pa_struct
_tmp2A2;_tmp2A2.tag=0;_tmp2A2.f1=(struct _tagged_arr)*_tmp29B;{void*_tmp2A1[3]={&
_tmp2A2,& _tmp2A3,& _tmp2A4};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("same type variable %s has kinds %s and %s",sizeof(
char),42),_tag_arr(_tmp2A1,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=
_tag_arr("(variable types are not the same)",sizeof(char),34);goto _LL1C1;}_LL1C8:
_tmp219=_tmp211.f1;if(_tmp219 <= (void*)3?1:*((int*)_tmp219)!= 10)goto _LL1CA;
_tmp21A=((struct Cyc_Absyn_AggrType_struct*)_tmp219)->f1;_tmp21B=(void*)_tmp21A.aggr_info;
_tmp21C=_tmp21A.targs;_tmp21D=_tmp211.f2;if(_tmp21D <= (void*)3?1:*((int*)_tmp21D)
!= 10)goto _LL1CA;_tmp21E=((struct Cyc_Absyn_AggrType_struct*)_tmp21D)->f1;_tmp21F=(
void*)_tmp21E.aggr_info;_tmp220=_tmp21E.targs;_LL1C9: {void*_tmp2A6;struct
_tuple1*_tmp2A7;struct _tuple5 _tmp2A5=Cyc_Absyn_aggr_kinded_name(_tmp21F);_tmp2A6=
_tmp2A5.f1;_tmp2A7=_tmp2A5.f2;{void*_tmp2A9;struct _tuple1*_tmp2AA;struct _tuple5
_tmp2A8=Cyc_Absyn_aggr_kinded_name(_tmp21B);_tmp2A9=_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;
if(_tmp2A6 != _tmp2A9){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1C1;}if(Cyc_Absyn_qvar_cmp(_tmp2A7,_tmp2AA)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different type name)",sizeof(char),22);goto _LL1C1;}Cyc_Tcutil_unify_list(
_tmp220,_tmp21C);return;}}_LL1CA: _tmp221=_tmp211.f1;if(_tmp221 <= (void*)3?1:*((
int*)_tmp221)!= 12)goto _LL1CC;_tmp222=((struct Cyc_Absyn_EnumType_struct*)_tmp221)->f1;
_tmp223=_tmp211.f2;if(_tmp223 <= (void*)3?1:*((int*)_tmp223)!= 12)goto _LL1CC;
_tmp224=((struct Cyc_Absyn_EnumType_struct*)_tmp223)->f1;_LL1CB: if(Cyc_Absyn_qvar_cmp(
_tmp222,_tmp224)== 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different enum types)",
sizeof(char),23);goto _LL1C1;_LL1CC: _tmp225=_tmp211.f1;if(_tmp225 <= (void*)3?1:*((
int*)_tmp225)!= 13)goto _LL1CE;_tmp226=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp225)->f1;_tmp227=_tmp211.f2;if(_tmp227 <= (void*)3?1:*((int*)_tmp227)!= 13)
goto _LL1CE;_tmp228=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp227)->f1;_LL1CD: {
int bad=0;for(0;_tmp226 != 0?_tmp228 != 0: 0;(_tmp226=_tmp226->tl,_tmp228=_tmp228->tl)){
struct Cyc_Absyn_Enumfield*_tmp2AB=(struct Cyc_Absyn_Enumfield*)_tmp226->hd;struct
Cyc_Absyn_Enumfield*_tmp2AC=(struct Cyc_Absyn_Enumfield*)_tmp228->hd;if(Cyc_Absyn_qvar_cmp(
_tmp2AB->name,_tmp2AC->name)!= 0){Cyc_Tcutil_failure_reason=_tag_arr("(different names for enum fields)",
sizeof(char),34);bad=1;break;}if(_tmp2AB->tag == _tmp2AC->tag)continue;if(_tmp2AB->tag
== 0?1: _tmp2AC->tag == 0){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(char),39);bad=1;break;}if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp2AB->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp2AC->tag))){Cyc_Tcutil_failure_reason=
_tag_arr("(different tag values for enum fields)",sizeof(char),39);bad=1;break;}}
if(bad)goto _LL1C1;if(_tmp226 == 0?_tmp228 == 0: 0)return;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of fields for enums)",sizeof(char),39);goto _LL1C1;}
_LL1CE: _tmp229=_tmp211.f1;if(_tmp229 <= (void*)3?1:*((int*)_tmp229)!= 2)goto
_LL1D0;_tmp22A=((struct Cyc_Absyn_TunionType_struct*)_tmp229)->f1;_tmp22B=(void*)
_tmp22A.tunion_info;if(*((int*)_tmp22B)!= 1)goto _LL1D0;_tmp22C=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp22B)->f1;_tmp22D=*_tmp22C;_tmp22E=_tmp22A.targs;_tmp22F=(void*)_tmp22A.rgn;
_tmp230=_tmp211.f2;if(_tmp230 <= (void*)3?1:*((int*)_tmp230)!= 2)goto _LL1D0;
_tmp231=((struct Cyc_Absyn_TunionType_struct*)_tmp230)->f1;_tmp232=(void*)_tmp231.tunion_info;
if(*((int*)_tmp232)!= 1)goto _LL1D0;_tmp233=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp232)->f1;_tmp234=*_tmp233;_tmp235=_tmp231.targs;_tmp236=(void*)_tmp231.rgn;
_LL1CF: if(_tmp22D == _tmp234?1: Cyc_Absyn_qvar_cmp(_tmp22D->name,_tmp234->name)== 
0){Cyc_Tcutil_unify_it(_tmp236,_tmp22F);Cyc_Tcutil_unify_list(_tmp235,_tmp22E);
return;}Cyc_Tcutil_failure_reason=_tag_arr("(different tunion types)",sizeof(
char),25);goto _LL1C1;_LL1D0: _tmp237=_tmp211.f1;if(_tmp237 <= (void*)3?1:*((int*)
_tmp237)!= 3)goto _LL1D2;_tmp238=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp237)->f1;_tmp239=(void*)_tmp238.field_info;if(*((int*)_tmp239)!= 1)goto
_LL1D2;_tmp23A=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp239)->f1;_tmp23B=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp239)->f2;_tmp23C=_tmp238.targs;
_tmp23D=_tmp211.f2;if(_tmp23D <= (void*)3?1:*((int*)_tmp23D)!= 3)goto _LL1D2;
_tmp23E=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp23D)->f1;_tmp23F=(void*)
_tmp23E.field_info;if(*((int*)_tmp23F)!= 1)goto _LL1D2;_tmp240=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp23F)->f1;_tmp241=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp23F)->f2;
_tmp242=_tmp23E.targs;_LL1D1: if((_tmp23A == _tmp240?1: Cyc_Absyn_qvar_cmp(_tmp23A->name,
_tmp240->name)== 0)?_tmp23B == _tmp241?1: Cyc_Absyn_qvar_cmp(_tmp23B->name,_tmp241->name)
== 0: 0){Cyc_Tcutil_unify_list(_tmp242,_tmp23C);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)",sizeof(char),31);goto _LL1C1;_LL1D2:
_tmp243=_tmp211.f1;if(_tmp243 <= (void*)3?1:*((int*)_tmp243)!= 4)goto _LL1D4;
_tmp244=((struct Cyc_Absyn_PointerType_struct*)_tmp243)->f1;_tmp245=(void*)
_tmp244.elt_typ;_tmp246=(void*)_tmp244.rgn_typ;_tmp247=_tmp244.nullable;_tmp248=
_tmp244.tq;_tmp249=_tmp244.bounds;_tmp24A=_tmp211.f2;if(_tmp24A <= (void*)3?1:*((
int*)_tmp24A)!= 4)goto _LL1D4;_tmp24B=((struct Cyc_Absyn_PointerType_struct*)
_tmp24A)->f1;_tmp24C=(void*)_tmp24B.elt_typ;_tmp24D=(void*)_tmp24B.rgn_typ;
_tmp24E=_tmp24B.nullable;_tmp24F=_tmp24B.tq;_tmp250=_tmp24B.bounds;_LL1D3: Cyc_Tcutil_unify_it(
_tmp24C,_tmp245);Cyc_Tcutil_unify_it(_tmp246,_tmp24D);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_unify_tqual(_tmp24F,_tmp248);Cyc_Tcutil_unify_it_conrefs(
Cyc_Tcutil_boundscmp,_tmp250,_tmp249,_tag_arr("(different pointer bounds)",
sizeof(char),27));{void*_tmp2AD=(void*)(Cyc_Absyn_compress_conref(_tmp250))->v;
void*_tmp2AE;_LL1FB: if(_tmp2AD <= (void*)1?1:*((int*)_tmp2AD)!= 0)goto _LL1FD;
_tmp2AE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2AD)->f1;if((int)_tmp2AE
!= 0)goto _LL1FD;_LL1FC: return;_LL1FD:;_LL1FE: goto _LL1FA;_LL1FA:;}((void(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp24E,_tmp247,
_tag_arr("(different pointer types)",sizeof(char),26));return;_LL1D4: _tmp251=
_tmp211.f1;if(_tmp251 <= (void*)3?1:*((int*)_tmp251)!= 5)goto _LL1D6;_tmp252=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp251)->f1;_tmp253=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp251)->f2;_tmp254=_tmp211.f2;if(_tmp254 <= (void*)3?1:*((int*)_tmp254)!= 5)
goto _LL1D6;_tmp255=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp254)->f1;_tmp256=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp254)->f2;_LL1D5: if(_tmp255 == _tmp252?
_tmp256 == _tmp253: 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different integral types)",
sizeof(char),27);goto _LL1C1;_LL1D6: _tmp257=_tmp211.f1;if((int)_tmp257 != 1)goto
_LL1D8;_tmp258=_tmp211.f2;if((int)_tmp258 != 1)goto _LL1D8;_LL1D7: return;_LL1D8:
_tmp259=_tmp211.f1;if(_tmp259 <= (void*)3?1:*((int*)_tmp259)!= 6)goto _LL1DA;
_tmp25A=((struct Cyc_Absyn_DoubleType_struct*)_tmp259)->f1;_tmp25B=_tmp211.f2;if(
_tmp25B <= (void*)3?1:*((int*)_tmp25B)!= 6)goto _LL1DA;_tmp25C=((struct Cyc_Absyn_DoubleType_struct*)
_tmp25B)->f1;_LL1D9: if(_tmp25A == _tmp25C)return;goto _LL1C1;_LL1DA: _tmp25D=
_tmp211.f1;if(_tmp25D <= (void*)3?1:*((int*)_tmp25D)!= 14)goto _LL1DC;_tmp25E=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp25D)->f1;_tmp25F=_tmp211.f2;if(
_tmp25F <= (void*)3?1:*((int*)_tmp25F)!= 14)goto _LL1DC;_tmp260=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp25F)->f1;_LL1DB: Cyc_Tcutil_unify_it(_tmp25E,_tmp260);return;_LL1DC: _tmp261=
_tmp211.f1;if(_tmp261 <= (void*)3?1:*((int*)_tmp261)!= 18)goto _LL1DE;_tmp262=((
struct Cyc_Absyn_TypeInt_struct*)_tmp261)->f1;_tmp263=_tmp211.f2;if(_tmp263 <= (
void*)3?1:*((int*)_tmp263)!= 18)goto _LL1DE;_tmp264=((struct Cyc_Absyn_TypeInt_struct*)
_tmp263)->f1;_LL1DD: if(_tmp262 == _tmp264)return;Cyc_Tcutil_failure_reason=
_tag_arr("(different type integers)",sizeof(char),26);goto _LL1C1;_LL1DE: _tmp265=
_tmp211.f1;if(_tmp265 <= (void*)3?1:*((int*)_tmp265)!= 17)goto _LL1E0;_tmp266=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp265)->f1;_tmp267=_tmp211.f2;if(
_tmp267 <= (void*)3?1:*((int*)_tmp267)!= 17)goto _LL1E0;_tmp268=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp267)->f1;_LL1DF: Cyc_Tcutil_unify_it(_tmp266,_tmp268);return;_LL1E0: _tmp269=
_tmp211.f1;if(_tmp269 <= (void*)3?1:*((int*)_tmp269)!= 7)goto _LL1E2;_tmp26A=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp269)->f1;_tmp26B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp269)->f2;_tmp26C=((struct Cyc_Absyn_ArrayType_struct*)_tmp269)->f3;_tmp26D=
_tmp211.f2;if(_tmp26D <= (void*)3?1:*((int*)_tmp26D)!= 7)goto _LL1E2;_tmp26E=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp26D)->f1;_tmp26F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp26D)->f2;_tmp270=((struct Cyc_Absyn_ArrayType_struct*)_tmp26D)->f3;_LL1E1: Cyc_Tcutil_unify_tqual(
_tmp26F,_tmp26B);Cyc_Tcutil_unify_it(_tmp26E,_tmp26A);if(_tmp26C == _tmp270)
return;if(_tmp26C == 0?1: _tmp270 == 0)goto _LL1C1;if(Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp26C),(struct Cyc_Absyn_Exp*)_check_null(
_tmp270)))return;Cyc_Tcutil_failure_reason=_tag_arr("(different array sizes)",
sizeof(char),24);goto _LL1C1;_LL1E2: _tmp271=_tmp211.f1;if(_tmp271 <= (void*)3?1:*((
int*)_tmp271)!= 8)goto _LL1E4;_tmp272=((struct Cyc_Absyn_FnType_struct*)_tmp271)->f1;
_tmp273=_tmp272.tvars;_tmp274=_tmp272.effect;_tmp275=(void*)_tmp272.ret_typ;
_tmp276=_tmp272.args;_tmp277=_tmp272.c_varargs;_tmp278=_tmp272.cyc_varargs;
_tmp279=_tmp272.rgn_po;_tmp27A=_tmp272.attributes;_tmp27B=_tmp211.f2;if(_tmp27B
<= (void*)3?1:*((int*)_tmp27B)!= 8)goto _LL1E4;_tmp27C=((struct Cyc_Absyn_FnType_struct*)
_tmp27B)->f1;_tmp27D=_tmp27C.tvars;_tmp27E=_tmp27C.effect;_tmp27F=(void*)_tmp27C.ret_typ;
_tmp280=_tmp27C.args;_tmp281=_tmp27C.c_varargs;_tmp282=_tmp27C.cyc_varargs;
_tmp283=_tmp27C.rgn_po;_tmp284=_tmp27C.attributes;_LL1E3: {int done=0;{struct
_RegionHandle _tmp2AF=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2AF;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp27D != 0){if(_tmp273 == 0){
Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too few type variables)",
sizeof(char),50);(int)_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp2B0=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp27D->hd);void*_tmp2B1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp273->hd);if(_tmp2B0 != _tmp2B1){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp2B5;_tmp2B5.tag=0;_tmp2B5.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2B1);{struct Cyc_Std_String_pa_struct
_tmp2B4;_tmp2B4.tag=0;_tmp2B4.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2B0);{struct Cyc_Std_String_pa_struct _tmp2B3;_tmp2B3.tag=0;_tmp2B3.f1=(struct
_tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp27D->hd);{void*
_tmp2B2[3]={& _tmp2B3,& _tmp2B4,& _tmp2B5};Cyc_Std_aprintf(_tag_arr("(type var %s has different kinds %s and %s)",
sizeof(char),44),_tag_arr(_tmp2B2,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp2B6=_region_malloc(rgn,sizeof(*_tmp2B6));_tmp2B6->hd=({
struct _tuple8*_tmp2B7=_region_malloc(rgn,sizeof(*_tmp2B7));_tmp2B7->f1=(struct
Cyc_Absyn_Tvar*)_tmp273->hd;_tmp2B7->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8[0]=({struct Cyc_Absyn_VarType_struct
_tmp2B9;_tmp2B9.tag=1;_tmp2B9.f1=(struct Cyc_Absyn_Tvar*)_tmp27D->hd;_tmp2B9;});
_tmp2B8;});_tmp2B7;});_tmp2B6->tl=inst;_tmp2B6;});_tmp27D=_tmp27D->tl;_tmp273=
_tmp273->tl;}}if(_tmp273 != 0){Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too many type variables)",
sizeof(char),51);_npop_handler(0);goto _LL1C1;}if(inst != 0){Cyc_Tcutil_unify_it((
void*)({struct Cyc_Absyn_FnType_struct*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));
_tmp2BA[0]=({struct Cyc_Absyn_FnType_struct _tmp2BB;_tmp2BB.tag=8;_tmp2BB.f1=({
struct Cyc_Absyn_FnInfo _tmp2BC;_tmp2BC.tvars=0;_tmp2BC.effect=_tmp27E;_tmp2BC.ret_typ=(
void*)_tmp27F;_tmp2BC.args=_tmp280;_tmp2BC.c_varargs=_tmp281;_tmp2BC.cyc_varargs=
_tmp282;_tmp2BC.rgn_po=_tmp283;_tmp2BC.attributes=_tmp284;_tmp2BC;});_tmp2BB;});
_tmp2BA;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD[0]=({struct Cyc_Absyn_FnType_struct
_tmp2BE;_tmp2BE.tag=8;_tmp2BE.f1=({struct Cyc_Absyn_FnInfo _tmp2BF;_tmp2BF.tvars=0;
_tmp2BF.effect=_tmp274;_tmp2BF.ret_typ=(void*)_tmp275;_tmp2BF.args=_tmp276;
_tmp2BF.c_varargs=_tmp277;_tmp2BF.cyc_varargs=_tmp278;_tmp2BF.rgn_po=_tmp279;
_tmp2BF.attributes=_tmp27A;_tmp2BF;});_tmp2BE;});_tmp2BD;})));done=1;}};
_pop_region(rgn);}if(done)return;Cyc_Tcutil_unify_it(_tmp27F,_tmp275);for(0;
_tmp280 != 0?_tmp276 != 0: 0;(_tmp280=_tmp280->tl,_tmp276=_tmp276->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)_tmp280->hd)).f2,(*((struct _tuple2*)_tmp276->hd)).f2);Cyc_Tcutil_unify_it((*((
struct _tuple2*)_tmp280->hd)).f3,(*((struct _tuple2*)_tmp276->hd)).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp280 != 0?1: _tmp276 != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)",sizeof(char),52);
goto _LL1C1;}if(_tmp281 != _tmp277){Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes C varargs)",
sizeof(char),41);goto _LL1C1;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp2C1=({
struct _tuple11 _tmp2C0;_tmp2C0.f1=_tmp282;_tmp2C0.f2=_tmp278;_tmp2C0;});struct Cyc_Absyn_VarargInfo*
_tmp2C2;struct Cyc_Absyn_VarargInfo*_tmp2C3;struct Cyc_Absyn_VarargInfo*_tmp2C4;
struct Cyc_Absyn_VarargInfo*_tmp2C5;struct Cyc_Absyn_VarargInfo*_tmp2C6;struct Cyc_Absyn_VarargInfo
_tmp2C7;struct Cyc_Core_Opt*_tmp2C8;struct Cyc_Absyn_Tqual _tmp2C9;void*_tmp2CA;int
_tmp2CB;struct Cyc_Absyn_VarargInfo*_tmp2CC;struct Cyc_Absyn_VarargInfo _tmp2CD;
struct Cyc_Core_Opt*_tmp2CE;struct Cyc_Absyn_Tqual _tmp2CF;void*_tmp2D0;int _tmp2D1;
_LL200: _tmp2C2=_tmp2C1.f1;if(_tmp2C2 != 0)goto _LL202;_tmp2C3=_tmp2C1.f2;if(
_tmp2C3 != 0)goto _LL202;_LL201: goto _LL1FF;_LL202: _tmp2C4=_tmp2C1.f1;if(_tmp2C4 != 
0)goto _LL204;_LL203: goto _LL205;_LL204: _tmp2C5=_tmp2C1.f2;if(_tmp2C5 != 0)goto
_LL206;_LL205: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes varargs)",
sizeof(char),39);goto _LL1FF;_LL206: _tmp2C6=_tmp2C1.f1;if(_tmp2C6 == 0)goto _LL1FF;
_tmp2C7=*_tmp2C6;_tmp2C8=_tmp2C7.name;_tmp2C9=_tmp2C7.tq;_tmp2CA=(void*)_tmp2C7.type;
_tmp2CB=_tmp2C7.inject;_tmp2CC=_tmp2C1.f2;if(_tmp2CC == 0)goto _LL1FF;_tmp2CD=*
_tmp2CC;_tmp2CE=_tmp2CD.name;_tmp2CF=_tmp2CD.tq;_tmp2D0=(void*)_tmp2CD.type;
_tmp2D1=_tmp2CD.inject;_LL207: Cyc_Tcutil_unify_tqual(_tmp2C9,_tmp2CF);Cyc_Tcutil_unify_it(
_tmp2CA,_tmp2D0);if(_tmp2CB != _tmp2D1){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type injects varargs)",sizeof(char),41);}goto _LL1FF;
_LL1FF:;}if(bad_cyc_vararg)goto _LL1C1;{int bad_effect=0;{struct _tuple12 _tmp2D3=({
struct _tuple12 _tmp2D2;_tmp2D2.f1=_tmp27E;_tmp2D2.f2=_tmp274;_tmp2D2;});struct Cyc_Core_Opt*
_tmp2D4;struct Cyc_Core_Opt*_tmp2D5;struct Cyc_Core_Opt*_tmp2D6;struct Cyc_Core_Opt*
_tmp2D7;_LL209: _tmp2D4=_tmp2D3.f1;if(_tmp2D4 != 0)goto _LL20B;_tmp2D5=_tmp2D3.f2;
if(_tmp2D5 != 0)goto _LL20B;_LL20A: goto _LL208;_LL20B: _tmp2D6=_tmp2D3.f1;if(_tmp2D6
!= 0)goto _LL20D;_LL20C: goto _LL20E;_LL20D: _tmp2D7=_tmp2D3.f2;if(_tmp2D7 != 0)goto
_LL20F;_LL20E: bad_effect=1;goto _LL208;_LL20F:;_LL210: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp27E))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp274))->v);goto _LL208;_LL208:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=_tag_arr("(function type effects do not unify)",
sizeof(char),37);goto _LL1C1;}if(!Cyc_Tcutil_same_atts(_tmp27A,_tmp284)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different attributes)",sizeof(char),43);goto
_LL1C1;}if(!Cyc_Tcutil_same_rgn_po(_tmp279,_tmp283)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different region lifetime orderings)",sizeof(char),
58);goto _LL1C1;}return;}}}_LL1E4: _tmp285=_tmp211.f1;if(_tmp285 <= (void*)3?1:*((
int*)_tmp285)!= 9)goto _LL1E6;_tmp286=((struct Cyc_Absyn_TupleType_struct*)_tmp285)->f1;
_tmp287=_tmp211.f2;if(_tmp287 <= (void*)3?1:*((int*)_tmp287)!= 9)goto _LL1E6;
_tmp288=((struct Cyc_Absyn_TupleType_struct*)_tmp287)->f1;_LL1E5: for(0;_tmp288 != 
0?_tmp286 != 0: 0;(_tmp288=_tmp288->tl,_tmp286=_tmp286->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)_tmp288->hd)).f1,(*((struct _tuple4*)_tmp286->hd)).f1);Cyc_Tcutil_unify_it((*((
struct _tuple4*)_tmp288->hd)).f2,(*((struct _tuple4*)_tmp286->hd)).f2);}if(_tmp288
== 0?_tmp286 == 0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(tuple types have different numbers of components)",sizeof(char),51);
goto _LL1C1;_LL1E6: _tmp289=_tmp211.f1;if(_tmp289 <= (void*)3?1:*((int*)_tmp289)!= 
11)goto _LL1E8;_tmp28A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp289)->f1;
_tmp28B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp289)->f2;_tmp28C=_tmp211.f2;
if(_tmp28C <= (void*)3?1:*((int*)_tmp28C)!= 11)goto _LL1E8;_tmp28D=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp28C)->f1;_tmp28E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp28C)->f2;_LL1E7: if(_tmp28D != _tmp28A){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1C1;}for(0;_tmp28E != 0?_tmp28B != 0: 0;(_tmp28E=_tmp28E->tl,
_tmp28B=_tmp28B->tl)){struct Cyc_Absyn_Aggrfield*_tmp2D8=(struct Cyc_Absyn_Aggrfield*)
_tmp28E->hd;struct Cyc_Absyn_Aggrfield*_tmp2D9=(struct Cyc_Absyn_Aggrfield*)
_tmp28B->hd;if(Cyc_Std_zstrptrcmp(_tmp2D8->name,_tmp2D9->name)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different member names)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual(_tmp2D8->tq,_tmp2D9->tq);Cyc_Tcutil_unify_it((void*)
_tmp2D8->type,(void*)_tmp2D9->type);if(!Cyc_Tcutil_same_atts(_tmp2D8->attributes,
_tmp2D9->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different attributes on member)",sizeof(char),33);(int)_throw((void*)
Cyc_Tcutil_Unify);}if(((_tmp2D8->width != 0?_tmp2D9->width == 0: 0)?1:(_tmp2D9->width
!= 0?_tmp2D8->width == 0: 0))?1:((_tmp2D8->width != 0?_tmp2D9->width != 0: 0)?!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2D8->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp2D9->width)): 0)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=_tag_arr("(different bitfield widths on member)",
sizeof(char),38);(int)_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp28E == 0?_tmp28B == 
0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of members)",sizeof(char),30);goto _LL1C1;_LL1E8:
_tmp28F=_tmp211.f1;if((int)_tmp28F != 2)goto _LL1EA;_tmp290=_tmp211.f2;if((int)
_tmp290 != 2)goto _LL1EA;_LL1E9: return;_LL1EA: _tmp291=_tmp211.f1;if(_tmp291 <= (
void*)3?1:*((int*)_tmp291)!= 15)goto _LL1EC;_tmp292=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp291)->f1;_tmp293=_tmp211.f2;if(_tmp293 <= (void*)3?1:*((int*)_tmp293)!= 15)
goto _LL1EC;_tmp294=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp293)->f1;
_LL1EB: Cyc_Tcutil_unify_it(_tmp292,_tmp294);return;_LL1EC: _tmp295=_tmp211.f1;if(
_tmp295 <= (void*)3?1:*((int*)_tmp295)!= 20)goto _LL1EE;_LL1ED: goto _LL1EF;_LL1EE:
_tmp296=_tmp211.f2;if(_tmp296 <= (void*)3?1:*((int*)_tmp296)!= 20)goto _LL1F0;
_LL1EF: goto _LL1F1;_LL1F0: _tmp297=_tmp211.f1;if(_tmp297 <= (void*)3?1:*((int*)
_tmp297)!= 19)goto _LL1F2;_LL1F1: goto _LL1F3;_LL1F2: _tmp298=_tmp211.f1;if(_tmp298
<= (void*)3?1:*((int*)_tmp298)!= 21)goto _LL1F4;_LL1F3: goto _LL1F5;_LL1F4: _tmp299=
_tmp211.f2;if(_tmp299 <= (void*)3?1:*((int*)_tmp299)!= 21)goto _LL1F6;_LL1F5: goto
_LL1F7;_LL1F6: _tmp29A=_tmp211.f2;if(_tmp29A <= (void*)3?1:*((int*)_tmp29A)!= 19)
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
for(0;l1 != 0?l2 != 0: 0;(l1=l1->tl,l2=l2->tl)){int _tmp2DA=cmp((void*)l1->hd,(void*)
l2->hd);if(_tmp2DA != 0)return _tmp2DA;}if(l2 != 0)return - 1;if(l1 != 0)return 1;
return 0;}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){int _tmp2DB=(tq1.q_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int
_tmp2DC=(tq2.q_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp2DB,_tmp2DC);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp2DD=(void*)x->v;void*
_tmp2DE;_LL212: if((int)_tmp2DD != 0)goto _LL214;_LL213: return - 1;_LL214: if(_tmp2DD
<= (void*)1?1:*((int*)_tmp2DD)!= 0)goto _LL216;_tmp2DE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2DD)->f1;_LL215: {void*_tmp2DF=(void*)y->v;void*_tmp2E0;_LL219: if((int)
_tmp2DF != 0)goto _LL21B;_LL21A: return 1;_LL21B: if(_tmp2DF <= (void*)1?1:*((int*)
_tmp2DF)!= 0)goto _LL21D;_tmp2E0=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2DF)->f1;_LL21C: return cmp(_tmp2DE,_tmp2E0);_LL21D: if(_tmp2DF <= (void*)1?1:*((
int*)_tmp2DF)!= 1)goto _LL218;_LL21E:({void*_tmp2E1[0]={};Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)",sizeof(char),40),_tag_arr(
_tmp2E1,sizeof(void*),0));});_LL218:;}_LL216: if(_tmp2DD <= (void*)1?1:*((int*)
_tmp2DD)!= 1)goto _LL211;_LL217:({void*_tmp2E2[0]={};Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress",
sizeof(char),37),_tag_arr(_tmp2E2,sizeof(void*),0));});_LL211:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp2E4;struct Cyc_Absyn_Tqual
_tmp2E5;void*_tmp2E6;struct _tuple4*_tmp2E3=tqt1;_tmp2E4=*_tmp2E3;_tmp2E5=_tmp2E4.f1;
_tmp2E6=_tmp2E4.f2;{struct _tuple4 _tmp2E8;struct Cyc_Absyn_Tqual _tmp2E9;void*
_tmp2EA;struct _tuple4*_tmp2E7=tqt2;_tmp2E8=*_tmp2E7;_tmp2E9=_tmp2E8.f1;_tmp2EA=
_tmp2E8.f2;{int _tmp2EB=Cyc_Tcutil_tqual_cmp(_tmp2E5,_tmp2E9);if(_tmp2EB != 0)
return _tmp2EB;return Cyc_Tcutil_typecmp(_tmp2E6,_tmp2EA);}}}static int Cyc_Tcutil_attribute_case_number(
void*att){void*_tmp2EC=att;_LL220: if(_tmp2EC <= (void*)16?1:*((int*)_tmp2EC)!= 0)
goto _LL222;_LL221: return 0;_LL222: if((int)_tmp2EC != 0)goto _LL224;_LL223: return 1;
_LL224: if((int)_tmp2EC != 1)goto _LL226;_LL225: return 2;_LL226: if((int)_tmp2EC != 2)
goto _LL228;_LL227: return 3;_LL228: if((int)_tmp2EC != 3)goto _LL22A;_LL229: return 4;
_LL22A: if((int)_tmp2EC != 4)goto _LL22C;_LL22B: return 5;_LL22C: if(_tmp2EC <= (void*)
16?1:*((int*)_tmp2EC)!= 1)goto _LL22E;_LL22D: return 6;_LL22E: if((int)_tmp2EC != 5)
goto _LL230;_LL22F: return 7;_LL230: if(_tmp2EC <= (void*)16?1:*((int*)_tmp2EC)!= 2)
goto _LL232;_LL231: return 8;_LL232: if((int)_tmp2EC != 6)goto _LL234;_LL233: return 9;
_LL234: if((int)_tmp2EC != 7)goto _LL236;_LL235: return 10;_LL236: if((int)_tmp2EC != 8)
goto _LL238;_LL237: return 11;_LL238: if((int)_tmp2EC != 9)goto _LL23A;_LL239: return 12;
_LL23A: if((int)_tmp2EC != 10)goto _LL23C;_LL23B: return 13;_LL23C: if((int)_tmp2EC != 
11)goto _LL23E;_LL23D: return 14;_LL23E: if((int)_tmp2EC != 12)goto _LL240;_LL23F:
return 15;_LL240: if((int)_tmp2EC != 13)goto _LL242;_LL241: return 16;_LL242: if((int)
_tmp2EC != 14)goto _LL244;_LL243: return 17;_LL244: if((int)_tmp2EC != 15)goto _LL246;
_LL245: return 18;_LL246: if(_tmp2EC <= (void*)16?1:*((int*)_tmp2EC)!= 3)goto _LL21F;
_LL247: return 19;_LL21F:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple6 _tmp2EE=({struct _tuple6 _tmp2ED;_tmp2ED.f1=att1;_tmp2ED.f2=att2;
_tmp2ED;});void*_tmp2EF;int _tmp2F0;void*_tmp2F1;int _tmp2F2;void*_tmp2F3;int
_tmp2F4;void*_tmp2F5;int _tmp2F6;void*_tmp2F7;struct _tagged_arr _tmp2F8;void*
_tmp2F9;struct _tagged_arr _tmp2FA;void*_tmp2FB;void*_tmp2FC;int _tmp2FD;int _tmp2FE;
void*_tmp2FF;void*_tmp300;int _tmp301;int _tmp302;_LL249: _tmp2EF=_tmp2EE.f1;if(
_tmp2EF <= (void*)16?1:*((int*)_tmp2EF)!= 0)goto _LL24B;_tmp2F0=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp2EF)->f1;_tmp2F1=_tmp2EE.f2;if(_tmp2F1 <= (void*)16?1:*((int*)_tmp2F1)!= 0)
goto _LL24B;_tmp2F2=((struct Cyc_Absyn_Regparm_att_struct*)_tmp2F1)->f1;_LL24A:
_tmp2F4=_tmp2F0;_tmp2F6=_tmp2F2;goto _LL24C;_LL24B: _tmp2F3=_tmp2EE.f1;if(_tmp2F3
<= (void*)16?1:*((int*)_tmp2F3)!= 1)goto _LL24D;_tmp2F4=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp2F3)->f1;_tmp2F5=_tmp2EE.f2;if(_tmp2F5 <= (void*)16?1:*((int*)_tmp2F5)!= 1)
goto _LL24D;_tmp2F6=((struct Cyc_Absyn_Aligned_att_struct*)_tmp2F5)->f1;_LL24C:
return Cyc_Core_intcmp(_tmp2F4,_tmp2F6);_LL24D: _tmp2F7=_tmp2EE.f1;if(_tmp2F7 <= (
void*)16?1:*((int*)_tmp2F7)!= 2)goto _LL24F;_tmp2F8=((struct Cyc_Absyn_Section_att_struct*)
_tmp2F7)->f1;_tmp2F9=_tmp2EE.f2;if(_tmp2F9 <= (void*)16?1:*((int*)_tmp2F9)!= 2)
goto _LL24F;_tmp2FA=((struct Cyc_Absyn_Section_att_struct*)_tmp2F9)->f1;_LL24E:
return Cyc_Std_strcmp(_tmp2F8,_tmp2FA);_LL24F: _tmp2FB=_tmp2EE.f1;if(_tmp2FB <= (
void*)16?1:*((int*)_tmp2FB)!= 3)goto _LL251;_tmp2FC=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp2FB)->f1;_tmp2FD=((struct Cyc_Absyn_Format_att_struct*)_tmp2FB)->f2;_tmp2FE=((
struct Cyc_Absyn_Format_att_struct*)_tmp2FB)->f3;_tmp2FF=_tmp2EE.f2;if(_tmp2FF <= (
void*)16?1:*((int*)_tmp2FF)!= 3)goto _LL251;_tmp300=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp2FF)->f1;_tmp301=((struct Cyc_Absyn_Format_att_struct*)_tmp2FF)->f2;_tmp302=((
struct Cyc_Absyn_Format_att_struct*)_tmp2FF)->f3;_LL250: {int _tmp303=Cyc_Core_intcmp((
int)((unsigned int)_tmp2FC),(int)((unsigned int)_tmp300));if(_tmp303 != 0)return
_tmp303;{int _tmp304=Cyc_Core_intcmp(_tmp2FD,_tmp301);if(_tmp304 != 0)return
_tmp304;return Cyc_Core_intcmp(_tmp2FE,_tmp302);}}_LL251:;_LL252: return Cyc_Core_intcmp(
Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));
_LL248:;}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct
Cyc_Absyn_Aggrfield*f2){int _tmp305=Cyc_Std_zstrptrcmp(f1->name,f2->name);if(
_tmp305 != 0)return _tmp305;{int _tmp306=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp306 != 0)return _tmp306;{int _tmp307=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp307 != 0)return _tmp307;{int _tmp308=Cyc_Tcutil_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp308 != 0)return _tmp308;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);}}}}static
int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp309=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp309 != 0)return
_tmp309;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t){void*_tmp30A=t;
_LL254: if((int)_tmp30A != 0)goto _LL256;_LL255: return 0;_LL256: if(_tmp30A <= (void*)
3?1:*((int*)_tmp30A)!= 0)goto _LL258;_LL257: return 1;_LL258: if(_tmp30A <= (void*)3?
1:*((int*)_tmp30A)!= 1)goto _LL25A;_LL259: return 2;_LL25A: if(_tmp30A <= (void*)3?1:*((
int*)_tmp30A)!= 2)goto _LL25C;_LL25B: return 3;_LL25C: if(_tmp30A <= (void*)3?1:*((
int*)_tmp30A)!= 3)goto _LL25E;_LL25D: return 4;_LL25E: if(_tmp30A <= (void*)3?1:*((
int*)_tmp30A)!= 4)goto _LL260;_LL25F: return 5;_LL260: if(_tmp30A <= (void*)3?1:*((
int*)_tmp30A)!= 5)goto _LL262;_LL261: return 6;_LL262: if((int)_tmp30A != 1)goto
_LL264;_LL263: return 7;_LL264: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 6)goto
_LL266;_LL265: return 8;_LL266: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 7)goto
_LL268;_LL267: return 9;_LL268: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 8)goto
_LL26A;_LL269: return 10;_LL26A: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 9)goto
_LL26C;_LL26B: return 11;_LL26C: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 10)goto
_LL26E;_LL26D: return 12;_LL26E: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 11)goto
_LL270;_LL26F: return 14;_LL270: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 12)goto
_LL272;_LL271: return 16;_LL272: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 13)goto
_LL274;_LL273: return 17;_LL274: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 15)goto
_LL276;_LL275: return 18;_LL276: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 16)goto
_LL278;_LL277: return 19;_LL278: if((int)_tmp30A != 2)goto _LL27A;_LL279: return 20;
_LL27A: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 19)goto _LL27C;_LL27B: return 21;
_LL27C: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 20)goto _LL27E;_LL27D: return 22;
_LL27E: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 21)goto _LL280;_LL27F: return 23;
_LL280: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 14)goto _LL282;_LL281: return 24;
_LL282: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 18)goto _LL284;_LL283: return 25;
_LL284: if(_tmp30A <= (void*)3?1:*((int*)_tmp30A)!= 17)goto _LL253;_LL285: return 26;
_LL253:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp30B=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(
t1),Cyc_Tcutil_type_case_number(t2));if(_tmp30B != 0)return _tmp30B;{struct _tuple6
_tmp30D=({struct _tuple6 _tmp30C;_tmp30C.f1=t2;_tmp30C.f2=t1;_tmp30C;});void*
_tmp30E;void*_tmp30F;void*_tmp310;void*_tmp311;void*_tmp312;struct Cyc_Absyn_Tvar*
_tmp313;void*_tmp314;struct Cyc_Absyn_Tvar*_tmp315;void*_tmp316;struct Cyc_Absyn_AggrInfo
_tmp317;void*_tmp318;struct Cyc_List_List*_tmp319;void*_tmp31A;struct Cyc_Absyn_AggrInfo
_tmp31B;void*_tmp31C;struct Cyc_List_List*_tmp31D;void*_tmp31E;struct _tuple1*
_tmp31F;void*_tmp320;struct _tuple1*_tmp321;void*_tmp322;struct Cyc_List_List*
_tmp323;void*_tmp324;struct Cyc_List_List*_tmp325;void*_tmp326;struct Cyc_Absyn_TunionInfo
_tmp327;void*_tmp328;struct Cyc_Absyn_Tuniondecl**_tmp329;struct Cyc_Absyn_Tuniondecl*
_tmp32A;struct Cyc_List_List*_tmp32B;void*_tmp32C;void*_tmp32D;struct Cyc_Absyn_TunionInfo
_tmp32E;void*_tmp32F;struct Cyc_Absyn_Tuniondecl**_tmp330;struct Cyc_Absyn_Tuniondecl*
_tmp331;struct Cyc_List_List*_tmp332;void*_tmp333;void*_tmp334;struct Cyc_Absyn_TunionFieldInfo
_tmp335;void*_tmp336;struct Cyc_Absyn_Tuniondecl*_tmp337;struct Cyc_Absyn_Tunionfield*
_tmp338;struct Cyc_List_List*_tmp339;void*_tmp33A;struct Cyc_Absyn_TunionFieldInfo
_tmp33B;void*_tmp33C;struct Cyc_Absyn_Tuniondecl*_tmp33D;struct Cyc_Absyn_Tunionfield*
_tmp33E;struct Cyc_List_List*_tmp33F;void*_tmp340;struct Cyc_Absyn_PtrInfo _tmp341;
void*_tmp342;void*_tmp343;struct Cyc_Absyn_Conref*_tmp344;struct Cyc_Absyn_Tqual
_tmp345;struct Cyc_Absyn_Conref*_tmp346;void*_tmp347;struct Cyc_Absyn_PtrInfo
_tmp348;void*_tmp349;void*_tmp34A;struct Cyc_Absyn_Conref*_tmp34B;struct Cyc_Absyn_Tqual
_tmp34C;struct Cyc_Absyn_Conref*_tmp34D;void*_tmp34E;void*_tmp34F;void*_tmp350;
void*_tmp351;void*_tmp352;void*_tmp353;void*_tmp354;void*_tmp355;void*_tmp356;
int _tmp357;void*_tmp358;int _tmp359;void*_tmp35A;void*_tmp35B;struct Cyc_Absyn_Tqual
_tmp35C;struct Cyc_Absyn_Exp*_tmp35D;void*_tmp35E;void*_tmp35F;struct Cyc_Absyn_Tqual
_tmp360;struct Cyc_Absyn_Exp*_tmp361;void*_tmp362;struct Cyc_Absyn_FnInfo _tmp363;
struct Cyc_List_List*_tmp364;struct Cyc_Core_Opt*_tmp365;void*_tmp366;struct Cyc_List_List*
_tmp367;int _tmp368;struct Cyc_Absyn_VarargInfo*_tmp369;struct Cyc_List_List*
_tmp36A;struct Cyc_List_List*_tmp36B;void*_tmp36C;struct Cyc_Absyn_FnInfo _tmp36D;
struct Cyc_List_List*_tmp36E;struct Cyc_Core_Opt*_tmp36F;void*_tmp370;struct Cyc_List_List*
_tmp371;int _tmp372;struct Cyc_Absyn_VarargInfo*_tmp373;struct Cyc_List_List*
_tmp374;struct Cyc_List_List*_tmp375;void*_tmp376;struct Cyc_List_List*_tmp377;
void*_tmp378;struct Cyc_List_List*_tmp379;void*_tmp37A;void*_tmp37B;struct Cyc_List_List*
_tmp37C;void*_tmp37D;void*_tmp37E;struct Cyc_List_List*_tmp37F;void*_tmp380;void*
_tmp381;void*_tmp382;void*_tmp383;void*_tmp384;void*_tmp385;void*_tmp386;void*
_tmp387;void*_tmp388;void*_tmp389;void*_tmp38A;void*_tmp38B;void*_tmp38C;void*
_tmp38D;void*_tmp38E;int _tmp38F;void*_tmp390;int _tmp391;void*_tmp392;void*
_tmp393;void*_tmp394;void*_tmp395;void*_tmp396;void*_tmp397;_LL287: _tmp30E=
_tmp30D.f1;if(_tmp30E <= (void*)3?1:*((int*)_tmp30E)!= 0)goto _LL289;_tmp30F=
_tmp30D.f2;if(_tmp30F <= (void*)3?1:*((int*)_tmp30F)!= 0)goto _LL289;_LL288:({void*
_tmp398[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typecmp: can only compare closed types",sizeof(char),39),_tag_arr(
_tmp398,sizeof(void*),0));});_LL289: _tmp310=_tmp30D.f1;if((int)_tmp310 != 0)goto
_LL28B;_tmp311=_tmp30D.f2;if((int)_tmp311 != 0)goto _LL28B;_LL28A: return 0;_LL28B:
_tmp312=_tmp30D.f1;if(_tmp312 <= (void*)3?1:*((int*)_tmp312)!= 1)goto _LL28D;
_tmp313=((struct Cyc_Absyn_VarType_struct*)_tmp312)->f1;_tmp314=_tmp30D.f2;if(
_tmp314 <= (void*)3?1:*((int*)_tmp314)!= 1)goto _LL28D;_tmp315=((struct Cyc_Absyn_VarType_struct*)
_tmp314)->f1;_LL28C: return Cyc_Core_intcmp(*((int*)_check_null(_tmp315->identity)),*((
int*)_check_null(_tmp313->identity)));_LL28D: _tmp316=_tmp30D.f1;if(_tmp316 <= (
void*)3?1:*((int*)_tmp316)!= 10)goto _LL28F;_tmp317=((struct Cyc_Absyn_AggrType_struct*)
_tmp316)->f1;_tmp318=(void*)_tmp317.aggr_info;_tmp319=_tmp317.targs;_tmp31A=
_tmp30D.f2;if(_tmp31A <= (void*)3?1:*((int*)_tmp31A)!= 10)goto _LL28F;_tmp31B=((
struct Cyc_Absyn_AggrType_struct*)_tmp31A)->f1;_tmp31C=(void*)_tmp31B.aggr_info;
_tmp31D=_tmp31B.targs;_LL28E: {struct _tuple1*_tmp39A;struct _tuple5 _tmp399=Cyc_Absyn_aggr_kinded_name(
_tmp318);_tmp39A=_tmp399.f2;{struct _tuple1*_tmp39C;struct _tuple5 _tmp39B=Cyc_Absyn_aggr_kinded_name(
_tmp31C);_tmp39C=_tmp39B.f2;{int _tmp39D=Cyc_Absyn_qvar_cmp(_tmp39A,_tmp39C);if(
_tmp39D != 0)return _tmp39D;else{return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp319,_tmp31D);}}}}_LL28F: _tmp31E=_tmp30D.f1;if(_tmp31E <= (void*)3?1:*((int*)
_tmp31E)!= 12)goto _LL291;_tmp31F=((struct Cyc_Absyn_EnumType_struct*)_tmp31E)->f1;
_tmp320=_tmp30D.f2;if(_tmp320 <= (void*)3?1:*((int*)_tmp320)!= 12)goto _LL291;
_tmp321=((struct Cyc_Absyn_EnumType_struct*)_tmp320)->f1;_LL290: return Cyc_Absyn_qvar_cmp(
_tmp31F,_tmp321);_LL291: _tmp322=_tmp30D.f1;if(_tmp322 <= (void*)3?1:*((int*)
_tmp322)!= 13)goto _LL293;_tmp323=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp322)->f1;
_tmp324=_tmp30D.f2;if(_tmp324 <= (void*)3?1:*((int*)_tmp324)!= 13)goto _LL293;
_tmp325=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp324)->f1;_LL292: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp323,
_tmp325);_LL293: _tmp326=_tmp30D.f1;if(_tmp326 <= (void*)3?1:*((int*)_tmp326)!= 2)
goto _LL295;_tmp327=((struct Cyc_Absyn_TunionType_struct*)_tmp326)->f1;_tmp328=(
void*)_tmp327.tunion_info;if(*((int*)_tmp328)!= 1)goto _LL295;_tmp329=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp328)->f1;_tmp32A=*_tmp329;_tmp32B=_tmp327.targs;_tmp32C=(void*)_tmp327.rgn;
_tmp32D=_tmp30D.f2;if(_tmp32D <= (void*)3?1:*((int*)_tmp32D)!= 2)goto _LL295;
_tmp32E=((struct Cyc_Absyn_TunionType_struct*)_tmp32D)->f1;_tmp32F=(void*)_tmp32E.tunion_info;
if(*((int*)_tmp32F)!= 1)goto _LL295;_tmp330=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp32F)->f1;_tmp331=*_tmp330;_tmp332=_tmp32E.targs;_tmp333=(void*)_tmp32E.rgn;
_LL294: if(_tmp331 == _tmp32A)return 0;{int _tmp39E=Cyc_Absyn_qvar_cmp(_tmp331->name,
_tmp32A->name);if(_tmp39E != 0)return _tmp39E;{int _tmp39F=Cyc_Tcutil_typecmp(
_tmp333,_tmp32C);if(_tmp39F != 0)return _tmp39F;return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp332,_tmp32B);}}_LL295: _tmp334=_tmp30D.f1;if(_tmp334 <= (void*)3?1:*((int*)
_tmp334)!= 3)goto _LL297;_tmp335=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp334)->f1;_tmp336=(void*)_tmp335.field_info;if(*((int*)_tmp336)!= 1)goto
_LL297;_tmp337=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp336)->f1;_tmp338=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp336)->f2;_tmp339=_tmp335.targs;
_tmp33A=_tmp30D.f2;if(_tmp33A <= (void*)3?1:*((int*)_tmp33A)!= 3)goto _LL297;
_tmp33B=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp33A)->f1;_tmp33C=(void*)
_tmp33B.field_info;if(*((int*)_tmp33C)!= 1)goto _LL297;_tmp33D=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp33C)->f1;_tmp33E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp33C)->f2;
_tmp33F=_tmp33B.targs;_LL296: if(_tmp33D == _tmp337)return 0;{int _tmp3A0=Cyc_Absyn_qvar_cmp(
_tmp337->name,_tmp33D->name);if(_tmp3A0 != 0)return _tmp3A0;{int _tmp3A1=Cyc_Absyn_qvar_cmp(
_tmp338->name,_tmp33E->name);if(_tmp3A1 != 0)return _tmp3A1;return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp33F,_tmp339);}}_LL297: _tmp340=_tmp30D.f1;if(_tmp340 <= (
void*)3?1:*((int*)_tmp340)!= 4)goto _LL299;_tmp341=((struct Cyc_Absyn_PointerType_struct*)
_tmp340)->f1;_tmp342=(void*)_tmp341.elt_typ;_tmp343=(void*)_tmp341.rgn_typ;
_tmp344=_tmp341.nullable;_tmp345=_tmp341.tq;_tmp346=_tmp341.bounds;_tmp347=
_tmp30D.f2;if(_tmp347 <= (void*)3?1:*((int*)_tmp347)!= 4)goto _LL299;_tmp348=((
struct Cyc_Absyn_PointerType_struct*)_tmp347)->f1;_tmp349=(void*)_tmp348.elt_typ;
_tmp34A=(void*)_tmp348.rgn_typ;_tmp34B=_tmp348.nullable;_tmp34C=_tmp348.tq;
_tmp34D=_tmp348.bounds;_LL298: {int _tmp3A2=Cyc_Tcutil_typecmp(_tmp349,_tmp342);
if(_tmp3A2 != 0)return _tmp3A2;{int _tmp3A3=Cyc_Tcutil_typecmp(_tmp34A,_tmp343);if(
_tmp3A3 != 0)return _tmp3A3;{int _tmp3A4=Cyc_Tcutil_tqual_cmp(_tmp34C,_tmp345);if(
_tmp3A4 != 0)return _tmp3A4;{int _tmp3A5=Cyc_Tcutil_conrefs_cmp(Cyc_Tcutil_boundscmp,
_tmp34D,_tmp346);if(_tmp3A5 != 0)return _tmp3A5;{void*_tmp3A6=(void*)(Cyc_Absyn_compress_conref(
_tmp34D))->v;void*_tmp3A7;_LL2C0: if(_tmp3A6 <= (void*)1?1:*((int*)_tmp3A6)!= 0)
goto _LL2C2;_tmp3A7=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp3A6)->f1;if((
int)_tmp3A7 != 0)goto _LL2C2;_LL2C1: return 0;_LL2C2:;_LL2C3: goto _LL2BF;_LL2BF:;}
return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp34B,_tmp344);}}}}_LL299: _tmp34E=
_tmp30D.f1;if(_tmp34E <= (void*)3?1:*((int*)_tmp34E)!= 5)goto _LL29B;_tmp34F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp34E)->f1;_tmp350=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp34E)->f2;_tmp351=_tmp30D.f2;if(_tmp351 <= (void*)3?1:*((int*)_tmp351)!= 5)
goto _LL29B;_tmp352=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp351)->f1;_tmp353=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp351)->f2;_LL29A: if(_tmp352 != _tmp34F)
return Cyc_Core_intcmp((int)((unsigned int)_tmp352),(int)((unsigned int)_tmp34F));
if(_tmp353 != _tmp350)return Cyc_Core_intcmp((int)((unsigned int)_tmp353),(int)((
unsigned int)_tmp350));return 0;_LL29B: _tmp354=_tmp30D.f1;if((int)_tmp354 != 1)
goto _LL29D;_tmp355=_tmp30D.f2;if((int)_tmp355 != 1)goto _LL29D;_LL29C: return 0;
_LL29D: _tmp356=_tmp30D.f1;if(_tmp356 <= (void*)3?1:*((int*)_tmp356)!= 6)goto
_LL29F;_tmp357=((struct Cyc_Absyn_DoubleType_struct*)_tmp356)->f1;_tmp358=_tmp30D.f2;
if(_tmp358 <= (void*)3?1:*((int*)_tmp358)!= 6)goto _LL29F;_tmp359=((struct Cyc_Absyn_DoubleType_struct*)
_tmp358)->f1;_LL29E: if(_tmp357 == _tmp359)return 0;else{if(_tmp357)return - 1;else{
return 1;}}_LL29F: _tmp35A=_tmp30D.f1;if(_tmp35A <= (void*)3?1:*((int*)_tmp35A)!= 7)
goto _LL2A1;_tmp35B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp35A)->f1;
_tmp35C=((struct Cyc_Absyn_ArrayType_struct*)_tmp35A)->f2;_tmp35D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp35A)->f3;_tmp35E=_tmp30D.f2;if(_tmp35E <= (void*)3?1:*((int*)_tmp35E)!= 7)
goto _LL2A1;_tmp35F=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp35E)->f1;
_tmp360=((struct Cyc_Absyn_ArrayType_struct*)_tmp35E)->f2;_tmp361=((struct Cyc_Absyn_ArrayType_struct*)
_tmp35E)->f3;_LL2A0: {int _tmp3A8=Cyc_Tcutil_tqual_cmp(_tmp360,_tmp35C);if(
_tmp3A8 != 0)return _tmp3A8;{int _tmp3A9=Cyc_Tcutil_typecmp(_tmp35F,_tmp35B);if(
_tmp3A9 != 0)return _tmp3A9;if(_tmp35D == _tmp361)return 0;if(_tmp35D == 0?1: _tmp361
== 0)({void*_tmp3AA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("missing expression in array index",sizeof(char),34),_tag_arr(_tmp3AA,
sizeof(void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp35D,_tmp361);}}_LL2A1: _tmp362=_tmp30D.f1;if(_tmp362 <= (void*)3?1:*((int*)
_tmp362)!= 8)goto _LL2A3;_tmp363=((struct Cyc_Absyn_FnType_struct*)_tmp362)->f1;
_tmp364=_tmp363.tvars;_tmp365=_tmp363.effect;_tmp366=(void*)_tmp363.ret_typ;
_tmp367=_tmp363.args;_tmp368=_tmp363.c_varargs;_tmp369=_tmp363.cyc_varargs;
_tmp36A=_tmp363.rgn_po;_tmp36B=_tmp363.attributes;_tmp36C=_tmp30D.f2;if(_tmp36C
<= (void*)3?1:*((int*)_tmp36C)!= 8)goto _LL2A3;_tmp36D=((struct Cyc_Absyn_FnType_struct*)
_tmp36C)->f1;_tmp36E=_tmp36D.tvars;_tmp36F=_tmp36D.effect;_tmp370=(void*)_tmp36D.ret_typ;
_tmp371=_tmp36D.args;_tmp372=_tmp36D.c_varargs;_tmp373=_tmp36D.cyc_varargs;
_tmp374=_tmp36D.rgn_po;_tmp375=_tmp36D.attributes;_LL2A2:({void*_tmp3AB[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: function types not handled",
sizeof(char),36),_tag_arr(_tmp3AB,sizeof(void*),0));});_LL2A3: _tmp376=_tmp30D.f1;
if(_tmp376 <= (void*)3?1:*((int*)_tmp376)!= 9)goto _LL2A5;_tmp377=((struct Cyc_Absyn_TupleType_struct*)
_tmp376)->f1;_tmp378=_tmp30D.f2;if(_tmp378 <= (void*)3?1:*((int*)_tmp378)!= 9)
goto _LL2A5;_tmp379=((struct Cyc_Absyn_TupleType_struct*)_tmp378)->f1;_LL2A4:
return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp379,
_tmp377);_LL2A5: _tmp37A=_tmp30D.f1;if(_tmp37A <= (void*)3?1:*((int*)_tmp37A)!= 11)
goto _LL2A7;_tmp37B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp37A)->f1;
_tmp37C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp37A)->f2;_tmp37D=_tmp30D.f2;
if(_tmp37D <= (void*)3?1:*((int*)_tmp37D)!= 11)goto _LL2A7;_tmp37E=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp37D)->f1;_tmp37F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp37D)->f2;_LL2A6: if(_tmp37E != _tmp37B){if(_tmp37E == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp37F,_tmp37C);_LL2A7: _tmp380=_tmp30D.f1;if((int)_tmp380 != 2)goto _LL2A9;
_tmp381=_tmp30D.f2;if((int)_tmp381 != 2)goto _LL2A9;_LL2A8: return 0;_LL2A9: _tmp382=
_tmp30D.f1;if(_tmp382 <= (void*)3?1:*((int*)_tmp382)!= 15)goto _LL2AB;_tmp383=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp382)->f1;_tmp384=_tmp30D.f2;
if(_tmp384 <= (void*)3?1:*((int*)_tmp384)!= 15)goto _LL2AB;_tmp385=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp384)->f1;_LL2AA: return Cyc_Tcutil_typecmp(
_tmp383,_tmp385);_LL2AB: _tmp386=_tmp30D.f1;if(_tmp386 <= (void*)3?1:*((int*)
_tmp386)!= 14)goto _LL2AD;_tmp387=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp386)->f1;_tmp388=_tmp30D.f2;if(_tmp388 <= (void*)3?1:*((int*)_tmp388)!= 14)
goto _LL2AD;_tmp389=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp388)->f1;
_LL2AC: return Cyc_Tcutil_typecmp(_tmp387,_tmp389);_LL2AD: _tmp38A=_tmp30D.f1;if(
_tmp38A <= (void*)3?1:*((int*)_tmp38A)!= 17)goto _LL2AF;_tmp38B=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp38A)->f1;_tmp38C=_tmp30D.f2;if(_tmp38C <= (void*)3?1:*((int*)_tmp38C)!= 17)
goto _LL2AF;_tmp38D=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp38C)->f1;_LL2AE:
return Cyc_Tcutil_typecmp(_tmp38B,_tmp38D);_LL2AF: _tmp38E=_tmp30D.f1;if(_tmp38E <= (
void*)3?1:*((int*)_tmp38E)!= 18)goto _LL2B1;_tmp38F=((struct Cyc_Absyn_TypeInt_struct*)
_tmp38E)->f1;_tmp390=_tmp30D.f2;if(_tmp390 <= (void*)3?1:*((int*)_tmp390)!= 18)
goto _LL2B1;_tmp391=((struct Cyc_Absyn_TypeInt_struct*)_tmp390)->f1;_LL2B0: return
Cyc_Core_intcmp(_tmp38F,_tmp391);_LL2B1: _tmp392=_tmp30D.f1;if(_tmp392 <= (void*)3?
1:*((int*)_tmp392)!= 20)goto _LL2B3;_LL2B2: goto _LL2B4;_LL2B3: _tmp393=_tmp30D.f2;
if(_tmp393 <= (void*)3?1:*((int*)_tmp393)!= 20)goto _LL2B5;_LL2B4: goto _LL2B6;
_LL2B5: _tmp394=_tmp30D.f1;if(_tmp394 <= (void*)3?1:*((int*)_tmp394)!= 19)goto
_LL2B7;_LL2B6: goto _LL2B8;_LL2B7: _tmp395=_tmp30D.f1;if(_tmp395 <= (void*)3?1:*((
int*)_tmp395)!= 21)goto _LL2B9;_LL2B8: goto _LL2BA;_LL2B9: _tmp396=_tmp30D.f2;if(
_tmp396 <= (void*)3?1:*((int*)_tmp396)!= 21)goto _LL2BB;_LL2BA: goto _LL2BC;_LL2BB:
_tmp397=_tmp30D.f2;if(_tmp397 <= (void*)3?1:*((int*)_tmp397)!= 19)goto _LL2BD;
_LL2BC:({void*_tmp3AC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("typecmp: effects not handled",sizeof(char),29),
_tag_arr(_tmp3AC,sizeof(void*),0));});_LL2BD:;_LL2BE:({void*_tmp3AD[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Unmatched case in typecmp",
sizeof(char),26),_tag_arr(_tmp3AD,sizeof(void*),0));});_LL286:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp3AE=Cyc_Tcutil_compress(t);_LL2C5: if(_tmp3AE <= (void*)3?1:*((
int*)_tmp3AE)!= 5)goto _LL2C7;_LL2C6: goto _LL2C8;_LL2C7: if((int)_tmp3AE != 1)goto
_LL2C9;_LL2C8: goto _LL2CA;_LL2C9: if(_tmp3AE <= (void*)3?1:*((int*)_tmp3AE)!= 6)
goto _LL2CB;_LL2CA: goto _LL2CC;_LL2CB: if(_tmp3AE <= (void*)3?1:*((int*)_tmp3AE)!= 
12)goto _LL2CD;_LL2CC: goto _LL2CE;_LL2CD: if(_tmp3AE <= (void*)3?1:*((int*)_tmp3AE)
!= 13)goto _LL2CF;_LL2CE: return 1;_LL2CF:;_LL2D0: return 0;_LL2C4:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp3B0=({struct _tuple6 _tmp3AF;_tmp3AF.f1=Cyc_Tcutil_compress(
t1);_tmp3AF.f2=Cyc_Tcutil_compress(t2);_tmp3AF;});void*_tmp3B1;int _tmp3B2;void*
_tmp3B3;int _tmp3B4;void*_tmp3B5;void*_tmp3B6;void*_tmp3B7;void*_tmp3B8;void*
_tmp3B9;void*_tmp3BA;void*_tmp3BB;void*_tmp3BC;void*_tmp3BD;void*_tmp3BE;void*
_tmp3BF;void*_tmp3C0;void*_tmp3C1;void*_tmp3C2;void*_tmp3C3;void*_tmp3C4;void*
_tmp3C5;void*_tmp3C6;void*_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*_tmp3CA;void*
_tmp3CB;void*_tmp3CC;void*_tmp3CD;void*_tmp3CE;void*_tmp3CF;void*_tmp3D0;void*
_tmp3D1;void*_tmp3D2;void*_tmp3D3;void*_tmp3D4;void*_tmp3D5;void*_tmp3D6;void*
_tmp3D7;void*_tmp3D8;void*_tmp3D9;void*_tmp3DA;void*_tmp3DB;void*_tmp3DC;void*
_tmp3DD;void*_tmp3DE;void*_tmp3DF;void*_tmp3E0;void*_tmp3E1;void*_tmp3E2;void*
_tmp3E3;_LL2D2: _tmp3B1=_tmp3B0.f1;if(_tmp3B1 <= (void*)3?1:*((int*)_tmp3B1)!= 6)
goto _LL2D4;_tmp3B2=((struct Cyc_Absyn_DoubleType_struct*)_tmp3B1)->f1;_tmp3B3=
_tmp3B0.f2;if(_tmp3B3 <= (void*)3?1:*((int*)_tmp3B3)!= 6)goto _LL2D4;_tmp3B4=((
struct Cyc_Absyn_DoubleType_struct*)_tmp3B3)->f1;_LL2D3: return !_tmp3B4?_tmp3B2: 0;
_LL2D4: _tmp3B5=_tmp3B0.f1;if(_tmp3B5 <= (void*)3?1:*((int*)_tmp3B5)!= 6)goto
_LL2D6;_tmp3B6=_tmp3B0.f2;if((int)_tmp3B6 != 1)goto _LL2D6;_LL2D5: goto _LL2D7;
_LL2D6: _tmp3B7=_tmp3B0.f1;if(_tmp3B7 <= (void*)3?1:*((int*)_tmp3B7)!= 6)goto
_LL2D8;_tmp3B8=_tmp3B0.f2;if(_tmp3B8 <= (void*)3?1:*((int*)_tmp3B8)!= 5)goto
_LL2D8;_LL2D7: goto _LL2D9;_LL2D8: _tmp3B9=_tmp3B0.f1;if(_tmp3B9 <= (void*)3?1:*((
int*)_tmp3B9)!= 6)goto _LL2DA;_tmp3BA=_tmp3B0.f2;if(_tmp3BA <= (void*)3?1:*((int*)
_tmp3BA)!= 14)goto _LL2DA;_LL2D9: goto _LL2DB;_LL2DA: _tmp3BB=_tmp3B0.f1;if((int)
_tmp3BB != 1)goto _LL2DC;_tmp3BC=_tmp3B0.f2;if(_tmp3BC <= (void*)3?1:*((int*)
_tmp3BC)!= 14)goto _LL2DC;_LL2DB: goto _LL2DD;_LL2DC: _tmp3BD=_tmp3B0.f1;if(_tmp3BD
<= (void*)3?1:*((int*)_tmp3BD)!= 6)goto _LL2DE;_tmp3BE=_tmp3B0.f2;if(_tmp3BE <= (
void*)3?1:*((int*)_tmp3BE)!= 17)goto _LL2DE;_LL2DD: goto _LL2DF;_LL2DE: _tmp3BF=
_tmp3B0.f1;if((int)_tmp3BF != 1)goto _LL2E0;_tmp3C0=_tmp3B0.f2;if(_tmp3C0 <= (void*)
3?1:*((int*)_tmp3C0)!= 17)goto _LL2E0;_LL2DF: goto _LL2E1;_LL2E0: _tmp3C1=_tmp3B0.f1;
if((int)_tmp3C1 != 1)goto _LL2E2;_tmp3C2=_tmp3B0.f2;if(_tmp3C2 <= (void*)3?1:*((int*)
_tmp3C2)!= 5)goto _LL2E2;_LL2E1: return 1;_LL2E2: _tmp3C3=_tmp3B0.f1;if(_tmp3C3 <= (
void*)3?1:*((int*)_tmp3C3)!= 5)goto _LL2E4;_tmp3C4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C3)->f2;if((int)_tmp3C4 != 3)goto _LL2E4;_tmp3C5=_tmp3B0.f2;if(_tmp3C5 <= (
void*)3?1:*((int*)_tmp3C5)!= 5)goto _LL2E4;_tmp3C6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C5)->f2;if((int)_tmp3C6 != 3)goto _LL2E4;_LL2E3: return 0;_LL2E4: _tmp3C7=
_tmp3B0.f1;if(_tmp3C7 <= (void*)3?1:*((int*)_tmp3C7)!= 5)goto _LL2E6;_tmp3C8=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3C7)->f2;if((int)_tmp3C8 != 3)goto _LL2E6;
_LL2E5: goto _LL2E7;_LL2E6: _tmp3C9=_tmp3B0.f1;if(_tmp3C9 <= (void*)3?1:*((int*)
_tmp3C9)!= 5)goto _LL2E8;_tmp3CA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C9)->f2;
if((int)_tmp3CA != 2)goto _LL2E8;_tmp3CB=_tmp3B0.f2;if((int)_tmp3CB != 1)goto _LL2E8;
_LL2E7: goto _LL2E9;_LL2E8: _tmp3CC=_tmp3B0.f1;if(_tmp3CC <= (void*)3?1:*((int*)
_tmp3CC)!= 5)goto _LL2EA;_tmp3CD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3CC)->f2;
if((int)_tmp3CD != 2)goto _LL2EA;_tmp3CE=_tmp3B0.f2;if(_tmp3CE <= (void*)3?1:*((int*)
_tmp3CE)!= 5)goto _LL2EA;_tmp3CF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3CE)->f2;
if((int)_tmp3CF != 1)goto _LL2EA;_LL2E9: goto _LL2EB;_LL2EA: _tmp3D0=_tmp3B0.f1;if(
_tmp3D0 <= (void*)3?1:*((int*)_tmp3D0)!= 5)goto _LL2EC;_tmp3D1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D0)->f2;if((int)_tmp3D1 != 2)goto _LL2EC;_tmp3D2=_tmp3B0.f2;if(_tmp3D2 <= (
void*)3?1:*((int*)_tmp3D2)!= 5)goto _LL2EC;_tmp3D3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D2)->f2;if((int)_tmp3D3 != 0)goto _LL2EC;_LL2EB: goto _LL2ED;_LL2EC: _tmp3D4=
_tmp3B0.f1;if(_tmp3D4 <= (void*)3?1:*((int*)_tmp3D4)!= 5)goto _LL2EE;_tmp3D5=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3D4)->f2;if((int)_tmp3D5 != 1)goto _LL2EE;
_tmp3D6=_tmp3B0.f2;if(_tmp3D6 <= (void*)3?1:*((int*)_tmp3D6)!= 5)goto _LL2EE;
_tmp3D7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D6)->f2;if((int)_tmp3D7 != 
0)goto _LL2EE;_LL2ED: goto _LL2EF;_LL2EE: _tmp3D8=_tmp3B0.f1;if(_tmp3D8 <= (void*)3?1:*((
int*)_tmp3D8)!= 17)goto _LL2F0;_tmp3D9=_tmp3B0.f2;if(_tmp3D9 <= (void*)3?1:*((int*)
_tmp3D9)!= 5)goto _LL2F0;_tmp3DA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D9)->f2;
if((int)_tmp3DA != 1)goto _LL2F0;_LL2EF: goto _LL2F1;_LL2F0: _tmp3DB=_tmp3B0.f1;if(
_tmp3DB <= (void*)3?1:*((int*)_tmp3DB)!= 17)goto _LL2F2;_tmp3DC=_tmp3B0.f2;if(
_tmp3DC <= (void*)3?1:*((int*)_tmp3DC)!= 5)goto _LL2F2;_tmp3DD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3DC)->f2;if((int)_tmp3DD != 0)goto _LL2F2;_LL2F1: goto _LL2F3;_LL2F2: _tmp3DE=
_tmp3B0.f1;if(_tmp3DE <= (void*)3?1:*((int*)_tmp3DE)!= 14)goto _LL2F4;_tmp3DF=
_tmp3B0.f2;if(_tmp3DF <= (void*)3?1:*((int*)_tmp3DF)!= 5)goto _LL2F4;_tmp3E0=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3DF)->f2;if((int)_tmp3E0 != 1)goto _LL2F4;
_LL2F3: goto _LL2F5;_LL2F4: _tmp3E1=_tmp3B0.f1;if(_tmp3E1 <= (void*)3?1:*((int*)
_tmp3E1)!= 14)goto _LL2F6;_tmp3E2=_tmp3B0.f2;if(_tmp3E2 <= (void*)3?1:*((int*)
_tmp3E2)!= 5)goto _LL2F6;_tmp3E3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E2)->f2;
if((int)_tmp3E3 != 0)goto _LL2F6;_LL2F5: return 1;_LL2F6:;_LL2F7: return 0;_LL2D1:;}
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=
el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0?1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->v=(
void*)t1;_tmp3E4;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)((
struct Cyc_Core_Opt*)_check_null(max_arith_type))->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_Std_String_pa_struct _tmp3E7;_tmp3E7.tag=0;_tmp3E7.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp3E6;_tmp3E6.tag=0;_tmp3E6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp3E5[2]={& _tmp3E6,& _tmp3E7};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,
_tag_arr("type mismatch: expecting %s but found %s",sizeof(char),41),_tag_arr(
_tmp3E5,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp3E8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL2F9: if(_tmp3E8 <= (void*)3?1:*((int*)_tmp3E8)!= 4)goto _LL2FB;_LL2FA: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ);goto _LL2F8;_LL2FB:;_LL2FC: return 0;_LL2F8:;}return 1;}int
Cyc_Tcutil_is_integral_type(void*t){void*_tmp3E9=Cyc_Tcutil_compress(t);_LL2FE:
if(_tmp3E9 <= (void*)3?1:*((int*)_tmp3E9)!= 5)goto _LL300;_LL2FF: goto _LL301;_LL300:
if(_tmp3E9 <= (void*)3?1:*((int*)_tmp3E9)!= 14)goto _LL302;_LL301: goto _LL303;
_LL302: if(_tmp3E9 <= (void*)3?1:*((int*)_tmp3E9)!= 17)goto _LL304;_LL303: goto
_LL305;_LL304: if(_tmp3E9 <= (void*)3?1:*((int*)_tmp3E9)!= 12)goto _LL306;_LL305:
goto _LL307;_LL306: if(_tmp3E9 <= (void*)3?1:*((int*)_tmp3E9)!= 13)goto _LL308;
_LL307: return 1;_LL308:;_LL309: return 0;_LL2FD:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp3EA[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3EA,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp3EB[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3EB,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp3ED=({struct _tuple6 _tmp3EC;
_tmp3EC.f1=t1;_tmp3EC.f2=t2;_tmp3EC;});void*_tmp3EE;struct Cyc_Absyn_PtrInfo
_tmp3EF;void*_tmp3F0;struct Cyc_Absyn_PtrInfo _tmp3F1;void*_tmp3F2;void*_tmp3F3;
struct Cyc_Absyn_Tqual _tmp3F4;struct Cyc_Absyn_Exp*_tmp3F5;void*_tmp3F6;void*
_tmp3F7;struct Cyc_Absyn_Tqual _tmp3F8;struct Cyc_Absyn_Exp*_tmp3F9;void*_tmp3FA;
struct Cyc_Absyn_TunionFieldInfo _tmp3FB;void*_tmp3FC;struct Cyc_Absyn_Tuniondecl*
_tmp3FD;struct Cyc_Absyn_Tunionfield*_tmp3FE;struct Cyc_List_List*_tmp3FF;void*
_tmp400;struct Cyc_Absyn_TunionInfo _tmp401;void*_tmp402;struct Cyc_Absyn_Tuniondecl**
_tmp403;struct Cyc_Absyn_Tuniondecl*_tmp404;struct Cyc_List_List*_tmp405;void*
_tmp406;struct Cyc_Absyn_PtrInfo _tmp407;void*_tmp408;void*_tmp409;struct Cyc_Absyn_Conref*
_tmp40A;struct Cyc_Absyn_Tqual _tmp40B;struct Cyc_Absyn_Conref*_tmp40C;void*_tmp40D;
struct Cyc_Absyn_TunionInfo _tmp40E;void*_tmp40F;struct Cyc_Absyn_Tuniondecl**
_tmp410;struct Cyc_Absyn_Tuniondecl*_tmp411;struct Cyc_List_List*_tmp412;void*
_tmp413;void*_tmp414;void*_tmp415;void*_tmp416;void*_tmp417;void*_tmp418;void*
_tmp419;_LL30B: _tmp3EE=_tmp3ED.f1;if(_tmp3EE <= (void*)3?1:*((int*)_tmp3EE)!= 4)
goto _LL30D;_tmp3EF=((struct Cyc_Absyn_PointerType_struct*)_tmp3EE)->f1;_tmp3F0=
_tmp3ED.f2;if(_tmp3F0 <= (void*)3?1:*((int*)_tmp3F0)!= 4)goto _LL30D;_tmp3F1=((
struct Cyc_Absyn_PointerType_struct*)_tmp3F0)->f1;_LL30C: {int okay=1;if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp3EF.nullable,_tmp3F1.nullable)){void*_tmp41A=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp3EF.nullable))->v;int _tmp41B;_LL31A: if(_tmp41A <= (void*)1?1:*((int*)_tmp41A)
!= 0)goto _LL31C;_tmp41B=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp41A)->f1;
_LL31B: okay=!_tmp41B;goto _LL319;_LL31C:;_LL31D:({void*_tmp41C[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("silent_castable conref not eq",
sizeof(char),30),_tag_arr(_tmp41C,sizeof(void*),0));});_LL319:;}if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp3EF.bounds,_tmp3F1.bounds)){struct _tuple6 _tmp41E=({
struct _tuple6 _tmp41D;_tmp41D.f1=(void*)(Cyc_Absyn_compress_conref(_tmp3EF.bounds))->v;
_tmp41D.f2=(void*)(Cyc_Absyn_compress_conref(_tmp3F1.bounds))->v;_tmp41D;});void*
_tmp41F;void*_tmp420;void*_tmp421;void*_tmp422;void*_tmp423;void*_tmp424;struct
Cyc_Absyn_Exp*_tmp425;void*_tmp426;void*_tmp427;struct Cyc_Absyn_Exp*_tmp428;void*
_tmp429;void*_tmp42A;void*_tmp42B;void*_tmp42C;_LL31F: _tmp41F=_tmp41E.f1;if(
_tmp41F <= (void*)1?1:*((int*)_tmp41F)!= 0)goto _LL321;_tmp420=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp41F)->f1;if(_tmp420 <= (void*)1?1:*((int*)_tmp420)!= 0)goto _LL321;_tmp421=
_tmp41E.f2;if(_tmp421 <= (void*)1?1:*((int*)_tmp421)!= 0)goto _LL321;_tmp422=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp421)->f1;if((int)_tmp422 != 0)goto _LL321;
_LL320: okay=1;goto _LL31E;_LL321: _tmp423=_tmp41E.f1;if(_tmp423 <= (void*)1?1:*((
int*)_tmp423)!= 0)goto _LL323;_tmp424=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp423)->f1;if(_tmp424 <= (void*)1?1:*((int*)_tmp424)!= 0)goto _LL323;_tmp425=((
struct Cyc_Absyn_Upper_b_struct*)_tmp424)->f1;_tmp426=_tmp41E.f2;if(_tmp426 <= (
void*)1?1:*((int*)_tmp426)!= 0)goto _LL323;_tmp427=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp426)->f1;if(_tmp427 <= (void*)1?1:*((int*)_tmp427)!= 0)goto _LL323;_tmp428=((
struct Cyc_Absyn_Upper_b_struct*)_tmp427)->f1;_LL322: okay=okay?Cyc_Evexp_lte_const_exp(
_tmp428,_tmp425): 0;({void*_tmp42D[0]={};Cyc_Tcutil_warn(loc,_tag_arr("implicit cast to shorter array",
sizeof(char),31),_tag_arr(_tmp42D,sizeof(void*),0));});goto _LL31E;_LL323: _tmp429=
_tmp41E.f1;if(_tmp429 <= (void*)1?1:*((int*)_tmp429)!= 0)goto _LL325;_tmp42A=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp429)->f1;if((int)_tmp42A != 0)goto _LL325;
_tmp42B=_tmp41E.f2;if(_tmp42B <= (void*)1?1:*((int*)_tmp42B)!= 0)goto _LL325;
_tmp42C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp42B)->f1;if((int)_tmp42C
!= 0)goto _LL325;_LL324: okay=1;goto _LL31E;_LL325:;_LL326: okay=0;goto _LL31E;_LL31E:;}
okay=okay?Cyc_Tcutil_unify((void*)_tmp3EF.elt_typ,(void*)_tmp3F1.elt_typ): 0;okay=
okay?Cyc_Tcutil_unify((void*)_tmp3EF.rgn_typ,(void*)_tmp3F1.rgn_typ)?1: Cyc_Tcenv_region_outlives(
te,(void*)_tmp3EF.rgn_typ,(void*)_tmp3F1.rgn_typ): 0;okay=okay?!(_tmp3EF.tq).q_const?
1:(_tmp3F1.tq).q_const: 0;return okay;}_LL30D: _tmp3F2=_tmp3ED.f1;if(_tmp3F2 <= (
void*)3?1:*((int*)_tmp3F2)!= 7)goto _LL30F;_tmp3F3=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp3F2)->f1;_tmp3F4=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F2)->f2;_tmp3F5=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F2)->f3;_tmp3F6=_tmp3ED.f2;if(_tmp3F6 <= (
void*)3?1:*((int*)_tmp3F6)!= 7)goto _LL30F;_tmp3F7=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp3F6)->f1;_tmp3F8=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F6)->f2;_tmp3F9=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F6)->f3;_LL30E: {int okay;okay=(_tmp3F5 != 
0?_tmp3F9 != 0: 0)?Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp3F5),(struct Cyc_Absyn_Exp*)_check_null(_tmp3F9)): 0;return(okay?Cyc_Tcutil_unify(
_tmp3F3,_tmp3F7): 0)?!_tmp3F4.q_const?1: _tmp3F8.q_const: 0;}_LL30F: _tmp3FA=_tmp3ED.f1;
if(_tmp3FA <= (void*)3?1:*((int*)_tmp3FA)!= 3)goto _LL311;_tmp3FB=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3FA)->f1;_tmp3FC=(void*)_tmp3FB.field_info;if(*((int*)_tmp3FC)!= 1)goto
_LL311;_tmp3FD=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3FC)->f1;_tmp3FE=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3FC)->f2;_tmp3FF=_tmp3FB.targs;
_tmp400=_tmp3ED.f2;if(_tmp400 <= (void*)3?1:*((int*)_tmp400)!= 2)goto _LL311;
_tmp401=((struct Cyc_Absyn_TunionType_struct*)_tmp400)->f1;_tmp402=(void*)_tmp401.tunion_info;
if(*((int*)_tmp402)!= 1)goto _LL311;_tmp403=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp402)->f1;_tmp404=*_tmp403;_tmp405=_tmp401.targs;_LL310: if((_tmp3FD == _tmp404?
1: Cyc_Absyn_qvar_cmp(_tmp3FD->name,_tmp404->name)== 0)?_tmp3FE->typs == 0: 0){for(
0;_tmp3FF != 0?_tmp405 != 0: 0;(_tmp3FF=_tmp3FF->tl,_tmp405=_tmp405->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp3FF->hd,(void*)_tmp405->hd))break;}if(_tmp3FF == 0?_tmp405 == 0: 0)return
1;}return 0;_LL311: _tmp406=_tmp3ED.f1;if(_tmp406 <= (void*)3?1:*((int*)_tmp406)!= 
4)goto _LL313;_tmp407=((struct Cyc_Absyn_PointerType_struct*)_tmp406)->f1;_tmp408=(
void*)_tmp407.elt_typ;_tmp409=(void*)_tmp407.rgn_typ;_tmp40A=_tmp407.nullable;
_tmp40B=_tmp407.tq;_tmp40C=_tmp407.bounds;_tmp40D=_tmp3ED.f2;if(_tmp40D <= (void*)
3?1:*((int*)_tmp40D)!= 2)goto _LL313;_tmp40E=((struct Cyc_Absyn_TunionType_struct*)
_tmp40D)->f1;_tmp40F=(void*)_tmp40E.tunion_info;if(*((int*)_tmp40F)!= 1)goto
_LL313;_tmp410=((struct Cyc_Absyn_KnownTunion_struct*)_tmp40F)->f1;_tmp411=*
_tmp410;_tmp412=_tmp40E.targs;_tmp413=(void*)_tmp40E.rgn;_LL312:{void*_tmp42E=
Cyc_Tcutil_compress(_tmp408);struct Cyc_Absyn_TunionFieldInfo _tmp42F;void*_tmp430;
struct Cyc_Absyn_Tuniondecl*_tmp431;struct Cyc_Absyn_Tunionfield*_tmp432;struct Cyc_List_List*
_tmp433;_LL328: if(_tmp42E <= (void*)3?1:*((int*)_tmp42E)!= 3)goto _LL32A;_tmp42F=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp42E)->f1;_tmp430=(void*)_tmp42F.field_info;
if(*((int*)_tmp430)!= 1)goto _LL32A;_tmp431=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp430)->f1;_tmp432=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp430)->f2;
_tmp433=_tmp42F.targs;_LL329: if(!Cyc_Tcutil_unify(_tmp409,_tmp413)?!Cyc_Tcenv_region_outlives(
te,_tmp409,_tmp413): 0)return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp40A,((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(0)))return 0;if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp40C,Cyc_Absyn_new_conref(Cyc_Absyn_bounds_one)))return 0;
if(Cyc_Absyn_qvar_cmp(_tmp411->name,_tmp431->name)== 0?_tmp432->typs != 0: 0){int
okay=1;for(0;_tmp433 != 0?_tmp412 != 0: 0;(_tmp433=_tmp433->tl,_tmp412=_tmp412->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp433->hd,(void*)_tmp412->hd)){okay=0;break;}}if((
!okay?1: _tmp433 != 0)?1: _tmp412 != 0)return 0;return 1;}goto _LL327;_LL32A:;_LL32B:
goto _LL327;_LL327:;}return 0;_LL313: _tmp414=_tmp3ED.f1;if(_tmp414 <= (void*)3?1:*((
int*)_tmp414)!= 14)goto _LL315;_tmp415=_tmp3ED.f2;if(_tmp415 <= (void*)3?1:*((int*)
_tmp415)!= 5)goto _LL315;_tmp416=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp415)->f2;
if((int)_tmp416 != 2)goto _LL315;_LL314: goto _LL316;_LL315: _tmp417=_tmp3ED.f1;if(
_tmp417 <= (void*)3?1:*((int*)_tmp417)!= 17)goto _LL317;_tmp418=_tmp3ED.f2;if(
_tmp418 <= (void*)3?1:*((int*)_tmp418)!= 5)goto _LL317;_tmp419=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp418)->f2;if((int)_tmp419 != 2)goto _LL317;_LL316: return 1;_LL317:;_LL318: return
Cyc_Tcutil_unify(t1,t2);_LL30A:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*
_tmp434=Cyc_Tcutil_compress(t);_LL32D: if(_tmp434 <= (void*)3?1:*((int*)_tmp434)!= 
4)goto _LL32F;_LL32E: return 1;_LL32F:;_LL330: return 0;_LL32C:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp435=(void*)e->r;void*_tmp436;int _tmp437;_LL332:
if(*((int*)_tmp435)!= 0)goto _LL334;_tmp436=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp435)->f1;if(_tmp436 <= (void*)1?1:*((int*)_tmp436)!= 2)goto _LL334;_tmp437=((
struct Cyc_Absyn_Int_c_struct*)_tmp436)->f2;if(_tmp437 != 0)goto _LL334;_LL333:
return 1;_LL334:;_LL335: return 0;_LL331:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((
void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt
Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)
1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*
e1){if(Cyc_Tcutil_is_pointer_type(t2)?Cyc_Tcutil_is_zero(e1): 0){(void*)(e1->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp438=_cycalloc(sizeof(*_tmp438));
_tmp438[0]=({struct Cyc_Absyn_Const_e_struct _tmp439;_tmp439.tag=0;_tmp439.f1=(
void*)((void*)0);_tmp439;});_tmp438;})));{struct Cyc_List_List*_tmp43A=Cyc_Tcenv_lookup_type_vars(
te);struct Cyc_Absyn_PointerType_struct*_tmp43B=({struct Cyc_Absyn_PointerType_struct*
_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C[0]=({struct Cyc_Absyn_PointerType_struct
_tmp43D;_tmp43D.tag=4;_tmp43D.f1=({struct Cyc_Absyn_PtrInfo _tmp43E;_tmp43E.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440->v=_tmp43A;_tmp440;}));_tmp43E.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->v=_tmp43A;_tmp43F;}));_tmp43E.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp43E.tq=Cyc_Absyn_empty_tqual();
_tmp43E.bounds=Cyc_Absyn_empty_conref();_tmp43E;});_tmp43D;});_tmp43C;});(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp43B));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}static int Cyc_Tcutil_is_sizeof_type(void*t1){void*_tmp441=
Cyc_Tcutil_compress(t1);_LL337: if(_tmp441 <= (void*)3?1:*((int*)_tmp441)!= 14)
goto _LL339;_LL338: return 1;_LL339:;_LL33A: return 0;_LL336:;}static int Cyc_Tcutil_is_tag_type(
void*t1){void*_tmp442=Cyc_Tcutil_compress(t1);_LL33C: if(_tmp442 <= (void*)3?1:*((
int*)_tmp442)!= 17)goto _LL33E;_LL33D: return 1;_LL33E:;_LL33F: return 0;_LL33B:;}int
Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
if(Cyc_Tcutil_unify(t1,t2))return 1;if(((Cyc_Tcutil_is_arithmetic_type(t1)?Cyc_Tcutil_is_arithmetic_type(
t2): 0)?1:(Cyc_Tcutil_is_sizeof_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0))?1:(
Cyc_Tcutil_is_tag_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0)){if(Cyc_Tcutil_will_lose_precision(
t1,t2))({struct Cyc_Std_String_pa_struct _tmp445;_tmp445.tag=0;_tmp445.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmp444;
_tmp444.tag=0;_tmp444.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmp443[2]={& _tmp444,& _tmp445};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; %s -> %s conversion supplied",
sizeof(char),53),_tag_arr(_tmp443,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if(Cyc_Tcutil_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);({struct Cyc_Std_String_pa_struct
_tmp448;_tmp448.tag=0;_tmp448.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp447;_tmp447.tag=0;_tmp447.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp446[2]={& _tmp447,& _tmp448};Cyc_Tcutil_warn(
e->loc,_tag_arr("implicit cast from %s to %s",sizeof(char),28),_tag_arr(_tmp446,
sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp449=Cyc_Tcutil_compress(t);
_LL341: if(_tmp449 <= (void*)3?1:*((int*)_tmp449)!= 5)goto _LL343;_LL342: goto _LL344;
_LL343: if((int)_tmp449 != 1)goto _LL345;_LL344: goto _LL346;_LL345: if(_tmp449 <= (
void*)3?1:*((int*)_tmp449)!= 6)goto _LL347;_LL346: return 1;_LL347:;_LL348: return 0;
_LL340:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*inst,
struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp44A=_cycalloc(sizeof(*
_tmp44A));_tmp44A->f1=x->tq;_tmp44A->f2=Cyc_Tcutil_substitute(inst,(void*)x->type);
_tmp44A;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp44B=t1;struct Cyc_List_List*
_tmp44C;struct Cyc_Absyn_AggrInfo _tmp44D;void*_tmp44E;struct Cyc_Absyn_Aggrdecl**
_tmp44F;struct Cyc_Absyn_Aggrdecl*_tmp450;struct Cyc_List_List*_tmp451;void*
_tmp452;struct Cyc_List_List*_tmp453;_LL34A: if((int)_tmp44B != 0)goto _LL34C;_LL34B:
return 0;_LL34C: if(_tmp44B <= (void*)3?1:*((int*)_tmp44B)!= 9)goto _LL34E;_tmp44C=((
struct Cyc_Absyn_TupleType_struct*)_tmp44B)->f1;_LL34D: return _tmp44C;_LL34E: if(
_tmp44B <= (void*)3?1:*((int*)_tmp44B)!= 10)goto _LL350;_tmp44D=((struct Cyc_Absyn_AggrType_struct*)
_tmp44B)->f1;_tmp44E=(void*)_tmp44D.aggr_info;if(*((int*)_tmp44E)!= 1)goto _LL350;
_tmp44F=((struct Cyc_Absyn_KnownAggr_struct*)_tmp44E)->f1;_tmp450=*_tmp44F;
_tmp451=_tmp44D.targs;_LL34F: if((((void*)_tmp450->kind == (void*)1?1: _tmp450->impl
== 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp450->impl))->exist_vars
!= 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp450->impl))->rgn_po != 0)
return({struct Cyc_List_List*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454->hd=({
struct _tuple4*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->f1=Cyc_Absyn_empty_tqual();
_tmp455->f2=t1;_tmp455;});_tmp454->tl=0;_tmp454;});{struct Cyc_List_List*_tmp456=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp450->tvs,_tmp451);return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp456,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp450->impl))->fields);}_LL350: if(_tmp44B <= (void*)3?1:*((int*)
_tmp44B)!= 11)goto _LL352;_tmp452=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp44B)->f1;if((int)_tmp452 != 0)goto _LL352;_tmp453=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp44B)->f2;_LL351: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp453);_LL352:;_LL353: return({struct Cyc_List_List*
_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->hd=({struct _tuple4*_tmp458=
_cycalloc(sizeof(*_tmp458));_tmp458->f1=Cyc_Absyn_empty_tqual();_tmp458->f2=t1;
_tmp458;});_tmp457->tl=0;_tmp457;});_LL349:;}}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1)?Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2):
0)return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp45A=({struct _tuple6 _tmp459;_tmp459.f1=t1;_tmp459.f2=t2;_tmp459;});void*
_tmp45B;struct Cyc_Absyn_PtrInfo _tmp45C;void*_tmp45D;void*_tmp45E;struct Cyc_Absyn_Conref*
_tmp45F;struct Cyc_Absyn_Tqual _tmp460;struct Cyc_Absyn_Conref*_tmp461;void*_tmp462;
struct Cyc_Absyn_PtrInfo _tmp463;void*_tmp464;void*_tmp465;struct Cyc_Absyn_Conref*
_tmp466;struct Cyc_Absyn_Tqual _tmp467;struct Cyc_Absyn_Conref*_tmp468;_LL355:
_tmp45B=_tmp45A.f1;if(_tmp45B <= (void*)3?1:*((int*)_tmp45B)!= 4)goto _LL357;
_tmp45C=((struct Cyc_Absyn_PointerType_struct*)_tmp45B)->f1;_tmp45D=(void*)
_tmp45C.elt_typ;_tmp45E=(void*)_tmp45C.rgn_typ;_tmp45F=_tmp45C.nullable;_tmp460=
_tmp45C.tq;_tmp461=_tmp45C.bounds;_tmp462=_tmp45A.f2;if(_tmp462 <= (void*)3?1:*((
int*)_tmp462)!= 4)goto _LL357;_tmp463=((struct Cyc_Absyn_PointerType_struct*)
_tmp462)->f1;_tmp464=(void*)_tmp463.elt_typ;_tmp465=(void*)_tmp463.rgn_typ;
_tmp466=_tmp463.nullable;_tmp467=_tmp463.tq;_tmp468=_tmp463.bounds;_LL356: if(
_tmp460.q_const?!_tmp467.q_const: 0)return 0;if((!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp45F,_tmp466)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp45F): 0)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp466): 0)
return 0;if(!Cyc_Tcutil_unify(_tmp45E,_tmp465)?!Cyc_Tcenv_region_outlives(te,
_tmp45E,_tmp465): 0)return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp461,_tmp468)){struct _tuple6 _tmp46A=({struct _tuple6 _tmp469;_tmp469.f1=Cyc_Absyn_conref_val(
_tmp461);_tmp469.f2=Cyc_Absyn_conref_val(_tmp468);_tmp469;});void*_tmp46B;void*
_tmp46C;void*_tmp46D;struct Cyc_Absyn_Exp*_tmp46E;void*_tmp46F;struct Cyc_Absyn_Exp*
_tmp470;_LL35A: _tmp46B=_tmp46A.f1;if(_tmp46B <= (void*)1?1:*((int*)_tmp46B)!= 0)
goto _LL35C;_tmp46C=_tmp46A.f2;if((int)_tmp46C != 0)goto _LL35C;_LL35B: goto _LL359;
_LL35C: _tmp46D=_tmp46A.f1;if(_tmp46D <= (void*)1?1:*((int*)_tmp46D)!= 0)goto
_LL35E;_tmp46E=((struct Cyc_Absyn_Upper_b_struct*)_tmp46D)->f1;_tmp46F=_tmp46A.f2;
if(_tmp46F <= (void*)1?1:*((int*)_tmp46F)!= 0)goto _LL35E;_tmp470=((struct Cyc_Absyn_Upper_b_struct*)
_tmp46F)->f1;_LL35D: if(!Cyc_Evexp_lte_const_exp(_tmp470,_tmp46E))return 0;goto
_LL359;_LL35E:;_LL35F: return 0;_LL359:;}return Cyc_Tcutil_ptrsubtype(te,({struct
Cyc_List_List*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->hd=({struct _tuple6*
_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->f1=t1;_tmp472->f2=t2;_tmp472;});
_tmp471->tl=assume;_tmp471;}),_tmp45D,_tmp464);_LL357:;_LL358: return 0;_LL354:;}}
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp474=({struct
_tuple6 _tmp473;_tmp473.f1=Cyc_Tcutil_compress(t1);_tmp473.f2=Cyc_Tcutil_compress(
t2);_tmp473;});void*_tmp475;void*_tmp476;void*_tmp477;void*_tmp478;_LL361:
_tmp475=_tmp474.f1;if(_tmp475 <= (void*)3?1:*((int*)_tmp475)!= 5)goto _LL363;
_tmp476=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp475)->f2;_tmp477=_tmp474.f2;
if(_tmp477 <= (void*)3?1:*((int*)_tmp477)!= 5)goto _LL363;_tmp478=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp477)->f2;_LL362: return _tmp476 == _tmp478;_LL363:;
_LL364: return 0;_LL360:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp47A;struct
Cyc_Absyn_Tqual _tmp47B;void*_tmp47C;struct _tuple4*_tmp479=(struct _tuple4*)tqs1->hd;
_tmp47A=*_tmp479;_tmp47B=_tmp47A.f1;_tmp47C=_tmp47A.f2;{struct _tuple4 _tmp47E;
struct Cyc_Absyn_Tqual _tmp47F;void*_tmp480;struct _tuple4*_tmp47D=(struct _tuple4*)
tqs2->hd;_tmp47E=*_tmp47D;_tmp47F=_tmp47E.f1;_tmp480=_tmp47E.f2;if(_tmp47F.q_const?
Cyc_Tcutil_subtype(te,assume,_tmp47C,_tmp480): 0)continue;else{if(Cyc_Tcutil_unify(
_tmp47C,_tmp480))continue;else{if(Cyc_Tcutil_isomorphic(_tmp47C,_tmp480))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp481=Cyc_Tcutil_compress(t);void*_tmp482;_LL366: if(_tmp481 <= (void*)3?1:*((
int*)_tmp481)!= 5)goto _LL368;_tmp482=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp481)->f2;if((int)_tmp482 != 0)goto _LL368;_LL367: return 1;_LL368:;_LL369: return
0;_LL365:;}int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{void*_tmp483=t2;void*_tmp484;_LL36B: if(_tmp483 <= (
void*)3?1:*((int*)_tmp483)!= 5)goto _LL36D;_tmp484=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp483)->f2;if((int)_tmp484 != 2)goto _LL36D;_LL36C: if(Cyc_Tcutil_typ_kind(t2)== (
void*)2)return 1;goto _LL36A;_LL36D:;_LL36E: goto _LL36A;_LL36A:;}{void*_tmp485=t1;
struct Cyc_Absyn_PtrInfo _tmp486;void*_tmp487;void*_tmp488;struct Cyc_Absyn_Conref*
_tmp489;struct Cyc_Absyn_Tqual _tmp48A;struct Cyc_Absyn_Conref*_tmp48B;void*_tmp48C;
struct Cyc_Absyn_Tqual _tmp48D;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Enumdecl*
_tmp48F;_LL370: if(_tmp485 <= (void*)3?1:*((int*)_tmp485)!= 4)goto _LL372;_tmp486=((
struct Cyc_Absyn_PointerType_struct*)_tmp485)->f1;_tmp487=(void*)_tmp486.elt_typ;
_tmp488=(void*)_tmp486.rgn_typ;_tmp489=_tmp486.nullable;_tmp48A=_tmp486.tq;
_tmp48B=_tmp486.bounds;_LL371:{void*_tmp490=t2;struct Cyc_Absyn_PtrInfo _tmp491;
void*_tmp492;void*_tmp493;struct Cyc_Absyn_Conref*_tmp494;struct Cyc_Absyn_Tqual
_tmp495;struct Cyc_Absyn_Conref*_tmp496;_LL381: if(_tmp490 <= (void*)3?1:*((int*)
_tmp490)!= 4)goto _LL383;_tmp491=((struct Cyc_Absyn_PointerType_struct*)_tmp490)->f1;
_tmp492=(void*)_tmp491.elt_typ;_tmp493=(void*)_tmp491.rgn_typ;_tmp494=_tmp491.nullable;
_tmp495=_tmp491.tq;_tmp496=_tmp491.bounds;_LL382: {struct Cyc_List_List*_tmp497=({
struct Cyc_List_List*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0->hd=({struct
_tuple6*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->f1=t1;_tmp4A1->f2=t2;
_tmp4A1;});_tmp4A0->tl=0;_tmp4A0;});int _tmp498=Cyc_Tcutil_ptrsubtype(te,_tmp497,
_tmp487,_tmp492)?!_tmp48A.q_const?1: _tmp495.q_const: 0;int _tmp499=_tmp498?0:((Cyc_Tcutil_bits_only(
_tmp487)?Cyc_Tcutil_is_char_type(_tmp492): 0)?_tmp495.q_const?1: !_tmp48A.q_const:
0);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp48B,_tmp496);
if(!bounds_ok?!_tmp499: 0){struct _tuple6 _tmp49B=({struct _tuple6 _tmp49A;_tmp49A.f1=
Cyc_Absyn_conref_val(_tmp48B);_tmp49A.f2=Cyc_Absyn_conref_val(_tmp496);_tmp49A;});
void*_tmp49C;struct Cyc_Absyn_Exp*_tmp49D;void*_tmp49E;struct Cyc_Absyn_Exp*
_tmp49F;_LL388: _tmp49C=_tmp49B.f1;if(_tmp49C <= (void*)1?1:*((int*)_tmp49C)!= 0)
goto _LL38A;_tmp49D=((struct Cyc_Absyn_Upper_b_struct*)_tmp49C)->f1;_tmp49E=
_tmp49B.f2;if(_tmp49E <= (void*)1?1:*((int*)_tmp49E)!= 0)goto _LL38A;_tmp49F=((
struct Cyc_Absyn_Upper_b_struct*)_tmp49E)->f1;_LL389: if(Cyc_Evexp_lte_const_exp(
_tmp49F,_tmp49D))bounds_ok=1;goto _LL387;_LL38A:;_LL38B: bounds_ok=1;goto _LL387;
_LL387:;}return(bounds_ok?_tmp498?1: _tmp499: 0)?Cyc_Tcutil_unify(_tmp488,_tmp493)?
1: Cyc_Tcenv_region_outlives(te,_tmp488,_tmp493): 0;}_LL383: if((int)_tmp490 != 0)
goto _LL385;_LL384: return 1;_LL385:;_LL386: goto _LL380;_LL380:;}return 0;_LL372: if(
_tmp485 <= (void*)3?1:*((int*)_tmp485)!= 7)goto _LL374;_tmp48C=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp485)->f1;_tmp48D=((struct Cyc_Absyn_ArrayType_struct*)_tmp485)->f2;_tmp48E=((
struct Cyc_Absyn_ArrayType_struct*)_tmp485)->f3;_LL373:{void*_tmp4A2=t2;void*
_tmp4A3;struct Cyc_Absyn_Tqual _tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;_LL38D: if(
_tmp4A2 <= (void*)3?1:*((int*)_tmp4A2)!= 7)goto _LL38F;_tmp4A3=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp4A2)->f1;_tmp4A4=((struct Cyc_Absyn_ArrayType_struct*)_tmp4A2)->f2;_tmp4A5=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4A2)->f3;_LL38E: {int okay;okay=(_tmp48E != 
0?_tmp4A5 != 0: 0)?Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp4A5),(struct Cyc_Absyn_Exp*)_check_null(_tmp48E)): 0;return(okay?Cyc_Tcutil_unify(
_tmp48C,_tmp4A3): 0)?!_tmp48D.q_const?1: _tmp4A4.q_const: 0;}_LL38F:;_LL390: return 0;
_LL38C:;}return 0;_LL374: if(_tmp485 <= (void*)3?1:*((int*)_tmp485)!= 12)goto _LL376;
_tmp48F=((struct Cyc_Absyn_EnumType_struct*)_tmp485)->f2;_LL375:{void*_tmp4A6=t2;
struct Cyc_Absyn_Enumdecl*_tmp4A7;_LL392: if(_tmp4A6 <= (void*)3?1:*((int*)_tmp4A6)
!= 12)goto _LL394;_tmp4A7=((struct Cyc_Absyn_EnumType_struct*)_tmp4A6)->f2;_LL393:
if((_tmp48F->fields != 0?_tmp4A7->fields != 0: 0)?((int(*)(struct Cyc_List_List*x))
Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp48F->fields))->v)
>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4A7->fields))->v): 0)return 1;goto _LL391;
_LL394:;_LL395: goto _LL391;_LL391:;}goto _LL377;_LL376: if(_tmp485 <= (void*)3?1:*((
int*)_tmp485)!= 5)goto _LL378;_LL377: goto _LL379;_LL378: if((int)_tmp485 != 1)goto
_LL37A;_LL379: goto _LL37B;_LL37A: if(_tmp485 <= (void*)3?1:*((int*)_tmp485)!= 6)
goto _LL37C;_LL37B: return Cyc_Tcutil_coerceable(t2)?1: t2 == (void*)0;_LL37C: if((int)
_tmp485 != 0)goto _LL37E;_LL37D: return t2 == (void*)0;_LL37E:;_LL37F: return 0;_LL36F:;}}
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*
t){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){
struct Cyc_Absyn_Exp*_tmp4A8=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Cast_e_struct*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9[0]=({
struct Cyc_Absyn_Cast_e_struct _tmp4AA;_tmp4AA.tag=13;_tmp4AA.f1=(void*)t;_tmp4AA.f2=
_tmp4A8;_tmp4AA;});_tmp4A9;})));e->topt=({struct Cyc_Core_Opt*_tmp4AB=_cycalloc(
sizeof(*_tmp4AB));_tmp4AB->v=(void*)t;_tmp4AB;});}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp4AC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL397: if(_tmp4AC <= (void*)3?1:*((int*)_tmp4AC)!= 5)
goto _LL399;_LL398: goto _LL39A;_LL399: if(_tmp4AC <= (void*)3?1:*((int*)_tmp4AC)!= 
12)goto _LL39B;_LL39A: goto _LL39C;_LL39B: if(_tmp4AC <= (void*)3?1:*((int*)_tmp4AC)
!= 13)goto _LL39D;_LL39C: goto _LL39E;_LL39D: if(_tmp4AC <= (void*)3?1:*((int*)
_tmp4AC)!= 17)goto _LL39F;_LL39E: goto _LL3A0;_LL39F: if(_tmp4AC <= (void*)3?1:*((int*)
_tmp4AC)!= 14)goto _LL3A1;_LL3A0: return 1;_LL3A1: if(_tmp4AC <= (void*)3?1:*((int*)
_tmp4AC)!= 0)goto _LL3A3;_LL3A2: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3A3:;_LL3A4: return 0;_LL396:;}int
Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return
1;{void*_tmp4AD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3A6: if((int)_tmp4AD != 1)goto _LL3A8;_LL3A7: goto _LL3A9;_LL3A8: if(_tmp4AD <= (
void*)3?1:*((int*)_tmp4AD)!= 6)goto _LL3AA;_LL3A9: return 1;_LL3AA:;_LL3AB: return 0;
_LL3A5:;}}int Cyc_Tcutil_is_function_type(void*t){void*_tmp4AE=Cyc_Tcutil_compress(
t);_LL3AD: if(_tmp4AE <= (void*)3?1:*((int*)_tmp4AE)!= 8)goto _LL3AF;_LL3AE: return 1;
_LL3AF:;_LL3B0: return 0;_LL3AC:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*
t2){struct _tuple6 _tmp4B0=({struct _tuple6 _tmp4AF;_tmp4AF.f1=t1;_tmp4AF.f2=t2;
_tmp4AF;});void*_tmp4B1;int _tmp4B2;void*_tmp4B3;int _tmp4B4;void*_tmp4B5;void*
_tmp4B6;void*_tmp4B7;void*_tmp4B8;void*_tmp4B9;void*_tmp4BA;void*_tmp4BB;void*
_tmp4BC;void*_tmp4BD;void*_tmp4BE;void*_tmp4BF;void*_tmp4C0;void*_tmp4C1;void*
_tmp4C2;void*_tmp4C3;void*_tmp4C4;void*_tmp4C5;void*_tmp4C6;void*_tmp4C7;void*
_tmp4C8;void*_tmp4C9;void*_tmp4CA;void*_tmp4CB;void*_tmp4CC;void*_tmp4CD;void*
_tmp4CE;void*_tmp4CF;void*_tmp4D0;_LL3B2: _tmp4B1=_tmp4B0.f1;if(_tmp4B1 <= (void*)
3?1:*((int*)_tmp4B1)!= 6)goto _LL3B4;_tmp4B2=((struct Cyc_Absyn_DoubleType_struct*)
_tmp4B1)->f1;_tmp4B3=_tmp4B0.f2;if(_tmp4B3 <= (void*)3?1:*((int*)_tmp4B3)!= 6)
goto _LL3B4;_tmp4B4=((struct Cyc_Absyn_DoubleType_struct*)_tmp4B3)->f1;_LL3B3: if(
_tmp4B2)return t1;else{return t2;}_LL3B4: _tmp4B5=_tmp4B0.f1;if(_tmp4B5 <= (void*)3?
1:*((int*)_tmp4B5)!= 6)goto _LL3B6;_LL3B5: return t1;_LL3B6: _tmp4B6=_tmp4B0.f2;if(
_tmp4B6 <= (void*)3?1:*((int*)_tmp4B6)!= 6)goto _LL3B8;_LL3B7: return t2;_LL3B8:
_tmp4B7=_tmp4B0.f1;if((int)_tmp4B7 != 1)goto _LL3BA;_LL3B9: goto _LL3BB;_LL3BA:
_tmp4B8=_tmp4B0.f2;if((int)_tmp4B8 != 1)goto _LL3BC;_LL3BB: return(void*)1;_LL3BC:
_tmp4B9=_tmp4B0.f1;if(_tmp4B9 <= (void*)3?1:*((int*)_tmp4B9)!= 5)goto _LL3BE;
_tmp4BA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B9)->f1;if((int)_tmp4BA != 
1)goto _LL3BE;_tmp4BB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B9)->f2;if((
int)_tmp4BB != 3)goto _LL3BE;_LL3BD: goto _LL3BF;_LL3BE: _tmp4BC=_tmp4B0.f2;if(
_tmp4BC <= (void*)3?1:*((int*)_tmp4BC)!= 5)goto _LL3C0;_tmp4BD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4BC)->f1;if((int)_tmp4BD != 1)goto _LL3C0;_tmp4BE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4BC)->f2;if((int)_tmp4BE != 3)goto _LL3C0;_LL3BF: return Cyc_Absyn_ulonglong_typ;
_LL3C0: _tmp4BF=_tmp4B0.f1;if(_tmp4BF <= (void*)3?1:*((int*)_tmp4BF)!= 5)goto
_LL3C2;_tmp4C0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4BF)->f1;if((int)
_tmp4C0 != 2)goto _LL3C2;_tmp4C1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4BF)->f2;
if((int)_tmp4C1 != 3)goto _LL3C2;_LL3C1: goto _LL3C3;_LL3C2: _tmp4C2=_tmp4B0.f2;if(
_tmp4C2 <= (void*)3?1:*((int*)_tmp4C2)!= 5)goto _LL3C4;_tmp4C3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C2)->f1;if((int)_tmp4C3 != 2)goto _LL3C4;_tmp4C4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C2)->f2;if((int)_tmp4C4 != 3)goto _LL3C4;_LL3C3: goto _LL3C5;_LL3C4: _tmp4C5=
_tmp4B0.f1;if(_tmp4C5 <= (void*)3?1:*((int*)_tmp4C5)!= 5)goto _LL3C6;_tmp4C6=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4C5)->f1;if((int)_tmp4C6 != 0)goto _LL3C6;
_tmp4C7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C5)->f2;if((int)_tmp4C7 != 
3)goto _LL3C6;_LL3C5: goto _LL3C7;_LL3C6: _tmp4C8=_tmp4B0.f2;if(_tmp4C8 <= (void*)3?1:*((
int*)_tmp4C8)!= 5)goto _LL3C8;_tmp4C9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C8)->f1;if((int)_tmp4C9 != 0)goto _LL3C8;_tmp4CA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C8)->f2;if((int)_tmp4CA != 3)goto _LL3C8;_LL3C7: return Cyc_Absyn_slonglong_typ;
_LL3C8: _tmp4CB=_tmp4B0.f1;if(_tmp4CB <= (void*)3?1:*((int*)_tmp4CB)!= 5)goto
_LL3CA;_tmp4CC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4CB)->f1;if((int)
_tmp4CC != 1)goto _LL3CA;_tmp4CD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4CB)->f2;
if((int)_tmp4CD != 2)goto _LL3CA;_LL3C9: goto _LL3CB;_LL3CA: _tmp4CE=_tmp4B0.f2;if(
_tmp4CE <= (void*)3?1:*((int*)_tmp4CE)!= 5)goto _LL3CC;_tmp4CF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4CE)->f1;if((int)_tmp4CF != 1)goto _LL3CC;_tmp4D0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4CE)->f2;if((int)_tmp4D0 != 2)goto _LL3CC;_LL3CB: return Cyc_Absyn_uint_typ;
_LL3CC:;_LL3CD: return Cyc_Absyn_sint_typ;_LL3B1:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp4D1=(void*)e->r;struct Cyc_Core_Opt*_tmp4D2;
_LL3CF: if(*((int*)_tmp4D1)!= 4)goto _LL3D1;_tmp4D2=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4D1)->f2;if(_tmp4D2 != 0)goto _LL3D1;_LL3D0:({void*_tmp4D3[0]={};Cyc_Tcutil_warn(
e->loc,_tag_arr("assignment in test",sizeof(char),19),_tag_arr(_tmp4D3,sizeof(
void*),0));});goto _LL3CE;_LL3D1:;_LL3D2: goto _LL3CE;_LL3CE:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp4D5=({struct _tuple6 _tmp4D4;_tmp4D4.f1=c1;_tmp4D4.f2=c2;_tmp4D4;});
void*_tmp4D6;void*_tmp4D7;void*_tmp4D8;void*_tmp4D9;void*_tmp4DA;struct Cyc_Core_Opt*
_tmp4DB;struct Cyc_Core_Opt**_tmp4DC;void*_tmp4DD;struct Cyc_Core_Opt*_tmp4DE;
struct Cyc_Core_Opt**_tmp4DF;void*_tmp4E0;struct Cyc_Core_Opt*_tmp4E1;struct Cyc_Core_Opt**
_tmp4E2;void*_tmp4E3;void*_tmp4E4;void*_tmp4E5;void*_tmp4E6;void*_tmp4E7;void*
_tmp4E8;struct Cyc_Core_Opt*_tmp4E9;struct Cyc_Core_Opt**_tmp4EA;void*_tmp4EB;void*
_tmp4EC;struct Cyc_Core_Opt*_tmp4ED;struct Cyc_Core_Opt**_tmp4EE;void*_tmp4EF;void*
_tmp4F0;struct Cyc_Core_Opt*_tmp4F1;struct Cyc_Core_Opt**_tmp4F2;void*_tmp4F3;
_LL3D4: _tmp4D6=_tmp4D5.f1;if(*((int*)_tmp4D6)!= 0)goto _LL3D6;_tmp4D7=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp4D6)->f1;_tmp4D8=_tmp4D5.f2;if(*((int*)_tmp4D8)
!= 0)goto _LL3D6;_tmp4D9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4D8)->f1;
_LL3D5: return _tmp4D7 == _tmp4D9;_LL3D6: _tmp4DA=_tmp4D5.f2;if(*((int*)_tmp4DA)!= 1)
goto _LL3D8;_tmp4DB=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4DA)->f1;_tmp4DC=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4DA)->f1;_LL3D7:*
_tmp4DC=({struct Cyc_Core_Opt*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->v=(
void*)c1;_tmp4F4;});return 1;_LL3D8: _tmp4DD=_tmp4D5.f1;if(*((int*)_tmp4DD)!= 1)
goto _LL3DA;_tmp4DE=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4DD)->f1;_tmp4DF=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4DD)->f1;_LL3D9:*
_tmp4DF=({struct Cyc_Core_Opt*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->v=(
void*)c2;_tmp4F5;});return 1;_LL3DA: _tmp4E0=_tmp4D5.f1;if(*((int*)_tmp4E0)!= 2)
goto _LL3DC;_tmp4E1=((struct Cyc_Absyn_Less_kb_struct*)_tmp4E0)->f1;_tmp4E2=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp4E0)->f1;_tmp4E3=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4E0)->f2;_tmp4E4=_tmp4D5.f2;if(*((
int*)_tmp4E4)!= 0)goto _LL3DC;_tmp4E5=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp4E4)->f1;_LL3DB: if(Cyc_Tcutil_kind_leq(_tmp4E5,_tmp4E3)){*_tmp4E2=({struct
Cyc_Core_Opt*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->v=(void*)c2;_tmp4F6;});
return 1;}else{return 0;}_LL3DC: _tmp4E6=_tmp4D5.f1;if(*((int*)_tmp4E6)!= 0)goto
_LL3DE;_tmp4E7=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4E6)->f1;_tmp4E8=
_tmp4D5.f2;if(*((int*)_tmp4E8)!= 2)goto _LL3DE;_tmp4E9=((struct Cyc_Absyn_Less_kb_struct*)
_tmp4E8)->f1;_tmp4EA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp4E8)->f1;_tmp4EB=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4E8)->f2;
_LL3DD: if(Cyc_Tcutil_kind_leq(_tmp4E7,_tmp4EB)){*_tmp4EA=({struct Cyc_Core_Opt*
_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->v=(void*)c1;_tmp4F7;});return 1;}
else{return 0;}_LL3DE: _tmp4EC=_tmp4D5.f1;if(*((int*)_tmp4EC)!= 2)goto _LL3D3;
_tmp4ED=((struct Cyc_Absyn_Less_kb_struct*)_tmp4EC)->f1;_tmp4EE=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4EC)->f1;_tmp4EF=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4EC)->f2;_tmp4F0=_tmp4D5.f2;if(*((int*)_tmp4F0)!= 2)goto _LL3D3;_tmp4F1=((
struct Cyc_Absyn_Less_kb_struct*)_tmp4F0)->f1;_tmp4F2=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4F0)->f1;_tmp4F3=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4F0)->f2;_LL3DF: if(Cyc_Tcutil_kind_leq(_tmp4EF,_tmp4F3)){*_tmp4F2=({struct
Cyc_Core_Opt*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->v=(void*)c1;_tmp4F8;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp4F3,_tmp4EF)){*_tmp4EE=({struct Cyc_Core_Opt*
_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->v=(void*)c2;_tmp4F9;});return 1;}
else{return 0;}}_LL3D3:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp4FA=_cycalloc_atomic(sizeof(*_tmp4FA));_tmp4FA[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp4FA;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp4FE;_tmp4FE.tag=1;_tmp4FE.f1=(int)((unsigned int)
i);{void*_tmp4FD[1]={& _tmp4FE};Cyc_Std_aprintf(_tag_arr("#%d",sizeof(char),4),
_tag_arr(_tmp4FD,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*_tmp4FB=
_cycalloc(sizeof(*_tmp4FB));_tmp4FB->name=({struct _tagged_arr*_tmp4FC=_cycalloc(
sizeof(struct _tagged_arr)* 1);_tmp4FC[0]=s;_tmp4FC;});_tmp4FB->identity=0;
_tmp4FB->kind=(void*)k;_tmp4FB;});}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _tagged_arr _tmp4FF=*t->name;return*((const char*)
_check_unknown_subscript(_tmp4FF,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_Std_String_pa_struct _tmp501;_tmp501.tag=0;
_tmp501.f1=(struct _tagged_arr)*t->name;{void*_tmp500[1]={& _tmp501};Cyc_Std_printf(
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp500,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp502=Cyc_Std_strconcat(_tag_arr("`",sizeof(char),
2),*t->name);*((char*)_check_unknown_subscript(_tmp502,sizeof(char),1))='t';t->name=({
struct _tagged_arr*_tmp503=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp503[0]=(
struct _tagged_arr)_tmp502;_tmp503;});}}struct _tuple13{struct _tagged_arr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple13*x){return({struct _tuple2*_tmp504=_cycalloc(sizeof(*_tmp504));
_tmp504->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp505=_cycalloc(sizeof(*
_tmp505));_tmp505->v=(*x).f1;_tmp505;});_tmp504->f2=(*x).f2;_tmp504->f3=(*x).f3;
_tmp504;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp506=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp506=({
struct Cyc_List_List*_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507->hd=(void*)((
void*)atts->hd);_tmp507->tl=_tmp506;_tmp507;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508[0]=({struct Cyc_Absyn_FnType_struct
_tmp509;_tmp509.tag=8;_tmp509.f1=({struct Cyc_Absyn_FnInfo _tmp50A;_tmp50A.tvars=
fd->tvs;_tmp50A.effect=fd->effect;_tmp50A.ret_typ=(void*)((void*)fd->ret_type);
_tmp50A.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp50A.c_varargs=
fd->c_varargs;_tmp50A.cyc_varargs=fd->cyc_varargs;_tmp50A.rgn_po=fd->rgn_po;
_tmp50A.attributes=_tmp506;_tmp50A;});_tmp509;});_tmp508;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple14{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple14*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->f1=(*pr).f1;_tmp50B->f2=t;_tmp50B;});}
struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple16{
struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp50C=_region_malloc(
rgn,sizeof(*_tmp50C));_tmp50C->f1=({struct _tuple15*_tmp50D=_region_malloc(rgn,
sizeof(*_tmp50D));_tmp50D->f1=(*y).f1;_tmp50D->f2=(*y).f2;_tmp50D;});_tmp50C->f2=(*
y).f3;_tmp50C;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple16*w){
struct _tuple15*_tmp50F;void*_tmp510;struct _tuple16 _tmp50E=*w;_tmp50F=_tmp50E.f1;
_tmp510=_tmp50E.f2;{struct Cyc_Core_Opt*_tmp512;struct Cyc_Absyn_Tqual _tmp513;
struct _tuple15 _tmp511=*_tmp50F;_tmp512=_tmp511.f1;_tmp513=_tmp511.f2;return({
struct _tuple2*_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514->f1=_tmp512;_tmp514->f2=
_tmp513;_tmp514->f3=_tmp510;_tmp514;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp515=
_cycalloc(sizeof(*_tmp515));_tmp515->name=f->name;_tmp515->tq=f->tq;_tmp515->type=(
void*)t;_tmp515->width=f->width;_tmp515->attributes=f->attributes;_tmp515;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp516=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp517;struct Cyc_Absyn_AggrInfo _tmp518;void*_tmp519;struct Cyc_List_List*_tmp51A;
struct Cyc_Absyn_TunionInfo _tmp51B;void*_tmp51C;struct Cyc_List_List*_tmp51D;void*
_tmp51E;struct Cyc_Absyn_TunionFieldInfo _tmp51F;void*_tmp520;struct Cyc_List_List*
_tmp521;struct _tuple1*_tmp522;struct Cyc_List_List*_tmp523;struct Cyc_Absyn_Typedefdecl*
_tmp524;struct Cyc_Core_Opt*_tmp525;void*_tmp526;struct Cyc_Absyn_Tqual _tmp527;
struct Cyc_Absyn_Exp*_tmp528;struct Cyc_Absyn_PtrInfo _tmp529;void*_tmp52A;void*
_tmp52B;struct Cyc_Absyn_Conref*_tmp52C;struct Cyc_Absyn_Tqual _tmp52D;struct Cyc_Absyn_Conref*
_tmp52E;struct Cyc_Absyn_FnInfo _tmp52F;struct Cyc_List_List*_tmp530;struct Cyc_Core_Opt*
_tmp531;void*_tmp532;struct Cyc_List_List*_tmp533;int _tmp534;struct Cyc_Absyn_VarargInfo*
_tmp535;struct Cyc_List_List*_tmp536;struct Cyc_List_List*_tmp537;struct Cyc_List_List*
_tmp538;void*_tmp539;struct Cyc_List_List*_tmp53A;struct Cyc_Core_Opt*_tmp53B;void*
_tmp53C;void*_tmp53D;void*_tmp53E;void*_tmp53F;void*_tmp540;struct Cyc_List_List*
_tmp541;_LL3E1: if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 1)goto _LL3E3;_tmp517=((
struct Cyc_Absyn_VarType_struct*)_tmp516)->f1;_LL3E2: {struct _handler_cons _tmp542;
_push_handler(& _tmp542);{int _tmp544=0;if(setjmp(_tmp542.handler))_tmp544=1;if(!
_tmp544){{void*_tmp545=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp517);_npop_handler(0);return _tmp545;};_pop_handler();}else{void*_tmp543=(
void*)_exn_thrown;void*_tmp547=_tmp543;_LL414: if(_tmp547 != Cyc_Core_Not_found)
goto _LL416;_LL415: return t;_LL416:;_LL417:(void)_throw(_tmp547);_LL413:;}}}_LL3E3:
if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 10)goto _LL3E5;_tmp518=((struct Cyc_Absyn_AggrType_struct*)
_tmp516)->f1;_tmp519=(void*)_tmp518.aggr_info;_tmp51A=_tmp518.targs;_LL3E4: {
struct Cyc_List_List*_tmp548=Cyc_Tcutil_substs(rgn,inst,_tmp51A);return _tmp548 == 
_tmp51A?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp549=_cycalloc(sizeof(*
_tmp549));_tmp549[0]=({struct Cyc_Absyn_AggrType_struct _tmp54A;_tmp54A.tag=10;
_tmp54A.f1=({struct Cyc_Absyn_AggrInfo _tmp54B;_tmp54B.aggr_info=(void*)_tmp519;
_tmp54B.targs=_tmp548;_tmp54B;});_tmp54A;});_tmp549;});}_LL3E5: if(_tmp516 <= (
void*)3?1:*((int*)_tmp516)!= 2)goto _LL3E7;_tmp51B=((struct Cyc_Absyn_TunionType_struct*)
_tmp516)->f1;_tmp51C=(void*)_tmp51B.tunion_info;_tmp51D=_tmp51B.targs;_tmp51E=(
void*)_tmp51B.rgn;_LL3E6: {struct Cyc_List_List*_tmp54C=Cyc_Tcutil_substs(rgn,
inst,_tmp51D);void*_tmp54D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp51E);return(
_tmp54C == _tmp51D?_tmp54D == _tmp51E: 0)?t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E[0]=({struct Cyc_Absyn_TunionType_struct
_tmp54F;_tmp54F.tag=2;_tmp54F.f1=({struct Cyc_Absyn_TunionInfo _tmp550;_tmp550.tunion_info=(
void*)_tmp51C;_tmp550.targs=_tmp54C;_tmp550.rgn=(void*)_tmp54D;_tmp550;});
_tmp54F;});_tmp54E;});}_LL3E7: if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 3)goto
_LL3E9;_tmp51F=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp516)->f1;_tmp520=(
void*)_tmp51F.field_info;_tmp521=_tmp51F.targs;_LL3E8: {struct Cyc_List_List*
_tmp551=Cyc_Tcutil_substs(rgn,inst,_tmp521);return _tmp551 == _tmp521?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp552=_cycalloc(sizeof(*_tmp552));
_tmp552[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp553;_tmp553.tag=3;_tmp553.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp554;_tmp554.field_info=(void*)_tmp520;_tmp554.targs=
_tmp551;_tmp554;});_tmp553;});_tmp552;});}_LL3E9: if(_tmp516 <= (void*)3?1:*((int*)
_tmp516)!= 16)goto _LL3EB;_tmp522=((struct Cyc_Absyn_TypedefType_struct*)_tmp516)->f1;
_tmp523=((struct Cyc_Absyn_TypedefType_struct*)_tmp516)->f2;_tmp524=((struct Cyc_Absyn_TypedefType_struct*)
_tmp516)->f3;_tmp525=((struct Cyc_Absyn_TypedefType_struct*)_tmp516)->f4;_LL3EA: {
struct Cyc_List_List*_tmp555=Cyc_Tcutil_substs(rgn,inst,_tmp523);return _tmp555 == 
_tmp523?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp556=_cycalloc(sizeof(*
_tmp556));_tmp556[0]=({struct Cyc_Absyn_TypedefType_struct _tmp557;_tmp557.tag=16;
_tmp557.f1=_tmp522;_tmp557.f2=_tmp555;_tmp557.f3=_tmp524;_tmp557.f4=_tmp525;
_tmp557;});_tmp556;});}_LL3EB: if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 7)goto
_LL3ED;_tmp526=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp516)->f1;_tmp527=((
struct Cyc_Absyn_ArrayType_struct*)_tmp516)->f2;_tmp528=((struct Cyc_Absyn_ArrayType_struct*)
_tmp516)->f3;_LL3EC: {void*_tmp558=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp526);
return _tmp558 == _tmp526?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp559=
_cycalloc(sizeof(*_tmp559));_tmp559[0]=({struct Cyc_Absyn_ArrayType_struct _tmp55A;
_tmp55A.tag=7;_tmp55A.f1=(void*)_tmp558;_tmp55A.f2=_tmp527;_tmp55A.f3=_tmp528;
_tmp55A;});_tmp559;});}_LL3ED: if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 4)goto
_LL3EF;_tmp529=((struct Cyc_Absyn_PointerType_struct*)_tmp516)->f1;_tmp52A=(void*)
_tmp529.elt_typ;_tmp52B=(void*)_tmp529.rgn_typ;_tmp52C=_tmp529.nullable;_tmp52D=
_tmp529.tq;_tmp52E=_tmp529.bounds;_LL3EE: {void*_tmp55B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp52A);void*_tmp55C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp52B);if(
_tmp55B == _tmp52A?_tmp55C == _tmp52B: 0)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D[0]=({struct Cyc_Absyn_PointerType_struct
_tmp55E;_tmp55E.tag=4;_tmp55E.f1=({struct Cyc_Absyn_PtrInfo _tmp55F;_tmp55F.elt_typ=(
void*)_tmp55B;_tmp55F.rgn_typ=(void*)_tmp55C;_tmp55F.nullable=_tmp52C;_tmp55F.tq=
_tmp52D;_tmp55F.bounds=_tmp52E;_tmp55F;});_tmp55E;});_tmp55D;});}_LL3EF: if(
_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 8)goto _LL3F1;_tmp52F=((struct Cyc_Absyn_FnType_struct*)
_tmp516)->f1;_tmp530=_tmp52F.tvars;_tmp531=_tmp52F.effect;_tmp532=(void*)_tmp52F.ret_typ;
_tmp533=_tmp52F.args;_tmp534=_tmp52F.c_varargs;_tmp535=_tmp52F.cyc_varargs;
_tmp536=_tmp52F.rgn_po;_tmp537=_tmp52F.attributes;_LL3F0:{struct Cyc_List_List*
_tmp560=_tmp530;for(0;_tmp560 != 0;_tmp560=_tmp560->tl){inst=({struct Cyc_List_List*
_tmp561=_region_malloc(rgn,sizeof(*_tmp561));_tmp561->hd=({struct _tuple8*_tmp562=
_region_malloc(rgn,sizeof(*_tmp562));_tmp562->f1=(struct Cyc_Absyn_Tvar*)_tmp560->hd;
_tmp562->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp563=_cycalloc(sizeof(*
_tmp563));_tmp563[0]=({struct Cyc_Absyn_VarType_struct _tmp564;_tmp564.tag=1;
_tmp564.f1=(struct Cyc_Absyn_Tvar*)_tmp560->hd;_tmp564;});_tmp563;});_tmp562;});
_tmp561->tl=inst;_tmp561;});}}{struct Cyc_List_List*_tmp566;struct Cyc_List_List*
_tmp567;struct _tuple0 _tmp565=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple16*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp533));_tmp566=_tmp565.f1;_tmp567=_tmp565.f2;{struct Cyc_List_List*_tmp568=
Cyc_Tcutil_substs(rgn,inst,_tmp567);struct Cyc_List_List*_tmp569=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp566,_tmp568));struct Cyc_Core_Opt*
eff2;if(_tmp531 == 0)eff2=0;else{void*_tmp56A=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp531->v);if(_tmp56A == (void*)_tmp531->v)eff2=_tmp531;else{eff2=({struct
Cyc_Core_Opt*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->v=(void*)_tmp56A;
_tmp56B;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp535 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp56D;struct Cyc_Absyn_Tqual _tmp56E;void*
_tmp56F;int _tmp570;struct Cyc_Absyn_VarargInfo _tmp56C=*_tmp535;_tmp56D=_tmp56C.name;
_tmp56E=_tmp56C.tq;_tmp56F=(void*)_tmp56C.type;_tmp570=_tmp56C.inject;{void*
_tmp571=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp56F);if(_tmp571 == _tmp56F)
cyc_varargs2=_tmp535;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp572=
_cycalloc(sizeof(*_tmp572));_tmp572->name=_tmp56D;_tmp572->tq=_tmp56E;_tmp572->type=(
void*)_tmp571;_tmp572->inject=_tmp570;_tmp572;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp574;struct Cyc_List_List*_tmp575;struct _tuple0 _tmp573=Cyc_List_rsplit(
rgn,rgn,_tmp536);_tmp574=_tmp573.f1;_tmp575=_tmp573.f2;{struct Cyc_List_List*
_tmp576=Cyc_Tcutil_substs(rgn,inst,_tmp574);struct Cyc_List_List*_tmp577=Cyc_Tcutil_substs(
rgn,inst,_tmp575);if(_tmp576 == _tmp574?_tmp577 == _tmp575: 0)rgn_po2=_tmp536;else{
rgn_po2=Cyc_List_zip(_tmp576,_tmp577);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578[0]=({struct Cyc_Absyn_FnType_struct
_tmp579;_tmp579.tag=8;_tmp579.f1=({struct Cyc_Absyn_FnInfo _tmp57A;_tmp57A.tvars=
_tmp530;_tmp57A.effect=eff2;_tmp57A.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp532);_tmp57A.args=_tmp569;_tmp57A.c_varargs=_tmp534;_tmp57A.cyc_varargs=
cyc_varargs2;_tmp57A.rgn_po=rgn_po2;_tmp57A.attributes=_tmp537;_tmp57A;});
_tmp579;});_tmp578;});}}}}}_LL3F1: if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 9)
goto _LL3F3;_tmp538=((struct Cyc_Absyn_TupleType_struct*)_tmp516)->f1;_LL3F2: {
struct Cyc_List_List*_tmp57B=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp538);struct Cyc_List_List*_tmp57C=Cyc_Tcutil_substs(rgn,inst,_tmp57B);if(
_tmp57C == _tmp57B)return t;{struct Cyc_List_List*_tmp57D=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp538,_tmp57C);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E[0]=({struct Cyc_Absyn_TupleType_struct
_tmp57F;_tmp57F.tag=9;_tmp57F.f1=_tmp57D;_tmp57F;});_tmp57E;});}}_LL3F3: if(
_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 11)goto _LL3F5;_tmp539=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp516)->f1;_tmp53A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp516)->f2;_LL3F4: {
struct Cyc_List_List*_tmp580=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp53A);struct Cyc_List_List*_tmp581=Cyc_Tcutil_substs(rgn,inst,_tmp580);if(
_tmp581 == _tmp580)return t;{struct Cyc_List_List*_tmp582=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp53A,
_tmp581);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp583=_cycalloc(
sizeof(*_tmp583));_tmp583[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp584;
_tmp584.tag=11;_tmp584.f1=(void*)_tmp539;_tmp584.f2=_tmp582;_tmp584;});_tmp583;});}}
_LL3F5: if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 0)goto _LL3F7;_tmp53B=((struct
Cyc_Absyn_Evar_struct*)_tmp516)->f2;_LL3F6: if(_tmp53B != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp53B->v);else{return t;}_LL3F7: if(_tmp516 <= (void*)3?1:*((int*)
_tmp516)!= 15)goto _LL3F9;_tmp53C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp516)->f1;_LL3F8: {void*_tmp585=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp53C);
return _tmp585 == _tmp53C?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp586=
_cycalloc(sizeof(*_tmp586));_tmp586[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp587;_tmp587.tag=15;_tmp587.f1=(void*)_tmp585;_tmp587;});_tmp586;});}_LL3F9:
if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 14)goto _LL3FB;_tmp53D=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp516)->f1;_LL3FA: {void*_tmp588=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp53D);return _tmp588 == _tmp53D?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp58A;_tmp58A.tag=14;_tmp58A.f1=(void*)_tmp588;_tmp58A;});_tmp589;});}_LL3FB:
if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 17)goto _LL3FD;_tmp53E=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp516)->f1;_LL3FC: {void*_tmp58B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp53E);return _tmp58B == _tmp53E?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C[0]=({struct Cyc_Absyn_TagType_struct
_tmp58D;_tmp58D.tag=17;_tmp58D.f1=(void*)_tmp58B;_tmp58D;});_tmp58C;});}_LL3FD:
if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 18)goto _LL3FF;_LL3FE: goto _LL400;
_LL3FF: if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 12)goto _LL401;_LL400: goto
_LL402;_LL401: if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 13)goto _LL403;_LL402:
goto _LL404;_LL403: if((int)_tmp516 != 0)goto _LL405;_LL404: goto _LL406;_LL405: if(
_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 5)goto _LL407;_LL406: goto _LL408;_LL407:
if((int)_tmp516 != 1)goto _LL409;_LL408: goto _LL40A;_LL409: if(_tmp516 <= (void*)3?1:*((
int*)_tmp516)!= 6)goto _LL40B;_LL40A: goto _LL40C;_LL40B: if((int)_tmp516 != 2)goto
_LL40D;_LL40C: return t;_LL40D: if(_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 21)goto
_LL40F;_tmp53F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp516)->f1;_LL40E: {
void*_tmp58E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp53F);return _tmp58E == _tmp53F?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp58F=_cycalloc(sizeof(*_tmp58F));
_tmp58F[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp590;_tmp590.tag=21;_tmp590.f1=(
void*)_tmp58E;_tmp590;});_tmp58F;});}_LL40F: if(_tmp516 <= (void*)3?1:*((int*)
_tmp516)!= 19)goto _LL411;_tmp540=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp516)->f1;_LL410: {void*_tmp591=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp540);
return _tmp591 == _tmp540?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp592=
_cycalloc(sizeof(*_tmp592));_tmp592[0]=({struct Cyc_Absyn_AccessEff_struct _tmp593;
_tmp593.tag=19;_tmp593.f1=(void*)_tmp591;_tmp593;});_tmp592;});}_LL411: if(
_tmp516 <= (void*)3?1:*((int*)_tmp516)!= 20)goto _LL3E0;_tmp541=((struct Cyc_Absyn_JoinEff_struct*)
_tmp516)->f1;_LL412: {struct Cyc_List_List*_tmp594=Cyc_Tcutil_substs(rgn,inst,
_tmp541);return _tmp594 == _tmp541?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp596;_tmp596.tag=20;_tmp596.f1=_tmp594;_tmp596;});_tmp595;});}_LL3E0:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp597=(void*)ts->hd;
struct Cyc_List_List*_tmp598=ts->tl;void*_tmp599=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp597);struct Cyc_List_List*_tmp59A=Cyc_Tcutil_substs(rgn,inst,_tmp598);if(
_tmp597 == _tmp599?_tmp598 == _tmp59A: 0)return ts;return(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->hd=(void*)
_tmp599;_tmp59B->tl=_tmp59A;_tmp59B;});}}extern void*Cyc_Tcutil_substitute(struct
Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp59C=_cycalloc(
sizeof(*_tmp59C));_tmp59C->f1=tv;_tmp59C->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->v=(void*)k;_tmp59D;}),({struct Cyc_Core_Opt*
_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E->v=s;_tmp59E;}));_tmp59C;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp5A0;struct Cyc_List_List*_tmp5A1;struct _RegionHandle*_tmp5A2;
struct _tuple9*_tmp59F=env;_tmp5A0=*_tmp59F;_tmp5A1=_tmp5A0.f1;_tmp5A2=_tmp5A0.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp5A3=_region_malloc(
_tmp5A2,sizeof(*_tmp5A3));_tmp5A3->f1=tv;_tmp5A3->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->v=(void*)k;_tmp5A4;}),({
struct Cyc_Core_Opt*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->v=_tmp5A1;
_tmp5A5;}));_tmp5A3;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_Std_zstrptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_Std_String_pa_struct _tmp5A9;_tmp5A9.tag=0;_tmp5A9.f1=(struct
_tagged_arr)Cyc_Absynpp_kindbound2string(k2);{struct Cyc_Std_String_pa_struct
_tmp5A8;_tmp5A8.tag=0;_tmp5A8.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k1);{struct Cyc_Std_String_pa_struct _tmp5A7;_tmp5A7.tag=0;_tmp5A7.f1=(struct
_tagged_arr)*tv->name;{void*_tmp5A6[3]={& _tmp5A7,& _tmp5A8,& _tmp5A9};Cyc_Tcutil_terr(
loc,_tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof(char),59),_tag_arr(_tmp5A6,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp5AA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("same type variable has different identity!",sizeof(char),43),_tag_arr(
_tmp5AA,sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->hd=tv;
_tmp5AB->tl=tvs;_tmp5AB;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*
_tmp5AC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv",sizeof(char),39),_tag_arr(
_tmp5AC,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp5AD[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof(char),41),_tag_arr(_tmp5AD,sizeof(void*),0));});if(*((int*)_check_null(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)_check_null(tv->identity)))
return tvs;}}return({struct Cyc_List_List*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));
_tmp5AE->hd=tv;_tmp5AE->tl=tvs;_tmp5AE;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_Std_String_pa_struct
_tmp5B0;_tmp5B0.tag=0;_tmp5B0.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string(tv);{
void*_tmp5AF[1]={& _tmp5B0};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bound tvar id for %s is NULL",sizeof(char),29),
_tag_arr(_tmp5AF,sizeof(void*),1));}});return({struct Cyc_List_List*_tmp5B1=
_cycalloc(sizeof(*_tmp5B1));_tmp5B1->hd=tv;_tmp5B1->tl=tvs;_tmp5B1;});}static
struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){void*
_tmp5B2=Cyc_Tcutil_compress(e);int _tmp5B3;_LL419: if(_tmp5B2 <= (void*)3?1:*((int*)
_tmp5B2)!= 0)goto _LL41B;_tmp5B3=((struct Cyc_Absyn_Evar_struct*)_tmp5B2)->f3;
_LL41A:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*_tmp5B4=Cyc_Tcutil_compress((
void*)es2->hd);int _tmp5B5;_LL41E: if(_tmp5B4 <= (void*)3?1:*((int*)_tmp5B4)!= 0)
goto _LL420;_tmp5B5=((struct Cyc_Absyn_Evar_struct*)_tmp5B4)->f3;_LL41F: if(_tmp5B3
== _tmp5B5)return es;goto _LL41D;_LL420:;_LL421: goto _LL41D;_LL41D:;}}return({
struct Cyc_List_List*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->hd=(void*)e;
_tmp5B6->tl=es;_tmp5B6;});_LL41B:;_LL41C: return es;_LL418:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))
== *((int*)_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;
break;}}}if(!present)r=({struct Cyc_List_List*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));
_tmp5B7->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp5B7->tl=r;_tmp5B7;});}r=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(width)))({struct Cyc_Std_String_pa_struct
_tmp5B9;_tmp5B9.tag=0;_tmp5B9.f1=(struct _tagged_arr)*fn;{void*_tmp5B8[1]={&
_tmp5B9};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s does not have constant width",
sizeof(char),41),_tag_arr(_tmp5B8,sizeof(void*),1));}});else{unsigned int _tmp5BB;
int _tmp5BC;struct _tuple7 _tmp5BA=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(width));_tmp5BB=_tmp5BA.f1;_tmp5BC=_tmp5BA.f2;if(!_tmp5BC)({void*
_tmp5BD[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield width cannot use sizeof or offsetof",
sizeof(char),45),_tag_arr(_tmp5BD,sizeof(void*),0));});w=_tmp5BB;}{void*_tmp5BE=
Cyc_Tcutil_compress(field_typ);void*_tmp5BF;_LL423: if(_tmp5BE <= (void*)3?1:*((
int*)_tmp5BE)!= 5)goto _LL425;_tmp5BF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5BE)->f2;_LL424:{void*_tmp5C0=_tmp5BF;_LL428: if((int)_tmp5C0 != 0)goto _LL42A;
_LL429: if(w > 8)({void*_tmp5C1[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(char),26),_tag_arr(_tmp5C1,sizeof(void*),0));});goto _LL427;_LL42A: if((int)
_tmp5C0 != 1)goto _LL42C;_LL42B: if(w > 16)({void*_tmp5C2[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp5C2,sizeof(
void*),0));});goto _LL427;_LL42C: if((int)_tmp5C0 != 2)goto _LL42E;_LL42D: if(w > 32)({
void*_tmp5C3[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(char),26),_tag_arr(_tmp5C3,sizeof(void*),0));});goto _LL427;_LL42E: if((int)
_tmp5C0 != 3)goto _LL427;_LL42F: if(w > 64)({void*_tmp5C4[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp5C4,sizeof(
void*),0));});goto _LL427;_LL427:;}goto _LL422;_LL425:;_LL426:({struct Cyc_Std_String_pa_struct
_tmp5C7;_tmp5C7.tag=0;_tmp5C7.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
field_typ);{struct Cyc_Std_String_pa_struct _tmp5C6;_tmp5C6.tag=0;_tmp5C6.f1=(
struct _tagged_arr)*fn;{void*_tmp5C5[2]={& _tmp5C6,& _tmp5C7};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield %s must have integral type but has type %s",sizeof(char),52),
_tag_arr(_tmp5C5,sizeof(void*),2));}}});goto _LL422;_LL422:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp5C8=(void*)atts->hd;_LL431: if((int)
_tmp5C8 != 5)goto _LL433;_LL432: continue;_LL433: if(_tmp5C8 <= (void*)16?1:*((int*)
_tmp5C8)!= 1)goto _LL435;_LL434: continue;_LL435:;_LL436:({struct Cyc_Std_String_pa_struct
_tmp5CB;_tmp5CB.tag=0;_tmp5CB.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_String_pa_struct
_tmp5CA;_tmp5CA.tag=0;_tmp5CA.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp5C9[2]={& _tmp5CA,& _tmp5CB};Cyc_Tcutil_terr(loc,
_tag_arr("bad attribute %s on member %s",sizeof(char),30),_tag_arr(_tmp5C9,
sizeof(void*),2));}}});_LL430:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t){{void*_tmp5CC=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp5CD;struct Cyc_Core_Opt**_tmp5CE;struct Cyc_Core_Opt*
_tmp5CF;struct Cyc_Core_Opt**_tmp5D0;struct Cyc_Absyn_Tvar*_tmp5D1;struct Cyc_List_List*
_tmp5D2;struct _tuple1*_tmp5D3;struct Cyc_Absyn_Enumdecl*_tmp5D4;struct Cyc_Absyn_Enumdecl**
_tmp5D5;struct Cyc_Absyn_TunionInfo _tmp5D6;void*_tmp5D7;void**_tmp5D8;struct Cyc_List_List*
_tmp5D9;struct Cyc_List_List**_tmp5DA;void*_tmp5DB;struct Cyc_Absyn_TunionFieldInfo
_tmp5DC;void*_tmp5DD;void**_tmp5DE;struct Cyc_List_List*_tmp5DF;struct Cyc_Absyn_PtrInfo
_tmp5E0;void*_tmp5E1;void*_tmp5E2;struct Cyc_Absyn_Conref*_tmp5E3;struct Cyc_Absyn_Tqual
_tmp5E4;struct Cyc_Absyn_Conref*_tmp5E5;void*_tmp5E6;void*_tmp5E7;void*_tmp5E8;
struct Cyc_Absyn_Tqual _tmp5E9;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_FnInfo
_tmp5EB;struct Cyc_List_List*_tmp5EC;struct Cyc_List_List**_tmp5ED;struct Cyc_Core_Opt*
_tmp5EE;struct Cyc_Core_Opt**_tmp5EF;void*_tmp5F0;struct Cyc_List_List*_tmp5F1;int
_tmp5F2;struct Cyc_Absyn_VarargInfo*_tmp5F3;struct Cyc_List_List*_tmp5F4;struct Cyc_List_List*
_tmp5F5;struct Cyc_List_List*_tmp5F6;void*_tmp5F7;struct Cyc_List_List*_tmp5F8;
struct Cyc_Absyn_AggrInfo _tmp5F9;void*_tmp5FA;void**_tmp5FB;struct Cyc_List_List*
_tmp5FC;struct Cyc_List_List**_tmp5FD;struct _tuple1*_tmp5FE;struct Cyc_List_List*
_tmp5FF;struct Cyc_List_List**_tmp600;struct Cyc_Absyn_Typedefdecl*_tmp601;struct
Cyc_Absyn_Typedefdecl**_tmp602;struct Cyc_Core_Opt*_tmp603;struct Cyc_Core_Opt**
_tmp604;void*_tmp605;void*_tmp606;void*_tmp607;struct Cyc_List_List*_tmp608;
_LL438: if((int)_tmp5CC != 0)goto _LL43A;_LL439: goto _LL437;_LL43A: if(_tmp5CC <= (
void*)3?1:*((int*)_tmp5CC)!= 0)goto _LL43C;_tmp5CD=((struct Cyc_Absyn_Evar_struct*)
_tmp5CC)->f1;_tmp5CE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp5CC)->f1;_tmp5CF=((struct Cyc_Absyn_Evar_struct*)_tmp5CC)->f2;_tmp5D0=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp5CC)->f2;_LL43B: if(*_tmp5CE == 
0)*_tmp5CE=({struct Cyc_Core_Opt*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->v=(
void*)expected_kind;_tmp609;});if((cvtenv.fn_result?cvtenv.generalize_evars: 0)?
expected_kind == (void*)3: 0)*_tmp5D0=({struct Cyc_Core_Opt*_tmp60A=_cycalloc(
sizeof(*_tmp60A));_tmp60A->v=(void*)((void*)2);_tmp60A;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp60B=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp610;_tmp610.tag=2;_tmp610.f1=0;_tmp610.f2=(void*)expected_kind;_tmp610;});
_tmp60F;}));*_tmp5D0=({struct Cyc_Core_Opt*_tmp60C=_cycalloc(sizeof(*_tmp60C));
_tmp60C->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp60D=_cycalloc(
sizeof(*_tmp60D));_tmp60D[0]=({struct Cyc_Absyn_VarType_struct _tmp60E;_tmp60E.tag=
1;_tmp60E.f1=_tmp60B;_tmp60E;});_tmp60D;}));_tmp60C;});_tmp5D1=_tmp60B;goto
_LL43D;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL437;_LL43C: if(_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 1)goto _LL43E;
_tmp5D1=((struct Cyc_Absyn_VarType_struct*)_tmp5CC)->f1;_LL43D:{void*_tmp611=Cyc_Absyn_compress_kb((
void*)_tmp5D1->kind);struct Cyc_Core_Opt*_tmp612;struct Cyc_Core_Opt**_tmp613;
_LL46B: if(*((int*)_tmp611)!= 1)goto _LL46D;_tmp612=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp611)->f1;_tmp613=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp611)->f1;_LL46C:*_tmp613=({struct Cyc_Core_Opt*_tmp614=_cycalloc(sizeof(*
_tmp614));_tmp614->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp615=
_cycalloc(sizeof(*_tmp615));_tmp615[0]=({struct Cyc_Absyn_Less_kb_struct _tmp616;
_tmp616.tag=2;_tmp616.f1=0;_tmp616.f2=(void*)expected_kind;_tmp616;});_tmp615;}));
_tmp614;});goto _LL46A;_LL46D:;_LL46E: goto _LL46A;_LL46A:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp5D1);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp5D1);goto _LL437;_LL43E: if(_tmp5CC <= (void*)3?1:*((int*)
_tmp5CC)!= 13)goto _LL440;_tmp5D2=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp5CC)->f1;
_LL43F: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp617=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp617;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp5D2 != 0;_tmp5D2=_tmp5D2->tl){struct Cyc_Absyn_Enumfield*_tmp618=(
struct Cyc_Absyn_Enumfield*)_tmp5D2->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp618->name).f2))({struct Cyc_Std_String_pa_struct
_tmp61A;_tmp61A.tag=0;_tmp61A.f1=(struct _tagged_arr)*(*_tmp618->name).f2;{void*
_tmp619[1]={& _tmp61A};Cyc_Tcutil_terr(_tmp618->loc,_tag_arr("duplicate enum field name %s",
sizeof(char),29),_tag_arr(_tmp619,sizeof(void*),1));}});else{prev_fields=({
struct Cyc_List_List*_tmp61B=_region_malloc(uprev_rgn,sizeof(*_tmp61B));_tmp61B->hd=(*
_tmp618->name).f2;_tmp61B->tl=prev_fields;_tmp61B;});}if(_tmp618->tag == 0)
_tmp618->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp618->loc);
else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp618->tag)))({
struct Cyc_Std_String_pa_struct _tmp61D;_tmp61D.tag=0;_tmp61D.f1=(struct
_tagged_arr)*(*_tmp618->name).f2;{void*_tmp61C[1]={& _tmp61D};Cyc_Tcutil_terr(loc,
_tag_arr("enum field %s: expression is not constant",sizeof(char),42),_tag_arr(
_tmp61C,sizeof(void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp618->tag))).f1;tag_count=t1 + 1;(*_tmp618->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp61F;_tmp61F.tag=1;_tmp61F.f1=te->ns;_tmp61F;});
_tmp61E;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp618->name).f2,({
struct _tuple17*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp622;_tmp622.tag=4;_tmp622.f1=(void*)t;
_tmp622.f2=_tmp618;_tmp622;});_tmp621;});_tmp620->f2=1;_tmp620;}));}}};
_pop_region(uprev_rgn);}goto _LL437;}_LL440: if(_tmp5CC <= (void*)3?1:*((int*)
_tmp5CC)!= 12)goto _LL442;_tmp5D3=((struct Cyc_Absyn_EnumType_struct*)_tmp5CC)->f1;
_tmp5D4=((struct Cyc_Absyn_EnumType_struct*)_tmp5CC)->f2;_tmp5D5=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp5CC)->f2;_LL441: if(*_tmp5D5 == 0?1:((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp5D5))->fields == 0){struct _handler_cons
_tmp623;_push_handler(& _tmp623);{int _tmp625=0;if(setjmp(_tmp623.handler))_tmp625=
1;if(!_tmp625){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp5D3);*_tmp5D5=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp624=(void*)_exn_thrown;void*_tmp627=_tmp624;_LL470: if(_tmp627 != Cyc_Dict_Absent)
goto _LL472;_LL471: {struct Cyc_Tcenv_Genv*_tmp628=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp629=({struct Cyc_Absyn_Enumdecl*_tmp62A=_cycalloc(
sizeof(*_tmp62A));_tmp62A->sc=(void*)((void*)3);_tmp62A->name=_tmp5D3;_tmp62A->fields=
0;_tmp62A;});Cyc_Tc_tcEnumdecl(te,_tmp628,loc,_tmp629);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp5D3);*_tmp5D5=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL46F;}}_LL472:;_LL473:(void)_throw(_tmp627);_LL46F:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp5D5);*_tmp5D3=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL437;}_LL442: if(_tmp5CC <= (void*)3?1:*((
int*)_tmp5CC)!= 2)goto _LL444;_tmp5D6=((struct Cyc_Absyn_TunionType_struct*)
_tmp5CC)->f1;_tmp5D7=(void*)_tmp5D6.tunion_info;_tmp5D8=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp5CC)->f1).tunion_info;_tmp5D9=_tmp5D6.targs;_tmp5DA=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp5CC)->f1).targs;_tmp5DB=(void*)_tmp5D6.rgn;
_LL443: {struct Cyc_List_List*_tmp62B=*_tmp5DA;{void*_tmp62C=*_tmp5D8;struct Cyc_Absyn_UnknownTunionInfo
_tmp62D;struct _tuple1*_tmp62E;int _tmp62F;struct Cyc_Absyn_Tuniondecl**_tmp630;
struct Cyc_Absyn_Tuniondecl*_tmp631;_LL475: if(*((int*)_tmp62C)!= 0)goto _LL477;
_tmp62D=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp62C)->f1;_tmp62E=_tmp62D.name;
_tmp62F=_tmp62D.is_xtunion;_LL476: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp632;_push_handler(& _tmp632);{int _tmp634=0;if(setjmp(_tmp632.handler))
_tmp634=1;if(!_tmp634){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp62E);;
_pop_handler();}else{void*_tmp633=(void*)_exn_thrown;void*_tmp636=_tmp633;_LL47A:
if(_tmp636 != Cyc_Dict_Absent)goto _LL47C;_LL47B: {struct Cyc_Tcenv_Genv*_tmp637=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp638=({struct Cyc_Absyn_Tuniondecl*
_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->sc=(void*)((void*)3);_tmp63C->name=
_tmp62E;_tmp63C->tvs=0;_tmp63C->fields=0;_tmp63C->is_xtunion=_tmp62F;_tmp63C;});
Cyc_Tc_tcTuniondecl(te,_tmp637,loc,_tmp638);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp62E);if(_tmp62B != 0){({struct Cyc_Std_String_pa_struct _tmp63B;_tmp63B.tag=
0;_tmp63B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp62E);{struct Cyc_Std_String_pa_struct
_tmp63A;_tmp63A.tag=0;_tmp63A.f1=(struct _tagged_arr)(_tmp62F?_tag_arr("xtunion",
sizeof(char),8): _tag_arr("tunion",sizeof(char),7));{void*_tmp639[2]={& _tmp63A,&
_tmp63B};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized %s %s before using",
sizeof(char),48),_tag_arr(_tmp639,sizeof(void*),2));}}});return cvtenv;}goto
_LL479;}_LL47C:;_LL47D:(void)_throw(_tmp636);_LL479:;}}}if((*tudp)->is_xtunion != 
_tmp62F)({struct Cyc_Std_String_pa_struct _tmp63E;_tmp63E.tag=0;_tmp63E.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp62E);{void*_tmp63D[1]={& _tmp63E};Cyc_Tcutil_terr(
loc,_tag_arr("[x]tunion is different from type declaration %s",sizeof(char),48),
_tag_arr(_tmp63D,sizeof(void*),1));}});*_tmp5D8=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp640;_tmp640.tag=1;_tmp640.f1=tudp;_tmp640;});_tmp63F;});_tmp631=*tudp;goto
_LL478;}_LL477: if(*((int*)_tmp62C)!= 1)goto _LL474;_tmp630=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp62C)->f1;_tmp631=*_tmp630;_LL478: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5DB);{struct Cyc_List_List*tvs=_tmp631->tvs;for(0;_tmp62B != 0?
tvs != 0: 0;(_tmp62B=_tmp62B->tl,tvs=tvs->tl)){void*t1=(void*)_tmp62B->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp62B != 0)({struct Cyc_Std_String_pa_struct _tmp642;
_tmp642.tag=0;_tmp642.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp631->name);{
void*_tmp641[1]={& _tmp642};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s",
sizeof(char),38),_tag_arr(_tmp641,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp643=
_cycalloc(sizeof(*_tmp643));_tmp643->hd=(void*)e;_tmp643->tl=hidden_ts;_tmp643;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*_tmp5DA=Cyc_List_imp_append(*
_tmp5DA,Cyc_List_imp_rev(hidden_ts));}goto _LL474;}_LL474:;}goto _LL437;}_LL444:
if(_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 3)goto _LL446;_tmp5DC=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5CC)->f1;_tmp5DD=(void*)_tmp5DC.field_info;_tmp5DE=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5CC)->f1).field_info;_tmp5DF=_tmp5DC.targs;_LL445:{void*_tmp644=*_tmp5DE;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp645;struct _tuple1*_tmp646;struct
_tuple1*_tmp647;int _tmp648;struct Cyc_Absyn_Tuniondecl*_tmp649;struct Cyc_Absyn_Tunionfield*
_tmp64A;_LL47F: if(*((int*)_tmp644)!= 0)goto _LL481;_tmp645=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp644)->f1;_tmp646=_tmp645.tunion_name;_tmp647=_tmp645.field_name;_tmp648=
_tmp645.is_xtunion;_LL480: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp64B;_push_handler(& _tmp64B);{int _tmp64D=0;if(setjmp(
_tmp64B.handler))_tmp64D=1;if(!_tmp64D){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp646);;_pop_handler();}else{void*_tmp64C=(void*)_exn_thrown;void*_tmp64F=
_tmp64C;_LL484: if(_tmp64F != Cyc_Dict_Absent)goto _LL486;_LL485:({struct Cyc_Std_String_pa_struct
_tmp651;_tmp651.tag=0;_tmp651.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp646);{void*_tmp650[1]={& _tmp651};Cyc_Tcutil_terr(loc,_tag_arr("unbound type tunion %s",
sizeof(char),23),_tag_arr(_tmp650,sizeof(void*),1));}});return cvtenv;_LL486:;
_LL487:(void)_throw(_tmp64F);_LL483:;}}}{struct _handler_cons _tmp652;
_push_handler(& _tmp652);{int _tmp654=0;if(setjmp(_tmp652.handler))_tmp654=1;if(!
_tmp654){{void*_tmp655=Cyc_Tcenv_lookup_ordinary(te,loc,_tmp647);struct Cyc_Absyn_Tuniondecl*
_tmp656;struct Cyc_Absyn_Tunionfield*_tmp657;_LL489: if(*((int*)_tmp655)!= 2)goto
_LL48B;_tmp656=((struct Cyc_Tcenv_TunionRes_struct*)_tmp655)->f1;_tmp657=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp655)->f2;_LL48A: tuf=_tmp657;tud=_tmp656;if(tud->is_xtunion
!= _tmp648)({struct Cyc_Std_String_pa_struct _tmp659;_tmp659.tag=0;_tmp659.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp646);{void*_tmp658[1]={& _tmp659};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(char),48),_tag_arr(_tmp658,sizeof(void*),1));}});goto _LL488;_LL48B:;
_LL48C:({struct Cyc_Std_String_pa_struct _tmp65C;_tmp65C.tag=0;_tmp65C.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp646);{struct Cyc_Std_String_pa_struct
_tmp65B;_tmp65B.tag=0;_tmp65B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp647);{void*_tmp65A[2]={& _tmp65B,& _tmp65C};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp65A,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv
_tmp65D=cvtenv;_npop_handler(0);return _tmp65D;}_LL488:;};_pop_handler();}else{
void*_tmp653=(void*)_exn_thrown;void*_tmp65F=_tmp653;_LL48E: if(_tmp65F != Cyc_Dict_Absent)
goto _LL490;_LL48F:({struct Cyc_Std_String_pa_struct _tmp662;_tmp662.tag=0;_tmp662.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp646);{struct Cyc_Std_String_pa_struct
_tmp661;_tmp661.tag=0;_tmp661.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp647);{void*_tmp660[2]={& _tmp661,& _tmp662};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp660,sizeof(void*),2));}}});return cvtenv;_LL490:;
_LL491:(void)_throw(_tmp65F);_LL48D:;}}}*_tmp5DE=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp664;_tmp664.tag=1;_tmp664.f1=tud;_tmp664.f2=tuf;_tmp664;});_tmp663;});
_tmp649=tud;_tmp64A=tuf;goto _LL482;}_LL481: if(*((int*)_tmp644)!= 1)goto _LL47E;
_tmp649=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp644)->f1;_tmp64A=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp644)->f2;_LL482: {struct Cyc_List_List*tvs=
_tmp649->tvs;for(0;_tmp5DF != 0?tvs != 0: 0;(_tmp5DF=_tmp5DF->tl,tvs=tvs->tl)){void*
t1=(void*)_tmp5DF->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp5DF != 0)({
struct Cyc_Std_String_pa_struct _tmp667;_tmp667.tag=0;_tmp667.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp64A->name);{struct Cyc_Std_String_pa_struct
_tmp666;_tmp666.tag=0;_tmp666.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp649->name);{void*_tmp665[2]={& _tmp666,& _tmp667};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s.%s",
sizeof(char),41),_tag_arr(_tmp665,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_Std_String_pa_struct
_tmp66A;_tmp66A.tag=0;_tmp66A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp64A->name);{struct Cyc_Std_String_pa_struct _tmp669;_tmp669.tag=0;_tmp669.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp649->name);{void*_tmp668[2]={&
_tmp669,& _tmp66A};Cyc_Tcutil_terr(loc,_tag_arr("too few type arguments for tunion %s.%s",
sizeof(char),40),_tag_arr(_tmp668,sizeof(void*),2));}}});goto _LL47E;}_LL47E:;}
goto _LL437;_LL446: if(_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 4)goto _LL448;
_tmp5E0=((struct Cyc_Absyn_PointerType_struct*)_tmp5CC)->f1;_tmp5E1=(void*)
_tmp5E0.elt_typ;_tmp5E2=(void*)_tmp5E0.rgn_typ;_tmp5E3=_tmp5E0.nullable;_tmp5E4=
_tmp5E0.tq;_tmp5E5=_tmp5E0.bounds;_LL447: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp5E1);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5E2);{void*_tmp66B=(void*)(Cyc_Absyn_compress_conref(_tmp5E5))->v;
void*_tmp66C;struct Cyc_Absyn_Exp*_tmp66D;_LL493: if(_tmp66B <= (void*)1?1:*((int*)
_tmp66B)!= 0)goto _LL495;_tmp66C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp66B)->f1;if(_tmp66C <= (void*)1?1:*((int*)_tmp66C)!= 0)goto _LL495;_tmp66D=((
struct Cyc_Absyn_Upper_b_struct*)_tmp66C)->f1;_LL494: if(!Cyc_Tcutil_is_const_exp(
te,_tmp66D))({void*_tmp66E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not a constant",
sizeof(char),44),_tag_arr(_tmp66E,sizeof(void*),0));});Cyc_Tcexp_tcExp(te,0,
_tmp66D);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp66D))({void*_tmp66F[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("pointer bounds expression is not an unsigned int",sizeof(char),49),
_tag_arr(_tmp66F,sizeof(void*),0));});Cyc_Evexp_eval_const_uint_exp(_tmp66D);
goto _LL492;_LL495:;_LL496: goto _LL492;_LL492:;}goto _LL437;_LL448: if(_tmp5CC <= (
void*)3?1:*((int*)_tmp5CC)!= 14)goto _LL44A;_tmp5E6=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp5CC)->f1;_LL449: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,
_tmp5E6);goto _LL437;_LL44A: if(_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 17)goto
_LL44C;_tmp5E7=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp5CC)->f1;_LL44B:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp5E7);goto _LL437;
_LL44C: if(_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 18)goto _LL44E;_LL44D: goto
_LL44F;_LL44E: if(_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 5)goto _LL450;_LL44F:
goto _LL451;_LL450: if((int)_tmp5CC != 1)goto _LL452;_LL451: goto _LL453;_LL452: if(
_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 6)goto _LL454;_LL453: goto _LL437;_LL454:
if(_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 7)goto _LL456;_tmp5E8=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp5CC)->f1;_tmp5E9=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5CC)->f2;_tmp5EA=((struct Cyc_Absyn_ArrayType_struct*)_tmp5CC)->f3;_LL455:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,_tmp5E8);if(_tmp5EA
== 0?1: !Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp5EA)))({
void*_tmp670[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array bounds expression is not constant",
sizeof(char),40),_tag_arr(_tmp670,sizeof(void*),0));});Cyc_Tcexp_tcExp(te,0,(
struct Cyc_Absyn_Exp*)_check_null(_tmp5EA));if(!Cyc_Tcutil_coerce_uint_typ(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp5EA)))({void*_tmp671[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("array bounds expression is not an unsigned int",sizeof(char),47),
_tag_arr(_tmp671,sizeof(void*),0));});Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp5EA));goto _LL437;_LL456: if(_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)
!= 8)goto _LL458;_tmp5EB=((struct Cyc_Absyn_FnType_struct*)_tmp5CC)->f1;_tmp5EC=
_tmp5EB.tvars;_tmp5ED=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp5CC)->f1).tvars;_tmp5EE=_tmp5EB.effect;_tmp5EF=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp5CC)->f1).effect;_tmp5F0=(void*)_tmp5EB.ret_typ;
_tmp5F1=_tmp5EB.args;_tmp5F2=_tmp5EB.c_varargs;_tmp5F3=_tmp5EB.cyc_varargs;
_tmp5F4=_tmp5EB.rgn_po;_tmp5F5=_tmp5EB.attributes;_LL457: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp5F5 != 0;_tmp5F5=_tmp5F5->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp5F5->hd))({struct Cyc_Std_String_pa_struct
_tmp673;_tmp673.tag=0;_tmp673.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp5F5->hd);{void*_tmp672[1]={& _tmp673};Cyc_Tcutil_terr(loc,_tag_arr("bad function type attribute %s",
sizeof(char),31),_tag_arr(_tmp672,sizeof(void*),1));}});{void*_tmp674=(void*)
_tmp5F5->hd;void*_tmp675;int _tmp676;int _tmp677;_LL498: if((int)_tmp674 != 0)goto
_LL49A;_LL499: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL497;_LL49A:
if((int)_tmp674 != 1)goto _LL49C;_LL49B: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}
goto _LL497;_LL49C: if((int)_tmp674 != 2)goto _LL49E;_LL49D: if(!seen_fastcall){
seen_fastcall=1;++ num_convs;}goto _LL497;_LL49E: if(_tmp674 <= (void*)16?1:*((int*)
_tmp674)!= 3)goto _LL4A0;_tmp675=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp674)->f1;_tmp676=((struct Cyc_Absyn_Format_att_struct*)_tmp674)->f2;_tmp677=((
struct Cyc_Absyn_Format_att_struct*)_tmp674)->f3;_LL49F: if(!seen_format){
seen_format=1;ft=_tmp675;fmt_desc_arg=_tmp676;fmt_arg_start=_tmp677;}else{({void*
_tmp678[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function can't have multiple format attributes",
sizeof(char),47),_tag_arr(_tmp678,sizeof(void*),0));});}goto _LL497;_LL4A0:;
_LL4A1: goto _LL497;_LL497:;}}if(num_convs > 1)({void*_tmp679[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("function can't have multiple calling conventions",sizeof(char),49),
_tag_arr(_tmp679,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp5ED);{
struct Cyc_List_List*b=*_tmp5ED;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp67A=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp67B;_LL4A3: if(*((int*)_tmp67A)!= 0)goto
_LL4A5;_tmp67B=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp67A)->f1;if((int)
_tmp67B != 1)goto _LL4A5;_LL4A4:({struct Cyc_Std_String_pa_struct _tmp67D;_tmp67D.tag=
0;_tmp67D.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name;{void*
_tmp67C[1]={& _tmp67D};Cyc_Tcutil_terr(loc,_tag_arr("function attempts to abstract Mem type variable %s",
sizeof(char),51),_tag_arr(_tmp67C,sizeof(void*),1));}});goto _LL4A2;_LL4A5:;
_LL4A6: goto _LL4A2;_LL4A2:;}}}{struct Cyc_Tcutil_CVTEnv _tmp67E=({struct Cyc_Tcutil_CVTEnv
_tmp6EC;_tmp6EC.kind_env=cvtenv.kind_env;_tmp6EC.free_vars=0;_tmp6EC.free_evars=
0;_tmp6EC.generalize_evars=cvtenv.generalize_evars;_tmp6EC.fn_result=1;_tmp6EC;});
_tmp67E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp67E,(void*)1,_tmp5F0);_tmp67E.fn_result=
0;{struct Cyc_List_List*a=_tmp5F1;for(0;a != 0;a=a->tl){_tmp67E=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp67E,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp5F3 != 0){if(
_tmp5F2)({void*_tmp67F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("both c_vararg and cyc_vararg",sizeof(char),29),
_tag_arr(_tmp67F,sizeof(void*),0));});{void*_tmp681;int _tmp682;struct Cyc_Absyn_VarargInfo
_tmp680=*_tmp5F3;_tmp681=(void*)_tmp680.type;_tmp682=_tmp680.inject;_tmp67E=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp67E,(void*)1,_tmp681);if(_tmp682){void*_tmp683=Cyc_Tcutil_compress(
_tmp681);struct Cyc_Absyn_TunionInfo _tmp684;void*_tmp685;_LL4A8: if(_tmp683 <= (
void*)3?1:*((int*)_tmp683)!= 2)goto _LL4AA;_tmp684=((struct Cyc_Absyn_TunionType_struct*)
_tmp683)->f1;_tmp685=(void*)_tmp684.tunion_info;if(*((int*)_tmp685)!= 1)goto
_LL4AA;_LL4A9: goto _LL4A7;_LL4AA:;_LL4AB:({void*_tmp686[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("can't inject a non-[x]tunion type",sizeof(char),34),_tag_arr(_tmp686,
sizeof(void*),0));});goto _LL4A7;_LL4A7:;}}}if(seen_format){int _tmp687=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp5F1);if((((fmt_desc_arg < 0?1:
fmt_desc_arg > _tmp687)?1: fmt_arg_start < 0)?1:(_tmp5F3 == 0?fmt_arg_start != 0: 0))?
1:(_tmp5F3 != 0?fmt_arg_start != _tmp687 + 1: 0))({void*_tmp688[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bad format descriptor",sizeof(char),22),_tag_arr(_tmp688,sizeof(
void*),0));});else{void*_tmp68A;struct _tuple2 _tmp689=*((struct _tuple2*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5F1,fmt_desc_arg - 1);_tmp68A=_tmp689.f3;{
void*_tmp68B=Cyc_Tcutil_compress(_tmp68A);struct Cyc_Absyn_PtrInfo _tmp68C;void*
_tmp68D;struct Cyc_Absyn_Conref*_tmp68E;_LL4AD: if(_tmp68B <= (void*)3?1:*((int*)
_tmp68B)!= 4)goto _LL4AF;_tmp68C=((struct Cyc_Absyn_PointerType_struct*)_tmp68B)->f1;
_tmp68D=(void*)_tmp68C.elt_typ;_tmp68E=_tmp68C.bounds;_LL4AE:{struct _tuple6
_tmp690=({struct _tuple6 _tmp68F;_tmp68F.f1=Cyc_Tcutil_compress(_tmp68D);_tmp68F.f2=
Cyc_Absyn_conref_def((void*)0,_tmp68E);_tmp68F;});void*_tmp691;void*_tmp692;void*
_tmp693;void*_tmp694;_LL4B2: _tmp691=_tmp690.f1;if(_tmp691 <= (void*)3?1:*((int*)
_tmp691)!= 5)goto _LL4B4;_tmp692=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp691)->f1;
if((int)_tmp692 != 2)goto _LL4B4;_tmp693=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp691)->f2;if((int)_tmp693 != 0)goto _LL4B4;_tmp694=_tmp690.f2;if((int)_tmp694 != 
0)goto _LL4B4;_LL4B3: goto _LL4B1;_LL4B4:;_LL4B5:({void*_tmp695[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("format descriptor is not a char ? type",sizeof(char),39),_tag_arr(
_tmp695,sizeof(void*),0));});goto _LL4B1;_LL4B1:;}goto _LL4AC;_LL4AF:;_LL4B0:({
void*_tmp696[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format descriptor is not a char ? type",
sizeof(char),39),_tag_arr(_tmp696,sizeof(void*),0));});goto _LL4AC;_LL4AC:;}if(
fmt_arg_start != 0){int problem;{struct _tuple6 _tmp698=({struct _tuple6 _tmp697;
_tmp697.f1=ft;_tmp697.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp5F3))->type);_tmp697;});void*_tmp699;void*_tmp69A;struct Cyc_Absyn_TunionInfo
_tmp69B;void*_tmp69C;struct Cyc_Absyn_Tuniondecl**_tmp69D;struct Cyc_Absyn_Tuniondecl*
_tmp69E;void*_tmp69F;void*_tmp6A0;struct Cyc_Absyn_TunionInfo _tmp6A1;void*_tmp6A2;
struct Cyc_Absyn_Tuniondecl**_tmp6A3;struct Cyc_Absyn_Tuniondecl*_tmp6A4;_LL4B7:
_tmp699=_tmp698.f1;if((int)_tmp699 != 0)goto _LL4B9;_tmp69A=_tmp698.f2;if(_tmp69A
<= (void*)3?1:*((int*)_tmp69A)!= 2)goto _LL4B9;_tmp69B=((struct Cyc_Absyn_TunionType_struct*)
_tmp69A)->f1;_tmp69C=(void*)_tmp69B.tunion_info;if(*((int*)_tmp69C)!= 1)goto
_LL4B9;_tmp69D=((struct Cyc_Absyn_KnownTunion_struct*)_tmp69C)->f1;_tmp69E=*
_tmp69D;_LL4B8: problem=Cyc_Absyn_qvar_cmp(_tmp69E->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL4B6;_LL4B9: _tmp69F=_tmp698.f1;if((int)_tmp69F != 1)goto _LL4BB;_tmp6A0=
_tmp698.f2;if(_tmp6A0 <= (void*)3?1:*((int*)_tmp6A0)!= 2)goto _LL4BB;_tmp6A1=((
struct Cyc_Absyn_TunionType_struct*)_tmp6A0)->f1;_tmp6A2=(void*)_tmp6A1.tunion_info;
if(*((int*)_tmp6A2)!= 1)goto _LL4BB;_tmp6A3=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp6A2)->f1;_tmp6A4=*_tmp6A3;_LL4BA: problem=Cyc_Absyn_qvar_cmp(_tmp6A4->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL4B6;_LL4BB:;_LL4BC: problem=1;goto
_LL4B6;_LL4B6:;}if(problem)({void*_tmp6A5[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format attribute and vararg types don't match",
sizeof(char),46),_tag_arr(_tmp6A5,sizeof(void*),0));});}}}{struct Cyc_List_List*
rpo=_tmp5F4;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp6A7;void*_tmp6A8;void*
_tmp6A9;struct _tuple6*_tmp6A6=(struct _tuple6*)rpo->hd;_tmp6A7=*_tmp6A6;_tmp6A8=
_tmp6A7.f1;_tmp6A9=_tmp6A7.f2;_tmp67E=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp67E,(void*)4,_tmp6A8);_tmp67E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp67E,(
void*)3,_tmp6A9);}}if(*_tmp5EF != 0)_tmp67E=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp67E,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp5EF))->v);else{
struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp67E.free_vars;for(0;
tvs != 0;tvs=tvs->tl){void*_tmp6AA=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmp6AB;struct Cyc_Core_Opt**_tmp6AC;void*
_tmp6AD;void*_tmp6AE;struct Cyc_Core_Opt*_tmp6AF;struct Cyc_Core_Opt**_tmp6B0;void*
_tmp6B1;void*_tmp6B2;struct Cyc_Core_Opt*_tmp6B3;struct Cyc_Core_Opt**_tmp6B4;
_LL4BE: if(*((int*)_tmp6AA)!= 2)goto _LL4C0;_tmp6AB=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6AA)->f1;_tmp6AC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6AA)->f1;_tmp6AD=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6AA)->f2;if((
int)_tmp6AD != 3)goto _LL4C0;_LL4BF:*_tmp6AC=({struct Cyc_Core_Opt*_tmp6B5=
_cycalloc(sizeof(*_tmp6B5));_tmp6B5->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp6B7;_tmp6B7.tag=0;_tmp6B7.f1=(void*)((void*)3);_tmp6B7;});_tmp6B6;}));
_tmp6B5;});goto _LL4C1;_LL4C0: if(*((int*)_tmp6AA)!= 0)goto _LL4C2;_tmp6AE=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp6AA)->f1;if((int)_tmp6AE != 3)goto _LL4C2;_LL4C1:
effect=({struct Cyc_List_List*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6B9=_cycalloc(sizeof(*
_tmp6B9));_tmp6B9[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6BA;_tmp6BA.tag=19;
_tmp6BA.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6BB=_cycalloc(
sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_VarType_struct _tmp6BC;_tmp6BC.tag=
1;_tmp6BC.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6BC;});_tmp6BB;}));_tmp6BA;});
_tmp6B9;}));_tmp6B8->tl=effect;_tmp6B8;});goto _LL4BD;_LL4C2: if(*((int*)_tmp6AA)
!= 2)goto _LL4C4;_tmp6AF=((struct Cyc_Absyn_Less_kb_struct*)_tmp6AA)->f1;_tmp6B0=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6AA)->f1;_tmp6B1=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6AA)->f2;if((int)_tmp6B1 != 4)goto
_LL4C4;_LL4C3:*_tmp6B0=({struct Cyc_Core_Opt*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));
_tmp6BD->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6BE=_cycalloc(
sizeof(*_tmp6BE));_tmp6BE[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6BF;_tmp6BF.tag=0;
_tmp6BF.f1=(void*)((void*)4);_tmp6BF;});_tmp6BE;}));_tmp6BD;});goto _LL4C5;_LL4C4:
if(*((int*)_tmp6AA)!= 0)goto _LL4C6;_tmp6B2=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp6AA)->f1;if((int)_tmp6B2 != 4)goto _LL4C6;_LL4C5: effect=({struct Cyc_List_List*
_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1[0]=({struct Cyc_Absyn_VarType_struct
_tmp6C2;_tmp6C2.tag=1;_tmp6C2.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6C2;});
_tmp6C1;}));_tmp6C0->tl=effect;_tmp6C0;});goto _LL4BD;_LL4C6: if(*((int*)_tmp6AA)
!= 1)goto _LL4C8;_tmp6B3=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6AA)->f1;
_tmp6B4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6AA)->f1;
_LL4C7:*_tmp6B4=({struct Cyc_Core_Opt*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));
_tmp6C4[0]=({struct Cyc_Absyn_Less_kb_struct _tmp6C5;_tmp6C5.tag=2;_tmp6C5.f1=0;
_tmp6C5.f2=(void*)((void*)0);_tmp6C5;});_tmp6C4;}));_tmp6C3;});goto _LL4C9;_LL4C8:;
_LL4C9: effect=({struct Cyc_List_List*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));
_tmp6C7[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6C8;_tmp6C8.tag=21;_tmp6C8.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));
_tmp6C9[0]=({struct Cyc_Absyn_VarType_struct _tmp6CA;_tmp6CA.tag=1;_tmp6CA.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6CA;});_tmp6C9;}));_tmp6C8;});_tmp6C7;}));
_tmp6C6->tl=effect;_tmp6C6;});goto _LL4BD;_LL4BD:;}}{struct Cyc_List_List*ts=
_tmp67E.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp6CB=Cyc_Tcutil_typ_kind((
void*)ts->hd);_LL4CB: if((int)_tmp6CB != 3)goto _LL4CD;_LL4CC: effect=({struct Cyc_List_List*
_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp6CE;_tmp6CE.tag=19;_tmp6CE.f1=(void*)((void*)ts->hd);_tmp6CE;});_tmp6CD;}));
_tmp6CC->tl=effect;_tmp6CC;});goto _LL4CA;_LL4CD: if((int)_tmp6CB != 4)goto _LL4CF;
_LL4CE: effect=({struct Cyc_List_List*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));_tmp6CF->hd=(
void*)((void*)ts->hd);_tmp6CF->tl=effect;_tmp6CF;});goto _LL4CA;_LL4CF:;_LL4D0:
effect=({struct Cyc_List_List*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));
_tmp6D1[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6D2;_tmp6D2.tag=21;_tmp6D2.f1=(
void*)((void*)ts->hd);_tmp6D2;});_tmp6D1;}));_tmp6D0->tl=effect;_tmp6D0;});goto
_LL4CA;_LL4CA:;}}*_tmp5EF=({struct Cyc_Core_Opt*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));
_tmp6D3->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6D4=_cycalloc(
sizeof(*_tmp6D4));_tmp6D4[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6D5;_tmp6D5.tag=
20;_tmp6D5.f1=effect;_tmp6D5;});_tmp6D4;}));_tmp6D3;});}if(*_tmp5ED != 0){struct
Cyc_List_List*bs=*_tmp5ED;for(0;bs != 0;bs=bs->tl){void*_tmp6D6=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmp6D7;struct
Cyc_Core_Opt**_tmp6D8;struct Cyc_Core_Opt*_tmp6D9;struct Cyc_Core_Opt**_tmp6DA;
void*_tmp6DB;struct Cyc_Core_Opt*_tmp6DC;struct Cyc_Core_Opt**_tmp6DD;void*_tmp6DE;
struct Cyc_Core_Opt*_tmp6DF;struct Cyc_Core_Opt**_tmp6E0;void*_tmp6E1;void*_tmp6E2;
_LL4D2: if(*((int*)_tmp6D6)!= 1)goto _LL4D4;_tmp6D7=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6D6)->f1;_tmp6D8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6D6)->f1;_LL4D3:({struct Cyc_Std_String_pa_struct _tmp6E4;_tmp6E4.tag=0;
_tmp6E4.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name;{void*
_tmp6E3[1]={& _tmp6E4};Cyc_Tcutil_warn(loc,_tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof(char),47),_tag_arr(_tmp6E3,sizeof(void*),1));}});_tmp6DA=_tmp6D8;goto
_LL4D5;_LL4D4: if(*((int*)_tmp6D6)!= 2)goto _LL4D6;_tmp6D9=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6D6)->f1;_tmp6DA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6D6)->f1;_tmp6DB=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6D6)->f2;if((
int)_tmp6DB != 1)goto _LL4D6;_LL4D5: _tmp6DD=_tmp6DA;goto _LL4D7;_LL4D6: if(*((int*)
_tmp6D6)!= 2)goto _LL4D8;_tmp6DC=((struct Cyc_Absyn_Less_kb_struct*)_tmp6D6)->f1;
_tmp6DD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6D6)->f1;
_tmp6DE=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6D6)->f2;if((int)_tmp6DE != 
0)goto _LL4D8;_LL4D7:*_tmp6DD=({struct Cyc_Core_Opt*_tmp6E5=_cycalloc(sizeof(*
_tmp6E5));_tmp6E5->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6E6=
_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6E7;
_tmp6E7.tag=0;_tmp6E7.f1=(void*)((void*)2);_tmp6E7;});_tmp6E6;}));_tmp6E5;});
goto _LL4D1;_LL4D8: if(*((int*)_tmp6D6)!= 2)goto _LL4DA;_tmp6DF=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6D6)->f1;_tmp6E0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6D6)->f1;_tmp6E1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6D6)->f2;
_LL4D9:*_tmp6E0=({struct Cyc_Core_Opt*_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6E9=_cycalloc(sizeof(*_tmp6E9));
_tmp6E9[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6EA;_tmp6EA.tag=0;_tmp6EA.f1=(void*)
_tmp6E1;_tmp6EA;});_tmp6E9;}));_tmp6E8;});goto _LL4D1;_LL4DA: if(*((int*)_tmp6D6)
!= 0)goto _LL4DC;_tmp6E2=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp6D6)->f1;if((
int)_tmp6E2 != 1)goto _LL4DC;_LL4DB:({void*_tmp6EB[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("functions can't abstract M types",sizeof(char),33),_tag_arr(_tmp6EB,
sizeof(void*),0));});goto _LL4D1;_LL4DC:;_LL4DD: goto _LL4D1;_LL4D1:;}}cvtenv.kind_env=
Cyc_Tcutil_remove_bound_tvars(_tmp67E.kind_env,*_tmp5ED);_tmp67E.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp67E.free_vars,*_tmp5ED);{struct Cyc_List_List*tvs=_tmp67E.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp67E.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL437;}}_LL458: if(_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 
9)goto _LL45A;_tmp5F6=((struct Cyc_Absyn_TupleType_struct*)_tmp5CC)->f1;_LL459:
for(0;_tmp5F6 != 0;_tmp5F6=_tmp5F6->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,(void*)1,(*((struct _tuple4*)_tmp5F6->hd)).f2);}goto _LL437;_LL45A: if(
_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 11)goto _LL45C;_tmp5F7=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5CC)->f1;_tmp5F8=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5CC)->f2;_LL45B:{
struct _RegionHandle _tmp6ED=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp6ED;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp5F8 != 0;_tmp5F8=_tmp5F8->tl){struct Cyc_Absyn_Aggrfield _tmp6EF;struct
_tagged_arr*_tmp6F0;struct Cyc_Absyn_Tqual _tmp6F1;void*_tmp6F2;struct Cyc_Absyn_Exp*
_tmp6F3;struct Cyc_List_List*_tmp6F4;struct Cyc_Absyn_Aggrfield*_tmp6EE=(struct Cyc_Absyn_Aggrfield*)
_tmp5F8->hd;_tmp6EF=*_tmp6EE;_tmp6F0=_tmp6EF.name;_tmp6F1=_tmp6EF.tq;_tmp6F2=(
void*)_tmp6EF.type;_tmp6F3=_tmp6EF.width;_tmp6F4=_tmp6EF.attributes;if(((int(*)(
int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,
struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,prev_fields,_tmp6F0))({
struct Cyc_Std_String_pa_struct _tmp6F6;_tmp6F6.tag=0;_tmp6F6.f1=(struct
_tagged_arr)*_tmp6F0;{void*_tmp6F5[1]={& _tmp6F6};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s",
sizeof(char),19),_tag_arr(_tmp6F5,sizeof(void*),1));}});if(Cyc_Std_strcmp(*
_tmp6F0,_tag_arr("",sizeof(char),1))!= 0)prev_fields=({struct Cyc_List_List*
_tmp6F7=_region_malloc(aprev_rgn,sizeof(*_tmp6F7));_tmp6F7->hd=_tmp6F0;_tmp6F7->tl=
prev_fields;_tmp6F7;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp6F2);if(_tmp5F7 == (void*)1?!Cyc_Tcutil_bits_only(_tmp6F2): 0)({struct Cyc_Std_String_pa_struct
_tmp6F9;_tmp6F9.tag=0;_tmp6F9.f1=(struct _tagged_arr)*_tmp6F0;{void*_tmp6F8[1]={&
_tmp6F9};Cyc_Tcutil_terr(loc,_tag_arr("union member %s has a non-integral type",
sizeof(char),40),_tag_arr(_tmp6F8,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(
loc,te,_tmp6F2,_tmp6F3,_tmp6F0);Cyc_Tcutil_check_field_atts(loc,_tmp6F0,_tmp6F4);}};
_pop_region(aprev_rgn);}goto _LL437;_LL45C: if(_tmp5CC <= (void*)3?1:*((int*)
_tmp5CC)!= 10)goto _LL45E;_tmp5F9=((struct Cyc_Absyn_AggrType_struct*)_tmp5CC)->f1;
_tmp5FA=(void*)_tmp5F9.aggr_info;_tmp5FB=(void**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp5CC)->f1).aggr_info;_tmp5FC=_tmp5F9.targs;_tmp5FD=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp5CC)->f1).targs;_LL45D:{void*_tmp6FA=*
_tmp5FB;void*_tmp6FB;struct _tuple1*_tmp6FC;struct Cyc_Absyn_Aggrdecl**_tmp6FD;
struct Cyc_Absyn_Aggrdecl*_tmp6FE;_LL4DF: if(*((int*)_tmp6FA)!= 0)goto _LL4E1;
_tmp6FB=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp6FA)->f1;_tmp6FC=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp6FA)->f2;_LL4E0: {struct Cyc_Absyn_Aggrdecl**
adp;{struct _handler_cons _tmp6FF;_push_handler(& _tmp6FF);{int _tmp701=0;if(setjmp(
_tmp6FF.handler))_tmp701=1;if(!_tmp701){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,
_tmp6FC);*_tmp5FB=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp702=_cycalloc(
sizeof(*_tmp702));_tmp702[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp703;_tmp703.tag=
1;_tmp703.f1=adp;_tmp703;});_tmp702;});;_pop_handler();}else{void*_tmp700=(void*)
_exn_thrown;void*_tmp705=_tmp700;_LL4E4: if(_tmp705 != Cyc_Dict_Absent)goto _LL4E6;
_LL4E5: {struct Cyc_Tcenv_Genv*_tmp706=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*
_tmp707=({struct Cyc_Absyn_Aggrdecl*_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C->kind=(
void*)_tmp6FB;_tmp70C->sc=(void*)((void*)3);_tmp70C->name=_tmp6FC;_tmp70C->tvs=0;
_tmp70C->impl=0;_tmp70C->attributes=0;_tmp70C;});Cyc_Tc_tcAggrdecl(te,_tmp706,
loc,_tmp707);adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp6FC);*_tmp5FB=(void*)({
struct Cyc_Absyn_KnownAggr_struct*_tmp708=_cycalloc(sizeof(*_tmp708));_tmp708[0]=({
struct Cyc_Absyn_KnownAggr_struct _tmp709;_tmp709.tag=1;_tmp709.f1=adp;_tmp709;});
_tmp708;});if(*_tmp5FD != 0){({struct Cyc_Std_String_pa_struct _tmp70B;_tmp70B.tag=
0;_tmp70B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp6FC);{void*_tmp70A[1]={&
_tmp70B};Cyc_Tcutil_terr(loc,_tag_arr("declare parameterized type %s before using",
sizeof(char),43),_tag_arr(_tmp70A,sizeof(void*),1));}});return cvtenv;}goto _LL4E3;}
_LL4E6:;_LL4E7:(void)_throw(_tmp705);_LL4E3:;}}}_tmp6FE=*adp;goto _LL4E2;}_LL4E1:
if(*((int*)_tmp6FA)!= 1)goto _LL4DE;_tmp6FD=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp6FA)->f1;_tmp6FE=*_tmp6FD;_LL4E2: {struct Cyc_List_List*tvs=_tmp6FE->tvs;
struct Cyc_List_List*ts=*_tmp5FD;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_Std_String_pa_struct
_tmp70E;_tmp70E.tag=0;_tmp70E.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6FE->name);{void*_tmp70D[1]={& _tmp70E};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for type %s",
sizeof(char),32),_tag_arr(_tmp70D,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp70F=
_cycalloc(sizeof(*_tmp70F));_tmp70F->hd=(void*)e;_tmp70F->tl=hidden_ts;_tmp70F;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*_tmp5FD=Cyc_List_imp_append(*
_tmp5FD,Cyc_List_imp_rev(hidden_ts));}}_LL4DE:;}goto _LL437;_LL45E: if(_tmp5CC <= (
void*)3?1:*((int*)_tmp5CC)!= 16)goto _LL460;_tmp5FE=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5CC)->f1;_tmp5FF=((struct Cyc_Absyn_TypedefType_struct*)_tmp5CC)->f2;_tmp600=(
struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp5CC)->f2;
_tmp601=((struct Cyc_Absyn_TypedefType_struct*)_tmp5CC)->f3;_tmp602=(struct Cyc_Absyn_Typedefdecl**)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp5CC)->f3;_tmp603=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5CC)->f4;_tmp604=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp5CC)->f4;_LL45F: {struct Cyc_List_List*targs=*_tmp600;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmp710;_push_handler(& _tmp710);{int _tmp712=0;if(setjmp(
_tmp710.handler))_tmp712=1;if(!_tmp712){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmp5FE);;_pop_handler();}else{void*_tmp711=(void*)_exn_thrown;void*_tmp714=
_tmp711;_LL4E9: if(_tmp714 != Cyc_Dict_Absent)goto _LL4EB;_LL4EA:({struct Cyc_Std_String_pa_struct
_tmp716;_tmp716.tag=0;_tmp716.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp5FE);{void*_tmp715[1]={& _tmp716};Cyc_Tcutil_terr(loc,_tag_arr("unbound typedef name %s",
sizeof(char),24),_tag_arr(_tmp715,sizeof(void*),1));}});return cvtenv;_LL4EB:;
_LL4EC:(void)_throw(_tmp714);_LL4E8:;}}}*_tmp602=(struct Cyc_Absyn_Typedefdecl*)
td;_tmp5FE[0]=(td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0?
tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717->hd=({
struct _tuple8*_tmp718=_cycalloc(sizeof(*_tmp718));_tmp718->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp718->f2=(void*)ts->hd;_tmp718;});_tmp717->tl=inst;_tmp717;});}if(ts
!= 0)({struct Cyc_Std_String_pa_struct _tmp71A;_tmp71A.tag=0;_tmp71A.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp5FE);{void*_tmp719[1]={& _tmp71A};Cyc_Tcutil_terr(
loc,_tag_arr("too many parameters for typedef %s",sizeof(char),35),_tag_arr(
_tmp719,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp71B=_cycalloc(
sizeof(*_tmp71B));_tmp71B->hd=(void*)e;_tmp71B->tl=hidden_ts;_tmp71B;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C->hd=({struct _tuple8*_tmp71D=
_cycalloc(sizeof(*_tmp71D));_tmp71D->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp71D->f2=
e;_tmp71D;});_tmp71C->tl=inst;_tmp71C;});}*_tmp600=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp604=({struct Cyc_Core_Opt*
_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->v=(void*)new_typ;_tmp71E;});}goto
_LL437;}}_LL460: if((int)_tmp5CC != 2)goto _LL462;_LL461: goto _LL437;_LL462: if(
_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 15)goto _LL464;_tmp605=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5CC)->f1;_LL463: _tmp606=_tmp605;goto _LL465;_LL464: if(_tmp5CC <= (void*)3?1:*((
int*)_tmp5CC)!= 19)goto _LL466;_tmp606=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp5CC)->f1;_LL465: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp606);goto _LL437;_LL466: if(_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 21)goto
_LL468;_tmp607=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp5CC)->f1;_LL467:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp607);goto _LL437;
_LL468: if(_tmp5CC <= (void*)3?1:*((int*)_tmp5CC)!= 20)goto _LL437;_tmp608=((struct
Cyc_Absyn_JoinEff_struct*)_tmp5CC)->f1;_LL469: for(0;_tmp608 != 0;_tmp608=_tmp608->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)_tmp608->hd);}
goto _LL437;_LL437:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))({
struct Cyc_Std_String_pa_struct _tmp722;_tmp722.tag=0;_tmp722.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(expected_kind);{struct Cyc_Std_String_pa_struct
_tmp721;_tmp721.tag=0;_tmp721.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t));{struct Cyc_Std_String_pa_struct _tmp720;_tmp720.tag=0;_tmp720.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp71F[3]={& _tmp720,& _tmp721,&
_tmp722};Cyc_Tcutil_terr(loc,_tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof(char),51),_tag_arr(_tmp71F,sizeof(void*),3));}}}});return cvtenv;}static
struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp723=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp72C;_tmp72C.kind_env=kind_env;_tmp72C.free_vars=
0;_tmp72C.free_evars=0;_tmp72C.generalize_evars=generalize_evars;_tmp72C.fn_result=
0;_tmp72C;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp723.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp723.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp723.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp724=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp725;struct Cyc_Core_Opt**_tmp726;_LL4EE: if(_tmp724 <= (void*)3?1:*((int*)
_tmp724)!= 0)goto _LL4F0;_tmp725=((struct Cyc_Absyn_Evar_struct*)_tmp724)->f4;
_tmp726=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp724)->f4;
_LL4EF: if(*_tmp726 == 0)*_tmp726=({struct Cyc_Core_Opt*_tmp727=_cycalloc(sizeof(*
_tmp727));_tmp727->v=kind_env;_tmp727;});else{struct Cyc_List_List*_tmp728=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp726))->v;struct Cyc_List_List*
_tmp729=0;for(0;_tmp728 != 0;_tmp728=_tmp728->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp728->hd))_tmp729=({struct Cyc_List_List*_tmp72A=_cycalloc(sizeof(*_tmp72A));
_tmp72A->hd=(struct Cyc_Absyn_Tvar*)_tmp728->hd;_tmp72A->tl=_tmp729;_tmp72A;});}*
_tmp726=({struct Cyc_Core_Opt*_tmp72B=_cycalloc(sizeof(*_tmp72B));_tmp72B->v=
_tmp729;_tmp72B;});}goto _LL4ED;_LL4F0:;_LL4F1: goto _LL4ED;_LL4ED:;}}return _tmp723;}
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_Tcutil_CVTEnv _tmp72D=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp72E=_tmp72D.free_vars;struct Cyc_List_List*
_tmp72F=_tmp72D.free_evars;{struct Cyc_List_List*x=_tmp72E;for(0;x != 0;x=x->tl){
void*_tmp730=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp731;struct Cyc_Core_Opt**_tmp732;struct Cyc_Core_Opt*
_tmp733;struct Cyc_Core_Opt**_tmp734;void*_tmp735;struct Cyc_Core_Opt*_tmp736;
struct Cyc_Core_Opt**_tmp737;void*_tmp738;struct Cyc_Core_Opt*_tmp739;struct Cyc_Core_Opt**
_tmp73A;void*_tmp73B;void*_tmp73C;_LL4F3: if(*((int*)_tmp730)!= 1)goto _LL4F5;
_tmp731=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp730)->f1;_tmp732=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp730)->f1;_LL4F4: _tmp734=_tmp732;goto
_LL4F6;_LL4F5: if(*((int*)_tmp730)!= 2)goto _LL4F7;_tmp733=((struct Cyc_Absyn_Less_kb_struct*)
_tmp730)->f1;_tmp734=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp730)->f1;_tmp735=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp730)->f2;if((
int)_tmp735 != 1)goto _LL4F7;_LL4F6: _tmp737=_tmp734;goto _LL4F8;_LL4F7: if(*((int*)
_tmp730)!= 2)goto _LL4F9;_tmp736=((struct Cyc_Absyn_Less_kb_struct*)_tmp730)->f1;
_tmp737=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp730)->f1;
_tmp738=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp730)->f2;if((int)_tmp738 != 
0)goto _LL4F9;_LL4F8:*_tmp737=({struct Cyc_Core_Opt*_tmp73D=_cycalloc(sizeof(*
_tmp73D));_tmp73D->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp73E=
_cycalloc(sizeof(*_tmp73E));_tmp73E[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp73F;
_tmp73F.tag=0;_tmp73F.f1=(void*)((void*)2);_tmp73F;});_tmp73E;}));_tmp73D;});
goto _LL4F2;_LL4F9: if(*((int*)_tmp730)!= 2)goto _LL4FB;_tmp739=((struct Cyc_Absyn_Less_kb_struct*)
_tmp730)->f1;_tmp73A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp730)->f1;_tmp73B=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp730)->f2;
_LL4FA:*_tmp73A=({struct Cyc_Core_Opt*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp741=_cycalloc(sizeof(*_tmp741));
_tmp741[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp742;_tmp742.tag=0;_tmp742.f1=(void*)
_tmp73B;_tmp742;});_tmp741;}));_tmp740;});goto _LL4F2;_LL4FB: if(*((int*)_tmp730)
!= 0)goto _LL4FD;_tmp73C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp730)->f1;if((
int)_tmp73C != 1)goto _LL4FD;_LL4FC:({struct Cyc_Std_String_pa_struct _tmp744;
_tmp744.tag=0;_tmp744.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd);{void*_tmp743[1]={& _tmp744};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s cannot have kind M",
sizeof(char),36),_tag_arr(_tmp743,sizeof(void*),1));}});goto _LL4F2;_LL4FD:;
_LL4FE: goto _LL4F2;_LL4F2:;}}if(_tmp72E != 0?1: _tmp72F != 0){{void*_tmp745=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp746;struct Cyc_List_List*_tmp747;struct Cyc_List_List**
_tmp748;struct Cyc_Core_Opt*_tmp749;void*_tmp74A;struct Cyc_List_List*_tmp74B;int
_tmp74C;struct Cyc_Absyn_VarargInfo*_tmp74D;struct Cyc_List_List*_tmp74E;struct Cyc_List_List*
_tmp74F;_LL500: if(_tmp745 <= (void*)3?1:*((int*)_tmp745)!= 8)goto _LL502;_tmp746=((
struct Cyc_Absyn_FnType_struct*)_tmp745)->f1;_tmp747=_tmp746.tvars;_tmp748=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp745)->f1).tvars;
_tmp749=_tmp746.effect;_tmp74A=(void*)_tmp746.ret_typ;_tmp74B=_tmp746.args;
_tmp74C=_tmp746.c_varargs;_tmp74D=_tmp746.cyc_varargs;_tmp74E=_tmp746.rgn_po;
_tmp74F=_tmp746.attributes;_LL501: if(*_tmp748 == 0){*_tmp748=_tmp72E;_tmp72E=0;}
goto _LL4FF;_LL502:;_LL503: goto _LL4FF;_LL4FF:;}if(_tmp72E != 0)({struct Cyc_Std_String_pa_struct
_tmp751;_tmp751.tag=0;_tmp751.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)
_tmp72E->hd)->name;{void*_tmp750[1]={& _tmp751};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s",
sizeof(char),25),_tag_arr(_tmp750,sizeof(void*),1));}});if(_tmp72F != 0)for(0;
_tmp72F != 0;_tmp72F=_tmp72F->tl){void*e=(void*)_tmp72F->hd;void*_tmp752=Cyc_Tcutil_typ_kind(
e);_LL505: if((int)_tmp752 != 3)goto _LL507;_LL506: if(!Cyc_Tcutil_unify(e,(void*)2))({
void*_tmp753[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with heap!",sizeof(char),28),_tag_arr(_tmp753,sizeof(
void*),0));});goto _LL504;_LL507: if((int)_tmp752 != 4)goto _LL509;_LL508: if(!Cyc_Tcutil_unify(
e,Cyc_Absyn_empty_effect))({void*_tmp754[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(char),26),_tag_arr(_tmp754,sizeof(void*),0));});goto _LL504;_LL509:;_LL50A:({
struct Cyc_Std_String_pa_struct _tmp757;_tmp757.tag=0;_tmp757.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp756;
_tmp756.tag=0;_tmp756.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp755[2]={& _tmp756,& _tmp757};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp755,sizeof(void*),2));}}});goto _LL504;_LL504:;}}}
void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(
loc,te,t);{void*_tmp758=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp759;
struct Cyc_List_List*_tmp75A;struct Cyc_Core_Opt*_tmp75B;_LL50C: if(_tmp758 <= (void*)
3?1:*((int*)_tmp758)!= 8)goto _LL50E;_tmp759=((struct Cyc_Absyn_FnType_struct*)
_tmp758)->f1;_tmp75A=_tmp759.tvars;_tmp75B=_tmp759.effect;_LL50D: fd->tvs=_tmp75A;
fd->effect=_tmp75B;goto _LL50B;_LL50E:;_LL50F:({void*_tmp75C[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_fndecl_valid_type: not a FnType",
sizeof(char),38),_tag_arr(_tmp75C,sizeof(void*),0));});_LL50B:;}{struct
_RegionHandle _tmp75D=_new_region("r");struct _RegionHandle*r=& _tmp75D;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct _tuple13*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct _tuple13*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,_tag_arr("function declaration has repeated parameter",sizeof(char),
44));;_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp75E=_cycalloc(
sizeof(*_tmp75E));_tmp75E->v=(void*)t;_tmp75E;});}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp75F=Cyc_Tcutil_check_valid_type(loc,te,bound_tvars,expected_kind,0,t);struct
Cyc_List_List*_tmp760=Cyc_Tcutil_remove_bound_tvars(_tmp75F.free_vars,
bound_tvars);struct Cyc_List_List*_tmp761=_tmp75F.free_evars;{struct Cyc_List_List*
fs=_tmp760;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp762=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;({struct Cyc_Std_String_pa_struct _tmp765;_tmp765.tag=0;_tmp765.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp764;_tmp764.tag=0;_tmp764.f1=(struct _tagged_arr)*_tmp762;{void*_tmp763[2]={&
_tmp764,& _tmp765};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s in type %s",
sizeof(char),36),_tag_arr(_tmp763,sizeof(void*),2));}}});}}if(!allow_evars?
_tmp761 != 0: 0)for(0;_tmp761 != 0;_tmp761=_tmp761->tl){void*e=(void*)_tmp761->hd;
void*_tmp766=Cyc_Tcutil_typ_kind(e);_LL511: if((int)_tmp766 != 3)goto _LL513;_LL512:
if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp767[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(char),28),_tag_arr(_tmp767,sizeof(void*),0));});goto _LL510;_LL513: if((int)
_tmp766 != 4)goto _LL515;_LL514: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp768[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!",sizeof(char),26),_tag_arr(_tmp768,sizeof(
void*),0));});goto _LL510;_LL515:;_LL516:({struct Cyc_Std_String_pa_struct _tmp76B;
_tmp76B.tag=0;_tmp76B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp76A;_tmp76A.tag=0;_tmp76A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{
void*_tmp769[2]={& _tmp76A,& _tmp76B};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp769,sizeof(void*),2));}}});goto _LL510;_LL510:;}}
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=
Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*
tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _tagged_arr(*a2string)(void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0)({struct Cyc_Std_String_pa_struct _tmp76E;_tmp76E.tag=0;_tmp76E.f1=(
struct _tagged_arr)a2string((void*)vs->hd);{struct Cyc_Std_String_pa_struct _tmp76D;
_tmp76D.tag=0;_tmp76D.f1=(struct _tagged_arr)msg;{void*_tmp76C[2]={& _tmp76D,&
_tmp76E};Cyc_Tcutil_terr(loc,_tag_arr("%s: %s",sizeof(char),7),_tag_arr(_tmp76C,
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
fields=({struct Cyc_List_List*_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F->hd=({
struct _tuple18*_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp770->f2=0;_tmp770;});_tmp76F->tl=fields;_tmp76F;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple19 _tmp772;struct Cyc_List_List*
_tmp773;void*_tmp774;struct _tuple19*_tmp771=(struct _tuple19*)des->hd;_tmp772=*
_tmp771;_tmp773=_tmp772.f1;_tmp774=_tmp772.f2;if(_tmp773 == 0){struct Cyc_List_List*
_tmp775=fields;for(0;_tmp775 != 0;_tmp775=_tmp775->tl){if(!(*((struct _tuple18*)
_tmp775->hd)).f2){(*((struct _tuple18*)_tmp775->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp776=_cycalloc(sizeof(*_tmp776));
_tmp776->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp777=_cycalloc(
sizeof(*_tmp777));_tmp777[0]=({struct Cyc_Absyn_FieldName_struct _tmp778;_tmp778.tag=
1;_tmp778.f1=((*((struct _tuple18*)_tmp775->hd)).f1)->name;_tmp778;});_tmp777;}));
_tmp776->tl=0;_tmp776;});ans=({struct Cyc_List_List*_tmp779=_region_malloc(rgn,
sizeof(*_tmp779));_tmp779->hd=({struct _tuple20*_tmp77A=_region_malloc(rgn,
sizeof(*_tmp77A));_tmp77A->f1=(*((struct _tuple18*)_tmp775->hd)).f1;_tmp77A->f2=
_tmp774;_tmp77A;});_tmp779->tl=ans;_tmp779;});break;}}if(_tmp775 == 0)({void*
_tmp77B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too many arguments to struct",
sizeof(char),29),_tag_arr(_tmp77B,sizeof(void*),0));});}else{if(_tmp773->tl != 0)({
void*_tmp77C[0]={};Cyc_Tcutil_terr(loc,_tag_arr("multiple designators are not supported",
sizeof(char),39),_tag_arr(_tmp77C,sizeof(void*),0));});else{void*_tmp77D=(void*)
_tmp773->hd;struct _tagged_arr*_tmp77E;_LL518: if(*((int*)_tmp77D)!= 0)goto _LL51A;
_LL519:({void*_tmp77F[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array designator used in argument to struct",
sizeof(char),44),_tag_arr(_tmp77F,sizeof(void*),0));});goto _LL517;_LL51A: if(*((
int*)_tmp77D)!= 1)goto _LL517;_tmp77E=((struct Cyc_Absyn_FieldName_struct*)_tmp77D)->f1;
_LL51B: {struct Cyc_List_List*_tmp780=fields;for(0;_tmp780 != 0;_tmp780=_tmp780->tl){
if(Cyc_Std_zstrptrcmp(_tmp77E,((*((struct _tuple18*)_tmp780->hd)).f1)->name)== 0){
if((*((struct _tuple18*)_tmp780->hd)).f2)({struct Cyc_Std_String_pa_struct _tmp782;
_tmp782.tag=0;_tmp782.f1=(struct _tagged_arr)*_tmp77E;{void*_tmp781[1]={& _tmp782};
Cyc_Tcutil_terr(loc,_tag_arr("field %s has already been used as an argument",
sizeof(char),46),_tag_arr(_tmp781,sizeof(void*),1));}});(*((struct _tuple18*)
_tmp780->hd)).f2=1;ans=({struct Cyc_List_List*_tmp783=_region_malloc(rgn,sizeof(*
_tmp783));_tmp783->hd=({struct _tuple20*_tmp784=_region_malloc(rgn,sizeof(*
_tmp784));_tmp784->f1=(*((struct _tuple18*)_tmp780->hd)).f1;_tmp784->f2=_tmp774;
_tmp784;});_tmp783->tl=ans;_tmp783;});break;}}if(_tmp780 == 0)({struct Cyc_Std_String_pa_struct
_tmp786;_tmp786.tag=0;_tmp786.f1=(struct _tagged_arr)*_tmp77E;{void*_tmp785[1]={&
_tmp786};Cyc_Tcutil_terr(loc,_tag_arr("bad field designator %s",sizeof(char),24),
_tag_arr(_tmp785,sizeof(void*),1));}});goto _LL517;}_LL517:;}}}for(0;fields != 0;
fields=fields->tl){if(!(*((struct _tuple18*)fields->hd)).f2){({void*_tmp787[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments to struct",sizeof(char),28),
_tag_arr(_tmp787,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp788=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp789;void*_tmp78A;struct Cyc_Absyn_Conref*_tmp78B;_LL51D: if(_tmp788 <= (void*)3?
1:*((int*)_tmp788)!= 4)goto _LL51F;_tmp789=((struct Cyc_Absyn_PointerType_struct*)
_tmp788)->f1;_tmp78A=(void*)_tmp789.elt_typ;_tmp78B=_tmp789.bounds;_LL51E: {
struct Cyc_Absyn_Conref*_tmp78C=Cyc_Absyn_compress_conref(_tmp78B);void*_tmp78D=(
void*)(Cyc_Absyn_compress_conref(_tmp78C))->v;void*_tmp78E;_LL522: if(_tmp78D <= (
void*)1?1:*((int*)_tmp78D)!= 0)goto _LL524;_tmp78E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp78D)->f1;if((int)_tmp78E != 0)goto _LL524;_LL523:*elt_typ_dest=_tmp78A;return 1;
_LL524: if((int)_tmp78D != 0)goto _LL526;_LL525:(void*)(_tmp78C->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp78F=_cycalloc(sizeof(*_tmp78F));_tmp78F[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp790;_tmp790.tag=0;_tmp790.f1=(void*)((void*)
0);_tmp790;});_tmp78F;})));*elt_typ_dest=_tmp78A;return 1;_LL526:;_LL527: return 0;
_LL521:;}_LL51F:;_LL520: return 0;_LL51C:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp7D1;_tmp7D1.f1=0;
_tmp7D1.f2=(void*)2;_tmp7D1;});void*_tmp791=(void*)e->r;struct _tuple1*_tmp792;
void*_tmp793;struct Cyc_Absyn_Exp*_tmp794;struct _tagged_arr*_tmp795;struct Cyc_Absyn_Exp*
_tmp796;struct _tagged_arr*_tmp797;struct Cyc_Absyn_Exp*_tmp798;struct Cyc_Absyn_Exp*
_tmp799;struct Cyc_Absyn_Exp*_tmp79A;_LL529: if(*((int*)_tmp791)!= 1)goto _LL52B;
_tmp792=((struct Cyc_Absyn_Var_e_struct*)_tmp791)->f1;_tmp793=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp791)->f2;_LL52A: {void*_tmp79B=_tmp793;struct Cyc_Absyn_Vardecl*_tmp79C;
struct Cyc_Absyn_Vardecl*_tmp79D;struct Cyc_Absyn_Vardecl*_tmp79E;struct Cyc_Absyn_Vardecl*
_tmp79F;_LL536: if((int)_tmp79B != 0)goto _LL538;_LL537: goto _LL539;_LL538: if(
_tmp79B <= (void*)1?1:*((int*)_tmp79B)!= 1)goto _LL53A;_LL539: return bogus_ans;
_LL53A: if(_tmp79B <= (void*)1?1:*((int*)_tmp79B)!= 0)goto _LL53C;_tmp79C=((struct
Cyc_Absyn_Global_b_struct*)_tmp79B)->f1;_LL53B: {void*_tmp7A0=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL543: if(_tmp7A0 <= (void*)
3?1:*((int*)_tmp7A0)!= 7)goto _LL545;_LL544: return({struct _tuple10 _tmp7A1;_tmp7A1.f1=
1;_tmp7A1.f2=(void*)2;_tmp7A1;});_LL545:;_LL546: return({struct _tuple10 _tmp7A2;
_tmp7A2.f1=(_tmp79C->tq).q_const;_tmp7A2.f2=(void*)2;_tmp7A2;});_LL542:;}_LL53C:
if(_tmp79B <= (void*)1?1:*((int*)_tmp79B)!= 3)goto _LL53E;_tmp79D=((struct Cyc_Absyn_Local_b_struct*)
_tmp79B)->f1;_LL53D: {void*_tmp7A3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL548: if(_tmp7A3 <= (void*)3?1:*((int*)_tmp7A3)!= 7)
goto _LL54A;_LL549: return({struct _tuple10 _tmp7A4;_tmp7A4.f1=1;_tmp7A4.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp79D->rgn))->v;_tmp7A4;});_LL54A:;_LL54B:
_tmp79D->escapes=1;return({struct _tuple10 _tmp7A5;_tmp7A5.f1=(_tmp79D->tq).q_const;
_tmp7A5.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp79D->rgn))->v;_tmp7A5;});
_LL547:;}_LL53E: if(_tmp79B <= (void*)1?1:*((int*)_tmp79B)!= 4)goto _LL540;_tmp79E=((
struct Cyc_Absyn_Pat_b_struct*)_tmp79B)->f1;_LL53F: _tmp79F=_tmp79E;goto _LL541;
_LL540: if(_tmp79B <= (void*)1?1:*((int*)_tmp79B)!= 2)goto _LL535;_tmp79F=((struct
Cyc_Absyn_Param_b_struct*)_tmp79B)->f1;_LL541: _tmp79F->escapes=1;return({struct
_tuple10 _tmp7A6;_tmp7A6.f1=(_tmp79F->tq).q_const;_tmp7A6.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp79F->rgn))->v;_tmp7A6;});_LL535:;}_LL52B: if(*((int*)_tmp791)!= 
21)goto _LL52D;_tmp794=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp791)->f1;
_tmp795=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp791)->f2;_LL52C: {void*
_tmp7A7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp794->topt))->v);
struct Cyc_List_List*_tmp7A8;struct Cyc_Absyn_AggrInfo _tmp7A9;void*_tmp7AA;struct
Cyc_Absyn_Aggrdecl**_tmp7AB;struct Cyc_Absyn_Aggrdecl*_tmp7AC;_LL54D: if(_tmp7A7 <= (
void*)3?1:*((int*)_tmp7A7)!= 11)goto _LL54F;_tmp7A8=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp7A7)->f2;_LL54E: {struct Cyc_Absyn_Aggrfield*_tmp7AD=Cyc_Absyn_lookup_field(
_tmp7A8,_tmp795);if(_tmp7AD != 0?_tmp7AD->width != 0: 0)return({struct _tuple10
_tmp7AE;_tmp7AE.f1=(_tmp7AD->tq).q_const;_tmp7AE.f2=(Cyc_Tcutil_addressof_props(
te,_tmp794)).f2;_tmp7AE;});return bogus_ans;}_LL54F: if(_tmp7A7 <= (void*)3?1:*((
int*)_tmp7A7)!= 10)goto _LL551;_tmp7A9=((struct Cyc_Absyn_AggrType_struct*)_tmp7A7)->f1;
_tmp7AA=(void*)_tmp7A9.aggr_info;if(*((int*)_tmp7AA)!= 1)goto _LL551;_tmp7AB=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp7AA)->f1;_tmp7AC=*_tmp7AB;_LL550: {struct
Cyc_Absyn_Aggrfield*_tmp7AF=Cyc_Absyn_lookup_decl_field(_tmp7AC,_tmp795);if(
_tmp7AF != 0?_tmp7AF->width != 0: 0)return({struct _tuple10 _tmp7B0;_tmp7B0.f1=(
_tmp7AF->tq).q_const;_tmp7B0.f2=(Cyc_Tcutil_addressof_props(te,_tmp794)).f2;
_tmp7B0;});return bogus_ans;}_LL551:;_LL552: return bogus_ans;_LL54C:;}_LL52D: if(*((
int*)_tmp791)!= 22)goto _LL52F;_tmp796=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp791)->f1;_tmp797=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp791)->f2;_LL52E: {
void*_tmp7B1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp796->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7B2;void*_tmp7B3;void*_tmp7B4;
_LL554: if(_tmp7B1 <= (void*)3?1:*((int*)_tmp7B1)!= 4)goto _LL556;_tmp7B2=((struct
Cyc_Absyn_PointerType_struct*)_tmp7B1)->f1;_tmp7B3=(void*)_tmp7B2.elt_typ;
_tmp7B4=(void*)_tmp7B2.rgn_typ;_LL555: {struct Cyc_Absyn_Aggrfield*finfo;{void*
_tmp7B5=Cyc_Tcutil_compress(_tmp7B3);struct Cyc_List_List*_tmp7B6;struct Cyc_Absyn_AggrInfo
_tmp7B7;void*_tmp7B8;struct Cyc_Absyn_Aggrdecl**_tmp7B9;struct Cyc_Absyn_Aggrdecl*
_tmp7BA;_LL559: if(_tmp7B5 <= (void*)3?1:*((int*)_tmp7B5)!= 11)goto _LL55B;_tmp7B6=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp7B5)->f2;_LL55A: finfo=Cyc_Absyn_lookup_field(
_tmp7B6,_tmp797);goto _LL558;_LL55B: if(_tmp7B5 <= (void*)3?1:*((int*)_tmp7B5)!= 10)
goto _LL55D;_tmp7B7=((struct Cyc_Absyn_AggrType_struct*)_tmp7B5)->f1;_tmp7B8=(void*)
_tmp7B7.aggr_info;if(*((int*)_tmp7B8)!= 1)goto _LL55D;_tmp7B9=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp7B8)->f1;_tmp7BA=*_tmp7B9;_LL55C: finfo=Cyc_Absyn_lookup_decl_field(_tmp7BA,
_tmp797);goto _LL558;_LL55D:;_LL55E: return bogus_ans;_LL558:;}if(finfo != 0?finfo->width
!= 0: 0)return({struct _tuple10 _tmp7BB;_tmp7BB.f1=(finfo->tq).q_const;_tmp7BB.f2=
_tmp7B4;_tmp7BB;});return bogus_ans;}_LL556:;_LL557: return bogus_ans;_LL553:;}
_LL52F: if(*((int*)_tmp791)!= 20)goto _LL531;_tmp798=((struct Cyc_Absyn_Deref_e_struct*)
_tmp791)->f1;_LL530: {void*_tmp7BC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp798->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7BD;void*_tmp7BE;
struct Cyc_Absyn_Tqual _tmp7BF;_LL560: if(_tmp7BC <= (void*)3?1:*((int*)_tmp7BC)!= 4)
goto _LL562;_tmp7BD=((struct Cyc_Absyn_PointerType_struct*)_tmp7BC)->f1;_tmp7BE=(
void*)_tmp7BD.rgn_typ;_tmp7BF=_tmp7BD.tq;_LL561: return({struct _tuple10 _tmp7C0;
_tmp7C0.f1=_tmp7BF.q_const;_tmp7C0.f2=_tmp7BE;_tmp7C0;});_LL562:;_LL563: return
bogus_ans;_LL55F:;}_LL531: if(*((int*)_tmp791)!= 23)goto _LL533;_tmp799=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp791)->f1;_tmp79A=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp791)->f2;_LL532: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp799->topt))->v);void*_tmp7C1=t;struct Cyc_List_List*_tmp7C2;
struct Cyc_Absyn_PtrInfo _tmp7C3;void*_tmp7C4;void*_tmp7C5;struct Cyc_Absyn_Tqual
_tmp7C6;struct Cyc_Absyn_Conref*_tmp7C7;struct Cyc_Absyn_Tqual _tmp7C8;_LL565: if(
_tmp7C1 <= (void*)3?1:*((int*)_tmp7C1)!= 9)goto _LL567;_tmp7C2=((struct Cyc_Absyn_TupleType_struct*)
_tmp7C1)->f1;_LL566: {unsigned int _tmp7CA;int _tmp7CB;struct _tuple7 _tmp7C9=Cyc_Evexp_eval_const_uint_exp(
_tmp79A);_tmp7CA=_tmp7C9.f1;_tmp7CB=_tmp7C9.f2;if(!_tmp7CB)return bogus_ans;{
struct _tuple4*_tmp7CC=Cyc_Absyn_lookup_tuple_field(_tmp7C2,(int)_tmp7CA);if(
_tmp7CC != 0)return({struct _tuple10 _tmp7CD;_tmp7CD.f1=((*_tmp7CC).f1).q_const;
_tmp7CD.f2=(Cyc_Tcutil_addressof_props(te,_tmp799)).f2;_tmp7CD;});return
bogus_ans;}}_LL567: if(_tmp7C1 <= (void*)3?1:*((int*)_tmp7C1)!= 4)goto _LL569;
_tmp7C3=((struct Cyc_Absyn_PointerType_struct*)_tmp7C1)->f1;_tmp7C4=(void*)
_tmp7C3.elt_typ;_tmp7C5=(void*)_tmp7C3.rgn_typ;_tmp7C6=_tmp7C3.tq;_tmp7C7=
_tmp7C3.bounds;_LL568: return({struct _tuple10 _tmp7CE;_tmp7CE.f1=_tmp7C6.q_const;
_tmp7CE.f2=_tmp7C5;_tmp7CE;});_LL569: if(_tmp7C1 <= (void*)3?1:*((int*)_tmp7C1)!= 
7)goto _LL56B;_tmp7C8=((struct Cyc_Absyn_ArrayType_struct*)_tmp7C1)->f2;_LL56A:
return({struct _tuple10 _tmp7CF;_tmp7CF.f1=_tmp7C8.q_const;_tmp7CF.f2=(Cyc_Tcutil_addressof_props(
te,_tmp799)).f2;_tmp7CF;});_LL56B:;_LL56C: return bogus_ans;_LL564:;}_LL533:;
_LL534:({void*_tmp7D0[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("unary & applied to non-lvalue",
sizeof(char),30),_tag_arr(_tmp7D0,sizeof(void*),0));});return bogus_ans;_LL528:;}
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*
e){void*_tmp7D2=Cyc_Tcutil_compress(e_typ);void*_tmp7D3;struct Cyc_Absyn_Tqual
_tmp7D4;_LL56E: if(_tmp7D2 <= (void*)3?1:*((int*)_tmp7D2)!= 7)goto _LL570;_tmp7D3=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp7D2)->f1;_tmp7D4=((struct Cyc_Absyn_ArrayType_struct*)
_tmp7D2)->f2;_LL56F: {void*_tmp7D6;struct _tuple10 _tmp7D5=Cyc_Tcutil_addressof_props(
te,e);_tmp7D6=_tmp7D5.f2;return Cyc_Absyn_atb_typ(_tmp7D3,_tmp7D6,_tmp7D4,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp7D7=_cycalloc(sizeof(*_tmp7D7));_tmp7D7[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp7D8;_tmp7D8.tag=0;_tmp7D8.f1=e;_tmp7D8;});
_tmp7D7;}));}_LL570:;_LL571: return e_typ;_LL56D:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{void*_tmp7D9=(void*)b->v;void*_tmp7DA;void*_tmp7DB;struct Cyc_Absyn_Exp*
_tmp7DC;_LL573: if(_tmp7D9 <= (void*)1?1:*((int*)_tmp7D9)!= 0)goto _LL575;_tmp7DA=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7D9)->f1;if((int)_tmp7DA != 0)goto
_LL575;_LL574: return;_LL575: if(_tmp7D9 <= (void*)1?1:*((int*)_tmp7D9)!= 0)goto
_LL577;_tmp7DB=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7D9)->f1;if(
_tmp7DB <= (void*)1?1:*((int*)_tmp7DB)!= 0)goto _LL577;_tmp7DC=((struct Cyc_Absyn_Upper_b_struct*)
_tmp7DB)->f1;_LL576: {unsigned int _tmp7DE;int _tmp7DF;struct _tuple7 _tmp7DD=Cyc_Evexp_eval_const_uint_exp(
_tmp7DC);_tmp7DE=_tmp7DD.f1;_tmp7DF=_tmp7DD.f2;if(_tmp7DF?_tmp7DE <= i: 0)({struct
Cyc_Std_Int_pa_struct _tmp7E2;_tmp7E2.tag=1;_tmp7E2.f1=(int)i;{struct Cyc_Std_Int_pa_struct
_tmp7E1;_tmp7E1.tag=1;_tmp7E1.f1=(int)_tmp7DE;{void*_tmp7E0[2]={& _tmp7E1,&
_tmp7E2};Cyc_Tcutil_terr(loc,_tag_arr("dereference is out of bounds: %d <= %d",
sizeof(char),39),_tag_arr(_tmp7E0,sizeof(void*),2));}}});return;}_LL577:;_LL578:(
void*)(b->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp7E3=_cycalloc(
sizeof(*_tmp7E3));_tmp7E3[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp7E4;_tmp7E4.tag=
0;_tmp7E4.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp7E5=_cycalloc(
sizeof(*_tmp7E5));_tmp7E5[0]=({struct Cyc_Absyn_Upper_b_struct _tmp7E6;_tmp7E6.tag=
0;_tmp7E6.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmp7E6;});_tmp7E5;}));_tmp7E4;});
_tmp7E3;})));return;_LL572:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp7E7=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp7E8;struct Cyc_Absyn_Exp*_tmp7E9;_LL57A: if(_tmp7E7 <= (void*)1?1:*((int*)
_tmp7E7)!= 0)goto _LL57C;_tmp7E8=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7E7)->f1;if(_tmp7E8 <= (void*)1?1:*((int*)_tmp7E8)!= 0)goto _LL57C;_tmp7E9=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7E8)->f1;_LL57B: {unsigned int _tmp7EB;int
_tmp7EC;struct _tuple7 _tmp7EA=Cyc_Evexp_eval_const_uint_exp(_tmp7E9);_tmp7EB=
_tmp7EA.f1;_tmp7EC=_tmp7EA.f2;return _tmp7EC?_tmp7EB == 1: 0;}_LL57C:;_LL57D: return
0;_LL579:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp7ED=Cyc_Tcutil_compress(t);
void*_tmp7EE;struct Cyc_List_List*_tmp7EF;struct Cyc_Absyn_AggrInfo _tmp7F0;void*
_tmp7F1;void*_tmp7F2;struct Cyc_Absyn_AggrInfo _tmp7F3;void*_tmp7F4;void*_tmp7F5;
struct Cyc_Absyn_AggrInfo _tmp7F6;void*_tmp7F7;struct Cyc_Absyn_Aggrdecl**_tmp7F8;
struct Cyc_Absyn_Aggrdecl*_tmp7F9;struct Cyc_List_List*_tmp7FA;struct Cyc_List_List*
_tmp7FB;_LL57F: if((int)_tmp7ED != 0)goto _LL581;_LL580: goto _LL582;_LL581: if(
_tmp7ED <= (void*)3?1:*((int*)_tmp7ED)!= 5)goto _LL583;_LL582: goto _LL584;_LL583:
if((int)_tmp7ED != 1)goto _LL585;_LL584: goto _LL586;_LL585: if(_tmp7ED <= (void*)3?1:*((
int*)_tmp7ED)!= 6)goto _LL587;_LL586: return 1;_LL587: if(_tmp7ED <= (void*)3?1:*((
int*)_tmp7ED)!= 12)goto _LL589;_LL588: goto _LL58A;_LL589: if(_tmp7ED <= (void*)3?1:*((
int*)_tmp7ED)!= 13)goto _LL58B;_LL58A: return 0;_LL58B: if(_tmp7ED <= (void*)3?1:*((
int*)_tmp7ED)!= 7)goto _LL58D;_tmp7EE=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp7ED)->f1;_LL58C: return Cyc_Tcutil_bits_only(_tmp7EE);_LL58D: if(_tmp7ED <= (
void*)3?1:*((int*)_tmp7ED)!= 9)goto _LL58F;_tmp7EF=((struct Cyc_Absyn_TupleType_struct*)
_tmp7ED)->f1;_LL58E: for(0;_tmp7EF != 0;_tmp7EF=_tmp7EF->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp7EF->hd)).f2))return 0;}return 1;_LL58F: if(_tmp7ED <= (void*)3?1:*((
int*)_tmp7ED)!= 10)goto _LL591;_tmp7F0=((struct Cyc_Absyn_AggrType_struct*)_tmp7ED)->f1;
_tmp7F1=(void*)_tmp7F0.aggr_info;if(*((int*)_tmp7F1)!= 0)goto _LL591;_tmp7F2=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp7F1)->f1;if((int)_tmp7F2 != 1)
goto _LL591;_LL590: return 1;_LL591: if(_tmp7ED <= (void*)3?1:*((int*)_tmp7ED)!= 10)
goto _LL593;_tmp7F3=((struct Cyc_Absyn_AggrType_struct*)_tmp7ED)->f1;_tmp7F4=(void*)
_tmp7F3.aggr_info;if(*((int*)_tmp7F4)!= 0)goto _LL593;_tmp7F5=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp7F4)->f1;if((int)_tmp7F5 != 0)goto _LL593;_LL592: return 0;_LL593: if(_tmp7ED <= (
void*)3?1:*((int*)_tmp7ED)!= 10)goto _LL595;_tmp7F6=((struct Cyc_Absyn_AggrType_struct*)
_tmp7ED)->f1;_tmp7F7=(void*)_tmp7F6.aggr_info;if(*((int*)_tmp7F7)!= 1)goto _LL595;
_tmp7F8=((struct Cyc_Absyn_KnownAggr_struct*)_tmp7F7)->f1;_tmp7F9=*_tmp7F8;
_tmp7FA=_tmp7F6.targs;_LL594: if((void*)_tmp7F9->kind == (void*)1)return 1;if(
_tmp7F9->impl == 0)return 0;{struct _RegionHandle _tmp7FC=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp7FC;_push_region(rgn);{struct Cyc_List_List*_tmp7FD=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp7F9->tvs,_tmp7FA);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7F9->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp7FD,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp7FE=0;_npop_handler(0);
return _tmp7FE;}}}{int _tmp7FF=1;_npop_handler(0);return _tmp7FF;}};_pop_region(rgn);}
_LL595: if(_tmp7ED <= (void*)3?1:*((int*)_tmp7ED)!= 11)goto _LL597;_tmp7FB=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp7ED)->f2;_LL596: for(0;_tmp7FB != 0;_tmp7FB=
_tmp7FB->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp7FB->hd)->type))return 0;}return 1;_LL597:;_LL598: return 0;_LL57E:;}struct
_tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp800=(void*)e->r;
struct _tuple1*_tmp801;void*_tmp802;struct Cyc_Absyn_Exp*_tmp803;struct Cyc_Absyn_Exp*
_tmp804;struct Cyc_Absyn_Exp*_tmp805;struct Cyc_Absyn_Exp*_tmp806;struct Cyc_Absyn_Exp*
_tmp807;struct Cyc_Absyn_Exp*_tmp808;struct Cyc_Absyn_Exp*_tmp809;void*_tmp80A;
struct Cyc_Absyn_Exp*_tmp80B;struct Cyc_Absyn_Exp*_tmp80C;struct Cyc_Absyn_Exp*
_tmp80D;struct Cyc_Absyn_Exp*_tmp80E;struct Cyc_List_List*_tmp80F;struct Cyc_List_List*
_tmp810;struct Cyc_List_List*_tmp811;void*_tmp812;struct Cyc_List_List*_tmp813;
struct Cyc_List_List*_tmp814;struct Cyc_List_List*_tmp815;_LL59A: if(*((int*)
_tmp800)!= 0)goto _LL59C;_LL59B: goto _LL59D;_LL59C: if(*((int*)_tmp800)!= 16)goto
_LL59E;_LL59D: goto _LL59F;_LL59E: if(*((int*)_tmp800)!= 17)goto _LL5A0;_LL59F: goto
_LL5A1;_LL5A0: if(*((int*)_tmp800)!= 18)goto _LL5A2;_LL5A1: goto _LL5A3;_LL5A2: if(*((
int*)_tmp800)!= 19)goto _LL5A4;_LL5A3: goto _LL5A5;_LL5A4: if(*((int*)_tmp800)!= 31)
goto _LL5A6;_LL5A5: goto _LL5A7;_LL5A6: if(*((int*)_tmp800)!= 32)goto _LL5A8;_LL5A7:
return 1;_LL5A8: if(*((int*)_tmp800)!= 1)goto _LL5AA;_tmp801=((struct Cyc_Absyn_Var_e_struct*)
_tmp800)->f1;_tmp802=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp800)->f2;_LL5A9: {
void*_tmp816=_tmp802;struct Cyc_Absyn_Vardecl*_tmp817;_LL5C7: if(_tmp816 <= (void*)
1?1:*((int*)_tmp816)!= 1)goto _LL5C9;_LL5C8: return 1;_LL5C9: if(_tmp816 <= (void*)1?
1:*((int*)_tmp816)!= 0)goto _LL5CB;_tmp817=((struct Cyc_Absyn_Global_b_struct*)
_tmp816)->f1;_LL5CA: {void*_tmp818=Cyc_Tcutil_compress((void*)_tmp817->type);
_LL5D0: if(_tmp818 <= (void*)3?1:*((int*)_tmp818)!= 7)goto _LL5D2;_LL5D1: goto _LL5D3;
_LL5D2: if(_tmp818 <= (void*)3?1:*((int*)_tmp818)!= 8)goto _LL5D4;_LL5D3: return 1;
_LL5D4:;_LL5D5: return var_okay;_LL5CF:;}_LL5CB: if((int)_tmp816 != 0)goto _LL5CD;
_LL5CC: return 0;_LL5CD:;_LL5CE: return var_okay;_LL5C6:;}_LL5AA: if(*((int*)_tmp800)
!= 6)goto _LL5AC;_tmp803=((struct Cyc_Absyn_Conditional_e_struct*)_tmp800)->f1;
_tmp804=((struct Cyc_Absyn_Conditional_e_struct*)_tmp800)->f2;_tmp805=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp800)->f3;_LL5AB: return(Cyc_Tcutil_cnst_exp(te,0,_tmp803)?Cyc_Tcutil_cnst_exp(
te,0,_tmp804): 0)?Cyc_Tcutil_cnst_exp(te,0,_tmp805): 0;_LL5AC: if(*((int*)_tmp800)
!= 7)goto _LL5AE;_tmp806=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp800)->f1;_tmp807=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp800)->f2;_LL5AD: return Cyc_Tcutil_cnst_exp(
te,0,_tmp806)?Cyc_Tcutil_cnst_exp(te,0,_tmp807): 0;_LL5AE: if(*((int*)_tmp800)!= 
11)goto _LL5B0;_tmp808=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp800)->f1;
_LL5AF: _tmp809=_tmp808;goto _LL5B1;_LL5B0: if(*((int*)_tmp800)!= 12)goto _LL5B2;
_tmp809=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp800)->f1;_LL5B1: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp809);_LL5B2: if(*((int*)_tmp800)!= 13)goto _LL5B4;_tmp80A=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp800)->f1;_tmp80B=((struct Cyc_Absyn_Cast_e_struct*)
_tmp800)->f2;_LL5B3: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp80B);_LL5B4: if(*((
int*)_tmp800)!= 14)goto _LL5B6;_tmp80C=((struct Cyc_Absyn_Address_e_struct*)
_tmp800)->f1;_LL5B5: return Cyc_Tcutil_cnst_exp(te,1,_tmp80C);_LL5B6: if(*((int*)
_tmp800)!= 27)goto _LL5B8;_tmp80D=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp800)->f2;_tmp80E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp800)->f3;
_LL5B7: return Cyc_Tcutil_cnst_exp(te,0,_tmp80D)?Cyc_Tcutil_cnst_exp(te,0,_tmp80E):
0;_LL5B8: if(*((int*)_tmp800)!= 26)goto _LL5BA;_tmp80F=((struct Cyc_Absyn_Array_e_struct*)
_tmp800)->f1;_LL5B9: _tmp810=_tmp80F;goto _LL5BB;_LL5BA: if(*((int*)_tmp800)!= 29)
goto _LL5BC;_tmp810=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp800)->f2;_LL5BB:
_tmp811=_tmp810;goto _LL5BD;_LL5BC: if(*((int*)_tmp800)!= 28)goto _LL5BE;_tmp811=((
struct Cyc_Absyn_Struct_e_struct*)_tmp800)->f3;_LL5BD: for(0;_tmp811 != 0;_tmp811=
_tmp811->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp811->hd)).f2))
return 0;}return 1;_LL5BE: if(*((int*)_tmp800)!= 3)goto _LL5C0;_tmp812=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp800)->f1;_tmp813=((struct Cyc_Absyn_Primop_e_struct*)
_tmp800)->f2;_LL5BF: _tmp814=_tmp813;goto _LL5C1;_LL5C0: if(*((int*)_tmp800)!= 24)
goto _LL5C2;_tmp814=((struct Cyc_Absyn_Tuple_e_struct*)_tmp800)->f1;_LL5C1: _tmp815=
_tmp814;goto _LL5C3;_LL5C2: if(*((int*)_tmp800)!= 30)goto _LL5C4;_tmp815=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp800)->f1;_LL5C3: for(0;_tmp815 != 0;_tmp815=_tmp815->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp815->hd))return 0;}return 1;
_LL5C4:;_LL5C5: return 0;_LL599:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp819=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp81A;
struct Cyc_Absyn_Conref*_tmp81B;struct Cyc_Absyn_Conref*_tmp81C;void*_tmp81D;
struct Cyc_List_List*_tmp81E;struct Cyc_Absyn_AggrInfo _tmp81F;void*_tmp820;struct
Cyc_List_List*_tmp821;struct Cyc_List_List*_tmp822;_LL5D7: if((int)_tmp819 != 0)
goto _LL5D9;_LL5D8: goto _LL5DA;_LL5D9: if(_tmp819 <= (void*)3?1:*((int*)_tmp819)!= 5)
goto _LL5DB;_LL5DA: goto _LL5DC;_LL5DB: if((int)_tmp819 != 1)goto _LL5DD;_LL5DC: goto
_LL5DE;_LL5DD: if(_tmp819 <= (void*)3?1:*((int*)_tmp819)!= 6)goto _LL5DF;_LL5DE:
return 1;_LL5DF: if(_tmp819 <= (void*)3?1:*((int*)_tmp819)!= 4)goto _LL5E1;_tmp81A=((
struct Cyc_Absyn_PointerType_struct*)_tmp819)->f1;_tmp81B=_tmp81A.nullable;
_tmp81C=_tmp81A.bounds;_LL5E0: {void*_tmp823=(void*)(Cyc_Absyn_compress_conref(
_tmp81C))->v;void*_tmp824;void*_tmp825;_LL5F0: if(_tmp823 <= (void*)1?1:*((int*)
_tmp823)!= 0)goto _LL5F2;_tmp824=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp823)->f1;if((int)_tmp824 != 0)goto _LL5F2;_LL5F1: return 1;_LL5F2: if(_tmp823 <= (
void*)1?1:*((int*)_tmp823)!= 0)goto _LL5F4;_tmp825=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp823)->f1;if(_tmp825 <= (void*)1?1:*((int*)_tmp825)!= 0)goto _LL5F4;_LL5F3: {
void*_tmp826=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp81B))->v;int _tmp827;_LL5F7: if(_tmp826 <= (void*)1?1:*((int*)_tmp826)!= 0)goto
_LL5F9;_tmp827=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp826)->f1;_LL5F8:
return _tmp827;_LL5F9:;_LL5FA: return 0;_LL5F6:;}_LL5F4:;_LL5F5: return 0;_LL5EF:;}
_LL5E1: if(_tmp819 <= (void*)3?1:*((int*)_tmp819)!= 7)goto _LL5E3;_tmp81D=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp819)->f1;_LL5E2: return Cyc_Tcutil_supports_default(
_tmp81D);_LL5E3: if(_tmp819 <= (void*)3?1:*((int*)_tmp819)!= 9)goto _LL5E5;_tmp81E=((
struct Cyc_Absyn_TupleType_struct*)_tmp819)->f1;_LL5E4: for(0;_tmp81E != 0;_tmp81E=
_tmp81E->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple4*)_tmp81E->hd)).f2))
return 0;}return 1;_LL5E5: if(_tmp819 <= (void*)3?1:*((int*)_tmp819)!= 10)goto _LL5E7;
_tmp81F=((struct Cyc_Absyn_AggrType_struct*)_tmp819)->f1;_tmp820=(void*)_tmp81F.aggr_info;
_tmp821=_tmp81F.targs;_LL5E6: {struct Cyc_Absyn_Aggrdecl*_tmp828=Cyc_Absyn_get_known_aggrdecl(
_tmp820);if(_tmp828->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp828->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp828->tvs,_tmp821,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp828->impl))->fields);}
_LL5E7: if(_tmp819 <= (void*)3?1:*((int*)_tmp819)!= 11)goto _LL5E9;_tmp822=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp819)->f2;_LL5E8: return Cyc_Tcutil_fields_support_default(
0,0,_tmp822);_LL5E9: if(_tmp819 <= (void*)3?1:*((int*)_tmp819)!= 13)goto _LL5EB;
_LL5EA: goto _LL5EC;_LL5EB: if(_tmp819 <= (void*)3?1:*((int*)_tmp819)!= 12)goto
_LL5ED;_LL5EC: return 1;_LL5ED:;_LL5EE: return 0;_LL5D6:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct
_RegionHandle _tmp829=_new_region("rgn");struct _RegionHandle*rgn=& _tmp829;
_push_region(rgn);{struct Cyc_List_List*_tmp82A=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp82A,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp82B=0;_npop_handler(0);return _tmp82B;}}};_pop_region(rgn);}return 1;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp82C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp82D;void*_tmp82E;struct Cyc_Absyn_FnInfo _tmp82F;struct Cyc_List_List*_tmp830;
_LL5FC: if(_tmp82C <= (void*)3?1:*((int*)_tmp82C)!= 4)goto _LL5FE;_tmp82D=((struct
Cyc_Absyn_PointerType_struct*)_tmp82C)->f1;_tmp82E=(void*)_tmp82D.elt_typ;_LL5FD:
return Cyc_Tcutil_is_noreturn(_tmp82E);_LL5FE: if(_tmp82C <= (void*)3?1:*((int*)
_tmp82C)!= 8)goto _LL600;_tmp82F=((struct Cyc_Absyn_FnType_struct*)_tmp82C)->f1;
_tmp830=_tmp82F.attributes;_LL5FF: for(0;_tmp830 != 0;_tmp830=_tmp830->tl){void*
_tmp831=(void*)_tmp830->hd;_LL603: if((int)_tmp831 != 3)goto _LL605;_LL604: return 1;
_LL605:;_LL606: continue;_LL602:;}goto _LL5FB;_LL600:;_LL601: goto _LL5FB;_LL5FB:;}
return 0;}
