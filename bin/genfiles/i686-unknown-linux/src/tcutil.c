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
struct _RegionHandle*Cyc_Core_heap_region;typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc_Std___mbstate_t;typedef struct{int
__pos;Cyc_Std___mbstate_t __state;}Cyc_Std__G_fpos_t;typedef Cyc_Std__G_fpos_t Cyc_Std_fpos_t;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);int Cyc_Std_fprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};int Cyc_Std_printf(struct _tagged_arr,struct _tagged_arr);struct
_tagged_arr Cyc_Std_vrprintf(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_map2(void*(*f)(
void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_iter(void(*
f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(struct
Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(
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
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(
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
_tmp129,_tmp12B);_LLDB:;}static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp12C=att;_LLE5: if(_tmp12C <= (void*)16?1:*((int*)_tmp12C)!= 0)goto _LLE7;
_LLE6: return 0;_LLE7: if((int)_tmp12C != 0)goto _LLE9;_LLE8: return 1;_LLE9: if((int)
_tmp12C != 1)goto _LLEB;_LLEA: return 2;_LLEB: if((int)_tmp12C != 2)goto _LLED;_LLEC:
return 3;_LLED: if((int)_tmp12C != 3)goto _LLEF;_LLEE: return 4;_LLEF: if((int)_tmp12C
!= 4)goto _LLF1;_LLF0: return 5;_LLF1: if(_tmp12C <= (void*)16?1:*((int*)_tmp12C)!= 1)
goto _LLF3;_LLF2: return 6;_LLF3: if((int)_tmp12C != 5)goto _LLF5;_LLF4: return 7;_LLF5:
if(_tmp12C <= (void*)16?1:*((int*)_tmp12C)!= 2)goto _LLF7;_LLF6: return 8;_LLF7: if((
int)_tmp12C != 6)goto _LLF9;_LLF8: return 9;_LLF9: if((int)_tmp12C != 7)goto _LLFB;
_LLFA: return 10;_LLFB: if((int)_tmp12C != 8)goto _LLFD;_LLFC: return 11;_LLFD: if((int)
_tmp12C != 9)goto _LLFF;_LLFE: return 12;_LLFF: if((int)_tmp12C != 10)goto _LL101;
_LL100: return 13;_LL101: if((int)_tmp12C != 11)goto _LL103;_LL102: return 14;_LL103:
if((int)_tmp12C != 12)goto _LL105;_LL104: return 15;_LL105: if((int)_tmp12C != 13)goto
_LL107;_LL106: return 16;_LL107: if((int)_tmp12C != 14)goto _LL109;_LL108: return 17;
_LL109: if((int)_tmp12C != 15)goto _LL10B;_LL10A: return 18;_LL10B: if(_tmp12C <= (void*)
16?1:*((int*)_tmp12C)!= 3)goto _LL10D;_LL10C: return 19;_LL10D: if(_tmp12C <= (void*)
16?1:*((int*)_tmp12C)!= 4)goto _LLE4;_LL10E: return 20;_LLE4:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2){struct _tuple6 _tmp12E=({struct _tuple6 _tmp12D;_tmp12D.f1=att1;
_tmp12D.f2=att2;_tmp12D;});void*_tmp12F;int _tmp130;void*_tmp131;int _tmp132;void*
_tmp133;int _tmp134;void*_tmp135;int _tmp136;void*_tmp137;int _tmp138;void*_tmp139;
int _tmp13A;void*_tmp13B;struct _tagged_arr _tmp13C;void*_tmp13D;struct _tagged_arr
_tmp13E;void*_tmp13F;void*_tmp140;int _tmp141;int _tmp142;void*_tmp143;void*
_tmp144;int _tmp145;int _tmp146;_LL110: _tmp12F=_tmp12E.f1;if(_tmp12F <= (void*)16?1:*((
int*)_tmp12F)!= 0)goto _LL112;_tmp130=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp12F)->f1;_tmp131=_tmp12E.f2;if(_tmp131 <= (void*)16?1:*((int*)_tmp131)!= 0)
goto _LL112;_tmp132=((struct Cyc_Absyn_Regparm_att_struct*)_tmp131)->f1;_LL111:
_tmp134=_tmp130;_tmp136=_tmp132;goto _LL113;_LL112: _tmp133=_tmp12E.f1;if(_tmp133
<= (void*)16?1:*((int*)_tmp133)!= 4)goto _LL114;_tmp134=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp133)->f1;_tmp135=_tmp12E.f2;if(_tmp135 <= (void*)16?1:*((int*)_tmp135)!= 4)
goto _LL114;_tmp136=((struct Cyc_Absyn_Initializes_att_struct*)_tmp135)->f1;_LL113:
_tmp138=_tmp134;_tmp13A=_tmp136;goto _LL115;_LL114: _tmp137=_tmp12E.f1;if(_tmp137
<= (void*)16?1:*((int*)_tmp137)!= 1)goto _LL116;_tmp138=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp137)->f1;_tmp139=_tmp12E.f2;if(_tmp139 <= (void*)16?1:*((int*)_tmp139)!= 1)
goto _LL116;_tmp13A=((struct Cyc_Absyn_Aligned_att_struct*)_tmp139)->f1;_LL115:
return Cyc_Core_intcmp(_tmp138,_tmp13A);_LL116: _tmp13B=_tmp12E.f1;if(_tmp13B <= (
void*)16?1:*((int*)_tmp13B)!= 2)goto _LL118;_tmp13C=((struct Cyc_Absyn_Section_att_struct*)
_tmp13B)->f1;_tmp13D=_tmp12E.f2;if(_tmp13D <= (void*)16?1:*((int*)_tmp13D)!= 2)
goto _LL118;_tmp13E=((struct Cyc_Absyn_Section_att_struct*)_tmp13D)->f1;_LL117:
return Cyc_Std_strcmp(_tmp13C,_tmp13E);_LL118: _tmp13F=_tmp12E.f1;if(_tmp13F <= (
void*)16?1:*((int*)_tmp13F)!= 3)goto _LL11A;_tmp140=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp13F)->f1;_tmp141=((struct Cyc_Absyn_Format_att_struct*)_tmp13F)->f2;_tmp142=((
struct Cyc_Absyn_Format_att_struct*)_tmp13F)->f3;_tmp143=_tmp12E.f2;if(_tmp143 <= (
void*)16?1:*((int*)_tmp143)!= 3)goto _LL11A;_tmp144=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp143)->f1;_tmp145=((struct Cyc_Absyn_Format_att_struct*)_tmp143)->f2;_tmp146=((
struct Cyc_Absyn_Format_att_struct*)_tmp143)->f3;_LL119: {int _tmp147=Cyc_Core_intcmp((
int)((unsigned int)_tmp140),(int)((unsigned int)_tmp144));if(_tmp147 != 0)return
_tmp147;{int _tmp148=Cyc_Core_intcmp(_tmp141,_tmp145);if(_tmp148 != 0)return
_tmp148;return Cyc_Core_intcmp(_tmp142,_tmp146);}}_LL11A:;_LL11B: return Cyc_Core_intcmp(
Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));
_LL10F:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return
Cyc_Tcutil_rgns_of((void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv){void*t;{void*_tmp149=Cyc_Tcutil_tvar_kind(tv);_LL11D:
if((int)_tmp149 != 3)goto _LL11F;_LL11E: t=(void*)2;goto _LL11C;_LL11F: if((int)
_tmp149 != 4)goto _LL121;_LL120: t=Cyc_Absyn_empty_effect;goto _LL11C;_LL121:;_LL122:
t=Cyc_Absyn_sint_typ;goto _LL11C;_LL11C:;}return({struct _tuple8*_tmp14A=_cycalloc(
sizeof(*_tmp14A));_tmp14A->f1=tv;_tmp14A->f2=t;_tmp14A;});}static void*Cyc_Tcutil_rgns_of(
void*t){void*_tmp14B=Cyc_Tcutil_compress(t);void*_tmp14C;struct Cyc_Absyn_TunionInfo
_tmp14D;struct Cyc_List_List*_tmp14E;void*_tmp14F;struct Cyc_Absyn_PtrInfo _tmp150;
void*_tmp151;void*_tmp152;void*_tmp153;struct Cyc_List_List*_tmp154;struct Cyc_Absyn_TunionFieldInfo
_tmp155;struct Cyc_List_List*_tmp156;struct Cyc_Absyn_AggrInfo _tmp157;struct Cyc_List_List*
_tmp158;struct Cyc_List_List*_tmp159;void*_tmp15A;struct Cyc_Absyn_FnInfo _tmp15B;
struct Cyc_List_List*_tmp15C;struct Cyc_Core_Opt*_tmp15D;void*_tmp15E;struct Cyc_List_List*
_tmp15F;struct Cyc_Absyn_VarargInfo*_tmp160;struct Cyc_List_List*_tmp161;void*
_tmp162;struct Cyc_List_List*_tmp163;_LL124: if((int)_tmp14B != 0)goto _LL126;_LL125:
goto _LL127;_LL126: if((int)_tmp14B != 1)goto _LL128;_LL127: goto _LL129;_LL128: if(
_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 6)goto _LL12A;_LL129: goto _LL12B;_LL12A:
if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 12)goto _LL12C;_LL12B: goto _LL12D;
_LL12C: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 13)goto _LL12E;_LL12D: goto
_LL12F;_LL12E: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 18)goto _LL130;_LL12F:
goto _LL131;_LL130: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 5)goto _LL132;_LL131:
return Cyc_Absyn_empty_effect;_LL132: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 0)
goto _LL134;_LL133: goto _LL135;_LL134: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 1)
goto _LL136;_LL135: {void*_tmp164=Cyc_Tcutil_typ_kind(t);_LL157: if((int)_tmp164 != 
3)goto _LL159;_LL158: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp165=
_cycalloc(sizeof(*_tmp165));_tmp165[0]=({struct Cyc_Absyn_AccessEff_struct _tmp166;
_tmp166.tag=19;_tmp166.f1=(void*)t;_tmp166;});_tmp165;});_LL159: if((int)_tmp164
!= 4)goto _LL15B;_LL15A: return t;_LL15B: if((int)_tmp164 != 5)goto _LL15D;_LL15C:
return Cyc_Absyn_empty_effect;_LL15D:;_LL15E: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp168;_tmp168.tag=21;_tmp168.f1=(void*)t;_tmp168;});_tmp167;});_LL156:;}_LL136:
if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 15)goto _LL138;_tmp14C=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp14B)->f1;_LL137: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp16A;_tmp16A.tag=19;_tmp16A.f1=(void*)_tmp14C;_tmp16A;});_tmp169;});_LL138:
if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 2)goto _LL13A;_tmp14D=((struct Cyc_Absyn_TunionType_struct*)
_tmp14B)->f1;_tmp14E=_tmp14D.targs;_tmp14F=(void*)_tmp14D.rgn;_LL139: {struct Cyc_List_List*
ts=({struct Cyc_List_List*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->hd=(void*)((
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp16E=_cycalloc(sizeof(*_tmp16E));
_tmp16E[0]=({struct Cyc_Absyn_AccessEff_struct _tmp16F;_tmp16F.tag=19;_tmp16F.f1=(
void*)_tmp14F;_tmp16F;});_tmp16E;}));_tmp16D->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp14E);_tmp16D;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp16C;_tmp16C.tag=20;_tmp16C.f1=ts;_tmp16C;});_tmp16B;}));}_LL13A: if(_tmp14B <= (
void*)3?1:*((int*)_tmp14B)!= 4)goto _LL13C;_tmp150=((struct Cyc_Absyn_PointerType_struct*)
_tmp14B)->f1;_tmp151=(void*)_tmp150.elt_typ;_tmp152=(void*)_tmp150.rgn_typ;
_LL13B: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp171;_tmp171.tag=20;_tmp171.f1=({void*_tmp172[2];_tmp172[1]=Cyc_Tcutil_rgns_of(
_tmp151);_tmp172[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp173=_cycalloc(
sizeof(*_tmp173));_tmp173[0]=({struct Cyc_Absyn_AccessEff_struct _tmp174;_tmp174.tag=
19;_tmp174.f1=(void*)_tmp152;_tmp174;});_tmp173;});Cyc_List_list(_tag_arr(
_tmp172,sizeof(void*),2));});_tmp171;});_tmp170;}));_LL13C: if(_tmp14B <= (void*)3?
1:*((int*)_tmp14B)!= 7)goto _LL13E;_tmp153=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp14B)->f1;_LL13D: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp153));
_LL13E: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 9)goto _LL140;_tmp154=((struct
Cyc_Absyn_TupleType_struct*)_tmp14B)->f1;_LL13F: {struct Cyc_List_List*_tmp175=0;
for(0;_tmp154 != 0;_tmp154=_tmp154->tl){_tmp175=({struct Cyc_List_List*_tmp176=
_cycalloc(sizeof(*_tmp176));_tmp176->hd=(void*)(*((struct _tuple4*)_tmp154->hd)).f2;
_tmp176->tl=_tmp175;_tmp176;});}_tmp156=_tmp175;goto _LL141;}_LL140: if(_tmp14B <= (
void*)3?1:*((int*)_tmp14B)!= 3)goto _LL142;_tmp155=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp14B)->f1;_tmp156=_tmp155.targs;_LL141: _tmp158=_tmp156;goto _LL143;_LL142: if(
_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 10)goto _LL144;_tmp157=((struct Cyc_Absyn_AggrType_struct*)
_tmp14B)->f1;_tmp158=_tmp157.targs;_LL143: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp177=_cycalloc(sizeof(*_tmp177));
_tmp177[0]=({struct Cyc_Absyn_JoinEff_struct _tmp178;_tmp178.tag=20;_tmp178.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp158);_tmp178;});_tmp177;}));_LL144: if(_tmp14B <= (void*)3?
1:*((int*)_tmp14B)!= 11)goto _LL146;_tmp159=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14B)->f2;_LL145: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp17A;_tmp17A.tag=20;_tmp17A.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp159);_tmp17A;});
_tmp179;}));_LL146: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 14)goto _LL148;
_tmp15A=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp14B)->f1;_LL147: return
Cyc_Tcutil_rgns_of(_tmp15A);_LL148: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 17)
goto _LL14A;_LL149: return Cyc_Tcutil_rgns_of(t);_LL14A: if(_tmp14B <= (void*)3?1:*((
int*)_tmp14B)!= 8)goto _LL14C;_tmp15B=((struct Cyc_Absyn_FnType_struct*)_tmp14B)->f1;
_tmp15C=_tmp15B.tvars;_tmp15D=_tmp15B.effect;_tmp15E=(void*)_tmp15B.ret_typ;
_tmp15F=_tmp15B.args;_tmp160=_tmp15B.cyc_varargs;_tmp161=_tmp15B.rgn_po;_LL14B: {
void*_tmp17B=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp15C),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp15D))->v);return Cyc_Tcutil_normalize_effect(
_tmp17B);}_LL14C: if((int)_tmp14B != 2)goto _LL14E;_LL14D: return Cyc_Absyn_empty_effect;
_LL14E: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 19)goto _LL150;_LL14F: goto
_LL151;_LL150: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 20)goto _LL152;_LL151:
return t;_LL152: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 21)goto _LL154;_tmp162=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp14B)->f1;_LL153: return Cyc_Tcutil_rgns_of(
_tmp162);_LL154: if(_tmp14B <= (void*)3?1:*((int*)_tmp14B)!= 16)goto _LL123;_tmp163=((
struct Cyc_Absyn_TypedefType_struct*)_tmp14B)->f2;_LL155: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp17C=_cycalloc(sizeof(*_tmp17C));
_tmp17C[0]=({struct Cyc_Absyn_JoinEff_struct _tmp17D;_tmp17D.tag=20;_tmp17D.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp163);_tmp17D;});_tmp17C;}));_LL123:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp17E=e;struct Cyc_List_List*_tmp17F;
struct Cyc_List_List**_tmp180;void*_tmp181;_LL160: if(_tmp17E <= (void*)3?1:*((int*)
_tmp17E)!= 20)goto _LL162;_tmp17F=((struct Cyc_Absyn_JoinEff_struct*)_tmp17E)->f1;
_tmp180=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)_tmp17E)->f1;
_LL161: {int redo_join=0;{struct Cyc_List_List*effs=*_tmp180;for(0;effs != 0;effs=
effs->tl){void*_tmp182=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect(_tmp182)));{void*_tmp183=(void*)effs->hd;void*
_tmp184;_LL167: if(_tmp183 <= (void*)3?1:*((int*)_tmp183)!= 20)goto _LL169;_LL168:
goto _LL16A;_LL169: if(_tmp183 <= (void*)3?1:*((int*)_tmp183)!= 19)goto _LL16B;
_tmp184=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp183)->f1;if((int)_tmp184
!= 2)goto _LL16B;_LL16A: redo_join=1;goto _LL166;_LL16B:;_LL16C: goto _LL166;_LL166:;}}}
if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp180;for(0;effs != 0;effs=effs->tl){void*_tmp185=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp186;void*_tmp187;_LL16E: if(_tmp185 <= (void*)3?
1:*((int*)_tmp185)!= 20)goto _LL170;_tmp186=((struct Cyc_Absyn_JoinEff_struct*)
_tmp185)->f1;_LL16F: effects=Cyc_List_revappend(_tmp186,effects);goto _LL16D;
_LL170: if(_tmp185 <= (void*)3?1:*((int*)_tmp185)!= 19)goto _LL172;_tmp187=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp185)->f1;if((int)_tmp187 != 2)goto _LL172;
_LL171: goto _LL16D;_LL172:;_LL173: effects=({struct Cyc_List_List*_tmp188=_cycalloc(
sizeof(*_tmp188));_tmp188->hd=(void*)_tmp185;_tmp188->tl=effects;_tmp188;});goto
_LL16D;_LL16D:;}}*_tmp180=Cyc_List_imp_rev(effects);return e;}}_LL162: if(_tmp17E
<= (void*)3?1:*((int*)_tmp17E)!= 21)goto _LL164;_tmp181=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp17E)->f1;_LL163: {void*_tmp189=Cyc_Tcutil_compress(_tmp181);_LL175: if(
_tmp189 <= (void*)3?1:*((int*)_tmp189)!= 0)goto _LL177;_LL176: goto _LL178;_LL177:
if(_tmp189 <= (void*)3?1:*((int*)_tmp189)!= 1)goto _LL179;_LL178: return e;_LL179:;
_LL17A: return Cyc_Tcutil_rgns_of(_tmp181);_LL174:;}_LL164:;_LL165: return e;_LL15F:;}}
static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_struct*_tmp18A=({struct Cyc_Absyn_FnType_struct*
_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B[0]=({struct Cyc_Absyn_FnType_struct
_tmp18C;_tmp18C.tag=8;_tmp18C.f1=({struct Cyc_Absyn_FnInfo _tmp18D;_tmp18D.tvars=0;
_tmp18D.effect=({struct Cyc_Core_Opt*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->v=(
void*)eff;_tmp18E;});_tmp18D.ret_typ=(void*)((void*)0);_tmp18D.args=0;_tmp18D.c_varargs=
0;_tmp18D.cyc_varargs=0;_tmp18D.rgn_po=0;_tmp18D.attributes=0;_tmp18D;});_tmp18C;});
_tmp18B;});return Cyc_Absyn_atb_typ((void*)_tmp18A,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_bounds_one);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{void*_tmp18F=Cyc_Tcutil_compress(
e);void*_tmp190;struct Cyc_List_List*_tmp191;void*_tmp192;struct Cyc_Core_Opt*
_tmp193;struct Cyc_Core_Opt*_tmp194;struct Cyc_Core_Opt**_tmp195;struct Cyc_Core_Opt*
_tmp196;_LL17C: if(_tmp18F <= (void*)3?1:*((int*)_tmp18F)!= 19)goto _LL17E;_tmp190=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp18F)->f1;_LL17D: if(constrain)
return Cyc_Tcutil_unify(r,_tmp190);_tmp190=Cyc_Tcutil_compress(_tmp190);if(r == 
_tmp190)return 1;{struct _tuple6 _tmp198=({struct _tuple6 _tmp197;_tmp197.f1=r;
_tmp197.f2=_tmp190;_tmp197;});void*_tmp199;struct Cyc_Absyn_Tvar*_tmp19A;void*
_tmp19B;struct Cyc_Absyn_Tvar*_tmp19C;_LL187: _tmp199=_tmp198.f1;if(_tmp199 <= (
void*)3?1:*((int*)_tmp199)!= 1)goto _LL189;_tmp19A=((struct Cyc_Absyn_VarType_struct*)
_tmp199)->f1;_tmp19B=_tmp198.f2;if(_tmp19B <= (void*)3?1:*((int*)_tmp19B)!= 1)
goto _LL189;_tmp19C=((struct Cyc_Absyn_VarType_struct*)_tmp19B)->f1;_LL188: return
Cyc_Absyn_tvar_cmp(_tmp19A,_tmp19C)== 0;_LL189:;_LL18A: return 0;_LL186:;}_LL17E:
if(_tmp18F <= (void*)3?1:*((int*)_tmp18F)!= 20)goto _LL180;_tmp191=((struct Cyc_Absyn_JoinEff_struct*)
_tmp18F)->f1;_LL17F: for(0;_tmp191 != 0;_tmp191=_tmp191->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp191->hd))return 1;}return 0;_LL180: if(_tmp18F <= (void*)3?1:*((
int*)_tmp18F)!= 21)goto _LL182;_tmp192=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp18F)->f1;_LL181: {void*_tmp19D=Cyc_Tcutil_rgns_of(_tmp192);void*_tmp19E;
_LL18C: if(_tmp19D <= (void*)3?1:*((int*)_tmp19D)!= 21)goto _LL18E;_tmp19E=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp19D)->f1;_LL18D: if(!constrain)return 0;{void*
_tmp19F=Cyc_Tcutil_compress(_tmp19E);struct Cyc_Core_Opt*_tmp1A0;struct Cyc_Core_Opt*
_tmp1A1;struct Cyc_Core_Opt**_tmp1A2;struct Cyc_Core_Opt*_tmp1A3;_LL191: if(_tmp19F
<= (void*)3?1:*((int*)_tmp19F)!= 0)goto _LL193;_tmp1A0=((struct Cyc_Absyn_Evar_struct*)
_tmp19F)->f1;_tmp1A1=((struct Cyc_Absyn_Evar_struct*)_tmp19F)->f2;_tmp1A2=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp19F)->f2;_tmp1A3=((struct Cyc_Absyn_Evar_struct*)
_tmp19F)->f4;_LL192: {void*_tmp1A4=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1A3);Cyc_Tcutil_occurs(_tmp1A4,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1A3))->v,r);{void*_tmp1A5=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));
_tmp1A7[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1A8;_tmp1A8.tag=20;_tmp1A8.f1=({
void*_tmp1A9[2];_tmp1A9[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1AA=
_cycalloc(sizeof(*_tmp1AA));_tmp1AA[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1AB;
_tmp1AB.tag=19;_tmp1AB.f1=(void*)r;_tmp1AB;});_tmp1AA;});_tmp1A9[0]=_tmp1A4;Cyc_List_list(
_tag_arr(_tmp1A9,sizeof(void*),2));});_tmp1A8;});_tmp1A7;}));*_tmp1A2=({struct
Cyc_Core_Opt*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->v=(void*)_tmp1A5;
_tmp1A6;});return 1;}}_LL193:;_LL194: return 0;_LL190:;}_LL18E:;_LL18F: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp19D);_LL18B:;}_LL182: if(_tmp18F <= (void*)3?1:*((int*)_tmp18F)!= 
0)goto _LL184;_tmp193=((struct Cyc_Absyn_Evar_struct*)_tmp18F)->f1;_tmp194=((
struct Cyc_Absyn_Evar_struct*)_tmp18F)->f2;_tmp195=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp18F)->f2;_tmp196=((struct Cyc_Absyn_Evar_struct*)
_tmp18F)->f4;_LL183: if(_tmp193 == 0?1:(void*)_tmp193->v != (void*)4)({void*_tmp1AC[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(char),27),_tag_arr(_tmp1AC,sizeof(
void*),0));});if(!constrain)return 0;{void*_tmp1AD=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp196);Cyc_Tcutil_occurs(_tmp1AD,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp196))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp1AE=({struct Cyc_Absyn_JoinEff_struct*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));
_tmp1B0[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1B1;_tmp1B1.tag=20;_tmp1B1.f1=({
struct Cyc_List_List*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2->hd=(void*)
_tmp1AD;_tmp1B2->tl=({struct Cyc_List_List*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));
_tmp1B3->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1B4=_cycalloc(
sizeof(*_tmp1B4));_tmp1B4[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1B5;_tmp1B5.tag=
19;_tmp1B5.f1=(void*)r;_tmp1B5;});_tmp1B4;}));_tmp1B3->tl=0;_tmp1B3;});_tmp1B2;});
_tmp1B1;});_tmp1B0;});*_tmp195=({struct Cyc_Core_Opt*_tmp1AF=_cycalloc(sizeof(*
_tmp1AF));_tmp1AF->v=(void*)((void*)_tmp1AE);_tmp1AF;});return 1;}}_LL184:;_LL185:
return 0;_LL17B:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp1B6=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp1B7;void*_tmp1B8;struct Cyc_Core_Opt*_tmp1B9;struct
Cyc_Core_Opt*_tmp1BA;struct Cyc_Core_Opt**_tmp1BB;struct Cyc_Core_Opt*_tmp1BC;
_LL196: if(_tmp1B6 <= (void*)3?1:*((int*)_tmp1B6)!= 19)goto _LL198;_LL197: return 0;
_LL198: if(_tmp1B6 <= (void*)3?1:*((int*)_tmp1B6)!= 20)goto _LL19A;_tmp1B7=((struct
Cyc_Absyn_JoinEff_struct*)_tmp1B6)->f1;_LL199: for(0;_tmp1B7 != 0;_tmp1B7=_tmp1B7->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp1B7->hd))return 1;}
return 0;_LL19A: if(_tmp1B6 <= (void*)3?1:*((int*)_tmp1B6)!= 21)goto _LL19C;_tmp1B8=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1B6)->f1;_LL19B: _tmp1B8=Cyc_Tcutil_compress(
_tmp1B8);if(t == _tmp1B8)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,
_tmp1B8);{void*_tmp1BD=Cyc_Tcutil_rgns_of(t);void*_tmp1BE;_LL1A1: if(_tmp1BD <= (
void*)3?1:*((int*)_tmp1BD)!= 21)goto _LL1A3;_tmp1BE=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1BD)->f1;_LL1A2: {struct _tuple6 _tmp1C0=({struct _tuple6 _tmp1BF;_tmp1BF.f1=Cyc_Tcutil_compress(
_tmp1BE);_tmp1BF.f2=_tmp1B8;_tmp1BF;});void*_tmp1C1;struct Cyc_Absyn_Tvar*_tmp1C2;
void*_tmp1C3;struct Cyc_Absyn_Tvar*_tmp1C4;_LL1A6: _tmp1C1=_tmp1C0.f1;if(_tmp1C1 <= (
void*)3?1:*((int*)_tmp1C1)!= 1)goto _LL1A8;_tmp1C2=((struct Cyc_Absyn_VarType_struct*)
_tmp1C1)->f1;_tmp1C3=_tmp1C0.f2;if(_tmp1C3 <= (void*)3?1:*((int*)_tmp1C3)!= 1)
goto _LL1A8;_tmp1C4=((struct Cyc_Absyn_VarType_struct*)_tmp1C3)->f1;_LL1A7: return
Cyc_Tcutil_unify(t,_tmp1B8);_LL1A8:;_LL1A9: return _tmp1BE == _tmp1B8;_LL1A5:;}
_LL1A3:;_LL1A4: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp1BD);
_LL1A0:;}_LL19C: if(_tmp1B6 <= (void*)3?1:*((int*)_tmp1B6)!= 0)goto _LL19E;_tmp1B9=((
struct Cyc_Absyn_Evar_struct*)_tmp1B6)->f1;_tmp1BA=((struct Cyc_Absyn_Evar_struct*)
_tmp1B6)->f2;_tmp1BB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1B6)->f2;_tmp1BC=((struct Cyc_Absyn_Evar_struct*)_tmp1B6)->f4;_LL19D: if(
_tmp1B9 == 0?1:(void*)_tmp1B9->v != (void*)4)({void*_tmp1C5[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",
sizeof(char),27),_tag_arr(_tmp1C5,sizeof(void*),0));});if(!may_constrain_evars)
return 0;{void*_tmp1C6=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1BC);Cyc_Tcutil_occurs(_tmp1C6,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1BC))->v,t);{struct Cyc_Absyn_JoinEff_struct*
_tmp1C7=({struct Cyc_Absyn_JoinEff_struct*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));
_tmp1C9[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1CA;_tmp1CA.tag=20;_tmp1CA.f1=({
struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=(void*)
_tmp1C6;_tmp1CB->tl=({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));
_tmp1CC->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp1CD=_cycalloc(
sizeof(*_tmp1CD));_tmp1CD[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp1CE;_tmp1CE.tag=
21;_tmp1CE.f1=(void*)t;_tmp1CE;});_tmp1CD;}));_tmp1CC->tl=0;_tmp1CC;});_tmp1CB;});
_tmp1CA;});_tmp1C9;});*_tmp1BB=({struct Cyc_Core_Opt*_tmp1C8=_cycalloc(sizeof(*
_tmp1C8));_tmp1C8->v=(void*)((void*)_tmp1C7);_tmp1C8;});return 1;}}_LL19E:;_LL19F:
return 0;_LL195:;}}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1CF=e;struct
Cyc_Absyn_Tvar*_tmp1D0;struct Cyc_List_List*_tmp1D1;void*_tmp1D2;struct Cyc_Core_Opt*
_tmp1D3;struct Cyc_Core_Opt*_tmp1D4;struct Cyc_Core_Opt**_tmp1D5;struct Cyc_Core_Opt*
_tmp1D6;_LL1AB: if(_tmp1CF <= (void*)3?1:*((int*)_tmp1CF)!= 1)goto _LL1AD;_tmp1D0=((
struct Cyc_Absyn_VarType_struct*)_tmp1CF)->f1;_LL1AC: return Cyc_Absyn_tvar_cmp(v,
_tmp1D0)== 0;_LL1AD: if(_tmp1CF <= (void*)3?1:*((int*)_tmp1CF)!= 20)goto _LL1AF;
_tmp1D1=((struct Cyc_Absyn_JoinEff_struct*)_tmp1CF)->f1;_LL1AE: for(0;_tmp1D1 != 0;
_tmp1D1=_tmp1D1->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp1D1->hd))return 1;}return 0;_LL1AF: if(_tmp1CF <= (void*)3?1:*((int*)
_tmp1CF)!= 21)goto _LL1B1;_tmp1D2=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1CF)->f1;_LL1B0: {void*_tmp1D7=Cyc_Tcutil_rgns_of(_tmp1D2);void*_tmp1D8;
_LL1B6: if(_tmp1D7 <= (void*)3?1:*((int*)_tmp1D7)!= 21)goto _LL1B8;_tmp1D8=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1D7)->f1;_LL1B7: if(!may_constrain_evars)
return 0;{void*_tmp1D9=Cyc_Tcutil_compress(_tmp1D8);struct Cyc_Core_Opt*_tmp1DA;
struct Cyc_Core_Opt*_tmp1DB;struct Cyc_Core_Opt**_tmp1DC;struct Cyc_Core_Opt*
_tmp1DD;_LL1BB: if(_tmp1D9 <= (void*)3?1:*((int*)_tmp1D9)!= 0)goto _LL1BD;_tmp1DA=((
struct Cyc_Absyn_Evar_struct*)_tmp1D9)->f1;_tmp1DB=((struct Cyc_Absyn_Evar_struct*)
_tmp1D9)->f2;_tmp1DC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1D9)->f2;_tmp1DD=((struct Cyc_Absyn_Evar_struct*)_tmp1D9)->f4;_LL1BC: {void*
_tmp1DE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1DD);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1DD))->v,v))return 0;{void*_tmp1DF=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));
_tmp1E1[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1E2;_tmp1E2.tag=20;_tmp1E2.f1=({
void*_tmp1E3[2];_tmp1E3[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp1E4=
_cycalloc(sizeof(*_tmp1E4));_tmp1E4[0]=({struct Cyc_Absyn_VarType_struct _tmp1E5;
_tmp1E5.tag=1;_tmp1E5.f1=v;_tmp1E5;});_tmp1E4;});_tmp1E3[0]=_tmp1DE;Cyc_List_list(
_tag_arr(_tmp1E3,sizeof(void*),2));});_tmp1E2;});_tmp1E1;}));*_tmp1DC=({struct
Cyc_Core_Opt*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->v=(void*)_tmp1DF;
_tmp1E0;});return 1;}}_LL1BD:;_LL1BE: return 0;_LL1BA:;}_LL1B8:;_LL1B9: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp1D7);_LL1B5:;}_LL1B1: if(_tmp1CF <= (void*)3?1:*((int*)
_tmp1CF)!= 0)goto _LL1B3;_tmp1D3=((struct Cyc_Absyn_Evar_struct*)_tmp1CF)->f1;
_tmp1D4=((struct Cyc_Absyn_Evar_struct*)_tmp1CF)->f2;_tmp1D5=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1CF)->f2;_tmp1D6=((struct Cyc_Absyn_Evar_struct*)
_tmp1CF)->f4;_LL1B2: if(_tmp1D3 == 0?1:(void*)_tmp1D3->v != (void*)4)({void*_tmp1E6[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(char),27),_tag_arr(_tmp1E6,sizeof(
void*),0));});{void*_tmp1E7=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1D6);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D6))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct*_tmp1E8=({struct Cyc_Absyn_JoinEff_struct*_tmp1EA=
_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1EB;
_tmp1EB.tag=20;_tmp1EB.f1=({struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*
_tmp1EC));_tmp1EC->hd=(void*)_tmp1E7;_tmp1EC->tl=({struct Cyc_List_List*_tmp1ED=
_cycalloc(sizeof(*_tmp1ED));_tmp1ED->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE[0]=({struct Cyc_Absyn_VarType_struct
_tmp1EF;_tmp1EF.tag=1;_tmp1EF.f1=v;_tmp1EF;});_tmp1EE;}));_tmp1ED->tl=0;_tmp1ED;});
_tmp1EC;});_tmp1EB;});_tmp1EA;});*_tmp1D5=({struct Cyc_Core_Opt*_tmp1E9=_cycalloc(
sizeof(*_tmp1E9));_tmp1E9->v=(void*)((void*)_tmp1E8);_tmp1E9;});return 1;}}_LL1B3:;
_LL1B4: return 0;_LL1AA:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=
Cyc_Tcutil_compress(e);{void*_tmp1F0=e;struct Cyc_List_List*_tmp1F1;void*_tmp1F2;
_LL1C0: if(_tmp1F0 <= (void*)3?1:*((int*)_tmp1F0)!= 20)goto _LL1C2;_tmp1F1=((struct
Cyc_Absyn_JoinEff_struct*)_tmp1F0)->f1;_LL1C1: for(0;_tmp1F1 != 0;_tmp1F1=_tmp1F1->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp1F1->hd))return 1;}return 0;_LL1C2:
if(_tmp1F0 <= (void*)3?1:*((int*)_tmp1F0)!= 21)goto _LL1C4;_tmp1F2=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp1F0)->f1;_LL1C3: {void*_tmp1F3=Cyc_Tcutil_rgns_of(
_tmp1F2);void*_tmp1F4;_LL1C9: if(_tmp1F3 <= (void*)3?1:*((int*)_tmp1F3)!= 21)goto
_LL1CB;_tmp1F4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1F3)->f1;_LL1CA:
return 0;_LL1CB:;_LL1CC: return Cyc_Tcutil_evar_in_effect(evar,_tmp1F3);_LL1C8:;}
_LL1C4: if(_tmp1F0 <= (void*)3?1:*((int*)_tmp1F0)!= 0)goto _LL1C6;_LL1C5: return evar
== e;_LL1C6:;_LL1C7: return 0;_LL1BF:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp1F5=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp1F6;void*_tmp1F7;struct Cyc_Absyn_Tvar*_tmp1F8;void*_tmp1F9;
struct Cyc_Core_Opt*_tmp1FA;struct Cyc_Core_Opt**_tmp1FB;struct Cyc_Core_Opt*
_tmp1FC;_LL1CE: if(_tmp1F5 <= (void*)3?1:*((int*)_tmp1F5)!= 20)goto _LL1D0;_tmp1F6=((
struct Cyc_Absyn_JoinEff_struct*)_tmp1F5)->f1;_LL1CF: for(0;_tmp1F6 != 0;_tmp1F6=
_tmp1F6->tl){if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp1F6->hd,
e2))return 0;}return 1;_LL1D0: if(_tmp1F5 <= (void*)3?1:*((int*)_tmp1F5)!= 19)goto
_LL1D2;_tmp1F7=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1F5)->f1;_LL1D1:
return Cyc_Tcutil_region_in_effect(0,_tmp1F7,e2)?1:(may_constrain_evars?Cyc_Tcutil_unify(
_tmp1F7,(void*)2): 0);_LL1D2: if(_tmp1F5 <= (void*)3?1:*((int*)_tmp1F5)!= 1)goto
_LL1D4;_tmp1F8=((struct Cyc_Absyn_VarType_struct*)_tmp1F5)->f1;_LL1D3: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,_tmp1F8,e2);_LL1D4: if(_tmp1F5 <= (void*)3?1:*((int*)_tmp1F5)
!= 21)goto _LL1D6;_tmp1F9=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1F5)->f1;
_LL1D5: {void*_tmp1FD=Cyc_Tcutil_rgns_of(_tmp1F9);void*_tmp1FE;_LL1DB: if(_tmp1FD
<= (void*)3?1:*((int*)_tmp1FD)!= 21)goto _LL1DD;_tmp1FE=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1FD)->f1;_LL1DC: return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp1FE,
e2)?1:(may_constrain_evars?Cyc_Tcutil_unify(_tmp1FE,Cyc_Absyn_sint_typ): 0);
_LL1DD:;_LL1DE: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp1FD,e2);
_LL1DA:;}_LL1D6: if(_tmp1F5 <= (void*)3?1:*((int*)_tmp1F5)!= 0)goto _LL1D8;_tmp1FA=((
struct Cyc_Absyn_Evar_struct*)_tmp1F5)->f2;_tmp1FB=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1F5)->f2;_tmp1FC=((struct Cyc_Absyn_Evar_struct*)
_tmp1F5)->f4;_LL1D7: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp1FB=({struct Cyc_Core_Opt*
_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->v=(void*)Cyc_Absyn_empty_effect;
_tmp1FF;});return 1;_LL1D8:;_LL1D9:({struct Cyc_Std_String_pa_struct _tmp201;
_tmp201.tag=0;_tmp201.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e1);{void*
_tmp200[1]={& _tmp201};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("subset_effect: bad effect: %s",sizeof(char),30),_tag_arr(_tmp200,
sizeof(void*),1));}});_LL1CD:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(
0,e1,e2)?Cyc_Tcutil_subset_effect(0,e2,e1): 0)return 1;if(Cyc_Tcutil_subset_effect(
1,e1,e2)?Cyc_Tcutil_subset_effect(1,e2,e1): 0)return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple6 _tmp203;void*_tmp204;void*_tmp205;struct
_tuple6*_tmp202=(struct _tuple6*)r1->hd;_tmp203=*_tmp202;_tmp204=_tmp203.f1;
_tmp205=_tmp203.f2;{int found=_tmp204 == (void*)2;{struct Cyc_List_List*r2=rpo2;
for(0;r2 != 0?!found: 0;r2=r2->tl){struct _tuple6 _tmp207;void*_tmp208;void*_tmp209;
struct _tuple6*_tmp206=(struct _tuple6*)r2->hd;_tmp207=*_tmp206;_tmp208=_tmp207.f1;
_tmp209=_tmp207.f2;if(Cyc_Tcutil_unify(_tmp204,_tmp208)?Cyc_Tcutil_unify(_tmp205,
_tmp209): 0){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2)?Cyc_Tcutil_sub_rgnpo(rpo2,rpo1): 0;}struct _tuple11{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _tagged_arr)
_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)
return;{void*_tmp20A=t1;struct Cyc_Core_Opt*_tmp20B;struct Cyc_Core_Opt*_tmp20C;
struct Cyc_Core_Opt**_tmp20D;struct Cyc_Core_Opt*_tmp20E;_LL1E0: if(_tmp20A <= (void*)
3?1:*((int*)_tmp20A)!= 0)goto _LL1E2;_tmp20B=((struct Cyc_Absyn_Evar_struct*)
_tmp20A)->f1;_tmp20C=((struct Cyc_Absyn_Evar_struct*)_tmp20A)->f2;_tmp20D=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp20A)->f2;_tmp20E=((struct Cyc_Absyn_Evar_struct*)
_tmp20A)->f4;_LL1E1: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp20E))->v,t2);{void*_tmp20F=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp20F,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp20B))->v)){*_tmp20D=({struct Cyc_Core_Opt*_tmp210=_cycalloc(sizeof(*_tmp210));
_tmp210->v=(void*)t2;_tmp210;});return;}else{{void*_tmp211=t2;struct Cyc_Core_Opt*
_tmp212;struct Cyc_Core_Opt**_tmp213;struct Cyc_Core_Opt*_tmp214;struct Cyc_Absyn_PtrInfo
_tmp215;_LL1E5: if(_tmp211 <= (void*)3?1:*((int*)_tmp211)!= 0)goto _LL1E7;_tmp212=((
struct Cyc_Absyn_Evar_struct*)_tmp211)->f2;_tmp213=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp211)->f2;_tmp214=((struct Cyc_Absyn_Evar_struct*)
_tmp211)->f4;_LL1E6: {struct Cyc_List_List*_tmp216=(struct Cyc_List_List*)_tmp20E->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp214))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp216,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=
_tag_arr("(type variable would escape scope)",sizeof(char),35);(int)_throw((void*)
Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((void*)_tmp20B->v,_tmp20F)){*_tmp213=({
struct Cyc_Core_Opt*_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217->v=(void*)t1;
_tmp217;});return;}Cyc_Tcutil_failure_reason=_tag_arr("(kinds are incompatible)",
sizeof(char),25);goto _LL1E4;}_LL1E7: if(_tmp211 <= (void*)3?1:*((int*)_tmp211)!= 4)
goto _LL1E9;_tmp215=((struct Cyc_Absyn_PointerType_struct*)_tmp211)->f1;if(!((void*)
_tmp20B->v == (void*)2))goto _LL1E9;_LL1E8: {struct Cyc_Absyn_Conref*_tmp218=Cyc_Absyn_compress_conref(
_tmp215.bounds);{void*_tmp219=(void*)_tmp218->v;_LL1EC: if((int)_tmp219 != 0)goto
_LL1EE;_LL1ED:(void*)(_tmp218->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp21B;_tmp21B.tag=0;_tmp21B.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp21D;_tmp21D.tag=0;_tmp21D.f1=Cyc_Absyn_signed_int_exp(1,0);_tmp21D;});
_tmp21C;}));_tmp21B;});_tmp21A;})));*_tmp20D=({struct Cyc_Core_Opt*_tmp21E=
_cycalloc(sizeof(*_tmp21E));_tmp21E->v=(void*)t2;_tmp21E;});return;_LL1EE:;
_LL1EF: goto _LL1EB;_LL1EB:;}goto _LL1E4;}_LL1E9:;_LL1EA: goto _LL1E4;_LL1E4:;}Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}
_LL1E2:;_LL1E3: goto _LL1DF;_LL1DF:;}{struct _tuple6 _tmp220=({struct _tuple6 _tmp21F;
_tmp21F.f1=t2;_tmp21F.f2=t1;_tmp21F;});void*_tmp221;void*_tmp222;void*_tmp223;
void*_tmp224;struct Cyc_Absyn_Tvar*_tmp225;void*_tmp226;struct Cyc_Absyn_Tvar*
_tmp227;void*_tmp228;struct Cyc_Absyn_AggrInfo _tmp229;void*_tmp22A;struct Cyc_List_List*
_tmp22B;void*_tmp22C;struct Cyc_Absyn_AggrInfo _tmp22D;void*_tmp22E;struct Cyc_List_List*
_tmp22F;void*_tmp230;struct _tuple1*_tmp231;void*_tmp232;struct _tuple1*_tmp233;
void*_tmp234;struct Cyc_List_List*_tmp235;void*_tmp236;struct Cyc_List_List*
_tmp237;void*_tmp238;struct Cyc_Absyn_TunionInfo _tmp239;void*_tmp23A;struct Cyc_Absyn_Tuniondecl**
_tmp23B;struct Cyc_Absyn_Tuniondecl*_tmp23C;struct Cyc_List_List*_tmp23D;void*
_tmp23E;void*_tmp23F;struct Cyc_Absyn_TunionInfo _tmp240;void*_tmp241;struct Cyc_Absyn_Tuniondecl**
_tmp242;struct Cyc_Absyn_Tuniondecl*_tmp243;struct Cyc_List_List*_tmp244;void*
_tmp245;void*_tmp246;struct Cyc_Absyn_TunionFieldInfo _tmp247;void*_tmp248;struct
Cyc_Absyn_Tuniondecl*_tmp249;struct Cyc_Absyn_Tunionfield*_tmp24A;struct Cyc_List_List*
_tmp24B;void*_tmp24C;struct Cyc_Absyn_TunionFieldInfo _tmp24D;void*_tmp24E;struct
Cyc_Absyn_Tuniondecl*_tmp24F;struct Cyc_Absyn_Tunionfield*_tmp250;struct Cyc_List_List*
_tmp251;void*_tmp252;struct Cyc_Absyn_PtrInfo _tmp253;void*_tmp254;void*_tmp255;
struct Cyc_Absyn_Conref*_tmp256;struct Cyc_Absyn_Tqual _tmp257;struct Cyc_Absyn_Conref*
_tmp258;void*_tmp259;struct Cyc_Absyn_PtrInfo _tmp25A;void*_tmp25B;void*_tmp25C;
struct Cyc_Absyn_Conref*_tmp25D;struct Cyc_Absyn_Tqual _tmp25E;struct Cyc_Absyn_Conref*
_tmp25F;void*_tmp260;void*_tmp261;void*_tmp262;void*_tmp263;void*_tmp264;void*
_tmp265;void*_tmp266;void*_tmp267;void*_tmp268;int _tmp269;void*_tmp26A;int
_tmp26B;void*_tmp26C;void*_tmp26D;void*_tmp26E;void*_tmp26F;void*_tmp270;int
_tmp271;void*_tmp272;int _tmp273;void*_tmp274;void*_tmp275;void*_tmp276;void*
_tmp277;void*_tmp278;void*_tmp279;struct Cyc_Absyn_Tqual _tmp27A;struct Cyc_Absyn_Exp*
_tmp27B;void*_tmp27C;void*_tmp27D;struct Cyc_Absyn_Tqual _tmp27E;struct Cyc_Absyn_Exp*
_tmp27F;void*_tmp280;struct Cyc_Absyn_FnInfo _tmp281;struct Cyc_List_List*_tmp282;
struct Cyc_Core_Opt*_tmp283;void*_tmp284;struct Cyc_List_List*_tmp285;int _tmp286;
struct Cyc_Absyn_VarargInfo*_tmp287;struct Cyc_List_List*_tmp288;struct Cyc_List_List*
_tmp289;void*_tmp28A;struct Cyc_Absyn_FnInfo _tmp28B;struct Cyc_List_List*_tmp28C;
struct Cyc_Core_Opt*_tmp28D;void*_tmp28E;struct Cyc_List_List*_tmp28F;int _tmp290;
struct Cyc_Absyn_VarargInfo*_tmp291;struct Cyc_List_List*_tmp292;struct Cyc_List_List*
_tmp293;void*_tmp294;struct Cyc_List_List*_tmp295;void*_tmp296;struct Cyc_List_List*
_tmp297;void*_tmp298;void*_tmp299;struct Cyc_List_List*_tmp29A;void*_tmp29B;void*
_tmp29C;struct Cyc_List_List*_tmp29D;void*_tmp29E;void*_tmp29F;void*_tmp2A0;void*
_tmp2A1;void*_tmp2A2;void*_tmp2A3;void*_tmp2A4;void*_tmp2A5;void*_tmp2A6;void*
_tmp2A7;void*_tmp2A8;void*_tmp2A9;_LL1F1: _tmp221=_tmp220.f1;if(_tmp221 <= (void*)
3?1:*((int*)_tmp221)!= 0)goto _LL1F3;_LL1F2: Cyc_Tcutil_unify_it(t2,t1);return;
_LL1F3: _tmp222=_tmp220.f1;if((int)_tmp222 != 0)goto _LL1F5;_tmp223=_tmp220.f2;if((
int)_tmp223 != 0)goto _LL1F5;_LL1F4: return;_LL1F5: _tmp224=_tmp220.f1;if(_tmp224 <= (
void*)3?1:*((int*)_tmp224)!= 1)goto _LL1F7;_tmp225=((struct Cyc_Absyn_VarType_struct*)
_tmp224)->f1;_tmp226=_tmp220.f2;if(_tmp226 <= (void*)3?1:*((int*)_tmp226)!= 1)
goto _LL1F7;_tmp227=((struct Cyc_Absyn_VarType_struct*)_tmp226)->f1;_LL1F6: {
struct _tagged_arr*_tmp2AA=_tmp225->name;struct _tagged_arr*_tmp2AB=_tmp227->name;
int _tmp2AC=*((int*)_check_null(_tmp225->identity));int _tmp2AD=*((int*)
_check_null(_tmp227->identity));void*_tmp2AE=Cyc_Tcutil_tvar_kind(_tmp225);void*
_tmp2AF=Cyc_Tcutil_tvar_kind(_tmp227);if(_tmp2AD == _tmp2AC?Cyc_Std_zstrptrcmp(
_tmp2AA,_tmp2AB)== 0: 0){if(_tmp2AE != _tmp2AF)({struct Cyc_Std_String_pa_struct
_tmp2B3;_tmp2B3.tag=0;_tmp2B3.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2AF);{struct Cyc_Std_String_pa_struct _tmp2B2;_tmp2B2.tag=0;_tmp2B2.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2AE);{struct Cyc_Std_String_pa_struct
_tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(struct _tagged_arr)*_tmp2AA;{void*_tmp2B0[3]={&
_tmp2B1,& _tmp2B2,& _tmp2B3};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("same type variable %s has kinds %s and %s",sizeof(
char),42),_tag_arr(_tmp2B0,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=
_tag_arr("(variable types are not the same)",sizeof(char),34);goto _LL1F0;}_LL1F7:
_tmp228=_tmp220.f1;if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 10)goto _LL1F9;
_tmp229=((struct Cyc_Absyn_AggrType_struct*)_tmp228)->f1;_tmp22A=(void*)_tmp229.aggr_info;
_tmp22B=_tmp229.targs;_tmp22C=_tmp220.f2;if(_tmp22C <= (void*)3?1:*((int*)_tmp22C)
!= 10)goto _LL1F9;_tmp22D=((struct Cyc_Absyn_AggrType_struct*)_tmp22C)->f1;_tmp22E=(
void*)_tmp22D.aggr_info;_tmp22F=_tmp22D.targs;_LL1F8: {void*_tmp2B5;struct
_tuple1*_tmp2B6;struct _tuple5 _tmp2B4=Cyc_Absyn_aggr_kinded_name(_tmp22E);_tmp2B5=
_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;{void*_tmp2B8;struct _tuple1*_tmp2B9;struct _tuple5
_tmp2B7=Cyc_Absyn_aggr_kinded_name(_tmp22A);_tmp2B8=_tmp2B7.f1;_tmp2B9=_tmp2B7.f2;
if(_tmp2B5 != _tmp2B8){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1F0;}if(Cyc_Absyn_qvar_cmp(_tmp2B6,_tmp2B9)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different type name)",sizeof(char),22);goto _LL1F0;}Cyc_Tcutil_unify_list(
_tmp22F,_tmp22B);return;}}_LL1F9: _tmp230=_tmp220.f1;if(_tmp230 <= (void*)3?1:*((
int*)_tmp230)!= 12)goto _LL1FB;_tmp231=((struct Cyc_Absyn_EnumType_struct*)_tmp230)->f1;
_tmp232=_tmp220.f2;if(_tmp232 <= (void*)3?1:*((int*)_tmp232)!= 12)goto _LL1FB;
_tmp233=((struct Cyc_Absyn_EnumType_struct*)_tmp232)->f1;_LL1FA: if(Cyc_Absyn_qvar_cmp(
_tmp231,_tmp233)== 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different enum types)",
sizeof(char),23);goto _LL1F0;_LL1FB: _tmp234=_tmp220.f1;if(_tmp234 <= (void*)3?1:*((
int*)_tmp234)!= 13)goto _LL1FD;_tmp235=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp234)->f1;_tmp236=_tmp220.f2;if(_tmp236 <= (void*)3?1:*((int*)_tmp236)!= 13)
goto _LL1FD;_tmp237=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp236)->f1;_LL1FC: {
int bad=0;for(0;_tmp235 != 0?_tmp237 != 0: 0;(_tmp235=_tmp235->tl,_tmp237=_tmp237->tl)){
struct Cyc_Absyn_Enumfield*_tmp2BA=(struct Cyc_Absyn_Enumfield*)_tmp235->hd;struct
Cyc_Absyn_Enumfield*_tmp2BB=(struct Cyc_Absyn_Enumfield*)_tmp237->hd;if(Cyc_Absyn_qvar_cmp(
_tmp2BA->name,_tmp2BB->name)!= 0){Cyc_Tcutil_failure_reason=_tag_arr("(different names for enum fields)",
sizeof(char),34);bad=1;break;}if(_tmp2BA->tag == _tmp2BB->tag)continue;if(_tmp2BA->tag
== 0?1: _tmp2BB->tag == 0){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(char),39);bad=1;break;}if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp2BA->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp2BB->tag))){Cyc_Tcutil_failure_reason=
_tag_arr("(different tag values for enum fields)",sizeof(char),39);bad=1;break;}}
if(bad)goto _LL1F0;if(_tmp235 == 0?_tmp237 == 0: 0)return;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of fields for enums)",sizeof(char),39);goto _LL1F0;}
_LL1FD: _tmp238=_tmp220.f1;if(_tmp238 <= (void*)3?1:*((int*)_tmp238)!= 2)goto
_LL1FF;_tmp239=((struct Cyc_Absyn_TunionType_struct*)_tmp238)->f1;_tmp23A=(void*)
_tmp239.tunion_info;if(*((int*)_tmp23A)!= 1)goto _LL1FF;_tmp23B=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp23A)->f1;_tmp23C=*_tmp23B;_tmp23D=_tmp239.targs;_tmp23E=(void*)_tmp239.rgn;
_tmp23F=_tmp220.f2;if(_tmp23F <= (void*)3?1:*((int*)_tmp23F)!= 2)goto _LL1FF;
_tmp240=((struct Cyc_Absyn_TunionType_struct*)_tmp23F)->f1;_tmp241=(void*)_tmp240.tunion_info;
if(*((int*)_tmp241)!= 1)goto _LL1FF;_tmp242=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp241)->f1;_tmp243=*_tmp242;_tmp244=_tmp240.targs;_tmp245=(void*)_tmp240.rgn;
_LL1FE: if(_tmp23C == _tmp243?1: Cyc_Absyn_qvar_cmp(_tmp23C->name,_tmp243->name)== 
0){Cyc_Tcutil_unify_it(_tmp245,_tmp23E);Cyc_Tcutil_unify_list(_tmp244,_tmp23D);
return;}Cyc_Tcutil_failure_reason=_tag_arr("(different tunion types)",sizeof(
char),25);goto _LL1F0;_LL1FF: _tmp246=_tmp220.f1;if(_tmp246 <= (void*)3?1:*((int*)
_tmp246)!= 3)goto _LL201;_tmp247=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp246)->f1;_tmp248=(void*)_tmp247.field_info;if(*((int*)_tmp248)!= 1)goto
_LL201;_tmp249=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp248)->f1;_tmp24A=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp248)->f2;_tmp24B=_tmp247.targs;
_tmp24C=_tmp220.f2;if(_tmp24C <= (void*)3?1:*((int*)_tmp24C)!= 3)goto _LL201;
_tmp24D=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp24C)->f1;_tmp24E=(void*)
_tmp24D.field_info;if(*((int*)_tmp24E)!= 1)goto _LL201;_tmp24F=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp24E)->f1;_tmp250=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp24E)->f2;
_tmp251=_tmp24D.targs;_LL200: if((_tmp249 == _tmp24F?1: Cyc_Absyn_qvar_cmp(_tmp249->name,
_tmp24F->name)== 0)?_tmp24A == _tmp250?1: Cyc_Absyn_qvar_cmp(_tmp24A->name,_tmp250->name)
== 0: 0){Cyc_Tcutil_unify_list(_tmp251,_tmp24B);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)",sizeof(char),31);goto _LL1F0;_LL201:
_tmp252=_tmp220.f1;if(_tmp252 <= (void*)3?1:*((int*)_tmp252)!= 4)goto _LL203;
_tmp253=((struct Cyc_Absyn_PointerType_struct*)_tmp252)->f1;_tmp254=(void*)
_tmp253.elt_typ;_tmp255=(void*)_tmp253.rgn_typ;_tmp256=_tmp253.nullable;_tmp257=
_tmp253.tq;_tmp258=_tmp253.bounds;_tmp259=_tmp220.f2;if(_tmp259 <= (void*)3?1:*((
int*)_tmp259)!= 4)goto _LL203;_tmp25A=((struct Cyc_Absyn_PointerType_struct*)
_tmp259)->f1;_tmp25B=(void*)_tmp25A.elt_typ;_tmp25C=(void*)_tmp25A.rgn_typ;
_tmp25D=_tmp25A.nullable;_tmp25E=_tmp25A.tq;_tmp25F=_tmp25A.bounds;_LL202: Cyc_Tcutil_unify_it(
_tmp25B,_tmp254);Cyc_Tcutil_unify_it(_tmp255,_tmp25C);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_unify_tqual(_tmp25E,_tmp257);Cyc_Tcutil_unify_it_conrefs(
Cyc_Tcutil_boundscmp,_tmp25F,_tmp258,_tag_arr("(different pointer bounds)",
sizeof(char),27));{void*_tmp2BC=(void*)(Cyc_Absyn_compress_conref(_tmp25F))->v;
void*_tmp2BD;_LL22A: if(_tmp2BC <= (void*)1?1:*((int*)_tmp2BC)!= 0)goto _LL22C;
_tmp2BD=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp2BC)->f1;if((int)_tmp2BD
!= 0)goto _LL22C;_LL22B: return;_LL22C:;_LL22D: goto _LL229;_LL229:;}((void(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp25D,_tmp256,
_tag_arr("(different pointer types)",sizeof(char),26));return;_LL203: _tmp260=
_tmp220.f1;if(_tmp260 <= (void*)3?1:*((int*)_tmp260)!= 5)goto _LL205;_tmp261=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp260)->f1;_tmp262=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp260)->f2;_tmp263=_tmp220.f2;if(_tmp263 <= (void*)3?1:*((int*)_tmp263)!= 5)
goto _LL205;_tmp264=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp263)->f1;_tmp265=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp263)->f2;_LL204: if(_tmp264 == _tmp261?
_tmp265 == _tmp262: 0)return;Cyc_Tcutil_failure_reason=_tag_arr("(different integral types)",
sizeof(char),27);goto _LL1F0;_LL205: _tmp266=_tmp220.f1;if((int)_tmp266 != 1)goto
_LL207;_tmp267=_tmp220.f2;if((int)_tmp267 != 1)goto _LL207;_LL206: return;_LL207:
_tmp268=_tmp220.f1;if(_tmp268 <= (void*)3?1:*((int*)_tmp268)!= 6)goto _LL209;
_tmp269=((struct Cyc_Absyn_DoubleType_struct*)_tmp268)->f1;_tmp26A=_tmp220.f2;if(
_tmp26A <= (void*)3?1:*((int*)_tmp26A)!= 6)goto _LL209;_tmp26B=((struct Cyc_Absyn_DoubleType_struct*)
_tmp26A)->f1;_LL208: if(_tmp269 == _tmp26B)return;goto _LL1F0;_LL209: _tmp26C=
_tmp220.f1;if(_tmp26C <= (void*)3?1:*((int*)_tmp26C)!= 14)goto _LL20B;_tmp26D=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp26C)->f1;_tmp26E=_tmp220.f2;if(
_tmp26E <= (void*)3?1:*((int*)_tmp26E)!= 14)goto _LL20B;_tmp26F=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp26E)->f1;_LL20A: Cyc_Tcutil_unify_it(_tmp26D,_tmp26F);return;_LL20B: _tmp270=
_tmp220.f1;if(_tmp270 <= (void*)3?1:*((int*)_tmp270)!= 18)goto _LL20D;_tmp271=((
struct Cyc_Absyn_TypeInt_struct*)_tmp270)->f1;_tmp272=_tmp220.f2;if(_tmp272 <= (
void*)3?1:*((int*)_tmp272)!= 18)goto _LL20D;_tmp273=((struct Cyc_Absyn_TypeInt_struct*)
_tmp272)->f1;_LL20C: if(_tmp271 == _tmp273)return;Cyc_Tcutil_failure_reason=
_tag_arr("(different type integers)",sizeof(char),26);goto _LL1F0;_LL20D: _tmp274=
_tmp220.f1;if(_tmp274 <= (void*)3?1:*((int*)_tmp274)!= 17)goto _LL20F;_tmp275=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp274)->f1;_tmp276=_tmp220.f2;if(
_tmp276 <= (void*)3?1:*((int*)_tmp276)!= 17)goto _LL20F;_tmp277=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp276)->f1;_LL20E: Cyc_Tcutil_unify_it(_tmp275,_tmp277);return;_LL20F: _tmp278=
_tmp220.f1;if(_tmp278 <= (void*)3?1:*((int*)_tmp278)!= 7)goto _LL211;_tmp279=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp278)->f1;_tmp27A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp278)->f2;_tmp27B=((struct Cyc_Absyn_ArrayType_struct*)_tmp278)->f3;_tmp27C=
_tmp220.f2;if(_tmp27C <= (void*)3?1:*((int*)_tmp27C)!= 7)goto _LL211;_tmp27D=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp27C)->f1;_tmp27E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp27C)->f2;_tmp27F=((struct Cyc_Absyn_ArrayType_struct*)_tmp27C)->f3;_LL210: Cyc_Tcutil_unify_tqual(
_tmp27E,_tmp27A);Cyc_Tcutil_unify_it(_tmp27D,_tmp279);if(_tmp27B == _tmp27F)
return;if(_tmp27B == 0?1: _tmp27F == 0)goto _LL1F0;if(Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp27B),(struct Cyc_Absyn_Exp*)_check_null(
_tmp27F)))return;Cyc_Tcutil_failure_reason=_tag_arr("(different array sizes)",
sizeof(char),24);goto _LL1F0;_LL211: _tmp280=_tmp220.f1;if(_tmp280 <= (void*)3?1:*((
int*)_tmp280)!= 8)goto _LL213;_tmp281=((struct Cyc_Absyn_FnType_struct*)_tmp280)->f1;
_tmp282=_tmp281.tvars;_tmp283=_tmp281.effect;_tmp284=(void*)_tmp281.ret_typ;
_tmp285=_tmp281.args;_tmp286=_tmp281.c_varargs;_tmp287=_tmp281.cyc_varargs;
_tmp288=_tmp281.rgn_po;_tmp289=_tmp281.attributes;_tmp28A=_tmp220.f2;if(_tmp28A
<= (void*)3?1:*((int*)_tmp28A)!= 8)goto _LL213;_tmp28B=((struct Cyc_Absyn_FnType_struct*)
_tmp28A)->f1;_tmp28C=_tmp28B.tvars;_tmp28D=_tmp28B.effect;_tmp28E=(void*)_tmp28B.ret_typ;
_tmp28F=_tmp28B.args;_tmp290=_tmp28B.c_varargs;_tmp291=_tmp28B.cyc_varargs;
_tmp292=_tmp28B.rgn_po;_tmp293=_tmp28B.attributes;_LL212: {int done=0;{struct
_RegionHandle _tmp2BE=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2BE;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp28C != 0){if(_tmp282 == 0){
Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too few type variables)",
sizeof(char),50);(int)_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp2BF=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp28C->hd);void*_tmp2C0=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp282->hd);if(_tmp2BF != _tmp2C0){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp2C4;_tmp2C4.tag=0;_tmp2C4.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2C0);{struct Cyc_Std_String_pa_struct
_tmp2C3;_tmp2C3.tag=0;_tmp2C3.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2BF);{struct Cyc_Std_String_pa_struct _tmp2C2;_tmp2C2.tag=0;_tmp2C2.f1=(struct
_tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp28C->hd);{void*
_tmp2C1[3]={& _tmp2C2,& _tmp2C3,& _tmp2C4};Cyc_Std_aprintf(_tag_arr("(type var %s has different kinds %s and %s)",
sizeof(char),44),_tag_arr(_tmp2C1,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp2C5=_region_malloc(rgn,sizeof(*_tmp2C5));_tmp2C5->hd=({
struct _tuple8*_tmp2C6=_region_malloc(rgn,sizeof(*_tmp2C6));_tmp2C6->f1=(struct
Cyc_Absyn_Tvar*)_tmp282->hd;_tmp2C6->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp2C7=_cycalloc(sizeof(*_tmp2C7));_tmp2C7[0]=({struct Cyc_Absyn_VarType_struct
_tmp2C8;_tmp2C8.tag=1;_tmp2C8.f1=(struct Cyc_Absyn_Tvar*)_tmp28C->hd;_tmp2C8;});
_tmp2C7;});_tmp2C6;});_tmp2C5->tl=inst;_tmp2C5;});_tmp28C=_tmp28C->tl;_tmp282=
_tmp282->tl;}}if(_tmp282 != 0){Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too many type variables)",
sizeof(char),51);_npop_handler(0);goto _LL1F0;}if(inst != 0){Cyc_Tcutil_unify_it((
void*)({struct Cyc_Absyn_FnType_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));
_tmp2C9[0]=({struct Cyc_Absyn_FnType_struct _tmp2CA;_tmp2CA.tag=8;_tmp2CA.f1=({
struct Cyc_Absyn_FnInfo _tmp2CB;_tmp2CB.tvars=0;_tmp2CB.effect=_tmp28D;_tmp2CB.ret_typ=(
void*)_tmp28E;_tmp2CB.args=_tmp28F;_tmp2CB.c_varargs=_tmp290;_tmp2CB.cyc_varargs=
_tmp291;_tmp2CB.rgn_po=_tmp292;_tmp2CB.attributes=_tmp293;_tmp2CB;});_tmp2CA;});
_tmp2C9;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC[0]=({struct Cyc_Absyn_FnType_struct
_tmp2CD;_tmp2CD.tag=8;_tmp2CD.f1=({struct Cyc_Absyn_FnInfo _tmp2CE;_tmp2CE.tvars=0;
_tmp2CE.effect=_tmp283;_tmp2CE.ret_typ=(void*)_tmp284;_tmp2CE.args=_tmp285;
_tmp2CE.c_varargs=_tmp286;_tmp2CE.cyc_varargs=_tmp287;_tmp2CE.rgn_po=_tmp288;
_tmp2CE.attributes=_tmp289;_tmp2CE;});_tmp2CD;});_tmp2CC;})));done=1;}};
_pop_region(rgn);}if(done)return;Cyc_Tcutil_unify_it(_tmp28E,_tmp284);for(0;
_tmp28F != 0?_tmp285 != 0: 0;(_tmp28F=_tmp28F->tl,_tmp285=_tmp285->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)_tmp28F->hd)).f2,(*((struct _tuple2*)_tmp285->hd)).f2);Cyc_Tcutil_unify_it((*((
struct _tuple2*)_tmp28F->hd)).f3,(*((struct _tuple2*)_tmp285->hd)).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp28F != 0?1: _tmp285 != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)",sizeof(char),52);
goto _LL1F0;}if(_tmp290 != _tmp286){Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes C varargs)",
sizeof(char),41);goto _LL1F0;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp2D0=({
struct _tuple11 _tmp2CF;_tmp2CF.f1=_tmp291;_tmp2CF.f2=_tmp287;_tmp2CF;});struct Cyc_Absyn_VarargInfo*
_tmp2D1;struct Cyc_Absyn_VarargInfo*_tmp2D2;struct Cyc_Absyn_VarargInfo*_tmp2D3;
struct Cyc_Absyn_VarargInfo*_tmp2D4;struct Cyc_Absyn_VarargInfo*_tmp2D5;struct Cyc_Absyn_VarargInfo
_tmp2D6;struct Cyc_Core_Opt*_tmp2D7;struct Cyc_Absyn_Tqual _tmp2D8;void*_tmp2D9;int
_tmp2DA;struct Cyc_Absyn_VarargInfo*_tmp2DB;struct Cyc_Absyn_VarargInfo _tmp2DC;
struct Cyc_Core_Opt*_tmp2DD;struct Cyc_Absyn_Tqual _tmp2DE;void*_tmp2DF;int _tmp2E0;
_LL22F: _tmp2D1=_tmp2D0.f1;if(_tmp2D1 != 0)goto _LL231;_tmp2D2=_tmp2D0.f2;if(
_tmp2D2 != 0)goto _LL231;_LL230: goto _LL22E;_LL231: _tmp2D3=_tmp2D0.f1;if(_tmp2D3 != 
0)goto _LL233;_LL232: goto _LL234;_LL233: _tmp2D4=_tmp2D0.f2;if(_tmp2D4 != 0)goto
_LL235;_LL234: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes varargs)",
sizeof(char),39);goto _LL22E;_LL235: _tmp2D5=_tmp2D0.f1;if(_tmp2D5 == 0)goto _LL22E;
_tmp2D6=*_tmp2D5;_tmp2D7=_tmp2D6.name;_tmp2D8=_tmp2D6.tq;_tmp2D9=(void*)_tmp2D6.type;
_tmp2DA=_tmp2D6.inject;_tmp2DB=_tmp2D0.f2;if(_tmp2DB == 0)goto _LL22E;_tmp2DC=*
_tmp2DB;_tmp2DD=_tmp2DC.name;_tmp2DE=_tmp2DC.tq;_tmp2DF=(void*)_tmp2DC.type;
_tmp2E0=_tmp2DC.inject;_LL236: Cyc_Tcutil_unify_tqual(_tmp2D8,_tmp2DE);Cyc_Tcutil_unify_it(
_tmp2D9,_tmp2DF);if(_tmp2DA != _tmp2E0){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type injects varargs)",sizeof(char),41);}goto _LL22E;
_LL22E:;}if(bad_cyc_vararg)goto _LL1F0;{int bad_effect=0;{struct _tuple12 _tmp2E2=({
struct _tuple12 _tmp2E1;_tmp2E1.f1=_tmp28D;_tmp2E1.f2=_tmp283;_tmp2E1;});struct Cyc_Core_Opt*
_tmp2E3;struct Cyc_Core_Opt*_tmp2E4;struct Cyc_Core_Opt*_tmp2E5;struct Cyc_Core_Opt*
_tmp2E6;_LL238: _tmp2E3=_tmp2E2.f1;if(_tmp2E3 != 0)goto _LL23A;_tmp2E4=_tmp2E2.f2;
if(_tmp2E4 != 0)goto _LL23A;_LL239: goto _LL237;_LL23A: _tmp2E5=_tmp2E2.f1;if(_tmp2E5
!= 0)goto _LL23C;_LL23B: goto _LL23D;_LL23C: _tmp2E6=_tmp2E2.f2;if(_tmp2E6 != 0)goto
_LL23E;_LL23D: bad_effect=1;goto _LL237;_LL23E:;_LL23F: bad_effect=!Cyc_Tcutil_unify_effect((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp28D))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp283))->v);goto _LL237;_LL237:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=_tag_arr("(function type effects do not unify)",
sizeof(char),37);goto _LL1F0;}if(!Cyc_Tcutil_same_atts(_tmp289,_tmp293)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different attributes)",sizeof(char),43);goto
_LL1F0;}if(!Cyc_Tcutil_same_rgn_po(_tmp288,_tmp292)){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different region lifetime orderings)",sizeof(char),
58);goto _LL1F0;}return;}}}_LL213: _tmp294=_tmp220.f1;if(_tmp294 <= (void*)3?1:*((
int*)_tmp294)!= 9)goto _LL215;_tmp295=((struct Cyc_Absyn_TupleType_struct*)_tmp294)->f1;
_tmp296=_tmp220.f2;if(_tmp296 <= (void*)3?1:*((int*)_tmp296)!= 9)goto _LL215;
_tmp297=((struct Cyc_Absyn_TupleType_struct*)_tmp296)->f1;_LL214: for(0;_tmp297 != 
0?_tmp295 != 0: 0;(_tmp297=_tmp297->tl,_tmp295=_tmp295->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)_tmp297->hd)).f1,(*((struct _tuple4*)_tmp295->hd)).f1);Cyc_Tcutil_unify_it((*((
struct _tuple4*)_tmp297->hd)).f2,(*((struct _tuple4*)_tmp295->hd)).f2);}if(_tmp297
== 0?_tmp295 == 0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(tuple types have different numbers of components)",sizeof(char),51);
goto _LL1F0;_LL215: _tmp298=_tmp220.f1;if(_tmp298 <= (void*)3?1:*((int*)_tmp298)!= 
11)goto _LL217;_tmp299=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp298)->f1;
_tmp29A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp298)->f2;_tmp29B=_tmp220.f2;
if(_tmp29B <= (void*)3?1:*((int*)_tmp29B)!= 11)goto _LL217;_tmp29C=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp29B)->f1;_tmp29D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp29B)->f2;_LL216: if(_tmp29C != _tmp299){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(char),24);goto _LL1F0;}for(0;_tmp29D != 0?_tmp29A != 0: 0;(_tmp29D=_tmp29D->tl,
_tmp29A=_tmp29A->tl)){struct Cyc_Absyn_Aggrfield*_tmp2E7=(struct Cyc_Absyn_Aggrfield*)
_tmp29D->hd;struct Cyc_Absyn_Aggrfield*_tmp2E8=(struct Cyc_Absyn_Aggrfield*)
_tmp29A->hd;if(Cyc_Std_zstrptrcmp(_tmp2E7->name,_tmp2E8->name)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different member names)",sizeof(char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual(_tmp2E7->tq,_tmp2E8->tq);Cyc_Tcutil_unify_it((void*)
_tmp2E7->type,(void*)_tmp2E8->type);if(!Cyc_Tcutil_same_atts(_tmp2E7->attributes,
_tmp2E8->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different attributes on member)",sizeof(char),33);(int)_throw((void*)
Cyc_Tcutil_Unify);}if(((_tmp2E7->width != 0?_tmp2E8->width == 0: 0)?1:(_tmp2E8->width
!= 0?_tmp2E7->width == 0: 0))?1:((_tmp2E7->width != 0?_tmp2E8->width != 0: 0)?!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2E7->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp2E8->width)): 0)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_failure_reason=_tag_arr("(different bitfield widths on member)",
sizeof(char),38);(int)_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp29D == 0?_tmp29A == 
0: 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of members)",sizeof(char),30);goto _LL1F0;_LL217:
_tmp29E=_tmp220.f1;if((int)_tmp29E != 2)goto _LL219;_tmp29F=_tmp220.f2;if((int)
_tmp29F != 2)goto _LL219;_LL218: return;_LL219: _tmp2A0=_tmp220.f1;if(_tmp2A0 <= (
void*)3?1:*((int*)_tmp2A0)!= 15)goto _LL21B;_tmp2A1=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp2A0)->f1;_tmp2A2=_tmp220.f2;if(_tmp2A2 <= (void*)3?1:*((int*)_tmp2A2)!= 15)
goto _LL21B;_tmp2A3=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2A2)->f1;
_LL21A: Cyc_Tcutil_unify_it(_tmp2A1,_tmp2A3);return;_LL21B: _tmp2A4=_tmp220.f1;if(
_tmp2A4 <= (void*)3?1:*((int*)_tmp2A4)!= 20)goto _LL21D;_LL21C: goto _LL21E;_LL21D:
_tmp2A5=_tmp220.f2;if(_tmp2A5 <= (void*)3?1:*((int*)_tmp2A5)!= 20)goto _LL21F;
_LL21E: goto _LL220;_LL21F: _tmp2A6=_tmp220.f1;if(_tmp2A6 <= (void*)3?1:*((int*)
_tmp2A6)!= 19)goto _LL221;_LL220: goto _LL222;_LL221: _tmp2A7=_tmp220.f1;if(_tmp2A7
<= (void*)3?1:*((int*)_tmp2A7)!= 21)goto _LL223;_LL222: goto _LL224;_LL223: _tmp2A8=
_tmp220.f2;if(_tmp2A8 <= (void*)3?1:*((int*)_tmp2A8)!= 21)goto _LL225;_LL224: goto
_LL226;_LL225: _tmp2A9=_tmp220.f2;if(_tmp2A9 <= (void*)3?1:*((int*)_tmp2A9)!= 19)
goto _LL227;_LL226: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)",sizeof(char),22);goto _LL1F0;_LL227:;_LL228: goto
_LL1F0;_LL1F0:;}(int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0?a2 != 0: 0)return -
1;if(a1 != 0?a2 == 0: 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(
a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){int _tmp2E9=(tq1.q_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int
_tmp2EA=(tq2.q_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp2E9,_tmp2EA);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp2EB=(void*)x->v;void*
_tmp2EC;_LL241: if((int)_tmp2EB != 0)goto _LL243;_LL242: return - 1;_LL243: if(_tmp2EB
<= (void*)1?1:*((int*)_tmp2EB)!= 0)goto _LL245;_tmp2EC=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2EB)->f1;_LL244: {void*_tmp2ED=(void*)y->v;void*_tmp2EE;_LL248: if((int)
_tmp2ED != 0)goto _LL24A;_LL249: return 1;_LL24A: if(_tmp2ED <= (void*)1?1:*((int*)
_tmp2ED)!= 0)goto _LL24C;_tmp2EE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2ED)->f1;_LL24B: return cmp(_tmp2EC,_tmp2EE);_LL24C: if(_tmp2ED <= (void*)1?1:*((
int*)_tmp2ED)!= 1)goto _LL247;_LL24D:({void*_tmp2EF[0]={};Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)",sizeof(char),40),_tag_arr(
_tmp2EF,sizeof(void*),0));});_LL247:;}_LL245: if(_tmp2EB <= (void*)1?1:*((int*)
_tmp2EB)!= 1)goto _LL240;_LL246:({void*_tmp2F0[0]={};Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress",
sizeof(char),37),_tag_arr(_tmp2F0,sizeof(void*),0));});_LL240:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp2F2;struct Cyc_Absyn_Tqual
_tmp2F3;void*_tmp2F4;struct _tuple4*_tmp2F1=tqt1;_tmp2F2=*_tmp2F1;_tmp2F3=_tmp2F2.f1;
_tmp2F4=_tmp2F2.f2;{struct _tuple4 _tmp2F6;struct Cyc_Absyn_Tqual _tmp2F7;void*
_tmp2F8;struct _tuple4*_tmp2F5=tqt2;_tmp2F6=*_tmp2F5;_tmp2F7=_tmp2F6.f1;_tmp2F8=
_tmp2F6.f2;{int _tmp2F9=Cyc_Tcutil_tqual_cmp(_tmp2F3,_tmp2F7);if(_tmp2F9 != 0)
return _tmp2F9;return Cyc_Tcutil_typecmp(_tmp2F4,_tmp2F8);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp2FA=Cyc_Std_zstrptrcmp(
f1->name,f2->name);if(_tmp2FA != 0)return _tmp2FA;{int _tmp2FB=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp2FB != 0)return _tmp2FB;{int _tmp2FC=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp2FC != 0)return _tmp2FC;{int _tmp2FD=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp2FD != 0)return
_tmp2FD;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp2FE=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp2FE != 0)return _tmp2FE;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp2FF=t;_LL24F: if((int)_tmp2FF != 0)goto _LL251;_LL250: return 0;
_LL251: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 0)goto _LL253;_LL252: return 1;
_LL253: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 1)goto _LL255;_LL254: return 2;
_LL255: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 2)goto _LL257;_LL256: return 3;
_LL257: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 3)goto _LL259;_LL258: return 4;
_LL259: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 4)goto _LL25B;_LL25A: return 5;
_LL25B: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 5)goto _LL25D;_LL25C: return 6;
_LL25D: if((int)_tmp2FF != 1)goto _LL25F;_LL25E: return 7;_LL25F: if(_tmp2FF <= (void*)
3?1:*((int*)_tmp2FF)!= 6)goto _LL261;_LL260: return 8;_LL261: if(_tmp2FF <= (void*)3?
1:*((int*)_tmp2FF)!= 7)goto _LL263;_LL262: return 9;_LL263: if(_tmp2FF <= (void*)3?1:*((
int*)_tmp2FF)!= 8)goto _LL265;_LL264: return 10;_LL265: if(_tmp2FF <= (void*)3?1:*((
int*)_tmp2FF)!= 9)goto _LL267;_LL266: return 11;_LL267: if(_tmp2FF <= (void*)3?1:*((
int*)_tmp2FF)!= 10)goto _LL269;_LL268: return 12;_LL269: if(_tmp2FF <= (void*)3?1:*((
int*)_tmp2FF)!= 11)goto _LL26B;_LL26A: return 14;_LL26B: if(_tmp2FF <= (void*)3?1:*((
int*)_tmp2FF)!= 12)goto _LL26D;_LL26C: return 16;_LL26D: if(_tmp2FF <= (void*)3?1:*((
int*)_tmp2FF)!= 13)goto _LL26F;_LL26E: return 17;_LL26F: if(_tmp2FF <= (void*)3?1:*((
int*)_tmp2FF)!= 15)goto _LL271;_LL270: return 18;_LL271: if(_tmp2FF <= (void*)3?1:*((
int*)_tmp2FF)!= 16)goto _LL273;_LL272: return 19;_LL273: if((int)_tmp2FF != 2)goto
_LL275;_LL274: return 20;_LL275: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 19)goto
_LL277;_LL276: return 21;_LL277: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 20)goto
_LL279;_LL278: return 22;_LL279: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 21)goto
_LL27B;_LL27A: return 23;_LL27B: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 14)goto
_LL27D;_LL27C: return 24;_LL27D: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 18)goto
_LL27F;_LL27E: return 25;_LL27F: if(_tmp2FF <= (void*)3?1:*((int*)_tmp2FF)!= 17)goto
_LL24E;_LL280: return 26;_LL24E:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp300=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp300 != 0)
return _tmp300;{struct _tuple6 _tmp302=({struct _tuple6 _tmp301;_tmp301.f1=t2;_tmp301.f2=
t1;_tmp301;});void*_tmp303;void*_tmp304;void*_tmp305;void*_tmp306;void*_tmp307;
struct Cyc_Absyn_Tvar*_tmp308;void*_tmp309;struct Cyc_Absyn_Tvar*_tmp30A;void*
_tmp30B;struct Cyc_Absyn_AggrInfo _tmp30C;void*_tmp30D;struct Cyc_List_List*_tmp30E;
void*_tmp30F;struct Cyc_Absyn_AggrInfo _tmp310;void*_tmp311;struct Cyc_List_List*
_tmp312;void*_tmp313;struct _tuple1*_tmp314;void*_tmp315;struct _tuple1*_tmp316;
void*_tmp317;struct Cyc_List_List*_tmp318;void*_tmp319;struct Cyc_List_List*
_tmp31A;void*_tmp31B;struct Cyc_Absyn_TunionInfo _tmp31C;void*_tmp31D;struct Cyc_Absyn_Tuniondecl**
_tmp31E;struct Cyc_Absyn_Tuniondecl*_tmp31F;struct Cyc_List_List*_tmp320;void*
_tmp321;void*_tmp322;struct Cyc_Absyn_TunionInfo _tmp323;void*_tmp324;struct Cyc_Absyn_Tuniondecl**
_tmp325;struct Cyc_Absyn_Tuniondecl*_tmp326;struct Cyc_List_List*_tmp327;void*
_tmp328;void*_tmp329;struct Cyc_Absyn_TunionFieldInfo _tmp32A;void*_tmp32B;struct
Cyc_Absyn_Tuniondecl*_tmp32C;struct Cyc_Absyn_Tunionfield*_tmp32D;struct Cyc_List_List*
_tmp32E;void*_tmp32F;struct Cyc_Absyn_TunionFieldInfo _tmp330;void*_tmp331;struct
Cyc_Absyn_Tuniondecl*_tmp332;struct Cyc_Absyn_Tunionfield*_tmp333;struct Cyc_List_List*
_tmp334;void*_tmp335;struct Cyc_Absyn_PtrInfo _tmp336;void*_tmp337;void*_tmp338;
struct Cyc_Absyn_Conref*_tmp339;struct Cyc_Absyn_Tqual _tmp33A;struct Cyc_Absyn_Conref*
_tmp33B;void*_tmp33C;struct Cyc_Absyn_PtrInfo _tmp33D;void*_tmp33E;void*_tmp33F;
struct Cyc_Absyn_Conref*_tmp340;struct Cyc_Absyn_Tqual _tmp341;struct Cyc_Absyn_Conref*
_tmp342;void*_tmp343;void*_tmp344;void*_tmp345;void*_tmp346;void*_tmp347;void*
_tmp348;void*_tmp349;void*_tmp34A;void*_tmp34B;int _tmp34C;void*_tmp34D;int
_tmp34E;void*_tmp34F;void*_tmp350;struct Cyc_Absyn_Tqual _tmp351;struct Cyc_Absyn_Exp*
_tmp352;void*_tmp353;void*_tmp354;struct Cyc_Absyn_Tqual _tmp355;struct Cyc_Absyn_Exp*
_tmp356;void*_tmp357;struct Cyc_Absyn_FnInfo _tmp358;struct Cyc_List_List*_tmp359;
struct Cyc_Core_Opt*_tmp35A;void*_tmp35B;struct Cyc_List_List*_tmp35C;int _tmp35D;
struct Cyc_Absyn_VarargInfo*_tmp35E;struct Cyc_List_List*_tmp35F;struct Cyc_List_List*
_tmp360;void*_tmp361;struct Cyc_Absyn_FnInfo _tmp362;struct Cyc_List_List*_tmp363;
struct Cyc_Core_Opt*_tmp364;void*_tmp365;struct Cyc_List_List*_tmp366;int _tmp367;
struct Cyc_Absyn_VarargInfo*_tmp368;struct Cyc_List_List*_tmp369;struct Cyc_List_List*
_tmp36A;void*_tmp36B;struct Cyc_List_List*_tmp36C;void*_tmp36D;struct Cyc_List_List*
_tmp36E;void*_tmp36F;void*_tmp370;struct Cyc_List_List*_tmp371;void*_tmp372;void*
_tmp373;struct Cyc_List_List*_tmp374;void*_tmp375;void*_tmp376;void*_tmp377;void*
_tmp378;void*_tmp379;void*_tmp37A;void*_tmp37B;void*_tmp37C;void*_tmp37D;void*
_tmp37E;void*_tmp37F;void*_tmp380;void*_tmp381;void*_tmp382;void*_tmp383;int
_tmp384;void*_tmp385;int _tmp386;void*_tmp387;void*_tmp388;void*_tmp389;void*
_tmp38A;void*_tmp38B;void*_tmp38C;_LL282: _tmp303=_tmp302.f1;if(_tmp303 <= (void*)
3?1:*((int*)_tmp303)!= 0)goto _LL284;_tmp304=_tmp302.f2;if(_tmp304 <= (void*)3?1:*((
int*)_tmp304)!= 0)goto _LL284;_LL283:({void*_tmp38D[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: can only compare closed types",
sizeof(char),39),_tag_arr(_tmp38D,sizeof(void*),0));});_LL284: _tmp305=_tmp302.f1;
if((int)_tmp305 != 0)goto _LL286;_tmp306=_tmp302.f2;if((int)_tmp306 != 0)goto _LL286;
_LL285: return 0;_LL286: _tmp307=_tmp302.f1;if(_tmp307 <= (void*)3?1:*((int*)_tmp307)
!= 1)goto _LL288;_tmp308=((struct Cyc_Absyn_VarType_struct*)_tmp307)->f1;_tmp309=
_tmp302.f2;if(_tmp309 <= (void*)3?1:*((int*)_tmp309)!= 1)goto _LL288;_tmp30A=((
struct Cyc_Absyn_VarType_struct*)_tmp309)->f1;_LL287: return Cyc_Core_intcmp(*((int*)
_check_null(_tmp30A->identity)),*((int*)_check_null(_tmp308->identity)));_LL288:
_tmp30B=_tmp302.f1;if(_tmp30B <= (void*)3?1:*((int*)_tmp30B)!= 10)goto _LL28A;
_tmp30C=((struct Cyc_Absyn_AggrType_struct*)_tmp30B)->f1;_tmp30D=(void*)_tmp30C.aggr_info;
_tmp30E=_tmp30C.targs;_tmp30F=_tmp302.f2;if(_tmp30F <= (void*)3?1:*((int*)_tmp30F)
!= 10)goto _LL28A;_tmp310=((struct Cyc_Absyn_AggrType_struct*)_tmp30F)->f1;_tmp311=(
void*)_tmp310.aggr_info;_tmp312=_tmp310.targs;_LL289: {struct _tuple1*_tmp38F;
struct _tuple5 _tmp38E=Cyc_Absyn_aggr_kinded_name(_tmp30D);_tmp38F=_tmp38E.f2;{
struct _tuple1*_tmp391;struct _tuple5 _tmp390=Cyc_Absyn_aggr_kinded_name(_tmp311);
_tmp391=_tmp390.f2;{int _tmp392=Cyc_Absyn_qvar_cmp(_tmp38F,_tmp391);if(_tmp392 != 
0)return _tmp392;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp30E,_tmp312);}}}}
_LL28A: _tmp313=_tmp302.f1;if(_tmp313 <= (void*)3?1:*((int*)_tmp313)!= 12)goto
_LL28C;_tmp314=((struct Cyc_Absyn_EnumType_struct*)_tmp313)->f1;_tmp315=_tmp302.f2;
if(_tmp315 <= (void*)3?1:*((int*)_tmp315)!= 12)goto _LL28C;_tmp316=((struct Cyc_Absyn_EnumType_struct*)
_tmp315)->f1;_LL28B: return Cyc_Absyn_qvar_cmp(_tmp314,_tmp316);_LL28C: _tmp317=
_tmp302.f1;if(_tmp317 <= (void*)3?1:*((int*)_tmp317)!= 13)goto _LL28E;_tmp318=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp317)->f1;_tmp319=_tmp302.f2;if(_tmp319
<= (void*)3?1:*((int*)_tmp319)!= 13)goto _LL28E;_tmp31A=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp319)->f1;_LL28D: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct
Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp318,_tmp31A);_LL28E: _tmp31B=_tmp302.f1;if(_tmp31B <= (
void*)3?1:*((int*)_tmp31B)!= 2)goto _LL290;_tmp31C=((struct Cyc_Absyn_TunionType_struct*)
_tmp31B)->f1;_tmp31D=(void*)_tmp31C.tunion_info;if(*((int*)_tmp31D)!= 1)goto
_LL290;_tmp31E=((struct Cyc_Absyn_KnownTunion_struct*)_tmp31D)->f1;_tmp31F=*
_tmp31E;_tmp320=_tmp31C.targs;_tmp321=(void*)_tmp31C.rgn;_tmp322=_tmp302.f2;if(
_tmp322 <= (void*)3?1:*((int*)_tmp322)!= 2)goto _LL290;_tmp323=((struct Cyc_Absyn_TunionType_struct*)
_tmp322)->f1;_tmp324=(void*)_tmp323.tunion_info;if(*((int*)_tmp324)!= 1)goto
_LL290;_tmp325=((struct Cyc_Absyn_KnownTunion_struct*)_tmp324)->f1;_tmp326=*
_tmp325;_tmp327=_tmp323.targs;_tmp328=(void*)_tmp323.rgn;_LL28F: if(_tmp326 == 
_tmp31F)return 0;{int _tmp393=Cyc_Absyn_qvar_cmp(_tmp326->name,_tmp31F->name);if(
_tmp393 != 0)return _tmp393;{int _tmp394=Cyc_Tcutil_typecmp(_tmp328,_tmp321);if(
_tmp394 != 0)return _tmp394;return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp327,
_tmp320);}}_LL290: _tmp329=_tmp302.f1;if(_tmp329 <= (void*)3?1:*((int*)_tmp329)!= 
3)goto _LL292;_tmp32A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp329)->f1;
_tmp32B=(void*)_tmp32A.field_info;if(*((int*)_tmp32B)!= 1)goto _LL292;_tmp32C=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp32B)->f1;_tmp32D=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp32B)->f2;_tmp32E=_tmp32A.targs;_tmp32F=_tmp302.f2;if(_tmp32F <= (void*)3?1:*((
int*)_tmp32F)!= 3)goto _LL292;_tmp330=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp32F)->f1;_tmp331=(void*)_tmp330.field_info;if(*((int*)_tmp331)!= 1)goto
_LL292;_tmp332=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp331)->f1;_tmp333=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp331)->f2;_tmp334=_tmp330.targs;
_LL291: if(_tmp332 == _tmp32C)return 0;{int _tmp395=Cyc_Absyn_qvar_cmp(_tmp32C->name,
_tmp332->name);if(_tmp395 != 0)return _tmp395;{int _tmp396=Cyc_Absyn_qvar_cmp(
_tmp32D->name,_tmp333->name);if(_tmp396 != 0)return _tmp396;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp334,_tmp32E);}}_LL292: _tmp335=_tmp302.f1;if(_tmp335 <= (
void*)3?1:*((int*)_tmp335)!= 4)goto _LL294;_tmp336=((struct Cyc_Absyn_PointerType_struct*)
_tmp335)->f1;_tmp337=(void*)_tmp336.elt_typ;_tmp338=(void*)_tmp336.rgn_typ;
_tmp339=_tmp336.nullable;_tmp33A=_tmp336.tq;_tmp33B=_tmp336.bounds;_tmp33C=
_tmp302.f2;if(_tmp33C <= (void*)3?1:*((int*)_tmp33C)!= 4)goto _LL294;_tmp33D=((
struct Cyc_Absyn_PointerType_struct*)_tmp33C)->f1;_tmp33E=(void*)_tmp33D.elt_typ;
_tmp33F=(void*)_tmp33D.rgn_typ;_tmp340=_tmp33D.nullable;_tmp341=_tmp33D.tq;
_tmp342=_tmp33D.bounds;_LL293: {int _tmp397=Cyc_Tcutil_typecmp(_tmp33E,_tmp337);
if(_tmp397 != 0)return _tmp397;{int _tmp398=Cyc_Tcutil_typecmp(_tmp33F,_tmp338);if(
_tmp398 != 0)return _tmp398;{int _tmp399=Cyc_Tcutil_tqual_cmp(_tmp341,_tmp33A);if(
_tmp399 != 0)return _tmp399;{int _tmp39A=Cyc_Tcutil_conrefs_cmp(Cyc_Tcutil_boundscmp,
_tmp342,_tmp33B);if(_tmp39A != 0)return _tmp39A;{void*_tmp39B=(void*)(Cyc_Absyn_compress_conref(
_tmp342))->v;void*_tmp39C;_LL2BB: if(_tmp39B <= (void*)1?1:*((int*)_tmp39B)!= 0)
goto _LL2BD;_tmp39C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp39B)->f1;if((
int)_tmp39C != 0)goto _LL2BD;_LL2BC: return 0;_LL2BD:;_LL2BE: goto _LL2BA;_LL2BA:;}
return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp340,_tmp339);}}}}_LL294: _tmp343=
_tmp302.f1;if(_tmp343 <= (void*)3?1:*((int*)_tmp343)!= 5)goto _LL296;_tmp344=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp343)->f1;_tmp345=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp343)->f2;_tmp346=_tmp302.f2;if(_tmp346 <= (void*)3?1:*((int*)_tmp346)!= 5)
goto _LL296;_tmp347=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp346)->f1;_tmp348=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp346)->f2;_LL295: if(_tmp347 != _tmp344)
return Cyc_Core_intcmp((int)((unsigned int)_tmp347),(int)((unsigned int)_tmp344));
if(_tmp348 != _tmp345)return Cyc_Core_intcmp((int)((unsigned int)_tmp348),(int)((
unsigned int)_tmp345));return 0;_LL296: _tmp349=_tmp302.f1;if((int)_tmp349 != 1)
goto _LL298;_tmp34A=_tmp302.f2;if((int)_tmp34A != 1)goto _LL298;_LL297: return 0;
_LL298: _tmp34B=_tmp302.f1;if(_tmp34B <= (void*)3?1:*((int*)_tmp34B)!= 6)goto
_LL29A;_tmp34C=((struct Cyc_Absyn_DoubleType_struct*)_tmp34B)->f1;_tmp34D=_tmp302.f2;
if(_tmp34D <= (void*)3?1:*((int*)_tmp34D)!= 6)goto _LL29A;_tmp34E=((struct Cyc_Absyn_DoubleType_struct*)
_tmp34D)->f1;_LL299: if(_tmp34C == _tmp34E)return 0;else{if(_tmp34C)return - 1;else{
return 1;}}_LL29A: _tmp34F=_tmp302.f1;if(_tmp34F <= (void*)3?1:*((int*)_tmp34F)!= 7)
goto _LL29C;_tmp350=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp34F)->f1;
_tmp351=((struct Cyc_Absyn_ArrayType_struct*)_tmp34F)->f2;_tmp352=((struct Cyc_Absyn_ArrayType_struct*)
_tmp34F)->f3;_tmp353=_tmp302.f2;if(_tmp353 <= (void*)3?1:*((int*)_tmp353)!= 7)
goto _LL29C;_tmp354=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp353)->f1;
_tmp355=((struct Cyc_Absyn_ArrayType_struct*)_tmp353)->f2;_tmp356=((struct Cyc_Absyn_ArrayType_struct*)
_tmp353)->f3;_LL29B: {int _tmp39D=Cyc_Tcutil_tqual_cmp(_tmp355,_tmp351);if(
_tmp39D != 0)return _tmp39D;{int _tmp39E=Cyc_Tcutil_typecmp(_tmp354,_tmp350);if(
_tmp39E != 0)return _tmp39E;if(_tmp352 == _tmp356)return 0;if(_tmp352 == 0?1: _tmp356
== 0)({void*_tmp39F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("missing expression in array index",sizeof(char),34),_tag_arr(_tmp39F,
sizeof(void*),0));});return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp352,_tmp356);}}_LL29C: _tmp357=_tmp302.f1;if(_tmp357 <= (void*)3?1:*((int*)
_tmp357)!= 8)goto _LL29E;_tmp358=((struct Cyc_Absyn_FnType_struct*)_tmp357)->f1;
_tmp359=_tmp358.tvars;_tmp35A=_tmp358.effect;_tmp35B=(void*)_tmp358.ret_typ;
_tmp35C=_tmp358.args;_tmp35D=_tmp358.c_varargs;_tmp35E=_tmp358.cyc_varargs;
_tmp35F=_tmp358.rgn_po;_tmp360=_tmp358.attributes;_tmp361=_tmp302.f2;if(_tmp361
<= (void*)3?1:*((int*)_tmp361)!= 8)goto _LL29E;_tmp362=((struct Cyc_Absyn_FnType_struct*)
_tmp361)->f1;_tmp363=_tmp362.tvars;_tmp364=_tmp362.effect;_tmp365=(void*)_tmp362.ret_typ;
_tmp366=_tmp362.args;_tmp367=_tmp362.c_varargs;_tmp368=_tmp362.cyc_varargs;
_tmp369=_tmp362.rgn_po;_tmp36A=_tmp362.attributes;_LL29D:({void*_tmp3A0[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: function types not handled",
sizeof(char),36),_tag_arr(_tmp3A0,sizeof(void*),0));});_LL29E: _tmp36B=_tmp302.f1;
if(_tmp36B <= (void*)3?1:*((int*)_tmp36B)!= 9)goto _LL2A0;_tmp36C=((struct Cyc_Absyn_TupleType_struct*)
_tmp36B)->f1;_tmp36D=_tmp302.f2;if(_tmp36D <= (void*)3?1:*((int*)_tmp36D)!= 9)
goto _LL2A0;_tmp36E=((struct Cyc_Absyn_TupleType_struct*)_tmp36D)->f1;_LL29F:
return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp36E,
_tmp36C);_LL2A0: _tmp36F=_tmp302.f1;if(_tmp36F <= (void*)3?1:*((int*)_tmp36F)!= 11)
goto _LL2A2;_tmp370=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp36F)->f1;
_tmp371=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp36F)->f2;_tmp372=_tmp302.f2;
if(_tmp372 <= (void*)3?1:*((int*)_tmp372)!= 11)goto _LL2A2;_tmp373=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp372)->f1;_tmp374=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp372)->f2;_LL2A1: if(_tmp373 != _tmp370){if(_tmp373 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp374,_tmp371);_LL2A2: _tmp375=_tmp302.f1;if((int)_tmp375 != 2)goto _LL2A4;
_tmp376=_tmp302.f2;if((int)_tmp376 != 2)goto _LL2A4;_LL2A3: return 0;_LL2A4: _tmp377=
_tmp302.f1;if(_tmp377 <= (void*)3?1:*((int*)_tmp377)!= 15)goto _LL2A6;_tmp378=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp377)->f1;_tmp379=_tmp302.f2;
if(_tmp379 <= (void*)3?1:*((int*)_tmp379)!= 15)goto _LL2A6;_tmp37A=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp379)->f1;_LL2A5: return Cyc_Tcutil_typecmp(
_tmp378,_tmp37A);_LL2A6: _tmp37B=_tmp302.f1;if(_tmp37B <= (void*)3?1:*((int*)
_tmp37B)!= 14)goto _LL2A8;_tmp37C=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp37B)->f1;_tmp37D=_tmp302.f2;if(_tmp37D <= (void*)3?1:*((int*)_tmp37D)!= 14)
goto _LL2A8;_tmp37E=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp37D)->f1;
_LL2A7: return Cyc_Tcutil_typecmp(_tmp37C,_tmp37E);_LL2A8: _tmp37F=_tmp302.f1;if(
_tmp37F <= (void*)3?1:*((int*)_tmp37F)!= 17)goto _LL2AA;_tmp380=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp37F)->f1;_tmp381=_tmp302.f2;if(_tmp381 <= (void*)3?1:*((int*)_tmp381)!= 17)
goto _LL2AA;_tmp382=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp381)->f1;_LL2A9:
return Cyc_Tcutil_typecmp(_tmp380,_tmp382);_LL2AA: _tmp383=_tmp302.f1;if(_tmp383 <= (
void*)3?1:*((int*)_tmp383)!= 18)goto _LL2AC;_tmp384=((struct Cyc_Absyn_TypeInt_struct*)
_tmp383)->f1;_tmp385=_tmp302.f2;if(_tmp385 <= (void*)3?1:*((int*)_tmp385)!= 18)
goto _LL2AC;_tmp386=((struct Cyc_Absyn_TypeInt_struct*)_tmp385)->f1;_LL2AB: return
Cyc_Core_intcmp(_tmp384,_tmp386);_LL2AC: _tmp387=_tmp302.f1;if(_tmp387 <= (void*)3?
1:*((int*)_tmp387)!= 20)goto _LL2AE;_LL2AD: goto _LL2AF;_LL2AE: _tmp388=_tmp302.f2;
if(_tmp388 <= (void*)3?1:*((int*)_tmp388)!= 20)goto _LL2B0;_LL2AF: goto _LL2B1;
_LL2B0: _tmp389=_tmp302.f1;if(_tmp389 <= (void*)3?1:*((int*)_tmp389)!= 19)goto
_LL2B2;_LL2B1: goto _LL2B3;_LL2B2: _tmp38A=_tmp302.f1;if(_tmp38A <= (void*)3?1:*((
int*)_tmp38A)!= 21)goto _LL2B4;_LL2B3: goto _LL2B5;_LL2B4: _tmp38B=_tmp302.f2;if(
_tmp38B <= (void*)3?1:*((int*)_tmp38B)!= 21)goto _LL2B6;_LL2B5: goto _LL2B7;_LL2B6:
_tmp38C=_tmp302.f2;if(_tmp38C <= (void*)3?1:*((int*)_tmp38C)!= 19)goto _LL2B8;
_LL2B7:({void*_tmp3A1[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("typecmp: effects not handled",sizeof(char),29),
_tag_arr(_tmp3A1,sizeof(void*),0));});_LL2B8:;_LL2B9:({void*_tmp3A2[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Unmatched case in typecmp",
sizeof(char),26),_tag_arr(_tmp3A2,sizeof(void*),0));});_LL281:;}}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp3A3=Cyc_Tcutil_compress(t);_LL2C0: if(_tmp3A3 <= (void*)3?1:*((
int*)_tmp3A3)!= 5)goto _LL2C2;_LL2C1: goto _LL2C3;_LL2C2: if((int)_tmp3A3 != 1)goto
_LL2C4;_LL2C3: goto _LL2C5;_LL2C4: if(_tmp3A3 <= (void*)3?1:*((int*)_tmp3A3)!= 6)
goto _LL2C6;_LL2C5: goto _LL2C7;_LL2C6: if(_tmp3A3 <= (void*)3?1:*((int*)_tmp3A3)!= 
12)goto _LL2C8;_LL2C7: goto _LL2C9;_LL2C8: if(_tmp3A3 <= (void*)3?1:*((int*)_tmp3A3)
!= 13)goto _LL2CA;_LL2C9: return 1;_LL2CA:;_LL2CB: return 0;_LL2BF:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp3A5=({struct _tuple6 _tmp3A4;_tmp3A4.f1=Cyc_Tcutil_compress(
t1);_tmp3A4.f2=Cyc_Tcutil_compress(t2);_tmp3A4;});void*_tmp3A6;int _tmp3A7;void*
_tmp3A8;int _tmp3A9;void*_tmp3AA;void*_tmp3AB;void*_tmp3AC;void*_tmp3AD;void*
_tmp3AE;void*_tmp3AF;void*_tmp3B0;void*_tmp3B1;void*_tmp3B2;void*_tmp3B3;void*
_tmp3B4;void*_tmp3B5;void*_tmp3B6;void*_tmp3B7;void*_tmp3B8;void*_tmp3B9;void*
_tmp3BA;void*_tmp3BB;void*_tmp3BC;void*_tmp3BD;void*_tmp3BE;void*_tmp3BF;void*
_tmp3C0;void*_tmp3C1;void*_tmp3C2;void*_tmp3C3;void*_tmp3C4;void*_tmp3C5;void*
_tmp3C6;void*_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*_tmp3CA;void*_tmp3CB;void*
_tmp3CC;void*_tmp3CD;void*_tmp3CE;void*_tmp3CF;void*_tmp3D0;void*_tmp3D1;void*
_tmp3D2;void*_tmp3D3;void*_tmp3D4;void*_tmp3D5;void*_tmp3D6;void*_tmp3D7;void*
_tmp3D8;_LL2CD: _tmp3A6=_tmp3A5.f1;if(_tmp3A6 <= (void*)3?1:*((int*)_tmp3A6)!= 6)
goto _LL2CF;_tmp3A7=((struct Cyc_Absyn_DoubleType_struct*)_tmp3A6)->f1;_tmp3A8=
_tmp3A5.f2;if(_tmp3A8 <= (void*)3?1:*((int*)_tmp3A8)!= 6)goto _LL2CF;_tmp3A9=((
struct Cyc_Absyn_DoubleType_struct*)_tmp3A8)->f1;_LL2CE: return !_tmp3A9?_tmp3A7: 0;
_LL2CF: _tmp3AA=_tmp3A5.f1;if(_tmp3AA <= (void*)3?1:*((int*)_tmp3AA)!= 6)goto
_LL2D1;_tmp3AB=_tmp3A5.f2;if((int)_tmp3AB != 1)goto _LL2D1;_LL2D0: goto _LL2D2;
_LL2D1: _tmp3AC=_tmp3A5.f1;if(_tmp3AC <= (void*)3?1:*((int*)_tmp3AC)!= 6)goto
_LL2D3;_tmp3AD=_tmp3A5.f2;if(_tmp3AD <= (void*)3?1:*((int*)_tmp3AD)!= 5)goto
_LL2D3;_LL2D2: goto _LL2D4;_LL2D3: _tmp3AE=_tmp3A5.f1;if(_tmp3AE <= (void*)3?1:*((
int*)_tmp3AE)!= 6)goto _LL2D5;_tmp3AF=_tmp3A5.f2;if(_tmp3AF <= (void*)3?1:*((int*)
_tmp3AF)!= 14)goto _LL2D5;_LL2D4: goto _LL2D6;_LL2D5: _tmp3B0=_tmp3A5.f1;if((int)
_tmp3B0 != 1)goto _LL2D7;_tmp3B1=_tmp3A5.f2;if(_tmp3B1 <= (void*)3?1:*((int*)
_tmp3B1)!= 14)goto _LL2D7;_LL2D6: goto _LL2D8;_LL2D7: _tmp3B2=_tmp3A5.f1;if(_tmp3B2
<= (void*)3?1:*((int*)_tmp3B2)!= 6)goto _LL2D9;_tmp3B3=_tmp3A5.f2;if(_tmp3B3 <= (
void*)3?1:*((int*)_tmp3B3)!= 17)goto _LL2D9;_LL2D8: goto _LL2DA;_LL2D9: _tmp3B4=
_tmp3A5.f1;if((int)_tmp3B4 != 1)goto _LL2DB;_tmp3B5=_tmp3A5.f2;if(_tmp3B5 <= (void*)
3?1:*((int*)_tmp3B5)!= 17)goto _LL2DB;_LL2DA: goto _LL2DC;_LL2DB: _tmp3B6=_tmp3A5.f1;
if((int)_tmp3B6 != 1)goto _LL2DD;_tmp3B7=_tmp3A5.f2;if(_tmp3B7 <= (void*)3?1:*((int*)
_tmp3B7)!= 5)goto _LL2DD;_LL2DC: return 1;_LL2DD: _tmp3B8=_tmp3A5.f1;if(_tmp3B8 <= (
void*)3?1:*((int*)_tmp3B8)!= 5)goto _LL2DF;_tmp3B9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3B8)->f2;if((int)_tmp3B9 != 3)goto _LL2DF;_tmp3BA=_tmp3A5.f2;if(_tmp3BA <= (
void*)3?1:*((int*)_tmp3BA)!= 5)goto _LL2DF;_tmp3BB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3BA)->f2;if((int)_tmp3BB != 3)goto _LL2DF;_LL2DE: return 0;_LL2DF: _tmp3BC=
_tmp3A5.f1;if(_tmp3BC <= (void*)3?1:*((int*)_tmp3BC)!= 5)goto _LL2E1;_tmp3BD=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3BC)->f2;if((int)_tmp3BD != 3)goto _LL2E1;
_LL2E0: goto _LL2E2;_LL2E1: _tmp3BE=_tmp3A5.f1;if(_tmp3BE <= (void*)3?1:*((int*)
_tmp3BE)!= 5)goto _LL2E3;_tmp3BF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3BE)->f2;
if((int)_tmp3BF != 2)goto _LL2E3;_tmp3C0=_tmp3A5.f2;if((int)_tmp3C0 != 1)goto _LL2E3;
_LL2E2: goto _LL2E4;_LL2E3: _tmp3C1=_tmp3A5.f1;if(_tmp3C1 <= (void*)3?1:*((int*)
_tmp3C1)!= 5)goto _LL2E5;_tmp3C2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C1)->f2;
if((int)_tmp3C2 != 2)goto _LL2E5;_tmp3C3=_tmp3A5.f2;if(_tmp3C3 <= (void*)3?1:*((int*)
_tmp3C3)!= 5)goto _LL2E5;_tmp3C4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C3)->f2;
if((int)_tmp3C4 != 1)goto _LL2E5;_LL2E4: goto _LL2E6;_LL2E5: _tmp3C5=_tmp3A5.f1;if(
_tmp3C5 <= (void*)3?1:*((int*)_tmp3C5)!= 5)goto _LL2E7;_tmp3C6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C5)->f2;if((int)_tmp3C6 != 2)goto _LL2E7;_tmp3C7=_tmp3A5.f2;if(_tmp3C7 <= (
void*)3?1:*((int*)_tmp3C7)!= 5)goto _LL2E7;_tmp3C8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C7)->f2;if((int)_tmp3C8 != 0)goto _LL2E7;_LL2E6: goto _LL2E8;_LL2E7: _tmp3C9=
_tmp3A5.f1;if(_tmp3C9 <= (void*)3?1:*((int*)_tmp3C9)!= 5)goto _LL2E9;_tmp3CA=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3C9)->f2;if((int)_tmp3CA != 1)goto _LL2E9;
_tmp3CB=_tmp3A5.f2;if(_tmp3CB <= (void*)3?1:*((int*)_tmp3CB)!= 5)goto _LL2E9;
_tmp3CC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3CB)->f2;if((int)_tmp3CC != 
0)goto _LL2E9;_LL2E8: goto _LL2EA;_LL2E9: _tmp3CD=_tmp3A5.f1;if(_tmp3CD <= (void*)3?1:*((
int*)_tmp3CD)!= 17)goto _LL2EB;_tmp3CE=_tmp3A5.f2;if(_tmp3CE <= (void*)3?1:*((int*)
_tmp3CE)!= 5)goto _LL2EB;_tmp3CF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3CE)->f2;
if((int)_tmp3CF != 1)goto _LL2EB;_LL2EA: goto _LL2EC;_LL2EB: _tmp3D0=_tmp3A5.f1;if(
_tmp3D0 <= (void*)3?1:*((int*)_tmp3D0)!= 17)goto _LL2ED;_tmp3D1=_tmp3A5.f2;if(
_tmp3D1 <= (void*)3?1:*((int*)_tmp3D1)!= 5)goto _LL2ED;_tmp3D2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D1)->f2;if((int)_tmp3D2 != 0)goto _LL2ED;_LL2EC: goto _LL2EE;_LL2ED: _tmp3D3=
_tmp3A5.f1;if(_tmp3D3 <= (void*)3?1:*((int*)_tmp3D3)!= 14)goto _LL2EF;_tmp3D4=
_tmp3A5.f2;if(_tmp3D4 <= (void*)3?1:*((int*)_tmp3D4)!= 5)goto _LL2EF;_tmp3D5=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3D4)->f2;if((int)_tmp3D5 != 1)goto _LL2EF;
_LL2EE: goto _LL2F0;_LL2EF: _tmp3D6=_tmp3A5.f1;if(_tmp3D6 <= (void*)3?1:*((int*)
_tmp3D6)!= 14)goto _LL2F1;_tmp3D7=_tmp3A5.f2;if(_tmp3D7 <= (void*)3?1:*((int*)
_tmp3D7)!= 5)goto _LL2F1;_tmp3D8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3D7)->f2;
if((int)_tmp3D8 != 0)goto _LL2F1;_LL2F0: return 1;_LL2F1:;_LL2F2: return 0;_LL2CC:;}
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=
el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0?1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->v=(
void*)t1;_tmp3D9;});}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)((
struct Cyc_Core_Opt*)_check_null(max_arith_type))->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_Std_String_pa_struct _tmp3DC;_tmp3DC.tag=0;_tmp3DC.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp3DB;_tmp3DB.tag=0;_tmp3DB.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp3DA[2]={& _tmp3DB,& _tmp3DC};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,
_tag_arr("type mismatch: expecting %s but found %s",sizeof(char),41),_tag_arr(
_tmp3DA,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp3DD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL2F4: if(_tmp3DD <= (void*)3?1:*((int*)_tmp3DD)!= 4)goto _LL2F6;_LL2F5: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ);goto _LL2F3;_LL2F6:;_LL2F7: return 0;_LL2F3:;}return 1;}int
Cyc_Tcutil_is_integral_type(void*t){void*_tmp3DE=Cyc_Tcutil_compress(t);_LL2F9:
if(_tmp3DE <= (void*)3?1:*((int*)_tmp3DE)!= 5)goto _LL2FB;_LL2FA: goto _LL2FC;_LL2FB:
if(_tmp3DE <= (void*)3?1:*((int*)_tmp3DE)!= 14)goto _LL2FD;_LL2FC: goto _LL2FE;
_LL2FD: if(_tmp3DE <= (void*)3?1:*((int*)_tmp3DE)!= 17)goto _LL2FF;_LL2FE: goto
_LL300;_LL2FF: if(_tmp3DE <= (void*)3?1:*((int*)_tmp3DE)!= 12)goto _LL301;_LL300:
goto _LL302;_LL301: if(_tmp3DE <= (void*)3?1:*((int*)_tmp3DE)!= 13)goto _LL303;
_LL302: return 1;_LL303:;_LL304: return 0;_LL2F8:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp3DF[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3DF,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp3E0[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(char),44),_tag_arr(_tmp3E0,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp3E2=({struct _tuple6 _tmp3E1;
_tmp3E1.f1=t1;_tmp3E1.f2=t2;_tmp3E1;});void*_tmp3E3;struct Cyc_Absyn_PtrInfo
_tmp3E4;void*_tmp3E5;struct Cyc_Absyn_PtrInfo _tmp3E6;void*_tmp3E7;void*_tmp3E8;
struct Cyc_Absyn_Tqual _tmp3E9;struct Cyc_Absyn_Exp*_tmp3EA;void*_tmp3EB;void*
_tmp3EC;struct Cyc_Absyn_Tqual _tmp3ED;struct Cyc_Absyn_Exp*_tmp3EE;void*_tmp3EF;
struct Cyc_Absyn_TunionFieldInfo _tmp3F0;void*_tmp3F1;struct Cyc_Absyn_Tuniondecl*
_tmp3F2;struct Cyc_Absyn_Tunionfield*_tmp3F3;struct Cyc_List_List*_tmp3F4;void*
_tmp3F5;struct Cyc_Absyn_TunionInfo _tmp3F6;void*_tmp3F7;struct Cyc_Absyn_Tuniondecl**
_tmp3F8;struct Cyc_Absyn_Tuniondecl*_tmp3F9;struct Cyc_List_List*_tmp3FA;void*
_tmp3FB;struct Cyc_Absyn_PtrInfo _tmp3FC;void*_tmp3FD;void*_tmp3FE;struct Cyc_Absyn_Conref*
_tmp3FF;struct Cyc_Absyn_Tqual _tmp400;struct Cyc_Absyn_Conref*_tmp401;void*_tmp402;
struct Cyc_Absyn_TunionInfo _tmp403;void*_tmp404;struct Cyc_Absyn_Tuniondecl**
_tmp405;struct Cyc_Absyn_Tuniondecl*_tmp406;struct Cyc_List_List*_tmp407;void*
_tmp408;void*_tmp409;void*_tmp40A;void*_tmp40B;void*_tmp40C;void*_tmp40D;void*
_tmp40E;_LL306: _tmp3E3=_tmp3E2.f1;if(_tmp3E3 <= (void*)3?1:*((int*)_tmp3E3)!= 4)
goto _LL308;_tmp3E4=((struct Cyc_Absyn_PointerType_struct*)_tmp3E3)->f1;_tmp3E5=
_tmp3E2.f2;if(_tmp3E5 <= (void*)3?1:*((int*)_tmp3E5)!= 4)goto _LL308;_tmp3E6=((
struct Cyc_Absyn_PointerType_struct*)_tmp3E5)->f1;_LL307: {int okay=1;if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp3E4.nullable,_tmp3E6.nullable)){void*_tmp40F=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp3E4.nullable))->v;int _tmp410;_LL315: if(_tmp40F <= (void*)1?1:*((int*)_tmp40F)
!= 0)goto _LL317;_tmp410=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp40F)->f1;
_LL316: okay=!_tmp410;goto _LL314;_LL317:;_LL318:({void*_tmp411[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("silent_castable conref not eq",
sizeof(char),30),_tag_arr(_tmp411,sizeof(void*),0));});_LL314:;}if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp3E4.bounds,_tmp3E6.bounds)){struct _tuple6 _tmp413=({
struct _tuple6 _tmp412;_tmp412.f1=(void*)(Cyc_Absyn_compress_conref(_tmp3E4.bounds))->v;
_tmp412.f2=(void*)(Cyc_Absyn_compress_conref(_tmp3E6.bounds))->v;_tmp412;});void*
_tmp414;void*_tmp415;void*_tmp416;void*_tmp417;void*_tmp418;void*_tmp419;struct
Cyc_Absyn_Exp*_tmp41A;void*_tmp41B;void*_tmp41C;struct Cyc_Absyn_Exp*_tmp41D;void*
_tmp41E;void*_tmp41F;void*_tmp420;void*_tmp421;_LL31A: _tmp414=_tmp413.f1;if(
_tmp414 <= (void*)1?1:*((int*)_tmp414)!= 0)goto _LL31C;_tmp415=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp414)->f1;if(_tmp415 <= (void*)1?1:*((int*)_tmp415)!= 0)goto _LL31C;_tmp416=
_tmp413.f2;if(_tmp416 <= (void*)1?1:*((int*)_tmp416)!= 0)goto _LL31C;_tmp417=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp416)->f1;if((int)_tmp417 != 0)goto _LL31C;
_LL31B: okay=1;goto _LL319;_LL31C: _tmp418=_tmp413.f1;if(_tmp418 <= (void*)1?1:*((
int*)_tmp418)!= 0)goto _LL31E;_tmp419=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp418)->f1;if(_tmp419 <= (void*)1?1:*((int*)_tmp419)!= 0)goto _LL31E;_tmp41A=((
struct Cyc_Absyn_Upper_b_struct*)_tmp419)->f1;_tmp41B=_tmp413.f2;if(_tmp41B <= (
void*)1?1:*((int*)_tmp41B)!= 0)goto _LL31E;_tmp41C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp41B)->f1;if(_tmp41C <= (void*)1?1:*((int*)_tmp41C)!= 0)goto _LL31E;_tmp41D=((
struct Cyc_Absyn_Upper_b_struct*)_tmp41C)->f1;_LL31D: okay=okay?Cyc_Evexp_lte_const_exp(
_tmp41D,_tmp41A): 0;({void*_tmp422[0]={};Cyc_Tcutil_warn(loc,_tag_arr("implicit cast to shorter array",
sizeof(char),31),_tag_arr(_tmp422,sizeof(void*),0));});goto _LL319;_LL31E: _tmp41E=
_tmp413.f1;if(_tmp41E <= (void*)1?1:*((int*)_tmp41E)!= 0)goto _LL320;_tmp41F=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp41E)->f1;if((int)_tmp41F != 0)goto _LL320;
_tmp420=_tmp413.f2;if(_tmp420 <= (void*)1?1:*((int*)_tmp420)!= 0)goto _LL320;
_tmp421=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp420)->f1;if((int)_tmp421
!= 0)goto _LL320;_LL31F: okay=1;goto _LL319;_LL320:;_LL321: okay=0;goto _LL319;_LL319:;}
okay=okay?Cyc_Tcutil_unify((void*)_tmp3E4.elt_typ,(void*)_tmp3E6.elt_typ): 0;okay=
okay?Cyc_Tcutil_unify((void*)_tmp3E4.rgn_typ,(void*)_tmp3E6.rgn_typ)?1: Cyc_Tcenv_region_outlives(
te,(void*)_tmp3E4.rgn_typ,(void*)_tmp3E6.rgn_typ): 0;okay=okay?!(_tmp3E4.tq).q_const?
1:(_tmp3E6.tq).q_const: 0;return okay;}_LL308: _tmp3E7=_tmp3E2.f1;if(_tmp3E7 <= (
void*)3?1:*((int*)_tmp3E7)!= 7)goto _LL30A;_tmp3E8=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E7)->f1;_tmp3E9=((struct Cyc_Absyn_ArrayType_struct*)_tmp3E7)->f2;_tmp3EA=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3E7)->f3;_tmp3EB=_tmp3E2.f2;if(_tmp3EB <= (
void*)3?1:*((int*)_tmp3EB)!= 7)goto _LL30A;_tmp3EC=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp3EB)->f1;_tmp3ED=((struct Cyc_Absyn_ArrayType_struct*)_tmp3EB)->f2;_tmp3EE=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3EB)->f3;_LL309: {int okay;okay=(_tmp3EA != 
0?_tmp3EE != 0: 0)?Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp3EA),(struct Cyc_Absyn_Exp*)_check_null(_tmp3EE)): 0;return(okay?Cyc_Tcutil_unify(
_tmp3E8,_tmp3EC): 0)?!_tmp3E9.q_const?1: _tmp3ED.q_const: 0;}_LL30A: _tmp3EF=_tmp3E2.f1;
if(_tmp3EF <= (void*)3?1:*((int*)_tmp3EF)!= 3)goto _LL30C;_tmp3F0=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3EF)->f1;_tmp3F1=(void*)_tmp3F0.field_info;if(*((int*)_tmp3F1)!= 1)goto
_LL30C;_tmp3F2=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3F1)->f1;_tmp3F3=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3F1)->f2;_tmp3F4=_tmp3F0.targs;
_tmp3F5=_tmp3E2.f2;if(_tmp3F5 <= (void*)3?1:*((int*)_tmp3F5)!= 2)goto _LL30C;
_tmp3F6=((struct Cyc_Absyn_TunionType_struct*)_tmp3F5)->f1;_tmp3F7=(void*)_tmp3F6.tunion_info;
if(*((int*)_tmp3F7)!= 1)goto _LL30C;_tmp3F8=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3F7)->f1;_tmp3F9=*_tmp3F8;_tmp3FA=_tmp3F6.targs;_LL30B: if((_tmp3F2 == _tmp3F9?
1: Cyc_Absyn_qvar_cmp(_tmp3F2->name,_tmp3F9->name)== 0)?_tmp3F3->typs == 0: 0){for(
0;_tmp3F4 != 0?_tmp3FA != 0: 0;(_tmp3F4=_tmp3F4->tl,_tmp3FA=_tmp3FA->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp3F4->hd,(void*)_tmp3FA->hd))break;}if(_tmp3F4 == 0?_tmp3FA == 0: 0)return
1;}return 0;_LL30C: _tmp3FB=_tmp3E2.f1;if(_tmp3FB <= (void*)3?1:*((int*)_tmp3FB)!= 
4)goto _LL30E;_tmp3FC=((struct Cyc_Absyn_PointerType_struct*)_tmp3FB)->f1;_tmp3FD=(
void*)_tmp3FC.elt_typ;_tmp3FE=(void*)_tmp3FC.rgn_typ;_tmp3FF=_tmp3FC.nullable;
_tmp400=_tmp3FC.tq;_tmp401=_tmp3FC.bounds;_tmp402=_tmp3E2.f2;if(_tmp402 <= (void*)
3?1:*((int*)_tmp402)!= 2)goto _LL30E;_tmp403=((struct Cyc_Absyn_TunionType_struct*)
_tmp402)->f1;_tmp404=(void*)_tmp403.tunion_info;if(*((int*)_tmp404)!= 1)goto
_LL30E;_tmp405=((struct Cyc_Absyn_KnownTunion_struct*)_tmp404)->f1;_tmp406=*
_tmp405;_tmp407=_tmp403.targs;_tmp408=(void*)_tmp403.rgn;_LL30D:{void*_tmp423=
Cyc_Tcutil_compress(_tmp3FD);struct Cyc_Absyn_TunionFieldInfo _tmp424;void*_tmp425;
struct Cyc_Absyn_Tuniondecl*_tmp426;struct Cyc_Absyn_Tunionfield*_tmp427;struct Cyc_List_List*
_tmp428;_LL323: if(_tmp423 <= (void*)3?1:*((int*)_tmp423)!= 3)goto _LL325;_tmp424=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp423)->f1;_tmp425=(void*)_tmp424.field_info;
if(*((int*)_tmp425)!= 1)goto _LL325;_tmp426=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp425)->f1;_tmp427=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp425)->f2;
_tmp428=_tmp424.targs;_LL324: if(!Cyc_Tcutil_unify(_tmp3FE,_tmp408)?!Cyc_Tcenv_region_outlives(
te,_tmp3FE,_tmp408): 0)return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp3FF,((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(0)))return 0;if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp401,Cyc_Absyn_new_conref(Cyc_Absyn_bounds_one)))return 0;
if(Cyc_Absyn_qvar_cmp(_tmp406->name,_tmp426->name)== 0?_tmp427->typs != 0: 0){int
okay=1;for(0;_tmp428 != 0?_tmp407 != 0: 0;(_tmp428=_tmp428->tl,_tmp407=_tmp407->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp428->hd,(void*)_tmp407->hd)){okay=0;break;}}if((
!okay?1: _tmp428 != 0)?1: _tmp407 != 0)return 0;return 1;}goto _LL322;_LL325:;_LL326:
goto _LL322;_LL322:;}return 0;_LL30E: _tmp409=_tmp3E2.f1;if(_tmp409 <= (void*)3?1:*((
int*)_tmp409)!= 14)goto _LL310;_tmp40A=_tmp3E2.f2;if(_tmp40A <= (void*)3?1:*((int*)
_tmp40A)!= 5)goto _LL310;_tmp40B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp40A)->f2;
if((int)_tmp40B != 2)goto _LL310;_LL30F: goto _LL311;_LL310: _tmp40C=_tmp3E2.f1;if(
_tmp40C <= (void*)3?1:*((int*)_tmp40C)!= 17)goto _LL312;_tmp40D=_tmp3E2.f2;if(
_tmp40D <= (void*)3?1:*((int*)_tmp40D)!= 5)goto _LL312;_tmp40E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp40D)->f2;if((int)_tmp40E != 2)goto _LL312;_LL311: return 1;_LL312:;_LL313: return
Cyc_Tcutil_unify(t1,t2);_LL305:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*
_tmp429=Cyc_Tcutil_compress(t);_LL328: if(_tmp429 <= (void*)3?1:*((int*)_tmp429)!= 
4)goto _LL32A;_LL329: return 1;_LL32A:;_LL32B: return 0;_LL327:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp42A=(void*)e->r;void*_tmp42B;int _tmp42C;_LL32D:
if(*((int*)_tmp42A)!= 0)goto _LL32F;_tmp42B=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp42A)->f1;if(_tmp42B <= (void*)1?1:*((int*)_tmp42B)!= 2)goto _LL32F;_tmp42C=((
struct Cyc_Absyn_Int_c_struct*)_tmp42B)->f2;if(_tmp42C != 0)goto _LL32F;_LL32E:
return 1;_LL32F:;_LL330: return 0;_LL32C:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((
void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt
Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)
1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*
e1){if(Cyc_Tcutil_is_pointer_type(t2)?Cyc_Tcutil_is_zero(e1): 0){(void*)(e1->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp42D=_cycalloc(sizeof(*_tmp42D));
_tmp42D[0]=({struct Cyc_Absyn_Const_e_struct _tmp42E;_tmp42E.tag=0;_tmp42E.f1=(
void*)((void*)0);_tmp42E;});_tmp42D;})));{struct Cyc_List_List*_tmp42F=Cyc_Tcenv_lookup_type_vars(
te);struct Cyc_Absyn_PointerType_struct*_tmp430=({struct Cyc_Absyn_PointerType_struct*
_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431[0]=({struct Cyc_Absyn_PointerType_struct
_tmp432;_tmp432.tag=4;_tmp432.f1=({struct Cyc_Absyn_PtrInfo _tmp433;_tmp433.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->v=_tmp42F;_tmp435;}));_tmp433.rgn_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->v=_tmp42F;_tmp434;}));_tmp433.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);_tmp433.tq=Cyc_Absyn_empty_tqual();
_tmp433.bounds=Cyc_Absyn_empty_conref();_tmp433;});_tmp432;});_tmp431;});(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp430));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}static int Cyc_Tcutil_is_sizeof_type(void*t1){void*_tmp436=
Cyc_Tcutil_compress(t1);_LL332: if(_tmp436 <= (void*)3?1:*((int*)_tmp436)!= 14)
goto _LL334;_LL333: return 1;_LL334:;_LL335: return 0;_LL331:;}static int Cyc_Tcutil_is_tag_type(
void*t1){void*_tmp437=Cyc_Tcutil_compress(t1);_LL337: if(_tmp437 <= (void*)3?1:*((
int*)_tmp437)!= 17)goto _LL339;_LL338: return 1;_LL339:;_LL33A: return 0;_LL336:;}int
Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
if(Cyc_Tcutil_unify(t1,t2))return 1;if(((Cyc_Tcutil_is_arithmetic_type(t1)?Cyc_Tcutil_is_arithmetic_type(
t2): 0)?1:(Cyc_Tcutil_is_sizeof_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0))?1:(
Cyc_Tcutil_is_tag_type(t1)?Cyc_Tcutil_is_arithmetic_type(t2): 0)){if(Cyc_Tcutil_will_lose_precision(
t1,t2))({struct Cyc_Std_String_pa_struct _tmp43A;_tmp43A.tag=0;_tmp43A.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmp439;
_tmp439.tag=0;_tmp439.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmp438[2]={& _tmp439,& _tmp43A};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; %s -> %s conversion supplied",
sizeof(char),53),_tag_arr(_tmp438,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if(Cyc_Tcutil_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);({struct Cyc_Std_String_pa_struct
_tmp43D;_tmp43D.tag=0;_tmp43D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp43C;_tmp43C.tag=0;_tmp43C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp43B[2]={& _tmp43C,& _tmp43D};Cyc_Tcutil_warn(
e->loc,_tag_arr("implicit cast from %s to %s",sizeof(char),28),_tag_arr(_tmp43B,
sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp43E=Cyc_Tcutil_compress(t);
_LL33C: if(_tmp43E <= (void*)3?1:*((int*)_tmp43E)!= 5)goto _LL33E;_LL33D: goto _LL33F;
_LL33E: if((int)_tmp43E != 1)goto _LL340;_LL33F: goto _LL341;_LL340: if(_tmp43E <= (
void*)3?1:*((int*)_tmp43E)!= 6)goto _LL342;_LL341: return 1;_LL342:;_LL343: return 0;
_LL33B:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*inst,
struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp43F=_cycalloc(sizeof(*
_tmp43F));_tmp43F->f1=x->tq;_tmp43F->f2=Cyc_Tcutil_substitute(inst,(void*)x->type);
_tmp43F;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp440=t1;struct Cyc_List_List*
_tmp441;struct Cyc_Absyn_AggrInfo _tmp442;void*_tmp443;struct Cyc_Absyn_Aggrdecl**
_tmp444;struct Cyc_Absyn_Aggrdecl*_tmp445;struct Cyc_List_List*_tmp446;void*
_tmp447;struct Cyc_List_List*_tmp448;_LL345: if((int)_tmp440 != 0)goto _LL347;_LL346:
return 0;_LL347: if(_tmp440 <= (void*)3?1:*((int*)_tmp440)!= 9)goto _LL349;_tmp441=((
struct Cyc_Absyn_TupleType_struct*)_tmp440)->f1;_LL348: return _tmp441;_LL349: if(
_tmp440 <= (void*)3?1:*((int*)_tmp440)!= 10)goto _LL34B;_tmp442=((struct Cyc_Absyn_AggrType_struct*)
_tmp440)->f1;_tmp443=(void*)_tmp442.aggr_info;if(*((int*)_tmp443)!= 1)goto _LL34B;
_tmp444=((struct Cyc_Absyn_KnownAggr_struct*)_tmp443)->f1;_tmp445=*_tmp444;
_tmp446=_tmp442.targs;_LL34A: if((((void*)_tmp445->kind == (void*)1?1: _tmp445->impl
== 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp445->impl))->exist_vars
!= 0)?1:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp445->impl))->rgn_po != 0)
return({struct Cyc_List_List*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->hd=({
struct _tuple4*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A->f1=Cyc_Absyn_empty_tqual();
_tmp44A->f2=t1;_tmp44A;});_tmp449->tl=0;_tmp449;});{struct Cyc_List_List*_tmp44B=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp445->tvs,_tmp446);return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp44B,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp445->impl))->fields);}_LL34B: if(_tmp440 <= (void*)3?1:*((int*)
_tmp440)!= 11)goto _LL34D;_tmp447=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp440)->f1;if((int)_tmp447 != 0)goto _LL34D;_tmp448=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp440)->f2;_LL34C: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp448);_LL34D:;_LL34E: return({struct Cyc_List_List*
_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->hd=({struct _tuple4*_tmp44D=
_cycalloc(sizeof(*_tmp44D));_tmp44D->f1=Cyc_Absyn_empty_tqual();_tmp44D->f2=t1;
_tmp44D;});_tmp44C->tl=0;_tmp44C;});_LL344:;}}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1)?Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2):
0)return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp44F=({struct _tuple6 _tmp44E;_tmp44E.f1=t1;_tmp44E.f2=t2;_tmp44E;});void*
_tmp450;struct Cyc_Absyn_PtrInfo _tmp451;void*_tmp452;void*_tmp453;struct Cyc_Absyn_Conref*
_tmp454;struct Cyc_Absyn_Tqual _tmp455;struct Cyc_Absyn_Conref*_tmp456;void*_tmp457;
struct Cyc_Absyn_PtrInfo _tmp458;void*_tmp459;void*_tmp45A;struct Cyc_Absyn_Conref*
_tmp45B;struct Cyc_Absyn_Tqual _tmp45C;struct Cyc_Absyn_Conref*_tmp45D;_LL350:
_tmp450=_tmp44F.f1;if(_tmp450 <= (void*)3?1:*((int*)_tmp450)!= 4)goto _LL352;
_tmp451=((struct Cyc_Absyn_PointerType_struct*)_tmp450)->f1;_tmp452=(void*)
_tmp451.elt_typ;_tmp453=(void*)_tmp451.rgn_typ;_tmp454=_tmp451.nullable;_tmp455=
_tmp451.tq;_tmp456=_tmp451.bounds;_tmp457=_tmp44F.f2;if(_tmp457 <= (void*)3?1:*((
int*)_tmp457)!= 4)goto _LL352;_tmp458=((struct Cyc_Absyn_PointerType_struct*)
_tmp457)->f1;_tmp459=(void*)_tmp458.elt_typ;_tmp45A=(void*)_tmp458.rgn_typ;
_tmp45B=_tmp458.nullable;_tmp45C=_tmp458.tq;_tmp45D=_tmp458.bounds;_LL351: if(
_tmp455.q_const?!_tmp45C.q_const: 0)return 0;if((!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp454,_tmp45B)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp454): 0)?!((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp45B): 0)
return 0;if(!Cyc_Tcutil_unify(_tmp453,_tmp45A)?!Cyc_Tcenv_region_outlives(te,
_tmp453,_tmp45A): 0)return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp456,_tmp45D)){struct _tuple6 _tmp45F=({struct _tuple6 _tmp45E;_tmp45E.f1=Cyc_Absyn_conref_val(
_tmp456);_tmp45E.f2=Cyc_Absyn_conref_val(_tmp45D);_tmp45E;});void*_tmp460;void*
_tmp461;void*_tmp462;struct Cyc_Absyn_Exp*_tmp463;void*_tmp464;struct Cyc_Absyn_Exp*
_tmp465;_LL355: _tmp460=_tmp45F.f1;if(_tmp460 <= (void*)1?1:*((int*)_tmp460)!= 0)
goto _LL357;_tmp461=_tmp45F.f2;if((int)_tmp461 != 0)goto _LL357;_LL356: goto _LL354;
_LL357: _tmp462=_tmp45F.f1;if(_tmp462 <= (void*)1?1:*((int*)_tmp462)!= 0)goto
_LL359;_tmp463=((struct Cyc_Absyn_Upper_b_struct*)_tmp462)->f1;_tmp464=_tmp45F.f2;
if(_tmp464 <= (void*)1?1:*((int*)_tmp464)!= 0)goto _LL359;_tmp465=((struct Cyc_Absyn_Upper_b_struct*)
_tmp464)->f1;_LL358: if(!Cyc_Evexp_lte_const_exp(_tmp465,_tmp463))return 0;goto
_LL354;_LL359:;_LL35A: return 0;_LL354:;}return Cyc_Tcutil_ptrsubtype(te,({struct
Cyc_List_List*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->hd=({struct _tuple6*
_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->f1=t1;_tmp467->f2=t2;_tmp467;});
_tmp466->tl=assume;_tmp466;}),_tmp452,_tmp459);_LL352:;_LL353: return 0;_LL34F:;}}
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp469=({struct
_tuple6 _tmp468;_tmp468.f1=Cyc_Tcutil_compress(t1);_tmp468.f2=Cyc_Tcutil_compress(
t2);_tmp468;});void*_tmp46A;void*_tmp46B;void*_tmp46C;void*_tmp46D;_LL35C:
_tmp46A=_tmp469.f1;if(_tmp46A <= (void*)3?1:*((int*)_tmp46A)!= 5)goto _LL35E;
_tmp46B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46A)->f2;_tmp46C=_tmp469.f2;
if(_tmp46C <= (void*)3?1:*((int*)_tmp46C)!= 5)goto _LL35E;_tmp46D=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp46C)->f2;_LL35D: return _tmp46B == _tmp46D;_LL35E:;
_LL35F: return 0;_LL35B:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp46F;struct
Cyc_Absyn_Tqual _tmp470;void*_tmp471;struct _tuple4*_tmp46E=(struct _tuple4*)tqs1->hd;
_tmp46F=*_tmp46E;_tmp470=_tmp46F.f1;_tmp471=_tmp46F.f2;{struct _tuple4 _tmp473;
struct Cyc_Absyn_Tqual _tmp474;void*_tmp475;struct _tuple4*_tmp472=(struct _tuple4*)
tqs2->hd;_tmp473=*_tmp472;_tmp474=_tmp473.f1;_tmp475=_tmp473.f2;if(_tmp474.q_const?
Cyc_Tcutil_subtype(te,assume,_tmp471,_tmp475): 0)continue;else{if(Cyc_Tcutil_unify(
_tmp471,_tmp475))continue;else{if(Cyc_Tcutil_isomorphic(_tmp471,_tmp475))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp476=Cyc_Tcutil_compress(t);void*_tmp477;_LL361: if(_tmp476 <= (void*)3?1:*((
int*)_tmp476)!= 5)goto _LL363;_tmp477=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp476)->f2;if((int)_tmp477 != 0)goto _LL363;_LL362: return 1;_LL363:;_LL364: return
0;_LL360:;}int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{void*_tmp478=t2;void*_tmp479;_LL366: if(_tmp478 <= (
void*)3?1:*((int*)_tmp478)!= 5)goto _LL368;_tmp479=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp478)->f2;if((int)_tmp479 != 2)goto _LL368;_LL367: if(Cyc_Tcutil_typ_kind(t2)== (
void*)2)return 1;goto _LL365;_LL368:;_LL369: goto _LL365;_LL365:;}{void*_tmp47A=t1;
struct Cyc_Absyn_PtrInfo _tmp47B;void*_tmp47C;void*_tmp47D;struct Cyc_Absyn_Conref*
_tmp47E;struct Cyc_Absyn_Tqual _tmp47F;struct Cyc_Absyn_Conref*_tmp480;void*_tmp481;
struct Cyc_Absyn_Tqual _tmp482;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Enumdecl*
_tmp484;_LL36B: if(_tmp47A <= (void*)3?1:*((int*)_tmp47A)!= 4)goto _LL36D;_tmp47B=((
struct Cyc_Absyn_PointerType_struct*)_tmp47A)->f1;_tmp47C=(void*)_tmp47B.elt_typ;
_tmp47D=(void*)_tmp47B.rgn_typ;_tmp47E=_tmp47B.nullable;_tmp47F=_tmp47B.tq;
_tmp480=_tmp47B.bounds;_LL36C:{void*_tmp485=t2;struct Cyc_Absyn_PtrInfo _tmp486;
void*_tmp487;void*_tmp488;struct Cyc_Absyn_Conref*_tmp489;struct Cyc_Absyn_Tqual
_tmp48A;struct Cyc_Absyn_Conref*_tmp48B;_LL37C: if(_tmp485 <= (void*)3?1:*((int*)
_tmp485)!= 4)goto _LL37E;_tmp486=((struct Cyc_Absyn_PointerType_struct*)_tmp485)->f1;
_tmp487=(void*)_tmp486.elt_typ;_tmp488=(void*)_tmp486.rgn_typ;_tmp489=_tmp486.nullable;
_tmp48A=_tmp486.tq;_tmp48B=_tmp486.bounds;_LL37D: {struct Cyc_List_List*_tmp48C=({
struct Cyc_List_List*_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495->hd=({struct
_tuple6*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->f1=t1;_tmp496->f2=t2;
_tmp496;});_tmp495->tl=0;_tmp495;});int _tmp48D=Cyc_Tcutil_ptrsubtype(te,_tmp48C,
_tmp47C,_tmp487)?!_tmp47F.q_const?1: _tmp48A.q_const: 0;int _tmp48E=_tmp48D?0:((Cyc_Tcutil_bits_only(
_tmp47C)?Cyc_Tcutil_is_char_type(_tmp487): 0)?_tmp48A.q_const?1: !_tmp47F.q_const:
0);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp480,_tmp48B);
if(!bounds_ok?!_tmp48E: 0){struct _tuple6 _tmp490=({struct _tuple6 _tmp48F;_tmp48F.f1=
Cyc_Absyn_conref_val(_tmp480);_tmp48F.f2=Cyc_Absyn_conref_val(_tmp48B);_tmp48F;});
void*_tmp491;struct Cyc_Absyn_Exp*_tmp492;void*_tmp493;struct Cyc_Absyn_Exp*
_tmp494;_LL383: _tmp491=_tmp490.f1;if(_tmp491 <= (void*)1?1:*((int*)_tmp491)!= 0)
goto _LL385;_tmp492=((struct Cyc_Absyn_Upper_b_struct*)_tmp491)->f1;_tmp493=
_tmp490.f2;if(_tmp493 <= (void*)1?1:*((int*)_tmp493)!= 0)goto _LL385;_tmp494=((
struct Cyc_Absyn_Upper_b_struct*)_tmp493)->f1;_LL384: if(Cyc_Evexp_lte_const_exp(
_tmp494,_tmp492))bounds_ok=1;goto _LL382;_LL385:;_LL386: bounds_ok=1;goto _LL382;
_LL382:;}return(bounds_ok?_tmp48D?1: _tmp48E: 0)?Cyc_Tcutil_unify(_tmp47D,_tmp488)?
1: Cyc_Tcenv_region_outlives(te,_tmp47D,_tmp488): 0;}_LL37E: if((int)_tmp485 != 0)
goto _LL380;_LL37F: return 1;_LL380:;_LL381: goto _LL37B;_LL37B:;}return 0;_LL36D: if(
_tmp47A <= (void*)3?1:*((int*)_tmp47A)!= 7)goto _LL36F;_tmp481=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp47A)->f1;_tmp482=((struct Cyc_Absyn_ArrayType_struct*)_tmp47A)->f2;_tmp483=((
struct Cyc_Absyn_ArrayType_struct*)_tmp47A)->f3;_LL36E:{void*_tmp497=t2;void*
_tmp498;struct Cyc_Absyn_Tqual _tmp499;struct Cyc_Absyn_Exp*_tmp49A;_LL388: if(
_tmp497 <= (void*)3?1:*((int*)_tmp497)!= 7)goto _LL38A;_tmp498=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp497)->f1;_tmp499=((struct Cyc_Absyn_ArrayType_struct*)_tmp497)->f2;_tmp49A=((
struct Cyc_Absyn_ArrayType_struct*)_tmp497)->f3;_LL389: {int okay;okay=(_tmp483 != 
0?_tmp49A != 0: 0)?Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp49A),(struct Cyc_Absyn_Exp*)_check_null(_tmp483)): 0;return(okay?Cyc_Tcutil_unify(
_tmp481,_tmp498): 0)?!_tmp482.q_const?1: _tmp499.q_const: 0;}_LL38A:;_LL38B: return 0;
_LL387:;}return 0;_LL36F: if(_tmp47A <= (void*)3?1:*((int*)_tmp47A)!= 12)goto _LL371;
_tmp484=((struct Cyc_Absyn_EnumType_struct*)_tmp47A)->f2;_LL370:{void*_tmp49B=t2;
struct Cyc_Absyn_Enumdecl*_tmp49C;_LL38D: if(_tmp49B <= (void*)3?1:*((int*)_tmp49B)
!= 12)goto _LL38F;_tmp49C=((struct Cyc_Absyn_EnumType_struct*)_tmp49B)->f2;_LL38E:
if((_tmp484->fields != 0?_tmp49C->fields != 0: 0)?((int(*)(struct Cyc_List_List*x))
Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp484->fields))->v)
>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp49C->fields))->v): 0)return 1;goto _LL38C;
_LL38F:;_LL390: goto _LL38C;_LL38C:;}goto _LL372;_LL371: if(_tmp47A <= (void*)3?1:*((
int*)_tmp47A)!= 5)goto _LL373;_LL372: goto _LL374;_LL373: if((int)_tmp47A != 1)goto
_LL375;_LL374: goto _LL376;_LL375: if(_tmp47A <= (void*)3?1:*((int*)_tmp47A)!= 6)
goto _LL377;_LL376: return Cyc_Tcutil_coerceable(t2)?1: t2 == (void*)0;_LL377: if((int)
_tmp47A != 0)goto _LL379;_LL378: return t2 == (void*)0;_LL379:;_LL37A: return 0;_LL36A:;}}
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*
t){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){
struct Cyc_Absyn_Exp*_tmp49D=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Cast_e_struct*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E[0]=({
struct Cyc_Absyn_Cast_e_struct _tmp49F;_tmp49F.tag=13;_tmp49F.f1=(void*)t;_tmp49F.f2=
_tmp49D;_tmp49F;});_tmp49E;})));e->topt=({struct Cyc_Core_Opt*_tmp4A0=_cycalloc(
sizeof(*_tmp4A0));_tmp4A0->v=(void*)t;_tmp4A0;});}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp4A1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL392: if(_tmp4A1 <= (void*)3?1:*((int*)_tmp4A1)!= 5)
goto _LL394;_LL393: goto _LL395;_LL394: if(_tmp4A1 <= (void*)3?1:*((int*)_tmp4A1)!= 
12)goto _LL396;_LL395: goto _LL397;_LL396: if(_tmp4A1 <= (void*)3?1:*((int*)_tmp4A1)
!= 13)goto _LL398;_LL397: goto _LL399;_LL398: if(_tmp4A1 <= (void*)3?1:*((int*)
_tmp4A1)!= 17)goto _LL39A;_LL399: goto _LL39B;_LL39A: if(_tmp4A1 <= (void*)3?1:*((int*)
_tmp4A1)!= 14)goto _LL39C;_LL39B: return 1;_LL39C: if(_tmp4A1 <= (void*)3?1:*((int*)
_tmp4A1)!= 0)goto _LL39E;_LL39D: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL39E:;_LL39F: return 0;_LL391:;}int
Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return
1;{void*_tmp4A2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3A1: if((int)_tmp4A2 != 1)goto _LL3A3;_LL3A2: goto _LL3A4;_LL3A3: if(_tmp4A2 <= (
void*)3?1:*((int*)_tmp4A2)!= 6)goto _LL3A5;_LL3A4: return 1;_LL3A5:;_LL3A6: return 0;
_LL3A0:;}}int Cyc_Tcutil_is_function_type(void*t){void*_tmp4A3=Cyc_Tcutil_compress(
t);_LL3A8: if(_tmp4A3 <= (void*)3?1:*((int*)_tmp4A3)!= 8)goto _LL3AA;_LL3A9: return 1;
_LL3AA:;_LL3AB: return 0;_LL3A7:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*
t2){struct _tuple6 _tmp4A5=({struct _tuple6 _tmp4A4;_tmp4A4.f1=t1;_tmp4A4.f2=t2;
_tmp4A4;});void*_tmp4A6;int _tmp4A7;void*_tmp4A8;int _tmp4A9;void*_tmp4AA;void*
_tmp4AB;void*_tmp4AC;void*_tmp4AD;void*_tmp4AE;void*_tmp4AF;void*_tmp4B0;void*
_tmp4B1;void*_tmp4B2;void*_tmp4B3;void*_tmp4B4;void*_tmp4B5;void*_tmp4B6;void*
_tmp4B7;void*_tmp4B8;void*_tmp4B9;void*_tmp4BA;void*_tmp4BB;void*_tmp4BC;void*
_tmp4BD;void*_tmp4BE;void*_tmp4BF;void*_tmp4C0;void*_tmp4C1;void*_tmp4C2;void*
_tmp4C3;void*_tmp4C4;void*_tmp4C5;_LL3AD: _tmp4A6=_tmp4A5.f1;if(_tmp4A6 <= (void*)
3?1:*((int*)_tmp4A6)!= 6)goto _LL3AF;_tmp4A7=((struct Cyc_Absyn_DoubleType_struct*)
_tmp4A6)->f1;_tmp4A8=_tmp4A5.f2;if(_tmp4A8 <= (void*)3?1:*((int*)_tmp4A8)!= 6)
goto _LL3AF;_tmp4A9=((struct Cyc_Absyn_DoubleType_struct*)_tmp4A8)->f1;_LL3AE: if(
_tmp4A7)return t1;else{return t2;}_LL3AF: _tmp4AA=_tmp4A5.f1;if(_tmp4AA <= (void*)3?
1:*((int*)_tmp4AA)!= 6)goto _LL3B1;_LL3B0: return t1;_LL3B1: _tmp4AB=_tmp4A5.f2;if(
_tmp4AB <= (void*)3?1:*((int*)_tmp4AB)!= 6)goto _LL3B3;_LL3B2: return t2;_LL3B3:
_tmp4AC=_tmp4A5.f1;if((int)_tmp4AC != 1)goto _LL3B5;_LL3B4: goto _LL3B6;_LL3B5:
_tmp4AD=_tmp4A5.f2;if((int)_tmp4AD != 1)goto _LL3B7;_LL3B6: return(void*)1;_LL3B7:
_tmp4AE=_tmp4A5.f1;if(_tmp4AE <= (void*)3?1:*((int*)_tmp4AE)!= 5)goto _LL3B9;
_tmp4AF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AE)->f1;if((int)_tmp4AF != 
1)goto _LL3B9;_tmp4B0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AE)->f2;if((
int)_tmp4B0 != 3)goto _LL3B9;_LL3B8: goto _LL3BA;_LL3B9: _tmp4B1=_tmp4A5.f2;if(
_tmp4B1 <= (void*)3?1:*((int*)_tmp4B1)!= 5)goto _LL3BB;_tmp4B2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B1)->f1;if((int)_tmp4B2 != 1)goto _LL3BB;_tmp4B3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B1)->f2;if((int)_tmp4B3 != 3)goto _LL3BB;_LL3BA: return Cyc_Absyn_ulonglong_typ;
_LL3BB: _tmp4B4=_tmp4A5.f1;if(_tmp4B4 <= (void*)3?1:*((int*)_tmp4B4)!= 5)goto
_LL3BD;_tmp4B5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B4)->f1;if((int)
_tmp4B5 != 2)goto _LL3BD;_tmp4B6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B4)->f2;
if((int)_tmp4B6 != 3)goto _LL3BD;_LL3BC: goto _LL3BE;_LL3BD: _tmp4B7=_tmp4A5.f2;if(
_tmp4B7 <= (void*)3?1:*((int*)_tmp4B7)!= 5)goto _LL3BF;_tmp4B8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B7)->f1;if((int)_tmp4B8 != 2)goto _LL3BF;_tmp4B9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B7)->f2;if((int)_tmp4B9 != 3)goto _LL3BF;_LL3BE: goto _LL3C0;_LL3BF: _tmp4BA=
_tmp4A5.f1;if(_tmp4BA <= (void*)3?1:*((int*)_tmp4BA)!= 5)goto _LL3C1;_tmp4BB=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4BA)->f1;if((int)_tmp4BB != 0)goto _LL3C1;
_tmp4BC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4BA)->f2;if((int)_tmp4BC != 
3)goto _LL3C1;_LL3C0: goto _LL3C2;_LL3C1: _tmp4BD=_tmp4A5.f2;if(_tmp4BD <= (void*)3?1:*((
int*)_tmp4BD)!= 5)goto _LL3C3;_tmp4BE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4BD)->f1;if((int)_tmp4BE != 0)goto _LL3C3;_tmp4BF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4BD)->f2;if((int)_tmp4BF != 3)goto _LL3C3;_LL3C2: return Cyc_Absyn_slonglong_typ;
_LL3C3: _tmp4C0=_tmp4A5.f1;if(_tmp4C0 <= (void*)3?1:*((int*)_tmp4C0)!= 5)goto
_LL3C5;_tmp4C1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C0)->f1;if((int)
_tmp4C1 != 1)goto _LL3C5;_tmp4C2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C0)->f2;
if((int)_tmp4C2 != 2)goto _LL3C5;_LL3C4: goto _LL3C6;_LL3C5: _tmp4C3=_tmp4A5.f2;if(
_tmp4C3 <= (void*)3?1:*((int*)_tmp4C3)!= 5)goto _LL3C7;_tmp4C4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C3)->f1;if((int)_tmp4C4 != 1)goto _LL3C7;_tmp4C5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C3)->f2;if((int)_tmp4C5 != 2)goto _LL3C7;_LL3C6: return Cyc_Absyn_uint_typ;
_LL3C7:;_LL3C8: return Cyc_Absyn_sint_typ;_LL3AC:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp4C6=(void*)e->r;struct Cyc_Core_Opt*_tmp4C7;
_LL3CA: if(*((int*)_tmp4C6)!= 4)goto _LL3CC;_tmp4C7=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4C6)->f2;if(_tmp4C7 != 0)goto _LL3CC;_LL3CB:({void*_tmp4C8[0]={};Cyc_Tcutil_warn(
e->loc,_tag_arr("assignment in test",sizeof(char),19),_tag_arr(_tmp4C8,sizeof(
void*),0));});goto _LL3C9;_LL3CC:;_LL3CD: goto _LL3C9;_LL3C9:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp4CA=({struct _tuple6 _tmp4C9;_tmp4C9.f1=c1;_tmp4C9.f2=c2;_tmp4C9;});
void*_tmp4CB;void*_tmp4CC;void*_tmp4CD;void*_tmp4CE;void*_tmp4CF;struct Cyc_Core_Opt*
_tmp4D0;struct Cyc_Core_Opt**_tmp4D1;void*_tmp4D2;struct Cyc_Core_Opt*_tmp4D3;
struct Cyc_Core_Opt**_tmp4D4;void*_tmp4D5;struct Cyc_Core_Opt*_tmp4D6;struct Cyc_Core_Opt**
_tmp4D7;void*_tmp4D8;void*_tmp4D9;void*_tmp4DA;void*_tmp4DB;void*_tmp4DC;void*
_tmp4DD;struct Cyc_Core_Opt*_tmp4DE;struct Cyc_Core_Opt**_tmp4DF;void*_tmp4E0;void*
_tmp4E1;struct Cyc_Core_Opt*_tmp4E2;struct Cyc_Core_Opt**_tmp4E3;void*_tmp4E4;void*
_tmp4E5;struct Cyc_Core_Opt*_tmp4E6;struct Cyc_Core_Opt**_tmp4E7;void*_tmp4E8;
_LL3CF: _tmp4CB=_tmp4CA.f1;if(*((int*)_tmp4CB)!= 0)goto _LL3D1;_tmp4CC=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp4CB)->f1;_tmp4CD=_tmp4CA.f2;if(*((int*)_tmp4CD)
!= 0)goto _LL3D1;_tmp4CE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4CD)->f1;
_LL3D0: return _tmp4CC == _tmp4CE;_LL3D1: _tmp4CF=_tmp4CA.f2;if(*((int*)_tmp4CF)!= 1)
goto _LL3D3;_tmp4D0=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4CF)->f1;_tmp4D1=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4CF)->f1;_LL3D2:*
_tmp4D1=({struct Cyc_Core_Opt*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->v=(
void*)c1;_tmp4E9;});return 1;_LL3D3: _tmp4D2=_tmp4CA.f1;if(*((int*)_tmp4D2)!= 1)
goto _LL3D5;_tmp4D3=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4D2)->f1;_tmp4D4=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4D2)->f1;_LL3D4:*
_tmp4D4=({struct Cyc_Core_Opt*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->v=(
void*)c2;_tmp4EA;});return 1;_LL3D5: _tmp4D5=_tmp4CA.f1;if(*((int*)_tmp4D5)!= 2)
goto _LL3D7;_tmp4D6=((struct Cyc_Absyn_Less_kb_struct*)_tmp4D5)->f1;_tmp4D7=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp4D5)->f1;_tmp4D8=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4D5)->f2;_tmp4D9=_tmp4CA.f2;if(*((
int*)_tmp4D9)!= 0)goto _LL3D7;_tmp4DA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp4D9)->f1;_LL3D6: if(Cyc_Tcutil_kind_leq(_tmp4DA,_tmp4D8)){*_tmp4D7=({struct
Cyc_Core_Opt*_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB->v=(void*)c2;_tmp4EB;});
return 1;}else{return 0;}_LL3D7: _tmp4DB=_tmp4CA.f1;if(*((int*)_tmp4DB)!= 0)goto
_LL3D9;_tmp4DC=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4DB)->f1;_tmp4DD=
_tmp4CA.f2;if(*((int*)_tmp4DD)!= 2)goto _LL3D9;_tmp4DE=((struct Cyc_Absyn_Less_kb_struct*)
_tmp4DD)->f1;_tmp4DF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp4DD)->f1;_tmp4E0=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4DD)->f2;
_LL3D8: if(Cyc_Tcutil_kind_leq(_tmp4DC,_tmp4E0)){*_tmp4DF=({struct Cyc_Core_Opt*
_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC->v=(void*)c1;_tmp4EC;});return 1;}
else{return 0;}_LL3D9: _tmp4E1=_tmp4CA.f1;if(*((int*)_tmp4E1)!= 2)goto _LL3CE;
_tmp4E2=((struct Cyc_Absyn_Less_kb_struct*)_tmp4E1)->f1;_tmp4E3=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4E1)->f1;_tmp4E4=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4E1)->f2;_tmp4E5=_tmp4CA.f2;if(*((int*)_tmp4E5)!= 2)goto _LL3CE;_tmp4E6=((
struct Cyc_Absyn_Less_kb_struct*)_tmp4E5)->f1;_tmp4E7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4E5)->f1;_tmp4E8=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4E5)->f2;_LL3DA: if(Cyc_Tcutil_kind_leq(_tmp4E4,_tmp4E8)){*_tmp4E7=({struct
Cyc_Core_Opt*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED->v=(void*)c1;_tmp4ED;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp4E8,_tmp4E4)){*_tmp4E3=({struct Cyc_Core_Opt*
_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE->v=(void*)c2;_tmp4EE;});return 1;}
else{return 0;}}_LL3CE:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp4EF=_cycalloc_atomic(sizeof(*_tmp4EF));_tmp4EF[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp4EF;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp4F3;_tmp4F3.tag=1;_tmp4F3.f1=(int)((unsigned int)
i);{void*_tmp4F2[1]={& _tmp4F3};Cyc_Std_aprintf(_tag_arr("#%d",sizeof(char),4),
_tag_arr(_tmp4F2,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*_tmp4F0=
_cycalloc(sizeof(*_tmp4F0));_tmp4F0->name=({struct _tagged_arr*_tmp4F1=_cycalloc(
sizeof(struct _tagged_arr)* 1);_tmp4F1[0]=s;_tmp4F1;});_tmp4F0->identity=0;
_tmp4F0->kind=(void*)k;_tmp4F0;});}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _tagged_arr _tmp4F4=*t->name;return*((const char*)
_check_unknown_subscript(_tmp4F4,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_Std_String_pa_struct _tmp4F6;_tmp4F6.tag=0;
_tmp4F6.f1=(struct _tagged_arr)*t->name;{void*_tmp4F5[1]={& _tmp4F6};Cyc_Std_printf(
_tag_arr("%s",sizeof(char),3),_tag_arr(_tmp4F5,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp4F7=Cyc_Std_strconcat(_tag_arr("`",sizeof(char),
2),*t->name);*((char*)_check_unknown_subscript(_tmp4F7,sizeof(char),1))='t';t->name=({
struct _tagged_arr*_tmp4F8=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp4F8[0]=(
struct _tagged_arr)_tmp4F7;_tmp4F8;});}}struct _tuple13{struct _tagged_arr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple13*x){return({struct _tuple2*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));
_tmp4F9->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp4FA=_cycalloc(sizeof(*
_tmp4FA));_tmp4FA->v=(*x).f1;_tmp4FA;});_tmp4F9->f2=(*x).f2;_tmp4F9->f3=(*x).f3;
_tmp4F9;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp4FB=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd))_tmp4FB=({
struct Cyc_List_List*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC->hd=(void*)((
void*)atts->hd);_tmp4FC->tl=_tmp4FB;_tmp4FC;});}}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD[0]=({struct Cyc_Absyn_FnType_struct
_tmp4FE;_tmp4FE.tag=8;_tmp4FE.f1=({struct Cyc_Absyn_FnInfo _tmp4FF;_tmp4FF.tvars=
fd->tvs;_tmp4FF.effect=fd->effect;_tmp4FF.ret_typ=(void*)((void*)fd->ret_type);
_tmp4FF.args=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp4FF.c_varargs=
fd->c_varargs;_tmp4FF.cyc_varargs=fd->cyc_varargs;_tmp4FF.rgn_po=fd->rgn_po;
_tmp4FF.attributes=_tmp4FB;_tmp4FF;});_tmp4FE;});_tmp4FD;});}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple14{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple14*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*t).f2;}static
struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({struct _tuple4*
_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->f1=(*pr).f1;_tmp500->f2=t;_tmp500;});}
struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple16{
struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp501=_region_malloc(
rgn,sizeof(*_tmp501));_tmp501->f1=({struct _tuple15*_tmp502=_region_malloc(rgn,
sizeof(*_tmp502));_tmp502->f1=(*y).f1;_tmp502->f2=(*y).f2;_tmp502;});_tmp501->f2=(*
y).f3;_tmp501;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(struct _tuple16*w){
struct _tuple15*_tmp504;void*_tmp505;struct _tuple16 _tmp503=*w;_tmp504=_tmp503.f1;
_tmp505=_tmp503.f2;{struct Cyc_Core_Opt*_tmp507;struct Cyc_Absyn_Tqual _tmp508;
struct _tuple15 _tmp506=*_tmp504;_tmp507=_tmp506.f1;_tmp508=_tmp506.f2;return({
struct _tuple2*_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509->f1=_tmp507;_tmp509->f2=
_tmp508;_tmp509->f3=_tmp505;_tmp509;});}}static void*Cyc_Tcutil_field_type(struct
Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*_tmp50A=
_cycalloc(sizeof(*_tmp50A));_tmp50A->name=f->name;_tmp50A->tq=f->tq;_tmp50A->type=(
void*)t;_tmp50A->width=f->width;_tmp50A->attributes=f->attributes;_tmp50A;});}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,void*t){void*_tmp50B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp50C;struct Cyc_Absyn_AggrInfo _tmp50D;void*_tmp50E;struct Cyc_List_List*_tmp50F;
struct Cyc_Absyn_TunionInfo _tmp510;void*_tmp511;struct Cyc_List_List*_tmp512;void*
_tmp513;struct Cyc_Absyn_TunionFieldInfo _tmp514;void*_tmp515;struct Cyc_List_List*
_tmp516;struct _tuple1*_tmp517;struct Cyc_List_List*_tmp518;struct Cyc_Absyn_Typedefdecl*
_tmp519;struct Cyc_Core_Opt*_tmp51A;void*_tmp51B;struct Cyc_Absyn_Tqual _tmp51C;
struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_PtrInfo _tmp51E;void*_tmp51F;void*
_tmp520;struct Cyc_Absyn_Conref*_tmp521;struct Cyc_Absyn_Tqual _tmp522;struct Cyc_Absyn_Conref*
_tmp523;struct Cyc_Absyn_FnInfo _tmp524;struct Cyc_List_List*_tmp525;struct Cyc_Core_Opt*
_tmp526;void*_tmp527;struct Cyc_List_List*_tmp528;int _tmp529;struct Cyc_Absyn_VarargInfo*
_tmp52A;struct Cyc_List_List*_tmp52B;struct Cyc_List_List*_tmp52C;struct Cyc_List_List*
_tmp52D;void*_tmp52E;struct Cyc_List_List*_tmp52F;struct Cyc_Core_Opt*_tmp530;void*
_tmp531;void*_tmp532;void*_tmp533;void*_tmp534;void*_tmp535;struct Cyc_List_List*
_tmp536;_LL3DC: if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 1)goto _LL3DE;_tmp50C=((
struct Cyc_Absyn_VarType_struct*)_tmp50B)->f1;_LL3DD: {struct _handler_cons _tmp537;
_push_handler(& _tmp537);{int _tmp539=0;if(setjmp(_tmp537.handler))_tmp539=1;if(!
_tmp539){{void*_tmp53A=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp50C);_npop_handler(0);return _tmp53A;};_pop_handler();}else{void*_tmp538=(
void*)_exn_thrown;void*_tmp53C=_tmp538;_LL40F: if(_tmp53C != Cyc_Core_Not_found)
goto _LL411;_LL410: return t;_LL411:;_LL412:(void)_throw(_tmp53C);_LL40E:;}}}_LL3DE:
if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 10)goto _LL3E0;_tmp50D=((struct Cyc_Absyn_AggrType_struct*)
_tmp50B)->f1;_tmp50E=(void*)_tmp50D.aggr_info;_tmp50F=_tmp50D.targs;_LL3DF: {
struct Cyc_List_List*_tmp53D=Cyc_Tcutil_substs(rgn,inst,_tmp50F);return _tmp53D == 
_tmp50F?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp53E=_cycalloc(sizeof(*
_tmp53E));_tmp53E[0]=({struct Cyc_Absyn_AggrType_struct _tmp53F;_tmp53F.tag=10;
_tmp53F.f1=({struct Cyc_Absyn_AggrInfo _tmp540;_tmp540.aggr_info=(void*)_tmp50E;
_tmp540.targs=_tmp53D;_tmp540;});_tmp53F;});_tmp53E;});}_LL3E0: if(_tmp50B <= (
void*)3?1:*((int*)_tmp50B)!= 2)goto _LL3E2;_tmp510=((struct Cyc_Absyn_TunionType_struct*)
_tmp50B)->f1;_tmp511=(void*)_tmp510.tunion_info;_tmp512=_tmp510.targs;_tmp513=(
void*)_tmp510.rgn;_LL3E1: {struct Cyc_List_List*_tmp541=Cyc_Tcutil_substs(rgn,
inst,_tmp512);void*_tmp542=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp513);return(
_tmp541 == _tmp512?_tmp542 == _tmp513: 0)?t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543[0]=({struct Cyc_Absyn_TunionType_struct
_tmp544;_tmp544.tag=2;_tmp544.f1=({struct Cyc_Absyn_TunionInfo _tmp545;_tmp545.tunion_info=(
void*)_tmp511;_tmp545.targs=_tmp541;_tmp545.rgn=(void*)_tmp542;_tmp545;});
_tmp544;});_tmp543;});}_LL3E2: if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 3)goto
_LL3E4;_tmp514=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp50B)->f1;_tmp515=(
void*)_tmp514.field_info;_tmp516=_tmp514.targs;_LL3E3: {struct Cyc_List_List*
_tmp546=Cyc_Tcutil_substs(rgn,inst,_tmp516);return _tmp546 == _tmp516?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp547=_cycalloc(sizeof(*_tmp547));
_tmp547[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp548;_tmp548.tag=3;_tmp548.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp549;_tmp549.field_info=(void*)_tmp515;_tmp549.targs=
_tmp546;_tmp549;});_tmp548;});_tmp547;});}_LL3E4: if(_tmp50B <= (void*)3?1:*((int*)
_tmp50B)!= 16)goto _LL3E6;_tmp517=((struct Cyc_Absyn_TypedefType_struct*)_tmp50B)->f1;
_tmp518=((struct Cyc_Absyn_TypedefType_struct*)_tmp50B)->f2;_tmp519=((struct Cyc_Absyn_TypedefType_struct*)
_tmp50B)->f3;_tmp51A=((struct Cyc_Absyn_TypedefType_struct*)_tmp50B)->f4;_LL3E5: {
struct Cyc_List_List*_tmp54A=Cyc_Tcutil_substs(rgn,inst,_tmp518);return _tmp54A == 
_tmp518?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp54B=_cycalloc(sizeof(*
_tmp54B));_tmp54B[0]=({struct Cyc_Absyn_TypedefType_struct _tmp54C;_tmp54C.tag=16;
_tmp54C.f1=_tmp517;_tmp54C.f2=_tmp54A;_tmp54C.f3=_tmp519;_tmp54C.f4=_tmp51A;
_tmp54C;});_tmp54B;});}_LL3E6: if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 7)goto
_LL3E8;_tmp51B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp50B)->f1;_tmp51C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp50B)->f2;_tmp51D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp50B)->f3;_LL3E7: {void*_tmp54D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp51B);
return _tmp54D == _tmp51B?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp54E=
_cycalloc(sizeof(*_tmp54E));_tmp54E[0]=({struct Cyc_Absyn_ArrayType_struct _tmp54F;
_tmp54F.tag=7;_tmp54F.f1=(void*)_tmp54D;_tmp54F.f2=_tmp51C;_tmp54F.f3=_tmp51D;
_tmp54F;});_tmp54E;});}_LL3E8: if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 4)goto
_LL3EA;_tmp51E=((struct Cyc_Absyn_PointerType_struct*)_tmp50B)->f1;_tmp51F=(void*)
_tmp51E.elt_typ;_tmp520=(void*)_tmp51E.rgn_typ;_tmp521=_tmp51E.nullable;_tmp522=
_tmp51E.tq;_tmp523=_tmp51E.bounds;_LL3E9: {void*_tmp550=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp51F);void*_tmp551=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp520);if(
_tmp550 == _tmp51F?_tmp551 == _tmp520: 0)return t;return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552[0]=({struct Cyc_Absyn_PointerType_struct
_tmp553;_tmp553.tag=4;_tmp553.f1=({struct Cyc_Absyn_PtrInfo _tmp554;_tmp554.elt_typ=(
void*)_tmp550;_tmp554.rgn_typ=(void*)_tmp551;_tmp554.nullable=_tmp521;_tmp554.tq=
_tmp522;_tmp554.bounds=_tmp523;_tmp554;});_tmp553;});_tmp552;});}_LL3EA: if(
_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 8)goto _LL3EC;_tmp524=((struct Cyc_Absyn_FnType_struct*)
_tmp50B)->f1;_tmp525=_tmp524.tvars;_tmp526=_tmp524.effect;_tmp527=(void*)_tmp524.ret_typ;
_tmp528=_tmp524.args;_tmp529=_tmp524.c_varargs;_tmp52A=_tmp524.cyc_varargs;
_tmp52B=_tmp524.rgn_po;_tmp52C=_tmp524.attributes;_LL3EB:{struct Cyc_List_List*
_tmp555=_tmp525;for(0;_tmp555 != 0;_tmp555=_tmp555->tl){inst=({struct Cyc_List_List*
_tmp556=_region_malloc(rgn,sizeof(*_tmp556));_tmp556->hd=({struct _tuple8*_tmp557=
_region_malloc(rgn,sizeof(*_tmp557));_tmp557->f1=(struct Cyc_Absyn_Tvar*)_tmp555->hd;
_tmp557->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp558=_cycalloc(sizeof(*
_tmp558));_tmp558[0]=({struct Cyc_Absyn_VarType_struct _tmp559;_tmp559.tag=1;
_tmp559.f1=(struct Cyc_Absyn_Tvar*)_tmp555->hd;_tmp559;});_tmp558;});_tmp557;});
_tmp556->tl=inst;_tmp556;});}}{struct Cyc_List_List*_tmp55B;struct Cyc_List_List*
_tmp55C;struct _tuple0 _tmp55A=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple16*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp528));_tmp55B=_tmp55A.f1;_tmp55C=_tmp55A.f2;{struct Cyc_List_List*_tmp55D=
Cyc_Tcutil_substs(rgn,inst,_tmp55C);struct Cyc_List_List*_tmp55E=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp55B,_tmp55D));struct Cyc_Core_Opt*
eff2;if(_tmp526 == 0)eff2=0;else{void*_tmp55F=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp526->v);if(_tmp55F == (void*)_tmp526->v)eff2=_tmp526;else{eff2=({struct
Cyc_Core_Opt*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->v=(void*)_tmp55F;
_tmp560;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp52A == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp562;struct Cyc_Absyn_Tqual _tmp563;void*
_tmp564;int _tmp565;struct Cyc_Absyn_VarargInfo _tmp561=*_tmp52A;_tmp562=_tmp561.name;
_tmp563=_tmp561.tq;_tmp564=(void*)_tmp561.type;_tmp565=_tmp561.inject;{void*
_tmp566=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp564);if(_tmp566 == _tmp564)
cyc_varargs2=_tmp52A;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp567=
_cycalloc(sizeof(*_tmp567));_tmp567->name=_tmp562;_tmp567->tq=_tmp563;_tmp567->type=(
void*)_tmp566;_tmp567->inject=_tmp565;_tmp567;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp569;struct Cyc_List_List*_tmp56A;struct _tuple0 _tmp568=Cyc_List_rsplit(
rgn,rgn,_tmp52B);_tmp569=_tmp568.f1;_tmp56A=_tmp568.f2;{struct Cyc_List_List*
_tmp56B=Cyc_Tcutil_substs(rgn,inst,_tmp569);struct Cyc_List_List*_tmp56C=Cyc_Tcutil_substs(
rgn,inst,_tmp56A);if(_tmp56B == _tmp569?_tmp56C == _tmp56A: 0)rgn_po2=_tmp52B;else{
rgn_po2=Cyc_List_zip(_tmp56B,_tmp56C);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D[0]=({struct Cyc_Absyn_FnType_struct
_tmp56E;_tmp56E.tag=8;_tmp56E.f1=({struct Cyc_Absyn_FnInfo _tmp56F;_tmp56F.tvars=
_tmp525;_tmp56F.effect=eff2;_tmp56F.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp527);_tmp56F.args=_tmp55E;_tmp56F.c_varargs=_tmp529;_tmp56F.cyc_varargs=
cyc_varargs2;_tmp56F.rgn_po=rgn_po2;_tmp56F.attributes=_tmp52C;_tmp56F;});
_tmp56E;});_tmp56D;});}}}}}_LL3EC: if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 9)
goto _LL3EE;_tmp52D=((struct Cyc_Absyn_TupleType_struct*)_tmp50B)->f1;_LL3ED: {
struct Cyc_List_List*_tmp570=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp52D);struct Cyc_List_List*_tmp571=Cyc_Tcutil_substs(rgn,inst,_tmp570);if(
_tmp571 == _tmp570)return t;{struct Cyc_List_List*_tmp572=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp52D,_tmp571);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573[0]=({struct Cyc_Absyn_TupleType_struct
_tmp574;_tmp574.tag=9;_tmp574.f1=_tmp572;_tmp574;});_tmp573;});}}_LL3EE: if(
_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 11)goto _LL3F0;_tmp52E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp50B)->f1;_tmp52F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp50B)->f2;_LL3EF: {
struct Cyc_List_List*_tmp575=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp52F);struct Cyc_List_List*_tmp576=Cyc_Tcutil_substs(rgn,inst,_tmp575);if(
_tmp576 == _tmp575)return t;{struct Cyc_List_List*_tmp577=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp52F,
_tmp576);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp578=_cycalloc(
sizeof(*_tmp578));_tmp578[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp579;
_tmp579.tag=11;_tmp579.f1=(void*)_tmp52E;_tmp579.f2=_tmp577;_tmp579;});_tmp578;});}}
_LL3F0: if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 0)goto _LL3F2;_tmp530=((struct
Cyc_Absyn_Evar_struct*)_tmp50B)->f2;_LL3F1: if(_tmp530 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp530->v);else{return t;}_LL3F2: if(_tmp50B <= (void*)3?1:*((int*)
_tmp50B)!= 15)goto _LL3F4;_tmp531=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp50B)->f1;_LL3F3: {void*_tmp57A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp531);
return _tmp57A == _tmp531?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp57B=
_cycalloc(sizeof(*_tmp57B));_tmp57B[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp57C;_tmp57C.tag=15;_tmp57C.f1=(void*)_tmp57A;_tmp57C;});_tmp57B;});}_LL3F4:
if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 14)goto _LL3F6;_tmp532=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp50B)->f1;_LL3F5: {void*_tmp57D=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp532);return _tmp57D == _tmp532?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp57F;_tmp57F.tag=14;_tmp57F.f1=(void*)_tmp57D;_tmp57F;});_tmp57E;});}_LL3F6:
if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 17)goto _LL3F8;_tmp533=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp50B)->f1;_LL3F7: {void*_tmp580=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp533);return _tmp580 == _tmp533?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581[0]=({struct Cyc_Absyn_TagType_struct
_tmp582;_tmp582.tag=17;_tmp582.f1=(void*)_tmp580;_tmp582;});_tmp581;});}_LL3F8:
if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 18)goto _LL3FA;_LL3F9: goto _LL3FB;
_LL3FA: if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 12)goto _LL3FC;_LL3FB: goto
_LL3FD;_LL3FC: if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 13)goto _LL3FE;_LL3FD:
goto _LL3FF;_LL3FE: if((int)_tmp50B != 0)goto _LL400;_LL3FF: goto _LL401;_LL400: if(
_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 5)goto _LL402;_LL401: goto _LL403;_LL402:
if((int)_tmp50B != 1)goto _LL404;_LL403: goto _LL405;_LL404: if(_tmp50B <= (void*)3?1:*((
int*)_tmp50B)!= 6)goto _LL406;_LL405: goto _LL407;_LL406: if((int)_tmp50B != 2)goto
_LL408;_LL407: return t;_LL408: if(_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 21)goto
_LL40A;_tmp534=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp50B)->f1;_LL409: {
void*_tmp583=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp534);return _tmp583 == _tmp534?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp584=_cycalloc(sizeof(*_tmp584));
_tmp584[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp585;_tmp585.tag=21;_tmp585.f1=(
void*)_tmp583;_tmp585;});_tmp584;});}_LL40A: if(_tmp50B <= (void*)3?1:*((int*)
_tmp50B)!= 19)goto _LL40C;_tmp535=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp50B)->f1;_LL40B: {void*_tmp586=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp535);
return _tmp586 == _tmp535?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp587=
_cycalloc(sizeof(*_tmp587));_tmp587[0]=({struct Cyc_Absyn_AccessEff_struct _tmp588;
_tmp588.tag=19;_tmp588.f1=(void*)_tmp586;_tmp588;});_tmp587;});}_LL40C: if(
_tmp50B <= (void*)3?1:*((int*)_tmp50B)!= 20)goto _LL3DB;_tmp536=((struct Cyc_Absyn_JoinEff_struct*)
_tmp50B)->f1;_LL40D: {struct Cyc_List_List*_tmp589=Cyc_Tcutil_substs(rgn,inst,
_tmp536);return _tmp589 == _tmp536?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp58B;_tmp58B.tag=20;_tmp58B.f1=_tmp589;_tmp58B;});_tmp58A;});}_LL3DB:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp58C=(void*)ts->hd;
struct Cyc_List_List*_tmp58D=ts->tl;void*_tmp58E=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp58C);struct Cyc_List_List*_tmp58F=Cyc_Tcutil_substs(rgn,inst,_tmp58D);if(
_tmp58C == _tmp58E?_tmp58D == _tmp58F: 0)return ts;return(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590->hd=(void*)
_tmp58E;_tmp590->tl=_tmp58F;_tmp590;});}}extern void*Cyc_Tcutil_substitute(struct
Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp591=_cycalloc(
sizeof(*_tmp591));_tmp591->f1=tv;_tmp591->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592->v=(void*)k;_tmp592;}),({struct Cyc_Core_Opt*
_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->v=s;_tmp593;}));_tmp591;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp595;struct Cyc_List_List*_tmp596;struct _RegionHandle*_tmp597;
struct _tuple9*_tmp594=env;_tmp595=*_tmp594;_tmp596=_tmp595.f1;_tmp597=_tmp595.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp598=_region_malloc(
_tmp597,sizeof(*_tmp598));_tmp598->f1=tv;_tmp598->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->v=(void*)k;_tmp599;}),({
struct Cyc_Core_Opt*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->v=_tmp596;
_tmp59A;}));_tmp598;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_Std_zstrptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_Std_String_pa_struct _tmp59E;_tmp59E.tag=0;_tmp59E.f1=(struct
_tagged_arr)Cyc_Absynpp_kindbound2string(k2);{struct Cyc_Std_String_pa_struct
_tmp59D;_tmp59D.tag=0;_tmp59D.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k1);{struct Cyc_Std_String_pa_struct _tmp59C;_tmp59C.tag=0;_tmp59C.f1=(struct
_tagged_arr)*tv->name;{void*_tmp59B[3]={& _tmp59C,& _tmp59D,& _tmp59E};Cyc_Tcutil_terr(
loc,_tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof(char),59),_tag_arr(_tmp59B,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))
!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*
_tmp59F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("same type variable has different identity!",sizeof(char),43),_tag_arr(
_tmp59F,sizeof(void*),0));});}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0->hd=tv;
_tmp5A0->tl=tvs;_tmp5A0;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*
_tmp5A1[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv",sizeof(char),39),_tag_arr(
_tmp5A1,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp5A2[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof(char),41),_tag_arr(_tmp5A2,sizeof(void*),0));});if(*((int*)_check_null(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)_check_null(tv->identity)))
return tvs;}}return({struct Cyc_List_List*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));
_tmp5A3->hd=tv;_tmp5A3->tl=tvs;_tmp5A3;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({struct Cyc_Std_String_pa_struct
_tmp5A5;_tmp5A5.tag=0;_tmp5A5.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string(tv);{
void*_tmp5A4[1]={& _tmp5A5};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bound tvar id for %s is NULL",sizeof(char),29),
_tag_arr(_tmp5A4,sizeof(void*),1));}});return({struct Cyc_List_List*_tmp5A6=
_cycalloc(sizeof(*_tmp5A6));_tmp5A6->hd=tv;_tmp5A6->tl=tvs;_tmp5A6;});}static
struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){void*
_tmp5A7=Cyc_Tcutil_compress(e);int _tmp5A8;_LL414: if(_tmp5A7 <= (void*)3?1:*((int*)
_tmp5A7)!= 0)goto _LL416;_tmp5A8=((struct Cyc_Absyn_Evar_struct*)_tmp5A7)->f3;
_LL415:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){void*_tmp5A9=Cyc_Tcutil_compress((
void*)es2->hd);int _tmp5AA;_LL419: if(_tmp5A9 <= (void*)3?1:*((int*)_tmp5A9)!= 0)
goto _LL41B;_tmp5AA=((struct Cyc_Absyn_Evar_struct*)_tmp5A9)->f3;_LL41A: if(_tmp5A8
== _tmp5AA)return es;goto _LL418;_LL41B:;_LL41C: goto _LL418;_LL418:;}}return({
struct Cyc_List_List*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->hd=(void*)e;
_tmp5AB->tl=es;_tmp5AB;});_LL416:;_LL417: return es;_LL413:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))
== *((int*)_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;
break;}}}if(!present)r=({struct Cyc_List_List*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));
_tmp5AC->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp5AC->tl=r;_tmp5AC;});}r=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(width)))({struct Cyc_Std_String_pa_struct
_tmp5AE;_tmp5AE.tag=0;_tmp5AE.f1=(struct _tagged_arr)*fn;{void*_tmp5AD[1]={&
_tmp5AE};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s does not have constant width",
sizeof(char),41),_tag_arr(_tmp5AD,sizeof(void*),1));}});else{unsigned int _tmp5B0;
int _tmp5B1;struct _tuple7 _tmp5AF=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(width));_tmp5B0=_tmp5AF.f1;_tmp5B1=_tmp5AF.f2;if(!_tmp5B1)({void*
_tmp5B2[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield width cannot use sizeof or offsetof",
sizeof(char),45),_tag_arr(_tmp5B2,sizeof(void*),0));});w=_tmp5B0;}{void*_tmp5B3=
Cyc_Tcutil_compress(field_typ);void*_tmp5B4;_LL41E: if(_tmp5B3 <= (void*)3?1:*((
int*)_tmp5B3)!= 5)goto _LL420;_tmp5B4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5B3)->f2;_LL41F:{void*_tmp5B5=_tmp5B4;_LL423: if((int)_tmp5B5 != 0)goto _LL425;
_LL424: if(w > 8)({void*_tmp5B6[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(char),26),_tag_arr(_tmp5B6,sizeof(void*),0));});goto _LL422;_LL425: if((int)
_tmp5B5 != 1)goto _LL427;_LL426: if(w > 16)({void*_tmp5B7[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp5B7,sizeof(
void*),0));});goto _LL422;_LL427: if((int)_tmp5B5 != 2)goto _LL429;_LL428: if(w > 32)({
void*_tmp5B8[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(char),26),_tag_arr(_tmp5B8,sizeof(void*),0));});goto _LL422;_LL429: if((int)
_tmp5B5 != 3)goto _LL422;_LL42A: if(w > 64)({void*_tmp5B9[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield larger than type",sizeof(char),26),_tag_arr(_tmp5B9,sizeof(
void*),0));});goto _LL422;_LL422:;}goto _LL41D;_LL420:;_LL421:({struct Cyc_Std_String_pa_struct
_tmp5BC;_tmp5BC.tag=0;_tmp5BC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
field_typ);{struct Cyc_Std_String_pa_struct _tmp5BB;_tmp5BB.tag=0;_tmp5BB.f1=(
struct _tagged_arr)*fn;{void*_tmp5BA[2]={& _tmp5BB,& _tmp5BC};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield %s must have integral type but has type %s",sizeof(char),52),
_tag_arr(_tmp5BA,sizeof(void*),2));}}});goto _LL41D;_LL41D:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp5BD=(void*)atts->hd;_LL42C: if((int)
_tmp5BD != 5)goto _LL42E;_LL42D: continue;_LL42E: if(_tmp5BD <= (void*)16?1:*((int*)
_tmp5BD)!= 1)goto _LL430;_LL42F: continue;_LL430:;_LL431:({struct Cyc_Std_String_pa_struct
_tmp5C0;_tmp5C0.tag=0;_tmp5C0.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_String_pa_struct
_tmp5BF;_tmp5BF.tag=0;_tmp5BF.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp5BE[2]={& _tmp5BF,& _tmp5C0};Cyc_Tcutil_terr(loc,
_tag_arr("bad attribute %s on member %s",sizeof(char),30),_tag_arr(_tmp5BE,
sizeof(void*),2));}}});_LL42B:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t){{void*_tmp5C1=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp5C2;struct Cyc_Core_Opt**_tmp5C3;struct Cyc_Core_Opt*
_tmp5C4;struct Cyc_Core_Opt**_tmp5C5;struct Cyc_Absyn_Tvar*_tmp5C6;struct Cyc_List_List*
_tmp5C7;struct _tuple1*_tmp5C8;struct Cyc_Absyn_Enumdecl*_tmp5C9;struct Cyc_Absyn_Enumdecl**
_tmp5CA;struct Cyc_Absyn_TunionInfo _tmp5CB;void*_tmp5CC;void**_tmp5CD;struct Cyc_List_List*
_tmp5CE;struct Cyc_List_List**_tmp5CF;void*_tmp5D0;struct Cyc_Absyn_TunionFieldInfo
_tmp5D1;void*_tmp5D2;void**_tmp5D3;struct Cyc_List_List*_tmp5D4;struct Cyc_Absyn_PtrInfo
_tmp5D5;void*_tmp5D6;void*_tmp5D7;struct Cyc_Absyn_Conref*_tmp5D8;struct Cyc_Absyn_Tqual
_tmp5D9;struct Cyc_Absyn_Conref*_tmp5DA;void*_tmp5DB;void*_tmp5DC;void*_tmp5DD;
struct Cyc_Absyn_Tqual _tmp5DE;struct Cyc_Absyn_Exp*_tmp5DF;struct Cyc_Absyn_FnInfo
_tmp5E0;struct Cyc_List_List*_tmp5E1;struct Cyc_List_List**_tmp5E2;struct Cyc_Core_Opt*
_tmp5E3;struct Cyc_Core_Opt**_tmp5E4;void*_tmp5E5;struct Cyc_List_List*_tmp5E6;int
_tmp5E7;struct Cyc_Absyn_VarargInfo*_tmp5E8;struct Cyc_List_List*_tmp5E9;struct Cyc_List_List*
_tmp5EA;struct Cyc_List_List*_tmp5EB;void*_tmp5EC;struct Cyc_List_List*_tmp5ED;
struct Cyc_Absyn_AggrInfo _tmp5EE;void*_tmp5EF;void**_tmp5F0;struct Cyc_List_List*
_tmp5F1;struct Cyc_List_List**_tmp5F2;struct _tuple1*_tmp5F3;struct Cyc_List_List*
_tmp5F4;struct Cyc_List_List**_tmp5F5;struct Cyc_Absyn_Typedefdecl*_tmp5F6;struct
Cyc_Absyn_Typedefdecl**_tmp5F7;struct Cyc_Core_Opt*_tmp5F8;struct Cyc_Core_Opt**
_tmp5F9;void*_tmp5FA;void*_tmp5FB;void*_tmp5FC;struct Cyc_List_List*_tmp5FD;
_LL433: if((int)_tmp5C1 != 0)goto _LL435;_LL434: goto _LL432;_LL435: if(_tmp5C1 <= (
void*)3?1:*((int*)_tmp5C1)!= 0)goto _LL437;_tmp5C2=((struct Cyc_Absyn_Evar_struct*)
_tmp5C1)->f1;_tmp5C3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp5C1)->f1;_tmp5C4=((struct Cyc_Absyn_Evar_struct*)_tmp5C1)->f2;_tmp5C5=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp5C1)->f2;_LL436: if(*_tmp5C3 == 
0)*_tmp5C3=({struct Cyc_Core_Opt*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->v=(
void*)expected_kind;_tmp5FE;});if((cvtenv.fn_result?cvtenv.generalize_evars: 0)?
expected_kind == (void*)3: 0)*_tmp5C5=({struct Cyc_Core_Opt*_tmp5FF=_cycalloc(
sizeof(*_tmp5FF));_tmp5FF->v=(void*)((void*)2);_tmp5FF;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp600=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp605;_tmp605.tag=2;_tmp605.f1=0;_tmp605.f2=(void*)expected_kind;_tmp605;});
_tmp604;}));*_tmp5C5=({struct Cyc_Core_Opt*_tmp601=_cycalloc(sizeof(*_tmp601));
_tmp601->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp602=_cycalloc(
sizeof(*_tmp602));_tmp602[0]=({struct Cyc_Absyn_VarType_struct _tmp603;_tmp603.tag=
1;_tmp603.f1=_tmp600;_tmp603;});_tmp602;}));_tmp601;});_tmp5C6=_tmp600;goto
_LL438;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL432;_LL437: if(_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 1)goto _LL439;
_tmp5C6=((struct Cyc_Absyn_VarType_struct*)_tmp5C1)->f1;_LL438:{void*_tmp606=Cyc_Absyn_compress_kb((
void*)_tmp5C6->kind);struct Cyc_Core_Opt*_tmp607;struct Cyc_Core_Opt**_tmp608;
_LL466: if(*((int*)_tmp606)!= 1)goto _LL468;_tmp607=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp606)->f1;_tmp608=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp606)->f1;_LL467:*_tmp608=({struct Cyc_Core_Opt*_tmp609=_cycalloc(sizeof(*
_tmp609));_tmp609->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp60A=
_cycalloc(sizeof(*_tmp60A));_tmp60A[0]=({struct Cyc_Absyn_Less_kb_struct _tmp60B;
_tmp60B.tag=2;_tmp60B.f1=0;_tmp60B.f2=(void*)expected_kind;_tmp60B;});_tmp60A;}));
_tmp609;});goto _LL465;_LL468:;_LL469: goto _LL465;_LL465:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp5C6);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp5C6);goto _LL432;_LL439: if(_tmp5C1 <= (void*)3?1:*((int*)
_tmp5C1)!= 13)goto _LL43B;_tmp5C7=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp5C1)->f1;
_LL43A: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp60C=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp60C;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp5C7 != 0;_tmp5C7=_tmp5C7->tl){struct Cyc_Absyn_Enumfield*_tmp60D=(
struct Cyc_Absyn_Enumfield*)_tmp5C7->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp60D->name).f2))({struct Cyc_Std_String_pa_struct
_tmp60F;_tmp60F.tag=0;_tmp60F.f1=(struct _tagged_arr)*(*_tmp60D->name).f2;{void*
_tmp60E[1]={& _tmp60F};Cyc_Tcutil_terr(_tmp60D->loc,_tag_arr("duplicate enum field name %s",
sizeof(char),29),_tag_arr(_tmp60E,sizeof(void*),1));}});else{prev_fields=({
struct Cyc_List_List*_tmp610=_region_malloc(uprev_rgn,sizeof(*_tmp610));_tmp610->hd=(*
_tmp60D->name).f2;_tmp610->tl=prev_fields;_tmp610;});}if(_tmp60D->tag == 0)
_tmp60D->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp60D->loc);
else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp60D->tag)))({
struct Cyc_Std_String_pa_struct _tmp612;_tmp612.tag=0;_tmp612.f1=(struct
_tagged_arr)*(*_tmp60D->name).f2;{void*_tmp611[1]={& _tmp612};Cyc_Tcutil_terr(loc,
_tag_arr("enum field %s: expression is not constant",sizeof(char),42),_tag_arr(
_tmp611,sizeof(void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp60D->tag))).f1;tag_count=t1 + 1;(*_tmp60D->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp614;_tmp614.tag=1;_tmp614.f1=te->ns;_tmp614;});
_tmp613;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp60D->name).f2,({
struct _tuple17*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp617;_tmp617.tag=4;_tmp617.f1=(void*)t;
_tmp617.f2=_tmp60D;_tmp617;});_tmp616;});_tmp615->f2=1;_tmp615;}));}}};
_pop_region(uprev_rgn);}goto _LL432;}_LL43B: if(_tmp5C1 <= (void*)3?1:*((int*)
_tmp5C1)!= 12)goto _LL43D;_tmp5C8=((struct Cyc_Absyn_EnumType_struct*)_tmp5C1)->f1;
_tmp5C9=((struct Cyc_Absyn_EnumType_struct*)_tmp5C1)->f2;_tmp5CA=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp5C1)->f2;_LL43C: if(*_tmp5CA == 0?1:((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp5CA))->fields == 0){struct _handler_cons
_tmp618;_push_handler(& _tmp618);{int _tmp61A=0;if(setjmp(_tmp618.handler))_tmp61A=
1;if(!_tmp61A){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp5C8);*_tmp5CA=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp619=(void*)_exn_thrown;void*_tmp61C=_tmp619;_LL46B: if(_tmp61C != Cyc_Dict_Absent)
goto _LL46D;_LL46C: {struct Cyc_Tcenv_Genv*_tmp61D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp61E=({struct Cyc_Absyn_Enumdecl*_tmp61F=_cycalloc(
sizeof(*_tmp61F));_tmp61F->sc=(void*)((void*)3);_tmp61F->name=_tmp5C8;_tmp61F->fields=
0;_tmp61F;});Cyc_Tc_tcEnumdecl(te,_tmp61D,loc,_tmp61E);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp5C8);*_tmp5CA=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL46A;}}_LL46D:;_LL46E:(void)_throw(_tmp61C);_LL46A:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp5CA);*_tmp5C8=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL432;}_LL43D: if(_tmp5C1 <= (void*)3?1:*((
int*)_tmp5C1)!= 2)goto _LL43F;_tmp5CB=((struct Cyc_Absyn_TunionType_struct*)
_tmp5C1)->f1;_tmp5CC=(void*)_tmp5CB.tunion_info;_tmp5CD=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp5C1)->f1).tunion_info;_tmp5CE=_tmp5CB.targs;_tmp5CF=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp5C1)->f1).targs;_tmp5D0=(void*)_tmp5CB.rgn;
_LL43E: {struct Cyc_List_List*_tmp620=*_tmp5CF;{void*_tmp621=*_tmp5CD;struct Cyc_Absyn_UnknownTunionInfo
_tmp622;struct _tuple1*_tmp623;int _tmp624;struct Cyc_Absyn_Tuniondecl**_tmp625;
struct Cyc_Absyn_Tuniondecl*_tmp626;_LL470: if(*((int*)_tmp621)!= 0)goto _LL472;
_tmp622=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp621)->f1;_tmp623=_tmp622.name;
_tmp624=_tmp622.is_xtunion;_LL471: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp627;_push_handler(& _tmp627);{int _tmp629=0;if(setjmp(_tmp627.handler))
_tmp629=1;if(!_tmp629){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp623);;
_pop_handler();}else{void*_tmp628=(void*)_exn_thrown;void*_tmp62B=_tmp628;_LL475:
if(_tmp62B != Cyc_Dict_Absent)goto _LL477;_LL476: {struct Cyc_Tcenv_Genv*_tmp62C=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp62D=({struct Cyc_Absyn_Tuniondecl*
_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->sc=(void*)((void*)3);_tmp631->name=
_tmp623;_tmp631->tvs=0;_tmp631->fields=0;_tmp631->is_xtunion=_tmp624;_tmp631;});
Cyc_Tc_tcTuniondecl(te,_tmp62C,loc,_tmp62D);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp623);if(_tmp620 != 0){({struct Cyc_Std_String_pa_struct _tmp630;_tmp630.tag=
0;_tmp630.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp623);{struct Cyc_Std_String_pa_struct
_tmp62F;_tmp62F.tag=0;_tmp62F.f1=(struct _tagged_arr)(_tmp624?_tag_arr("xtunion",
sizeof(char),8): _tag_arr("tunion",sizeof(char),7));{void*_tmp62E[2]={& _tmp62F,&
_tmp630};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized %s %s before using",
sizeof(char),48),_tag_arr(_tmp62E,sizeof(void*),2));}}});return cvtenv;}goto
_LL474;}_LL477:;_LL478:(void)_throw(_tmp62B);_LL474:;}}}if((*tudp)->is_xtunion != 
_tmp624)({struct Cyc_Std_String_pa_struct _tmp633;_tmp633.tag=0;_tmp633.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp623);{void*_tmp632[1]={& _tmp633};Cyc_Tcutil_terr(
loc,_tag_arr("[x]tunion is different from type declaration %s",sizeof(char),48),
_tag_arr(_tmp632,sizeof(void*),1));}});*_tmp5CD=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp635;_tmp635.tag=1;_tmp635.f1=tudp;_tmp635;});_tmp634;});_tmp626=*tudp;goto
_LL473;}_LL472: if(*((int*)_tmp621)!= 1)goto _LL46F;_tmp625=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp621)->f1;_tmp626=*_tmp625;_LL473: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5D0);{struct Cyc_List_List*tvs=_tmp626->tvs;for(0;_tmp620 != 0?
tvs != 0: 0;(_tmp620=_tmp620->tl,tvs=tvs->tl)){void*t1=(void*)_tmp620->hd;void*k1=
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp620 != 0)({struct Cyc_Std_String_pa_struct _tmp637;
_tmp637.tag=0;_tmp637.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp626->name);{
void*_tmp636[1]={& _tmp637};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s",
sizeof(char),38),_tag_arr(_tmp636,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp638=
_cycalloc(sizeof(*_tmp638));_tmp638->hd=(void*)e;_tmp638->tl=hidden_ts;_tmp638;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*_tmp5CF=Cyc_List_imp_append(*
_tmp5CF,Cyc_List_imp_rev(hidden_ts));}goto _LL46F;}_LL46F:;}goto _LL432;}_LL43F:
if(_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 3)goto _LL441;_tmp5D1=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5C1)->f1;_tmp5D2=(void*)_tmp5D1.field_info;_tmp5D3=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5C1)->f1).field_info;_tmp5D4=_tmp5D1.targs;_LL440:{void*_tmp639=*_tmp5D3;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp63A;struct _tuple1*_tmp63B;struct
_tuple1*_tmp63C;int _tmp63D;struct Cyc_Absyn_Tuniondecl*_tmp63E;struct Cyc_Absyn_Tunionfield*
_tmp63F;_LL47A: if(*((int*)_tmp639)!= 0)goto _LL47C;_tmp63A=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp639)->f1;_tmp63B=_tmp63A.tunion_name;_tmp63C=_tmp63A.field_name;_tmp63D=
_tmp63A.is_xtunion;_LL47B: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp640;_push_handler(& _tmp640);{int _tmp642=0;if(setjmp(
_tmp640.handler))_tmp642=1;if(!_tmp642){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp63B);;_pop_handler();}else{void*_tmp641=(void*)_exn_thrown;void*_tmp644=
_tmp641;_LL47F: if(_tmp644 != Cyc_Dict_Absent)goto _LL481;_LL480:({struct Cyc_Std_String_pa_struct
_tmp646;_tmp646.tag=0;_tmp646.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp63B);{void*_tmp645[1]={& _tmp646};Cyc_Tcutil_terr(loc,_tag_arr("unbound type tunion %s",
sizeof(char),23),_tag_arr(_tmp645,sizeof(void*),1));}});return cvtenv;_LL481:;
_LL482:(void)_throw(_tmp644);_LL47E:;}}}{struct _handler_cons _tmp647;
_push_handler(& _tmp647);{int _tmp649=0;if(setjmp(_tmp647.handler))_tmp649=1;if(!
_tmp649){{void*_tmp64A=Cyc_Tcenv_lookup_ordinary(te,loc,_tmp63C);struct Cyc_Absyn_Tuniondecl*
_tmp64B;struct Cyc_Absyn_Tunionfield*_tmp64C;_LL484: if(*((int*)_tmp64A)!= 2)goto
_LL486;_tmp64B=((struct Cyc_Tcenv_TunionRes_struct*)_tmp64A)->f1;_tmp64C=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp64A)->f2;_LL485: tuf=_tmp64C;tud=_tmp64B;if(tud->is_xtunion
!= _tmp63D)({struct Cyc_Std_String_pa_struct _tmp64E;_tmp64E.tag=0;_tmp64E.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp63B);{void*_tmp64D[1]={& _tmp64E};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(char),48),_tag_arr(_tmp64D,sizeof(void*),1));}});goto _LL483;_LL486:;
_LL487:({struct Cyc_Std_String_pa_struct _tmp651;_tmp651.tag=0;_tmp651.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp63B);{struct Cyc_Std_String_pa_struct
_tmp650;_tmp650.tag=0;_tmp650.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp63C);{void*_tmp64F[2]={& _tmp650,& _tmp651};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp64F,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv
_tmp652=cvtenv;_npop_handler(0);return _tmp652;}_LL483:;};_pop_handler();}else{
void*_tmp648=(void*)_exn_thrown;void*_tmp654=_tmp648;_LL489: if(_tmp654 != Cyc_Dict_Absent)
goto _LL48B;_LL48A:({struct Cyc_Std_String_pa_struct _tmp657;_tmp657.tag=0;_tmp657.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp63B);{struct Cyc_Std_String_pa_struct
_tmp656;_tmp656.tag=0;_tmp656.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp63C);{void*_tmp655[2]={& _tmp656,& _tmp657};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(char),35),_tag_arr(_tmp655,sizeof(void*),2));}}});return cvtenv;_LL48B:;
_LL48C:(void)_throw(_tmp654);_LL488:;}}}*_tmp5D3=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp659;_tmp659.tag=1;_tmp659.f1=tud;_tmp659.f2=tuf;_tmp659;});_tmp658;});
_tmp63E=tud;_tmp63F=tuf;goto _LL47D;}_LL47C: if(*((int*)_tmp639)!= 1)goto _LL479;
_tmp63E=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp639)->f1;_tmp63F=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp639)->f2;_LL47D: {struct Cyc_List_List*tvs=
_tmp63E->tvs;for(0;_tmp5D4 != 0?tvs != 0: 0;(_tmp5D4=_tmp5D4->tl,tvs=tvs->tl)){void*
t1=(void*)_tmp5D4->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp5D4 != 0)({
struct Cyc_Std_String_pa_struct _tmp65C;_tmp65C.tag=0;_tmp65C.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp63F->name);{struct Cyc_Std_String_pa_struct
_tmp65B;_tmp65B.tag=0;_tmp65B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp63E->name);{void*_tmp65A[2]={& _tmp65B,& _tmp65C};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s.%s",
sizeof(char),41),_tag_arr(_tmp65A,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_Std_String_pa_struct
_tmp65F;_tmp65F.tag=0;_tmp65F.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp63F->name);{struct Cyc_Std_String_pa_struct _tmp65E;_tmp65E.tag=0;_tmp65E.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp63E->name);{void*_tmp65D[2]={&
_tmp65E,& _tmp65F};Cyc_Tcutil_terr(loc,_tag_arr("too few type arguments for tunion %s.%s",
sizeof(char),40),_tag_arr(_tmp65D,sizeof(void*),2));}}});goto _LL479;}_LL479:;}
goto _LL432;_LL441: if(_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 4)goto _LL443;
_tmp5D5=((struct Cyc_Absyn_PointerType_struct*)_tmp5C1)->f1;_tmp5D6=(void*)
_tmp5D5.elt_typ;_tmp5D7=(void*)_tmp5D5.rgn_typ;_tmp5D8=_tmp5D5.nullable;_tmp5D9=
_tmp5D5.tq;_tmp5DA=_tmp5D5.bounds;_LL442: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp5D6);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5D7);{void*_tmp660=(void*)(Cyc_Absyn_compress_conref(_tmp5DA))->v;
void*_tmp661;struct Cyc_Absyn_Exp*_tmp662;_LL48E: if(_tmp660 <= (void*)1?1:*((int*)
_tmp660)!= 0)goto _LL490;_tmp661=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp660)->f1;if(_tmp661 <= (void*)1?1:*((int*)_tmp661)!= 0)goto _LL490;_tmp662=((
struct Cyc_Absyn_Upper_b_struct*)_tmp661)->f1;_LL48F: Cyc_Tcexp_tcExp(te,0,_tmp662);
Cyc_Evexp_eval_const_uint_exp(_tmp662);goto _LL48D;_LL490:;_LL491: goto _LL48D;
_LL48D:;}goto _LL432;_LL443: if(_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 14)goto
_LL445;_tmp5DB=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp5C1)->f1;_LL444:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp5DB);goto _LL432;
_LL445: if(_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 17)goto _LL447;_tmp5DC=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp5C1)->f1;_LL446: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)5,_tmp5DC);goto _LL432;_LL447: if(_tmp5C1 <= (void*)3?1:*((int*)
_tmp5C1)!= 18)goto _LL449;_LL448: goto _LL44A;_LL449: if(_tmp5C1 <= (void*)3?1:*((int*)
_tmp5C1)!= 5)goto _LL44B;_LL44A: goto _LL44C;_LL44B: if((int)_tmp5C1 != 1)goto _LL44D;
_LL44C: goto _LL44E;_LL44D: if(_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 6)goto _LL44F;
_LL44E: goto _LL432;_LL44F: if(_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 7)goto _LL451;
_tmp5DD=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp5C1)->f1;_tmp5DE=((struct
Cyc_Absyn_ArrayType_struct*)_tmp5C1)->f2;_tmp5DF=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5C1)->f3;_LL450: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp5DD);if(_tmp5DF == 0)({void*_tmp663[0]={};Cyc_Tcutil_terr(loc,_tag_arr("an explicit array bound is required here",
sizeof(char),41),_tag_arr(_tmp663,sizeof(void*),0));});else{Cyc_Tcexp_tcExp(te,0,(
struct Cyc_Absyn_Exp*)_check_null(_tmp5DF));Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp5DF));}goto _LL432;_LL451: if(_tmp5C1 <= (void*)3?1:*((
int*)_tmp5C1)!= 8)goto _LL453;_tmp5E0=((struct Cyc_Absyn_FnType_struct*)_tmp5C1)->f1;
_tmp5E1=_tmp5E0.tvars;_tmp5E2=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp5C1)->f1).tvars;_tmp5E3=_tmp5E0.effect;_tmp5E4=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp5C1)->f1).effect;_tmp5E5=(void*)_tmp5E0.ret_typ;
_tmp5E6=_tmp5E0.args;_tmp5E7=_tmp5E0.c_varargs;_tmp5E8=_tmp5E0.cyc_varargs;
_tmp5E9=_tmp5E0.rgn_po;_tmp5EA=_tmp5E0.attributes;_LL452: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp5EA != 0;_tmp5EA=_tmp5EA->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp5EA->hd))({struct Cyc_Std_String_pa_struct
_tmp665;_tmp665.tag=0;_tmp665.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp5EA->hd);{void*_tmp664[1]={& _tmp665};Cyc_Tcutil_terr(loc,_tag_arr("bad function type attribute %s",
sizeof(char),31),_tag_arr(_tmp664,sizeof(void*),1));}});{void*_tmp666=(void*)
_tmp5EA->hd;void*_tmp667;int _tmp668;int _tmp669;_LL493: if((int)_tmp666 != 0)goto
_LL495;_LL494: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL492;_LL495:
if((int)_tmp666 != 1)goto _LL497;_LL496: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}
goto _LL492;_LL497: if((int)_tmp666 != 2)goto _LL499;_LL498: if(!seen_fastcall){
seen_fastcall=1;++ num_convs;}goto _LL492;_LL499: if(_tmp666 <= (void*)16?1:*((int*)
_tmp666)!= 3)goto _LL49B;_tmp667=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp666)->f1;_tmp668=((struct Cyc_Absyn_Format_att_struct*)_tmp666)->f2;_tmp669=((
struct Cyc_Absyn_Format_att_struct*)_tmp666)->f3;_LL49A: if(!seen_format){
seen_format=1;ft=_tmp667;fmt_desc_arg=_tmp668;fmt_arg_start=_tmp669;}else{({void*
_tmp66A[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function can't have multiple format attributes",
sizeof(char),47),_tag_arr(_tmp66A,sizeof(void*),0));});}goto _LL492;_LL49B:;
_LL49C: goto _LL492;_LL492:;}}if(num_convs > 1)({void*_tmp66B[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("function can't have multiple calling conventions",sizeof(char),49),
_tag_arr(_tmp66B,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp5E2);{
struct Cyc_List_List*b=*_tmp5E2;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp66C=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp66D;_LL49E: if(*((int*)_tmp66C)!= 0)goto
_LL4A0;_tmp66D=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp66C)->f1;if((int)
_tmp66D != 1)goto _LL4A0;_LL49F:({struct Cyc_Std_String_pa_struct _tmp66F;_tmp66F.tag=
0;_tmp66F.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name;{void*
_tmp66E[1]={& _tmp66F};Cyc_Tcutil_terr(loc,_tag_arr("function attempts to abstract Mem type variable %s",
sizeof(char),51),_tag_arr(_tmp66E,sizeof(void*),1));}});goto _LL49D;_LL4A0:;
_LL4A1: goto _LL49D;_LL49D:;}}}{struct Cyc_Tcutil_CVTEnv _tmp670=({struct Cyc_Tcutil_CVTEnv
_tmp6DE;_tmp6DE.kind_env=cvtenv.kind_env;_tmp6DE.free_vars=0;_tmp6DE.free_evars=
0;_tmp6DE.generalize_evars=cvtenv.generalize_evars;_tmp6DE.fn_result=1;_tmp6DE;});
_tmp670=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp670,(void*)1,_tmp5E5);_tmp670.fn_result=
0;{struct Cyc_List_List*a=_tmp5E6;for(0;a != 0;a=a->tl){_tmp670=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp670,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp5E8 != 0){if(
_tmp5E7)({void*_tmp671[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("both c_vararg and cyc_vararg",sizeof(char),29),
_tag_arr(_tmp671,sizeof(void*),0));});{void*_tmp673;int _tmp674;struct Cyc_Absyn_VarargInfo
_tmp672=*_tmp5E8;_tmp673=(void*)_tmp672.type;_tmp674=_tmp672.inject;_tmp670=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp670,(void*)1,_tmp673);if(_tmp674){void*_tmp675=Cyc_Tcutil_compress(
_tmp673);struct Cyc_Absyn_TunionInfo _tmp676;void*_tmp677;_LL4A3: if(_tmp675 <= (
void*)3?1:*((int*)_tmp675)!= 2)goto _LL4A5;_tmp676=((struct Cyc_Absyn_TunionType_struct*)
_tmp675)->f1;_tmp677=(void*)_tmp676.tunion_info;if(*((int*)_tmp677)!= 1)goto
_LL4A5;_LL4A4: goto _LL4A2;_LL4A5:;_LL4A6:({void*_tmp678[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("can't inject a non-[x]tunion type",sizeof(char),34),_tag_arr(_tmp678,
sizeof(void*),0));});goto _LL4A2;_LL4A2:;}}}if(seen_format){int _tmp679=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp5E6);if((((fmt_desc_arg < 0?1:
fmt_desc_arg > _tmp679)?1: fmt_arg_start < 0)?1:(_tmp5E8 == 0?fmt_arg_start != 0: 0))?
1:(_tmp5E8 != 0?fmt_arg_start != _tmp679 + 1: 0))({void*_tmp67A[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bad format descriptor",sizeof(char),22),_tag_arr(_tmp67A,sizeof(
void*),0));});else{void*_tmp67C;struct _tuple2 _tmp67B=*((struct _tuple2*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5E6,fmt_desc_arg - 1);_tmp67C=_tmp67B.f3;{
void*_tmp67D=Cyc_Tcutil_compress(_tmp67C);struct Cyc_Absyn_PtrInfo _tmp67E;void*
_tmp67F;struct Cyc_Absyn_Conref*_tmp680;_LL4A8: if(_tmp67D <= (void*)3?1:*((int*)
_tmp67D)!= 4)goto _LL4AA;_tmp67E=((struct Cyc_Absyn_PointerType_struct*)_tmp67D)->f1;
_tmp67F=(void*)_tmp67E.elt_typ;_tmp680=_tmp67E.bounds;_LL4A9:{struct _tuple6
_tmp682=({struct _tuple6 _tmp681;_tmp681.f1=Cyc_Tcutil_compress(_tmp67F);_tmp681.f2=
Cyc_Absyn_conref_def((void*)0,_tmp680);_tmp681;});void*_tmp683;void*_tmp684;void*
_tmp685;void*_tmp686;_LL4AD: _tmp683=_tmp682.f1;if(_tmp683 <= (void*)3?1:*((int*)
_tmp683)!= 5)goto _LL4AF;_tmp684=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp683)->f1;
if((int)_tmp684 != 2)goto _LL4AF;_tmp685=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp683)->f2;if((int)_tmp685 != 0)goto _LL4AF;_tmp686=_tmp682.f2;if((int)_tmp686 != 
0)goto _LL4AF;_LL4AE: goto _LL4AC;_LL4AF:;_LL4B0:({void*_tmp687[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("format descriptor is not a char ? type",sizeof(char),39),_tag_arr(
_tmp687,sizeof(void*),0));});goto _LL4AC;_LL4AC:;}goto _LL4A7;_LL4AA:;_LL4AB:({
void*_tmp688[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format descriptor is not a char ? type",
sizeof(char),39),_tag_arr(_tmp688,sizeof(void*),0));});goto _LL4A7;_LL4A7:;}if(
fmt_arg_start != 0){int problem;{struct _tuple6 _tmp68A=({struct _tuple6 _tmp689;
_tmp689.f1=ft;_tmp689.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp5E8))->type);_tmp689;});void*_tmp68B;void*_tmp68C;struct Cyc_Absyn_TunionInfo
_tmp68D;void*_tmp68E;struct Cyc_Absyn_Tuniondecl**_tmp68F;struct Cyc_Absyn_Tuniondecl*
_tmp690;void*_tmp691;void*_tmp692;struct Cyc_Absyn_TunionInfo _tmp693;void*_tmp694;
struct Cyc_Absyn_Tuniondecl**_tmp695;struct Cyc_Absyn_Tuniondecl*_tmp696;_LL4B2:
_tmp68B=_tmp68A.f1;if((int)_tmp68B != 0)goto _LL4B4;_tmp68C=_tmp68A.f2;if(_tmp68C
<= (void*)3?1:*((int*)_tmp68C)!= 2)goto _LL4B4;_tmp68D=((struct Cyc_Absyn_TunionType_struct*)
_tmp68C)->f1;_tmp68E=(void*)_tmp68D.tunion_info;if(*((int*)_tmp68E)!= 1)goto
_LL4B4;_tmp68F=((struct Cyc_Absyn_KnownTunion_struct*)_tmp68E)->f1;_tmp690=*
_tmp68F;_LL4B3: problem=Cyc_Absyn_qvar_cmp(_tmp690->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL4B1;_LL4B4: _tmp691=_tmp68A.f1;if((int)_tmp691 != 1)goto _LL4B6;_tmp692=
_tmp68A.f2;if(_tmp692 <= (void*)3?1:*((int*)_tmp692)!= 2)goto _LL4B6;_tmp693=((
struct Cyc_Absyn_TunionType_struct*)_tmp692)->f1;_tmp694=(void*)_tmp693.tunion_info;
if(*((int*)_tmp694)!= 1)goto _LL4B6;_tmp695=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp694)->f1;_tmp696=*_tmp695;_LL4B5: problem=Cyc_Absyn_qvar_cmp(_tmp696->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL4B1;_LL4B6:;_LL4B7: problem=1;goto
_LL4B1;_LL4B1:;}if(problem)({void*_tmp697[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format attribute and vararg types don't match",
sizeof(char),46),_tag_arr(_tmp697,sizeof(void*),0));});}}}{struct Cyc_List_List*
rpo=_tmp5E9;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp699;void*_tmp69A;void*
_tmp69B;struct _tuple6*_tmp698=(struct _tuple6*)rpo->hd;_tmp699=*_tmp698;_tmp69A=
_tmp699.f1;_tmp69B=_tmp699.f2;_tmp670=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp670,(void*)4,_tmp69A);_tmp670=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp670,(
void*)3,_tmp69B);}}if(*_tmp5E4 != 0)_tmp670=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp670,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp5E4))->v);else{
struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp670.free_vars;for(0;
tvs != 0;tvs=tvs->tl){void*_tmp69C=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmp69D;struct Cyc_Core_Opt**_tmp69E;void*
_tmp69F;void*_tmp6A0;struct Cyc_Core_Opt*_tmp6A1;struct Cyc_Core_Opt**_tmp6A2;void*
_tmp6A3;void*_tmp6A4;struct Cyc_Core_Opt*_tmp6A5;struct Cyc_Core_Opt**_tmp6A6;
_LL4B9: if(*((int*)_tmp69C)!= 2)goto _LL4BB;_tmp69D=((struct Cyc_Absyn_Less_kb_struct*)
_tmp69C)->f1;_tmp69E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp69C)->f1;_tmp69F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp69C)->f2;if((
int)_tmp69F != 3)goto _LL4BB;_LL4BA:*_tmp69E=({struct Cyc_Core_Opt*_tmp6A7=
_cycalloc(sizeof(*_tmp6A7));_tmp6A7->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp6A9;_tmp6A9.tag=0;_tmp6A9.f1=(void*)((void*)3);_tmp6A9;});_tmp6A8;}));
_tmp6A7;});goto _LL4BC;_LL4BB: if(*((int*)_tmp69C)!= 0)goto _LL4BD;_tmp6A0=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp69C)->f1;if((int)_tmp6A0 != 3)goto _LL4BD;_LL4BC:
effect=({struct Cyc_List_List*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6AB=_cycalloc(sizeof(*
_tmp6AB));_tmp6AB[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6AC;_tmp6AC.tag=19;
_tmp6AC.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6AD=_cycalloc(
sizeof(*_tmp6AD));_tmp6AD[0]=({struct Cyc_Absyn_VarType_struct _tmp6AE;_tmp6AE.tag=
1;_tmp6AE.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6AE;});_tmp6AD;}));_tmp6AC;});
_tmp6AB;}));_tmp6AA->tl=effect;_tmp6AA;});goto _LL4B8;_LL4BD: if(*((int*)_tmp69C)
!= 2)goto _LL4BF;_tmp6A1=((struct Cyc_Absyn_Less_kb_struct*)_tmp69C)->f1;_tmp6A2=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp69C)->f1;_tmp6A3=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp69C)->f2;if((int)_tmp6A3 != 4)goto
_LL4BF;_LL4BE:*_tmp6A2=({struct Cyc_Core_Opt*_tmp6AF=_cycalloc(sizeof(*_tmp6AF));
_tmp6AF->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6B0=_cycalloc(
sizeof(*_tmp6B0));_tmp6B0[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6B1;_tmp6B1.tag=0;
_tmp6B1.f1=(void*)((void*)4);_tmp6B1;});_tmp6B0;}));_tmp6AF;});goto _LL4C0;_LL4BF:
if(*((int*)_tmp69C)!= 0)goto _LL4C1;_tmp6A4=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp69C)->f1;if((int)_tmp6A4 != 4)goto _LL4C1;_LL4C0: effect=({struct Cyc_List_List*
_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3[0]=({struct Cyc_Absyn_VarType_struct
_tmp6B4;_tmp6B4.tag=1;_tmp6B4.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6B4;});
_tmp6B3;}));_tmp6B2->tl=effect;_tmp6B2;});goto _LL4B8;_LL4C1: if(*((int*)_tmp69C)
!= 1)goto _LL4C3;_tmp6A5=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp69C)->f1;
_tmp6A6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp69C)->f1;
_LL4C2:*_tmp6A6=({struct Cyc_Core_Opt*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));
_tmp6B6[0]=({struct Cyc_Absyn_Less_kb_struct _tmp6B7;_tmp6B7.tag=2;_tmp6B7.f1=0;
_tmp6B7.f2=(void*)((void*)0);_tmp6B7;});_tmp6B6;}));_tmp6B5;});goto _LL4C4;_LL4C3:;
_LL4C4: effect=({struct Cyc_List_List*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));
_tmp6B9[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6BA;_tmp6BA.tag=21;_tmp6BA.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));
_tmp6BB[0]=({struct Cyc_Absyn_VarType_struct _tmp6BC;_tmp6BC.tag=1;_tmp6BC.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6BC;});_tmp6BB;}));_tmp6BA;});_tmp6B9;}));
_tmp6B8->tl=effect;_tmp6B8;});goto _LL4B8;_LL4B8:;}}{struct Cyc_List_List*ts=
_tmp670.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp6BD=Cyc_Tcutil_typ_kind((
void*)ts->hd);_LL4C6: if((int)_tmp6BD != 3)goto _LL4C8;_LL4C7: effect=({struct Cyc_List_List*
_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp6C0;_tmp6C0.tag=19;_tmp6C0.f1=(void*)((void*)ts->hd);_tmp6C0;});_tmp6BF;}));
_tmp6BE->tl=effect;_tmp6BE;});goto _LL4C5;_LL4C8: if((int)_tmp6BD != 4)goto _LL4CA;
_LL4C9: effect=({struct Cyc_List_List*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->hd=(
void*)((void*)ts->hd);_tmp6C1->tl=effect;_tmp6C1;});goto _LL4C5;_LL4CA:;_LL4CB:
effect=({struct Cyc_List_List*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));
_tmp6C3[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6C4;_tmp6C4.tag=21;_tmp6C4.f1=(
void*)((void*)ts->hd);_tmp6C4;});_tmp6C3;}));_tmp6C2->tl=effect;_tmp6C2;});goto
_LL4C5;_LL4C5:;}}*_tmp5E4=({struct Cyc_Core_Opt*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));
_tmp6C5->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp6C6=_cycalloc(
sizeof(*_tmp6C6));_tmp6C6[0]=({struct Cyc_Absyn_JoinEff_struct _tmp6C7;_tmp6C7.tag=
20;_tmp6C7.f1=effect;_tmp6C7;});_tmp6C6;}));_tmp6C5;});}if(*_tmp5E2 != 0){struct
Cyc_List_List*bs=*_tmp5E2;for(0;bs != 0;bs=bs->tl){void*_tmp6C8=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmp6C9;struct
Cyc_Core_Opt**_tmp6CA;struct Cyc_Core_Opt*_tmp6CB;struct Cyc_Core_Opt**_tmp6CC;
void*_tmp6CD;struct Cyc_Core_Opt*_tmp6CE;struct Cyc_Core_Opt**_tmp6CF;void*_tmp6D0;
struct Cyc_Core_Opt*_tmp6D1;struct Cyc_Core_Opt**_tmp6D2;void*_tmp6D3;void*_tmp6D4;
_LL4CD: if(*((int*)_tmp6C8)!= 1)goto _LL4CF;_tmp6C9=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6C8)->f1;_tmp6CA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6C8)->f1;_LL4CE:({struct Cyc_Std_String_pa_struct _tmp6D6;_tmp6D6.tag=0;
_tmp6D6.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name;{void*
_tmp6D5[1]={& _tmp6D6};Cyc_Tcutil_warn(loc,_tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof(char),47),_tag_arr(_tmp6D5,sizeof(void*),1));}});_tmp6CC=_tmp6CA;goto
_LL4D0;_LL4CF: if(*((int*)_tmp6C8)!= 2)goto _LL4D1;_tmp6CB=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6C8)->f1;_tmp6CC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6C8)->f1;_tmp6CD=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6C8)->f2;if((
int)_tmp6CD != 1)goto _LL4D1;_LL4D0: _tmp6CF=_tmp6CC;goto _LL4D2;_LL4D1: if(*((int*)
_tmp6C8)!= 2)goto _LL4D3;_tmp6CE=((struct Cyc_Absyn_Less_kb_struct*)_tmp6C8)->f1;
_tmp6CF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6C8)->f1;
_tmp6D0=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6C8)->f2;if((int)_tmp6D0 != 
0)goto _LL4D3;_LL4D2:*_tmp6CF=({struct Cyc_Core_Opt*_tmp6D7=_cycalloc(sizeof(*
_tmp6D7));_tmp6D7->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6D8=
_cycalloc(sizeof(*_tmp6D8));_tmp6D8[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6D9;
_tmp6D9.tag=0;_tmp6D9.f1=(void*)((void*)2);_tmp6D9;});_tmp6D8;}));_tmp6D7;});
goto _LL4CC;_LL4D3: if(*((int*)_tmp6C8)!= 2)goto _LL4D5;_tmp6D1=((struct Cyc_Absyn_Less_kb_struct*)
_tmp6C8)->f1;_tmp6D2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp6C8)->f1;_tmp6D3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6C8)->f2;
_LL4D4:*_tmp6D2=({struct Cyc_Core_Opt*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));
_tmp6DB[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6DC;_tmp6DC.tag=0;_tmp6DC.f1=(void*)
_tmp6D3;_tmp6DC;});_tmp6DB;}));_tmp6DA;});goto _LL4CC;_LL4D5: if(*((int*)_tmp6C8)
!= 0)goto _LL4D7;_tmp6D4=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp6C8)->f1;if((
int)_tmp6D4 != 1)goto _LL4D7;_LL4D6:({void*_tmp6DD[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("functions can't abstract M types",sizeof(char),33),_tag_arr(_tmp6DD,
sizeof(void*),0));});goto _LL4CC;_LL4D7:;_LL4D8: goto _LL4CC;_LL4CC:;}}cvtenv.kind_env=
Cyc_Tcutil_remove_bound_tvars(_tmp670.kind_env,*_tmp5E2);_tmp670.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp670.free_vars,*_tmp5E2);{struct Cyc_List_List*tvs=_tmp670.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp670.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL432;}}_LL453: if(_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 
9)goto _LL455;_tmp5EB=((struct Cyc_Absyn_TupleType_struct*)_tmp5C1)->f1;_LL454:
for(0;_tmp5EB != 0;_tmp5EB=_tmp5EB->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,(void*)1,(*((struct _tuple4*)_tmp5EB->hd)).f2);}goto _LL432;_LL455: if(
_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 11)goto _LL457;_tmp5EC=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5C1)->f1;_tmp5ED=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5C1)->f2;_LL456:{
struct _RegionHandle _tmp6DF=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp6DF;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp5ED != 0;_tmp5ED=_tmp5ED->tl){struct Cyc_Absyn_Aggrfield _tmp6E1;struct
_tagged_arr*_tmp6E2;struct Cyc_Absyn_Tqual _tmp6E3;void*_tmp6E4;struct Cyc_Absyn_Exp*
_tmp6E5;struct Cyc_List_List*_tmp6E6;struct Cyc_Absyn_Aggrfield*_tmp6E0=(struct Cyc_Absyn_Aggrfield*)
_tmp5ED->hd;_tmp6E1=*_tmp6E0;_tmp6E2=_tmp6E1.name;_tmp6E3=_tmp6E1.tq;_tmp6E4=(
void*)_tmp6E1.type;_tmp6E5=_tmp6E1.width;_tmp6E6=_tmp6E1.attributes;if(((int(*)(
int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,
struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,prev_fields,_tmp6E2))({
struct Cyc_Std_String_pa_struct _tmp6E8;_tmp6E8.tag=0;_tmp6E8.f1=(struct
_tagged_arr)*_tmp6E2;{void*_tmp6E7[1]={& _tmp6E8};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s",
sizeof(char),19),_tag_arr(_tmp6E7,sizeof(void*),1));}});if(Cyc_Std_strcmp(*
_tmp6E2,_tag_arr("",sizeof(char),1))!= 0)prev_fields=({struct Cyc_List_List*
_tmp6E9=_region_malloc(aprev_rgn,sizeof(*_tmp6E9));_tmp6E9->hd=_tmp6E2;_tmp6E9->tl=
prev_fields;_tmp6E9;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp6E4);if(_tmp5EC == (void*)1?!Cyc_Tcutil_bits_only(_tmp6E4): 0)({struct Cyc_Std_String_pa_struct
_tmp6EB;_tmp6EB.tag=0;_tmp6EB.f1=(struct _tagged_arr)*_tmp6E2;{void*_tmp6EA[1]={&
_tmp6EB};Cyc_Tcutil_terr(loc,_tag_arr("union member %s has a non-integral type",
sizeof(char),40),_tag_arr(_tmp6EA,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(
loc,te,_tmp6E4,_tmp6E5,_tmp6E2);Cyc_Tcutil_check_field_atts(loc,_tmp6E2,_tmp6E6);}};
_pop_region(aprev_rgn);}goto _LL432;_LL457: if(_tmp5C1 <= (void*)3?1:*((int*)
_tmp5C1)!= 10)goto _LL459;_tmp5EE=((struct Cyc_Absyn_AggrType_struct*)_tmp5C1)->f1;
_tmp5EF=(void*)_tmp5EE.aggr_info;_tmp5F0=(void**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp5C1)->f1).aggr_info;_tmp5F1=_tmp5EE.targs;_tmp5F2=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp5C1)->f1).targs;_LL458:{void*_tmp6EC=*
_tmp5F0;void*_tmp6ED;struct _tuple1*_tmp6EE;struct Cyc_Absyn_Aggrdecl**_tmp6EF;
struct Cyc_Absyn_Aggrdecl*_tmp6F0;_LL4DA: if(*((int*)_tmp6EC)!= 0)goto _LL4DC;
_tmp6ED=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp6EC)->f1;_tmp6EE=((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp6EC)->f2;_LL4DB: {struct Cyc_Absyn_Aggrdecl**
adp;{struct _handler_cons _tmp6F1;_push_handler(& _tmp6F1);{int _tmp6F3=0;if(setjmp(
_tmp6F1.handler))_tmp6F3=1;if(!_tmp6F3){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,
_tmp6EE);*_tmp5F0=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp6F4=_cycalloc(
sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp6F5;_tmp6F5.tag=
1;_tmp6F5.f1=adp;_tmp6F5;});_tmp6F4;});;_pop_handler();}else{void*_tmp6F2=(void*)
_exn_thrown;void*_tmp6F7=_tmp6F2;_LL4DF: if(_tmp6F7 != Cyc_Dict_Absent)goto _LL4E1;
_LL4E0: {struct Cyc_Tcenv_Genv*_tmp6F8=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Aggrdecl*
_tmp6F9=({struct Cyc_Absyn_Aggrdecl*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE->kind=(
void*)_tmp6ED;_tmp6FE->sc=(void*)((void*)3);_tmp6FE->name=_tmp6EE;_tmp6FE->tvs=0;
_tmp6FE->impl=0;_tmp6FE->attributes=0;_tmp6FE;});Cyc_Tc_tcAggrdecl(te,_tmp6F8,
loc,_tmp6F9);adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp6EE);*_tmp5F0=(void*)({
struct Cyc_Absyn_KnownAggr_struct*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({
struct Cyc_Absyn_KnownAggr_struct _tmp6FB;_tmp6FB.tag=1;_tmp6FB.f1=adp;_tmp6FB;});
_tmp6FA;});if(*_tmp5F2 != 0){({struct Cyc_Std_String_pa_struct _tmp6FD;_tmp6FD.tag=
0;_tmp6FD.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp6EE);{void*_tmp6FC[1]={&
_tmp6FD};Cyc_Tcutil_terr(loc,_tag_arr("declare parameterized type %s before using",
sizeof(char),43),_tag_arr(_tmp6FC,sizeof(void*),1));}});return cvtenv;}goto _LL4DE;}
_LL4E1:;_LL4E2:(void)_throw(_tmp6F7);_LL4DE:;}}}_tmp6F0=*adp;goto _LL4DD;}_LL4DC:
if(*((int*)_tmp6EC)!= 1)goto _LL4D9;_tmp6EF=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp6EC)->f1;_tmp6F0=*_tmp6EF;_LL4DD: {struct Cyc_List_List*tvs=_tmp6F0->tvs;
struct Cyc_List_List*ts=*_tmp5F2;for(0;ts != 0?tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_Std_String_pa_struct
_tmp700;_tmp700.tag=0;_tmp700.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp6F0->name);{void*_tmp6FF[1]={& _tmp700};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for type %s",
sizeof(char),32),_tag_arr(_tmp6FF,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp701=
_cycalloc(sizeof(*_tmp701));_tmp701->hd=(void*)e;_tmp701->tl=hidden_ts;_tmp701;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*_tmp5F2=Cyc_List_imp_append(*
_tmp5F2,Cyc_List_imp_rev(hidden_ts));}}_LL4D9:;}goto _LL432;_LL459: if(_tmp5C1 <= (
void*)3?1:*((int*)_tmp5C1)!= 16)goto _LL45B;_tmp5F3=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5C1)->f1;_tmp5F4=((struct Cyc_Absyn_TypedefType_struct*)_tmp5C1)->f2;_tmp5F5=(
struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp5C1)->f2;
_tmp5F6=((struct Cyc_Absyn_TypedefType_struct*)_tmp5C1)->f3;_tmp5F7=(struct Cyc_Absyn_Typedefdecl**)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp5C1)->f3;_tmp5F8=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5C1)->f4;_tmp5F9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp5C1)->f4;_LL45A: {struct Cyc_List_List*targs=*_tmp5F5;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmp702;_push_handler(& _tmp702);{int _tmp704=0;if(setjmp(
_tmp702.handler))_tmp704=1;if(!_tmp704){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmp5F3);;_pop_handler();}else{void*_tmp703=(void*)_exn_thrown;void*_tmp706=
_tmp703;_LL4E4: if(_tmp706 != Cyc_Dict_Absent)goto _LL4E6;_LL4E5:({struct Cyc_Std_String_pa_struct
_tmp708;_tmp708.tag=0;_tmp708.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp5F3);{void*_tmp707[1]={& _tmp708};Cyc_Tcutil_terr(loc,_tag_arr("unbound typedef name %s",
sizeof(char),24),_tag_arr(_tmp707,sizeof(void*),1));}});return cvtenv;_LL4E6:;
_LL4E7:(void)_throw(_tmp706);_LL4E3:;}}}*_tmp5F7=(struct Cyc_Absyn_Typedefdecl*)
td;_tmp5F3[0]=(td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0?
tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709->hd=({
struct _tuple8*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp70A->f2=(void*)ts->hd;_tmp70A;});_tmp709->tl=inst;_tmp709;});}if(ts
!= 0)({struct Cyc_Std_String_pa_struct _tmp70C;_tmp70C.tag=0;_tmp70C.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp5F3);{void*_tmp70B[1]={& _tmp70C};Cyc_Tcutil_terr(
loc,_tag_arr("too many parameters for typedef %s",sizeof(char),35),_tag_arr(
_tmp70B,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp70D=_cycalloc(
sizeof(*_tmp70D));_tmp70D->hd=(void*)e;_tmp70D->tl=hidden_ts;_tmp70D;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E->hd=({struct _tuple8*_tmp70F=
_cycalloc(sizeof(*_tmp70F));_tmp70F->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp70F->f2=
e;_tmp70F;});_tmp70E->tl=inst;_tmp70E;});}*_tmp5F5=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp5F9=({struct Cyc_Core_Opt*
_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710->v=(void*)new_typ;_tmp710;});}goto
_LL432;}}_LL45B: if((int)_tmp5C1 != 2)goto _LL45D;_LL45C: goto _LL432;_LL45D: if(
_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 15)goto _LL45F;_tmp5FA=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5C1)->f1;_LL45E: _tmp5FB=_tmp5FA;goto _LL460;_LL45F: if(_tmp5C1 <= (void*)3?1:*((
int*)_tmp5C1)!= 19)goto _LL461;_tmp5FB=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp5C1)->f1;_LL460: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp5FB);goto _LL432;_LL461: if(_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 21)goto
_LL463;_tmp5FC=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp5C1)->f1;_LL462:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp5FC);goto _LL432;
_LL463: if(_tmp5C1 <= (void*)3?1:*((int*)_tmp5C1)!= 20)goto _LL432;_tmp5FD=((struct
Cyc_Absyn_JoinEff_struct*)_tmp5C1)->f1;_LL464: for(0;_tmp5FD != 0;_tmp5FD=_tmp5FD->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)_tmp5FD->hd);}
goto _LL432;_LL432:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))({
struct Cyc_Std_String_pa_struct _tmp714;_tmp714.tag=0;_tmp714.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(expected_kind);{struct Cyc_Std_String_pa_struct
_tmp713;_tmp713.tag=0;_tmp713.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t));{struct Cyc_Std_String_pa_struct _tmp712;_tmp712.tag=0;_tmp712.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp711[3]={& _tmp712,& _tmp713,&
_tmp714};Cyc_Tcutil_terr(loc,_tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof(char),51),_tag_arr(_tmp711,sizeof(void*),3));}}}});return cvtenv;}static
struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp715=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp71E;_tmp71E.kind_env=kind_env;_tmp71E.free_vars=
0;_tmp71E.free_evars=0;_tmp71E.generalize_evars=generalize_evars;_tmp71E.fn_result=
0;_tmp71E;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp715.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp715.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp715.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp716=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp717;struct Cyc_Core_Opt**_tmp718;_LL4E9: if(_tmp716 <= (void*)3?1:*((int*)
_tmp716)!= 0)goto _LL4EB;_tmp717=((struct Cyc_Absyn_Evar_struct*)_tmp716)->f4;
_tmp718=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp716)->f4;
_LL4EA: if(*_tmp718 == 0)*_tmp718=({struct Cyc_Core_Opt*_tmp719=_cycalloc(sizeof(*
_tmp719));_tmp719->v=kind_env;_tmp719;});else{struct Cyc_List_List*_tmp71A=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp718))->v;struct Cyc_List_List*
_tmp71B=0;for(0;_tmp71A != 0;_tmp71A=_tmp71A->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)
_tmp71A->hd))_tmp71B=({struct Cyc_List_List*_tmp71C=_cycalloc(sizeof(*_tmp71C));
_tmp71C->hd=(struct Cyc_Absyn_Tvar*)_tmp71A->hd;_tmp71C->tl=_tmp71B;_tmp71C;});}*
_tmp718=({struct Cyc_Core_Opt*_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D->v=
_tmp71B;_tmp71D;});}goto _LL4E8;_LL4EB:;_LL4EC: goto _LL4E8;_LL4E8:;}}return _tmp715;}
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_Tcutil_CVTEnv _tmp71F=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp720=_tmp71F.free_vars;struct Cyc_List_List*
_tmp721=_tmp71F.free_evars;{struct Cyc_List_List*x=_tmp720;for(0;x != 0;x=x->tl){
void*_tmp722=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp723;struct Cyc_Core_Opt**_tmp724;struct Cyc_Core_Opt*
_tmp725;struct Cyc_Core_Opt**_tmp726;void*_tmp727;struct Cyc_Core_Opt*_tmp728;
struct Cyc_Core_Opt**_tmp729;void*_tmp72A;struct Cyc_Core_Opt*_tmp72B;struct Cyc_Core_Opt**
_tmp72C;void*_tmp72D;void*_tmp72E;_LL4EE: if(*((int*)_tmp722)!= 1)goto _LL4F0;
_tmp723=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp722)->f1;_tmp724=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp722)->f1;_LL4EF: _tmp726=_tmp724;goto
_LL4F1;_LL4F0: if(*((int*)_tmp722)!= 2)goto _LL4F2;_tmp725=((struct Cyc_Absyn_Less_kb_struct*)
_tmp722)->f1;_tmp726=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp722)->f1;_tmp727=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp722)->f2;if((
int)_tmp727 != 1)goto _LL4F2;_LL4F1: _tmp729=_tmp726;goto _LL4F3;_LL4F2: if(*((int*)
_tmp722)!= 2)goto _LL4F4;_tmp728=((struct Cyc_Absyn_Less_kb_struct*)_tmp722)->f1;
_tmp729=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp722)->f1;
_tmp72A=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp722)->f2;if((int)_tmp72A != 
0)goto _LL4F4;_LL4F3:*_tmp729=({struct Cyc_Core_Opt*_tmp72F=_cycalloc(sizeof(*
_tmp72F));_tmp72F->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp730=
_cycalloc(sizeof(*_tmp730));_tmp730[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp731;
_tmp731.tag=0;_tmp731.f1=(void*)((void*)2);_tmp731;});_tmp730;}));_tmp72F;});
goto _LL4ED;_LL4F4: if(*((int*)_tmp722)!= 2)goto _LL4F6;_tmp72B=((struct Cyc_Absyn_Less_kb_struct*)
_tmp722)->f1;_tmp72C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp722)->f1;_tmp72D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp722)->f2;
_LL4F5:*_tmp72C=({struct Cyc_Core_Opt*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp733=_cycalloc(sizeof(*_tmp733));
_tmp733[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp734;_tmp734.tag=0;_tmp734.f1=(void*)
_tmp72D;_tmp734;});_tmp733;}));_tmp732;});goto _LL4ED;_LL4F6: if(*((int*)_tmp722)
!= 0)goto _LL4F8;_tmp72E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp722)->f1;if((
int)_tmp72E != 1)goto _LL4F8;_LL4F7:({struct Cyc_Std_String_pa_struct _tmp736;
_tmp736.tag=0;_tmp736.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd);{void*_tmp735[1]={& _tmp736};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s cannot have kind M",
sizeof(char),36),_tag_arr(_tmp735,sizeof(void*),1));}});goto _LL4ED;_LL4F8:;
_LL4F9: goto _LL4ED;_LL4ED:;}}if(_tmp720 != 0?1: _tmp721 != 0){{void*_tmp737=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp738;struct Cyc_List_List*_tmp739;struct Cyc_List_List**
_tmp73A;struct Cyc_Core_Opt*_tmp73B;void*_tmp73C;struct Cyc_List_List*_tmp73D;int
_tmp73E;struct Cyc_Absyn_VarargInfo*_tmp73F;struct Cyc_List_List*_tmp740;struct Cyc_List_List*
_tmp741;_LL4FB: if(_tmp737 <= (void*)3?1:*((int*)_tmp737)!= 8)goto _LL4FD;_tmp738=((
struct Cyc_Absyn_FnType_struct*)_tmp737)->f1;_tmp739=_tmp738.tvars;_tmp73A=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp737)->f1).tvars;
_tmp73B=_tmp738.effect;_tmp73C=(void*)_tmp738.ret_typ;_tmp73D=_tmp738.args;
_tmp73E=_tmp738.c_varargs;_tmp73F=_tmp738.cyc_varargs;_tmp740=_tmp738.rgn_po;
_tmp741=_tmp738.attributes;_LL4FC: if(*_tmp73A == 0){*_tmp73A=_tmp720;_tmp720=0;}
goto _LL4FA;_LL4FD:;_LL4FE: goto _LL4FA;_LL4FA:;}if(_tmp720 != 0)({struct Cyc_Std_String_pa_struct
_tmp743;_tmp743.tag=0;_tmp743.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)
_tmp720->hd)->name;{void*_tmp742[1]={& _tmp743};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s",
sizeof(char),25),_tag_arr(_tmp742,sizeof(void*),1));}});if(_tmp721 != 0)for(0;
_tmp721 != 0;_tmp721=_tmp721->tl){void*e=(void*)_tmp721->hd;void*_tmp744=Cyc_Tcutil_typ_kind(
e);_LL500: if((int)_tmp744 != 3)goto _LL502;_LL501: if(!Cyc_Tcutil_unify(e,(void*)2))({
void*_tmp745[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with heap!",sizeof(char),28),_tag_arr(_tmp745,sizeof(
void*),0));});goto _LL4FF;_LL502: if((int)_tmp744 != 4)goto _LL504;_LL503: if(!Cyc_Tcutil_unify(
e,Cyc_Absyn_empty_effect))({void*_tmp746[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(char),26),_tag_arr(_tmp746,sizeof(void*),0));});goto _LL4FF;_LL504:;_LL505:({
struct Cyc_Std_String_pa_struct _tmp749;_tmp749.tag=0;_tmp749.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp748;
_tmp748.tag=0;_tmp748.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp747[2]={& _tmp748,& _tmp749};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp747,sizeof(void*),2));}}});goto _LL4FF;_LL4FF:;}}}
void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(
loc,te,t);{void*_tmp74A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp74B;
struct Cyc_List_List*_tmp74C;struct Cyc_Core_Opt*_tmp74D;_LL507: if(_tmp74A <= (void*)
3?1:*((int*)_tmp74A)!= 8)goto _LL509;_tmp74B=((struct Cyc_Absyn_FnType_struct*)
_tmp74A)->f1;_tmp74C=_tmp74B.tvars;_tmp74D=_tmp74B.effect;_LL508: fd->tvs=_tmp74C;
fd->effect=_tmp74D;goto _LL506;_LL509:;_LL50A:({void*_tmp74E[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("check_fndecl_valid_type: not a FnType",
sizeof(char),38),_tag_arr(_tmp74E,sizeof(void*),0));});_LL506:;}{struct
_RegionHandle _tmp74F=_new_region("r");struct _RegionHandle*r=& _tmp74F;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tagged_arr*(*f)(struct _tuple13*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct _tuple13*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,_tag_arr("function declaration has repeated parameter",sizeof(char),
44));;_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*_tmp750=_cycalloc(
sizeof(*_tmp750));_tmp750->v=(void*)t;_tmp750;});}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp751=Cyc_Tcutil_check_valid_type(loc,te,bound_tvars,expected_kind,0,t);struct
Cyc_List_List*_tmp752=Cyc_Tcutil_remove_bound_tvars(_tmp751.free_vars,
bound_tvars);struct Cyc_List_List*_tmp753=_tmp751.free_evars;{struct Cyc_List_List*
fs=_tmp752;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp754=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;({struct Cyc_Std_String_pa_struct _tmp757;_tmp757.tag=0;_tmp757.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp756;_tmp756.tag=0;_tmp756.f1=(struct _tagged_arr)*_tmp754;{void*_tmp755[2]={&
_tmp756,& _tmp757};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s in type %s",
sizeof(char),36),_tag_arr(_tmp755,sizeof(void*),2));}}});}}if(!allow_evars?
_tmp753 != 0: 0)for(0;_tmp753 != 0;_tmp753=_tmp753->tl){void*e=(void*)_tmp753->hd;
void*_tmp758=Cyc_Tcutil_typ_kind(e);_LL50C: if((int)_tmp758 != 3)goto _LL50E;_LL50D:
if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp759[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(char),28),_tag_arr(_tmp759,sizeof(void*),0));});goto _LL50B;_LL50E: if((int)
_tmp758 != 4)goto _LL510;_LL50F: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp75A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!",sizeof(char),26),_tag_arr(_tmp75A,sizeof(
void*),0));});goto _LL50B;_LL510:;_LL511:({struct Cyc_Std_String_pa_struct _tmp75D;
_tmp75D.tag=0;_tmp75D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp75C;_tmp75C.tag=0;_tmp75C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{
void*_tmp75B[2]={& _tmp75C,& _tmp75D};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(char),52),_tag_arr(_tmp75B,sizeof(void*),2));}}});goto _LL50B;_LL50B:;}}
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=
Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*
tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _tagged_arr(*a2string)(void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0)({struct Cyc_Std_String_pa_struct _tmp760;_tmp760.tag=0;_tmp760.f1=(
struct _tagged_arr)a2string((void*)vs->hd);{struct Cyc_Std_String_pa_struct _tmp75F;
_tmp75F.tag=0;_tmp75F.f1=(struct _tagged_arr)msg;{void*_tmp75E[2]={& _tmp75F,&
_tmp760};Cyc_Tcutil_terr(loc,_tag_arr("%s: %s",sizeof(char),7),_tag_arr(_tmp75E,
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
fields=({struct Cyc_List_List*_tmp761=_cycalloc(sizeof(*_tmp761));_tmp761->hd=({
struct _tuple18*_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp762->f2=0;_tmp762;});_tmp761->tl=fields;_tmp761;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple19 _tmp764;struct Cyc_List_List*
_tmp765;void*_tmp766;struct _tuple19*_tmp763=(struct _tuple19*)des->hd;_tmp764=*
_tmp763;_tmp765=_tmp764.f1;_tmp766=_tmp764.f2;if(_tmp765 == 0){struct Cyc_List_List*
_tmp767=fields;for(0;_tmp767 != 0;_tmp767=_tmp767->tl){if(!(*((struct _tuple18*)
_tmp767->hd)).f2){(*((struct _tuple18*)_tmp767->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp768=_cycalloc(sizeof(*_tmp768));
_tmp768->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp769=_cycalloc(
sizeof(*_tmp769));_tmp769[0]=({struct Cyc_Absyn_FieldName_struct _tmp76A;_tmp76A.tag=
1;_tmp76A.f1=((*((struct _tuple18*)_tmp767->hd)).f1)->name;_tmp76A;});_tmp769;}));
_tmp768->tl=0;_tmp768;});ans=({struct Cyc_List_List*_tmp76B=_region_malloc(rgn,
sizeof(*_tmp76B));_tmp76B->hd=({struct _tuple20*_tmp76C=_region_malloc(rgn,
sizeof(*_tmp76C));_tmp76C->f1=(*((struct _tuple18*)_tmp767->hd)).f1;_tmp76C->f2=
_tmp766;_tmp76C;});_tmp76B->tl=ans;_tmp76B;});break;}}if(_tmp767 == 0)({void*
_tmp76D[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too many arguments to struct",
sizeof(char),29),_tag_arr(_tmp76D,sizeof(void*),0));});}else{if(_tmp765->tl != 0)({
void*_tmp76E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("multiple designators are not supported",
sizeof(char),39),_tag_arr(_tmp76E,sizeof(void*),0));});else{void*_tmp76F=(void*)
_tmp765->hd;struct _tagged_arr*_tmp770;_LL513: if(*((int*)_tmp76F)!= 0)goto _LL515;
_LL514:({void*_tmp771[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array designator used in argument to struct",
sizeof(char),44),_tag_arr(_tmp771,sizeof(void*),0));});goto _LL512;_LL515: if(*((
int*)_tmp76F)!= 1)goto _LL512;_tmp770=((struct Cyc_Absyn_FieldName_struct*)_tmp76F)->f1;
_LL516: {struct Cyc_List_List*_tmp772=fields;for(0;_tmp772 != 0;_tmp772=_tmp772->tl){
if(Cyc_Std_zstrptrcmp(_tmp770,((*((struct _tuple18*)_tmp772->hd)).f1)->name)== 0){
if((*((struct _tuple18*)_tmp772->hd)).f2)({struct Cyc_Std_String_pa_struct _tmp774;
_tmp774.tag=0;_tmp774.f1=(struct _tagged_arr)*_tmp770;{void*_tmp773[1]={& _tmp774};
Cyc_Tcutil_terr(loc,_tag_arr("field %s has already been used as an argument",
sizeof(char),46),_tag_arr(_tmp773,sizeof(void*),1));}});(*((struct _tuple18*)
_tmp772->hd)).f2=1;ans=({struct Cyc_List_List*_tmp775=_region_malloc(rgn,sizeof(*
_tmp775));_tmp775->hd=({struct _tuple20*_tmp776=_region_malloc(rgn,sizeof(*
_tmp776));_tmp776->f1=(*((struct _tuple18*)_tmp772->hd)).f1;_tmp776->f2=_tmp766;
_tmp776;});_tmp775->tl=ans;_tmp775;});break;}}if(_tmp772 == 0)({struct Cyc_Std_String_pa_struct
_tmp778;_tmp778.tag=0;_tmp778.f1=(struct _tagged_arr)*_tmp770;{void*_tmp777[1]={&
_tmp778};Cyc_Tcutil_terr(loc,_tag_arr("bad field designator %s",sizeof(char),24),
_tag_arr(_tmp777,sizeof(void*),1));}});goto _LL512;}_LL512:;}}}for(0;fields != 0;
fields=fields->tl){if(!(*((struct _tuple18*)fields->hd)).f2){({void*_tmp779[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments to struct",sizeof(char),28),
_tag_arr(_tmp779,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp77A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp77B;void*_tmp77C;struct Cyc_Absyn_Conref*_tmp77D;_LL518: if(_tmp77A <= (void*)3?
1:*((int*)_tmp77A)!= 4)goto _LL51A;_tmp77B=((struct Cyc_Absyn_PointerType_struct*)
_tmp77A)->f1;_tmp77C=(void*)_tmp77B.elt_typ;_tmp77D=_tmp77B.bounds;_LL519: {
struct Cyc_Absyn_Conref*_tmp77E=Cyc_Absyn_compress_conref(_tmp77D);void*_tmp77F=(
void*)(Cyc_Absyn_compress_conref(_tmp77E))->v;void*_tmp780;_LL51D: if(_tmp77F <= (
void*)1?1:*((int*)_tmp77F)!= 0)goto _LL51F;_tmp780=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp77F)->f1;if((int)_tmp780 != 0)goto _LL51F;_LL51E:*elt_typ_dest=_tmp77C;return 1;
_LL51F: if((int)_tmp77F != 0)goto _LL521;_LL520:(void*)(_tmp77E->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp781=_cycalloc(sizeof(*_tmp781));_tmp781[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp782;_tmp782.tag=0;_tmp782.f1=(void*)((void*)
0);_tmp782;});_tmp781;})));*elt_typ_dest=_tmp77C;return 1;_LL521:;_LL522: return 0;
_LL51C:;}_LL51A:;_LL51B: return 0;_LL517:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp7C3;_tmp7C3.f1=0;
_tmp7C3.f2=(void*)2;_tmp7C3;});void*_tmp783=(void*)e->r;struct _tuple1*_tmp784;
void*_tmp785;struct Cyc_Absyn_Exp*_tmp786;struct _tagged_arr*_tmp787;struct Cyc_Absyn_Exp*
_tmp788;struct _tagged_arr*_tmp789;struct Cyc_Absyn_Exp*_tmp78A;struct Cyc_Absyn_Exp*
_tmp78B;struct Cyc_Absyn_Exp*_tmp78C;_LL524: if(*((int*)_tmp783)!= 1)goto _LL526;
_tmp784=((struct Cyc_Absyn_Var_e_struct*)_tmp783)->f1;_tmp785=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp783)->f2;_LL525: {void*_tmp78D=_tmp785;struct Cyc_Absyn_Vardecl*_tmp78E;
struct Cyc_Absyn_Vardecl*_tmp78F;struct Cyc_Absyn_Vardecl*_tmp790;struct Cyc_Absyn_Vardecl*
_tmp791;_LL531: if((int)_tmp78D != 0)goto _LL533;_LL532: goto _LL534;_LL533: if(
_tmp78D <= (void*)1?1:*((int*)_tmp78D)!= 1)goto _LL535;_LL534: return bogus_ans;
_LL535: if(_tmp78D <= (void*)1?1:*((int*)_tmp78D)!= 0)goto _LL537;_tmp78E=((struct
Cyc_Absyn_Global_b_struct*)_tmp78D)->f1;_LL536: {void*_tmp792=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL53E: if(_tmp792 <= (void*)
3?1:*((int*)_tmp792)!= 7)goto _LL540;_LL53F: return({struct _tuple10 _tmp793;_tmp793.f1=
1;_tmp793.f2=(void*)2;_tmp793;});_LL540:;_LL541: return({struct _tuple10 _tmp794;
_tmp794.f1=(_tmp78E->tq).q_const;_tmp794.f2=(void*)2;_tmp794;});_LL53D:;}_LL537:
if(_tmp78D <= (void*)1?1:*((int*)_tmp78D)!= 3)goto _LL539;_tmp78F=((struct Cyc_Absyn_Local_b_struct*)
_tmp78D)->f1;_LL538: {void*_tmp795=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL543: if(_tmp795 <= (void*)3?1:*((int*)_tmp795)!= 7)
goto _LL545;_LL544: return({struct _tuple10 _tmp796;_tmp796.f1=1;_tmp796.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp78F->rgn))->v;_tmp796;});_LL545:;_LL546:
_tmp78F->escapes=1;return({struct _tuple10 _tmp797;_tmp797.f1=(_tmp78F->tq).q_const;
_tmp797.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp78F->rgn))->v;_tmp797;});
_LL542:;}_LL539: if(_tmp78D <= (void*)1?1:*((int*)_tmp78D)!= 4)goto _LL53B;_tmp790=((
struct Cyc_Absyn_Pat_b_struct*)_tmp78D)->f1;_LL53A: _tmp791=_tmp790;goto _LL53C;
_LL53B: if(_tmp78D <= (void*)1?1:*((int*)_tmp78D)!= 2)goto _LL530;_tmp791=((struct
Cyc_Absyn_Param_b_struct*)_tmp78D)->f1;_LL53C: _tmp791->escapes=1;return({struct
_tuple10 _tmp798;_tmp798.f1=(_tmp791->tq).q_const;_tmp798.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp791->rgn))->v;_tmp798;});_LL530:;}_LL526: if(*((int*)_tmp783)!= 
21)goto _LL528;_tmp786=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp783)->f1;
_tmp787=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp783)->f2;_LL527: {void*
_tmp799=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp786->topt))->v);
struct Cyc_List_List*_tmp79A;struct Cyc_Absyn_AggrInfo _tmp79B;void*_tmp79C;struct
Cyc_Absyn_Aggrdecl**_tmp79D;struct Cyc_Absyn_Aggrdecl*_tmp79E;_LL548: if(_tmp799 <= (
void*)3?1:*((int*)_tmp799)!= 11)goto _LL54A;_tmp79A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp799)->f2;_LL549: {struct Cyc_Absyn_Aggrfield*_tmp79F=Cyc_Absyn_lookup_field(
_tmp79A,_tmp787);if(_tmp79F != 0?_tmp79F->width != 0: 0)return({struct _tuple10
_tmp7A0;_tmp7A0.f1=(_tmp79F->tq).q_const;_tmp7A0.f2=(Cyc_Tcutil_addressof_props(
te,_tmp786)).f2;_tmp7A0;});return bogus_ans;}_LL54A: if(_tmp799 <= (void*)3?1:*((
int*)_tmp799)!= 10)goto _LL54C;_tmp79B=((struct Cyc_Absyn_AggrType_struct*)_tmp799)->f1;
_tmp79C=(void*)_tmp79B.aggr_info;if(*((int*)_tmp79C)!= 1)goto _LL54C;_tmp79D=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp79C)->f1;_tmp79E=*_tmp79D;_LL54B: {struct
Cyc_Absyn_Aggrfield*_tmp7A1=Cyc_Absyn_lookup_decl_field(_tmp79E,_tmp787);if(
_tmp7A1 != 0?_tmp7A1->width != 0: 0)return({struct _tuple10 _tmp7A2;_tmp7A2.f1=(
_tmp7A1->tq).q_const;_tmp7A2.f2=(Cyc_Tcutil_addressof_props(te,_tmp786)).f2;
_tmp7A2;});return bogus_ans;}_LL54C:;_LL54D: return bogus_ans;_LL547:;}_LL528: if(*((
int*)_tmp783)!= 22)goto _LL52A;_tmp788=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp783)->f1;_tmp789=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp783)->f2;_LL529: {
void*_tmp7A3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp788->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7A4;void*_tmp7A5;void*_tmp7A6;
_LL54F: if(_tmp7A3 <= (void*)3?1:*((int*)_tmp7A3)!= 4)goto _LL551;_tmp7A4=((struct
Cyc_Absyn_PointerType_struct*)_tmp7A3)->f1;_tmp7A5=(void*)_tmp7A4.elt_typ;
_tmp7A6=(void*)_tmp7A4.rgn_typ;_LL550: {struct Cyc_Absyn_Aggrfield*finfo;{void*
_tmp7A7=Cyc_Tcutil_compress(_tmp7A5);struct Cyc_List_List*_tmp7A8;struct Cyc_Absyn_AggrInfo
_tmp7A9;void*_tmp7AA;struct Cyc_Absyn_Aggrdecl**_tmp7AB;struct Cyc_Absyn_Aggrdecl*
_tmp7AC;_LL554: if(_tmp7A7 <= (void*)3?1:*((int*)_tmp7A7)!= 11)goto _LL556;_tmp7A8=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp7A7)->f2;_LL555: finfo=Cyc_Absyn_lookup_field(
_tmp7A8,_tmp789);goto _LL553;_LL556: if(_tmp7A7 <= (void*)3?1:*((int*)_tmp7A7)!= 10)
goto _LL558;_tmp7A9=((struct Cyc_Absyn_AggrType_struct*)_tmp7A7)->f1;_tmp7AA=(void*)
_tmp7A9.aggr_info;if(*((int*)_tmp7AA)!= 1)goto _LL558;_tmp7AB=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp7AA)->f1;_tmp7AC=*_tmp7AB;_LL557: finfo=Cyc_Absyn_lookup_decl_field(_tmp7AC,
_tmp789);goto _LL553;_LL558:;_LL559: return bogus_ans;_LL553:;}if(finfo != 0?finfo->width
!= 0: 0)return({struct _tuple10 _tmp7AD;_tmp7AD.f1=(finfo->tq).q_const;_tmp7AD.f2=
_tmp7A6;_tmp7AD;});return bogus_ans;}_LL551:;_LL552: return bogus_ans;_LL54E:;}
_LL52A: if(*((int*)_tmp783)!= 20)goto _LL52C;_tmp78A=((struct Cyc_Absyn_Deref_e_struct*)
_tmp783)->f1;_LL52B: {void*_tmp7AE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp78A->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7AF;void*_tmp7B0;
struct Cyc_Absyn_Tqual _tmp7B1;_LL55B: if(_tmp7AE <= (void*)3?1:*((int*)_tmp7AE)!= 4)
goto _LL55D;_tmp7AF=((struct Cyc_Absyn_PointerType_struct*)_tmp7AE)->f1;_tmp7B0=(
void*)_tmp7AF.rgn_typ;_tmp7B1=_tmp7AF.tq;_LL55C: return({struct _tuple10 _tmp7B2;
_tmp7B2.f1=_tmp7B1.q_const;_tmp7B2.f2=_tmp7B0;_tmp7B2;});_LL55D:;_LL55E: return
bogus_ans;_LL55A:;}_LL52C: if(*((int*)_tmp783)!= 23)goto _LL52E;_tmp78B=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp783)->f1;_tmp78C=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp783)->f2;_LL52D: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp78B->topt))->v);void*_tmp7B3=t;struct Cyc_List_List*_tmp7B4;
struct Cyc_Absyn_PtrInfo _tmp7B5;void*_tmp7B6;void*_tmp7B7;struct Cyc_Absyn_Tqual
_tmp7B8;struct Cyc_Absyn_Conref*_tmp7B9;struct Cyc_Absyn_Tqual _tmp7BA;_LL560: if(
_tmp7B3 <= (void*)3?1:*((int*)_tmp7B3)!= 9)goto _LL562;_tmp7B4=((struct Cyc_Absyn_TupleType_struct*)
_tmp7B3)->f1;_LL561: {unsigned int _tmp7BC;int _tmp7BD;struct _tuple7 _tmp7BB=Cyc_Evexp_eval_const_uint_exp(
_tmp78C);_tmp7BC=_tmp7BB.f1;_tmp7BD=_tmp7BB.f2;if(!_tmp7BD)return bogus_ans;{
struct _tuple4*_tmp7BE=Cyc_Absyn_lookup_tuple_field(_tmp7B4,(int)_tmp7BC);if(
_tmp7BE != 0)return({struct _tuple10 _tmp7BF;_tmp7BF.f1=((*_tmp7BE).f1).q_const;
_tmp7BF.f2=(Cyc_Tcutil_addressof_props(te,_tmp78B)).f2;_tmp7BF;});return
bogus_ans;}}_LL562: if(_tmp7B3 <= (void*)3?1:*((int*)_tmp7B3)!= 4)goto _LL564;
_tmp7B5=((struct Cyc_Absyn_PointerType_struct*)_tmp7B3)->f1;_tmp7B6=(void*)
_tmp7B5.elt_typ;_tmp7B7=(void*)_tmp7B5.rgn_typ;_tmp7B8=_tmp7B5.tq;_tmp7B9=
_tmp7B5.bounds;_LL563: return({struct _tuple10 _tmp7C0;_tmp7C0.f1=_tmp7B8.q_const;
_tmp7C0.f2=_tmp7B7;_tmp7C0;});_LL564: if(_tmp7B3 <= (void*)3?1:*((int*)_tmp7B3)!= 
7)goto _LL566;_tmp7BA=((struct Cyc_Absyn_ArrayType_struct*)_tmp7B3)->f2;_LL565:
return({struct _tuple10 _tmp7C1;_tmp7C1.f1=_tmp7BA.q_const;_tmp7C1.f2=(Cyc_Tcutil_addressof_props(
te,_tmp78B)).f2;_tmp7C1;});_LL566:;_LL567: return bogus_ans;_LL55F:;}_LL52E:;
_LL52F:({void*_tmp7C2[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("unary & applied to non-lvalue",
sizeof(char),30),_tag_arr(_tmp7C2,sizeof(void*),0));});return bogus_ans;_LL523:;}
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*
e){void*_tmp7C4=Cyc_Tcutil_compress(e_typ);void*_tmp7C5;struct Cyc_Absyn_Tqual
_tmp7C6;_LL569: if(_tmp7C4 <= (void*)3?1:*((int*)_tmp7C4)!= 7)goto _LL56B;_tmp7C5=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp7C4)->f1;_tmp7C6=((struct Cyc_Absyn_ArrayType_struct*)
_tmp7C4)->f2;_LL56A: {void*_tmp7C8;struct _tuple10 _tmp7C7=Cyc_Tcutil_addressof_props(
te,e);_tmp7C8=_tmp7C7.f2;return Cyc_Absyn_atb_typ(_tmp7C5,_tmp7C8,_tmp7C6,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp7C9=_cycalloc(sizeof(*_tmp7C9));_tmp7C9[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp7CA;_tmp7CA.tag=0;_tmp7CA.f1=e;_tmp7CA;});
_tmp7C9;}));}_LL56B:;_LL56C: return e_typ;_LL568:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{void*_tmp7CB=(void*)b->v;void*_tmp7CC;void*_tmp7CD;struct Cyc_Absyn_Exp*
_tmp7CE;_LL56E: if(_tmp7CB <= (void*)1?1:*((int*)_tmp7CB)!= 0)goto _LL570;_tmp7CC=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7CB)->f1;if((int)_tmp7CC != 0)goto
_LL570;_LL56F: return;_LL570: if(_tmp7CB <= (void*)1?1:*((int*)_tmp7CB)!= 0)goto
_LL572;_tmp7CD=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7CB)->f1;if(
_tmp7CD <= (void*)1?1:*((int*)_tmp7CD)!= 0)goto _LL572;_tmp7CE=((struct Cyc_Absyn_Upper_b_struct*)
_tmp7CD)->f1;_LL571: {unsigned int _tmp7D0;int _tmp7D1;struct _tuple7 _tmp7CF=Cyc_Evexp_eval_const_uint_exp(
_tmp7CE);_tmp7D0=_tmp7CF.f1;_tmp7D1=_tmp7CF.f2;if(_tmp7D1?_tmp7D0 <= i: 0)({struct
Cyc_Std_Int_pa_struct _tmp7D4;_tmp7D4.tag=1;_tmp7D4.f1=(int)i;{struct Cyc_Std_Int_pa_struct
_tmp7D3;_tmp7D3.tag=1;_tmp7D3.f1=(int)_tmp7D0;{void*_tmp7D2[2]={& _tmp7D3,&
_tmp7D4};Cyc_Tcutil_terr(loc,_tag_arr("dereference is out of bounds: %d <= %d",
sizeof(char),39),_tag_arr(_tmp7D2,sizeof(void*),2));}}});return;}_LL572:;_LL573:(
void*)(b->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp7D5=_cycalloc(
sizeof(*_tmp7D5));_tmp7D5[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp7D6;_tmp7D6.tag=
0;_tmp7D6.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp7D7=_cycalloc(
sizeof(*_tmp7D7));_tmp7D7[0]=({struct Cyc_Absyn_Upper_b_struct _tmp7D8;_tmp7D8.tag=
0;_tmp7D8.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmp7D8;});_tmp7D7;}));_tmp7D6;});
_tmp7D5;})));return;_LL56D:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp7D9=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp7DA;struct Cyc_Absyn_Exp*_tmp7DB;_LL575: if(_tmp7D9 <= (void*)1?1:*((int*)
_tmp7D9)!= 0)goto _LL577;_tmp7DA=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7D9)->f1;if(_tmp7DA <= (void*)1?1:*((int*)_tmp7DA)!= 0)goto _LL577;_tmp7DB=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7DA)->f1;_LL576: {unsigned int _tmp7DD;int
_tmp7DE;struct _tuple7 _tmp7DC=Cyc_Evexp_eval_const_uint_exp(_tmp7DB);_tmp7DD=
_tmp7DC.f1;_tmp7DE=_tmp7DC.f2;return _tmp7DE?_tmp7DD == 1: 0;}_LL577:;_LL578: return
0;_LL574:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp7DF=Cyc_Tcutil_compress(t);
void*_tmp7E0;struct Cyc_List_List*_tmp7E1;struct Cyc_Absyn_AggrInfo _tmp7E2;void*
_tmp7E3;void*_tmp7E4;struct Cyc_Absyn_AggrInfo _tmp7E5;void*_tmp7E6;void*_tmp7E7;
struct Cyc_Absyn_AggrInfo _tmp7E8;void*_tmp7E9;struct Cyc_Absyn_Aggrdecl**_tmp7EA;
struct Cyc_Absyn_Aggrdecl*_tmp7EB;struct Cyc_List_List*_tmp7EC;struct Cyc_List_List*
_tmp7ED;_LL57A: if((int)_tmp7DF != 0)goto _LL57C;_LL57B: goto _LL57D;_LL57C: if(
_tmp7DF <= (void*)3?1:*((int*)_tmp7DF)!= 5)goto _LL57E;_LL57D: goto _LL57F;_LL57E:
if((int)_tmp7DF != 1)goto _LL580;_LL57F: goto _LL581;_LL580: if(_tmp7DF <= (void*)3?1:*((
int*)_tmp7DF)!= 6)goto _LL582;_LL581: return 1;_LL582: if(_tmp7DF <= (void*)3?1:*((
int*)_tmp7DF)!= 12)goto _LL584;_LL583: goto _LL585;_LL584: if(_tmp7DF <= (void*)3?1:*((
int*)_tmp7DF)!= 13)goto _LL586;_LL585: return 0;_LL586: if(_tmp7DF <= (void*)3?1:*((
int*)_tmp7DF)!= 7)goto _LL588;_tmp7E0=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp7DF)->f1;_LL587: return Cyc_Tcutil_bits_only(_tmp7E0);_LL588: if(_tmp7DF <= (
void*)3?1:*((int*)_tmp7DF)!= 9)goto _LL58A;_tmp7E1=((struct Cyc_Absyn_TupleType_struct*)
_tmp7DF)->f1;_LL589: for(0;_tmp7E1 != 0;_tmp7E1=_tmp7E1->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp7E1->hd)).f2))return 0;}return 1;_LL58A: if(_tmp7DF <= (void*)3?1:*((
int*)_tmp7DF)!= 10)goto _LL58C;_tmp7E2=((struct Cyc_Absyn_AggrType_struct*)_tmp7DF)->f1;
_tmp7E3=(void*)_tmp7E2.aggr_info;if(*((int*)_tmp7E3)!= 0)goto _LL58C;_tmp7E4=(
void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp7E3)->f1;if((int)_tmp7E4 != 1)
goto _LL58C;_LL58B: return 1;_LL58C: if(_tmp7DF <= (void*)3?1:*((int*)_tmp7DF)!= 10)
goto _LL58E;_tmp7E5=((struct Cyc_Absyn_AggrType_struct*)_tmp7DF)->f1;_tmp7E6=(void*)
_tmp7E5.aggr_info;if(*((int*)_tmp7E6)!= 0)goto _LL58E;_tmp7E7=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp7E6)->f1;if((int)_tmp7E7 != 0)goto _LL58E;_LL58D: return 0;_LL58E: if(_tmp7DF <= (
void*)3?1:*((int*)_tmp7DF)!= 10)goto _LL590;_tmp7E8=((struct Cyc_Absyn_AggrType_struct*)
_tmp7DF)->f1;_tmp7E9=(void*)_tmp7E8.aggr_info;if(*((int*)_tmp7E9)!= 1)goto _LL590;
_tmp7EA=((struct Cyc_Absyn_KnownAggr_struct*)_tmp7E9)->f1;_tmp7EB=*_tmp7EA;
_tmp7EC=_tmp7E8.targs;_LL58F: if((void*)_tmp7EB->kind == (void*)1)return 1;if(
_tmp7EB->impl == 0)return 0;{struct _RegionHandle _tmp7EE=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp7EE;_push_region(rgn);{struct Cyc_List_List*_tmp7EF=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp7EB->tvs,_tmp7EC);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7EB->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp7EF,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp7F0=0;_npop_handler(0);
return _tmp7F0;}}}{int _tmp7F1=1;_npop_handler(0);return _tmp7F1;}};_pop_region(rgn);}
_LL590: if(_tmp7DF <= (void*)3?1:*((int*)_tmp7DF)!= 11)goto _LL592;_tmp7ED=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp7DF)->f2;_LL591: for(0;_tmp7ED != 0;_tmp7ED=
_tmp7ED->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp7ED->hd)->type))return 0;}return 1;_LL592:;_LL593: return 0;_LL579:;}struct
_tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp7F2=(void*)e->r;
struct _tuple1*_tmp7F3;void*_tmp7F4;struct Cyc_Absyn_Exp*_tmp7F5;struct Cyc_Absyn_Exp*
_tmp7F6;struct Cyc_Absyn_Exp*_tmp7F7;struct Cyc_Absyn_Exp*_tmp7F8;struct Cyc_Absyn_Exp*
_tmp7F9;struct Cyc_Absyn_Exp*_tmp7FA;struct Cyc_Absyn_Exp*_tmp7FB;void*_tmp7FC;
struct Cyc_Absyn_Exp*_tmp7FD;struct Cyc_Absyn_Exp*_tmp7FE;struct Cyc_Absyn_Exp*
_tmp7FF;struct Cyc_Absyn_Exp*_tmp800;struct Cyc_List_List*_tmp801;struct Cyc_List_List*
_tmp802;struct Cyc_List_List*_tmp803;void*_tmp804;struct Cyc_List_List*_tmp805;
struct Cyc_List_List*_tmp806;struct Cyc_List_List*_tmp807;_LL595: if(*((int*)
_tmp7F2)!= 0)goto _LL597;_LL596: goto _LL598;_LL597: if(*((int*)_tmp7F2)!= 16)goto
_LL599;_LL598: goto _LL59A;_LL599: if(*((int*)_tmp7F2)!= 17)goto _LL59B;_LL59A: goto
_LL59C;_LL59B: if(*((int*)_tmp7F2)!= 18)goto _LL59D;_LL59C: goto _LL59E;_LL59D: if(*((
int*)_tmp7F2)!= 19)goto _LL59F;_LL59E: goto _LL5A0;_LL59F: if(*((int*)_tmp7F2)!= 31)
goto _LL5A1;_LL5A0: goto _LL5A2;_LL5A1: if(*((int*)_tmp7F2)!= 32)goto _LL5A3;_LL5A2:
return 1;_LL5A3: if(*((int*)_tmp7F2)!= 1)goto _LL5A5;_tmp7F3=((struct Cyc_Absyn_Var_e_struct*)
_tmp7F2)->f1;_tmp7F4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp7F2)->f2;_LL5A4: {
void*_tmp808=_tmp7F4;struct Cyc_Absyn_Vardecl*_tmp809;_LL5C2: if(_tmp808 <= (void*)
1?1:*((int*)_tmp808)!= 1)goto _LL5C4;_LL5C3: return 1;_LL5C4: if(_tmp808 <= (void*)1?
1:*((int*)_tmp808)!= 0)goto _LL5C6;_tmp809=((struct Cyc_Absyn_Global_b_struct*)
_tmp808)->f1;_LL5C5: {void*_tmp80A=Cyc_Tcutil_compress((void*)_tmp809->type);
_LL5CB: if(_tmp80A <= (void*)3?1:*((int*)_tmp80A)!= 7)goto _LL5CD;_LL5CC: goto _LL5CE;
_LL5CD: if(_tmp80A <= (void*)3?1:*((int*)_tmp80A)!= 8)goto _LL5CF;_LL5CE: return 1;
_LL5CF:;_LL5D0: return var_okay;_LL5CA:;}_LL5C6: if((int)_tmp808 != 0)goto _LL5C8;
_LL5C7: return 0;_LL5C8:;_LL5C9: return var_okay;_LL5C1:;}_LL5A5: if(*((int*)_tmp7F2)
!= 6)goto _LL5A7;_tmp7F5=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7F2)->f1;
_tmp7F6=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7F2)->f2;_tmp7F7=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp7F2)->f3;_LL5A6: return(Cyc_Tcutil_cnst_exp(te,0,_tmp7F5)?Cyc_Tcutil_cnst_exp(
te,0,_tmp7F6): 0)?Cyc_Tcutil_cnst_exp(te,0,_tmp7F7): 0;_LL5A7: if(*((int*)_tmp7F2)
!= 7)goto _LL5A9;_tmp7F8=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp7F2)->f1;_tmp7F9=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp7F2)->f2;_LL5A8: return Cyc_Tcutil_cnst_exp(
te,0,_tmp7F8)?Cyc_Tcutil_cnst_exp(te,0,_tmp7F9): 0;_LL5A9: if(*((int*)_tmp7F2)!= 
11)goto _LL5AB;_tmp7FA=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp7F2)->f1;
_LL5AA: _tmp7FB=_tmp7FA;goto _LL5AC;_LL5AB: if(*((int*)_tmp7F2)!= 12)goto _LL5AD;
_tmp7FB=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp7F2)->f1;_LL5AC: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp7FB);_LL5AD: if(*((int*)_tmp7F2)!= 13)goto _LL5AF;_tmp7FC=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp7F2)->f1;_tmp7FD=((struct Cyc_Absyn_Cast_e_struct*)
_tmp7F2)->f2;_LL5AE: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp7FD);_LL5AF: if(*((
int*)_tmp7F2)!= 14)goto _LL5B1;_tmp7FE=((struct Cyc_Absyn_Address_e_struct*)
_tmp7F2)->f1;_LL5B0: return Cyc_Tcutil_cnst_exp(te,1,_tmp7FE);_LL5B1: if(*((int*)
_tmp7F2)!= 27)goto _LL5B3;_tmp7FF=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp7F2)->f2;_tmp800=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7F2)->f3;
_LL5B2: return Cyc_Tcutil_cnst_exp(te,0,_tmp7FF)?Cyc_Tcutil_cnst_exp(te,0,_tmp800):
0;_LL5B3: if(*((int*)_tmp7F2)!= 26)goto _LL5B5;_tmp801=((struct Cyc_Absyn_Array_e_struct*)
_tmp7F2)->f1;_LL5B4: _tmp802=_tmp801;goto _LL5B6;_LL5B5: if(*((int*)_tmp7F2)!= 29)
goto _LL5B7;_tmp802=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp7F2)->f2;_LL5B6:
_tmp803=_tmp802;goto _LL5B8;_LL5B7: if(*((int*)_tmp7F2)!= 28)goto _LL5B9;_tmp803=((
struct Cyc_Absyn_Struct_e_struct*)_tmp7F2)->f3;_LL5B8: for(0;_tmp803 != 0;_tmp803=
_tmp803->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp803->hd)).f2))
return 0;}return 1;_LL5B9: if(*((int*)_tmp7F2)!= 3)goto _LL5BB;_tmp804=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp7F2)->f1;_tmp805=((struct Cyc_Absyn_Primop_e_struct*)
_tmp7F2)->f2;_LL5BA: _tmp806=_tmp805;goto _LL5BC;_LL5BB: if(*((int*)_tmp7F2)!= 24)
goto _LL5BD;_tmp806=((struct Cyc_Absyn_Tuple_e_struct*)_tmp7F2)->f1;_LL5BC: _tmp807=
_tmp806;goto _LL5BE;_LL5BD: if(*((int*)_tmp7F2)!= 30)goto _LL5BF;_tmp807=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp7F2)->f1;_LL5BE: for(0;_tmp807 != 0;_tmp807=_tmp807->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp807->hd))return 0;}return 1;
_LL5BF:;_LL5C0: return 0;_LL594:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp80B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp80C;
struct Cyc_Absyn_Conref*_tmp80D;struct Cyc_Absyn_Conref*_tmp80E;void*_tmp80F;
struct Cyc_List_List*_tmp810;struct Cyc_Absyn_AggrInfo _tmp811;void*_tmp812;struct
Cyc_List_List*_tmp813;struct Cyc_List_List*_tmp814;_LL5D2: if((int)_tmp80B != 0)
goto _LL5D4;_LL5D3: goto _LL5D5;_LL5D4: if(_tmp80B <= (void*)3?1:*((int*)_tmp80B)!= 5)
goto _LL5D6;_LL5D5: goto _LL5D7;_LL5D6: if((int)_tmp80B != 1)goto _LL5D8;_LL5D7: goto
_LL5D9;_LL5D8: if(_tmp80B <= (void*)3?1:*((int*)_tmp80B)!= 6)goto _LL5DA;_LL5D9:
return 1;_LL5DA: if(_tmp80B <= (void*)3?1:*((int*)_tmp80B)!= 4)goto _LL5DC;_tmp80C=((
struct Cyc_Absyn_PointerType_struct*)_tmp80B)->f1;_tmp80D=_tmp80C.nullable;
_tmp80E=_tmp80C.bounds;_LL5DB: {void*_tmp815=(void*)(Cyc_Absyn_compress_conref(
_tmp80E))->v;void*_tmp816;void*_tmp817;_LL5EB: if(_tmp815 <= (void*)1?1:*((int*)
_tmp815)!= 0)goto _LL5ED;_tmp816=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp815)->f1;if((int)_tmp816 != 0)goto _LL5ED;_LL5EC: return 1;_LL5ED: if(_tmp815 <= (
void*)1?1:*((int*)_tmp815)!= 0)goto _LL5EF;_tmp817=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp815)->f1;if(_tmp817 <= (void*)1?1:*((int*)_tmp817)!= 0)goto _LL5EF;_LL5EE: {
void*_tmp818=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp80D))->v;int _tmp819;_LL5F2: if(_tmp818 <= (void*)1?1:*((int*)_tmp818)!= 0)goto
_LL5F4;_tmp819=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp818)->f1;_LL5F3:
return _tmp819;_LL5F4:;_LL5F5: return 0;_LL5F1:;}_LL5EF:;_LL5F0: return 0;_LL5EA:;}
_LL5DC: if(_tmp80B <= (void*)3?1:*((int*)_tmp80B)!= 7)goto _LL5DE;_tmp80F=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp80B)->f1;_LL5DD: return Cyc_Tcutil_supports_default(
_tmp80F);_LL5DE: if(_tmp80B <= (void*)3?1:*((int*)_tmp80B)!= 9)goto _LL5E0;_tmp810=((
struct Cyc_Absyn_TupleType_struct*)_tmp80B)->f1;_LL5DF: for(0;_tmp810 != 0;_tmp810=
_tmp810->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple4*)_tmp810->hd)).f2))
return 0;}return 1;_LL5E0: if(_tmp80B <= (void*)3?1:*((int*)_tmp80B)!= 10)goto _LL5E2;
_tmp811=((struct Cyc_Absyn_AggrType_struct*)_tmp80B)->f1;_tmp812=(void*)_tmp811.aggr_info;
_tmp813=_tmp811.targs;_LL5E1: {struct Cyc_Absyn_Aggrdecl*_tmp81A=Cyc_Absyn_get_known_aggrdecl(
_tmp812);if(_tmp81A->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp81A->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp81A->tvs,_tmp813,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp81A->impl))->fields);}
_LL5E2: if(_tmp80B <= (void*)3?1:*((int*)_tmp80B)!= 11)goto _LL5E4;_tmp814=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp80B)->f2;_LL5E3: return Cyc_Tcutil_fields_support_default(
0,0,_tmp814);_LL5E4: if(_tmp80B <= (void*)3?1:*((int*)_tmp80B)!= 13)goto _LL5E6;
_LL5E5: goto _LL5E7;_LL5E6: if(_tmp80B <= (void*)3?1:*((int*)_tmp80B)!= 12)goto
_LL5E8;_LL5E7: return 1;_LL5E8:;_LL5E9: return 0;_LL5D1:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct
_RegionHandle _tmp81B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp81B;
_push_region(rgn);{struct Cyc_List_List*_tmp81C=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp81C,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp81D=0;_npop_handler(0);return _tmp81D;}}};_pop_region(rgn);}return 1;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp81E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp81F;void*_tmp820;struct Cyc_Absyn_FnInfo _tmp821;struct Cyc_List_List*_tmp822;
_LL5F7: if(_tmp81E <= (void*)3?1:*((int*)_tmp81E)!= 4)goto _LL5F9;_tmp81F=((struct
Cyc_Absyn_PointerType_struct*)_tmp81E)->f1;_tmp820=(void*)_tmp81F.elt_typ;_LL5F8:
return Cyc_Tcutil_is_noreturn(_tmp820);_LL5F9: if(_tmp81E <= (void*)3?1:*((int*)
_tmp81E)!= 8)goto _LL5FB;_tmp821=((struct Cyc_Absyn_FnType_struct*)_tmp81E)->f1;
_tmp822=_tmp821.attributes;_LL5FA: for(0;_tmp822 != 0;_tmp822=_tmp822->tl){void*
_tmp823=(void*)_tmp822->hd;_LL5FE: if((int)_tmp823 != 3)goto _LL600;_LL5FF: return 1;
_LL600:;_LL601: continue;_LL5FD:;}goto _LL5F6;_LL5FB:;_LL5FC: goto _LL5F6;_LL5F6:;}
return 0;}
