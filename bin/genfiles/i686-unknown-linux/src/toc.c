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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
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
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int
Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct _tagged_arr Cyc_Std_strconcat(struct
_tagged_arr,struct _tagged_arr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict*d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
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
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};extern char Cyc_CfFlowInfo_IsZero[11];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{
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
_tuple5{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
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
_tmp53 != 0;_tmp53=_tmp53->tl){if(!Cyc_Toc_is_zero((*((struct _tuple5*)_tmp53->hd)).f2))
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
tag);_tmp59;});_tmp58;});}struct _tuple6{struct _tagged_arr*f1;struct Cyc_List_List*
f2;};static struct _tagged_arr*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){{
struct Cyc_List_List*_tmp5A=Cyc_Toc_tuple_types;for(0;_tmp5A != 0;_tmp5A=_tmp5A->tl){
struct _tuple6 _tmp5C;struct _tagged_arr*_tmp5D;struct Cyc_List_List*_tmp5E;struct
_tuple6*_tmp5B=(struct _tuple6*)_tmp5A->hd;_tmp5C=*_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=
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
_tmp6B;});_tmp69->tvs=0;_tmp69->exist_vars=0;_tmp69->fields=({struct Cyc_Core_Opt*
_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->v=_tmp60;_tmp6A;});_tmp69->attributes=
0;_tmp69;});Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*
_tmp64));_tmp64->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp66;_tmp66.tag=4;_tmp66.f1=_tmp63;_tmp66;});_tmp65;}),0);_tmp64->tl=Cyc_Toc_result_decls;
_tmp64;});Cyc_Toc_tuple_types=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*
_tmp67));_tmp67->hd=({struct _tuple6*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->f1=
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
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){struct Cyc_List_List*
_tmp8B=fs;for(0;_tmp8B != 0;_tmp8B=_tmp8B->tl){if(((struct Cyc_Absyn_Enumfield*)
_tmp8B->hd)->tag != 0){unsigned int i=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(((struct Cyc_Absyn_Enumfield*)_tmp8B->hd)->tag));((struct Cyc_Absyn_Enumfield*)
_tmp8B->hd)->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(i,0);}}}static void*Cyc_Toc_typ_to_c(
void*t){void*_tmp8C=t;struct Cyc_Core_Opt*_tmp8D;struct Cyc_Core_Opt*_tmp8E;struct
Cyc_Core_Opt _tmp8F;void*_tmp90;struct Cyc_Absyn_Tvar*_tmp91;struct Cyc_Absyn_TunionInfo
_tmp92;void*_tmp93;struct Cyc_Absyn_TunionFieldInfo _tmp94;void*_tmp95;struct Cyc_Absyn_Tuniondecl*
_tmp96;struct Cyc_Absyn_Tunionfield*_tmp97;struct Cyc_Absyn_PtrInfo _tmp98;void*
_tmp99;struct Cyc_Absyn_Tqual _tmp9A;struct Cyc_Absyn_Conref*_tmp9B;void*_tmp9C;
struct Cyc_Absyn_Tqual _tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_FnInfo
_tmp9F;void*_tmpA0;struct Cyc_List_List*_tmpA1;int _tmpA2;struct Cyc_Absyn_VarargInfo*
_tmpA3;struct Cyc_List_List*_tmpA4;struct Cyc_List_List*_tmpA5;void*_tmpA6;struct
Cyc_List_List*_tmpA7;struct Cyc_Absyn_AggrInfo _tmpA8;void*_tmpA9;struct Cyc_List_List*
_tmpAA;struct _tuple1*_tmpAB;struct Cyc_List_List*_tmpAC;struct _tuple1*_tmpAD;
struct Cyc_List_List*_tmpAE;struct Cyc_Absyn_Typedefdecl*_tmpAF;struct Cyc_Core_Opt*
_tmpB0;void*_tmpB1;_LL26: if((int)_tmp8C != 0)goto _LL28;_LL27: return t;_LL28: if(
_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 0)goto _LL2A;_tmp8D=((struct Cyc_Absyn_Evar_struct*)
_tmp8C)->f2;if(_tmp8D != 0)goto _LL2A;_LL29: return Cyc_Absyn_sint_t;_LL2A: if(_tmp8C
<= (void*)3?1:*((int*)_tmp8C)!= 0)goto _LL2C;_tmp8E=((struct Cyc_Absyn_Evar_struct*)
_tmp8C)->f2;if(_tmp8E == 0)goto _LL2C;_tmp8F=*_tmp8E;_tmp90=(void*)_tmp8F.v;_LL2B:
return Cyc_Toc_typ_to_c(_tmp90);_LL2C: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 1)
goto _LL2E;_tmp91=((struct Cyc_Absyn_VarType_struct*)_tmp8C)->f1;_LL2D: if(Cyc_Tcutil_tvar_kind(
_tmp91)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL2E:
if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 2)goto _LL30;_tmp92=((struct Cyc_Absyn_TunionType_struct*)
_tmp8C)->f1;_tmp93=(void*)_tmp92.tunion_info;if(*((int*)_tmp93)!= 1)goto _LL30;
_LL2F: return Cyc_Absyn_void_star_typ();_LL30: if(_tmp8C <= (void*)3?1:*((int*)
_tmp8C)!= 2)goto _LL32;_LL31:({void*_tmpB2[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unresolved TunionType",sizeof(
char),22),_tag_arr(_tmpB2,sizeof(void*),0));});_LL32: if(_tmp8C <= (void*)3?1:*((
int*)_tmp8C)!= 3)goto _LL34;_tmp94=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp8C)->f1;_tmp95=(void*)_tmp94.field_info;if(*((int*)_tmp95)!= 1)goto _LL34;
_tmp96=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp95)->f1;_tmp97=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp95)->f2;_LL33: if(_tmp97->typs == 0){if(_tmp96->is_xtunion)return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,(void*)2,Cyc_Toc_mt_tq);else{return Cyc_Absyn_uint_t;}}else{
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp97->name,_tag_arr("_struct",
sizeof(char),8)));}_LL34: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 3)goto _LL36;
_LL35:({void*_tmpB3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unresolved TunionFieldType",sizeof(char),27),_tag_arr(_tmpB3,sizeof(
void*),0));});_LL36: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 4)goto _LL38;_tmp98=((
struct Cyc_Absyn_PointerType_struct*)_tmp8C)->f1;_tmp99=(void*)_tmp98.elt_typ;
_tmp9A=_tmp98.tq;_tmp9B=_tmp98.bounds;_LL37: _tmp99=Cyc_Toc_typ_to_c_array(_tmp99);{
void*_tmpB4=(void*)(Cyc_Absyn_compress_conref(_tmp9B))->v;void*_tmpB5;_LL5B: if(
_tmpB4 <= (void*)1?1:*((int*)_tmpB4)!= 0)goto _LL5D;_tmpB5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpB4)->f1;if((int)_tmpB5 != 0)goto _LL5D;_LL5C: return Cyc_Toc_tagged_arr_typ;
_LL5D:;_LL5E: return Cyc_Absyn_star_typ(_tmp99,(void*)2,_tmp9A);_LL5A:;}_LL38: if(
_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 5)goto _LL3A;_LL39: goto _LL3B;_LL3A: if((int)
_tmp8C != 1)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)
!= 6)goto _LL3E;_LL3D: return t;_LL3E: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 7)
goto _LL40;_tmp9C=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp8C)->f1;_tmp9D=((
struct Cyc_Absyn_ArrayType_struct*)_tmp8C)->f2;_tmp9E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp8C)->f3;_LL3F: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpB6=
_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_ArrayType_struct _tmpB7;
_tmpB7.tag=7;_tmpB7.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp9C);_tmpB7.f2=_tmp9D;
_tmpB7.f3=_tmp9E;_tmpB7;});_tmpB6;});_LL40: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)
!= 8)goto _LL42;_tmp9F=((struct Cyc_Absyn_FnType_struct*)_tmp8C)->f1;_tmpA0=(void*)
_tmp9F.ret_typ;_tmpA1=_tmp9F.args;_tmpA2=_tmp9F.c_varargs;_tmpA3=_tmp9F.cyc_varargs;
_tmpA4=_tmp9F.attributes;_LL41: {struct Cyc_List_List*_tmpB8=0;for(0;_tmpA4 != 0;
_tmpA4=_tmpA4->tl){void*_tmpB9=(void*)_tmpA4->hd;_LL60: if((int)_tmpB9 != 3)goto
_LL62;_LL61: goto _LL63;_LL62: if((int)_tmpB9 != 4)goto _LL64;_LL63: goto _LL65;_LL64:
if(_tmpB9 <= (void*)16?1:*((int*)_tmpB9)!= 3)goto _LL66;_LL65: continue;_LL66:;
_LL67: _tmpB8=({struct Cyc_List_List*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->hd=(
void*)((void*)_tmpA4->hd);_tmpBA->tl=_tmpB8;_tmpBA;});goto _LL5F;_LL5F:;}{struct
Cyc_List_List*_tmpBB=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmpA1);if(_tmpA3 != 0){
void*_tmpBC=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ((void*)_tmpA3->type,(void*)2,
Cyc_Toc_mt_tq));struct _tuple2*_tmpBD=({struct _tuple2*_tmpBF=_cycalloc(sizeof(*
_tmpBF));_tmpBF->f1=_tmpA3->name;_tmpBF->f2=_tmpA3->tq;_tmpBF->f3=_tmpBC;_tmpBF;});
_tmpBB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmpBB,({struct Cyc_List_List*_tmpBE=_cycalloc(sizeof(*
_tmpBE));_tmpBE->hd=_tmpBD;_tmpBE->tl=0;_tmpBE;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_FnType_struct
_tmpC1;_tmpC1.tag=8;_tmpC1.f1=({struct Cyc_Absyn_FnInfo _tmpC2;_tmpC2.tvars=0;
_tmpC2.effect=0;_tmpC2.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmpA0);_tmpC2.args=
_tmpBB;_tmpC2.c_varargs=_tmpA2;_tmpC2.cyc_varargs=0;_tmpC2.rgn_po=0;_tmpC2.attributes=
_tmpB8;_tmpC2;});_tmpC1;});_tmpC0;});}}_LL42: if(_tmp8C <= (void*)3?1:*((int*)
_tmp8C)!= 9)goto _LL44;_tmpA5=((struct Cyc_Absyn_TupleType_struct*)_tmp8C)->f1;
_LL43: _tmpA5=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmpA5);{struct _tagged_arr*
_tmpC3=Cyc_Toc_add_tuple_type(_tmpA5);return Cyc_Absyn_strct(_tmpC3);}_LL44: if(
_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 11)goto _LL46;_tmpA6=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp8C)->f1;_tmpA7=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp8C)->f2;_LL45:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpC4=_cycalloc(sizeof(*
_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpC5;_tmpC5.tag=11;
_tmpC5.f1=(void*)_tmpA6;_tmpC5.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmpA7);_tmpC5;});_tmpC4;});_LL46: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 10)
goto _LL48;_tmpA8=((struct Cyc_Absyn_AggrType_struct*)_tmp8C)->f1;_tmpA9=(void*)
_tmpA8.aggr_info;_tmpAA=_tmpA8.targs;_LL47: {struct Cyc_Absyn_Aggrdecl*_tmpC6=Cyc_Absyn_get_known_aggrdecl(
_tmpA9);if((void*)_tmpC6->kind == (void*)1)return Cyc_Absyn_unionq_typ(_tmpC6->name);
return Cyc_Absyn_strctq(_tmpC6->name);}_LL48: if(_tmp8C <= (void*)3?1:*((int*)
_tmp8C)!= 12)goto _LL4A;_tmpAB=((struct Cyc_Absyn_EnumType_struct*)_tmp8C)->f1;
_LL49: return t;_LL4A: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 13)goto _LL4C;_tmpAC=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp8C)->f1;_LL4B: Cyc_Toc_enumfields_to_c(
_tmpAC);return t;_LL4C: if(_tmp8C <= (void*)3?1:*((int*)_tmp8C)!= 16)goto _LL4E;
_tmpAD=((struct Cyc_Absyn_TypedefType_struct*)_tmp8C)->f1;_tmpAE=((struct Cyc_Absyn_TypedefType_struct*)
_tmp8C)->f2;_tmpAF=((struct Cyc_Absyn_TypedefType_struct*)_tmp8C)->f3;_tmpB0=((
struct Cyc_Absyn_TypedefType_struct*)_tmp8C)->f4;_LL4D: if(_tmpB0 == 0)return(void*)({
struct Cyc_Absyn_TypedefType_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({
struct Cyc_Absyn_TypedefType_struct _tmpC8;_tmpC8.tag=16;_tmpC8.f1=_tmpAD;_tmpC8.f2=
0;_tmpC8.f3=_tmpAF;_tmpC8.f4=0;_tmpC8;});_tmpC7;});else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_TypedefType_struct
_tmpCA;_tmpCA.tag=16;_tmpCA.f1=_tmpAD;_tmpCA.f2=0;_tmpCA.f3=_tmpAF;_tmpCA.f4=({
struct Cyc_Core_Opt*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->v=(void*)Cyc_Toc_typ_to_c_array((
void*)_tmpB0->v);_tmpCB;});_tmpCA;});_tmpC9;});}_LL4E: if(_tmp8C <= (void*)3?1:*((
int*)_tmp8C)!= 14)goto _LL50;_LL4F: return Cyc_Absyn_uint_t;_LL50: if(_tmp8C <= (void*)
3?1:*((int*)_tmp8C)!= 15)goto _LL52;_tmpB1=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp8C)->f1;_LL51: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_LL52: if((int)_tmp8C != 2)goto _LL54;_LL53:({void*_tmpCC[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof(char),55),_tag_arr(_tmpCC,sizeof(void*),0));});_LL54: if(_tmp8C <= (void*)3?
1:*((int*)_tmp8C)!= 17)goto _LL56;_LL55:({void*_tmpCD[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof(char),54),_tag_arr(_tmpCD,sizeof(void*),0));});_LL56: if(_tmp8C <= (void*)3?
1:*((int*)_tmp8C)!= 18)goto _LL58;_LL57:({void*_tmpCE[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof(char),52),_tag_arr(_tmpCE,sizeof(void*),0));});_LL58: if(_tmp8C <= (void*)3?
1:*((int*)_tmp8C)!= 19)goto _LL25;_LL59:({void*_tmpCF[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof(char),55),_tag_arr(_tmpCF,sizeof(void*),0));});_LL25:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l){void*_tmpD0=t;void*_tmpD1;struct Cyc_Absyn_Tqual _tmpD2;_LL69: if(_tmpD0 <= (void*)
3?1:*((int*)_tmpD0)!= 7)goto _LL6B;_tmpD1=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmpD0)->f1;_tmpD2=((struct Cyc_Absyn_ArrayType_struct*)_tmpD0)->f2;_LL6A: return
Cyc_Absyn_cast_exp(Cyc_Absyn_star_typ(_tmpD1,(void*)2,_tmpD2),e,l);_LL6B:;_LL6C:
return Cyc_Absyn_cast_exp(t,e,l);_LL68:;}static int Cyc_Toc_atomic_typ(void*t){void*
_tmpD3=Cyc_Tcutil_compress(t);void*_tmpD4;struct Cyc_Absyn_AggrInfo _tmpD5;void*
_tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_Absyn_TunionFieldInfo _tmpD8;void*
_tmpD9;struct Cyc_Absyn_Tuniondecl*_tmpDA;struct Cyc_Absyn_Tunionfield*_tmpDB;
struct Cyc_List_List*_tmpDC;_LL6E: if((int)_tmpD3 != 0)goto _LL70;_LL6F: return 1;
_LL70: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 1)goto _LL72;_LL71: return 0;_LL72:
if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 5)goto _LL74;_LL73: goto _LL75;_LL74: if(
_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 12)goto _LL76;_LL75: goto _LL77;_LL76: if(
_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 13)goto _LL78;_LL77: goto _LL79;_LL78: if((int)
_tmpD3 != 1)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)
!= 6)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 8)
goto _LL7E;_LL7D: goto _LL7F;_LL7E: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 14)goto
_LL80;_LL7F: return 1;_LL80: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 7)goto _LL82;
_tmpD4=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpD3)->f1;_LL81: return Cyc_Toc_atomic_typ(
_tmpD4);_LL82: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 10)goto _LL84;_tmpD5=((
struct Cyc_Absyn_AggrType_struct*)_tmpD3)->f1;_tmpD6=(void*)_tmpD5.aggr_info;
_LL83:{void*_tmpDD=_tmpD6;_LL93: if(*((int*)_tmpDD)!= 0)goto _LL95;_LL94: return 0;
_LL95:;_LL96: goto _LL92;_LL92:;}{struct Cyc_Absyn_Aggrdecl*_tmpDE=Cyc_Absyn_get_known_aggrdecl(
_tmpD6);if(_tmpDE->fields == 0)return 0;{struct Cyc_List_List*_tmpDF=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpDE->fields))->v;for(0;_tmpDF != 0;_tmpDF=
_tmpDF->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmpDF->hd)->type))
return 0;}}return 1;}_LL84: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)!= 11)goto _LL86;
_tmpD7=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpD3)->f2;_LL85: for(0;_tmpD7 != 0;
_tmpD7=_tmpD7->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmpD7->hd)->type))return 0;}return 1;_LL86: if(_tmpD3 <= (void*)3?1:*((int*)_tmpD3)
!= 3)goto _LL88;_tmpD8=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpD3)->f1;
_tmpD9=(void*)_tmpD8.field_info;if(*((int*)_tmpD9)!= 1)goto _LL88;_tmpDA=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmpD9)->f1;_tmpDB=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpD9)->f2;_LL87: _tmpDC=_tmpDB->typs;goto _LL89;_LL88: if(_tmpD3 <= (void*)3?1:*((
int*)_tmpD3)!= 9)goto _LL8A;_tmpDC=((struct Cyc_Absyn_TupleType_struct*)_tmpD3)->f1;
_LL89: for(0;_tmpDC != 0;_tmpDC=_tmpDC->tl){if(!Cyc_Toc_atomic_typ((*((struct
_tuple4*)_tmpDC->hd)).f2))return 0;}return 1;_LL8A: if(_tmpD3 <= (void*)3?1:*((int*)
_tmpD3)!= 2)goto _LL8C;_LL8B: goto _LL8D;_LL8C: if(_tmpD3 <= (void*)3?1:*((int*)
_tmpD3)!= 4)goto _LL8E;_LL8D: goto _LL8F;_LL8E: if(_tmpD3 <= (void*)3?1:*((int*)
_tmpD3)!= 15)goto _LL90;_LL8F: return 0;_LL90:;_LL91:({struct Cyc_Std_String_pa_struct
_tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpE0[1]={& _tmpE1};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("atomic_typ:  bad type %s",sizeof(char),25),_tag_arr(_tmpE0,sizeof(void*),
1));}});_LL6D:;}static int Cyc_Toc_is_void_star(void*t){void*_tmpE2=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE3;void*_tmpE4;_LL98: if(_tmpE2 <= (void*)3?1:*((int*)
_tmpE2)!= 4)goto _LL9A;_tmpE3=((struct Cyc_Absyn_PointerType_struct*)_tmpE2)->f1;
_tmpE4=(void*)_tmpE3.elt_typ;_LL99: {void*_tmpE5=Cyc_Tcutil_compress(_tmpE4);
_LL9D: if((int)_tmpE5 != 0)goto _LL9F;_LL9E: return 1;_LL9F:;_LLA0: return 0;_LL9C:;}
_LL9A:;_LL9B: return 0;_LL97:;}static int Cyc_Toc_is_poly_field(void*t,struct
_tagged_arr*f){void*_tmpE6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmpE7;
void*_tmpE8;struct Cyc_List_List*_tmpE9;_LLA2: if(_tmpE6 <= (void*)3?1:*((int*)
_tmpE6)!= 10)goto _LLA4;_tmpE7=((struct Cyc_Absyn_AggrType_struct*)_tmpE6)->f1;
_tmpE8=(void*)_tmpE7.aggr_info;_LLA3: {struct Cyc_Absyn_Aggrdecl*_tmpEA=Cyc_Absyn_get_known_aggrdecl(
_tmpE8);if(_tmpEA->fields == 0)({void*_tmpEB[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: type missing fields",
sizeof(char),35),_tag_arr(_tmpEB,sizeof(void*),0));});_tmpE9=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpEA->fields))->v;goto _LLA5;}_LLA4: if(_tmpE6 <= (
void*)3?1:*((int*)_tmpE6)!= 11)goto _LLA6;_tmpE9=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE6)->f2;_LLA5: {struct Cyc_Absyn_Aggrfield*_tmpEC=Cyc_Absyn_lookup_field(
_tmpE9,f);if(_tmpEC == 0)({struct Cyc_Std_String_pa_struct _tmpEE;_tmpEE.tag=0;
_tmpEE.f1=(struct _tagged_arr)*f;{void*_tmpED[1]={& _tmpEE};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_field: bad field %s",
sizeof(char),28),_tag_arr(_tmpED,sizeof(void*),1));}});return Cyc_Toc_is_void_star((
void*)_tmpEC->type);}_LLA6:;_LLA7:({struct Cyc_Std_String_pa_struct _tmpF0;_tmpF0.tag=
0;_tmpF0.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmpEF[1]={&
_tmpF0};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: bad type %s",sizeof(char),27),_tag_arr(_tmpEF,sizeof(
void*),1));}});_LLA1:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmpF1=(void*)e->r;struct Cyc_Absyn_Exp*_tmpF2;struct _tagged_arr*_tmpF3;
struct Cyc_Absyn_Exp*_tmpF4;struct _tagged_arr*_tmpF5;_LLA9: if(*((int*)_tmpF1)!= 
21)goto _LLAB;_tmpF2=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpF1)->f1;_tmpF3=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpF1)->f2;_LLAA: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF2->topt))->v,_tmpF3);_LLAB: if(*((
int*)_tmpF1)!= 22)goto _LLAD;_tmpF4=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpF1)->f1;
_tmpF5=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpF1)->f2;_LLAC: {void*_tmpF6=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF4->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF7;void*_tmpF8;_LLB0: if(_tmpF6 <= (void*)3?1:*((int*)_tmpF6)!= 4)goto _LLB2;
_tmpF7=((struct Cyc_Absyn_PointerType_struct*)_tmpF6)->f1;_tmpF8=(void*)_tmpF7.elt_typ;
_LLB1: return Cyc_Toc_is_poly_field(_tmpF8,_tmpF5);_LLB2:;_LLB3:({struct Cyc_Std_String_pa_struct
_tmpFA;_tmpFA.tag=0;_tmpFA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF4->topt))->v);{void*_tmpF9[1]={& _tmpFA};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_project:  bad type %s",
sizeof(char),30),_tag_arr(_tmpF9,sizeof(void*),1));}});_LLAF:;}_LLAD:;_LLAE:
return 0;_LLA8:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*
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
_LLB5: if(*((int*)_tmp102)!= 0)goto _LLB7;_tmp103=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp102)->f1;if(_tmp103 <= (void*)1?1:*((int*)_tmp103)!= 5)goto _LLB7;_LLB6:
is_string=1;goto _LLB4;_LLB7:;_LLB8: goto _LLB4;_LLB4:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp107=_cycalloc(sizeof(*
_tmp107));_tmp107[0]=({struct Cyc_Absyn_ArrayType_struct _tmp108;_tmp108.tag=7;
_tmp108.f1=(void*)Cyc_Absyn_uchar_t;_tmp108.f2=Cyc_Toc_mt_tq;_tmp108.f3=(struct
Cyc_Absyn_Exp*)sz;_tmp108;});_tmp107;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[0]=({
struct Cyc_Absyn_Var_d_struct _tmp106;_tmp106.tag=0;_tmp106.f1=vd;_tmp106;});
_tmp105;}),0);_tmp104->tl=Cyc_Toc_result_decls;_tmp104;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
e,0);xplussz=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,
0),0);}{struct Cyc_Absyn_Exp*urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple5*_tmp109[3];_tmp109[2]=({struct _tuple5*_tmp10C=_cycalloc(sizeof(*_tmp10C));
_tmp10C->f1=0;_tmp10C->f2=xplussz;_tmp10C;});_tmp109[1]=({struct _tuple5*_tmp10B=
_cycalloc(sizeof(*_tmp10B));_tmp10B->f1=0;_tmp10B->f2=xexp;_tmp10B;});_tmp109[0]=({
struct _tuple5*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->f1=0;_tmp10A->f2=xexp;
_tmp10A;});((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp109,sizeof(struct _tuple5*),3));}),0);return urm_exp;}}}struct Cyc_Toc_Env{
struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*continue_lab;struct Cyc_Core_Opt*
fallthru_info;struct Cyc_Dict_Dict*varmap;int toplevel;};static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*_tmp10D=_cycalloc(sizeof(*_tmp10D));
_tmp10D->break_lab=0;_tmp10D->continue_lab=0;_tmp10D->fallthru_info=0;_tmp10D->varmap=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp10D->toplevel=1;_tmp10D;});}static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp10E=_cycalloc(
sizeof(*_tmp10E));_tmp10E[0]=*e;_tmp10E;});ans->toplevel=0;return ans;}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct Cyc_Toc_Env*e,struct _tuple1*x,struct
Cyc_Absyn_Exp*y){{void*_tmp10F=(*x).f1;_LLBA: if(_tmp10F <= (void*)1?1:*((int*)
_tmp10F)!= 0)goto _LLBC;_LLBB:({struct Cyc_Std_String_pa_struct _tmp111;_tmp111.tag=
0;_tmp111.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(x);{void*_tmp110[1]={&
_tmp111};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Toc::add_varmap on Rel_n: %s\n",sizeof(char),30),_tag_arr(_tmp110,
sizeof(void*),1));}});_LLBC:;_LLBD: goto _LLB9;_LLB9:;}{struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=*e;_tmp112;});
ans->varmap=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,
struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(ans->varmap,x,y);return ans;}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=({
struct Cyc_Toc_Env*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=*e;_tmp113;});
ans->break_lab=0;ans->continue_lab=0;return ans;}struct _tuple7{struct _tagged_arr*
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
_tuple7*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->f1=fallthru_l;_tmp118->f2=
_tmp114;_tmp118->f3=next_case_env->varmap;_tmp118;});_tmp117;});return ans;}}
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct Cyc_Toc_Env*e,struct
_tagged_arr*break_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp11B=_cycalloc(
sizeof(*_tmp11B));_tmp11B[0]=*e;_tmp11B;});ans->break_lab=({struct Cyc_Core_Opt*
_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->v=break_l;_tmp11A;});ans->fallthru_info=
0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct Cyc_Toc_Env*
e,struct _tagged_arr*next_l){struct Cyc_Toc_Env*ans=({struct Cyc_Toc_Env*_tmp11E=
_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=*e;_tmp11E;});ans->break_lab=0;ans->fallthru_info=({
struct Cyc_Core_Opt*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->v=({struct
_tuple7*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->f1=next_l;_tmp11D->f2=0;
_tmp11D->f3=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple1*,struct _tuple1*)))
Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp11D;});_tmp11C;});return ans;}static void
Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp11F=(void*)e->annot;_LLBF: if(*((void**)_tmp11F)
!= Cyc_CfFlowInfo_UnknownZ)goto _LLC1;_LLC0: return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLC1: if(*((void**)_tmp11F)!= Cyc_CfFlowInfo_NotZero)
goto _LLC3;_LLC2: return 0;_LLC3: if(*((void**)_tmp11F)!= Cyc_CfFlowInfo_IsZero)goto
_LLC5;_LLC4:({void*_tmp120[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(char),28),_tag_arr(_tmp120,sizeof(void*),0));});return 0;_LLC5: if(_tmp11F
!= Cyc_Absyn_EmptyAnnot)goto _LLC7;_LLC6:({void*_tmp121[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("!",sizeof(char),2),_tag_arr(_tmp121,sizeof(void*),0));});({void*
_tmp122[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("unreachable dereference",sizeof(
char),24),_tag_arr(_tmp122,sizeof(void*),0));});return 0;_LLC7:;_LLC8:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp123=_cycalloc(sizeof(*_tmp123));
_tmp123[0]=({struct Cyc_Core_Impossible_struct _tmp124;_tmp124.tag=Cyc_Core_Impossible;
_tmp124.f1=_tag_arr("need_null_check",sizeof(char),16);_tmp124;});_tmp123;}));
_LLBE:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp125=(void*)e->annot;struct Cyc_List_List*_tmp126;struct Cyc_List_List*_tmp127;
_LLCA: if(*((void**)_tmp125)!= Cyc_CfFlowInfo_UnknownZ)goto _LLCC;_tmp126=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp125)->f1;_LLCB: return _tmp126;_LLCC: if(*((
void**)_tmp125)!= Cyc_CfFlowInfo_NotZero)goto _LLCE;_tmp127=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp125)->f1;_LLCD: return _tmp127;_LLCE: if(*((void**)_tmp125)!= Cyc_CfFlowInfo_IsZero)
goto _LLD0;_LLCF:({void*_tmp128[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(char),28),_tag_arr(_tmp128,sizeof(void*),0));});return 0;_LLD0: if(_tmp125
!= Cyc_Absyn_EmptyAnnot)goto _LLD2;_LLD1: return 0;_LLD2:;_LLD3:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129[0]=({
struct Cyc_Core_Impossible_struct _tmp12A;_tmp12A.tag=Cyc_Core_Impossible;_tmp12A.f1=
_tag_arr("get_relns",sizeof(char),10);_tmp12A;});_tmp129;}));_LLC9:;}static int
Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp12B=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp12C;struct Cyc_Absyn_Conref*_tmp12D;struct Cyc_Absyn_Exp*
_tmp12E;_LLD5: if(_tmp12B <= (void*)3?1:*((int*)_tmp12B)!= 4)goto _LLD7;_tmp12C=((
struct Cyc_Absyn_PointerType_struct*)_tmp12B)->f1;_tmp12D=_tmp12C.bounds;_LLD6: {
void*_tmp12F=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp12D);struct Cyc_Absyn_Exp*
_tmp130;_LLDC: if((int)_tmp12F != 0)goto _LLDE;_LLDD: return 0;_LLDE: if(_tmp12F <= (
void*)1?1:*((int*)_tmp12F)!= 0)goto _LLDB;_tmp130=((struct Cyc_Absyn_Upper_b_struct*)
_tmp12F)->f1;_LLDF: {unsigned int j=Cyc_Evexp_eval_const_uint_exp(_tmp130);return
i <= j;}_LLDB:;}_LLD7: if(_tmp12B <= (void*)3?1:*((int*)_tmp12B)!= 7)goto _LLD9;
_tmp12E=((struct Cyc_Absyn_ArrayType_struct*)_tmp12B)->f3;_LLD8: if(_tmp12E == 0)
return 0;{unsigned int j=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp12E));return i <= j;}_LLD9:;_LLDA: return 0;_LLD4:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp131=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp131->vd != y)continue;{void*_tmp132=(void*)_tmp131->rop;struct
Cyc_Absyn_Vardecl*_tmp133;struct Cyc_Absyn_Vardecl*_tmp134;_LLE1: if(*((int*)
_tmp132)!= 2)goto _LLE3;_tmp133=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp132)->f1;
_LLE2: _tmp134=_tmp133;goto _LLE4;_LLE3: if(*((int*)_tmp132)!= 4)goto _LLE5;_tmp134=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp132)->f1;_LLE4: if(_tmp134 == v)return
1;else{goto _LLE0;}_LLE5:;_LLE6: continue;_LLE0:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp135=(void*)e->r;void*_tmp136;struct Cyc_Absyn_Vardecl*_tmp137;void*_tmp138;
struct Cyc_Absyn_Vardecl*_tmp139;void*_tmp13A;struct Cyc_Absyn_Vardecl*_tmp13B;
void*_tmp13C;struct Cyc_Absyn_Vardecl*_tmp13D;void*_tmp13E;struct Cyc_List_List*
_tmp13F;struct Cyc_List_List _tmp140;struct Cyc_Absyn_Exp*_tmp141;_LLE8: if(*((int*)
_tmp135)!= 1)goto _LLEA;_tmp136=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp135)->f2;
if(_tmp136 <= (void*)1?1:*((int*)_tmp136)!= 4)goto _LLEA;_tmp137=((struct Cyc_Absyn_Pat_b_struct*)
_tmp136)->f1;_LLE9: _tmp139=_tmp137;goto _LLEB;_LLEA: if(*((int*)_tmp135)!= 1)goto
_LLEC;_tmp138=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp135)->f2;if(_tmp138 <= (
void*)1?1:*((int*)_tmp138)!= 3)goto _LLEC;_tmp139=((struct Cyc_Absyn_Local_b_struct*)
_tmp138)->f1;_LLEB: _tmp13B=_tmp139;goto _LLED;_LLEC: if(*((int*)_tmp135)!= 1)goto
_LLEE;_tmp13A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp135)->f2;if(_tmp13A <= (
void*)1?1:*((int*)_tmp13A)!= 0)goto _LLEE;_tmp13B=((struct Cyc_Absyn_Global_b_struct*)
_tmp13A)->f1;_LLED: _tmp13D=_tmp13B;goto _LLEF;_LLEE: if(*((int*)_tmp135)!= 1)goto
_LLF0;_tmp13C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp135)->f2;if(_tmp13C <= (
void*)1?1:*((int*)_tmp13C)!= 2)goto _LLF0;_tmp13D=((struct Cyc_Absyn_Param_b_struct*)
_tmp13C)->f1;_LLEF: if(_tmp13D->escapes)return 0;if(Cyc_Toc_check_leq_size_var(
relns,v,_tmp13D))return 1;goto _LLE7;_LLF0: if(*((int*)_tmp135)!= 3)goto _LLF2;
_tmp13E=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp135)->f1;if((int)_tmp13E != 
19)goto _LLF2;_tmp13F=((struct Cyc_Absyn_Primop_e_struct*)_tmp135)->f2;if(_tmp13F
== 0)goto _LLF2;_tmp140=*_tmp13F;_tmp141=(struct Cyc_Absyn_Exp*)_tmp140.hd;_LLF1:{
void*_tmp142=(void*)_tmp141->r;void*_tmp143;struct Cyc_Absyn_Vardecl*_tmp144;void*
_tmp145;struct Cyc_Absyn_Vardecl*_tmp146;void*_tmp147;struct Cyc_Absyn_Vardecl*
_tmp148;void*_tmp149;struct Cyc_Absyn_Vardecl*_tmp14A;_LLF5: if(*((int*)_tmp142)!= 
1)goto _LLF7;_tmp143=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp142)->f2;if(
_tmp143 <= (void*)1?1:*((int*)_tmp143)!= 4)goto _LLF7;_tmp144=((struct Cyc_Absyn_Pat_b_struct*)
_tmp143)->f1;_LLF6: _tmp146=_tmp144;goto _LLF8;_LLF7: if(*((int*)_tmp142)!= 1)goto
_LLF9;_tmp145=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp142)->f2;if(_tmp145 <= (
void*)1?1:*((int*)_tmp145)!= 3)goto _LLF9;_tmp146=((struct Cyc_Absyn_Local_b_struct*)
_tmp145)->f1;_LLF8: _tmp148=_tmp146;goto _LLFA;_LLF9: if(*((int*)_tmp142)!= 1)goto
_LLFB;_tmp147=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp142)->f2;if(_tmp147 <= (
void*)1?1:*((int*)_tmp147)!= 0)goto _LLFB;_tmp148=((struct Cyc_Absyn_Global_b_struct*)
_tmp147)->f1;_LLFA: _tmp14A=_tmp148;goto _LLFC;_LLFB: if(*((int*)_tmp142)!= 1)goto
_LLFD;_tmp149=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp142)->f2;if(_tmp149 <= (
void*)1?1:*((int*)_tmp149)!= 2)goto _LLFD;_tmp14A=((struct Cyc_Absyn_Param_b_struct*)
_tmp149)->f1;_LLFC: return _tmp14A == v;_LLFD:;_LLFE: goto _LLF4;_LLF4:;}goto _LLE7;
_LLF2:;_LLF3: goto _LLE7;_LLE7:;}return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp14B=(void*)a->r;
void*_tmp14C;struct Cyc_Absyn_Vardecl*_tmp14D;void*_tmp14E;struct Cyc_Absyn_Vardecl*
_tmp14F;void*_tmp150;struct Cyc_Absyn_Vardecl*_tmp151;void*_tmp152;struct Cyc_Absyn_Vardecl*
_tmp153;_LL100: if(*((int*)_tmp14B)!= 1)goto _LL102;_tmp14C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14B)->f2;if(_tmp14C <= (void*)1?1:*((int*)_tmp14C)!= 4)goto _LL102;_tmp14D=((
struct Cyc_Absyn_Pat_b_struct*)_tmp14C)->f1;_LL101: _tmp14F=_tmp14D;goto _LL103;
_LL102: if(*((int*)_tmp14B)!= 1)goto _LL104;_tmp14E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14B)->f2;if(_tmp14E <= (void*)1?1:*((int*)_tmp14E)!= 3)goto _LL104;_tmp14F=((
struct Cyc_Absyn_Local_b_struct*)_tmp14E)->f1;_LL103: _tmp151=_tmp14F;goto _LL105;
_LL104: if(*((int*)_tmp14B)!= 1)goto _LL106;_tmp150=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14B)->f2;if(_tmp150 <= (void*)1?1:*((int*)_tmp150)!= 0)goto _LL106;_tmp151=((
struct Cyc_Absyn_Global_b_struct*)_tmp150)->f1;_LL105: _tmp153=_tmp151;goto _LL107;
_LL106: if(*((int*)_tmp14B)!= 1)goto _LL108;_tmp152=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp14B)->f2;if(_tmp152 <= (void*)1?1:*((int*)_tmp152)!= 2)goto _LL108;_tmp153=((
struct Cyc_Absyn_Param_b_struct*)_tmp152)->f1;_LL107: if(_tmp153->escapes)return 0;
inner_loop: {void*_tmp154=(void*)i->r;void*_tmp155;struct Cyc_Absyn_Exp*_tmp156;
void*_tmp157;void*_tmp158;int _tmp159;void*_tmp15A;void*_tmp15B;int _tmp15C;void*
_tmp15D;struct Cyc_List_List*_tmp15E;struct Cyc_List_List _tmp15F;struct Cyc_Absyn_Exp*
_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_List_List _tmp162;struct Cyc_Absyn_Exp*
_tmp163;void*_tmp164;struct Cyc_Absyn_Vardecl*_tmp165;void*_tmp166;struct Cyc_Absyn_Vardecl*
_tmp167;void*_tmp168;struct Cyc_Absyn_Vardecl*_tmp169;void*_tmp16A;struct Cyc_Absyn_Vardecl*
_tmp16B;_LL10B: if(*((int*)_tmp154)!= 13)goto _LL10D;_tmp155=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp154)->f1;_tmp156=((struct Cyc_Absyn_Cast_e_struct*)_tmp154)->f2;_LL10C: i=
_tmp156;goto inner_loop;_LL10D: if(*((int*)_tmp154)!= 0)goto _LL10F;_tmp157=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp154)->f1;if(_tmp157 <= (void*)1?1:*((int*)
_tmp157)!= 2)goto _LL10F;_tmp158=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp157)->f1;
if((int)_tmp158 != 0)goto _LL10F;_tmp159=((struct Cyc_Absyn_Int_c_struct*)_tmp157)->f2;
_LL10E: return _tmp159 >= 0?Cyc_Toc_check_const_array((unsigned int)(_tmp159 + 1),(
void*)_tmp153->type): 0;_LL10F: if(*((int*)_tmp154)!= 0)goto _LL111;_tmp15A=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp154)->f1;if(_tmp15A <= (void*)1?1:*((int*)
_tmp15A)!= 2)goto _LL111;_tmp15B=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp15A)->f1;
if((int)_tmp15B != 1)goto _LL111;_tmp15C=((struct Cyc_Absyn_Int_c_struct*)_tmp15A)->f2;
_LL110: return Cyc_Toc_check_const_array((unsigned int)(_tmp15C + 1),(void*)_tmp153->type);
_LL111: if(*((int*)_tmp154)!= 3)goto _LL113;_tmp15D=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp154)->f1;if((int)_tmp15D != 4)goto _LL113;_tmp15E=((struct Cyc_Absyn_Primop_e_struct*)
_tmp154)->f2;if(_tmp15E == 0)goto _LL113;_tmp15F=*_tmp15E;_tmp160=(struct Cyc_Absyn_Exp*)
_tmp15F.hd;_tmp161=_tmp15F.tl;if(_tmp161 == 0)goto _LL113;_tmp162=*_tmp161;_tmp163=(
struct Cyc_Absyn_Exp*)_tmp162.hd;_LL112: return Cyc_Toc_check_leq_size(relns,
_tmp153,_tmp163);_LL113: if(*((int*)_tmp154)!= 1)goto _LL115;_tmp164=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp154)->f2;if(_tmp164 <= (void*)1?1:*((int*)
_tmp164)!= 4)goto _LL115;_tmp165=((struct Cyc_Absyn_Pat_b_struct*)_tmp164)->f1;
_LL114: _tmp167=_tmp165;goto _LL116;_LL115: if(*((int*)_tmp154)!= 1)goto _LL117;
_tmp166=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp154)->f2;if(_tmp166 <= (void*)
1?1:*((int*)_tmp166)!= 3)goto _LL117;_tmp167=((struct Cyc_Absyn_Local_b_struct*)
_tmp166)->f1;_LL116: _tmp169=_tmp167;goto _LL118;_LL117: if(*((int*)_tmp154)!= 1)
goto _LL119;_tmp168=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp154)->f2;if(
_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 0)goto _LL119;_tmp169=((struct Cyc_Absyn_Global_b_struct*)
_tmp168)->f1;_LL118: _tmp16B=_tmp169;goto _LL11A;_LL119: if(*((int*)_tmp154)!= 1)
goto _LL11B;_tmp16A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp154)->f2;if(
_tmp16A <= (void*)1?1:*((int*)_tmp16A)!= 2)goto _LL11B;_tmp16B=((struct Cyc_Absyn_Param_b_struct*)
_tmp16A)->f1;_LL11A: if(_tmp16B->escapes)return 0;{struct Cyc_List_List*_tmp16C=
relns;for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){struct Cyc_CfFlowInfo_Reln*_tmp16D=(
struct Cyc_CfFlowInfo_Reln*)_tmp16C->hd;if(_tmp16D->vd == _tmp16B){void*_tmp16E=(
void*)_tmp16D->rop;struct Cyc_Absyn_Vardecl*_tmp16F;struct Cyc_Absyn_Vardecl*
_tmp170;unsigned int _tmp171;_LL11E: if(*((int*)_tmp16E)!= 2)goto _LL120;_tmp16F=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp16E)->f1;_LL11F: if(_tmp153 == _tmp16F)
return 1;else{goto _LL11D;}_LL120: if(*((int*)_tmp16E)!= 1)goto _LL122;_tmp170=((
struct Cyc_CfFlowInfo_LessVar_struct*)_tmp16E)->f1;_LL121:{struct Cyc_List_List*
_tmp172=relns;for(0;_tmp172 != 0;_tmp172=_tmp172->tl){struct Cyc_CfFlowInfo_Reln*
_tmp173=(struct Cyc_CfFlowInfo_Reln*)_tmp172->hd;if(_tmp173->vd == _tmp170){void*
_tmp174=(void*)_tmp173->rop;struct Cyc_Absyn_Vardecl*_tmp175;struct Cyc_Absyn_Vardecl*
_tmp176;unsigned int _tmp177;struct Cyc_Absyn_Vardecl*_tmp178;_LL127: if(*((int*)
_tmp174)!= 4)goto _LL129;_tmp175=((struct Cyc_CfFlowInfo_LessEqSize_struct*)
_tmp174)->f1;_LL128: _tmp176=_tmp175;goto _LL12A;_LL129: if(*((int*)_tmp174)!= 2)
goto _LL12B;_tmp176=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp174)->f1;_LL12A:
if(_tmp153 == _tmp176)return 1;else{goto _LL126;}_LL12B: if(*((int*)_tmp174)!= 0)
goto _LL12D;_tmp177=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp174)->f1;_LL12C:
return Cyc_Toc_check_const_array(_tmp177,(void*)_tmp153->type);_LL12D: if(*((int*)
_tmp174)!= 1)goto _LL12F;_tmp178=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp174)->f1;
_LL12E: if(Cyc_Toc_check_leq_size_var(relns,_tmp153,_tmp178))return 1;else{goto
_LL126;}_LL12F:;_LL130: goto _LL126;_LL126:;}}}goto _LL11D;_LL122: if(*((int*)
_tmp16E)!= 3)goto _LL124;_tmp171=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp16E)->f1;
_LL123: return Cyc_Toc_check_const_array(_tmp171,(void*)_tmp153->type);_LL124:;
_LL125: goto _LL11D;_LL11D:;}}}goto _LL10A;_LL11B:;_LL11C: goto _LL10A;_LL10A:;}goto
_LLFF;_LL108:;_LL109: goto _LLFF;_LLFF:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp179[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",
sizeof(char),24),_tag_arr(_tmp179,sizeof(void*),0));});return Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp17A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",
sizeof(char),24),_tag_arr(_tmp17A,sizeof(void*),0));});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*
e){return({struct _tuple4*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->f1=Cyc_Toc_mt_tq;
_tmp17B->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp17B;});}static struct _tuple5*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);return({struct _tuple5*_tmp17C=
_cycalloc(sizeof(*_tmp17C));_tmp17C->f1=0;_tmp17C->f2=e;_tmp17C;});}static struct
Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple1*x,void*
struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int
is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp17D=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0)eo=(struct Cyc_Absyn_Exp*)(
is_atomic?Cyc_Toc_malloc_atomic(_tmp17D): Cyc_Toc_malloc_ptr(_tmp17D));else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(rgnopt);Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp17D);}}}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int e1_already_translated);static struct
Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp17E=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp17E != 0;_tmp17E=_tmp17E->tl){struct _tuple5 _tmp180;struct Cyc_List_List*
_tmp181;struct Cyc_Absyn_Exp*_tmp182;struct _tuple5*_tmp17F=(struct _tuple5*)
_tmp17E->hd;_tmp180=*_tmp17F;_tmp181=_tmp180.f1;_tmp182=_tmp180.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp181 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp181->tl != 0)({void*_tmp183[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("multiple designators in array",sizeof(
char),30),_tag_arr(_tmp183,sizeof(void*),0));});{void*_tmp184=(void*)_tmp181->hd;
void*_tmp185=_tmp184;struct Cyc_Absyn_Exp*_tmp186;_LL132: if(*((int*)_tmp185)!= 0)
goto _LL134;_tmp186=((struct Cyc_Absyn_ArrayElement_struct*)_tmp185)->f1;_LL133:
Cyc_Toc_exp_to_c(nv,_tmp186);e_index=_tmp186;goto _LL131;_LL134: if(*((int*)
_tmp185)!= 1)goto _LL131;_LL135:({void*_tmp187[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("field name designators in array",
sizeof(char),32),_tag_arr(_tmp187,sizeof(void*),0));});_LL131:;}}{struct Cyc_Absyn_Exp*
lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp188=(void*)_tmp182->r;
struct Cyc_List_List*_tmp189;struct Cyc_Absyn_Vardecl*_tmp18A;struct Cyc_Absyn_Exp*
_tmp18B;struct Cyc_Absyn_Exp*_tmp18C;void*_tmp18D;struct Cyc_List_List*_tmp18E;
_LL137: if(*((int*)_tmp188)!= 26)goto _LL139;_tmp189=((struct Cyc_Absyn_Array_e_struct*)
_tmp188)->f1;_LL138: s=Cyc_Toc_init_array(nv,lval,_tmp189,s);goto _LL136;_LL139:
if(*((int*)_tmp188)!= 27)goto _LL13B;_tmp18A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp188)->f1;_tmp18B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp188)->f2;
_tmp18C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp188)->f3;_LL13A: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp18A,_tmp18B,_tmp18C,s,0);goto _LL136;_LL13B: if(*((int*)_tmp188)!= 29)
goto _LL13D;_tmp18D=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp188)->f1;
_tmp18E=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp188)->f2;_LL13C: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp18D,_tmp18E,s);goto _LL136;_LL13D:;_LL13E: Cyc_Toc_exp_to_c(nv,_tmp182);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp182,0),s,0);goto _LL136;_LL136:;}}}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple1*_tmp18F=vd->name;if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(nv,_tmp18F,Cyc_Absyn_varb_exp(
_tmp18F,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp197=_cycalloc(sizeof(*
_tmp197));_tmp197[0]=({struct Cyc_Absyn_Local_b_struct _tmp198;_tmp198.tag=3;
_tmp198.f1=vd;_tmp198;});_tmp197;}),0));struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp18F,0),Cyc_Absyn_signed_int_exp(
0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp18F,0),Cyc_Absyn_var_exp(
max,0),0);struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(
_tmp18F,0),0);struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(
_tmp18F,0),0);struct Cyc_Absyn_Stmt*body;{void*_tmp190=(void*)e2->r;struct Cyc_List_List*
_tmp191;struct Cyc_Absyn_Vardecl*_tmp192;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*
_tmp194;void*_tmp195;struct Cyc_List_List*_tmp196;_LL140: if(*((int*)_tmp190)!= 26)
goto _LL142;_tmp191=((struct Cyc_Absyn_Array_e_struct*)_tmp190)->f1;_LL141: body=
Cyc_Toc_init_array(nv2,lval,_tmp191,Cyc_Toc_skip_stmt_dl());goto _LL13F;_LL142:
if(*((int*)_tmp190)!= 27)goto _LL144;_tmp192=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp190)->f1;_tmp193=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp190)->f2;
_tmp194=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp190)->f3;_LL143: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp192,_tmp193,_tmp194,Cyc_Toc_skip_stmt_dl(),0);goto _LL13F;_LL144: if(*((
int*)_tmp190)!= 29)goto _LL146;_tmp195=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp190)->f1;_tmp196=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp190)->f2;_LL145:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp195,_tmp196,Cyc_Toc_skip_stmt_dl());
goto _LL13F;_LL146:;_LL147: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL13F;_LL13F:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp18F,Cyc_Absyn_uint_t,0,s2,0),
0),s,0);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp199=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp199 != 0;_tmp199=_tmp199->tl){struct _tuple5 _tmp19B;
struct Cyc_List_List*_tmp19C;struct Cyc_Absyn_Exp*_tmp19D;struct _tuple5*_tmp19A=(
struct _tuple5*)_tmp199->hd;_tmp19B=*_tmp19A;_tmp19C=_tmp19B.f1;_tmp19D=_tmp19B.f2;
if(_tmp19C == 0)({void*_tmp19E[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",sizeof(char),
22),_tag_arr(_tmp19E,sizeof(void*),0));});if(_tmp19C->tl != 0)({void*_tmp19F[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("too many designators in anonymous struct",
sizeof(char),41),_tag_arr(_tmp19F,sizeof(void*),0));});{void*_tmp1A0=(void*)
_tmp19C->hd;struct _tagged_arr*_tmp1A1;_LL149: if(*((int*)_tmp1A0)!= 1)goto _LL14B;
_tmp1A1=((struct Cyc_Absyn_FieldName_struct*)_tmp1A0)->f1;_LL14A: {struct Cyc_Absyn_Exp*
lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp1A1,0);{void*_tmp1A2=(void*)_tmp19D->r;
struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_Vardecl*_tmp1A4;struct Cyc_Absyn_Exp*
_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A6;void*_tmp1A7;struct Cyc_List_List*_tmp1A8;
_LL14E: if(*((int*)_tmp1A2)!= 26)goto _LL150;_tmp1A3=((struct Cyc_Absyn_Array_e_struct*)
_tmp1A2)->f1;_LL14F: s=Cyc_Toc_init_array(nv,lval,_tmp1A3,s);goto _LL14D;_LL150:
if(*((int*)_tmp1A2)!= 27)goto _LL152;_tmp1A4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1A2)->f1;_tmp1A5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A2)->f2;
_tmp1A6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A2)->f3;_LL151: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp1A4,_tmp1A5,_tmp1A6,s,0);goto _LL14D;_LL152: if(*((int*)_tmp1A2)!= 29)
goto _LL154;_tmp1A7=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1A2)->f1;
_tmp1A8=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1A2)->f2;_LL153: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp1A7,_tmp1A8,s);goto _LL14D;_LL154:;_LL155: Cyc_Toc_exp_to_c(nv,_tmp19D);
if(Cyc_Toc_is_poly_field(struct_type,_tmp1A1))_tmp19D=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp19D,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp19D,0),0),s,0);goto _LL14D;_LL14D:;}goto _LL148;}_LL14B:;_LL14C:({void*_tmp1A9[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct",sizeof(char),27),_tag_arr(_tmp1A9,sizeof(
void*),0));});_LL148:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(
struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
es){struct _RegionHandle _tmp1AA=_new_region("r");struct _RegionHandle*r=& _tmp1AA;
_push_region(r);{struct Cyc_List_List*_tmp1AB=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _tagged_arr*_tmp1AC=Cyc_Toc_add_tuple_type(
_tmp1AB);struct _tuple1*_tmp1AD=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1AE=
Cyc_Absyn_var_exp(_tmp1AD,0);struct Cyc_Absyn_Stmt*_tmp1AF=Cyc_Absyn_exp_stmt(
_tmp1AE,0);struct Cyc_Absyn_Exp*(*_tmp1B0)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,
struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp1B1=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp1B1);for(0;_tmp1B1 != 0;(_tmp1B1=_tmp1B1->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp1B1->hd;struct Cyc_Absyn_Exp*
lval=_tmp1B0(_tmp1AE,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic?Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v): 0;{void*_tmp1B2=(void*)e->r;
struct Cyc_List_List*_tmp1B3;struct Cyc_Absyn_Vardecl*_tmp1B4;struct Cyc_Absyn_Exp*
_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B6;_LL157: if(*((int*)_tmp1B2)!= 26)goto _LL159;
_tmp1B3=((struct Cyc_Absyn_Array_e_struct*)_tmp1B2)->f1;_LL158: _tmp1AF=Cyc_Toc_init_array(
nv,lval,_tmp1B3,_tmp1AF);goto _LL156;_LL159: if(*((int*)_tmp1B2)!= 27)goto _LL15B;
_tmp1B4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1B2)->f1;_tmp1B5=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1B2)->f2;_tmp1B6=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1B2)->f3;_LL15A: _tmp1AF=Cyc_Toc_init_comprehension(nv,lval,_tmp1B4,_tmp1B5,
_tmp1B6,_tmp1AF,0);goto _LL156;_LL15B:;_LL15C: Cyc_Toc_exp_to_c(nv,e);_tmp1AF=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1B0(_tmp1AE,Cyc_Absyn_fieldname(i),0),
e,0),0),_tmp1AF,0);goto _LL156;_LL156:;}}}{struct Cyc_Absyn_Exp*_tmp1B7=Cyc_Toc_make_struct(
nv,_tmp1AD,Cyc_Absyn_strct(_tmp1AC),_tmp1AF,pointer,rgnopt,is_atomic);
_npop_handler(0);return _tmp1B7;}};_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp1B8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp1B9=Cyc_Absyn_var_exp(_tmp1B8,0);struct Cyc_Absyn_Stmt*
_tmp1BA=Cyc_Absyn_exp_stmt(_tmp1B9,0);struct Cyc_Absyn_Exp*(*_tmp1BB)(struct Cyc_Absyn_Exp*,
struct _tagged_arr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*
_tmp1BC=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp1BD;void*
_tmp1BE;_LL15E: if(_tmp1BC <= (void*)3?1:*((int*)_tmp1BC)!= 10)goto _LL160;_tmp1BD=((
struct Cyc_Absyn_AggrType_struct*)_tmp1BC)->f1;_tmp1BE=(void*)_tmp1BD.aggr_info;
_LL15F: ad=Cyc_Absyn_get_known_aggrdecl(_tmp1BE);goto _LL15D;_LL160:;_LL161:({void*
_tmp1BF[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("init_struct: bad struct type",sizeof(char),29),_tag_arr(_tmp1BF,
sizeof(void*),0));});_LL15D:;}{struct _RegionHandle _tmp1C0=_new_region("r");
struct _RegionHandle*r=& _tmp1C0;_push_region(r);{struct Cyc_List_List*_tmp1C1=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
r,dles);for(0;_tmp1C1 != 0;_tmp1C1=_tmp1C1->tl){struct _tuple5 _tmp1C3;struct Cyc_List_List*
_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C5;struct _tuple5*_tmp1C2=(struct _tuple5*)
_tmp1C1->hd;_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;is_atomic=
is_atomic?Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1C5->topt))->v):
0;if(_tmp1C4 == 0)({void*_tmp1C6[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",sizeof(char),
22),_tag_arr(_tmp1C6,sizeof(void*),0));});if(_tmp1C4->tl != 0){struct _tuple1*
_tmp1C7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp1C8=Cyc_Absyn_var_exp(_tmp1C7,
0);for(0;_tmp1C4 != 0;_tmp1C4=_tmp1C4->tl){void*_tmp1C9=(void*)_tmp1C4->hd;struct
_tagged_arr*_tmp1CA;_LL163: if(*((int*)_tmp1C9)!= 1)goto _LL165;_tmp1CA=((struct
Cyc_Absyn_FieldName_struct*)_tmp1C9)->f1;_LL164: if(Cyc_Toc_is_poly_field(
struct_type,_tmp1CA))_tmp1C8=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp1C8,0);_tmp1BA=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp1BB(_tmp1B9,_tmp1CA,0),_tmp1C8,0),0),_tmp1BA,0);goto _LL162;_LL165:;_LL166:({
void*_tmp1CB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("array designator in struct",sizeof(char),27),_tag_arr(_tmp1CB,sizeof(
void*),0));});_LL162:;}Cyc_Toc_exp_to_c(nv,_tmp1C5);_tmp1BA=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp1C8,_tmp1C5,0),0),_tmp1BA,0);}else{
void*_tmp1CC=(void*)_tmp1C4->hd;struct _tagged_arr*_tmp1CD;_LL168: if(*((int*)
_tmp1CC)!= 1)goto _LL16A;_tmp1CD=((struct Cyc_Absyn_FieldName_struct*)_tmp1CC)->f1;
_LL169: {struct Cyc_Absyn_Exp*lval=_tmp1BB(_tmp1B9,_tmp1CD,0);{void*_tmp1CE=(void*)
_tmp1C5->r;struct Cyc_List_List*_tmp1CF;struct Cyc_Absyn_Vardecl*_tmp1D0;struct Cyc_Absyn_Exp*
_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D2;void*_tmp1D3;struct Cyc_List_List*_tmp1D4;
_LL16D: if(*((int*)_tmp1CE)!= 26)goto _LL16F;_tmp1CF=((struct Cyc_Absyn_Array_e_struct*)
_tmp1CE)->f1;_LL16E: _tmp1BA=Cyc_Toc_init_array(nv,lval,_tmp1CF,_tmp1BA);goto
_LL16C;_LL16F: if(*((int*)_tmp1CE)!= 27)goto _LL171;_tmp1D0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1CE)->f1;_tmp1D1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1CE)->f2;
_tmp1D2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1CE)->f3;_LL170: _tmp1BA=
Cyc_Toc_init_comprehension(nv,lval,_tmp1D0,_tmp1D1,_tmp1D2,_tmp1BA,0);goto _LL16C;
_LL171: if(*((int*)_tmp1CE)!= 29)goto _LL173;_tmp1D3=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1CE)->f1;_tmp1D4=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1CE)->f2;_LL172:
_tmp1BA=Cyc_Toc_init_anon_struct(nv,lval,_tmp1D3,_tmp1D4,_tmp1BA);goto _LL16C;
_LL173:;_LL174: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1C5->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp1C5);if(Cyc_Toc_is_poly_field(
struct_type,_tmp1CD)?!was_ptr_type: 0)_tmp1C5=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp1C5,0);if(has_exists)_tmp1C5=Cyc_Absyn_cast_exp((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(Cyc_Absyn_lookup_decl_field(ad,_tmp1CD)))->type,_tmp1C5,0);_tmp1BA=
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp1C5,0),0),
_tmp1BA,0);goto _LL16C;}_LL16C:;}goto _LL167;}_LL16A:;_LL16B:({void*_tmp1D5[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("array designator in struct",
sizeof(char),27),_tag_arr(_tmp1D5,sizeof(void*),0));});_LL167:;}}}{struct Cyc_Absyn_Exp*
_tmp1D6=Cyc_Toc_make_struct(nv,_tmp1B8,Cyc_Absyn_strctq(tdn),_tmp1BA,pointer,
rgnopt,is_atomic);_npop_handler(0);return _tmp1D6;};_pop_region(r);}}struct
_tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple8*pr){return Cyc_Absyn_assignop_exp(
el,(*pr).f1,(*pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct
Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));
_tmp1D7[0]=({struct Cyc_Absyn_Increment_e_struct _tmp1D8;_tmp1D8.tag=5;_tmp1D8.f1=
e1;_tmp1D8.f2=(void*)incr;_tmp1D8;});_tmp1D7;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp1D9=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp1DA;void*_tmp1DB;struct Cyc_Absyn_Exp*
_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DD;struct _tagged_arr*_tmp1DE;_LL176: if(*((int*)
_tmp1D9)!= 35)goto _LL178;_tmp1DA=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp1D9)->f1;
_LL177: Cyc_Toc_lvalue_assign_stmt(_tmp1DA,fs,f,f_env);goto _LL175;_LL178: if(*((
int*)_tmp1D9)!= 13)goto _LL17A;_tmp1DB=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp1D9)->f1;_tmp1DC=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D9)->f2;_LL179: Cyc_Toc_lvalue_assign(
_tmp1DC,fs,f,f_env);goto _LL175;_LL17A: if(*((int*)_tmp1D9)!= 21)goto _LL17C;
_tmp1DD=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D9)->f1;_tmp1DE=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1D9)->f2;_LL17B:(void*)(e1->r=(void*)((void*)_tmp1DD->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));
_tmp1DF->hd=_tmp1DE;_tmp1DF->tl=fs;_tmp1DF;}),f,f_env);goto _LL175;_LL17C:;_LL17D: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _tagged_arr*)fs->hd,e1_copy->loc);}(
void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL175;}_LL175:;}static
void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp1E0=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Decl*_tmp1E2;struct Cyc_Absyn_Stmt*
_tmp1E3;struct Cyc_Absyn_Stmt*_tmp1E4;_LL17F: if(_tmp1E0 <= (void*)1?1:*((int*)
_tmp1E0)!= 0)goto _LL181;_tmp1E1=((struct Cyc_Absyn_Exp_s_struct*)_tmp1E0)->f1;
_LL180: Cyc_Toc_lvalue_assign(_tmp1E1,fs,f,f_env);goto _LL17E;_LL181: if(_tmp1E0 <= (
void*)1?1:*((int*)_tmp1E0)!= 12)goto _LL183;_tmp1E2=((struct Cyc_Absyn_Decl_s_struct*)
_tmp1E0)->f1;_tmp1E3=((struct Cyc_Absyn_Decl_s_struct*)_tmp1E0)->f2;_LL182: Cyc_Toc_lvalue_assign_stmt(
_tmp1E3,fs,f,f_env);goto _LL17E;_LL183: if(_tmp1E0 <= (void*)1?1:*((int*)_tmp1E0)!= 
1)goto _LL185;_tmp1E4=((struct Cyc_Absyn_Seq_s_struct*)_tmp1E0)->f2;_LL184: Cyc_Toc_lvalue_assign_stmt(
_tmp1E4,fs,f,f_env);goto _LL17E;_LL185:;_LL186:({struct Cyc_Std_String_pa_struct
_tmp1E6;_tmp1E6.tag=0;_tmp1E6.f1=(struct _tagged_arr)Cyc_Absynpp_stmt2string(s);{
void*_tmp1E5[1]={& _tmp1E6};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("lvalue_assign_stmt: %s",sizeof(char),23),_tag_arr(
_tmp1E5,sizeof(void*),1));}});_LL17E:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp1E7=_region_malloc(r2,sizeof(*_tmp1E7));_tmp1E7->hd=(
void*)f((void*)x->hd,env);_tmp1E7->tl=0;_tmp1E7;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp1E8=_region_malloc(r2,sizeof(*_tmp1E8));_tmp1E8->hd=(void*)f((void*)x->hd,
env);_tmp1E8->tl=0;_tmp1E8;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple5*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple5*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->f1=0;_tmp1E9->f2=e;_tmp1E9;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp1EA=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp1EB;_LL188: if(_tmp1EA <= (void*)3?1:*((int*)_tmp1EA)
!= 4)goto _LL18A;_tmp1EB=((struct Cyc_Absyn_PointerType_struct*)_tmp1EA)->f1;
_LL189: return _tmp1EB;_LL18A:;_LL18B:({void*_tmp1EC[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("get_ptr_typ: not a pointer!",
sizeof(char),28),_tag_arr(_tmp1EC,sizeof(void*),0));});_LL187:;}struct _tuple9{
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple10{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){void*_tmp1ED=(void*)e->r;if(e->topt == 0)({struct Cyc_Std_String_pa_struct
_tmp1EF;_tmp1EF.tag=0;_tmp1EF.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e);{
void*_tmp1EE[1]={& _tmp1EF};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: no type for %s",sizeof(char),25),_tag_arr(
_tmp1EE,sizeof(void*),1));}});{void*old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;void*_tmp1F0=_tmp1ED;void*_tmp1F1;struct _tuple1*_tmp1F2;
void*_tmp1F3;struct _tuple1*_tmp1F4;void*_tmp1F5;struct Cyc_List_List*_tmp1F6;
struct Cyc_Absyn_Exp*_tmp1F7;void*_tmp1F8;struct Cyc_Absyn_Exp*_tmp1F9;struct Cyc_Core_Opt*
_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_Absyn_Exp*
_tmp1FD;struct Cyc_Absyn_Exp*_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_Absyn_Exp*
_tmp200;struct Cyc_Absyn_Exp*_tmp201;struct Cyc_List_List*_tmp202;struct Cyc_Absyn_Exp*
_tmp203;struct Cyc_List_List*_tmp204;struct Cyc_Absyn_VarargCallInfo*_tmp205;
struct Cyc_Absyn_Exp*_tmp206;struct Cyc_List_List*_tmp207;struct Cyc_Absyn_VarargCallInfo*
_tmp208;struct Cyc_Absyn_VarargCallInfo _tmp209;int _tmp20A;struct Cyc_List_List*
_tmp20B;struct Cyc_Absyn_VarargInfo*_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;struct Cyc_Absyn_Exp*
_tmp20E;struct Cyc_Absyn_Exp*_tmp20F;struct Cyc_List_List*_tmp210;void*_tmp211;
void**_tmp212;struct Cyc_Absyn_Exp*_tmp213;struct Cyc_Absyn_Exp*_tmp214;struct Cyc_Absyn_Exp*
_tmp215;struct Cyc_Absyn_Exp*_tmp216;struct Cyc_Absyn_Exp*_tmp217;void*_tmp218;
void*_tmp219;void*_tmp21A;struct _tagged_arr*_tmp21B;void*_tmp21C;void*_tmp21D;
unsigned int _tmp21E;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_Absyn_Exp*_tmp220;
struct _tagged_arr*_tmp221;struct Cyc_Absyn_Exp*_tmp222;struct _tagged_arr*_tmp223;
struct Cyc_Absyn_Exp*_tmp224;struct Cyc_Absyn_Exp*_tmp225;struct Cyc_List_List*
_tmp226;struct Cyc_List_List*_tmp227;struct Cyc_Absyn_Vardecl*_tmp228;struct Cyc_Absyn_Exp*
_tmp229;struct Cyc_Absyn_Exp*_tmp22A;struct _tuple1*_tmp22B;struct Cyc_List_List*
_tmp22C;struct Cyc_List_List*_tmp22D;struct Cyc_Absyn_Aggrdecl*_tmp22E;void*
_tmp22F;struct Cyc_List_List*_tmp230;struct Cyc_List_List*_tmp231;struct Cyc_Absyn_Tuniondecl*
_tmp232;struct Cyc_Absyn_Tunionfield*_tmp233;struct Cyc_List_List*_tmp234;struct
Cyc_Absyn_Tuniondecl*_tmp235;struct Cyc_Absyn_Tunionfield*_tmp236;struct Cyc_Absyn_MallocInfo
_tmp237;int _tmp238;struct Cyc_Absyn_Exp*_tmp239;void**_tmp23A;struct Cyc_Absyn_Exp*
_tmp23B;int _tmp23C;struct Cyc_Absyn_Stmt*_tmp23D;struct Cyc_Absyn_Fndecl*_tmp23E;
_LL18D: if(*((int*)_tmp1F0)!= 0)goto _LL18F;_tmp1F1=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1F0)->f1;if((int)_tmp1F1 != 0)goto _LL18F;_LL18E: {struct Cyc_Absyn_Exp*_tmp23F=
Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_typ,_tmp23F,
_tmp23F))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp240[3];_tmp240[2]=_tmp23F;_tmp240[1]=_tmp23F;_tmp240[0]=
_tmp23F;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp240,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(void*)((
void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL18C;}_LL18F: if(*((int*)
_tmp1F0)!= 0)goto _LL191;_LL190: goto _LL18C;_LL191: if(*((int*)_tmp1F0)!= 1)goto
_LL193;_tmp1F2=((struct Cyc_Absyn_Var_e_struct*)_tmp1F0)->f1;_tmp1F3=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp1F0)->f2;_LL192:{struct _handler_cons _tmp241;
_push_handler(& _tmp241);{int _tmp243=0;if(setjmp(_tmp241.handler))_tmp243=1;if(!
_tmp243){(void*)(e->r=(void*)((void*)(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k))Cyc_Dict_lookup)(nv->varmap,_tmp1F2))->r));;_pop_handler();}
else{void*_tmp242=(void*)_exn_thrown;void*_tmp245=_tmp242;_LL1E2: if(_tmp245 != 
Cyc_Dict_Absent)goto _LL1E4;_LL1E3:({struct Cyc_Std_String_pa_struct _tmp247;
_tmp247.tag=0;_tmp247.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1F2);{
void*_tmp246[1]={& _tmp247};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("Can't find %s in exp_to_c, Var\n",sizeof(char),32),
_tag_arr(_tmp246,sizeof(void*),1));}});_LL1E4:;_LL1E5:(void)_throw(_tmp245);
_LL1E1:;}}}goto _LL18C;_LL193: if(*((int*)_tmp1F0)!= 2)goto _LL195;_tmp1F4=((struct
Cyc_Absyn_UnknownId_e_struct*)_tmp1F0)->f1;_LL194:({void*_tmp248[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unknownid",
sizeof(char),10),_tag_arr(_tmp248,sizeof(void*),0));});_LL195: if(*((int*)_tmp1F0)
!= 3)goto _LL197;_tmp1F5=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp1F0)->f1;
_tmp1F6=((struct Cyc_Absyn_Primop_e_struct*)_tmp1F0)->f2;_LL196: {struct Cyc_List_List*
_tmp249=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp1F6);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp1F6);{void*_tmp24A=_tmp1F5;_LL1E7: if((int)_tmp24A != 19)
goto _LL1E9;_LL1E8: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1F6))->hd;{void*_tmp24B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_Exp*_tmp24C;struct Cyc_Absyn_PtrInfo
_tmp24D;void*_tmp24E;struct Cyc_Absyn_Conref*_tmp24F;struct Cyc_Absyn_Conref*
_tmp250;_LL1FC: if(_tmp24B <= (void*)3?1:*((int*)_tmp24B)!= 7)goto _LL1FE;_tmp24C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp24B)->f3;_LL1FD:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp24C))->r));goto _LL1FB;_LL1FE: if(_tmp24B <= (
void*)3?1:*((int*)_tmp24B)!= 4)goto _LL200;_tmp24D=((struct Cyc_Absyn_PointerType_struct*)
_tmp24B)->f1;_tmp24E=(void*)_tmp24D.elt_typ;_tmp24F=_tmp24D.nullable;_tmp250=
_tmp24D.bounds;_LL1FF:{void*_tmp251=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp250);struct Cyc_Absyn_Exp*_tmp252;_LL203: if((int)_tmp251 != 0)goto _LL205;
_LL204:(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp253[2];_tmp253[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp24E),0);_tmp253[0]=(struct Cyc_Absyn_Exp*)_tmp1F6->hd;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp253,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL202;_LL205: if(_tmp251 <= (void*)1?1:*((int*)_tmp251)!= 0)
goto _LL202;_tmp252=((struct Cyc_Absyn_Upper_b_struct*)_tmp251)->f1;_LL206: if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp24F))(void*)(e->r=(
void*)((void*)(Cyc_Absyn_conditional_exp(arg,_tmp252,Cyc_Absyn_uint_exp(0,0),0))->r));
else{(void*)(e->r=(void*)((void*)_tmp252->r));goto _LL202;}_LL202:;}goto _LL1FB;
_LL200:;_LL201:({struct Cyc_Std_String_pa_struct _tmp256;_tmp256.tag=0;_tmp256.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);{struct Cyc_Std_String_pa_struct _tmp255;_tmp255.tag=0;
_tmp255.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);{void*_tmp254[2]={& _tmp255,& _tmp256};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("size primop applied to non-array %s (%s)",
sizeof(char),41),_tag_arr(_tmp254,sizeof(void*),2));}}});_LL1FB:;}goto _LL1E6;}
_LL1E9: if((int)_tmp24A != 0)goto _LL1EB;_LL1EA: {void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp249))->hd,& elt_typ)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1F6))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1F6->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp257[3];_tmp257[2]=e2;_tmp257[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp257[0]=e1;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp257,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}goto _LL1E6;}_LL1EB: if((int)_tmp24A != 2)goto _LL1ED;_LL1EC: {void*
elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp249))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1F6))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1F6->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp249->tl))->hd)){(void*)(e1->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,
0))->r));(void*)(e2->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(
Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp258[3];_tmp258[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);
_tmp258[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp258[0]=e1;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp258,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}goto _LL1E6;}_LL1ED: if((int)
_tmp24A != 5)goto _LL1EF;_LL1EE: goto _LL1F0;_LL1EF: if((int)_tmp24A != 6)goto _LL1F1;
_LL1F0: goto _LL1F2;_LL1F1: if((int)_tmp24A != 7)goto _LL1F3;_LL1F2: goto _LL1F4;_LL1F3:
if((int)_tmp24A != 9)goto _LL1F5;_LL1F4: goto _LL1F6;_LL1F5: if((int)_tmp24A != 8)goto
_LL1F7;_LL1F6: goto _LL1F8;_LL1F7: if((int)_tmp24A != 10)goto _LL1F9;_LL1F8: {struct
Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp1F6))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1F6->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)_check_null(
_tmp249))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp249->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));if(Cyc_Tcutil_is_tagged_pointer_typ(
t2))(void*)(e2->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp,0))->r));goto _LL1E6;}_LL1F9:;_LL1FA: goto _LL1E6;
_LL1E6:;}goto _LL18C;}_LL197: if(*((int*)_tmp1F0)!= 5)goto _LL199;_tmp1F7=((struct
Cyc_Absyn_Increment_e_struct*)_tmp1F0)->f1;_tmp1F8=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp1F0)->f2;_LL198: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1F7->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp1F7);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*_tmp259=(_tmp1F8 == (void*)1?1: _tmp1F8 == (
void*)3)?Cyc_Toc__tagged_arr_inplace_plus_post_e: Cyc_Toc__tagged_arr_inplace_plus_e;
int _tmp25A=(_tmp1F8 == (void*)0?1: _tmp1F8 == (void*)1)?1: - 1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_fncall_exp(_tmp259,({struct Cyc_Absyn_Exp*_tmp25B[3];_tmp25B[2]=
Cyc_Absyn_signed_int_exp(_tmp25A,0);_tmp25B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0);_tmp25B[0]=Cyc_Absyn_address_exp(_tmp1F7,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp25B,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}if(elt_typ == (void*)0?!Cyc_Absyn_is_lvalue(_tmp1F7): 0){Cyc_Toc_lvalue_assign(
_tmp1F7,0,Cyc_Toc_incr_lvalue,_tmp1F8);(void*)(e->r=(void*)((void*)_tmp1F7->r));}
goto _LL18C;}}_LL199: if(*((int*)_tmp1F0)!= 4)goto _LL19B;_tmp1F9=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1F0)->f1;_tmp1FA=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1F0)->f2;_tmp1FB=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1F0)->f3;_LL19A: {int e1_poly=Cyc_Toc_is_poly_project(
_tmp1F9);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1F9->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FB->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp1F9);Cyc_Toc_exp_to_c(nv,_tmp1FB);{int done=0;if(_tmp1FA != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp25C=(void*)_tmp1FA->v;_LL208: if((int)_tmp25C != 0)goto _LL20A;
_LL209: change=_tmp1FB;goto _LL207;_LL20A: if((int)_tmp25C != 2)goto _LL20C;_LL20B:
change=Cyc_Absyn_prim1_exp((void*)2,_tmp1FB,0);goto _LL207;_LL20C:;_LL20D:({void*
_tmp25D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("bad t ? pointer arithmetic",sizeof(char),27),_tag_arr(_tmp25D,sizeof(
void*),0));});_LL207:;}done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_inplace_plus_e,({struct Cyc_Absyn_Exp*_tmp25E[3];_tmp25E[2]=
change;_tmp25E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp25E[0]=
Cyc_Absyn_address_exp(_tmp1F9,0);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp25E,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}
if(!done){if(e1_poly)(void*)(_tmp1FB->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp1FB->r,0),0))->r));if(!Cyc_Absyn_is_lvalue(_tmp1F9)){((
void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Absyn_Exp*,struct _tuple8*),struct _tuple8*f_env))Cyc_Toc_lvalue_assign)(
_tmp1F9,0,Cyc_Toc_assignop_lvalue,({struct _tuple8*_tmp25F=_cycalloc(sizeof(
struct _tuple8)* 1);_tmp25F[0]=({struct _tuple8 _tmp260;_tmp260.f1=_tmp1FA;_tmp260.f2=
_tmp1FB;_tmp260;});_tmp25F;}));(void*)(e->r=(void*)((void*)_tmp1F9->r));}}goto
_LL18C;}}_LL19B: if(*((int*)_tmp1F0)!= 6)goto _LL19D;_tmp1FC=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1F0)->f1;_tmp1FD=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1F0)->f2;
_tmp1FE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1F0)->f3;_LL19C: Cyc_Toc_exp_to_c(
nv,_tmp1FC);Cyc_Toc_exp_to_c(nv,_tmp1FD);Cyc_Toc_exp_to_c(nv,_tmp1FE);goto _LL18C;
_LL19D: if(*((int*)_tmp1F0)!= 7)goto _LL19F;_tmp1FF=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1F0)->f1;_tmp200=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1F0)->f2;_LL19E: Cyc_Toc_exp_to_c(
nv,_tmp1FF);Cyc_Toc_exp_to_c(nv,_tmp200);goto _LL18C;_LL19F: if(*((int*)_tmp1F0)!= 
8)goto _LL1A1;_tmp201=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1F0)->f1;
_tmp202=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1F0)->f2;_LL1A0: _tmp203=
_tmp201;_tmp204=_tmp202;goto _LL1A2;_LL1A1: if(*((int*)_tmp1F0)!= 9)goto _LL1A3;
_tmp203=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1F0)->f1;_tmp204=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1F0)->f2;_tmp205=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1F0)->f3;if(_tmp205
!= 0)goto _LL1A3;_LL1A2: Cyc_Toc_exp_to_c(nv,_tmp203);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp204);goto _LL18C;_LL1A3: if(*((int*)_tmp1F0)!= 9)goto
_LL1A5;_tmp206=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1F0)->f1;_tmp207=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp1F0)->f2;_tmp208=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1F0)->f3;if(_tmp208 == 0)goto _LL1A5;_tmp209=*_tmp208;_tmp20A=_tmp209.num_varargs;
_tmp20B=_tmp209.injectors;_tmp20C=_tmp209.vai;_LL1A4:{struct _RegionHandle _tmp261=
_new_region("r");struct _RegionHandle*r=& _tmp261;_push_region(r);{struct _tuple1*
argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp20A,0);
void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp20C->type);void*arr_type=(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp276;_tmp276.tag=7;_tmp276.f1=(void*)cva_type;
_tmp276.f2=Cyc_Toc_mt_tq;_tmp276.f3=(struct Cyc_Absyn_Exp*)num_varargs_exp;
_tmp276;});_tmp275;});int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp207);int num_normargs=num_args - _tmp20A;struct Cyc_List_List*new_args=0;{int i=
0;for(0;i < num_normargs;(++ i,_tmp207=_tmp207->tl)){Cyc_Toc_exp_to_c(nv,(struct
Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp207))->hd);new_args=({
struct Cyc_List_List*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262->hd=(struct Cyc_Absyn_Exp*)
_tmp207->hd;_tmp262->tl=new_args;_tmp262;});}}new_args=({struct Cyc_List_List*
_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp264[3];_tmp264[2]=num_varargs_exp;_tmp264[1]=Cyc_Absyn_sizeoftyp_exp(
cva_type,0);_tmp264[0]=argvexp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp264,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp263->tl=new_args;
_tmp263;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_args);Cyc_Toc_exp_to_c(nv,_tmp206);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(_tmp206,new_args,0),0);if(_tmp20C->inject){struct Cyc_Absyn_Tuniondecl*
tud;{void*_tmp265=Cyc_Tcutil_compress((void*)_tmp20C->type);struct Cyc_Absyn_TunionInfo
_tmp266;void*_tmp267;struct Cyc_Absyn_Tuniondecl**_tmp268;struct Cyc_Absyn_Tuniondecl*
_tmp269;_LL20F: if(_tmp265 <= (void*)3?1:*((int*)_tmp265)!= 2)goto _LL211;_tmp266=((
struct Cyc_Absyn_TunionType_struct*)_tmp265)->f1;_tmp267=(void*)_tmp266.tunion_info;
if(*((int*)_tmp267)!= 1)goto _LL211;_tmp268=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp267)->f1;_tmp269=*_tmp268;_LL210: tud=_tmp269;goto _LL20E;_LL211:;_LL212:({
void*_tmp26A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("toc: unknown tunion in vararg with inject",sizeof(char),42),_tag_arr(
_tmp26A,sizeof(void*),0));});_LL20E:;}{struct _tagged_arr vs=({unsigned int _tmp271=(
unsigned int)_tmp20A;struct _tuple1**_tmp272=(struct _tuple1**)_region_malloc(r,
_check_times(sizeof(struct _tuple1*),_tmp271));struct _tagged_arr _tmp274=_tag_arr(
_tmp272,sizeof(struct _tuple1*),(unsigned int)_tmp20A);{unsigned int _tmp273=
_tmp271;unsigned int i;for(i=0;i < _tmp273;i ++){_tmp272[i]=Cyc_Toc_temp_var();}}
_tmp274;});struct Cyc_List_List*_tmp26B=0;{int i=_tmp20A - 1;for(0;i >= 0;-- i){
_tmp26B=({struct Cyc_List_List*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(((struct _tuple1**)vs.curr)[
i],0),0));_tmp26C->tl=_tmp26B;_tmp26C;});}}s=Cyc_Absyn_declare_stmt(argv,
arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp26B,0),s,0);{
int i=0;for(0;_tmp207 != 0;(((_tmp207=_tmp207->tl,_tmp20B=_tmp20B->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp207->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple1*var=((struct _tuple1**)
vs.curr)[i];struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield
_tmp26E;struct _tuple1*_tmp26F;struct Cyc_List_List*_tmp270;struct Cyc_Absyn_Tunionfield*
_tmp26D=(struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp20B))->hd;
_tmp26E=*_tmp26D;_tmp26F=_tmp26E.name;_tmp270=_tmp26E.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp270))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Absyn_cast_exp(field_typ,arg,
0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,
Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp26F,
1),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp26F,_tag_arr("_struct",sizeof(char),8))),0,s,0);}}}}}else{{int i=0;for(0;
_tmp207 != 0;(_tmp207=_tmp207->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp207->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp207->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL18C;_LL1A5: if(*((
int*)_tmp1F0)!= 10)goto _LL1A7;_tmp20D=((struct Cyc_Absyn_Throw_e_struct*)_tmp1F0)->f1;
_LL1A6: Cyc_Toc_exp_to_c(nv,_tmp20D);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp20D),0))->r));goto _LL18C;
_LL1A7: if(*((int*)_tmp1F0)!= 11)goto _LL1A9;_tmp20E=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1F0)->f1;_LL1A8: Cyc_Toc_exp_to_c(nv,_tmp20E);goto _LL18C;_LL1A9: if(*((int*)
_tmp1F0)!= 12)goto _LL1AB;_tmp20F=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1F0)->f1;
_tmp210=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1F0)->f2;_LL1AA: Cyc_Toc_exp_to_c(
nv,_tmp20F);for(0;_tmp210 != 0;_tmp210=_tmp210->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp210->hd);if(k != (void*)4?k != (void*)3: 0){{void*_tmp277=Cyc_Tcutil_compress((
void*)_tmp210->hd);_LL214: if(_tmp277 <= (void*)3?1:*((int*)_tmp277)!= 1)goto
_LL216;_LL215: goto _LL217;_LL216: if(_tmp277 <= (void*)3?1:*((int*)_tmp277)!= 2)
goto _LL218;_LL217: continue;_LL218:;_LL219:(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp20F,
0))->r));goto _LL213;_LL213:;}break;}}goto _LL18C;_LL1AB: if(*((int*)_tmp1F0)!= 13)
goto _LL1AD;_tmp211=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1F0)->f1;_tmp212=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1F0)->f1);_tmp213=((struct
Cyc_Absyn_Cast_e_struct*)_tmp1F0)->f2;_LL1AC: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp213->topt))->v;void*new_typ=*_tmp212;*_tmp212=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp213);{struct _tuple0 _tmp279=({struct _tuple0
_tmp278;_tmp278.f1=Cyc_Tcutil_compress(old_t2);_tmp278.f2=Cyc_Tcutil_compress(
new_typ);_tmp278;});void*_tmp27A;struct Cyc_Absyn_PtrInfo _tmp27B;void*_tmp27C;
struct Cyc_Absyn_PtrInfo _tmp27D;void*_tmp27E;struct Cyc_Absyn_PtrInfo _tmp27F;void*
_tmp280;_LL21B: _tmp27A=_tmp279.f1;if(_tmp27A <= (void*)3?1:*((int*)_tmp27A)!= 4)
goto _LL21D;_tmp27B=((struct Cyc_Absyn_PointerType_struct*)_tmp27A)->f1;_tmp27C=
_tmp279.f2;if(_tmp27C <= (void*)3?1:*((int*)_tmp27C)!= 4)goto _LL21D;_tmp27D=((
struct Cyc_Absyn_PointerType_struct*)_tmp27C)->f1;_LL21C: {int _tmp281=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp27B.nullable);int
_tmp282=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp27D.nullable);
void*_tmp283=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp27B.bounds);void*
_tmp284=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp27D.bounds);{struct _tuple0
_tmp286=({struct _tuple0 _tmp285;_tmp285.f1=_tmp283;_tmp285.f2=_tmp284;_tmp285;});
void*_tmp287;void*_tmp288;void*_tmp289;struct Cyc_Absyn_Exp*_tmp28A;void*_tmp28B;
void*_tmp28C;void*_tmp28D;struct Cyc_Absyn_Exp*_tmp28E;_LL222: _tmp287=_tmp286.f1;
if(_tmp287 <= (void*)1?1:*((int*)_tmp287)!= 0)goto _LL224;_tmp288=_tmp286.f2;if(
_tmp288 <= (void*)1?1:*((int*)_tmp288)!= 0)goto _LL224;_LL223: if(_tmp281?!_tmp282:
0){if(nv->toplevel)({void*_tmp28F[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("can't do NULL-check conversion at top-level",
sizeof(char),44),_tag_arr(_tmp28F,sizeof(void*),0));});(void*)(e->r=(void*)((
void*)(Cyc_Absyn_cast_exp(*_tmp212,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp290=_cycalloc(sizeof(*_tmp290));_tmp290->hd=_tmp213;
_tmp290->tl=0;_tmp290;}),0),0))->r));}goto _LL221;_LL224: _tmp289=_tmp286.f1;if(
_tmp289 <= (void*)1?1:*((int*)_tmp289)!= 0)goto _LL226;_tmp28A=((struct Cyc_Absyn_Upper_b_struct*)
_tmp289)->f1;_tmp28B=_tmp286.f2;if((int)_tmp28B != 0)goto _LL226;_LL225: if(nv->toplevel)(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_t2,_tmp28A,
_tmp213))->r));else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp291[3];_tmp291[2]=_tmp28A;_tmp291[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp27D.elt_typ),0);_tmp291[0]=_tmp213;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp291,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}goto _LL221;_LL226: _tmp28C=_tmp286.f1;if((int)_tmp28C != 0)goto
_LL228;_tmp28D=_tmp286.f2;if(_tmp28D <= (void*)1?1:*((int*)_tmp28D)!= 0)goto
_LL228;_tmp28E=((struct Cyc_Absyn_Upper_b_struct*)_tmp28D)->f1;_LL227: if(nv->toplevel)({
void*_tmp292[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level",sizeof(char),45),
_tag_arr(_tmp292,sizeof(void*),0));});{struct Cyc_Absyn_Exp*_tmp293=Cyc_Absyn_fncall_exp(
Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*_tmp295[3];_tmp295[2]=_tmp28E;_tmp295[
1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_tmp27B.elt_typ),0);_tmp295[0]=
_tmp213;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp295,sizeof(struct Cyc_Absyn_Exp*),3));}),0);if(_tmp282)(void*)(_tmp293->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp294=_cycalloc(sizeof(*_tmp294));_tmp294->hd=Cyc_Absyn_copy_exp(_tmp293);
_tmp294->tl=0;_tmp294;}),0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(*
_tmp212,_tmp293,0))->r));goto _LL221;}_LL228:;_LL229: goto _LL221;_LL221:;}goto
_LL21A;}_LL21D: _tmp27E=_tmp279.f1;if(_tmp27E <= (void*)3?1:*((int*)_tmp27E)!= 4)
goto _LL21F;_tmp27F=((struct Cyc_Absyn_PointerType_struct*)_tmp27E)->f1;_tmp280=
_tmp279.f2;if(_tmp280 <= (void*)3?1:*((int*)_tmp280)!= 5)goto _LL21F;_LL21E:{void*
_tmp296=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp27F.bounds);_LL22B: if((int)
_tmp296 != 0)goto _LL22D;_LL22C:(void*)(_tmp213->r=(void*)((void*)(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_new_exp((void*)_tmp213->r,_tmp213->loc),Cyc_Toc_curr_sp,0))->r));goto
_LL22A;_LL22D:;_LL22E: goto _LL22A;_LL22A:;}goto _LL21A;_LL21F:;_LL220: goto _LL21A;
_LL21A:;}goto _LL18C;}_LL1AD: if(*((int*)_tmp1F0)!= 14)goto _LL1AF;_tmp214=((struct
Cyc_Absyn_Address_e_struct*)_tmp1F0)->f1;_LL1AE:{void*_tmp297=(void*)_tmp214->r;
struct _tuple1*_tmp298;struct Cyc_List_List*_tmp299;struct Cyc_List_List*_tmp29A;
struct Cyc_List_List*_tmp29B;_LL230: if(*((int*)_tmp297)!= 28)goto _LL232;_tmp298=((
struct Cyc_Absyn_Struct_e_struct*)_tmp297)->f1;_tmp299=((struct Cyc_Absyn_Struct_e_struct*)
_tmp297)->f2;_tmp29A=((struct Cyc_Absyn_Struct_e_struct*)_tmp297)->f3;_LL231: if(
nv->toplevel)({struct Cyc_Std_String_pa_struct _tmp29D;_tmp29D.tag=0;_tmp29D.f1=(
struct _tagged_arr)Cyc_Position_string_of_segment(_tmp214->loc);{void*_tmp29C[1]={&
_tmp29D};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level",sizeof(char),42),_tag_arr(
_tmp29C,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp214->topt))->v,_tmp299 != 0,1,0,
_tmp29A,_tmp298))->r));goto _LL22F;_LL232: if(*((int*)_tmp297)!= 24)goto _LL234;
_tmp29B=((struct Cyc_Absyn_Tuple_e_struct*)_tmp297)->f1;_LL233: if(nv->toplevel)({
struct Cyc_Std_String_pa_struct _tmp29F;_tmp29F.tag=0;_tmp29F.f1=(struct
_tagged_arr)Cyc_Position_string_of_segment(_tmp214->loc);{void*_tmp29E[1]={&
_tmp29F};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level",sizeof(char),42),_tag_arr(
_tmp29E,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,
1,0,_tmp29B))->r));goto _LL22F;_LL234:;_LL235: Cyc_Toc_exp_to_c(nv,_tmp214);if(!
Cyc_Absyn_is_lvalue(_tmp214)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp214,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)((void*)(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp214,
0))->r));}goto _LL22F;_LL22F:;}goto _LL18C;_LL1AF: if(*((int*)_tmp1F0)!= 15)goto
_LL1B1;_tmp215=((struct Cyc_Absyn_New_e_struct*)_tmp1F0)->f1;_tmp216=((struct Cyc_Absyn_New_e_struct*)
_tmp1F0)->f2;_LL1B0: if(nv->toplevel)({struct Cyc_Std_String_pa_struct _tmp2A1;
_tmp2A1.tag=0;_tmp2A1.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp216->loc);{void*_tmp2A0[1]={& _tmp2A1};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("%s: new at top-level",sizeof(char),21),
_tag_arr(_tmp2A0,sizeof(void*),1));}});{void*_tmp2A2=(void*)_tmp216->r;struct Cyc_List_List*
_tmp2A3;struct Cyc_Absyn_Vardecl*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A5;struct Cyc_Absyn_Exp*
_tmp2A6;struct _tuple1*_tmp2A7;struct Cyc_List_List*_tmp2A8;struct Cyc_List_List*
_tmp2A9;struct Cyc_List_List*_tmp2AA;_LL237: if(*((int*)_tmp2A2)!= 26)goto _LL239;
_tmp2A3=((struct Cyc_Absyn_Array_e_struct*)_tmp2A2)->f1;_LL238: {struct _tuple1*
_tmp2AB=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2AC=Cyc_Absyn_var_exp(_tmp2AB,
0);struct Cyc_Absyn_Stmt*_tmp2AD=Cyc_Toc_init_array(nv,_tmp2AC,_tmp2A3,Cyc_Absyn_exp_stmt(
_tmp2AC,0));void*old_elt_typ;{void*_tmp2AE=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_PtrInfo _tmp2AF;void*_tmp2B0;struct Cyc_Absyn_Tqual _tmp2B1;_LL242: if(
_tmp2AE <= (void*)3?1:*((int*)_tmp2AE)!= 4)goto _LL244;_tmp2AF=((struct Cyc_Absyn_PointerType_struct*)
_tmp2AE)->f1;_tmp2B0=(void*)_tmp2AF.elt_typ;_tmp2B1=_tmp2AF.tq;_LL243:
old_elt_typ=_tmp2B0;goto _LL241;_LL244:;_LL245: old_elt_typ=({void*_tmp2B2[0]={};
Cyc_Toc_toc_impos(_tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof(char),52),_tag_arr(_tmp2B2,sizeof(void*),0));});_LL241:;}{void*elt_typ=
Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp2B3=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2B4=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,
0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2A3),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp215 == 0)e1=Cyc_Toc_malloc_exp(
old_elt_typ,_tmp2B4);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(_tmp215);Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp2B4);}(
void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2AB,
_tmp2B3,(struct Cyc_Absyn_Exp*)e1,_tmp2AD,0),0))->r));goto _LL236;}}_LL239: if(*((
int*)_tmp2A2)!= 27)goto _LL23B;_tmp2A4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2A2)->f1;_tmp2A5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A2)->f2;
_tmp2A6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2A2)->f3;_LL23A: {int
is_tagged_ptr=0;{void*_tmp2B5=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp2B6;void*_tmp2B7;struct Cyc_Absyn_Tqual _tmp2B8;struct Cyc_Absyn_Conref*_tmp2B9;
_LL247: if(_tmp2B5 <= (void*)3?1:*((int*)_tmp2B5)!= 4)goto _LL249;_tmp2B6=((struct
Cyc_Absyn_PointerType_struct*)_tmp2B5)->f1;_tmp2B7=(void*)_tmp2B6.elt_typ;
_tmp2B8=_tmp2B6.tq;_tmp2B9=_tmp2B6.bounds;_LL248: is_tagged_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp2B9)== (void*)0;goto _LL246;_LL249:;_LL24A:({void*
_tmp2BA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: comprehension not an array type",sizeof(char),42),_tag_arr(
_tmp2BA,sizeof(void*),0));});_LL246:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2A6->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp2A5);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),
_tmp2A4,Cyc_Absyn_var_exp(max,0),_tmp2A6,Cyc_Toc_skip_stmt_dl(),1);{struct
_RegionHandle _tmp2BB=_new_region("r");struct _RegionHandle*r=& _tmp2BB;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp2CB=
_region_malloc(r,sizeof(*_tmp2CB));_tmp2CB->hd=({struct _tuple9*_tmp2CC=
_region_malloc(r,sizeof(*_tmp2CC));_tmp2CC->f1=max;_tmp2CC->f2=Cyc_Absyn_uint_t;
_tmp2CC->f3=(struct Cyc_Absyn_Exp*)_tmp2A5;_tmp2CC;});_tmp2CB->tl=0;_tmp2CB;});
struct Cyc_Absyn_Exp*ai;if(_tmp215 == 0)ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp2BC[2];_tmp2BC[1]=Cyc_Absyn_var_exp(
max,0);_tmp2BC[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2BC,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(_tmp215);
Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp2BD[2];_tmp2BD[1]=Cyc_Absyn_var_exp(max,0);_tmp2BD[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp2BD,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct
Cyc_Absyn_Exp*ainit=Cyc_Absyn_cast_exp(ptr_typ,ai,0);decls=({struct Cyc_List_List*
_tmp2BE=_region_malloc(r,sizeof(*_tmp2BE));_tmp2BE->hd=({struct _tuple9*_tmp2BF=
_region_malloc(r,sizeof(*_tmp2BF));_tmp2BF->f1=a;_tmp2BF->f2=ptr_typ;_tmp2BF->f3=(
struct Cyc_Absyn_Exp*)ainit;_tmp2BF;});_tmp2BE->tl=decls;_tmp2BE;});if(
is_tagged_ptr){struct _tuple1*_tmp2C0=Cyc_Toc_temp_var();void*_tmp2C1=Cyc_Toc_typ_to_c(
old_typ);struct Cyc_Absyn_Exp*_tmp2C2=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2C5[3];_tmp2C5[2]=_tmp2A5;_tmp2C5[1]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0);_tmp2C5[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp2C5,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);decls=({struct Cyc_List_List*_tmp2C3=_region_malloc(r,sizeof(*_tmp2C3));
_tmp2C3->hd=({struct _tuple9*_tmp2C4=_region_malloc(r,sizeof(*_tmp2C4));_tmp2C4->f1=
_tmp2C0;_tmp2C4->f2=_tmp2C1;_tmp2C4->f3=(struct Cyc_Absyn_Exp*)_tmp2C2;_tmp2C4;});
_tmp2C3->tl=decls;_tmp2C3;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
_tmp2C0,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
a,0),0),0);}{struct Cyc_List_List*_tmp2C6=decls;for(0;_tmp2C6 != 0;_tmp2C6=_tmp2C6->tl){
struct _tuple1*_tmp2C8;void*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;struct _tuple9
_tmp2C7=*((struct _tuple9*)_tmp2C6->hd);_tmp2C8=_tmp2C7.f1;_tmp2C9=_tmp2C7.f2;
_tmp2CA=_tmp2C7.f3;s=Cyc_Absyn_declare_stmt(_tmp2C8,_tmp2C9,_tmp2CA,s,0);}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL236;}}}
_LL23B: if(*((int*)_tmp2A2)!= 28)goto _LL23D;_tmp2A7=((struct Cyc_Absyn_Struct_e_struct*)
_tmp2A2)->f1;_tmp2A8=((struct Cyc_Absyn_Struct_e_struct*)_tmp2A2)->f2;_tmp2A9=((
struct Cyc_Absyn_Struct_e_struct*)_tmp2A2)->f3;_LL23C:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp216->topt))->v,
_tmp2A8 != 0,1,_tmp215,_tmp2A9,_tmp2A7))->r));goto _LL236;_LL23D: if(*((int*)
_tmp2A2)!= 24)goto _LL23F;_tmp2AA=((struct Cyc_Absyn_Tuple_e_struct*)_tmp2A2)->f1;
_LL23E:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,_tmp215,_tmp2AA))->r));
goto _LL236;_LL23F:;_LL240: {void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp216->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct
_tuple1*_tmp2CD=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2CE=Cyc_Absyn_var_exp(
_tmp2CD,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(
_tmp2CE,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp215 == 0)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(
_tmp215);Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{
void*_tmp2CF=(void*)_tmp216->r;void*_tmp2D0;struct Cyc_Absyn_Exp*_tmp2D1;_LL24C:
if(*((int*)_tmp2CF)!= 13)goto _LL24E;_tmp2D0=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp2CF)->f1;_tmp2D1=((struct Cyc_Absyn_Cast_e_struct*)_tmp2CF)->f2;_LL24D:{
struct _tuple0 _tmp2D3=({struct _tuple0 _tmp2D2;_tmp2D2.f1=Cyc_Tcutil_compress(
_tmp2D0);_tmp2D2.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2D1->topt))->v);_tmp2D2;});void*_tmp2D4;struct Cyc_Absyn_PtrInfo _tmp2D5;void*
_tmp2D6;struct Cyc_Absyn_Conref*_tmp2D7;void*_tmp2D8;struct Cyc_Absyn_PtrInfo
_tmp2D9;struct Cyc_Absyn_Conref*_tmp2DA;_LL251: _tmp2D4=_tmp2D3.f1;if(_tmp2D4 <= (
void*)3?1:*((int*)_tmp2D4)!= 4)goto _LL253;_tmp2D5=((struct Cyc_Absyn_PointerType_struct*)
_tmp2D4)->f1;_tmp2D6=(void*)_tmp2D5.elt_typ;_tmp2D7=_tmp2D5.bounds;_tmp2D8=
_tmp2D3.f2;if(_tmp2D8 <= (void*)3?1:*((int*)_tmp2D8)!= 4)goto _LL253;_tmp2D9=((
struct Cyc_Absyn_PointerType_struct*)_tmp2D8)->f1;_tmp2DA=_tmp2D9.bounds;_LL252:{
struct _tuple0 _tmp2DC=({struct _tuple0 _tmp2DB;_tmp2DB.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp2D7);_tmp2DB.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp2DA);_tmp2DB;});
void*_tmp2DD;void*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2DF;_LL256: _tmp2DD=_tmp2DC.f1;
if((int)_tmp2DD != 0)goto _LL258;_tmp2DE=_tmp2DC.f2;if(_tmp2DE <= (void*)1?1:*((int*)
_tmp2DE)!= 0)goto _LL258;_tmp2DF=((struct Cyc_Absyn_Upper_b_struct*)_tmp2DE)->f1;
_LL257: Cyc_Toc_exp_to_c(nv,_tmp2D1);(void*)(inner_mexp->r=(void*)((void*)(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0))->r));done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp2E0[4];_tmp2E0[3]=_tmp2DF;_tmp2E0[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp2D6),0);_tmp2E0[1]=_tmp2D1;_tmp2E0[0]=mexp;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp2E0,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))->r));goto _LL255;_LL258:;_LL259: goto _LL255;_LL255:;}goto _LL250;_LL253:;
_LL254: goto _LL250;_LL250:;}goto _LL24B;_LL24E:;_LL24F: goto _LL24B;_LL24B:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp2E1=Cyc_Absyn_exp_stmt(_tmp2CE,0);struct Cyc_Absyn_Exp*
_tmp2E2=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp216);_tmp2E1=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp2CE,_tmp2E2,0),_tmp216,0),
_tmp2E1,0);{void*_tmp2E3=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2CD,_tmp2E3,(struct
Cyc_Absyn_Exp*)mexp,_tmp2E1,0),0))->r));}}goto _LL236;}}_LL236:;}goto _LL18C;
_LL1B1: if(*((int*)_tmp1F0)!= 17)goto _LL1B3;_tmp217=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp1F0)->f1;_LL1B2: Cyc_Toc_exp_to_c(nv,_tmp217);goto _LL18C;_LL1B3: if(*((int*)
_tmp1F0)!= 16)goto _LL1B5;_tmp218=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp1F0)->f1;_LL1B4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp2E4=_cycalloc(sizeof(*_tmp2E4));_tmp2E4[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp2E5;_tmp2E5.tag=16;_tmp2E5.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp218);_tmp2E5;});
_tmp2E4;})));goto _LL18C;_LL1B5: if(*((int*)_tmp1F0)!= 19)goto _LL1B7;_LL1B6:({void*
_tmp2E6[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("__gen() in code generator",sizeof(char),26),_tag_arr(_tmp2E6,sizeof(
void*),0));});_LL1B7: if(*((int*)_tmp1F0)!= 18)goto _LL1B9;_tmp219=(void*)((struct
Cyc_Absyn_Offsetof_e_struct*)_tmp1F0)->f1;_tmp21A=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1F0)->f2;if(*((int*)_tmp21A)!= 0)goto _LL1B9;_tmp21B=((struct Cyc_Absyn_StructField_struct*)
_tmp21A)->f1;_LL1B8:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp2E7=_cycalloc(sizeof(*_tmp2E7));_tmp2E7[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp2E8;_tmp2E8.tag=18;_tmp2E8.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp219);_tmp2E8.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2E9=_cycalloc(sizeof(*
_tmp2E9));_tmp2E9[0]=({struct Cyc_Absyn_StructField_struct _tmp2EA;_tmp2EA.tag=0;
_tmp2EA.f1=_tmp21B;_tmp2EA;});_tmp2E9;}));_tmp2E8;});_tmp2E7;})));goto _LL18C;
_LL1B9: if(*((int*)_tmp1F0)!= 18)goto _LL1BB;_tmp21C=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1F0)->f1;_tmp21D=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1F0)->f2;
if(*((int*)_tmp21D)!= 1)goto _LL1BB;_tmp21E=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp21D)->f1;_LL1BA:{void*_tmp2EB=Cyc_Tcutil_compress(_tmp21C);struct Cyc_Absyn_AggrInfo
_tmp2EC;void*_tmp2ED;struct Cyc_List_List*_tmp2EE;_LL25B: if(_tmp2EB <= (void*)3?1:*((
int*)_tmp2EB)!= 10)goto _LL25D;_tmp2EC=((struct Cyc_Absyn_AggrType_struct*)_tmp2EB)->f1;
_tmp2ED=(void*)_tmp2EC.aggr_info;_LL25C: {struct Cyc_Absyn_Aggrdecl*_tmp2EF=Cyc_Absyn_get_known_aggrdecl(
_tmp2ED);if(_tmp2EF->fields == 0)({void*_tmp2F0[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("struct fields must be known",
sizeof(char),28),_tag_arr(_tmp2F0,sizeof(void*),0));});_tmp2EE=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2EF->fields))->v;goto _LL25E;}_LL25D: if(
_tmp2EB <= (void*)3?1:*((int*)_tmp2EB)!= 11)goto _LL25F;_tmp2EE=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2EB)->f2;_LL25E: {struct Cyc_Absyn_Aggrfield*_tmp2F1=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2EE,(int)_tmp21E);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp2F2=_cycalloc(sizeof(*
_tmp2F2));_tmp2F2[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp2F3;_tmp2F3.tag=18;
_tmp2F3.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp21C);_tmp2F3.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4[0]=({
struct Cyc_Absyn_StructField_struct _tmp2F5;_tmp2F5.tag=0;_tmp2F5.f1=_tmp2F1->name;
_tmp2F5;});_tmp2F4;}));_tmp2F3;});_tmp2F2;})));goto _LL25A;}_LL25F: if(_tmp2EB <= (
void*)3?1:*((int*)_tmp2EB)!= 9)goto _LL261;_LL260:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp2F6=_cycalloc(sizeof(*_tmp2F6));_tmp2F6[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp2F7;_tmp2F7.tag=18;_tmp2F7.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp21C);_tmp2F7.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2F8=
_cycalloc(sizeof(*_tmp2F8));_tmp2F8[0]=({struct Cyc_Absyn_StructField_struct
_tmp2F9;_tmp2F9.tag=0;_tmp2F9.f1=Cyc_Absyn_fieldname((int)(_tmp21E + 1));_tmp2F9;});
_tmp2F8;}));_tmp2F7;});_tmp2F6;})));goto _LL25A;_LL261: if(_tmp2EB <= (void*)3?1:*((
int*)_tmp2EB)!= 3)goto _LL263;_LL262: if(_tmp21E == 0)(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp2FA=_cycalloc(sizeof(*_tmp2FA));_tmp2FA[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp2FB;_tmp2FB.tag=18;_tmp2FB.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp21C);_tmp2FB.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp2FC=
_cycalloc(sizeof(*_tmp2FC));_tmp2FC[0]=({struct Cyc_Absyn_StructField_struct
_tmp2FD;_tmp2FD.tag=0;_tmp2FD.f1=Cyc_Toc_tag_sp;_tmp2FD;});_tmp2FC;}));_tmp2FB;});
_tmp2FA;})));else{(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp2FE=_cycalloc(sizeof(*_tmp2FE));_tmp2FE[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp2FF;_tmp2FF.tag=18;_tmp2FF.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp21C);_tmp2FF.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp300=_cycalloc(sizeof(*
_tmp300));_tmp300[0]=({struct Cyc_Absyn_StructField_struct _tmp301;_tmp301.tag=0;
_tmp301.f1=Cyc_Absyn_fieldname((int)_tmp21E);_tmp301;});_tmp300;}));_tmp2FF;});
_tmp2FE;})));}goto _LL25A;_LL263:;_LL264:({void*_tmp302[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("impossible type for offsetof tuple index",
sizeof(char),41),_tag_arr(_tmp302,sizeof(void*),0));});_LL25A:;}goto _LL18C;
_LL1BB: if(*((int*)_tmp1F0)!= 20)goto _LL1BD;_tmp21F=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1F0)->f1;_LL1BC: {void*_tmp303=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp21F->topt))->v);{void*_tmp304=_tmp303;struct Cyc_Absyn_PtrInfo
_tmp305;void*_tmp306;void*_tmp307;struct Cyc_Absyn_Conref*_tmp308;struct Cyc_Absyn_Tqual
_tmp309;struct Cyc_Absyn_Conref*_tmp30A;_LL266: if(_tmp304 <= (void*)3?1:*((int*)
_tmp304)!= 4)goto _LL268;_tmp305=((struct Cyc_Absyn_PointerType_struct*)_tmp304)->f1;
_tmp306=(void*)_tmp305.elt_typ;_tmp307=(void*)_tmp305.rgn_typ;_tmp308=_tmp305.nullable;
_tmp309=_tmp305.tq;_tmp30A=_tmp305.bounds;_LL267:{void*_tmp30B=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp30A);_LL26B: if(_tmp30B <= (void*)1?1:*((int*)_tmp30B)!= 
0)goto _LL26D;_LL26C: {int do_null_check=Cyc_Toc_need_null_check(_tmp21F);Cyc_Toc_exp_to_c(
nv,_tmp21F);if(do_null_check)(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp21F->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*
_tmp30C=_cycalloc(sizeof(*_tmp30C));_tmp30C->hd=_tmp21F;_tmp30C->tl=0;_tmp30C;}),
0),0),0))->r));goto _LL26A;}_LL26D: if((int)_tmp30B != 0)goto _LL26A;_LL26E: {struct
Cyc_Absyn_Exp*_tmp30D=Cyc_Absyn_uint_exp(0,0);_tmp30D->topt=({struct Cyc_Core_Opt*
_tmp30E=_cycalloc(sizeof(*_tmp30E));_tmp30E->v=(void*)Cyc_Absyn_uint_t;_tmp30E;});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(_tmp21F,_tmp30D,0))->r));Cyc_Toc_exp_to_c(
nv,e);goto _LL26A;}_LL26A:;}goto _LL265;_LL268:;_LL269:({void*_tmp30F[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: Deref: non-pointer",
sizeof(char),29),_tag_arr(_tmp30F,sizeof(void*),0));});_LL265:;}goto _LL18C;}
_LL1BD: if(*((int*)_tmp1F0)!= 21)goto _LL1BF;_tmp220=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1F0)->f1;_tmp221=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1F0)->f2;_LL1BE:
Cyc_Toc_exp_to_c(nv,_tmp220);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL18C;
_LL1BF: if(*((int*)_tmp1F0)!= 22)goto _LL1C1;_tmp222=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1F0)->f1;_tmp223=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1F0)->f2;_LL1C0: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp222->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp222);Cyc_Toc_exp_to_c(nv,_tmp222);{
int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp311;void*_tmp312;struct Cyc_Absyn_Conref*
_tmp313;struct Cyc_Absyn_Tqual _tmp314;struct Cyc_Absyn_Conref*_tmp315;struct Cyc_Absyn_PtrInfo
_tmp310=Cyc_Toc_get_ptr_type(e1typ);_tmp311=(void*)_tmp310.elt_typ;_tmp312=(void*)
_tmp310.rgn_typ;_tmp313=_tmp310.nullable;_tmp314=_tmp310.tq;_tmp315=_tmp310.bounds;{
void*_tmp316=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp315);struct Cyc_Absyn_Exp*
_tmp317;_LL270: if(_tmp316 <= (void*)1?1:*((int*)_tmp316)!= 0)goto _LL272;_tmp317=((
struct Cyc_Absyn_Upper_b_struct*)_tmp316)->f1;_LL271: if(Cyc_Evexp_eval_const_uint_exp(
_tmp317)< 1)({void*_tmp318[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(_tag_arr("exp_to_c:  AggrArrow_e on pointer of size 0",
sizeof(char),44),_tag_arr(_tmp318,sizeof(void*),0));});if(do_null_check)(void*)(
e->r=(void*)((void*)(Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp222->topt))->v),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp319=_cycalloc(sizeof(*_tmp319));
_tmp319->hd=_tmp222;_tmp319->tl=0;_tmp319;}),0),0),_tmp223,0))->r));goto _LL26F;
_LL272: if((int)_tmp316 != 0)goto _LL26F;_LL273: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp311);(void*)(_tmp222->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
ta1,_tmp314),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*
_tmp31A[3];_tmp31A[2]=Cyc_Absyn_uint_exp(0,0);_tmp31A[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp31A[0]=Cyc_Absyn_copy_exp(_tmp222);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp31A,sizeof(struct Cyc_Absyn_Exp*),3));}),
0),0))->r));goto _LL26F;}_LL26F:;}if(is_poly)(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((
void*)e->r,0),0))->r));goto _LL18C;}}_LL1C1: if(*((int*)_tmp1F0)!= 23)goto _LL1C3;
_tmp224=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1F0)->f1;_tmp225=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1F0)->f2;_LL1C2: {void*_tmp31B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp224->topt))->v);{void*_tmp31C=_tmp31B;struct Cyc_List_List*
_tmp31D;struct Cyc_Absyn_PtrInfo _tmp31E;void*_tmp31F;void*_tmp320;struct Cyc_Absyn_Conref*
_tmp321;struct Cyc_Absyn_Tqual _tmp322;struct Cyc_Absyn_Conref*_tmp323;_LL275: if(
_tmp31C <= (void*)3?1:*((int*)_tmp31C)!= 9)goto _LL277;_tmp31D=((struct Cyc_Absyn_TupleType_struct*)
_tmp31C)->f1;_LL276: Cyc_Toc_exp_to_c(nv,_tmp224);Cyc_Toc_exp_to_c(nv,_tmp225);{
unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmp225)+ 1;(void*)(e->r=(void*)((
void*)(Cyc_Absyn_aggrmember_exp(_tmp224,Cyc_Absyn_fieldname((int)i),0))->r));
goto _LL274;}_LL277: if(_tmp31C <= (void*)3?1:*((int*)_tmp31C)!= 4)goto _LL279;
_tmp31E=((struct Cyc_Absyn_PointerType_struct*)_tmp31C)->f1;_tmp31F=(void*)
_tmp31E.elt_typ;_tmp320=(void*)_tmp31E.rgn_typ;_tmp321=_tmp31E.nullable;_tmp322=
_tmp31E.tq;_tmp323=_tmp31E.bounds;_LL278: {struct Cyc_List_List*_tmp324=Cyc_Toc_get_relns(
_tmp224);int _tmp325=Cyc_Toc_check_bounds(_tmp324,_tmp224,_tmp225);if(Cyc_Toc_warn_bounds_checks?
!_tmp325: 0)({struct Cyc_Std_String_pa_struct _tmp327;_tmp327.tag=0;_tmp327.f1=(
struct _tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp326[1]={& _tmp327};Cyc_Tcutil_warn(
e->loc,_tag_arr("bounds check necessary for %s",sizeof(char),30),_tag_arr(
_tmp326,sizeof(void*),1));}});Cyc_Toc_exp_to_c(nv,_tmp224);Cyc_Toc_exp_to_c(nv,
_tmp225);Cyc_Toc_total_bounds_checks ++;{void*_tmp328=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp323);struct Cyc_Absyn_Exp*_tmp329;_LL27C: if(_tmp328 <= (void*)1?1:*((int*)
_tmp328)!= 0)goto _LL27E;_tmp329=((struct Cyc_Absyn_Upper_b_struct*)_tmp328)->f1;
_LL27D: _tmp329=Cyc_Absyn_uint_exp(Cyc_Evexp_eval_const_uint_exp(_tmp329),0);{int
possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp321);void*ta1=Cyc_Toc_typ_to_c(_tmp31F);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp322);if(_tmp325)Cyc_Toc_bounds_checks_eliminated ++;else{if(possibly_null)(
void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp32A[4];_tmp32A[3]=
_tmp225;_tmp32A[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp32A[1]=_tmp329;_tmp32A[0]=
_tmp224;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp32A,sizeof(struct Cyc_Absyn_Exp*),4));}),0),0),0))->r));else{(void*)(_tmp225->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp32B[2];_tmp32B[1]=Cyc_Absyn_copy_exp(_tmp225);_tmp32B[0]=
_tmp329;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp32B,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}goto _LL27B;}_LL27E: if((
int)_tmp328 != 0)goto _LL27B;_LL27F: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp31F);if(
_tmp325){Cyc_Toc_bounds_checks_eliminated ++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(
Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp322),Cyc_Absyn_aggrmember_exp(
_tmp224,Cyc_Toc_curr_sp,0),0),_tmp225,0))->r));}else{(void*)(e->r=(void*)((void*)(
Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(ta1,_tmp322),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*_tmp32C[3];_tmp32C[2]=
_tmp225;_tmp32C[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp32C[0]=_tmp224;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp32C,sizeof(struct Cyc_Absyn_Exp*),
3));}),0),0),0))->r));}goto _LL27B;}_LL27B:;}goto _LL274;}_LL279:;_LL27A:({void*
_tmp32D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",sizeof(char),49),
_tag_arr(_tmp32D,sizeof(void*),0));});_LL274:;}goto _LL18C;}_LL1C3: if(*((int*)
_tmp1F0)!= 24)goto _LL1C5;_tmp226=((struct Cyc_Absyn_Tuple_e_struct*)_tmp1F0)->f1;
_LL1C4: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,
_tmp226))->r));else{struct Cyc_List_List*_tmp32E=((struct Cyc_List_List*(*)(struct
_tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,
_tmp226);struct _tagged_arr*_tmp32F=Cyc_Toc_add_tuple_type(_tmp32E);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp226 != 0;(_tmp226=_tmp226->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp226->hd);dles=({struct Cyc_List_List*_tmp330=_cycalloc(
sizeof(*_tmp330));_tmp330->hd=({struct _tuple5*_tmp331=_cycalloc(sizeof(*_tmp331));
_tmp331->f1=0;_tmp331->f2=(struct Cyc_Absyn_Exp*)_tmp226->hd;_tmp331;});_tmp330->tl=
dles;_tmp330;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,dles,0))->r));}
goto _LL18C;_LL1C5: if(*((int*)_tmp1F0)!= 26)goto _LL1C7;_tmp227=((struct Cyc_Absyn_Array_e_struct*)
_tmp1F0)->f1;_LL1C6:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,
_tmp227,0))->r));{struct Cyc_List_List*_tmp332=_tmp227;for(0;_tmp332 != 0;_tmp332=
_tmp332->tl){struct _tuple5 _tmp334;struct Cyc_Absyn_Exp*_tmp335;struct _tuple5*
_tmp333=(struct _tuple5*)_tmp332->hd;_tmp334=*_tmp333;_tmp335=_tmp334.f2;Cyc_Toc_exp_to_c(
nv,_tmp335);}}goto _LL18C;_LL1C7: if(*((int*)_tmp1F0)!= 27)goto _LL1C9;_tmp228=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1F0)->f1;_tmp229=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1F0)->f2;_tmp22A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1F0)->f3;
_LL1C8: {unsigned int _tmp336=Cyc_Evexp_eval_const_uint_exp(_tmp229);Cyc_Toc_exp_to_c(
nv,_tmp22A);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp22A)){unsigned int
i=0;for(0;i < _tmp336;i ++){es=({struct Cyc_List_List*_tmp337=_cycalloc(sizeof(*
_tmp337));_tmp337->hd=({struct _tuple5*_tmp338=_cycalloc(sizeof(*_tmp338));
_tmp338->f1=0;_tmp338->f2=_tmp22A;_tmp338;});_tmp337->tl=es;_tmp337;});}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));goto _LL18C;}}
_LL1C9: if(*((int*)_tmp1F0)!= 28)goto _LL1CB;_tmp22B=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1F0)->f1;_tmp22C=((struct Cyc_Absyn_Struct_e_struct*)_tmp1F0)->f2;_tmp22D=((
struct Cyc_Absyn_Struct_e_struct*)_tmp1F0)->f3;_tmp22E=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1F0)->f4;_LL1CA: if(!nv->toplevel)(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,_tmp22C != 0,0,0,_tmp22D,_tmp22B))->r));else{if(_tmp22E == 0)({void*
_tmp339[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing aggrdecl pointer",sizeof(char),35),_tag_arr(_tmp339,
sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)
_check_null(_tmp22E);struct _RegionHandle _tmp33A=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp33A;_push_region(rgn);{struct Cyc_List_List*_tmp33B=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,e->loc,_tmp22D,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sd2->fields))->v);
struct Cyc_List_List*_tmp33C=0;{struct Cyc_List_List*_tmp33D=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(sd2->fields))->v;for(0;_tmp33D != 0;_tmp33D=
_tmp33D->tl){struct Cyc_List_List*_tmp33E=_tmp33B;for(0;_tmp33E != 0;_tmp33E=
_tmp33E->tl){if((*((struct _tuple10*)_tmp33E->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)
_tmp33D->hd){struct _tuple10 _tmp340;struct Cyc_Absyn_Aggrfield*_tmp341;struct Cyc_Absyn_Exp*
_tmp342;struct _tuple10*_tmp33F=(struct _tuple10*)_tmp33E->hd;_tmp340=*_tmp33F;
_tmp341=_tmp340.f1;_tmp342=_tmp340.f2;{void*_tmp343=(void*)_tmp341->type;Cyc_Toc_exp_to_c(
nv,_tmp342);if(Cyc_Toc_is_void_star(_tmp343))(void*)(_tmp342->r=(void*)((void*)(
Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp342->r,
0),0))->r));_tmp33C=({struct Cyc_List_List*_tmp344=_cycalloc(sizeof(*_tmp344));
_tmp344->hd=({struct _tuple5*_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345->f1=0;
_tmp345->f2=_tmp342;_tmp345;});_tmp344->tl=_tmp33C;_tmp344;});break;}}}}}(void*)(
e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp33C),0))->r));};_pop_region(rgn);}}
goto _LL18C;_LL1CB: if(*((int*)_tmp1F0)!= 29)goto _LL1CD;_tmp22F=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1F0)->f1;_tmp230=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp1F0)->f2;_LL1CC: {
struct Cyc_List_List*fs;{void*_tmp346=Cyc_Tcutil_compress(_tmp22F);struct Cyc_List_List*
_tmp347;_LL281: if(_tmp346 <= (void*)3?1:*((int*)_tmp346)!= 11)goto _LL283;_tmp347=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp346)->f2;_LL282: fs=_tmp347;goto _LL280;
_LL283:;_LL284:({struct Cyc_Std_String_pa_struct _tmp349;_tmp349.tag=0;_tmp349.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp22F);{void*_tmp348[1]={& _tmp349};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("anon struct has type %s",
sizeof(char),24),_tag_arr(_tmp348,sizeof(void*),1));}});_LL280:;}{struct
_RegionHandle _tmp34A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp34A;
_push_region(rgn);{struct Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp230,
fs);for(0;_tmp34B != 0;_tmp34B=_tmp34B->tl){struct _tuple10 _tmp34D;struct Cyc_Absyn_Aggrfield*
_tmp34E;struct Cyc_Absyn_Exp*_tmp34F;struct _tuple10*_tmp34C=(struct _tuple10*)
_tmp34B->hd;_tmp34D=*_tmp34C;_tmp34E=_tmp34D.f1;_tmp34F=_tmp34D.f2;{void*_tmp350=(
void*)_tmp34E->type;Cyc_Toc_exp_to_c(nv,_tmp34F);if(Cyc_Toc_is_void_star(_tmp350))(
void*)(_tmp34F->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp34F->r,0),0))->r));}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,_tmp230,0))->r));};_pop_region(rgn);}goto _LL18C;}
_LL1CD: if(*((int*)_tmp1F0)!= 30)goto _LL1CF;_tmp231=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1F0)->f1;if(_tmp231 != 0)goto _LL1CF;_tmp232=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1F0)->f2;_tmp233=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1F0)->f3;_LL1CE: {
struct _tuple1*qv=_tmp233->name;struct Cyc_Absyn_Exp*tag_exp=_tmp232->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp232,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL18C;}_LL1CF: if(*((int*)_tmp1F0)!= 30)goto _LL1D1;
_tmp234=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1F0)->f1;_tmp235=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1F0)->f2;_tmp236=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1F0)->f3;_LL1D0: {
struct _tuple1*_tmp351=_tmp236->name;struct Cyc_List_List*_tmp352=_tmp236->typs;
struct _tuple1*_tmp353=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp354=Cyc_Absyn_var_exp(
_tmp353,0);void*_tmp355=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp351,
_tag_arr("_struct",sizeof(char),8)));struct Cyc_Absyn_Exp*tag_exp=_tmp235->is_xtunion?
Cyc_Absyn_var_exp(_tmp351,0): Cyc_Toc_tunion_tag(_tmp235,_tmp351,1);if(nv->toplevel){
struct Cyc_List_List*dles=0;for(0;_tmp234 != 0;(_tmp234=_tmp234->tl,_tmp352=
_tmp352->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp234->hd;void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)_check_null(
_tmp352))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))
cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);dles=({struct Cyc_List_List*_tmp356=
_cycalloc(sizeof(*_tmp356));_tmp356->hd=({struct _tuple5*_tmp357=_cycalloc(
sizeof(*_tmp357));_tmp357->f1=0;_tmp357->f2=cur_e;_tmp357;});_tmp356->tl=dles;
_tmp356;});}dles=({struct Cyc_List_List*_tmp358=_cycalloc(sizeof(*_tmp358));
_tmp358->hd=({struct _tuple5*_tmp359=_cycalloc(sizeof(*_tmp359));_tmp359->f1=0;
_tmp359->f2=tag_exp;_tmp359;});_tmp358->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp358;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt*_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A->v=Cyc_Toc_collapse_qvar_tag(
_tmp351,_tag_arr("_struct",sizeof(char),8));_tmp35A;}),dles,0))->r));}else{
struct Cyc_List_List*_tmp35B=({struct Cyc_List_List*_tmp361=_cycalloc(sizeof(*
_tmp361));_tmp361->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp354,Cyc_Toc_tag_sp,
0),tag_exp,0);_tmp361->tl=0;_tmp361;});{int i=1;for(0;_tmp234 != 0;(((_tmp234=
_tmp234->tl,i ++)),_tmp352=_tmp352->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp234->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp352))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);{struct Cyc_Absyn_Stmt*
_tmp35C=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(_tmp354,Cyc_Absyn_fieldname(
i),0),cur_e,0);_tmp35B=({struct Cyc_List_List*_tmp35D=_cycalloc(sizeof(*_tmp35D));
_tmp35D->hd=_tmp35C;_tmp35D->tl=_tmp35B;_tmp35D;});}}}{struct Cyc_Absyn_Stmt*
_tmp35E=Cyc_Absyn_exp_stmt(_tmp354,0);struct Cyc_Absyn_Stmt*_tmp35F=Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*
_tmp360=_cycalloc(sizeof(*_tmp360));_tmp360->hd=_tmp35E;_tmp360->tl=_tmp35B;
_tmp360;})),0);(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
_tmp353,_tmp355,0,_tmp35F,0),0))->r));}}goto _LL18C;}_LL1D1: if(*((int*)_tmp1F0)!= 
31)goto _LL1D3;_LL1D2: goto _LL1D4;_LL1D3: if(*((int*)_tmp1F0)!= 32)goto _LL1D5;
_LL1D4: goto _LL18C;_LL1D5: if(*((int*)_tmp1F0)!= 33)goto _LL1D7;_tmp237=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp1F0)->f1;_tmp238=_tmp237.is_calloc;_tmp239=_tmp237.rgn;_tmp23A=_tmp237.elt_type;
_tmp23B=_tmp237.num_elts;_tmp23C=_tmp237.fat_result;_LL1D6: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp23A)));Cyc_Toc_exp_to_c(nv,_tmp23B);if(_tmp23C){struct
_tuple1*_tmp362=Cyc_Toc_temp_var();struct _tuple1*_tmp363=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp238){xexp=_tmp23B;if(_tmp239 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp239);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp362,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp23A,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp362,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp364[3];_tmp364[2]=Cyc_Absyn_var_exp(_tmp362,0);_tmp364[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp364[0]=Cyc_Absyn_var_exp(_tmp363,0);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp364,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp23B,0);if(_tmp239 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp239);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp362,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp23A,Cyc_Absyn_var_exp(_tmp362,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp365[3];
_tmp365[2]=Cyc_Absyn_var_exp(_tmp362,0);_tmp365[1]=Cyc_Absyn_uint_exp(1,0);
_tmp365[0]=Cyc_Absyn_var_exp(_tmp363,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp365,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}{struct Cyc_Absyn_Stmt*_tmp366=Cyc_Absyn_declare_stmt(_tmp362,Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp363,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp366,0))->r));}}else{if(_tmp239 != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp239);Cyc_Toc_exp_to_c(
nv,rgn);(void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp23A,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL18C;}_LL1D7: if(*((int*)_tmp1F0)!= 35)goto _LL1D9;_tmp23D=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp1F0)->f1;_LL1D8: Cyc_Toc_stmt_to_c(nv,
_tmp23D);goto _LL18C;_LL1D9: if(*((int*)_tmp1F0)!= 34)goto _LL1DB;_LL1DA:({void*
_tmp367[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem",sizeof(char),14),_tag_arr(_tmp367,sizeof(void*),0));});
_LL1DB: if(*((int*)_tmp1F0)!= 25)goto _LL1DD;_LL1DC:({void*_tmp368[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("compoundlit",
sizeof(char),12),_tag_arr(_tmp368,sizeof(void*),0));});_LL1DD: if(*((int*)_tmp1F0)
!= 36)goto _LL1DF;_tmp23E=((struct Cyc_Absyn_Codegen_e_struct*)_tmp1F0)->f1;_LL1DE:({
void*_tmp369[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("codegen",sizeof(char),8),_tag_arr(_tmp369,sizeof(void*),0));});_LL1DF:
if(*((int*)_tmp1F0)!= 37)goto _LL18C;_LL1E0:({void*_tmp36A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("fill",sizeof(char),
5),_tag_arr(_tmp36A,sizeof(void*),0));});_LL18C:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple11{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple12{struct _tuple1*f1;void*f2;};struct
_tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple11 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct
Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{void*_tmp36B=(void*)p->r;struct Cyc_Absyn_Vardecl*
_tmp36C;struct Cyc_Absyn_Vardecl _tmp36D;struct _tuple1*_tmp36E;struct Cyc_Absyn_Vardecl*
_tmp36F;void*_tmp370;int _tmp371;char _tmp372;struct _tagged_arr _tmp373;struct Cyc_Absyn_Enumdecl*
_tmp374;struct Cyc_Absyn_Enumfield*_tmp375;void*_tmp376;struct Cyc_Absyn_Enumfield*
_tmp377;struct Cyc_Absyn_Tuniondecl*_tmp378;struct Cyc_Absyn_Tunionfield*_tmp379;
struct Cyc_List_List*_tmp37A;struct Cyc_Absyn_Pat*_tmp37B;struct Cyc_Absyn_Pat
_tmp37C;void*_tmp37D;struct Cyc_Absyn_Tuniondecl*_tmp37E;struct Cyc_Absyn_Tunionfield*
_tmp37F;struct Cyc_List_List*_tmp380;struct Cyc_List_List*_tmp381;struct Cyc_List_List*
_tmp382;struct Cyc_Absyn_AggrInfo _tmp383;void*_tmp384;struct Cyc_List_List*_tmp385;
struct Cyc_Absyn_Pat*_tmp386;_LL286: if(_tmp36B <= (void*)2?1:*((int*)_tmp36B)!= 0)
goto _LL288;_tmp36C=((struct Cyc_Absyn_Var_p_struct*)_tmp36B)->f1;_tmp36D=*_tmp36C;
_tmp36E=_tmp36D.name;_LL287: nv=Cyc_Toc_add_varmap(nv,_tmp36E,r);goto _LL289;
_LL288: if((int)_tmp36B != 0)goto _LL28A;_LL289: s=Cyc_Toc_skip_stmt_dl();goto _LL285;
_LL28A: if(_tmp36B <= (void*)2?1:*((int*)_tmp36B)!= 6)goto _LL28C;_tmp36F=((struct
Cyc_Absyn_Reference_p_struct*)_tmp36B)->f1;_LL28B: {struct _tuple1*_tmp387=Cyc_Toc_temp_var();
decls=({struct Cyc_List_List*_tmp388=_region_malloc(rgn,sizeof(*_tmp388));_tmp388->hd=({
struct _tuple12*_tmp389=_region_malloc(rgn,sizeof(*_tmp389));_tmp389->f1=_tmp387;
_tmp389->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp389;});
_tmp388->tl=decls;_tmp388;});nv=Cyc_Toc_add_varmap(nv,_tmp36F->name,Cyc_Absyn_var_exp(
_tmp387,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp387,0),Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(
path,0),0),0);goto _LL285;}_LL28C: if((int)_tmp36B != 1)goto _LL28E;_LL28D: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL285;_LL28E: if(_tmp36B <= (void*)
2?1:*((int*)_tmp36B)!= 1)goto _LL290;_tmp370=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp36B)->f1;_tmp371=((struct Cyc_Absyn_Int_p_struct*)_tmp36B)->f2;_LL28F: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_int_exp(_tmp370,_tmp371,0),fail_stmt);goto _LL285;_LL290: if(_tmp36B <= (
void*)2?1:*((int*)_tmp36B)!= 2)goto _LL292;_tmp372=((struct Cyc_Absyn_Char_p_struct*)
_tmp36B)->f1;_LL291: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp372,0),
fail_stmt);goto _LL285;_LL292: if(_tmp36B <= (void*)2?1:*((int*)_tmp36B)!= 3)goto
_LL294;_tmp373=((struct Cyc_Absyn_Float_p_struct*)_tmp36B)->f1;_LL293: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_float_exp(_tmp373,0),fail_stmt);goto _LL285;_LL294: if(_tmp36B <= (void*)
2?1:*((int*)_tmp36B)!= 9)goto _LL296;_tmp374=((struct Cyc_Absyn_Enum_p_struct*)
_tmp36B)->f1;_tmp375=((struct Cyc_Absyn_Enum_p_struct*)_tmp36B)->f2;_LL295: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp38A=_cycalloc(
sizeof(*_tmp38A));_tmp38A[0]=({struct Cyc_Absyn_Enum_e_struct _tmp38B;_tmp38B.tag=
31;_tmp38B.f1=_tmp375->name;_tmp38B.f2=(struct Cyc_Absyn_Enumdecl*)_tmp374;
_tmp38B.f3=(struct Cyc_Absyn_Enumfield*)_tmp375;_tmp38B;});_tmp38A;}),0),
fail_stmt);goto _LL285;_LL296: if(_tmp36B <= (void*)2?1:*((int*)_tmp36B)!= 10)goto
_LL298;_tmp376=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp36B)->f1;_tmp377=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp36B)->f2;_LL297: s=Cyc_Toc_if_neq_stmt(r,
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp38C=_cycalloc(
sizeof(*_tmp38C));_tmp38C[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp38D;_tmp38D.tag=
32;_tmp38D.f1=_tmp377->name;_tmp38D.f2=(void*)_tmp376;_tmp38D.f3=(struct Cyc_Absyn_Enumfield*)
_tmp377;_tmp38D;});_tmp38C;}),0),fail_stmt);goto _LL285;_LL298: if(_tmp36B <= (void*)
2?1:*((int*)_tmp36B)!= 8)goto _LL29A;_tmp378=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp36B)->f1;_tmp379=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36B)->f2;_tmp37A=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp36B)->f3;if(_tmp37A != 0)goto _LL29A;_LL299: {
struct Cyc_Absyn_Exp*cmp_exp;if(_tmp378->is_xtunion)cmp_exp=Cyc_Absyn_var_exp(
_tmp379->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(_tmp378,_tmp379->name,0);r=Cyc_Absyn_cast_exp(
Cyc_Absyn_sint_t,r,0);}s=Cyc_Toc_if_neq_stmt(r,cmp_exp,fail_stmt);goto _LL285;}
_LL29A: if(_tmp36B <= (void*)2?1:*((int*)_tmp36B)!= 5)goto _LL29C;_tmp37B=((struct
Cyc_Absyn_Pointer_p_struct*)_tmp36B)->f1;_tmp37C=*_tmp37B;_tmp37D=(void*)_tmp37C.r;
if(_tmp37D <= (void*)2?1:*((int*)_tmp37D)!= 8)goto _LL29C;_tmp37E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp37D)->f1;_tmp37F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp37D)->f2;_tmp380=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp37D)->f3;if(!(_tmp380 != 0))goto _LL29C;
_LL29B: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(
_tmp37F->name,_tag_arr("_struct",sizeof(char),8));struct Cyc_Absyn_Exp*rcast=Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r,0);struct Cyc_List_List*
_tmp38E=_tmp37F->typs;for(0;_tmp380 != 0;(((_tmp380=_tmp380->tl,_tmp38E=((struct
Cyc_List_List*)_check_null(_tmp38E))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp38F=(
struct Cyc_Absyn_Pat*)_tmp380->hd;if((void*)_tmp38F->r == (void*)0)continue;{void*
_tmp390=(*((struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp38E))->hd)).f2;
struct _tuple1*_tmp391=Cyc_Toc_temp_var();void*_tmp392=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp38F->topt))->v;void*_tmp393=Cyc_Toc_typ_to_c(_tmp392);struct Cyc_Absyn_Exp*
_tmp394=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp390)))_tmp394=Cyc_Absyn_cast_exp(_tmp393,_tmp394,0);decls=({
struct Cyc_List_List*_tmp395=_region_malloc(rgn,sizeof(*_tmp395));_tmp395->hd=({
struct _tuple12*_tmp396=_region_malloc(rgn,sizeof(*_tmp396));_tmp396->f1=_tmp391;
_tmp396->f2=_tmp393;_tmp396;});_tmp395->tl=decls;_tmp395;});{struct _tuple11
_tmp397=Cyc_Toc_xlate_pat(nv,rgn,_tmp392,Cyc_Absyn_var_exp(_tmp391,0),_tmp394,
_tmp38F,fail_stmt,decls);nv=_tmp397.f1;decls=_tmp397.f2;{struct Cyc_Absyn_Stmt*
_tmp398=_tmp397.f3;struct Cyc_Absyn_Stmt*_tmp399=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp391,0),_tmp394,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp399,_tmp398,
0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(_tmp37E->is_xtunion){struct Cyc_Absyn_Exp*
e2=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),
r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp37F->name,0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);}else{int max_tag=Cyc_Toc_num_void_tags(
_tmp37E);struct Cyc_Absyn_Exp*max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)
max_tag,0);struct Cyc_Absyn_Exp*e3=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_t,
Cyc_Toc_mt_tq),r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*
e5=Cyc_Absyn_lte_exp(r,Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),max_tag_exp,
0),0);struct Cyc_Absyn_Exp*e=Cyc_Toc_tunion_tag(_tmp37E,_tmp37F->name,1);test_exp=
max_tag > 0?Cyc_Absyn_or_exp(e5,Cyc_Absyn_neq_exp(e1,e,0),0): Cyc_Absyn_neq_exp(e1,
e,0);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);goto _LL285;}}_LL29C: if(_tmp36B <= (void*)2?1:*((int*)_tmp36B)!= 8)goto
_LL29E;_tmp381=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36B)->f3;_LL29D: _tmp382=
_tmp381;goto _LL29F;_LL29E: if(_tmp36B <= (void*)2?1:*((int*)_tmp36B)!= 4)goto
_LL2A0;_tmp382=((struct Cyc_Absyn_Tuple_p_struct*)_tmp36B)->f1;_LL29F: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;for(0;_tmp382 != 0;(_tmp382=_tmp382->tl,++ cnt)){struct Cyc_Absyn_Pat*
_tmp39A=(struct Cyc_Absyn_Pat*)_tmp382->hd;if((void*)_tmp39A->r == (void*)0)
continue;{struct _tuple1*_tmp39B=Cyc_Toc_temp_var();void*_tmp39C=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp39A->topt))->v;decls=({struct Cyc_List_List*_tmp39D=
_region_malloc(rgn,sizeof(*_tmp39D));_tmp39D->hd=({struct _tuple12*_tmp39E=
_region_malloc(rgn,sizeof(*_tmp39E));_tmp39E->f1=_tmp39B;_tmp39E->f2=Cyc_Toc_typ_to_c(
_tmp39C);_tmp39E;});_tmp39D->tl=decls;_tmp39D;});{struct _tuple11 _tmp39F=Cyc_Toc_xlate_pat(
nv,rgn,_tmp39C,Cyc_Absyn_var_exp(_tmp39B,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp39A,fail_stmt,decls);nv=_tmp39F.f1;decls=_tmp39F.f2;{struct Cyc_Absyn_Stmt*
_tmp3A0=_tmp39F.f3;struct Cyc_Absyn_Stmt*_tmp3A1=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp39B,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp3A1,_tmp3A0,0),0);}}}}goto _LL285;}_LL2A0: if(_tmp36B <= (
void*)2?1:*((int*)_tmp36B)!= 7)goto _LL2A2;_tmp383=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp36B)->f1;_tmp384=(void*)_tmp383.aggr_info;_tmp385=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp36B)->f3;_LL2A1: {struct Cyc_Absyn_Aggrdecl*_tmp3A2=Cyc_Absyn_get_known_aggrdecl(
_tmp384);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp385 != 0;_tmp385=_tmp385->tl){struct
_tuple13*_tmp3A3=(struct _tuple13*)_tmp385->hd;struct Cyc_Absyn_Pat*_tmp3A4=(*
_tmp3A3).f2;if((void*)_tmp3A4->r == (void*)0)continue;{struct _tagged_arr*f;{void*
_tmp3A5=(void*)((struct Cyc_List_List*)_check_null((*_tmp3A3).f1))->hd;struct
_tagged_arr*_tmp3A6;_LL2A9: if(*((int*)_tmp3A5)!= 1)goto _LL2AB;_tmp3A6=((struct
Cyc_Absyn_FieldName_struct*)_tmp3A5)->f1;_LL2AA: f=_tmp3A6;goto _LL2A8;_LL2AB:;
_LL2AC:(int)_throw((void*)Cyc_Toc_Match_error);_LL2A8:;}{struct _tuple1*_tmp3A7=
Cyc_Toc_temp_var();void*_tmp3A8=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A4->topt))->v;
void*_tmp3A9=Cyc_Toc_typ_to_c(_tmp3A8);decls=({struct Cyc_List_List*_tmp3AA=
_region_malloc(rgn,sizeof(*_tmp3AA));_tmp3AA->hd=({struct _tuple12*_tmp3AB=
_region_malloc(rgn,sizeof(*_tmp3AB));_tmp3AB->f1=_tmp3A7;_tmp3AB->f2=_tmp3A9;
_tmp3AB;});_tmp3AA->tl=decls;_tmp3AA;});{struct _tuple11 _tmp3AC=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3A8,Cyc_Absyn_var_exp(_tmp3A7,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp3A4,fail_stmt,decls);nv=_tmp3AC.f1;decls=_tmp3AC.f2;{struct Cyc_Absyn_Exp*
_tmp3AD=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((void*)((struct
Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp3A2->fields))->v,f)))->type))_tmp3AD=Cyc_Absyn_cast_exp(
_tmp3A9,_tmp3AD,0);{struct Cyc_Absyn_Stmt*_tmp3AE=_tmp3AC.f3;struct Cyc_Absyn_Stmt*
_tmp3AF=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp3A7,0),_tmp3AD,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp3AF,_tmp3AE,0),0);}}}}}}goto _LL285;}_LL2A2: if(_tmp36B <= (
void*)2?1:*((int*)_tmp36B)!= 5)goto _LL2A4;_tmp386=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp36B)->f1;_LL2A3: {struct _tuple1*_tmp3B0=Cyc_Toc_temp_var();void*_tmp3B1=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp386->topt))->v;decls=({struct Cyc_List_List*
_tmp3B2=_region_malloc(rgn,sizeof(*_tmp3B2));_tmp3B2->hd=({struct _tuple12*
_tmp3B3=_region_malloc(rgn,sizeof(*_tmp3B3));_tmp3B3->f1=_tmp3B0;_tmp3B3->f2=Cyc_Toc_typ_to_c(
_tmp3B1);_tmp3B3;});_tmp3B2->tl=decls;_tmp3B2;});{struct _tuple11 _tmp3B4=Cyc_Toc_xlate_pat(
nv,rgn,_tmp3B1,Cyc_Absyn_var_exp(_tmp3B0,0),Cyc_Absyn_deref_exp(path,0),_tmp386,
fail_stmt,decls);nv=_tmp3B4.f1;decls=_tmp3B4.f2;{struct Cyc_Absyn_Stmt*_tmp3B5=
_tmp3B4.f3;struct Cyc_Absyn_Stmt*_tmp3B6=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp3B0,0),Cyc_Absyn_deref_exp(r,0),0),_tmp3B5,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp3B6,0);else{s=_tmp3B6;}goto
_LL285;}}}_LL2A4: if(_tmp36B <= (void*)2?1:*((int*)_tmp36B)!= 11)goto _LL2A6;_LL2A5:({
void*_tmp3B7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unknownid",sizeof(char),10),_tag_arr(_tmp3B7,sizeof(void*),0));});
_LL2A6: if(_tmp36B <= (void*)2?1:*((int*)_tmp36B)!= 12)goto _LL285;_LL2A7:({void*
_tmp3B8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("unknowncall",sizeof(char),12),_tag_arr(_tmp3B8,sizeof(void*),0));});
_LL285:;}return({struct _tuple11 _tmp3B9;_tmp3B9.f1=nv;_tmp3B9.f2=decls;_tmp3B9.f3=
s;_tmp3B9;});}struct _tuple14{struct _tagged_arr*f1;struct _tagged_arr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple14*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple14*_tmp3BA=_region_malloc(r,sizeof(*_tmp3BA));_tmp3BA->f1=
Cyc_Toc_fresh_label();_tmp3BA->f2=Cyc_Toc_fresh_label();_tmp3BA->f3=sc;_tmp3BA;});}
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*
_tmp3BB=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int leave_as_switch;{
void*_tmp3BC=Cyc_Tcutil_compress(_tmp3BB);_LL2AE: if(_tmp3BC <= (void*)3?1:*((int*)
_tmp3BC)!= 5)goto _LL2B0;_LL2AF: goto _LL2B1;_LL2B0: if(_tmp3BC <= (void*)3?1:*((int*)
_tmp3BC)!= 12)goto _LL2B2;_LL2B1: leave_as_switch=1;goto _LL2AD;_LL2B2:;_LL2B3:
leave_as_switch=0;goto _LL2AD;_LL2AD:;}{struct Cyc_List_List*_tmp3BD=scs;for(0;
_tmp3BD != 0;_tmp3BD=_tmp3BD->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp3BD->hd)->pat_vars))->v != 0?1:((
struct Cyc_Absyn_Switch_clause*)_tmp3BD->hd)->where_clause != 0){leave_as_switch=0;
break;}}}if(leave_as_switch){struct _tagged_arr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp3BE=scs;for(0;_tmp3BE != 0;_tmp3BE=_tmp3BE->tl){struct Cyc_Absyn_Stmt*
_tmp3BF=((struct Cyc_Absyn_Switch_clause*)_tmp3BE->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp3BE->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp3BF,0);next_l=Cyc_Toc_fresh_label();
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(nv,next_l),_tmp3BF);}}return;}{
struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct _tagged_arr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp3C0=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3C0;
_push_region(rgn);{struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple14*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,
rgn,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp3C1=lscs;for(0;_tmp3C1 != 0;_tmp3C1=_tmp3C1->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple14*)_tmp3C1->hd)).f3;struct
_tagged_arr*fail_lab=_tmp3C1->tl == 0?end_l:(*((struct _tuple14*)((struct Cyc_List_List*)
_check_null(_tmp3C1->tl))->hd)).f1;struct Cyc_Toc_Env*_tmp3C3;struct Cyc_List_List*
_tmp3C4;struct Cyc_Absyn_Stmt*_tmp3C5;struct _tuple11 _tmp3C2=Cyc_Toc_xlate_pat(nv,
rgn,_tmp3BB,r,path,sc->pattern,Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp3C3=
_tmp3C2.f1;_tmp3C4=_tmp3C2.f2;_tmp3C5=_tmp3C2.f3;if(sc->where_clause != 0){struct
Cyc_Absyn_Exp*_tmp3C6=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);Cyc_Toc_exp_to_c(
_tmp3C3,_tmp3C6);_tmp3C5=Cyc_Absyn_seq_stmt(_tmp3C5,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp3C6,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp3C4;nvs=({struct Cyc_List_List*_tmp3C7=_region_malloc(rgn,
sizeof(*_tmp3C7));_tmp3C7->hd=_tmp3C3;_tmp3C7->tl=nvs;_tmp3C7;});test_stmts=({
struct Cyc_List_List*_tmp3C8=_region_malloc(rgn,sizeof(*_tmp3C8));_tmp3C8->hd=
_tmp3C5;_tmp3C8->tl=test_stmts;_tmp3C8;});}}nvs=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple14 _tmp3CA;struct _tagged_arr*_tmp3CB;struct _tagged_arr*_tmp3CC;struct
Cyc_Absyn_Switch_clause*_tmp3CD;struct _tuple14*_tmp3C9=(struct _tuple14*)lscs->hd;
_tmp3CA=*_tmp3C9;_tmp3CB=_tmp3CA.f1;_tmp3CC=_tmp3CA.f2;_tmp3CD=_tmp3CA.f3;{
struct Cyc_Toc_Env*_tmp3CE=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp3CD->body;if(lscs->tl != 0){struct _tuple14
_tmp3D0;struct _tagged_arr*_tmp3D1;struct Cyc_Absyn_Switch_clause*_tmp3D2;struct
_tuple14*_tmp3CF=(struct _tuple14*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;
_tmp3D0=*_tmp3CF;_tmp3D1=_tmp3D0.f2;_tmp3D2=_tmp3D0.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp3CE,end_l,_tmp3D1,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp3D2->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp3CE,
end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp3CB,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp3CC,s,0),0);stmts=({struct Cyc_List_List*_tmp3D3=_region_malloc(rgn,sizeof(*
_tmp3D3));_tmp3D3->hd=s;_tmp3D3->tl=stmts;_tmp3D3;});}}{struct Cyc_Absyn_Stmt*res=
Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple12 _tmp3D5;struct _tuple1*
_tmp3D6;void*_tmp3D7;struct _tuple12*_tmp3D4=(struct _tuple12*)((struct Cyc_List_List*)
_check_null(decls))->hd;_tmp3D5=*_tmp3D4;_tmp3D6=_tmp3D5.f1;_tmp3D7=_tmp3D5.f2;
res=Cyc_Absyn_declare_stmt(_tmp3D6,_tmp3D7,0,res,0);}(void*)(whole_s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(
rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));
_tmp3D8->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp3D8->tl=0;_tmp3D8;}),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){if(n > 0)(void*)(s->r=(
void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp3D9=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3DA;
struct Cyc_Absyn_Stmt*_tmp3DB;struct Cyc_Absyn_Stmt*_tmp3DC;struct Cyc_Absyn_Exp*
_tmp3DD;struct Cyc_Absyn_Exp*_tmp3DE;struct Cyc_Absyn_Stmt*_tmp3DF;struct Cyc_Absyn_Stmt*
_tmp3E0;struct _tuple3 _tmp3E1;struct Cyc_Absyn_Exp*_tmp3E2;struct Cyc_Absyn_Stmt*
_tmp3E3;struct Cyc_Absyn_Stmt*_tmp3E4;struct Cyc_Absyn_Stmt*_tmp3E5;struct Cyc_Absyn_Stmt*
_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;struct _tuple3 _tmp3E8;struct Cyc_Absyn_Exp*
_tmp3E9;struct _tuple3 _tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;struct Cyc_Absyn_Stmt*
_tmp3EC;struct Cyc_Absyn_Exp*_tmp3ED;struct Cyc_List_List*_tmp3EE;struct Cyc_List_List*
_tmp3EF;struct Cyc_Absyn_Switch_clause**_tmp3F0;struct Cyc_Absyn_Decl*_tmp3F1;
struct Cyc_Absyn_Stmt*_tmp3F2;struct _tagged_arr*_tmp3F3;struct Cyc_Absyn_Stmt*
_tmp3F4;struct Cyc_Absyn_Stmt*_tmp3F5;struct _tuple3 _tmp3F6;struct Cyc_Absyn_Exp*
_tmp3F7;struct Cyc_Absyn_Stmt*_tmp3F8;struct Cyc_List_List*_tmp3F9;struct Cyc_Absyn_Tvar*
_tmp3FA;struct Cyc_Absyn_Vardecl*_tmp3FB;int _tmp3FC;struct Cyc_Absyn_Stmt*_tmp3FD;
struct Cyc_Absyn_Exp*_tmp3FE;_LL2B5: if((int)_tmp3D9 != 0)goto _LL2B7;_LL2B6: return;
_LL2B7: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 0)goto _LL2B9;_tmp3DA=((struct
Cyc_Absyn_Exp_s_struct*)_tmp3D9)->f1;_LL2B8: Cyc_Toc_exp_to_c(nv,_tmp3DA);return;
_LL2B9: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 1)goto _LL2BB;_tmp3DB=((struct
Cyc_Absyn_Seq_s_struct*)_tmp3D9)->f1;_tmp3DC=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3D9)->f2;_LL2BA: Cyc_Toc_stmt_to_c(nv,_tmp3DB);s=_tmp3DC;continue;_LL2BB: if(
_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 2)goto _LL2BD;_tmp3DD=((struct Cyc_Absyn_Return_s_struct*)
_tmp3D9)->f1;_LL2BC: {struct Cyc_Core_Opt*topt=0;if(_tmp3DD != 0){topt=({struct Cyc_Core_Opt*
_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3DD->topt))->v);_tmp3FF;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp3DD));}if(s->try_depth > 0){if(topt != 0){
struct _tuple1*_tmp400=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp401=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp400,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp400,(void*)topt->v,_tmp3DD,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp401,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL2BD: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 3)goto _LL2BF;_tmp3DE=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D9)->f1;_tmp3DF=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp3D9)->f2;_tmp3E0=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3D9)->f3;_LL2BE:
Cyc_Toc_exp_to_c(nv,_tmp3DE);Cyc_Toc_stmt_to_c(nv,_tmp3DF);s=_tmp3E0;continue;
_LL2BF: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 4)goto _LL2C1;_tmp3E1=((struct
Cyc_Absyn_While_s_struct*)_tmp3D9)->f1;_tmp3E2=_tmp3E1.f1;_tmp3E3=((struct Cyc_Absyn_While_s_struct*)
_tmp3D9)->f2;_LL2C0: Cyc_Toc_exp_to_c(nv,_tmp3E2);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp3E3);return;_LL2C1: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 5)goto
_LL2C3;_tmp3E4=((struct Cyc_Absyn_Break_s_struct*)_tmp3D9)->f1;_LL2C2: if(nv->break_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,0))->r));{int dest_depth=
_tmp3E4 == 0?0: _tmp3E4->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,
s);return;}_LL2C3: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 6)goto _LL2C5;
_tmp3E5=((struct Cyc_Absyn_Continue_s_struct*)_tmp3D9)->f1;_LL2C4: if(nv->continue_lab
!= 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null(nv->continue_lab))->v,0))->r));_tmp3E6=_tmp3E5;
goto _LL2C6;_LL2C5: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 7)goto _LL2C7;
_tmp3E6=((struct Cyc_Absyn_Goto_s_struct*)_tmp3D9)->f2;_LL2C6: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp3E6))->try_depth,s);
return;_LL2C7: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 8)goto _LL2C9;_tmp3E7=((
struct Cyc_Absyn_For_s_struct*)_tmp3D9)->f1;_tmp3E8=((struct Cyc_Absyn_For_s_struct*)
_tmp3D9)->f2;_tmp3E9=_tmp3E8.f1;_tmp3EA=((struct Cyc_Absyn_For_s_struct*)_tmp3D9)->f3;
_tmp3EB=_tmp3EA.f1;_tmp3EC=((struct Cyc_Absyn_For_s_struct*)_tmp3D9)->f4;_LL2C8:
Cyc_Toc_exp_to_c(nv,_tmp3E7);Cyc_Toc_exp_to_c(nv,_tmp3E9);Cyc_Toc_exp_to_c(nv,
_tmp3EB);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp3EC);return;_LL2C9: if(
_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 9)goto _LL2CB;_tmp3ED=((struct Cyc_Absyn_Switch_s_struct*)
_tmp3D9)->f1;_tmp3EE=((struct Cyc_Absyn_Switch_s_struct*)_tmp3D9)->f2;_LL2CA: Cyc_Toc_xlate_switch(
nv,s,_tmp3ED,_tmp3EE);return;_LL2CB: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 
11)goto _LL2CD;_tmp3EF=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp3D9)->f1;_tmp3F0=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp3D9)->f2;_LL2CC: if(nv->fallthru_info == 0)({
void*_tmp402[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("fallthru in unexpected place",sizeof(char),29),_tag_arr(_tmp402,
sizeof(void*),0));});{struct _tuple7 _tmp404;struct _tagged_arr*_tmp405;struct Cyc_List_List*
_tmp406;struct Cyc_Dict_Dict*_tmp407;struct _tuple7*_tmp403=(struct _tuple7*)((
struct Cyc_Core_Opt*)_check_null(nv->fallthru_info))->v;_tmp404=*_tmp403;_tmp405=
_tmp404.f1;_tmp406=_tmp404.f2;_tmp407=_tmp404.f3;{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(
_tmp405,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)
_check_null(_tmp3F0)))->body)->try_depth,s2);{struct Cyc_List_List*_tmp408=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp406);struct Cyc_List_List*
_tmp409=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp3EF);
for(0;_tmp408 != 0;(_tmp408=_tmp408->tl,_tmp409=_tmp409->tl)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp409))->hd);s2=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct
_tuple1*k))Cyc_Dict_lookup)(_tmp407,(struct _tuple1*)_tmp408->hd),(struct Cyc_Absyn_Exp*)
_tmp409->hd,0),s2,0);}(void*)(s->r=(void*)((void*)s2->r));return;}}}_LL2CD: if(
_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 12)goto _LL2CF;_tmp3F1=((struct Cyc_Absyn_Decl_s_struct*)
_tmp3D9)->f1;_tmp3F2=((struct Cyc_Absyn_Decl_s_struct*)_tmp3D9)->f2;_LL2CE:{void*
_tmp40A=(void*)_tmp3F1->r;struct Cyc_Absyn_Vardecl*_tmp40B;struct Cyc_Absyn_Pat*
_tmp40C;struct Cyc_Core_Opt*_tmp40D;struct Cyc_Absyn_Exp*_tmp40E;int _tmp40F;struct
Cyc_List_List*_tmp410;_LL2E2: if(*((int*)_tmp40A)!= 0)goto _LL2E4;_tmp40B=((struct
Cyc_Absyn_Var_d_struct*)_tmp40A)->f1;_LL2E3: {struct Cyc_Toc_Env*_tmp411=Cyc_Toc_add_varmap(
nv,_tmp40B->name,Cyc_Absyn_varb_exp(_tmp40B->name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412[0]=({struct Cyc_Absyn_Local_b_struct
_tmp413;_tmp413.tag=3;_tmp413.f1=_tmp40B;_tmp413;});_tmp412;}),0));Cyc_Toc_local_decl_to_c(
_tmp411,_tmp411,_tmp40B,_tmp3F2);goto _LL2E1;}_LL2E4: if(*((int*)_tmp40A)!= 2)goto
_LL2E6;_tmp40C=((struct Cyc_Absyn_Let_d_struct*)_tmp40A)->f1;_tmp40D=((struct Cyc_Absyn_Let_d_struct*)
_tmp40A)->f3;_tmp40E=((struct Cyc_Absyn_Let_d_struct*)_tmp40A)->f4;_tmp40F=((
struct Cyc_Absyn_Let_d_struct*)_tmp40A)->f5;_LL2E5:{void*_tmp414=(void*)_tmp40C->r;
struct Cyc_Absyn_Vardecl*_tmp415;_LL2EB: if(_tmp414 <= (void*)2?1:*((int*)_tmp414)
!= 0)goto _LL2ED;_tmp415=((struct Cyc_Absyn_Var_p_struct*)_tmp414)->f1;_LL2EC: {
struct _tuple1*old_name=_tmp415->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp415->name=new_name;_tmp415->initializer=(struct Cyc_Absyn_Exp*)_tmp40E;(void*)(
_tmp3F1->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp416=_cycalloc(
sizeof(*_tmp416));_tmp416[0]=({struct Cyc_Absyn_Var_d_struct _tmp417;_tmp417.tag=0;
_tmp417.f1=_tmp415;_tmp417;});_tmp416;})));{struct Cyc_Toc_Env*_tmp418=Cyc_Toc_add_varmap(
nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419[0]=({struct Cyc_Absyn_Local_b_struct
_tmp41A;_tmp41A.tag=3;_tmp41A.f1=_tmp415;_tmp41A;});_tmp419;}),0));Cyc_Toc_local_decl_to_c(
_tmp418,nv,_tmp415,_tmp3F2);goto _LL2EA;}}_LL2ED:;_LL2EE:(void*)(s->r=(void*)((
void*)(Cyc_Toc_letdecl_to_c(nv,_tmp40C,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp40D))->v,_tmp40E,_tmp3F2))->r));goto _LL2EA;_LL2EA:;}goto _LL2E1;_LL2E6: if(*((
int*)_tmp40A)!= 3)goto _LL2E8;_tmp410=((struct Cyc_Absyn_Letv_d_struct*)_tmp40A)->f1;
_LL2E7: {struct Cyc_List_List*_tmp41B=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp410);if(_tmp41B == 0)({void*_tmp41C[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("empty Letv_d",
sizeof(char),13),_tag_arr(_tmp41C,sizeof(void*),0));});(void*)(_tmp3F1->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[
0]=({struct Cyc_Absyn_Var_d_struct _tmp41E;_tmp41E.tag=0;_tmp41E.f1=(struct Cyc_Absyn_Vardecl*)
_tmp41B->hd;_tmp41E;});_tmp41D;})));_tmp41B=_tmp41B->tl;for(0;_tmp41B != 0;
_tmp41B=_tmp41B->tl){struct Cyc_Absyn_Decl*_tmp41F=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420[0]=({
struct Cyc_Absyn_Var_d_struct _tmp421;_tmp421.tag=0;_tmp421.f1=(struct Cyc_Absyn_Vardecl*)
_tmp41B->hd;_tmp421;});_tmp420;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp41F,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL2E1;}_LL2E8:;_LL2E9:({void*_tmp422[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad nested declaration within function",
sizeof(char),39),_tag_arr(_tmp422,sizeof(void*),0));});_LL2E1:;}return;_LL2CF:
if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 15)goto _LL2D1;_tmp3F3=((struct Cyc_Absyn_Label_s_struct*)
_tmp3D9)->f1;_tmp3F4=((struct Cyc_Absyn_Label_s_struct*)_tmp3D9)->f2;_LL2D0: s=
_tmp3F4;continue;_LL2D1: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 16)goto _LL2D3;
_tmp3F5=((struct Cyc_Absyn_Do_s_struct*)_tmp3D9)->f1;_tmp3F6=((struct Cyc_Absyn_Do_s_struct*)
_tmp3D9)->f2;_tmp3F7=_tmp3F6.f1;_LL2D2: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),
_tmp3F5);Cyc_Toc_exp_to_c(nv,_tmp3F7);return;_LL2D3: if(_tmp3D9 <= (void*)1?1:*((
int*)_tmp3D9)!= 17)goto _LL2D5;_tmp3F8=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp3D9)->f1;_tmp3F9=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp3D9)->f2;_LL2D4: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_t);e_exp->topt=({struct Cyc_Core_Opt*
_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->v=(void*)e_typ;_tmp423;});nv=Cyc_Toc_add_varmap(
nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp3F8);{struct Cyc_Absyn_Stmt*_tmp424=Cyc_Absyn_seq_stmt(
_tmp3F8,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple1*_tmp425=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp426=Cyc_Absyn_var_exp(
_tmp425,0);struct Cyc_Absyn_Vardecl*_tmp427=Cyc_Absyn_new_vardecl(_tmp425,Cyc_Absyn_exn_typ,
0);_tmp426->topt=({struct Cyc_Core_Opt*_tmp428=_cycalloc(sizeof(*_tmp428));
_tmp428->v=(void*)Cyc_Absyn_exn_typ;_tmp428;});{struct Cyc_Absyn_Pat*_tmp429=({
struct Cyc_Absyn_Pat*_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B[0]=({
struct Cyc_Absyn_Var_p_struct _tmp43C;_tmp43C.tag=0;_tmp43C.f1=_tmp427;_tmp43C;});
_tmp43B;}));_tmp439->topt=({struct Cyc_Core_Opt*_tmp43A=_cycalloc(sizeof(*_tmp43A));
_tmp43A->v=(void*)Cyc_Absyn_exn_typ;_tmp43A;});_tmp439->loc=0;_tmp439;});struct
Cyc_Absyn_Exp*_tmp42A=Cyc_Absyn_throw_exp(_tmp426,0);_tmp42A->topt=({struct Cyc_Core_Opt*
_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->v=(void*)((void*)0);_tmp42B;});{
struct Cyc_Absyn_Stmt*_tmp42C=Cyc_Absyn_exp_stmt(_tmp42A,0);struct Cyc_Absyn_Switch_clause*
_tmp42D=({struct Cyc_Absyn_Switch_clause*_tmp436=_cycalloc(sizeof(*_tmp436));
_tmp436->pattern=_tmp429;_tmp436->pat_vars=({struct Cyc_Core_Opt*_tmp437=
_cycalloc(sizeof(*_tmp437));_tmp437->v=({struct Cyc_List_List*_tmp438=_cycalloc(
sizeof(*_tmp438));_tmp438->hd=_tmp427;_tmp438->tl=0;_tmp438;});_tmp437;});
_tmp436->where_clause=0;_tmp436->body=_tmp42C;_tmp436->loc=0;_tmp436;});struct
Cyc_Absyn_Stmt*_tmp42E=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp3F9,({struct
Cyc_List_List*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->hd=_tmp42D;_tmp435->tl=
0;_tmp435;})),0);Cyc_Toc_stmt_to_c(nv,_tmp42E);{struct Cyc_Absyn_Exp*_tmp42F=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp434=_cycalloc(sizeof(*_tmp434));
_tmp434->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp434->tl=0;
_tmp434;}),0);struct Cyc_Absyn_Stmt*_tmp430=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp433=_cycalloc(sizeof(*_tmp433));
_tmp433->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp433->tl=0;_tmp433;}),0),0);struct
Cyc_Absyn_Exp*_tmp431=Cyc_Absyn_int_exp((void*)0,0,0);struct Cyc_Absyn_Exp*
_tmp432=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp430,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp431,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp42F,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp432,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp424,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_cast_exp(
e_typ,Cyc_Toc__exn_thrown_e,0),_tmp42E,0),0),0),0),0),0))->r));return;}}}}}
_LL2D5: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 18)goto _LL2D7;_tmp3FA=((struct
Cyc_Absyn_Region_s_struct*)_tmp3D9)->f1;_tmp3FB=((struct Cyc_Absyn_Region_s_struct*)
_tmp3D9)->f2;_tmp3FC=((struct Cyc_Absyn_Region_s_struct*)_tmp3D9)->f3;_tmp3FD=((
struct Cyc_Absyn_Region_s_struct*)_tmp3D9)->f4;_LL2D6: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp3FB->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
nv,x_var,x_exp),_tmp3FD);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp43D->tl=0;_tmp43D;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp43E[1];_tmp43E[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp43E,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),Cyc_Absyn_seq_stmt(_tmp3FD,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp*_tmp43F[1];_tmp43F[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp43F,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL2D7: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)
!= 20)goto _LL2D9;_tmp3FE=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp3D9)->f1;
_LL2D8: Cyc_Toc_exp_to_c(nv,_tmp3FE);(void*)(s->r=(void*)((void*)(Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp440=
_cycalloc(sizeof(*_tmp440));_tmp440->hd=_tmp3FE;_tmp440->tl=0;_tmp440;}),0),0))->r));
return;_LL2D9: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 10)goto _LL2DB;_LL2DA:({
void*_tmp441[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("switchC_s",sizeof(char),10),_tag_arr(_tmp441,sizeof(void*),0));});
_LL2DB: if(_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 13)goto _LL2DD;_LL2DC:({void*
_tmp442[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("cut",sizeof(char),4),_tag_arr(_tmp442,sizeof(void*),0));});_LL2DD: if(
_tmp3D9 <= (void*)1?1:*((int*)_tmp3D9)!= 14)goto _LL2DF;_LL2DE:({void*_tmp443[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("splice",
sizeof(char),7),_tag_arr(_tmp443,sizeof(void*),0));});_LL2DF: if(_tmp3D9 <= (void*)
1?1:*((int*)_tmp3D9)!= 19)goto _LL2B4;_LL2E0:({void*_tmp444[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("forarray",sizeof(
char),9),_tag_arr(_tmp444,sizeof(void*),0));});_LL2B4:;}}struct _tuple15{struct
_tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f){f->tvs=0;f->effect=0;f->rgn_po=0;(
void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{struct Cyc_List_List*
_tmp445=f->args;for(0;_tmp445 != 0;_tmp445=_tmp445->tl){(*((struct _tuple15*)
_tmp445->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple15*)_tmp445->hd)).f3);{struct
_tuple1*_tmp446=({struct _tuple1*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->f1=(
void*)0;_tmp447->f2=(*((struct _tuple15*)_tmp445->hd)).f1;_tmp447;});nv=Cyc_Toc_add_varmap(
nv,_tmp446,Cyc_Absyn_var_exp(_tmp446,0));}}}if((unsigned int)f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0: 0){struct Cyc_Core_Opt*
_tmp449;struct Cyc_Absyn_Tqual _tmp44A;void*_tmp44B;int _tmp44C;struct Cyc_Absyn_VarargInfo
_tmp448=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp449=
_tmp448.name;_tmp44A=_tmp448.tq;_tmp44B=(void*)_tmp448.type;_tmp44C=_tmp448.inject;{
void*_tmp44D=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ(_tmp44B,(void*)2,_tmp44A));
struct _tuple1*_tmp44E=({struct _tuple1*_tmp451=_cycalloc(sizeof(*_tmp451));
_tmp451->f1=(void*)0;_tmp451->f2=(struct _tagged_arr*)((struct Cyc_Core_Opt*)
_check_null(_tmp449))->v;_tmp451;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp44F=
_cycalloc(sizeof(*_tmp44F));_tmp44F->hd=({struct _tuple15*_tmp450=_cycalloc(
sizeof(*_tmp450));_tmp450->f1=(struct _tagged_arr*)_tmp449->v;_tmp450->f2=_tmp44A;
_tmp450->f3=_tmp44D;_tmp450;});_tmp44F->tl=0;_tmp44F;}));nv=Cyc_Toc_add_varmap(
nv,_tmp44E,Cyc_Absyn_var_exp(_tmp44E,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp452=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp452 != 0;_tmp452=_tmp452->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp452->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp452->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(nv),f->body);}
static void*Cyc_Toc_scope_to_c(void*s){void*_tmp453=s;_LL2F0: if((int)_tmp453 != 1)
goto _LL2F2;_LL2F1: return(void*)2;_LL2F2: if((int)_tmp453 != 4)goto _LL2F4;_LL2F3:
return(void*)3;_LL2F4:;_LL2F5: return s;_LL2EF:;}struct Cyc_Core_Opt*Cyc_Toc_aggrs_so_far=
0;static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*
_tmp454=ad->name;if(Cyc_Toc_aggrs_so_far == 0)Cyc_Toc_aggrs_so_far=({struct Cyc_Core_Opt*
_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->v=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);
_tmp455;});{int seen_defn_before;struct Cyc_Core_Opt*_tmp456=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*_tmp454).f2);if(
_tmp456 == 0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*_tmp454).f2,ad));}
else{if(((struct Cyc_Absyn_Aggrdecl*)_tmp456->v)->fields == 0){(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_aggrs_so_far))->v,(*
_tmp454).f2,ad));seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(
void*)((void*)2));ad->tvs=0;ad->exist_vars=0;if(ad->fields != 0){if(
seen_defn_before)ad->fields=0;else{struct Cyc_List_List*_tmp457=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ad->fields))->v;for(0;_tmp457 != 0;_tmp457=
_tmp457->tl){(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp457->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp457->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0)Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);_tmp458;});{
struct _tuple1*_tmp459=tud->name;if(tud->fields == 0?1:((int(*)(struct Cyc_Set_Set*
s,struct _tuple1*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_tunions_so_far))->v,_tmp459))return;(struct Cyc_Set_Set*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(void*)((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_insert)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp459));{struct Cyc_List_List*
_tmp45A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;
for(0;_tmp45A != 0;_tmp45A=_tmp45A->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp45A->hd;if(f->typs != 0){struct Cyc_List_List*_tmp45B=0;int i=1;{struct Cyc_List_List*
_tmp45C=f->typs;for(0;_tmp45C != 0;(_tmp45C=_tmp45C->tl,i ++)){struct _tagged_arr*
_tmp45D=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*_tmp45E=({struct Cyc_Absyn_Aggrfield*
_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->name=_tmp45D;_tmp460->tq=(*((struct
_tuple4*)_tmp45C->hd)).f1;_tmp460->type=(void*)Cyc_Toc_typ_to_c_array((*((struct
_tuple4*)_tmp45C->hd)).f2);_tmp460->width=0;_tmp460->attributes=0;_tmp460;});
_tmp45B=({struct Cyc_List_List*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->hd=
_tmp45E;_tmp45F->tl=_tmp45B;_tmp45F;});}}_tmp45B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp45B);_tmp45B=({struct Cyc_List_List*
_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461->hd=({struct Cyc_Absyn_Aggrfield*
_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->name=Cyc_Toc_tag_sp;_tmp462->tq=Cyc_Toc_mt_tq;
_tmp462->type=(void*)Cyc_Absyn_sint_t;_tmp462->width=0;_tmp462->attributes=0;
_tmp462;});_tmp461->tl=_tmp45B;_tmp461;});{struct Cyc_Absyn_Aggrdecl*_tmp463=({
struct Cyc_Absyn_Aggrdecl*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->kind=(void*)((
void*)0);_tmp467->sc=(void*)((void*)2);_tmp467->name=Cyc_Toc_collapse_qvar_tag(f->name,
_tag_arr("_struct",sizeof(char),8));_tmp467->tvs=0;_tmp467->exist_vars=0;_tmp467->fields=({
struct Cyc_Core_Opt*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->v=_tmp45B;
_tmp468;});_tmp467->attributes=0;_tmp467;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp466;_tmp466.tag=4;_tmp466.f1=_tmp463;_tmp466;});
_tmp465;}),0);_tmp464->tl=Cyc_Toc_result_decls;_tmp464;});}}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
xd){if(Cyc_Toc_xtunion_fields_so_far == 0)Cyc_Toc_xtunion_fields_so_far=({struct
Cyc_Core_Opt*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469->v=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp469;});if(xd->fields == 0)return;{struct _tuple1*_tmp46A=xd->name;struct Cyc_List_List*
_tmp46B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp46B != 0;_tmp46B=_tmp46B->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp46B->hd;struct _tagged_arr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp46C=Cyc_Absyn_uint_exp(
_get_arr_size(*fn,sizeof(char))+ 4,0);struct Cyc_Absyn_ArrayType_struct*_tmp46D=({
struct Cyc_Absyn_ArrayType_struct*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp497;_tmp497.tag=7;_tmp497.f1=(void*)Cyc_Absyn_uchar_t;
_tmp497.f2=Cyc_Toc_mt_tq;_tmp497.f3=(struct Cyc_Absyn_Exp*)_tmp46C;_tmp497;});
_tmp496;});struct Cyc_Core_Opt*_tmp46E=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
d,struct _tuple1*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_xtunion_fields_so_far))->v,f->name);struct Cyc_Core_Opt
_tmp46F;int _tmp470;_LL2F7: if(_tmp46E != 0)goto _LL2F9;_LL2F8: {struct Cyc_Absyn_Exp*
initopt=0;if((void*)f->sc != (void*)3)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp476;_tmp476.tag=0;_tmp476.f1=(
struct _tagged_arr)*fn;{struct Cyc_Std_Int_pa_struct _tmp475;_tmp475.tag=1;_tmp475.f1=(
int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct _tmp474;_tmp474.tag=
1;_tmp474.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp473;_tmp473.tag=1;_tmp473.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp472;_tmp472.tag=1;_tmp472.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp471[5]={& _tmp472,& _tmp473,& _tmp474,& _tmp475,& _tmp476};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(char),11),_tag_arr(_tmp471,sizeof(void*),5));}}}}}}),
0);{struct Cyc_Absyn_Vardecl*_tmp477=Cyc_Absyn_new_vardecl(f->name,(void*)_tmp46D,
initopt);(void*)(_tmp477->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479[
0]=({struct Cyc_Absyn_Var_d_struct _tmp47A;_tmp47A.tag=0;_tmp47A.f1=_tmp477;
_tmp47A;});_tmp479;}),0);_tmp478->tl=Cyc_Toc_result_decls;_tmp478;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)3));if(f->typs != 0){struct Cyc_List_List*fields=0;
int i=1;{struct Cyc_List_List*_tmp47B=f->typs;for(0;_tmp47B != 0;(_tmp47B=_tmp47B->tl,
i ++)){struct _tagged_arr*_tmp47C=({struct _tagged_arr*_tmp480=_cycalloc(sizeof(*
_tmp480));_tmp480[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp482;
_tmp482.tag=1;_tmp482.f1=(int)((unsigned int)i);{void*_tmp481[1]={& _tmp482};Cyc_Std_aprintf(
_tag_arr("f%d",sizeof(char),4),_tag_arr(_tmp481,sizeof(void*),1));}});_tmp480;});
struct Cyc_Absyn_Aggrfield*_tmp47D=({struct Cyc_Absyn_Aggrfield*_tmp47F=_cycalloc(
sizeof(*_tmp47F));_tmp47F->name=_tmp47C;_tmp47F->tq=(*((struct _tuple4*)_tmp47B->hd)).f1;
_tmp47F->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)_tmp47B->hd)).f2);
_tmp47F->width=0;_tmp47F->attributes=0;_tmp47F;});fields=({struct Cyc_List_List*
_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E->hd=_tmp47D;_tmp47E->tl=fields;
_tmp47E;});}}fields=({struct Cyc_List_List*_tmp483=_cycalloc(sizeof(*_tmp483));
_tmp483->hd=({struct Cyc_Absyn_Aggrfield*_tmp484=_cycalloc(sizeof(*_tmp484));
_tmp484->name=Cyc_Toc_tag_sp;_tmp484->tq=Cyc_Toc_mt_tq;_tmp484->type=(void*)Cyc_Absyn_cstar_typ(
Cyc_Absyn_uchar_t,Cyc_Toc_mt_tq);_tmp484->width=0;_tmp484->attributes=0;_tmp484;});
_tmp483->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp483;});{struct Cyc_Absyn_Aggrdecl*_tmp485=({struct Cyc_Absyn_Aggrdecl*
_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->kind=(void*)((void*)0);_tmp489->sc=(
void*)((void*)2);_tmp489->name=Cyc_Toc_collapse_qvar_tag(f->name,_tag_arr("_struct",
sizeof(char),8));_tmp489->tvs=0;_tmp489->exist_vars=0;_tmp489->fields=({struct
Cyc_Core_Opt*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A->v=fields;_tmp48A;});
_tmp489->attributes=0;_tmp489;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp488;_tmp488.tag=4;_tmp488.f1=_tmp485;_tmp488;});
_tmp487;}),0);_tmp486->tl=Cyc_Toc_result_decls;_tmp486;});}}goto _LL2F6;}}_LL2F9:
if(_tmp46E == 0)goto _LL2FB;_tmp46F=*_tmp46E;_tmp470=(int)_tmp46F.v;if(_tmp470 != 0)
goto _LL2FB;_LL2FA: if((void*)f->sc != (void*)3){struct Cyc_Absyn_Exp*_tmp48B=Cyc_Absyn_string_exp((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp495;_tmp495.tag=0;_tmp495.f1=(
struct _tagged_arr)*fn;{struct Cyc_Std_Int_pa_struct _tmp494;_tmp494.tag=1;_tmp494.f1=(
int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct _tmp493;_tmp493.tag=
1;_tmp493.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp492;_tmp492.tag=1;_tmp492.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp491;_tmp491.tag=1;_tmp491.f1=(int)((unsigned int)((int)'\000'));{void*
_tmp490[5]={& _tmp491,& _tmp492,& _tmp493,& _tmp494,& _tmp495};Cyc_Std_aprintf(
_tag_arr("%c%c%c%c%s",sizeof(char),11),_tag_arr(_tmp490,sizeof(void*),5));}}}}}}),
0);struct Cyc_Absyn_Vardecl*_tmp48C=Cyc_Absyn_new_vardecl(f->name,(void*)_tmp46D,(
struct Cyc_Absyn_Exp*)_tmp48B);(void*)(_tmp48C->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E[
0]=({struct Cyc_Absyn_Var_d_struct _tmp48F;_tmp48F.tag=0;_tmp48F.f1=_tmp48C;
_tmp48F;});_tmp48E;}),0);_tmp48D->tl=Cyc_Toc_result_decls;_tmp48D;});(struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(void*)((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple1*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL2F6;_LL2FB:;_LL2FC: goto _LL2F6;_LL2F6:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)
2));if(ed->fields != 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array((void*)vd->type));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp498=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;_LL2FE: if(*((int*)
_tmp498)!= 27)goto _LL300;_tmp499=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp498)->f1;_tmp49A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp498)->f2;
_tmp49B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp498)->f3;_LL2FF: vd->initializer=
0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(
vd->name,0),_tmp499,_tmp49A,_tmp49B,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL2FD;_LL300:;_LL301: if((void*)vd->sc == (void*)0){struct Cyc_Toc_Env*_tmp49C=({
struct Cyc_Toc_Env*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D[0]=*init_nv;
_tmp49D;});_tmp49C->toplevel=1;Cyc_Toc_exp_to_c(_tmp49C,init);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL2FD;_LL2FD:;}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=
0;static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp49E=_cycalloc(
sizeof(*_tmp49E));_tmp49E[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp49E;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp49F=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp49F;_push_region(prgn);{struct Cyc_Toc_Env*_tmp4A1;
struct Cyc_List_List*_tmp4A2;struct Cyc_Absyn_Stmt*_tmp4A3;struct _tuple11 _tmp4A0=
Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp4A1=_tmp4A0.f1;_tmp4A2=_tmp4A0.f2;_tmp4A3=_tmp4A0.f3;Cyc_Toc_stmt_to_c(
_tmp4A1,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp4A3,s,0),0);for(0;_tmp4A2 != 0;_tmp4A2=_tmp4A2->tl){
struct _tuple12 _tmp4A5;struct _tuple1*_tmp4A6;void*_tmp4A7;struct _tuple12*_tmp4A4=(
struct _tuple12*)_tmp4A2->hd;_tmp4A5=*_tmp4A4;_tmp4A6=_tmp4A5.f1;_tmp4A7=_tmp4A5.f2;
s=Cyc_Absyn_declare_stmt(_tmp4A6,_tmp4A7,0,s,0);}};_pop_region(prgn);}return s;}}
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top){for(0;ds != 0;ds=ds->tl){if(!nv->toplevel)({void*_tmp4A8[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("decls_to_c: not at toplevel!",
sizeof(char),29),_tag_arr(_tmp4A8,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(
struct Cyc_Absyn_Decl*)ds->hd;void*_tmp4A9=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp4AA;struct Cyc_Absyn_Fndecl*_tmp4AB;struct Cyc_Absyn_Aggrdecl*_tmp4AC;struct
Cyc_Absyn_Tuniondecl*_tmp4AD;struct Cyc_Absyn_Enumdecl*_tmp4AE;struct Cyc_Absyn_Typedefdecl*
_tmp4AF;struct Cyc_List_List*_tmp4B0;struct Cyc_List_List*_tmp4B1;struct Cyc_List_List*
_tmp4B2;_LL303: if(*((int*)_tmp4A9)!= 0)goto _LL305;_tmp4AA=((struct Cyc_Absyn_Var_d_struct*)
_tmp4A9)->f1;_LL304: {struct _tuple1*_tmp4B3=_tmp4AA->name;if((void*)_tmp4AA->sc
== (void*)4)_tmp4B3=({struct _tuple1*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4->f1=
Cyc_Absyn_rel_ns_null;_tmp4B4->f2=(*_tmp4B3).f2;_tmp4B4;});if(_tmp4AA->initializer
!= 0)Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp4AA->initializer));
nv=Cyc_Toc_add_varmap(nv,_tmp4AA->name,Cyc_Absyn_varb_exp(_tmp4B3,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5[0]=({
struct Cyc_Absyn_Global_b_struct _tmp4B6;_tmp4B6.tag=0;_tmp4B6.f1=_tmp4AA;_tmp4B6;});
_tmp4B5;}),0));_tmp4AA->name=_tmp4B3;(void*)(_tmp4AA->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp4AA->sc));(void*)(_tmp4AA->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp4AA->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4B7=_cycalloc(
sizeof(*_tmp4B7));_tmp4B7->hd=d;_tmp4B7->tl=Cyc_Toc_result_decls;_tmp4B7;});goto
_LL302;}_LL305: if(*((int*)_tmp4A9)!= 1)goto _LL307;_tmp4AB=((struct Cyc_Absyn_Fn_d_struct*)
_tmp4A9)->f1;_LL306: {struct _tuple1*_tmp4B8=_tmp4AB->name;if((void*)_tmp4AB->sc
== (void*)4)_tmp4B8=({struct _tuple1*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->f1=
Cyc_Absyn_rel_ns_null;_tmp4B9->f2=(*_tmp4B8).f2;_tmp4B9;});nv=Cyc_Toc_add_varmap(
nv,_tmp4AB->name,Cyc_Absyn_var_exp(_tmp4B8,0));_tmp4AB->name=_tmp4B8;Cyc_Toc_fndecl_to_c(
nv,_tmp4AB);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4BA=_cycalloc(sizeof(*
_tmp4BA));_tmp4BA->hd=d;_tmp4BA->tl=Cyc_Toc_result_decls;_tmp4BA;});goto _LL302;}
_LL307: if(*((int*)_tmp4A9)!= 2)goto _LL309;_LL308: goto _LL30A;_LL309: if(*((int*)
_tmp4A9)!= 3)goto _LL30B;_LL30A:({void*_tmp4BB[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("letdecl at toplevel",
sizeof(char),20),_tag_arr(_tmp4BB,sizeof(void*),0));});_LL30B: if(*((int*)_tmp4A9)
!= 4)goto _LL30D;_tmp4AC=((struct Cyc_Absyn_Aggr_d_struct*)_tmp4A9)->f1;_LL30C: Cyc_Toc_aggrdecl_to_c(
_tmp4AC);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4BC=_cycalloc(sizeof(*
_tmp4BC));_tmp4BC->hd=d;_tmp4BC->tl=Cyc_Toc_result_decls;_tmp4BC;});goto _LL302;
_LL30D: if(*((int*)_tmp4A9)!= 5)goto _LL30F;_tmp4AD=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp4A9)->f1;_LL30E: if(_tmp4AD->is_xtunion)Cyc_Toc_xtuniondecl_to_c(_tmp4AD);
else{Cyc_Toc_tuniondecl_to_c(_tmp4AD);}goto _LL302;_LL30F: if(*((int*)_tmp4A9)!= 6)
goto _LL311;_tmp4AE=((struct Cyc_Absyn_Enum_d_struct*)_tmp4A9)->f1;_LL310: Cyc_Toc_enumdecl_to_c(
nv,_tmp4AE);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp4BD=_cycalloc(sizeof(*
_tmp4BD));_tmp4BD->hd=d;_tmp4BD->tl=Cyc_Toc_result_decls;_tmp4BD;});goto _LL302;
_LL311: if(*((int*)_tmp4A9)!= 7)goto _LL313;_tmp4AF=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp4A9)->f1;_LL312: _tmp4AF->name=_tmp4AF->name;_tmp4AF->tvs=0;if(_tmp4AF->defn
!= 0)_tmp4AF->defn=({struct Cyc_Core_Opt*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));
_tmp4BE->v=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4AF->defn))->v);_tmp4BE;});else{void*_tmp4BF=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4AF->kind))->v;_LL31A: if((int)_tmp4BF != 2)goto _LL31C;_LL31B:
_tmp4AF->defn=({struct Cyc_Core_Opt*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->v=(
void*)Cyc_Absyn_void_star_typ();_tmp4C0;});goto _LL319;_LL31C:;_LL31D: _tmp4AF->defn=({
struct Cyc_Core_Opt*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1->v=(void*)((void*)
0);_tmp4C1;});goto _LL319;_LL319:;}Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp4C2=_cycalloc(sizeof(*_tmp4C2));_tmp4C2->hd=d;_tmp4C2->tl=Cyc_Toc_result_decls;
_tmp4C2;});goto _LL302;_LL313: if(*((int*)_tmp4A9)!= 8)goto _LL315;_tmp4B0=((struct
Cyc_Absyn_Namespace_d_struct*)_tmp4A9)->f2;_LL314: _tmp4B1=_tmp4B0;goto _LL316;
_LL315: if(*((int*)_tmp4A9)!= 9)goto _LL317;_tmp4B1=((struct Cyc_Absyn_Using_d_struct*)
_tmp4A9)->f2;_LL316: _tmp4B2=_tmp4B1;goto _LL318;_LL317: if(*((int*)_tmp4A9)!= 10)
goto _LL302;_tmp4B2=((struct Cyc_Absyn_ExternC_d_struct*)_tmp4A9)->f1;_LL318: nv=
Cyc_Toc_decls_to_c(nv,_tmp4B2,top);goto _LL302;_LL302:;}}return nv;}static void Cyc_Toc_init(){
Cyc_Toc_result_decls=0;Cyc_Toc_tuple_types=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=
0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_aggrs_so_far=0;Cyc_Toc_tunions_so_far=0;
Cyc_Toc_xtunion_fields_so_far=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(),ds,1);return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
