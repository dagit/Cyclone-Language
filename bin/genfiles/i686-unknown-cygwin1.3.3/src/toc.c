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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
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
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);int Cyc_Std_vfprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_list(struct _tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);struct
_tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*
d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
15];int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);extern struct Cyc_Absyn_Rel_n_struct
Cyc_Absyn_rel_ns_null_value;extern void*Cyc_Absyn_rel_ns_null;struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(
void*,struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_uchar_t;extern void*Cyc_Absyn_uint_t;
extern void*Cyc_Absyn_sint_t;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_cstar_typ(
void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct
_tagged_arr*name);void*Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(
struct _tuple1*name);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
unsigned char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _tagged_arr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _tagged_arr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(
void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _tagged_arr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _tagged_arr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct
Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct _tagged_arr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_unify(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*
Cyc_Tcutil_snd_tqt(struct _tuple4*);unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
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
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();int Cyc_Toc_warn_bounds_checks=
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
_tmp7,_tmp7,_tmp7 + 7};static struct _tuple1 Cyc_Toc__throw_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((void*)&
Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static unsigned char _tmp9[7]="setjmp";static
struct _tagged_arr Cyc_Toc_setjmp_str={_tmp9,_tmp9,_tmp9 + 7};static struct _tuple1
Cyc_Toc_setjmp_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)((void*)& Cyc_Toc_setjmp_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=&
Cyc_Toc_setjmp_ev;static unsigned char _tmpB[14]="_push_handler";static struct
_tagged_arr Cyc_Toc__push_handler_str={_tmpB,_tmpB,_tmpB + 14};static struct _tuple1
Cyc_Toc__push_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((
void*)& Cyc_Toc__push_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static
unsigned char _tmpD[13]="_pop_handler";static struct _tagged_arr Cyc_Toc__pop_handler_str={
_tmpD,_tmpD,_tmpD + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
unsigned char _tmpF[12]="_exn_thrown";static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_tmpF,_tmpF,_tmpF + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={
0,(void*)((void*)& Cyc_Toc__exn_thrown_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static
unsigned char _tmp11[14]="_npop_handler";static struct _tagged_arr Cyc_Toc__npop_handler_str={
_tmp11,_tmp11,_tmp11 + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)((void*)& Cyc_Toc__npop_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev;static unsigned char _tmp13[12]="_check_null";static
struct _tagged_arr Cyc_Toc__check_null_str={_tmp13,_tmp13,_tmp13 + 12};static struct
_tuple1 Cyc_Toc__check_null_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((
void*)& Cyc_Toc__check_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static
unsigned char _tmp15[28]="_check_known_subscript_null";static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={
_tmp15,_tmp15,_tmp15 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _tmp17[31]="_check_known_subscript_notnull";static struct
_tagged_arr Cyc_Toc__check_known_subscript_notnull_str={_tmp17,_tmp17,_tmp17 + 31};
static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static unsigned char _tmp19[25]="_check_unknown_subscript";
static struct _tagged_arr Cyc_Toc__check_unknown_subscript_str={_tmp19,_tmp19,
_tmp19 + 25};static struct _tuple1 Cyc_Toc__check_unknown_subscript_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1,& Cyc_Toc__check_unknown_subscript_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_unknown_subscript_ev={0,(void*)((void*)& Cyc_Toc__check_unknown_subscript_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_unknown_subscript_e=&
Cyc_Toc__check_unknown_subscript_ev;static unsigned char _tmp1B[12]="_tagged_arr";
static struct _tagged_arr Cyc_Toc__tagged_arr_str={_tmp1B,_tmp1B,_tmp1B + 12};static
struct _tuple1 Cyc_Toc__tagged_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={1,& Cyc_Toc__tagged_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={0,(void*)((
void*)& Cyc_Toc__tagged_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;static
unsigned char _tmp1D[9]="_tag_arr";static struct _tagged_arr Cyc_Toc__tag_arr_str={
_tmp1D,_tmp1D,_tmp1D + 9};static struct _tuple1 Cyc_Toc__tag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={1,&
Cyc_Toc__tag_arr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,(void*)((void*)& Cyc_Toc__tag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_arr_e=& Cyc_Toc__tag_arr_ev;static
unsigned char _tmp1F[14]="_init_tag_arr";static struct _tagged_arr Cyc_Toc__init_tag_arr_str={
_tmp1F,_tmp1F,_tmp1F + 14};static struct _tuple1 Cyc_Toc__init_tag_arr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_tag_arr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__init_tag_arr_re={1,& Cyc_Toc__init_tag_arr_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__init_tag_arr_ev={0,(void*)((void*)& Cyc_Toc__init_tag_arr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__init_tag_arr_e=&
Cyc_Toc__init_tag_arr_ev;static unsigned char _tmp21[11]="_untag_arr";static struct
_tagged_arr Cyc_Toc__untag_arr_str={_tmp21,_tmp21,_tmp21 + 11};static struct _tuple1
Cyc_Toc__untag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_arr_re={1,& Cyc_Toc__untag_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={0,(void*)((
void*)& Cyc_Toc__untag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;static
unsigned char _tmp23[14]="_get_arr_size";static struct _tagged_arr Cyc_Toc__get_arr_size_str={
_tmp23,_tmp23,_tmp23 + 14};static struct _tuple1 Cyc_Toc__get_arr_size_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_arr_size_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_arr_size_re={1,& Cyc_Toc__get_arr_size_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={0,(void*)((void*)& Cyc_Toc__get_arr_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__get_arr_size_e=&
Cyc_Toc__get_arr_size_ev;static unsigned char _tmp25[17]="_tagged_arr_plus";static
struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={_tmp25,_tmp25,_tmp25 + 17};static
struct _tuple1 Cyc_Toc__tagged_arr_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tagged_arr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_plus_re={
1,& Cyc_Toc__tagged_arr_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={
0,(void*)((void*)& Cyc_Toc__tagged_arr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_plus_e=& Cyc_Toc__tagged_arr_plus_ev;
static unsigned char _tmp27[25]="_tagged_arr_inplace_plus";static struct _tagged_arr
Cyc_Toc__tagged_arr_inplace_plus_str={_tmp27,_tmp27,_tmp27 + 25};static struct
_tuple1 Cyc_Toc__tagged_arr_inplace_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tagged_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={
1,& Cyc_Toc__tagged_arr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_ev={0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_e=&
Cyc_Toc__tagged_arr_inplace_plus_ev;static unsigned char _tmp29[30]="_tagged_arr_inplace_plus_post";
static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={_tmp29,_tmp29,
_tmp29 + 30};static struct _tuple1 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={1,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _tmp2B[10]="_cycalloc";static struct _tagged_arr Cyc_Toc__cycalloc_str={
_tmp2B,_tmp2B,_tmp2B + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static
unsigned char _tmp2D[11]="_cyccalloc";static struct _tagged_arr Cyc_Toc__cyccalloc_str={
_tmp2D,_tmp2D,_tmp2D + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={
1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static
unsigned char _tmp2F[17]="_cycalloc_atomic";static struct _tagged_arr Cyc_Toc__cycalloc_atomic_str={
_tmp2F,_tmp2F,_tmp2F + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=&
Cyc_Toc__cycalloc_atomic_ev;static unsigned char _tmp31[18]="_cyccalloc_atomic";
static struct _tagged_arr Cyc_Toc__cyccalloc_atomic_str={_tmp31,_tmp31,_tmp31 + 18};
static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static unsigned char _tmp33[15]="_region_malloc";static struct _tagged_arr Cyc_Toc__region_malloc_str={
_tmp33,_tmp33,_tmp33 + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static unsigned char _tmp35[15]="_region_calloc";static
struct _tagged_arr Cyc_Toc__region_calloc_str={_tmp35,_tmp35,_tmp35 + 15};static
struct _tuple1 Cyc_Toc__region_calloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)((
void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static
unsigned char _tmp37[13]="_check_times";static struct _tagged_arr Cyc_Toc__check_times_str={
_tmp37,_tmp37,_tmp37 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static unsigned char _tmp39[12]="_new_region";static struct
_tagged_arr Cyc_Toc__new_region_str={_tmp39,_tmp39,_tmp39 + 12};static struct
_tuple1 Cyc_Toc__new_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)((
void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
unsigned char _tmp3B[13]="_push_region";static struct _tagged_arr Cyc_Toc__push_region_str={
_tmp3B,_tmp3B,_tmp3B + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static unsigned char _tmp3D[12]="_pop_region";static struct
_tagged_arr Cyc_Toc__pop_region_str={_tmp3D,_tmp3D,_tmp3D + 12};static struct
_tuple1 Cyc_Toc__pop_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)((
void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static struct
Cyc_Absyn_UnknownAggr_struct Cyc_Toc_tagged_arr_typ_u={0,(void*)((void*)0),& Cyc_Toc__tagged_arr_pr};
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_tagged_arr_typ_v={10,{(void*)((void*)&
Cyc_Toc_tagged_arr_typ_u),0}};static void*Cyc_Toc_tagged_arr_typ=(void*)& Cyc_Toc_tagged_arr_typ_v;
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({
struct Cyc_Absyn_Stmt**_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41[0]=Cyc_Absyn_skip_stmt(
0);_tmp41;});return*((struct Cyc_Absyn_Stmt**)_check_null(skip_stmt_opt));}struct
_tuple5{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp42=(void*)e->r;void*_tmp43;unsigned char _tmp44;
void*_tmp45;short _tmp46;void*_tmp47;int _tmp48;void*_tmp49;long long _tmp4A;void*
_tmp4B;struct Cyc_Absyn_Exp*_tmp4C;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*
_tmp4E;struct Cyc_List_List*_tmp4F;struct Cyc_List_List*_tmp50;struct Cyc_List_List*
_tmp51;_LL1: if(*((int*)_tmp42)!= 0)goto _LL3;_tmp43=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp42)->f1;if(_tmp43 <= (void*)1?1:*((int*)_tmp43)!= 0)goto _LL3;_tmp44=((struct
Cyc_Absyn_Char_c_struct*)_tmp43)->f2;_LL2: return _tmp44 == '\000';_LL3: if(*((int*)
_tmp42)!= 0)goto _LL5;_tmp45=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp42)->f1;
if(_tmp45 <= (void*)1?1:*((int*)_tmp45)!= 1)goto _LL5;_tmp46=((struct Cyc_Absyn_Short_c_struct*)
_tmp45)->f2;_LL4: return _tmp46 == 0;_LL5: if(*((int*)_tmp42)!= 0)goto _LL7;_tmp47=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp42)->f1;if(_tmp47 <= (void*)1?1:*((
int*)_tmp47)!= 2)goto _LL7;_tmp48=((struct Cyc_Absyn_Int_c_struct*)_tmp47)->f2;
_LL6: return _tmp48 == 0;_LL7: if(*((int*)_tmp42)!= 0)goto _LL9;_tmp49=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp42)->f1;if(_tmp49 <= (void*)1?1:*((int*)
_tmp49)!= 3)goto _LL9;_tmp4A=((struct Cyc_Absyn_LongLong_c_struct*)_tmp49)->f2;
_LL8: return _tmp4A == 0;_LL9: if(*((int*)_tmp42)!= 0)goto _LLB;_tmp4B=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp42)->f1;if((int)_tmp4B != 0)goto _LLB;_LLA:
return 1;_LLB: if(*((int*)_tmp42)!= 13)goto _LLD;_tmp4C=((struct Cyc_Absyn_Cast_e_struct*)
_tmp42)->f2;_LLC: return Cyc_Toc_is_zero(_tmp4C);_LLD: if(*((int*)_tmp42)!= 24)goto
_LLF;_tmp4D=((struct Cyc_Absyn_Tuple_e_struct*)_tmp42)->f1;_LLE: return((int(*)(
int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp4D);_LLF: if(*((int*)_tmp42)!= 26)goto _LL11;_tmp4E=((struct Cyc_Absyn_Array_e_struct*)
_tmp42)->f1;_LL10: _tmp4F=_tmp4E;goto _LL12;_LL11: if(*((int*)_tmp42)!= 28)goto
_LL13;_tmp4F=((struct Cyc_Absyn_Struct_e_struct*)_tmp42)->f3;_LL12: _tmp50=_tmp4F;
goto _LL14;_LL13: if(*((int*)_tmp42)!= 25)goto _LL15;_tmp50=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp42)->f2;_LL14: _tmp51=_tmp50;goto _LL16;_LL15: if(*((int*)_tmp42)!= 34)goto
_LL17;_tmp51=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp42)->f2;_LL16: for(0;
_tmp51 != 0;_tmp51=_tmp51->tl){if(!Cyc_Toc_is_zero((*((struct _tuple5*)_tmp51->hd)).f2))
return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}static int Cyc_Toc_is_nullable(void*t){
void*_tmp52=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp53;struct Cyc_Absyn_Conref*
_tmp54;_LL1A: if(_tmp52 <= (void*)3?1:*((int*)_tmp52)!= 4)goto _LL1C;_tmp53=((
struct Cyc_Absyn_PointerType_struct*)_tmp52)->f1;_tmp54=_tmp53.nullable;_LL1B:
return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp54);
_LL1C:;_LL1D:({void*_tmp55[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(_tag_arr("is_nullable",sizeof(unsigned char),12),_tag_arr(
_tmp55,sizeof(void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _tagged_arr tag){return({struct _tuple1*_tmp56=_cycalloc(
sizeof(*_tmp56));_tmp56->f1=(*x).f1;_tmp56->f2=({struct _tagged_arr*_tmp57=
_cycalloc(sizeof(*_tmp57));_tmp57[0]=(struct _tagged_arr)Cyc_Std_strconcat(*(*x).f2,
tag);_tmp57;});_tmp56;});}struct _tuple6{struct _tagged_arr*f1;struct Cyc_List_List*
f2;};static struct _tagged_arr*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){{
struct Cyc_List_List*_tmp58=Cyc_Toc_tuple_types;for(0;_tmp58 != 0;_tmp58=_tmp58->tl){
struct _tuple6 _tmp5A;struct _tagged_arr*_tmp5B;struct Cyc_List_List*_tmp5C;struct
_tuple6*_tmp59=(struct _tuple6*)_tmp58->hd;_tmp5A=*_tmp59;_tmp5B=_tmp5A.f1;_tmp5C=
_tmp5A.f2;{struct Cyc_List_List*_tmp5D=tqs0;for(0;_tmp5D != 0?_tmp5C != 0: 0;(_tmp5D=
_tmp5D->tl,_tmp5C=_tmp5C->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)_tmp5D->hd)).f2,(
void*)_tmp5C->hd))break;}if(_tmp5D == 0?_tmp5C == 0: 0)return _tmp5B;}}}{struct
_tagged_arr*x=({struct _tagged_arr*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=(
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp6C;_tmp6C.tag=1;_tmp6C.f1=(
int)((unsigned int)Cyc_Toc_tuple_type_counter ++);{void*_tmp6B[1]={& _tmp6C};Cyc_Std_aprintf(
_tag_arr("_tuple%d",sizeof(unsigned char),9),_tag_arr(_tmp6B,sizeof(void*),1));}});
_tmp6A;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct
Cyc_List_List*_tmp5E=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,
i ++)){_tmp5E=({struct Cyc_List_List*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->hd=({
struct Cyc_Absyn_Aggrfield*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->name=Cyc_Absyn_fieldname(
i);_tmp60->tq=Cyc_Toc_mt_tq;_tmp60->type=(void*)((void*)ts2->hd);_tmp60->width=0;
_tmp60->attributes=0;_tmp60;});_tmp5F->tl=_tmp5E;_tmp5F;});}}_tmp5E=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5E);{struct Cyc_Absyn_Aggrdecl*
_tmp61=({struct Cyc_Absyn_Aggrdecl*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->kind=(
void*)((void*)0);_tmp67->sc=(void*)((void*)2);_tmp67->name=({struct _tuple1*
_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->f1=Cyc_Absyn_rel_ns_null;_tmp69->f2=x;
_tmp69;});_tmp67->tvs=0;_tmp67->exist_vars=0;_tmp67->fields=({struct Cyc_Core_Opt*
_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->v=_tmp5E;_tmp68;});_tmp67->attributes=
0;_tmp67;});Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*
_tmp62));_tmp62->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp64;_tmp64.tag=4;_tmp64.f1=_tmp61;_tmp64;});_tmp63;}),0);_tmp62->tl=Cyc_Toc_result_decls;
_tmp62;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmp65=_cycalloc(sizeof(*
_tmp65));_tmp65->hd=({struct _tuple6*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=
x;_tmp66->f2=ts;_tmp66;});_tmp65->tl=Cyc_Toc_tuple_types;_tmp65;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmp6D=_cycalloc(sizeof(*
_tmp6D));_tmp6D->f1=(void*)0;_tmp6D->f2=({struct _tagged_arr*_tmp6E=_cycalloc(
sizeof(*_tmp6E));_tmp6E[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp70;_tmp70.tag=1;_tmp70.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*
_tmp6F[1]={& _tmp70};Cyc_Std_aprintf(_tag_arr("_tmp%X",sizeof(unsigned char),7),
_tag_arr(_tmp6F,sizeof(void*),1));}});_tmp6E;});_tmp6D;});}static struct
_tagged_arr*Cyc_Toc_fresh_label(){return({struct _tagged_arr*_tmp71=_cycalloc(
sizeof(*_tmp71));_tmp71[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp73;_tmp73.tag=1;_tmp73.f1=(unsigned int)Cyc_Toc_fresh_label_counter ++;{void*
_tmp72[1]={& _tmp73};Cyc_Std_aprintf(_tag_arr("_LL%X",sizeof(unsigned char),6),
_tag_arr(_tmp72,sizeof(void*),1));}});_tmp71;});}static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(
struct Cyc_Absyn_Tuniondecl*td,struct _tuple1*name,int carries_value){int ans=0;
struct Cyc_List_List*_tmp74=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*)_check_null(_tmp74))->hd)->name)!= 0){if((carries_value?((
struct Cyc_Absyn_Tunionfield*)_tmp74->hd)->typs != 0: 0)?1:(!carries_value?((struct
Cyc_Absyn_Tunionfield*)_tmp74->hd)->typs == 0: 0))++ ans;_tmp74=_tmp74->tl;}return
Cyc_Absyn_uint_exp((unsigned int)ans,0);}static int Cyc_Toc_num_void_tags(struct
Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*_tmp75=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;_tmp75 != 0;_tmp75=_tmp75->tl){
if(((struct Cyc_Absyn_Tunionfield*)_tmp75->hd)->typs == 0)++ ans;}}return ans;}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*Cyc_Toc_arg_to_c(struct
_tuple2*a){struct Cyc_Core_Opt*_tmp77;struct Cyc_Absyn_Tqual _tmp78;void*_tmp79;
struct _tuple2 _tmp76=*a;_tmp77=_tmp76.f1;_tmp78=_tmp76.f2;_tmp79=_tmp76.f3;return({
struct _tuple2*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=_tmp77;_tmp7A->f2=
_tmp78;_tmp7A->f3=Cyc_Toc_typ_to_c(_tmp79);_tmp7A;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(
struct _tuple4*x){struct Cyc_Absyn_Tqual _tmp7C;void*_tmp7D;struct _tuple4 _tmp7B=*x;
_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;return({struct _tuple4*_tmp7E=_cycalloc(sizeof(*
_tmp7E));_tmp7E->f1=_tmp7C;_tmp7E->f2=Cyc_Toc_typ_to_c(_tmp7D);_tmp7E;});}static
void*Cyc_Toc_typ_to_c_array(void*t){void*_tmp7F=t;void*_tmp80;struct Cyc_Absyn_Tqual
_tmp81;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_Core_Opt*_tmp83;struct Cyc_Core_Opt
_tmp84;void*_tmp85;_LL1F: if(_tmp7F <= (void*)3?1:*((int*)_tmp7F)!= 7)goto _LL21;
_tmp80=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp7F)->f1;_tmp81=((struct Cyc_Absyn_ArrayType_struct*)
_tmp7F)->f2;_tmp82=((struct Cyc_Absyn_ArrayType_struct*)_tmp7F)->f3;_LL20: return(
void*)({struct Cyc_Absyn_ArrayType_struct*_tmp86=_cycalloc(sizeof(*_tmp86));
_tmp86[0]=({struct Cyc_Absyn_ArrayType_struct _tmp87;_tmp87.tag=7;_tmp87.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp80);_tmp87.f2=_tmp81;_tmp87.f3=_tmp82;_tmp87;});
_tmp86;});_LL21: if(_tmp7F <= (void*)3?1:*((int*)_tmp7F)!= 0)goto _LL23;_tmp83=((
struct Cyc_Absyn_Evar_struct*)_tmp7F)->f2;if(_tmp83 == 0)goto _LL23;_tmp84=*_tmp83;
_tmp85=(void*)_tmp84.v;_LL22: return Cyc_Toc_typ_to_c_array(_tmp85);_LL23:;_LL24:
return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(
struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*_tmp88=_cycalloc(
sizeof(*_tmp88));_tmp88->name=f->name;_tmp88->tq=f->tq;_tmp88->type=(void*)Cyc_Toc_typ_to_c((
void*)f->type);_tmp88->width=f->width;_tmp88->attributes=f->attributes;_tmp88;});}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){struct Cyc_List_List*
_tmp89=fs;for(0;_tmp89 != 0;_tmp89=_tmp89->tl){if(((struct Cyc_Absyn_Enumfield*)
_tmp89->hd)->tag != 0){unsigned int i=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(((struct Cyc_Absyn_Enumfield*)_tmp89->hd)->tag));((struct Cyc_Absyn_Enumfield*)
_tmp89->hd)->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(i,0);}}}static void*Cyc_Toc_typ_to_c(
void*t){void*_tmp8A=t;struct Cyc_Core_Opt*_tmp8B;struct Cyc_Core_Opt*_tmp8C;struct
Cyc_Core_Opt _tmp8D;void*_tmp8E;struct Cyc_Absyn_Tvar*_tmp8F;struct Cyc_Absyn_TunionInfo
_tmp90;void*_tmp91;struct Cyc_Absyn_TunionFieldInfo _tmp92;void*_tmp93;struct Cyc_Absyn_Tuniondecl*
_tmp94;struct Cyc_Absyn_Tunionfield*_tmp95;struct Cyc_Absyn_PtrInfo _tmp96;void*
_tmp97;struct Cyc_Absyn_Tqual _tmp98;struct Cyc_Absyn_Conref*_tmp99;void*_tmp9A;
struct Cyc_Absyn_Tqual _tmp9B;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_FnInfo
_tmp9D;void*_tmp9E;struct Cyc_List_List*_tmp9F;int _tmpA0;struct Cyc_Absyn_VarargInfo*
_tmpA1;struct Cyc_List_List*_tmpA2;struct Cyc_List_List*_tmpA3;void*_tmpA4;struct
Cyc_List_List*_tmpA5;struct Cyc_Absyn_AggrInfo _tmpA6;void*_tmpA7;struct Cyc_List_List*
_tmpA8;struct _tuple1*_tmpA9;struct Cyc_List_List*_tmpAA;struct _tuple1*_tmpAB;
struct Cyc_List_List*_tmpAC;struct Cyc_Core_Opt*_tmpAD;void*_tmpAE;_LL26: if((int)
_tmp8A != 0)goto _LL28;_LL27: return t;_LL28: if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)
!= 0)goto _LL2A;_tmp8B=((struct Cyc_Absyn_Evar_struct*)_tmp8A)->f2;if(_tmp8B != 0)
goto _LL2A;_LL29: return Cyc_Absyn_sint_t;_LL2A: if(_tmp8A <= (void*)3?1:*((int*)
_tmp8A)!= 0)goto _LL2C;_tmp8C=((struct Cyc_Absyn_Evar_struct*)_tmp8A)->f2;if(
_tmp8C == 0)goto _LL2C;_tmp8D=*_tmp8C;_tmp8E=(void*)_tmp8D.v;_LL2B: return Cyc_Toc_typ_to_c(
_tmp8E);_LL2C: if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 1)goto _LL2E;_tmp8F=((
struct Cyc_Absyn_VarType_struct*)_tmp8A)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(_tmp8F)
== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL2E: if(_tmp8A
<= (void*)3?1:*((int*)_tmp8A)!= 2)goto _LL30;_tmp90=((struct Cyc_Absyn_TunionType_struct*)
_tmp8A)->f1;_tmp91=(void*)_tmp90.tunion_info;if(*((int*)_tmp91)!= 1)goto _LL30;
_LL2F: return Cyc_Absyn_void_star_typ();_LL30: if(_tmp8A <= (void*)3?1:*((int*)
_tmp8A)!= 2)goto _LL32;_LL31:({void*_tmpAF[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unresolved TunionType",sizeof(
unsigned char),22),_tag_arr(_tmpAF,sizeof(void*),0));});_LL32: if(_tmp8A <= (void*)
3?1:*((int*)_tmp8A)!= 3)goto _LL34;_tmp92=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp8A)->f1;_tmp93=(void*)_tmp92.field_info;if(*((int*)_tmp93)!= 1)goto _LL34;
_tmp94=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp93)->f1;_tmp95=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp93)->f2;_LL33: if(_tmp95->typs == 0){if(_tmp94->is_xtunion)return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,(void*)2,Cyc_Toc_mt_tq);else{return Cyc_Absyn_uint_t;}}else{
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp95->name,_tag_arr("_struct",
sizeof(unsigned char),8)));}_LL34: if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 3)
goto _LL36;_LL35:({void*_tmpB0[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unresolved TunionFieldType",sizeof(
unsigned char),27),_tag_arr(_tmpB0,sizeof(void*),0));});_LL36: if(_tmp8A <= (void*)
3?1:*((int*)_tmp8A)!= 4)goto _LL38;_tmp96=((struct Cyc_Absyn_PointerType_struct*)
_tmp8A)->f1;_tmp97=(void*)_tmp96.elt_typ;_tmp98=_tmp96.tq;_tmp99=_tmp96.bounds;
_LL37: _tmp97=Cyc_Toc_typ_to_c_array(_tmp97);{void*_tmpB1=(void*)(Cyc_Absyn_compress_conref(
_tmp99))->v;void*_tmpB2;_LL5B: if(_tmpB1 <= (void*)1?1:*((int*)_tmpB1)!= 0)goto
_LL5D;_tmpB2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmpB1)->f1;if((int)
_tmpB2 != 0)goto _LL5D;_LL5C: return Cyc_Toc_tagged_arr_typ;_LL5D:;_LL5E: return Cyc_Absyn_star_typ(
_tmp97,(void*)2,_tmp98);_LL5A:;}_LL38: if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 5)
goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)_tmp8A != 1)goto _LL3C;_LL3B: goto _LL3D;
_LL3C: if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 6)goto _LL3E;_LL3D: return t;_LL3E:
if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 7)goto _LL40;_tmp9A=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp8A)->f1;_tmp9B=((struct Cyc_Absyn_ArrayType_struct*)_tmp8A)->f2;_tmp9C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp8A)->f3;_LL3F: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpB4;_tmpB4.tag=7;_tmpB4.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp9A);_tmpB4.f2=
_tmp9B;_tmpB4.f3=_tmp9C;_tmpB4;});_tmpB3;});_LL40: if(_tmp8A <= (void*)3?1:*((int*)
_tmp8A)!= 8)goto _LL42;_tmp9D=((struct Cyc_Absyn_FnType_struct*)_tmp8A)->f1;_tmp9E=(
void*)_tmp9D.ret_typ;_tmp9F=_tmp9D.args;_tmpA0=_tmp9D.c_varargs;_tmpA1=_tmp9D.cyc_varargs;
_tmpA2=_tmp9D.attributes;_LL41: {struct Cyc_List_List*_tmpB5=0;for(0;_tmpA2 != 0;
_tmpA2=_tmpA2->tl){void*_tmpB6=(void*)_tmpA2->hd;_LL60: if((int)_tmpB6 != 3)goto
_LL62;_LL61: goto _LL63;_LL62: if((int)_tmpB6 != 4)goto _LL64;_LL63: goto _LL65;_LL64:
if(_tmpB6 <= (void*)16?1:*((int*)_tmpB6)!= 3)goto _LL66;_LL65: continue;_LL66:;
_LL67: _tmpB5=({struct Cyc_List_List*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->hd=(
void*)((void*)_tmpA2->hd);_tmpB7->tl=_tmpB5;_tmpB7;});goto _LL5F;_LL5F:;}{struct
Cyc_List_List*_tmpB8=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp9F);if(_tmpA1 != 0){
void*_tmpB9=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ((void*)_tmpA1->type,(void*)2,
Cyc_Toc_mt_tq));struct _tuple2*_tmpBA=({struct _tuple2*_tmpBC=_cycalloc(sizeof(*
_tmpBC));_tmpBC->f1=_tmpA1->name;_tmpBC->f2=_tmpA1->tq;_tmpBC->f3=_tmpB9;_tmpBC;});
_tmpB8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmpB8,({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*
_tmpBB));_tmpBB->hd=_tmpBA;_tmpBB->tl=0;_tmpBB;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_FnType_struct
_tmpBE;_tmpBE.tag=8;_tmpBE.f1=({struct Cyc_Absyn_FnInfo _tmpBF;_tmpBF.tvars=0;
_tmpBF.effect=0;_tmpBF.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp9E);_tmpBF.args=
_tmpB8;_tmpBF.c_varargs=_tmpA0;_tmpBF.cyc_varargs=0;_tmpBF.rgn_po=0;_tmpBF.attributes=
_tmpB5;_tmpBF;});_tmpBE;});_tmpBD;});}}_LL42: if(_tmp8A <= (void*)3?1:*((int*)
_tmp8A)!= 9)goto _LL44;_tmpA3=((struct Cyc_Absyn_TupleType_struct*)_tmp8A)->f1;
_LL43: _tmpA3=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmpA3);{struct _tagged_arr*
_tmpC0=Cyc_Toc_add_tuple_type(_tmpA3);return Cyc_Absyn_strct(_tmpC0);}_LL44: if(
_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 11)goto _LL46;_tmpA4=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp8A)->f1;_tmpA5=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp8A)->f2;_LL45:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpC1=_cycalloc(sizeof(*
_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpC2;_tmpC2.tag=11;
_tmpC2.f1=(void*)_tmpA4;_tmpC2.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmpA5);_tmpC2;});_tmpC1;});_LL46: if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 10)
goto _LL48;_tmpA6=((struct Cyc_Absyn_AggrType_struct*)_tmp8A)->f1;_tmpA7=(void*)
_tmpA6.aggr_info;_tmpA8=_tmpA6.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*_tmpC3=Cyc_Absyn_get_known_aggrdecl(
_tmpA7);if((void*)_tmpC3->kind == (void*)1)return Cyc_Absyn_unionq_typ(_tmpC3->name);
return Cyc_Absyn_strctq(_tmpC3->name);}_LL48: if(_tmp8A <= (void*)3?1:*((int*)
_tmp8A)!= 12)goto _LL4A;_tmpA9=((struct Cyc_Absyn_EnumType_struct*)_tmp8A)->f1;
_LL49: return t;_LL4A: if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 13)goto _LL4C;_tmpAA=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp8A)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmpAA);return t;_LL4C: if(_tmp8A <= (void*)3?1:*((int*)_tmp8A)!= 16)goto _LL4E;
_tmpAB=((struct Cyc_Absyn_TypedefType_struct*)_tmp8A)->f1;_tmpAC=((struct Cyc_Absyn_TypedefType_struct*)
_tmp8A)->f2;_tmpAD=((struct Cyc_Absyn_TypedefType_struct*)_tmp8A)->f3;_LL4D: if(
_tmpAD == 0)({void*_tmpC4[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: unresolved TypedefType",sizeof(
unsigned char),38),_tag_arr(_tmpC4,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpC6;_tmpC6.tag=16;_tmpC6.f1=_tmpAB;_tmpC6.f2=0;_tmpC6.f3=({struct Cyc_Core_Opt*
_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->v=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmpAD->v);_tmpC7;});_tmpC6;});_tmpC5;});_LL4E: if(_tmp8A <= (void*)3?1:*((int*)
_tmp8A)!= 14)goto _LL50;_LL4F: return Cyc_Absyn_uint_t;_LL50: if(_tmp8A <= (void*)3?1:*((
int*)_tmp8A)!= 15)goto _LL52;_tmpAE=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp8A)->f1;_LL51: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL52: if((int)_tmp8A != 2)goto _LL54;_LL53:({void*_tmpC8[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof(unsigned char),55),_tag_arr(_tmpC8,sizeof(void*),0));});_LL54: if(_tmp8A <= (
void*)3?1:*((int*)_tmp8A)!= 17)goto _LL56;_LL55:({void*_tmpC9[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof(unsigned char),54),_tag_arr(_tmpC9,sizeof(void*),0));});_LL56: if(_tmp8A <= (
void*)3?1:*((int*)_tmp8A)!= 18)goto _LL58;_LL57:({void*_tmpCA[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof(unsigned char),52),_tag_arr(_tmpCA,sizeof(void*),0));});_LL58: if(_tmp8A <= (
void*)3?1:*((int*)_tmp8A)!= 19)goto _LL25;_LL59:({void*_tmpCB[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof(unsigned char),55),_tag_arr(_tmpCB,sizeof(void*),0));});_LL25:;}static
struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*l){void*_tmpCC=t;void*_tmpCD;struct Cyc_Absyn_Tqual
_tmpCE;_LL69: if(_tmpCC <= (void*)3?1:*((int*)_tmpCC)!= 7)goto _LL6B;_tmpCD=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmpCC)->f1;_tmpCE=((struct Cyc_Absyn_ArrayType_struct*)
_tmpCC)->f2;_LL6A: return Cyc_Absyn_cast_exp(Cyc_Absyn_star_typ(_tmpCD,(void*)2,
_tmpCE),e,l);_LL6B:;_LL6C: return Cyc_Absyn_cast_exp(t,e,l);_LL68:;}static int Cyc_Toc_atomic_typ(
void*t){void*_tmpCF=Cyc_Tcutil_compress(t);void*_tmpD0;struct Cyc_Absyn_AggrInfo
_tmpD1;void*_tmpD2;struct Cyc_List_List*_tmpD3;struct Cyc_Absyn_TunionFieldInfo
_tmpD4;void*_tmpD5;struct Cyc_Absyn_Tuniondecl*_tmpD6;struct Cyc_Absyn_Tunionfield*
_tmpD7;struct Cyc_List_List*_tmpD8;_LL6E: if((int)_tmpCF != 0)goto _LL70;_LL6F:
return 1;_LL70: if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)!= 1)goto _LL72;_LL71: return 0;
_LL72: if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)!= 5)goto _LL74;_LL73: goto _LL75;_LL74:
if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)!= 12)goto _LL76;_LL75: goto _LL77;_LL76: if(
_tmpCF <= (void*)3?1:*((int*)_tmpCF)!= 13)goto _LL78;_LL77: goto _LL79;_LL78: if((int)
_tmpCF != 1)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)
!= 6)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)!= 8)
goto _LL7E;_LL7D: goto _LL7F;_LL7E: if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)!= 14)goto
_LL80;_LL7F: return 1;_LL80: if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)!= 7)goto _LL82;
_tmpD0=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpCF)->f1;_LL81: return Cyc_Toc_atomic_typ(
_tmpD0);_LL82: if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)!= 10)goto _LL84;_tmpD1=((
struct Cyc_Absyn_AggrType_struct*)_tmpCF)->f1;_tmpD2=(void*)_tmpD1.aggr_info;
_LL83:{void*_tmpD9=_tmpD2;_LL93: if(*((int*)_tmpD9)!= 0)goto _LL95;_LL94: return 0;
_LL95:;_LL96: goto _LL92;_LL92:;}{struct Cyc_Absyn_Aggrdecl*_tmpDA=Cyc_Absyn_get_known_aggrdecl(
_tmpD2);if(_tmpDA->fields == 0)return 0;{struct Cyc_List_List*_tmpDB=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpDA->fields))->v;for(0;_tmpDB != 0;_tmpDB=
_tmpDB->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmpDB->hd)->type))
return 0;}}return 1;}_LL84: if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)!= 11)goto _LL86;
_tmpD3=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpCF)->f2;_LL85: for(0;_tmpD3 != 0;
_tmpD3=_tmpD3->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmpD3->hd)->type))return 0;}return 1;_LL86: if(_tmpCF <= (void*)3?1:*((int*)_tmpCF)
!= 3)goto _LL88;_tmpD4=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpCF)->f1;
_tmpD5=(void*)_tmpD4.field_info;if(*((int*)_tmpD5)!= 1)goto _LL88;_tmpD6=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmpD5)->f1;_tmpD7=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpD5)->f2;_LL87: _tmpD8=_tmpD7->typs;goto _LL89;_LL88: if(_tmpCF <= (void*)3?1:*((
int*)_tmpCF)!= 9)goto _LL8A;_tmpD8=((struct Cyc_Absyn_TupleType_struct*)_tmpCF)->f1;
_LL89: for(0;_tmpD8 != 0;_tmpD8=_tmpD8->tl){if(!Cyc_Toc_atomic_typ((*((struct
_tuple4*)_tmpD8->hd)).f2))return 0;}return 1;_LL8A: if(_tmpCF <= (void*)3?1:*((int*)
_tmpCF)!= 2)goto _LL8C;_LL8B: goto _LL8D;_LL8C: if(_tmpCF <= (void*)3?1:*((int*)
_tmpCF)!= 4)goto _LL8E;_LL8D: goto _LL8F;_LL8E: if(_tmpCF <= (void*)3?1:*((int*)
_tmpCF)!= 15)goto _LL90;_LL8F: return 0;_LL90:;_LL91:({struct Cyc_Std_String_pa_struct
_tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpDC[1]={& _tmpDD};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("atomic_typ:  bad type %s",sizeof(unsigned char),25),_tag_arr(_tmpDC,
sizeof(void*),1));}});_LL6D:;}static int Cyc_Toc_is_void_star(void*t){void*_tmpDE=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpDF;void*_tmpE0;_LL98: if(_tmpDE
<= (void*)3?1:*((int*)_tmpDE)!= 4)goto _LL9A;_tmpDF=((struct Cyc_Absyn_PointerType_struct*)
_tmpDE)->f1;_tmpE0=(void*)_tmpDF.elt_typ;_LL99: {void*_tmpE1=Cyc_Tcutil_compress(
_tmpE0);_LL9D: if((int)_tmpE1 != 0)goto _LL9F;_LL9E: return 1;_LL9F:;_LLA0: return 0;
_LL9C:;}_LL9A:;_LL9B: return 0;_LL97:;}static int Cyc_Toc_is_poly_field(void*t,
struct _tagged_arr*f){void*_tmpE2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpE3;void*_tmpE4;struct Cyc_List_List*_tmpE5;_LLA2: if(_tmpE2 <= (void*)3?1:*((
int*)_tmpE2)!= 10)goto _LLA4;_tmpE3=((struct Cyc_Absyn_AggrType_struct*)_tmpE2)->f1;
_tmpE4=(void*)_tmpE3.aggr_info;_LLA3: {struct Cyc_Absyn_Aggrdecl*_tmpE6=Cyc_Absyn_get_known_aggrdecl(
_tmpE4);if(_tmpE6->fields == 0)({void*_tmpE7[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: type missing fields",
sizeof(unsigned char),35),_tag_arr(_tmpE7,sizeof(void*),0));});_tmpE5=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpE6->fields))->v;goto _LLA5;}_LLA4: if(_tmpE2 <= (
void*)3?1:*((int*)_tmpE2)!= 11)goto _LLA6;_tmpE5=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE2)->f2;_LLA5: {struct Cyc_Absyn_Aggrfield*_tmpE8=Cyc_Absyn_lookup_field(
_tmpE5,f);if(_tmpE8 == 0)({struct Cyc_Std_String_pa_struct _tmpEA;_tmpEA.tag=0;
_tmpEA.f1=(struct _tagged_arr)*f;{void*_tmpE9[1]={& _tmpEA};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: bad field %s",
sizeof(unsigned char),28),_tag_arr(_tmpE9,sizeof(void*),1));}});return Cyc_Toc_is_void_star((
void*)_tmpE8->type);}_LLA6:;_LLA7:({struct Cyc_Std_String_pa_struct _tmpEC;_tmpEC.tag=
0;_tmpEC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpEB[1]={&
_tmpEC};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: bad type %s",sizeof(unsigned char),27),_tag_arr(_tmpEB,
sizeof(void*),1));}});_LLA1:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*
e){void*_tmpED=(void*)e->r;struct Cyc_Absyn_Exp*_tmpEE;struct _tagged_arr*_tmpEF;
struct Cyc_Absyn_Exp*_tmpF0;struct _tagged_arr*_tmpF1;_LLA9: if(*((int*)_tmpED)!= 
21)goto _LLAB;_tmpEE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpED)->f1;_tmpEF=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpED)->f2;_LLAA: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEE->topt))->v,_tmpEF);_LLAB: if(*((
int*)_tmpED)!= 22)goto _LLAD;_tmpF0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpED)->f1;
_tmpF1=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpED)->f2;_LLAC: {void*_tmpF2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF0->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF3;void*_tmpF4;_LLB0: if(_tmpF2 <= (void*)3?1:*((int*)_tmpF2)!= 4)goto _LLB2;
_tmpF3=((struct Cyc_Absyn_PointerType_struct*)_tmpF2)->f1;_tmpF4=(void*)_tmpF3.elt_typ;
_LLB1: return Cyc_Toc_is_poly_field(_tmpF4,_tmpF1);_LLB2:;_LLB3:({struct Cyc_Std_String_pa_struct
_tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF0->topt))->v);{void*_tmpF5[1]={& _tmpF6};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_project:  bad type %s",
sizeof(unsigned char),30),_tag_arr(_tmpF5,sizeof(void*),1));}});_LLAF:;}_LLAD:;
_LLAE: return 0;_LLA8:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmpF7=
_cycalloc(sizeof(*_tmpF7));_tmpF7->hd=s;_tmpF7->tl=0;_tmpF7;}),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmpF8=_cycalloc(sizeof(*_tmpF8));
_tmpF8->hd=s;_tmpF8->tl=0;_tmpF8;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({
struct Cyc_Absyn_Exp*_tmpF9[2];_tmpF9[1]=s;_tmpF9[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpF9,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmpFA[2];_tmpFA[1]=n;_tmpFA[0]=
s;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFA,
sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({
struct Cyc_Absyn_Exp*_tmpFB[2];_tmpFB[1]=n;_tmpFB[0]=s;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFB,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,
struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({
struct Cyc_Absyn_Exp*_tmpFC[3];_tmpFC[2]=n;_tmpFC[1]=s;_tmpFC[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFC,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return
Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmpFD=_cycalloc(
sizeof(*_tmpFD));_tmpFD->hd=e;_tmpFD->tl=0;_tmpFD;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*
e){int is_string=0;{void*_tmpFE=(void*)e->r;void*_tmpFF;_LLB5: if(*((int*)_tmpFE)
!= 0)goto _LLB7;_tmpFF=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpFE)->f1;if(
_tmpFF <= (void*)1?1:*((int*)_tmpFF)!= 5)goto _LLB7;_LLB6: is_string=1;goto _LLB4;
_LLB7:;_LLB8: goto _LLB4;_LLB4:;}{struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*
xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();void*vd_typ=(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp104;_tmp104.tag=7;_tmp104.f1=(void*)Cyc_Absyn_uchar_t;
_tmp104.f2=Cyc_Toc_mt_tq;_tmp104.f3=(struct Cyc_Absyn_Exp*)sz;_tmp104;});_tmp103;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,(struct Cyc_Absyn_Exp*)
e);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp100=_cycalloc(sizeof(*_tmp100));
_tmp100->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp101=
_cycalloc(sizeof(*_tmp101));_tmp101[0]=({struct Cyc_Absyn_Var_d_struct _tmp102;
_tmp102.tag=0;_tmp102.f1=vd;_tmp102;});_tmp101;}),0);_tmp100->tl=Cyc_Toc_result_decls;
_tmp100;});xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}
else{xexp=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),e,0);xplussz=Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0),0);}{struct Cyc_Absyn_Exp*
urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple5*_tmp105[3];_tmp105[2]=({
struct _tuple5*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->f1=0;_tmp108->f2=
xplussz;_tmp108;});_tmp105[1]=({struct _tuple5*_tmp107=_cycalloc(sizeof(*_tmp107));
_tmp107->f1=0;_tmp107->f2=xexp;_tmp107;});_tmp105[0]=({struct _tuple5*_tmp106=
_cycalloc(sizeof(*_tmp106));_tmp106->f1=0;_tmp106->f2=xexp;_tmp106;});((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp105,sizeof(
struct _tuple5*),3));}),0);return urm_exp;}}}struct Cyc_Toc_Env{struct Cyc_Core_Opt*
break_lab;struct Cyc_Core_Opt*continue_lab;struct Cyc_Core_Opt*fallthru_info;
struct Cyc_Dict_Dict*varmap;int toplevel;};static struct Cyc_Toc_Env*Cyc_Toc_empty_env(){
return({struct Cyc_Toc_Env*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->break_lab=
0;_tmp109->continue_lab=0;_tmp109->fallthru_info=0;_tmp109->varmap=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp109->toplevel=1;_tmp109;});}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp10A=_cycalloc(
sizeof(*_tmp10A));_tmp10A[0]=*e;_tmp10A;});ans->toplevel=0;return ans;}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct Cyc_Toc_Env*e,struct _tuple1*x,struct
Cyc_Absyn_Exp*y){{void*_tmp10B=(*x).f1;_LLBA: if(_tmp10B <= (void*)1?1:*((int*)
_tmp10B)!= 0)goto _LLBC;_LLBB:({struct Cyc_Std_String_pa_struct _tmp10D;_tmp10D.tag=
0;_tmp10D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(x);{void*_tmp10C[1]={&
_tmp10D};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::add_varmap on Rel_n: %s\n",sizeof(unsigned char),30),_tag_arr(
_tmp10C,sizeof(void*),1));}});_LLBC:;_LLBD: goto _LLB9;_LLB9:;}{struct Cyc_Toc_Env*
ans=({struct Cyc_Toc_Env*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=*e;
_tmp10E;});ans->varmap=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(ans->varmap,x,y);return ans;}}
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*
ans=({struct Cyc_Toc_Env*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F[0]=*e;
_tmp10F;});ans->break_lab=0;ans->continue_lab=0;return ans;}struct _tuple7{struct
_tagged_arr*f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*f3;};static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env(struct Cyc_Toc_Env*e,struct _tagged_arr*break_l,
struct _tagged_arr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*_tmp110=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){_tmp110=({struct Cyc_List_List*_tmp111=
_cycalloc(sizeof(*_tmp111));_tmp111->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp111->tl=_tmp110;_tmp111;});}_tmp110=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp110);{struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115[0]=*e;_tmp115;});
ans->break_lab=({struct Cyc_Core_Opt*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->v=
break_l;_tmp112;});ans->fallthru_info=({struct Cyc_Core_Opt*_tmp113=_cycalloc(
sizeof(*_tmp113));_tmp113->v=({struct _tuple7*_tmp114=_cycalloc(sizeof(*_tmp114));
_tmp114->f1=fallthru_l;_tmp114->f2=_tmp110;_tmp114->f3=next_case_env->varmap;
_tmp114;});_tmp113;});return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*break_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117[0]=*e;_tmp117;});ans->break_lab=({
struct Cyc_Core_Opt*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->v=break_l;
_tmp116;});ans->fallthru_info=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*next_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*
_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=*e;_tmp11A;});ans->break_lab=0;
ans->fallthru_info=({struct Cyc_Core_Opt*_tmp118=_cycalloc(sizeof(*_tmp118));
_tmp118->v=({struct _tuple7*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->f1=
next_l;_tmp119->f2=0;_tmp119->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp119;});_tmp118;});
return ans;}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static
int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp11B=(void*)e->annot;
_LLBF: if(*((void**)_tmp11B)!= Cyc_CfFlowInfo_UnknownZ)goto _LLC1;_LLC0: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLC1: if(*((void**)_tmp11B)
!= Cyc_CfFlowInfo_NotZero)goto _LLC3;_LLC2: return 0;_LLC3: if(*((void**)_tmp11B)!= 
Cyc_CfFlowInfo_IsZero)goto _LLC5;_LLC4:({void*_tmp11C[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("dereference of NULL pointer",sizeof(unsigned char),28),_tag_arr(
_tmp11C,sizeof(void*),0));});return 0;_LLC5: if(_tmp11B != Cyc_Absyn_EmptyAnnot)
goto _LLC7;_LLC6:({void*_tmp11D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("!",
sizeof(unsigned char),2),_tag_arr(_tmp11D,sizeof(void*),0));});({void*_tmp11E[0]={};
Cyc_Tcutil_warn(e->loc,_tag_arr("unreachable dereference",sizeof(unsigned char),
24),_tag_arr(_tmp11E,sizeof(void*),0));});return 0;_LLC7:;_LLC8:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=({
struct Cyc_Core_Impossible_struct _tmp120;_tmp120.tag=Cyc_Core_Impossible;_tmp120.f1=
_tag_arr("need_null_check",sizeof(unsigned char),16);_tmp120;});_tmp11F;}));
_LLBE:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp121=(void*)e->annot;struct Cyc_List_List*_tmp122;struct Cyc_List_List*_tmp123;
_LLCA: if(*((void**)_tmp121)!= Cyc_CfFlowInfo_UnknownZ)goto _LLCC;_tmp122=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp121)->f1;_LLCB: return _tmp122;_LLCC: if(*((
void**)_tmp121)!= Cyc_CfFlowInfo_NotZero)goto _LLCE;_tmp123=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp121)->f1;_LLCD: return _tmp123;_LLCE: if(*((void**)_tmp121)!= Cyc_CfFlowInfo_IsZero)
goto _LLD0;_LLCF:({void*_tmp124[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(unsigned char),28),_tag_arr(_tmp124,sizeof(void*),0));});return 0;_LLD0:
if(_tmp121 != Cyc_Absyn_EmptyAnnot)goto _LLD2;_LLD1: return 0;_LLD2:;_LLD3:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp125=_cycalloc(sizeof(*
_tmp125));_tmp125[0]=({struct Cyc_Core_Impossible_struct _tmp126;_tmp126.tag=Cyc_Core_Impossible;
_tmp126.f1=_tag_arr("get_relns",sizeof(unsigned char),10);_tmp126;});_tmp125;}));
_LLC9:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp127=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp128;struct Cyc_Absyn_Conref*
_tmp129;struct Cyc_Absyn_Exp*_tmp12A;_LLD5: if(_tmp127 <= (void*)3?1:*((int*)
_tmp127)!= 4)goto _LLD7;_tmp128=((struct Cyc_Absyn_PointerType_struct*)_tmp127)->f1;
_tmp129=_tmp128.bounds;_LLD6: {void*_tmp12B=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp129);struct Cyc_Absyn_Exp*_tmp12C;_LLDC: if((int)_tmp12B != 0)goto _LLDE;_LLDD:
return 0;_LLDE: if(_tmp12B <= (void*)1?1:*((int*)_tmp12B)!= 0)goto _LLDB;_tmp12C=((
struct Cyc_Absyn_Upper_b_struct*)_tmp12B)->f1;_LLDF: {unsigned int j=Cyc_Evexp_eval_const_uint_exp(
_tmp12C);return i <= j;}_LLDB:;}_LLD7: if(_tmp127 <= (void*)3?1:*((int*)_tmp127)!= 7)
goto _LLD9;_tmp12A=((struct Cyc_Absyn_ArrayType_struct*)_tmp127)->f3;_LLD8: if(
_tmp12A == 0)return 0;{unsigned int j=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp12A));return i <= j;}_LLD9:;_LLDA: return 0;_LLD4:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp12D=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp12D->vd != y)continue;{void*_tmp12E=(void*)_tmp12D->rop;struct
Cyc_Absyn_Vardecl*_tmp12F;struct Cyc_Absyn_Vardecl*_tmp130;_LLE1: if(*((int*)
_tmp12E)!= 2)goto _LLE3;_tmp12F=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp12E)->f1;
_LLE2: _tmp130=_tmp12F;goto _LLE4;_LLE3: if(*((int*)_tmp12E)!= 4)goto _LLE5;_tmp130=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp12E)->f1;_LLE4: if(_tmp130 == v)return
1;else{goto _LLE0;}_LLE5:;_LLE6: continue;_LLE0:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp131=(void*)e->r;void*_tmp132;struct Cyc_Absyn_Vardecl*_tmp133;void*_tmp134;
struct Cyc_Absyn_Vardecl*_tmp135;void*_tmp136;struct Cyc_Absyn_Vardecl*_tmp137;
void*_tmp138;struct Cyc_Absyn_Vardecl*_tmp139;void*_tmp13A;struct Cyc_List_List*
_tmp13B;struct Cyc_List_List _tmp13C;struct Cyc_Absyn_Exp*_tmp13D;_LLE8: if(*((int*)
_tmp131)!= 1)goto _LLEA;_tmp132=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp131)->f2;
if(_tmp132 <= (void*)1?1:*((int*)_tmp132)!= 4)goto _LLEA;_tmp133=((struct Cyc_Absyn_Pat_b_struct*)
_tmp132)->f1;_LLE9: _tmp135=_tmp133;goto _LLEB;_LLEA: if(*((int*)_tmp131)!= 1)goto
_LLEC;_tmp134=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp131)->f2;if(_tmp134 <= (
void*)1?1:*((int*)_tmp134)!= 3)goto _LLEC;_tmp135=((struct Cyc_Absyn_Local_b_struct*)
_tmp134)->f1;_LLEB: _tmp137=_tmp135;goto _LLED;_LLEC: if(*((int*)_tmp131)!= 1)goto
_LLEE;_tmp136=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp131)->f2;if(_tmp136 <= (
void*)1?1:*((int*)_tmp136)!= 0)goto _LLEE;_tmp137=((struct Cyc_Absyn_Global_b_struct*)
_tmp136)->f1;_LLED: _tmp139=_tmp137;goto _LLEF;_LLEE: if(*((int*)_tmp131)!= 1)goto
_LLF0;_tmp138=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp131)->f2;if(_tmp138 <= (
void*)1?1:*((int*)_tmp138)!= 2)goto _LLF0;_tmp139=((struct Cyc_Absyn_Param_b_struct*)
_tmp138)->f1;_LLEF: if(_tmp139->escapes)return 0;if(Cyc_Toc_check_leq_size_var(
relns,v,_tmp139))return 1;goto _LLE7;_LLF0: if(*((int*)_tmp131)!= 3)goto _LLF2;
_tmp13A=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp131)->f1;if((int)_tmp13A != 
19)goto _LLF2;_tmp13B=((struct Cyc_Absyn_Primop_e_struct*)_tmp131)->f2;if(_tmp13B
== 0)goto _LLF2;_tmp13C=*_tmp13B;_tmp13D=(struct Cyc_Absyn_Exp*)_tmp13C.hd;_LLF1:{
void*_tmp13E=(void*)_tmp13D->r;void*_tmp13F;struct Cyc_Absyn_Vardecl*_tmp140;void*
_tmp141;struct Cyc_Absyn_Vardecl*_tmp142;void*_tmp143;struct Cyc_Absyn_Vardecl*
_tmp144;void*_tmp145;struct Cyc_Absyn_Vardecl*_tmp146;_LLF5: if(*((int*)_tmp13E)!= 
1)goto _LLF7;_tmp13F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if(
_tmp13F <= (void*)1?1:*((int*)_tmp13F)!= 4)goto _LLF7;_tmp140=((struct Cyc_Absyn_Pat_b_struct*)
_tmp13F)->f1;_LLF6: _tmp142=_tmp140;goto _LLF8;_LLF7: if(*((int*)_tmp13E)!= 1)goto
_LLF9;_tmp141=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if(_tmp141 <= (
void*)1?1:*((int*)_tmp141)!= 3)goto _LLF9;_tmp142=((struct Cyc_Absyn_Local_b_struct*)
_tmp141)->f1;_LLF8: _tmp144=_tmp142;goto _LLFA;_LLF9: if(*((int*)_tmp13E)!= 1)goto
_LLFB;_tmp143=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if(_tmp143 <= (
void*)1?1:*((int*)_tmp143)!= 0)goto _LLFB;_tmp144=((struct Cyc_Absyn_Global_b_struct*)
_tmp143)->f1;_LLFA: _tmp146=_tmp144;goto _LLFC;_LLFB: if(*((int*)_tmp13E)!= 1)goto
_LLFD;_tmp145=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if(_tmp145 <= (
void*)1?1:*((int*)_tmp145)!= 2)goto _LLFD;_tmp146=((struct Cyc_Absyn_Param_b_struct*)
_tmp145)->f1;_LLFC: return _tmp146 == v;_LLFD:;_LLFE: goto _LLF4;_LLF4:;}goto _LLE7;
_LLF2:;_LLF3: goto _LLE7;_LLE7:;}return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp147=(void*)a->r;
void*_tmp148;struct Cyc_Absyn_Vardecl*_tmp149;void*_tmp14A;struct Cyc_Absyn_Vardecl*
_tmp14B;void*_tmp14C;struct Cyc_Absyn_Vardecl*_tmp14D;void*_tmp14E;struct Cyc_Absyn_Vardecl*
_tmp14F;_LL100: if(*((int*)_tmp147)!= 1)goto _LL102;_tmp148=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp147)->f2;if(_tmp148 <= (void*)1?1:*((int*)_tmp148)!= 4)goto _LL102;_tmp149=((
struct Cyc_Absyn_Pat_b_struct*)_tmp148)->f1;_LL101: _tmp14B=_tmp149;goto _LL103;
_LL102: if(*((int*)_tmp147)!= 1)goto _LL104;_tmp14A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp147)->f2;if(_tmp14A <= (void*)1?1:*((int*)_tmp14A)!= 3)goto _LL104;_tmp14B=((
struct Cyc_Absyn_Local_b_struct*)_tmp14A)->f1;_LL103: _tmp14D=_tmp14B;goto _LL105;
_LL104: if(*((int*)_tmp147)!= 1)goto _LL106;_tmp14C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp147)->f2;if(_tmp14C <= (void*)1?1:*((int*)_tmp14C)!= 0)goto _LL106;_tmp14D=((
struct Cyc_Absyn_Global_b_struct*)_tmp14C)->f1;_LL105: _tmp14F=_tmp14D;goto _LL107;
_LL106: if(*((int*)_tmp147)!= 1)goto _LL108;_tmp14E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp147)->f2;if(_tmp14E <= (void*)1?1:*((int*)_tmp14E)!= 2)goto _LL108;_tmp14F=((
struct Cyc_Absyn_Param_b_struct*)_tmp14E)->f1;_LL107: if(_tmp14F->escapes)return 0;
inner_loop: {void*_tmp150=(void*)i->r;void*_tmp151;struct Cyc_Absyn_Exp*_tmp152;
void*_tmp153;void*_tmp154;int _tmp155;void*_tmp156;void*_tmp157;int _tmp158;void*
_tmp159;struct Cyc_List_List*_tmp15A;struct Cyc_List_List _tmp15B;struct Cyc_Absyn_Exp*
_tmp15C;struct Cyc_List_List*_tmp15D;struct Cyc_List_List _tmp15E;struct Cyc_Absyn_Exp*
_tmp15F;void*_tmp160;struct Cyc_Absyn_Vardecl*_tmp161;void*_tmp162;struct Cyc_Absyn_Vardecl*
_tmp163;void*_tmp164;struct Cyc_Absyn_Vardecl*_tmp165;void*_tmp166;struct Cyc_Absyn_Vardecl*
_tmp167;_LL10B: if(*((int*)_tmp150)!= 13)goto _LL10D;_tmp151=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp150)->f1;_tmp152=((struct Cyc_Absyn_Cast_e_struct*)_tmp150)->f2;_LL10C: i=
_tmp152;goto inner_loop;_LL10D: if(*((int*)_tmp150)!= 0)goto _LL10F;_tmp153=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp150)->f1;if(_tmp153 <= (void*)1?1:*((int*)
_tmp153)!= 2)goto _LL10F;_tmp154=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp153)->f1;
if((int)_tmp154 != 0)goto _LL10F;_tmp155=((struct Cyc_Absyn_Int_c_struct*)_tmp153)->f2;
_LL10E: return _tmp155 >= 0?Cyc_Toc_check_const_array((unsigned int)(_tmp155 + 1),(
void*)_tmp14F->type): 0;_LL10F: if(*((int*)_tmp150)!= 0)goto _LL111;_tmp156=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp150)->f1;if(_tmp156 <= (void*)1?1:*((int*)
_tmp156)!= 2)goto _LL111;_tmp157=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp156)->f1;
if((int)_tmp157 != 1)goto _LL111;_tmp158=((struct Cyc_Absyn_Int_c_struct*)_tmp156)->f2;
_LL110: return Cyc_Toc_check_const_array((unsigned int)(_tmp158 + 1),(void*)_tmp14F->type);
_LL111: if(*((int*)_tmp150)!= 3)goto _LL113;_tmp159=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp150)->f1;if((int)_tmp159 != 4)goto _LL113;_tmp15A=((struct Cyc_Absyn_Primop_e_struct*)
_tmp150)->f2;if(_tmp15A == 0)goto _LL113;_tmp15B=*_tmp15A;_tmp15C=(struct Cyc_Absyn_Exp*)
_tmp15B.hd;_tmp15D=_tmp15B.tl;if(_tmp15D == 0)goto _LL113;_tmp15E=*_tmp15D;_tmp15F=(
struct Cyc_Absyn_Exp*)_tmp15E.hd;_LL112: return Cyc_Toc_check_leq_size(relns,
_tmp14F,_tmp15F);_LL113: if(*((int*)_tmp150)!= 1)goto _LL115;_tmp160=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp150)->f2;if(_tmp160 <= (void*)1?1:*((int*)
_tmp160)!= 4)goto _LL115;_tmp161=((struct Cyc_Absyn_Pat_b_struct*)_tmp160)->f1;
_LL114: _tmp163=_tmp161;goto _LL116;_LL115: if(*((int*)_tmp150)!= 1)goto _LL117;
_tmp162=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp150)->f2;if(_tmp162 <= (void*)
1?1:*((int*)_tmp162)!= 3)goto _LL117;_tmp163=((struct Cyc_Absyn_Local_b_struct*)
_tmp162)->f1;_LL116: _tmp165=_tmp163;goto _LL118;_LL117: if(*((int*)_tmp150)!= 1)
goto _LL119;_tmp164=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp150)->f2;if(
_tmp164 <= (void*)1?1:*((int*)_tmp164)!= 0)goto _LL119;_tmp165=((struct Cyc_Absyn_Global_b_struct*)
_tmp164)->f1;_LL118: _tmp167=_tmp165;goto _LL11A;_LL119: if(*((int*)_tmp150)!= 1)
goto _LL11B;_tmp166=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp150)->f2;if(
_tmp166 <= (void*)1?1:*((int*)_tmp166)!= 2)goto _LL11B;_tmp167=((struct Cyc_Absyn_Param_b_struct*)
_tmp166)->f1;_LL11A: if(_tmp167->escapes)return 0;{struct Cyc_List_List*_tmp168=
relns;for(0;_tmp168 != 0;_tmp168=_tmp168->tl){struct Cyc_CfFlowInfo_Reln*_tmp169=(
struct Cyc_CfFlowInfo_Reln*)_tmp168->hd;if(_tmp169->vd == _tmp167){void*_tmp16A=(
void*)_tmp169->rop;struct Cyc_Absyn_Vardecl*_tmp16B;struct Cyc_Absyn_Vardecl*
_tmp16C;unsigned int _tmp16D;_LL11E: if(*((int*)_tmp16A)!= 2)goto _LL120;_tmp16B=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp16A)->f1;_LL11F: if(_tmp14F == _tmp16B)
return 1;else{goto _LL11D;}_LL120: if(*((int*)_tmp16A)!= 1)goto _LL122;_tmp16C=((
struct Cyc_CfFlowInfo_LessVar_struct*)_tmp16A)->f1;_LL121:{struct Cyc_List_List*
_tmp16E=relns;for(0;_tmp16E != 0;_tmp16E=_tmp16E->tl){struct Cyc_CfFlowInfo_Reln*
_tmp16F=(struct Cyc_CfFlowInfo_Reln*)_tmp16E->hd;if(_tmp16F->vd == _tmp16C){void*
_tmp170=(void*)_tmp16F->rop;struct Cyc_Absyn_Vardecl*_tmp171;struct Cyc_Absyn_Vardecl*
_tmp172;unsigned int _tmp173;struct Cyc_Absyn_Vardecl*_tmp174;_LL127: if(*((int*)
_tmp170)!= 4)goto _LL129;_tmp171=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp170)->f1;_LL128: _tmp172=_tmp171;goto _LL12A;_LL129: if(*((int*)_tmp170)!= 2)
goto _LL12B;_tmp172=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp170)->f1;_LL12A:
if(_tmp14F == _tmp172)return 1;else{goto _LL126;}_LL12B: if(*((int*)_tmp170)!= 0)
goto _LL12D;_tmp173=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp170)->f1;_LL12C:
return Cyc_Toc_check_const_array(_tmp173,(void*)_tmp14F->type);_LL12D: if(*((int*)
_tmp170)!= 1)goto _LL12F;_tmp174=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp170)->f1;
_LL12E: if(Cyc_Toc_check_leq_size_var(relns,_tmp14F,_tmp174))return 1;else{goto
_LL126;}_LL12F:;_LL130: goto _LL126;_LL126:;}}}goto _LL11D;_LL122: if(*((int*)
_tmp16A)!= 3)goto _LL124;_tmp16D=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp16A)->f1;
_LL123: return Cyc_Toc_check_const_array(_tmp16D,(void*)_tmp14F->type);_LL124:;
_LL125: goto _LL11D;_LL11D:;}}}goto _LL10A;_LL11B:;_LL11C: goto _LL10A;_LL10A:;}goto
_LLFF;_LL108:;_LL109: goto _LLFF;_LLFF:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp175[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",
sizeof(unsigned char),24),_tag_arr(_tmp175,sizeof(void*),0));});return Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp176[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",
sizeof(unsigned char),24),_tag_arr(_tmp176,sizeof(void*),0));});return(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple4*Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp*e){return({struct _tuple4*_tmp177=_cycalloc(sizeof(*_tmp177));
_tmp177->f1=Cyc_Toc_mt_tq;_tmp177->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp177;});}static struct _tuple5*Cyc_Toc_add_designator(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);return({
struct _tuple5*_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178->f1=0;_tmp178->f2=e;
_tmp178;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,
struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*
rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp179=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(
is_atomic?Cyc_Toc_malloc_atomic(_tmp179): Cyc_Toc_malloc_ptr(_tmp179));else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(rgnopt);Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp179);}}}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int e1_already_translated);static struct
Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp17A=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp17A != 0;_tmp17A=_tmp17A->tl){struct _tuple5 _tmp17C;struct Cyc_List_List*
_tmp17D;struct Cyc_Absyn_Exp*_tmp17E;struct _tuple5*_tmp17B=(struct _tuple5*)
_tmp17A->hd;_tmp17C=*_tmp17B;_tmp17D=_tmp17C.f1;_tmp17E=_tmp17C.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp17D == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp17D->tl != 0)({void*_tmp17F[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("multiple designators in array",sizeof(
unsigned char),30),_tag_arr(_tmp17F,sizeof(void*),0));});{void*_tmp180=(void*)
_tmp17D->hd;void*_tmp181=_tmp180;struct Cyc_Absyn_Exp*_tmp182;_LL132: if(*((int*)
_tmp181)!= 0)goto _LL134;_tmp182=((struct Cyc_Absyn_ArrayElement_struct*)_tmp181)->f1;
_LL133: Cyc_Toc_exp_to_c(nv,_tmp182);e_index=_tmp182;goto _LL131;_LL134: if(*((int*)
_tmp181)!= 1)goto _LL131;_LL135:({void*_tmp183[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("field name designators in array",
sizeof(unsigned char),32),_tag_arr(_tmp183,sizeof(void*),0));});_LL131:;}}{
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp184=(
void*)_tmp17E->r;struct Cyc_List_List*_tmp185;struct Cyc_Absyn_Vardecl*_tmp186;
struct Cyc_Absyn_Exp*_tmp187;struct Cyc_Absyn_Exp*_tmp188;void*_tmp189;struct Cyc_List_List*
_tmp18A;_LL137: if(*((int*)_tmp184)!= 26)goto _LL139;_tmp185=((struct Cyc_Absyn_Array_e_struct*)
_tmp184)->f1;_LL138: s=Cyc_Toc_init_array(nv,lval,_tmp185,s);goto _LL136;_LL139:
if(*((int*)_tmp184)!= 27)goto _LL13B;_tmp186=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp184)->f1;_tmp187=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp184)->f2;
_tmp188=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp184)->f3;_LL13A: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp186,_tmp187,_tmp188,s,0);goto _LL136;_LL13B: if(*((int*)_tmp184)!= 29)
goto _LL13D;_tmp189=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp184)->f1;
_tmp18A=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp184)->f2;_LL13C: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp189,_tmp18A,s);goto _LL136;_LL13D:;_LL13E: Cyc_Toc_exp_to_c(nv,_tmp17E);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp17E,0),s,0);goto _LL136;_LL136:;}}}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple1*_tmp18B=vd->name;if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(nv,_tmp18B,Cyc_Absyn_varb_exp(
_tmp18B,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp193=_cycalloc(sizeof(*
_tmp193));_tmp193[0]=({struct Cyc_Absyn_Local_b_struct _tmp194;_tmp194.tag=3;
_tmp194.f1=vd;_tmp194;});_tmp193;}),0));struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp18B,0),Cyc_Absyn_signed_int_exp(
0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp18B,0),Cyc_Absyn_var_exp(
max,0),0);struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(
_tmp18B,0),0);struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(
_tmp18B,0),0);struct Cyc_Absyn_Stmt*body;{void*_tmp18C=(void*)e2->r;struct Cyc_List_List*
_tmp18D;struct Cyc_Absyn_Vardecl*_tmp18E;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*
_tmp190;void*_tmp191;struct Cyc_List_List*_tmp192;_LL140: if(*((int*)_tmp18C)!= 26)
goto _LL142;_tmp18D=((struct Cyc_Absyn_Array_e_struct*)_tmp18C)->f1;_LL141: body=
Cyc_Toc_init_array(nv2,lval,_tmp18D,Cyc_Toc_skip_stmt_dl());goto _LL13F;_LL142:
if(*((int*)_tmp18C)!= 27)goto _LL144;_tmp18E=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp18C)->f1;_tmp18F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp18C)->f2;
_tmp190=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp18C)->f3;_LL143: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp18E,_tmp18F,_tmp190,Cyc_Toc_skip_stmt_dl(),0);goto _LL13F;_LL144: if(*((
int*)_tmp18C)!= 29)goto _LL146;_tmp191=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp18C)->f1;_tmp192=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp18C)->f2;_LL145:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp191,_tmp192,Cyc_Toc_skip_stmt_dl());
goto _LL13F;_LL146:;_LL147: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL13F;_LL13F:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp18B,Cyc_Absyn_uint_t,0,s2,0),
0),s,0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp195=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp195 != 0;_tmp195=_tmp195->tl){struct _tuple5 _tmp197;
struct Cyc_List_List*_tmp198;struct Cyc_Absyn_Exp*_tmp199;struct _tuple5*_tmp196=(
struct _tuple5*)_tmp195->hd;_tmp197=*_tmp196;_tmp198=_tmp197.f1;_tmp199=_tmp197.f2;
if(_tmp198 == 0)({void*_tmp19A[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",sizeof(
unsigned char),22),_tag_arr(_tmp19A,sizeof(void*),0));});if(_tmp198->tl != 0)({
void*_tmp19B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("too many designators in anonymous struct",sizeof(unsigned char),41),
_tag_arr(_tmp19B,sizeof(void*),0));});{void*_tmp19C=(void*)_tmp198->hd;struct
_tagged_arr*_tmp19D;_LL149: if(*((int*)_tmp19C)!= 1)goto _LL14B;_tmp19D=((struct
Cyc_Absyn_FieldName_struct*)_tmp19C)->f1;_LL14A: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp19D,0);{void*_tmp19E=(void*)_tmp199->r;struct Cyc_List_List*_tmp19F;
struct Cyc_Absyn_Vardecl*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_Absyn_Exp*
_tmp1A2;void*_tmp1A3;struct Cyc_List_List*_tmp1A4;_LL14E: if(*((int*)_tmp19E)!= 26)
goto _LL150;_tmp19F=((struct Cyc_Absyn_Array_e_struct*)_tmp19E)->f1;_LL14F: s=Cyc_Toc_init_array(
nv,lval,_tmp19F,s);goto _LL14D;_LL150: if(*((int*)_tmp19E)!= 27)goto _LL152;_tmp1A0=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp19E)->f1;_tmp1A1=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp19E)->f2;_tmp1A2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp19E)->f3;
_LL151: s=Cyc_Toc_init_comprehension(nv,lval,_tmp1A0,_tmp1A1,_tmp1A2,s,0);goto
_LL14D;_LL152: if(*((int*)_tmp19E)!= 29)goto _LL154;_tmp1A3=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp19E)->f1;_tmp1A4=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp19E)->f2;_LL153:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp1A3,_tmp1A4,s);goto _LL14D;_LL154:;_LL155:
Cyc_Toc_exp_to_c(nv,_tmp199);if(Cyc_Toc_is_poly_field(struct_type,_tmp19D))
_tmp199=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp199,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp199,0),0),s,0);goto _LL14D;
_LL14D:;}goto _LL148;}_LL14B:;_LL14C:({void*_tmp1A5[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(unsigned char),27),_tag_arr(_tmp1A5,sizeof(void*),0));});_LL148:;}}}
return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int
pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct _RegionHandle
_tmp1A6=_new_region("r");struct _RegionHandle*r=& _tmp1A6;_push_region(r);{struct
Cyc_List_List*_tmp1A7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,
es);struct _tagged_arr*_tmp1A8=Cyc_Toc_add_tuple_type(_tmp1A7);struct _tuple1*
_tmp1A9=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1AA=Cyc_Absyn_var_exp(_tmp1A9,
0);struct Cyc_Absyn_Stmt*_tmp1AB=Cyc_Absyn_exp_stmt(_tmp1AA,0);struct Cyc_Absyn_Exp*(*
_tmp1AC)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*)=
pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct
Cyc_List_List*_tmp1AD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp1AD);for(0;_tmp1AD != 0;(_tmp1AD=_tmp1AD->tl,-- i)){struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)_tmp1AD->hd;struct Cyc_Absyn_Exp*lval=_tmp1AC(_tmp1AA,Cyc_Absyn_fieldname(
i),0);is_atomic=is_atomic?Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v): 0;{void*_tmp1AE=(void*)e->r;struct Cyc_List_List*
_tmp1AF;struct Cyc_Absyn_Vardecl*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_Exp*
_tmp1B2;_LL157: if(*((int*)_tmp1AE)!= 26)goto _LL159;_tmp1AF=((struct Cyc_Absyn_Array_e_struct*)
_tmp1AE)->f1;_LL158: _tmp1AB=Cyc_Toc_init_array(nv,lval,_tmp1AF,_tmp1AB);goto
_LL156;_LL159: if(*((int*)_tmp1AE)!= 27)goto _LL15B;_tmp1B0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1AE)->f1;_tmp1B1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1AE)->f2;
_tmp1B2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1AE)->f3;_LL15A: _tmp1AB=
Cyc_Toc_init_comprehension(nv,lval,_tmp1B0,_tmp1B1,_tmp1B2,_tmp1AB,0);goto _LL156;
_LL15B:;_LL15C: Cyc_Toc_exp_to_c(nv,e);_tmp1AB=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(_tmp1AC(_tmp1AA,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp1AB,0);
goto _LL156;_LL156:;}}}{struct Cyc_Absyn_Exp*_tmp1B3=Cyc_Toc_make_struct(nv,
_tmp1A9,Cyc_Absyn_strct(_tmp1A8),_tmp1AB,pointer,rgnopt,is_atomic);_npop_handler(
0);return _tmp1B3;}};_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp1B4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp1B5=Cyc_Absyn_var_exp(_tmp1B4,0);struct Cyc_Absyn_Stmt*
_tmp1B6=Cyc_Absyn_exp_stmt(_tmp1B5,0);struct Cyc_Absyn_Exp*(*_tmp1B7)(struct Cyc_Absyn_Exp*,
struct _tagged_arr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp1B8=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp1B9;void*
_tmp1BA;_LL15E: if(_tmp1B8 <= (void*)3?1:*((int*)_tmp1B8)!= 10)goto _LL160;_tmp1B9=((
struct Cyc_Absyn_AggrType_struct*)_tmp1B8)->f1;_tmp1BA=(void*)_tmp1B9.aggr_info;
_LL15F: ad=Cyc_Absyn_get_known_aggrdecl(_tmp1BA);goto _LL15D;_LL160:;_LL161:({void*
_tmp1BB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("init_struct: bad struct type",sizeof(unsigned char),29),_tag_arr(
_tmp1BB,sizeof(void*),0));});_LL15D:;}{struct _RegionHandle _tmp1BC=_new_region("r");
struct _RegionHandle*r=& _tmp1BC;_push_region(r);{struct Cyc_List_List*_tmp1BD=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
r,dles);for(0;_tmp1BD != 0;_tmp1BD=_tmp1BD->tl){struct _tuple5 _tmp1BF;struct Cyc_List_List*
_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C1;struct _tuple5*_tmp1BE=(struct _tuple5*)
_tmp1BD->hd;_tmp1BF=*_tmp1BE;_tmp1C0=_tmp1BF.f1;_tmp1C1=_tmp1BF.f2;is_atomic=
is_atomic?Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1C1->topt))->v):
0;if(_tmp1C0 == 0)({void*_tmp1C2[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",sizeof(
unsigned char),22),_tag_arr(_tmp1C2,sizeof(void*),0));});if(_tmp1C0->tl != 0){
struct _tuple1*_tmp1C3=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1C4=Cyc_Absyn_var_exp(
_tmp1C3,0);for(0;_tmp1C0 != 0;_tmp1C0=_tmp1C0->tl){void*_tmp1C5=(void*)_tmp1C0->hd;
struct _tagged_arr*_tmp1C6;_LL163: if(*((int*)_tmp1C5)!= 1)goto _LL165;_tmp1C6=((
struct Cyc_Absyn_FieldName_struct*)_tmp1C5)->f1;_LL164: if(Cyc_Toc_is_poly_field(
struct_type,_tmp1C6))_tmp1C4=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp1C4,0);_tmp1B6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp1B7(_tmp1B5,_tmp1C6,0),_tmp1C4,0),0),_tmp1B6,0);goto _LL162;_LL165:;_LL166:({
void*_tmp1C7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct",sizeof(unsigned char),27),_tag_arr(_tmp1C7,
sizeof(void*),0));});_LL162:;}Cyc_Toc_exp_to_c(nv,_tmp1C1);_tmp1B6=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1C4,_tmp1C1,0),0),_tmp1B6,0);}else{
void*_tmp1C8=(void*)_tmp1C0->hd;struct _tagged_arr*_tmp1C9;_LL168: if(*((int*)
_tmp1C8)!= 1)goto _LL16A;_tmp1C9=((struct Cyc_Absyn_FieldName_struct*)_tmp1C8)->f1;
_LL169: {struct Cyc_Absyn_Exp*lval=_tmp1B7(_tmp1B5,_tmp1C9,0);{void*_tmp1CA=(void*)
_tmp1C1->r;struct Cyc_List_List*_tmp1CB;struct Cyc_Absyn_Vardecl*_tmp1CC;struct Cyc_Absyn_Exp*
_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CE;void*_tmp1CF;struct Cyc_List_List*_tmp1D0;
_LL16D: if(*((int*)_tmp1CA)!= 26)goto _LL16F;_tmp1CB=((struct Cyc_Absyn_Array_e_struct*)
_tmp1CA)->f1;_LL16E: _tmp1B6=Cyc_Toc_init_array(nv,lval,_tmp1CB,_tmp1B6);goto
_LL16C;_LL16F: if(*((int*)_tmp1CA)!= 27)goto _LL171;_tmp1CC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1CA)->f1;_tmp1CD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1CA)->f2;
_tmp1CE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1CA)->f3;_LL170: _tmp1B6=
Cyc_Toc_init_comprehension(nv,lval,_tmp1CC,_tmp1CD,_tmp1CE,_tmp1B6,0);goto _LL16C;
_LL171: if(*((int*)_tmp1CA)!= 29)goto _LL173;_tmp1CF=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1CA)->f1;_tmp1D0=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1CA)->f2;_LL172:
_tmp1B6=Cyc_Toc_init_anon_struct(nv,lval,_tmp1CF,_tmp1D0,_tmp1B6);goto _LL16C;
_LL173:;_LL174: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1C1->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp1C1);if(Cyc_Toc_is_poly_field(
struct_type,_tmp1C9)?!was_ptr_type: 0)_tmp1C1=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp1C1,0);if(has_exists)_tmp1C1=Cyc_Absyn_cast_exp((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(Cyc_Absyn_lookup_decl_field(ad,_tmp1C9)))->type,_tmp1C1,0);_tmp1B6=
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1C1,0),0),
_tmp1B6,0);goto _LL16C;}_LL16C:;}goto _LL167;}_LL16A:;_LL16B:({void*_tmp1D1[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(unsigned char),27),_tag_arr(_tmp1D1,sizeof(void*),0));});_LL167:;}}}{
struct Cyc_Absyn_Exp*_tmp1D2=Cyc_Toc_make_struct(nv,_tmp1B4,Cyc_Absyn_strctq(tdn),
_tmp1B6,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp1D2;};_pop_region(r);}}
struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple8*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));
_tmp1D3[0]=({struct Cyc_Absyn_Increment_e_struct _tmp1D4;_tmp1D4.tag=5;_tmp1D4.f1=
e1;_tmp1D4.f2=(void*)incr;_tmp1D4;});_tmp1D3;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp1D5=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp1D6;void*_tmp1D7;struct Cyc_Absyn_Exp*
_tmp1D8;struct Cyc_Absyn_Exp*_tmp1D9;struct _tagged_arr*_tmp1DA;_LL176: if(*((int*)
_tmp1D5)!= 35)goto _LL178;_tmp1D6=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp1D5)->f1;
_LL177: Cyc_Toc_lvalue_assign_stmt(_tmp1D6,fs,f,f_env);goto _LL175;_LL178: if(*((
int*)_tmp1D5)!= 13)goto _LL17A;_tmp1D7=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp1D5)->f1;_tmp1D8=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D5)->f2;_LL179: Cyc_Toc_lvalue_assign(
_tmp1D8,fs,f,f_env);goto _LL175;_LL17A: if(*((int*)_tmp1D5)!= 21)goto _LL17C;
_tmp1D9=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D5)->f1;_tmp1DA=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1D5)->f2;_LL17B:(void*)(e1->r=(void*)((void*)_tmp1D9->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));
_tmp1DB->hd=_tmp1DA;_tmp1DB->tl=fs;_tmp1DB;}),f,f_env);goto _LL175;_LL17C:;_LL17D: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _tagged_arr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL175;}_LL175:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp1DC=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp1DD;struct Cyc_Absyn_Decl*_tmp1DE;struct Cyc_Absyn_Stmt*
_tmp1DF;struct Cyc_Absyn_Stmt*_tmp1E0;_LL17F: if(_tmp1DC <= (void*)1?1:*((int*)
_tmp1DC)!= 0)goto _LL181;_tmp1DD=((struct Cyc_Absyn_Exp_s_struct*)_tmp1DC)->f1;
_LL180: Cyc_Toc_lvalue_assign(_tmp1DD,fs,f,f_env);goto _LL17E;_LL181: if(_tmp1DC <= (
void*)1?1:*((int*)_tmp1DC)!= 12)goto _LL183;_tmp1DE=((struct Cyc_Absyn_Decl_s_struct*)
_tmp1DC)->f1;_tmp1DF=((struct Cyc_Absyn_Decl_s_struct*)_tmp1DC)->f2;_LL182: Cyc_Toc_lvalue_assign_stmt(
_tmp1DF,fs,f,f_env);goto _LL17E;_LL183: if(_tmp1DC <= (void*)1?1:*((int*)_tmp1DC)!= 
1)goto _LL185;_tmp1E0=((struct Cyc_Absyn_Seq_s_struct*)_tmp1DC)->f2;_LL184: Cyc_Toc_lvalue_assign_stmt(
_tmp1E0,fs,f,f_env);goto _LL17E;_LL185:;_LL186:({struct Cyc_Std_String_pa_struct
_tmp1E2;_tmp1E2.tag=0;_tmp1E2.f1=(struct _tagged_arr)Cyc_Absynpp_stmt2string(s);{
void*_tmp1E1[1]={& _tmp1E2};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("lvalue_assign_stmt: %s",sizeof(unsigned char),23),
_tag_arr(_tmp1E1,sizeof(void*),1));}});_LL17E:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp1E3=_region_malloc(r2,sizeof(*_tmp1E3));_tmp1E3->hd=(
void*)f((void*)x->hd,env);_tmp1E3->tl=0;_tmp1E3;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp1E4=_region_malloc(r2,sizeof(*_tmp1E4));_tmp1E4->hd=(void*)f((void*)x->hd,
env);_tmp1E4->tl=0;_tmp1E4;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple5*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple5*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5->f1=0;_tmp1E5->f2=e;_tmp1E5;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp1E6=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp1E7;_LL188: if(_tmp1E6 <= (void*)3?1:*((int*)_tmp1E6)
!= 4)goto _LL18A;_tmp1E7=((struct Cyc_Absyn_PointerType_struct*)_tmp1E6)->f1;
_LL189: return _tmp1E7;_LL18A:;_LL18B:({void*_tmp1E8[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("get_ptr_typ: not a pointer!",
sizeof(unsigned char),28),_tag_arr(_tmp1E8,sizeof(void*),0));});_LL187:;}struct
_tuple9{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple10{struct
Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp1E9=(void*)e->r;if(e->topt
== 0)({struct Cyc_Std_String_pa_struct _tmp1EB;_tmp1EB.tag=0;_tmp1EB.f1=(struct
_tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp1EA[1]={& _tmp1EB};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: no type for %s",
sizeof(unsigned char),25),_tag_arr(_tmp1EA,sizeof(void*),1));}});{void*old_typ=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp1EC=_tmp1E9;void*
_tmp1ED;struct _tuple1*_tmp1EE;void*_tmp1EF;struct _tuple1*_tmp1F0;void*_tmp1F1;
struct Cyc_List_List*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;void*_tmp1F4;struct Cyc_Absyn_Exp*
_tmp1F5;struct Cyc_Core_Opt*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F7;struct Cyc_Absyn_Exp*
_tmp1F8;struct Cyc_Absyn_Exp*_tmp1F9;struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Exp*
_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;struct Cyc_List_List*
_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_List_List*_tmp200;struct Cyc_Absyn_VarargCallInfo*
_tmp201;struct Cyc_Absyn_Exp*_tmp202;struct Cyc_List_List*_tmp203;struct Cyc_Absyn_VarargCallInfo*
_tmp204;struct Cyc_Absyn_VarargCallInfo _tmp205;int _tmp206;struct Cyc_List_List*
_tmp207;struct Cyc_Absyn_VarargInfo*_tmp208;struct Cyc_Absyn_Exp*_tmp209;struct Cyc_Absyn_Exp*
_tmp20A;struct Cyc_Absyn_Exp*_tmp20B;struct Cyc_List_List*_tmp20C;void*_tmp20D;
void**_tmp20E;struct Cyc_Absyn_Exp*_tmp20F;struct Cyc_Absyn_Exp*_tmp210;struct Cyc_Absyn_Exp*
_tmp211;struct Cyc_Absyn_Exp*_tmp212;struct Cyc_Absyn_Exp*_tmp213;void*_tmp214;
void*_tmp215;void*_tmp216;struct _tagged_arr*_tmp217;void*_tmp218;void*_tmp219;
unsigned int _tmp21A;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Exp*_tmp21C;
struct _tagged_arr*_tmp21D;struct Cyc_Absyn_Exp*_tmp21E;struct _tagged_arr*_tmp21F;
struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Exp*_tmp221;struct Cyc_List_List*
_tmp222;struct Cyc_List_List*_tmp223;struct Cyc_Absyn_Vardecl*_tmp224;struct Cyc_Absyn_Exp*
_tmp225;struct Cyc_Absyn_Exp*_tmp226;struct _tuple1*_tmp227;struct Cyc_List_List*
_tmp228;struct Cyc_List_List*_tmp229;struct Cyc_Absyn_Aggrdecl*_tmp22A;void*
_tmp22B;struct Cyc_List_List*_tmp22C;struct Cyc_List_List*_tmp22D;struct Cyc_Absyn_Tuniondecl*
_tmp22E;struct Cyc_Absyn_Tunionfield*_tmp22F;struct Cyc_List_List*_tmp230;struct
Cyc_Absyn_Tuniondecl*_tmp231;struct Cyc_Absyn_Tunionfield*_tmp232;struct Cyc_Absyn_MallocInfo
_tmp233;int _tmp234;struct Cyc_Absyn_Exp*_tmp235;void**_tmp236;struct Cyc_Absyn_Exp*
_tmp237;int _tmp238;struct Cyc_Absyn_Stmt*_tmp239;struct Cyc_Absyn_Fndecl*_tmp23A;
_LL18D: if(*((int*)_tmp1EC)!= 0)goto _LL18F;_tmp1ED=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1EC)->f1;if((int)_tmp1ED != 0)goto _LL18F;_LL18E: {struct Cyc_Absyn_Exp*_tmp23B=
Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_typ,_tmp23B,
_tmp23B))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp23C[3];_tmp23C[2]=_tmp23B;_tmp23C[1]=_tmp23B;_tmp23C[0]=
_tmp23B;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp23C,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(void*)((
void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL18C;}_LL18F: if(*((int*)
_tmp1EC)!= 0)goto _LL191;_LL190: goto _LL18C;_LL191: if(*((int*)_tmp1EC)!= 1)goto
_LL193;_tmp1EE=((struct Cyc_Absyn_Var_e_struct*)_tmp1EC)->f1;_tmp1EF=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp1EC)->f2;_LL192:{struct _handler_cons _tmp23D;
_push_handler(& _tmp23D);{int _tmp23F=0;if(setjmp(_tmp23D.handler))_tmp23F=1;if(!
_tmp23F){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k))Cyc_Dict_lookup)(nv->varmap,_tmp1EE))->r));;_pop_handler();}
else{void*_tmp23E=(void*)_exn_thrown;void*_tmp241=_tmp23E;_LL1E2: if(_tmp241 != 
Cyc_Dict_Absent)goto _LL1E4;_LL1E3:({struct Cyc_Std_String_pa_struct _tmp243;
_tmp243.tag=0;_tmp243.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1EE);{
void*_tmp242[1]={& _tmp243};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("Can't find %s in exp_to_c, Var\n",sizeof(
unsigned char),32),_tag_arr(_tmp242,sizeof(void*),1));}});_LL1E4:;_LL1E5:(void)
_throw(_tmp241);_LL1E1:;}}}goto _LL18C;_LL193: if(*((int*)_tmp1EC)!= 2)goto _LL195;
_tmp1F0=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1EC)->f1;_LL194:({void*_tmp244[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unknownid",sizeof(unsigned char),10),_tag_arr(_tmp244,sizeof(void*),0));});
_LL195: if(*((int*)_tmp1EC)!= 3)goto _LL197;_tmp1F1=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1EC)->f1;_tmp1F2=((struct Cyc_Absyn_Primop_e_struct*)_tmp1EC)->f2;_LL196: {
struct Cyc_List_List*_tmp245=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp1F2);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp1F2);{void*_tmp246=_tmp1F1;_LL1E7:
if((int)_tmp246 != 19)goto _LL1E9;_LL1E8: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1F2))->hd;{void*_tmp247=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_Exp*
_tmp248;struct Cyc_Absyn_PtrInfo _tmp249;void*_tmp24A;struct Cyc_Absyn_Conref*
_tmp24B;struct Cyc_Absyn_Conref*_tmp24C;_LL1FC: if(_tmp247 <= (void*)3?1:*((int*)
_tmp247)!= 7)goto _LL1FE;_tmp248=((struct Cyc_Absyn_ArrayType_struct*)_tmp247)->f3;
_LL1FD:(void*)(e->r=(void*)((void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp248))->r));
goto _LL1FB;_LL1FE: if(_tmp247 <= (void*)3?1:*((int*)_tmp247)!= 4)goto _LL200;
_tmp249=((struct Cyc_Absyn_PointerType_struct*)_tmp247)->f1;_tmp24A=(void*)
_tmp249.elt_typ;_tmp24B=_tmp249.nullable;_tmp24C=_tmp249.bounds;_LL1FF:{void*
_tmp24D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp24C);struct Cyc_Absyn_Exp*
_tmp24E;_LL203: if((int)_tmp24D != 0)goto _LL205;_LL204:(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({struct Cyc_Absyn_Exp*_tmp24F[2];
_tmp24F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp24A),0);_tmp24F[0]=(
struct Cyc_Absyn_Exp*)_tmp1F2->hd;((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp24F,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));
goto _LL202;_LL205: if(_tmp24D <= (void*)1?1:*((int*)_tmp24D)!= 0)goto _LL202;
_tmp24E=((struct Cyc_Absyn_Upper_b_struct*)_tmp24D)->f1;_LL206: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp24B))(void*)(e->r=(void*)((
void*)(Cyc_Absyn_conditional_exp(arg,_tmp24E,Cyc_Absyn_uint_exp(0,0),0))->r));
else{(void*)(e->r=(void*)((void*)_tmp24E->r));goto _LL202;}_LL202:;}goto _LL1FB;
_LL200:;_LL201:({struct Cyc_Std_String_pa_struct _tmp252;_tmp252.tag=0;_tmp252.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);{struct Cyc_Std_String_pa_struct _tmp251;_tmp251.tag=0;
_tmp251.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);{void*_tmp250[2]={& _tmp251,& _tmp252};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("size primop applied to non-array %s (%s)",
sizeof(unsigned char),41),_tag_arr(_tmp250,sizeof(void*),2));}}});_LL1FB:;}goto
_LL1E6;}_LL1E9: if((int)_tmp246 != 0)goto _LL1EB;_LL1EA: {void*elt_typ=(void*)0;if(
Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(
_tmp245))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp1F2))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1F2->tl))->hd;(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*_tmp253[3];
_tmp253[2]=e2;_tmp253[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);
_tmp253[0]=e1;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp253,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}goto _LL1E6;}
_LL1EB: if((int)_tmp246 != 2)goto _LL1ED;_LL1EC: {void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp245))->hd,& elt_typ)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1F2))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1F2->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp245->tl))->hd)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,
0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*
_tmp254[3];_tmp254[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);_tmp254[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp254[0]=e1;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp254,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}}goto _LL1E6;}_LL1ED: if((int)_tmp246 != 5)goto _LL1EF;_LL1EE: goto _LL1F0;
_LL1EF: if((int)_tmp246 != 6)goto _LL1F1;_LL1F0: goto _LL1F2;_LL1F1: if((int)_tmp246 != 
7)goto _LL1F3;_LL1F2: goto _LL1F4;_LL1F3: if((int)_tmp246 != 9)goto _LL1F5;_LL1F4: goto
_LL1F6;_LL1F5: if((int)_tmp246 != 8)goto _LL1F7;_LL1F6: goto _LL1F8;_LL1F7: if((int)
_tmp246 != 10)goto _LL1F9;_LL1F8: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1F2))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1F2->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp245))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp245->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,
0))->r));if(Cyc_Tcutil_is_tagged_pointer_typ(t2))(void*)(e2->r=(void*)((void*)(
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));
goto _LL1E6;}_LL1F9:;_LL1FA: goto _LL1E6;_LL1E6:;}goto _LL18C;}_LL197: if(*((int*)
_tmp1EC)!= 5)goto _LL199;_tmp1F3=((struct Cyc_Absyn_Increment_e_struct*)_tmp1EC)->f1;
_tmp1F4=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1EC)->f2;_LL198: {void*
e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1F3->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp1F3);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*_tmp255=(_tmp1F4 == (void*)1?1: _tmp1F4 == (
void*)3)?Cyc_Toc__tagged_arr_inplace_plus_post_e: Cyc_Toc__tagged_arr_inplace_plus_e;
int _tmp256=(_tmp1F4 == (void*)0?1: _tmp1F4 == (void*)1)?1: - 1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(_tmp255,({struct Cyc_Absyn_Exp*_tmp257[3];_tmp257[2]=
Cyc_Absyn_signed_int_exp(_tmp256,0);_tmp257[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0);_tmp257[0]=Cyc_Absyn_address_exp(_tmp1F3,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp257,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}if(elt_typ == (void*)0?!Cyc_Absyn_is_lvalue(_tmp1F3): 0){Cyc_Toc_lvalue_assign(
_tmp1F3,0,Cyc_Toc_incr_lvalue,_tmp1F4);(void*)(e->r=(void*)((void*)_tmp1F3->r));}
goto _LL18C;}}_LL199: if(*((int*)_tmp1EC)!= 4)goto _LL19B;_tmp1F5=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1EC)->f1;_tmp1F6=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1EC)->f2;_tmp1F7=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1EC)->f3;_LL19A: {int e1_poly=Cyc_Toc_is_poly_project(
_tmp1F5);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1F5->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1F7->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp1F5);Cyc_Toc_exp_to_c(nv,_tmp1F7);{int done=0;if(_tmp1F6 != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp258=(void*)_tmp1F6->v;_LL208: if((int)_tmp258 != 0)goto _LL20A;
_LL209: change=_tmp1F7;goto _LL207;_LL20A: if((int)_tmp258 != 2)goto _LL20C;_LL20B:
change=Cyc_Absyn_prim1_exp((void*)2,_tmp1F7,0);goto _LL207;_LL20C:;_LL20D:({void*
_tmp259[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("bad t ? pointer arithmetic",sizeof(unsigned char),27),_tag_arr(_tmp259,
sizeof(void*),0));});_LL207:;}done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({struct Cyc_Absyn_Exp*_tmp25A[3];_tmp25A[2]=
change;_tmp25A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp25A[0]=
Cyc_Absyn_address_exp(_tmp1F5,0);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp25A,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}
if(!done){if(e1_poly)(void*)(_tmp1F7->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp1F7->r,0),0))->r));if(!Cyc_Absyn_is_lvalue(_tmp1F5)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,struct _tuple8*),struct _tuple8*f_env))Cyc_Toc_lvalue_assign)(
_tmp1F5,0,Cyc_Toc_assignop_lvalue,({struct _tuple8*_tmp25B=_cycalloc(sizeof(
struct _tuple8)* 1);_tmp25B[0]=({struct _tuple8 _tmp25C;_tmp25C.f1=_tmp1F6;_tmp25C.f2=
_tmp1F7;_tmp25C;});_tmp25B;}));(void*)(e->r=(void*)((void*)_tmp1F5->r));}}goto
_LL18C;}}_LL19B: if(*((int*)_tmp1EC)!= 6)goto _LL19D;_tmp1F8=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1EC)->f1;_tmp1F9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1EC)->f2;
_tmp1FA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1EC)->f3;_LL19C: Cyc_Toc_exp_to_c(
nv,_tmp1F8);Cyc_Toc_exp_to_c(nv,_tmp1F9);Cyc_Toc_exp_to_c(nv,_tmp1FA);goto _LL18C;
_LL19D: if(*((int*)_tmp1EC)!= 7)goto _LL19F;_tmp1FB=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1EC)->f1;_tmp1FC=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1EC)->f2;_LL19E: Cyc_Toc_exp_to_c(
nv,_tmp1FB);Cyc_Toc_exp_to_c(nv,_tmp1FC);goto _LL18C;_LL19F: if(*((int*)_tmp1EC)!= 
8)goto _LL1A1;_tmp1FD=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1EC)->f1;
_tmp1FE=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1EC)->f2;_LL1A0: _tmp1FF=
_tmp1FD;_tmp200=_tmp1FE;goto _LL1A2;_LL1A1: if(*((int*)_tmp1EC)!= 9)goto _LL1A3;
_tmp1FF=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1EC)->f1;_tmp200=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1EC)->f2;_tmp201=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1EC)->f3;if(_tmp201
!= 0)goto _LL1A3;_LL1A2: Cyc_Toc_exp_to_c(nv,_tmp1FF);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp200);goto _LL18C;_LL1A3: if(*((int*)_tmp1EC)!= 9)goto
_LL1A5;_tmp202=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1EC)->f1;_tmp203=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp1EC)->f2;_tmp204=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1EC)->f3;if(_tmp204 == 0)goto _LL1A5;_tmp205=*_tmp204;_tmp206=_tmp205.num_varargs;
_tmp207=_tmp205.injectors;_tmp208=_tmp205.vai;_LL1A4:{struct _RegionHandle _tmp25D=
_new_region("r");struct _RegionHandle*r=& _tmp25D;_push_region(r);{struct _tuple1*
argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp206,0);
void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp208->type);void*arr_type=(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp271=_cycalloc(sizeof(*_tmp271));_tmp271[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp272;_tmp272.tag=7;_tmp272.f1=(void*)cva_type;
_tmp272.f2=Cyc_Toc_mt_tq;_tmp272.f3=(struct Cyc_Absyn_Exp*)num_varargs_exp;
_tmp272;});_tmp271;});int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp203);int num_normargs=num_args - _tmp206;struct Cyc_List_List*new_args=0;{int i=
0;for(0;i < num_normargs;(++ i,_tmp203=_tmp203->tl)){Cyc_Toc_exp_to_c(nv,(struct
Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp203))->hd);new_args=({
struct Cyc_List_List*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->hd=(struct Cyc_Absyn_Exp*)
_tmp203->hd;_tmp25E->tl=new_args;_tmp25E;});}}new_args=({struct Cyc_List_List*
_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp260[3];_tmp260[2]=num_varargs_exp;_tmp260[1]=Cyc_Absyn_sizeoftyp_exp(
cva_type,0);_tmp260[0]=argvexp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp260,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp25F->tl=new_args;
_tmp25F;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_args);Cyc_Toc_exp_to_c(nv,_tmp202);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(_tmp202,new_args,0),0);if(_tmp208->inject){struct Cyc_Absyn_Tuniondecl*
tud;{void*_tmp261=Cyc_Tcutil_compress((void*)_tmp208->type);struct Cyc_Absyn_TunionInfo
_tmp262;void*_tmp263;struct Cyc_Absyn_Tuniondecl**_tmp264;struct Cyc_Absyn_Tuniondecl*
_tmp265;_LL20F: if(_tmp261 <= (void*)3?1:*((int*)_tmp261)!= 2)goto _LL211;_tmp262=((
struct Cyc_Absyn_TunionType_struct*)_tmp261)->f1;_tmp263=(void*)_tmp262.tunion_info;
if(*((int*)_tmp263)!= 1)goto _LL211;_tmp264=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp263)->f1;_tmp265=*_tmp264;_LL210: tud=_tmp265;goto _LL20E;_LL211:;_LL212:({
void*_tmp266[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("toc: unknown tunion in vararg with inject",sizeof(unsigned char),42),
_tag_arr(_tmp266,sizeof(void*),0));});_LL20E:;}{struct _tagged_arr vs=({
unsigned int _tmp26D=(unsigned int)_tmp206;struct _tuple1**_tmp26E=(struct _tuple1**)
_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmp26D));struct _tagged_arr
_tmp270=_tag_arr(_tmp26E,sizeof(struct _tuple1*),(unsigned int)_tmp206);{
unsigned int _tmp26F=_tmp26D;unsigned int i;for(i=0;i < _tmp26F;i ++){_tmp26E[i]=Cyc_Toc_temp_var();}}
_tmp270;});struct Cyc_List_List*_tmp267=0;{int i=_tmp206 - 1;for(0;i >= 0;-- i){
_tmp267=({struct Cyc_List_List*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(((struct _tuple1**)vs.curr)[
i],0),0));_tmp268->tl=_tmp267;_tmp268;});}}s=Cyc_Absyn_declare_stmt(argv,
arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp267,0),s,0);{
int i=0;for(0;_tmp203 != 0;(((_tmp203=_tmp203->tl,_tmp207=_tmp207->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp203->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple1*var=((struct _tuple1**)
vs.curr)[i];struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield
_tmp26A;struct _tuple1*_tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_Absyn_Tunionfield*
_tmp269=(struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp207))->hd;
_tmp26A=*_tmp269;_tmp26B=_tmp26A.name;_tmp26C=_tmp26A.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp26C))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Absyn_cast_exp(field_typ,arg,
0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,
Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp26B,
1),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp26B,_tag_arr("_struct",sizeof(unsigned char),8))),0,s,0);}}}}}else{{int i=0;
for(0;_tmp203 != 0;(_tmp203=_tmp203->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp203->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp203->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL18C;_LL1A5: if(*((
int*)_tmp1EC)!= 10)goto _LL1A7;_tmp209=((struct Cyc_Absyn_Throw_e_struct*)_tmp1EC)->f1;
_LL1A6: Cyc_Toc_exp_to_c(nv,_tmp209);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp209),0))->r));goto _LL18C;
_LL1A7: if(*((int*)_tmp1EC)!= 11)goto _LL1A9;_tmp20A=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1EC)->f1;_LL1A8: Cyc_Toc_exp_to_c(nv,_tmp20A);goto _LL18C;_LL1A9: if(*((int*)
_tmp1EC)!= 12)goto _LL1AB;_tmp20B=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1EC)->f1;
_tmp20C=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1EC)->f2;_LL1AA: Cyc_Toc_exp_to_c(
nv,_tmp20B);for(0;_tmp20C != 0;_tmp20C=_tmp20C->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp20C->hd);if(k != (void*)4?k != (void*)3: 0){{void*_tmp273=Cyc_Tcutil_compress((
void*)_tmp20C->hd);_LL214: if(_tmp273 <= (void*)3?1:*((int*)_tmp273)!= 1)goto
_LL216;_LL215: goto _LL217;_LL216: if(_tmp273 <= (void*)3?1:*((int*)_tmp273)!= 2)
goto _LL218;_LL217: continue;_LL218:;_LL219:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp20B,
0))->r));goto _LL213;_LL213:;}break;}}goto _LL18C;_LL1AB: if(*((int*)_tmp1EC)!= 13)
goto _LL1AD;_tmp20D=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1EC)->f1;_tmp20E=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1EC)->f1);_tmp20F=((struct
Cyc_Absyn_Cast_e_struct*)_tmp1EC)->f2;_LL1AC: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp20F->topt))->v;void*new_typ=*_tmp20E;*_tmp20E=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp20F);{struct _tuple0 _tmp275=({struct _tuple0
_tmp274;_tmp274.f1=Cyc_Tcutil_compress(old_t2);_tmp274.f2=Cyc_Tcutil_compress(
new_typ);_tmp274;});void*_tmp276;struct Cyc_Absyn_PtrInfo _tmp277;void*_tmp278;
struct Cyc_Absyn_PtrInfo _tmp279;void*_tmp27A;struct Cyc_Absyn_PtrInfo _tmp27B;void*
_tmp27C;_LL21B: _tmp276=_tmp275.f1;if(_tmp276 <= (void*)3?1:*((int*)_tmp276)!= 4)
goto _LL21D;_tmp277=((struct Cyc_Absyn_PointerType_struct*)_tmp276)->f1;_tmp278=
_tmp275.f2;if(_tmp278 <= (void*)3?1:*((int*)_tmp278)!= 4)goto _LL21D;_tmp279=((
struct Cyc_Absyn_PointerType_struct*)_tmp278)->f1;_LL21C: {int _tmp27D=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp277.nullable);int
_tmp27E=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp279.nullable);
void*_tmp27F=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp277.bounds);void*
_tmp280=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp279.bounds);{struct _tuple0
_tmp282=({struct _tuple0 _tmp281;_tmp281.f1=_tmp27F;_tmp281.f2=_tmp280;_tmp281;});
void*_tmp283;void*_tmp284;void*_tmp285;struct Cyc_Absyn_Exp*_tmp286;void*_tmp287;
void*_tmp288;void*_tmp289;struct Cyc_Absyn_Exp*_tmp28A;_LL222: _tmp283=_tmp282.f1;
if(_tmp283 <= (void*)1?1:*((int*)_tmp283)!= 0)goto _LL224;_tmp284=_tmp282.f2;if(
_tmp284 <= (void*)1?1:*((int*)_tmp284)!= 0)goto _LL224;_LL223: if(_tmp27D?!_tmp27E:
0){if(nv->toplevel)({void*_tmp28B[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("can't do NULL-check conversion at top-level",
sizeof(unsigned char),44),_tag_arr(_tmp28B,sizeof(void*),0));});(void*)(e->r=(
void*)((void*)(Cyc_Absyn_cast_exp(*_tmp20E,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->hd=_tmp20F;
_tmp28C->tl=0;_tmp28C;}),0),0))->r));}goto _LL221;_LL224: _tmp285=_tmp282.f1;if(
_tmp285 <= (void*)1?1:*((int*)_tmp285)!= 0)goto _LL226;_tmp286=((struct Cyc_Absyn_Upper_b_struct*)
_tmp285)->f1;_tmp287=_tmp282.f2;if((int)_tmp287 != 0)goto _LL226;_LL225: if(nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_t2,_tmp286,
_tmp20F))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp28D[3];_tmp28D[2]=_tmp286;_tmp28D[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp279.elt_typ),0);_tmp28D[0]=_tmp20F;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp28D,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}goto _LL221;_LL226: _tmp288=_tmp282.f1;if((int)_tmp288 != 0)goto
_LL228;_tmp289=_tmp282.f2;if(_tmp289 <= (void*)1?1:*((int*)_tmp289)!= 0)goto
_LL228;_tmp28A=((struct Cyc_Absyn_Upper_b_struct*)_tmp289)->f1;_LL227: if(nv->toplevel)({
void*_tmp28E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level",sizeof(unsigned char),45),
_tag_arr(_tmp28E,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp28F=Cyc_Absyn_fncall_exp(
Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*_tmp291[3];_tmp291[2]=_tmp28A;_tmp291[
1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_tmp277.elt_typ),0);_tmp291[0]=
_tmp20F;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp291,sizeof(struct Cyc_Absyn_Exp*),3));}),0);if(_tmp27E)(void*)(_tmp28F->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp290=_cycalloc(sizeof(*_tmp290));_tmp290->hd=Cyc_Absyn_copy_exp(_tmp28F);
_tmp290->tl=0;_tmp290;}),0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*
_tmp20E,_tmp28F,0))->r));goto _LL221;}_LL228:;_LL229: goto _LL221;_LL221:;}goto
_LL21A;}_LL21D: _tmp27A=_tmp275.f1;if(_tmp27A <= (void*)3?1:*((int*)_tmp27A)!= 4)
goto _LL21F;_tmp27B=((struct Cyc_Absyn_PointerType_struct*)_tmp27A)->f1;_tmp27C=
_tmp275.f2;if(_tmp27C <= (void*)3?1:*((int*)_tmp27C)!= 5)goto _LL21F;_LL21E:{void*
_tmp292=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp27B.bounds);_LL22B: if((int)
_tmp292 != 0)goto _LL22D;_LL22C:(void*)(_tmp20F->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)_tmp20F->r,_tmp20F->loc),Cyc_Toc_curr_sp,0))->r));goto
_LL22A;_LL22D:;_LL22E: goto _LL22A;_LL22A:;}goto _LL21A;_LL21F:;_LL220: goto _LL21A;
_LL21A:;}goto _LL18C;}_LL1AD: if(*((int*)_tmp1EC)!= 14)goto _LL1AF;_tmp210=((struct
Cyc_Absyn_Address_e_struct*)_tmp1EC)->f1;_LL1AE:{void*_tmp293=(void*)_tmp210->r;
struct _tuple1*_tmp294;struct Cyc_List_List*_tmp295;struct Cyc_List_List*_tmp296;
struct Cyc_List_List*_tmp297;_LL230: if(*((int*)_tmp293)!= 28)goto _LL232;_tmp294=((
struct Cyc_Absyn_Struct_e_struct*)_tmp293)->f1;_tmp295=((struct Cyc_Absyn_Struct_e_struct*)
_tmp293)->f2;_tmp296=((struct Cyc_Absyn_Struct_e_struct*)_tmp293)->f3;_LL231: if(
nv->toplevel)({struct Cyc_Std_String_pa_struct _tmp299;_tmp299.tag=0;_tmp299.f1=(
struct _tagged_arr)Cyc_Position_string_of_segment(_tmp210->loc);{void*_tmp298[1]={&
_tmp299};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level",sizeof(unsigned char),42),
_tag_arr(_tmp298,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp210->topt))->v,_tmp295 != 0,1,0,
_tmp296,_tmp294))->r));goto _LL22F;_LL232: if(*((int*)_tmp293)!= 24)goto _LL234;
_tmp297=((struct Cyc_Absyn_Tuple_e_struct*)_tmp293)->f1;_LL233: if(nv->toplevel)({
struct Cyc_Std_String_pa_struct _tmp29B;_tmp29B.tag=0;_tmp29B.f1=(struct
_tagged_arr)Cyc_Position_string_of_segment(_tmp210->loc);{void*_tmp29A[1]={&
_tmp29B};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level",sizeof(unsigned char),42),
_tag_arr(_tmp29A,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp297))->r));goto _LL22F;_LL234:;_LL235: Cyc_Toc_exp_to_c(nv,_tmp210);if(
!Cyc_Absyn_is_lvalue(_tmp210)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp210,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp210,
0))->r));}goto _LL22F;_LL22F:;}goto _LL18C;_LL1AF: if(*((int*)_tmp1EC)!= 15)goto
_LL1B1;_tmp211=((struct Cyc_Absyn_New_e_struct*)_tmp1EC)->f1;_tmp212=((struct Cyc_Absyn_New_e_struct*)
_tmp1EC)->f2;_LL1B0: if(nv->toplevel)({struct Cyc_Std_String_pa_struct _tmp29D;
_tmp29D.tag=0;_tmp29D.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp212->loc);{void*_tmp29C[1]={& _tmp29D};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("%s: new at top-level",sizeof(
unsigned char),21),_tag_arr(_tmp29C,sizeof(void*),1));}});{void*_tmp29E=(void*)
_tmp212->r;struct Cyc_List_List*_tmp29F;struct Cyc_Absyn_Vardecl*_tmp2A0;struct Cyc_Absyn_Exp*
_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A2;struct _tuple1*_tmp2A3;struct Cyc_List_List*
_tmp2A4;struct Cyc_List_List*_tmp2A5;struct Cyc_List_List*_tmp2A6;_LL237: if(*((int*)
_tmp29E)!= 26)goto _LL239;_tmp29F=((struct Cyc_Absyn_Array_e_struct*)_tmp29E)->f1;
_LL238: {struct _tuple1*_tmp2A7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2A8=
Cyc_Absyn_var_exp(_tmp2A7,0);struct Cyc_Absyn_Stmt*_tmp2A9=Cyc_Toc_init_array(nv,
_tmp2A8,_tmp29F,Cyc_Absyn_exp_stmt(_tmp2A8,0));void*old_elt_typ;{void*_tmp2AA=
Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp2AB;void*_tmp2AC;struct
Cyc_Absyn_Tqual _tmp2AD;_LL242: if(_tmp2AA <= (void*)3?1:*((int*)_tmp2AA)!= 4)goto
_LL244;_tmp2AB=((struct Cyc_Absyn_PointerType_struct*)_tmp2AA)->f1;_tmp2AC=(void*)
_tmp2AB.elt_typ;_tmp2AD=_tmp2AB.tq;_LL243: old_elt_typ=_tmp2AC;goto _LL241;_LL244:;
_LL245: old_elt_typ=({void*_tmp2AE[0]={};Cyc_Toc_toc_impos(_tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof(unsigned char),52),_tag_arr(_tmp2AE,sizeof(void*),0));});_LL241:;}{void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp2AF=Cyc_Absyn_cstar_typ(elt_typ,
Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp2B0=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp29F),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp211 == 0)e1=Cyc_Toc_malloc_exp(
old_elt_typ,_tmp2B0);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(_tmp211);Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp2B0);}(
void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2A7,
_tmp2AF,(struct Cyc_Absyn_Exp*)e1,_tmp2A9,0),0))->r));goto _LL236;}}_LL239: if(*((
int*)_tmp29E)!= 27)goto _LL23B;_tmp2A0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp29E)->f1;_tmp2A1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp29E)->f2;
_tmp2A2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp29E)->f3;_LL23A: {int
is_tagged_ptr=0;{void*_tmp2B1=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp2B2;void*_tmp2B3;struct Cyc_Absyn_Tqual _tmp2B4;struct Cyc_Absyn_Conref*_tmp2B5;
_LL247: if(_tmp2B1 <= (void*)3?1:*((int*)_tmp2B1)!= 4)goto _LL249;_tmp2B2=((struct
Cyc_Absyn_PointerType_struct*)_tmp2B1)->f1;_tmp2B3=(void*)_tmp2B2.elt_typ;
_tmp2B4=_tmp2B2.tq;_tmp2B5=_tmp2B2.bounds;_LL248: is_tagged_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp2B5)== (void*)0;goto _LL246;_LL249:;_LL24A:({void*
_tmp2B6[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: comprehension not an array type",sizeof(unsigned char),42),
_tag_arr(_tmp2B6,sizeof(void*),0));});_LL246:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2A2->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp2A1);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),
_tmp2A0,Cyc_Absyn_var_exp(max,0),_tmp2A2,Cyc_Toc_skip_stmt_dl(),1);{struct
_RegionHandle _tmp2B7=_new_region("r");struct _RegionHandle*r=& _tmp2B7;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp2C7=
_region_malloc(r,sizeof(*_tmp2C7));_tmp2C7->hd=({struct _tuple9*_tmp2C8=
_region_malloc(r,sizeof(*_tmp2C8));_tmp2C8->f1=max;_tmp2C8->f2=Cyc_Absyn_uint_t;
_tmp2C8->f3=(struct Cyc_Absyn_Exp*)_tmp2A1;_tmp2C8;});_tmp2C7->tl=0;_tmp2C7;});
struct Cyc_Absyn_Exp*ai;if(_tmp211 == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp2B8[2];_tmp2B8[1]=Cyc_Absyn_var_exp(
max,0);_tmp2B8[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2B8,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(_tmp211);
Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp2B9[2];_tmp2B9[1]=Cyc_Absyn_var_exp(max,0);_tmp2B9[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp2B9,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct
Cyc_Absyn_Exp*ainit=Cyc_Absyn_cast_exp(ptr_typ,ai,0);decls=({struct Cyc_List_List*
_tmp2BA=_region_malloc(r,sizeof(*_tmp2BA));_tmp2BA->hd=({struct _tuple9*_tmp2BB=
_region_malloc(r,sizeof(*_tmp2BB));_tmp2BB->f1=a;_tmp2BB->f2=ptr_typ;_tmp2BB->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp2BB;});_tmp2BA->tl=decls;_tmp2BA;});if(
is_tagged_ptr){struct _tuple1*_tmp2BC=Cyc_Toc_temp_var();void*_tmp2BD=Cyc_Toc_typ_to_c(
old_typ);struct Cyc_Absyn_Exp*_tmp2BE=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2C1[3];_tmp2C1[2]=_tmp2A1;_tmp2C1[1]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);_tmp2C1[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp2C1,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);decls=({struct Cyc_List_List*_tmp2BF=_region_malloc(r,sizeof(*_tmp2BF));
_tmp2BF->hd=({struct _tuple9*_tmp2C0=_region_malloc(r,sizeof(*_tmp2C0));_tmp2C0->f1=
_tmp2BC;_tmp2C0->f2=_tmp2BD;_tmp2C0->f3=(struct Cyc_Absyn_Exp*)_tmp2BE;_tmp2C0;});
_tmp2BF->tl=decls;_tmp2BF;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
_tmp2BC,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
a,0),0),0);}{struct Cyc_List_List*_tmp2C2=decls;for(0;_tmp2C2 != 0;_tmp2C2=_tmp2C2->tl){
struct _tuple1*_tmp2C4;void*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C6;struct _tuple9
_tmp2C3=*((struct _tuple9*)_tmp2C2->hd);_tmp2C4=_tmp2C3.f1;_tmp2C5=_tmp2C3.f2;
_tmp2C6=_tmp2C3.f3;s=Cyc_Absyn_declare_stmt(_tmp2C4,_tmp2C5,_tmp2C6,s,0);}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL236;}}}
_LL23B: if(*((int*)_tmp29E)!= 28)goto _LL23D;_tmp2A3=((struct Cyc_Absyn_Struct_e_struct*)
_tmp29E)->f1;_tmp2A4=((struct Cyc_Absyn_Struct_e_struct*)_tmp29E)->f2;_tmp2A5=((
struct Cyc_Absyn_Struct_e_struct*)_tmp29E)->f3;_LL23C:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp212->topt))->v,
_tmp2A4 != 0,1,_tmp211,_tmp2A5,_tmp2A3))->r));goto _LL236;_LL23D: if(*((int*)
_tmp29E)!= 24)goto _LL23F;_tmp2A6=((struct Cyc_Absyn_Tuple_e_struct*)_tmp29E)->f1;
_LL23E:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp211,_tmp2A6))->r));
goto _LL236;_LL23F:;_LL240: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp212->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp2C9=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2CA=Cyc_Absyn_var_exp(
_tmp2C9,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp2CA,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp211 == 0)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(
_tmp211);Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp2CB=(void*)_tmp212->r;void*_tmp2CC;struct Cyc_Absyn_Exp*_tmp2CD;_LL24C:
if(*((int*)_tmp2CB)!= 13)goto _LL24E;_tmp2CC=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp2CB)->f1;_tmp2CD=((struct Cyc_Absyn_Cast_e_struct*)_tmp2CB)->f2;_LL24D:{
struct _tuple0 _tmp2CF=({struct _tuple0 _tmp2CE;_tmp2CE.f1=Cyc_Tcutil_compress(
_tmp2CC);_tmp2CE.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2CD->topt))->v);_tmp2CE;});void*_tmp2D0;struct Cyc_Absyn_PtrInfo _tmp2D1;void*
_tmp2D2;struct Cyc_Absyn_Conref*_tmp2D3;void*_tmp2D4;struct Cyc_Absyn_PtrInfo
_tmp2D5;struct Cyc_Absyn_Conref*_tmp2D6;_LL251: _tmp2D0=_tmp2CF.f1;if(_tmp2D0 <= (
void*)3?1:*((int*)_tmp2D0)!= 4)goto _LL253;_tmp2D1=((struct Cyc_Absyn_PointerType_struct*)
_tmp2D0)->f1;_tmp2D2=(void*)_tmp2D1.elt_typ;_tmp2D3=_tmp2D1.bounds;_tmp2D4=
_tmp2CF.f2;if(_tmp2D4 <= (void*)3?1:*((int*)_tmp2D4)!= 4)goto _LL253;_tmp2D5=((
struct Cyc_Absyn_PointerType_struct*)_tmp2D4)->f1;_tmp2D6=_tmp2D5.bounds;_LL252:{
struct _tuple0 _tmp2D8=({struct _tuple0 _tmp2D7;_tmp2D7.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp2D3);_tmp2D7.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp2D6);_tmp2D7;});
void*_tmp2D9;void*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;_LL256: _tmp2D9=_tmp2D8.f1;
if((int)_tmp2D9 != 0)goto _LL258;_tmp2DA=_tmp2D8.f2;if(_tmp2DA <= (void*)1?1:*((int*)
_tmp2DA)!= 0)goto _LL258;_tmp2DB=((struct Cyc_Absyn_Upper_b_struct*)_tmp2DA)->f1;
_LL257: Cyc_Toc_exp_to_c(nv,_tmp2CD);(void*)(inner_mexp->r=(void*)((void*)(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0))->r));done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2DC[4];_tmp2DC[3]=_tmp2DB;_tmp2DC[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp2D2),0);_tmp2DC[1]=_tmp2CD;_tmp2DC[0]=mexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2DC,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))->r));goto _LL255;_LL258:;_LL259: goto _LL255;_LL255:;}goto _LL250;_LL253:;
_LL254: goto _LL250;_LL250:;}goto _LL24B;_LL24E:;_LL24F: goto _LL24B;_LL24B:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp2DD=Cyc_Absyn_exp_stmt(_tmp2CA,0);struct Cyc_Absyn_Exp*
_tmp2DE=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp212);_tmp2DD=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp2CA,_tmp2DE,0),_tmp212,0),
_tmp2DD,0);{void*_tmp2DF=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2C9,_tmp2DF,(struct
Cyc_Absyn_Exp*)mexp,_tmp2DD,0),0))->r));}}goto _LL236;}}_LL236:;}goto _LL18C;
_LL1B1: if(*((int*)_tmp1EC)!= 17)goto _LL1B3;_tmp213=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp1EC)->f1;_LL1B2: Cyc_Toc_exp_to_c(nv,_tmp213);goto _LL18C;_LL1B3: if(*((int*)
_tmp1EC)!= 16)goto _LL1B5;_tmp214=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp1EC)->f1;_LL1B4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp2E0=_cycalloc(sizeof(*_tmp2E0));_tmp2E0[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp2E1;_tmp2E1.tag=16;_tmp2E1.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp214);_tmp2E1;});
_tmp2E0;})));goto _LL18C;_LL1B5: if(*((int*)_tmp1EC)!= 19)goto _LL1B7;_LL1B6:({void*
_tmp2E2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("__gen() in code generator",sizeof(unsigned char),26),_tag_arr(_tmp2E2,
sizeof(void*),0));});_LL1B7: if(*((int*)_tmp1EC)!= 18)goto _LL1B9;_tmp215=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp1EC)->f1;_tmp216=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1EC)->f2;if(*((int*)_tmp216)!= 0)goto _LL1B9;_tmp217=((struct Cyc_Absyn_StructField_struct*)
_tmp216)->f1;_LL1B8:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp2E4;_tmp2E4.tag=18;_tmp2E4.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp215);_tmp2E4.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2E5=_cycalloc(sizeof(*
_tmp2E5));_tmp2E5[0]=({struct Cyc_Absyn_StructField_struct _tmp2E6;_tmp2E6.tag=0;
_tmp2E6.f1=_tmp217;_tmp2E6;});_tmp2E5;}));_tmp2E4;});_tmp2E3;})));goto _LL18C;
_LL1B9: if(*((int*)_tmp1EC)!= 18)goto _LL1BB;_tmp218=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1EC)->f1;_tmp219=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1EC)->f2;
if(*((int*)_tmp219)!= 1)goto _LL1BB;_tmp21A=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp219)->f1;_LL1BA:{void*_tmp2E7=Cyc_Tcutil_compress(_tmp218);struct Cyc_Absyn_AggrInfo
_tmp2E8;void*_tmp2E9;struct Cyc_List_List*_tmp2EA;_LL25B: if(_tmp2E7 <= (void*)3?1:*((
int*)_tmp2E7)!= 10)goto _LL25D;_tmp2E8=((struct Cyc_Absyn_AggrType_struct*)_tmp2E7)->f1;
_tmp2E9=(void*)_tmp2E8.aggr_info;_LL25C: {struct Cyc_Absyn_Aggrdecl*_tmp2EB=Cyc_Absyn_get_known_aggrdecl(
_tmp2E9);if(_tmp2EB->fields == 0)({void*_tmp2EC[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("struct fields must be known",
sizeof(unsigned char),28),_tag_arr(_tmp2EC,sizeof(void*),0));});_tmp2EA=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2EB->fields))->v;goto _LL25E;}
_LL25D: if(_tmp2E7 <= (void*)3?1:*((int*)_tmp2E7)!= 11)goto _LL25F;_tmp2EA=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp2E7)->f2;_LL25E: {struct Cyc_Absyn_Aggrfield*
_tmp2ED=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmp2EA,(int)_tmp21A);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp2EF;_tmp2EF.tag=18;_tmp2EF.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp218);_tmp2EF.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2F0=_cycalloc(sizeof(*
_tmp2F0));_tmp2F0[0]=({struct Cyc_Absyn_StructField_struct _tmp2F1;_tmp2F1.tag=0;
_tmp2F1.f1=_tmp2ED->name;_tmp2F1;});_tmp2F0;}));_tmp2EF;});_tmp2EE;})));goto
_LL25A;}_LL25F: if(_tmp2E7 <= (void*)3?1:*((int*)_tmp2E7)!= 9)goto _LL261;_LL260:(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2F2=_cycalloc(
sizeof(*_tmp2F2));_tmp2F2[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2F3;_tmp2F3.tag=
18;_tmp2F3.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp218);_tmp2F3.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4[0]=({
struct Cyc_Absyn_StructField_struct _tmp2F5;_tmp2F5.tag=0;_tmp2F5.f1=Cyc_Absyn_fieldname((
int)(_tmp21A + 1));_tmp2F5;});_tmp2F4;}));_tmp2F3;});_tmp2F2;})));goto _LL25A;
_LL261: if(_tmp2E7 <= (void*)3?1:*((int*)_tmp2E7)!= 3)goto _LL263;_LL262: if(_tmp21A
== 0)(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2F6=
_cycalloc(sizeof(*_tmp2F6));_tmp2F6[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp2F7;_tmp2F7.tag=18;_tmp2F7.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp218);_tmp2F7.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2F8=_cycalloc(sizeof(*
_tmp2F8));_tmp2F8[0]=({struct Cyc_Absyn_StructField_struct _tmp2F9;_tmp2F9.tag=0;
_tmp2F9.f1=Cyc_Toc_tag_sp;_tmp2F9;});_tmp2F8;}));_tmp2F7;});_tmp2F6;})));else{(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2FA=_cycalloc(
sizeof(*_tmp2FA));_tmp2FA[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2FB;_tmp2FB.tag=
18;_tmp2FB.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp218);_tmp2FB.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC[0]=({
struct Cyc_Absyn_StructField_struct _tmp2FD;_tmp2FD.tag=0;_tmp2FD.f1=Cyc_Absyn_fieldname((
int)_tmp21A);_tmp2FD;});_tmp2FC;}));_tmp2FB;});_tmp2FA;})));}goto _LL25A;_LL263:;
_LL264:({void*_tmp2FE[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("impossible type for offsetof tuple index",sizeof(
unsigned char),41),_tag_arr(_tmp2FE,sizeof(void*),0));});_LL25A:;}goto _LL18C;
_LL1BB: if(*((int*)_tmp1EC)!= 20)goto _LL1BD;_tmp21B=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1EC)->f1;_LL1BC: {void*_tmp2FF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp21B->topt))->v);{void*_tmp300=_tmp2FF;struct Cyc_Absyn_PtrInfo
_tmp301;void*_tmp302;void*_tmp303;struct Cyc_Absyn_Conref*_tmp304;struct Cyc_Absyn_Tqual
_tmp305;struct Cyc_Absyn_Conref*_tmp306;_LL266: if(_tmp300 <= (void*)3?1:*((int*)
_tmp300)!= 4)goto _LL268;_tmp301=((struct Cyc_Absyn_PointerType_struct*)_tmp300)->f1;
_tmp302=(void*)_tmp301.elt_typ;_tmp303=(void*)_tmp301.rgn_typ;_tmp304=_tmp301.nullable;
_tmp305=_tmp301.tq;_tmp306=_tmp301.bounds;_LL267:{void*_tmp307=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp306);_LL26B: if(_tmp307 <= (void*)1?1:*((int*)_tmp307)!= 
0)goto _LL26D;_LL26C: {int do_null_check=Cyc_Toc_need_null_check(_tmp21B);Cyc_Toc_exp_to_c(
nv,_tmp21B);if(do_null_check)(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp21B->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp308=_cycalloc(sizeof(*_tmp308));_tmp308->hd=_tmp21B;_tmp308->tl=0;_tmp308;}),
0),0),0))->r));goto _LL26A;}_LL26D: if((int)_tmp307 != 0)goto _LL26A;_LL26E: {struct
Cyc_Absyn_Exp*_tmp309=Cyc_Absyn_uint_exp(0,0);_tmp309->topt=({struct Cyc_Core_Opt*
_tmp30A=_cycalloc(sizeof(*_tmp30A));_tmp30A->v=(void*)Cyc_Absyn_uint_t;_tmp30A;});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp21B,_tmp309,0))->r));Cyc_Toc_exp_to_c(
nv,e);goto _LL26A;}_LL26A:;}goto _LL265;_LL268:;_LL269:({void*_tmp30B[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: Deref: non-pointer",
sizeof(unsigned char),29),_tag_arr(_tmp30B,sizeof(void*),0));});_LL265:;}goto
_LL18C;}_LL1BD: if(*((int*)_tmp1EC)!= 21)goto _LL1BF;_tmp21C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1EC)->f1;_tmp21D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1EC)->f2;_LL1BE:
Cyc_Toc_exp_to_c(nv,_tmp21C);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL18C;
_LL1BF: if(*((int*)_tmp1EC)!= 22)goto _LL1C1;_tmp21E=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1EC)->f1;_tmp21F=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1EC)->f2;_LL1C0: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp21E->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp21E);Cyc_Toc_exp_to_c(nv,_tmp21E);{
int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp30D;void*_tmp30E;struct Cyc_Absyn_Conref*
_tmp30F;struct Cyc_Absyn_Tqual _tmp310;struct Cyc_Absyn_Conref*_tmp311;struct Cyc_Absyn_PtrInfo
_tmp30C=Cyc_Toc_get_ptr_type(e1typ);_tmp30D=(void*)_tmp30C.elt_typ;_tmp30E=(void*)
_tmp30C.rgn_typ;_tmp30F=_tmp30C.nullable;_tmp310=_tmp30C.tq;_tmp311=_tmp30C.bounds;{
void*_tmp312=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp311);struct Cyc_Absyn_Exp*
_tmp313;_LL270: if(_tmp312 <= (void*)1?1:*((int*)_tmp312)!= 0)goto _LL272;_tmp313=((
struct Cyc_Absyn_Upper_b_struct*)_tmp312)->f1;_LL271: if(Cyc_Evexp_eval_const_uint_exp(
_tmp313)< 1)({void*_tmp314[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(_tag_arr("exp_to_c:  AggrArrow_e on pointer of size 0",
sizeof(unsigned char),44),_tag_arr(_tmp314,sizeof(void*),0));});if(do_null_check)(
void*)(e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp21E->topt))->v),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp315=_cycalloc(sizeof(*_tmp315));
_tmp315->hd=_tmp21E;_tmp315->tl=0;_tmp315;}),0),0),_tmp21F,0))->r));goto _LL26F;
_LL272: if((int)_tmp312 != 0)goto _LL26F;_LL273: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp30D);(void*)(_tmp21E->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
ta1,_tmp310),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*
_tmp316[3];_tmp316[2]=Cyc_Absyn_uint_exp(0,0);_tmp316[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp316[0]=Cyc_Absyn_copy_exp(_tmp21E);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp316,sizeof(struct Cyc_Absyn_Exp*),3));}),
0),0))->r));goto _LL26F;}_LL26F:;}if(is_poly)(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((
void*)e->r,0),0))->r));goto _LL18C;}}_LL1C1: if(*((int*)_tmp1EC)!= 23)goto _LL1C3;
_tmp220=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1EC)->f1;_tmp221=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1EC)->f2;_LL1C2: {void*_tmp317=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp220->topt))->v);{void*_tmp318=_tmp317;struct Cyc_List_List*
_tmp319;struct Cyc_Absyn_PtrInfo _tmp31A;void*_tmp31B;void*_tmp31C;struct Cyc_Absyn_Conref*
_tmp31D;struct Cyc_Absyn_Tqual _tmp31E;struct Cyc_Absyn_Conref*_tmp31F;_LL275: if(
_tmp318 <= (void*)3?1:*((int*)_tmp318)!= 9)goto _LL277;_tmp319=((struct Cyc_Absyn_TupleType_struct*)
_tmp318)->f1;_LL276: Cyc_Toc_exp_to_c(nv,_tmp220);Cyc_Toc_exp_to_c(nv,_tmp221);{
unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmp221)+ 1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(_tmp220,Cyc_Absyn_fieldname((int)i),0))->r));
goto _LL274;}_LL277: if(_tmp318 <= (void*)3?1:*((int*)_tmp318)!= 4)goto _LL279;
_tmp31A=((struct Cyc_Absyn_PointerType_struct*)_tmp318)->f1;_tmp31B=(void*)
_tmp31A.elt_typ;_tmp31C=(void*)_tmp31A.rgn_typ;_tmp31D=_tmp31A.nullable;_tmp31E=
_tmp31A.tq;_tmp31F=_tmp31A.bounds;_LL278: {struct Cyc_List_List*_tmp320=Cyc_Toc_get_relns(
_tmp220);int _tmp321=Cyc_Toc_check_bounds(_tmp320,_tmp220,_tmp221);if(Cyc_Toc_warn_bounds_checks?
!_tmp321: 0)({struct Cyc_Std_String_pa_struct _tmp323;_tmp323.tag=0;_tmp323.f1=(
struct _tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp322[1]={& _tmp323};Cyc_Tcutil_warn(
e->loc,_tag_arr("bounds check necessary for %s",sizeof(unsigned char),30),
_tag_arr(_tmp322,sizeof(void*),1));}});Cyc_Toc_exp_to_c(nv,_tmp220);Cyc_Toc_exp_to_c(
nv,_tmp221);Cyc_Toc_total_bounds_checks ++;{void*_tmp324=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp31F);struct Cyc_Absyn_Exp*_tmp325;_LL27C: if(_tmp324 <= (void*)1?1:*((int*)
_tmp324)!= 0)goto _LL27E;_tmp325=((struct Cyc_Absyn_Upper_b_struct*)_tmp324)->f1;
_LL27D: _tmp325=Cyc_Absyn_uint_exp(Cyc_Evexp_eval_const_uint_exp(_tmp325),0);{int
possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp31D);void*ta1=Cyc_Toc_typ_to_c(_tmp31B);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp31E);if(_tmp321)Cyc_Toc_bounds_checks_eliminated ++;else{if(possibly_null)(
void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp326[4];_tmp326[3]=
_tmp221;_tmp326[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp326[1]=_tmp325;_tmp326[0]=
_tmp220;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp326,sizeof(struct Cyc_Absyn_Exp*),4));}),0),0),0))->r));else{(void*)(_tmp221->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp327[2];_tmp327[1]=Cyc_Absyn_copy_exp(_tmp221);_tmp327[0]=
_tmp325;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp327,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}goto _LL27B;}_LL27E: if((
int)_tmp324 != 0)goto _LL27B;_LL27F: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp31B);if(
_tmp321){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(
Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp31E),Cyc_Absyn_aggrmember_exp(
_tmp220,Cyc_Toc_curr_sp,0),0),_tmp221,0))->r));}else{(void*)(e->r=(void*)((void*)(
Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp31E),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp328[3];_tmp328[2]=
_tmp221;_tmp328[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp328[0]=_tmp220;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp328,sizeof(struct Cyc_Absyn_Exp*),
3));}),0),0),0))->r));}goto _LL27B;}_LL27B:;}goto _LL274;}_LL279:;_LL27A:({void*
_tmp329[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",sizeof(unsigned char),
49),_tag_arr(_tmp329,sizeof(void*),0));});_LL274:;}goto _LL18C;}_LL1C3: if(*((int*)
_tmp1EC)!= 24)goto _LL1C5;_tmp222=((struct Cyc_Absyn_Tuple_e_struct*)_tmp1EC)->f1;
_LL1C4: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,
_tmp222))->r));else{struct Cyc_List_List*_tmp32A=((struct Cyc_List_List*(*)(struct
_tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,
_tmp222);struct _tagged_arr*_tmp32B=Cyc_Toc_add_tuple_type(_tmp32A);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp222 != 0;(_tmp222=_tmp222->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp222->hd);dles=({struct Cyc_List_List*_tmp32C=_cycalloc(
sizeof(*_tmp32C));_tmp32C->hd=({struct _tuple5*_tmp32D=_cycalloc(sizeof(*_tmp32D));
_tmp32D->f1=0;_tmp32D->f2=(struct Cyc_Absyn_Exp*)_tmp222->hd;_tmp32D;});_tmp32C->tl=
dles;_tmp32C;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));}
goto _LL18C;_LL1C5: if(*((int*)_tmp1EC)!= 26)goto _LL1C7;_tmp223=((struct Cyc_Absyn_Array_e_struct*)
_tmp1EC)->f1;_LL1C6:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp223,0))->r));{struct Cyc_List_List*_tmp32E=_tmp223;for(0;_tmp32E != 0;_tmp32E=
_tmp32E->tl){struct _tuple5 _tmp330;struct Cyc_Absyn_Exp*_tmp331;struct _tuple5*
_tmp32F=(struct _tuple5*)_tmp32E->hd;_tmp330=*_tmp32F;_tmp331=_tmp330.f2;Cyc_Toc_exp_to_c(
nv,_tmp331);}}goto _LL18C;_LL1C7: if(*((int*)_tmp1EC)!= 27)goto _LL1C9;_tmp224=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EC)->f1;_tmp225=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1EC)->f2;_tmp226=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1EC)->f3;
_LL1C8: {unsigned int _tmp332=Cyc_Evexp_eval_const_uint_exp(_tmp225);Cyc_Toc_exp_to_c(
nv,_tmp226);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp226)){unsigned int
i=0;for(0;i < _tmp332;i ++){es=({struct Cyc_List_List*_tmp333=_cycalloc(sizeof(*
_tmp333));_tmp333->hd=({struct _tuple5*_tmp334=_cycalloc(sizeof(*_tmp334));
_tmp334->f1=0;_tmp334->f2=_tmp226;_tmp334;});_tmp333->tl=es;_tmp333;});}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));goto _LL18C;}}
_LL1C9: if(*((int*)_tmp1EC)!= 28)goto _LL1CB;_tmp227=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1EC)->f1;_tmp228=((struct Cyc_Absyn_Struct_e_struct*)_tmp1EC)->f2;_tmp229=((
struct Cyc_Absyn_Struct_e_struct*)_tmp1EC)->f3;_tmp22A=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1EC)->f4;_LL1CA: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp228 != 0,0,0,_tmp229,_tmp227))->r));else{if(_tmp22A == 0)({void*
_tmp335[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing aggrdecl pointer",sizeof(unsigned char),35),_tag_arr(
_tmp335,sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)
_check_null(_tmp22A);struct _RegionHandle _tmp336=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp336;_push_region(rgn);{struct Cyc_List_List*_tmp337=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,e->loc,_tmp229,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sd2->fields))->v);
struct Cyc_List_List*_tmp338=0;{struct Cyc_List_List*_tmp339=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(sd2->fields))->v;for(0;_tmp339 != 0;_tmp339=
_tmp339->tl){struct Cyc_List_List*_tmp33A=_tmp337;for(0;_tmp33A != 0;_tmp33A=
_tmp33A->tl){if((*((struct _tuple10*)_tmp33A->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)
_tmp339->hd){struct _tuple10 _tmp33C;struct Cyc_Absyn_Aggrfield*_tmp33D;struct Cyc_Absyn_Exp*
_tmp33E;struct _tuple10*_tmp33B=(struct _tuple10*)_tmp33A->hd;_tmp33C=*_tmp33B;
_tmp33D=_tmp33C.f1;_tmp33E=_tmp33C.f2;{void*_tmp33F=(void*)_tmp33D->type;Cyc_Toc_exp_to_c(
nv,_tmp33E);if(Cyc_Toc_is_void_star(_tmp33F))(void*)(_tmp33E->r=(void*)((void*)(
Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp33E->r,
0),0))->r));_tmp338=({struct Cyc_List_List*_tmp340=_cycalloc(sizeof(*_tmp340));
_tmp340->hd=({struct _tuple5*_tmp341=_cycalloc(sizeof(*_tmp341));_tmp341->f1=0;
_tmp341->f2=_tmp33E;_tmp341;});_tmp340->tl=_tmp338;_tmp340;});break;}}}}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp338),0))->r));};_pop_region(rgn);}}
goto _LL18C;_LL1CB: if(*((int*)_tmp1EC)!= 29)goto _LL1CD;_tmp22B=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1EC)->f1;_tmp22C=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1EC)->f2;_LL1CC: {
struct Cyc_List_List*fs;{void*_tmp342=Cyc_Tcutil_compress(_tmp22B);struct Cyc_List_List*
_tmp343;_LL281: if(_tmp342 <= (void*)3?1:*((int*)_tmp342)!= 11)goto _LL283;_tmp343=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp342)->f2;_LL282: fs=_tmp343;goto _LL280;
_LL283:;_LL284:({struct Cyc_Std_String_pa_struct _tmp345;_tmp345.tag=0;_tmp345.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp22B);{void*_tmp344[1]={& _tmp345};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("anon struct has type %s",
sizeof(unsigned char),24),_tag_arr(_tmp344,sizeof(void*),1));}});_LL280:;}{
struct _RegionHandle _tmp346=_new_region("rgn");struct _RegionHandle*rgn=& _tmp346;
_push_region(rgn);{struct Cyc_List_List*_tmp347=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp22C,
fs);for(0;_tmp347 != 0;_tmp347=_tmp347->tl){struct _tuple10 _tmp349;struct Cyc_Absyn_Aggrfield*
_tmp34A;struct Cyc_Absyn_Exp*_tmp34B;struct _tuple10*_tmp348=(struct _tuple10*)
_tmp347->hd;_tmp349=*_tmp348;_tmp34A=_tmp349.f1;_tmp34B=_tmp349.f2;{void*_tmp34C=(
void*)_tmp34A->type;Cyc_Toc_exp_to_c(nv,_tmp34B);if(Cyc_Toc_is_void_star(_tmp34C))(
void*)(_tmp34B->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp34B->r,0),0))->r));}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,_tmp22C,0))->r));};_pop_region(rgn);}goto _LL18C;}
_LL1CD: if(*((int*)_tmp1EC)!= 30)goto _LL1CF;_tmp22D=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1EC)->f1;if(_tmp22D != 0)goto _LL1CF;_tmp22E=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1EC)->f2;_tmp22F=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1EC)->f3;_LL1CE: {
struct _tuple1*qv=_tmp22F->name;struct Cyc_Absyn_Exp*tag_exp=_tmp22E->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp22E,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL18C;}_LL1CF: if(*((int*)_tmp1EC)!= 30)goto _LL1D1;
_tmp230=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1EC)->f1;_tmp231=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1EC)->f2;_tmp232=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1EC)->f3;_LL1D0: {
struct _tuple1*_tmp34D=_tmp232->name;struct Cyc_List_List*_tmp34E=_tmp232->typs;
struct _tuple1*_tmp34F=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp350=Cyc_Absyn_var_exp(
_tmp34F,0);void*_tmp351=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp34D,
_tag_arr("_struct",sizeof(unsigned char),8)));struct Cyc_Absyn_Exp*tag_exp=
_tmp231->is_xtunion?Cyc_Absyn_var_exp(_tmp34D,0): Cyc_Toc_tunion_tag(_tmp231,
_tmp34D,1);if(nv->toplevel){struct Cyc_List_List*dles=0;for(0;_tmp230 != 0;(
_tmp230=_tmp230->tl,_tmp34E=_tmp34E->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp230->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp34E))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);dles=({struct Cyc_List_List*
_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352->hd=({struct _tuple5*_tmp353=
_cycalloc(sizeof(*_tmp353));_tmp353->f1=0;_tmp353->f2=cur_e;_tmp353;});_tmp352->tl=
dles;_tmp352;});}dles=({struct Cyc_List_List*_tmp354=_cycalloc(sizeof(*_tmp354));
_tmp354->hd=({struct _tuple5*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355->f1=0;
_tmp355->f2=tag_exp;_tmp355;});_tmp354->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp354;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356->v=Cyc_Toc_collapse_qvar_tag(
_tmp34D,_tag_arr("_struct",sizeof(unsigned char),8));_tmp356;}),dles,0))->r));}
else{struct Cyc_List_List*_tmp357=({struct Cyc_List_List*_tmp35D=_cycalloc(sizeof(*
_tmp35D));_tmp35D->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp350,Cyc_Toc_tag_sp,
0),tag_exp,0);_tmp35D->tl=0;_tmp35D;});{int i=1;for(0;_tmp230 != 0;(((_tmp230=
_tmp230->tl,i ++)),_tmp34E=_tmp34E->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp230->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp34E))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);{struct Cyc_Absyn_Stmt*
_tmp358=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp350,Cyc_Absyn_fieldname(
i),0),cur_e,0);_tmp357=({struct Cyc_List_List*_tmp359=_cycalloc(sizeof(*_tmp359));
_tmp359->hd=_tmp358;_tmp359->tl=_tmp357;_tmp359;});}}}{struct Cyc_Absyn_Stmt*
_tmp35A=Cyc_Absyn_exp_stmt(_tmp350,0);struct Cyc_Absyn_Stmt*_tmp35B=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C->hd=_tmp35A;_tmp35C->tl=_tmp357;
_tmp35C;})),0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp34F,_tmp351,0,_tmp35B,0),0))->r));}}goto _LL18C;}_LL1D1: if(*((int*)_tmp1EC)!= 
31)goto _LL1D3;_LL1D2: goto _LL1D4;_LL1D3: if(*((int*)_tmp1EC)!= 32)goto _LL1D5;
_LL1D4: goto _LL18C;_LL1D5: if(*((int*)_tmp1EC)!= 33)goto _LL1D7;_tmp233=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp1EC)->f1;_tmp234=_tmp233.is_calloc;_tmp235=_tmp233.rgn;_tmp236=_tmp233.elt_type;
_tmp237=_tmp233.num_elts;_tmp238=_tmp233.fat_result;_LL1D6: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp236)));Cyc_Toc_exp_to_c(nv,_tmp237);if(_tmp238){struct
_tuple1*_tmp35E=Cyc_Toc_temp_var();struct _tuple1*_tmp35F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp234){xexp=_tmp237;if(_tmp235 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp235);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp35E,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp236,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp35E,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp360[3];_tmp360[2]=Cyc_Absyn_var_exp(_tmp35E,0);_tmp360[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp360[0]=Cyc_Absyn_var_exp(_tmp35F,0);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp360,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp237,0);if(_tmp235 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp235);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp35E,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp236,Cyc_Absyn_var_exp(_tmp35E,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp361[3];
_tmp361[2]=Cyc_Absyn_var_exp(_tmp35E,0);_tmp361[1]=Cyc_Absyn_uint_exp(1,0);
_tmp361[0]=Cyc_Absyn_var_exp(_tmp35F,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp361,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}{struct Cyc_Absyn_Stmt*_tmp362=Cyc_Absyn_declare_stmt(_tmp35E,Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp35F,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp362,0))->r));}}else{if(_tmp235 != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp235);Cyc_Toc_exp_to_c(
nv,rgn);(void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp236,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL18C;}_LL1D7: if(*((int*)_tmp1EC)!= 35)goto _LL1D9;_tmp239=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp1EC)->f1;_LL1D8: Cyc_Toc_stmt_to_c(nv,
_tmp239);goto _LL18C;_LL1D9: if(*((int*)_tmp1EC)!= 34)goto _LL1DB;_LL1DA:({void*
_tmp363[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem",sizeof(unsigned char),14),_tag_arr(_tmp363,sizeof(void*),
0));});_LL1DB: if(*((int*)_tmp1EC)!= 25)goto _LL1DD;_LL1DC:({void*_tmp364[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("compoundlit",
sizeof(unsigned char),12),_tag_arr(_tmp364,sizeof(void*),0));});_LL1DD: if(*((int*)
_tmp1EC)!= 36)goto _LL1DF;_tmp23A=((struct Cyc_Absyn_Codegen_e_struct*)_tmp1EC)->f1;
_LL1DE:({void*_tmp365[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(_tag_arr("codegen",sizeof(unsigned char),8),_tag_arr(_tmp365,
sizeof(void*),0));});_LL1DF: if(*((int*)_tmp1EC)!= 37)goto _LL18C;_LL1E0:({void*
_tmp366[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("fill",sizeof(unsigned char),5),_tag_arr(_tmp366,sizeof(void*),0));});
_LL18C:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);}struct _tuple11{
struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple12{struct _tuple1*f1;void*f2;};struct _tuple13{struct Cyc_List_List*f1;struct
Cyc_Absyn_Pat*f2;};static struct _tuple11 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,
struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp367=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp368;
struct Cyc_Absyn_Vardecl _tmp369;struct _tuple1*_tmp36A;struct Cyc_Absyn_Vardecl*
_tmp36B;void*_tmp36C;int _tmp36D;unsigned char _tmp36E;struct _tagged_arr _tmp36F;
struct Cyc_Absyn_Enumdecl*_tmp370;struct Cyc_Absyn_Enumfield*_tmp371;void*_tmp372;
struct Cyc_Absyn_Enumfield*_tmp373;struct Cyc_Absyn_Tuniondecl*_tmp374;struct Cyc_Absyn_Tunionfield*
_tmp375;struct Cyc_List_List*_tmp376;struct Cyc_Absyn_Pat*_tmp377;struct Cyc_Absyn_Pat
_tmp378;void*_tmp379;struct Cyc_Absyn_Tuniondecl*_tmp37A;struct Cyc_Absyn_Tunionfield*
_tmp37B;struct Cyc_List_List*_tmp37C;struct Cyc_List_List*_tmp37D;struct Cyc_List_List*
_tmp37E;struct Cyc_Absyn_AggrInfo _tmp37F;void*_tmp380;struct Cyc_List_List*_tmp381;
struct Cyc_Absyn_Pat*_tmp382;_LL286: if(_tmp367 <= (void*)2?1:*((int*)_tmp367)!= 0)
goto _LL288;_tmp368=((struct Cyc_Absyn_Var_p_struct*)_tmp367)->f1;_tmp369=*_tmp368;
_tmp36A=_tmp369.name;_LL287: nv=Cyc_Toc_add_varmap(nv,_tmp36A,r);goto _LL289;
_LL288: if((int)_tmp367 != 0)goto _LL28A;_LL289: s=Cyc_Toc_skip_stmt_dl();goto _LL285;
_LL28A: if(_tmp367 <= (void*)2?1:*((int*)_tmp367)!= 6)goto _LL28C;_tmp36B=((struct
Cyc_Absyn_Reference_p_struct*)_tmp367)->f1;_LL28B: {struct _tuple1*_tmp383=Cyc_Toc_temp_var();
decls=({struct Cyc_List_List*_tmp384=_region_malloc(rgn,sizeof(*_tmp384));_tmp384->hd=({
struct _tuple12*_tmp385=_region_malloc(rgn,sizeof(*_tmp385));_tmp385->f1=_tmp383;
_tmp385->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp385;});
_tmp384->tl=decls;_tmp384;});nv=Cyc_Toc_add_varmap(nv,_tmp36B->name,Cyc_Absyn_var_exp(
_tmp383,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp383,0),Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(
path,0),0),0);goto _LL285;}_LL28C: if((int)_tmp367 != 1)goto _LL28E;_LL28D: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL285;_LL28E: if(_tmp367 <= (void*)
2?1:*((int*)_tmp367)!= 1)goto _LL290;_tmp36C=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp367)->f1;_tmp36D=((struct Cyc_Absyn_Int_p_struct*)_tmp367)->f2;_LL28F: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_int_exp(_tmp36C,_tmp36D,0),fail_stmt);goto _LL285;_LL290: if(_tmp367 <= (
void*)2?1:*((int*)_tmp367)!= 2)goto _LL292;_tmp36E=((struct Cyc_Absyn_Char_p_struct*)
_tmp367)->f1;_LL291: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp36E,0),
fail_stmt);goto _LL285;_LL292: if(_tmp367 <= (void*)2?1:*((int*)_tmp367)!= 3)goto
_LL294;_tmp36F=((struct Cyc_Absyn_Float_p_struct*)_tmp367)->f1;_LL293: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_float_exp(_tmp36F,0),fail_stmt);goto _LL285;_LL294: if(_tmp367 <= (void*)
2?1:*((int*)_tmp367)!= 9)goto _LL296;_tmp370=((struct Cyc_Absyn_Enum_p_struct*)
_tmp367)->f1;_tmp371=((struct Cyc_Absyn_Enum_p_struct*)_tmp367)->f2;_LL295: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp386=_cycalloc(
sizeof(*_tmp386));_tmp386[0]=({struct Cyc_Absyn_Enum_e_struct _tmp387;_tmp387.tag=
31;_tmp387.f1=_tmp371->name;_tmp387.f2=(struct Cyc_Absyn_Enumdecl*)_tmp370;
_tmp387.f3=(struct Cyc_Absyn_Enumfield*)_tmp371;_tmp387;});_tmp386;}),0),
fail_stmt);goto _LL285;_LL296: if(_tmp367 <= (void*)2?1:*((int*)_tmp367)!= 10)goto
_LL298;_tmp372=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp367)->f1;_tmp373=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp367)->f2;_LL297: s=Cyc_Toc_if_neq_stmt(r,
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp388=_cycalloc(
sizeof(*_tmp388));_tmp388[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp389;_tmp389.tag=
32;_tmp389.f1=_tmp373->name;_tmp389.f2=(void*)_tmp372;_tmp389.f3=(struct Cyc_Absyn_Enumfield*)
_tmp373;_tmp389;});_tmp388;}),0),fail_stmt);goto _LL285;_LL298: if(_tmp367 <= (void*)
2?1:*((int*)_tmp367)!= 8)goto _LL29A;_tmp374=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp367)->f1;_tmp375=((struct Cyc_Absyn_Tunion_p_struct*)_tmp367)->f2;_tmp376=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp367)->f3;if(_tmp376 != 0)goto _LL29A;_LL299: {
struct Cyc_Absyn_Exp*cmp_exp;if(_tmp374->is_xtunion)cmp_exp=Cyc_Absyn_var_exp(
_tmp375->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(_tmp374,_tmp375->name,0);r=Cyc_Absyn_cast_exp(
Cyc_Absyn_sint_t,r,0);}s=Cyc_Toc_if_neq_stmt(r,cmp_exp,fail_stmt);goto _LL285;}
_LL29A: if(_tmp367 <= (void*)2?1:*((int*)_tmp367)!= 5)goto _LL29C;_tmp377=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp367)->f1;_tmp378=*_tmp377;_tmp379=(void*)_tmp378.r;
if(_tmp379 <= (void*)2?1:*((int*)_tmp379)!= 8)goto _LL29C;_tmp37A=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp379)->f1;_tmp37B=((struct Cyc_Absyn_Tunion_p_struct*)_tmp379)->f2;_tmp37C=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp379)->f3;if(!(_tmp37C != 0))goto _LL29C;
_LL29B: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(
_tmp37B->name,_tag_arr("_struct",sizeof(unsigned char),8));struct Cyc_Absyn_Exp*
rcast=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),
r,0);struct Cyc_List_List*_tmp38A=_tmp37B->typs;for(0;_tmp37C != 0;(((_tmp37C=
_tmp37C->tl,_tmp38A=((struct Cyc_List_List*)_check_null(_tmp38A))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp38B=(struct Cyc_Absyn_Pat*)_tmp37C->hd;if((void*)_tmp38B->r
== (void*)0)continue;{void*_tmp38C=(*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp38A))->hd)).f2;struct _tuple1*_tmp38D=Cyc_Toc_temp_var();void*
_tmp38E=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp38B->topt))->v;void*_tmp38F=
Cyc_Toc_typ_to_c(_tmp38E);struct Cyc_Absyn_Exp*_tmp390=Cyc_Absyn_aggrarrow_exp(
rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(Cyc_Toc_typ_to_c(
_tmp38C)))_tmp390=Cyc_Absyn_cast_exp(_tmp38F,_tmp390,0);decls=({struct Cyc_List_List*
_tmp391=_region_malloc(rgn,sizeof(*_tmp391));_tmp391->hd=({struct _tuple12*
_tmp392=_region_malloc(rgn,sizeof(*_tmp392));_tmp392->f1=_tmp38D;_tmp392->f2=
_tmp38F;_tmp392;});_tmp391->tl=decls;_tmp391;});{struct _tuple11 _tmp393=Cyc_Toc_xlate_pat(
nv,rgn,_tmp38E,Cyc_Absyn_var_exp(_tmp38D,0),_tmp390,_tmp38B,fail_stmt,decls);nv=
_tmp393.f1;decls=_tmp393.f2;{struct Cyc_Absyn_Stmt*_tmp394=_tmp393.f3;struct Cyc_Absyn_Stmt*
_tmp395=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp38D,0),_tmp390,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp395,_tmp394,0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(
_tmp37A->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp37B->name,0);test_exp=Cyc_Absyn_neq_exp(
e1,e,0);}else{int max_tag=Cyc_Toc_num_void_tags(_tmp37A);struct Cyc_Absyn_Exp*
max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);struct Cyc_Absyn_Exp*e3=
Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_t,Cyc_Toc_mt_tq),r,0);
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*e5=Cyc_Absyn_lte_exp(
r,Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),max_tag_exp,0),0);struct Cyc_Absyn_Exp*
e=Cyc_Toc_tunion_tag(_tmp37A,_tmp37B->name,1);test_exp=max_tag > 0?Cyc_Absyn_or_exp(
e5,Cyc_Absyn_neq_exp(e1,e,0),0): Cyc_Absyn_neq_exp(e1,e,0);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);goto
_LL285;}}_LL29C: if(_tmp367 <= (void*)2?1:*((int*)_tmp367)!= 8)goto _LL29E;_tmp37D=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp367)->f3;_LL29D: _tmp37E=_tmp37D;goto _LL29F;
_LL29E: if(_tmp367 <= (void*)2?1:*((int*)_tmp367)!= 4)goto _LL2A0;_tmp37E=((struct
Cyc_Absyn_Tuple_p_struct*)_tmp367)->f1;_LL29F: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;
for(0;_tmp37E != 0;(_tmp37E=_tmp37E->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp396=(
struct Cyc_Absyn_Pat*)_tmp37E->hd;if((void*)_tmp396->r == (void*)0)continue;{
struct _tuple1*_tmp397=Cyc_Toc_temp_var();void*_tmp398=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp396->topt))->v;decls=({struct Cyc_List_List*_tmp399=
_region_malloc(rgn,sizeof(*_tmp399));_tmp399->hd=({struct _tuple12*_tmp39A=
_region_malloc(rgn,sizeof(*_tmp39A));_tmp39A->f1=_tmp397;_tmp39A->f2=Cyc_Toc_typ_to_c(
_tmp398);_tmp39A;});_tmp399->tl=decls;_tmp399;});{struct _tuple11 _tmp39B=Cyc_Toc_xlate_pat(
nv,rgn,_tmp398,Cyc_Absyn_var_exp(_tmp397,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp396,fail_stmt,decls);nv=_tmp39B.f1;decls=_tmp39B.f2;{struct Cyc_Absyn_Stmt*
_tmp39C=_tmp39B.f3;struct Cyc_Absyn_Stmt*_tmp39D=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp397,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp39D,_tmp39C,0),0);}}}}goto _LL285;}_LL2A0: if(_tmp367 <= (
void*)2?1:*((int*)_tmp367)!= 7)goto _LL2A2;_tmp37F=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp367)->f1;_tmp380=(void*)_tmp37F.aggr_info;_tmp381=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp367)->f3;_LL2A1: {struct Cyc_Absyn_Aggrdecl*_tmp39E=Cyc_Absyn_get_known_aggrdecl(
_tmp380);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp381 != 0;_tmp381=_tmp381->tl){struct
_tuple13*_tmp39F=(struct _tuple13*)_tmp381->hd;struct Cyc_Absyn_Pat*_tmp3A0=(*
_tmp39F).f2;if((void*)_tmp3A0->r == (void*)0)continue;{struct _tagged_arr*f;{void*
_tmp3A1=(void*)((struct Cyc_List_List*)_check_null((*_tmp39F).f1))->hd;struct
_tagged_arr*_tmp3A2;_LL2A9: if(*((int*)_tmp3A1)!= 1)goto _LL2AB;_tmp3A2=((struct
Cyc_Absyn_FieldName_struct*)_tmp3A1)->f1;_LL2AA: f=_tmp3A2;goto _LL2A8;_LL2AB:;
_LL2AC:(int)_throw((void*)Cyc_Toc_Match_error);_LL2A8:;}{struct _tuple1*_tmp3A3=
Cyc_Toc_temp_var();void*_tmp3A4=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A0->topt))->v;
void*_tmp3A5=Cyc_Toc_typ_to_c(_tmp3A4);decls=({struct Cyc_List_List*_tmp3A6=
_region_malloc(rgn,sizeof(*_tmp3A6));_tmp3A6->hd=({struct _tuple12*_tmp3A7=
_region_malloc(rgn,sizeof(*_tmp3A7));_tmp3A7->f1=_tmp3A3;_tmp3A7->f2=_tmp3A5;
_tmp3A7;});_tmp3A6->tl=decls;_tmp3A6;});{struct _tuple11 _tmp3A8=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3A4,Cyc_Absyn_var_exp(_tmp3A3,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp3A0,fail_stmt,decls);nv=_tmp3A8.f1;decls=_tmp3A8.f2;{struct Cyc_Absyn_Exp*
_tmp3A9=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((void*)((struct
Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp39E->fields))->v,f)))->type))_tmp3A9=Cyc_Absyn_cast_exp(
_tmp3A5,_tmp3A9,0);{struct Cyc_Absyn_Stmt*_tmp3AA=_tmp3A8.f3;struct Cyc_Absyn_Stmt*
_tmp3AB=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp3A3,0),_tmp3A9,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp3AB,_tmp3AA,0),0);}}}}}}goto _LL285;}_LL2A2: if(_tmp367 <= (
void*)2?1:*((int*)_tmp367)!= 5)goto _LL2A4;_tmp382=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp367)->f1;_LL2A3: {struct _tuple1*_tmp3AC=Cyc_Toc_temp_var();void*_tmp3AD=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp382->topt))->v;decls=({struct Cyc_List_List*
_tmp3AE=_region_malloc(rgn,sizeof(*_tmp3AE));_tmp3AE->hd=({struct _tuple12*
_tmp3AF=_region_malloc(rgn,sizeof(*_tmp3AF));_tmp3AF->f1=_tmp3AC;_tmp3AF->f2=Cyc_Toc_typ_to_c(
_tmp3AD);_tmp3AF;});_tmp3AE->tl=decls;_tmp3AE;});{struct _tuple11 _tmp3B0=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3AD,Cyc_Absyn_var_exp(_tmp3AC,0),Cyc_Absyn_deref_exp(path,0),_tmp382,
fail_stmt,decls);nv=_tmp3B0.f1;decls=_tmp3B0.f2;{struct Cyc_Absyn_Stmt*_tmp3B1=
_tmp3B0.f3;struct Cyc_Absyn_Stmt*_tmp3B2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp3AC,0),Cyc_Absyn_deref_exp(r,0),0),_tmp3B1,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp3B2,0);else{s=_tmp3B2;}goto
_LL285;}}}_LL2A4: if(_tmp367 <= (void*)2?1:*((int*)_tmp367)!= 11)goto _LL2A6;_LL2A5:({
void*_tmp3B3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unknownid",sizeof(unsigned char),10),_tag_arr(_tmp3B3,sizeof(void*),0));});
_LL2A6: if(_tmp367 <= (void*)2?1:*((int*)_tmp367)!= 12)goto _LL285;_LL2A7:({void*
_tmp3B4[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unknowncall",sizeof(unsigned char),12),_tag_arr(_tmp3B4,sizeof(void*),
0));});_LL285:;}return({struct _tuple11 _tmp3B5;_tmp3B5.f1=nv;_tmp3B5.f2=decls;
_tmp3B5.f3=s;_tmp3B5;});}struct _tuple14{struct _tagged_arr*f1;struct _tagged_arr*
f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple14*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){return({struct _tuple14*
_tmp3B6=_region_malloc(r,sizeof(*_tmp3B6));_tmp3B6->f1=Cyc_Toc_fresh_label();
_tmp3B6->f2=Cyc_Toc_fresh_label();_tmp3B6->f3=sc;_tmp3B6;});}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp3B7=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp3B8=Cyc_Tcutil_compress(
_tmp3B7);_LL2AE: if(_tmp3B8 <= (void*)3?1:*((int*)_tmp3B8)!= 5)goto _LL2B0;_LL2AF:
goto _LL2B1;_LL2B0: if(_tmp3B8 <= (void*)3?1:*((int*)_tmp3B8)!= 12)goto _LL2B2;
_LL2B1: leave_as_switch=1;goto _LL2AD;_LL2B2:;_LL2B3: leave_as_switch=0;goto _LL2AD;
_LL2AD:;}{struct Cyc_List_List*_tmp3B9=scs;for(0;_tmp3B9 != 0;_tmp3B9=_tmp3B9->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)
_tmp3B9->hd)->pat_vars))->v != 0?1:((struct Cyc_Absyn_Switch_clause*)_tmp3B9->hd)->where_clause
!= 0){leave_as_switch=0;break;}}}if(leave_as_switch){struct _tagged_arr*next_l=
Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp3BA=scs;for(0;_tmp3BA != 0;_tmp3BA=
_tmp3BA->tl){struct Cyc_Absyn_Stmt*_tmp3BB=((struct Cyc_Absyn_Switch_clause*)
_tmp3BA->hd)->body;((struct Cyc_Absyn_Switch_clause*)_tmp3BA->hd)->body=Cyc_Absyn_label_stmt(
next_l,_tmp3BB,0);next_l=Cyc_Toc_fresh_label();Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
nv,next_l),_tmp3BB);}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct
_tagged_arr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp3BC=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp3BC;_push_region(rgn);{struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp3BD=lscs;for(0;_tmp3BD != 0;_tmp3BD=_tmp3BD->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple14*)_tmp3BD->hd)).f3;struct _tagged_arr*fail_lab=_tmp3BD->tl == 
0?end_l:(*((struct _tuple14*)((struct Cyc_List_List*)_check_null(_tmp3BD->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp3BF;struct Cyc_List_List*_tmp3C0;struct Cyc_Absyn_Stmt*
_tmp3C1;struct _tuple11 _tmp3BE=Cyc_Toc_xlate_pat(nv,rgn,_tmp3B7,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp3BF=_tmp3BE.f1;_tmp3C0=_tmp3BE.f2;
_tmp3C1=_tmp3BE.f3;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp3C2=(struct
Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp3BF,_tmp3C2);
_tmp3C1=Cyc_Absyn_seq_stmt(_tmp3C1,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((
void*)11,_tmp3C2,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}
decls=_tmp3C0;nvs=({struct Cyc_List_List*_tmp3C3=_region_malloc(rgn,sizeof(*
_tmp3C3));_tmp3C3->hd=_tmp3BF;_tmp3C3->tl=nvs;_tmp3C3;});test_stmts=({struct Cyc_List_List*
_tmp3C4=_region_malloc(rgn,sizeof(*_tmp3C4));_tmp3C4->hd=_tmp3C1;_tmp3C4->tl=
test_stmts;_tmp3C4;});}}nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
nvs);test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
test_stmts);{struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple14 _tmp3C6;struct _tagged_arr*_tmp3C7;
struct _tagged_arr*_tmp3C8;struct Cyc_Absyn_Switch_clause*_tmp3C9;struct _tuple14*
_tmp3C5=(struct _tuple14*)lscs->hd;_tmp3C6=*_tmp3C5;_tmp3C7=_tmp3C6.f1;_tmp3C8=
_tmp3C6.f2;_tmp3C9=_tmp3C6.f3;{struct Cyc_Toc_Env*_tmp3CA=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp3C9->body;
if(lscs->tl != 0){struct _tuple14 _tmp3CC;struct _tagged_arr*_tmp3CD;struct Cyc_Absyn_Switch_clause*
_tmp3CE;struct _tuple14*_tmp3CB=(struct _tuple14*)((struct Cyc_List_List*)
_check_null(lscs->tl))->hd;_tmp3CC=*_tmp3CB;_tmp3CD=_tmp3CC.f2;_tmp3CE=_tmp3CC.f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp3CA,end_l,_tmp3CD,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3CE->pat_vars))->v,(
struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp3CA,end_l),s);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_label_stmt(_tmp3C7,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)
_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(_tmp3C8,s,0),0);stmts=({
struct Cyc_List_List*_tmp3CF=_region_malloc(rgn,sizeof(*_tmp3CF));_tmp3CF->hd=s;
_tmp3CF->tl=stmts;_tmp3CF;});}}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(
end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct
_tuple12 _tmp3D1;struct _tuple1*_tmp3D2;void*_tmp3D3;struct _tuple12*_tmp3D0=(
struct _tuple12*)((struct Cyc_List_List*)_check_null(decls))->hd;_tmp3D1=*_tmp3D0;
_tmp3D2=_tmp3D1.f1;_tmp3D3=_tmp3D1.f2;res=Cyc_Absyn_declare_stmt(_tmp3D2,_tmp3D3,
0,res,0);}(void*)(whole_s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,
res,0))->r));}}};_pop_region(rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp3D4=
_cycalloc(sizeof(*_tmp3D4));_tmp3D4->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),
0);_tmp3D4->tl=0;_tmp3D4;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp3D5=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Stmt*_tmp3D7;struct Cyc_Absyn_Stmt*
_tmp3D8;struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_Absyn_Stmt*
_tmp3DB;struct Cyc_Absyn_Stmt*_tmp3DC;struct _tuple3 _tmp3DD;struct Cyc_Absyn_Exp*
_tmp3DE;struct Cyc_Absyn_Stmt*_tmp3DF;struct Cyc_Absyn_Stmt*_tmp3E0;struct Cyc_Absyn_Stmt*
_tmp3E1;struct Cyc_Absyn_Stmt*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E3;struct _tuple3
_tmp3E4;struct Cyc_Absyn_Exp*_tmp3E5;struct _tuple3 _tmp3E6;struct Cyc_Absyn_Exp*
_tmp3E7;struct Cyc_Absyn_Stmt*_tmp3E8;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_List_List*
_tmp3EA;struct Cyc_List_List*_tmp3EB;struct Cyc_Absyn_Switch_clause**_tmp3EC;
struct Cyc_Absyn_Decl*_tmp3ED;struct Cyc_Absyn_Stmt*_tmp3EE;struct _tagged_arr*
_tmp3EF;struct Cyc_Absyn_Stmt*_tmp3F0;struct Cyc_Absyn_Stmt*_tmp3F1;struct _tuple3
_tmp3F2;struct Cyc_Absyn_Exp*_tmp3F3;struct Cyc_Absyn_Stmt*_tmp3F4;struct Cyc_List_List*
_tmp3F5;struct Cyc_Absyn_Tvar*_tmp3F6;struct Cyc_Absyn_Vardecl*_tmp3F7;struct Cyc_Absyn_Stmt*
_tmp3F8;_LL2B5: if((int)_tmp3D5 != 0)goto _LL2B7;_LL2B6: return;_LL2B7: if(_tmp3D5 <= (
void*)1?1:*((int*)_tmp3D5)!= 0)goto _LL2B9;_tmp3D6=((struct Cyc_Absyn_Exp_s_struct*)
_tmp3D5)->f1;_LL2B8: Cyc_Toc_exp_to_c(nv,_tmp3D6);return;_LL2B9: if(_tmp3D5 <= (
void*)1?1:*((int*)_tmp3D5)!= 1)goto _LL2BB;_tmp3D7=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3D5)->f1;_tmp3D8=((struct Cyc_Absyn_Seq_s_struct*)_tmp3D5)->f2;_LL2BA: Cyc_Toc_stmt_to_c(
nv,_tmp3D7);s=_tmp3D8;continue;_LL2BB: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 
2)goto _LL2BD;_tmp3D9=((struct Cyc_Absyn_Return_s_struct*)_tmp3D5)->f1;_LL2BC: {
struct Cyc_Core_Opt*topt=0;if(_tmp3D9 != 0){topt=({struct Cyc_Core_Opt*_tmp3F9=
_cycalloc(sizeof(*_tmp3F9));_tmp3F9->v=(void*)Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp3D9->topt))->v);_tmp3F9;});Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmp3D9));}if(s->try_depth > 0){if(topt != 0){
struct _tuple1*_tmp3FA=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp3FB=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp3FA,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp3FA,(void*)topt->v,_tmp3D9,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp3FB,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL2BD: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 3)goto _LL2BF;_tmp3DA=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D5)->f1;_tmp3DB=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp3D5)->f2;_tmp3DC=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D5)->f3;_LL2BE:
Cyc_Toc_exp_to_c(nv,_tmp3DA);Cyc_Toc_stmt_to_c(nv,_tmp3DB);s=_tmp3DC;continue;
_LL2BF: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 4)goto _LL2C1;_tmp3DD=((struct
Cyc_Absyn_While_s_struct*)_tmp3D5)->f1;_tmp3DE=_tmp3DD.f1;_tmp3DF=((struct Cyc_Absyn_While_s_struct*)
_tmp3D5)->f2;_LL2C0: Cyc_Toc_exp_to_c(nv,_tmp3DE);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp3DF);return;_LL2C1: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 5)goto
_LL2C3;_tmp3E0=((struct Cyc_Absyn_Break_s_struct*)_tmp3D5)->f1;_LL2C2: if(nv->break_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,0))->r));{int dest_depth=
_tmp3E0 == 0?0: _tmp3E0->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,
s);return;}_LL2C3: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 6)goto _LL2C5;
_tmp3E1=((struct Cyc_Absyn_Continue_s_struct*)_tmp3D5)->f1;_LL2C4: if(nv->continue_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0))->r));_tmp3E2=_tmp3E1;
goto _LL2C6;_LL2C5: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 7)goto _LL2C7;
_tmp3E2=((struct Cyc_Absyn_Goto_s_struct*)_tmp3D5)->f2;_LL2C6: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp3E2))->try_depth,s);
return;_LL2C7: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 8)goto _LL2C9;_tmp3E3=((
struct Cyc_Absyn_For_s_struct*)_tmp3D5)->f1;_tmp3E4=((struct Cyc_Absyn_For_s_struct*)
_tmp3D5)->f2;_tmp3E5=_tmp3E4.f1;_tmp3E6=((struct Cyc_Absyn_For_s_struct*)_tmp3D5)->f3;
_tmp3E7=_tmp3E6.f1;_tmp3E8=((struct Cyc_Absyn_For_s_struct*)_tmp3D5)->f4;_LL2C8:
Cyc_Toc_exp_to_c(nv,_tmp3E3);Cyc_Toc_exp_to_c(nv,_tmp3E5);Cyc_Toc_exp_to_c(nv,
_tmp3E7);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp3E8);return;_LL2C9: if(
_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 9)goto _LL2CB;_tmp3E9=((struct Cyc_Absyn_Switch_s_struct*)
_tmp3D5)->f1;_tmp3EA=((struct Cyc_Absyn_Switch_s_struct*)_tmp3D5)->f2;_LL2CA: Cyc_Toc_xlate_switch(
nv,s,_tmp3E9,_tmp3EA);return;_LL2CB: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 
11)goto _LL2CD;_tmp3EB=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp3D5)->f1;_tmp3EC=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp3D5)->f2;_LL2CC: if(nv->fallthru_info == 0)({
void*_tmp3FC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("fallthru in unexpected place",sizeof(unsigned char),29),_tag_arr(
_tmp3FC,sizeof(void*),0));});{struct _tuple7 _tmp3FE;struct _tagged_arr*_tmp3FF;
struct Cyc_List_List*_tmp400;struct Cyc_Dict_Dict*_tmp401;struct _tuple7*_tmp3FD=(
struct _tuple7*)((struct Cyc_Core_Opt*)_check_null(nv->fallthru_info))->v;_tmp3FE=*
_tmp3FD;_tmp3FF=_tmp3FE.f1;_tmp400=_tmp3FE.f2;_tmp401=_tmp3FE.f3;{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_goto_stmt(_tmp3FF,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((
struct Cyc_Absyn_Switch_clause**)_check_null(_tmp3EC)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp402=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
_tmp400);struct Cyc_List_List*_tmp403=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp3EB);for(0;_tmp402 != 0;(_tmp402=_tmp402->tl,_tmp403=_tmp403->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp403))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp401,(struct _tuple1*)
_tmp402->hd),(struct Cyc_Absyn_Exp*)_tmp403->hd,0),s2,0);}(void*)(s->r=(void*)((
void*)s2->r));return;}}}_LL2CD: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 12)
goto _LL2CF;_tmp3ED=((struct Cyc_Absyn_Decl_s_struct*)_tmp3D5)->f1;_tmp3EE=((
struct Cyc_Absyn_Decl_s_struct*)_tmp3D5)->f2;_LL2CE:{void*_tmp404=(void*)_tmp3ED->r;
struct Cyc_Absyn_Vardecl*_tmp405;struct Cyc_Absyn_Pat*_tmp406;struct Cyc_Core_Opt*
_tmp407;struct Cyc_Absyn_Exp*_tmp408;int _tmp409;struct Cyc_List_List*_tmp40A;
_LL2E0: if(*((int*)_tmp404)!= 0)goto _LL2E2;_tmp405=((struct Cyc_Absyn_Var_d_struct*)
_tmp404)->f1;_LL2E1: {struct Cyc_Toc_Env*_tmp40B=Cyc_Toc_add_varmap(nv,_tmp405->name,
Cyc_Absyn_varb_exp(_tmp405->name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp40C=
_cycalloc(sizeof(*_tmp40C));_tmp40C[0]=({struct Cyc_Absyn_Local_b_struct _tmp40D;
_tmp40D.tag=3;_tmp40D.f1=_tmp405;_tmp40D;});_tmp40C;}),0));Cyc_Toc_local_decl_to_c(
_tmp40B,_tmp40B,_tmp405,_tmp3EE);goto _LL2DF;}_LL2E2: if(*((int*)_tmp404)!= 2)goto
_LL2E4;_tmp406=((struct Cyc_Absyn_Let_d_struct*)_tmp404)->f1;_tmp407=((struct Cyc_Absyn_Let_d_struct*)
_tmp404)->f3;_tmp408=((struct Cyc_Absyn_Let_d_struct*)_tmp404)->f4;_tmp409=((
struct Cyc_Absyn_Let_d_struct*)_tmp404)->f5;_LL2E3:{void*_tmp40E=(void*)_tmp406->r;
struct Cyc_Absyn_Vardecl*_tmp40F;_LL2E9: if(_tmp40E <= (void*)2?1:*((int*)_tmp40E)
!= 0)goto _LL2EB;_tmp40F=((struct Cyc_Absyn_Var_p_struct*)_tmp40E)->f1;_LL2EA: {
struct _tuple1*old_name=_tmp40F->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp40F->name=new_name;_tmp40F->initializer=(struct Cyc_Absyn_Exp*)_tmp408;(void*)(
_tmp3ED->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp410=_cycalloc(
sizeof(*_tmp410));_tmp410[0]=({struct Cyc_Absyn_Var_d_struct _tmp411;_tmp411.tag=0;
_tmp411.f1=_tmp40F;_tmp411;});_tmp410;})));{struct Cyc_Toc_Env*_tmp412=Cyc_Toc_add_varmap(
nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413[0]=({struct Cyc_Absyn_Local_b_struct
_tmp414;_tmp414.tag=3;_tmp414.f1=_tmp40F;_tmp414;});_tmp413;}),0));Cyc_Toc_local_decl_to_c(
_tmp412,nv,_tmp40F,_tmp3EE);goto _LL2E8;}}_LL2EB:;_LL2EC:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp406,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp407))->v,_tmp408,_tmp3EE))->r));goto _LL2E8;_LL2E8:;}goto _LL2DF;_LL2E4: if(*((
int*)_tmp404)!= 3)goto _LL2E6;_tmp40A=((struct Cyc_Absyn_Letv_d_struct*)_tmp404)->f1;
_LL2E5: {struct Cyc_List_List*_tmp415=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp40A);if(_tmp415 == 0)({void*_tmp416[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("empty Letv_d",
sizeof(unsigned char),13),_tag_arr(_tmp416,sizeof(void*),0));});(void*)(_tmp3ED->r=(
void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp417=_cycalloc(sizeof(*_tmp417));
_tmp417[0]=({struct Cyc_Absyn_Var_d_struct _tmp418;_tmp418.tag=0;_tmp418.f1=(
struct Cyc_Absyn_Vardecl*)_tmp415->hd;_tmp418;});_tmp417;})));_tmp415=_tmp415->tl;
for(0;_tmp415 != 0;_tmp415=_tmp415->tl){struct Cyc_Absyn_Decl*_tmp419=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A[
0]=({struct Cyc_Absyn_Var_d_struct _tmp41B;_tmp41B.tag=0;_tmp41B.f1=(struct Cyc_Absyn_Vardecl*)
_tmp415->hd;_tmp41B;});_tmp41A;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp419,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL2DF;}_LL2E6:;_LL2E7:({void*_tmp41C[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad nested declaration within function",
sizeof(unsigned char),39),_tag_arr(_tmp41C,sizeof(void*),0));});_LL2DF:;}return;
_LL2CF: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 15)goto _LL2D1;_tmp3EF=((struct
Cyc_Absyn_Label_s_struct*)_tmp3D5)->f1;_tmp3F0=((struct Cyc_Absyn_Label_s_struct*)
_tmp3D5)->f2;_LL2D0: s=_tmp3F0;continue;_LL2D1: if(_tmp3D5 <= (void*)1?1:*((int*)
_tmp3D5)!= 16)goto _LL2D3;_tmp3F1=((struct Cyc_Absyn_Do_s_struct*)_tmp3D5)->f1;
_tmp3F2=((struct Cyc_Absyn_Do_s_struct*)_tmp3D5)->f2;_tmp3F3=_tmp3F2.f1;_LL2D2:
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp3F1);Cyc_Toc_exp_to_c(nv,_tmp3F3);
return;_LL2D3: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 17)goto _LL2D5;_tmp3F4=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp3D5)->f1;_tmp3F5=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3D5)->f2;_LL2D4: {struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=
Cyc_Toc_temp_var();struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
h_exp=Cyc_Absyn_var_exp(h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(
e_var,0);struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ);void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_t);e_exp->topt=({
struct Cyc_Core_Opt*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D->v=(void*)e_typ;
_tmp41D;});nv=Cyc_Toc_add_varmap(nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp3F4);{
struct Cyc_Absyn_Stmt*_tmp41E=Cyc_Absyn_seq_stmt(_tmp3F4,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_handler_e,0,0),0),0);struct _tuple1*_tmp41F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp420=Cyc_Absyn_var_exp(_tmp41F,0);struct Cyc_Absyn_Vardecl*
_tmp421=Cyc_Absyn_new_vardecl(_tmp41F,Cyc_Absyn_exn_typ,0);_tmp420->topt=({
struct Cyc_Core_Opt*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->v=(void*)Cyc_Absyn_exn_typ;
_tmp422;});{struct Cyc_Absyn_Pat*_tmp423=({struct Cyc_Absyn_Pat*_tmp433=_cycalloc(
sizeof(*_tmp433));_tmp433->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*
_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435[0]=({struct Cyc_Absyn_Var_p_struct
_tmp436;_tmp436.tag=0;_tmp436.f1=_tmp421;_tmp436;});_tmp435;}));_tmp433->topt=({
struct Cyc_Core_Opt*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->v=(void*)Cyc_Absyn_exn_typ;
_tmp434;});_tmp433->loc=0;_tmp433;});struct Cyc_Absyn_Exp*_tmp424=Cyc_Absyn_throw_exp(
_tmp420,0);_tmp424->topt=({struct Cyc_Core_Opt*_tmp425=_cycalloc(sizeof(*_tmp425));
_tmp425->v=(void*)((void*)0);_tmp425;});{struct Cyc_Absyn_Stmt*_tmp426=Cyc_Absyn_exp_stmt(
_tmp424,0);struct Cyc_Absyn_Switch_clause*_tmp427=({struct Cyc_Absyn_Switch_clause*
_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430->pattern=_tmp423;_tmp430->pat_vars=({
struct Cyc_Core_Opt*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->v=({struct Cyc_List_List*
_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->hd=_tmp421;_tmp432->tl=0;_tmp432;});
_tmp431;});_tmp430->where_clause=0;_tmp430->body=_tmp426;_tmp430->loc=0;_tmp430;});
struct Cyc_Absyn_Stmt*_tmp428=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp3F5,({struct
Cyc_List_List*_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F->hd=_tmp427;_tmp42F->tl=
0;_tmp42F;})),0);Cyc_Toc_stmt_to_c(nv,_tmp428);{struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp42E=_cycalloc(sizeof(*_tmp42E));
_tmp42E->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp42E->tl=0;
_tmp42E;}),0);struct Cyc_Absyn_Stmt*_tmp42A=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp42D=_cycalloc(sizeof(*_tmp42D));
_tmp42D->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp42D->tl=0;_tmp42D;}),0),0);struct
Cyc_Absyn_Exp*_tmp42B=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp42C=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp42A,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp42B,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp429,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp42C,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp41E,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_cast_exp(
e_typ,Cyc_Toc__exn_thrown_e,0),_tmp428,0),0),0),0),0),0))->r));return;}}}}}
_LL2D5: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)!= 18)goto _LL2D7;_tmp3F6=((struct
Cyc_Absyn_Region_s_struct*)_tmp3D5)->f1;_tmp3F7=((struct Cyc_Absyn_Region_s_struct*)
_tmp3D5)->f2;_tmp3F8=((struct Cyc_Absyn_Region_s_struct*)_tmp3D5)->f3;_LL2D6: {
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*
rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp3F7->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(nv,x_var,x_exp),_tmp3F8);(void*)(s->r=(
void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp437=
_cycalloc(sizeof(*_tmp437));_tmp437->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0);_tmp437->tl=0;_tmp437;}),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*
_tmp438[1];_tmp438[0]=x_exp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp438,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),Cyc_Absyn_seq_stmt(
_tmp3F8,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct
Cyc_Absyn_Exp*_tmp439[1];_tmp439[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp439,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL2D7: if(_tmp3D5 <= (void*)1?1:*((int*)_tmp3D5)
!= 10)goto _LL2D9;_LL2D8:({void*_tmp43A[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("switchC_s",sizeof(
unsigned char),10),_tag_arr(_tmp43A,sizeof(void*),0));});_LL2D9: if(_tmp3D5 <= (
void*)1?1:*((int*)_tmp3D5)!= 13)goto _LL2DB;_LL2DA:({void*_tmp43B[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("cut",sizeof(
unsigned char),4),_tag_arr(_tmp43B,sizeof(void*),0));});_LL2DB: if(_tmp3D5 <= (
void*)1?1:*((int*)_tmp3D5)!= 14)goto _LL2DD;_LL2DC:({void*_tmp43C[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("splice",
sizeof(unsigned char),7),_tag_arr(_tmp43C,sizeof(void*),0));});_LL2DD: if(_tmp3D5
<= (void*)1?1:*((int*)_tmp3D5)!= 19)goto _LL2B4;_LL2DE:({void*_tmp43D[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("forarray",
sizeof(unsigned char),9),_tag_arr(_tmp43D,sizeof(void*),0));});_LL2B4:;}}struct
_tuple15{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f){f->tvs=0;f->effect=0;f->rgn_po=0;(
void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{struct Cyc_List_List*
_tmp43E=f->args;for(0;_tmp43E != 0;_tmp43E=_tmp43E->tl){(*((struct _tuple15*)
_tmp43E->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple15*)_tmp43E->hd)).f3);{struct
_tuple1*_tmp43F=({struct _tuple1*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440->f1=(
void*)0;_tmp440->f2=(*((struct _tuple15*)_tmp43E->hd)).f1;_tmp440;});nv=Cyc_Toc_add_varmap(
nv,_tmp43F,Cyc_Absyn_var_exp(_tmp43F,0));}}}if((unsigned int)f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0: 0){struct Cyc_Core_Opt*
_tmp442;struct Cyc_Absyn_Tqual _tmp443;void*_tmp444;int _tmp445;struct Cyc_Absyn_VarargInfo
_tmp441=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp442=
_tmp441.name;_tmp443=_tmp441.tq;_tmp444=(void*)_tmp441.type;_tmp445=_tmp441.inject;{
void*_tmp446=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ(_tmp444,(void*)2,_tmp443));
struct _tuple1*_tmp447=({struct _tuple1*_tmp44A=_cycalloc(sizeof(*_tmp44A));
_tmp44A->f1=(void*)0;_tmp44A->f2=(struct _tagged_arr*)((struct Cyc_Core_Opt*)
_check_null(_tmp442))->v;_tmp44A;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp448=
_cycalloc(sizeof(*_tmp448));_tmp448->hd=({struct _tuple15*_tmp449=_cycalloc(
sizeof(*_tmp449));_tmp449->f1=(struct _tagged_arr*)_tmp442->v;_tmp449->f2=_tmp443;
_tmp449->f3=_tmp446;_tmp449;});_tmp448->tl=0;_tmp448;}));nv=Cyc_Toc_add_varmap(
nv,_tmp447,Cyc_Absyn_var_exp(_tmp447,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp44B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp44B != 0;_tmp44B=_tmp44B->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp44B->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp44B->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(nv),f->body);}
static void*Cyc_Toc_scope_to_c(void*s){void*_tmp44C=s;_LL2EE: if((int)_tmp44C != 1)
goto _LL2F0;_LL2EF: return(void*)2;_LL2F0: if((int)_tmp44C != 4)goto _LL2F2;_LL2F1:
return(void*)3;_LL2F2:;_LL2F3: return s;_LL2ED:;}struct Cyc_Core_Opt*Cyc_Toc_aggrs_so_far=
0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*
_tmp44D=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({struct Cyc_Core_Opt*
_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E->v=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);
_tmp44E;});{int seen_defn_before;struct Cyc_Core_Opt*_tmp44F=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*_tmp44D).f2);if(
_tmp44F == 0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*_tmp44D).f2,ad));}
else{if(((struct Cyc_Absyn_Aggrdecl*)_tmp44F->v)->fields == 0){(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*
_tmp44D).f2,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(
void*)((void*)2));ad->tvs=0;ad->exist_vars=0;if(ad->fields != 0){if(
seen_defn_before)ad->fields=0;else{struct Cyc_List_List*_tmp450=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ad->fields))->v;for(0;_tmp450 != 0;_tmp450=
_tmp450->tl){(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp450->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp450->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp451;});{
struct _tuple1*_tmp452=tud->name;if(tud->fields == 0?1:((int(*)(struct Cyc_Set_Set*
s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp452))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp452));{struct Cyc_List_List*
_tmp453=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp453 != 0;_tmp453=_tmp453->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp453->hd;if(f->typs != 0){struct Cyc_List_List*_tmp454=0;int i=1;{struct Cyc_List_List*
_tmp455=f->typs;for(0;_tmp455 != 0;(_tmp455=_tmp455->tl,i ++)){struct _tagged_arr*
_tmp456=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*_tmp457=({struct Cyc_Absyn_Aggrfield*
_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459->name=_tmp456;_tmp459->tq=(*((struct
_tuple4*)_tmp455->hd)).f1;_tmp459->type=(void*)Cyc_Toc_typ_to_c_array((*((struct
_tuple4*)_tmp455->hd)).f2);_tmp459->width=0;_tmp459->attributes=0;_tmp459;});
_tmp454=({struct Cyc_List_List*_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->hd=
_tmp457;_tmp458->tl=_tmp454;_tmp458;});}}_tmp454=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp454);_tmp454=({struct Cyc_List_List*
_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->hd=({struct Cyc_Absyn_Aggrfield*
_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->name=Cyc_Toc_tag_sp;_tmp45B->tq=Cyc_Toc_mt_tq;
_tmp45B->type=(void*)Cyc_Absyn_sint_t;_tmp45B->width=0;_tmp45B->attributes=0;
_tmp45B;});_tmp45A->tl=_tmp454;_tmp45A;});{struct Cyc_Absyn_Aggrdecl*_tmp45C=({
struct Cyc_Absyn_Aggrdecl*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->kind=(void*)((
void*)0);_tmp460->sc=(void*)((void*)2);_tmp460->name=Cyc_Toc_collapse_qvar_tag(f->name,
_tag_arr("_struct",sizeof(unsigned char),8));_tmp460->tvs=0;_tmp460->exist_vars=
0;_tmp460->fields=({struct Cyc_Core_Opt*_tmp461=_cycalloc(sizeof(*_tmp461));
_tmp461->v=_tmp454;_tmp461;});_tmp460->attributes=0;_tmp460;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp45E=_cycalloc(sizeof(*_tmp45E));
_tmp45E[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp45F;_tmp45F.tag=4;_tmp45F.f1=
_tmp45C;_tmp45F;});_tmp45E;}),0);_tmp45D->tl=Cyc_Toc_result_decls;_tmp45D;});}}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Core_Opt*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp462;});if(xd->fields == 0)return;{struct _tuple1*_tmp463=xd->name;struct Cyc_List_List*
_tmp464=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp464 != 0;_tmp464=_tmp464->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp464->hd;struct _tagged_arr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp465=Cyc_Absyn_uint_exp(
_get_arr_size(*fn,sizeof(unsigned char))+ 4,0);struct Cyc_Absyn_ArrayType_struct*
_tmp466=({struct Cyc_Absyn_ArrayType_struct*_tmp48F=_cycalloc(sizeof(*_tmp48F));
_tmp48F[0]=({struct Cyc_Absyn_ArrayType_struct _tmp490;_tmp490.tag=7;_tmp490.f1=(
void*)Cyc_Absyn_uchar_t;_tmp490.f2=Cyc_Toc_mt_tq;_tmp490.f3=(struct Cyc_Absyn_Exp*)
_tmp465;_tmp490;});_tmp48F;});struct Cyc_Core_Opt*_tmp467=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,f->name);
struct Cyc_Core_Opt _tmp468;int _tmp469;_LL2F5: if(_tmp467 != 0)goto _LL2F7;_LL2F6: {
struct Cyc_Absyn_Exp*initopt=0;if((void*)f->sc != (void*)3)initopt=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp((struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp46F;
_tmp46F.tag=0;_tmp46F.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_Int_pa_struct
_tmp46E;_tmp46E.tag=1;_tmp46E.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp46D;_tmp46D.tag=1;_tmp46D.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp46C;_tmp46C.tag=1;_tmp46C.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp46B;_tmp46B.tag=1;_tmp46B.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp46A[5]={& _tmp46B,& _tmp46C,& _tmp46D,& _tmp46E,& _tmp46F};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(unsigned char),11),_tag_arr(_tmp46A,sizeof(void*),5));}}}}}}),
0);{struct Cyc_Absyn_Vardecl*_tmp470=Cyc_Absyn_new_vardecl(f->name,(void*)_tmp466,
initopt);(void*)(_tmp470->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472[
0]=({struct Cyc_Absyn_Var_d_struct _tmp473;_tmp473.tag=0;_tmp473.f1=_tmp470;
_tmp473;});_tmp472;}),0);_tmp471->tl=Cyc_Toc_result_decls;_tmp471;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp474=f->typs;for(0;_tmp474 != 0;(_tmp474=_tmp474->tl,
i ++)){struct _tagged_arr*_tmp475=({struct _tagged_arr*_tmp479=_cycalloc(sizeof(*
_tmp479));_tmp479[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp47B;
_tmp47B.tag=1;_tmp47B.f1=(int)((unsigned int)i);{void*_tmp47A[1]={& _tmp47B};Cyc_Std_aprintf(
_tag_arr("f%d",sizeof(unsigned char),4),_tag_arr(_tmp47A,sizeof(void*),1));}});
_tmp479;});struct Cyc_Absyn_Aggrfield*_tmp476=({struct Cyc_Absyn_Aggrfield*_tmp478=
_cycalloc(sizeof(*_tmp478));_tmp478->name=_tmp475;_tmp478->tq=(*((struct _tuple4*)
_tmp474->hd)).f1;_tmp478->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp474->hd)).f2);_tmp478->width=0;_tmp478->attributes=0;_tmp478;});fields=({
struct Cyc_List_List*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->hd=_tmp476;
_tmp477->tl=fields;_tmp477;});}}fields=({struct Cyc_List_List*_tmp47C=_cycalloc(
sizeof(*_tmp47C));_tmp47C->hd=({struct Cyc_Absyn_Aggrfield*_tmp47D=_cycalloc(
sizeof(*_tmp47D));_tmp47D->name=Cyc_Toc_tag_sp;_tmp47D->tq=Cyc_Toc_mt_tq;_tmp47D->type=(
void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_t,Cyc_Toc_mt_tq);_tmp47D->width=0;
_tmp47D->attributes=0;_tmp47D;});_tmp47C->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);_tmp47C;});{struct Cyc_Absyn_Aggrdecl*_tmp47E=({
struct Cyc_Absyn_Aggrdecl*_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482->kind=(void*)((
void*)0);_tmp482->sc=(void*)((void*)2);_tmp482->name=Cyc_Toc_collapse_qvar_tag(f->name,
_tag_arr("_struct",sizeof(unsigned char),8));_tmp482->tvs=0;_tmp482->exist_vars=
0;_tmp482->fields=({struct Cyc_Core_Opt*_tmp483=_cycalloc(sizeof(*_tmp483));
_tmp483->v=fields;_tmp483;});_tmp482->attributes=0;_tmp482;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp480=_cycalloc(sizeof(*_tmp480));
_tmp480[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp481;_tmp481.tag=4;_tmp481.f1=
_tmp47E;_tmp481;});_tmp480;}),0);_tmp47F->tl=Cyc_Toc_result_decls;_tmp47F;});}}
goto _LL2F4;}}_LL2F7: if(_tmp467 == 0)goto _LL2F9;_tmp468=*_tmp467;_tmp469=(int)
_tmp468.v;if(_tmp469 != 0)goto _LL2F9;_LL2F8: if((void*)f->sc != (void*)3){struct Cyc_Absyn_Exp*
_tmp484=Cyc_Absyn_string_exp((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp48E;_tmp48E.tag=0;_tmp48E.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_Int_pa_struct
_tmp48D;_tmp48D.tag=1;_tmp48D.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp48C;_tmp48C.tag=1;_tmp48C.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp48B;_tmp48B.tag=1;_tmp48B.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp48A;_tmp48A.tag=1;_tmp48A.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp489[5]={& _tmp48A,& _tmp48B,& _tmp48C,& _tmp48D,& _tmp48E};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(unsigned char),11),_tag_arr(_tmp489,sizeof(void*),5));}}}}}}),
0);struct Cyc_Absyn_Vardecl*_tmp485=Cyc_Absyn_new_vardecl(f->name,(void*)_tmp466,(
struct Cyc_Absyn_Exp*)_tmp484);(void*)(_tmp485->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487[
0]=({struct Cyc_Absyn_Var_d_struct _tmp488;_tmp488.tag=0;_tmp488.f1=_tmp485;
_tmp488;});_tmp487;}),0);_tmp486->tl=Cyc_Toc_result_decls;_tmp486;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL2F4;_LL2F9:;_LL2FA: goto _LL2F4;_LL2F4:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array((void*)vd->type));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp491=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp492;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*_tmp494;_LL2FC: if(*((int*)
_tmp491)!= 27)goto _LL2FE;_tmp492=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp491)->f1;_tmp493=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp491)->f2;
_tmp494=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp491)->f3;_LL2FD: vd->initializer=
0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(
vd->name,0),_tmp492,_tmp493,_tmp494,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL2FB;_LL2FE:;_LL2FF: if((void*)vd->sc == (void*)0){struct Cyc_Toc_Env*_tmp495=({
struct Cyc_Toc_Env*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496[0]=*init_nv;
_tmp496;});_tmp495->toplevel=1;Cyc_Toc_exp_to_c(_tmp495,init);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL2FB;_LL2FB:;}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=
0;static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp497=_cycalloc(
sizeof(*_tmp497));_tmp497[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp497;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp498=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp498;_push_region(prgn);{struct Cyc_Toc_Env*_tmp49A;
struct Cyc_List_List*_tmp49B;struct Cyc_Absyn_Stmt*_tmp49C;struct _tuple11 _tmp499=
Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp49A=_tmp499.f1;_tmp49B=_tmp499.f2;_tmp49C=_tmp499.f3;Cyc_Toc_stmt_to_c(
_tmp49A,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp49C,s,0),0);for(0;_tmp49B != 0;_tmp49B=_tmp49B->tl){
struct _tuple12 _tmp49E;struct _tuple1*_tmp49F;void*_tmp4A0;struct _tuple12*_tmp49D=(
struct _tuple12*)_tmp49B->hd;_tmp49E=*_tmp49D;_tmp49F=_tmp49E.f1;_tmp4A0=_tmp49E.f2;
s=Cyc_Absyn_declare_stmt(_tmp49F,_tmp4A0,0,s,0);}};_pop_region(prgn);}return s;}}
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top){for(0;ds != 0;ds=ds->tl){if(!nv->toplevel)({void*_tmp4A1[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("decls_to_c: not at toplevel!",
sizeof(unsigned char),29),_tag_arr(_tmp4A1,sizeof(void*),0));});{struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp4A2=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp4A3;struct Cyc_Absyn_Fndecl*_tmp4A4;struct Cyc_Absyn_Aggrdecl*_tmp4A5;struct
Cyc_Absyn_Tuniondecl*_tmp4A6;struct Cyc_Absyn_Enumdecl*_tmp4A7;struct Cyc_Absyn_Typedefdecl*
_tmp4A8;struct Cyc_List_List*_tmp4A9;struct Cyc_List_List*_tmp4AA;struct Cyc_List_List*
_tmp4AB;_LL301: if(*((int*)_tmp4A2)!= 0)goto _LL303;_tmp4A3=((struct Cyc_Absyn_Var_d_struct*)
_tmp4A2)->f1;_LL302: {struct _tuple1*_tmp4AC=_tmp4A3->name;if((void*)_tmp4A3->sc
== (void*)4)_tmp4AC=({struct _tuple1*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->f1=
Cyc_Absyn_rel_ns_null;_tmp4AD->f2=(*_tmp4AC).f2;_tmp4AD;});if(_tmp4A3->initializer
!= 0)Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp4A3->initializer));
nv=Cyc_Toc_add_varmap(nv,_tmp4A3->name,Cyc_Absyn_varb_exp(_tmp4AC,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE[0]=({
struct Cyc_Absyn_Global_b_struct _tmp4AF;_tmp4AF.tag=0;_tmp4AF.f1=_tmp4A3;_tmp4AF;});
_tmp4AE;}),0));_tmp4A3->name=_tmp4AC;(void*)(_tmp4A3->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp4A3->sc));(void*)(_tmp4A3->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp4A3->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4B0=_cycalloc(
sizeof(*_tmp4B0));_tmp4B0->hd=d;_tmp4B0->tl=Cyc_Toc_result_decls;_tmp4B0;});goto
_LL300;}_LL303: if(*((int*)_tmp4A2)!= 1)goto _LL305;_tmp4A4=((struct Cyc_Absyn_Fn_d_struct*)
_tmp4A2)->f1;_LL304: {struct _tuple1*_tmp4B1=_tmp4A4->name;if((void*)_tmp4A4->sc
== (void*)4)_tmp4B1=({struct _tuple1*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2->f1=
Cyc_Absyn_rel_ns_null;_tmp4B2->f2=(*_tmp4B1).f2;_tmp4B2;});nv=Cyc_Toc_add_varmap(
nv,_tmp4A4->name,Cyc_Absyn_var_exp(_tmp4B1,0));_tmp4A4->name=_tmp4B1;Cyc_Toc_fndecl_to_c(
nv,_tmp4A4);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4B3=_cycalloc(sizeof(*
_tmp4B3));_tmp4B3->hd=d;_tmp4B3->tl=Cyc_Toc_result_decls;_tmp4B3;});goto _LL300;}
_LL305: if(*((int*)_tmp4A2)!= 2)goto _LL307;_LL306: goto _LL308;_LL307: if(*((int*)
_tmp4A2)!= 3)goto _LL309;_LL308:({void*_tmp4B4[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("letdecl at toplevel",
sizeof(unsigned char),20),_tag_arr(_tmp4B4,sizeof(void*),0));});_LL309: if(*((int*)
_tmp4A2)!= 4)goto _LL30B;_tmp4A5=((struct Cyc_Absyn_Aggr_d_struct*)_tmp4A2)->f1;
_LL30A: Cyc_Toc_aggrdecl_to_c(_tmp4A5);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->hd=d;_tmp4B5->tl=Cyc_Toc_result_decls;
_tmp4B5;});goto _LL300;_LL30B: if(*((int*)_tmp4A2)!= 5)goto _LL30D;_tmp4A6=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp4A2)->f1;_LL30C: if(_tmp4A6->is_xtunion)Cyc_Toc_xtuniondecl_to_c(
_tmp4A6);else{Cyc_Toc_tuniondecl_to_c(_tmp4A6);}goto _LL300;_LL30D: if(*((int*)
_tmp4A2)!= 6)goto _LL30F;_tmp4A7=((struct Cyc_Absyn_Enum_d_struct*)_tmp4A2)->f1;
_LL30E: Cyc_Toc_enumdecl_to_c(nv,_tmp4A7);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6->hd=d;_tmp4B6->tl=Cyc_Toc_result_decls;
_tmp4B6;});goto _LL300;_LL30F: if(*((int*)_tmp4A2)!= 7)goto _LL311;_tmp4A8=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp4A2)->f1;_LL310: _tmp4A8->name=_tmp4A8->name;
_tmp4A8->tvs=0;(void*)(_tmp4A8->defn=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp4A8->defn));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4B7=_cycalloc(
sizeof(*_tmp4B7));_tmp4B7->hd=d;_tmp4B7->tl=Cyc_Toc_result_decls;_tmp4B7;});goto
_LL300;_LL311: if(*((int*)_tmp4A2)!= 8)goto _LL313;_tmp4A9=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp4A2)->f2;_LL312: _tmp4AA=_tmp4A9;goto _LL314;_LL313: if(*((int*)_tmp4A2)!= 9)
goto _LL315;_tmp4AA=((struct Cyc_Absyn_Using_d_struct*)_tmp4A2)->f2;_LL314: _tmp4AB=
_tmp4AA;goto _LL316;_LL315: if(*((int*)_tmp4A2)!= 10)goto _LL300;_tmp4AB=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp4A2)->f1;_LL316: nv=Cyc_Toc_decls_to_c(nv,_tmp4AB,
top);goto _LL300;_LL300:;}}return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(),ds,1);return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
