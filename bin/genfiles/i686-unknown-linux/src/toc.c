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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
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
int tag;float*f1;};int Cyc_Std_vfprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr,
struct _tagged_arr);extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter_c(void(*f)(void*,void*),
void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int Cyc_List_forall(int(*pred)(
void*),struct Cyc_List_List*x);int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct
_tagged_arr*);struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct
_tagged_arr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int
Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(
int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,
void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[
11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*
d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*name;int*identity;void*kind;
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrInfo{
void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Tqual
tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*
name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct
Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
_tuple1*,struct _tuple1*);extern struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value;
extern void*Cyc_Absyn_rel_ns_null;struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_cstar_typ(void*t,struct
Cyc_Absyn_Tqual tq);void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _tagged_arr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(struct
_tuple1*name);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
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
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_unify(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};void*
Cyc_Tcutil_snd_tqt(struct _tuple4*);struct _tuple5{unsigned int f1;int f2;};struct
_tuple5 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct
Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;void*rop;};extern char
Cyc_CfFlowInfo_IsZero[11];struct Cyc_CfFlowInfo_IsZero_struct{char*tag;struct Cyc_List_List*
f1;};extern char Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct
Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int
tag;struct Cyc_Dict_Dict*f1;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;
struct Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;};struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();int Cyc_Toc_warn_bounds_checks=
0;unsigned int Cyc_Toc_total_bounds_checks=0;unsigned int Cyc_Toc_bounds_checks_eliminated=
0;static struct Cyc_List_List*Cyc_Toc_result_decls=0;static struct Cyc_List_List*Cyc_Toc_tuple_types=
0;static int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented";
char Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible";static void*Cyc_Toc_unimp(
struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Std_vfprintf(Cyc_Std_stderr,fmt,ap);({
void*_tmp0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n",sizeof(char),2),
_tag_arr(_tmp0,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);(
int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(
struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Std_vfprintf(Cyc_Std_stderr,fmt,ap);({
void*_tmp1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n",sizeof(char),2),
_tag_arr(_tmp1,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);(
int)_throw((void*)Cyc_Toc_Toc_Impossible);}char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error";
static char _tmp2[5]="curr";static struct _tagged_arr Cyc_Toc_curr_string={_tmp2,
_tmp2,_tmp2 + 5};static struct _tagged_arr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp3[4]="tag";static struct _tagged_arr Cyc_Toc_tag_string={_tmp3,_tmp3,
_tmp3 + 4};static struct _tagged_arr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char
_tmp4[14]="_handler_cons";static struct _tagged_arr Cyc_Toc__handler_cons_string={
_tmp4,_tmp4,_tmp4 + 14};static struct _tagged_arr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp5[8]="handler";static struct _tagged_arr Cyc_Toc_handler_string={
_tmp5,_tmp5,_tmp5 + 8};static struct _tagged_arr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp6[14]="_RegionHandle";static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_tmp6,_tmp6,_tmp6 + 14};static struct _tagged_arr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp7[7]="_throw";static struct _tagged_arr Cyc_Toc__throw_str={_tmp7,
_tmp7,_tmp7 + 7};static struct _tuple1 Cyc_Toc__throw_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((void*)&
Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp9[7]="setjmp";static struct
_tagged_arr Cyc_Toc_setjmp_str={_tmp9,_tmp9,_tmp9 + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc_setjmp_ev={0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmpB[14]="_push_handler";
static struct _tagged_arr Cyc_Toc__push_handler_str={_tmpB,_tmpB,_tmpB + 14};static
struct _tuple1 Cyc_Toc__push_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((
void*)& Cyc_Toc__push_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char
_tmpD[13]="_pop_handler";static struct _tagged_arr Cyc_Toc__pop_handler_str={_tmpD,
_tmpD,_tmpD + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmpF[12]="_exn_thrown";static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_tmpF,_tmpF,_tmpF + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={
0,(void*)((void*)& Cyc_Toc__exn_thrown_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static
char _tmp11[14]="_npop_handler";static struct _tagged_arr Cyc_Toc__npop_handler_str={
_tmp11,_tmp11,_tmp11 + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)((void*)& Cyc_Toc__npop_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev;static char _tmp13[12]="_check_null";static struct
_tagged_arr Cyc_Toc__check_null_str={_tmp13,_tmp13,_tmp13 + 12};static struct
_tuple1 Cyc_Toc__check_null_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((
void*)& Cyc_Toc__check_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char
_tmp15[28]="_check_known_subscript_null";static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={
_tmp15,_tmp15,_tmp15 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp17[31]="_check_known_subscript_notnull";static struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={
_tmp17,_tmp17,_tmp17 + 31};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp19[25]="_check_unknown_subscript";
static struct _tagged_arr Cyc_Toc__check_unknown_subscript_str={_tmp19,_tmp19,
_tmp19 + 25};static struct _tuple1 Cyc_Toc__check_unknown_subscript_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1,& Cyc_Toc__check_unknown_subscript_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_unknown_subscript_ev={0,(void*)((void*)& Cyc_Toc__check_unknown_subscript_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_unknown_subscript_e=&
Cyc_Toc__check_unknown_subscript_ev;static char _tmp1B[12]="_tagged_arr";static
struct _tagged_arr Cyc_Toc__tagged_arr_str={_tmp1B,_tmp1B,_tmp1B + 12};static struct
_tuple1 Cyc_Toc__tagged_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={1,& Cyc_Toc__tagged_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={0,(void*)((
void*)& Cyc_Toc__tagged_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;static char
_tmp1D[9]="_tag_arr";static struct _tagged_arr Cyc_Toc__tag_arr_str={_tmp1D,_tmp1D,
_tmp1D + 9};static struct _tuple1 Cyc_Toc__tag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={1,&
Cyc_Toc__tag_arr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,(void*)((void*)& Cyc_Toc__tag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_arr_e=& Cyc_Toc__tag_arr_ev;static char
_tmp1F[14]="_init_tag_arr";static struct _tagged_arr Cyc_Toc__init_tag_arr_str={
_tmp1F,_tmp1F,_tmp1F + 14};static struct _tuple1 Cyc_Toc__init_tag_arr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_tag_arr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__init_tag_arr_re={1,& Cyc_Toc__init_tag_arr_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__init_tag_arr_ev={0,(void*)((void*)& Cyc_Toc__init_tag_arr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__init_tag_arr_e=&
Cyc_Toc__init_tag_arr_ev;static char _tmp21[11]="_untag_arr";static struct
_tagged_arr Cyc_Toc__untag_arr_str={_tmp21,_tmp21,_tmp21 + 11};static struct _tuple1
Cyc_Toc__untag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_arr_re={1,& Cyc_Toc__untag_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={0,(void*)((
void*)& Cyc_Toc__untag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;static char _tmp23[
14]="_get_arr_size";static struct _tagged_arr Cyc_Toc__get_arr_size_str={_tmp23,
_tmp23,_tmp23 + 14};static struct _tuple1 Cyc_Toc__get_arr_size_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__get_arr_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={
1,& Cyc_Toc__get_arr_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={
0,(void*)((void*)& Cyc_Toc__get_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_arr_size_e=& Cyc_Toc__get_arr_size_ev;
static char _tmp25[17]="_tagged_arr_plus";static struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={
_tmp25,_tmp25,_tmp25 + 17};static struct _tuple1 Cyc_Toc__tagged_arr_plus_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_plus_re={1,& Cyc_Toc__tagged_arr_plus_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={0,(void*)((void*)& Cyc_Toc__tagged_arr_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_plus_e=&
Cyc_Toc__tagged_arr_plus_ev;static char _tmp27[25]="_tagged_arr_inplace_plus";
static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={_tmp27,_tmp27,
_tmp27 + 25};static struct _tuple1 Cyc_Toc__tagged_arr_inplace_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tagged_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={
1,& Cyc_Toc__tagged_arr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_ev={0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_e=&
Cyc_Toc__tagged_arr_inplace_plus_ev;static char _tmp29[30]="_tagged_arr_inplace_plus_post";
static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={_tmp29,_tmp29,
_tmp29 + 30};static struct _tuple1 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={1,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static char _tmp2B[10]="_cycalloc";static struct _tagged_arr Cyc_Toc__cycalloc_str={
_tmp2B,_tmp2B,_tmp2B + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char
_tmp2D[11]="_cyccalloc";static struct _tagged_arr Cyc_Toc__cyccalloc_str={_tmp2D,
_tmp2D,_tmp2D + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={
1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char
_tmp2F[17]="_cycalloc_atomic";static struct _tagged_arr Cyc_Toc__cycalloc_atomic_str={
_tmp2F,_tmp2F,_tmp2F + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=&
Cyc_Toc__cycalloc_atomic_ev;static char _tmp31[18]="_cyccalloc_atomic";static
struct _tagged_arr Cyc_Toc__cyccalloc_atomic_str={_tmp31,_tmp31,_tmp31 + 18};static
struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmp33[15]="_region_malloc";static struct _tagged_arr Cyc_Toc__region_malloc_str={
_tmp33,_tmp33,_tmp33 + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmp35[15]="_region_calloc";static struct
_tagged_arr Cyc_Toc__region_calloc_str={_tmp35,_tmp35,_tmp35 + 15};static struct
_tuple1 Cyc_Toc__region_calloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)((
void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static
char _tmp37[13]="_check_times";static struct _tagged_arr Cyc_Toc__check_times_str={
_tmp37,_tmp37,_tmp37 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmp39[12]="_new_region";static struct
_tagged_arr Cyc_Toc__new_region_str={_tmp39,_tmp39,_tmp39 + 12};static struct
_tuple1 Cyc_Toc__new_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)((
void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char
_tmp3B[13]="_push_region";static struct _tagged_arr Cyc_Toc__push_region_str={
_tmp3B,_tmp3B,_tmp3B + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmp3D[12]="_pop_region";static struct
_tagged_arr Cyc_Toc__pop_region_str={_tmp3D,_tmp3D,_tmp3D + 12};static struct
_tuple1 Cyc_Toc__pop_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)((
void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char
_tmp3F[14]="_reset_region";static struct _tagged_arr Cyc_Toc__reset_region_str={
_tmp3F,_tmp3F,_tmp3F + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)((void*)& Cyc_Toc__reset_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static struct Cyc_Absyn_UnknownAggr_struct Cyc_Toc_tagged_arr_typ_u={
0,(void*)((void*)0),& Cyc_Toc__tagged_arr_pr};static struct Cyc_Absyn_AggrType_struct
Cyc_Toc_tagged_arr_typ_v={10,{(void*)((void*)& Cyc_Toc_tagged_arr_typ_u),0}};
static void*Cyc_Toc_tagged_arr_typ=(void*)& Cyc_Toc_tagged_arr_typ_v;static struct
Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({
struct Cyc_Absyn_Stmt**_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=Cyc_Absyn_skip_stmt(
0);_tmp43;});return*((struct Cyc_Absyn_Stmt**)_check_null(skip_stmt_opt));}struct
_tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp44=(void*)e->r;void*_tmp45;char _tmp46;void*
_tmp47;short _tmp48;void*_tmp49;int _tmp4A;void*_tmp4B;long long _tmp4C;void*_tmp4D;
struct Cyc_Absyn_Exp*_tmp4E;struct Cyc_List_List*_tmp4F;struct Cyc_List_List*_tmp50;
struct Cyc_List_List*_tmp51;struct Cyc_List_List*_tmp52;struct Cyc_List_List*_tmp53;
_LL1: if(*((int*)_tmp44)!= 0)goto _LL3;_tmp45=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp44)->f1;if(_tmp45 <= (void*)1?1:*((int*)_tmp45)!= 0)goto _LL3;_tmp46=((struct
Cyc_Absyn_Char_c_struct*)_tmp45)->f2;_LL2: return _tmp46 == '\000';_LL3: if(*((int*)
_tmp44)!= 0)goto _LL5;_tmp47=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp44)->f1;
if(_tmp47 <= (void*)1?1:*((int*)_tmp47)!= 1)goto _LL5;_tmp48=((struct Cyc_Absyn_Short_c_struct*)
_tmp47)->f2;_LL4: return _tmp48 == 0;_LL5: if(*((int*)_tmp44)!= 0)goto _LL7;_tmp49=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp44)->f1;if(_tmp49 <= (void*)1?1:*((
int*)_tmp49)!= 2)goto _LL7;_tmp4A=((struct Cyc_Absyn_Int_c_struct*)_tmp49)->f2;
_LL6: return _tmp4A == 0;_LL7: if(*((int*)_tmp44)!= 0)goto _LL9;_tmp4B=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp44)->f1;if(_tmp4B <= (void*)1?1:*((int*)
_tmp4B)!= 3)goto _LL9;_tmp4C=((struct Cyc_Absyn_LongLong_c_struct*)_tmp4B)->f2;
_LL8: return _tmp4C == 0;_LL9: if(*((int*)_tmp44)!= 0)goto _LLB;_tmp4D=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp44)->f1;if((int)_tmp4D != 0)goto _LLB;_LLA:
return 1;_LLB: if(*((int*)_tmp44)!= 13)goto _LLD;_tmp4E=((struct Cyc_Absyn_Cast_e_struct*)
_tmp44)->f2;_LLC: return Cyc_Toc_is_zero(_tmp4E);_LLD: if(*((int*)_tmp44)!= 24)goto
_LLF;_tmp4F=((struct Cyc_Absyn_Tuple_e_struct*)_tmp44)->f1;_LLE: return((int(*)(
int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp4F);_LLF: if(*((int*)_tmp44)!= 26)goto _LL11;_tmp50=((struct Cyc_Absyn_Array_e_struct*)
_tmp44)->f1;_LL10: _tmp51=_tmp50;goto _LL12;_LL11: if(*((int*)_tmp44)!= 28)goto
_LL13;_tmp51=((struct Cyc_Absyn_Struct_e_struct*)_tmp44)->f3;_LL12: _tmp52=_tmp51;
goto _LL14;_LL13: if(*((int*)_tmp44)!= 25)goto _LL15;_tmp52=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp44)->f2;_LL14: _tmp53=_tmp52;goto _LL16;_LL15: if(*((int*)_tmp44)!= 34)goto
_LL17;_tmp53=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp44)->f2;_LL16: for(0;
_tmp53 != 0;_tmp53=_tmp53->tl){if(!Cyc_Toc_is_zero((*((struct _tuple6*)_tmp53->hd)).f2))
return 0;}return 1;_LL17:;_LL18: return 0;_LL0:;}static int Cyc_Toc_is_nullable(void*t){
void*_tmp54=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp55;struct Cyc_Absyn_Conref*
_tmp56;_LL1A: if(_tmp54 <= (void*)3?1:*((int*)_tmp54)!= 4)goto _LL1C;_tmp55=((
struct Cyc_Absyn_PointerType_struct*)_tmp54)->f1;_tmp56=_tmp55.nullable;_LL1B:
return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp56);
_LL1C:;_LL1D:({void*_tmp57[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Toc_toc_impos)(_tag_arr("is_nullable",sizeof(char),12),_tag_arr(_tmp57,
sizeof(void*),0));});_LL19:;}static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _tagged_arr tag){return({struct _tuple1*_tmp58=_cycalloc(
sizeof(*_tmp58));_tmp58->f1=(*x).f1;_tmp58->f2=({struct _tagged_arr*_tmp59=
_cycalloc(sizeof(*_tmp59));_tmp59[0]=(struct _tagged_arr)Cyc_Std_strconcat(*(*x).f2,
tag);_tmp59;});_tmp58;});}struct _tuple7{struct _tagged_arr*f1;struct Cyc_List_List*
f2;};static struct _tagged_arr*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){{
struct Cyc_List_List*_tmp5A=Cyc_Toc_tuple_types;for(0;_tmp5A != 0;_tmp5A=_tmp5A->tl){
struct _tuple7 _tmp5C;struct _tagged_arr*_tmp5D;struct Cyc_List_List*_tmp5E;struct
_tuple7*_tmp5B=(struct _tuple7*)_tmp5A->hd;_tmp5C=*_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=
_tmp5C.f2;{struct Cyc_List_List*_tmp5F=tqs0;for(0;_tmp5F != 0?_tmp5E != 0: 0;(_tmp5F=
_tmp5F->tl,_tmp5E=_tmp5E->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)_tmp5F->hd)).f2,(
void*)_tmp5E->hd))break;}if(_tmp5F == 0?_tmp5E == 0: 0)return _tmp5D;}}}{struct
_tagged_arr*x=({struct _tagged_arr*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=(
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp6E;_tmp6E.tag=1;_tmp6E.f1=(
int)((unsigned int)Cyc_Toc_tuple_type_counter ++);{void*_tmp6D[1]={& _tmp6E};Cyc_Std_aprintf(
_tag_arr("_tuple%d",sizeof(char),9),_tag_arr(_tmp6D,sizeof(void*),1));}});_tmp6C;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple4*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct Cyc_List_List*
_tmp60=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmp60=({struct Cyc_List_List*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->hd=({
struct Cyc_Absyn_Aggrfield*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->name=Cyc_Absyn_fieldname(
i);_tmp62->tq=Cyc_Toc_mt_tq;_tmp62->type=(void*)((void*)ts2->hd);_tmp62->width=0;
_tmp62->attributes=0;_tmp62;});_tmp61->tl=_tmp60;_tmp61;});}}_tmp60=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp60);{struct Cyc_Absyn_Aggrdecl*
_tmp63=({struct Cyc_Absyn_Aggrdecl*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->kind=(
void*)((void*)0);_tmp69->sc=(void*)((void*)2);_tmp69->name=({struct _tuple1*
_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->f1=Cyc_Absyn_rel_ns_null;_tmp6B->f2=x;
_tmp6B;});_tmp69->tvs=0;_tmp69->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp6A=
_cycalloc(sizeof(*_tmp6A));_tmp6A->exist_vars=0;_tmp6A->rgn_po=0;_tmp6A->fields=
_tmp60;_tmp6A;});_tmp69->attributes=0;_tmp69;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->hd=Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Aggr_d_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp66;_tmp66.tag=4;_tmp66.f1=_tmp63;_tmp66;});_tmp65;}),0);_tmp64->tl=Cyc_Toc_result_decls;
_tmp64;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*
_tmp67));_tmp67->hd=({struct _tuple7*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->f1=
x;_tmp68->f2=ts;_tmp68;});_tmp67->tl=Cyc_Toc_tuple_types;_tmp67;});return x;}}}
struct _tuple1*Cyc_Toc_temp_var(){return({struct _tuple1*_tmp6F=_cycalloc(sizeof(*
_tmp6F));_tmp6F->f1=(void*)0;_tmp6F->f2=({struct _tagged_arr*_tmp70=_cycalloc(
sizeof(*_tmp70));_tmp70[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp72;_tmp72.tag=1;_tmp72.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*
_tmp71[1]={& _tmp72};Cyc_Std_aprintf(_tag_arr("_tmp%X",sizeof(char),7),_tag_arr(
_tmp71,sizeof(void*),1));}});_tmp70;});_tmp6F;});}static struct _tagged_arr*Cyc_Toc_fresh_label(){
return({struct _tagged_arr*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73[0]=(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp75;_tmp75.tag=1;_tmp75.f1=(
unsigned int)Cyc_Toc_fresh_label_counter ++;{void*_tmp74[1]={& _tmp75};Cyc_Std_aprintf(
_tag_arr("_LL%X",sizeof(char),6),_tag_arr(_tmp74,sizeof(void*),1));}});_tmp73;});}
static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*td,
struct _tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmp76=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(
name,((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp76))->hd)->name)
!= 0){if((carries_value?((struct Cyc_Absyn_Tunionfield*)_tmp76->hd)->typs != 0: 0)?
1:(!carries_value?((struct Cyc_Absyn_Tunionfield*)_tmp76->hd)->typs == 0: 0))++ ans;
_tmp76=_tmp76->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static int Cyc_Toc_num_void_tags(
struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*_tmp77=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;_tmp77 != 0;_tmp77=_tmp77->tl){
if(((struct Cyc_Absyn_Tunionfield*)_tmp77->hd)->typs == 0)++ ans;}}return ans;}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*Cyc_Toc_arg_to_c(struct
_tuple2*a){struct Cyc_Core_Opt*_tmp79;struct Cyc_Absyn_Tqual _tmp7A;void*_tmp7B;
struct _tuple2 _tmp78=*a;_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;_tmp7B=_tmp78.f3;return({
struct _tuple2*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=_tmp79;_tmp7C->f2=
_tmp7A;_tmp7C->f3=Cyc_Toc_typ_to_c(_tmp7B);_tmp7C;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(
struct _tuple4*x){struct Cyc_Absyn_Tqual _tmp7E;void*_tmp7F;struct _tuple4 _tmp7D=*x;
_tmp7E=_tmp7D.f1;_tmp7F=_tmp7D.f2;return({struct _tuple4*_tmp80=_cycalloc(sizeof(*
_tmp80));_tmp80->f1=_tmp7E;_tmp80->f2=Cyc_Toc_typ_to_c(_tmp7F);_tmp80;});}static
void*Cyc_Toc_typ_to_c_array(void*t){void*_tmp81=t;void*_tmp82;struct Cyc_Absyn_Tqual
_tmp83;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_Core_Opt*_tmp85;struct Cyc_Core_Opt
_tmp86;void*_tmp87;_LL1F: if(_tmp81 <= (void*)3?1:*((int*)_tmp81)!= 7)goto _LL21;
_tmp82=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp81)->f1;_tmp83=((struct Cyc_Absyn_ArrayType_struct*)
_tmp81)->f2;_tmp84=((struct Cyc_Absyn_ArrayType_struct*)_tmp81)->f3;_LL20: return(
void*)({struct Cyc_Absyn_ArrayType_struct*_tmp88=_cycalloc(sizeof(*_tmp88));
_tmp88[0]=({struct Cyc_Absyn_ArrayType_struct _tmp89;_tmp89.tag=7;_tmp89.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp82);_tmp89.f2=_tmp83;_tmp89.f3=_tmp84;_tmp89;});
_tmp88;});_LL21: if(_tmp81 <= (void*)3?1:*((int*)_tmp81)!= 0)goto _LL23;_tmp85=((
struct Cyc_Absyn_Evar_struct*)_tmp81)->f2;if(_tmp85 == 0)goto _LL23;_tmp86=*_tmp85;
_tmp87=(void*)_tmp86.v;_LL22: return Cyc_Toc_typ_to_c_array(_tmp87);_LL23:;_LL24:
return Cyc_Toc_typ_to_c(t);_LL1E:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(
struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*_tmp8A=_cycalloc(
sizeof(*_tmp8A));_tmp8A->name=f->name;_tmp8A->tq=f->tq;_tmp8A->type=(void*)Cyc_Toc_typ_to_c((
void*)f->type);_tmp8A->width=f->width;_tmp8A->attributes=f->attributes;_tmp8A;});}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_typ_to_c(
void*t){void*_tmp8B=t;struct Cyc_Core_Opt*_tmp8C;struct Cyc_Core_Opt*_tmp8D;struct
Cyc_Core_Opt _tmp8E;void*_tmp8F;struct Cyc_Absyn_Tvar*_tmp90;struct Cyc_Absyn_TunionInfo
_tmp91;void*_tmp92;struct Cyc_Absyn_TunionFieldInfo _tmp93;void*_tmp94;struct Cyc_Absyn_Tuniondecl*
_tmp95;struct Cyc_Absyn_Tunionfield*_tmp96;struct Cyc_Absyn_PtrInfo _tmp97;void*
_tmp98;struct Cyc_Absyn_Tqual _tmp99;struct Cyc_Absyn_Conref*_tmp9A;void*_tmp9B;
struct Cyc_Absyn_Tqual _tmp9C;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_FnInfo
_tmp9E;void*_tmp9F;struct Cyc_List_List*_tmpA0;int _tmpA1;struct Cyc_Absyn_VarargInfo*
_tmpA2;struct Cyc_List_List*_tmpA3;struct Cyc_List_List*_tmpA4;void*_tmpA5;struct
Cyc_List_List*_tmpA6;struct Cyc_Absyn_AggrInfo _tmpA7;void*_tmpA8;struct Cyc_List_List*
_tmpA9;struct _tuple1*_tmpAA;struct Cyc_List_List*_tmpAB;struct _tuple1*_tmpAC;
struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Typedefdecl*_tmpAE;struct Cyc_Core_Opt*
_tmpAF;void*_tmpB0;_LL26: if((int)_tmp8B != 0)goto _LL28;_LL27: return t;_LL28: if(
_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 0)goto _LL2A;_tmp8C=((struct Cyc_Absyn_Evar_struct*)
_tmp8B)->f2;if(_tmp8C != 0)goto _LL2A;_LL29: return Cyc_Absyn_sint_typ;_LL2A: if(
_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 0)goto _LL2C;_tmp8D=((struct Cyc_Absyn_Evar_struct*)
_tmp8B)->f2;if(_tmp8D == 0)goto _LL2C;_tmp8E=*_tmp8D;_tmp8F=(void*)_tmp8E.v;_LL2B:
return Cyc_Toc_typ_to_c(_tmp8F);_LL2C: if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 1)
goto _LL2E;_tmp90=((struct Cyc_Absyn_VarType_struct*)_tmp8B)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(
_tmp90)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL2E:
if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 2)goto _LL30;_tmp91=((struct Cyc_Absyn_TunionType_struct*)
_tmp8B)->f1;_tmp92=(void*)_tmp91.tunion_info;if(*((int*)_tmp92)!= 1)goto _LL30;
_LL2F: return Cyc_Absyn_void_star_typ();_LL30: if(_tmp8B <= (void*)3?1:*((int*)
_tmp8B)!= 2)goto _LL32;_LL31:({void*_tmpB1[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unresolved TunionType",sizeof(
char),22),_tag_arr(_tmpB1,sizeof(void*),0));});_LL32: if(_tmp8B <= (void*)3?1:*((
int*)_tmp8B)!= 3)goto _LL34;_tmp93=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp8B)->f1;_tmp94=(void*)_tmp93.field_info;if(*((int*)_tmp94)!= 1)goto _LL34;
_tmp95=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp94)->f1;_tmp96=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp94)->f2;_LL33: if(_tmp96->typs == 0){if(_tmp95->is_xtunion)return Cyc_Absyn_star_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Toc_mt_tq);else{return Cyc_Absyn_uint_typ;}}else{
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp96->name,_tag_arr("_struct",
sizeof(char),8)));}_LL34: if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 3)goto _LL36;
_LL35:({void*_tmpB2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unresolved TunionFieldType",sizeof(char),27),_tag_arr(_tmpB2,sizeof(
void*),0));});_LL36: if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 4)goto _LL38;_tmp97=((
struct Cyc_Absyn_PointerType_struct*)_tmp8B)->f1;_tmp98=(void*)_tmp97.elt_typ;
_tmp99=_tmp97.tq;_tmp9A=_tmp97.bounds;_LL37: _tmp98=Cyc_Toc_typ_to_c_array(_tmp98);{
void*_tmpB3=(void*)(Cyc_Absyn_compress_conref(_tmp9A))->v;void*_tmpB4;_LL5F: if(
_tmpB3 <= (void*)1?1:*((int*)_tmpB3)!= 0)goto _LL61;_tmpB4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpB3)->f1;if((int)_tmpB4 != 0)goto _LL61;_LL60: return Cyc_Toc_tagged_arr_typ;
_LL61:;_LL62: return Cyc_Absyn_star_typ(_tmp98,(void*)2,_tmp99);_LL5E:;}_LL38: if(
_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)
_tmp8B != 1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)
!= 6)goto _LL3E;_LL3D: return t;_LL3E: if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 7)
goto _LL40;_tmp9B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp8B)->f1;_tmp9C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp8B)->f2;_tmp9D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp8B)->f3;_LL3F: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpB5=
_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_ArrayType_struct _tmpB6;
_tmpB6.tag=7;_tmpB6.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp9B);_tmpB6.f2=_tmp9C;
_tmpB6.f3=_tmp9D;_tmpB6;});_tmpB5;});_LL40: if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)
!= 8)goto _LL42;_tmp9E=((struct Cyc_Absyn_FnType_struct*)_tmp8B)->f1;_tmp9F=(void*)
_tmp9E.ret_typ;_tmpA0=_tmp9E.args;_tmpA1=_tmp9E.c_varargs;_tmpA2=_tmp9E.cyc_varargs;
_tmpA3=_tmp9E.attributes;_LL41: {struct Cyc_List_List*_tmpB7=0;for(0;_tmpA3 != 0;
_tmpA3=_tmpA3->tl){void*_tmpB8=(void*)_tmpA3->hd;_LL64: if((int)_tmpB8 != 3)goto
_LL66;_LL65: goto _LL67;_LL66: if((int)_tmpB8 != 4)goto _LL68;_LL67: goto _LL69;_LL68:
if(_tmpB8 <= (void*)16?1:*((int*)_tmpB8)!= 3)goto _LL6A;_LL69: continue;_LL6A:;
_LL6B: _tmpB7=({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->hd=(
void*)((void*)_tmpA3->hd);_tmpB9->tl=_tmpB7;_tmpB9;});goto _LL63;_LL63:;}{struct
Cyc_List_List*_tmpBA=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmpA0);if(_tmpA2 != 0){
void*_tmpBB=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ((void*)_tmpA2->type,(void*)2,
Cyc_Toc_mt_tq));struct _tuple2*_tmpBC=({struct _tuple2*_tmpBE=_cycalloc(sizeof(*
_tmpBE));_tmpBE->f1=_tmpA2->name;_tmpBE->f2=_tmpA2->tq;_tmpBE->f3=_tmpBB;_tmpBE;});
_tmpBA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmpBA,({struct Cyc_List_List*_tmpBD=_cycalloc(sizeof(*
_tmpBD));_tmpBD->hd=_tmpBC;_tmpBD->tl=0;_tmpBD;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_FnType_struct
_tmpC0;_tmpC0.tag=8;_tmpC0.f1=({struct Cyc_Absyn_FnInfo _tmpC1;_tmpC1.tvars=0;
_tmpC1.effect=0;_tmpC1.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp9F);_tmpC1.args=
_tmpBA;_tmpC1.c_varargs=_tmpA1;_tmpC1.cyc_varargs=0;_tmpC1.rgn_po=0;_tmpC1.attributes=
_tmpB7;_tmpC1;});_tmpC0;});_tmpBF;});}}_LL42: if(_tmp8B <= (void*)3?1:*((int*)
_tmp8B)!= 9)goto _LL44;_tmpA4=((struct Cyc_Absyn_TupleType_struct*)_tmp8B)->f1;
_LL43: _tmpA4=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmpA4);{struct _tagged_arr*
_tmpC2=Cyc_Toc_add_tuple_type(_tmpA4);return Cyc_Absyn_strct(_tmpC2);}_LL44: if(
_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 11)goto _LL46;_tmpA5=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp8B)->f1;_tmpA6=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp8B)->f2;_LL45:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpC3=_cycalloc(sizeof(*
_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpC4;_tmpC4.tag=11;
_tmpC4.f1=(void*)_tmpA5;_tmpC4.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmpA6);_tmpC4;});_tmpC3;});_LL46: if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 10)
goto _LL48;_tmpA7=((struct Cyc_Absyn_AggrType_struct*)_tmp8B)->f1;_tmpA8=(void*)
_tmpA7.aggr_info;_tmpA9=_tmpA7.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*_tmpC5=Cyc_Absyn_get_known_aggrdecl(
_tmpA8);if((void*)_tmpC5->kind == (void*)1)return Cyc_Absyn_unionq_typ(_tmpC5->name);
return Cyc_Absyn_strctq(_tmpC5->name);}_LL48: if(_tmp8B <= (void*)3?1:*((int*)
_tmp8B)!= 12)goto _LL4A;_tmpAA=((struct Cyc_Absyn_EnumType_struct*)_tmp8B)->f1;
_LL49: return t;_LL4A: if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 13)goto _LL4C;_tmpAB=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp8B)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmpAB);return t;_LL4C: if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 16)goto _LL4E;
_tmpAC=((struct Cyc_Absyn_TypedefType_struct*)_tmp8B)->f1;_tmpAD=((struct Cyc_Absyn_TypedefType_struct*)
_tmp8B)->f2;_tmpAE=((struct Cyc_Absyn_TypedefType_struct*)_tmp8B)->f3;_tmpAF=((
struct Cyc_Absyn_TypedefType_struct*)_tmp8B)->f4;_LL4D: if(_tmpAF == 0)return(void*)({
struct Cyc_Absyn_TypedefType_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({
struct Cyc_Absyn_TypedefType_struct _tmpC7;_tmpC7.tag=16;_tmpC7.f1=_tmpAC;_tmpC7.f2=
0;_tmpC7.f3=_tmpAE;_tmpC7.f4=0;_tmpC7;});_tmpC6;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpC9;_tmpC9.tag=16;_tmpC9.f1=_tmpAC;_tmpC9.f2=0;_tmpC9.f3=_tmpAE;_tmpC9.f4=({
struct Cyc_Core_Opt*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->v=(void*)Cyc_Toc_typ_to_c_array((
void*)_tmpAF->v);_tmpCA;});_tmpC9;});_tmpC8;});}_LL4E: if(_tmp8B <= (void*)3?1:*((
int*)_tmp8B)!= 14)goto _LL50;_LL4F: return Cyc_Absyn_uint_typ;_LL50: if(_tmp8B <= (
void*)3?1:*((int*)_tmp8B)!= 17)goto _LL52;_LL51: return Cyc_Absyn_sint_typ;_LL52:
if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 15)goto _LL54;_tmpB0=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp8B)->f1;_LL53: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL54: if(_tmp8B <= (void*)3?1:*((int*)_tmp8B)!= 18)goto _LL56;_LL55:({
void*_tmpCB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::typ_to_c: type translation passed a type integer",sizeof(char),54),
_tag_arr(_tmpCB,sizeof(void*),0));});_LL56: if((int)_tmp8B != 2)goto _LL58;_LL57:({
void*_tmpCC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::typ_to_c: type translation passed the heap region",sizeof(char),
55),_tag_arr(_tmpCC,sizeof(void*),0));});_LL58: if(_tmp8B <= (void*)3?1:*((int*)
_tmp8B)!= 19)goto _LL5A;_LL59:({void*_tmpCD[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof(char),54),_tag_arr(_tmpCD,sizeof(void*),0));});_LL5A: if(_tmp8B <= (void*)3?
1:*((int*)_tmp8B)!= 20)goto _LL5C;_LL5B:({void*_tmpCE[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof(char),52),_tag_arr(_tmpCE,sizeof(void*),0));});_LL5C: if(_tmp8B <= (void*)3?
1:*((int*)_tmp8B)!= 21)goto _LL25;_LL5D:({void*_tmpCF[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof(char),55),_tag_arr(_tmpCF,sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmpD0=t;void*_tmpD1;struct Cyc_Absyn_Tqual _tmpD2;_LL6D: if(_tmpD0 <= (void*)
3?1:*((int*)_tmpD0)!= 7)goto _LL6F;_tmpD1=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmpD0)->f1;_tmpD2=((struct Cyc_Absyn_ArrayType_struct*)_tmpD0)->f2;_LL6E: return
Cyc_Absyn_cast_exp(Cyc_Absyn_star_typ(_tmpD1,(void*)2,_tmpD2),e,l);_LL6F:;_LL70:
return Cyc_Absyn_cast_exp(t,e,l);_LL6C:;}static int Cyc_Toc_atomic_typ(void*t){void*
_tmpD3=Cyc_Tcutil_compress(t);void*_tmpD4;struct Cyc_Absyn_AggrInfo _tmpD5;void*
_tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_Absyn_TunionFieldInfo _tmpD8;void*
_tmpD9;struct Cyc_Absyn_Tuniondecl*_tmpDA;struct Cyc_Absyn_Tunionfield*_tmpDB;
struct Cyc_List_List*_tmpDC;_LL72: if((int)_tmpD3 != 0)goto _LL74;_LL73: return 1;
_LL74: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 1)goto _LL76;_LL75: return 0;_LL76:
if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 5)goto _LL78;_LL77: goto _LL79;_LL78: if(
_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 12)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(
_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 13)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if((int)
_tmpD3 != 1)goto _LL7E;_LL7D: goto _LL7F;_LL7E: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)
!= 6)goto _LL80;_LL7F: goto _LL81;_LL80: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 8)
goto _LL82;_LL81: goto _LL83;_LL82: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 17)goto
_LL84;_LL83: goto _LL85;_LL84: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 14)goto
_LL86;_LL85: return 1;_LL86: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 7)goto _LL88;
_tmpD4=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpD3)->f1;_LL87: return Cyc_Toc_atomic_typ(
_tmpD4);_LL88: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 10)goto _LL8A;_tmpD5=((
struct Cyc_Absyn_AggrType_struct*)_tmpD3)->f1;_tmpD6=(void*)_tmpD5.aggr_info;
_LL89:{void*_tmpDD=_tmpD6;_LL99: if(*((int*)_tmpDD)!= 0)goto _LL9B;_LL9A: return 0;
_LL9B:;_LL9C: goto _LL98;_LL98:;}{struct Cyc_Absyn_Aggrdecl*_tmpDE=Cyc_Absyn_get_known_aggrdecl(
_tmpD6);if(_tmpDE->impl == 0)return 0;{struct Cyc_List_List*_tmpDF=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpDE->impl))->fields;for(0;_tmpDF != 0;_tmpDF=_tmpDF->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpDF->hd)->type))return 0;}}return 1;}_LL8A:
if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 11)goto _LL8C;_tmpD7=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpD3)->f2;_LL8B: for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpD7->hd)->type))return 0;}return 1;_LL8C: if(
_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 3)goto _LL8E;_tmpD8=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpD3)->f1;_tmpD9=(void*)_tmpD8.field_info;if(*((int*)_tmpD9)!= 1)goto _LL8E;
_tmpDA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpD9)->f1;_tmpDB=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpD9)->f2;_LL8D: _tmpDC=_tmpDB->typs;goto _LL8F;_LL8E: if(_tmpD3 <= (void*)3?1:*((
int*)_tmpD3)!= 9)goto _LL90;_tmpDC=((struct Cyc_Absyn_TupleType_struct*)_tmpD3)->f1;
_LL8F: for(0;_tmpDC != 0;_tmpDC=_tmpDC->tl){if(!Cyc_Toc_atomic_typ((*((struct
_tuple4*)_tmpDC->hd)).f2))return 0;}return 1;_LL90: if(_tmpD3 <= (void*)3?1:*((int*)
_tmpD3)!= 2)goto _LL92;_LL91: goto _LL93;_LL92: if(_tmpD3 <= (void*)3?1:*((int*)
_tmpD3)!= 4)goto _LL94;_LL93: goto _LL95;_LL94: if(_tmpD3 <= (void*)3?1:*((int*)
_tmpD3)!= 15)goto _LL96;_LL95: return 0;_LL96:;_LL97:({struct Cyc_Std_String_pa_struct
_tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpE0[1]={& _tmpE1};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("atomic_typ:  bad type %s",sizeof(char),25),_tag_arr(_tmpE0,sizeof(void*),
1));}});_LL71:;}static int Cyc_Toc_is_void_star(void*t){void*_tmpE2=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE3;void*_tmpE4;_LL9E: if(_tmpE2 <= (void*)3?1:*((int*)
_tmpE2)!= 4)goto _LLA0;_tmpE3=((struct Cyc_Absyn_PointerType_struct*)_tmpE2)->f1;
_tmpE4=(void*)_tmpE3.elt_typ;_LL9F: {void*_tmpE5=Cyc_Tcutil_compress(_tmpE4);
_LLA3: if((int)_tmpE5 != 0)goto _LLA5;_LLA4: return 1;_LLA5:;_LLA6: return 0;_LLA2:;}
_LLA0:;_LLA1: return 0;_LL9D:;}static int Cyc_Toc_is_poly_field(void*t,struct
_tagged_arr*f){void*_tmpE6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmpE7;
void*_tmpE8;struct Cyc_List_List*_tmpE9;_LLA8: if(_tmpE6 <= (void*)3?1:*((int*)
_tmpE6)!= 10)goto _LLAA;_tmpE7=((struct Cyc_Absyn_AggrType_struct*)_tmpE6)->f1;
_tmpE8=(void*)_tmpE7.aggr_info;_LLA9: {struct Cyc_Absyn_Aggrdecl*_tmpEA=Cyc_Absyn_get_known_aggrdecl(
_tmpE8);if(_tmpEA->impl == 0)({void*_tmpEB[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: type missing fields",
sizeof(char),35),_tag_arr(_tmpEB,sizeof(void*),0));});_tmpE9=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpEA->impl))->fields;goto _LLAB;}_LLAA: if(_tmpE6 <= (void*)3?1:*((
int*)_tmpE6)!= 11)goto _LLAC;_tmpE9=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpE6)->f2;
_LLAB: {struct Cyc_Absyn_Aggrfield*_tmpEC=Cyc_Absyn_lookup_field(_tmpE9,f);if(
_tmpEC == 0)({struct Cyc_Std_String_pa_struct _tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct
_tagged_arr)*f;{void*_tmpED[1]={& _tmpEE};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: bad field %s",sizeof(
char),28),_tag_arr(_tmpED,sizeof(void*),1));}});return Cyc_Toc_is_void_star((void*)
_tmpEC->type);}_LLAC:;_LLAD:({struct Cyc_Std_String_pa_struct _tmpF0;_tmpF0.tag=0;
_tmpF0.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpEF[1]={& _tmpF0};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: bad type %s",
sizeof(char),27),_tag_arr(_tmpEF,sizeof(void*),1));}});_LLA7:;}static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp*e){void*_tmpF1=(void*)e->r;struct Cyc_Absyn_Exp*_tmpF2;struct
_tagged_arr*_tmpF3;struct Cyc_Absyn_Exp*_tmpF4;struct _tagged_arr*_tmpF5;_LLAF: if(*((
int*)_tmpF1)!= 21)goto _LLB1;_tmpF2=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpF1)->f1;
_tmpF3=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpF1)->f2;_LLB0: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF2->topt))->v,_tmpF3);_LLB1: if(*((
int*)_tmpF1)!= 22)goto _LLB3;_tmpF4=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpF1)->f1;
_tmpF5=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpF1)->f2;_LLB2: {void*_tmpF6=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF4->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF7;void*_tmpF8;_LLB6: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 4)goto _LLB8;
_tmpF7=((struct Cyc_Absyn_PointerType_struct*)_tmpF6)->f1;_tmpF8=(void*)_tmpF7.elt_typ;
_LLB7: return Cyc_Toc_is_poly_field(_tmpF8,_tmpF5);_LLB8:;_LLB9:({struct Cyc_Std_String_pa_struct
_tmpFA;_tmpFA.tag=0;_tmpFA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF4->topt))->v);{void*_tmpF9[1]={& _tmpFA};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_project:  bad type %s",
sizeof(char),30),_tag_arr(_tmpF9,sizeof(void*),1));}});_LLB5:;}_LLB3:;_LLB4:
return 0;_LLAE:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmpFB=
_cycalloc(sizeof(*_tmpFB));_tmpFB->hd=s;_tmpFB->tl=0;_tmpFB;}),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmpFC=_cycalloc(sizeof(*_tmpFC));
_tmpFC->hd=s;_tmpFC->tl=0;_tmpFC;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({
struct Cyc_Absyn_Exp*_tmpFD[2];_tmpFD[1]=s;_tmpFD[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFD,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmpFE[2];_tmpFE[1]=n;_tmpFE[0]=
s;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFE,
sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({
struct Cyc_Absyn_Exp*_tmpFF[2];_tmpFF[1]=n;_tmpFF[0]=s;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmpFF,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,
struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({
struct Cyc_Absyn_Exp*_tmp100[3];_tmp100[2]=n;_tmp100[1]=s;_tmp100[0]=rgn;((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp100,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*
_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->hd=e;_tmp101->tl=0;_tmp101;}),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_tagged_arr(void*t,struct Cyc_Absyn_Exp*
sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*_tmp102=(void*)e->r;void*_tmp103;
_LLBB: if(*((int*)_tmp102)!= 0)goto _LLBD;_tmp103=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp102)->f1;if(_tmp103 <= (void*)1?1:*((int*)_tmp103)!= 5)goto _LLBD;_LLBC:
is_string=1;goto _LLBA;_LLBD:;_LLBE: goto _LLBA;_LLBA:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp107=_cycalloc(sizeof(*
_tmp107));_tmp107[0]=({struct Cyc_Absyn_ArrayType_struct _tmp108;_tmp108.tag=7;
_tmp108.f1=(void*)Cyc_Absyn_char_typ;_tmp108.f2=Cyc_Toc_mt_tq;_tmp108.f3=(struct
Cyc_Absyn_Exp*)sz;_tmp108;});_tmp107;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[0]=({
struct Cyc_Absyn_Var_d_struct _tmp106;_tmp106.tag=0;_tmp106.f1=vd;_tmp106;});
_tmp105;}),0);_tmp104->tl=Cyc_Toc_result_decls;_tmp104;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
e,0);xplussz=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,
0),0);}{struct Cyc_Absyn_Exp*urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple6*_tmp109[3];_tmp109[2]=({struct _tuple6*_tmp10C=_cycalloc(sizeof(*_tmp10C));
_tmp10C->f1=0;_tmp10C->f2=xplussz;_tmp10C;});_tmp109[1]=({struct _tuple6*_tmp10B=
_cycalloc(sizeof(*_tmp10B));_tmp10B->f1=0;_tmp10B->f2=xexp;_tmp10B;});_tmp109[0]=({
struct _tuple6*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->f1=0;_tmp10A->f2=xexp;
_tmp10A;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp109,sizeof(struct _tuple6*),3));}),0);return urm_exp;}}}struct Cyc_Toc_Env{
struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;struct Cyc_Core_Opt*
fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp10D=_cycalloc(sizeof(*_tmp10D));
_tmp10D->break_lab=0;_tmp10D->continue_lab=0;_tmp10D->fallthru_info=0;_tmp10D->varmap=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp10D->toplevel=1;_tmp10D;});}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp10E=_cycalloc(
sizeof(*_tmp10E));_tmp10E[0]=*e;_tmp10E;});ans->toplevel=0;return ans;}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct Cyc_Toc_Env*e,struct _tuple1*x,struct
Cyc_Absyn_Exp*y){{void*_tmp10F=(*x).f1;_LLC0: if(_tmp10F <= (void*)1?1:*((int*)
_tmp10F)!= 0)goto _LLC2;_LLC1:({struct Cyc_Std_String_pa_struct _tmp111;_tmp111.tag=
0;_tmp111.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(x);{void*_tmp110[1]={&
_tmp111};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::add_varmap on Rel_n: %s\n",sizeof(char),30),_tag_arr(_tmp110,
sizeof(void*),1));}});_LLC2:;_LLC3: goto _LLBF;_LLBF:;}{struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=*e;_tmp112;});
ans->varmap=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,
struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(ans->varmap,x,y);return ans;}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=*e;_tmp113;});
ans->break_lab=0;ans->continue_lab=0;return ans;}struct _tuple8{struct _tagged_arr*
f1;struct Cyc_List_List*f2;struct Cyc_Dict_Dict*f3;};static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*break_l,struct _tagged_arr*fallthru_l,
struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*
_tmp114=0;for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
_tmp114=({struct Cyc_List_List*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->hd=((
struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name;_tmp115->tl=_tmp114;_tmp115;});}
_tmp114=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp114);{struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp119=_cycalloc(sizeof(*
_tmp119));_tmp119[0]=*e;_tmp119;});ans->break_lab=({struct Cyc_Core_Opt*_tmp116=
_cycalloc(sizeof(*_tmp116));_tmp116->v=break_l;_tmp116;});ans->fallthru_info=({
struct Cyc_Core_Opt*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->v=({struct
_tuple8*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->f1=fallthru_l;_tmp118->f2=
_tmp114;_tmp118->f3=next_case_env->varmap;_tmp118;});_tmp117;});return ans;}}
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct Cyc_Toc_Env*e,struct
_tagged_arr*break_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp11B=_cycalloc(
sizeof(*_tmp11B));_tmp11B[0]=*e;_tmp11B;});ans->break_lab=({struct Cyc_Core_Opt*
_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->v=break_l;_tmp11A;});ans->fallthru_info=
0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct Cyc_Toc_Env*
e,struct _tagged_arr*next_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp11E=
_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=*e;_tmp11E;});ans->break_lab=0;ans->fallthru_info=({
struct Cyc_Core_Opt*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->v=({struct
_tuple8*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->f1=next_l;_tmp11D->f2=0;
_tmp11D->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct _tuple1*)))
Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp11D;});_tmp11C;});return ans;}static void
Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp11F=(void*)e->annot;_LLC5: if(*((void**)_tmp11F)
!= Cyc_CfFlowInfo_UnknownZ)goto _LLC7;_LLC6: return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLC7: if(*((void**)_tmp11F)!= Cyc_CfFlowInfo_NotZero)
goto _LLC9;_LLC8: return 0;_LLC9: if(*((void**)_tmp11F)!= Cyc_CfFlowInfo_IsZero)goto
_LLCB;_LLCA:({void*_tmp120[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(char),28),_tag_arr(_tmp120,sizeof(void*),0));});return 0;_LLCB: if(_tmp11F
!= Cyc_Absyn_EmptyAnnot)goto _LLCD;_LLCC: return 0;_LLCD:;_LLCE:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121[0]=({
struct Cyc_Core_Impossible_struct _tmp122;_tmp122.tag=Cyc_Core_Impossible;_tmp122.f1=
_tag_arr("need_null_check",sizeof(char),16);_tmp122;});_tmp121;}));_LLC4:;}
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp123=(
void*)e->annot;struct Cyc_List_List*_tmp124;struct Cyc_List_List*_tmp125;_LLD0: if(*((
void**)_tmp123)!= Cyc_CfFlowInfo_UnknownZ)goto _LLD2;_tmp124=((struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp123)->f1;_LLD1: return _tmp124;_LLD2: if(*((void**)_tmp123)!= Cyc_CfFlowInfo_NotZero)
goto _LLD4;_tmp125=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmp123)->f1;_LLD3:
return _tmp125;_LLD4: if(*((void**)_tmp123)!= Cyc_CfFlowInfo_IsZero)goto _LLD6;
_LLD5:({void*_tmp126[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(char),28),_tag_arr(_tmp126,sizeof(void*),0));});return 0;_LLD6: if(_tmp123
!= Cyc_Absyn_EmptyAnnot)goto _LLD8;_LLD7: return 0;_LLD8:;_LLD9:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=({
struct Cyc_Core_Impossible_struct _tmp128;_tmp128.tag=Cyc_Core_Impossible;_tmp128.f1=
_tag_arr("get_relns",sizeof(char),10);_tmp128;});_tmp127;}));_LLCF:;}static int
Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp129=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp12A;struct Cyc_Absyn_Conref*_tmp12B;struct Cyc_Absyn_Exp*
_tmp12C;_LLDB: if(_tmp129 <= (void*)3?1:*((int*)_tmp129)!= 4)goto _LLDD;_tmp12A=((
struct Cyc_Absyn_PointerType_struct*)_tmp129)->f1;_tmp12B=_tmp12A.bounds;_LLDC: {
void*_tmp12D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp12B);struct Cyc_Absyn_Exp*
_tmp12E;_LLE2: if((int)_tmp12D != 0)goto _LLE4;_LLE3: return 0;_LLE4: if(_tmp12D <= (
void*)1?1:*((int*)_tmp12D)!= 0)goto _LLE1;_tmp12E=((struct Cyc_Absyn_Upper_b_struct*)
_tmp12D)->f1;_LLE5: {unsigned int _tmp130;int _tmp131;struct _tuple5 _tmp12F=Cyc_Evexp_eval_const_uint_exp(
_tmp12E);_tmp130=_tmp12F.f1;_tmp131=_tmp12F.f2;return _tmp131?i <= _tmp130: 0;}
_LLE1:;}_LLDD: if(_tmp129 <= (void*)3?1:*((int*)_tmp129)!= 7)goto _LLDF;_tmp12C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp129)->f3;_LLDE: if(_tmp12C == 0)return 0;{
unsigned int _tmp133;int _tmp134;struct _tuple5 _tmp132=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp12C));_tmp133=_tmp132.f1;_tmp134=_tmp132.f2;
return _tmp134?i <= _tmp133: 0;}_LLDF:;_LLE0: return 0;_LLDA:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp135=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp135->vd != y)continue;{void*_tmp136=(void*)_tmp135->rop;struct
Cyc_Absyn_Vardecl*_tmp137;struct Cyc_Absyn_Vardecl*_tmp138;_LLE7: if(*((int*)
_tmp136)!= 2)goto _LLE9;_tmp137=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp136)->f1;
_LLE8: _tmp138=_tmp137;goto _LLEA;_LLE9: if(*((int*)_tmp136)!= 4)goto _LLEB;_tmp138=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp136)->f1;_LLEA: if(_tmp138 == v)return
1;else{goto _LLE6;}_LLEB:;_LLEC: continue;_LLE6:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp139=(void*)e->r;void*_tmp13A;struct Cyc_Absyn_Vardecl*_tmp13B;void*_tmp13C;
struct Cyc_Absyn_Vardecl*_tmp13D;void*_tmp13E;struct Cyc_Absyn_Vardecl*_tmp13F;
void*_tmp140;struct Cyc_Absyn_Vardecl*_tmp141;void*_tmp142;struct Cyc_List_List*
_tmp143;struct Cyc_List_List _tmp144;struct Cyc_Absyn_Exp*_tmp145;_LLEE: if(*((int*)
_tmp139)!= 1)goto _LLF0;_tmp13A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp139)->f2;
if(_tmp13A <= (void*)1?1:*((int*)_tmp13A)!= 4)goto _LLF0;_tmp13B=((struct Cyc_Absyn_Pat_b_struct*)
_tmp13A)->f1;_LLEF: _tmp13D=_tmp13B;goto _LLF1;_LLF0: if(*((int*)_tmp139)!= 1)goto
_LLF2;_tmp13C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp139)->f2;if(_tmp13C <= (
void*)1?1:*((int*)_tmp13C)!= 3)goto _LLF2;_tmp13D=((struct Cyc_Absyn_Local_b_struct*)
_tmp13C)->f1;_LLF1: _tmp13F=_tmp13D;goto _LLF3;_LLF2: if(*((int*)_tmp139)!= 1)goto
_LLF4;_tmp13E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp139)->f2;if(_tmp13E <= (
void*)1?1:*((int*)_tmp13E)!= 0)goto _LLF4;_tmp13F=((struct Cyc_Absyn_Global_b_struct*)
_tmp13E)->f1;_LLF3: _tmp141=_tmp13F;goto _LLF5;_LLF4: if(*((int*)_tmp139)!= 1)goto
_LLF6;_tmp140=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp139)->f2;if(_tmp140 <= (
void*)1?1:*((int*)_tmp140)!= 2)goto _LLF6;_tmp141=((struct Cyc_Absyn_Param_b_struct*)
_tmp140)->f1;_LLF5: if(_tmp141->escapes)return 0;if(Cyc_Toc_check_leq_size_var(
relns,v,_tmp141))return 1;goto _LLED;_LLF6: if(*((int*)_tmp139)!= 3)goto _LLF8;
_tmp142=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp139)->f1;if((int)_tmp142 != 
19)goto _LLF8;_tmp143=((struct Cyc_Absyn_Primop_e_struct*)_tmp139)->f2;if(_tmp143
== 0)goto _LLF8;_tmp144=*_tmp143;_tmp145=(struct Cyc_Absyn_Exp*)_tmp144.hd;_LLF7:{
void*_tmp146=(void*)_tmp145->r;void*_tmp147;struct Cyc_Absyn_Vardecl*_tmp148;void*
_tmp149;struct Cyc_Absyn_Vardecl*_tmp14A;void*_tmp14B;struct Cyc_Absyn_Vardecl*
_tmp14C;void*_tmp14D;struct Cyc_Absyn_Vardecl*_tmp14E;_LLFB: if(*((int*)_tmp146)!= 
1)goto _LLFD;_tmp147=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp146)->f2;if(
_tmp147 <= (void*)1?1:*((int*)_tmp147)!= 4)goto _LLFD;_tmp148=((struct Cyc_Absyn_Pat_b_struct*)
_tmp147)->f1;_LLFC: _tmp14A=_tmp148;goto _LLFE;_LLFD: if(*((int*)_tmp146)!= 1)goto
_LLFF;_tmp149=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp146)->f2;if(_tmp149 <= (
void*)1?1:*((int*)_tmp149)!= 3)goto _LLFF;_tmp14A=((struct Cyc_Absyn_Local_b_struct*)
_tmp149)->f1;_LLFE: _tmp14C=_tmp14A;goto _LL100;_LLFF: if(*((int*)_tmp146)!= 1)goto
_LL101;_tmp14B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp146)->f2;if(_tmp14B <= (
void*)1?1:*((int*)_tmp14B)!= 0)goto _LL101;_tmp14C=((struct Cyc_Absyn_Global_b_struct*)
_tmp14B)->f1;_LL100: _tmp14E=_tmp14C;goto _LL102;_LL101: if(*((int*)_tmp146)!= 1)
goto _LL103;_tmp14D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp146)->f2;if(
_tmp14D <= (void*)1?1:*((int*)_tmp14D)!= 2)goto _LL103;_tmp14E=((struct Cyc_Absyn_Param_b_struct*)
_tmp14D)->f1;_LL102: return _tmp14E == v;_LL103:;_LL104: goto _LLFA;_LLFA:;}goto _LLED;
_LLF8:;_LLF9: goto _LLED;_LLED:;}return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp14F=(void*)a->r;
void*_tmp150;struct Cyc_Absyn_Vardecl*_tmp151;void*_tmp152;struct Cyc_Absyn_Vardecl*
_tmp153;void*_tmp154;struct Cyc_Absyn_Vardecl*_tmp155;void*_tmp156;struct Cyc_Absyn_Vardecl*
_tmp157;_LL106: if(*((int*)_tmp14F)!= 1)goto _LL108;_tmp150=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14F)->f2;if(_tmp150 <= (void*)1?1:*((int*)_tmp150)!= 4)goto _LL108;_tmp151=((
struct Cyc_Absyn_Pat_b_struct*)_tmp150)->f1;_LL107: _tmp153=_tmp151;goto _LL109;
_LL108: if(*((int*)_tmp14F)!= 1)goto _LL10A;_tmp152=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14F)->f2;if(_tmp152 <= (void*)1?1:*((int*)_tmp152)!= 3)goto _LL10A;_tmp153=((
struct Cyc_Absyn_Local_b_struct*)_tmp152)->f1;_LL109: _tmp155=_tmp153;goto _LL10B;
_LL10A: if(*((int*)_tmp14F)!= 1)goto _LL10C;_tmp154=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14F)->f2;if(_tmp154 <= (void*)1?1:*((int*)_tmp154)!= 0)goto _LL10C;_tmp155=((
struct Cyc_Absyn_Global_b_struct*)_tmp154)->f1;_LL10B: _tmp157=_tmp155;goto _LL10D;
_LL10C: if(*((int*)_tmp14F)!= 1)goto _LL10E;_tmp156=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14F)->f2;if(_tmp156 <= (void*)1?1:*((int*)_tmp156)!= 2)goto _LL10E;_tmp157=((
struct Cyc_Absyn_Param_b_struct*)_tmp156)->f1;_LL10D: if(_tmp157->escapes)return 0;
inner_loop: {void*_tmp158=(void*)i->r;void*_tmp159;struct Cyc_Absyn_Exp*_tmp15A;
void*_tmp15B;void*_tmp15C;int _tmp15D;void*_tmp15E;void*_tmp15F;int _tmp160;void*
_tmp161;void*_tmp162;int _tmp163;void*_tmp164;struct Cyc_List_List*_tmp165;struct
Cyc_List_List _tmp166;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_List_List*_tmp168;
struct Cyc_List_List _tmp169;struct Cyc_Absyn_Exp*_tmp16A;void*_tmp16B;struct Cyc_Absyn_Vardecl*
_tmp16C;void*_tmp16D;struct Cyc_Absyn_Vardecl*_tmp16E;void*_tmp16F;struct Cyc_Absyn_Vardecl*
_tmp170;void*_tmp171;struct Cyc_Absyn_Vardecl*_tmp172;_LL111: if(*((int*)_tmp158)
!= 13)goto _LL113;_tmp159=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp158)->f1;
_tmp15A=((struct Cyc_Absyn_Cast_e_struct*)_tmp158)->f2;_LL112: i=_tmp15A;goto
inner_loop;_LL113: if(*((int*)_tmp158)!= 0)goto _LL115;_tmp15B=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp158)->f1;if(_tmp15B <= (void*)1?1:*((int*)_tmp15B)!= 2)goto _LL115;_tmp15C=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp15B)->f1;if((int)_tmp15C != 2)goto
_LL115;_tmp15D=((struct Cyc_Absyn_Int_c_struct*)_tmp15B)->f2;_LL114: _tmp160=
_tmp15D;goto _LL116;_LL115: if(*((int*)_tmp158)!= 0)goto _LL117;_tmp15E=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp158)->f1;if(_tmp15E <= (void*)1?1:*((int*)
_tmp15E)!= 2)goto _LL117;_tmp15F=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp15E)->f1;
if((int)_tmp15F != 0)goto _LL117;_tmp160=((struct Cyc_Absyn_Int_c_struct*)_tmp15E)->f2;
_LL116: return _tmp160 >= 0?Cyc_Toc_check_const_array((unsigned int)(_tmp160 + 1),(
void*)_tmp157->type): 0;_LL117: if(*((int*)_tmp158)!= 0)goto _LL119;_tmp161=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp158)->f1;if(_tmp161 <= (void*)1?1:*((int*)
_tmp161)!= 2)goto _LL119;_tmp162=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp161)->f1;
if((int)_tmp162 != 1)goto _LL119;_tmp163=((struct Cyc_Absyn_Int_c_struct*)_tmp161)->f2;
_LL118: return Cyc_Toc_check_const_array((unsigned int)(_tmp163 + 1),(void*)_tmp157->type);
_LL119: if(*((int*)_tmp158)!= 3)goto _LL11B;_tmp164=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp158)->f1;if((int)_tmp164 != 4)goto _LL11B;_tmp165=((struct Cyc_Absyn_Primop_e_struct*)
_tmp158)->f2;if(_tmp165 == 0)goto _LL11B;_tmp166=*_tmp165;_tmp167=(struct Cyc_Absyn_Exp*)
_tmp166.hd;_tmp168=_tmp166.tl;if(_tmp168 == 0)goto _LL11B;_tmp169=*_tmp168;_tmp16A=(
struct Cyc_Absyn_Exp*)_tmp169.hd;_LL11A: return Cyc_Toc_check_leq_size(relns,
_tmp157,_tmp16A);_LL11B: if(*((int*)_tmp158)!= 1)goto _LL11D;_tmp16B=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp158)->f2;if(_tmp16B <= (void*)1?1:*((int*)
_tmp16B)!= 4)goto _LL11D;_tmp16C=((struct Cyc_Absyn_Pat_b_struct*)_tmp16B)->f1;
_LL11C: _tmp16E=_tmp16C;goto _LL11E;_LL11D: if(*((int*)_tmp158)!= 1)goto _LL11F;
_tmp16D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp158)->f2;if(_tmp16D <= (void*)
1?1:*((int*)_tmp16D)!= 3)goto _LL11F;_tmp16E=((struct Cyc_Absyn_Local_b_struct*)
_tmp16D)->f1;_LL11E: _tmp170=_tmp16E;goto _LL120;_LL11F: if(*((int*)_tmp158)!= 1)
goto _LL121;_tmp16F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp158)->f2;if(
_tmp16F <= (void*)1?1:*((int*)_tmp16F)!= 0)goto _LL121;_tmp170=((struct Cyc_Absyn_Global_b_struct*)
_tmp16F)->f1;_LL120: _tmp172=_tmp170;goto _LL122;_LL121: if(*((int*)_tmp158)!= 1)
goto _LL123;_tmp171=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp158)->f2;if(
_tmp171 <= (void*)1?1:*((int*)_tmp171)!= 2)goto _LL123;_tmp172=((struct Cyc_Absyn_Param_b_struct*)
_tmp171)->f1;_LL122: if(_tmp172->escapes)return 0;{struct Cyc_List_List*_tmp173=
relns;for(0;_tmp173 != 0;_tmp173=_tmp173->tl){struct Cyc_CfFlowInfo_Reln*_tmp174=(
struct Cyc_CfFlowInfo_Reln*)_tmp173->hd;if(_tmp174->vd == _tmp172){void*_tmp175=(
void*)_tmp174->rop;struct Cyc_Absyn_Vardecl*_tmp176;struct Cyc_Absyn_Vardecl*
_tmp177;unsigned int _tmp178;_LL126: if(*((int*)_tmp175)!= 2)goto _LL128;_tmp176=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp175)->f1;_LL127: if(_tmp157 == _tmp176)
return 1;else{goto _LL125;}_LL128: if(*((int*)_tmp175)!= 1)goto _LL12A;_tmp177=((
struct Cyc_CfFlowInfo_LessVar_struct*)_tmp175)->f1;_LL129:{struct Cyc_List_List*
_tmp179=relns;for(0;_tmp179 != 0;_tmp179=_tmp179->tl){struct Cyc_CfFlowInfo_Reln*
_tmp17A=(struct Cyc_CfFlowInfo_Reln*)_tmp179->hd;if(_tmp17A->vd == _tmp177){void*
_tmp17B=(void*)_tmp17A->rop;struct Cyc_Absyn_Vardecl*_tmp17C;struct Cyc_Absyn_Vardecl*
_tmp17D;unsigned int _tmp17E;struct Cyc_Absyn_Vardecl*_tmp17F;_LL12F: if(*((int*)
_tmp17B)!= 4)goto _LL131;_tmp17C=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp17B)->f1;_LL130: _tmp17D=_tmp17C;goto _LL132;_LL131: if(*((int*)_tmp17B)!= 2)
goto _LL133;_tmp17D=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp17B)->f1;_LL132:
if(_tmp157 == _tmp17D)return 1;else{goto _LL12E;}_LL133: if(*((int*)_tmp17B)!= 0)
goto _LL135;_tmp17E=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp17B)->f1;_LL134:
return Cyc_Toc_check_const_array(_tmp17E,(void*)_tmp157->type);_LL135: if(*((int*)
_tmp17B)!= 1)goto _LL137;_tmp17F=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp17B)->f1;
_LL136: if(Cyc_Toc_check_leq_size_var(relns,_tmp157,_tmp17F))return 1;else{goto
_LL12E;}_LL137:;_LL138: goto _LL12E;_LL12E:;}}}goto _LL125;_LL12A: if(*((int*)
_tmp175)!= 3)goto _LL12C;_tmp178=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp175)->f1;
_LL12B: return Cyc_Toc_check_const_array(_tmp178,(void*)_tmp157->type);_LL12C:;
_LL12D: goto _LL125;_LL125:;}}}goto _LL110;_LL123:;_LL124: goto _LL110;_LL110:;}goto
_LL105;_LL10E:;_LL10F: goto _LL105;_LL105:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp180[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",
sizeof(char),24),_tag_arr(_tmp180,sizeof(void*),0));});return Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp181[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",
sizeof(char),24),_tag_arr(_tmp181,sizeof(void*),0));});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*
e){return({struct _tuple4*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->f1=Cyc_Toc_mt_tq;
_tmp182->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp182;});}static struct _tuple6*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);return({struct _tuple6*_tmp183=
_cycalloc(sizeof(*_tmp183));_tmp183->f1=0;_tmp183->f2=e;_tmp183;});}static struct
Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple1*x,void*
struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int
is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp184=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(
is_atomic?Cyc_Toc_malloc_atomic(_tmp184): Cyc_Toc_malloc_ptr(_tmp184));else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(rgnopt);Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp184);}}}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int e1_already_translated);static struct
Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp185=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp185 != 0;_tmp185=_tmp185->tl){struct _tuple6 _tmp187;struct Cyc_List_List*
_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct _tuple6*_tmp186=(struct _tuple6*)
_tmp185->hd;_tmp187=*_tmp186;_tmp188=_tmp187.f1;_tmp189=_tmp187.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp188 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp188->tl != 0)({void*_tmp18A[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("multiple designators in array",sizeof(
char),30),_tag_arr(_tmp18A,sizeof(void*),0));});{void*_tmp18B=(void*)_tmp188->hd;
void*_tmp18C=_tmp18B;struct Cyc_Absyn_Exp*_tmp18D;_LL13A: if(*((int*)_tmp18C)!= 0)
goto _LL13C;_tmp18D=((struct Cyc_Absyn_ArrayElement_struct*)_tmp18C)->f1;_LL13B:
Cyc_Toc_exp_to_c(nv,_tmp18D);e_index=_tmp18D;goto _LL139;_LL13C: if(*((int*)
_tmp18C)!= 1)goto _LL139;_LL13D:({void*_tmp18E[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("field name designators in array",
sizeof(char),32),_tag_arr(_tmp18E,sizeof(void*),0));});_LL139:;}}{struct Cyc_Absyn_Exp*
lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp18F=(void*)_tmp189->r;
struct Cyc_List_List*_tmp190;struct Cyc_Absyn_Vardecl*_tmp191;struct Cyc_Absyn_Exp*
_tmp192;struct Cyc_Absyn_Exp*_tmp193;void*_tmp194;struct Cyc_List_List*_tmp195;
_LL13F: if(*((int*)_tmp18F)!= 26)goto _LL141;_tmp190=((struct Cyc_Absyn_Array_e_struct*)
_tmp18F)->f1;_LL140: s=Cyc_Toc_init_array(nv,lval,_tmp190,s);goto _LL13E;_LL141:
if(*((int*)_tmp18F)!= 27)goto _LL143;_tmp191=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp18F)->f1;_tmp192=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp18F)->f2;
_tmp193=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp18F)->f3;_LL142: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp191,_tmp192,_tmp193,s,0);goto _LL13E;_LL143: if(*((int*)_tmp18F)!= 29)
goto _LL145;_tmp194=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp18F)->f1;
_tmp195=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp18F)->f2;_LL144: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp194,_tmp195,s);goto _LL13E;_LL145:;_LL146: Cyc_Toc_exp_to_c(nv,_tmp189);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp189,0),s,0);goto _LL13E;_LL13E:;}}}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple1*_tmp196=vd->name;if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(nv,_tmp196,Cyc_Absyn_varb_exp(
_tmp196,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp19E=_cycalloc(sizeof(*
_tmp19E));_tmp19E[0]=({struct Cyc_Absyn_Local_b_struct _tmp19F;_tmp19F.tag=3;
_tmp19F.f1=vd;_tmp19F;});_tmp19E;}),0));struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp196,0),Cyc_Absyn_signed_int_exp(
0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp196,0),Cyc_Absyn_var_exp(
max,0),0);struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(
_tmp196,0),0);struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(
_tmp196,0),0);struct Cyc_Absyn_Stmt*body;{void*_tmp197=(void*)e2->r;struct Cyc_List_List*
_tmp198;struct Cyc_Absyn_Vardecl*_tmp199;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*
_tmp19B;void*_tmp19C;struct Cyc_List_List*_tmp19D;_LL148: if(*((int*)_tmp197)!= 26)
goto _LL14A;_tmp198=((struct Cyc_Absyn_Array_e_struct*)_tmp197)->f1;_LL149: body=
Cyc_Toc_init_array(nv2,lval,_tmp198,Cyc_Toc_skip_stmt_dl());goto _LL147;_LL14A:
if(*((int*)_tmp197)!= 27)goto _LL14C;_tmp199=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp197)->f1;_tmp19A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp197)->f2;
_tmp19B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp197)->f3;_LL14B: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp199,_tmp19A,_tmp19B,Cyc_Toc_skip_stmt_dl(),0);goto _LL147;_LL14C: if(*((
int*)_tmp197)!= 29)goto _LL14E;_tmp19C=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp197)->f1;_tmp19D=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp197)->f2;_LL14D:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp19C,_tmp19D,Cyc_Toc_skip_stmt_dl());
goto _LL147;_LL14E:;_LL14F: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL147;_LL147:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp196,Cyc_Absyn_uint_typ,0,s2,0),
0),s,0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp1A0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){struct _tuple6 _tmp1A2;
struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct _tuple6*_tmp1A1=(
struct _tuple6*)_tmp1A0->hd;_tmp1A2=*_tmp1A1;_tmp1A3=_tmp1A2.f1;_tmp1A4=_tmp1A2.f2;
if(_tmp1A3 == 0)({void*_tmp1A5[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",sizeof(char),
22),_tag_arr(_tmp1A5,sizeof(void*),0));});if(_tmp1A3->tl != 0)({void*_tmp1A6[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("too many designators in anonymous struct",
sizeof(char),41),_tag_arr(_tmp1A6,sizeof(void*),0));});{void*_tmp1A7=(void*)
_tmp1A3->hd;struct _tagged_arr*_tmp1A8;_LL151: if(*((int*)_tmp1A7)!= 1)goto _LL153;
_tmp1A8=((struct Cyc_Absyn_FieldName_struct*)_tmp1A7)->f1;_LL152: {struct Cyc_Absyn_Exp*
lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp1A8,0);{void*_tmp1A9=(void*)_tmp1A4->r;
struct Cyc_List_List*_tmp1AA;struct Cyc_Absyn_Vardecl*_tmp1AB;struct Cyc_Absyn_Exp*
_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AD;void*_tmp1AE;struct Cyc_List_List*_tmp1AF;
_LL156: if(*((int*)_tmp1A9)!= 26)goto _LL158;_tmp1AA=((struct Cyc_Absyn_Array_e_struct*)
_tmp1A9)->f1;_LL157: s=Cyc_Toc_init_array(nv,lval,_tmp1AA,s);goto _LL155;_LL158:
if(*((int*)_tmp1A9)!= 27)goto _LL15A;_tmp1AB=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1A9)->f1;_tmp1AC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A9)->f2;
_tmp1AD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A9)->f3;_LL159: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1AB,_tmp1AC,_tmp1AD,s,0);goto _LL155;_LL15A: if(*((int*)_tmp1A9)!= 29)
goto _LL15C;_tmp1AE=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1A9)->f1;
_tmp1AF=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1A9)->f2;_LL15B: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp1AE,_tmp1AF,s);goto _LL155;_LL15C:;_LL15D: Cyc_Toc_exp_to_c(nv,_tmp1A4);
if(Cyc_Toc_is_poly_field(struct_type,_tmp1A8))_tmp1A4=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp1A4,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp1A4,0),0),s,0);goto _LL155;_LL155:;}goto _LL150;}_LL153:;_LL154:({void*_tmp1B0[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct",sizeof(char),27),_tag_arr(_tmp1B0,sizeof(
void*),0));});_LL150:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(
struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
es){struct _RegionHandle _tmp1B1=_new_region("r");struct _RegionHandle*r=& _tmp1B1;
_push_region(r);{struct Cyc_List_List*_tmp1B2=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _tagged_arr*_tmp1B3=Cyc_Toc_add_tuple_type(
_tmp1B2);struct _tuple1*_tmp1B4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1B5=
Cyc_Absyn_var_exp(_tmp1B4,0);struct Cyc_Absyn_Stmt*_tmp1B6=Cyc_Absyn_exp_stmt(
_tmp1B5,0);struct Cyc_Absyn_Exp*(*_tmp1B7)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,
struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp1B8=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp1B8);for(0;_tmp1B8 != 0;(_tmp1B8=_tmp1B8->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp1B8->hd;struct Cyc_Absyn_Exp*
lval=_tmp1B7(_tmp1B5,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic?Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v): 0;{void*_tmp1B9=(void*)e->r;
struct Cyc_List_List*_tmp1BA;struct Cyc_Absyn_Vardecl*_tmp1BB;struct Cyc_Absyn_Exp*
_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BD;_LL15F: if(*((int*)_tmp1B9)!= 26)goto _LL161;
_tmp1BA=((struct Cyc_Absyn_Array_e_struct*)_tmp1B9)->f1;_LL160: _tmp1B6=Cyc_Toc_init_array(
nv,lval,_tmp1BA,_tmp1B6);goto _LL15E;_LL161: if(*((int*)_tmp1B9)!= 27)goto _LL163;
_tmp1BB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1B9)->f1;_tmp1BC=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1B9)->f2;_tmp1BD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1B9)->f3;_LL162: _tmp1B6=Cyc_Toc_init_comprehension(nv,lval,_tmp1BB,_tmp1BC,
_tmp1BD,_tmp1B6,0);goto _LL15E;_LL163:;_LL164: Cyc_Toc_exp_to_c(nv,e);_tmp1B6=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1B7(_tmp1B5,Cyc_Absyn_fieldname(i),0),
e,0),0),_tmp1B6,0);goto _LL15E;_LL15E:;}}}{struct Cyc_Absyn_Exp*_tmp1BE=Cyc_Toc_make_struct(
nv,_tmp1B4,Cyc_Absyn_strct(_tmp1B3),_tmp1B6,pointer,rgnopt,is_atomic);
_npop_handler(0);return _tmp1BE;}};_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp1BF=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp1C0=Cyc_Absyn_var_exp(_tmp1BF,0);struct Cyc_Absyn_Stmt*
_tmp1C1=Cyc_Absyn_exp_stmt(_tmp1C0,0);struct Cyc_Absyn_Exp*(*_tmp1C2)(struct Cyc_Absyn_Exp*,
struct _tagged_arr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp1C3=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp1C4;void*
_tmp1C5;_LL166: if(_tmp1C3 <= (void*)3?1:*((int*)_tmp1C3)!= 10)goto _LL168;_tmp1C4=((
struct Cyc_Absyn_AggrType_struct*)_tmp1C3)->f1;_tmp1C5=(void*)_tmp1C4.aggr_info;
_LL167: ad=Cyc_Absyn_get_known_aggrdecl(_tmp1C5);goto _LL165;_LL168:;_LL169:({void*
_tmp1C6[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("init_struct: bad struct type",sizeof(char),29),_tag_arr(_tmp1C6,
sizeof(void*),0));});_LL165:;}{struct _RegionHandle _tmp1C7=_new_region("r");
struct _RegionHandle*r=& _tmp1C7;_push_region(r);{struct Cyc_List_List*_tmp1C8=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
r,dles);for(0;_tmp1C8 != 0;_tmp1C8=_tmp1C8->tl){struct _tuple6 _tmp1CA;struct Cyc_List_List*
_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CC;struct _tuple6*_tmp1C9=(struct _tuple6*)
_tmp1C8->hd;_tmp1CA=*_tmp1C9;_tmp1CB=_tmp1CA.f1;_tmp1CC=_tmp1CA.f2;is_atomic=
is_atomic?Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1CC->topt))->v):
0;if(_tmp1CB == 0)({void*_tmp1CD[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",sizeof(char),
22),_tag_arr(_tmp1CD,sizeof(void*),0));});if(_tmp1CB->tl != 0){struct _tuple1*
_tmp1CE=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1CF=Cyc_Absyn_var_exp(_tmp1CE,
0);for(0;_tmp1CB != 0;_tmp1CB=_tmp1CB->tl){void*_tmp1D0=(void*)_tmp1CB->hd;struct
_tagged_arr*_tmp1D1;_LL16B: if(*((int*)_tmp1D0)!= 1)goto _LL16D;_tmp1D1=((struct
Cyc_Absyn_FieldName_struct*)_tmp1D0)->f1;_LL16C: if(Cyc_Toc_is_poly_field(
struct_type,_tmp1D1))_tmp1CF=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp1CF,0);_tmp1C1=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp1C2(_tmp1C0,_tmp1D1,0),_tmp1CF,0),0),_tmp1C1,0);goto _LL16A;_LL16D:;_LL16E:({
void*_tmp1D2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct",sizeof(char),27),_tag_arr(_tmp1D2,sizeof(
void*),0));});_LL16A:;}Cyc_Toc_exp_to_c(nv,_tmp1CC);_tmp1C1=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1CF,_tmp1CC,0),0),_tmp1C1,0);}else{
void*_tmp1D3=(void*)_tmp1CB->hd;struct _tagged_arr*_tmp1D4;_LL170: if(*((int*)
_tmp1D3)!= 1)goto _LL172;_tmp1D4=((struct Cyc_Absyn_FieldName_struct*)_tmp1D3)->f1;
_LL171: {struct Cyc_Absyn_Exp*lval=_tmp1C2(_tmp1C0,_tmp1D4,0);{void*_tmp1D5=(void*)
_tmp1CC->r;struct Cyc_List_List*_tmp1D6;struct Cyc_Absyn_Vardecl*_tmp1D7;struct Cyc_Absyn_Exp*
_tmp1D8;struct Cyc_Absyn_Exp*_tmp1D9;void*_tmp1DA;struct Cyc_List_List*_tmp1DB;
_LL175: if(*((int*)_tmp1D5)!= 26)goto _LL177;_tmp1D6=((struct Cyc_Absyn_Array_e_struct*)
_tmp1D5)->f1;_LL176: _tmp1C1=Cyc_Toc_init_array(nv,lval,_tmp1D6,_tmp1C1);goto
_LL174;_LL177: if(*((int*)_tmp1D5)!= 27)goto _LL179;_tmp1D7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1D5)->f1;_tmp1D8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D5)->f2;
_tmp1D9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D5)->f3;_LL178: _tmp1C1=
Cyc_Toc_init_comprehension(nv,lval,_tmp1D7,_tmp1D8,_tmp1D9,_tmp1C1,0);goto _LL174;
_LL179: if(*((int*)_tmp1D5)!= 29)goto _LL17B;_tmp1DA=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1D5)->f1;_tmp1DB=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1D5)->f2;_LL17A:
_tmp1C1=Cyc_Toc_init_anon_struct(nv,lval,_tmp1DA,_tmp1DB,_tmp1C1);goto _LL174;
_LL17B:;_LL17C: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1CC->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp1CC);if(Cyc_Toc_is_poly_field(
struct_type,_tmp1D4)?!was_ptr_type: 0)_tmp1CC=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp1CC,0);if(has_exists)_tmp1CC=Cyc_Absyn_cast_exp((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(Cyc_Absyn_lookup_decl_field(ad,_tmp1D4)))->type,_tmp1CC,0);_tmp1C1=
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1CC,0),0),
_tmp1C1,0);goto _LL174;}_LL174:;}goto _LL16F;}_LL172:;_LL173:({void*_tmp1DC[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(char),27),_tag_arr(_tmp1DC,sizeof(void*),0));});_LL16F:;}}}{struct Cyc_Absyn_Exp*
_tmp1DD=Cyc_Toc_make_struct(nv,_tmp1BF,Cyc_Absyn_strctq(tdn),_tmp1C1,pointer,
rgnopt,is_atomic);_npop_handler(0);return _tmp1DD;};_pop_region(r);}}struct
_tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple9*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));
_tmp1DE[0]=({struct Cyc_Absyn_Increment_e_struct _tmp1DF;_tmp1DF.tag=5;_tmp1DF.f1=
e1;_tmp1DF.f2=(void*)incr;_tmp1DF;});_tmp1DE;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp1E0=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp1E1;void*_tmp1E2;struct Cyc_Absyn_Exp*
_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct _tagged_arr*_tmp1E5;_LL17E: if(*((int*)
_tmp1E0)!= 35)goto _LL180;_tmp1E1=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp1E0)->f1;
_LL17F: Cyc_Toc_lvalue_assign_stmt(_tmp1E1,fs,f,f_env);goto _LL17D;_LL180: if(*((
int*)_tmp1E0)!= 13)goto _LL182;_tmp1E2=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp1E0)->f1;_tmp1E3=((struct Cyc_Absyn_Cast_e_struct*)_tmp1E0)->f2;_LL181: Cyc_Toc_lvalue_assign(
_tmp1E3,fs,f,f_env);goto _LL17D;_LL182: if(*((int*)_tmp1E0)!= 21)goto _LL184;
_tmp1E4=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1E0)->f1;_tmp1E5=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1E0)->f2;_LL183:(void*)(e1->r=(void*)((void*)_tmp1E4->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));
_tmp1E6->hd=_tmp1E5;_tmp1E6->tl=fs;_tmp1E6;}),f,f_env);goto _LL17D;_LL184:;_LL185: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _tagged_arr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL17D;}_LL17D:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp1E7=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Decl*_tmp1E9;struct Cyc_Absyn_Stmt*
_tmp1EA;struct Cyc_Absyn_Stmt*_tmp1EB;_LL187: if(_tmp1E7 <= (void*)1?1:*((int*)
_tmp1E7)!= 0)goto _LL189;_tmp1E8=((struct Cyc_Absyn_Exp_s_struct*)_tmp1E7)->f1;
_LL188: Cyc_Toc_lvalue_assign(_tmp1E8,fs,f,f_env);goto _LL186;_LL189: if(_tmp1E7 <= (
void*)1?1:*((int*)_tmp1E7)!= 12)goto _LL18B;_tmp1E9=((struct Cyc_Absyn_Decl_s_struct*)
_tmp1E7)->f1;_tmp1EA=((struct Cyc_Absyn_Decl_s_struct*)_tmp1E7)->f2;_LL18A: Cyc_Toc_lvalue_assign_stmt(
_tmp1EA,fs,f,f_env);goto _LL186;_LL18B: if(_tmp1E7 <= (void*)1?1:*((int*)_tmp1E7)!= 
1)goto _LL18D;_tmp1EB=((struct Cyc_Absyn_Seq_s_struct*)_tmp1E7)->f2;_LL18C: Cyc_Toc_lvalue_assign_stmt(
_tmp1EB,fs,f,f_env);goto _LL186;_LL18D:;_LL18E:({struct Cyc_Std_String_pa_struct
_tmp1ED;_tmp1ED.tag=0;_tmp1ED.f1=(struct _tagged_arr)Cyc_Absynpp_stmt2string(s);{
void*_tmp1EC[1]={& _tmp1ED};Cyc_Toc_toc_impos(_tag_arr("lvalue_assign_stmt: %s",
sizeof(char),23),_tag_arr(_tmp1EC,sizeof(void*),1));}});_LL186:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp1EE=_region_malloc(r2,sizeof(*_tmp1EE));_tmp1EE->hd=(
void*)f((void*)x->hd,env);_tmp1EE->tl=0;_tmp1EE;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp1EF=_region_malloc(r2,sizeof(*_tmp1EF));_tmp1EF->hd=(void*)f((void*)x->hd,
env);_tmp1EF->tl=0;_tmp1EF;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple6*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple6*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->f1=0;_tmp1F0->f2=e;_tmp1F0;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp1F1=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp1F2;_LL190: if(_tmp1F1 <= (void*)3?1:*((int*)_tmp1F1)
!= 4)goto _LL192;_tmp1F2=((struct Cyc_Absyn_PointerType_struct*)_tmp1F1)->f1;
_LL191: return _tmp1F2;_LL192:;_LL193:({void*_tmp1F3[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("get_ptr_typ: not a pointer!",
sizeof(char),28),_tag_arr(_tmp1F3,sizeof(void*),0));});_LL18F:;}struct _tuple10{
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple11{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){void*_tmp1F4=(void*)e->r;if(e->topt == 0)({struct Cyc_Std_String_pa_struct
_tmp1F6;_tmp1F6.tag=0;_tmp1F6.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e);{
void*_tmp1F5[1]={& _tmp1F6};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: no type for %s",sizeof(char),25),_tag_arr(
_tmp1F5,sizeof(void*),1));}});{void*old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;void*_tmp1F7=_tmp1F4;void*_tmp1F8;struct _tuple1*_tmp1F9;
void*_tmp1FA;struct _tuple1*_tmp1FB;void*_tmp1FC;struct Cyc_List_List*_tmp1FD;
struct Cyc_Absyn_Exp*_tmp1FE;void*_tmp1FF;struct Cyc_Absyn_Exp*_tmp200;struct Cyc_Core_Opt*
_tmp201;struct Cyc_Absyn_Exp*_tmp202;struct Cyc_Absyn_Exp*_tmp203;struct Cyc_Absyn_Exp*
_tmp204;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Exp*_tmp206;struct Cyc_Absyn_Exp*
_tmp207;struct Cyc_Absyn_Exp*_tmp208;struct Cyc_List_List*_tmp209;struct Cyc_Absyn_Exp*
_tmp20A;struct Cyc_List_List*_tmp20B;struct Cyc_Absyn_VarargCallInfo*_tmp20C;
struct Cyc_Absyn_Exp*_tmp20D;struct Cyc_List_List*_tmp20E;struct Cyc_Absyn_VarargCallInfo*
_tmp20F;struct Cyc_Absyn_VarargCallInfo _tmp210;int _tmp211;struct Cyc_List_List*
_tmp212;struct Cyc_Absyn_VarargInfo*_tmp213;struct Cyc_Absyn_Exp*_tmp214;struct Cyc_Absyn_Exp*
_tmp215;struct Cyc_Absyn_Exp*_tmp216;struct Cyc_List_List*_tmp217;void*_tmp218;
void**_tmp219;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Exp*
_tmp21C;struct Cyc_Absyn_Exp*_tmp21D;struct Cyc_Absyn_Exp*_tmp21E;void*_tmp21F;
void*_tmp220;void*_tmp221;struct _tagged_arr*_tmp222;void*_tmp223;void*_tmp224;
unsigned int _tmp225;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Exp*_tmp227;
struct _tagged_arr*_tmp228;struct Cyc_Absyn_Exp*_tmp229;struct _tagged_arr*_tmp22A;
struct Cyc_Absyn_Exp*_tmp22B;struct Cyc_Absyn_Exp*_tmp22C;struct Cyc_List_List*
_tmp22D;struct Cyc_List_List*_tmp22E;struct Cyc_Absyn_Vardecl*_tmp22F;struct Cyc_Absyn_Exp*
_tmp230;struct Cyc_Absyn_Exp*_tmp231;struct _tuple1*_tmp232;struct Cyc_List_List*
_tmp233;struct Cyc_List_List*_tmp234;struct Cyc_Absyn_Aggrdecl*_tmp235;void*
_tmp236;struct Cyc_List_List*_tmp237;struct Cyc_List_List*_tmp238;struct Cyc_Absyn_Tuniondecl*
_tmp239;struct Cyc_Absyn_Tunionfield*_tmp23A;struct Cyc_List_List*_tmp23B;struct
Cyc_Absyn_Tuniondecl*_tmp23C;struct Cyc_Absyn_Tunionfield*_tmp23D;struct Cyc_Absyn_MallocInfo
_tmp23E;int _tmp23F;struct Cyc_Absyn_Exp*_tmp240;void**_tmp241;struct Cyc_Absyn_Exp*
_tmp242;int _tmp243;struct Cyc_Absyn_Stmt*_tmp244;struct Cyc_Absyn_Fndecl*_tmp245;
_LL195: if(*((int*)_tmp1F7)!= 0)goto _LL197;_tmp1F8=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1F7)->f1;if((int)_tmp1F8 != 0)goto _LL197;_LL196: {struct Cyc_Absyn_Exp*_tmp246=
Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_typ,_tmp246,
_tmp246))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp247[3];_tmp247[2]=_tmp246;_tmp247[1]=_tmp246;_tmp247[0]=
_tmp246;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp247,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(void*)((
void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL194;}_LL197: if(*((int*)
_tmp1F7)!= 0)goto _LL199;_LL198: goto _LL194;_LL199: if(*((int*)_tmp1F7)!= 1)goto
_LL19B;_tmp1F9=((struct Cyc_Absyn_Var_e_struct*)_tmp1F7)->f1;_tmp1FA=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp1F7)->f2;_LL19A:{struct _handler_cons _tmp248;
_push_handler(& _tmp248);{int _tmp24A=0;if(setjmp(_tmp248.handler))_tmp24A=1;if(!
_tmp24A){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k))Cyc_Dict_lookup)(nv->varmap,_tmp1F9))->r));;_pop_handler();}
else{void*_tmp249=(void*)_exn_thrown;void*_tmp24C=_tmp249;_LL1EA: if(_tmp24C != 
Cyc_Dict_Absent)goto _LL1EC;_LL1EB:({struct Cyc_Std_String_pa_struct _tmp24E;
_tmp24E.tag=0;_tmp24E.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1F9);{
void*_tmp24D[1]={& _tmp24E};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("Can't find %s in exp_to_c, Var\n",sizeof(char),32),
_tag_arr(_tmp24D,sizeof(void*),1));}});_LL1EC:;_LL1ED:(void)_throw(_tmp24C);
_LL1E9:;}}}goto _LL194;_LL19B: if(*((int*)_tmp1F7)!= 2)goto _LL19D;_tmp1FB=((struct
Cyc_Absyn_UnknownId_e_struct*)_tmp1F7)->f1;_LL19C:({void*_tmp24F[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unknownid",
sizeof(char),10),_tag_arr(_tmp24F,sizeof(void*),0));});_LL19D: if(*((int*)_tmp1F7)
!= 3)goto _LL19F;_tmp1FC=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1F7)->f1;
_tmp1FD=((struct Cyc_Absyn_Primop_e_struct*)_tmp1F7)->f2;_LL19E: {struct Cyc_List_List*
_tmp250=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp1FD);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp1FD);{void*_tmp251=_tmp1FC;_LL1EF: if((int)_tmp251 != 19)
goto _LL1F1;_LL1F0: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1FD))->hd;{void*_tmp252=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_Exp*_tmp253;struct Cyc_Absyn_PtrInfo
_tmp254;void*_tmp255;struct Cyc_Absyn_Conref*_tmp256;struct Cyc_Absyn_Conref*
_tmp257;_LL204: if(_tmp252 <= (void*)3?1:*((int*)_tmp252)!= 7)goto _LL206;_tmp253=((
struct Cyc_Absyn_ArrayType_struct*)_tmp252)->f3;_LL205:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp253))->r));goto _LL203;_LL206: if(_tmp252 <= (
void*)3?1:*((int*)_tmp252)!= 4)goto _LL208;_tmp254=((struct Cyc_Absyn_PointerType_struct*)
_tmp252)->f1;_tmp255=(void*)_tmp254.elt_typ;_tmp256=_tmp254.nullable;_tmp257=
_tmp254.bounds;_LL207:{void*_tmp258=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp257);struct Cyc_Absyn_Exp*_tmp259;_LL20B: if((int)_tmp258 != 0)goto _LL20D;
_LL20C:(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp25A[2];_tmp25A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp255),0);_tmp25A[0]=(struct Cyc_Absyn_Exp*)_tmp1FD->hd;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp25A,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL20A;_LL20D: if(_tmp258 <= (void*)1?1:*((int*)_tmp258)!= 0)
goto _LL20A;_tmp259=((struct Cyc_Absyn_Upper_b_struct*)_tmp258)->f1;_LL20E: if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp256))(void*)(e->r=(
void*)((void*)(Cyc_Absyn_conditional_exp(arg,_tmp259,Cyc_Absyn_uint_exp(0,0),0))->r));
else{(void*)(e->r=(void*)((void*)_tmp259->r));goto _LL20A;}_LL20A:;}goto _LL203;
_LL208:;_LL209:({struct Cyc_Std_String_pa_struct _tmp25D;_tmp25D.tag=0;_tmp25D.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);{struct Cyc_Std_String_pa_struct _tmp25C;_tmp25C.tag=0;
_tmp25C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);{void*_tmp25B[2]={& _tmp25C,& _tmp25D};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("size primop applied to non-array %s (%s)",
sizeof(char),41),_tag_arr(_tmp25B,sizeof(void*),2));}}});_LL203:;}goto _LL1EE;}
_LL1F1: if((int)_tmp251 != 0)goto _LL1F3;_LL1F2: {void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp250))->hd,& elt_typ)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1FD))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1FD->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp25E[3];_tmp25E[2]=e2;_tmp25E[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp25E[0]=e1;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp25E,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}goto _LL1EE;}_LL1F3: if((int)_tmp251 != 2)goto _LL1F5;_LL1F4: {void*
elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp250))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1FD))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1FD->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp250->tl))->hd)){(void*)(e1->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,
0))->r));(void*)(e2->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(
Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp25F[3];_tmp25F[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);
_tmp25F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp25F[0]=e1;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp25F,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}goto _LL1EE;}_LL1F5: if((int)
_tmp251 != 5)goto _LL1F7;_LL1F6: goto _LL1F8;_LL1F7: if((int)_tmp251 != 6)goto _LL1F9;
_LL1F8: goto _LL1FA;_LL1F9: if((int)_tmp251 != 7)goto _LL1FB;_LL1FA: goto _LL1FC;_LL1FB:
if((int)_tmp251 != 9)goto _LL1FD;_LL1FC: goto _LL1FE;_LL1FD: if((int)_tmp251 != 8)goto
_LL1FF;_LL1FE: goto _LL200;_LL1FF: if((int)_tmp251 != 10)goto _LL201;_LL200: {struct
Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1FD))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1FD->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)_check_null(
_tmp250))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp250->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));if(Cyc_Tcutil_is_tagged_pointer_typ(
t2))(void*)(e2->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));goto _LL1EE;}_LL201:;_LL202: goto _LL1EE;
_LL1EE:;}goto _LL194;}_LL19F: if(*((int*)_tmp1F7)!= 5)goto _LL1A1;_tmp1FE=((struct
Cyc_Absyn_Increment_e_struct*)_tmp1F7)->f1;_tmp1FF=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp1F7)->f2;_LL1A0: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1FE->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp1FE);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*_tmp260=(_tmp1FF == (void*)1?1: _tmp1FF == (
void*)3)?Cyc_Toc__tagged_arr_inplace_plus_post_e: Cyc_Toc__tagged_arr_inplace_plus_e;
int _tmp261=(_tmp1FF == (void*)0?1: _tmp1FF == (void*)1)?1: - 1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(_tmp260,({struct Cyc_Absyn_Exp*_tmp262[3];_tmp262[2]=
Cyc_Absyn_signed_int_exp(_tmp261,0);_tmp262[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0);_tmp262[0]=Cyc_Absyn_address_exp(_tmp1FE,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp262,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}if(elt_typ == (void*)0?!Cyc_Absyn_is_lvalue(_tmp1FE): 0){Cyc_Toc_lvalue_assign(
_tmp1FE,0,Cyc_Toc_incr_lvalue,_tmp1FF);(void*)(e->r=(void*)((void*)_tmp1FE->r));}
goto _LL194;}}_LL1A1: if(*((int*)_tmp1F7)!= 4)goto _LL1A3;_tmp200=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1F7)->f1;_tmp201=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1F7)->f2;_tmp202=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1F7)->f3;_LL1A2: {int e1_poly=Cyc_Toc_is_poly_project(
_tmp200);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp200->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp202->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp200);Cyc_Toc_exp_to_c(nv,_tmp202);{int done=0;if(_tmp201 != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp263=(void*)_tmp201->v;_LL210: if((int)_tmp263 != 0)goto _LL212;
_LL211: change=_tmp202;goto _LL20F;_LL212: if((int)_tmp263 != 2)goto _LL214;_LL213:
change=Cyc_Absyn_prim1_exp((void*)2,_tmp202,0);goto _LL20F;_LL214:;_LL215:({void*
_tmp264[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("bad t ? pointer arithmetic",sizeof(char),27),_tag_arr(_tmp264,sizeof(
void*),0));});_LL20F:;}done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({struct Cyc_Absyn_Exp*_tmp265[3];_tmp265[2]=
change;_tmp265[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp265[0]=
Cyc_Absyn_address_exp(_tmp200,0);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp265,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}
if(!done){if(e1_poly)(void*)(_tmp202->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp202->r,0),0))->r));if(!Cyc_Absyn_is_lvalue(_tmp200)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,struct _tuple9*),struct _tuple9*f_env))Cyc_Toc_lvalue_assign)(
_tmp200,0,Cyc_Toc_assignop_lvalue,({struct _tuple9*_tmp266=_cycalloc(sizeof(
struct _tuple9)* 1);_tmp266[0]=({struct _tuple9 _tmp267;_tmp267.f1=_tmp201;_tmp267.f2=
_tmp202;_tmp267;});_tmp266;}));(void*)(e->r=(void*)((void*)_tmp200->r));}}goto
_LL194;}}_LL1A3: if(*((int*)_tmp1F7)!= 6)goto _LL1A5;_tmp203=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1F7)->f1;_tmp204=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1F7)->f2;
_tmp205=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1F7)->f3;_LL1A4: Cyc_Toc_exp_to_c(
nv,_tmp203);Cyc_Toc_exp_to_c(nv,_tmp204);Cyc_Toc_exp_to_c(nv,_tmp205);goto _LL194;
_LL1A5: if(*((int*)_tmp1F7)!= 7)goto _LL1A7;_tmp206=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1F7)->f1;_tmp207=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1F7)->f2;_LL1A6: Cyc_Toc_exp_to_c(
nv,_tmp206);Cyc_Toc_exp_to_c(nv,_tmp207);goto _LL194;_LL1A7: if(*((int*)_tmp1F7)!= 
8)goto _LL1A9;_tmp208=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1F7)->f1;
_tmp209=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1F7)->f2;_LL1A8: _tmp20A=
_tmp208;_tmp20B=_tmp209;goto _LL1AA;_LL1A9: if(*((int*)_tmp1F7)!= 9)goto _LL1AB;
_tmp20A=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1F7)->f1;_tmp20B=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1F7)->f2;_tmp20C=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1F7)->f3;if(_tmp20C
!= 0)goto _LL1AB;_LL1AA: Cyc_Toc_exp_to_c(nv,_tmp20A);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp20B);goto _LL194;_LL1AB: if(*((int*)_tmp1F7)!= 9)goto
_LL1AD;_tmp20D=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1F7)->f1;_tmp20E=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp1F7)->f2;_tmp20F=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1F7)->f3;if(_tmp20F == 0)goto _LL1AD;_tmp210=*_tmp20F;_tmp211=_tmp210.num_varargs;
_tmp212=_tmp210.injectors;_tmp213=_tmp210.vai;_LL1AC:{struct _RegionHandle _tmp268=
_new_region("r");struct _RegionHandle*r=& _tmp268;_push_region(r);{struct _tuple1*
argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp211,0);
void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp213->type);void*arr_type=(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp27D;_tmp27D.tag=7;_tmp27D.f1=(void*)cva_type;
_tmp27D.f2=Cyc_Toc_mt_tq;_tmp27D.f3=(struct Cyc_Absyn_Exp*)num_varargs_exp;
_tmp27D;});_tmp27C;});int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp20E);int num_normargs=num_args - _tmp211;struct Cyc_List_List*new_args=0;{int i=
0;for(0;i < num_normargs;(++ i,_tmp20E=_tmp20E->tl)){Cyc_Toc_exp_to_c(nv,(struct
Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp20E))->hd);new_args=({
struct Cyc_List_List*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->hd=(struct Cyc_Absyn_Exp*)
_tmp20E->hd;_tmp269->tl=new_args;_tmp269;});}}new_args=({struct Cyc_List_List*
_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp26B[3];_tmp26B[2]=num_varargs_exp;_tmp26B[1]=Cyc_Absyn_sizeoftyp_exp(
cva_type,0);_tmp26B[0]=argvexp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp26B,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp26A->tl=new_args;
_tmp26A;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_args);Cyc_Toc_exp_to_c(nv,_tmp20D);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(_tmp20D,new_args,0),0);if(_tmp213->inject){struct Cyc_Absyn_Tuniondecl*
tud;{void*_tmp26C=Cyc_Tcutil_compress((void*)_tmp213->type);struct Cyc_Absyn_TunionInfo
_tmp26D;void*_tmp26E;struct Cyc_Absyn_Tuniondecl**_tmp26F;struct Cyc_Absyn_Tuniondecl*
_tmp270;_LL217: if(_tmp26C <= (void*)3?1:*((int*)_tmp26C)!= 2)goto _LL219;_tmp26D=((
struct Cyc_Absyn_TunionType_struct*)_tmp26C)->f1;_tmp26E=(void*)_tmp26D.tunion_info;
if(*((int*)_tmp26E)!= 1)goto _LL219;_tmp26F=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp26E)->f1;_tmp270=*_tmp26F;_LL218: tud=_tmp270;goto _LL216;_LL219:;_LL21A:({
void*_tmp271[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("toc: unknown tunion in vararg with inject",sizeof(char),42),_tag_arr(
_tmp271,sizeof(void*),0));});_LL216:;}{struct _tagged_arr vs=({unsigned int _tmp278=(
unsigned int)_tmp211;struct _tuple1**_tmp279=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp278));struct _tagged_arr _tmp27B=_tag_arr(
_tmp279,sizeof(struct _tuple1*),(unsigned int)_tmp211);{unsigned int _tmp27A=
_tmp278;unsigned int i;for(i=0;i < _tmp27A;i ++){_tmp279[i]=Cyc_Toc_temp_var();}}
_tmp27B;});struct Cyc_List_List*_tmp272=0;{int i=_tmp211 - 1;for(0;i >= 0;-- i){
_tmp272=({struct Cyc_List_List*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp273->tl=
_tmp272;_tmp273;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp272,0),s,0);{int i=0;for(0;_tmp20E != 0;(((
_tmp20E=_tmp20E->tl,_tmp212=_tmp212->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp20E->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_unknown_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp275;struct _tuple1*
_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_Absyn_Tunionfield*_tmp274=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp212))->hd;_tmp275=*
_tmp274;_tmp276=_tmp275.name;_tmp277=_tmp275.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp277))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Absyn_cast_exp(field_typ,arg,
0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,
Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp276,
1),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp276,_tag_arr("_struct",sizeof(char),8))),0,s,0);}}}}}else{{int i=0;for(0;
_tmp20E != 0;(_tmp20E=_tmp20E->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp20E->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp20E->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL194;_LL1AD: if(*((
int*)_tmp1F7)!= 10)goto _LL1AF;_tmp214=((struct Cyc_Absyn_Throw_e_struct*)_tmp1F7)->f1;
_LL1AE: Cyc_Toc_exp_to_c(nv,_tmp214);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp214),0))->r));goto _LL194;
_LL1AF: if(*((int*)_tmp1F7)!= 11)goto _LL1B1;_tmp215=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1F7)->f1;_LL1B0: Cyc_Toc_exp_to_c(nv,_tmp215);goto _LL194;_LL1B1: if(*((int*)
_tmp1F7)!= 12)goto _LL1B3;_tmp216=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1F7)->f1;
_tmp217=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1F7)->f2;_LL1B2: Cyc_Toc_exp_to_c(
nv,_tmp216);for(0;_tmp217 != 0;_tmp217=_tmp217->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp217->hd);if(k != (void*)4?k != (void*)3: 0){{void*_tmp27E=Cyc_Tcutil_compress((
void*)_tmp217->hd);_LL21C: if(_tmp27E <= (void*)3?1:*((int*)_tmp27E)!= 1)goto
_LL21E;_LL21D: goto _LL21F;_LL21E: if(_tmp27E <= (void*)3?1:*((int*)_tmp27E)!= 2)
goto _LL220;_LL21F: continue;_LL220:;_LL221:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp216,
0))->r));goto _LL21B;_LL21B:;}break;}}goto _LL194;_LL1B3: if(*((int*)_tmp1F7)!= 13)
goto _LL1B5;_tmp218=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1F7)->f1;_tmp219=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1F7)->f1);_tmp21A=((struct
Cyc_Absyn_Cast_e_struct*)_tmp1F7)->f2;_LL1B4: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp21A->topt))->v;void*new_typ=*_tmp219;*_tmp219=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp21A);{struct _tuple0 _tmp280=({struct _tuple0
_tmp27F;_tmp27F.f1=Cyc_Tcutil_compress(old_t2);_tmp27F.f2=Cyc_Tcutil_compress(
new_typ);_tmp27F;});void*_tmp281;struct Cyc_Absyn_PtrInfo _tmp282;void*_tmp283;
struct Cyc_Absyn_PtrInfo _tmp284;void*_tmp285;struct Cyc_Absyn_PtrInfo _tmp286;void*
_tmp287;_LL223: _tmp281=_tmp280.f1;if(_tmp281 <= (void*)3?1:*((int*)_tmp281)!= 4)
goto _LL225;_tmp282=((struct Cyc_Absyn_PointerType_struct*)_tmp281)->f1;_tmp283=
_tmp280.f2;if(_tmp283 <= (void*)3?1:*((int*)_tmp283)!= 4)goto _LL225;_tmp284=((
struct Cyc_Absyn_PointerType_struct*)_tmp283)->f1;_LL224: {int _tmp288=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp282.nullable);int
_tmp289=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp284.nullable);
void*_tmp28A=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp282.bounds);void*
_tmp28B=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp284.bounds);{struct _tuple0
_tmp28D=({struct _tuple0 _tmp28C;_tmp28C.f1=_tmp28A;_tmp28C.f2=_tmp28B;_tmp28C;});
void*_tmp28E;void*_tmp28F;void*_tmp290;struct Cyc_Absyn_Exp*_tmp291;void*_tmp292;
void*_tmp293;void*_tmp294;struct Cyc_Absyn_Exp*_tmp295;_LL22A: _tmp28E=_tmp28D.f1;
if(_tmp28E <= (void*)1?1:*((int*)_tmp28E)!= 0)goto _LL22C;_tmp28F=_tmp28D.f2;if(
_tmp28F <= (void*)1?1:*((int*)_tmp28F)!= 0)goto _LL22C;_LL22B: if(_tmp288?!_tmp289:
0){if(nv->toplevel)({void*_tmp296[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("can't do NULL-check conversion at top-level",
sizeof(char),44),_tag_arr(_tmp296,sizeof(void*),0));});(void*)(e->r=(void*)((
void*)(Cyc_Absyn_cast_exp(*_tmp219,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297->hd=_tmp21A;
_tmp297->tl=0;_tmp297;}),0),0))->r));}goto _LL229;_LL22C: _tmp290=_tmp28D.f1;if(
_tmp290 <= (void*)1?1:*((int*)_tmp290)!= 0)goto _LL22E;_tmp291=((struct Cyc_Absyn_Upper_b_struct*)
_tmp290)->f1;_tmp292=_tmp28D.f2;if((int)_tmp292 != 0)goto _LL22E;_LL22D: if(nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_t2,_tmp291,
_tmp21A))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp298[3];_tmp298[2]=_tmp291;_tmp298[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp284.elt_typ),0);_tmp298[0]=_tmp21A;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp298,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}goto _LL229;_LL22E: _tmp293=_tmp28D.f1;if((int)_tmp293 != 0)goto
_LL230;_tmp294=_tmp28D.f2;if(_tmp294 <= (void*)1?1:*((int*)_tmp294)!= 0)goto
_LL230;_tmp295=((struct Cyc_Absyn_Upper_b_struct*)_tmp294)->f1;_LL22F: if(nv->toplevel)({
void*_tmp299[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level",sizeof(char),45),
_tag_arr(_tmp299,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp29A=Cyc_Absyn_fncall_exp(
Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*_tmp29C[3];_tmp29C[2]=_tmp295;_tmp29C[
1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_tmp282.elt_typ),0);_tmp29C[0]=
_tmp21A;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp29C,sizeof(struct Cyc_Absyn_Exp*),3));}),0);if(_tmp289)(void*)(_tmp29A->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->hd=Cyc_Absyn_copy_exp(_tmp29A);
_tmp29B->tl=0;_tmp29B;}),0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*
_tmp219,_tmp29A,0))->r));goto _LL229;}_LL230:;_LL231: goto _LL229;_LL229:;}goto
_LL222;}_LL225: _tmp285=_tmp280.f1;if(_tmp285 <= (void*)3?1:*((int*)_tmp285)!= 4)
goto _LL227;_tmp286=((struct Cyc_Absyn_PointerType_struct*)_tmp285)->f1;_tmp287=
_tmp280.f2;if(_tmp287 <= (void*)3?1:*((int*)_tmp287)!= 5)goto _LL227;_LL226:{void*
_tmp29D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp286.bounds);_LL233: if((int)
_tmp29D != 0)goto _LL235;_LL234:(void*)(_tmp21A->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)_tmp21A->r,_tmp21A->loc),Cyc_Toc_curr_sp,0))->r));goto
_LL232;_LL235:;_LL236: goto _LL232;_LL232:;}goto _LL222;_LL227:;_LL228: goto _LL222;
_LL222:;}goto _LL194;}_LL1B5: if(*((int*)_tmp1F7)!= 14)goto _LL1B7;_tmp21B=((struct
Cyc_Absyn_Address_e_struct*)_tmp1F7)->f1;_LL1B6:{void*_tmp29E=(void*)_tmp21B->r;
struct _tuple1*_tmp29F;struct Cyc_List_List*_tmp2A0;struct Cyc_List_List*_tmp2A1;
struct Cyc_List_List*_tmp2A2;_LL238: if(*((int*)_tmp29E)!= 28)goto _LL23A;_tmp29F=((
struct Cyc_Absyn_Struct_e_struct*)_tmp29E)->f1;_tmp2A0=((struct Cyc_Absyn_Struct_e_struct*)
_tmp29E)->f2;_tmp2A1=((struct Cyc_Absyn_Struct_e_struct*)_tmp29E)->f3;_LL239: if(
nv->toplevel)({struct Cyc_Std_String_pa_struct _tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(
struct _tagged_arr)Cyc_Position_string_of_segment(_tmp21B->loc);{void*_tmp2A3[1]={&
_tmp2A4};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level",sizeof(char),42),_tag_arr(
_tmp2A3,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp21B->topt))->v,_tmp2A0 != 0,1,0,
_tmp2A1,_tmp29F))->r));goto _LL237;_LL23A: if(*((int*)_tmp29E)!= 24)goto _LL23C;
_tmp2A2=((struct Cyc_Absyn_Tuple_e_struct*)_tmp29E)->f1;_LL23B: if(nv->toplevel)({
struct Cyc_Std_String_pa_struct _tmp2A6;_tmp2A6.tag=0;_tmp2A6.f1=(struct
_tagged_arr)Cyc_Position_string_of_segment(_tmp21B->loc);{void*_tmp2A5[1]={&
_tmp2A6};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level",sizeof(char),42),_tag_arr(
_tmp2A5,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,
1,0,_tmp2A2))->r));goto _LL237;_LL23C:;_LL23D: Cyc_Toc_exp_to_c(nv,_tmp21B);if(!
Cyc_Absyn_is_lvalue(_tmp21B)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp21B,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp21B,
0))->r));}goto _LL237;_LL237:;}goto _LL194;_LL1B7: if(*((int*)_tmp1F7)!= 15)goto
_LL1B9;_tmp21C=((struct Cyc_Absyn_New_e_struct*)_tmp1F7)->f1;_tmp21D=((struct Cyc_Absyn_New_e_struct*)
_tmp1F7)->f2;_LL1B8: if(nv->toplevel)({struct Cyc_Std_String_pa_struct _tmp2A8;
_tmp2A8.tag=0;_tmp2A8.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp21D->loc);{void*_tmp2A7[1]={& _tmp2A8};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("%s: new at top-level",sizeof(char),21),
_tag_arr(_tmp2A7,sizeof(void*),1));}});{void*_tmp2A9=(void*)_tmp21D->r;struct Cyc_List_List*
_tmp2AA;struct Cyc_Absyn_Vardecl*_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*
_tmp2AD;struct _tuple1*_tmp2AE;struct Cyc_List_List*_tmp2AF;struct Cyc_List_List*
_tmp2B0;struct Cyc_List_List*_tmp2B1;_LL23F: if(*((int*)_tmp2A9)!= 26)goto _LL241;
_tmp2AA=((struct Cyc_Absyn_Array_e_struct*)_tmp2A9)->f1;_LL240: {struct _tuple1*
_tmp2B2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2B3=Cyc_Absyn_var_exp(_tmp2B2,
0);struct Cyc_Absyn_Stmt*_tmp2B4=Cyc_Toc_init_array(nv,_tmp2B3,_tmp2AA,Cyc_Absyn_exp_stmt(
_tmp2B3,0));void*old_elt_typ;{void*_tmp2B5=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_PtrInfo _tmp2B6;void*_tmp2B7;struct Cyc_Absyn_Tqual _tmp2B8;_LL24A: if(
_tmp2B5 <= (void*)3?1:*((int*)_tmp2B5)!= 4)goto _LL24C;_tmp2B6=((struct Cyc_Absyn_PointerType_struct*)
_tmp2B5)->f1;_tmp2B7=(void*)_tmp2B6.elt_typ;_tmp2B8=_tmp2B6.tq;_LL24B:
old_elt_typ=_tmp2B7;goto _LL249;_LL24C:;_LL24D: old_elt_typ=({void*_tmp2B9[0]={};
Cyc_Toc_toc_impos(_tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof(char),52),_tag_arr(_tmp2B9,sizeof(void*),0));});_LL249:;}{void*elt_typ=
Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp2BA=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2BB=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,
0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2AA),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp21C == 0)e1=Cyc_Toc_malloc_exp(
old_elt_typ,_tmp2BB);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(_tmp21C);Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp2BB);}(
void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2B2,
_tmp2BA,(struct Cyc_Absyn_Exp*)e1,_tmp2B4,0),0))->r));goto _LL23E;}}_LL241: if(*((
int*)_tmp2A9)!= 27)goto _LL243;_tmp2AB=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2A9)->f1;_tmp2AC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A9)->f2;
_tmp2AD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A9)->f3;_LL242: {int
is_tagged_ptr=0;{void*_tmp2BC=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp2BD;void*_tmp2BE;struct Cyc_Absyn_Tqual _tmp2BF;struct Cyc_Absyn_Conref*_tmp2C0;
_LL24F: if(_tmp2BC <= (void*)3?1:*((int*)_tmp2BC)!= 4)goto _LL251;_tmp2BD=((struct
Cyc_Absyn_PointerType_struct*)_tmp2BC)->f1;_tmp2BE=(void*)_tmp2BD.elt_typ;
_tmp2BF=_tmp2BD.tq;_tmp2C0=_tmp2BD.bounds;_LL250: is_tagged_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp2C0)== (void*)0;goto _LL24E;_LL251:;_LL252:({void*
_tmp2C1[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: comprehension not an array type",sizeof(char),42),_tag_arr(
_tmp2C1,sizeof(void*),0));});_LL24E:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2AD->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp2AC);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),
_tmp2AB,Cyc_Absyn_var_exp(max,0),_tmp2AD,Cyc_Toc_skip_stmt_dl(),1);{struct
_RegionHandle _tmp2C2=_new_region("r");struct _RegionHandle*r=& _tmp2C2;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp2D2=
_region_malloc(r,sizeof(*_tmp2D2));_tmp2D2->hd=({struct _tuple10*_tmp2D3=
_region_malloc(r,sizeof(*_tmp2D3));_tmp2D3->f1=max;_tmp2D3->f2=Cyc_Absyn_uint_typ;
_tmp2D3->f3=(struct Cyc_Absyn_Exp*)_tmp2AC;_tmp2D3;});_tmp2D2->tl=0;_tmp2D2;});
struct Cyc_Absyn_Exp*ai;if(_tmp21C == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp2C3[2];_tmp2C3[1]=Cyc_Absyn_var_exp(
max,0);_tmp2C3[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2C3,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(_tmp21C);
Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp2C4[2];_tmp2C4[1]=Cyc_Absyn_var_exp(max,0);_tmp2C4[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp2C4,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct
Cyc_Absyn_Exp*ainit=Cyc_Absyn_cast_exp(ptr_typ,ai,0);decls=({struct Cyc_List_List*
_tmp2C5=_region_malloc(r,sizeof(*_tmp2C5));_tmp2C5->hd=({struct _tuple10*_tmp2C6=
_region_malloc(r,sizeof(*_tmp2C6));_tmp2C6->f1=a;_tmp2C6->f2=ptr_typ;_tmp2C6->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp2C6;});_tmp2C5->tl=decls;_tmp2C5;});if(
is_tagged_ptr){struct _tuple1*_tmp2C7=Cyc_Toc_temp_var();void*_tmp2C8=Cyc_Toc_typ_to_c(
old_typ);struct Cyc_Absyn_Exp*_tmp2C9=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2CC[3];_tmp2CC[2]=_tmp2AC;_tmp2CC[1]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);_tmp2CC[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp2CC,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);decls=({struct Cyc_List_List*_tmp2CA=_region_malloc(r,sizeof(*_tmp2CA));
_tmp2CA->hd=({struct _tuple10*_tmp2CB=_region_malloc(r,sizeof(*_tmp2CB));_tmp2CB->f1=
_tmp2C7;_tmp2CB->f2=_tmp2C8;_tmp2CB->f3=(struct Cyc_Absyn_Exp*)_tmp2C9;_tmp2CB;});
_tmp2CA->tl=decls;_tmp2CA;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
_tmp2C7,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
a,0),0),0);}{struct Cyc_List_List*_tmp2CD=decls;for(0;_tmp2CD != 0;_tmp2CD=_tmp2CD->tl){
struct _tuple1*_tmp2CF;void*_tmp2D0;struct Cyc_Absyn_Exp*_tmp2D1;struct _tuple10
_tmp2CE=*((struct _tuple10*)_tmp2CD->hd);_tmp2CF=_tmp2CE.f1;_tmp2D0=_tmp2CE.f2;
_tmp2D1=_tmp2CE.f3;s=Cyc_Absyn_declare_stmt(_tmp2CF,_tmp2D0,_tmp2D1,s,0);}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL23E;}}}
_LL243: if(*((int*)_tmp2A9)!= 28)goto _LL245;_tmp2AE=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2A9)->f1;_tmp2AF=((struct Cyc_Absyn_Struct_e_struct*)_tmp2A9)->f2;_tmp2B0=((
struct Cyc_Absyn_Struct_e_struct*)_tmp2A9)->f3;_LL244:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp21D->topt))->v,
_tmp2AF != 0,1,_tmp21C,_tmp2B0,_tmp2AE))->r));goto _LL23E;_LL245: if(*((int*)
_tmp2A9)!= 24)goto _LL247;_tmp2B1=((struct Cyc_Absyn_Tuple_e_struct*)_tmp2A9)->f1;
_LL246:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp21C,_tmp2B1))->r));
goto _LL23E;_LL247:;_LL248: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp21D->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp2D4=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2D5=Cyc_Absyn_var_exp(
_tmp2D4,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp2D5,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp21C == 0)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(
_tmp21C);Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp2D6=(void*)_tmp21D->r;void*_tmp2D7;struct Cyc_Absyn_Exp*_tmp2D8;_LL254:
if(*((int*)_tmp2D6)!= 13)goto _LL256;_tmp2D7=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp2D6)->f1;_tmp2D8=((struct Cyc_Absyn_Cast_e_struct*)_tmp2D6)->f2;_LL255:{
struct _tuple0 _tmp2DA=({struct _tuple0 _tmp2D9;_tmp2D9.f1=Cyc_Tcutil_compress(
_tmp2D7);_tmp2D9.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2D8->topt))->v);_tmp2D9;});void*_tmp2DB;struct Cyc_Absyn_PtrInfo _tmp2DC;void*
_tmp2DD;struct Cyc_Absyn_Conref*_tmp2DE;void*_tmp2DF;struct Cyc_Absyn_PtrInfo
_tmp2E0;struct Cyc_Absyn_Conref*_tmp2E1;_LL259: _tmp2DB=_tmp2DA.f1;if(_tmp2DB <= (
void*)3?1:*((int*)_tmp2DB)!= 4)goto _LL25B;_tmp2DC=((struct Cyc_Absyn_PointerType_struct*)
_tmp2DB)->f1;_tmp2DD=(void*)_tmp2DC.elt_typ;_tmp2DE=_tmp2DC.bounds;_tmp2DF=
_tmp2DA.f2;if(_tmp2DF <= (void*)3?1:*((int*)_tmp2DF)!= 4)goto _LL25B;_tmp2E0=((
struct Cyc_Absyn_PointerType_struct*)_tmp2DF)->f1;_tmp2E1=_tmp2E0.bounds;_LL25A:{
struct _tuple0 _tmp2E3=({struct _tuple0 _tmp2E2;_tmp2E2.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp2DE);_tmp2E2.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp2E1);_tmp2E2;});
void*_tmp2E4;void*_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E6;_LL25E: _tmp2E4=_tmp2E3.f1;
if((int)_tmp2E4 != 0)goto _LL260;_tmp2E5=_tmp2E3.f2;if(_tmp2E5 <= (void*)1?1:*((int*)
_tmp2E5)!= 0)goto _LL260;_tmp2E6=((struct Cyc_Absyn_Upper_b_struct*)_tmp2E5)->f1;
_LL25F: Cyc_Toc_exp_to_c(nv,_tmp2D8);(void*)(inner_mexp->r=(void*)((void*)(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0))->r));done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2E7[4];_tmp2E7[3]=_tmp2E6;_tmp2E7[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp2DD),0);_tmp2E7[1]=_tmp2D8;_tmp2E7[0]=mexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2E7,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))->r));goto _LL25D;_LL260:;_LL261: goto _LL25D;_LL25D:;}goto _LL258;_LL25B:;
_LL25C: goto _LL258;_LL258:;}goto _LL253;_LL256:;_LL257: goto _LL253;_LL253:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp2E8=Cyc_Absyn_exp_stmt(_tmp2D5,0);struct Cyc_Absyn_Exp*
_tmp2E9=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp21D);_tmp2E8=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp2D5,_tmp2E9,0),_tmp21D,0),
_tmp2E8,0);{void*_tmp2EA=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2D4,_tmp2EA,(struct
Cyc_Absyn_Exp*)mexp,_tmp2E8,0),0))->r));}}goto _LL23E;}}_LL23E:;}goto _LL194;
_LL1B9: if(*((int*)_tmp1F7)!= 17)goto _LL1BB;_tmp21E=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp1F7)->f1;_LL1BA: Cyc_Toc_exp_to_c(nv,_tmp21E);goto _LL194;_LL1BB: if(*((int*)
_tmp1F7)!= 16)goto _LL1BD;_tmp21F=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp1F7)->f1;_LL1BC:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp2EB=_cycalloc(sizeof(*_tmp2EB));_tmp2EB[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp2EC;_tmp2EC.tag=16;_tmp2EC.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp21F);_tmp2EC;});
_tmp2EB;})));goto _LL194;_LL1BD: if(*((int*)_tmp1F7)!= 19)goto _LL1BF;_LL1BE:({void*
_tmp2ED[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("__gen() in code generator",sizeof(char),26),_tag_arr(_tmp2ED,sizeof(
void*),0));});_LL1BF: if(*((int*)_tmp1F7)!= 18)goto _LL1C1;_tmp220=(void*)((struct
Cyc_Absyn_Offsetof_e_struct*)_tmp1F7)->f1;_tmp221=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1F7)->f2;if(*((int*)_tmp221)!= 0)goto _LL1C1;_tmp222=((struct Cyc_Absyn_StructField_struct*)
_tmp221)->f1;_LL1C0:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp2EF;_tmp2EF.tag=18;_tmp2EF.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp220);_tmp2EF.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2F0=_cycalloc(sizeof(*
_tmp2F0));_tmp2F0[0]=({struct Cyc_Absyn_StructField_struct _tmp2F1;_tmp2F1.tag=0;
_tmp2F1.f1=_tmp222;_tmp2F1;});_tmp2F0;}));_tmp2EF;});_tmp2EE;})));goto _LL194;
_LL1C1: if(*((int*)_tmp1F7)!= 18)goto _LL1C3;_tmp223=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1F7)->f1;_tmp224=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1F7)->f2;
if(*((int*)_tmp224)!= 1)goto _LL1C3;_tmp225=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp224)->f1;_LL1C2:{void*_tmp2F2=Cyc_Tcutil_compress(_tmp223);struct Cyc_Absyn_AggrInfo
_tmp2F3;void*_tmp2F4;struct Cyc_List_List*_tmp2F5;_LL263: if(_tmp2F2 <= (void*)3?1:*((
int*)_tmp2F2)!= 10)goto _LL265;_tmp2F3=((struct Cyc_Absyn_AggrType_struct*)_tmp2F2)->f1;
_tmp2F4=(void*)_tmp2F3.aggr_info;_LL264: {struct Cyc_Absyn_Aggrdecl*_tmp2F6=Cyc_Absyn_get_known_aggrdecl(
_tmp2F4);if(_tmp2F6->impl == 0)({void*_tmp2F7[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("struct fields must be known",
sizeof(char),28),_tag_arr(_tmp2F7,sizeof(void*),0));});_tmp2F5=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2F6->impl))->fields;goto _LL266;}_LL265: if(_tmp2F2 <= (void*)3?1:*((
int*)_tmp2F2)!= 11)goto _LL267;_tmp2F5=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F2)->f2;_LL266: {struct Cyc_Absyn_Aggrfield*_tmp2F8=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F5,(int)_tmp225);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2F9=_cycalloc(sizeof(*
_tmp2F9));_tmp2F9[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2FA;_tmp2FA.tag=18;
_tmp2FA.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp223);_tmp2FA.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB[0]=({
struct Cyc_Absyn_StructField_struct _tmp2FC;_tmp2FC.tag=0;_tmp2FC.f1=_tmp2F8->name;
_tmp2FC;});_tmp2FB;}));_tmp2FA;});_tmp2F9;})));goto _LL262;}_LL267: if(_tmp2F2 <= (
void*)3?1:*((int*)_tmp2F2)!= 9)goto _LL269;_LL268:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp2FE;_tmp2FE.tag=18;_tmp2FE.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp223);_tmp2FE.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2FF=
_cycalloc(sizeof(*_tmp2FF));_tmp2FF[0]=({struct Cyc_Absyn_StructField_struct
_tmp300;_tmp300.tag=0;_tmp300.f1=Cyc_Absyn_fieldname((int)(_tmp225 + 1));_tmp300;});
_tmp2FF;}));_tmp2FE;});_tmp2FD;})));goto _LL262;_LL269: if(_tmp2F2 <= (void*)3?1:*((
int*)_tmp2F2)!= 3)goto _LL26B;_LL26A: if(_tmp225 == 0)(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp302;_tmp302.tag=18;_tmp302.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp223);_tmp302.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp303=
_cycalloc(sizeof(*_tmp303));_tmp303[0]=({struct Cyc_Absyn_StructField_struct
_tmp304;_tmp304.tag=0;_tmp304.f1=Cyc_Toc_tag_sp;_tmp304;});_tmp303;}));_tmp302;});
_tmp301;})));else{(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp305=_cycalloc(sizeof(*_tmp305));_tmp305[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp306;_tmp306.tag=18;_tmp306.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp223);_tmp306.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp307=_cycalloc(sizeof(*
_tmp307));_tmp307[0]=({struct Cyc_Absyn_StructField_struct _tmp308;_tmp308.tag=0;
_tmp308.f1=Cyc_Absyn_fieldname((int)_tmp225);_tmp308;});_tmp307;}));_tmp306;});
_tmp305;})));}goto _LL262;_LL26B:;_LL26C:({void*_tmp309[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("impossible type for offsetof tuple index",
sizeof(char),41),_tag_arr(_tmp309,sizeof(void*),0));});_LL262:;}goto _LL194;
_LL1C3: if(*((int*)_tmp1F7)!= 20)goto _LL1C5;_tmp226=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1F7)->f1;_LL1C4: {void*_tmp30A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp226->topt))->v);{void*_tmp30B=_tmp30A;struct Cyc_Absyn_PtrInfo
_tmp30C;void*_tmp30D;void*_tmp30E;struct Cyc_Absyn_Conref*_tmp30F;struct Cyc_Absyn_Tqual
_tmp310;struct Cyc_Absyn_Conref*_tmp311;_LL26E: if(_tmp30B <= (void*)3?1:*((int*)
_tmp30B)!= 4)goto _LL270;_tmp30C=((struct Cyc_Absyn_PointerType_struct*)_tmp30B)->f1;
_tmp30D=(void*)_tmp30C.elt_typ;_tmp30E=(void*)_tmp30C.rgn_typ;_tmp30F=_tmp30C.nullable;
_tmp310=_tmp30C.tq;_tmp311=_tmp30C.bounds;_LL26F:{void*_tmp312=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp311);_LL273: if(_tmp312 <= (void*)1?1:*((int*)_tmp312)!= 
0)goto _LL275;_LL274: {int do_null_check=Cyc_Toc_need_null_check(_tmp226);Cyc_Toc_exp_to_c(
nv,_tmp226);if(do_null_check)(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp226->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313->hd=_tmp226;_tmp313->tl=0;_tmp313;}),
0),0),0))->r));goto _LL272;}_LL275: if((int)_tmp312 != 0)goto _LL272;_LL276: {struct
Cyc_Absyn_Exp*_tmp314=Cyc_Absyn_uint_exp(0,0);_tmp314->topt=({struct Cyc_Core_Opt*
_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315->v=(void*)Cyc_Absyn_uint_typ;_tmp315;});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp226,_tmp314,0))->r));Cyc_Toc_exp_to_c(
nv,e);goto _LL272;}_LL272:;}goto _LL26D;_LL270:;_LL271:({void*_tmp316[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: Deref: non-pointer",
sizeof(char),29),_tag_arr(_tmp316,sizeof(void*),0));});_LL26D:;}goto _LL194;}
_LL1C5: if(*((int*)_tmp1F7)!= 21)goto _LL1C7;_tmp227=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1F7)->f1;_tmp228=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1F7)->f2;_LL1C6:
Cyc_Toc_exp_to_c(nv,_tmp227);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL194;
_LL1C7: if(*((int*)_tmp1F7)!= 22)goto _LL1C9;_tmp229=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1F7)->f1;_tmp22A=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1F7)->f2;_LL1C8: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp229->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp229);Cyc_Toc_exp_to_c(nv,_tmp229);{
int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp318;void*_tmp319;struct Cyc_Absyn_Conref*
_tmp31A;struct Cyc_Absyn_Tqual _tmp31B;struct Cyc_Absyn_Conref*_tmp31C;struct Cyc_Absyn_PtrInfo
_tmp317=Cyc_Toc_get_ptr_type(e1typ);_tmp318=(void*)_tmp317.elt_typ;_tmp319=(void*)
_tmp317.rgn_typ;_tmp31A=_tmp317.nullable;_tmp31B=_tmp317.tq;_tmp31C=_tmp317.bounds;{
void*_tmp31D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp31C);struct Cyc_Absyn_Exp*
_tmp31E;_LL278: if(_tmp31D <= (void*)1?1:*((int*)_tmp31D)!= 0)goto _LL27A;_tmp31E=((
struct Cyc_Absyn_Upper_b_struct*)_tmp31D)->f1;_LL279: {unsigned int _tmp320;int
_tmp321;struct _tuple5 _tmp31F=Cyc_Evexp_eval_const_uint_exp(_tmp31E);_tmp320=
_tmp31F.f1;_tmp321=_tmp31F.f2;if(_tmp321){if(_tmp320 < 1)({void*_tmp322[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("exp_to_c:  AggrArrow_e on pointer of size 0",
sizeof(char),44),_tag_arr(_tmp322,sizeof(void*),0));});if(do_null_check)(void*)(
e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp229->topt))->v),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp323=_cycalloc(sizeof(*_tmp323));
_tmp323->hd=_tmp229;_tmp323->tl=0;_tmp323;}),0),0),_tmp22A,0))->r));}else{(void*)(
e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp229->topt))->v),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp324[4];_tmp324[3]=
Cyc_Absyn_uint_exp(0,0);_tmp324[2]=Cyc_Absyn_sizeoftyp_exp(_tmp318,0);_tmp324[1]=
_tmp31E;_tmp324[0]=_tmp229;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp324,sizeof(struct Cyc_Absyn_Exp*),4));}),0),0),_tmp22A,0))->r));}
goto _LL277;}_LL27A: if((int)_tmp31D != 0)goto _LL277;_LL27B: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp318);(void*)(_tmp229->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
ta1,_tmp31B),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*
_tmp325[3];_tmp325[2]=Cyc_Absyn_uint_exp(0,0);_tmp325[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp325[0]=Cyc_Absyn_copy_exp(_tmp229);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp325,sizeof(struct Cyc_Absyn_Exp*),3));}),
0),0))->r));goto _LL277;}_LL277:;}if(is_poly)(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((
void*)e->r,0),0))->r));goto _LL194;}}_LL1C9: if(*((int*)_tmp1F7)!= 23)goto _LL1CB;
_tmp22B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1F7)->f1;_tmp22C=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1F7)->f2;_LL1CA: {void*_tmp326=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp22B->topt))->v);{void*_tmp327=_tmp326;struct Cyc_List_List*
_tmp328;struct Cyc_Absyn_PtrInfo _tmp329;void*_tmp32A;void*_tmp32B;struct Cyc_Absyn_Conref*
_tmp32C;struct Cyc_Absyn_Tqual _tmp32D;struct Cyc_Absyn_Conref*_tmp32E;_LL27D: if(
_tmp327 <= (void*)3?1:*((int*)_tmp327)!= 9)goto _LL27F;_tmp328=((struct Cyc_Absyn_TupleType_struct*)
_tmp327)->f1;_LL27E: Cyc_Toc_exp_to_c(nv,_tmp22B);Cyc_Toc_exp_to_c(nv,_tmp22C);{
unsigned int _tmp330;int _tmp331;struct _tuple5 _tmp32F=Cyc_Evexp_eval_const_uint_exp(
_tmp22C);_tmp330=_tmp32F.f1;_tmp331=_tmp32F.f2;if(!_tmp331)({void*_tmp332[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unknown tuple subscript in translation to C",
sizeof(char),44),_tag_arr(_tmp332,sizeof(void*),0));});(void*)(e->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(_tmp22B,Cyc_Absyn_fieldname((int)(_tmp330 + 1)),0))->r));
goto _LL27C;}_LL27F: if(_tmp327 <= (void*)3?1:*((int*)_tmp327)!= 4)goto _LL281;
_tmp329=((struct Cyc_Absyn_PointerType_struct*)_tmp327)->f1;_tmp32A=(void*)
_tmp329.elt_typ;_tmp32B=(void*)_tmp329.rgn_typ;_tmp32C=_tmp329.nullable;_tmp32D=
_tmp329.tq;_tmp32E=_tmp329.bounds;_LL280: {struct Cyc_List_List*_tmp333=Cyc_Toc_get_relns(
_tmp22B);int _tmp334=Cyc_Toc_check_bounds(_tmp333,_tmp22B,_tmp22C);if(Cyc_Toc_warn_bounds_checks?
!_tmp334: 0)({struct Cyc_Std_String_pa_struct _tmp336;_tmp336.tag=0;_tmp336.f1=(
struct _tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp335[1]={& _tmp336};Cyc_Tcutil_warn(
e->loc,_tag_arr("bounds check necessary for %s",sizeof(char),30),_tag_arr(
_tmp335,sizeof(void*),1));}});Cyc_Toc_exp_to_c(nv,_tmp22B);Cyc_Toc_exp_to_c(nv,
_tmp22C);Cyc_Toc_total_bounds_checks ++;{void*_tmp337=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp32E);struct Cyc_Absyn_Exp*_tmp338;_LL284: if(_tmp337 <= (void*)1?1:*((int*)
_tmp337)!= 0)goto _LL286;_tmp338=((struct Cyc_Absyn_Upper_b_struct*)_tmp337)->f1;
_LL285: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp32C);void*ta1=Cyc_Toc_typ_to_c(_tmp32A);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp32D);if(_tmp334)Cyc_Toc_bounds_checks_eliminated ++;else{if(possibly_null)(
void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp339[4];_tmp339[3]=
_tmp22C;_tmp339[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp339[1]=_tmp338;_tmp339[0]=
_tmp22B;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp339,sizeof(struct Cyc_Absyn_Exp*),4));}),0),0),0))->r));else{(void*)(_tmp22C->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp33A[2];_tmp33A[1]=Cyc_Absyn_copy_exp(_tmp22C);_tmp33A[0]=
_tmp338;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp33A,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}goto _LL283;}_LL286: if((
int)_tmp337 != 0)goto _LL283;_LL287: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp32A);if(
_tmp334){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(
Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp32D),Cyc_Absyn_aggrmember_exp(
_tmp22B,Cyc_Toc_curr_sp,0),0),_tmp22C,0))->r));}else{(void*)(e->r=(void*)((void*)(
Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp32D),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp33B[3];_tmp33B[2]=
_tmp22C;_tmp33B[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp33B[0]=_tmp22B;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp33B,sizeof(struct Cyc_Absyn_Exp*),
3));}),0),0),0))->r));}goto _LL283;}_LL283:;}goto _LL27C;}_LL281:;_LL282:({void*
_tmp33C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",sizeof(char),49),
_tag_arr(_tmp33C,sizeof(void*),0));});_LL27C:;}goto _LL194;}_LL1CB: if(*((int*)
_tmp1F7)!= 24)goto _LL1CD;_tmp22D=((struct Cyc_Absyn_Tuple_e_struct*)_tmp1F7)->f1;
_LL1CC: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,
_tmp22D))->r));else{struct Cyc_List_List*_tmp33D=((struct Cyc_List_List*(*)(struct
_tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,
_tmp22D);struct _tagged_arr*_tmp33E=Cyc_Toc_add_tuple_type(_tmp33D);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp22D != 0;(_tmp22D=_tmp22D->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp22D->hd);dles=({struct Cyc_List_List*_tmp33F=_cycalloc(
sizeof(*_tmp33F));_tmp33F->hd=({struct _tuple6*_tmp340=_cycalloc(sizeof(*_tmp340));
_tmp340->f1=0;_tmp340->f2=(struct Cyc_Absyn_Exp*)_tmp22D->hd;_tmp340;});_tmp33F->tl=
dles;_tmp33F;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));}
goto _LL194;_LL1CD: if(*((int*)_tmp1F7)!= 26)goto _LL1CF;_tmp22E=((struct Cyc_Absyn_Array_e_struct*)
_tmp1F7)->f1;_LL1CE:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp22E,0))->r));{struct Cyc_List_List*_tmp341=_tmp22E;for(0;_tmp341 != 0;_tmp341=
_tmp341->tl){struct _tuple6 _tmp343;struct Cyc_Absyn_Exp*_tmp344;struct _tuple6*
_tmp342=(struct _tuple6*)_tmp341->hd;_tmp343=*_tmp342;_tmp344=_tmp343.f2;Cyc_Toc_exp_to_c(
nv,_tmp344);}}goto _LL194;_LL1CF: if(*((int*)_tmp1F7)!= 27)goto _LL1D1;_tmp22F=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1F7)->f1;_tmp230=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1F7)->f2;_tmp231=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1F7)->f3;
_LL1D0: {unsigned int _tmp346;int _tmp347;struct _tuple5 _tmp345=Cyc_Evexp_eval_const_uint_exp(
_tmp230);_tmp346=_tmp345.f1;_tmp347=_tmp345.f2;Cyc_Toc_exp_to_c(nv,_tmp231);{
struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp231)){if(!_tmp347)({void*
_tmp348[0]={};Cyc_Tcutil_terr(_tmp230->loc,_tag_arr("cannot determine value of constant",
sizeof(char),35),_tag_arr(_tmp348,sizeof(void*),0));});{unsigned int i=0;for(0;i < 
_tmp346;i ++){es=({struct Cyc_List_List*_tmp349=_cycalloc(sizeof(*_tmp349));
_tmp349->hd=({struct _tuple6*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A->f1=0;
_tmp34A->f2=_tmp231;_tmp34A;});_tmp349->tl=es;_tmp349;});}}}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));goto _LL194;}}_LL1D1: if(*((int*)
_tmp1F7)!= 28)goto _LL1D3;_tmp232=((struct Cyc_Absyn_Struct_e_struct*)_tmp1F7)->f1;
_tmp233=((struct Cyc_Absyn_Struct_e_struct*)_tmp1F7)->f2;_tmp234=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1F7)->f3;_tmp235=((struct Cyc_Absyn_Struct_e_struct*)_tmp1F7)->f4;_LL1D2: if(!
nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,old_typ,_tmp233
!= 0,0,0,_tmp234,_tmp232))->r));else{if(_tmp235 == 0)({void*_tmp34B[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Struct_e: missing aggrdecl pointer",
sizeof(char),35),_tag_arr(_tmp34B,sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_check_null(_tmp235);struct _RegionHandle _tmp34C=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp34C;_push_region(rgn);{struct Cyc_List_List*
_tmp34D=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,e->loc,_tmp234,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
struct Cyc_List_List*_tmp34E=0;struct Cyc_List_List*_tmp34F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp34F != 0;_tmp34F=_tmp34F->tl){struct Cyc_List_List*
_tmp350=_tmp34D;for(0;_tmp350 != 0;_tmp350=_tmp350->tl){if((*((struct _tuple11*)
_tmp350->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp34F->hd){struct _tuple11
_tmp352;struct Cyc_Absyn_Aggrfield*_tmp353;struct Cyc_Absyn_Exp*_tmp354;struct
_tuple11*_tmp351=(struct _tuple11*)_tmp350->hd;_tmp352=*_tmp351;_tmp353=_tmp352.f1;
_tmp354=_tmp352.f2;{void*_tmp355=(void*)_tmp353->type;Cyc_Toc_exp_to_c(nv,
_tmp354);if(Cyc_Toc_is_void_star(_tmp355))(void*)(_tmp354->r=(void*)((void*)(Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp354->r,0),0))->r));
_tmp34E=({struct Cyc_List_List*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356->hd=({
struct _tuple6*_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357->f1=0;_tmp357->f2=
_tmp354;_tmp357;});_tmp356->tl=_tmp34E;_tmp356;});break;}}}}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp34E),0))->r));};_pop_region(rgn);}}goto _LL194;_LL1D3:
if(*((int*)_tmp1F7)!= 29)goto _LL1D5;_tmp236=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1F7)->f1;_tmp237=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1F7)->f2;_LL1D4: {
struct Cyc_List_List*fs;{void*_tmp358=Cyc_Tcutil_compress(_tmp236);struct Cyc_List_List*
_tmp359;_LL289: if(_tmp358 <= (void*)3?1:*((int*)_tmp358)!= 11)goto _LL28B;_tmp359=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp358)->f2;_LL28A: fs=_tmp359;goto _LL288;
_LL28B:;_LL28C:({struct Cyc_Std_String_pa_struct _tmp35B;_tmp35B.tag=0;_tmp35B.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp236);{void*_tmp35A[1]={& _tmp35B};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("anon struct has type %s",
sizeof(char),24),_tag_arr(_tmp35A,sizeof(void*),1));}});_LL288:;}{struct
_RegionHandle _tmp35C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp35C;
_push_region(rgn);{struct Cyc_List_List*_tmp35D=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp237,
fs);for(0;_tmp35D != 0;_tmp35D=_tmp35D->tl){struct _tuple11 _tmp35F;struct Cyc_Absyn_Aggrfield*
_tmp360;struct Cyc_Absyn_Exp*_tmp361;struct _tuple11*_tmp35E=(struct _tuple11*)
_tmp35D->hd;_tmp35F=*_tmp35E;_tmp360=_tmp35F.f1;_tmp361=_tmp35F.f2;{void*_tmp362=(
void*)_tmp360->type;Cyc_Toc_exp_to_c(nv,_tmp361);if(Cyc_Toc_is_void_star(_tmp362))(
void*)(_tmp361->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp361->r,0),0))->r));}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,_tmp237,0))->r));};_pop_region(rgn);}goto _LL194;}
_LL1D5: if(*((int*)_tmp1F7)!= 30)goto _LL1D7;_tmp238=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1F7)->f1;if(_tmp238 != 0)goto _LL1D7;_tmp239=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1F7)->f2;_tmp23A=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1F7)->f3;_LL1D6: {
struct _tuple1*qv=_tmp23A->name;struct Cyc_Absyn_Exp*tag_exp=_tmp239->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp239,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL194;}_LL1D7: if(*((int*)_tmp1F7)!= 30)goto _LL1D9;
_tmp23B=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1F7)->f1;_tmp23C=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1F7)->f2;_tmp23D=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1F7)->f3;_LL1D8: {
struct _tuple1*_tmp363=_tmp23D->name;struct Cyc_List_List*_tmp364=_tmp23D->typs;
struct _tuple1*_tmp365=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp366=Cyc_Absyn_var_exp(
_tmp365,0);void*_tmp367=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp363,
_tag_arr("_struct",sizeof(char),8)));struct Cyc_Absyn_Exp*tag_exp=_tmp23C->is_xtunion?
Cyc_Absyn_var_exp(_tmp363,0): Cyc_Toc_tunion_tag(_tmp23C,_tmp363,1);if(nv->toplevel){
struct Cyc_List_List*dles=0;for(0;_tmp23B != 0;(_tmp23B=_tmp23B->tl,_tmp364=
_tmp364->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp23B->hd;void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)_check_null(
_tmp364))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))
cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);dles=({struct Cyc_List_List*_tmp368=
_cycalloc(sizeof(*_tmp368));_tmp368->hd=({struct _tuple6*_tmp369=_cycalloc(
sizeof(*_tmp369));_tmp369->f1=0;_tmp369->f2=cur_e;_tmp369;});_tmp368->tl=dles;
_tmp368;});}dles=({struct Cyc_List_List*_tmp36A=_cycalloc(sizeof(*_tmp36A));
_tmp36A->hd=({struct _tuple6*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->f1=0;
_tmp36B->f2=tag_exp;_tmp36B;});_tmp36A->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp36A;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->v=Cyc_Toc_collapse_qvar_tag(
_tmp363,_tag_arr("_struct",sizeof(char),8));_tmp36C;}),dles,0))->r));}else{
struct Cyc_List_List*_tmp36D=({struct Cyc_List_List*_tmp373=_cycalloc(sizeof(*
_tmp373));_tmp373->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp366,Cyc_Toc_tag_sp,
0),tag_exp,0);_tmp373->tl=0;_tmp373;});{int i=1;for(0;_tmp23B != 0;(((_tmp23B=
_tmp23B->tl,i ++)),_tmp364=_tmp364->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp23B->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp364))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);{struct Cyc_Absyn_Stmt*
_tmp36E=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp366,Cyc_Absyn_fieldname(
i),0),cur_e,0);_tmp36D=({struct Cyc_List_List*_tmp36F=_cycalloc(sizeof(*_tmp36F));
_tmp36F->hd=_tmp36E;_tmp36F->tl=_tmp36D;_tmp36F;});}}}{struct Cyc_Absyn_Stmt*
_tmp370=Cyc_Absyn_exp_stmt(_tmp366,0);struct Cyc_Absyn_Stmt*_tmp371=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372->hd=_tmp370;_tmp372->tl=_tmp36D;
_tmp372;})),0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp365,_tmp367,0,_tmp371,0),0))->r));}}goto _LL194;}_LL1D9: if(*((int*)_tmp1F7)!= 
31)goto _LL1DB;_LL1DA: goto _LL1DC;_LL1DB: if(*((int*)_tmp1F7)!= 32)goto _LL1DD;
_LL1DC: goto _LL194;_LL1DD: if(*((int*)_tmp1F7)!= 33)goto _LL1DF;_tmp23E=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp1F7)->f1;_tmp23F=_tmp23E.is_calloc;_tmp240=_tmp23E.rgn;_tmp241=_tmp23E.elt_type;
_tmp242=_tmp23E.num_elts;_tmp243=_tmp23E.fat_result;_LL1DE: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp241)));Cyc_Toc_exp_to_c(nv,_tmp242);if(_tmp243){struct
_tuple1*_tmp374=Cyc_Toc_temp_var();struct _tuple1*_tmp375=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp23F){xexp=_tmp242;if(_tmp240 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp240);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp374,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp241,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp374,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp376[3];_tmp376[2]=Cyc_Absyn_var_exp(_tmp374,0);_tmp376[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp376[0]=Cyc_Absyn_var_exp(_tmp375,0);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp376,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp242,0);if(_tmp240 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp240);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp374,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp241,Cyc_Absyn_var_exp(_tmp374,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp377[3];
_tmp377[2]=Cyc_Absyn_var_exp(_tmp374,0);_tmp377[1]=Cyc_Absyn_uint_exp(1,0);
_tmp377[0]=Cyc_Absyn_var_exp(_tmp375,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp377,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}{struct Cyc_Absyn_Stmt*_tmp378=Cyc_Absyn_declare_stmt(_tmp374,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp375,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp378,0))->r));}}else{if(_tmp240 != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp240);Cyc_Toc_exp_to_c(
nv,rgn);(void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp241,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL194;}_LL1DF: if(*((int*)_tmp1F7)!= 35)goto _LL1E1;_tmp244=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp1F7)->f1;_LL1E0: Cyc_Toc_stmt_to_c(nv,
_tmp244);goto _LL194;_LL1E1: if(*((int*)_tmp1F7)!= 34)goto _LL1E3;_LL1E2:({void*
_tmp379[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem",sizeof(char),14),_tag_arr(_tmp379,sizeof(void*),0));});
_LL1E3: if(*((int*)_tmp1F7)!= 25)goto _LL1E5;_LL1E4:({void*_tmp37A[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("compoundlit",
sizeof(char),12),_tag_arr(_tmp37A,sizeof(void*),0));});_LL1E5: if(*((int*)_tmp1F7)
!= 36)goto _LL1E7;_tmp245=((struct Cyc_Absyn_Codegen_e_struct*)_tmp1F7)->f1;_LL1E6:({
void*_tmp37B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("codegen",sizeof(char),8),_tag_arr(_tmp37B,sizeof(void*),0));});_LL1E7:
if(*((int*)_tmp1F7)!= 37)goto _LL194;_LL1E8:({void*_tmp37C[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("fill",sizeof(char),
5),_tag_arr(_tmp37C,sizeof(void*),0));});_LL194:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple12{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple13{struct _tuple1*f1;void*f2;};struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple12 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct
Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{void*_tmp37D=(void*)p->r;struct Cyc_Absyn_Vardecl*
_tmp37E;struct Cyc_Absyn_Vardecl _tmp37F;struct _tuple1*_tmp380;struct Cyc_Absyn_Vardecl*
_tmp381;void*_tmp382;int _tmp383;char _tmp384;struct _tagged_arr _tmp385;struct Cyc_Absyn_Enumdecl*
_tmp386;struct Cyc_Absyn_Enumfield*_tmp387;void*_tmp388;struct Cyc_Absyn_Enumfield*
_tmp389;struct Cyc_Absyn_Tuniondecl*_tmp38A;struct Cyc_Absyn_Tunionfield*_tmp38B;
struct Cyc_List_List*_tmp38C;struct Cyc_Absyn_Pat*_tmp38D;struct Cyc_Absyn_Pat
_tmp38E;void*_tmp38F;struct Cyc_Absyn_Tuniondecl*_tmp390;struct Cyc_Absyn_Tunionfield*
_tmp391;struct Cyc_List_List*_tmp392;struct Cyc_List_List*_tmp393;struct Cyc_List_List*
_tmp394;struct Cyc_Absyn_AggrInfo _tmp395;void*_tmp396;struct Cyc_List_List*_tmp397;
struct Cyc_Absyn_Pat*_tmp398;_LL28E: if(_tmp37D <= (void*)2?1:*((int*)_tmp37D)!= 0)
goto _LL290;_tmp37E=((struct Cyc_Absyn_Var_p_struct*)_tmp37D)->f1;_tmp37F=*_tmp37E;
_tmp380=_tmp37F.name;_LL28F: nv=Cyc_Toc_add_varmap(nv,_tmp380,r);goto _LL291;
_LL290: if((int)_tmp37D != 0)goto _LL292;_LL291: s=Cyc_Toc_skip_stmt_dl();goto _LL28D;
_LL292: if(_tmp37D <= (void*)2?1:*((int*)_tmp37D)!= 6)goto _LL294;_tmp381=((struct
Cyc_Absyn_Reference_p_struct*)_tmp37D)->f1;_LL293: {struct _tuple1*_tmp399=Cyc_Toc_temp_var();
decls=({struct Cyc_List_List*_tmp39A=_region_malloc(rgn,sizeof(*_tmp39A));_tmp39A->hd=({
struct _tuple13*_tmp39B=_region_malloc(rgn,sizeof(*_tmp39B));_tmp39B->f1=_tmp399;
_tmp39B->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp39B;});
_tmp39A->tl=decls;_tmp39A;});nv=Cyc_Toc_add_varmap(nv,_tmp381->name,Cyc_Absyn_var_exp(
_tmp399,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp399,0),Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(
path,0),0),0);goto _LL28D;}_LL294: if((int)_tmp37D != 1)goto _LL296;_LL295: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL28D;_LL296: if(_tmp37D <= (void*)
2?1:*((int*)_tmp37D)!= 1)goto _LL298;_tmp382=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp37D)->f1;_tmp383=((struct Cyc_Absyn_Int_p_struct*)_tmp37D)->f2;_LL297: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_int_exp(_tmp382,_tmp383,0),fail_stmt);goto _LL28D;_LL298: if(_tmp37D <= (
void*)2?1:*((int*)_tmp37D)!= 2)goto _LL29A;_tmp384=((struct Cyc_Absyn_Char_p_struct*)
_tmp37D)->f1;_LL299: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp384,0),
fail_stmt);goto _LL28D;_LL29A: if(_tmp37D <= (void*)2?1:*((int*)_tmp37D)!= 3)goto
_LL29C;_tmp385=((struct Cyc_Absyn_Float_p_struct*)_tmp37D)->f1;_LL29B: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_float_exp(_tmp385,0),fail_stmt);goto _LL28D;_LL29C: if(_tmp37D <= (void*)
2?1:*((int*)_tmp37D)!= 9)goto _LL29E;_tmp386=((struct Cyc_Absyn_Enum_p_struct*)
_tmp37D)->f1;_tmp387=((struct Cyc_Absyn_Enum_p_struct*)_tmp37D)->f2;_LL29D: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp39C=_cycalloc(
sizeof(*_tmp39C));_tmp39C[0]=({struct Cyc_Absyn_Enum_e_struct _tmp39D;_tmp39D.tag=
31;_tmp39D.f1=_tmp387->name;_tmp39D.f2=(struct Cyc_Absyn_Enumdecl*)_tmp386;
_tmp39D.f3=(struct Cyc_Absyn_Enumfield*)_tmp387;_tmp39D;});_tmp39C;}),0),
fail_stmt);goto _LL28D;_LL29E: if(_tmp37D <= (void*)2?1:*((int*)_tmp37D)!= 10)goto
_LL2A0;_tmp388=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp37D)->f1;_tmp389=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp37D)->f2;_LL29F: s=Cyc_Toc_if_neq_stmt(r,
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp39E=_cycalloc(
sizeof(*_tmp39E));_tmp39E[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp39F;_tmp39F.tag=
32;_tmp39F.f1=_tmp389->name;_tmp39F.f2=(void*)_tmp388;_tmp39F.f3=(struct Cyc_Absyn_Enumfield*)
_tmp389;_tmp39F;});_tmp39E;}),0),fail_stmt);goto _LL28D;_LL2A0: if(_tmp37D <= (void*)
2?1:*((int*)_tmp37D)!= 8)goto _LL2A2;_tmp38A=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp37D)->f1;_tmp38B=((struct Cyc_Absyn_Tunion_p_struct*)_tmp37D)->f2;_tmp38C=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp37D)->f3;if(_tmp38C != 0)goto _LL2A2;_LL2A1: {
struct Cyc_Absyn_Exp*cmp_exp;if(_tmp38A->is_xtunion)cmp_exp=Cyc_Absyn_var_exp(
_tmp38B->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(_tmp38A,_tmp38B->name,0);r=Cyc_Absyn_cast_exp(
Cyc_Absyn_sint_typ,r,0);}s=Cyc_Toc_if_neq_stmt(r,cmp_exp,fail_stmt);goto _LL28D;}
_LL2A2: if(_tmp37D <= (void*)2?1:*((int*)_tmp37D)!= 5)goto _LL2A4;_tmp38D=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp37D)->f1;_tmp38E=*_tmp38D;_tmp38F=(void*)_tmp38E.r;
if(_tmp38F <= (void*)2?1:*((int*)_tmp38F)!= 8)goto _LL2A4;_tmp390=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp38F)->f1;_tmp391=((struct Cyc_Absyn_Tunion_p_struct*)_tmp38F)->f2;_tmp392=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp38F)->f3;if(!(_tmp392 != 0))goto _LL2A4;
_LL2A3: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(
_tmp391->name,_tag_arr("_struct",sizeof(char),8));struct Cyc_Absyn_Exp*rcast=Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r,0);struct Cyc_List_List*
_tmp3A0=_tmp391->typs;for(0;_tmp392 != 0;(((_tmp392=_tmp392->tl,_tmp3A0=((struct
Cyc_List_List*)_check_null(_tmp3A0))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp3A1=(
struct Cyc_Absyn_Pat*)_tmp392->hd;if((void*)_tmp3A1->r == (void*)0)continue;{void*
_tmp3A2=(*((struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp3A0))->hd)).f2;
struct _tuple1*_tmp3A3=Cyc_Toc_temp_var();void*_tmp3A4=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3A1->topt))->v;void*_tmp3A5=Cyc_Toc_typ_to_c(_tmp3A4);struct Cyc_Absyn_Exp*
_tmp3A6=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp3A2)))_tmp3A6=Cyc_Absyn_cast_exp(_tmp3A5,_tmp3A6,0);decls=({
struct Cyc_List_List*_tmp3A7=_region_malloc(rgn,sizeof(*_tmp3A7));_tmp3A7->hd=({
struct _tuple13*_tmp3A8=_region_malloc(rgn,sizeof(*_tmp3A8));_tmp3A8->f1=_tmp3A3;
_tmp3A8->f2=_tmp3A5;_tmp3A8;});_tmp3A7->tl=decls;_tmp3A7;});{struct _tuple12
_tmp3A9=Cyc_Toc_xlate_pat(nv,rgn,_tmp3A4,Cyc_Absyn_var_exp(_tmp3A3,0),_tmp3A6,
_tmp3A1,fail_stmt,decls);nv=_tmp3A9.f1;decls=_tmp3A9.f2;{struct Cyc_Absyn_Stmt*
_tmp3AA=_tmp3A9.f3;struct Cyc_Absyn_Stmt*_tmp3AB=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp3A3,0),_tmp3A6,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp3AB,_tmp3AA,
0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(_tmp390->is_xtunion){struct Cyc_Absyn_Exp*
e2=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),
r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp391->name,0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);}else{int max_tag=Cyc_Toc_num_void_tags(
_tmp390);struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)
max_tag,0);struct Cyc_Absyn_Exp*e3=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,
Cyc_Toc_mt_tq),r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e5=Cyc_Absyn_lte_exp(r,Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),max_tag_exp,
0),0);struct Cyc_Absyn_Exp*e=Cyc_Toc_tunion_tag(_tmp390,_tmp391->name,1);test_exp=
max_tag > 0?Cyc_Absyn_or_exp(e5,Cyc_Absyn_neq_exp(e1,e,0),0): Cyc_Absyn_neq_exp(e1,
e,0);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);goto _LL28D;}}_LL2A4: if(_tmp37D <= (void*)2?1:*((int*)_tmp37D)!= 8)goto
_LL2A6;_tmp393=((struct Cyc_Absyn_Tunion_p_struct*)_tmp37D)->f3;_LL2A5: _tmp394=
_tmp393;goto _LL2A7;_LL2A6: if(_tmp37D <= (void*)2?1:*((int*)_tmp37D)!= 4)goto
_LL2A8;_tmp394=((struct Cyc_Absyn_Tuple_p_struct*)_tmp37D)->f1;_LL2A7: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;for(0;_tmp394 != 0;(_tmp394=_tmp394->tl,++ cnt)){struct Cyc_Absyn_Pat*
_tmp3AC=(struct Cyc_Absyn_Pat*)_tmp394->hd;if((void*)_tmp3AC->r == (void*)0)
continue;{struct _tuple1*_tmp3AD=Cyc_Toc_temp_var();void*_tmp3AE=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp3AC->topt))->v;decls=({struct Cyc_List_List*_tmp3AF=
_region_malloc(rgn,sizeof(*_tmp3AF));_tmp3AF->hd=({struct _tuple13*_tmp3B0=
_region_malloc(rgn,sizeof(*_tmp3B0));_tmp3B0->f1=_tmp3AD;_tmp3B0->f2=Cyc_Toc_typ_to_c(
_tmp3AE);_tmp3B0;});_tmp3AF->tl=decls;_tmp3AF;});{struct _tuple12 _tmp3B1=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3AE,Cyc_Absyn_var_exp(_tmp3AD,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp3AC,fail_stmt,decls);nv=_tmp3B1.f1;decls=_tmp3B1.f2;{struct Cyc_Absyn_Stmt*
_tmp3B2=_tmp3B1.f3;struct Cyc_Absyn_Stmt*_tmp3B3=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp3AD,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp3B3,_tmp3B2,0),0);}}}}goto _LL28D;}_LL2A8: if(_tmp37D <= (
void*)2?1:*((int*)_tmp37D)!= 7)goto _LL2AA;_tmp395=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp37D)->f1;_tmp396=(void*)_tmp395.aggr_info;_tmp397=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp37D)->f3;_LL2A9: {struct Cyc_Absyn_Aggrdecl*_tmp3B4=Cyc_Absyn_get_known_aggrdecl(
_tmp396);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp397 != 0;_tmp397=_tmp397->tl){struct
_tuple14*_tmp3B5=(struct _tuple14*)_tmp397->hd;struct Cyc_Absyn_Pat*_tmp3B6=(*
_tmp3B5).f2;if((void*)_tmp3B6->r == (void*)0)continue;{struct _tagged_arr*f;{void*
_tmp3B7=(void*)((struct Cyc_List_List*)_check_null((*_tmp3B5).f1))->hd;struct
_tagged_arr*_tmp3B8;_LL2B1: if(*((int*)_tmp3B7)!= 1)goto _LL2B3;_tmp3B8=((struct
Cyc_Absyn_FieldName_struct*)_tmp3B7)->f1;_LL2B2: f=_tmp3B8;goto _LL2B0;_LL2B3:;
_LL2B4:(int)_throw((void*)Cyc_Toc_Match_error);_LL2B0:;}{struct _tuple1*_tmp3B9=
Cyc_Toc_temp_var();void*_tmp3BA=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3B6->topt))->v;
void*_tmp3BB=Cyc_Toc_typ_to_c(_tmp3BA);decls=({struct Cyc_List_List*_tmp3BC=
_region_malloc(rgn,sizeof(*_tmp3BC));_tmp3BC->hd=({struct _tuple13*_tmp3BD=
_region_malloc(rgn,sizeof(*_tmp3BD));_tmp3BD->f1=_tmp3B9;_tmp3BD->f2=_tmp3BB;
_tmp3BD;});_tmp3BC->tl=decls;_tmp3BC;});{struct _tuple12 _tmp3BE=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3BA,Cyc_Absyn_var_exp(_tmp3B9,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp3B6,fail_stmt,decls);nv=_tmp3BE.f1;decls=_tmp3BE.f2;{struct Cyc_Absyn_Exp*
_tmp3BF=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((void*)((struct
Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3B4->impl))->fields,f)))->type))_tmp3BF=Cyc_Absyn_cast_exp(
_tmp3BB,_tmp3BF,0);{struct Cyc_Absyn_Stmt*_tmp3C0=_tmp3BE.f3;struct Cyc_Absyn_Stmt*
_tmp3C1=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp3B9,0),_tmp3BF,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp3C1,_tmp3C0,0),0);}}}}}}goto _LL28D;}_LL2AA: if(_tmp37D <= (
void*)2?1:*((int*)_tmp37D)!= 5)goto _LL2AC;_tmp398=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp37D)->f1;_LL2AB: {struct _tuple1*_tmp3C2=Cyc_Toc_temp_var();void*_tmp3C3=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp398->topt))->v;decls=({struct Cyc_List_List*
_tmp3C4=_region_malloc(rgn,sizeof(*_tmp3C4));_tmp3C4->hd=({struct _tuple13*
_tmp3C5=_region_malloc(rgn,sizeof(*_tmp3C5));_tmp3C5->f1=_tmp3C2;_tmp3C5->f2=Cyc_Toc_typ_to_c(
_tmp3C3);_tmp3C5;});_tmp3C4->tl=decls;_tmp3C4;});{struct _tuple12 _tmp3C6=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3C3,Cyc_Absyn_var_exp(_tmp3C2,0),Cyc_Absyn_deref_exp(path,0),_tmp398,
fail_stmt,decls);nv=_tmp3C6.f1;decls=_tmp3C6.f2;{struct Cyc_Absyn_Stmt*_tmp3C7=
_tmp3C6.f3;struct Cyc_Absyn_Stmt*_tmp3C8=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp3C2,0),Cyc_Absyn_deref_exp(r,0),0),_tmp3C7,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp3C8,0);else{s=_tmp3C8;}goto
_LL28D;}}}_LL2AC: if(_tmp37D <= (void*)2?1:*((int*)_tmp37D)!= 11)goto _LL2AE;_LL2AD:({
void*_tmp3C9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unknownid",sizeof(char),10),_tag_arr(_tmp3C9,sizeof(void*),0));});
_LL2AE: if(_tmp37D <= (void*)2?1:*((int*)_tmp37D)!= 12)goto _LL28D;_LL2AF:({void*
_tmp3CA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unknowncall",sizeof(char),12),_tag_arr(_tmp3CA,sizeof(void*),0));});
_LL28D:;}return({struct _tuple12 _tmp3CB;_tmp3CB.f1=nv;_tmp3CB.f2=decls;_tmp3CB.f3=
s;_tmp3CB;});}struct _tuple15{struct _tagged_arr*f1;struct _tagged_arr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple15*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple15*_tmp3CC=_region_malloc(r,sizeof(*_tmp3CC));_tmp3CC->f1=
Cyc_Toc_fresh_label();_tmp3CC->f2=Cyc_Toc_fresh_label();_tmp3CC->f3=sc;_tmp3CC;});}
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*
_tmp3CD=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int leave_as_switch;{
void*_tmp3CE=Cyc_Tcutil_compress(_tmp3CD);_LL2B6: if(_tmp3CE <= (void*)3?1:*((int*)
_tmp3CE)!= 5)goto _LL2B8;_LL2B7: goto _LL2B9;_LL2B8: if(_tmp3CE <= (void*)3?1:*((int*)
_tmp3CE)!= 12)goto _LL2BA;_LL2B9: leave_as_switch=1;goto _LL2B5;_LL2BA:;_LL2BB:
leave_as_switch=0;goto _LL2B5;_LL2B5:;}{struct Cyc_List_List*_tmp3CF=scs;for(0;
_tmp3CF != 0;_tmp3CF=_tmp3CF->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp3CF->hd)->pat_vars))->v != 0?1:((
struct Cyc_Absyn_Switch_clause*)_tmp3CF->hd)->where_clause != 0){leave_as_switch=0;
break;}}}if(leave_as_switch){struct _tagged_arr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp3D0=scs;for(0;_tmp3D0 != 0;_tmp3D0=_tmp3D0->tl){struct Cyc_Absyn_Stmt*
_tmp3D1=((struct Cyc_Absyn_Switch_clause*)_tmp3D0->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp3D0->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp3D1,0);next_l=Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(nv,next_l),_tmp3D1);}}return;}{
struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct _tagged_arr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp3D2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3D2;
_push_region(rgn);{struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple15*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,
rgn,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp3D3=lscs;for(0;_tmp3D3 != 0;_tmp3D3=_tmp3D3->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple15*)_tmp3D3->hd)).f3;struct
_tagged_arr*fail_lab=_tmp3D3->tl == 0?end_l:(*((struct _tuple15*)((struct Cyc_List_List*)
_check_null(_tmp3D3->tl))->hd)).f1;struct Cyc_Toc_Env*_tmp3D5;struct Cyc_List_List*
_tmp3D6;struct Cyc_Absyn_Stmt*_tmp3D7;struct _tuple12 _tmp3D4=Cyc_Toc_xlate_pat(nv,
rgn,_tmp3CD,r,path,sc->pattern,Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp3D5=
_tmp3D4.f1;_tmp3D6=_tmp3D4.f2;_tmp3D7=_tmp3D4.f3;if(sc->where_clause != 0){struct
Cyc_Absyn_Exp*_tmp3D8=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp3D5,_tmp3D8);_tmp3D7=Cyc_Absyn_seq_stmt(_tmp3D7,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp3D8,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp3D6;nvs=({struct Cyc_List_List*_tmp3D9=_region_malloc(rgn,
sizeof(*_tmp3D9));_tmp3D9->hd=_tmp3D5;_tmp3D9->tl=nvs;_tmp3D9;});test_stmts=({
struct Cyc_List_List*_tmp3DA=_region_malloc(rgn,sizeof(*_tmp3DA));_tmp3DA->hd=
_tmp3D7;_tmp3DA->tl=test_stmts;_tmp3DA;});}}nvs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple15 _tmp3DC;struct _tagged_arr*_tmp3DD;struct _tagged_arr*_tmp3DE;struct
Cyc_Absyn_Switch_clause*_tmp3DF;struct _tuple15*_tmp3DB=(struct _tuple15*)lscs->hd;
_tmp3DC=*_tmp3DB;_tmp3DD=_tmp3DC.f1;_tmp3DE=_tmp3DC.f2;_tmp3DF=_tmp3DC.f3;{
struct Cyc_Toc_Env*_tmp3E0=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp3DF->body;if(lscs->tl != 0){struct _tuple15
_tmp3E2;struct _tagged_arr*_tmp3E3;struct Cyc_Absyn_Switch_clause*_tmp3E4;struct
_tuple15*_tmp3E1=(struct _tuple15*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;
_tmp3E2=*_tmp3E1;_tmp3E3=_tmp3E2.f2;_tmp3E4=_tmp3E2.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp3E0,end_l,_tmp3E3,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp3E4->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp3E0,
end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp3DD,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp3DE,s,0),0);stmts=({struct Cyc_List_List*_tmp3E5=_region_malloc(rgn,sizeof(*
_tmp3E5));_tmp3E5->hd=s;_tmp3E5->tl=stmts;_tmp3E5;});}}{struct Cyc_Absyn_Stmt*res=
Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple13 _tmp3E7;struct _tuple1*
_tmp3E8;void*_tmp3E9;struct _tuple13*_tmp3E6=(struct _tuple13*)((struct Cyc_List_List*)
_check_null(decls))->hd;_tmp3E7=*_tmp3E6;_tmp3E8=_tmp3E7.f1;_tmp3E9=_tmp3E7.f2;
res=Cyc_Absyn_declare_stmt(_tmp3E8,_tmp3E9,0,res,0);}(void*)(whole_s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(
rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));
_tmp3EA->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp3EA->tl=0;_tmp3EA;}),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){if(n > 0)(void*)(s->r=(
void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp3EB=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3EC;
struct Cyc_Absyn_Stmt*_tmp3ED;struct Cyc_Absyn_Stmt*_tmp3EE;struct Cyc_Absyn_Exp*
_tmp3EF;struct Cyc_Absyn_Exp*_tmp3F0;struct Cyc_Absyn_Stmt*_tmp3F1;struct Cyc_Absyn_Stmt*
_tmp3F2;struct _tuple3 _tmp3F3;struct Cyc_Absyn_Exp*_tmp3F4;struct Cyc_Absyn_Stmt*
_tmp3F5;struct Cyc_Absyn_Stmt*_tmp3F6;struct Cyc_Absyn_Stmt*_tmp3F7;struct Cyc_Absyn_Stmt*
_tmp3F8;struct Cyc_Absyn_Exp*_tmp3F9;struct _tuple3 _tmp3FA;struct Cyc_Absyn_Exp*
_tmp3FB;struct _tuple3 _tmp3FC;struct Cyc_Absyn_Exp*_tmp3FD;struct Cyc_Absyn_Stmt*
_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_List_List*_tmp400;struct Cyc_List_List*
_tmp401;struct Cyc_Absyn_Switch_clause**_tmp402;struct Cyc_Absyn_Decl*_tmp403;
struct Cyc_Absyn_Stmt*_tmp404;struct _tagged_arr*_tmp405;struct Cyc_Absyn_Stmt*
_tmp406;struct Cyc_Absyn_Stmt*_tmp407;struct _tuple3 _tmp408;struct Cyc_Absyn_Exp*
_tmp409;struct Cyc_Absyn_Stmt*_tmp40A;struct Cyc_List_List*_tmp40B;struct Cyc_Absyn_Tvar*
_tmp40C;struct Cyc_Absyn_Vardecl*_tmp40D;int _tmp40E;struct Cyc_Absyn_Stmt*_tmp40F;
struct Cyc_Absyn_Exp*_tmp410;_LL2BD: if((int)_tmp3EB != 0)goto _LL2BF;_LL2BE: return;
_LL2BF: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 0)goto _LL2C1;_tmp3EC=((struct
Cyc_Absyn_Exp_s_struct*)_tmp3EB)->f1;_LL2C0: Cyc_Toc_exp_to_c(nv,_tmp3EC);return;
_LL2C1: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 1)goto _LL2C3;_tmp3ED=((struct
Cyc_Absyn_Seq_s_struct*)_tmp3EB)->f1;_tmp3EE=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3EB)->f2;_LL2C2: Cyc_Toc_stmt_to_c(nv,_tmp3ED);s=_tmp3EE;continue;_LL2C3: if(
_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 2)goto _LL2C5;_tmp3EF=((struct Cyc_Absyn_Return_s_struct*)
_tmp3EB)->f1;_LL2C4: {struct Cyc_Core_Opt*topt=0;if(_tmp3EF != 0){topt=({struct Cyc_Core_Opt*
_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3EF->topt))->v);_tmp411;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp3EF));}if(s->try_depth > 0){if(topt != 0){
struct _tuple1*_tmp412=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp413=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp412,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp412,(void*)topt->v,_tmp3EF,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp413,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL2C5: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 3)goto _LL2C7;_tmp3F0=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3EB)->f1;_tmp3F1=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp3EB)->f2;_tmp3F2=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3EB)->f3;_LL2C6:
Cyc_Toc_exp_to_c(nv,_tmp3F0);Cyc_Toc_stmt_to_c(nv,_tmp3F1);s=_tmp3F2;continue;
_LL2C7: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 4)goto _LL2C9;_tmp3F3=((struct
Cyc_Absyn_While_s_struct*)_tmp3EB)->f1;_tmp3F4=_tmp3F3.f1;_tmp3F5=((struct Cyc_Absyn_While_s_struct*)
_tmp3EB)->f2;_LL2C8: Cyc_Toc_exp_to_c(nv,_tmp3F4);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp3F5);return;_LL2C9: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 5)goto
_LL2CB;_tmp3F6=((struct Cyc_Absyn_Break_s_struct*)_tmp3EB)->f1;_LL2CA: if(nv->break_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,0))->r));{int dest_depth=
_tmp3F6 == 0?0: _tmp3F6->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,
s);return;}_LL2CB: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 6)goto _LL2CD;
_tmp3F7=((struct Cyc_Absyn_Continue_s_struct*)_tmp3EB)->f1;_LL2CC: if(nv->continue_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0))->r));_tmp3F8=_tmp3F7;
goto _LL2CE;_LL2CD: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 7)goto _LL2CF;
_tmp3F8=((struct Cyc_Absyn_Goto_s_struct*)_tmp3EB)->f2;_LL2CE: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp3F8))->try_depth,s);
return;_LL2CF: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 8)goto _LL2D1;_tmp3F9=((
struct Cyc_Absyn_For_s_struct*)_tmp3EB)->f1;_tmp3FA=((struct Cyc_Absyn_For_s_struct*)
_tmp3EB)->f2;_tmp3FB=_tmp3FA.f1;_tmp3FC=((struct Cyc_Absyn_For_s_struct*)_tmp3EB)->f3;
_tmp3FD=_tmp3FC.f1;_tmp3FE=((struct Cyc_Absyn_For_s_struct*)_tmp3EB)->f4;_LL2D0:
Cyc_Toc_exp_to_c(nv,_tmp3F9);Cyc_Toc_exp_to_c(nv,_tmp3FB);Cyc_Toc_exp_to_c(nv,
_tmp3FD);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp3FE);return;_LL2D1: if(
_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 9)goto _LL2D3;_tmp3FF=((struct Cyc_Absyn_Switch_s_struct*)
_tmp3EB)->f1;_tmp400=((struct Cyc_Absyn_Switch_s_struct*)_tmp3EB)->f2;_LL2D2: Cyc_Toc_xlate_switch(
nv,s,_tmp3FF,_tmp400);return;_LL2D3: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 
11)goto _LL2D5;_tmp401=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp3EB)->f1;_tmp402=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp3EB)->f2;_LL2D4: if(nv->fallthru_info == 0)({
void*_tmp414[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("fallthru in unexpected place",sizeof(char),29),_tag_arr(_tmp414,
sizeof(void*),0));});{struct _tuple8 _tmp416;struct _tagged_arr*_tmp417;struct Cyc_List_List*
_tmp418;struct Cyc_Dict_Dict*_tmp419;struct _tuple8*_tmp415=(struct _tuple8*)((
struct Cyc_Core_Opt*)_check_null(nv->fallthru_info))->v;_tmp416=*_tmp415;_tmp417=
_tmp416.f1;_tmp418=_tmp416.f2;_tmp419=_tmp416.f3;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(
_tmp417,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)
_check_null(_tmp402)))->body)->try_depth,s2);{struct Cyc_List_List*_tmp41A=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp418);struct Cyc_List_List*
_tmp41B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp401);
for(0;_tmp41A != 0;(_tmp41A=_tmp41A->tl,_tmp41B=_tmp41B->tl)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp41B))->hd);s2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct
_tuple1*k))Cyc_Dict_lookup)(_tmp419,(struct _tuple1*)_tmp41A->hd),(struct Cyc_Absyn_Exp*)
_tmp41B->hd,0),s2,0);}(void*)(s->r=(void*)((void*)s2->r));return;}}}_LL2D5: if(
_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 12)goto _LL2D7;_tmp403=((struct Cyc_Absyn_Decl_s_struct*)
_tmp3EB)->f1;_tmp404=((struct Cyc_Absyn_Decl_s_struct*)_tmp3EB)->f2;_LL2D6:{void*
_tmp41C=(void*)_tmp403->r;struct Cyc_Absyn_Vardecl*_tmp41D;struct Cyc_Absyn_Pat*
_tmp41E;struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_List_List*_tmp420;_LL2EA: if(*((int*)
_tmp41C)!= 0)goto _LL2EC;_tmp41D=((struct Cyc_Absyn_Var_d_struct*)_tmp41C)->f1;
_LL2EB: {struct Cyc_Toc_Env*_tmp421=Cyc_Toc_add_varmap(nv,_tmp41D->name,Cyc_Absyn_varb_exp(
_tmp41D->name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp422=_cycalloc(sizeof(*
_tmp422));_tmp422[0]=({struct Cyc_Absyn_Local_b_struct _tmp423;_tmp423.tag=3;
_tmp423.f1=_tmp41D;_tmp423;});_tmp422;}),0));Cyc_Toc_local_decl_to_c(_tmp421,
_tmp421,_tmp41D,_tmp404);goto _LL2E9;}_LL2EC: if(*((int*)_tmp41C)!= 2)goto _LL2EE;
_tmp41E=((struct Cyc_Absyn_Let_d_struct*)_tmp41C)->f1;_tmp41F=((struct Cyc_Absyn_Let_d_struct*)
_tmp41C)->f3;_LL2ED:{void*_tmp424=(void*)_tmp41E->r;struct Cyc_Absyn_Vardecl*
_tmp425;_LL2F3: if(_tmp424 <= (void*)2?1:*((int*)_tmp424)!= 0)goto _LL2F5;_tmp425=((
struct Cyc_Absyn_Var_p_struct*)_tmp424)->f1;_LL2F4: {struct _tuple1*old_name=
_tmp425->name;struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp425->name=new_name;
_tmp425->initializer=(struct Cyc_Absyn_Exp*)_tmp41F;(void*)(_tmp403->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426[
0]=({struct Cyc_Absyn_Var_d_struct _tmp427;_tmp427.tag=0;_tmp427.f1=_tmp425;
_tmp427;});_tmp426;})));{struct Cyc_Toc_Env*_tmp428=Cyc_Toc_add_varmap(nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429[0]=({struct Cyc_Absyn_Local_b_struct
_tmp42A;_tmp42A.tag=3;_tmp42A.f1=_tmp425;_tmp42A;});_tmp429;}),0));Cyc_Toc_local_decl_to_c(
_tmp428,nv,_tmp425,_tmp404);goto _LL2F2;}}_LL2F5:;_LL2F6:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp41E,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp41F->topt))->v,_tmp41F,_tmp404))->r));goto _LL2F2;_LL2F2:;}goto _LL2E9;_LL2EE:
if(*((int*)_tmp41C)!= 3)goto _LL2F0;_tmp420=((struct Cyc_Absyn_Letv_d_struct*)
_tmp41C)->f1;_LL2EF: {struct Cyc_List_List*_tmp42B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp420);if(_tmp42B == 0)({void*_tmp42C[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("empty Letv_d",
sizeof(char),13),_tag_arr(_tmp42C,sizeof(void*),0));});(void*)(_tmp403->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D[
0]=({struct Cyc_Absyn_Var_d_struct _tmp42E;_tmp42E.tag=0;_tmp42E.f1=(struct Cyc_Absyn_Vardecl*)
_tmp42B->hd;_tmp42E;});_tmp42D;})));_tmp42B=_tmp42B->tl;for(0;_tmp42B != 0;
_tmp42B=_tmp42B->tl){struct Cyc_Absyn_Decl*_tmp42F=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430[0]=({
struct Cyc_Absyn_Var_d_struct _tmp431;_tmp431.tag=0;_tmp431.f1=(struct Cyc_Absyn_Vardecl*)
_tmp42B->hd;_tmp431;});_tmp430;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp42F,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL2E9;}_LL2F0:;_LL2F1:({void*_tmp432[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad nested declaration within function",
sizeof(char),39),_tag_arr(_tmp432,sizeof(void*),0));});_LL2E9:;}return;_LL2D7:
if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 15)goto _LL2D9;_tmp405=((struct Cyc_Absyn_Label_s_struct*)
_tmp3EB)->f1;_tmp406=((struct Cyc_Absyn_Label_s_struct*)_tmp3EB)->f2;_LL2D8: s=
_tmp406;continue;_LL2D9: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 16)goto _LL2DB;
_tmp407=((struct Cyc_Absyn_Do_s_struct*)_tmp3EB)->f1;_tmp408=((struct Cyc_Absyn_Do_s_struct*)
_tmp3EB)->f2;_tmp409=_tmp408.f1;_LL2DA: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp407);Cyc_Toc_exp_to_c(nv,_tmp409);return;_LL2DB: if(_tmp3EB <= (void*)1?1:*((
int*)_tmp3EB)!= 17)goto _LL2DD;_tmp40A=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3EB)->f1;_tmp40B=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp3EB)->f2;_LL2DC: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->v=(void*)e_typ;_tmp433;});nv=Cyc_Toc_add_varmap(
nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp40A);{struct Cyc_Absyn_Stmt*_tmp434=Cyc_Absyn_seq_stmt(
_tmp40A,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple1*_tmp435=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp436=Cyc_Absyn_var_exp(
_tmp435,0);struct Cyc_Absyn_Vardecl*_tmp437=Cyc_Absyn_new_vardecl(_tmp435,Cyc_Absyn_exn_typ,
0);_tmp436->topt=({struct Cyc_Core_Opt*_tmp438=_cycalloc(sizeof(*_tmp438));
_tmp438->v=(void*)Cyc_Absyn_exn_typ;_tmp438;});{struct Cyc_Absyn_Pat*_tmp439=({
struct Cyc_Absyn_Pat*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B[0]=({
struct Cyc_Absyn_Var_p_struct _tmp44C;_tmp44C.tag=0;_tmp44C.f1=_tmp437;_tmp44C;});
_tmp44B;}));_tmp449->topt=({struct Cyc_Core_Opt*_tmp44A=_cycalloc(sizeof(*_tmp44A));
_tmp44A->v=(void*)Cyc_Absyn_exn_typ;_tmp44A;});_tmp449->loc=0;_tmp449;});struct
Cyc_Absyn_Exp*_tmp43A=Cyc_Absyn_throw_exp(_tmp436,0);_tmp43A->topt=({struct Cyc_Core_Opt*
_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->v=(void*)((void*)0);_tmp43B;});{
struct Cyc_Absyn_Stmt*_tmp43C=Cyc_Absyn_exp_stmt(_tmp43A,0);struct Cyc_Absyn_Switch_clause*
_tmp43D=({struct Cyc_Absyn_Switch_clause*_tmp446=_cycalloc(sizeof(*_tmp446));
_tmp446->pattern=_tmp439;_tmp446->pat_vars=({struct Cyc_Core_Opt*_tmp447=
_cycalloc(sizeof(*_tmp447));_tmp447->v=({struct Cyc_List_List*_tmp448=_cycalloc(
sizeof(*_tmp448));_tmp448->hd=_tmp437;_tmp448->tl=0;_tmp448;});_tmp447;});
_tmp446->where_clause=0;_tmp446->body=_tmp43C;_tmp446->loc=0;_tmp446;});struct
Cyc_Absyn_Stmt*_tmp43E=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp40B,({struct
Cyc_List_List*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->hd=_tmp43D;_tmp445->tl=
0;_tmp445;})),0);Cyc_Toc_stmt_to_c(nv,_tmp43E);{struct Cyc_Absyn_Exp*_tmp43F=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp444=_cycalloc(sizeof(*_tmp444));
_tmp444->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp444->tl=0;
_tmp444;}),0);struct Cyc_Absyn_Stmt*_tmp440=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp443=_cycalloc(sizeof(*_tmp443));
_tmp443->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp443->tl=0;_tmp443;}),0),0);struct
Cyc_Absyn_Exp*_tmp441=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp442=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp440,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp441,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp43F,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp442,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp434,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_cast_exp(
e_typ,Cyc_Toc__exn_thrown_e,0),_tmp43E,0),0),0),0),0),0))->r));return;}}}}}
_LL2DD: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 18)goto _LL2DF;_tmp40C=((struct
Cyc_Absyn_Region_s_struct*)_tmp3EB)->f1;_tmp40D=((struct Cyc_Absyn_Region_s_struct*)
_tmp3EB)->f2;_tmp40E=((struct Cyc_Absyn_Region_s_struct*)_tmp3EB)->f3;_tmp40F=((
struct Cyc_Absyn_Region_s_struct*)_tmp3EB)->f4;_LL2DE: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp40D->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
nv,x_var,x_exp),_tmp40F);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp44D->tl=0;_tmp44D;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp44E[1];_tmp44E[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp44E,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),Cyc_Absyn_seq_stmt(_tmp40F,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp*_tmp44F[1];_tmp44F[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp44F,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL2DF: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)
!= 20)goto _LL2E1;_tmp410=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp3EB)->f1;
_LL2E0: Cyc_Toc_exp_to_c(nv,_tmp410);(void*)(s->r=(void*)((void*)(Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp450=
_cycalloc(sizeof(*_tmp450));_tmp450->hd=_tmp410;_tmp450->tl=0;_tmp450;}),0),0))->r));
return;_LL2E1: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 10)goto _LL2E3;_LL2E2:({
void*_tmp451[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("switchC_s",sizeof(char),10),_tag_arr(_tmp451,sizeof(void*),0));});
_LL2E3: if(_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 13)goto _LL2E5;_LL2E4:({void*
_tmp452[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("cut",sizeof(char),4),_tag_arr(_tmp452,sizeof(void*),0));});_LL2E5: if(
_tmp3EB <= (void*)1?1:*((int*)_tmp3EB)!= 14)goto _LL2E7;_LL2E6:({void*_tmp453[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("splice",
sizeof(char),7),_tag_arr(_tmp453,sizeof(void*),0));});_LL2E7: if(_tmp3EB <= (void*)
1?1:*((int*)_tmp3EB)!= 19)goto _LL2BC;_LL2E8:({void*_tmp454[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("forarray",sizeof(
char),9),_tag_arr(_tmp454,sizeof(void*),0));});_LL2BC:;}}struct _tuple16{struct
_tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f){f->tvs=0;f->effect=0;f->rgn_po=0;(
void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{struct Cyc_List_List*
_tmp455=f->args;for(0;_tmp455 != 0;_tmp455=_tmp455->tl){(*((struct _tuple16*)
_tmp455->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple16*)_tmp455->hd)).f3);{struct
_tuple1*_tmp456=({struct _tuple1*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->f1=(
void*)0;_tmp457->f2=(*((struct _tuple16*)_tmp455->hd)).f1;_tmp457;});nv=Cyc_Toc_add_varmap(
nv,_tmp456,Cyc_Absyn_var_exp(_tmp456,0));}}}if((unsigned int)f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0: 0){struct Cyc_Core_Opt*
_tmp459;struct Cyc_Absyn_Tqual _tmp45A;void*_tmp45B;int _tmp45C;struct Cyc_Absyn_VarargInfo
_tmp458=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp459=
_tmp458.name;_tmp45A=_tmp458.tq;_tmp45B=(void*)_tmp458.type;_tmp45C=_tmp458.inject;{
void*_tmp45D=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ(_tmp45B,(void*)2,_tmp45A));
struct _tuple1*_tmp45E=({struct _tuple1*_tmp461=_cycalloc(sizeof(*_tmp461));
_tmp461->f1=(void*)0;_tmp461->f2=(struct _tagged_arr*)((struct Cyc_Core_Opt*)
_check_null(_tmp459))->v;_tmp461;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp45F=
_cycalloc(sizeof(*_tmp45F));_tmp45F->hd=({struct _tuple16*_tmp460=_cycalloc(
sizeof(*_tmp460));_tmp460->f1=(struct _tagged_arr*)_tmp459->v;_tmp460->f2=_tmp45A;
_tmp460->f3=_tmp45D;_tmp460;});_tmp45F->tl=0;_tmp45F;}));nv=Cyc_Toc_add_varmap(
nv,_tmp45E,Cyc_Absyn_var_exp(_tmp45E,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp462=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp462 != 0;_tmp462=_tmp462->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp462->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp462->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(nv),f->body);}
static void*Cyc_Toc_scope_to_c(void*s){void*_tmp463=s;_LL2F8: if((int)_tmp463 != 1)
goto _LL2FA;_LL2F9: return(void*)2;_LL2FA: if((int)_tmp463 != 4)goto _LL2FC;_LL2FB:
return(void*)3;_LL2FC:;_LL2FD: return s;_LL2F7:;}struct Cyc_Core_Opt*Cyc_Toc_aggrs_so_far=
0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*
_tmp464=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({struct Cyc_Core_Opt*
_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->v=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);
_tmp465;});{int seen_defn_before;struct Cyc_Core_Opt*_tmp466=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*_tmp464).f2);if(
_tmp466 == 0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*_tmp464).f2,ad));}
else{if(((struct Cyc_Absyn_Aggrdecl*)_tmp466->v)->impl == 0){(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*
_tmp464).f2,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(
void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp467=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp467 != 0;_tmp467=_tmp467->tl){(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp467->hd)->type=(
void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Absyn_Aggrfield*)_tmp467->hd)->type));}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({
struct Cyc_Core_Opt*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->v=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);
_tmp468;});{struct _tuple1*_tmp469=tud->name;if(tud->fields == 0?1:((int(*)(struct
Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp469))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp469));{struct Cyc_List_List*
_tmp46A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp46A != 0;_tmp46A=_tmp46A->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp46A->hd;if(f->typs != 0){struct Cyc_List_List*_tmp46B=0;int i=1;{struct Cyc_List_List*
_tmp46C=f->typs;for(0;_tmp46C != 0;(_tmp46C=_tmp46C->tl,i ++)){struct _tagged_arr*
_tmp46D=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*_tmp46E=({struct Cyc_Absyn_Aggrfield*
_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->name=_tmp46D;_tmp470->tq=(*((struct
_tuple4*)_tmp46C->hd)).f1;_tmp470->type=(void*)Cyc_Toc_typ_to_c_array((*((struct
_tuple4*)_tmp46C->hd)).f2);_tmp470->width=0;_tmp470->attributes=0;_tmp470;});
_tmp46B=({struct Cyc_List_List*_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F->hd=
_tmp46E;_tmp46F->tl=_tmp46B;_tmp46F;});}}_tmp46B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp46B);_tmp46B=({struct Cyc_List_List*
_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->hd=({struct Cyc_Absyn_Aggrfield*
_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->name=Cyc_Toc_tag_sp;_tmp472->tq=Cyc_Toc_mt_tq;
_tmp472->type=(void*)Cyc_Absyn_sint_typ;_tmp472->width=0;_tmp472->attributes=0;
_tmp472;});_tmp471->tl=_tmp46B;_tmp471;});{struct Cyc_Absyn_Aggrdecl*_tmp473=({
struct Cyc_Absyn_Aggrdecl*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->kind=(void*)((
void*)0);_tmp477->sc=(void*)((void*)2);_tmp477->name=Cyc_Toc_collapse_qvar_tag(f->name,
_tag_arr("_struct",sizeof(char),8));_tmp477->tvs=0;_tmp477->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->exist_vars=0;_tmp478->rgn_po=0;
_tmp478->fields=_tmp46B;_tmp478;});_tmp477->attributes=0;_tmp477;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp475=_cycalloc(sizeof(*_tmp475));
_tmp475[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp476;_tmp476.tag=4;_tmp476.f1=
_tmp473;_tmp476;});_tmp475;}),0);_tmp474->tl=Cyc_Toc_result_decls;_tmp474;});}}}}}}
static struct Cyc_Core_Opt*Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({
struct Cyc_Core_Opt*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp479;});if(xd->fields == 0)return;{struct _tuple1*_tmp47A=xd->name;struct Cyc_List_List*
_tmp47B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp47B != 0;_tmp47B=_tmp47B->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp47B->hd;struct _tagged_arr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp47C=Cyc_Absyn_uint_exp(
_get_arr_size(*fn,sizeof(char))+ 4,0);struct Cyc_Absyn_ArrayType_struct*_tmp47D=({
struct Cyc_Absyn_ArrayType_struct*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp4A7;_tmp4A7.tag=7;_tmp4A7.f1=(void*)Cyc_Absyn_char_typ;
_tmp4A7.f2=Cyc_Toc_mt_tq;_tmp4A7.f3=(struct Cyc_Absyn_Exp*)_tmp47C;_tmp4A7;});
_tmp4A6;});struct Cyc_Core_Opt*_tmp47E=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_xtunion_fields_so_far))->v,f->name);struct Cyc_Core_Opt
_tmp47F;int _tmp480;_LL2FF: if(_tmp47E != 0)goto _LL301;_LL300: {struct Cyc_Absyn_Exp*
initopt=0;if((void*)f->sc != (void*)3)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp486;_tmp486.tag=0;_tmp486.f1=(
struct _tagged_arr)*fn;{struct Cyc_Std_Int_pa_struct _tmp485;_tmp485.tag=1;_tmp485.f1=(
int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct _tmp484;_tmp484.tag=
1;_tmp484.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp483;_tmp483.tag=1;_tmp483.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp482;_tmp482.tag=1;_tmp482.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp481[5]={& _tmp482,& _tmp483,& _tmp484,& _tmp485,& _tmp486};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(char),11),_tag_arr(_tmp481,sizeof(void*),5));}}}}}}),
0);{struct Cyc_Absyn_Vardecl*_tmp487=Cyc_Absyn_new_vardecl(f->name,(void*)_tmp47D,
initopt);(void*)(_tmp487->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489[
0]=({struct Cyc_Absyn_Var_d_struct _tmp48A;_tmp48A.tag=0;_tmp48A.f1=_tmp487;
_tmp48A;});_tmp489;}),0);_tmp488->tl=Cyc_Toc_result_decls;_tmp488;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp48B=f->typs;for(0;_tmp48B != 0;(_tmp48B=_tmp48B->tl,
i ++)){struct _tagged_arr*_tmp48C=({struct _tagged_arr*_tmp490=_cycalloc(sizeof(*
_tmp490));_tmp490[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp492;
_tmp492.tag=1;_tmp492.f1=(int)((unsigned int)i);{void*_tmp491[1]={& _tmp492};Cyc_Std_aprintf(
_tag_arr("f%d",sizeof(char),4),_tag_arr(_tmp491,sizeof(void*),1));}});_tmp490;});
struct Cyc_Absyn_Aggrfield*_tmp48D=({struct Cyc_Absyn_Aggrfield*_tmp48F=_cycalloc(
sizeof(*_tmp48F));_tmp48F->name=_tmp48C;_tmp48F->tq=(*((struct _tuple4*)_tmp48B->hd)).f1;
_tmp48F->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)_tmp48B->hd)).f2);
_tmp48F->width=0;_tmp48F->attributes=0;_tmp48F;});fields=({struct Cyc_List_List*
_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E->hd=_tmp48D;_tmp48E->tl=fields;
_tmp48E;});}}fields=({struct Cyc_List_List*_tmp493=_cycalloc(sizeof(*_tmp493));
_tmp493->hd=({struct Cyc_Absyn_Aggrfield*_tmp494=_cycalloc(sizeof(*_tmp494));
_tmp494->name=Cyc_Toc_tag_sp;_tmp494->tq=Cyc_Toc_mt_tq;_tmp494->type=(void*)Cyc_Absyn_cstar_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp494->width=0;_tmp494->attributes=0;_tmp494;});
_tmp493->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp493;});{struct Cyc_Absyn_Aggrdecl*_tmp495=({struct Cyc_Absyn_Aggrdecl*
_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->kind=(void*)((void*)0);_tmp499->sc=(
void*)((void*)2);_tmp499->name=Cyc_Toc_collapse_qvar_tag(f->name,_tag_arr("_struct",
sizeof(char),8));_tmp499->tvs=0;_tmp499->impl=({struct Cyc_Absyn_AggrdeclImpl*
_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A->exist_vars=0;_tmp49A->rgn_po=0;
_tmp49A->fields=fields;_tmp49A;});_tmp499->attributes=0;_tmp499;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp497=_cycalloc(sizeof(*_tmp497));
_tmp497[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp498;_tmp498.tag=4;_tmp498.f1=
_tmp495;_tmp498;});_tmp497;}),0);_tmp496->tl=Cyc_Toc_result_decls;_tmp496;});}}
goto _LL2FE;}}_LL301: if(_tmp47E == 0)goto _LL303;_tmp47F=*_tmp47E;_tmp480=(int)
_tmp47F.v;if(_tmp480 != 0)goto _LL303;_LL302: if((void*)f->sc != (void*)3){struct Cyc_Absyn_Exp*
_tmp49B=Cyc_Absyn_string_exp((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp4A5;_tmp4A5.tag=0;_tmp4A5.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_Int_pa_struct
_tmp4A4;_tmp4A4.tag=1;_tmp4A4.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp4A3;_tmp4A3.tag=1;_tmp4A3.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp4A2;_tmp4A2.tag=1;_tmp4A2.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp4A1;_tmp4A1.tag=1;_tmp4A1.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp4A0[5]={& _tmp4A1,& _tmp4A2,& _tmp4A3,& _tmp4A4,& _tmp4A5};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(char),11),_tag_arr(_tmp4A0,sizeof(void*),5));}}}}}}),
0);struct Cyc_Absyn_Vardecl*_tmp49C=Cyc_Absyn_new_vardecl(f->name,(void*)_tmp47D,(
struct Cyc_Absyn_Exp*)_tmp49B);(void*)(_tmp49C->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E[
0]=({struct Cyc_Absyn_Var_d_struct _tmp49F;_tmp49F.tag=0;_tmp49F.f1=_tmp49C;
_tmp49F;});_tmp49E;}),0);_tmp49D->tl=Cyc_Toc_result_decls;_tmp49D;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL2FE;_LL303:;_LL304: goto _LL2FE;_LL2FE:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array((void*)vd->type));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp4A8=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4AB;_LL306: if(*((int*)
_tmp4A8)!= 27)goto _LL308;_tmp4A9=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4A8)->f1;_tmp4AA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4A8)->f2;
_tmp4AB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4A8)->f3;_LL307: vd->initializer=
0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(
vd->name,0),_tmp4A9,_tmp4AA,_tmp4AB,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL305;_LL308:;_LL309: if((void*)vd->sc == (void*)0){struct Cyc_Toc_Env*_tmp4AC=({
struct Cyc_Toc_Env*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=*init_nv;
_tmp4AD;});_tmp4AC->toplevel=1;Cyc_Toc_exp_to_c(_tmp4AC,init);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL305;_LL305:;}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=
0;static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp4AE=_cycalloc(
sizeof(*_tmp4AE));_tmp4AE[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp4AE;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp4AF=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp4AF;_push_region(prgn);{struct Cyc_Toc_Env*_tmp4B1;
struct Cyc_List_List*_tmp4B2;struct Cyc_Absyn_Stmt*_tmp4B3;struct _tuple12 _tmp4B0=
Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp4B1=_tmp4B0.f1;_tmp4B2=_tmp4B0.f2;_tmp4B3=_tmp4B0.f3;Cyc_Toc_stmt_to_c(
_tmp4B1,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp4B3,s,0),0);for(0;_tmp4B2 != 0;_tmp4B2=_tmp4B2->tl){
struct _tuple13 _tmp4B5;struct _tuple1*_tmp4B6;void*_tmp4B7;struct _tuple13*_tmp4B4=(
struct _tuple13*)_tmp4B2->hd;_tmp4B5=*_tmp4B4;_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;
s=Cyc_Absyn_declare_stmt(_tmp4B6,_tmp4B7,0,s,0);}};_pop_region(prgn);}return s;}}
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top){for(0;ds != 0;ds=ds->tl){if(!nv->toplevel)({void*_tmp4B8[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("decls_to_c: not at toplevel!",
sizeof(char),29),_tag_arr(_tmp4B8,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(
struct Cyc_Absyn_Decl*)ds->hd;void*_tmp4B9=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp4BA;struct Cyc_Absyn_Fndecl*_tmp4BB;struct Cyc_Absyn_Aggrdecl*_tmp4BC;struct
Cyc_Absyn_Tuniondecl*_tmp4BD;struct Cyc_Absyn_Enumdecl*_tmp4BE;struct Cyc_Absyn_Typedefdecl*
_tmp4BF;struct Cyc_List_List*_tmp4C0;struct Cyc_List_List*_tmp4C1;struct Cyc_List_List*
_tmp4C2;_LL30B: if(*((int*)_tmp4B9)!= 0)goto _LL30D;_tmp4BA=((struct Cyc_Absyn_Var_d_struct*)
_tmp4B9)->f1;_LL30C: {struct _tuple1*_tmp4C3=_tmp4BA->name;if((void*)_tmp4BA->sc
== (void*)4)_tmp4C3=({struct _tuple1*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));_tmp4C4->f1=
Cyc_Absyn_rel_ns_null;_tmp4C4->f2=(*_tmp4C3).f2;_tmp4C4;});if(_tmp4BA->initializer
!= 0)Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp4BA->initializer));
nv=Cyc_Toc_add_varmap(nv,_tmp4BA->name,Cyc_Absyn_varb_exp(_tmp4C3,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5[0]=({
struct Cyc_Absyn_Global_b_struct _tmp4C6;_tmp4C6.tag=0;_tmp4C6.f1=_tmp4BA;_tmp4C6;});
_tmp4C5;}),0));_tmp4BA->name=_tmp4C3;(void*)(_tmp4BA->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp4BA->sc));(void*)(_tmp4BA->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp4BA->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4C7=_cycalloc(
sizeof(*_tmp4C7));_tmp4C7->hd=d;_tmp4C7->tl=Cyc_Toc_result_decls;_tmp4C7;});goto
_LL30A;}_LL30D: if(*((int*)_tmp4B9)!= 1)goto _LL30F;_tmp4BB=((struct Cyc_Absyn_Fn_d_struct*)
_tmp4B9)->f1;_LL30E: {struct _tuple1*_tmp4C8=_tmp4BB->name;if((void*)_tmp4BB->sc
== (void*)4)_tmp4C8=({struct _tuple1*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9->f1=
Cyc_Absyn_rel_ns_null;_tmp4C9->f2=(*_tmp4C8).f2;_tmp4C9;});nv=Cyc_Toc_add_varmap(
nv,_tmp4BB->name,Cyc_Absyn_var_exp(_tmp4C8,0));_tmp4BB->name=_tmp4C8;Cyc_Toc_fndecl_to_c(
nv,_tmp4BB);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4CA=_cycalloc(sizeof(*
_tmp4CA));_tmp4CA->hd=d;_tmp4CA->tl=Cyc_Toc_result_decls;_tmp4CA;});goto _LL30A;}
_LL30F: if(*((int*)_tmp4B9)!= 2)goto _LL311;_LL310: goto _LL312;_LL311: if(*((int*)
_tmp4B9)!= 3)goto _LL313;_LL312:({void*_tmp4CB[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("letdecl at toplevel",
sizeof(char),20),_tag_arr(_tmp4CB,sizeof(void*),0));});_LL313: if(*((int*)_tmp4B9)
!= 4)goto _LL315;_tmp4BC=((struct Cyc_Absyn_Aggr_d_struct*)_tmp4B9)->f1;_LL314: Cyc_Toc_aggrdecl_to_c(
_tmp4BC);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4CC=_cycalloc(sizeof(*
_tmp4CC));_tmp4CC->hd=d;_tmp4CC->tl=Cyc_Toc_result_decls;_tmp4CC;});goto _LL30A;
_LL315: if(*((int*)_tmp4B9)!= 5)goto _LL317;_tmp4BD=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp4B9)->f1;_LL316: if(_tmp4BD->is_xtunion)Cyc_Toc_xtuniondecl_to_c(_tmp4BD);
else{Cyc_Toc_tuniondecl_to_c(_tmp4BD);}goto _LL30A;_LL317: if(*((int*)_tmp4B9)!= 6)
goto _LL319;_tmp4BE=((struct Cyc_Absyn_Enum_d_struct*)_tmp4B9)->f1;_LL318: Cyc_Toc_enumdecl_to_c(
nv,_tmp4BE);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4CD=_cycalloc(sizeof(*
_tmp4CD));_tmp4CD->hd=d;_tmp4CD->tl=Cyc_Toc_result_decls;_tmp4CD;});goto _LL30A;
_LL319: if(*((int*)_tmp4B9)!= 7)goto _LL31B;_tmp4BF=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp4B9)->f1;_LL31A: _tmp4BF->name=_tmp4BF->name;_tmp4BF->tvs=0;if(_tmp4BF->defn
!= 0)_tmp4BF->defn=({struct Cyc_Core_Opt*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));
_tmp4CE->v=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4BF->defn))->v);_tmp4CE;});else{void*_tmp4CF=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4BF->kind))->v;_LL322: if((int)_tmp4CF != 2)goto _LL324;_LL323:
_tmp4BF->defn=({struct Cyc_Core_Opt*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->v=(
void*)Cyc_Absyn_void_star_typ();_tmp4D0;});goto _LL321;_LL324:;_LL325: _tmp4BF->defn=({
struct Cyc_Core_Opt*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1->v=(void*)((void*)
0);_tmp4D1;});goto _LL321;_LL321:;}Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2->hd=d;_tmp4D2->tl=Cyc_Toc_result_decls;
_tmp4D2;});goto _LL30A;_LL31B: if(*((int*)_tmp4B9)!= 8)goto _LL31D;_tmp4C0=((struct
Cyc_Absyn_Namespace_d_struct*)_tmp4B9)->f2;_LL31C: _tmp4C1=_tmp4C0;goto _LL31E;
_LL31D: if(*((int*)_tmp4B9)!= 9)goto _LL31F;_tmp4C1=((struct Cyc_Absyn_Using_d_struct*)
_tmp4B9)->f2;_LL31E: _tmp4C2=_tmp4C1;goto _LL320;_LL31F: if(*((int*)_tmp4B9)!= 10)
goto _LL30A;_tmp4C2=((struct Cyc_Absyn_ExternC_d_struct*)_tmp4B9)->f1;_LL320: nv=
Cyc_Toc_decls_to_c(nv,_tmp4C2,top);goto _LL30A;_LL30A:;}}return nv;}static void Cyc_Toc_init(){
Cyc_Toc_result_decls=0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=
0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(),ds,1);return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
